#include "types.hpp"

struct Registers {
  mem_addr_t pc = 0;
  mem_data_t A = 0;
  mem_data_t X = 0;
  mem_data_t Y = 0;
};
