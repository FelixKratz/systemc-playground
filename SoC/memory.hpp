#include <systemc.h>
#include <unordered_map>

class Memory : public sc_module {
  public:
  std::unordered_map<uint64_t, uint8_t> memory_map;
  struct Input {
    sc_in<uint64_t> address;
    sc_in<uint8_t> write_data;
    sc_in<bool> req, clock, write_flag;
  } in;

  struct Output {
    sc_out<bool> ack;
    sc_out<uint8_t> read_data;
  } out;

  void operate() {
    if (!in.req) out.ack = false;
    else {
      if (in.write_flag) memory_map[in.address] = in.write_data;
      out.read_data = memory_map[in.address];
      out.ack = true;
    }
  }

  Memory(sc_module_name name) : sc_module(name) {
    SC_METHOD(operate);
    sensitive << in.clock.pos();
    dont_initialize();
  }
};
