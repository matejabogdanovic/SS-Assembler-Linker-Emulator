#pragma once
#include "../common/Instruction.hpp"
#include <cstdint>
#include <map>
#include <iomanip>
#include <iostream>

class CPU {

public:

  enum GPR : uint8_t{
    R0 = 0, R1, R2, R3, R4, R5, R6, 
    R7, R8, R9, R10, 
    R11, R12, R13, R14, R15, 
    SP = R14,
    PC = R15
  };
  enum CSR : uint8_t{
    STATUS = 0, 
    HANDLER = 1,
    CAUSE = 2
  };
  enum Interrupt_T : uint8_t{
    NOT_INTERRUPTED = 0x0,
    INVALID_INSTRUCTION = 0x1,
    TIMER = 0x2,
    TERMINAL = 0x3,
    SOFTWARE = 0x4
  };
  enum InterruptMask : uint8_t{
    TIMER_MASK = 0x1,
    TERMINAL_MASK = 0x2, 
    GLOBAL_MASK = 0x4
  };
  // 1 => masked
  inline void maskInterrupt(InterruptMask mask){
    this->writeCsr(STATUS, this->readCsr(STATUS) | mask);
  }
  inline bool isMaskedInterrupt(InterruptMask mask){
    return this->readCsr(STATUS) & mask;
  }
  inline Interrupt_T isInterrupted(){
    if(this->isMaskedInterrupt(GLOBAL_MASK))return NOT_INTERRUPTED;
    // check for timer, terminal
    // check prioritys, change them dinamically
    return NOT_INTERRUPTED; // when timer and terminal are implemented
  }

  inline void writeGpr(GPR gpr, uint32_t value){
    if(gpr!=R0)
      regfile[gpr] = value;
  }
  inline uint32_t readGpr(GPR gpr){
    return regfile[gpr];
  }

  inline void writeCsr(CSR csr, uint32_t value){
    csrfile[csr] = value;
  }
  inline uint32_t readCsr(CSR csr){
    return csrfile[csr];
  }//0 0 0 f
  static inline int32_t disp(uint8_t cd, uint8_t dd){
    int32_t sd__ = ((int16_t)(((int16_t)(cd & 0x0f))<<12)>>4) ;
    int32_t __dd = 0x0000 | dd; 
    return (int32_t)(sd__ | __dd);
  }
  
  static inline GPR A(uint8_t xx){
    return (GPR)((xx >> 4)&0x0f);
  }
  static inline GPR B(uint8_t xx){
    return (GPR)(xx & 0x0f);
  }
  static inline GPR C(uint8_t xx){
    return A(xx);
  }
  static inline CSR csr_A(uint8_t xx){
    uint8_t a = ((xx >> 4)&0x0f) - 1;
    if(a < 0 || a > 2){
      std::cerr << "Invalid CSR register." << std::endl;
      exit(-1);
    }
    return (CSR)a;
  }
  static inline CSR csr_B(uint8_t xx){
    uint8_t b = (xx & 0x0f) - 1;
    if(b < 0 || b > 2){
      std::cerr << "Invalid CSR register." << std::endl;
      exit(-1);
    }
    return (CSR)b;
  }
  
  inline void nextPC(){
    regfile[PC] += WORD_SZ;
  }

  inline uint32_t getPC(){
    return regfile[PC];
  }



  void printGpr();
  void printCsr();

private:


  uint32_t csrfile [3] = {0, 0, 0};
  uint32_t regfile[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x40000000};
};

