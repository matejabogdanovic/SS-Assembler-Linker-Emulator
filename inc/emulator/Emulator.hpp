#pragma once
#include "./EmulatedMemory.hpp"
#include "./CPU.hpp"

#include <fstream>
#include <iostream>
class Emulator{
  
public:
  static int start(int argc, char* argv[]);
  
private:

  static void i_not(CPU& cpu, uint8_t ab, uint8_t co, uint8_t oo);
  static void i_add(CPU& cpu, uint8_t ab, uint8_t co, uint8_t oo);
  static void i_halt(CPU& cpu, uint8_t ab, uint8_t co, uint8_t oo);

  typedef  void(*InstructionPointer)(CPU&, uint8_t, uint8_t, uint8_t);
  static std::map<uint8_t, InstructionPointer> instructions;


  static int emulation();
  static int processing();
  static EmulatedMemory memory;
  static char* input;

  static CPU cpu;
};