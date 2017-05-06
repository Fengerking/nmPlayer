@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************
 #include "../../../defineID.h"
    .section	  .text


    .global  get_luma_12_32_ARMV6
    .global  add_luma_12_32_ARMV6
	.align 8	

@// Declare input registers
 pSrc            .req r0
 srcStep         .req r1
 pDst            .req r2
 dstStep         .req r3

@// Declare inner loop registers
 n_ValA            .req r5
 n_deltaX          .req r5
 ValA0           .req r4
 n_deltaY          .req r4
 ValA1           .req r5
 ValAF0          .req r4
 ValAF1          .req r5

 n_ValB            .req r11

 n_ValC            .req r5
 n_ValC0           .req r4
 n_ValC1           .req r5
 n_ValCD0          .req r12
 n_ValCD1          .req r14
 ValCF0          .req r4
 ValCF1          .req r5

 n_ValD            .req r10

 n_ValE            .req r7
 n_ValE0           .req r6
 ValE1           .req r7
 ValEB0          .req r10
 ValEB1          .req r11
 ValED0          .req r6
 ValED1          .req r7

 n_ValF            .req r10

 n_ValG            .req r14
 ValG0           .req r12
 ValG1           .req r14
 ValGB0          .req r12
 ValGB1          .req r14

 voAcc0            .req r4
 n_voACC1            .req r5
 n_voACC2            .req r6
 n_voACC3            .req r7

 Temp            .req r7
 Step            .req r6

 pInterBuf       .req r8
 Counter         .req r8
 Count           .req r8
 r0x00ff00ff     .req r9                                        @// [0 255 0 255] where 255 is offset
 n_partW           .req r9
 r0x0001fc00     .req r11                                       @// [0 (16*255 - 16) 0 (16*255 - 16)]
 n_partH           .req r10
    
@// Declare inner loop registers
 ValCA           .req r8
 ValDB           .req r9
 ValGE           .req r10
 ValHF           .req r11
 r0x00140001     .req r12
 r0x0014fffb     .req r14

 r0x00000200     .req r12
 r0x000000ff     .req r12

 ppSrcArgs       =    0x78
 ppSrcStepArgs   =    0x7C
 ppDstArgs       =    0x80
 ppDstStepArgs   =    0x84
 pTempResult1    =    0x88
 pTempResult2    =    0x90
 ppSrc           =    0x98
 pSrcStep        =    0x9C
 ppDst           =    0xA0
 pDstStep        =    0xA4
 pCounter        =    0xA8
 pCount          =    0xAC
 ppDstTemp       =    0xB4
 pPartWidth      =    0xE8
 pPartHeight     =    0xEC
 pDeltaX         =    0xF0
 pDeltaY         =    0xF4
 pHorOffset      =    0xF8

@0x00 - 0x77 InterBuf
@0x78    M_ALLOC8 ppSrcArgs, 8
@0x80    M_ALLOC8 ppDstArgs, 8
@0x88    M_ALLOC8 pTempResult1, 8
@0x90    M_ALLOC8 pTempResult2, 8
@0xA0    M_ALLOC4 ppSrc, 4
@0xA4    M_ALLOC4 pSrcStep, 4
@0xA8    M_ALLOC4 ppDst, 4
@0xAC    M_ALLOC4 pDstStep, 4
@0xB0    M_ALLOC4 pCounter, 4
@0xB4    M_ALLOC4 pCount,4

