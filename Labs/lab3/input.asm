.ORIG x3000
AND R0,R0,x0000 ; 9 R0<--0
ADD R0,R0,x7 ; 18 R0 <-- x7
AND R1,R0,x-2 ; 27 R1 <-- x6
LEA R2, A ; 36 R2 <-- x3036
LDW R3, R2, #0 ; 51 MAR<--3036 R3 <-- x4000
LDW R4, R2, #1 ; 66 MAR<--3038 R4 <-- xFFFF N=1
LSHF R4, R4, #8 ; 75 R4 <-- xFF00
RSHFL R3, R3, #3 ; 84 R3 <-- x800 P = 1
RSHFA R3, R3, #1 ; 93 R3 <-- x400
RSHFA R4, R4, #10 ; 102 R4 <-- xFFFF
ADD R5, R4, R0 ; 111 R5 <--- x6 p=1
XOR R5, R5, R1 ; 120 R5 <--- 0 z=1
BRz B ; 130 PC <-- x302c

D     LEA R0, C ; 196 x301a r0<-303e
LDW R0, R0, #0; 211 MAR<-303e MDR<-3500 R0<-- x3500
STW R1, R0, #0 ; 226 MAR<-x3500 MDR<-x0010 Mx3500 <-- 0010
STW R1, R0, #-2 ; 241 MAR<-x34FC MDR<-x0010 Mx34FC < -- 0010
STB R1, R0, #2 ; 256 MAR<-x3502 (251)BUS<-0010 MDR<-x1010 Mx3502 <-- x0010
STB R1, R0, #3 ; 271 MAR<-x3503 (266)BUS<-0010 MDR<-x1010 x3502 <-- x1010
JSR E          ; 281 PC<-3040 R7<-x3028
LEA R0, F  ; 317 x3028 R0<-3046
JSRR R0 ; 327 PC<-3046 R7<-302c

B     LEA R0, C ; 139 x302c R0<--303e
LDB R1, R0, #-4 ; 154 MAR<-303a MDR<-1001 R1 <-- x0001
LDB R1, R0, #-3 ; 169 MAR<-303b MDR<-1001 R1 <-- x0010
LEA R2, D ; 178 r2<-x301a
JMP R2   ; 187 PC <-- x301a

A     .FILL x4000 ;3036
.FILL xFFFF ;3038
.FILL x1001 ; 303a
.FILL x0000 ; 303c
C     .FILL x3500 ; 303e

E     AND R0, R0, x0000 ; 290 x3040
ADD R0, R0, x07 ; 299 R0 <-- 7
RET ; 308 PC<-3028

F     HALT ;342 x3046


.END
