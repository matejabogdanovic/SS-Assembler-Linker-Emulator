.section a
push %r2
pop %r1
call b
call 0x0f
# jmp b
# jmp 0x0f
b:
iret
ret
.section b
.skip 1
a:
.end
