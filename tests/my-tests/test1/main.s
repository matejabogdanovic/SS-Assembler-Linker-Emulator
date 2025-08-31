.extern symbol2
.section my_code

jmp symbol2


halt


.section handler
symbol1:
add %r1, %r1
halt


.end
