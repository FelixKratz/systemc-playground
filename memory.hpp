#include <systemc.h>
#include "types.hpp"

class Memory : public sc_module {
  public:
  struct Input {
    sc_in<mem_addr_t> address;
    sc_in<mem_data_t> write_data;
    sc_in<bool> req, clock, write_flag;
  } in;

  struct Output {
    sc_out<mem_data_t> read_data;
  } out;

  void set_memory(mem_t& mem) { memory = mem; }

  Memory(sc_module_name name) : sc_module(name) {
    memory.fill(0);
    SC_METHOD(operate);
    sensitive << in.clock.neg();
    dont_initialize();
  }

  private:
  mem_t memory;

  void operate() {
    if (in.req) {
      if (in.write_flag) memory[in.address] = in.write_data;
      out.read_data = memory[in.address];
    }
  }
};
