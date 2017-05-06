@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@**************************************************************

@**********************************************************************
@  initial_recursive_conv  function
@**********************************************************************
@short Autocorr(
@		short  x[],            /* (i)    : Input signal (L_WINDOW)            */
@		short  m,              /* (i)    : LPC order                          */
@		int    R[],            /* (o)    : Autocorrelations                   */
@		short  length          /* (i)    : windows length                     */
@	      )

#include   "voQcelpEncID.h"

	.section  .text
        .global   Autocorr_s
        .extern   HAMMING_TAB
@******************************
@ ARM register 
@******************************
@ x[]                  RN           0
@ m                    RN           1
@ R[]                  RN           2
@ length               RN           3

Autocorr_s:

          STMFD          r13!, {r4 - r12, r14}
          SUB            r13, r13, #400         @ malloc y[LPCSIZE] space        
          MOV            r8,  #0                @ i = 0
          MOV            r10, r13               @ copy y address
          LDR            r5,  Table             @ get HAMMING_TAB address
	  MOV            r4, #0x4000
	  VDUP.S32       Q15, r4                
          MOV            r4, r13
	  MOV            r7, #3

LOOP:
	  @Windowing of signal
	  VLD1.S16       {Q0, Q1}, [r0]!
	  VLD1.S16       {Q2, Q3}, [r0]!
          VLD1.S16       {D8, D9}, [r0]!        @ load 40 x[]

	  VLD1.S16       {Q5, Q6}, [r5]!
	  VLD1.S16       {Q7, Q8}, [r5]!
	  VLD1.S16       {D18, D19}, [r5]!      @ load 40 HAMMING_TAB[]

	  VMULL.S16      Q11, D0, D10
	  VMULL.S16      Q12, D1, D11
	  VMULL.S16      Q13, D2, D12
	  VMULL.S16      Q14, D3, D13
	  VADD.S32       Q11, Q11, Q15
	  VADD.S32       Q12, Q12, Q15
	  VADD.S32       Q13, Q13, Q15
	  VADD.S32       Q14, Q14, Q15
	  
	  VSHRN.S32      D0, Q11, #15
	  VSHRN.S32      D1, Q12, #15
	  VSHRN.S32      D2, Q13, #15
	  VSHRN.S32      D3, Q14, #15
	  
	  VMULL.S16      Q11, D4, D14
	  VMULL.S16      Q12, D5, D15
	  VMULL.S16      Q13, D6, D16
	  VMULL.S16      Q14, D7, D17
	  VST1.S16       {D0, D1, D2, D3}, [r10]!
          	  
	  VADD.S32       Q11, Q11, Q15
	  VADD.S32       Q12, Q12, Q15
	  VADD.S32       Q13, Q13, Q15
	  VADD.S32       Q14, Q14, Q15
	  
	  VSHRN.S32      D0, Q11, #15
	  VSHRN.S32      D1, Q12, #15
	  VSHRN.S32      D2, Q13, #15
	  VSHRN.S32      D3, Q14, #15

          VMULL.S16      Q11, D8, D18
	  VMULL.S16      Q12, D9, D19
	  VST1.S16       {D0, D1, D2, D3}, [r10]!

	  VADD.S32       Q11, Q11, Q15
	  VADD.S32       Q12, Q12, Q15
	  VSHRN.S32      D0, Q11, #15
	  VSHRN.S32      D1, Q12, #15
	  SUBS           r7, r7, #1
	  VST1.S16       {D0, D1}, [r10]!

          BGE            LOOP

          MOV            r6, #0                  @overfl_shft = 0

