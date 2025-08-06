%{
#include "parser.hpp"  
#include <iostream>
#include <string>
extern int yyerror(const char*);
%}

%option outfile="misc/lexer.cpp" header-file="misc/lexer.hpp"
%option noyywrap


whitespace [ \t]+ 
endl [\n]+
colon ":"

symbol [a-zA-Z_][a-zA-Z0-9_]*


/* directives */
end ".end"
section ".section"


%%


{whitespace} { };

{endl} { return ENDL;}

{colon} { return COLON;}

{symbol} { yylval.str = new std::string(yytext); return SYMBOL;}

{end} { return END; }

{section} return SECTION;


. {yyerror("Unknown symbol.");} 
%%

