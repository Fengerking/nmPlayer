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
;//     idctARM.s
;//
;// Abstract:
;// 
;//     ARM specific transforms
;//     Optimized assembly routines to implement WMV9 8x8, 4x8, 8x4, 4x4 IDCT
;//
;//     Custom build with 
;//          armasm $(InputDir)\$(InputName).s $(OutDir)\$(InputName).obj
;//     and
;//          $(OutDir)\$(InputName).obj
;// 
;// Author:
;// 
;//     Chuang Gu (chuanggu@microsoft.com) Nov. 10, 2002
;//
;// Revision History:
;//
;//*************************************************************************
;//
;// r0 : x0, r1 : x1, ..., r8 : x8;
;// r14: blk[]
;// r9, r10, r11, r12 -> temporal registers
;//
;//*************************************************************************


    INCLUDE wmvdec_member_arm.inc
    INCLUDE xplatform_arm_asm.h 
    IF UNDER_CE != 0
    INCLUDE kxarm.h
    ENDIF 

    AREA |.text|, CODE, READONLY
    
    IF WMV_OPT_IDCT_ARM = 1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   WMV9 transform
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	EXPORT  g_IDCTDec_WMV3_Pass1_ARMV4
	EXPORT  g_IDCTDec_WMV3_Pass2_ARMV4
	EXPORT  g_IDCTDec_WMV3_Pass3_ARMV4
	EXPORT  g_IDCTDec_WMV3_Pass4_ARMV4

    EXPORT  SignPatch_ARMV4
    EXPORT  SignPatch32_ARMV4
    EXPORT  g_SubBlkIDCTClear_EMB_ARMV4

    EXPORT  g_IDCTDec16_WMV3_SSIMD_ARMV4

    EXPORT  g_IDCTDec_WMV3_Pass1_Naked_ARMV4
    EXPORT  g_IDCTDec_WMV3_Pass2_Naked_ARMV4
    EXPORT  g_IDCTDec_WMV3_Pass3_Naked_ARMV4
    EXPORT  g_IDCTDec_WMV3_Pass4_Naked_ARMV4

;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
	ALIGN 8	
Pass1_table	
		dcd 15,12,24,20,6,4				
	ALIGN 8	
Pass2_table	
		dcd -3,6,-5,7,-11,-10,-12,-6				
	ALIGN 8	
Pass3_table	
		dcd 10,17,-32,12				

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA    |.text|, CODE
    WMV_LEAF_ENTRY g_IDCTDec16_WMV3_SSIMD_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; r0 = piDst
; r1 = piSrc
; r2 = iOffsetToNextRowForDCT
; r3 = iDCTHorzFlags

ST_piDst_4x4IDCTDec       EQU   0
ST_tmpBuffer_4x4IDCTDec   EQU   4

