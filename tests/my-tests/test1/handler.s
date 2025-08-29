.global lhandler
.section handler
lhandler:
ld data1, %r11


iret 

.section my_data
data1:
.word 0x3
data2:
.word 0x4

.end
