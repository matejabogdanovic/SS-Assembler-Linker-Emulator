#include "../../inc/assembler/Assembler.hpp"


int main(int argc, char* argv[]){

  int ret = 0;
  try{
   ret = Assembler::start(argc, argv);
  }
  catch(const AssemblerException& e)
  {
    std::cout << e.what() << '\n';
    return -1;
  }
  
  return ret;
}