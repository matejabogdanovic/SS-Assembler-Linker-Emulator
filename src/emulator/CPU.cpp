#include "../../inc/emulator/CPU.hpp"


void CPU::printGpr(){
  std::cout << "\n-----------------------------------------------------------------\n"<<
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
void CPU::printCsr(){


  std::cout << "\n"<< " " << "STATUS" << "=0x" <<
      std::right << std::uppercase << std::setw(8) << std::setfill('0') << std::hex <<
      csrfile[STATUS] << std::dec;
  std::cout << " " << "HANDLER"<< "=0x" <<
      std::right << std::uppercase << std::setw(8) << std::setfill('0') << std::hex <<
      csrfile[HANDLER] << std::dec;
    std::cout << " " << "CAUSE" << "=0x" <<
      std::right << std::uppercase << std::setw(8) << std::setfill('0') << std::hex <<
      csrfile[CAUSE] << std::dec;
      std::cout << std::endl;
}