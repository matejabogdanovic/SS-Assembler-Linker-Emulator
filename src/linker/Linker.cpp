#include "../../inc/linker/Linker.hpp"

std::vector<Linker::FileState> Linker::files;
std::vector<std::string> Linker::file_names;

int Linker::parseArguments(int argc, char* argv[]){
  for (size_t i = 1; i < argc; i++){
    file_names.push_back(argv[i]);
  }
  return 0;
}

int Linker::start(int argc, char* argv[]){
  
  if(parseArguments(argc, argv)<0){
    std::cerr << "linker: error: invalid arguments.\n";
    return -1;
  };
  

  if(processing() < 0)return -1;

  return 0;

}

int Linker::processing(){
  for (size_t i = 0; i < file_names.size(); i++)
  {
    SymbolTable symtab(false);
    RelTable rel;
    Memory memory;
    
    std::ifstream inputBinary(file_names[i], std::ios::binary); // otvara fajl za pisanje

    if (!inputBinary.is_open()) {
      std::cerr << "assembler: error: can't open input file\n";
      return -1;
    }
    // read data
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    std::cout << "From file: " << file_names[i] << std::endl;

    symtab.loadFromFile(inputBinary);
    rel.loadFromFile(inputBinary, &symtab);
    memory.loadFromFile(inputBinary);
    
    inputBinary.close(); 
    
    // store data
    files.push_back({symtab, rel, memory});
    files[0].symtab.print(std::cout);
    files[0].rel.print(std::cout, &symtab);
    files[0].memory.printCode(std::cout, &symtab);

    }
  return 0;
}