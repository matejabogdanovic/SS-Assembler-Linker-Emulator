 .extern data
.section my_code
    # inicijalno: svi registri = 0
    
    ld $0x1, %r1
    ld $data, %sp
    push %r1
    ld $0x2, %r2
    push %r2
    pop %r1
    pop %r1
    st %sp, data
    csrwr %sp, %status
    csrrd %status, %r5
    ld $0x1, %r1

.end
