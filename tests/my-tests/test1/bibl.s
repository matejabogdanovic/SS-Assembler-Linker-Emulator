.global data1
.global function
.section bibl
function:
  # ld data2, %r1

  ret


data1:
.word 0x1
data2:
.word 0x2
.end
