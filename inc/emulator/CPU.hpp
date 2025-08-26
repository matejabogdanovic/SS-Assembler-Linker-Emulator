#pragma once
#include "../common/Instruction.hpp"
#include <cstdint>
#include <map>

class CPU {

public:

  enum GPR : uint8_t{
    R0 = 0, R1, R2, R3, R4, R5, R6, 
    R7, R8, R9, R10, 
    R11, R12, R13, R14, R15, 
    SP = R14,
    PC = R15
  };
  
  inline void writeGpr(GPR gpr, uint32_t value){
    if(gpr!=R0)
      regfile[gpr] = value;
  }
  inline uint32_t readGpr(GPR gpr){
    return regfile[gpr];
  }
  static inline GPR A(uint8_t xx){
    return (GPR)((xx >> 4)&0x0f);
  }

  static inline  GPR B(uint8_t xx){
    return (GPR)(xx & 0x0f);
  }

  static inline  GPR C(uint8_t xx){
    return A(xx);
  }

  inline void nextPC(){
    regfile[PC] += WORD_SZ;
  }

  inline uint32_t getPC(){
    return regfile[PC];
  }



  void print();


private:

 

  uint32_t regfile[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x40000000};
};

