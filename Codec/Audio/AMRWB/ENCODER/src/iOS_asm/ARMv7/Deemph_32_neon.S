@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@*   VisualOn, Inc. Confidential and Proprietary, 2003-2010 
@*  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@void Deemph_32(
@     Word16 x_hi[],                        /* (i)     : input signal (bit31..16) */
@     Word16 x_lo[],                        /* (i)     : input signal (bit15..4)  */
@     Word16 y[],                           /* (o)     : output signal (x16)      */
@     Word16 mu,                            /* (i) Q15 : deemphasis factor        */
@     Word16 L,                             /* (i)     : vector size              */
@     Word16 * mem                          /* (i/o)   : memory (y[-1])           */
@     )

@x_hi     RN      R0
@x_lo     RN      R1
@y[]      RN      R2
@*mem     RN      R3
          #include "voAMRWBEncID.h"
           .text    .align   4
           .globl   _Deemph_32_asm
	   
_Deemph_32_asm:

           stmfd   	r13!, {r4 - r12, r14} 
	   mov          r4, #2                   @i=0
	   ldrsh        r6, [r0], #2             @load x_hi[0]
	   ldrsh        r7, [r1], #2             @load x_lo[0]
	   ldr          r5, Cvalue7               @r5---mu
	   mov          r11, #0x8000

           @y[0]
	   mov          r10, r6, lsl #16         @L_tmp = x_hi[0]<<16
	   mov          r8,  r5, asr #1          @fac = mu >> 1
	   ldr          r5,  [r3]
	   add          r12, r10, r7, lsl #4     @L_tmp += x_lo[0] << 4
	   mov          r10, r12, lsl #3         @L_tmp <<= 3
	   mul          r9, r5, r8
	   ldrsh        r6, [r0], #2             @load x_hi[1] 
	   qdadd        r10, r10, r9
	   ldrsh        r7, [r1], #2             @load x_lo[1]  
	   mov          r12, r10, lsl #1         @L_tmp = L_mac(L_tmp, *mem, fac)
	   qadd         r10, r12, r11
	   mov          r14, r10, asr #16        @y[0] = round(L_tmp)


	   mov          r10, r6, lsl #16
	   add          r12, r10, r7, lsl #4
           strh         r14, [r2], #2            @update y[0]
	   mov          r10, r12, lsl #3
	   mul          r9, r14, r8
	   qdadd        r10, r10, r9
	   mov          r12, r10, lsl #1
	   qadd         r10, r12, r11
	   mov          r14, r10, asr #16        @y[1] = round(L_tmp)

LOOP:
           ldrsh        r6, [r0], #2             @load x_hi[]
	   ldrsh        r7, [r1], #2
	   strh         r14, [r2], #2
	   mov          r10, r6, lsl #16
	   add          r12, r10, r7, lsl #4
	   mul          r9, r14, r8
	   mov          r10, r12, lsl #3
	   qdadd        r10, r10, r9
           ldrsh        r6, [r0], #2             @load x_hi[]
	   mov          r12, r10, lsl #1
	   qadd         r10, r12, r11
	   ldrsh        r7, [r1], #2
	   mov          r14, r10, asr #16

	   mov          r10, r6, lsl #16
	   add          r12, r10, r7, lsl #4
	   strh         r14, [r2], #2
	   mul          r9, r14, r8
	   mov          r10, r12, lsl #3
	   qdadd        r10, r10, r9
           add          r4, r4, #2
	   mov          r12, r10, lsl #1
	   qadd         r10, r12, r11
           cmp          r4, #64
	   mov          r14, r10, asr #16

           blt          LOOP
           str          r14, [r3]
           strh         r14, [r2]	   

           ldmfd   	r13!, {r4 - r12, r15} 

	   @.END
Cvalue7:
           .word        22282
