#pragma once
#include <list>

#include "../../inc/common/SymbolTable.hpp"
#include "../../inc/common/Memory.hpp"

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

  static void handleHalt();
  static void handleInt();
  static void handleCallLiteral(uint32_t value);
  typedef enum {
    OK,
    ERROR
  } Status;



private:
  typedef struct {
    uint32_t location;
    SymbolTable::Entry* symbol;
    SymbolTable::Entry* section;
  }Backpatch;


  // static std::vector<uint8_t> memory;
  static Memory memory;

  typedef struct {
    uint32_t location;
    uint32_t index_of_literal;
  }LiteralPatch;

  static std::unordered_map <uint32_t, uint32_t> literalTable;
  static std::list <LiteralPatch> literalpatch;
  static std::vector <uint32_t> literalPool;
  static std::list <Backpatch> backpatch;
  static void startBackpatch();



  static void closeSection();
  static void handleSymbolDefinition(std::string* name);
  static void handleSymbolUsage(std::string* name);
  static void handleLiteral();

  static int parseArguments(int argc, char* argv[]);
  static int processing();
  static char* input; 
  static char* output;
  static bool finished;


  static SymbolTable symtab;
  static uint32_t LC;
};