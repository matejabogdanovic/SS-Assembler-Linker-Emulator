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

/* directives */
%token GLOBAL EXTERN SECTION WORD SKIP END
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
        std::cout << *$1 << ":" << std::endl; 
        Assembler::handleLabel($1);

        delete $1;
    }
;  
notlabel: directive | instruction; 
directive:
    GLOBAL sym_list_global { std::cout << std::endl; } |
    EXTERN  sym_list_extern { std::cout << std::endl; } |
    SECTION section_name_t {
         std::cout << ".section " << *$2 << std::endl; 
         Assembler::handleSection($2);
         
         delete $2; 
         } |
    WORD sym_or_lit_list_word { std::cout << " <- .word " << std::endl; } |
    SKIP LITERAL { std::cout << ".skip 0x" << std::hex << $2 << std::dec  << std::endl; 
        Assembler::handleSkip((int32_t)$2);
    } |
    END { std::cout << ".end" << std::endl;  Assembler::handleEnd(); }
;
sym_list_global:
    SYMBOL {std::cout << ".global "<< *$1; delete $1;}|
    sym_list_global COMMA SYMBOL { std::cout << ", " << *$3; delete $3;}
;
sym_list_extern:
    SYMBOL {std::cout << ".extern "<< *$1; delete $1;}|
    sym_list_extern COMMA SYMBOL { std::cout << ", " << *$3; delete $3;}
;

sym_or_lit_list_word:
    sym_or_lit_word 
    |
    sym_or_lit_list_word COMMA sym_or_lit_word 
;

sym_or_lit_word:
    SYMBOL  {std::cout << std::hex << *$1<< std::dec; delete $1;}
    |
    LITERAL {std::cout << "0x" <<  std::hex <<$1<< std::dec; }
;
section_name_t:
  SYMBOL        { $$ = $1; } |  
  SECTION_NAME  { $$ = $1; }
;
instruction: 
    HALT {std::cout<<"halt"<< std::endl;}|
    INT {std::cout<<"int"<< std::endl;}|
    IRET {std::cout<<"iret"<< std::endl;}|
    RET {std::cout<<"ret"<< std::endl;} 
    |
    alu_instructions
    |
    XCHG GPRX COMMA GPRX { std::cout << "xchg %r" << $2 << ", %r" << $4 << std::endl; }|
    CSRRD CSR COMMA GPRX { std::cout << "csrrd %csr" << $2 << ", %r" << $4 << std::endl; }|
    CSRWR GPRX COMMA CSR { std::cout << "csrwr %r" << $2 << ", %csr" << $4 << std::endl; }
    |
    jump_instructions
    |
    memory_instructions
   
;
alu_instructions:
    ADD GPRX COMMA GPRX { std::cout << "add %r" << $2 << ", %r" << $4 << std::endl; }|
    SUB GPRX COMMA GPRX { std::cout << "sub %r" << $2 << ", %r" << $4 << std::endl; }|
    MUL GPRX COMMA GPRX { std::cout << "mul %r" << $2 << ", %r" << $4 << std::endl; }|
    DIV GPRX COMMA GPRX { std::cout << "div %r" << $2 << ", %r" << $4 << std::endl; }|
    NOT GPRX {std::cout << "not %r" << $2 << std::endl; }| 
    AND GPRX COMMA GPRX { std::cout << "and %r" << $2 << ", %r" << $4 << std::endl; }|
    OR GPRX COMMA GPRX { std::cout << "or %r" << $2 << ", %r" << $4 << std::endl; }|
    XOR GPRX COMMA GPRX { std::cout << "xor %r" << $2 << ", %r" << $4 << std::endl; }|
    SHL GPRX COMMA GPRX { std::cout << "shl %r" << $2 << ", %r" << $4 << std::endl; }|
    SHR GPRX COMMA GPRX { std::cout << "shr %r" << $2 << ", %r" << $4 << std::endl; }
