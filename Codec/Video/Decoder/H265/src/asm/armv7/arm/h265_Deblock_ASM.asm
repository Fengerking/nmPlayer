;/************************************************************************
;VisualOn Proprietary
;Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved
;
;VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
;
;All data and information contained in or disclosed by this document are
;confidential and proprietary information of VisualOn, and all rights
;therein are expressly reserved. By accepting this material, the
;recipient agrees that this material and the information contained
;therein are held in confidence and in trust. The material may only be
;used and/or disclosed as authorized in a license agreement controlling
;such use and disclosure.
;************************************************************************/
;
;/************************************************************************
;* ;file h265_deblock_neon.asm 
;*
;* H.265 decoder deblock filter functions neon optimization 
;*
;* ;author  Huaping Liu
;* ;date    2012-12-28
;************************************************************************/
	include     h265dec_ASM_config.h
	AREA     |.text|, CODE, READONLY, ALIGN=2
	if DEBLOCK_ASM_ENABLED==1
        EXPORT   voLumaInFilterASM_V
        EXPORT   voLumaInFilterASM_H
        EXPORT   voChromaInFilterASM_V
        EXPORT   voChromaInFilterASM_H
        EXPORT   voPelLumaWeakASM_V
        EXPORT   voPelLumaWeakASM_H

