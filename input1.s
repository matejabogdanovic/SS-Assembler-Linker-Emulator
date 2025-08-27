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
    halt
.section my_data
.skip 8
data:
.skip 4
.end
