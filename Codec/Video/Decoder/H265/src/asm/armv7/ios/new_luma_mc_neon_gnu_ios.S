
#include     "h265dec_ASM_config.h"

    .if MC_ASM_ENABLED==1
     


@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_put_MC_InterLuma_H0V0_neon
_VO_put_MC_InterLuma_H0V0_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]

loc_4c:					@ code xref: .text:0000009cj
					@ .text:000000f4j ...
		cmp		r7, #0x20 @ ' '
		blt		loc_a4
		ands		r5, r0,	#0xf
		bne		loc_a4
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_68:					@ code xref: .text:0000008cj
		vld1.64		{d0-d3}, [r4],r3
		vld1.64		{d4-d7}, [r4],r3
		vld1.64		{d8-d11}, [r4],r3
		vld1.64		{d12-d15}, [r4],r3
		subs		r8, r8,	#4
		vst1.64		{d0-d3}, [r5,:128],r1
		vst1.64		{d4-d7}, [r5,:128],r1
		vst1.64		{d8-d11}, [r5,:128],r1
		vst1.64		{d12-d15}, [r5,:128],r1
		bgt		loc_68
		add		r2, r2,	#0x20
		add		r0, r0,	#0x20
		subs		r7, r7,	#0x20
		bgt		loc_4c
		b		locret_1fc
@ ---------------------------------------------------------------------------

loc_a4:					@ code xref: .text:00000050j
					@ .text:00000058j
		cmp		r7, #0x10
		blt		loc_fc
		ands		r5, r0,	#0xf
		bne		loc_fc
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_c0:					@ code xref: .text:000000e4j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d4-d5}, [r4],r3
		vld1.64		{d6-d7}, [r4],r3
		subs		r8, r8,	#4
		vst1.64		{d0-d1}, [r5,:128],r1
		vst1.64		{d2-d3}, [r5,:128],r1
		vst1.64		{d4-d5}, [r5,:128],r1
		vst1.64		{d6-d7}, [r5,:128],r1
		bgt		loc_c0
		add		r2, r2,	#0x10
		add		r0, r0,	#0x10
		subs		r7, r7,	#0x10
		bgt		loc_4c
		b		locret_1fc
@ ---------------------------------------------------------------------------

loc_fc:					@ code xref: .text:000000a8j
					@ .text:000000b0j
		cmp		r7, #8
		blt		loc_158
		ands		r5, r0,	#7
		bne		loc_158
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_118:				@ code xref: .text:00000140j
		vld1.64		{d0}, [r4],r3
		vld1.64		{d1}, [r4],r3
		vld1.64		{d2}, [r4],r3
		vld1.64		{d3}, [r4],r3
		subs		r8, r8,	#4
		vst1.64		{d0}, [r5,:64],r1
		vst1.64		{d1}, [r5,:64],r1
		blt		loc_144
		vst1.64		{d2}, [r5,:64],r1
		vst1.64		{d3}, [r5,:64],r1
		bgt		loc_118

loc_144:				@ code xref: .text:00000134j
		add		r2, r2,	#8
		add		r0, r0,	#8
		subs		r7, r7,	#8
		bgt		loc_4c
		b		locret_1fc
@ ---------------------------------------------------------------------------

loc_158:				@ code xref: .text:00000100j
					@ .text:00000108j
		cmp		r7, #4
		blt		loc_1b4
		ands		r5, r0,	#3
		bne		loc_1b4
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_174:				@ code xref: .text:0000019cj
		vld1.32		{d0[0]}, [r4],r3
		vld1.32		{d0[1]}, [r4],r3
		vld1.32		{d1[0]}, [r4],r3
		vld1.32		{d1[1]}, [r4],r3
		subs		r8, r8,	#4
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d0[1]}, [r5,:32],r1
		blt		loc_1a0
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d1[1]}, [r5,:32],r1
		bgt		loc_174

loc_1a0:				@ code xref: .text:00000190j
		add		r2, r2,	#4
		add		r0, r0,	#4
		subs		r7, r7,	#4
		bgt		loc_4c
		b		locret_1fc
@ ---------------------------------------------------------------------------

loc_1b4:				@ code xref: .text:0000015cj
					@ .text:00000164j
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_1c0:				@ code xref: .text:000001e8j
		vld1.16		{d0[0]}, [r4],r3
		vld1.16		{d0[1]}, [r4],r3
		vld1.16		{d0[2]}, [r4],r3
		vld1.16		{d0[3]}, [r4],r3
		subs		r8, r8,	#4
		vst1.16		{d0[0]}, [r5,:16],r1
		vst1.16		{d0[1]}, [r5,:16],r1
		blt		loc_1ec
		vst1.16		{d0[2]}, [r5,:16],r1
		vst1.16		{d0[3]}, [r5,:16],r1
		bgt		loc_1c0

loc_1ec:				@ code xref: .text:000001dcj
		add		r2, r2,	#2
		add		r0, r0,	#2
		subs		r7, r7,	#2
		bgt		loc_4c

locret_1fc:				@ code xref: .text:000000a0j
					@ .text:000000f8j ...
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------
.align 4
.globl _VO_put_MC_InterLuma_H1V0_neon
_VO_put_MC_InterLuma_H1V0_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d12, #4
		vmov.i8		d13, #0xa
		vmov.i8		d14, #0x3a @ ':'
		vmov.i8		d15, #0x11
		vmov.i8		d30, #5
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_348

loc_228:				@ code xref: .text:00000340j
					@ .text:0000045cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#3
		subs		r7, r7,	#4
		ble		loc_358

loc_23c:				@ code xref: .text:00000330j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #6
		vext.8		d6, d2,	d3, #6
		vsubl.u8	q2, d4,	d0
		vsubl.u8	q3, d6,	d2
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #2
		vext.8		d10, d2, d3, #1
		vext.8		d11, d2, d3, #2
		vmlal.u8	q2, d8,	d12
		vmlsl.u8	q2, d9,	d13
		vmlal.u8	q3, d10, d12
		vmlsl.u8	q3, d11, d13
		vext.8		d8, d0,	d1, #3
		vext.8		d9, d0,	d1, #4
		vext.8		d10, d2, d3, #3
		vext.8		d11, d2, d3, #4
		vmlal.u8	q2, d8,	d14
		vmlal.u8	q2, d9,	d15
		vmlal.u8	q3, d10, d14
		vmlal.u8	q3, d11, d15
		vext.8		d8, d0,	d1, #5
		vext.8		d9, d2,	d3, #5
		vmlsl.u8	q2, d8,	d30
		vmlsl.u8	q3, d9,	d30
		vext.8		d20, d16, d17, #6
		vext.8		d22, d18, d19, #6
		vsubl.u8	q10, d20, d16
		vsubl.u8	q11, d22, d18
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #2
		vext.8		d26, d18, d19, #1
		vext.8		d27, d18, d19, #2
		vmlal.u8	q10, d24, d12
		vmlsl.u8	q10, d25, d13
		vmlal.u8	q11, d26, d12
		vmlsl.u8	q11, d27, d13
		vext.8		d28, d16, d17, #3
		vext.8		d29, d16, d17, #4
		vext.8		d24, d18, d19, #3
		vext.8		d25, d18, d19, #4
		vmlal.u8	q10, d28, d14
		vmlal.u8	q10, d29, d15
		vmlal.u8	q11, d24, d14
		vmlal.u8	q11, d25, d15
		vext.8		d28, d16, d17, #5
		vext.8		d29, d18, d19, #5
		vmlsl.u8	q10, d28, d30
		vmlsl.u8	q11, d29, d30
		vqrshrun.s16	d0, q2,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d16, q10, #6
		vqrshrun.s16	d17, q11, #6
		vst1.64		{d0}, [r5,:64],r1
		vst1.64		{d1}, [r5,:64],r1
		vst1.64		{d16}, [r5,:64],r1
		vst1.64		{d17}, [r5,:64],r1
		bne		loc_23c
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_228
		b		locret_460
@ ---------------------------------------------------------------------------

loc_348:				@ code xref: .text:00000224j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#3
		sub		r7, r7,	#4

loc_358:				@ code xref: .text:00000238j
					@ .text:0000044cj
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #6
		vext.8		d6, d2,	d3, #6
		vsubl.u8	q2, d4,	d0
		vsubl.u8	q3, d6,	d2
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #2
		vext.8		d10, d2, d3, #1
		vext.8		d11, d2, d3, #2
		vmlal.u8	q2, d8,	d12
		vmlsl.u8	q2, d9,	d13
		vmlal.u8	q3, d10, d12
		vmlsl.u8	q3, d11, d13
		vext.8		d8, d0,	d1, #3
		vext.8		d9, d0,	d1, #4
		vext.8		d10, d2, d3, #3
		vext.8		d11, d2, d3, #4
		vmlal.u8	q2, d8,	d14
		vmlal.u8	q2, d9,	d15
		vmlal.u8	q3, d10, d14
		vmlal.u8	q3, d11, d15
		vext.8		d8, d0,	d1, #5
		vext.8		d9, d2,	d3, #5
		vmlsl.u8	q2, d8,	d30
		vmlsl.u8	q3, d9,	d30
		vext.8		d20, d16, d17, #6
		vext.8		d22, d18, d19, #6
		vsubl.u8	q10, d20, d16
		vsubl.u8	q11, d22, d18
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #2
		vext.8		d26, d18, d19, #1
		vext.8		d27, d18, d19, #2
		vmlal.u8	q10, d24, d12
		vmlsl.u8	q10, d25, d13
		vmlal.u8	q11, d26, d12
		vmlsl.u8	q11, d27, d13
		vext.8		d28, d16, d17, #3
		vext.8		d29, d16, d17, #4
		vext.8		d24, d18, d19, #3
		vext.8		d25, d18, d19, #4
		vmlal.u8	q10, d28, d14
		vmlal.u8	q10, d29, d15
		vmlal.u8	q11, d24, d14
		vmlal.u8	q11, d25, d15
		vext.8		d28, d16, d17, #5
		vext.8		d29, d18, d19, #5
		vmlsl.u8	q10, d28, d30
		vmlsl.u8	q11, d29, d30
		vqrshrun.s16	d0, q2,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d16, q10, #6
		vqrshrun.s16	d17, q11, #6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d16[0]}, [r5,:32],r1
		vst1.32		{d17[0]}, [r5,:32],r1
		bne		loc_358
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_228

locret_460:				@ code xref: .text:00000344j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------
     
		.align 4
.globl _VO_put_MC_InterLuma_H2V0_neon
_VO_put_MC_InterLuma_H2V0_neon:
		stmfd		sp!, {r4-r10,lr}
		mov		r6, #4
		movt  r6, 0xb
		mov		r7, #0xa
		vmov		d6, r6,	r7
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_5d8

loc_488:				@ code xref: .text:000005d0j
					@ .text:0000071cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#3
		subs		r7, r7,	#4
		ble		loc_5e8

loc_49c:				@ code xref: .text:000005c0j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d0,	d1, #4
		vext.8		d12, d2, d3, #3
		vext.8		d13, d2, d3, #4
		vext.8		d22, d16, d17, #3
		vext.8		d23, d16, d17, #4
		vext.8		d28, d18, d19, #3
		vext.8		d29, d18, d19, #4
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q6, d12, d13
		vaddl.u8	q11, d22, d23
		vaddl.u8	q14, d28, d29
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #6
		vext.8		d14, d2, d3, #1
		vext.8		d15, d2, d3, #6
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #6
		vext.8		d30, d18, d19, #1
		vext.8		d31, d18, d19, #6
		vaddl.u8	q4, d8,	d9
		vaddl.u8	q7, d14, d15
		vaddl.u8	q12, d24, d25
		vaddl.u8	q15, d30, d31
		vmla.i16	q4, q2,	d6[2]
		vmla.i16	q7, q6,	d6[2]
		vmla.i16	q12, q11, d6[2]
		vmla.i16	q15, q14, d6[2]
		vext.8		d10, d0, d1, #2
		vext.8		d11, d0, d1, #5
		vext.8		d20, d2, d3, #2
		vext.8		d21, d2, d3, #5
		vext.8		d26, d16, d17, #2
		vext.8		d27, d16, d17, #5
		vext.8		d28, d18, d19, #2
		vext.8		d29, d18, d19, #5
		vaddl.u8	q5, d10, d11
		vaddl.u8	q10, d20, d21
		vaddl.u8	q13, d26, d27
		vaddl.u8	q14, d28, d29
		vext.8		d5, d0,	d1, #7
		vext.8		d13, d2, d3, #7
		vext.8		d23, d16, d17, #7
		vext.8		d19, d18, d19, #7
		vaddl.u8	q2, d0,	d5
		vaddl.u8	q6, d2,	d13
		vaddl.u8	q11, d16, d23
		vaddl.u8	q9, d18, d19
		vshl.i16	q4, q4,	#2
		vshl.i16	q7, q7,	#2
		vshl.i16	q12, q12, #2
		vshl.i16	q15, q15, #2
		vmla.i16	q2, q5,	d6[1]
		vmla.i16	q6, q10, d6[1]
		vmla.i16	q11, q13, d6[1]
		vmla.i16	q9, q14, d6[1]
		vsub.i16	q4, q4,	q2
		vsub.i16	q5, q7,	q6
		vsub.i16	q8, q12, q11
		vsub.i16	q9, q15, q9
		vqrshrun.s16	d0, q4,	#6
		vqrshrun.s16	d1, q5,	#6
		vqrshrun.s16	d16, q8, #6
		vqrshrun.s16	d17, q9, #6
		vst1.64		{d0}, [r5,:64],r1
		vst1.64		{d1}, [r5,:64],r1
		vst1.64		{d16}, [r5,:64],r1
		vst1.64		{d17}, [r5,:64],r1
		bne		loc_49c
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_488
		b		locret_720
@ ---------------------------------------------------------------------------

loc_5d8:				@ code xref: .text:00000484j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#3
		sub		r7, r7,	#4

loc_5e8:				@ code xref: .text:00000498j
					@ .text:0000070cj
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d0,	d1, #4
		vext.8		d12, d2, d3, #3
		vext.8		d13, d2, d3, #4
		vext.8		d22, d16, d17, #3
		vext.8		d23, d16, d17, #4
		vext.8		d28, d18, d19, #3
		vext.8		d29, d18, d19, #4
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q6, d12, d13
		vaddl.u8	q11, d22, d23
		vaddl.u8	q14, d28, d29
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #6
		vext.8		d14, d2, d3, #1
		vext.8		d15, d2, d3, #6
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #6
		vext.8		d30, d18, d19, #1
		vext.8		d31, d18, d19, #6
		vaddl.u8	q4, d8,	d9
		vaddl.u8	q7, d14, d15
		vaddl.u8	q12, d24, d25
		vaddl.u8	q15, d30, d31
		vmla.i16	q4, q2,	d6[2]
		vmla.i16	q7, q6,	d6[2]
		vmla.i16	q12, q11, d6[2]
		vmla.i16	q15, q14, d6[2]
		vext.8		d10, d0, d1, #2
		vext.8		d11, d0, d1, #5
		vext.8		d20, d2, d3, #2
		vext.8		d21, d2, d3, #5
		vext.8		d26, d16, d17, #2
		vext.8		d27, d16, d17, #5
		vext.8		d28, d18, d19, #2
		vext.8		d29, d18, d19, #5
		vaddl.u8	q5, d10, d11
		vaddl.u8	q10, d20, d21
		vaddl.u8	q13, d26, d27
		vaddl.u8	q14, d28, d29
		vext.8		d5, d0,	d1, #7
		vext.8		d13, d2, d3, #7
		vext.8		d23, d16, d17, #7
		vext.8		d19, d18, d19, #7
		vaddl.u8	q2, d0,	d5
		vaddl.u8	q6, d2,	d13
		vaddl.u8	q11, d16, d23
		vaddl.u8	q9, d18, d19
		vshl.i16	q4, q4,	#2
		vshl.i16	q7, q7,	#2
		vshl.i16	q12, q12, #2
		vshl.i16	q15, q15, #2
		vmla.i16	q2, q5,	d6[1]
		vmla.i16	q6, q10, d6[1]
		vmla.i16	q11, q13, d6[1]
		vmla.i16	q9, q14, d6[1]
		vsub.i16	q4, q4,	q2
		vsub.i16	q5, q7,	q6
		vsub.i16	q8, q12, q11
		vsub.i16	q9, q15, q9
		vqrshrun.s16	d0, q4,	#6
		vqrshrun.s16	d1, q5,	#6
		vqrshrun.s16	d16, q8, #6
		vqrshrun.s16	d17, q9, #6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d16[0]}, [r5,:32],r1
		vst1.32		{d17[0]}, [r5,:32],r1
		bne		loc_5e8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_488

locret_720:				@ code xref: .text:000005d4j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		
.align 4
.globl _VO_put_MC_InterLuma_H3V0_neon
_VO_put_MC_InterLuma_H3V0_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d12, #5
		vmov.i8		d13, #0x11
		vmov.i8		d14, #0x3a @ ':'
		vmov.i8		d15, #0xa
		vmov.i8		d30, #4
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_86c

loc_74c:				@ code xref: .text:00000864j
					@ .text:00000980j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#2
		subs		r7, r7,	#4
		ble		loc_87c

loc_760:				@ code xref: .text:00000854j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #6
		vext.8		d6, d2,	d3, #6
		vsubl.u8	q2, d0,	d4
		vsubl.u8	q3, d2,	d6
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #2
		vext.8		d10, d2, d3, #1
		vext.8		d11, d2, d3, #2
		vmlsl.u8	q2, d8,	d12
		vmlal.u8	q2, d9,	d13
		vmlsl.u8	q3, d10, d12
		vmlal.u8	q3, d11, d13
		vext.8		d8, d0,	d1, #3
		vext.8		d9, d0,	d1, #4
		vext.8		d10, d2, d3, #3
		vext.8		d11, d2, d3, #4
		vmlal.u8	q2, d8,	d14
		vmlsl.u8	q2, d9,	d15
		vmlal.u8	q3, d10, d14
		vmlsl.u8	q3, d11, d15
		vext.8		d8, d0,	d1, #5
		vext.8		d9, d2,	d3, #5
		vmlal.u8	q2, d8,	d30
		vmlal.u8	q3, d9,	d30
		vext.8		d20, d16, d17, #6
		vext.8		d22, d18, d19, #6
		vsubl.u8	q10, d16, d20
		vsubl.u8	q11, d18, d22
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #2
		vext.8		d26, d18, d19, #1
		vext.8		d27, d18, d19, #2
		vmlsl.u8	q10, d24, d12
		vmlal.u8	q10, d25, d13
		vmlsl.u8	q11, d26, d12
		vmlal.u8	q11, d27, d13
		vext.8		d28, d16, d17, #3
		vext.8		d29, d16, d17, #4
		vext.8		d24, d18, d19, #3
		vext.8		d25, d18, d19, #4
		vmlal.u8	q10, d28, d14
		vmlsl.u8	q10, d29, d15
		vmlal.u8	q11, d24, d14
		vmlsl.u8	q11, d25, d15
		vext.8		d28, d16, d17, #5
		vext.8		d29, d18, d19, #5
		vmlal.u8	q10, d28, d30
		vmlal.u8	q11, d29, d30
		vqrshrun.s16	d0, q2,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d16, q10, #6
		vqrshrun.s16	d17, q11, #6
		vst1.64		{d0}, [r5,:64],r1
		vst1.64		{d1}, [r5,:64],r1
		vst1.64		{d16}, [r5,:64],r1
		vst1.64		{d17}, [r5,:64],r1
		bne		loc_760
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_74c
		b		locret_984
@ ---------------------------------------------------------------------------

loc_86c:				@ code xref: .text:00000748j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#2
		sub		r7, r7,	#4

loc_87c:				@ code xref: .text:0000075cj
					@ .text:00000970j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #6
		vext.8		d6, d2,	d3, #6
		vsubl.u8	q2, d0,	d4
		vsubl.u8	q3, d2,	d6
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #2
		vext.8		d10, d2, d3, #1
		vext.8		d11, d2, d3, #2
		vmlsl.u8	q2, d8,	d12
		vmlal.u8	q2, d9,	d13
		vmlsl.u8	q3, d10, d12
		vmlal.u8	q3, d11, d13
		vext.8		d8, d0,	d1, #3
		vext.8		d9, d0,	d1, #4
		vext.8		d10, d2, d3, #3
		vext.8		d11, d2, d3, #4
		vmlal.u8	q2, d8,	d14
		vmlsl.u8	q2, d9,	d15
		vmlal.u8	q3, d10, d14
		vmlsl.u8	q3, d11, d15
		vext.8		d8, d0,	d1, #5
		vext.8		d9, d2,	d3, #5
		vmlal.u8	q2, d8,	d30
		vmlal.u8	q3, d9,	d30
		vext.8		d20, d16, d17, #6
		vext.8		d22, d18, d19, #6
		vsubl.u8	q10, d16, d20
		vsubl.u8	q11, d18, d22
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #2
		vext.8		d26, d18, d19, #1
		vext.8		d27, d18, d19, #2
		vmlsl.u8	q10, d24, d12
		vmlal.u8	q10, d25, d13
		vmlsl.u8	q11, d26, d12
		vmlal.u8	q11, d27, d13
		vext.8		d28, d16, d17, #3
		vext.8		d29, d16, d17, #4
		vext.8		d24, d18, d19, #3
		vext.8		d25, d18, d19, #4
		vmlal.u8	q10, d28, d14
		vmlsl.u8	q10, d29, d15
		vmlal.u8	q11, d24, d14
		vmlsl.u8	q11, d25, d15
		vext.8		d28, d16, d17, #5
		vext.8		d29, d18, d19, #5
		vmlal.u8	q10, d28, d30
		vmlal.u8	q11, d29, d30
		vqrshrun.s16	d0, q2,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d16, q10, #6
		vqrshrun.s16	d17, q11, #6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d16[0]}, [r5,:32],r1
		vst1.32		{d17[0]}, [r5,:32],r1
		bne		loc_87c
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_74c

locret_984:				@ code xref: .text:00000868j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_put_MC_InterLuma_H0V1_neon
_VO_put_MC_InterLuma_H0V1_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d16, #0xa
		vmov.i8		d17, #0x3a @ ':'
		vmov.i8		d18, #0x11
		vmov.i8		d19, #5
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_aa8

loc_9ac:				@ code xref: .text:00000aa0j
					@ .text:00000b98j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		subs		r7, r7,	#4
		ble		loc_abc
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3

loc_9dc:				@ code xref: .text:00000a90j
		vld1.64		{d26}, [r4],r3
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		subs		r8, r8,	#4
		vsubl.u8	q0, d26, d20
		vshll.u8	q1, d21, #2
		vsubl.u8	q2, d27, d21
		vshll.u8	q3, d22, #2
		vsubl.u8	q4, d28, d22
		vshll.u8	q5, d23, #2
		vsubl.u8	q6, d29, d23
		vshll.u8	q7, d24, #2
		vadd.i16	q1, q1,	q0
		vadd.i16	q3, q3,	q2
		vadd.i16	q5, q5,	q4
		vadd.i16	q7, q7,	q6
		vmlsl.u8	q1, d22, d16
		vmlsl.u8	q3, d23, d16
		vmlsl.u8	q5, d24, d16
		vmlsl.u8	q7, d25, d16
		vmlal.u8	q1, d23, d17
		vmlal.u8	q3, d24, d17
		vmlal.u8	q5, d25, d17
		vmlal.u8	q7, d26, d17
		vmlal.u8	q1, d24, d18
		vmlal.u8	q3, d25, d18
		vmlal.u8	q5, d26, d18
		vmlal.u8	q7, d27, d18
		vmlsl.u8	q1, d25, d19
		vmlsl.u8	q3, d26, d19
		vmlsl.u8	q5, d27, d19
		vmlsl.u8	q7, d28, d19
		vqrshrun.s16	d0, q1,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q5,	#6
		vqrshrun.s16	d3, q7,	#6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		ble		loc_a94
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		b		loc_9dc
@ ---------------------------------------------------------------------------

loc_a94:				@ code xref: .text:00000a80j
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_9ac
		b		locret_b9c
@ ---------------------------------------------------------------------------

loc_aa8:				@ code xref: .text:000009a8j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r7, r7,	#4

loc_abc:				@ code xref: .text:000009c0j
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3

loc_ad4:				@ code xref: .text:00000b88j
		vld1.64		{d26}, [r4],r3
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		subs		r8, r8,	#4
		vsubl.u8	q0, d26, d20
		vshll.u8	q1, d21, #2
		vsubl.u8	q2, d27, d21
		vshll.u8	q3, d22, #2
		vsubl.u8	q4, d28, d22
		vshll.u8	q5, d23, #2
		vsubl.u8	q6, d29, d23
		vshll.u8	q7, d24, #2
		vadd.i16	q1, q1,	q0
		vadd.i16	q3, q3,	q2
		vadd.i16	q5, q5,	q4
		vadd.i16	q7, q7,	q6
		vmlsl.u8	q1, d22, d16
		vmlsl.u8	q3, d23, d16
		vmlsl.u8	q5, d24, d16
		vmlsl.u8	q7, d25, d16
		vmlal.u8	q1, d23, d17
		vmlal.u8	q3, d24, d17
		vmlal.u8	q5, d25, d17
		vmlal.u8	q7, d26, d17
		vmlal.u8	q1, d24, d18
		vmlal.u8	q3, d25, d18
		vmlal.u8	q5, d26, d18
		vmlal.u8	q7, d27, d18
		vmlsl.u8	q1, d25, d19
		vmlsl.u8	q3, d26, d19
		vmlsl.u8	q5, d27, d19
		vmlsl.u8	q7, d28, d19
		vqrshrun.s16	d0, q1,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q5,	#6
		vqrshrun.s16	d3, q7,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		ble		loc_b8c
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		b		loc_ad4
@ ---------------------------------------------------------------------------

loc_b8c:				@ code xref: .text:00000b78j
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_9ac

locret_b9c:				@ code xref: .text:00000aa4j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_put_MC_InterLuma_H0V2_neon
_VO_put_MC_InterLuma_H0V2_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i16	q8, #0xb
		vmov.i16	q9, #0x28 @ '('
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_cd0

loc_bbc:				@ code xref: .text:00000cc8j
					@ .text:00000dd8j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		subs		r7, r7,	#4
		ble		loc_ce4
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3
		vld1.64		{d26}, [r4],r3

loc_bf0:				@ code xref: .text:00000cb8j
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		vld1.64		{d30}, [r4],r3
		subs		r8, r8,	#4
		vaddl.u8	q0, d20, d27
		vaddl.u8	q1, d21, d26
		vaddl.u8	q2, d21, d28
		vaddl.u8	q3, d22, d27
		vaddl.u8	q4, d22, d29
		vaddl.u8	q5, d23, d28
		vaddl.u8	q6, d23, d30
		vaddl.u8	q7, d24, d29
		vshl.i16	q1, q1,	#2
		vshl.i16	q3, q3,	#2
		vshl.i16	q5, q5,	#2
		vshl.i16	q7, q7,	#2
		vsub.i16	q1, q1,	q0
		vsub.i16	q3, q3,	q2
		vsub.i16	q5, q5,	q4
		vsub.i16	q7, q7,	q6
		vaddl.u8	q0, d22, d25
		vaddl.u8	q2, d23, d26
		vaddl.u8	q4, d24, d27
		vaddl.u8	q6, d25, d28
		vmls.i16	q1, q0,	q8
		vmls.i16	q3, q2,	q8
		vmls.i16	q5, q4,	q8
		vmls.i16	q7, q6,	q8
		vaddl.u8	q0, d23, d24
		vaddl.u8	q2, d24, d25
		vaddl.u8	q4, d25, d26
		vaddl.u8	q6, d26, d27
		vmla.i16	q1, q0,	q9
		vmla.i16	q3, q2,	q9
		vmla.i16	q5, q4,	q9
		vmla.i16	q7, q6,	q9
		vqrshrun.s16	d0, q1,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q5,	#6
		vqrshrun.s16	d3, q7,	#6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		ble		loc_cbc
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		d26, d30
		b		loc_bf0
@ ---------------------------------------------------------------------------

loc_cbc:				@ code xref: .text:00000ca4j
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_bbc
		b		locret_ddc
@ ---------------------------------------------------------------------------

loc_cd0:				@ code xref: .text:00000bb8j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r7, r7,	#4

loc_ce4:				@ code xref: .text:00000bd0j
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3
		vld1.64		{d26}, [r4],r3

