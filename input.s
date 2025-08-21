.extern e
.section a
.skip 1
call 0x1
call 0xfffffffff1
jmp e
beq %r1, %r2, 0x1
call b
.section b
.skip 2
b:
.end
