.section my_code
    # inicijalno: svi registri = 0
    
    ld $0x1, %r1
    ld $0x2, %r2
    ld $0x3, %r3
    csrwr %r1, %status
    csrrd %status, %r10 
    csrwr %r2, %handler
    csrrd %handler, %r11
    csrwr %r3, %cause
    csrrd %cause, %r12 
   
    halt

.end
