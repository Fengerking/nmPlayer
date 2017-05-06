@	/************************************************************************
@	*																		*
@	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
@	*																		*
@	************************************************************************/


	.section .text
	.global  quant_h263_inter_wmmx
	.extern  __voMPEG4E0221  @__voH263E0221dc_scalar @__voH263E0221 
	.extern  __voMPEG4E0220 @__voH263E0220 quant_multi 
	.type    quant_h263_inter_wmmx, function

quant_h263_inter_wmmx:
.ALIGN 4

	stmdb		sp!, {r4,r5, lr}		@ save regs used
	ldr			r3, data_mul
	add			r4, r3, r2, LSL #2		@ get the start of quant_multi
	wldrd		wr0, [r1], #8			@ 1st 4 data
	ldr			r5, [r4]				@ get mul
	wldrd		wr1, [r1], #8			@ 2nd 4 data
	tbcsth		wr14, r5				@ wr14 = mul
	mov			r5, #16				
	tmcr		wcgr0, r5				@ for shift
	mov			r5, r2, ASR #1			@ quant_d_2
	tbcsth		wr11, r5
	wzero		wr13					@ for compare
	mov			r5, #8					@ for count

LLOOP:
	subs		r5, r5, #1	

	wsrahg		wr2, wr0, wcgr0			@ extract sign mask
	wxor		wr4, wr0, wr2	
	wsrahg		wr3, wr1, wcgr0			@ extract sign mask			
	wxor		wr5, wr1, wr3
	wldrdne		wr0, [r1], #8			@ 1st 4 data
	wsubhss		wr4, wr4, wr2
	wsubhss		wr5, wr5, wr3
	wldrdne		wr1, [r1], #8			@ 2nd 4 data	
	wsubhus		wr4, wr4, wr11			@ acLevel = |acLevel| - quant_d_2;
	wsubhus		wr5, wr5, wr11

	wmulum		wr4, wr4, wr14			@ acLevel = (acLevel * mult) >> SCALEBITS;
	wmulum		wr5, wr5, wr14

	waddh		wr13, wr13, wr4			@ collect SAD
	waddh		wr13, wr13, wr5

	wxor		wr4, wr4, wr2
	wxor		wr5, wr5, wr3

	wsubhss		wr4, wr4, wr2
	wsubhss		wr5, wr5, wr3

	wstrd		wr4, [r0], #8
	wstrd		wr5, [r0], #8


	bne			LLOOP

	wacch		wr15, wr13
 
    textrmsh	r0, wr15, #0   @ return result
    ldmia		sp!, {r4,r5, pc}		@ restore and return   


	.section .text
	.global  dequant_h263_inter_wmmx
	.type    dequant_h263_inter_wmmx, function

dequant_h263_inter_wmmx:
.ALIGN 4

	stmdb		sp!, {r4,r5, lr}		@ save regs used
	mov			r3, r2, lsl #1			@ r3 = quant << 1
	wldrd		wr0, [r1], #8
	sub			r4, r2, #1				@ r4 = quant - 1
	wldrd		wr1, [r1], #8
	and			r2, r2, #1				@ r2 = quant&1
	wldrd		wr8, [r1], #8
	add			r2, r2, r4				@ r2 = quant-1+quant&1
	wldrd		wr9, [r1], #8
	ldr			r5, =#2047
	tbcsth		wr15, r5				@ 2048
	ldr			r5, =#-2048
	tbcsth		wr14, r5				@ -2048
	tbcsth		wr13, r2				@ wr13 = quant_add
	wzero		wr12
	wsubh		wr12, wr12, wr13		@ wr12 = -quant_add

	wzero		wr11
	tbcsth		wr10, r3
	mov			r5, #4					@ for count

