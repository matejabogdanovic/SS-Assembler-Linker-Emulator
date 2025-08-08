.section my_section_1
.global a, b
.extern  a, b, c, d, sym1, sym_vb1
.skip 0xf

label1: .section .gas 

.word 1, 0x2, simbol
lab:
call 15
call a
jmp -0x1
jmp a
beq %r1 , %r0 , 0x05 
beq %r1 , %r0 , labela_simbol1
bne %r1 , %r0 , 0x05 
bne %r1 , %r0 , labela_simbol1
bgt %r1 , %r0 , 0x05 
bgt %r1 , %r0 , labela_simbol1
int
ld $-0x1, %r15
ld $konstant4, %r0
st %sp, $15
st %r11, $konstanta
ld -0x1, %r15
ld konstant4, %r0
st %sp, 15
st %r11, konstanta

ld %r2, %pc
st %pc, %r2
lab4:

.section 1s

.section sectionotAr
# komentar neki kao

lab2: 
lab3: .section .13sect.io321n_2
label2: label3: halt
.end
# .section
