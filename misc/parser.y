%{
#include <iostream>
#include <string>
 #include <cstdint>
int yylex();
void yyerror(const char *s);
%}

%defines "misc/parser.hpp"
%output "misc/parser.cpp"

%code requires {
    #include <cstdint>
    #include <string>
    #include "../inc/assembler/Assembler.hpp"
    #include "../inc/common/Macros.hpp"
}

%union {
  std::string* str;
  std::uint32_t num;
  int reg;
}

/*  */
%token ENDL COLON COMMA DOLLAR LBRACKET RBRACKET PLUS
%token <num> LITERAL 
%token <str> SYMBOL 
%token <str> ASCII_STRING
/* directives */
%token GLOBAL EXTERN SECTION WORD SKIP END ASCII
%token <str> SECTION_NAME 

%type <str> section_name_t

/* registers */
%token <reg> GPRX
%token <reg> CSR

/* instrucions 0 args */
%token HALT
%token INT
%token IRET
%token RET

/* instrucions */
%token CALL
%token JMP
%token BEQ
%token BNE
%token BGT

%token PUSH
%token POP
%token XCHG
%token ADD
%token SUB
%token MUL
%token DIV
%token NOT
%token AND
%token OR
%token XOR
%token SHL
%token SHR
%token LD
%token ST
%token CSRRD
%token CSRWR
%%

program: 
    lines;

lines:
     line  
    | lines line;
line: 
    notlabel ENDL  |
    labels notlabel ENDL |
    labels ENDL | 
    ENDL;
labels:
    label | labels label;
label:
    SYMBOL COLON { 
        LOG(std::cout << *$1 << ":" << std::endl; );
        Assembler::handleLabel($1);

        delete $1;
    }
;  
notlabel: directive | instruction; 
directive:
    GLOBAL sym_list_global { LOG(std::cout << std::endl;); } |
    EXTERN  sym_list_extern { LOG(std::cout << std::endl;); } |
    SECTION section_name_t {
        LOG(std::cout << ".section " << *$2 << std::endl;); 
         Assembler::handleSection($2);
         
         delete $2; 
         } |
    WORD sym_or_lit_list_word { LOG(std::cout << " <- .word " << std::endl;); } |
    SKIP LITERAL { LOG(std::cout << ".skip 0x" << std::hex << $2 << std::dec  << std::endl;); 
        Assembler::handleSkip((int32_t)$2);
    } |
    END { LOG(std::cout << ".end" << std::endl;);  Assembler::handleEnd(); YYACCEPT;} |
    ASCII ASCII_STRING { LOG(std::cout << ".ascii " << *$2 << std::endl); 
        Assembler::handleAscii($2);
        delete $2;}
;
sym_list_global:
    SYMBOL {LOG(std::cout << ".global "<< *$1;);
    Assembler::handleGlobal($1);
    delete $1;}|
    sym_list_global COMMA SYMBOL { LOG(std::cout << ", " << *$3;);
    Assembler::handleGlobal($3);
    delete $3;}
;
sym_list_extern:
    SYMBOL {LOG(std::cout << ".extern "<< *$1;); 
    Assembler::handleExtern($1);
    delete $1;}|
    sym_list_extern COMMA SYMBOL { LOG(std::cout << ", " << *$3;);
    Assembler::handleExtern($3);
    delete $3;}
;

sym_or_lit_list_word:
    sym_or_lit_word 
    |
    sym_or_lit_list_word COMMA sym_or_lit_word 
;

sym_or_lit_word:
    SYMBOL  {LOG(std::cout << std::hex << *$1<< std::dec; );
    Assembler::handleWordSymbol($1);
    delete $1;}
    |
    LITERAL {LOG(std::cout << "0x" <<  std::hex <<$1<< std::dec; );
    Assembler::handleWordLiteral($1);
    }
;
section_name_t:
  SYMBOL        { $$ = $1; } |  
  SECTION_NAME  { $$ = $1; }