LOOP_D:
          MOV            r10, r4                 @ y[]
          VLD1.S16       {Q0, Q1}, [r10]!
          VLD1.S16       {Q2, Q3}, [r10]!
          VLD1.S16       {Q4, Q5}, [r10]!
          VLD1.S16       {Q6, Q7}, [r10]!
          VLD1.S16       {Q8, Q9}, [r10]!
          VQDMULL.S16  	 Q10, D0, D0
	  VQDMLAL.S16    Q10, D1, D1
          VQDMLAL.S16    Q10, D2, D2
          VQDMLAL.S16    Q10, D3, D3
          VQDMLAL.S16    Q10, D4, D4
          VQDMLAL.S16  	 Q10, D5, D5
	  VQDMLAL.S16    Q10, D6, D6
          VQDMLAL.S16    Q10, D7, D7
          VQDMLAL.S16    Q10, D8, D8
          VQDMLAL.S16    Q10, D9, D9
          VQDMLAL.S16  	 Q10, D10, D10
	  VQDMLAL.S16    Q10, D11, D11
          VQDMLAL.S16    Q10, D12, D12
          VQDMLAL.S16    Q10, D13, D13
          VQDMLAL.S16    Q10, D14, D14
          VQDMLAL.S16  	 Q10, D15, D15
	  VQDMLAL.S16    Q10, D16, D16
          VQDMLAL.S16    Q10, D17, D17
          VQDMLAL.S16    Q10, D18, D18
          VQDMLAL.S16    Q10, D19, D19
          VLD1.S16       {Q0, Q1}, [r10]!
          VLD1.S16       {Q2, Q3}, [r10]!
          VLD1.S16       {Q4, Q5}, [r10]!
          VLD1.S16       {Q6, Q7}, [r10]!
          VLD1.S16       {Q8, Q9}, [r10]!
          VQDMLAL.S16  	 Q10, D0, D0
	  VQDMLAL.S16    Q10, D1, D1
          VQDMLAL.S16    Q10, D2, D2
          VQDMLAL.S16    Q10, D3, D3
          VQDMLAL.S16    Q10, D4, D4
          VQDMLAL.S16  	 Q10, D5, D5
	  VQDMLAL.S16    Q10, D6, D6
          VQDMLAL.S16    Q10, D7, D7
          VQDMLAL.S16    Q10, D8, D8
          VQDMLAL.S16    Q10, D9, D9
          VQDMLAL.S16  	 Q10, D10, D10
	  VQDMLAL.S16    Q10, D11, D11
          VQDMLAL.S16    Q10, D12, D12
          VQDMLAL.S16    Q10, D13, D13
          VQDMLAL.S16    Q10, D14, D14
          VQDMLAL.S16  	 Q10, D15, D15
	  VQDMLAL.S16    Q10, D16, D16
          VQDMLAL.S16    Q10, D17, D17
          VQDMLAL.S16    Q10, D18, D18
          VQDMLAL.S16    Q10, D19, D19
        
	  VPADD.S32      D20, D20, D21 
	  VPADD.S32      D20, D20, D20
          VMOV.S32       r12, D20[0]           
          
          CMP            r12, #0x7fffffff
          BNE            Lable
          ADD            r6, r6, #4               @ overfl_shft += 4
          @for(i = 0@ i < LPCSIZE@ i++)
          @{ 
          @   y[i] = y[i] >>2
          @}
          MOV          r8, #0                   @i = 0
          MOV          r10, r4
          MOV          r11, r4
LOOP_S:
          LDRSH        r3, [r10], #2
          LDRSH        r7, [r10], #2
          LDRSH        r5, [r10], #2
          LDRSH        r9, [r10], #2
          MOV          r3, r3, ASR #2
          MOV          r7, r7, ASR #2
          MOV          r5, r5, ASR #2
          MOV          r9, r9, ASR #2
          STRH         r3, [r11], #2
          STRH         r7, [r11], #2
          STRH         r5, [r11], #2
          STRH         r9, [r11], #2
          ADD          r8, r8, #4                 
          CMP          r8, #160
          BLT          LOOP_S

          BL           LOOP_D

Lable:           
          ADD         r12, r12, #1              @ sum = L_add(sum, 1L)
          STR          r12, [r2], #4             @ R[0] = sum

          MOV          r8, #1                    @ i = 1   

LOOP_O:
          MOV          r12, #0
          MOV          r10, r4                   @ y[j]
          ADD          r11, r4, r8, LSL #1       @ y[j+i]
          MOV          r9, #0                    @ j = 0
          RSB          r3, r8, #160              @ LPCSIZE - i
LOOP_I:
          LDRSH        r5, [r10], #2             @ load y[j]
          LDRSH        r7, [r11], #2             @ load y[j+i]
          ADD          r9, r9, #1
          MUL          r0, r5, r7
          CMP          r9, r3
          QDADD        r12, r12, r0

          BLT          LOOP_I
          STR          r12, [r2], #4
          ADD          r8, r8, #1
          CMP          r8, r1
          BLE          LOOP_O
         
Autocorr_end:

          MOV          r0, r6                    @return overfl_shft
          ADD          r13, r13, #400
          LDMFD        r13!, {r4 - r12, r15} 

Table:

          .word          HAMMING_TAB
          .END
