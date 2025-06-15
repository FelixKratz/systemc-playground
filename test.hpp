#include <systemc>
#include "simulation.hpp"

class CPUTest {
  bool success = true;
  Simulation simulation;

  void run_test(std::string&& name,
                uint64_t cycles,
                mem_t&& start_memory,
                mem_t&& end_memory,
                Registers&& start_registers,
                Registers&& end_registers) {
    simulation.memory.set_memory(std::move(start_memory));

    simulation.step(1);
    uint64_t start_cycles = simulation.cpu.get_cycle_count();

    while (simulation.cpu.get_cycle_count() < start_cycles + cycles
           && !simulation.cpu.is_halted()) {
      simulation.step(1);
    }
    uint64_t end_cycles = simulation.cpu.get_cycle_count();

    if (end_registers != simulation.cpu.copy_registers()) {
      std::cout << name << ": register deviation!" << std::endl;
      success = false;
      printf("%d\n", simulation.cpu.copy_registers().A);
      printf("%d : %d\n", end_registers.A, simulation.cpu.copy_registers().A);
      printf("%d : %d\n", end_registers.X, simulation.cpu.copy_registers().X);
      printf("%d : %d\n", end_registers.Y, simulation.cpu.copy_registers().Y);
    }

    if (end_memory != simulation.memory.copy_memory()) {
      std::cout << name << ": memory deviation!" << std::endl;
      success = false;
    }

    if (cycles != end_cycles - start_cycles) {
      std::cout << name << ": cycle count deviation! Expected: "
                << cycles << " Have: " << end_cycles - start_cycles
                << std::endl;
      success = false;
    }

    if (success) {
      std::cout << "[+] " << name << " passed." << std::endl;
    }
  }

  public:
  CPUTest(std::string&& name,
          uint64_t cycles,
          mem_t&& start_memory,
          Registers&& end_registers,
          mem_t&& end_memory,
          Registers&& start_registers = Registers()) {
    run_test(std::move(name), cycles, std::move(start_memory), std::move(end_memory), std::move(start_registers), std::move(end_registers));
  };

  CPUTest(std::string&& name,
          uint64_t cycles,
          mem_t&& start_memory,
          Registers&& end_registers,
          Registers&& start_registers = Registers()) {
    mem_t end_memory = start_memory;
    run_test(std::move(name), cycles, std::move(start_memory), std::move(end_memory), std::move(start_registers), std::move(end_registers));
  };
};
