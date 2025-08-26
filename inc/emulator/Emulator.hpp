#pragma once
#include "./EmulatedMemory.hpp"
#include "./CPU.hpp"

#include <fstream>
#include <iostream>
class Emulator{
  
public:
  static int start(int argc, char* argv[]);
  
private:
  static int emulation();
  static int processing();
  static EmulatedMemory memory;
  static char* input;

  static CPU cpu;
};