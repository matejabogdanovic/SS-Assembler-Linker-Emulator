.section a

# .skip 1
# .word a
# a:
# int
# call 0x12345678


xchg %r1, %r2
add %r1, %r2
sub %r1, %r2
mul %r1, %r2
div %r1, %r2
not %r1
and %r1, %r2
or %r1, %r2
xor %r1, %r2
shl %r1, %r2
shr %r1, %r2

halt
.end

