#include <systemc.h>
#include "memory.hpp"
#include "cpu.hpp"

int sc_main(int argc, char* argv[]) {
  sc_clock clock("clock", 1, SC_MS);

  sc_signal<bool> write_flag, ack, req;
  sc_signal<uint64_t> address;
  sc_signal<uint8_t> write_data, read_data;

  CPU cpu("CPU");
  Memory memory("Memory");

  cpu.clock(clock);
  cpu.address(address);
  cpu.write_flag(write_flag);
  cpu.read_data(read_data);
  cpu.write_data(write_data);
  cpu.ack(ack);
  cpu.req(req);

  memory.in.clock(clock);
  memory.in.write_flag(write_flag);
  memory.in.address(address);
  memory.in.write_data(write_data);
  memory.in.req(req);

  memory.out.read_data(read_data);
  memory.out.ack(ack);

  ack.write(false);
  req.write(false);


  uint64_t counter = 0;
  // Write mov 0xf0, 2
  memory.memory_map[counter++] = OP_STORE;
  memory.memory_map[counter++] = 0xf0;
  memory.memory_map[counter++] = 2;

  // // Write dsp 0x100
  memory.memory_map[counter++] = OP_PRINT;
  memory.memory_map[counter++] = 0xf0;

  // Write stp
  memory.memory_map[counter++] = OP_HALT;

  sc_start(100, SC_MS);
  return 0;
}
