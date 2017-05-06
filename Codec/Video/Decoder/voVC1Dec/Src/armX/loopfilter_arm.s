;//*@@@+++@@@@******************************************************************
;//
;// Microsoft Windows Media
;// Copyright (C) Microsoft Corporation. All rights reserved.
;//
;//*@@@---@@@@******************************************************************

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
;  THIS IS ASSEMBLY VERSION OF ROUTINES IN SHARE.CPP WHEN 
;  WMV_OPT_LOOPFILTER_ARM ARE DEFINED
;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


 
    INCLUDE wmvdec_member_arm.inc
    INCLUDE xplatform_arm_asm.h 
    IF UNDER_CE != 0
    INCLUDE kxarm.h
    ENDIF   

    AREA    |.text|, CODE

    IF WMV_OPT_LOOPFILTER_ARM = 1
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

 
    IF PRO_VER != 0
    EXPORT  g_FilterHorizontalEdgeV9_ARMV4
    EXPORT  g_FilterVerticalEdgeV9_ARMV4
    IMPORT  s_iNextPixel    
    ENDIF  ;PRO_VER

	EXPORT  g_OverlapBlockVerticalEdge_ARMV4
	EXPORT  g_OverlapBlockHorizontalEdge_ARMV4


    WMV_LEAF_ENTRY g_OverlapBlockVerticalEdge_ARMV4

