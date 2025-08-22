#include "../../inc/common/SymbolTable.hpp"
#include <fstream>
#include <iostream>


int main(){

  // READ
  SymbolTable symtab(false);
  std::ifstream inputBinary("output.o", std::ios::binary); // otvara fajl za pisanje

  if (!inputBinary.is_open()) {
        std::cerr << "assembler: error: can't open input file\n";
        return -1;
  }

  symtab.loadTableFromFile(inputBinary);
  // symtab.printTable(inputBinary);
  // rel.print(inputBinary, &symtab);
  // printCode(inputBinary);

  inputBinary.close(); 

  return 0;
}