;
jump_instructions:
    /* <literal> or <symbol> => value of (<literal> or <symbol>) = address */
    CALL LITERAL { std::cout << std::hex <<"call 0x" << $2 << std::dec << std::endl; }|
    CALL SYMBOL { std::cout << "call " << *$2<< std::endl; delete $2; }|
    
    JMP LITERAL { std::cout << std::hex << "jmp 0x" << $2<< std::dec << std::endl; }|
    JMP SYMBOL { std::cout << "jmp " << *$2<< std::endl; delete $2; }|
    
    BEQ GPRX COMMA GPRX COMMA LITERAL { 
        std::cout << "beq %r"<< $2 << ", %r" << $4 <<", 0x" << std::hex << $6 << std::dec << std::endl;
    }|
    BEQ GPRX COMMA GPRX COMMA SYMBOL { 
        std::cout << "beq %r"<< $2 << ", %r" << $4 <<", " <<  *$6 << std::endl;
        delete $6;
    }|
    BNE GPRX COMMA GPRX COMMA LITERAL { 
        std::cout << "bne %r"<< $2 << ", %r" << $4 <<", 0x" << std::hex << $6 << std::dec << std::endl;
    }|
    BNE GPRX COMMA GPRX COMMA SYMBOL { 
        std::cout << "bne %r"<< $2 << ", %r" << $4 <<", " <<  *$6 << std::endl;
        delete $6;
    }|
    BGT GPRX COMMA GPRX COMMA LITERAL { 
        std::cout << "bgt %r"<< $2 << ", %r" << $4 <<", 0x" << std::hex << $6 << std::dec << std::endl;
    }|
    BGT GPRX COMMA GPRX COMMA SYMBOL { 
        std::cout << "bgt %r"<< $2 << ", %r" << $4 <<", " <<  *$6 << std::endl;
        delete $6;
    }
;

memory_instructions:
    PUSH GPRX {std::cout<<"push %r" << $2 << std::endl;}|
    POP GPRX {std::cout<<"pop %r" << $2 << std::endl;}|

    /* $<literal> or $<symbol> => value of (<literal> or <symbol>) = data */
    LD DOLLAR LITERAL COMMA GPRX {std::cout << "ld $0x" << std::hex << $3 << std::dec << ", %r" << $5 << std::endl; }|
    LD DOLLAR SYMBOL COMMA GPRX {std::cout << "ld $" << *$3 << ", %r" << $5 << std::endl; delete $3; }|
    ST GPRX COMMA DOLLAR LITERAL {std::cout << "st %r"<< $2  << ", $" << std::hex << $5 << std::dec << std::endl; }|
    ST GPRX COMMA DOLLAR SYMBOL {std::cout << "st %r"<< $2  << ", $" << std::hex << *$5 << std::dec << std::endl; delete $5; }|
   
    /* <literal> or <symbol> => mem[value of (<literal> or <symbol>)] = data */
    LD LITERAL COMMA GPRX {std::cout << "ld 0x" << std::hex << $2 << std::dec << ", %r" << $4 << std::endl; }|
    LD SYMBOL COMMA GPRX {std::cout << "ld " << *$2 << ", %r" << $4 << std::endl; delete $2; }|
    ST GPRX COMMA LITERAL {std::cout << "st %r"<< $2  << ", " << std::hex << $4 << std::dec << std::endl; }|
    ST GPRX COMMA SYMBOL {std::cout << "st %r"<< $2  << ", " << std::hex << *$4 << std::dec << std::endl; delete $4; }|
  
    /* %<reg> => value in (<reg>) = data */
    LD GPRX COMMA GPRX {std::cout << "ld %r" <<  $2 <<  ", %r" << $4 << std::endl; }|
    ST GPRX COMMA GPRX {std::cout << "st %r"<< $2  << ", %r" << $4 << std::endl; }|

    /* [%<reg>] => mem[value in (<reg>)] = data */
    LD LBRACKET GPRX RBRACKET COMMA GPRX {std::cout << "ld [%r"<< $3  << "], %r" << $6 << std::endl; }|
    ST GPRX COMMA LBRACKET GPRX RBRACKET {std::cout << "st %r" <<  $2 <<  ", [%r" << $5 <<"]" << std::endl; }|

    /* [%<reg> + <literal>] => mem[value in (<reg>) + value of(<literal>)] = data */
    LD LBRACKET GPRX PLUS LITERAL RBRACKET COMMA GPRX 
    {std::cout << "ld [%r"<< $3  <<" + 0x" << std::hex << $5 << std::dec <<"], %r" << $8 << std::endl; }|
    ST GPRX COMMA LBRACKET GPRX PLUS LITERAL RBRACKET 
    {std::cout << "st %r" <<  $2 <<  ", [%r" << $5 <<" + 0x" << std::hex << $7 << std::dec << "]" << std::endl; }|

    /* [%<reg> + <symbol>] => mem[value in (<reg>) + value of(<symbol>)] = data */
    LD LBRACKET GPRX PLUS SYMBOL RBRACKET COMMA GPRX 
    {std::cout << "ld [%r"<< $3  <<" + " << *$5 << "], %r" << $8 << std::endl; delete $5; }|
    ST GPRX COMMA LBRACKET GPRX PLUS SYMBOL RBRACKET 
    {std::cout << "st %r" <<  $2 <<  ", [%r" << $5 <<" + " <<  *$7 <<  "]" << std::endl; delete $7; }

;

%%
void yyerror(const char* s) {
  std::cerr << "Greška: " << s << std::endl;
}
