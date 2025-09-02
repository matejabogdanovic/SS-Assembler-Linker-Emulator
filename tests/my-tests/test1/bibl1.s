.extern  increment
.global indirectIncrement

.section biblioteka
indirectIncrement:

  call increment
  ret




.end
