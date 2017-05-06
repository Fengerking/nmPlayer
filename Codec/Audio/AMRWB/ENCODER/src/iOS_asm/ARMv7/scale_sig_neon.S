@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@*   VisualOn, Inc. Confidential and Proprietary, 2003-2010 
@*  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@                        
@**********************************************************************/
@void Scale_sig(
@               Word16 x[],                           /* (i/o) : signal to scale               */
@               Word16 lg,                            /* (i)   : size of x[]                   */
@               Word16 exp                            /* (i)   : exponent: x = round(x << exp) */
@)
@***********************************************************************
@  x[]   ---  r0
@  lg    ---  r1
@  exp   ---  r2
          #include "voAMRWBEncID.h"
          .text   .align 4 
          .globl   _Scale_sig_opt
          .globl   _Scale_sig_opt_v6

_Scale_sig_opt:

          stmfd   	r13!, {r4 - r12, r14} 
          mov           r4, #4
          vmov.s32      q15, #0x8000       
          vdup.s32      q14, r2  
          mov           r5, r0                          @ copy x[] address    
          cmp           r1, #64
          moveq         r4, #1
          beq           LOOP
	  cmp           r1, #128
	  moveq         r4, #2
	  beq           LOOP
          cmp           r1, #256
          beq           LOOP
	  cmp           r1, #80
	  moveq         r4, #1
	  beq           LOOP1

LOOP1:
          vld1.s16      {q0, q1}, [r5]!                 @load 16 Word16 x[]     
          vshll.s16     q10, d0, #16
          vshll.s16     q11, d1, #16
          vshll.s16     q12, d2, #16
          vshll.s16     q13, d3, #16
          vshl.s32      q10, q10, q14
          vshl.s32      q11, q11, q14
          vshl.s32      q12, q12, q14
          vshl.s32      q13, q13, q14
          vaddhn.s32    d16, q10, q15
          vaddhn.s32    d17, q11, q15
          vaddhn.s32    d18, q12, q15
          vaddhn.s32    d19, q13, q15
          vst1.s16      {q8, q9}, [r0]!                 @store 16 Word16 x[]

LOOP:                
          vld1.s16      {q0, q1}, [r5]!                 @load 16 Word16 x[]
          vld1.s16      {q2, q3}, [r5]!                 @load 16 Word16 x[]
          vld1.s16      {q4, q5}, [r5]!                 @load 16 Word16 x[]
          vld1.s16      {q6, q7}, [r5]!                 @load 16 Word16 x[]

          vshll.s16     q8, d0, #16
          vshll.s16     q9, d1, #16
          vshll.s16     q10, d2, #16
          vshll.s16     q11, d3, #16     
          vshl.s32      q8, q8, q14
          vshl.s32      q9, q9, q14
          vshl.s32      q10, q10, q14
          vshl.s32      q11, q11, q14
          vaddhn.s32    d16, q8, q15
          vaddhn.s32    d17, q9, q15
          vaddhn.s32    d18, q10, q15
          vaddhn.s32    d19, q11, q15
          vst1.s16      {q8, q9}, [r0]!                 @store 16 Word16 x[]

   
          vshll.s16     q12, d4, #16
          vshll.s16     q13, d5, #16
          vshll.s16     q10, d6, #16
          vshll.s16     q11, d7, #16
          vshl.s32      q12, q12, q14
          vshl.s32      q13, q13, q14
          vshl.s32      q10, q10, q14
          vshl.s32      q11, q11, q14
          vaddhn.s32    d16, q12, q15
          vaddhn.s32    d17, q13, q15
          vaddhn.s32    d18, q10, q15
          vaddhn.s32    d19, q11, q15
          vst1.s16      {q8, q9}, [r0]!                 @store 16 Word16 x[]

          vshll.s16     q10, d8, #16
          vshll.s16     q11, d9, #16
          vshll.s16     q12, d10, #16
          vshll.s16     q13, d11, #16
          vshl.s32      q10, q10, q14
          vshl.s32      q11, q11, q14
          vshl.s32      q12, q12, q14
          vshl.s32      q13, q13, q14
          vaddhn.s32    d16, q10, q15
          vaddhn.s32    d17, q11, q15
          vaddhn.s32    d18, q12, q15
          vaddhn.s32    d19, q13, q15
          vst1.s16      {q8, q9}, [r0]!                 @store 16 Word16 x[]

          vshll.s16     q10, d12, #16   
          vshll.s16     q11, d13, #16
          vshll.s16     q12, d14, #16
          vshll.s16     q13, d15, #16
          vshl.s32      q10, q10, q14
          vshl.s32      q11, q11, q14
          vshl.s32      q12, q12, q14
          vshl.s32      q13, q13, q14
          vaddhn.s32    d16, q10, q15
          vaddhn.s32    d17, q11, q15
          vaddhn.s32    d18, q12, q15
          vaddhn.s32    d19, q13, q15 
          vst1.s16      {q8, q9}, [r0]!                 @store 16 Word16 x[]  
          subs          r4, r4, #1
          bgt           LOOP     
                
                          
Scale_sig_asm_end:

          ldmfd   	r13!, {r4 - r12, r15} 
          @ENdFUNC
          @.ENd

_Scale_sig_opt_v6:

         stmfd         r13!, {r4 - r12, r14}
     
	 sub           r3, r1, #1                  @i = lg - 1
         cmp           r2, #0                      @Compare exp and 0
	 rsb           r7, r2, #0                  @exp = -exp
	 add           r10, r2, #16                @16 + exp
         add           r4, r0, r3, lsl #1          @x[i] address
	 mov           r8, #0x7fffffff
	 mov           r9, #0x8000
	 ble           LOOP2_V6
	 
LOOP1_V6:
         ldrsh          r5, [r4]                    @load x[i]
         mov           r12, r5, lsl r10
	 teq           r5, r12, asr r10
	 eorne         r12, r8, r5, asr #31
	 subs          r3, r3, #1
	 qadd          r11, r12, r9
	 mov           r12, r11, asr #16
	 strh          r12, [r4], #-2
	 bge           LOOP1_V6
         bl            The_end

LOOP2_V6:
         ldrsh          r5, [r4]                   @load x[i]
	 mov           r6, r5, lsl #16            @L_tmp = x[i] << 16
	 mov           r5, r6, asr r7             @L_tmp >>= exp
	 qadd          r11, r5, r9
	 mov           r12, r11, asr #16
	 subs          r3, r3, #1
	 strh          r12, [r4], #-2
	 bge           LOOP2_V6

The_end:

         ldmfd         r13!, {r4 - r12, r15}
 