;
instruction: 
    HALT {LOG(std::cout<<"halt"<< std::endl;); Assembler::handleZeroArgsInstructions(Instruction::OPCode::HALT);}|
    INT {LOG(std::cout<<"int"<< std::endl;); Assembler::handleZeroArgsInstructions(Instruction::OPCode::INT);}|
    IRET {LOG(std::cout<<"iret"<< std::endl;); Assembler::handleZeroArgsInstructions(Instruction::OPCode::IRET);}|
    RET {LOG(std::cout<<"ret"<< std::endl;); Assembler::handleZeroArgsInstructions(Instruction::OPCode::RET);} 
    |
    gpr_instructions
    |
   
    jump_instructions
    |
    memory_instructions
   
;
gpr_instructions:
    CSRRD CSR COMMA GPRX { LOG(std::cout << "csrrd %csr" << $2 << ", %r" << $4 << std::endl;);
        Assembler::handleGprInstructions(Instruction::OPCode::CSRRD, $2, $4);
     }|
    CSRWR GPRX COMMA CSR { LOG(std::cout << "csrwr %r" << $2 << ", %csr" << $4 << std::endl;); 
        Assembler::handleGprInstructions(Instruction::OPCode::CSRWR, $2, $4);
    } |
    XCHG GPRX COMMA GPRX { LOG(std::cout << "xchg %r" << $2 << ", %r" << $4 << std::endl;); 
        Assembler::handleGprInstructions(Instruction::OPCode::XCHG, $2, $4);
    }|
    ADD GPRX COMMA GPRX { LOG(std::cout << "add %r" << $2 << ", %r" << $4 << std::endl;); 
        Assembler::handleGprInstructions(Instruction::OPCode::ADD, $2, $4);
    }|
    SUB GPRX COMMA GPRX { LOG(std::cout << "sub %r" << $2 << ", %r" << $4 << std::endl;); 
        Assembler::handleGprInstructions(Instruction::OPCode::SUB, $2, $4);
    }|
    MUL GPRX COMMA GPRX { LOG(std::cout << "mul %r" << $2 << ", %r" << $4 << std::endl;); 
        Assembler::handleGprInstructions(Instruction::OPCode::MUL, $2, $4);
    }|
    DIV GPRX COMMA GPRX { LOG(std::cout << "div %r" << $2 << ", %r" << $4 << std::endl;); 
        Assembler::handleGprInstructions(Instruction::OPCode::DIV, $2, $4);
    }|
    NOT GPRX {LOG(std::cout << "not %r" << $2 << std::endl;); 
        Assembler::handleGprInstructions(Instruction::OPCode::NOT, $2, $2);
    }| 
    AND GPRX COMMA GPRX { LOG(std::cout << "and %r" << $2 << ", %r" << $4 << std::endl;); 
        Assembler::handleGprInstructions(Instruction::OPCode::AND, $2, $4);
    }|
    OR GPRX COMMA GPRX { LOG(std::cout << "or %r" << $2 << ", %r" << $4 << std::endl;); 
        Assembler::handleGprInstructions(Instruction::OPCode::OR, $2, $4);
    }|
    XOR GPRX COMMA GPRX { LOG(std::cout << "xor %r" << $2 << ", %r" << $4 << std::endl;); 
        Assembler::handleGprInstructions(Instruction::OPCode::XOR, $2, $4);
    }|
    SHL GPRX COMMA GPRX { LOG(std::cout << "shl %r" << $2 << ", %r" << $4 << std::endl;); 
        Assembler::handleGprInstructions(Instruction::OPCode::SHL, $2, $4);
    }|
    SHR GPRX COMMA GPRX { LOG(std::cout << "shr %r" << $2 << ", %r" << $4 << std::endl;); 
        Assembler::handleGprInstructions(Instruction::OPCode::SHR, $2, $4);
    }
