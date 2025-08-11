#include "../../inc/assembler/Assembler.hpp"
#include <iostream>
#include <string.h>

bool Assembler::finished = false;
char* Assembler::input; 
char* Assembler::output;
uint32_t Assembler::LC = 0;
uint32_t Assembler::current_section = 0;
SymbolTable Assembler::symtab;

extern void yyerror(const char*); // error function
extern int yyparse(); // parsing function
extern FILE* yyin; // input file

int Assembler::processing(){
  FILE* inputFile = fopen(input, "rw+");
  if(!inputFile){
    std::cerr << "asembler: error: input file doesn't exist.\n";
  	return -1;
  }
  // check if file ends with \n
  if (fseek(inputFile, -1, SEEK_END) != 0) {
    std::cerr <<"asembler: error: fseek";
    fclose(inputFile);
    return -1;
  }
  // if file doesn't end with \n then append \n 
  if(((char)fgetc(inputFile))!='\n'){
    std::cout << "asembler: warning: input file doesn't end with \\n. Inserting new line.\n";
    fseek(inputFile, 0, SEEK_END);
    fputc('\n', inputFile);
  }
  fseek(inputFile, 0, SEEK_SET);

  // parsing
  yyin = inputFile;
  yyparse();
    
  fclose(inputFile);

  if(!finished){
    std::cerr << "No end directive." << std::endl;
    
  }
  symtab.printTable();
      
  return 0;
}


int Assembler::parseArguments(int argc, char* argv[]){
 if(argc < 2)return -1; 

  if(!strcmp("-o", argv[1])){ 
    if(argc != 4)return -1; 
    output = argv[2];
    input = argv[3];
    return 0;
  }

  if(argc == 2){
    input = argv[1];
    output = nullptr;
    return 0;
  }
  
  return -1;
}

bool Assembler::sectionOpened(){
  return current_section != 0;
}

int Assembler::start(int argc, char* argv[]){
  if(parseArguments(argc, argv)<0){
    std::cerr << "asembler: error: invalid arguments.\n";
    return -1;
  };
  
  std::cout << "Input file: " 
            << (input ? input : "no input file.") 
            << std::endl;
  std::cout << "Output file: " 
            << (output != nullptr ? output : "a.out") 
            << std::endl;

  if(processing() < 0)return -1;

  return 0;
}

Assembler::Status Assembler::addSymbol(
    uint32_t value,
    SymbolTable::Type type,
    SymbolTable::Bind bind,
    std::string* name,
    bool is_defined
  )
{

  uint32_t num = 0, size = 0, ndx = 0;
  if(type==SymbolTable::Type::SCTN){
    
    if(sectionOpened()){ 
      if(symtab.doesSymbolExist(name)){ // section opened again
        return Status::ERROR;  
      }
      // set previous section size
      symtab.table[symtab.sections[current_section]].size = LC;
    }
    current_section = symtab.sections.size();
    symtab.sections.push_back(*name);
    LC = 0;
  }
  // else if (symtab.doesSymbolExist(name)){
  //   if(symtab.table[*name].bind ==)
  //   std::cerr << "Undefined section for symbol: " << name << std::endl;
  //   return Status::ERROR;
  // }
  
  symtab.table[*name] = { num, value, size, type, bind, symtab.sections[current_section], ndx, false, is_defined};
  if(type!=SymbolTable::Type::SCTN)symtab.symbols.push_back(*name);
  // printTable();
    

  return Status::OK;
}

Assembler::Status Assembler::addExternSymbol(std::string* name){
  
  symtab.table[*name] = { 0, 0, 0, 
    SymbolTable::Type::NOTYP, 
    SymbolTable::Bind::GLOB, 
    symtab.sections[0], 
    0, true, false};
  symtab.symbols.push_back(*name);
  
  return Status::OK;
}

void Assembler::handleLabel(std::string* name){
  if(!sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }
  Assembler::addSymbol(
    LC,
    SymbolTable::Type::NOTYP,
    SymbolTable::Bind::LOC,
    name, 
    true
  );
};

void Assembler::handleSection(std::string* name){
  
  Assembler::addSymbol(
    0,
    SymbolTable::Type::SCTN,
    SymbolTable::Bind::LOC,
    name, 
    true
  );
  
};


void Assembler::handleGlobal(std::string* name){
  
  Assembler::addSymbol(
    0,
    SymbolTable::Type::NOTYP,
    SymbolTable::Bind::GLOB,
    name, 
    false
  );
  
};


void Assembler::handleExtern(std::string* name){
  
  Assembler::addExternSymbol(
    name
  );
  
};

void Assembler::handleSkip(int32_t size){

  if(!sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }

  if(size < 0){
    std::cerr << "Can't use negative size for .skip." << std::endl;
    return;
  };


  LC+=size;

};

void Assembler::handleEnd(){
  
  symtab.table[symtab.sections[current_section]].size = LC;

  finished = true;
};


// int Assembler::incLC(uint32_t val){
//   if(SymbolTable::current_section == "UND" && val > 0){
//     std::cerr << "Undefined section." << std::endl;
//     return -1;
//   }
//   LC += val;
//   return 0;
// };

// uint32_t Assembler::getLC(){
//   return LC;
// };