; Vertical weak filter, iStride = picture 
voPelLumaWeakASM_V   FUNCTION
        stmfd           r13!, {r4 - r12, r14}
        ; r0 --- piSrc, r1 --- iStride, r2 --- tc, r3 --- bFilterSecondP, r4 --- bFilterSecondQ, r5 --- 8line flag
        mov             r5, r2, lsr #1           ; tc2 = tc >> 1
        mov             r6, #10
        mul             r7, r2, r6               ; iThrCut = tc * 10
        vmov.i16        q1, #8
        vmov.i16        q9, #9
        vmov.i16        q10, #3
        vdup.u8         d25, r7                  ; d25 --- iThrCut
        sub             r6, r0, #2                                      ; piSrc[-2]
        ldr             r4, [r13, #40]            ; get bFilterSecond      
        ldr             r11, [r13, #44]
        
        sub             r8, r0, #3                ; piSrc[-3]
        vld3.8          {d7[0], d8[0], d9[0]}, [r8], r1          ; m1, m2, m3
        vld3.8          {d7[1], d8[1], d9[1]}, [r8], r1          ; m1, m2, m3
        vld3.8          {d7[2], d8[2], d9[2]}, [r8], r1          ; m1, m2, m3
        vld3.8          {d7[3], d8[3], d9[3]}, [r8], r1          ; m1, m2, m3
        cmp             r11, #1
        beq             V_Next

        vld3.8          {d7[4], d8[4], d9[4]}, [r8], r1          ; m1, m2, m3
        vld3.8          {d7[5], d8[5], d9[5]}, [r8], r1          ; m1, m2, m3
        vld3.8          {d7[6], d8[6], d9[6]}, [r8], r1          ; m1, m2, m3
        vld3.8          {d7[7], d8[7], d9[7]}, [r8], r1          ; m1, m2, m3        

V_Next
        mov             r8, r0
        vld3.8          {d10[0], d11[0], d12[0]}, [r8], r1      ; m4, m5, m6, m7
        vld3.8          {d10[1], d11[1], d12[1]}, [r8], r1      ; m4, m5, m6, m7        
        vld3.8          {d10[2], d11[2], d12[2]}, [r8], r1      ; m4, m5, m6, m7
        vld3.8          {d10[3], d11[3], d12[3]}, [r8], r1      ; m4, m5, m6, m7

        cmp             r11, #1
        beq             V_Start
        vld3.8          {d10[4], d11[4], d12[4]}, [r8], r1      ; m4, m5, m6, m7
        vld3.8          {d10[5], d11[5], d12[5]}, [r8], r1      ; m4, m5, m6, m7        
        vld3.8          {d10[6], d11[6], d12[6]}, [r8], r1      ; m4, m5, m6, m7
        vld3.8          {d10[7], d11[7], d12[7]}, [r8], r1      ; m4, m5, m6, m7           

V_Start

        ; delta = (9*(m4-m3) - 3*(m5-m2) + 8)>>4 ;
        vsubl.u8        q8, d10, d9                                     ; m4 - m3
        vsubl.u8        q2, d11, d8                                     ; m5 - m2
        vmul.s16        q8, q8, q9                                      ; 9*(m4 - m3)
        vmul.s16        q2, q2, q10                                     ; 3*(m5 - m2)
        vsub.s16        q8, q8, q2                                      ; 9*(m4 - m3) - 3 * (m5 - m2)
        vadd.s16        q8, q8, q1                                      ; + 8
        vdup.s8         d14, r2                                          ; tc
        vshrn.s16       d16, q8, #4                                     ; delta   --- d16

        ; r2 --- tc, r5 --- tc2
        vneg.s8         d15, d14                                         ; -tc
        vabs.s8         d24, d16                                         ; ABS(delta)
        vmin.s8         d18, d14, d16
        vclt.u8         d13, d24, d25                                    ; ABS(delta) < iThrCut --- d21
        vmax.s8         d18, d18, d15                                    ; d16 --- delta = Clip3(-tc, tc, delta)       
       
        vand.u8         d23, d18, d13  
        vmov.s16        q13, #1
        vadd.u8         d2, d23, d9                                     ; piSrc[-1] = (m3 + delta)
        vsub.u8         d3, d10, d23
        vmov.u8         d1, d8 
                             
        cmp             r3, #0
        beq             VQLab
        vaddl.u8        q2, d7, d9                                      ; m1 + m3
        vmovl.u8        q15, d8
        vsubw.s8        q8, q15, d18                                     ; m2 - delta
        vadd.s16        q2, q2, q13                                      ; m1 + m3 + 1

        vshr.s16        q2, q2, #1
        vdup.s8         d26, r5                                          ; tc2
        vsub.s16        q2, q2, q8
        vneg.s8         d27, d26                                         ; -tc2
        vshrn.s16       d22, q2, #1                                      ; ((m1+m3+1)>>1)- (m2 - delta))>>1)
        vmin.s8         d22, d22, d26
        vmax.s8         d22, d22, d27                                     ; delta1
        vand.u8         d26, d22, d13
        vadd.u8         d1, d8, d26                                     ; piSrc[-2]
VQLab
        vmov.s16        q13, #1
        vmov.u8         d4, d11    
        cmp             r4, #0
        beq             WeakvEnd
        vaddl.u8        q10, d10, d12                                    ; m4 + m6
        vmovl.u8        q15, d11
        vaddw.s8        q11, q15, d18                                    ; m5 + delta
        vadd.s16        q10, q10, q13                                      ; m4 + m6 + 1

        vshr.s16        q10, q10, #1
        vsub.s16        q14, q10, q11
        vdup.s8         d26, r5                                          ; tc2
        vneg.s8         d27, d26                                         ; -tc2
        vshrn.s16       d22, q14, #1                                     ; ((m4+m6+1)>>1)- (m5 + delta))>>1)
        vmin.s8         d22, d22, d26
        vmax.s8         d22, d22, d27                                     ; delta1
        vand.u8         d23, d22, d13
        vadd.u8         d4, d4, d23                                     ; piSrc[1]
         
WeakvEnd
        vst4.8          {d1[0], d2[0], d3[0], d4[0]}, [r6], r1
        vst4.8          {d1[1], d2[1], d3[1], d4[1]}, [r6], r1
        vst4.8          {d1[2], d2[2], d3[2], d4[2]}, [r6], r1
        vst4.8          {d1[3], d2[3], d3[3], d4[3]}, [r6], r1
        cmp             r11, #1
        beq             V_END
        vst4.8          {d1[4], d2[4], d3[4], d4[4]}, [r6], r1
        vst4.8          {d1[5], d2[5], d3[5], d4[5]}, [r6], r1
        vst4.8          {d1[6], d2[6], d3[6], d4[6]}, [r6], r1
        vst4.8          {d1[7], d2[7], d3[7], d4[7]}, [r6], r1
V_END

        ldmfd           r13!, {r4 - r12, r15}
        endfunc

; Horizontal Luma weak filter,  iOffset = Picture Width
voPelLumaWeakASM_H  FUNCTION

        stmfd           r13!, {r4 - r12, r14}
        ; r0 --- piSrc, r1 --- iStride, r2 --- tc, r3 --- bFilterSecondP, r4 --- bFilterSecondQ, r5 --- 8line flag
        mov             r5, r2, lsr #1           ; tc2 = tc >> 1
        mov             r6, #10
        mul             r7, r2, r6               ; iThrCut = tc * 10
        vmov.i16        q1, #8
        vmov.i16        q9, #9
        vmov.i16        q10, #3
        vdup.u8         d25, r7                  ; d25 --- iThrCut
        sub             r6, r0, r1, lsl #1      ; piSrc[-4 * iStride]
        sub             r6, r6, r1
        ldr             r4, [r13, #40]            ; get bFilterSecond   
        ldr             r11, [r13, #44]

        cmp             r11, #2
        beq             H_Next

        vld1.32         {d7[0]}, [r6], r1       ; m1, m1, m1, m1
        vld1.32         {d8[0]}, [r6], r1       ; m2, m2, m2, m2
        vld1.32         {d9[0]}, [r6], r1       ; m3, m3, m3, m3
        vld1.32         {d10[0]}, [r6], r1      ; m4, m4, m4, m4
        vld1.32         {d11[0]}, [r6], r1      ; m5, m5, m5, m5
        vld1.32         {d12[0]}, [r6], r1      ; m6, m6, m6, m6
        b               H_Start
H_Next
        vld1.64         {d7}, [r6], r1       ; m1, m1, m1, m1
        vld1.64         {d8}, [r6], r1       ; m2, m2, m2, m2
        vld1.64         {d9}, [r6], r1       ; m3, m3, m3, m3
        vld1.64         {d10}, [r6], r1      ; m4, m4, m4, m4
        vld1.64         {d11}, [r6], r1      ; m5, m5, m5, m5
        vld1.64         {d12}, [r6], r1      ; m6, m6, m6, m6

H_Start
        
        ; delta = (9*(m4-m3) - 3*(m5-m2) + 8)>>4 ;
        vsubl.u8        q8, d10, d9                                     ; m4 - m3
        vsubl.u8        q2, d11, d8                                     ; m5 - m2
        vmul.s16        q8, q8, q9                                      ; 9*(m4 - m3)
        vmul.s16        q2, q2, q10                                     ; 3*(m5 - m2)
        vsub.s16        q8, q8, q2                                      ; 9*(m4 - m3) - 3 * (m5 - m2)
        vadd.s16        q8, q8, q1                                      ; + 8
        vdup.s8         d14, r2                                          ; tc
        vshrn.s16       d16, q8, #4                                     ; delta   --- d16

        ; r2 --- tc, r5 --- tc2    
        vabs.s8         d24, d16                                         ; ABS(delta)
        vneg.s8         d15, d14                                         ; -tc
        vclt.u8         d13, d24, d25                                    ; ABS(delta) < iThrCut --- d21
 
        vmin.s8         d18, d14, d16
        vmax.s8         d18, d18, d15                                    ; d16 --- delta = Clip3(-tc, tc, delta)       
       
        vand.u8         d23, d18, d13 
        vadd.u8         d2, d23, d9                                     ; piSrc[-1] = (m3 + delta)
        vsub.u8         d3, d10, d23
        vmov.s16        q13, #1                                    
        vmov.u8         d1, d8 
                             
        cmp             r3, #0
        beq             HQLab
        vaddl.u8        q2, d7, d9                                      ; m1 + m3
        vmovl.u8        q15, d8
        vsubw.s8        q8, q15, d18                                     ; m2 - delta
        vadd.s16        q2, q2, q13                                      ; m1 + m3 + 1
        vshr.s16        q2, q2, #1
        vdup.s8         d26, r5                                          ; tc2
        vsub.s16        q2, q2, q8
        vneg.s8         d27, d26                                         ; -tc2
        vshrn.s16       d22, q2, #1                                      ; ((m1+m3+1)>>1)- (m2 - delta))>>1)
        vmin.s8         d22, d22, d26
        vmax.s8         d22, d22, d27                                     ; delta1
        vand.u8         d26, d22, d13
        vadd.u8         d1, d8, d26                                     ; piSrc[-2]
      
HQLab
        vmov.s16        q13, #1
        vmov.u8         d4, d11    
        cmp             r4, #0
        beq             WeakHEnd
        vaddl.u8        q10, d10, d12                                    ; m4 + m6
        vmovl.u8        q15, d11
        vaddw.s8        q11, q15, d18                                    ; m5 + delta
        vadd.s16        q10, q10, q13                                      ; m4 + m6 + 1
        vshr.s16        q10, q10, #1
        vdup.s8         d26, r5                                          ; tc2
        vsub.s16        q14, q10, q11
        vneg.s8         d27, d26                                         ; -tc2
        vshrn.s16       d22, q14, #1                                     ; ((m4+m6+1)>>1)- (m5 + delta))>>1)
        vmin.s8         d22, d22, d26
        vmax.s8         d22, d22, d27                                     ; delta1
        vand.u8         d23, d22, d13
        vadd.u8         d4, d4, d23                                     ; piSrc[1]

WeakHEnd
        sub             r6, r0, r1, lsl #1                              ; piSrc[-2*iStride]
        cmp             r11, #2
        beq             H_LAB

        vst1.32         {d1[0]}, [r6], r1
        vst1.32         {d2[0]}, [r6], r1
        vst1.32         {d3[0]}, [r6], r1
        vst1.32         {d4[0]}, [r6], r1
        b               H_END
H_LAB
        vst1.64         {d1}, [r6], r1
        vst1.64         {d2}, [r6], r1
        vst1.64         {d3}, [r6], r1
        vst1.64         {d4}, [r6], r1

H_END
        ldmfd           r13!, {r4 - r12, r15}
        endfunc         
        

; Vertical Luma filter, iOffset = 1, iStride = Picture Width  
voLumaInFilterASM_V      FUNCTION

	stmfd		r13!, {r4 - r12, r14}
        ; r0 --- piSrc, r1 --- iStride, r2 --- tc, r3 --- 1 or 2
        add             r7, r2, r2              ; nTemp = tc << 1

        vmov.i16        q7, #4
        vmov.i16        q15, #2

        sub             r6, r0, #4              ; piSrc[-4]
        vld4.8          {d6[0], d7[0], d8[0], d9[0]}, [r6], r1          ; m0, m1, m2, m3
        vld4.8          {d6[1], d7[1], d8[1], d9[1]}, [r6], r1          ; m0, m1, m2, m3
        vld4.8          {d6[2], d7[2], d8[2], d9[2]}, [r6], r1          ; m0, m1, m2, m3
        vld4.8          {d6[3], d7[3], d8[3], d9[3]}, [r6], r1          ; m0, m1, m2, m3
        cmp             r3, #1
        beq             LumaVX1

        vld4.8          {d6[4], d7[4], d8[4], d9[4]}, [r6], r1          ; m0, m1, m2, m3
        vld4.8          {d6[5], d7[5], d8[5], d9[5]}, [r6], r1          ; m0, m1, m2, m3
        vld4.8          {d6[6], d7[6], d8[6], d9[6]}, [r6], r1          ; m0, m1, m2, m3
        vld4.8          {d6[7], d7[7], d8[7], d9[7]}, [r6]              ; m0, m1, m2, m3

LumaVX1
        mov             r6, r0                  ; piSrc[0]
        vld4.8          {d10[0], d11[0], d12[0], d13[0]}, [r6], r1      ; m4, m5, m6, m7
        vld4.8          {d10[1], d11[1], d12[1], d13[1]}, [r6], r1      ; m4, m5, m6, m7        
        vld4.8          {d10[2], d11[2], d12[2], d13[2]}, [r6], r1      ; m4, m5, m6, m7
        vld4.8          {d10[3], d11[3], d12[3], d13[3]}, [r6], r1      ; m4, m5, m6, m7

        cmp             r3, #1
        beq             LumaVX2

        vld4.8          {d10[4], d11[4], d12[4], d13[4]}, [r6], r1      ; m4, m5, m6, m7
        vld4.8          {d10[5], d11[5], d12[5], d13[5]}, [r6], r1      ; m4, m5, m6, m7        
        vld4.8          {d10[6], d11[6], d12[6], d13[6]}, [r6], r1      ; m4, m5, m6, m7
        vld4.8          {d10[7], d11[7], d12[7], d13[7]}, [r6]          ; m4, m5, m6, m7
LumaVX2

        vdup.u8         d0, r7

        ;(2*m0 + 3*m1 + m2 + m3 + m4 + 4 )>>3
        vsubl.u8        q2, d7, d0                                      ; m1 - nTemp
        vaddl.u8        q1, d7, d0                                      ; m1 + nTemp

        vaddl.u8        q8, d6, d6                                      ; 2*m0
        vaddl.u8        q9, d7, d7                                      ; 2*m1
        vaddl.u8        q10, d8, d9                                     ; m2 + m3
        vaddl.u8        q11, d7, d10                                    ; m1 + m4
        vadd.s16        q8, q8, q9 
        vadd.s16        q8, q10, q8
        vadd.s16        q8, q11, q8
        vadd.s16        q8, q8, q7                                      ; (2*m0 + 3*m1 + m2 + m3 + m4 + 4 )
        vshr.s16        q8, q8, #3

        vmin.s16        q11, q1, q8
        vmax.s16        q11, q2, q11
        vqmovun.s16     d24, q11

        ; ((m1 + m2 + m3 + m4 + 2)>>2));
        vsubl.u8        q2, d8, d0                                      ; m2 - nTemp
        vaddl.u8        q1, d8, d0                                      ; m2 + nTemp
        vaddl.u8        q8, d7, d8
        vaddl.u8        q9, d9, d10
        vadd.s16        q10, q8, q9
        vadd.s16        q10, q10, q15                                    ; (m1 + m2 + m3 + m4 + 2)
        vshr.s16        q10, q10, #2
        vmin.s16        q11, q1, q10
        vmax.s16        q11, q2, q11
        vqmovun.s16     d25, q11

        ; (m1 + ((m2 + m3 + m4)<<1) + m5 + 4) >> 3)
        vsubl.u8        q2, d9, d0                                      ; m3 - nTemp
        vaddl.u8        q1, d9, d0                                      ; m3 + nTemp     
  
        vaddl.u8        q8, d8, d9
        vaddw.u8        q8, q8, d10
        vshl.s16        q8, #1
        vaddw.u8        q8, q8, d7
        vaddw.u8        q8, q8, d11
        vadd.s16        q8, q8, q7
        vshr.s16        q11, q8, #3
        vmin.s16        q11, q1, q11
        vmax.s16        q11, q2, q11
        vqmovun.s16     d26, q11
    
        ;(m2 + ((m3 + m4 + m5)<<1) + m6 + 4) >> 3)    
        vsubl.u8        q2, d10, d0                                     ; m4 - nTemp
        vaddl.u8        q1, d10, d0                                     ; m4 + nTemp

        vaddl.u8        q8, d9, d10
        vaddw.u8        q8, q8, d11
        vshl.s16        q8, #1
        vaddw.u8        q8, q8, d8
        vaddw.u8        q8, q8, d12
        vadd.s16        q8, q8, q7
        vshr.s16        q11, q8, #3
        vmin.s16        q11, q1, q11
        vmax.s16        q11, q2, q11
        vqmovun.s16     d27, q11
                   
        ;(m3 + m4 + m5 + m6 + 2)>>2)
        vsubl.u8        q2, d11, d0                                     ; m5 - nTemp
        vaddl.u8        q1, d11, d0                                     ; m5 + nTemp

        vaddl.u8        q8, d9, d10
        vaddw.u8        q8, q8, d11
        vaddw.u8        q8, q8, d12
        vadd.s16        q8, q8, q15
        vshr.s16        q11, q8, #2
        vmin.s16        q11, q1, q11
        vmax.s16        q11, q2, q11
        vqmovun.s16     d28, q11
        
        ;(m3 + m4 + m5 + 3*m6 + 2*m7 +4 )>>3)   
        vsubl.u8        q2, d12, d0                                     ; m6 - nTemp
        vaddl.u8        q1, d12, d0                                     ; m6 + nTemp

        vaddl.u8        q8, d12, d13
        vshl.s16        q8, #1
        vaddw.u8        q8, q8, d9
        vaddw.u8        q8, q8, d10
        vaddw.u8        q8, q8, d11
        vaddw.u8        q8, q8, d12
        vadd.s16        q8, q8, q7
        vshr.s16        q11, q8, #3
        vmin.s16        q11, q1, q11
        vmax.s16        q11, q2, q11
        vqmovun.s16     d29, q11        

        sub             r6, r0, #3             ; piSrc[-3]
        vst3.8          {d24[0], d25[0], d26[0]}, [r6], r1
        vst3.8          {d24[1], d25[1], d26[1]}, [r6], r1
        vst3.8          {d24[2], d25[2], d26[2]}, [r6], r1
        vst3.8          {d24[3], d25[3], d26[3]}, [r6], r1

        cmp             r3, #1
        beq             LumaVX3
        vst3.8          {d24[4], d25[4], d26[4]}, [r6], r1
        vst3.8          {d24[5], d25[5], d26[5]}, [r6], r1
        vst3.8          {d24[6], d25[6], d26[6]}, [r6], r1
        vst3.8          {d24[7], d25[7], d26[7]}, [r6]

