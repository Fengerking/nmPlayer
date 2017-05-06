@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************
 #include "../../../defineID.h"
    .section	  .text


    .global  get_luma_21_23_ARMV6
    .global  add_luma_21_23_ARMV6
	.align 8	

@// Declare input registers
 pSrc            .req r0
 srcStep         .req r1
 pDst            .req r2
 dstStep         .req r3

@// Declare inner loop registers
 voAcc0            .req r4
 n_partW           .req r4
 n_voACC1            .req r5
 n_partH           .req r5
 n_voACC2            .req r6
 n_deltaX          .req r6
 n_voACC3            .req r7
 n_deltaY          .req r7

 n_ValA            .req r4
 n_ValB            .req r5
 n_ValC            .req r6
 n_ValD            .req r7
 n_ValE            .req r8
 n_ValF            .req r9
 n_ValG            .req r12
 n_ValH            .req r14
 n_ValI            .req r1

 n_Tmp1           .req r3
 n_Tmp2           .req r1
 n_Tmp3           .req r12
 n_Tmp4           .req r7
 n_Tmp5           .req r5
 r0x0fe00fe0     .req r3                                    @// [0 (16*255 - 16) 0 (16*255 - 16)]
 r0x00ff00ff     .req r10                                   @// [0 255 0 255] where 255 is offset
 Counter         .req r11
 Count           .req r11
 pInterBuf       .req r8

 ValCA           .req r8
 ValDB           .req r9
 ValGE           .req r10
 ValHF           .req r11
 r0x00140001     .req r12
 r0x0014fffb     .req r14

 r0x0001fc00     .req r11

 Accx            .req r8
 Accy            .req r9
 n_Tmp6           .req r14

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
 pPartWidth      =    0xE4
 pPartHeight     =    0xE8
 pDeltaX         =    0xEC
 pDeltaY         =    0xF0
 pVerOffset      =    0xF4

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