LLOOP_DEQUANT_INTER:
	@1
	subs		r5, r5, #1	
	wcmpgtsh	wr2, wr0, wr11
	wand		wr2, wr2, wr13
	wcmpgtsh	wr3, wr11, wr0
	wand		wr3, wr3, wr12
	wor			wr2, wr2, wr3

	wcmpgtsh	wr4, wr1, wr11
	wand		wr4, wr4, wr13
	wcmpgtsh	wr5, wr11, wr1
	wand		wr5, wr5, wr12
	wmulsl		wr6, wr0, wr10
	wor			wr4, wr4, wr5
	wmulsl		wr7, wr1, wr10

	waddh		wr6, wr6, wr2
	wldrd		wr0, [r1], #8
	waddh		wr7, wr7, wr4
	wldrd		wr1, [r1], #8

	wminsh		wr6, wr6, wr15
	wmaxsh		wr6, wr6, wr14
	wminsh		wr7, wr7, wr15
	wmaxsh		wr7, wr7, wr14


	@2
	wcmpgtsh	wr2, wr8, wr11
	wstrd		wr6, [r0], #8
	wand		wr2, wr2, wr13
	wstrd		wr7, [r0], #8
	wcmpgtsh	wr3, wr11, wr8
	wand		wr3, wr3, wr12
	wor			wr2, wr2, wr3

	wcmpgtsh	wr4, wr9, wr11
	wand		wr4, wr4, wr13
	wcmpgtsh	wr5, wr11, wr9
	wand		wr5, wr5, wr12
	wmulsl		wr6, wr8, wr10
	wor			wr4, wr4, wr5
	wmulsl		wr7, wr9, wr10

	waddh		wr6, wr6, wr2
	waddh		wr7, wr7, wr4

	wldrdne		wr8, [r1], #8
	wminsh		wr6, wr6, wr15
	wldrdne		wr9, [r1], #8
	wmaxsh		wr6, wr6, wr14

	wminsh		wr7, wr7, wr15
	wstrd		wr6, [r0], #8
	wmaxsh		wr7, wr7, wr14
	wstrd		wr7, [r0], #8

	bne			LLOOP_DEQUANT_INTER

    ldmia		sp!, {r4,r5, pc}		@ restore and return   


	.section .text
	.global  quant_h263_intra_wmmx
	.type    quant_h263_intra_wmmx, function

quant_h263_intra_wmmx:
.ALIGN 4
	stmdb		sp!, {r4-r7, lr}		@ save regs used
	ldr			r4, data_mul			@ load quant_multi
	ldr			r5, data_dc			@ load dc_scalar table
	mov			r7, r1
	add			r4, r4, r2, LSL #2		@ get the start of quant_multi
	wldrd		wr0, [r1], #8			@ 1st 4 data
	ldr			r4, [r4]				@ r4 = mul
	wldrd		wr1, [r1], #8			@ 2nd 4 data
	tbcsth		wr14, r4				@ wr14 = mul
	add			r5, r5, r3, LSL #2		@ get the start of dc_scalar
	ldr			r5, [r5]				@ r5 = dc_scalar
	mov			r6, #16				
	tmcr		wcgr0, r6				@ for shift
	tbcsth		wr11, r6


	ldrsh		r4, [r7]				@ r4 = data[0]
	cmp			r4, #0
	mov			r6, r3, ASR	#1			@ r6 = (b)>>1
	rsbmi 		r6, r6, #0
	add			r4, r6, r4
	mul			r4, r4, r5
	mov			r4, r4, ASR #16			@ coeff[0]
	mov			r6, #8					@ for count
	mov			r7, r0	
				@ back address for coeff[0]
LLOOP1:
	subs		r6, r6, #1	
	wsrahg		wr2, wr0, wcgr0			@ extract sign mask
	wxor		wr4, wr0, wr2	
	wsrahg		wr3, wr1, wcgr0			@ extract sign mask			
	wxor		wr5, wr1, wr3
	wldrdne		wr0, [r1], #8			@ 1st 4 data
	wsubhss		wr4, wr4, wr2
	wsubhss		wr5, wr5, wr3
	wldrdne		wr1, [r1], #8			@ 2nd 4 data	

	wmulum		wr4, wr4, wr14			@ acLevel = (acLevel * mult) >> SCALEBITS;
	wmulum		wr5, wr5, wr14

	wxor		wr4, wr4, wr2
	wxor		wr5, wr5, wr3

	wsubhss		wr4, wr4, wr2
	wsubhss		wr5, wr5, wr3

	wstrd		wr4, [r0], #8
	wstrd		wr5, [r0], #8

	bne			LLOOP1
	strh		r4, [r7]
	
    ldmia		sp!, {r4-r7, pc}		@ restore and return   

