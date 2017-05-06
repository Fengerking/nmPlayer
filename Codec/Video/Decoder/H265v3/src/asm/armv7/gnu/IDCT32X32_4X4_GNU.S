@@@@@@@@@@@@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@;void IDCT32X32(
@;							const short *pSrcData,
@;							const unsigned char *pPerdictionData,
@;							unsigned char *pDstRecoData,
@;							unsigned int uiDstStride)
@; short kg_IDCT_coef_for_t32_4x4_asm_EEEO[18] = {                                                                         
@;	  90, 87, 80, 70, 57, 43, 25, 9                                        
@; };                                                                                                                                
@; const short kg_IDCT_coef_for_t32_4x4_asm_O[16 * 2] = {                                                             
@;   90, 82, 67, 46,         //10~13                    
@;   85, 46, -13, -67,       //30~33 
@;
@;   -78, -61, -38, -13,     //112~15                   
@;   -61, -90, -78, -31,     //312~315
@;                   
@;   22, -4, -31, -54,       //14~17                    
@;   -90, -73, -22, 38,      //34~37                    
@;   -73, -85, -90, -88,     //18~111                   
@;   82, 88, 54, -4,         //38~311                   
                  
@; };                                                   
@; 
@@@@@@@@@@@@;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        @include		h265dec_ASM_config.h                                                         
        @include h265dec_idct_macro.inc
        @area |.text|, code, readonly 
        @align 4 
        @if IDCT_ASM_ENABLED==1 
        @import kg_IDCT_coef_for_t32_4x4_asm_EEEO
        @import kg_IDCT_coef_for_t32_4x4_asm_O
        @export IDCT32X32_4X4_ASMV7
        
        #include     "h265dec_ASM_config.h"
        #include "../../../h265dec_ID.h"
        @#include "../../../h265dec_config.h"
        .section .text
        .align 4
        .if IDCT_ASM_ENABLED==1
        @.extern kg_IDCT_coef_for_t32_4x4_asm_EEEO
        @.extern kg_IDCT_coef_for_t32_4x4_asm_O
        .global IDCT32X32_4X4_ASMV7
		
kg_IDCT_coef_for_t32_4x4_asm_EEEO:		
	.hword 90, 87, 80, 70, 57, 43, 25, 9      
kg_IDCT_coef_for_t32_4x4_asm_O: 
	.hword  90, 90, 88, 85, 90, 82, 67, 46
	.hword  31, 22, 13,  4, -78,-61,-38,-13
    .hword  82, 78, 73, 67, 22, -4,-31,-54
    .hword  61, 54, 46, 38, -73,-85,-90,-88	
        
