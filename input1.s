.extern addRegs, handler
ld $0x05, %r2
.section my_code
    ld $0x5, %r5
    ld $0x1, %r1
    ld $0x2, %r2
    ld $0xfffffefe, %sp
    call addRegs
    ld $handler, %r1
    csrwr %r1, %handler
    int
    halt
.end