loc_d00:				@ code xref: .text:00000dc8j
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		vld1.64		{d30}, [r4],r3
		subs		r8, r8,	#4
		vaddl.u8	q0, d20, d27
		vaddl.u8	q1, d21, d26
		vaddl.u8	q2, d21, d28
		vaddl.u8	q3, d22, d27
		vaddl.u8	q4, d22, d29
		vaddl.u8	q5, d23, d28
		vaddl.u8	q6, d23, d30
		vaddl.u8	q7, d24, d29
		vshl.i16	q1, q1,	#2
		vshl.i16	q3, q3,	#2
		vshl.i16	q5, q5,	#2
		vshl.i16	q7, q7,	#2
		vsub.i16	q1, q1,	q0
		vsub.i16	q3, q3,	q2
		vsub.i16	q5, q5,	q4
		vsub.i16	q7, q7,	q6
		vaddl.u8	q0, d22, d25
		vaddl.u8	q2, d23, d26
		vaddl.u8	q4, d24, d27
		vaddl.u8	q6, d25, d28
		vmls.i16	q1, q0,	q8
		vmls.i16	q3, q2,	q8
		vmls.i16	q5, q4,	q8
		vmls.i16	q7, q6,	q8
		vaddl.u8	q0, d23, d24
		vaddl.u8	q2, d24, d25
		vaddl.u8	q4, d25, d26
		vaddl.u8	q6, d26, d27
		vmla.i16	q1, q0,	q9
		vmla.i16	q3, q2,	q9
		vmla.i16	q5, q4,	q9
		vmla.i16	q7, q6,	q9
		vqrshrun.s16	d0, q1,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q5,	#6
		vqrshrun.s16	d3, q7,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		ble		loc_dcc
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		d26, d30
		b		loc_d00
@ ---------------------------------------------------------------------------

loc_dcc:				@ code xref: .text:00000db4j
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_bbc

locret_ddc:				@ code xref: .text:00000cccj
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_put_MC_InterLuma_H0V3_neon
_VO_put_MC_InterLuma_H0V3_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d16, #5
		vmov.i8		d17, #0x11
		vmov.i8		d18, #0x3a @ ':'
		vmov.i8		d19, #0xa
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_efc

loc_e04:				@ code xref: .text:00000ef4j
					@ .text:00000fe8j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		subs		r7, r7,	#4
		ble		loc_f0c
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3

loc_e30:				@ code xref: .text:00000ee4j
		vld1.64		{d26}, [r4],r3
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		subs		r8, r8,	#4
		vsubl.u8	q0, d20, d26
		vshll.u8	q1, d25, #2
		vsubl.u8	q2, d21, d27
		vshll.u8	q3, d26, #2
		vsubl.u8	q4, d22, d28
		vshll.u8	q5, d27, #2
		vsubl.u8	q6, d23, d29
		vshll.u8	q7, d28, #2
		vadd.i16	q1, q1,	q0
		vadd.i16	q3, q3,	q2
		vadd.i16	q5, q5,	q4
		vadd.i16	q7, q7,	q6
		vmlsl.u8	q1, d21, d16
		vmlsl.u8	q3, d22, d16
		vmlsl.u8	q5, d23, d16
		vmlsl.u8	q7, d24, d16
		vmlal.u8	q1, d22, d17
		vmlal.u8	q3, d23, d17
		vmlal.u8	q5, d24, d17
		vmlal.u8	q7, d25, d17
		vmlal.u8	q1, d23, d18
		vmlal.u8	q3, d24, d18
		vmlal.u8	q5, d25, d18
		vmlal.u8	q7, d26, d18
		vmlsl.u8	q1, d24, d19
		vmlsl.u8	q3, d25, d19
		vmlsl.u8	q5, d26, d19
		vmlsl.u8	q7, d27, d19
		vqrshrun.s16	d0, q1,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q5,	#6
		vqrshrun.s16	d3, q7,	#6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		ble		loc_ee8
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		b		loc_e30
@ ---------------------------------------------------------------------------

loc_ee8:				@ code xref: .text:00000ed4j
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_e04
		b		locret_fec
@ ---------------------------------------------------------------------------

loc_efc:				@ code xref: .text:00000e00j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r7, r7,	#4

loc_f0c:				@ code xref: .text:00000e14j
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3

loc_f24:				@ code xref: .text:00000fd8j
		vld1.64		{d26}, [r4],r3
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		subs		r8, r8,	#4
		vsubl.u8	q0, d20, d26
		vshll.u8	q1, d25, #2
		vsubl.u8	q2, d21, d27
		vshll.u8	q3, d26, #2
		vsubl.u8	q4, d22, d28
		vshll.u8	q5, d27, #2
		vsubl.u8	q6, d23, d29
		vshll.u8	q7, d28, #2
		vadd.i16	q1, q1,	q0
		vadd.i16	q3, q3,	q2
		vadd.i16	q5, q5,	q4
		vadd.i16	q7, q7,	q6
		vmlsl.u8	q1, d21, d16
		vmlsl.u8	q3, d22, d16
		vmlsl.u8	q5, d23, d16
		vmlsl.u8	q7, d24, d16
		vmlal.u8	q1, d22, d17
		vmlal.u8	q3, d23, d17
		vmlal.u8	q5, d24, d17
		vmlal.u8	q7, d25, d17
		vmlal.u8	q1, d23, d18
		vmlal.u8	q3, d24, d18
		vmlal.u8	q5, d25, d18
		vmlal.u8	q7, d26, d18
		vmlsl.u8	q1, d24, d19
		vmlsl.u8	q3, d25, d19
		vmlsl.u8	q5, d26, d19
		vmlsl.u8	q7, d27, d19
		vqrshrun.s16	d0, q1,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q5,	#6
		vqrshrun.s16	d3, q7,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		ble		loc_fdc
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		b		loc_f24
@ ---------------------------------------------------------------------------

loc_fdc:				@ code xref: .text:00000fc8j
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_e04

locret_fec:				@ code xref: .text:00000ef8j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_put_MC_InterLuma_H1V1_neon
_VO_put_MC_InterLuma_H1V1_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_12c8

loc_1034:				@ code xref: .text:000012c0j
					@ .text:000015d0j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_12e0
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7

loc_11a0:				@ code xref: .text:000012b0j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vsubl.s16	q4, d28, d16
		vsubl.s16	q5, d29, d17
		vsubl.s16	q6, d30, d18
		vsubl.s16	q7, d31, d19
		vmlal.s16	q4, d18, d6[0]
		vmlal.s16	q5, d19, d6[0]
		vmlal.s16	q6, d20, d6[0]
		vmlal.s16	q7, d21, d6[0]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlal.s16	q4, d24, d6[3]
		vmlal.s16	q5, d25, d6[3]
		vmlal.s16	q6, d26, d6[3]
		vmlal.s16	q7, d27, d6[3]
		vmlsl.s16	q4, d26, d6[1]
		vmlsl.s16	q5, d27, d6[1]
		vmlsl.s16	q6, d28, d6[1]
		vmlsl.s16	q7, d29, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqmovn.u16	d8, q4
		vqrshrun.s32	d12, q6, #0xc
		vqrshrun.s32	d13, q7, #0xc
		vqmovn.u16	d9, q6
		vst1.8		{d8}, [r5,:64],r1
		vst1.8		{d9}, [r5,:64],r1
		ble		loc_12b4
		b		loc_11a0
@ ---------------------------------------------------------------------------

loc_12b4:				@ code xref: .text:000012acj
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_1034
		b		locret_15d4
@ ---------------------------------------------------------------------------

loc_12c8:				@ code xref: .text:00001030j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_12e0:				@ code xref: .text:0000104cj
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22

loc_143c:				@ code xref: .text:000015c0j
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q11, d8, #2
		vshll.u8	q12, d10, #2
		vadd.i16	q11, q11, q6
		vadd.i16	q12, q12, q7
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vmov		d25, d26
		vsubl.s16	q4, d22, d16
		vsubl.s16	q5, d23, d17
		vsubl.s16	q6, d24, d18
		vsubl.s16	q7, d25, d19
		vmlal.s16	q4, d17, d6[0]
		vmlal.s16	q5, d18, d6[0]
		vmlal.s16	q6, d19, d6[0]
		vmlal.s16	q7, d20, d6[0]
		vmlsl.s16	q4, d18, d7[0]
		vmlsl.s16	q5, d19, d7[0]
		vmlsl.s16	q6, d20, d7[0]
		vmlsl.s16	q7, d21, d7[0]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlsl.s16	q4, d21, d6[1]
		vmlsl.s16	q5, d22, d6[1]
		vmlsl.s16	q6, d23, d6[1]
		vmlsl.s16	q7, d24, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqmovn.u16	d8, q4
		vqrshrun.s32	d12, q6, #0xc
		vqrshrun.s32	d13, q7, #0xc
		vqmovn.u16	d9, q6
		vst1.32		{d8[0]}, [r5,:32],r1
		vst1.32		{d8[1]}, [r5,:32],r1
		vst1.32		{d9[0]}, [r5,:32],r1
		vst1.32		{d9[1]}, [r5,:32],r1
		ble		loc_15c4
		b		loc_143c
@ ---------------------------------------------------------------------------

loc_15c4:				@ code xref: .text:000015bcj
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_1034

locret_15d4:				@ code xref: .text:000012c4j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_put_MC_InterLuma_H1V2_neon
_VO_put_MC_InterLuma_H1V2_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_1884

loc_161c:				@ code xref: .text:0000187cj
					@ .text:00001bf4j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_189c
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vext.8		d8, d28, d29, #6
		vsubl.u8	q6, d8,	d28
		vext.8		d8, d28, d29, #2
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d28, d29, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d28, d29, #4
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d28, d29, #5
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d28, d29, #1
		vshll.u8	q14, d8, #2
		vadd.i16	q14, q14, q6

loc_17c0:				@ code xref: .text:0000186cj
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d30, d31, #6
		vsubl.u8	q6, d8,	d30
		vext.8		d8, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d30, d31, #1
		vshll.u8	q15, d8, #2
		vadd.i16	q15, q15, q6
		subs		r8, r8,	#1
		vaddl.s16	q4, d18, d28
		vaddl.s16	q5, d19, d29
		vaddl.s16	q6, d22, d24
		vaddl.s16	q7, d23, d25
		vmla.i32	q4, q6,	d7[0]
		vmla.i32	q5, q7,	d7[0]
		vaddl.s16	q6, d16, d30
		vaddl.s16	q7, d17, d31
		vaddl.s16	q2, d20, d26
		vaddl.s16	q8, d21, d27
		vmla.i32	q6, q2,	d7[1]
		vmla.i32	q7, q8,	d7[1]
		vshl.i32	q4, q4,	#2
		vshl.i32	q5, q5,	#2
		vmov		q8, q9
		vmov		q9, q10
		vmov		q10, q11
		vsub.i32	q4, q4,	q6
		vsub.i32	q5, q5,	q7
		vmov		q11, q12
		vmov		q12, q13
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vmov		q13, q14
		vmov		q14, q15
		vqmovn.u16	d8, q4
		vst1.8		{d8}, [r5,:64],r1
		ble		loc_1870
		b		loc_17c0
@ ---------------------------------------------------------------------------

loc_1870:				@ code xref: .text:00001868j
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_161c
		b		locret_1bf8
@ ---------------------------------------------------------------------------

loc_1884:				@ code xref: .text:00001618j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_189c:				@ code xref: .text:00001634j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22
		vext.8		d8, d28, d29, #6
		vsubl.u8	q6, d8,	d28
		vext.8		d8, d28, d29, #2
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d28, d29, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d28, d29, #4
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d28, d29, #5
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d28, d29, #1
		vshll.u8	q14, d8, #2
		vadd.i16	q14, q14, q6
		vmov		d22, d28

loc_1a34:				@ code xref: .text:00001be4j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vmov		d23, d28
		vmov		d24, d30
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vmov		d25, d28
		vmov		d26, d30
		subs		r8, r8,	#4
		vaddl.s16	q4, d16, d23
		vaddl.s16	q5, d18, d21
		vmla.i32	q4, q5,	d7[1]
		vaddl.s16	q5, d17, d22
		vaddl.s16	q6, d19, d20
		vmla.i32	q5, q6,	d7[0]
		vshl.i32	q5, q5,	#2
		vsub.i32	q4, q5,	q4
		vaddl.s16	q5, d17, d24
		vaddl.s16	q6, d19, d22
		vmla.i32	q5, q6,	d7[1]
		vaddl.s16	q6, d18, d23
		vaddl.s16	q7, d20, d21
		vmla.i32	q6, q7,	d7[0]
		vshl.i32	q6, q6,	#2
		vsub.i32	q5, q6,	q5
		vaddl.s16	q6, d18, d25
		vaddl.s16	q7, d20, d23
		vmla.i32	q6, q7,	d7[1]
		vaddl.s16	q7, d19, d24
		vaddl.s16	q8, d21, d22
		vmla.i32	q7, q8,	d7[0]
		vshl.i32	q7, q7,	#2
		vsub.i32	q6, q7,	q6
		vaddl.s16	q7, d19, d26
		vaddl.s16	q8, d21, d24
		vmla.i32	q7, q8,	d7[1]
		vaddl.s16	q8, d20, d25
		vaddl.s16	q15, d22, d23
		vmla.i32	q8, q15, d7[0]
		vshl.i32	q8, q8,	#2
		vsub.i32	q7, q8,	q7
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqrshrun.s32	d10, q6, #0xc
		vqrshrun.s32	d11, q7, #0xc
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		d22, d26
		vqmovn.u16	d8, q4
		vqmovn.u16	d9, q5
		vst1.32		{d8[0]}, [r5,:32],r1
		vst1.32		{d8[1]}, [r5,:32],r1
		vst1.32		{d9[0]}, [r5,:32],r1
		vst1.32		{d9[1]}, [r5,:32],r1
		ble		loc_1be8
		b		loc_1a34
@ ---------------------------------------------------------------------------

loc_1be8:				@ code xref: .text:00001be0j
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_161c

locret_1bf8:				@ code xref: .text:00001880j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_put_MC_InterLuma_H1V3_neon
_VO_put_MC_InterLuma_H1V3_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_1ed0

loc_1c40:				@ code xref: .text:00001ec8j
					@ .text:000021d4j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_1ee4
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7

loc_1da8:				@ code xref: .text:00001eb8j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vsubl.s16	q4, d16, d28
		vsubl.s16	q5, d17, d29
		vsubl.s16	q6, d18, d30
		vsubl.s16	q7, d19, d31
		vmlsl.s16	q4, d18, d6[1]
		vmlsl.s16	q5, d19, d6[1]
		vmlsl.s16	q6, d20, d6[1]
		vmlsl.s16	q7, d21, d6[1]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlsl.s16	q4, d24, d7[0]
		vmlsl.s16	q5, d25, d7[0]
		vmlsl.s16	q6, d26, d7[0]
		vmlsl.s16	q7, d27, d7[0]
		vmlal.s16	q4, d26, d6[0]
		vmlal.s16	q5, d27, d6[0]
		vmlal.s16	q6, d28, d6[0]
		vmlal.s16	q7, d29, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqmovn.u16	d8, q4
		vqrshrun.s32	d12, q6, #0xc
		vqrshrun.s32	d13, q7, #0xc
		vqmovn.u16	d9, q6
		vst1.8		{d8}, [r5,:64],r1
		vst1.8		{d9}, [r5,:64],r1
		ble		loc_1ebc
		b		loc_1da8
@ ---------------------------------------------------------------------------

loc_1ebc:				@ code xref: .text:00001eb4j
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_1c40
		b		locret_21d8
@ ---------------------------------------------------------------------------

loc_1ed0:				@ code xref: .text:00001c3cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_1ee4:				@ code xref: .text:00001c54j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22

loc_2040:				@ code xref: .text:000021c4j
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q11, d8, #2
		vshll.u8	q12, d10, #2
		vadd.i16	q11, q11, q6
		vadd.i16	q12, q12, q7
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vmov		d25, d26
		vsubl.s16	q4, d16, d22
		vsubl.s16	q5, d17, d23
		vsubl.s16	q6, d18, d24
		vsubl.s16	q7, d19, d25
		vmlsl.s16	q4, d17, d6[1]
		vmlsl.s16	q5, d18, d6[1]
		vmlsl.s16	q6, d19, d6[1]
		vmlsl.s16	q7, d20, d6[1]
		vmlal.s16	q4, d18, d6[3]
		vmlal.s16	q5, d19, d6[3]
		vmlal.s16	q6, d20, d6[3]
		vmlal.s16	q7, d21, d6[3]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d21, d6[0]
		vmlal.s16	q5, d22, d6[0]
		vmlal.s16	q6, d23, d6[0]
		vmlal.s16	q7, d24, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqmovn.u16	d8, q4
		vqrshrun.s32	d12, q6, #0xc
		vqrshrun.s32	d13, q7, #0xc
		vqmovn.u16	d9, q6
		vst1.32		{d8[0]}, [r5,:32],r1
		vst1.32		{d8[1]}, [r5,:32],r1
		vst1.32		{d9[0]}, [r5,:32],r1
		vst1.32		{d9[1]}, [r5,:32],r1
		ble		loc_21c8
		b		loc_2040
@ ---------------------------------------------------------------------------

loc_21c8:				@ code xref: .text:000021c0j
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_1c40

locret_21d8:				@ code xref: .text:00001eccj
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_put_MC_InterLuma_H2V1_neon
_VO_put_MC_InterLuma_H2V1_neon:
		stmfd		sp!, {r4-r10,lr}
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_24e4

loc_2210:				@ code xref: .text:000024dcj
					@ .text:0000283cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_24fc
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q10, d20, d8
		vaddl.u8	q11, d22, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q12, d24, d8
		vaddl.u8	q13, d26, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13

loc_23ac:				@ code xref: .text:000024ccj
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q14, d28, d8
		vaddl.u8	q15, d30, d9
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q15, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q14, q1, q14
		vsub.i16	q15, q6, q15
		vsubl.s16	q4, d28, d16
		vsubl.s16	q5, d29, d17
		vsubl.s16	q6, d30, d18
		vsubl.s16	q7, d31, d19
		vmlal.s16	q4, d18, d6[0]
		vmlal.s16	q5, d19, d6[0]
		vmlal.s16	q6, d20, d6[0]
		vmlal.s16	q7, d21, d6[0]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlal.s16	q4, d24, d6[3]
		vmlal.s16	q5, d25, d6[3]
		vmlal.s16	q6, d26, d6[3]
		vmlal.s16	q7, d27, d6[3]
		vmlsl.s16	q4, d26, d6[1]
		vmlsl.s16	q5, d27, d6[1]
		vmlsl.s16	q6, d28, d6[1]
		vmlsl.s16	q7, d29, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqmovn.u16	d0, q4
		vqrshrun.s32	d12, q6, #0xc
		vqrshrun.s32	d13, q7, #0xc
		vqmovn.u16	d1, q6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		ble		loc_24d0
		b		loc_23ac
@ ---------------------------------------------------------------------------

loc_24d0:				@ code xref: .text:000024c8j
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_2210
		b		locret_2840
@ ---------------------------------------------------------------------------

loc_24e4:				@ code xref: .text:0000220cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_24fc:				@ code xref: .text:00002228j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vmov		d17, d18
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q9, d20, d8
		vaddl.u8	q10, d22, d9
		vmla.i16	q9, q2,	d7[2]
		vmla.i16	q10, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q9, q1,	q9
		vsub.i16	q10, q6, q10
		vmov		d19, d20
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q10, d24, d8
		vaddl.u8	q11, d26, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vmov		d21, d22

loc_2688:				@ code xref: .text:0000282cj
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q11, d24, d8
		vaddl.u8	q12, d26, d9
		vmla.i16	q11, q2, d7[2]
		vmla.i16	q12, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q11, q1, q11
		vsub.i16	q12, q6, q12
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q12, d28, d8
		vaddl.u8	q13, d30, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13
		vmov		d25, d26
		vsubl.s16	q4, d22, d16
		vsubl.s16	q5, d23, d17
		vsubl.s16	q6, d24, d18
		vsubl.s16	q7, d25, d19
		vmlal.s16	q4, d17, d6[0]
		vmlal.s16	q5, d18, d6[0]
		vmlal.s16	q6, d19, d6[0]
		vmlal.s16	q7, d20, d6[0]
		vmlsl.s16	q4, d18, d7[0]
		vmlsl.s16	q5, d19, d7[0]
		vmlsl.s16	q6, d20, d7[0]
		vmlsl.s16	q7, d21, d7[0]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlsl.s16	q4, d21, d6[1]
		vmlsl.s16	q5, d22, d6[1]
		vmlsl.s16	q6, d23, d6[1]
		vmlsl.s16	q7, d24, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqmovn.u16	d0, q4
		vqrshrun.s32	d12, q6, #0xc
		vqrshrun.s32	d13, q7, #0xc
		vqmovn.u16	d1, q6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d0[1]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d1[1]}, [r5,:32],r1
		ble		loc_2830
		b		loc_2688
@ ---------------------------------------------------------------------------

loc_2830:				@ code xref: .text:00002828j
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_2210

locret_2840:				@ code xref: .text:000024e0j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_put_MC_InterLuma_H2V2_neon
_VO_put_MC_InterLuma_H2V2_neon:
		stmfd		sp!, {r4-r10,lr}
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_2b20

loc_2878:				@ code xref: .text:00002b18j
					@ .text:00002eecj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_2b38
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q10, d20, d8
		vaddl.u8	q11, d22, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q12, d24, d8
		vaddl.u8	q13, d26, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vaddl.u8	q0, d0,	d1
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vaddl.u8	q1, d2,	d3
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vaddl.u8	q2, d4,	d5
		vext.8		d8, d28, d29, #7
		vaddl.u8	q14, d28, d8
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q1, q0,	d7[0]
		vshl.i16	q1, q1,	#2
		vsub.i16	q14, q1, q14

loc_2a54:				@ code xref: .text:00002b08j
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d0, d30, d31, #3
		vext.8		d1, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vext.8		d2, d30, d31, #1
		vext.8		d3, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vext.8		d4, d30, d31, #2
		vext.8		d5, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vext.8		d8, d30, d31, #7
		vaddl.u8	q15, d30, d8
		vmla.i16	q15, q2, d7[2]
		vmla.i16	q1, q0,	d7[0]
		vshl.i16	q1, q1,	#2
		vsub.i16	q15, q1, q15
		subs		r8, r8,	#1
		vaddl.s16	q4, d18, d28
		vaddl.s16	q5, d19, d29
		vaddl.s16	q6, d16, d30
		vaddl.s16	q7, d17, d31
		vaddl.s16	q0, d22, d24
		vaddl.s16	q1, d23, d25
		vaddl.s16	q2, d20, d26
		vaddl.s16	q8, d21, d27
		vmla.i32	q4, q0,	d7[0]
		vmla.i32	q5, q1,	d7[0]
		vmla.i32	q6, q2,	d7[1]
		vmla.i32	q7, q8,	d7[1]
		vshl.i32	q4, q4,	#2
		vshl.i32	q5, q5,	#2
		vmov		q8, q9
		vmov		q9, q10
		vmov		q10, q11
		vsub.i32	q4, q4,	q6
		vsub.i32	q5, q5,	q7
		vmov		q11, q12
		vmov		q12, q13
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vmov		q13, q14
		vmov		q14, q15
		vqmovn.u16	d0, q4
		vst1.8		{d0}, [r5,:64],r1
		ble		loc_2b0c
		b		loc_2a54
@ ---------------------------------------------------------------------------

loc_2b0c:				@ code xref: .text:00002b04j
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_2878
		b		locret_2ef0
@ ---------------------------------------------------------------------------

loc_2b20:				@ code xref: .text:00002874j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_2b38:				@ code xref: .text:00002890j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vmov		d17, d18
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q9, d20, d8
		vaddl.u8	q10, d22, d9
		vmla.i16	q9, q2,	d7[2]
		vmla.i16	q10, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q9, q1,	q9
		vsub.i16	q10, q6, q10
		vmov		d19, d20
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q10, d24, d8
		vaddl.u8	q11, d26, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vmov		d21, d22
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vaddl.u8	q0, d0,	d1
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vaddl.u8	q1, d2,	d3
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vaddl.u8	q2, d4,	d5
		vext.8		d8, d28, d29, #7
		vaddl.u8	q14, d28, d8
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q1, q0,	d7[0]
		vshl.i16	q1, q1,	#2
		vsub.i16	q14, q1, q14
		vmov		d22, d28

loc_2d08:				@ code xref: .text:00002edcj
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q14, d28, d8
		vaddl.u8	q15, d30, d9
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q15, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q14, q1, q14
		vsub.i16	q15, q6, q15
		vmov		d23, d28
		vmov		d24, d30
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q14, d28, d8
		vaddl.u8	q15, d30, d9
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q15, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q14, q1, q14
		vsub.i16	q15, q6, q15
		vmov		d25, d28
		vmov		d26, d30
		subs		r8, r8,	#4
		vmov		r9, r10, d26
		vaddl.s16	q0, d16, d23
		vaddl.s16	q1, d17, d24
		vaddl.s16	q2, d18, d25
		vaddl.s16	q4, d19, d26
		vaddl.s16	q5, d18, d21
		vaddl.s16	q6, d19, d22
		vaddl.s16	q7, d20, d23
		vaddl.s16	q14, d21, d24
		vmla.i32	q0, q5,	d7[1]
		vmla.i32	q1, q6,	d7[1]
		vmla.i32	q2, q7,	d7[1]
		vmla.i32	q4, q14, d7[1]
		vaddl.s16	q5, d17, d22
		vaddl.s16	q6, d18, d23
		vaddl.s16	q7, d19, d24
		vaddl.s16	q14, d20, d25
		vaddl.s16	q15, d19, d20
		vaddl.s16	q8, d20, d21
		vaddl.s16	q9, d21, d22
		vaddl.s16	q13, d22, d23
		vmla.i32	q5, q15, d7[0]
		vmla.i32	q6, q8,	d7[0]
		vmla.i32	q7, q9,	d7[0]
		vmla.i32	q14, q13, d7[0]
		vshl.i32	q5, q5,	#2
		vshl.i32	q6, q6,	#2
		vshl.i32	q7, q7,	#2
		vshl.i32	q14, q14, #2
		vsub.i32	q5, q5,	q0
		vsub.i32	q6, q6,	q1
		vsub.i32	q7, q7,	q2
		vsub.i32	q14, q14, q4
		vqrshrun.s32	d8, q5,	#0xc
		vqrshrun.s32	d9, q6,	#0xc
		vqrshrun.s32	d10, q7, #0xc
		vqrshrun.s32	d11, q14, #0xc
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		d22, r9, r10
		vqmovn.u16	d0, q4
		vqmovn.u16	d1, q5
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d0[1]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d1[1]}, [r5,:32],r1
		ble		loc_2ee0
		b		loc_2d08
@ ---------------------------------------------------------------------------

loc_2ee0:				@ code xref: .text:00002ed8j
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_2878

locret_2ef0:				@ code xref: .text:00002b1cj
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_put_MC_InterLuma_H2V3_neon
_VO_put_MC_InterLuma_H2V3_neon:
		stmfd		sp!, {r4-r10,lr}
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_31f8

loc_2f28:				@ code xref: .text:000031f0j
					@ .text:0000354cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_320c
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q10, d20, d8
		vaddl.u8	q11, d22, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q12, d24, d8
		vaddl.u8	q13, d26, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13

loc_30c0:				@ code xref: .text:000031e0j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q14, d28, d8
		vaddl.u8	q15, d30, d9
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q15, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q14, q1, q14
		vsub.i16	q15, q6, q15
		vsubl.s16	q4, d16, d28
		vsubl.s16	q5, d17, d29
		vsubl.s16	q6, d18, d30
		vsubl.s16	q7, d19, d31
		vmlsl.s16	q4, d18, d6[1]
		vmlsl.s16	q5, d19, d6[1]
		vmlsl.s16	q6, d20, d6[1]
		vmlsl.s16	q7, d21, d6[1]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlsl.s16	q4, d24, d7[0]
		vmlsl.s16	q5, d25, d7[0]
		vmlsl.s16	q6, d26, d7[0]
		vmlsl.s16	q7, d27, d7[0]
		vmlal.s16	q4, d26, d6[0]
		vmlal.s16	q5, d27, d6[0]
		vmlal.s16	q6, d28, d6[0]
		vmlal.s16	q7, d29, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqmovn.u16	d0, q4
		vqrshrun.s32	d12, q6, #0xc
		vqrshrun.s32	d13, q7, #0xc
		vqmovn.u16	d1, q6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		ble		loc_31e4
		b		loc_30c0
@ ---------------------------------------------------------------------------

loc_31e4:				@ code xref: .text:000031dcj
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_2f28
		b		locret_3550
@ ---------------------------------------------------------------------------

loc_31f8:				@ code xref: .text:00002f24j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_320c:				@ code xref: .text:00002f3cj
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vmov		d17, d18
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q9, d20, d8
		vaddl.u8	q10, d22, d9
		vmla.i16	q9, q2,	d7[2]
		vmla.i16	q10, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q9, q1,	q9
		vsub.i16	q10, q6, q10
		vmov		d19, d20
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q10, d24, d8
		vaddl.u8	q11, d26, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vmov		d21, d22

loc_3398:				@ code xref: .text:0000353cj
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q11, d24, d8
		vaddl.u8	q12, d26, d9
		vmla.i16	q11, q2, d7[2]
		vmla.i16	q12, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q11, q1, q11
		vsub.i16	q12, q6, q12
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q12, d28, d8
		vaddl.u8	q13, d30, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13
		vmov		d25, d26
		vsubl.s16	q4, d16, d22
		vsubl.s16	q5, d17, d23
		vsubl.s16	q6, d18, d24
		vsubl.s16	q7, d19, d25
		vmlsl.s16	q4, d17, d6[1]
		vmlsl.s16	q5, d18, d6[1]
		vmlsl.s16	q6, d19, d6[1]
		vmlsl.s16	q7, d20, d6[1]
		vmlal.s16	q4, d18, d6[3]
		vmlal.s16	q5, d19, d6[3]
		vmlal.s16	q6, d20, d6[3]
		vmlal.s16	q7, d21, d6[3]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d21, d6[0]
		vmlal.s16	q5, d22, d6[0]
		vmlal.s16	q6, d23, d6[0]
		vmlal.s16	q7, d24, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqmovn.u16	d0, q4
		vqrshrun.s32	d12, q6, #0xc
		vqrshrun.s32	d13, q7, #0xc
		vqmovn.u16	d1, q6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d0[1]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d1[1]}, [r5,:32],r1
		ble		loc_3540
		b		loc_3398