LumaVX3        
        mov             r6, r0
        vst3.8          {d27[0], d28[0], d29[0]}, [r6], r1
        vst3.8          {d27[1], d28[1], d29[1]}, [r6], r1
        vst3.8          {d27[2], d28[2], d29[2]}, [r6], r1
        vst3.8          {d27[3], d28[3], d29[3]}, [r6], r1

        cmp             r3, #1
        beq             LumaV_END
        vst3.8          {d27[4], d28[4], d29[4]}, [r6], r1
        vst3.8          {d27[5], d28[5], d29[5]}, [r6], r1
        vst3.8          {d27[6], d28[6], d29[6]}, [r6], r1
        vst3.8          {d27[7], d28[7], d29[7]}, [r6]

LumaV_END
        ldmfd           r13!, {r4 - r12, r15}
        endfunc


;static __inline void voPelAllLumaFilter(VO_U8   *piSrc,
;					 VO_S32  iStride,
;					 VO_U8   tc
;					 )
;{
;	VO_S32 i;
;       VO_U8  m0,m1,m2,m3,m4,m5,m6,m7;
;	nTemp = tc << 1;
;	for (i = 0; i < 4; i++)
;	{	
;		m0  = piSrc[-iStride*4];
;		m1  = piSrc[-iStride*3];
;		m2  = piSrc[-iStride*2];
;		m3  = piSrc[-iStride];
;		m4  = piSrc[0];
;		m5  = piSrc[ iStride];
;		m6  = piSrc[ iStride*2];
;		m7  = piSrc[ iStride*3];
;
;		piSrc[-iStride*3] = Clip3(m1-nTemp, m1+nTemp, ((2*m0 + 3*m1 + m2 + m3 + m4 + 4 )>>3));
;		piSrc[-iStride*2] = Clip3(m2-nTemp, m2+nTemp, ((m1 + m2 + m3 + m4 + 2)>>2));
;		piSrc[-iStride]   = Clip3(m3-nTemp, m3+nTemp, ((m1 + 2*m2 + 2*m3 + 2*m4 + m5 + 4) >> 3));
;		piSrc[0]          = Clip3(m4-nTemp, m4+nTemp, ((m2 + 2*m3 + 2*m4 + 2*m5 + m6 + 4) >> 3));
;		piSrc[ iStride]   = Clip3(m5-nTemp, m5+nTemp, ((m3 + m4 + m5 + m6 + 2)>>2));
;		piSrc[ iStride*2] = Clip3(m6-nTemp, m6+nTemp, ((m3 + m4 + m5 + 3*m6 + 2*m7 +4 )>>3));
;
;		piSrc += 1;
;	}
;}
; Horizontal Luma filter, iOffset = Picture Width, iStride = 1
voLumaInFilterASM_H     FUNCTION

	stmfd		r13!, {r4 - r12, r14}
        ; r0 --- piSrc, r1 --- iStride, r2 --- tc, r3 --- 1 or 2
        
        sub             r6, r0, r1, lsl #2      ; piSrc[-4 * iStride]
        add             r7, r2, r2              ; nTemp = tc << 1
        vmov.i16        q7, #4
        vmov.i16        q15, #2

        cmp             r3, #2
        beq             LumaHX1

        vld1.32         {d6[0]}, [r6], r1       ; m0, m0, m0, m0
        vld1.32         {d7[0]}, [r6], r1       ; m1, m1, m1, m1
        vld1.32         {d8[0]}, [r6], r1       ; m2, m2, m2, m2
        vld1.32         {d9[0]}, [r6], r1       ; m3, m3, m3, m3
        vld1.32         {d10[0]}, [r6], r1      ; m4, m4, m4, m4
        vld1.32         {d11[0]}, [r6], r1      ; m5, m5, m5, m5
        vld1.32         {d12[0]}, [r6], r1      ; m6, m6, m6, m6
        vld1.32         {d13[0]}, [r6]          ; m7, m7, m7, m7
        b               LumaHX2

