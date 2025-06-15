#include <systemc.h>
#include "memory.hpp"
#include "cpu.hpp"

class Simulation {
  private:
  CPU cpu;
  Memory memory;

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
  void set_memory(mem_t& mem) {
    memory.set_memory(mem);
  }

  Simulation() : clock("clock", 1, SC_MS), cpu("cpu"), memory("memory") {
    connect();
  }
};
