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
          .text   .align  4
	  .globl  _voAMRWBEncSyn_filt_asm
          .globl  _voAMRWBEnc_Copy

_voAMRWBEncSyn_filt_asm:

          stmfd   	r13!, {r4 - r12, r14} 
          sub           r13, r13, #700                   @ y_buf[L_FRAME16k + M16k]
   
          mov           r4, r3                           @ copy mem[] address
          mov           r5, r13                          @ copy yy = y_buf address

          @ for(i = 0@ i < m@ i++)
          @{
          @    *yy++ = mem[i]@
          @} 

          ldrh          r6,  [r4], #2          
          ldrh          r7,  [r4], #2
          ldrh          r8,  [r4], #2
          ldrh          r9,  [r4], #2
          ldrh          r10, [r4], #2
          ldrh          r11, [r4], #2
          ldrh          r12, [r4], #2
          ldrh          r14, [r4], #2

          strh          r6,  [r5], #2
          strh          r7,  [r5], #2
          strh          r8,  [r5], #2
          strh          r9,  [r5], #2
          strh          r10, [r5], #2
          strh          r11, [r5], #2
          strh          r12, [r5], #2
          strh          r14, [r5], #2

          ldrh          r6,  [r4], #2          
          ldrh          r7,  [r4], #2
          ldrh          r8,  [r4], #2
          ldrh          r9,  [r4], #2
          ldrh          r10, [r4], #2
          ldrh          r11, [r4], #2
          ldrh          r12, [r4], #2
          ldrh          r14, [r4], #2

          strh          r6,  [r5], #2
          strh          r7,  [r5], #2
          strh          r8,  [r5], #2
          strh          r9,  [r5], #2
          strh          r10, [r5], #2
          strh          r11, [r5], #2
          strh          r12, [r5], #2
          strh          r14, [r5], #2

          ldrsh         r5, [r0]                         @ load a[0]
          mov           r8, #0                           @ i = 0
          mov           r5, r5, asr #1                   @ a0 = a[0] >> 1
          @mov           r4, r13
          @ load all a[]

          ldrh          r6, [r0, #2]                     @ load a[1]
          ldrh          r7, [r0, #4]                     @ load a[2]
          ldrh          r9, [r0, #6]                     @ load a[3]
          ldrh          r11,[r0, #8]                     @ load a[4]
          orr           r10, r6, r7, lsl #16             @ -a[2] -- -a[1]
          orr           r12, r9, r11, lsl #16            @ -a[4] -- -a[3]
          str           r10, [r13, #-4]
          str           r12, [r13, #-8]
          
          ldrh          r6, [r0, #10]                    @ load a[5]
          ldrh          r7, [r0, #12]                    @ load a[6]
          ldrh          r9, [r0, #14]                    @ load a[7]
          ldrh          r11,[r0, #16]                    @ load a[8]
          orr           r10, r6, r7, lsl #16             @ -a[6] -- -a[5]
          orr           r12, r9, r11, lsl #16            @ -a[8] -- -a[7]
          str           r10, [r13, #-12]
          str           r12, [r13, #-16]          
           
          ldrh          r6, [r0, #18]                    @ load a[9]
          ldrh          r7, [r0, #20]                    @ load a[10]
          ldrh          r9, [r0, #22]                    @ load a[11]
          ldrh          r11,[r0, #24]                    @ load a[12]
          orr           r10, r6, r7, lsl #16             @ -a[10] -- -a[9]
          orr           r12, r9, r11, lsl #16            @ -a[12] -- -a[11]
          str           r10, [r13, #-20]
          str           r12, [r13, #-24]    

          ldrh          r6, [r0, #26]                    @ load a[13]
          ldrh          r7, [r0, #28]                    @ load a[14]
          ldrh          r9, [r0, #30]                    @ load a[15]
          ldrh          r11,[r0, #32]                    @ load a[16]
          orr           r10, r6, r7, lsl #16             @ -a[14] -- -a[13]
          orr           r12, r9, r11, lsl #16            @ -a[16] -- -a[15]
          str           r10, [r13, #-28]
          str           r12, [r13, #-32]                
                     
          add           r4, r13, #32

LOOP:
          add           r10, r4, r8, lsl #1              @ temp_p = yy + i          
          ldrsh         r6,  [r1], #2                    @ load x[i]
          ldrsh         r9,  [r10, #-2]                  @ *(temp_p - 1)
          ldrsh         r12, [r10, #-4]                  @ *(temp_p - 2)
	  mov           r14, #0
          ldr           r7,  [r13, #-4]                  @ -a[2]  -a[1]
	  pkhbt         r9, r9, r12, lsl #16             @ *(temp_p - 2), *(temp_p - 1)
          mul           r0, r5, r6                       @ L_tmp = x[i] * a0
	  ldrsh         r6, [r10, #-6]                   @ *(temp_p - 3)
	  ldrsh         r11, [r10, #-8]                  @ *(temp_p - 4)
          smlad         r14, r9, r7, r14

          ldr           r7,  [r13, #-8]                  @ -a[4] -a[3]
          pkhbt         r6, r6, r11, lsl #16             @ *(temp_p - 4), *(temp_p - 3)
          ldrsh         r9,  [r10, #-10]                 @ *(temp_p - 5)
          ldrsh         r12, [r10, #-12]                 @ *(temp_p - 6)
	  smlad         r14, r6, r7, r14


          ldr           r7,  [r13, #-12]                 @ -a[6]  -a[5]
          pkhbt         r9, r9, r12, lsl #16             @ *(temp_p - 6), *(temp_p - 5)
          ldrsh         r6,  [r10, #-14]                 @ *(temp_p - 7)
          ldrsh         r11, [r10, #-16]                 @ *(temp_p - 8)
	  smlad         r14, r9, r7, r14

          ldr           r7,  [r13, #-16]                 @ -a[8] -a[7]
	  pkhbt         r6, r6, r11, lsl #16             @ *(temp_p - 8), *(temp_p - 7)
          ldrsh         r9,  [r10, #-18]                 @ *(temp_p - 9)   
          ldrsh         r12, [r10, #-20]                 @ *(temp_p - 10)
	  smlad         r14, r6, r7, r14
     	  
          ldr           r7,  [r13, #-20]                 @ -a[10]  -a[9]
	  pkhbt         r9, r9, r12, lsl #16             @ *(temp_p - 10), *(temp_p - 9)
          ldrsh         r6,  [r10, #-22]                 @ *(temp_p - 11)
          ldrsh         r11, [r10, #-24]                 @ *(temp_p - 12)
	  smlad         r14, r9, r7, r14

          ldr           r7,  [r13, #-24]                 @ -a[12] -a[11]
	  pkhbt         r6, r6, r11, lsl #16             @ *(temp_p - 12), *(temp_p - 11)
          ldrsh         r9,  [r10, #-26]                 @ *(temp_p - 13)       
          ldrsh         r12, [r10, #-28]                 @ *(temp_p - 14)
	  smlad         r14, r6, r7, r14

          ldr           r7,  [r13, #-28]                 @ -a[14] -a[13]
	  pkhbt         r9, r9, r12, lsl #16             @ *(temp_p - 14), *(temp_p - 13)
          ldrsh         r6,  [r10, #-30]                 @ *(temp_p - 15)
          ldrsh         r11, [r10, #-32]                 @ *(temp_p - 16)
	  smlad         r14, r9, r7, r14

          ldr           r7,  [r13, #-32]                 @ -a[16] -a[15]
	  pkhbt         r6, r6, r11, lsl #16
	  smlad         r14, r6, r7, r14

          rsb           r14, r14, r0
                                  
          mov           r7, r14, lsl #4                  @ L_tmp <<=4
          add           r8, r8, #1
          add           r14, r7, #0x8000                 
          mov           r7, r14, asr #16                 @ (L_tmp + 0x8000) >> 16
          cmp           r8, #80
          strh          r7, [r10]                        @ yy[i]
          strh          r7, [r2], #2                     @ y[i]
          blt           LOOP
 
          @ update mem[]
          add           r5, r13, #160                    @ yy[64] address
          mov           r1, r3
          mov           r0, r5
          mov           r2, #16
          bl            _voAMRWBEnc_Copy          

Syn_filt_asm_end:
 
          add           r13, r13, #700		     
          ldmfd   	r13!, {r4 - r12, r15} 

          @.END
 