@ ---------------------------------------------------------------------------

loc_3540:				@ code xref: .text:00003538j
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_2f28

locret_3550:				@ code xref: .text:000031f4j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_put_MC_InterLuma_H3V1_neon
_VO_put_MC_InterLuma_H3V1_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_382c

loc_3598:				@ code xref: .text:00003824j
					@ .text:00003b34j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#2
		subs		r7, r7,	#4
		ble		loc_3844
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7

loc_3704:				@ code xref: .text:00003814j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vsubl.s16	q4, d28, d16
		vsubl.s16	q5, d29, d17
		vsubl.s16	q6, d30, d18
		vsubl.s16	q7, d31, d19
		vmlal.s16	q4, d18, d6[0]
		vmlal.s16	q5, d19, d6[0]
		vmlal.s16	q6, d20, d6[0]
		vmlal.s16	q7, d21, d6[0]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlal.s16	q4, d24, d6[3]
		vmlal.s16	q5, d25, d6[3]
		vmlal.s16	q6, d26, d6[3]
		vmlal.s16	q7, d27, d6[3]
		vmlsl.s16	q4, d26, d6[1]
		vmlsl.s16	q5, d27, d6[1]
		vmlsl.s16	q6, d28, d6[1]
		vmlsl.s16	q7, d29, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqmovn.u16	d8, q4
		vqrshrun.s32	d12, q6, #0xc
		vqrshrun.s32	d13, q7, #0xc
		vqmovn.u16	d9, q6
		vst1.8		{d8}, [r5,:64],r1
		vst1.8		{d9}, [r5,:64],r1
		ble		loc_3818
		b		loc_3704
@ ---------------------------------------------------------------------------

loc_3818:				@ code xref: .text:00003810j
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_3598
		b		locret_3b38
@ ---------------------------------------------------------------------------

loc_382c:				@ code xref: .text:00003594j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#2
		sub		r7, r7,	#4

loc_3844:				@ code xref: .text:000035b0j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22

loc_39a0:				@ code xref: .text:00003b24j
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q11, d8, #2
		vshll.u8	q12, d10, #2
		vadd.i16	q11, q11, q6
		vadd.i16	q12, q12, q7
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vmov		d25, d26
		vsubl.s16	q4, d22, d16
		vsubl.s16	q5, d23, d17
		vsubl.s16	q6, d24, d18
		vsubl.s16	q7, d25, d19
		vmlal.s16	q4, d17, d6[0]
		vmlal.s16	q5, d18, d6[0]
		vmlal.s16	q6, d19, d6[0]
		vmlal.s16	q7, d20, d6[0]
		vmlsl.s16	q4, d18, d7[0]
		vmlsl.s16	q5, d19, d7[0]
		vmlsl.s16	q6, d20, d7[0]
		vmlsl.s16	q7, d21, d7[0]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlsl.s16	q4, d21, d6[1]
		vmlsl.s16	q5, d22, d6[1]
		vmlsl.s16	q6, d23, d6[1]
		vmlsl.s16	q7, d24, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqmovn.u16	d8, q4
		vqrshrun.s32	d12, q6, #0xc
		vqrshrun.s32	d13, q7, #0xc
		vqmovn.u16	d9, q6
		vst1.32		{d8[0]}, [r5,:32],r1
		vst1.32		{d8[1]}, [r5,:32],r1
		vst1.32		{d9[0]}, [r5,:32],r1
		vst1.32		{d9[1]}, [r5,:32],r1
		ble		loc_3b28
		b		loc_39a0
@ ---------------------------------------------------------------------------

loc_3b28:				@ code xref: .text:00003b20j
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_3598

locret_3b38:				@ code xref: .text:00003828j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_put_MC_InterLuma_H3V2_neon
_VO_put_MC_InterLuma_H3V2_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_3de8

loc_3b80:				@ code xref: .text:00003de0j
					@ .text:00004158j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#2
		subs		r7, r7,	#4
		ble		loc_3e00
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vext.8		d8, d28, d29, #6
		vsubl.u8	q6, d28, d8
		vext.8		d8, d28, d29, #1
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d28, d29, #2
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d28, d29, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d28, d29, #4
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d28, d29, #5
		vshll.u8	q14, d8, #2
		vadd.i16	q14, q14, q6

loc_3d24:				@ code xref: .text:00003dd0j
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d30, d31, #6
		vsubl.u8	q6, d30, d8
		vext.8		d8, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d30, d31, #5
		vshll.u8	q15, d8, #2
		vadd.i16	q15, q15, q6
		subs		r8, r8,	#1
		vaddl.s16	q4, d18, d28
		vaddl.s16	q5, d19, d29
		vaddl.s16	q6, d22, d24
		vaddl.s16	q7, d23, d25
		vmla.i32	q4, q6,	d7[0]
		vmla.i32	q5, q7,	d7[0]
		vaddl.s16	q6, d16, d30
		vaddl.s16	q7, d17, d31
		vaddl.s16	q2, d20, d26
		vaddl.s16	q8, d21, d27
		vmla.i32	q6, q2,	d7[1]
		vmla.i32	q7, q8,	d7[1]
		vshl.i32	q4, q4,	#2
		vshl.i32	q5, q5,	#2
		vmov		q8, q9
		vmov		q9, q10
		vmov		q10, q11
		vsub.i32	q4, q4,	q6
		vsub.i32	q5, q5,	q7
		vmov		q11, q12
		vmov		q12, q13
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vmov		q13, q14
		vmov		q14, q15
		vqmovn.u16	d8, q4
		vst1.8		{d8}, [r5,:64],r1
		ble		loc_3dd4
		b		loc_3d24
@ ---------------------------------------------------------------------------

loc_3dd4:				@ code xref: .text:00003dccj
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_3b80
		b		locret_415c
@ ---------------------------------------------------------------------------

loc_3de8:				@ code xref: .text:00003b7cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#2
		sub		r7, r7,	#4

loc_3e00:				@ code xref: .text:00003b98j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22
		vext.8		d8, d28, d29, #6
		vsubl.u8	q6, d28, d8
		vext.8		d8, d28, d29, #1
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d28, d29, #2
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d28, d29, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d28, d29, #4
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d28, d29, #5
		vshll.u8	q14, d8, #2
		vadd.i16	q14, q14, q6
		vmov		d22, d28

loc_3f98:				@ code xref: .text:00004148j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vmov		d23, d28
		vmov		d24, d30
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vmov		d25, d28
		vmov		d26, d30
		subs		r8, r8,	#4
		vaddl.s16	q4, d16, d23
		vaddl.s16	q5, d18, d21
		vmla.i32	q4, q5,	d7[1]
		vaddl.s16	q5, d17, d22
		vaddl.s16	q6, d19, d20
		vmla.i32	q5, q6,	d7[0]
		vshl.i32	q5, q5,	#2
		vsub.i32	q4, q5,	q4
		vaddl.s16	q5, d17, d24
		vaddl.s16	q6, d19, d22
		vmla.i32	q5, q6,	d7[1]
		vaddl.s16	q6, d18, d23
		vaddl.s16	q7, d20, d21
		vmla.i32	q6, q7,	d7[0]
		vshl.i32	q6, q6,	#2
		vsub.i32	q5, q6,	q5
		vaddl.s16	q6, d18, d25
		vaddl.s16	q7, d20, d23
		vmla.i32	q6, q7,	d7[1]
		vaddl.s16	q7, d19, d24
		vaddl.s16	q8, d21, d22
		vmla.i32	q7, q8,	d7[0]
		vshl.i32	q7, q7,	#2
		vsub.i32	q6, q7,	q6
		vaddl.s16	q7, d19, d26
		vaddl.s16	q8, d21, d24
		vmla.i32	q7, q8,	d7[1]
		vaddl.s16	q8, d20, d25
		vaddl.s16	q15, d22, d23
		vmla.i32	q8, q15, d7[0]
		vshl.i32	q8, q8,	#2
		vsub.i32	q7, q8,	q7
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqrshrun.s32	d10, q6, #0xc
		vqrshrun.s32	d11, q7, #0xc
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		d22, d26
		vqmovn.u16	d8, q4
		vqmovn.u16	d9, q5
		vst1.32		{d8[0]}, [r5,:32],r1
		vst1.32		{d8[1]}, [r5,:32],r1
		vst1.32		{d9[0]}, [r5,:32],r1
		vst1.32		{d9[1]}, [r5,:32],r1
		ble		loc_414c
		b		loc_3f98
@ ---------------------------------------------------------------------------

loc_414c:				@ code xref: .text:00004144j
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_3b80

locret_415c:				@ code xref: .text:00003de4j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_put_MC_InterLuma_H3V3_neon
_VO_put_MC_InterLuma_H3V3_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_4434

loc_41a4:				@ code xref: .text:0000442cj
					@ .text:00004738j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#2
		subs		r7, r7,	#4
		ble		loc_4448
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7

loc_430c:				@ code xref: .text:0000441cj
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vsubl.s16	q4, d16, d28
		vsubl.s16	q5, d17, d29
		vsubl.s16	q6, d18, d30
		vsubl.s16	q7, d19, d31
		vmlsl.s16	q4, d18, d6[1]
		vmlsl.s16	q5, d19, d6[1]
		vmlsl.s16	q6, d20, d6[1]
		vmlsl.s16	q7, d21, d6[1]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlsl.s16	q4, d24, d7[0]
		vmlsl.s16	q5, d25, d7[0]
		vmlsl.s16	q6, d26, d7[0]
		vmlsl.s16	q7, d27, d7[0]
		vmlal.s16	q4, d26, d6[0]
		vmlal.s16	q5, d27, d6[0]
		vmlal.s16	q6, d28, d6[0]
		vmlal.s16	q7, d29, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqmovn.u16	d8, q4
		vqrshrun.s32	d12, q6, #0xc
		vqrshrun.s32	d13, q7, #0xc
		vqmovn.u16	d9, q6
		vst1.8		{d8}, [r5,:64],r1
		vst1.8		{d9}, [r5,:64],r1
		ble		loc_4420
		b		loc_430c
@ ---------------------------------------------------------------------------

loc_4420:				@ code xref: .text:00004418j
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_41a4
		b		locret_473c
@ ---------------------------------------------------------------------------

loc_4434:				@ code xref: .text:000041a0j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#2
		sub		r7, r7,	#4

loc_4448:				@ code xref: .text:000041b8j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22

loc_45a4:				@ code xref: .text:00004728j
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q11, d8, #2
		vshll.u8	q12, d10, #2
		vadd.i16	q11, q11, q6
		vadd.i16	q12, q12, q7
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vmov		d25, d26
		vsubl.s16	q4, d16, d22
		vsubl.s16	q5, d17, d23
		vsubl.s16	q6, d18, d24
		vsubl.s16	q7, d19, d25
		vmlsl.s16	q4, d17, d6[1]
		vmlsl.s16	q5, d18, d6[1]
		vmlsl.s16	q6, d19, d6[1]
		vmlsl.s16	q7, d20, d6[1]
		vmlal.s16	q4, d18, d6[3]
		vmlal.s16	q5, d19, d6[3]
		vmlal.s16	q6, d20, d6[3]
		vmlal.s16	q7, d21, d6[3]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d21, d6[0]
		vmlal.s16	q5, d22, d6[0]
		vmlal.s16	q6, d23, d6[0]
		vmlal.s16	q7, d24, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vqrshrun.s32	d8, q4,	#0xc
		vqrshrun.s32	d9, q5,	#0xc
		vqmovn.u16	d8, q4
		vqrshrun.s32	d12, q6, #0xc
		vqrshrun.s32	d13, q7, #0xc
		vqmovn.u16	d9, q6
		vst1.32		{d8[0]}, [r5,:32],r1
		vst1.32		{d8[1]}, [r5,:32],r1
		vst1.32		{d9[0]}, [r5,:32],r1
		vst1.32		{d9[1]}, [r5,:32],r1
		ble		loc_472c
		b		loc_45a4
@ ---------------------------------------------------------------------------

loc_472c:				@ code xref: .text:00004724j
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_41a4

locret_473c:				@ code xref: .text:00004430j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H0V0_neon
_VO_avg_nornd_MC_InterLuma_H0V0_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]

loc_4754:				@ code xref: .text:00004804j
					@ .text:00004884j ...
		cmp		r7, #0x20 @ ' '
		blt		loc_480c
		ands		r5, r0,	#0xf
		bne		loc_480c
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_4770:				@ code xref: .text:000047ecj
		vld1.64		{d0-d3}, [r4],r3
		vld1.64		{d4-d7}, [r4],r3
		vld1.64		{d8-d11}, [r4],r3
		vld1.64		{d12-d15}, [r4],r3
		subs		r8, r8,	#4
		vshll.u8	q8, d0,	#6
		vshll.u8	q9, d1,	#6
		vshll.u8	q10, d2, #6
		vshll.u8	q11, d3, #6
		vshll.u8	q12, d4, #6
		vshll.u8	q13, d5, #6
		vshll.u8	q14, d6, #6
		vshll.u8	q15, d7, #6
		vst1.64		{d16-d19}, [r12,:128]!
		vst1.64		{d20-d23}, [r12,:128],lr
		vst1.64		{d28-d31}, [r12,:128]
		sub		r12, r12, #0x20
		vst1.64		{d24-d27}, [r12,:128],lr
		vshll.u8	q8, d8,	#6
		vshll.u8	q9, d9,	#6
		vshll.u8	q10, d10, #6
		vshll.u8	q11, d11, #6
		vshll.u8	q12, d12, #6
		vshll.u8	q13, d13, #6
		vshll.u8	q14, d14, #6
		vshll.u8	q15, d15, #6
		vst1.64		{d16-d19}, [r12,:128]!
		vst1.64		{d20-d23}, [r12,:128],lr
		vst1.64		{d28-d31}, [r12,:128]
		sub		r12, r12, #0x20
		vst1.64		{d24-d27}, [r12,:128],lr
		bgt		loc_4770
		mls		r12, lr, r6, r12
		add		r12, r12, #0x40
		add		r2, r2,	#0x20
		add		r0, r0,	#0x20
		subs		r7, r7,	#0x20
		bgt		loc_4754
		b		locret_49c0
@ ---------------------------------------------------------------------------

loc_480c:				@ code xref: .text:00004758j
					@ .text:00004760j
		cmp		r7, #0x10
		blt		loc_488c
		ands		r5, r0,	#0xf
		bne		loc_488c
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_4828:				@ code xref: .text:0000486cj
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d4-d5}, [r4],r3
		vld1.64		{d6-d7}, [r4],r3
		subs		r8, r8,	#4
		vshll.u8	q8, d0,	#6
		vshll.u8	q9, d1,	#6
		vshll.u8	q10, d2, #6
		vshll.u8	q11, d3, #6
		vshll.u8	q12, d4, #6
		vshll.u8	q13, d5, #6
		vshll.u8	q14, d6, #6
		vshll.u8	q15, d7, #6
		vst1.64		{d16-d19}, [r12,:128],lr
		vst1.64		{d20-d23}, [r12,:128],lr
		vst1.64		{d24-d27}, [r12,:128],lr
		vst1.64		{d28-d31}, [r12,:128],lr
		bgt		loc_4828
		mls		r12, lr, r6, r12
		add		r12, r12, #0x20
		add		r2, r2,	#0x10
		add		r0, r0,	#0x10
		subs		r7, r7,	#0x10
		bgt		loc_4754
		b		locret_49c0
@ ---------------------------------------------------------------------------

loc_488c:				@ code xref: .text:00004810j
					@ .text:00004818j
		cmp		r7, #8
		blt		loc_4900
		ands		r5, r0,	#7
		bne		loc_4900
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_48a8:				@ code xref: .text:000048e0j
		vld1.64		{d0}, [r4],r3
		vld1.64		{d1}, [r4],r3
		vld1.64		{d2}, [r4],r3
		vld1.64		{d3}, [r4],r3
		subs		r8, r8,	#4
		vshll.u8	q8, d0,	#6
		vshll.u8	q9, d1,	#6
		vshll.u8	q10, d2, #6
		vshll.u8	q11, d3, #6
		vst1.64		{d16-d17}, [r12,:128],lr
		vst1.64		{d18-d19}, [r12,:128],lr
		blt		loc_48e4
		vst1.64		{d20-d21}, [r12,:128],lr
		vst1.64		{d22-d23}, [r12,:128],lr
		bgt		loc_48a8

loc_48e4:				@ code xref: .text:000048d4j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		add		r2, r2,	#8
		add		r0, r0,	#8
		subs		r7, r7,	#8
		bgt		loc_4754
		b		locret_49c0
@ ---------------------------------------------------------------------------

loc_4900:				@ code xref: .text:00004890j
					@ .text:00004898j
		cmp		r7, #4
		blt		loc_496c
		ands		r5, r0,	#3
		bne		loc_496c
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_491c:				@ code xref: .text:0000494cj
		vld1.32		{d0[0]}, [r4],r3
		vld1.32		{d0[1]}, [r4],r3
		vld1.32		{d1[0]}, [r4],r3
		vld1.32		{d1[1]}, [r4],r3
		subs		r8, r8,	#4
		vshll.u8	q8, d0,	#6
		vshll.u8	q9, d1,	#6
		vst1.64		{d16}, [r12,:64],lr
		vst1.64		{d17}, [r12,:64],lr
		blt		loc_4950
		vst1.64		{d18}, [r12,:64],lr
		vst1.64		{d19}, [r12,:64],lr
		bgt		loc_491c

loc_4950:				@ code xref: .text:00004940j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		subs		r7, r7,	#4
		bgt		loc_4754
		b		locret_49c0
@ ---------------------------------------------------------------------------

loc_496c:				@ code xref: .text:00004904j
					@ .text:0000490cj
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_4978:				@ code xref: .text:000049a4j
		vld1.16		{d0[0]}, [r4],r3
		vld1.16		{d0[1]}, [r4],r3
		vld1.16		{d0[2]}, [r4],r3
		vld1.16		{d0[3]}, [r4],r3
		subs		r8, r8,	#4
		vshll.u8	q8, d0,	#6
		vst1.32		{d16[0]}, [r12,:32],lr
		vst1.32		{d16[1]}, [r12,:32],lr
		blt		loc_49a8
		vst1.32		{d17[0]}, [r12,:32],lr
		vst1.32		{d17[1]}, [r12,:32],lr
		bgt		loc_4978

loc_49a8:				@ code xref: .text:00004998j
		mls		r12, lr, r6, r12
		add		r12, r12, #4
		add		r2, r2,	#2
		add		r0, r0,	#2
		subs		r7, r7,	#2
		bgt		loc_4754

locret_49c0:				@ code xref: .text:00004808j
					@ .text:00004888j ...
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H1V0_neon
_VO_avg_nornd_MC_InterLuma_H1V0_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d12, #4
		vmov.i8		d13, #0xa
		vmov.i8		d14, #0x3a @ ':'
		vmov.i8		d15, #0x11
		vmov.i8		d30, #5
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_4b0c

loc_49f4:				@ code xref: .text:00004b04j
					@ .text:00004c18j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#3
		subs		r7, r7,	#4
		ble		loc_4b1c

loc_4a08:				@ code xref: .text:00004aecj
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #6
		vext.8		d6, d2,	d3, #6
		vsubl.u8	q2, d4,	d0
		vsubl.u8	q3, d6,	d2
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #2
		vext.8		d10, d2, d3, #1
		vext.8		d11, d2, d3, #2
		vmlal.u8	q2, d8,	d12
		vmlsl.u8	q2, d9,	d13
		vmlal.u8	q3, d10, d12
		vmlsl.u8	q3, d11, d13
		vext.8		d8, d0,	d1, #3
		vext.8		d9, d0,	d1, #4
		vext.8		d10, d2, d3, #3
		vext.8		d11, d2, d3, #4
		vmlal.u8	q2, d8,	d14
		vmlal.u8	q2, d9,	d15
		vmlal.u8	q3, d10, d14
		vmlal.u8	q3, d11, d15
		vext.8		d8, d0,	d1, #5
		vext.8		d9, d2,	d3, #5
		vmlsl.u8	q2, d8,	d30
		vmlsl.u8	q3, d9,	d30
		vext.8		d20, d16, d17, #6
		vext.8		d22, d18, d19, #6
		vsubl.u8	q10, d20, d16
		vsubl.u8	q11, d22, d18
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #2
		vext.8		d26, d18, d19, #1
		vext.8		d27, d18, d19, #2
		vmlal.u8	q10, d24, d12
		vmlsl.u8	q10, d25, d13
		vmlal.u8	q11, d26, d12
		vmlsl.u8	q11, d27, d13
		vext.8		d28, d16, d17, #3
		vext.8		d29, d16, d17, #4
		vext.8		d24, d18, d19, #3
		vext.8		d25, d18, d19, #4
		vmlal.u8	q10, d28, d14
		vmlal.u8	q10, d29, d15
		vmlal.u8	q11, d24, d14
		vmlal.u8	q11, d25, d15
		vext.8		d28, d16, d17, #5
		vext.8		d29, d18, d19, #5
		vmlsl.u8	q10, d28, d30
		vmlsl.u8	q11, d29, d30
		vst1.8		{d4-d5}, [r12,:128],lr
		vst1.8		{d6-d7}, [r12,:128],lr
		vst1.8		{d20-d21}, [r12,:128],lr
		vst1.8		{d22-d23}, [r12,:128],lr
		bne		loc_4a08
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_49f4
		b		locret_4c1c
@ ---------------------------------------------------------------------------

loc_4b0c:				@ code xref: .text:000049f0j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#3
		sub		r7, r7,	#4

loc_4b1c:				@ code xref: .text:00004a04j
					@ .text:00004c00j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #6
		vext.8		d6, d2,	d3, #6
		vsubl.u8	q2, d4,	d0
		vsubl.u8	q3, d6,	d2
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #2
		vext.8		d10, d2, d3, #1
		vext.8		d11, d2, d3, #2
		vmlal.u8	q2, d8,	d12
		vmlsl.u8	q2, d9,	d13
		vmlal.u8	q3, d10, d12
		vmlsl.u8	q3, d11, d13
		vext.8		d8, d0,	d1, #3
		vext.8		d9, d0,	d1, #4
		vext.8		d10, d2, d3, #3
		vext.8		d11, d2, d3, #4
		vmlal.u8	q2, d8,	d14
		vmlal.u8	q2, d9,	d15
		vmlal.u8	q3, d10, d14
		vmlal.u8	q3, d11, d15
		vext.8		d8, d0,	d1, #5
		vext.8		d9, d2,	d3, #5
		vmlsl.u8	q2, d8,	d30
		vmlsl.u8	q3, d9,	d30
		vext.8		d20, d16, d17, #6
		vext.8		d22, d18, d19, #6
		vsubl.u8	q10, d20, d16
		vsubl.u8	q11, d22, d18
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #2
		vext.8		d26, d18, d19, #1
		vext.8		d27, d18, d19, #2
		vmlal.u8	q10, d24, d12
		vmlsl.u8	q10, d25, d13
		vmlal.u8	q11, d26, d12
		vmlsl.u8	q11, d27, d13
		vext.8		d28, d16, d17, #3
		vext.8		d29, d16, d17, #4
		vext.8		d24, d18, d19, #3
		vext.8		d25, d18, d19, #4
		vmlal.u8	q10, d28, d14
		vmlal.u8	q10, d29, d15
		vmlal.u8	q11, d24, d14
		vmlal.u8	q11, d25, d15
		vext.8		d28, d16, d17, #5
		vext.8		d29, d18, d19, #5
		vmlsl.u8	q10, d28, d30
		vmlsl.u8	q11, d29, d30
		vst1.8		{d4-d5}, [r12,:64],lr
		vst1.8		{d6-d7}, [r12,:64],lr
		vst1.8		{d20-d21}, [r12,:64],lr
		vst1.8		{d22-d23}, [r12,:64],lr
		bne		loc_4b1c
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_49f4

locret_4c1c:				@ code xref: .text:00004b08j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H2V0_neon
_VO_avg_nornd_MC_InterLuma_H2V0_neon:
		stmfd		sp!, {r4-r10,lr}
		mov		r6, #4
		movt  r6, 0xb
		mov		r7, #0xa
		vmov		d6, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_4d94

loc_4c4c:				@ code xref: .text:00004d8cj
					@ .text:00004ed0j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#3
		subs		r7, r7,	#4
		ble		loc_4da4

loc_4c60:				@ code xref: .text:00004d74j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d0,	d1, #4
		vext.8		d12, d2, d3, #3
		vext.8		d13, d2, d3, #4
		vext.8		d22, d16, d17, #3
		vext.8		d23, d16, d17, #4
		vext.8		d28, d18, d19, #3
		vext.8		d29, d18, d19, #4
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q6, d12, d13
		vaddl.u8	q11, d22, d23
		vaddl.u8	q14, d28, d29
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #6
		vext.8		d14, d2, d3, #1
		vext.8		d15, d2, d3, #6
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #6
		vext.8		d30, d18, d19, #1
		vext.8		d31, d18, d19, #6
		vaddl.u8	q4, d8,	d9
		vaddl.u8	q7, d14, d15
		vaddl.u8	q12, d24, d25
		vaddl.u8	q15, d30, d31
		vmla.i16	q4, q2,	d6[2]
		vmla.i16	q7, q6,	d6[2]
		vmla.i16	q12, q11, d6[2]
		vmla.i16	q15, q14, d6[2]
		vext.8		d10, d0, d1, #2
		vext.8		d11, d0, d1, #5
		vext.8		d20, d2, d3, #2
		vext.8		d21, d2, d3, #5
		vext.8		d26, d16, d17, #2
		vext.8		d27, d16, d17, #5
		vext.8		d28, d18, d19, #2
		vext.8		d29, d18, d19, #5
		vaddl.u8	q5, d10, d11
		vaddl.u8	q10, d20, d21
		vaddl.u8	q13, d26, d27
		vaddl.u8	q14, d28, d29
		vext.8		d5, d0,	d1, #7
		vext.8		d13, d2, d3, #7
		vext.8		d23, d16, d17, #7
		vext.8		d19, d18, d19, #7
		vaddl.u8	q2, d0,	d5
		vaddl.u8	q6, d2,	d13
		vaddl.u8	q11, d16, d23
		vaddl.u8	q9, d18, d19
		vshl.i16	q4, q4,	#2
		vshl.i16	q7, q7,	#2
		vshl.i16	q12, q12, #2
		vshl.i16	q15, q15, #2
		vmla.i16	q2, q5,	d6[1]
		vmla.i16	q6, q10, d6[1]
		vmla.i16	q11, q13, d6[1]
		vmla.i16	q9, q14, d6[1]
		vsub.i16	q4, q4,	q2
		vsub.i16	q5, q7,	q6
		vsub.i16	q8, q12, q11
		vsub.i16	q9, q15, q9
		vst1.8		{d8-d9}, [r12,:128],lr
		vst1.8		{d10-d11}, [r12,:128],lr
		vst1.8		{d16-d17}, [r12,:128],lr
		vst1.8		{d18-d19}, [r12,:128],lr
		bne		loc_4c60
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_4c4c
		b		locret_4ed4
@ ---------------------------------------------------------------------------

loc_4d94:				@ code xref: .text:00004c48j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#3
		sub		r7, r7,	#4

loc_4da4:				@ code xref: .text:00004c5cj
					@ .text:00004eb8j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d0,	d1, #4
		vext.8		d12, d2, d3, #3
		vext.8		d13, d2, d3, #4
		vext.8		d22, d16, d17, #3
		vext.8		d23, d16, d17, #4
		vext.8		d28, d18, d19, #3
		vext.8		d29, d18, d19, #4
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q6, d12, d13
		vaddl.u8	q11, d22, d23
		vaddl.u8	q14, d28, d29
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #6
		vext.8		d14, d2, d3, #1
		vext.8		d15, d2, d3, #6
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #6
		vext.8		d30, d18, d19, #1
		vext.8		d31, d18, d19, #6
		vaddl.u8	q4, d8,	d9
		vaddl.u8	q7, d14, d15
		vaddl.u8	q12, d24, d25
		vaddl.u8	q15, d30, d31
		vmla.i16	q4, q2,	d6[2]
		vmla.i16	q7, q6,	d6[2]
		vmla.i16	q12, q11, d6[2]
		vmla.i16	q15, q14, d6[2]
		vext.8		d10, d0, d1, #2
		vext.8		d11, d0, d1, #5
		vext.8		d20, d2, d3, #2
		vext.8		d21, d2, d3, #5
		vext.8		d26, d16, d17, #2
		vext.8		d27, d16, d17, #5
		vext.8		d28, d18, d19, #2
		vext.8		d29, d18, d19, #5
		vaddl.u8	q5, d10, d11
		vaddl.u8	q10, d20, d21
		vaddl.u8	q13, d26, d27
		vaddl.u8	q14, d28, d29
		vext.8		d5, d0,	d1, #7
		vext.8		d13, d2, d3, #7
		vext.8		d23, d16, d17, #7
		vext.8		d19, d18, d19, #7
		vaddl.u8	q2, d0,	d5
		vaddl.u8	q6, d2,	d13
		vaddl.u8	q11, d16, d23
		vaddl.u8	q9, d18, d19
		vshl.i16	q4, q4,	#2
		vshl.i16	q7, q7,	#2
		vshl.i16	q12, q12, #2
		vshl.i16	q15, q15, #2
		vmla.i16	q2, q5,	d6[1]
		vmla.i16	q6, q10, d6[1]
		vmla.i16	q11, q13, d6[1]
		vmla.i16	q9, q14, d6[1]
		vsub.i16	q4, q4,	q2
		vsub.i16	q5, q7,	q6
		vsub.i16	q8, q12, q11
		vsub.i16	q9, q15, q9
		vst1.8		{d8-d9}, [r12,:64],lr
		vst1.8		{d10-d11}, [r12,:64],lr
		vst1.8		{d16-d17}, [r12,:64],lr
		vst1.8		{d18-d19}, [r12,:64],lr
		bne		loc_4da4
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_4c4c

