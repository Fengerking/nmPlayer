;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
; Word32 voAMRNBDecsum_armv7_xx(s, s_excf[], s_excf[], Length)
; {
;   for (j = 0; j < L_subfr; j++) {
;	s = L_mac (s, s_excf[j], s_excf[j]);
; }

	AREA	|.text|, CODE, READONLY
        EXPORT voAMRNBDecsum_armv7_xx

;******************************
; constant
;******************************

;******************************
; ARM register 
;******************************
; r0 --- s
; r1 --- s_excf[]
; r2 --- s_excf[]
; r3 --- length         
voAMRNBDecsum_armv7_xx FUNCTION

        STMFD      r13!, {r1 - r12, r14} 
        VLD4.S16       {D0, D1, D2, D3}, [r1]!
        VMOV.S32       Q10, #0
        VLD4.S16       {D4, D5, D6, D7}, [r1]!
        VLD2.S16       {D8, D9}, [r1]!       
        VQDMLAL.S16    Q10, D0, D0
        VQDMLAL.S16    Q10, D1, D1
        VQDMLAL.S16    Q10, D2, D2
        VQDMLAL.S16    Q10, D3, D3
        VQDMLAL.S16    Q10, D4, D4
        VQDMLAL.S16    Q10, D5, D5
        VQDMLAL.S16    Q10, D6, D6
        VQDMLAL.S16    Q10, D7, D7
        VQDMLAL.S16    Q10, D8, D8
        VQDMLAL.S16    Q10, D9, D9

        CMP        r3, #40
        BEQ        END
         
        VLD4.S16       {D0, D1, D2, D3}, [r1]!
        VLD4.S16       {D4, D5, D6, D7}, [r1]!
        VLD2.S16       {D8, D9}, [r1]!          
        VQDMLAL.S16    Q10, D0, D0
        VQDMLAL.S16    Q10, D1, D1
        VQDMLAL.S16    Q10, D2, D2
        VQDMLAL.S16    Q10, D3, D3
        VQDMLAL.S16    Q10, D4, D4
        VQDMLAL.S16    Q10, D5, D5
        VQDMLAL.S16    Q10, D6, D6
        VQDMLAL.S16    Q10, D7, D7
        VQDMLAL.S16    Q10, D8, D8
        VQDMLAL.S16    Q10, D9, D9
        
END 
        VQADD.S32      D21, D21, D20
        VEXT.32        D20, D21, D21, #1
        VQADD.S32      D21, D21, D20
        VMOV.S32       r0, D21[0]

sum_armv7_xx_end
 
        LDMFD      r13!, {r1 - r12, r15}
        ENDP
        END
