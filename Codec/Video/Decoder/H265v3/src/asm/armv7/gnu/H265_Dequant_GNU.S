@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@;void H265_Dequant( const short* i_pSrc, 
@;							short* io_pDes,
@;							int iWidth, 
@;							stDequant * pDequant)
@;
@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        @;AREA |.text|, CODE, READONLY    
        @;IMPORT g_DeQuantScales
        @;EXPORT H265_Dequant_ASM
        @;align 4
        
        #include     "h265dec_ASM_config.h"
        #include "../../../h265dec_ID.h"
        .section .text
        .align 4
        .if DEQUANT_ASM_ENABLED==1
        .extern kg_dequant_scales
        .global DoDequantASMV7
        
DoDequantASMV7:   
        stmfd sp!, {r4-r11, lr}
        ldmia   r3, {r4 - r12} 			@;取出pDequant结构中的值
        cmp   r2, r5 						@;  if ( iWidth > (int)pDequant->uiMaxTrSize )
        movgt  r2, r5						@;  iWidth  = pDequant->uiMaxTrSize;
        mul    r2, r2, r2 			@; nPixels = iWidth * iWidth;
        @;ands   r12, r11, #0xff00  		@; if( ! pDequant->scalingListEnabledFlag)
        cmp  r12, #0  								@; if( ! pDequant->scalingListEnabledFlag)
        bne  H265_Dequant_ASM_scalingListEnabledFlag_True
        ldr    r12, = kg_dequant_scales
        @;and   r11, r11, #0xff 				@; uiLog2Width
        ldr    r7, [r12, r9, lsl #2] 		@; kg_dequant_scales[pDequant->iR]
        mov    r4, r7, lsl r8 						 	@; scale = kg_dequant_scales[pDequt->iRem] << pDequant->iPer;
        add    r5, r6, r11 			@; uiBitDepth + uiLog2Width
        @;sub    r5, r5, #-7  						@; iShift = uiBitDepth + uiLog2Width - 7; 
        rsb    r5, r5, #7 		@; -iShift
        vdup.16 q0, r4 					@; scale|scale|scale|scale|scale|scale|scale
        vdup.32 q1, r5 				@; -iShift|...|-iShift(8个)
        
H265_Dequant_ASM_For_n_1:        
        vld1.16  {d4, d5, d6, d7}, [r0]! 	@; 取16个数
        vmull.s16  q4, d4, d1
        vmull.s16  q5, d5, d1
        vqrshl.s32 q4, q4, q1 						@; piCoef[0~3]
        vqrshl.s32 q5, q5, q1 						@; piCoef[4~7]
        vmovn.s32  d8, q4
        vmovn.s32  d9, q5
        vmull.s16  q6, d6, d1
        vmull.s16  q7, d7, d1
        vqrshl.s32 q6, q6, q1 						@; piCoef[8~11]
        vqrshl.s32 q7, q7, q1 						@; piCoef[12~15]
        vmovn.s32  d10, q6
        vmovn.s32  d11, q7
        vst1.16  {d8,d9,d10,d11}, [r1]!
        subs    r2, r2, #16 							@; nPixels -=16
        bgt    H265_Dequant_ASM_For_n_1
        ldmfd sp!, {r4-r11, pc}
        
H265_Dequant_ASM_scalingListEnabledFlag_True:        
        @;and   r11, r11, #0xff				@; uiLog2Width
        add    r5, r6, r11 							@; uiBitDepth + uiLog2Width
        sub    r5, r5, #3 							@; iShift = uiBitDepth + uiLog2Width - 3;
        sub    r5, r8, r5 							@; pDequant->iPer - iShift
        mov    r12, #36
        mla    r7, r12, r11, r9  			@; uiLog2Width*36+pDequant->iRem
        mov    r12, #6
        mla    r7, r4, r12, r7 					
        ldr    r4, [r10, r7, lsl #2] 			@; piDequantCoef
        
        vdup.16 q1, r5  
        
H265_Dequant_ASM_For_n_2:        
        vld1.16  {d4, d5, d6, d7}, [r0]! 	  @; 取16个数piQCoef[n]
        vld1.16  {d8, d9, d10, d11}, [r4]! 	@; 取16个数piDequantCoef[n]
        vmull.s16 q6, d4, d8
        vmull.s16 q7, d5, d9
        vmull.s16 q8, d6, d10
        vmull.s16 q9, d7, d11
        vqrshl.s32 q6, q6, q1 						@; piCoef[0~3]
        vqrshl.s32 q7, q7, q1 						@; piCoef[4~7]
        vqrshl.s32 q8, q8, q1 						@; piCoef[8~11]
        vqrshl.s32 q9, q9, q1 						@; piCoef[12~15]
        vmovn.s32  d20, q6
        vmovn.s32  d21, q7
        vmovn.s32  d22, q8
        vmovn.s32  d23, q9
        vst1.16  {d20,d21,d22,d23}, [r1]!
        subs    r2, r2, #16 							@; nPixels -=16
        bgt    H265_Dequant_ASM_For_n_2

        ldmfd sp!, {r4-r11, pc}

        .endif    @.if IDCT_ASM_ENABLED==1
        .end