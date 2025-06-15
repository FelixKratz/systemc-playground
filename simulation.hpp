#include <systemc.h>
#include "memory.hpp"
#include "cpu.hpp"

constexpr int time_per_cycle = 1000; // Measures in ns -> 1MHz

class Simulation {
  private:
  sc_signal<bool> write_flag, req;
  sc_signal<mem_addr_t> address;
  sc_signal<mem_data_t> write_data, read_data;

  sc_clock clock;

  void connect() {
    cpu.in.clock(clock);
    cpu.in.read_data(read_data);

    cpu.out.req(req);
    cpu.out.address(address);
    cpu.out.write_flag(write_flag);
    cpu.out.write_data(write_data);

    memory.in.clock(clock);
    memory.in.write_flag(write_flag);
    memory.in.address(address);
    memory.in.write_data(write_data);
    memory.in.req(req);

    memory.out.read_data(read_data);

    req.write(false);
  }

  public:
  CPU cpu;
  Memory memory;

  void step(int count) {
    sc_start(time_per_cycle * count, SC_NS);
  }

  Simulation(bool logging = false) : clock("clock", time_per_cycle, SC_NS), cpu("cpu"), memory("memory") {
    connect();
    cpu.set_logging(logging);
  }
};
