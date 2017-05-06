;//*@@@+++@@@@******************************************************************
;//
;// Microsoft Windows Media
;// Copyright (C) Microsoft Corporation. All rights reserved.
;//
;//*@@@---@@@@******************************************************************

;//************************************************************************
;//
;// Module Name:
;//
;//     interpolate_arm.s
;//
;// Abstract:
;//  
;//     ARM specific WMV9 interpolation
;//     Optimized assembly routines to implement motion compensation
;//
;//     Custom build with 
;//          armasm $(InputDir)\$(InputName).s $(OutDir)\$(InputName).obj
;//     and
;//          $(OutDir)\$(InputName).obj
;// 
;// Author:
;// 
;//     Chuang Gu (chuanggu@microsoft.com) Oct. 8, 2002
;//
;// Revision History:
;//
;//************************************************************************

    INCLUDE wmvdec_member_arm.inc
    INCLUDE xplatform_arm_asm.h 
    IF UNDER_CE != 0
    INCLUDE kxarm.h
    ENDIF

    IF WMV_OPT_MOTIONCOMP_ARM=1

    AREA |.text|, CODE, READONLY
       
    EXPORT  g_NewVertFilterX_ARMV4
    EXPORT  g_NewHorzFilterX_ARMV4
	EXPORT  g_NewVertFilter0LongNoGlblTbl_ARMV4
    EXPORT  g_InterpolateBlock_00_SSIMD_ARMV4
    EXPORT  g_AddNull_SSIMD_ARMV4
    EXPORT  g_AddNullB_SSIMD_ARMV4
    EXPORT  g_InterpolateBlockBilinear_SSIMD_ARMV4
    EXPORT  g_InterpolateBlockBilinear_SSIMD_11_ARMV4
    EXPORT  g_InterpolateBlockBilinear_SSIMD_01_ARMV4
    EXPORT  g_InterpolateBlockBilinear_SSIMD_10_ARMV4
	EXPORT  IntensityComp_ARMV4


     AREA  |.text|, CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;Void_WMV  g_InterpolateBlockBilinear_SSIMD (const U8_WMV *pSrc, 
;                                           I32_WMV iSrcStride, 
;                                           U8_WMV *pDst, 
;                                           I32_WMV iXFrac, 
;                                           I32_WMV iYFrac, 
;                                           I32_WMV iRndCtrl, 
;                                           Bool_WMV b1MV)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;{  
;    I32_WMV i, j, k0,k1;
;    I32_WMV iNumLoops = 8<<b1MV;
;    U8_WMV *pD ; 
;    const U8_WMV  *pT ;
;    iRndCtrl = 8 - ( iRndCtrl&0xff); 
;    pT = pSrc;	
;    pD = pDst;
;
;	if( 0 == b1MV )	{
;	I16_WMV a,b,c,d;
;	    a = (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac;
;	    for (i = 0; i < 8; i++) 
;	    {
;	//0
;	        b = (pT[1]<<2)+  ( pT[iSrcStride+1] - pT[1])* iYFrac;
;	        c = (pT[2]<<2)+  ( pT[iSrcStride+2] - pT[2])* iYFrac;
;	        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+0) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+20) = (U8_WMV) k1;
;	
;	        d = (pT[3]<<2)+  ( pT[iSrcStride+3] - pT[3])* iYFrac;
;	        a = (pT[4]<<2)+  ( pT[iSrcStride+4] - pT[4])* iYFrac;
;	        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+2) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+22) = (U8_WMV) k1;
;	//1
;	        b = (pT[5]<<2)+  ( pT[iSrcStride+5] - pT[5])* iYFrac;
;	        c = (pT[6]<<2)+  ( pT[iSrcStride+6] - pT[6])* iYFrac;
;	        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+4) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+24) = (U8_WMV) k1;
;	
;	        d = (pT[7]<<2)+  ( pT[iSrcStride+7] - pT[7])* iYFrac;
;	        a = (pT[8]<<2)+  ( pT[iSrcStride+8] - pT[8])* iYFrac;
;	        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+6) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+26) = (U8_WMV) k1;
;
;	        pT += iSrcStride;
;	        pD += 40;
;	    } 
;	else {
;		I16_WMV a,b,c,d;
;        a = (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac;
;	    for (i = 0; i < 16; i++) 
;	    {
;//0
;        b = (pT[1]<<2)+  ( pT[iSrcStride+1] - pT[1])* iYFrac;
;        c = (pT[2]<<2)+  ( pT[iSrcStride+2] - pT[2])* iYFrac;
;        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+0) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+20) = (U8_WMV) k1;
;
;        d = (pT[3]<<2)+  ( pT[iSrcStride+3] - pT[3])* iYFrac;
;        a = (pT[4]<<2)+  ( pT[iSrcStride+4] - pT[4])* iYFrac;
;        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+2) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+22) = (U8_WMV) k1;
;//1
;        b = (pT[5]<<2)+  ( pT[iSrcStride+5] - pT[5])* iYFrac;
;        c = (pT[6]<<2)+  ( pT[iSrcStride+6] - pT[6])* iYFrac;
;        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+4) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+24) = (U8_WMV) k1;
;
;        d = (pT[7]<<2)+  ( pT[iSrcStride+7] - pT[7])* iYFrac;
;        a = (pT[8]<<2)+  ( pT[iSrcStride+8] - pT[8])* iYFrac;
;        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+6) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+26) = (U8_WMV) k1;
;	//2
;	        b = (pT[9]<<2)+  ( pT[iSrcStride+9] - pT[9])* iYFrac;
;	        c = (pT[10]<<2)+  ( pT[iSrcStride+10] - pT[10])* iYFrac;
;	        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+8) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+28) = (U8_WMV) k1;
;	
;	        d = (pT[11]<<2)+  ( pT[iSrcStride+11] - pT[11])* iYFrac;
;	        a = (pT[12]<<2)+  ( pT[iSrcStride+12] - pT[12])* iYFrac;
;	        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+10) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+30) = (U8_WMV) k1;
;	//3
;	        b = (pT[13]<<2)+  ( pT[iSrcStride+13] - pT[13])* iYFrac;
;	        c = (pT[14]<<2)+  ( pT[iSrcStride+14] - pT[14])* iYFrac;
;	        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+12) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+32) = (U8_WMV) k1;
;	
;	        d = (pT[15]<<2)+  ( pT[iSrcStride+15] - pT[15])* iYFrac;
;	        a = (pT[16]<<2)+  ( pT[iSrcStride+16] - pT[16])* iYFrac;
;	        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;	        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;	        *(I16_WMV *)(pD+14) = (U8_WMV) k0;
;	        *(I16_WMV *)(pD+34) = (U8_WMV) k1;
;
;        pT += iSrcStride;
;        pD += 40;
;    } 
;}
;}

    AREA  |.text|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlockBilinear_SSIMD_ARMV4

;r0 = pSrc
;r12 = pSrc + iSrcStride	
;r1 = iSrcStride
;r2 = pDst
;r3 = iXFrac
;r4 = iYFrac
;r5 = iRndCtl
;r14 = iNumLoops;
;r6-r11

;; stack usage
IBB_StackSize              EQU 0x24
IBB_OffsetRegSaving        EQU 0x24

