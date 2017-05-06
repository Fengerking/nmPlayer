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
    .globl  NeonDeblockingLumaV_ASM
NeonDeblockingLumaV_ASM:   @PROC
@	int inc=srcdstStep@
@	int inc2 = (inc<<1)@
@	int inc3 = inc2 + inc@
@	int i=1,j=0,str=0,edge=0@
@	avdUInt8 *pt2=NULL,*pt1	= pBS@
@	avdUInt8* srcY	= pSrcDst@
@	int srcInc=1@
@	avdUInt8* tc0=pThresholds@
@	avdUInt8* alpha2	=	pAlpha@
@	avdUInt8* beta2		=	pBeta@
@	avdUInt8* clip255 = &img->clip255[0]@
@	int L0,L1,L2,R0,R1,R2,tmp1,tmp2,Beta,Alpha,Delta,AbsDelta,aq,ap,RL0,C0,c0,small_gap@


			STMFD     sp!, {r4-r11,lr}@36 byte
 			MOV		  R11,#4							@r11 is loop count	
 			LDR       R9,  [R2]					@r9=apha+1//inner alpha 					
 			LDR       R5,  [SP, #40]@pt1
						
 			LDR       R4,  [SP, #36]@tc0


 			LDR       R10, [R3]					@r10=beta+1//inner beta
 			
 			LDR       R7,  [R5]@//, #0x8@r7=str[8]-str[11] 			
		

			AND		  R2,R9,#255			@alpha
			AND		  R3,R10,#255			@beta			
 			

										@r14:next_half flag
			CMP       	 R7, #0x0				@next edge if it is zero
 			BEQ		  NEXT_EDGE
	
BEGIN_EDGE:			
@@@@@@@@@@@@@@@@@@@@@@@@@different bigain@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			SUB		R8,R0,#4
 			MOV		R6,R0			
 			VLD4.8 {D2[0],D3[0],D4[0],D5[0]}, [R8],R1
			VLD4.8 {D10[0],D11[0],D12[0],D13[0]}, [R6],R1 			
 			VLD4.8 {D2[1],D3[1],D4[1],D5[1]}, [R8],R1
 			VLD4.8 {D10[1],D11[1],D12[1],D13[1]}, [R6],R1 			
 			VLD4.8 {D2[2],D3[2],D4[2],D5[2]}, [R8],R1
 			VLD4.8 {D10[2],D11[2],D12[2],D13[2]}, [R6],R1 
			VDUP.32		Q15,R7						
 			VLD4.8 {D2[3],D3[3],D4[3],D5[3]}, [R8],R1
 			VLD4.8 {D10[3],D11[3],D12[3],D13[3]}, [R6],R1 			
 			VLD4.8 {D2[4],D3[4],D4[4],D5[4]}, [R8],R1
 			VLD4.8 {D10[4],D11[4],D12[4],D13[4]}, [R6],R1 			
 			VLD4.8 {D2[5],D3[5],D4[5],D5[5]}, [R8],R1
 			VLD4.8 {D10[5],D11[5],D12[5],D13[5]}, [R6],R1 			
 			VLD4.8 {D2[6],D3[6],D4[6],D5[6]}, [R8],R1
 			VLD4.8 {D10[6],D11[6],D12[6],D13[6]}, [R6],R1
			VZIP.8		D31,D30			@3			 			
 			VLD4.8 {D2[7],D3[7],D4[7],D5[7]}, [R8],R1
 			VLD4.8 {D10[7],D11[7],D12[7],D13[7]}, [R6],R1 			
 			VLD4.8 {D6[0],D7[0],D8[0],D9[0]}, [R8],R1
 			VLD4.8 {D14[0],D15[0],D16[0],D17[0]}, [R6],R1 			
 			VLD4.8 {D6[1],D7[1],D8[1],D9[1]}, [R8],R1
 			VLD4.8 {D14[1],D15[1],D16[1],D17[1]}, [R6],R1 			
 			VLD4.8 {D6[2],D7[2],D8[2],D9[2]}, [R8],R1
 			VLD4.8 {D14[2],D15[2],D16[2],D17[2]}, [R6],R1
			VCGT.s8		D30,D31,#0    			 			
 			VLD4.8 {D6[3],D7[3],D8[3],D9[3]}, [R8],R1
 			VLD4.8 {D14[3],D15[3],D16[3],D17[3]}, [R6],R1 			
 			VLD4.8 {D6[4],D7[4],D8[4],D9[4]}, [R8],R1
 			VLD4.8 {D14[4],D15[4],D16[4],D17[4]}, [R6],R1 			
 			VLD4.8 {D6[5],D7[5],D8[5],D9[5]}, [R8],R1
 			VLD4.8 {D14[5],D15[5],D16[5],D17[5]}, [R6],R1 			
 			VLD4.8 {D6[6],D7[6],D8[6],D9[6]}, [R8],R1
 			VLD4.8 {D14[6],D15[6],D16[6],D17[6]}, [R6],R1 			
 			VLD4.8 {D6[7],D7[7],D8[7],D9[7]}, [R8]
 			VLD4.8 {D14[7],D15[7],D16[7],D17[7]}, [R6]
			vshr.s64 D31, D30, #0	 			
			VSWP		D3,D6
			VSWP		D5,D8
			VSWP		D11,D14
			VSWP		D13,D16
			VSWP		Q2,Q3			
			VSWP		Q6,Q7
@@@@@@@@@@@@@@@@@@@@@@@@@different @end@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			
			@interleave the R7 to Q15,TODO									
			VZIP.16		D30,D31			@3
										
			VABD.u8  	Q10,Q5,Q6@tmp1,CR0,CR1						 			
			VDUP.8		Q12,R3@Beta_D,Beta_R	
			VABD.u8  	Q11,Q4,Q3							
			VDUP.8		Q13,R2@Alpha_D,Alpha_R 			
			VSUB.s8  	Q14,Q5,Q4
			
									
			@tmp2 = L0 - L1@
			VCLT.u8  	Q0,Q10,Q12@Flag,tmp1,Beta_D
			VCLT.u8	 	Q10,Q11,Q12@tmp2,tmp2,Beta_D
			VABS.s8	 	Q11,Q14			

			AND       R8, R7, #0xff						
		
			@if((tmp1 >= Beta) || (tmp2 >= Beta)) coninue			
			VAND.u8	 	Q0,Q0,Q15
			VCLT.u8	 	Q15,Q11,Q13					 
 			CMP       R8, #0x4			
			@Delta = R0 - L0@
			@if(abs(Delta) >= Alpha) continue			
							

 			BEQ		STRTNGTH_4
 			@STRENGTH_NOT4 $L1,$L0,$R0,$R1,$tc0_R,$C0_D,$Delta,$ap,$aq,$tmp1_Q,$tmp2_Q,$tmp3_Q,$tmp1_D,$tmp2_D,$tmp3_D,$L1_Q,$L0_Q,$R0_Q,$R1_Q
			@tmp1 = R0 - R2@
			@tmp2 = L0 - L2@		
			@aq   = tmp1 < Beta@ap   = tmp2 < Beta@
			VAND.u8	 	Q0,Q0,Q10
			VABD.u8  	Q13,Q4,Q2
			ldr		r6,[r4]@tc0 									
			VAND.u8	 	Q0,Q0,Q15
			VABD.u8  	Q15,Q5,Q7			
			VCLT.u8		Q10,Q13,Q12		@ap
			VSHLL.s8	Q13,D29,#2			 						
			VCLT.u8		Q9,Q15,Q12		@aq 			
 			@C0			  = tc0[j/4] @		 			
								
	      		VDUP.32		Q15,R6	
		  	
		    @tmp1		  = ((Delta << 2) + (L1 - R1) + 4)>>3@
			VSHLL.s8		  Q12,D28,#2@(Delta << 2)			
			VMOV.I8		  D2,#4			
			VSUBL.u8	  Q8,D7,D13	@L1-R1				
			VZIP.8		D31,D30			@3
			VADDW.s8	  Q12,Q12,D2@+4							
			vshr.s32 D30, D31, #0
			VADDW.s8	  Q13,Q13,D2@+4	
			VSUBL.u8	  Q1,D6,D12	@L1-R1	
			VADD.s16	  Q1,Q1,Q12@+(L1 - R1)								
			VZIP.16		D30,D31			@3

			VADD.s16	  Q8,Q8,Q13@+(L1 - R1)

			VSHRN.s16	  D28,Q1,#3@>>3
			VABS.s8	  	Q1,Q9			
			VSHRN.s16	  D29,Q8,#3@>>3		@now Q14 is tmp1
			
			@c0 = (C0 + ap + aq) @

			VABS.s8	  	Q8,Q10
			VADD.s8		Q1,Q1,Q15@tmp2_D,C0_D,tmp2_D
			VADDL.u16.u8.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q					
			VADD.s8		Q1,Q1,Q8@tmp2_D,tmp2_D,tmp3_D
			VADDL.u16.u8.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q			
			@CLIP3(-c0,tmp1,c0)
			VNEG.s8		Q8,Q1@tmp3_D,tmp2_D
			VMIN.s8		Q14,Q14,Q1@tmp1_D,tmp2_D,tmp1_D
			VMAX.s8		Q14,Q14,Q8@tmp1_D,tmp3_D,tmp1_D
			vshr.s32 Q8, Q15, #0
			VAND.u8		Q1,Q14,Q0@tmp1_D,tmp1_D,Flag.k
			
			@L0  = clip255[(L0 + tmp1)]@


			
			VMOVL.s8	Q14,D2
			VMOVL.s8	Q15,D3			
			VADDW.u8 	Q14,Q14,D8	@L0_Q,L0_Q,tmp1_Q
			VQMOVUN.s16	D8,Q14			
			VADDW.u8 	Q15,Q15,D9	@L0_Q,L0_Q,tmp1_Q
			VMOVL.u8	Q14,D10
			VQMOVUN.s16	D9,Q15

			@R0  = clip255[(R0 - tmp1)]@
			

			VMOVL.u8	Q15,D11			
			VSUBW.s8 	Q14,Q14,D2	@L0_Q,L0_Q,tmp1_Q
			VSUBW.s8 	Q15,Q15,D3	@L0_Q,L0_Q,tmp1_Q
			VMOV.I16	Q1,#1	
			VMOVL.u8	Q11,D4					
			VQMOVUN.s16	D10,Q14
			VQMOVUN.s16	D11,Q15

			
 			@tmp1 = ((L2 + ((RL0 + 1) >> 1))>>1) - L1) != 0	
			VMOVL.u8	Q15,D5 				
			VHADD.s16	Q12,Q12,Q1@tmp1_Q,RL0_Q,tmp2_Q
			VHADD.s16	Q13,Q13,Q1@tmp1_Q,RL0_Q,tmp2_Q							

	 pld [R0]
			VHADD.s16	Q14,Q12,Q11@tmp1_Q,RL0_Q,tmp2_Q
			VHADD.s16	Q15,Q13,Q15@tmp1_Q,RL0_Q,tmp2_Q				
	 pld [R0, R1]						
			VSUBW.S8 	Q14,Q14,D6	@L0_Q,L0_Q,tmp1_Q
			VSUBW.S8 	Q15,Q15,D7	@L0_Q,L0_Q,tmp1_Q
			VMOVN.s16	D22,Q14	@tmp1_D,tmp1_Q
			VMOVN.s16	D23,Q15 @Now,Q11 is the tmp1 
 				
			VNEG.s8		Q15,Q8@Note:Q13 is -C0,and Q8 is C0			
			VMIN.s8		Q11,Q8,Q11@tmp1_D,C0_D,tmp1_D
	 pld [R0, R1, lsl #1]			
			VMAX.s8		Q11,Q15,Q11@tmp1_D,tmp3_D,tmp1_D
			VMOVL.u8	Q15,D15				
			VAND.u8		Q11,Q11,Q0
			VAND.u8		Q11,Q11,Q10@tmp1_D,tmp1_D,ap	
			VADD.u8		Q3,Q3,Q11@L1,L1,tmp1_D
				
			@(tmp1 = ((R2 + ((RL0 + 1) >> 1))>>1) - R1) != 0)			
						
			VMOVL.u8	Q11,D14
			VHADD.s16	Q15,Q13,Q15@tmp1_Q,RL0_Q,tmp2_Q		
			VHADD.s16	Q14,Q12,Q11@tmp1_Q,RL0_Q,tmp2_Q
						
			VSUBW.s8	Q15,Q15,D13@tmp1_Q,tmp1_Q,R1_Q							
			VSUBW.s8	Q14,Q14,D12@tmp1_Q,tmp1_Q,R1_Q				
			VMOVN.s16	D22,Q14	@tmp1_D,tmp1_Q
			VMOVN.s16	D23,Q15 @Now,Q11 is the tmp1
			
			VNEG.s8		Q1,Q8@Note:Q1 is -C0,and Q8 is C0
			VMIN.s8		Q11,Q8,Q11@tmp1_D,C0_D,tmp1_D
			VAND.u8		Q9,Q9,Q0
			VMAX.s8		Q11,Q1,Q11@tmp1_D,tmp3_D,tmp1_D						
			VAND.u8		Q11,Q11,Q9@tmp1_D,tmp1_D,aq
			SUB	   R8,R0,#2
			VADD.u8		Q6,Q6,Q11@L1,L1,tmp1_D
			
			@store
@@@@@@@@@@@@@@@@@@@@@@@@@different bigain@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			VSWP		D7,D8
			VSWP	D11,D12
			VSWP	Q4,Q5


			VST4.8 {D6[0],D7[0],D8[0],D9[0]}, [R8],R1
 			VST4.8 {D6[1],D7[1],D8[1],D9[1]}, [R8],R1
 			VST4.8 {D6[2],D7[2],D8[2],D9[2]}, [R8],R1
 			VST4.8 {D6[3],D7[3],D8[3],D9[3]}, [R8],R1
 			VST4.8 {D6[4],D7[4],D8[4],D9[4]}, [R8],R1
 			VST4.8 {D6[5],D7[5],D8[5],D9[5]}, [R8],R1
 			VST4.8 {D6[6],D7[6],D8[6],D9[6]}, [R8],R1
 			VST4.8 {D6[7],D7[7],D8[7],D9[7]}, [R8],R1
 			VST4.8 {D10[0],D11[0],D12[0],D13[0]}, [R8],R1
 			VST4.8 {D10[1],D11[1],D12[1],D13[1]}, [R8],R1
 			VST4.8 {D10[2],D11[2],D12[2],D13[2]}, [R8],R1
 			VST4.8 {D10[3],D11[3],D12[3],D13[3]}, [R8],R1
 			VST4.8 {D10[4],D11[4],D12[4],D13[4]}, [R8],R1
 			VST4.8 {D10[5],D11[5],D12[5],D13[5]}, [R8],R1
 			VST4.8 {D10[6],D11[6],D12[6],D13[6]}, [R8],R1
 			VST4.8 {D10[7],D11[7],D12[7],D13[7]}, [R8],R1
@@@@@@@@@@@@@@@@@@@@@@@@@different @end@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 			B	NEXT_EDGE
STRTNGTH_4:
			@tmp1 = R0 - R2@
			@tmp2 = L0 - L2@		
			@aq   = tmp1 < Beta@ap   = tmp2 < Beta@
			VAND.u8	 	Q0,Q0,Q10			
			VABD.u8  	Q13,Q4,Q2
			MOV		R8,#2			
			VAND.u8	 	Q0,Q0,Q15
			VABD.u8		Q11,Q5,Q4	@absDelta			
			ADD		R8, R8, R2, LSR #2						
			VABD.u8  	Q15,Q5,Q7			
			VCLT.u8		Q10,Q13,Q12		@ap 						
			VCLT.u8		Q9,Q15,Q12		@aq		
			@small_gap = (AbsDelta < ((Alpha >> 2) + 2))@

			VDUP.8		Q15,R8@Alpha_D,Alpha_R						
			
			VADDL.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q
			VADDL.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q
			
			VCLT.s8		Q11,Q11,Q15@small_gap,small_gap,Alpha_D
			VMOV.I8		  D30,#4			
			VADDW.u8	Q12,Q12,D12@tmp1_Q,RL0_Q,R1_Q						
			VADDW.u8	Q13,Q13,D13@tmp1_Q,RL0_Q,R1_Q			
			VAND.s8		Q9,Q11,Q9@aq,small_gap,aq
			VAND.s8		Q10,Q11,Q10@ap,small_gap,ap
			
			@pt2[  0 ] = (L1 + ((tmp1 = RL0 + R1)<<1) +  R2 + 4)>>3@
			@RL0
			@tmp1=RL0+R1	
			@<<1		
		VSHL.s16	Q12,#1@tmp1_Q,#1			
		VSHL.s16	Q13,#1@tmp1_Q,#1
	
			@+4						

			VADDW.s8	Q12,Q12,D30
			VADDW.s8	Q13,Q13,D30
			
			VMOV.I8		  D28,#2			
			@+R2
			VADDW.u8	Q12,Q12,D14@
			VADDW.u8	Q13,Q13,D15@			
			@+L1
			VADDW.u8	Q12,Q12,D6@
			VADDW.u8	Q13,Q13,D7@
			@>>3
			VSHRN.s16	D22,Q12,#3
			VSHLL.u8	Q12,D12,#1		@R1 << 1			
			VSHRN.s16	D23,Q13,#3	@now Q11 is R0
			@pt2[  0 ] = ((R1 << 1) + R0 + L1 + 2)>>2@
			@R1 << 1
			VSHLL.u8	Q13,D13,#1		@R1 << 1
			@+ 2
			
			VADDW.s8	Q12,Q12,D28
			VADDW.s8	Q13,Q13,D28
			
			@+R0
			VADDW.u8	Q12,Q12,D10@
			VADDW.u8	Q13,Q13,D11@
			@+L1
			VADDW.u8	Q12,Q12,D6@
			VADDW.u8	Q13,Q13,D7@
			@>>2
			VAND.s8		Q14,Q11,Q9@tmp3_D,tmp3_D,aq			
			VSHRN.s16	D30,Q12,#2
			VCEQ.u8		Q12,Q9,#0@tmp6_D,aq,#0			
			VSHRN.s16	D31,Q13,#2	@@now Q15 is another R0
			@now choose one R0
			VADDL.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q
			VAND.s8		Q15,Q12,Q15@tmp2_D,tmp2_D,tmp6_D			
			VORR.s8		Q14,Q15,Q14@
			VADDL.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q				
			@choose R0 by Flag
			VAND.s8		Q15,Q14,Q0

			VCEQ.u8		Q14,Q0,#0@tmp6_D,Flag,#0
			VADDW.u8	Q13,Q13,D13@tmp1_Q,RL0_Q,R1_Q
			VAND.s8		Q14,Q5,Q14
			VADDW.u8	Q12,Q12,D12@tmp1_Q,RL0_Q,R1_Q			
			VORR.s8		Q11,Q15,Q14@	now Q11 is last R0
			
			@pt2[inc ] = ((tmp1 += R2) + 2)>>2@
			@RL0
			
			@tmp1=RL0+R1				
			@tmp1 += R2
			VMOV.I8		  D30,#2			
			VADDW.u8	Q12,Q12,D14@tmp1_Q,RL0_Q,R1_Q						
			VADDW.u8	Q13,Q13,D15@tmp1_Q,RL0_Q,R1_Q
			@+2

			VADDW.s8	Q12,Q12,D30@tmp2_Q,tmp1_Q,tmp2_Q
			VADDW.s8	Q13,Q13,D30@tmp2_Q,tmp1_Q,tmp2_Q
			VSHRN.s16	D28,Q12,#2
			VSHRN.s16	D29,Q13,#2		@now Q14 is R1
			@pt2[inc2] = (((pt2[inc3] + R2)<<1) + tmp1 + 4)>>3@
			@RL0
			VADDL.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q
			VADDL.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q
			@tmp1=RL0+R1
			VADDW.u8	Q12,Q12,D12@tmp1_Q,RL0_Q,R1_Q						
			VADDW.u8	Q13,Q13,D13@tmp1_Q,RL0_Q,R1_Q
			@tmp1 += R2
			VADDW.u8	Q12,Q12,D14@tmp1_Q,RL0_Q,R1_Q						
			VADDW.u8	Q13,Q13,D15@tmp1_Q,RL0_Q,R1_Q
			@+R3<<1
			VSHLL.u8	Q15,D16,#1	@R3
			VADD.s16	Q12,Q12,Q15@tmp1_Q,RL0_Q,R1_Q
			VSHLL.u8	Q15,D17,#1	@R3
			VADD.s16	Q13,Q13,Q15@tmp1_Q,RL0_Q,R1_Q
			@+R2<<1
			VSHLL.u8	Q15,D14,#1	@R2
			VADD.s16	Q12,Q12,Q15@tmp1_Q,RL0_Q,R1_Q
			VSHLL.u8	Q15,D15,#1	@R2
			VADD.s16	Q13,Q13,Q15@tmp1_Q,RL0_Q,R1_Q
			@+4
			VMOV.I16		  Q15,#4
			VADD.s16	Q12,Q12,Q15
			VADD.s16	Q13,Q13,Q15
			VAND.s8		Q9,Q9,Q0			
			@>>3
			VSHRN.s16	D30,Q12,#3
			VSHRN.s16	D31,Q13,#3	@Q15:R2  
			
			VAND.s8		Q14,Q14,Q9
			VAND.s8		Q15,Q15,Q9
			VCEQ.u8		Q9,Q9,#0
			
			VAND.s8		Q12,Q6,Q9
			VAND.s8		Q13,Q7,Q9
			VORR.s8		Q8,Q12,Q14
			VORR.s8		Q9,Q13,Q15
			
			
			@(R1 + ((RL0 += L1)<<1) + L2 + 4)>>3@
			@RL0
			VADDL.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q
			VADDL.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q
			VMOV.I16		  Q15,#4			
			@tmp1=RL0+L1
			VADDW.u8	Q12,Q12,D6@tmp1_Q,RL0_Q,R1_Q						
			VADDW.u8	Q13,Q13,D7@tmp1_Q,RL0_Q,R1_Q
			
			@<<1
			
			VSHL.s16	Q12,#1@tmp1_Q,#1
			VSHL.s16	Q13,#1@tmp1_Q,#1
			@+4

			VADD.s16	Q12,Q15,Q12
			VADD.s16	Q13,Q15,Q13
			
			@+L2
			VADDW.u8	Q12,Q12,D4@tmp1_Q,RL0_Q,R1_Q						
			VADDW.u8	Q13,Q13,D5@tmp1_Q,RL0_Q,R1_Q
			VMOV.I16		  Q14,#2			
			@+R1 Q6
			VADDW.u8	Q12,Q12,D12@tmp1_Q,RL0_Q,R1_Q						
			VADDW.u8	Q13,Q13,D13@tmp1_Q,RL0_Q,R1_Q			
			@>>3
			VSHRN.s16	D30,Q12,#3
			VSHRN.s16	D31,Q13,#3	@now Q15 is L0
			@pt2[-inc ] = ((L1 << 1) + L0 + R1 + 2)>>2 @
			@L1 << 1:Q3
			
			VSHLL.u8	Q12,D6,#1		@R1 << 1
			VSHLL.u8	Q13,D7,#1		@R1 << 1
			@+ 2

			VADD.s16	Q12,Q12,Q14
			VADD.s16	Q13,Q13,Q14
			@+L0:Q4
			VADDW.u8	Q12,Q12,D8@tmp1_Q,RL0_Q,R1_Q						
			VADDW.u8	Q13,Q13,D9@tmp1_Q,RL0_Q,R1_Q
			@+R1:Q6
			VADDW.u8	Q12,Q12,D12@tmp1_Q,RL0_Q,R1_Q						
			VADDW.u8	Q13,Q13,D13@tmp1_Q,RL0_Q,R1_Q
			@>>2
			VSHRN.s16	D28,Q12,#2
			VSHRN.s16	D29,Q13,#2	@@now Q14 is another L0
			@now choose one R0
			VAND.s8		Q15,Q15,Q10@tmp3_D,tmp3_D,aq
			VCEQ.u8		Q12,Q10,#0@tmp6_D,aq,#0
			VCEQ.u8		Q13,Q0,#0@tmp6_D,Flag,#0			
			VAND.s8		Q14,Q12,Q14@tmp2_D,tmp2_D,tmp6_D
			VAND.s8		Q13,Q4,Q13			
			VORR.s8		Q14,Q15,Q14@	
			@choose R0 by Flag
			VAND.s8		Q12,Q14,Q0
			VMOV.I16		  Q14,#2			
			VORR.s8		Q15,Q12,Q13@	now Q15 is last L0
			
			@pt2[-inc2] = ((RL0 += L2) + 2)>>2@
			@RL0
			VADDL.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q
			VADDL.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q
	 pld [R0]			
			@+L1:Q3
			VADDW.u8	Q12,Q12,D6@tmp1_Q,RL0_Q,R1_Q						
			VADDW.u8	Q13,Q13,D7@tmp1_Q,RL0_Q,R1_Q
	 pld [R0, R1]			
			@+L2:Q2
			
			VADDW.u8	Q12,Q12,D4@tmp1_Q,RL0_Q,R1_Q						
			VADDW.u8	Q13,Q13,D5@tmp1_Q,RL0_Q,R1_Q
	 pld [R0, R1, lsl #1]  			
			@+2

			VADD.s16	Q12,Q12,Q14@tmp2_Q,tmp1_Q,tmp2_Q
			VADD.s16	Q13,Q13,Q14@tmp2_Q,tmp1_Q,tmp2_Q
			VSHRN.s16	D28,Q12,#2
			VSHRN.s16	D29,Q13,#2		@now Q14 is L1
			@pt2[-inc3] = (((pt2[-(inc2<<1)] + L2)<<1) + RL0 + 4)>>3@
			@RL0
			VADDL.u8  Q12,	D8,D10@//RL0_Q,L0_Q,R0_Q
			VADDL.u8  Q13,	D9,D11@//RL0_Q,L0_Q,R0_Q
			@R0 is not used any more,reuse Q11
			vshr.s32 Q5, Q11, #0
			
			@+L1 Q3
			VADDW.u8	Q12,Q12,D6@tmp1_Q,RL0_Q,R1_Q						
			VADDW.u8	Q13,Q13,D7@tmp1_Q,RL0_Q,R1_Q			
			@+L2 Q2
			VSHLL.u8	Q11,D2,#1	@R3			
			VADDW.u8	Q12,Q12,D4@tmp1_Q,RL0_Q,R1_Q						
			VADDW.u8	Q13,Q13,D5@tmp1_Q,RL0_Q,R1_Q
			@+L3<<1 Q1
			VADD.s16	Q12,Q12,Q11@tmp1_Q,RL0_Q,R1_Q
			VSHLL.u8	Q11,D3,#1	@R3
			VADD.s16	Q13,Q13,Q11@tmp1_Q,RL0_Q,R1_Q
			@+L2<<1 Q2
			VSHLL.u8	Q11,D4,#1	@R2
			VADD.s16	Q12,Q12,Q11@tmp1_Q,RL0_Q,R1_Q
			VSHLL.u8	Q11,D5,#1	@R2
			VADD.s16	Q13,Q13,Q11@tmp1_Q,RL0_Q,R1_Q
			@+4
			VMOV.I16		  Q11,#4
			VADD.s16	Q12,Q12,Q11
			VADD.s16	Q11,Q13,Q11
			@>>3
			VSHRN.s16	D26,Q12,#3
			VSHRN.s16	D27,Q11,#3	@Q13:L2  
			
			
			VAND.s8		Q10,Q10,Q0
			SUB		  R8,R0,#3			
			VAND.s8		Q13,Q13,Q10
			VAND.s8		Q14,Q14,Q10
			VCEQ.u8		Q10,Q10,#0
 			MOV		  R6,R0			
			VAND.s8		Q11,Q2,Q10
			VAND.s8		Q12,Q3,Q10
			VORR.s8		Q2,Q11,Q13
			VORR.s8		Q3,Q12,Q14

			vshr.s32 Q4, Q15, #0

			vshr.s32 Q6, Q8, #0
			vshr.s32 Q7, Q9, #0
 			@store 6 columns

@@@@@@@@@@@@@@@@@@@@@@@@@different bigain@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			
			VSWP	  D5,D6
			VSWP	  D7,D8
			VSWP	  D7,D6
 			VSWP	  D11,D12
			VSWP	  D13,D14
			VSWP	  D12,D13
			VST3.8    {D4[0],D5[0],D6[0]}, [R8],R1
			VST3.8    {D10[0],D11[0],D12[0]}, [R6],R1			
 			VST3.8    {D4[1],D5[1],D6[1]}, [R8],R1
 			VST3.8    {D10[1],D11[1],D12[1]}, [R6],R1 			
 			VST3.8    {D4[2],D5[2],D6[2]}, [R8],R1
 			VST3.8    {D10[2],D11[2],D12[2]}, [R6],R1 			
 			VST3.8    {D4[3],D5[3],D6[3]}, [R8],R1
 			VST3.8    {D10[3],D11[3],D12[3]}, [R6],R1 			
 			VST3.8    {D4[4],D5[4],D6[4]}, [R8],R1
 			VST3.8    {D10[4],D11[4],D12[4]}, [R6],R1 			
 			VST3.8    {D4[5],D5[5],D6[5]}, [R8],R1
 			VST3.8    {D10[5],D11[5],D12[5]}, [R6],R1 			
 			VST3.8    {D4[6],D5[6],D6[6]}, [R8],R1
 			VST3.8    {D10[6],D11[6],D12[6]}, [R6],R1 			
 			VST3.8    {D4[7],D5[7],D6[7]}, [R8],R1
 			VST3.8    {D10[7],D11[7],D12[7]}, [R6],R1 			
 			VST3.8    {D7[0],D8[0],D9[0]}, [R8],R1
 			VST3.8    {D13[0],D14[0],D15[0]}, [R6],R1 			
 			VST3.8    {D7[1],D8[1],D9[1]}, [R8],R1
 			VST3.8    {D13[1],D14[1],D15[1]}, [R6],R1 			
 			VST3.8    {D7[2],D8[2],D9[2]}, [R8],R1
 			VST3.8    {D13[2],D14[2],D15[2]}, [R6],R1 			
 			VST3.8    {D7[3],D8[3],D9[3]}, [R8],R1
 			VST3.8    {D13[3],D14[3],D15[3]}, [R6],R1 			
 			VST3.8    {D7[4],D8[4],D9[4]}, [R8],R1
 			VST3.8    {D13[4],D14[4],D15[4]}, [R6],R1 			
 			VST3.8    {D7[5],D8[5],D9[5]}, [R8],R1
 			VST3.8    {D13[5],D14[5],D15[5]}, [R6],R1 			
 			VST3.8    {D7[6],D8[6],D9[6]}, [R8],R1
 			VST3.8    {D13[6],D14[6],D15[6]}, [R6],R1 			
 			VST3.8    {D7[7],D8[7],D9[7]}, [R8],R1
 			VST3.8    {D13[7],D14[7],D15[7]}, [R6],R1
 					
NEXT_EDGE:
			
			SUBS		R11,R11,#1
			BEQ		NeonDeblockLuma_end
			ADD		R5,R5,#4
			ADD		r0,r0,#4@src
@@@@@@@@@@@@@@@@@@@@@@@@@different @end@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			LDR	   	R7,[R5]@

			ADD		r4,r4,#4@tc0
			
			
			LSR    		R2,R9,#8
			LSR		R3,R10,#8
			
 			CMP     R7, #0x0
 			BNE		BEGIN_EDGE	
 			BEQ		NEXT_EDGE			

NeonDeblockLuma_end:			
 			LDMFD           sp!, {r4 - r11,pc}  
			@endP
			@end    
