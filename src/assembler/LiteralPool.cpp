#include "../../inc/assembler/LiteralPool.hpp"


void LiteralPool::put(uint32_t value, uint32_t location_to_patch, bool invalid){

  isInvalidLocation[location_to_patch] = invalid;
  patches.push_back({location_to_patch, value}); 

}

bool LiteralPool::isInvalid(uint32_t location){
  
  return isInvalidLocation.count(location) > 0 &&  isInvalidLocation[location] == true;
}
