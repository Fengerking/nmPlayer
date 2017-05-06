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
    
    EXPORT  |MC_InterChroma_4x4_neon|
    EXPORT  |MC_InterChroma_8x8_neon|
    
    EXPORT  |MC_InterChromaBi_4x4_neon|
    EXPORT  |MC_InterChromaBi_8x8_neon|

ChromaCoeff
    DCW      0,  64,  0,    0
    DCW     -2,  58,  10,  -2
    DCW     -4,  54,  16,  -2
    DCW     -6,  46,  28,  -4
    DCW     -4,  36,  36,  -4
    DCW     -4,  28,  46,  -6
    DCW     -2,  16,  54,  -4
    DCW     -2,  10,  58,  -2

;----------------------------------------------------------------
 macro
    Back_1Line
    
    sub             r0, r0, r1           ;go back 1 line of src data 
 mend   

 macro
    Back_1Column
 
    sub             r0, r0, #1           ;go back 1 columns of src data
 mend

 macro 
    Back_1Line_1Column
     
    sub             r0, r0, #1           ;go back 1 columns of src data
    sub             r0, r0, r1           ;go back 1 line of src data 
 mend    

 macro
    CDuplicateCoeff_8
    
    vdup.8          d0, d5[0]         ;coeff[0] (first pass)
    vdup.8          d1, d5[2]
    vdup.8          d2, d5[4]
    vdup.8          d3, d5[6]
 mend
 
 macro
    CDuplicateCoeff_16_4x4
    
    vdup.s16        d0, d5[0]         ;coeff[0] (second pass)
    vdup.s16        d1, d5[1]
    vdup.s16        d2, d5[2]
    vdup.s16        d3, d5[3] 
 mend

;---------------------------------------------------------------- 
 macro
    CLoadSource_FirstPass_4x4
    
    vld1.u8         {d6}, [r0], r1    ;load first 4-line src data
    vld1.u8         {d7}, [r0], r1
    vld1.u8         {d8}, [r0], r1
    vld1.u8         {d9}, [r0], r1
 mend
 
 macro 
    CLoadSource_FirstPass_4x3
    
    vld1.u8         {d6}, [r0], r1       ;load last 3-line src data
    vld1.u8         {d7}, [r0], r1
    vld1.u8         {d8}, [r0]
 mend

;---------------------------------------------------------------- 
;output q7 q8
 macro
    CInterpolate_FirstPass_4x4
    
    vshr.u64        q5, q3, #16       ;+(src_ptr[1]*coeff[2])
    vshr.u64        q6, q4, #16
    vzip.32         d10, d11
    vzip.32         d12, d13
    vmull.u8        q7, d10, d2
    vmull.u8        q8, d12, d2
         
    vshr.u64        q5, q3, #24       ;-(src_ptr[2]*coeff[3])
    vshr.u64        q6, q4, #24
    vzip.32         d10, d11
    vzip.32         d12, d13
    vmlsl.u8        q7, d10, d3
    vmlsl.u8        q8, d12, d3
         
    vshr.u64        q5, q3, #8        ;+(src_ptr[0]*coeff[1])
    vshr.u64        q6, q4, #8
    vzip.32         d10, d11
    vzip.32         d12, d13
    vmlal.u8        q7, d10, d1
    vmlal.u8        q8, d12, d1  
         
    vzip.32         d6, d7            ;-(src_ptr[-1]*coeff[0])
    vzip.32         d8, d9
    vmlsl.u8        q7, d6, d0
    vmlsl.u8        q8, d8, d0
 mend 
 
 macro
    CBitShift_FirstPass_4x4
    
    vmov.s16        q11, #0x2000        ;construct offset = -8192(-0x2000H)
 
    vsub.s16        q7, q7, q11
    vsub.s16        q8, q8, q11
 mend
 
