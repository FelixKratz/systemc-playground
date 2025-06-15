#include <systemc>
#include "simulation.hpp"

static void run_test(std::string&& name,
              uint64_t cycles,
              mem_t& start_memory,
              mem_t& end_memory,
              Registers& start_registers,
              Registers& end_registers) {

  bool success = true;
  Simulation simulation;

  simulation.cpu.set_registers(start_registers);
  simulation.memory.set_memory(std::move(start_memory));
  simulation.step(1);
  uint64_t start_cycles = simulation.cpu.get_cycle_count();
  simulation.cpu.set_logging(true);

  while (simulation.cpu.get_cycle_count() < start_cycles + cycles
         && !simulation.cpu.is_halted()) {
    simulation.step(1);
  }
  uint64_t end_cycles = simulation.cpu.get_cycle_count();

  if (end_registers != simulation.cpu.copy_registers()) {
    std::cout << name << ": register deviation!" << std::endl;
    success = false;
    printf("%d : %d\n",simulation.cpu.copy_registers().pc, end_registers.pc);
    printf("%d : %d\n",simulation.cpu.copy_registers().A, end_registers.A);
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
