#include "voMP3DecID.h"
	.text
	.align 4
        
        .globl    _IDCT9_D
        .globl    _csa_table
	.globl	  _voMP3Decidct9_2
        .globl	  _AliasReduce

_voMP3Decidct9_2:	@PROC
	stmdb           sp!, {r4 - r11, lr}
M2844:
	mov		r14, r0
	ldr		r2,  table1
	
	vld1.32		{d0, d1, d2, d3}, [r14]!
	vld1.32		{d4, d5, d6, d7}, [r14]!
	vld1.32		{d8}, [r14]!	
	
	vsub.s32	d11, d0, d6				@ a1 = x0 - x6@       
        vsub.s32	d12, d1, d5				@ a2 = x1 - x5@ 
	vadd.s32	d13, d1, d5             @ a3 = x1 + x5@ 
        vsub.s32        d14, d2, d4             @ a4 = x2 - x4@
        vadd.s32        d15, d2, d4             @ a5 = x2 + x4@
        vadd.s32        d16, d2, d8             @ a6 = x2 + x8@  
        vadd.s32        d17, d1, d7             @ a7 = x1 + x7@
        vshr.s32	d9, d6, #1
        vld1.s32	{d2}, [r2]!
        vsub.s32	d18, d16, d15           @ a8 = a6 - a5@ 
        vadd.s32        d22, d0, d9             @ a12 = x0 + (x6 >> 1)@
        vld1.s32	{d4}, [r2]!	 
        vsub.s32	d20, d12, d7            @ a10 = a2 - x7@ 
        vsub.s32	d19, d13, d17           @ a9 = a3 - a7@  
        vld1.s32	{d0[]}, [r2]
	vsub.s32	d21, d14, d8            @ a11 = a4 - x8@  
        vqdmulh.s32     d1, d3, d2[0]			@ m1 =  MUL_32(c9_0, x3)@
	vqdmulh.s32	d7, d18, d2[1]			@ m7 =  MUL_32(c9_1, a8)@
	vqdmulh.s32	d30, d20, d2[0]			@ m3 =  MUL_32(c9_0, a10)@
	vqdmulh.s32     d5, d15, d2[1]			@ m5 =  MUL_32(c9_1, a5)@
	vqdmulh.s32	d6, d16, d4[0]			@ m6 =  MUL_32(c9_2, a6)@
	vqdmulh.s32	d8, d15, d4[0]			@ m8 =  MUL_32(c9_2, a5)@
	vadd.s32	d26, d5, d6				@ a16 = ( m5 + m6 ) << 1@
	vqdmulh.s32	d28, d13, d4[1]			@ m11 = MUL_32(c9_3, a3)@
	vshr.s32	d31, d21, #1
	vqdmulh.s32	d9, d19, d4[1]			@ m9 =  MUL_32(c9_3, a9)@
	vqdmulh.s32	d10, d17, d0	    	@ m10 = MUL_32(c9_4, a7)@
	vqdmulh.s32	d12, d19, d0			@ m12 = MUL_32(c9_4, a9)@
	vsub.s32	d17, d7, d8				@ a17 = ( m7 - m8 ) << 1@
	vadd.s32	d19, d9, d10			@ a19 = ( m9 + m10) << 1@
	
	vsub.s32	d14, d22, d1			@ a14 = a12  -  (  m1 << 1)@
	vadd.s32	d13, d22, d1			@ a13 = a12  +  (  m1 << 1)@		
	vsub.s32	d20, d28, d12			@ a20 = (m11 - m12) << 1@
	vadd.s32	d22, d13, d26			@ a22 = a13 + a16@
	vadd.s32	d23, d14, d26			@ a23 = a14 + a16@
	vadd.s32	d18, d26, d17			@ a18 = a16 + a17@
	vadd.s32	d24, d14, d17			@ a24 = a14 + a17@
	vadd.s32	d25, d13, d17			@ a25 = a13 + a17@
	vsub.s32	d26, d14, d18			@ a26 = a14 - a18@
	vsub.s32	d27, d13, d18			@ a27 = a13 - a18@
	vadd.s32	d15, d11, d31			@ a15 = a1   +  ( a11 >> 1)@
	vsub.s32	d4, d11, d21            @ x4 = a1 - a11@			x[4] = x4@
	vadd.s32	d0, d22, d19			@ x0 = a22 + a19@			x[0] = x0@
	vsub.s32	d21, d20, d19			@ a21 = a20 - a19@
	vadd.s32	d1, d15, d30			@ x1 = a15 + (m3 << 1)@		x[1] = x1@
	vadd.s32        d2, d24, d20			@ x2 = a24 + a20@			x[2] = x2@
	vsub.s32	d3, d26, d21			@ x3 = a26 - a21@			x[3] = x3@
	vadd.s32	d5, d27, d21			@ x5 = a27 + a21@			x[5] = x5@
	vsub.s32	d6, d25, d20			@ x6 = a25 - a20@			x[6] = x6@
	vsub.s32	d7, d15, d30			@ x7 = a15 - (m3 << 1)@		x[7] = x7@
	vsub.s32	d8, d23, d19			@ x8 = a23 - a19@			x[8] = x8@

	vuzp.s32	q0, q1
	vuzp.s32	q2, q3
	
	vst1.s32	{q0}, [r0]!
	vst1.s32	{q2}, [r0]!
	vst1.s32	d8[0], [r0]!
	vst1.s32	{q1}, [r0]!
	vst1.s32	{q3}, [r0]!
	vst1.s32	d8[1], [r0]!	
		
	ldmia           sp!, {r4 - r11, pc}
	@ENdP  @ idct9_2

