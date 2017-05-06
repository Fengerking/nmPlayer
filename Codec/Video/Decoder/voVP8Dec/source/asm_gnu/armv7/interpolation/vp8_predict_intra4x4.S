@
@  Copyright (c) 2011 VisualOn. All Rights Reserved.
@
@

		@AREA |.text|, CODE, READONLY
    @	EXPORT  vo_B_LD_PRED_neon
    @	EXPORT  vo_B_DC_PRED_neon
    @	EXPORT  vo_B_TM_PRED_neon
    @	EXPORT  vo_B_VE_PRED_neon
    @	EXPORT  vo_B_HE_PRED_neon
    @	EXPORT  vo_B_RD_PRED_neon
    @	EXPORT  vo_Extend_Broder_y_neon
    @	EXPORT  vo_Extend_Broder_uv_neon
    #include "../../../../Inc/voVP8DecID.h"
    .section .text 
    .global  vo_B_LD_PRED_neon
    .global  vo_B_DC_PRED_neon
    .global  vo_B_TM_PRED_neon
    .global  vo_B_VE_PRED_neon
    .global  vo_B_HE_PRED_neon
    .global  vo_B_RD_PRED_neon
		.global  vo_Extend_Broder_y_neon
    .global  vo_Extend_Broder_uv_neon
@vo_B_HE_PRED_neon(BYTE*left,BYTE top_left,BYTE*predictor)@
@vo_B_VE_PRED_neon(BYTE*Above,BYTE top_left,BYTE*predictor)@
@vo_B_TM_PRED_neon(BYTE*Above,BYTE*Left,BYTE top_left,BYTE*predictor)@
@vo_B_DC_PRED_neon(BYTE*Above,BYTE*left,BYTE* predictor)@ 
@vo_B_LD_PRED_neon(BYTE*Above,BYTE* predictor)@

@vo_B_RD_PRED_neon(BYTE*pp,BYTE* predictor)@
vo_Extend_Broder_y_neon: @PROC
	stmdb       sp!, {r4 - r9,lr}
	sub			r4,r0,#32
	add         r5,r1,#1
LOOP:
	ldr         r6,[r0]
	ldr         r7,[r1]
	
	vdup.u8       q0,r6
	vdup.u8       q1,r6
	
	vdup.u8       q2,r7
	vdup.u8       q3,r7
	
	add           r0,r0,r2
	add           r1,r1,r2
	
	vst1.u8     {q0,q1},[r4]
	vst1.u8     {q2,q3},[r5]
	subs          r3,r3,#1
	
	add           r4,r4,r2
	add           r5,r5,r2	
	
	bge  LOOP	
	ldmia       sp!, {r4 - r9,pc}
	@ENDP
vo_Extend_Broder_uv_neon:@ PROC
	stmdb       sp!, {r4 - r9,lr}
	sub			r4,r0,#16
	add         r5,r1,#1
LOOP2:
	ldr         r6,[r0]
	ldr         r7,[r1]
	
	vdup.u8       q0,r6
	
	vdup.u8       q2,r7
	
	add           r0,r0,r2
	add           r1,r1,r2
	
	vst1.u8     {q0},[r4]
	vst1.u8     {q2},[r5]
	subs          r3,r3,#1
	
	add           r4,r4,r2
	add           r5,r5,r2	
	
	bge  LOOP2	
	ldmia       sp!, {r4 - r9,pc}
	@ENDP
	
