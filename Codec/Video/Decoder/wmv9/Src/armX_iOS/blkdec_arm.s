@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************
    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 

	.if WMV_OPT_BLKDEC_ARM == 1
	
	.globl	_DecodeInverseInterBlockQuantizeEscCodeEMB2
	.globl  _DecodeInverseInterBlockQuantizeEscCodeEMB
@ 
    .globl  _BS_getBits2
    .globl  _getHuffman
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	 .text
	 .align 4
    @WMV_LEAF_ENTRY DecodeInverseInterBlockQuantizeEscCodeEMB
  .macro DecodeInverseInterBlockQuantizeEscCodeEMB
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@r0-r3=scratch
@r4=iDCTHorzFlags
@r5=uiCoefCounter
@r6=InterDCTTableInfo_Dec
@r7=pShortCut
@r8=bIsLastRun
@r14=uiRun
@r10=iLevel
@r11=pZigzagInv


    .if 0
@CDCTTableInfo_Dec offsets. See vodec.hpp

CDCTID_base			   equ 0x0
CDCTID_hufDCTACDec     equ 0x0
CDCTID_iTcoef_ESCAPE   equ 0x4
CDCTID_iStartIndxOfLastRunMinus1 equ 0x8
CDCTID_puiNotLastNumOfLevelAtRun equ 0xc
CDCTID_puiLastNumOfLevelAtRun    equ 0x10
CDCTID_puiNotLastNumOfRunAtLevel equ 0x14
CDCTID_puiLastNumOfRunAtLevel    equ 0x18
CDCTID_pcLevelAtIndx   equ 0x1c
CDCTID_puiRunAtIndx    equ 0x20
CDCTID_combined_levelrun  equ 0x24

@Huffman offsets, See huffman.hpp
H_m_decInfo   equ 0x1c
H_m_tableInfo equ 0x20
H_m_firstLength equ 0x24 @@ifdef NO_TABLE_INFO

@CE_DecodeP_ShortCut offsets, See vodec.hpp
tagEMB_DecodeP_ShortCut_base                       equ 0x0
tagEMB_DecodeP_ShortCut_pVideoDecoderObject        equ 0x0
tagEMB_DecodeP_ShortCut_m_rgiCoefRecon             equ 0x4
tagEMB_DecodeP_ShortCut_m_pbitstrmIn               equ 0x8
tagEMB_DecodeP_ShortCut_m_i2DoublePlusStepSize     equ 0xc
tagEMB_DecodeP_ShortCut_m_iDoubleStepSize          equ 0x10
tagEMB_DecodeP_ShortCut_m_iStepMinusStepIsEven     equ 0x14
tagEMB_DecodeP_ShortCut_XFormMode                  equ 0x18
tagEMB_DecodeP_ShortCut_m_iDCTHorzFlags            equ 0x1c

@Class CInputBitStream offsets
BS_OFFSET_BASE         equ 0x0 @ OPT_HUFFMAN_GET_CBSbase

@ offset relative to the base.

BS_OFFSET_m_pBuffer    equ 0x0 + BS_OFFSET_BASE
BS_OFFSET_m_cbBuflen   equ 0x4 + BS_OFFSET_BASE
BS_OFFSET_m_currBits   equ 0x8 + BS_OFFSET_BASE
BS_OFFSET_m_nextBits   equ 0xc + BS_OFFSET_BASE
BS_OFFSET_m_BNS_base   equ 0x10 + BS_OFFSET_BASE
BS_OFFSET_m_BNS_base16L   equ 0x10 + BS_OFFSET_BASE
BS_OFFSET_m_BNS_base16H   equ 0x12 + BS_OFFSET_BASE
BS_OFFSET_m_wBitsLeft     equ 0x10 + BS_OFFSET_BASE
BS_OFFSET_m_wNextBitsSkip equ 0x11 + BS_OFFSET_BASE
BS_OFFSET_m_fStreamStaus  equ 0x12 + BS_OFFSET_BASE
    .endif

    stmdb     sp!, {r4 - r11, lr}  @ stmfd
    FRAME_PROFILE_COUNT
    FUNCTION_PROFILE_START DECINVIBQUANTESCCODE_PROFILE

@Bool bIsLastRun = FALSE@
    mov r8, #0

@UInt uiCoefCounter = 0@
    mov r5, #0

    mov r4, #0

    mov r7, r3
    mov r11, r2
    mov r6, r1


