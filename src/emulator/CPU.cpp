#include "../../inc/emulator/CPU.hpp"
#include <iomanip>
#include <iostream>
std::map<uint8_t,CPU::InstructionPointer> 
    CPU::instructions = {
      {Instruction::OPCode::NOT, &CPU::i_not},
      {Instruction::OPCode::HALT, &CPU::i_halt},
      {Instruction::OPCode::ADD, &CPU::i_add},};

// todo set psw
void CPU::i_not(CPU& cpu, uint8_t ab, uint8_t co, uint8_t oo){
  GPR regA = A(ab), regB = B(ab);
  std::cout << "not";
  cpu.writeGpr(regA, ~cpu.readGpr(regB));
}

void CPU::i_halt(CPU& cpu, uint8_t ab, uint8_t co, uint8_t oo){
  std::cout << "halt";
}

void CPU::i_add(CPU& cpu, uint8_t ab, uint8_t co, uint8_t oo){
  GPR regA = A(ab), regB = B(ab), regC = C(co);
  std::cout << "add";
  cpu.writeGpr(regA, cpu.readGpr(regB) + cpu.readGpr(regC));
}

void CPU::print(){
  std::cout << "-----------------------------------------------------------------\n"<<
    "Emulated processor executed halt instruction\n"<<
    "Emulated processor state:\n";
  for (size_t i = 0; i < 16; i++)
  {
    if(i!=0 && i % 4 == 0)std::cout << std::endl;
    std::cout << (i>9?" ":"  ") << "r" << i << "=0x" <<
      std::left << std::uppercase << std::setw(8) << std::setfill('0') << std::hex <<
      regfile[i] << std::dec;
  }
  std::cout << std::endl;
}