;
jump_instructions:
    /* <literal> or <symbol> => value of (<literal> or <symbol>) = address */
    CALL LITERAL { LOG(std::cout << std::hex <<"call 0x" << $2 << std::dec << std::endl;); 
 
        Assembler::handleJustLiteralInstructions(Instruction::OPCode::CALL_IND, $2);
    }|
    CALL SYMBOL { 
        LOG(std::cout << "call " << *$2<< std::endl;); 
        Assembler::handleJustSymbolInstructions(Instruction::OPCode::CALL_IND, $2);
        delete $2; 
    }|
    
    JMP LITERAL { LOG(std::cout << std::hex << "jmp 0x" << $2<< std::dec << std::endl;); 
        Assembler::handleJustLiteralInstructions(Instruction::OPCode::JMP_REG_IND_DISP, $2);
    }|
    JMP SYMBOL { LOG(std::cout << "jmp " << *$2<< std::endl;); 
        Assembler::handleJustSymbolInstructions(Instruction::OPCode::JMP_REG_IND_DISP, $2);
        delete $2; 
    }|
    
    BEQ GPRX COMMA GPRX COMMA LITERAL { 
        LOG(std::cout << "beq %r"<< $2 << ", %r" << $4 <<", 0x" << std::hex << $6 << std::dec << std::endl;);
        Assembler::handleBranchLiteralInstructions(Instruction::OPCode::BEQ_REG_IND_DISP, $2, $4, $6);
    }|
    BEQ GPRX COMMA GPRX COMMA SYMBOL { 
        LOG(std::cout << "beq %r"<< $2 << ", %r" << $4 <<", " <<  *$6 << std::endl;);
        Assembler::handleBranchSymbolInstructions(Instruction::OPCode::BEQ_REG_IND_DISP, $2, $4, $6);
        delete $6;
    }|
    BNE GPRX COMMA GPRX COMMA LITERAL { 
        LOG(std::cout << "bne %r"<< $2 << ", %r" << $4 <<", 0x" << std::hex << $6 << std::dec << std::endl;);
        Assembler::handleBranchLiteralInstructions(Instruction::OPCode::BNE_REG_IND_DISP, $2, $4, $6);
    }|
    BNE GPRX COMMA GPRX COMMA SYMBOL { 
        LOG(std::cout << "bne %r"<< $2 << ", %r" << $4 <<", " <<  *$6 << std::endl;);
        Assembler::handleBranchSymbolInstructions(Instruction::OPCode::BNE_REG_IND_DISP, $2, $4, $6);
        delete $6;
    }|
    BGT GPRX COMMA GPRX COMMA LITERAL { 
        LOG(std::cout << "bgt %r"<< $2 << ", %r" << $4 <<", 0x" << std::hex << $6 << std::dec << std::endl;);
        Assembler::handleBranchLiteralInstructions(Instruction::OPCode::BGT_REG_IND_DISP, $2, $4, $6);
    }|
    BGT GPRX COMMA GPRX COMMA SYMBOL { 
        LOG(std::cout << "bgt %r"<< $2 << ", %r" << $4 <<", " <<  *$6 << std::endl;);
        Assembler::handleBranchSymbolInstructions(Instruction::OPCode::BGT_REG_IND_DISP, $2, $4, $6);
        delete $6;
    }
;

