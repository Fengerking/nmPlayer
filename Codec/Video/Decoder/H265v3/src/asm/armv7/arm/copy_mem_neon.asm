;************************************************************************
; VisualOn Proprietary
; Copyright (c) 2012, VisualOn Incorporated. All rights Reserved
;
; VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
;
; All data and information contained in or disclosed by this document are
; confidential and proprietary information of VisualOn, and all rights
; therein are expressly reserved. By accepting this material, the
; recipient agrees that this material and the information contained
; therein are held in confidence and in trust. The material may only be
; used and/or disclosed as authorized in a license agreement controlling
; such use and disclosure.
;************************************************************************
	include     h265dec_ASM_config.h
	AREA |.text|, CODE, READONLY, ALIGN=2
    if MC_ASM_ENABLED==1
    
    EXPORT |CopyBlock_4x4_neon|
    EXPORT |CopyBlock_8x4_neon|
    EXPORT |CopyBlock_8x8_neon|
    EXPORT |CopyBlock_8x16_neon|
    EXPORT |CopyBlock_16x8_neon|
    EXPORT |CopyBlock_16x16_neon|
    EXPORT |AddAverageBi_4x4_neon|
    EXPORT |AddAverageBi_8x8_neon|

; r0    unsigned char  *src,
; r1    int  srcStride,
; r2    unsigned char  dst,
; r3    int  dstStride
; r0    unsigned char  *src,
; r1    int  srcStride,
; r2    unsigned char  dst,
; r3    int  dstStride
CopyBlock_4x4_neon ; PROC
    vld1.u8     {d0}, [r0], r1
    vld1.u8     {d1}, [r0], r1
    vst1.32     {d0[0]}, [r2], r3
    vld1.u8     {d2}, [r0], r1
    vst1.32     {d1[0]}, [r2], r3
    vld1.u8     {d3}, [r0], r1
    vst1.32     {d2[0]}, [r2], r3
    vst1.32     {d3[0]}, [r2], r3

    mov     pc, lr
; ENDP

; r0    unsigned char  *src,
; r1    int  srcStride,
; r2    unsigned char  dst,
; r3    int  dstStride
CopyBlock_8x4_neon ; PROC
    vld1.u8     {d0}, [r0], r1
    vld1.u8     {d1}, [r0], r1
    vst1.u8     {d0}, [r2], r3
    vld1.u8     {d2}, [r0], r1
    vst1.u8     {d1}, [r2], r3
    vld1.u8     {d3}, [r0], r1
    vst1.u8     {d2}, [r2], r3
    vst1.u8     {d3}, [r2], r3

    mov     pc, lr
; ENDP

; r0    unsigned char  *src,
; r1    int  srcStride,
; r2    unsigned char  dst,
; r3    int  dstStride    
CopyBlock_8x8_neon ; PROC
    vld1.u8     {d0}, [r0], r1
    vld1.u8     {d1}, [r0], r1
    vst1.u8     {d0}, [r2], r3
    vld1.u8     {d2}, [r0], r1
    vst1.u8     {d1}, [r2], r3
    vld1.u8     {d3}, [r0], r1
    vst1.u8     {d2}, [r2], r3
    vld1.u8     {d4}, [r0], r1
    vst1.u8     {d3}, [r2], r3
    vld1.u8     {d5}, [r0], r1
    vst1.u8     {d4}, [r2], r3
    vld1.u8     {d6}, [r0], r1
    vst1.u8     {d5}, [r2], r3
    vld1.u8     {d7}, [r0], r1
    vst1.u8     {d6}, [r2], r3
    vst1.u8     {d7}, [r2], r3

    mov     pc, lr
; ENDP

