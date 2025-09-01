#pragma once
#include "../common/Memory.hpp"
#include "../common/Macros.hpp"
#include "./EmulatorException.hpp"
#include <unordered_map>
#include <cstdint>


class EmulatedMemory{
public:
  struct MemoryRegion{
    uint32_t saddr = 0;
    uint32_t size = 0;
    Memory memory;

  }; 


 
  void loadFromFile(std::istream& is);
  
  inline uint8_t readByte(uint32_t address ){

    uint8_t data = map[address];

    return data;
  }
  
  uint32_t readWord(uint32_t address);
  void changeWord(uint32_t data , uint32_t address );

  static const uint32_t TERM_OUT = 0xffffff00;
  static const uint32_t TERM_IN = 0xffffff04;
  
private:

  uint64_t size = 0;

  std::unordered_map<uint32_t, uint32_t> map;

 
};