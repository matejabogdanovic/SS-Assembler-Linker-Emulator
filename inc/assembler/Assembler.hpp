#pragma once
#include <string>
#include "../../inc/assembler/SymbolTable.hpp"

class Assembler {
public:
  static void handleLabel(std::string* name);
  static void handleSection(std::string* name);
  static void handleSkip(int32_t size);
  static void handleEnd();

  static bool finished;
};