memory_instructions:
    PUSH GPRX {LOG(std::cout<<"push %r" << $2 << std::endl;);
        Assembler::handleStackInstructions(Instruction::OPCode::PUSH, $2);
    }|
    POP GPRX {LOG(std::cout<<"pop %r" << $2 << std::endl;);
        Assembler::handleStackInstructions(Instruction::OPCode::POP, $2);
    }|

    /* $<literal> or $<symbol> => value of (<literal> or <symbol>) = data */
    LD DOLLAR LITERAL COMMA GPRX {LOG(std::cout << "ld $0x" << std::hex << $3 << std::dec << ", %r" << $5 << std::endl;); 
        Assembler::handleLoadLiteral(Instruction::OPCode::LD_VLIT, $3,  $5);
    }|
    LD DOLLAR SYMBOL COMMA GPRX {LOG(std::cout << "ld $" << *$3 << ", %r" << $5 << std::endl;);
        Assembler::handleLoadSymbol(Instruction::OPCode::LD_VSYM,  $3 , $5);
        delete $3; }|
    /*ST GPRX COMMA DOLLAR LITERAL {LOG(std::cout << "st %r"<< $2  << ", $" << std::hex << $5 << std::dec << std::endl;); }|
    ST GPRX COMMA DOLLAR SYMBOL {LOG(std::cout << "st %r"<< $2  << ", $" << std::hex << *$5 << std::dec << std::endl;); delete $5; }|
    */
    /* <literal> or <symbol> => mem[value of (<literal> or <symbol>)] = data */
    LD LITERAL COMMA GPRX {LOG(std::cout << "ld 0x" << std::hex << $2 << std::dec << ", %r" << $4 << std::endl;); 
        Assembler::handleLoadLiteral(Instruction::OPCode::LD_LIT, $2,  $4);
    }|
    LD SYMBOL COMMA GPRX {LOG(std::cout << "ld " << *$2 << ", %r" << $4 << std::endl;);
        Assembler::handleLoadSymbol(Instruction::OPCode::LD_SYM,  $2 , $4);
        delete $2; 
    }|
    ST GPRX COMMA LITERAL {LOG(std::cout << "st %r"<< $2  << ", " << std::hex << $4 << std::dec << std::endl;); 
        Assembler::handleStoreLiteral(Instruction::OPCode::ST_LIT, $4, $2);
    }|
    ST GPRX COMMA SYMBOL {LOG(std::cout << "st %r"<< $2  << ", " << std::hex << *$4 << std::dec << std::endl;);  
        Assembler::handleStoreSymbol(Instruction::OPCode::ST_SYM,  $4, $2 );
        delete $4;
    }|
  
    /* %<reg> => value in (<reg>) = data */
    LD GPRX COMMA GPRX {LOG(std::cout << "ld %r" <<  $2 <<  ", %r" << $4 << std::endl;); 
        Assembler::handleLoadRegisters(Instruction::OPCode::LD_REG,   $4 , $2  );
    }|
    ST GPRX COMMA GPRX {LOG(std::cout << "st %r"<< $2  << ", %r" << $4 << std::endl;); 
        Assembler::handleStoreRegisters(Instruction::OPCode::ST_REG, $2, $4);
    }|

    /* [%<reg>] => mem[value in (<reg>)] = data */
    LD LBRACKET GPRX RBRACKET COMMA GPRX {LOG(std::cout << "ld [%r"<< $3  << "], %r" << $6 << std::endl;); 
    Assembler::handleLoadRegisters(Instruction::OPCode::LD_IND_REG,   $6 , $3  );
    }|
    ST GPRX COMMA LBRACKET GPRX RBRACKET {LOG(std::cout << "st %r" <<  $2 <<  ", [%r" << $5 <<"]" << std::endl;); 
    Assembler::handleStoreRegisters(Instruction::OPCode::ST_IND_REG, $2, $5);
    
    }|

    /* [%<reg> + <literal>] => mem[value in (<reg>) + value of(<literal>)] = data */
    LD LBRACKET GPRX PLUS LITERAL RBRACKET COMMA GPRX 
    {LOG(std::cout << "ld [%r"<< $3  <<" + 0x" << std::hex << $5 << std::dec <<"], %r" << $8 << std::endl;); 
    Assembler::handleLoadLiteral(Instruction::OPCode::LD_IND_REG_LIT, $5,  $8, $3);
    
    }|
    ST GPRX COMMA LBRACKET GPRX PLUS LITERAL RBRACKET 
    {LOG(std::cout << "st %r" <<  $2 <<  ", [%r" << $5 <<" + 0x" << std::hex << $7 << std::dec << "]" << std::endl;); 
    Assembler::handleStoreLiteral(Instruction::OPCode::ST_IND_REG_LIT, $7, $2, $5);
    
    }
    

    /* [%<reg> + <symbol>] => mem[value in (<reg>) + value of(<symbol>)] = data */
    /*LD LBRACKET GPRX PLUS SYMBOL RBRACKET COMMA GPRX 
    {LOG(std::cout << "ld [%r"<< $3  <<" + " << *$5 << "], %r" << $8 << std::endl;); 
        Assembler::handleLoadSymbol(Instruction::OPCode::LD_IND_REG_SYM, $5,  $8, $3);
        delete $5; }|
    
    ST GPRX COMMA LBRACKET GPRX PLUS SYMBOL RBRACKET 
    {LOG(std::cout << "st %r" <<  $2 <<  ", [%r" << $5 <<" + " <<  *$7 <<  "]" << std::endl;); delete $7; }
    */
;

%%
void yyerror(const char* s) {

  Assembler::handleSyntaxError();
}

