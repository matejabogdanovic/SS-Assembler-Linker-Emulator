#include "../../inc/emulator/EmulatedMemory.hpp"

EmulatedMemory::MemoryRegion* EmulatedMemory::findRegion(uint32_t address){
  for(MemoryRegion& region: regions){
    //0[]1[]2[]
    // 2
    if(region.saddr <= address && (region.saddr + region.size) > address ){
      return &region;
    }
  }
  return nullptr;
}

void EmulatedMemory::loadFromFile(std::istream& is){
  uint32_t start_address = 0;

  MemoryRegion* curr_region,*prev_region = nullptr;

  uint8_t byte;
  uint32_t sz = 0;

  while(is.read(reinterpret_cast<char*>(&start_address), sizeof(start_address))){
 

    is.read(reinterpret_cast<char*>(& sz), sizeof(sz));
    if((uint64_t)(this->size + sz > 0x100000000)){
      std::cerr << "Not enough memory space." << std::endl;
      exit(-1);
    }
    this->size += sz;

    if(prev_region && prev_region->saddr+prev_region->size == start_address){
      
      curr_region = prev_region;
    }else{ // making new region
      regions.push_back({});
      curr_region = &regions.back();

      curr_region->saddr = start_address;
    }
  
    
    for(int i = 0; i < sz; i++){
      is.read(reinterpret_cast<char*>(& byte), sizeof(byte));
      curr_region->memory.writeByte(byte);
    }

LOG(curr_region->memory.print(std::cout, curr_region->size, sz, curr_region->saddr);)
    curr_region->size += sz;
    prev_region = curr_region;
    
  }
  
LOG(std::cout << "Region count: " << regions.size() << std::endl;)
}