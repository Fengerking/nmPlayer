@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@*************************************************************** 
@void Filt_7k(
@     Word16 signal[],                      /* input:  signal                  */
@     Word16 lg,                            /* input:  length of input         */
@     Word16 mem[]                          /* in/out: memory (size=30)        */
@)
@******************************************************************
@ r0    ---  signal[]
@ r1    ---  lg
@ r2    ---  mem[] 
          #include "voAMRWBDecID.h"
          .text   .align 4
	  .globl   _Filt_7k_asm
	  .globl   _Copy
	  .globl   _fir_7k

_Filt_7k_asm:
          stmfd   		r13!, {r4 - r12, r14} 
          sub    		r13, r13, #240              @ x[L_subFR16k + (L_FIR - 1)]
          mov     		r8, r0                      @ copy signal[] address
          mov     		r4, r1                      @ copy lg address
          mov     		r5, r2                      @ copy mem[] address

          mov     		r1, r13
          mov     		r0, r2
          mov     		r2, #30                     @ L_FIR - 1
          bl      		_Copy                        @ memcpy(x, mem, (L_FIR - 1)<<1)

          ldr     		r10, Lable1                 @ get fir_7k address     

          add     		r6, r13, #60                @ get x[L_FIR - 1] address
          mov     		r0, r8
          mov     		r1, r6
          mov     		r2, #80
          bl      		_Copy                   @ memcpy(&x[30], signal, lg <<1)

          str     		r5, [sp, #-4]               @ PUSH  r5 to stack

          mov                   r3, r8
          @ not use registers: r4, r10, r12, r14, r5
          mov     		r4, r13 
          mov     	        r5, #0                      @ i = 0              
LOOP:
          ldr                   r0, [r10]

          ldrsh  		r1, [r4]                   @ load x[i]
          ldrsh   		r2, [r4, #60]              @ load x[i + 30]
          ldrsh                 r6, [r4, #2]               @ load x[i + 1]
          ldrsh                 r7, [r4, #58]              @ load x[i + 29]
          add                   r1, r1, r2                 @ x[i] + x[i + 30]
          add                   r6, r6, r7                 @ x[i + 1] + x[i + 29]
          ldrsh                 r8, [r4, #4]               @ load x[i + 2]
          ldrsh                 r9, [r4, #56]              @ load x[i + 28]
          smulbb                r14, r1, r0                @ (x[i] + x[i + 30]) * fir_7k[0]
          add                   r8, r8, r9                 @ x[i + 2] + x[i + 28]
          smlabt                r14, r6, r0, r14           @ (x[i + 1] + x[i + 29]) * fir_7k[1]
          ldr                   r0, [r10, #4]
          ldrsh                 r1, [r4, #6]               @ load x[i+3]
          ldrsh                 r2, [r4, #54]              @ load x[i+27]
          ldrsh                 r6, [r4, #8]               @ load x[i+4]
          ldrsh                 r7, [r4, #52]              @ load x[i+26]
          add                   r1, r1, r2                 @ x[i+3] + x[i+27]
          add                   r6, r6, r7                 @ x[i+4] + x[i+26]
	  smlabb                r14, r8, r0, r14           @ (x[i + 2] + x[i + 28]) * fir_7k[2]
	  ldrsh                 r8, [r4, #10]              @ load x[i+5]
	  ldrsh                 r9, [r4, #50]              @ load x[i+25]
	  smlabt                r14, r1, r0, r14           @ (x[i+3] + x[i+27]) * fir_7k[3]
	  add                   r8, r8, r9                 @ x[i+5] + x[i+25] 
          ldr                   r0, [r10, #8]
          ldrsh                 r1, [r4, #12]              @ x[i+6]
          ldrsh                 r2, [r4, #48]              @ x[i+24]
          smlabb                r14, r6, r0, r14           @ (x[i+4] + x[i+26]) * fir_7k[4]
          ldrsh                 r6, [r4, #14]              @ x[i+7] 
          ldrsh                 r7, [r4, #46]              @ x[i+23]
          smlabt                r14, r8, r0, r14           @ (x[i+5] + x[i+25]) * fir_7k[5]
          ldr                   r0, [r10, #12]
          add                   r1, r1, r2                 @ (x[i+6] + x[i+24])
          add                   r6, r6, r7                 @ (x[i+7] + x[i+23])
          smlabb                r14, r1, r0, r14           @ (x[i+6] + x[i+24]) * fir_7k[6]
          ldrsh                 r8, [r4, #16]              @ x[i+8]
          ldrsh                 r9, [r4, #44]              @ x[i+22] 
          smlabt                r14, r6, r0, r14           @ (x[i+7] + x[i+23]) * fir_7k[7]  
          ldr                   r0, [r10, #16]
          ldrsh                 r1, [r4, #18]              @ x[i+9]
          ldrsh                 r2, [r4, #42]              @ x[i+21]
          ldrsh                 r6, [r4, #20]              @ x[i+10]
          ldrsh                 r7, [r4, #40]              @ x[i+20]
          add                   r8, r8, r9                 @ (x[i+8] + x[i+22])
          add                   r1, r1, r2                 @ (x[i+9] + x[i+21])
          add                   r6, r6, r7                 @ (x[i+10] + x[i+20])

          smlabb                r14, r8, r0, r14           @ (x[i+8] + x[i+22]) * fir_7k[8]

          ldrsh                 r8, [r4, #22]              @ x[i+11]
          ldrsh                 r9, [r4, #38]              @ x[i+19]

          smlabt                r14, r1, r0, r14           @ (x[i+9] + x[i+21]) * fir_7k[9]

          ldr                   r0, [r10, #20]
          ldrsh                 r1, [r4, #24]              @ x[i+12]
          ldrsh                 r2, [r4, #36]              @ x[i+18]
          smlabb                r14, r6, r0, r14           @ (x[i+10] + x[i+20]) * fir_7k[10]
	  ldrsh                 r6, [r4, #26]              @ x[i+13]
          add                   r8, r8, r9                 @ (x[i+11] + x[i+19])  
          ldrsh                 r7, [r4, #34]              @ x[i+17]
   
          smlabt                r14, r8, r0, r14           @ (x[i+11] + x[i+19]) * fir_7k[11]

	  ldr                   r0, [r10, #24]
          add                   r1, r1, r2                 @ x[i+12] + x[i+18]
          ldrsh                 r8, [r4, #28]              @ x[i+14]
          smlabb                r14, r1, r0, r14           @ (x[i+12] + x[i+18]) * fir_7k[12]
	  add                   r6, r6, r7                 @ (x[i+13] + x[i+17])
          ldrsh                 r9, [r4, #32]              @ x[i+16] 
          smlabt                r14, r6, r0, r14           @ (x[i+13] + x[i+17]) * fir_7k[13]
	  ldr                   r0, [r10, #28]         
          add                   r8, r8, r9                 @ (x[i+14] + x[i+16])
          ldrsh                 r1, [r4, #30]              @ x[i+15]

          smlabb                r14, r8, r0, r14           @ (x[i+14] + x[i+16]) * fir_7k[14]
          smlabt                r14, r1, r0, r14           @ x[i+15] * fir_7k[15]      
          add     	    	r5, r5, #1
          add     		r14, r14, #0x4000
          add     		r4, r4, #2                
          mov     		r1, r14, ASR #15
          cmp     		r5, #80
          strh    		r1, [r3], #2               @signal[i] = (L_tmp + 0x4000) >> 15
          blt     		LOOP      
           
          ldr		     	r1, [sp, #-4]               @mem address
          add     		r0, r13, #160               @x + lg
          mov     		r2, #30
          bl      		_Copy
                    
Filt_7k_end:

          add     		r13, r13, #240  		     
          ldmfd   		r13!, {r4 - r12, r15} 
 
Lable1:
          .word  		_fir_7k
          @.END

