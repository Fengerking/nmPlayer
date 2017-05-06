@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************
    #include "../c/voWMVDecID.h"
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 

    @AREA MOTIONCOMP, CODE, READONLY
		.section .text 

    .if DYNAMIC_EDGEPAD == 1   @SW
    
    .if WMV_OPT_DYNAMICPAD_ARM == 1

    .global edgePadding
    IMPORT edgepad_tableY01
    IMPORT edgepad_tableY11
    IMPORT edgepad_tableY21
    IMPORT edgepad_tableY10
    IMPORT edgepad_tableY20
    IMPORT edgepad_tableY12
    IMPORT edgepad_tableY02
    IMPORT edgepad_tableY22

    .if ARCH_V3 = 1
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        MACRO             
        LOADONE16bitsHi $srcRN, $offset, $dstRN
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        LDR     $dstRN, [$srcRN, $offset]
        MOV     $dstRN, $dstRN, LSR #16
        MEND

        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        MACRO             
        LOADONE16bitsLo $srcRN, $offset, $dstRN
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        LDR     $dstRN, [$srcRN, $offset]
        MOV     $dstRN, $dstRN, LSL #16
        MOV     $dstRN, $dstRN, LSR #16
        MEND

        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        MACRO             
        LOADONE16bitsSHi $srcRN, $offset, $dstRN
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        LDR     $dstRN, [$srcRN, $offset]
        MOV     $dstRN, $dstRN, ASR #16
        MEND

        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        MACRO             
        LOADONE16bitsSLo $srcRN, $offset, $dstRN
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        LDR     $dstRN, [$srcRN, $offset]
        MOV     $dstRN, $dstRN, LSL #16
        MOV     $dstRN, $dstRN, ASR #16
        MEND
    .endif @ //ARCH_V3


@@@@@@@@@@@@@@@@@@@@@@@@@@@
    AREA	|.text|, CODE
    WMV_LEAF_ENTRY edgePadding
@@@@@@@@@@@@@@@@@@@@@@@@@@@

BLOCKSIZE_PLUS_2EDGES EQU 12
EDGEPAD_m_iPBMV_x  EQU  0x0
EDGEPAD_m_iPBMV_y  EQU  0x2
EDGEPAD_x_off_left EQU  0x8
EDGEPAD_x_off_right EQU  0xa
EDGEPAD_src_offset EQU  0xc
EDGEPAD_ppxlcRefBufferOrig EQU 0x10
EDGEPAD_ppxlcRef EQU 0x14
EDGEPAD_ppxlcCurr EQU 0x18
EDGEPAD_m_iWidthCombine EQU 0x1c
EDGEPAD_m_iround_ctrl EQU 0x20
EDGEPAD_buffer EQU 0x9c

Y_UV_Data_m_iStartOfLastLine EQU 0x0
Y_UV_Data_m_iPMBMV_limit_x EQU 0x4
Y_UV_Data_m_iPMBMV_limit_y EQU 0x6
Y_UV_Data_m_iWidth EQU 0x8
Y_UV_Data_m_iHeight EQU 0xa

Blk_Data_m_ppxliRef EQU 0x0
Blk_Data_m_ppxliCurr EQU 0x4
Blk_Data_m_ppxliRef0 EQU 0x8
Blk_Data_m_BlkInc EQU 0xc

@r0=pEdgePad
@r1=pData
@r2=pBlkData
@r3=index
@r4=pSrc
@r5=pSrc2
@r6=pDst
@r7=length_middle
@r8=i
@r9=k
@r10=scratch
@r11=pEdgePad->x_off_left
@r12=pData->m_iHeight
@r14=pData->m_iWidth

    stmdb     sp!, { r4 - r12, lr }  @ stmfd
    FRAME_PROFILE_COUNT

@index=0@
    
