@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@;使用矩阵乘法实现：
@;void IDST4X4(
@;							const short *pSrcData,
@;							const unsigned char *pPerdictionData,
@;							unsigned char *pDstRecoData,
@;							unsigned int uiDstStride)
@; 按照列向量的顺序排列数组：
@; short g_arrS16IDST4X4[16] = 
@; {
@;     29,55,74,84,  
@;     74,74,0,-74,    
@;     84,-29,-74,55,    
@;     55,-84,74,-29
@; };
@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        @;AREA |.text|, CODE, READONLY    
        @;IMPORT g_uiBitIncrement = 0
        @;IMPORT g_arrS16IDST4X4
        @;EXPORT IDST4X4_ASM
        @;align 4
        #include     "h265dec_ASM_config.h"
        @#include "../../../h265dec_config.h"
        .section .text
        .align 4
        .if IDCT_ASM_ENABLED==1
        .extern kg_IDST_coef_for_t4_asm
        .global IDST4X4ASMV7
IDST4X4ASMV7: 
        
        vld1.16  {d0, d1, d2, d3}, [r0] 		@; psrcdata:d0,d1,d2,d3??
        
        ldr       r12, = kg_IDST_coef_for_t4_asm
        vld1.16  {d14, d15, d16, d17}, [r12] 		@; kg_IDST_coef_for_t4_as
        
        vmull.s16 q4, d14, d0[0] 						@; 第一列的乘法
        vmlal.s16 q4, d15, d1[0] 						@; 第一列加上第二列的乘法
        vmlal.s16 q4, d16, d2[0] 						@; 加上第三列乘法
        vmlal.s16 q4, d17, d3[0] 						@; 加上第四列乘法
        vqrshrn.s32 d4, q4, #7 						@; 下一次循环的第一行
        
        vmull.s16 q4, d14, d0[1] 						@; 第一列的乘法
        vmlal.s16 q4, d15, d1[1] 						@; 第一列加上第二列的乘法
        vmlal.s16 q4, d16, d2[1] 						@; 加上第三列乘法
        vmlal.s16 q4, d17, d3[1] 						@; 加上第四列乘法
        vqrshrn.s32 d5, q4, #7 						@; 下一次循环的第二行
        
        vmull.s16 q4, d14, d0[2] 						@; 第一列的乘法
        vmlal.s16 q4, d15, d1[2] 						@; 第一列加上第二列的乘法
        vmlal.s16 q4, d16, d2[2] 						@; 加上第三列乘法
        vmlal.s16 q4, d17, d3[2] 						@; 加上第四列乘法
        vqrshrn.s32 d6, q4, #7 						@; 下一次循环的第一行
        
        vmull.s16 q4, d14, d0[3] 						@; 第一列的乘法
        vmlal.s16 q4, d15, d1[3] 						@; 第一列加上第二列的乘法
        vmlal.s16 q4, d16, d2[3] 						@; 加上第三列乘法
        vmlal.s16 q4, d17, d3[3] 						@; 加上第四列乘法
        vqrshrn.s32 d7, q4, #7 						@; 下一次循环的第一行
        
        @; 第二个循环的源矩阵为d10,d11,d12,d13
        vmull.s16 q4, d14, d4[0] 						@; 第一列的乘法
        vmlal.s16 q4, d15, d5[0] 						@; 第一列加上第二列的乘法
        vmlal.s16 q4, d16, d6[0] 						@; 加上第三列乘法
        vmlal.s16 q4, d17, d7[0] 						@; 加上第四列乘法
        vqrshrn.s32 d0, q4, #12 						@; 变换后的残差的第一行
        
        vmull.s16 q4, d14, d4[1] 						@; 第一列的乘法
        vmlal.s16 q4, d15, d5[1] 						@; 第一列加上第二列的乘法
        vmlal.s16 q4, d16, d6[1] 						@; 加上第三列乘法
        vmlal.s16 q4, d17, d7[1] 						@; 加上第四列乘法
        vqrshrn.s32 d1, q4, #12 						@; 变换后的残差的第二行
        
        vmull.s16 q4, d14, d4[2] 						@; 第一列的乘法
        vmlal.s16 q4, d15, d5[2] 						@; 第一列加上第二列的乘法
        vmlal.s16 q4, d16, d6[2] 						@; 加上第三列乘法
        vmlal.s16 q4, d17, d7[2] 						@; 加上第四列乘法
        vqrshrn.s32 d2, q4, #12 						@; 变换后的残差的第三行
        
        vmull.s16 q4, d14, d4[3] 						@; 第一列的乘法
        vmlal.s16 q4, d15, d5[3] 						@; 第一列加上第二列的乘法
        vmlal.s16 q4, d16, d6[3] 						@; 加上第三列乘法
        vmlal.s16 q4, d17, d7[3] 						@; 加上第四列乘法
        vqrshrn.s32 d3, q4, #12 						@; 变换后的残差的第四行
        
        @;mov      r12, #PRED_CACHE_STRIDE 					@;uipredstride = 132
        ldr      r12, [sp, #0]						@; predStride
        vld1.32  {d14[0]}, [r1], r12 			@; pperdiction[0]
        vld1.32  {d14[1]}, [r1], r12 			@; pperdiction[1]
        vld1.32  {d16[0]}, [r1], r12 			@; pperdiction[2]
        vld1.32  {d16[1]}, [r1] 					@; pperdiction[3]
        
        vaddw.u8    q12, q0, d14
        vaddw.u8    q13, q1, d16

        vqmovun.s16  d0, q12
        vqmovun.s16  d1, q13

        @; store the output data
        vst1.32      {d0[0]}, [r2],r3
        vst1.32      {d0[1]}, [r2],r3
        vst1.32      {d1[0]}, [r2],r3
        vst1.32      {d1[1]}, [r2]
        
        @;ldmia sp!, {r4,r5,r12,pc}      
        mov pc, lr
        
        .endif    @.if IDCT_ASM_ENABLED==1        
        .end
       