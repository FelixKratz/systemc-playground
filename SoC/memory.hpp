#include <systemc.h>
#include <unordered_map>

SC_MODULE(Memory) {
  sc_in<bool> clock;
  sc_in<bool> write_flag;
  sc_in<uint64_t> address;
  sc_in<uint8_t> write_data;
  sc_out<uint8_t> read_data;

  std::unordered_map<uint64_t, uint8_t> memory_map;

  void operate() {
    if (write_flag.read()) {
      memory_map[address.read()] = write_data.read();
    }
    read_data.write(memory_map[address.read()]);
  }

  SC_CTOR(Memory) {
    SC_METHOD(operate);
    sensitive << clock.pos();
    dont_initialize();
    memory_map[0] = 1;
  }
};
