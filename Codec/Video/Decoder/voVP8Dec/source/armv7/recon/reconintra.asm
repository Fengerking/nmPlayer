;
;  Copyright (c) 2010 The VP8 project authors. All Rights Reserved.
;
;  Use of this source code is governed by a BSD-style license and patent
;  grant that can be found in the LICENSE file in the root of the source
;  tree. All contributing project authors may be found in the AUTHORS
;  file in the root of the source tree.
;


    EXPORT  |build_tm_pred|
    ARM
    REQUIRE8
    PRESERVE8

    AREA ||.text||, CODE, READONLY, ALIGN=2
; r0    unsigned char  *yleft_col,
; r1    unsigned char  *yabove_row,
; r2    unsigned char  *ypred_ptr,
; r3    unsigned char   ytop_left,
; r4    unsigend int    ystride

|build_tm_pred| PROC
    push            {lr}
    
    vld1.u8     {q0}, [r0]  ;yleft_col
    vld1.u8     {q1}, [r1]  ;yabove_row
    
    vdup.u8     q2, d0[0] 
    vdup.u16     q3, r3
    
    ldr         r3, [sp, #0]               ; load y_stride
        
    vaddl.u8     q4,d2,d4            ;**********************
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3  
    vdup.u8     q2, d0[1]  
    vqmovn.u16    d12,q4        ;q6  0 row
    vqmovn.u16    d13,q5   
    
    ;vdup.u8     q2, d0[1]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3 
    vdup.u8     q2, d0[2]   
    vqmovn.u16    d14,q4        ;q7  1 row
    vqmovn.u16    d15,q5        ;********************
    
    ;vdup.u8     q2, d0[2]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d16,q4        ;q8  2 row
    vqmovn.u16    d17,q5
    
    vdup.u8     q2, d0[3]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d18,q4        ;q9  3 row
    vqmovn.u16    d19,q5
    
    vdup.u8     q2, d0[4]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d20,q4        ;q10  4 row
    vqmovn.u16    d21,q5
    
    vdup.u8     q2, d0[5]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d22,q4        ;q11  5 row
    vqmovn.u16    d23,q5
    
    vdup.u8     q2, d0[6]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d24,q4        ;q12  6 row
    vqmovn.u16    d25,q5
    
    vdup.u8     q2, d0[7]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d26,q4        ;q13  7 row
    vqmovn.u16    d27,q5
    
    
    vst1.u8      q6,[r2],r3
    vst1.u8      q7,[r2],r3
    vst1.u8      q8,[r2],r3
    vst1.u8      q9,[r2],r3
    vst1.u8      q10,[r2],r3
    vst1.u8      q11,[r2],r3
    vst1.u8      q12,[r2],r3
    vst1.u8      q13,[r2],r3
    
    vdup.u8     q2, d1[0]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d12,q4        ;q6  8 row
    vqmovn.u16    d13,q5   
    
    vdup.u8     q2, d1[1]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d14,q4        ;q7  9 row
    vqmovn.u16    d15,q5
    
    vdup.u8     q2, d1[2]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d16,q4        ;q8  10 row
    vqmovn.u16    d17,q5
    
    vdup.u8     q2, d1[3]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d18,q4        ;q9  11 row
    vqmovn.u16    d19,q5
    
    vdup.u8     q2, d1[4]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d20,q4        ;q10  12 row
    vqmovn.u16    d21,q5
    
    vdup.u8     q2, d1[5]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d22,q4        ;q11  13 row
    vqmovn.u16    d23,q5
    
    vdup.u8     q2, d1[6]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d24,q4        ;q12  14 row
    vqmovn.u16    d25,q5
    
    vdup.u8     q2, d1[7]
    vaddl.u8     q4,d2,d4 
    vaddl.u8     q5,d3,d5     
    vqsub.u16     q4,q4,q3
    vqsub.u16     q5,q5,q3    
    vqmovn.u16    d26,q4        ;q13  15 row
    vqmovn.u16    d27,q5
    
    vst1.u8      q6,[r2],r3
    vst1.u8      q7,[r2],r3
    vst1.u8      q8,[r2],r3
    vst1.u8      q9,[r2],r3
    vst1.u8      q10,[r2],r3
    vst1.u8      q11,[r2],r3
    vst1.u8      q12,[r2],r3
    vst1.u8      q13,[r2],r3

    pop             {pc}

    ENDP
    END