#include <systemc.h>
#include "registers.hpp"
#include "types.hpp"

class CPU;
struct Instruction;
using InstructionHandler = void(CPU::*)(const Instruction&);

enum AddressingMode : uint8_t {
  Implied,
  Immediate,
  ZeroPage,
  Absolute
};

enum OPCodes : opcode_t {
  OP_BRK     = 0x00, // brk
  OP_JMP_ABS = 0x4C, // jmp <addr>
  OP_STA_ZPG = 0x85, // sta $addr
  OP_LDA_ZPG = 0xA5, // lda $addr
  OP_LDA_IMM = 0xA9, // lda #imm
  OP_NOP     = 0xEA, // nop
};

struct Instruction {
  std::string name;
  InstructionHandler handler;
  AddressingMode mode;
};

class CPU : public sc_module {
  public:
  struct {
    sc_in<bool> clock;
    sc_in<mem_data_t> read_data;
  } in;

  struct {
    sc_out<bool> req;
    sc_out<bool> write_flag;
    sc_out<mem_addr_t> address;
    sc_out<mem_data_t> write_data;
  } out;

  CPU(sc_module_name name) : sc_module(name) {
    SC_THREAD(execute);
    sensitive << in.clock.pos();
  }

  bool is_halted() { return halted; };
  Registers copy_registers() { return registers; };
  uint64_t get_cycle_count() { return cycle_count; };
  void set_logging(bool log) { logging = log; };

  private:
  bool halted = false;
  bool logging = false;
  Registers registers;
  uint64_t cycle_count = 0;

  const std::unordered_map<opcode_t, Instruction> opcode_handlers = {
    { OP_BRK,     { "brk", &CPU::brk, AddressingMode::Implied } },
    { OP_JMP_ABS, { "jmp", &CPU::jmp, AddressingMode::Absolute } },
    { OP_STA_ZPG, { "sta", &CPU::sta, AddressingMode::ZeroPage } },
    { OP_LDA_IMM, { "lda", &CPU::lda, AddressingMode::Immediate } },
    { OP_LDA_ZPG, { "lda", &CPU::lda, AddressingMode::ZeroPage } },
    { OP_NOP,     { "nop", &CPU::nop, AddressingMode::Implied } },
  };

  // Just a wrapper around wait to count cpu cycles
  void wait() {
    ++cycle_count;
    ::wait();
  }

  // A full handshake transaction with the memory
  mem_data_t memory_transaction() {
    out.req = true;

    // Wait for memory to handle the request.
    // On the 6502 this is guaranteed to happen in 1 cycle.
    wait();
    mem_data_t data = in.read_data;
    out.req = false;

    return data;
  }

  // Read one byte of memory and dont progress the pc
  mem_data_t read_from_memory(mem_addr_t source_address) {
    out.address = source_address;
    out.write_flag = false;
    return memory_transaction();
  }

  // Read one byte of memory and dont progress the pc
  mem_data_t write_to_memory(mem_addr_t address, mem_data_t data) {
    out.address = address;
    out.write_data = data;
    out.write_flag = true;
    return memory_transaction();
  }

  // Fetch a byte segment from memory and increment the pc
  template <typename T>
  T fetch() {
    static_assert(std::is_trivially_copyable_v<T>,
                  "Template type passed to fetch must be trivially copyable.");

    // Little-endian byte loading
    mem_data_t buffer[sizeof(T)];
    for (size_t i = 0; i < sizeof(T); i++) {
      buffer[i] = read_from_memory(registers.pc++);
    }
    T result;
    std::memcpy(&result, buffer, sizeof(T));
    return result;
  }

  mem_addr_t resolve_address(const Instruction& instruction) {
    switch(instruction.mode) {
      case AddressingMode::ZeroPage:
        return fetch<mem_addr_zp_t>();
      case AddressingMode::Absolute:
        return fetch<mem_addr_t>();
      default:
        return 0;  // or error
    }
  }

  // Store the accumulator in memory
  void sta(const Instruction& instruction) {
    mem_addr_t destination = resolve_address(instruction);
    write_to_memory(destination, registers.A);

    if (logging) {
      std::cout << sc_time_stamp() << ": " << instruction.name
                << " " << (int)destination << ", " << (int)registers.A
                << std::endl;
    }
  }

  // Print a byte stored in memory
  void lda(const Instruction& instruction) {
    if (instruction.mode == AddressingMode::ZeroPage) {
      mem_addr_t source_address = resolve_address(instruction);
      registers.A = read_from_memory(source_address);

      if (logging) {
        std::cout << sc_time_stamp() << ": " << instruction.name
                  << " "  << (int)source_address << " -> "
                  << (int)registers.A << std::endl;
      }
    } else if (instruction.mode == AddressingMode::Immediate) {
      registers.A = fetch<mem_data_t>();
      if (logging) {
        std::cout << sc_time_stamp() << ": " << instruction.name
                  << " #" << (int)registers.A << std::endl;
      }
    }
  }

  // Perform a jump of the pc
  void jmp(const Instruction& instruction) {
    registers.pc = resolve_address(instruction);

    if (logging) {
      std::cout << sc_time_stamp() << ": jmp " << (int)registers.pc
                << std::endl;
    }
  }

  // Halt the CPU
  void brk(const Instruction& instruction) {
    // brk actually takes 7 instructions, which we model later
    for (int i = 0; i < 6; i++) wait();
    halted = true;

    if (logging) {
      std::cout << sc_time_stamp() << ": brk" << std::endl;
    }
  }

  // Do nothing, just wait
  void nop(const Instruction& instruction) {
    wait();

    if (logging) {
      std::cout << sc_time_stamp() << ": nop" << std::endl;
    }
  }

  // Core loop of the CPU
  void execute() {
    while (!halted) {
      wait();
      uint64_t cycles_start = cycle_count;
      opcode_t opcode = static_cast<OPCodes>(fetch<opcode_t>());
      auto function_map_it = opcode_handlers.find(opcode);

      if (function_map_it != opcode_handlers.end()) {
        // Call the OPCode handler function
        (this->*(function_map_it->second.handler))(function_map_it->second);
        uint64_t cycles_end = cycle_count;

        if (logging) {
          std::cout << function_map_it->second.name << " took "
                    << cycles_end - cycles_start << " cycles"
                    << std::endl << std::endl;
        }
      }
      else {
          std::cout << sc_time_stamp() << ": Unknown opcode " << (int)opcode << std::endl;
          halted = true;
      }
    };
  }
};