@uint32_t
@dequant_h263_intra_c(int16_t * data,
@					 const int16_t * coeff,
@					 const uint32_t quant,
@					 const uint32_t dcscalar,
@					 const uint16_t * mpeg_quant_matrices)
@{
@	const int32_t quant_m_2 = quant << 1;
@	const int32_t quant_add = (quant & 1 ? quant : quant - 1);
@	int i;
@
@	data[0] = coeff[0] * dcscalar;
@	if (data[0] < -2048) {
@		data[0] = -2048;
@	} else if (data[0] > 2047) {
@		data[0] = 2047;
@	}
@
@	for (i = 1; i < 64; i++) {
@		int32_t acLevel = coeff[i];
@
@		if (acLevel == 0) {
@			data[i] = 0;
@		} else if (acLevel < 0) {
@			acLevel = quant_m_2 * -acLevel + quant_add;
@			data[i] = (acLevel <= 2048 ? -acLevel : -2048);
@		} else {
@			acLevel = quant_m_2 * acLevel + quant_add;
@			data[i] = (acLevel <= 2047 ? acLevel : 2047);
@		}
@	}
@
@	return(0);
@}
@
@

	.section .text
	.global  dequant_h263_intra_wmmx
	.type    dequant_h263_intra_wmmx, function

dequant_h263_intra_wmmx:
.ALIGN 4
	stmdb		sp!, {r4-r8, lr}		@ save regs used
	ldrsh		r8, [r1]				@ r8 = coeff[0]
	wldrd		wr0, [r1], #8			@ 1st 4 data
	mov			r4, r2, LSL #1			@ r4 = quant_m_2 = quant << 1;
	wldrd		wr1, [r1], #8			@ 1st 4 data
	and			r5, r2, #1				@ r5 = quant&1
	sub			r5, r5, #1				
	add			r5, r5, r2				@ r5 = quant_add
	mov			r6, #16				
	tmcr		wcgr0, r6				@ for shift
	tbcsth		wr15, r4				@ wr15 = quant_m_2
	tbcsth		wr14, r5				@ wr14 = quant_add

	ldr			r4, =-2048				@
	ldr			r5, =2047				@
	tbcsth		wr13, r4				@ wr15 = -2048
	tbcsth		wr12, r5				@ wr14 = 2047
	wzero		wr11

	mul			r8, r8, r3				@ r6 = coeff[0] * dcscalar;
	cmp			r8, r4
	movlt		r8, r4		
	cmp			r8, r5
	movgt		r8, r5

	mov			r6, #8					@ for count
	mov			r7, r0	
				@ back address for coeff[0]
LLOOP2:
	subs		r6, r6, #1	
	wsrahg		wr2, wr0, wcgr0			@ extract sign mask
	wxor		wr4, wr0, wr2	
	wsrahg		wr3, wr1, wcgr0			@ extract sign mask			
	wxor		wr5, wr1, wr3
	wldrdne		wr0, [r1], #8			@ 1st 4 data
	wsubhss		wr4, wr4, wr2
	wsubhss		wr5, wr5, wr3
	wldrdne		wr1, [r1], #8			@ 2nd 4 data	
	wcmpgtsh	wr6, wr4, wr11
	wcmpgtsh	wr7, wr5, wr11

	wmulsl		wr4, wr4, wr15			@ acLevel = quant_m_2 * |acLevel|
	wmulsl		wr5, wr5, wr15

	waddh		wr4, wr4, wr14			@ acLevel = quant_m_2 * |acLevel| + quant_add
	waddh		wr5, wr5, wr14			

	wxor		wr4, wr4, wr2
	wxor		wr5, wr5, wr3

	wsubhss		wr4, wr4, wr2
	wsubhss		wr5, wr5, wr3

	wmaxsh		wr4, wr4, wr13
	wminsh		wr4, wr4, wr12

	wmaxsh		wr5, wr5, wr13
	wminsh		wr5, wr5, wr12

	wand		wr4, wr4, wr6
	wand		wr5, wr5, wr7

	wstrd		wr4, [r0], #8
	wstrd		wr5, [r0], #8

	bne			LLOOP2
	strh		r8, [r7]
	
    ldmia		sp!, {r4-r8, pc}		@ restore and return   

data_mul:
.word __voMPEG4E0220 		@__voH263E0220quant_multi __voH263E0220
data_dc:
.word __voMPEG4E0221   @__voH263E0221dc_scalar __voH263E0221 