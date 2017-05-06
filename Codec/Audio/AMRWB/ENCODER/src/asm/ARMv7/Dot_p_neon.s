;/*
;*  Copyright 2003 ~ 2009 by Visualon software Incorporated.
;*  All rights reserved. Property of Visualon software Incorporated.
;*  Restricted rights to use, duplicate or disclose this code are
;*  granted through contract.
;*  
;*/

;/***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver         Description             Author
;*    -----------     --------     ------------------        -----------
;*    07-10-2008        1.0        File imported from        Huaping Liu
;*                                             
;**********************************************************************/
;Word32 Dot_product12(                      /* (o) Q31: normalized result (1 < val <= -1) */
;       Word16 x[],                           /* (i) 12bits: x vector                       */
;       Word16 y[],                           /* (i) 12bits: y vector                       */
;       Word16 lg,                            /* (i)    : vector length                     */
;       Word16 * exp                          /* (o)    : exponent of result (0..+30)       */
;)
;***********************************************************************
;  x[]   ---  r0
;  y[]   ---  r1
;  lg    ---  r2
;  *exp  ---  r3

          AREA    |.text|, CODE, READONLY
          EXPORT  voAMRWBEncDot_product12_asm

voAMRWBEncDot_product12_asm FUNCTION
          STMFD   	    r13!, {r4 - r12, r14} 
          VLD1.S16          {Q0, Q1}, [r0]!               ;load 16 Word16 x[]
          VLD1.S16          {Q2, Q3}, [r0]!               ;load 16 Word16 x[]
          VLD1.S16          {Q4, Q5}, [r0]!               ;load 16 Word16 x[]
          VLD1.S16          {Q6, Q7}, [r0]!               ;load 16 Word16 x[]
	  VLD1.S16          {Q8, Q9}, [r1]!               ;load 16 Word16 y[]
	  VLD1.S16          {Q10, Q11}, [r1]!             ;load 16 Word16 y[]
	  VLD1.S16          {Q12, Q13}, [r1]!             ;load 16 Word16 y[]

          VQDMULL.S16       Q15, D16, D0
          VQDMLAL.S16       Q15, D17, D1               
          VQDMLAL.S16       Q15, D18, D2
          VQDMLAL.S16       Q15, D19, D3
	  VLD1.S16          {Q0, Q1}, [r1]!               ;load 16 Word16 y[]   
          VQDMLAL.S16       Q15, D20, D4       
          VQDMLAL.S16       Q15, D21, D5
          VQDMLAL.S16       Q15, D22, D6
          VQDMLAL.S16       Q15, D23, D7                                       
          VQDMLAL.S16       Q15, D24, D8
          VQDMLAL.S16       Q15, D25, D9
          VQDMLAL.S16       Q15, D26, D10
          VQDMLAL.S16       Q15, D27, D11
          VQDMLAL.S16       Q15, D0, D12
          VQDMLAL.S16       Q15, D1, D13
          VQDMLAL.S16       Q15, D2, D14
          VQDMLAL.S16       Q15, D3, D15

          CMP               r2, #64
          BEQ               Lable1
          VLD1.S16          {Q0, Q1}, [r0]!               ;load 16 Word16 x[]  
	  VLD1.S16          {Q2, Q3}, [r1]! 
          VQDMLAL.S16       Q15, D4, D0             
          VQDMLAL.S16       Q15, D5, D1
          VQDMLAL.S16       Q15, D6, D2
          VQDMLAL.S16       Q15, D7, D3
Lable1              
          VQADD.S32         D30, D30, D31
          VPADD.S32         D30, D30, D30
          VMOV.S32          r12, D30[0]        

          ADD               r12, r12, #1                         ; L_sum = (L_sum << 1)  + 1
	  MOV               r4, r12
	  CMP               r12, #0
	  RSBLT             r4, r12, #0
          CLZ               r10, r4
          SUB               r10, r10, #1                         ; sft = norm_l(L_sum)
          MOV               r0, r12, LSL r10                     ; L_sum = L_sum << sft
          RSB               r11, r10, #30                        ; *exp = 30 - sft
          STRH              r11, [r3]                     

Dot_product12_end
		     
          LDMFD   	    r13!, {r4 - r12, r15} 
          ENDFUNC

          END

