ASSEMBLER=../../../asembler
LINKER=../../../linker
EMULATOR=../../../emulator

${ASSEMBLER} -o main.o main.s
${ASSEMBLER} -o handler.o handler.s

 ${LINKER} -hex \
   -place=my_code@0x40000000 -place=handler@0x10  -place=my_data@0x70000000 \
   -o program.hex \
   main.o handler.o
${EMULATOR} program.hex