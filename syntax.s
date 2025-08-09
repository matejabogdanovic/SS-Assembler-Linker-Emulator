# directives
.global symbol_list1
.global sym1, sym2, a, b, _23 # 43a_ - can't start with a number

.extern symbol_list1
.extern sym1, sym2

.section 123_sectionname_343.4
.section .text.2text
.section .text
.section section
.section .
.section ....

.word 1
.word symbol_or_literals
.word sym1, a, b, 0x123, 2
.word 1, 2, 3, symbols

.skip 1 # literal only
.skip 0x2


# .ascii <string>
# .equ <symbol>, <expression>

.end

label1:
label2:
_label: # label is same as symbol, can't start with number
a: b: halt

halt
int 
iret
ret

call 1234
call 0x01
call label

jmp 1
jmp 0x1
jmp label

beq %sp, %pc, 0x05 
beq %r1, %r3, label
bne %r1, %r3, 0x05 
bne %r1, %r3, label
bgt %r1, %r3, 0x05 
bgt %r1, %r3, label

push %r11
pop %r0

ld $0x1, %r15
ld $symbol, %r0

st %sp, $15
st %r11, $symbol

ld 0x1, %r15
ld symbol, %r0

st %sp, 15
st %r11, symbol

ld %r2, %pc
st %pc, %r2

st %r2, [%pc]
ld [%r1], %r2

st %r2, [%pc+1]
ld [%r1+2], %r2

st %r2, [%pc+label]
ld [%r1+label], %r2

add %r1, %pc
sub %r1, %r2
mul %r1, %r2
div %r1, %r2
not %sp
and %r1, %r2
or %r1, %r2
xor %r1, %r2
shl %r1, %r2
shr %r1, %r2

xchg %r1, %r12
csrrd %status, %sp
csrwr %pc, %handler
csrwr %pc, %cause

