#pragma once
#include "../../inc/common/SymbolTable.hpp"
#include "../../inc/common/Memory.hpp"
#include "../../inc/common/Macros.hpp"
#include "../../inc/assembler/LiteralPool.hpp"

#include <list>



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


  static void handleZeroArgsInstructions(Instruction::OPCode op);
  static void handleJustLiteralInstructions(Instruction::OPCode op, uint32_t value);
  static void handleJustSymbolInstructions(Instruction::OPCode op, std::string* name);
  static void handleGprInstructions(Instruction::OPCode op,uint8_t gprS, uint8_t gprD);
  static void handleStackInstructions(Instruction::OPCode op, uint8_t reg);
  typedef enum {
    OK,
    ERROR
  } Status;



private:
  typedef struct Backpatch{
    uint32_t location;
    SymbolTable::Entry* symbol;
    SymbolTable::Entry* section;
    Instruction alternative; // reg dir
    
    Backpatch(
      uint32_t location,
    SymbolTable::Entry* symbol,
    SymbolTable::Entry* section,
    Instruction alternative = {Instruction::OPCode::HALT}
    ): location(location), symbol(symbol), section(section), alternative(alternative)
    {
      
    }
  }Backpatch;


  // static std::vector<uint8_t> memory;
  static Memory memory;


  static LiteralPool literalPool;
  static std::list <Backpatch> backpatch;
  static void symbolBackpatch();
  static void literalBackpatch();


  static void closeSection();
  static void handleSymbolDefinition(std::string* name);
  static void handleSymbolUsage(std::string* name);
  static void handleLiteral();

  static int parseArguments(int argc, char* argv[]);
  static int processing();

  static void printCode(std::ostream& os);

  static char* input; 
  static char* output;
  static bool finished;


  static SymbolTable symtab;
  static uint32_t LC;
};