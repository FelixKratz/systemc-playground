#include "sysc/tracing/sc_trace.h"
#include <systemc.h>
#include "consumer.hpp"
#include "counter.hpp"

int sc_main(int argc, char* argv[]) {
  // Signals
  sc_clock clock("clock", 1, SC_MS);
  sc_signal<bool> reset;
  sc_signal<uint32_t> signal_line;

  // Setup the clock counter module
  ClockCounter counter("ClockCounter");
  counter.clock(clock);
  counter.reset(reset);
  counter.result(signal_line);

  // Setup the consumer module
  UIntConsumer consumer("ClockConsumer");
  consumer.clock(clock);
  consumer.input(signal_line);

  // Create trace file
  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace");
  sc_trace(trace_file, clock, "clock");
  sc_trace(trace_file, reset, "reset");
  sc_trace(trace_file, signal_line, "signal");

  // Run simulation
  reset = true;

  for (int i = 0; i < 100; i++) {
    sc_start(1, SC_MS);
    reset = i == 30;
  }

  sc_close_vcd_trace_file(trace_file);
  return 0;
}
