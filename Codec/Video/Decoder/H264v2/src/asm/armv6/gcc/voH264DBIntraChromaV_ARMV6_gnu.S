@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************
 #include "../../../defineID.h"
    .section	  .text

	.global  DeblockIntraChromaV_ARMV6
	
	.align 8	
	
@// Declare input registers

 p_pix1       .req r0
 n_StepArg    .req r1
 n_ystride    .req r1
 n_tC0Arg     .req r2
 n_alpha      .req r6

 n_beta       .req r7
 n_bS         .req r14
 n_tC          .req r12
 n_tC0         .req r5
 n_tC1         .req r12
 n_pos         .req r5
 voneg         .req r9
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
        
 m00     .req r14
 m01     .req r11

 apflg   .req r0 
 aqflg   .req r6

@n_tC      RN 12


@// Output
            
 voP_0         .req r6
 voQ_0         .req r7


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

@// Filtering

 dp0q0       .req r12
 dp1p0       .req r12
 dq1q0       .req r12
 dp2p0       .req r12
 dq2q0       .req r12
 pskip       .req r12

@//Declarations for bSLT4 kernel

 n_tC          .req r12
 n_tC0         .req r5
 n_tC1         .req r12
 n_pos         .req r5
 voneg         .req r9

@//Declarations for bSGE4 kernel


