SRC = src
INC = inc
MISC = misc
TESTS = tests

ASSEMBLER_DIR = $(SRC)/assembler
ASSEMBLER_FILES = $(ASSEMBLER_DIR)/*.cpp

LINKER_DIR = $(SRC)/linker
LINKER_FILES = $(LINKER_DIR)/*.cpp

LEXER_GENERATOR=  $(MISC)/lexer.l
PARSER_GENERATOR = $(MISC)/parser.y

LEXER_CPP= $(MISC)/lexer.cpp
PARSER_CPP = $(MISC)/parser.cpp

LEXER_HPP= $(MISC)/lexer.hpp
PARSER_HPP = $(MISC)/parser.hpp

all: parsing asembler linker

parsing: 
	bison -d $(PARSER_GENERATOR)
	flex $(LEXER_GENERATOR)

asembler: parsing  
	g++ -fdiagnostics-color=always -g -o ${@} $(ASSEMBLER_FILES) $(PARSER_CPP) $(LEXER_CPP)

linker: $(LINKER_FILES)
	g++ -fdiagnostics-color=always -g -o ${@} ${^}
	
clean:
	rm -f asembler linker $(PARSER_CPP) $(LEXER_CPP) $(PARSER_HPP) $(LEXER_HPP)



	

