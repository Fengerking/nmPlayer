  
        #include "voAMRNBEncID.h"
        .text   .align   4
        .globl   _Syn_filt_0
	.globl   _myMemCopy

_Syn_filt_0:

        stmfd   r13!, {r4 - r11, r14} 
        sub     r13, r13, #116                   
        mov     r12, #40
@myMemCopy
        add     r4, r13, #8                           
        stmfd   r13!, {r0, r1, r2, r12}
        mov     r2, #20
        mov     r0, r4
        mov     r1, r3
        bl      _myMemCopy
        ldmfd   r13!, {r0, r1, r2, r12}

    	ldrsh   r3, [r0], #2                         @r3 --- X, a0
        ldrsh   r5, [r0], #2                         @a1
	ldrsh   r6, [r0], #2                         @a2
	ldrsh   r7, [r0], #2                         @a3
        str     r3, [r13, #4]
	ldrsh   r8, [r0], #2                         @a4
	pkhbt   r4, r5, r6, lsl #16                  @r4 --- a2, a1
	ldrsh   r6, [r0], #2                         @a5
	pkhbt   r5, r7, r8, lsl #16                  @r5 --- a4, a3

	ldrsh   r7, [r0], #2                         @a6
	ldrsh   r8, [r0], #2                         @a7
        ldrsh   r9, [r0], #2                         @a8
	pkhbt   r6, r6, r7, lsl #16                  @r6 --- a6, a5
        ldrsh   r10, [r0], #2                         @a9
	ldrsh   r11, [r0], #2                         @a10
	pkhbt   r7, r8, r9, lsl #16                  @r7 --- a8, a7
        add     r9, r13, #28                         @yy = tmp +10
	pkhbt   r8, r10, r11, lsl #16                  

L10_4:

        ldrsh   r10, [r1], #2                     @ get x[i]
        ldrsh   r11, [r9, #-2]                    @ yy[-1]
	@ r0, r3, r12, r10
	ldrsh   r0,  [r9, #-4]                    @ yy[-2]
	mul     r14, r3, r10                      @ s = x[i] * (*(tmpA++))@
	ldrsh   r3,  [r9, #-6]                    @ yy[-3]
	pkhbt   r0, r11, r0, lsl #16              @ r0 --- yy[-2], yy[-1]
	ldrsh   r11, [r9, #-8]                    @ yy[-4]
	rsb     r14, r14, #0
	smlad   r14, r0, r4, r14                  @ a[1] * yy[-1] + a[2] * yy[-2]
	ldrsh   r0, [r9, #-10]                    @ yy[-5]
	pkhbt   r3, r3, r11, lsl #16              @ r3 --- yy[-4], yy[-3]
	ldrsh   r10, [r9, #-14]                   @ yy[-7]
	smlad   r14, r3, r5, r14                  @ a[3] * yy[-3] + a[4] * yy[-4]

	ldrsh   r3, [r9, #-12]                    @ yy[-6]
	ldrsh   r11, [r9, #-16]                   @ yy[-8]

	pkhbt   r0, r0, r3, lsl #16               @ r0 --- yy[-6], yy[-5]
	pkhbt   r3, r10, r11, lsl #16             @ r3 --- yy[-8], yy[-7]

	smlad   r14, r0, r6, r14                  @ a[6] * yy[-6] + a[5] * yy[-5]
	smlad   r14, r3, r7, r14                  @ a[8] * yy[-8] + a[7] * yy[-7]
        
	ldrsh   r10, [r9, #-18]                   @ yy[-9]
	ldrsh   r11, [r9, #-20]                   @ yy[-10]
	ldr     r3, [r13, #4]
	pkhbt   r10, r10, r11, lsl #16            @ r10 --- yy[-10], yy[-9]

	smlad   r14, r10, r8, r14                 @ a[10] * yy[-10] + a[9] * y[-9]
        rsb     r14, r14, #0	
        
        @s = ASM_L_shl(s, 4)
        ssat    r14, #32, r14, lsl #4 

        mov     r10, #0x8000
        qadd    r11, r14, r10
        subs    r12, r12, #1        
        mov     r11, r11, asr #16

        strh    r11, [r9], #2
        strh    r11, [r2], #2
        bgt     L10_4

        add     r13, r13, #116        
        ldmfd   r13!, {r4 - r11, r15}  
        @.END

