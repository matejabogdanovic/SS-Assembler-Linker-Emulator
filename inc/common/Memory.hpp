#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include "./Instruction.hpp"
#include "./SymbolTable.hpp"
class Memory{
public:
  inline size_t size()const{
    return memory.size();
  }  

  void writeInstruction(Instruction instruction);
  void changeInstruction(Instruction instruction, uint32_t location);
  void changeInstructionDisplacement(uint32_t instr_location, uint16_t displacement);

  uint8_t readByte(uint32_t location) const;
  void writeByte(uint8_t data, uint32_t n = 1);
  void changeByte(uint8_t data, uint32_t location);
  
  void writeWord(uint32_t data, uint32_t n = 1);
  void changeWord(uint32_t data, uint32_t location);
  
  void writeWordVector(std::vector<uint32_t>* vector, uint32_t n=0);
  void changeWordVector(std::vector<uint32_t>* vector, uint32_t location, uint32_t n=0);
  
  void printCode(std::ostream& os, SymbolTable* symtab);
  void print(std::ostream& os, uint32_t location=0, uint32_t n=0, uint32_t base_address=0);
  void printBinary(std::ostream& os, uint32_t location=0, uint32_t n=0, bool write_size=true);
  void loadFromFile(std::istream& is);


   std::vector<uint8_t>*  getMemoryVector() ;
private:
  std::vector<uint8_t> memory;

};