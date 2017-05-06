
    INCLUDE wmvdec_member_arm.inc
    INCLUDE xplatform_arm_asm.h 
    IF UNDER_CE != 0
    INCLUDE kxarm.h
    ENDIF
 
	AREA REPEATPAD, CODE, READONLY

	IF WMV_OPT_REPEATPAD_ARM=1

    EXPORT  ARMV6_g_memcpy  
    EXPORT  ARMV6_g_RepeatRef0Y_LeftRight 
    EXPORT  ARMV6_g_RepeatRef0UV_LeftRight 

;Void g_RepeatRef0Y_32 (
;	PixelC* ppxlcRef0Y,
;	CoordI  iStart, 
;   CoordI  iEnd,
;	Int     iOldLeftOffet,
;	Bool    fTop, 
;   Bool    fBottom,
;   Int     iWidthY,
;   Int     iWidthYPlusExp,
;   Int     iWidthPrevY
;)
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Void_WMV g_RepeatRef0Y_LeftRight ( 
;	const U8_WMV* pLeft, 
;	const U8_WMV* pRight, 
;	U8_WMV* pDst, 
;	I32_WMV iWidthPrevY,	
;	I32_WMV iWidthYPlusExp,  
;	I32_WMV iRowNum )    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	WMV_LEAF_ENTRY ARMV6_g_RepeatRef0Y_LeftRight

;r0: pLeft, 
;r1: pRight, 
;r2: pDst, 
;r3: iWidthPrevY,	
;r4: pDstPlusWidth
;r5: iRowNum
;r6,r7: uipadValueLeft
;r8,r9: uipadValueRight
;r14: temp

