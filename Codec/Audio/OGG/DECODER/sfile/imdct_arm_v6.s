;	/************************************************************************
;	*																		*
;	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
;	*																		*
;	************************************************************************/
;/***************************************************************
;
;	File:		imdct_arm_v6.s
;
;	Contains:	Inverse MDCT computition
;
;	Written by:	Witten Wen
;
;	Change History (most recent first):
;	2010-05-13		W.W			Create file
;
;****************************************************************/


		OPT         2       ; disable listing 
		#include "voOGGDecID.h"
		INCLUDE     kxarm.h
		INCLUDE		arm_version.h
		OPT         1       ; enable listing
		
		AREA    |.text|, CODE, READONLY
		
		IF	OPT_IMDCT_ARM	= 1
		
POSTPROCESS		EQU	0
		
		IF	ARMVERSION >= 6
		IMPORT	sincos_lookup0
		IMPORT	sincos_lookup1
		IMPORT	bitrev
		
		EXPORT	mdct_butterfly_generic	
		EXPORT	mdct_butterfly_8
		EXPORT	mdct_butterfly_16
		EXPORT	mdct_butterfly_32
		EXPORT	mdct_bitreverse
		EXPORT	mdct_preprocess
		;EXPORT	mdct_postprocess
		
;******************************************************************
;	
;	void mdct_butterfly_generic(DATA_TYPE *x,int points,int step)
;
;******************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for mdct_butterfly_generic
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x				RN	0
points			RN	1
STEP			RN	2

tp0				RN	12
tp1				RN	14

pT				RN	6
px1				RN	7
px2				RN	3
Tbegin			RN	4
Tend			RN	5


x1_1			RN	8
x1_2			RN	9
x2_1			RN	10
x2_2			RN	11

T0				RN	8
T1				RN	9

result0			RN	10
result1			RN	11

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Constants for mdct_butterfly_generic
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;iStackSpaceRev			EQU		1*4
;iOffset_sincoslookup	EQU		iStackSpaceRev-4

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   MACROs for mdct_butterfly_generic
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO             
    BUTTERFLY_GENERIC_FORWARD0 $px1, $px2, $pT, $STEP
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    LDRD	x1_1, [px1]
	LDRD	x2_1, [px2]
	SUB		tp0, x1_1, x2_1
	ADD		x1_1, x1_1, x2_1
	SUB		tp1, x2_2, x1_2
	ADD		x1_2, x1_2, x2_2	
	STRD	x1_1, [px1], #-8
	LDRD	T0, [pT]	
	;XPROD31(OGG_S32 a, OGG_S32 b, OGG_S32 t, OGG_S32 v, OGG_S32 *x, OGG_S32 *y)
	SMMULR	result0, tp1, T0
	SMMLAR	result0, tp0, T1, result0
	SMMULR	result1, tp0, T0
	SMMLSR	result1, tp1, T1, result1
	MOV		result0, result0, LSL #1
	MOV		result1, result1, LSL #1
	STRD	result0, [px2], #-8
	ADD		pT, pT, STEP, LSL #2
	MEND
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO             
    BUTTERFLY_GENERIC_BACKWARD0 $px1, $px2, $pT, $STEP
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    LDRD	x1_1, [px1]
	LDRD	x2_1, [px2]
	SUB		tp0, x1_1, x2_1
	ADD		x1_1, x1_1, x2_1
	SUB		tp1, x1_2, x2_2
	ADD		x1_2, x1_2, x2_2	
	STRD	x1_1, [px1], #-8
	LDRD	T0, [pT]	
	;XNPROD31(OGG_S32 a, OGG_S32 b, OGG_S32 t, OGG_S32 v, OGG_S32 *x, OGG_S32 *y);
	SMMULR	result0, tp0, T0
	SMMLSR	result0, tp1, T1, result0
	SMMULR	result1, tp1, T0
	SMMLAR	result1, tp0, T1, result1
	MOV		result0, result0, LSL #1
	MOV		result1, result1, LSL #1
	STRD	result0, [px2], #-8
	SUB		pT, pT, STEP, LSL #2
	MEND
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO             
    BUTTERFLY_GENERIC_FORWARD1 $px1, $px2, $pT, $STEP
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    LDRD	x1_1, [px1]
	LDRD	x2_1, [px2]
	SUB		tp0, x2_1, x1_1
	ADD		x1_1, x1_1, x2_1
	SUB		tp1, x2_2, x1_2
	ADD		x1_2, x1_2, x2_2	
	STRD	x1_1, [px1], #-8
	LDRD	T0, [pT]	
	SMMULR	result0, tp0, T0
	SMMLAR	result0, tp1, T1, result0
	SMMULR	result1, tp1, T0
	SMMLSR	result1, tp0, T1, result1
	MOV		result0, result0, LSL #1
	MOV		result1, result1, LSL #1
	STRD	result0, [px2], #-8
	ADD		pT, pT, STEP, LSL #2
	MEND
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO             
    BUTTERFLY_GENERIC_BACKWARD1 $px1, $px2, $pT, $STEP
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDRD	x1_1, [px1]
	LDRD	x2_1, [px2]
	SUB		tp0, x1_1, x2_1
	ADD		x1_1, x1_1, x2_1
	SUB		tp1, x2_2, x1_2
	ADD		x1_2, x1_2, x2_2	
	STRD	x1_1, [px1], #-8
	LDRD	T0, [pT]
	SMMULR	result0, tp1, T0
	SMMLSR	result0, tp0, T1, result0
	SMMULR	result1, tp0, T0
	SMMLAR	result1, tp1, T1, result1
	MOV		result0, result0, LSL #1
	MOV		result1, result1, LSL #1
	STRD	result0, [px2], #-8
	SUB		pT, pT, STEP, LSL #2
	MEND
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	AREA    |.text|, CODE
  	LEAF_ENTRY mdct_butterfly_generic 
  	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	STMFD	sp!, {r4-r11, lr}
	LDR		pT, =sincos_lookup0
	ADD		px1, x, points, LSL #2
	SUB		px1, px1, #8
	ADD		px2, x, points, LSL #1	;(points>>1)<<2
	SUB		px2, px2, #8	
