.section my_code
my_start:
    ld $0xFFFFFEFE, %sp

wait:
    ld my_counter, %r1
    ld $20, %r2
    bne %r1, %r2, wait
    halt

.section my_data
my_counter:
.word 0

.end
