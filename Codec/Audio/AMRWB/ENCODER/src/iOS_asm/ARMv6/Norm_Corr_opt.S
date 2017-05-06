@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************

@static void Norm_Corr (Word16 exc[], 
@                       Word16 xn[], 
@                       Word16 h[],
@                       Word16 L_subfr,
@                       Word16 t_min, 
@                       Word16 t_max,
@                       Word16 corr_norm[])
@
        #include "voAMRWBEncID.h"
	.text     .align 4 
        .globl    _voAMRWBEncNorm_corr_asm 
        .globl    _voAMRWBEncConvolve_asm
        .globl    _Isqrt_n

                  
_voAMRWBEncNorm_corr_asm:

        stmfd      r13!, {r4 - r12, r14}  
        sub        r13, r13, #172
  
        add        r8, r13, #20                 @get the excf[L_subFR]
        ldr        r4, [r13, #212]              @get t_min
        rsb        r11, r4, #0                  @k = -t_min
        add        r5, r0, r11, lsl #1          @get the &exc[k]   
        
        @transfer Convolve function
        stmfd       sp!, {r0 - r3}
        mov         r0, r5
        mov         r1, r2
        mov         r2, r8                       @r2 --- excf[]
        bl          _voAMRWBEncConvolve_asm
        ldmfd       sp!, {r0 - r3}

        @ r8 --- excf[]

	mov         r14, r1                       @copy xn[] address                      
        mov         r5, #64
        mov         r6, #0                       @L_tmp = 0
        mov         r7, #1	
LOOP1:
        ldr         r9,  [r14], #4
	ldr         r10, [r14], #4
	ldr         r11, [r14], #4
	ldr         r12, [r14], #4
	smlad       r6, r9, r9, r6
	smlad       r6, r10, r10, r6
	smlad       r6, r11, r11, r6
	smlad       r6, r12, r12, r6
        subs        r5, r5, #8
        bne         LOOP1

	add         r9, r7, r6, lsl #1           @L_tmp = (L_tmp << 1) + 1
	clz         r7, r9
	sub         r6, r7, #1                   @exp = norm_l(L_tmp)
        rsb         r7, r6, #32                  @exp = 32 - exp
	mov         r6, r7, asr #1         
	rsb         r7, r6, #0                   @scale = -(exp >> 1)
	
        @loop for every possible period
	@for(t = t_min@ t <= t_max@ t++)
	@r7 --- scale r4 --- t_min r8 --- excf[]

LOOPFOR:

        mov         r5, #0                       @L_tmp  = 0
	mov         r6, #0                       @L_tmp1 = 0
	mov         r9, #64  
	mov         r12, r1                      @copy of xn[]
	add         r14, r13, #20                @copy of excf[]
	mov         r8, #0x8000
        	
LOOPi:
	ldr         r11, [r14], #4               @load excf[i], excf[i+1]
        ldr         r10, [r12], #4               @load xn[i], xn[i+1]
	smlad       r6, r11, r11, r6
	smlad       r5, r10, r11, r5
	ldr         r11, [r14], #4               @load excf[i+2], excf[i+3]
	ldr         r10, [r12], #4               @load xn[i+2], xn[i+3]
	smlad       r6, r11, r11, r6
	smlad       r5, r10, r11, r5

	subs        r9, r9, #4
        bne         LOOPi

	@r5 --- L_tmp, r6 --- L_tmp1
	mov         r10, #1
	add         r5, r10, r5, lsl #1          @L_tmp = (L_tmp << 1) + 1
	add         r6, r10, r6, lsl #1          @L_tmp1 = (L_tmp1 << 1) + 1
 
	clz         r10, r5        
	cmp         r5, #0
	rsblt       r11, r5, #0
	clzlt       r10, r11
	sub         r10, r10, #1                 @exp = norm_l(L_tmp)
     
	mov         r5, r5, lsl r10              @L_tmp = (L_tmp << exp)
	rsb         r10, r10, #30                @exp_corr = 30 - exp
	mov         r11, r5, asr #16             @corr = extract_h(L_tmp)

	clz         r5, r6
	sub         r5, r5, #1
	mov         r6, r6, lsl r5               @L_tmp = (L_tmp1 << exp)
	rsb         r5, r5, #30                  @exp_norm = 30 - exp

	@r10 --- exp_corr, r11 --- corr
	@r6  --- L_tmp, r5 --- exp_norm

	@Isqrt_n(&L_tmp, &exp_norm)

	mov         r14, r0
	mov         r12, r1 

        stmfd       sp!, {r0 - r4, r7 - r12, r14}
	add         r1, sp, #4
	add         r0, sp, #0
	str         r6, [sp]
	strh        r5, [sp, #4]
	bl          _Isqrt_n
	ldr         r6, [sp]
	ldrsh       r5, [sp, #4]
        ldmfd       sp!, {r0 - r4, r7 - r12, r14}
	mov         r0, r14
	mov         r1, r12


	mov         r6, r6, asr #16              @norm = extract_h(L_tmp)
	mul         r12, r6, r11
	add         r12, r12, r12                @L_tmp = vo_L_mult(corr, norm)
  
	add         r6, r10, r5
	add         r6, r6, r7                   @exp_corr + exp_norm + scale

        cmp         r6, #0
        rsblt       r6, r6, #0
	movlt       r12, r12, asr r6
        movgt       r12, r12, lsl r6             @L_tmp = L_shl(L_tmp, exp_corr + exp_norm + scale)

        add         r12, r12, r8
        mov         r12, r12, asr #16            @vo_round(L_tmp)

        ldr         r5, [r13, #220]              @ get corr_norm address
	ldr         r6, [r13, #216]              @ get t_max
	add         r10, r5, r4, lsl #1          @ get corr_norm[t] address
	strh        r12, [r10]                   @ corr_norm[t] = vo_round(L_tmp)

	cmp         r4, r6
	beq         Norm_corr_asm_end
 
	add         r4, r4, #1                   @ t_min ++
        
	rsb         r5, r4, #0                   @ k

	mov         r6, #63                      @ i = 63
	mov         r8, r0                       @ exc[]
	mov         r9, r2                       @ h[]
	add         r10, r13, #20                @ excf[]

	add         r8, r8, r5, lsl #1           @ exc[k] address
	add         r9, r9, r6, lsl #1           @ h[i] address
	add         r10, r10, r6, lsl #1         @ excf[i] address
	ldrsh       r11, [r8]                    @ tmp = exc[k]

LOOPK:
        ldrsh       r8, [r9], #-2                @ load h[i]
	ldrsh       r12, [r10, #-2]              @ load excf[i - 1]
	mul         r14, r11, r8
	mov         r8, r14, asr #15
	add         r14, r8, r12
	strh        r14, [r10], #-2
	subs        r6, r6, #1
	bgt         LOOPK

	ldrsh       r8, [r9]                     @ load h[0]
	mul         r14, r11, r8
        ldr         r6, [r13, #216]              @ get t_max
	mov         r8, r14, asr #15
	strh        r8, [r10]                    

	cmp         r4, r6
	ble         LOOPFOR

Norm_corr_asm_end: 
        
        add            r13, r13, #172      
        ldmfd          r13!, {r4 - r12, r15}
    
        @.END


