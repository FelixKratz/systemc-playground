#include <systemc.h>

SC_MODULE(UIntConsumer) {
  sc_in<bool> clock;
  sc_in<uint32_t> input;

  void consume() {
      std::cout
      << sc_time_stamp()
      << ": input = "
      << input.read()
      << std::endl;
  }
  
  SC_CTOR(UIntConsumer) {
    SC_METHOD(consume);
    sensitive << clock.pos();
    dont_initialize();
  }
};
