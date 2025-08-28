#include "../../inc/emulator/CPU.hpp"


void CPU::print(){
  std::cout << "-----------------------------------------------------------------\n"<<
    "Emulated processor executed halt instruction\n"<<
    "Emulated processor state:\n";
  for (size_t i = 0; i < 16; i++)
  {
    if(i!=0 && i % 4 == 0)std::cout << std::endl;
    std::cout << (i>9?" ":"  ") << "r" << i << "=0x" <<
      std::right << std::uppercase << std::setw(8) << std::setfill('0') << std::hex <<
      regfile[i] << std::dec;
  }
  std::cout << std::endl;
}