LumaHX1
        vld1.64         {d6}, [r6], r1      	; m0, m0, m0, m0
        vld1.64         {d7}, [r6], r1       	; m1, m1, m1, m1
        vld1.64         {d8}, [r6], r1       	; m2, m2, m2, m2
        vld1.64         {d9}, [r6], r1       	; m3, m3, m3, m3
        vld1.64         {d10}, [r6], r1      	; m4, m4, m4, m4
        vld1.64         {d11}, [r6], r1      	; m5, m5, m5, m5
        vld1.64         {d12}, [r6], r1      	; m6, m6, m6, m6
        vld1.64         {d13}, [r6]          	; m7, m7, m7, m7        

LumaHX2
        
        vdup.u8         d0, r7

        ;(2*m0 + 3*m1 + m2 + m3 + m4 + 4 )>>3
        vsubl.u8        q2, d7, d0                                      ; m1 - nTemp
        vaddl.u8        q1, d7, d0                                      ; m1 + nTemp

        vaddl.u8        q8, d6, d6                                      ; 2*m0
        vaddl.u8        q9, d7, d7                                      ; 2*m1
        vaddl.u8        q10, d8, d9                                     ; m2 + m3
        vaddl.u8        q11, d7, d10                                    ; m1 + m4
        vadd.s16        q8, q8, q9 
        vadd.s16        q8, q10, q8
        vadd.s16        q8, q11, q8
        vadd.s16        q8, q8, q7                                      ; (2*m0 + 3*m1 + m2 + m3 + m4 + 4 )
        vshr.s16        q8, q8, #3

        vmin.s16        q11, q1, q8
        vmax.s16        q11, q2, q11
        vqmovun.s16     d24, q11

        ; ((m1 + m2 + m3 + m4 + 2)>>2));
        vsubl.u8        q2, d8, d0                                      ; m2 - nTemp
        vaddl.u8        q1, d8, d0                                      ; m2 + nTemp
        vaddl.u8        q8, d7, d8
        vaddl.u8        q9, d9, d10
        vadd.s16        q10, q8, q9
        vadd.s16        q10, q10, q15                                    ; (m1 + m2 + m3 + m4 + 2)
        vshr.s16        q10, q10, #2
        vmin.s16        q11, q1, q10
        vmax.s16        q11, q2, q11
        vqmovun.s16     d25, q11

        ; (m1 + ((m2 + m3 + m4)<<1) + m5 + 4) >> 3)
        vsubl.u8        q2, d9, d0                                      ; m3 - nTemp
        vaddl.u8        q1, d9, d0                                      ; m3 + nTemp     
  
        vaddl.u8        q8, d8, d9
        vaddw.u8        q8, q8, d10
        vshl.s16        q8, #1
        vaddw.u8        q8, q8, d7
        vaddw.u8        q8, q8, d11
        vadd.s16        q8, q8, q7
        vshr.s16        q11, q8, #3
        vmin.s16        q11, q1, q11
        vmax.s16        q11, q2, q11
        vqmovun.s16     d26, q11
    
        ;(m2 + ((m3 + m4 + m5)<<1) + m6 + 4) >> 3)    
        vsubl.u8        q2, d10, d0                                     ; m4 - nTemp
        vaddl.u8        q1, d10, d0                                     ; m4 + nTemp

        vaddl.u8        q8, d9, d10
        vaddw.u8        q8, q8, d11
        vshl.s16        q8, #1
        vaddw.u8        q8, q8, d8
        vaddw.u8        q8, q8, d12
        vadd.s16        q8, q8, q7
        vshr.s16        q11, q8, #3
        vmin.s16        q11, q1, q11
        vmax.s16        q11, q2, q11
        vqmovun.s16     d27, q11
                   
        ;(m3 + m4 + m5 + m6 + 2)>>2)
        vsubl.u8        q2, d11, d0                                     ; m5 - nTemp
        vaddl.u8        q1, d11, d0                                     ; m5 + nTemp

        vaddl.u8        q8, d9, d10
        vaddw.u8        q8, q8, d11
        vaddw.u8        q8, q8, d12
        vadd.s16        q8, q8, q15
        vshr.s16        q11, q8, #2
        vmin.s16        q11, q1, q11
        vmax.s16        q11, q2, q11
        vqmovun.s16     d28, q11
        
        ;(m3 + m4 + m5 + 3*m6 + 2*m7 +4 )>>3)   
        vsubl.u8        q2, d12, d0                                     ; m6 - nTemp
        vaddl.u8        q1, d12, d0                                     ; m6 + nTemp

        vaddl.u8        q8, d12, d13
        vshl.s16        q8, #1
        vaddw.u8        q8, q8, d9
        vaddw.u8        q8, q8, d10
        vaddw.u8        q8, q8, d11
        vaddw.u8        q8, q8, d12
        vadd.s16        q8, q8, q7
        vshr.s16        q11, q8, #3
        vmin.s16        q11, q1, q11
        vmax.s16        q11, q2, q11
        vqmovun.s16     d29, q11        


        sub             r6, r0, r1, lsl #1
        sub             r6, r6, r1              ; piSrc[-3*iStride]
        cmp             r3, #2
        beq             LumaHX3

        vst1.32         {d24[0]}, [r6], r1
        vst1.32         {d25[0]}, [r6], r1
        vst1.32         {d26[0]}, [r6], r1
        vst1.32         {d27[0]}, [r6], r1
        vst1.32         {d28[0]}, [r6], r1
        vst1.32         {d29[0]}, [r6]
        b               LumaH_END

