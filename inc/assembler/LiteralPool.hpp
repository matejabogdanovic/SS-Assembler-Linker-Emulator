#pragma once
#include "../common/Memory.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <list>
class LiteralPool{
public:


  void put(uint32_t value, uint32_t location_to_patch, std::string* symbol_name = nullptr, 
    Instruction alternative ={Instruction::OPCode::HALT, 0, 0, 0, 0});

  bool isUsingSymbol(uint32_t location);


  typedef struct LiteralPatch {
    uint32_t location;
    uint32_t literal; 
    std::string symbol_name;
    Instruction alternative ={Instruction::OPCode::HALT, 0, 0, 0, 0}; 
    LiteralPatch(uint32_t location,
    uint32_t literal, 
    std::string symbol_name,
    Instruction alternative ={Instruction::OPCode::HALT, 0, 0, 0, 0})
    :location(location), literal(literal), symbol_name(symbol_name), alternative(alternative)
    {

    }

  }LiteralPatch;
  std::list <LiteralPatch> patches; 

  std::unordered_map <uint32_t, bool> usingSymbol;

   
};