vo_B_RD_PRED_neon:@ PROC
	stmdb       sp!, {lr}
	vld1.u8     {q0}, [r0]      @q0: p0  p1	p2	p3 	p4 	p5 	p6 	p7 	p8
							    @d0: p0  p1	p2	p3 	p4 	p5 	p6 	p7
	mov            r2,#2
	mov            r3,#16						    
	vdup.u16       q5,r2        @q5: 2	2	2	2	2	2	2	2	---->
	
	vmovl.u8       q1,d0     @q1: p0  p1	p2	p3 	p4 	p5 	p6 	p7 ---->	
	vmovl.u8       q2,d1     @q2: p8  x	x	x	x	x	x	x	
	
	vext.u16       q3,q1,q1,#1  @q3: p1	p2	p3 	p4 	p5 	p6 	p7	x 
	vadd.u16       q3,q3,q3     @q3: 2p1 2p2 2p3 2p4 2p5 2p6 2p7	x	---->
	
	vext.u16	   q4,q1,q2,#2  @q4: p2	 p3  p4  p5  p6  p7	 p8	x  ----->
	
	vadd.u16	   q6,q4,q3
	vadd.u16	   q6,q6,q5
	vadd.u16       q6,q6,q1
	
	vshrn.u16	   d0,q6,#2
	
	vshr.u64       d2,d0,#8          @d2	
	vshr.u64       d4,d0,#16          @d4	
	vshr.u64       d6,d0,#24          @d6
	
	vst1.u32       d6[0],[r1],r3
	vst1.u32       d4[0],[r1],r3
	vst1.u32       d2[0],[r1],r3	
	vst1.u32       d0[0],[r1],r3     @d0	
	ldmia       sp!, {pc}
	@ENDP

vo_B_HE_PRED_neon:@ PROC
	stmdb       sp!, {lr}
	
	@add     r1,r1,#2
	mov     r3,#16
	
	vld1.u32         {d0}, [r0]   @ d0[0]: L0  L1  L2  L3 X  X  X  X
	
	vdup.u16        d6, r1        @d6 : tp tp tp tp 
	
	mov        r1, #2
	vdup.u16        d10, r1        @d6 : 2 2 2 2
	
	
	vmovl.u8         q0,d0     @d0 : L0  L1  L2  L3
	
	vdup.u16          d1,d0[3]
	vext.u16         d2,d0,d1,#1  @d2 : L1  L2  L3  L3  !!!
	
	vadd.u16         d4,d0,d0     @d4 : 2L0  2L1  2L2  2L3	
	vext.u16         d6,d6,d0,#3  @d6 : tp   L0  L1  L2
	vadd.u16         d6,d6,d10
	
	vadd.u16         d8,d6,d4
	vadd.u16         d8,d8,d2
	
	vshrn.u16        d0, q4, #2
	
	vdup.u8        d2,d0[0]
	vdup.u8        d4,d0[1]
	vdup.u8        d6,d0[2]
	vdup.u8        d8,d0[3]
	
	vst1.u32        {d2[0]},[r2],r3  
	vst1.u32        {d4[0]},[r2],r3
	vst1.u32        {d6[0]},[r2],r3
	vst1.u32        {d8[0]},[r2],r3	
	
	ldmia       sp!, {pc}
	@ENDP
vo_B_VE_PRED_neon:@ PROC
	stmdb       sp!, {lr}
	
	@add     r1,r1,#2
	mov     r3,#16
	
	vld1.u32         {d0}, [r0]   @ d0[0]: A0  A1  A2  A3 A4 A5 A6 A7
	
	vdup.u16        d6, r1        @d6 : tp tp tp tp 
	
	mov        r1, #2
	vdup.u16        d10, r1        @d6 : 2 2 2 2
	
	
	vmovl.u8         q0,d0     @d0 : A0  A1  A2  A3
	
	vext.u16         d2,d0,d1,#1  @d2 : A1  A2  A3 A4
	vadd.u16         d4,d0,d0     @d4 : 2A0  2A1  2A2  2A3	
	vext.u16         d6,d6,d0,#3  @d6 : tp   A0  A1  A2
	vadd.u16         d6,d6,d10
	
	vadd.u16         d8,d6,d4
	vadd.u16         d8,d8,d2
	
	vshrn.u16        d0, q4, #2
	
	vst1.u32        {d0[0]},[r2],r3  
	vst1.u32        {d0[0]},[r2],r3
	vst1.u32        {d0[0]},[r2],r3
	vst1.u32        {d0[0]},[r2],r3
	
	
	ldmia       sp!, {pc}
	@ENDP
