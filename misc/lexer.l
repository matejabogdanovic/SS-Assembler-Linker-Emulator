%{
#include "parser.hpp"  
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdint.h>
extern int yyerror(const char*);
%}

%option outfile="misc/lexer.cpp" header-file="misc/lexer.hpp"
%option noyywrap


whitespace [ \t]+
comment "#".* 
endl [\n]+
colon ":"
comma ","
dollar "$"
lbracket "["
rbracket "]"
plus "+"

literal (([0-9]+|(0[Xx])[0-9A-Fa-f]+))
symbol [a-zA-Z_][a-zA-Z0-9_]*
section_name [\.a-zA-Z0-9_]+

gprx ("%sp"|"%pc"|("%r"[0|1|2|3|4|5|6|7|8|9])|("%r1"[0|1|2|3|4|5]))
csr ("%status"|"%handler"|"%cause")
%%

".global" {return GLOBAL;}
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


{comment} { }

{whitespace} {  }

{endl} { return ENDL;}

{colon} { return COLON;}

{comma} { return COMMA;}

{dollar} { return DOLLAR;}

{plus} {return PLUS;}

{lbracket} { return LBRACKET; }

{rbracket} { return RBRACKET; }

{literal} { yylval.num = (uint32_t) strtoul(yytext, NULL, 0); return LITERAL; }

{gprx} { 
  std::string r = yytext;
 
  if(r=="%sp")yylval.reg = 14;
  else if (r=="%pc")yylval.reg = 15;
  else yylval.reg =  (uint8_t)strtoul((yytext+2), NULL, 10);

  return GPRX; 
}

{csr} {
  std::string r = yytext;
 
  if(r=="%status")yylval.reg = 1;
  else if (r=="%handler")yylval.reg = 2;
  else yylval.reg = 3; // cause

  return CSR; 
}

{symbol} { yylval.str = new std::string(yytext); return SYMBOL;}

{section_name} {yylval.str = new std::string(yytext); return SECTION_NAME;}

. {yyerror("Unknown symbol.");} 
%%

