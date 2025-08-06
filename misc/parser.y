%{
#include <iostream>
#include <string>

int yylex();
void yyerror(const char *s);
%}

%defines "misc/parser.hpp"
%output "misc/parser.cpp"

%code requires {
    #include <string>
}

%union {
  std::string* str;
}

/*  */
%token ENDL COLON 
%token <str> SYMBOL 

/* directives */
%token GLOBAl EXTERN SECTION WORD SKIP END
%token <str> SECTION_NAME 

%type <str> section_name_t 
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
    GLOBAl { std::cout << ".global " << std::endl; } |
    EXTERN  { std::cout << ".extern " << std::endl; } |
    SECTION section_name_t { std::cout << ".section " << *$2 << std::endl; delete $2; } |
    WORD { std::cout << ".word " << std::endl; } |
    SKIP { std::cout << ".skip "  << std::endl;  } |
    END { std::cout << ".end" << std::endl; }
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

;

%%
void yyerror(const char* s) {
  std::cerr << "Greška: " << s << std::endl;
}