@I32_WMV outer_x=pEdgePad->m_iPBMV_x@
@I32_WMV outer_y=pEdgePad->m_iPBMV_y@

    .if ARCH_V3 = 1
        LOADONE16bitsSLo r0, #EDGEPAD_m_iPBMV_x, r8
        SUB r0, r0, #2
        LOADONE16bitsSHi r0, #EDGEPAD_m_iPBMV_y, r5
        ADD r0, r0, #2
    .else
        ldrsh r8, [ r0, #EDGEPAD_m_iPBMV_x ]
        ldrsh r5, [ r0, #EDGEPAD_m_iPBMV_y ]
    .endif

@  index = (((U32_WMV)outer_x)>>28)&0x8@
@  index |= ~((pData->m_iPMBMV_limit_x - outer_x ) >> 29) & 0x4@
@  index |= ~((pData->m_iPMBMV_limit_y - outer_y ) >> 30) & 0x2@
@  index |= ((U32_WMV)outer_y)>>31@

    .if ARCH_V3 = 1
        LOADONE16bitsLo r1, #Y_UV_Data_m_iPMBMV_limit_x, r4
        SUB r1, r1, #2
        LOADONE16bitsHi r1, #Y_UV_Data_m_iPMBMV_limit_y, r6
        ADD r1, r1, #2    
    .else
        ldrh r4, [ r1, #Y_UV_Data_m_iPMBMV_limit_x ] @ always word aligned
        ldrh r6, [ r1, #Y_UV_Data_m_iPMBMV_limit_y ] @ always not word aligned
    .endif

    sub r7, r4, r8
    and  r3, r7, #0x80000000
    and  r8, r8, #0x80000000
    orr  r3, r3, r8, lsr #1
    sub r7, r6, r5
    and  r7, r7, #0x80000000
    orr  r3, r3, r7, lsr #2
    and  r5, r5, #0x80000000
    orr  r3, r3, r5, lsr #3
    movs  r3, r3, lsr #28


@  if(!index)
    bne  edgePad_L0

@pEdgePad->ppxlcRef = pBlkData->m_ppxliRef + pBlkData->m_BlkInc@
@pEdgePad->m_iWidthCombine=pData->m_iWidth@
@pEdgePad->m_iWidthCombine = pData->m_iWidth | (pEdgePad->m_iWidthCombine<<17) | pEdgePad->m_iround_ctrl@
@pEdgePad->ppxlcCurr = pBlkData->m_ppxliCurr + pBlkData->m_BlkInc@


    ldr r3, [ r2, #Blk_Data_m_ppxliRef ]
    ldr r6, [ r0, #EDGEPAD_m_iround_ctrl ]
    ldr r4, [ r2, #Blk_Data_m_BlkInc ]
    .if ARCH_V3 = 1
        LOADONE16bitsLo r1, #Y_UV_Data_m_iWidth, r5
    .else
        ldrh r5, [ r1, #Y_UV_Data_m_iWidth ]
    .endif
    ldr r7, [ r2, #Blk_Data_m_ppxliCurr ]
    add r3, r3, r4
    str r3, [ r0, #EDGEPAD_ppxlcRef ]
    orr r5, r5, r5, lsl #17
    orr r5, r5, r6
    str r5, [ r0, #EDGEPAD_m_iWidthCombine ]
    add r7, r7, r4
    str r7, [ r0, #EDGEPAD_ppxlcCurr ]

    LDMFD   sp!, {r4 - r12, PC}
    WMV_ENTRY_END

edgePad_L0

@@@@@@@@@@@@@@@@@@@@@@@@@@

@EdgePadFuncTable[index](pEdgePad, pData)@
    stmdb     sp!, { r0 - r2 }  @ stmfd
    add r14, pc, #48
    ldr pc, [ pc, r3, lsl #2 ]
    DCD edgepad_tableY_invalid @skip
    DCD edgepad_tableY_invalid @0x0
    DCD edgepad_tableY01       @0x1
    DCD edgepad_tableY02       @0x2
    DCD edgepad_tableY_invalid @0x3
    DCD edgepad_tableY10       @0x4
    DCD edgepad_tableY11       @0x5
    DCD edgepad_tableY12       @0x6
    DCD edgepad_tableY_invalid @0x7
    DCD edgepad_tableY20       @0x8
    DCD edgepad_tableY21       @0x9
    DCD edgepad_tableY22       @0xa
    

edgePad_L1
    LDMFD   sp!, { r0 - r2 }

    .if ARCH_V3 = 1
        LOADONE16bitsLo r1, #Y_UV_Data_m_iWidth, r14
        SUB r1, r1, #2
        LOADONE16bitsHi r1, #Y_UV_Data_m_iHeight, r12
        ADD r1, r1, #2
        LOADONE16bitsSLo r0, #EDGEPAD_x_off_left, r11
        SUB r0, r0, #2
        LOADONE16bitsSHi r0, #EDGEPAD_x_off_right, r7
        LOADONE16bitsSHi r0, #EDGEPAD_m_iPBMV_y, r8
        ADD r0, r0, #2
    .else
        ldrh r14, [ r1, #Y_UV_Data_m_iWidth ]
        ldrh r12, [ r1, #Y_UV_Data_m_iHeight ]
        ldrsh r11, [ r0, #EDGEPAD_x_off_left ]
        ldrsh r7, [ r0, #EDGEPAD_x_off_right ] @@relo1
        ldrsh r8, [ r0, #EDGEPAD_m_iPBMV_y ] @@relo0
    .endif

@   U8_WMV * pSrc = pBlkData->m_ppxliRef0 + pEdgePad->src_offset@
@   U8_WMV * pDst = &pEdgePad->buffer[0]@
    ldr r3, [ r2, #Blk_Data_m_ppxliRef0 ]
    ldr r4, [ r0, #EDGEPAD_src_offset ]
    ldr r6, [ r0, #EDGEPAD_buffer ]
    add r4, r4, r3
@I32_WMV length_middle=BLOCKSIZE_PLUS_2EDGES  - pEdgePad->x_off_right@
@@relo1    ldrsh r7, [ r0, #EDGEPAD_x_off_right ]
    rsb r7, r7, #BLOCKSIZE_PLUS_2EDGES@
@for(i=pEdgePad->m_iPBMV_y@ i< ( pEdgePad->m_iPBMV_y + BLOCKSIZE_PLUS_2EDGES )@ i++)
@@relo0   ldrsh r8, [ r0, #EDGEPAD_m_iPBMV_y ]
    add r3, r8, #BLOCKSIZE_PLUS_2EDGES

edgePad_L2
    
    ldrb r10, [ r4 ]@
    movs r9, r11
    ble  edgePad_L4
@for(k=0@ k< pEdgePad->x_off_left @ k++) *pDst++ = pSrc[0]@
edgePad_L3
    strb r10, [ r6 ], #1
    subs r9, r9, #1
    bgt  edgePad_L3
edgePad_L4
@ pSrc2 = pSrc@
@ for( @ k < length_middle@ k++) *pDst++ = *pSrc2++@
    mov r5, r4
    subs r9, r7, r11
    ble  edgePad_L6
edgePad_L5
    ldrb r10, [ r5 ], #1
    subs r9, r9, #1
    strb r10, [ r6 ], #1
    bgt  edgePad_L5
edgePad_L6
@for( @ k< BLOCKSIZE_PLUS_2EDGES @ k++) *pDst++ = pSrc2[-1]@
    rsbs r9, r7, #BLOCKSIZE_PLUS_2EDGES
    ble  edgePad_L8
    ldrb r10, [ r5, #-1 ]@
edgePad_L7
    strb r10, [ r6 ], #1
    subs r9, r9, #1
    bgt  edgePad_L7
edgePad_L8
@if((i>=0)&&(i<(pData->m_iHeight-1)))
@   pSrc+=pData->m_iWidth@
    adds r8, r8, #1
    cmpgt r12, r8
    addgt r4, r4, r14

    cmp r8, r3
    blt edgePad_L2

@pEdgePad->ppxlcRef=pEdgePad->ppxlcRefBufferOrig@
    ldr r8, [ r0, #EDGEPAD_ppxlcRefBufferOrig ]
    mov r9, #BLOCKSIZE_PLUS_2EDGES @@relo2
    ldr r6, [ r0, #EDGEPAD_m_iround_ctrl ] @@relo3
    ldr r4, [ r2, #Blk_Data_m_BlkInc ] @@relo4
    ldr r7, [ r2, #Blk_Data_m_ppxliCurr ] @@relo5
    str r8, [ r0, #EDGEPAD_ppxlcRef ]

@pEdgePad->m_iWidthCombine=BLOCKSIZE_PLUS_2EDGES@
@    pEdgePad->m_iWidthCombine = pData->m_iWidth | (pEdgePad->m_iWidthCombine<<17) | pEdgePad->m_iround_ctrl@
@@relo2    mov r9, #BLOCKSIZE_PLUS_2EDGES
@@relo3    ldr r6, [ r0, #EDGEPAD_m_iround_ctrl ]
    orr r9, r14, r9, lsl #17
    orr r9, r9, r6
    str r9, [ r0, #EDGEPAD_m_iWidthCombine ]



    
@    pEdgePad->ppxlcCurr = pBlkData->m_ppxliCurr + pBlkData->m_BlkInc@

@@relo4    ldr r4, [ r2, #Blk_Data_m_BlkInc ]
@@relo5    ldr r7, [ r2, #Blk_Data_m_ppxliCurr ]
    add r7, r7, r4
    str r7, [ r0, #EDGEPAD_ppxlcCurr ]


    LDMFD   sp!, {r4 - r12, PC}

edgepad_tableY_invalid
    DCD       0xE6000010
    mov pc, r14
    
    .endif @ WMV_OPT_DYNAMICPAD_ARM

    .endif @ DYNAMIC_EDGEPAD

    .END

