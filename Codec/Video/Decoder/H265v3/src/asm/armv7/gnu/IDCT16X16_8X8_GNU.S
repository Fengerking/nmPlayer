@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@void IDCT16X16(
@@							const short *pSrcData,
@@							const unsigned char *pPerdictionData,
@@							unsigned char *pDstRecoData,
@@							unsigned int uiDstStride)
@@ 处理4列的数据
@@ short g_IDCT16X16H_EO[16] = 
@@				{
@@					 89,  75,  50,  18,																																				
@@           75, -18, -89, -50,																																				
@@           50, -89,  18, +75 																																				
@@           18, -50,  75, -89
@@        }@ 																																															  
@@ short g_IDCT16X16H_EEE[8] = 
@@        {
@@				   64, 64, 83, 36,
@@           64,-64, 36,-83
@@ 				}@ 
@@ short g_IDCT16X16H_O[8 * 8] =
@@		    {
@@		        90, 87, 80, 70, 57, 43, 25,  9,
@@		        87, 57,  9,-43,-80,-90,-70,-25,
@@		        80,  9,-70,-87,-25, 57, 90, 43,
@@		        70,-43,-87,  9, 90, 25,-80,-57,
@@		        57,-80,-25, 90, -9,-87, 43, 70,
@@		        43,-90, 57, 25,-87, 70,  9,-80,
@@		        25,-70, 90,-80, 43,  9,-57, 87,
@@		        9,-25, 43,-57, 70,-80, 87,-90
@@		    }@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@ 1506cyles vs c：22341（-o3 -A8 rvds4.0) 
	#include     "h265dec_ASM_config.h"
        #include "../../../h265dec_ID.h"
        @#include "../../../h265dec_config.h"
        .section .text
        .align 4
        .if IDCT_ASM_ENABLED==1
        @.extern kg_IDCT_coef_for_t16_asm
        .global IDCT16X16_8X8_ASMV7
kg_IDCT_coef_for_t16_asm:		
	.hword 64, 83, 64, 36, 89, 75, 50, 18
	.hword 90, 87, 80, 70, 57, 43, 25, 9    
       
        
        
IDCT16X16_8X8_ASMV7:  
        stmfd sp!, {r4, r5, r6, r7,r8,r9, r10, r11, lr}
         @@ 512字节放pTmpBlock[64]的值，但是存储顺序是使用vld4.16的，
        @@ 并保留空间为pTmpBlock是16字节对齐
        sub  sp, sp, #528 			
        and  r5, sp, #0xF
        rsb  r5, r5, #0x10
        add  r5, sp, r5     						@@ pTmpBlock
        mov  r11, r0
        
        @@ g_IDCT16X16H_EEE,g_IDCT16X16H_EO,g_IDCT16X16H_O
        adr   r12, kg_IDCT_coef_for_t16_asm
        vld1.16  {d0, d1, d2, d3},  [r12]
        @@ add   r4, r5, #96											@@ pTmpBlock+12*16
        mov   r6, #64   			 				 	  @@ 存储在sp的跨度1         
        mov   r8, #96 										@@ 存储在sp的跨度2
        mov   r12, #-64 									@@ 存储在sp的跨度3
        
        ldr   r10, =-160	 								@@ pSrc[6*16]的地址到pSrc+16的跨度  -64*3+32=-160
        ldr   r7, =-216  									@@ pSrc+16  -64*3-32+8=-216
        
        mov   r9, #2 										@@ j=4
        
