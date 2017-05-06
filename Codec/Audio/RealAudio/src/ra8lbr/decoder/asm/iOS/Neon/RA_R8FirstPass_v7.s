        #include "voRADecID.h"
	@AREA	.text, CODE, READONLY
	.text
        .align  4
	.globl	_R8FirstPass

_R8FirstPass: @PROC
	stmdb     sp!, {r4 - r11, lr}
	ldr       r3, L2652
	cmp       r1, #0
	
	vdup.i32  q15, r3	
	beq       L2125
L2123:
	vld1.i32		{d0, d1, d2, d3},	[r0]!
	vld1.i32		{d8, d9, d10, d11},	[r0]!
		
	vadd.s32		d4, d0, d1		@ ar = x[0] + x[2]@ai = x[1] + x[3]@
	vsub.s32		d5, d0, d1		@ br = x[0] - x[2]@bi = x[1] - x[3]@	
	vsub.s32		d7, d2, d3		@ dr = x[4] - x[6]@di = x[5] - x[7]@	
	vadd.s32		d6, d2, d3		@ cr = x[4] + x[6]@ci = x[5] + x[7]@
	vrev64.i32		d7, d7	
	
	vadd.s32		q0, q2, q3		@ sr = ar + cr@si = ai + ci@vr = br + di@ti = bi + dr@
	vsub.s32		q1, q2, q3		@ ur = ar - cr@ui = ai - ci@tr = br - di@vi = bi - dr@

	vrev64.i32		d3, d3	

	vadd.s32		d4, d8, d9		@ ar = x[ 8] + x[10]@ai = x[ 9] + x[11]@
	vsub.s32		d7, d10, d11	@ dr = x[12] - x[14]@di = x[13] - x[15]@	
	vadd.s32		d6, d10, d11	@ cr = x[12] + x[14]@ci = x[13] + x[15]@
	vrev64.i32		d7, d7	
	vsub.s32		d5, d8, d9		@ br = x[ 8] - x[10]@bi = x[ 9] - x[11]@
	
	vtrn.32			d1, d3	
	
	vadd.s32		q4, q2, q3		@ wr = (ar + cr) >> 1@wi = (ai + ci) >> 1@cr = br + di@ai = bi + dr@
	vsub.s32		q5, q2, q3		@ yr = (ar - cr) >> 1@yi = (ai - ci) >> 1@ar = br - di@ci = bi - dr@
	
	vrev64.i32		d3, d3
	
	vshr.s32		d8, d8, #1		 
	vshr.s32		q0, q0, #1
	vrev64.i32		d10, d10
	vtrn.32			d11, d9
	vshr.s32		q1, q1, #1
	vshr.s32		d10, d10, #1
	vrev64.i32		d9, d9
	
	sub       		r0, r0, #0x40
	
	vadd.s32		d12, d0, d8
	vsub.s32		d16, d0, d8	
	vadd.s32		d14, d2, d10
	vsub.s32		d18, d2, d10
	
	vsub.s32		d4, d11, d9
	vadd.s32		d5, d11, d9
	
	vrev64.i32		d18, d18
	
	vqdmulh.s32		q3, q2, q15
	vtrn.32			d14, d18
	vtrn.32			d6, d7
	vrev64.i32		d18, d18	
	
	vsub.s32		d15, d3, d6
	vrev64.i32		d7, d7
	vadd.s32		d19, d3, d6
	vadd.s32		d13, d1, d7
	vsub.s32		d17, d1, d7
	
	vrev64.i32		d17, d17
	vtrn.32			d13, d17
	vrev64.i32		d17, d17
	
	sub       		r1, r1, #1	
	
	vst1.i32		{d12, d13, d14, d15}, [r0]!
	vst1.i32		{d16, d17, d18, d19}, [r0]!	


	cmp       		r1, #0
	bhi       		L2123
L2125:

	ldmia     sp!, {r4 - r11, pc}
L2652:
	.word       0x2d413ccd

	@ENDP  @ R8FirstPass

	.globl	_R4FirstPass

	@AREA	.text, CODE, READONLY

_R4FirstPass: @PROC
	stmdb     	sp!, {r4 - r11, lr}
M3649:
	cmp       	r1, #0
	beq       	L3125
L3123:
	vld1.i32		{d0, d1, d2, d3}, [r0]
	vadd.s32		d4, d0, d1
	vsub.s32		d5, d0, d1
	vsub.s32		d7, d2, d3
	vadd.s32		d6, d2, d3
	vrev64.32		d7, d7
	
	vadd.s32		q4, q2, q3
	vsub.s32		q5, q2, q3
	
	vrev64.32		d11, d11
	vtrn.32			d9, d11
	sub       		r1, r1, #1	
	vrev64.32		d11, d11
	vst1.i32		{d8, d9, d10, d11}, [r0]!

	cmp       		r1, #0	
	bhi       		L3123
L3125:
	ldmia    		sp!, {r4 - r11, pc}
	@ENDP  @ R4FirstPass
	
	@.END
