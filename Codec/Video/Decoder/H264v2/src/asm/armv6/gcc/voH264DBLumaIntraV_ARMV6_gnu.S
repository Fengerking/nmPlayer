@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************
 #include "../../../defineID.h"
    .section	  .text

	.global  DeblockIntraLumaV_ARMV6
	
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
    LDR   p_3, [p_pix1], n_ystride
    LDR   p_2, [p_pix1], n_ystride
    STR   p_3, [sp, #pP3]
    LDR   p_1, [p_pix1], n_ystride

    LDR   p_0, [p_pix1], n_ystride
    LDR   q_0, [p_pix1], n_ystride
    LDR   q_1, [p_pix1], n_ystride
    LDR   q_2, [p_pix1], n_ystride
    LDR   q_3, [p_pix1], n_ystride
    STR   q_3, [sp, #pQ3]
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
	
@extern void extern void DeblockIntraLumaV_ARMV6(VO_U8 *p_pix1, VO_S32 n_ystride, VO_S32 n_alpha, VO_S32 n_beta);
DeblockIntraLumaV_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    SUB     sp, sp, #0x10

@//--------------Filtering Decision -------------------
    LDR     m01, =MASK_1                @//  01010101 mask 
    MOV     m00, #MASK_0                @//  00000000 mask 

    MUL     r2, r2, m01
    MUL     r3, r3, m01

    STRD  r2, r3, [sp, #pAlphaArg]	
	
	
@    LDR   n_alpha,   [sp, #pTc0]
    LDRD  p_pix1, n_ystride,  [sp, #pPix1]
@    LDRSB  pskip, [n_alpha]
@    CMP   pskip, #0
    SUB     p_pix1, p_pix1, n_ystride, LSL #2
    STRD   p_pix1, n_ystride,  [sp, #pPix1]
    	
@	BLT   NoFilter0
    LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
 
    
   
    UpackToReg
    CheckApAndAqFlg
     
@     LDR     ptC0, [sp, #pTc0]
     CMP     filt, #0
     ORR     apqflg, apflg, n_t1, LSL #1
     BEQ     NoFilter0
     
@     LDRB    n_t2, [ptC0]

     LoopFilterInter  	
 
   @//---------Store result---------------
   @  MOV     p_0, P0a
     MOV     p_2, n_Q1b
     MOV     p_1, n_P2b
   
     LDRD  p_pix1, n_ystride,  [sp, #pPix1] 
@     ADD     p_pix1, p_pix1, n_ystride, LSL #1

     STR   p_1, [p_pix1, n_ystride]!
     STR   n_P1b, [p_pix1, n_ystride]!
     STR   n_P0b, [p_pix1, n_ystride]!
     STR   n_Q0b, [p_pix1, n_ystride]!
     STR   p_2, [p_pix1, n_ystride]
     STR   n_Q2b, [p_pix1, n_ystride, LSL #1]

NoFilter0:


@	LDR   n_alpha,   [sp, #pTc0]
	LDRD  p_pix1, n_ystride,  [sp, #pPix1]
@	LDRSB  pskip, [n_alpha, #1]
@	CMP   pskip, #0
    ADD     p_pix1, p_pix1, #4
    STRD   p_pix1, n_ystride,  [sp, #pPix1]	
@	BLT   NoFilter1
    LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
    
 
    
    UpackToReg
    CheckApAndAqFlg
     
@     LDR     ptC0, [sp, #pTc0]
     CMP     filt, #0
     ORR     apqflg, apflg, n_t1, LSL #1
     BEQ     NoFilter1
     
@     LDRB    n_t2, [ptC0, #1]

     LoopFilterInter   
   @//---------Store result---------------
   @  MOV     p_0, P0a
     MOV     p_2, n_Q1b
     MOV     p_1, n_P2b
   
     LDRD  p_pix1, n_ystride,  [sp, #pPix1] 
@     ADD     p_pix1, p_pix1, n_ystride, LSL #1

     STR   p_1, [p_pix1, n_ystride]!
     STR   n_P1b, [p_pix1, n_ystride]!
     STR   n_P0b, [p_pix1, n_ystride]!
     STR   n_Q0b, [p_pix1, n_ystride]!
     STR   p_2, [p_pix1, n_ystride]
     STR   n_Q2b, [p_pix1, n_ystride, LSL #1]

NoFilter1:
	   
@	LDR   n_alpha,   [sp, #pTc0]
	LDRD  p_pix1, n_ystride,  [sp, #pPix1]
@	LDRSB  pskip, [n_alpha,#2]
@	CMP   pskip, #0
	ADD     p_pix1, p_pix1, #4
    STRD   p_pix1, n_ystride,  [sp, #pPix1]
@	BLT   NoFilter2
    LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
    
   

    UpackToReg
    CheckApAndAqFlg
    
@     LDR     ptC0, [sp, #pTc0]
     CMP     filt, #0
     ORR     apqflg, apflg, n_t1, LSL #1
     BEQ     NoFilter2
     
@     LDRB    n_t2, [ptC0, #2]

     LoopFilterInter    
 
   @//---------Store result---------------
   @  MOV     p_0, P0a
     MOV     p_2, n_Q1b
     MOV     p_1, n_P2b
   
     LDRD  p_pix1, n_ystride,  [sp, #pPix1] 
@     ADD     p_pix1, p_pix1, n_ystride, LSL #1

     STR   p_1, [p_pix1, n_ystride]!
     STR   n_P1b, [p_pix1, n_ystride]!
     STR   n_P0b, [p_pix1, n_ystride]!
     STR   n_Q0b, [p_pix1, n_ystride]!
     STR   p_2, [p_pix1, n_ystride]
     STR   n_Q2b, [p_pix1, n_ystride, LSL #1]    	

NoFilter2:

@	LDR   n_alpha,   [sp, #pTc0]
	LDRD  p_pix1, n_ystride,  [sp, #pPix1]
@	LDRSB  pskip, [n_alpha, #3]
@	CMP   pskip, #0
	ADD     p_pix1, p_pix1, #4
        STRD   p_pix1, n_ystride,  [sp, #pPix1]
@	BLT   NoFilter3
        LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
    
   

    UpackToReg
    CheckApAndAqFlg
 @    LDR     ptC0, [sp, #pTc0]
     CMP     filt, #0
     ORR     apqflg, apflg, n_t1, LSL #1
     BEQ     NoFilter3
     
 @    LDRB    n_t2, [ptC0, #3]

     LoopFilterInter
 
   @//---------Store result---------------
   @  MOV     p_0, P0a
     MOV     p_2, n_Q1b
     MOV     p_1, n_P2b
   
     LDRD  p_pix1, n_ystride,  [sp, #pPix1] 
@     ADD     p_pix1, p_pix1, n_ystride, LSL #1

     STR   p_1, [p_pix1, n_ystride]!
     STR   n_P1b, [p_pix1, n_ystride]!
     STR   n_P0b, [p_pix1, n_ystride]!
     STR   n_Q0b, [p_pix1, n_ystride]!
     STR   p_2, [p_pix1, n_ystride]
     STR   n_Q2b, [p_pix1, n_ystride, LSL #1]
     
NoFilter3:	     	
	
	ADD     sp,sp,#0x20
    LDMFD   sp!, {r4-r11, pc}
     
	
	@.end

