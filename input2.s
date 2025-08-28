.global data, end
.section my_code
    div %r0, %r13
    ld data, %r1
end: halt

.section my_data

data:
.word 0x1111

.end
