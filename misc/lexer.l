%{
#include "parser.hpp"  
#include <iostream>
#include <string>
extern int yyerror(const char*);
%}

%option outfile="misc/lexer.cpp" header-file="misc/lexer.hpp"
%option noyywrap


whitespace [ \t]+
comment "#".* 
endl [\n]+
colon ":"
symbol [a-zA-Z_][a-zA-Z0-9_]*
section_name [\.a-zA-Z0-9_]+

%%

".global" {return GLOBAl;}
".extern" {return EXTERN;}
".section" {return SECTION;}
".word" {return WORD;}
".skip" {return SKIP;}
".end" { return END;}


"halt" {return HALT;}
"int" {return INT;}
"iret" {return IRET;}
"ret" {return RET;}

"call" {return CALL;}
"jmp" {return JMP;}
"beq" {return BEQ;}
"bne" {return BNE;}
"bgt" {return BGT;}
"push" {return PUSH;}
"pop" {return POP;}
"xchg" {return XCHG;}
"add" {return ADD;}
"sub" {return SUB;}
"mul" {return MUL;}
"div" {return DIV;}
"not" {return NOT;}
"and" {return AND;}
"or" {return OR;}
"xor" {return XOR;}
"shl" {return SHL;}
"shr" {return SHR;}
"ld" {return LD;}
"st" {return ST;}
"csrrd" {return CSRRD;}
"csrwr" {return CSRWR;}


{comment} { std::cout<<"(komentar)"; }

{whitespace} {  }

{endl} { return ENDL;}

{colon} { return COLON;}

{symbol} { yylval.str = new std::string(yytext); return SYMBOL;}

{section_name} {yylval.str = new std::string(yytext); return SECTION_NAME;}

. {yyerror("Unknown symbol.");} 
%%