IBB_Offset_iYFrac          EQU IBB_StackSize + IBB_OffsetRegSaving + 0
IBB_Offset_iRndCtrl        EQU IBB_StackSize + IBB_OffsetRegSaving + 4
IBB_Offset_b1MV            EQU IBB_StackSize + IBB_OffsetRegSaving + 8


    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

    ;I32_WMV iNumLoops = 8<<b1MV;

	
    ldr r6, [sp, #44]
	pld		[r0, #32]

    ldr r10, [sp, #40]
    mov r12, #8
    mov r14, r12, lsl r6				;r14 = iNumLoops
  
    ;iRndCtrl = 8 - ( iRndCtrl&0xff);
    and r10, r10, #0xff
    sub r5 , r12, r10					;r5 = iRndCtrl

    ldr  r4, [sp, #36]					;r4=iYFrac

	add r12, r0, r1						;r12 = pT + iSrcStride
	cmp	r6, #0
	bne lab_16_loop
loop_g_InterpolateBlockBilinear_SSIMD

;        a = (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac;
;//0
;        b = (pT[1]<<2)+  ( pT[iSrcStride+1] - pT[1])* iYFrac;
;        c = (pT[2]<<2)+  ( pT[iSrcStride+2] - pT[2])* iYFrac;
;        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+0) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+20) = (U8_WMV) k1;
;
;        d = (pT[3]<<2)+  ( pT[iSrcStride+3] - pT[3])* iYFrac;
;        a = (pT[4]<<2)+  ( pT[iSrcStride+4] - pT[4])* iYFrac;
;        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+2) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+22) = (U8_WMV) k1;

    ldrb r6, [ r0];
    ldrb r9, [ r12];
    ldrb r7, [ r0, #1];
    ldrb r10, [ r12, #1];
        
	sub	r9, r9, r6
    ldrb r8, [ r0, #2];
	mul	r9, r4, r9
    
	sub	r10, r10, r7
    ldrb r11, [ r12, #2];

	mul	r10, r4, r10
	sub	r11, r11, r8
	add r9, r9, r6, lsl #2		;a

	mul	r11, r4, r11
	add r10, r10, r7, lsl #2	;b
	sub	r6, r10, r9
	add r11, r11, r8, lsl #2	;c

	mul	r6, r3, r6
	sub	r7, r11, r10
	add	r9, r5, r9, lsl #2
	add	r6, r6, r9

	mul	r7, r3, r7
	mov	r6, r6, asr #4			;k0
    strh r6, [r2]
	add	r10, r5, r10, lsl #2
	add	r7, r7, r10


;;;;;;;;;;;;;;;;;;;;;
    ldrb r6, [ r0, #3];
    ldrb r9, [ r12, #3];
    
	mov	r7, r7, asr #4			;k1
    strh r7, [r2, #20]
    ldrb r7, [ r0, #4];
	sub	r9, r9, r6
    ldrb r10, [ r12, #4];
   
	mul	r9, r4, r9
	sub	r10, r10, r7 
		
	mul	r10, r4, r10
	add r6, r9, r6, lsl #2		;d
	sub	r8, r6, r11
	add r9, r10, r7, lsl #2		;a

	mul	r8, r3, r8
	add	r7, r5, r11, lsl #2
	sub	r10, r9, r6
	add	r8, r8, r7

	mul	r10, r3, r10
	mov	r8, r8, asr #4			;k0
	add	r6, r5, r6, lsl #2
	add	r10, r10, r6
    strh r8, [r2, #2]
	mov	r10, r10, asr #4			;k1

    strh r10, [r2, #22]

;//1
;        b = (pT[5]<<2)+  ( pT[iSrcStride+5] - pT[5])* iYFrac;
;        c = (pT[6]<<2)+  ( pT[iSrcStride+6] - pT[6])* iYFrac;
;        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+4) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+24) = (U8_WMV) k1;
;
;        d = (pT[7]<<2)+  ( pT[iSrcStride+7] - pT[7])* iYFrac;
;        a = (pT[8]<<2)+  ( pT[iSrcStride+8] - pT[8])* iYFrac;
;        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+6) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+26) = (U8_WMV) k1;
    ldrb r7, [ r0, #5];
    ldrb r10, [ r12, #5];
    ldrb r8, [ r0, #6];
    ldrb r11, [ r12, #6];

	sub	r10, r10, r7
	sub	r11, r11, r8	
	mul	r10, r4, r10

	mul	r11, r4, r11
	add r10, r10, r7, lsl #2	;b
	sub	r6, r10, r9
	
	add r11, r11, r8, lsl #2	;c
	mul	r6, r3, r6

	sub	r7, r11, r10
	add	r9, r5, r9, lsl #2

	mul	r7, r3, r7
	add	r10, r5, r10, lsl #2
	add	r6, r6, r9
	add	r7, r7, r10
	mov	r6, r6, asr #4			;k0
	mov	r7, r7, asr #4			;k1

    strh r6, [r2, #4]
    
;;;;;;;;;;;;;;;;;;;;;
    ldrb r6, [ r0, #7];
    ldrb r9, [ r12, #7];
    strh r7, [r2, #24]    
    ldrb r7, [ r0, #8];
	sub	r9, r9, r6
    ldrb r10, [ r12, #8];


	mul	r9, r4, r9
	sub	r10, r10, r7
	subs r14, r14, #1
	add r6, r9, r6, lsl #2		;d
	
	sub	r8, r6, r11
	mul	r10, r4, r10
	
	mul	r8, r3, r8
	add r9, r10, r7, lsl #2		;a
	add	r7, r5, r11, lsl #2
	add	r8, r8, r7
	sub	r10, r9, r6
	mov	r8, r8, asr #4			;k0

	mul	r10, r3, r10
    strh r8, [r2, #6]
	add	r6, r5, r6, lsl #2
	add r0, r0, r1
	
	add	r10, r10, r6
	pld	[r0, #32]
	
	add r12, r12, r1
	mov	r10, r10, asr #4			;k1

    strh r10, [r2, #26]

;        pT += iSrcStride;
;        pD += 40;
	add r2, r2, #40
	bne	loop_g_InterpolateBlockBilinear_SSIMD

    ldmia     sp!, {r4 - r11, pc}


lab_16_loop
loop_g_InterpolateBlockBilinear_SSIMD_16

;        a = (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac;
;//0
;        b = (pT[1]<<2)+  ( pT[iSrcStride+1] - pT[1])* iYFrac;
;        c = (pT[2]<<2)+  ( pT[iSrcStride+2] - pT[2])* iYFrac;
;        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+0) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+20) = (U8_WMV) k1;
;
;        d = (pT[3]<<2)+  ( pT[iSrcStride+3] - pT[3])* iYFrac;
;        a = (pT[4]<<2)+  ( pT[iSrcStride+4] - pT[4])* iYFrac;
;        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+2) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+22) = (U8_WMV) k1;


    ldrb r6, [ r0];
    ldrb r9, [ r12];
    ldrb r7, [ r0, #1];
    ldrb r10, [ r12, #1];

	sub	r9, r9, r6
    ldrb r8, [ r0, #2];
    ldrb r11, [ r12, #2];
	mul	r9, r4, r9
	sub	r10, r10, r7

	sub	r11, r11, r8
	mul	r10, r4, r10
	mul	r11, r4, r11
	add r9, r9, r6, lsl #2		;a
	add r10, r10, r7, lsl #2	;b

	sub	r6, r10, r9
	add r11, r11, r8, lsl #2	;c

	mul	r6, r3, r6
	add	r9, r5, r9, lsl #2
	sub	r7, r11, r10
	add	r6, r6, r9

	mul	r7, r3, r7
	mov	r6, r6, asr #4			;k0
	add	r10, r5, r10, lsl #2
	add	r7, r7, r10
    strh r6, [r2]



;;;;;;;;;;;;;;;;;;;;;
    ldrb r6, [ r0, #3];
    ldrb r9, [ r12, #3];
	mov	r7, r7, asr #4			;k1
    strh r7, [r2, #20]
    
    ldrb r7, [ r0, #4];
	sub	r9, r9, r6
    ldrb r10, [ r12, #4];

	mul	r9, r4, r9
	sub	r10, r10, r7
	sub	r8, r6, r11
	add r6, r9, r6, lsl #2		;d

	mul	r10, r4, r10

	mul	r8, r3, r8
	add r9, r10, r7, lsl #2		;a

	sub	r10, r9, r6
	add	r7, r5, r11, lsl #2
	mul	r10, r3, r10

	add	r6, r5, r6, lsl #2
	add	r8, r8, r7
	add	r10, r10, r6
	mov	r8, r8, asr #4			;k0
	mov	r10, r10, asr #4			;k1


;//1
;        b = (pT[5]<<2)+  ( pT[iSrcStride+5] - pT[5])* iYFrac;
;        c = (pT[6]<<2)+  ( pT[iSrcStride+6] - pT[6])* iYFrac;
;        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+4) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+24) = (U8_WMV) k1;
;
;        d = (pT[7]<<2)+  ( pT[iSrcStride+7] - pT[7])* iYFrac;
;        a = (pT[8]<<2)+  ( pT[iSrcStride+8] - pT[8])* iYFrac;
;        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+6) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+26) = (U8_WMV) k1;
    ldrb r7, [ r0, #5];
    strh r10, [r2, #22]
    ldrb r10, [ r12, #5];
    ldrb r11, [ r12, #6];
    strh r8, [r2, #2]
 	sub	r10, r10, r7
    ldrb r8, [ r0, #6];

	mul	r10, r4, r10
	sub	r11, r11, r8
	add	r9, r5, r9, lsl #2
	add r10, r10, r7, lsl #2	;b
	
	sub	r6, r10, r9
	mul	r11, r4, r11
	mul	r6, r3, r6
	
	add r11, r11, r8, lsl #2	;c
	sub	r7, r11, r10
	add	r6, r6, r9
	
	mul	r7, r3, r7
	mov	r6, r6, asr #4			;k0

	add	r10, r5, r10, lsl #2

;;;;;;;;;;;;;;;;;;;;;
    strh r6, [r2, #4]
    ldrb r6, [ r0, #7];
	add	r7, r7, r10
    ldrb r9, [ r12, #7];
	mov	r7, r7, asr #4			;k1

    strh r7, [r2, #24]
    ldrb r7, [ r0, #8];
	sub	r9, r9, r6
    ldrb r10, [ r12, #8];

	mul	r9, r4, r9
	sub	r10, r10, r7
	add	r7, r5, r11, lsl #2
	add r6, r9, r6, lsl #2		;d

	sub	r8, r6, r11
	mul	r10, r4, r10
	mul	r8, r3, r8
	
	add r9, r10, r7, lsl #2		;a
	sub	r10, r9, r6
	add	r8, r8, r7

	mul	r10, r3, r10
	mov	r8, r8, asr #4			;k0

	add	r6, r5, r6, lsl #2


;//2
;        b = (pT[9]<<2)+  ( pT[iSrcStride+9] - pT[9])* iYFrac;
;        c = (pT[10]<<2)+  ( pT[iSrcStride+10] - pT[10])* iYFrac;
;        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+8) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+28) = (U8_WMV) k1;
;
;        d = (pT[11]<<2)+  ( pT[iSrcStride+11] - pT[11])* iYFrac;
;        a = (pT[12]<<2)+  ( pT[iSrcStride+12] - pT[12])* iYFrac;
;        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+10) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+30) = (U8_WMV) k1;

 	add	r10, r10, r6
    ldrb r7, [ r0, #9];
	mov	r10, r10, asr #4			;k1
    strh r10, [r2, #26]
    ldrb r10, [ r12, #9];
    strh r8, [r2, #6]    
    ldrb r8, [ r0, #10];
	sub	r10, r10, r7
    ldrb r11, [ r12, #10];

	mul	r10, r4, r10
	sub	r11, r11, r8
	add	r9, r5, r9, lsl #2

	mul	r11, r4, r11
	sub	r6, r10, r9
	add r10, r10, r7, lsl #2	;b

	mul	r6, r3, r6
	add r11, r11, r8, lsl #2	;c
	sub	r7, r11, r10
	add	r6, r6, r9
	
	mul	r7, r3, r7
	mov	r6, r6, asr #4			;k0

	add	r10, r5, r10, lsl #2
	add	r7, r7, r10
    strh r6, [r2, #8]

;;;;;;;;;;;;;;;;;;;;;
    ldrb r6, [ r0, #11];
    ldrb r9, [ r12, #11];
	mov	 r7, r7, asr #4			;k1
    strh r7, [r2, #28]
    ldrb r7, [ r0, #12];

	sub	r9, r9, r6
    ldrb r10, [ r12, #12];
    
	mul	r9, r4, r9
	sub	r10, r10, r7
	add	r7, r5, r11, lsl #2

	mul	r10, r4, r10
	add r6, r9, r6, lsl #2		;d
	sub	r8, r6, r11
	add r9, r10, r7, lsl #2		;a

	mul	r8, r3, r8
	sub	r10, r9, r6
	add	r6, r5, r6, lsl #2
	
	mul	r10, r3, r10
	add	r8, r8, r7
    ldrb r7, [ r0, #13];
	add	r10, r10, r6
	
	mov	r8, r8, asr #4			;k0
	mov	r10, r10, asr #4			;k1


;//3
;        b = (pT[13]<<2)+  ( pT[iSrcStride+13] - pT[13])* iYFrac;
;        c = (pT[14]<<2)+  ( pT[iSrcStride+14] - pT[14])* iYFrac;
;        k0 = ((a <<2) + (b - a) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((b <<2) + (c - b) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+12) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+32) = (U8_WMV) k1;
;
;        d = (pT[15]<<2)+  ( pT[iSrcStride+15] - pT[15])* iYFrac;
;        a = (pT[16]<<2)+  ( pT[iSrcStride+16] - pT[16])* iYFrac;
;        k0 = ((c <<2) + (d - c) * iXFrac +  iRndCtrl) >> 4;
;        k1 = ((d <<2) + (a - d) * iXFrac + iRndCtrl) >> 4;
;        *(I16_WMV *)(pD+14) = (U8_WMV) k0;
;        *(I16_WMV *)(pD+34) = (U8_WMV) k1;


    strh r10, [r2, #30]
    ldrb r10, [ r12, #13];
    strh r8, [r2, #10]
    ldrb r8, [ r0, #14];
 	sub	r10, r10, r7
    ldrb r11, [ r12, #14];

	mul	r10, r4, r10
	sub	r11, r11, r8
	subs r14, r14, #1
	sub	r6, r10, r9

	mul	r11, r4, r11
	add r10, r10, r7, lsl #2	;b
	mul	r6, r3, r6
	add r11, r11, r8, lsl #2	;c

	add	r9, r5, r9, lsl #2
	sub	r7, r11, r10
	add	r6, r6, r9
	mul	r7, r3, r7
	mov	r6, r6, asr #4			;k0

	add	r10, r5, r10, lsl #2
	add	r7, r7, r10
    strh r6, [r2, #12]
	mov	r7, r7, asr #4			;k1


;;;;;;;;;;;;;;;;;;;;;
    ldrb r6, [ r0, #15];
    ldrb r9, [ r12, #15];
    strh r7, [r2, #32]
    ldrb r7, [ r0, #16];
	sub	r9, r9, r6
    ldrb r10, [ r12, #16];
	
	mul	r9, r4, r9
	add r0, r0, r1
	
	sub	r10, r10, r7
	pld	[r0, #32]

	add r6, r9, r6, lsl #2		;d

	mul	r10, r4, r10
	add r12, r12, r1
	sub	r8, r6, r11
	add r9, r10, r7, lsl #2		;a

	mul	r8, r3, r8
	sub	r10, r9, r6
	add	r7, r5, r11, lsl #2
	add	r8, r8, r7
	
	mul	r10, r3, r10
	mov	r8, r8, asr #4			;k0

	add	r6, r5, r6, lsl #2
    strh r8, [r2, #14]
	add	r10, r10, r6
	add r2, r2, #40
	mov	r10, r10, asr #4			;k1

    strh r10, [r2, #34]
;        pT += iSrcStride;
;        pD += 40;

	bne	loop_g_InterpolateBlockBilinear_SSIMD_16

    ldmia     sp!, {r4 - r11, pc}
    	
    WMV_ENTRY_END	;g_InterpolateBlockBilinear_SSIMD


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;Void_WMV  g_InterpolateBlockBilinear_SSIMD_11 (const U8_WMV *pSrc, 
;                                           I32_WMV iSrcStride, 
;                                           U8_WMV *pDst, 
;                                           I32_WMV iXFrac, 
;                                           I32_WMV iYFrac, 
;                                           I32_WMV iRndCtrl, 
;                                           Bool_WMV b1MV)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA  |.text|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlockBilinear_SSIMD_11_ARMV4

;r0=pSrc
;r1=iSrcStride
;r2=pDst
;r3=iRndCtl
;r4=i;
;r5=j
;r6=PF0
;r7=PF1
;r8=PF2
;r9=PF3
;r10=PF4
;r14=iNumLoops
;r11, r12 = tmp

; stack usage:
IBB_11_OffsetRegSaving        EQU	36
IBB_11_Offset_iYFrac          EQU	IBB_11_OffsetRegSaving + 0
IBB_11_Offset_iRndCtrl        EQU	IBB_11_OffsetRegSaving + 4
IBB_11_Offset_b1MV            EQU	IBB_11_OffsetRegSaving + 8

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT
 
;	I32_WMV iNumLoops = 8<<b1MV;
;	iRndCtrl = 8 - ( iRndCtrl&0xff);

	pld		[r0]
	pld		[r0, r1]
	
	ldr		r3 , [sp, #IBB_11_Offset_iRndCtrl]
	ldr		r12, [sp, #IBB_11_Offset_b1MV]
	mov		r14, #8
	and		r3, r3, #0xff
	rsb		r3, r3, #8			;iRndCtrl
	mov		r14, r14, lsl r12
	mov		r4, r14	
	
IBB_11_OuterLoop

 ;   for (i = 0; i < iNumLoops; i++) 
 ;   {
 ;       const U8_WMV  *pT ;
	;	I16_WMV PF0, PF1, PF2, PF3, PF4;
        
    ;    PF0 = pSrc[0] + pSrc[iSrcStride];
        
    ldrb	r6 , [r0]
    ldrb	r8 , [r0, r1]
	mov		r5, r14
	pld		[r0, r1, lsl #1]
	add		r6, r6, r8
IBB_11_InnerLoop
    ;    for (j = 0; j < iNumLoops; j += 4) {

    ;        pT = pSrc + j;
	;		PF1 = pT[1] + pT[iSrcStride+1];
	;		PF2 = pT[2] + pT[iSrcStride+2];
	;		PF3 = pT[3] + pT[iSrcStride+3];
	;		PF4 = pT[4] + pT[iSrcStride+4];
	
	add		r12, r0, r1
    ldrb	r7 , [r0, #1]
    ldrb	r9 , [r12, #1]
    ldrb	r8, [r0, #2]
    ldrb	r10, [r12, #2]
    
	add		r7, r7, r9
    ldrb	r9, [r0, #3]
    ldrb	r11, [r12, #3]
    
	add		r8, r8, r10
    ldrb	r10, [r0, #4]!
    ldrb	r12, [r12, #4]
    
	add		r9, r9, r11

     ;       k0 = (((PF0 + PF1) << 2) + iRndCtrl) >> 4;
     ;       k1 = (((PF1 + PF2) << 2) + iRndCtrl) >> 4;
     ;       *(I16_WMV *)(pDst + j) = (U8_WMV) k0;
     ;       *(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;
     
    add		r6, r6, r7
    add		r7, r7, r8
    add		r10, r10, r12
	add		r6, r3, r6, lsl #2
	add		r7, r3, r7, lsl #2
	mov		r6, r6, asr #4
	mov		r7, r7, asr #4
    strh	r7, [r2, #20]

     ;       k0 = (((PF2 + PF3) << 2) + iRndCtrl) >> 4;
     ;       k1 = (((PF3 + PF4) << 2) + iRndCtrl) >> 4;
     ;       *(I16_WMV *)(pDst + j + 2) = (U8_WMV) k0;
     ;       *(I16_WMV *)(pDst + j + 22) = (U8_WMV) k1;
     
    add		r8, r8, r9
    add		r9, r9, r10
	add		r8, r3, r8, lsl #2
	add		r9, r3, r9, lsl #2
	mov		r8, r8, asr #4
	mov		r9, r9, asr #4
    strh	r8, [r2, #2]
    strh	r9, [r2, #22]
    strh	r6, [r2], #4
    
		;	PF0 = PF4;
	mov		r6, r10
		
	subs	r5, r5, #4
	bne		IBB_11_InnerLoop
      ;  }
        
      ;  pSrc += iSrcStride;
      ;  pDst += 40;
 
	sub		r0, r0, r14
	sub		r2, r2, r14
	add		r0, r0, r1
	add		r2, r2, #40
	
	subs	r4, r4, #1	
	bne		IBB_11_OuterLoop
   ; }

	ldmia     sp!, {r4 - r11, pc}
;}	
		
	WMV_ENTRY_END	;g_InterpolateBlockBilinear_SSIMD_11



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;Void_WMV  g_InterpolateBlockBilinear_SSIMD_10 (const U8_WMV *pSrc, 
;                                           I32_WMV iSrcStride, 
;                                           U8_WMV *pDst, 
;                                           I32_WMV iXFrac, 
;                                           I32_WMV iYFrac, 
;                                           I32_WMV iRndCtrl, 
;                                           Bool_WMV b1MV)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA  |.text|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlockBilinear_SSIMD_10_ARMV4

;r0=pSrc
;r1=iSrcStride
;r2=pDst
;r3=iRndCtl
;r4=i;
;r5=j
;r6=PF0
;r7=PF1
;r8=PF2
;r9=PF3
;r10=PF4
;r14 = iNumLoops
;r12 = tmp

; stack usage:
IBB_10_OffsetRegSaving        EQU	32
IBB_10_Offset_iYFrac          EQU	IBB_10_OffsetRegSaving + 0
IBB_10_Offset_iRndCtrl        EQU	IBB_10_OffsetRegSaving + 4
IBB_10_Offset_b1MV            EQU	IBB_10_OffsetRegSaving + 8

    stmdb     sp!, {r4 - r10, r14}
    FRAME_PROFILE_COUNT

 ;   I32_WMV iNumLoops = 8<<b1MV;
 ;   iRndCtrl = 8 - ( iRndCtrl&0xff);
 
	pld		[r0]
	
	ldr		r3 , [sp, #IBB_10_Offset_iRndCtrl]
	ldr		r12, [sp, #IBB_10_Offset_b1MV]
	mov		r14, #8
	and		r3, r3, #0xff
	rsb		r3, r3, #8			;iRndCtrl
	mov		r14, r14, lsl r12
	mov		r4, r14	
	
IBB_10_OuterLoop
   
 ;   for (i = 0; i < iNumLoops; i++) 
 ;   {
 ;       const U8_WMV  *pT ;
	;	I16_WMV PF0, PF1, PF2, PF3, PF4;

    ;    PF0 = pSrc[0];

	pld		[r0, r1]

	mov		r5, r14
	ldrb	r6, [r0]
IBB_10_InnerLoop

    ;    for (j = 0; j < iNumLoops; j += 4) {

     ;       pT = pSrc + j;
     ;       PF1 = pT[1];
     ;       PF2 = pT[2];
     ;       PF3 = pT[3];
     ;       PF4 = pT[4];
     
	ldrb	r7, [r0, #1]
	ldrb	r8, [r0, #2]
	ldrb	r9, [r0, #3]
	ldrb	r10, [r0, #4]!

      ;      k0 = (((PF0 + PF1 ) << 3) + iRndCtrl) >> 4;
      ;      k1 = (((PF1 + PF2 ) << 3) + iRndCtrl) >> 4;
      ;      *(I16_WMV *)(pDst + j) = (U8_WMV) k0;
      ;      *(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;
      
    add		r6, r6, r7
    add		r7, r7, r8
    add		r6, r3, r6, lsl #3
    add		r7, r3, r7, lsl #3
    mov		r6, r6, asr #4
    mov		r7, r7, asr #4
    strh	r7, [r2, #20]

      ;      k0 = (((PF2 + PF3 ) << 3) + iRndCtrl) >> 4;
      ;      k1 = (((PF3 + PF4 ) << 3) + iRndCtrl) >> 4;
      ;      *(I16_WMV *)(pDst + j + 2) = (U8_WMV) k0;
      ;      *(I16_WMV *)(pDst + j + 22) = (U8_WMV) k1;

    add		r8, r8, r9
    add		r9, r9, r10
    add		r8, r3, r8, lsl #3
    add		r9, r3, r9, lsl #3
    mov		r8, r8, asr #4
    mov		r9, r9, asr #4
    strh	r8, [r2, #2]
    strh	r9, [r2, #22]
    strh	r6, [r2], #4

		;	PF0 = PF4;		
    mov		r6, r10
    
	subs	r5, r5, #4
	bne		IBB_10_InnerLoop
       ; }
        
      ;  pSrc += iSrcStride;
      ;  pDst += 40;

	sub		r0, r0, r14
	sub		r2, r2, r14
	add		r0, r0, r1
	add		r2, r2, #40
	
	subs	r4, r4, #1
	bne		IBB_10_OuterLoop
   ; }
    
	ldmia     sp!, {r4 - r10, pc}
	
	WMV_ENTRY_END	;g_InterpolateBlockBilinear_SSIMD_10

    
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;Void_WMV  g_InterpolateBlockBilinear_SSIMD_01 (const U8_WMV *pSrc, 
;                                           I32_WMV iSrcStride, 
;                                           U8_WMV *pDst, 
;                                           I32_WMV iXFrac, 
;                                           I32_WMV iYFrac, 
;                                           I32_WMV iRndCtrl, 
;                                           Bool_WMV b1MV)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA  |.text|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlockBilinear_SSIMD_01_ARMV4

;r5=j
;r6=PF0
;r7=PF1
;r8=PF2
;r9=PF3
;r10, r11, r12 = tmp

; stack usage:
IBB_01_OffsetRegSaving        EQU	36
IBB_01_Offset_iYFrac          EQU	IBB_01_OffsetRegSaving + 0
IBB_01_Offset_iRndCtrl        EQU	IBB_01_OffsetRegSaving + 4
IBB_01_Offset_b1MV            EQU	IBB_01_OffsetRegSaving + 8

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

  ;  I32_WMV iNumLoops = 8<<b1MV;    
  ;  iRndCtrl = 8 - ( iRndCtrl&0xff);

	pld		[r0]
	pld		[r0, r1]
	
	ldr		r3 , [sp, #IBB_01_Offset_iRndCtrl]
	ldr		r12, [sp, #IBB_01_Offset_b1MV]
	mov		r14, #8
	and		r3, r3, #0xff
	rsb		r3, r3, #8			;iRndCtrl
	mov		r14, r14, lsl r12
	mov		r4, r14	
	
IBB_01_OuterLoop
  ;  for (i = 0; i < iNumLoops; i++) 
  ;  {

	mov		r5, r14
IBB_01_InnerLoop

   ;     for (j = 0; j < iNumLoops; j += 4) {
		;	 pT = pSrc + j;
        ;    PF0 = pT[0] + pT[iSrcStride+0];
        ;    PF1 = pT[1] + pT[iSrcStride+1];
        ;    PF2 = pT[2] + pT[iSrcStride+2];
        ;    PF3 = pT[3] + pT[iSrcStride+3];
        
    add		r12, r0, r1
    ldrb	r8 , [r0, #2]
    ldrb	r9 , [r12, #2]
    ldrb	r10, [r12]
    ldrb	r7 , [r0, #1]
    ldrb	r11, [r12, #1]
    add		r8 , r8, r9
    ldrb	r9 , [r0, #3]
    ldrb	r12, [r12, #3]
    ldrb	r6 , [r0], #4

         ;   k0 = ((PF0 << 3) + iRndCtrl) >> 4;
         ;   k1 = ((PF1 << 3) + iRndCtrl) >> 4;
         ;   *(I16_WMV *)(pDst + j) = (U8_WMV) k0;
         ;   *(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;

         ;   k0 = ((PF2 << 3) + iRndCtrl) >> 4;
         ;   k1 = ((PF3 << 3) + iRndCtrl) >> 4;
         ;   *(I16_WMV *)(pDst + j + 2) = (U8_WMV) k0;
         ;   *(I16_WMV *)(pDst + j + 22) = (U8_WMV) k1;
         
    add		r8, r3, r8, lsl #3
    add		r7, r7, r11
    add		r9, r9, r12
    add		r7, r3, r7, lsl #3
    add		r6, r6, r10
    add		r9, r3, r9, lsl #3
    add		r6, r3, r6, lsl #3
    
    mov		r7, r7, asr #4
    mov		r6, r6, asr #4
    mov		r8, r8, asr #4
    mov		r9, r9, asr #4
    strh	r7, [r2, #20]
    strh	r8, [r2, #2]
    strh	r9, [r2, #22]
    strh	r6, [r2], #4

	subs	r5, r5, #4
	bne		IBB_01_InnerLoop
  ;      }


 ;   pSrc += iSrcStride;
 ;   pDst += 40;
 
	sub		r0, r0, r14
	sub		r2, r2, r14
	add		r0, r0, r1
	add		r2, r2, #40
	
	subs	r4, r4, #1

	pld		[r0, r1]
	
	bne		IBB_01_OuterLoop
	
;	}
	
    ldmia     sp!, {r4 - r11, pc}
    
    WMV_ENTRY_END	;g_InterpolateBlockBilinear_SSIMD_01


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;U32_WMV g_NewVertFilterX(const U8_WMV  *pSrc,
;                             const I32_WMV iSrcStride, 
;                             U8_WMV * pDst, 
;                             const I32_WMV iShift, 
;                             const I32_WMV iRound32, 
;                             const I8_WMV * const pV, 
;                             I32_WMV iNumHorzLoop, 
;                             const U32_WMV uiMask,
;                             Bool_WMV b1MV
;                             )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    AREA  |.text|, CODE
	    
    WMV_LEAF_ENTRY g_NewVertFilterX_ARMV4

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

;pSrc = r0
;iSrcStride = r1
;pDst = r2
;iShift = r3
;m=r4
;o1=r5
;o2=r6
;o3=r7
;v0 = r8
;v1 = r12;
;v3 = r9
;overflow = r10
;t0 = r11;

;r14 = tmp;

;; stack usage:
VFX_StackSize              EQU 0xc
VFX_OffsetRegSaving        EQU 0x24

VFX_Stack_iNumHorzLoop   EQU 0
VFX_Stack_iNumInnerLoop  EQU 4
VFX_Stack_v2             EQU 0x8
;VFX_Stack_pDstUpdate     EQU 0xc
;VFX_Stack_pSrcUpdate     EQU 0x10


VFX_Offset_iRound32        EQU VFX_StackSize + VFX_OffsetRegSaving + 0
VFX_Offset_pV              EQU VFX_StackSize + VFX_OffsetRegSaving + 4
VFX_Offset_iNumHorzLoop    EQU VFX_StackSize + VFX_OffsetRegSaving + 8
VFX_Offset_uiMask          EQU VFX_StackSize + VFX_OffsetRegSaving + 0xc
VFX_Offset_b1MV            EQU VFX_StackSize + VFX_OffsetRegSaving + 0x10
VFX_Offset_pTbl	             EQU VFX_StackSize + VFX_OffsetRegSaving + 0x14

;  for(k = 0; k < (iNumHorzLoop<<1); k++)
    
    ldr r8, [sp, #VFX_Offset_iNumHorzLoop - VFX_StackSize ]
    ldr r4, [sp, #VFX_Offset_b1MV - VFX_StackSize]
    ldr r5, [sp, #VFX_Offset_pV - VFX_StackSize ]
    mov  r6, #1    
	mov r8, r8, lsl #3



    ; I32_WMV iNumInnerLoop = 1<<(3+b1MV);
    add  r4, r4, #3  ; 3+b1MV
    str r8, [sp, #-VFX_StackSize]!    
    mov  r6, r6, lsl r4 
   ;sub  r11, r6, #1                                    ; adjust iNumInnerLoop since it will be in iNumInnerLoop,,iShift
    
    sub  r3, r3, #0x10000
    mov r11, r6, lsl #16
    str  r11, [sp, #VFX_Stack_iNumInnerLoop]

    ldrsb r12, [ r5, #1] ;relo11

    ;v1;
    ;v2 = pV[2];
    ldrsb r7, [ r5, #2]
    
    ;v0, v3
    ldrsb r8, [ r5, #0]
    ldrsb r9, [ r5, #3]
    str r7, [sp, #VFX_Stack_v2] ;relo12

    ;U32_WMV overflow = 0;
    mov r10, #0

     ; for(k = 0; k < (iNumHorzLoop<<1); k++)
     ;{

g_NewVertFilterX_outloop
        
            IF _XSC_=1
                PLD [r0, #32]
            ENDIF

            ldrb r14, [r0, #2]                          ;o1 = pSrc[0] | (pSrc[2]<<16);
            ldrb r5,  [r0], +r1                         ;pSrc += iSrcStride;
            
            IF _XSC_=1
                PLD [r0, #32]
            ENDIF

            ldrb r11, [r0, #2]                          ;o2 = pSrc[0] | (pSrc[2]<<16);
            ldrb r6,  [r0], +r1                         ;pSrc += iSrcStride;
            
            orr r5, r5, r14, lsl #16
            IF _XSC_=1
                PLD [r0, #32]
            ENDIF
           
            ldrb r14, [r0, #2]                          ;o3 = pSrc[0] | (pSrc[2]<<16);
            ldrb r7,  [r0], +r1                         ;pSrc += iSrcStride;
            orr r6, r6, r11, lsl #16                     
            
            orr r7, r7, r14, lsl #16

            ldr r14, [sp, #VFX_Stack_iNumInnerLoop]
            ldr r4,  [sp, #VFX_Offset_iRound32]
	        add  r3, r3, r14

           ; for(m = 0; m < iNumInnerLoop; m++)
           ; {
g_NewVertFilterX_innerloop
                
                IF _XSC_=1
                    PLD [r0, #32]
                ENDIF

                mla  r11, r5, r8, r4                    ; t0 = o1*v0 + iRound32;
                ldr  r14, [sp, #VFX_Stack_v2]           ; v2
                
                mla  r11, r6, r12, r11                  ; t0 += o2*v1;
			    ldrb r5, [r0, #2]         ;relo100             ; o1 = pSrc[0] | (pSrc[2]<<16);
               
                mla  r11, r7, r14, r11                  ; t0 += o3*v2;
                ldrb r14,  [r0], +r1                     ; pSrc += iSrcStride;
                
                IF _XSC_=1 
                    PLD [r0, #32]
                ENDIF            

                orr  r5, r14, r5, lsl #16
                mla  r11, r5, r9, r11                   ; t0 += o1*v3;
            
                ldr  r14, [sp, #VFX_Offset_uiMask]      ; uiMask
                
                orr  r10, r10, r11                      ; overflow |= t0;

			    and  r11, r14, r11, lsr r3              ; t0 =(t0>>iShift)&uiMask

				mla  r14, r6, r8, r4                    ; t0 = o2*v0 + iRound32;
                str  r11, [r2], #+40                    ; *(U32_WMV *)pDst = t0;
                                                        ; pDst += 40;
              ;  bpl  g_NewVertFilterX_innerloop

								
                ldr  r11, [sp, #VFX_Stack_v2]           ; v2
                
                mla  r14, r7, r12, r14                  ; t0 += o3*v1;

                mov  r6, r5                             ; o2 = o1;
               
                mla  r14, r5, r11, r14                  ; t0 += o1*v2;
                
                ldrb r11, [r0, #2]                      ; o3 = pSrc[0] | (pSrc[2]<<16);
                
				mov  r5, r7                             ; o1 = o3;
				
				ldrb r7,  [r0], +r1                     ; pSrc += iSrcStride;

                subs r3, r3, #0x20000                   ; m++
                             
                orr  r7, r7, r11, lsl #16

                mla  r14, r7, r9, r14                   ; t0 += o3*v3;
            
                ldr  r11, [sp, #VFX_Offset_uiMask]      ; uiMask
                
                orr  r10, r10, r14                      ; overflow |= t0; 

		        and r14, r11, r14, lsr r3               ; t0 =(t0>>iShift)&uiMask

                str  r14, [r2], #+40                    ; *(U32_WMV *)pDst = t0;
                                                        ; pDst += 40;
                bpl  g_NewVertFilterX_innerloop
           ; }

;/*
;            pSrc += pTbl[0] - 3*iSrcStride - (iSrcStride<<(3+b1MV)) ;
;            pDst += pTbl[1] - (40<<(3+b1MV));
;            pTbl += 2;
;*/

        ;if(k&1)
        ldr  r14, [sp, #VFX_Stack_iNumHorzLoop]
		ldr  r11, [sp, #VFX_Offset_pTbl]
		and r5, r14, #4
		ldrh  r6, [r11, r5] !
		ldrh  r7, [r11, #2] 

		subs r14, r14, #4
		sub r0, r0, r6
		sub  r2, r2, r7
		
        str  r14, [sp, #VFX_Stack_iNumHorzLoop]
        bgt  g_NewVertFilterX_outloop
    ;}

     ;return overflow;

     add  sp, sp, #VFX_StackSize
     mov  r0, r10
     ldmia     sp!, {r4 - r11, pc}
;}

     WMV_ENTRY_END
     ENDP  ;  g_NewVertFilterX


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;    U32_WMV g_NewHorzFilterX(U8_WMV *pF, 
;                              const I32_WMV iShift, 
;                              const I32_WMV iRound2_32, 
;                              const I8_WMV * const pH, 
;                              Bool_WMV b1MV)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    
    
    AREA  |.text|, CODE
    WMV_LEAF_ENTRY g_NewHorzFilterX_ARMV4
    
;r0 = pF
;r1 = iShift
;r2 = iRound2_32
;r3 = i
;r4 = h0
;r5 = h1
;r6 = h2
;r7 = h3
;r8 = o0
;r9 = o1
;r10 = t0;
;r11 = t1
;r12 = overflow;
;r14 = mask

;; stack usage:
HFX_StackSize         EQU 0x10
HFX_OffsetRegSaving   EQU 0x24

HFX_Offset_b1MV       EQU HFX_StackSize + HFX_OffsetRegSaving + 0

HFX_Stack_iNumLoops   EQU 0
HFX_Stack_j           EQU 4
HFX_Stack_pFUpdate    EQU 8
HFX_Stack_iRound2_32  EQU 12

    ;I32_WMV j, i;
    ;U32_WMV overflow = 0;
    ; register U32_WMV t0, t1; //, t2, t3;
    ;I32_WMV  iNumLoops = 1<<(3+b1MV);
    
    ;const I16_WMV h0 = pH[0];
    ;const I16_WMV h1 = pH[1];
    ;const I16_WMV h2 = pH[2];
    ;const I16_WMV h3 = pH[3];

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

   
    ldr r11, [sp, #HFX_Offset_b1MV - HFX_StackSize]
   
    pld	 [r0] 
       
    mov r10, #8    
	str r2, [sp, #HFX_Stack_iRound2_32 - HFX_StackSize] 
	    
    mov r10, r10, lsl r11
    str r10, [sp, #HFX_Stack_j-HFX_StackSize]
    sub r10, r10, #1
    str r10, [sp, #HFX_Stack_iNumLoops-HFX_StackSize]!

    mov r9, #320
    ldrsb r4, [r3, #0]
    mov r9, r9, lsl r11
    sub r9, r9, #4
    str r9, [sp, #HFX_Stack_pFUpdate]

    ldrsb r5, [r3, #1]
    ldrsb r6, [r3, #2]
    ldrsb r7, [r3, #3]

    mov r14, #0xff
    mov r12, #0
    orr r14, r14, r14, lsl #16


HFX_Outerloop
    ;for (j = 0; j < iNumLoops; j += 4) 
    ;{

    ldr  r10, [sp, #HFX_Stack_iNumLoops]
    and  r1, r1, #0xff                          ; mask iShift
    orr  r1, r1, r10, lsl #16

HFX_InnerLoop
    
    ;  for(i = 0; i < iNumLoops; i++)
    ;  {
    ;        register I32_WMV o0;
    ;        register I32_WMV o1;
           
    ldr  r10, [r0]                              ; t0 = *(I32_WMV *)pF;
	ldr  r2, [sp, #HFX_Stack_iRound2_32]    
    ldr  r11, [r0, #20]                         ; t1 = *(I32_WMV *)(pF + 20); 
    ldr  r3,  [r0, #4]                          ; t1 = (*(U32_WMV *)(pF+4));
    mla  r8,  r10, r4, r2                       ; o0 = t0 * h0 + iRound2_32;

    mla  r9,  r11, r4, r2                       ; o1 = t1 * h0 + iRound2_32;
    mla  r8,  r11, r5, r8                       ; o0 += t1 * h1;

    mov  r10, r10, lsr #16                       ; t0 = t0>>16;
    orr  r11, r10, r3, lsl #16                   ; t1 = t0 | (t1<<16);
    ldrh r10, [r0, #22]                         ; t0 = *(U16_WMV *)(pF + 20 + 2);  
    ldr  r2, [r0, #24]         ; t1 = (*(U32_WMV *)(pF+20+4));
    
    mla  r8,  r11, r6, r8                       ; o0 += t1 * h2;
    mla  r9,  r11, r5, r9                       ; o1 += t1 * h1;
                  
    orr  r11, r10, r2, lsl #16                  ; t1 = t0 | (t1<<16);
    subs r1,  r1, #0x10000
    
    ; ldr r10, [r0, #4]                         ; t0 = *(I32_WMV *)(pF+4);  still in r3
    
    mla  r8,  r11, r7, r8                       ; o0 += t1 * h3;
    
    pld	 [r0, #40]
    
    mla  r9,  r11, r6, r9                       ; o1 += t1 * h2;
    orr  r12, r12, r8                           ; overflow |= o0;
    mla  r9,  r3,  r7, r9                       ; o1 += t0 * h3;

    and  r8,  r14, r8, lsr r1                       ; o0 >>= iShift; o0 &= 0x00ff00ff;
	orr  r12, r12, r9                           ; overflow |= o1;
    and  r9,  r14, r9, lsr r1                       ; o1 >>= iShift; o1 &= 0x00ff00ff;

    str  r9,  [r0, #20]                         ;  *(U32_WMV *)(pF+20) = o1;
    str  r8,  [r0], #40                         ;  *(U32_WMV *)pF = o0;
                                                ;  pF += 40;
    bpl  HFX_InnerLoop

    ;    }

    ldr r10, [sp, #HFX_Stack_j]
    ldr r11, [sp, #HFX_Stack_pFUpdate]

    ;    pF += -(40<<(3+b1MV)) + 4;
    ;}
    subs r10, r10, #4
    str r10, [sp, #HFX_Stack_j]
    sub  r0, r0, r11
    
    pld	 [r0]
    
    bgt  HFX_Outerloop

    ;return overflow;

    add  sp, sp, #HFX_StackSize
    mov r0, r12
    ldmia     sp!, {r4 - r11, pc}

;}
	
    WMV_ENTRY_END	;g_NewHorzFilterX

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Void_WMV g_InterpolateBlock_00_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, 
;										I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV) // iXFrac == 0; iYFrac == 0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA  |.text|, CODE
    WMV_LEAF_ENTRY g_InterpolateBlock_00_SSIMD_ARMV4

;r0 = pSrc
;r1 = iSrcStride
;r2 = pDst
;r7 = i
;r14, r4-r7 = tmp;

    stmdb   sp!, {r4 - r11, r14}
  
    ldr		r3, [ sp, #44]
    tst		r0, #3
    beq		IB00_SRC_ALIGN4
    
	cmp		r3, #0
	ldreq	r3, =IB00_Loop8
	moveq 	r12, #8
	ldrne	r3, =IB00_Loop16
	movne	r12, #16
	mov		pc, r3
    
IB00_Loop16

     ;       *(I32_WMV *)(pDst + 12)       = pSrc[12   ]|(pSrc[12+2]<<16);
     ;       *(I32_WMV *)(pDst + 12 + 20)  = pSrc[12+1 ]|(pSrc[12+3]<<16);

     ldrb r14, [r0, #12]
     ldrb r4,  [r0, #2+12];
     ldrb r5,  [r0, #1+12]
     ldrb r6,  [r0, #3+12];
     ldrb r7,  [r0, #8]

     orr  r4, r14, r4, lsl #16
     str  r4,  [r2, #0 +12 ]
     
     orr  r6, r5, r6, lsl #16

     ;       *(I32_WMV *)(pDst + 8)       = pSrc[8   ]|(pSrc[8+2]<<16);
     ;       *(I32_WMV *)(pDst + 8 + 20)  = pSrc[8+1 ] |(pSrc[8+3]<<16);

     ldrb r4,  [r0, #2+8];
     str  r6,  [r2, #20 +12]
     ldrb r6,  [r0, #3+8];
     ldrb r5,  [r0, #1+8]

     orr  r4, r7, r4, lsl #16
     str  r4,  [r2, #0 +8 ]
     
     orr  r6, r5, r6, lsl #16
     str  r6,  [r2, #20 +8]

IB00_Loop8

     ;       *(I32_WMV *)(pDst + 4)       = pSrc[4   ]|(pSrc[4+2]<<16);
     ;       *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+1 ]|(pSrc[4+3]<<16);
     ldrb r14, [r0, #4]
     ldrb r4,  [r0, #2+4];
     ldrb r5,  [r0, #1+4]
     ldrb r6,  [r0, #3+4];
     
     subs  r12,r12, #1

     orr  r4, r14, r4, lsl #16
     str  r4,  [r2, #0 + 4 ]
     
     orr  r6, r5, r6, lsl #16
     str  r6,  [r2, #20 + 4]

     ;       *(I32_WMV *)(pDst + 0)      = pSrc[0   ]|(pSrc[0+2]<<16);
     ;       *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+1 ]|(pSrc[0+3]<<16);
     
     ldrb r5,  [r0, #1]
     ldrb r6,  [r0, #3];
     ldrb r4,  [r0, #2];

     IF _XSC_=1
          PLD  [r0, #32]
     ENDIF

     ldrb r14, [r0], r1
     orr  r6, r5, r6, lsl #16
     str  r6,  [r2, #20]
     
     orr  r4, r14, r4, lsl #16
     str  r4,  [r2], #40

     movgt pc, r3
     
	mov  r0, #0
	ldmia     sp!, {r4 - r11, pc}

IB00_SRC_ALIGN4		;src address is 4 byte alignment

	pld		[r0, #32]
	cmp		r3, #0
	mov		r14, #0xff
	moveq 	r12, #4
	movne	r12, #16
	orr		r14, r14, r14, lsl #16	;0x00ff00ff
	bne		IB00_Loop16_SRC_ALIGN4
     
IB00_Loop8_SRC_ALIGN4
     ;   for (i = 0; i < 4; i++) 
     ;   {
     ;       *(I32_WMV *)(pDst + 0)      = pSrc[0   ]|(pSrc[0+2]<<16);
     ;       *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+1 ]|(pSrc[0+3]<<16);
     ;       *(I32_WMV *)(pDst + 4)       = pSrc[4   ]|(pSrc[4+2]<<16);
     ;       *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+1 ]|(pSrc[4+3]<<16);
     ;       pSrc += iSrcStride;
     ;       pDst += 40;
     ;       *(I32_WMV *)(pDst + 0)      = pSrc[0   ]|(pSrc[0+2]<<16);
     ;       *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+1 ]|(pSrc[0+3]<<16);
     ;       *(I32_WMV *)(pDst + 4)       = pSrc[4   ]|(pSrc[4+2]<<16);
     ;       *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+1 ]|(pSrc[4+3]<<16);
     ;       pSrc += iSrcStride;
     ;       pDst += 40;
     
	ldmia	r0, {r4, r5}
	add		r0, r0, r1
	add		r3, r2, #20     
	pld		[r0, #32]
	ldmia	r0, {r6, r7}
	add		r0, r0, r1    

	and		r8 , r14, r4, lsr #8
	pld		[r0, #32]
	and		r4 , r14, r4
	and		r9 , r14, r5, lsr #8
	and		r5 , r14, r5
	stmia	r2, {r4, r5}    
	add		r2, r2, #40     
	stmia	r3, {r8, r9}    

	and		r8 , r14, r6, lsr #8
	and		r4 , r14, r6
	and		r9 , r14, r7, lsr #8
	and		r5 , r14, r7
	add		r3, r2, #20     
	stmia	r2, {r4, r5}    
	add		r2, r2, #40     
	stmia	r3, {r8, r9}    

	subs	r12, r12, #1
	bne	IB00_Loop8_SRC_ALIGN4
     ;	}
     
	mov		r0, #0
	ldmia   sp!, {r4 - r11, pc}

IB00_Loop16_SRC_ALIGN4

	pld		[r0, r1]
	pld		[r0, r1, lsl #1]
IB00_Loop16_SRC_ALIGN4_Start
     ;   for (i = 0; i < 16; i++) 
     ;   {

      ;      *(I32_WMV *)(pDst + 0)       = pSrc[0   ]|(pSrc[0+2]<<16);
      ;      *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+1 ]|(pSrc[0+3]<<16);
      ;      *(I32_WMV *)(pDst + 4)       = pSrc[4   ]|(pSrc[4+2]<<16);
      ;      *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+1 ]|(pSrc[4+3]<<16);
      ;      *(I32_WMV *)(pDst + 8)       = pSrc[8   ]|(pSrc[8+2]<<16);
      ;      *(I32_WMV *)(pDst + 8 + 20)  = pSrc[8+1 ]|(pSrc[8+3]<<16);
      ;      *(I32_WMV *)(pDst + 12)      = pSrc[12  ]|(pSrc[12+2]<<16);
      ;      *(I32_WMV *)(pDst + 12 + 20) = pSrc[12+1]|(pSrc[12+3]<<16);
     ;       pSrc += iSrcStride;
     ;       pDst += 40;
     
	ldmia	r0, {r4-r7}
	add		r0, r0, r1 
	subs	r12, r12, #1
	pld		[r0, r1, lsl #1]

	and		r8 , r14, r4, lsr #8
	and		r9 , r14, r5, lsr #8
	and		r10, r14, r6, lsr #8
	and		r11, r14, r7, lsr #8
	and		r4 , r14, r4
	and		r5 , r14, r5
	and		r6 , r14, r6
	and		r7 , r14, r7

	stmia	r2, {r4-r7}    
	add		r3, r2, #20     
	add		r2, r2, #40     
	stmia	r3, {r8-r11} 

	bne	IB00_Loop16_SRC_ALIGN4_Start
     ;	}
        
    mov  r0, #0
    ldmia     sp!, {r4 - r11, pc}
    
    WMV_ENTRY_END	;g_InterpolateBlock_00_SSIMD

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Void_WMV g_NewVertFilter0Long(const U8_WMV  *pSrc,   
;                              I32_WMV iSrcStride, 
;                              U8_WMV * pDst, 
;                              Bool_WMV b1MV)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
;|g_NewVertFilter0Long| PROC
    WMV_LEAF_ENTRY g_NewVertFilter0LongNoGlblTbl_ARMV4

;r0 = pSrc
;r1 = iSrcStride
;r2 = pDst
;r7 = i
;r14, r4-r6 = tmp;

    FRAME_PROFILE_COUNT
    stmdb     sp!, {r4 - r7, r14}
	
	cmp		r3, #0
	ldreq	r3, =NVFL_Loop8
	moveq	r12, #8
	ldrne	r3, =NVFL_Loop16
	movne	r12, #16
	mov		pc, r3

NVFL_Loop16
     ;   for(i = 0; i < 8; i++)
     ;   {

     ;*(I32_WMV *)(pDst + 16)       = pSrc[16   ]|(pSrc[16+2]<<16);
     ;*(I32_WMV *)(pDst + 16 + 20)  = pSrc[16+1 ] |(pSrc[16+3]<<16);
     ldrb r14, [r0, #16]
     ldrb r4,  [r0, #2+16];
     ldrb r5,  [r0, #1+16]
     ldrb r6,  [r0, #3+16];
     ldrb r7, [r0, #12]

     orr  r4, r14, r4, lsl #16
     str  r4,  [r2, #0 +16 ]

     orr  r6, r5, r6, lsl #16

     ;       *(I32_WMV *)(pDst + 12)       = pSrc[12   ]|(pSrc[12+2]<<16);
     ;       *(I32_WMV *)(pDst + 12 + 20)  = pSrc[12+1 ]|(pSrc[12+3]<<16);

     ldrb r4,  [r0, #2+12];
     str  r6,  [r2, #20 +16]
     ldrb r6,  [r0, #3+12];
     ldrb r5,  [r0, #1+12]

     orr  r4, r7, r4, lsl #16
     str  r4,  [r2, #0 +12 ]
     
     orr  r6, r5, r6, lsl #16
     str  r6,  [r2, #20 +12]

NVFL_Loop8

     ;       *(I32_WMV *)(pDst + 8)       = pSrc[8   ]|(pSrc[8+2]<<16);
     ;       *(I32_WMV *)(pDst + 8 + 20)  = pSrc[8+1 ] |(pSrc[8+3]<<16);

     ldrb r14, [r0, #8]
     ldrb r4,  [r0, #2+8];
     ldrb r5,  [r0, #1+8]
     ldrb r6,  [r0, #3+8];
     
     subs  r12,r12, #1

     orr  r4, r14, r4, lsl #16
     str  r4,  [r2, #0 +8 ]
     
     orr  r6, r5, r6, lsl #16

     ;       *(I32_WMV *)(pDst + 4)       = pSrc[4   ]|(pSrc[4+2]<<16);
     ;       *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+1 ]|(pSrc[4+3]<<16);
     ldrb r14, [r0, #4]
     ldrb r4,  [r0, #2+4];
     str  r6,  [r2, #20 +8]
     ldrb r6,  [r0, #3+4];
     ldrb r5,  [r0, #1+4]
     
     orr  r4, r14, r4, lsl #16
     str  r4,  [r2, #0 + 4 ]
     
     orr  r6, r5, r6, lsl #16
     str  r6,  [r2, #20 + 4]

     ;       *(I32_WMV *)(pDst + 0)      = pSrc[0   ]|(pSrc[0+2]<<16);
     ;       *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+1 ]|(pSrc[0+3]<<16);
     
     ldrb r5,  [r0, #1]
     ldrb r6,  [r0, #3];
     ldrb r4,  [r0, #2];

     IF _XSC_=1
          PLD  [r0, #32]
     ENDIF

     ldrb r14, [r0], r1
     orr  r6, r5, r6, lsl #16
     str  r6,  [r2, #20]
     
     orr  r4, r14, r4, lsl #16
     str  r4,  [r2], #40

     movgt pc, r3
     
    mov  r0, #0
    ldmia     sp!, {r4 - r7, pc}

    WMV_ENTRY_END	;g_NewVertFilter0LongNoGlblTbl


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Void_WMV g_AddNull_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch)
;{
;    I32_WMV iy;
;    U32_WMV u0,u1,u2,u3, y0,y1;  
;    for (iy = 0; iy < 8; iy++) 
;    {     
;        u0 = pRef[0];
;        u1 = pRef[0 + 5];
;        u2 = pRef[1];
;        u3 = pRef[1 + 5];
;
;        pRef += 10;
;        
;        y0 = (u0) | ((u1) << 8);
;        y1 = (u2) | ((u3) << 8);
;        
;        *(U32_WMV *)ppxlcDst = y0;
;        *(U32_WMV *)(ppxlcDst + 4)= y1;
;        ppxlcDst += iPitch;
;    }
;}

    AREA  |.text|, CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    WMV_LEAF_ENTRY g_AddNull_SSIMD_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
;r0 = ppxlcDst
;r1 = pRef
;r2 = iPitch
;r3 = iy
;r4 = u0
;r5 = u1
;r12 = u2
;r14 = u3

    stmdb     sp!, {r4 - r5, r14}
    FRAME_PROFILE_COUNT

    ldr  r3,[r1, #4]

    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2
 
    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2

    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2
 
    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2        
        
    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2
 
    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2

    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2
 
    ldr  r14,[r1, #24]
    ldr  r5, [r1, #20]
    ldr  r4, [r1], #40
    orr r12,r3,r14, lsl #8
	ldr  r3,[r1, #4]
    orr r4, r4, r5,  lsl #8
    str r12,[r0, #4]
    str r4, [r0], r2        

    ldmia     sp!, {r4 - r5, pc}

;}

    WMV_ENTRY_END  ;  g_AddNull_SSIMD


    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;Void_WMV g_AddNullB_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV iOffset, I32_WMV iPitch)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA  |.text|, CODE
    WMV_LEAF_ENTRY g_AddNullB_SSIMD_ARMV4
    
;r0 = ppxlcDst
;r1 = pRef0
;r2 = pRef1
;r3 = iy
;r4 = u0
;r5 = u1
;r12 = u2
;r14 = u3
;r6 = v0
;r7 = v1
;r8 = v2
;r9 = v3
;r10 = 0x00010001
;r11 = iPitch

ANBE_OffsetRegSaving   EQU       0x24
ANBE_Offset_iPitch          EQU       ANBE_OffsetRegSaving + 0

    ;I32_WMV iy;
    ;U32_WMV u0,u1,u2,u3, y0,y1;

    stmdb     sp!, {r4 - r11, r14}
    FRAME_PROFILE_COUNT

    ldr r11, [sp, #ANBE_Offset_iPitch]

    ;pRef0 += iOffset;
    ;pRef1 += iOffset;

    mov r10, #1
    add r1, r1, r3, lsl #2
    orr   r10, r10, r10, lsl #16
    add r2, r2, r3, lsl #2
    mov r3, #8

ANBE_Loop
   ;for (iy = 0; iy < BLOCK_SIZE; iy++) 
    

        ;u0 = pRef[0];
        ;u1 = pRef[0 + 5];
        ;u2 = pRef[1];
        ;u3 = pRef[1 + 5];
        ;pRef0 += 10;

        ;v0 = pRef1[0];
        ;v1 = pRef1[0 + 5];
        ;v2 = pRef1[1];
        ;v3 = pRef1[1 + 5];
        ;pRef1 += 10;

        ldr  r12,[r1, #4]
        ldr  r8,  [r2,  #4]
        ldr  r14,[r1, #24]
        ldr  r9,  [r2,   #24]

        subs r3, r3, #1

        ldr  r5, [r1, #20]
        ldr  r7,[r2,  #20]
        ldr  r4, [r1], #40
        ldr  r6, [r2], #40

        ;u2 = (u2 + v2 + 0x00010001) >>1;
        ;u3 = (u3 + v3 + 0x00010001) >>1;
        ;u2 = u2 & ~0x8000;
        ;u3 = u3 & ~0x8000;

        add r12, r12, r8
        add r12, r12, r10
        bic  r12, r12, #0x10000
        add r14, r14, r9
        mov r12, r12, lsr #1

        add r14, r14, r10
        bic  r14, r14, #0x10000
        add r5, r5, r7
        mov r14, r14, lsr #1

        add r5, r5, r10
        bic  r5, r5, #0x10000
        add r4, r4, r6
        mov r5, r5, lsr #1

        add r4, r4, r10
        bic  r4, r4, #0x10000
        orr r12,r12,r14, lsl #8
        mov r4, r4, lsr #1
        
        ;u2 = (u2) | ((u3) << 8);
        ;u0 = (u0) | ((u1) << 8);
        
        orr r4, r4, r5,  lsl #8

        ;*(U32_WMV *)(ppxlcDst + 4)= u2;
        ;*(U32_WMV *)ppxlcDst = u0;
        ;ppxlcDst += iPitch;

        str r12,[r0, #4]
        str r4, [r0], r11
        
        bgt ANBE_Loop
        
    ldmia     sp!, {r4 - r11, pc}
    
    
;}
    WMV_ENTRY_END



    EXPORT end_interpolate_wmv9
end_interpolate_wmv9
        nop             ; establish location of end of previous function for cache analysis

    ENDIF ; WMV_OPT_MOTIONCOMP_ARM= 1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    IF WMV_OPT_INTENSITYCOMP_ARM=1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    AREA  |.text|, CODE
    WMV_LEAF_ENTRY IntensityComp_ARMV4

pSrc        RN  0
iFrameSize  RN  1
pLUT        RN  2
dst0        RN  3
dst1        RN  4
dst2        RN  5
dst3        RN  6
temp0       RN  7
temp1       RN  8
temp2       RN  9
temp3       RN  10

	stmdb     sp!, {r4 - r11, lr}

FadingLoop    
        
    ldmia   pSrc, {dst0 - dst3}
	ldrb    temp0, [pLUT, +dst0, lsr #24]
    ldrb    temp1, [pLUT, +dst1, lsr #24]
    ldrb    temp2, [pLUT, +dst2, lsr #24]
    ldrb    temp3, [pLUT, +dst3, lsr #24]
    add     dst0, temp0, dst0, lsl #8
    add     dst1, temp1, dst1, lsl #8
    add     dst2, temp2, dst2, lsl #8
    add     dst3, temp3, dst3, lsl #8
	ldrb    temp0, [pLUT, +dst0, lsr #24]
    ldrb    temp1, [pLUT, +dst1, lsr #24]
    ldrb    temp2, [pLUT, +dst2, lsr #24]
    ldrb    temp3, [pLUT, +dst3, lsr #24]
    add     dst0, temp0, dst0, lsl #8
    add     dst1, temp1, dst1, lsl #8
    IF _XSC_=1
        pld     [pSrc, #32]
    ENDIF
    add     dst2, temp2, dst2, lsl #8
    add     dst3, temp3, dst3, lsl #8
	ldrb    temp0, [pLUT, +dst0, lsr #24]
    ldrb    temp1, [pLUT, +dst1, lsr #24]
    ldrb    temp2, [pLUT, +dst2, lsr #24]
    ldrb    temp3, [pLUT, +dst3, lsr #24]
    add     dst0, temp0, dst0, lsl #8
    add     dst1, temp1, dst1, lsl #8
    add     dst2, temp2, dst2, lsl #8
    add     dst3, temp3, dst3, lsl #8
	ldrb    temp0, [pLUT, +dst0, lsr #24]
    ldrb    temp1, [pLUT, +dst1, lsr #24]
    ldrb    temp2, [pLUT, +dst2, lsr #24]
    ldrb    temp3, [pLUT, +dst3, lsr #24]
    add     dst0, temp0, dst0, lsl #8
    add     dst1, temp1, dst1, lsl #8
    add     dst2, temp2, dst2, lsl #8
    add     dst3, temp3, dst3, lsl #8
    stmia   pSrc!, {dst0 - dst3}

    subs    iFrameSize, iFrameSize, #1
    bne     FadingLoop

	ldmia   sp!, {r4 - r11, pc}
    WMV_ENTRY_END

    ENDIF ;WMV_OPT_INTENSITYCOMP_ARM



    END 