; r0    unsigned char  *src,
; r1    int  srcStride,
; r2    unsigned char  dst,
; r3    int  dstStride 
CopyBlock_8x16_neon ; PROC
    vld1.u8     {d0}, [r0], r1
    vld1.u8     {d1}, [r0], r1
    vst1.u8     {d0}, [r2], r3
    vld1.u8     {d2}, [r0], r1
    vst1.u8     {d1}, [r2], r3
    vld1.u8     {d3}, [r0], r1
    vst1.u8     {d2}, [r2], r3
    vld1.u8     {d4}, [r0], r1
    vst1.u8     {d3}, [r2], r3
    vld1.u8     {d5}, [r0], r1
    vst1.u8     {d4}, [r2], r3
    vld1.u8     {d6}, [r0], r1
    vst1.u8     {d5}, [r2], r3
    vld1.u8     {d7}, [r0], r1
    vst1.u8     {d6}, [r2], r3
    vst1.u8     {d7}, [r2], r3
    
    vld1.u8     {d0}, [r0], r1
    vld1.u8     {d1}, [r0], r1
    vst1.u8     {d0}, [r2], r3
    vld1.u8     {d2}, [r0], r1
    vst1.u8     {d1}, [r2], r3
    vld1.u8     {d3}, [r0], r1
    vst1.u8     {d2}, [r2], r3
    vld1.u8     {d4}, [r0], r1
    vst1.u8     {d3}, [r2], r3
    vld1.u8     {d5}, [r0], r1
    vst1.u8     {d4}, [r2], r3
    vld1.u8     {d6}, [r0], r1
    vst1.u8     {d5}, [r2], r3
    vld1.u8     {d7}, [r0], r1
    vst1.u8     {d6}, [r2], r3
    vst1.u8     {d7}, [r2], r3
; ENDP

; r0    unsigned char  *src,
; r1    int  srcStride,
; r2    unsigned char  dst,
; r3    int  dstStride      
CopyBlock_16x8_neon ; PROC
    vld1.u8     {q0}, [r0], r1
    vld1.u8     {q1}, [r0], r1
    vst1.u8     {q0}, [r2], r3
    vld1.u8     {q2}, [r0], r1
    vst1.u8     {q1}, [r2], r3
    vld1.u8     {q3}, [r0], r1
    vst1.u8     {q2}, [r2], r3
    vld1.u8     {q4}, [r0], r1
    vst1.u8     {q3}, [r2], r3
    vld1.u8     {q5}, [r0], r1
    vst1.u8     {q4}, [r2], r3
    vld1.u8     {q6}, [r0], r1
    vst1.u8     {q5}, [r2], r3
    vld1.u8     {q7}, [r0], r1
    vst1.u8     {q6}, [r2], r3
    vst1.u8     {q7}, [r2], r3
    
    mov     pc, lr
; ENDP

; r0    unsigned char  *src,
; r1    int  srcStride,
; r2    unsigned char  dst,
; r3    int  dstStride      
CopyBlock_16x16_neon ; PROC
    vld1.u8     {q0}, [r0], r1
    vld1.u8     {q1}, [r0], r1
    vld1.u8     {q2}, [r0], r1
    vst1.u8     {q0}, [r2], r3
    vld1.u8     {q3}, [r0], r1
    vst1.u8     {q1}, [r2], r3
    vld1.u8     {q4}, [r0], r1
    vst1.u8     {q2}, [r2], r3
    vld1.u8     {q5}, [r0], r1
    vst1.u8     {q3}, [r2], r3
    vld1.u8     {q6}, [r0], r1
    vst1.u8     {q4}, [r2], r3
    vld1.u8     {q7}, [r0], r1
    vst1.u8     {q5}, [r2], r3
    vld1.u8     {q8}, [r0], r1
    vst1.u8     {q6}, [r2], r3
    vld1.u8     {q9}, [r0], r1
    vst1.u8     {q7}, [r2], r3
    vld1.u8     {q10}, [r0], r1
    vst1.u8     {q8}, [r2], r3
    vld1.u8     {q11}, [r0], r1
    vst1.u8     {q9}, [r2], r3
    vld1.u8     {q12}, [r0], r1
    vst1.u8     {q10}, [r2], r3
    vld1.u8     {q13}, [r0], r1
    vst1.u8     {q11}, [r2], r3
    vld1.u8     {q14}, [r0], r1
    vst1.u8     {q12}, [r2], r3
    vld1.u8     {q15}, [r0], r1
    vst1.u8     {q13}, [r2], r3
    vst1.u8     {q14}, [r2], r3
    vst1.u8     {q15}, [r2], r3

    mov     pc, lr
; ENDP    

