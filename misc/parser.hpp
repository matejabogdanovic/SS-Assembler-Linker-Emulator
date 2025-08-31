/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_MISC_PARSER_HPP_INCLUDED
# define YY_YY_MISC_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 12 "misc/parser.y"

    #include <cstdint>
    #include <string>
    #include "../inc/assembler/Assembler.hpp"
    #include "../inc/common/Macros.hpp"

#line 55 "misc/parser.hpp"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ENDL = 258,
    COLON = 259,
    COMMA = 260,
    DOLLAR = 261,
    LBRACKET = 262,
    RBRACKET = 263,
    PLUS = 264,
    LITERAL = 265,
    SYMBOL = 266,
    ASCII_STRING = 267,
    GLOBAL = 268,
    EXTERN = 269,
    SECTION = 270,
    WORD = 271,
    SKIP = 272,
    END = 273,
    ASCII = 274,
    SECTION_NAME = 275,
    GPRX = 276,
    CSR = 277,
    HALT = 278,
    INT = 279,
    IRET = 280,
    RET = 281,
    CALL = 282,
    JMP = 283,
    BEQ = 284,
    BNE = 285,
    BGT = 286,
    PUSH = 287,
    POP = 288,
    XCHG = 289,
    ADD = 290,
    SUB = 291,
    MUL = 292,
    DIV = 293,
    NOT = 294,
    AND = 295,
    OR = 296,
    XOR = 297,
    SHL = 298,
    SHR = 299,
    LD = 300,
    ST = 301,
    CSRRD = 302,
    CSRWR = 303
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 19 "misc/parser.y"

  std::string* str;
  std::uint32_t num;
  int reg;

#line 121 "misc/parser.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_MISC_PARSER_HPP_INCLUDED  */
