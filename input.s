.section a
ld 0xfff, %r1
ld 0x1fff, %r1
ld [%r1 + 0x32], %r2
ld %r1, %r2
ld [%r1], %r2
.skip 1
.end
