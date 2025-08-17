.section a
# push %r2
# pop %r4
# call b
# call 0x0f
beq %r1, %r2, 0x12345678
bne %r1, %r2, 0x12345678
bgt %r1, %r2, 0x12345678
beq %r3, %r4, b
bne %r3, %r4, b
bgt %r3, %r4, b
beq %r3, %r4, a
bne %r3, %r4, a
bgt %r3, %r4, a
# jmp b
# jmp 0x69
.skip 1
b:

 .skip 2
 
 push %r2

# iret
# ret
.section b
jmp b
.skip 1
a:
.end
