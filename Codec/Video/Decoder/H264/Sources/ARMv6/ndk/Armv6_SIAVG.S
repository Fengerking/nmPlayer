@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2011					*
@*																			*
@*****************************************************************************

    @AREA    |.text|, CODE, READONLY
  .text

	.global	SIAvgBlock_ASM
	
@-----------------------------------------------------------------------------------------------------
@SIAvgBlock_C(int blockSizeY, int blockSizeX,avdUInt8 * src, avdUInt8 *mpr, int Pitch)
@-----------------------------------------------------------------------------------------------------
   .ALIGN 4
SIAvgBlock_ASM:	@PROC
	stmdb       sp!,{r4-r12,lr}
	
	ldr	        r4, [sp,#40]  @Pitch
	mov         r5,  #16	
	cmp         r1,  #16
	beq         AddBlock16xN_Armv6
	cmp         r1,  #8
	beq         AddBlock8xN_Armv6
	cmp         r1,  #4
	beq         AddBlock4xN_Armv6
	cmp         r1,  #2
	beq         AddBlock2xN_Armv6
AddBlock16xN_Armv6:
	mov			r6,#1
	mov			r7,r6,lsl #8
	orr			r8,r6,r7
	mov			r9,r8,lsl #16
	orr			r12,r8,r9 
AddBlock16xNLoop_Armv6:
    ldrd		r6, [r2, #8]      
	ldrd		r10,[r3, #8]
	
	uqadd8		r6, r6, r12
	uqadd8		r7, r7, r12
	uhadd8		r6, r6, r10  
	uhadd8		r7, r7, r11
	
	strd		r6,[r2, #8]
	
	ldrd		r6, [r2]
	ldrd		r10,[r3], r5
	
	uqadd8		r6, r6, r12
	uqadd8		r7, r7, r12
	uhadd8		r6, r6, r10  
	uhadd8		r7, r7, r11

	strd		r6,[r2], r4
    
    subs        r0, r0, #1 
	bgt         AddBlock16xNLoop_Armv6  
    ldmia       sp!,{r4-r12,pc}
AddBlock8xN_Armv6:
	mov			r6,#1
	mov			r7,r6,lsl #8
	orr			r8,r6,r7
	mov			r9,r8,lsl #16
	orr			r12,r8,r9 
AddBlock8xNLoop_Armv6:
	ldrd	    r6, [r2]
	ldrd	    r8, [r3], r5
		
	uqadd8	    r6, r6, r12
	uqadd8      r7, r7, r12		
	uhadd8	    r6, r6, r8
	uhadd8	    r7, r7, r9
	  		 	
	strd	    r6,[r2], r4
		
    subs        r0, r0, #1 
	bgt         AddBlock8xNLoop_Armv6 
    ldmia       sp!,{r4-r12,pc}
AddBlock4xN_Armv6:
	mov			r6,#1
	mov			r7,r6,lsl #8
	orr			r8,r6,r7
	mov			r9,r8,lsl #16
	orr			r12,r8,r9 
	mov         r10,r2
AddBlock4xNLoop_Armv6:	
	ldr 		r6,[r10],r4
	ldr			r7,[r10],r4
	ldr			r8,[r3], r5
	ldr			r9,[r3], r5
		
	uqadd8		r6, r6, r12
	uqadd8		r7, r7, r12
		
	uhadd8		r6, r6, r8	
	uhadd8		r7, r7, r9
	  		 	
	str			r6,[r2], r4
	str			r7,[r2], r4
	subs        r0, r0, #2 
	bgt         AddBlock4xNLoop_Armv6 
    ldmia       sp!,{r4-r12,pc}
AddBlock2xN_Armv6:
	mov			r6,#1
	mov			r7,r6,lsl #8
	orr			r8,r6,r7
	mov			r9,r8,lsl #16
	orr			r12,r8,r9 
AddBlock2xNLoop_Armv6:
	ldrh 		r6,[r2]
	ldrh		r8,[r3], r5		
	uqadd8		r6, r6, r12		
	uhadd8		r6, r6, r8		  		 	
	strh		r6,[r2], r4

	subs        r0, r0, #1 
	bgt         AddBlock2xNLoop_Armv6  
    ldmia       sp!,{r4-r12,pc}
    
    @ENDP