#pragma once
#include "./EmulatedMemory.hpp"
#include "./CPU.hpp"
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <cstdint>


class Terminal {
public:
  Terminal(CPU* cpu, EmulatedMemory* memory) {
    this->cpu = cpu;
    this->memory = memory;
    input = std::thread(&Terminal::in, this);

  }


  
 ~Terminal() {
   close();
   if (input.joinable())  
    input.join(); 

  }
  

  
  void close();
private:

  void in();

  std::thread input;
  CPU* cpu;
  EmulatedMemory* memory;
  volatile bool closed;
  
};

