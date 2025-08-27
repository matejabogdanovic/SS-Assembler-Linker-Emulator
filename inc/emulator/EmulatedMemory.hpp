#pragma once
#include "../common/Memory.hpp"
#include "../common/Macros.hpp"
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
    // auto region = findRegion(address);
    // if(!region){
    //   std::cerr << "Segmentation fault :)"<< std::endl;
    //   exit(-1);
    // }
    // return region->memory.readByte(address-region->saddr);
    return map[address];
  }
  // read byte x 4??, ako poslednja adresa a mi hocemo da citamo jos 3 sledece, problem.
  inline uint32_t readWord(uint32_t address ){
    // auto region = findRegion(address);
    // if(!region){
    //   std::cerr << "Segmentation fault :)"<< std::endl;
    //   exit(-1);
    // }
    // return region->memory.readWord(address-region->saddr);
    uint32_t data = 0;
    uint8_t byte;
    for (size_t i = 0; i < 4; i++)
    {
      byte = map[address+i];
      data = data | ((uint32_t)byte << (8 * i)); 
    }
    
    return data;
  }
  inline void changeWord(uint32_t data , uint32_t address ){
    // auto region = findRegion(address);
    // if(!region){
    //   std::cerr << "Segmentation fault :)"<< std::endl;
    //   exit(-1);
    // }
    // region->memory.changeWord(data, address-region->saddr);
     
    for (int i = 0; i < 4; i++){
      map[address+i] = (uint8_t)data;

      data >>= 8;

    }

  }
private:
  // MemoryRegion* findRegion(uint32_t address);
  // std::list<MemoryRegion> regions;
  uint64_t size = 0;

  std::unordered_map<uint32_t, uint32_t> map;

};