@U32 ptr=(U32)pShortCut->m_rgiCoefRecon@
    ldr r0, [ r7, #tagEMB_DecodeP_ShortCut_m_rgiCoefRecon ]

@U32 ptrmax=ptr+BLOCK_SQUARE_SIZE_2@
@if(pShortCut->XFormMode!=XFORMMODE_8x8)
@ {
@    ptrmax-=BLOCK_SQUARE_SIZE_2>>1@
@ }

    ldr r1, [ r7, #tagEMB_DecodeP_ShortCut_XFormMode ]
    cmp r1, #0
    addeq r1, r0, #0x100
    addne r1, r0, #0x80
@    for(@ptr<ptrmax@ptr+=32)
@	{

@      *(U32 *)(ptr+0)=0@
@	   *(U32 *)(ptr+4)=0@
@	   *(U32 *)(ptr+8)=0@
@	   *(U32 *)(ptr+12)=0@
@	   *(U32 *)(ptr+16)=0@
@	   *(U32 *)(ptr+20)=0@
@	   *(U32 *)(ptr+24)=0@
@	   *(U32 *)(ptr+28)=0@

@    }
DIIBQECCE_L1:
    str r5, [ r0 ], #4
    str r5, [ r0 ], #4
    str r5, [ r0 ], #4
    str r5, [ r0 ], #4
    str r5, [ r0 ], #4
    str r5, [ r0 ], #4
    str r5, [ r0 ], #4
    str r5, [ r0 ], #4
    cmp r0, r1
    bne DIIBQECCE_L1
@    do {	
DIIBQECCE_L2:
@           lIndex = InterDCTTableInfo_Dec -> hufDCTACDec-> get (pShortCut->m_pbitstrmIn)@
    ldr r0, [ r6, #_CDCTTableInfo_Dec_hufDCTACDec ]
    ldr r2, [ r0, #H_m_decInfo ]

	.if NO_TABLE_INFO = 1
	ldr r1, [ r0, #H_m_firstLength ]
	.else
    ldr r1, [ r0, #H_m_tableInfo ]
	.endif
    ldr r0, [ r7, #tagEMB_DecodeP_ShortCut_m_pbitstrmIn ]
    ldr r10, [ r6, #_CDCTTableInfo_Dec_iTcoef_ESCAPE ]@@relo0
    bl _getHuffman

@if (lIndex != iTCOEF_ESCAPE)	
@@relo0    ldr r10, [ r6, #_CDCTTableInfo_Dec_iTcoef_ESCAPE ]
    cmp r0, r10
    beq DIIBQECCE_L3
@			if (lIndex >= iStartIndxOfLastRun)
@				bIsLastRun = TRUE@
    ldr r2, [ r6, #_CDCTTableInfo_Dec_iStartIndxOfLastRunMinus1 ]
@@relo1    cmp r0, r2
@@relo2    movgt r8, #1

    
@           uiRun = rgRunAtIndx[lIndex]@
@			iLevel=rgLevelAtIndx[lIndex]@
    
	.if COMBINED_LEVELRUN = 1
	ldr r10, [ r6, #_CDCTTableInfo_Dec_combined_levelrun ]
	.else
    ldr r10, [ r6, #_CDCTTableInfo_Dec_pcLevelAtIndx ]
    ldr r14, [ r6, #_CDCTTableInfo_Dec_puiRunAtIndx ]
    .endif

    cmp r0, r2  @@relo1
    movgt r8, #1 @@relo2

	.if COMBINED_LEVELRUN = 1
	ldrb r14, [ r10, r0, lsl #1 ]! 
	ldrb r10, [ r10, #1 ]
	.else
    ldrb r10, [ r10, r0 ]
	.endif
@@relo4    ldrb r14, [ r14, r0 ]

@ iLevel = pShortCut->m_iDoubleStepSize * iLevel + pShortCut->m_iStepMinusStepIsEven@

    ldr r1, [ r7, #tagEMB_DecodeP_ShortCut_m_iDoubleStepSize ]
    ldr r2, [ r7, #tagEMB_DecodeP_ShortCut_m_iStepMinusStepIsEven ]

	.if COMBINED_LEVELRUN = 0
    ldrb r14, [ r14, r0 ] @@relo4
	.endif
@@relo6    mla r10, r1, r10, r2

@uiCoefCounter += uiRun@
@@relo5    add r5, r5, r14

@if(pShortCut->m_pbitstrmIn->getBit())
@     iLevel=-iLevel@
@@  getBit() macro:
@@            Int wBitsLeft@
@@           if ((wBitsLeft=(m_BMS8.m_wBitsLeft-1))>=0) 
@@            {
@@                // result=(m_currBits >> m_dwBitsLeft) & GetMask[dwNumBits]@
@@                m_BMS8.m_wBitsLeft=wBitsLeft@
@@                return (m_currBits >> wBitsLeft) & 1@
@@            }
@@            return getBits2(1, wBitsLeft)@
    ldr r0, [ r7, #tagEMB_DecodeP_ShortCut_m_pbitstrmIn ]
    mla r10, r1, r10, r2 @@relo6
    ldrb r3, [ r0, #BS_OFFSET_m_wBitsLeft  ]
    add r5, r5, r14 @@relo5
    subs r2, r3, #1
    strgeb r2, [ r0, #BS_OFFSET_m_wBitsLeft  ]
    ldrge r2, [ r0, #BS_OFFSET_m_currBits ]
    blt  DIIBQECCE_L4 @ return getBits2(1, wBitsLeft)@
    movges r2, r2, ROR r3
    rsbcs r10, r10, #0

@		if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
@			result= ICERR_ERROR@
@			goto bailout@}

DIIBQECCE_L6:

    cmp r5, #64
@    bge DIIBQECCE_L5
	blt GO_ON_NEXT_STEP @2008/01/18
	mov r5, #63        @ if(uiCoefCounter >= BLOCK_SQUARE_SIZE) uiCoefCounter = 63@

GO_ON_NEXT_STEP:

@register Int iIndex = pZigzagInv [uiCoefCounter]@

    ldrb r1, [ r11, r5 ]

    ldr r14, [ r7, #tagEMB_DecodeP_ShortCut_m_rgiCoefRecon ] @@relo8
    
@if (iIndex & 0x7)
@  iDCTHorzFlags |= (1<<((iIndex & 0x38)>>3))@
    tst r1, #7
	movne     r3, r1, lsr #3
	andne     r3, r3, #7
	movne     r2, #1
	orrne     r4, r4, r2, lsl r3

@rgiCoefRecon2[iIndex]=iLevel@
@@relo8  ldr r14, [ r7, #tagEMB_DecodeP_ShortCut_m_rgiCoefRecon ]
    str r10, [ r14, r1, lsl #2 ]
    

@uiCoefCounter++@
    add r5, r5, #1

    movs r8, r8
    beq  DIIBQECCE_L2

@pShortCut->m_iDCTHorzFlags = iDCTHorzFlags@
@@relo9   str r4, [ r7, #tagEMB_DecodeP_ShortCut_m_iDCTHorzFlags ]
    
    ldrb r1, [ r0, #BS_OFFSET_m_fStreamStaus ]
    str r4, [ r7, #tagEMB_DecodeP_ShortCut_m_iDCTHorzFlags ] @@relo9
    cmp r1, #0
    movne r0, #100
    rsbne r0, r0, #0
    moveq r0, #0

    @FUNCTION_PROFILE_STOP
    ldmia     sp!, {r4 - r11, pc}  @ ldmfd

DIIBQECCE_L3:
    
@result=DecodeInverseInterBlockQuantizeEscCodeCE2(InterDCTTableInfo_Dec, uiRun, iLevel, bIsLastRun)@
    stmdb     sp!, { r8, r10, r14 }
    mov r3, sp 
    str r3, [ sp, #-4 ]!
    add r3, sp, #8
    add r2, sp, #12
    mov r1, r6
    ldr r0, [ r7, #tagEMB_DecodeP_ShortCut_pVideoDecoderObject ]

    bl _DecodeInverseInterBlockQuantizeEscCodeEMB2
    

    add sp, sp, #4

    ldmia   sp!, { r8, r10, r14 }  @ ldmfd

    ldr r0, [ r7, #tagEMB_DecodeP_ShortCut_m_pbitstrmIn ]

    @uiCoefCounter += uiRun@
    add r5, r5, r14

    b DIIBQECCE_L6

DIIBQECCE_L4:
    mov r1, #1
    bl _BS_getBits2
    cmp r0, #0
    rsbne r10, r10, #0
    ldr r0, [ r7, #tagEMB_DecodeP_ShortCut_m_pbitstrmIn ]
    b DIIBQECCE_L6

DIIBQECCE_L5:
    mov r0, #100
    rsb r0, r0, #0
    
    @FUNCTION_PROFILE_STOP
    ldmia     sp!, {r4 - r11, pc}  @ ldmfd

    @WMV_ENTRY_END
		.endmacro

	.endif @ WMV_OPT_BLKDEC_ARM
	
    @.end


