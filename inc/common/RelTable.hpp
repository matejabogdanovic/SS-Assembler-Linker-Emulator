#pragma once
#include "SymbolTable.hpp"

class RelTable{
public:
  typedef enum RelType {
    Type1=0x0
  }RelType;

  typedef struct Entry{
    uint32_t offset;
    RelType type;
    SymbolTable::Entry* symbol;
    bool symbol_global;
    SymbolTable::Entry* section;
    uint32_t addend;

    Entry(uint32_t offset,
    SymbolTable::Entry* symbol,
    SymbolTable::Entry* section,
    bool symbol_global = true,
    uint32_t addend = 0,
    RelType type = Type1
   ):
    offset(offset), symbol(symbol), section(section), 
    symbol_global(symbol_global),
     addend(addend), type(type)
    {

    }
  }Entry;
  

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