#include "sysc/tracing/sc_trace.h"
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


int sc_main(int argc, char* argv[]) {
  sc_clock clock("clock", 1, SC_MS);
  sc_signal<bool> reset;
  sc_signal<uint32_t> result;

  ClockCounter counter("ClockCounter");
  counter.clock(clock);
  counter.reset(reset);
  counter.result(result);

  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace");
  sc_trace(trace_file, clock, "clock");
  sc_trace(trace_file, reset, "reset");
  sc_trace(trace_file, result, "result");

  reset = true;

  for (int i = 0; i < 100; i++) {
    sc_start(1, SC_MS);
    std::cout
    << sc_time_stamp()
    << ": counter.result = "
    << result.read()
    << std::endl;

    reset = i == 30;
  }

  sc_close_vcd_trace_file(trace_file);

  return 0;
}
