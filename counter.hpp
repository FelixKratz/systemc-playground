#include <systemc.h>

SC_MODULE(ClockCounter) {
  sc_in<bool> clock;
  sc_in<bool> reset;
  sc_out<uint32_t> result;

  void tick() {
    result.write((result.read() + 1)  * !reset.read());
  }

  SC_CTOR(ClockCounter) {
    SC_METHOD(tick);
    sensitive << clock.pos();
    dont_initialize();
  }
};

