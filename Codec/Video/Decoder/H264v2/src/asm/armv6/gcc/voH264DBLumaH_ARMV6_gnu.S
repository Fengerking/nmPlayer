@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************
 #include "../../../defineID.h"
    .section	  .text

	.global  DeblockLumaH_ARMV6
	
	.align 8	
	
@// Declare input registers

 p_pix1       .req r0
 n_StepArg    .req r1
 n_ystride    .req r1
 n_tC0Arg     .req r2
 n_alpha      .req r6

 n_beta       .req r7
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
 @   PKHBT   p_3, n_tpk3, n_tpk6, LSL#16
    
 @   STR     p_3,[sp, #pP3]

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

@    PKHTB   q_3, n_tpk4, n_tpk6, ASR#16
    PKHTB   q_2, n_tpk5, n_tpk7, ASR#16
    PKHBT   q_1, n_tpk6, n_tpk4, LSL#16
    PKHBT   q_0, n_tpk7, n_tpk5, LSL#16

@    STR   q_3, [sp, #pQ3]

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
  
     MOV     r14,#0     
     SEL     aqflg, r14, filt            @// aqflg = filt && (aq<n_beta)    
 .endm

 .macro LoopFilterInter
 @// Inputs - 3,4,5,8,9,10 - Input Pixels (p0-p2,q0-q2)
@//        - 2 - filt, 0 - apflg, 6 - aqflg
@//        - 11 - m01, 7 - n_tC0
@//         
@// Outputs - 1,8,7,11 - Output Pixels(P0a,P1a,Q0a,Q1a)
@//
@// Registers Corrupted - 0-3,5-12,14

	
    @// Since n_beta <= 18 and n_alpha <= 255 we know
    @// -254 <= p0-q0 <= 254
    @//  -17 <= q1-q0 <= 17
    @//  -17 <= p1-p0 <= 17

    @// delta = Clip3( -n_tC, n_tC, ((((q0-p0)<<2) + (p1-q1) + 4)>>3))
    @// 
    @//    Calculate A = (((q0-p0)<<2) + (p1-q1) + 4)>>3
    @//                = (4*q0 - 4*p0 + p1 - q1 + 4)>>3
    @//                = ((p1-p0) - (q1-q0) - 3*(p0-q0) + 4)>>3
        
    USUB8   n_t1, p_1, p_0
    MUL     n_tC0, n_t2, m01
        
    USUB8   n_t2, q_1, q_0
    SSUB8   n_t1, n_t1, n_t2

    USUB8   n_t2, p_0, q_0
    AND     n_t2, n_t2, m01
    SHSUB8  n_t1, n_t1, n_t2
    UHSUB8  n_t5, p_0, q_0
    SSUB8   n_t1, n_t1, n_t2
    SHSUB8  n_t1, n_t1, n_t5
    MOV     m00, #0
    SADD8   n_t1, n_t1, m01
    SHSUB8  n_t1, n_t1, n_t5
        
    @// n_tC = n_tC0
    @// if (ap < n_beta) n_tC++;
    @// if (aq < n_beta) n_tC++;
    USUB8   n_t5, filt, m01   
    SEL     n_tC0, n_tC0, m00
    UQADD8  n_tC, n_tC0, apflg
    SSUB8   n_t1, n_t1, m00
    UQADD8  n_tC, n_tC, aqflg

    @// Split into positive and negative part and clip 
    SEL     n_pos, n_t1, m00
    USUB8   voneg, n_pos, n_t1
    USUB8   n_t3, n_pos, n_tC
    SEL     n_pos, n_tC, n_pos
    USUB8   n_t3, voneg, n_tC
    SEL     voneg, n_tC, voneg
        
    @//Reload m01
    LDR     m01,=MASK_1

    UQADD8  P0a, p_0, n_pos
    UQSUB8  Q0a, q_0, n_pos
    UQSUB8  P0a, P0a, voneg
    UQADD8  Q0a, Q0a, voneg
        
    @// Choose to store the filtered
    @// value or the original pixel
    USUB8   n_t1, filt, m01    
    SEL     P0a, P0a, p_0
    SEL     Q0a, Q0a, q_0
    
    @// delta = (p2 + ((p0+q0+1)>>1) - (p1<<1))>>1;
    @// u1 = (p0 + q0 + 1)>>1
    @// u1 = ( (q_0 - p_0')>>1 ) ^ 0x80
    MVN     p_0, p_0
    UHSUB8  u1, q_0, p_0 
    UQADD8  max, p_1, n_tC0
    EOR     u1, u1, m01 ,LSL #7
    
    @// Calculate A = (p2+u1)>>1 
    @// Then delta = Clip3( -n_tC0, n_tC0, A - p1)

    @// Clip P1
    UHADD8  P1a, p_2, u1
    UQSUB8  min, p_1, n_tC0
    USUB8   n_t4, P1a, max
    SEL     P1a, max, P1a
    USUB8   n_t4, P1a, min
    SEL     P1a, P1a, min

    @// Clip Q1
    UHADD8  Q1a, q_2, u1
    UQADD8  max, q_1, n_tC0
    UQSUB8  min, q_1, n_tC0
    USUB8   n_t0, Q1a, max
    SEL     Q1a, max, Q1a
    USUB8   n_t0, Q1a, min
    SEL     Q1a, Q1a, min
        
     @// Choose to store the filtered
     @// value or the original pixel
    USUB8   n_t0, apflg, m01
    SEL     P1a, P1a, p_1
    USUB8   n_t0, aqflg, m01
    SEL     Q1a, Q1a, q_1       	
 
 .endm 
	
@extern void extern void DeblockLumaH_ARMV6(VO_U8 *p_pix1, VO_S32 n_ystride, VO_S32 n_alpha, VO_S32 n_beta, VO_S8 *tc0 );
DeblockLumaH_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    SUB     sp, sp, #0x10

@//--------------Filtering Decision -------------------
    LDR     m01, =MASK_1                @//  01010101 mask 
    MOV     m00, #MASK_0                @//  00000000 mask 

    MUL     r2, r2, m01
    MUL     r3, r3, m01

	STRD  r2, r3, [sp, #pAlphaArg]	
	
	
	LDR   n_alpha,   [sp, #pTc0]
	LDRD  p_pix1, n_ystride,  [sp, #pPix1]
	LDRSB  pskip, [n_alpha]
	CMP   pskip, #0
	
	BLT   NoFilter0
    LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
 
    SUB     p_pix1, p_pix1, #4
   
    UpackToReg
    CheckApAndAqFlg
     
     LDR     ptC0, [sp, #pTc0]
     CMP     filt, #0
     BEQ     NoFilter0
     
     LDRB    n_t2, [ptC0]

     LoopFilterInter  	
 
   @//---------Store result---------------

     LDR     maska,=MASK_2

     @// P1a = [n_r0p1 r1p1 n_r2p1 r3p1]
     @// P0a = [n_r0p0 n_r1p0 n_r2p0 r3p0]
     @// Q0a = [r0q0 r1q0 r2q0 r3q0]
     @// Q1a = [r0q1 r1q1 r2q1 r3q1]

     AND     n_tpk1, maska, P0a
     AND     n_tpk2, maska, P0a, LSL#8
     UXTAB16 n_tpk1, n_tpk1, P1a, ROR#8
     UXTAB16 n_tpk2, n_tpk2, P1a

     LDRD  p_pix1, n_ystride,  [sp, #pPix1]  
     SUB     p_pix1, p_pix1, #4 
      
     AND     n_tpk9, maska, Q1a
     AND     n_tpk3, maska, Q1a, LSL#8
     UXTAB16 n_tpk9, n_tpk9, Q0a, ROR#8
     UXTAB16 n_tpk3, n_tpk3, Q0a
     
     @// n_tpk1 = [n_r0p0 n_r0p1 n_r2p0 n_r2p1]
     @// n_tpk2 = [n_r1p0 r1p1 r3p0 r3p1]
     @// n_tpk9 = [r0q1 r0q0 r2q1 r2q0]
     @// n_tpk3 = [r1q1 r1q0 r3q1 r3q0]

     MOV     n_t4, n_tpk1, LSR #16
     MOV     n_t0, n_tpk9, LSR #16

     STRH    n_t4,[p_pix1, #2]!          @//Stores [n_r0p0 n_r0p1]
     STRH    n_t0,[p_pix1, #2]           @//Stores [r0q0 r0q1]

     MOV     n_t4, n_tpk2, LSR #16
     MOV     n_t0, n_tpk3, LSR #16

     STRH  n_t4,[p_pix1, n_ystride]!       @//Stores [n_r1p0 r1p1]
     STRH    n_t0,[p_pix1, #2]           @//Stores [r1q0 r1q1]
        
     STRH  n_tpk1,[p_pix1, n_ystride]!   @//Stores [n_r2p0 n_r2p1]
     STRH    n_tpk2,[p_pix1, n_ystride]    @//Stores [r3p0 r3p1]
     STRH    n_tpk9,[p_pix1, #2]!        @//Stores [r2q0 r2q1]
     STRH    n_tpk3,[p_pix1, n_ystride]    @//Stores [r3q0 r3q1]       

NoFilter0:


	LDR   n_alpha,   [sp, #pTc0]
	LDRD  p_pix1, n_ystride,  [sp, #pPix1]
	LDRSB  pskip, [n_alpha, #1]
	CMP   pskip, #0
	
	BLT   NoFilter1
    LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
    
    ADD     p_pix1, p_pix1, n_ystride, LSL #2
    SUB     p_pix1, p_pix1, #4
    UpackToReg
    CheckApAndAqFlg
     
     LDR     ptC0, [sp, #pTc0]
     CMP     filt, #0
     BEQ     NoFilter1
     
     LDRB    n_t2, [ptC0, #1]

     LoopFilterInter   
   @//---------Store result---------------

     LDR     maska,=MASK_2

     @// P1a = [n_r0p1 r1p1 n_r2p1 r3p1]
     @// P0a = [n_r0p0 n_r1p0 n_r2p0 r3p0]
     @// Q0a = [r0q0 r1q0 r2q0 r3q0]
     @// Q1a = [r0q1 r1q1 r2q1 r3q1]

     AND     n_tpk1, maska, P0a
     AND     n_tpk2, maska, P0a, LSL#8
     UXTAB16 n_tpk1, n_tpk1, P1a, ROR#8
     UXTAB16 n_tpk2, n_tpk2, P1a

     LDRD  p_pix1, n_ystride,  [sp, #pPix1]  
     ADD     p_pix1, p_pix1, n_ystride, LSL #2
     SUB     p_pix1, p_pix1, #4 
      
     AND     n_tpk9, maska, Q1a
     AND     n_tpk3, maska, Q1a, LSL#8
     UXTAB16 n_tpk9, n_tpk9, Q0a, ROR#8
     UXTAB16 n_tpk3, n_tpk3, Q0a
     
     @// n_tpk1 = [n_r0p0 n_r0p1 n_r2p0 n_r2p1]
     @// n_tpk2 = [n_r1p0 r1p1 r3p0 r3p1]
     @// n_tpk9 = [r0q1 r0q0 r2q1 r2q0]
     @// n_tpk3 = [r1q1 r1q0 r3q1 r3q0]

     MOV     n_t4, n_tpk1, LSR #16
     MOV     n_t0, n_tpk9, LSR #16

     STRH    n_t4,[p_pix1, #2]!          @//Stores [n_r0p0 n_r0p1]
     STRH    n_t0,[p_pix1, #2]           @//Stores [r0q0 r0q1]

     MOV     n_t4, n_tpk2, LSR #16
     MOV     n_t0, n_tpk3, LSR #16

     STRH  n_t4,[p_pix1, n_ystride]!       @//Stores [n_r1p0 r1p1]
     STRH    n_t0,[p_pix1, #2]           @//Stores [r1q0 r1q1]
        
     STRH  n_tpk1,[p_pix1, n_ystride]!   @//Stores [n_r2p0 n_r2p1]
     STRH    n_tpk2,[p_pix1, n_ystride]    @//Stores [r3p0 r3p1]
     STRH    n_tpk9,[p_pix1, #2]!        @//Stores [r2q0 r2q1]
     STRH    n_tpk3,[p_pix1, n_ystride]    @//Stores [r3q0 r3q1]        	

NoFilter1:
	   
	LDR   n_alpha,   [sp, #pTc0]
	LDRD  p_pix1, n_ystride,  [sp, #pPix1]
	LDRSB  pskip, [n_alpha,#2]
	CMP   pskip, #0
	
	BLT   NoFilter2
    LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
    
    ADD     p_pix1, p_pix1, n_ystride, LSL #3
    SUB     p_pix1, p_pix1, #4
    UpackToReg
    CheckApAndAqFlg
    
     LDR     ptC0, [sp, #pTc0]
     CMP     filt, #0
     BEQ     NoFilter2
     
     LDRB    n_t2, [ptC0, #2]

     LoopFilterInter    
 
    @//---------Store result---------------

     LDR     maska,=MASK_2

     @// P1a = [n_r0p1 r1p1 n_r2p1 r3p1]
     @// P0a = [n_r0p0 n_r1p0 n_r2p0 r3p0]
     @// Q0a = [r0q0 r1q0 r2q0 r3q0]
     @// Q1a = [r0q1 r1q1 r2q1 r3q1]

     AND     n_tpk1, maska, P0a
     AND     n_tpk2, maska, P0a, LSL#8
     UXTAB16 n_tpk1, n_tpk1, P1a, ROR#8
     UXTAB16 n_tpk2, n_tpk2, P1a

     LDRD  p_pix1, n_ystride,  [sp, #pPix1]  
     ADD     p_pix1, p_pix1, n_ystride, LSL #3
     SUB     p_pix1, p_pix1, #4 
      
     AND     n_tpk9, maska, Q1a
     AND     n_tpk3, maska, Q1a, LSL#8
     UXTAB16 n_tpk9, n_tpk9, Q0a, ROR#8
     UXTAB16 n_tpk3, n_tpk3, Q0a
     
     @// n_tpk1 = [n_r0p0 n_r0p1 n_r2p0 n_r2p1]
     @// n_tpk2 = [n_r1p0 r1p1 r3p0 r3p1]
     @// n_tpk9 = [r0q1 r0q0 r2q1 r2q0]
     @// n_tpk3 = [r1q1 r1q0 r3q1 r3q0]

     MOV     n_t4, n_tpk1, LSR #16
     MOV     n_t0, n_tpk9, LSR #16

     STRH    n_t4,[p_pix1, #2]!          @//Stores [n_r0p0 n_r0p1]
     STRH    n_t0,[p_pix1, #2]           @//Stores [r0q0 r0q1]

     MOV     n_t4, n_tpk2, LSR #16
     MOV     n_t0, n_tpk3, LSR #16

     STRH  n_t4,[p_pix1, n_ystride]!       @//Stores [n_r1p0 r1p1]
     STRH    n_t0,[p_pix1, #2]           @//Stores [r1q0 r1q1]
        
     STRH  n_tpk1,[p_pix1, n_ystride]!   @//Stores [n_r2p0 n_r2p1]
     STRH    n_tpk2,[p_pix1, n_ystride]    @//Stores [r3p0 r3p1]
     STRH    n_tpk9,[p_pix1, #2]!        @//Stores [r2q0 r2q1]
     STRH    n_tpk3,[p_pix1, n_ystride]    @//Stores [r3q0 r3q1]         	

NoFilter2:

	LDR   n_alpha,   [sp, #pTc0]
	LDRD  p_pix1, n_ystride,  [sp, #pPix1]
	LDRSB  pskip, [n_alpha, #3]
	CMP   pskip, #0
	
	BLT   NoFilter3
    LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
    
    ADD     p_pix1, p_pix1, n_ystride, LSL #2
    ADD     p_pix1, p_pix1, n_ystride, LSL #3
    SUB     p_pix1, p_pix1, #4
    UpackToReg
    CheckApAndAqFlg
     LDR     ptC0, [sp, #pTc0]
     CMP     filt, #0
     BEQ     NoFilter3
     
     LDRB    n_t2, [ptC0, #3]

     LoopFilterInter
 
    @//---------Store result---------------

     LDR     maska,=MASK_2

     @// P1a = [n_r0p1 r1p1 n_r2p1 r3p1]
     @// P0a = [n_r0p0 n_r1p0 n_r2p0 r3p0]
     @// Q0a = [r0q0 r1q0 r2q0 r3q0]
     @// Q1a = [r0q1 r1q1 r2q1 r3q1]

     AND     n_tpk1, maska, P0a
     AND     n_tpk2, maska, P0a, LSL#8
     UXTAB16 n_tpk1, n_tpk1, P1a, ROR#8
     UXTAB16 n_tpk2, n_tpk2, P1a

     LDRD  p_pix1, n_ystride,  [sp, #pPix1]  
     ADD     p_pix1, p_pix1, n_ystride, LSL #2
     ADD     p_pix1, p_pix1, n_ystride, LSL #3
     SUB     p_pix1, p_pix1, #4 
      
     AND     n_tpk9, maska, Q1a
     AND     n_tpk3, maska, Q1a, LSL#8
     UXTAB16 n_tpk9, n_tpk9, Q0a, ROR#8
     UXTAB16 n_tpk3, n_tpk3, Q0a
     
     @// n_tpk1 = [n_r0p0 n_r0p1 n_r2p0 n_r2p1]
     @// n_tpk2 = [n_r1p0 r1p1 r3p0 r3p1]
     @// n_tpk9 = [r0q1 r0q0 r2q1 r2q0]
     @// n_tpk3 = [r1q1 r1q0 r3q1 r3q0]

     MOV     n_t4, n_tpk1, LSR #16
     MOV     n_t0, n_tpk9, LSR #16

     STRH    n_t4,[p_pix1, #2]!          @//Stores [n_r0p0 n_r0p1]
     STRH    n_t0,[p_pix1, #2]           @//Stores [r0q0 r0q1]

     MOV     n_t4, n_tpk2, LSR #16
     MOV     n_t0, n_tpk3, LSR #16

     STRH  n_t4,[p_pix1, n_ystride]!       @//Stores [n_r1p0 r1p1]
     STRH    n_t0,[p_pix1, #2]           @//Stores [r1q0 r1q1]
        
     STRH  n_tpk1,[p_pix1, n_ystride]!   @//Stores [n_r2p0 n_r2p1]
     STRH    n_tpk2,[p_pix1, n_ystride]    @//Stores [r3p0 r3p1]
     STRH    n_tpk9,[p_pix1, #2]!        @//Stores [r2q0 r2q1]
     STRH    n_tpk3,[p_pix1, n_ystride]    @//Stores [r3q0 r3q1]        
          	

NoFilter3:	     	
	
	ADD     sp,sp,#0x20
    LDMFD   sp!, {r4-r11, pc}
     
	
	@.end

