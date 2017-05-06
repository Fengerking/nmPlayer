
@	/************************************************************************
@	*																		*
@	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
@	*																		*
@	************************************************************************/


	.section .text
	.global  sad16_wmmx
	.type    sad16_wmmx, function

sad16_wmmx:
.ALIGN 4
	stmdb	sp!, {r5,r6, lr}		@ save regs used
    WLDRD	wR0, [r0]			@ load eight source pixels
    AND		r5,  r1,  #7		@ r7 = 3LSBs of *pRef
	WLDRD	wR1, [r0, #8]		@ load next eight source pixels	
	BIC		r1,  r1,  #7		@ r0 64-bit aligned  
	WLDRD	wR2, [r1]			@ load eight reference pixels
	TMCR	wCGR0, r5			@ transfer alignment to wCGR0
	WLDRD	wR3, [r1, #8]		@ load eight reference pixels
    WZERO	wR15				@ zero accumulator
	WLDRD	wR4, [r1, #16]		@ load eight reference pixels

	ADD      r1,   r1,   r2		@ increment *pRef by RefStride
	ADD      r0,   r0,   r2		@ increment *pSrc by SrcStride
	MOV		r6, #16				@ setup loop count
	SAD16x16_loop:
		SUBS     r6,   r6,   #1			@ decrement loop counter 
		WALIGNR0 wR5, wR2, wR3			@ align odd reference row
		WLDRDNE    wR2,  [r1]			@ load next row ref. pixels
		WSADB    wR15, wR0, wR5			@ calculate SAD for odd rows
		WLDRDNE    wR0,  [r0]			@ load next row source pixels
		WALIGNR0 wR5, wR3,  wR4			@ align odd reference row
		WLDRDNE    wR3,  [r1, #8]		@ load next ref. pixels
		WSADB    wR15, wR1, wR5			@ calculate SAD for odd rows
		WLDRDNE    wR1,  [r0, #8]		@ load next row source pixels
		ADD      r0,   r0,   r2			@ increment *pSrc by SrcStride
		WLDRDNE    wR4,  [r1, #16]		@ load next ref. pixels
		ADD      r1,   r1,   r2			@ increment *pRef by RefStride

		BNE SAD16x16_loop   
    TEXTRMSH  r0, wR15, #0   @ return result
    ldmia		sp!, {r5,r6, pc}		@ restore and return   
	

	.section .text
	.global  dev16_wmmx
	.type    dev16_wmmx, function

dev16_wmmx:
.ALIGN 4
	stmdb	sp!, {r4-r6, lr}		@ save regs used
	wldrd	wr0, [r0]
	mov		r5, #8
	wldrd	wr1, [r0, #8]
	tmcr	wcgr0, r5			@ 
	waccb	wr15, wr0
	add		r2, r0, r1
	wldrd	wr2, [r2]
	waccb	wr14, wr1
	wldrd	wr3, [r2, #8]
	waddw	wr15, wr15, wr14
	add		r2, r2, r1
	wldrd	wr0, [r2]
	waccb	wr14, wr2
	wldrd	wr1, [r2, #8]
	waddw	wr15, wr15, wr14
	add		r2, r2, r1
	wldrd	wr2, [r2]
	waccb	wr14, wr3
	wldrd	wr3, [r2, #8]
	waddw	wr15, wr15, wr14	
	add		r2, r2, r1
	waccb	wr14, wr0
	wldrd	wr0, [r2]
	waddw	wr15, wr15, wr14
	waccb	wr14, wr1
	wldrd	wr1, [r2, #8]
	waddw	wr15, wr15, wr14
	add		r2, r2, r1
	waccb	wr14, wr2
	wldrd	wr2, [r2]
	waddw	wr15, wr15, wr14
	waccb	wr14, wr3
	wldrd	wr3, [r2, #8]
	waddw	wr15, wr15, wr14
	add		r2, r2, r1
	waccb	wr14, wr0
	wldrd	wr0, [r2]
	waddw	wr15, wr15, wr14
	waccb	wr14, wr1
	wldrd	wr1, [r2, #8]
	waddw	wr15, wr15, wr14
	add		r2, r2, r1
	waccb	wr14, wr2
	wldrd	wr2, [r2]
	waddw	wr15, wr15, wr14
	waccb	wr14, wr3
	wldrd	wr3, [r2, #8]
	waddw	wr15, wr15, wr14
	add		r2, r2, r1
	waccb	wr14, wr0
	wldrd	wr0, [r2]
	waddw	wr15, wr15, wr14
	waccb	wr14, wr1
	wldrd	wr1, [r2, #8]
	waddw	wr15, wr15, wr14
	add		r2, r2, r1
	waccb	wr14, wr2
	wldrd	wr2, [r2]
	waddw	wr15, wr15, wr14
	waccb	wr14, wr3
	wldrd	wr3, [r2, #8]
	waddw	wr15, wr15, wr14
	add		r2, r2, r1
	waccb	wr14, wr0
	wldrd	wr0, [r2]
	waddw	wr15, wr15, wr14
	waccb	wr14, wr1
	wldrd	wr1, [r2, #8]
	waddw	wr15, wr15, wr14
	add		r2, r2, r1
	waccb	wr14, wr2
	wldrd	wr2, [r2]
	waddw	wr15, wr15, wr14
	waccb	wr14, wr3
	wldrd	wr3, [r2, #8]
	waddw	wr15, wr15, wr14
	add		r2, r2, r1
	waccb	wr14, wr0
	wldrd	wr0, [r2]
	waddw	wr15, wr15, wr14
	waccb	wr14, wr1
	wldrd	wr1, [r2, #8]
	waddw	wr15, wr15, wr14
	add		r2, r2, r1
	waccb	wr14, wr2
	wldrd	wr2, [r2]
	waddw	wr15, wr15, wr14
	waccb	wr14, wr3
	wldrd	wr3, [r2, #8]
	waddw	wr15, wr15, wr14
	add		r2, r2, r1
	waccb	wr14, wr0
	wldrd	wr0, [r2]
	waddw	wr15, wr15, wr14
	waccb	wr14, wr1
	wldrd	wr1, [r2, #8]
	waddw	wr15, wr15, wr14
	add		r2, r2, r1
	waccb	wr14, wr2
	wldrd	wr2, [r2]
	waddw	wr15, wr15, wr14
	waccb	wr14, wr3
	wldrd	wr3, [r2, #8]
	waddw	wr15, wr15, wr14
	waccb	wr14, wr0
	waddw	wr15, wr15, wr14
	waccb	wr14, wr1
	waddw	wr15, wr15, wr14
	waccb	wr14, wr2
	waddw	wr15, wr15, wr14
	wldrd	wr0, [r0]
	waccb	wr14, wr3
	wldrd	wr1, [r0, #8]
	waddw	wr15, wr15, wr14
	add		r0, r0, r1
	wldrd	wr2, [r0]
	wsrawg	wr15, wr15, wcgr0
	wldrd	wr3, [r0, #8]
	tmrrc	r4, r6, wr15
	tbcstb	wr15, r4
	wzero	wr14

	add		r0, r0, r1
	wsadb	wr14, wr0, wr15
	wldrd	wr0, [r0]
	wsadb	wr14, wr1, wr15
	wldrd	wr1, [r0, #8]
	add		r0, r0, r1
	wsadb	wr14, wr2, wr15
	wldrd	wr2, [r0]
	wsadb	wr14, wr3, wr15
	wldrd	wr3, [r0, #8]
	add		r0, r0, r1
	wsadb	wr14, wr0, wr15
	wldrd	wr0, [r0]
	wsadb	wr14, wr1, wr15
	wldrd	wr1, [r0, #8]
	add		r0, r0, r1
	wsadb	wr14, wr2, wr15
	wldrd	wr2, [r0]
	wsadb	wr14, wr3, wr15
	wldrd	wr3, [r0, #8]
	add		r0, r0, r1
	wsadb	wr14, wr0, wr15
	wldrd	wr0, [r0]
	wsadb	wr14, wr1, wr15
	wldrd	wr1, [r0, #8]
	add		r0, r0, r1
	wsadb	wr14, wr2, wr15
	wldrd	wr2, [r0]
	wsadb	wr14, wr3, wr15
	wldrd	wr3, [r0, #8]
	add		r0, r0, r1
	wsadb	wr14, wr0, wr15
	wldrd	wr0, [r0]
	wsadb	wr14, wr1, wr15
	wldrd	wr1, [r0, #8]
	add		r0, r0, r1
	wsadb	wr14, wr2, wr15
	wldrd	wr2, [r0]
	wsadb	wr14, wr3, wr15
	wldrd	wr3, [r0, #8]
	add		r0, r0, r1
	wsadb	wr14, wr0, wr15
	wldrd	wr0, [r0]
	wsadb	wr14, wr1, wr15
	wldrd	wr1, [r0, #8]
	add		r0, r0, r1
	wsadb	wr14, wr2, wr15
	wldrd	wr2, [r0]
	wsadb	wr14, wr3, wr15
	wldrd	wr3, [r0, #8]
	add		r0, r0, r1
	wsadb	wr14, wr0, wr15
	wldrd	wr0, [r0]
	wsadb	wr14, wr1, wr15
	wldrd	wr1, [r0, #8]
	add		r0, r0, r1
	wsadb	wr14, wr2, wr15
	wldrd	wr2, [r0]
	wsadb	wr14, wr3, wr15
	wldrd	wr3, [r0, #8]
	add		r0, r0, r1
	wsadb	wr14, wr0, wr15
	wldrd	wr0, [r0]
	wsadb	wr14, wr1, wr15
	wldrd	wr1, [r0, #8]
	add		r0, r0, r1
	wsadb	wr14, wr2, wr15
	wldrd	wr2, [r0]
	wsadb	wr14, wr3, wr15
	wldrd	wr3, [r0, #8]
	wsadb	wr14, wr0, wr15
	wsadb	wr14, wr1, wr15
	wsadb	wr14, wr2, wr15
	wsadb	wr14, wr3, wr15
  
    TEXTRMSH  r0, wr14, #0   @ return result
    ldmia		sp!, {r4-r6, pc}		@ restore and return  


	.section .text
	.global  sad8_wmmx
	.type    sad8_wmmx, function


sad8_wmmx:
.ALIGN 4
	stmdb	sp!, {r7, lr}		@ save regs used
    WLDRD	wR0, [r0]			@ load eight source pixels	
    AND		r7,  r1,  #7		@ r7 = 3LSBs of *pRef
    BIC		r1,  r1,  #7		@ r0 64-bit aligned  
	WLDRD	wR1, [r1]			@ load eight reference pixels
    TMCR	wCGR0, r7			@ transfer alignment to wCGR0
	WLDRD	wR2, [r1, #8]		@ load eight reference pixels
    ADD     r0, r0, r2			@ increment *pSrc by SrcStride
    WLDRD	wR3, [r0]			@ load eight source pixels	
	ADD		r1, r1, r3
	WLDRD	wR4, [r1]			@ load eight reference pixels
    WZERO	wR15				@ zero accumulator
	WLDRD	wR5, [r1, #8]		@ load eight reference pixels

    WALIGNR0 wR14, wR1, wR2		@ align odd reference row
	ADD		r1, r1, r3
	WLDRD	wR1, [r1]			@ load eight reference pixels
	WSADB    wR15, wR0, wR14	@ calculate SAD for odd rows
	WLDRD	wR2, [r1, #8]		@ load eight reference pixels
    ADD     r0, r0, r2			@ increment *pSrc by SrcStride
    WLDRD	wR0, [r0]			@ load eight source pixels
		
    WALIGNR0 wR14, wR4, wR5		@ align odd reference row
	ADD		r1, r1, r3
	WLDRD	wR4, [r1]			@ load eight reference pixels
	WSADB    wR15, wR3, wR14	@ calculate SAD for odd rows
	WLDRD	wR5, [r1, #8]		@ load eight reference pixels

    WALIGNR0 wR14, wR1, wR2		@ align odd reference row
    ADD     r0, r0, r2			@ increment *pSrc by SrcStride
	WSADB    wR15, wR0, wR14	@ calculate SAD for odd rows
    WLDRD	wR3, [r0]			@ load eight source pixels	
    ADD     r0, r0, r2			@ increment *pSrc by SrcStride
    WLDRD	wR0, [r0]			@ load eight source pixels
	ADD		r1, r1, r3	
	WLDRD	wR1, [r1]			@ load eight reference pixels		
    WALIGNR0 wR14, wR4, wR5		@ align odd reference row
	WLDRD	wR2, [r1, #8]		@ load eight reference pixels
	ADD		r1, r1, r3
	WLDRD	wR4, [r1]			@ load eight reference pixels
	WSADB    wR15, wR3, wR14	@ calculate SAD for odd rows
	WLDRD	wR5, [r1, #8]		@ load eight reference pixels
    ADD     r0, r0, r2			@ increment *pSrc by SrcStride
    WLDRD	wR3, [r0]			@ load eight source pixels	

    WALIGNR0 wR14, wR1, wR2		@ align odd reference row
	ADD		r1, r1, r3
	WLDRD	wR1, [r1]			@ load eight reference pixels
	WSADB    wR15, wR0, wR14	@ calculate SAD for odd rows
	WLDRD	wR2, [r1, #8]		@ load eight reference pixels
    ADD     r0, r0, r2			@ increment *pSrc by SrcStride	
    WLDRD	wR0, [r0]			@ load eight source pixels	
		
    WALIGNR0 wR14, wR4, wR5		@ align odd reference row
	ADD		r1, r1, r3
	WLDRD	wR4, [r1]			@ load eight reference pixels
	WSADB    wR15, wR3, wR14	@ calculate SAD for odd rows
	WLDRD	wR5, [r1, #8]		@ load eight reference pixels
    ADD     r0, r0, r2			@ increment *pSrc by SrcStride
    WLDRD	wR3, [r0]			@ load eight source pixels	

    WALIGNR0 wR14, wR1, wR2		@ align odd reference row
	WSADB    wR15, wR0, wR14	@ calculate SAD for odd rows
		
    WALIGNR0 wR14, wR4, wR5		@ align odd reference row
	WSADB    wR15, wR3, wR14	@ calculate SAD for odd rows
   
    TEXTRMSH  r0, wR15, #0   @ return result
    ldmia		sp!, {r7, pc}		@ restore and return   


	.section .text
	.global  sad16v_wmmx
	.type    sad16v_wmmx, function

sad16v_wmmx:
.ALIGN 4
	stmdb		sp!, {r4, r6, r7, lr}		@ save regs used
    WLDRD	wR0, [r0], #8		@ load low eight source pixels
    AND		r7,  r1,  #7		@ r7 = 3LSBs of *pRef
	WLDRD	wR1, [r0], #8	 	@ load high eight source pixels	
    BIC		r1,  r1,  #7		@ r0 64-bit aligned  
    WLDRD	wR2, [r1]			@ load low eight reference pixels
    TMCR	wCGR0, r7			@ transfer alignment to wCGR0
	WLDRD	wR3, [r1, #8]		@ load middle eight reference pixels
	WZERO	wR14				@ zero low accumulator
	WLDRD	wR4, [r1, #16]		@ load high eight reference pixels
	ADD		r1, r1, r3
    WLDRD	wR5, [r0], #8		@ load low eight source pixels
    WZERO	wR15				@ zero high accumulator
    WLDRD	wR6, [r0], #8		@ load low eight source pixels
	MOV		r6, #4				@ setup loop count
    WLDRD	wR7, [r1]			@ load low eight ref pixels
	WZERO	wR13
    WLDRD	wR8, [r1, #8]		@ load low eight ref pixels
	WZERO	wR12
    WLDRD	wR9, [r1, #16]		@ load low eight source pixels
	ldr		r4, [sp, #16]

	@!< pC[NORTHWEST_DIVISION] and  pC[NORTHEAST_DIVISION]
Lhight1:
	ADD			r1, r1, r3				@ ref add refstride
	WALIGNR0	wR10, wR2, wR3			@ align low ref 8 pixel
    WLDRD		wR2, [r1]				@ load low eight reference pixels
	WALIGNR0	wR11, wR3, wR4			@ align low ref 8 pixel	
	WLDRD		wR3, [r1, #8]			@ load low eight reference pixels
	WSADB		wR14, wR0, wR10			@ calculate SAD for low 8 pixels
    WLDRD		wR0, [r0], #8			@ load low eight source pixels
	WSADB		wR15, wR1, wR11			@ calculate SAD for high 8 pixels
	WLDRD		wR4, [r1, #16]			@ load high eight reference pixels
	ADD			r1, r1, r3				@ ref add refstride
	WLDRD		wR1, [r0], #8			@ load high eight source pixels	
	WALIGNR0	wR10, wR7, wR8			@ align low ref 8 pixel
    WLDRD		wR7, [r1]				@ load low eight reference pixels
	WALIGNR0	wR11, wR8, wR9			@ align low ref 8 pixel	
	WLDRD		wR8, [r1, #8]			@ load low eight reference pixels
	WSADB		wR14, wR5, wR10			@ calculate SAD for low 8 pixels
    WLDRD		wR5, [r0], #8			@ load low eight source pixels
	WSADB		wR15, wR6, wR11			@ calculate SAD for high 8 pixels
	WLDRD		wR9, [r1, #16]			@ load high eight reference pixels
	SUBS		r6, r6, #1				@ decrement loop counter 
    WLDRD		wR6, [r0], #8			@ load low eight source pixels
	BNE Lhight1 

	WSTRW		wR14, [r4]				@ save PC[NORTHWEST_DIVISION];
	WSTRW		wR15, [r4, #4]

	WADDW		wR15, wR15, wR14

	@!< pC[SOUTHWEST_DIVISION] and  pC[SOUTHEAST_DIVISION]	

	MOV		r6, #4						@ setup loop count

Lhight2:
	SUBS		r6, r6, #1				@ decrement loop counter 
	ADDNE		r1, r1, r3				@ ref add refstride
	WALIGNR0	wR10, wR2, wR3			@ align low ref 8 pixel
    WLDRDNE		wR2, [r1]				@ load low eight reference pixels
	WALIGNR0	wR11, wR3, wR4			@ align low ref 8 pixel	
	WLDRDNE		wR3, [r1, #8]			@ load low eight reference pixels
	WSADB		wR12, wR0, wR10			@ calculate SAD for low 8 pixels
    WLDRDNE		wR0, [r0], #8			@ load low eight source pixels
	WSADB		wR13, wR1, wR11			@ calculate SAD for high 8 pixels
	WLDRDNE		wR4, [r1, #16]			@ load high eight reference pixels
	ADDNE		r1, r1, r3				@ ref add refstride
	WLDRDNE		wR1, [r0], #8			@ load high eight source pixels	
	WALIGNR0	wR10, wR7, wR8			@ align low ref 8 pixel
    WLDRDNE		wR7, [r1]				@ load low eight reference pixels
	WALIGNR0	wR11, wR8, wR9			@ align low ref 8 pixel	
	WLDRDNE		wR8, [r1, #8]			@ load low eight reference pixels
	WSADB		wR12, wR5, wR10			@ calculate SAD for low 8 pixels
    WLDRDNE		wR5, [r0], #8			@ load low eight source pixels
	WSADB		wR13, wR6, wR11			@ calculate SAD for high 8 pixels
	WLDRDNE		wR9, [r1, #16]			@ load high eight reference pixels
    WLDRDNE		wR6, [r0], #8			@ load low eight source pixels
	BNE Lhight2 

	WSTRW		wR12, [r4, #8]			@ save PC[SOUTHWEST_DIVISION];
	WSTRW		wR13, [r4, #12]			@ save PC[SOUTHEAST_DIVISION]

	WADDW		wR15, wR15, wR12
	WADDW		wR15, wR15, wR13

	TEXTRMSH	r0, wR15, #0   @ return result    
    ldmia		sp!, {r4, r6, r7, pc}		@ restore and return

	.section .text
	.global  sad16v_wmmx2
	.type    sad16v_wmmx2, function

sad16v_wmmx2:
.ALIGN 4
	stmdb		sp!, {r4-r7, lr}		@ save regs used
	ldr		r5, [sp, #24]
    WLDRD	wR0, [r0]			@ load low eight source pixels
    AND		r7,  r1,  #7		@ r7 = 3LSBs of *pRef
	WLDRD	wR1, [r0, #8]	 	@ load high eight source pixels	
    BIC		r1,  r1,  #7		@ r0 64-bit aligned  
    WLDRD	wR2, [r1]			@ load low eight reference pixels
    TMCR	wCGR0, r7			@ transfer alignment to wCGR0
	WLDRD	wR3, [r1, #8]		@ load middle eight reference pixels
	WZERO	wR14				@ zero low accumulator
	WLDRD	wR4, [r1, #16]		@ load high eight reference pixels
	ADD		r1, r1, r3
	add		r0, r0, r2
    WLDRD	wR5, [r0]			@ load low eight source pixels
    WZERO	wR15				@ zero high accumulator
    WLDRD	wR6, [r0, #8]		@ load low eight source pixels
	MOV		r6, #4				@ setup loop count
    WLDRD	wR7, [r1]			@ load low eight ref pixels
	WZERO	wR13
    WLDRD	wR8, [r1, #8]		@ load low eight ref pixels
	WZERO	wR12
    WLDRD	wR9, [r1, #16]		@ load low eight source pixels
	ldr		r4, [sp, #20]

	@!< pC[NORTHWEST_DIVISION] and  pC[NORTHEAST_DIVISION]
Lhight11:
	ADD			r1, r1, r3				@ ref add refstride

	add			r0, r0, r2
	WALIGNR0	wR10, wR2, wR3			@ align low ref 8 pixel
    WLDRD		wR2, [r1]				@ load low eight reference pixels
	WALIGNR0	wR11, wR3, wR4			@ align low ref 8 pixel	
	WLDRD		wR3, [r1, #8]			@ load low eight reference pixels
	WSADB		wR14, wR0, wR10			@ calculate SAD for low 8 pixels
	wstrd		wr0, [r5], #8
    WLDRD		wR0, [r0]				@ load low eight source pixels
	WSADB		wR15, wR1, wR11			@ calculate SAD for high 8 pixels
	wstrd		wr1, [r5], #8
	WLDRD		wR4, [r1, #16]			@ load high eight reference pixels
	ADD			r1, r1, r3				@ ref add refstride
	WLDRD		wR1, [r0, #8]			@ load high eight source pixels	
	add			r0, r0, r2
	WALIGNR0	wR10, wR7, wR8			@ align low ref 8 pixel
    WLDRD		wR7, [r1]				@ load low eight reference pixels
	WALIGNR0	wR11, wR8, wR9			@ align low ref 8 pixel	
	WLDRD		wR8, [r1, #8]			@ load low eight reference pixels
	WSADB		wR14, wR5, wR10			@ calculate SAD for low 8 pixels
	wstrd		wr5, [r5], #8
    WLDRD		wR5, [r0]				@ load low eight source pixels
	WSADB		wR15, wR6, wR11			@ calculate SAD for high 8 pixels
	wstrd		wr6, [r5], #8
	WLDRD		wR9, [r1, #16]			@ load high eight reference pixels
	SUBS		r6, r6, #1				@ decrement loop counter 
    WLDRD		wR6, [r0, #8]			@ load low eight source pixels
	BNE Lhight11 

	WSTRW		wR14, [r4]				@ save PC[NORTHWEST_DIVISION];
	WSTRW		wR15, [r4, #4]

	WADDW		wR15, wR15, wR14

	@!< pC[SOUTHWEST_DIVISION] and  pC[SOUTHEAST_DIVISION]	

	MOV		r6, #4						@ setup loop count

Lhight22:
	SUBS		r6, r6, #1				@ decrement loop counter 
	ADDNE		r1, r1, r3				@ ref add refstride
	ADDNE		r0, r0, r2
	WALIGNR0	wR10, wR2, wR3			@ align low ref 8 pixel
    WLDRDNE		wR2, [r1]				@ load low eight reference pixels
	WALIGNR0	wR11, wR3, wR4			@ align low ref 8 pixel	
	WLDRDNE		wR3, [r1, #8]			@ load low eight reference pixels
	WSADB		wR12, wR0, wR10			@ calculate SAD for low 8 pixels
	wstrd		wr0, [r5], #8
    WLDRDNE		wR0, [r0]				@ load low eight source pixels
	WSADB		wR13, wR1, wR11			@ calculate SAD for high 8 pixels
	wstrd		wr1, [r5], #8
	WLDRDNE		wR4, [r1, #16]			@ load high eight reference pixels
	ADDNE		r1, r1, r3				@ ref add refstride
	WLDRDNE		wR1, [r0, #8]			@ load high eight source pixels	
	addne		r0, r0, r2
	WALIGNR0	wR10, wR7, wR8			@ align low ref 8 pixel
    WLDRDNE		wR7, [r1]				@ load low eight reference pixels
	WALIGNR0	wR11, wR8, wR9			@ align low ref 8 pixel	
	WLDRDNE		wR8, [r1, #8]			@ load low eight reference pixels
	WSADB		wR12, wR5, wR10			@ calculate SAD for low 8 pixels
	wstrd		wr5, [r5], #8
    WLDRDNE		wR5, [r0]				@ load low eight source pixels
	WSADB		wR13, wR6, wR11			@ calculate SAD for high 8 pixels
	wstrd		wr6, [r5], #8
	WLDRDNE		wR9, [r1, #16]			@ load high eight reference pixels
    WLDRDNE		wR6, [r0, #8]			@ load low eight source pixels
	BNE Lhight22 

	WSTRW		wR12, [r4, #8]			@ save PC[SOUTHWEST_DIVISION];
	WSTRW		wR13, [r4, #12]			@ save PC[SOUTHEAST_DIVISION]

	WADDW		wR15, wR15, wR12
	WADDW		wR15, wR15, wR13

	TEXTRMSH	r0, wR15, #0   @ return result    
    ldmia		sp!, {r4-r7, pc}		@ restore and return


@void
@HalfpelRefineSad16x16(uint8_t *cur, 
@					  uint8_t *ref, 
@					  const int32_t ref_stride, 
@					  const int32_t rounding, 
@					  uint32_t* tmp_sad8)
@   (i+j+k+l+3)/4 = (s+t+1)/2 - (ij&kl)&st
@   (i+j+k+l+2)/4 = (s+t+1)/2 - (ij|kl)&st
@   (i+j+k+l+1)/4 = (s+t+1)/2 - (ij&kl)|st
@   (i+j+k+l+0)/4 = (s+t+1)/2 - (ij|kl)|st
@ with  s=(i+j+1)/2, t=(k+l+1)/2, ij = i^j, kl = k^l, st = s^t.

	.section .text
	.global  HalfpelRefineSad16x16_WMMX
	.type    HalfpelRefineSad16x16_WMMX, function

HalfpelRefineSad16x16_WMMX:
.ALIGN 4	
	stmdb		sp!, {r4-r7, lr}		@ save regs used
	ldr			r4, [sp, #20]			@ r7 = tmp_sad
	mov			r5, #1
	sub			r3, r5, r3				@ r3 = 1 - roudning

	sub			r1, r1, #1				@ ref - 1
	sub			r1, r1, r2				@ ref - 1 - stride
	and			r5, r1, #7	
	bic			r1, r1, #7

	tmcr		wcgr0, r5
	add			r5, r5, #1			
	tmcr		wcgr1, r5

	wldrd		wr0, [r1]				@ 1st line 1st eight source pixels
	wldrd		wr1, [r1, #8]			@ lst 2nd pel	
	wldrd		wr2, [r1, #16]			@ 1st 3rd pel
	wldrd		wr3, [r1, #24]			@ 1st 4th pel

	add			r1, r1, r2				@ ref + ref_stride
	mov			r6, #16					@ block height = 17
	mov			r7, #1

	walignr0	wr4, wr0, wr1			@ ref - 1 - ref_stride = i
	walignr0	wr6, wr1, wr2			
	walignr0	wr8, wr2, wr3

	wldrd		wr0, [r1]				@ 2nd line 1st eight source pixels
	wldrd		wr1, [r1, #8]			@ 2nd 2nd pel
	wldrd		wr2, [r1, #16]
	wldrd		wr3, [r1, #24]

	waligni		wr5, wr4, wr6, #1		@ ref - ref_stride = j
	waligni		wr7, wr6, wr8, #1
	waligni		wr9, wr8, wr6, #1

	walignr0	wr10, wr0, wr1			@ ref - 1  = k
	walignr0	wr12, wr1, wr2			
	walignr0	wr14, wr2, wr3

	waligni		wr11, wr10, wr12, #1	@ ref = l
	waligni		wr13, wr12, wr14, #1
	waligni		wr15, wr14, wr12, #1	

	@ 0
	wavg2br		wr0, wr4, wr5			@ (i+j+1)/2=s0  
	wavg2br		wr1, wr10, wr11			@ (k+l+1)/2=t0  h0 reserved

	wxor		wr2, wr0, wr1			@ s^t
	wavg2br		wr0, wr0, wr1			@ (s+t+1)/2 
	wxor		wr3, wr4, wr5			@ i^j
	wxor		wr4, wr10, wr11			@ k^l
	wor			wr3, wr3, wr4			@ i^j|k^l
	wand		wr2, wr3, wr2			@ (k^l|i^j)&s^t	
	wsubb		wr0, wr0, wr2			@ (s+t+1)/2 - (k^l|i^j)&s^t  hv0 reserved
	
	wavg2br		wr4, wr5, wr11			@ (k+l+1)/2  v0 reserved

	@ 1
	wavg2br		wr4, wr6, wr7			@ (i+j+1)/2=s1  
	wavg2br		wr2, wr12, wr13			@ (k+l+1)/2=t1  h1 reserved

	wxor		wr0, wr4, wr2			@ s^t
	wavg2br		wr4, wr4, wr2			@ (s+t+1)/2 
	wxor		wr5, wr6, wr7			@ i^j

	wxor		wr4, wr10, wr11			@ k^l
	wor			wr3, wr3, wr4			@ i^j|k^l
	wand		wr2, wr3, wr2			@ (k^l|i^j)&s^t	
	wsubb		wr0, wr0, wr2			@ (s+t+1)/2 - (k^l|i^j)&s^t  hv0 reserved
	
	wavg2br		wr2, wr5, wr11			@ (k+l+1)/2  v0 reserved


Lheight2:
	subs		r6, r6, #1
	addne		r3, r3, r8				@ src + src_stride
	walignr0	wr10, wr0, wr1			@ src - 1 = k
	walignr0	wr12, wr1, wr2
	walignr0	wr14, wr2, wr3
	waligni		wr11, wr10, wr12, #1
	waligni		wr13, wr12, wr14, #1
	waligni		wr15, wr14, wr12, #1	
	
	@ calculate v
	wavg2b		wr0, wr5, wr11			@ v:(j+l)/2
	wstrd		wr0, [r1], #8			@ store v
	wavg2b		wr1, wr7, wr13
	wstrd		wr1, [r1], #8
	wavg2b		wr2, wr9, wr15		
	wstrd		wr2, [r1],#8
	
	@ calculate h and hv
	@ 1st h and hv
	wavg2br		wr0, wr4, wr5			@ (i+j+1)/2=s
	wavg2b		wr1, wr10, wr11			@ h=(k+l)/2
	wstrd		wr1, [r0], #8			@ store h
	wavg2br		wr1, wr10, wr11			@ (k+l+1)/2=t
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr4, wr5			@ i^j
	wxor		wr3, wr10, wr11			@ k^l
	wand		wr1, wr1, wr3			@ i^j&k^l
	wor			wr1, wr1, wr0			@ (k^l&i^j)|s^t	
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l&i^j)|s^t	
	wstrd		wr2, [r2], #8			@ store hv		
	
	@ 2nd h and hv
	wavg2br		wr0, wr6, wr7			@ (i+j+1)/2=s
	wavg2b		wr1, wr12, wr13			@ h=(k+l)/2
	wstrd		wr1, [r0], #8			@ store h
	wavg2br		wr1, wr12, wr13			@ (k+l+1)/2=t
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr6, wr7			@ i^j
	wxor		wr3, wr12, wr13			@ k^l
	wand		wr1, wr1, wr3			@ i^j&k^l
	wor			wr1, wr1, wr0			@ (k^l&i^j)|s^t	
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l&i^j)|s^t	
	wstrd		wr2, [r2], #8			@ store hv			

	@ 3rd h and hv
	wavg2br		wr0, wr8, wr9			@ (i+j+1)/2=s
	wavg2b		wr1, wr14, wr15			@ h=(k+l)/2
	wstrd		wr1, [r0], #8			@ store h
	wavg2br		wr1, wr14, wr15			@ (k+l+1)/2=t
	wavg2br		wr2, wr0, wr1			@ (s+t+1)/2
	wxor		wr0, wr0, wr1			@ s^t
	wxor		wr1, wr8, wr9			@ i^j
	wxor		wr3, wr14, wr15			@ k^l
	wand		wr1, wr1, wr3			@ i^j&k^l
	wor			wr1, wr1, wr0			@ (k^l&i^j)|s^t	
	wldrdne		wr0, [r3]
	tbcstb		wr3, r7
	wand		wr1, wr1, wr3
	wsubb		wr2, wr2, wr1			@ (s+t+1)/2 - (k^l&i^j)|s^t	
	wstrd		wr2, [r2], #8			@ store hv	
	
	wmovne		wr4, wr10
	wldrdne		wr1, [r3, #8]
	wmovne		wr5, wr11
	wldrdne		wr2, [r3, #16]
	wmovne		wr6, wr12
	wldrdne		wr3, [r3, #24]
	wmovne		wr7, wr13
	wmovne		wr8, wr14
	wmovne		wr9, wr15
	bne			Lheight2				
	ldmia		sp!, {r4-r7, pc}		@ restore and return					  
   .end