IDCT16X16_8X8_4Row_ASM_loop:        
        vld1.16  {d4},  [r0], r6 							@@pSrc[0]...[3]
        vld1.16  {d5},  [r0], r6 							@@pSrc[2*16]...[3]
        vld1.16  {d6},  [r0], r6 							@@pSrc[4*16]...[3]
        vld1.16  {d7},  [r0], r10 							@@pSrc[6*16]...[3]
        
        
        vmull.s16 q6, d4, d0[0] 							@@ 64 * pSrc[0]=EEE[0]        
        vmull.s16 q7, d4, d0[0] 							@@ 64 * pSrc[0]=EEE[1]       
        vmull.s16 q8, d6, d0[1] 							@@ 83 * pSrc[0]=EE0[0]        
        vmull.s16 q9, d6, d0[3] 							@@ 36 * pSrc[0]=EE0[1]
        
        vadd.s32  q10, q6, q8 								@@ EE[0] = EEE[0] + EEO[0]@
        vadd.s32  q11, q7, q9 								@@ EE[1] = EEE[1] + EEO[1]@
        vsub.s32  q13, q6, q8 								@@ EE[3] = EEE[0] - EEO[0]@
        vsub.s32  q12, q7, q9 								@@ EE[2] = EEE[1] - EEO[1]@
        
        vmull.s16 q6, d5, d1[0]
        vmlal.s16 q6, d7, d1[1]                             @@ EO[0]
        
        vmull.s16 q7, d5, d1[1]
        vmlsl.s16 q7, d7, d1[3]                             @@ EO[1]
        
        vmull.s16 q8, d5, d1[2]
        vmlsl.s16 q8, d7, d1[0]                             @@ EO[2]
        
        vmull.s16 q9, d5, d1[3]
        vmlsl.s16 q9, d7, d1[2]                             @@ EO[3]
        
        vadd.s32  q2, q10, q6 								@@ E[0]
        vadd.s32  q3, q11, q7 								@@ E[1]
        vadd.s32  q4, q12, q8 								@@ E[2]
        vadd.s32  q5, q13, q9 								@@ E[3]
        
        vsub.s32  q6, q10, q6 								@@ E[7]
        vsub.s32  q7, q11, q7 								@@ E[6]
        vsub.s32  q8, q12, q8 								@@ E[5]
        vsub.s32  q9, q13, q9 								@@ E[4]
       
        
        vld1.16  {d20},  [r0], r6 							@@pSrc[1*16]...[3]
        vld1.16  {d21},  [r0], r6 							@@pSrc[3*16]...[3]
        vld1.16  {d22},  [r0], r6 							@@pSrc[5*16]...[3]
        vld1.16  {d23},  [r0], r7 							@@pSrc[7*16]...[3]
        
        vmull.s16 q15, d20, d2[0]
        vmlal.s16 q15, d21, d2[1]
        vmlal.s16 q15, d22, d2[2]
        vmlal.s16 q15, d23, d2[3]                           @@ O[0]
        
        vadd.s32  q14, q2, q15  								@@ E[0] + O[0]
        vsub.s32  q2,  q2, q15  								@@ E[0] - O[0]
        vqrshrn.s32 d28, q14, #7 								@@ pTmpBlock[00~03]
        vqrshrn.s32 d29, q2,  #7 								@@ pTmpBlock[150~153]      
        
        vmull.s16 q15, d20, d2[1]
        vmlal.s16 q15, d21, d3[0]
        vmlal.s16 q15, d22, d3[3]
        vmlsl.s16 q15, d23, d3[1]                               @@ O[1]
        
        vadd.s32  q2,  q3, q15  								@@ E[1] + O[1]
        vsub.s32  q3,  q3, q15  								@@ E[1] - O[1]
        vqrshrn.s32 d5, q2, #7  								@@ pTmpBlock[10~13]
        vqrshrn.s32 d4,  q3,  #7 								@@ pTmpBlock[140~143]
        
        vmull.s16 q3, d20, d2[2]
        vmlal.s16 q3, d21, d3[3]
        vmlsl.s16 q3, d22, d2[3]
        vmlsl.s16 q3, d23, d2[1]                                @@ O[2]
        
        vadd.s32  q15, q4, q3  								@@ E[2] + O[2]
        vsub.s32  q3,  q4, q3  								@@ E[2] - O[2]
        vqrshrn.s32 d30, q15, #7 								@@ pTmpBlock[20~23]
        vqrshrn.s32 d31,  q3,  #7 								@@ pTmpBlock[130~133]
        
        
        vmull.s16 q4, d20, d2[3]
        vmlsl.s16 q4, d21, d3[1]
        vmlsl.s16 q4, d22, d2[1]
        vmlal.s16 q4, d23, d3[3]                            @@ O[3]
        
        vadd.s32  q3, q5, q4  								@@ E[3] + O[3]
        vsub.s32  q5, q5, q4  								@@ E[3] - O[3]
        vqrshrn.s32 d7, q3, #7 								@@ pTmpBlock[30~33]
        vqrshrn.s32 d6,  q5,  #7 								@@ pTmpBlock[120~123]
        
        @@ 转置存储0~3行和15~12行的4个元素
        vswp    d5, d29
        vswp    d7, d31
        vswp    q2, q3
        @@ d28~d31存放的是00~03,10~13,20~23,30~33
        @@ d4~d7存放的是 120~123,130~133,140~143,150~153
        vst4.16 {d28, d29, d30, d31}, [r5],r8
        vst4.16 {d4, d5, d6, d7}, [r5],r12       
                
        vmull.s16 q14, d20, d3[0]
        vmlsl.s16 q14, d21, d2[2]
        vmlsl.s16 q14, d22, d3[2]
        vmlal.s16 q14, d23, d2[0]                               @@ O[4]
        
        vadd.s32  q15, q9, q14  								@@ E[4] + O[4]
        vsub.s32  q14, q9, q14  								@@ E[4] - O[4]
        vqrshrn.s32 d4, q15, #7 								@@ pTmpBlock[40~43]
        vqrshrn.s32 d11,q14,  #7 								@@ pTmpBlock[110~113]
        
        
        vmull.s16 q14, d20, d3[1]
        vmlsl.s16 q14, d21, d2[0]
        vmlal.s16 q14, d22, d3[0]
        vmlal.s16 q14, d23, d3[2]                               @@ O[5]
        
        vadd.s32  q15, q8, q14  								@@ E[5] + O[5]
        vsub.s32  q14, q8, q14  								@@ E[5] - O[5]
        vqrshrn.s32 d5, q15, #7 								@@ pTmpBlock[50~53]
        vqrshrn.s32 d10,  q14,  #7 								@@ pTmpBlock[100~103]
        
        
        vmull.s16 q14, d20, d3[2]
        vmlsl.s16 q14, d21, d2[3]
        vmlal.s16 q14, d22, d2[0]
        vmlsl.s16 q14, d23, d2[2]                               @@ O[6]
        
        vadd.s32  q15, q7, q14  								@@ E[6] + O[6]
        vsub.s32  q14,  q7, q14  								@@ E[6] - O[6]
        vqrshrn.s32 d6, q15, #7 								@@ pTmpBlock[60~63]
        vqrshrn.s32 d9,  q14,  #7 								@@ pTmpBlock[90~93]
       
        
        vmull.s16 q14, d20, d3[3]
        vmlsl.s16 q14, d21, d3[2]
        vmlal.s16 q14, d22, d3[1]
        vmlsl.s16 q14, d23, d3[0]                               @@ O[7]
        
        vadd.s32  q15, q6, q14  								@@ E[7] + O[7]
        vsub.s32  q14,  q6, q14  								@@ E[7] - O[7]
        vqrshrn.s32 d7, q15, #7 								@@ pTmpBlock[70~73]
        vqrshrn.s32 d8,  q14,  #7								@@ pTmpBlock[80~83]
        vst4.16 {d4, d5, d6, d7}, [r5]! 			@@ 4*8+64
        vst4.16 {d8, d9,d10, d11},[r5],r6         @@ 32*2  pTmpBlock+15*16

        subs r9, r9, #1 													@@ j--
        bgt  IDCT16X16_8X8_4Row_ASM_loop
        
        mov  r9, #4
        sub  r0, r5, #256 							@@ 新pSrc的首地址，为上一次循环的堆栈中
        mov  r5, r11 										@@ 恢复pTmpBlock的首地址，此时存储在原始pSrc的内存中
        mov  r4, #16  			 				 	@@ 在sp中，2 line的跨度 4*2*2        
        mov  r11, #112								@@ 在sp中，2~4行的跨度 36*3+16
        ldr  r10, =-120 									@@ pSrc先恢复到原始地址，再找到pSrc+4的偏移处：-(16*2+112*1)-8+32=-120
        ldr  r7, =-136									@@ pSrc先恢复到原始地址，再找到pSrc[1*16]的偏移处：-(16*2+112*1)+8=-136
