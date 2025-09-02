.global increment
.extern counter

.section biblioteka
increment:
  push %r10
  push %r1
  # increment counter
  ld counter, %r10
  ld $1, %r1
  add %r10, %r1
  st %r1, counter

  pop %r1
  pop %r10
  ret



.end
