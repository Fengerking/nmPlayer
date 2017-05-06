@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@* File Name: NeonDeblockLuma.s
@*            
@*
@* Author: Number Huang
@r0:pSrcDst
@r1:srcdstStep
@r2:pAlpha
@r3:pBeta
@r4:pThresholds,//got from sp+4
@r5:pBS//got from sp+8
@****************************************************************

			@EXPORT  NeonDeblockingLumaV_ASM
     .text
	  .align 2
    .globl  _NeonDeblockingLumaV_ASM
_NeonDeblockingLumaV_ASM:   @PROC
			stmfd     sp!, {r4-r11,lr}@36 byte
			
 			mov		  R11,#4							@r11 is loop count	
 			ldr       R9,  [R2]					@r9=apha+1//inner alpha 					
 			ldr       R5,  [SP, #40]@pt1						
 			ldr       R4,  [SP, #36]@tc0
 			ldr       R10, [R3]					@r10=beta+1//inner beta 			
 			ldr       R7,  [R5]@//, #0x8@r7=str[8]-str[11] 			
		

			and		  R2,R9,#255			@alpha
			and		  R3,R10,#255			@beta			
 			
			cmp       	 R7, #0x0				@next edge if it is zero
 			beq		  NEXT_EDGE
	
BEGIN_EDGE:			
@@@@@@@@@@@@@@@@@@@@@@@@@different bigain@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			sub		R8,R0,#4
 			mov		R6,R0			
 			vld4.8 {D2[0],D3[0],D4[0],D5[0]}, [R8],R1
			vld4.8 {D10[0],D11[0],D12[0],D13[0]}, [R6],R1 			
 			vld4.8 {D2[1],D3[1],D4[1],D5[1]}, [R8],R1
 			vld4.8 {D10[1],D11[1],D12[1],D13[1]}, [R6],R1 			
 			vld4.8 {D2[2],D3[2],D4[2],D5[2]}, [R8],R1
 			vld4.8 {D10[2],D11[2],D12[2],D13[2]}, [R6],R1 
			vdup.32		Q15,R7						
 			vld4.8 {D2[3],D3[3],D4[3],D5[3]}, [R8],R1
 			vld4.8 {D10[3],D11[3],D12[3],D13[3]}, [R6],R1 			
 			vld4.8 {D2[4],D3[4],D4[4],D5[4]}, [R8],R1
 			vld4.8 {D10[4],D11[4],D12[4],D13[4]}, [R6],R1 			
 			vld4.8 {D2[5],D3[5],D4[5],D5[5]}, [R8],R1
 			vld4.8 {D10[5],D11[5],D12[5],D13[5]}, [R6],R1 			
 			vld4.8 {D2[6],D3[6],D4[6],D5[6]}, [R8],R1
 			vld4.8 {D10[6],D11[6],D12[6],D13[6]}, [R6],R1
			vzip.8		D31,D30			@3			 			
 			vld4.8 {D2[7],D3[7],D4[7],D5[7]}, [R8],R1
 			vld4.8 {D10[7],D11[7],D12[7],D13[7]}, [R6],R1 			
 			vld4.8 {D6[0],D7[0],D8[0],D9[0]}, [R8],R1
 			vld4.8 {D14[0],D15[0],D16[0],D17[0]}, [R6],R1 			
 			vld4.8 {D6[1],D7[1],D8[1],D9[1]}, [R8],R1
 			vld4.8 {D14[1],D15[1],D16[1],D17[1]}, [R6],R1 			
 			vld4.8 {D6[2],D7[2],D8[2],D9[2]}, [R8],R1
 			vld4.8 {D14[2],D15[2],D16[2],D17[2]}, [R6],R1
			vcgt.s8		D30,D31,#0    			 			
 			vld4.8 {D6[3],D7[3],D8[3],D9[3]}, [R8],R1
 			vld4.8 {D14[3],D15[3],D16[3],D17[3]}, [R6],R1 			
 			vld4.8 {D6[4],D7[4],D8[4],D9[4]}, [R8],R1
 			vld4.8 {D14[4],D15[4],D16[4],D17[4]}, [R6],R1 			
 			vld4.8 {D6[5],D7[5],D8[5],D9[5]}, [R8],R1
 			vld4.8 {D14[5],D15[5],D16[5],D17[5]}, [R6],R1 			
 			vld4.8 {D6[6],D7[6],D8[6],D9[6]}, [R8],R1
 			vld4.8 {D14[6],D15[6],D16[6],D17[6]}, [R6],R1 			
 			vld4.8 {D6[7],D7[7],D8[7],D9[7]}, [R8]
 			vld4.8 {D14[7],D15[7],D16[7],D17[7]}, [R6]
			vshr.s64 D31, D30, #0	 			
			vswp		D3,D6
			vswp		D5,D8
			vswp		D11,D14
			vswp		D13,D16
			vswp		Q2,Q3			
			vswp		Q6,Q7
@@@@@@@@@@@@@@@@@@@@@@@@@different @end@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			
			@interleave the R7 to Q15,TODO									
			vzip.16		D30,D31			@3
										
			vabd.u8  	Q10,Q5,Q6@tmp1,CR0,CR1						 			
			vdup.8		Q12,R3@Beta_D,Beta_R	
			vabd.u8  	Q11,Q4,Q3							
			vdup.8		Q13,R2@Alpha_D,Alpha_R 			
			@vsub.s8  	Q14,Q5,Q4  @remove by Really Yang 20110503
										
			@tmp2 = L0 - L1@
			vclt.u8  	Q0,Q10,Q12@Flag,tmp1,Beta_D
			vclt.u8	 	Q10,Q11,Q12@tmp2,tmp2,Beta_D
			@vabs.s8	 	Q11,Q14		 @remove by Really Yang 20110503
			vabd.u8   Q11, Q5, Q4 @add by Really Yang 20110503	

			and       R8, R7, #0xff						
		
			@if((tmp1 >= Beta) || (tmp2 >= Beta)) coninue			
			vand.u8	 	Q0,Q0,Q15
			vclt.u8	 	Q15,Q11,Q13					 
 			cmp       R8, #0x4			
			@Delta = R0 - L0@
			@if(abs(Delta) >= Alpha) continue			
							

 			beq		STRTNGTH_4
			vand.u8	 	Q0,Q0,Q10
			vabd.u8  	Q13,Q4,Q2
			ldr		r6,[r4]@tc0 									
			vand.u8	 	Q0,Q0,Q15
			vabd.u8  	Q15,Q5,Q7			
			vclt.u8		Q10,Q13,Q12		@ap
			@vshll.s8	Q13,D29,#2  @remove by Really Yang 20110503
			vsubl.u8 Q13, D11, D9 @add by Really Yang 20110503
			vshl.s16 Q13, Q13, #2 @add by Really Yang 20110503		 						
			vclt.u8		Q9,Q15,Q12		@aq 			
 			@C0			  = tc0[j/4] @		 			
								
	      		vdup.32		Q15,R6	
		  	
		    @tmp1		  = ((Delta << 2) + (L1 - R1) + 4)>>3@
			@vshll.s8		  Q12,D28,#2@(Delta << 2)	 @remove by Really Yang 20110503
			vsubl.u8 q12, d10, d8 @add by Really Yang 20110503
			vshl.s16 q12, q12, #2	@add by Really Yang 20110503			
			vmov.I8		  D2,#4			
			vsubl.u8	  Q8,D7,D13	@L1-R1				
			vzip.8		D31,D30			@3
			vaddw.s8	  Q12,Q12,D2@+4							
			vshr.s32 D30, D31, #0
			vaddw.s8	  Q13,Q13,D2@+4	
			vsubl.u8	  Q1,D6,D12	@L1-R1	
			vadd.s16	  Q1,Q1,Q12@+(L1 - R1)								
			vzip.16		D30,D31			@3

			vadd.s16	  Q8,Q8,Q13@+(L1 - R1)

			vshrn.s16	  D28,Q1,#3@>>3
			vabs.s8	  	Q1,Q9			
			vshrn.s16	  D29,Q8,#3@>>3		@now Q14 is tmp1
			
			@c0 = (C0 + ap + aq) @

			vabs.s8	  	Q8,Q10
			vadd.s8		Q1,Q1,Q15@tmp2_D,C0_D,tmp2_D
			vaddl.u16.u8.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q					
			vadd.s8		Q1,Q1,Q8@tmp2_D,tmp2_D,tmp3_D
			vaddl.u16.u8.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q			
			@CLIP3(-c0,tmp1,c0)
			vneg.s8		Q8,Q1@tmp3_D,tmp2_D
			vmin.s8		Q14,Q14,Q1@tmp1_D,tmp2_D,tmp1_D
			vmax.s8		Q14,Q14,Q8@tmp1_D,tmp3_D,tmp1_D
			vshr.s32 Q8, Q15, #0
			vand.u8		Q1,Q14,Q0@tmp1_D,tmp1_D,Flag.k
			
			vmovl.s8	Q14,D2
			vmovl.s8	Q15,D3			
			vaddw.u8 	Q14,Q14,D8	@L0_Q,L0_Q,tmp1_Q
			vqmovun.s16	D8,Q14			
			vaddw.u8 	Q15,Q15,D9	@L0_Q,L0_Q,tmp1_Q
			vmovl.u8	Q14,D10
			vqmovun.s16	D9,Q15

			@R0  = clip255[(R0 - tmp1)]@
		
			vmovl.u8	Q15,D11			
			vsubw.s8 	Q14,Q14,D2	@L0_Q,L0_Q,tmp1_Q
			vsubw.s8 	Q15,Q15,D3	@L0_Q,L0_Q,tmp1_Q
			vmov.I16	Q1,#1	
			vmovl.u8	Q11,D4					
			vqmovun.s16	D10,Q14
			vqmovun.s16	D11,Q15

			
 			@tmp1 = ((L2 + ((RL0 + 1) >> 1))>>1) - L1) != 0	
			vmovl.u8	Q15,D5 				
			vhadd.s16	Q12,Q12,Q1@tmp1_Q,RL0_Q,tmp2_Q
			vhadd.s16	Q13,Q13,Q1@tmp1_Q,RL0_Q,tmp2_Q							

	 		@@pld [R0]
			vhadd.s16	Q14,Q12,Q11@tmp1_Q,RL0_Q,tmp2_Q
			vhadd.s16	Q15,Q13,Q15@tmp1_Q,RL0_Q,tmp2_Q				
	 		@pld [R0, R1]						
			vsubw.S8 	Q14,Q14,D6	@L0_Q,L0_Q,tmp1_Q
			vsubw.S8 	Q15,Q15,D7	@L0_Q,L0_Q,tmp1_Q
			vmovn.s16	D22,Q14	@tmp1_D,tmp1_Q
			vmovn.s16	D23,Q15 @Now,Q11 is the tmp1 
 				
			vneg.s8		Q15,Q8@Note:Q13 is -C0,and Q8 is C0			
			vmin.s8		Q11,Q8,Q11@tmp1_D,C0_D,tmp1_D
	 		@pld [R0, R1, lsl #1]			
			vmax.s8		Q11,Q15,Q11@tmp1_D,tmp3_D,tmp1_D
			vmovl.u8	Q15,D15				
			vand.u8		Q11,Q11,Q0
			vand.u8		Q11,Q11,Q10@tmp1_D,tmp1_D,ap	
			vadd.u8		Q3,Q3,Q11@L1,L1,tmp1_D
				
			@(tmp1 = ((R2 + ((RL0 + 1) >> 1))>>1) - R1) != 0)			
						
			vmovl.u8	Q11,D14
			vhadd.s16	Q15,Q13,Q15@tmp1_Q,RL0_Q,tmp2_Q		
			vhadd.s16	Q14,Q12,Q11@tmp1_Q,RL0_Q,tmp2_Q
						
			vsubw.s8	Q15,Q15,D13@tmp1_Q,tmp1_Q,R1_Q							
			vsubw.s8	Q14,Q14,D12@tmp1_Q,tmp1_Q,R1_Q				
			vmovn.s16	D22,Q14	@tmp1_D,tmp1_Q
			vmovn.s16	D23,Q15 @Now,Q11 is the tmp1
			
			vneg.s8		Q1,Q8@Note:Q1 is -C0,and Q8 is C0
			vmin.s8		Q11,Q8,Q11@tmp1_D,C0_D,tmp1_D
			vand.u8		Q9,Q9,Q0
			vmax.s8		Q11,Q1,Q11@tmp1_D,tmp3_D,tmp1_D						
			vand.u8		Q11,Q11,Q9@tmp1_D,tmp1_D,aq
			sub	   R8,R0,#2
			vadd.u8		Q6,Q6,Q11@L1,L1,tmp1_D
			
			@store
@@@@@@@@@@@@@@@@@@@@@@@@@different bigain@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			vswp		D7,D8
			vswp	D11,D12
			vswp	Q4,Q5


			vst4.8 {D6[0],D7[0],D8[0],D9[0]}, [R8],R1
 			vst4.8 {D6[1],D7[1],D8[1],D9[1]}, [R8],R1
 			vst4.8 {D6[2],D7[2],D8[2],D9[2]}, [R8],R1
 			vst4.8 {D6[3],D7[3],D8[3],D9[3]}, [R8],R1
 			vst4.8 {D6[4],D7[4],D8[4],D9[4]}, [R8],R1
 			vst4.8 {D6[5],D7[5],D8[5],D9[5]}, [R8],R1
 			vst4.8 {D6[6],D7[6],D8[6],D9[6]}, [R8],R1
 			vst4.8 {D6[7],D7[7],D8[7],D9[7]}, [R8],R1
 			vst4.8 {D10[0],D11[0],D12[0],D13[0]}, [R8],R1
 			vst4.8 {D10[1],D11[1],D12[1],D13[1]}, [R8],R1
 			vst4.8 {D10[2],D11[2],D12[2],D13[2]}, [R8],R1
 			vst4.8 {D10[3],D11[3],D12[3],D13[3]}, [R8],R1
 			vst4.8 {D10[4],D11[4],D12[4],D13[4]}, [R8],R1
 			vst4.8 {D10[5],D11[5],D12[5],D13[5]}, [R8],R1
 			vst4.8 {D10[6],D11[6],D12[6],D13[6]}, [R8],R1
 			vst4.8 {D10[7],D11[7],D12[7],D13[7]}, [R8],R1
@@@@@@@@@@@@@@@@@@@@@@@@@different @end@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 			b	NEXT_EDGE
STRTNGTH_4:
			@tmp1 = R0 - R2@
			@tmp2 = L0 - L2@		
			@aq   = tmp1 < Beta@ap   = tmp2 < Beta@
			vand.u8	 	Q0,Q0,Q10			
			vabd.u8  	Q13,Q4,Q2
			mov		R8,#2			
			vand.u8	 	Q0,Q0,Q15
			vabd.u8		Q11,Q5,Q4	@absDelta			
			add		R8, R8, R2, lsr #2						
			vabd.u8  	Q15,Q5,Q7			
			vclt.u8		Q10,Q13,Q12		@ap 						
			vclt.u8		Q9,Q15,Q12		@aq		
			@small_gap = (AbsDelta < ((Alpha >> 2) + 2))@

			vdup.8		Q15,R8@Alpha_D,Alpha_R						
			
			vaddl.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q
			vaddl.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q
			
			@vclt.s8		Q11,Q11,Q15@small_gap,small_gap,Alpha_D @remove by Really Yang 20110503
			vclt.u8		Q11,Q11,Q15@small_gap,small_gap,Alpha_D add by Really Yang 20110503
			vmov.I8		  D30,#4			
			vaddw.u8	Q12,Q12,D12@tmp1_Q,RL0_Q,R1_Q						
			vaddw.u8	Q13,Q13,D13@tmp1_Q,RL0_Q,R1_Q			
			vand.s8		Q9,Q11,Q9@aq,small_gap,aq
			vand.s8		Q10,Q11,Q10@ap,small_gap,ap
			
			@pt2[  0 ] = (L1 + ((tmp1 = RL0 + R1)<<1) +  R2 + 4)>>3@
			@RL0
			@tmp1=RL0+R1	
			@<<1		
		vshl.s16	Q12,#1@tmp1_Q,#1			
		vshl.s16	Q13,#1@tmp1_Q,#1
	
			@+4						

			vaddw.s8	Q12,Q12,D30
			vaddw.s8	Q13,Q13,D30
			
			vmov.I8		  D28,#2			
			@+R2
			vaddw.u8	Q12,Q12,D14@
			vaddw.u8	Q13,Q13,D15@			
			@+L1
			vaddw.u8	Q12,Q12,D6@
			vaddw.u8	Q13,Q13,D7@
			@>>3
			vshrn.s16	D22,Q12,#3
			vshll.u8	Q12,D12,#1		@R1 << 1			
			vshrn.s16	D23,Q13,#3	@now Q11 is R0
			@pt2[  0 ] = ((R1 << 1) + R0 + L1 + 2)>>2@
			@R1 << 1
			vshll.u8	Q13,D13,#1		@R1 << 1
			@+ 2
			
			vaddw.s8	Q12,Q12,D28
			vaddw.s8	Q13,Q13,D28
			
			@+R0
			vaddw.u8	Q12,Q12,D10@
			vaddw.u8	Q13,Q13,D11@
			@+L1
			vaddw.u8	Q12,Q12,D6@
			vaddw.u8	Q13,Q13,D7@
			@>>2
			vand.s8		Q14,Q11,Q9@tmp3_D,tmp3_D,aq			
			vshrn.s16	D30,Q12,#2
			vceq.u8		Q12,Q9,#0@tmp6_D,aq,#0			
			vshrn.s16	D31,Q13,#2	@@now Q15 is another R0
			@now choose one R0
			vaddl.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q
			vand.s8		Q15,Q12,Q15@tmp2_D,tmp2_D,tmp6_D			
			vorr.s8		Q14,Q15,Q14@
			vaddl.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q				
			@choose R0 by Flag
			vand.s8		Q15,Q14,Q0

			vceq.u8		Q14,Q0,#0@tmp6_D,Flag,#0
			vaddw.u8	Q13,Q13,D13@tmp1_Q,RL0_Q,R1_Q
			vand.s8		Q14,Q5,Q14
			vaddw.u8	Q12,Q12,D12@tmp1_Q,RL0_Q,R1_Q			
			vorr.s8		Q11,Q15,Q14@	now Q11 is last R0
			
			@pt2[inc ] = ((tmp1 += R2) + 2)>>2@
			@RL0
			
			@tmp1=RL0+R1				
			@tmp1 += R2
			vmov.I8		  D30,#2			
			vaddw.u8	Q12,Q12,D14@tmp1_Q,RL0_Q,R1_Q						
			vaddw.u8	Q13,Q13,D15@tmp1_Q,RL0_Q,R1_Q
			@+2

			vaddw.s8	Q12,Q12,D30@tmp2_Q,tmp1_Q,tmp2_Q
			vaddw.s8	Q13,Q13,D30@tmp2_Q,tmp1_Q,tmp2_Q
			vshrn.s16	D28,Q12,#2
			vshrn.s16	D29,Q13,#2		@now Q14 is R1
			@pt2[inc2] = (((pt2[inc3] + R2)<<1) + tmp1 + 4)>>3@
			@RL0
			vaddl.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q
			vaddl.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q
			@tmp1=RL0+R1
			vaddw.u8	Q12,Q12,D12@tmp1_Q,RL0_Q,R1_Q						
			vaddw.u8	Q13,Q13,D13@tmp1_Q,RL0_Q,R1_Q
			@tmp1 += R2
			vaddw.u8	Q12,Q12,D14@tmp1_Q,RL0_Q,R1_Q						
			vaddw.u8	Q13,Q13,D15@tmp1_Q,RL0_Q,R1_Q
			@+R3<<1
			vshll.u8	Q15,D16,#1	@R3
			vadd.s16	Q12,Q12,Q15@tmp1_Q,RL0_Q,R1_Q
			vshll.u8	Q15,D17,#1	@R3
			vadd.s16	Q13,Q13,Q15@tmp1_Q,RL0_Q,R1_Q
			@+R2<<1
			vshll.u8	Q15,D14,#1	@R2
			vadd.s16	Q12,Q12,Q15@tmp1_Q,RL0_Q,R1_Q
			vshll.u8	Q15,D15,#1	@R2
			vadd.s16	Q13,Q13,Q15@tmp1_Q,RL0_Q,R1_Q
			@+4
			vmov.I16		  Q15,#4
			vadd.s16	Q12,Q12,Q15
			vadd.s16	Q13,Q13,Q15
			vand.s8		Q9,Q9,Q0			
			@>>3
			vshrn.s16	D30,Q12,#3
			vshrn.s16	D31,Q13,#3	@Q15:R2  
			
			vand.s8		Q14,Q14,Q9
			vand.s8		Q15,Q15,Q9
			vceq.u8		Q9,Q9,#0
			
			vand.s8		Q12,Q6,Q9
			vand.s8		Q13,Q7,Q9
			vorr.s8		Q8,Q12,Q14
			vorr.s8		Q9,Q13,Q15
			
			
			@(R1 + ((RL0 += L1)<<1) + L2 + 4)>>3@
			@RL0
			vaddl.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q
			vaddl.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q
			vmov.I16		  Q15,#4			
			@tmp1=RL0+L1
			vaddw.u8	Q12,Q12,D6@tmp1_Q,RL0_Q,R1_Q						
			vaddw.u8	Q13,Q13,D7@tmp1_Q,RL0_Q,R1_Q
			
			@<<1
			
			vshl.s16	Q12,#1@tmp1_Q,#1
			vshl.s16	Q13,#1@tmp1_Q,#1
			@+4

			vadd.s16	Q12,Q15,Q12
			vadd.s16	Q13,Q15,Q13
			
			@+L2
			vaddw.u8	Q12,Q12,D4@tmp1_Q,RL0_Q,R1_Q						
			vaddw.u8	Q13,Q13,D5@tmp1_Q,RL0_Q,R1_Q
			vmov.I16		  Q14,#2			
			@+R1 Q6
			vaddw.u8	Q12,Q12,D12@tmp1_Q,RL0_Q,R1_Q						
			vaddw.u8	Q13,Q13,D13@tmp1_Q,RL0_Q,R1_Q			
			@>>3
			vshrn.s16	D30,Q12,#3
			vshrn.s16	D31,Q13,#3	@now Q15 is L0
			@pt2[-inc ] = ((L1 << 1) + L0 + R1 + 2)>>2 @
			@L1 << 1:Q3
			
			vshll.u8	Q12,D6,#1		@R1 << 1
			vshll.u8	Q13,D7,#1		@R1 << 1
			@+ 2

			vadd.s16	Q12,Q12,Q14
			vadd.s16	Q13,Q13,Q14
			@+L0:Q4
			vaddw.u8	Q12,Q12,D8@tmp1_Q,RL0_Q,R1_Q						
			vaddw.u8	Q13,Q13,D9@tmp1_Q,RL0_Q,R1_Q
			@+R1:Q6
			vaddw.u8	Q12,Q12,D12@tmp1_Q,RL0_Q,R1_Q						
			vaddw.u8	Q13,Q13,D13@tmp1_Q,RL0_Q,R1_Q
			@>>2
			vshrn.s16	D28,Q12,#2
			vshrn.s16	D29,Q13,#2	@@now Q14 is another L0
			@now choose one R0
			vand.s8		Q15,Q15,Q10@tmp3_D,tmp3_D,aq
			vceq.u8		Q12,Q10,#0@tmp6_D,aq,#0
			vceq.u8		Q13,Q0,#0@tmp6_D,Flag,#0			
			vand.s8		Q14,Q12,Q14@tmp2_D,tmp2_D,tmp6_D
			vand.s8		Q13,Q4,Q13			
			vorr.s8		Q14,Q15,Q14@	
			@choose R0 by Flag
			vand.s8		Q12,Q14,Q0
			vmov.I16		  Q14,#2			
			vorr.s8		Q15,Q12,Q13@	now Q15 is last L0
			
			@pt2[-inc2] = ((RL0 += L2) + 2)>>2@
			@RL0
			vaddl.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q
			vaddl.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q
	 @pld [R0]			
			@+L1:Q3
			vaddw.u8	Q12,Q12,D6@tmp1_Q,RL0_Q,R1_Q						
			vaddw.u8	Q13,Q13,D7@tmp1_Q,RL0_Q,R1_Q
	 @pld [R0, R1]			
			@+L2:Q2
			
			vaddw.u8	Q12,Q12,D4@tmp1_Q,RL0_Q,R1_Q						
			vaddw.u8	Q13,Q13,D5@tmp1_Q,RL0_Q,R1_Q
	 @pld [R0, R1, lsl #1]  			
			@+2

			vadd.s16	Q12,Q12,Q14@tmp2_Q,tmp1_Q,tmp2_Q
			vadd.s16	Q13,Q13,Q14@tmp2_Q,tmp1_Q,tmp2_Q
			vshrn.s16	D28,Q12,#2
			vshrn.s16	D29,Q13,#2		@now Q14 is L1
			@pt2[-inc3] = (((pt2[-(inc2<<1)] + L2)<<1) + RL0 + 4)>>3@
			@RL0
			vaddl.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q
			vaddl.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q
			@R0 is not used any more,reuse Q11
			vshr.s32 Q5, Q11, #0
			
			@+L1 Q3
			vaddw.u8	Q12,Q12,D6@tmp1_Q,RL0_Q,R1_Q						
			vaddw.u8	Q13,Q13,D7@tmp1_Q,RL0_Q,R1_Q			
			@+L2 Q2
			vshll.u8	Q11,D2,#1	@R3			
			vaddw.u8	Q12,Q12,D4@tmp1_Q,RL0_Q,R1_Q						
			vaddw.u8	Q13,Q13,D5@tmp1_Q,RL0_Q,R1_Q
			@+L3<<1 Q1
			vadd.s16	Q12,Q12,Q11@tmp1_Q,RL0_Q,R1_Q
			vshll.u8	Q11,D3,#1	@R3
			vadd.s16	Q13,Q13,Q11@tmp1_Q,RL0_Q,R1_Q
			@+L2<<1 Q2
			vshll.u8	Q11,D4,#1	@R2
			vadd.s16	Q12,Q12,Q11@tmp1_Q,RL0_Q,R1_Q
			vshll.u8	Q11,D5,#1	@R2
			vadd.s16	Q13,Q13,Q11@tmp1_Q,RL0_Q,R1_Q
			@+4
			vmov.I16		  Q11,#4
			vadd.s16	Q12,Q12,Q11
			vadd.s16	Q11,Q13,Q11
			@>>3
			vshrn.s16	D26,Q12,#3
			vshrn.s16	D27,Q11,#3	@Q13:L2  
					
			vand.s8		Q10,Q10,Q0
			sub		  R8,R0,#3			
			vand.s8		Q13,Q13,Q10
			vand.s8		Q14,Q14,Q10
			vceq.u8		Q10,Q10,#0
 			mov		  R6,R0			
			vand.s8		Q11,Q2,Q10
			vand.s8		Q12,Q3,Q10
			vorr.s8		Q2,Q11,Q13
			vorr.s8		Q3,Q12,Q14

			vshr.s32 Q4, Q15, #0

			vshr.s32 Q6, Q8, #0
			vshr.s32 Q7, Q9, #0
 			@store 6 columns

@@@@@@@@@@@@@@@@@@@@@@@@@different bigain@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			
			vswp	  D5,D6
			vswp	  D7,D8
			vswp	  D7,D6
 			vswp	  D11,D12
			vswp	  D13,D14
			vswp	  D12,D13
			vst3.8    {D4[0],D5[0],D6[0]}, [R8],R1
			vst3.8    {D10[0],D11[0],D12[0]}, [R6],R1			
 			vst3.8    {D4[1],D5[1],D6[1]}, [R8],R1
 			vst3.8    {D10[1],D11[1],D12[1]}, [R6],R1 			
 			vst3.8    {D4[2],D5[2],D6[2]}, [R8],R1
 			vst3.8    {D10[2],D11[2],D12[2]}, [R6],R1 			
 			vst3.8    {D4[3],D5[3],D6[3]}, [R8],R1
 			vst3.8    {D10[3],D11[3],D12[3]}, [R6],R1 			
 			vst3.8    {D4[4],D5[4],D6[4]}, [R8],R1
 			vst3.8    {D10[4],D11[4],D12[4]}, [R6],R1 			
 			vst3.8    {D4[5],D5[5],D6[5]}, [R8],R1
 			vst3.8    {D10[5],D11[5],D12[5]}, [R6],R1 			
 			vst3.8    {D4[6],D5[6],D6[6]}, [R8],R1
 			vst3.8    {D10[6],D11[6],D12[6]}, [R6],R1 			
 			vst3.8    {D4[7],D5[7],D6[7]}, [R8],R1
 			vst3.8    {D10[7],D11[7],D12[7]}, [R6],R1 			
 			vst3.8    {D7[0],D8[0],D9[0]}, [R8],R1
 			vst3.8    {D13[0],D14[0],D15[0]}, [R6],R1 			
 			vst3.8    {D7[1],D8[1],D9[1]}, [R8],R1
 			vst3.8    {D13[1],D14[1],D15[1]}, [R6],R1 			
 			vst3.8    {D7[2],D8[2],D9[2]}, [R8],R1
 			vst3.8    {D13[2],D14[2],D15[2]}, [R6],R1 			
 			vst3.8    {D7[3],D8[3],D9[3]}, [R8],R1
 			vst3.8    {D13[3],D14[3],D15[3]}, [R6],R1 			
 			vst3.8    {D7[4],D8[4],D9[4]}, [R8],R1
 			vst3.8    {D13[4],D14[4],D15[4]}, [R6],R1 			
 			vst3.8    {D7[5],D8[5],D9[5]}, [R8],R1
 			vst3.8    {D13[5],D14[5],D15[5]}, [R6],R1 			
 			vst3.8    {D7[6],D8[6],D9[6]}, [R8],R1
 			vst3.8    {D13[6],D14[6],D15[6]}, [R6],R1 			
 			vst3.8    {D7[7],D8[7],D9[7]}, [R8],R1
 			vst3.8    {D13[7],D14[7],D15[7]}, [R6],R1
 					
NEXT_EDGE:
			
			subs		R11,R11,#1
			beq		NeonDeblockLuma_end
			add		R5,R5,#4
			add		r0,r0,#4@src
@@@@@@@@@@@@@@@@@@@@@@@@@different @end@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			ldr	   	R7,[R5]@

			add		r4,r4,#4@tc0
			
			
			lsr    		R2,R9,#8
			lsr		R3,R10,#8
			
 			cmp     R7, #0x0
 			bne		BEGIN_EDGE	
 			beq		NEXT_EDGE			

NeonDeblockLuma_end:			
 			ldmfd           sp!, {r4 - r11,pc}  
			@endP
			@end    
