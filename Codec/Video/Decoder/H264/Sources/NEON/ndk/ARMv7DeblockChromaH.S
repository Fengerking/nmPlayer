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

				     
           .text
	  .align 2
    .globl  NeonDeblockingChromaH_ASM
    .globl  SetInPlaceIntraMBStrength_ASM
W_table:	
		.word 0x04040404           
SetInPlaceIntraMBStrength_ASM:
@void SetInPlaceIntraMBStrength_ASM(ImageParameters *img)
@{
@	avdUInt32 *str32 = (avdUInt32 *)&img->ipDeblocker->strength[0]@
@	*str32 = *(str32 + 4) = 0x04040404@
@	*(str32 + 1) = *(str32 + 2) = *(str32 + 3) = 
@	*(str32 + 5) = *(str32 + 6) = *(str32 + 7) = 0x03030303@
@}
	LDR      R2,=W_table
	LDR		 R1, [R2]
	SUB      R2,R1,R1,ASR #2   

	STR      R1,[r0,#0]
	STR      R2,[r0,#4]
	STR      R2,[r0,#8]
	STR      R1,[r0,#0x10]	
	STR      R2,[r0,#0xc] 
	STR      R2,[r0,#0x14] 
	STR      R2,[r0,#0x18]           
	STR      R2,[r0,#0x1c]
	mov	pc, lr
			@ENDP	
	

NeonDeblockingChromaH_ASM:
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
 			MOV		  R11,#2							@r11 is loop count
 			LDR       R9,  [R2]					@r9=apha+1//inner alpha
 			LDR       R5,  [SP, #40]@pt1
						
 			LDR       R4,  [SP, #36]@tc0

 			LDR       R10, [R3]					@r10=beta+1//inner beta
 			
 			LDR       R7,  [R5]@//, #0x8@r7=str[8]-str[11] 			
		
 			LDR       R12,  [R0, #4]					@r12
 			LDR       R0, [R0]					@r0 			
			AND		  R2,R9,#255			@alpha
			AND		  R3,R10,#255			@beta			
 			

										@r14:next_half flag
			CMP       	 R7, #0x0				@next edge if it is zero
 			BEQ		  NEXT_EDGE
	
BEGIN_EDGE:			
@@@@@@@@@@@@@@@@@@@@@@@@@different bigain@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 			@ldr L1,L0,R0,R1
 			VDUP.32		Q15,R7	 			
 			SUB		R8,R0,R1,LSL #1
 			SUB		R6,R12,R1,LSL #1 			
 			@d1:L1,d2:L0,d3:R0,d4:R1
			VZIP.8		D31,D30			@3 			
 			VLD1.64 {d6}, [R8],R1			@L1
 			VLD1.64 {d8}, [R8],R1			@L0
 			VLD1.64 {d10}, [R8],R1			@R0
 			VLD1.64 {d12}, [R8]			@R1
			VCGT.s8		D30,D31,#0  			
 			VLD1.64 {d7}, [R6],R1			@L1
 			VLD1.64 {d9}, [R6],R1			@L0
 			VLD1.64 {d11}, [R6],R1			@R0
 			VLD1.64 {d13}, [R6]			@R1 			
 			
@@@@@@@@@@@@@@@@@@@@@@@@@different end@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@		 			
							


			vshr.s64 D31, D30, #0
									
			VDUP.8	Q13,R2
 			@tmp1=R0-R1
			VABD.u8  	Q10,Q5,Q6												
 			VDUP.8	Q12,R3				
			@tmp2 = L0 - L1@
			VABD.u8  	Q11,Q4,Q3
			@VSUB.s8  	Q14,Q5,Q4	 @remove by Really Yang 20110503		
		
			@if((tmp1 >= Beta) || (tmp2 >= Beta)) coninue
			VCLT.u8	 	Q0,Q11,Q12@tmp2,tmp2,Beta_D
			VCLT.u8  	Q10,Q10,Q12@Flag,tmp1,Beta_D
			VAND.u8	 	Q0,Q0,Q15 
			@VABS.s8	 	Q11,Q14	 @remove by Really Yang 20110503
			VABD.u8   Q11, Q5, Q4 @add by Really Yang 20110503
			AND       R8, R7, #0xff						
			VAND.u8	 	Q0,Q0,Q10
			@Delta = R0 - L0@
			@if(abs(Delta) >= Alpha) continue
 		
			VCLT.u8	 	Q10,Q11,Q13			

 			CMP       R8, #0x4						

 			BEQ		STRTNGTH_4
		    @tmp1		  = ((Delta << 2) + (L1 - R1) + 4)>>3@
 			ldr		R6,[r4]@tc0
			VMOV.I16	  Q1,#4	
			@VSHLL.s8		  Q12,D28,#2@(Delta << 2) @remove by Really Yang 20110503
			vsubl.u8 q12, d10, d8 @add by Really Yang 20110503
			vshl.s16 q12, q12, #2	@add by Really Yang 20110503
			VDUP.32		Q15,R6			
			@VSHLL.s8		  Q13,D29,#2	@remove by Really Yang 20110503
			VSUBL.u8 Q13, D11, D9 @add by Really Yang 20110503
			VSHL.s16 Q13, Q13, #2 @add by Really Yang 20110503		 					    
			VAND.u8	 	Q0,Q0,Q10		    
 			
			VSUBL.u8	  Q10,D6,D12	@L1-R1
			VSUBL.u8	  Q11,D7,D13	@L1-R1			
			VZIP.8		D31,D30			@3								
				
			VADD.s16	  Q12,Q12,Q1@+4					
			VADD.s16	  Q13,Q13,Q1@+4
			VMOV.I8		  Q1,#1			
			vshr.s64 D30, D31, #0
			VADD.s16	  Q12,Q12,Q10@+(L1 - R1)
			VADD.s16	  Q13,Q13,Q11@+(L1 - R1)
			VADD.s8		Q1,Q1,Q15						
			VSHRN.s16	  D28,Q12,#3@>>3
			VSHRN.s16	  D29,Q13,#3@>>3		@now Q14 is tmp1	
			
			@c0 = (C0 + ap + aq) @ c0 = (tc0[j/2] + 1)@	
 			@tmp1=CLIP3(-c,tmp1,c)
 			VMIN.s8		Q14,Q1,Q14
 			VNEG.s8		Q1,Q1
 			VMAX.s8		Q14,Q1,Q14
 			VAND.u8		Q1,Q0,Q14
			
			@L0  = clip255[(L0 + tmp1)]@	
			VMOVL.s8	Q14,D2
			VMOVL.s8	Q15,D3			
			VADDW.u8 	Q14,Q14,D8	@L0_Q,L0_Q,tmp1_Q
			VADDW.u8 	Q15,Q15,D9	@L0_Q,L0_Q,tmp1_Q

			VQMOVUN.s16	D8,Q14
			VQMOVUN.s16	D9,Q15

			@R0  = clip255[(R0 - tmp1)]@
			
			VMOVL.u8	Q14,D10
			VMOVL.u8	Q15,D11
			SUB			R8,R0,R1						
			VSUBW.s8 	Q14,Q14,D2	@L0_Q,L0_Q,tmp1_Q
			VSUBW.s8 	Q15,Q15,D3	@L0_Q,L0_Q,tmp1_Q
 			SUB			R6,R12,R1 			
			VQMOVUN.s16	D10,Q14
			VQMOVUN.s16	D11,Q15

@@@@@@@@@@@@@@@@@@@@@@@@@different bigain@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			@store			
 
		
 			VST1.64    d8,		 [R8],R1
 			VST1.64    d10,		 [R8]
 			VST1.64    d9,		 [R6],R1
 			VST1.64    d11,		 [R6] 			
			
 			
@@@@@@@@@@@@@@@@@@@@@@@@@different end@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 			B	NEXT_EDGE
STRTNGTH_4:
			VMOV.I16	Q1,#2
			VADDL.u8  Q12,	D8,D12		@L0 + R1
			VADDL.u8  Q13,	D9,D13		@L0 + R1			
			VSHLL.u8	Q14,D6,#1	@L1<<1
			VSHLL.u8	Q15,D7,#1	@L1<<1				
			VAND.u8	 	Q0,Q0,Q10
@Q3 L1	@Q4 L0	@Q5 R0	@Q6 R1
@	pt2[-inc] = ((L1 << 1) + L0 + R1 + 2) >> 2@ 	
			@+L1<<1							
			@L0 + R1 + 2
			VADD.S16	Q12,Q12,Q1		@ + 2						
			VADD.S16	Q13,Q13,Q1		@ + 2
			VSHLL.u8	Q10,D12,#1	@R1<<1			
			VADD.s16	Q12,Q12,Q14
			VADD.s16	Q13,Q13,Q15
			VSHLL.u8	Q15,D13,#1	@R1<<1				
			@((L1 << 1) + L0 + R1 + 2) >> 2
			VSHRN.s16	  D28,Q12,#2@>>2
			VADDL.u8  Q12,	D10,D6		@R0 + L1			
			VSHRN.s16	  D29,Q13,#2@>>2	@Q14
@	pt2[   0] = ((R1 << 1) + R0 + L1 + 2) >> 2@
			@+R1<<1				
			VADDL.u8  Q13,	D11,D7		@R0 + L1
			@R0 + L1 + 2
			VADD.S16	Q12,Q12,Q1		@ + 2						
			VADD.S16	Q13,Q13,Q1		@ + 2			
			VADD.s16	Q12,Q12,Q10
			VADD.s16	Q13,Q13,Q15
			VCEQ.u8		Q10,Q0,#0@tmp6_D,Flag,#0			
			@((R1 << 1) + R0 + L1 + 2) >> 2
			VSHRN.s16	  D30,Q12,#2@>>2
			VAND.s8		Q12,Q14,Q0			
			VSHRN.s16	  D31,Q13,#2@>>2	@Q15

			VAND.s8		Q13,Q4,Q10
 			SUB			R8,R0,R1			
			VORR.s8		Q4,Q12,Q13@	now Q4 is last L0
			
			VAND.s8		Q12,Q15,Q0
		SUB			R6,R12,R1 			
			VAND.s8		Q13,Q5,Q10
			VORR.s8		Q5,Q12,Q13@	now Q5 is last R0									
@@@@@@@@@@@@@@@@@@@@@@@@@different bigain@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

 				
 			VST1.64    d8,		 [R8],R1
 			VST1.64    d10,		 [R8]
 			VST1.64    d9,		 [R6],R1
 			VST1.64    d11,		 [R6] 			

NEXT_EDGE:
			SUBS	R11,R11,#1
			BEQ		NeonDeblockChromaH_END 	
			ADD		R5,R5,#8
			ADD		r0,r0,R1,LSL #2@src
			ADD		r12,r12,R1,LSL #2@src			
@@@@@@@@@@@@@@@@@@@@@@@@@different end@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			LDR	   	R7,[R5]@

			ADD		r4,r4,#4@tc0
			
			
			LSR    		R2,R9,#8
			LSR		R3,R10,#8
			
 			CMP     R7, #0x0
 			BNE		BEGIN_EDGE	
					
NeonDeblockChromaH_END:			
 			LDMFD          sp!, {r4 - r11,pc}  
			@ENDP
			@END    
