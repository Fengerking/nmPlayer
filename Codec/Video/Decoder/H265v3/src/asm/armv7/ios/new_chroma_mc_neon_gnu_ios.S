
#include     "h265dec_ASM_config.h"
#include "../../../h265dec_ID.h" 
    .text

    .align 4
		.if MC_ASM_ENABLED==1

		.globl _VO_put_MC_InterChroma_H0V0_neon
_VO_put_MC_InterChroma_H0V0_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]

loc_dbf4:				@ code xref: .text:0000dc44j
					@ .text:0000dc9cj ...
		cmp		r7, #0x20 @ ' '
		blt		loc_dc4c
		ands		r5, r0,	#0xf
		bne		loc_dc4c
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_dc10:				@ code xref: .text:0000dc34j
		vld1.64		{d0-d3}, [r4],r3
		vld1.64		{d4-d7}, [r4],r3
		vld1.64		{d8-d11}, [r4],r3
		vld1.64		{d12-d15}, [r4],r3
		subs		r8, r8,	#4
		vst1.64		{d0-d3}, [r5,:128],r1
		vst1.64		{d4-d7}, [r5,:128],r1
		vst1.64		{d8-d11}, [r5,:128],r1
		vst1.64		{d12-d15}, [r5,:128],r1
		bgt		loc_dc10
		add		r2, r2,	#0x20
		add		r0, r0,	#0x20
		subs		r7, r7,	#0x20
		bgt		loc_dbf4
		b		locret_dda4
@ ---------------------------------------------------------------------------

loc_dc4c:				@ code xref: .text:0000dbf8j
					@ .text:0000dc00j
		cmp		r7, #0x10
		blt		loc_dca4
		ands		r5, r0,	#0xf
		bne		loc_dca4
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_dc68:				@ code xref: .text:0000dc8cj
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d4-d5}, [r4],r3
		vld1.64		{d6-d7}, [r4],r3
		subs		r8, r8,	#4
		vst1.64		{d0-d1}, [r5,:128],r1
		vst1.64		{d2-d3}, [r5,:128],r1
		vst1.64		{d4-d5}, [r5,:128],r1
		vst1.64		{d6-d7}, [r5,:128],r1
		bgt		loc_dc68
		add		r2, r2,	#0x10
		add		r0, r0,	#0x10
		subs		r7, r7,	#0x10
		bgt		loc_dbf4
		b		locret_dda4
@ ---------------------------------------------------------------------------

loc_dca4:				@ code xref: .text:0000dc50j
					@ .text:0000dc58j
		cmp		r7, #8
		blt		loc_dd00
		ands		r5, r0,	#7
		bne		loc_dd00
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_dcc0:				@ code xref: .text:0000dce8j
		vld1.64		{d0}, [r4],r3
		vld1.64		{d1}, [r4],r3
		vld1.64		{d2}, [r4],r3
		vld1.64		{d3}, [r4],r3
		subs		r8, r8,	#4
		vst1.64		{d0}, [r5,:64],r1
		vst1.64		{d1}, [r5,:64],r1
		blt		loc_dcec
		vst1.64		{d2}, [r5,:64],r1
		vst1.64		{d3}, [r5,:64],r1
		bgt		loc_dcc0

loc_dcec:				@ code xref: .text:0000dcdcj
		add		r2, r2,	#8
		add		r0, r0,	#8
		subs		r7, r7,	#8
		bgt		loc_dbf4
		b		locret_dda4
@ ---------------------------------------------------------------------------

loc_dd00:				@ code xref: .text:0000dca8j
					@ .text:0000dcb0j
		cmp		r7, #4
		blt		loc_dd5c
		ands		r5, r0,	#3
		bne		loc_dd5c
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_dd1c:				@ code xref: .text:0000dd44j
		vld1.32		{d0[0]}, [r4],r3
		vld1.32		{d0[1]}, [r4],r3
		vld1.32		{d1[0]}, [r4],r3
		vld1.32		{d1[1]}, [r4],r3
		subs		r8, r8,	#4
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d0[1]}, [r5,:32],r1
		blt		loc_dd48
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d1[1]}, [r5,:32],r1
		bgt		loc_dd1c

loc_dd48:				@ code xref: .text:0000dd38j
		add		r2, r2,	#4
		add		r0, r0,	#4
		subs		r7, r7,	#4
		bgt		loc_dbf4
		b		locret_dda4
@ ---------------------------------------------------------------------------

loc_dd5c:				@ code xref: .text:0000dd04j
					@ .text:0000dd0cj
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_dd68:				@ code xref: .text:0000dd90j
		vld1.16		{d0[0]}, [r4],r3
		vld1.16		{d0[1]}, [r4],r3
		vld1.16		{d0[2]}, [r4],r3
		vld1.16		{d0[3]}, [r4],r3
		subs		r8, r8,	#4
		vst1.16		{d0[0]}, [r5,:16],r1
		vst1.16		{d0[1]}, [r5,:16],r1
		blt		loc_dd94
		vst1.16		{d0[2]}, [r5,:16],r1
		vst1.16		{d0[3]}, [r5,:16],r1
		bgt		loc_dd68

loc_dd94:				@ code xref: .text:0000dd84j
		add		r2, r2,	#2
		add		r0, r0,	#2
		subs		r7, r7,	#2
		bgt		loc_dbf4

locret_dda4:				@ code xref: .text:0000dc48j
					@ .text:0000dca0j ...
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

	.align 4
ChromaCoeff_table:
	.short       0,  64,  0,    0
	.short       2,  58,  10,   2
	.short       4,  54,  16,   2
	.short       6,  46,  28,   4
	.short       4,  36,  36,   4
	.short       4,  28,  46,   6
	.short       2,  16,  54,   4
	.short       2,  10,  58,   2

		.globl _VO_put_MC_InterChroma_H1V0_neon