vo_B_TM_PRED_neon:@ PROC
	stmdb       sp!, {lr}
	vdup.u16        d10,r2  
	
	mov    r2,#16  
	vld1.u32         {d0[0]}, [r0]   @ d0[0]: A0  A1  A2  A3	
	vld1.u32         {d0[1]}, [r1]   @ d0[1]: L0  L1  L2  L3
	
	vmovl.u8           q0,d0 
	
	vsub.s16	     d0,d0,d10         
	
	vdup.s16           d2,d1[0]       @ L0 L0 L0 L0
	vdup.s16           d4,d1[1]       @ L1 L1 L1 L1
	vdup.s16           d6,d1[2]       @ L2 L2 L2 L2
	vdup.s16           d8,d1[3]       @ L3 L3 L3 L3
	
	vadd.s16          d2,d2,d0
	vadd.s16          d4,d4,d0
	vadd.s16          d6,d6,d0
	vadd.s16          d8,d8,d0	
	
	vqmovun.s16     d2, q1
	vqmovun.s16     d4, q2
	vqmovun.s16     d6, q3
	vqmovun.s16     d8, q4
	
	vst1.u32        {d2[0]},[r3],r2  
	vst1.u32        {d4[0]},[r3],r2
	vst1.u32        {d6[0]},[r3],r2
	vst1.u32        {d8[0]},[r3],r2		
	
	ldmia       sp!, {pc}
	@ENDP 
vo_B_DC_PRED_neon:@ PROC
	stmdb       sp!, {lr}	
	adr             r3, DATA2
	vld1.u32         {d8}, [r3]	
	mov             r3,#16	
	vld1.u32         {d0[0]}, [r0]   @ d0: A0  A1  A2  A3	
	vld1.u32         {d0[1]}, [r1]   @ d0: L0  L1  L2  L3	
	vpaddl.u8         d0,d0	
	vpaddl.u16        d0,d0	
	vpaddl.u32        d0,d0		
	vadd.u32          d0,d0,d8
	vshr.u32          d0,d0,#3	
	vdup.u8           d0,d0[0]		
	vst1.u32        {d0[0]},[r2],r3  
	vst1.u32        {d0[0]},[r2],r3
	vst1.u32        {d0[0]},[r2],r3
	vst1.u32        {d0[0]},[r2],r3	
	ldmia       sp!, {pc}
	@ENDP 
vo_B_LD_PRED_neon :@PROC
	
	stmdb       sp!, {lr}
	vld1.u8         {d0}, [r0]      @d0:  p0 		p1		p2		p3 		p4 		p5 		p6 		p7
	
	adr             r2, DATA
	vld1.u8         {d11}, [r2] 
	
	mov  r3,#16
	
	vshr.u64	   d1,d0,#8	        @d1:  p1 		p2		p3		p4 		p5 		p6 		p7 		xx
	
	@vshr.u64       d10,d1,#8       @d10:  p2		p3 		p4 		p5 		p6 		p7	    xx      xx
	
	
	vshr.u64	   d12,d0,#56
	vext.u8        d10,d0,d12,#2
	
	
	vmovl.u8        q1,d0        @q1:  p0 		p1		p2		p3 		p4 		p5 		p6 		p7				
	vshll.u8        q2,d1,#1        @q2:  2p1 		2p2		2p3		2p4 		2p5 		2p6 		2p7 		xx		
	
	vaddl.u8        q3,d10,d11
	
	vadd.u16		q4,q1,q2        @ p0+2p1
	vadd.u16        q4,q4,q3        @ p0+2p1+p2+2
	
	vshrn.u16       d20,q4,#2	
	
	vst1.u32        {d20[0]},[r1],r3    
	vshr.u64	    d20,d20,#8		
	vst1.u32        {d20[0]},[r1],r3 
	vshr.u64	    d20,d20,#8		
	vst1.u32        {d20[0]},[r1],r3 
	vshr.u64	    d20,d20,#8		
	vst1.u32        {d20[0]},[r1],r3 
	
	ldmia       sp!, {pc}
	
	@ENDP
		.ALIGN 4
DATA:
    .word     0x02020202
    .word		0x02020202
DATA2:
    .word     0x00000004
    .word		0x00000000
		.END