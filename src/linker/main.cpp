
#include "../../inc/common/RelTable.hpp"
#include "../../inc/common/Memory.hpp"
#include <fstream>
#include <iostream>


int main(){

  // READ
  SymbolTable symtab(false);
  RelTable rel;
  Memory memory;
  
  std::ifstream inputBinary("output.o", std::ios::binary); // otvara fajl za pisanje

  if (!inputBinary.is_open()) {
        std::cerr << "assembler: error: can't open input file\n";
        return -1;
  }

  symtab.loadFromFile(inputBinary);
  symtab.print(std::cout);
  rel.loadFromFile(inputBinary, &symtab);
  rel.print(std::cout, &symtab);
  memory.loadFromFile(inputBinary);
  memory.printCode(std::cout, &symtab);


  inputBinary.close(); 

  return 0;
}