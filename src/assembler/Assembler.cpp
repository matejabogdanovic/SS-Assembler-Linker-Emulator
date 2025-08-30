#include "../../inc/assembler/Assembler.hpp"

#include <iostream>
#include <string.h>
#include <iomanip>
#include <fstream>
#include <exception>

bool Assembler::finished = false;
char* Assembler::input; 
char* Assembler::output;
uint32_t Assembler::LC = 0;

LiteralPool Assembler::literalPool;
std::list <Assembler::Backpatch> Assembler::backpatch;
Memory Assembler::memory;

SymbolTable Assembler::symtab;
RelTable Assembler::rel;

extern void yyerror(const char*); // error function
extern int yyparse(); // parsing function
extern FILE* yyin; // input file

int Assembler::processing(){
  FILE* inputFile = fopen(input, "rw+");
  if(!inputFile){
    throw AssemblerException("can't open input file");
  }
  // check if file ends with \n
  if (fseek(inputFile, -1, SEEK_END) != 0) {
    fclose(inputFile);
    throw AssemblerException("can't read from file");
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

    throw AssemblerException("no .end directive");
  }

  LOG(symtab.print(std::cout);)
  LOG(rel.print(std::cout, &symtab);)
  LOG(memory.printCode(std::cout, &symtab);)

  std::ofstream outputFile(output); // otvara fajl za pisanje

  if (!outputFile.is_open()) {
     throw AssemblerException("can't open output file");
  }

  symtab.print(outputFile);
  rel.print(outputFile, &symtab);
  memory.printCode(outputFile, &symtab);


  outputFile.close(); 
  // binary
  
  std::ofstream outputFileBinary(std::string(output)+std::string(".bin"), std::ios::binary); 

  if (!outputFileBinary.is_open()) {
    throw AssemblerException("can't open binary output file");
    return -1;
  }

  symtab.printBinary(outputFileBinary);
  rel.printBinary(outputFileBinary);
  memory.printBinary(outputFileBinary);

  outputFileBinary.close(); 


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
    throw AssemblerException("invalid arguments");
  };
  
  LOG(std::cout << "Input file: " 
            << (input ? input : "no input file.") 
            << std::endl;)
  LOG(std::cout << "Output file: " 
            << (output != nullptr ? output : "a.out") 
            << std::endl;)

  if(processing() < 0)return -1;

  return 0;
}


