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
}

%union {
  std::string* str;
  std::uint32_t num;
  int reg;
}

/*  */
%token ENDL COLON COMMA DOLLAR
%token <num> LITERAL 
%token <str> SYMBOL 

/* directives */
%token GLOBAL EXTERN SECTION WORD SKIP END
%token <str> SECTION_NAME 

%type <str> section_name_t

/* registers */
%token <reg> GPRX

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
    SYMBOL COLON { std::cout << *$1 << ":" << std::endl; delete $1;}
;  
notlabel: directive | instruction; 
directive:
    GLOBAL sym_list_global { std::cout << std::endl; } |
    EXTERN  sym_list_extern { std::cout << std::endl; } |
    SECTION section_name_t { std::cout << ".section " << *$2 << std::endl; delete $2; } |
    WORD sym_or_lit_list_word { std::cout << " <- .word " << std::endl; } |
    SKIP LITERAL { std::cout << ".skip 0x" << std::hex << $2 << std::dec  << std::endl;  } |
    END { std::cout << ".end" << std::endl; }
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
    RET {std::cout<<"ret"<< std::endl;} |
    

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
    }|

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
    ST GPRX COMMA GPRX {std::cout << "st %r"<< $2  << ", %r" << $4 << std::endl; }
;



%%
void yyerror(const char* s) {
  std::cerr << "Greška: " << s << std::endl;
}
