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



// todo set psw
// void Emulator::i_not(CPU& cpu, uint8_t ab, uint8_t co, uint8_t oo){
//   CPU::GPR regA = CPU::A(ab), regB = CPU::B(ab);
//   std::cout << "not";
//   cpu.writeGpr(regA, ~cpu.readGpr(regB));
// }

// void Emulator::i_halt(CPU& cpu, uint8_t ab, uint8_t co, uint8_t oo){

// }

// void Emulator::i_add(CPU& cpu, uint8_t ab, uint8_t co, uint8_t oo){
//   CPU::GPR regA = CPU::A(ab), regB = CPU::B(ab), regC = CPU::C(co);
//   std::cout << "add";
//   cpu.writeGpr(regA, cpu.readGpr(regB) + cpu.readGpr(regC));
// }


void Emulator::handleGprInstructions(uint8_t ocm,  CPU::GPR gprA, CPU::GPR gprB, CPU::GPR gprC){
  uint32_t temp = 0;
  switch (ocm)
  {
    case Instruction::OPCode::XCHG: // has 2 destinations
      LOG(std::cout << "XCHG";)
      temp  = cpu.readGpr(gprB);
      cpu.writeGpr(gprB, cpu.readGpr(gprC));
      cpu.writeGpr(gprC, temp);
      break;
    case Instruction::OPCode::ADD:
      LOG(std::cout << "ADD";)
      cpu.writeGpr(gprA, cpu.readGpr(gprB) + cpu.readGpr(gprC));
      break;
    case Instruction::OPCode::SUB:
      LOG(std::cout << "SUB";)
      cpu.writeGpr(gprA, cpu.readGpr(gprB) - cpu.readGpr(gprC));
      break;
    case Instruction::OPCode::MUL:
      LOG(std::cout << "MUL";)
      cpu.writeGpr(gprA, cpu.readGpr(gprB) * cpu.readGpr(gprC));
      break;
    case Instruction::OPCode::DIV:
      LOG(std::cout << "DIV";)
      temp  =  cpu.readGpr(gprC);
      cpu.writeGpr(gprA, temp == 0?UINT32_MAX:cpu.readGpr(gprB) / temp);
      break;
    case Instruction::OPCode::NOT:
      LOG(std::cout << "NOT";)
      cpu.writeGpr(gprA, ~cpu.readGpr(gprB));
      break;
    case Instruction::OPCode::AND:
      LOG(std::cout << "AND";)
      cpu.writeGpr(gprA, cpu.readGpr(gprB) / cpu.readGpr(gprC));
      break;
    case Instruction::OPCode::OR:
      LOG(std::cout << "OR";)
      cpu.writeGpr(gprA, cpu.readGpr(gprB) | cpu.readGpr(gprC));
      break;
    case Instruction::OPCode::XOR:
      LOG(std::cout << "XOR";)
      cpu.writeGpr(gprA, cpu.readGpr(gprB) ^ cpu.readGpr(gprC));
      break;
    case Instruction::OPCode::SHL:
      LOG(std::cout << "SHL";)
      cpu.writeGpr(gprA, cpu.readGpr(gprB) << cpu.readGpr(gprC));
      break;
    case Instruction::OPCode::SHR:
      LOG(std::cout << "SHR";)
      cpu.writeGpr(gprA, cpu.readGpr(gprB) >> cpu.readGpr(gprC));
      break;
  

  default:
    std::cerr << "Invalid instruction: PC = 0x" << std::hex <<  cpu.getPC();
    exit(-1);
  }

  LOG(std::cout << " " << gprA << " " << gprB << " " << gprC << " \n";)

}