IDCT32X32_4X4_ASMV7:   
        
        stmfd sp!, {r4, r5, r6, r7,r8,r9, r10, lr}
        @; 需要2048B放pTmpBlock[32*32]的值，但是存储顺序是使用vld4.16的，
        @; 再保存E[0]~E[15](0~3),需要4B*16*4=256
        @; 并保留空间为pTmpBlock是4字节对齐,故至少需要2308
        adr   r4, kg_IDCT_coef_for_t32_4x4_asm_EEEO   
        mov   r5, #224 									@; 第0个4x4块到第7个块的跨度：7*32    
        mov   r6, #64   			 				 	@; pSrc[0]到pSrc[1*32]的跨度1 
        mov   r7, #-128 								@; 第7个块起始点到第3个块的跨度 = -4*32
        mov   r8, #-96									@; 第4个块起始点到第1个块的跨度 = -3*32
        mov   r9, #160									@; 第1个块起始点到第6个块的跨度 = 5*32
        vld1.16  {d2, d3},  [r4]
        @; 90,87,80,70,  57,43,25,9
        pld      [r0]        
        vld1.16  {d4},  [r0], r6 							@;pSrc[0*32]...[3]
        vld1.16  {d22}, [r0], r6 							@;pSrc[1*32]...[3]
        vld1.16  {d5},  [r0], r6 							@;pSrc[2*32]...[3]
        vld1.16  {d23}, [r0], r6 							@;pSrc[3*32]...[3]
        sub   r0, r0, r6, lsl #2
        vshll.s16  q12, d4, #6 								@; midValue = pSrc[0] << 6
        vmull.s16  q3, d5, d2[0] 							@; EO[0]
        vmull.s16  q4, d5, d2[1] 							@; EO[1]
        vmull.s16  q5, d5, d2[2] 							@; EO[2]
        vmull.s16  q6, d5, d2[3] 							@; EO[3]
        
        vadd.s32  q8, q12, q3  								@; E[0] = midValue+ EO[k];
        vsub.s32  q3, q12, q3  								@; E[15] = midValue - EO[k];
        
        vadd.s32  q7, q12, q4  								@; E[1] = midValue+ EO[k];
        vsub.s32  q4, q12, q4  								@; E[14] = midValue - EO[k];
        
        vadd.s32  q9, q12, q5  								@; E[2] = midValue+ EO[k];
        vsub.s32  q5, q12, q5  								@; E[13] = midValue - EO[k];
        
        vadd.s32  q10, q12, q6  								@; E[3] = midValue+ EO[k];
        vsub.s32  q6, q12, q6  								@; E[12] = midValue - EO[k];
        
        adr   r4, kg_IDCT_coef_for_t32_4x4_asm_O
        vld1.16  {d0, d1}, [r4]! 						@; kg_IDCT_coef_for_t32_4x4_asm_O[0~8]
        
        vmull.s16 q13, d22, d0[0]
        vmlal.s16 q13, d23, d1[0]  					@; O[0]
        vadd.s32  q14, q8, q13 								@; E[0]+O[0]
        vsub.s32  q8,  q8, q13 								@; E[0]-O[0]
        vqrshrn.s32 d28, q14, #7 							@; p_tmp_block[00~03]
        vqrshrn.s32 d17, q8,  #7 							@; p_tmp_block[310~313]
        
        vmull.s16 q13, d22, d0[1]
        vmlal.s16 q13, d23, d1[1]  					@; O[1]
        vadd.s32  q15, q7, q13 								@; E[1]+O[1]
        vsub.s32  q7,  q7, q13 								@; E[1]-O[1]
        vqrshrn.s32 d29, q15, #7 							@; p_tmp_block[10~13]
        vqrshrn.s32 d16, q7,  #7 							@; p_tmp_block[300~303]
        
        vmull.s16 q13, d22, d0[2]
        vmlal.s16 q13, d23, d1[2]  					@; O[2]
        vadd.s32  q15, q9, q13 								@; E[2]+O[2]
        vsub.s32  q9,  q9, q13 								@; E[2]-O[2]
        vqrshrn.s32 d30, q15, #7 							@; p_tmp_block[20~23]
        vqrshrn.s32 d15, q9,  #7 							@; p_tmp_block[290~293]
        
        vmull.s16 q13, d22, d0[3]
        vmlal.s16 q13, d23, d1[3]  					@; O[3]
        vadd.s32  q9,  q10, q13 							@; E[3]+O[3]
        vsub.s32  q10, q10, q13 							@; E[3]-O[3]
        vqrshrn.s32 d31, q9, #7 							@; p_tmp_block[30~33]
        vqrshrn.s32 d14, q10,  #7 							@; p_tmp_block[280~283]
        @; 0~3行转置
        vst4.16 {d28, d29, d30, d31}, [r0], r5  @; 
        @; 28~31行转置
        vst4.16 {d14, d15, d16, d17}, [r0], r7
        
        vld1.16  {d0, d1}, [r4]! 						@; kg_IDCT_coef_for_t32_4x4_asm_O[0~8]
        @;-78, -61, -38, -13,     //112~15                   
				@;   -61, -90, -78, -31,     //312~315
        
        vmull.s16 q13, d22, d0[0]
        vmlal.s16 q13, d23, d1[0]  					@; O[12]
        vadd.s32  q14, q6, q13 								@; E[12]+O[12]
        vsub.s32  q8,  q6, q13 								@; E[12]-O[12]
        vqrshrn.s32 d28, q14, #7 							@; p_tmp_block[120~123]
        vqrshrn.s32 d17, q8,  #7 							@; p_tmp_block[190~193]
        
        vmull.s16 q13, d22, d0[1]
        vmlal.s16 q13, d23, d1[1]  					@; O[13]
        vadd.s32  q15, q5, q13 								@; E[13]+O[13]
        vsub.s32  q7,  q5, q13 								@; E[13]-O[13]
        vqrshrn.s32 d29, q15, #7 							@; p_tmp_block[130~133]
        vqrshrn.s32 d16, q7,  #7 							@; p_tmp_block[180~183]
        
        vmull.s16 q13, d22, d0[2]
        vmlal.s16 q13, d23, d1[2]  					@; O[14]
        vadd.s32  q15, q4, q13 								@; E[14]+O[14]
        vsub.s32  q9,  q4, q13 								@; E[14]-O[14]
        vqrshrn.s32 d30, q15, #7 							@; p_tmp_block[140~143]
        vqrshrn.s32 d15, q9,  #7 							@; p_tmp_block[170~173]
        
        vmull.s16 q13, d22, d0[3]
        vmlal.s16 q13, d23, d1[3]  					@; O[15]
        vadd.s32  q9,  q3, q13 							@; E[15]+O[15]
        vsub.s32  q10, q3, q13 							@; E[15]-O[15]
        vqrshrn.s32 d31, q9, #7 							@; p_tmp_block[150~153]
        vqrshrn.s32 d14, q10, #7 							@; p_tmp_block[160~163]
        
        @; 12~15行转置
        vst4.16 {d28, d29, d30, d31}, [r0]!  @; 
        @; 16~19行转置
        vst4.16 {d14, d15, d16, d17}, [r0], r8
        
        vmull.s16  q3, d5, d3[0] 							@; EO[4]
        vmull.s16  q4, d5, d3[1] 							@; EO[5]
        vmull.s16  q5, d5, d3[2] 							@; EO[6]
        vmull.s16  q6, d5, d3[3] 							@; EO[7]
        
        vadd.s32  q8, q12, q3  								@; E[4] = midValue+ EO[k];
        vsub.s32  q3, q12, q3  								@; E[11] = midValue - EO[k];
        
        vadd.s32  q7, q12, q4  								@; E[5] = midValue+ EO[k];
        vsub.s32  q4, q12, q4  								@; E[10] = midValue - EO[k];
        
        vadd.s32  q9, q12, q5  								@; E[6] = midValue+ EO[k];
        vsub.s32  q5, q12, q5  								@; E[9] = midValue - EO[k];
        
        vadd.s32  q10, q12, q6  								@; E[7] = midValue+ EO[k];
        vsub.s32  q6, q12, q6  								@; E[8] = midValue - EO[k];
        
        
        vld1.16  {d0, d1}, [r4]! 						@; kg_IDCT_coef_for_t32_4x4_asm_O[0~8]
        @;   22, -4, -31, -54,       //14~17                    
				@;   -90, -73, -22, 38,      //34~37
        vmull.s16 q13, d22, d0[0]
        vmlal.s16 q13, d23, d1[0]  					@; O[4]
        vadd.s32  q14, q8, q13 								@; E[4]+O[4]
        vsub.s32  q8,  q8, q13 								@; E[4]-O[4]
        vqrshrn.s32 d28, q14, #7 							@; p_tmp_block[40~43]
        vqrshrn.s32 d17, q8,  #7 							@; p_tmp_block[270~273]
        
        vmull.s16 q13, d22, d0[1]
        vmlal.s16 q13, d23, d1[1]  					@; O[5]
        vadd.s32  q15, q7, q13 								@; E[5]+O[5]
        vsub.s32  q7,  q7, q13 								@; E[5]-O[5]
        vqrshrn.s32 d29, q15, #7 							@; p_tmp_block[50~53]
        vqrshrn.s32 d16, q7,  #7 							@; p_tmp_block[260~263]
        
        vmull.s16 q13, d22, d0[2]
        vmlal.s16 q13, d23, d1[2]  					@; O[6]
        vadd.s32  q15, q9, q13 								@; E[6]+O[6]
        vsub.s32  q9,  q9, q13 								@; E[6]-O[6]
        vqrshrn.s32 d30, q15, #7 							@; p_tmp_block[60~63]
        vqrshrn.s32 d15, q9,  #7 							@; p_tmp_block[250~253]
        
        vmull.s16 q13, d22, d0[3]
        vmlal.s16 q13, d23, d1[3]  					@; O[7]
        vadd.s32  q9,  q10, q13 							@; E[7]+O[7]
        vsub.s32  q10, q10, q13 							@; E[7]-O[7]
        vqrshrn.s32 d31, q9, #7 							@; p_tmp_block[70~73]
        vqrshrn.s32 d14, q10,  #7 							@; p_tmp_block[240~243]
        @; 4~8行转置
        vst4.16 {d28, d29, d30, d31}, [r0], r9  @; 
        @; 24~27行转置
        vst4.16 {d14, d15, d16, d17}, [r0], r7
        
        vld1.16  {d0, d1}, [r4]! 						@; kg_IDCT_coef_for_t32_4x4_asm_O[0~8]
        @;   -73, -85, -90, -88,     //18~111                   
				@;   82, 88, 54, -4,         //38~311
        
        vmull.s16 q13, d22, d0[0]
        vmlal.s16 q13, d23, d1[0]  					@; O[8]
        vadd.s32  q14, q6, q13 								@; E[8]+O[8]
        vsub.s32  q8,  q6, q13 								@; E[8]-O[8]
        vqrshrn.s32 d28, q14, #7 							@; p_tmp_block[80~83]
        vqrshrn.s32 d17, q8,  #7 							@; p_tmp_block[230~233]
        
        vmull.s16 q13, d22, d0[1]
        vmlal.s16 q13, d23, d1[1]  					@; O[9]
        vadd.s32  q15, q5, q13 								@; E[9]+O[9]
        vsub.s32  q7,  q5, q13 								@; E[9]-O[9]
        vqrshrn.s32 d29, q15, #7 							@; p_tmp_block[90~93]
        vqrshrn.s32 d16, q7,  #7 							@; p_tmp_block[220~223]
        
        vmull.s16 q13, d22, d0[2]
        vmlal.s16 q13, d23, d1[2]  					@; O[10]
        vadd.s32  q15, q4, q13 								@; E[10]+O[10]
        vsub.s32  q9,  q4, q13 								@; E[10]-O[10]
        vqrshrn.s32 d30, q15, #7 							@; p_tmp_block[100~103]
        vqrshrn.s32 d15, q9,  #7 							@; p_tmp_block[210~213]
        
        vmull.s16 q13, d22, d0[3]
        vmlal.s16 q13, d23, d1[3]  					@; O[11]
        vadd.s32  q9,  q3, q13 							@; E[11]+O[11]
        vsub.s32  q10, q3, q13 							@; E[11]-O[11]
        vqrshrn.s32 d31, q9, #7 							@; p_tmp_block[110~113]
        vqrshrn.s32 d14, q10, #7 							@; p_tmp_block[200~203]
        
        vst4.16 {d28, d29, d30, d31}, [r0]  @; 
        sub   r0, r0, r8
        vst4.16 {d14, d15, d16, d17}, [r0] @; r0回到起始点
        sub   r0, r0, r9
        
        
        @; 第二次变换
        
        @; mov   r5, #PRED_CACHE_STRIDE
        mov   r5, r2 			@; pred_stride
        mov   r3, r2
        mov   r2, r1
        mov   r6, #3
        mov   r12, #28
        mls   r7, r5, r6, r12 	@; -3*predStride+28
        mls   r8, r3, r6, r12 	@; -3*dstStride+28
        sub   r9, r7, #44 			@; -3*predStride-16
        sub   r10,r8, #44 			@; -3*dstStride-16
        mov   r6, #8 						@; j/4
        
