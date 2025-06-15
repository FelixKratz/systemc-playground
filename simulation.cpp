#include "simulation.hpp"

int sc_main(int argc, char* argv[]) {
  Simulation simulation;

  mem_addr_t counter = 0;
  mem_t memory;
  // lda #2
  memory[counter++] = OP_LDA_IMM;
  memory[counter++] = 2;

  // sta $0xf0
  memory[counter++] = OP_STA_ZPG;
  memory[counter++] = 0xf0;

  // lda $0xf0
  memory[counter++] = OP_LDA_ZPG;
  memory[counter++] = 0xf0;

  // Write jmp 0xa
  memory[counter++] = OP_JMP_ABS;
  memory[counter++] = counter + 0x2;
  memory[counter++] = 0;

  // Write nop slide
  memory[counter++] = OP_NOP;
  memory[counter++] = OP_NOP;
  memory[counter++] = OP_NOP;

  // Write stp
  memory[counter++] = OP_BRK;

  simulation.memory.set_memory(std::move(memory));

  sc_start(100, SC_MS);
  return 0;
}
