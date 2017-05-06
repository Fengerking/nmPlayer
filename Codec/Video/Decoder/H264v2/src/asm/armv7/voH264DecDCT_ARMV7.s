;*****************************************************************************
;*																			*
;*		VisualOn, Inc. Confidential and Proprietary, 2011 					*
;*																			*
;*****************************************************************************
    AREA	|.text|, CODE

	EXPORT	Idct4x4Add_ARMV7
	EXPORT	Idct4x4DCAdd_ARMV7
	EXPORT	Idct8x8Add_ARMV7
	
MPR_STRIDE EQU	 16

 align 8
Idct4x4DCAdd_ARMV7
;r0  dc
;r1  imgbuf1
;r2  dst_stride 
  mov			r3, r1
  vld1.32       {d4[0]}, [r1], r2
  vdup.16      q0,  r0
  vld1.32       {d4[1]}, [r1 ], r2
  vdup.16      q1,  r0
  vld1.32       {d5[0]}, [r1], r2
  vld1.32       {d5[1]}, [r1], r2
  vaddw.u8       q0, q0, d4
  vaddw.u8        q1, q1, d5
  vqmovun.s16     d0, q0
  vqmovun.s16     d2, q1

  vst1.32         {d0[0]}, [r3 ], r2
  vst1.32         {d0[1]}, [r3 ], r2
  vst1.32         {d2[0]}, [r3 ], r2
  vst1.32         {d2[1]}, [r3 ], r2
  bx                lr
 
Idct4x4Add_ARMV7
;r0  tblock
;r1  imgbuf1
;r2  dst_stride 
  vld4.16		{q0,q1},  [r0]		;d0 0,4,8,12
  vaddl.s16     q2, d0, d2          ;t0+t2
  vshr.s16      d8, d1, #1          ;t1>>1
  vshr.s16      d9, d3, #1          ;t3>>1   
  vsubl.s16     q3, d0, d2          ;t0-t2
  vsubl.s16     q5, d8, d3          ;(t1>>1)-t3
  vaddl.s16     q6, d1, d9          ;t1+(t3>>1)
  vadd.s32      q0, q2, q6
  vadd.s32      q1, q3, q5 
  vsub.s32		q4, q3, q5
  vsub.s32		q7, q2, q6
  
  vswp          d1, d8
  vswp          d3, d14
  vtrn.32       q0, q1
  vtrn.32       q4, q7
  
  vadd.s32      q2, q0, q4          ;t0+t2
  vshr.s32      q3, q1, #1          ;t1>>1
  vshr.s32      q5, q7, #1          ;t3>>1 
  vsub.s32      q6, q0, q4          ;t0-t2
  vsub.s32      q8, q3, q7          ;(t1>>1)-t3
  vadd.s32      q9, q1, q5          ;t1+(t3>>1)
  vadd.s32      q0, q2, q9
  vadd.s32      q1, q6, q8 
  vsub.s32		q4, q6, q8
  vsub.s32		q7, q2, q9

  mov			r3, r1
  vld1.32       {d4[0]}, [r1 ], r2
  vrshrn.s32     d0, q0, #6
  vld1.32       {d4[1]}, [r1 ], r2
  vrshrn.s32     d1, q1, #6
  vld1.32       {d5[0]}, [r1 ], r2
  vrshrn.s32     d2, q4, #6
  vld1.32       {d5[1]}, [r1 ], r2
  vrshrn.s32     d3, q7, #6
  vaddw.u8       q0, q0, d4
  vaddw.u8        q1, q1, d5
  vqmovun.s16     d0, q0
  vqmovun.s16     d2, q1

  vst1.32         {d0[0]}, [r3 ], r2
  vst1.32         {d0[1]}, [r3 ], r2
  vst1.32         {d2[0]}, [r3], r2
  vst1.32         {d2[1]}, [r3 ], r2
  
  bx                lr

Idct8x8Add_ARMV7
;r0  tblock
;r1  imgbuf1
;r2  dst_stride 

        vld1.16         {q8-q9},  [r0]!
        vld1.16         {q10-q11},[r0]!
        vld1.16         {q12-q13},[r0]!
        vld1.16         {q14-q15},[r0]!       

