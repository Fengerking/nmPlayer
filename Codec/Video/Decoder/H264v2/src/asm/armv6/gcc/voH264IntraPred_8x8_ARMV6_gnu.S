@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************  
 #include "../../../defineID.h"
	.section	  .text
 
	.global PredIntraLuma8x8DC_ARMV6
	.global PredIntraLuma8x8V_ARMV6
	.global PredIntraLuma8x8H_ARMV6
	.global Predict8x8VerLeft_ARMV6
	.global Predict8x8DownLeft_ARMV6
	.global Predict8x8HorDown_ARMV6
	.global Predict8x8VerRight_ARMV6
	.global Predict8x8DownRight_ARMV6
	.global Predict8x8HorUp_ARMV6
	.global Predict8x8DC_ARMV6
	.global Predict8x8DCLeft_ARMV6
	.global Predict8x8DCTop_ARMV6
	.global Predict8x8DC128_ARMV6

@//--------------------------------------------
@// Constants 
@//--------------------------------------------  
 BLK_SIZE        = 0x10
 MUL_CONST0      = 0x01010101
 MUL_CONST1      = 0x00060004
 MUL_CONST2      = 0x00070005
 MUL_CONST3      = 0x00030001
 MASK_CONST      = 0x00FF00FF

@//--------------------------------------------
@// Scratch variable
@//--------------------------------------------
 y               .req r12   
@pc              RN 15   

 return          .req r0    
 innerCount      .req r0    
 outerCount      .req r1    
 pSrcLeft2       .req r1    
 pDst2           .req r2    
 sum             .req r6    
 pTable          .req r9    
 n_tmp1           .req r10   
 n_tmp2           .req r12   
 cMul1           .req r11   
 cMul2           .req r12   
 n_cnt           .req r12   
 dstStepx2       .req r11   
 leftStepx2      .req r14   
 r0x01010101     .req r10   
 r0x00FF00FF     .req r11

 tVal0           .req r0    
 n_tVal1           .req r1    
 n_tVal2           .req r2    
 n_tVal3           .req r3    
 n_tVal4           .req r4    
 n_tVal5           .req r5    
 n_tVal6           .req r6    
 n_tVal7           .req r7    
 n_tVal8           .req r8    
 n_tVal9           .req r9    
 n_tVal10          .req r10   
 n_tVal11          .req r11   
 n_tVal12          .req r12   
 n_tVal14          .req r14   

@b               RN 12   
 c               .req r14   

 n_p2p0            .req r0    
 n_p3p1            .req r1    
 n_p6p4            .req r2    
 n_p7p5            .req r4    
 n_p10p8           .req r6    
 n_p11p9           .req r7    
 n_p14p12          .req r8    
 n_p15p13          .req r9    

 n_p3210           .req r10   
 n_p7654           .req r10   
 n_p111098         .req r10   
 n_p15141312       .req r10   

@//--------------------------------------------
@// Declare input registers
@//--------------------------------------------
 pSrcLeft        .req r0    @// input pointer
 pSrcAbove       .req r1    @// input pointer
 pSrcAboveLeft   .req r2    @// input pointer
 pDst            .req r3    @// output pointer
 leftStep        .req r4    @// input variable
 dstStep         .req r5    @// input variable
 predMode        .req r6    @// input variable
 availability    .req r7    @// input variable	
	
 p_Src          .req r0
 src_stride   .req r1
 p_Dst          .req r2
 dst_stride   .req r3
 src_above    .req r4
 src_left     .req r5
 n_x0           .req r6
 n_x1           .req r7
 n_x2           .req r8
 n_x3           .req r9
 n_x4           .req r10
 n_x5           .req r11
 n_x6           .req r12
 n_x7           .req r14

 dst2             .req r10
 dst_stride2      .req r11
 r01010101        .req r12

 DC_MODE = 0x34

@extern	void Predict8x8DCLeft_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right);  
@r0 p_Src
@r1 src_stride
@r2 topleft
@r3 topright  
Predict8x8DC128_ARMV6:
   ldr r2, =0x80808080
   ldr r3, =0x80808080
   strd r2, r3, [r0], r1
   strd r2, r3, [r0], r1
   strd r2, r3, [r0], r1
   strd r2, r3, [r0], r1
   strd r2, r3, [r0], r1
   strd r2, r3, [r0], r1
   strd r2, r3, [r0], r1
   strd r2, r3, [r0]
   bx lr



@extern	void Predict8x8DCLeft_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right);  
@r0 p_Src
@r1 src_stride
@r2 topleft
@r3 topright  
Predict8x8DCLeft_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
    SUB     p_Src, p_Src, #1
    CMP    r2,  #1
    LDREQB r4,  [p_Src, -src_stride]    
    LDRB   r6,  [p_Src], src_stride   
    LDRB   r12, [p_Src], src_stride
    LDRB   r14, [p_Src], src_stride
    ORR    r6, r12, LSL #8    
    ORR    r6, r14, LSL #16
    LDRB   r12, [p_Src], src_stride 
    LDRB   r7,  [p_Src], src_stride  
    ORR    r6, r12, LSL #24
    ANDNE  r4, r6, #0xFF  
    ORR    r4, r6, LSL #8      
    LDRB   r12, [p_Src], src_stride
    LDRB   r14, [p_Src], src_stride
    ORR    r7, r12, LSL #8   
    ORR    r7, r14, LSL #16
    LDRB   r12, [p_Src], src_stride
    MOV    r5, r6, LSR #24
    ORR    r7, r12, LSL #24    
    ORR    r5, r7, LSL #8
    MOV    r8, r6, LSR #8
    ORR    r8, r7, LSL #24
    CMP    r3, #1
    MOV    r9, r7, LSR #8
    ORR    r9, r12, LSL #24
 @   LDREQB r12, [p_Src]
 @   ORREQ  r9, r9, r12, LSL #24
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r6, r8
    USUB8   r6, r6, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r7, r9
    USUB8   r7, r7, r9    
      
    @ r6 r7   l0 l1 l2 l3 l4 l5 l6 l7      
     MOV     r12, #0
     MOV     r2, #4
   @  MVN     r10, r10
   @  MVN     r11, r11
     USADA8  r6, r6, r12, r2
     USADA8  r2, r7, r12, r6
          
     ADD     p_Src, p_Src, #1
     SUB     p_Src, p_Src, src_stride, LSL #3    

     LDR     r12, =0x01010101
     LSR     r2, r2, #3
     MUL     r2, r12, r2

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride
  
     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src]
        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}

@extern	void Predict8x8DCTop_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right);  
@r0 p_Src
@r1 src_stride
@r2 topleft
@r3 topright  
Predict8x8DCTop_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
    SUB     p_Src, p_Src, #4
    SUB     p_Src, p_Src, src_stride 
    LDM     p_Src, {r5, r10, r11, r14}    @src0, 1, 2, 3, 4, 5, 6, 7
    
    CMP     r2, #1
    MOVEQ   r4, r5, LSR #24  @p_Src-1, 0, 1, 2
    ANDNE   r4, r10, #0xFF    @src0 , 0, 1, 2
    ORR     r4, r10, LSL #8      
    MOV     r5, r10, LSR #24
    ORR     r5, r11, LSL #8    @src3 , 4, 5, 6  
    MOV     r8, r10, LSR #8   
    ORR     r8, r8, r11, LSL #24  @src1, 2, 3, 4
    MOV     r9, r11, LSR #8
    CMP     r3, #1
    ORREQ   r9, r9, r14, LSL #24  @src5, 6, 7, 8
    MOVNE   r14, r11, LSR #24
    ORRNE   r9, r9, r14, LSL #24   @src5, 6, 7, 7
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r10, r8
    USUB8   r10, r10, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r11, r9
    USUB8   r11, r11, r9 
      
    @ r6 r7   l0 l1 l2 l3 l4 l5 l6 l7      
    @ r10 r11 u0 u1 u2 u3 u4 u5 u6 u7
     MOV     r12, #0
     MOV     r2, #4
   @  MVN     r10, r10
   @  MVN     r11, r11
     USADA8  r10, r10, r12, r2
     USADA8  r2, r11, r12, r10
          
     ADD     p_Src, p_Src, #4
     ADD     p_Src, p_Src, src_stride    

     LDR     r12, =0x01010101
     LSR     r2, r2, #3
     MUL     r2, r12, r2

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride
  
     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src]
        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 



