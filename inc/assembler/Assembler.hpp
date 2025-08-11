#pragma once
#include "../../inc/assembler/SymbolTable.hpp"

class Assembler {
  
public:
  static int start(int argc, char* argv[]);

  static void handleLabel(std::string* name);
  static void handleSection(std::string* name);
  static void handleGlobal(std::string* name);
  static void handleExtern(std::string* name);
  static void handleSkip(int32_t size);
  static void handleEnd();
  typedef enum {
    OK,
    ERROR
  } Status;


  // static int incLC(uint32_t val);
  // static uint32_t getLC();

private:
  static Status addSymbol(uint32_t value,
    SymbolTable::Type type,
    SymbolTable::Bind bind,
    std::string* name,
     bool is_defined);
  static Status addExternSymbol(std::string* name);
  static int parseArguments(int argc, char* argv[]);
  static int processing();
  static char* input; 
  static char* output;
  static bool finished;
  static bool sectionOpened();
  static uint32_t current_section;
  static SymbolTable symtab;
  static uint32_t LC;
};