;hor
        vtrn.16         q8,  q9     ;t00 t10 t02 t12 t04 t14 t06 t16     t01 t11 t03 t13 t05 t15 t07 t17
        vtrn.16         q10, q11    ;t20 t30 t22 t32 t24 t34 t26 t36     t21 t31 t23 t33 t25 t35 t27 t37
        vtrn.32         q8,  q10    ;t00 t10 t20 t30 t04 t14 t24 t34     t02 t12 t22 t32 t06 t16 t26 t36
        vtrn.16         q12, q13    ;t40 t50 t42 t52 t44 t54 t46 t56     t41 t51 t43 t53 t45 t55 t47 t57
        vtrn.16         q14,  q15   ;t60 t70 t62 t72 t64 t74 t66 t76     t61 t71 t63 t73 t65 t75 t67 t77 
        vtrn.32         q9, q11     ;t01 t11 t21 t31 t05 t15 t25 t35     t03 t13 t23 t33 t05 t15 t25 t35
        vtrn.32         q12, q14    ;t40 t50 t60 t70 t44 t54 t64 t74     t42 t52 t62 t72 t46 t56 t66 t76
        vtrn.32         q13, q15    ;t41 t51 t61 t71 t45 t55 t65 t75     t43 t53 t63 t73 t47 t57 t67 t77
        
        vswp            d24, d17    ; t00 t10 t20 t30 t40 t50 t60 t70    t04 t14 t24 t34 t44 t54 t64 t74
        vswp            d26, d19    ; t01 t11 t21 t31 t41 t51 t61 t71    t05 t15 t25 t35 t45 t55 t65 t75  
        vswp            d28, d21    ; t02 t12 t22 t32 t42 t52 t62 t72    t06 t16 t26 t36 t46 t56 t66 t76 
        vswp            d30, d23    ; t03 t13 t23 t33 t43 t53 t63 t73    t07 t17 t27 t37 t47 t57 t67 t77
        
        vshr.s16        q2,  q10, #1
        vadd.i16        q0,  q8,  q12   ;a0
        vsub.i16        q1,  q8,  q12   ;a2
        vshr.s16        q3,  q14, #1
        vsub.i16        q2,  q2,  q14  ; a4
        vadd.i16        q3,  q3,  q10  ; a6

        vadd.i16        q10, q1,  q2  ;b2
        vsub.i16        q12, q1,  q2  ;b4
        vadd.i16        q8,  q0,  q3  ;b0
        vsub.i16        q2,  q0,  q3 ;b6
        vsub.i16        q0,  q13, q11
        vadd.i16        q1,  q15, q9
        vsub.i16        q14,  q15, q9
        vadd.i16        q3,  q13, q11
        vsub.i16        q0,  q0,  q15
        vsub.i16        q1,  q1,  q11
        vadd.i16        q14,  q14,  q13
        vadd.i16        q3,  q3,  q9
        vshr.s16        q9,  q9,  #1
        vshr.s16        q11, q11, #1
        vshr.s16        q13, q13, #1
        vshr.s16        q15, q15, #1
        vsub.i16        q0,  q0,  q15   ;a1
        vsub.i16        q1,  q1,  q11   ;a3
        vadd.i16        q14,  q14,  q13   ;a5
        vadd.i16        q3,  q3,  q9    ;a7
        vshr.s16        q9,  q0,  #2
        vshr.s16        q11, q1,  #2
        vshr.s16        q13, q14,  #2
        vshr.s16        q15, q3,  #2
        vsub.i16        q3,  q3,  q9 ;b7
        vsub.i16        q14,  q14, q11 ;b5
        vadd.i16        q1,  q1,  q13;b3
        vadd.i16        q0,  q0,  q15;b1

        vsub.i16        q15, q8,  q3;r7 = b0 - b7
        vadd.i16        q8,  q8,  q3;r0 = b0 + b7
        vsub.i16        q9,  q10, q14;r1 = b2 - b5
        vadd.i16        q14,  q10, q14;r6 = b2 + b5
        vadd.i16        q10, q12, q1;r2 = b4 + b3
        vadd.i16        q11, q2, q0;r3 = b6 + b1
        vsub.i16        q13, q12, q1;r5 = b4 - b3
        vsub.i16        q12, q2, q0;r4 = b6 - b1
 
 ;ver
        vtrn.16         q8,  q9     ;t00 t10 t02 t12 t04 t14 t06 t16     t01 t11 t03 t13 t05 t15 t07 t17
        vtrn.16         q10, q11    ;t20 t30 t22 t32 t24 t34 t26 t36     t21 t31 t23 t33 t25 t35 t27 t37
        vtrn.32         q8,  q10    ;t00 t10 t20 t30 t04 t14 t24 t34     t02 t12 t22 t32 t06 t16 t26 t36
        vtrn.16         q12, q13    ;t40 t50 t42 t52 t44 t54 t46 t56     t41 t51 t43 t53 t45 t55 t47 t57
        vtrn.16         q14,  q15   ;t60 t70 t62 t72 t64 t74 t66 t76     t61 t71 t63 t73 t65 t75 t67 t77 
        vtrn.32         q9, q11     ;t01 t11 t21 t31 t05 t15 t25 t35     t03 t13 t23 t33 t05 t15 t25 t35
        vtrn.32         q12, q14    ;t40 t50 t60 t70 t44 t54 t64 t74     t42 t52 t62 t72 t46 t56 t66 t76
        vtrn.32         q13, q15    ;t41 t51 t61 t71 t45 t55 t65 t75     t43 t53 t63 t73 t47 t57 t67 t77
        
        vswp            d24, d17    ; t00 t10 t20 t30 t40 t50 t60 t70    t04 t14 t24 t34 t44 t54 t64 t74
        vswp            d26, d19    ; t01 t11 t21 t31 t41 t51 t61 t71    t05 t15 t25 t35 t45 t55 t65 t75  
        vswp            d28, d21    ; t02 t12 t22 t32 t42 t52 t62 t72    t06 t16 t26 t36 t46 t56 t66 t76 
        vswp            d30, d23    ; t03 t13 t23 t33 t43 t53 t63 t73    t07 t17 t27 t37 t47 t57 t67 t77
        
        vshr.s16        q2,  q10, #1
        vadd.i16        q0,  q8,  q12   ;a0
        vsub.i16        q1,  q8,  q12   ;a2
        vshr.s16        q3,  q14, #1
        vsub.i16        q2,  q2,  q14  ; a4
        vadd.i16        q3,  q3,  q10  ; a6

        vadd.i16        q10, q1,  q2  ;b2
        vsub.i16        q12, q1,  q2  ;b4
        vadd.i16        q8,  q0,  q3  ;b0
        vsub.i16        q2,  q0,  q3 ;b6
        vsub.i16        q0,  q13, q11
        vadd.i16        q1,  q15, q9
        vsub.i16        q14,  q15, q9
        vadd.i16        q3,  q13, q11
        vsub.i16        q0,  q0,  q15
        vsub.i16        q1,  q1,  q11
        vadd.i16        q14,  q14,  q13
        vadd.i16        q3,  q3,  q9
        vshr.s16        q9,  q9,  #1
        vshr.s16        q11, q11, #1
        vshr.s16        q13, q13, #1
        vshr.s16        q15, q15, #1
        vsub.i16        q0,  q0,  q15   ;a1
        vsub.i16        q1,  q1,  q11   ;a3
        vadd.i16        q14,  q14,  q13   ;a5
        vadd.i16        q3,  q3,  q9    ;a7
        vshr.s16        q9,  q0,  #2
        vshr.s16        q11, q1,  #2
        vshr.s16        q13, q14,  #2
        vshr.s16        q15, q3,  #2
        vsub.i16        q3,  q3,  q9 ;b7
        vsub.i16        q14,  q14, q11 ;b5
        vadd.i16        q1,  q1,  q13;b3
        vadd.i16        q0,  q0,  q15;b1

        vsub.i16        q15, q8,  q3;r7 = b0 - b7
        vadd.i16        q8,  q8,  q3;r0 = b0 + b7
        vsub.i16        q9,  q10, q14;r1 = b2 - b5
        vadd.i16        q14,  q10, q14;r6 = b2 + b5
        vadd.i16        q10, q12, q1;r2 = b4 + b3
        vadd.i16        q11, q2, q0;r3 = b6 + b1
        vsub.i16        q13, q12, q1;r5 = b4 - b3
        vsub.i16        q12, q2, q0;r4 = b6 - b1
        
