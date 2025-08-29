#pragma once
#include "../../inc/common/RelTable.hpp"
#include "../../inc/common/Memory.hpp"
#include "../../inc/common/Macros.hpp"
#include "../../inc/assembler/LiteralPool.hpp"
#include "../../inc/assembler/AssemblerException.hpp"
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
  static void handleBranchLiteralInstructions(Instruction::OPCode op, uint8_t gpr1, uint8_t gpr2, uint32_t value);
  static void handleBranchSymbolInstructions(Instruction::OPCode op, uint8_t gpr1, uint8_t gpr2, std::string* name);
 
  static void handleLoadLiteral(Instruction::OPCode op, uint32_t value , uint8_t reg, uint8_t gprS = 0 );
  static void handleLoadRegisters(Instruction::OPCode op, uint8_t gprD, uint8_t gprS);
  static void handleLoadSymbol(Instruction::OPCode op,  std::string* name , uint8_t gprD, uint8_t gprS = 0 );

  static void handleStoreLiteral(Instruction::OPCode op, uint32_t value, uint8_t gprData, uint8_t gprS = 0);
  static void handleStoreRegisters(Instruction::OPCode op, uint8_t gprData, uint8_t gprS);
  static void handleStoreSymbol(Instruction::OPCode op,  std::string* name, uint8_t gprData);
  
  
  typedef enum {
    OK,
    ERROR
  } Status;

  static void handleSyntaxError();

private:
  typedef struct Backpatch{
    
    
    uint32_t location;
    SymbolTable::Entry* symbol;
    SymbolTable::Entry* section;
  
    
    Backpatch(
      uint32_t location,
      SymbolTable::Entry* symbol,
      SymbolTable::Entry* section

    ): location(location), symbol(symbol), section(section)
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

  static int parseArguments(int argc, char* argv[]);
  static int processing();


  static char* input; 
  static char* output;
  static bool finished;


  static SymbolTable symtab;
  static RelTable rel;
  static uint32_t LC;
};