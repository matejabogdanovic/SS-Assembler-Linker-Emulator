#include "../../inc/assembler/SymbolTable.hpp"
#include <iostream>
#include <iomanip>
// Num Value Size Type Bind Ndx Name
// 0: 00000000 0 NOTYP LOC UND
// 1: 00000000 0 SCTN LOC 1 .text
// 2: 00000000 0 SCTN LOC 2 .data
// 3: 00000000 0 SCTN LOC 3 .bss
// 4: 00000008 0 NOTYP LOC 2 e
// 5: 00000014 0 NOTYP LOC 1 d
// 6: 00000014 0 NOTYP GLOB 2 a
// 7: 00000000 0 NOTYP GLOB 3 c
// 8: 00000000 0 NOTYP GLOB UND b

std::vector <std::string> SymbolTable::sym_names = {""};
SymbolTable::Map SymbolTable::table = initMap();

uint32_t SymbolTable::current_section = 0;
const char* SymbolTable::type_str[] = {"NOTYP", "SCTN"};
const char* SymbolTable::bind_str[] = {"LOC", "GLOB"};  
uint32_t SymbolTable::LC = 0;

SymbolTable::Map SymbolTable::initMap(){
  Map map;
  map[""] = {0, 0, 0, Type::NOTYP, Bind::LOC, 0};
  return map;
}


void SymbolTable::printTable(){
  
  std::cout << 
    "|  Num  " << 
    "|  Type  "<< 
    "|  Bind  " << 
    "|  Value  " << 
    "|  Size  " << 
    "|  Ndx  "<< 
    "|  Name  |" << 
    std::endl;
  for(int i = 0; i < sym_names.size(); i++){
    Entry* e =  &table[sym_names[i]];
    std::cout << 
    " " << std::left << std::setw(7) << std::setfill(' ') << e->num <<
    " " << std::left << std::setw(8) << std::setfill(' ') << type_str[e->type] << 
    " " << std::left << std::setw(8) << std::setfill(' ') << bind_str[e->bind] << 
    " x" << std::right << std::setw(8) << std::setfill('0') << std::hex << e->value << std::dec <<  
    " " << std::left << std::setw(8) << std::setfill(' ') << e->size << 
    " " << std::left << std::setw(7) << std::setfill(' ');

    if(e->ndx == 0)std::cout << "UND";
    else std::cout << e->ndx;

    std::cout
    << " " << std::left << std::setw(8) << std::setfill(' ') << sym_names[i] << 
    std::endl;
  }

}

SymbolTable::Status SymbolTable::addSymbol(uint32_t value,
    Type type,
    Bind bind,
    std::string name){
    
  
      
  if(type==Type::SCTN){
    if(current_section != 0){ // set previous section size
      if(table.count(name) > 0){
        return Status::ERROR;  
      }
      table[sym_names[current_section]].size = LC;
    }
    current_section = sym_names.size();
    LC = 0;
  }else if (current_section == 0){
    std::cerr << "Undefined section for symbol: " << name << std::endl;
    return Status::ERROR;
  }

  table[name] = {(uint32_t)sym_names.size(), value, 0, type, bind, current_section};
  sym_names.push_back(name);
  
  // printTable();
    

  return Status::OK;
}

SymbolTable:: Status SymbolTable::end(){
  // close current section
  table[sym_names[current_section]].size = LC;

  return Status::OK;
};


SymbolTable::Status SymbolTable::incLC(uint32_t val){
  if(current_section == 0 && val > 0){
    std::cerr << "Undefined section for symbol." << std::endl;
    return Status::ERROR;
  }
  LC += val;
  return Status::OK;
};

uint32_t SymbolTable::getLC(){
  return LC;
};