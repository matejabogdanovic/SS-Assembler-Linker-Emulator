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


const char* SymbolTable::type_str[] = {"NOTYP", "SCTN"};
const char* SymbolTable::bind_str[] = {"LOC", "GLOB"};  



SymbolTable::SymbolTable(){
 
  this->table[""] = {0, 0, 0, Type::NOTYP, Bind::LOC, "UND", 0};
  this->sections.push_back("");
}

void SymbolTable::printTable(){
  
  std::cout << 
    "|  Num  " << 
    "|  Type  "<< 
    "|  Bind  " << 
    "|  Value  " << 
    "|  Size  " << 
    "|  Ndx  "<< 
    "|  Extern  " << 
    "|  Defined  " << 
    "|  Name  |" << 
    std::endl;
  for(int i = 0; i < sections.size(); i++){
    Entry* e =  &table[sections[i]];
    e->num = i;
    e->ndx = i;
    std::cout << 
    " " << std::left << std::setw(7) << std::setfill(' ') << e->num <<
    " " << std::left << std::setw(8) << std::setfill(' ') << type_str[e->type] << 
    " " << std::left << std::setw(8) << std::setfill(' ') << bind_str[e->bind] << 
    " x" << std::right << std::setw(8) << std::setfill('0') << std::hex << e->value << std::dec <<  
    " " << std::left << std::setw(8) << std::setfill(' ') << e->size << 
    " " << std::left << std::setw(7) << std::setfill(' ');

    if(e->ndx == 0)std::cout << "UND";
    else std::cout << e->ndx;

     std::cout << 
    " " << std::left << std::setw(10) << std::setfill(' ') << (e->is_extern ? "true" : "false")<< 
    " " << std::left << std::setw(11) << std::setfill(' ') << (e->is_defined ? "true" : "false")<< 
    " " << std::left << std::setw(8) << std::setfill(' ') << sections[i] << 
    std::endl;
  }

  for(int i = 0; i < symbols.size(); i++){
    Entry* e =  &table[symbols[i]];
    e->num = sections.size() + i;
    e->ndx = table[e->section].ndx;
    std::cout << 
    " " << std::left << std::setw(7) << std::setfill(' ') << e->num <<
    " " << std::left << std::setw(8) << std::setfill(' ') << type_str[e->type] << 
    " " << std::left << std::setw(8) << std::setfill(' ') << bind_str[e->bind] << 
    " x" << std::right << std::setw(8) << std::setfill('0') << std::hex << e->value << std::dec <<  
    " " << std::left << std::setw(8) << std::setfill(' ') << e->size << 
    " " << std::left << std::setw(7) << std::setfill(' ');

    if(e->ndx == 0)std::cout << "UND";
    else std::cout << e->ndx;

    std::cout << 
    " " << std::left << std::setw(10) << std::setfill(' ') << (e->is_extern ? "true" : "false")<< 
    " " << std::left << std::setw(11) << std::setfill(' ') << (e->is_defined ? "true" : "false")<< 
    " " << std::left << std::setw(8) << std::setfill(' ') << symbols[i] << 
    std::endl;
  }

}
bool SymbolTable::doesSymbolExist(std::string* name) const{
  return table.count(*name) > 0;
};


// SymbolTable:: Status SymbolTable::end(){
//   // close current section
//   table[current_section].size = LC;

//   return Status::OK;
// };


// SymbolTable::Status SymbolTable::incLC(uint32_t val){
//   if(current_section == 0 && val > 0){
//     std::cerr << "Undefined section for symbol." << std::endl;
//     return Status::ERROR;
//   }
//   LC += val;
//   return Status::OK;
// };

// uint32_t SymbolTable::getLC(){
//   return LC;
// };