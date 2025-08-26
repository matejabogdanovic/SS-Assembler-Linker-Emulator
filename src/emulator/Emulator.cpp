#include "../../inc/emulator/Emulator.hpp"
char* Emulator::input;
CPU Emulator::cpu;
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


  emulation();


  return 0;
}


int Emulator::emulation(){
    
  uint8_t op = 1;
  uint8_t ab = 2;
  uint8_t co = 3;
  uint8_t oo = 4;
  while(op!=Instruction::HALT){
    op = memory.readByte(cpu.getPC());
    ab = memory.readByte(cpu.getPC()+1);
    co = memory.readByte(cpu.getPC()+2);
    oo = memory.readByte(cpu.getPC()+3);
    cpu.nextPC();
    if(cpu.instructions.count(op) == 0){
      std::cerr << "Invalid instruction: PC = 0x" << std::hex <<  cpu.getPC();
      exit(-1);
    }

    CPU::instructions[op](cpu, ab, co, oo);
    switch (op){
      case Instruction::HALT:
        std::cout << "HALT"<<std::endl;
        break;
      default:
        break;
    }
  }
  cpu.print();
  return 0;
}
