#include "../../inc/assembler/Assembler.hpp"
#include <iostream>

bool Assembler::finished = false;

void Assembler::handleLabel(std::string* name){
  SymbolTable::addSymbol(
    SymbolTable::getLC(),
    SymbolTable::Type::NOTYP,
    SymbolTable::Bind::LOC,
    *name
  );
};

void Assembler::handleSection(std::string* name){
  SymbolTable::addSymbol(
    SymbolTable::getLC(),
    SymbolTable::Type::SCTN,
    SymbolTable::Bind::LOC,
    *name
  );
  
};

void Assembler::handleSkip(int32_t size){
  if(size < 0){
    std::cerr << "Can't use negative size for .skip." << std::endl;
    return;
  };
  SymbolTable::incLC(size);  

};

void Assembler::handleEnd(){
  
  SymbolTable::end();  
  finished = true;
};