IDCT32X32_4X4_ASMV7_4row_loop_2:        
        vld1.16  {d4},  [r0]! 							@;pSrc[0*32]...[3]
        vld1.16  {d22}, [r0]! 							@;pSrc[1*32]...[3]
        vld1.16  {d5},  [r0]! 							@;pSrc[2*32]...[3]
        vld1.16  {d23}, [r0]! 							@;pSrc[3*32]...[3]
        
        @;======EO[0]~EO[3],  E[0]~E[3],[12]~[15], O[0]~O[3],O[12]~O[15], Resi[0]~[3],[28]~[31],[12]~[15],[16]~[19]
        
        vshll.s16 q12, d4, #6 								@; midValue = pSrc[0] << 6
        vmull.s16  q3, d5, d2[0] 							@; EO[0]
        vmull.s16  q4, d5, d2[1] 							@; EO[1]
        vmull.s16  q5, d5, d2[2] 							@; EO[2]
        vmull.s16  q6, d5, d2[3] 							@; EO[3]
        
        vadd.s32  q8, q12, q3  								@; E[0] = midValue+ EO[k];
        vsub.s32  q3, q12, q3  								@; E[15] = midValue - EO[k];
        
        vadd.s32  q7, q12, q4  								@; E[1] = midValue+ EO[k];
        vsub.s32  q4, q12, q4  								@; E[14] = midValue - EO[k];
        
        vadd.s32  q9, q12, q5  								@; E[2] = midValue+ EO[k];
        vsub.s32  q5, q12, q5  								@; E[13] = midValue - EO[k];
        
        vadd.s32  q10, q12, q6  								@; E[3] = midValue+ EO[k];
        vsub.s32  q6, q12, q6  								@; E[12] = midValue - EO[k];
        
        adr   r4, kg_IDCT_coef_for_t32_4x4_asm_O
        vld1.16  {d0, d1}, [r4]! 						@; kg_IDCT_coef_for_t32_4x4_asm_O[0~8]
        
        vmull.s16 q13, d22, d0[0]
        vmlal.s16 q13, d23, d1[0]  					@; O[0]
        vadd.s32  q14, q8, q13 								@; E[0]+O[0]
        vsub.s32  q8,  q8, q13 								@; E[0]-O[0]
        vqrshrn.s32 d28, q14, #12 							@; p_tmp_block[00~03]
        vqrshrn.s32 d17, q8,  #12 							@; p_tmp_block[310~313]
        
        vmull.s16 q13, d22, d0[1]
        vmlal.s16 q13, d23, d1[1]  					@; O[1]
        vadd.s32  q15, q7, q13 								@; E[1]+O[1]
        vsub.s32  q7,  q7, q13 								@; E[1]-O[1]
        vqrshrn.s32 d29, q15, #12 							@; p_tmp_block[10~13]
        vqrshrn.s32 d16, q7,  #12 							@; p_tmp_block[300~303]
        
        vmull.s16 q13, d22, d0[2]
        vmlal.s16 q13, d23, d1[2]  					@; O[2]
        vadd.s32  q15, q9, q13 								@; E[2]+O[2]
        vsub.s32  q9,  q9, q13 								@; E[2]-O[2]
        vqrshrn.s32 d30, q15, #12 							@; p_tmp_block[20~23]
        vqrshrn.s32 d15, q9,  #12 							@; p_tmp_block[290~293]
        
        vmull.s16 q13, d22, d0[3]
        vmlal.s16 q13, d23, d1[3]  					@; O[3]
        vadd.s32  q9,  q10, q13 							@; E[3]+O[3]
        vsub.s32  q10, q10, q13 							@; E[3]-O[3]
        vqrshrn.s32 d31, q9, #12 							@; p_tmp_block[30~33]
        vqrshrn.s32 d14, q10, #12 							@; p_tmp_block[280~283]
        
        @; 转置存储0~3行
        vtrn.16  d28, d29
        vtrn.16  d30, d31
        vtrn.32  q14, q15
        pld      [r1]
        vld1.32   {d18[0]}, [r1],r5							@; pPerdiction[00~03]
        vld1.32   {d18[1]}, [r1],r5							@; pPerdiction[10~13]
        vld1.32   {d19[0]}, [r1],r5							@; pPerdiction[20~23]
        vld1.32   {d19[1]}, [r1],r7							@; pPerdiction[30~33]
        
        vaddw.u8  q14, q14, d18
        vaddw.u8  q15, q15, d19
        vqmovun.s16 d28, q14
        vqmovun.s16 d29, q15
        
        vst1.32  {d28[0]}, [r2], r3
        vst1.32  {d28[1]}, [r2], r3
        vst1.32  {d29[0]}, [r2], r3
        vst1.32  {d29[1]}, [r2], r8        
        
        @; 转置28~31行的4个元素
        vtrn.16  d14, d15
        vtrn.16  d16, d17
        vtrn.32  q7, q8
        
        vld1.32   {d18[0]}, [r1],r5							@; pPerdiction[280~283]
        vld1.32   {d18[1]}, [r1],r5							@; pPerdiction[290~293]
        vld1.32   {d19[0]}, [r1],r5							@; pPerdiction[300~303]
        vld1.32   {d19[1]}, [r1],r9						@; pPerdiction[310~313]
        
        vaddw.u8  q14, q7, d18
        vaddw.u8  q15, q8, d19
        vqmovun.s16 d28, q14
        vqmovun.s16 d29, q15
        
        vst1.32  {d28[0]}, [r2], r3
        vst1.32  {d28[1]}, [r2], r3
        vst1.32  {d29[0]}, [r2], r3
        vst1.32  {d29[1]}, [r2], r10
        
        vld1.16  {d0, d1}, [r4]! 						@; kg_IDCT_coef_for_t32_4x4_asm_O[0~8]
        @;-78, -61, -38, -13,     //112~15                   
				@;   -61, -90, -78, -31,     //312~315
        
        vmull.s16 q13, d22, d0[0]
        vmlal.s16 q13, d23, d1[0]  					@; O[12]
        vadd.s32  q14, q6, q13 								@; E[12]+O[12]
        vsub.s32  q8,  q6, q13 								@; E[12]-O[12]
        vqrshrn.s32 d28, q14, #12 							@; p_tmp_block[120~123]
        vqrshrn.s32 d17, q8,  #12 							@; p_tmp_block[190~193]
        
        vmull.s16 q13, d22, d0[1]
        vmlal.s16 q13, d23, d1[1]  					@; O[13]
        vadd.s32  q15, q5, q13 								@; E[13]+O[13]
        vsub.s32  q7,  q5, q13 								@; E[13]-O[13]
        vqrshrn.s32 d29, q15, #12 							@; p_tmp_block[130~133]
        vqrshrn.s32 d16, q7,  #12 							@; p_tmp_block[180~183]
        
        vmull.s16 q13, d22, d0[2]
        vmlal.s16 q13, d23, d1[2]  					@; O[14]
        vadd.s32  q15, q4, q13 								@; E[14]+O[14]
        vsub.s32  q9,  q4, q13 								@; E[14]-O[14]
        vqrshrn.s32 d30, q15, #12 							@; p_tmp_block[140~143]
        vqrshrn.s32 d15, q9,  #12 							@; p_tmp_block[170~173]
        
        vmull.s16 q13, d22, d0[3]
        vmlal.s16 q13, d23, d1[3]  					@; O[15]
        vadd.s32  q9,  q3, q13 							@; E[15]+O[15]
        vsub.s32  q10, q3, q13 							@; E[15]-O[15]
        vqrshrn.s32 d31, q9, #12 							@; p_tmp_block[150~153]
        vqrshrn.s32 d14, q10, #12 							@; p_tmp_block[160~163]
        
        @; 转置存储12~15行
        vtrn.16  d28, d29
        vtrn.16  d30, d31
        vtrn.32  q14, q15
        
        vld1.32   {d18[0]}, [r1],r5							@; pPerdiction[120~123]
        vld1.32   {d18[1]}, [r1],r5							@; pPerdiction[130~133]
        vld1.32   {d19[0]}, [r1],r5							@; pPerdiction[140~143]
        vld1.32   {d19[1]}, [r1],r7							@; pPerdiction[150~153]
        sub  r1, r1, #24
        vaddw.u8  q14, q14, d18
        vaddw.u8  q15, q15, d19
        vqmovun.s16 d28, q14
        vqmovun.s16 d29, q15
        
        vst1.32  {d28[0]}, [r2], r3
        vst1.32  {d28[1]}, [r2], r3
        vst1.32  {d29[0]}, [r2], r3
        vst1.32  {d29[1]}, [r2], r8 
        sub  r2, r2, #24       
        
        @; 转置16~20行的4个元素
        vtrn.16  d14, d15
        vtrn.16  d16, d17
        vtrn.32  q7, q8
        
        vld1.32   {d18[0]}, [r1],r5							@; pPerdiction[160~163]
        vld1.32   {d18[1]}, [r1],r5							@; pPerdiction[170~173]
        vld1.32   {d19[0]}, [r1],r5							@; pPerdiction[180~183]
        vld1.32   {d19[1]}, [r1],r9						@; pPerdiction[190~193]
        add  r1, r1, #4
        vaddw.u8  q14, q7, d18
        vaddw.u8  q15, q8, d19
        vqmovun.s16 d28, q14
        vqmovun.s16 d29, q15
        
        vst1.32  {d28[0]}, [r2], r3
        vst1.32  {d28[1]}, [r2], r3
        vst1.32  {d29[0]}, [r2], r3
        vst1.32  {d29[1]}, [r2], r10
        add r2, r2, #4       
        
        
        @;======EO[4]~EO[7],  E[4]~E[11], O[4]~O[11], Resi[4]~[7],[24]~[27],[8]~[11],[20]~[23]
        
        @; vqshll.s16 q12, d4, #6 								@; midValue = pSrc[0] << 6
        vmull.s16  q3, d5, d3[0] 							@; EO[4]
        vmull.s16  q4, d5, d3[1] 							@; EO[5]
        vmull.s16  q5, d5, d3[2] 							@; EO[6]
        vmull.s16  q6, d5, d3[3] 							@; EO[7]
        
        vadd.s32  q8, q12, q3  								@; E[4] = midValue+ EO[k];
        vsub.s32  q3, q12, q3  								@; E[11] = midValue - EO[k];
        
        vadd.s32  q7, q12, q4  								@; E[5] = midValue+ EO[k];
        vsub.s32  q4, q12, q4  								@; E[10] = midValue - EO[k];
        
        vadd.s32  q9, q12, q5  								@; E[6] = midValue+ EO[k];
        vsub.s32  q5, q12, q5  								@; E[9] = midValue - EO[k];
        
        vadd.s32  q10, q12, q6  								@; E[7] = midValue+ EO[k];
        vsub.s32  q6, q12, q6  								@; E[8] = midValue - EO[k];
        
        @;ldr   r4, = kg_IDCT_coef_for_t32_4x4_asm_O
        vld1.16  {d0, d1}, [r4]! 						@; kg_IDCT_coef_for_t32_4x4_asm_O[0~8]
        
        vmull.s16 q13, d22, d0[0]
        vmlal.s16 q13, d23, d1[0]  					@; O[0]
        vadd.s32  q14, q8, q13 								@; E[0]+O[0]
        vsub.s32  q8,  q8, q13 								@; E[0]-O[0]
        vqrshrn.s32 d28, q14, #12 							@; p_tmp_block[00~03]
        vqrshrn.s32 d17, q8,  #12 							@; p_tmp_block[310~313]
        
        vmull.s16 q13, d22, d0[1]
        vmlal.s16 q13, d23, d1[1]  					@; O[1]
        vadd.s32  q15, q7, q13 								@; E[1]+O[1]
        vsub.s32  q7,  q7, q13 								@; E[1]-O[1]
        vqrshrn.s32 d29, q15, #12 							@; p_tmp_block[10~13]
        vqrshrn.s32 d16, q7,  #12 							@; p_tmp_block[300~303]
        
        vmull.s16 q13, d22, d0[2]
        vmlal.s16 q13, d23, d1[2]  					@; O[2]
        vadd.s32  q15, q9, q13 								@; E[2]+O[2]
        vsub.s32  q9,  q9, q13 								@; E[2]-O[2]
        vqrshrn.s32 d30, q15, #12 							@; p_tmp_block[20~23]
        vqrshrn.s32 d15, q9,  #12 							@; p_tmp_block[290~293]
        
        vmull.s16 q13, d22, d0[3]
        vmlal.s16 q13, d23, d1[3]  					@; O[3]
        vadd.s32  q9,  q10, q13 							@; E[3]+O[3]
        vsub.s32  q10, q10, q13 							@; E[3]-O[3]
        vqrshrn.s32 d31, q9, #12 							@; p_tmp_block[30~33]
        vqrshrn.s32 d14, q10, #12 							@; p_tmp_block[280~283]
        
        @; 转置存储4~7行
        vtrn.16  d28, d29
        vtrn.16  d30, d31
        vtrn.32  q14, q15
        pld      [r1]
        vld1.32   {d18[0]}, [r1],r5							@; pPerdiction[00~03]
        vld1.32   {d18[1]}, [r1],r5							@; pPerdiction[10~13]
        vld1.32   {d19[0]}, [r1],r5							@; pPerdiction[20~23]
        vld1.32   {d19[1]}, [r1],r7							@; pPerdiction[30~33]
        sub r1, r1, #8
        vaddw.u8  q14, q14, d18
        vaddw.u8  q15, q15, d19
        vqmovun.s16 d28, q14
        vqmovun.s16 d29, q15
        
        vst1.32  {d28[0]}, [r2], r3
        vst1.32  {d28[1]}, [r2], r3
        vst1.32  {d29[0]}, [r2], r3
        vst1.32  {d29[1]}, [r2], r8
        sub  r2, r2, #8        
        
        @; 转置24~27行的4个元素
        vtrn.16  d14, d15
        vtrn.16  d16, d17
        vtrn.32  q7, q8
        
        vld1.32   {d18[0]}, [r1],r5							@; pPerdiction[280~283]
        vld1.32   {d18[1]}, [r1],r5							@; pPerdiction[290~293]
        vld1.32   {d19[0]}, [r1],r5							@; pPerdiction[300~303]
        vld1.32   {d19[1]}, [r1],r9						@; pPerdiction[310~313]
        
        vaddw.u8  q14, q7, d18
        vaddw.u8  q15, q8, d19
        vqmovun.s16 d28, q14
        vqmovun.s16 d29, q15
        
        vst1.32  {d28[0]}, [r2], r3
        vst1.32  {d28[1]}, [r2], r3
        vst1.32  {d29[0]}, [r2], r3
        vst1.32  {d29[1]}, [r2], r10
        
        vld1.16  {d0, d1}, [r4]! 						@; kg_IDCT_coef_for_t32_4x4_asm_O[0~8]
        @;   -73, -85, -90, -88,     //18~111                   
				@;   82, 88, 54, -4,         //38~311
        
        vmull.s16 q13, d22, d0[0]
        vmlal.s16 q13, d23, d1[0]  					@; O[8]
        vadd.s32  q14, q6, q13 								@; E[8]+O[8]
        vsub.s32  q8,  q6, q13 								@; E[8]-O[8]
        vqrshrn.s32 d28, q14, #12 							@; p_tmp_block[80~83]
        vqrshrn.s32 d17, q8,  #12 							@; p_tmp_block[230~233]
        
        vmull.s16 q13, d22, d0[1]
        vmlal.s16 q13, d23, d1[1]  					@; O[9]
        vadd.s32  q15, q5, q13 								@; E[9]+O[9]
        vsub.s32  q7,  q5, q13 								@; E[9]-O[9]
        vqrshrn.s32 d29, q15, #12 							@; p_tmp_block[90~93]
        vqrshrn.s32 d16, q7,  #12 							@; p_tmp_block[220~223]
        
        vmull.s16 q13, d22, d0[2]
        vmlal.s16 q13, d23, d1[2]  					@; O[10]
        vadd.s32  q15, q4, q13 								@; E[10]+O[10]
        vsub.s32  q9,  q4, q13 								@; E[10]-O[10]
        vqrshrn.s32 d30, q15, #12 							@; p_tmp_block[100~103]
        vqrshrn.s32 d15, q9,  #12 							@; p_tmp_block[210~213]
        
        vmull.s16 q13, d22, d0[3]
        vmlal.s16 q13, d23, d1[3]  					@; O[11]
        vadd.s32  q9,  q3, q13 							@; E[11]+O[11]
        vsub.s32  q10, q3, q13 							@; E[11]-O[11]
        vqrshrn.s32 d31, q9, #12 							@; p_tmp_block[110~113]
        vqrshrn.s32 d14, q10, #12 							@; p_tmp_block[200~203]
        
               
        
        @; 转置存储8~11行
        vtrn.16  d28, d29
        vtrn.16  d30, d31
        vtrn.32  q14, q15
        
        vld1.32   {d18[0]}, [r1],r5							@; pPerdiction[00~03]
        vld1.32   {d18[1]}, [r1],r5							@; pPerdiction[10~13]
        vld1.32   {d19[0]}, [r1],r5							@; pPerdiction[20~23]
        vld1.32   {d19[1]}, [r1],r7							@; pPerdiction[30~33]
        sub    r1, r1, #16
        
        vaddw.u8  q14, q14, d18
        vaddw.u8  q15, q15, d19
        vqmovun.s16 d28, q14
        vqmovun.s16 d29, q15
        
        vst1.32  {d28[0]}, [r2], r3
        vst1.32  {d28[1]}, [r2], r3
        vst1.32  {d29[0]}, [r2], r3
        vst1.32  {d29[1]}, [r2], r8
        sub   r2, r2, #16        
        
        @; 转置20~23行的4个元素
        vtrn.16  d14, d15
        vtrn.16  d16, d17
        vtrn.32  q7, q8
        
        vld1.32   {d18[0]}, [r1],r5							@; pPerdiction[160~163]
        vld1.32   {d18[1]}, [r1],r5							@; pPerdiction[170~173]
        vld1.32   {d19[0]}, [r1],r5							@; pPerdiction[180~183]
        vld1.32   {d19[1]}, [r1],r5						@; pPerdiction[190~193]
        add     r1, r1, #-20
        
        vaddw.u8  q14, q7, d18
        vaddw.u8  q15, q8, d19
        vqmovun.s16 d28, q14
        vqmovun.s16 d29, q15
        
        vst1.32  {d28[0]}, [r2], r3
        vst1.32  {d28[1]}, [r2], r3
        vst1.32  {d29[0]}, [r2], r3
        vst1.32  {d29[1]}, [r2], r3
        add  r2, r2, #-20
               
        subs r6, r6, #1
        bgt IDCT32X32_4X4_ASMV7_4row_loop_2
        ldmfd sp!, {r4, r5, r6, r7,r8,r9, r10, pc}
        .endif											@;if IDCT_ASM_ENABLED==1
        .end
        