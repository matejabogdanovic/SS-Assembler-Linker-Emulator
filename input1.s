.section my_code
    # inicijalno: svi registri = 0
    ld [%r1+0x2], %r2

    not   %r1        # r1 = ~0 = 0xFF
    not   %r2        # r2 = ~0 = 0xFF

    add   %r2, %r1   # r1 = 0xFF + 0xFF = 0xFE
    sub   %r2, %r1   # r1 = 0xFE - 0xFF = 0xFF  (reset na punu vrednost)

    not   %r0        # r0 = ~0 = 0xFF
    mul   %r0, %r2   # r2 = r2 * r0 = 0xFF * 0xFF = 0x01 (8-bit wrap)

    div   %r0, %r2   # r2 = r2 / r0 = 0x01 / 0xFF = 0x00  (jer 1/255 = 0 u celobrojnom delu)

    not   %r0        # r0 = ~0xFF = 0x00

    and   %r1, %r2   # r2 = r2 & r1 = 0x00 & 0xFF = 0x00
    or    %r1, %r2   # r2 = r2 | r1 = 0x00 | 0xFF = 0xFF
    xor   %r1, %r2   # r2 = r2 ^ r1 = 0xFF ^ 0xFF = 0x00

    shl   %r1, %r2   # r2 = r2 << r1 (0 << 0xFF = 0x00)
    shr   %r1, %r2   # r2 = r2 >> r1 (0 >> 0xFF = 0x00)

    xchg  %r1, %r12  # zamena r1 i r12



.end
