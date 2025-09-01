
#include "./CPU.hpp"
#include "./Terminal.hpp"
#include <fstream>
#include <iostream>

class Emulator{
  
public:
  static int start(int argc, char* argv[]);
  
private:
  static std::string toHex(uint32_t value);
  static void handleGprInstructions(uint8_t ocm, CPU::GPR gprA, CPU::GPR gprB, CPU::GPR gprC);
  // ab used for csr
  static void handleLoadStoreInstructions(uint8_t ocm, uint8_t ab,  CPU::GPR gprA, CPU::GPR gprB, CPU::GPR gprC, int32_t disp);
  static void handleJumpInstructions(uint8_t ocm, CPU::GPR gprA, CPU::GPR gprB, CPU::GPR gprC, int32_t disp);
  static void handleCallInstructions(uint8_t ocm, CPU::GPR gprA, CPU::GPR gprB,  int32_t disp);

  static void handleInterrupt(CPU::Interrupt_T interrupt_t);
  



  static int emulation();
  static int processing();
  static EmulatedMemory memory;
  static char* input;

  static CPU cpu;
  
};
