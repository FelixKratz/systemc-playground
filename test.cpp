#include <systemc>
#include "test.hpp"

int sc_main(int argc, char* argv[]) {
  CPUTest lda_imm("LDA IMM", 2, {OP_LDA_IMM, 0xff}, {.A = 0xff, .pc = 0x2});

  std::cout << "All tests completed" << std::endl;
  return 0;
}
