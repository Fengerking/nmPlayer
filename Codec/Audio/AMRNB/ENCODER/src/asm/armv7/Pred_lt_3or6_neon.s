;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;void Pred_lt_3or6 (
;    Word16 exc[],     /* in/out: excitation buffer                         */
;    Word16 T0,        /* input : integer pitch lag                         */
;    Word16 frac,      /* input : fraction of lag                           */
;    Word16 flag3      /* input : if set, upsampling rate = 3 (6 otherwise) */
;)

        PRESERVE8
	AREA	|.text|, CODE, READONLY
        EXPORT Pred_lt_3or6_asm 
        IMPORT inter_6
;******************************
; ARM register 
;******************************
; exc[]                RN                 r0
; T0                   RN                 r1
; frac                 RN                 r2
; flag3                RN                 r3

Pred_lt_3or6_asm     FUNCTION

        STMFD           r13!, {r4 - r12, r14}  
        SUB             r8, r0, r1, LSL #1             ; get the x0 = &exc[-T0]
        RSB             r2, r2, #0                     ; frac = -(frac)
        CMP             r3, #0
        BEQ             Lable1
        MOV             r2, r2, LSR #1                 ; frac = (frac >>1);
Lable1
        CMP             r2, #0
        BGE             Lable2
        ADD             r2, r2, #6                     ; frac = frac + UP_SAMP_MAX
        SUB             r8, r8, #2                     ; x0--
Lable2
        RSB             r9, r2, #6                     ; frac2 = UP_SAMP_MAX - frac
        LDR             r5, Table
        
        MOV             r10, #40                       ; j = L_SUBFR
        ADD             r6, r5, r2, LSL #1             ; c1 = &inter_6[frac]
        ADD             r7, r5, r9, LSL #1             ; c2 = &inter_6[frac2]
        
        LDRSH           r12, [r6]                      ; c1[0]
        LDRSH           r14, [r7]                      ; c2[0]
        VDUP.S16        D24, r12
        VDUP.S16        D25, r14
        VMOV.S32        Q11, #0x8000

        ADD             r12, r8, #2
        
        VLD1.S16        D0, [r8]!
        VLD1.S16        D10, [r12]!
        VQDMULL.S16     Q10, D0, D24
        VQDMLAL.S16     Q10, D10, D25
        VADDHN.S32      D0, Q10, Q11       
        VST1.S16        D0, [r0]!
        
        VLD1.S16        D1, [r8]!
        VLD1.S16        D11, [r12]!
        VQDMULL.S16     Q10, D1, D24
        VQDMLAL.S16     Q10, D11, D25
        VADDHN.S32      D1, Q10, Q11    
        VST1.S16        D1, [r0]!
    
        VLD1.S16        D2, [r8]!
        VLD1.S16        D12, [r12]!
        VQDMULL.S16     Q10, D2, D24
        VQDMLAL.S16     Q10, D12, D25
        VADDHN.S32      D2, Q10, Q11       
        VST1.S16        D2, [r0]!

        VLD1.S16        D3, [r8]!
        VLD1.S16        D13, [r12]!
        VQDMULL.S16     Q10, D3, D24
        VQDMLAL.S16     Q10, D13, D25
        VADDHN.S32      D3, Q10, Q11     
        VST1.S16        D3, [r0]!
     
        VLD1.S16        D4, [r8]!
        VLD1.S16        D14, [r12]!
        VQDMULL.S16     Q10, D4, D24
        VQDMLAL.S16     Q10, D14, D25
        VADDHN.S32      D4, Q10, Q11      
        VST1.S16        D4, [r0]!
     
        VLD1.S16        D5, [r8]!
        VLD1.S16        D15, [r12]!
        VQDMULL.S16     Q10, D5, D24
        VQDMLAL.S16     Q10, D15, D25
        VADDHN.S32      D5, Q10, Q11     
        VST1.S16        D5, [r0]!
     
        VLD1.S16        D6, [r8]!
        VLD1.S16        D16, [r12]!
        VQDMULL.S16     Q10, D6, D24
        VQDMLAL.S16     Q10, D16, D25
        VADDHN.S32      D6, Q10, Q11     
        VST1.S16        D6, [r0]!
       
        VLD1.S16        D7, [r8]!
        VLD1.S16        D17, [r12]!
        VQDMULL.S16     Q10, D7, D24
        VQDMLAL.S16     Q10, D17, D25
        VADDHN.S32      D7, Q10, Q11   
        VST1.S16        D7, [r0]!
      
        VLD1.S16        D8, [r8]!
        VLD1.S16        D18, [r12]!
        VQDMULL.S16     Q10, D8, D24
        VQDMLAL.S16     Q10, D18, D25
        VADDHN.S32      D8, Q10, Q11      
        VST1.S16        D8, [r0]!
        
        VLD1.S16        D9, [r8]!
        VLD1.S16        D19, [r12]!
        VQDMULL.S16     Q10, D9, D24
        VQDMLAL.S16     Q10, D19, D25
        VADDHN.S32      D9, Q10, Q11   
        VST1.S16        D9, [r0]! 

Pred_lt_3or6_asm_end 
        LDMFD           r13!, {r4 - r12, r15}
        ENDFUNC
Table
        DCD      inter_6

        END
