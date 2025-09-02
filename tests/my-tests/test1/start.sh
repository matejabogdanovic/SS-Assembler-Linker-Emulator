ASSEMBLER=../../../asembler
LINKER=../../../linker
EMULATOR=../../../emulator

${ASSEMBLER} -o main.o main.s
${ASSEMBLER} -o bibl1.o bibl1.s
${ASSEMBLER} -o bibl2.o bibl2.s
${ASSEMBLER} -o handler.o handler.s

# INDIRECT LINKING
# make bibl.relocatable
${LINKER} -relocatable \
   -place=biblioteka@0x40000000 \
   -o bibl.relocatable \
    bibl1.o bibl2.o
# make main.relocatable
${LINKER} -relocatable \
   -place=my_code@0x40000000 -place=invalidsection@0x0 \
   -o main.relocatable \
    main.o  handler.o
# make program.hex
${LINKER} -hex \
   -place=my_code@0x40000000  -place=handler@0x10000000  \
   -o program.hex \
    main.relocatable bibl.relocatable
# DIRECT LINKING
# ${LINKER} -hex \
#    -place=my_code@0x40000000  -place=handler@0x10000000  \
#    -o program.hex \
#     main.o handler.o bibl1.o bibl2.o



${EMULATOR} program.hex