LumaHX3
        vst1.64         {d24}, [r6], r1
        vst1.64         {d25}, [r6], r1
        vst1.64         {d26}, [r6], r1
        vst1.64         {d27}, [r6], r1
        vst1.64         {d28}, [r6], r1
        vst1.64         {d29}, [r6]        

LumaH_END
        ldmfd           r13!, {r4 - r12, r15}
        endfunc


; Vertical Chroma filter, iOffset = 1, iStride = Picture Width 
;static __inline void voPelAllChromaFilter_V(VO_U8  *pCbSrc,
;					     VO_U8  *pCrSrc,
;					     VO_S32 iStride,
;					     VO_U8  tc
;	                                     )
;{
;	VO_S32 i;
;       VO_S8  delta;
;	VO_U8  m2, m3, m4, m5;
;	for (i = 0; i < 4; i++)
;	{
; 	        m2  = pCbSrc[-2];
;	        m3  = pCbSrc[-1];
;               m4  = pCbSrc[0];
;               m5  = pCbSrc[1];
;		delta = Clip3(-tc,tc, (((( m4 - m3 ) << 2 ) + m2 - m5 + 4 ) >> 3) );
;		pCbSrc[-1] = Clip(m3+delta);
;		pCbSrc[0] = Clip(m4-delta);
;
;		m2  = pCrSrc[-2];
;		m3  = pCrSrc[-1];
;		m4  = pCrSrc[0];
;		m5  = pCrSrc[1];
;		delta = Clip3(-tc,tc, (((( m4 - m3 ) << 2 ) + m2 - m5 + 4 ) >> 3) );
;		pCrSrc[-1] = Clip(m3+delta);
;		pCrSrc[0] = Clip(m4-delta);
;		pCbSrc += iStride;
;		pCrSrc += iStride;
;	}
;}
voChromaInFilterASM_V   FUNCTION
	stmfd		r13!, {r4 - r12, r14}
        ; r0 --- pCbSrc, r1 --- pCrSrc, r2 --- iStride, r3 --- tc
        sub		r8, r0, #2                ; pCbSrc[-2] address
        sub             r6, r1, #2                ; pCrSrc[-2] address
        mov             r4, #4

        vld4.8         {d6[0], d7[0], d8[0], d9[0]}, [r8], r2         ; Cb m02, m03, m04, m05
        vld4.8         {d6[1], d7[1], d8[1], d9[1]}, [r8], r2         ; Cb m12, m13, m14, m15
        vld4.8         {d6[2], d7[2], d8[2], d9[2]}, [r8], r2         ; Cb m22, m23, m24, m25
        vld4.8         {d6[3], d7[3], d8[3], d9[3]}, [r8]             ; Cb m32, m33, m34, m35

        vld4.8         {d6[4], d7[4], d8[4], d9[4]}, [r6], r2         ; Cr m02, m03, m04, m05
        vld4.8         {d6[5], d7[5], d8[5], d9[5]}, [r6], r2         ; Cr m12, m13, m14, m15
        vld4.8         {d6[6], d7[6], d8[6], d9[6]}, [r6], r2         ; Cr m22, m23, m24, m25
        vld4.8         {d6[7], d7[7], d8[7], d9[7]}, [r6]             ; Cr m32, m33, m34, m35 

        vdup.u8         d10, r3                   ; d10 : tc, tc, tc,..., tc
        vneg.s8         d11, d10                  ; d11 : -tc, -tc, ....., -tc
        
  
        vsubl.u8        q6, d8, d7                ; m4 - m3
        vsubl.u8        q7, d6, d9                ; m2 - m5

        vmov.i16        q8, #4                    ; 4
        vshl.s16        q9, q6, #2                ; (m4 - m3) << 2

        vadd.s16        q10, q9, q7               ; ((m4 - m3) << 2) + m2 - m5 
        vadd.s16        q10, q10, q8
        vshrn.s16       d14, q10, #3              

        vmin.s8         d14, d10, d14
        vmax.s8         d14, d11, d14             ; delta = clip3()
        
        vadd.s8         d7, d7, d14               ; m3 + delta
        vsub.s8         d8, d8, d14               ; m4 - delta
        
        ; Update Pels
        sub             r8, r0, #1                ; pCbSrc[-1] address
        sub             r6, r1, #1                ; pCrSrc[-1] address
        
        vst2.8          {d7[0], d8[0]}, [r8], r2
        vst2.8          {d7[1], d8[1]}, [r8], r2
        vst2.8          {d7[2], d8[2]}, [r8], r2
        vst2.8          {d7[3], d8[3]}, [r8]

        vst2.8          {d7[4], d8[4]}, [r6], r2
        vst2.8          {d7[5], d8[5]}, [r6], r2
        vst2.8          {d7[6], d8[6]}, [r6], r2
        vst2.8          {d7[7], d8[7]}, [r6]
        
