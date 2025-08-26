#pragma once
#include "../../inc/linker/Sections.hpp"
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <unordered_map>
class Linker{  
public:


  static Sections sections;

  static std::map<std::string, uint32_t> defined_syms; // name, offset
  static std::unordered_map<std::string, uint32_t> section_starts; // name, start address
  static std::list<FileState> files;
  static std::vector<std::string> file_names;
  static int start(int argc, char* argv[]);
private:
  static int parseArguments(int argc, char* argv[]);
  static int processing();
  static int loadData();

  static void createSectionOrder();
  static void findDefinedSymbols();

  static void linking();
  static std::string output;
};