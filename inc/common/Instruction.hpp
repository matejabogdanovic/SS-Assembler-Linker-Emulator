#pragma once
#include <cstdint>



typedef struct Instruction{ // 4B = 32b
  enum OPCode : uint8_t{
    HALT = 0x00,
    INT = 0x10,
    CALL_REG_DIR_DISP = 0x20,
    CALL_REG_IND_DISP = 0x21,
    
    JMP_REG_DIR_DISP = 0x30,
    BEQ_REG_DIR_DISP = 0x31,
    BNE_REG_DIR_DISP = 0x32,
    BGT_REG_DIR_DISP = 0x33,
    JMP_REG_IND_DISP = 0x38,
    BEQ_REG_IND_DISP = 0x39,
    BNE_REG_IND_DISP = 0x3A,
    BGT_REG_IND_DISP = 0x3B

  } ;
  OPCode oc; // 4b op and 4b mod 
  uint8_t ra; // 4b
  uint8_t rb; // 4b
  uint8_t rc; // 4b
  uint16_t disp; // 12b signed
}Instruction;