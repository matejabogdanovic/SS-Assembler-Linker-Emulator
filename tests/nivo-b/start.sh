ASSEMBLER=../../asembler
LINKER=../../linker
EMULATOR=../../emulator

${ASSEMBLER} -o main.o main.s
${ASSEMBLER} -o handler.o handler.s
${ASSEMBLER} -o isr_terminal.o isr_terminal.s
${ASSEMBLER} -o isr_timer.o isr_timer.s

${LINKER} -relocatable \
  -place=my_code@0x40000000 \
  -o programlib.o \
  main.o isr_terminal.o isr_timer.o handler.o

${LINKER} -hex \
  -place=my_code@0x40000000 \
  -o program.hex \
  programlib.o

# ${LINKER} -hex \
#   -place=my_code@0x40000000 \
#   -o program.hex \
#   main.o isr_terminal.o isr_timer.o handler.o

${EMULATOR} program.hex