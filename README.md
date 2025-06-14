# 6502 SystemC CPU Simulation

This project is for educational purposes (primarily my own education) and
implements a cycle-accurate simulation of the MOS 6502 CPU using C++ and
SystemC. It models instruction execution and memory transactions with timing
that reflects the original hardware.

## Features

- Cycle-accurate execution
- Single-cycle memory transactions
- Modular opcode and addressing mode handling
- Per-instruction cycle logging
- Initial instruction support:
  - `LDA` (immediate, zero page)
  - `STA` (zero page)
  - `JMP` (absolute)
  - `NOP`
  - `BRK`

## Next Steps

- Extend the instruction set
- Allow connection peripherals
- Run 6502 ROMS
