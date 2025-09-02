.extern indirectIncrement, increment, handler
.global counter

.section data2
text:
  .ascii "="
  
.section my_code
_start:
 
  ld $0xFFFFFEFE, %sp
  ld $handler, %r1
  csrwr %r1, %handler

  call indirectIncrement
  call increment

  # now press anything 2 times to incerement 2 more times
  wait:
    ld counter, %r1
    ld $4, %r2
    bne %r1, %r2, wait
   
  # arguments in %r1 for example
  ld counter, %r1
  call print   

  halt
  

# =4
# -----------------------------------------------------------------
# Emulated processor executed halt instruction
# Emulated processor state:
#   r0=0x00000000  r1=0x00000004  r2=0x00000004  r3=0x00000000
#   r4=0x00000000  r5=0x00000000  r6=0x00000000  r7=0x00000000
#   r8=0x00000000  r9=0x00000000 r10=0x00000000 r11=0x00000000
#  r12=0x00000000 r13=0x00000000 r14=0xFFFFFEFE r15=0x40000034


  


# print '=2'
print: # in %r1 is argument that is number to print
  
  push %r1
  push %r10
  push %r2
  
  ld term_out, %r10 # r1 <= 0xffffff00

  ld text, %r2  # r2 <= ascii(=)
  st  %r2, [%r10] # print
  
  ld $48, %r2 # 48 decimal, if your assembler doesn't support, change to hex
  add %r2, %r1 # 48 + x = ascii for number x

  st  %r1, [%r10] # print

  pop %r2
  pop %r10
  pop %r1

  ret

.section data
counter:
  .word 0
term_out:
  .word 0xffffff00

  

.end
