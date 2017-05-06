@@@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@;void IDCT8X8(
@;							const short *pSrcData,
@;							const unsigned char *pPerdictionData,
@;							unsigned char *pDstRecoData,
@;							unsigned int uiDstStride)
@;
@; short g_IDCT8X8Half_1[8] = {89,75,50,18,  64,83,64,36};
@@@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@; 242 cycles
        @;AREA |.text|, CODE, READONLY    
        @;IMPORT g_uiBitIncrement = 0
        @;IMPORT g_IDCT8X8Half_1
        @;EXPORT IDCT8X8_ASM
        @;align 4
        #include     "h265dec_ASM_config.h"
        #include "../../../h265dec_ID.h" 
        @#include "../../../h265dec_config.h"
        .text
        .align 4
        .if IDCT_ASM_ENABLED==1
        @.extern kg_IDCT_coef_for_t8_asm
        .globl _IDCT8X8ASMV7
kg_IDCT_coef_for_t8_asm:
  .short 89, 75, 50, 18, 64, 83, 64, 36  
  
_IDCT8X8ASMV7:
        @;stmdb sp!, {r4,r5,r12,lr}
        adr   r12, kg_IDCT_coef_for_t8_asm
        vld1.16   {d0, d1}, [r12]     	@; {89,75,50,18,  64,83,64,36};
        @;mov   r12, r0
        vld1.16   {d2, d3, d4, d5},       [r0]! 			@; pSrc[0],pSrc[8]
        vld1.16   {d6, d7, d8, d9},       [r0]! 			@; pSrc[16],pSrc[24]
        vld1.16   {d10, d11, d12,d13},     [r0]! 			@; pSrc[32],pSrc[40]
        vld1.16   {d14, d15, d16,d17},    [r0] 			 @; pSrc[48],pSrc[56]
        
        vmull.s16 q9,  d6, d1[1]							@; 83*pSrc[ 2*8 ]
        vmull.s16 q10, d7, d1[1]							@; 83*pSrc[ 2*8 ]
        vmlal.s16 q9,  d14, d1[3]							@; + 36*pSrc[ 6*8 ] =EO[0]
        vmlal.s16 q10, d15, d1[3]							@; + 36*pSrc[ 6*8 ] =EO[0]
        
        vmull.s16 q11,  d6, d1[3]							@; 36*pSrc[ 2*8 ]
        vmull.s16 q12,  d7, d1[3]							@; 36*pSrc[ 2*8 ]
        vmlsl.s16 q11,  d14, d1[1]							@; - 83*pSrc[ 6*8 ] =EO[1]
        vmlsl.s16 q12,  d15, d1[1]							@; - 83*pSrc[ 6*8 ] =EO[1]
       
        vmull.s16  q13,  d2, d1[0] 					@; 64*pSrc[ 0   ]
       vmlal.s16  q13,  d10,d1[0] 					@; +64*pSrc[ 4*8   ]=EE[0]
       vmull.s16  q14,  d3, d1[0] 					@; 64*pSrc[ 0   ]
       vmlal.s16  q14,  d11,d1[0] 					@; +64*pSrc[ 4*8   ]=EE[0]
               
       vmull.s16  q15,  d2, d1[0] 					@; 64*pSrc[ 0   ]
       vmlsl.s16  q15,  d10,d1[0] 					@; -64*pSrc[ 4*8   ]=EE[1]
       vmull.s16  q3,  d3, d1[0] 					@; 64*pSrc[ 0   ]
       vmlsl.s16  q3,  d11,d1[0] 					@; -64*pSrc[ 4*8   ]=EE[1]
        vmov       q1, q15
        
        vadd.s32  q5, q13, q9 							@; E[0] = EE[0] + EO[0];
        vadd.s32  q7, q14, q10 							@; E[0] = EE[0] + EO[0];
        vsub.s32  q9, q13, q9 							@; E[3] = EE[0] - EO[0];
        vsub.s32  q10, q14, q10 						@; E[3] = EE[0] - EO[0];
        
        vadd.s32  q13, q1, q11 							@;  E[1] = EE[1] + EO[1];
        vadd.s32  q14, q3, q12 							@;  E[1] = EE[1] + EO[1];
        vsub.s32  q11, q1, q11 							@;  E[2] = EE[1] - EO[1];
        vsub.s32  q12, q3, q12 							@;  E[2] = EE[1] - EO[1];
        
        vmull.s16 q1,  d4, d0[0]							@; 89*pSrc[8]
        vmull.s16 q3,  d5, d0[0]							@; 89*pSrc[8]
        vmlal.s16 q1,  d8, d0[1]							@; + 75*pSrc[3*8]
        vmlal.s16 q3,  d9, d0[1]							@; + 75*pSrc[3*8]
        vmlal.s16 q1,  d12, d0[2]							@; + 50*pSrc[5*8]
        vmlal.s16 q3,  d13, d0[2]							@; + 50*pSrc[5*8]
        vmlal.s16 q1,  d16, d0[3]							@; + 18*pSrc[7*8] = o[0]
        vmlal.s16 q3,  d17, d0[3]							@; + 18*pSrc[7*8] = o[0]
        
        vadd.s32     q15, q5, q1 						@; E[k] + O[k] (0~3)
        vsub.s32     q5, q5, q1 						@; E[k] - O[k] (0~3)
        vqrshrn.s32  d2, q15, #7 						@; pTmpBlock[0](0~3)
        vqrshrn.s32  d10, q5, #7 						@; pTmpBlock[7](0~3)
        
        vadd.s32     q15, q7, q3 						@; E[k] + O[k] (4~7)
        vsub.s32     q3, q7, q3 						@; E[k] - O[k] (4~7)
        vqrshrn.s32  d3, q15, #7 						@; pTmpBlock[0](4~7)
        vqrshrn.s32  d11, q3, #7 						@; pTmpBlock[7](4~7)
        
        vmull.s16 q3, d4, d0[1]							@; 75*pSrc[8]
        vmull.s16 q7, d5, d0[1]							@; 75*pSrc[8]
        vmlsl.s16 q3, d8, d0[3]							@; - 18*pSrc[3*8]
        vmlsl.s16 q7, d9, d0[3]							@; - 18*pSrc[3*8]
        vmlsl.s16 q3, d12, d0[0]						  @; - 89*pSrc[5*8]
        vmlsl.s16 q7, d13, d0[0]							@; - 89*pSrc[5*8]
        vmlsl.s16 q3, d16, d0[2]							@; - 50*pSrc[7*8]; = o[1]
        vmlsl.s16 q7, d17, d0[2]							@; - 50*pSrc[7*8]; = o[1]
        
        vadd.s32     q15, q13, q3 						@; E[1] + O[1] (0~3)
        vsub.s32     q13, q13, q3 						@; E[1] - O[1] (0~3)
        vqrshrn.s32  d6,  q15, #7 						@; pTmpBlock[1](0~3)
        vqrshrn.s32  d26, q13, #7 						@; pTmpBlock[6](0~3)
        
        vadd.s32     q15, q14, q7 						@; E[1] + O[1] (4~7)
        vsub.s32     q14, q14, q7 						@; E[1] - O[1] (4~7)
        vqrshrn.s32  d7,  q15, #7 						@; pTmpBlock[1](4~7)
        vqrshrn.s32  d27, q14, #7 						@; pTmpBlock[6](4~7)
        
        vmull.s16 q7,  d4, d0[2]							@; 50*pSrc[8]
        vmull.s16 q14, d5, d0[2]							@; 50*pSrc[8]
        vmlsl.s16 q7,  d8, d0[0]							@; - 89*pSrc[3*8]
        vmlsl.s16 q14, d9, d0[0]							@; - 89*pSrc[3*8]
        vmlal.s16 q7,  d12, d0[3]							@; + 18*pSrc[5*8]
        vmlal.s16 q14, d13, d0[3]							@; + 18*pSrc[5*8]
        vmlal.s16 q7,  d16, d0[1]							@; + 75*pSrc[7*8] = o[2]
        vmlal.s16 q14, d17, d0[1]							@; + 75*pSrc[7*8] = o[2]
        
        vadd.s32     q15, q11, q7 						@; E[2] + O[2] (0~3)
        vsub.s32     q11, q11, q7 						@; E[2] - O[2] (0~3)
        vqrshrn.s32  d14, q15, #7 						@; pTmpBlock[2](0~3)
        vqrshrn.s32  d22, q11, #7 						@; pTmpBlock[5](0~3)
        
        vadd.s32     q15, q12, q14 						@; E[2] + O[2] (4~7)
        vsub.s32     q14, q12, q14 						@; E[2] - O[2] (4~7)
        vqrshrn.s32  d15, q15, #7 						@; pTmpBlock[2](4~7)
        vqrshrn.s32  d23, q14, #7 						@; pTmpBlock[5](4~7)
        
        vmull.s16 q12,  d4, d0[3]							@; 18*pSrc[8]
        vmull.s16 q14,  d5, d0[3]						  @; 18*pSrc[8]
        vmlsl.s16 q12,  d8, d0[2]							@; - 50*pSrc[3*8]
        vmlsl.s16 q14,  d9, d0[2]						  @; - 50*pSrc[3*8]
        vmlal.s16 q12,  d12, d0[1]							@; + 75*pSrc[5*8]
        vmlal.s16 q14,  d13, d0[1]							@; + 75*pSrc[5*8]
        vmlsl.s16 q12,  d16, d0[0]							@; - 89*pSrc[7*8] = o[3]
        vmlsl.s16 q14,  d17, d0[0]							@; - 89*pSrc[7*8] = o[3]
        
        vadd.s32     q15, q9, q12 						@; E[3] + O[3] (0~3)
        vsub.s32     q12, q9, q12 						@; E[3] - O[3] (0~3)
        vqrshrn.s32  d4,  q15, #7 						@; pTmpBlock[3](0~3)
        vqrshrn.s32  d8,  q12, #7 						@; pTmpBlock[4](0~3)
        
        vadd.s32     q15, q10, q14 						@; E[3] + O[3] (4~7)
        vsub.s32     q14, q10, q14 						@; E[3] - O[3] (4~7)
        vqrshrn.s32  d5,  q15, #7 						@; pTmpBlock[3](4~7)
        vqrshrn.s32  d9,  q14, #7 						@; pTmpBlock[4](4~7)
        
        @; pTmpBlock[0]~pTmpBlock[7]:q1,q3,q7,q2,q4,q11,q13,q5
        @; 将8*8矩阵转置，最好能按照q1~q8的顺序存放
        
        vtrn.16   q1, q3
        vtrn.16   q7, q2
        vtrn.16   q4, q11
        vtrn.16   q13, q5
        
        vtrn.32   q1, q7
        vtrn.32   q3, q2
        vtrn.32   q4, q13
        vtrn.32   q11, q5
        @; q1:  arrTmpBlock[0](0~3) | arrTmpBlock[4](0~3)
        @; q3:  arrTmpBlock[1](0~3) | arrTmpBlock[5](0~3)
        @; q7:  arrTmpBlock[2](0~3) | arrTmpBlock[6](0~3)
        @; q2:  arrTmpBlock[3](0~3) | arrTmpBlock[7](0~3)
        @; q4:  arrTmpBlock[0](4~7) | arrTmpBlock[4](4~7)
        @; q11: arrTmpBlock[1](4~7) | arrTmpBlock[5](4~7)
        @; q13: arrTmpBlock[2](4~7) | arrTmpBlock[6](4~7)
        @; q5:  arrTmpBlock[3](4~7) | arrTmpBlock[7](4~7)

        vmull.s16 q12, d14, d1[1]							@; 83*arrTmpBlock[ 2*8 ]
        vmull.s16 q14, d26, d1[1]							@; 83*arrTmpBlock[ 2*8 ]
        vmlal.s16 q12, d15, d1[3]							@; + 36*arrTmpBlock[ 6*8 ] =EO[0]
        vmlal.s16 q14, d27, d1[3]							@; + 36*arrTmpBlock[ 6*8 ] =EO[0]
        
        vmull.s16 q6,  d14, d1[3]							@; 36*arrTmpBlock[ 2*8 ]
        vmull.s16 q8,  d26, d1[3]							@; 36*arrTmpBlock[ 2*8 ]
        vmlsl.s16 q6,  d15, d1[1]							@; - 83*arrTmpBlock[ 6*8 ] =EO[1]
        vmlsl.s16 q8,  d27, d1[1]							@; - 83*arrTmpBlock[ 6*8 ] =EO[1]
       
       
        @; vswp     d3, d8  				@; q1:arrTmpBlock[0](0~7) , q4:arrTmpBlock[4](0~7)
        
       vmull.s16  q9,  d2, d1[0] 					@; 64*arrTmpBlock[ 0   ]
       vmlal.s16  q9,  d3,d1[0] 					@; +64*arrTmpBlock[ 4*8   ]=EE[0]
       vmull.s16  q10, d8, d1[0] 					@; 64*pSrc[ 0   ]
       vmlal.s16  q10, d9,d1[0] 					@; +64*pSrc[ 4*8   ]=EE[0]
               
       vmull.s16  q15,  d2, d1[0] 					@; 64*pSrc[ 0   ]
       vmlsl.s16  q15,  d3, d1[0] 					@; -64*pSrc[ 4*8   ]=EE[1]
       vmov       q1, q15
       vmull.s16  q15,  d8, d1[0] 					@; 64*pSrc[ 0   ]
       vmlsl.s16  q15,  d9, d1[0] 					@; -64*pSrc[ 4*8   ]=EE[1]
       vmov       q4, q15
        
        vadd.s32  q7, q9, q12 							@; E[0] = EE[0] + EO[0];
        vadd.s32  q13, q10, q14 							@; E[0] = EE[0] + EO[0];
        vsub.s32  q12, q9, q12 							@; E[3] = EE[0] - EO[0];
        vsub.s32  q14, q10, q14 						@; E[3] = EE[0] - EO[0];
        
        vadd.s32  q9, q1, q6 							@;  E[1] = EE[1] + EO[1];
        vadd.s32  q10, q4, q8 							@;  E[1] = EE[1] + EO[1];
        vsub.s32  q6, q1, q6 							@;  E[2] = EE[1] - EO[1];
        vsub.s32  q8, q4, q8 							@;  E[2] = EE[1] - EO[1];
        
        vmull.s16 q1,  d6,  d0[0]							@; 89*arrTmpBlock[8]
        vmull.s16 q4,  d22, d0[0]							@; 89*arrTmpBlock[8]
        vmlal.s16 q1,  d4,  d0[1]							@; + 75*arrTmpBlock[3*8]
        vmlal.s16 q4,  d10, d0[1]							@; + 75*arrTmpBlock[3*8]
        vmlal.s16 q1,  d7,  d0[2]							@; + 50*arrTmpBlock[5*8]
        vmlal.s16 q4,  d23, d0[2]							@; + 50*arrTmpBlock[5*8]
        vmlal.s16 q1,  d5,  d0[3]							@; + 18*arrTmpBlock[7*8] = o[0]
        vmlal.s16 q4,  d11, d0[3]							@; + 18*arrTmpBlock[7*8] = o[0]
        
        vadd.s32     q15, q7, q1 						@; E[0] + O[0] (0~3)
        vsub.s32     q7, q7, q1 						@; E[0] - O[0] (0~3)
        vqrshrn.s32  d2, q15, #12 						@; pDstBlock[0](0~3)
        vqrshrn.s32  d14, q7, #12 						@; pDstBlock[7](0~3)
        
        vadd.s32     q15, q13, q4 						@; E[0] + O[0] (4~7)
        vsub.s32     q4, q13, q4 						@; E[0] - O[0] (4~7)
        vqrshrn.s32  d3, q15, #12 						@; pDstBlock[0](4~7)
        vqrshrn.s32  d15, q4, #12						@; pDstBlock[7](4~7)
        
        vmull.s16 q4,  d6,  d0[1]							@; 75*arrTmpBlock[8]
        vmull.s16 q13, d22, d0[1]							@; 75*arrTmpBlock[8]
        vmlsl.s16 q4,  d4,  d0[3]							@; - 18*arrTmpBlock[3*8]
        vmlsl.s16 q13, d10, d0[3]							@; - 18*arrTmpBlock[3*8]
        vmlsl.s16 q4,  d7,  d0[0]						  @; - 89*arrTmpBlock[5*8]
        vmlsl.s16 q13, d23, d0[0]							@; - 89*arrTmpBlock[5*8]
        vmlsl.s16 q4,  d5,  d0[2]							@; - 50*arrTmpBlock[7*8]; = o[1]
        vmlsl.s16 q13, d11, d0[2]							@; - 50*arrTmpBlock[7*8]; = o[1]
        
        vadd.s32     q15, q9, q4 						@; E[1] + O[1] (0~3)
        vsub.s32     q9,  q9, q4 						@; E[1] - O[1] (0~3)
        vqrshrn.s32  d8,  q15, #12 						@; pDstBlock[1](0~3)
        vqrshrn.s32  d18, q9, #12 						@; pDstBlock[6](0~3)
        
        vadd.s32     q15, q10, q13 						@; E[1] + O[1] (4~7)
        vsub.s32     q10, q10, q13 						@; E[1] - O[1] (4~7)
        vqrshrn.s32  d9,  q15, #12 						@; pDstBlock[1](4~7)
        vqrshrn.s32  d19, q10, #12 						@; pDstBlock[6](4~7)
        
        vmull.s16 q10,  d6, d0[2]							@; 50*arrTmpBlock[8]
        vmull.s16 q13, d22, d0[2]							@; 50*arrTmpBlock[8]
        vmlsl.s16 q10,  d4, d0[0]							@; - 89*arrTmpBlock[3*8]
        vmlsl.s16 q13, d10, d0[0]							@; - 89*arrTmpBlock[3*8]
        vmlal.s16 q10,  d7, d0[3]							@; + 18*arrTmpBlock[5*8]
        vmlal.s16 q13, d23, d0[3]							@; + 18*arrTmpBlock[5*8]
        vmlal.s16 q10,  d5, d0[1]							@; + 75*arrTmpBlock[7*8] = o[2]
        vmlal.s16 q13, d11, d0[1]							@; + 75*arrTmpBlock[7*8] = o[2]
        
        vadd.s32     q15, q6,  q10 						@; E[2] + O[2] (0~3)
        vsub.s32     q6,  q6,  q10 						@; E[2] - O[2] (0~3)
        vqrshrn.s32  d20, q15, #12 						@; pDstBlock[2](0~3)
        vqrshrn.s32  d12, q6,  #12 						@; pDstBlock[5](0~3)
        
        vadd.s32     q15, q8,  q13 						@; E[2] + O[2] (4~7)
        vsub.s32     q8,  q8,  q13						@; E[2] - O[2] (4~7)
        vqrshrn.s32  d21, q15, #12 						@; pDstBlock[2](4~7)
        vqrshrn.s32  d13, q8,  #12 						@; pDstBlock[5](4~7)
        
        vmull.s16 q8,  d6,  d0[3]							@; 18*arrTmpBlock[8]
        vmull.s16 q13, d22, d0[3]						  @; 18*arrTmpBlock[8]
        vmlsl.s16 q8,  d4,  d0[2]							@; - 50*arrTmpBlock[3*8]
        vmlsl.s16 q13, d10, d0[2]						  @; - 50*arrTmpBlock[3*8]
        vmlal.s16 q8,  d7,  d0[1]							@; + 75*arrTmpBlock[5*8]
        vmlal.s16 q13, d23, d0[1]							@; + 75*arrTmpBlock[5*8]
        vmlsl.s16 q8,  d5,  d0[0]							@; - 89*arrTmpBlock[7*8] = o[3]
        vmlsl.s16 q13, d11, d0[0]							@; - 89*arrTmpBlock[7*8] = o[3]
        
        vadd.s32     q15, q12, q8 						@; E[3] + O[3] (0~3)
        vsub.s32     q12, q12, q8 						@; E[3] - O[3] (0~3)
        vqrshrn.s32  d4,  q15, #12 						@; pDstBlock[3](0~3)
        vqrshrn.s32  d6,  q12, #12 						@; pDstBlock[4](0~3)
        
        vadd.s32     q15, q14, q13 						@; E[3] + O[3] (4~7)
        vsub.s32     q14, q14, q13 						@; E[3] - O[3] (4~7)
        vqrshrn.s32  d5,  q15, #12 						@; pDstBlock[3](4~7)
        vqrshrn.s32  d7,  q14, #12 						@; pDstBlock[4](4~7)
        
        @; pDstBlock[0]~pDstBlock[7]:q1,q4,q10,q2,q3,q6,q9,q7
        @; 将8*8矩阵转置，最好能按照q1~q8的顺序存放
        
        vtrn.16   q1, q4
        vtrn.16   q10, q2
        vtrn.16   q3, q6
        vtrn.16   q9, q7
        vtrn.32   q1, q10
        vtrn.32   q4, q2
        vtrn.32   q3, q9
        vtrn.32   q6, q7
        @; q1:  arrTmpBlock[0](0~3) | arrTmpBlock[4](0~3)
        @; q4:  arrTmpBlock[1](0~3) | arrTmpBlock[5](0~3)
        @; q10:  arrTmpBlock[2](0~3) | arrTmpBlock[6](0~3)
        @; q2:  arrTmpBlock[3](0~3) | arrTmpBlock[7](0~3)
        @; q3:  arrTmpBlock[0](4~7) | arrTmpBlock[4](4~7)
        @; q6: arrTmpBlock[1](4~7) | arrTmpBlock[5](4~7)
        @; q9: arrTmpBlock[2](4~7) | arrTmpBlock[6](4~7)
        @; q7:  arrTmpBlock[3](4~7) | arrTmpBlock[7](4~7)
        vswp d3, d6
        vswp d9, d12
        vswp d21, d18
        vswp d5, d14
        @; 获得arrTmpBlock[0]~arrTmpBlock[7]:q1,q4,q10,q2,q3,q6,q9,q7
        
        @; 加载预测值pPerdiction[0]~pPerdiction[63]
        @; mov  r12, #PRED_CACHE_STRIDE
        mov    r12, r1
        vld1.8 {d0}, [r1], r2  			@;pPerdiction[0]
        vld1.8 {d10},[r1], r2  			@;pPerdiction[1]
        vld1.8 {d16},[r1], r2  			@;pPerdiction[2]
        vld1.8 {d22},[r1], r2  			@;pPerdiction[3]
        vld1.8 {d24},[r1], r2  			@;pPerdiction[4]
        vld1.8 {d26},[r1], r2  			@;pPerdiction[5]
        vld1.8 {d28},[r1], r2  			@;pPerdiction[6]
        vld1.8 {d30},[r1] 		  			@;pPerdiction[7]
        
        vaddw.u8  q1,  q1,  d0
        vaddw.u8  q4,  q4,  d10
        vaddw.u8  q10, q10, d16
        vaddw.u8  q2,  q2,  d22
        vaddw.u8  q3,  q3,  d24
        vaddw.u8  q6,  q6, d26
        vaddw.u8  q9,  q9, d28
        vaddw.u8  q7,  q7,  d30
        
        vqmovun.s16  d2,  q1         	@;pDstBlock[0]
        vqmovun.s16  d8,  q4        	@;pDstBlock[1]
        vqmovun.s16  d20, q10       	@;pDstBlock[2]
        vqmovun.s16  d4,  q2        	@;pDstBlock[3]
        vqmovun.s16  d6,  q3        	@;pDstBlock[4]
        vqmovun.s16  d12, q6        	@;pDstBlock[5]
        vqmovun.s16  d18, q9        	@;pDstBlock[6]
        vqmovun.s16  d14, q7        	@;pDstBlock[7]
        
        @; 存储到buffer中
        vst1.8   {d2},   [r12], r2
        vst1.8   {d8},   [r12], r2
        vst1.8   {d20},  [r12], r2
        vst1.8   {d4},   [r12], r2
        vst1.8   {d6},   [r12], r2
        vst1.8   {d12},  [r12], r2
        vst1.8   {d18},  [r12], r2
        vst1.8   {d14},  [r12], r2
        
        @;ldmia sp!, {r4,r5,r12,pc}      
        mov pc, lr      
        .endif    @.if IDCT_ASM_ENABLED==1
        @.end