@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@*   VisualOn, Inc. Confidential and Proprietary, 2003-2010 
@*  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@**********************************************************************/
@void Syn_filt_32(
@     Word16 a[],                           /* (i) q12 : a[m+1] prediction coefficients */
@     Word16 m,                             /* (i)     : order of LP filter             */
@     Word16 exc[],                         /* (i) qnew: excitation (exc[i] >> qnew)    */
@     Word16 qnew,                          /* (i)     : exc scaling = 0(min) to 8(max) */
@     Word16 sig_hi[],                      /* (o) /16 : synthesis high                 */
@     Word16 sig_lo[],                      /* (o) /16 : synthesis low                  */
@     Word16 lg                             /* (i)     : size of filtering              */
@)
@***********************************************************************
@ a[]      --- r0
@ m        --- r1
@ exc[]    --- r2
@ qnew     --- r3
@ sig_hi[] --- r4
@ sig_lo[] --- r5
@ lg       --- r6
          #include "voAMRWBEncID.h"
          .text   .align 4 
          .globl   _voAMRWBEncSyn_filt_32_asm

_voAMRWBEncSyn_filt_32_asm:

          stmfd   	r13!, {r4 - r12, r14} 
          ldr           r4,  [r13, #40]                  @ get sig_hi[] address
          ldr           r5,  [r13, #44]                  @ get sig_lo[] address

          ldrsh         r6,  [r0], #2                    @ load Aq[0]
          add           r7,  r3, #4                      @ 4 + q_new
          mov           r3, r6, asr r7                   @ a0 = Aq[0] >> (4 + q_new)

	  sub           r10, r4, #32                     @ sig_hi[-16] address
	  sub           r11, r5, #32                     @ sig_lo[-16] address

	  vld1.s16      {d0, d1, d2, d3}, [r0]!          @a[1] ~ a[16] 
  
          mov           r8, #0                           @ i = 0

	  vld1.s16      {d4, d5, d6, d7}, [r10]!         @ sig_hi[-16] ~ sig_hi[-1]
          vrev64.16     d0, d0
          vrev64.16     d1, d1
	  vld1.s16      {d8, d9, d10, d11}, [r11]!       @ sig_lo[-16] ~ sig_lo[-1]
          vrev64.16     d2, d2
          vrev64.16     d3, d3	
          vdup.s32      q15, r8
              
SYN_LOOP:

          ldrsh         r6, [r2], #2                     @exc[i]
	  @L_tmp = L_msu(L_tmp, sig_lo[i - j], a[j])@
	  vmull.s16     q10, d8, d3
	  vext.8        d8, d8, d9, #2
	  vmlal.s16     q10, d9, d2
	  vmlal.s16     q10, d10, d1
	  vmlal.s16     q10, d11, d0

	  vext.8        d9, d9, d10, #2
	  vext.8        d10, d10, d11, #2
	  
	  vpadd.s32     d28, d20, d21
          mul           r12, r6, r3                      @exc[i] * a0
	  vpadd.s32     d29, d28, d28
	  vdup.s32      q10, d29[0]                      @result1
          
	  vmull.s16     q11, d4, d3
	  vmlal.s16     q11, d5, d2
          vsub.s32      q10, q15, q10
	  @L_tmp = L_msu(L_tmp, sig_hi[i - j], a[j])@

	  vmlal.s16     q11, d6, d1
	  vext.8        d4, d4, d5, #2
	  vmlal.s16     q11, d7, d0


	  vext.8        d5, d5, d6, #2
	  vext.8        d6, d6, d7, #2

	  vpadd.s32     d28, d22, d23
          vpadd.s32     d29, d28, d28
          mov           r14, r12, lsl #1                 @exc[i] * a0 << 1
          vdup.s32      q11, d29[0]                      @result2



	  vshr.s32      q10, q10, #11                    @result1 >>= 11
	  vshl.s32      q11, q11, #1                     @result2 <<= 1
	  vdup.s32      q12, r14                         
	  vadd.s32      q12, q12, q10                    @L_tmp = L_tmp - (result1 >>= 11) - (result2 <<= 1)
	  vsub.s32      q12, q12, q11

	  vshl.s32      q12, q12, #3                     @L_tmp <<= 3


	  vshrn.s32     d20, q12, #16                    @sig_hi[i] = L_tmp >> 16@
	  vmov.s16      r10, d20[0]
	  vshr.s32      q12, q12, #4                     @L_tmp >>= 4
	  vext.8        d7, d7, d20, #2
	  strh          r10, [r4], #2                    @store sig_hi[i]
          vmov.s32      r11, d24[0]                      @r11 --- L_tmp >>= 4
	  add           r8, r8, #1
	  sub           r12, r11, r10, lsl #12
	  @mov           r11, r12, asr #16                @sig_lo[i]
	  vdup.s16      d21, r12
	  vext.8        d11, d11, d21, #2
	  strh          r12, [r5], #2                    @stroe sig_lo[i]

          cmp           r8, #64
          blt           SYN_LOOP                          
         
Syn_filt_32_end:
		     
          ldmfd   	    r13!, {r4 - r12, r15} 
          @ENdFUNC
          @.ENd
 

