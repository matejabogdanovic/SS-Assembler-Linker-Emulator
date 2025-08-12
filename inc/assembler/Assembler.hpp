#pragma once
#include "../../inc/assembler/SymbolTable.hpp"

class Assembler {
  
public:
  static int start(int argc, char* argv[]);

  static void handleLabel(std::string* name);
  static void handleSection(std::string* name);
  static void handleGlobal(std::string* name);
  static void handleExtern(std::string* name);
  static void handleSkip(uint32_t size);
  static void handleWordLiteral(uint32_t value);
  static void handleWordSymbol(std::string* name);
  static void handleEnd();

  typedef enum {
    OK,
    ERROR
  } Status;



private:
  static void closeSection();
  static void handleSymbolDefinition(std::string* name);
  static void handleSymbolDeclaration(std::string* name);
  static void handleLiteral();

  static int parseArguments(int argc, char* argv[]);
  static int processing();
  static char* input; 
  static char* output;
  static bool finished;


  static SymbolTable symtab;
  static uint32_t LC;
};