locret_4ed4:				@ code xref: .text:00004d90j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H3V0_neon
_VO_avg_nornd_MC_InterLuma_H3V0_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d12, #5
		vmov.i8		d13, #0x11
		vmov.i8		d14, #0x3a @ ':'
		vmov.i8		d15, #0xa
		vmov.i8		d30, #4
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_5020

loc_4f08:				@ code xref: .text:00005018j
					@ .text:0000512cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#2
		subs		r7, r7,	#4
		ble		loc_5030

loc_4f1c:				@ code xref: .text:00005000j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #6
		vext.8		d6, d2,	d3, #6
		vsubl.u8	q2, d0,	d4
		vsubl.u8	q3, d2,	d6
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #2
		vext.8		d10, d2, d3, #1
		vext.8		d11, d2, d3, #2
		vmlsl.u8	q2, d8,	d12
		vmlal.u8	q2, d9,	d13
		vmlsl.u8	q3, d10, d12
		vmlal.u8	q3, d11, d13
		vext.8		d8, d0,	d1, #3
		vext.8		d9, d0,	d1, #4
		vext.8		d10, d2, d3, #3
		vext.8		d11, d2, d3, #4
		vmlal.u8	q2, d8,	d14
		vmlsl.u8	q2, d9,	d15
		vmlal.u8	q3, d10, d14
		vmlsl.u8	q3, d11, d15
		vext.8		d8, d0,	d1, #5
		vext.8		d9, d2,	d3, #5
		vmlal.u8	q2, d8,	d30
		vmlal.u8	q3, d9,	d30
		vext.8		d20, d16, d17, #6
		vext.8		d22, d18, d19, #6
		vsubl.u8	q10, d16, d20
		vsubl.u8	q11, d18, d22
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #2
		vext.8		d26, d18, d19, #1
		vext.8		d27, d18, d19, #2
		vmlsl.u8	q10, d24, d12
		vmlal.u8	q10, d25, d13
		vmlsl.u8	q11, d26, d12
		vmlal.u8	q11, d27, d13
		vext.8		d28, d16, d17, #3
		vext.8		d29, d16, d17, #4
		vext.8		d24, d18, d19, #3
		vext.8		d25, d18, d19, #4
		vmlal.u8	q10, d28, d14
		vmlsl.u8	q10, d29, d15
		vmlal.u8	q11, d24, d14
		vmlsl.u8	q11, d25, d15
		vext.8		d28, d16, d17, #5
		vext.8		d29, d18, d19, #5
		vmlal.u8	q10, d28, d30
		vmlal.u8	q11, d29, d30
		vst1.8		{d4-d5}, [r12,:128],lr
		vst1.8		{d6-d7}, [r12,:128],lr
		vst1.8		{d20-d21}, [r12,:128],lr
		vst1.8		{d22-d23}, [r12,:128],lr
		bne		loc_4f1c
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_4f08
		b		locret_5130
@ ---------------------------------------------------------------------------

loc_5020:				@ code xref: .text:00004f04j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#2
		sub		r7, r7,	#4

loc_5030:				@ code xref: .text:00004f18j
					@ .text:00005114j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #6
		vext.8		d6, d2,	d3, #6
		vsubl.u8	q2, d0,	d4
		vsubl.u8	q3, d2,	d6
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #2
		vext.8		d10, d2, d3, #1
		vext.8		d11, d2, d3, #2
		vmlsl.u8	q2, d8,	d12
		vmlal.u8	q2, d9,	d13
		vmlsl.u8	q3, d10, d12
		vmlal.u8	q3, d11, d13
		vext.8		d8, d0,	d1, #3
		vext.8		d9, d0,	d1, #4
		vext.8		d10, d2, d3, #3
		vext.8		d11, d2, d3, #4
		vmlal.u8	q2, d8,	d14
		vmlsl.u8	q2, d9,	d15
		vmlal.u8	q3, d10, d14
		vmlsl.u8	q3, d11, d15
		vext.8		d8, d0,	d1, #5
		vext.8		d9, d2,	d3, #5
		vmlal.u8	q2, d8,	d30
		vmlal.u8	q3, d9,	d30
		vext.8		d20, d16, d17, #6
		vext.8		d22, d18, d19, #6
		vsubl.u8	q10, d16, d20
		vsubl.u8	q11, d18, d22
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #2
		vext.8		d26, d18, d19, #1
		vext.8		d27, d18, d19, #2
		vmlsl.u8	q10, d24, d12
		vmlal.u8	q10, d25, d13
		vmlsl.u8	q11, d26, d12
		vmlal.u8	q11, d27, d13
		vext.8		d28, d16, d17, #3
		vext.8		d29, d16, d17, #4
		vext.8		d24, d18, d19, #3
		vext.8		d25, d18, d19, #4
		vmlal.u8	q10, d28, d14
		vmlsl.u8	q10, d29, d15
		vmlal.u8	q11, d24, d14
		vmlsl.u8	q11, d25, d15
		vext.8		d28, d16, d17, #5
		vext.8		d29, d18, d19, #5
		vmlal.u8	q10, d28, d30
		vmlal.u8	q11, d29, d30
		vst1.8		{d4-d5}, [r12,:64],lr
		vst1.8		{d6-d7}, [r12,:64],lr
		vst1.8		{d20-d21}, [r12,:64],lr
		vst1.8		{d22-d23}, [r12,:64],lr
		bne		loc_5030
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_4f08

locret_5130:				@ code xref: .text:0000501cj
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H0V1_neon
_VO_avg_nornd_MC_InterLuma_H0V1_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d16, #0xa
		vmov.i8		d17, #0x3a @ ':'
		vmov.i8		d18, #0x11
		vmov.i8		d19, #5
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_5254

loc_5160:				@ code xref: .text:0000524cj
					@ .text:0000533cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		subs		r7, r7,	#4
		ble		loc_5268
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3

loc_5190:				@ code xref: .text:00005234j
		vld1.64		{d26}, [r4],r3
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		subs		r8, r8,	#4
		vsubl.u8	q0, d26, d20
		vshll.u8	q1, d21, #2
		vsubl.u8	q2, d27, d21
		vshll.u8	q3, d22, #2
		vsubl.u8	q4, d28, d22
		vshll.u8	q5, d23, #2
		vsubl.u8	q6, d29, d23
		vshll.u8	q7, d24, #2
		vadd.i16	q1, q1,	q0
		vadd.i16	q3, q3,	q2
		vadd.i16	q5, q5,	q4
		vadd.i16	q7, q7,	q6
		vmlsl.u8	q1, d22, d16
		vmlsl.u8	q3, d23, d16
		vmlsl.u8	q5, d24, d16
		vmlsl.u8	q7, d25, d16
		vmlal.u8	q1, d23, d17
		vmlal.u8	q3, d24, d17
		vmlal.u8	q5, d25, d17
		vmlal.u8	q7, d26, d17
		vmlal.u8	q1, d24, d18
		vmlal.u8	q3, d25, d18
		vmlal.u8	q5, d26, d18
		vmlal.u8	q7, d27, d18
		vmlsl.u8	q1, d25, d19
		vmlsl.u8	q3, d26, d19
		vmlsl.u8	q5, d27, d19
		vmlsl.u8	q7, d28, d19
		vst1.8		{d2-d3}, [r12,:128],lr
		vst1.8		{d6-d7}, [r12,:128],lr
		vst1.8		{d10-d11}, [r12,:128],lr
		vst1.8		{d14-d15}, [r12,:128],lr
		ble		loc_5238
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		b		loc_5190
@ ---------------------------------------------------------------------------

loc_5238:				@ code xref: .text:00005224j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_5160
		b		locret_5340
@ ---------------------------------------------------------------------------

loc_5254:				@ code xref: .text:0000515cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r7, r7,	#4

loc_5268:				@ code xref: .text:00005174j
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3

loc_5280:				@ code xref: .text:00005324j
		vld1.64		{d26}, [r4],r3
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		subs		r8, r8,	#4
		vsubl.u8	q0, d26, d20
		vshll.u8	q1, d21, #2
		vsubl.u8	q2, d27, d21
		vshll.u8	q3, d22, #2
		vsubl.u8	q4, d28, d22
		vshll.u8	q5, d23, #2
		vsubl.u8	q6, d29, d23
		vshll.u8	q7, d24, #2
		vadd.i16	q1, q1,	q0
		vadd.i16	q3, q3,	q2
		vadd.i16	q5, q5,	q4
		vadd.i16	q7, q7,	q6
		vmlsl.u8	q1, d22, d16
		vmlsl.u8	q3, d23, d16
		vmlsl.u8	q5, d24, d16
		vmlsl.u8	q7, d25, d16
		vmlal.u8	q1, d23, d17
		vmlal.u8	q3, d24, d17
		vmlal.u8	q5, d25, d17
		vmlal.u8	q7, d26, d17
		vmlal.u8	q1, d24, d18
		vmlal.u8	q3, d25, d18
		vmlal.u8	q5, d26, d18
		vmlal.u8	q7, d27, d18
		vmlsl.u8	q1, d25, d19
		vmlsl.u8	q3, d26, d19
		vmlsl.u8	q5, d27, d19
		vmlsl.u8	q7, d28, d19
		vst1.8		{d2-d3}, [r12,:64],lr
		vst1.8		{d6-d7}, [r12,:64],lr
		vst1.8		{d10-d11}, [r12,:64],lr
		vst1.8		{d14-d15}, [r12,:64],lr
		ble		loc_5328
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		b		loc_5280
@ ---------------------------------------------------------------------------

loc_5328:				@ code xref: .text:00005314j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_5160

locret_5340:				@ code xref: .text:00005250j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H0V2_neon
_VO_avg_nornd_MC_InterLuma_H0V2_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i16	q8, #0xb
		vmov.i16	q9, #0x28 @ '('
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_5474

loc_5368:				@ code xref: .text:0000546cj
					@ .text:00005574j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		subs		r7, r7,	#4
		ble		loc_5488
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3
		vld1.64		{d26}, [r4],r3

loc_539c:				@ code xref: .text:00005454j
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		vld1.64		{d30}, [r4],r3
		subs		r8, r8,	#4
		vaddl.u8	q0, d20, d27
		vaddl.u8	q1, d21, d26
		vaddl.u8	q2, d21, d28
		vaddl.u8	q3, d22, d27
		vaddl.u8	q4, d22, d29
		vaddl.u8	q5, d23, d28
		vaddl.u8	q6, d23, d30
		vaddl.u8	q7, d24, d29
		vshl.i16	q1, q1,	#2
		vshl.i16	q3, q3,	#2
		vshl.i16	q5, q5,	#2
		vshl.i16	q7, q7,	#2
		vsub.i16	q1, q1,	q0
		vsub.i16	q3, q3,	q2
		vsub.i16	q5, q5,	q4
		vsub.i16	q7, q7,	q6
		vaddl.u8	q0, d22, d25
		vaddl.u8	q2, d23, d26
		vaddl.u8	q4, d24, d27
		vaddl.u8	q6, d25, d28
		vmls.i16	q1, q0,	q8
		vmls.i16	q3, q2,	q8
		vmls.i16	q5, q4,	q8
		vmls.i16	q7, q6,	q8
		vaddl.u8	q0, d23, d24
		vaddl.u8	q2, d24, d25
		vaddl.u8	q4, d25, d26
		vaddl.u8	q6, d26, d27
		vmla.i16	q1, q0,	q9
		vmla.i16	q3, q2,	q9
		vmla.i16	q5, q4,	q9
		vmla.i16	q7, q6,	q9
		vst1.8		{d2-d3}, [r12,:128],lr
		vst1.8		{d6-d7}, [r12,:128],lr
		vst1.8		{d10-d11}, [r12,:128],lr
		vst1.8		{d14-d15}, [r12,:128],lr
		ble		loc_5458
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		d26, d30
		b		loc_539c
@ ---------------------------------------------------------------------------

loc_5458:				@ code xref: .text:00005440j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_5368
		b		locret_5578
@ ---------------------------------------------------------------------------

loc_5474:				@ code xref: .text:00005364j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r7, r7,	#4

loc_5488:				@ code xref: .text:0000537cj
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3
		vld1.64		{d26}, [r4],r3

loc_54a4:				@ code xref: .text:0000555cj
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		vld1.64		{d30}, [r4],r3
		subs		r8, r8,	#4
		vaddl.u8	q0, d20, d27
		vaddl.u8	q1, d21, d26
		vaddl.u8	q2, d21, d28
		vaddl.u8	q3, d22, d27
		vaddl.u8	q4, d22, d29
		vaddl.u8	q5, d23, d28
		vaddl.u8	q6, d23, d30
		vaddl.u8	q7, d24, d29
		vshl.i16	q1, q1,	#2
		vshl.i16	q3, q3,	#2
		vshl.i16	q5, q5,	#2
		vshl.i16	q7, q7,	#2
		vsub.i16	q1, q1,	q0
		vsub.i16	q3, q3,	q2
		vsub.i16	q5, q5,	q4
		vsub.i16	q7, q7,	q6
		vaddl.u8	q0, d22, d25
		vaddl.u8	q2, d23, d26
		vaddl.u8	q4, d24, d27
		vaddl.u8	q6, d25, d28
		vmls.i16	q1, q0,	q8
		vmls.i16	q3, q2,	q8
		vmls.i16	q5, q4,	q8
		vmls.i16	q7, q6,	q8
		vaddl.u8	q0, d23, d24
		vaddl.u8	q2, d24, d25
		vaddl.u8	q4, d25, d26
		vaddl.u8	q6, d26, d27
		vmla.i16	q1, q0,	q9
		vmla.i16	q3, q2,	q9
		vmla.i16	q5, q4,	q9
		vmla.i16	q7, q6,	q9
		vst1.8		{d2-d3}, [r12,:64],lr
		vst1.8		{d6-d7}, [r12,:64],lr
		vst1.8		{d10-d11}, [r12,:64],lr
		vst1.8		{d14-d15}, [r12,:64],lr
		ble		loc_5560
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		d26, d30
		b		loc_54a4
@ ---------------------------------------------------------------------------

loc_5560:				@ code xref: .text:00005548j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_5368

locret_5578:				@ code xref: .text:00005470j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H0V3_neon
_VO_avg_nornd_MC_InterLuma_H0V3_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d16, #5
		vmov.i8		d17, #0x11
		vmov.i8		d18, #0x3a @ ':'
		vmov.i8		d19, #0xa
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_5698

loc_55a8:				@ code xref: .text:00005690j
					@ .text:0000577cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		subs		r7, r7,	#4
		ble		loc_56a8
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3

loc_55d4:				@ code xref: .text:00005678j
		vld1.64		{d26}, [r4],r3
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		subs		r8, r8,	#4
		vsubl.u8	q0, d20, d26
		vshll.u8	q1, d25, #2
		vsubl.u8	q2, d21, d27
		vshll.u8	q3, d26, #2
		vsubl.u8	q4, d22, d28
		vshll.u8	q5, d27, #2
		vsubl.u8	q6, d23, d29
		vshll.u8	q7, d28, #2
		vadd.i16	q1, q1,	q0
		vadd.i16	q3, q3,	q2
		vadd.i16	q5, q5,	q4
		vadd.i16	q7, q7,	q6
		vmlsl.u8	q1, d21, d16
		vmlsl.u8	q3, d22, d16
		vmlsl.u8	q5, d23, d16
		vmlsl.u8	q7, d24, d16
		vmlal.u8	q1, d22, d17
		vmlal.u8	q3, d23, d17
		vmlal.u8	q5, d24, d17
		vmlal.u8	q7, d25, d17
		vmlal.u8	q1, d23, d18
		vmlal.u8	q3, d24, d18
		vmlal.u8	q5, d25, d18
		vmlal.u8	q7, d26, d18
		vmlsl.u8	q1, d24, d19
		vmlsl.u8	q3, d25, d19
		vmlsl.u8	q5, d26, d19
		vmlsl.u8	q7, d27, d19
		vst1.8		{d2-d3}, [r12,:128],lr
		vst1.8		{d6-d7}, [r12,:128],lr
		vst1.8		{d10-d11}, [r12,:128],lr
		vst1.8		{d14-d15}, [r12,:128],lr
		ble		loc_567c
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		b		loc_55d4
@ ---------------------------------------------------------------------------

loc_567c:				@ code xref: .text:00005668j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_55a8
		b		locret_5780
@ ---------------------------------------------------------------------------

loc_5698:				@ code xref: .text:000055a4j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r7, r7,	#4

loc_56a8:				@ code xref: .text:000055b8j
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3

loc_56c0:				@ code xref: .text:00005764j
		vld1.64		{d26}, [r4],r3
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		subs		r8, r8,	#4
		vsubl.u8	q0, d20, d26
		vshll.u8	q1, d25, #2
		vsubl.u8	q2, d21, d27
		vshll.u8	q3, d26, #2
		vsubl.u8	q4, d22, d28
		vshll.u8	q5, d27, #2
		vsubl.u8	q6, d23, d29
		vshll.u8	q7, d28, #2
		vadd.i16	q1, q1,	q0
		vadd.i16	q3, q3,	q2
		vadd.i16	q5, q5,	q4
		vadd.i16	q7, q7,	q6
		vmlsl.u8	q1, d21, d16
		vmlsl.u8	q3, d22, d16
		vmlsl.u8	q5, d23, d16
		vmlsl.u8	q7, d24, d16
		vmlal.u8	q1, d22, d17
		vmlal.u8	q3, d23, d17
		vmlal.u8	q5, d24, d17
		vmlal.u8	q7, d25, d17
		vmlal.u8	q1, d23, d18
		vmlal.u8	q3, d24, d18
		vmlal.u8	q5, d25, d18
		vmlal.u8	q7, d26, d18
		vmlsl.u8	q1, d24, d19
		vmlsl.u8	q3, d25, d19
		vmlsl.u8	q5, d26, d19
		vmlsl.u8	q7, d27, d19
		vst1.8		{d2-d3}, [r12,:64],lr
		vst1.8		{d6-d7}, [r12,:64],lr
		vst1.8		{d10-d11}, [r12,:64],lr
		vst1.8		{d14-d15}, [r12,:64],lr
		ble		loc_5768
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		b		loc_56c0
@ ---------------------------------------------------------------------------

loc_5768:				@ code xref: .text:00005754j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_55a8

locret_5780:				@ code xref: .text:00005694j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H1V1_neon
_VO_avg_nornd_MC_InterLuma_H1V1_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_5a64

loc_57d0:				@ code xref: .text:00005a5cj
					@ .text:00005d6cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_5a7c
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7

loc_593c:				@ code xref: .text:00005a44j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vsubl.s16	q4, d28, d16
		vsubl.s16	q5, d29, d17
		vsubl.s16	q6, d30, d18
		vsubl.s16	q7, d31, d19
		vmlal.s16	q4, d18, d6[0]
		vmlal.s16	q5, d19, d6[0]
		vmlal.s16	q6, d20, d6[0]
		vmlal.s16	q7, d21, d6[0]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlal.s16	q4, d24, d6[3]
		vmlal.s16	q5, d25, d6[3]
		vmlal.s16	q6, d26, d6[3]
		vmlal.s16	q7, d27, d6[3]
		vmlsl.s16	q4, d26, d6[1]
		vmlsl.s16	q5, d27, d6[1]
		vmlsl.s16	q6, d28, d6[1]
		vmlsl.s16	q7, d29, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vst1.8		{d8-d9}, [r12,:128],lr
		vst1.8		{d10-d11}, [r12,:128],lr
		ble		loc_5a48
		b		loc_593c
@ ---------------------------------------------------------------------------

loc_5a48:				@ code xref: .text:00005a40j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_57d0
		b		locret_5d70
@ ---------------------------------------------------------------------------

loc_5a64:				@ code xref: .text:000057ccj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_5a7c:				@ code xref: .text:000057e8j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22

loc_5bd8:				@ code xref: .text:00005d54j
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q11, d8, #2
		vshll.u8	q12, d10, #2
		vadd.i16	q11, q11, q6
		vadd.i16	q12, q12, q7
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vmov		d25, d26
		vsubl.s16	q4, d22, d16
		vsubl.s16	q5, d23, d17
		vsubl.s16	q6, d24, d18
		vsubl.s16	q7, d25, d19
		vmlal.s16	q4, d17, d6[0]
		vmlal.s16	q5, d18, d6[0]
		vmlal.s16	q6, d19, d6[0]
		vmlal.s16	q7, d20, d6[0]
		vmlsl.s16	q4, d18, d7[0]
		vmlsl.s16	q5, d19, d7[0]
		vmlsl.s16	q6, d20, d7[0]
		vmlsl.s16	q7, d21, d7[0]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlsl.s16	q4, d21, d6[1]
		vmlsl.s16	q5, d22, d6[1]
		vmlsl.s16	q6, d23, d6[1]
		vmlsl.s16	q7, d24, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vst1.8		{d8}, [r12,:64],lr
		vst1.8		{d9}, [r12,:64],lr
		vst1.8		{d10}, [r12,:64],lr
		vst1.8		{d11}, [r12,:64],lr
		ble		loc_5d58
		b		loc_5bd8
@ ---------------------------------------------------------------------------

loc_5d58:				@ code xref: .text:00005d50j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_57d0

locret_5d70:				@ code xref: .text:00005a60j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H1V2_neon
_VO_avg_nornd_MC_InterLuma_H1V2_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_602c

loc_5dc0:				@ code xref: .text:00006024j
					@ .text:0000639cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_6044
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vext.8		d8, d28, d29, #6
		vsubl.u8	q6, d8,	d28
		vext.8		d8, d28, d29, #2
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d28, d29, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d28, d29, #4
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d28, d29, #5
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d28, d29, #1
		vshll.u8	q14, d8, #2
		vadd.i16	q14, q14, q6

loc_5f64:				@ code xref: .text:0000600cj
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d30, d31, #6
		vsubl.u8	q6, d8,	d30
		vext.8		d8, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d30, d31, #1
		vshll.u8	q15, d8, #2
		vadd.i16	q15, q15, q6
		subs		r8, r8,	#1
		vaddl.s16	q4, d18, d28
		vaddl.s16	q5, d19, d29
		vaddl.s16	q6, d22, d24
		vaddl.s16	q7, d23, d25
		vmla.i32	q4, q6,	d7[0]
		vmla.i32	q5, q7,	d7[0]
		vaddl.s16	q6, d16, d30
		vaddl.s16	q7, d17, d31
		vaddl.s16	q2, d20, d26
		vaddl.s16	q8, d21, d27
		vmla.i32	q6, q2,	d7[1]
		vmla.i32	q7, q8,	d7[1]
		vshl.i32	q4, q4,	#2
		vshl.i32	q5, q5,	#2
		vmov		q8, q9
		vmov		q9, q10
		vmov		q10, q11
		vsub.i32	q4, q4,	q6
		vsub.i32	q5, q5,	q7
		vmov		q11, q12
		vmov		q12, q13
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vmov		q13, q14
		vmov		q14, q15
		vst1.8		{d8-d9}, [r12,:128],lr
		ble		loc_6010
		b		loc_5f64
@ ---------------------------------------------------------------------------

loc_6010:				@ code xref: .text:00006008j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_5dc0
		b		locret_63a0
@ ---------------------------------------------------------------------------

loc_602c:				@ code xref: .text:00005dbcj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_6044:				@ code xref: .text:00005dd8j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22
		vext.8		d8, d28, d29, #6
		vsubl.u8	q6, d8,	d28
		vext.8		d8, d28, d29, #2
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d28, d29, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d28, d29, #4
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d28, d29, #5
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d28, d29, #1
		vshll.u8	q14, d8, #2
		vadd.i16	q14, q14, q6
		vmov		d22, d28

loc_61dc:				@ code xref: .text:00006384j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vmov		d23, d28
		vmov		d24, d30
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vmov		d25, d28
		vmov		d26, d30
		subs		r8, r8,	#4
		vaddl.s16	q4, d16, d23
		vaddl.s16	q5, d18, d21
		vmla.i32	q4, q5,	d7[1]
		vaddl.s16	q5, d17, d22
		vaddl.s16	q6, d19, d20
		vmla.i32	q5, q6,	d7[0]
		vshl.i32	q5, q5,	#2
		vsub.i32	q4, q5,	q4
		vaddl.s16	q5, d17, d24
		vaddl.s16	q6, d19, d22
		vmla.i32	q5, q6,	d7[1]
		vaddl.s16	q6, d18, d23
		vaddl.s16	q7, d20, d21
		vmla.i32	q6, q7,	d7[0]
		vshl.i32	q6, q6,	#2
		vsub.i32	q5, q6,	q5
		vaddl.s16	q6, d18, d25
		vaddl.s16	q7, d20, d23
		vmla.i32	q6, q7,	d7[1]
		vaddl.s16	q7, d19, d24
		vaddl.s16	q8, d21, d22
		vmla.i32	q7, q8,	d7[0]
		vshl.i32	q7, q7,	#2
		vsub.i32	q6, q7,	q6
		vaddl.s16	q7, d19, d26
		vaddl.s16	q8, d21, d24
		vmla.i32	q7, q8,	d7[1]
		vaddl.s16	q8, d20, d25
		vaddl.s16	q15, d22, d23
		vmla.i32	q8, q15, d7[0]
		vshl.i32	q8, q8,	#2
		vsub.i32	q7, q8,	q7
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		d22, d26
		vst1.8		{d8}, [r12,:64],lr
		vst1.8		{d9}, [r12,:64],lr
		vst1.8		{d10}, [r12,:64],lr
		vst1.8		{d11}, [r12,:64],lr
		ble		loc_6388
		b		loc_61dc
@ ---------------------------------------------------------------------------

loc_6388:				@ code xref: .text:00006380j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_5dc0

locret_63a0:				@ code xref: .text:00006028j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H1V3_neon
_VO_avg_nornd_MC_InterLuma_H1V3_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_6680

loc_63f0:				@ code xref: .text:00006678j
					@ .text:00006984j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_6694
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7

loc_6558:				@ code xref: .text:00006660j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vsubl.s16	q4, d16, d28
		vsubl.s16	q5, d17, d29
		vsubl.s16	q6, d18, d30
		vsubl.s16	q7, d19, d31
		vmlsl.s16	q4, d18, d6[1]
		vmlsl.s16	q5, d19, d6[1]
		vmlsl.s16	q6, d20, d6[1]
		vmlsl.s16	q7, d21, d6[1]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlsl.s16	q4, d24, d7[0]
		vmlsl.s16	q5, d25, d7[0]
		vmlsl.s16	q6, d26, d7[0]
		vmlsl.s16	q7, d27, d7[0]
		vmlal.s16	q4, d26, d6[0]
		vmlal.s16	q5, d27, d6[0]
		vmlal.s16	q6, d28, d6[0]
		vmlal.s16	q7, d29, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vst1.8		{d8-d9}, [r12,:128],lr
		vst1.8		{d10-d11}, [r12,:128],lr
		ble		loc_6664
		b		loc_6558
@ ---------------------------------------------------------------------------

loc_6664:				@ code xref: .text:0000665cj
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_63f0
		b		locret_6988
@ ---------------------------------------------------------------------------

loc_6680:				@ code xref: .text:000063ecj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_6694:				@ code xref: .text:00006404j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22

loc_67f0:				@ code xref: .text:0000696cj
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q11, d8, #2
		vshll.u8	q12, d10, #2
		vadd.i16	q11, q11, q6
		vadd.i16	q12, q12, q7
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vmov		d25, d26
		vsubl.s16	q4, d16, d22
		vsubl.s16	q5, d17, d23
		vsubl.s16	q6, d18, d24
		vsubl.s16	q7, d19, d25
		vmlsl.s16	q4, d17, d6[1]
		vmlsl.s16	q5, d18, d6[1]
		vmlsl.s16	q6, d19, d6[1]
		vmlsl.s16	q7, d20, d6[1]
		vmlal.s16	q4, d18, d6[3]
		vmlal.s16	q5, d19, d6[3]
		vmlal.s16	q6, d20, d6[3]
		vmlal.s16	q7, d21, d6[3]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d21, d6[0]
		vmlal.s16	q5, d22, d6[0]
		vmlal.s16	q6, d23, d6[0]
		vmlal.s16	q7, d24, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vst1.8		{d8}, [r12,:64],lr
		vst1.8		{d9}, [r12,:64],lr
		vst1.8		{d10}, [r12,:64],lr
		vst1.8		{d11}, [r12,:64],lr
		ble		loc_6970
		b		loc_67f0
