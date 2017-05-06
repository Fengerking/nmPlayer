    #include "../../Src/c/voVC1DID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 
    
    .if WMV_OPT_MOTIONCOMP_ARM == 1
    
	@AREA	|.text|, CODE, READONLY
     .text
     .align 4
	
    .globl  _g_AddError_EMB_Overflow_C	
	.globl	_ARMV7_g_AddError_SSIMD
	

@Void_WMV g_AddError_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   WMV_LEAF_ENTRY ARMV7_g_AddError_SSIMD
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
@r0 == ppxlcDst
@r1 == pRef
@r2 == pErrorBuf
@r3 == iPitch
@q6~q9   == err_even...
@q10~q13 == err_odd...
@q0~q2   == u0..., u2...
@q3~q5   == u1..., u3...
@q15 == 0x80008000,0x80008000,0x80008000,0x80008000
@q14 == err_overflow

    stmdb     sp!, {r4 - r6, r14}
    FRAME_PROFILE_COUNT
    
	vldmia		r2, {q6-q13}		@load error data from pErrorBuf[].

	add			r4, r1, #20
	mov			r5, #40
	mov			r6, r1
	vld1.64		d0 , [r6], r5	@u0 == pRef[0]@
	vld1.64		d1 , [r6], r5
	vld1.64		d2 , [r6], r5
	vld1.64		d3 , [r6], r5
	vld1.64		d4 , [r6], r5
	vld1.64		d5 , [r6]
	vld1.32		d6 , [r4], r5	@u1 == pRef[0 + 5]@
	vld1.32		d7 , [r4], r5
	vld1.32		d8 , [r4], r5
	vld1.32		d9 , [r4], r5
	vld1.32		d10, [r4], r5
	vld1.32		d11, [r4]

	vmov.u32	q14, #0
	vmov.u16	q15, #0x8000	
	
	@u0 == u0 + err_even-((err_even & 0x8000) << 1)@
	@u1 == u1 + err_odd -((err_odd  & 0x8000) << 1)@
	vadd.u32	q0, q6
	vadd.u32	q3, q10
	vand		q6 , q15
	vand		q10, q15
	vshl.u32	q6, #1
	vshl.u32	q10, #1
	vsub.u32	q0, q6
	vsub.u32	q3, q10
	vorr		q14, q0	@err_overflow  |== u0@
	vorr		q14, q3	@err_overflow  |== u1@

	@u2 == u2 + err_even-((err_even & 0x8000) << 1)@
	@u3 == u3 + err_odd -((err_odd  & 0x8000) << 1)@
	vadd.u32	q1, q7
	vadd.u32	q4, q11
	vand		q7 , q15
	vand		q11, q15
	vshl.u32	q7, #1
	vshl.u32	q11, #1
	vsub.u32	q1, q7
	vsub.u32	q4, q11
	vorr		q14, q1	@err_overflow  |== u2@
	vorr		q14, q4	@err_overflow  |== u3@

    @y0 == (u0) | ((u1) << 8)@
    @y1 == (u2) | ((u3) << 8)@
	vshl.u32	q3, #8
	vshl.u32	q4, #8
	vorr		q0, q3
	vorr		q1, q4
	
	vldr		d6, [r1, #240]	@u0 == pRef[0]@
	vldr		d7, [r1, #280]
	vldr		d8, [r1, #260]	@u1 == pRef[0 + 5]@
	vldr		d9, [r1, #300]

	@u0 == u0 + err_even-((err_even & 0x8000) << 1)@
	@u1 == u1 + err_odd -((err_odd  & 0x8000) << 1)@
	vadd.u32	q2, q8
	vadd.u32	q5, q12
	vand		q8 , q15
	vand		q12, q15
	vshl.u32	q8, #1
	vshl.u32	q12, #1
	vsub.u32	q2, q8
	vsub.u32	q5, q12
	vorr		q14, q2	@err_overflow  |== u0@
	vorr		q14, q5	@err_overflow  |== u1@

	@u2 == u2 + err_even-((err_even & 0x8000) << 1)@
	@u3 == u3 + err_odd -((err_odd  & 0x8000) << 1)@
	vadd.u32	q3, q9
	vadd.u32	q4, q13
	vand		q9, q15
	vand		q13, q15
	vshl.u32	q9, #1
	vshl.u32	q13, #1
	vsub.u32	q3, q9
	vsub.u32	q4, q13
	vorr		q14, q3	@err_overflow  |== u2@
	vorr		q14, q4	@err_overflow  |== u3@

    @y0 == (u0) | ((u1) << 8)@
    @y1 == (u2) | ((u3) << 8)@
	vshl.u32	q5, #8
	vshl.u32	q4, #8
	vorr		q2, q5
	vorr		q3, q4

    @*(U32_WMV *)ppxlcDst == y0@
    @*(U32_WMV *)(ppxlcDst + 4)== y1@
    @ppxlcDst +== iPitch@
    
	vst1.u32	d0, [r0], r3
	vst1.u32	d1, [r0], r3
	vst1.u32	d2, [r0], r3
	vst1.u32	d3, [r0], r3
	vst1.u32	d4, [r0], r3
	vst1.u32	d5, [r0], r3
	vst1.u32	d6, [r0], r3
	vst1.u32	d7, [r0], r3

  @  if(err_overflow & 0xff00ff00) {
  @      ppxlcDst -== 8*iPitch@
  @      g_AddError_EMB_Overflow(ppxlcDst, pRef , pErrorBuf, iPitch)@
  @  }

	vmov		r4, r5, d28
	vmov		r6, r14, d29
	orr			r4, r4, r5
	mov			r5, #0xff00
	orr			r4, r4, r6
	orr			r5, r5, r5, lsl #16
	orr			r4, r4, r14
	
    tst			r4, r5
    ldmeqia		sp!, {r4 - r6, pc}
    
    sub			r0, r0, r3, lsl #3
    bl			_g_AddError_EMB_Overflow_C

    ldmia		sp!, {r4 - r6, pc}
		
    WMV_ENTRY_END
    
	.endif  @    .if WMV_OPT_MOTIONCOMP_ARM == 1
    
	@@.end
    