// Directives ----------
void Assembler::handleSection(std::string* name){

  if(symtab.doesSectionExist(name)){ // section opened again
    throw AssemblerException("multiple section definition -> " + *name);
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

void Assembler::handleLabel(std::string* name){
  
  if(!symtab.sectionOpened()){
    throw AssemblerException("undefined section");
  }

  if(symtab.doesSymbolExist(name)){
    SymbolTable::Entry* s = symtab.getSymbol(name);
    if(SymbolTable::isDefined(s->flags) || SymbolTable::isExtern(s->flags)){
      throw AssemblerException("symbol redeclaration -> " + *name);
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
};

void Assembler::handleGlobal(std::string* name){
  if(symtab.doesSymbolExist(name)){
    SymbolTable::Entry* s = symtab.getSymbol(name);

    if(SymbolTable::isExtern(s->flags)){
      throw AssemblerException("symbol flagged as extern, now global -> " + *name);
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
      throw AssemblerException("symbol can't be defined and extern -> " + *name);
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
    throw AssemblerException("section not opened");
  }
  
  // populate memory with zeros

  memory.writeByte(0, size);


  LC+=size;

};

void Assembler::handleWordLiteral(uint32_t value){
  
  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
  }
  
  memory.writeWord(value);
  
  LC+=4;
}

void Assembler::handleWordSymbol(std::string* name){
  // [] [] [] [] <= value of name (address)
  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
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

#define in12bSigned(value) ((value <= 0x7ff) || ((value & 0xfffff800)  == 0xfffff800))

// Instructions ----------
// iret ret halt int
void Assembler::handleZeroArgsInstructions(Instruction::OPCode op){
  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
  }
  switch (op)
  {
    case Instruction::OPCode::IRET:
      // handleStackInstructions(Instruction::OPCode::POP, PC);
      // handleStackInstructions(Instruction::OPCode::POP_CSR, STATUS);

      memory.writeInstruction({Instruction::OPCode::LD_TO_CSR_REG_IND_DISP, STATUS, SP, 0, 4});
      memory.writeInstruction({Instruction::OPCode::POP, PC, SP, 0, 8}); // pop pc
      //memory.writeInstruction({op}); // pop pc, pop status
      //memory.writeInstruction({op}); // pop pc, pop status
      LC += 2*4;
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
LOG(std::cout << "Invalid handleZeroArgsInstructions call." << std::endl;)
    return;
  }
  LC += 4;
}
// call and jmp
void Assembler::handleJustLiteralInstructions(Instruction::OPCode op, uint32_t value){
  std::cout << std::hex << (value & 0xfffff800 ) ;
  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
  }
   switch (op)
  {
    case Instruction::OPCode::CALL_IND:
      if(in12bSigned(value)){
        memory.writeInstruction(
          {Instruction::OPCode::CALL_REG, 0, 0, 0, (uint16_t)value});
          break;
      }
      memory.writeInstruction({op, PC});
      literalPool.put(value, LC);
      break;
    case Instruction::OPCode::JMP_REG_IND_DISP:
      if(in12bSigned(value)){
        memory.writeInstruction(
          {Instruction::OPCode::JMP_REG_DIR_DISP, 0, 0, 0, (uint16_t)value});
          break;
      }
      memory.writeInstruction({op, PC});
      literalPool.put(value, LC);
    break;


  default:
LOG(std::cout << "Invalid handleJustLiteralInstructions call." << std::endl;)
    return;
  }

  LC+=4;
}
void Assembler::handleJustSymbolInstructions(Instruction::OPCode op, std::string* name){
  
  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
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
      // backpatch.push_back({LC, s, symtab.getCurrentSection()});
      memory.writeInstruction({op, PC});
      literalPool.put(0xfffffff0, LC, name, {Instruction::OPCode::CALL_REG, PC});
    break;
    case Instruction::OPCode::JMP_REG_IND_DISP:
      // location is not known, it will be known when backpatch starts
      // backpatch.push_back({LC, s, symtab.getCurrentSection()});
      memory.writeInstruction({op, PC});
      literalPool.put(0xfffffff1, LC, name,{Instruction::OPCode::JMP_REG_DIR_DISP, PC});

    break;


  default:
LOG(std::cout << "Invalid handleJustLiteralInstructions call." << std::endl;)
    return;
  }


  LC+=4;
}
// alu
void Assembler::handleGprInstructions(Instruction::OPCode op, uint8_t gprS, uint8_t gprD){
  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
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
LOG(std::cout << "Invalid handleGprInstructions call." << std::endl;)
    return;
  }
  LC += 4;
}
// push and pop
void Assembler::handleStackInstructions(Instruction::OPCode op, uint8_t reg){
  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
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
LOG(std::cout << "Invalid handleStackInstructions call." << std::endl;)
    return;
  }
  LC += 4;
}
// branch
void Assembler::handleBranchLiteralInstructions(Instruction::OPCode op, uint8_t gpr1, uint8_t gpr2, uint32_t value){
  
  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
  }
   switch (op)
  {
    case Instruction::OPCode::BEQ_REG_IND_DISP:
      if(in12bSigned(value)){
        memory.writeInstruction({Instruction::OPCode::BEQ_REG_DIR_DISP,
           0, gpr1, gpr2, (uint16_t)value});
        break;
      }
      memory.writeInstruction({op, PC, gpr1, gpr2});
      literalPool.put(value, LC);
      break;
    case Instruction::OPCode::BNE_REG_IND_DISP:
      if(in12bSigned(value)){
        memory.writeInstruction({Instruction::OPCode::BNE_REG_DIR_DISP,
           0, gpr1, gpr2, (uint16_t)value});
        break;
      }
      memory.writeInstruction({op, PC, gpr1, gpr2});
      literalPool.put(value, LC);
      break;
    case Instruction::OPCode::BGT_REG_IND_DISP:
      if(in12bSigned(value)){
        memory.writeInstruction({Instruction::OPCode::BGT_REG_DIR_DISP,
           0, gpr1, gpr2, (uint16_t)value});
        break;
      }
      memory.writeInstruction({op, PC, gpr1, gpr2});
      literalPool.put(value, LC);
      break;


  default:
LOG(std::cout << "Invalid handleBranchLiteralInstructions call." << std::endl;)
    return;
  }

  LC+=4;

}
void Assembler::handleBranchSymbolInstructions(Instruction::OPCode op, uint8_t gpr1, uint8_t gpr2, std::string* name){

  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
  }

  if(!symtab.doesSymbolExist(name)){
    symtab.addSymbol(name, SymbolTable::Entry{
          0, SymbolTable::Bind::LOC, SymbolTable::UNDEFINED_SECTION
        });
  }
  SymbolTable::Entry* s = symtab.getSymbol(name);
  
   switch (op)
  {
    case Instruction::OPCode::BEQ_REG_IND_DISP:
      // backpatch.push_back({LC, s, symtab.getCurrentSection()});
      memory.writeInstruction({op, PC, gpr1, gpr2});
      literalPool.put(0xfffffff2, LC, name, 
        {Instruction::OPCode::BEQ_REG_DIR_DISP, PC, gpr1, gpr2});
    break;
    case Instruction::OPCode::BNE_REG_IND_DISP:
      // location is not known, it will be known when backpatch starts
      // backpatch.push_back({LC, s, symtab.getCurrentSection()});
      memory.writeInstruction({op, PC, gpr1, gpr2});
      literalPool.put(0xfffffff3, LC,name,{Instruction::OPCode::BNE_REG_DIR_DISP, PC, gpr1, gpr2});

    break;
    case Instruction::OPCode::BGT_REG_IND_DISP:
      // location is not known, it will be known when backpatch starts
      // backpatch.push_back({LC, s, symtab.getCurrentSection() });
      memory.writeInstruction({op, PC, gpr1, gpr2});
      literalPool.put(0xfffffff4, LC,name,{Instruction::OPCode::BGT_REG_DIR_DISP, PC, gpr1, gpr2});

    break;

  default:
LOG(std::cout << "Invalid handleJustLiteralInstructions call." << std::endl;)
    return;
  }


  LC+=4;
}

