@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@;void IDCT16X16(
@;							const short *pSrcData,
@;							const unsigned char *pPerdictionData,
@;							unsigned char *pDstRecoData,
@;							unsigned int uiDstStride)
@;@;@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@; 
				@;include		h265dec_ASM_config.h
        @;include h265dec_idct_macro.inc
        @;area |.text|, code, readonly
        @;align 4
        @;if IDCT_ASM_ENABLED==1  
        @;import kg_IDCT_coef_for_t16_asm
        @;export IDCT16X16_4X4_ASMV7
        @;
        #include     "h265dec_ASM_config.h"
        @#include "../../../h265dec_config.h"
        .text
        .align 4
        .if IDCT_ASM_ENABLED==1
        @.globl kg_IDCT_coef_for_t16_asm
        .globl _IDCT16X16_4X4_ASMV7
 kg_IDCT_coef_for_t16_asm:	 
	 .short     64, 83, 64, 36, 89, 75, 50, 18
   .short     90, 87, 80, 70, 57, 43, 25, 9
	                
        
_IDCT16X16_4X4_ASMV7:  
        stmfd sp!, {r4, r5, r6, r7,r8,r9, r10, lr}
        @; g_IDCT16X16H_EEE,g_IDCT16X16H_EO,g_IDCT16X16H_O
        adr   r12, kg_IDCT_coef_for_t16_asm
        vld1.16  {d0, d1, d2, d3},  [r12]
        @; add   r4, r5, #96											@; pTmpBlock+12*16
        @;mov  r4, #PRED_CACHE_STRIDE
        ldr  r4, [sp, #32] 
        mov  r5, r0 										@; pSrc,也存放临时空间
        mov   r6, #32   			 				 	  @; pSrc跨度
        
        
        
        vld1.16  {d4},  [r0], r6 							@;pSrc[0]...[3]
        vld1.16  {d20},  [r0], r6 							@;pSrc[2*16]...[3]
        vld1.16  {d5},  [r0], r6 							@;pSrc[1*16]...[3]
        vld1.16  {d21},  [r0], r6 							@;pSrc[3*16]...[3]
        
        vmull.s16 q15, d4, d0[0] 							@; midValue = 64 * pSrc[0]
        
        vmull.s16 q6, d5, d1[0] 							@; EO[0]   
        vmull.s16 q7, d5, d1[1] 							@; EO[1]       
        vmull.s16 q8, d5, d1[2] 							@; EO[2]     
        vmull.s16 q9, d5, d1[3] 							@; EO[3]
        
        vadd.s32  q14, q15, q6 								@; E[0]
        vadd.s32  q13, q15, q7 								@; E[1]
        vadd.s32  q4, q15, q8 								@; E[2]
        vadd.s32  q5, q15, q9 								@; E[3]
        
        vsub.s32  q6, q15, q6 								@; E[7]
        vsub.s32  q7, q15, q7 								@; E[6]
        vsub.s32  q8, q15, q8 								@; E[5]
        vsub.s32  q9, q15, q9 								@; E[4]
        
        vmull.s16 q15, d20, d2[0]
        vmlal.s16 q15, d21, d2[1] 							@; O[0]
        
        
        vadd.s32  q11, q14, q15  								@; E[0] + O[0]
        vsub.s32  q15, q14, q15  								@; E[0] - O[0]
        vqrshrn.s32 d4, q11, #7 								@; pTmpBlock[00~03]
        vqrshrn.s32 d29, q15,#7 								@; pTmpBlock[150~153]      
        
        vmull.s16 q15, d20, d2[1]
        vmlal.s16 q15, d21, d3[0] 							@; O[1]
        
        vadd.s32  q11,  q13, q15  								@; E[1] + O[1]
        vsub.s32  q15,  q13, q15  								@; E[1] - O[1]
        vqrshrn.s32 d5, q11, #7  								@; pTmpBlock[10~13]
        vqrshrn.s32 d28,q15, #7 								@; pTmpBlock[140~143]
        
        vmull.s16 q12, d20, d2[2]
        vmlal.s16 q12, d21, d3[3] 							@; O[2]
        
        vadd.s32  q15, q4, q12  								@; E[2] + O[2]
        vsub.s32  q12,  q4, q12  								@; E[2] - O[2]
        vqrshrn.s32 d6, q15, #7 								@; pTmpBlock[20~23]
        vqrshrn.s32 d27,q12, #7 								@; pTmpBlock[130~133]
        
        
        vmull.s16 q4, d20, d2[3]
        vmlsl.s16 q4, d21, d3[1] 								@; O[3]
        
        vadd.s32  q15, q5, q4  								@; E[3] + O[3]
        vsub.s32  q5,  q5, q4  								@; E[3] - O[3]
        vqrshrn.s32 d7, q15, #7 								@; pTmpBlock[30~33]
        vqrshrn.s32 d26, q5, #7 								@; pTmpBlock[120~123]
        
        @; 转置存储0~3行和12~15行的4个元素
        mov    r6, #96
        vst4.16 {d4, d5, d6, d7}, [r5], r6
        mov    r6, #-64
        @;sub    r6, r6, #1280
        vst4.16 {d26, d27, d28, d29}, [r5],r6        
      
        vmull.s16 q5, d20, d3[0]
        vmlsl.s16 q5, d21, d2[2] 							@; O[4]
        
        vadd.s32  q15, q9, q5  								@; E[4] + O[4]
        vsub.s32  q5,  q9, q5  								@; E[4] - O[4]
        vqrshrn.s32 d8, q15, #7 								@; pTmpBlock[40~43]
        vqrshrn.s32 d25,q5,  #7 								@; pTmpBlock[110~113]
                
        vmull.s16 q9, d20, d3[1]
        vmlsl.s16 q9, d21, d2[0] 							@; O[5]
        
        vadd.s32  q15, q8, q9  								@; E[5] + O[5]
        vsub.s32  q9,  q8, q9  								@; E[5] - O[5]
        vqrshrn.s32 d9, q15, #7 								@; pTmpBlock[50~53]
        vqrshrn.s32 d24,q9,  #7 								@; pTmpBlock[100~103]
        
        vmull.s16 q9, d20, d3[2]
        vmlsl.s16 q9, d21, d2[3]							@; O[6]
        
        vadd.s32  q15, q7, q9  								@; E[6] + O[6]
        vsub.s32  q9,  q7, q9  								@; E[6] - O[6]
        vqrshrn.s32 d10, q15, #7 								@; pTmpBlock[60~63]
        vqrshrn.s32 d23, q9,  #7 								@; pTmpBlock[90~93]
        
        vmull.s16 q9, d20, d3[3]
        vmlsl.s16 q9, d21, d3[2] 							@; O[7]
        
        vadd.s32  q15, q6, q9  								@; E[7] + O[7]
        vsub.s32  q9,  q6, q9  								@; E[7] - O[7]
        vqrshrn.s32 d11, q15, #7 								@; pTmpBlock[70~73]
        vqrshrn.s32 d22,  q9,  #7								@; pTmpBlock[80~83]
        
         @; 转置存储4~7行和8~11行的4个元素
        vst4.16 {d8, d9, d10, d11}, [r5]!
        @;mov  r6, #-512
        vst4.16 {d22, d23, d24, d25}, [r5],r6
        
        @; ==============第二次变换+预测值
        @; r0 == r5
        mov  r12, #12
        add  r6, r4, r4, asl #1 						@; 3*predStride
        sub  r6, r12, r6					 						@; -3*predStride + 12
        add  r8, r3, r3, asl #1 						@; 3*uiDstStride
        sub  r8, r12, r8 				 						@; 3*uiDstStride + 12
        mov  r0, #8
        sub  r9, r6, #20 						@; -3*predStride - 8
        sub  r10,r8, #20 						@; -3*uiDstStride - 8
        
        mov  r7, #4    @; j = 16/4
IDCT16X16_4X4_ASMV7_loop2:       
        @; d4~d7:pSrc[00~03]...[30~33]
        vld1.16  {d4, d5, d6, d7}, [r5]!
               
        vmull.s16 q15, d4, d0[0] 							@; midValue = 64 * pSrc[0]
        
        vmull.s16 q6, d6, d1[0] 							@; EO[0]   
        vmull.s16 q7, d6, d1[1] 							@; EO[1]       
        vmull.s16 q8, d6, d1[2] 							@; EO[2]     
        vmull.s16 q9, d6, d1[3] 							@; EO[3]
        
        vadd.s32  q14, q15, q6 								@; E[0]
        vadd.s32  q13, q15, q7 								@; E[1]
        vadd.s32  q12, q15, q8 								@; E[2]
        vadd.s32  q11, q15, q9 								@; E[3]
        
        vsub.s32  q10, q15, q6 								@; E[7]
        vsub.s32  q7, q15, q7 								@; E[6]
        vsub.s32  q8, q15, q8 								@; E[5]
        vsub.s32  q9, q15, q9 								@; E[4]
        
        vmull.s16 q15, d5, d2[0]
        vmlal.s16 q15, d7, d2[1] 							@; O[0]
        
        vadd.s32  q4, q14, q15  								@; E[0] + O[0]
        vsub.s32  q5, q14, q15  								@; E[0] - O[0]
        vqrshrn.s32 d8, q4, #12 								@; pTmpBlock[00~03]
        vqrshrn.s32 d31, q5,#12 								@; pTmpBlock[150~153]      

        vmull.s16 q14, d5, d2[1]
        vmlal.s16 q14, d7, d3[0] 							@; O[1]
        
        vadd.s32  q5,  q13, q14  								@; E[1] + O[1]
        vsub.s32  q6,  q13, q14  								@; E[1] - O[1]
        vqrshrn.s32 d9, q5, #12  								@; pTmpBlock[10~13]
        vqrshrn.s32 d30,q6, #12 								@; pTmpBlock[140~143]
        
        vmull.s16 q13, d5, d2[2]
        vmlal.s16 q13, d7, d3[3] 							@; O[2]
        
        vadd.s32  q5, q12, q13  								@; E[2] + O[2]
        vsub.s32  q6,  q12, q13  								@; E[2] - O[2]
        vqrshrn.s32 d10, q5, #12 								@; pTmpBlock[20~23]
        vqrshrn.s32 d29, q6, #12 								@; pTmpBlock[130~133]
        
        vmull.s16 q12, d5, d2[3]
        vmlsl.s16 q12, d7, d3[1] 								@; O[3]
        
        vadd.s32  q6, q11, q12  								@; E[3] + O[3]
        vsub.s32  q12,q11, q12  								@; E[3] - O[3]
        vqrshrn.s32 d11, q6, #12 								@; pTmpBlock[30~33]
        vqrshrn.s32 d28,q12, #12 								@; pTmpBlock[120~123]
        
        @; 转置存储0~3行和12~15行的4个元素
        vtrn.16  d8, d9
        vtrn.16  d10, d11
        vtrn.32  q4, q5
        
        vtrn.16  d28, d29
        vtrn.16  d30, d31
        vtrn.32  q14, q15         
        
        vld1.32   {d12[0]}, [r1],r4							@; pPerdiction[00~03]
        vld1.32   {d12[1]}, [r1],r4							@; pPerdiction[10~13]
        vaddw.u8   q4, q4, d12 									
        vqmovun.s16 d8, q4 											@; p_reconstruction[00~03,10~13]
        vst1.32   {d8[0]}, [r2], r3
        vst1.32   {d8[1]}, [r2], r3
        
        vld1.32   {d12[0]}, [r1],r4							@; pPerdiction[20~23]
        vld1.32   {d12[1]}, [r1],r6							@; pPerdiction[30~33]
        vaddw.u8   q4, q5, d12 									
        vqmovun.s16 d8, q4 											@; p_reconstruction[20~23,30~33]
        vst1.32   {d8[0]}, [r2], r3
        vst1.32   {d8[1]}, [r2], r8
        
        @; p_reconstruction[120~123.150~153] = pPerdiction[120~123.150~153] + resi[120~123.150~153]
        vld1.32   {d12[0]}, [r1],r4							@; pPerdiction[120~123]
        vld1.32   {d12[1]}, [r1],r4							@; pPerdiction[130~133]
        vaddw.u8   q4, q14, d12 									
        vqmovun.s16 d8, q4 											@; p_reconstruction[120~123,130~133]
        vst1.32   {d8[0]}, [r2], r3
        vst1.32   {d8[1]}, [r2], r3
        
        vld1.32   {d12[0]}, [r1],r4							@; pPerdiction[140~143]
        vld1.32   {d12[1]}, [r1],r9							@; pPerdiction[150~153]
        vaddw.u8   q4, q15, d12 									
        vqmovun.s16 d8, q4 											@; p_reconstruction[140~143,150~153]
        vst1.32   {d8[0]}, [r2], r3
        vst1.32   {d8[1]}, [r2], r10
        
        vmull.s16 q11, d5, d3[0]
        vmlsl.s16 q11, d7, d2[2] 							@; O[4]
        
        vadd.s32  q12, q9, q11  								@; E[4] + O[4]
        vsub.s32  q13, q9, q11  								@; E[4] - O[4]
        vqrshrn.s32 d8, q12, #12 								@; pTmpBlock[40~43]
        vqrshrn.s32 d31,q13, #12 								@; pTmpBlock[110~113]
                
        vmull.s16 q11, d5, d3[1]
        vmlsl.s16 q11, d7, d2[0] 							@; O[5]
        
        vadd.s32  q12, q8, q11  								@; E[5] + O[5]
        vsub.s32  q13, q8, q11  								@; E[5] - O[5]
        vqrshrn.s32 d9, q12, #12								@; pTmpBlock[50~53]
        vqrshrn.s32 d30,q13, #12 								@; pTmpBlock[100~103]
        
        vmull.s16 q11, d5, d3[2]
        vmlsl.s16 q11, d7, d2[3]							@; O[6]
        
        vadd.s32  q12, q7, q11  								@; E[6] + O[6]
        vsub.s32  q13,  q7, q11  								@; E[6] - O[6]
        vqrshrn.s32 d10, q12, #12								@; pTmpBlock[60~63]
        vqrshrn.s32 d29, q13, #12 								@; pTmpBlock[90~93]
        
        vmull.s16 q11, d5, d3[3]
        vmlsl.s16 q11, d7, d3[2] 							@; O[7]
        
        vadd.s32  q12, q10, q11  								@; E[7] + O[7]
        vsub.s32  q13, q10, q11  								@; E[7] - O[7]
        vqrshrn.s32 d11, q12, #12								@; pTmpBlock[70~73]
        vqrshrn.s32 d28, q13, #12								@; pTmpBlock[80~83]
        
        @; 转置存储4~7行和8~11行的4个元素        
        vtrn.16  d8, d9
        vtrn.16  d10, d11
        vtrn.32  q4, q5
        
        vtrn.16  d28, d29
        vtrn.16  d30, d31
        vtrn.32  q14, q15
        
        vld1.32   {d12[0]}, [r1],r4							@; pPerdiction[40~43]
        vld1.32   {d12[1]}, [r1],r4							@; pPerdiction[50~53]
        vaddw.u8   q4, q4, d12 									
        vqmovun.s16 d8, q4 											@; p_reconstruction[40~43,50~53]
        vst1.32   {d8[0]}, [r2], r3
        vst1.32   {d8[1]}, [r2], r3
        
        vld1.32   {d12[0]}, [r1],r4							@; pPerdiction[60~63]
        vld1.32   {d12[1]}, [r1],r6							@; pPerdiction[70~73]
        vaddw.u8   q4, q5, d12 									
        vqmovun.s16 d8, q4 											@; p_reconstruction[60~63,70~73]
        vst1.32   {d8[0]}, [r2], r3
        vst1.32   {d8[1]}, [r2], r8
        
        @; p_reconstruction[80~83.110~113] = pPerdiction[80~83.110~113] + resi[80~83.110~113]
        sub  r1, r1, r0  	@; +12
        vld1.32   {d12[0]}, [r1], r4							@; pPerdiction[80~83]
        vld1.32   {d12[1]}, [r1], r4							@; pPerdiction[90~93]
        vaddw.u8   q4, q14, d12 									
        vqmovun.s16 d8, q4 											@; p_reconstruction[80~83,90~93]
        sub   r2, r2, r0
        vst1.32   {d8[0]}, [r2], r3
        vst1.32   {d8[1]}, [r2], r3
        
        vld1.32   {d12[0]}, [r1], r4							@; pPerdiction[100~103]
        vld1.32   {d12[1]}, [r1], r6							@; pPerdiction[110~113]
        vaddw.u8   q4, q15, d12 									
        vqmovun.s16 d8, q4 											@; p_reconstruction[100~103,110~113]
        vst1.32   {d8[0]}, [r2], r3
        vst1.32   {d8[1]}, [r2], r8      
        
        sub  r1, r1, #20
        sub  r2, r2, #20
        add  r1, r1, r4, asl #2												@; pPerdiction[04~07]
        add  r2, r2, r3, asl #2
        subs r7, r7, #1
        bgt  IDCT16X16_4X4_ASMV7_loop2
        
        ldmfd sp!, {r4, r5, r6, r7,r8,r9, r10,pc}
        
        

        .endif											@;if IDCT_ASM_ENABLED==1
        @.end