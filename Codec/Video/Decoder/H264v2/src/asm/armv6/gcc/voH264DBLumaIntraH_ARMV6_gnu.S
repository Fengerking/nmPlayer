@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************
 #include "../../../defineID.h"
    .section	  .text

	.global  DeblockIntraLumaH_ARMV6
	
	.align 8	
	
@// Declare input registers

 p_pix1       .req r0
 n_StepArg    .req r1
 n_ystride    .req r1
 n_tC0Arg     .req r2
 n_alpha      .req r6

 n_beta       .req r7
 n_t7         .req r7
 n_bS         .req r14
 n_tC0        .req r14
 ptC0       .req r1

@// Declare Local/Temporary variables

@// Pixels
 p_0     .req r3 
 p_1     .req r5  
 p_2     .req r4  
 p_3     .req r2  
 q_0     .req r8  
 q_1     .req r9  
 q_2     .req r10 
 q_3     .req r12 


@// Filtering

 ap0q0   .req r1  
 filt    .req r2
        
 m00     .req r7
 m01     .req r11

 apflg   .req r0 
 aqflg   .req r6

 n_tC      .req r1


@//Declarations for bSLT4 kernel

 n_pos     .req r7
 voneg     .req r12

 P0a     .req r1   
 P1a     .req r8   
 Q0a     .req r7  
 Q1a     .req r4   

 u1      .req r3   
 max     .req r12
 min     .req r2   
               
                
                
@//Declarations for bSGE4 kernel

 q_3b    .req r9   
 p_3b    .req r0
 apqflg  .req r12

 n_P0b     .req r6
 n_P1b     .req r7 
 n_P2b     .req r1

 n_Q0b     .req r9 
 n_Q1b     .req r0 
 n_Q2b     .req r2

@// Miscellanous

 a       .req r0
 n_t0      .req r3 
 n_t1      .req r12
 n_t2      .req r7
 n_t3      .req r11
 n_t4      .req r4   
 n_t5      .req r1   
 n_t8      .req r6   
 n_t9      .req r14  
 n_t10     .req r5   
 n_t11     .req r9   

@// Filtering

 dp0q0       .req r12
 dp1p0       .req r12
 dq1q0       .req r12
 dp2p0       .req r12
 dq2q0       .req r12
 pskip       .req r12

 row0        .req r2
 row1        .req r4
 row2        .req r5
 row3        .req r3

 row4        .req r8
 row5        .req r9
 row6        .req r10
 row7        .req r12
 row8        .req r14
 row9        .req r7
            
 n_tpk0      .req r8
 n_tpk1      .req r9
 n_tpk2      .req r10
 n_tpk3      .req r12
 n_tpk4      .req r0

 n_tpk5      .req r1
 n_tpk6      .req r14
 n_tpk7      .req r2 
 n_tpk8      .req r5 
 n_tpk9      .req r6 

@// Unpacking
 mask        .req r11 

@//Declarations for bSGE4 kernel

 pQ0b        .req r0
 Stepb       .req r1
 maska       .req r14
 maskb       .req r14
            
 n_P0b         .req r6
 n_P1b         .req r7
 n_P2b         .req r1
 P3b         .req r3
            
 n_Q0b         .req r9 
 n_Q1b         .req r0   
 n_Q2b         .req r2 
 Q3b         .req r3 


@//
@//
@// Inputs - 3,4,5,8,9,10 - Input Pixels (p0-p2,q0-q2)
@//        - 2 - filt, 0 - apflg, 6 - aqflg
@//        - 11 - m01, 7 - n_tC0
@//         
@// Outputs - 1,8,7,11 - Output Pixels(P0a,P1a,Q0a,Q1a)
@//
@// Registers Corrupted - 0-3,5-12,14

 MASK_0      = 0x00000000   
 MASK_1      = 0x01010101
 MASK_2      = 0xff00ff00

 pAlphaArg   = 0x00
 pBetaArg    = 0x04
 pP3         = 0x08
 pQ3         = 0x0C
 pPix1       = 0x10
 pYstride    = 0x14
 pAlpha      = 0x18
 pBeta       = 0x1C
 pTc0        = 0x44

 .macro UpackToReg
