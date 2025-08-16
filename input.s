.section a
call here
.skip 1
here: 
call there

halt

.section b
.skip 2
there:
call here
.end
