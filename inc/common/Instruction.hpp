#pragma once
#include <cstdint>



static const uint8_t SP = 14;
static const uint8_t PC = 15;
static const uint8_t STATUS = 1;
static const uint8_t HANDLER = 2;
static const uint8_t CAUSE = 3;
static const uint8_t WORD_SZ = 4;
typedef struct Instruction{ // 4B = 32b
  // status = 1;
  // handler = 2;
  // cause = 3;

  enum OPCode : uint8_t{
    HALT = 0x00,
    INT = 0x10,
    CALL_REG = 0x20,
    CALL_IND = 0x21,
    IRET = 0xF0, // not real code
    RET = 0xF1, // not real code
    JMP_REG_DIR_DISP = 0x30,
    BEQ_REG_DIR_DISP = 0x31,
    BNE_REG_DIR_DISP = 0x32,
    BGT_REG_DIR_DISP = 0x33,
    JMP_REG_IND_DISP = 0x38,
    BEQ_REG_IND_DISP = 0x39,
    BNE_REG_IND_DISP = 0x3A,
    BGT_REG_IND_DISP = 0x3B,

    PUSH = 0x81,
    POP = 0x93,
    POP_CSR = 0x97,
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
    CSRWR = 0x94,

    LD_VLIT = 0xE0, // not real code
    LD_VSYM = 0xE1, // not real code
    LD_LIT = 0xE2, // not real code
    LD_SYM = 0xE3, // not real code
    LD_REG = 0xE4, // not real code
    LD_IND_REG = 0xE5, // not real code
    LD_IND_REG_LIT = 0xE6, // not real code
    // LD_IND_REG_SYM = 0xE7, // not real code

    LD_TO_GPR_REG_DIR_DISP = 0x91,
    LD_TO_GPR_REG_IND_DISP = 0x92,

    // ST_VLIT = 0xD0, // not real code
    // ST_VSYM = 0xD1, // not real code
    ST_LIT = 0xD2, // not real code
    ST_SYM = 0xD3, // not real code
    ST_REG = 0xD4, // not real code
    ST_IND_REG = 0xD5, // not real code
    ST_IND_REG_LIT = 0xD6, // not real code
    // ST_IND_REG_SYM = 0xD7, // not real code

    ST_MEM_DIR = 0x80,
    ST_MEM_IND = 0x82,
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



