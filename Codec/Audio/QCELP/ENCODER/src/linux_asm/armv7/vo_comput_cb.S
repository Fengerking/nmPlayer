@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@**************************************************************

@**********************************************************************
@  vo_comput_cb  function
@**********************************************************************
@void vo_comput_cb(
@		int		Exy,
@		int		Eyy,
@		int             *err,
@               int             *G
@		)

#include   "voQcelpEncID.h"

	.section  .text
        .global   vo_comput_cb
        .extern   divide_s_cb

@******************************
@ ARM register 
@******************************
@ Exy                   RN           0
@ Eyy                   RN           1
@ *err                  RN           2
@ *G                    RN           3

vo_comput_cb:

          STMFD      r13!, {r4 - r12, r14}
          @if ((ABS(Exy) < 21)||(Eyy < 21))
          SUB        r13, r13, #40

  
          CMP        r0, #0
          MOVGT      r10, r0
          RSBLE      r10, r0, #0
          CMP        r10, #0x15
          BLT        CONST_Label
          CMP        r1, #0x15
          BLT        CONST_Label
          STR        r2, [r13, #20]
          STR        r3, [r13, #24]  
          CMP        r0, #0
          BLT        L_label
          CLZ        r4, r0
          SUB        r4, r4, #2                  @ n_shiftcnt = norm_l(Exy) - 1
          CLZ        r5, r1
          SUB        r5, r5, #1                  @ d_shiftcnt = norm_l(Eyy)
          MOV        r6, r0, LSL r4
          MOV        r6, r6, ASR #16             @ extract_h(Exy << n_shiftcnt)
          MOV        r7, r1, LSL r5
          MOV        r7, r7, ASR #16             @ extract_h(Eyy << d_shiftcnt)
          MOV        r0, r6
          MOV        r1, r7
          BL         divide_s_cb
          @ r0 ---- s_G
          SUB        r8, r5, r4
          ADDS       r9, r8, #5                  @ G2 = 5 + (d_shiftcnt - n_shiftcnt)
          @CMP        r9, #0
          MOVPL      r10, r0, LSL r9             @ G = s_G << G2
          RSBMI      r11, r9, #0
          MOVMI      r10, r0, ASR r11           @ G = s_G >> (-G2)
          @ r10 --- G , r6 --- s_Exy , r4 --- n_shiftcnt
          @ L_mpy_ls(G, s_Exy)

          @MOV         r2, r10, ASR #16
          @MUL         r3, r2, r6
          @MOV         r2, r10, LSL #16
          @MOV         r2, r2,  LSR #16
          @MUL         r11, r2, r6
          @RSB         r3, r3, #0
          @MOV         r3, r3, LSL #1
          @SUB         r3, r3, r11, ASR #15
          @MOV         r9, r11, ASR r4

          SMULTB     r7, r10, r6                 @ (r10 >> 16) * r6
          SMULBB     r8, r10, r6                 @ (Word16)r10 * r6
          LDR        r2, [r13, #20]
          LDR        r3, [r13, #24] 
          MOV        r9, r7, LSL #1
          ADD        r11, r9, r8,ASR #15 
          RSB        r11, r11, #0
          MOV        r9, r11, ASR r4
 
          STR        r10, [r3]                   @ store G
          STR        r9,  [r2]                   @ store err
          B          vo_comput_cb_end

L_label:
          RSB        r0, r0, #0                  @ -Exy
          CLZ        r4, r0
          SUB        r4, r4, #2                  @ n_shiftcnt
    
          CLZ        r5, r1
          SUB        r5, r5, #1                  @ d_shiftcnt
          
          MOV        r6, r0, LSL r4
          MOV        r6, r6, ASR #16             @ s_Exy
          MOV        r7, r1, LSL r5
          MOV        r7, r7, ASR #16             @ S_Eyy
          MOV        r0, r6
          MOV        r1, r7
          BL         divide_s_cb
          SUB        r8, r5, r4
          ADD        r9, r8, #5
          MOVPL      r10, r0, LSL r9
          RSBMI      r11, r9, #0
          MOVMI      r10, r0, ASR r11         
          RSB        r10, r10, #0                @ r10 --- G
          @ r10 --- G, r6 --- s_Exy, r4 --- n_shiftcnt
          SMULTB     r7, r10, r6                 @ (r10 >> 16) * r6
          SMULBB     r8, r10, r6                 @ (Word16)r10 * r6
          MOV        r9, r7, LSL #1
          ADD        r11, r9, r8, ASR #15 
          LDR        r2, [r13, #20]
          LDR        r3, [r13, #24]  
          MOV        r9, r11, ASR r4
          STR        r10, [r3]                   @ store G
          STR        r9,  [r2]                   @ store err        
          B          vo_comput_cb_end

CONST_Label:

          LDR        r4, =104858
          MOV        r5, #0
          STR        r4, [r3]                    @G = 104858
          STR        r5, [r2]                    @err = 0         
          
vo_comput_cb_end:

          ADD        r13, r13, #40
          LDMFD      r13!, {r4 - r12, r15}        
          .END


