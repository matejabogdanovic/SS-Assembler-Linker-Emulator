ASSEMBLER=../../../asembler
LINKER=../../../linker
EMULATOR=../../../emulator

${ASSEMBLER} -o main.o main.s
${ASSEMBLER} -o bibl1.o bibl1.s
${ASSEMBLER} -o bibl2.o bibl2.s
 ${LINKER} -relocatable \
   -place=my_code@0x40000000 \
   -o bibl.o \
    bibl1.o bibl2.o

 ${LINKER} -hex \
   -place=my_code@0x40000000  \
   -o bad1.hex \
    main.o bibl2.o

 ${LINKER} -hex \
   -place=my_code@0x40000000  \
   -o bad2.hex \
    main.o bibl1.o

${LINKER} -hex \
   -place=my_code@0x40000000  \
   -o program.hex \
    main.o bibl.o
${EMULATOR} program.hex