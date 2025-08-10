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
std::unordered_map <std::string, SymbolTable::Entry> SymbolTable::table{
 {std::string(" "), {0, 0, SymbolTable::Type::NOTYP, SymbolTable::Bind::LOC, 0 }}};

uint32_t SymbolTable::current_section = 0;
const char* SymbolTable::type_str[] = {"NOTYP", "SCTN"};
const char* SymbolTable::bind_str[] = {"LOC", "GLOB"};  
uint32_t SymbolTable::LC = 0;
void SymbolTable::printTable(){
  
  std::cout << 
    "| Num " << 
    "| Type "<< 
    "| Bind " << 
    "| Value " << 
    "| Ndx "<< 
    "| Name |" << 
    std::endl;
  for(int i = 0; i < sym_names.size(); i++){
    Entry* e =  &table[sym_names[i]];
    std::cout <<
    e->num <<"      "<< 
    type_str[e->type]<<"  " <<  
    bind_str[e->bind] <<"  " << 
    std::setw(8) << std::setfill('0')<< std::hex << e->value << std::dec <<"  " ;

    if(e->ndx == 0)std::cout << "UND";
    else std::cout << e->ndx;

    std::cout<<"  " << 
    sym_names[i] <<"  " << 
    std::endl;
  }

}
    // uint32_t value;
    // Type type;
    // Bind bind;
    // uint32_t ndx;
    // std::string name;
SymbolTable::Status SymbolTable::addSymbol(uint32_t value,
    Type type,
    Bind bind,
    std::string name){
    
  
  table[name] = {(uint32_t)sym_names.size(), value, type, bind, 0};
  sym_names.push_back(name);
  
  printTable();
    

  return Status::OK;
}
