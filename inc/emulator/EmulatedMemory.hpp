#pragma once
#include "../common/Memory.hpp"
#include "../common/Macros.hpp"
#include <list>
#include <cstdint>
class EmulatedMemory{
public:
  struct MemoryRegion{
    uint32_t saddr = 0;
    uint32_t size = 0;
    Memory memory;

  }; 

  MemoryRegion* findRegion(uint32_t address);
 
  void loadFromFile(std::istream& is);


  std::list<MemoryRegion> regions;
  uint64_t size = 0;
};