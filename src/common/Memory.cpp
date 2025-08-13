#include "../../inc/common/Memory.hpp"
#include <iostream>
#include <iomanip>
void Memory::writeInstruction(Instruction instruction){
  
  memory.push_back(instruction.oc);
  memory.push_back((instruction.ra << 4)|instruction.rb);
  memory.push_back((instruction.rc << 4)|(8>>instruction.disp));
  memory.push_back(instruction.disp);
}

void Memory::writeByte(uint8_t data, uint32_t n){
  
  for(uint32_t i = 0; i<n; i++)
    memory.push_back(0);
}

void Memory::changeByte(uint8_t data, uint32_t location){
  memory[location] = (uint8_t)data;
}

void Memory::print(){

  for(uint32_t i = 0; i < memory.size(); i++){
     if(i%8==0)std::cout << (i>0 ? "\n":"") << std::right << std::uppercase << std::setw(4) << std::setfill('0') << std::hex <<  
      i << ":";
   std::cout << " " <<  std::right << std::uppercase << std::setw(2) << std::setfill('0') << std::hex <<  
   static_cast<int>(memory[i]);
  

  }
  std::cout << std::dec << std::endl;
}