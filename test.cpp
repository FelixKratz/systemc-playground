#include <systemc>
#include "test.hpp"

void test_lda_imm() {
  uint64_t cycles = 2;

  mem_t start_memory = {OP_LDA_IMM, 0xff};
  mem_t end_memory = start_memory;

  Registers start_registers = {};
  Registers end_registers = {.A = 0xff, .pc = 0x2};
  run_test("lda imm", cycles, start_memory, end_memory, start_registers, end_registers);
}

void test_sta_zpg() {
  uint64_t cycles = 3;

  mem_t start_memory = {OP_STA_ZPG, 0xff};
  mem_t end_memory = start_memory;
  end_memory[0xff] = 0x1a;

  Registers start_registers = {.A = 0x1a};
  Registers end_registers = {.A = 0x1a, .pc = 0x2};
  run_test("sta zpg", cycles, start_memory, end_memory, start_registers, end_registers);
}


using TestFunction = void (*)();
TestFunction test_cases[] = {
  test_lda_imm,
  test_sta_zpg,
};

int sc_main(int argc, char* argv[]) {
  if (argc == 2) {
    int test_case = 0;
    if (sscanf(argv[1], "%d", &test_case) == 1) {
      if (test_case < sizeof(test_cases) / sizeof(TestFunction)) {
        test_cases[test_case]();
      } 
    }
  }
  return 0;
}
