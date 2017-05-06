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

    IF 0=1	;All code is not used.

    IF WMV_OPT_MOTIONCOMP_ARM=1

    AREA WMV9MOTIONCOMP, CODE, READONLY
    
    IMPORT  memcpy
    IMPORT  gaSubsampleBicubic
    IMPORT  gaSubsampleBilinear
    IMPORT  g_InterpolateBlockBilinear_Overflow_C
    IMPORT  g_InterpolateBicubic_Overflow_C
    IMPORT  g_InterpolateBicubic_Overflow_zeroXFrac_C
    IMPORT  g_InterpolateBicubic_Overflow_zeroYFrac_C
    EXPORT  g_InterpolateBlockBicubic_00_C
    EXPORT  g_InterpolateBlockBicubic_01_C
    EXPORT  g_InterpolateBlockBicubic_10_C
    EXPORT  g_InterpolateBlockBicubic_11_C
    EXPORT  g_InterpolateBlockBilinear_C

    ; The following routines are moved from interpolate_wmv9_arm.s
    IMPORT  g_InterpolateBlockBicubic_EMB
    IMPORT  g_VertFilterStepTbl
    IMPORT  gaSubsampleBicubic
    EXPORT  g_InterpolateBlockBicubic_xx_EMB

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

|g_InterpolateBlockBilinear_C| PROC

