#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include "./Instruction.hpp"
class Memory{
public:
  void writeInstruction(Instruction instruction);
  void changeInstruction(Instruction instruction, uint32_t location);

  uint8_t readByte(uint32_t location) const;
  void writeByte(uint8_t data, uint32_t n = 1);
  void changeByte(uint8_t data, uint32_t location);
  
  void writeWord(uint32_t data, uint32_t n = 1);
  void changeWord(uint32_t data, uint32_t location);
  
  void writeWordVector(std::vector<uint32_t>* vector, uint32_t n=0);
  void changeWordVector(std::vector<uint32_t>* vector, uint32_t location, uint32_t n=0);

  void print(std::ostream& os);
private:
  std::vector<uint8_t> memory;

};