.extern sym
.section a
.skip 2
call simbol

.section b
.skip 1
simbol:
call sym
.end
