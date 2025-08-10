#include "../../inc/assembler/Assembler.hpp"
#include <iostream>


void Assembler::handleLabel(std::string* name){
  SymbolTable::addSymbol(
    SymbolTable::LC,
    SymbolTable::Type::NOTYP,
    SymbolTable::Bind::LOC,
    *name
  );
};