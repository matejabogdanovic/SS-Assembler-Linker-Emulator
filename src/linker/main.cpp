
#include "../../inc/common/RelTable.hpp"
#include <fstream>
#include <iostream>


int main(){

  // READ
  SymbolTable symtab(false);
  RelTable rel;
  
  std::ifstream inputBinary("output.o", std::ios::binary); // otvara fajl za pisanje

  if (!inputBinary.is_open()) {
        std::cerr << "assembler: error: can't open input file\n";
        return -1;
  }

  symtab.loadFromFile(inputBinary);
  symtab.print(std::cout);
  rel.loadFromFile(inputBinary, &symtab);
  rel.print(std::cout, &symtab);

  
  // printCode(inputBinary);

  inputBinary.close(); 

  return 0;
}