@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@        VisualOn, Inc. Confidential and Proprietary, 2003-2010 
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@static void Norm_Corr (Word16 exc[],                    /* (i)     : excitation buffer          */
@                       Word16 xn[],                     /* (i)     : target vector              */
@                       Word16 h[],                      /* (i) q15 : impulse response of synth/wgt filters */
@                       Word16 L_subfr,                  /* (i)     : sub-frame length */
@                       Word16 t_min,                    /* (i)     : minimum value of pitch lag.   */
@                       Word16 t_max,                    /* (i)     : maximum value of pitch lag.   */
@                       Word16 corr_norm[])              /* (o) q15 : normalized correlation    */
@

@ r0 --- exc[]
@ r1 --- xn[]
@ r2 --- h[]
@ r3 --- L_subfr
@ r4 --- t_min
@ r5 --- t_max
@ r6 --- corr_norm[]

        #include "voAMRWBEncID.h"
	.text     .align    4
        .globl    _voAMRWBEncNorm_corr_asm 
        .globl    _voAMRWBEncConvolve_asm
        .globl    _Isqrt_n
                  
_voAMRWBEncNorm_corr_asm:

        stmfd          r13!, {r4 - r12, r14}  
        sub            r13, r13, #172
  
        add            r8, r13, #20                 @get the excf[L_subFR]
        ldr            r4, [r13, #212]              @get t_min
        rsb            r11, r4, #0                  @k = -t_min
        add            r5, r0, r11, lsl #1          @get the &exc[k]   
        
        @transfer Convolve function
        stmfd          sp!, {r0 - r3}
        mov            r0, r5
        mov            r1, r2
        mov            r2, r8                       @r2 --- excf[]
        bl             _voAMRWBEncConvolve_asm
        ldmfd          sp!, {r0 - r3}

        @ r8 --- excf[]

	mov            r14, r1                       @copy xn[] address                      
        mov            r7, #1
	vld1.s16       {q0, q1}, [r14]!
	vld1.s16       {q2, q3}, [r14]!
	vld1.s16       {q4, q5}, [r14]!
	vld1.s16       {q6, q7}, [r14]!

        vmull.s16      q10, d0, d0
        vmlal.s16      q10, d1, d1
        vmlal.s16      q10, d2, d2
        vmlal.s16      q10, d3, d3
        vmlal.s16      q10, d4, d4
        vmlal.s16      q10, d5, d5
        vmlal.s16      q10, d6, d6
        vmlal.s16      q10, d7, d7
        vmlal.s16      q10, d8, d8
        vmlal.s16      q10, d9, d9
	vmlal.s16      q10, d10, d10
	vmlal.s16      q10, d11, d11
	vmlal.s16      q10, d12, d12
	vmlal.s16      q10, d13, d13
	vmlal.s16      q10, d14, d14
	vmlal.s16      q10, d15, d15

        vqadd.s32      d20, d20, d21
        vmov.s32       r9,  d20[0]
        vmov.s32       r10, d20[1]
        qadd           r6, r9, r10 
	qadd           r6, r6, r6
        qadd           r9, r6, r7                   @L_tmp = (L_tmp << 1) + 1;
	clz            r7, r9
	sub            r6, r7, #1                   @exp = norm_l(L_tmp)
        rsb            r7, r6, #32                  @exp = 32 - exp
	mov            r6, r7, asr #1         
	rsb            r7, r6, #0                   @scale = -(exp >> 1)
	
        @loop for every possible period
	@for(t = t_min@ t <= t_max@ t++)
	@r7 --- scale r4 --- t_min r8 --- excf[]

LOOPFOR:	
	add            r14, r13, #20                @copy of excf[]
	mov            r12, r1                      @copy of xn[]
	mov            r8, #0x8000

        vld1.s16       {q0, q1}, [r14]!                 @ load 16 excf[]
        vld1.s16       {q2, q3}, [r14]!                 @ load 16 excf[]       
        vld1.s16       {q4, q5}, [r12]!                 @ load 16 x[]
	vld1.s16       {q6, q7}, [r12]!                 @ load 16 x[]
        vmull.s16    q10, d0, d0                      @L_tmp1 += excf[] * excf[]
        vmull.s16    q11, d0, d8                      @L_tmp  += x[] * excf[]                   
        vmlal.s16    q10, d1, d1
        vmlal.s16    q11, d1, d9
        vmlal.s16    q10, d2, d2
        vmlal.s16    q11, d2, d10        
        vmlal.s16    q10, d3, d3
        vmlal.s16    q11, d3, d11
        vmlal.s16    q10, d4, d4
        vmlal.s16    q11, d4, d12
        vmlal.s16    q10, d5, d5
        vmlal.s16    q11, d5, d13
        vmlal.s16    q10, d6, d6
        vmlal.s16    q11, d6, d14
        vmlal.s16    q10, d7, d7
        vmlal.s16    q11, d7, d15

	vld1.s16       {q0, q1}, [r14]!                 @ load 16 excf[]
        vld1.s16       {q2, q3}, [r14]!                 @ load 16 excf[]
        vld1.s16       {q4, q5}, [r12]!                 @ load 16 x[]
        vld1.s16       {q6, q7}, [r12]!                 @ load 16 x[]
        vmlal.s16    q10, d0, d0
        vmlal.s16    q11, d0, d8
        vmlal.s16    q10, d1, d1
        vmlal.s16    q11, d1, d9
        vmlal.s16    q10, d2, d2
        vmlal.s16    q11, d2, d10        
        vmlal.s16    q10, d3, d3
        vmlal.s16    q11, d3, d11
        vmlal.s16    q10, d4, d4
        vmlal.s16    q11, d4, d12
        vmlal.s16    q10, d5, d5
        vmlal.s16    q11, d5, d13
        vmlal.s16    q10, d6, d6
        vmlal.s16    q11, d6, d14
        vmlal.s16    q10, d7, d7
        vmlal.s16    q11, d7, d15

        vqadd.s32      d20, d20, d21
        vqadd.s32      d22, d22, d23

	vpadd.s32      d20, d20, d20                   @d20[0] --- L_tmp1 << 1
	vpadd.s32      d22, d22, d22                   @d22[0] --- L_tmp << 1

	vmov.s32       r6, d20[0]
        vmov.s32       r5, d22[0]	

	@r5 --- L_tmp, r6 --- L_tmp1
	mov            r10, #1
	add            r5, r10, r5, lsl #1                     @L_tmp = (L_tmp << 1) + 1
	add            r6, r10, r6, lsl #1                     @L_tmp1 = (L_tmp1 << 1) + 1
 
	clz            r10, r5        
	cmp            r5, #0
	rsblt          r11, r5, #0
	clzlt          r10, r11
	sub            r10, r10, #1                 @exp = norm_l(L_tmp)
     
	mov            r5, r5, lsl r10              @L_tmp = (L_tmp << exp)
	rsb            r10, r10, #30                @exp_corr = 30 - exp
	mov            r11, r5, asr #16             @corr = extract_h(L_tmp)

	clz            r5, r6
	sub            r5, r5, #1
	mov            r6, r6, lsl r5               @L_tmp = (L_tmp1 << exp)
	rsb            r5, r5, #30                  @exp_norm = 30 - exp

	@r10 --- exp_corr, r11 --- corr
	@r6  --- L_tmp, r5 --- exp_norm

	@Isqrt_n(&L_tmp, &exp_norm)

	mov            r14, r0
	mov            r12, r1 

        stmfd          sp!, {r0 - r4, r7 - r12, r14}
	add            r1, sp, #4
	add            r0, sp, #0
	str            r6, [sp]
	strh           r5, [sp, #4]
	bl             _Isqrt_n
	ldr            r6, [sp]
	ldrsh          r5, [sp, #4]
        ldmfd          sp!, {r0 - r4, r7 - r12, r14}
	mov            r0, r14
	mov            r1, r12


	mov            r6, r6, asr #16              @norm = extract_h(L_tmp)
	mul            r12, r6, r11
	add            r12, r12, r12                @L_tmp = vo_L_mult(corr, norm)
  
	add            r6, r10, r5
	add            r6, r6, r7                   @exp_corr + exp_norm + scale

        cmp            r6, #0
        rsblt          r6, r6, #0
	movlt          r12, r12, asr r6
        movgt          r12, r12, lsl r6             @L_tmp = L_shl(L_tmp, exp_corr + exp_norm + scale)

        add            r12, r12, r8
        mov            r12, r12, asr #16            @vo_round(L_tmp)

        ldr            r5, [r13, #220]              @ get corr_norm address
	ldr            r6, [r13, #216]              @ get t_max
	add            r10, r5, r4, lsl #1          @ get corr_norm[t] address
	strh           r12, [r10]                   @ corr_norm[t] = vo_round(L_tmp)

	cmp            r4, r6
	beq            Norm_corr_asm_end
 
	add            r4, r4, #1                   @ t_min ++ 
	rsb            r5, r4, #0                   @ k

	mov            r6, #63                      @ i = 63
	mov            r8, r0                       @ exc[]
	mov            r9, r2                       @ h[]
	add            r10, r13, #20                @ excf[]

	add            r8, r8, r5, lsl #1           @ exc[k] address
	add            r9, r9, r6, lsl #1           @ h[i] address
	add            r10, r10, r6, lsl #1         @ excf[i] address
	ldrsh          r11, [r8]                    @ tmp = exc[k]

LOOPK:
        ldrsh          r8, [r9], #-2                @ load h[i]
	ldrsh          r12, [r10, #-2]              @ load excf[i - 1]
	mul            r14, r11, r8
	mov            r8, r14, asr #15
	add            r14, r8, r12
	strh           r14, [r10], #-2
	subs           r6, r6, #1
	bgt            LOOPK

	ldrsh          r8, [r9]                     @ load h[0]
	mul            r14, r11, r8
        ldr            r6, [r13, #216]              @ get t_max
	mov            r8, r14, asr #15
	strh           r8, [r10]                    

	cmp            r4, r6
	ble            LOOPFOR

Norm_corr_asm_end: 
        
        add            r13, r13, #172      
        ldmfd          r13!, {r4 - r12, r15}
    
        @ENdFUNC
        @.ENd


