#pragma once
#include <cstdint>



static const uint8_t SP = 14;
static const uint8_t PC = 15;


typedef struct Instruction{ // 4B = 32b
  

  enum OPCode : uint8_t{
    HALT = 0x00,
    INT = 0x10,
    CALL_REG = 0x20,
    CALL_IND = 0x21,
    
    JMP_REG_DIR_DISP = 0x30,
    BEQ_REG_DIR_DISP = 0x31,
    BNE_REG_DIR_DISP = 0x32,
    BGT_REG_DIR_DISP = 0x33,
    JMP_REG_IND_DISP = 0x38,
    BEQ_REG_IND_DISP = 0x39,
    BNE_REG_IND_DISP = 0x3A,
    BGT_REG_IND_DISP = 0x3B,

    XCHG = 0x40,
    ADD = 0x50, 
    SUB = 0x51, 
    MUL = 0x52, 
    DIV = 0x53,

    NOT = 0x60,
    AND = 0x61, 
    OR = 0x62,
    XOR = 0x63,

    SHL = 0x70,
    SHR = 0x71,

    CSRRD = 0x90,
    CSRWR = 0x94

  } ;
  OPCode oc; // 4b op and 4b mod 
  uint8_t ra = 0; // 4b
  uint8_t rb = 0; // 4b
  uint8_t rc = 0; // 4b
  uint16_t disp = 0; // 12b signed
  Instruction(OPCode oc, uint8_t ra = 0, uint8_t rb = 0, uint8_t rc = 0, uint16_t disp = 0)
  : oc(oc), ra(ra), rb(rb), rc(rc), disp(disp) 
  {

  }

}Instruction;