IDCT16X16_8X8_4Row_ASM_loop_2:
        vld1.16  {d4},  [r0], r4 							@@pSrc[0]...[3]
        vld1.16  {d5},  [r0], r11 							@@pSrc[2*16]...[3]
        vld1.16  {d6},  [r0], r4 							@@pSrc[4*16]...[3]
        vld1.16  {d7},  [r0], r7 							@@pSrc[6*16]...[3]
        
        
        @@ 该循环内的主要部分和上一个循环是一样的。只是取数方式不一样
        vmull.s16 q6, d4, d0[0] 							@@ 64 * pSrc[0]=EEE[0]        
        vmull.s16 q7, d4, d0[0] 							@@ 64 * pSrc[0]=EEE[1]        
        vmull.s16 q8, d6, d0[1] 							@@ 83 * pSrc[0]=EE0[0]        
        vmull.s16 q9, d6, d0[3] 							@@ 36 * pSrc[0]=EE0[1]
        
        vadd.s32  q10, q6, q8 								@@ EE[0] = EEE[0] + EEO[0]@
        vadd.s32  q11, q7, q9 								@@ EE[1] = EEE[1] + EEO[1]@
        vsub.s32  q13, q6, q8 								@@ EE[3] = EEE[0] - EEO[0]@
        vsub.s32  q12, q7, q9 								@@ EE[2] = EEE[1] - EEO[1]@
        
        vmull.s16 q6, d5, d1[0]
        vmlal.s16 q6, d7, d1[1]                             @@ EO[0]
        
        vmull.s16 q7, d5, d1[1]
        vmlsl.s16 q7, d7, d1[3]                             @@ EO[1]
        
        vmull.s16 q8, d5, d1[2]
        vmlsl.s16 q8, d7, d1[0]                             @@ EO[2]
        
        vmull.s16 q9, d5, d1[3]
        vmlsl.s16 q9, d7, d1[2]                             @@ EO[3]
        
        vadd.s32  q2, q10, q6 								@@ E[0]
        vadd.s32  q3, q11, q7 								@@ E[1]
        vadd.s32  q4, q12, q8 								@@ E[2]
        vadd.s32  q5, q13, q9 								@@ E[3]
        
        vsub.s32  q6, q10, q6 								@@ E[7]
        vsub.s32  q7, q11, q7 								@@ E[6]
        vsub.s32  q8, q12, q8 								@@ E[5]
        vsub.s32  q9, q13, q9 								@@ E[4]
       
        
        vld1.16  {d20},  [r0], r4 							@@pSrc[1*16]...[3]
        vld1.16  {d21},  [r0], r11 							@@pSrc[3*16]...[3]
        vld1.16  {d22},  [r0], r4  							@@pSrc[5*16]...[3]
        vld1.16  {d23},  [r0], r10 							@@pSrc[7*16]...[3]
        
        vmull.s16 q15, d20, d2[0]
        vmlal.s16 q15, d21, d2[1]
        vmlal.s16 q15, d22, d2[2]
        vmlal.s16 q15, d23, d2[3]                           @@ O[0]
        
        vadd.s32  q14, q2, q15  								@@ E[0] + O[0]
        vsub.s32  q2,  q2, q15  								@@ E[0] - O[0]
        vqrshrn.s32 d28, q14, #12 								@@ pTmpBlock[00~03]
        vqrshrn.s32 d29, q2,  #12 								@@ pTmpBlock[150~153]      
        
        vmull.s16 q15, d20, d2[1]
        vmlal.s16 q15, d21, d3[0]
        vmlal.s16 q15, d22, d3[3]
        vmlsl.s16 q15, d23, d3[1]                           @@ O[1]
        
        vadd.s32  q2,  q3, q15  								@@ E[1] + O[1]
        vsub.s32  q3,  q3, q15  								@@ E[1] - O[1]
        vqrshrn.s32 d5, q2, #12  								@@ pTmpBlock[10~13]
        vqrshrn.s32 d4,  q3,  #12 								@@ pTmpBlock[140~143]
        
        vmull.s16 q3, d20, d2[2]
        vmlal.s16 q3, d21, d3[3]
        vmlsl.s16 q3, d22, d2[3]
        vmlsl.s16 q3, d23, d2[1]                            @@ O[2]
        
        vadd.s32  q15, q4, q3  								@@ E[2] + O[2]
        vsub.s32  q3,  q4, q3  								@@ E[2] - O[2]
        vqrshrn.s32 d30, q15, #12 								@@ pTmpBlock[20~23]
        vqrshrn.s32 d31,  q3,  #12 								@@ pTmpBlock[130~133]
        
        
        vmull.s16 q4, d20, d2[3]
        vmlsl.s16 q4, d21, d3[1]
        vmlsl.s16 q4, d22, d2[1]
        vmlal.s16 q4, d23, d3[3]                            @@ O[3]
        
        vadd.s32  q3, q5, q4  								@@ E[3] + O[3]
        vsub.s32  q5, q5, q4  								@@ E[3] - O[3]
        vqrshrn.s32 d7, q3, #12 								@@ pTmpBlock[30~33]
        vqrshrn.s32 d6,  q5,  #12 								@@ pTmpBlock[120~123]
        
        @@ 转置存储0~3行和15~12行的4个元素
        vswp    d5, d29
        vswp    d7, d31
        vswp    q2, q3
        @@ d28~d31存放的是00~03,10~13,20~23,30~33
        @@ d4~d7存放的是 120~123,130~133,140~143,150~153
        vst4.16 {d28, d29, d30, d31}, [r5],r8
        vst4.16 {d4, d5, d6, d7}, [r5],r12        
                
        vmull.s16 q14, d20, d3[0]
        vmlsl.s16 q14, d21, d2[2]
        vmlsl.s16 q14, d22, d3[2]
        vmlal.s16 q14, d23, d2[0]                               @@ O[4]
        
        vadd.s32  q15, q9, q14  								@@ E[4] + O[4]
        vsub.s32  q14, q9, q14  								@@ E[4] - O[4]
        vqrshrn.s32 d4, q15, #12 								@@ pTmpBlock[40~43]
        vqrshrn.s32 d11,q14,  #12 								@@ pTmpBlock[110~113]
        
        
        vmull.s16 q14, d20, d3[1]
        vmlsl.s16 q14, d21, d2[0]
        vmlal.s16 q14, d22, d3[0]
        vmlal.s16 q14, d23, d3[2]                               @@ O[5]
        
        vadd.s32  q15, q8, q14  								@@ E[5] + O[5]
        vsub.s32  q14, q8, q14  								@@ E[5] - O[5]
        vqrshrn.s32 d5, q15, #12 								@@ pTmpBlock[50~53]
        vqrshrn.s32 d10,  q14,  #12 								@@ pTmpBlock[100~103]
        
        
        vmull.s16 q14, d20, d3[2]
        vmlsl.s16 q14, d21, d2[3]
        vmlal.s16 q14, d22, d2[0]
        vmlsl.s16 q14, d23, d2[2]                               @@ O[6]
        
        vadd.s32  q15, q7, q14  								@@ E[6] + O[6]
        vsub.s32  q14,  q7, q14  								@@ E[6] - O[6]
        vqrshrn.s32 d6, q15, #12 								@@ pTmpBlock[60~63]
        vqrshrn.s32 d9,  q14,  #12 								@@ pTmpBlock[90~93]
       
        
        vmull.s16 q14, d20, d3[3]
        vmlsl.s16 q14, d21, d3[2]
        vmlal.s16 q14, d22, d3[1]
        vmlsl.s16 q14, d23, d3[0]                               @@ O[7]
        
        vadd.s32  q15, q6, q14  								@@ E[7] + O[7]
        vsub.s32  q14,  q6, q14  								@@ E[7] - O[7]
        vqrshrn.s32 d7, q15, #12 								@@ pTmpBlock[70~73]
        vqrshrn.s32 d8,  q14,  #12								@@ pTmpBlock[80~83]
        vst4.16 {d4, d5, d6, d7}, [r5]!
        vst4.16 {d8, d9,d10, d11},[r5],r6

        subs r9, r9, #1 													@@ j--
        bgt  IDCT16X16_8X8_4Row_ASM_loop_2
        
        
        @@ 取出来的变换值是转置后的，将其与预测值相加得到最终值
        
        @@ 恢复pTmpBlock的首地址应该和r11相等 
        sub  r5, r5, #512 										
        mov  r6, #32
        @@ mov  r7, #PRED_CACHE_STRIDE
        add  sp, sp, #528
        @ldr  r7, [sp, #36]
        mov  r4, r1
        mov  r9, #15
        mov  r12, #8
        mov  r8, #-88
IDCT16X16_8X8_ASMV7_Dstloop:        

        vld1.16  {d4},  [r5], r6 							@@残差[01]...[03]
        vld1.16  {d5},  [r5], r6 							@@残差[04]...[07]
        vld1.16  {d6},  [r5], r6 							@@残差[08]...[011]
        vld1.16  {d7},  [r5], r8 							@@残差[012]...[015]
        
        @@ pPerdiction
        vld1.8  {d8, d9}, [r1], r2 					@@ pPerdiction[0]...[15]
        
        vaddw.u8 q7, q2, d8
        vaddw.u8 q8, q3, d9
        vqmovun.s16 d18, q7
        vqmovun.s16 d19, q8
        
        vst1.u8 {d18, d19}, [r4], r2					@@ pTmpBlock[0~015]
        
        vld1.16  {d4},  [r5], r6 							@@残差[10]...[13]
        vld1.16  {d5},  [r5], r6 							@@残差[14]...[17]
        vld1.16  {d6},  [r5], r6 							@@残差[18]...[111]
        vld1.16  {d7},  [r5], r8 							@@残差[112]...[115]
        
        @@ pPerdiction
        vld1.8  {d8, d9}, [r1], r2 					@@ pPerdiction[10]...[115]
        vaddw.u8 q7, q2, d8
        vaddw.u8 q8, q3, d9
        vqmovun.s16 d18, q7
        vqmovun.s16 d19, q8
        
        vst1.u8 {d18, d19}, [r4], r2					@@ pTmpBlock[10~115]
        
        vld1.16  {d4},  [r5], r6 							@@残差[20]...[23]
        vld1.16  {d5},  [r5], r6 							@@残差[24]...[27]
        vld1.16  {d6},  [r5], r6 							@@残差[28]...[211]
        vld1.16  {d7},  [r5], r8 							@@残差[212]...[215]
        
        @@ pPerdiction
        vld1.8  {d8, d9}, [r1], r2 					@@ pPerdiction[20]...[215]
        vaddw.u8 q7, q2, d8
        vaddw.u8 q8, q3, d9
        vqmovun.s16 d18, q7
        vqmovun.s16 d19, q8
        
        vst1.u8 {d18, d19}, [r4], r2					@@ pTmpBlock[20~215]
        
        vld1.16  {d4},  [r5], r6 							@@残差[30]...[33]
        vld1.16  {d5},  [r5], r6 							@@残差[34]...[37]
        vld1.16  {d6},  [r5], r6 							@@残差[38]...[311]
        vld1.16  {d7},  [r5], r12 							@@残差[312]...[315]
        
        @@ pPerdiction
        vld1.8  {d8, d9}, [r1], r2 					@@ pPerdiction[30]...[315]
        vaddw.u8 q7, q2, d8
        vaddw.u8 q8, q3, d9
        vqmovun.s16 d18, q7
        vqmovun.s16 d19, q8
        
        vst1.u8 {d18, d19}, [r4], r2					@@ pTmpBlock[30~315]
        
        subs r9, r9 , #4
        bge  IDCT16X16_8X8_ASMV7_Dstloop
        
        
        ldmfd sp!, {r4, r5, r6, r7,r8,r9, r10,r11, pc}
        .endif    @.if IDCT_ASM_ENABLED==1
        .end