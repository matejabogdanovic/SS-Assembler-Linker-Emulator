.extern addRegs
.section my_code
    ld $0x1, %r1
    ld $0x2, %r2
    call addRegs
.end
