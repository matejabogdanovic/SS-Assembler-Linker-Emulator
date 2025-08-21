#include "../../inc/common/RelTable.hpp"
#include <iostream>
#include <iomanip>
const char* RelTable::relType_str[] = {"Type1"};


void RelTable::put(RelTable::Entry entry){
  table.push_back(entry);
}


void RelTable::print(std::ostream& os, SymbolTable* symtab){
  os << "| Offset |"
  << " Type |"
  << " Symbol |"
  << " symbol_global |"
  << " Section |"
  << " Addend |" << std::endl;
  for (size_t i = 0; i < table.size(); i++)
  {
    Entry* e = &table[i];
    os <<" "<< std::right << std::uppercase<< std::setw(8) << std::setfill('0') << std::hex <<
       e->offset << " "
      << std::dec << std::left << std::uppercase << std::setw(6) << std::setfill(' ') << 
      relType_str[e->type] << " " << std::left << std::uppercase << std::setw(8) << std::setfill(' ')<<  
      (e->symbol_global ? symtab->getSymbolName(e->symbol) : symtab->getSectionName(e->symbol) )<< " "
       << std::uppercase<< std::setw(15) << std::setfill(' ')<< 
      e->symbol_global << " "<< std::uppercase<< std::setw(9) << std::setfill(' ')<<
      symtab->getSectionName(e->section) << " "
      << e->addend << " \n";
      
  }
  
}