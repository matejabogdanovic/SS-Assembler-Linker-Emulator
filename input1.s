.extern data, end
.section my_code
    # inicijalno: svi registri = 0
    
     ld $0x1, %r1
    ld $0xfffffefe, %sp
     push %r1
    ld $0x2, %r2
     push %r2
     pop %r1
     pop %r1
    st %sp, 0x1
    ld 0x1, %r7
    csrwr %sp, %status
    csrrd %status, %r5
    ld $0x1, %r1

     beq %r1, %r1, 0x40000054
.end
