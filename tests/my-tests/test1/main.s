
.section my_code

jmp symbol1


halt


.section handler
symbol1:
add %r1, %r1
halt


.end
