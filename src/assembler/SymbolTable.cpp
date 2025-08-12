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

  this->sections[""] = Entry (0, Bind::LOC, "", 0, Type::SCTN);
  this->section_names.push_back("");
}
bool SymbolTable::isDefined(uint8_t flags) {
  return flags & (Flags::DEFINED);
}
bool SymbolTable::isExtern(uint8_t flags){
  return flags & (Flags::EXTERN);
}
bool SymbolTable::isAbsolute(uint8_t flags){
  return flags & (Flags::ABSOLUTE);
}

bool SymbolTable::doesSymbolExist(std::string* name) const{
  return symbols.count(*name) > 0;
};

bool SymbolTable::doesSectionExist(std::string* name) const{
  return sections.count(*name) > 0;
};

std::string SymbolTable::getCurrentSectionName() const{
  return section_names[current_section];
}
std::string SymbolTable::getUndefinedSectionName() const{
  return section_names[0];
}
bool SymbolTable::sectionOpened() const{
    return current_section != 0;
}

SymbolTable::Entry* SymbolTable::getCurrentSection(){
  return &sections[getCurrentSectionName()];
}

void SymbolTable::addSymbol(std::string* name, Entry e){
  symbols[*name] = e;
  symbol_names.push_back(*name);
}

void SymbolTable::addSection(std::string* name, Entry e){
  sections[*name] = e;
  current_section = section_names.size();
  section_names.push_back(*name);
}

SymbolTable::Entry* SymbolTable::getSymbol(std::string* name){
  return &symbols[*name];
}
SymbolTable::Entry* SymbolTable::getSection(std::string* name){
  return &sections[*name];
}



void SymbolTable::printTable(){
  
  std::cout << 
    "|  Num  " << 
    "|  Type  "<< 
    "|  Bind  " << 
    "|  Offset  " << 
    "|  Size  " << 
    "|  Ndx  "<< 
    "|  Flags  " << 
    "|  Name  |" << 
    std::endl;

    
  for(int i = 0; i < section_names.size(); i++){
    Entry* e =  &sections[section_names[i]];
    e->num = i;
    e->ndx = i;
    printTablePart(&section_names[i], e);
  }

  for(int i = 0; i < symbol_names.size(); i++){
    Entry* e =  &symbols[symbol_names[i]];
    e->num = section_names.size() + i;
    e->ndx = symbols[e->section].ndx;
    printTablePart(&symbol_names[i], e);
  }

}


void SymbolTable::printTablePart(std::string* name, Entry* e) const {
  
      std::cout << 
    " " << std::left << std::setw(7) << std::setfill(' ') << e->num <<
    " " << std::left << std::setw(8) << std::setfill(' ') << type_str[e->type] << 
    " " << std::left << std::setw(8) << std::setfill(' ') << bind_str[e->bind] << 
    " 0x" << std::right << std::setw(8) << std::setfill('0') << std::hex << e->offset << std::dec <<  
    " " << std::left << std::setw(8) << std::setfill(' ') << e->size << 
    " " << std::left << std::setw(7) << std::setfill(' ');

    if(e->ndx == 0)std::cout << "UND";
    else std::cout << e->ndx;

     std::cout << 
   
    " " << std::left << std::setw(1) << std::setfill(' ') << (isDefined(e->flags) ? "d":" ") << 
    std::left << std::setw(1) << std::setfill(' ') << (isExtern(e->flags) ? "e":" ") <<  
    std::left << std::setw(7) << std::setfill(' ') << (isAbsolute(e->flags) ? "a":" ")<< 
    " " << std::left << std::setw(8) << std::setfill(' ') << *name << 
    std::endl;
}
