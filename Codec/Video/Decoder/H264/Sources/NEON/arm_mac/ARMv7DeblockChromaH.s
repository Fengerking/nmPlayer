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
    .globl  _NeonDeblockingChromaH_ASM
    .globl  _SetInPlaceIntraMBStrength_ASM
          
_SetInPlaceIntraMBStrength_ASM:
@void SetInPlaceIntraMBStrength_ASM(ImageParameters *img)
@{
@	avdUInt32 *str32 = (avdUInt32 *)&img->ipDeblocker->strength[0]@
@	*str32 = *(str32 + 4) = 0x04040404@
@	*(str32 + 1) = *(str32 + 2) = *(str32 + 3) = 
@	*(str32 + 5) = *(str32 + 6) = *(str32 + 7) = 0x03030303@
@}
	ldr      r2, W_table
	ldr		   r1, [r2]
	sub      r2,r1,r1,asr #2   

	str      r1,[r0,#0]
	str      r2,[r0,#4]
	str      r2,[r0,#8]
	str      r1,[r0,#0x10]	
	str      r2,[r0,#0xc] 
	str      r2,[r0,#0x14] 
	str      r2,[r0,#0x18]           
	str      r2,[r0,#0x1c]
	mov	pc, lr
			@ENDP	
	

_NeonDeblockingChromaH_ASM:
			stmfd     sp!, {r4-r11,lr}@36 byte
 			mov		    r11,#2							@r11 is loop count
 			ldr       r9,  [r2]					@r9=apha+1//inner alpha
 			ldr       r5,  [SP, #40]@pt1
						
 			ldr       R4,  [SP, #36]@tc0

 			ldr       R10, [R3]					@r10=beta+1//inner beta
 			
 			ldr       R7,  [r5]@//, #0x8@r7=str[8]-str[11] 			
		
 			ldr       R12,  [R0, #4]					@r12
 			ldr       R0, [R0]					@r0 			
			and		  r2,r9,#255			@alpha
			and		  R3,R10,#255			@beta			
 			
			cmp       	 R7, #0x0				@next edge if it is zero
 			beq		  NEXT_EDGE
	
BEGIN_EDGE:			
@@@@@@@@@@@@@@@@@@@@@@@@@different bigain@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 			@ldr L1,L0,R0,r1
 			vdup.32		Q15,R7	 			
 			sub		R8,R0,r1,LSL #1
 			sub		R6,R12,r1,LSL #1 			
 			@d1:L1,d2:L0,d3:R0,d4:r1
			vzip.8		D31,D30			@3 			
 			vld1.64 {d6}, [R8],r1			@L1
 			vld1.64 {d8}, [R8],r1			@L0
 			vld1.64 {d10}, [R8],r1			@R0
 			vld1.64 {d12}, [R8]			@r1
			vcgt.s8		D30,D31,#0  			
 			vld1.64 {d7}, [R6],r1			@L1
 			vld1.64 {d9}, [R6],r1			@L0
 			vld1.64 {d11}, [R6],r1			@R0
 			vld1.64 {d13}, [R6]			@r1 			
 			
@@@@@@@@@@@@@@@@@@@@@@@@@different end@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@		 			
							
			vshr.s64 D31, D30, #0
									
			vdup.8	Q13,r2
 			@tmp1=R0-r1
			vabd.u8  	Q10,Q5,Q6												
 			vdup.8	Q12,R3				
			@tmp2 = L0 - L1@
			vabd.u8  	Q11,Q4,Q3
			@vsub.s8  	Q14,Q5,Q4		@remove by Really Yang 20110503	
		
			@if((tmp1 >= Beta) || (tmp2 >= Beta)) coninue
			vclt.u8	 	Q0,Q11,Q12@tmp2,tmp2,Beta_D
			vclt.u8  	Q10,Q10,Q12@Flag,tmp1,Beta_D
			vand.u8	 	Q0,Q0,Q15 
			@vabs.s8	 	Q11,Q14 @remove by Really Yang 20110503
			vabd.u8   Q11, Q5, Q4 @add by Really Yang 20110503	
			and       R8, R7, #0xff						
			vand.u8	 	Q0,Q0,Q10
 		
			vclt.u8	 	Q10,Q11,Q13			
 			cmp       R8, #0x4						

 			beq		STRTNGTH_4
		    @tmp1		  = ((Delta << 2) + (L1 - r1) + 4)>>3@
 			ldr		R6,[r4]@tc0
			vmov.I16	  Q1,#4	
			@vshll.s8		  Q12,D28,#2@(Delta << 2)  @remove by Really Yang 20110503
			vsubl.u8 q12, d10, d8 @add by Really Yang 20110503
			vshl.s16 q12, q12, #2	@add by Really Yang 20110503	
			vdup.32		Q15,R6			
			@vshll.s8		  Q13,D29,#2  @remove by Really Yang 20110503
			vsubl.u8 Q13, D11, D9 @add by Really Yang 20110503
			vshl.s16 Q13, Q13, #2 @add by Really Yang 20110503			 					    
			vand.u8	 	Q0,Q0,Q10		    
 			
			vsubl.u8	  Q10,D6,D12	@L1-r1
			vsubl.u8	  Q11,D7,D13	@L1-r1			
			vzip.8		D31,D30			@3								
				
			vadd.s16	  Q12,Q12,Q1@+4					
			vadd.s16	  Q13,Q13,Q1@+4
			vmov.I8		  Q1,#1			
			vshr.s64 D30, D31, #0
			vadd.s16	  Q12,Q12,Q10@+(L1 - r1)
			vadd.s16	  Q13,Q13,Q11@+(L1 - r1)
			vadd.s8		Q1,Q1,Q15						
			vshrn.s16	  D28,Q12,#3@>>3
			vshrn.s16	  D29,Q13,#3@>>3		@now Q14 is tmp1	
			
			@c0 = (C0 + ap + aq) @ c0 = (tc0[j/2] + 1)@	
 			@tmp1=CLIP3(-c,tmp1,c)
 			vmin.s8		Q14,Q1,Q14
 			vneg.s8		Q1,Q1
 			vmax.s8		Q14,Q1,Q14
 			vand.u8		Q1,Q0,Q14
			
			@L0  = clip255[(L0 + tmp1)]@	
			vmovl.s8	Q14,D2
			vmovl.s8	Q15,D3			
			vaddw.u8 	Q14,Q14,D8	@L0_Q,L0_Q,tmp1_Q
			vaddw.u8 	Q15,Q15,D9	@L0_Q,L0_Q,tmp1_Q

			vqmovun.s16	D8,Q14
			vqmovun.s16	D9,Q15

			@R0  = clip255[(R0 - tmp1)]@
			
			vmovl.u8	Q14,D10
			vmovl.u8	Q15,D11
			sub			R8,R0,r1						
			vsubw.s8 	Q14,Q14,D2	@L0_Q,L0_Q,tmp1_Q
			vsubw.s8 	Q15,Q15,D3	@L0_Q,L0_Q,tmp1_Q
 			sub			R6,R12,r1 			
			vqmovun.s16	D10,Q14
			vqmovun.s16	D11,Q15

@@@@@@@@@@@@@@@@@@@@@@@@@different bigain@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			@store			
 
		
 			vst1.64    d8,		 [R8],r1
 			vst1.64    d10,		 [R8]
 			vst1.64    d9,		 [R6],r1
 			vst1.64    d11,		 [R6] 			
			
 			
@@@@@@@@@@@@@@@@@@@@@@@@@different end@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 			b	NEXT_EDGE
STRTNGTH_4:
			vmov.I16	Q1,#2
			vaddl.u8  Q12,	D8,D12		@L0 + r1
			vaddl.u8  Q13,	D9,D13		@L0 + r1			
			vshll.u8	Q14,D6,#1	@L1<<1
			vshll.u8	Q15,D7,#1	@L1<<1				
			vand.u8	 	Q0,Q0,Q10
@Q3 L1	@Q4 L0	@Q5 R0	@Q6 r1
@	pt2[-inc] = ((L1 << 1) + L0 + r1 + 2) >> 2@ 	
			@+L1<<1							
			@L0 + r1 + 2
			vadd.S16	Q12,Q12,Q1		@ + 2						
			vadd.S16	Q13,Q13,Q1		@ + 2
			vshll.u8	Q10,D12,#1	@r1<<1			
			vadd.s16	Q12,Q12,Q14
			vadd.s16	Q13,Q13,Q15
			vshll.u8	Q15,D13,#1	@r1<<1				
			@((L1 << 1) + L0 + r1 + 2) >> 2
			vshrn.s16	  D28,Q12,#2@>>2
			vaddl.u8  Q12,	D10,D6		@R0 + L1			
			vshrn.s16	  D29,Q13,#2@>>2	@Q14
@	pt2[   0] = ((r1 << 1) + R0 + L1 + 2) >> 2@
			@+r1<<1				
			vaddl.u8  Q13,	D11,D7		@R0 + L1
			@R0 + L1 + 2
			vadd.S16	Q12,Q12,Q1		@ + 2						
			vadd.S16	Q13,Q13,Q1		@ + 2			
			vadd.s16	Q12,Q12,Q10
			vadd.s16	Q13,Q13,Q15
			vceq.u8		Q10,Q0,#0@tmp6_D,Flag,#0			
			@((r1 << 1) + R0 + L1 + 2) >> 2
			vshrn.s16	  D30,Q12,#2@>>2
			vand.s8		Q12,Q14,Q0			
			vshrn.s16	  D31,Q13,#2@>>2	@Q15

			vand.s8		Q13,Q4,Q10
 			sub			R8,R0,r1			
			vorr.s8		Q4,Q12,Q13@	now Q4 is last L0
			
			vand.s8		Q12,Q15,Q0
		sub			R6,R12,r1 			
			vand.s8		Q13,Q5,Q10
			vorr.s8		Q5,Q12,Q13@	now Q5 is last R0									
@@@@@@@@@@@@@@@@@@@@@@@@@different bigain@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

 				
 			vst1.64    d8,		 [R8],r1
 			vst1.64    d10,		 [R8]
 			vst1.64    d9,		 [R6],r1
 			vst1.64    d11,		 [R6] 			

NEXT_EDGE:
			subs	r11,r11,#1
			beq		NeonDeblockChromaH_END 	
			add		r5,r5,#8
			add		r0,r0,r1,LSL #2@src
			add		r12,r12,r1,LSL #2@src			
@@@@@@@@@@@@@@@@@@@@@@@@@different end@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			ldr	   	R7,[r5]@

			add		r4,r4,#4@tc0
			
			
			lsr    		r2,r9,#8
			lsr		R3,R10,#8
			
 			cmp     R7, #0x0
 			bne		BEGIN_EDGE	
					
NeonDeblockChromaH_END:			
 			ldmfd          sp!, {r4 - r11,pc}  
			@ENDP
W_table:		.word 0x04040404    
