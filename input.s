.extern sym
.section b
.skip 1
simbol:
call sym

.section a
.skip 2
call simbol


.end