;----------------------------------------------------------------
 macro
    CInterpolate_FirstPass_4x3
 
    vshr.u64        q5, q3, #16       ;+(src_ptr[1]*coeff[2])
    vshr.u64        d12, d8, #16
    vzip.32         d10, d11
    vmull.u8        q9, d10, d2
    vmull.u8        q10, d12, d2
         
    vshr.u64        q5, q3, #24       ;-(src_ptr[2]*coeff[3])
    vshr.u64        d12, d8, #24
    vzip.32         d10, d11
    vmlsl.u8        q9, d10, d3
    vmlsl.u8        q10, d12, d3
         
    vshr.u64        q5, q3, #8        ;+(src_ptr[0]*coeff[1])
    vshr.u64        d12, d8, #8
    vzip.32         d10, d11
    vmlal.u8        q9, d10, d1
    vmlal.u8        q10, d12, d1 
         
    vzip.32         d6, d7            ;-(src_ptr[-1]*coeff[0])
    vmlsl.u8        q9, d6, d0
    vmlsl.u8        q10, d8, d0
 mend
 
 macro
    CBitShift_FirstPass_4x3
 
    vsub.s16        q9, q9, q11
    vsub.s16        d20, d20, d22
 mend
 
 macro
    CBitShift_FirstPassOnly_4x4
    
    vqrshrun.s16    d18, q7, #6  ;shift/round/saturate to u8             
    vqrshrun.s16    d19, q8, #6
 
 mend
 
;----------------------------------------------------------------
 macro
    CInterpolate_SecondPass_4x4
    
    vmull.s16       q12, d14, d0
    vmull.s16       q13, d15, d0
    vmull.s16       q14, d16, d0
    vmull.s16       q15, d17, d0
         
    vmlal.s16       q12, d15, d1
    vmlal.s16       q13, d16, d1
    vmlal.s16       q14, d17, d1
    vmlal.s16       q15, d18, d1
         
    vmlal.s16       q12, d16, d2
    vmlal.s16       q13, d17, d2
    vmlal.s16       q14, d18, d2
    vmlal.s16       q15, d19, d2
         
    vmlal.s16       q12, d17, d3
    vmlal.s16       q13, d18, d3
    vmlal.s16       q14, d19, d3
    vmlal.s16       q15, d20, d3
 mend 
 
 macro
    CBitShift_SecondPass_4x4
    
    vmov.u32        q4,  #0x80000
    
    vadd.s32        q12, q12, q4
    vadd.s32        q13, q13, q4
    vadd.s32        q14, q14, q4
    vadd.s32        q15, q15, q4
    
    vqrshrun.s32    d20, q12, #12
    vqrshrun.s32    d21, q13, #12
    
    vqrshrun.s32    d22, q14, #12
    vqrshrun.s32    d23, q15, #12
    
    vqmovn.u16       d18, q10
    vqmovn.u16       d19, q11
 mend

;----------------------------------------------------------------  
 macro
    CLoadSource_SecondPassOnly_4x7
 
    vld1.u8         {d6}, [r0], r1    ;load src data
    vld1.u8         {d7}, [r0], r1
    vzip.32         d6, d7
         
    vld1.u8         {d8}, [r0], r1
    vld1.u8         {d9}, [r0], r1
    vzip.32         d8, d9
         
    vld1.u8         {d10}, [r0], r1
    vld1.u8         {d11}, [r0], r1
    vzip.32         d10, d11
         
    vld1.u8         {d12}, [r0], r1
         
    vext.8          d7, d6, d8, #4
    vext.8          d9, d8, d10, #4
    vext.8          d11, d10, d12, #4
 mend
 
;----------------------------------------------------------------
 macro
    CInterpolate_SecondPassOnly_4x4
    
    vmull.u8        q7, d8, d2
    vmull.u8        q8, d10, d2
         
    vmlsl.u8        q7, d9, d3
    vmlsl.u8        q8, d11, d3
         
    vmlal.u8        q7, d7, d1
    vmlal.u8        q8, d9, d1
         
    vmlsl.u8        q7, d6, d0
    vmlsl.u8        q8, d8, d0
 mend 
 
 macro
    CBitShift_SecondPassOnly_4x4
    
    vqrshrun.s16    d18, q7, #6  ;shift/round/saturate to u8
    vqrshrun.s16    d19, q8, #6 
 mend
 
