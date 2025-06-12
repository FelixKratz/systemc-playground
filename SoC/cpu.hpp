#include <systemc.h>
#include "registers.hpp"

enum OPCodes : uint8_t {
  OP_NULL,       // nop
  OP_STORE,      // str
  OP_PRINT,      // dsp
  OP_HALT = 0xff // stp
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

  private:
  bool halted = false;
  uint64_t pc = 0;

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

  uint8_t read_from_memory(uint64_t source_address) {
    out.address = source_address;
    out.write_flag = false;
    return memory_transaction();
  }


  template <typename t_out>
  t_out fetch() {
    t_out result = 0;
    for (size_t i = 0; i < sizeof(t_out); i++) {
      *((uint8_t*)&result + i) = read_from_memory(pc++);
    }
    return result;
  }

  void store() {
    uint64_t destination = fetch<uint8_t>();
    uint8_t data = fetch<uint8_t>();

    out.address = destination;
    out.write_data = data;
    out.write_flag = true;
    memory_transaction();

    std::cout << sc_time_stamp() << ": str " << (int)destination << ", " << (int)data << std::endl;
  }

  void print() {
    uint64_t source_address = fetch<uint8_t>();
    uint8_t data = read_from_memory(source_address);

    std::cout << sc_time_stamp() << ": dsp " << source_address << " -> " << (int)data << std::endl;
  }

  void halt() {
    halted = true;
    std::cout << sc_time_stamp() << ": stp" << std::endl;
  }

  void execute() {
    while (!halted) {
      wait();

      uint8_t opcode = fetch<uint8_t>();

      switch (opcode) {
        case OP_STORE: {
          store();
          break;
        };
        case OP_PRINT: {
          print();
          break;
        };
        case OP_HALT: {
          halt();
          break;
        };
        default: {
          std::cout << sc_time_stamp() << ": Unknown opcode " << (int)opcode << std::endl;
          halted = true;
        }
      }
    };
  }

  public:
  CPU(sc_module_name name) : sc_module(name) {
    SC_THREAD(execute);
    sensitive << in.clock.pos();
  }
};
