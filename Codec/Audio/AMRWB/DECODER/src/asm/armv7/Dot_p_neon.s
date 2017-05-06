;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;*************************************************************** 
;Word32 Dot_product12(                      /* (o) Q31: normalized result (1 < val <= -1) */
;       Word16 x[],                           /* (i) 12bits: x vector                       */
;       Word16 y[],                           /* (i) 12bits: y vector                       */
;       Word16 lg,                            /* (i)    : vector length                     */
;       Word16 * exp                          /* (o)    : exponent of result (0..+30)       */
;)
;****************************************************************
;  x[]   ---  r0
;  y[]   ---  r1
;  lg    ---  r2
;  *exp  ---  r3

          AREA    |.text|, CODE, READONLY
          EXPORT  Dot_product12_asm

Dot_product12_asm FUNCTION

          STMFD   	    r13!, {r4 - r12, r14} 
          VLD1.S16          {Q0, Q1}, [r0]!               ;load 16 Word16 x[]
          VLD1.S16          {Q2, Q3}, [r0]!               ;load 16 Word16 x[]
          VLD1.S16          {Q4, Q5}, [r0]!               ;load 16 Word16 x[]
          VLD1.S16          {Q6, Q7}, [r0]!               ;load 16 Word16 x[]
          VQDMULL.S16       Q15, D0, D0
          VQDMLAL.S16       Q15, D1, D1
          VQDMLAL.S16       Q15, D2, D2
          VQDMLAL.S16       Q15, D3, D3
          VQDMLAL.S16       Q15, D4, D4
          VQDMLAL.S16       Q15, D5, D5
          VQDMLAL.S16       Q15, D6, D6
          VQDMLAL.S16       Q15, D7, D7                                       
          VQDMLAL.S16       Q15, D8, D8
          VQDMLAL.S16       Q15, D9, D9
          VQDMLAL.S16       Q15, D10, D10
          VQDMLAL.S16       Q15, D11, D11
          VQDMLAL.S16       Q15, D12, D12
          VQDMLAL.S16       Q15, D13, D13
          VQDMLAL.S16       Q15, D14, D14
          VQDMLAL.S16       Q15, D15, D15

          CMP               r2, #64
          BEQ               Lable
          VLD1.S16          {Q0, Q1}, [r0]!               ;load 16 Word16 x[]   
          VQDMLAL.S16       Q15, D0, D0             
          VQDMLAL.S16       Q15, D1, D1
          VQDMLAL.S16       Q15, D2, D2
          VQDMLAL.S16       Q15, D3, D3
Lable              
          VQADD.S32         D30, D30, D31
          VPADD.S32         D30, D30, D30
          VMOV.S32          r12, D30[0]        

          ADD               r12, r12, #1                         ; L_sum = (L_sum << 1)  + 1
          CLZ               r10, r12
          SUB               r10, r10, #1                         ; sft = norm_l(L_sum)
          MOV               r0, r12, LSL r10                     ; L_sum = L_sum << sft
          RSB               r11, r10, #30                        ; *exp = 30 - sft
          STRH              r11, [r3]                     

Dot_product12_end
		     
          LDMFD   	    r13!, {r4 - r12, r15} 
          ENDFUNC
          END
 