;----------------------------------------------------------------
 macro
    CStoreResult_4x4
    
    vst1.32         {d18[0]}, [r2], r3
    vst1.32         {d18[1]}, [r2], r3
    vst1.32         {d19[0]}, [r2], r3
    vst1.32         {d19[1]}, [r2] 
 mend
  
; r0    unsigned char  *src,
; r1    int  srcStride,
; r2    unsigned char  dst,
; r3    int  dstStride,
; stack(r4) int xFrac,
; stack(r5) int yFrac
|MC_InterChroma_4x4_neon| PROC
    push            {r4-r5, lr}

    adr             r12, ChromaCoeff
    ldr             r4, [sp, #12]        ;load parameters from stack
    ldr             r5, [sp, #16]        ;load parameters from stack

    cmp             r4, #0               ;skip first_pass filter if xoffset=0
    beq             Chroma_secondpass_filter4x4_only

    cmp             r5, #0               ;skip second_pass filter if yoffset=0
    beq             Chroma_firstpass_filter4x4_only
    
;First pass: width x height = 4 x 7
    add             r4, r12, r4, lsl #3  ;calculate filter location
    vld1.16         {d4}, [r4]           ;load coeff_x
    vabs.s16        d5, d4

    Back_1Line_1Column
         
    CDuplicateCoeff_8
         
    CLoadSource_FirstPass_4x4       
         
    pld             [r0]
    pld             [r0, r1]    
    
    CInterpolate_FirstPass_4x4
    
    CBitShift_FirstPass_4x4
    
    CLoadSource_FirstPass_4x3
    
    CInterpolate_FirstPass_4x3
    
    CBitShift_FirstPass_4x3
    
;Second pass, use q7~q10 16-bit first pass result to perform an execution of vertical
;4-tap interpolation filter.
    add             r5, r12, r5, lsl #3
    vld1.16         {d5}, [r5]           ;load coeff_y   
    
    CDuplicateCoeff_16_4x4
    
    CInterpolate_SecondPass_4x4 
    
    CBitShift_SecondPass_4x4
         
    CStoreResult_4x4
    
    pop             {r4-r5, pc}
    
;-------------------------------------------------------    
Chroma_firstpass_filter4x4_only    
    add             r4, r12, r4, lsl #3 ;calculate filter location
    vld1.s16        {d4}, [r4]
    vabs.s16        d5, d4
    
    CDuplicateCoeff_8
         
    Back_1Column
         
    CLoadSource_FirstPass_4x4

    CInterpolate_FirstPass_4x4
    
    CBitShift_FirstPassOnly_4x4
    
    CStoreResult_4x4
         
    pop             {r4-r5, pc}
 
;-------------------------------------------------------    
Chroma_secondpass_filter4x4_only
    add             r5, r12, r5, lsl #3
    vld1.s16        {d4}, [r5]
    vabs.s16        d5, d4

    CDuplicateCoeff_8          
         
    Back_1Line
         
    CLoadSource_SecondPassOnly_4x7
         
    CInterpolate_SecondPassOnly_4x4 
    
    CBitShift_SecondPassOnly_4x4
         
    CStoreResult_4x4
        
    pop             {r4-r5, pc}
    
    ENDP

;----------------------------------------------------------------
 macro
    CBitShift_SecondPassBi_4x4
    
    vqshrn.s32     d20, q12, #6
    vqshrn.s32     d21, q13, #6
    vqshrn.s32     d22, q14, #6
    vqshrn.s32     d23, q15, #6
    
    vmov            q7, q10
    vmov            q8, q11
 mend

 macro
;output q7 q8
    CBitShift_SecondPassOnlyBi_4x4
    
    vmov.s16        q11, #0x2000        ;construct offset = -8192(-0x2000H)
 
    vsub.s16        q7, q7, q11
    vsub.s16        q8, q8, q11
 mend
 
 macro
    CStoreResultBi_4x4
    
    lsl             r3, #1
    
    vst1.16         {d14}, [r2], r3
    vst1.16         {d15}, [r2], r3
    vst1.16         {d16}, [r2], r3
    vst1.16         {d17}, [r2]
 mend

; r0    unsigned char  *ref0,
; r1    int  refStride0,
; r2    short *dst
; r3    int dstStride
; stack(r4) int xFrac0,
; stack(r5) int yFrac0
|MC_InterChromaBi_4x4_neon| PROC
    push            {r4-r5, lr}
        
    adr             r12, ChromaCoeff
    ldr             r4, [sp, #12]        ;load parameters from stack
    ldr             r5, [sp, #16]        ;load parameters from stack

    cmp             r4, #0               ;skip first_pass filter if xoffset=0
    beq             Chroma_bi_secondpass_filter4x4_only

    cmp             r5, #0               ;skip second_pass filter if yoffset=0
    beq             Chroma_bi_firstpass_filter4x4_only
    
    ;First pass: width x height = 4 x 7
    add             r4, r12, r4, lsl #3  ;calculate filter location
    vld1.16         {d4}, [r4]           ;load coeff_x
    vabs.s16        d5, d4

    Back_1Line_1Column
         
    CDuplicateCoeff_8
         
    CLoadSource_FirstPass_4x4       
         
    pld             [r0]
    pld             [r0, r1]    
    
    CInterpolate_FirstPass_4x4
    
    CBitShift_FirstPass_4x4
    
    CLoadSource_FirstPass_4x3
    
    CInterpolate_FirstPass_4x3
    
    CBitShift_FirstPass_4x3
    
;Second pass, use q7~q10 16-bit first pass result to perform an execution of vertical
;4-tap interpolation filter.
    add             r5, r12, r5, lsl #3
    vld1.16         {d5}, [r5]           ;load coeff_y   
    
    CDuplicateCoeff_16_4x4
    
    CInterpolate_SecondPass_4x4
    
    CBitShift_SecondPassBi_4x4
    
    CStoreResultBi_4x4
    
    pop             {r4-r5, pc}

Chroma_bi_firstpass_filter4x4_only
    add             r4, r12, r4, lsl #3 ;calculate filter location
    vld1.s16        {d4}, [r4]
    vabs.s16        d5, d4
    
    CDuplicateCoeff_8
    
    Back_1Column
    
    CLoadSource_FirstPass_4x4
    
    CInterpolate_FirstPass_4x4
    
    CBitShift_FirstPass_4x4
    
    CStoreResultBi_4x4
    
    pop             {r4-r5, pc}
    
Chroma_bi_secondpass_filter4x4_only
    add             r5, r12, r5, lsl #3
    vld1.s16        {d4}, [r5]
    vabs.s16        d5, d4

    CDuplicateCoeff_8          
         
    Back_1Line
    
    CLoadSource_SecondPassOnly_4x7
    
    CInterpolate_SecondPassOnly_4x4

    CBitShift_SecondPassOnlyBi_4x4
    
    CStoreResultBi_4x4
    
    pop             {r4-r5, pc}

    ENDP


;----------------------------------------------------------------
 macro
    CLoadSource_FirstPass_8x3
    
    vld1.u8         {q3}, [r0], r1    ;load first 3-line src data
    vld1.u8         {q4}, [r0], r1
    vld1.u8         {q5}, [r0], r1
    
    pld             [r0]
    pld             [r0, r1]
 mend

 macro
    CLoadSource_FirstPass_8x1
    
    vld1.u8         {q10}, [r0], r1
    
    pld             [r0]
 mend

;----------------------------------------------------------------
 macro
    CLoadSource_FirstPassOnly_8x4
    
    vld1.u8         {q3}, [r0], r1    ;load first 4-line src data
    vld1.u8         {q4}, [r0], r1
    vld1.u8         {q5}, [r0], r1
    vld1.u8         {q6}, [r0], r1
 mend

;---------------------------------------------------------------- 
 macro
    CLoadSource_SecondPassOnly_8x11
    
    vld1.u8         {d6},  [r0], r1
    vld1.u8         {d7},  [r0], r1
    vld1.u8         {d8},  [r0], r1
    vld1.u8         {d9},  [r0], r1
    vld1.u8         {d10}, [r0], r1
    vld1.u8         {d11}, [r0], r1
    vld1.u8         {d12}, [r0], r1
    vld1.u8         {d13}, [r0], r1
    vld1.u8         {d14}, [r0], r1
    vld1.u8         {d15}, [r0], r1
    vld1.u8         {d16}, [r0], r1
 mend

;---------------------------------------------------------------- 
 macro
    CInterpolate_FirstPass_8x3
    
    vext.8          d22, d6, d7, #2   ;+(src_ptr[1]*coeff[2])
    vext.8          d23, d8, d9, #2
    vext.8          d24, d10, d11, #2
         
    vmull.u8        q6, d22, d2
    vmull.u8        q7, d23, d2
    vmull.u8        q8, d24, d2
         
    vext.8          d22, d6, d7, #3   ;-(src_ptr[2]*coeff[3])
    vext.8          d23, d8, d9, #3
    vext.8          d24, d10, d11, #3
         
    vmlsl.u8        q6, d22, d3
    vmlsl.u8        q7, d23, d3
    vmlsl.u8        q8, d24, d3
         
    vext.8          d22, d6, d7, #1   ;+(src_ptr[3]*coeff[1])
    vext.8          d23, d8, d9, #1
    vext.8          d24, d10, d11, #1
         
    vmlal.u8        q6, d22, d1
    vmlal.u8        q7, d23, d1
    vmlal.u8        q8, d24, d1
         
    vmlsl.u8        q6, d6, d0
    vmlsl.u8        q7, d8, d0
    vmlsl.u8        q8, d10, d0
    
    ;vsub.s16        q6, q6, q14
    ;vsub.s16        q7, q7, q14
    ;vsub.s16        q8, q8, q14
 mend
 
 macro
    CBitShift_FirstPass_8x3
 
    vsub.s16        q6, q6, q14
    vsub.s16        q7, q7, q14
    vsub.s16        q8, q8, q14
 mend
 
 macro
    CInterpolate_FirstPass_8x1
    
    vext.8          d22, d20, d21, #2   ;+(src_ptr[1]*coeff[2])
    vmull.u8        q12, d22, d2
         
    vext.8          d22, d20, d21, #3   ;-(src_ptr[2]*coeff[3])
    vmlsl.u8        q12, d22, d3
         
    vext.8          d22, d20, d21, #1   ;+(src_ptr[3]*coeff[1])
    vmlal.u8        q12, d22, d1
         
    vmlsl.u8        q12, d20, d0
    
    ;vsub.s16        q9, q12, q14
 mend 
 
 macro
    CBitShift_FirstPass_8x1
    
    vsub.s16        q9, q12, q14
 mend 
 
;----------------------------------------------------------------
 macro
    CInterpolate_SecondPass_8x1
 
    vmull.s16       q11, d12, d6        ;second pass calculation.
    vmull.s16       q12, d13, d6
         
    vmlal.s16       q11, d14, d7
    vmlal.s16       q12, d15, d7
         
    vmlal.s16       q11, d16, d8
    vmlal.s16       q12, d17, d8
         
    vmlal.s16       q11, d18, d9 
    vmlal.s16       q12, d19, d9
 mend 
 
 macro
    CBitShift_SecondPass_8x1
    
    vadd.s32        q11, q11, q15
    vadd.s32        q12, q12, q15
    
    vqrshrun.s32    d26, q11, #12     ;shift/round/saturate to u8  
    vqrshrun.s32    d27, q12, #12
 mend

;----------------------------------------------------------------
;output q7 q8 q9 q10
 macro
    CInterpolate_FirstPassOnly_8x4
    
    vext.8          d22, d6, d7, #2   ;+(src_ptr[1]*coeff[2])
    vext.8          d23, d8, d9, #2
    vext.8          d24, d10, d11, #2
    vext.8          d25, d12, d13, #2
    
    vmull.u8        q7, d22, d2
    vmull.u8        q8, d23, d2
    vmull.u8        q9, d24, d2
    vmull.u8        q10, d25, d2
         
    vext.8          d22, d6, d7, #3   ;-(src_ptr[2]*coeff[3])
    vext.8          d23, d8, d9, #3
    vext.8          d24, d10, d11, #3
    vext.8          d25, d12, d13, #3
         
    vmlsl.u8        q7, d22, d3
    vmlsl.u8        q8, d23, d3
    vmlsl.u8        q9, d24, d3
    vmlsl.u8        q10, d25, d3
         
    vext.8          d22, d6, d7, #1   ;+(src_ptr[3]*coeff[1])
    vext.8          d23, d8, d9, #1
    vext.8          d24, d10, d11, #1
    vext.8          d25, d12, d13, #1
         
    vmlal.u8        q7, d22, d1
    vmlal.u8        q8, d23, d1
    vmlal.u8        q9, d24, d1
    vmlal.u8        q10, d25, d1
         
    vmlsl.u8        q7, d6, d0
    vmlsl.u8        q8, d8, d0
    vmlsl.u8        q9, d10, d0
    vmlsl.u8        q10, d12, d0
 mend   
 
 macro
    CBitShift_FirstPassOnly_8x4
 
    vqrshrun.s16    d26, q7, #6       ;shift/round/saturate to u8
    vqrshrun.s16    d27, q8, #6
    vqrshrun.s16    d28, q9, #6
    vqrshrun.s16    d29, q10, #6 
 mend
 
;---------------------------------------------------------------- 
;output q9 q10 q11 q12
 macro
    CInterpolate_SecondPassOnly_8x4
    
    vmull.u8        q9, d8, d2
    vmull.u8        q10, d9, d2
    vmull.u8        q11, d10, d2
    vmull.u8        q12, d11, d2
         
    vmlsl.u8        q9, d9, d3
    vmlsl.u8        q10, d10, d3
    vmlsl.u8        q11, d11, d3
    vmlsl.u8        q12, d12, d3
         
    vmlal.u8        q9, d7, d1
    vmlal.u8        q10, d8, d1
    vmlal.u8        q11, d9, d1
    vmlal.u8        q12, d10, d1
         
    vmlsl.u8        q9, d6, d0
    vmlsl.u8        q10, d7, d0
    vmlsl.u8        q11, d8, d0
    vmlsl.u8        q12, d9, d0
 mend
 
 macro 
    CBitShift_SecondPassOnly_8x4
 
    vqrshrun.s16    d26, q9, #6     ;shift/round/saturate to u8
    vqrshrun.s16    d27, q10, #6
    vqrshrun.s16    d28, q11, #6
    vqrshrun.s16    d29, q12, #6
 mend
 
 macro
    CStoreResult_Only_8x4
    
    vst1.u8         {d26}, [r2], r3   ;store result
    vst1.u8         {d27}, [r2], r3
    vst1.u8         {d28}, [r2], r3
    vst1.u8         {d29}, [r2], r3
 mend

; r0    unsigned char  *src,
; r1    int  srcStride,
; r2    unsigned char  dst,
; r3    int  dstStride,
; stack(r4) int xFrac,
; stack(r5) int yFrac
|MC_InterChroma_8x8_neon| PROC
    push            {r4-r5, lr}

    adrl            r12, ChromaCoeff
    ldr             r4, [sp, #12]       ;load parameters from stack
    ldr             r5, [sp, #16]       ;load parameters from stack

    cmp             r4, #0              ;skip first_pass filter if xoffset=0
    beq             Chroma_secondpass_filter8x8_only

    cmp             r5, #0              ;skip second_pass filter if yoffset=0
    beq             Chroma_firstpass_filter8x8_only
    
    add             r4, r12, r4, lsl #3 ;calculate filter location
    vld1.16         {d4}, [r4]          ;load coeff_x
    vabs.s16        d5, d4
    
    vmov.s16		q14, #0x2000
    vmov.u32        q15, #0x80000
    
    CDuplicateCoeff_8
         
    Back_1Line_1Column
         
    CLoadSource_FirstPass_8x3
         
    CInterpolate_FirstPass_8x3
    
    CBitShift_FirstPass_8x3

;q0~q1 coeff_x q6~q8 first pass temp value.  
    add             r5, r12, r5, lsl #3      
    vld1.16         {d5}, [r5]          ;load coeff_y
         
    vdup.s16        d6, d5[0]           ;coeff[0] (y)
    vdup.s16        d7, d5[1]
    vdup.s16        d8, d5[2]
    vdup.s16        d9, d5[3]
         
    mov             lr, #8
Chroma_8x8_loop_neon

    CLoadSource_FirstPass_8x1
    
    CInterpolate_FirstPass_8x1
    
    CBitShift_FirstPass_8x1
    
    CInterpolate_SecondPass_8x1
    
    CBitShift_SecondPass_8x1
    
    vqmovn.u16       d20, q13 
    vst1.u8         {d20}, [r2], r3     ;store result 
    
    vmov            q6, q7
    vmov            q7, q8
    vmov            q8, q9
         
    subs            lr, lr, #1     
    bne             Chroma_8x8_loop_neon
         
    pop             {r4-r5, pc}
    
;-------------------------------------------------------    
Chroma_firstpass_filter8x8_only
    add             r4, r12, r4, lsl #3  ;calculate filter location
    vld1.16         {d4}, [r4]           ;load coeff_x
    vabs.s16        d5, d4
         
    CDuplicateCoeff_8
         
    Back_1Column

    mov             lr, #2               ;loop counter 
Chroma_first_only_8x8_loop_neon  
       
    CLoadSource_FirstPassOnly_8x4
         
    CInterpolate_FirstPassOnly_8x4
    
    CBitShift_FirstPassOnly_8x4
    
    CStoreResult_Only_8x4
         
    subs            lr, lr, #1
    bne             Chroma_first_only_8x8_loop_neon
    
    pop             {r4-r5, pc}

;-------------------------------------------------------
Chroma_secondpass_filter8x8_only
    add             r5, r12, r5, lsl #3
    vld1.16         {d4}, [r5]        ;load coeff
    vabs.s16        d5, d4 
         
    CDuplicateCoeff_8
         
    Back_1Line
    
    CLoadSource_SecondPassOnly_8x11
         
    mov             lr, #2           ;loop counter  
Chroma_second_only_8x8_loop_neon         
         
    CInterpolate_SecondPassOnly_8x4
    
    CBitShift_SecondPassOnly_8x4
    
    CStoreResult_Only_8x4
    
    vmov            q3, q5
    vmov            q4, q6
    vmov            q5, q7
    vmov            d12, d16
         
    subs            lr, lr, #1
    bne             Chroma_second_only_8x8_loop_neon
    
    pop             {r4-r5, pc}
    
    ENDP	

;-------------------------------------------------------
 macro
    CBitShift_SecondPassBi_8x1
    
    vqshrn.s32      d20, q11, #6
    vqshrn.s32      d21, q12, #6
 mend
 
 macro
    CStoreResult_SecondPassBi_8x1
    
    vst1.16         {q10}, [r2], r3
 mend

 macro
    CBitShift_FirstPassOnlyBi_8x4
    
    vmov.s16         q14, #0x2000
    
    vsub.s16         q7, q7, q14
    vsub.s16         q8, q8, q14
    vsub.s16         q9, q9, q14
    vsub.s16         q10, q10, q14
 mend
 
 macro
    CStoreResult_FirstPassOnlyBi_8x4
    
    vst1.16          {q7}, [r2], r3
    vst1.16          {q8}, [r2], r3
    vst1.16          {q9}, [r2], r3
    vst1.16          {q10}, [r2], r3
 mend

 macro
    CBitShift_SecondPassOnlyBi_8x4
    
    vmov.s16         q14, #0x2000
    
    vsub.s16         q9, q9, q14
    vsub.s16         q10, q10, q14
    vsub.s16         q11, q11, q14
    vsub.s16         q12, q12, q14
 mend
 
 macro
    CStoreResult_SecondPassOnlyBi_8x4
 
    vst1.16          {q9}, [r2], r3
    vst1.16          {q10}, [r2], r3
    vst1.16          {q11}, [r2], r3
    vst1.16          {q12}, [r2], r3
 mend

; r0    unsigned char  *src,
; r1    int  srcStride,
; r2    unsigned char  dst,
; r3    int  dstStride,
; stack(r4) int xFrac,
; stack(r5) int yFrac	
|MC_InterChromaBi_8x8_neon| PROC
    push            {r4-r5, lr}
    
    lsl             r3, #1

    adrl            r12, ChromaCoeff
    ldr             r4, [sp, #12]       ;load parameters from stack
    ldr             r5, [sp, #16]       ;load parameters from stack

    cmp             r4, #0              ;skip first_pass filter if xoffset=0
    beq             Chroma_bi_secondpass_filter8x8_only

    cmp             r5, #0              ;skip second_pass filter if yoffset=0
    beq             Chroma_bi_firstpass_filter8x8_only
    
    add             r4, r12, r4, lsl #3 ;calculate filter location
    vld1.16         {d4}, [r4]          ;load coeff_x
    vabs.s16        d5, d4
    
    vmov.s16		q14, #0x2000
    ;vmov.u32        q15, #0x80000
    
    CDuplicateCoeff_8
    
    Back_1Line_1Column
         
    CLoadSource_FirstPass_8x3
         
    CInterpolate_FirstPass_8x3
    
    CBitShift_FirstPass_8x3
    
    add             r5, r12, r5, lsl #3      
    vld1.16         {d5}, [r5]          ;load coeff_y
    
    vdup.s16        d6, d5[0]           ;coeff[0] (y)
    vdup.s16        d7, d5[1]
    vdup.s16        d8, d5[2]
    vdup.s16        d9, d5[3]
    
    mov             lr, #8
Chroma_bi_8x8_loop_neon

    CLoadSource_FirstPass_8x1
         
    CInterpolate_FirstPass_8x1
    
    CBitShift_FirstPass_8x1
       
    CInterpolate_SecondPass_8x1
    
    CBitShift_SecondPassBi_8x1
    
    CStoreResult_SecondPassBi_8x1
    
    vmov            q6, q7
    vmov            q7, q8
    vmov            q8, q9
         
    subs            lr, lr, #1     
    bne             Chroma_bi_8x8_loop_neon
         
    pop             {r4-r5, pc}

;-------------------------------------------------------    
Chroma_bi_firstpass_filter8x8_only  
    add             r4, r12, r4, lsl #3  ;calculate filter location
    vld1.16         {d4}, [r4]           ;load coeff_x
    vabs.s16        d5, d4
         
    CDuplicateCoeff_8
         
    Back_1Column  
    
    mov             lr, #2               ;loop counter 
Chroma_bi_first_only_8x8_loop_neon  
       
    CLoadSource_FirstPassOnly_8x4
         
    CInterpolate_FirstPassOnly_8x4
    
    CBitShift_FirstPassOnlyBi_8x4
    
    CStoreResult_FirstPassOnlyBi_8x4
         
    subs            lr, lr, #1
    bne             Chroma_bi_first_only_8x8_loop_neon
    
    pop             {r4-r5, pc}
;-------------------------------------------------------    
Chroma_bi_secondpass_filter8x8_only
    add             r5, r12, r5, lsl #3
    vld1.16         {d4}, [r5]        ;load coeff
    vabs.s16        d5, d4 
         
    CDuplicateCoeff_8
         
    Back_1Line
    
    CLoadSource_SecondPassOnly_8x11

    mov             lr, #2           ;loop counter  
Chroma_bi_second_only_8x8_loop_neon     
         
    CInterpolate_SecondPassOnly_8x4
    
    CBitShift_SecondPassOnlyBi_8x4
    
    CStoreResult_SecondPassOnlyBi_8x4
    
    vmov            q3, q5
    vmov            q4, q6
    vmov            q5, q7
    vmov            d12, d16
    
    subs            lr, lr, #1
    bne             Chroma_bi_second_only_8x8_loop_neon
    
    pop             {r4-r5, pc}
    ENDP
	
;-------------------------------------------------------
	endif			;if MC_ASM_ENABLED==1
    END
	