// load 
void Assembler::handleLoadLiteral(Instruction::OPCode op,  uint32_t value , uint8_t gprD, uint8_t gprS  ){
  
  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
  }
   switch (op)
  {
    case Instruction::OPCode::LD_VLIT:
     // gprD <= mem32[PC+disp_to_pool]
     if(in12bSigned(value)){ 
        memory.writeInstruction({Instruction::OPCode::LD_TO_GPR_REG_DIR_DISP, 
        gprD, 0, 0, (uint16_t)value});
      break;
    }
      memory.writeInstruction({Instruction::OPCode::LD_TO_GPR_REG_IND_DISP, 
        gprD, PC, 0, 0});
      literalPool.put(value, LC);
    
    break;
    
    case Instruction::OPCode::LD_LIT: 
      // gprD <= value of literal
      // gprD <= mem32[gprD] (mem32[literal])
     if(in12bSigned(value)){ 
        memory.writeInstruction({Instruction::OPCode::LD_TO_GPR_REG_IND_DISP, 
        gprD, 0, 0, (uint16_t)value});
        break;
      }
      handleLoadLiteral(Instruction::OPCode::LD_VLIT, value, gprD);
      memory.writeInstruction({Instruction::OPCode::LD_TO_GPR_REG_IND_DISP, 
          gprD, gprD, 0, 0});
      

    break;
    
    case Instruction::OPCode::LD_IND_REG_LIT: // gprD <= mem32[gprS+disp]
      if(!in12bSigned(value)){ // std::abs((int)value) >= (1 << 13)
        throw AssemblerException("invalid displacement -> LC=" + std::to_string(LC));
      }
      memory.writeInstruction({Instruction::OPCode::LD_TO_GPR_REG_IND_DISP, 
        gprD, gprS, 0, (uint16_t)value});

    break;

  default:
LOG(std::cout << "Invalid handleLoadLiteral call." << std::endl;)
    return;
  }

  LC+=4;


}
void Assembler::handleLoadRegisters(Instruction::OPCode op,  uint8_t gprD, uint8_t gprS ){

  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
  }
   switch (op)
  {

    case Instruction::OPCode::LD_REG:
      memory.writeInstruction({Instruction::OPCode::LD_TO_GPR_REG_DIR_DISP, 
        gprD, gprS, 0, 0});
    break;
    case Instruction::OPCode::LD_IND_REG:
    memory.writeInstruction({Instruction::OPCode::LD_TO_GPR_REG_IND_DISP, 
        gprD, gprS, 0, 0});
    break;

  default:
LOG(std::cout << "Invalid handleLoadRegisters call." << std::endl;)
    return;
  }

  LC+=4;


}
void Assembler::handleLoadSymbol(Instruction::OPCode op,  std::string* name , uint8_t gprD, uint8_t gprS  ){
if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
  }

  if(!symtab.doesSymbolExist(name)){
    symtab.addSymbol(name, SymbolTable::Entry{
          0, SymbolTable::Bind::LOC, SymbolTable::UNDEFINED_SECTION
        });
  }
  SymbolTable::Entry* s = symtab.getSymbol(name);
  
   switch (op)
  {
    case Instruction::OPCode::LD_VSYM:
      
      memory.writeInstruction({Instruction::OPCode::LD_TO_GPR_REG_IND_DISP,
         gprD, PC});
      literalPool.put(0xffffffd0, LC, name,{Instruction::OPCode::LD_TO_GPR_REG_DIR_DISP, gprD, PC});
    break;

    case Instruction::OPCode::LD_SYM:

      // gprD <= val sym
      handleLoadSymbol(Instruction::OPCode::LD_VSYM, name, gprD);
      // gprD <= mem[gprD]
      memory.writeInstruction({Instruction::OPCode::LD_TO_GPR_REG_IND_DISP, 
          gprD, gprD, 0, 0});
      
    break;
    // NOT POSSIBLE
    // case Instruction::OPCode::LD_IND_REG_SYM:
    //   // if(value > 0xfff){ // CAN'T CHECK NOW
    //   //   std::cerr << "Invalid displacement." << std::endl;
    //   //   return;
    //   // }
    //   memory.writeInstruction({Instruction::OPCode::LD_TO_GPR_REG_IND_DISP, 
    //     gprD, gprS, 0, 0
    //   });
    //   backpatch.push_back({LC, s, symtab.getCurrentSection(), true});

    // break;
  default:
LOG(std::cout << "Invalid handleLoadSymbol call." << std::endl;)
    return;
  }


  LC+=4;
}
// store
void Assembler::handleStoreLiteral(Instruction::OPCode op, uint32_t value, uint8_t gprData, uint8_t gprS){

  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
  }
   switch (op)
  {
    case Instruction::OPCode::ST_LIT:
     // mem32[mem32[PC+0+Disp]] <= gprData

      if(in12bSigned(value)){ 
        memory.writeInstruction({Instruction::OPCode::ST_MEM_DIR, 
        0, 0, gprData, (uint16_t)value});
        break;
      }

      memory.writeInstruction({Instruction::OPCode::ST_MEM_IND, 
        PC, 0, gprData, 0});
      literalPool.put(value, LC);
    
    break;
    case Instruction::OPCode::ST_IND_REG_LIT:
    // mem32[gprS+0+Disp] <= gprData
      if(!in12bSigned(value)){ // std::abs((int)value) >= (1 << 13) or value > 0xfff
        throw AssemblerException("invalid displacement -> LC=" + std::to_string(LC));
      }

      memory.writeInstruction({Instruction::OPCode::ST_MEM_DIR, 
        gprS, 0, gprData, (uint16_t)value});
    break;
  

  default:
LOG(std::cout << "Invalid handleStoreLiteral call." << std::endl;)
    return;
  }

  LC+=4;

}
void Assembler::handleStoreRegisters(Instruction::OPCode op, uint8_t gprData, uint8_t gprS){

  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
  }
   switch (op)
  {

    case Instruction::OPCode::ST_REG:
      // switch places for soruce and dest
      handleLoadRegisters(Instruction::OPCode::LD_REG, gprS, gprData);
    break;
    case Instruction::OPCode::ST_IND_REG:
    memory.writeInstruction({Instruction::OPCode::ST_MEM_DIR, 
        gprS, 0, gprData, 0});
    break;

  default:
LOG(std::cout << "Invalid handleStoreRegisters call." << std::endl;)
    return;
  }

  LC+=4;
}
void Assembler::handleStoreSymbol(Instruction::OPCode op,  std::string* name, uint8_t gprData){

  if(!symtab.sectionOpened()){
    throw AssemblerException("section not opened");
  }
  if(!symtab.doesSymbolExist(name)){
    symtab.addSymbol(name, SymbolTable::Entry{
          0, SymbolTable::Bind::LOC, SymbolTable::UNDEFINED_SECTION
        });
  }
  SymbolTable::Entry* s = symtab.getSymbol(name);

   switch (op)
  {

    case Instruction::OPCode::ST_SYM:
       memory.writeInstruction({Instruction::OPCode::ST_MEM_IND, 
        PC, 0, gprData, 0});
      literalPool.put(0xffffffe0, LC, name,{Instruction::OPCode::ST_MEM_DIR, PC, 0, gprData, 0});
    break;

  default:
LOG(std::cout << "Invalid handleStoreRegisters call." << std::endl;)
    return;
  }

  LC+=4;

}