@//----------------Pack p0-p3-----------------------
    LDR     mask, =MASK_2
        
    LDR     row0, [p_pix1], n_ystride          
    LDR     row1, [p_pix1], n_ystride          
    LDR     row2, [p_pix1]
    LDR     row3, [p_pix1, n_ystride]
    SUB     p_pix1, p_pix1, n_ystride, LSL #1
        
    @// row0 = [n_r0p0 n_r0p1 r0p2 r0p3]
    @// row1 = [n_r1p0 r1p1 r1p2 r1p3]
    @// row2 = [n_r2p0 n_r2p1 r2p2 r2p3]
    @// row3 = [r3p0 r3p1 r3p2 r3p3]

    AND     n_tpk0, mask, row0
    AND     n_tpk6, mask, row0, LSL#8
    UXTAB16 n_tpk0, n_tpk0, row1, ROR#8
    UXTAB16 n_tpk6, n_tpk6, row1
    AND     n_tpk2, mask, row2
    AND     n_tpk3, mask, row2, LSL#8
    UXTAB16 n_tpk2, n_tpk2, row3, ROR#8
    UXTAB16 n_tpk3, n_tpk3, row3

    @// n_tpk0 = [n_r0p0 n_r1p0 r0p2 r1p2]
    @// n_tpk6 = [n_r0p1 r1p1 r0p3 r1p3]
    @// n_tpk2 = [n_r2p0 r3p0 r2p2 r3p2]
    @// n_tpk3 = [n_r2p1 r3p1 r2p3 r3p3]

    PKHTB   p_0, n_tpk0, n_tpk2, ASR#16
    PKHTB   p_1, n_tpk6, n_tpk3, ASR#16
    PKHBT   p_2, n_tpk2, n_tpk0, LSL#16
    PKHBT   p_3, n_tpk3, n_tpk6, LSL#16
    
    STR     p_3,[sp, #pP3]

    @// p_0 = [n_r0p0 n_r1p0 n_r2p0 r3p0]
    @// p_1 = [n_r0p1 r1p1 n_r2p1 r3p1]
    @// p_2 = [r0p2 r1p2 n_r2p1 r3p2]
    @// p_3 = [r0p3 r1p3 r2p3 r3p3]

@//----------------Pack q0-q3-----------------------
	
	LDR   row4, [p_pix1, #4]!
    LDR   row5, [p_pix1, n_ystride]!
    LDR   row6, [p_pix1, n_ystride]!
    LDR   row7, [p_pix1, n_ystride]
	
    @// row4 = [r0q3 r0q2 r0q1 r0q0]
    @// row5 = [r1q3 r1q2 r1q1 r1q0]
    @// row6 = [r2q3 r2q2 r2q1 r2q0]
    @// row7 = [r3q3 r3q2 r3q1 r3q0]
    
    AND     n_tpk4, mask, row4
    AND     n_tpk5, mask, row4, LSL#8
    UXTAB16 n_tpk4, n_tpk4, row5, ROR#8
    UXTAB16 n_tpk5, n_tpk5, row5
    AND     n_tpk6, mask, row6
    AND     n_tpk7, mask, row6, LSL#8
    UXTAB16 n_tpk6, n_tpk6, row7, ROR#8
    UXTAB16 n_tpk7, n_tpk7, row7

    @// n_tpk4 = [r0q0 r1q0 r0q2 r1q2]
    @// n_tpk5 = [r0q1 r1q1 r0q3 r1q3]
    @// n_tpk6 = [r2q0 r3q0 r2q2 r3q2]
    @// n_tpk7 = [r2q1 r3q1 r2q3 r3q3]

    PKHTB   q_3, n_tpk4, n_tpk6, ASR#16
    PKHTB   q_2, n_tpk5, n_tpk7, ASR#16
    PKHBT   q_1, n_tpk6, n_tpk4, LSL#16
    PKHBT   q_0, n_tpk7, n_tpk5, LSL#16

    STR   q_3, [sp, #pQ3]

    @// q_0 = [r0q0 r1q0 r2q0 r3q0]
    @// q_1 = [r0q1 r1q1 r2q1 r3q1]
    @// q_2 = [r0q2 r1q2 r2q1 r3q2]
    @// q_3 = [r0q3 r1q3 r2q3 r3q3]	
 .endm
 
 .macro CheckApAndAqFlg
 @//--------------Filtering Decision -------------------
    LDR     m01, =MASK_1                @//  01010101 mask 
    MOV     r14, #MASK_0                @//  00000000 mask 
        	
     @// Check |p0-q0|<Alpha 
     USUB8   dp0q0, p_0, q_0 
     USUB8   a, q_0, p_0
     SEL     ap0q0, a, dp0q0
     USUB8   a, ap0q0, n_alpha
     SEL     filt, r14, m01
        
     @// Check |p1-p0|<Beta 
     USUB8   dp1p0, p_1, p_0
     USUB8   a, p_0, p_1
     SEL     a, a, dp1p0
     USUB8   a, a, n_beta
     SEL     filt, r14, filt

     @// Check |q1-q0|<Beta 
     USUB8   dq1q0, q_1, q_0
     USUB8   a, q_0, q_1
     SEL     a, a, dq1q0
     USUB8   a, a, n_beta
     SEL     filt, r14, filt

     @// Check ap<Beta 
     USUB8   dp2p0, p_2, p_0
     USUB8   a, p_0, p_2
     SEL     a, a, dp2p0
     USUB8   a, a, n_beta
     SEL     apflg, r14, filt            @// apflg = filt && (ap<n_beta)

     @// Check aq<Beta 
     USUB8   dq2q0, q_2, q_0
     USUB8   r14, q_0, q_2
     SEL     r14, r14, dq2q0
     USUB8   r14, r14, n_beta	
  
     MOV     r7,#0     
     SEL     n_t1, r7, filt            @// aqflg = filt && (aq<n_beta)    
 .endm

 .macro LoopFilterInter
@//
@// Inputs - 3,4,5,8,9,10 - Input Pixels (p0-p2,q0-q2)
@//        - 2 - filt, 0 - apflg,aqflg
@//        - 1 - ap0q0, 6 - n_alpha
@//        - 7 - m00, 11 - m01
@//         
@// Outputs - 6,7,1,9,0,2 - Output Pixels(n_P0b,n_P1b,n_P2b, n_Q0b,n_Q1b,n_Q2b)
@// 
@// Registers Corrupted - 0-3,5-12,14


        UHADD8  n_alpha, n_alpha, m00
        USUB8   n_t9, p_2, p_0    @//n_t9 = dp2p0
        UHADD8  n_alpha, n_alpha, m00
        ADD     n_alpha, n_alpha, m01, LSL #1        
        USUB8   ap0q0, ap0q0, n_alpha
        SEL     apqflg, m00, apqflg

        @// P0 = (p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4)>>3 
        @//    = ((p2-p0) + 2*(p1-p0) + (q1-q0) + 3*(q0-p0) + 8*p0 + 4)>>3
        @//    = p0 + (((p2-p0) + 2*(p1-p0) + (q1-q0) - 3*(p0-q0) + 4)>>3)

        @// P1 = (p2 + p1 + q0 + p0 + 2)>>2
        @//    = p0 + (((p2-p0) + (p1-p0) - (p0-q0) + 2)>>2)
        
        @// P2 = (2*p3 + 3*p2 + p1 + p0 + q0 + 4)>>3
        @//    = (2*(p3-p0) + 3*(p2-p0) + (p1-p0) - (p0-q0) + 8*p0 + 4)>>3
        @//    = p0 + (((p3-p0) + (p2-p0) + n_t2 + 2)>>2)

        @// Compute n_P0b
        USUB8   n_t2, p_0, q_0         
        SSUB8   n_t5, n_t9, n_t2           

        USUB8   n_t8, q_1, q_0         
        SHADD8  n_t8, n_t5, n_t8

        USUB8   n_t9, p_1, p_0         
        SADD8   n_t8, n_t8, n_t9
        SHSUB8  n_t8, n_t8, n_t2
        SHADD8  n_t5, n_t5, n_t9
        SHADD8  n_t8, n_t8, m01
        SHADD8  n_t9, n_t5, m01
        SADD8   n_P0b, p_0, n_t8         
        @// n_P0b ready
        
        @// Compute n_P1b
        LDR   p_3b, [sp, #pP3]
        SADD8   n_P1b, p_0, n_t9         
        @// n_P1b ready
        
        @// Compute n_P2b
        USUB8   n_t9, p_2, p_0         
        SADD8   n_t5, n_t5, n_t9
        UHSUB8  n_t9, p_3b, p_0        
        EOR     a, p_3b, p_0         
        AND     a, a, m01
        SHADD8  n_t5, n_t5, a
        UHADD8  a, p_0, q_1
        SADD8   n_t5, n_t5, m01
        SHADD8  n_t5, n_t5, n_t9
        MVN     n_t9, p_1
        SADD8   n_P2b, p_0, n_t5         
        @// n_P2b ready
        
        UHSUB8  a, a, n_t9
        ORR     n_t9, apqflg, m01
        USUB8   n_t9, apqflg, n_t9

        EOR     a, a, m01, LSL #7
        SEL     n_P0b, n_P0b, a
        SEL     n_P1b, n_P1b, p_1
        SEL     n_P2b, n_P2b, p_2

        USUB8   n_t4, filt, m01
        SEL     n_P0b, n_P0b, p_0

        
        @// Q0 = (q2 + 2*q1 + 2*q0 + 2*p0 + p1 + 4)>>3 
        @//    = ((q2-q0) + 2*(q1-q0) + (p1-p0) + 3*(p0-q0) + 8*q0 + 4)>>3
        @//    = q0 + (((q2-q0) + 2*(q1-q0) + (p1-p0) + 3*(p0-q0) + 4)>>3)

        @// Q1 = (q2 + q1 + p0 + q0 + 2)>>2
        @//    = q0 + (((q2-q0) + (q1-q0) + (p0-q0) + 2)>>2)

        @// Q2 = (2*q3 + 3*q2 + q1 + q0 + p0 + 4)>>3
        @//    = (2*(q3-q0) + 3*(q2-q0) + (q1-q0) + (p0-q0) + 8*q0 + 4)>>3
        @//    = q0 + (((q3-q0) + (q2-q0) + n_t2 + 2)>>2)


        @// Compute n_Q0b n_Q1b
        USUB8   n_t4, q_2, q_0           
        USUB8   a, p_0, q_0
        USUB8   n_t9, p_1, p_0
        SADD8   n_t0, n_t4, a
        SHADD8  n_t9, n_t0, n_t9
        UHADD8  n_t10, q_0, p_1
        SADD8   n_t9, n_t9, a
        USUB8   a, q_1, q_0
        SHADD8  n_t9, n_t9, a
        SHADD8  n_t0, n_t0, a
        SHADD8  n_t9, n_t9, m01
        SHADD8  a, n_t0, m01
        SADD8   n_t9, q_0, n_t9            
        @// n_Q0b ready - n_t9
        
        MOV     n_t4, #0
        UHADD8  apqflg, apqflg, n_t4
        
        SADD8   n_Q1b, q_0, a 
        @// n_Q1b ready
       
        USUB8   n_t4, apqflg, m01
        SEL     n_Q1b, n_Q1b, q_1
        MVN     n_t11, q_1
        UHSUB8  n_t10, n_t10, n_t11
        LDR   q_3b, [SP, #pQ3]
        EOR     n_t10, n_t10, m01, LSL #7
        SEL     n_t9, n_t9, n_t10            
        
        @// Compute n_Q2b
        USUB8   n_t4, q_2, q_0
        SADD8   n_t4, n_t0, n_t4
        EOR     n_t0, q_3b, q_0 
        AND     n_t0, n_t0, m01
        SHADD8  n_t4, n_t4, n_t0
        UHSUB8  n_t10, q_3b, q_0
        SADD8   n_t4, n_t4, m01
        SHADD8  n_t4, n_t4, n_t10

        USUB8   n_t10, filt, m01
        SEL     n_Q0b, n_t9, q_0

        SADD8   n_t4, q_0, n_t4            
        @// n_Q2b ready - n_t4

        USUB8   n_t10, apqflg, m01
        SEL     n_Q2b, n_t4, q_2      	
 
 .endm

 .macro StoreResult 
       @//---------Store result---------------
        LDR     maskb,=MASK_2
 
         @// n_P0b = [n_r0p0 n_r1p0 n_r2p0 r3p0]
        @// n_P1b = [n_r0p1 r1p1 n_r2p1 r3p1]
        @// n_P2b = [r0p2 r1p2 r2p2 r3p2]
        @// P3b = [r0p3 r1p3 r2p3 r3p3]

        LDR   P3b, [sp, #pP3]   
        STR   n_Q0b, [sp, #pP3]   

        @//------Pack p0-p3------
        AND     n_tpk0, maskb, n_P0b
        AND     n_tpk2, maskb, n_P0b, LSL#8
        UXTAB16 n_tpk0, n_tpk0, n_P1b, ROR#8
        UXTAB16 n_tpk2, n_tpk2, n_P1b

        AND     n_tpk3, maskb, n_P2b
        AND     n_tpk8, maskb, n_P2b, LSL#8
        UXTAB16 n_tpk3, n_tpk3, P3b, ROR#8
        UXTAB16 n_tpk8, n_tpk8, P3b

        @// n_tpk0 = [n_r0p0 n_r0p1 n_r2p0 n_r2p1]
        @// n_tpk2 = [n_r1p0 r1p1 r3p0 r3p1]
        @// n_tpk3 = [r0p2 r0p3 r2p2 r2p3]
        @// n_tpk8 = [r1p2 r1p3 r3p2 r3p3]

        MOV     p_2, n_Q1b
        LDRD    p_pix1, n_ystride, [sp, #pPix1] 

        PKHTB   row9, n_tpk0, n_tpk3, ASR#16
        PKHBT   row7, n_tpk3, n_tpk0, LSL#16
        PKHTB   row3, n_tpk2, n_tpk8, ASR#16
        PKHBT   row6, n_tpk8, n_tpk2, LSL#16

        @// row9 = [n_r0p0 n_r0p1 r0p2 r0p3]
        @// row3 = [n_r1p0 r1p1 r1p2 r1p3]
        @// row7 = [n_r2p0 n_r2p1 r2p2 r2p3]
        @// row6 = [r3p0 r3p1 r3p2 r3p3]

        STR     row9, [p_pix1], n_ystride
        STR     row7, [p_pix1, n_ystride]
        STR     row6, [p_pix1, n_ystride, LSL #1]
        STR     row3, [p_pix1], #4
        
        LDR   Q3b, [sp, #pQ3]

        @// n_Q0b = [r0q0 r1q0 r2q0 r3q0]
        @// n_Q1b = [r0q1 r1q1 r2q1 r3q1]
        @// n_Q2b = [r0q2 r1q2 r2q2 r3q2]
        @// Q3b = [r0q3 r1q3 r2q3 r3q3]

        @//------Pack q0-q3------
        AND     n_tpk0, maskb, p_2
        AND     n_tpk2, maskb, p_2, LSL#8
        UXTAB16 n_tpk0, n_tpk0, n_Q0b, ROR#8
        UXTAB16 n_tpk2, n_tpk2, n_Q0b

        AND     n_tpk3, maskb, Q3b
        AND     n_tpk8, maskb, Q3b, LSL#8
        UXTAB16 n_tpk3, n_tpk3, n_Q2b, ROR#8
        UXTAB16 n_tpk8, n_tpk8, n_Q2b

        @// n_tpk0 = [r0q1 r0q0 r2q1 r2q0]
        @// n_tpk2 = [r1q1 r1q0 r3q1 r3q0]
        @// n_tpk3 = [r0q3 r0q2 r2q3 r2q2]
        @// n_tpk8 = [r1q3 r1q2 r3q3 r3q2]

        PKHTB   row8, n_tpk3, n_tpk0, ASR#16
        PKHBT   row7, n_tpk0, n_tpk3, LSL#16
        PKHTB   row4, n_tpk8, n_tpk2, ASR#16
        PKHBT   row6, n_tpk2, n_tpk8, LSL#16

        @// row8 = [r0q0 r0q1 r0q2 r0q3]
        @// row4 = [r1q0 r1q1 r1q2 r1q3]
        @// row7 = [r2q0 r2q1 r2q2 r2q3]
        @// row6 = [r3q0 r3q1 r3q2 r3q3]

        STR     row4, [p_pix1]
        STR     row7, [p_pix1, n_ystride]
        STR     row6, [p_pix1, n_ystride, LSL #1]
        SUB     p_pix1, p_pix1, n_ystride
        STR     row8, [p_pix1]
 .endm
	
@extern void extern void DeblockIntraLumaH_ARMV6(VO_U8 *p_pix1, VO_S32 n_ystride, VO_S32 n_alpha, VO_S32 n_beta, VO_S8 *tc0 );
DeblockIntraLumaH_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    SUB     sp, sp, #0x10

@//--------------Filtering Decision -------------------
    LDR     m01, =MASK_1                @//  01010101 mask 
    MOV     m00, #MASK_0                @//  00000000 mask 

    MUL     r2, r2, m01
    MUL     r3, r3, m01

    STRD  r2, r3, [sp, #pAlphaArg]	
	
	
@	LDR   n_alpha,   [sp, #pTc0]
   LDRD  p_pix1, n_ystride,  [sp, #pPix1]
@	LDRSB  pskip, [n_alpha]
@	CMP   pskip, #0
	
@	BLT   NoFilter0
    LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
 
    SUB     p_pix1, p_pix1, #4
    STRD    p_pix1, n_ystride, [sp, #pPix1] 
   
    UpackToReg
    CheckApAndAqFlg
     
@     LDR     ptC0, [sp, #pTc0]
     CMP     filt, #0
     ORR     apqflg, apflg, n_t1, LSL #1
     BEQ     NoFilter0
     
 @    LDRB    n_t2, [ptC0]

     LoopFilterInter  	
     StoreResult     

NoFilter0:


@	LDR   n_alpha,   [sp, #pTc0]
	LDRD  p_pix1, n_ystride,  [sp, #pPix1]
@	LDRSB  pskip, [n_alpha, #1]
@	CMP   pskip, #0
	
@	BLT   NoFilter1
    LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
    
    ADD     p_pix1, p_pix1, n_ystride, LSL #2
    STRD    p_pix1, n_ystride, [sp, #pPix1] 
    UpackToReg
    CheckApAndAqFlg
     
@     LDR     ptC0, [sp, #pTc0]
     CMP     filt, #0
     ORR     apqflg, apflg, n_t1, LSL #1
     BEQ     NoFilter1
     
@     LDRB    n_t2, [ptC0, #1]

     LoopFilterInter   
     StoreResult  	

NoFilter1:
	   
@	LDR   n_alpha,   [sp, #pTc0]
	LDRD  p_pix1, n_ystride,  [sp, #pPix1]
@	LDRSB  pskip, [n_alpha,#2]
@	CMP   pskip, #0
	
@	BLT   NoFilter2
    LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
    
    ADD     p_pix1, p_pix1, n_ystride, LSL #2
    STRD    p_pix1, n_ystride, [sp, #pPix1]
    UpackToReg
    CheckApAndAqFlg
    
@     LDR     ptC0, [sp, #pTc0]
     CMP     filt, #0
     ORR     apqflg, apflg, n_t1, LSL #1
     BEQ     NoFilter2
     
@     LDRB    n_t2, [ptC0, #2]

     LoopFilterInter    
     StoreResult
   	
NoFilter2:

@	LDR   n_alpha,   [sp, #pTc0]
	LDRD  p_pix1, n_ystride,  [sp, #pPix1]
@	LDRSB  pskip, [n_alpha, #3]
@	CMP   pskip, #0
	
@	BLT   NoFilter3
    LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
    
    ADD     p_pix1, p_pix1, n_ystride, LSL #2
    STRD    p_pix1, n_ystride, [sp, #pPix1]
    UpackToReg
    CheckApAndAqFlg
@     LDR     ptC0, [sp, #pTc0] 
     CMP     filt, #0
     ORR     apqflg, apflg, n_t1, LSL #1
     BEQ     NoFilter3
     
@     LDRB    n_t2, [ptC0, #3]

     LoopFilterInter
     StoreResult             	

NoFilter3:	     	
	
	ADD     sp,sp,#0x20
    LDMFD   sp!, {r4-r11, pc}
     
	
	@.end