table1:
        .word           _IDCT9_D

	
	@AREA	.text, COdE, REAdONLY


_AliasReduce: @PROC
	stmdb         sp!, {r4 - r11, lr}
M2693:
        add		  r10, r0, #72
        cmp		  r1, #0      
        sub		  r11, r10, #32
        ldr		  r12, 	table2
        ble     	  L1604


	@#define INT_AA(j) \
        @tmp0 = ptr[-1-j]@\
        @tmp1 = ptr[   j]@\
        @tmp2= MUL_32(tmp0 + tmp1, csa[0+4*j])@\
        @ptr[-1-j] = 4*(tmp2 - MUL_32(tmp1, csa[2+4*j]))@\
        @ptr[   j] = 4*(tmp2 + MUL_32(tmp0, csa[3+4*j]))@

	vld4.32		{d8, d10, d12, d14}, [r12]!
	vld4.32		{d9, d11, d13, d15}, [r12]!
	vld4.32		{d16, d18, d20, d22}, [r12]!
	vld4.32		{d17, d19, d21, d23}, [r12]!
	
L2691:
	vld1.32	  	{d0, d1, d2, d3}, [r11]		        @ tmp0
	vld1.32		{d4, d5, d6, d7}, [r10]	        	@ tmp1	
	
	vrev64.32	q1, q1
	vrev64.32	q0, q0
		
	vswp		d2, d3			        	@ tmp0
	vswp		d0, d1			        	@ tmp0
	
	vadd.s32	q12, q1, q2		        	@ tmp0 + tmp1
	vadd.s32	q13, q0, q3		        	@ tmp0 + tmp1	
	
	vqdmulh.s32 	q12, q12, q4				@ tmp2= MUL_32(tmp0 + tmp1, csa[0+4*j])
	vqdmulh.s32 	q13, q13, q8				@ tmp2= MUL_32(tmp0 + tmp1, csa[0+4*j])
	
	vqdmulh.s32		q2, q2, q6			@ MUL_32(tmp1, csa[2+4*j])
	vqdmulh.s32 	q3, q3, q10				@ MUL_32(tmp1, csa[2+4*j])
	
	vqdmulh.s32 	q1, q1, q7				@ MUL_32(tmp0, csa[3+4*j])
	vqdmulh.s32 	q0, q0, q11				@ MUL_32(tmp0, csa[3+4*j])
		
	vsub.s32	q2, q12, q2
	vsub.s32	q3, q13, q3
	
	vadd.s32	q12, q12, q1
	vadd.s32	q13, q13, q0
	
	vqshl.s32	q1, q2, #1
	vqshl.s32	q0, q3, #1	

	vqshl.s32	q12, q12, #1	
	
	vrev64.32	q1, q1
	vrev64.32	q0, q0
		
	vswp.32		d2, d3						
	vswp.32		d0, d1						
	
	vqshl.s32	q13, q13, #1	
	
	vst1.32	  	{d0, d1, d2, d3}, [r11]			@ tmp0
	vst1.32		{d24, d25, d26, d27}, [r10]		@ tmp1	
	
	subs            r1, r1, #1
	add             r10, r10, #0x48
	add		r11, r11, #0x48
	bne             L2691

L1604:
	ldmia           sp!, {r4 - r11, pc}

table2:
        .word           _csa_table
	
	@.ENd
	
