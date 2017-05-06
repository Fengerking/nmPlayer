@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************  
 #include "../../../defineID.h"
	.section	  .text
 

	.global PredIntraLuma16x16P_ARMV6
	.global PredIntraLuma16x16DC_ARMV6
	.global PredIntraLuma16x16V_ARMV6
	.global PredIntraLuma16x16H_ARMV6

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


@extern	void PredIntraLuma16x16H_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_U8 *p_Dst, VO_S32 dst_stride);  
PredIntraLuma16x16H_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    
    LDR    r01010101, =0x01010101
    MOV     r14, #4
    SUB     p_Src, p_Src, #1
    SUB     p_Dst, dst_stride
 
Loop_H: 
    LDRB    n_x0, [p_Src], src_stride
    MUL     n_x0, n_x0, r01010101
    LDRB    n_x1, [p_Src], src_stride
    MUL     n_x1, n_x1, r01010101
    STR     n_x0, [p_Dst, dst_stride]!
    STR     n_x0, [p_Dst, #4]
    STR     n_x0, [p_Dst, #8]
    STR     n_x0, [p_Dst, #12]
    LDRB    n_x2, [p_Src], src_stride
    MUL     n_x2, n_x2, r01010101
    STR     n_x1, [p_Dst, dst_stride]!
    STR     n_x1, [p_Dst, #4]
    STR     n_x1, [p_Dst, #8]
    STR     n_x1, [p_Dst, #12]
    LDRB    n_x3, [p_Src], src_stride
    MUL     n_x3, n_x3, r01010101
    STR     n_x2, [p_Dst, dst_stride]! 
    STR     n_x2, [p_Dst, #4]
    STR     n_x2, [p_Dst, #8]
    STR     n_x2, [p_Dst, #12]
    STR     n_x3, [p_Dst, dst_stride]!
    STR     n_x3, [p_Dst, #4]
    STR     n_x3, [p_Dst, #8]
    STR     n_x3, [p_Dst, #12]
    SUBS    r14, r14, #1
    BNE Loop_H    
  
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 
 
@extern	void PredIntraLuma16x16V_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_U8 *p_Dst, VO_S32 dst_stride);
PredIntraLuma16x16V_ARMV6: 
    STMFD   sp!, {r0-r11, lr}
    
    MOV     y, #16
    SUB     p_Src, src_stride
    LDM     p_Src, {n_x0, n_x1, n_x2, n_x3}
    ADD     dst_stride2, dst_stride, dst_stride
    ADD     dst2, p_Dst, dst_stride
 
Loop_V: 
    STM     p_Dst, {n_x0, n_x1, n_x2, n_x3}
    SUBS    y, #2
    ADD     p_Dst, p_Dst,  dst_stride2
    STM     dst2,{n_x0, n_x1, n_x2, n_x3}    
    ADD     dst2, dst2, dst_stride2
    BNE     Loop_V
    
    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc}    


@extern	void PredIntraLuma16x16DC_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_U8 *p_Dst, VO_S32 dst_stride, VO_U32 availability);

PredIntraLuma16x16DC_ARMV6:
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
	

@extern	void PredIntraLuma16x16P_ARMV6( VO_U8 *p_Src , VO_S32 src_stride, VO_U8 *p_Dst, VO_S32 dst_stride);
	
PredIntraLuma16x16P_ARMV6:
@r0  p_Src
@r1	 src_stride
@r2  p_Dst		
@r3  dst_stride
    STMFD   sp!, {r0-r11, lr}
	
    MOV  dstStep, dst_stride
    MOV  pDst, p_Dst
    MOV  leftStep, src_stride
    SUB  pSrcAbove, p_Src, leftStep
    SUB  pSrcLeft, p_Src, #1
    SUB  pSrcAboveLeft, pSrcAbove, #1

    @// M_STALL ARM1136JS=3
    RSB      n_tVal14, leftStep, leftStep, LSL #4  @// n_tVal14 = 15*leftStep
        
    @// M_STALL ARM1136JS=2
    LDRB     n_tVal10, [pSrcLeft,  n_tVal14]         @// n_tVal10 = pSrcLeft[15*leftStep]
    LDRB     n_tVal11, [pSrcAboveLeft]             @// n_tVal11 = pSrcAboveLeft[0]
    LDRB     n_tVal12, [pSrcAbove, #15]

    ADD      n_tVal2,  n_tVal12,  n_tVal10             @// n_tVal2  = pSrcAbove[15] + pSrcLeft[15*leftStep]
    SUB      n_tVal10, n_tVal10,  n_tVal11             @// n_tVal10 = V0 = pSrcLeft[15*leftStep] - pSrcAboveLeft[0]
    SUB      n_tVal11, n_tVal12,  n_tVal11             @// n_tVal11 = H0 = pSrcAbove[15] - pSrcAboveLeft[0]
    MOV      n_tVal2,  n_tVal2,   LSL #4             @// n_tVal2  = a = 16 * (pSrcAbove[15] + pSrcLeft[15*leftStep])

    MOV     n_tVal11, n_tVal11, LSL #3              @// 8*[15]-[-1]
    LDRB    n_tVal6, [pSrcAbove, #0]
    LDRB    n_tVal7, [pSrcAbove, #14]
    SUB     n_tVal8, n_tVal7, n_tVal6
    RSB     n_tVal8, n_tVal8, n_tVal8, LSL #3         @// 7*[14]-[0]
    ADD     n_tVal11, n_tVal11, n_tVal8
    LDRB    n_tVal6, [pSrcAbove, #1]
    LDRB    n_tVal7, [pSrcAbove, #13]
    SUB     n_tVal8, n_tVal7, n_tVal6
    ADD     n_tVal8, n_tVal8, n_tVal8
    ADD     n_tVal8, n_tVal8, n_tVal8, LSL #1         @// 6*[13]-[1]
    ADD     n_tVal11, n_tVal11, n_tVal8
    LDRB    n_tVal6, [pSrcAbove, #2]
    LDRB    n_tVal7, [pSrcAbove, #12]
    SUB     n_tVal8, n_tVal7, n_tVal6
    ADD     n_tVal8, n_tVal8, n_tVal8, LSL #2         @// 5*[12]-[2]
    ADD     n_tVal11, n_tVal11, n_tVal8
    LDRB    n_tVal6, [pSrcAbove, #3]
    LDRB    n_tVal7, [pSrcAbove, #11]
    SUB     n_tVal8, n_tVal7, n_tVal6
    ADD     n_tVal11, n_tVal11, n_tVal8, LSL #2       @// + 4*[11]-[3]
    LDRB    n_tVal6, [pSrcAbove, #4]
    LDRB    n_tVal7, [pSrcAbove, #10]
    SUB     n_tVal8, n_tVal7, n_tVal6
    ADD     n_tVal8, n_tVal8, n_tVal8, LSL #1         @// 3*[10]-[4]
    ADD     n_tVal11, n_tVal11, n_tVal8
    LDRB    n_tVal6, [pSrcAbove, #5]
    LDRB    n_tVal7, [pSrcAbove, #9]
    SUB     n_tVal8, n_tVal7, n_tVal6
    ADD     n_tVal11, n_tVal11, n_tVal8, LSL #1       @// + 2*[9]-[5]
    LDRB    n_tVal6, [pSrcAbove, #6]
    LDRB    n_tVal7, [pSrcAbove, #8]
    SUB     n_tVal8, n_tVal7, n_tVal6                 @// 1*[8]-[6]
    ADD     n_tVal7, n_tVal11, n_tVal8

    ADD      n_tVal2,  n_tVal2,   #16                @// n_tVal2  = a + 16
    MOV      n_tVal1,  pSrcLeft                    @// n_tVal4  = pSrcLeft
    SUB      n_tVal9,  n_tVal14,   leftStep          @// n_tVal9  = 14*leftStep
    ADD      n_tVal9,  pSrcLeft, n_tVal9             @// n_tVal9  = pSrcLeft + 14*leftStep
        
    LDRB   n_tVal8,  [n_tVal9], -leftStep          @// n_tVal8  = pSrcLeft[14*leftStep]
    LDRB   n_tVal11, [n_tVal1], +leftStep          @// n_tVal11 = pSrcLeft[0]
    ADD      n_tVal7,  n_tVal7,  n_tVal7,  LSL #2      @// n_tVal7  = 5 * H
    ADD      n_tVal7,  n_tVal7,  #32                 @// n_tVal7  = 5 * H + 32
    SUB      n_tVal8,  n_tVal8,  n_tVal11              @// n_tVal8  = pSrcLeft[14*leftStep] - pSrcLeft[0]
    ASR      n_tVal12, n_tVal7,  #6                  @// n_tVal12 = b = (5 * H + 32) >> 6
       
    RSB      n_tVal8,  n_tVal8,  n_tVal8,  LSL #3      @// n_tVal8  = V1 = 7* (pSrcLeft[14*leftStep]-pSrcLeft[0])
    ADD      n_tVal6,  n_tVal8,  n_tVal10, LSL #3      @// n_tVal6  = V = V0 +V1
    LDRB   n_tVal8,  [n_tVal9], -leftStep          @// n_tVal8  = pSrcLeft[13*leftStep]
    LDRB   n_tVal10, [n_tVal1], +leftStep          @// n_tVal10 = pSrcLeft[leftStep]
    RSB      n_tVal7,  n_tVal12,  n_tVal12,  LSL #3    @// n_tVal7  = 7*b
    SUB      n_tVal2,  n_tVal2,   n_tVal7              @// n_tVal2  = a + 16 - 7*b
    SUB      n_tVal7,  n_tVal8,   n_tVal10             @// n_tVal7  = pSrcLeft[13*leftStep] - pSrcLeft[leftStep]
    LDRB   n_tVal8,  [n_tVal9], -leftStep          @// n_tVal8  = pSrcLeft[12*lS]
    ADD      n_tVal7,  n_tVal7,   n_tVal7              @// n_tVal7  = 2 * (pSrcLeft[13*leftStep] - pSrcLeft[leftStep])
    LDRB   n_tVal10, [n_tVal1], +leftStep          @// n_tVal10 = pSrcLeft[2*leftStep]        
    ADD      n_tVal7,  n_tVal7,   n_tVal7,  LSL #1     @// n_tVal7  = 6 * (pSrcLeft[13*leftStep] - pSrcLeft[leftStep])
    ADD      n_tVal6,  n_tVal6,   n_tVal7              @// n_tVal6  = V = V + V2
    SUB      n_tVal7,  n_tVal8,   n_tVal10             @// n_tVal7  = pSrcLeft[12*leftStep] - pSrcLeft[2*leftStep]
    LDRB   n_tVal8,  [n_tVal9], -leftStep          @// n_tVal8  = pSrcLeft[11*leftStep]
    LDRB   n_tVal10, [n_tVal1], +leftStep          @// n_tVal10 = pSrcLeft[3*leftStep]
    ADD      n_tVal7,  n_tVal7,   n_tVal7,  LSL #2     @// n_tVal7  = 5 * (pSrcLeft[12*leftStep] - pSrcLeft[2*leftStep])
    ADD      n_tVal6,  n_tVal6,   n_tVal7              @// n_tVal6  = V = V + V3
    SUB      n_tVal7,  n_tVal8,   n_tVal10             @// n_tVal7  = pSrcLeft[11*leftStep] - pSrcLeft[3*leftStep]
    LDRB   n_tVal8,  [n_tVal9], -leftStep          @// n_tVal8  = pSrcLeft[10*leftStep]
    LDRB   n_tVal10, [n_tVal1], +leftStep          @// n_tVal10 = pSrcLeft[4*leftStep]
    ADD      n_tVal6,  n_tVal6,   n_tVal7,  LSL #2     @// n_tVal6  = V = V + V4
    SUB      dstStep, dstStep, #16               @// n_tVal5  = dstStep - 16
    SUB      n_tVal7,  n_tVal8,   n_tVal10             @// n_tVal7  = pSrcLeft[10*leftStep] - pSrcLeft[4*leftStep]
    LDRB   n_tVal8,  [n_tVal9], -leftStep          @// n_tVal8  = pSrcLeft[9*leftStep]
    LDRB   n_tVal10, [n_tVal1], +leftStep          @// n_tVal10 = pSrcLeft[5*leftStep]
    ADD      n_tVal7,  n_tVal7,   n_tVal7,  LSL #1     @// n_tVal7  = 3 * (pSrcLeft[10*leftStep] - pSrcLeft[4*leftStep])
    ADD      n_tVal6,  n_tVal6,   n_tVal7              @// n_tVal6  = V = V + V5
    SUB      n_tVal7,  n_tVal8,   n_tVal10             @// n_tVal7  = pSrcLeft[9*leftStep] - pSrcLeft[5*leftStep]
    LDRB   n_tVal8,  [n_tVal9], -leftStep          @// n_tVal8  = pSrcLeft[8*leftStep]
    LDRB   n_tVal10, [n_tVal1], +leftStep          @// n_tVal10 = pSrcLeft[6*leftStep]
    ADD      n_tVal6,  n_tVal6,   n_tVal7,  LSL #1     @// n_tVal6  = V = V + V6
        
    @// M_STALL ARM1136JS=1
    SUB      n_tVal7,  n_tVal8,   n_tVal10             @// n_tVal7  = pSrcLeft[8*leftStep] - pSrcLeft[6*leftStep]
    ADD      n_tVal6,  n_tVal6,   n_tVal7              @// n_tVal6  = V = V + V7
        
    @// M_STALL ARM1136JS=1
    ADD      n_tVal6,  n_tVal6,   n_tVal6,  LSL #2     @// n_tVal6  = 5*V
    ADD      n_tVal6,  n_tVal6,   #32                @// n_tVal6  = 5*V + 32
        
    @// M_STALL ARM1136JS=1
    ASR      n_tVal14, n_tVal6,   #6                 @// n_tVal14 = c = (5*V + 32)>>6
        
    @// M_STALL ARM1136JS=1
    RSB      n_tVal6,  n_tVal14,  n_tVal14, LSL #3     @// n_tVal6  = 7*c
    UXTH     n_tVal14, n_tVal14                      @// n_tVal14 = Cleared the upper half word
    ADD      n_tVal10, n_tVal12,  n_tVal12             @// n_tVal10 = 2*b
    ORR      n_tVal14, n_tVal14,  n_tVal14, LSL #16    @// n_tVal14 = {c  ,  c}
    SUB      n_tVal6,  n_tVal2,   n_tVal6              @// n_tVal6  = d = a - 7*b - 7*c + 16
    ADD      n_tVal1,  n_tVal6,   n_tVal10             @// n_tVal1  = pp2 = d + 2*b
    ADD      n_tVal10, n_tVal10,  n_tVal12             @// n_tVal10 =3*b
@    ORR      tVal0,  n_tVal6,   n_tVal1,  LSL #16    ;// tval0  = n_p2p0   = pack {p2, p0}
    PKHBT     tVal0,  n_tVal6,   n_tVal1,  LSL #16 
    UXTH     n_tVal12, n_tVal12                      @// n_tVal12 = Cleared the upper half word
    UXTH     n_tVal10, n_tVal10                      @// n_tVal12 = Cleared the upper half word
    ORR      n_tVal12, n_tVal12,  n_tVal12, LSL #16    @// n_tVal12 = {b  ,  b}
    ORR      n_tVal10, n_tVal10,  n_tVal10, LSL #16    @// n_tVal10 = {3b , 3b}
    SADD16   n_tVal1,  tVal0,   n_tVal12             @// n_tVal1  = n_p3p1   = n_p2p0   + {b,b}
    SADD16   n_tVal2,  n_tVal1,   n_tVal10             @// n_tVal2  = n_p6p4   = n_p3p1   + {3b,3b}
    SADD16   n_tVal4,  n_tVal2,   n_tVal12             @// n_tVal4  = n_p7p5   = n_p6p4   + {b,b}
    SADD16   n_tVal6,  n_tVal4,   n_tVal10             @// n_tVal6  = n_p10p8  = n_p7p5   + {3b,3b}
    SADD16   n_tVal7,  n_tVal6,   n_tVal12             @// n_tVal7  = n_p11p9  = n_p10p8  + {b,b}
    SADD16   n_tVal8,  n_tVal7,   n_tVal10             @// n_tVal8  = n_p14p12 = n_p11p9  + {3b,3b}
    SADD16   n_tVal9,  n_tVal8,   n_tVal12             @// n_tVal9  = n_p15p13 = n_p14p12 + {b,b}
    LDR      r0x00FF00FF,     =MASK_CONST        @// r0x00FF00FF = 0x00FF00FF
 @   MOV      dstStep, leftStep
        
LOOP_PLANE:        

    USAT16   n_tmp2, #13, n_p3p1
    USAT16   n_tmp1, #13, n_p2p0
    SADD16   n_p3p1,   n_p3p1,   c                    
    SADD16   n_p2p0,   n_p2p0,   c                    
    AND      n_tmp2, r0x00FF00FF, n_tmp2, ASR #5
    AND      n_tmp1, r0x00FF00FF, n_tmp1, ASR #5
    ORR      n_tmp1, n_tmp1, n_tmp2, LSL #8
    STR      n_tmp1, [pDst], #4
        
    USAT16   n_tmp2, #13, n_p7p5
    USAT16   n_tmp1, #13, n_p6p4
    SADD16   n_p7p5,   n_p7p5,   c                    
    SADD16   n_p6p4,   n_p6p4,   c                    
    AND      n_tmp2, r0x00FF00FF, n_tmp2, ASR #5
    AND      n_tmp1, r0x00FF00FF, n_tmp1, ASR #5
    ORR      n_tmp1, n_tmp1, n_tmp2, LSL #8
    STR      n_tmp1, [pDst], #4
        
    USAT16   n_tmp2, #13, n_p11p9
    USAT16   n_tmp1, #13, n_p10p8
    SADD16   n_p11p9,  n_p11p9,  c                    
    SADD16   n_p10p8,  n_p10p8,  c                    
    AND      n_tmp2, r0x00FF00FF, n_tmp2, ASR #5
    AND      n_tmp1, r0x00FF00FF, n_tmp1, ASR #5
    ORR      n_tmp1, n_tmp1, n_tmp2, LSL #8
    STR      n_tmp1, [pDst], #4
        
    USAT16   n_tmp2, #13, n_p15p13
    USAT16   n_tmp1, #13, n_p14p12
    SADD16   n_p15p13, n_p15p13, c                    
    SADD16   n_p14p12, n_p14p12, c                    
    AND      n_tmp2, r0x00FF00FF, n_tmp2, ASR #5
    AND      n_tmp1, r0x00FF00FF, n_tmp1, ASR #5
    ORR      n_tmp1, n_tmp1, n_tmp2, LSL #8
    STR      n_tmp1, [pDst], #4
        
    ADDS     r0x00FF00FF, r0x00FF00FF, #1<<28     @// Loop counter value in top 4 bits
        
    ADD      pDst, pDst, dstStep                   
        
    BCC      LOOP_PLANE   	
	

    ADD     sp,sp,#0x10
    LDMFD   sp!, {r4-r11, pc} 
 
    @.end

