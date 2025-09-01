#include "../../inc/emulator/Terminal.hpp"
#include <iostream>
#include <fcntl.h>


void Terminal::in(){
  termios oldt, newt;

  // save previous terminal config
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  // turn off canonical and echo
  newt.c_lflag &= ~(ICANON | ECHO);
  newt.c_cc[VMIN] = 0;  // minimum 0 bytes to read
  newt.c_cc[VTIME] = 0; // no timeout
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

  char c;
  ssize_t n;
  while (!closed) {
    n = read(STDIN_FILENO, &c, 1); 
    if(n<=0)continue;
    // c = getchar();
    // shouldn't be race condition, only if user program writes in TERM_IN but it shouldn't
    memory->changeWord((uint32_t)c, EmulatedMemory::TERM_IN);
    cpu->interruptTerminal();
  }
  // restore previous terminal config
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

}



void Terminal::close(){
  closed = true;
}
