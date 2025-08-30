

.section my_code
ld $0x1, %r1
st  %r1, 0xfffff9ff
halt

.section data
.word 0x6969

.end