@ ---------------------------------------------------------------------------

loc_6970:				@ code xref: .text:00006968j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_63f0

locret_6988:				@ code xref: .text:0000667cj
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H2V1_neon
_VO_avg_nornd_MC_InterLuma_H2V1_neon:
		stmfd		sp!, {r4-r10,lr}
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_6c9c

loc_69c8:				@ code xref: .text:00006c94j
					@ .text:00006ff4j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_6cb4
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q10, d20, d8
		vaddl.u8	q11, d22, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q12, d24, d8
		vaddl.u8	q13, d26, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13

loc_6b64:				@ code xref: .text:00006c7cj
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q14, d28, d8
		vaddl.u8	q15, d30, d9
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q15, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q14, q1, q14
		vsub.i16	q15, q6, q15
		vsubl.s16	q4, d28, d16
		vsubl.s16	q5, d29, d17
		vsubl.s16	q6, d30, d18
		vsubl.s16	q7, d31, d19
		vmlal.s16	q4, d18, d6[0]
		vmlal.s16	q5, d19, d6[0]
		vmlal.s16	q6, d20, d6[0]
		vmlal.s16	q7, d21, d6[0]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlal.s16	q4, d24, d6[3]
		vmlal.s16	q5, d25, d6[3]
		vmlal.s16	q6, d26, d6[3]
		vmlal.s16	q7, d27, d6[3]
		vmlsl.s16	q4, d26, d6[1]
		vmlsl.s16	q5, d27, d6[1]
		vmlsl.s16	q6, d28, d6[1]
		vmlsl.s16	q7, d29, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d12, q6, #6
		vshrn.i32	d13, q7, #6
		vst1.8		{d8-d9}, [r12,:128],lr
		vst1.8		{d12-d13}, [r12,:128],lr
		ble		loc_6c80
		b		loc_6b64
@ ---------------------------------------------------------------------------

loc_6c80:				@ code xref: .text:00006c78j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_69c8
		b		locret_6ff8
@ ---------------------------------------------------------------------------

loc_6c9c:				@ code xref: .text:000069c4j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_6cb4:				@ code xref: .text:000069e0j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vmov		d17, d18
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q9, d20, d8
		vaddl.u8	q10, d22, d9
		vmla.i16	q9, q2,	d7[2]
		vmla.i16	q10, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q9, q1,	q9
		vsub.i16	q10, q6, q10
		vmov		d19, d20
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q10, d24, d8
		vaddl.u8	q11, d26, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vmov		d21, d22

loc_6e40:				@ code xref: .text:00006fdcj
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q11, d24, d8
		vaddl.u8	q12, d26, d9
		vmla.i16	q11, q2, d7[2]
		vmla.i16	q12, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q11, q1, q11
		vsub.i16	q12, q6, q12
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q12, d28, d8
		vaddl.u8	q13, d30, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13
		vmov		d25, d26
		vsubl.s16	q4, d22, d16
		vsubl.s16	q5, d23, d17
		vsubl.s16	q6, d24, d18
		vsubl.s16	q7, d25, d19
		vmlal.s16	q4, d17, d6[0]
		vmlal.s16	q5, d18, d6[0]
		vmlal.s16	q6, d19, d6[0]
		vmlal.s16	q7, d20, d6[0]
		vmlsl.s16	q4, d18, d7[0]
		vmlsl.s16	q5, d19, d7[0]
		vmlsl.s16	q6, d20, d7[0]
		vmlsl.s16	q7, d21, d7[0]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlsl.s16	q4, d21, d6[1]
		vmlsl.s16	q5, d22, d6[1]
		vmlsl.s16	q6, d23, d6[1]
		vmlsl.s16	q7, d24, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d12, q6, #6
		vshrn.i32	d13, q7, #6
		vst1.8		{d8}, [r12,:64],lr
		vst1.8		{d9}, [r12,:64],lr
		vst1.8		{d12}, [r12,:64],lr
		vst1.8		{d13}, [r12,:64],lr
		ble		loc_6fe0
		b		loc_6e40
@ ---------------------------------------------------------------------------

loc_6fe0:				@ code xref: .text:00006fd8j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_69c8

locret_6ff8:				@ code xref: .text:00006c98j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H2V2_neon
_VO_avg_nornd_MC_InterLuma_H2V2_neon:
		stmfd		sp!, {r4-r10,lr}
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_72e4

loc_7038:				@ code xref: .text:000072dcj
					@ .text:000076b0j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_72fc
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q10, d20, d8
		vaddl.u8	q11, d22, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q12, d24, d8
		vaddl.u8	q13, d26, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vaddl.u8	q0, d0,	d1
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vaddl.u8	q1, d2,	d3
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vaddl.u8	q2, d4,	d5
		vext.8		d8, d28, d29, #7
		vaddl.u8	q14, d28, d8
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q1, q0,	d7[0]
		vshl.i16	q1, q1,	#2
		vsub.i16	q14, q1, q14

loc_7214:				@ code xref: .text:000072c4j
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d0, d30, d31, #3
		vext.8		d1, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vext.8		d2, d30, d31, #1
		vext.8		d3, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vext.8		d4, d30, d31, #2
		vext.8		d5, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vext.8		d8, d30, d31, #7
		vaddl.u8	q15, d30, d8
		vmla.i16	q15, q2, d7[2]
		vmla.i16	q1, q0,	d7[0]
		vshl.i16	q1, q1,	#2
		vsub.i16	q15, q1, q15
		subs		r8, r8,	#1
		vaddl.s16	q4, d18, d28
		vaddl.s16	q5, d19, d29
		vaddl.s16	q6, d16, d30
		vaddl.s16	q7, d17, d31
		vaddl.s16	q0, d22, d24
		vaddl.s16	q1, d23, d25
		vaddl.s16	q2, d20, d26
		vaddl.s16	q8, d21, d27
		vmla.i32	q4, q0,	d7[0]
		vmla.i32	q5, q1,	d7[0]
		vmla.i32	q6, q2,	d7[1]
		vmla.i32	q7, q8,	d7[1]
		vshl.i32	q4, q4,	#2
		vshl.i32	q5, q5,	#2
		vmov		q8, q9
		vmov		q9, q10
		vmov		q10, q11
		vsub.i32	q4, q4,	q6
		vsub.i32	q5, q5,	q7
		vmov		q11, q12
		vmov		q12, q13
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vmov		q13, q14
		vmov		q14, q15
		vst1.8		{d8-d9}, [r12,:128],lr
		ble		loc_72c8
		b		loc_7214
@ ---------------------------------------------------------------------------

loc_72c8:				@ code xref: .text:000072c0j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_7038
		b		locret_76b4
@ ---------------------------------------------------------------------------

loc_72e4:				@ code xref: .text:00007034j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_72fc:				@ code xref: .text:00007050j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vmov		d17, d18
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q9, d20, d8
		vaddl.u8	q10, d22, d9
		vmla.i16	q9, q2,	d7[2]
		vmla.i16	q10, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q9, q1,	q9
		vsub.i16	q10, q6, q10
		vmov		d19, d20
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q10, d24, d8
		vaddl.u8	q11, d26, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vmov		d21, d22
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vaddl.u8	q0, d0,	d1
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vaddl.u8	q1, d2,	d3
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vaddl.u8	q2, d4,	d5
		vext.8		d8, d28, d29, #7
		vaddl.u8	q14, d28, d8
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q1, q0,	d7[0]
		vshl.i16	q1, q1,	#2
		vsub.i16	q14, q1, q14
		vmov		d22, d28

loc_74cc:				@ code xref: .text:00007698j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q14, d28, d8
		vaddl.u8	q15, d30, d9
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q15, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q14, q1, q14
		vsub.i16	q15, q6, q15
		vmov		d23, d28
		vmov		d24, d30
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q14, d28, d8
		vaddl.u8	q15, d30, d9
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q15, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q14, q1, q14
		vsub.i16	q15, q6, q15
		vmov		d25, d28
		vmov		d26, d30
		subs		r8, r8,	#4
		vmov		r9, r10, d26
		vaddl.s16	q0, d16, d23
		vaddl.s16	q1, d17, d24
		vaddl.s16	q2, d18, d25
		vaddl.s16	q4, d19, d26
		vaddl.s16	q5, d18, d21
		vaddl.s16	q6, d19, d22
		vaddl.s16	q7, d20, d23
		vaddl.s16	q14, d21, d24
		vmla.i32	q0, q5,	d7[1]
		vmla.i32	q1, q6,	d7[1]
		vmla.i32	q2, q7,	d7[1]
		vmla.i32	q4, q14, d7[1]
		vaddl.s16	q5, d17, d22
		vaddl.s16	q6, d18, d23
		vaddl.s16	q7, d19, d24
		vaddl.s16	q14, d20, d25
		vaddl.s16	q15, d19, d20
		vaddl.s16	q8, d20, d21
		vaddl.s16	q9, d21, d22
		vaddl.s16	q13, d22, d23
		vmla.i32	q5, q15, d7[0]
		vmla.i32	q6, q8,	d7[0]
		vmla.i32	q7, q9,	d7[0]
		vmla.i32	q14, q13, d7[0]
		vshl.i32	q5, q5,	#2
		vshl.i32	q6, q6,	#2
		vshl.i32	q7, q7,	#2
		vshl.i32	q14, q14, #2
		vsub.i32	q5, q5,	q0
		vsub.i32	q6, q6,	q1
		vsub.i32	q7, q7,	q2
		vsub.i32	q14, q14, q4
		vshrn.i32	d8, q5,	#6
		vshrn.i32	d9, q6,	#6
		vshrn.i32	d10, q7, #6
		vshrn.i32	d11, q14, #6
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		d22, r9, r10
		vst1.8		{d8}, [r12,:64],lr
		vst1.8		{d9}, [r12,:64],lr
		vst1.8		{d10}, [r12,:64],lr
		vst1.8		{d11}, [r12,:64],lr
		ble		loc_769c
		b		loc_74cc
@ ---------------------------------------------------------------------------

loc_769c:				@ code xref: .text:00007694j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_7038

locret_76b4:				@ code xref: .text:000072e0j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H2V3_neon
_VO_avg_nornd_MC_InterLuma_H2V3_neon:
		stmfd		sp!, {r4-r10,lr}
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_79c4

loc_76f4:				@ code xref: .text:000079bcj
					@ .text:00007d18j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_79d8
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q10, d20, d8
		vaddl.u8	q11, d22, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q12, d24, d8
		vaddl.u8	q13, d26, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13

loc_788c:				@ code xref: .text:000079a4j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q14, d28, d8
		vaddl.u8	q15, d30, d9
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q15, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q14, q1, q14
		vsub.i16	q15, q6, q15
		vsubl.s16	q4, d16, d28
		vsubl.s16	q5, d17, d29
		vsubl.s16	q6, d18, d30
		vsubl.s16	q7, d19, d31
		vmlsl.s16	q4, d18, d6[1]
		vmlsl.s16	q5, d19, d6[1]
		vmlsl.s16	q6, d20, d6[1]
		vmlsl.s16	q7, d21, d6[1]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlsl.s16	q4, d24, d7[0]
		vmlsl.s16	q5, d25, d7[0]
		vmlsl.s16	q6, d26, d7[0]
		vmlsl.s16	q7, d27, d7[0]
		vmlal.s16	q4, d26, d6[0]
		vmlal.s16	q5, d27, d6[0]
		vmlal.s16	q6, d28, d6[0]
		vmlal.s16	q7, d29, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d12, q6, #6
		vshrn.i32	d13, q7, #6
		vst1.8		{d8-d9}, [r12,:128],lr
		vst1.8		{d12-d13}, [r12,:128],lr
		ble		loc_79a8
		b		loc_788c
@ ---------------------------------------------------------------------------

loc_79a8:				@ code xref: .text:000079a0j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_76f4
		b		locret_7d1c
@ ---------------------------------------------------------------------------

loc_79c4:				@ code xref: .text:000076f0j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_79d8:				@ code xref: .text:00007708j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vmov		d17, d18
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q9, d20, d8
		vaddl.u8	q10, d22, d9
		vmla.i16	q9, q2,	d7[2]
		vmla.i16	q10, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q9, q1,	q9
		vsub.i16	q10, q6, q10
		vmov		d19, d20
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q10, d24, d8
		vaddl.u8	q11, d26, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vmov		d21, d22

loc_7b64:				@ code xref: .text:00007d00j
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q11, d24, d8
		vaddl.u8	q12, d26, d9
		vmla.i16	q11, q2, d7[2]
		vmla.i16	q12, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q11, q1, q11
		vsub.i16	q12, q6, q12
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q12, d28, d8
		vaddl.u8	q13, d30, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13
		vmov		d25, d26
		vsubl.s16	q4, d16, d22
		vsubl.s16	q5, d17, d23
		vsubl.s16	q6, d18, d24
		vsubl.s16	q7, d19, d25
		vmlsl.s16	q4, d17, d6[1]
		vmlsl.s16	q5, d18, d6[1]
		vmlsl.s16	q6, d19, d6[1]
		vmlsl.s16	q7, d20, d6[1]
		vmlal.s16	q4, d18, d6[3]
		vmlal.s16	q5, d19, d6[3]
		vmlal.s16	q6, d20, d6[3]
		vmlal.s16	q7, d21, d6[3]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d21, d6[0]
		vmlal.s16	q5, d22, d6[0]
		vmlal.s16	q6, d23, d6[0]
		vmlal.s16	q7, d24, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d12, q6, #6
		vshrn.i32	d13, q7, #6
		vst1.8		{d8}, [r12,:64],lr
		vst1.8		{d9}, [r12,:64],lr
		vst1.8		{d12}, [r12,:64],lr
		vst1.8		{d13}, [r12,:64],lr
		ble		loc_7d04
		b		loc_7b64
@ ---------------------------------------------------------------------------

loc_7d04:				@ code xref: .text:00007cfcj
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_76f4

locret_7d1c:				@ code xref: .text:000079c0j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H3V1_neon
_VO_avg_nornd_MC_InterLuma_H3V1_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_8000

loc_7d6c:				@ code xref: .text:00007ff8j
					@ .text:00008308j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#2
		subs		r7, r7,	#4
		ble		loc_8018
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7

loc_7ed8:				@ code xref: .text:00007fe0j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vsubl.s16	q4, d28, d16
		vsubl.s16	q5, d29, d17
		vsubl.s16	q6, d30, d18
		vsubl.s16	q7, d31, d19
		vmlal.s16	q4, d18, d6[0]
		vmlal.s16	q5, d19, d6[0]
		vmlal.s16	q6, d20, d6[0]
		vmlal.s16	q7, d21, d6[0]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlal.s16	q4, d24, d6[3]
		vmlal.s16	q5, d25, d6[3]
		vmlal.s16	q6, d26, d6[3]
		vmlal.s16	q7, d27, d6[3]
		vmlsl.s16	q4, d26, d6[1]
		vmlsl.s16	q5, d27, d6[1]
		vmlsl.s16	q6, d28, d6[1]
		vmlsl.s16	q7, d29, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vst1.8		{d8-d9}, [r12,:128],lr
		vst1.8		{d10-d11}, [r12,:128],lr
		ble		loc_7fe4
		b		loc_7ed8
@ ---------------------------------------------------------------------------

loc_7fe4:				@ code xref: .text:00007fdcj
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_7d6c
		b		locret_830c
@ ---------------------------------------------------------------------------

loc_8000:				@ code xref: .text:00007d68j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#2
		sub		r7, r7,	#4

loc_8018:				@ code xref: .text:00007d84j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22

loc_8174:				@ code xref: .text:000082f0j
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q11, d8, #2
		vshll.u8	q12, d10, #2
		vadd.i16	q11, q11, q6
		vadd.i16	q12, q12, q7
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vmov		d25, d26
		vsubl.s16	q4, d22, d16
		vsubl.s16	q5, d23, d17
		vsubl.s16	q6, d24, d18
		vsubl.s16	q7, d25, d19
		vmlal.s16	q4, d17, d6[0]
		vmlal.s16	q5, d18, d6[0]
		vmlal.s16	q6, d19, d6[0]
		vmlal.s16	q7, d20, d6[0]
		vmlsl.s16	q4, d18, d7[0]
		vmlsl.s16	q5, d19, d7[0]
		vmlsl.s16	q6, d20, d7[0]
		vmlsl.s16	q7, d21, d7[0]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlsl.s16	q4, d21, d6[1]
		vmlsl.s16	q5, d22, d6[1]
		vmlsl.s16	q6, d23, d6[1]
		vmlsl.s16	q7, d24, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vst1.8		{d8}, [r12,:64],lr
		vst1.8		{d9}, [r12,:64],lr
		vst1.8		{d10}, [r12,:64],lr
		vst1.8		{d11}, [r12,:64],lr
		ble		loc_82f4
		b		loc_8174
@ ---------------------------------------------------------------------------

loc_82f4:				@ code xref: .text:000082ecj
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_7d6c

locret_830c:				@ code xref: .text:00007ffcj
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H3V2_neon
_VO_avg_nornd_MC_InterLuma_H3V2_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_85c8

loc_835c:				@ code xref: .text:000085c0j
					@ .text:00008938j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#2
		subs		r7, r7,	#4
		ble		loc_85e0
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vext.8		d8, d28, d29, #6
		vsubl.u8	q6, d28, d8
		vext.8		d8, d28, d29, #1
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d28, d29, #2
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d28, d29, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d28, d29, #4
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d28, d29, #5
		vshll.u8	q14, d8, #2
		vadd.i16	q14, q14, q6

loc_8500:				@ code xref: .text:000085a8j
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d30, d31, #6
		vsubl.u8	q6, d30, d8
		vext.8		d8, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d30, d31, #5
		vshll.u8	q15, d8, #2
		vadd.i16	q15, q15, q6
		subs		r8, r8,	#1
		vaddl.s16	q4, d18, d28
		vaddl.s16	q5, d19, d29
		vaddl.s16	q6, d22, d24
		vaddl.s16	q7, d23, d25
		vmla.i32	q4, q6,	d7[0]
		vmla.i32	q5, q7,	d7[0]
		vaddl.s16	q6, d16, d30
		vaddl.s16	q7, d17, d31
		vaddl.s16	q2, d20, d26
		vaddl.s16	q8, d21, d27
		vmla.i32	q6, q2,	d7[1]
		vmla.i32	q7, q8,	d7[1]
		vshl.i32	q4, q4,	#2
		vshl.i32	q5, q5,	#2
		vmov		q8, q9
		vmov		q9, q10
		vmov		q10, q11
		vsub.i32	q4, q4,	q6
		vsub.i32	q5, q5,	q7
		vmov		q11, q12
		vmov		q12, q13
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vmov		q13, q14
		vmov		q14, q15
		vst1.8		{d8-d9}, [r12,:128],lr
		ble		loc_85ac
		b		loc_8500
@ ---------------------------------------------------------------------------

loc_85ac:				@ code xref: .text:000085a4j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_835c
		b		locret_893c
@ ---------------------------------------------------------------------------

loc_85c8:				@ code xref: .text:00008358j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#2
		sub		r7, r7,	#4

loc_85e0:				@ code xref: .text:00008374j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22
		vext.8		d8, d28, d29, #6
		vsubl.u8	q6, d28, d8
		vext.8		d8, d28, d29, #1
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d28, d29, #2
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d28, d29, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d28, d29, #4
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d28, d29, #5
		vshll.u8	q14, d8, #2
		vadd.i16	q14, q14, q6
		vmov		d22, d28

loc_8778:				@ code xref: .text:00008920j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vmov		d23, d28
		vmov		d24, d30
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vmov		d25, d28
		vmov		d26, d30
		subs		r8, r8,	#4
		vaddl.s16	q4, d16, d23
		vaddl.s16	q5, d18, d21
		vmla.i32	q4, q5,	d7[1]
		vaddl.s16	q5, d17, d22
		vaddl.s16	q6, d19, d20
		vmla.i32	q5, q6,	d7[0]
		vshl.i32	q5, q5,	#2
		vsub.i32	q4, q5,	q4
		vaddl.s16	q5, d17, d24
		vaddl.s16	q6, d19, d22
		vmla.i32	q5, q6,	d7[1]
		vaddl.s16	q6, d18, d23
		vaddl.s16	q7, d20, d21
		vmla.i32	q6, q7,	d7[0]
		vshl.i32	q6, q6,	#2
		vsub.i32	q5, q6,	q5
		vaddl.s16	q6, d18, d25
		vaddl.s16	q7, d20, d23
		vmla.i32	q6, q7,	d7[1]
		vaddl.s16	q7, d19, d24
		vaddl.s16	q8, d21, d22
		vmla.i32	q7, q8,	d7[0]
		vshl.i32	q7, q7,	#2
		vsub.i32	q6, q7,	q6
		vaddl.s16	q7, d19, d26
		vaddl.s16	q8, d21, d24
		vmla.i32	q7, q8,	d7[1]
		vaddl.s16	q8, d20, d25
		vaddl.s16	q15, d22, d23
		vmla.i32	q8, q15, d7[0]
		vshl.i32	q8, q8,	#2
		vsub.i32	q7, q8,	q7
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		d22, d26
		vst1.8		{d8}, [r12,:64],lr
		vst1.8		{d9}, [r12,:64],lr
		vst1.8		{d10}, [r12,:64],lr
		vst1.8		{d11}, [r12,:64],lr
		ble		loc_8924
		b		loc_8778
@ ---------------------------------------------------------------------------

loc_8924:				@ code xref: .text:0000891cj
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_835c

locret_893c:				@ code xref: .text:000085c4j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_nornd_MC_InterLuma_H3V3_neon
_VO_avg_nornd_MC_InterLuma_H3V3_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_8c1c

loc_898c:				@ code xref: .text:00008c14j
					@ .text:00008f20j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#2
		subs		r7, r7,	#4
		ble		loc_8c30
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7

loc_8af4:				@ code xref: .text:00008bfcj
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vsubl.s16	q4, d16, d28
		vsubl.s16	q5, d17, d29
		vsubl.s16	q6, d18, d30
		vsubl.s16	q7, d19, d31
		vmlsl.s16	q4, d18, d6[1]
		vmlsl.s16	q5, d19, d6[1]
		vmlsl.s16	q6, d20, d6[1]
		vmlsl.s16	q7, d21, d6[1]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlsl.s16	q4, d24, d7[0]
		vmlsl.s16	q5, d25, d7[0]
		vmlsl.s16	q6, d26, d7[0]
		vmlsl.s16	q7, d27, d7[0]
		vmlal.s16	q4, d26, d6[0]
		vmlal.s16	q5, d27, d6[0]
		vmlal.s16	q6, d28, d6[0]
		vmlal.s16	q7, d29, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vst1.8		{d8-d9}, [r12,:128],lr
		vst1.8		{d10-d11}, [r12,:128],lr
		ble		loc_8c00
		b		loc_8af4
@ ---------------------------------------------------------------------------

loc_8c00:				@ code xref: .text:00008bf8j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_898c
		b		locret_8f24
@ ---------------------------------------------------------------------------

loc_8c1c:				@ code xref: .text:00008988j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#2
		sub		r7, r7,	#4

loc_8c30:				@ code xref: .text:000089a0j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22

loc_8d8c:				@ code xref: .text:00008f08j
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q11, d8, #2
		vshll.u8	q12, d10, #2
		vadd.i16	q11, q11, q6
		vadd.i16	q12, q12, q7
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vmov		d25, d26
		vsubl.s16	q4, d16, d22
		vsubl.s16	q5, d17, d23
		vsubl.s16	q6, d18, d24
		vsubl.s16	q7, d19, d25
		vmlsl.s16	q4, d17, d6[1]
		vmlsl.s16	q5, d18, d6[1]
		vmlsl.s16	q6, d19, d6[1]
		vmlsl.s16	q7, d20, d6[1]
		vmlal.s16	q4, d18, d6[3]
		vmlal.s16	q5, d19, d6[3]
		vmlal.s16	q6, d20, d6[3]
		vmlal.s16	q7, d21, d6[3]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d21, d6[0]
		vmlal.s16	q5, d22, d6[0]
		vmlal.s16	q6, d23, d6[0]
		vmlal.s16	q7, d24, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vst1.8		{d8}, [r12,:64],lr
		vst1.8		{d9}, [r12,:64],lr
		vst1.8		{d10}, [r12,:64],lr
		vst1.8		{d11}, [r12,:64],lr
		ble		loc_8f0c
		b		loc_8d8c
@ ---------------------------------------------------------------------------

loc_8f0c:				@ code xref: .text:00008f04j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_898c

locret_8f24:				@ code xref: .text:00008c18j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H0V0_neon
_VO_avg_MC_InterLuma_H0V0_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]

loc_8f3c:				@ code xref: .text:00008ff8j
					@ .text:000090c8j ...
		cmp		r7, #0x20 @ ' '
		blt		loc_9000
		ands		r5, r0,	#0xf
		bne		loc_9000
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_8f58:				@ code xref: .text:00008fe0j
		vld1.64		{d8-d11}, [r4],r3
		vld1.64		{d12-d15}, [r4],r3
		vshll.u8	q0, d8,	#6
		vshll.u8	q1, d9,	#6
		vshll.u8	q2, d10, #6
		vshll.u8	q3, d11, #6
		vshll.u8	q4, d12, #6
		vshll.u8	q5, d13, #6
		vshll.u8	q6, d14, #6
		vshll.u8	q7, d15, #6
		subs		r8, r8,	#2
		vld1.64		{d16-d19}, [r12,:128]!
		vld1.64		{d20-d23}, [r12,:128],lr
		vld1.64		{d28-d31}, [r12,:128]
		sub		r12, r12, #0x20
		vld1.64		{d24-d27}, [r12,:128],lr
		vhadd.s16	q0, q0,	q8
		vhadd.s16	q1, q1,	q9
		vhadd.s16	q2, q2,	q10
		vhadd.s16	q3, q3,	q11
		vhadd.s16	q4, q4,	q12
		vhadd.s16	q5, q5,	q13
		vhadd.s16	q6, q6,	q14
		vhadd.s16	q7, q7,	q15
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vqrshrun.s16	d2, q2,	#6
		vqrshrun.s16	d3, q3,	#6
		vqrshrun.s16	d4, q4,	#6
		vqrshrun.s16	d5, q5,	#6
		vqrshrun.s16	d6, q6,	#6
		vqrshrun.s16	d7, q7,	#6
		vst1.64		{d0-d3}, [r5,:128],r1
		vst1.64		{d4-d7}, [r5,:128],r1
		bgt		loc_8f58
		mls		r12, lr, r6, r12
		add		r12, r12, #0x40
		add		r2, r2,	#0x20
		add		r0, r0,	#0x20
		subs		r7, r7,	#0x20
		bgt		loc_8f3c
		b		locret_926c
@ ---------------------------------------------------------------------------

loc_9000:				@ code xref: .text:00008f40j
					@ .text:00008f48j
		cmp		r7, #0x10
		blt		loc_90d0
		ands		r5, r0,	#0xf
		bne		loc_90d0
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_901c:				@ code xref: .text:000090b0j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d4-d5}, [r4],r3
		vld1.64		{d6-d7}, [r4],r3
		subs		r8, r8,	#4
		vshll.u8	q8, d0,	#6
		vshll.u8	q9, d1,	#6
		vshll.u8	q10, d2, #6
		vshll.u8	q11, d3, #6
		vshll.u8	q12, d4, #6
		vshll.u8	q13, d5, #6
		vshll.u8	q14, d6, #6
		vshll.u8	q15, d7, #6
		vld1.64		{d0-d3}, [r12,:128],lr
		vld1.64		{d4-d7}, [r12,:128],lr
		vld1.64		{d8-d11}, [r12,:128],lr
		vld1.64		{d12-d15}, [r12,:128],lr
		vhadd.s16	q0, q0,	q8
		vhadd.s16	q1, q1,	q9
		vhadd.s16	q2, q2,	q10
		vhadd.s16	q3, q3,	q11
		vhadd.s16	q4, q4,	q12
		vhadd.s16	q5, q5,	q13
		vhadd.s16	q6, q6,	q14
		vhadd.s16	q7, q7,	q15
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vqrshrun.s16	d2, q2,	#6
		vqrshrun.s16	d3, q3,	#6
		vqrshrun.s16	d4, q4,	#6
		vqrshrun.s16	d5, q5,	#6
		vqrshrun.s16	d6, q6,	#6
		vqrshrun.s16	d7, q7,	#6
		vst1.64		{d0-d1}, [r5,:128],r1
		vst1.64		{d2-d3}, [r5,:128],r1
		vst1.64		{d4-d5}, [r5,:128],r1
		vst1.64		{d6-d7}, [r5,:128],r1
		bgt		loc_901c
		mls		r12, lr, r6, r12
		add		r12, r12, #0x20
		add		r2, r2,	#0x10
		add		r0, r0,	#0x10
		subs		r7, r7,	#0x10
		bgt		loc_8f3c
		b		locret_926c