void Assembler::literalBackpatch(){
   if(literalPool.patches.empty())return;
  std::vector<std::pair<uint32_t, uint32_t> > known_literals; // location in pool, value
  // literal patch, writing PC relative displacement in instruction to literal pool

  bool can_use_relative_jump = literalPool.patches.size()*4 <= 0xfff;
  
  handleWordLiteral(0); // make space for jump instruction
  if(!can_use_relative_jump)handleWordLiteral(0); // space to read where to jump
  auto literal_pool_start = LC;
  while(!literalPool.patches.empty()){
    // get location to patch
    LiteralPool::LiteralPatch p = literalPool.patches.front();
    literalPool.patches.pop_front();
    
    // displacement 
    uint32_t displacement =   
     LC - // end of pool
     (p.instr_location+4); // pc

    bool aggregate = false;
    bool symbol_defined_in_this_section = false;

    if(literalPool.isUsingSymbol(&p)){

      SymbolTable::Entry* s =  symtab.getSymbol(&p.symbol_name);
      if(SymbolTable::isDefined(s->flags)&&
         s->ndx == symtab.current_section){ // defined in this section

        if(std::abs((int)s->offset - ((int)p.instr_location+4)) > 0xfff){
          // LOG(std::cout << "Symbol too far." << s->num << std::endl;)
          throw AssemblerException("symbol too far");
          // literalPool.put(s->offset, p.instr_location); // CHECK DISPLACEMENT FOR
          
        }
        // write displacement in instruction
LOG(std::cout << "SYMBOL DEFINED, LITERAL POOL CAN PATCH" << std::endl;)
        memory.changeInstruction(p.alternative, p.instr_location); // reg dir disp 
        symbol_defined_in_this_section = true;  
        displacement = s->offset - (p.instr_location+4); // offset to symbol in section
       
        
      }else {
LOG(std::cout << "Needs relocation." << std::endl;)
        // temporary
        //rel.put({0, s, symtab.getCurrentSection(), false});
        backpatch.push_back({LC, s, symtab.getCurrentSection()});
      }
    }else {
      // try to find literal in pool that is same as p.literal
      // to avoid repeating literals that linker won't patch
      for(auto known_literal: known_literals){
        if(known_literal.second == p.literal){
            // displacement to literal in literal pool
            displacement = known_literal.first - (p.instr_location+4);
            aggregate = true;
            break;
        }
      
      }

      known_literals.push_back({LC, p.literal});
    }

    if(!symbol_defined_in_this_section && !aggregate){ 
      // add literal to pool
      memory.writeWord(p.literal);
      LC += 4;
    }

    memory.changeInstructionDisplacement(
      p.instr_location+symtab.getSectionStart(symtab.current_section),
      displacement
    );
  }

  
  if(can_use_relative_jump){
    memory.changeInstruction({Instruction::OPCode::JMP_REG_DIR_DISP, PC, 0, 0, (uint16_t)(LC-literal_pool_start)}, literal_pool_start-4);
  }else{
    memory.changeInstruction({Instruction::OPCode::JMP_REG_IND_DISP, PC, 0, 0, 0}, literal_pool_start-8);
    memory.changeWord(LC, literal_pool_start-4);
    rel.put({literal_pool_start-4, 
      symtab.getCurrentSection(),
      symtab.getCurrentSection(),
      RelTable::T_LOC,
      LC
    });    
  }

}

