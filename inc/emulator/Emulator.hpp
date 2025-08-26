#pragma once
#include "../common/Memory.hpp"
#include <fstream>
#include <iostream>
class Emulator{
  
public:
  static int start(int argc, char* argv[]);

private:
  static void loadFromFile(std::istream& is);
};