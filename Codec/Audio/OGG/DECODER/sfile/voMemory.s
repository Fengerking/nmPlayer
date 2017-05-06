;	/************************************************************************
;	*																		*
;	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
;	*																		*
;	************************************************************************/
;/***************************************************************
;
;	File:		voMemory.s
;
;	Contains:	memory function
;
;	Written by:	Witten Wen
;
;	Change History (most recent first):
;	2009-05-26		W.W			Create file
;
;****************************************************************/


		OPT         2       ; disable listing 
		#include "voOGGDecID.h"
		INCLUDE     kxarm.h
		INCLUDE		arm_version.h
		OPT         1       ; enable listing

		AREA    |.text|, CODE, READONLY
		
		IF		OPT_VOMEMORY_ARM = 1
		
		EXPORT		voMemset
		EXPORT		voMemcpy		

;******************************************************************
;	
;	void *voMemset(void *pSrc, int n, size_t size);
;
;******************************************************************
		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for voMemset
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pSrc					RN	6
n						RN	1
size					RN	2

PreAddrRest				RN	3

n_32bits1				RN	4
n_32bits2				RN	5


	AREA    |.text|, CODE
  	LEAF_ENTRY voMemset 
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 	
;r0 = pSrc;
;r1 = n;
;r2 = size;
  	
  	STMFD	sp!, {r4-r6, lr}
  	
  	CMP		r0, #0
  	BEQ		EndMemset
  	CMP		size, #0
  	BEQ		EndMemset
  	
  	MOV		pSrc, r0
  	AND		n, n, #0xff 
  	ANDS	PreAddrRest, pSrc, #0x3  	
	BEQ		Align4
	RSB		r4, PreAddrRest, #4
	CMP		r4, size
	MOVHI	r4, size
	MOV		PreAddrRest, r4
PreLoop
	STRB	n, [pSrc], #1  	 
	SUBS	PreAddrRest, PreAddrRest, #1
	BNE		PreLoop
	SUBS	size, size, r4
	BEQ		EndMemset
	
Align4
	MOVS	r3, size, LSR #3							;size/8
	BEQ		PostSize
	ORR		n_32bits1, n, n, LSL #8
	ORR		n_32bits2, n_32bits1, n_32bits1, LSL #16
	MOV		n_32bits1, n_32bits2
MainLoop	
	IF		ARMVERSION	>= 5
	STRD	n_32bits1, [pSrc], #8
	ELSE
	STR		n_32bits1, [pSrc], #4
	STR		n_32bits2, [pSrc], #4
	ENDIF
	SUBS	r3, r3, #1
	BNE		MainLoop
	
PostSize
	ANDS		size, size, #7
	BEQ			EndMemset
PostLoop	
	STRB	n, [pSrc], #1
	SUBS	size, size, #1
	BNE		PostLoop	
EndMemset
	LDMFD	sp!, {r4-r6, pc}
	ENTRY_END	voMemset
	
	
;******************************************************************
;	
;	void *voMemcpy(void *pDst, const void *pSrc, size_t size);
;
;******************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for voMemcpy
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

pDst					RN		5
pSrc_cpy				RN		1
size_cpy				RN		2

SrcAddrRest				RN		3
DstAddrRest				RN		4

DATA1					RN		6
DATA2					RN		7
	AREA    |.text|, CODE
  	LEAF_ENTRY voMemcpy 
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
;r0 = pDst;
;r1 = pSrc;
;r2 = size_cpy;

	STMFD	sp!, {r4-r7, lr}
	
	CMP		size_cpy, #0
	BEQ		EndMemcpy
	
	MOV		pDst, r0
	AND		SrcAddrRest, pSrc_cpy, #0x3
	AND		DstAddrRest, pDst, #0x3
	CMP		SrcAddrRest, DstAddrRest
	BNE		CPYPostSize
	
	CMP		SrcAddrRest, #0
	BEQ		CPYAlign4
	RSB		r4, SrcAddrRest, #4	
	CMP		r4, size_cpy
	MOVHI	r4, size_cpy
	MOV		SrcAddrRest, r4
CPYPreLoop
	LDRB	DATA1, [pSrc_cpy], #1
	SUBS	SrcAddrRest, SrcAddrRest, #1
	STRB	DATA1, [pDst], #1	
	BNE		CPYPreLoop
	SUBS	size_cpy, size_cpy, r4
	BEQ		EndMemcpy
	
CPYAlign4
	MOVS	r3, size_cpy, LSR #3							;size/8
	BEQ		CPYPostSize
CPYMainLoop
	IF		ARMVERSION	>= 5
	LDRD	DATA1, [pSrc_cpy], #8							;armv6
	SUBS	r3, r3, #1
	STRD	DATA1, [pDst], #8
	ELSE
	LDR		DATA1, [pSrc_cpy], #4							;armv4
	LDR		DATA2, [pSrc_cpy], #4
	SUBS	r3, r3, #1
	STR		DATA1, [pDst], #4
	STR		DATA2, [pDst], #4
	ENDIF	
	BNE		CPYMainLoop
	
	ANDS		size_cpy, size_cpy, #7
	BEQ			EndMemcpy
CPYPostSize
	LDRB	DATA1, [pSrc_cpy], #1
	SUBS	size_cpy, size_cpy, #1
	STRB	DATA1, [pDst], #1	
	BNE		CPYPostSize
	
EndMemcpy
	LDMFD	sp!, {r4-r7, pc}
	ENTRY_END	voMemcpy
	
	ENDIF		;OPT_VOMEMORY_ARM
			END