.section a
bne %r1, %r1, 0x1
call sym
.skip 0x800
sym:

.end
