#include "../../inc/assembler/Assembler.hpp"
#include <iostream>
#include <string.h>

bool Assembler::finished = false;
char* Assembler::input; 
char* Assembler::output;
uint32_t Assembler::LC = 0;

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


void Assembler::closeSection(){
  // close previous section 

  if(!symtab.sectionOpened())return;
  
  symtab.getCurrentSection()->size = LC;
  LC = 0;

}

void Assembler::handleSection(std::string* name){

  if(symtab.doesSectionExist(name)){ // section opened again
    std::cerr << "Multiple section definitions." << std::endl;
    return;  
  }
  // close previous section
  closeSection();
  // add new section and set it to current
  symtab.addSection(name, SymbolTable::Entry(      
    0, 
    SymbolTable::Bind::LOC,
    *name,
    SymbolTable::Flags::DEFINED,
    SymbolTable::Type::SCTN
    )
  );


};

void Assembler::handleSymbolDefinition(std::string* name){
  if(!symtab.sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }

  if(symtab.doesSymbolExist(name)){
    SymbolTable::Entry* s = symtab.getSymbol(name);
    if(SymbolTable::isDefined(s->flags) || SymbolTable::isExtern(s->flags)){
      std::cerr << "Symbol redeclaration." << std::endl;
    }  
    return;
  }

  symtab.addSymbol(name, SymbolTable::Entry{ 
    LC,  
    SymbolTable::Bind::LOC, 
    symtab.getCurrentSectionName(), 
    SymbolTable::Flags::DEFINED  
  });
}

void Assembler::handleSymbolDeclaration(std::string* name){

}

void Assembler::handleLiteral(){
  
}

void Assembler::handleLabel(std::string* name){
  handleSymbolDefinition(name);
};

void Assembler::handleGlobal(std::string* name){
  SymbolTable::Entry* s;
  if(symtab.doesSymbolExist(name)){
    s = symtab.getSymbol(name);

    if(SymbolTable::isExtern(s->flags)){
      std::cerr << "Symbol is flagged as extern, can't be global." << std::endl;
      return;
    }

    s->bind = SymbolTable::Bind::GLOB;
    return;
  }


   symtab.addSymbol(name, SymbolTable::Entry{ 
    0, 
    SymbolTable::Bind::GLOB,
    symtab.getUndefinedSectionName()
  });
};


void Assembler::handleExtern(std::string* name){
  SymbolTable::Entry* s;
  if(symtab.doesSymbolExist(name)){
    s = symtab.getSymbol(name);

    if(SymbolTable::isDefined(s->flags)){
      std::cerr << "Symbol can't be defined and extern." << std::endl;
      return;
    }

    s->flags |= SymbolTable::Flags::EXTERN;
    return;
  }
  

  symtab.addSymbol(name, SymbolTable::Entry{ 
    0, 
    SymbolTable::Bind::GLOB,
    symtab.getUndefinedSectionName(),
    SymbolTable::Flags::EXTERN
  });
};

void Assembler::handleSkip(uint32_t size){

  if(!symtab.sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }
  
  LC+=size;

};

void Assembler::handleWordLiteral(uint32_t value){
  
  if(!symtab.sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }
  std::cout << std::hex << value << std::dec; // [] [] [] [] <= value

  LC+=4;
}

void Assembler::handleWordSymbol(std::string* name){
  // [] [] [] [] <= value of name (address)
  if(!symtab.sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }

  if(!symtab.doesSymbolExist(name)){
    symtab.addSymbol(name, SymbolTable::Entry{
          0, SymbolTable::Bind::LOC, symtab.getUndefinedSectionName()
        });
  }
  SymbolTable::Entry* e = symtab.getSymbol(name);
  // fill with zeros and backpatch real value
  std::cout << std::hex << 0 << std::dec;


  LC+=4;
}

void Assembler::handleEnd(){

  closeSection();

  finished = true;
};

