@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2011					*
@*																			*
@*****************************************************************************

    @AREA    |.text|, CODE, READONLY
    @.section .text
    .text

	.globl  _SIAvgBlock_ASM	
@-----------------------------------------------------------------------------------------------------
@SIAvgBlock_C(int blockSizeY, int blockSizeX,avdUInt8 * src, avdUInt8 *mpr, int Pitch)
@-----------------------------------------------------------------------------------------------------
   .align 4
_SIAvgBlock_ASM:	@PROC
	stmdb        sp!,{r4-r12,lr}
	
	ldr	         r4, [sp,#40]  @Pitch
	mov          r5,  #16	
	cmp          r1,  #16
	beq          AddBlock16xN_Ctx
	cmp          r1,  #8
	beq          AddBlock8xN_Ctx
	cmp          r1,  #4
	beq          AddBlock4xN_Ctx
	cmp          r1,  #2
	beq          AddBlock2xN_Ctx
AddBlock16xN_Ctx:
	mov          r10,  r2
AddBlock16xNLoop_Ctx:
    vld1.64		{q0},  [r10,:64], r4
    vld1.64		{q1},  [r10,:64], r4
    vld1.64		{q2},  [r10,:64], r4
    vld1.64		{q3},  [r10,:64], r4
    vld1.64		{q4},  [r10,:64], r4
    vld1.64		{q5},  [r10,:64], r4
    vld1.64		{q6},  [r10,:64], r4
    vld1.64		{q7},  [r10,:64], r4
    
    vld1.64		{q8},  [r3,:64], r5
    vld1.64		{q9},  [r3,:64], r5
    vld1.64		{q10}, [r3,:64], r5
    vld1.64		{q11}, [r3,:64], r5
    vld1.64		{q12}, [r3,:64], r5
    vld1.64		{q13}, [r3,:64], r5
    vld1.64		{q14}, [r3,:64], r5
    vld1.64		{q15}, [r3,:64], r5
    
    vrhadd.u8   q0,  q0,  q8
    vrhadd.u8   q1,  q1,  q9
    vrhadd.u8   q2,  q2,  q10
    vrhadd.u8   q3,  q3,  q11
    vrhadd.u8   q4,  q4,  q12
    vrhadd.u8   q5,  q5,  q13
    vrhadd.u8   q6,  q6,  q14
    vrhadd.u8   q7,  q7,  q15
    
    vst1.64     {q0},  [r2,:64], r4
    vst1.64     {q1},  [r2,:64], r4
    vst1.64     {q2},  [r2,:64], r4
    vst1.64     {q3},  [r2,:64], r4
    vst1.64     {q4},  [r2,:64], r4
    vst1.64     {q5},  [r2,:64], r4
    vst1.64     {q6},  [r2,:64], r4
    vst1.64     {q7},  [r2,:64], r4
    
    subs        r0, r0, #8 
	bgt         AddBlock16xNLoop_Ctx  
    ldmia       sp!,{r4-r12,pc}
AddBlock8xN_Ctx:
	mov         r10,   r2
AddBlock8xNLoop_Ctx:
    vld1.64		{d0},  [r10,:64], r4
    vld1.64		{d1},  [r10,:64], r4
    vld1.64		{d2},  [r10,:64], r4
    vld1.64		{d3},  [r10,:64], r4
    
    vld1.64		{d8},  [r3,:64], r5
    vld1.64		{d9},  [r3,:64], r5
    vld1.64		{d10}, [r3,:64], r5
    vld1.64		{d11}, [r3,:64], r5
    
    vrhadd.u8   q0,  q0,  q4
    vrhadd.u8   q1,  q1,  q5
   
    vst1.64     {d0},  [r2,:64], r4
    vst1.64     {d1},  [r2,:64], r4
    vst1.64     {d2},  [r2,:64], r4
    vst1.64     {d3},  [r2,:64], r4
    
    subs        r0, r0, #4 
	bgt         AddBlock8xNLoop_Ctx  
    ldmia       sp!,{r4-r12,pc}
AddBlock4xN_Ctx:
	mov			r6,#1
	mov			r7,r6,lsl #8
	orr			r8,r6,r7
	mov			r9,r8,lsl #16
	orr			r12,r8,r9
	mov         r10,r2 
AddBlock4xNLoop_Ctx:	
	ldr 		r6,[r10], r4
	ldr			r7,[r10], r4
	ldr			r8,[r3], r5
	ldr			r9,[r3], r5
		
	uqadd8		r6, r6, r12
	uqadd8		r7, r7, r12
		
	uhadd8		r6, r6, r8	
	uhadd8		r7, r7, r9
	  		 	
	str			r6,[r2], r4
	str			r7,[r2], r4
	subs        r0, r0, #2 
	bgt         AddBlock4xNLoop_Ctx  
    ldmia       sp!,{r4-r12,pc}
AddBlock2xN_Ctx:
	mov			r6,#1
	mov			r7,r6,lsl #8
	orr			r8,r6,r7
	mov			r9,r8,lsl #16
	orr			r12,r8,r9 
AddBlock2xNLoop_Ctx:
	ldrh 		r6,[r2]
	ldrh		r8,[r3], r5		
	uqadd8		r6, r6, r12		
	uhadd8		r6, r6, r8		  		 	
	strh		r6,[r2], r4

	subs        r0, r0, #1 
	bgt         AddBlock2xNLoop_Ctx  
    ldmia       sp!,{r4-r12,pc}
    
    @ENDP