#include <systemc.h>
#include "memory.hpp"
#include "cpu.hpp"

int sc_main(int argc, char* argv[]) {
  sc_clock clock("clock", 1, SC_MS);

  sc_signal<bool> write_flag, req;
  sc_signal<mem_addr_t> address;
  sc_signal<mem_data_t> write_data, read_data;

  CPU cpu("CPU");
  Memory memory("Memory");

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


  uint64_t counter = 0;
  // lda #2
  memory.memory_map[counter++] = OP_LDA_IMM;
  memory.memory_map[counter++] = 2;

  // sta $0xf0
  memory.memory_map[counter++] = OP_STA_ZPG;
  memory.memory_map[counter++] = 0xf0;

  // lda $0xf0
  memory.memory_map[counter++] = OP_LDA_ZPG;
  memory.memory_map[counter++] = 0xf0;

  // Write jmp 0xa
  memory.memory_map[counter++] = OP_JMP_ABS;
  memory.memory_map[counter++] = counter + 0x2;
  memory.memory_map[counter++] = 0;

  // Write nop slide
  memory.memory_map[counter++] = OP_NOP;
  memory.memory_map[counter++] = OP_NOP;
  memory.memory_map[counter++] = OP_NOP;

  // Write stp
  memory.memory_map[counter++] = OP_BRK;

  sc_start(100, SC_MS);
  return 0;
}