@extern	void Predict8x8DC_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right);  
@r0 p_Src
@r1 src_stride
@r2 topleft
@r3 topright  
Predict8x8DC_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
    SUB     p_Src, p_Src, #1
    CMP    r2,  #1
    LDREQB r4,  [p_Src, -src_stride]    
    LDRB   r6,  [p_Src], src_stride   
    LDRB   r12, [p_Src], src_stride
    LDRB   r14, [p_Src], src_stride
    ORR    r6, r12, LSL #8    
    ORR    r6, r14, LSL #16
    LDRB   r12, [p_Src], src_stride 
    LDRB   r7,  [p_Src], src_stride  
    ORR    r6, r12, LSL #24
    ANDNE  r4, r6, #0xFF  
    ORR    r4, r6, LSL #8      
    LDRB   r12, [p_Src], src_stride
    LDRB   r14, [p_Src], src_stride
    ORR    r7, r12, LSL #8   
    ORR    r7, r14, LSL #16
    LDRB   r12, [p_Src]
    MOV    r5, r6, LSR #24
    ORR    r7, r12, LSL #24    
    ORR    r5, r7, LSL #8
    MOV    r8, r6, LSR #8
    ORR    r8, r7, LSL #24
    CMP    r3, #1
    MOV    r9, r7, LSR #8
    ORR    r9, r12, LSL #24
 @   LDREQB r12, [p_Src]
 @   ORREQ  r9, r9, r12, LSL #24

     SUB     p_Src, p_Src, #3
     SUB     p_Src, p_Src, src_stride, LSL #3    

  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r6, r8
    USUB8   r6, r6, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r7, r9
    USUB8   r7, r7, r9    

    LDM     p_Src, {r5, r10, r11, r14}    @src0, 1, 2, 3, 4, 5, 6, 7
    
    CMP     r2, #1
    MOVEQ   r4, r5, LSR #24  @p_Src-1, 0, 1, 2
    ANDNE   r4, r10, #0xFF    @src0 , 0, 1, 2
    ORR     r4, r10, LSL #8      
    MOV     r5, r10, LSR #24
    ORR     r5, r11, LSL #8    @src3 , 4, 5, 6  
    MOV     r8, r10, LSR #8   
    ORR     r8, r8, r11, LSL #24  @src1, 2, 3, 4
    MOV     r9, r11, LSR #8
    CMP     r3, #1
    ORREQ   r9, r9, r14, LSL #24  @src5, 6, 7, 8
    MOVNE   r14, r11, LSR #24
    ORRNE   r9, r9, r14, LSL #24   @src5, 6, 7, 7
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r10, r8
    USUB8   r10, r10, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r11, r9
    USUB8   r11, r11, r9 
      
    @ r6 r7   l0 l1 l2 l3 l4 l5 l6 l7      
    @ r10 r11 u0 u1 u2 u3 u4 u5 u6 u7
     MOV     r12, #0
     MOV     r2, #8
   @  MVN     r10, r10
   @  MVN     r11, r11
     USADA8  r6, r6, r12, r2
     USADA8  r7, r7, r12, r6
     USADA8  r10, r10, r12, r7
     USADA8  r2, r11, r12, r10
          
     ADD     p_Src, p_Src, #4
     ADD     p_Src, p_Src, src_stride    

     LDR     r12, =0x01010101
     LSR     r2, r2, #4
     MUL     r2, r12, r2

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride
  
     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src], src_stride

     STR    r2, [p_Src, #4]
     STR    r2, [p_Src]
        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 
    .ltorg

@extern	void PredIntraLuma8x8H_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right);  
@r0 p_Src
@r1 src_stride
@r2 topleft
@r3 topright  
PredIntraLuma8x8H_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
    SUB     p_Src, p_Src, #1
    CMP    r2,  #1
    LDREQB r4,  [p_Src, -src_stride]    
    LDRB   r6,  [p_Src], src_stride   
    LDRB   r12, [p_Src], src_stride
    LDRB   r14, [p_Src], src_stride
    ORR    r6, r12, LSL #8    
    ORR    r6, r14, LSL #16
    LDRB   r12, [p_Src], src_stride 
    LDRB   r7,  [p_Src], src_stride  
    ORR    r6, r12, LSL #24
    ANDNE  r4, r6, #0xFF  
    ORR    r4, r6, LSL #8      
    LDRB   r12, [p_Src], src_stride
    LDRB   r14, [p_Src], src_stride
    ORR    r7, r12, LSL #8   
    ORR    r7, r14, LSL #16
    LDRB   r12, [p_Src], src_stride
    MOV    r5, r6, LSR #24
    ORR    r7, r12, LSL #24    
    ORR    r5, r7, LSL #8
    MOV    r8, r6, LSR #8
    ORR    r8, r7, LSL #24
    CMP    r3, #1
    MOV    r9, r7, LSR #8
    ORR    r9, r12, LSL #24
 @   LDREQB r12, [p_Src]
 @   ORREQ  r9, r9, r12, LSL #24
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r6, r8
    USUB8   r6, r6, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r7, r9
    USUB8   r7, r7, r9    
    
    LDR    r01010101, =0x01010101
    
    MOV     r14, #2
    ADD     p_Src, p_Src, #1
    SUB     p_Src, p_Src, src_stride, LSL #3
    
    AND    r4, r6, #0xFF
    MUL    r4, r4, r01010101
    LSR    r6, r6, #8 
    AND    r5, r6, #0xFF
    STR    r4, [p_Src, #4]
    STR    r4, [p_Src], src_stride  
    
    MUL    r5, r5, r01010101
    LSR    r6, r6, #8 
    AND    r4, r6, #0xFF   
    STR    r5, [p_Src, #4]
    STR    r5, [p_Src], src_stride    
 
    MUL    r4, r4, r01010101
    LSR    r5, r6, #8 
    STR    r4, [p_Src, #4]
    STR    r4, [p_Src], src_stride   
    
   
@    AND    r4, r6, #0xFF
    MUL    r5, r5, r01010101
    AND    r4, r7, #0xFF
    STR    r5, [p_Src, #4]
    STR    r5, [p_Src], src_stride  
      
    MUL    r4, r4, r01010101
    LSR    r7, r7, #8 
    AND    r5, r7, #0xFF   
    STR    r4, [p_Src, #4]
    STR    r4, [p_Src], src_stride
   
    MUL    r5, r5, r01010101
    LSR    r7, r7, #8 
    AND    r4, r7, #0xFF   
    STR    r5, [p_Src, #4]
    STR    r5, [p_Src], src_stride   
 
    MUL    r4, r4, r01010101
    LSR    r5, r7, #8 
    STR    r4, [p_Src, #4]
    STR    r4, [p_Src], src_stride      
   
@    AND    r4, r6, #0xFF
    MUL    r5, r5, r01010101
    STR    r5, [p_Src, #4]
    STR    r5, [p_Src]      
  
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 

@extern	void Predict8x8HorUp_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right);  
@r0 p_Src
@r1 src_stride
@r2 topleft
@r3 topright  
Predict8x8HorUp_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
    SUB     p_Src, p_Src, #1
    CMP    r2,  #1
    LDREQB r4,  [p_Src, -src_stride]    
    LDRB   r6,  [p_Src], src_stride   
    LDRB   r12, [p_Src], src_stride
    LDRB   r14, [p_Src], src_stride
    ORR    r6, r12, LSL #8    
    ORR    r6, r14, LSL #16
    LDRB   r12, [p_Src], src_stride 
    LDRB   r7,  [p_Src], src_stride  
    ORR    r6, r12, LSL #24
    ANDNE  r4, r6, #0xFF  
    ORR    r4, r6, LSL #8      
    LDRB   r12, [p_Src], src_stride
    LDRB   r14, [p_Src], src_stride
    ORR    r7, r12, LSL #8   
    ORR    r7, r14, LSL #16
    LDRB   r12, [p_Src], src_stride
    MOV    r5, r6, LSR #24
    ORR    r7, r12, LSL #24    
    ORR    r5, r7, LSL #8
    MOV    r8, r6, LSR #8
    ORR    r8, r7, LSL #24
    CMP    r3, #1
    MOV    r9, r7, LSR #8
    ORR    r9, r12, LSL #24
 @   LDREQB r12, [p_Src]
 @   ORREQ  r9, r9, r12, LSL #24
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r6, r8
    USUB8   r6, r6, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r7, r9
    USUB8   r7, r7, r9 
    
    @r6 r7 l0 l1 l2 ... l7
    MOV  r14, r7, LSR #24
    MOV r2, r6, LSR #8
    ORR r2, r7, LSL #24
    MOV r3, r7, LSR #8
    ORR r3, r3, r14, LSL #24
    
    UHSUB8 r10, r2, r6
    USUB8  r2, r2, r10
    UHSUB8 r11, r3, r7
    USUB8  r3, r3, r11
    
    MOV  r8, r6, LSR #8
    ORR  r8, r7, LSL #24
    MOV  r9, r7, LSR #8
    ORR  r9, r9, r14, LSL #24
    
    MOV  r10, r8, LSR #8
    ORR  r10, r9, LSL #24
    MOV  r11, r9, LSR #8
    ORR  r11, r11, r14, LSL #24
    
    UHADD8 r10, r10, r6
    UHSUB8 r10, r8, r10
    USUB8  r4,  r8, r10
    UHADD8 r11, r11, r7
    UHSUB8 r11, r9, r11
    USUB8  r5,  r9, r11
    
    @r2 r3 a0 - a7
    @r4 r5 l0 - l7 
    LDR     r12, =0x0000FFFF
    LDR     r14, =0xFF0000FF   
     
     PKHBT  r10, r3, r5, LSL #16 @
     PKHTB  r11, r5, r3, ASR #16 @
     AND    r8, r10, r14
     AND    r9, r12, r10, LSR #8
     REV16  r9, r9
     ORR    r8, r8, r9, LSL #8
     AND    r10, r11, r14
     AND    r9, r12, r11, LSR #8
     REV16  r9, r9
     ORR    r9, r10, r9, LSL #8

     PKHBT  r10, r2, r4, LSL #16 @
     PKHTB  r11, r4, r2, ASR #16 @
     AND    r3, r10, r14
     AND    r7, r12, r10, LSR #8
     REV16  r7, r7
     ORR    r6, r3, r7, LSL #8
     AND    r10, r11, r14
     AND    r7, r12, r11, LSR #8
     REV16  r7, r7
     ORR    r7, r10, r7, LSL #8 
     
     @r6 r7 a0 l0 a1 l1 a2 l2 a3 l3
     @r8 r9 a4 l4 a5 l5 a6 l6 a6 l7
         
       
    ADD     p_Src, p_Src, #1
    SUB     p_Src, p_Src, src_stride, LSL #3
    MOV   r14, r9, LSR #24 @l7
    LDR   r12, =0x01010101
    MUL   r14, r14, r12
 
 @ row 0
    STR   r7, [p_Src, #4]
    STR   r6, [p_Src], src_stride   

@ row 1
    MOV   r4, r6, LSR #16
    ORR   r4, r7, LSL #16
    MOV   r5, r7, LSR #16
    ORR   r5, r8, LSL #16
 
    STR   r5, [p_Src, #4]
    STR   r4, [p_Src], src_stride      
 
 @ row 2
    STR   r8, [p_Src, #4]
    STR   r7, [p_Src], src_stride   
 
 @ row 3
    MOV   r4, r7, LSR #16
    ORR   r4, r8, LSL #16
    MOV   r5, r8, LSR #16
    ORR   r5, r9, LSL #16   
  
    STR   r5, [p_Src, #4]
    STR   r4, [p_Src], src_stride       
  
 @ row 4
    STR   r9, [p_Src, #4]
    STR   r8, [p_Src], src_stride   
 
 @ row 5
    MOV   r4, r8, LSR #16
    ORR   r4, r9, LSL #16
    MOV   r5, r9, LSR #16
    ORR   r5, r14, LSL #16   
  
    STR   r5, [p_Src, #4]
    STR   r4, [p_Src], src_stride 
 
 @ row 6
    STR   r14, [p_Src, #4]
    STR   r9, [p_Src], src_stride   
 
 @ row 7
    MOV   r4, r9, LSR #16
    ORR   r4, r14, LSL #16
    MOV   r5, r14, LSR #16
    ORR   r5, r14, LSL #16   
  
    STR   r5, [p_Src, #4]
    STR   r4, [p_Src]  
  
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 

@extern	void Predict8x8HorDown_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right);  
@r0 p_Src
@r1 src_stride
@r2 topleft
@r3 topright  
Predict8x8HorDown_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
    SUB     p_Src, p_Src, #1
    CMP    r2,  #1
    LDREQB r4,  [p_Src, -src_stride]    
    LDRB   r6,  [p_Src], src_stride   
    LDRB   r12, [p_Src], src_stride
    LDRB   r14, [p_Src], src_stride
    ORR    r6, r12, LSL #8    
    ORR    r6, r14, LSL #16
    LDRB   r12, [p_Src], src_stride 
    LDRB   r7,  [p_Src], src_stride  
    ORR    r6, r12, LSL #24
    ANDNE  r4, r6, #0xFF  
    ORR    r4, r6, LSL #8      
    LDRB   r12, [p_Src], src_stride
    LDRB   r14, [p_Src], src_stride
    ORR    r7, r12, LSL #8   
    ORR    r7, r14, LSL #16
    LDRB   r12, [p_Src]
    MOV    r5, r6, LSR #24
    ORR    r7, r12, LSL #24    
    ORR    r5, r7, LSL #8
    MOV    r8, r6, LSR #8
    ORR    r8, r7, LSL #24
    CMP    r3, #1
    MOV    r9, r7, LSR #8
    ORR    r9, r12, LSL #24
 @   LDREQB r12, [p_Src]
 @   ORREQ  r9, r9, r12, LSL #24


    SUB    p_Src, p_Src, src_stride, LSL #3
    LDRB   r10, [p_Src, src_stride]
    LDRB   r11, [p_Src]
    LDRB   r12, [p_Src, #1]
    UHADD8  r12, r12, r10
    UHSUB8  r12, r11, r12
    USUB8   r12, r11, r12
    ORR    r1, r12, r1, LSL #8
    SUB    p_Src, p_Src, #3
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r6, r8
    USUB8   r6, r6, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r7, r9
    USUB8   r7, r7, r9    

    LDM     p_Src, {r5, r10, r11, r14}    @src0, 1, 2, 3, 4, 5, 6, 7
    
    CMP     r2, #1
    MOVEQ   r4, r5, LSR #24  @p_Src-1, 0, 1, 2
    ANDNE   r4, r10, #0xFF    @src0 , 0, 1, 2
    ORR     r4, r10, LSL #8      
    MOV     r5, r10, LSR #24
    ORR     r5, r11, LSL #8    @src3 , 4, 5, 6  
    MOV     r8, r10, LSR #8   
    ORR     r8, r8, r11, LSL #24  @src1, 2, 3, 4
    MOV     r9, r11, LSR #8
    CMP     r3, #1
    ORREQ   r9, r9, r14, LSL #24  @src5, 6, 7, 8
    MOVNE   r14, r11, LSR #24
    ORRNE   r9, r9, r14, LSL #24   @src5, 6, 7, 7
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r10, r8
    USUB8   r10, r10, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r11, r9
    USUB8   r11, r11, r9 
      
    @ r6 r7   l0 l1 l2 l3 l4 l5 l6 l7      
    @ r10 r11 u0 u1 u2 u3 u4 u5 u6 u7  
    @ r1  src_stride | lt
    AND r2, r1, #0xFF
    ORR r2, r2, r6, LSL #8
    MOV r3, r6, LSR #24
    ORR r3, r3, r7, LSL #8
    MOV r4, r6,LSR #8
    ORR r4, r4, r7, LSL #24
    MOV r5, r7, LSR #8
    
    @r2 r3 (lt + 2*l0 + l1 + 2) >> 2
    UHADD8 r4, r4, r2
    UHSUB8 r4, r6, r4
    USUB8  r2, r6, r4
    UHADD8 r5, r5, r3
    UHSUB8 r5, r7, r5
    USUB8  r3, r7, r5
    
   
    AND r4, r1, #0xFF          @ b
    ORR r4, r4, r10, LSL #8
    MOV r5, r10, LSR #24
    ORR r5, r5, r11, LSL #8
    AND r8, r1, #0xFF          @ c
    AND r9, r6, #0xFF
    ORR r8, r9, r8, LSL #8
    ORR r8, r8, r10, LSL #16
    MOV r9, r10, LSR #16
    ORR r9, r11, LSL #16
    
     @r4 r5 (l0 + 2*lt + u0 + 2) >> 2
     UHADD8 r8, r8, r10
     UHSUB8 r8, r4, r8
     USUB8  r4, r4, r8
     UHADD8 r9, r9, r11
     UHSUB8 r9, r5, r9
     USUB8  r5, r5, r9
     
     @r6 r7 (lt + l0 + 1) >> 1
     AND r8, r1, #0xFF
     ORR r8, r6, LSL #8
     MOV r9, r6, LSR #24
     ORR r9, r7, LSL #8
     
     UHSUB8 r8, r6, r8
     USUB8  r6, r6, r8
     UHSUB8 r9, r7, r9
     USUB8  r7, r7, r9
     
     ADD     p_Src, p_Src, #4
     ADD     p_Src, p_Src, src_stride, LSR #8     
     LDR     r12, =0x0000FFFF
     LDR     r14, =0xFF0000FF     
     
     REV    r2, r2  @l3 l2 l1 l0
     REV    r3, r3  @l7 l6 l5 l4
     REV    r6, r6  @a3 a2 a1 a0
     REV    r7, r7  @a7 a6 a5 a4
     
     PKHBT  r10, r3, r7, LSL #16 @l7 l6 a7 a6
     PKHTB  r11, r7, r3, ASR #16 @l5 l4 a5 a4
     AND    r8, r10, r14
     AND    r9, r12, r10, LSR #8
     REV16  r9, r9
     ORR    r8, r8, r9, LSL #8
     AND    r10, r11, r14
     AND    r9, r12, r11, LSR #8
     REV16  r9, r9
     ORR    r9, r10, r9, LSL #8

     PKHBT  r10, r2, r6, LSL #16 @l7 l6 a7 a6
     PKHTB  r11, r6, r2, ASR #16 @l5 l4 a5 a4
     AND    r3, r10, r14
     AND    r7, r12, r10, LSR #8
     REV16  r7, r7
     ORR    r6, r3, r7, LSL #8
     AND    r10, r11, r14
     AND    r7, r12, r11, LSR #8
     REV16  r7, r7
     ORR    r7, r10, r7, LSL #8 
     
     LDR    r14, =0xFF
     @ r8 l7 a7 l6 a6
     @ r9 l5 a5 l4 a4
     @ r6 l3 a3 l2 a2
     @ r7 l1 a1 l0 a0         
     @ r4 u0 u1 u2 u3
     @ r5 u4 u5 u6 u7
             
@row 0
     MOV   r2, r7, LSR #24
     ORR   r2, r2, r4, LSL #8
     MOV   r3, r4, LSR #24
     ORR   r3, r5, LSL #8
     STR   r3, [p_Src, #4]
     STR   r2, [p_Src], src_stride, LSR #8    

@row 1
     MOV  r2, r7, LSR #8
     ORR  r2, r4, LSL #24
     MOV  r3, r4, LSR #8
     ORR  r3, r5, LSL #24
 
     STR   r3, [p_Src, #4]
     STR   r2, [p_Src], src_stride, LSR #8 
   
@row 2
     MOV  r2, r6, LSR #24
     ORR  r2, r7, LSL #8
     
     MOV  r3, r7, LSR #24
     ORR  r3, r4, LSL #8
     
     STR   r3, [p_Src, #4]
     STR   r2, [p_Src], src_stride, LSR #8                 

@row 3
     MOV  r2, r6, LSR #8
     ORR  r2, r7, LSL #24
     MOV  r3, r7, LSR #8
     ORR  r3, r4, LSL #24
 
     STR   r3, [p_Src, #4]
     STR   r2, [p_Src], src_stride, LSR #8 

@row 4
     MOV  r2, r9, LSR #24
     ORR  r2, r6, LSL #8
     
     MOV  r3, r6, LSR #24
     ORR  r3, r7, LSL #8
     
     STR   r3, [p_Src, #4]
     STR   r2, [p_Src], src_stride, LSR #8                 

@row 5
     MOV  r2, r9, LSR #8
     ORR  r2, r6, LSL #24
     MOV  r3, r6, LSR #8
     ORR  r3, r7, LSL #24
 
     STR   r3, [p_Src, #4]
     STR   r2, [p_Src], src_stride, LSR #8 

@row 6
     MOV  r2, r8, LSR #24
     ORR  r2, r9, LSL #8
     
     MOV  r3, r9, LSR #24
     ORR  r3, r6, LSL #8
     
     STR   r3, [p_Src, #4]
     STR   r2, [p_Src], src_stride, LSR #8                 

@row 7
     MOV  r2, r8, LSR #8
     ORR  r2, r9, LSL #24
     MOV  r3, r9, LSR #8
     ORR  r3, r6, LSL #24
 
     STR   r3, [p_Src, #4]
     STR   r2, [p_Src]           
        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}     

@extern	void Predict8x8VerRight_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right);  
@r0 p_Src
@r1 src_stride
@r2 topleft
@r3 topright  
Predict8x8VerRight_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
    LDRB   r8, [p_Src, -src_stride] 
    SUB     p_Src, p_Src, #1
    LDRB   r10, [p_Src]       @lt    
    LDRB   r11, [p_Src, -src_stride]   
    CMP    r2,  #1
    LDREQB r4,  [p_Src, -src_stride]  @l-1  
    LDRB   r6,  [p_Src], src_stride   @l0
    LDRB   r12, [p_Src], src_stride   @l1
    LDRB   r14, [p_Src], src_stride   @l2
    LSL    r6, r6, #8
    ORR    r6, r6, r12, LSL #16    
    
    LDRB   r12, [p_Src], src_stride  @l3
    LDRB   r7,  [p_Src], src_stride  @l4
 @   ORR    r6, r12, LSL #24

    ANDNE  r4, r6, #0xFF00        
    ORR    r4, r10, r4, LSL #8   
    ORR    r4, r6, LSL #8        @lt l(-1)  l0 l1
    ORR    r6, r6, r14, LSL #24
    ORR    r6, r6, r11           @lt l0     l1 l2
    ORR    r8, r8, r12, LSL #24
@    LDRB   r12, [p_Src], src_stride
    LDRB   r14, [p_Src], src_stride @l5
    ORR    r7, r12, r7,  LSL #8   
    ORR    r7, r7,  r14, LSL #16
    LDRB   r12, [p_Src], src_stride @l6
    MOV    r5, r6, LSR #24
    ORR    r7, r12, LSL #24    @l3 l4 l5 l6
    ORR    r5, r7, LSL #8      @l2 l3 l4 l5
    LSR    r10, r6, #16
    ORR    r8, r8, r10, LSL #8  @lt l1 l2 l3
 @   ORR    r8, r7, LSL #24     
@    CMP    r3, #1
    
@    ORR    r9, r12, LSL #24    ;l4 l5 l6 l7
    LDRB r12, [p_Src]
    MOV  r9, r7, LSR #8      @
    ORR  r9, r9, r12, LSL #24   @l4 l5 l6 l7

    SUB    p_Src, p_Src, #3
    SUB    p_Src, p_Src, src_stride, LSL #3
@    LDRB   r12, [p_Src, #3]
@    LDRB   r14, [p_Src, #4]
@    AND    r5, r5, #0xFF000000
@    ORR    r5, r5, r12, LSL #24
@    AND    r7, r7, #0xFF000000
@    ORR    r5, r5, r12, LSL #24
@    AND    r9, r9, #0xFF000000
@    ORR    r9, r9, r14, LSL #24
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r6, r8
    USUB8   r6, r6, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r7, r9
    USUB8   r7, r7, r9 
    
 
    LDM     p_Src, {r5, r10, r11, r14}    @src0, 1, 2, 3, 4, 5, 6, 7
    
    CMP     r2, #1
    MOVEQ   r4, r5, LSR #24  @p_Src-1, 0, 1, 2
    ANDNE   r4, r10, #0xFF    @src0 , 0, 1, 2
    ORR     r4, r10, LSL #8      
    MOV     r5, r10, LSR #24
    ORR     r5, r11, LSL #8    @src3 , 4, 5, 6  
    MOV     r8, r10, LSR #8   
    ORR     r8, r8, r11, LSL #24  @src1, 2, 3, 4
    MOV     r9, r11, LSR #8
    CMP     r3, #1
    ORREQ   r9, r9, r14, LSL #24  @src5, 6, 7, 8
    MOVNE   r14, r11, LSR #24
    ORRNE   r9, r9, r14, LSL #24   @src5, 6, 7, 7
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r10, r8
    USUB8   r10, r10, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r11, r9
    USUB8   r11, r11, r9  
    
    @ r6 r7   lt l0 l1 l2 l3 l4 l5 l6       
    @ r10 r11 u0 u1 u2 u3 u4 u5 u6 u7
    LDR  r14, =0xFF
    
    AND  r2, r10, r14
    ORR  r2, r6, LSL #8
    MOV  r3, r6, LSR #24
    ORR  r3, r7, LSL #8
    
    AND  r4, r14, r10, LSR #8
    ORR  r4, r2, LSL #8
    MOV  r5, r2, LSR #24
    ORR  r5, r3, LSL #8
    
    UHADD8  r4, r4, r6
    UHSUB8  r4, r2, r4
    USUB8   r2, r2, r4  
    UHADD8  r5, r5, r7
    UHSUB8  r5, r3, r5
    USUB8   r3, r3, r5
    
    MOV   r8, r10, LSR #8
    ORR   r8, r8,  r11, LSL #24
  @  MOV   r9, r10, LSR #24
  @  ORR   r9, r9, r11, LSL #8
    MOV  r9, r11, LSR #8
    
    MOV   r4, r8, LSR #8
    ORR   r4, r4, r9, LSL #24
@    MOV   r5, r8, LSR #24
@    ORR   r5, r5, r9, LSL #8  
    MOV  r5, r9, LSR #8
    
    UHADD8  r4, r4, r10
    UHSUB8  r4, r8, r4
    USUB8   r8, r8, r4  
    UHADD8  r5, r5, r11
    UHSUB8  r5, r9, r5
    USUB8   r9, r9, r5 
    
    AND   r4, r6, #0xFF
    ORR   r4, r4, r10, LSL #8
    MOV   r5, r10, LSR #24
    ORR   r5, r11, LSL #8
    
    UHSUB8 r10, r4, r10
    USUB8  r4, r4, r10
    UHSUB8 r11, r5, r11
    USUB8  r5, r5, r11 
    
  @output
  @r2 r3 l0 - l7
  @r4 r5 a0 - a7
  @r8 r9 u0 - u7
    ADD   p_Src, p_Src, #4
    ADD   p_Src, p_Src, src_stride
    
    LDR  r12, =0xFF
    
  @ row 0   
    STR   r5, [p_Src, #4]
    STR   r4, [p_Src], src_stride
 
  @ row 1
 @   ROR  r14, r2, #8
@    AND  r14, r14, r12
@    ORR  r14, r14, r14,LSL #16 
    REV  r6, r2    
    MOV  r6, r6, LSR #16
    ORR  r6, r8, LSL #16
    MOV  r7, r8, LSR #16
    ORR  r7, r9, LSL #16
 
    STR  r7, [p_Src, #4]
    STR  r6, [p_Src], src_stride
    
 @ row 2
    MOV  r6, r2, LSR #16
    AND  r6, r6, #0xFF
    ORR  r6, r4, LSL #8
    MOV  r7, r4, LSR #24
    ORR  r7, r5, LSL #8   
    
    STR  r7, [p_Src, #4]
    STR  r6, [p_Src], src_stride
    
 @ row 3
    REV  r6, r2
    AND  r7, r6, #0xFF
    MOV  r6, r6, LSR #16
    ORR  r6, r7, r6, LSL #8
    ORR  r6, r6, r8, LSL #24
    MOV  r7, r8, LSR #8
    ORR  r7, r7, r9, LSL #24  

    STR  r7, [p_Src, #4]
    STR  r6, [p_Src], src_stride   
 
 @ row 4
    AND  r6, r3, r12
    AND  r7, r12, r2, LSR #16
    ORR  r6, r6, r7, LSL #8
    ORR  r6, r6, r4, LSL #16
    MOV  r7, r4, LSR #16
    ORR  r7, r7, r5, LSL #16  

    STR  r7, [p_Src, #4]
    STR  r6, [p_Src], src_stride   
 
 @ row 5   
    REV  r6, r2
    AND  r7, r6, #0xFF
    LSL  r7, r7, #8
    MOV  r6, r6, LSR #16
    ORR  r6, r7, r6, LSL #16
    AND r7, r12, r3, LSR #8
    ORR  r6, r6, r7       
  
    STR  r8, [p_Src, #4]
    STR  r6, [p_Src], src_stride  
 
 @ row 6
    AND r7, r12, r3, LSR #16
    AND r6, r12, r3
    ORR r6, r7, r6, LSL #8
    AND r7, r12, r2, LSR #16
    ORR r6, r6, r7, LSL #16
    ORR r6, r6, r4, LSL #24
    MOV r7, r4, LSR #8
    ORR r7, r7, r5, LSL #24
  
    STR  r7, [p_Src, #4]
    STR  r6, [p_Src], src_stride  

@ row 7
    AND r7 , r12, r3, LSR #24
    AND r6, r12, r3, LSR #8
    ORR r6, r7, r6, LSL #8
    AND r7, r12, r2,  LSR #24
    ORR r6, r6, r7, LSL #16
    AND r7, r12, r2, LSR #8
    ORR r6, r6, r7, LSL #24
    
    AND r7, r12, r2
    ORR r7, r7, r8, LSL #8    
  
    STR  r7, [p_Src, #4]
    STR  r6, [p_Src]     
       
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}      

@extern	void Predict8x8DownRight_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right);  
@r0 p_Src
@r1 src_stride
@r2 topleft
@r3 topright  
Predict8x8DownRight_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
    SUB     p_Src, p_Src, #1
    CMP    r2,  #1
    LDREQB r4,  [p_Src, -src_stride]    
    LDRB   r6,  [p_Src], src_stride   
    LDRB   r12, [p_Src], src_stride
    LDRB   r14, [p_Src], src_stride
    ORR    r6, r12, LSL #8    
    ORR    r6, r14, LSL #16
    LDRB   r12, [p_Src], src_stride 
    LDRB   r7,  [p_Src], src_stride  
    ORR    r6, r12, LSL #24
    ANDNE  r4, r6, #0xFF  
    ORR    r4, r6, LSL #8      
    LDRB   r12, [p_Src], src_stride
    LDRB   r14, [p_Src], src_stride
    ORR    r7, r12, LSL #8   
    ORR    r7, r14, LSL #16
    LDRB   r12, [p_Src]
    MOV    r5, r6, LSR #24
    ORR    r7, r12, LSL #24    
    ORR    r5, r7, LSL #8
    MOV    r8, r6, LSR #8
    ORR    r8, r7, LSL #24
    CMP    r3, #1
    MOV    r9, r7, LSR #8
    ORR    r9, r12, LSL #24
 @   LDREQB r12, [p_Src]
 @   ORREQ  r9, r9, r12, LSL #24


    SUB    p_Src, p_Src, src_stride, LSL #3
    LDRB   r10, [p_Src, src_stride]
    LDRB   r11, [p_Src]
    LDRB   r12, [p_Src, #1]
    UHADD8  r12, r12, r10
    UHSUB8  r12, r11, r12
    USUB8   r12, r11, r12
    ORR    r1, r12, r1, LSL #8
    SUB    p_Src, p_Src, #3
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r6, r8
    USUB8   r6, r6, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r7, r9
    USUB8   r7, r7, r9    

    LDM     p_Src, {r5, r10, r11, r14}    @src0, 1, 2, 3, 4, 5, 6, 7
    
    CMP     r2, #1
    MOVEQ   r4, r5, LSR #24  @p_Src-1, 0, 1, 2
    ANDNE   r4, r10, #0xFF    @src0 , 0, 1, 2
    ORR     r4, r10, LSL #8      
    MOV     r5, r10, LSR #24
    ORR     r5, r11, LSL #8    @src3 , 4, 5, 6  
    MOV     r8, r10, LSR #8   
    ORR     r8, r8, r11, LSL #24  @src1, 2, 3, 4
    MOV     r9, r11, LSR #8
    CMP     r3, #1
    ORREQ   r9, r9, r14, LSL #24  @src5, 6, 7, 8
    MOVNE   r14, r11, LSR #24
    ORRNE   r9, r9, r14, LSL #24   @src5, 6, 7, 7
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r10, r8
    USUB8   r10, r10, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r11, r9
    USUB8   r11, r11, r9 
      
    @ r6 r7   l0 l1 l2 l3 l4 l5 l6 l7      
    @ r10 r11 u0 u1 u2 u3 u4 u5 u6 u7  
    @ r1  src_stride | lt
    AND r2, r1, #0xFF
    ORR r2, r2, r6, LSL #8
    MOV r3, r6, LSR #24
    ORR r3, r3, r7, LSL #8
    MOV r4, r6,LSR #8
    ORR r4, r4, r7, LSL #24
    MOV r5, r7, LSR #8
    
    @r2 r3 (lt + 2*l0 + l1 + 2) >> 2
    UHADD8 r4, r4, r2
    UHSUB8 r4, r6, r4
    USUB8  r2, r6, r4
    UHADD8 r5, r5, r3
    UHSUB8 r5, r7, r5
    USUB8  r3, r7, r5
    
   
    AND r4, r1, #0xFF          @ b
    ORR r4, r4, r10, LSL #8
    MOV r5, r10, LSR #24
    ORR r5, r5, r11, LSL #8
    AND r8, r1, #0xFF          @ c
    AND r9, r6, #0xFF
    ORR r8, r9, r8, LSL #8
    ORR r8, r8, r10, LSL #16
    MOV r9, r10, LSR #16
    ORR r9, r11, LSL #16
    
     @r4 r5 (l0 + 2*lt + u0 + 2) >> 2
     UHADD8 r8, r8, r10
     UHSUB8 r8, r4, r8
     USUB8  r4, r4, r8
     UHADD8 r9, r9, r11
     UHSUB8 r9, r5, r9
     USUB8  r5, r5, r9
       
     
     ADD     p_Src, p_Src, #4
     ADD     p_Src, p_Src, src_stride, LSR #8  
     
     REV    r2, r2
     REV    r3, r3   
     
     LDR    r14, =0xFF
     @ r2 l3 l2 l1 l0
     @ r3 l7 l6 l5 l4        
     @ r4 u0 u1 u2 u3
     @ r5 u4 u5 u6 u7
             
@row 0
     STR   r5, [p_Src, #4]
     STR   r4, [p_Src], src_stride, LSR #8    

@row 1
     MOV   r8, r2, LSR #24
     ORR   r8, r4, LSL #8
     MOV   r9, r4, LSR #24
     ORR   r9, r5, LSL #8
     
     STR   r9, [p_Src, #4]
     STR   r8, [p_Src], src_stride, LSR #8 
   
@row 2
     MOV   r8, r2, LSR #16
     ORR   r8, r4, LSL #16
     MOV   r9, r4, LSR #16
     ORR   r9, r5, LSL #16
     
     STR   r9, [p_Src, #4]
     STR   r8, [p_Src], src_stride, LSR #8                

@row 3
      MOV   r8, r2, LSR #8
     ORR   r8, r4, LSL #24
     MOV   r9, r4, LSR #8
     ORR   r9, r5, LSL #24
     
     STR   r9, [p_Src, #4]
     STR   r8, [p_Src], src_stride, LSR #8 

@row 4
     
     STR   r4, [p_Src, #4]
     STR   r2, [p_Src], src_stride, LSR #8                  

@row 5
     MOV   r8, r3, LSR #24
     ORR   r8, r2, LSL #8
     MOV   r9, r2, LSR #24
     ORR   r9, r4, LSL #8
     
     STR   r9, [p_Src, #4]
     STR   r8, [p_Src], src_stride, LSR #8  

@row 6
     MOV   r8, r3, LSR #16
     ORR   r8, r2, LSL #16
     MOV   r9, r2, LSR #16
     ORR   r9, r4, LSL #16
     
     STR   r9, [p_Src, #4]
     STR   r8, [p_Src], src_stride, LSR #8                 

@row 7
     MOV   r8, r3, LSR #8
     ORR   r8, r2, LSL #24
     MOV   r9, r2, LSR #8
     ORR   r9, r4, LSL #24
     
     STR   r9, [p_Src, #4]
     STR   r8, [p_Src]
        
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}           

@extern	void PredIntraLuma8x8V_ARMV6( VO_U8 *p_Src , VO_S32 src_stride,VO_S32 block_available_up_left, VO_S32 block_available_up_right);
@r0 p_Src
@r1 src_stride
@r2 block_available_up_left
@r3 block_available_up_right
PredIntraLuma8x8V_ARMV6: 
    STMFD   sp!, {r0-r11, lr}
 
    SUB     p_Src, src_stride
    SUB     p_Src, p_Src, #4
    LDM     p_Src, {r5, r6, r7, r14}    @src0, 1, 2, 3, 4, 5, 6, 7
    
    CMP     r2, #1
    MOVEQ   r4, r5, LSR #24  @p_Src-1, 0, 1, 2
    ANDNE   r4, r6, #0xFF    @src0 , 0, 1, 2
    ORR     r4, r6, LSL #8      
    MOV     r5, r6, LSR #24
    ORR     r5, r7, LSL #8    @src3 , 4, 5, 6  
    MOV     r8, r6, LSR #8   
    ORR     r8, r8, r7, LSL #24  @src1, 2, 3, 4
    MOV     r9, r7, LSR #8
    CMP     r3, #1
    ORREQ   r9, r9, r14, LSL #24  @src5, 6, 7, 8
    MOVNE   r14, r7, LSR #24
    ORRNE   r9, r9, r14, LSL #24   @src5, 6, 7, 7
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r6, r8
    USUB8   r6, r6, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r7, r9
    USUB8   r7, r7, r9
        
    MOV     y, #8
 @   LDM     p_Src, {n_x0, n_x1}
    ADD    p_Src, src_stride
    ADD    p_Src, p_Src, #4
    ADD     dst_stride2, src_stride, src_stride
    ADD     dst2, p_Src, src_stride
 
Loop_V: 
    STM     p_Src, {n_x0, n_x1}
    SUBS    y, #2
    ADD     p_Src, p_Src,  dst_stride2
    STM     dst2,{n_x0, n_x1}    
    ADD     dst2, dst2, dst_stride2
    BNE     Loop_V
    
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 

@extern  void Predict8x8VerLeft_ARMV6(VO_U8 *p_Src, VO_S32 stride, int topleft, int topright)
@r0 p_Src
@r1 stride
@r2 topleft
@r3 topright
Predict8x8VerLeft_ARMV6:
    STMFD   sp!, {r0-r11, lr}
 
    SUB     p_Src, src_stride
    SUB     p_Src, p_Src, #4
    LDM     p_Src, {r5, r6, r7, r14}    @src0, 1, 2, 3, 4, 5, 6, 7
    
    CMP     r2, #1
    MOVEQ   r4, r5, LSR #24  @p_Src-1, 0, 1, 2
    ANDNE   r4, r6, #0xFF    @src0 , 0, 1, 2
    ORR     r4, r6, LSL #8      
    MOV     r5, r6, LSR #24
    ORR     r5, r7, LSL #8    @src3 , 4, 5, 6  
    MOV     r8, r6, LSR #8   
    ORR     r8, r8, r7, LSL #24  @src1, 2, 3, 4
    MOV     r9, r7, LSR #8
    CMP     r3, #1
    MOV    r10, r7, LSR #24
    ORREQ   r9, r9, r14, LSL #24  @src5, 6, 7, 8
    ORRNE   r9, r9, r10, LSL #24   @src5, 6, 7, 7
    LDRNE   r01010101, =0x01010101    
    MULNE   r10, r10, r01010101
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r6, r8
    USUB8   r6, r6, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r7, r9
    USUB8   r7, r7, r9
    
    CMP     r3, #1
    MOVNE   r8, r10
    MOVNE   r9, r10
    BNE     NoRight
 @   ADD     p_Src, p_Src, #8
 @   LDM     p_Src, {r8, r9}    ;src8, 9, 10, 11, 12, 13, 14, 15  
    LDRD    r8, r9, [p_Src, #12]      
    ORR     r4, r10, r8, LSL #8
    MOV     r5, r8, LSR #24
    ORR     r5, r9, LSL #8    @src3 , 4, 5, 6  
    MOV     r10, r8, LSR #8   
    ORR     r10, r10, r9, LSL #24  @src1, 2, 3, 4
    MOV     r11, r9, LSR #8
    AND     r14, r9, #0xFF000000
    ORR     r11, r11, r14
 
    UHADD8  r10, r10, r4   @ (c + a) / 2
    UHSUB8  r10, r8, r10   @ (b - (c + a) / 2) /2
    USUB8   r8, r8, r10    @ b - (b - (c+a) /2 ) /2
    UHADD8  r11, r11, r5
    UHSUB8  r11, r9, r11
    USUB8   r9, r9, r11    
NoRight: 
@   r6 u0  u1  u2  u3
@   r7 u4  u5  u6  u7
@   r8 u8  u9  u10 u11   
@   r9 u12 u13 u14 u15 
    MOV  r2, r6, LSR #8  
    ORR  r2, r7, LSL #24  @u1 u2 u3 u4 ;b
    MOV  r3, r7, LSR #8
    ORR  r3, r8, LSL #24  @u5 u6 u7 u8 ;b
    MOV  r4, r8, LSR #8
    ORR  r4, r9, LSL #24  @u9 u10 u11 u12 ;b
    MOV  r10, r6, LSR #16
    ORR  r10, r7, LSL #16 @u2 u3 u4 u5 ;c
    MOV  r11, r7, LSR #16
    ORR  r11, r8, LSL #16 @u6, u7, u8, u9;c
    MOV  r12, r8, LSR #16
    ORR  r12, r9, LSL #16 @u10,u11,u12,u13;c
   
    UHADD8  r10, r10, r6   @ (c + a) / 2
    UHSUB8  r10, r2, r10   @ (b - (c + a) / 2) /2
    USUB8   r10, r2, r10    @ b - (b - (c+a) /2 ) /2
    UHADD8  r11, r11, r7
    UHSUB8  r11, r3, r11
    USUB8   r11, r3, r11         
    UHADD8  r12, r12, r8   @ (c + a) / 2
    UHSUB8  r12, r4, r12   @ (b - (c + a) / 2) /2
    USUB8   r12, r4, r12    @ b - (b - (c+a) /2 ) /2 
    
    UHSUB8  r6, r2, r6
    USUB8   r2, r2, r6   
    UHSUB8  r7, r3, r7
    USUB8   r3, r3, r7
    UHSUB8  r8, r4, r8
    USUB8   r4, r4, r8   
 
 @ r10 r11 r12 s0 - s11 s = (u0 + 2 * u1 + u2 + 2) /2
 @ r2  r3  r4  n_t0 - n_t11 t = (u0 + u1 + 1) /2    
    ADD    p_Src, p_Src, #4
    ADD    p_Src, src_stride
    STR    r3, [p_Src, #4]
    STR    r2, [p_Src], src_stride
    STR    r11, [p_Src, #4]
    STR    r10, [p_Src], src_stride
    
    MOV    r8, r3, LSR #8
    ORR    r8, r8, r4, LSL #24
    STR    r8, [p_Src, #4]
    MOV    r9, r2, LSR #8
    ORR    r9, r9, r3, LSL #24
    STR    r9, [p_Src], src_stride
    
    MOV    r8, r11, LSR #8
    ORR    r8, r8, r12, LSL #24
    STR    r8, [p_Src, #4]
    MOV    r9, r10, LSR #8
    ORR    r9, r9, r11, LSL #24
    STR    r9, [p_Src], src_stride 
    
    MOV    r8, r3, LSR #16
    ORR    r8, r8, r4, LSL #16
    STR    r8, [p_Src, #4]
    MOV    r9, r2, LSR #16
    ORR    r9, r9, r3, LSL #16
    STR    r9, [p_Src], src_stride    
 
    MOV    r8, r11, LSR #16
    ORR    r8, r8, r12, LSL #16
    STR    r8, [p_Src, #4]
    MOV    r9, r10, LSR #16
    ORR    r9, r9, r11, LSL #16
    STR    r9, [p_Src], src_stride   
 
    MOV    r8, r3, LSR #24
    ORR    r8, r8, r4, LSL #8
    STR    r8, [p_Src, #4]
    MOV    r9, r2, LSR #24
    ORR    r9, r9, r3, LSL #8
    STR    r9, [p_Src], src_stride    
 
    MOV    r8, r11, LSR #24
    ORR    r8, r8, r12, LSL #8
    STR    r8, [p_Src, #4]
    MOV    r9, r10, LSR #24
    ORR    r9, r9, r11, LSL #8
    STR    r9, [p_Src]         
    
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 
    

@extern  void Predict8x8DownLeft_ARMV6(VO_U8 *p_Src, VO_S32 stride, int topleft, int topright)
@r0 p_Src
@r1 stride
@r2 topleft
@r3 topright
Predict8x8DownLeft_ARMV6:
    STMFD   sp!, {r0-r11, lr}
 
    SUB     p_Src, src_stride
    SUB     p_Src, p_Src, #4
    LDM     p_Src, {r5, r6, r7, r14}    @src0, 1, 2, 3, 4, 5, 6, 7
    
    CMP     r2, #1
    MOVEQ   r4, r5, LSR #24  @p_Src-1, 0, 1, 2
    ANDNE   r4, r6, #0xFF    @src0 , 0, 1, 2
    ORR     r4, r6, LSL #8      
    MOV     r5, r6, LSR #24
    ORR     r5, r7, LSL #8    @src3 , 4, 5, 6  
    MOV     r8, r6, LSR #8   
    ORR     r8, r8, r7, LSL #24  @src1, 2, 3, 4
    MOV     r9, r7, LSR #8
    CMP     r3, #1
    MOV    r10, r7, LSR #24
    ORREQ   r9, r9, r14, LSL #24  @src5, 6, 7, 8
    ORRNE   r9, r9, r10, LSL #24   @src5, 6, 7, 7
    LDRNE   r01010101, =0x01010101    
    MULNE   r10, r10, r01010101
  
    UHADD8  r8, r8, r4
    UHSUB8  r8, r6, r8
    USUB8   r6, r6, r8  
    UHADD8  r9, r9, r5
    UHSUB8  r9, r7, r9
    USUB8   r7, r7, r9
    
    CMP     r3, #1
    MOVNE   r8, r10
    MOVNE   r9, r10
    BNE     NoDownLeftRight
 @   ADD     p_Src, p_Src, #8
 @   LDM     p_Src, {r8, r9}    ;src8, 9, 10, 11, 12, 13, 14, 15  
    LDRD    r8, r9, [p_Src, #12]      
    ORR     r4, r10, r8, LSL #8
    MOV     r5, r8, LSR #24
    ORR     r5, r9, LSL #8    @src3 , 4, 5, 6  
    MOV     r10, r8, LSR #8   
    ORR     r10, r10, r9, LSL #24  @src1, 2, 3, 4
    MOV     r11, r9, LSR #8
    AND     r14, r9, #0xFF000000
    ORR     r11, r11, r14
 
    UHADD8  r10, r10, r4   @ (c + a) / 2
    UHSUB8  r10, r8, r10   @ (b - (c + a) / 2) /2
    USUB8   r8, r8, r10    @ b - (b - (c+a) /2 ) /2
    UHADD8  r11, r11, r5
    UHSUB8  r11, r9, r11
    USUB8   r9, r9, r11    
NoDownLeftRight: 
@   r6 u0  u1  u2  u3
@   r7 u4  u5  u6  u7
@   r8 u8  u9  u10 u11   
@   r9 u12 u13 u14 u15 
    MOV  r2, r6, LSR #8  
    ORR  r2, r7, LSL #24  @u1 u2 u3 u4 ;b
    MOV  r3, r7, LSR #8
    ORR  r3, r8, LSL #24  @u5 u6 u7 u8 ;b
    MOV  r4, r8, LSR #8
    ORR  r4, r9, LSL #24  @u9 u10 u11 u12 ;b
    AND  r5, r9, #0xFF000000
    ORR  r5, r9, LSR #8
    
    MOV  r10, r6, LSR #16
    ORR  r10, r7, LSL #16 @u2 u3 u4 u5 ;c
    MOV  r11, r7, LSR #16
    ORR  r11, r8, LSL #16 @u6, u7, u8, u9;c
    MOV  r12, r8, LSR #16
    ORR  r12, r9, LSL #16 @u10,u11,u12,u13;c
    AND  r14, r9, #0xFF000000
    ORR  r14, r14, r14, LSR #8
    ORR  r14, r14, r9, LSR #16
   
    UHADD8  r10, r10, r6   @ (c + a) / 2
    UHSUB8  r10, r2, r10   @ (b - (c + a) / 2) /2
    USUB8   r10, r2, r10    @ b - (b - (c+a) /2 ) /2
    UHADD8  r11, r11, r7
    UHSUB8  r11, r3, r11
    USUB8   r11, r3, r11         
    UHADD8  r12, r12, r8   @ (c + a) / 2
    UHSUB8  r12, r4, r12   @ (b - (c + a) / 2) /2
    USUB8   r12, r4, r12    @ b - (b - (c+a) /2 ) /2 
    UHADD8  r14, r14, r9   @ (c + a) / 2
    UHSUB8  r14, r5, r14   @ (b - (c + a) / 2) /2
    USUB8   r14, r5, r14    @ b - (b - (c+a) /2 ) /2 
    
    
@    UHSUB8  r6, r2, r6
@    USUB8   r2, r2, r6   
@    UHSUB8  r7, r3, r7
@    USUB8   r3, r3, r7
@    UHSUB8  r8, r4, r8
@    USUB8   r4, r4, r8   
 
 @ r10 r11 r12 r14 s0 - s15 s = (u0 + 2 * u1 + u2 + 2) /2   
    ADD    p_Src, p_Src, #4
    ADD    p_Src, src_stride
@ col 0
    STR    r11, [p_Src, #4]
    STR    r10, [p_Src], src_stride
@ col 1
    MOV    r8, r11, LSR #8
    ORR    r8, r8, r12, LSL #24
    STR    r8, [p_Src, #4]
    MOV    r9, r10, LSR #8
    ORR    r9, r9, r11, LSL #24
    STR    r9, [p_Src], src_stride 
@ col 2
    MOV    r8, r11, LSR #16
    ORR    r8, r8, r12, LSL #16
    STR    r8, [p_Src, #4]
    MOV    r9, r10, LSR #16
    ORR    r9, r9, r11, LSL #16
    STR    r9, [p_Src], src_stride   
@ col 3
    MOV    r8, r11, LSR #24
    ORR    r8, r8, r12, LSL #8
    STR    r8, [p_Src, #4]
    MOV    r9, r10, LSR #24
    ORR    r9, r9, r11, LSL #8
    STR    r9, [p_Src], src_stride         
@ col 4
    STR    r12, [p_Src, #4]
    STR    r11, [p_Src], src_stride
@ col 5
    MOV    r8, r12, LSR #8
    ORR    r8, r8, r14, LSL #24
    STR    r8, [p_Src, #4]
    MOV    r9, r11, LSR #8
    ORR    r9, r9, r12, LSL #24
    STR    r9, [p_Src], src_stride 
@ col 6
    MOV    r8, r12, LSR #16
    ORR    r8, r8, r14, LSL #16
    STR    r8, [p_Src, #4]
    MOV    r9, r11, LSR #16
    ORR    r9, r9, r12, LSL #16
    STR    r9, [p_Src], src_stride   
@ col 7
    MOV    r8, r12, LSR #24
    ORR    r8, r8, r14, LSL #8
    STR    r8, [p_Src, #4]
    MOV    r9, r11, LSR #24
    ORR    r9, r9, r12, LSL #8
    STR    r9, [p_Src]         
   
    
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 


@extern	void PredIntraLuma8x8DC_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_U8 *p_Dst, VO_S32 dst_stride, VO_U32 availability);

PredIntraLuma8x8DC_ARMV6:
@r0  p_Src
@r1	 src_stride
@r2  p_Dst		
@r3  dst_stride
    STMFD   sp!, {r0-r11, lr}

    @// M_STALL ARM1136JS=2

    LDR  availability, [sp, #DC_MODE]
    
    MOV  dstStep, dst_stride
    MOV  pDst, p_Dst
    MOV  leftStep, src_stride
    SUB  pSrcAbove, p_Src, leftStep
    SUB  pSrcLeft, p_Src, #1
    SUB  pSrcAboveLeft, pSrcAbove, #1  
        
    MOV      n_cnt, #0                           @// n_cnt = 0
    TST      availability, #1                @// if(availability & #OMX_VC_UPPER)
    BEQ      TST_LEFT                            @// Jump to Left if not upper
    LDM      pSrcAbove,{n_tVal8,n_tVal9,n_tVal10,n_tVal11}@// tVal 8 to 11 = pSrcAbove[0 to 15]
    ADD      n_cnt, n_cnt, #1                    @// if upper inc n_cnt by 1
        
    @// M_STALL ARM1136JS=2
        
    UXTB16   n_tVal2, n_tVal8                        @// pSrcAbove[0, 2]
    UXTB16   n_tVal6, n_tVal9                        @// pSrcAbove[4, 6]
    UADD16   n_tVal2, n_tVal2, n_tVal6                 @// pSrcAbove[0, 2] + pSrcAbove[4, 6]
    UXTB16   n_tVal8, n_tVal8, ROR #8                @// pSrcAbove[1, 3]
    UXTB16   n_tVal9, n_tVal9, ROR #8                @// pSrcAbove[5, 7]
    UADD16   n_tVal8, n_tVal8, n_tVal9                 @// pSrcAbove[1, 3] + pSrcAbove[5, 7]
    UADD16   n_tVal2, n_tVal2, n_tVal8                 @// sum(pSrcAbove[0] to pSrcAbove[7])
        
    UXTB16   n_tVal8, n_tVal10                       @// pSrcAbove[8, 10]
    UXTB16   n_tVal9, n_tVal11                       @// pSrcAbove[12, 14]
    UADD16   n_tVal8, n_tVal8, n_tVal9                 @// pSrcAbove[8, 10] + pSrcAbove[12, 14]
    UXTB16   n_tVal10, n_tVal10, ROR #8              @// pSrcAbove[9, 11]
    UXTB16   n_tVal11, n_tVal11, ROR #8              @// pSrcAbove[13, 15]
    UADD16   n_tVal10, n_tVal10, n_tVal11              @// pSrcAbove[9, 11] + pSrcAbove[13, 15]
    UADD16   n_tVal8, n_tVal8, n_tVal10                @// sum(pSrcAbove[8] to pSrcAbove[15])
        
    UADD16   n_tVal2, n_tVal2, n_tVal8                 @// sum(pSrcAbove[0] to pSrcAbove[15])
        
    @// M_STALL ARM1136JS=1
        
    ADD      n_tVal2, n_tVal2, n_tVal2, LSR #16        @// sum(pSrcAbove[0] to pSrcAbove[15])
        
    @// M_STALL ARM1136JS=1
        
    UXTH     sum, n_tVal2                          @// Extract the lower half for result
        
TST_LEFT:        
    TST      availability, #2
    BEQ      TST_COUNT
    ADD      leftStepx2, leftStep,leftStep       @// leftStepx2 = 2 * leftStep
    ADD      pSrcLeft2, pSrcLeft, leftStep       @// pSrcLeft2 = pSrcLeft + leftStep
        
    LDRB   n_tVal8, [pSrcLeft],  +leftStepx2     @// n_tVal8 = pSrcLeft[0]
    LDRB   n_tVal9, [pSrcLeft2], +leftStepx2     @// n_tVal9 = pSrcLeft[1]
    LDRB   n_tVal10, [pSrcLeft], +leftStepx2     @// n_tVal10= pSrcLeft[2]
    LDRB   n_tVal11, [pSrcLeft2],+leftStepx2     @// n_tVal11= pSrcLeft[3]
    ADD      n_tVal7, n_tVal8, n_tVal9                 @// n_tVal7 = n_tVal8 + n_tVal9
    ADD      n_cnt, n_cnt, #1                    @// Inc Counter if Left is available
    ADD      n_tVal6, n_tVal10, n_tVal11               @// n_tVal6 = n_tVal10 + n_tVal11
        
    LDRB   n_tVal8, [pSrcLeft],  +leftStepx2     @// n_tVal8 = pSrcLeft[0]
    LDRB   n_tVal9, [pSrcLeft2], +leftStepx2     @// n_tVal9 = pSrcLeft[1]
    LDRB   n_tVal10, [pSrcLeft], +leftStepx2     @// n_tVal10= pSrcLeft[2]
    LDRB   n_tVal11, [pSrcLeft2],+leftStepx2     @// n_tVal11= pSrcLeft[3]
    ADD      sum, n_tVal7, n_tVal6                   @// sum = n_tVal8 + n_tVal10
    ADD      n_tVal8, n_tVal8, n_tVal9                 @// n_tVal8 = n_tVal8 + n_tVal9
    ADD      n_tVal10, n_tVal10, n_tVal11              @// n_tVal10= n_tVal10 + n_tVal11
    ADD      n_tVal7, n_tVal8, n_tVal10                @// n_tVal7 = n_tVal8 + n_tVal10
        
        
    LDRB   n_tVal8, [pSrcLeft],  +leftStepx2     @// n_tVal8 = pSrcLeft[0]
    LDRB   n_tVal9, [pSrcLeft2], +leftStepx2     @// n_tVal9 = pSrcLeft[1]
    LDRB   n_tVal10, [pSrcLeft], +leftStepx2     @// n_tVal10= pSrcLeft[2]
    LDRB   n_tVal11, [pSrcLeft2],+leftStepx2     @// n_tVal11= pSrcLeft[3]
    ADD      sum, sum, n_tVal7                     @// sum = sum + n_tVal7
    ADD      n_tVal8, n_tVal8, n_tVal9                 @// n_tVal8 = n_tVal8 + n_tVal9
    ADD      n_tVal10, n_tVal10, n_tVal11              @// n_tVal10= n_tVal10 + n_tVal11
    ADD      n_tVal7, n_tVal8, n_tVal10                @// n_tVal7 = n_tVal8 + n_tVal10
        
        
    LDRB   n_tVal8, [pSrcLeft],  +leftStepx2     @// n_tVal8 = pSrcLeft[0]
    LDRB   n_tVal9, [pSrcLeft2], +leftStepx2     @// n_tVal9 = pSrcLeft[1]
    LDRB   n_tVal10, [pSrcLeft], +leftStepx2     @// n_tVal10= pSrcLeft[2]
    LDRB   n_tVal11, [pSrcLeft2],+leftStepx2     @// n_tVal11= pSrcLeft[3]
    ADD      sum, sum, n_tVal7                     @// sum = sum + n_tVal7
    ADD      n_tVal8, n_tVal8, n_tVal9                 @// n_tVal8 = n_tVal8 + n_tVal9
    ADD      n_tVal10, n_tVal10, n_tVal11              @// n_tVal10= n_tVal10 + n_tVal11
    ADD      n_tVal7, n_tVal8, n_tVal10                @// n_tVal7 = n_tVal8 + n_tVal10
    ADD      sum, sum, n_tVal7                     @// sum = sum + n_tVal7

TST_COUNT:        
    CMP      n_cnt, #0                           @// if(n_cnt == 0)
    MOVEQ    sum, #128                           @// sum = 128 if(n_cnt == 0)
    BEQ      TST_COUNT0                          @// if(n_cnt == 0)
    CMP      n_cnt, #1                           @// if(n_cnt == 1)
    ADDEQ    sum, sum, #8                        @// sum += 8 if(n_cnt == 1)
    ADDNE    sum, sum, n_tVal2                     @// sum = sumleft + sumupper
    ADDNE    sum, sum, #16                       @// sum += 16 if(n_cnt == 2)
        
    @// M_STALL ARM1136JS=1
        
    UXTH     sum, sum                            @// sum only byte rest cleared
        
    @// M_STALL ARM1136JS=1
       
    LSREQ    sum, sum, #4                        @// sum >> 4 if(n_cnt == 1)
        
    @// M_STALL ARM1136JS=1
        
    LSRNE    sum, sum, #5                        @// sum >> 5 if(n_cnt == 2)

TST_COUNT0:
        
    @// M_STALL ARM1136JS=1
        
    ORR      sum, sum, sum, LSL #8               @// sum replicated in two halfword
        
    @// M_STALL ARM1136JS=1
        
    ORR      n_tVal6, sum, sum, LSL #16            @// sum  replicated in all bytes
    CPY      n_tVal7, n_tVal6                        @// n_tVal1 = tVal0
    CPY      n_tVal8, n_tVal6                        @// n_tVal2 = tVal0
    CPY      n_tVal9, n_tVal6                        @// n_tVal3 = tVal0
    ADD      dstStepx2, dstStep, dstStep         @// double dstStep
    ADD      pDst2, pDst, dstStep                @// pDst2- pDst advanced by dstStep
    MOV      y, #BLK_SIZE                        @// Outer Loop Count
        
LOOP_DC:        
    STM      pDst, {n_tVal6,n_tVal7,n_tVal8,n_tVal9}     @// pDst[0 to 15] = tVal 6 to 9
    SUBS     y, y, #2                            @// y--
    ADD      pDst, pDst, dstStepx2               @// pDst advanced by dstStep
    STM      pDst2, {n_tVal6,n_tVal7,n_tVal8,n_tVal9}    @// pDst2[16 to 31] = tVal 6 to 9
    ADD      pDst2, pDst2, dstStepx2             @// pDst advanced by dstStep
    BNE      LOOP_DC                             @// Loop for 8 times       	
	
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 
	
 
    @.end