@extern void get_luma_21_23_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride,VO_U32 n_partW, VO_U32 n_partH, VO_S32 n_deltaX, VO_S32 n_deltaY, VO_U32 verOffset)	
get_luma_21_23_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    SUB     sp, sp, #0xB0
    
    LDR     n_partW, [sp,#pPartWidth]       @// partWidth
    LDR     n_partH, [sp,#pPartHeight]       @// partHeight
    LDR     n_deltaX ,[sp,#pDeltaX]         @// n_deltaX
    LDR     n_deltaY ,[sp,#pDeltaY]         @// n_deltaY
    MLA     n_Tmp6, srcStep, n_deltaY, n_deltaX   @// n_deltaY*n_w+n_deltaX
    ADD     pSrc, pSrc, n_Tmp6          @// p_Src += n_deltaY*n_w+n_deltaX


    @// pack values to n_cnt register
    @// [31:28] loop_x (partWidth-1)
    @// [27:24] loop_y (partHeight-1)
    @// [23:20] partWidth-1
    @// [19:16] partHeight-1
    @// [15:00] n_w
    MOV     Count, srcStep
    SUB     n_partW, n_partW, #1@
    SUB     n_partH, n_partH, #1@
    ADD     n_Tmp6, n_partH, n_partW, LSL #4
    ADD     Count, Count, n_Tmp6, LSL #16    

    AND     n_Tmp6, Count, #0x000F0000    @// partHeight-1
    AND     n_Tmp3, Count, #0x00F00000    @// partWidth-1
    ADD     Count, Count, n_Tmp6, LSL #8
loop_y:
    ADD     Count, Count, n_Tmp3, LSL #8


@    M_STRD      pDst, dstStep, ppDstArgs
    STRD    pSrc, srcStep, [sp, #ppSrcArgs]
    STRD    pDst, dstStep, [sp, #ppDstArgs]    

loop_x:
    STRD    pSrc, srcStep, [sp, #ppSrc]
    STRD    pDst, dstStep, [sp, #ppDst]
    STR     Count,       [sp, #pCount]

    MOV     pDst, sp                
    MOV     dstStep, #16

    @// Set up counter of format, [0]  [0]  [1 (n_h)]  [8 (n_w)]                                                                                 
    MOV     Counter, #4
    STR     dstStep, [sp, #pDstStep]
    LDR     r0x00ff00ff, =0x00ff00ff               @// [0 255 0 255] 255 is offset to avoid negative results 

HeightLoop:
NextTwoRowsLoop:
    LDR     n_ValD, [pSrc, srcStep]                   @// Load row 1 [d1 c1 b1 a1]
    LDR     n_ValA, [pSrc], #4                        @// Load row 0 [d0 c0 b0 a0]
    LDR     n_ValH, [pSrc, srcStep]                   @// Load  [h1 g1 f1 e1]        
    LDR     n_ValE, [pSrc], #4                        @// Load  [h0 g0 f0 e0]
    LDRB    n_Tmp2, [pSrc, srcStep]                  @// Load row 1 [l1 k1 j1 i1]
    LDRB    n_Tmp1, [pSrc], #-8                      @// Load row 0 [l0 k0 j0 i0]
        
    PKHBT   n_ValB, n_ValA, n_ValD, LSL #16               @// [b1 a1 b0 a0]
    PKHTB   n_ValD, n_ValD, n_ValA, ASR #16               @// [d1 c1 d0 c0]
    UXTAB16 n_ValA, r0x00ff00ff, n_ValB                 @// [00 a1 00 a0] + [0 255 0 255]
    UXTAB16 n_ValC, r0x00ff00ff, n_ValD                 @// [00 c1 00 c0] + [0 255 0 255]
    PKHBT   n_ValI, n_Tmp1, n_Tmp2, LSL #16             @// [00 i1 00 i0]            
    PKHBT   n_ValF, n_ValE, n_ValH, LSL #16               @// [f1 e1 f0 e0]
    PKHTB   n_ValH, n_ValH, n_ValE, ASR #16               @// [h1 g1 h0 g0]
    UXTAB16 n_ValE, r0x00ff00ff, n_ValF                 @// [00 e1 00 e0] + [0 255 0 255]

    @// Calculate voAcc0
    @// voAcc0 = a - 5*b + 20*c + 20*d - 5*e + f
    UXTAB16 n_Tmp1, n_ValC, n_ValD, ROR #8
    UXTAB16 n_Tmp3, n_ValE, n_ValB, ROR #8
    RSB     n_Tmp1, n_Tmp3, n_Tmp1, LSL #2                
    UXTAB16 voAcc0, n_ValA, n_ValF, ROR #8
    ADD     n_Tmp1, n_Tmp1, n_Tmp1, LSL #2        
    ADD     voAcc0, voAcc0, n_Tmp1       

    @// Calculate n_voACC1
    @// n_voACC1 = b - 5*c + 20*d + 20*e - 5*f + g
    UXTAB16 n_Tmp1, n_ValE, n_ValD, ROR #8
    UXTAB16 n_Tmp3, n_ValC, n_ValF, ROR #8
    RSB     n_Tmp1, n_Tmp3, n_Tmp1, LSL #2                        
    UXTAB16 n_ValG, r0x00ff00ff, n_ValH                 @// [00 g1 00 g0] + [0 255 0 255]
    ADD     n_Tmp1, n_Tmp1, n_Tmp1, LSL #2        
    UXTAB16 n_voACC1, n_ValG, n_ValB, ROR #8
    ADD     n_voACC1, n_voACC1, n_Tmp1        

    UXTAB16 n_voACC2, n_ValC, n_ValH, ROR #8        
    ADD     n_ValI, r0x00ff00ff, n_ValI                 @// [00 i1 00 i0] + [0 255 0 255]        
        
    @// Calculate n_voACC2
    @// n_voACC2 = c - 5*d + 20*e + 20*f - 5*g + h
    UXTAB16 n_Tmp1, n_ValG, n_ValD, ROR #8
    UXTAB16 n_voACC3, n_ValI, n_ValD, ROR #8
    UXTAB16 n_Tmp2, n_ValE, n_ValF, ROR #8
        
    RSB     n_Tmp1, n_Tmp1, n_Tmp2, LSL #2        
    UXTAB16 n_Tmp2, n_ValG, n_ValF, ROR #8
    ADD     n_Tmp1, n_Tmp1, n_Tmp1, LSL #2        
    ADD     n_voACC2, n_voACC2, n_Tmp1        

    @// Calculate n_voACC3
    @// n_voACC3 = d - 5*e + 20*f + 20*g - 5*h + i
    UXTAB16 n_Tmp1, n_ValE, n_ValH, ROR #8
    RSB     n_Tmp1, n_Tmp1, n_Tmp2, LSL #2
    ADD     n_Tmp1, n_Tmp1, n_Tmp1, LSL #2        
    ADD     n_voACC3, n_voACC3, n_Tmp1
        
    LDR     dstStep, [sp, #pDstStep]
    LDR     srcStep, [sp, #pSrcStep]

    @// If Counter is even store voAcc0-n_voACC3 in a temporary buffer
    @// If Counter is off store voAcc0-n_voACC3 and previous voAcc0-n_voACC3 in a intermediate buf 
    ANDS        n_Tmp3, Counter, #1
    BEQ         NoProcessing        
        
    @// Packing previous and current voAcc0-n_voACC3 values
@    M_LDRD      Accx, Accy, pTempResult1
    LDRD        Accx, Accy, [sp, #pTempResult1]
    PKHBT       n_Tmp6, Accx, voAcc0, LSL #16          @//[0 a2 0 a0] = [0 a3 0 a2] [0 a1 0 a0]
    PKHTB       voAcc0, voAcc0, Accx, ASR #16           @//[0 a3 0 a1] = [0 a1 0 a0] [0 a3 0 a2] 
    STR         voAcc0, [pDst, dstStep]                        
    STR         n_Tmp6, [pDst], #4                   
    PKHBT       n_Tmp6, Accy, n_voACC1, LSL #16          @//[0 b2 0 b0] = [0 b3 0 b2] [0 b1 0 b0]
    PKHTB       n_voACC1, n_voACC1, Accy, ASR #16            @//[0 b3 0 b1] = [0 b1 0 b0] [0 b3 0 b2]
@    M_LDRD     Accx, Accy, pTempResult2
    LDRD         Accx, Accy, [sp, #pTempResult2]
    STR         n_voACC1, [pDst, dstStep]                        
    STR         n_Tmp6, [pDst], #4                   
        
    PKHBT       n_Tmp6, Accx, n_voACC2, LSL #16          @//[0 c2 0 c0] = [0 c3 0 c2] [0 c1 0 c0]
    PKHTB       n_voACC2, n_voACC2, Accx, ASR #16            @//[0 c3 0 c1] = [0 c1 0 c0] [0 c3 0 c2]
    STR         n_voACC2, [pDst, dstStep]                        
    STR         n_Tmp6, [pDst], #4                   
    PKHBT       n_Tmp6, Accy, n_voACC3, LSL #16          @//[0 d2 0 d0] = [0 d3 0 d2] [0 d1 0 d0]
    PKHTB       n_voACC3, n_voACC3, Accy, ASR #16            @//[0 d3 0 d1] = [0 d1 0 d0] [0 d3 0 d2]
    STR         n_voACC3, [pDst, dstStep]                        
    STR         n_Tmp6, [pDst], #-12
    ADD         pDst, pDst, dstStep, LSL #1                   
    B           AfterStore

NoProcessing:
@    M_STRD      voAcc0, n_voACC1, pTempResult1
     STRD        voAcc0, n_voACC1, [sp, #pTempResult1]
@    M_STRD      n_voACC2, n_voACC3, pTempResult2
     STRD        n_voACC2, n_voACC3, [sp, #pTempResult2]
AfterStore:
    SUBS        Counter, Counter, #1                @// Loop till n_h is 10
    ADD         pSrc, pSrc, srcStep, LSL #1
    BPL         HeightLoop

    STR         voAcc0, [pDst], #4                    @//[0 a1 0 a0]
    STR         n_voACC1, [pDst], #4
    STR         n_voACC2, [pDst], #4
    STR         n_voACC3, [pDst], #-12
        
    @//
    @// Horizontal interpolation using multiplication
    @//
    
    SUB         pSrc, pDst, dstStep, LSL #2
    MOV         srcStep, #16
@    M_LDRD      pDst, dstStep, ppDstArgs
@    LDRD      pDst, dstStep, [sp, #ppDstArgs]
    LDR      pDst,     [sp, #ppDst]
    LDR      dstStep,  [sp, #ppDstStepArgs]

    MOV         Counter, #4
    LDR         r0x0014fffb, =0x0014fffb
    LDR         r0x00140001, =0x00140001

HeightLoop1:
@    M_STR       Counter, pCounter
    STR         Counter, [sp, #pCounter]

    LDR       ValCA, [pSrc], srcStep               @// Load  [0 c 0 a]
    LDR       ValDB, [pSrc], srcStep               @// Load  [0 d 0 b]
    LDR       ValGE, [pSrc], srcStep               @// Load  [0 g 0 e]
    LDR       ValHF, [pSrc], srcStep               @// Load  [0 h 0 f]

    LDR         voAcc0, [sp, #pVerOffset]
    SUBS        voAcc0 ,voAcc0, #1
    BCS         vOr23Else        
vOr23If:
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
    LDRH        ValCA, [pSrc], #4                   @// 8 = srcStep - 16
    SMLABB      voAcc0, ValHF, r0x00140001, voAcc0      @// voAcc0 += [0 h 0 f] * [0 0 0 1]        
    SMLABB      n_voACC1, ValHF, r0x0014fffb, n_voACC1      @// n_voACC1 += [0 h 0 f] * [0 0 0 -5]
    SMLATB      n_voACC2, ValDB, r0x0014fffb, n_voACC2      @// n_voACC2 += [0 d 0 b] * [0 0 0 -5]        
    SMLABB      n_voACC3, ValCA, r0x00140001, n_voACC3      @// n_voACC3 += [0 d 0 b] * [0 0 0 1]          
    
    PKHTB       ValDB, ValDB, ValHF 
    B           vOr23Endif
vOr23Else:       
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
   
    LDRH        ValCA, [pSrc], #4                   @// 8 = srcStep - 16
    SMLABB      voAcc0, ValHF, r0x00140001, voAcc0      @// voAcc0 += [0 h 0 f] * [0 0 0 1]        
    SMLABB      n_voACC1, ValHF, r0x0014fffb, n_voACC1      @// n_voACC1 += [0 h 0 f] * [0 0 0 -5]
    SMLATB      n_voACC2, ValDB, r0x0014fffb, n_voACC2      @// n_voACC2 += [0 d 0 b] * [0 0 0 -5]        
    SMLABB      n_voACC3, ValCA, r0x00140001, n_voACC3      @// n_voACC3 += [0 d 0 b] * [0 0 0 1]          
    
    PKHTB       ValHF, ValDB, ValHF                 @  [0 d 0 f]
    MOV         ValDB, ValGE, ROR #16               @  [0 e 0 g] 
    MOV         ValGE, ValHF                        @  [0 d 0 f]
vOr23Endif:
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
    UQSUB16      ValGE, ValGE, r0x0001fc00
    UQSUB16      ValDB, ValDB, r0x0001fc00
          
    LDR         r0x0001fc00, = 0x01FF01FF
    AND         ValGE, r0x0001fc00, ValGE, LSR #5  @  [0 c 0 e]
    AND         ValHF, r0x0001fc00, ValDB, LSR #5  @  [0 d 0 f]
    USAT16      ValGE, #8, ValGE
    USAT16      ValHF, #8, ValHF
@    MOV     ValGE,  ValGE, LSR #5  ;  [0 c 0 e]
@    MOV     ValHF,  ValDB, LSR #5  ;  [0 d 0 f]
@    USAT16      ValGE, #8, ValGE
@    USAT16      ValHF, #8, ValHF
    UXTB        ValCA, ValGE, ROR #16
    UXTB        ValDB, ValHF, ROR #16
    UXTB        ValGE, ValGE
    UXTB        ValHF, ValHF
     
    MOV         voAcc0, voAcc0, LSR #10 
    UHSUB8      ValCA, voAcc0, ValCA 
    USUB8       voAcc0, voAcc0, ValCA            
    STRB        voAcc0, [pDst], dstStep
    MOV         n_voACC1, n_voACC1, LSR #10
    UHSUB8      ValDB, n_voACC1, ValDB 
    USUB8       n_voACC1, n_voACC1, ValDB       
    STRB        n_voACC1, [pDst], dstStep
    MOV         n_voACC2, n_voACC2, LSR #10
    UHSUB8      ValGE, n_voACC2, ValGE 
    USUB8       n_voACC2, n_voACC2, ValGE       
    STRB        n_voACC2, [pDst], dstStep
    MOV         n_voACC3, n_voACC3, LSR #10
    UHSUB8      ValHF, n_voACC3, ValHF 
    USUB8       n_voACC3, n_voACC3, ValHF   
    STRB        n_voACC3, [pDst], dstStep


@    M_LDR       Counter, pCounter
    LDR         Counter, [sp, #pCounter]
    SUB         pDst, pDst, dstStep, LSL #2
    SUB         pSrc, pSrc, srcStep, LSL #2
    ADD         pDst, pDst, #1
    SUBS        Counter, Counter, #1
    BGT         HeightLoop1

    LDR     Count, [sp, #pCount]
    SUBS    Count, Count, #4<<28
    LDR     srcStep, [sp, #pSrcStep]
    LDR     pSrc,    [sp, #ppSrc]
    ADD     pSrc, pSrc, #4
    BCS     loop_x

    LDRD    pSrc, srcStep, [sp, #ppSrcArgs]  
    ADD     pSrc, pSrc, srcStep, LSL #2
    LDRD    pDst, dstStep, [sp, #ppDstArgs]
    ADD     pDst, pDst, dstStep, LSL #2
    AND     n_Tmp3, Count, #0x00F00000    @// partWidth-1
    ADDS    Count, Count, #(1<<28)-(4<<24)    
    BGE     loop_y  

    ADD     sp,sp,#0xC0
    LDMFD   sp!, {r4-r11, pc}   

@extern void add_luma_21_23_ARMV6(VO_U8 *p_Src,VO_S32 src_stride,VO_U8 *p_Dst,VO_S32 dst_stride,VO_U32 n_partW, VO_U32 n_partH, VO_S32 n_deltaX, VO_S32 n_deltaY, VO_U32 verOffset)	
add_luma_21_23_ARMV6:
    STMFD   sp!, {r0-r11, lr}
    SUB     sp, sp, #0xB0
    
    LDR     n_partW, [sp,#pPartWidth]       @// partWidth
    LDR     n_partH, [sp,#pPartHeight]       @// partHeight
    LDR     n_deltaX ,[sp,#pDeltaX]         @// n_deltaX
    LDR     n_deltaY ,[sp,#pDeltaY]         @// n_deltaY
    MLA     n_Tmp6, srcStep, n_deltaY, n_deltaX   @// n_deltaY*n_w+n_deltaX
    ADD     pSrc, pSrc, n_Tmp6          @// p_Src += n_deltaY*n_w+n_deltaX


    @// pack values to n_cnt register
    @// [31:28] loop_x (partWidth-1)
    @// [27:24] loop_y (partHeight-1)
    @// [23:20] partWidth-1
    @// [19:16] partHeight-1
    @// [15:00] n_w
    MOV     Count, srcStep
    SUB     n_partW, n_partW, #1@
    SUB     n_partH, n_partH, #1@
    ADD     n_Tmp6, n_partH, n_partW, LSL #4
    ADD     Count, Count, n_Tmp6, LSL #16    

    AND     n_Tmp6, Count, #0x000F0000    @// partHeight-1
    AND     n_Tmp3, Count, #0x00F00000    @// partWidth-1
    ADD     Count, Count, n_Tmp6, LSL #8
add_loop_y:
    ADD     Count, Count, n_Tmp3, LSL #8


@    M_STRD      pDst, dstStep, ppDstArgs
    STRD    pSrc, srcStep, [sp, #ppSrcArgs]
    STRD    pDst, dstStep, [sp, #ppDstArgs]    

add_loop_x:
    STRD    pSrc, srcStep, [sp, #ppSrc]
    STRD    pDst, dstStep, [sp, #ppDst]
    STR     Count,       [sp, #pCount]

    MOV     pDst, sp                
    MOV     dstStep, #16

    @// Set up counter of format, [0]  [0]  [1 (n_h)]  [8 (n_w)]                                                                                 
    MOV     Counter, #4
    STR     dstStep, [sp, #pDstStep]
    LDR     r0x00ff00ff, =0x00ff00ff               @// [0 255 0 255] 255 is offset to avoid negative results 

add_HeightLoop:
add_NextTwoRowsLoop:
    LDR     n_ValD, [pSrc, srcStep]                   @// Load row 1 [d1 c1 b1 a1]
    LDR     n_ValA, [pSrc], #4                        @// Load row 0 [d0 c0 b0 a0]
    LDR     n_ValH, [pSrc, srcStep]                   @// Load  [h1 g1 f1 e1]        
    LDR     n_ValE, [pSrc], #4                        @// Load  [h0 g0 f0 e0]
    LDRB    n_Tmp2, [pSrc, srcStep]                  @// Load row 1 [l1 k1 j1 i1]
    LDRB    n_Tmp1, [pSrc], #-8                      @// Load row 0 [l0 k0 j0 i0]
        
    PKHBT   n_ValB, n_ValA, n_ValD, LSL #16               @// [b1 a1 b0 a0]
    PKHTB   n_ValD, n_ValD, n_ValA, ASR #16               @// [d1 c1 d0 c0]
    UXTAB16 n_ValA, r0x00ff00ff, n_ValB                 @// [00 a1 00 a0] + [0 255 0 255]
    UXTAB16 n_ValC, r0x00ff00ff, n_ValD                 @// [00 c1 00 c0] + [0 255 0 255]
    PKHBT   n_ValI, n_Tmp1, n_Tmp2, LSL #16             @// [00 i1 00 i0]            
    PKHBT   n_ValF, n_ValE, n_ValH, LSL #16               @// [f1 e1 f0 e0]
    PKHTB   n_ValH, n_ValH, n_ValE, ASR #16               @// [h1 g1 h0 g0]
    UXTAB16 n_ValE, r0x00ff00ff, n_ValF                 @// [00 e1 00 e0] + [0 255 0 255]

    @// Calculate voAcc0
    @// voAcc0 = a - 5*b + 20*c + 20*d - 5*e + f
    UXTAB16 n_Tmp1, n_ValC, n_ValD, ROR #8
    UXTAB16 n_Tmp3, n_ValE, n_ValB, ROR #8
    RSB     n_Tmp1, n_Tmp3, n_Tmp1, LSL #2                
    UXTAB16 voAcc0, n_ValA, n_ValF, ROR #8
    ADD     n_Tmp1, n_Tmp1, n_Tmp1, LSL #2        
    ADD     voAcc0, voAcc0, n_Tmp1       

    @// Calculate n_voACC1
    @// n_voACC1 = b - 5*c + 20*d + 20*e - 5*f + g
    UXTAB16 n_Tmp1, n_ValE, n_ValD, ROR #8
    UXTAB16 n_Tmp3, n_ValC, n_ValF, ROR #8
    RSB     n_Tmp1, n_Tmp3, n_Tmp1, LSL #2                        
    UXTAB16 n_ValG, r0x00ff00ff, n_ValH                 @// [00 g1 00 g0] + [0 255 0 255]
    ADD     n_Tmp1, n_Tmp1, n_Tmp1, LSL #2        
    UXTAB16 n_voACC1, n_ValG, n_ValB, ROR #8
    ADD     n_voACC1, n_voACC1, n_Tmp1        

    UXTAB16 n_voACC2, n_ValC, n_ValH, ROR #8        
    ADD     n_ValI, r0x00ff00ff, n_ValI                 @// [00 i1 00 i0] + [0 255 0 255]        
        
    @// Calculate n_voACC2
    @// n_voACC2 = c - 5*d + 20*e + 20*f - 5*g + h
    UXTAB16 n_Tmp1, n_ValG, n_ValD, ROR #8
    UXTAB16 n_voACC3, n_ValI, n_ValD, ROR #8
    UXTAB16 n_Tmp2, n_ValE, n_ValF, ROR #8
        
    RSB     n_Tmp1, n_Tmp1, n_Tmp2, LSL #2        
    UXTAB16 n_Tmp2, n_ValG, n_ValF, ROR #8
    ADD     n_Tmp1, n_Tmp1, n_Tmp1, LSL #2        
    ADD     n_voACC2, n_voACC2, n_Tmp1        

    @// Calculate n_voACC3
    @// n_voACC3 = d - 5*e + 20*f + 20*g - 5*h + i
    UXTAB16 n_Tmp1, n_ValE, n_ValH, ROR #8
    RSB     n_Tmp1, n_Tmp1, n_Tmp2, LSL #2
    ADD     n_Tmp1, n_Tmp1, n_Tmp1, LSL #2        
    ADD     n_voACC3, n_voACC3, n_Tmp1
        
    LDR     dstStep, [sp, #pDstStep]
    LDR     srcStep, [sp, #pSrcStep]

    @// If Counter is even store voAcc0-n_voACC3 in a temporary buffer
    @// If Counter is off store voAcc0-n_voACC3 and previous voAcc0-n_voACC3 in a intermediate buf 
    ANDS        n_Tmp3, Counter, #1
    BEQ         add_NoProcessing        
        
    @// Packing previous and current voAcc0-n_voACC3 values
@    M_LDRD      Accx, Accy, pTempResult1
    LDRD        Accx, Accy, [sp, #pTempResult1]
    PKHBT       n_Tmp6, Accx, voAcc0, LSL #16          @//[0 a2 0 a0] = [0 a3 0 a2] [0 a1 0 a0]
    PKHTB       voAcc0, voAcc0, Accx, ASR #16           @//[0 a3 0 a1] = [0 a1 0 a0] [0 a3 0 a2] 
    STR         voAcc0, [pDst, dstStep]                        
    STR         n_Tmp6, [pDst], #4                   
    PKHBT       n_Tmp6, Accy, n_voACC1, LSL #16          @//[0 b2 0 b0] = [0 b3 0 b2] [0 b1 0 b0]
    PKHTB       n_voACC1, n_voACC1, Accy, ASR #16            @//[0 b3 0 b1] = [0 b1 0 b0] [0 b3 0 b2]
@    M_LDRD     Accx, Accy, pTempResult2
    LDRD         Accx, Accy, [sp, #pTempResult2]
    STR         n_voACC1, [pDst, dstStep]                        
    STR         n_Tmp6, [pDst], #4                   
        
    PKHBT       n_Tmp6, Accx, n_voACC2, LSL #16          @//[0 c2 0 c0] = [0 c3 0 c2] [0 c1 0 c0]
    PKHTB       n_voACC2, n_voACC2, Accx, ASR #16            @//[0 c3 0 c1] = [0 c1 0 c0] [0 c3 0 c2]
    STR         n_voACC2, [pDst, dstStep]                        
    STR         n_Tmp6, [pDst], #4                   
    PKHBT       n_Tmp6, Accy, n_voACC3, LSL #16          @//[0 d2 0 d0] = [0 d3 0 d2] [0 d1 0 d0]
    PKHTB       n_voACC3, n_voACC3, Accy, ASR #16            @//[0 d3 0 d1] = [0 d1 0 d0] [0 d3 0 d2]
    STR         n_voACC3, [pDst, dstStep]                        
    STR         n_Tmp6, [pDst], #-12
    ADD         pDst, pDst, dstStep, LSL #1                   
    B           add_AfterStore

add_NoProcessing:
@    M_STRD      voAcc0, n_voACC1, pTempResult1
     STRD        voAcc0, n_voACC1, [sp, #pTempResult1]
@    M_STRD      n_voACC2, n_voACC3, pTempResult2
     STRD        n_voACC2, n_voACC3, [sp, #pTempResult2]
add_AfterStore:
    SUBS        Counter, Counter, #1                @// Loop till n_h is 10
    ADD         pSrc, pSrc, srcStep, LSL #1
    BPL         add_HeightLoop

    STR         voAcc0, [pDst], #4                    @//[0 a1 0 a0]
    STR         n_voACC1, [pDst], #4
    STR         n_voACC2, [pDst], #4
    STR         n_voACC3, [pDst], #-12
        
    @//
    @// Horizontal interpolation using multiplication
    @//
    
    SUB         pSrc, pDst, dstStep, LSL #2
    MOV         srcStep, #16
@    M_LDRD      pDst, dstStep, ppDstArgs
@    LDRD      pDst, dstStep, [sp, #ppDstArgs]
    LDR      pDst,     [sp, #ppDst]
    LDR      dstStep,  [sp, #ppDstStepArgs]

    MOV         Counter, #4
    LDR         r0x0014fffb, =0x0014fffb
    LDR         r0x00140001, =0x00140001

add_HeightLoop1:
@    M_STR       Counter, pCounter
    STR         Counter, [sp, #pCounter]

    LDR       ValCA, [pSrc], srcStep               @// Load  [0 c 0 a]
    LDR       ValDB, [pSrc], srcStep               @// Load  [0 d 0 b]
    LDR       ValGE, [pSrc], srcStep               @// Load  [0 g 0 e]
    LDR       ValHF, [pSrc], srcStep               @// Load  [0 h 0 f]

    LDR         voAcc0, [sp, #pVerOffset]
    SUBS        voAcc0 ,voAcc0, #1
    BCS         add_vOr23Else        
add_vOr23If:
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
    LDRH        ValCA, [pSrc], #4                   @// 8 = srcStep - 16
    SMLABB      voAcc0, ValHF, r0x00140001, voAcc0      @// voAcc0 += [0 h 0 f] * [0 0 0 1]        
    SMLABB      n_voACC1, ValHF, r0x0014fffb, n_voACC1      @// n_voACC1 += [0 h 0 f] * [0 0 0 -5]
    SMLATB      n_voACC2, ValDB, r0x0014fffb, n_voACC2      @// n_voACC2 += [0 d 0 b] * [0 0 0 -5]        
    SMLABB      n_voACC3, ValCA, r0x00140001, n_voACC3      @// n_voACC3 += [0 d 0 b] * [0 0 0 1]          
    
    PKHTB       ValDB, ValDB, ValHF 
    B           add_vOr23Endif
add_vOr23Else:       
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
   
    LDRH        ValCA, [pSrc], #4                   @// 8 = srcStep - 16
    SMLABB      voAcc0, ValHF, r0x00140001, voAcc0      @// voAcc0 += [0 h 0 f] * [0 0 0 1]        
    SMLABB      n_voACC1, ValHF, r0x0014fffb, n_voACC1      @// n_voACC1 += [0 h 0 f] * [0 0 0 -5]
    SMLATB      n_voACC2, ValDB, r0x0014fffb, n_voACC2      @// n_voACC2 += [0 d 0 b] * [0 0 0 -5]        
    SMLABB      n_voACC3, ValCA, r0x00140001, n_voACC3      @// n_voACC3 += [0 d 0 b] * [0 0 0 1]          
    
    PKHTB       ValHF, ValDB, ValHF                 @  [0 d 0 f]
    MOV         ValDB, ValGE, ROR #16               @  [0 e 0 g] 
    MOV         ValGE, ValHF                        @  [0 d 0 f]
add_vOr23Endif:
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
    UQSUB16      ValGE, ValGE, r0x0001fc00
    UQSUB16      ValDB, ValDB, r0x0001fc00
          
    LDR         r0x0001fc00, = 0x01FF01FF
    AND         ValGE, r0x0001fc00, ValGE, LSR #5  @  [0 c 0 e]
    AND         ValHF, r0x0001fc00, ValDB, LSR #5  @  [0 d 0 f]
    USAT16      ValGE, #8, ValGE
    USAT16      ValHF, #8, ValHF
@    MOV     ValGE,  ValGE, LSR #5  ;  [0 c 0 e]
@    MOV     ValHF,  ValDB, LSR #5  ;  [0 d 0 f]
@    USAT16      ValGE, #8, ValGE
@    USAT16      ValHF, #8, ValHF
    UXTB        ValCA, ValGE, ROR #16
    UXTB        ValDB, ValHF, ROR #16
    UXTB        ValGE, ValGE
    UXTB        ValHF, ValHF
     
    MOV         voAcc0, voAcc0, LSR #10 
    UHSUB8      ValCA, voAcc0, ValCA 
    USUB8       voAcc0, voAcc0, ValCA  
    LDR         ValCA, [pDst]
    UHSUB8      ValCA, voAcc0, ValCA
    USUB8       voAcc0,  voAcc0, ValCA
    STRB        voAcc0, [pDst], dstStep
    
    MOV         n_voACC1, n_voACC1, LSR #10
    UHSUB8      ValDB, n_voACC1, ValDB 
    USUB8       n_voACC1, n_voACC1, ValDB
    LDR         ValDB, [pDst]
    UHSUB8      ValDB, n_voACC1, ValDB
    USUB8       n_voACC1,  n_voACC1, ValDB    
    STRB        n_voACC1, [pDst], dstStep
    
    MOV         n_voACC2, n_voACC2, LSR #10
    UHSUB8      ValGE, n_voACC2, ValGE 
    USUB8       n_voACC2, n_voACC2, ValGE 
    LDR         ValGE, [pDst]
    UHSUB8      ValGE, n_voACC2, ValGE
    USUB8       n_voACC2,  n_voACC2, ValGE       
    STRB        n_voACC2, [pDst], dstStep
    
    MOV         n_voACC3, n_voACC3, LSR #10
    UHSUB8      ValHF, n_voACC3, ValHF 
    USUB8       n_voACC3, n_voACC3, ValHF   
    LDR         ValHF, [pDst]
    UHSUB8      ValHF, n_voACC3, ValHF
    USUB8       n_voACC3,  n_voACC3, ValHF           
    STRB        n_voACC3, [pDst], dstStep


@    M_LDR       Counter, pCounter
    LDR         Counter, [sp, #pCounter]
    SUB         pDst, pDst, dstStep, LSL #2
    SUB         pSrc, pSrc, srcStep, LSL #2
    ADD         pDst, pDst, #1
    SUBS        Counter, Counter, #1
    BGT         add_HeightLoop1

    LDR     Count, [sp, #pCount]
    SUBS    Count, Count, #4<<28
    LDR     srcStep, [sp, #pSrcStep]
    LDR     pSrc,    [sp, #ppSrc]
    ADD     pSrc, pSrc, #4
    BCS     add_loop_x

    LDRD    pSrc, srcStep, [sp, #ppSrcArgs]  
    ADD     pSrc, pSrc, srcStep, LSL #2
    LDRD    pDst, dstStep, [sp, #ppDstArgs]
    ADD     pDst, pDst, dstStep, LSL #2
    AND     n_Tmp3, Count, #0x00F00000    @// partWidth-1
    ADDS    Count, Count, #(1<<28)-(4<<24)    
    BGE     add_loop_y  

    ADD     sp,sp,#0xC0
    LDMFD   sp!, {r4-r11, pc}       

	@.end
