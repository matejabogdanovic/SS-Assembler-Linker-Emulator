.section my_section_1
.global a, b
.extern  a, b, c, d, sym1, sym_vb1
.skip 0xf

label1: .section .gas 

.word 1, 0x2, simbol
lab:
call 15
call a
jmp 0x1
jmp a
beq %r1 , %r0 , 0x05 
beq %r1 , %r0 , labela_simbol1
bne %r1 , %r0 , 0x05 
bne %r1 , %r0 , labela_simbol1
bgt %r1 , %r0 , 0x05 
bgt %r1 , %r0 , labela_simbol1
int
halt
iret
ret
ld $0x1, %r15
ld $konstant4, %r0
st %sp, $15
st %r11, $konstanta
ld 0x1, %r15
ld konstant4, %r0
st %sp, 15
st %r11, konstanta

ld %r2, %pc
st %pc, %r2

st %r2, [%pc]
ld [%r1], %r2

st %r2, [%pc+1]
ld [%r1+2], %r2

st %r2, [%pc+labela1]
ld [%r1+labela3], %r2

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

csrrd %status, %sp
csrwr %pc, %handler
csrwr %pc, %cause

push %r11
pop %r12
lab4:

xchg %r1, %r12

.section 1s

.section sectionotAr
# komentar neki kao

lab2: 
lab3: .section .13sect.io321n_2
label2: label3: halt
.end
# .section
