#include "../../inc/assembler/Assembler.hpp"
#include <iostream>
#include <string.h>
#include <iomanip>
#include <fstream>


bool Assembler::finished = false;
char* Assembler::input; 
char* Assembler::output;
uint32_t Assembler::LC = 0;

LiteralPool Assembler::literalPool;
std::list <Assembler::Backpatch> Assembler::backpatch;
Memory Assembler::memory;

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
  memory.print(outputFile);
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


void Assembler::literalBackpatch(){
  // get end of section without literal pool
  uint32_t start_of_pool_relative = LC; //+ symtab.getSectionStart(symtab.current_section); 
  // write literal pool to end of section
  memory.writeWordVector(&literalPool.pool);
  // increase section size
  LC += 4 * literalPool.size();
  // literal patch, writing PC relative displacement in instruction to literal pool
  while(!literalPool.literalpatch.empty()){
    // get location to patch
    LiteralPool::LiteralPatch p = literalPool.literalpatch.front();
    literalPool.literalpatch.pop_front();

    uint32_t loc_to_patch_relative = p.location;

    if(literalPool.isInvalid(loc_to_patch_relative)){
      // fix location in backpatch to point to location in pool
      for (auto it = backpatch.begin(); it != backpatch.end(); ++it) {
        if(it->location == loc_to_patch_relative){
          it->location = start_of_pool_relative + p.index_of_literal * 4 ;
        }
      }
    }

    uint32_t displacement =
     start_of_pool_relative +    
     p.index_of_literal * 4 - // offset in literal pool
     (loc_to_patch_relative+2); // after displacement
    // [CCCC][DDDD] [DDDD][DDDD]
    //  ^rc  ^highDisp 
    uint32_t loc_to_patch = loc_to_patch_relative + 
    symtab.getSectionStart(symtab.current_section); // absolute address

    uint8_t rc_highDisp = memory.readByte(loc_to_patch); 
    uint8_t oo_highDisp = (uint8_t)(0x0F & (displacement >> 8));
    uint8_t rc_oo = (0xF0 &rc_highDisp);
    
    memory.changeByte( rc_oo | oo_highDisp,
     loc_to_patch);
    memory.changeByte(displacement, loc_to_patch+1);
    
  }
  // todo

  literalPool.isInvalidLocation.clear();
  literalPool.pool.clear();
  //literalPool.literalpatch.clear();
  // literal pool clear
  //literalPool.literalBackpatch(&memory, end_of_section_addr);
}

void Assembler::closeSection(){
  // close previous section 

  if(!symtab.sectionOpened())return;
  
  literalBackpatch();

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
  
  // std::cout << std::hex << 0 << std::dec;

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

  memory.writeInstruction({Instruction::OPCode::CALL_IND, PC});
  
  literalPool.put(value, LC+2);

  LC+=4;
}

void Assembler::handleCallSymbol(std::string* name){
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
  
  // location is not known, it will be known when backpatch starts

  backpatch.push_back({LC+2, s, symtab.getCurrentSection()});

  memory.writeInstruction({Instruction::OPCode::CALL_IND, PC});

  
  literalPool.put(0xffffffff, LC+2, true);



  LC+=4;
}

void Assembler::handleGprInstructions(Instruction::OPCode op, uint8_t gprS, uint8_t gprD){

  switch (op)
  {
    case Instruction::OPCode::XCHG: // has 2 destinations
       memory.writeInstruction({op, 0, gprD, gprS});
    break;
    case Instruction::OPCode::ADD:
    case Instruction::OPCode::SUB:
    case Instruction::OPCode::MUL:
    case Instruction::OPCode::DIV:
    case Instruction::OPCode::AND:
    case Instruction::OPCode::OR:
    case Instruction::OPCode::XOR:
    case Instruction::OPCode::SHL:
    case Instruction::OPCode::SHR:
       memory.writeInstruction({op, gprD, gprD, gprS});
    break;
    case Instruction::OPCode::CSRRD:
    case Instruction::OPCode::CSRWR:
       memory.writeInstruction({op, gprD, gprS, 0});
    break;
    case Instruction::OPCode::NOT:
      memory.writeInstruction({op, gprD, gprS});
    break;

 
  default:
    std::cout << "Invalid handleGprInstructions call." << std::endl;
    return;
  }
  LC += 4;
}

void Assembler::symbolBackpatch(){
  // all values should be known except for extern symbols
  while(!backpatch.empty()){
    Backpatch p = backpatch.front();
    backpatch.pop_front();

    std::cout << 
    "====================================\n Backpatching symbol "  <<
     symtab.getSymbolName(p.symbol) << 
    " in section " << p.section->ndx <<
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
    if(p.symbol->ndx != p.section->ndx){
      std::cout << ">>> Symbol in different section, needs relocation." << std::endl;
    }
    
    memory.print(std::cout);

    
    // write
    uint32_t loc_to_patch = p.location + symtab.getSectionStart(p.section->ndx);

    std::cout << std::hex << "(abs) 0x" << loc_to_patch << 
    " (rel) 0x" << p.location << std::dec << std::endl;

    uint32_t value = p.symbol->offset; 
    memory.changeWord(value, loc_to_patch);
    

  }

  std::cout << "====================================\n After patch: " << std::endl;
  memory.print(std::cout);

}

void Assembler::handleEnd(){
  
    closeSection();
    symbolBackpatch();
    finished = true;
  
};

