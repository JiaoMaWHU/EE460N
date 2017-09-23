.orig x3000
ADD R2, R2, #2
ADD R1, R1, #2
LEA R0, C
LDW R0, R0, #0
LDW R1, R0, #0 ; unaligned load, MAR<-3501 but internally made the address 3500, so should write 0 to R1
STW R2, R0, #0 ; unaligned store, should exit with error

C .FILL x3501
.END
