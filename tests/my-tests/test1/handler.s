.extern increment
.global handler
.section my_handler
handler:
    push %r1
    push %r2
    csrrd %cause, %r1
    ld $2, %r2
    beq %r1, %r2, handle_timer
    ld $3, %r2
    beq %r1, %r2, handle_terminal
finish:
    pop %r2
    pop %r1
    iret

handle_timer:
    
    jmp finish

handle_terminal:
    call increment
    jmp finish
    
.end
