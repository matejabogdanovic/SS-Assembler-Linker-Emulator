#include "../../inc/emulator/EmulatedMemory.hpp"


void EmulatedMemory::loadFromFile(std::istream& is){
  uint32_t start_address = 0;

  uint8_t byte;
  uint32_t sz = 0;

  while(is.read(reinterpret_cast<char*>(&start_address), sizeof(start_address))){
 

    is.read(reinterpret_cast<char*>(& sz), sizeof(sz));
    if((uint64_t)(this->size + sz > 0x100000000)){
      throw EmulatorException("not enough memory");

    }
    this->size += sz;

    
    for(int i = 0; i < sz; i++){
      is.read(reinterpret_cast<char*>(& byte), sizeof(byte));

      map[start_address+i] = byte;
    }
 
  }

}

void EmulatedMemory::changeWord(uint32_t data , uint32_t address ){
  
  LOG(std::cout <<  "\tWriting word to: "<<std::hex << address <<std::dec;)
  LOG(std::cout <<  "\tData is: "<<std::hex << data <<std::dec << std::endl;)

  for (int i = 0; i < 4; i++){
    map[address+i] = (uint8_t)data;
    data >>= 8;
  }
  // terminal out, needed to do like this so we get correct output every time
  if(address >= TERM_OUT && address<= (TERM_OUT+3)){

    std::cout <<  (char)(readWord(TERM_OUT)) << std::flush;
  
  }

}

uint32_t EmulatedMemory::readWord(uint32_t address ){

  LOG(std::cout <<  "\tReading word from: "<<std::hex << address <<std::dec ;)
  uint32_t data = 0;
  uint8_t byte;
  for (size_t i = 0; i < 4; i++)
  {
    byte = map[address+i];
    data = data | ((uint32_t)byte << (8 * i)); 
  }
  LOG(std::cout <<  "\tData is: "<<std::hex << data <<std::dec << std::endl;)

  return data;
}