StackOffset_RegSaving		EQU		28
StackOffset_iWidthYPlusExp	EQU		StackOffset_RegSaving + 0
StackOffset_iRowNum			EQU		StackOffset_RegSaving + 4

	stmdb     sp!, {r4-r9, r14}

	pld		[r0, r3]
	pld		[r1, r3]
	ldr		r4, [sp, #StackOffset_iWidthYPlusExp]
	ldr		r5, [sp, #StackOffset_iRowNum]
	pld		[r0, r3, lsl #1]
	pld		[r1, r3, lsl #1]
	add		r4, r2, r4
	
Y_LR_Loop

	ldrb	r6, [r0], r3
	ldrb	r8, [r1], r3
	pld		[r0, r3, lsl #1]
	pld		[r1, r3, lsl #1]
	
	orr		r6, r6, r6, lsl #8
	orr		r8, r8, r8, lsl #8
	orr		r6, r6, r6, lsl #16
	orr		r8, r8, r8, lsl #16
		
	mov		r7, r6
	mov		r9, r8
	strd	r6, [r2, #8]
	strd	r6, [r2, #16]
	strd	r6, [r2, #24]
	strd	r6, [r2], r3
	strd	r8, [r4, #8]
	strd	r8, [r4, #16]
	strd	r8, [r4, #24]
	strd	r8, [r4], r3
	
	subs	r5, r5, #1
	bne		Y_LR_Loop

	ldmia     sp!, {r4-r9,pc}    
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Void_WMV g_RepeatRef0UV_LeftRight ( 
;	const U8_WMV* pLeftU, 
;	const U8_WMV* pRightU, 
;	const U8_WMV* pLeftV, 
;	const U8_WMV* pRightV, 
;	U8_WMV* pDstU, 
;	U8_WMV* pDstV, 
;	I32_WMV iWidthPrevUV,	
;	I32_WMV iWidthUVPlusExp,  
;	I32_WMV iRowNum)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	WMV_LEAF_ENTRY ARMV6_g_RepeatRef0UV_LeftRight

;r0: pLeftU
;r1: pRightU
;r2: pLeftV
;r3: pRightV	
;r4: pDstU
;r5: pDstV
;r6,r7: uipadValueLeft
;r8,r9: uipadValueRight
;r10: pDstPlusWidth
;r11: iWidthPrevUV
;r12: iWidthUVPlusExp
;r14: iRowNum

StackOffset_RegSaving_UV	EQU		36
StackOffset_pDstU			EQU		StackOffset_RegSaving_UV + 0
StackOffset_pDstV			EQU		StackOffset_RegSaving_UV + 4
StackOffset_iWidthPrevUV	EQU		StackOffset_RegSaving_UV + 8
StackOffset_iWidthUVPlusExp	EQU		StackOffset_RegSaving_UV + 12
StackOffset_iRowNum_UV 		EQU		StackOffset_RegSaving_UV + 16
StackOffset_iRowNum_Back	EQU		-4

	stmdb     sp!, {r4-r11, r14}
	
	ldr		r12, [sp, #StackOffset_iWidthUVPlusExp]
	ldr		r11, [sp, #StackOffset_iWidthPrevUV]
	ldr		r14, [sp, #StackOffset_iRowNum_UV]
	ldr		r4 , [sp, #StackOffset_pDstU]
	ldr		r5 , [sp, #StackOffset_pDstV]
	
	pld		[r0, r11]
	pld		[r1, r11]
	pld		[r0, r11, lsl #1]
	pld		[r1, r11, lsl #1]
	
	add		r10, r4, r12   ; pDstUPlusWidth
	str		r14, [sp, #StackOffset_iRowNum_Back]
U_LR_Loop

	ldrb	r6, [r0], r11
	ldrb	r8, [r1], r11
	pld		[r0, r11, lsl #1]
	pld		[r1, r11, lsl #1]
	
	orr		r6, r6, r6, lsl #8
	orr		r8, r8, r8, lsl #8
	orr		r6, r6, r6, lsl #16
	orr		r8, r8, r8, lsl #16
	
	mov		r7, r6
	mov		r9, r8
	strd	r6, [r4, #8]
	strd	r6, [r4], r11
	strd	r8, [r10, #8]
	strd	r8, [r10], r11
	
	subs	r14, r14, #1
	bne		U_LR_Loop

	pld		[r2, r11]
	pld		[r3, r11]
	pld		[r2, r11, lsl #1]
	pld		[r3, r11, lsl #1]
	ldr		r14, [sp, #StackOffset_iRowNum_Back]
	add		r10, r5, r12   ; pDstVPlusWidth
V_LR_Loop

	ldrb	r6, [r2], r11
	ldrb	r8, [r3], r11
	pld		[r2, r11, lsl #1]
	pld		[r3, r11, lsl #1]
	
	orr		r6, r6, r6, lsl #8
	orr		r8, r8, r8, lsl #8
	orr		r6, r6, r6, lsl #16
	orr		r8, r8, r8, lsl #16
	
	mov		r7, r6
	mov		r9, r8
	strd	r6, [r5, #8]
	strd	r6, [r5], r11
	strd	r8, [r10, #8]
	strd	r8, [r10], r11
	
	subs	r14, r14, #1
	bne		V_LR_Loop
	
	ldmia     sp!, {r4-r11,pc}    


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Void_WMV g_memcpy(U8_WMV* ppxliLeftTop, U8_WMV* ppxliSrc, I32_WMV iWidthPrevY, I32_WMV iTrueWidth, I32_WMV iVertPad)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	WMV_LEAF_ENTRY ARMV6_g_memcpy
	
iVertPad_offset		EQU		24

     pld	[r1, #16]
     stmdb  sp!, {r4-r8, r14}

     ldr	r8, [sp, #iVertPad_offset]
     sub	r2, r2, #16
     mov	r14, r0

memcpy_load
     ldrd	r4, [r1], #8
     ldrd	r6, [r1], #8
     pld	[r1, #16]
     mov	r12, r8
    
memcpy_store
     strd	r4, [r0], #8
     strd	r6, [r0], #8
     subs	r12, r12, #1
     add	r0, r0, r2      
     bne	memcpy_store
     
     add	r14, r14, #16
     sub	r3, r3, #16
     mov	r0, r14     
     cmp	r3, #16
     bge	memcpy_load         
                 
     cmp	r3, #0
     ldmeqia sp!, {r4-r8,pc}    
                       
     ldrd	r4, [r1]
     mov	r12, r8
     add	r2, r2, #16
     
memcpy_store_last
     subs	r12, r12, #1
     strd	r4, [r0], r2
     bne	memcpy_store_last
                     
     ldmia sp!, {r4-r8,pc}    
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	ENDIF ;WMV_OPT_REPEATPAD_ARM


	END	
	
	