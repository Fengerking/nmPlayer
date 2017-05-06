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

        .section  .text
	.global   Autocorr_s
	.extern   HAMMING_TAB

@******************************
@ constant
@******************************


@******************************
@ ARM register 
@******************************
@ x[]                  RN           0
@ m                    RN           1
@ R[]                  RN           2
@ length               RN           3

Autocorr_s:

          STMFD        r13!, {r4 - r12, r14}
          SUB          r13, r13, #400         @ malloc y[LPCSIZE] space
          MOV          r4, r13          
          MOV          r8,  #0                @ i = 0
          MOV          r10, r4                @ copy y address
          LDR          r5,  Table             @ get HAMMING_TAB address

LOOP_W:
          LDR          r11, [r0], #4   
          LDR          r3,  [r5], #4
          LDR          r6,  [r5], #4
          LDR          r12, [r0], #4

          SMULBB       r14, r3, r11
          SMULTT       r9,  r3, r11
          ADD          r14, r14, #0x4000
          ADD          r9, r9, #0x4000
          MOV          r14, r14, ASR #15
          MOV          r9, r9, ASR #15
          STRH         r14, [r10], #2
          STRH         r9,  [r10], #2

          SMULBB       r14, r6, r12
          SMULTT       r9,  r6, r12
          ADD          r14, r14, #0x4000
          ADD          r9, r9, #0x4000
          MOV          r14, r14, ASR #15
          MOV          r9, r9, ASR #15
          STRH         r14, [r10], #2
          STRH         r9,  [r10], #2

          ADD          r8, r8, #4
          CMP          r8, #160
          BLT          LOOP_W

          MOV          r6, #0                  @overfl_shft = 0

LOOP_D:
          MOV          r10, r4                 @ y[]
          MOV          r12, #0                 @ sum = 0L
          MOV          r8, #0                  @ i = 0
LOOP_F:
          LDR          r3, [r10], #4
          LDR          r7, [r10], #4
          LDR          r5, [r10], #4
          LDR          r9, [r10], #4
          SMULBB       r11, r3, r3
          SMULTT       r14, r3, r3
          QDADD        r12, r12, r11
          SMULBB       r11, r7, r7
          QDADD        r12, r12, r14
          SMULTT       r14, r7, r7
          QDADD        r12, r12, r11
          SMULBB       r11, r5, r5
          QDADD        r12, r12, r14 
          SMULTT       r14, r5, r5
          QDADD        r12, r12, r11
          SMULBB       r11, r9, r9
          QDADD        r12, r12, r14
          SMULTT       r14, r9, r9
          QDADD        r12, r12, r11
          ADD          r8, r8, #8
          QDADD        r12, r12, r14           


          CMP          r8, #160
          BLT          LOOP_F

          CMP          r12, #0x7fffffff
          BNE          Lable
          ADD          r6, r6, #4               @ overfl_shft += 4
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