void Assembler::handleSyntaxError(){
  throw AssemblerException("syntax error -> LC="+std::to_string(LC));
  
}
void Assembler::closeSection(){
  // close previous section 

  if(!symtab.sectionOpened())return;
  
  literalBackpatch();

  symtab.getCurrentSection()->size = LC;
  LC = 0;

}

void Assembler::symbolBackpatch(){
  // all values should be known except for extern symbols
  while(!backpatch.empty()){
    Backpatch p = backpatch.front();
    backpatch.pop_front();

    LOG(std::cout << 
    "====================================\n Backpatching symbol "  <<
     symtab.getSymbolName(p.symbol) << 
    " in section " << p.section->ndx <<
     " location 0x" << std::hex << p.location << std::dec << 
    std::endl;)

    if(SymbolTable::isExtern(p.symbol->flags)){
LOG(std::cout << "Extern symbol -> reallocation (section, location) " << std::endl;)
      rel.put({p.location, p.symbol, p.section});
      continue;
    }
    if(!SymbolTable::isDefined(p.symbol->flags)){
      throw AssemblerException("undefined symbol -> " + symtab.getSymbolName(p.symbol->num));
    }

LOG(std::cout << "Symbol defined and global/local. Assembler can patch." << std::endl;)
    if(p.symbol->ndx != p.section->ndx){
LOG(std::cout << ">>> Symbol in different section, needs relocation." << std::endl;)
      rel.put({p.location, 
         (p.symbol->bind == SymbolTable::Bind::LOC)?symtab.getSection(p.symbol->ndx):p.symbol,
         p.section, 
        (p.symbol->bind == SymbolTable::Bind::GLOB) ? RelTable::T_GLOB : RelTable::T_LOC, 
        (p.symbol->bind == SymbolTable::Bind::LOC)?p.symbol->offset:0});
    }
    
    //memory.print(std::cout);

    
    // write
    uint32_t loc_to_patch = p.location + symtab.getSectionStart(p.section->ndx);

    LOG(std::cout << std::hex << "(abs) 0x" << loc_to_patch << 
    " (rel) 0x" << p.location << std::dec << std::endl;)

    uint32_t value = p.symbol->offset; 
    memory.changeWord(value, loc_to_patch);
    

  }

  // std::cout << "====================================\n After patch: " << std::endl;
  // memory.print(std::cout);

}



void Assembler::handleEnd(){
  closeSection();
  symbolBackpatch();
  finished = true;
  
};