@ ---------------------------------------------------------------------------

loc_90d0:				@ code xref: .text:00009004j
					@ .text:0000900cj
		cmp		r7, #8
		blt		loc_9174
		ands		r5, r0,	#7
		bne		loc_9174
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_90ec:				@ code xref: .text:00009154j
		vld1.64		{d0}, [r4],r3
		vld1.64		{d1}, [r4],r3
		vld1.64		{d2}, [r4],r3
		vld1.64		{d3}, [r4],r3
		subs		r8, r8,	#4
		vshll.u8	q8, d0,	#6
		vshll.u8	q9, d1,	#6
		vshll.u8	q10, d2, #6
		vshll.u8	q11, d3, #6
		vld1.64		{d0-d1}, [r12,:128],lr
		vld1.64		{d2-d3}, [r12,:128],lr
		vld1.64		{d4-d5}, [r12,:128],lr
		vld1.64		{d6-d7}, [r12,:128],lr
		vhadd.s16	q0, q0,	q8
		vhadd.s16	q1, q1,	q9
		vhadd.s16	q2, q2,	q10
		vhadd.s16	q3, q3,	q11
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vqrshrun.s16	d2, q2,	#6
		vqrshrun.s16	d3, q3,	#6
		vst1.64		{d0}, [r5,:64],r1
		vst1.64		{d1}, [r5,:64],r1
		blt		loc_9158
		vst1.64		{d2}, [r5,:64],r1
		vst1.64		{d3}, [r5,:64],r1
		bgt		loc_90ec

loc_9158:				@ code xref: .text:00009148j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		add		r2, r2,	#8
		add		r0, r0,	#8
		subs		r7, r7,	#8
		bgt		loc_8f3c
		b		locret_926c
@ ---------------------------------------------------------------------------

loc_9174:				@ code xref: .text:000090d4j
					@ .text:000090dcj
		cmp		r7, #4
		blt		loc_9200
		ands		r5, r0,	#3
		bne		loc_9200
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_9190:				@ code xref: .text:000091e0j
		vld1.32		{d0[0]}, [r4],r3
		vld1.32		{d0[1]}, [r4],r3
		vld1.32		{d1[0]}, [r4],r3
		vld1.32		{d1[1]}, [r4],r3
		subs		r8, r8,	#4
		vshll.u8	q8, d0,	#6
		vshll.u8	q9, d1,	#6
		vld1.64		{d0}, [r12,:64],lr
		vld1.64		{d1}, [r12,:64],lr
		vld1.64		{d2}, [r12,:64],lr
		vld1.64		{d3}, [r12,:64],lr
		vhadd.s16	q0, q0,	q8
		vhadd.s16	q1, q1,	q9
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d0[1]}, [r5,:32],r1
		blt		loc_91e4
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d1[1]}, [r5,:32],r1
		bgt		loc_9190

loc_91e4:				@ code xref: .text:000091d4j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		subs		r7, r7,	#4
		bgt		loc_8f3c
		b		locret_926c
@ ---------------------------------------------------------------------------

loc_9200:				@ code xref: .text:00009178j
					@ .text:00009180j
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_920c:				@ code xref: .text:00009250j
		vld1.16		{d0[0]}, [r4],r3
		vld1.16		{d0[1]}, [r4],r3
		vld1.16		{d0[2]}, [r4],r3
		vld1.16		{d0[3]}, [r4],r3
		subs		r8, r8,	#4
		vshll.u8	q8, d0,	#6
		vld1.32		{d4[0]}, [r12,:32],lr
		vld1.32		{d4[1]}, [r12,:32],lr
		vld1.32		{d5[0]}, [r12,:32],lr
		vld1.32		{d5[1]}, [r12,:32],lr
		vhadd.s16	q2, q2,	q8
		vqrshrun.s16	d0, q2,	#6
		vst1.16		{d0[0]}, [r5,:16],r1
		vst1.16		{d0[1]}, [r5,:16],r1
		blt		loc_9254
		vst1.16		{d0[2]}, [r5,:16],r1
		vst1.16		{d0[3]}, [r5,:16],r1
		bgt		loc_920c

loc_9254:				@ code xref: .text:00009244j
		mls		r12, lr, r6, r12
		add		r12, r12, #4
		add		r2, r2,	#2
		add		r0, r0,	#2
		subs		r7, r7,	#2
		bgt		loc_8f3c

locret_926c:				@ code xref: .text:00008ffcj
					@ .text:000090ccj ...
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H1V0_neon
_VO_avg_MC_InterLuma_H1V0_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d12, #4
		vmov.i8		d13, #0xa
		vmov.i8		d14, #0x3a @ ':'
		vmov.i8		d15, #0x11
		vmov.i8		d30, #5
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_93e8

loc_92a0:				@ code xref: .text:000093e0j
					@ .text:00009524j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#3
		subs		r7, r7,	#4
		ble		loc_93f8

loc_92b4:				@ code xref: .text:000093c8j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #6
		vext.8		d6, d2,	d3, #6
		vsubl.u8	q2, d4,	d0
		vsubl.u8	q3, d6,	d2
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #2
		vext.8		d10, d2, d3, #1
		vext.8		d11, d2, d3, #2
		vmlal.u8	q2, d8,	d12
		vmlsl.u8	q2, d9,	d13
		vmlal.u8	q3, d10, d12
		vmlsl.u8	q3, d11, d13
		vext.8		d8, d0,	d1, #3
		vext.8		d9, d0,	d1, #4
		vext.8		d10, d2, d3, #3
		vext.8		d11, d2, d3, #4
		vmlal.u8	q2, d8,	d14
		vmlal.u8	q2, d9,	d15
		vmlal.u8	q3, d10, d14
		vmlal.u8	q3, d11, d15
		vext.8		d8, d0,	d1, #5
		vext.8		d9, d2,	d3, #5
		vmlsl.u8	q2, d8,	d30
		vmlsl.u8	q3, d9,	d30
		vext.8		d20, d16, d17, #6
		vext.8		d22, d18, d19, #6
		vsubl.u8	q10, d20, d16
		vsubl.u8	q11, d22, d18
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #2
		vext.8		d26, d18, d19, #1
		vext.8		d27, d18, d19, #2
		vmlal.u8	q10, d24, d12
		vmlsl.u8	q10, d25, d13
		vmlal.u8	q11, d26, d12
		vmlsl.u8	q11, d27, d13
		vext.8		d28, d16, d17, #3
		vext.8		d29, d16, d17, #4
		vext.8		d24, d18, d19, #3
		vext.8		d25, d18, d19, #4
		vmlal.u8	q10, d28, d14
		vmlal.u8	q10, d29, d15
		vmlal.u8	q11, d24, d14
		vmlal.u8	q11, d25, d15
		vext.8		d28, d16, d17, #5
		vext.8		d29, d18, d19, #5
		vmlsl.u8	q10, d28, d30
		vmlsl.u8	q11, d29, d30
		vld1.16		{d0-d1}, [r12,:128],lr
		vld1.16		{d2-d3}, [r12,:128],lr
		vld1.16		{d16-d17}, [r12,:128],lr
		vld1.16		{d18-d19}, [r12,:128],lr
		vhadd.s16	q0, q0,	q2
		vhadd.s16	q1, q1,	q3
		vhadd.s16	q8, q8,	q10
		vhadd.s16	q9, q9,	q11
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vqrshrun.s16	d2, q8,	#6
		vqrshrun.s16	d3, q9,	#6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		bne		loc_92b4
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_92a0
		b		locret_9528
@ ---------------------------------------------------------------------------

loc_93e8:				@ code xref: .text:0000929cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#3
		sub		r7, r7,	#4

loc_93f8:				@ code xref: .text:000092b0j
					@ .text:0000950cj
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #6
		vext.8		d6, d2,	d3, #6
		vsubl.u8	q2, d4,	d0
		vsubl.u8	q3, d6,	d2
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #2
		vext.8		d10, d2, d3, #1
		vext.8		d11, d2, d3, #2
		vmlal.u8	q2, d8,	d12
		vmlsl.u8	q2, d9,	d13
		vmlal.u8	q3, d10, d12
		vmlsl.u8	q3, d11, d13
		vext.8		d8, d0,	d1, #3
		vext.8		d9, d0,	d1, #4
		vext.8		d10, d2, d3, #3
		vext.8		d11, d2, d3, #4
		vmlal.u8	q2, d8,	d14
		vmlal.u8	q2, d9,	d15
		vmlal.u8	q3, d10, d14
		vmlal.u8	q3, d11, d15
		vext.8		d8, d0,	d1, #5
		vext.8		d9, d2,	d3, #5
		vmlsl.u8	q2, d8,	d30
		vmlsl.u8	q3, d9,	d30
		vext.8		d20, d16, d17, #6
		vext.8		d22, d18, d19, #6
		vsubl.u8	q10, d20, d16
		vsubl.u8	q11, d22, d18
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #2
		vext.8		d26, d18, d19, #1
		vext.8		d27, d18, d19, #2
		vmlal.u8	q10, d24, d12
		vmlsl.u8	q10, d25, d13
		vmlal.u8	q11, d26, d12
		vmlsl.u8	q11, d27, d13
		vext.8		d28, d16, d17, #3
		vext.8		d29, d16, d17, #4
		vext.8		d24, d18, d19, #3
		vext.8		d25, d18, d19, #4
		vmlal.u8	q10, d28, d14
		vmlal.u8	q10, d29, d15
		vmlal.u8	q11, d24, d14
		vmlal.u8	q11, d25, d15
		vext.8		d28, d16, d17, #5
		vext.8		d29, d18, d19, #5
		vmlsl.u8	q10, d28, d30
		vmlsl.u8	q11, d29, d30
		vld1.16		{d0-d1}, [r12,:64],lr
		vld1.16		{d2-d3}, [r12,:64],lr
		vld1.16		{d16-d17}, [r12,:64],lr
		vld1.16		{d18-d19}, [r12,:64],lr
		vhadd.s16	q0, q0,	q2
		vhadd.s16	q1, q1,	q3
		vhadd.s16	q8, q8,	q10
		vhadd.s16	q9, q9,	q11
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vqrshrun.s16	d2, q8,	#6
		vqrshrun.s16	d3, q9,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		bne		loc_93f8
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_92a0

locret_9528:				@ code xref: .text:000093e4j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H2V0_neon
_VO_avg_MC_InterLuma_H2V0_neon:
		stmfd		sp!, {r4-r10,lr}
		mov		r6, #4
		movt  r6, 0xb
		mov		r7, #0xa
		vmov		d6, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_96d0

loc_9558:				@ code xref: .text:000096c8j
					@ .text:0000983cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#3
		subs		r7, r7,	#4
		ble		loc_96e0

loc_956c:				@ code xref: .text:000096b0j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d0,	d1, #4
		vext.8		d12, d2, d3, #3
		vext.8		d13, d2, d3, #4
		vext.8		d22, d16, d17, #3
		vext.8		d23, d16, d17, #4
		vext.8		d28, d18, d19, #3
		vext.8		d29, d18, d19, #4
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q6, d12, d13
		vaddl.u8	q11, d22, d23
		vaddl.u8	q14, d28, d29
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #6
		vext.8		d14, d2, d3, #1
		vext.8		d15, d2, d3, #6
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #6
		vext.8		d30, d18, d19, #1
		vext.8		d31, d18, d19, #6
		vaddl.u8	q4, d8,	d9
		vaddl.u8	q7, d14, d15
		vaddl.u8	q12, d24, d25
		vaddl.u8	q15, d30, d31
		vmla.i16	q4, q2,	d6[2]
		vmla.i16	q7, q6,	d6[2]
		vmla.i16	q12, q11, d6[2]
		vmla.i16	q15, q14, d6[2]
		vext.8		d10, d0, d1, #2
		vext.8		d11, d0, d1, #5
		vext.8		d20, d2, d3, #2
		vext.8		d21, d2, d3, #5
		vext.8		d26, d16, d17, #2
		vext.8		d27, d16, d17, #5
		vext.8		d28, d18, d19, #2
		vext.8		d29, d18, d19, #5
		vaddl.u8	q5, d10, d11
		vaddl.u8	q10, d20, d21
		vaddl.u8	q13, d26, d27
		vaddl.u8	q14, d28, d29
		vext.8		d5, d0,	d1, #7
		vext.8		d13, d2, d3, #7
		vext.8		d23, d16, d17, #7
		vext.8		d19, d18, d19, #7
		vaddl.u8	q2, d0,	d5
		vaddl.u8	q6, d2,	d13
		vaddl.u8	q11, d16, d23
		vaddl.u8	q9, d18, d19
		vshl.i16	q4, q4,	#2
		vshl.i16	q7, q7,	#2
		vshl.i16	q12, q12, #2
		vshl.i16	q15, q15, #2
		vmla.i16	q2, q5,	d6[1]
		vmla.i16	q6, q10, d6[1]
		vmla.i16	q11, q13, d6[1]
		vmla.i16	q9, q14, d6[1]
		vsub.i16	q4, q4,	q2
		vsub.i16	q5, q7,	q6
		vsub.i16	q8, q12, q11
		vsub.i16	q9, q15, q9
		vld1.16		{d0-d1}, [r12,:128],lr
		vld1.16		{d2-d3}, [r12,:128],lr
		vld1.16		{d20-d21}, [r12,:128],lr
		vld1.16		{d22-d23}, [r12,:128],lr
		vhadd.s16	q0, q0,	q4
		vhadd.s16	q1, q1,	q5
		vhadd.s16	q10, q10, q8
		vhadd.s16	q11, q11, q9
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vqrshrun.s16	d2, q10, #6
		vqrshrun.s16	d3, q11, #6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		bne		loc_956c
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_9558
		b		locret_9840
@ ---------------------------------------------------------------------------

loc_96d0:				@ code xref: .text:00009554j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#3
		sub		r7, r7,	#4

loc_96e0:				@ code xref: .text:00009568j
					@ .text:00009824j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d0,	d1, #4
		vext.8		d12, d2, d3, #3
		vext.8		d13, d2, d3, #4
		vext.8		d22, d16, d17, #3
		vext.8		d23, d16, d17, #4
		vext.8		d28, d18, d19, #3
		vext.8		d29, d18, d19, #4
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q6, d12, d13
		vaddl.u8	q11, d22, d23
		vaddl.u8	q14, d28, d29
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #6
		vext.8		d14, d2, d3, #1
		vext.8		d15, d2, d3, #6
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #6
		vext.8		d30, d18, d19, #1
		vext.8		d31, d18, d19, #6
		vaddl.u8	q4, d8,	d9
		vaddl.u8	q7, d14, d15
		vaddl.u8	q12, d24, d25
		vaddl.u8	q15, d30, d31
		vmla.i16	q4, q2,	d6[2]
		vmla.i16	q7, q6,	d6[2]
		vmla.i16	q12, q11, d6[2]
		vmla.i16	q15, q14, d6[2]
		vext.8		d10, d0, d1, #2
		vext.8		d11, d0, d1, #5
		vext.8		d20, d2, d3, #2
		vext.8		d21, d2, d3, #5
		vext.8		d26, d16, d17, #2
		vext.8		d27, d16, d17, #5
		vext.8		d28, d18, d19, #2
		vext.8		d29, d18, d19, #5
		vaddl.u8	q5, d10, d11
		vaddl.u8	q10, d20, d21
		vaddl.u8	q13, d26, d27
		vaddl.u8	q14, d28, d29
		vext.8		d5, d0,	d1, #7
		vext.8		d13, d2, d3, #7
		vext.8		d23, d16, d17, #7
		vext.8		d19, d18, d19, #7
		vaddl.u8	q2, d0,	d5
		vaddl.u8	q6, d2,	d13
		vaddl.u8	q11, d16, d23
		vaddl.u8	q9, d18, d19
		vshl.i16	q4, q4,	#2
		vshl.i16	q7, q7,	#2
		vshl.i16	q12, q12, #2
		vshl.i16	q15, q15, #2
		vmla.i16	q2, q5,	d6[1]
		vmla.i16	q6, q10, d6[1]
		vmla.i16	q11, q13, d6[1]
		vmla.i16	q9, q14, d6[1]
		vsub.i16	q4, q4,	q2
		vsub.i16	q5, q7,	q6
		vsub.i16	q8, q12, q11
		vsub.i16	q9, q15, q9
		vld1.16		{d0-d1}, [r12,:64],lr
		vld1.16		{d2-d3}, [r12,:64],lr
		vld1.16		{d20-d21}, [r12,:64],lr
		vld1.16		{d22-d23}, [r12,:64],lr
		vhadd.s16	q0, q0,	q4
		vhadd.s16	q1, q1,	q5
		vhadd.s16	q10, q10, q8
		vhadd.s16	q11, q11, q9
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vqrshrun.s16	d2, q10, #6
		vqrshrun.s16	d3, q11, #6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		bne		loc_96e0
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_9558

locret_9840:				@ code xref: .text:000096ccj
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H3V0_neon
_VO_avg_MC_InterLuma_H3V0_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d12, #5
		vmov.i8		d13, #0x11
		vmov.i8		d14, #0x3a @ ':'
		vmov.i8		d15, #0xa
		vmov.i8		d30, #4
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_99bc

loc_9874:				@ code xref: .text:000099b4j
					@ .text:00009af8j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#2
		subs		r7, r7,	#4
		ble		loc_99cc

loc_9888:				@ code xref: .text:0000999cj
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #6
		vext.8		d6, d2,	d3, #6
		vsubl.u8	q2, d0,	d4
		vsubl.u8	q3, d2,	d6
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #2
		vext.8		d10, d2, d3, #1
		vext.8		d11, d2, d3, #2
		vmlsl.u8	q2, d8,	d12
		vmlal.u8	q2, d9,	d13
		vmlsl.u8	q3, d10, d12
		vmlal.u8	q3, d11, d13
		vext.8		d8, d0,	d1, #3
		vext.8		d9, d0,	d1, #4
		vext.8		d10, d2, d3, #3
		vext.8		d11, d2, d3, #4
		vmlal.u8	q2, d8,	d14
		vmlsl.u8	q2, d9,	d15
		vmlal.u8	q3, d10, d14
		vmlsl.u8	q3, d11, d15
		vext.8		d8, d0,	d1, #5
		vext.8		d9, d2,	d3, #5
		vmlal.u8	q2, d8,	d30
		vmlal.u8	q3, d9,	d30
		vext.8		d20, d16, d17, #6
		vext.8		d22, d18, d19, #6
		vsubl.u8	q10, d16, d20
		vsubl.u8	q11, d18, d22
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #2
		vext.8		d26, d18, d19, #1
		vext.8		d27, d18, d19, #2
		vmlsl.u8	q10, d24, d12
		vmlal.u8	q10, d25, d13
		vmlsl.u8	q11, d26, d12
		vmlal.u8	q11, d27, d13
		vext.8		d28, d16, d17, #3
		vext.8		d29, d16, d17, #4
		vext.8		d24, d18, d19, #3
		vext.8		d25, d18, d19, #4
		vmlal.u8	q10, d28, d14
		vmlsl.u8	q10, d29, d15
		vmlal.u8	q11, d24, d14
		vmlsl.u8	q11, d25, d15
		vext.8		d28, d16, d17, #5
		vext.8		d29, d18, d19, #5
		vmlal.u8	q10, d28, d30
		vmlal.u8	q11, d29, d30
		vld1.16		{d8-d9}, [r12,:128],lr
		vld1.16		{d10-d11}, [r12,:128],lr
		vld1.16		{d16-d17}, [r12,:128],lr
		vld1.16		{d18-d19}, [r12,:128],lr
		vhadd.s16	q2, q2,	q4
		vhadd.s16	q3, q3,	q5
		vhadd.s16	q10, q10, q8
		vhadd.s16	q11, q11, q9
		vqrshrun.s16	d0, q2,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q10, #6
		vqrshrun.s16	d3, q11, #6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		bne		loc_9888
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_9874
		b		locret_9afc
@ ---------------------------------------------------------------------------

loc_99bc:				@ code xref: .text:00009870j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#2
		sub		r7, r7,	#4

loc_99cc:				@ code xref: .text:00009884j
					@ .text:00009ae0j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #6
		vext.8		d6, d2,	d3, #6
		vsubl.u8	q2, d0,	d4
		vsubl.u8	q3, d2,	d6
		vext.8		d8, d0,	d1, #1
		vext.8		d9, d0,	d1, #2
		vext.8		d10, d2, d3, #1
		vext.8		d11, d2, d3, #2
		vmlsl.u8	q2, d8,	d12
		vmlal.u8	q2, d9,	d13
		vmlsl.u8	q3, d10, d12
		vmlal.u8	q3, d11, d13
		vext.8		d8, d0,	d1, #3
		vext.8		d9, d0,	d1, #4
		vext.8		d10, d2, d3, #3
		vext.8		d11, d2, d3, #4
		vmlal.u8	q2, d8,	d14
		vmlsl.u8	q2, d9,	d15
		vmlal.u8	q3, d10, d14
		vmlsl.u8	q3, d11, d15
		vext.8		d8, d0,	d1, #5
		vext.8		d9, d2,	d3, #5
		vmlal.u8	q2, d8,	d30
		vmlal.u8	q3, d9,	d30
		vext.8		d20, d16, d17, #6
		vext.8		d22, d18, d19, #6
		vsubl.u8	q10, d16, d20
		vsubl.u8	q11, d18, d22
		vext.8		d24, d16, d17, #1
		vext.8		d25, d16, d17, #2
		vext.8		d26, d18, d19, #1
		vext.8		d27, d18, d19, #2
		vmlsl.u8	q10, d24, d12
		vmlal.u8	q10, d25, d13
		vmlsl.u8	q11, d26, d12
		vmlal.u8	q11, d27, d13
		vext.8		d28, d16, d17, #3
		vext.8		d29, d16, d17, #4
		vext.8		d24, d18, d19, #3
		vext.8		d25, d18, d19, #4
		vmlal.u8	q10, d28, d14
		vmlsl.u8	q10, d29, d15
		vmlal.u8	q11, d24, d14
		vmlsl.u8	q11, d25, d15
		vext.8		d28, d16, d17, #5
		vext.8		d29, d18, d19, #5
		vmlal.u8	q10, d28, d30
		vmlal.u8	q11, d29, d30
		vld1.16		{d8-d9}, [r12,:64],lr
		vld1.16		{d10-d11}, [r12,:64],lr
		vld1.16		{d16-d17}, [r12,:64],lr
		vld1.16		{d18-d19}, [r12,:64],lr
		vhadd.s16	q2, q2,	q4
		vhadd.s16	q3, q3,	q5
		vhadd.s16	q10, q10, q8
		vhadd.s16	q11, q11, q9
		vqrshrun.s16	d0, q2,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q10, #6
		vqrshrun.s16	d3, q11, #6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		bne		loc_99cc
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_9874

locret_9afc:				@ code xref: .text:000099b8j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H0V1_neon
_VO_avg_MC_InterLuma_H0V1_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d16, #0xa
		vmov.i8		d17, #0x3a @ ':'
		vmov.i8		d18, #0x11
		vmov.i8		d19, #5
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_9c50

loc_9b2c:				@ code xref: .text:00009c48j
					@ .text:00009d68j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		subs		r7, r7,	#4
		ble		loc_9c64
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3

loc_9b5c:				@ code xref: .text:00009c30j
		vld1.64		{d26}, [r4],r3
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		subs		r8, r8,	#4
		vsubl.u8	q0, d26, d20
		vshll.u8	q1, d21, #2
		vsubl.u8	q2, d27, d21
		vshll.u8	q3, d22, #2
		vsubl.u8	q4, d28, d22
		vshll.u8	q5, d23, #2
		vsubl.u8	q6, d29, d23
		vshll.u8	q7, d24, #2
		vadd.i16	q1, q1,	q0
		vadd.i16	q3, q3,	q2
		vadd.i16	q5, q5,	q4
		vadd.i16	q7, q7,	q6
		vmlsl.u8	q1, d22, d16
		vmlsl.u8	q3, d23, d16
		vmlsl.u8	q5, d24, d16
		vmlsl.u8	q7, d25, d16
		vmlal.u8	q1, d23, d17
		vmlal.u8	q3, d24, d17
		vmlal.u8	q5, d25, d17
		vmlal.u8	q7, d26, d17
		vmlal.u8	q1, d24, d18
		vmlal.u8	q3, d25, d18
		vmlal.u8	q5, d26, d18
		vmlal.u8	q7, d27, d18
		vmlsl.u8	q1, d25, d19
		vmlsl.u8	q3, d26, d19
		vmlsl.u8	q5, d27, d19
		vmlsl.u8	q7, d28, d19
		vld1.16		{d0-d1}, [r12,:128],lr
		vld1.16		{d4-d5}, [r12,:128],lr
		vld1.16		{d8-d9}, [r12,:128],lr
		vld1.16		{d12-d13}, [r12,:128],lr
		vhadd.s16	q1, q1,	q0
		vhadd.s16	q3, q3,	q2
		vhadd.s16	q5, q5,	q4
		vhadd.s16	q7, q7,	q6
		vqrshrun.s16	d0, q1,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q5,	#6
		vqrshrun.s16	d3, q7,	#6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		ble		loc_9c34
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		b		loc_9b5c
@ ---------------------------------------------------------------------------

loc_9c34:				@ code xref: .text:00009c20j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_9b2c
		b		locret_9d6c
@ ---------------------------------------------------------------------------

loc_9c50:				@ code xref: .text:00009b28j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r7, r7,	#4

loc_9c64:				@ code xref: .text:00009b40j
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3

loc_9c7c:				@ code xref: .text:00009d50j
		vld1.64		{d26}, [r4],r3
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		subs		r8, r8,	#4
		vsubl.u8	q0, d26, d20
		vshll.u8	q1, d21, #2
		vsubl.u8	q2, d27, d21
		vshll.u8	q3, d22, #2
		vsubl.u8	q4, d28, d22
		vshll.u8	q5, d23, #2
		vsubl.u8	q6, d29, d23
		vshll.u8	q7, d24, #2
		vadd.i16	q1, q1,	q0
		vadd.i16	q3, q3,	q2
		vadd.i16	q5, q5,	q4
		vadd.i16	q7, q7,	q6
		vmlsl.u8	q1, d22, d16
		vmlsl.u8	q3, d23, d16
		vmlsl.u8	q5, d24, d16
		vmlsl.u8	q7, d25, d16
		vmlal.u8	q1, d23, d17
		vmlal.u8	q3, d24, d17
		vmlal.u8	q5, d25, d17
		vmlal.u8	q7, d26, d17
		vmlal.u8	q1, d24, d18
		vmlal.u8	q3, d25, d18
		vmlal.u8	q5, d26, d18
		vmlal.u8	q7, d27, d18
		vmlsl.u8	q1, d25, d19
		vmlsl.u8	q3, d26, d19
		vmlsl.u8	q5, d27, d19
		vmlsl.u8	q7, d28, d19
		vld1.16		{d0-d1}, [r12,:64],lr
		vld1.16		{d4-d5}, [r12,:64],lr
		vld1.16		{d8-d9}, [r12,:64],lr
		vld1.16		{d12-d13}, [r12,:64],lr
		vhadd.s16	q1, q1,	q0
		vhadd.s16	q3, q3,	q2
		vhadd.s16	q5, q5,	q4
		vhadd.s16	q7, q7,	q6
		vqrshrun.s16	d0, q1,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q5,	#6
		vqrshrun.s16	d3, q7,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		ble		loc_9d54
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		b		loc_9c7c
@ ---------------------------------------------------------------------------

loc_9d54:				@ code xref: .text:00009d40j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_9b2c

locret_9d6c:				@ code xref: .text:00009c4cj
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H0V2_neon
_VO_avg_MC_InterLuma_H0V2_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i16	q8, #0xb
		vmov.i16	q9, #0x28 @ '('
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_9ed0

loc_9d94:				@ code xref: .text:00009ec8j
					@ .text:0000a000j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		subs		r7, r7,	#4
		ble		loc_9ee4
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3
		vld1.64		{d26}, [r4],r3

loc_9dc8:				@ code xref: .text:00009eb0j
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		vld1.64		{d30}, [r4],r3
		subs		r8, r8,	#4
		vaddl.u8	q0, d20, d27
		vaddl.u8	q1, d21, d26
		vaddl.u8	q2, d21, d28
		vaddl.u8	q3, d22, d27
		vaddl.u8	q4, d22, d29
		vaddl.u8	q5, d23, d28
		vaddl.u8	q6, d23, d30
		vaddl.u8	q7, d24, d29
		vshl.i16	q1, q1,	#2
		vshl.i16	q3, q3,	#2
		vshl.i16	q5, q5,	#2
		vshl.i16	q7, q7,	#2
		vsub.i16	q1, q1,	q0
		vsub.i16	q3, q3,	q2
		vsub.i16	q5, q5,	q4
		vsub.i16	q7, q7,	q6
		vaddl.u8	q0, d22, d25
		vaddl.u8	q2, d23, d26
		vaddl.u8	q4, d24, d27
		vaddl.u8	q6, d25, d28
		vmls.i16	q1, q0,	q8
		vmls.i16	q3, q2,	q8
		vmls.i16	q5, q4,	q8
		vmls.i16	q7, q6,	q8
		vaddl.u8	q0, d23, d24
		vaddl.u8	q2, d24, d25
		vaddl.u8	q4, d25, d26
		vaddl.u8	q6, d26, d27
		vmla.i16	q1, q0,	q9
		vmla.i16	q3, q2,	q9
		vmla.i16	q5, q4,	q9
		vmla.i16	q7, q6,	q9
		vld1.16		{d0-d1}, [r12,:128],lr
		vld1.16		{d4-d5}, [r12,:128],lr
		vld1.16		{d8-d9}, [r12,:128],lr
		vld1.16		{d12-d13}, [r12,:128],lr
		vhadd.s16	q1, q1,	q0
		vhadd.s16	q3, q3,	q2
		vhadd.s16	q5, q5,	q4
		vhadd.s16	q7, q7,	q6
		vqrshrun.s16	d0, q1,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q5,	#6
		vqrshrun.s16	d3, q7,	#6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		ble		loc_9eb4
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		d26, d30
		b		loc_9dc8
