#pragma once
#include "../../inc/common/RelTable.hpp"
#include "../../inc/common/Memory.hpp"
#include <fstream>
#include <iostream>
#include <list>
#include <map>

class Linker{  
public:

  struct FileState{
    SymbolTable symtab;
    RelTable rel;
    Memory memory;
  };
  struct Section{
    Memory* memory;
    SymbolTable::Entry* section;
    SymbolTable* symtab;
  };

  struct SectionsUnion{
    std::string name = ""; 
    uint32_t start_address = 0;
    uint64_t size = 0;
    std::vector <Section> sections; // aggregate
    std::vector <RelTable*> rels; 
    
  };

  static std::list <SectionsUnion> map;
  static std::map<std::string, SymbolTable::Entry*> defined_syms;
  static std::list<FileState> files;
  static std::vector<std::string> file_names;
  static int start(int argc, char* argv[]);
private:
  static int parseArguments(int argc, char* argv[]);
  static int processing();
  static int loadData();

  static void createMap();

  static void linking();
};