void Emulator::handleLoadStoreInstructions(uint8_t ocm, uint8_t ab, CPU::GPR gprA, CPU::GPR gprB, CPU::GPR gprC, int32_t disp){
  switch (ocm){
    case Instruction::OPCode::ST_MEM_DIR:
      LOG(std::cout << "ST_MEM_DIR";)
      memory.changeWord(cpu.readGpr(gprC), cpu.readGpr(gprA) + cpu.readGpr(gprB) + disp);
    break;

    case Instruction::OPCode::ST_MEM_IND:
      LOG(std::cout << "ST_MEM_IND";)
      memory.changeWord(cpu.readGpr(gprC) ,memory.readWord(cpu.readGpr(gprA) + cpu.readGpr(gprB) + disp));
    break;
    case Instruction::OPCode::PUSH:
      LOG(std::cout << "PUSH";)
      cpu.writeGpr(gprA, cpu.readGpr(gprA)+disp);
      memory.changeWord(cpu.readGpr(gprC), cpu.readGpr(gprA));

    break;

    case Instruction::OPCode::CSRRD:
      LOG(std::cout << "CSRRD";)
      cpu.writeGpr(gprA, cpu.readCsr(CPU::csr_B(ab)));
    break;

    case Instruction::OPCode::LD_TO_GPR_REG_DIR_DISP:
      LOG(std::cout << "LD_TO_GPR_REG_DIR_DISP";)
      cpu.writeGpr(gprA, cpu.readGpr(gprB) + cpu.readGpr(gprC) + disp);
    break;

    case Instruction::OPCode::LD_TO_GPR_REG_IND_DISP:
      LOG(std::cout << "LD_TO_GPR_REG_IND_DISP";)
      cpu.writeGpr(gprA, memory.readWord(cpu.readGpr(gprB) + cpu.readGpr(gprC) + disp));
    break;

    case Instruction::OPCode::POP:
      LOG(std::cout << "POP";)
      cpu.writeGpr(gprA, memory.readWord(cpu.readGpr(gprB)));
      cpu.writeGpr(gprB, cpu.readGpr(gprB) + disp);

    break;

    case Instruction::OPCode::CSRWR:
      LOG(std::cout << "CSRWR";)
      cpu.writeCsr(CPU::csr_A(ab), cpu.readGpr(gprB));
    break;

    case Instruction::OPCode::POP_CSR:
      LOG(std::cout << "POP_CSR";)
      cpu.writeCsr(cpu.csr_A(ab), memory.readWord(cpu.readGpr(gprB)));
      cpu.writeGpr(gprB, cpu.readGpr(gprB) + disp);

    break;

  default:
    std::cerr << "Invalid instruction: PC = 0x" << std::hex <<  cpu.getPC();
    exit(-1);
  }

  LOG(std::cout << " " << gprA << " " << gprB << " " << gprC << " "<< disp << " \n";)
}
int Emulator::emulation(){
    
  uint8_t ocm = 1; // oc + mod
  uint8_t oc; // just oc
  uint8_t ab = 2;
  uint8_t cd = 3;
  uint8_t dd = 4;
  while(ocm!=Instruction::HALT){
    ocm = memory.readByte(cpu.getPC());
    oc = (ocm>>4)&0x0f;
    ab = memory.readByte(cpu.getPC()+1);
    cd = memory.readByte(cpu.getPC()+2);
    dd = memory.readByte(cpu.getPC()+3);
    cpu.nextPC();
    cpu.print();
    CPU::GPR gprA = CPU::A(ab), gprB = CPU::B(ab), gprC = CPU::C(cd);
    int32_t disp = CPU::disp(cd, dd);
    std::cout << "disp: " << std::dec << disp;
    switch (oc){
      case Instruction::OC::HALT_T:
          LOG(std::cout << "halt " << gprA << " " << gprB << " " << gprC << " \n" ;)
      break;
      case Instruction::OC::INT_T:
          LOG(std::cout << "int\n" << gprA << " " << gprB << " " << gprC << " \n" ;)
      break;
      case Instruction::OC::CALL_T: 
      case Instruction::OC::JUMPS_T: 
        LOG(std::cout << "jmps\n" << gprA << " " << gprB << " " << gprC << " \n" ;)
      break;
      // without csr, csr is in load/store
      case Instruction::OC::XCHG_T:
      case Instruction::OC::ARIT_T:
      case Instruction::OC::LOG_T:
      case Instruction::OC::SH_T:
        handleGprInstructions(ocm, gprA, gprB, gprC);
      break;
      case Instruction::OC::ST_T:
      case Instruction::OC::LD_T:
        LOG(std::cout << "memory\n" << gprA << " " << gprB << " " << gprC << " \n" ;)
        handleLoadStoreInstructions(ocm, ab, gprA, gprB, gprC, disp);
      break;
      default:
        std::cerr << "Invalid instruction: PC = 0x" << std::hex <<  cpu.getPC();
        exit(-1);
      break;
    }
  }
  cpu.print();
  return 0;
}
