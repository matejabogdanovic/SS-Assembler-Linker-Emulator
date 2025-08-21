#include "../../inc/assembler/LiteralPool.hpp"


void LiteralPool::put(uint32_t value, 
  uint32_t instr_location,
  std::string* symbol_name,
  Instruction alternative){

 
  patches.push_back({instr_location, value,  symbol_name?*symbol_name:"", alternative}); 

}

bool LiteralPool::isUsingSymbol(LiteralPatch* patch){
  
  return patch->symbol_name != ""; 
}
