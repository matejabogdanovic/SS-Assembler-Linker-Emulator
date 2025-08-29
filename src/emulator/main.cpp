#include "../../inc/emulator/Emulator.hpp"


int main(int argc, char* argv[]){

  int ret = 0;
  try{
   ret = Emulator::start(argc, argv);
  }
  catch(const EmulatorException& e)
  {
    std::cout << e.what() << '\n';
    return -1;
  }
  
  return ret;
}
