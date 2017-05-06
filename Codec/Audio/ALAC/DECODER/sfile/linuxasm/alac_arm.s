@/**********************************************************************
@*
@* VisualOn voplayer
@*
@* CopyRight (C) VisualOn SoftWare Co., Ltd. All rights are reserved.
@*
@***********************************************************************/

@/*********************************************************************
@*
@* Filename:	
@*		alac_arm.s
@* 
@* Abstact:	
@*
@*		Apple Lossless Audio Codec for VisualOn Decoder ARM function file.
@*
@* Author:
@*
@*		Witten Wen 20-April-2010
@*
@* Revision History:
@*
@******************************************************/

	.INCLUDE		"kxarm.h"
	.INCLUDE		"arm_version.h"
	
	@AREA	|.text|, CODE, READONLY
	.section	.text
	
	.IF		ARM == 1
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.global	CountHeadZeros
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@         int CountHeadZeros(int input)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		@AREA	|.text|, CODE
		LEAF_ENTRY CountHeadZeros
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	.IF		ARMVERSION	>= 5
	CLZ			r0, r0
	.ELSE
	MOV			r1, r0
	MOV			r0, #0
	CMP			r1, #0x10000
	MOVCC		r1, r1, LSL #16
	ADDCC		r0, r0, #16
	TST			r1, #0xFF000000
	MOVEQ		r1, r1, LSL #8
	ADDEQ		r0, r0, #8
	TST			r1, #0xF0000000
	MOVEQ		r1, r1, LSL #4
	ADDEQ		r0, r0, #4
	TST			r1, #0xC0000000
	MOVEQ		r1, r1, LSL #2
	ADDEQ		r0, r0, #2
	TST			r1, #0x80000000
	MOVEQ		r1, r1, LSL #1
	ADDEQS		r0, r0, #1
	MOVEQ		r0, #32
	.ENDIF		@ARMVERSION
	MOV			pc, lr	
	ENTRY_END	CountHeadZeros
	
	.ENDIF		@ARM
	
	.END
	
	