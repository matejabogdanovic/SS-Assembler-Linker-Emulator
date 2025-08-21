#include "../../inc/common/Memory.hpp"

#include <iomanip>
void Memory::writeInstruction(Instruction instruction){
  
  memory.push_back(instruction.oc);
  memory.push_back((instruction.ra << 4)|instruction.rb);
  // [] [c] | [] [d] [d] [d]
  //[] [c]  | [] [] [] [d]
  memory.push_back(((instruction.disp>>8)&0x0F)|(instruction.rc << 4));
  memory.push_back((uint8_t)instruction.disp);
}

void Memory::changeInstruction(Instruction instruction, uint32_t location){
  
  changeByte(instruction.oc, location++);
  changeByte((instruction.ra << 4)|instruction.rb, location++);
  changeByte(((instruction.disp>>8)&0x0F)|(instruction.rc << 4), location++);
  changeByte((uint8_t)instruction.disp, location);
}

void Memory::changeInstructionDisplacement(uint32_t instr_location, uint16_t displacement){
    // [CCCC][DDDD] [DDDD][DDDD]
    //  ^rc  ^highDisp 


    uint8_t rc_highDisp = readByte(instr_location+2); 
    uint8_t oo_highDisp = (uint8_t)(0x0F & (displacement >> 8));
    uint8_t rc_oo = (0xF0 & rc_highDisp);

    changeByte( rc_oo | oo_highDisp,
     instr_location+2);
    changeByte(displacement, instr_location+3);
    
}

uint8_t Memory::readByte(uint32_t location) const{
  return memory[location];
}

void Memory::writeByte(uint8_t data, uint32_t n){
  
  for(uint32_t i = 0; i<n; i++)memory.push_back(data);
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
    changeWord(vector->at(i), location + i);
  }
}



void Memory::print(std::ostream& os, uint32_t location, uint32_t n){
  if(n==0){
    n = memory.size();
  }
    if(location % 8 != 0){
      os << std::right << std::uppercase << std::setw(4) << std::setfill('0') << std::hex <<  
      location<< ":";
    }
  for(uint32_t i = location; i < location + n; i++){

     if(i%8==0)os << (i>0 ? "\n":"") << std::right << std::uppercase << std::setw(4) << std::setfill('0') << std::hex <<  
      i << ":";
   os << " " <<  std::right << std::uppercase << std::setw(2) << std::setfill('0') << std::hex <<  
   static_cast<int>(memory[i]);
  
    
  }
  os << std::dec << std::endl;
}