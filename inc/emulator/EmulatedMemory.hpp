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


 
  void loadFromFile(std::istream& is);
  
  inline uint8_t readByte(uint32_t address ){
    auto region = findRegion(address);
    if(!region){
      std::cerr << "Segmentation fault :)"<< std::endl;
      exit(-1);
    }
    return region->memory.readByte(address-region->saddr);
  }
private:
  MemoryRegion* findRegion(uint32_t address);
  std::list<MemoryRegion> regions;
  uint64_t size = 0;


};