#include "../../inc/assembler/LiteralPool.hpp"


void LiteralPool::put(uint32_t value, uint32_t location_to_patch,
  std::string* symbol_name,
  Instruction alternative){

  if(symbol_name)usingSymbol[location_to_patch] = true;
  patches.push_back({location_to_patch, value,  symbol_name?*symbol_name:"", alternative}); 

}

bool LiteralPool::isUsingSymbol(uint32_t location){
  
  return usingSymbol.count(location) > 0 &&  usingSymbol[location] == true;
}
