#include "../../inc/emulator/Emulator.hpp"
char* Emulator::input;
EmulatedMemory Emulator::memory;
int Emulator::start(int argc, char* argv[]){
  // parse arguments
  if(argc<2){
    std::cerr << "No arguments." << std::endl;
    return -1;
  }
  input = argv[1];

  if(processing()<0)return -2;

  return 0;
}

int Emulator::processing(){
  LOG(std::cout << "EMULATOR\n";)
  std::ifstream inputFileBinary(std::string(input)+".bin", std::ios::binary);

  if (!inputFileBinary.is_open()) {
    std::cerr << "assembler: error: can't open output file\n";
    return -1;
  }

  memory.loadFromFile(inputFileBinary);

  inputFileBinary.close(); 
  return 0;
}

