@/*
@*  Copyright 2003 ~ 2009 by Visualon software Incorporated.
@*  All rights reserved. Property of Visualon software Incorporated.
@*  Restricted rights to use, duplicate or disclose this code are
@*  granted through contract.
@*  
@*/

@/***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver         Description             Author
@*    -----------     --------     ------------------        -----------
@*    07-10-2008        1.0        File imported from        Huaping Liu
@*                                             
@**********************************************************************/
          #include "voAMRWBEncID.h"
          .section .text
	  .global  voAMRWBEncSyn_filt_asm
          .extern  voAMRWBEnc_Copy

voAMRWBEncSyn_filt_asm:

          STMFD   	r13!, {r4 - r12, r14} 
          SUB           r13, r13, #700                   @ y_buf[L_FRAME16k + M16k]
   
          MOV           r4, r3                           @ copy mem[] address
          MOV           r5, r13                          @ copy yy = y_buf address

          @ for(i = 0@ i < m@ i++)
          @{
          @    *yy++ = mem[i]@
          @} 

          LDRH          r6,  [r4], #2          
          LDRH          r7,  [r4], #2
          LDRH          r8,  [r4], #2
          LDRH          r9,  [r4], #2
          LDRH          r10, [r4], #2
          LDRH          r11, [r4], #2
          LDRH          r12, [r4], #2
          LDRH          r14, [r4], #2

          STRH          r6,  [r5], #2
          STRH          r7,  [r5], #2
          STRH          r8,  [r5], #2
          STRH          r9,  [r5], #2
          STRH          r10, [r5], #2
          STRH          r11, [r5], #2
          STRH          r12, [r5], #2
          STRH          r14, [r5], #2

          LDRH          r6,  [r4], #2          
          LDRH          r7,  [r4], #2
          LDRH          r8,  [r4], #2
          LDRH          r9,  [r4], #2
          LDRH          r10, [r4], #2
          LDRH          r11, [r4], #2
          LDRH          r12, [r4], #2
          LDRH          r14, [r4], #2

          STRH          r6,  [r5], #2
          STRH          r7,  [r5], #2
          STRH          r8,  [r5], #2
          STRH          r9,  [r5], #2
          STRH          r10, [r5], #2
          STRH          r11, [r5], #2
          STRH          r12, [r5], #2
          STRH          r14, [r5], #2

          LDRSH         r5, [r0]                         @ load a[0]
          MOV           r8, #0                           @ i = 0
          MOV           r5, r5, ASR #1                   @ a0 = a[0] >> 1
          @MOV           r4, r13
          @ load all a[]

          LDRH          r6, [r0, #2]                     @ load a[1]
          LDRH          r7, [r0, #4]                     @ load a[2]
          LDRH          r9, [r0, #6]                     @ load a[3]
          LDRH          r11,[r0, #8]                     @ load a[4]
          ORR           r10, r6, r7, LSL #16             @ -a[2] -- -a[1]
          ORR           r12, r9, r11, LSL #16            @ -a[4] -- -a[3]
          STR           r10, [r13, #-4]
          STR           r12, [r13, #-8]
          
          LDRH          r6, [r0, #10]                    @ load a[5]
          LDRH          r7, [r0, #12]                    @ load a[6]
          LDRH          r9, [r0, #14]                    @ load a[7]
          LDRH          r11,[r0, #16]                    @ load a[8]
          ORR           r10, r6, r7, LSL #16             @ -a[6] -- -a[5]
          ORR           r12, r9, r11, LSL #16            @ -a[8] -- -a[7]
          STR           r10, [r13, #-12]
          STR           r12, [r13, #-16]          
           
          LDRH          r6, [r0, #18]                    @ load a[9]
          LDRH          r7, [r0, #20]                    @ load a[10]
          LDRH          r9, [r0, #22]                    @ load a[11]
          LDRH          r11,[r0, #24]                    @ load a[12]
          ORR           r10, r6, r7, LSL #16             @ -a[10] -- -a[9]
          ORR           r12, r9, r11, LSL #16            @ -a[12] -- -a[11]
          STR           r10, [r13, #-20]
          STR           r12, [r13, #-24]    

          LDRH          r6, [r0, #26]                    @ load a[13]
          LDRH          r7, [r0, #28]                    @ load a[14]
          LDRH          r9, [r0, #30]                    @ load a[15]
          LDRH          r11,[r0, #32]                    @ load a[16]
          ORR           r10, r6, r7, LSL #16             @ -a[14] -- -a[13]
          ORR           r12, r9, r11, LSL #16            @ -a[16] -- -a[15]
          STR           r10, [r13, #-28]
          STR           r12, [r13, #-32]                
                     
          ADD           r4, r13, #32

LOOP:
          ADD           r10, r4, r8, LSL #1              @ temp_p = yy + i          
          LDRSH         r6,  [r1], #2                    @ load x[i]
          LDRSH         r9,  [r10, #-2]                  @ *(temp_p - 1)
          LDRSH         r12, [r10, #-4]                  @ *(temp_p - 2)
	  MOV           r14, #0
          LDR           r7,  [r13, #-4]                  @ -a[2]  -a[1]
	  PKHBT         r9, r9, r12, LSL #16             @ *(temp_p - 2), *(temp_p - 1)
          MUL           r0, r5, r6                       @ L_tmp = x[i] * a0
	  LDRSH         r6, [r10, #-6]                   @ *(temp_p - 3)
	  LDRSH         r11, [r10, #-8]                  @ *(temp_p - 4)
          SMLAD         r14, r9, r7, r14

          LDR           r7,  [r13, #-8]                  @ -a[4] -a[3]
          PKHBT         r6, r6, r11, LSL #16             @ *(temp_p - 4), *(temp_p - 3)
          LDRSH         r9,  [r10, #-10]                 @ *(temp_p - 5)
          LDRSH         r12, [r10, #-12]                 @ *(temp_p - 6)
	  SMLAD         r14, r6, r7, r14


          LDR           r7,  [r13, #-12]                 @ -a[6]  -a[5]
          PKHBT         r9, r9, r12, LSL #16             @ *(temp_p - 6), *(temp_p - 5)
          LDRSH         r6,  [r10, #-14]                 @ *(temp_p - 7)
          LDRSH         r11, [r10, #-16]                 @ *(temp_p - 8)
	  SMLAD         r14, r9, r7, r14

          LDR           r7,  [r13, #-16]                 @ -a[8] -a[7]
	  PKHBT         r6, r6, r11, LSL #16             @ *(temp_p - 8), *(temp_p - 7)
          LDRSH         r9,  [r10, #-18]                 @ *(temp_p - 9)   
          LDRSH         r12, [r10, #-20]                 @ *(temp_p - 10)
	  SMLAD         r14, r6, r7, r14
     	  
          LDR           r7,  [r13, #-20]                 @ -a[10]  -a[9]
	  PKHBT         r9, r9, r12, LSL #16             @ *(temp_p - 10), *(temp_p - 9)
          LDRSH         r6,  [r10, #-22]                 @ *(temp_p - 11)
          LDRSH         r11, [r10, #-24]                 @ *(temp_p - 12)
	  SMLAD         r14, r9, r7, r14

          LDR           r7,  [r13, #-24]                 @ -a[12] -a[11]
	  PKHBT         r6, r6, r11, LSL #16             @ *(temp_p - 12), *(temp_p - 11)
          LDRSH         r9,  [r10, #-26]                 @ *(temp_p - 13)       
          LDRSH         r12, [r10, #-28]                 @ *(temp_p - 14)
	  SMLAD         r14, r6, r7, r14

          LDR           r7,  [r13, #-28]                 @ -a[14] -a[13]
	  PKHBT         r9, r9, r12, LSL #16             @ *(temp_p - 14), *(temp_p - 13)
          LDRSH         r6,  [r10, #-30]                 @ *(temp_p - 15)
          LDRSH         r11, [r10, #-32]                 @ *(temp_p - 16)
	  SMLAD         r14, r9, r7, r14

          LDR           r7,  [r13, #-32]                 @ -a[16] -a[15]
	  PKHBT         r6, r6, r11, LSL #16
	  SMLAD         r14, r6, r7, r14

          RSB           r14, r14, r0
                                  
          MOV           r7, r14, LSL #4                  @ L_tmp <<=4
          ADD           r8, r8, #1
          ADD           r14, r7, #0x8000                 
          MOV           r7, r14, ASR #16                 @ (L_tmp + 0x8000) >> 16
          CMP           r8, #80
          STRH          r7, [r10]                        @ yy[i]
          STRH          r7, [r2], #2                     @ y[i]
          BLT           LOOP
 
          @ update mem[]
          ADD           r5, r13, #160                    @ yy[64] address
          MOV           r1, r3
          MOV           r0, r5
          MOV           r2, #16
          BL            voAMRWBEnc_Copy          

Syn_filt_asm_end:
 
          ADD           r13, r13, #700		     
          LDMFD   	r13!, {r4 - r12, r15} 

          .END
 

