#include "../../inc/linker/Linker.hpp"


int main(int argc, char* argv[]){

  
  int ret = 0;
  try{
   ret = Linker::start(argc, argv);
  }
  catch(const LinkerException& e)
  {
    std::cout << e.what() << '\n';
    return -1;
  }
  
  return ret;
}

