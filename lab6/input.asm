.ORIG x3000
AND R0,R0,x0000 ;x3000 R0<--0
ADD R0,R0,x7 ;x3002 R0 <-- x7 RAW
AND R1,R0,x-2 ; R1 <-- x6 RAW
LEA R2, A ; R2 <-- x3036
LDW R3, R2, #0 ; R3 <-- x4000 RAW
LDW R4, R2, #1 ; R4 <-- xFFFF N=1 mem_stall
LSHF R4, R4, #8 ; R4 <-- xFF00 RAW
RSHFL R3, R3, #3 ; R3 <-- x800 P = 1
RSHFA R3, R3, #1 ; R3 <-- x400 RAW
RSHFA R4, R4, #10 ; R4 <-- xFFFF !icache_r
ADD R5, R4, R0 ; R5 <--- x6 p=1 RAW
XOR R5, R5, R1 ; R5 <--- 0 z=1 RAW
BRz B ; PC <-- x302c control flow + CC Dependency

D     LEA R0, C ; x301a r0<-303e
LDW R0, R0, #0; R0<-- x3500 RAW
STW R1, R0, #0 ; Mx3500 <-- 0010 RAW
STW R1, R0, #-2 ;Mx34FC < -- 0010
STB R1, R0, #2 ; Mx3502 <-- x0010
STB R1, R0, #3 ; Mx3502 <-- x1010
JSR E          ; PC<-3040 R7<-x3028
LEA R0, F  ; x3028 R0<-3046
JSRR R0 ; PC<-3046 R7<-302c RAW

B     LEA R0, C ; x302c R0<--303e
LDB R1, R0, #-4 ; R1 <-- x0001  RAW
LDB R1, R0, #-3 ; R1 <-- x0010
LEA R2, D ; r2<-x301a
JMP R2   ; PC <-- x301a control flow, RAW

A     .FILL x4000 ;3036
.FILL xFFFF ;3038
.FILL x1001 ; 303a
.FILL x0000 ; 303c
C     .FILL x3500 ; 303e

E     AND R0, R0, x0000 ; x3040
ADD R0, R0, x07 ; R0 <-- 7 RAW
RET ; PC<-3028 control flow

F    HALT ; x3046


.END
