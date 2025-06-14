#include <systemc.h>
#include <unordered_map>
#include "types.hpp"

class Memory : public sc_module {
  public:
  std::unordered_map<mem_addr_t, mem_data_t> memory_map;
  struct Input {
    sc_in<mem_addr_t> address;
    sc_in<mem_data_t> write_data;
    sc_in<bool> req, clock, write_flag;
  } in;

  struct Output {
    sc_out<mem_data_t> read_data;
  } out;

  void operate() {
    if (in.req) {
      if (in.write_flag) memory_map[in.address] = in.write_data;
      out.read_data = memory_map[in.address];
    }
  }

  Memory(sc_module_name name) : sc_module(name) {
    SC_METHOD(operate);
    sensitive << in.clock.neg();
    dont_initialize();
  }
};