@ ---------------------------------------------------------------------------

loc_9eb4:				@ code xref: .text:00009e9cj
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_9d94
		b		locret_a004
@ ---------------------------------------------------------------------------

loc_9ed0:				@ code xref: .text:00009d90j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r7, r7,	#4

loc_9ee4:				@ code xref: .text:00009da8j
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3
		vld1.64		{d26}, [r4],r3

loc_9f00:				@ code xref: .text:00009fe8j
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		vld1.64		{d30}, [r4],r3
		subs		r8, r8,	#4
		vaddl.u8	q0, d20, d27
		vaddl.u8	q1, d21, d26
		vaddl.u8	q2, d21, d28
		vaddl.u8	q3, d22, d27
		vaddl.u8	q4, d22, d29
		vaddl.u8	q5, d23, d28
		vaddl.u8	q6, d23, d30
		vaddl.u8	q7, d24, d29
		vshl.i16	q1, q1,	#2
		vshl.i16	q3, q3,	#2
		vshl.i16	q5, q5,	#2
		vshl.i16	q7, q7,	#2
		vsub.i16	q1, q1,	q0
		vsub.i16	q3, q3,	q2
		vsub.i16	q5, q5,	q4
		vsub.i16	q7, q7,	q6
		vaddl.u8	q0, d22, d25
		vaddl.u8	q2, d23, d26
		vaddl.u8	q4, d24, d27
		vaddl.u8	q6, d25, d28
		vmls.i16	q1, q0,	q8
		vmls.i16	q3, q2,	q8
		vmls.i16	q5, q4,	q8
		vmls.i16	q7, q6,	q8
		vaddl.u8	q0, d23, d24
		vaddl.u8	q2, d24, d25
		vaddl.u8	q4, d25, d26
		vaddl.u8	q6, d26, d27
		vmla.i16	q1, q0,	q9
		vmla.i16	q3, q2,	q9
		vmla.i16	q5, q4,	q9
		vmla.i16	q7, q6,	q9
		vld1.16		{d0-d1}, [r12,:64],lr
		vld1.16		{d4-d5}, [r12,:64],lr
		vld1.16		{d8-d9}, [r12,:64],lr
		vld1.16		{d12-d13}, [r12,:64],lr
		vhadd.s16	q1, q1,	q0
		vhadd.s16	q3, q3,	q2
		vhadd.s16	q5, q5,	q4
		vhadd.s16	q7, q7,	q6
		vqrshrun.s16	d0, q1,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q5,	#6
		vqrshrun.s16	d3, q7,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		ble		loc_9fec
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		d26, d30
		b		loc_9f00
@ ---------------------------------------------------------------------------

loc_9fec:				@ code xref: .text:00009fd4j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_9d94

locret_a004:				@ code xref: .text:00009eccj
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H0V3_neon
_VO_avg_MC_InterLuma_H0V3_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d16, #5
		vmov.i8		d17, #0x11
		vmov.i8		d18, #0x3a @ ':'
		vmov.i8		d19, #0xa
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_a154

loc_a034:				@ code xref: .text:0000a14cj
					@ .text:0000a268j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		subs		r7, r7,	#4
		ble		loc_a164
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3

loc_a060:				@ code xref: .text:0000a134j
		vld1.64		{d26}, [r4],r3
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		subs		r8, r8,	#4
		vsubl.u8	q0, d20, d26
		vshll.u8	q1, d25, #2
		vsubl.u8	q2, d21, d27
		vshll.u8	q3, d26, #2
		vsubl.u8	q4, d22, d28
		vshll.u8	q5, d27, #2
		vsubl.u8	q6, d23, d29
		vshll.u8	q7, d28, #2
		vadd.i16	q1, q1,	q0
		vadd.i16	q3, q3,	q2
		vadd.i16	q5, q5,	q4
		vadd.i16	q7, q7,	q6
		vmlsl.u8	q1, d21, d16
		vmlsl.u8	q3, d22, d16
		vmlsl.u8	q5, d23, d16
		vmlsl.u8	q7, d24, d16
		vmlal.u8	q1, d22, d17
		vmlal.u8	q3, d23, d17
		vmlal.u8	q5, d24, d17
		vmlal.u8	q7, d25, d17
		vmlal.u8	q1, d23, d18
		vmlal.u8	q3, d24, d18
		vmlal.u8	q5, d25, d18
		vmlal.u8	q7, d26, d18
		vmlsl.u8	q1, d24, d19
		vmlsl.u8	q3, d25, d19
		vmlsl.u8	q5, d26, d19
		vmlsl.u8	q7, d27, d19
		vld1.16		{d0-d1}, [r12,:128],lr
		vld1.16		{d4-d5}, [r12,:128],lr
		vld1.16		{d8-d9}, [r12,:128],lr
		vld1.16		{d12-d13}, [r12,:128],lr
		vhadd.s16	q1, q1,	q0
		vhadd.s16	q3, q3,	q2
		vhadd.s16	q5, q5,	q4
		vhadd.s16	q7, q7,	q6
		vqrshrun.s16	d0, q1,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q5,	#6
		vqrshrun.s16	d3, q7,	#6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		ble		loc_a138
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		b		loc_a060
@ ---------------------------------------------------------------------------

loc_a138:				@ code xref: .text:0000a124j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_a034
		b		locret_a26c
@ ---------------------------------------------------------------------------

loc_a154:				@ code xref: .text:0000a030j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r7, r7,	#4

loc_a164:				@ code xref: .text:0000a044j
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		vld1.64		{d23}, [r4],r3
		vld1.64		{d24}, [r4],r3
		vld1.64		{d25}, [r4],r3

loc_a17c:				@ code xref: .text:0000a250j
		vld1.64		{d26}, [r4],r3
		vld1.64		{d27}, [r4],r3
		vld1.64		{d28}, [r4],r3
		vld1.64		{d29}, [r4],r3
		subs		r8, r8,	#4
		vsubl.u8	q0, d20, d26
		vshll.u8	q1, d25, #2
		vsubl.u8	q2, d21, d27
		vshll.u8	q3, d26, #2
		vsubl.u8	q4, d22, d28
		vshll.u8	q5, d27, #2
		vsubl.u8	q6, d23, d29
		vshll.u8	q7, d28, #2
		vadd.i16	q1, q1,	q0
		vadd.i16	q3, q3,	q2
		vadd.i16	q5, q5,	q4
		vadd.i16	q7, q7,	q6
		vmlsl.u8	q1, d21, d16
		vmlsl.u8	q3, d22, d16
		vmlsl.u8	q5, d23, d16
		vmlsl.u8	q7, d24, d16
		vmlal.u8	q1, d22, d17
		vmlal.u8	q3, d23, d17
		vmlal.u8	q5, d24, d17
		vmlal.u8	q7, d25, d17
		vmlal.u8	q1, d23, d18
		vmlal.u8	q3, d24, d18
		vmlal.u8	q5, d25, d18
		vmlal.u8	q7, d26, d18
		vmlsl.u8	q1, d24, d19
		vmlsl.u8	q3, d25, d19
		vmlsl.u8	q5, d26, d19
		vmlsl.u8	q7, d27, d19
		vld1.16		{d0-d1}, [r12,:64],lr
		vld1.16		{d4-d5}, [r12,:64],lr
		vld1.16		{d8-d9}, [r12,:64],lr
		vld1.16		{d12-d13}, [r12,:64],lr
		vhadd.s16	q1, q1,	q0
		vhadd.s16	q3, q3,	q2
		vhadd.s16	q5, q5,	q4
		vhadd.s16	q7, q7,	q6
		vqrshrun.s16	d0, q1,	#6
		vqrshrun.s16	d1, q3,	#6
		vqrshrun.s16	d2, q5,	#6
		vqrshrun.s16	d3, q7,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		ble		loc_a254
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		b		loc_a17c
@ ---------------------------------------------------------------------------

loc_a254:				@ code xref: .text:0000a240j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_a034

locret_a26c:				@ code xref: .text:0000a150j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H1V1_neon
_VO_avg_MC_InterLuma_H1V1_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_a568

loc_a2bc:				@ code xref: .text:0000a560j
					@ .text:0000a890j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_a580
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7

loc_a428:				@ code xref: .text:0000a548j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vsubl.s16	q4, d28, d16
		vsubl.s16	q5, d29, d17
		vsubl.s16	q6, d30, d18
		vsubl.s16	q7, d31, d19
		vmlal.s16	q4, d18, d6[0]
		vmlal.s16	q5, d19, d6[0]
		vmlal.s16	q6, d20, d6[0]
		vmlal.s16	q7, d21, d6[0]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlal.s16	q4, d24, d6[3]
		vmlal.s16	q5, d25, d6[3]
		vmlal.s16	q6, d26, d6[3]
		vmlal.s16	q7, d27, d6[3]
		vmlsl.s16	q4, d26, d6[1]
		vmlsl.s16	q5, d27, d6[1]
		vmlsl.s16	q6, d28, d6[1]
		vmlsl.s16	q7, d29, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vld1.16		{d12-d13}, [r12,:128],lr
		vld1.16		{d14-d15}, [r12,:128],lr
		vhadd.s16	q6, q6,	q4
		vhadd.s16	q7, q7,	q5
		vqrshrun.s16	d12, q6, #6
		vqrshrun.s16	d13, q7, #6
		vst1.8		{d12}, [r5,:64],r1
		vst1.8		{d13}, [r5,:64],r1
		ble		loc_a54c
		b		loc_a428
@ ---------------------------------------------------------------------------

loc_a54c:				@ code xref: .text:0000a544j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_a2bc
		b		locret_a894
@ ---------------------------------------------------------------------------

loc_a568:				@ code xref: .text:0000a2b8j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_a580:				@ code xref: .text:0000a2d4j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22

loc_a6dc:				@ code xref: .text:0000a878j
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q11, d8, #2
		vshll.u8	q12, d10, #2
		vadd.i16	q11, q11, q6
		vadd.i16	q12, q12, q7
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vmov		d25, d26
		vsubl.s16	q4, d22, d16
		vsubl.s16	q5, d23, d17
		vsubl.s16	q6, d24, d18
		vsubl.s16	q7, d25, d19
		vmlal.s16	q4, d17, d6[0]
		vmlal.s16	q5, d18, d6[0]
		vmlal.s16	q6, d19, d6[0]
		vmlal.s16	q7, d20, d6[0]
		vmlsl.s16	q4, d18, d7[0]
		vmlsl.s16	q5, d19, d7[0]
		vmlsl.s16	q6, d20, d7[0]
		vmlsl.s16	q7, d21, d7[0]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlsl.s16	q4, d21, d6[1]
		vmlsl.s16	q5, d22, d6[1]
		vmlsl.s16	q6, d23, d6[1]
		vmlsl.s16	q7, d24, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vld1.16		{d12}, [r12,:64],lr
		vld1.16		{d13}, [r12,:64],lr
		vld1.16		{d14}, [r12,:64],lr
		vld1.16		{d15}, [r12,:64],lr
		vhadd.s16	q6, q6,	q4
		vhadd.s16	q7, q7,	q5
		vqrshrun.s16	d12, q6, #6
		vqrshrun.s16	d13, q7, #6
		vst1.32		{d12[0]}, [r5,:32],r1
		vst1.32		{d12[1]}, [r5,:32],r1
		vst1.32		{d13[0]}, [r5,:32],r1
		vst1.32		{d13[1]}, [r5,:32],r1
		ble		loc_a87c
		b		loc_a6dc
@ ---------------------------------------------------------------------------

loc_a87c:				@ code xref: .text:0000a874j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_a2bc

locret_a894:				@ code xref: .text:0000a564j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H1V2_neon
_VO_avg_MC_InterLuma_H1V2_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_ab5c

loc_a8e4:				@ code xref: .text:0000ab54j
					@ .text:0000aeecj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_ab74
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vext.8		d8, d28, d29, #6
		vsubl.u8	q6, d8,	d28
		vext.8		d8, d28, d29, #2
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d28, d29, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d28, d29, #4
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d28, d29, #5
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d28, d29, #1
		vshll.u8	q14, d8, #2
		vadd.i16	q14, q14, q6

loc_aa88:				@ code xref: .text:0000ab3cj
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d30, d31, #6
		vsubl.u8	q6, d8,	d30
		vext.8		d8, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d30, d31, #1
		vshll.u8	q15, d8, #2
		vadd.i16	q15, q15, q6
		subs		r8, r8,	#1
		vaddl.s16	q4, d18, d28
		vaddl.s16	q5, d19, d29
		vaddl.s16	q6, d22, d24
		vaddl.s16	q7, d23, d25
		vmla.i32	q4, q6,	d7[0]
		vmla.i32	q5, q7,	d7[0]
		vaddl.s16	q6, d16, d30
		vaddl.s16	q7, d17, d31
		vaddl.s16	q2, d20, d26
		vaddl.s16	q8, d21, d27
		vmla.i32	q6, q2,	d7[1]
		vmla.i32	q7, q8,	d7[1]
		vshl.i32	q4, q4,	#2
		vshl.i32	q5, q5,	#2
		vmov		q8, q9
		vmov		q9, q10
		vmov		q10, q11
		vsub.i32	q4, q4,	q6
		vsub.i32	q5, q5,	q7
		vmov		q11, q12
		vmov		q12, q13
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vmov		q13, q14
		vmov		q14, q15
		vld1.16		{d10-d11}, [r12,:128],lr
		vhadd.s16	q5, q5,	q4
		vqrshrun.s16	d10, q5, #6
		vst1.8		{d10}, [r5,:64],r1
		ble		loc_ab40
		b		loc_aa88
@ ---------------------------------------------------------------------------

loc_ab40:				@ code xref: .text:0000ab38j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_a8e4
		b		locret_aef0
@ ---------------------------------------------------------------------------

loc_ab5c:				@ code xref: .text:0000a8e0j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_ab74:				@ code xref: .text:0000a8fcj
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22
		vext.8		d8, d28, d29, #6
		vsubl.u8	q6, d8,	d28
		vext.8		d8, d28, d29, #2
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d28, d29, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d28, d29, #4
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d28, d29, #5
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d28, d29, #1
		vshll.u8	q14, d8, #2
		vadd.i16	q14, q14, q6
		vmov		d22, d28

loc_ad0c:				@ code xref: .text:0000aed4j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vmov		d23, d28
		vmov		d24, d30
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vmov		d25, d28
		vmov		d26, d30
		subs		r8, r8,	#4
		vaddl.s16	q4, d16, d23
		vaddl.s16	q5, d18, d21
		vmla.i32	q4, q5,	d7[1]
		vaddl.s16	q5, d17, d22
		vaddl.s16	q6, d19, d20
		vmla.i32	q5, q6,	d7[0]
		vshl.i32	q5, q5,	#2
		vsub.i32	q4, q5,	q4
		vaddl.s16	q5, d17, d24
		vaddl.s16	q6, d19, d22
		vmla.i32	q5, q6,	d7[1]
		vaddl.s16	q6, d18, d23
		vaddl.s16	q7, d20, d21
		vmla.i32	q6, q7,	d7[0]
		vshl.i32	q6, q6,	#2
		vsub.i32	q5, q6,	q5
		vaddl.s16	q6, d18, d25
		vaddl.s16	q7, d20, d23
		vmla.i32	q6, q7,	d7[1]
		vaddl.s16	q7, d19, d24
		vaddl.s16	q8, d21, d22
		vmla.i32	q7, q8,	d7[0]
		vshl.i32	q7, q7,	#2
		vsub.i32	q6, q7,	q6
		vaddl.s16	q7, d19, d26
		vaddl.s16	q8, d21, d24
		vmla.i32	q7, q8,	d7[1]
		vaddl.s16	q8, d20, d25
		vaddl.s16	q15, d22, d23
		vmla.i32	q8, q15, d7[0]
		vshl.i32	q8, q8,	#2
		vsub.i32	q7, q8,	q7
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		d22, d26
		vld1.16		{d12}, [r12,:64],lr
		vld1.16		{d13}, [r12,:64],lr
		vld1.16		{d14}, [r12,:64],lr
		vld1.16		{d15}, [r12,:64],lr
		vhadd.s16	q6, q6,	q4
		vhadd.s16	q7, q7,	q5
		vqrshrun.s16	d12, q6, #6
		vqrshrun.s16	d13, q7, #6
		vst1.32		{d12[0]}, [r5,:32],r1
		vst1.32		{d12[1]}, [r5,:32],r1
		vst1.32		{d13[0]}, [r5,:32],r1
		vst1.32		{d13[1]}, [r5,:32],r1
		ble		loc_aed8
		b		loc_ad0c
@ ---------------------------------------------------------------------------

loc_aed8:				@ code xref: .text:0000aed0j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_a8e4

locret_aef0:				@ code xref: .text:0000ab58j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H1V3_neon
_VO_avg_MC_InterLuma_H1V3_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_b1e8

loc_af40:				@ code xref: .text:0000b1e0j
					@ .text:0000b50cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_b1fc
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7

loc_b0a8:				@ code xref: .text:0000b1c8j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vsubl.s16	q4, d16, d28
		vsubl.s16	q5, d17, d29
		vsubl.s16	q6, d18, d30
		vsubl.s16	q7, d19, d31
		vmlsl.s16	q4, d18, d6[1]
		vmlsl.s16	q5, d19, d6[1]
		vmlsl.s16	q6, d20, d6[1]
		vmlsl.s16	q7, d21, d6[1]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlsl.s16	q4, d24, d7[0]
		vmlsl.s16	q5, d25, d7[0]
		vmlsl.s16	q6, d26, d7[0]
		vmlsl.s16	q7, d27, d7[0]
		vmlal.s16	q4, d26, d6[0]
		vmlal.s16	q5, d27, d6[0]
		vmlal.s16	q6, d28, d6[0]
		vmlal.s16	q7, d29, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vld1.16		{d12-d13}, [r12,:128],lr
		vld1.16		{d14-d15}, [r12,:128],lr
		vhadd.s16	q6, q6,	q4
		vhadd.s16	q7, q7,	q5
		vqrshrun.s16	d12, q6, #6
		vqrshrun.s16	d13, q7, #6
		vst1.8		{d12}, [r5,:64],r1
		vst1.8		{d13}, [r5,:64],r1
		ble		loc_b1cc
		b		loc_b0a8
@ ---------------------------------------------------------------------------

loc_b1cc:				@ code xref: .text:0000b1c4j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_af40
		b		locret_b510
@ ---------------------------------------------------------------------------

loc_b1e8:				@ code xref: .text:0000af3cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_b1fc:				@ code xref: .text:0000af54j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d8,	d16
		vsubl.u8	q7, d10, d18
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d8,	d20
		vsubl.u8	q7, d10, d22
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22

loc_b358:				@ code xref: .text:0000b4f4j
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d8,	d24
		vsubl.u8	q7, d10, d26
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vshll.u8	q11, d8, #2
		vshll.u8	q12, d10, #2
		vadd.i16	q11, q11, q6
		vadd.i16	q12, q12, q7
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d8,	d28
		vsubl.u8	q7, d10, d30
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vmov		d25, d26
		vsubl.s16	q4, d16, d22
		vsubl.s16	q5, d17, d23
		vsubl.s16	q6, d18, d24
		vsubl.s16	q7, d19, d25
		vmlsl.s16	q4, d17, d6[1]
		vmlsl.s16	q5, d18, d6[1]
		vmlsl.s16	q6, d19, d6[1]
		vmlsl.s16	q7, d20, d6[1]
		vmlal.s16	q4, d18, d6[3]
		vmlal.s16	q5, d19, d6[3]
		vmlal.s16	q6, d20, d6[3]
		vmlal.s16	q7, d21, d6[3]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d21, d6[0]
		vmlal.s16	q5, d22, d6[0]
		vmlal.s16	q6, d23, d6[0]
		vmlal.s16	q7, d24, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vld1.16		{d12}, [r12,:64],lr
		vld1.16		{d13}, [r12,:64],lr
		vld1.16		{d14}, [r12,:64],lr
		vld1.16		{d15}, [r12,:64],lr
		vhadd.s16	q6, q6,	q4
		vhadd.s16	q7, q7,	q5
		vqrshrun.s16	d12, q6, #6
		vqrshrun.s16	d13, q7, #6
		vst1.32		{d12[0]}, [r5,:32],r1
		vst1.32		{d12[1]}, [r5,:32],r1
		vst1.32		{d13[0]}, [r5,:32],r1
		vst1.32		{d13[1]}, [r5,:32],r1
		ble		loc_b4f8
		b		loc_b358
@ ---------------------------------------------------------------------------

loc_b4f8:				@ code xref: .text:0000b4f0j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_af40

locret_b510:				@ code xref: .text:0000b1e4j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H2V1_neon
_VO_avg_MC_InterLuma_H2V1_neon:
		stmfd		sp!, {r4-r10,lr}
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_b83c

loc_b550:				@ code xref: .text:0000b834j
					@ .text:0000bbb4j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_b854
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q10, d20, d8
		vaddl.u8	q11, d22, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q12, d24, d8
		vaddl.u8	q13, d26, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13

loc_b6ec:				@ code xref: .text:0000b81cj
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q14, d28, d8
		vaddl.u8	q15, d30, d9
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q15, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q14, q1, q14
		vsub.i16	q15, q6, q15
		vsubl.s16	q4, d28, d16
		vsubl.s16	q5, d29, d17
		vsubl.s16	q6, d30, d18
		vsubl.s16	q7, d31, d19
		vmlal.s16	q4, d18, d6[0]
		vmlal.s16	q5, d19, d6[0]
		vmlal.s16	q6, d20, d6[0]
		vmlal.s16	q7, d21, d6[0]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlal.s16	q4, d24, d6[3]
		vmlal.s16	q5, d25, d6[3]
		vmlal.s16	q6, d26, d6[3]
		vmlal.s16	q7, d27, d6[3]
		vmlsl.s16	q4, d26, d6[1]
		vmlsl.s16	q5, d27, d6[1]
		vmlsl.s16	q6, d28, d6[1]
		vmlsl.s16	q7, d29, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d12, q6, #6
		vshrn.i32	d13, q7, #6
		vld1.16		{d0-d1}, [r12,:128],lr
		vld1.16		{d2-d3}, [r12,:128],lr
		vhadd.s16	q0, q0,	q4
		vhadd.s16	q1, q1,	q6
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		ble		loc_b820
		b		loc_b6ec
@ ---------------------------------------------------------------------------

loc_b820:				@ code xref: .text:0000b818j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_b550
		b		locret_bbb8
@ ---------------------------------------------------------------------------

loc_b83c:				@ code xref: .text:0000b54cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_b854:				@ code xref: .text:0000b568j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vmov		d17, d18
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q9, d20, d8
		vaddl.u8	q10, d22, d9
		vmla.i16	q9, q2,	d7[2]
		vmla.i16	q10, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q9, q1,	q9
		vsub.i16	q10, q6, q10
		vmov		d19, d20
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q10, d24, d8
		vaddl.u8	q11, d26, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vmov		d21, d22

loc_b9e0:				@ code xref: .text:0000bb9cj
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q11, d24, d8
		vaddl.u8	q12, d26, d9
		vmla.i16	q11, q2, d7[2]
		vmla.i16	q12, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q11, q1, q11
		vsub.i16	q12, q6, q12
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q12, d28, d8
		vaddl.u8	q13, d30, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13
		vmov		d25, d26
		vsubl.s16	q4, d22, d16
		vsubl.s16	q5, d23, d17
		vsubl.s16	q6, d24, d18
		vsubl.s16	q7, d25, d19
		vmlal.s16	q4, d17, d6[0]
		vmlal.s16	q5, d18, d6[0]
		vmlal.s16	q6, d19, d6[0]
		vmlal.s16	q7, d20, d6[0]
		vmlsl.s16	q4, d18, d7[0]
		vmlsl.s16	q5, d19, d7[0]
		vmlsl.s16	q6, d20, d7[0]
		vmlsl.s16	q7, d21, d7[0]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlsl.s16	q4, d21, d6[1]
		vmlsl.s16	q5, d22, d6[1]
		vmlsl.s16	q6, d23, d6[1]
		vmlsl.s16	q7, d24, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d12, q6, #6
		vshrn.i32	d13, q7, #6
		vld1.16		{d0}, [r12,:64],lr
		vld1.16		{d1}, [r12,:64],lr
		vld1.16		{d2}, [r12,:64],lr
		vld1.16		{d3}, [r12,:64],lr
		vhadd.s16	q0, q0,	q4
		vhadd.s16	q1, q1,	q6
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d0[1]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d1[1]}, [r5,:32],r1
		ble		loc_bba0
		b		loc_b9e0
@ ---------------------------------------------------------------------------

loc_bba0:				@ code xref: .text:0000bb98j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_b550

locret_bbb8:				@ code xref: .text:0000b838j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H2V2_neon
_VO_avg_MC_InterLuma_H2V2_neon:
		stmfd		sp!, {r4-r10,lr}
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_beb0

loc_bbf8:				@ code xref: .text:0000bea8j
					@ .text:0000c29cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_bec8
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q10, d20, d8
		vaddl.u8	q11, d22, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q12, d24, d8
		vaddl.u8	q13, d26, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vaddl.u8	q0, d0,	d1
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vaddl.u8	q1, d2,	d3
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vaddl.u8	q2, d4,	d5
		vext.8		d8, d28, d29, #7
		vaddl.u8	q14, d28, d8
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q1, q0,	d7[0]
		vshl.i16	q1, q1,	#2
		vsub.i16	q14, q1, q14

loc_bdd4:				@ code xref: .text:0000be90j
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d0, d30, d31, #3
		vext.8		d1, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vext.8		d2, d30, d31, #1
		vext.8		d3, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vext.8		d4, d30, d31, #2
		vext.8		d5, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vext.8		d8, d30, d31, #7
		vaddl.u8	q15, d30, d8
		vmla.i16	q15, q2, d7[2]
		vmla.i16	q1, q0,	d7[0]
		vshl.i16	q1, q1,	#2
		vsub.i16	q15, q1, q15
		subs		r8, r8,	#1
		vaddl.s16	q4, d18, d28
		vaddl.s16	q5, d19, d29
		vaddl.s16	q6, d16, d30
		vaddl.s16	q7, d17, d31
		vaddl.s16	q0, d22, d24
		vaddl.s16	q1, d23, d25
		vaddl.s16	q2, d20, d26
		vaddl.s16	q8, d21, d27
		vmla.i32	q4, q0,	d7[0]
		vmla.i32	q5, q1,	d7[0]
		vmla.i32	q6, q2,	d7[1]
		vmla.i32	q7, q8,	d7[1]
		vshl.i32	q4, q4,	#2
		vshl.i32	q5, q5,	#2
		vmov		q8, q9
		vmov		q9, q10
		vmov		q10, q11
		vsub.i32	q4, q4,	q6
		vsub.i32	q5, q5,	q7
		vmov		q11, q12
		vmov		q12, q13
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vmov		q13, q14
		vmov		q14, q15
		vld1.16		{d0-d1}, [r12,:128],lr
		vhadd.s16	q0, q0,	q4
		vqrshrun.s16	d0, q0,	#6
		vst1.8		{d0}, [r5,:64],r1
		ble		loc_be94
		b		loc_bdd4
@ ---------------------------------------------------------------------------

loc_be94:				@ code xref: .text:0000be8cj
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_bbf8
		b		locret_c2a0
@ ---------------------------------------------------------------------------

loc_beb0:				@ code xref: .text:0000bbf4j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_bec8:				@ code xref: .text:0000bc10j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vmov		d17, d18
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q9, d20, d8
		vaddl.u8	q10, d22, d9
		vmla.i16	q9, q2,	d7[2]
		vmla.i16	q10, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q9, q1,	q9
		vsub.i16	q10, q6, q10
		vmov		d19, d20
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q10, d24, d8
		vaddl.u8	q11, d26, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vmov		d21, d22
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vaddl.u8	q0, d0,	d1
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vaddl.u8	q1, d2,	d3
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vaddl.u8	q2, d4,	d5
		vext.8		d8, d28, d29, #7
		vaddl.u8	q14, d28, d8
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q1, q0,	d7[0]
		vshl.i16	q1, q1,	#2
		vsub.i16	q14, q1, q14
		vmov		d22, d28

