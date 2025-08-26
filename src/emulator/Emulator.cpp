#include "../../inc/emulator/Emulator.hpp"


int Emulator::start(int argc, char* argv[]){
  if(argc<2){
    std::cerr << "No arguments." << std::endl;
    return -1;
  }
  std::cout << "EMULATOR\n";
  std::ifstream inputFileBinary(std::string(argv[1])+".bin", std::ios::binary);

  if (!inputFileBinary.is_open()) {
    std::cerr << "assembler: error: can't open output file\n";
    return -1;
  }

  loadFromFile(inputFileBinary);

  inputFileBinary.close(); 

  return 0;
}

void Emulator::loadFromFile(std::istream& is){
  uint32_t start_address;
  uint32_t size;
 
  while(is.read(reinterpret_cast<char*>(&start_address), sizeof(start_address))){
    std::cout << "REAL " << std::hex << start_address << std::dec << ": \n";
    Memory mem;
    mem.loadFromFile(is);
    mem.print(std::cout, 0, 0, start_address);
  }

  
}