#include "../../inc/common/SymbolTable.hpp"
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

  this->sections[""] = Entry (0, Bind::LOC, 0, 0, Type::SCTN);
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


void SymbolTable::addSymbol(std::string* name, Entry e){
  symbols[*name] = e;
  e.num = symbol_names.size(); // to get my name
  symbol_names.push_back(*name);
}

void SymbolTable::addSection(std::string* name, Entry e){
  current_section = section_names.size();
  e.num = current_section; 
  e.ndx = current_section;
  sections[*name] = e;
  section_names.push_back(*name);
}


SymbolTable::Entry* SymbolTable::getSymbol(std::string* name){
  return &symbols[*name];
}

SymbolTable::Entry* SymbolTable::getSection(std::string* name){
  return &sections[*name];
}

std::string SymbolTable::getSymbolName(Entry* e) const{
  return symbol_names[e->num];
}

std::string SymbolTable::getSectionName(Entry* e) const{
  return section_names[e->ndx];
}

uint32_t SymbolTable::getSectionStart(uint32_t ndx){
  
  uint32_t sz = 0;
  SymbolTable::Entry* curr;
  for(int i = 0; i < section_names.size() ; i++){
    curr =  &sections[section_names[i]];
    if(curr->ndx == ndx){
      
        return sz;
    }
    sz += curr->size;

  }
  return 0;
}

// for assembler, linker won't need -----

bool SymbolTable::sectionOpened() const{
    return current_section != 0;
}

std::string SymbolTable::getCurrentSectionName() const{
  return section_names[current_section];
}

std::string SymbolTable::getUndefinedSectionName() const{
  return section_names[0];
}

SymbolTable::Entry* SymbolTable::getCurrentSection(){
  return &sections[getCurrentSectionName()];
}
// --------------------------------------



void SymbolTable::printTable(std::ostream& os){
  
  os << 
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
   // e->num = i;
    //e->ndx = i;
    printTablePart(&section_names[i], e, os);
  }

  for(int i = 0; i < symbol_names.size(); i++){
    Entry* e =  &symbols[symbol_names[i]];
    e->num = section_names.size() + i; // correct index (from sections)
    // e->ndx = e->ndx+1;
    printTablePart(&symbol_names[i], e, os);
  }

}


void SymbolTable::printTablePart(std::string* name, Entry* e, std::ostream& os) const {
  
      os<< 
    " " << std::left << std::setw(7) << std::setfill(' ') << e->num <<
    " " << std::left << std::setw(8) << std::setfill(' ') << type_str[e->type] << 
    " " << std::left << std::setw(8) << std::setfill(' ') << bind_str[e->bind] << 
    " 0x" << std::right << std::setw(8) << std::setfill('0') << std::hex << e->offset << std::dec <<  
    " " << std::left << std::setw(8) << std::setfill(' ') << e->size << 
    " " << std::left << std::setw(7) << std::setfill(' ');

    if(e->ndx == 0)os<< "UND";
    else os<< e->ndx;

     os << 
   
    " " << std::left << std::setw(1) << std::setfill(' ') << (isDefined(e->flags) ? "d":" ") << 
    std::left << std::setw(1) << std::setfill(' ') << (isExtern(e->flags) ? "e":" ") <<  
    std::left << std::setw(7) << std::setfill(' ') << (isAbsolute(e->flags) ? "a":" ")<< 
    " " << std::left << std::setw(8) << std::setfill(' ') << *name << 
    std::endl;
}