@// Miscellanous
 XY          .req r8

 a           .req r10
 n_t1          .req r10
 n_t2          .req r12
 n_t3          .req r14
 n_t4          .req r6
 n_t5          .req r5

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
@    LDR   p_3, [p_pix1], n_ystride
@    LDR   p_2, [p_pix1], n_ystride
@    STR   p_3, [sp, #pP3]
    LDR   p_1, [p_pix1], n_ystride

    LDR   p_0, [p_pix1], n_ystride
    LDR   q_0, [p_pix1], n_ystride
    LDR   q_1, [p_pix1], n_ystride
@    LDR   q_2, [p_pix1], n_ystride
@    LDR   q_3, [p_pix1], n_ystride
@    STR   q_3, [sp, #pQ3]
 .endm
 
 .macro CheckApAndAqFlg
 @//--------------Filtering Decision -------------------
    LDR     m01, =MASK_1                @//  01010101 mask 
    MOV     r14, #MASK_0                @//  00000000 mask 
    
     MOV     filt, m01
 @    TST     pskip, #0xff00
 @    MOVEQ   filt, filt, LSR #16
 @    TST     pskip, #0xff
 @    MOVEQ   filt, filt, LSL #16
         	
     @// Check |p0-q0|<Alpha 
     USUB8   dp0q0, p_0, q_0 
     USUB8   a, q_0, p_0
     SEL     ap0q0, a, dp0q0
     USUB8   a, ap0q0, n_alpha
     SEL     filt, r14, filt
        
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
@     USUB8   dp2p0, p_2, p_0
@     USUB8   a, p_0, p_2
@     SEL     a, a, dp2p0
@     USUB8   a, a, n_beta
@     SEL     apflg, r14, filt            ;// apflg = filt && (ap<n_beta)

     @// Check aq<Beta 
 @    USUB8   dq2q0, q_2, q_0
 @    USUB8   r14, q_0, q_2
 @    SEL     r14, r14, dq2q0
 @    USUB8   r14, r14, n_beta	
  
 @    MOV     r14,#0     
 @    SEL     aqflg, r14, filt            ;// aqflg = filt && (aq<n_beta)    
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
        
 @   USUB8   n_t1, p_1, p_0
 @   MUL     n_tC0, n_t2, m01
        
 @   USUB8   n_t2, q_1, q_0
 @   SSUB8   n_t1, n_t1, n_t2

 @   USUB8   n_t2, p_0, q_0
 @   AND     n_t2, n_t2, m01
 @   SHSUB8  n_t1, n_t1, n_t2
 @   UHSUB8  n_t5, p_0, q_0
 @   SSUB8   n_t1, n_t1, n_t2
 @   SHSUB8  n_t1, n_t1, n_t5
 @   MOV     m00, #0
 @   SADD8   n_t1, n_t1, m01
 @   SHSUB8  n_t1, n_t1, n_t5
     USUB8   n_t1, p_1, p_0
     USUB8   n_t2, q_1, q_0
  @   BEQ     NoFilterFilt0
        
     LDRB    n_tC0, [ptC0]
     SSUB8   n_t1, n_t1, n_t2
     LDRB    n_tC1, [ptC0, #1]
   @  STR   pThresholds, ppThresholds
     UHSUB8  n_t4, p_0, q_0
     ORR     n_tC, n_tC0, n_tC1, LSL #16
     USUB8   n_t5, p_0, q_0
     AND     n_t5, n_t5, m01
     SHSUB8  n_t1, n_t1, n_t5
     ORR     n_tC, n_tC, LSL #8        
     SSUB8   n_t1, n_t1, n_t5
     SHSUB8  n_t1, n_t1, n_t4
 @    UQADD8  n_tC, n_tC, m01
     SADD8   n_t1, n_t1, m01
     USUB8   n_t5, filt, m01   
     SHSUB8  n_t1, n_t1, n_t4
     SEL     n_tC, n_tC, m00
        
    @// n_tC = n_tC0
    @// if (ap < n_beta) n_tC++;
    @// if (aq < n_beta) n_tC++;
@    USUB8   n_t5, filt, m01   
@    SEL     n_tC0, n_tC0, m00
@    UQADD8  n_tC, n_tC0, apflg
@    SSUB8   n_t1, n_t1, m00
@    UQADD8  n_tC, n_tC, aqflg

      @// Split into positive and negative part and clip 

     SSUB8   n_t1, n_t1, m00
     SEL     n_pos, n_t1, m00
     USUB8   voneg, n_pos, n_t1
     USUB8   n_t3, n_pos, n_tC
     SEL     n_pos, n_tC, n_pos
     USUB8   n_t3, voneg, n_tC
     SEL     voneg, n_tC, voneg
     UQADD8  voP_0, p_0, n_pos
     UQSUB8  voQ_0, q_0, n_pos
     UQSUB8  voP_0, voP_0, voneg
     UQADD8  voQ_0, voQ_0, voneg
        
  @   SUB     pQ0, pQ0, srcdstStep, LSL #1

     @// Choose to store the filtered
     @// value or the original pixel
     USUB8   n_t1, filt, m01    
     SEL     voP_0, voP_0, p_0
     SEL     voQ_0, voQ_0, q_0     	
 
 .endm 
 
 
  .macro LoopFilterInter1
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
        
 @   USUB8   n_t1, p_1, p_0
 @   MUL     n_tC0, n_t2, m01
        
 @   USUB8   n_t2, q_1, q_0
 @   SSUB8   n_t1, n_t1, n_t2

 @   USUB8   n_t2, p_0, q_0
 @   AND     n_t2, n_t2, m01
 @   SHSUB8  n_t1, n_t1, n_t2
 @   UHSUB8  n_t5, p_0, q_0
 @   SSUB8   n_t1, n_t1, n_t2
 @   SHSUB8  n_t1, n_t1, n_t5
 @   MOV     m00, #0
 @   SADD8   n_t1, n_t1, m01
 @   SHSUB8  n_t1, n_t1, n_t5
     USUB8   n_t1, p_1, p_0
     USUB8   n_t2, q_1, q_0
  @   BEQ     NoFilterFilt0
        
     LDRB    n_tC0, [ptC0, #2]
     SSUB8   n_t1, n_t1, n_t2
     LDRB    n_tC1, [ptC0, #3]
   @  STR   pThresholds, ppThresholds
     UHSUB8  n_t4, p_0, q_0
     ORR     n_tC, n_tC0, n_tC1, LSL #16
     USUB8   n_t5, p_0, q_0
     AND     n_t5, n_t5, m01
     SHSUB8  n_t1, n_t1, n_t5
     ORR     n_tC, n_tC, LSL #8        
     SSUB8   n_t1, n_t1, n_t5
     SHSUB8  n_t1, n_t1, n_t4
@     UQADD8  n_tC, n_tC, m01
     SADD8   n_t1, n_t1, m01
     USUB8   n_t5, filt, m01   
     SHSUB8  n_t1, n_t1, n_t4
     SEL     n_tC, n_tC, m00
        
    @// n_tC = n_tC0
    @// if (ap < n_beta) n_tC++;
    @// if (aq < n_beta) n_tC++;
@    USUB8   n_t5, filt, m01   
@    SEL     n_tC0, n_tC0, m00
@    UQADD8  n_tC, n_tC0, apflg
@    SSUB8   n_t1, n_t1, m00
@    UQADD8  n_tC, n_tC, aqflg

      @// Split into positive and negative part and clip 

     SSUB8   n_t1, n_t1, m00
     SEL     n_pos, n_t1, m00
     USUB8   voneg, n_pos, n_t1
     USUB8   n_t3, n_pos, n_tC
     SEL     n_pos, n_tC, n_pos
     USUB8   n_t3, voneg, n_tC
     SEL     voneg, n_tC, voneg
     UQADD8  voP_0, p_0, n_pos
     UQSUB8  voQ_0, q_0, n_pos
     UQSUB8  voP_0, voP_0, voneg
     UQADD8  voQ_0, voQ_0, voneg
        
  @   SUB     pQ0, pQ0, srcdstStep, LSL #1

     @// Choose to store the filtered
     @// value or the original pixel
     USUB8   n_t1, filt, m01    
     SEL     voP_0, voP_0, p_0
     SEL     voQ_0, voQ_0, q_0     	
 
 .endm 
	
	
@extern void extern void DeblockIntraChromaV_ARMV6(VO_U8 *p_pix1, VO_S32 n_ystride, VO_S32 n_alpha, VO_S32 n_beta, VO_S8 *tc0 );
DeblockIntraChromaV_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    SUB     sp, sp, #0x10

@//--------------Filtering Decision -------------------
    LDR     m01, =MASK_1                @//  01010101 mask 
    MOV     m00, #MASK_0                @//  00000000 mask 

    MUL     r2, r2, m01
    MUL     r3, r3, m01

    STRD  r2, r3, [sp, #pAlphaArg]	
	
	
 @   LDR   n_alpha,   [sp, #pTc0]
    LDRD  p_pix1, n_ystride,  [sp, #pPix1]
@    LDRH  pskip, [n_alpha]
@    CMP   pskip, #0
    SUB     p_pix1, p_pix1, n_ystride, LSL #1
    STRD   p_pix1, n_ystride,  [sp, #pPix1]
    	
@	BEQ   NoFilter0
    LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
 
    
   
    UpackToReg
    CheckApAndAqFlg
     
@     LDR     ptC0, [sp, #pTc0]
     CMP     filt, #0
     BEQ     NoFilter0
     
 @   LDRB    n_tC0, [ptC0], #1
 @   LDRB    n_tC1, [ptC0], #1

 @   LoopFilterInter  	
     UHADD8  n_t1, p_0, q_1
     MVN     n_t2, p_1
     UHSUB8  n_t1, n_t1, n_t2
     USUB8   n_t2, filt, m01
     EOR     n_t1, n_t1, m01, LSL #7
     
     UHADD8  n_t2, q_0, p_1
     MVN     n_t3, q_1
     UHSUB8  n_t2, n_t2, n_t3
     
     SEL     voP_0, n_t1, p_0
     EOR     n_t2, n_t2, m01, LSL #7
     SEL     voQ_0, n_t2, q_0
  
   @//---------Store result---------------

 @    MOV     p_0, P0a
     LDRD  p_pix1, n_ystride,  [sp, #pPix1]  
@     SUB   p_pix1, p_pix1, n_ystride, LSL #2 

@     STR   P1a, [p_pix1, n_ystride, LSL #1]!
     STR   voP_0, [p_pix1, n_ystride]!
     STR   voQ_0, [p_pix1, n_ystride]!
@     STR   Q1a, [p_pix1, n_ystride]   

NoFilter0:


@	LDR   n_alpha,   [sp, #pTc0]
	LDRD  p_pix1, n_ystride,  [sp, #pPix1]
@	LDRH  pskip, [n_alpha, #2]
@	CMP   pskip, #0
    ADD     p_pix1, p_pix1, #4
    STRD   p_pix1, n_ystride,  [sp, #pPix1]	
@	BEQ   NoFilter1
    LDRD   n_alpha, n_beta, [sp, #pAlphaArg]
    
 
    
    UpackToReg
    CheckApAndAqFlg
     
 @    LDR     ptC0, [sp, #pTc0]
     CMP     filt, #0
     BEQ     NoFilter1
     
 @    LDRB    n_tC0, [ptC0, #2]
 @    LDRB    n_tC1, [ptC0, #3]
     
 @    LoopFilterInter1   
     
     UHADD8  n_t1, p_0, q_1
     MVN     n_t2, p_1
     UHSUB8  n_t1, n_t1, n_t2
     USUB8   n_t2, filt, m01
     EOR     n_t1, n_t1, m01, LSL #7
     
     UHADD8  n_t2, q_0, p_1
     MVN     n_t3, q_1
     UHSUB8  n_t2, n_t2, n_t3
     
     SEL     voP_0, n_t1, p_0
     EOR     n_t2, n_t2, m01, LSL #7
     SEL     voQ_0, n_t2, q_0
     
   @//---------Store result---------------
 @    MOV     p_0, P0a
     LDRD  p_pix1, n_ystride,  [sp, #pPix1] 
@     ADD     p_pix1, p_pix1, n_ystride, LSL #1

@     STR   P1a, [p_pix1, n_ystride, LSL #1]!
     STR   voP_0, [p_pix1, n_ystride]!
     STR   voQ_0, [p_pix1, n_ystride]!
 @    STR   Q1a, [p_pix1, n_ystride]     	

NoFilter1:
	   
	ADD     sp,sp,#0x20
    LDMFD   sp!, {r4-r11, pc}
     
	
	@.end