ST_SIZE_4x4IDCTDec        EQU   260

    stmdb     sp!, {r4 - r12, lr}
    FRAME_PROFILE_COUNT

    sub       sp, sp, #ST_SIZE_4x4IDCTDec
    str       r0, [sp, #ST_piDst_4x4IDCTDec]

;   g_IDCTDec_WMV3_Pass1(piSrc0, blk32, 4, iDCTHorzFlags);
    mov       r0, r1
    mov       r2, #4
    add       r1, sp, #ST_tmpBuffer_4x4IDCTDec
    bl        g_IDCTDec_WMV3_Pass1_Naked_ARMV4

;   g_IDCTDec_WMV3_Pass2(piSrc0, blk16, 4);
    mov       r2, #4
    ldr       r1, [sp, #ST_piDst_4x4IDCTDec]
    add       r0, sp, #ST_tmpBuffer_4x4IDCTDec
    bl        g_IDCTDec_WMV3_Pass2_Naked_ARMV4

    add       sp, sp, #ST_SIZE_4x4IDCTDec
    ldmia     sp!, {r4 - r12, pc}
    WMV_ENTRY_END


;Void_WMV g_SubBlkIDCTClear_EMB_ARMV4 (UnionBuffer * piDstBuf,  I32_WMV iIdx)

    AREA |.text|, CODE, READONLY
    WMV_LEAF_ENTRY g_SubBlkIDCTClear_EMB_ARMV4
;r3  = iStep
;r12 = iNumLoop

    stmdb     sp!, {lr}  ; stmfd

    FRAME_PROFILE_COUNT

;I32_WMV iNumLoop = 8<<(iIdx>>7);
    mov       r3, r1, asr #7
    mov       r2, #8
    mov       r12,r2, lsl r3

;I32_WMV iStep   =  1<< ((iIdx >>6)&0x1);
    mov       r3, r1, asr #6
    mov       r2, #1
    and       r3, r3, #1
    mov       r3, r2, lsl r3

;I32_WMV iStep2 = iStep<<1;
;I32_WMV* piDst = piDstBuf->i32 + (iIdx&0x3f);
    and       r2, r1, #0x3F  ; 0x3F = 63
    mov       lr, #0
    add       r0, r0, r2, lsl #2
    mov       r2, r3, lsl #3
    add       r1, r0, r3, lsl #2

;for (i = 0; i < iNumLoop; i+=iStep2) 
g_SubBlkLoop

;       piDst[i] = 0;
;       piDst[i+16] = 0;
    str       lr, [r0]
    subs      r12,r12,r3, lsl #1
    str       lr, [r0, #0x40]  ; 0x40 = 64

;       piDst[i + iStep] = 0;
;       piDst[i+16 + iStep] = 0;
    str       lr, [r1]
    add       r0, r0, r2
    str       lr, [r1, #0x40]

    add       r1, r1, r2
    bgt       g_SubBlkLoop

    ldmia     sp!, {pc}  ; ldmfd
    WMV_ENTRY_END	;

    ENDP  ; |g_SubBlkIDCTClear_EMB|

    
    WMV_LEAF_ENTRY SignPatch_ARMV4

; 1341 : {

    stmdb     sp!, {r4 - r6, lr}
    FRAME_PROFILE_COUNT
|$M37638|
    mov       r4, r0

; 1342 :     
; 1343 :     int i;
; 1344 : 
; 1345 :     I16_WMV *piSrcTmp = rgiCoefRecon;
; 1346 :   //  I16_WMV * dsttmp = g_dsttmp;
; 1347 : 
; 1348 : 
; 1349 :      I16_WMV * dsttmp = rgiCoefRecon;
; 1350 : 
; 1351 : //    memset(dsttmp, 0, 128);
; 1352 : 
; 1353 :     for(i=0;i<(len/2);i+=2)

    mov       r3, r1, asr #2
    mov       r3, r3, asr #2
    sub       r4, r4, #16

|$L37076|

; 1354 :     {
; 1355 :         I16_WMV data_odd = piSrcTmp[i+1];
; 1356 :        
; 1357 :             *(I32_WMV *)(dsttmp + i) =  (I32_WMV) (piSrcTmp[i]);

    ldrsh     r0, [r4, #16]!
    ldrsh     r2, [r4, #2]
    ldrsh     r5, [r4, #4]
    ldrsh     r6, [r4, #6]
    

; 1358 :         
; 1359 :             dsttmp[i+1]  +=  data_odd;

    add       r1, r2, r0, asr #31 
    strh      r1, [r4, #2]

; 1358 :         
; 1359 :             dsttmp[i+1]  +=  data_odd;

    add       r1, r6, r5, asr #31 
    strh      r1, [r4, #6]

    ldrsh     r0, [r4, #8]
    ldrsh     r2, [r4, #10]
    ldrsh     r5, [r4, #12]
    ldrsh     r6, [r4, #14]

; 1358 :         
; 1359 :             dsttmp[i+1]  +=  data_odd;

    add       r1, r2, r0, asr #31 
    strh      r1, [r4, #10]

; 1358 :         
; 1359 :             dsttmp[i+1]  +=  data_odd;

    add       r1, r6, r5, asr #31 
    strh      r1, [r4, #14]
    
    subs      r3, r3, #1
    bhi       |$L37076|
|$L37078|

; 1360 :     }
; 1361 : 
; 1362 :    
; 1363 : }

    ldmia     sp!, {r4 - r6, pc}
|$M37639|
    WMV_ENTRY_END
    ENDP  ; |SignPatch|

    WMV_LEAF_ENTRY SignPatch32_ARMV4
;void SignPatch32(I32_WMV * rgiCoefRecon, int len)
;{
;    int i;
;    I32_WMV v1, v2;
;    
;    for(i=0; i < (len >> 2); i++)
;    {
;        v1 = rgiCoefRecon[i*2];
;        v2 = rgiCoefRecon[i*2+1];
;        
;        rgiCoefRecon[i] = (v1 & 0x0000ffff) | (((v1 >> 16) + v2) << 16);
;    }
;}
    stmdb     sp!, {r4-r9, lr}
    FRAME_PROFILE_COUNT

    mov     r14, r0

    mov   r12, #0xFF
    add   r12, r12, r12, LSL #8
	pld	  [r14, #32]

SignPatch32_loop
	pld	  [r14, #64]
    ldr      r2, [r14], #+4
    ldr      r3, [r14], #+4
    ldr      r4, [r14], #+4
    ldr      r5, [r14], #+4
    ldr      r6, [r14], #+4
    ldr      r7, [r14], #+4
    ldr      r8, [r14], #+4
    ldr      r9, [r14], #+4

    add     r3, r3, r2, asr #16
    and     r2, r2, r12
    orr     r2, r2, r3, LSL #16

    add     r5, r5, r4, asr #16
    and     r4, r4, r12
    orr     r4, r4, r5, LSL #16

    add     r7, r7, r6, asr #16
    and     r6, r6, r12
    orr     r6, r6, r7, LSL #16

    add     r9, r9, r8, asr #16
    and     r8, r8, r12
    orr     r8, r8, r9, LSL #16

    str     r2, [r0], #+4
    str     r4, [r0], #+4
    str     r6, [r0], #+4
    str     r8, [r0], #+4

    subs    r1, r1, #16
    BGE    SignPatch32_loop

    ldmia     sp!, {r4-r9, pc}
    WMV_ENTRY_END
    ;ENDP  ; |SignPatch32|

;
; Void_WMV g_IDCTDec_WMV3_Pass1(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops, I32_WMV iDCTHorzFlags)
; r0 = piSrc0   r1= blk32   r2 = iNumLoops  r3 = iDCTHorzFlags
; 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA |.text|, CODE, READONLY
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass1_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    stmdb   sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT

    bl   |g_IDCTDec_WMV3_Pass1_Naked_ARMV4|

    ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA    |.text|, CODE
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass1_Naked_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;   stmdb   sp!, {r4 - r11, lr}
;   stmdb   sp!, {lr}
    str     lr,  [sp, #-4]!

    FRAME_PROFILE_COUNT

;	pld		[r0]       
;	pld		[r0, #32]  
;	pld		[r0, #64]  
;	pld		[r0, #96]  
				
    mov     r4, #4

|Pass1LoopStart|

    TST     r3, #3
    bne     |Pass1FullTransform|

;   r6 = b0
    ldr     r7, [r0], #4            ; piSrc0[i]
    add     r6, r4, r4, lsl #16
    add     r6, r6, r7, asl #2      ; piSrc0[i] * 4 + (4+(4<<16))
    adds    r6, r6, r7, asl #3      ; +piSrc0[i] * 8

;   r8 = b1
    add     r8, r6, r4, lsl #13     ; b0 + 0x8000
    movne   r6, r6, lsl #16
    
    movne   r6, r6, asr #19         ; ((I16_WMV)b0)>>3

;   r6 = iCurr  r8 = iNext
    addne   r6, r6, r6, asl #16
    movs    r8, r8, asr #19

    addne   r8, r8, r8, asl #16

;   blk32[0-7]

    str     r6, [r1], #4
    str     r6, [r1], #4
    str     r6, [r1], #4
    str     r6, [r1], #4
    str     r8, [r1], #4
    str     r8, [r1], #4
    str     r8, [r1], #4
    str     r8, [r1], #4

    b       |Pass1BeforeEnd|

|Pass1FullTransform|
;
;zeroth stage
;
;x4 = piSrc0[ i +1*4 ]
;x5 = piSrc0[ i +7*4 ]
;y3 = x4 + x5
;x8 = W3 * y3
;x4a = x8 - W3pW5 * x5
;x5a = x8 - W3_W5 * x4

    ldr     r8, [r0, #16]           ; x4 = [4]
    ldr     r9, [r0, #112]          ; x5 = [28*4]
    ldr     r6, [r0, #80]           ; x6
    ldr     r7, [r0, #48]           ; x7
    add     lr, r8, r9              ; y3
   
    rsb     r10, lr, lr, asl #4     ; x8 = 15*y3
    sub     r11, r10, r9, asl #4    ; x4a = x8 - 16 * x5
    sub     r11, r11, r9, asl #3    ;          - 8 * x5
    sub     r12, r10, r8, asl #2    ; x5a = x8 - 4 * x4
    sub     r12, r12, r8, asl #1    ;          - 2 * x4
;
;x8 = W7 * y3;          //4
;x4 = x8 + W1_W7 * x4;  //12
;x5 = x8 - W1pW7 * x5;  //20

    mov     r10, lr, asl #2         ; x8 = W7(4) * y3
    add     lr,  r10, r8, asl #3    ; x4 = x8 + 8 * x4
    add     r8,  lr,  r8, asl #2    ;         + 4 * x4
    
;
;first stage
;   r8 = x4     r9 = x5     r11 = x4a   r12 = x5a

;x7 = piSrc0[ i +3*4 ]
;x6 = piSrc0[ i +5*4 ]

    
    sub     lr,  r10, r9, asl #4    ; x5 = x8 - 16 * x5
    sub     r9,  lr,  r9 ,asl #2    ;         -  4 * x5

;y3 = x6 + x7;
;x8 = W7 * y3;          //4
;x4a -= x8 + W1_W7 * x6; //12
;x5a += x8 - W1pW7 * x7;    //20

    adds    lr, r6, r7              ; x6+x7
;    mov     r10, lr, asl #2         ; x8 = 4 * y3
    addne   r12, r12, lr, asl #2
    subne   r11, r11, lr, asl #2
    sub     r11, r11, r6, asl #3
    sub     r11, r11, r6, asl #2    ; x4a -= x8 + W1_W7 * x6
    sub     r12, r12, r7, asl #4
    sub     r12, r12, r7, asl #2    ; x5a += x8 - W1pW7 * x7

;x8 = W3 * y3;          //15
;x4 += x8 - W3_W5 * x6; //6
;x5 += x8 - W3pW5 * x7; //24

    rsbs    r10, lr, lr, asl #4     ; x8 = 15 * y3
    addne   r8,  r8, r10            ; x4 += x8 - W3_W5 * x6
    sub     r8,  r8, r6, asl #2
;
;second stage
;
;
;x0 = piSrc0[ i +0*4 ]; /* for proper rounding */
;x1 = piSrc0[ i +4*4 ];
;x1 = x1 * W0;  //12
;x0 = x0 * W0 + (4+(4<<16)); /* for proper rounding */
;x8 = x0 + x1;
;x0 -= x1;

    ldr     lr, [r0], #4            ; x0 = r6
    sub     r8,  r8, r6, asl #1
    addne   r9,  r9, r10            ; x5 += x8 - W3pW5 * x7
    add     r6, r4, lr, asl #2      ; x0*4+(4+(4<<16))
    add     r6, r6, r4, lsl #16
    add     r6, r6, lr, asl #3      ;     +x0*8

    ldr     lr, [r0, #60]           ; x1 = r7
    sub     r9,  r9, r7, asl #4
    sub     r9,  r9, r7, asl #3
    movs    r7, lr, asl #2          
    addnes  r7, r7, lr, asl #3

;r6=x0  r10 = x8
    add     r10, r6, r7             ; x8 = x0 + x1

;x3 = piSrc0[ i +2*4 ];
;x2 = piSrc0[ i +6*4 ];     
;x1 = x2;
;x2 = W6 * x3 - W2 * x2;  //6,  16
;x3 = W6 * x1 + W2A * x3; //6,  16

    ldr     lr, [r0, #28]           ; x3
    subne   r6, r6, r7              ; x0
    ldr     r7, [r0, #92]           ; x2

;r5 = x2
    movs    r5, lr, asl #2          ; 4*x3
    addne   r5, r5, lr, asl #1      ; 6 * x3
    sub     r5, r5, r7, asl #4      ;  - 16 * x2

;r7 = x3
    movne   lr, lr, asl #4          ; 16*x3
    add     lr, lr, r7, asl #2      ; 4*x2+16*x3
    adds    r7, lr, r7, asl #1      ; 6 * x2 + 16 * x3
;
;third stage
;  lr=x7   r10=x8  r7=x3      r6=x0
;
;x7 = x8 + x3;
;x8 -= x3;
;x3 = x0 + x2;
;x0 -= x2;

    add     lr, r10, r7             ; x7 = x8 + x3
    subne   r10, r10, r7
    add     r7, r6, r5
    sub     r6, r6, r5
;
; store blk32[0], blk32[4]
;  lr=x7   r10=x8  r7=x3      r6=x0
;  r8=x4   r9=x5   r11=x4a    r12=x5a
;

;b0 = x7 + x4;
;b1 = (b0 + 0x8000)>>19;
;b0 = ((I16_WMV)b0)>>3;

;c0 = x3 + x4a;
;c1 = (c0 + 0x8000)>>19;
;c0 = ((I16_WMV)c0)>>3;

    add     lr, lr, r8              ; b0 = x7 + x4
    sub     r8, lr, r8, asl #1      ; cn0 = x7 - x4
    add     r7, r7, r11             ; c0 = x3 + x4a
    
    add     r5, lr, r4, lsl #13     ; b1 = b0+0x8000
    movs    lr, lr, lsl #16
    sub     r11, r7, r11, asl #1    ; bn0 = x3 - x4a
    movne   lr, lr, asr #19
    add     r4, r7, r4, lsl #13     ; c1 = c0+0x8000
    movs    r7, r7, lsl #16
    
    movne   r7, r7, asr #19
    movs    r4, r4, asr #19
    
    movne   r4, r4, asl #16
    add     r4, r4, r5, asr #19     ; (c1<<16) + b1
    str     r4, [r1, #16]           ; blk32[4]

; store blk32[3], blk32[7]
;   r11=b0  r8=c0  lr,r7,r5,r4 free
;
    mov     r4, #4
    add     r7, lr, r7, asl #16     ; (c0<<16) + b0
    adds    lr, r11, r4, lsl #13    
    str     r7, [r1]                ; blk32[0]
    movne   lr, lr, asr #19         ; b1 = (b0 + 0x8000)>>19
    movs    r11, r11, lsl #16
    add     r6,  r6, r12            ; b0 = x0+x5a
    movne   r11, r11, asr #19       ; ((I16_WMV)b0)>>3

    adds    r7, r8, r4, lsl #13    
    sub     r12, r6, r12, asl #1    ; cn0= x0-x5a
    movne   r7, r7, asr #19         ; c1 = (c0 + 0x8000)>>19
    movs    r8, r8, lsl #16
    add     r7, lr, r7, asl #16     ; blk32[7] = (c1<<16) + b1
    movne   r8, r8, asr #19         ; ((I16_WMV)c0)>>3
    
    
; store blk32[1], blk32[5]
;   r6=x0   r12=x5a r10=x8  r9=x5
;
    
    adds    lr, r6, r4, lsl #13    
    add     r10, r10, r9            ; c0 = x8+x5
    movne   lr, lr, asr #19         ; b1 = (b0 + 0x8000)>>19
    movs    r6, r6, lsl #16
    sub     r9,  r10, r9, asl #1    ; bn0= x8-x5
    movne   r6, r6, asr #19       ; ((I16_WMV)b0)>>3
    
    adds    r5, r10, r4, lsl #13    
    add     r8, r11, r8, asl #16    ; blk32[3] = (c0<<16) + b0
    movne   r5, r5, asr #19         ; c1 = (c0 + 0x8000)>>19
    movs    r10, r10, lsl #16
    str     r8, [r1, #12]
    movne   r10, r10, asr #19         ; ((I16_WMV)c0)>>3
    str     r7, [r1, #28]
    add     r5, lr, r5, asl #16     ; blk32[7] = (c1<<16) + b1
    
            
; store blk32[2], blk32[6]
;

    adds    lr, r9, r4, lsl #13    
    add     r6, r6, r10, asl #16    ; blk32[3] = (c0<<16) + b0
    movne   lr, lr, asr #19         ; b1 = (b0 + 0x8000)>>19
    movs    r9, r9, lsl #16
    str     r5, [r1, #20]
    movne   r9, r9, asr #19       ; ((I16_WMV)b0)>>3
    adds    r10, r12, r4, lsl #13    
    str     r6, [r1, #4]
    movne   r10, r10, asr #19         ; c1 = (c0 + 0x8000)>>19
    movs    r12, r12, lsl #16
    movne   r12, r12, asr #19         ; ((I16_WMV)c0)>>3
    add     r10, lr, r10, asl #16     ; blk32[7] = (c1<<16) + b1
    add     r9, r9, r12, asl #16    ; blk32[3] = (c0<<16) + b0
    str     r9, [r1, #8]
    str     r10, [r1, #24]

    add     r1,  r1,  #32           ; blk32+=8
    
|Pass1BeforeEnd|
    subs    r2, r2, #1               ; if (i<iNumLoops)
    mov     r3,  r3,  asr #2        ; iDCTHorzFlags >>= 2
    bne     |Pass1LoopStart|

;   ldmia     sp!, {r4 - r11, pc}
;   ldmia     sp!, {pc}
    ldr       pc,  [sp], #4
    
    WMV_ENTRY_END	;g_IDCTDec_WMV3_Pass1_Naked

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA |.text|, CODE, READONLY
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass2_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    stmdb   sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT

    bl   |g_IDCTDec_WMV3_Pass2_Naked_ARMV4|

    ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END
;
;Void_WMV g_IDCTDec_WMV3_Pass2(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA    |.text|, CODE
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass2_Naked_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; r0=piSrc0   r1=blk16   r2=iNumLoops
;
;   stmdb   sp!, {r4 - r11, lr}
;   stmdb   sp!, {lr}
    str     lr,  [sp, #-4]!  

    FRAME_PROFILE_COUNT

;	pld		[r0]       
;	pld		[r0, #32]  
;	pld		[r0, #64]  
;	pld		[r0, #96]  
				
    ldr     r8,  [r0, #16]          ; x4
    ldr     r9,  [r0, #112]         ; x5
    mov     r3, #32

|Pass2LoopStart|
;x4 = piSrc0[i + 1*4 ];
;x5 = piSrc0[i + 7*4 ];
;y4a = x4 + x5;
;x8 = 7 * y4a;
;x4a = x8 - 12 * x5;
;x5a = x8 - 3 * x4;
;  zeroth stage

    ldr     r10, [r0, #80]          ; x6
    adds    lr,  r8, r9             ; y4a
    ldr     r11, [r0, #48]          ; x7
    rsb     r12, lr, lr, asl #3     ; x8
    sub     r6,  r12, r9, asl #3
    sub     r6,  r6, r9, asl #2     ; x4a
    sub     r7,  r12, r8, asl #1    
    sub     r7,  r7,  r8            ; x5a

;x8 = 2 * y4a;
;x4 = x8 + 6 * x4;
;x5 = x8 - 10 * x5;

    movne   r12, lr, asl #1         ; x8
    add     r4,  r12, r8, asl #2
    add     r8,  r4, r8, asl #1     ; x4
    sub     r4,  r12, r9, asl #3
    sub     r9,  r4,  r9, asl #1    ; x5

;ls_signbit=y4a&0x8000;
;y4a = (y4a >> 1) - ls_signbit;
;y4a = y4a & ~0x8000;
;y4a = y4a | ls_signbit;
;x4a += y4a;
;x5a += y4a;

    and     r4,  lr, r3, lsl #10
    rsb     lr,  r4, lr, asr #1
    bic     lr,  lr, r3, lsl #10
    orrs    lr,  lr, r4             ; y4a



;x6 = piSrc0[i + 5*4 ];
;x7 = piSrc0[i + 3*4 ];
;y4 = x6 + x7;
;x8 = 2 * y4;
;x4a -= x8 + 6 * x6;
;x5a += x8 - 10 * x7;
;  first stage

    addne   r6,  r6, lr             ; x4a
    addne   r7,  r7, lr             ; x5a
    adds    lr,  r10, r11           ; y4
;    mov     r12, lr, asl #1         ; x8
    rsb     r12, lr, lr, asl #3     ; x8
    subne   r6,  r6, lr, asl #1
    sub     r6,  r6, r10, asl #2
    sub     r6,  r6, r10, asl #1    ; x4a
    addne   r7,  r7, lr, asl #1
    sub     r7,  r7, r11, asl #3
    sub     r7,  r7, r11, asl #1    ; x5a

;x8 = 7 * y4;
;ls_signbit=y4&0x8000;
;y4 = (y4 >> 1) - ls_signbit;
;y4 = y4 & ~0x8000;
;y4 = y4 | ls_signbit;
;x4 += y4;
;x5 += y4;
;x4 += x8 - 3 * x6;
;x5 += x8 - 12 * x7;

    ands    r4,  lr, r3, lsl #10
    rsb     lr,  r4, lr, asr #1
    bic     lr,  lr, r3, lsl #10
    orrnes  lr,  lr, r4             ; y4
    adds    r12, r12, lr           ; y4+x8
    addne   r8,  r8, r12
;
; second stage
;
;x0 = piSrc0[i + 0*4 ] * 6 + 32 + (32<<16) /* rounding */;
;x1 = piSrc0[i + 4*4 ] * 6;
;x8 = x0 + x1;
;x0 -= x1;
;x2 = piSrc0[i + 6*4 ];
;x3 = piSrc0[i + 2*4 ];
;x1 = 8 * (x2 + x3);
;x6 = x1 - 5 * x2;
;x1 -= 11 * x3;

    ldr     lr,  [r0], #4
    addne   r9,  r9, r12
    add     r4,  r3, r3, lsl #16
    add     r4,  r4, lr, asl #2
    add     r4,  r4, lr, asl #1     ; x0

    ldr     lr,  [r0, #60]          ; [4*4]
    sub     r8,  r8, r10, asl #1
    sub     r8,  r8, r10            ; x4
    adds    lr,  lr, lr, asl #1      
    sub     r9,  r9, r11, asl #3
    add     r12, r4, lr, asl #1     ; x8
    subne   r4,  r4, lr, asl #1     ; x0

    ldr     lr,  [r0, #92]          ; x2
    ldr     r5,  [r0, #28]          ; x3
    sub     r9,  r9, r11, asl #2    ; x5
    add     r11, lr, lr, asl #1     ; 3*x2
    adds    r10, r11, r5, asl #3    ; x6
    sub     r11, r5,  r5, asl #2     ; -3*x3
    add     r5,  r11, lr, asl #3    ; x1

; third stage
;x7 = x8 + x6;
;x8 -= x6;
;x6 = x0 - x1;
;x0 += x1;

    add     r11,  r12, r10          ; x7
    subne   r12,  r12, r10          ; x8
    sub     r10,  r4, r5            ; x6
    add     r4,   r4, r5            ; x0
;
;r4=x0 r6=x4a r7=x5a r8=x4 r9=x5 r10=x6 r11=x7 x12=x8 
; r5, lr are free

;// blk0
;b0 = (x7 + x4);
;b1 = (b0 + 0x8000)>>22;
;b0 = ((I16_WMV)b0)>>6;
;blk16[ i + 0   + 0*4] = b0;
;blk16[ i + 32 + 0*4] = b1;

    adds    r5,  r11, r8            ; b0
    add     lr,  r5,  r3, lsl #10
    movne   r5,  r5,  asl #16
    mov     lr,  lr,  asr #22       ; b1
    strh    lr,  [r1, #64]
    movne   r5,  r5,  asr #22       ; b0
    

;// blk7
;b0 = (x7 - x4);
;b1 = (b0 + 0x8000)>>22;
;b0 = ((I16_WMV)b0)>>6;
;blk16[ i + 0 + 7*4] = b0;
;blk16[ i + 32 + 7*4] = b1;

    subs    r11, r11,  r8          ; b0
    add     lr,  r11,  r3, lsl #10
    strh    r5,  [r1]
    mov     lr,  lr,  asr #22      ; b1
    
    movne   r11,  r11,  asl #16
    strh    lr,  [r1, #120]
    movne   r11,  r11,  asr #22    ; b0
    
;// blk1
;b0 = (x6 + x4a);
;b1 = (b0 + 0x8000)>>22;
;b0 = ((I16_WMV)b0)>>6;
;blk16[ i + 0 + 1*4] = b0;
;blk16[ i + 32 + 1*4] = b1;

    adds    r5,  r10, r6            ; b0
    add     lr,  r5,  r3, lsl #10
    strh    r11,  [r1, #56]
    mov     lr,  lr,  asr #22       ; b1
    movne   r5,  r5,  asl #16
    strh    lr,  [r1, #72]
    movne   r5,  r5,  asr #22       ; b0
    

;// blk6
;b0 = (x6 - x4a);
;b1 = (b0 + 0x8000)>>22;
;b0 = ((I16_WMV)b0)>>6;
;blk16[ i + 0 + 6*4] = b0;
;blk16[ i + 32 + 6*4] = b1;

    subs    r11, r10, r6     
    add     lr,  r11, r3, lsl #10
    strh    r5,  [r1, #8]
    mov     lr,  lr,  asr #22      ; b1
    
    movne   r11,  r11,  asl #16
    strh    lr,  [r1, #112]
    movne   r11,  r11,  asr #22    ; b0
    

;// blk2
;b0 = (x0 + x5a);
;b1 = (b0 + 0x8000)>>22;
;b0 = ((I16_WMV)b0)>>6;
;blk16[ i + 0 + 2*4] = b0;
;blk16[ i + 32 + 2*4] = b1;

    adds    r5,  r4, r7            ; b0
    add     lr,  r5,  r3, lsl #10
    strh    r11,  [r1, #48]
    mov     lr,  lr,  asr #22       ; b1

    movne   r5,  r5,  asl #16
    strh    lr,  [r1, #80]
    movne   r5,  r5,  asr #22       ; b0

;// blk5
;b0 = (x0 - x5a);
;b1 = (b0 + 0x8000)>>22;
;b0 = ((I16_WMV)b0)>>6;
;blk16[ i + 0 + 5*4] = b0;
;blk16[ i + 32 + 5*4] = b1;

    subs    r11, r4, r7     
    add     lr,  r11, r3, lsl #10
    strh    r5,  [r1, #16]
    mov     lr,  lr,  asr #22      ; b1
    
    movne   r11,  r11,  asl #16
    strh    lr,  [r1, #104]
    movne   r11,  r11,  asr #22    ; b0
    
;// blk3
;b0 = (x8 + x5);
;b1 = (b0 + 0x8000)>>22;
;b0 = ((I16_WMV)b0)>>6;
;blk16[ i + 0 + 3*4] = b0;
;blk16[ i + 32 + 3*4] = b1;

    adds    r5,  r12, r9            ; b0
    add     lr,  r5,  r3, lsl #10
    strh    r11,  [r1, #40]
    mov     lr,  lr,  asr #22       ; b1

    movne   r5,  r5,  asl #16
    strh    lr,  [r1, #88]
    movne   r5,  r5,  asr #22       ; b0
    

;// blk4
;b0 = (x8 - x5);
;b1 = (b0 + 0x8000)>>22;
;b0 = ((I16_WMV)b0)>>6;
;blk16[ i + 0 + 4*4] = b0;
;blk16[ i + 32 + 4*4] = b1;

    subs    r11, r12, r9     
    add     lr,  r11, r3, lsl #10
    strh    r5,  [r1, #24]
    mov     lr,  lr,  asr #22      ; b1
    
    movne   r11,  r11,  asl #16
    strh    lr,  [r1, #96]
    movne   r11,  r11,  asr #22    ; b0
    strh    r11,  [r1, #32]

    subs    r2, r2, #1               ; if (i<iNumLoops)
    add     r1, r1, #2
    ldrne   r8,  [r0, #16]          ; x4
    ldrne   r9,  [r0, #112]         ; x5
    bne     |Pass2LoopStart|

;   ldmia     sp!, {r4 - r11, pc}
;   ldmia     sp!, {pc}
    ldr       pc,  [sp], #4
    
    WMV_ENTRY_END	;g_IDCTDec_WMV3_Pass2_Naked_ARMV4

;
;Void_WMV g_IDCTDec_WMV3_Pass3(const I32_WMV  *piSrc0, I32_WMV * blk32, const I32_WMV iNumLoops,  I32_WMV iDCTHorzFlags)
;
; r0 = piSrc0   r1= blk32   r2 = iNumLoops  r3 = iDCTHorzFlags
; 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA |.text|, CODE, READONLY
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass3_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    stmdb   sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT

    bl   |g_IDCTDec_WMV3_Pass3_Naked_ARMV4|

    ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA    |.text|, CODE
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass3_Naked_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
;   stmdb   sp!, {r4 - r9, lr}
;   stmdb   sp!, {lr}
    str     lr,  [sp, #-4]!

    FRAME_PROFILE_COUNT

;	pld		[r0]
;	pld		[r0, #32]

    mov     r4, #4
    add     r5, r4, r4, lsl #16

|Pass3LoopStart|
    
    TST     r3, #3
    bne     |Pass3FullTransform|

    ldr     r7, [r0], #4            ; piSrc0[i]
    
    
    add     r6, r5, r7, asl #4      ; piSrc0[i] * 16 + (4+(4<<16))
    adds    r6, r6, r7              ; +piSrc0[i]

;   r7 = b1
    add     r7, r6, r4, lsl #13     ; b0 + 0x8000
    movne   r6, r6, lsl #16
    
    movne   r6, r6, asr #19         ; ((I16_WMV)b0)>>3

;   r6 = iCurr  r7 = iNext
    
    addne   r6, r6, r6, asl #16
    movs    r7, r7, asr #19
    str     r6, [r1], #32
    addne   r7, r7, r7, asl #16

;   blk32[0-7]
    
    str     r6, [r1, #-28]
    str     r7, [r1, #-16]
    str     r7, [r1, #-12]

    b       |Pass3BeforeEnd|

|Pass3FullTransform|
;x4 = piSrc0[ i +0*4 ];     
;x6 = piSrc0[ i +2*4 ];

    ldr     r8, [r0], #4            ; x4
    ldr     r9, [r0, #28]           ; x6
    ldr     r10, [r0, #12]           ; x5

    add     lr, r8, r9
    ldr     r11, [r0, #44]           ; x7

    add     r6, lr, lr, asl #4
    sub     lr, r8, r9
    
;x5 = piSrc0[ i +1*4 ];     
;x7 = piSrc0[ i +3*4 ];
;x8 = 10 * (x5 + x7);
;x2 = x8 + 12 * x5;
;x3 = x8 - 32 * x7;

    add     r7, lr, lr, asl #4
    add     r6, r6, r5              ; x0
    adds    r12, r10, r11
    add     r7, r7, r5              ; x1
    addne   r12, r12, r12, asl #2
    
    movne   r12, r12, asl #1        ; x8

    add     lr, r12, r10, asl #3
    adds    r8, lr, r10, asl #2      ; x2
    sub     r9, r12, r11, asl #5     ; x3
;
; store blk32[0], blk32[4]
;
;b0 = x0 + x2;
;b1 = (b0 + 0x8000)>>19;
;b0 = ((I16_WMV)b0)>>3;

    addne   r6, r6, r8              ; b0 =  x0 + x2;
    sub     r8, r6, r8, asl #1      ; cn0 = x0 - x2;
    add     r7, r7, r9              ; c0 = x1 + x3
    
    add     lr, r6, r4, lsl #13     ; b1 = b0+0x8000
    movs    r6, r6, lsl #16
    sub     r9, r7, r9, asl #1      ; bn0 = x1 - x3
    movne   r6, r6, asr #19         ; b0
    add     r12,r7, r4, lsl #13     ; c1
    movs    r7, r7, lsl #16
    
    movne   r7, r7, asr #19         ; c0
    movs    r12, r12, asr #19       ; c1 >> 19
    add     r7, r6, r7, asl #16     ; (c0<<16) + b0
    str     r7, [r1], #32           ; blk32[0]
    movne   r12, r12, asl #16       ; c1 << 16
    add     r12,  r12, lr, asr #19  ; c1 + b1>>19
    
; store  blk[1,5]
;
    add     lr, r9, r4, lsl #13     ; b1
    movs    r9, r9, lsl #16
    str     r12,  [r1, #-16]         ; blk32[4]
    movne   r9, r9, asr #19         ; b0

    add     r12, r8, r4, lsl #13    ; c1
    movs    r8, r8, lsl #16
    
    movne   r8, r8, asr #19         ; c0
    movs    r12, r12, asr #19       ; c1 >> 19
    add     r8, r9, r8, asl #16     ; (c0<<16) + b0
    str     r8, [r1, #-28]      
    movne   r12, r12, asl #16       ; c1 << 16
    add     r12, r12, lr, asr #19   ; c1 + b1>>19
    str     r12, [r1, #-12]        

;    add     r1,  r1,  #32           ; blk32+=8
    
|Pass3BeforeEnd|
    subs    r2, r2, #1               ; if (i<iNumLoops)
    mov     r3,  r3,  asr #2        ; iDCTHorzFlags >>= 2
    bne     |Pass3LoopStart|


;   ldmia   sp!, {r4 - r9, pc}
;   ldmia   sp!, {pc}
    ldr     pc,  [sp], #4
	
    WMV_ENTRY_END	;g_IDCTDec_WMV3_Pass3_Naked

;
;Void_WMV g_IDCTDec_WMV3_Pass4(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops)
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA |.text|, CODE, READONLY
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass4_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    stmdb   sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT

    bl   |g_IDCTDec_WMV3_Pass4_Naked_ARMV4|

    ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    AREA    |.text|, CODE
    WMV_LEAF_ENTRY g_IDCTDec_WMV3_Pass4_Naked_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; r0=piSrc0   r1=blk16   r2=iNumLoops
;
;   stmdb   sp!, {r4 - r8, lr}
;   stmdb   sp!, {lr}
    str     lr,  [sp, #-4]!


    FRAME_PROFILE_COUNT

;	pld		[r0]
;	pld		[r0, #32]

    ldr     r5,  [r0], #4           ; x4
    ldr     r12, [r0, #28]          ; x6

    mov     r3, #32
    add     r4, r3, r3, lsl #16

|Pass4LoopStart|

;x4 = piSrc0[i + 0*4 ];
;x6 = piSrc0[i + 2*4 ];
;x3 = (x4 - x6); 
;x6 += x4;
;x4 = 8 * x6 + 32 + (32<<16); //rounding
;x8 = 8 * x3 + 32 + (32<<16);   //rounding

    sub     lr,  r5, r12            ; x3
    add     r12,  r12, r5           ; x6
    ldr     r6,  [r0, #12]          ; x5
    add     r5,  r4, r12, asl #3
    add     r7,  r4, lr, asl #3     ; x8

;ls_signbit=x6&0x8000;
;x6 = (x6 >> 1) - ls_signbit;
;x6 = x6 & ~0x8000;
;x6 = x6 | ls_signbit;
;x4 += x6; // guaranteed to have enough head room

    and     r8, r12,  r3, lsl #10
    rsb     r12, r8,  r12, asr #1
    bic     r12, r12,  r3, lsl #10
    orrs    r12, r12,  r8
    addne   r5,  r5,  r12

;ls_signbit=lr&0x8000;
;x3 = (x3 >> 1) - ls_signbit;
;x3 = x3 & ~0x8000;
;x3 = x3 | ls_signbit;
;x8 += x3 ;

    and     r8,  lr,  r3, lsl #10
    rsb     lr,  r8,  lr, asr #1
    ldr     r12, [r0, #44]          ; x7
    bic     lr,  lr,  r3, lsl #10
    orrs    lr,  lr,  r8
    addne   r7,  r7,  lr

;x5 = piSrc0[i + 1*4 ];
;x7 = piSrc0[i + 3*4 ];
;x1 = 5 * ( x5 + x7);
;x5a = x1 + 6 * x5;
;x5 =  x1 - x7 >> 4;

    adds    lr, r6, r12
    
    addne   lr, lr, lr, asl #2      ; 5*(x5+x7)
    add     r8, lr, r6, asl #2
    adds    r8, r8, r6, asl #1      ; x5a
    sub     r6, lr, r12, asl #4     ; x5

;blk0, blk3
;b0 = (x4 + x5a);
;b0 = (x4 - x5a);

    addne   r5,  r5, r8             ; b0
    adds    r12, r5, r3, lsl #10   
    sub     r8,  r5, r8, asl #1     ; bn0
    movne   r12, r12, asr #22      ; b1
    movs    r5,  r5, asl #16
    strh    r12, [r1, #64]
    movne   r5,  r5, asr #22       ; b0
    adds    r12, r8, r3, lsl #10   
    strh    r5,  [r1]
    movne   r12, r12, asr #22      ; b1
    movs    r8,  r8, asl #16
    strh    r12, [r1, #88]
    movne   r8,  r8, asr #22       ; b0
    

;blk1 blk2
;b0 = (x8 + x5);
;b0 = (x8 - x5);

    add     r7,  r7, r6             ; b0
    sub     r6,  r7, r6, asl #1     ; bn0

    adds    r12, r7, r3, lsl #10   
    strh    r8,  [r1, #24]
    movne   r12, r12, asr #22      ; b1
    movs    r7,  r7, asl #16
    strh    r12, [r1, #72]
    movne   r7,  r7, asr #22       ; b0
    adds    r12, r6, r3, lsl #10   
    strh    r7,  [r1, #8]
    movne   r12, r12, asr #22      ; b1
    movs    r6,  r6, asl #16
    strh    r12, [r1, #80]
    movne   r6,  r6, asr #22       ; b0
    strh    r6,  [r1, #16]
    add     r1, r1, #2

    subs    r2, r2, #1               ; if (i<iNumLoops)
    ldrne   r5,  [r0], #4           ; x4
    ldrne   r12, [r0, #28]          ; x6
    bne     |Pass4LoopStart|

;   ldmia     sp!, {r4 - r8, pc}
;   ldmia     sp!, {pc}
    ldr       pc,  [sp], #4
    
    WMV_ENTRY_END	;g_IDCTDec_WMV3_Pass4_Naked

    EXPORT end_idct_wmv9_arm
end_idct_wmv9_arm
    nop                 ; to know where previous function ends.

    ENDIF ; WMV_OPT_IDCT_ARM

    END 