;output        

        mov             r3,  r1
        vrshr.s16       q8,  q8,  #6
        vld1.8          {d0},     [r1], r2
        vrshr.s16       q9,  q9,  #6
        vld1.8          {d1},     [r1], r2
        vrshr.s16       q10, q10, #6
        vld1.8          {d2},     [r1], r2
        vrshr.s16       q11, q11, #6
        vld1.8          {d3},     [r1], r2
        vrshr.s16       q12, q12, #6
        vld1.8          {d4},     [r1], r2
        vrshr.s16       q13, q13, #6
        vld1.8          {d5},     [r1], r2
        vrshr.s16       q14, q14, #6
        vld1.8          {d6},     [r1], r2
        vrshr.s16       q15, q15, #6
        vld1.8          {d7},     [r1], r2
                    
        vaddw.u8        q8,  q8,  d0
        vaddw.u8        q9,  q9,  d1
        vaddw.u8        q10, q10, d2        
        vaddw.u8        q11, q11, d3        
        vaddw.u8        q12, q12, d4        
        vaddw.u8        q13, q13, d5      
        vaddw.u8        q14, q14, d6        
        vaddw.u8        q15, q15, d7
 
;        vtrn.16         q8,  q9     ;t00 t10 t02 t12 t04 t14 t06 t16     t01 t11 t03 t13 t05 t15 t07 t17
;        vtrn.16         q10, q11    ;t20 t30 t22 t32 t24 t34 t26 t36     t21 t31 t23 t33 t25 t35 t27 t37
;        vtrn.32         q8,  q10    ;t00 t10 t20 t30 t04 t14 t24 t34     t02 t12 t22 t32 t06 t16 t26 t36
;        vtrn.16         q12, q13    ;t40 t50 t42 t52 t44 t54 t46 t56     t41 t51 t43 t53 t45 t55 t47 t57
;        vtrn.16         q14,  q15   ;t60 t70 t62 t72 t64 t74 t66 t76     t61 t71 t63 t73 t65 t75 t67 t77 
;        vtrn.32         q9, q11     ;t01 t11 t21 t31 t05 t15 t25 t35     t03 t13 t23 t33 t05 t15 t25 t35
;        vtrn.32         q12, q14    ;t40 t50 t60 t70 t44 t54 t64 t74     t42 t52 t62 t72 t46 t56 t66 t76
;        vtrn.32         q13, q15    ;t41 t51 t61 t71 t45 t55 t65 t75     t43 t53 t63 t73 t47 t57 t67 t77
        
