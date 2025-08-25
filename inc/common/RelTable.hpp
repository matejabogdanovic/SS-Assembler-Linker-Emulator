#pragma once
#include "SymbolTable.hpp"

class RelTable{
public:
  typedef enum RelType {
    T_GLOB=0x0,
    T_LOC=0x1
  }RelType;

  typedef struct Entry{
    uint32_t offset;
    RelType type;
    SymbolTable::Entry* symbol;
    SymbolTable::Entry* section;
    uint32_t addend;

    Entry(uint32_t offset,
    SymbolTable::Entry* symbol,
    SymbolTable::Entry* section,
    RelType type = T_GLOB,
    uint32_t addend = 0
   ):
    offset(offset), symbol(symbol), section(section), 
     type(type), addend(addend)
    {

    }
  }Entry;
  void loadFromFile(std::istream& is, SymbolTable* symtab);
  void printBinary(std::ostream& os);
  void put(RelTable::Entry entry);
  void print(std::ostream& os, SymbolTable* symtab);
  std::vector<Entry> table; 
private:
  static const char* relType_str[];
   
};

// Offset Type Symbol Section Addend
// 00000014 R_X86_64_32 1 (.text) 14
// 0000001c R_X86_64_32S 8 (b) 0
// 00000024 R_X86_64_32S 7 (c) 0
// 0000002c R_X86_64_32S 2 (.data) 8