ChromaV_END
        ldmfd           r13!, {r4 - r12, r15} 
        endfunc        

; Horizontal Chroma filter, iOffset = picture width, iStride = 1 
;static __inline void voPelAllChromaFilter_V(VO_U8  *pCbSrc,
;					     VO_U8  *pCrSrc,
;					     VO_S32 iOffset,
;					     VO_U8  tc
;	                                     )
;{
;	VO_S32 i;
;       VO_S8  delta;
;	VO_U8  m2, m3, m4, m5;
;	for (i = 0; i < 4; i++)
;	{
; 	        m2  = pCbSrc[-2*iOffset];
;	        m3  = pCbSrc[-iOffset];
;               m4  = pCbSrc[0];
;               m5  = pCbSrc[iOffset];
;		delta = Clip3(-tc,tc, (((( m4 - m3 ) << 2 ) + m2 - m5 + 4 ) >> 3) );
;		pCbSrc[-iOffset] = Clip(m3+delta);
;		pCbSrc[0] = Clip(m4-delta);
;
;		m2  = pCrSrc[-2*iOffset];
;		m3  = pCrSrc[-iOffset];
;		m4  = pCrSrc[0];
;		m5  = pCrSrc[iOffset];
;		delta = Clip3(-tc,tc, (((( m4 - m3 ) << 2 ) + m2 - m5 + 4 ) >> 3) );
;		pCrSrc[-iOffset] = Clip(m3+delta);
;		pCrSrc[0] = Clip(m4-delta);
;		pCbSrc += 1;
;		pCrSrc += 1;
;	}
;}