; 224  : {

    mov       r12, sp
    stmdb     sp!, {r0 - r3}  ; stmfd
    stmdb     sp!, {r4 - r12, lr}  ; stmfd
    FRAME_PROFILE_COUNT
    sub       sp, sp, #0xC8  ; 0xC8 = 200
|$M37620|
    str       r0, [sp, #0x10]  ; 0x10 = 16
    mov       r5, r2
    mov       r12, r1
    ;str       r1, [sp, #0x14]  ; 0x14 = 20
    str       r3, [sp, #0x1C]  ; 0x1C = 28

; 225  : 
; 226  :     I32_WMV i, j;
; 227  :     I32_WMV iRound_32;
; 228  :     U32_WMV overflow = 0;

    mov       r0, #0

; 229  :     I16_WMV v0, v1, h0, h1;
; 230  :     const I16_WMV *pH = gaSubsampleBilinear + 2 * iXFrac;

    ldr       r2, =gaSubsampleBilinear ;;;; [pc, #0x1E4]  ;  pc+8+484 = 00000210
    str       r0, [sp, #0x20]  ; 0x20 = 32
    ldr       r0, [sp, #0x100]  ; 0x100 = 256

; 231  :     const I16_WMV *pV = gaSubsampleBilinear + 2 * iYFrac;

    ldr       r1, [sp, #0x104]  ; 0x104 = 260
    add       r6, r2, r0, lsl #2
    add       r0, r2, r1, lsl #2

; 232  :     I16_WMV pFilter[(32 + 4)*2];
; 233  :  
; 234  :     //assert (iWidth < 32); // Due to pFilter size, we can only handle widths of <= 32
; 235  : 
; 236  :     v0 = pV[0];
; 237  :     v1 = pV[1];
; 238  :     h0 = pH[0];

    ldrsh     r2, [r6]
    str       r0, [sp, #0x28]  ; 0x28 = 40

; 239  :     h1 = pH[1];

    ldrsh     r4, [r6, #2]
    str       r6, [sp, #0x30]  ; 0x30 = 48

; 240  :    
; 241  :     iRound_32 =  8 - iRndCtrl ;

    ldr       r6, [sp, #0x108]  ; 0x108 = 264
    ldrsh     r1, [r0, #2]
    ldrsh     r3, [r0]
    rsb       r0, r6, #8

; 242  :     iRound_32 = iRound_32 | iRound_32<<16;

    orr       r11, r0, r0, lsl #16

; 243  : 
; 244  :     for (i = 0; i < 8; i++) {

    mov       r9, r3
    mov       r8, r1
    mov       lr, r4
    mov       r10, r2
    mov       r0, #8
    str       r0, [sp, #0x24]  ; 0x24 = 36
    ldr       r0, [sp, #0x1C]  ; 0x1C = 28
    add       r1, sp, #0x38  ; 0x38 = 56
    sub       r7, r5, r1
    add       r2, r5, r0, lsl #3
    str       r7, [sp, #0x18]  ; 0x18 = 24
    str       r2, [sp, #0x2C]  ; 0x2C = 44
    b         |$L37418|  ; 000000A8
|$L37619|

; 225  : 
; 226  :     I32_WMV i, j;
; 227  :     I32_WMV iRound_32;
; 228  :     U32_WMV overflow = 0;

    ldr       r7, [sp, #0x18]  ; 0x18 = 24
|$L37418|

; 245  :         const U8_WMV  *pT = pSrc;

    ldr       r0, [sp, #0x10]  ; 0x10 = 16

; 246  :         U32_WMV * pF32 = (U32_WMV *) pFilter;

    add       r5, sp, #0x38  ; 0x38 = 56
    mov       r6, #3
    mov       r4, r0
|$L37426|

; 247  :         U8_WMV * pF;
; 248  :         U32_WMV * pDst32;
; 249  : 
; 250  :         for (j = 0; j < 12; j+=4) {
; 251  :             
; 252  :             U32_WMV t0, o0;
; 253  : 
; 254  :             t0 = pT[0] | (pT[2]<<16);
; 255  :             o0 = t0 * v0;

    ;ldr       r0, [sp, #0x14]  ; 0x14 = 20
    sub       r6, r6, #1
    ldrb      r1, [r4, #2]
    cmp       r6, #0
    ldrb      r0, [r4], +r12
    orr       r2, r0, r1, lsl #16

; 256  :             pT += iSrcStride;
; 257  :             t0 = pT[0] | (pT[2]<<16);
; 258  :             o0 += t0 * v1;
; 259  : 
; 260  :             //o0  =  ( t0 * v0 + t1 * v1 );
; 261  : 
; 262  :             *(I32_WMV *)pF32 = o0 ;

    ldrb      r1, [r4, #2]
    ldrb      r0, [r4]
    mul       r3, r2, r9
    orr       r2, r0, r1, lsl #16

; 263  : 
; 264  :             t0 = pT[1] | (pT[3]<<16);
; 265  :             o0 = t0 * v1;

    ldrb      r1, [r4, #3]
    ldrb      r0, [r4, #1]
    mla       r3, r2, r8, r3
    orr       r2, r0, r1, lsl #16

; 266  :             pT -= iSrcStride;

    ;ldr       r0, [sp, #0x14]  ; 0x14 = 20
    str       r3, [r5]
    mul       r3, r2, r8
    sub       r4, r4, r12

; 267  :             t0 = pT[1] | (pT[3]<<16);
; 268  :             o0 += t0 * v0;
; 269  : 
; 270  :             //o0  =  ( t0 * v0 + t1 * v1 );
; 271  : 
; 272  :             *(I32_WMV *)(pF32 + PFILTERODD_OFFSET32) = o0 ;

    ldrb      r1, [r4, #3]
    ldrb      r0, [r4, #1]

; 273  : 
; 274  :             pF32 ++;
; 275  :             pT +=  4;

    add       r4, r4, #4
    orr       r2, r0, r1, lsl #16
    mla       r3, r2, r9, r3
    str       r3, [r5, #0x48]  ; 0x48 = 72
    add       r5, r5, #4
    bhi       |$L37426|  ; 000000B8

; 276  :         }
; 277  :         
; 278  : 
; 279  :         pF = (U8_WMV *)pFilter;

    add       r6, sp, #0x38  ; 0x38 = 56
    mov       r7, #2
|$L37435|

; 280  :         pDst32 = (U32_WMV *)pDst;
; 281  : 
; 282  :         for (j = 0; j < 8; j+=4) 
; 283  :         {
; 284  :             U32_WMV t0, t1,t2;
; 285  :             U32_WMV o0, o1;
; 286  : 
; 287  :             o0 = iRound_32;
; 288  :             o1 = iRound_32;
; 289  :             t0 = *(I32_WMV *)pF;
; 290  :             o0 += t0 * h0;

    ldr       r0, [r6]
    sub       r7, r7, #1

; 291  :             t0 = *(I32_WMV *)(pF + PFILTERODD_OFFSET8);
; 292  :             o0 += t0 * h1;
; 293  :             o1 += t0 * h0;
; 294  :             t0 = *(U16_WMV *)(pF+2) |  (*(U16_WMV *)(pF+4)) <<16;
; 295  :             o1 += t0 * h1;

    ldrh      r3, [r6, #4]
    cmp       r7, #0
    mla       r1, r0, r10, r11
    ldr       r2, [r6, #0x48]  ; 0x48 = 72
    mov       r0, r3, lsl #16
    mla       r5, lr, r2, r1
    mla       r4, r10, r2, r11
    mov       r1, r0, lsr #16
    ldrh      r0, [r6, #2]
    mov       r2, r1, lsl #16
    mov       r1, r0, lsl #16
    orr       r2, r2, r1, lsr #16
    mla       r3, r2, lr, r4

; 296  :             
; 297  :             //o0 = t0 * h0  + t1 * h1 + iRound_32;
; 298  :             //o1 = t1 * h0  + t2 * h1 + iRound_32;
; 299  : 
; 300  :             overflow |= o0;
; 301  :             overflow |= o1;

    ldr       r2, [sp, #0x20]  ; 0x20 = 32

; 302  :             o0 >>= 4;
; 303  :             o1 >>= 4;
; 304  :             o0 &= 0x00ff00ff;
; 305  :             o1 &= 0x00ff00ff;
; 306  :            
; 307  :             *pDst32 = o0| o1<<8;

    mov       r1, r3, lsl #4

; 308  :             pDst32++;
; 309  : 
; 310  :             pF += 4;
; 311  :         }
; 312  : 
; 313  :         pSrc += iSrcStride;
; 314  :         pDst += iDstStride;
; 315  :     }
; 316  : 
; 317  :     if ( overflow & 0xf000f000) {
; 318  :         g_InterpolateBlockBilinear_Overflow_C (pSrc, iSrcStride, pDst, iDstStride, iRndCtrl, pH, pV, pFilter);

    orr       r0, r3, r5
    orr       r2, r0, r2
    eor       r0, r1, r5, lsr #4
    str       r2, [sp, #0x20]  ; 0x20 = 32
    mov       r1, #0xFF, 16  ; 0xFF0000 = 16711680
    orr       r1, r1, #0xFF  ; 0xFF = 255
    and       r0, r0, r1
    eor       r1, r0, r3, lsl #4
    ldr       r0, [sp, #0x18]  ; 0x18 = 24
    str       r1, [r0, +r6]
    add       r6, r6, #4
    bhi       |$L37435|  ; 00000128
    ldr       r3, [sp, #0x1C]  ; 0x1C = 28
    ldr       r4, [sp, #0x10]  ; 0x10 = 16
    add       r0, r0, r3
    ;ldr       r1, [sp, #0x14]  ; 0x14 = 20
    mov       r1, r12
    str       r0, [sp, #0x18]  ; 0x18 = 24
    ldr       r0, [sp, #0x24]  ; 0x24 = 36
    add       r4, r4, r12
    str       r4, [sp, #0x10]  ; 0x10 = 16
    sub       r0, r0, #1
    str       r0, [sp, #0x24]  ; 0x24 = 36
    cmp       r0, #0
    bhi       |$L37619|  ; 000000A4
    mov       r0, #0xF, 4  ; 0xF0000000 = 4026531840
    orr       r0, r0, #0xF, 20  ; 0xF000 = 61440
    tst       r2, r0
    beq       |$L37447|  ; 00000208
    add       r0, sp, #0x38  ; 0x38 = 56
    ldr       r2, [sp, #0x2C]  ; 0x2C = 44
    str       r0, [sp, #0xC]  ; 0xC = 12
    ldr       r0, [sp, #0x28]  ; 0x28 = 40
    str       r0, [sp, #8]
    ldr       r0, [sp, #0x30]  ; 0x30 = 48
    str       r0, [sp, #4]
    ldr       r0, [sp, #0x108]  ; 0x108 = 264
    str       r0, [sp]
    mov       r0, r4
    bl        g_InterpolateBlockBilinear_Overflow_C  ; 0000020C
|$L37447|

; 319  :     }
; 320  :     // dbg_cnt_2++;
; 321  : }

    add       sp, sp, #0xC8  ; 0xC8 = 200
    ldmia     sp, {r4 - r11, sp, pc}  ; ldmfd

    ENDP  ; |g_InterpolateBlockBilinear_C|

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

|g_InterpolateBlockBicubic_00_C| PROC

; 341  : {

    mov       r12, sp
    stmdb     sp!, {r0 - r3}  ; stmfd
    stmdb     sp!, {r4 - r8, r12, lr}  ; stmfd
    FRAME_PROFILE_COUNT
|$M37804|
    mov       r8, r0
    mov       r7, r1
    mov       r5, r2
    mov       r4, r3

; 342  :     I32_WMV i;
; 343  :     for (i = 0; i < 8; i++) {

    mov       r6, #8
|$L37465|

; 344  :         memcpy(pDst, pSrc, 8);

    mov       r2, #8
    mov       r1, r8
    mov       r0, r5
    bl        memcpy  ; 00000034
    sub       r6, r6, #1

; 345  :         pDst += iDstStride;

    add       r5, r5, r4

; 346  :         pSrc += iSrcStride;

    add       r8, r8, r7
    cmp       r6, #0
    bhi       |$L37465|  ; 00000020

; 347  :     }
; 348  : }

    ldmia     sp, {r4 - r8, sp, pc}  ; ldmfd
|$M37805|

    ENDP  ; |g_InterpolateBlockBicubic_00_C|

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

|g_InterpolateBlockBicubic_01_C| PROC

; 351  : {

    mov       r12, sp
    stmdb     sp!, {r0 - r3}  ; stmfd
    stmdb     sp!, {r4 - r12, lr}  ; stmfd
    FRAME_PROFILE_COUNT
    sub       sp, sp, #0x34  ; 0x34 = 52
|$M37848|
    mov       r7, r0
    mov       r12, r1
    ;str       r1, [sp, #0xC]  ; 0xC = 12
    str       r2, [sp, #0x18]  ; 0x18 = 24
    str       r3, [sp, #0x28]  ; 0x28 = 40

; 352  :     // vertical filtering only
; 353  :     register I16_WMV v0;
; 354  :     register I16_WMV v1;
; 355  :     register I16_WMV v2;
; 356  :     register I16_WMV v3;
; 357  :     I32_WMV i, j;
; 358  :     I32_WMV iShift = (iYFrac == 2) ? 4 : 6;

    ldr       r2, [sp, #0x70]  ; 0x70 = 112
    mov       r6, #4

; 359  :     I32_WMV iRound = (1 << (iShift - 1)) - 1 + iRndCtrl;

    ldr       r0, [sp, #0x74]  ; 0x74 = 116
    mov       r8, #1
    cmp       r2, #2
    movne     r6, #6
    sub       r1, r6, #1
    str       r6, [sp, #0x10]  ; 0x10 = 16
    add       r1, r0, r8, lsl r1

; 360  :     I32_WMV iRound32 = iRound | (iRound<<16);
; 361  :     U32_WMV overflow = 0;
; 362  :     U32_WMV mask;
; 363  :     I16_WMV *pV = gaSubsampleBicubic + 4 * iYFrac;

    ldr       r0, =gaSubsampleBicubic ;[pc, #0x200]  ;  pc+8+512 = 0000024C
    sub       r1, r1, #1
    str       r1, [sp, #0x1C]  ; 0x1C = 28
    mov       r1, #0
    str       r1, [sp, #0x24]  ; 0x24 = 36
    add       r0, r0, r2, lsl #3

; 364  : 
; 365  :     v0 = pV[0];
; 366  :     v1 = pV[1];
; 367  :     v2 = pV[2];
; 368  :     v3 = pV[3];
; 369  : 
; 370  :     mask = ((1<<(iShift+8))-1);

    ldr       r1, [sp, #0x10]  ; 0x10 = 16
    ldrsh     r6, [r0]
    ldrsh     r5, [r0, #2]
    ldrsh     r4, [r0, #4]
    ldrsh     r3, [r0, #6]
    str       r0, [sp, #0x30]  ; 0x30 = 48
    add       r0, r1, #8
    mov       r1, r8, lsl r0
    sub       r2, r1, #1

; 371  :     mask = mask | (mask<<16);

    orr       r0, r2, r2, lsl #16

; 372  :     mask = ~mask;

    mvn       r0, r0
    str       r0, [sp, #0x2C]  ; 0x2C = 44

; 373  : 
; 374  :     pSrc -= iSrcStride;

    ;ldr       r0, [sp, #0xC]  ; 0xC = 12
    sub       r1, r7, r12

; 375  :     for (i = 0; i < 8; i++)

    mov       r7, r3
    str       r1, [sp, #0x14]  ; 0x14 = 20
    mov       r8, r4
    mov       r9, r5
    ldr       r5, [sp, #0x24]  ; 0x24 = 36
    mov       r10, r6

; 378  :         {
; 379  :             register U32_WMV t0;
; 380  :             register U32_WMV o0;
; 381  :             register U32_WMV o1;
; 382  : 
; 383  :             o0 = pSrc[0] | (pSrc[2]<<16);
; 384  :             o0 *= v0;
; 385  :             pSrc += iSrcStride;
; 386  :             t0 = pSrc[0] | (pSrc[2]<<16);
; 387  :             o0 += t0 * v1;
; 388  :             pSrc += iSrcStride;
; 389  :             t0 = pSrc[0] | (pSrc[2]<<16);
; 390  :             o0 += t0 * v2;
; 391  :             pSrc += iSrcStride;
; 392  :             t0 = pSrc[0] | (pSrc[2]<<16);
; 393  :             o0 += t0 * v3;
; 394  :             o0 += iRound32;
; 395  :             overflow |= o0;
; 396  :             o0 >>= iShift;
; 397  :             o0 &= 0x00ff00ff;
; 398  : 
; 399  :             o1 = pSrc[1] | (pSrc[3]<<16);
; 400  :             o1 *= v3;
; 401  :             pSrc -= iSrcStride;
; 402  :             t0 = pSrc[1] | (pSrc[3]<<16);
; 403  :             o1 += t0 * v2;
; 404  :             pSrc -= iSrcStride;
; 405  :             t0 = pSrc[1] | (pSrc[3]<<16);
; 406  :             o1 += t0 * v1;
; 407  :             pSrc -= iSrcStride;
; 408  :             t0 = pSrc[1] | (pSrc[3]<<16);
; 409  :             o1 += t0 * v0;
; 410  :             o1 += iRound32;
; 411  :             overflow |= o1;
; 412  :             o1 >>= iShift;
; 413  :             o1 &= 0x00ff00ff;
; 414  : 
; 415  :             *(U32_WMV *)pDst = o0 | (o1<<8);
; 416  : 
; 417  :             pSrc += 4;
; 418  :             pDst += 4;
; 419  :         }
; 420  : 
; 421  :         pSrc += (iSrcStride - 8);

    ;ldr       r6, [sp, #0xC]  ; 0xC = 12
    mov       r0, #8
    str       r0, [sp, #0x20]  ; 0x20 = 32
    sub       lr, r12, #8
    b         |$L37495|  ; 000000D4
|$L37845|
    ;ldr       r6, [sp, #0xC]  ; 0xC = 12
|$L37495|

; 376  :     {
; 377  :         for (j = 0; j < 8; j+=4) 

    mov       r11, #2

; 378  :         {
; 379  :             register U32_WMV t0;
; 380  :             register U32_WMV o0;
; 381  :             register U32_WMV o1;
; 382  : 
; 383  :             o0 = pSrc[0] | (pSrc[2]<<16);
; 384  :             o0 *= v0;
; 385  :             pSrc += iSrcStride;
; 386  :             t0 = pSrc[0] | (pSrc[2]<<16);
; 387  :             o0 += t0 * v1;
; 388  :             pSrc += iSrcStride;
; 389  :             t0 = pSrc[0] | (pSrc[2]<<16);
; 390  :             o0 += t0 * v2;
; 391  :             pSrc += iSrcStride;
; 392  :             t0 = pSrc[0] | (pSrc[2]<<16);
; 393  :             o0 += t0 * v3;
; 394  :             o0 += iRound32;
; 395  :             overflow |= o0;
; 396  :             o0 >>= iShift;
; 397  :             o0 &= 0x00ff00ff;
; 398  : 
; 399  :             o1 = pSrc[1] | (pSrc[3]<<16);
; 400  :             o1 *= v3;
; 401  :             pSrc -= iSrcStride;
; 402  :             t0 = pSrc[1] | (pSrc[3]<<16);
; 403  :             o1 += t0 * v2;
; 404  :             pSrc -= iSrcStride;
; 405  :             t0 = pSrc[1] | (pSrc[3]<<16);
; 406  :             o1 += t0 * v1;
; 407  :             pSrc -= iSrcStride;
; 408  :             t0 = pSrc[1] | (pSrc[3]<<16);
; 409  :             o1 += t0 * v0;
; 410  :             o1 += iRound32;
; 411  :             overflow |= o1;
; 412  :             o1 >>= iShift;
; 413  :             o1 &= 0x00ff00ff;
; 414  : 
; 415  :             *(U32_WMV *)pDst = o0 | (o1<<8);
; 416  : 
; 417  :             pSrc += 4;
; 418  :             pDst += 4;
; 419  :         }
; 420  : 
; 421  :         pSrc += (iSrcStride - 8);

    b         |$L37847|  ; 000000E0
|$L37844|
    ;ldr       r6, [sp, #0xC]  ; 0xC = 12
|$L37847|
    ldr       r4, [sp, #0x14]  ; 0x14 = 20
    sub       r11, r11, #1
    cmp       r11, #0
    ldrb      r1, [r4, #2]
    ldrb      r0, [r4], +r12
    orr       r2, r0, r1, lsl #16
    ldrb      r1, [r4, #2]
    ldrb      r0, [r4], +r12
    mul       r3, r2, r10
    orr       r2, r0, r1, lsl #16
    ldrb      r1, [r4, #2]
    ldrb      r0, [r4], +r12
    mla       r3, r2, r9, r3
    orr       r2, r0, r1, lsl #16
    ldrb      r1, [r4, #2]
    ldrb      r0, [r4]
    mla       r3, r2, r8, r3
    orr       r2, r0, r1, lsl #16
    ldr       r1, [sp, #0x1C]  ; 0x1C = 28

; 422  :         pDst += (iDstStride - 8);
; 423  :     }
; 424  : 
; 425  :     if ( overflow & mask) {
; 426  :         g_InterpolateBicubic_Overflow_zeroXFrac_C (pSrc, iSrcStride, pDst, iDstStride, pV, iRound, iShift);

    mla       r3, r2, r7, r3
    orr       r0, r1, r1, lsl #16
    ldrb      r1, [r4, #3]
    add       r6, r3, r0
    ldrb      r0, [r4, #1]
    orr       r5, r6, r5
    orr       r2, r0, r1, lsl #16
    ;ldr       r1, [sp, #0xC]  ; 0xC = 12
    mul       r3, r2, r7
    sub       r4, r4, r12
    ldrb      r1, [r4, #3]
    ldrb      r0, [r4, #1]
    orr       r2, r0, r1, lsl #16
    ;ldr       r1, [sp, #0xC]  ; 0xC = 12
    mla       r3, r2, r8, r3
    sub       r4, r4, r12
    ldrb      r1, [r4, #3]
    ldrb      r0, [r4, #1]
    orr       r2, r0, r1, lsl #16
    ;ldr       r1, [sp, #0xC]  ; 0xC = 12
    mla       r3, r2, r9, r3
    sub       r4, r4, r12
    ldrb      r1, [r4, #3]
    ldrb      r0, [r4, #1]
    add       r4, r4, #4
    str       r4, [sp, #0x14]  ; 0x14 = 20
    orr       r2, r0, r1, lsl #16
    ldr       r1, [sp, #0x1C]  ; 0x1C = 28
    mla       r3, r2, r10, r3
    ldr       r2, [sp, #0x10]  ; 0x10 = 16
    orr       r0, r1, r1, lsl #16
    add       r3, r3, r0
    mov       r0, r3, lsr r2
    orr       r5, r3, r5
    mov       r1, r0, lsl #8
    mov       r0, #0xFF, 16  ; 0xFF0000 = 16711680
    orr       r0, r0, #0xFF  ; 0xFF = 255
    eor       r2, r1, r6, lsr r2
    ldr       r6, [sp, #0x18]  ; 0x18 = 24
    and       r1, r2, r0
    ldr       r2, [sp, #0x10]  ; 0x10 = 16
    mov       r0, r3, lsr r2
    eor       r1, r1, r0, lsl #8
    str       r1, [r6], #4
    str       r6, [sp, #0x18]  ; 0x18 = 24
    bhi       |$L37844|  ; 000000DC
    ldr       r3, [sp, #0x28]  ; 0x28 = 40
    add       r4, r4, lr
    str       r4, [sp, #0x14]  ; 0x14 = 20
    sub       r0, r3, #8
    add       r6, r6, r0
    ldr       r0, [sp, #0x20]  ; 0x20 = 32
    str       r6, [sp, #0x18]  ; 0x18 = 24
    sub       r0, r0, #1
    str       r0, [sp, #0x20]  ; 0x20 = 32
    cmp       r0, #0
    bhi       |$L37845|  ; 000000D0
    ldr       r0, [sp, #0x2C]  ; 0x2C = 44
    tst       r0, r5
    beq       |$L37505|  ; 00000244
    ldr       r0, [sp, #0x1C]  ; 0x1C = 28
    str       r2, [sp, #8]
    mov       r2, r6
    str       r0, [sp, #4]
    ldr       r0, [sp, #0x30]  ; 0x30 = 48
    str       r0, [sp]
    ;ldr       r0, [sp, #0xC]  ; 0xC = 12
    mov       r1, r12
    mov       r0, r4
    bl        g_InterpolateBicubic_Overflow_zeroXFrac_C  ; 00000248
|$L37505|

; 427  :     }
; 428  : }

    add       sp, sp, #0x34  ; 0x34 = 52
    ldmia     sp, {r4 - r11, sp, pc}  ; ldmfd

|$M37849|

    ENDP  ; |g_InterpolateBlockBicubic_01_C|

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

|g_InterpolateBlockBicubic_10_C| PROC

; 431  : {

    mov       r12, sp
    stmdb     sp!, {r0 - r3}  ; stmfd
    stmdb     sp!, {r4 - r12, lr}  ; stmfd
    FRAME_PROFILE_COUNT
    sub       sp, sp, #0x34  ; 0x34 = 52
|$M37887|
    mov       r7, r0
    str       r1, [sp, #0x28]  ; 0x28 = 40
    str       r2, [sp, #0x10]  ; 0x10 = 16
    str       r3, [sp, #0x24]  ; 0x24 = 36

; 432  :     // horizontal filtering only
; 433  :     register I16_WMV h0;
; 434  :     register I16_WMV h1;
; 435  :     register I16_WMV h2;
; 436  :     register I16_WMV h3;
; 437  :     I32_WMV iShift = (iXFrac == 2) ? 4 : 6;

    ldr       r2, [sp, #0x6C]  ; 0x6C = 108
    mov       r8, #4

; 438  :     I32_WMV iRound = (1 << (iShift - 1)) - iRndCtrl;

    ldr       r0, [sp, #0x74]  ; 0x74 = 116
    mov       r9, #1
    cmp       r2, #2
    movne     r8, #6
    sub       r1, r8, #1
    str       r8, [sp, #0x18]  ; 0x18 = 24
    rsb       r6, r0, r9, lsl r1

; 439  :     I32_WMV iRound32 = iRound | (iRound<<16);
; 440  :     U32_WMV overflow = 0;
; 441  :     U32_WMV mask;
; 442  :     I32_WMV i, j;
; 443  :     I16_WMV *pH = gaSubsampleBicubic + 4 * iXFrac;

    ldr       r0, =gaSubsampleBicubic ;;;[pc, #0x18C]  ;  pc+8+396 = 000001D8
    str       r6, [sp, #0x20]  ; 0x20 = 32
    add       r0, r0, r2, lsl #3

; 444  : 
; 445  :     h0 = pH[0];
; 446  :     h1 = pH[1];

    ldrsh     r4, [r0, #2]

; 447  :     h2 = pH[2];

    ldrsh     r6, [r0, #4]
    ldrsh     r3, [r0]

; 448  :     h3 = pH[3];

    ldrsh     r5, [r0, #6]
    str       r0, [sp, #0x30]  ; 0x30 = 48

; 449  : 
; 450  :     mask = ((1<<(iShift+8))-1);

    add       r0, r8, #8
    mov       r1, r9, lsl r0
    sub       r2, r1, #1

; 451  :     mask = mask | (mask<<16);

    orr       r0, r2, r2, lsl #16

; 452  :     mask = ~mask;

    mvn       r0, r0
    str       r0, [sp, #0x2C]  ; 0x2C = 44

; 453  : 
; 454  :     overflow = 0;

    mov       r1, #0

; 455  : 
; 456  :     pSrc --;
; 457  :     for (i = 0; i < 8; i++) {

    mov       r8, r3
    str       r1, [sp, #0x14]  ; 0x14 = 20
    mov       r9, r4
    sub       r2, r7, #1
    mov       r12, r2
    ;str       r2, [sp, #0xC]  ; 0xC = 12
    mov       r10, r5
    mov       r11, r6
    mov       r0, #8
    str       r0, [sp, #0x1C]  ; 0x1C = 28
|$L37533|

; 458  :         for (j = 0; j < 8; j+=4) {

    mov       lr, #2
|$L37536|

; 459  :             register U32_WMV t0;
; 460  :             register U32_WMV o0;
; 461  :             register U32_WMV o1;
; 462  : 
; 463  :             t0 = pSrc[0] | (pSrc[2]<<16);
; 464  :             o0 = t0 * h0;
; 465  :             t0 = pSrc[4] | (pSrc[6]<<16);
; 466  :             o1 = t0 * h3;
; 467  :             t0 = pSrc[1] | (pSrc[3]<<16);

    ldrb      r7, [r12, #3]
    sub       lr, lr, #1
    ldrb      r0, [r12, #1]
    cmp       lr, #0

; 468  :             o0 += t0 * h1;
; 469  :             o1 += t0 * h0;
; 470  :             t0 = pSrc[2] | (pSrc[4]<<16);
; 471  :             o0 += t0 * h2;
; 472  :             o1 += t0 * h1;
; 473  :             t0 = pSrc[3] | (pSrc[5]<<16);
; 474  :             o0 += t0 * h3;
; 475  :             o1 += t0 * h2;
; 476  :             o0 += iRound32;
; 477  :             o1 += iRound32;
; 478  :             
; 479  :             overflow |= o0;
; 480  :             overflow |= o1;
; 481  : 
; 482  :             o0 >>= iShift;
; 483  :             o1 >>= iShift;
; 484  :             o0 &= 0x00ff00ff;
; 485  :             o1 &= 0x00ff00ff;
; 486  : 
; 487  :             *(U32_WMV *)pDst = o0 | o1<<8;
; 488  : 
; 489  :             pDst += 4;
; 490  :             pSrc += 4;
; 491  :         
; 492  :         }
; 493  :         pDst += (iDstStride - 8);
; 494  :         pSrc += (iSrcStride - 8);
; 495  :     }
; 496  : 
; 497  :     if(overflow&mask) {
; 498  :         g_InterpolateBicubic_Overflow_zeroYFrac_C (pSrc, iSrcStride, pDst, iDstStride, pH, iRound, iShift);

    ldrb      r6, [r12, #2]
    orr       r3, r0, r7, lsl #16
    ldrb      r0, [r12]
    orr       r1, r0, r6, lsl #16
    mul       r2, r1, r8
    ;ldr       r1, [sp, #0xC]  ; 0xC = 12
    ldrb      r4, [r12, #4]
    mla       r5, r9, r3, r2
    ldrb      r0, [r12, #6]
    orr       r1, r4, r0, lsl #16
    mul       r2, r1, r10
    orr       r0, r6, r4, lsl #16
    ldr       r6, [sp, #0x20]  ; 0x20 = 32
    mla       r3, r8, r3, r2
    mla       r1, r11, r0, r5
    mla       r3, r9, r0, r3
    ;ldr       r0, [sp, #0xC]  ; 0xC = 12
    orr       r5, r6, r6, lsl #16
    ldrb      r0, [r12, #5]
    orr       r2, r7, r0, lsl #16
    ldr       r7, [sp, #0x18]  ; 0x18 = 24
    mla       r0, r11, r2, r3
    mla       r1, r10, r2, r1
    add       r3, r0, r5
    add       r4, r1, r5
    ldr       r5, [sp, #0x14]  ; 0x14 = 20
    mov       r1, r3, lsr r7
    orr       r0, r3, r4
    orr       r5, r0, r5
    mov       r0, r1, lsl #8
    str       r5, [sp, #0x14]  ; 0x14 = 20
    mov       r1, #0xFF, 16  ; 0xFF0000 = 16711680
    orr       r1, r1, #0xFF  ; 0xFF = 255
    eor       r2, r0, r4, lsr r7
    ldr       r4, [sp, #0x10]  ; 0x10 = 16
    mov       r0, r3, lsr r7
    and       r1, r2, r1
    ;ldr       r2, [sp, #0xC]  ; 0xC = 12
    eor       r1, r1, r0, lsl #8
    str       r1, [r4], #4
    add       r12, r12, #4
    ;str       r2, [sp, #0xC]  ; 0xC = 12
    str       r4, [sp, #0x10]  ; 0x10 = 16
    bhi       |$L37536|  ; 000000B8
    ldr       r3, [sp, #0x24]  ; 0x24 = 36
    ldr       r1, [sp, #0x28]  ; 0x28 = 40
    sub       r0, r3, #8
    add       r4, r4, r0
    sub       r0, r1, #8
    str       r4, [sp, #0x10]  ; 0x10 = 16
    add       r12, r12, r0
    ldr       r0, [sp, #0x1C]  ; 0x1C = 28
    ;str       r2, [sp, #0xC]  ; 0xC = 12
    sub       r0, r0, #1
    str       r0, [sp, #0x1C]  ; 0x1C = 28
    cmp       r0, #0
    bhi       |$L37533|  ; 000000B4
    ldr       r0, [sp, #0x2C]  ; 0x2C = 44
    tst       r0, r5
    ldrne     r0, [sp, #0x30]  ; 0x30 = 48
    movne     r2, r4
    strne     r7, [sp, #8]
    strne     r0, [sp]
    ;ldrne     r0, [sp, #0xC]  ; 0xC = 12
    movne     r0, r12
    strne     r6, [sp, #4]
    blne      g_InterpolateBicubic_Overflow_zeroYFrac_C  ; 000001D4

; 499  :     }
; 500  : }

    add       sp, sp, #0x34  ; 0x34 = 52
    ldmia     sp, {r4 - r11, sp, pc}  ; ldmfd

|$M37888|

    ENDP  ; |g_InterpolateBlockBicubic_10_C|

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

|g_InterpolateBlockBicubic_11_C| PROC

; 503  : {

    mov       r12, sp
    stmdb     sp!, {r0 - r3}  ; stmfd
    stmdb     sp!, {r4 - r12, lr}  ; stmfd
    FRAME_PROFILE_COUNT
    sub       sp, sp, #0xF0  ; 0xF0 = 240
|$M37954|
    mov       r8, r1
    mov       r12, r1
    str       r2, [sp, #0x24]  ; 0x24 = 36
    mov       r6, r0
    ;str       r8, [sp, #0x18]  ; 0x18 = 24
    str       r3, [sp, #0x4C]  ; 0x4C = 76

; 504  :     I16_WMV v0;
; 505  :     I16_WMV v1;
; 506  :     I16_WMV v2;
; 507  :     I16_WMV v3;
; 508  :     I16_WMV h0;
; 509  :     I16_WMV h1; 
; 510  :     I16_WMV h2;
; 511  :     I16_WMV h3;
; 512  :     I16_WMV pFilter[(32+4)*2];
; 513  :     I32_WMV i, j;
; 514  :     I32_WMV iRound1;
; 515  :     I32_WMV iRound2;
; 516  :     U32_WMV uRound1_32;
; 517  :     I32_WMV mask, iRound2_32;
; 518  :     U32_WMV overflow = 0;

    mov       r0, #0

; 519  :     
; 520  :     I16_WMV *pH = gaSubsampleBicubic + 4 * iXFrac;

    ldr       r1, [sp, #0x128]  ; 0x128 = 296
    str       r0, [sp, #0x20]  ; 0x20 = 32

; 521  :     I16_WMV *pV = gaSubsampleBicubic + 4 * iYFrac;
; 522  :     I32_WMV iShift = ((iXFrac == 2) ? 4 : 6) + ((iYFrac == 2) ? 4 : 6) - 7;

    mov       r3, #4
    ldr       r0, =gaSubsampleBicubic ;;;;;;[pc, #0x35C]  ;  pc+8+860 = 00000398
    cmp       r1, #2
    mov       r4, #6
    add       r2, r0, r1, lsl #3
    str       r2, [sp, #0x38]  ; 0x38 = 56

; 523  :     mask = ~(((1<<iShift)-1)<<(16-iShift));

    mov       r7, #1
    ldr       r2, [sp, #0x12C]  ; 0x12C = 300
    add       r0, r0, r2, lsl #3
    str       r0, [sp, #0x40]  ; 0x40 = 64
    mov       r0, r3
    movne     r0, r4
    cmp       r2, #2
    movne     r3, r4
    add       r0, r3, r0
    sub       r4, r0, #7
    mov       r0, r7, lsl r4
    str       r4, [sp, #0x28]  ; 0x28 = 40
    rsb       r1, r4, #0x10  ; 0x10 = 16
    sub       r2, r0, #1
    mvn       r0, r2, lsl r1
    str       r0, [sp, #0x34]  ; 0x34 = 52

; 524  : 
; 525  :     v0 = pV[0];

    ldr       r0, [sp, #0x40]  ; 0x40 = 64
    ldrsh     r5, [r0]

; 526  :     v1 = pV[1];

    ldrsh     r3, [r0, #2]

; 527  :     v2 = pV[2];
; 528  :     v3 = pV[3];

    ldrsh     r1, [r0, #6]
    ldrsh     r2, [r0, #4]

; 529  :     iRound1 = (1 << (iShift - 1)) - 1 + iRndCtrl;

    sub       r0, r4, #1
    ldr       r4, [sp, #0x130]  ; 0x130 = 304

; 531  :     pSrc -= (iSrcStride + 1);
; 532  :     for (i = 0; i < 8; i++) {

    mov       lr, r1
    add       r0, r4, r7, lsl r0
    sub       r0, r0, #1
    add       r7, r0, r0, lsl #16
    str       r0, [sp, #0x54]  ; 0x54 = 84
    sub       r0, r6, r8
    str       r7, [sp, #0x58]  ; 0x58 = 88
    sub       r0, r0, #1
    str       r0, [sp, #0x1C]  ; 0x1C = 28
    mov       r6, r2
    mov       r9, r3
    mov       r11, r5
    ldr       r5, [sp, #0x20]  ; 0x20 = 32
    str       r6, [sp, #0x44]  ; 0x44 = 68
    rsb       r1, r4, #0x40  ; 0x40 = 64
    str       r9, [sp, #0x50]  ; 0x50 = 80
    mov       r0, #8

; 602  :             o0 += t0 * h1;
; 603  :             o1 += t0 * h0;

    str       r1, [sp, #0x2C]  ; 0x2C = 44
    str       r11, [sp, #0x48]  ; 0x48 = 72
    str       r0, [sp, #0x30]  ; 0x30 = 48
    b         |$L37579|  ; 00000118
|$L37953|

; 530  :     uRound1_32 = iRound1 + (iRound1<<16);

    ldr       r7, [sp, #0x58]  ; 0x58 = 88
    ldr       r6, [sp, #0x44]  ; 0x44 = 68
    ldr       r9, [sp, #0x50]  ; 0x50 = 80
    ldr       r11, [sp, #0x48]  ; 0x48 = 72
|$L37579|

; 533  :         U32_WMV * pF32 = (U32_WMV *) pFilter;

    ldr       r4, [sp, #0x1C]  ; 0x1C = 28
    add       r8, sp, #0x60  ; 0x60 = 96
    mov       r10, #3
|$L37585|

; 534  :         U8_WMV * pF; 
; 535  : 
; 536  :         for (j = 0; j < 12; j+=4) {
; 537  :             
; 538  :             register U32_WMV t0;
; 539  :             register I32_WMV o0;
; 540  : 
; 541  :             o0 = uRound1_32;
; 542  :             t0 = pSrc[0] | (pSrc[2]<<16);
; 543  :             o0 += t0 * v0;

    ;ldr       r0, [sp, #0x18]  ; 0x18 = 24
    sub       r10, r10, #1
    ldrb      r1, [r4, #2]
    cmp       r10, #0
    ldrb      r0, [r4], +r12
    orr       r2, r0, r1, lsl #16

; 544  :             pSrc += iSrcStride;
; 545  :             t0 = pSrc[0] | (pSrc[2]<<16);
; 546  :             o0 += t0 * v1;

    ;ldr       r0, [sp, #0x18]  ; 0x18 = 24
    ldrb      r1, [r4, #2]
    mla       r3, r2, r11, r7
    ldrb      r0, [r4], +r12
    orr       r2, r0, r1, lsl #16

; 547  :             pSrc += iSrcStride;
; 548  :             t0 = pSrc[0] | (pSrc[2]<<16);
; 549  :             o0 += t0 * v2;

    ;ldr       r0, [sp, #0x18]  ; 0x18 = 24
    ldrb      r1, [r4, #2]
    mla       r3, r2, r9, r3
    ldrb      r0, [r4], +r12
    orr       r2, r0, r1, lsl #16

; 550  :             pSrc += iSrcStride;
; 551  :             t0 = pSrc[0] | (pSrc[2]<<16);
; 552  :             o0 += t0 * v3;

    ldrb      r1, [r4, #2]
    ldrb      r0, [r4]
    mla       r3, r2, r6, r3
    orr       r2, r0, r1, lsl #16

; 553  : 
; 554  :             //o0  =  ( t0 * v0 + t1 * v1 + t2 * v2 + t3 * v3 + uRound1_32);
; 555  :             overflow |= o0;
; 556  :             o0 >>= iShift;
; 557  :             o0 &= mask;

    ldr       r1, [sp, #0x34]  ; 0x34 = 52
    mla       r3, r2, lr, r3
    ldr       r2, [sp, #0x28]  ; 0x28 = 40
    and       r0, r1, r3, asr r2

; 558  : 
; 559  :             *(I32_WMV *)pF32 = o0 ;
; 560  :             o0 = uRound1_32;
; 561  :             t0 = pSrc[1] | (pSrc[3]<<16);
; 562  :             o0 += t0 * v3;

    ldrb      r1, [r4, #3]
    str       r0, [r8]
    orr       r5, r3, r5
    ldrb      r0, [r4, #1]
    orr       r2, r0, r1, lsl #16

; 563  :             pSrc -= iSrcStride;

    ;ldr       r0, [sp, #0x18]  ; 0x18 = 24
    mla       r3, r2, lr, r7
    sub       r4, r4, r12

; 564  :             t0 = pSrc[1] | (pSrc[3]<<16);
; 565  :             o0 += t0 * v2;

    ldrb      r1, [r4, #3]
    ldrb      r0, [r4, #1]
    orr       r2, r0, r1, lsl #16

; 566  :             pSrc -= iSrcStride;

    ;ldr       r0, [sp, #0x18]  ; 0x18 = 24
    mla       r3, r2, r6, r3
    sub       r4, r4, r12

; 567  :             t0 = pSrc[1] | (pSrc[3]<<16);
; 568  :             o0 += t0 * v1;

    ldrb      r1, [r4, #3]
    ldrb      r0, [r4, #1]
    orr       r2, r0, r1, lsl #16

; 569  :             pSrc -= iSrcStride;

    ;ldr       r0, [sp, #0x18]  ; 0x18 = 24
    mla       r3, r2, r9, r3
    sub       r4, r4, r12

; 570  :             t0 = pSrc[1] | (pSrc[3]<<16);
; 571  :             o0 += t0 * v0;

    ldrb      r1, [r4, #3]
    ldrb      r0, [r4, #1]

; 572  : 
; 573  :             //o0  =  ( t0 * v0 + t1 * v1 + t2 * v2 + t3 * v3 + uRound1_32);
; 574  :             overflow |= o0;
; 575  :             o0 >>= iShift;
; 576  :             o0 &= mask;
; 577  : 
; 578  :             *(I32_WMV *)(pF32 + PFILTERODD_OFFSET32) = o0 ;
; 579  : 
; 580  :             pF32 ++;
; 581  :             pSrc +=  4;

    add       r4, r4, #4
    str       r4, [sp, #0x1C]  ; 0x1C = 28
    orr       r2, r0, r1, lsl #16
    ldr       r1, [sp, #0x34]  ; 0x34 = 52
    mla       r3, r2, r11, r3
    ldr       r2, [sp, #0x28]  ; 0x28 = 40
    and       r0, r1, r3, asr r2
    orr       r5, r3, r5
    str       r0, [r8, #0x48]  ; 0x48 = 72
    str       r5, [sp, #0x20]  ; 0x20 = 32
    add       r8, r8, #4
    bhi       |$L37585|  ; 00000124

; 582  : 
; 583  :         }
; 584  :         
; 585  :         h0 = pH[0];

    ldr       r4, [sp, #0x38]  ; 0x38 = 56

; 586  :         h1 = pH[1];
; 587  :         h2 = pH[2];
; 588  :         h3 = pH[3];
; 589  :         iRound2 = 64 - iRndCtrl;
; 590  :         iRound2_32 = iRound2 + (iRound2<<16);
; 591  :         pF = (U8_WMV *)pFilter;

    add       r11, sp, #0x60  ; 0x60 = 96
    ldrsh     r0, [r4]
    ldrsh     r1, [r4, #2]

; 592  :         for (j = 0; j < 8; j += 4) 

    mov       r7, r0
    ldrsh     r2, [r4, #4]
    mov       r8, r1
    ldrsh     r3, [r4, #6]
    mov       r9, r2
    mov       r10, r3
    mov       r0, #2
    str       r0, [sp, #0x3C]  ; 0x3C = 60
|$L37593|

; 593  :         {
; 594  :             register I32_WMV o0;
; 595  :             register I32_WMV o1;
; 596  :             register U32_WMV t0;
; 597  :             o0 = iRound2_32;
; 598  :             o1 = iRound2_32;
; 599  :             t0 = *(I32_WMV *)pF;
; 600  :             o0 += t0 * h0;

    ldr       r2, [sp, #0x2C]  ; 0x2C = 44
    ldr       r0, [r11]
    add       r1, r2, r2, lsl #16

; 601  :             t0 = *(I32_WMV *)(pF + PFILTERODD_OFFSET8); 

    ldr       r2, [r11, #0x48]  ; 0x48 = 72
    mla       r1, r0, r7, r1

; 604  :             t0 = *(U16_WMV *)(pF+2) |  (*(U16_WMV *)(pF+4)) <<16;

    ldrh      r3, [r11, #4]

; 605  :             o0 += t0 * h2;
; 606  :             o1 += t0 * h1;
; 607  :             t0 = *(U16_WMV *)(pF + PFILTERODD_OFFSET8 + 2) |  (*(U16_WMV *)(pF + PFILTERODD_OFFSET8 +4)) <<16;

    ldrh      r6, [r11, #0x4C]  ; 0x4C = 76
    mla       r4, r8, r2, r1
    ldr       r1, [sp, #0x2C]  ; 0x2C = 44
    add       r0, r1, r1, lsl #16
    mla       r5, r7, r2, r0
    mov       r0, r3, lsl #16
    mov       r1, r0, lsr #16
    ldrh      r0, [r11, #2]
    mov       r2, r1, lsl #16
    mov       r1, r0, lsl #16
    mov       r0, r6, lsl #16
    orr       r3, r2, r1, lsr #16
    mov       r1, r0, lsr #16
    ldrh      r0, [r11, #0x4A]  ; 0x4A = 74
    mla       r5, r8, r3, r5
    mla       r4, r9, r3, r4
    mov       r2, r1, lsl #16
    mov       r1, r0, lsl #16

; 608  :             o0 += t0 * h3;
; 609  :             o1 += t0 * h2;            
; 610  :             t0 = *(I32_WMV *)(pF+4);
; 611  :             o1 += t0 * h3;

    ldr       r0, [r11, #4]!
    orr       r3, r2, r1, lsr #16
    mla       r1, r9, r3, r5

; 612  : 
; 613  :             //o0 = t0 * h0  + t1 * h1 + t2 * h2 + t3 * h3 + iRound2_32;
; 614  :             //o1 = t1 * h0  + t2 * h1 + t3 * h2 + t4 * h3 + iRound2_32;
; 615  : 
; 616  :             overflow |= o0;
; 617  :             overflow |= o1;

    ldr       r5, [sp, #0x20]  ; 0x20 = 32
    mla       r2, r10, r3, r4
    mla       r3, r0, r10, r1

; 618  :             o0 >>= 7;
; 619  :             o1 >>= 7;
; 620  :             o0 &= 0x00ff00ff;           
; 621  :             o1 &= 0x00ff00ff;
; 622  : 
; 623  :             *(U32_WMV *)pDst = o0| o1<<8;
; 624  :             pDst += 4;
; 625  :             pF += 4;
; 626  :         }
; 627  : 
; 628  :         pSrc += (iSrcStride - 12);
; 629  :         pDst += (iDstStride - 8);
; 630  :     }
; 631  : 
; 632  :     if( (overflow&0x80008000) )
; 633  :     {
; 634  :         g_InterpolateBicubic_Overflow_C (pSrc, iSrcStride, pDst, iDstStride, pFilter, pV, pH, iRound1, iRound2, iShift);

    orr       r0, r3, r2
    mov       r1, r3, asr #7
    orr       r5, r0, r5
    mov       r0, r1, lsl #8
    str       r5, [sp, #0x20]  ; 0x20 = 32
    mov       r1, #0xFF, 16  ; 0xFF0000 = 16711680
    orr       r1, r1, #0xFF  ; 0xFF = 255
    eor       r2, r0, r2, asr #7
    mov       r0, r3, asr #7
    and       r1, r2, r1
    ldr       r2, [sp, #0x24]  ; 0x24 = 36
    eor       r1, r1, r0, lsl #8
    ldr       r0, [sp, #0x3C]  ; 0x3C = 60
    str       r1, [r2], #4
    sub       r0, r0, #1
    str       r2, [sp, #0x24]  ; 0x24 = 36
    cmp       r0, #0
    str       r0, [sp, #0x3C]  ; 0x3C = 60
    bhi       |$L37593|  ; 0000024C
    ;ldr       r1, [sp, #0x18]  ; 0x18 = 24
    mov       r1, r12
    ldr       r4, [sp, #0x1C]  ; 0x1C = 28
    sub       r0, r12, #0xC  ; 0xC = 12
    ldr       r3, [sp, #0x4C]  ; 0x4C = 76
    add       r4, r4, r0
    sub       r0, r3, #8
    str       r4, [sp, #0x1C]  ; 0x1C = 28
    add       r2, r2, r0
    ldr       r0, [sp, #0x30]  ; 0x30 = 48
    str       r2, [sp, #0x24]  ; 0x24 = 36
    sub       r0, r0, #1
    str       r0, [sp, #0x30]  ; 0x30 = 48
    cmp       r0, #0
    bhi       |$L37953|  ; 00000108
    mov       r0, #2, 2  ; 0x80000000 = 2147483648
    orr       r0, r0, #2, 18  ; 0x8000 = 32768
    tst       r5, r0
    beq       |$L37607|  ; 00000390
    ldr       r0, [sp, #0x28]  ; 0x28 = 40
    str       r0, [sp, #0x14]  ; 0x14 = 20
    ldr       r0, [sp, #0x2C]  ; 0x2C = 44
    str       r0, [sp, #0x10]  ; 0x10 = 16
    ldr       r0, [sp, #0x54]  ; 0x54 = 84
    str       r0, [sp, #0xC]  ; 0xC = 12
    ldr       r0, [sp, #0x38]  ; 0x38 = 56
    str       r0, [sp, #8]
    ldr       r0, [sp, #0x40]  ; 0x40 = 64
    str       r0, [sp, #4]
    add       r0, sp, #0x60  ; 0x60 = 96
    str       r0, [sp]
    mov       r0, r4
    bl        g_InterpolateBicubic_Overflow_C  ; 00000394
|$L37607|

; 635  :     }
; 636  : }

    add       sp, sp, #0xF0  ; 0xF0 = 240
    ldmia     sp, {r4 - r11, sp, pc}  ; ldmfd
;|$L37958|
;   DCD       |gaSubsampleBicubic|
|$M37955|

    ENDP  ; |g_InterpolateBlockBicubic_11_C|

|g_InterpolateBlockBicubic_xx_EMB| PROC

; 621  : {

    stmdb     sp!, {r4 - r12, lr}
    FRAME_PROFILE_COUNT
    sub       sp, sp, #0x84
|$M39327|
    mov       r6, r1
    str       r3, [sp, #0x30]
    mov       r4, r0
    str       r6, [sp, #8]

; 622  :     I32_WMV i;
; 623  :     I32_WMV pFilter[16+3];
; 624  :     U32_WMV overflow = 0;

    ldr       r12, =0x00ff00ff
    mov       r5, #0
    mov       r0, r5
    str       r0, [sp, #0x20]

; 625  :     
; 626  :     const I16_WMV *pH = gaSubsampleBicubic + 4 * iXFrac;

    ldr       r0, =gaSubsampleBicubic ;[pc, #0x308]
    add       r1, r0, r3, lsl #3
    str       r1, [sp, #0x28]

; 627  :     const I16_WMV *pV = gaSubsampleBicubic + 4 * iYFrac;

    ldr       r1, [sp, #0xAC] ;ldr       r1, [sp, #0xA8]
    add       r9, r0, r1, lsl #3

; 628  : 
; 629  :     assert((iYFrac&1)==1);
; 630  :     assert((iXFrac&1)==1);
; 631  : 
; 632  :    pSrc -= (iSrcStride + 1);

    ldr       r1, [sp, #0xB0] ;ldr       r1, [sp, #0xAC]
    sub       r0, r4, r6
    mov       r4, #1, 10
    orr       r4, r4, #0x40
    sub       r3, r0, #1
    rsb       r0, r1, r4

; 633  : 
; 634  :     for (i = 0; i < 8; i++) 

    ldr       r4, =g_VertFilterStepTbl ;[pc, #0x2D8]
    add       r1, sp, #0x38

; 638  :         overflow |= g_horzFilterX((U8_WMV *)pFilter, pDst, 7, 0x00400040 - iRndCtrl, pH);

    str       r0, [sp, #0x1C]
    sub       r0, r2, r1
    str       r3, [sp, #0x14]
    str       r0, [sp, #0x18]
    mov       r0, #8
    str       r0, [sp, #0x24]
    add       r0, r2, #0x40
    str       r0, [sp, #0x34]
    b         |$L38097|
|$L39326|

; 628  : 
; 629  :     assert((iYFrac&1)==1);
; 630  :     assert((iXFrac&1)==1);
; 631  : 
; 632  :    pSrc -= (iSrcStride + 1);

    ldr       r3, [sp, #0x14]
    mov       r5, #0
|$L38097|

; 635  :    {
; 636  : 
; 637  :         overflow |= g_VertFilterX(pSrc, iSrcStride, (U8_WMV *)pFilter, 5, 0x000f000f + iRndCtrl,  pV, 3,  0xffff07ff);

    mov       r1, r5
    ldrsh     r2, [r9, #4]
    add       r0, sp, #0x38
    str       r1, [sp, #0x10]
    str       r0, [sp, #0xC]
    mov       r11, r3
    ldrsh     r0, [r9]
    mov       lr, r4
    ldrsh     r1, [r9, #2]
    mov       r10, #6
    ldrsh     r3, [r9, #6]
    mov       r0, r0, lsl #16
    mov       r1, r1, lsl #16
    mov       r5, r0, asr #16
    mov       r6, r1, asr #16
    mov       r0, r2, lsl #16
    mov       r1, r3, lsl #16
    mov       r7, r0, asr #16
    mov       r8, r1, asr #16
|$L39255|
    ldr       r2, [sp, #8]
    ldrb      r1, [r11, #2]
    ;cmp       r10, #0
    ldrb      r0, [r11], +r2
        subs       r10, r10, #1
    orr       r2, r0, r1, lsl #16
    ldrb      r1, [r11, #2]
    mul       r3, r2, r5
    ldr       r2, [sp, #8]
    ldrb      r0, [r11], +r2
    
    orr       r2, r0, r1, lsl #16
    ldrb      r1, [r11, #2]
    mla       r3, r2, r6, r3
    ldr       r2, [sp, #8]
    ldrb      r0, [r11], +r2
    
    orr       r2, r0, r1, lsl #16
    ldrb      r1, [r11, #2]
    mla       r3, r2, r7, r3
    ldrb      r0, [r11]

    orr       r2, r0, r1, lsl #16

; 639  : 
; 640  :         pSrc += iSrcStride;
; 641  :         pDst += 8;
; 642  :     }
; 643  : 
; 644  :     DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)
; 645  :     if(overflow & 0x80008000)
; 646  :     {
; 647  :         DEBUG_STATEMENT( g_BiCubic_xx_Overflow_cnt++;)
; 648  :         pSrc -= 7*iSrcStride - 1;
; 649  :         pDst -= 64;
; 650  :         g_InterpolateBlockBicubic_EMB(pSrc, iSrcStride, pDst,  iXFrac,  iYFrac,  iRndCtrl);

    mla       r3, r2, r8, r3
        ldr       r1, [sp, #0xB0] ;ldr       r1, [sp, #0xAC]
    
    add       r0, r3, r1
    ldr       r3, [sp, #0x10]
    add       r1, r0, #0xF, 16
    ldr       r0, =0x7ff07ff ;[pc, #0x1F0]
    add       r2, r1, #0xF
    orr       r3, r2, r3
    and       r0, r0, r2, lsr #5
    ldr       r2, [sp, #0xC]
    str       r3, [sp, #0x10]
    ldr       r3, [sp, #8]
    str       r0, [r2]
    ldrsb     r1, [lr]
    add       r2, r3, r3, lsl #1
    ldr       r3, [sp, #0x10]
    sub       r0, r1, r2
    ldrsb     r1, [lr, #1]
    ldr       r2, [sp, #0xC]
    add       r11, r11, r0
    add       lr, lr, #2
    add       r2, r2, r1
    str       r2, [sp, #0xC]
    bne       |$L39255|
    ldr       r1, [sp, #0x20]
    add       r6, sp, #0x38
    ldr       r5, [sp, #0x28]
    orr       r0, r3, r1
    str       r6, [sp, #0xC]
    mov       r1, #0
    str       r0, [sp, #0x2C]
    mov       r0, r1
    ldrsh     r2, [r5, #4]
    str       r0, [sp, #0x10]
    ldrsh     r0, [r5]
    ldrsh     r1, [r5, #2]
    mov       r0, r0, lsl #16
    ldrsh     r3, [r5, #6]
    mov       r8, r0, asr #16
    mov       r1, r1, lsl #16
    mov       r0, r2, lsl #16
    mov       r10, r1, asr #16
    mov       r11, r0, asr #16
    mov       r1, r3, lsl #16
    mov       r0, #2
    mov       lr, r1, asr #16
    str       r0, [sp, #0x20]
|$L39272|
    ldr       r0, [r6]
    ldr       r1, [sp, #0x1C]
    ldr       r2, [r6, #0x40]
    mla       r1, r0, r8, r1
    ldr       r0, [sp, #0x1C]
    ldrh      r3, [r6, #4]
    ldrh      r7, [r6, #0x44]
    mla       r6, r8, r2, r0
    mov       r0, r3, lsl #16
    mla       r5, r10, r2, r1
    mov       r1, r0, lsr #16
    ldr       r0, [sp, #0xC]
    mov       r2, r1, lsl #16
    ldrh      r0, [r0, #2]
    
    mov       r1, r0, lsl #16
    mov       r0, r7, lsl #16
    ldr       r7, [sp, #0x10]
    orr       r3, r2, r1, lsr #16
    mov       r1, r0, lsr #16
    ldr       r0, [sp, #0xC]
    mla       r6, r10, r3, r6
    ldrh      r0, [r0, #0x42]
    mov       r2, r1, lsl #16
    mla       r5, r11, r3, r5
    mov       r1, r0, lsl #16
    orr       r3, r2, r1, lsr #16
    mla       r1, r11, r3, r6
    ldr       r6, [sp, #0xC]
    mla       r5, lr, r3, r5
    ldr       r0, [r6, #4]
    mla       r3, r0, lr, r1
    
    
    orr       r0, r3, r5
    orr       r7, r0, r7
    ldr       r0, [sp, #0x18]
    str       r7, [sp, #0x10]
    mov       r2, r0
    ;mov       r0, #0xFF, 16
    ;orr       r0, r0, #0xFF
    and       r0, r12, r5, asr #7
    str       r0, [r2, +r6]!
    ;mov       r0, #0xFF, 16
    ;orr       r0, r0, #0xFF
    ;ldr       r0, =0x00ff00ff 
    and       r1, r12, r3, asr #7
    ldr       r0, [sp, #0x20]
    add       r6, r6, #4
    str       r1, [r2, #0x40]
    subs       r0, r0, #1
    str       r6, [sp, #0xC]
    str       r0, [sp, #0x20]
    ;cmp       r0, #0
    bne       |$L39272|
    ldr       r0, [sp, #0x2C]
    ldr       r5, [sp, #0x14]
    orr       r1, r7, r0
    ldr       r0, [sp, #0x18]
    ldr       r6, [sp, #8]
    add       r0, r0, #8
    str       r1, [sp, #0x20]
    str       r0, [sp, #0x18]
    add       r5, r5, r6
    ldr       r0, [sp, #0x24]
    str       r5, [sp, #0x14]
    subs       r0, r0, #1
    str       r0, [sp, #0x24]
    ;cmp       r0, #0
    bne       |$L39326|
    mov       r0, #2, 2
    orr       r0, r0, #2, 18
    tst       r1, r0
    beq       |$L38102|
    ldr       r0, [sp, #0xB0] ;ldr       r0, [sp, #0xAC]
    mov       r1, r6
    ldr       r3, [sp, #0x30]
    str       r0, [sp, #4]
    ldr       r0, [sp, #0xAC] ;ldr       r0, [sp, #0xA8]
    str       r0, [sp]
    ldr       r0, [sp, #0x34]
    sub       r2, r0, #0x40
    rsb       r0, r6, r6, lsl #3
    sub       r0, r5, r0
    add       r0, r0, #1
    bl        g_InterpolateBlockBicubic_EMB
|$L38102|

; 651  :     }
; 652  : 
; 653  : }

    add       sp, sp, #0x84
    ldmia     sp!, {r4 - r12, pc}
|$L39331|
    ;DCD       0x7ff07ff
    ;DCD       |g_VertFilterStepTbl|
    ;DCD       |gaSubsampleBicubic|
|$M39328|

    ENDP  ; |g_InterpolateBlockBicubic_xx_EMB|

    ENDIF ; WMV_OPT_MOTIONCOMP_ARM= 1

    ENDIF

    END 
