@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@*************************************************************** 
@void Syn_filt_32(
@     Word16 a[],                           /* (i) Q12 : a[m+1] prediction coefficients */
@     Word16 m,                             /* (i)     : order of LP filter             */
@     Word16 exc[],                         /* (i) Qnew: excitation (exc[i] >> Qnew)    */
@     Word16 Qnew,                          /* (i)     : exc scaling = 0(min) to 8(max) */
@     Word16 sig_hi[],                      /* (o) /16 : synthesis high                 */
@     Word16 sig_lo[],                      /* (o) /16 : synthesis low                  */
@     Word16 lg                             /* (i)     : size of filtering              */
@)
@***************************************************************
@ a[]      --- r0
@ m        --- r1
@ exc[]    --- r2
@ Qnew     --- r3
@ sig_hi[] --- r4
@ sig_lo[] --- r5
@ lg       --- r6
          #include "voAMRWBDecID.h"
          .text   .align 4
	  .globl   _Syn_filt_32_asm

_Syn_filt_32_asm:

          stmfd   	r13!, {r4 - r12, r14} 
          ldr           r4,  [r13, #40]                  @ get sig_hi[] address
          ldr           r5,  [r13, #44]                  @ get sig_lo[] address

          ldrsh         r6,  [r0]                        @ load Aq[0]
          add           r7,  r3, #4                      @ 4 + Q_new
          mov           r3, r6, ASR r7                   @ a0 = Aq[0] >> (4 + Q_new)

          ldr           r14, TempTab
          ldrsh         r6, [r0, #2]                     @ load Aq[1]
          ldrsh         r7, [r0, #4]                     @ load Aq[2]
          ldrsh         r8, [r0, #6]                     @ load Aq[3]
          ldrsh         r9, [r0, #8]                     @ load Aq[4]
          and           r6, r6, r14
          and           r8, r8, r14
          orr           r10, r6, r7, lsl #16             @ Aq[2] -- Aq[1]
          orr           r11, r8, r9, lsl #16             @ Aq[4] -- Aq[3]
          str           r10, [r13, #-4]                  
          str           r11, [r13, #-8]

          ldrsh         r6, [r0, #10]                    @ load Aq[5]
          ldrsh         r7, [r0, #12]                    @ load Aq[6]
          ldrsh         r8, [r0, #14]                    @ load Aq[7]
          ldrsh         r9, [r0, #16]                    @ load Aq[8]
          and           r6, r6, r14
          and           r8, r8, r14
          orr           r10, r6, r7, lsl #16             @ Aq[6] -- Aq[5]
          orr           r11, r8, r9, lsl #16             @ Aq[8] -- Aq[7]
          str           r10, [r13, #-12]
          str           r11, [r13, #-16]

          ldrsh         r6, [r0, #18]                    @ load Aq[9]
          ldrsh         r7, [r0, #20]                    @ load Aq[10]
          ldrsh         r8, [r0, #22]                    @ load Aq[11]
          ldrsh         r9, [r0, #24]                    @ load Aq[12]
          and           r6, r6, r14 
          and           r8, r8, r14
          orr           r10, r6, r7, lsl #16             @ Aq[10] -- Aq[9]
          orr           r11, r8, r9, lsl #16             @ Aq[12] -- Aq[11]
          str           r10, [r13, #-20]
          str           r11, [r13, #-24]  

          ldrsh         r6, [r0, #26]                    @ load Aq[13]
          ldrsh         r7, [r0, #28]                    @ load Aq[14]
          ldrsh         r8, [r0, #30]                    @ load Aq[15]
          ldrsh         r9, [r0, #32]                    @ load Aq[16]
          and           r6, r6, r14
          and           r8, r8, r14
          orr           r10, r6, r7, lsl #16             @ Aq[14] -- Aq[13]
          orr           r11, r8, r9, lsl #16             @ Aq[16] -- Aq[15]
          str           r10, [r13, #-28]
          str           r11, [r13, #-32]
          
          mov           r8, #0                           @ i = 0
              
LOOP:            
          ldrsh         r6, [r5, #-2]                    @ load sig_lo[i-1]
          ldrsh         r7, [r5, #-4]                    @ load sig_lo[i-2]

          ldr           r11, [r13, #-4]                  @ Aq[2] -- Aq[1]        
          ldrsh         r9, [r5, #-6]                    @ load sig_lo[i-3]
          ldrsh         r10, [r5, #-8]                   @ load sig_lo[i-4]

          smulbb        r12, r6, r11                     @ sig_lo[i-1] * Aq[1]

          ldrsh         r6, [r5, #-10]                   @ load sig_lo[i-5]
          smlabt        r12, r7, r11, r12                @ sig_lo[i-2] * Aq[2]
          ldr           r11, [r13, #-8]                  @ Aq[4] -- Aq[3]
          ldrsh         r7, [r5, #-12]                   @ load sig_lo[i-6]
          smlabb        r12, r9, r11, r12                @ sig_lo[i-3] * Aq[3]
          ldrsh         r9, [r5, #-14]                   @ load sig_lo[i-7]
          smlabt        r12, r10, r11, r12               @ sig_lo[i-4] * Aq[4]
          ldr           r11, [r13, #-12]                 @ Aq[6] -- Aq[5]
          ldrsh         r10, [r5, #-16]                  @ load sig_lo[i-8]
          smlabb        r12, r6, r11, r12                @ sig_lo[i-5] * Aq[5]
          ldrsh         r6,  [r5, #-18]                  @ load sig_lo[i-9]
          smlabt        r12, r7, r11, r12                @ sig_lo[i-6] * Aq[6]
          ldr           r11, [r13, #-16]                 @ Aq[8] -- Aq[7]
          ldrsh         r7,  [r5, #-20]                  @ load sig_lo[i-10]
          smlabb        r12, r9, r11, r12                @ sig_lo[i-7] * Aq[7]
          ldrsh         r9, [r5, #-22]                   @ load sig_lo[i-11]
          smlabt        r12, r10, r11, r12               @ sig_lo[i-8] * Aq[8]
          ldr           r11, [r13, #-20]                 @ Aq[10] -- Aq[9]
          ldrsh         r10,[r5, #-24]                   @ load sig_lo[i-12]
          smlabb        r12, r6, r11, r12                @ sig_lo[i-9] * Aq[9]
          ldrsh         r6, [r5, #-26]                   @ load sig_lo[i-13]
          smlabt        r12, r7, r11, r12                @ sig_lo[i-10] * Aq[10]
          ldr           r11, [r13, #-24]                 @ Aq[12] -- Aq[11]
          ldrsh         r7, [r5, #-28]                   @ load sig_lo[i-14]
          smlabb        r12, r9, r11, r12                @ sig_lo[i-11] * Aq[11]
          ldrsh         r9, [r5, #-30]                   @ load sig_lo[i-15]
          smlabt        r12, r10, r11, r12               @ sig_lo[i-12] * Aq[12]

          ldr           r11, [r13, #-28]                 @ Aq[14] -- Aq[13]
          ldrsh         r10, [r5, #-32]                  @ load sig_lo[i-16]
          smlabb        r12, r6, r11, r12                @ sig_lo[i-13] * Aq[13]
          smlabt        r12, r7, r11, r12                @ sig_lo[i-14] * Aq[14]
 
          ldr           r11, [r13, #-32]                 @ Aq[16] -- Aq[15]
          ldrsh         r6, [r2],#2                      @ load exc[i] 
          smlabb        r12, r9, r11, r12                @ sig_lo[i-15] * Aq[15]
          smlabt        r12, r10, r11, r12               @ sig_lo[i-16] * Aq[16]
          mul           r7, r6, r3                       @ exc[i] * a0 
          rsb           r14, r12, #0                     @ L_tmp
          mov           r14, r14, ASR #11                @ L_tmp >>= 11
          add           r14, r14, r7, lsl #1             @ L_tmp += (exc[i] * a0) << 1


          ldrsh         r6, [r4, #-2]                    @ load sig_hi[i-1]
          ldrsh         r7, [r4, #-4]                    @ load sig_hi[i-2]

          ldr           r11, [r13, #-4]                  @ Aq[2] -- Aq[1]        
          ldrsh         r9, [r4, #-6]                    @ load sig_hi[i-3]
          ldrsh         r10, [r4, #-8]                   @ load sig_hi[i-4]
          smulbb        r12, r6, r11                     @ sig_hi[i-1] * Aq[1]
	  ldrsh         r6, [r4, #-10]                   @ load sig_hi[i-5]
	  smlabt        r12, r7, r11, r12                @ sig_hi[i-2] * Aq[2]

	  ldr           r11, [r13, #-8]                  @ Aq[4] -- Aq[3]
	  ldrsh         r7, [r4, #-12]                   @ load sig_hi[i-6]

	  smlabb        r12, r9, r11, r12                @ sig_hi[i-3] * Aq[3]
	  ldrsh         r9, [r4, #-14]                   @ load sig_hi[i-7]

	  smlabt        r12, r10, r11, r12               @ sig_hi[i-4] * Aq[4]

	  ldr           r11, [r13, #-12]                 @ Aq[6] -- Aq[5]
	  ldrsh         r10, [r4, #-16]                  @ load sig_hi[i-8]

	  smlabb        r12, r6, r11, r12                @ sig_hi[i-5] * Aq[5]

	  ldrsh         r6,  [r4, #-18]                  @ load sig_hi[i-9]
	  smlabt        r12, r7, r11, r12                @ sig_hi[i-6] * Aq[6]

	  ldr           r11, [r13, #-16]                 @ Aq[8] -- Aq[7]
	  ldrsh         r7,  [r4, #-20]                  @ load sig_hi[i-10]

	  smlabb        r12, r9, r11, r12                @ sig_hi[i-7] * Aq[7]

	  ldrsh         r9, [r4, #-22]                   @ load sig_hi[i-11]

	  smlabt        r12, r10, r11, r12               @ sig_hi[i-8] * Aq[8]

	  ldr           r11, [r13, #-20]                 @ Aq[10] -- Aq[9]
	  ldrsh         r10,[r4, #-24]                   @ load sig_hi[i-12]

	  smlabb        r12, r6, r11, r12                @ sig_hi[i-9] * Aq[9]
	  ldrsh         r6, [r4, #-26]                   @ load sig_hi[i-13]
	  smlabt        r12, r7, r11, r12                @ sig_hi[i-10] * Aq[10]

	  ldr           r11, [r13, #-24]                 @ Aq[12] -- Aq[11]
	  ldrsh         r7, [r4, #-28]                   @ load sig_hi[i-14]
          smlabb        r12, r9, r11, r12                @ sig_hi[i-11] * Aq[11]
          ldrsh         r9, [r4, #-30]                   @ load sig_hi[i-15]
          smlabt        r12, r10, r11, r12               @ sig_hi[i-12] * Aq[12]

          ldr           r11, [r13, #-28]                 @ Aq[14] -- Aq[13]
          ldrsh         r10, [r4, #-32]                  @ load sig_hi[i-16]
          smlabb        r12, r6, r11, r12                @ sig_hi[i-13] * Aq[13]
          smlabt        r12, r7, r11, r12                @ sig_hi[i-14] * Aq[14]
 
          ldr           r11, [r13, #-32]                 @ Aq[16] -- Aq[15]
          smlabb        r12, r9, r11, r12                @ sig_hi[i-15] * Aq[15]
          smlabt        r12, r10, r11, r12               @ sig_hi[i-16] * Aq[16]       
          add           r6, r12, r12                     @ r12 << 1
          sub           r14, r14, r6                     
          mov           r14, r14, lsl #3                 @ L_tmp <<=3
  
          mov           r7, r14, ASR #16                 @ L_tmp >> 16

          mov           r14, r14, ASR #4                 @ L_tmp >>=4
          strh          r7, [r4], #2                         @ sig_hi[i] = L_tmp >> 16
          sub           r9, r14, r7, lsl #12             @ sig_lo[i] = L_tmp - (sig_hi[i] << 12)

          add           r8, r8, #1
          strh          r9, [r5], #2   
          cmp           r8, #64
          blt           LOOP                            
         
Syn_filt_32_end:
		     
          ldmfd   	r13!, {r4 - r12, r15}

TempTab:
          .word         0xffff 
          @.END
 

