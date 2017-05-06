@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@;void IDCT4X4(
@;							const short *pSrcData,
@;							const unsigned char *pPerdictionData,
@;							unsigned char *pDstRecoData,
@;							unsigned int uiDstStride)
@;
@; short g_aIDCTCefT4_asm[4] = {64,83,64,36}
@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@;V5 =V4+adjust pipeline
        @AREA |.text|, CODE, READONLY    
        @@;IMPORT g_uiBitIncrement = 0
        @IMPORT g_aIDCTCefT4_asm
        @EXPORT IDCT4X4_ASM
        @align 4
        #include     "h265dec_ASM_config.h"
        #include "../../../h265dec_ID.h"
        @#include "../../../h265dec_config.h"
        .section .text
        .align 4
        .if IDCT_ASM_ENABLED==1
        @.extern kg_IDCT_coef_for_t4_asm
        .global IDCT4X4ASMV7
IDCT4X4ASMV7:  
        
        vld1.16  {d0, d1, d2, d3}, [r0] 		@; psrcdata:d0,d1,d2,d3??
        
        adr       r12, kg_IDCT_coef_for_t4_asm
        vld1.16  {d4}, [r12] 								@; g_ait4[0],g_ait4[4],g_ait4[8],g_ait4[12]
        vmull.s16  q3, d1, d4[1] 						@; 83 * psrc[1]
        vmlal.s16  q3, d3, d4[3]						@; +36 * psrc[3 * 4]
        
        vmull.s16  q4, d1, d4[3]						@; 36 * psrc[4]
        vmlsl.s16  q4, d3, d4[1]						@; -83 * psrc[3 * 4]
        
        vmull.s16  q5, d0, d4[0] 						@; 64 * psrc[0]       
        vmlal.s16  q5, d2, d4[2]						@; +64 * psrc[2 * 4]

        vmull.s16  q6, d0, d4[0] 						@; 64 * psrc[0]
        vmlsl.s16  q6, d2, d4[2]						@; -64 * psrc[2 * 4]
        
        @; q3: o[0], q4:o[1], q5:e[0], q6:e[1] @;  + add--通过VQRSHRN进行舍入
        vadd.s32   q8, q5, q3 		@; e[0] + o[0]        
        vadd.s32   q9, q6, q4 		@; e[1] + o[1]        
        vsub.s32   q10, q6, q4 		@; e[1] - o[1]        
        vsub.s32   q11, q5, q3 		@; e[0] - o[0]
        
        vqrshrn.s32 d0, q8, #7     	@; ptmpblock[0]
        vqrshrn.s32 d1, q9, #7     	@; ptmpblock[1]
        vqrshrn.s32 d2, q10, #7     	@; ptmpblock[2]
        vqrshrn.s32 d3, q11, #7     	@; ptmpblock[3]
        @;@矩阵转置
        @;mov         r12, #PRED_CACHE_STRIDE 					@;uipredstride = 132
        mov     r12, r1
        pld      [r1]
        vld1.32  {d24[0]}, [r1], r2 			@; pperdiction[0]
        vld1.32  {d24[1]}, [r1], r2 			@; pperdiction[1]
        
        vtrn.16    d0, d1
        vtrn.16    d2, d3
        vtrn.32    d1, d3
        vtrn.32    d0, d2
        
        vld1.32  {d26[0]}, [r1], r2 			@; pperdiction[2]
        vld1.32  {d26[1]}, [r1] 					@; pperdiction[3]
        
        @; 转置后Di分别为第二个for中的:pTmpBlock[i*4]
        vmull.s16  q3, d1, d4[1] 						@; 83 * psrc[1]
        vmlal.s16  q3, d3, d4[3]						@; +36 * psrc[3 * 4]
        
        vmull.s16  q4, d1, d4[3]						@; 36 * psrc[4]
        vmlsl.s16  q4, d3, d4[1]						@; -83 * psrc[3 * 4]
        
        vmull.s16  q5, d0, d4[0] 						@; 64 * psrc[0]       
        vmlal.s16  q5, d2, d4[2]						@; +64 * psrc[2 * 4]

        vmull.s16  q6, d0, d4[0] 						@; 64 * psrc[0]
        vmlsl.s16  q6, d2, d4[2]						@; -64 * psrc[2 * 4]
        
        @; q3: o[0], q4:o[1], q5:e[0], q6:e[1] @;  + add--通过VQRSHRN进行舍入
        vadd.s32   q8, q5, q3 		@; e[0] + o[0]        
        vadd.s32   q9, q6, q4 		@; e[1] + o[1]        
        vsub.s32   q10, q6, q4 		@; e[1] - o[1]        
        vsub.s32   q11, q5, q3 		@; e[0] - o[0]
        
        vrshrn.s32  d0, q8, #12  			@;((e[0] + o[0] + add)>>shift_2nd )
        vrshrn.s32  d1, q9, #12  			@;((e[1] + o[1] + add)>>shift_2nd )
        vrshrn.s32  d2, q10, #12  			@;((e[1] - o[1] + add)>>shift_2nd )
        vrshrn.s32  d3, q11, #12  			@;((e[0] - o[0] + add)>>shift_2nd )
        
        vtrn.16    d0, d1
        vtrn.16    d2, d3
        vtrn.32    d1, d3
        vtrn.32    d0, d2
        
        vaddw.u8    q12, q0, d24
        vaddw.u8    q13, q1, d26
        
        vqmovun.s16  d0, q12
        vqmovun.s16  d1, q13
        
        
        @; store the output data
        vst1.32      {d0[0]}, [r12],r2
        vst1.32      {d0[1]}, [r12],r2
        vst1.32      {d1[0]}, [r12],r2
        vst1.32      {d1[1]}, [r12]
        
        @;ldmia sp!, {r4,r5,r12,pc}      
        mov pc, lr
        .endif    @.if IDCT_ASM_ENABLED==1
kg_IDCT_coef_for_t4_asm:		
        .hword 64, 83, 64, 36 		
        .end
        
        