; r0  short* pred0, 
; r1  int predStride0, 
; r2  short* pred1, 
; r3  int predStride1, 
; stack(r4)  unsigned char* dst, 
; stack(r5)  int dstStride,    
AddAverageBi_4x4_neon ; PROC
    push {r4-r5, lr}
    
    ldr           r4, [sp, #12]      ;load parameters from stack
    ldr           r5, [sp, #16]      ;load parameters from stack
    
    lsl           r1, #1
    lsl           r3, #1
    
; load pred0 4x4    
    vld1.16       {d0}, [r0], r1
    vld1.16       {d1}, [r0], r1
    vld1.16       {d2}, [r0], r1
    vld1.16       {d3}, [r0], r1
    
; load pred1 4x4
    vld1.16       {d4}, [r2], r3
    vld1.16       {d5}, [r2], r3
    vld1.16       {d6}, [r2], r3
    vld1.16       {d7}, [r2], r3

; bi add and put the result to dst    
    vqadd.s16      d8, d0, d4
    vqadd.s16      d9, d1, d5
    vqadd.s16      d10, d2, d6
    vqadd.s16      d11, d3, d7
    
    vmov.s16      d30, #0x4000
    
    vqadd.s16      d12, d8, d30
    vqadd.s16      d13, d9, d30
    vqadd.s16      d14, d10, d30
    vqadd.s16      d15, d11, d30
    
; shift/round
    vqrshrun.s16  d16, q6, #7
    vqrshrun.s16  d17, q7, #7
    
    vst1.32       {d16[0]}, [r4], r5 
    vst1.32       {d16[1]}, [r4], r5 
    vst1.32       {d17[0]}, [r4], r5 
    vst1.32       {d17[1]}, [r4]
    
    pop  {r4-r5, pc}
; ENDP    
 
; r0  short* pred0, 
; r1  int predStride0, 
; r2  short* pred1, 
; r3  int predStride1, 
; stack(r4)  unsigned char* dst, 
; stack(r5)  int dstStride,   
 
;  int x, y;
;  int shift = IF_INTERNAL_PREC + 1 - g_bitDepth;
;  int offset = ( 1 << ( shift - 1 ) ) + 2 * IF_INTERNAL_OFFS;

;  for ( y = 0; y < height; y++ )
;  {
;    for ( x = 0; x < width; x ++ )
;    {
;      dst[ x + 0 ] = Clip( ( pred0[ x + 0 ] + pred1[ x + 0 ] + offset ) >> shift );
;    }

;    pred0 += predStride0;
;    pred1 += predStride1;
;    dst  += dstStride;
;  } 
AddAverageBi_8x8_neon ; PROC
    push {r4-r5, lr}
    
    ldr           r4, [sp, #12]      ;load parameters from stack
    ldr           r5, [sp, #16]      ;load parameters from stack
    
    lsl           r1, #1
    lsl           r3, #1
    
    mov           lr, #2             ;loop counter 
    
Add_average_bi_8x4_loop_neon    
    
; load pred0 4x4    
    vld1.16       {q0}, [r0], r1
    vld1.16       {q1}, [r0], r1
    vld1.16       {q2}, [r0], r1
    vld1.16       {q3}, [r0], r1
    
; load pred1 4x4
    vld1.16       {q4}, [r2], r3
    vld1.16       {q5}, [r2], r3
    vld1.16       {q6}, [r2], r3
    vld1.16       {q7}, [r2], r3
    
; bi add and put the result to dst    
    vqadd.s16      q8, q0, q4
    vqadd.s16      q9, q1, q5
    vqadd.s16      q10, q2, q6
    vqadd.s16      q11, q3, q7
    
    vmov.s16      q15, #0x4000
    
    vqadd.s16      q4, q8, q15
    vqadd.s16      q5, q9, q15
    vqadd.s16      q6, q10, q15
    vqadd.s16      q7, q11, q15    
    
; shift/round
    vqrshrun.s16  d16, q4, #7
    vqrshrun.s16  d17, q5, #7
    vqrshrun.s16  d18, q6, #7
    vqrshrun.s16  d19, q7, #7
    
    vst1.u8       {d16}, [r4], r5 
    vst1.u8       {d17}, [r4], r5 
    vst1.u8       {d18}, [r4], r5 
    vst1.u8       {d19}, [r4], r5
    
    subs          lr, lr, #1
    bne           Add_average_bi_8x4_loop_neon
    
    pop  {r4-r5, pc}
  
; ENDP 
	endif			;if MC_ASM_ENABLED==1
	end
    
