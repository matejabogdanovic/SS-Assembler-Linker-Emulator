#pragma once
#include "../../inc/common/RelTable.hpp"
#include "../../inc/common/Memory.hpp"
#include <fstream>
#include <iostream>

class Linker{  
public:

  struct FileState{
    SymbolTable symtab;
    RelTable rel;
    Memory memory;
  };

  static std::vector<FileState> files;
  static std::vector<std::string> file_names;
  static int start(int argc, char* argv[]);
private:
  static int parseArguments(int argc, char* argv[]);
  static int processing();
};