;	STR		pT, [sp, #iOffset_sincoslookup]
	MOV		Tbegin, pT
	ADD		Tend, pT, #0x1000		;1024*4
	
Butterfly_forward0	
	BUTTERFLY_GENERIC_FORWARD0 px1, px2, pT, STEP
	
	BUTTERFLY_GENERIC_FORWARD0 px1, px2, pT, STEP
	
	BUTTERFLY_GENERIC_FORWARD0 px1, px2, pT, STEP
	
	BUTTERFLY_GENERIC_FORWARD0 px1, px2, pT, STEP
	
	CMP		pT, Tend
	BLT		Butterfly_forward0
	
;	LDR		Tend, [sp, #iOffset_sincoslookup]
Butterfly_backward0	
	BUTTERFLY_GENERIC_BACKWARD0	px1, px2, pT, STEP
	
	BUTTERFLY_GENERIC_BACKWARD0	px1, px2, pT, STEP
	
	BUTTERFLY_GENERIC_BACKWARD0	px1, px2, pT, STEP
	
	BUTTERFLY_GENERIC_BACKWARD0	px1, px2, pT, STEP
	
	CMP		pT, Tbegin
	BGT		Butterfly_backward0
	
Butterfly_forward1
	BUTTERFLY_GENERIC_FORWARD1 px1, px2, pT, STEP
	
	BUTTERFLY_GENERIC_FORWARD1 px1, px2, pT, STEP
	
	BUTTERFLY_GENERIC_FORWARD1 px1, px2, pT, STEP
	
	BUTTERFLY_GENERIC_FORWARD1 px1, px2, pT, STEP
	
	CMP		pT, Tend
	BLT		Butterfly_forward1
	
Butterfly_backward1
	BUTTERFLY_GENERIC_BACKWARD1 px1, px2, pT, STEP
	
	BUTTERFLY_GENERIC_BACKWARD1 px1, px2, pT, STEP
	
	BUTTERFLY_GENERIC_BACKWARD1 px1, px2, pT, STEP
	
	BUTTERFLY_GENERIC_BACKWARD1 px1, px2, pT, STEP
	
	CMP		pT, Tbegin
	BGT		Butterfly_backward1
	
	LDMFD	sp!, {r4-r11, pc}
	ENTRY_END	mdct_butterfly_generic
	
;******************************************************************
;	
;	void mdct_butterfly_8(DATA_TYPE *x)
;
;******************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for mdct_butterfly_8
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x					RN	0
px					RN	1

x0					RN	2
x1					RN	3
x2					RN	4
x3					RN	5
x4					RN	6
x5					RN	7
x6					RN	8
x7					RN	9

tp0					RN	10
tp1					RN	6
tp2					RN	11
tp3					RN	7
tp4					RN	12
tp5					RN	8
tp6					RN	14
tp7					RN	9

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	AREA    |.text|, CODE
  	LEAF_ENTRY mdct_butterfly_8
  	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	STMFD	sp!, {r4-r11, lr}	
	MOV		px, x
	
	LDRD	x0, [px], #8
	LDRD	x2, [px], #8
	LDRD	x4, [px], #8
	LDRD	x6, [px]

	ADD		tp0, x4, x0
	SUB		tp1, x4, x0
	ADD		tp2, x5, x1
	SUB		tp3, x5, x1
	ADD		tp4, x6, x2
	SUB		tp5, x6, x2
	ADD		tp6, x7, x3
	SUB		tp7, x7, x3
	
	ADD		x0, tp5, tp3
	SUB		x1, tp7, tp1
	SUB		x2, tp5, tp3
	ADD		x3, tp7, tp1
	SUB		x4, tp4, tp0
	SUB		x5, tp6, tp2
	ADD		x6, tp4, tp0
	ADD		x7, tp6, tp2
	
	STRD	x0, [x], #8
	STRD	x2, [x], #8
	STRD	x4, [x], #8
	STRD	x6, [x]
	
	LDMFD	sp!, {r4-r11, pc}
	ENTRY_END	mdct_butterfly_8
	
;******************************************************************
;	
;	void mdct_butterfly_16(DATA_TYPE *x)
;
;******************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for mdct_butterfly_16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x_16				RN	0
px1_16				RN	1
px2_16				RN	2
T1_16				RN	3

x1_1_16				RN	4
x1_2_16				RN	5
x2_1_16				RN	6
x2_2_16				RN	7

tp0_16				RN	8
tp1_16				RN	9
tp2_16				RN	10

rt0_16				RN	6
rt1_16				RN	7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Static Data for mdct_butterfly_16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	AREA    |.text|, CODE
  	LEAF_ENTRY mdct_butterfly_16
  	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	STMFD	sp!, {r4-r10, lr}
	ADD		px2_16, x_16, #0x20		;x+8
	MOV		px1_16, x_16
	LDR		T1_16, cPI2_8

	LDRD	x1_1_16,	[px1_16]
	LDRD	x2_1_16,	[px2_16]
	SUB		tp0_16,		x1_1_16, x2_1_16
	ADD		x2_1_16,	x2_1_16, x1_1_16
	SUB		tp1_16,		x1_2_16, x2_2_16
	ADD		x2_2_16,	x2_2_16, x1_2_16
	STRD	x2_1_16,	[px2_16], #8
	SUB		tp2_16,		tp1_16, tp0_16
	ADD		tp1_16,		tp1_16, tp0_16
	SMMULR	rt0_16,		tp1_16, T1_16
	SMMULR	rt1_16,		tp2_16, T1_16
	MOV		rt0_16,		rt0_16, LSL #1
	MOV		rt1_16,		rt1_16, LSL #1
	STRD	rt0_16,		[px1_16], #8
	
	LDRD	x1_1_16,	[px1_16]
	LDRD	x2_1_16,	[px2_16]
	SUB		tp1_16,		x2_1_16, x1_1_16
	ADD		x2_1_16,	x2_1_16, x1_1_16
	SUB		tp0_16,		x1_2_16, x2_2_16
	ADD		x2_2_16,	x2_2_16, x1_2_16
	STRD	x2_1_16,	[px2_16], #8
	STRD	tp0_16,		[px1_16], #8
	
	LDRD	x1_1_16,	[px1_16]
	LDRD	x2_1_16,	[px2_16]
	SUB		tp0_16,		x2_1_16, x1_1_16
	ADD		x2_1_16,	x2_1_16, x1_1_16
	SUB		tp1_16,		x2_2_16, x1_2_16
	ADD		x2_2_16,	x2_2_16, x1_2_16
	STRD	x2_1_16,	[px2_16], #8
	ADD		tp2_16,		tp0_16, tp1_16
	SUB		tp1_16,		tp0_16, tp1_16
	SMMULR	rt0_16,		tp1_16, T1_16
	SMMULR	rt1_16,		tp2_16, T1_16
	MOV		rt0_16,		rt0_16, LSL #1
	MOV		rt1_16,		rt1_16, LSL #1
	STRD	rt0_16,		[px1_16], #8
	
	LDRD	x1_1_16,	[px1_16]
	LDRD	x2_1_16,	[px2_16]
	SUB		tp0_16,		x2_1_16, x1_1_16
	ADD		x2_1_16,	x2_1_16, x1_1_16
	SUB		tp1_16,		x2_2_16, x1_2_16
	ADD		x2_2_16,	x2_2_16, x1_2_16
	STRD	x2_1_16,	[px2_16]
	STRD	tp0_16,		[px1_16]
	
	MOV		r4, x_16
	BL		mdct_butterfly_8
	
	ADD		r0, r4, #0x20
	BL		mdct_butterfly_8
	
	LDMFD	sp!, {r4-r10, pc}
	ENTRY_END	mdct_butterfly_16
	
;******************************************************************
;	
;	void mdct_butterfly_32(DATA_TYPE *x)
;
;******************************************************************
	
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for mdct_butterfly_32
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x				RN	0
px1				RN	2
px2				RN	3

x1_1			RN	4
x1_2			RN	5
x2_1			RN	6
x2_2			RN	7

tp0				RN	8
tp1				RN	9
tp2				RN	12

rt0				RN	6
rt1				RN	7

T0_32			RN	10
T1_32			RN	11
T2_32			RN	1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   MACROs for mdct_butterfly_32
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO             
    READ_FORWARD_PROCESS $px1, $px2
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDRD	x1_1, [px1]
	LDRD	x2_1, [px2]
	SUB		tp0, x1_1, x2_1
	ADD		x2_1, x2_1, x1_1
	SUB		tp1, x1_2, x2_2
	ADD		x2_2, x2_2, x1_2
	STRD	x2_1, [px2], #8
	MEND

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO             
    READ_BACKWARD_PROCESS $px1, $px2
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	LDRD	x1_1, [px1]
	LDRD	x2_1, [px2]
	SUB		tp0, x2_1, x1_1
	ADD		x2_1, x2_1, x1_1
	SUB		tp1, x2_2, x1_2
	ADD		x2_2, x2_2, x1_2
	STRD	x2_1, [px2], #8
	MEND
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Static Data for mdct_butterfly_32
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
cPI3_8		DCD		0x30fbc54d
cPI2_8		DCD		0x5a82799a
cPI1_8		DCD		0x7641af3d

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	AREA    |.text|, CODE
  	LEAF_ENTRY mdct_butterfly_32 
  	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	STMFD	sp!, {r4-r11, lr}
	ADD		px2, x, #0x40		;x+16
	MOV		px1, x
	
	LDR		T0_32, cPI1_8
	LDR		T1_32, cPI2_8
	LDR		T2_32, cPI3_8
	
	READ_FORWARD_PROCESS	px1, px2
	SMMULR	rt0, tp0, T0_32
	SMMLAR	rt0, tp1, T2_32, rt0
	SMMULR	rt1, tp1, T0_32
	SMMLSR	rt1, tp0, T2_32, rt1
	MOV		rt0, rt0, LSL #1
	MOV		rt1, rt1, LSL #1
	STRD	rt0, [px1], #8
	
	READ_FORWARD_PROCESS	px1, px2
	SUB		tp2, tp1, tp0
	ADD		tp1, tp1, tp0
	SMMULR	rt0, tp1, T1_32
	SMMULR	rt1, tp2, T1_32
	MOV		rt0, rt0, LSL #1
	MOV		rt1, rt1, LSL #1
	STRD	rt0, [px1], #8
	
	READ_FORWARD_PROCESS	px1, px2
	SMMULR	rt0, tp0, T2_32
	SMMLAR	rt0, tp1, T0_32, rt0
	SMMULR	rt1, tp1, T2_32
	SMMLSR	rt1, tp0, T0_32, rt1
	MOV		rt0, rt0, LSL #1
	MOV		rt1, rt1, LSL #1
	STRD	rt0, [px1], #8
	
	LDRD	x1_1, [px1]
	LDRD	x2_1, [px2]
	SUB		tp1, x2_1, x1_1
	ADD		x2_1, x2_1, x1_1
	SUB		tp0, x1_2, x2_2
	ADD		x2_2, x2_2, x1_2
	STRD	x2_1, [px2], #8
	STRD	tp0, [px1], #8
	
	READ_BACKWARD_PROCESS	px1, px2
	SMMULR	rt0, tp0, T2_32
	SMMLSR	rt0, tp1, T0_32, rt0
	SMMULR	rt1, tp1, T2_32
	SMMLAR	rt1, tp0, T0_32, rt1
	MOV		rt0, rt0, LSL #1
	MOV		rt1, rt1, LSL #1
	STRD	rt0, [px1], #8
	
	READ_BACKWARD_PROCESS	px1, px2
	ADD		tp2, tp0, tp1
	SUB		tp1, tp0, tp1
	SMMULR	rt0, tp1, T1_32
	SMMULR	rt1, tp2, T1_32
	MOV		rt0, rt0, LSL #1
	MOV		rt1, rt1, LSL #1
	STRD	rt0, [px1], #8
	
	READ_BACKWARD_PROCESS	px1, px2
	SMMULR	rt0, tp0, T0_32
	SMMLSR	rt0, tp1, T2_32, rt0
	SMMULR	rt1, tp1, T0_32
	SMMLAR	rt1, tp0, T2_32, rt1
	MOV		rt0, rt0, LSL #1
	MOV		rt1, rt1, LSL #1
	STRD	rt0, [px1], #8
	
	READ_BACKWARD_PROCESS	px1, px2
	STRD	tp0, [px1]
	
	MOV		r4, r0
	BL		mdct_butterfly_16
	
	ADD		r0, r4, #0x40	;x+16
	BL		mdct_butterfly_16
	
	LDMFD	sp!, {r4-r11,pc}
	ENTRY_END	mdct_butterfly_32
	
	
;******************************************************************
;	
;	void mdct_bitreverse(DATA_TYPE *x,int n,int step,int shift)
;
;******************************************************************	
	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for mdct_bitreverse
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x					RN	0
Ttop				RN	1
step				RN	2
shift				RN	3

T					RN	4
w0					RN	5
w1					RN	6
bit					RN	7

px0					RN	12
px1					RN	14

x0_a				RN	8
x0_b				RN	9
x1_a				RN	10
x1_b				RN	11

w0_a				RN	8
w0_b				RN	9
w1_a				RN	10
w1_b				RN	11

T0_bv				RN	2
T1_bv				RN	3

temp0				RN	12
temp1				RN	14
temp2				RN	0
temp3				RN	1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Constants for mdct_bitreverse
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
iStackSpaceRev		EQU	6*4
iOffset_x			EQU	iStackSpaceRev-4
iOffset_Ttop		EQU	iStackSpaceRev-8
iOffset_step		EQU	iStackSpaceRev-12
iOffset_shift		EQU	iStackSpaceRev-16
iOffset_bitrev		EQU	iStackSpaceRev-20
iOffset_0XFFF		EQU	iStackSpaceRev-24

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   MACROs for mdct_bitreverse
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO
    BITREV12 $bit
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    LDR		r0, [sp, #iOffset_bitrev]
    LDRB	temp3, [r0, bit, LSR #8]
    AND		r8, bit, #0xf0
    LDRB	r2, [r0, r8, LSR #4]
    AND		r8, bit, #0xf
    LDRB	r3, [r0, r8]
    ORR		temp3, temp3, r2, LSL #4
    ORR		temp3, temp3, r3, LSL #8
    MEND
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	AREA    |.text|, CODE
  	LEAF_ENTRY mdct_bitreverse 
  	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	STMFD	sp!, {r4-r11, lr}
	SUB		sp, sp, #iStackSpaceRev
	
	MOV		w0, x
	ADD		x, w0, r1, LSL	#1		;w0+((n>>1)<<2)
	SUB		w1, x, #8
	MOV		bit, #0

	CMP		step, #4
	LDR		T, =sincos_lookup0
	ADD		T, T, step, LSL #1
	LDRLT	T, =sincos_lookup1
	ADD		Ttop, T, #0x1000	;1024*4
	LDR		r8, =bitrev
	MOV		r9, #0x1000
	SUB		r9, r9, #1
	
	STR		x, [sp, #iOffset_x]
	STR		Ttop, [sp, #iOffset_Ttop]
	STR		step, [sp, #iOffset_step]
	STR		shift, [sp, #iOffset_shift]	
	STR		r8, [sp, #iOffset_bitrev]
	STR		r9, [sp, #iOffset_0XFFF]

FirstLoop	
	BITREV12	bit
	ADD		bit, bit, #1
	LDR		x, [sp, #iOffset_x]
	LDR		temp1, [sp, #iOffset_0XFFF]
	LDR		shift, [sp, #iOffset_shift]
	EOR		temp0, temp3, temp1
	MOV		temp0, temp0, LSR shift
	SUB		temp0, temp0, #1
	ADD		px0, x, temp0, LSL #2
	MOV		temp3, temp3, LSR shift
	ADD		px1, x, temp3, LSL #2
	
	LDRD	x0_a, [px0]
	LDRD	x1_a, [px1]
	LDRD	T0_bv, [T]
	ADD		temp0, x0_a, x1_a
	SUB		temp1, x1_b, x0_b
	SMMUL	temp2, temp0, T1_bv
	SMMLA	temp2, temp1, T0_bv, temp2
	SMMUL	temp3, temp1, T1_bv
	SMMLS	temp3, temp0, T0_bv, temp3
	LDR		step, [sp, #iOffset_step]	
	ADD		temp0, x0_b, x1_b
	SUB		temp1, x0_a, x1_a
	ADD		T, T, step LSL #2
	
	ADD		w0_a, temp2, temp0, ASR #1
	ADD		w0_b, temp3, temp1, ASR #1
	RSB		w1_a, temp2, temp0, ASR #1
	SUB		w1_b, temp3, temp1, ASR #1
	STRD	w0_a, [w0], #8
	STRD	w1_a, [w1], #-8
	
	BITREV12	bit
	ADD		bit, bit, #1
	LDR		x, [sp, #iOffset_x]
	LDR		temp1, [sp, #iOffset_0XFFF]
	LDR		shift, [sp, #iOffset_shift]
	EOR		temp0, temp3, temp1
	MOV		temp0, temp0, LSR shift
	SUB		temp0, temp0, #1
	ADD		px0, x, temp0, LSL #2
	MOV		temp3, temp3, LSR shift
	ADD		px1, x, temp3, LSL #2
	
	LDRD	x0_a, [px0]
	LDRD	x1_a, [px1]
	LDRD	T0_bv, [T]
	ADD		temp0, x0_a, x1_a
	SUB		temp1, x1_b, x0_b
	SMMUL	temp2, temp0, T1_bv
	SMMLA	temp2, temp1, T0_bv, temp2
	SMMUL	temp3, temp1, T1_bv
	SMMLS	temp3, temp0, T0_bv, temp3
	LDR		step, [sp, #iOffset_step]	
	ADD		temp0, x0_b, x1_b
	SUB		temp1, x0_a, x1_a
	ADD		T, T, step LSL #2
	
	ADD		w0_a, temp2, temp0, ASR #1
	ADD		w0_b, temp3, temp1, ASR #1
	RSB		w1_a, temp2, temp0, ASR #1
	SUB		w1_b, temp3, temp1, ASR #1
	LDR		Ttop, [sp, #iOffset_Ttop]
	STRD	w0_a, [w0], #8
	STRD	w1_a, [w1], #-8
	CMP		T, Ttop
	BLT		FirstLoop
	
SecondLoop
	BITREV12	bit
	ADD		bit, bit, #1
	LDR		x, [sp, #iOffset_x]
	LDR		temp1, [sp, #iOffset_0XFFF]
	LDR		shift, [sp, #iOffset_shift]
	EOR		temp0, temp3, temp1
	MOV		temp0, temp0, LSR shift
	SUB		temp0, temp0, #1
	ADD		px0, x, temp0, LSL #2
	MOV		temp3, temp3, LSR shift
	ADD		px1, x, temp3, LSL #2
	
	LDR		step, [sp, #iOffset_step]	
	LDRD	x0_a, [px0]
	SUB		T, T, step, LSL #2
	LDRD	x1_a, [px1]
	LDRD	T0_bv, [T]
	ADD		temp0, x0_a, x1_a
	SUB		temp1, x1_b, x0_b
	SMMUL	temp2, temp0, T0_bv
	SMMLA	temp2, temp1, T1_bv, temp2
	SMMUL	temp3, temp1, T0_bv
	SMMLS	temp3, temp0, T1_bv, temp3
	ADD		temp0, x0_b, x1_b
	SUB		temp1, x0_a, x1_a
	
	ADD		w0_a, temp2, temp0, ASR #1
	ADD		w0_b, temp3, temp1, ASR #1
	RSB		w1_a, temp2, temp0, ASR #1
	SUB		w1_b, temp3, temp1, ASR #1
	STRD	w0_a, [w0], #8
	STRD	w1_a, [w1], #-8
	
	BITREV12	bit
	ADD		bit, bit, #1
	LDR		x, [sp, #iOffset_x]
	LDR		temp1, [sp, #iOffset_0XFFF]
	LDR		shift, [sp, #iOffset_shift]
	EOR		temp0, temp3, temp1
	MOV		temp0, temp0, LSR shift
	SUB		temp0, temp0, #1
	ADD		px0, x, temp0, LSL #2
	MOV		temp3, temp3, LSR shift
	ADD		px1, x, temp3, LSL #2
	
	LDR		step, [sp, #iOffset_step]	
	LDRD	x0_a, [px0]
	SUB		T, T, step, LSL #2
	LDRD	x1_a, [px1]
	LDRD	T0_bv, [T]
	ADD		temp0, x0_a, x1_a
	SUB		temp1, x1_b, x0_b
	SMMUL	temp2, temp0, T0_bv
	SMMLA	temp2, temp1, T1_bv, temp2
	SMMUL	temp3, temp1, T0_bv
	SMMLS	temp3, temp0, T1_bv, temp3
	ADD		temp0, x0_b, x1_b
	SUB		temp1, x0_a, x1_a
	
	ADD		w0_a, temp2, temp0, ASR #1
	ADD		w0_b, temp3, temp1, ASR #1
	RSB		w1_a, temp2, temp0, ASR #1
	SUB		w1_b, temp3, temp1, ASR #1
	STRD	w0_a, [w0], #8
	STRD	w1_a, [w1], #-8
	CMP		w0, w1
	BLT		SecondLoop
	
	ADD		sp, sp, #iStackSpaceRev
	LDMFD	sp!, {r4-r11, pc}
	ENTRY_END	mdct_bitreverse
	
	
	
;************************************************************************
;	
;	void mdct_preprocess(DATA_TYPE *in, DATA_TYPE *out, int n4, int step)
;
;************************************************************************	


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for mdct_preprocess
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
In					RN	0
Out					RN	1
n4					RN	2

step_pre			RN	3
pIn					RN	4
pOut				RN	5
pA					RN	6

In_n4				RN	7

temp0				RN	12
temp1				RN	14

t0					RN	10
t1					RN	11

y0					RN	8
y1					RN	9

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	AREA    |.text|, CODE
  	LEAF_ENTRY mdct_preprocess 
  	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	STMFD	sp!, {r4-r11, lr}
	
	ADD		pIn, In, n4, LSL #3		;(n4<<1)<<2
	SUB		pIn, pIn, #4
	ADD		temp0, n4, n4, LSL #1
	SUB		temp0, temp0, #2
	ADD		pOut, Out, temp0, LSL #2	
	LDR		pA, =sincos_lookup0
	MOV		step_pre, step_pre, LSL #2
	
	ADD		In_n4, In, n4, LSL #2
LOOP0_0
	LDR		temp0, [pIn], #-8
	LDR		temp1, [pIn], #-8
	LDRD	t0, [pA], step_pre
	SMMUL	y1, temp0, t0
	SMMLS	y1, temp1, t1, y1
	SMMUL	y0, temp1, t0
	SMMLA	y0, temp0, t1, y0
	MOV		y1, y1, LSL #1
	MOV		y0, y0, LSL #1
	STRD	y0, [pOut], #-8
	
	LDR		temp0, [pIn], #-8
	LDR		temp1, [pIn], #-8
	LDRD	t0, [pA], step_pre
	SMMUL	y1, temp0, t0
	SMMLS	y1, temp1, t1, y1
	SMMUL	y0, temp1, t0
	SMMLA	y0, temp0, t1, y0
	MOV		y1, y1, LSL #1
	MOV		y0, y0, LSL #1
	STRD	y0, [pOut], #-8
	CMP		pIn, In_n4
	BCS		LOOP0_0
	
LOOP0_1
	LDR		temp0, [pIn], #-8
	LDR		temp1, [pIn], #-8
	LDRD	t0, [pA], -step_pre
	SMMUL	y1, temp0, t1
	SMMLS	y1, temp1, t0, y1
	SMMUL	y0, temp1, t1
	SMMLA	y0, temp0, t0, y0
	MOV		y1, y1, LSL #1
	MOV		y0, y0, LSL #1
	STRD	y0, [pOut], #-8
	
	LDR		temp0, [pIn], #-8
	LDR		temp1, [pIn], #-8
	LDRD	t0, [pA], -step_pre
	SMMUL	y1, temp0, t1
	SMMLS	y1, temp1, t0, y1
	SMMUL	y0, temp1, t1
	SMMLA	y0, temp0, t0, y0
	MOV		y1, y1, LSL #1
	MOV		y0, y0, LSL #1
	STRD	y0, [pOut], #-8
	CMP		pIn, In
	BCS		LOOP0_1
	
	ADD		pIn, In, n4, LSL #3		;(n4<<1)<<2
	SUB		pIn, pIn, #8
	ADD		temp0, n4, n4, LSL #1
	ADD		pOut, Out, temp0, LSL #2
	LDR		pA, =sincos_lookup0
	
	ADD		In_n4, In, n4, LSL #2
	ADD		pA, pA, step_pre
LOOP1_0
	LDR		temp0, [pIn], #-8
	LDR		temp1, [pIn], #-8
	LDRD	t0, [pA], step_pre
	SMMUL	y0, temp0, t0
	SMMLS	y0, temp1, t1, y0
	SMMUL	y1, temp1, t0
	SMMLA	y1, temp0, t1, y1
	MOV		y1, y1, LSL #1
	MOV		y0, y0, LSL #1
	STRD	y0, [pOut], #8
	
	LDR		temp0, [pIn], #-8
	LDR		temp1, [pIn], #-8
	LDRD	t0, [pA], step_pre
	SMMUL	y0, temp0, t0
	SMMLS	y0, temp1, t1, y0
	SMMUL	y1, temp1, t0
	SMMLA	y1, temp0, t1, y1
	MOV		y1, y1, LSL #1
	MOV		y0, y0, LSL #1
	STRD	y0, [pOut], #8
	CMP		pIn, In_n4
	BCS		LOOP1_0
	
	SUB		pA, pA, step_pre, LSL #1
LOOP1_1
	LDR		temp0, [pIn], #-8
	LDR		temp1, [pIn], #-8
	LDRD	t0, [pA], -step_pre
	SMMUL	y0, temp0, t1
	SMMLS	y0, temp1, t0, y0
	SMMUL	y1, temp0, t0
	SMMLA	y1, temp1, t1, y1
	MOV		y1, y1, LSL #1
	MOV		y0, y0, LSL #1
	STRD	y0, [pOut], #8
	
	LDR		temp0, [pIn], #-8
	LDR		temp1, [pIn], #-8
	LDRD	t0, [pA], -step_pre
	SMMUL	y0, temp0, t1
	SMMLS	y0, temp1, t0, y0
	SMMUL	y1, temp0, t0
	SMMLA	y1, temp1, t1, y1
	MOV		y1, y1, LSL #1
	MOV		y0, y0, LSL #1
	STRD	y0, [pOut], #8
	CMP		pIn, In
	BCS		LOOP1_1
	
	LDMFD	sp!, {r4-r11, pc}
	ENTRY_END	mdct_preprocess
	
		IF	POSTPROCESS	= 1
;************************************************************************
;	
;	void mdct_postprocess(DATA_TYPE *in, DATA_TYPE *out, int n4, int step)
;
;************************************************************************	


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Registers for mdct_postprocess
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
n4_post				RN	2
step_post			RN	3

pOut1				RN	4
pOut2				RN	5
pIn_post			RN	12
pA_post				RN	14
pB_post				RN	11

In1					RN	6
In2					RN	7

Out1_1				RN	9
Out1_2				RN	8
Out2_1				RN	10
Out2_2				RN	11

t0_post				RN	0
t1_post				RN	1

v0_post				RN	2
v1_post				RN	3

q0_post				RN	11
q1_post				RN	14

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Constants for mdct_postprocess
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
iStackSpacePost		EQU	2*4
iOffset_pA			EQU	iStackSpacePost-4
iOffset_pB			EQU	iStackSpacePost-8

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   MACROs for mdct_postprocess
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO
    POSTDEFAULT1 $Out1_1, $Out2_1
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    LDRD	In1, [pIn_post], #8
	LDRD	t0_post, [pA_post], step_post
	RSB		In2, In2, #0
	SMMUL	Out1_1, In1, t0_post
	SMMLA	Out1_1, In2, t1_post, Out1_1
	SMMUL	Out2_1, In2, t0_post
	SMMLS	Out2_1, In1, t1_post, Out2_1
	MOV		Out1_1, Out1_1, LSL #1
	MOV		Out2_1, Out2_1, LSL #1
	MEND
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    MACRO
    POSTDEFAULT2 $Out1_2, $Out2_2
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    LDRD	In1, [pIn_post], #8
	LDRD	t0_post, [pA_post], step_post
	RSB		In2, In2, #0
	SMMUL	Out1_2, In1, t0_post
	SMMLA	Out1_2, In2, t1_post, Out1_2
	SMMUL	Out2_2, In2, t0_post
	SMMLS	Out2_2, In1, t1_post, Out2_2
	MOV		Out1_2, Out1_2, LSL #1
	MOV		Out2_2, Out2_2, LSL #1
	MEND
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	AREA    |.text|, CODE
  	LEAF_ENTRY mdct_postprocess 
  	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	STMFD	sp!, {r4-r11, lr}
	
	ADD		n4_post, n4_post, n4_post, LSL #1
	ADD		pOut2, r1, n4_post, LSL #2
	SUB		pOut1, pOut2, #8			;pOut1 = pOut2 - 2
	MOV		pIn_post, r1
	
	CMP		step_post, #1
	BEQ		STEP1
	CMP		step_post, #0
	BEQ		STEP0
	
	CMP		step_post, #4
	LDR		pA_post, =sincos_lookup0
	ADD		pA_post, pA_post, step_post, LSL #1		;sincos_lookup0+(step>>1)
	LDRLT	pA_post, =sincos_lookup1
	MOV		step_post, step_post, LSL #2	
LOOPDEFAULT
	POSTDEFAULT1 Out1_1, Out2_1
	POSTDEFAULT2 Out1_2, Out2_2
	STRD	Out1_2, [pOut1], #-8
	STRD	Out2_1, [pOut2], #8	
	POSTDEFAULT1 Out1_1, Out2_1
	POSTDEFAULT2 Out1_2, Out2_2
	STRD	Out1_2, [pOut1], #-8
	STRD	Out2_1, [pOut2], #8
	CMP		pIn_post, pOut1
	BLT		LOOPDEFAULT
	B		POSTPROCESSEND
	
STEP1
	LDR		pA_post, =sincos_lookup0
	LDR		pB_post, =sincos_lookup1
	LDRD	t0_post, [pA_post], #8
	MOV		t0_post, t0_post, ASR #1
	MOV		t1_post, t1_post, ASR #1
LOOPSTEP1	
	LDRD	v0_post, [pB_post], #8
	LDRD	In1, [pIn_post], #8	
	MOV		v0_post, v0_post, ASR #1
	MOV		v1_post, v1_post, ASR #1
	RSB		In2, In2, #0
	ADD		t0_post, t0_post, v0_post
	ADD		t1_post, t1_post, v1_post	
	SMMUL	Out2_1, In2, t0_post
	SMMLS	Out2_1, In1, t1_post, Out2_1
	SMMUL	Out1_1, In1, t0_post
	SMMLA	Out1_1, In2, t1_post, Out1_1	
	MOV		Out2_1, Out2_1, LSL #1
	STR		Out2_1, [pOut2], #4	
	MOV		Out1_1, Out1_1, LSL #1
	
	LDRD	t0_post, [pA_post], #8
	LDRD	In1, [pIn_post], #8	
	MOV		t0_post, t0_post, ASR #1
	MOV		t1_post, t1_post, ASR #1
	RSB		In2, In2, #0
	ADD		v0_post, v0_post, t0_post
	ADD		v1_post, v1_post, t1_post
	SMMUL	Out1_2, In1, v0_post
	SMMLA	Out1_2, In2, v1_post, Out1_2
	SMMUL	Out2_1, In2, v0_post
	SMMLS	Out2_1, In1, v1_post, Out2_1
	MOV		Out1_2, Out1_2, LSL #1
	MOV		Out2_1, Out2_1, LSL #1
	STRD	Out1_2, [pOut1], #-8
	STR		Out2_1, [pOut2], #4
	
	LDRD	In1, [pIn_post], #8
	LDRD	v0_post, [pB_post], #8
	RSB		In2, In2, #0
	MOV		v0_post, v0_post, ASR #1
	MOV		v1_post, v1_post, ASR #1
	ADD		t0_post, t0_post, v0_post
	ADD		t1_post, t1_post, v1_post
	SMMUL	Out1_1, In1, t0_post
	SMMLA	Out1_1, In2, t1_post, Out1_1
	SMMUL	Out2_1, In2, t0_post
	SMMLS	Out2_1, In1, t1_post, Out2_1
	MOV		Out1_1, Out1_1, LSL #1
	MOV		Out2_1, Out2_1, LSL #1
	STR		Out2_1, [pOut2], #4
	
	LDRD	In1, [pIn_post], #8
	LDRD	t0_post, [pA_post], #8
	RSB		In2, In2, #0
	MOV		t0_post, t0_post, ASR #1
	MOV		t1_post, t1_post, ASR #1
	ADD		v0_post, v0_post, t0_post
	ADD		v1_post, v1_post, t1_post
	SMMUL	Out1_2, In1, v0_post
	SMMLA	Out1_2, In2, v1_post, Out1_2
	SMMUL	Out2_1, In2, v0_post
	SMMLS	Out2_1, In1, v1_post, Out2_1
	MOV		Out1_2, Out1_2, LSL #1
	MOV		Out2_1, Out2_1, LSL #1
	STRD	Out1_2, [pOut1], #-8
	STR		Out2_1, [pOut2], #4
	CMP		pIn_post, pOut1
	BLT		LOOPSTEP1
	B		POSTPROCESSEND
	
STEP0
	SUB		sp, sp, #iStackSpacePost
	LDR		pA_post, =sincos_lookup0
	LDR		pB_post, =sincos_lookup1
	LDRD	t0_post, [pA_post], #8
	STR		pA_post, [sp, #iOffset_pA]
	STR		pB_post, [sp, #iOffset_pB]
LOOPSTEP0
	LDRD	In1, [pIn_post], #8
	LDR		pB_post, [sp, #iOffset_pB]
	RSB		In2, In2, #0
	LDRD	v0_post, [pB_post], #8
	STR		pB_post, [sp, #iOffset_pB]
	SUB		q0_post, v0_post, t0_post
	SUB		q1_post, v1_post, t1_post
;	MOV		q0_post, q0_post, ASR #2
;	MOV		q1_post, q1_post, ASR #2
	ADD		t0_post, t0_post, q0_post, ASR #2
	ADD		t1_post, t1_post, q1_post, ASR #2
	SMMUL	Out1_1, In1, t0_post
	SMMLA	Out1_1, In2, t1_post, Out1_1
	SMMUL	Out2_1, In2, t0_post
	SMMLS	Out2_1, In1, t1_post, Out2_1
	MOV		Out1_1, Out1_1, LSL #1
	MOV		Out2_1, Out2_1, LSL #1
	STR		Out2_1, [pOut2], #4
	
	LDRD	In1, [pIn_post], #8
	SUB		t0_post, v0_post, q0_post, ASR #2
	SUB		t1_post, v1_post, q1_post, ASR #2
	RSB		In2, In2, #0
	SMMUL	Out1_2, In1, t0_post
	SMMLA	Out1_2, In2, t1_post, Out1_2
	SMMUL	Out2_1, In2, t0_post
	SMMLS	Out2_1, In1, t1_post, Out2_1
	MOV		Out1_2, Out1_2, LSL #1
	MOV		Out2_1, Out2_1, LSL #1
	STRD	Out1_2, [pOut1], #-8
	STR		Out2_1, [pOut2], #4
	
	LDRD	In1, [pIn_post], #8
	LDR		pA_post, [sp, #iOffset_pA]
	RSB		In2, In2, #0
	LDRD	t0_post, [pA_post], #8
	STR		pA_post, [sp, #iOffset_pA]
	SUB		q0_post, t0_post, v0_post
	SUB		q1_post, t1_post, v1_post
;	MOV		q0_post, q0_post, ASR #2
;	MOV		q1_post, q1_post, ASR #2
	ADD		v0_post, v0_post, q0_post, ASR #2
	ADD		v1_post, v1_post, q1_post, ASR #2
	SMMUL	Out1_1, In1, v0_post
	SMMLA	Out1_1, In2, v1_post, Out1_1
	SMMUL	Out2_1, In2, v0_post
	SMMLS	Out2_1, In1, v1_post, Out2_1
	MOV		Out1_1, Out1_1, LSL #1
	MOV		Out2_1, Out2_1, LSL #1
	STR		Out2_1, [pOut2], #4
	
	LDRD	In1, [pIn_post], #8
	SUB		v0_post, t0_post, q0_post, ASR #2
	SUB		v1_post, t1_post, q1_post, ASR #2
	RSB		In2, In2, #0
	SMMUL	Out1_2, In1, v0_post
	SMMLA	Out1_2, In2, v1_post, Out1_2
	SMMUL	Out2_1, In2, v0_post
	SMMLS	Out2_1, In1, v1_post, Out2_1
	MOV		Out1_2, Out1_2, LSL #1
	MOV		Out2_1, Out2_1, LSL #1
	STRD	Out1_2, [pOut1], #-8
	STR		Out2_1, [pOut2], #4
	CMP		pIn_post, pOut1
	BLT		LOOPSTEP0
	ADD		sp, sp, #iStackSpacePost
	
POSTPROCESSEND
	LDMFD	sp!, {r4-r11, pc}
	ENTRY_END	mdct_postprocess
	
	ENDIF	;POSTPROCESS
	
		ENDIF	;ARMVERSION 
		ENDIF	;OPT_IMDCT_ARM
		END
		
		