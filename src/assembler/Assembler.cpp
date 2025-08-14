#include "../../inc/assembler/Assembler.hpp"
#include <iostream>
#include <string.h>
#include <iomanip>
#include <fstream>

bool Assembler::finished = false;
char* Assembler::input; 
char* Assembler::output;
uint32_t Assembler::LC = 0;
std::list <Assembler::LiteralPatch> Assembler::literalpatch;
std::list <Assembler::Backpatch> Assembler::backpatch;
Memory Assembler::memory;
std::vector <uint32_t> Assembler::literalPool;
std::unordered_map <uint32_t, uint32_t> Assembler::literalTable;
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

  symtab.printTable(std::cout);
    
  std::ofstream outputFile(output); // otvara fajl za pisanje

  symtab.printTable(outputFile);

  if (!outputFile.is_open()) {
        std::cerr << "assembler: error: can't open output file\n";
        return -1;
  }

  outputFile.close(); 
    
  // std::ofstream outputFileBinary(output, std::ios_base::binary); // otvara fajl za pisanje

  // symtab.printTable(outputFileBinary);

  // if (!outputFileBinary.is_open()) {
  //       std::cerr << "assembler: error: can't open output file\n";
  //       return -1;
  // }

  // outputFileBinary.close(); 
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
  

  uint32_t end_of_section_addr = LC + symtab.getSectionStart(symtab.current_section); 
  memory.writeWordVector(&literalPool);
  LC += 4 * literalPool.size();

  while(!literalpatch.empty()){
    
    LiteralPatch p = literalpatch.front();
    literalpatch.pop_front();
    
    uint32_t loc_to_patch = p.location;
    uint32_t value = end_of_section_addr + p.index_of_literal * 4 - (loc_to_patch+2);
    //value = 0x12345678;
     

    uint8_t rc_highDisp = memory.readByte(loc_to_patch); 
    uint8_t oo_highDisp = (uint8_t)(0x0F & (value >> 8));
    uint8_t rc_oo = (0xF0 &rc_highDisp);

    memory.changeByte( rc_oo | oo_highDisp,
     loc_to_patch);
    memory.changeByte(value, loc_to_patch+1);
    
  }

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
    symtab.current_section+1,
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

    s->offset = LC;
    s->ndx = symtab.getCurrentSection()->ndx;
    s->flags |= SymbolTable::Flags::DEFINED ;
    return;
  }

  symtab.addSymbol(name, SymbolTable::Entry{ 
    LC,  
    SymbolTable::Bind::LOC, 
    symtab.current_section, 
    SymbolTable::Flags::DEFINED  
  });
}

void Assembler::handleSymbolUsage(std::string* name){
  // SymbolTable::Entry* s ;
  // if(!symtab.doesSymbolExist(name)){
  //   s = &SymbolTable::Entry{0, SymbolTable::Bind::LOC, symtab.getUndefinedSectionName()};
  //   if(!symtab.sectionOpened()){
  //     s->flags = SymbolTable::Flags::ABSOLUTE;
  //     s->bind = SymbolTable::Bind::GLOB;
  //   }
  //   symtab.addSymbol(name, *s);
  // }

  // insert symbol
}

void Assembler::handleLiteral(){
  
}

void Assembler::handleLabel(std::string* name){
  handleSymbolDefinition(name);
};

void Assembler::handleGlobal(std::string* name){
  if(symtab.doesSymbolExist(name)){
    SymbolTable::Entry* s = symtab.getSymbol(name);

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
    SymbolTable::UNDEFINED_SECTION
  });
};


void Assembler::handleExtern(std::string* name){

  if(symtab.doesSymbolExist(name)){
    SymbolTable::Entry* s = symtab.getSymbol(name);

    if(SymbolTable::isDefined(s->flags)){
      std::cerr << "Symbol can't be defined and extern." << std::endl;
      return;
    }

    s->flags |= SymbolTable::Flags::EXTERN;
    s->bind = SymbolTable::Bind::GLOB;
    return;
  }
  

  symtab.addSymbol(name, SymbolTable::Entry{ 
    0, 
    SymbolTable::Bind::GLOB,
    symtab.current_section,
    SymbolTable::Flags::EXTERN
  });
};

void Assembler::handleSkip(uint32_t size){

  if(!symtab.sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }
  
  // populate memory with zeros

  memory.writeByte(0, size);


  LC+=size;

};

void Assembler::handleWordLiteral(uint32_t value){
  
  if(!symtab.sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }
  std::cout << std::hex << value << std::dec; // [] [] [] [] <= value

  memory.writeByte(0, 4);
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
          0, SymbolTable::Bind::LOC, SymbolTable::UNDEFINED_SECTION
        });
  }
  SymbolTable::Entry* s = symtab.getSymbol(name);
  // populate memory with zeros and backpatch
  backpatch.push_back({LC, s, symtab.getCurrentSection()});
  
  std::cout << std::hex << 0 << std::dec;

  memory.writeByte(0, 4);

  LC+=4;
}

void Assembler::handleHalt(){
  if(!symtab.sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }
  memory.writeInstruction({Instruction::OPCode::HALT});
  LC+=4;
}

void Assembler::handleInt(){
  if(!symtab.sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }
  memory.writeInstruction({Instruction::OPCode::INT});
  LC+=4;
}

void Assembler::handleCallLiteral(uint32_t value){
  if(!symtab.sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }

  memory.writeInstruction({Instruction::OPCode::CALL_IND, PC, 7, 2, 0});
  

  if(literalTable.count(value)==0){ // literal doesn't exist 
    
    literalTable[value] = {(uint32_t)literalPool.size()};

    literalPool.push_back(value);
  }
  literalpatch.push_back({LC+2, literalTable[value]}); // should be 2.5 but can't write

  LC+=4;
}

void Assembler::startBackpatch(){
  // all values should be known except for extern symbols
  while(!backpatch.empty()){
    Backpatch p = backpatch.front();
    backpatch.pop_front();

    std::cout << 
    "Backpatching symbol "  <<
     symtab.getSymbolName(p.symbol) << 
    " in section " << symtab.getSectionName(p.section)<<
     " location 0x" << std::hex << p.location << std::dec << 
    std::endl;

    if(SymbolTable::isExtern(p.symbol->flags)){
      std::cout << "Extern symbol -> reallocation (section, location) " << std::endl;

      continue;
    }
    if(!SymbolTable::isDefined(p.symbol->flags)){
      std::cerr << "Symbol not defined." << std::endl;
      return;
    }

    std::cout << "Symbol defined and global/local. Assembler can patch." << std::endl;

    memory.print();

  
    // get section beginning
    // uint32_t sz = 0;
    // for(int i = 0; i < symtab.section_names.size() ; i++){
    //   SymbolTable::Entry* section = symtab.getSection(&symtab.section_names[0]);
    //   if(section->ndx == p.section->ndx){
    //     break;  
    //   }
    //   sz += section->size;

    // }
    // write
    uint32_t loc_to_patch = p.location + symtab.getSectionStart(p.section->ndx);
   
    uint32_t value = p.symbol->offset;

    memory.changeWord(value, loc_to_patch);
    

  }

  std::cout << "After patch: " << std::endl;
  memory.print();

}

void Assembler::handleEnd(){
  
    closeSection();
    startBackpatch();
    finished = true;
  
};

