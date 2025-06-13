#include <systemc.h>
#include "registers.hpp"

enum OPCodes : uint8_t {
  OP_NOP,       // nop
  OP_STR,       // str <addr> <value>
  OP_DSP,       // dsp <addr>
  OP_JMP,       // jmp <addr>
  OP_STP = 0xff // stp
};

class CPU : public sc_module {
  public:
  struct {
    sc_in<bool> clock, ack;
    sc_in<uint8_t> read_data;
  } in;

  struct {
    sc_out<bool> req;
    sc_out<bool> write_flag;
    sc_out<uint8_t> address;
    sc_out<uint8_t> write_data;
  } out;

  CPU(sc_module_name name) : sc_module(name) {
    SC_THREAD(execute);
    sensitive << in.clock.pos();
  }

  private:
  using Handler = void(CPU::*)();
  const std::unordered_map<OPCodes, Handler> opcode_handlers = {
    { OP_NOP, &CPU::nop },
    { OP_STR, &CPU::str },
    { OP_DSP, &CPU::dsp },
    { OP_JMP, &CPU::jmp },
    { OP_STP, &CPU::stp }
  };

  bool halted = false;
  uint8_t pc = 0;

  // A full handshake transaction with the memory
  uint8_t memory_transaction() {
    // If ack or req is high, some other operation is ongoing.
    while (in.ack || out.req) wait();
    out.req = true;

    // Wait for memory to handle the request
    while(!in.ack) wait();
    uint8_t data = in.read_data;
    out.req = false;

    // Wait for memory to reset ack to low
    while(in.ack) wait();
    return data;
  }

  // Read one byte of memory and dont progress the pc
  uint8_t read_from_memory(uint8_t source_address) {
    out.address = source_address;
    out.write_flag = false;
    return memory_transaction();
  }

  // Fetch a byte segment from memory and increment the pc
  template <typename T>
  T fetch() {
    static_assert(std::is_trivially_copyable_v<T>,
                  "Template type passed to fetch must be trivially copyable.");

    // Little-endian byte loading
    uint8_t buffer[sizeof(T)];
    for (size_t i = 0; i < sizeof(T); i++) {
      buffer[i] = read_from_memory(pc++);
    }
    T result;
    std::memcpy(&result, buffer, sizeof(T));
    return result;
  }

  // Store a byte in memory
  void str() {
    uint8_t destination = fetch<uint8_t>();
    uint8_t data = fetch<uint8_t>();

    out.address = destination;
    out.write_data = data;
    out.write_flag = true;
    memory_transaction();

    std::cout << sc_time_stamp() << ": str " << (int)destination << ", " << (int)data << std::endl;
  }

  // Print a byte stored in memory
  void dsp() {
    uint8_t source_address = fetch<uint8_t>();
    uint8_t data = read_from_memory(source_address);

    std::cout << sc_time_stamp() << ": dsp " << (int)source_address << " -> " << (int)data << std::endl;
  }

  // Perform a jump of the pc
  void jmp() {
    pc = fetch<uint8_t>();
    std::cout << sc_time_stamp() << ": jmp " << (int)pc << std::endl;
  }

  // Halt the CPU
  void stp() {
    halted = true;
    std::cout << sc_time_stamp() << ": stp" << std::endl;
  }

  // Do nothing, just wait
  void nop() {
    std::cout << sc_time_stamp() << ": nop" << std::endl;
  }

  // Core loop of the CPU
  void execute() {
    while (!halted) {
      wait();

      OPCodes opcode = static_cast<OPCodes>(fetch<uint8_t>());
      auto function_map_it = opcode_handlers.find(opcode);
      if (function_map_it != opcode_handlers.end()) {
        // Call the OPCode handler function
        (this->*function_map_it->second)();
      }
      else {
          std::cout << sc_time_stamp() << ": Unknown opcode " << (int)opcode << std::endl;
          halted = true;
      }
    };
  }
};
