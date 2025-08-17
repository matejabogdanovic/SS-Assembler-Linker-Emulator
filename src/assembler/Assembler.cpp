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
  
  // literal patch, writing PC relative displacement in instruction to literal pool
  while(!literalPool.patches.empty()){
    // get location to patch
    bool instruction_alternative_used = false;
    LiteralPool::LiteralPatch p = literalPool.patches.front();
    literalPool.patches.pop_front();
    
    uint32_t displacement =   
     LC - // end of pool
     (p.location+2); // after displacement

    if(literalPool.isInvalid(p.location)){
      // fix location in backpatch to point to location in pool
      for (auto it = backpatch.begin(); it != backpatch.end(); ++it) {
        if(it->location == p.location){ 
            
          if(it->symbol->ndx == symtab.current_section && SymbolTable::isDefined(it->symbol->flags)){
            // THEN I WOULD NEED TO CHANGE INSTRUCTION TO BE JUST DISPLACEMENT
            std::cout << "SYMBOL DEFINED, LITERAL POOL CAN PATCH" << std::endl;
            memory.changeInstruction(it->alternative, p.location-2); // TODO changeInstruction
            instruction_alternative_used = true;  
            displacement = it->symbol->offset - (p.location+2);
            backpatch.erase(it);
            break;
          }
          it->location = LC; // end of pool
        }
      }
    }

    
    // [CCCC][DDDD] [DDDD][DDDD]
    //  ^rc  ^highDisp 
    uint32_t loc_to_patch = p.location + 
    symtab.getSectionStart(symtab.current_section); // absolute address

    uint8_t rc_highDisp = memory.readByte(loc_to_patch); 
    uint8_t oo_highDisp = (uint8_t)(0x0F & (displacement >> 8));
    uint8_t rc_oo = (0xF0 &rc_highDisp);
    // write a literal
    if(!instruction_alternative_used){
      memory.writeWord(p.literal);
      LC += 4;
    }

    memory.changeByte( rc_oo | oo_highDisp,
     loc_to_patch);
    memory.changeByte(displacement, loc_to_patch+1);
    
  }



  literalPool.isInvalidLocation.clear();
  //literalPool.pool.clear();
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
// todo
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
// todo
void Assembler::handleLiteral(){
  
}
// Directives ----------
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

  memory.writeByte(0, 4);

  LC+=4;
}
// Instructions ----------

void Assembler::handleZeroArgsInstructions(Instruction::OPCode op){
  if(!symtab.sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }
  switch (op)
  {
    case Instruction::OPCode::IRET:
      handleStackInstructions(Instruction::OPCode::POP, PC);
      handleStackInstructions(Instruction::OPCode::POP_CSR, STATUS);
      //memory.writeInstruction({op}); // pop pc, pop status
      //memory.writeInstruction({op}); // pop pc, pop status
      //LC += 4*2;
      return; 
    break;
    case Instruction::OPCode::RET:
      //memory.writeInstruction({op}); // pop pc
      handleStackInstructions(Instruction::OPCode::POP, PC);
      return;
    break;

    case Instruction::OPCode::HALT: 
    case Instruction::OPCode::INT:
       memory.writeInstruction({op});
    break;

  default:
    std::cout << "Invalid handleZeroArgsInstructions call." << std::endl;
    return;
  }
  LC += 4;
}

// call and jmp
void Assembler::handleJustLiteralInstructions(Instruction::OPCode op, uint32_t value){
  if(!symtab.sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }
   switch (op)
  {
    case Instruction::OPCode::CALL_IND:
    case Instruction::OPCode::JMP_REG_IND_DISP:
      memory.writeInstruction({op, PC});
      literalPool.put(value, LC+2);

    break;


  default:
    std::cout << "Invalid handleJustLiteralInstructions call." << std::endl;
    return;
  }

  LC+=4;
}
// call and jmp
void Assembler::handleJustSymbolInstructions(Instruction::OPCode op, std::string* name){
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
  
   switch (op)
  {
    case Instruction::OPCode::CALL_IND:
      backpatch.push_back({LC+2, s, symtab.getCurrentSection(), 
        {Instruction::OPCode::CALL_REG, PC}});
      memory.writeInstruction({op, PC});
      literalPool.put(0xfffffff0, LC+2, true);
    break;
    case Instruction::OPCode::JMP_REG_IND_DISP:
      // location is not known, it will be known when backpatch starts
      backpatch.push_back({LC+2, s, symtab.getCurrentSection(), 
      {Instruction::OPCode::JMP_REG_DIR_DISP, PC}});
      memory.writeInstruction({op, PC});
      literalPool.put(0xfffffff1, LC+2, true);

    break;


  default:
    std::cout << "Invalid handleJustLiteralInstructions call." << std::endl;
    return;
  }


  LC+=4;
}

void Assembler::handleGprInstructions(Instruction::OPCode op, uint8_t gprS, uint8_t gprD){
  if(!symtab.sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }
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
// push and pop
void Assembler::handleStackInstructions(Instruction::OPCode op, uint8_t reg){
  if(!symtab.sectionOpened()){
    std::cerr << "Undefined section." << std::endl;
    return;
  }
  switch (op)
  {
    case Instruction::OPCode::PUSH:
      memory.writeInstruction({op, SP, 0, reg, (uint16_t)-4}); 

    break;
    case Instruction::OPCode::POP:
    case Instruction::OPCode::POP_CSR:
      memory.writeInstruction({op, reg, SP, 0, 4}); // pop pc

    break;

  default:
    std::cout << "Invalid handleStackInstructions call." << std::endl;
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