; 5161 : {

	stmdb     sp!, {r4 - r11, lr}
    FRAME_PROFILE_COUNT
|$M39579|
	mov       r11, r0

; 5162 :     I32_WMV iRnd = 1;

	mov       r8, #1
	mov       r9, r1, lsl #1  
	mov       r10, #8
|$L38833|

; 5163 :     I32_WMV ii ;
; 5164 : 
; 5165 :     for ( ii = 0; ii < 8; ii++) {
; 5166 :         I32_WMV temp;
; 5167 :         I32_WMV v0 = pInOut[-2];

    IF _XSC_=1
        PLD [r11, #32]
    ENDIF
	ldrsh     r7, [r11, #-4]
	subs       r10, r10, #1

; 5168 :         I32_WMV v1 = pInOut[-1];
; 5169 :         I32_WMV v2 = pInOut[0];
; 5170 :         I32_WMV v3 = pInOut[1];

	ldrsh     r6, [r11, #2]

; 5171 : 
; 5172 :         temp = v0 + iRnd + 3;

	add       r0, r7, r8
	ldrsh     r5, [r11]
	add       r3, r0, #3
	ldrsh     r4, [r11, #-2]

; 5173 :         pInOut[-2] = (v3 + temp + 6 * v0) >> 3 ; //OUTER_1;

	add       r0, r7, r7, lsl #1
	add       r1, r3, r0, lsl #1
	add       r2, r1, r6
	mov       r0, r2, asr #3
	strh      r0, [r11, #-4]

; 5174 :         pInOut[0] = (7 * v2 - v3 + v1 + temp) >> 3 ; //INNER_2;

	rsb       r0, r5, r5, lsl #3
	sub       r1, r0, r6
	add       r2, r1, r3
	add       r0, r2, r4
	mov       r1, r0, asr #3
	strh      r1, [r11]

; 5175 :         temp = v3 + 4 - iRnd;

	sub       r1, r6, r8

; 5176 :         pInOut[-1] = (7 * v1 - v0 + v2 + temp) >> 3 ; //INNER_1;

	rsb       r0, r4, r4, lsl #3
	add       r3, r1, #4
	sub       r1, r0, r7
	add       r2, r1, r3
	add       r0, r2, r5
	mov       r1, r0, asr #3
	strh      r1, [r11, #-2]

; 5177 :         pInOut[1] = (v0 + temp + 6 * v3) >> 3 ; //OUTER_2;

	add       r0, r6, r6, lsl #1
	add       r1, r3, r0, lsl #1
	add       r2, r1, r7
	mov       r0, r2, asr #3
	strh      r0, [r11, #2]

; 5178 : 
; 5179 :         iRnd ^= 1;

	eor       r8, r8, #1

; 5180 :         pInOut += iStride;

	add       r11, r11, r9
	bne       |$L38833|

; 5181 :     }
; 5182 : }

	ldmia     sp!, {r4 - r11, pc}
    WMV_ENTRY_END
|$M39580|


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    WMV_LEAF_ENTRY g_OverlapBlockHorizontalEdge_ARMV4

; 5198 : {

	stmdb     sp!, {r4 - r12, lr}
    FRAME_PROFILE_COUNT
	sub       sp, sp, #0x24
|$M39663|
	mov       r6, r1
	mov       r5, r2
	str       r6, [sp, #0x1C]
	mov       r7, r3
	str       r5, [sp, #0x18]
	mov       r4, r0
	str       r7, [sp, #0xC]

; 5199 :     I32_WMV  iRnd = 1;
; 5200 :     pSrcTop += 6 * iSrcStride;
; 5201 : 
; 5202 :     if (bTop & bCurrent) {

	ldr       r1, [sp, #0x50] ;[#0x4C]
	add       r0, r5, r5, lsl #1
	add       r8, r4, r0, lsl #2
	ldr       r0, [sp, #0x54] ;[#0x50]
	mov       r2, #1
	str       r2, [sp, #0x10]
	tst       r1, r0
	beq       |$L38874|

; 5203 :         I32_WMV i;
; 5204 :         for ( i = 0; i < 8; i++) {

	ldr       r4, [sp, #0x4C] ;[#0x48]
	mov       r1, #0
    mov       r12, #0
	mov       r1, r4, lsl #1
	sub       r0, r7, r1
	str       r0, [sp, #0x14]
	mov       r0, r5, lsl #1
	str       r0, [sp, #0x20]
	add       r11, r0, r6
	mov       lr, r6
	sub       r3, r8, r6
	sub       r10, r7, r4
	add       r9, r7, r4
|$L38876|

; 5205 :             I32_WMV v0 = pSrcTop[i];

	ldrsh     r7, [r3, +lr]

; 5206 :             I32_WMV v1 = pSrcTop[i + iSrcStride];
; 5207 :             I32_WMV v2 = pSrcCurr[i];

	ldrsh     r8, [lr], #2

; 5208 :             I32_WMV v3 = pSrcCurr[i + iSrcStride];
; 5209 :             I32_WMV temp, k;
; 5210 : 
; 5211 :             temp = v0 + iRnd + 3;

	ldrsh     r5, [r11]
    add       r0, r7, r2
	add       r4, r0, #3

	ldrsh     r6, [r3, +r11]

; 5212 :             k = ((7 * v2 - v3 + v1 + temp) >> 3) + g_iOverlapOffset;

	rsb       r0, r8, r8, lsl #3
    IF _XSC_=1
        PLD [r11, #32]
    ENDIF
	sub       r1, r0, r5
	add       r2, r1, r4
	add       r0, r2, r6
    IF _XSC_=1
        PLD [lr, #32]
    ENDIF
	mov       r1, r0, asr #3
	adds      r2, r1, #0x80
	ldr       r1, [sp, #0xC]

; 5213 :             pDst[i] = (k < 0) ? 0 : ((k > 255) ? 255 : k);
    bics      r0, r2, #0xFF
    mvnne     r2, r2, asr #31

; 5214 :             k = ((6 * v0 + v3 + temp) >> 3) + g_iOverlapOffset;

	add       r0, r7, r7, lsl #1
	strb      r2, [r12, +r1]
    IF _XSC_=1
        add r1, r3, r11
        PLD [r1, #32]
    ENDIF   
	add       r1, r4, r0, lsl #1
	add       r2, r1, r5
	mov       r0, r2, asr #3
	adds      r1, r0, #0x80

; 5215 :             pDst[i - 2 * iDstStride] = (k < 0) ? 0 : ((k > 255) ? 255 : k);

    ldr       r2, [sp, #0x14]
    bics      r0, r1, #0xFF
    mvnne     r1, r1, asr #31

; 5216 :             
; 5217 :             temp = v3 + 4 - iRnd;
    ldr       r4, [sp, #0x10]
	add       r0, r5, r5, lsl #1
    strb      r1, [r2]
	sub       r1, r5, r4
	add       r4, r1, #4

; 5218 :             k = ((6 * v3 + v0 + temp) >> 3) + g_iOverlapOffset;


    IF _XSC_=1
        add r1, r3, lr
        PLD [r1, #32]
    ENDIF
	add       r1, r4, r0, lsl #1
	add       r2, r1, r7
	mov       r0, r2, asr #3
	adds      r1, r0, #0x80

; 5219 :             pDst[i + iDstStride] = (k < 0) ? 0 : ((k > 255) ? 255 : k);
    bics      r0, r1, #0xFF
    mvnne     r1, r1, asr #31
	rsb       r0, r6, r6, lsl #3
	strb      r1, [r9, +r12]

; 5220 :             k = ((7 * v1 - v0 + v2 + temp) >> 3) + g_iOverlapOffset;

	sub       r1, r0, r7
	add       r2, r1, r4
	add       r0, r2, r8
	mov       r1, r0, asr #3
	adds      r1, r1, #0x80

; 5221 :             pDst[i - iDstStride] = (k < 0) ? 0 : ((k > 255) ? 255 : k);
    bics      r0, r1, #0xFF
    mvnne     r1, r1, asr #31
	ldr       r2, [sp, #0x10]
    strb      r1, [r10, +r12]

; 5222 : 
; 5223 :             iRnd ^= 1;


	add       r12, r12, #1
	ldr       r0, [sp, #0x14]
	eor       r2, r2, #1
	str       r2, [sp, #0x10]
	add       r11, r11, #2
	add       r0, r0, #1
	str       r0, [sp, #0x14]
	cmp       r12, #8
	blt       |$L38876|

; 5224 :         }
; 5225 : 
; 5226 :         pSrcCurr += 2 * iSrcStride;
; 5227 :         pDst += 2 * iDstStride;

	ldr       r7, [sp, #0x4C] ;[#0x48]
	ldr       r1, [sp, #0xC]
	mov       r0, r7, lsl #1
	ldr       r10, [sp, #0x18]
	add       r6, r1, r0
	ldr       r8, [sp, #0x1C]

; 5228 : 
; 5229 :         if (!bWindup) {

	ldr       r0, [sp, #0x58] ;[#0x54]
	add       r5, r8, r10, lsl #2
	cmp       r0, #0
	bne       |$L38911|

; 5230 :             I32_WMV i ;
; 5231 :             for (i = 0; i < 4; i++) {

	mov       r4, #4
|$L38887|

; 5232 :                 I32_WMV j;
; 5233 :                 for ( j = 0; j < 8; j++) {

;	mov       r3, #0
	mov       r2, r5
    IF _XSC_=1
        PLD [r2, #32]
    ENDIF
|$L38891|

; 5234 :                     I32_WMV k = pSrcCurr[j] + g_iOverlapOffset;
; 5235 :                     pDst[j] = (k < 0) ? 0 : ((k > 255) ? 255 : k);
;num = 0~3	
	ldrsh     r0, [r2], #2
	ldrsh     r1, [r2], #2
	ldrsh     r8, [r2], #2
	ldrsh     r9, [r2], #2

	adds      r0, r0, #0x80
	adds      r1, r1, #0x80
	adds      r8, r8, #0x80
	adds      r9, r9, #0x80

	bics      r10, r0, #0xFF
	mvnne     r0, r0, asr #31
	bics      r10, r1, #0xFF
	mvnne     r1, r1, asr #31
	bics      r10, r8, #0xFF
	mvnne     r8, r8, asr #31
	bics      r10, r9, #0xFF
	mvnne     r9, r9, asr #31
	strb      r0, [r6, #0]
	strb      r1, [r6, #1]
	strb      r8, [r6, #2]
	strb      r9, [r6, #3]

;num = 4~7	
	ldrsh     r0, [r2], #2
	ldrsh     r1, [r2], #2
	ldrsh     r8, [r2], #2
	ldrsh     r9, [r2], #2

	adds      r0, r0, #0x80
	adds      r1, r1, #0x80
	adds      r8, r8, #0x80
	adds      r9, r9, #0x80
	bics      r10, r0, #0xFF
	mvnne     r0, r0, asr #31
	bics      r10, r1, #0xFF
	mvnne     r1, r1, asr #31
	bics      r10, r8, #0xFF
	mvnne     r8, r8, asr #31
	bics      r10, r9, #0xFF
	mvnne     r9, r9, asr #31
	strb      r0, [r6, #4]
	strb      r1, [r6, #5]
	strb      r8, [r6, #6]
	strb      r9, [r6, #7]

;	add       r3, r3, #1
;	cmp       r3, #8
;	blt       |$L38891|

; 5236 :                 }
; 5237 :                 pSrcCurr += iSrcStride;

	ldr       r0, [sp, #0x20]
	subs       r4, r4, #1

; 5238 :                 pDst += iDstStride;

	add       r6, r6, r7
	add       r5, r5, r0
	bne       |$L38887|
|$L38911|

; 5267 :         }
; 5268 :     }
; 5269 : }

	add       sp, sp, #0x24
	ldmia     sp!, {r4 - r12, pc}
|$L38874|

; 5239 :             }
; 5240 :         }
; 5241 :     }
; 5242 :     // remaining 2 of past
; 5243 :     else if (bTop) {

	cmp       r1, #0
	beq       |$L38896|

; 5244 :         I32_WMV i;
; 5245 :         pDst -= 2 * iDstStride;

	ldr       r7, [sp, #0x4C] ;[#0x48]
	ldr       r0, [sp, #0xC]
	mov       r6, #2
	ldr       r10, [sp, #0x18]
	sub       r5, r0, r7, lsl #1
	mov       r4, r10, lsl #1
|$L38898|

; 5246 :         for ( i = 0; i < 2; i++) {
; 5247 :             I32_WMV j;
; 5248 :             for ( j = 0; j < 8; j++) {

	mov       r2, r8
    IF _XSC_=1
        PLD [r2, #32]
    ENDIF
|$L38902|

; 5249 :                 I32_WMV k = pSrcTop[j] + g_iOverlapOffset;
; 5250 :                 pDst[j] = (k < 0) ? 0 : ((k > 255) ? 255 : k);

;num = 0~3	
	ldrsh     r0, [r2], #2
	ldrsh     r1, [r2], #2
	ldrsh     r12, [r2], #2
	ldrsh     r9, [r2], #2

	adds      r0, r0, #0x80
	adds      r1, r1, #0x80
	adds      r12, r12, #0x80
	adds      r9, r9, #0x80
	bics      r10, r0, #0xFF
	mvnne     r0, r0, asr #31
	bics      r10, r1, #0xFF
	mvnne     r1, r1, asr #31
	bics      r10, r12, #0xFF
	mvnne     r12, r12, asr #31
	bics      r10, r9, #0xFF
	mvnne     r9, r9, asr #31
	strb      r0, [r5, #0]
	strb      r1, [r5, #1]
	strb      r12, [r5, #2]
	strb      r9, [r5, #3]

;num = 4~7	
	ldrsh     r0, [r2], #2
	ldrsh     r1, [r2], #2
	ldrsh     r12, [r2], #2
	ldrsh     r9, [r2], #2

	adds      r0, r0, #0x80
	adds      r1, r1, #0x80
	adds      r12, r12, #0x80
	adds      r9, r9, #0x80
	bics      r10, r0, #0xFF
	mvnne     r0, r0, asr #31
	bics      r10, r1, #0xFF
	mvnne     r1, r1, asr #31
	bics      r10, r12, #0xFF
	mvnne     r12, r12, asr #31
	bics      r10, r9, #0xFF
	mvnne     r9, r9, asr #31
	strb      r0, [r5, #4]
	strb      r1, [r5, #5]
	strb      r12, [r5, #6]
	strb      r9, [r5, #7]

	subs       r6, r6, #1

; 5251 :             }
; 5252 :             pSrcTop += iSrcStride;

	add       r8, r8, r4

; 5253 :             pDst += iDstStride;

	add       r5, r5, r7
	bne       |$L38898|

; 5267 :         }
; 5268 :     }
; 5269 : }

	add       sp, sp, #0x24
	ldmia     sp!, {r4 - r12, pc}
|$L38896|

; 5254 :         }
; 5255 :     }
; 5256 :     // remaining 6 of current
; 5257 :     else if (bCurrent) {

	cmp       r0, #0
	beq       |$L38911|

; 5258 :         I32_WMV i;
; 5259 :         for ( i = 0; i < (bWindup ? 2 : 6); i++) {

	ldr       r1, [sp, #0x58] ;[#0x54]
	ldr       r5, [sp, #0x4C] ;[#0x48]
	mov       r6, #0
	ldr       r10, [sp, #0x18]
	mov       r11, #2
	ldr       r7, [sp, #0xC]
|$L38909|
	cmp       r1, #0
	mov       r0, r11
	moveq     r0, #6
	cmp       r6, r0
	bge       |$L38911|

; 5260 :             I32_WMV j;
; 5261 :             for ( j = 0; j < 8; j++) {

	ldr       r8, [sp, #0x1C]
	mov       r3, r8
|$L38913|

; 5262 :                 I32_WMV k = pSrcCurr[j] + g_iOverlapOffset;
; 5263 :                 pDst[j] = (k < 0) ? 0 : ((k > 255) ? 255 : k);
;num = 0~3	
	ldrsh     r0, [r3], #2
	ldrsh     r4, [r3], #2
	ldrsh     r12, [r3], #2
	ldrsh     r9, [r3], #2

	adds      r0, r0, #0x80
	adds      r4, r4, #0x80
	adds      r12, r12, #0x80
	adds      r9, r9, #0x80
	bics      r2, r0, #0xFF
	mvnne     r0, r0, asr #31
	bics      r2, r4, #0xFF
	mvnne     r4, r4, asr #31
	bics      r2, r12, #0xFF
	mvnne     r12, r12, asr #31
	bics      r2, r9, #0xFF
	mvnne     r9, r9, asr #31
	strb      r0, [r7, #0]
	strb      r4, [r7, #1]
	strb      r12, [r7, #2]
	strb      r9, [r7, #3]

;num = 4~7	
	ldrsh     r0, [r3], #2
	ldrsh     r4, [r3], #2
	ldrsh     r12, [r3], #2
	ldrsh     r9, [r3], #2

	adds      r0, r0, #0x80
	adds      r4, r4, #0x80
	adds      r12, r12, #0x80
	adds      r9, r9, #0x80
	bics      r2, r0, #0xFF
	mvnne     r0, r0, asr #31
	bics      r2, r4, #0xFF
	mvnne     r4, r4, asr #31
	bics      r2, r12, #0xFF
	mvnne     r12, r12, asr #31
	bics      r2, r9, #0xFF
	mvnne     r9, r9, asr #31
	strb      r0, [r7, #4]
	strb      r4, [r7, #5]
	strb      r12, [r7, #6]
	strb      r9, [r7, #7]

; 5264 :             }
; 5265 :             pSrcCurr += iSrcStride;

	add       r8, r8, r10, lsl #1
	str       r8, [sp, #0x1C]

; 5266 :             pDst += iDstStride;

	add       r7, r7, r5
	add       r6, r6, #1
	b         |$L38909|
|$M39664|
    WMV_ENTRY_END


    IF PRO_VER != 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    AREA    |.text|, CODE
    WMV_LEAF_ENTRY g_FilterHorizontalEdgeV9_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;pV5=r0
;iPixelDistance=r1
;iStepSize=r2
;iNumPixel=r3


  STMFD sp!, {r4 - r11, r14}       ; r0-r3 are preserved
  FRAME_PROFILE_COUNT
  LDR   r4,  =s_iNextPixel         ; r4 address of s_iNextPixel

gFHEOutloop

  IF _XSC_=1
      ADD   r8, r0, #32
      PLD   [r8, -r1, LSL #1]
      PLD   [r8, -r1]
      PLD   [r8]
      PLD   [r8, +r1]
  ENDIF
  
  ADD   r0,  r0,  #2
  MOV   r12, #0                    ; r12 = inner counter


gFHEInloop
  LDRB  r8, [ r0 ]                 ; r8 = v5
  LDRB  r7, [ r0 , -r1 ]           ; r7 = v4


; if abs(v4_v5)/2 == 0, break
  SUBS  r14,  r7,   r8             ; r14 = v4_v5
  MOV   r5,   r14                  ; r5  = v4_v5
  RSBLT r14,  r14,  #0
  MOVS  r14,  r14,  ASR #1         ; r14 = abs(v4_v5)/2
  BEQ   gFHEShortCut


; int a30 = (2*(v3_v6) - 5*v4_v5 + 4) >> 3;
  LDRB  r6,  [ r0, -r1, LSL #1 ]   ; r6  = v3
  LDRB  r9,  [ r0, r1 ]            ; r9  = v6
  ADD   r11, r5,  r5,  LSL #2      ; r11 = 5*v4_v5
  SUB   r10, r6,  r9               ; r10 = v3_v6
  RSB	r11, r11, #4			   ; 4 - 5*v4_v5
  ADD	r11, r11, r10, LSL #1 
  MOVS  r11, r11, ASR #3			; r11 = a30
  BEQ   gFHEShortCut
  MOVS  r5,  r11
  RSBLT r5,  r11, #0               ; r5 = abs(a30)


; if (iStepSize <= absA30) break
  CMP   r2, r5
  BLS   gFHEShortCut


; int a31 = (2 * (v1_v4) - 5 * v2_v3 + 4) >> 3; 
  LDRB  r10, [ r0, -r1, LSL #2]!   ; r10 = v1, r0 point to v1
  IF _XSC_=1
      PLD    [ r0, #32 ]
  ENDIF
  LDRB  r5,  [ r0 , r1 ]!          ; r5  = v2, r0 point to v2
  IF _XSC_=1
      PLD    [ r0, #32 ]
  ENDIF
  
  ADD   r0,  r0,  r1, LSL #2       ; r0 point to v6
  SUB   r5,  r5,  r6               ; r5  = v2_v3  
  SUB   r6,  r10, r7               ; r6  = v1_v4
  ADD   r5,  r5,  r5, LSL #2       ; r5  = 5*v2_v3
  RSB   r5,  r5,  r6, LSL #1 
  ADDS  r5,  r5,  #4
  MOV   r6,  r5,  ASR #3
  RSBLT r6,  r6,  #0               ; r6 = abs(a31)

  
; int a32 = (2 * (v5_v8) - 5 * v6_v7 + 4) >> 3;
  IF _XSC_=1
      add    r5, r0, #32
      PLD    [ r5, r1 ]
      PLD    [ r5, r1, LSL #1 ]
  ENDIF
  LDRB  r10, [ r0,  r1 ]          ; r10 = v7
  LDRB  r5,  [ r0,  r1, LSL #1 ]  ; r5  = v8
  SUB   r0,  r0,  r1              ; r0 point to v5
  SUB   r9,  r9,  r10             ; r9  = v6_v7  
  SUB   r10, r8,  r5              ; r10 = v5_v8
  ADD   r9,  r9,  r9,  LSL #2
  RSB   r9,  r9,  r10, LSL #1 
  ADDS  r9,  r9,  #4
  MOV   r10, r9,  ASR #3
  RSBLT r10, r10, #0              ; r10 = abs(a32)


; int iMina31_a32 = min(abs(a31),abs(a32)); 
  CMP   r6,  r10
  MOVLT r10, r6                   ; r10 = iMina31_a32

  MOVS  r5,  r11
  RSBLT r5,  r11,  #0             ; r5 = abs(a30)


; if (absA30 <= iMina31_a32), break
  CMP   r5,  r10
  BLS   gFHEShortCut


  ; if (sign(v4_v5) == sign(a30)), break
  SUB   r9,  r7, r8
  TEQ   r11, r9
  BPL   gFHEInloopDone


; int d = 5*(abs(a30)-iMina31_a32)/8;
  SUB   r10, r5 , r10
  ADD   r10, r10, r10, LSL #2
  MOV   r9,  r10, ASR #3         ; r9 = d


; d = min(d, c)
  CMP   r14, r9
  MOVLT r9,  r14

; if (d ==0) break
  TEQ   r9, #0
  BEQ   gFHEInloopDone


; if (v4 < v5) d = -d
  CMP   r7,  r8
  RSBLT r9,  r9,  #0  

; *pV4 = v4 - d
  SUB   r7, r7, r9
  STRB  r7, [ r0, -r1 ]


; *pV5 = v5 + d;
  ADD   r8, r8, r9
  STRB  r8, [ r0 ]

  B     gFHEInloopDone


gFHEShortCut

; if (i==0) pV5+=2, break
  TEQ   r12, #0                 
  ADDEQ r0,  r0,  #2
  BEQ   gFHEOutloopDone


gFHEInloopDone

; pV5 += s_iNextPixel[i]
  LDRSB r5,  [ r4, r12 ]   
  ADD   r12, r12, #1                ; update inner counter
  TEQ   r12, #4
  ADD   r0,  r0,  r5
  BNE   gFHEInloop


gFHEOutloopDone
  SUBS  r3,  r3,  #4                ; update outer counter
  BNE   gFHEOutloop
 
gFHEloopDoneV9
  LDMFD   sp!, {r4 - r11, PC}
  WMV_ENTRY_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    AREA    |.text|, CODE
    WMV_LEAF_ENTRY g_FilterVerticalEdgeV9_ARMV4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;pVStart=r0
;iPixelDistance=r1
;iStepSize=r2
;iNumPixel=r3


  STMFD sp!, {r4 - r11, r14}       ; r0-r3 are preserved
  FRAME_PROFILE_COUNT
  ;;;ADD   r0,  r0,  #5               ;    pointing to v5

gFVEOutloop
  ADD   r0,  r0,  r1, LSL #1
  MOV   r12, #0                    ; r12 = inner counter

gFVEInloop

  LDRB  r8, [ r0, #5 ]                 ; r8 = v5
  LDRB  r7, [ r0 , #4 ]           ; r7 = v4

  IF _XSC_=1
        PLD [r0, #32]
  ENDIF

; if abs(v4_v5)/2 == 0, break
  SUBS  r14,  r7,   r8             ; r14 = v4_v5
  MOV   r4,   r14                  ; r4  = v4_v5
  RSBLT r14,  r14,  #0             ; r14 = abs(v4_v5)
  MOVS  r14,  r14,  ASR #1         ; r14 = abs(v4_v5)/2
  BEQ   gFVEShortCut

  
; int a30 = (2*(v3-v6) - 5*v4_v5 + 4) >> 3;
  LDRB  r6, [ r0,  #3 ]           ; r6  = v3
  LDRB  r9, [ r0,  #6  ]           ; r9  = v6
  ADD   r11, r4,  r4,  LSL #2      ; r11 = 5*v4_v5
  SUB   r10, r6,  r9               ; r10 = v3_v6
  RSB	r11, r11, #4				;4 - 5*v4_v5
  ADD   r11, r11, r10, lsl #1               
  MOVS  r11, r11, ASR #3           ; r11 = a30
  BEQ   gFVEShortCut

  MOV   r10, r11
  RSBLT r10, r11, #0               ; r10 = abs(a30)


; if (iStepSize <= absA30) break
  CMP   r2, r10
  BLE   gFVEShortCut


; int a31 = (2 * (v1-v4) - 5 * v2_v3 + 4) >> 3;
  LDRB  r5,  [ r0 , #2 ]          ; r5 = v2
  LDRB  r4,  [ r0 , #1 ]          ; r4 = v1
  SUB   r5,  r5,  r6               ; r5 = v2_v3  
  SUB   r6,  r4,  r7
  ADD   r5,  r5,  r5, LSL  #2
  RSB   r5,  r5,  r6, LSL #1 
  ADDS  r5,  r5,  #4
  LDRB  r4,  [ r0 , #7  ]         ; r4 = v7
  MOV   r6,  r5,  ASR #3
 
; int a32 = (2 * (v5-v8) - 5 * v6_v7 + 4) >> 3; 
  
  LDRB  r5,  [ r0 , #8  ]         ; r5 = v8
  SUB   r9,  r9,  r4              ; r9 = v6_v7 
  RSBLT r6,  r6,  #0               ; r6 = abs(a31)
  ADD   r9,  r9,  r9,  LSL  #2
  SUB   r4,  r8,  r5              ; r4 = v5_v8
  RSB	r9, r9, #4				  ; r9 = 4 - 5 * v6_v7
  ADDS  r9,  r9,  r4,  LSL #1 
  MOV   r5,  r9,  ASR #3
  RSBLT r5,  r5,  #0              ; r5 = abs(a32)


; int iMina31_a32 = min(abs(a31),abs(a32)); 
  CMP   r6,  r5
  MOVLT r5,  r6                   ; r5 = iMina31_a32


; if (absA30 <= iMina31_a32), break
  CMP   r10, r5
  BLE   gFVEShortCut

; if (sign(v4_v5) == sign(a30)), break
  SUB   r9,  r7,  r8
  TEQ   r11, r9
  BPL   gFVEInloopDone


; int d = 5*(abs(a30)-iMina31_a32)/8;
  SUB   r10, r10, r5
  ADD   r10, r10, r10, LSL #2
  MOV   r9,  r10, ASR #3         ; r9 = d

; d = min(d, c)
  CMP   r14, r9
  MOVLT r9,  r14


; if (d ==0) break
  TEQ   r9, #0
  BEQ   gFVEInloopDone


; if (v4 < v5) d = -d
  CMP   r7,  r8
  RSBLT r9,  r9,  #0 


; *pV4 = v4 - d
  SUB  r7, r7, r9
  STRB r7, [ r0, #4 ]


; *pV5 = v5 + d;
  ADD  r8, r8, r9
  STRB r8, [ r0, #5 ]


  B gFVEInloopDone


gFVEShortCut

; if (i==0) pV5+=2*iPixelDistance, break
  TEQ   r12, #0
  ADDEQ r0,  r0,  r1,  LSL #1
  BEQ   gFVEOutloopDone


gFVEInloopDone

; pV5 += (s_iNextPixel[i]*iPixelDistance)
  TEQ   r12, #0                    
  SUBEQ r0,  r0,  r1,  LSL #1      ; if (i==0), pV5-=2*iPixelDistance 
  BEQ   gFVEInLoopShort

  TEQ   r12, #2
  ADDEQ r0,  r0,  r1,  LSL #1      ; if (i==2), pV5+=2*iPixelDistance
  BEQ   gFVEInLoopShort

  ADD   r0,  r0,  r1               ; elseif (i==1 || i==3), pV5+=iPixelDistance

gFVEInLoopShort
  ADD   r12, r12, #1               ; update inner counter
  TEQ   r12, #4
  BNE   gFVEInloop


gFVEOutloopDone
  SUBS  r3,  r3,  #4               ; update outer counter
  BNE   gFVEOutloop

  
gFVEloopDoneV9
  
  LDMFD   sp!, {r4 - r11, PC}

  WMV_ENTRY_END  

  
	ENDIF ; PRO_VER
	ENDIF ; WMV_OPT_LOOPFILTER_ARM

    EXPORT end_loopfilter_ARMV4
end_loopfilter_ARMV4
    nop                 ; to know where previous function ends.
    
  END