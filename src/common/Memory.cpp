#include "../../inc/common/Memory.hpp"
#include <iostream>
#include <iomanip>
void Memory::writeInstruction(Instruction instruction){
  
  memory.push_back(instruction.oc);
  memory.push_back((instruction.ra << 4)|instruction.rb);
  // [] [c] | [] [d] [d] [d]
  //[] [c]  | [] [] [] [d]
  memory.push_back((instruction.disp>>8)|(instruction.rc << 4));
  memory.push_back(instruction.disp);
}

void Memory::writeByte(uint8_t data, uint32_t n){
  
  for(uint32_t i = 0; i<n; i++)
  memory.push_back(data);
}

void Memory::changeByte(uint8_t data, uint32_t location){
  memory[location] = (uint8_t)data;
}


void Memory::writeWord(uint32_t data, uint32_t n){
  for(int i = 0; i < n; i++){
   for (int j = 0; j < 4; j++){

      memory.push_back(data);
      data >>= 8;
    }
  }
}

void Memory::changeWord(uint32_t data, uint32_t location){
 
    for (int i = 0; i < 4; i++){
      memory[location+i] = (uint8_t)data;

      data >>= 8;

    }

}

void Memory::writeWordVector(std::vector<uint32_t>* vector, uint32_t n){
  if(n==0){
    n = vector->size();
  }

  for(int i = 0; i < n; i++){
    writeWord(vector->at(i));
  }
}

void Memory::changeWordVector(std::vector<uint32_t>* vector, uint32_t location, uint32_t n){
  if(n==0){
    n = vector->size();
  }
  for(int i = 0; i < n; i++){
    changeWord(vector->at(i), location);
  }
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