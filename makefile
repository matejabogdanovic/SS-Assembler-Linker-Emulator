SRC = src
INC = inc
MISC = misc
TESTS = tests

ASSEMBLER_DIR = $(SRC)/assembler
ASSEMBLER_FILES = $(ASSEMBLER_DIR)/*.cpp

LINKER_DIR = $(SRC)/linker
LINKER_FILES = $(LINKER_DIR)/*.cpp

all: asembler linker

asembler: $(ASSEMBLER_FILES)
	g++ -fdiagnostics-color=always -g -o ${@} ${^}

linker: $(LINKER_FILES)
	g++ -fdiagnostics-color=always -g -o ${@} ${^}
	
clean:
	rm -f asembler linker



	

