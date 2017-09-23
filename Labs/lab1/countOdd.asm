;  Name 1: Xinyuan (Allen) Pan
;  Name 2: Ze Lyu
;  UTEID 1: xp572
;  UTEID 2: zl5298
;  There is a problem in this code
; (x-(x>>1)<<1)[0] == 1 ? odd : even
             .ORIG x3000
             LEA R0, addrNum  ; R0<-addrNum
             LDW R0, R0, #0   ; R0<- M[addrNum] = addr
             AND R3, R3, #0   ; R3 = 0
             LEA R4, addrCount
             LDW R4, R4, #0   ; R4 = x4100
             NOT R4, R4
             ADD R4, R4, #1   ; R4= -x4100

loop         LDB R1, R0, #0   ; R1<- M[addr] = num
             RSHFL R2, R1, #1
             LSHF R2, R2, #1  ; R2 is the same as R1 except bit[0]
             NOT R2, R2
             ADD R2, R2, #1   ; -R2
             ADD R2, R1, R2   ; 1 or 0
             BRz even
             ADD R3, R3, #1
even         ADD R0, R0, #1   ; increment addr
             ADD R5, R0, R4; check if finished all numbers
             BRz done
             BR loop

done         STB R3, R0, #0
             HALT
addrNum     .FILL x4000
addrCount   .FILL x4100       ; also the address right after all numbers
            .END
