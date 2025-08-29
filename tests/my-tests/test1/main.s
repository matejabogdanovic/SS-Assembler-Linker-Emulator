.extern lhandler
.section my_code
ld $0xf0000000, %sp
ld data1, %r10

ld $lhandler, %r1
csrwr %r1, %handler

int
ld $0x69, %r1


halt


.section my_data
data1:
.word 0x1
data2:
.word 0x2
.end
