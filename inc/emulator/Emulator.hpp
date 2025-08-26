#pragma once
#include "./EmulatedMemory.hpp"

#include <fstream>
#include <iostream>
class Emulator{
  
public:
  static int start(int argc, char* argv[]);

private:

  static int processing();
  static EmulatedMemory memory;
  static char* input;
};