voChromaInFilterASM_H   FUNCTION
      
	stmfd		r13!, {r4 - r12, r14}
        ; r0 --- pCbSrc, r1 --- pCrSrc, r2 --- iOffset, r3 --- tc
        sub             r8, r0, r2, lsl #1        ; pCbSrc[-2*iOffset]
        sub             r6, r1, r2, lsl #1        ; pCrSrc[-2*iOffset]
        
        vdup.u8         d10, r3                   ; d10 : tc, tc, tc,..., tc
       
        vld1.32         {d6[0]}, [r8], r2         ; Cb, i = 0, 4 Row m2
        vld1.32         {d7[0]}, [r8], r2         ; Cb, i = 1, 4 Row m3
        vld1.32         {d8[0]}, [r8], r2         ; Cb, i = 2, 4 Row m4
        vld1.32         {d9[0]}, [r8]             ; Cb, i = 3, 4 Row m5

        vld1.32         {d6[1]}, [r6], r2         ; Cr, i = 0, 4 Row m2
        vld1.32         {d7[1]}, [r6], r2         ; Cr, i = 1, 4 Row m3
        vld1.32         {d8[1]}, [r6], r2         ; Cr, i = 2, 4 Row m4
        vld1.32         {d9[1]}, [r6]             ; Cr, i = 3, 4 Row m5

        vsubl.u8        q6, d8, d7                ; m4 - m3
        vneg.s8         d11, d10                  ; d11 : -tc, -tc, ....., -tc
        sub             r8, r0, r2                ; pCbSrc[-iOffset]
 
        vsubl.u8        q7, d6, d9                ; m2 - m5

        vmov.i16        q8, #4                    ; 4
        vshl.s16        q9, q6, #2                ; (m4 - m3) << 2

        vadd.s16        q10, q9, q7               ; ((m4 - m3) << 2) + m2 - m5 
        vadd.s16        q10, q10, q8
        vshrn.s16       d14, q10, #3              

        vmin.s8         d14, d10, d14
        vmax.s8         d14, d11, d14             ; delta = clip3()
        
        vadd.s8         d7, d7, d14               ; m3 + delta
        vsub.s8         d8, d8, d14               ; m4 - delta


        sub             r6, r1, r2                ; pCrSrc[-iOffset]

        vst1.32         {d7[0]}, [r8], r2     
        vst1.32         {d8[0]}, [r8]

        vst1.32         {d7[1]}, [r6], r2
        vst1.32         {d8[1]}, [r6]
                        
ChromaH_END
        ldmfd           r13!, {r4 - r12, r15}
        endfunc
        endif			;if DEBLOCK_ASM_ENABLED==1   
        end 

