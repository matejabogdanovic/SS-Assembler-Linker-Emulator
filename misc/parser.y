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
%token SECTION END


%%

program: 
    lines;

lines:
    line  
    | lines line;

line:

    notlabel ENDL  |
    label ENDL |
    label notlabel ENDL  ;
label:
    SYMBOL COLON { std::cout << *$1 << ":" << std::endl; delete $1;};
notlabel: directive ; 
directive:
    SECTION SYMBOL { std::cout << ".section " << *$2 << std::endl; delete $2; } |
    END { std::cout << ".end" << std::endl; }
;



    


%%
void yyerror(const char* s) {
  std::cerr << "Greška: " << s << std::endl;
}