;        vswp            d24, d17    ; t00 t10 t20 t30 t40 t50 t60 t70    t04 t14 t24 t34 t44 t54 t64 t74
;        vswp            d26, d19    ; t01 t11 t21 t31 t41 t51 t61 t71    t05 t15 t25 t35 t45 t55 t65 t75  
;        vswp            d28, d21    ; t02 t12 t22 t32 t42 t52 t62 t72    t06 t16 t26 t36 t46 t56 t66 t76 
;        vswp            d30, d23    ; t03 t13 t23 t33 t43 t53 t63 t73    t07 t17 t27 t37 t47 t57 t67 t77       
        
        
        vqmovun.s16     d0,  q8
        vqmovun.s16     d1,  q9
        vqmovun.s16     d2,  q10
        vqmovun.s16     d3,  q11
        vqmovun.s16     d4,  q12
        vqmovun.s16     d5,  q13
        vqmovun.s16     d6,  q14
        vqmovun.s16     d7,  q15      
        
        vst1.8          {d0},     [r3], r2
        vst1.8          {d1},     [r3], r2
        vst1.8          {d2},     [r3], r2
        vst1.8          {d3},     [r3], r2
        vst1.8          {d4},     [r3], r2
        vst1.8          {d5},     [r3], r2
        vst1.8          {d6},     [r3], r2
        vst1.8          {d7},     [r3], r2

        sub             r0,  r0,  #128

  bx              lr

  END

