@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@void IDCT8X8(
@@							const short *pSrcData,
@@							const unsigned char *pPerdictionData,
@@							unsigned char *pDstRecoData,
@@							unsigned int uiDstStride)
@@
@@ short kg_IDCT_coef_for_t8_asm[8] = {89,75,50,18,  64,83,64,36}@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@ 242 cycles
		@@include		h265dec_ASM_config.h
        @@include h265dec_idct_macro.inc
        @area |.text|, code, readonly 
        @align 4
        @@if IDCT_ASM_ENABLED==1  
        @import kg_IDCT_coef_for_t8_asm
        #include     "h265dec_ASM_config.h"
        #include "../../../h265dec_ID.h"
        .text
        .align 4
        .if IDCT_ASM_ENABLED==1
        .globl _IDCT8X8_4X4_ASMV7
        
kg_IDCT_coef_for_t8_asm:		
		.short 89, 75, 50, 18, 64, 83, 64, 36        
_IDCT8X8_4X4_ASMV7:   
        @@ldmia sp!, {r4,r5,r12,pc}      
        stmfd sp!, {r4,r5,lr}
        adr   r4, kg_IDCT_coef_for_t8_asm
        vld1.16   {d0, d1}, [r4]     	@ {89,75,50,18,  64,83,64,36}@
        mov   r4, #16
        vld1.16   {d2}, [r0], r4 			@@ pSrc[0]
        vld1.16   {d3}, [r0], r4 			@@ pSrc[8]
        vld1.16   {d4}, [r0], r4 		    @@ pSrc[2*8]
        vld1.16   {d5}, [r0], r4 			@@ pSrc[3*8]
        
        vmull.s16 q3,  d2, d1[0]			@@ 64*pSrc[ 8 ] EE[0]
        vmull.s16 q4,  d2, d1[0]			@@ 64*pSrc[ 8 ] EE[1]
        vmull.s16 q5,  d4, d1[1]			@@ 83*pSrc[ 2*8 ] EO[0]
        vmull.s16 q6,  d4, d1[3]			@@ 36*pSrc[ 2*8 ] EO[1]
        
        vadd.s32  q7, q3, q5 			    @@ E[0] = EE[0] + EO[0]
        vsub.s32  q8, q3, q5 				@@ E[3] = EE[0] - EO[0]
        vadd.s32  q9, q4, q6 			    @@ E[1] = EE[1] + EO[1]
        vsub.s32  q10, q4, q6 				@@ E[2] = EE[1] - EO[1]
        
        vmull.s16 q11, d3, d0[0]			@@ 89*pSrc[8]
        vmlal.s16 q11, d5, d0[1]			@@ + 75*pSrc[3*8] O[0]
        vmull.s16 q12, d3, d0[1]			@@ 75*pSrc[8]
        vmlsl.s16 q12, d5, d0[3]			@@ - 18*pSrc[3*8] O[1]
        vmull.s16 q13, d3, d0[2]			@@ 50*pSrc[8]
        vmlsl.s16 q13, d5, d0[0]			@@ - 89*pSrc[3*8] O[2]
        vmull.s16 q14, d3, d0[3]			@@ 18*pSrc[8]
        vmlsl.s16 q14, d5, d0[2]			@@ - 50*pSrc[3*8] O[3]
        
        vadd.s32     q15, q7, q11 		    @@ E[0] + O[0] (0~3)
        vsub.s32     q5, q7, q11 			@@ E[0] - O[0] (0~3)
        vqrshrn.s32  d22, q15, #7 			@@ pTmpBlock[0](0~3)
        vqrshrn.s32  d14, q5, #7 			@@ pTmpBlock[7](0~3)
        vadd.s32     q15, q9, q12 			@@ E[1] + O[1] (0~3)
        vsub.s32     q12, q9, q12 		    @@ E[1] - O[1] (0~3)
        vqrshrn.s32  d2, q15, #7 			@@ pTmpBlock[1](0~3)
        vqrshrn.s32  d12, q12, #7 			@@ pTmpBlock[6](0~3)
        vadd.s32     q15, q10, q13 			@@ E[2] + O[2] (0~3)
        vsub.s32     q12, q10, q13			@@ E[2] - O[2] (0~3)
        vqrshrn.s32  d4, q15, #7 			@@ pTmpBlock[2](0~3)
        vqrshrn.s32  d10, q12, #7 			@@ pTmpBlock[5](0~3)
        vadd.s32     q15, q8, q14 			@@ E[3] + O[3] (0~3)
        vsub.s32     q12, q8, q14			@@ E[3] - O[3] (0~3)
        vqrshrn.s32  d6, q15, #7 			@@ pTmpBlock[3](0~3)
        vqrshrn.s32  d8, q12, #7 			@@ pTmpBlock[4](0~3)
        
        @@ pTmpBlock[0]~pTmpBlock[7]:q11,q1,q2,q3,q4,q5,q6,q7       
        vtrn.16   q11, q1
        vtrn.16   q2, q3
        vtrn.16   q4, q5
        vtrn.16   q6, q7
        vtrn.32   q11, q2
        vtrn.32   q1, q3
        vtrn.32   q4, q6
        vtrn.32   q5, q7
        @@ q11:  arrTmpBlock[0](0~3) | arrTmpBlock[4](0~3)
        @@ q1:  arrTmpBlock[1](0~3) | arrTmpBlock[5](0~3)
        @@ q2:  arrTmpBlock[2](0~3) | arrTmpBlock[6](0~3)
        @@ q3:  arrTmpBlock[3](0~3) | arrTmpBlock[7](0~3)
        @@ q4:  arrTmpBlock[0](4~7) | arrTmpBlock[4](4~7)
        @@ q5: arrTmpBlock[1](4~7) | arrTmpBlock[5](4~7)
        @@ q6: arrTmpBlock[2](4~7) | arrTmpBlock[6](4~7)
        @@ q7:  arrTmpBlock[3](4~7) | arrTmpBlock[7](4~7)
        
        vmull.s16 q8,  d22, d1[0]			@@ 64*pSrc[ 0 ] EE[0] 0-3 EE[1]
        vmull.s16 q9,  d8, d1[0]			@@ 64*pSrc[ 0 ] EE[0] 4-7 EE[1]
        vmull.s16 q10,  d4, d1[1]			@@ 83*pSrc[ 2*8 ] EO[0] 0-3
        vmull.s16 q11,  d12, d1[1]			@@ 83*pSrc[ 2*8 ] EO[0] 4-7
        vmull.s16 q12,  d4, d1[3]			@@ 36*pSrc[ 2*8 ] EO[1] 0-3
        vmull.s16 q13,  d12, d1[3]			@@ 36*pSrc[ 2*8 ] EO[1] 4-7
        
        vadd.s32  q14, q8, q10 			    @@ E[0] = EE[0] + EO[0] 0-3
        vadd.s32  q4, q9, q11 			    @@ E[0] = EE[0] + EO[0] 4-7
        vsub.s32  q2, q8, q10 				@@ E[3] = EE[0] - EO[0] 0-3
        vsub.s32  q6, q9, q11 				@@ E[3] = EE[0] - EO[0] 4-7
        vadd.s32  q10, q8, q12 			    @@ E[1] = EE[1] + EO[1] 0-3
        vadd.s32  q11, q9, q13 			    @@ E[1] = EE[1] + EO[1] 4-7
        vsub.s32  q8, q8, q12 				@@ E[2] = EE[1] - EO[1] 0-3
        vsub.s32  q9, q9, q13 				@@ E[2] = EE[1] - EO[1] 4-7
        
        vmull.s16 q12, d2, d0[0]			@@ 89*pSrc[8]
        vmlal.s16 q12, d6, d0[1]			@@ + 75*pSrc[3*8] O[0] 0-3
        vmull.s16 q13, d10, d0[0]			@@ 89*pSrc[8]
        vmlal.s16 q13, d14, d0[1]			@@ + 75*pSrc[3*8] O[0] 4-7
        vadd.s32     q15, q14, q12 		    @@ E[0] + O[0] (0~3)
        vsub.s32     q12, q14, q12 			@@ E[0] - O[0] (0~3)
        vqrshrn.s32  d30, q15, #12 			@@ pDstBlock[0](0~3)
        vqrshrn.s32  d28, q12, #12 			@@ pDstBlock[7](0~3)    
        vadd.s32     q12, q4, q13 			@@ E[0] + O[0] (4~7)
        vsub.s32     q4, q4, q13 			@@ E[0] - O[0] (4~7)
        vqrshrn.s32  d31, q12, #12 			@@ pDstBlock[0](4~7)
        vqrshrn.s32  d29, q4, #12			@@ pDstBlock[7](4~7)
        
        vmull.s16 q12, d2, d0[1]			@@ 75*pSrc[8]
        vmlsl.s16 q12, d6, d0[3]			@@ - 18*pSrc[3*8] O[1] 0-3
        vmull.s16 q13, d10, d0[1]			@@ 75*pSrc[8]
        vmlsl.s16 q13, d14, d0[3]			@@ - 18*pSrc[3*8] O[1] 4-7
        vadd.s32     q4, q10, q12 		    @@ E[1] + O[1] (0~3)
        vsub.s32     q12, q10, q12 			@@ E[1] - O[1] (0~3)
        vqrshrn.s32  d20, q4, #12 			@@ pDstBlock[1](0~3)
        vqrshrn.s32  d8, q12, #12 			@@ pDstBlock[6](0~3)    
        vadd.s32     q12, q11, q13 			@@ E[1] + O[1] (4~7)
        vsub.s32     q11, q11, q13 			@@ E[1] - O[1] (4~7)
        vqrshrn.s32  d21, q12, #12 			@@ pDstBlock[1](4~7)
        vqrshrn.s32  d9, q11, #12			@@ pDstBlock[6](4~7)
        
        vmull.s16 q12, d2, d0[2]			@@ 50*pSrc[8]
        vmlsl.s16 q12, d6, d0[0]			@@ - 89*pSrc[3*8] O[2] 0-3
        vmull.s16 q13, d10, d0[2]			@@ 50*pSrc[8]
        vmlsl.s16 q13, d14, d0[0]			@@ - 89*pSrc[3*8] O[2] 4-7
        vadd.s32     q11, q8, q12 		    @@ E[2] + O[2] (0~3)
        vsub.s32     q12, q8, q12 			@@ E[2] - O[2] (0~3)
        vqrshrn.s32  d22, q11, #12 			@@ pDstBlock[2](0~3)
        vqrshrn.s32  d16, q12, #12 			@@ pDstBlock[5](0~3)    
        vadd.s32     q12, q9, q13 			@@ E[2] + O[2] (4~7)
        vsub.s32     q9, q9, q13 			@@ E[2] - O[2] (4~7)
        vqrshrn.s32  d23, q12, #12 			@@ pDstBlock[2](4~7)
        vqrshrn.s32  d17, q9, #12			@@ pDstBlock[5](4~7)
        
        vmull.s16 q12, d2, d0[3]			@@ 18*pSrc[8]
        vmlsl.s16 q12, d6, d0[2]			@@ - 50*pSrc[3*8] O[3] 0-3
        vmull.s16 q13, d10, d0[3]			@@ 18*pSrc[8]
        vmlsl.s16 q13, d14, d0[2]			@@ - 50*pSrc[3*8] O[3] 4-7
        vadd.s32     q9, q2, q12 		    @@ E[3] + O[3] (0~3)
        vsub.s32     q12, q2, q12 			@@ E[3] - O[3] (0~3)
        vqrshrn.s32  d18, q9, #12 			@@ pDstBlock[3](0~3)
        vqrshrn.s32  d4, q12, #12 			@@ pDstBlock[4](0~3)    
        vadd.s32     q12, q6, q13 			@@ E[3] + O[3] (4~7)
        vsub.s32     q6, q6, q13 			@@ E[3] - O[3] (4~7)
        vqrshrn.s32  d19, q12, #12 			@@ pDstBlock[3](4~7)
        vqrshrn.s32  d5, q6, #12			@@ pDstBlock[4](4~7)
        
       
        @@ pDstBlock[0]~pDstBlock[7]:q15,q10,q11,q9,q2,q8,q4,q14
        vtrn.16   q15, q10
        vtrn.16   q11, q9
        vtrn.16   q2, q8
        vtrn.16   q4, q14
        vtrn.32   q15, q11
        vtrn.32   q10, q9
        vtrn.32   q2, q4
        vtrn.32   q8, q14
        @@ q15:  arrTmpBlock[0](0~3) | arrTmpBlock[4](0~3)
        @@ q10:  arrTmpBlock[1](0~3) | arrTmpBlock[5](0~3)
        @@ q11:  arrTmpBlock[2](0~3) | arrTmpBlock[6](0~3)
        @@ q9:  arrTmpBlock[3](0~3) | arrTmpBlock[7](0~3)
        @@ q2:  arrTmpBlock[0](4~7) | arrTmpBlock[4](4~7)
        @@ q8: arrTmpBlock[1](4~7) | arrTmpBlock[5](4~7)
        @@ q4: arrTmpBlock[2](4~7) | arrTmpBlock[6](4~7)
        @@ q14:  arrTmpBlock[3](4~7) | arrTmpBlock[7](4~7)
        vswp d31, d4
        vswp d21, d16
        vswp d23, d8
        vswp d19, d28
        @@ 获得arrTmpBlock[0]~arrTmpBlock[7]:q15,q14,q10,q4,q11,q8,q9,q2
        
        @@ 加载预测值pPerdiction[0]~pPerdiction[63]
        mov r5, r1
        vld1.8 {d2}, [r1], r2  			@@pPerdiction[0]
        vld1.8 {d6},[r1], r2  			@@pPerdiction[1]
        vld1.8 {d10},[r1], r2  			@@pPerdiction[2]
        vld1.8 {d12},[r1], r2  			@@pPerdiction[3]
        vld1.8 {d14},[r1], r2  			@@pPerdiction[4]
        vld1.8 {d24},[r1], r2  			@@pPerdiction[5]
        vld1.8 {d26},[r1], r2  			@@pPerdiction[6]
        vld1.8 {d0},[r1] 		  		@@pPerdiction[7]
                
        vaddw.u8  q15,  q15,  d2
        vaddw.u8  q10,  q10,  d6
        vaddw.u8  q11, q11, d10
        vaddw.u8  q9,  q9,  d12
        vaddw.u8  q2,  q2,  d14
        vaddw.u8  q8,  q8, d24
        vaddw.u8  q4,  q4, d26
        vaddw.u8  q14,  q14,  d0
        
        vqmovun.s16  d0,  q15         	@@pDstBlock[0]
        vqmovun.s16  d20,  q10        	@@pDstBlock[1]
        vqmovun.s16  d22, q11       	@@pDstBlock[2]
        vqmovun.s16  d18,  q9        	@@pDstBlock[3]
        vqmovun.s16  d4,  q2       	@@pDstBlock[4]
        vqmovun.s16  d16, q8        	@@pDstBlock[5]
        vqmovun.s16  d8, q4        	@@pDstBlock[6]
        vqmovun.s16  d28, q14        	@@pDstBlock[7]
        
        @@ 存储到buffer中
        vst1.8   {d0},   [r5], r2
        vst1.8   {d20},   [r5], r2
        vst1.8   {d22},  [r5], r2
        vst1.8   {d18},   [r5], r2
        vst1.8   {d4},   [r5], r2
        vst1.8   {d16},  [r5], r2
        vst1.8   {d8},  [r5], r2
        vst1.8   {d28},  [r5], r2
       
        ldmfd sp!, {r4,r5,pc}    
        .endif    @.if IDCT_ASM_ENABLED==1
        @.end