@extern void get_luma_12_32_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride,VO_U32 n_partW, VO_U32 n_partH, VO_S32 n_deltaX, VO_S32 n_deltaY, VO_U32 horOffset)	
get_luma_12_32_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    SUB     sp, sp, #0xB4
    
    LDR     n_partW, [sp,#pPartWidth]       @// partWidth
    LDR     n_partH, [sp,#pPartHeight]       @// partHeight
    LDR     n_deltaX ,[sp,#pDeltaX]         @// n_deltaX
    LDR     n_deltaY ,[sp,#pDeltaY]         @// n_deltaY
    MLA     Temp, srcStep, n_deltaY, n_deltaX   @// n_deltaY*n_w+n_deltaX
    ADD     pSrc, pSrc, Temp          @// p_Src += n_deltaY*n_w+n_deltaX


    @// pack values to n_cnt register
    @// [31:28] loop_x (partWidth-1)
    @// [27:24] loop_y (partHeight-1)
    @// [23:20] partWidth-1
    @// [19:16] partHeight-1
    @// [15:00] n_w
    MOV     Count, srcStep
    SUB     n_partW, n_partW, #1@
    SUB     n_partH, n_partH, #1@
    ADD     Temp, n_partH, n_partW, LSL #4
    ADD     Count, Count, Temp, LSL #16    

    AND     Temp, Count, #0x000F0000    @// partHeight-1
    ADD     Count, Count, Temp, LSL #8
    AND     Temp, Count, #0x00F00000    @// partWidth-1
loop_y:
    ADD     Count, Count, Temp, LSL #8


@    M_STRD      pDst, dstStep, ppDstArgs
    STRD    pSrc, srcStep, [sp, #ppSrcArgs]
    STRD    pDst, dstStep, [sp, #ppDstArgs]    

loop_x:
    STRD    pSrc, srcStep, [sp, #ppSrc]
    STRD    pDst, dstStep, [sp, #ppDst]
    STR     pDst, [sp, #ppDstTemp]
    STR     Count,         [sp, #pCount]

    MOV     pDst, sp                
    MOV     dstStep, #24
    
    @// Set up counter of format, [0]  [0]  [1 (n_h)]  [8 (n_w)]                                                                                    
    MOV         Counter, #1
    MOV         Temp, #8                                                        
    ADD         Counter, Temp, Counter, LSL #8        @// [0 0 H W]                        
        
    LDR         r0x00ff00ff, =0x00ff00ff                @// [0 255 0 255] 255 is offset to avoid negative results 
WidthLoop:
    STR       pSrc, [sp, #ppSrc]
    STR       pDst, [sp, #ppDst]
HeightLoop:
TwoRowsLoop:
    LDR       n_ValC, [pSrc], srcStep                   @// Load  [c3 c2 c1 c0]
    LDR       n_ValD, [pSrc], srcStep                   @// Load  [d3 d2 d1 d0]
    LDR       n_ValE, [pSrc], srcStep                   @// Load  [e3 e2 e1 e0]        
    SUB         pSrc, pSrc, srcStep, LSL #2                
    UXTAB16     n_ValC0, r0x00ff00ff, n_ValC                @// [0 c2 0 c0] + [0 255 0 255]
    UXTAB16     n_ValC1, r0x00ff00ff, n_ValC, ROR #8        @// [0 c3 0 c1] + [0 255 0 255]        
    LDR         n_ValB, [pSrc]                            @// Load  [b3 b2 b1 b0]        
    UXTAB16     n_ValE0, r0x00ff00ff, n_ValE                @// [0 e2 0 e0] + [0 255 0 255]
    UXTAB16     ValE1, r0x00ff00ff, n_ValE, ROR #8        @// [0 e3 0 e1] + [0 255 0 255]        
    UXTAB16     n_ValCD0, n_ValC0, n_ValD                     @// [0 c2 0 c0] + [0 255 0 255] + [0 d2 0 d0]
    UXTAB16     n_ValCD1, n_ValC1, n_ValD, ROR #8             @// [0 c3 0 c1] + [0 255 0 255] + [0 d3 0 d1]                                
    UXTAB16     ValEB0, n_ValE0, n_ValB                     @// [0 e2 0 e0] + [0 255 0 255] + [0 b2 0 b0]
    RSB         n_ValCD0, ValEB0, n_ValCD0, LSL #2          @// 4*(Off+C+D) - (Off+B+E)
        
    LDR         n_ValD, [pSrc, srcStep, LSL #1]                       @// Load  [d3 d2 d1 d0]
    UXTAB16     ValEB1, ValE1, n_ValB, ROR #8             @// [0 e3 0 e1] + [0 255 0 255] + [0 b3 0 b1]                                               
    RSB         n_ValCD1, ValEB1, n_ValCD1, LSL #2                
        
    UXTAB16     ValED0, n_ValE0, n_ValD                     @// [0 e2 0 e0] + [0 255 0 255] + [0 d2 0 d0]
    UXTAB16     ValED1, ValE1, n_ValD, ROR #8             @// [0 e3 0 e1] + [0 255 0 255] + [0 d3 0 d1]                                                       
    LDR         n_ValF, [pSrc, srcStep, LSL #2]           @// Load  [f3 f2 f1 f0]
    LDR       n_ValB, [pSrc], srcStep                   @// Load  [b3 b2 b1 b0]                
    ADD         n_ValCD0, n_ValCD0, n_ValCD0, LSL #2          @// 5 * [4*(Off+C+D) - (Off+B+E)]
    ADD         n_ValCD1, n_ValCD1, n_ValCD1, LSL #2                          
    UXTAB16     ValCF1, n_ValC1, n_ValF, ROR #8             @// [0 c3 0 c1] + [0 255 0 255] + [0 f3 0 f1]                                
    UXTAB16     ValCF0, n_ValC0, n_ValF                     @// [0 c2 0 c0] + [0 255 0 255] + [0 f2 0 f0]        
    RSB         ValED1, ValCF1, ValED1, LSL #2        
        
    SUB         n_ValA, pSrc, srcStep, LSL #1
    LDR         n_ValA, [n_ValA]                            @// Load  [a3 a2 a1 a0]
    RSB         ValED0, ValCF0, ValED0, LSL #2          @// 4*(Off+E+D) - (Off+C+F)        
    ADD         ValED1, ValED1, ValED1, LSL #2          
    ADD         ValED0, ValED0, ValED0, LSL #2          @// 5 * [4*(Off+E+D) - (Off+C+F)]
    UXTAB16     ValA0, r0x00ff00ff, n_ValA                @// [0 a2 0 a0] + [0 255 0 255]
    UXTAB16     ValA1, r0x00ff00ff, n_ValA, ROR #8        @// [0 a3 0 a1] + [0 255 0 255]
    UXTAB16     ValAF0, ValA0, n_ValF                     @// [0 a2 0 a0] + [0 255 0 255] + [0 f2 0 f0]
    UXTAB16     ValAF1, ValA1, n_ValF, ROR #8             @// [0 a3 0 a1] + [0 255 0 255] + [0 f3 0 f1]                                        
    ADD         n_voACC1, n_ValCD1, ValAF1        
        
    LDR         n_ValG, [pSrc, srcStep, LSL #2]           @// Load  [g3 g2 g1 g0]
    ADD         voAcc0, n_ValCD0, ValAF0                    @// voAcc0 = 16*Off + (A+F) + 20*(C+D) - 5*(B+E)        
    STR         n_voACC1, [pDst, #4]                        @// Store result & adjust pointer
    STR         voAcc0, [pDst], dstStep                   @// Store result & adjust pointer
    UXTAB16     ValG0, r0x00ff00ff, n_ValG                @// [0 g2 0 g0] + [0 255 0 255]
    UXTAB16     ValG1, r0x00ff00ff, n_ValG, ROR #8        @// [0 g3 0 g1] + [0 255 0 255]
    UXTAB16     ValGB0, ValG0, n_ValB                     @// [0 g2 0 g0] + [0 255 0 255] + [0 b2 0 b0]
    UXTAB16     ValGB1, ValG1, n_ValB, ROR #8             @// [0 g3 0 g1] + [0 255 0 255] + [0 b3 0 b1]                        
    ADD         n_voACC2, ValED0, ValGB0                    @// n_voACC2 = 16*Off + (B+G) + 20*(D+E) - 5*(C+F)
    ADD         n_voACC3, ValED1, ValGB1        
        
    STR         n_voACC3, [pDst, #4]                        @// Store result & adjust pointer                                       
    STR         n_voACC2, [pDst], dstStep                   @// Store result & adjust pointer                                               
        
    SUBS        Counter, Counter, #1 << 8               @// Loop till n_h is 10
    ADD         pSrc, pSrc, srcStep, LSL #1
    BPL         HeightLoop
        
    LDR         pSrc, [sp, #ppSrc]
    LDR         pDst, [sp, #ppDst]
    ADDS        Counter, Counter, #(1 << 8)-4           @// Loop till n_w is 12
    ADD         pSrc, pSrc, #4
    ADD         pDst, pDst, #8
    ADD         Counter, Counter, #1<<8
    BPL         WidthLoop
    
    @//
    @// Horizontal interpolation using multiplication
    @//
    
    SUB         pSrc, pDst, #24
    MOV         srcStep, #24
 @   LDRD        pDst, dstStep, [sp, #ppDstArgs]
    LDR         pDst,     [sp, #ppDstTemp]
    LDR         dstStep,  [sp, #ppDstStepArgs]

    MOV         Counter, #4
    LDR         r0x0014fffb, =0x0014fffb
    LDR         r0x00140001, =0x00140001

HeightLoop1:
    STR         Counter, [sp, #pCounter]


    LDR         ValCA, [pSrc], #4                   @// Load  [0 c 0 a]
    LDR         ValDB, [pSrc], #4                   @// Load  [0 d 0 b]
    LDR         ValGE, [pSrc], #4                   @// Load  [0 g 0 e]
    LDR         ValHF, [pSrc], #4                   @// Load  [0 h 0 f]

    LDR         voAcc0, [sp, #pHorOffset]
    SUBS        voAcc0 ,voAcc0, #1
    BCS         hOr32Else    
hOr32If:
    @// voAcc0 = smuad ([0 20 0 1], add([0 c 0 a] + [0 d 0 f])) - (5 * (b + e)) 
    @// n_voACC1 = smuad ([0 20 0 1], add([0 e 0 g] + [0 d 0 b])) - (5 * (c + f)) 
    @// n_voACC2 = smuad ([0 1 0 20], add([0 c 0 e] + [0 h 0 f])) - (5 * (d + g)) 
    @// n_voACC3 = smuad ([0 20 0 1], add([0 d 0 f] + [0 i 0 g])) - (5 * (e + h)) 
    SMUAD       voAcc0, ValCA, r0x00140001            @// voAcc0  = [0 c 0 a] * [0 20 0 1]
    SMUAD       n_voACC1, ValDB, r0x00140001            @// n_voACC1  = [0 c 0 a] * [0 20 0 1]
    SMUADX      n_voACC2, ValGE, r0x0014fffb            @// n_voACC2  = [0 g 0 e] * [0 20 0 -5]
    SMUAD       n_voACC3, ValGE, r0x0014fffb            @// n_voACC3  = [0 g 0 e] * [0 20 0 -5]

    SMLAD       voAcc0, ValDB, r0x0014fffb, voAcc0      @// voAcc0 += [0 d 0 b] * [0 20 0 -5]
    SMLADX      n_voACC1, ValGE, r0x00140001, n_voACC1      @// n_voACC1 += [0 g 0 e] * [0 20 0 1]
    SMLADX      n_voACC2, ValHF, r0x00140001, n_voACC2      @// n_voACC2 += [0 h 0 f] * [0 20 0 1]
    SMLADX      n_voACC3, ValHF, r0x0014fffb, n_voACC3      @// n_voACC3 += [0 h 0 f] * [0 20 0 -5]

    SMLABB      voAcc0, ValGE, r0x0014fffb, voAcc0      @// voAcc0 += [0 g 0 e] * [0 0 0 -5]
    SMLATB      n_voACC1, ValCA, r0x0014fffb, n_voACC1      @// n_voACC1 += [0 d 0 b] * [0 0 0 -5]
    SMLATB      n_voACC2, ValCA, r0x00140001, n_voACC2      @// n_voACC2 += [0 c 0 a] * [0 0 0 1]
    SMLATB      n_voACC3, ValDB, r0x00140001, n_voACC3      @// n_voACC3 += [0 c 0 a] * [0 0 0 1]

    PKHBT       ValGE, ValGE, ValCA                 @  [0 c 0 e]
    LDRH        ValCA, [pSrc], #8                   @// 8 = srcStep - 16
    SMLABB      voAcc0, ValHF, r0x00140001, voAcc0      @// voAcc0 += [0 h 0 f] * [0 0 0 1]        
    SMLABB      n_voACC1, ValHF, r0x0014fffb, n_voACC1      @// n_voACC1 += [0 h 0 f] * [0 0 0 -5]
    SMLATB      n_voACC2, ValDB, r0x0014fffb, n_voACC2      @// n_voACC2 += [0 d 0 b] * [0 0 0 -5]        
    SMLABB      n_voACC3, ValCA, r0x00140001, n_voACC3      @// n_voACC3 += [0 d 0 b] * [0 0 0 1]     
    PKHTB       ValDB, ValDB, ValHF                 @  [0 d 0 f]
    
    B           hOr32EndIf
hOr32Else:

    @// voAcc0 = smuad ([0 20 0 1], add([0 c 0 a] + [0 d 0 f])) - (5 * (b + e)) 
    @// n_voACC1 = smuad ([0 20 0 1], add([0 e 0 g] + [0 d 0 b])) - (5 * (c + f)) 
    @// n_voACC2 = smuad ([0 1 0 20], add([0 c 0 e] + [0 h 0 f])) - (5 * (d + g)) 
    @// n_voACC3 = smuad ([0 20 0 1], add([0 d 0 f] + [0 i 0 g])) - (5 * (e + h)) 
    SMUAD       voAcc0, ValCA, r0x00140001            @// voAcc0  = [0 c 0 a] * [0 20 0 1]
    SMUAD       n_voACC1, ValDB, r0x00140001            @// n_voACC1  = [0 c 0 a] * [0 20 0 1]
    SMUADX      n_voACC2, ValGE, r0x0014fffb            @// n_voACC2  = [0 g 0 e] * [0 20 0 -5]
    SMUAD       n_voACC3, ValGE, r0x0014fffb            @// n_voACC3  = [0 g 0 e] * [0 20 0 -5]

    SMLAD       voAcc0, ValDB, r0x0014fffb, voAcc0      @// voAcc0 += [0 d 0 b] * [0 20 0 -5]
    SMLADX      n_voACC1, ValGE, r0x00140001, n_voACC1      @// n_voACC1 += [0 g 0 e] * [0 20 0 1]
    SMLADX      n_voACC2, ValHF, r0x00140001, n_voACC2      @// n_voACC2 += [0 h 0 f] * [0 20 0 1]
    SMLADX      n_voACC3, ValHF, r0x0014fffb, n_voACC3      @// n_voACC3 += [0 h 0 f] * [0 20 0 -5]

    SMLABB      voAcc0, ValGE, r0x0014fffb, voAcc0      @// voAcc0 += [0 g 0 e] * [0 0 0 -5]
    SMLATB      n_voACC1, ValCA, r0x0014fffb, n_voACC1      @// n_voACC1 += [0 d 0 b] * [0 0 0 -5]
    SMLATB      n_voACC2, ValCA, r0x00140001, n_voACC2      @// n_voACC2 += [0 c 0 a] * [0 0 0 1]
    SMLATB      n_voACC3, ValDB, r0x00140001, n_voACC3      @// n_voACC3 += [0 c 0 a] * [0 0 0 1]

    LDRH        ValCA, [pSrc], #8                   @// 8 = srcStep - 16
    SMLABB      voAcc0, ValHF, r0x00140001, voAcc0      @// voAcc0 += [0 h 0 f] * [0 0 0 1]        
    SMLABB      n_voACC1, ValHF, r0x0014fffb, n_voACC1      @// n_voACC1 += [0 h 0 f] * [0 0 0 -5]
    SMLATB      n_voACC2, ValDB, r0x0014fffb, n_voACC2      @// n_voACC2 += [0 d 0 b] * [0 0 0 -5]        
    SMLABB      n_voACC3, ValCA, r0x00140001, n_voACC3      @// n_voACC3 += [0 d 0 b] * [0 0 0 1] 

    PKHTB       ValHF, ValDB, ValHF                 @  [0 d 0 f]
    MOV         ValDB, ValGE, ROR #16               @  [0 e 0 g] 
    MOV         ValGE, ValHF                        @  [0 d 0 f]
hOr32EndIf:    
    
    LDR         r0x0001fc00, =0x0001fc00            @// (0xff * 16 * 32) - 512
    SUB         voAcc0, voAcc0, r0x0001fc00        
    SUB         n_voACC1, n_voACC1, r0x0001fc00        
    SUB         n_voACC2, n_voACC2, r0x0001fc00        
    SUB         n_voACC3, n_voACC3, r0x0001fc00        

    USAT        voAcc0, #18, voAcc0
    USAT        n_voACC1, #18, n_voACC1
    USAT        n_voACC2, #18, n_voACC2
    USAT        n_voACC3, #18, n_voACC3
   
    LDR         r0x0001fc00, = 0x0FE00FE0    @ // (0xff * 16) - 16   
    UQSUB16     ValGE, ValGE, r0x0001fc00   @  [0 c 0 e]
    UQSUB16     ValDB, ValDB, r0x0001fc00   @  [0 d 0 f]
    
    LDR         r0x0001fc00, = 0x01FF01FF
    AND         ValGE, r0x0001fc00, ValGE, LSR #5  @  [0 c 0 e]
    AND         ValDB, r0x0001fc00, ValDB, LSR #5  @  [0 d 0 f]
    USAT16      ValGE, #8, ValGE
    USAT16      ValDB, #8, ValDB

    ORR         ValGE, ValGE, ValDB, LSL #8 @  [d c f e]
    ROR         ValGE, ValGE, #16           @  [f e c d]
      
    MOV         voAcc0, voAcc0, LSR #10
    MOV         n_voACC1, n_voACC1, LSR #10
    MOV         n_voACC2, n_voACC2, LSR #10
    MOV         n_voACC3, n_voACC3, LSR #10

    LDR         Counter, [sp, #pCounter]        
    ORR         voAcc0, voAcc0, n_voACC1, LSL #8
    ORR         n_voACC2, n_voACC2, n_voACC3, LSL #8
    ORR         voAcc0, voAcc0, n_voACC2, LSL #16
    UHSUB8      ValGE, voAcc0, ValGE
    USUB8       voAcc0, voAcc0, ValGE
    STR         voAcc0, [pDst], dstStep
    SUBS        Counter, Counter, #1
    BGT         HeightLoop1

    LDR     Count, [sp, #pCount]
    SUBS    Count, Count, #4<<28
    LDRD    pSrc, srcStep, [sp, #ppSrc]
@    LDRD    pDst, dstStep, [sp, #ppDst]
    LDR     pDst, [sp, #ppDstTemp]    
    SUB     pSrc, pSrc, #4
    ADD     pDst, pDst, #4
    BCS     loop_x

    LDRD    pSrc, srcStep, [sp, #ppSrcArgs]  
    ADD     pSrc, pSrc, srcStep, LSL #2
    LDRD    pDst, dstStep, [sp, #ppDstArgs]
    ADD     pDst, pDst, dstStep, LSL #2
    AND     Temp, Count, #0x00F00000    @// partWidth-1
    ADDS    Count, Count, #(1<<28)-(4<<24)    
    BGE     loop_y  

    ADD     sp,sp,#0xC4
    LDMFD   sp!, {r4-r11, pc}   

@extern void add_luma_12_32_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride,VO_U32 n_partW, VO_U32 n_partH, VO_S32 n_deltaX, VO_S32 n_deltaY, VO_U32 horOffset)	
add_luma_12_32_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    SUB     sp, sp, #0xB4
    
    LDR     n_partW, [sp,#pPartWidth]       @// partWidth
    LDR     n_partH, [sp,#pPartHeight]       @// partHeight
    LDR     n_deltaX ,[sp,#pDeltaX]         @// n_deltaX
    LDR     n_deltaY ,[sp,#pDeltaY]         @// n_deltaY
    MLA     Temp, srcStep, n_deltaY, n_deltaX   @// n_deltaY*n_w+n_deltaX
    ADD     pSrc, pSrc, Temp          @// p_Src += n_deltaY*n_w+n_deltaX


    @// pack values to n_cnt register
    @// [31:28] loop_x (partWidth-1)
    @// [27:24] loop_y (partHeight-1)
    @// [23:20] partWidth-1
    @// [19:16] partHeight-1
    @// [15:00] n_w
    MOV     Count, srcStep
    SUB     n_partW, n_partW, #1@
    SUB     n_partH, n_partH, #1@
    ADD     Temp, n_partH, n_partW, LSL #4
    ADD     Count, Count, Temp, LSL #16    

    AND     Temp, Count, #0x000F0000    @// partHeight-1
    ADD     Count, Count, Temp, LSL #8
    AND     Temp, Count, #0x00F00000    @// partWidth-1
add_loop_y:
    ADD     Count, Count, Temp, LSL #8


@    M_STRD      pDst, dstStep, ppDstArgs
    STRD    pSrc, srcStep, [sp, #ppSrcArgs]
    STRD    pDst, dstStep, [sp, #ppDstArgs]    

add_loop_x:
    STRD    pSrc, srcStep, [sp, #ppSrc]
    STRD    pDst, dstStep, [sp, #ppDst]
    STR     pDst, [sp, #ppDstTemp]
    STR     Count,         [sp, #pCount]

    MOV     pDst, sp                
    MOV     dstStep, #24
    
    @// Set up counter of format, [0]  [0]  [1 (n_h)]  [8 (n_w)]                                                                                    
    MOV         Counter, #1
    MOV         Temp, #8                                                        
    ADD         Counter, Temp, Counter, LSL #8        @// [0 0 H W]                        
        
    LDR         r0x00ff00ff, =0x00ff00ff                @// [0 255 0 255] 255 is offset to avoid negative results 
add_WidthLoop:
    STR       pSrc, [sp, #ppSrc]
    STR       pDst, [sp, #ppDst]
add_HeightLoop:
add_TwoRowsLoop:
    LDR       n_ValC, [pSrc], srcStep                   @// Load  [c3 c2 c1 c0]
    LDR       n_ValD, [pSrc], srcStep                   @// Load  [d3 d2 d1 d0]
    LDR       n_ValE, [pSrc], srcStep                   @// Load  [e3 e2 e1 e0]        
    SUB         pSrc, pSrc, srcStep, LSL #2                
    UXTAB16     n_ValC0, r0x00ff00ff, n_ValC                @// [0 c2 0 c0] + [0 255 0 255]
    UXTAB16     n_ValC1, r0x00ff00ff, n_ValC, ROR #8        @// [0 c3 0 c1] + [0 255 0 255]        
    LDR         n_ValB, [pSrc]                            @// Load  [b3 b2 b1 b0]        
    UXTAB16     n_ValE0, r0x00ff00ff, n_ValE                @// [0 e2 0 e0] + [0 255 0 255]
    UXTAB16     ValE1, r0x00ff00ff, n_ValE, ROR #8        @// [0 e3 0 e1] + [0 255 0 255]        
    UXTAB16     n_ValCD0, n_ValC0, n_ValD                     @// [0 c2 0 c0] + [0 255 0 255] + [0 d2 0 d0]
    UXTAB16     n_ValCD1, n_ValC1, n_ValD, ROR #8             @// [0 c3 0 c1] + [0 255 0 255] + [0 d3 0 d1]                                
    UXTAB16     ValEB0, n_ValE0, n_ValB                     @// [0 e2 0 e0] + [0 255 0 255] + [0 b2 0 b0]
    RSB         n_ValCD0, ValEB0, n_ValCD0, LSL #2          @// 4*(Off+C+D) - (Off+B+E)
        
    LDR         n_ValD, [pSrc, srcStep, LSL #1]                       @// Load  [d3 d2 d1 d0]
    UXTAB16     ValEB1, ValE1, n_ValB, ROR #8             @// [0 e3 0 e1] + [0 255 0 255] + [0 b3 0 b1]                                               
    RSB         n_ValCD1, ValEB1, n_ValCD1, LSL #2                
        
    UXTAB16     ValED0, n_ValE0, n_ValD                     @// [0 e2 0 e0] + [0 255 0 255] + [0 d2 0 d0]
    UXTAB16     ValED1, ValE1, n_ValD, ROR #8             @// [0 e3 0 e1] + [0 255 0 255] + [0 d3 0 d1]                                                       
    LDR         n_ValF, [pSrc, srcStep, LSL #2]           @// Load  [f3 f2 f1 f0]
    LDR       n_ValB, [pSrc], srcStep                   @// Load  [b3 b2 b1 b0]                
    ADD         n_ValCD0, n_ValCD0, n_ValCD0, LSL #2          @// 5 * [4*(Off+C+D) - (Off+B+E)]
    ADD         n_ValCD1, n_ValCD1, n_ValCD1, LSL #2                          
    UXTAB16     ValCF1, n_ValC1, n_ValF, ROR #8             @// [0 c3 0 c1] + [0 255 0 255] + [0 f3 0 f1]                                
    UXTAB16     ValCF0, n_ValC0, n_ValF                     @// [0 c2 0 c0] + [0 255 0 255] + [0 f2 0 f0]        
    RSB         ValED1, ValCF1, ValED1, LSL #2        
        
    SUB         n_ValA, pSrc, srcStep, LSL #1
    LDR         n_ValA, [n_ValA]                            @// Load  [a3 a2 a1 a0]
    RSB         ValED0, ValCF0, ValED0, LSL #2          @// 4*(Off+E+D) - (Off+C+F)        
    ADD         ValED1, ValED1, ValED1, LSL #2          
    ADD         ValED0, ValED0, ValED0, LSL #2          @// 5 * [4*(Off+E+D) - (Off+C+F)]
    UXTAB16     ValA0, r0x00ff00ff, n_ValA                @// [0 a2 0 a0] + [0 255 0 255]
    UXTAB16     ValA1, r0x00ff00ff, n_ValA, ROR #8        @// [0 a3 0 a1] + [0 255 0 255]
    UXTAB16     ValAF0, ValA0, n_ValF                     @// [0 a2 0 a0] + [0 255 0 255] + [0 f2 0 f0]
    UXTAB16     ValAF1, ValA1, n_ValF, ROR #8             @// [0 a3 0 a1] + [0 255 0 255] + [0 f3 0 f1]                                        
    ADD         n_voACC1, n_ValCD1, ValAF1        
        
    LDR         n_ValG, [pSrc, srcStep, LSL #2]           @// Load  [g3 g2 g1 g0]
    ADD         voAcc0, n_ValCD0, ValAF0                    @// voAcc0 = 16*Off + (A+F) + 20*(C+D) - 5*(B+E)        
    STR         n_voACC1, [pDst, #4]                        @// Store result & adjust pointer
    STR         voAcc0, [pDst], dstStep                   @// Store result & adjust pointer
    UXTAB16     ValG0, r0x00ff00ff, n_ValG                @// [0 g2 0 g0] + [0 255 0 255]
    UXTAB16     ValG1, r0x00ff00ff, n_ValG, ROR #8        @// [0 g3 0 g1] + [0 255 0 255]
    UXTAB16     ValGB0, ValG0, n_ValB                     @// [0 g2 0 g0] + [0 255 0 255] + [0 b2 0 b0]
    UXTAB16     ValGB1, ValG1, n_ValB, ROR #8             @// [0 g3 0 g1] + [0 255 0 255] + [0 b3 0 b1]                        
    ADD         n_voACC2, ValED0, ValGB0                    @// n_voACC2 = 16*Off + (B+G) + 20*(D+E) - 5*(C+F)
    ADD         n_voACC3, ValED1, ValGB1        
        
    STR         n_voACC3, [pDst, #4]                        @// Store result & adjust pointer                                       
    STR         n_voACC2, [pDst], dstStep                   @// Store result & adjust pointer                                               
        
    SUBS        Counter, Counter, #1 << 8               @// Loop till n_h is 10
    ADD         pSrc, pSrc, srcStep, LSL #1
    BPL         add_HeightLoop
        
    LDR         pSrc, [sp, #ppSrc]
    LDR         pDst, [sp, #ppDst]
    ADDS        Counter, Counter, #(1 << 8)-4           @// Loop till n_w is 12
    ADD         pSrc, pSrc, #4
    ADD         pDst, pDst, #8
    ADD         Counter, Counter, #1<<8
    BPL         add_WidthLoop
    
    @//
    @// Horizontal interpolation using multiplication
    @//
    
    SUB         pSrc, pDst, #24
    MOV         srcStep, #24
 @   LDRD        pDst, dstStep, [sp, #ppDstArgs]
    LDR         pDst,     [sp, #ppDstTemp]
    LDR         dstStep,  [sp, #ppDstStepArgs]

    MOV         Counter, #4
    LDR         r0x0014fffb, =0x0014fffb
    LDR         r0x00140001, =0x00140001

add_HeightLoop1:
    STR         Counter, [sp, #pCounter]


    LDR         ValCA, [pSrc], #4                   @// Load  [0 c 0 a]
    LDR         ValDB, [pSrc], #4                   @// Load  [0 d 0 b]
    LDR         ValGE, [pSrc], #4                   @// Load  [0 g 0 e]
    LDR         ValHF, [pSrc], #4                   @// Load  [0 h 0 f]

    LDR         voAcc0, [sp, #pHorOffset]
    SUBS        voAcc0 ,voAcc0, #1
    BCS         add_hOr32Else    
add_hOr32If:
    @// voAcc0 = smuad ([0 20 0 1], add([0 c 0 a] + [0 d 0 f])) - (5 * (b + e)) 
    @// n_voACC1 = smuad ([0 20 0 1], add([0 e 0 g] + [0 d 0 b])) - (5 * (c + f)) 
    @// n_voACC2 = smuad ([0 1 0 20], add([0 c 0 e] + [0 h 0 f])) - (5 * (d + g)) 
    @// n_voACC3 = smuad ([0 20 0 1], add([0 d 0 f] + [0 i 0 g])) - (5 * (e + h)) 
    SMUAD       voAcc0, ValCA, r0x00140001            @// voAcc0  = [0 c 0 a] * [0 20 0 1]
    SMUAD       n_voACC1, ValDB, r0x00140001            @// n_voACC1  = [0 c 0 a] * [0 20 0 1]
    SMUADX      n_voACC2, ValGE, r0x0014fffb            @// n_voACC2  = [0 g 0 e] * [0 20 0 -5]
    SMUAD       n_voACC3, ValGE, r0x0014fffb            @// n_voACC3  = [0 g 0 e] * [0 20 0 -5]

    SMLAD       voAcc0, ValDB, r0x0014fffb, voAcc0      @// voAcc0 += [0 d 0 b] * [0 20 0 -5]
    SMLADX      n_voACC1, ValGE, r0x00140001, n_voACC1      @// n_voACC1 += [0 g 0 e] * [0 20 0 1]
    SMLADX      n_voACC2, ValHF, r0x00140001, n_voACC2      @// n_voACC2 += [0 h 0 f] * [0 20 0 1]
    SMLADX      n_voACC3, ValHF, r0x0014fffb, n_voACC3      @// n_voACC3 += [0 h 0 f] * [0 20 0 -5]

    SMLABB      voAcc0, ValGE, r0x0014fffb, voAcc0      @// voAcc0 += [0 g 0 e] * [0 0 0 -5]
    SMLATB      n_voACC1, ValCA, r0x0014fffb, n_voACC1      @// n_voACC1 += [0 d 0 b] * [0 0 0 -5]
    SMLATB      n_voACC2, ValCA, r0x00140001, n_voACC2      @// n_voACC2 += [0 c 0 a] * [0 0 0 1]
    SMLATB      n_voACC3, ValDB, r0x00140001, n_voACC3      @// n_voACC3 += [0 c 0 a] * [0 0 0 1]

    PKHBT       ValGE, ValGE, ValCA                 @  [0 c 0 e]
    LDRH        ValCA, [pSrc], #8                   @// 8 = srcStep - 16
    SMLABB      voAcc0, ValHF, r0x00140001, voAcc0      @// voAcc0 += [0 h 0 f] * [0 0 0 1]        
    SMLABB      n_voACC1, ValHF, r0x0014fffb, n_voACC1      @// n_voACC1 += [0 h 0 f] * [0 0 0 -5]
    SMLATB      n_voACC2, ValDB, r0x0014fffb, n_voACC2      @// n_voACC2 += [0 d 0 b] * [0 0 0 -5]        
    SMLABB      n_voACC3, ValCA, r0x00140001, n_voACC3      @// n_voACC3 += [0 d 0 b] * [0 0 0 1]     
    PKHTB       ValDB, ValDB, ValHF                 @  [0 d 0 f]
    
    B           add_hOr32EndIf
add_hOr32Else:

    @// voAcc0 = smuad ([0 20 0 1], add([0 c 0 a] + [0 d 0 f])) - (5 * (b + e)) 
    @// n_voACC1 = smuad ([0 20 0 1], add([0 e 0 g] + [0 d 0 b])) - (5 * (c + f)) 
    @// n_voACC2 = smuad ([0 1 0 20], add([0 c 0 e] + [0 h 0 f])) - (5 * (d + g)) 
    @// n_voACC3 = smuad ([0 20 0 1], add([0 d 0 f] + [0 i 0 g])) - (5 * (e + h)) 
    SMUAD       voAcc0, ValCA, r0x00140001            @// voAcc0  = [0 c 0 a] * [0 20 0 1]
    SMUAD       n_voACC1, ValDB, r0x00140001            @// n_voACC1  = [0 c 0 a] * [0 20 0 1]
    SMUADX      n_voACC2, ValGE, r0x0014fffb            @// n_voACC2  = [0 g 0 e] * [0 20 0 -5]
    SMUAD       n_voACC3, ValGE, r0x0014fffb            @// n_voACC3  = [0 g 0 e] * [0 20 0 -5]

    SMLAD       voAcc0, ValDB, r0x0014fffb, voAcc0      @// voAcc0 += [0 d 0 b] * [0 20 0 -5]
    SMLADX      n_voACC1, ValGE, r0x00140001, n_voACC1      @// n_voACC1 += [0 g 0 e] * [0 20 0 1]
    SMLADX      n_voACC2, ValHF, r0x00140001, n_voACC2      @// n_voACC2 += [0 h 0 f] * [0 20 0 1]
    SMLADX      n_voACC3, ValHF, r0x0014fffb, n_voACC3      @// n_voACC3 += [0 h 0 f] * [0 20 0 -5]

    SMLABB      voAcc0, ValGE, r0x0014fffb, voAcc0      @// voAcc0 += [0 g 0 e] * [0 0 0 -5]
    SMLATB      n_voACC1, ValCA, r0x0014fffb, n_voACC1      @// n_voACC1 += [0 d 0 b] * [0 0 0 -5]
    SMLATB      n_voACC2, ValCA, r0x00140001, n_voACC2      @// n_voACC2 += [0 c 0 a] * [0 0 0 1]
    SMLATB      n_voACC3, ValDB, r0x00140001, n_voACC3      @// n_voACC3 += [0 c 0 a] * [0 0 0 1]

    LDRH        ValCA, [pSrc], #8                   @// 8 = srcStep - 16
    SMLABB      voAcc0, ValHF, r0x00140001, voAcc0      @// voAcc0 += [0 h 0 f] * [0 0 0 1]        
    SMLABB      n_voACC1, ValHF, r0x0014fffb, n_voACC1      @// n_voACC1 += [0 h 0 f] * [0 0 0 -5]
    SMLATB      n_voACC2, ValDB, r0x0014fffb, n_voACC2      @// n_voACC2 += [0 d 0 b] * [0 0 0 -5]        
    SMLABB      n_voACC3, ValCA, r0x00140001, n_voACC3      @// n_voACC3 += [0 d 0 b] * [0 0 0 1] 

    PKHTB       ValHF, ValDB, ValHF                 @  [0 d 0 f]
    MOV         ValDB, ValGE, ROR #16               @  [0 e 0 g] 
    MOV         ValGE, ValHF                        @  [0 d 0 f]
add_hOr32EndIf:    
    
    LDR         r0x0001fc00, =0x0001fc00            @// (0xff * 16 * 32) - 512
    SUB         voAcc0, voAcc0, r0x0001fc00        
    SUB         n_voACC1, n_voACC1, r0x0001fc00        
    SUB         n_voACC2, n_voACC2, r0x0001fc00        
    SUB         n_voACC3, n_voACC3, r0x0001fc00        

    USAT        voAcc0, #18, voAcc0
    USAT        n_voACC1, #18, n_voACC1
    USAT        n_voACC2, #18, n_voACC2
    USAT        n_voACC3, #18, n_voACC3
   
    LDR         r0x0001fc00, = 0x0FE00FE0    @ // (0xff * 16) - 16   
    UQSUB16     ValGE, ValGE, r0x0001fc00   @  [0 c 0 e]
    UQSUB16     ValDB, ValDB, r0x0001fc00   @  [0 d 0 f]
    
    LDR         r0x0001fc00, = 0x01FF01FF
    AND         ValGE, r0x0001fc00, ValGE, LSR #5  @  [0 c 0 e]
    AND         ValDB, r0x0001fc00, ValDB, LSR #5  @  [0 d 0 f]
    USAT16      ValGE, #8, ValGE
    USAT16      ValDB, #8, ValDB

    ORR         ValGE, ValGE, ValDB, LSL #8 @  [d c f e]
    ROR         ValGE, ValGE, #16           @  [f e c d]
      
    MOV         voAcc0, voAcc0, LSR #10
    MOV         n_voACC1, n_voACC1, LSR #10
    MOV         n_voACC2, n_voACC2, LSR #10
    MOV         n_voACC3, n_voACC3, LSR #10

    LDR         Counter, [sp, #pCounter]        
    ORR         voAcc0, voAcc0, n_voACC1, LSL #8
    ORR         n_voACC2, n_voACC2, n_voACC3, LSL #8
    ORR         voAcc0, voAcc0, n_voACC2, LSL #16
    UHSUB8      ValGE, voAcc0, ValGE
    USUB8       voAcc0, voAcc0, ValGE
    LDR         n_voACC2, [pDst]
    UHSUB8      n_voACC2, voAcc0, n_voACC2
    USUB8       voAcc0, voAcc0, n_voACC2
    STR         voAcc0, [pDst], dstStep
    SUBS        Counter, Counter, #1
    BGT         add_HeightLoop1

    LDR     Count, [sp, #pCount]
    SUBS    Count, Count, #4<<28
    LDRD    pSrc, srcStep, [sp, #ppSrc]
@    LDRD    pDst, dstStep, [sp, #ppDst]
    LDR     pDst, [sp, #ppDstTemp]    
    SUB     pSrc, pSrc, #4
    ADD     pDst, pDst, #4
    BCS     add_loop_x

    LDRD    pSrc, srcStep, [sp, #ppSrcArgs]  
    ADD     pSrc, pSrc, srcStep, LSL #2
    LDRD    pDst, dstStep, [sp, #ppDstArgs]
    ADD     pDst, pDst, dstStep, LSL #2
    AND     Temp, Count, #0x00F00000    @// partWidth-1
    ADDS    Count, Count, #(1<<28)-(4<<24)    
    BGE     add_loop_y  

    ADD     sp,sp,#0xC4
    LDMFD   sp!, {r4-r11, pc}   

	@.end