_VO_put_MC_InterChroma_H1V0_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r5, [sp,#0x30]
		adr    r9, ChromaCoeff_table
		add		r10, r9, r5,lsl#3
		vld1.64		{d7}, [r10]
		vdup.8		d12, d7[0]
		vdup.8		d13, d7[2]
		vdup.8		d14, d7[4]
		vdup.8		d15, d7[6]
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_dec0

loc_dde0:				@ code xref: .text:0000deb8j
					@ .text:0000df9cj ...
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#1
		subs		r7, r7,	#2
		ble		loc_dfb4
		subs		r7, r7,	#2
		ble		loc_dee0

loc_ddfc:				@ code xref: .text:0000dea8j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vqrshrun.s16	d0, q11, #6
		vqrshrun.s16	d1, q12, #6
		vqrshrun.s16	d2, q13, #6
		vqrshrun.s16	d3, q14, #6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		blt		loc_deac
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		ble		loc_deac
		b		loc_ddfc
@ ---------------------------------------------------------------------------

loc_deac:				@ code xref: .text:0000de98j
					@ .text:0000dea4j
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_dde0
		b		locret_e074
@ ---------------------------------------------------------------------------

loc_dec0:				@ code xref: .text:0000dddcj
		ands		r5, r0,	#3
		bne		loc_dfa4
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#1
		subs		r7, r7,	#2
		ble		loc_dfb4
		sub		r7, r7,	#2

loc_dee0:				@ code xref: .text:0000ddf8j
					@ .text:0000df8cj
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vqrshrun.s16	d0, q11, #6
		vqrshrun.s16	d1, q12, #6
		vqrshrun.s16	d2, q13, #6
		vqrshrun.s16	d3, q14, #6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		blt		loc_df90
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		ble		loc_df90
		b		loc_dee0
@ ---------------------------------------------------------------------------

loc_df90:				@ code xref: .text:0000df7cj
					@ .text:0000df88j
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_dde0
		b		locret_e074
@ ---------------------------------------------------------------------------

loc_dfa4:				@ code xref: .text:0000dec4j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#1
		sub		r7, r7,	#2

loc_dfb4:				@ code xref: .text:0000ddf0j
					@ .text:0000ded8j ...
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vqrshrun.s16	d0, q11, #6
		vqrshrun.s16	d1, q12, #6
		vqrshrun.s16	d2, q13, #6
		vqrshrun.s16	d3, q14, #6
		vst1.16		{d0[0]}, [r5,:16],r1
		vst1.16		{d1[0]}, [r5,:16],r1
		blt		loc_e064
		vst1.16		{d2[0]}, [r5,:16],r1
		vst1.16		{d3[0]}, [r5,:16],r1
		ble		loc_e064
		b		loc_dfb4
@ ---------------------------------------------------------------------------

loc_e064:				@ code xref: .text:0000e050j
					@ .text:0000e05cj
		add		r2, r2,	#2
		add		r0, r0,	#2
		cmp		r7, #0
		bgt		loc_dde0

locret_e074:				@ code xref: .text:0000debcj
					@ .text:0000dfa0j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------
	.align 4
ChromaCoeff_table1:
	.short       0,  64,  0,    0
	.short       2,  58,  10,   2
	.short       4,  54,  16,   2
	.short       6,  46,  28,   4
	.short       4,  36,  36,   4
	.short       4,  28,  46,   6
	.short       2,  16,  54,   4
	.short       2,  10,  58,   2
		.globl _VO_put_MC_InterChroma_H0V1_neon
_VO_put_MC_InterChroma_H0V1_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r6, [sp,#0x34]
		adr		r9, ChromaCoeff_table1
		add		r10, r9, r6,lsl#3
		vld1.64		{d6}, [r10]
		vdup.8		d12, d6[0]
		vdup.8		d13, d6[2]
		vdup.8		d14, d6[4]
		vdup.8		d15, d6[6]
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_e178

loc_e0b0:				@ code xref: .text:0000e170j
					@ .text:0000e23cj ...
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		subs		r7, r7,	#2
		ble		loc_e254
		subs		r7, r7,	#2
		ble		loc_e198
		vld1.64		{d16}, [r4],r3
		vld1.64		{d17}, [r4],r3
		vld1.64		{d18}, [r4],r3

loc_e0d8:				@ code xref: .text:0000e160j
		vld1.64		{d19}, [r4],r3
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		subs		r8, r8,	#4
		vmull.u8	q0, d17, d13
		vmull.u8	q1, d18, d13
		vmull.u8	q2, d19, d13
		vmull.u8	q3, d20, d13
		vmlsl.u8	q0, d16, d12
		vmlsl.u8	q1, d17, d12
		vmlsl.u8	q2, d18, d12
		vmlsl.u8	q3, d19, d12
		vmlal.u8	q0, d18, d14
		vmlal.u8	q1, d19, d14
		vmlal.u8	q2, d20, d14
		vmlal.u8	q3, d21, d14
		vmlsl.u8	q0, d19, d15
		vmlsl.u8	q1, d20, d15
		vmlsl.u8	q2, d21, d15
		vmlsl.u8	q3, d22, d15
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vqrshrun.s16	d2, q2,	#6
		vqrshrun.s16	d3, q3,	#6
		vmov		d16, d20
		vmov		d17, d21
		vmov		d18, d22
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		blt		loc_e164
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		ble		loc_e164
		b		loc_e0d8
@ ---------------------------------------------------------------------------

loc_e164:				@ code xref: .text:0000e150j
					@ .text:0000e15cj
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_e0b0
		b		locret_e2fc
@ ---------------------------------------------------------------------------

loc_e178:				@ code xref: .text:0000e0acj
		ands		r5, r0,	#3
		bne		loc_e244
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		subs		r7, r7,	#2
		ble		loc_e254
		sub		r7, r7,	#2

loc_e198:				@ code xref: .text:0000e0c8j
		vld1.64		{d16}, [r4],r3
		vld1.64		{d17}, [r4],r3
		vld1.64		{d18}, [r4],r3

loc_e1a4:				@ code xref: .text:0000e22cj
		vld1.64		{d19}, [r4],r3
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		subs		r8, r8,	#4
		vmull.u8	q0, d17, d13
		vmull.u8	q1, d18, d13
		vmull.u8	q2, d19, d13
		vmull.u8	q3, d20, d13
		vmlsl.u8	q0, d16, d12
		vmlsl.u8	q1, d17, d12
		vmlsl.u8	q2, d18, d12
		vmlsl.u8	q3, d19, d12
		vmlal.u8	q0, d18, d14
		vmlal.u8	q1, d19, d14
		vmlal.u8	q2, d20, d14
		vmlal.u8	q3, d21, d14
		vmlsl.u8	q0, d19, d15
		vmlsl.u8	q1, d20, d15
		vmlsl.u8	q2, d21, d15
		vmlsl.u8	q3, d22, d15
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vqrshrun.s16	d2, q2,	#6
		vqrshrun.s16	d3, q3,	#6
		vmov		d16, d20
		vmov		d17, d21
		vmov		d18, d22
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		blt		loc_e230
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		ble		loc_e230
		b		loc_e1a4
@ ---------------------------------------------------------------------------

loc_e230:				@ code xref: .text:0000e21cj
					@ .text:0000e228j
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_e0b0
		b		locret_e2fc
@ ---------------------------------------------------------------------------

loc_e244:				@ code xref: .text:0000e17cj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		sub		r7, r7,	#2

loc_e254:				@ code xref: .text:0000e0c0j
					@ .text:0000e190j
		vld1.64		{d16}, [r4],r3
		vld1.64		{d17}, [r4],r3
		vld1.64		{d18}, [r4],r3

loc_e260:				@ code xref: .text:0000e2e8j
		vld1.64		{d19}, [r4],r3
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		subs		r8, r8,	#4
		vmull.u8	q0, d17, d13
		vmull.u8	q1, d18, d13
		vmull.u8	q2, d19, d13
		vmull.u8	q3, d20, d13
		vmlsl.u8	q0, d16, d12
		vmlsl.u8	q1, d17, d12
		vmlsl.u8	q2, d18, d12
		vmlsl.u8	q3, d19, d12
		vmlal.u8	q0, d18, d14
		vmlal.u8	q1, d19, d14
		vmlal.u8	q2, d20, d14
		vmlal.u8	q3, d21, d14
		vmlsl.u8	q0, d19, d15
		vmlsl.u8	q1, d20, d15
		vmlsl.u8	q2, d21, d15
		vmlsl.u8	q3, d22, d15
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vqrshrun.s16	d2, q2,	#6
		vqrshrun.s16	d3, q3,	#6
		vmov		d16, d20
		vmov		d17, d21
		vmov		d18, d22
		vst1.16		{d0[0]}, [r5,:16],r1
		vst1.16		{d1[0]}, [r5,:16],r1
		blt		loc_e2ec
		vst1.16		{d2[0]}, [r5,:16],r1
		vst1.16		{d3[0]}, [r5,:16],r1
		ble		loc_e2ec
		b		loc_e260
@ ---------------------------------------------------------------------------

loc_e2ec:				@ code xref: .text:0000e2d8j
					@ .text:0000e2e4j
		add		r2, r2,	#2
		add		r0, r0,	#2
		cmp		r7, #0
		bgt		loc_e0b0

locret_e2fc:				@ code xref: .text:0000e174j
					@ .text:0000e240j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

	.align 4
ChromaCoeff_table2:
	.short       0,  64,  0,    0
	.short       2,  58,  10,   2
	.short       4,  54,  16,   2
	.short       6,  46,  28,   4
	.short       4,  36,  36,   4
	.short       4,  28,  46,   6
	.short       2,  16,  54,   4
	.short       2,  10,  58,   2	
		.globl _VO_put_MC_InterChroma_H1V1_neon
_VO_put_MC_InterChroma_H1V1_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r5, [sp,#0x30]
		ldr		r6, [sp,#0x34]
		adr		r9, ChromaCoeff_table2
		add		r10, r9, r5,lsl#3
		vld1.64		{d7}, [r10]
		add		r10, r9, r6,lsl#3
		vld1.64		{d6}, [r10]
		vdup.8		d12, d7[0]
		vdup.8		d13, d7[2]
		vdup.8		d14, d7[4]
		vdup.8		d15, d7[6]
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ands		r5, r0,	#7
		bne		loc_e534

loc_e344:				@ code xref: .text:0000e52cj
					@ .text:0000e720j ...
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		sub		r4, r4,	#1
		subs		r7, r7,	#2
		ble		loc_e73c
		subs		r7, r7,	#2
		ble		loc_e558
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vmull.u8	q8, d4,	d13
		vmull.u8	q9, d5,	d13
		vmull.u8	q10, d30, d13
		vmlsl.u8	q8, d0,	d12
		vmlsl.u8	q9, d2,	d12
		vmlsl.u8	q10, d8, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vmlal.u8	q8, d4,	d14
		vmlal.u8	q9, d5,	d14
		vmlal.u8	q10, d30, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vmlsl.u8	q8, d4,	d15
		vmlsl.u8	q9, d5,	d15
		vmlsl.u8	q10, d30, d15

loc_e3c4:				@ code xref: .text:0000e51cj
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vmull.s16	q0, d18, d6[1]
		vmull.s16	q1, d19, d6[1]
		vmull.s16	q2, d20, d6[1]
		vmull.s16	q4, d21, d6[1]
		vmull.s16	q5, d22, d6[1]
		vmlsl.s16	q0, d16, d6[0]
		vmlsl.s16	q1, d17, d6[0]
		vmlsl.s16	q2, d18, d6[0]
		vmlsl.s16	q4, d19, d6[0]
		vmull.s16	q15, d23, d6[1]
		vmull.s16	q8, d24, d6[1]
		vmull.s16	q9, d25, d6[1]
		vmlsl.s16	q5, d20, d6[0]
		vmlsl.s16	q15, d21, d6[0]
		vmlsl.s16	q8, d22, d6[0]
		vmlsl.s16	q9, d23, d6[0]
		vmlal.s16	q0, d20, d6[2]
		vmlal.s16	q1, d21, d6[2]
		vmlal.s16	q2, d22, d6[2]
		vmlal.s16	q4, d23, d6[2]
		vmlal.s16	q5, d24, d6[2]
		vmlal.s16	q15, d25, d6[2]
		vmlal.s16	q8, d26, d6[2]
		vmlal.s16	q9, d27, d6[2]
		vmlsl.s16	q0, d22, d6[3]
		vmlsl.s16	q1, d23, d6[3]
		vmlsl.s16	q2, d24, d6[3]
		vmlsl.s16	q4, d25, d6[3]
		vmlsl.s16	q5, d26, d6[3]
		vmlsl.s16	q15, d27, d6[3]
		vmlsl.s16	q8, d28, d6[3]
		vmlsl.s16	q9, d29, d6[3]
		vqrshrun.s32	d0, q0,	#0xc
		vqrshrun.s32	d1, q1,	#0xc
		vqmovn.u16	d0, q0
		vqrshrun.s32	d2, q2,	#0xc
		vqrshrun.s32	d3, q4,	#0xc
		vqmovn.u16	d1, q1
		vqrshrun.s32	d4, q5,	#0xc
		vqrshrun.s32	d5, q15, #0xc
		vqmovn.u16	d2, q2
		vqrshrun.s32	d8, q8,	#0xc
		vqrshrun.s32	d9, q9,	#0xc
		vqmovn.u16	d3, q4
		vmov		q8, q12
		vmov		q9, q13
		vmov		q10, q14
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		blt		loc_e520
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		ble		loc_e520
		b		loc_e3c4
@ ---------------------------------------------------------------------------

loc_e520:				@ code xref: .text:0000e50cj
					@ .text:0000e518j
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_e344
		b		locret_e908
@ ---------------------------------------------------------------------------

loc_e534:				@ code xref: .text:0000e340j
		ands		r5, r0,	#3
		bne		loc_e728
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		sub		r4, r4,	#1
		subs		r7, r7,	#2
		ble		loc_e73c
		sub		r7, r7,	#2

loc_e558:				@ code xref: .text:0000e360j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vmull.u8	q8, d4,	d13
		vmull.u8	q9, d5,	d13
		vmull.u8	q10, d30, d13
		vmlsl.u8	q8, d0,	d12
		vmlsl.u8	q9, d2,	d12
		vmlsl.u8	q10, d8, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vmlal.u8	q8, d4,	d14
		vmlal.u8	q9, d5,	d14
		vmlal.u8	q10, d30, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vmlsl.u8	q8, d4,	d15
		vmlsl.u8	q9, d5,	d15
		vmlsl.u8	q10, d30, d15

loc_e5b8:				@ code xref: .text:0000e710j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vmull.s16	q0, d18, d6[1]
		vmull.s16	q1, d19, d6[1]
		vmull.s16	q2, d20, d6[1]
		vmull.s16	q4, d21, d6[1]
		vmull.s16	q5, d22, d6[1]
		vmlsl.s16	q0, d16, d6[0]
		vmlsl.s16	q1, d17, d6[0]
		vmlsl.s16	q2, d18, d6[0]
		vmlsl.s16	q4, d19, d6[0]
		vmull.s16	q15, d23, d6[1]
		vmull.s16	q8, d24, d6[1]
		vmull.s16	q9, d25, d6[1]
		vmlsl.s16	q5, d20, d6[0]
		vmlsl.s16	q15, d21, d6[0]
		vmlsl.s16	q8, d22, d6[0]
		vmlsl.s16	q9, d23, d6[0]
		vmlal.s16	q0, d20, d6[2]
		vmlal.s16	q1, d21, d6[2]
		vmlal.s16	q2, d22, d6[2]
		vmlal.s16	q4, d23, d6[2]
		vmlal.s16	q5, d24, d6[2]
		vmlal.s16	q15, d25, d6[2]
		vmlal.s16	q8, d26, d6[2]
		vmlal.s16	q9, d27, d6[2]
		vmlsl.s16	q0, d22, d6[3]
		vmlsl.s16	q1, d23, d6[3]
		vmlsl.s16	q2, d24, d6[3]
		vmlsl.s16	q4, d25, d6[3]
		vmlsl.s16	q5, d26, d6[3]
		vmlsl.s16	q15, d27, d6[3]
		vmlsl.s16	q8, d28, d6[3]
		vmlsl.s16	q9, d29, d6[3]
		vqrshrun.s32	d0, q0,	#0xc
		vqrshrun.s32	d1, q1,	#0xc
		vqmovn.u16	d0, q0
		vqrshrun.s32	d2, q2,	#0xc
		vqrshrun.s32	d3, q4,	#0xc
		vqmovn.u16	d1, q1
		vqrshrun.s32	d4, q5,	#0xc
		vqrshrun.s32	d5, q15, #0xc
		vqmovn.u16	d2, q2
		vqrshrun.s32	d8, q8,	#0xc
		vqrshrun.s32	d9, q9,	#0xc
		vqmovn.u16	d3, q4
		vmov		q8, q12
		vmov		q9, q13
		vmov		q10, q14
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		blt		loc_e714
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		ble		loc_e714
		b		loc_e5b8
@ ---------------------------------------------------------------------------

loc_e714:				@ code xref: .text:0000e700j
					@ .text:0000e70cj
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_e344
		b		locret_e908
@ ---------------------------------------------------------------------------

loc_e728:				@ code xref: .text:0000e538j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		sub		r4, r4,	#1
		sub		r7, r7,	#2

loc_e73c:				@ code xref: .text:0000e358j
					@ .text:0000e550j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vmull.u8	q8, d4,	d13
		vmull.u8	q9, d5,	d13
		vmull.u8	q10, d30, d13
		vmlsl.u8	q8, d0,	d12
		vmlsl.u8	q9, d2,	d12
		vmlsl.u8	q10, d8, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vmlal.u8	q8, d4,	d14
		vmlal.u8	q9, d5,	d14
		vmlal.u8	q10, d30, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vmlsl.u8	q8, d4,	d15
		vmlsl.u8	q9, d5,	d15
		vmlsl.u8	q10, d30, d15

loc_e79c:				@ code xref: .text:0000e8f4j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vmull.s16	q0, d18, d6[1]
		vmull.s16	q1, d19, d6[1]
		vmull.s16	q2, d20, d6[1]
		vmull.s16	q4, d21, d6[1]
		vmull.s16	q5, d22, d6[1]
		vmlsl.s16	q0, d16, d6[0]
		vmlsl.s16	q1, d17, d6[0]
		vmlsl.s16	q2, d18, d6[0]
		vmlsl.s16	q4, d19, d6[0]
		vmull.s16	q15, d23, d6[1]
		vmull.s16	q8, d24, d6[1]
		vmull.s16	q9, d25, d6[1]
		vmlsl.s16	q5, d20, d6[0]
		vmlsl.s16	q15, d21, d6[0]
		vmlsl.s16	q8, d22, d6[0]
		vmlsl.s16	q9, d23, d6[0]
		vmlal.s16	q0, d20, d6[2]
		vmlal.s16	q1, d21, d6[2]
		vmlal.s16	q2, d22, d6[2]
		vmlal.s16	q4, d23, d6[2]
		vmlal.s16	q5, d24, d6[2]
		vmlal.s16	q15, d25, d6[2]
		vmlal.s16	q8, d26, d6[2]
		vmlal.s16	q9, d27, d6[2]
		vmlsl.s16	q0, d22, d6[3]
		vmlsl.s16	q1, d23, d6[3]
		vmlsl.s16	q2, d24, d6[3]
		vmlsl.s16	q4, d25, d6[3]
		vmlsl.s16	q5, d26, d6[3]
		vmlsl.s16	q15, d27, d6[3]
		vmlsl.s16	q8, d28, d6[3]
		vmlsl.s16	q9, d29, d6[3]
		vqrshrun.s32	d0, q0,	#0xc
		vqrshrun.s32	d1, q1,	#0xc
		vqmovn.u16	d0, q0
		vqrshrun.s32	d2, q2,	#0xc
		vqrshrun.s32	d3, q4,	#0xc
		vqmovn.u16	d1, q1
		vqrshrun.s32	d4, q5,	#0xc
		vqrshrun.s32	d5, q15, #0xc
		vqmovn.u16	d2, q2
		vqrshrun.s32	d8, q8,	#0xc
		vqrshrun.s32	d9, q9,	#0xc
		vqmovn.u16	d3, q4
		vmov		q8, q12
		vmov		q9, q13
		vmov		q10, q14
		vst1.16		{d0[0]}, [r5,:16],r1
		vst1.16		{d1[0]}, [r5,:16],r1
		blt		loc_e8f8
		vst1.16		{d2[0]}, [r5,:16],r1
		vst1.16		{d3[0]}, [r5,:16],r1
		ble		loc_e8f8
		b		loc_e79c
@ ---------------------------------------------------------------------------

loc_e8f8:				@ code xref: .text:0000e8e4j
					@ .text:0000e8f0j
		add		r2, r2,	#2
		add		r0, r0,	#2
		cmp		r7, #0
		bgt		loc_e344

locret_e908:				@ code xref: .text:0000e530j
					@ .text:0000e724j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------


		.globl _VO_avg_nornd_MC_InterChroma_H0V0_neon
_VO_avg_nornd_MC_InterChroma_H0V0_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ldr		r12, [sp,#0x28]
		ldr		lr, [sp,#0x2c]

loc_e920:				@ code xref: .text:0000e9d0j
					@ .text:0000ea50j ...
		cmp		r7, #0x20 @ ' '
		blt		loc_e9d8
		ands		r5, r0,	#0xf
		bne		loc_e9d8
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_e93c:				@ code xref: .text:0000e9b8j
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
		bgt		loc_e93c
		mls		r12, lr, r6, r12
		add		r12, r12, #0x40
		add		r2, r2,	#0x20
		add		r0, r0,	#0x20
		subs		r7, r7,	#0x20
		bgt		loc_e920
		b		locret_eb8c
@ ---------------------------------------------------------------------------

loc_e9d8:				@ code xref: .text:0000e924j
					@ .text:0000e92cj
		cmp		r7, #0x10
		blt		loc_ea58
		ands		r5, r0,	#0xf
		bne		loc_ea58
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_e9f4:				@ code xref: .text:0000ea38j
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
		bgt		loc_e9f4
		mls		r12, lr, r6, r12
		add		r12, r12, #0x20
		add		r2, r2,	#0x10
		add		r0, r0,	#0x10
		subs		r7, r7,	#0x10
		bgt		loc_e920
		b		locret_eb8c
@ ---------------------------------------------------------------------------

loc_ea58:				@ code xref: .text:0000e9dcj
					@ .text:0000e9e4j
		cmp		r7, #8
		blt		loc_eacc
		ands		r5, r0,	#7
		bne		loc_eacc
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_ea74:				@ code xref: .text:0000eaacj
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
		blt		loc_eab0
		vst1.64		{d20-d21}, [r12,:128],lr
		vst1.64		{d22-d23}, [r12,:128],lr
		bgt		loc_ea74

loc_eab0:				@ code xref: .text:0000eaa0j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		add		r2, r2,	#8
		add		r0, r0,	#8
		subs		r7, r7,	#8
		bgt		loc_e920
		b		locret_eb8c
@ ---------------------------------------------------------------------------

loc_eacc:				@ code xref: .text:0000ea5cj
					@ .text:0000ea64j
		cmp		r7, #4
		blt		loc_eb38
		ands		r5, r0,	#3
		bne		loc_eb38
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_eae8:				@ code xref: .text:0000eb18j
		vld1.32		{d0[0]}, [r4],r3
		vld1.32		{d0[1]}, [r4],r3
		vld1.32		{d1[0]}, [r4],r3
		vld1.32		{d1[1]}, [r4],r3
		subs		r8, r8,	#4
		vshll.u8	q8, d0,	#6
		vshll.u8	q9, d1,	#6
		vst1.64		{d16}, [r12,:64],lr
		vst1.64		{d17}, [r12,:64],lr
		blt		loc_eb1c
		vst1.64		{d18}, [r12,:64],lr
		vst1.64		{d19}, [r12,:64],lr
		bgt		loc_eae8

loc_eb1c:				@ code xref: .text:0000eb0cj
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		subs		r7, r7,	#4
		bgt		loc_e920
		b		locret_eb8c
@ ---------------------------------------------------------------------------

loc_eb38:				@ code xref: .text:0000ead0j
					@ .text:0000ead8j
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_eb44:				@ code xref: .text:0000eb70j
		vld1.16		{d0[0]}, [r4],r3
		vld1.16		{d0[1]}, [r4],r3
		vld1.16		{d0[2]}, [r4],r3
		vld1.16		{d0[3]}, [r4],r3
		subs		r8, r8,	#4
		vshll.u8	q8, d0,	#6
		vst1.32		{d16[0]}, [r12,:32],lr
		vst1.32		{d16[1]}, [r12,:32],lr
		blt		loc_eb74
		vst1.32		{d17[0]}, [r12,:32],lr
		vst1.32		{d17[1]}, [r12,:32],lr
		bgt		loc_eb44

loc_eb74:				@ code xref: .text:0000eb64j
		mls		r12, lr, r6, r12
		add		r12, r12, #4
		add		r2, r2,	#2
		add		r0, r0,	#2
		subs		r7, r7,	#2
		bgt		loc_e920

locret_eb8c:				@ code xref: .text:0000e9d4j
					@ .text:0000ea54j ...
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

	.align 4
ChromaCoeff_table3:
	.short       0,  64,  0,    0
	.short       2,  58,  10,   2
	.short       4,  54,  16,   2
	.short       6,  46,  28,   4
	.short       4,  36,  36,   4
	.short       4,  28,  46,   6
	.short       2,  16,  54,   4
	.short       2,  10,  58,   2	
		.globl _VO_avg_nornd_MC_InterChroma_H1V0_neon
_VO_avg_nornd_MC_InterChroma_H1V0_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r5, [sp,#0x30]
		adr     r9, ChromaCoeff_table3
		add		r10, r9, r5,lsl#3
		vld1.64		{d7}, [r10]
		vdup.8		d12, d7[0]
		vdup.8		d13, d7[2]
		vdup.8		d14, d7[4]
		vdup.8		d15, d7[6]
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ldr		r12, [sp,#0x28]
		ldr		lr, [sp,#0x2c]
		ands		r5, r0,	#7
		bne		loc_eca8

loc_ebd0:				@ code xref: .text:0000eca0j
					@ .text:0000ed7cj ...
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#1
		subs		r7, r7,	#2
		ble		loc_ed94
		subs		r7, r7,	#2
		ble		loc_ecc8

loc_ebec:				@ code xref: .text:0000ec88j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vst1.8		{d22-d23}, [r12,:128],lr
		vst1.8		{d24-d25}, [r12,:128],lr
		blt		loc_ec8c
		vst1.8		{d26-d27}, [r12,:128],lr
		vst1.8		{d28-d29}, [r12,:128],lr
		ble		loc_ec8c
		b		loc_ebec
@ ---------------------------------------------------------------------------

loc_ec8c:				@ code xref: .text:0000ec78j
					@ .text:0000ec84j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_ebd0
		b		locret_ee4c
@ ---------------------------------------------------------------------------

loc_eca8:				@ code xref: .text:0000ebccj
		ands		r5, r0,	#3
		bne		loc_ed84
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#1
		subs		r7, r7,	#2
		ble		loc_ed94
		sub		r7, r7,	#2

loc_ecc8:				@ code xref: .text:0000ebe8j
					@ .text:0000ed64j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vst1.8		{d22}, [r12,:64],lr
		vst1.8		{d24}, [r12,:64],lr
		blt		loc_ed68
		vst1.8		{d26}, [r12,:64],lr
		vst1.8		{d28}, [r12,:64],lr
		ble		loc_ed68
		b		loc_ecc8
@ ---------------------------------------------------------------------------

loc_ed68:				@ code xref: .text:0000ed54j
					@ .text:0000ed60j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_ebd0
		b		locret_ee4c
@ ---------------------------------------------------------------------------

loc_ed84:				@ code xref: .text:0000ecacj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#1
		sub		r7, r7,	#2

loc_ed94:				@ code xref: .text:0000ebe0j
					@ .text:0000ecc0j ...
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vst1.32		{d22[0]}, [r12,:32],lr
		vst1.32		{d24[0]}, [r12,:32],lr
		blt		loc_ee34
		vst1.32		{d26[0]}, [r12,:32],lr
		vst1.32		{d28[0]}, [r12,:32],lr
		ble		loc_ee34
		b		loc_ed94
@ ---------------------------------------------------------------------------

loc_ee34:				@ code xref: .text:0000ee20j
					@ .text:0000ee2cj
		mls		r12, lr, r6, r12
		add		r12, r12, #4
		add		r2, r2,	#2
		add		r0, r0,	#2
		cmp		r7, #0
		bgt		loc_ebd0

locret_ee4c:				@ code xref: .text:0000eca4j
					@ .text:0000ed80j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

@coeff_off_4: .long ChromaCoeff
	.align 4
ChromaCoeff_table4:
	.short       0,  64,  0,    0
	.short       2,  58,  10,   2
	.short       4,  54,  16,   2
	.short       6,  46,  28,   4
	.short       4,  36,  36,   4
	.short       4,  28,  46,   6
	.short       2,  16,  54,   4
	.short       2,  10,  58,   2
	
		.globl _VO_avg_nornd_MC_InterChroma_H0V1_neon
_VO_avg_nornd_MC_InterChroma_H0V1_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r6, [sp,#0x34]
		adr   r9, ChromaCoeff_table4
		add		r10, r9, r6,lsl#3
		vld1.64		{d6}, [r10]
		vdup.8		d12, d6[0]
		vdup.8		d13, d6[2]
		vdup.8		d14, d6[4]
		vdup.8		d15, d6[6]
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ldr		r12, [sp,#0x28]
		ldr		lr, [sp,#0x2c]
		ands		r5, r0,	#7
		bne		loc_ef50

loc_ee90:				@ code xref: .text:0000ef48j
					@ .text:0000f00cj ...
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		subs		r7, r7,	#2
		ble		loc_f024
		subs		r7, r7,	#2
		ble		loc_ef70
		vld1.64		{d16}, [r4],r3
		vld1.64		{d17}, [r4],r3
		vld1.64		{d18}, [r4],r3

loc_eeb8:				@ code xref: .text:0000ef30j
		vld1.64		{d19}, [r4],r3
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		subs		r8, r8,	#4
		vmull.u8	q0, d17, d13
		vmull.u8	q1, d18, d13
		vmull.u8	q2, d19, d13
		vmull.u8	q3, d20, d13
		vmlsl.u8	q0, d16, d12
		vmlsl.u8	q1, d17, d12
		vmlsl.u8	q2, d18, d12
		vmlsl.u8	q3, d19, d12
		vmlal.u8	q0, d18, d14
		vmlal.u8	q1, d19, d14
		vmlal.u8	q2, d20, d14
		vmlal.u8	q3, d21, d14
		vmlsl.u8	q0, d19, d15
		vmlsl.u8	q1, d20, d15
		vmlsl.u8	q2, d21, d15
		vmlsl.u8	q3, d22, d15
		vmov		d16, d20
		vmov		d17, d21
		vmov		d18, d22
		vst1.8		{d0-d1}, [r12,:128],lr
		vst1.8		{d2-d3}, [r12,:128],lr
		blt		loc_ef34
		vst1.8		{d4-d5}, [r12,:128],lr
		vst1.8		{d6-d7}, [r12,:128],lr
		ble		loc_ef34
		b		loc_eeb8
@ ---------------------------------------------------------------------------

loc_ef34:				@ code xref: .text:0000ef20j
					@ .text:0000ef2cj
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_ee90
		b		locret_f0c4
@ ---------------------------------------------------------------------------

loc_ef50:				@ code xref: .text:0000ee8cj
		ands		r5, r0,	#3
		bne		loc_f014
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		subs		r7, r7,	#2
		ble		loc_f024
		sub		r7, r7,	#2

loc_ef70:				@ code xref: .text:0000eea8j
		vld1.64		{d16}, [r4],r3
		vld1.64		{d17}, [r4],r3
		vld1.64		{d18}, [r4],r3

loc_ef7c:				@ code xref: .text:0000eff4j
		vld1.64		{d19}, [r4],r3
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		subs		r8, r8,	#4
		vmull.u8	q0, d17, d13
		vmull.u8	q1, d18, d13
		vmull.u8	q2, d19, d13
		vmull.u8	q3, d20, d13
		vmlsl.u8	q0, d16, d12
		vmlsl.u8	q1, d17, d12
		vmlsl.u8	q2, d18, d12
		vmlsl.u8	q3, d19, d12
		vmlal.u8	q0, d18, d14
		vmlal.u8	q1, d19, d14
		vmlal.u8	q2, d20, d14
		vmlal.u8	q3, d21, d14
		vmlsl.u8	q0, d19, d15
		vmlsl.u8	q1, d20, d15
		vmlsl.u8	q2, d21, d15
		vmlsl.u8	q3, d22, d15
		vmov		d16, d20
		vmov		d17, d21
		vmov		d18, d22
		vst1.8		{d0}, [r12,:64],lr
		vst1.8		{d2}, [r12,:64],lr
		blt		loc_eff8
		vst1.8		{d4}, [r12,:64],lr
		vst1.8		{d6}, [r12,:64],lr
		ble		loc_eff8
		b		loc_ef7c
@ ---------------------------------------------------------------------------

loc_eff8:				@ code xref: .text:0000efe4j
					@ .text:0000eff0j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_ee90
		b		locret_f0c4
@ ---------------------------------------------------------------------------

loc_f014:				@ code xref: .text:0000ef54j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		sub		r7, r7,	#2

loc_f024:				@ code xref: .text:0000eea0j
					@ .text:0000ef68j
		vld1.64		{d16}, [r4],r3
		vld1.64		{d17}, [r4],r3
		vld1.64		{d18}, [r4],r3

loc_f030:				@ code xref: .text:0000f0a8j
		vld1.64		{d19}, [r4],r3
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		subs		r8, r8,	#4
		vmull.u8	q0, d17, d13
		vmull.u8	q1, d18, d13
		vmull.u8	q2, d19, d13
		vmull.u8	q3, d20, d13
		vmlsl.u8	q0, d16, d12
		vmlsl.u8	q1, d17, d12
		vmlsl.u8	q2, d18, d12
		vmlsl.u8	q3, d19, d12
		vmlal.u8	q0, d18, d14
		vmlal.u8	q1, d19, d14
		vmlal.u8	q2, d20, d14
		vmlal.u8	q3, d21, d14
		vmlsl.u8	q0, d19, d15
		vmlsl.u8	q1, d20, d15
		vmlsl.u8	q2, d21, d15
		vmlsl.u8	q3, d22, d15
		vmov		d16, d20
		vmov		d17, d21
		vmov		d18, d22
		vst1.32		{d0[0]}, [r12,:32],lr
		vst1.32		{d2[0]}, [r12,:32],lr
		blt		loc_f0ac
		vst1.32		{d4[0]}, [r12,:32],lr
		vst1.32		{d6[0]}, [r12,:32],lr
		ble		loc_f0ac
		b		loc_f030
@ ---------------------------------------------------------------------------

loc_f0ac:				@ code xref: .text:0000f098j
					@ .text:0000f0a4j
		mls		r12, lr, r6, r12
		add		r12, r12, #4
		add		r2, r2,	#2
		add		r0, r0,	#2
		cmp		r7, #0
		bgt		loc_ee90

locret_f0c4:				@ code xref: .text:0000ef4cj
					@ .text:0000f010j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

@coeff_off_5: .long ChromaCoeff
ChromaCoeff_table5:
	.short       0,  64,  0,    0
	.short       2,  58,  10,   2
	.short       4,  54,  16,   2
	.short       6,  46,  28,   4
	.short       4,  36,  36,   4
	.short       4,  28,  46,   6
	.short       2,  16,  54,   4
	.short       2,  10,  58,   2
	
		.globl _VO_avg_nornd_MC_InterChroma_H1V1_neon
_VO_avg_nornd_MC_InterChroma_H1V1_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r5, [sp,#0x30]
		ldr		r6, [sp,#0x34]
		adr   r9, ChromaCoeff_table5
		add		r10, r9, r5,lsl#3
		vld1.64		{d7}, [r10]
		add		r10, r9, r6,lsl#3
		vld1.64		{d6}, [r10]
		vdup.8		d12, d7[0]
		vdup.8		d13, d7[2]
		vdup.8		d14, d7[4]
		vdup.8		d15, d7[6]
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ldr		r12, [sp,#0x28]
		ldr		lr, [sp,#0x2c]
		ands		r5, r0,	#7
		bne		loc_f2fc

loc_f114:				@ code xref: .text:0000f2f4j
					@ .text:0000f4e0j ...
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		sub		r4, r4,	#1
		subs		r7, r7,	#2
		ble		loc_f4fc
		subs		r7, r7,	#2
		ble		loc_f320
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vmull.u8	q8, d4,	d13
		vmull.u8	q9, d5,	d13
		vmull.u8	q10, d30, d13
		vmlsl.u8	q8, d0,	d12
		vmlsl.u8	q9, d2,	d12
		vmlsl.u8	q10, d8, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vmlal.u8	q8, d4,	d14
		vmlal.u8	q9, d5,	d14
		vmlal.u8	q10, d30, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vmlsl.u8	q8, d4,	d15
		vmlsl.u8	q9, d5,	d15
		vmlsl.u8	q10, d30, d15

loc_f194:				@ code xref: .text:0000f2dcj
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vmull.s16	q0, d18, d6[1]
		vmull.s16	q1, d19, d6[1]
		vmull.s16	q2, d20, d6[1]
		vmull.s16	q4, d21, d6[1]
		vmull.s16	q5, d22, d6[1]
		vmlsl.s16	q0, d16, d6[0]
		vmlsl.s16	q1, d17, d6[0]
		vmlsl.s16	q2, d18, d6[0]
		vmlsl.s16	q4, d19, d6[0]
		vmull.s16	q15, d23, d6[1]
		vmull.s16	q8, d24, d6[1]
		vmull.s16	q9, d25, d6[1]
		vmlsl.s16	q5, d20, d6[0]
		vmlsl.s16	q15, d21, d6[0]
		vmlsl.s16	q8, d22, d6[0]
		vmlsl.s16	q9, d23, d6[0]
		vmlal.s16	q0, d20, d6[2]
		vmlal.s16	q1, d21, d6[2]
		vmlal.s16	q2, d22, d6[2]
		vmlal.s16	q4, d23, d6[2]
		vmlal.s16	q5, d24, d6[2]
		vmlal.s16	q15, d25, d6[2]
		vmlal.s16	q8, d26, d6[2]
		vmlal.s16	q9, d27, d6[2]
		vmlsl.s16	q0, d22, d6[3]
		vmlsl.s16	q1, d23, d6[3]
		vmlsl.s16	q2, d24, d6[3]
		vmlsl.s16	q4, d25, d6[3]
		vmlsl.s16	q5, d26, d6[3]
		vmlsl.s16	q15, d27, d6[3]
		vmlsl.s16	q8, d28, d6[3]
		vmlsl.s16	q9, d29, d6[3]
		vshrn.i32	d0, q0,	#6
		vshrn.i32	d1, q1,	#6
		vshrn.i32	d2, q2,	#6
		vshrn.i32	d3, q4,	#6
		vshrn.i32	d4, q5,	#6
		vshrn.i32	d5, q15, #6
		vshrn.i32	d8, q8,	#6
		vshrn.i32	d9, q9,	#6
		vmov		q8, q12
		vmov		q9, q13
		vmov		q10, q14
		vst1.8		{d0-d1}, [r12,:128],lr
		vst1.8		{d2-d3}, [r12,:128],lr
		blt		loc_f2e0
		vst1.8		{d4-d5}, [r12,:128],lr
		vst1.8		{d8-d9}, [r12,:128],lr
		ble		loc_f2e0
		b		loc_f194
@ ---------------------------------------------------------------------------

loc_f2e0:				@ code xref: .text:0000f2ccj
					@ .text:0000f2d8j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_f114
		b		locret_f6c0
@ ---------------------------------------------------------------------------

loc_f2fc:				@ code xref: .text:0000f110j
		ands		r5, r0,	#3
		bne		loc_f4e8
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		sub		r4, r4,	#1
		subs		r7, r7,	#2
		ble		loc_f4fc
		sub		r7, r7,	#2

loc_f320:				@ code xref: .text:0000f130j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vmull.u8	q8, d4,	d13
		vmull.u8	q9, d5,	d13
		vmull.u8	q10, d30, d13
		vmlsl.u8	q8, d0,	d12
		vmlsl.u8	q9, d2,	d12
		vmlsl.u8	q10, d8, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vmlal.u8	q8, d4,	d14
		vmlal.u8	q9, d5,	d14
		vmlal.u8	q10, d30, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vmlsl.u8	q8, d4,	d15
		vmlsl.u8	q9, d5,	d15
		vmlsl.u8	q10, d30, d15

loc_f380:				@ code xref: .text:0000f4c8j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vmull.s16	q0, d18, d6[1]
		vmull.s16	q1, d19, d6[1]
		vmull.s16	q2, d20, d6[1]
		vmull.s16	q4, d21, d6[1]
		vmull.s16	q5, d22, d6[1]
		vmlsl.s16	q0, d16, d6[0]
		vmlsl.s16	q1, d17, d6[0]
		vmlsl.s16	q2, d18, d6[0]
		vmlsl.s16	q4, d19, d6[0]
		vmull.s16	q15, d23, d6[1]
		vmull.s16	q8, d24, d6[1]
		vmull.s16	q9, d25, d6[1]
		vmlsl.s16	q5, d20, d6[0]
		vmlsl.s16	q15, d21, d6[0]
		vmlsl.s16	q8, d22, d6[0]
		vmlsl.s16	q9, d23, d6[0]
		vmlal.s16	q0, d20, d6[2]
		vmlal.s16	q1, d21, d6[2]
		vmlal.s16	q2, d22, d6[2]
		vmlal.s16	q4, d23, d6[2]
		vmlal.s16	q5, d24, d6[2]
		vmlal.s16	q15, d25, d6[2]
		vmlal.s16	q8, d26, d6[2]
		vmlal.s16	q9, d27, d6[2]
		vmlsl.s16	q0, d22, d6[3]
		vmlsl.s16	q1, d23, d6[3]
		vmlsl.s16	q2, d24, d6[3]
		vmlsl.s16	q4, d25, d6[3]
		vmlsl.s16	q5, d26, d6[3]
		vmlsl.s16	q15, d27, d6[3]
		vmlsl.s16	q8, d28, d6[3]
		vmlsl.s16	q9, d29, d6[3]
		vshrn.i32	d0, q0,	#6
		vshrn.i32	d1, q1,	#6
		vshrn.i32	d2, q2,	#6
		vshrn.i32	d3, q4,	#6
		vshrn.i32	d4, q5,	#6
		vshrn.i32	d5, q15, #6
		vshrn.i32	d8, q8,	#6
		vshrn.i32	d9, q9,	#6
		vmov		q8, q12
		vmov		q9, q13
		vmov		q10, q14
		vst1.8		{d0}, [r12,:64],lr
		vst1.8		{d2}, [r12,:64],lr
		blt		loc_f4cc
		vst1.8		{d4}, [r12,:64],lr
		vst1.8		{d8}, [r12,:64],lr
		ble		loc_f4cc
		b		loc_f380
@ ---------------------------------------------------------------------------

loc_f4cc:				@ code xref: .text:0000f4b8j
					@ .text:0000f4c4j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_f114
		b		locret_f6c0
@ ---------------------------------------------------------------------------

loc_f4e8:				@ code xref: .text:0000f300j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		sub		r4, r4,	#1
		sub		r7, r7,	#2

loc_f4fc:				@ code xref: .text:0000f128j
					@ .text:0000f318j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vmull.u8	q8, d4,	d13
		vmull.u8	q9, d5,	d13
		vmull.u8	q10, d30, d13
		vmlsl.u8	q8, d0,	d12
		vmlsl.u8	q9, d2,	d12
		vmlsl.u8	q10, d8, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vmlal.u8	q8, d4,	d14
		vmlal.u8	q9, d5,	d14
		vmlal.u8	q10, d30, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vmlsl.u8	q8, d4,	d15
		vmlsl.u8	q9, d5,	d15
		vmlsl.u8	q10, d30, d15

loc_f55c:				@ code xref: .text:0000f6a4j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vmull.s16	q0, d18, d6[1]
		vmull.s16	q1, d19, d6[1]
		vmull.s16	q2, d20, d6[1]
		vmull.s16	q4, d21, d6[1]
		vmull.s16	q5, d22, d6[1]
		vmlsl.s16	q0, d16, d6[0]
		vmlsl.s16	q1, d17, d6[0]
		vmlsl.s16	q2, d18, d6[0]
		vmlsl.s16	q4, d19, d6[0]
		vmull.s16	q15, d23, d6[1]
		vmull.s16	q8, d24, d6[1]
		vmull.s16	q9, d25, d6[1]
		vmlsl.s16	q5, d20, d6[0]
		vmlsl.s16	q15, d21, d6[0]
		vmlsl.s16	q8, d22, d6[0]
		vmlsl.s16	q9, d23, d6[0]
		vmlal.s16	q0, d20, d6[2]
		vmlal.s16	q1, d21, d6[2]
		vmlal.s16	q2, d22, d6[2]
		vmlal.s16	q4, d23, d6[2]
		vmlal.s16	q5, d24, d6[2]
		vmlal.s16	q15, d25, d6[2]
		vmlal.s16	q8, d26, d6[2]
		vmlal.s16	q9, d27, d6[2]
		vmlsl.s16	q0, d22, d6[3]
		vmlsl.s16	q1, d23, d6[3]
		vmlsl.s16	q2, d24, d6[3]
		vmlsl.s16	q4, d25, d6[3]
		vmlsl.s16	q5, d26, d6[3]
		vmlsl.s16	q15, d27, d6[3]
		vmlsl.s16	q8, d28, d6[3]
		vmlsl.s16	q9, d29, d6[3]
		vshrn.i32	d0, q0,	#6
		vshrn.i32	d1, q1,	#6
		vshrn.i32	d2, q2,	#6
		vshrn.i32	d3, q4,	#6
		vshrn.i32	d4, q5,	#6
		vshrn.i32	d5, q15, #6
		vshrn.i32	d8, q8,	#6
		vshrn.i32	d9, q9,	#6
		vmov		q8, q12
		vmov		q9, q13
		vmov		q10, q14
		vst1.32		{d0[0]}, [r12,:32],lr
		vst1.32		{d2[0]}, [r12,:32],lr
		blt		loc_f6a8
		vst1.32		{d4[0]}, [r12,:32],lr
		vst1.32		{d8[0]}, [r12,:32],lr
		ble		loc_f6a8
		b		loc_f55c
@ ---------------------------------------------------------------------------

loc_f6a8:				@ code xref: .text:0000f694j
					@ .text:0000f6a0j
		mls		r12, lr, r6, r12
		add		r12, r12, #4
		add		r2, r2,	#2
		add		r0, r0,	#2
		cmp		r7, #0
		bgt		loc_f114

locret_f6c0:				@ code xref: .text:0000f2f8j
					@ .text:0000f4e4j
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

		.globl _VO_avg_MC_InterChroma_H0V0_neon
_VO_avg_MC_InterChroma_H0V0_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ldr		r12, [sp,#0x28]
		ldr		lr, [sp,#0x2c]

loc_f6d8:				@ code xref: .text:0000f794j
					@ .text:0000f864j ...
		cmp		r7, #0x20 @ ' '
		blt		loc_f79c
		ands		r5, r0,	#0xf
		bne		loc_f79c
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_f6f4:				@ code xref: .text:0000f77cj
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
		bgt		loc_f6f4
		mls		r12, lr, r6, r12
		add		r12, r12, #0x40
		add		r2, r2,	#0x20
		add		r0, r0,	#0x20
		subs		r7, r7,	#0x20
		bgt		loc_f6d8
		b		locret_fa08
@ ---------------------------------------------------------------------------

loc_f79c:				@ code xref: .text:0000f6dcj
					@ .text:0000f6e4j
		cmp		r7, #0x10
		blt		loc_f86c
		ands		r5, r0,	#0xf
		bne		loc_f86c
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_f7b8:				@ code xref: .text:0000f84cj
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
		bgt		loc_f7b8
		mls		r12, lr, r6, r12
		add		r12, r12, #0x20
		add		r2, r2,	#0x10
		add		r0, r0,	#0x10
		subs		r7, r7,	#0x10
		bgt		loc_f6d8
		b		locret_fa08
@ ---------------------------------------------------------------------------

loc_f86c:				@ code xref: .text:0000f7a0j
					@ .text:0000f7a8j
		cmp		r7, #8
		blt		loc_f910
		ands		r5, r0,	#7
		bne		loc_f910
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_f888:				@ code xref: .text:0000f8f0j
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
		blt		loc_f8f4
		vst1.64		{d2}, [r5,:64],r1
		vst1.64		{d3}, [r5,:64],r1
		bgt		loc_f888

loc_f8f4:				@ code xref: .text:0000f8e4j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		add		r2, r2,	#8
		add		r0, r0,	#8
		subs		r7, r7,	#8
		bgt		loc_f6d8
		b		locret_fa08
@ ---------------------------------------------------------------------------

loc_f910:				@ code xref: .text:0000f870j
					@ .text:0000f878j
		cmp		r7, #4
		blt		loc_f99c
		ands		r5, r0,	#3
		bne		loc_f99c
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_f92c:				@ code xref: .text:0000f97cj
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
		blt		loc_f980
		vst1.32		{d1[0]}, [r5,:32],r1
		vst1.32		{d1[1]}, [r5,:32],r1
		bgt		loc_f92c

loc_f980:				@ code xref: .text:0000f970j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		subs		r7, r7,	#4
		bgt		loc_f6d8
		b		locret_fa08
@ ---------------------------------------------------------------------------

loc_f99c:				@ code xref: .text:0000f914j
					@ .text:0000f91cj
		mov		r8, r6
		mov		r5, r0
		mov		r4, r2

loc_f9a8:				@ code xref: .text:0000f9ecj
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
		blt		loc_f9f0
		vst1.16		{d0[2]}, [r5,:16],r1
		vst1.16		{d0[3]}, [r5,:16],r1
		bgt		loc_f9a8

loc_f9f0:				@ code xref: .text:0000f9e0j
		mls		r12, lr, r6, r12
		add		r12, r12, #4
		add		r2, r2,	#2
		add		r0, r0,	#2
		subs		r7, r7,	#2
		bgt		loc_f6d8

locret_fa08:				@ code xref: .text:0000f798j
					@ .text:0000f868j ...
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------

@coeff_off_6: .long ChromaCoeff	
ChromaCoeff_table6:
	.short       0,  64,  0,    0
	.short       2,  58,  10,   2
	.short       4,  54,  16,   2
	.short       6,  46,  28,   4
	.short       4,  36,  36,   4
	.short       4,  28,  46,   6
	.short       2,  16,  54,   4
	.short       2,  10,  58,   2
		.globl _VO_avg_MC_InterChroma_H1V0_neon
_VO_avg_MC_InterChroma_H1V0_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r5, [sp,#0x30]
		adr   r9, ChromaCoeff_table6
		add		r10, r9, r5,lsl#3
		vld1.64		{d7}, [r10]
		vdup.8		d12, d7[0]
		vdup.8		d13, d7[2]
		vdup.8		d14, d7[4]
		vdup.8		d15, d7[6]
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ldr		r12, [sp,#0x28]
		ldr		lr, [sp,#0x2c]
		ands		r5, r0,	#7
		bne		loc_fb54

loc_fa4c:				@ code xref: .text:0000fb4cj
					@ .text:0000fc58j ...
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#1
		subs		r7, r7,	#2
		ble		loc_fc70
		subs		r7, r7,	#2
		ble		loc_fb74

loc_fa68:				@ code xref: .text:0000fb34j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vld1.16		{d0-d1}, [r12,:128],lr
		vld1.16		{d2-d3}, [r12,:128],lr
		vld1.16		{d8-d9}, [r12,:128],lr
		vld1.16		{d10-d11}, [r12,:128],lr
		vhadd.s16	q0, q0,	q11
		vhadd.s16	q1, q1,	q12
		vhadd.s16	q4, q4,	q13
		vhadd.s16	q5, q5,	q14
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vqrshrun.s16	d2, q4,	#6
		vqrshrun.s16	d3, q5,	#6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		blt		loc_fb38
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		ble		loc_fb38
		b		loc_fa68
@ ---------------------------------------------------------------------------

loc_fb38:				@ code xref: .text:0000fb24j
					@ .text:0000fb30j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_fa4c
		b		locret_fd58
@ ---------------------------------------------------------------------------

loc_fb54:				@ code xref: .text:0000fa48j
		ands		r5, r0,	#3
		bne		loc_fc60
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#1
		subs		r7, r7,	#2
		ble		loc_fc70
		sub		r7, r7,	#2

loc_fb74:				@ code xref: .text:0000fa64j
					@ .text:0000fc40j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vld1.16		{d0}, [r12,:64],lr
		vld1.16		{d2}, [r12,:64],lr
		vld1.16		{d8}, [r12,:64],lr
		vld1.16		{d10}, [r12,:64],lr
		vhadd.s16	d0, d0,	d22
		vhadd.s16	d2, d2,	d24
		vhadd.s16	d8, d8,	d26
		vhadd.s16	d10, d10, d28
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vqrshrun.s16	d2, q4,	#6
		vqrshrun.s16	d3, q5,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		blt		loc_fc44
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		ble		loc_fc44
		b		loc_fb74
@ ---------------------------------------------------------------------------

loc_fc44:				@ code xref: .text:0000fc30j
					@ .text:0000fc3cj
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_fa4c
		b		locret_fd58
@ ---------------------------------------------------------------------------

loc_fc60:				@ code xref: .text:0000fb58j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	#1
		sub		r7, r7,	#2

loc_fc70:				@ code xref: .text:0000fa5cj
					@ .text:0000fb6cj ...
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vld1.32		{d0[0]}, [r12,:32],lr
		vld1.32		{d2[0]}, [r12,:32],lr
		vld1.32		{d8[0]}, [r12,:32],lr
		vld1.32		{d10[0]}, [r12,:32],lr
		vhadd.s16	d0, d0,	d22
		vhadd.s16	d2, d2,	d24
		vhadd.s16	d8, d8,	d26
		vhadd.s16	d10, d10, d28
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vst1.16		{d0[0]}, [r5,:16],r1
		vst1.16		{d1[0]}, [r5,:16],r1
		blt		loc_fd40
		vqrshrun.s16	d2, q4,	#6
		vqrshrun.s16	d3, q5,	#6
		vst1.16		{d2[0]}, [r5,:16],r1
		vst1.16		{d3[0]}, [r5,:16],r1
		ble		loc_fd40
		b		loc_fc70
@ ---------------------------------------------------------------------------

loc_fd40:				@ code xref: .text:0000fd24j
					@ .text:0000fd38j
		mls		r12, lr, r6, r12
		add		r12, r12, #4
		add		r2, r2,	#2
		add		r0, r0,	#2
		cmp		r7, #0
		bgt		loc_fa4c

locret_fd58:				@ code xref: .text:0000fb50j
					@ .text:0000fc5cj
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------
@coeff_off_7: .long ChromaCoeff
ChromaCoeff_table7:
	.short       0,  64,  0,    0
	.short       2,  58,  10,   2
	.short       4,  54,  16,   2
	.short       6,  46,  28,   4
	.short       4,  36,  36,   4
	.short       4,  28,  46,   6
	.short       2,  16,  54,   4
	.short       2,  10,  58,   2
		.globl _VO_avg_MC_InterChroma_H0V1_neon
_VO_avg_MC_InterChroma_H0V1_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r6, [sp,#0x34]
		adr   r9, ChromaCoeff_table7
		add		r10, r9, r6,lsl#3
		vld1.64		{d6}, [r10]
		vdup.8		d12, d6[0]
		vdup.8		d13, d6[2]
		vdup.8		d14, d6[4]
		vdup.8		d15, d6[6]
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ldr		r12, [sp,#0x28]
		ldr		lr, [sp,#0x2c]
		ands		r5, r0,	#7
		bne		loc_fe8c

loc_fd9c:				@ code xref: .text:0000fe84j
					@ .text:0000ff78j ...
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		subs		r7, r7,	#2
		ble		loc_ff90
		subs		r7, r7,	#2
		ble		loc_feac
		vld1.64		{d16}, [r4],r3
		vld1.64		{d17}, [r4],r3
		vld1.64		{d18}, [r4],r3

loc_fdc4:				@ code xref: .text:0000fe6cj
		vld1.64		{d19}, [r4],r3
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		subs		r8, r8,	#4
		vmull.u8	q0, d17, d13
		vmull.u8	q1, d18, d13
		vmull.u8	q2, d19, d13
		vmull.u8	q3, d20, d13
		vmlsl.u8	q0, d16, d12
		vmlsl.u8	q1, d17, d12
		vmlsl.u8	q2, d18, d12
		vmlsl.u8	q3, d19, d12
		vmlal.u8	q0, d18, d14
		vmlal.u8	q1, d19, d14
		vmlal.u8	q2, d20, d14
		vmlal.u8	q3, d21, d14
		vmlsl.u8	q0, d19, d15
		vmlsl.u8	q1, d20, d15
		vmlsl.u8	q2, d21, d15
		vmlsl.u8	q3, d22, d15
		vmov		d16, d20
		vmov		d17, d21
		vmov		d18, d22
		vld1.16		{d8-d9}, [r12,:128],lr
		vld1.16		{d10-d11}, [r12,:128],lr
		vld1.16		{d28-d29}, [r12,:128],lr
		vld1.16		{d30-d31}, [r12,:128],lr
		vhadd.s16	q0, q0,	q4
		vhadd.s16	q1, q1,	q5
		vhadd.s16	q2, q2,	q14
		vhadd.s16	q3, q3,	q15
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		blt		loc_fe70
		vqrshrun.s16	d2, q2,	#6
		vqrshrun.s16	d3, q3,	#6
		vst1.8		{d2}, [r5,:64],r1
		vst1.8		{d3}, [r5,:64],r1
		ble		loc_fe70
		b		loc_fdc4
@ ---------------------------------------------------------------------------

loc_fe70:				@ code xref: .text:0000fe54j
					@ .text:0000fe68j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_fd9c
		b		locret_10060
@ ---------------------------------------------------------------------------

loc_fe8c:				@ code xref: .text:0000fd98j
		ands		r5, r0,	#3
		bne		loc_ff80
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		subs		r7, r7,	#2
		ble		loc_ff90
		sub		r7, r7,	#2

loc_feac:				@ code xref: .text:0000fdb4j
		vld1.64		{d16}, [r4],r3
		vld1.64		{d17}, [r4],r3
		vld1.64		{d18}, [r4],r3

loc_feb8:				@ code xref: .text:0000ff60j
		vld1.64		{d19}, [r4],r3
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		subs		r8, r8,	#4
		vmull.u8	q0, d17, d13
		vmull.u8	q1, d18, d13
		vmull.u8	q2, d19, d13
		vmull.u8	q3, d20, d13
		vmlsl.u8	q0, d16, d12
		vmlsl.u8	q1, d17, d12
		vmlsl.u8	q2, d18, d12
		vmlsl.u8	q3, d19, d12
		vmlal.u8	q0, d18, d14
		vmlal.u8	q1, d19, d14
		vmlal.u8	q2, d20, d14
		vmlal.u8	q3, d21, d14
		vmlsl.u8	q0, d19, d15
		vmlsl.u8	q1, d20, d15
		vmlsl.u8	q2, d21, d15
		vmlsl.u8	q3, d22, d15
		vmov		d16, d20
		vmov		d17, d21
		vmov		d18, d22
		vld1.16		{d8}, [r12,:64],lr
		vld1.16		{d9}, [r12,:64],lr
		vld1.16		{d10}, [r12,:64],lr
		vld1.16		{d11}, [r12,:64],lr
		vhadd.s16	d0, d0,	d8
		vhadd.s16	d2, d2,	d9
		vhadd.s16	d4, d4,	d10
		vhadd.s16	d6, d6,	d11
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		blt		loc_ff64
		vqrshrun.s16	d2, q2,	#6
		vqrshrun.s16	d3, q3,	#6
		vst1.32		{d2[0]}, [r5,:32],r1
		vst1.32		{d3[0]}, [r5,:32],r1
		ble		loc_ff64
		b		loc_feb8
@ ---------------------------------------------------------------------------

loc_ff64:				@ code xref: .text:0000ff48j
					@ .text:0000ff5cj
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_fd9c
		b		locret_10060
@ ---------------------------------------------------------------------------

loc_ff80:				@ code xref: .text:0000fe90j
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		sub		r7, r7,	#2

loc_ff90:				@ code xref: .text:0000fdacj
					@ .text:0000fea4j
		vld1.64		{d16}, [r4],r3
		vld1.64		{d17}, [r4],r3
		vld1.64		{d18}, [r4],r3

loc_ff9c:				@ code xref: .text:00010044j
		vld1.64		{d19}, [r4],r3
		vld1.64		{d20}, [r4],r3
		vld1.64		{d21}, [r4],r3
		vld1.64		{d22}, [r4],r3
		subs		r8, r8,	#4
		vmull.u8	q0, d17, d13
		vmull.u8	q1, d18, d13
		vmull.u8	q2, d19, d13
		vmull.u8	q3, d20, d13
		vmlsl.u8	q0, d16, d12
		vmlsl.u8	q1, d17, d12
		vmlsl.u8	q2, d18, d12
		vmlsl.u8	q3, d19, d12
		vmlal.u8	q0, d18, d14
		vmlal.u8	q1, d19, d14
		vmlal.u8	q2, d20, d14
		vmlal.u8	q3, d21, d14
		vmlsl.u8	q0, d19, d15
		vmlsl.u8	q1, d20, d15
		vmlsl.u8	q2, d21, d15
		vmlsl.u8	q3, d22, d15
		vmov		d16, d20
		vmov		d17, d21
		vmov		d18, d22
		vld1.32		{d8[0]}, [r12,:32],lr
		vld1.32		{d9[0]}, [r12,:32],lr
		vld1.32		{d10[0]}, [r12,:32],lr
		vld1.32		{d11[0]}, [r12,:32],lr
		vhadd.s16	d0, d0,	d8
		vhadd.s16	d2, d2,	d9
		vhadd.s16	d4, d4,	d10
		vhadd.s16	d6, d6,	d11
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vqrshrun.s16	d2, q2,	#6
		vqrshrun.s16	d3, q3,	#6
		vst1.16		{d0[0]}, [r5,:16],r1
		vst1.16		{d1[0]}, [r5,:16],r1
		blt		loc_10048
		vst1.16		{d2[0]}, [r5,:16],r1
		vst1.16		{d3[0]}, [r5,:16],r1
		ble		loc_10048
		b		loc_ff9c
@ ---------------------------------------------------------------------------

loc_10048:				@ code xref: .text:00010034j
					@ .text:00010040j
		mls		r12, lr, r6, r12
		add		r12, r12, #4
		add		r2, r2,	#2
		add		r0, r0,	#2
		cmp		r7, #0
		bgt		loc_fd9c

locret_10060:				@ code xref: .text:0000fe88j
					@ .text:0000ff7cj
		ldmfd		sp!, {r4-r10,pc}
@ ---------------------------------------------------------------------------
@coeff_off_8: .long ChromaCoeff
ChromaCoeff_table8:
	.short       0,  64,  0,    0
	.short       2,  58,  10,   2
	.short       4,  54,  16,   2
	.short       6,  46,  28,   4
	.short       4,  36,  36,   4
	.short       4,  28,  46,   6
	.short       2,  16,  54,   4
	.short       2,  10,  58,   2
		.globl _VO_avg_MC_InterChroma_H1V1_neon
_VO_avg_MC_InterChroma_H1V1_neon:
		stmfd		sp!, {r4-r10,lr}
		ldr		r5, [sp,#0x30]
		ldr		r6, [sp,#0x34]
		adr   r9, ChromaCoeff_table8
		add		r10, r9, r5,lsl#3
		vld1.64		{d7}, [r10]
		add		r10, r9, r6,lsl#3
		vld1.64		{d6}, [r10]
		vdup.8		d12, d7[0]
		vdup.8		d13, d7[2]
		vdup.8		d14, d7[4]
		vdup.8		d15, d7[6]
		ldr		r7, [sp,#0x20]
		ldr		r6, [sp,#0x24]
		ldr		r12, [sp,#0x28]
		ldr		lr, [sp,#0x2c]
		ands		r5, r0,	#7
		bne		loc_102c8

loc_100b0:				@ code xref: .text:000102c0j
					@ .text:000104dcj ...
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		sub		r4, r4,	#1
		subs		r7, r7,	#2
		ble		loc_104f8
		subs		r7, r7,	#2
		ble		loc_102ec
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vmull.u8	q8, d4,	d13
		vmull.u8	q9, d5,	d13
		vmull.u8	q10, d30, d13
		vmlsl.u8	q8, d0,	d12
		vmlsl.u8	q9, d2,	d12
		vmlsl.u8	q10, d8, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vmlal.u8	q8, d4,	d14
		vmlal.u8	q9, d5,	d14
		vmlal.u8	q10, d30, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vmlsl.u8	q8, d4,	d15
		vmlsl.u8	q9, d5,	d15
		vmlsl.u8	q10, d30, d15

loc_10130:				@ code xref: .text:000102a8j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vmull.s16	q0, d18, d6[1]
		vmull.s16	q1, d19, d6[1]
		vmull.s16	q2, d20, d6[1]
		vmull.s16	q4, d21, d6[1]
		vmull.s16	q5, d22, d6[1]
		vmlsl.s16	q0, d16, d6[0]
		vmlsl.s16	q1, d17, d6[0]
		vmlsl.s16	q2, d18, d6[0]
		vmlsl.s16	q4, d19, d6[0]
		vmull.s16	q15, d23, d6[1]
		vmull.s16	q8, d24, d6[1]
		vmull.s16	q9, d25, d6[1]
		vmlsl.s16	q5, d20, d6[0]
		vmlsl.s16	q15, d21, d6[0]
		vmlsl.s16	q8, d22, d6[0]
		vmlsl.s16	q9, d23, d6[0]
		vmlal.s16	q0, d20, d6[2]
		vmlal.s16	q1, d21, d6[2]
		vmlal.s16	q2, d22, d6[2]
		vmlal.s16	q4, d23, d6[2]
		vmlal.s16	q5, d24, d6[2]
		vmlal.s16	q15, d25, d6[2]
		vmlal.s16	q8, d26, d6[2]
		vmlal.s16	q9, d27, d6[2]
		vmlsl.s16	q0, d22, d6[3]
		vmlsl.s16	q1, d23, d6[3]
		vmlsl.s16	q2, d24, d6[3]
		vmlsl.s16	q4, d25, d6[3]
		vmlsl.s16	q5, d26, d6[3]
		vmlsl.s16	q15, d27, d6[3]
		vmlsl.s16	q8, d28, d6[3]
		vmlsl.s16	q9, d29, d6[3]
		vshrn.i32	d0, q0,	#6
		vshrn.i32	d1, q1,	#6
		vshrn.i32	d2, q2,	#6
		vshrn.i32	d3, q4,	#6
		vshrn.i32	d4, q5,	#6
		vshrn.i32	d5, q15, #6
		vshrn.i32	d8, q8,	#6
		vshrn.i32	d9, q9,	#6
		vmov		q8, q12
		vmov		q9, q13
		vmov		q10, q14
		vld1.16		{d22-d23}, [r12,:128],lr
		vld1.16		{d24-d25}, [r12,:128],lr
		vhadd.s16	q0, q0,	q11
		vhadd.s16	q1, q1,	q12
		vld1.16		{d26-d27}, [r12,:128],lr
		vld1.16		{d28-d29}, [r12,:128],lr
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vhadd.s16	q2, q2,	q13
		vhadd.s16	q4, q4,	q14
		vst1.8		{d0}, [r5,:64],r1
		vst1.8		{d1}, [r5,:64],r1
		blt		loc_102ac
		vqrshrun.s16	d4, q2,	#6
		vqrshrun.s16	d5, q4,	#6
		vst1.8		{d4}, [r5,:64],r1
		vst1.8		{d5}, [r5,:64],r1
		ble		loc_102ac
		b		loc_10130
@ ---------------------------------------------------------------------------

loc_102ac:				@ code xref: .text:00010290j
					@ .text:000102a4j
		mls		r12, lr, r6, r12
		add		r12, r12, #0x10
		subs		r7, r7,	#4
		add		r2, r2,	#8
		add		r0, r0,	#8
		bgt		loc_100b0
		b		locret_106ec
@ ---------------------------------------------------------------------------

loc_102c8:				@ code xref: .text:000100acj
		ands		r5, r0,	#3
		bne		loc_104e4
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		sub		r4, r4,	#1
		subs		r7, r7,	#2
		ble		loc_104f8
		sub		r7, r7,	#2

loc_102ec:				@ code xref: .text:000100ccj
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vmull.u8	q8, d4,	d13
		vmull.u8	q9, d5,	d13
		vmull.u8	q10, d30, d13
		vmlsl.u8	q8, d0,	d12
		vmlsl.u8	q9, d2,	d12
		vmlsl.u8	q10, d8, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vmlal.u8	q8, d4,	d14
		vmlal.u8	q9, d5,	d14
		vmlal.u8	q10, d30, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vmlsl.u8	q8, d4,	d15
		vmlsl.u8	q9, d5,	d15
		vmlsl.u8	q10, d30, d15

loc_1034c:				@ code xref: .text:000104c4j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vmull.s16	q0, d18, d6[1]
		vmull.s16	q1, d19, d6[1]
		vmull.s16	q2, d20, d6[1]
		vmull.s16	q4, d21, d6[1]
		vmull.s16	q5, d22, d6[1]
		vmlsl.s16	q0, d16, d6[0]
		vmlsl.s16	q1, d17, d6[0]
		vmlsl.s16	q2, d18, d6[0]
		vmlsl.s16	q4, d19, d6[0]
		vmull.s16	q15, d23, d6[1]
		vmull.s16	q8, d24, d6[1]
		vmull.s16	q9, d25, d6[1]
		vmlsl.s16	q5, d20, d6[0]
		vmlsl.s16	q15, d21, d6[0]
		vmlsl.s16	q8, d22, d6[0]
		vmlsl.s16	q9, d23, d6[0]
		vmlal.s16	q0, d20, d6[2]
		vmlal.s16	q1, d21, d6[2]
		vmlal.s16	q2, d22, d6[2]
		vmlal.s16	q4, d23, d6[2]
		vmlal.s16	q5, d24, d6[2]
		vmlal.s16	q15, d25, d6[2]
		vmlal.s16	q8, d26, d6[2]
		vmlal.s16	q9, d27, d6[2]
		vmlsl.s16	q0, d22, d6[3]
		vmlsl.s16	q1, d23, d6[3]
		vmlsl.s16	q2, d24, d6[3]
		vmlsl.s16	q4, d25, d6[3]
		vmlsl.s16	q5, d26, d6[3]
		vmlsl.s16	q15, d27, d6[3]
		vmlsl.s16	q8, d28, d6[3]
		vmlsl.s16	q9, d29, d6[3]
		vshrn.i32	d0, q0,	#6
		vshrn.i32	d1, q1,	#6
		vshrn.i32	d2, q2,	#6
		vshrn.i32	d3, q4,	#6
		vshrn.i32	d4, q5,	#6
		vshrn.i32	d5, q15, #6
		vshrn.i32	d8, q8,	#6
		vshrn.i32	d9, q9,	#6
		vmov		q8, q12
		vmov		q9, q13
		vmov		q10, q14
		vld1.16		{d10}, [r12,:64],lr
		vld1.16		{d11}, [r12,:64],lr
		vld1.16		{d30}, [r12,:64],lr
		vld1.16		{d31}, [r12,:64],lr
		vhadd.s16	d0, d0,	d10
		vhadd.s16	d2, d2,	d11
		vhadd.s16	d4, d4,	d30
		vhadd.s16	d8, d8,	d31
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vst1.32		{d0[0]}, [r5,:32],r1
		vst1.32		{d1[0]}, [r5,:32],r1
		blt		loc_104c8
		vqrshrun.s16	d4, q2,	#6
		vqrshrun.s16	d5, q4,	#6
		vst1.32		{d4[0]}, [r5,:32],r1
		vst1.32		{d5[0]}, [r5,:32],r1
		ble		loc_104c8
		b		loc_1034c
@ ---------------------------------------------------------------------------

loc_104c8:				@ code xref: .text:000104acj
					@ .text:000104c0j
		mls		r12, lr, r6, r12
		add		r12, r12, #8
		add		r2, r2,	#4
		add		r0, r0,	#4
		cmp		r7, #0
		bgt		loc_100b0
		b		locret_106ec
@ ---------------------------------------------------------------------------

loc_104e4:				@ code xref: .text:000102ccj
		mov		r8, r6
		mov		r5, r0
		sub		r4, r2,	r3
		sub		r4, r4,	#1
		sub		r7, r7,	#2

loc_104f8:				@ code xref: .text:000100c4j
					@ .text:000102e4j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vmull.u8	q8, d4,	d13
		vmull.u8	q9, d5,	d13
		vmull.u8	q10, d30, d13
		vmlsl.u8	q8, d0,	d12
		vmlsl.u8	q9, d2,	d12
		vmlsl.u8	q10, d8, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vmlal.u8	q8, d4,	d14
		vmlal.u8	q9, d5,	d14
		vmlal.u8	q10, d30, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vmlsl.u8	q8, d4,	d15
		vmlsl.u8	q9, d5,	d15
		vmlsl.u8	q10, d30, d15

loc_10558:				@ code xref: .text:000106d0j
		vld1.64		{d0-d1}, [r4],r3
		vld1.64		{d2-d3}, [r4],r3
		vld1.64		{d8-d9}, [r4],r3
		vld1.64		{d10-d11}, [r4],r3
		subs		r8, r8,	#4
		vext.8		d4, d0,	d1, #1
		vext.8		d5, d2,	d3, #1
		vext.8		d30, d8, d9, #1
		vext.8		d31, d10, d11, #1
		vmull.u8	q11, d4, d13
		vmull.u8	q12, d5, d13
		vmull.u8	q13, d30, d13
		vmull.u8	q14, d31, d13
		vmlsl.u8	q11, d0, d12
		vmlsl.u8	q12, d2, d12
		vmlsl.u8	q13, d8, d12
		vmlsl.u8	q14, d10, d12
		vext.8		d4, d0,	d1, #2
		vext.8		d5, d2,	d3, #2
		vext.8		d30, d8, d9, #2
		vext.8		d31, d10, d11, #2
		vmlal.u8	q11, d4, d14
		vmlal.u8	q12, d5, d14
		vmlal.u8	q13, d30, d14
		vmlal.u8	q14, d31, d14
		vext.8		d4, d0,	d1, #3
		vext.8		d5, d2,	d3, #3
		vext.8		d30, d8, d9, #3
		vext.8		d31, d10, d11, #3
		vmlsl.u8	q11, d4, d15
		vmlsl.u8	q12, d5, d15
		vmlsl.u8	q13, d30, d15
		vmlsl.u8	q14, d31, d15
		vmull.s16	q0, d18, d6[1]
		vmull.s16	q1, d19, d6[1]
		vmull.s16	q2, d20, d6[1]
		vmull.s16	q4, d21, d6[1]
		vmull.s16	q5, d22, d6[1]
		vmlsl.s16	q0, d16, d6[0]
		vmlsl.s16	q1, d17, d6[0]
		vmlsl.s16	q2, d18, d6[0]
		vmlsl.s16	q4, d19, d6[0]
		vmull.s16	q15, d23, d6[1]
		vmull.s16	q8, d24, d6[1]
		vmull.s16	q9, d25, d6[1]
		vmlsl.s16	q5, d20, d6[0]
		vmlsl.s16	q15, d21, d6[0]
		vmlsl.s16	q8, d22, d6[0]
		vmlsl.s16	q9, d23, d6[0]
		vmlal.s16	q0, d20, d6[2]
		vmlal.s16	q1, d21, d6[2]
		vmlal.s16	q2, d22, d6[2]
		vmlal.s16	q4, d23, d6[2]
		vmlal.s16	q5, d24, d6[2]
		vmlal.s16	q15, d25, d6[2]
		vmlal.s16	q8, d26, d6[2]
		vmlal.s16	q9, d27, d6[2]
		vmlsl.s16	q0, d22, d6[3]
		vmlsl.s16	q1, d23, d6[3]
		vmlsl.s16	q2, d24, d6[3]
		vmlsl.s16	q4, d25, d6[3]
		vmlsl.s16	q5, d26, d6[3]
		vmlsl.s16	q15, d27, d6[3]
		vmlsl.s16	q8, d28, d6[3]
		vmlsl.s16	q9, d29, d6[3]
		vshrn.i32	d0, q0,	#6
		vshrn.i32	d1, q1,	#6
		vshrn.i32	d2, q2,	#6
		vshrn.i32	d3, q4,	#6
		vshrn.i32	d4, q5,	#6
		vshrn.i32	d5, q15, #6
		vshrn.i32	d8, q8,	#6
		vshrn.i32	d9, q9,	#6
		vmov		q8, q12
		vmov		q9, q13
		vmov		q10, q14
		vld1.32		{d10[0]}, [r12,:32],lr
		vld1.32		{d11[0]}, [r12,:32],lr
		vld1.32		{d30[0]}, [r12,:32],lr
		vld1.32		{d31[0]}, [r12,:32],lr
		vhadd.s16	d0, d0,	d10
		vhadd.s16	d2, d2,	d11
		vhadd.s16	d4, d4,	d30
		vhadd.s16	d8, d8,	d31
		vqrshrun.s16	d0, q0,	#6
		vqrshrun.s16	d1, q1,	#6
		vst1.16		{d0[0]}, [r5,:16],r1
		vst1.16		{d1[0]}, [r5,:16],r1
		blt		loc_106d4
		vqrshrun.s16	d4, q2,	#6
		vqrshrun.s16	d5, q4,	#6
		vst1.16		{d4[0]}, [r5,:16],r1
		vst1.16		{d5[0]}, [r5,:16],r1
		ble		loc_106d4
		b		loc_10558
@ ---------------------------------------------------------------------------

loc_106d4:				@ code xref: .text:000106b8j
					@ .text:000106ccj
		mls		r12, lr, r6, r12
		add		r12, r12, #4
		add		r2, r2,	#2
		add		r0, r0,	#2
		cmp		r7, #0
		bgt		loc_100b0

locret_106ec:				@ code xref: .text:000102c4j
					@ .text:000104e0j
		ldmfd		sp!, {r4-r10,pc}		
		
		
		.endif
