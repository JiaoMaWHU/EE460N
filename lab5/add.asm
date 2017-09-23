      .ORIG x3000
      LEA R0, data
      LDW R0, R0, #0
      LEA R1, twenty
      LDW R1, R1, #0 ; counter
      AND R2, R2, #0 ; sum  22cyc

LOOP  LDB R3, R0, #0 ; 41 cyc
      ADD R0, R0, #1 ;
      ADD R2, R2, R3 ;
      ADD R1, R1, #-1 ;
      BRp LOOP        ; 23 cyc
      STW R2, R0, #0 ;

      ; jump to this sum as an address
      JMP R2
      HALT


data .FILL xC000   ;PA 0x3800
twenty .FILL #20
.END
