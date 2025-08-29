.extern data1
.extern lhandler, bibl
.section my_code
ld $0xf0000000, %sp
# ld $data1, %r10
# ld $lhandler, %r1
# st %r1, [%r10+0x4]
# csrwr %r1, %handler
ld data1, %r1
call bibl
# int
# ld $0x69, %r1


halt

.end
