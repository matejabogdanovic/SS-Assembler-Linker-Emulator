.global lhandler
.section handler
lhandler:
ld data1, %r11


iret 

.section my_data
data1:
.word 0xff1
data2:
.word 0xff2

.end
