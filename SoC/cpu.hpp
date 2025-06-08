#include <systemc.h>
#include "registers.hpp"

enum OPCodes : uint8_t {
  OP_NULL,       // nll
  OP_STORE,      // str
  OP_PRINT,      // dsp
  OP_HALT = 0xff // stp
};

SC_MODULE(CPU) {
  sc_in<bool> clock;
  sc_out<bool> write_flag;
  sc_out<uint64_t> address;
  sc_out<uint8_t> write_data;
  sc_in<uint8_t> read_data;

  bool halted = false;
  uint64_t pc = 0;

  uint8_t read_from_memory(uint64_t source_address) {
    address.write(source_address);
    write_flag.write(false);
    wait(SC_ZERO_TIME);
    wait();
    return read_data.read();
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

    address.write(destination);
    write_data.write(data);
    write_flag.write(true);

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

  SC_CTOR(CPU) {
    SC_THREAD(execute);
    sensitive << clock.neg();
  }
};
