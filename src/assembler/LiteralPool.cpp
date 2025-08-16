#include "../../inc/assembler/LiteralPool.hpp"


void LiteralPool::put(uint32_t value, uint32_t location_to_patch, bool invalid){

  isInvalidLocation[location_to_patch] = invalid;
  literalpatch.push_back({location_to_patch, (uint32_t)pool.size()}); 
  pool.push_back(value);
}

size_t LiteralPool::size() const{
  return pool.size();
}

bool LiteralPool::isInvalid(uint32_t location){
  
  return isInvalidLocation.count(location) > 0 &&  isInvalidLocation[location] == true;
}

// void LiteralPool::literalBackpatch(Memory* memory, uint32_t literalPoolStartAddr){


//   // literal patch, writing PC relative displacement in instruction to literal pool
//   while(!literalpatch.empty()){
//     // get location to patch
//     LiteralPatch p = literalpatch.front();
//     literalpatch.pop_front();
//     // calculate location to patch and displacement
//     uint32_t loc_to_patch = p.location;


//     uint32_t displacement = literalPoolStartAddr + p.index_of_literal * 4 - (loc_to_patch+2);
//     // [CCCC][DDDD] [DDDD][DDDD]
//     //  ^rc  ^highDisp 
//     uint8_t rc_highDisp = memory->readByte(loc_to_patch); 
//     uint8_t oo_highDisp = (uint8_t)(0x0F & (displacement >> 8));
//     uint8_t rc_oo = (0xF0 &rc_highDisp);

//     memory->changeByte( rc_oo | oo_highDisp,
//      loc_to_patch);
//     memory->changeByte(displacement, loc_to_patch+1);
    
//   }
// }