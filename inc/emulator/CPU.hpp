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
    return (GPR)(xx & GPRA);
  }

  static inline  GPR B(uint8_t xx){
    return (GPR)(xx & GPRB);
  }

  static inline  GPR C(uint8_t xx){
    return (GPR)(xx & GPRC);
  }

  inline void nextPC(){
    regfile[PC] += WORD_SZ;
  }

  inline uint32_t getPC(){
    return regfile[PC];
  }


  static void i_not(CPU& cpu, uint8_t ab, uint8_t co, uint8_t oo);
  static void i_add(CPU& cpu, uint8_t ab, uint8_t co, uint8_t oo);
  static void i_halt(CPU& cpu, uint8_t ab, uint8_t co, uint8_t oo);

  void print();


  typedef  void(*InstructionPointer)(CPU&, uint8_t, uint8_t, uint8_t);
  static std::map<uint8_t, InstructionPointer> instructions;
private:
  static const uint8_t GPRA = 0x0f; 
  static const uint8_t GPRB = 0xf0;
  static const uint8_t GPRC = 0x0f; 
 

  uint32_t regfile[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x40000000};
};

