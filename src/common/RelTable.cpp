#include "../../inc/common/RelTable.hpp"
#include <iostream>
#include <iomanip>
const char* RelTable::relType_str[] = {"T_GLOB", "T_LOC"};


void RelTable::put(RelTable::Entry entry){
  table.push_back(entry);
}

void RelTable::printBinary(std::ostream& os){
  auto num_of_entries = table.size();
 
  os.write(reinterpret_cast<const char*>(& num_of_entries), sizeof(num_of_entries));

  for(int i = 0; i < num_of_entries; i++){
    Entry* e = &table[i]; 
    os.write(reinterpret_cast<const char*>(&e->offset), sizeof(e->offset));
    os.write(reinterpret_cast<const char*>(&e->type), sizeof(e->type));
    os.write(reinterpret_cast<const char*>(&e->symbol->num), sizeof(e->symbol->num));
    os.write(reinterpret_cast<const char*>(&e->section->num), sizeof(e->section->num));
    os.write(reinterpret_cast<const char*>(&e->addend), sizeof(e->addend));
  }

}

void RelTable::loadFromFile(std::istream& is, SymbolTable* symtab){
  size_t num_of_entries;
  is.read(reinterpret_cast<char*>(& num_of_entries), sizeof(num_of_entries));
  
  for(int i = 0; i < num_of_entries; i++){
    Entry e = {0, nullptr, nullptr};
    is.read(reinterpret_cast<char*>(&e.offset), sizeof(e.offset));
    is.read(reinterpret_cast<char*>(&e.type), sizeof(e.type));

    uint32_t sym_num;
    is.read(reinterpret_cast<char*>(&sym_num), sizeof(SymbolTable::Entry::num));

    if(e.type != T_LOC)    e.symbol = symtab->getSymbol(&symtab->symbol_names[sym_num]);
    else e.symbol = symtab->getSection(&symtab->section_names[sym_num]);

    uint32_t sect_num;
    is.read(reinterpret_cast<char*>(&sect_num), sizeof(SymbolTable::Entry::num));
    e.section = symtab->getSection(&symtab->section_names[sect_num]);

    is.read(reinterpret_cast<char*>(&e.addend), sizeof(e.addend));
    
    put(e);
  }

}

void RelTable::print(std::ostream& os, SymbolTable* symtab){
  os << "| Offset |"
  << " Type |"
  << " Symbol |"
  << " Section |"
  << " Addend |" << std::endl;
  for (size_t i = 0; i < table.size(); i++)
  {
    Entry* e = &table[i];
    os <<" "<< std::right << std::uppercase<< std::setw(8) << std::setfill('0') << std::hex <<
       e->offset << " "
      << std::dec << std::left << std::uppercase << std::setw(6) << std::setfill(' ') << 
      relType_str[e->type] << " " << std::left << std::uppercase << std::setw(8) << std::setfill(' ')<<  
      (e->type != RelTable::T_LOC ? symtab->getSymbolName(e->symbol) : symtab->getSectionName(e->symbol) )<< " "
       
      << std::uppercase<< std::setw(9) << std::setfill(' ')<< 
      symtab->getSectionName(e->section) << " "
      << e->addend << " \n";
      
  }
  
}