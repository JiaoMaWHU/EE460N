.ORIG x1200
; push R0 and R1 to the stack
ADD R6, R6, #-2
STW R0, R6, #0
ADD R6, R6, #-2
STW R1, R6, #0
ADD R6, R6, #-2
STW R2, R6, #0

LEA R2, PTlength
LDW R2, R2, #0
LEA R0, PTbaseaddr
LDW R0, R0, #0

LOOP LDW R1, R0, #0
AND R1, R1, #-2  ; can't use xFFFE, idk it's my assembler's problem or it's just not how it should be
STW R1, R0, #0
ADD R0, R0, #2
ADD R2, R2, #-1
BRp LOOP

LDW R2, R6, #0
ADD R6, R6, #2
LDW R1, R6, #0
ADD R6, R6, #2
LDW R0, R6, #0
ADD R6, R6, #2

RTI

PTbaseaddr .FILL x1000
PTlength   .FILL #128
.END
