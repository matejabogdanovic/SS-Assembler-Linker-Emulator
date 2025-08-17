.section a
# push %r2
# pop %r4
# call b
# call 0x0f

 jmp b
 jmp 0x69
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