loc_c098:				@ code xref: .text:0000c284j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q14, d28, d8
		vaddl.u8	q15, d30, d9
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q15, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q14, q1, q14
		vsub.i16	q15, q6, q15
		vmov		d23, d28
		vmov		d24, d30
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q14, d28, d8
		vaddl.u8	q15, d30, d9
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q15, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q14, q1, q14
		vsub.i16	q15, q6, q15
		vmov		d25, d28
		vmov		d26, d30
		subs		r8, r8,	#4
		vmov		r9, r10, d26
		vaddl.s16	q0, d16, d23
		vaddl.s16	q1, d17, d24
		vaddl.s16	q2, d18, d25
		vaddl.s16	q4, d19, d26
		vaddl.s16	q5, d18, d21
		vaddl.s16	q6, d19, d22
		vaddl.s16	q7, d20, d23
		vaddl.s16	q14, d21, d24
		vmla.i32	q0, q5,	d7[1]
		vmla.i32	q1, q6,	d7[1]
		vmla.i32	q2, q7,	d7[1]
		vmla.i32	q4, q14, d7[1]
		vaddl.s16	q5, d17, d22
		vaddl.s16	q6, d18, d23
		vaddl.s16	q7, d19, d24
		vaddl.s16	q14, d20, d25
		vaddl.s16	q15, d19, d20
		vaddl.s16	q8, d20, d21
		vaddl.s16	q9, d21, d22
		vaddl.s16	q13, d22, d23
		vmla.i32	q5, q15, d7[0]
		vmla.i32	q6, q8,	d7[0]
		vmla.i32	q7, q9,	d7[0]
		vmla.i32	q14, q13, d7[0]
		vshl.i32	q5, q5,	#2
		vshl.i32	q6, q6,	#2
		vshl.i32	q7, q7,	#2
		vshl.i32	q14, q14, #2
		vsub.i32	q5, q5,	q0
		vsub.i32	q6, q6,	q1
		vsub.i32	q7, q7,	q2
		vsub.i32	q14, q14, q4
		vshrn.i32	d8, q5,	#6
		vshrn.i32	d9, q6,	#6
		vshrn.i32	d10, q7, #6
		vshrn.i32	d11, q14, #6
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		d22, r9, r10
		vld1.16		{d0}, [r12,:64],lr
		vld1.16		{d1}, [r12,:64],lr
		vld1.16		{d2}, [r12,:64],lr
		vld1.16		{d3}, [r12,:64],lr
		vhadd.s16	q0, q0,	q4
		vhadd.s16	q1, q1,	q5
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d0[1]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d1[1]}, [r5,:32],r1
		ble		loc_c288
		b		loc_c098
@ ---------------------------------------------------------------------------

loc_c288:				@ code xref: .text:0000c280j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_bbf8

locret_c2a0:				@ code xref: .text:0000beacj
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H2V3_neon
_VO_avg_MC_InterLuma_H2V3_neon:
		stmfd		sp!, {r4-r10,lr}
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_c5c8

loc_c2e0:				@ code xref: .text:0000c5c0j
					@ .text:0000c93cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#3
		subs		r7, r7,	#4
		ble		loc_c5dc
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q10, d20, d8
		vaddl.u8	q11, d22, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q12, d24, d8
		vaddl.u8	q13, d26, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13

loc_c478:				@ code xref: .text:0000c5a8j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q14, d28, d8
		vaddl.u8	q15, d30, d9
		vmla.i16	q14, q2, d7[2]
		vmla.i16	q15, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q14, q1, q14
		vsub.i16	q15, q6, q15
		vsubl.s16	q4, d16, d28
		vsubl.s16	q5, d17, d29
		vsubl.s16	q6, d18, d30
		vsubl.s16	q7, d19, d31
		vmlsl.s16	q4, d18, d6[1]
		vmlsl.s16	q5, d19, d6[1]
		vmlsl.s16	q6, d20, d6[1]
		vmlsl.s16	q7, d21, d6[1]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlsl.s16	q4, d24, d7[0]
		vmlsl.s16	q5, d25, d7[0]
		vmlsl.s16	q6, d26, d7[0]
		vmlsl.s16	q7, d27, d7[0]
		vmlal.s16	q4, d26, d6[0]
		vmlal.s16	q5, d27, d6[0]
		vmlal.s16	q6, d28, d6[0]
		vmlal.s16	q7, d29, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d12, q6, #6
		vshrn.i32	d13, q7, #6
		vld1.16		{d0-d1}, [r12,:128],lr
		vld1.16		{d2-d3}, [r12,:128],lr
		vhadd.s16	q0, q0,	q4
		vhadd.s16	q1, q1,	q6
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		ble		loc_c5ac
		b		loc_c478
@ ---------------------------------------------------------------------------

loc_c5ac:				@ code xref: .text:0000c5a4j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_c2e0
		b		locret_c940
@ ---------------------------------------------------------------------------

loc_c5c8:				@ code xref: .text:0000c2dcj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#3
		sub		r7, r7,	#4

loc_c5dc:				@ code xref: .text:0000c2f4j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d16, d17, #3
		vext.8		d1, d16, d17, #4
		vext.8		d10, d18, d19, #3
		vext.8		d11, d18, d19, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d16, d17, #1
		vext.8		d3, d16, d17, #6
		vext.8		d12, d18, d19, #1
		vext.8		d13, d18, d19, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d16, d17, #2
		vext.8		d5, d16, d17, #5
		vext.8		d14, d18, d19, #2
		vext.8		d15, d18, d19, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d16, d17, #7
		vext.8		d9, d18, d19, #7
		vaddl.u8	q8, d16, d8
		vaddl.u8	q9, d18, d9
		vmla.i16	q8, q2,	d7[2]
		vmla.i16	q9, q7,	d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q8, q1,	q8
		vsub.i16	q9, q6,	q9
		vmov		d17, d18
		vext.8		d0, d20, d21, #3
		vext.8		d1, d20, d21, #4
		vext.8		d10, d22, d23, #3
		vext.8		d11, d22, d23, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d20, d21, #1
		vext.8		d3, d20, d21, #6
		vext.8		d12, d22, d23, #1
		vext.8		d13, d22, d23, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d20, d21, #2
		vext.8		d5, d20, d21, #5
		vext.8		d14, d22, d23, #2
		vext.8		d15, d22, d23, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d20, d21, #7
		vext.8		d9, d22, d23, #7
		vaddl.u8	q9, d20, d8
		vaddl.u8	q10, d22, d9
		vmla.i16	q9, q2,	d7[2]
		vmla.i16	q10, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q9, q1,	q9
		vsub.i16	q10, q6, q10
		vmov		d19, d20
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q10, d24, d8
		vaddl.u8	q11, d26, d9
		vmla.i16	q10, q2, d7[2]
		vmla.i16	q11, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q10, q1, q10
		vsub.i16	q11, q6, q11
		vmov		d21, d22

loc_c768:				@ code xref: .text:0000c924j
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d0, d24, d25, #3
		vext.8		d1, d24, d25, #4
		vext.8		d10, d26, d27, #3
		vext.8		d11, d26, d27, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d24, d25, #1
		vext.8		d3, d24, d25, #6
		vext.8		d12, d26, d27, #1
		vext.8		d13, d26, d27, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d24, d25, #2
		vext.8		d5, d24, d25, #5
		vext.8		d14, d26, d27, #2
		vext.8		d15, d26, d27, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d24, d25, #7
		vext.8		d9, d26, d27, #7
		vaddl.u8	q11, d24, d8
		vaddl.u8	q12, d26, d9
		vmla.i16	q11, q2, d7[2]
		vmla.i16	q12, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q11, q1, q11
		vsub.i16	q12, q6, q12
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d0, d28, d29, #3
		vext.8		d1, d28, d29, #4
		vext.8		d10, d30, d31, #3
		vext.8		d11, d30, d31, #4
		vaddl.u8	q0, d0,	d1
		vaddl.u8	q5, d10, d11
		vext.8		d2, d28, d29, #1
		vext.8		d3, d28, d29, #6
		vext.8		d12, d30, d31, #1
		vext.8		d13, d30, d31, #6
		vaddl.u8	q1, d2,	d3
		vaddl.u8	q6, d12, d13
		vext.8		d4, d28, d29, #2
		vext.8		d5, d28, d29, #5
		vext.8		d14, d30, d31, #2
		vext.8		d15, d30, d31, #5
		vaddl.u8	q2, d4,	d5
		vaddl.u8	q7, d14, d15
		vmla.i16	q1, q0,	d7[0]
		vmla.i16	q6, q5,	d7[0]
		vext.8		d8, d28, d29, #7
		vext.8		d9, d30, d31, #7
		vaddl.u8	q12, d28, d8
		vaddl.u8	q13, d30, d9
		vmla.i16	q12, q2, d7[2]
		vmla.i16	q13, q7, d7[2]
		vshl.i16	q1, q1,	#2
		vshl.i16	q6, q6,	#2
		vsub.i16	q12, q1, q12
		vsub.i16	q13, q6, q13
		vmov		d25, d26
		vsubl.s16	q4, d16, d22
		vsubl.s16	q5, d17, d23
		vsubl.s16	q6, d18, d24
		vsubl.s16	q7, d19, d25
		vmlsl.s16	q4, d17, d6[1]
		vmlsl.s16	q5, d18, d6[1]
		vmlsl.s16	q6, d19, d6[1]
		vmlsl.s16	q7, d20, d6[1]
		vmlal.s16	q4, d18, d6[3]
		vmlal.s16	q5, d19, d6[3]
		vmlal.s16	q6, d20, d6[3]
		vmlal.s16	q7, d21, d6[3]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d21, d6[0]
		vmlal.s16	q5, d22, d6[0]
		vmlal.s16	q6, d23, d6[0]
		vmlal.s16	q7, d24, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d12, q6, #6
		vshrn.i32	d13, q7, #6
		vld1.16		{d0}, [r12,:64],lr
		vld1.16		{d1}, [r12,:64],lr
		vld1.16		{d2}, [r12,:64],lr
		vld1.16		{d3}, [r12,:64],lr
		vhadd.s16	q0, q0,	q4
		vhadd.s16	q1, q1,	q6
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d0[1]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d1[1]}, [r5,:32],r1
		ble		loc_c928
		b		loc_c768
@ ---------------------------------------------------------------------------

loc_c928:				@ code xref: .text:0000c920j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_c2e0

locret_c940:				@ code xref: .text:0000c5c4j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H3V1_neon
_VO_avg_MC_InterLuma_H3V1_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_cc3c

loc_c990:				@ code xref: .text:0000cc34j
					@ .text:0000cf64j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#2
		subs		r7, r7,	#4
		ble		loc_cc54
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7

loc_cafc:				@ code xref: .text:0000cc1cj
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vsubl.s16	q4, d28, d16
		vsubl.s16	q5, d29, d17
		vsubl.s16	q6, d30, d18
		vsubl.s16	q7, d31, d19
		vmlal.s16	q4, d18, d6[0]
		vmlal.s16	q5, d19, d6[0]
		vmlal.s16	q6, d20, d6[0]
		vmlal.s16	q7, d21, d6[0]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlal.s16	q4, d24, d6[3]
		vmlal.s16	q5, d25, d6[3]
		vmlal.s16	q6, d26, d6[3]
		vmlal.s16	q7, d27, d6[3]
		vmlsl.s16	q4, d26, d6[1]
		vmlsl.s16	q5, d27, d6[1]
		vmlsl.s16	q6, d28, d6[1]
		vmlsl.s16	q7, d29, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vld1.16		{d12-d13}, [r12,:128],lr
		vld1.16		{d14-d15}, [r12,:128],lr
		vhadd.s16	q6, q6,	q4
		vhadd.s16	q7, q7,	q5
		vqrshrun.s16	d12, q6, #6
		vqrshrun.s16	d13, q7, #6
		vst1.8		{d12}, [r5,:64],r1
		vst1.8		{d13}, [r5,:64],r1
		ble		loc_cc20
		b		loc_cafc
@ ---------------------------------------------------------------------------

loc_cc20:				@ code xref: .text:0000cc18j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_c990
		b		locret_cf68
@ ---------------------------------------------------------------------------

loc_cc3c:				@ code xref: .text:0000c98cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#2
		sub		r7, r7,	#4

loc_cc54:				@ code xref: .text:0000c9a8j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22

loc_cdb0:				@ code xref: .text:0000cf4cj
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q11, d8, #2
		vshll.u8	q12, d10, #2
		vadd.i16	q11, q11, q6
		vadd.i16	q12, q12, q7
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vmov		d25, d26
		vsubl.s16	q4, d22, d16
		vsubl.s16	q5, d23, d17
		vsubl.s16	q6, d24, d18
		vsubl.s16	q7, d25, d19
		vmlal.s16	q4, d17, d6[0]
		vmlal.s16	q5, d18, d6[0]
		vmlal.s16	q6, d19, d6[0]
		vmlal.s16	q7, d20, d6[0]
		vmlsl.s16	q4, d18, d7[0]
		vmlsl.s16	q5, d19, d7[0]
		vmlsl.s16	q6, d20, d7[0]
		vmlsl.s16	q7, d21, d7[0]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlsl.s16	q4, d21, d6[1]
		vmlsl.s16	q5, d22, d6[1]
		vmlsl.s16	q6, d23, d6[1]
		vmlsl.s16	q7, d24, d6[1]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vld1.16		{d12}, [r12,:64],lr
		vld1.16		{d13}, [r12,:64],lr
		vld1.16		{d14}, [r12,:64],lr
		vld1.16		{d15}, [r12,:64],lr
		vhadd.s16	q6, q6,	q4
		vhadd.s16	q7, q7,	q5
		vqrshrun.s16	d12, q6, #6
		vqrshrun.s16	d13, q7, #6
		vst1.32		{d12[0]}, [r5,:32],r1
		vst1.32		{d12[1]}, [r5,:32],r1
		vst1.32		{d13[0]}, [r5,:32],r1
		vst1.32		{d13[1]}, [r5,:32],r1
		ble		loc_cf50
		b		loc_cdb0
@ ---------------------------------------------------------------------------

loc_cf50:				@ code xref: .text:0000cf48j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_c990

locret_cf68:				@ code xref: .text:0000cc38j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H3V2_neon
_VO_avg_MC_InterLuma_H3V2_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_d230

loc_cfb8:				@ code xref: .text:0000d228j
					@ .text:0000d5c0j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#2
		subs		r7, r7,	#4
		ble		loc_d248
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vext.8		d8, d28, d29, #6
		vsubl.u8	q6, d28, d8
		vext.8		d8, d28, d29, #1
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d28, d29, #2
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d28, d29, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d28, d29, #4
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d28, d29, #5
		vshll.u8	q14, d8, #2
		vadd.i16	q14, q14, q6

loc_d15c:				@ code xref: .text:0000d210j
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d30, d31, #6
		vsubl.u8	q6, d30, d8
		vext.8		d8, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d30, d31, #5
		vshll.u8	q15, d8, #2
		vadd.i16	q15, q15, q6
		subs		r8, r8,	#1
		vaddl.s16	q4, d18, d28
		vaddl.s16	q5, d19, d29
		vaddl.s16	q6, d22, d24
		vaddl.s16	q7, d23, d25
		vmla.i32	q4, q6,	d7[0]
		vmla.i32	q5, q7,	d7[0]
		vaddl.s16	q6, d16, d30
		vaddl.s16	q7, d17, d31
		vaddl.s16	q2, d20, d26
		vaddl.s16	q8, d21, d27
		vmla.i32	q6, q2,	d7[1]
		vmla.i32	q7, q8,	d7[1]
		vshl.i32	q4, q4,	#2
		vshl.i32	q5, q5,	#2
		vmov		q8, q9
		vmov		q9, q10
		vmov		q10, q11
		vsub.i32	q4, q4,	q6
		vsub.i32	q5, q5,	q7
		vmov		q11, q12
		vmov		q12, q13
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vmov		q13, q14
		vmov		q14, q15
		vld1.16		{d10-d11}, [r12,:128],lr
		vhadd.s16	q5, q5,	q4
		vqrshrun.s16	d10, q5, #6
		vst1.8		{d10}, [r5,:64],r1
		ble		loc_d214
		b		loc_d15c
@ ---------------------------------------------------------------------------

loc_d214:				@ code xref: .text:0000d20cj
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_cfb8
		b		locret_d5c4
@ ---------------------------------------------------------------------------

loc_d230:				@ code xref: .text:0000cfb4j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	r3
		sub		r4, r4,	#2
		sub		r7, r7,	#4

loc_d248:				@ code xref: .text:0000cfd0j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vld1.64		{d28-d29}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22
		vext.8		d8, d28, d29, #6
		vsubl.u8	q6, d28, d8
		vext.8		d8, d28, d29, #1
		vmlsl.u8	q6, d8,	d3
		vext.8		d8, d28, d29, #2
		vmlal.u8	q6, d8,	d2
		vext.8		d8, d28, d29, #3
		vmlal.u8	q6, d8,	d1
		vext.8		d8, d28, d29, #4
		vmlsl.u8	q6, d8,	d0
		vext.8		d8, d28, d29, #5
		vshll.u8	q14, d8, #2
		vadd.i16	q14, q14, q6
		vmov		d22, d28

loc_d3e0:				@ code xref: .text:0000d5a8j
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vmov		d23, d28
		vmov		d24, d30
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vmov		d25, d28
		vmov		d26, d30
		subs		r8, r8,	#4
		vaddl.s16	q4, d16, d23
		vaddl.s16	q5, d18, d21
		vmla.i32	q4, q5,	d7[1]
		vaddl.s16	q5, d17, d22
		vaddl.s16	q6, d19, d20
		vmla.i32	q5, q6,	d7[0]
		vshl.i32	q5, q5,	#2
		vsub.i32	q4, q5,	q4
		vaddl.s16	q5, d17, d24
		vaddl.s16	q6, d19, d22
		vmla.i32	q5, q6,	d7[1]
		vaddl.s16	q6, d18, d23
		vaddl.s16	q7, d20, d21
		vmla.i32	q6, q7,	d7[0]
		vshl.i32	q6, q6,	#2
		vsub.i32	q5, q6,	q5
		vaddl.s16	q6, d18, d25
		vaddl.s16	q7, d20, d23
		vmla.i32	q6, q7,	d7[1]
		vaddl.s16	q7, d19, d24
		vaddl.s16	q8, d21, d22
		vmla.i32	q7, q8,	d7[0]
		vshl.i32	q7, q7,	#2
		vsub.i32	q6, q7,	q6
		vaddl.s16	q7, d19, d26
		vaddl.s16	q8, d21, d24
		vmla.i32	q7, q8,	d7[1]
		vaddl.s16	q8, d20, d25
		vaddl.s16	q15, d22, d23
		vmla.i32	q8, q15, d7[0]
		vshl.i32	q8, q8,	#2
		vsub.i32	q7, q8,	q7
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		d22, d26
		vld1.16		{d12}, [r12,:64],lr
		vld1.16		{d13}, [r12,:64],lr
		vld1.16		{d14}, [r12,:64],lr
		vld1.16		{d15}, [r12,:64],lr
		vhadd.s16	q6, q6,	q4
		vhadd.s16	q7, q7,	q5
		vqrshrun.s16	d12, q6, #6
		vqrshrun.s16	d13, q7, #6
		vst1.32		{d12[0]}, [r5,:32],r1
		vst1.32		{d12[1]}, [r5,:32],r1
		vst1.32		{d13[0]}, [r5,:32],r1
		vst1.32		{d13[1]}, [r5,:32],r1
		ble		loc_d5ac
		b		loc_d3e0
@ ---------------------------------------------------------------------------

loc_d5ac:				@ code xref: .text:0000d5a4j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_cfb8

locret_d5c4:				@ code xref: .text:0000d22cj
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.align 4
.globl _VO_avg_MC_InterLuma_H3V3_neon
_VO_avg_MC_InterLuma_H3V3_neon:
		stmfd		sp!, {r4-r10,lr}
		vmov.i8		d0, #0xa
		vmov.i8		d1, #0x3a @ ':'
		vmov.i8		d2, #0x11
		vmov.i8		d3, #5
		mov		r6, #4
		movt  r6, 0x5
		mov		r7, #0x3a
		movt  r7, 0x11
		vmov		d6, r6,	r7
		mov		r6, #0xa
		mov		r7, #0xb
		vmov		d7, r6,	r7
		ldr		r7, [sp,#0x28]
		ldr		r6, [sp,#0x2c]
		ldr		r12, [sp,#0x20]
		ldr		lr, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_d8bc

loc_d614:				@ code xref: .text:0000d8b4j
					@ .text:0000dbe0j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#2
		subs		r7, r7,	#4
		ble		loc_d8d0
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7

loc_d77c:				@ code xref: .text:0000d89cj
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#2
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q14, d8, #2
		vshll.u8	q15, d10, #2
		vadd.i16	q14, q14, q6
		vadd.i16	q15, q15, q7
		vsubl.s16	q4, d16, d28
		vsubl.s16	q5, d17, d29
		vsubl.s16	q6, d18, d30
		vsubl.s16	q7, d19, d31
		vmlsl.s16	q4, d18, d6[1]
		vmlsl.s16	q5, d19, d6[1]
		vmlsl.s16	q6, d20, d6[1]
		vmlsl.s16	q7, d21, d6[1]
		vmlal.s16	q4, d20, d6[3]
		vmlal.s16	q5, d21, d6[3]
		vmlal.s16	q6, d22, d6[3]
		vmlal.s16	q7, d23, d6[3]
		vmlal.s16	q4, d22, d6[2]
		vmlal.s16	q5, d23, d6[2]
		vmlal.s16	q6, d24, d6[2]
		vmlal.s16	q7, d25, d6[2]
		vmlsl.s16	q4, d24, d7[0]
		vmlsl.s16	q5, d25, d7[0]
		vmlsl.s16	q6, d26, d7[0]
		vmlsl.s16	q7, d27, d7[0]
		vmlal.s16	q4, d26, d6[0]
		vmlal.s16	q5, d27, d6[0]
		vmlal.s16	q6, d28, d6[0]
		vmlal.s16	q7, d29, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vmov		q11, q13
		vmov		q12, q14
		vmov		q13, q15
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vld1.16		{d12-d13}, [r12,:128],lr
		vld1.16		{d14-d15}, [r12,:128],lr
		vhadd.s16	q6, q6,	q4
		vhadd.s16	q7, q7,	q5
		vqrshrun.s16	d12, q6, #6
		vqrshrun.s16	d13, q7, #6
		vst1.8		{d12}, [r5,:64],r1
		vst1.8		{d13}, [r5,:64],r1
		ble		loc_d8a0
		b		loc_d77c
@ ---------------------------------------------------------------------------

loc_d8a0:				@ code xref: .text:0000d898j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_d614
		b		locret_dbe4
@ ---------------------------------------------------------------------------

loc_d8bc:				@ code xref: .text:0000d610j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3,lsl#1
		sub		r4, r4,	#2
		sub		r7, r7,	#4

loc_d8d0:				@ code xref: .text:0000d628j
		vld1.64		{d16-d17}, [r4],r3
		vld1.64		{d18-d19}, [r4],r3
		vld1.64		{d20-d21}, [r4],r3
		vld1.64		{d22-d23}, [r4],r3
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d16, d17, #6
		vext.8		d10, d18, d19, #6
		vsubl.u8	q6, d16, d8
		vsubl.u8	q7, d18, d10
		vext.8		d8, d16, d17, #1
		vext.8		d10, d18, d19, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d16, d17, #2
		vext.8		d10, d18, d19, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d16, d17, #3
		vext.8		d10, d18, d19, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d16, d17, #4
		vext.8		d10, d18, d19, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d16, d17, #5
		vext.8		d10, d18, d19, #5
		vshll.u8	q8, d8,	#2
		vshll.u8	q9, d10, #2
		vadd.i16	q8, q8,	q6
		vadd.i16	q9, q9,	q7
		vmov		d17, d18
		vext.8		d8, d20, d21, #6
		vext.8		d10, d22, d23, #6
		vsubl.u8	q6, d20, d8
		vsubl.u8	q7, d22, d10
		vext.8		d8, d20, d21, #1
		vext.8		d10, d22, d23, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d20, d21, #2
		vext.8		d10, d22, d23, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d20, d21, #3
		vext.8		d10, d22, d23, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d20, d21, #4
		vext.8		d10, d22, d23, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d20, d21, #5
		vext.8		d10, d22, d23, #5
		vshll.u8	q9, d8,	#2
		vshll.u8	q10, d10, #2
		vadd.i16	q9, q9,	q6
		vadd.i16	q10, q10, q7
		vmov		d19, d20
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q10, d8, #2
		vshll.u8	q11, d10, #2
		vadd.i16	q10, q10, q6
		vadd.i16	q11, q11, q7
		vmov		d21, d22

loc_da2c:				@ code xref: .text:0000dbc8j
		vld1.64		{d24-d25}, [r4],r3
		vld1.64		{d26-d27}, [r4],r3
		vext.8		d8, d24, d25, #6
		vext.8		d10, d26, d27, #6
		vsubl.u8	q6, d24, d8
		vsubl.u8	q7, d26, d10
		vext.8		d8, d24, d25, #1
		vext.8		d10, d26, d27, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d24, d25, #2
		vext.8		d10, d26, d27, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d24, d25, #3
		vext.8		d10, d26, d27, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d24, d25, #4
		vext.8		d10, d26, d27, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d24, d25, #5
		vext.8		d10, d26, d27, #5
		vshll.u8	q11, d8, #2
		vshll.u8	q12, d10, #2
		vadd.i16	q11, q11, q6
		vadd.i16	q12, q12, q7
		vmov		d23, d24
		vld1.64		{d28-d29}, [r4],r3
		vld1.64		{d30-d31}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d8, d28, d29, #6
		vext.8		d10, d30, d31, #6
		vsubl.u8	q6, d28, d8
		vsubl.u8	q7, d30, d10
		vext.8		d8, d28, d29, #1
		vext.8		d10, d30, d31, #1
		vmlsl.u8	q6, d8,	d3
		vmlsl.u8	q7, d10, d3
		vext.8		d8, d28, d29, #2
		vext.8		d10, d30, d31, #2
		vmlal.u8	q6, d8,	d2
		vmlal.u8	q7, d10, d2
		vext.8		d8, d28, d29, #3
		vext.8		d10, d30, d31, #3
		vmlal.u8	q6, d8,	d1
		vmlal.u8	q7, d10, d1
		vext.8		d8, d28, d29, #4
		vext.8		d10, d30, d31, #4
		vmlsl.u8	q6, d8,	d0
		vmlsl.u8	q7, d10, d0
		vext.8		d8, d28, d29, #5
		vext.8		d10, d30, d31, #5
		vshll.u8	q12, d8, #2
		vshll.u8	q13, d10, #2
		vadd.i16	q12, q12, q6
		vadd.i16	q13, q13, q7
		vmov		d25, d26
		vsubl.s16	q4, d16, d22
		vsubl.s16	q5, d17, d23
		vsubl.s16	q6, d18, d24
		vsubl.s16	q7, d19, d25
		vmlsl.s16	q4, d17, d6[1]
		vmlsl.s16	q5, d18, d6[1]
		vmlsl.s16	q6, d19, d6[1]
		vmlsl.s16	q7, d20, d6[1]
		vmlal.s16	q4, d18, d6[3]
		vmlal.s16	q5, d19, d6[3]
		vmlal.s16	q6, d20, d6[3]
		vmlal.s16	q7, d21, d6[3]
		vmlal.s16	q4, d19, d6[2]
		vmlal.s16	q5, d20, d6[2]
		vmlal.s16	q6, d21, d6[2]
		vmlal.s16	q7, d22, d6[2]
		vmlsl.s16	q4, d20, d7[0]
		vmlsl.s16	q5, d21, d7[0]
		vmlsl.s16	q6, d22, d7[0]
		vmlsl.s16	q7, d23, d7[0]
		vmlal.s16	q4, d21, d6[0]
		vmlal.s16	q5, d22, d6[0]
		vmlal.s16	q6, d23, d6[0]
		vmlal.s16	q7, d24, d6[0]
		vmov		q8, q10
		vmov		q9, q11
		vmov		q10, q12
		vshrn.i32	d8, q4,	#6
		vshrn.i32	d9, q5,	#6
		vshrn.i32	d10, q6, #6
		vshrn.i32	d11, q7, #6
		vld1.16		{d12}, [r12,:64],lr
		vld1.16		{d13}, [r12,:64],lr
		vld1.16		{d14}, [r12,:64],lr
		vld1.16		{d15}, [r12,:64],lr
		vhadd.s16	q6, q6,	q4
		vhadd.s16	q7, q7,	q5
		vqrshrun.s16	d12, q6, #6
		vqrshrun.s16	d13, q7, #6
		vst1.32		{d12[0]}, [r5,:32],r1
		vst1.32		{d12[1]}, [r5,:32],r1
		vst1.32		{d13[0]}, [r5,:32],r1
		vst1.32		{d13[1]}, [r5,:32],r1
		ble		loc_dbcc
		b		loc_da2c
@ ---------------------------------------------------------------------------

loc_dbcc:				@ code xref: .text:0000dbc4j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_d614

locret_dbe4:				@ code xref: .text:0000d8b8j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------


   .endif
   
