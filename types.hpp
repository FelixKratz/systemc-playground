#include <systemc>

using mem_data_t = uint8_t;
using mem_addr_t = uint16_t;
using mem_addr_zp_t = uint8_t;
using mem_t = std::array<uint8_t, 0x10000>;
using opcode_t   = uint8_t;
