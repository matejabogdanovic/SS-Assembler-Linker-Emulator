.global addRegs, handler
.section math
addRegs:
    add %r1, %r2 # r2<=3
ret
handler:
    ld $0x2, %r1
    and %r1, %r2
iret
.end
