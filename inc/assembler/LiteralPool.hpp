#pragma once
#include "../common/Memory.hpp"

#include <cstdint>
#include <string>

#include <list>
class LiteralPool{
public:
  typedef struct LiteralPatch {
    uint32_t instr_location; 
    uint32_t literal; 
    std::string symbol_name;
    Instruction alternative ={Instruction::OPCode::HALT, 0, 0, 0, 0}; 
    LiteralPatch(uint32_t instr_location,
    uint32_t literal, 
    std::string symbol_name,
    Instruction alternative ={Instruction::OPCode::HALT, 0, 0, 0, 0})
    :instr_location(instr_location), literal(literal), symbol_name(symbol_name), alternative(alternative)
    {

    }

  }LiteralPatch;

  void put(uint32_t value, uint32_t instr_location, std::string* symbol_name = nullptr, 
    Instruction alternative ={Instruction::OPCode::HALT, 0, 0, 0, 0});

  bool isUsingSymbol(LiteralPatch* patch);



  std::list <LiteralPatch> patches; 

};