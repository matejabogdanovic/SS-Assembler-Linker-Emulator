#pragma once
#include <cstdint>
#include <vector>
#include "./Instruction.hpp"
class Memory{
public:
  void writeInstruction(Instruction instruction);
  void writeByte(uint8_t data, uint32_t n = 0);
  void changeByte(uint8_t data, uint32_t location);
  void print();
private:
  std::vector<uint8_t> memory;

};