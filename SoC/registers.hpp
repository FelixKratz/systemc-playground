#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

class Registers {
  private:
  std::vector<uint32_t> registers;
  inline void check_range(size_t index) const {
    if (index >= registers.size())
      throw std::out_of_range("The provided index is out of range.");
  }

  public:
  Registers() {};

  void set_size(uint32_t size) {
    registers.resize(size, 0);
  };

  void write(size_t index, uint32_t value) {
    check_range(index);
    registers[index] = value;
  };
  
  uint32_t read(size_t index) const {
    check_range(index);
    return registers[index];
  };

  void reset() {
    std::fill(registers.begin(), registers.end(), 0);
  }

  Registers(const Registers&) = delete;
  Registers& operator=(const Registers&) = delete;

  Registers(Registers&&) = default;
  Registers& operator=(Registers&&) = default;

  friend std::ostream& operator<<(std::ostream& o, const Registers& reg) {
    o << "Contents of the registers: " << std::endl;
    for (size_t i = 0; i < reg.registers.size(); i++) {
      o << i << " = " << reg.registers[i] << std::endl;
    }
    return o;
  };
};
