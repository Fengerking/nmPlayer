@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2009		            *
@	written by John							 	                                    *
@***********************************************************************/
	@AREA	 .text , CODE, READONLY
#include "../../../../Inc/voVP6DecID.h"
	.section .text

	.global	 VP6DEC_VO_Armv7IdctA
	.global	 VP6DEC_VO_Armv7IdctB	
	.global	 VP6DEC_VO_Armv7IdctC	
	.align 4	
 .macro	LOAD_IDCT_SRC
	 vld1.64 {d0}, [r3], r12 
	 vld1.64 {d1}, [r3], r12 
	 vld1.64 {d2}, [r3], r12 
	 vld1.64 {d3}, [r3], r12 
	 vld1.64 {d4}, [r3], r12 
	 vld1.64 {d5}, [r3], r12 
	 vld1.64 {d6}, [r3], r12 
	 vld1.64 {d7}, [r3]
 .endm 	
 .macro LOAD_IDCT4x8_COEF		
	mov	r12, #16
 	vmov.s32 d24, #0	
	vld1.64	{d8}, [r0], r12		@q4 x0 x1 d8-d9
	vld1.64	{d10}, [r0], r12	@q5 x4 x5 d10-d11
	vld1.64	{d12}, [r0], r12	@q6 x3 x2 d12-d13
	vld1.64	{d14}, [r0]		@q7 x7 x6 d14-d15
	sub	r0, r0, #48	
	vst1.64	{d24}, [r0], r12		@
	vst1.64	{d24}, [r0], r12	@
	vst1.64	{d24}, [r0], r12	@
	vst1.64	{d24}, [r0]		@		
 .endm
 .macro LOAD_IDCT8x8_COEF
 	vmov.s32 q12, #0		
	vld1.64	{q4}, [r0]!		@q4 x0 x4 d8-d9	
	vld1.64	{q5}, [r0]!		@q5 x1 x7 d10-d11	
	vld1.64	{q6}, [r0]!		@q6 x2 x6 d12-d13	
	vld1.64	{q7}, [r0]!		@q7 x3 x5 d14-d15	
	vld1.64	{q8}, [r0]!		@	
	vld1.64	{q9}, [r0]!		@	
	vld1.64	{q10}, [r0]!		@	
	vld1.64	{q11}, [r0]		@
	
	sub	r0, r0, #112
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]!		@	
	vst1.64	{q12}, [r0]		@		
 .endm 	 
 .macro LOAD_IDCT_SRC_ADD		
	ldr		r12, [sp]		@ r12 = src_stride
	LOAD_IDCT_SRC			
	vaddw.u8	q8, q8, d0
	vaddw.u8	q9, q9, d1
	vaddw.u8	q10, q10, d2
	vaddw.u8	q11, q11, d3
	vaddw.u8	q4, q4, d4
	vaddw.u8	q5, q5, d5
	vaddw.u8	q6, q6, d6
	vaddw.u8	q7, q7, d7	
 .endm
 .macro	STR_IDCT
	vqmovun.s16	d16, q8
	vqmovun.s16	d18, q9
	vqmovun.s16	d20, q10
	vqmovun.s16	d22, q11
	vqmovun.s16	d8, q4
	vqmovun.s16	d10, q5
	vqmovun.s16	d12, q6
	vqmovun.s16	d14, q7		
						
	vst1.64	{d16}, [r1], r2
	vst1.64	{d18}, [r1], r2		
	vst1.64	{d20}, [r1], r2
	vst1.64	{d22}, [r1], r2	
	vst1.64	{d8}, [r1], r2
	vst1.64	{d10}, [r1], r2	
	vst1.64	{d12}, [r1], r2
	vst1.64	{d14}, [r1]			
 .endm	

 
@q4 x0 x4 d8-d9  
@q5 x1 x7 d10-d11						
@q6 x2 x6 d12-d13						
@q7 x3 x5 d14-d15 
  .macro Col8_first4line_4x8  is_row
@            A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])@
@            B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@
@            C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
@            D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
@            E = M(xC4S4, (ip[0*8] + ip[4*8]))@
@            F = M(xC4S4, (ip[0*8] - ip[4*8]))@
@            G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
@            H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@

@	vaddl.s16	q12, d8, d9		@ip0 + ip4 
@	vsubl.s16	q13, d8, d9		@ip0 - ip4
	vmull.s16	q12, d8, D1[2]		@E = xC4*(ip0 + ip4)@
@	vmul.s32	q13, q13, D1[1]		@F = xC4*(ip0 - ip4)@		

	vmull.s16	q15, d10, D0[0]		@B = xC7*ip1@
	.if \is_row>0	
@	E += (16*128 + 8)<<16@	or E += 8<<16@
@	F += (16*128 + 8)<<16@	or F += 8<<16@
	vadd.s32	q12, q12, q14 
@	vadd.s32	q13, q13, q14
	.endif			
			
@	vmull.s16	q14, d11, D0[0]		@xC7*ip7
@	vmlal.s16	q14, d10, D0[1]		@A = xC7*ip7 + xC1*ip1@
	vmull.s16	q14, d10, D0[1]		@A = xC1*ip1@

@	vmull.s16	q15, d10, D0[0]		@xC7*ip1
@	vmlsl.s16	q15, d11, D0[1]		@B = xC7*ip1 - xC1*ip7@

	
	
@	vmull.s16	q4, d14, D0[2]		@xC3*ip3
@	vmlal.s16	q4, d15, D0[3]		@C = xC3*ip3 + xC5*ip5@
@	vmull.s16	q5, d15, D0[2]		@xC3*ip5
@	vmlsl.s16	q5, d14, D0[3]		@D = xC3*ip5 - xC5*ip3@
	vmull.s16	q4, d14, D0[2]		@C = xC3*ip3@	
	vmull.s16	q5, d14, D0[3]		@D = - xC5*ip3@
	 	
@	vmull.s16	q7, d13, D1[0]		@xC6*ip6
@	vmlal.s16	q7, d12, D1[1]		@G = xC6*ip6 + xC2*ip2@
@	vmull.s16	q3, d12, D1[0]		@xC6*ip2
@	vmlsl.s16	q3, d13, D1[1]		@H = xC6*ip2 - xC2*ip6@
	vmull.s16	q7, d12, D1[1]		@G = xC2*ip2@
	vmull.s16	q3, d12, D1[0]		@H = xC6*ip2@
		
@            Ad = M(xC4S4, (A - C)>>16)@
@            Cd = A + C@
@            Bd = M(xC4S4, (B - D)>>16)@
@            Dd = B + D@
	vsub.s32	q6, q14, q4		@ A - C 
	vadd.s32	q4, q14, q4		@Cd = A + C
	vadd.s32	q14, q15, q5		@ B - D 
	vsub.s32	q15, q15, q5		@Dd = B + D
	vshr.s32	q6, q6, #15
	vshr.s32	q14, q14, #15			
	vmul.s32	q6, q6, D1[1]		@Ad = xC4*((A - C)>>16)@
	vmul.s32	q14, q14, D1[1]		@Bd = xC4*((B - D)>>16)@		
@            Ed = E - G@
@            Fd = E + G@
@            Gd = F - Ad@
@            Add = F + Ad@
@            Bdd = Bd - H@
@            Hd = Bd + H@
	vmov	q13, q12
	vsub.s32	q5, q12, q7		@Ed = E - G@ 
	vadd.s32	q12, q12, q7		@Fd = E + G@ 
	vsub.s32	q7, q13, q6		@Gd = F - Ad@ 
	vadd.s32	q13, q13, q6		@Add = F + Ad@ 
	vsub.s32	q6, q14, q3		@Bdd = Bd - H@ 
	vadd.s32	q14, q14, q3		@Hd = Bd + H@ 	 		
				
@            ip[0*8] = (Fd + Cd)>>16 @
@            ip[7*8] = (Fd - Cd)>>16 @
@            ip[1*8] = (Add + Hd)>>16@
@            ip[2*8] = (Add - Hd)>>16@
@            ip[3*8] = (Ed + Dd)>>16 @
@            ip[4*8] = (Ed - Dd)>>16 @
@            ip[5*8] = (Gd + Bdd)>>16@
@            ip[6*8] = (Gd - Bdd)>>16@

	vadd.s32	q3, q12, q4		@ip[0*8] = (Fd + Cd)@
	vsub.s32	q4, q12, q4		@ip[7*8] = (Fd - Cd)@
	
	vadd.s32	q12, q13, q14		@ip[1*8] = (Add + Hd)@
	vsub.s32	q14, q13, q14		@ip[2*8] = (Add - Hd)@
	
	vadd.s32	q13, q5, q15		@ip[3*8] = (Ed + Dd)@
	vsub.s32	q15, q5, q15		@ip[4*8] = (Ed - Dd)@
	
	vadd.s32	q5, q7, q6		@ip[5*8] = (Gd + Bdd)@
	vsub.s32	q6, q7, q6		@ip[6*8] = (Gd - Bdd)@		
 .endm

@q4 x0 x4 d8-d9  
@q5 x1 x7 d10-d11						
@q6 x2 x6 d12-d13						
@q7 x3 x5 d14-d15 
  .macro Col8_first4line  is_row
@            A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])@
@            B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@
@            C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
@            D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
@            E = M(xC4S4, (ip[0*8] + ip[4*8]))@
@            F = M(xC4S4, (ip[0*8] - ip[4*8]))@
@            G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
@            H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@
	vaddl.s16	q12, d8, d9		@ip0 + ip4 
	vsubl.s16	q13, d8, d9		@ip0 - ip4
	vmul.s32	q12, q12, D1[1]		@E = xC4*(ip0 + ip4)@
	vmul.s32	q13, q13, D1[1]		@F = xC4*(ip0 - ip4)@	

	vmull.s16	q15, d10, D0[0]		@xC7*ip1
	.if \is_row>0	
@	E += (16*128 + 8)<<16@	or E += 8<<16@
@	F += (16*128 + 8)<<16@	or F += 8<<16@
	vadd.s32	q12, q12, q14 
	vadd.s32	q13, q13, q14
	.endif		
			
	vmull.s16	q14, d11, D0[0]		@xC7*ip7


	vmlal.s16	q14, d10, D0[1]		@A = xC7*ip7 + xC1*ip1@
	vmlsl.s16	q15, d11, D0[1]		@B = xC7*ip1 - xC1*ip7@
	vmull.s16	q4, d14, D0[2]		@xC3*ip3	
	
	vmull.s16	q5, d15, D0[2]		@xC3*ip5
	vmlal.s16	q4, d15, D0[3]		@C = xC3*ip3 + xC5*ip5@
	vmlsl.s16	q5, d14, D0[3]		@D = xC3*ip5 - xC5*ip3@	
 	
	vmull.s16	q7, d13, D1[0]		@xC6*ip6
	vmull.s16	q3, d12, D1[0]		@xC6*ip2	
	vmlal.s16	q7, d12, D1[1]		@G = xC6*ip6 + xC2*ip2@
	vmlsl.s16	q3, d13, D1[1]		@H = xC6*ip2 - xC2*ip6@
		
@            Ad = M(xC4S4, (A - C)>>16)@
@            Cd = A + C@
@            Bd = M(xC4S4, (B - D)>>16)@
@            Dd = B + D@
	vsub.s32	q6, q14, q4		@ A - C 
	vadd.s32	q4, q14, q4		@Cd = A + C
	vsub.s32	q14, q15, q5		@ B - D 
	vadd.s32	q15, q15, q5		@Dd = B + D
	vshr.s32	q6, q6, #15
	vshr.s32	q14, q14, #15			
	vmul.s32	q6, q6, D1[1]		@Ad = xC4*((A - C)>>16)@
	vmul.s32	q14, q14, D1[1]		@Bd = xC4*((B - D)>>16)@		
@            Ed = E - G@
@            Fd = E + G@
@            Gd = F - Ad@
@            Add = F + Ad@
@            Bdd = Bd - H@
@            Hd = Bd + H@
	vsub.s32	q5, q12, q7		@Ed = E - G@ 
	vadd.s32	q12, q12, q7		@Fd = E + G@ 
	vsub.s32	q7, q13, q6		@Gd = F - Ad@ 
	vadd.s32	q13, q13, q6		@Add = F + Ad@ 
	vsub.s32	q6, q14, q3		@Bdd = Bd - H@ 
	vadd.s32	q14, q14, q3		@Hd = Bd + H@ 	 		
				
@            ip[0*8] = (Fd + Cd)>>16 @
@            ip[7*8] = (Fd - Cd)>>16 @
@            ip[1*8] = (Add + Hd)>>16@
@            ip[2*8] = (Add - Hd)>>16@
@            ip[3*8] = (Ed + Dd)>>16 @
@            ip[4*8] = (Ed - Dd)>>16 @
@            ip[5*8] = (Gd + Bdd)>>16@
@            ip[6*8] = (Gd - Bdd)>>16@

	vadd.s32	q3, q12, q4		@ip[0*8] = (Fd + Cd)@
	vsub.s32	q4, q12, q4		@ip[7*8] = (Fd - Cd)@
	
	vadd.s32	q12, q13, q14		@ip[1*8] = (Add + Hd)@
	vsub.s32	q14, q13, q14		@ip[2*8] = (Add - Hd)@
	
	vadd.s32	q13, q5, q15		@ip[3*8] = (Ed + Dd)@
	vsub.s32	q15, q5, q15		@ip[4*8] = (Ed - Dd)@
	
	vadd.s32	q5, q7, q6		@ip[5*8] = (Gd + Bdd)@
	vsub.s32	q6, q7, q6		@ip[6*8] = (Gd - Bdd)@		
 .endm

@q8  x0 x4 d16-17
@q11 x1 x7 d22-23  
@q10 x2 x6 d20-21	
@q9  x3 x5 d18-19
  .macro Col8_last4line_4x8  is_row
@            A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])@
@            B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@
@            C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
@            D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
@            E = M(xC4S4, (ip[0*8] + ip[4*8]))@
@            F = M(xC4S4, (ip[0*8] - ip[4*8]))@
@            G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
@            H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@
@	vaddl.s16	q12, d16, d17		@ip0 + ip4 
@	vsubl.s16	q13, d16, d17		@ip0 - ip4
	vmull.s16	q12, d16, D1[2]		@E = xC4*(ip0 + ip4)@
@	vmul.s32	q13, q13, D1[1]		@F = xC4*(ip0 - ip4)@

	vmull.s16	q15, d22, D0[0]		@B = xC7*ip1@	
	.if \is_row>0	
@	E += (16*128 + 8)<<16@	or E += 8<<16@
@	F += (16*128 + 8)<<16@	or F += 8<<16@
	vadd.s32	q12, q12, q14 
@	vadd.s32	q13, q13, q14
	.endif
			
@	vmull.s16	q14, d23, D0[0]		@xC7*ip7
@	vmlal.s16	q14, d22, D0[1]		@A = xC7*ip7 + xC1*ip1@
	vmull.s16	q14, d22, D0[1]		@A = xC1*ip1@
@	vmull.s16	q15, d22, D0[0]		@xC7*ip1
@	vmlsl.s16	q15, d23, D0[1]		@B = xC7*ip1 - xC1*ip7@
	

	
@	vmull.s16	q8, d18, D0[2]		@xC3*ip3
@	vmlal.s16	q8, d19, D0[3]		@C = xC3*ip3 + xC5*ip5@
@	vmull.s16	q11, d19, D0[2]		@xC3*ip5
@	vmlsl.s16	q11, d18, D0[3]		@D = xC3*ip5 - xC5*ip3@	
	vmull.s16	q8, d18, D0[2]		@C = xC3*ip3
	vmull.s16	q11, d18, D0[3]		@D = - xC5*ip3@
 	
@	vmull.s16	q9, d21, D1[0]		@xC6*ip6
@	vmlal.s16	q9, d20, D1[1]		@G = xC6*ip6 + xC2*ip2@
@	vmull.s16	q3, d20, D1[0]		@xC2*ip5
@	vmlsl.s16	q3, d21, D1[1]		@H = xC6*ip2 - xC2*ip6@
	vmull.s16	q9, d20, D1[1]		@G = xC2*ip2
	vmull.s16	q3, d20, D1[0]		@H = xC6*ip2@
		
@            Ad = M(xC4S4, (A - C)>>16)@
@            Cd = A + C@
@            Bd = M(xC4S4, (B - D)>>16)@
@            Dd = B + D@
	vsub.s32	q10, q14, q8		@ A - C 
	vadd.s32	q8, q14, q8		@Cd = A + C
	vadd.s32	q14, q15, q11		@ B - D 
	vsub.s32	q15, q15, q11		@Dd = B + D
	vshr.s32	q10, q10, #15
	vshr.s32	q14, q14, #15			
	vmul.s32	q10, q10, D1[1]		@Ad = xC4*((A - C)>>16)@
	vmul.s32	q14, q14, D1[1]		@Bd = xC4*((B - D)>>16)@		
@            Ed = E - G@
@            Fd = E + G@
@            Gd = F - Ad@
@            Add = F + Ad@
@            Bdd = Bd - H@
@            Hd = Bd + H@
	vmov	q13, q12
	vsub.s32	q11, q12, q9		@Ed = E - G@ 
	vadd.s32	q12, q12, q9		@Fd = E + G@ 
	vsub.s32	q9, q13, q10		@Gd = F - Ad@ 
	vadd.s32	q13, q13, q10		@Add = F + Ad@ 
	vsub.s32	q10, q14, q3		@Bdd = Bd - H@ 
	vadd.s32	q14, q14, q3		@Hd = Bd + H@ 	 		
				
@            ip[0*8] = (Fd + Cd)>>16 @
@            ip[7*8] = (Fd - Cd)>>16 @
@            ip[1*8] = (Add + Hd)>>16@
@            ip[2*8] = (Add - Hd)>>16@
@            ip[3*8] = (Ed + Dd)>>16 @
@            ip[4*8] = (Ed - Dd)>>16 @
@            ip[5*8] = (Gd + Bdd)>>16@
@            ip[6*8] = (Gd - Bdd)>>16@

	vadd.s32	q3, q12, q8		@ip[0*8] = (Fd + Cd)@
	vsub.s32	q8, q12, q8		@ip[7*8] = (Fd - Cd)@
	
	vadd.s32	q12, q13, q14		@ip[1*8] = (Add + Hd)@
	vsub.s32	q14, q13, q14		@ip[2*8] = (Add - Hd)@
	
	vadd.s32	q13, q11, q15		@ip[3*8] = (Ed + Dd)@
	vsub.s32	q15, q11, q15		@ip[4*8] = (Ed - Dd)@
	
	vadd.s32	q11, q9, q10		@ip[5*8] = (Gd + Bdd)@
	vsub.s32	q10, q9, q10		@ip[6*8] = (Gd - Bdd)@
			
 .endm

@q8  x0 x4 d16-17
@q11 x1 x7 d22-23  
@q10 x2 x6 d20-21	
@q9  x3 x5 d18-19
  .macro Col8_last4line  is_row
@            A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8])@
@            B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8])@
@            C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])@
@            D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8])@
@            E = M(xC4S4, (ip[0*8] + ip[4*8]))@
@            F = M(xC4S4, (ip[0*8] - ip[4*8]))@
@            G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8])@
@            H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8])@
	vaddl.s16	q12, d16, d17		@ip0 + ip4 
	vsubl.s16	q13, d16, d17		@ip0 - ip4
	vmul.s32	q12, q12, D1[1]		@E = xC4*(ip0 + ip4)@
	vmul.s32	q13, q13, D1[1]		@F = xC4*(ip0 - ip4)@
	
	vmull.s16	q15, d22, D0[0]		@xC7*ip1	
	.if \is_row>0	
@	E += (16*128 + 8)<<16@	or E += 8<<16@
@	F += (16*128 + 8)<<16@	or F += 8<<16@
	vadd.s32	q12, q12, q14 
	vadd.s32	q13, q13, q14
	.endif
			
	vmull.s16	q14, d23, D0[0]		@xC7*ip7
	
	vmlal.s16	q14, d22, D0[1]		@A = xC7*ip7 + xC1*ip1@
	vmlsl.s16	q15, d23, D0[1]		@B = xC7*ip1 - xC1*ip7@
	
	vmull.s16	q8, d18, D0[2]		@xC3*ip3
	vmull.s16	q11, d19, D0[2]		@xC3*ip5	
	vmlal.s16	q8, d19, D0[3]		@C = xC3*ip3 + xC5*ip5@
	vmlsl.s16	q11, d18, D0[3]		@D = xC3*ip5 - xC5*ip3@	
 	
	vmull.s16	q9, d21, D1[0]		@xC6*ip6
	vmull.s16	q3, d20, D1[0]		@xC2*ip5	
	vmlal.s16	q9, d20, D1[1]		@G = xC6*ip6 + xC2*ip2@
	vmlsl.s16	q3, d21, D1[1]		@H = xC6*ip2 - xC2*ip6@
		
@            Ad = M(xC4S4, (A - C)>>16)@
@            Cd = A + C@
@            Bd = M(xC4S4, (B - D)>>16)@
@            Dd = B + D@
	vsub.s32	q10, q14, q8		@ A - C 
	vadd.s32	q8, q14, q8		@Cd = A + C
	vsub.s32	q14, q15, q11		@ B - D 
	vadd.s32	q15, q15, q11		@Dd = B + D
	vshr.s32	q10, q10, #15
	vshr.s32	q14, q14, #15			
	vmul.s32	q10, q10, D1[1]		@Ad = xC4*((A - C)>>16)@
	vmul.s32	q14, q14, D1[1]		@Bd = xC4*((B - D)>>16)@		
@            Ed = E - G@
@            Fd = E + G@
@            Gd = F - Ad@
@            Add = F + Ad@
@            Bdd = Bd - H@
@            Hd = Bd + H@
	vsub.s32	q11, q12, q9		@Ed = E - G@ 
	vadd.s32	q12, q12, q9		@Fd = E + G@ 
	vsub.s32	q9, q13, q10		@Gd = F - Ad@ 
	vadd.s32	q13, q13, q10		@Add = F + Ad@ 
	vsub.s32	q10, q14, q3		@Bdd = Bd - H@ 
	vadd.s32	q14, q14, q3		@Hd = Bd + H@ 	 		
				
@            ip[0*8] = (Fd + Cd)>>16 @
@            ip[7*8] = (Fd - Cd)>>16 @
@            ip[1*8] = (Add + Hd)>>16@
@            ip[2*8] = (Add - Hd)>>16@
@            ip[3*8] = (Ed + Dd)>>16 @
@            ip[4*8] = (Ed - Dd)>>16 @
@            ip[5*8] = (Gd + Bdd)>>16@
@            ip[6*8] = (Gd - Bdd)>>16@

	vadd.s32	q3, q12, q8		@ip[0*8] = (Fd + Cd)@
	vsub.s32	q8, q12, q8		@ip[7*8] = (Fd - Cd)@
	
	vadd.s32	q12, q13, q14		@ip[1*8] = (Add + Hd)@
	vsub.s32	q14, q13, q14		@ip[2*8] = (Add - Hd)@
	
	vadd.s32	q13, q11, q15		@ip[3*8] = (Ed + Dd)@
	vsub.s32	q15, q11, q15		@ip[4*8] = (Ed - Dd)@
	
	vadd.s32	q11, q9, q10		@ip[5*8] = (Gd + Bdd)@
	vsub.s32	q10, q9, q10		@ip[6*8] = (Gd - Bdd)@
			
 .endm
  .macro Col8_4x8
 
	LOAD_IDCT4x8_COEF
	
	cmp	r3, #0
	@movne	r12, #8
	@moveq	r12, #0x0808
	mov	r12, #8
	orreq	r12, r12, r12, lsl #8		  
	lsl	r12, r12, #15	
							@q4 x0 x4 d8-d9  
							@q5 x1 x7 d10-d11						
							@q6 x2 x6 d12-d13						
							@q7 x3 x5 d14-d15		
	
	
	Col8_first4line_4x8 0
		
	vshrn.s32	d16, q3, #15
	vshrn.s32	d18, q12, #15	
	vshrn.s32	d20, q14, #15
	vshrn.s32	d22, q13, #15
	
	vshrn.s32	d17, q15, #15
	vshrn.s32	d19, q5, #15	
	vshrn.s32	d21, q6, #15	
	vshrn.s32	d23, q4, #15	
			
@	Transpose				
	VTRN.16 q8, q9
	VTRN.16 q10, q11
	VTRN.32 q8, q10
	VTRN.32 q9, q11
	
	vmov.64 d8, d17
	vmov.64 d10, d19
	vmov.64 d12, d21
	vmov.64 d14, d23			
	vswp.s64	d18, d22
@row
	vdup.s32	q14, r12		
	Col8_last4line_4x8 1

	vshrn.s32	d2, q15, #15
	vshrn.s32	d3, q11, #15
	vshrn.s32	d4, q10, #15
	vshrn.s32	d5, q8, #15
			
	vshrn.s32	d16, q3, #15
	vshrn.s32	d18, q12, #15	
	vshrn.s32	d20, q14, #15
	vshrn.s32	d22, q13, #15	

	vdup.s32	q14, r12
	
	Col8_first4line_4x8 1
	
	vshrn.s32	d17, q3, #15
	vshrn.s32	d19, q12, #15	
	vshrn.s32	d21, q14, #15
	vshrn.s32	d23, q13, #15
	
	vshrn.s32	d15, q4, #15	
	vshrn.s32	d9, q15, #15
	vshrn.s32	d13, q6, #15
	vshrn.s32	d11, q5, #15		

	vswp.s32	d2, d8
	vswp.s32	d3, d10
	vswp.s32	d4, d12	
	vswp.s32	d5, d14				
@	Transpose	
	VTRN.16 q8, q9
	VTRN.16 q10, q11
	VTRN.16 q4, q5
	VTRN.16 q6, q7
	VTRN.32 q8, q10
	VTRN.32 q9, q11
	VTRN.32 q4, q6
	VTRN.32 q5, q7
	VSWP d17, d8
	VSWP d19, d10
	VSWP d21, d12
	VSWP d23, d14	
	
	vshr.s16	q8, q8, #4
	vshr.s16	q9, q9, #4
	vshr.s16	q10, q10, #4
	vshr.s16	q11, q11, #4
	vshr.s16	q4, q4, #4
	vshr.s16	q5, q5, #4
	vshr.s16	q6, q6, #4
	vshr.s16	q7, q7, #4
		
	cmp			r3, #0
	beq			IDCT_CPY_NOSRC_4x8
	LOAD_IDCT_SRC_ADD				
IDCT_CPY_NOSRC_4x8:	
	STR_IDCT					
 .endm	 
  .macro Col8_8x8
 
	LOAD_IDCT8x8_COEF
	
	cmp	r3, #0
	@movne	r12, #8
	@moveq	r12, #0x0808
	mov	r12, #8
	orreq	r12, r12, r12, lsl #8			  
	lsl	r12, r12, #15	
							@q4 x0 x4 d8-d9  
							@q5 x1 x7 d10-d11						
							@q6 x2 x6 d12-d13						
							@q7 x3 x5 d14-d15						
								
	vswp.s32	d9, d16				@q8  x0 x4 d16-17
	vswp.s32	d11, d22			@q9  x1 x7 d18-19
	vswp.s32	d13, d20			@q10 x2 x6 d20-21	
	vswp.s32	d15, d18			@q11 x3 x5 d22-23		
	
	
	Col8_first4line 0
		
	vshrn.s32	d2, q3, #15
	vshrn.s32	d3, q12, #15	
	vshrn.s32	d4, q14, #15
	vshrn.s32	d5, q13, #15
	
	vshrn.s32	d14, q4, #15	
	vshrn.s32	d8, q15, #15
	vshrn.s32	d12, q6, #15
	vshrn.s32	d10, q5, #15		

	Col8_last4line 0
	
	
	vshrn.s32	d9, q15, #15
	vshrn.s32	d11, q11, #15
	vshrn.s32	d13, q10, #15
	vshrn.s32	d15, q8, #15
			
	vshrn.s32	d17, q3, #15
	vshrn.s32	d19, q12, #15	
	vshrn.s32	d21, q14, #15
	vshrn.s32	d23, q13, #15	
	
	vswp.s32	d2, d16
	vswp.s32	d3, d18
	vswp.s32	d4, d20	
	vswp.s32	d5, d22
	
@	Transpose	
			
	VTRN.16 q8, q9
	VTRN.16 q10, q11
	VTRN.16 q4, q5
	VTRN.16 q6, q7
	VTRN.32 q8, q10
	VTRN.32 q9, q11
	VTRN.32 q4, q6
	VTRN.32 q5, q7

							@q8  x0 x1 d16-d17
	vswp.s32	d19, d23			@q11 x4 x5 d18-d19
							@q10 x3 x2 d20-d21	
							@q9  x7 x6 d22-d23
	vswp.s64	q9, q11									
		
							@q4 x0 x1 d8-d9
	vswp.s32	d11, d15			@q5 x4 x5 d10-d11
							@q6 x3 x2 d12-d13	
							@q7 x7 x6 d14-d15								
@q8  x0 x1 d16-d17
@q11 x4 x5 d22-d23
@q10 x3 x2 d20-d21	
@q9  x7 x6 d18-d19	
	
@q4 x0 x1 d8-d9
@q5 x4 x5 d10-d11
@q6 x3 x2 d12-d13
@q7 x7 x6 d14-d15

	vdup.s32	q14, r12
				
	Col8_last4line 1
	vshrn.s32	d2, q15, #15
	vshrn.s32	d3, q11, #15
	vshrn.s32	d4, q10, #15
	vshrn.s32	d5, q8, #15
			
	vshrn.s32	d16, q3, #15
	vshrn.s32	d18, q12, #15	
	vshrn.s32	d20, q14, #15
	vshrn.s32	d22, q13, #15

	vdup.s32	q14, r12
	
	Col8_first4line 1
	
	vshrn.s32	d17, q3, #15
	vshrn.s32	d19, q12, #15	
	vshrn.s32	d21, q14, #15
	vshrn.s32	d23, q13, #15
	
	vshrn.s32	d15, q4, #15	
	vshrn.s32	d9, q15, #15
	vshrn.s32	d13, q6, #15
	vshrn.s32	d11, q5, #15		

	vswp.s32	d2, d8
	vswp.s32	d3, d10
	vswp.s32	d4, d12	
	vswp.s32	d5, d14				
@	Transpose	
	VTRN.16 q8, q9
	VTRN.16 q10, q11
	VTRN.16 q4, q5
	VTRN.16 q6, q7
	VTRN.32 q8, q10
	VTRN.32 q9, q11
	VTRN.32 q4, q6
	VTRN.32 q5, q7
	VSWP d17, d8
	VSWP d19, d10
	VSWP d21, d12
	VSWP d23, d14		
	
	vshr.s16	q8, q8, #4
	vshr.s16	q9, q9, #4
	vshr.s16	q10, q10, #4
	vshr.s16	q11, q11, #4
	vshr.s16	q4, q4, #4
	vshr.s16	q5, q5, #4
	vshr.s16	q6, q6, #4
	vshr.s16	q7, q7, #4
		
	cmp			r3, #0
	beq			IDCT_CPY_NOSRC_8x8
	LOAD_IDCT_SRC_ADD				
IDCT_CPY_NOSRC_8x8:	
	STR_IDCT					
 .endm	
 
	.align 4
W_table:	
		.word 0x7D8A18F8			@xC7 = D0.S16[0] xC1 = D0.S16[1]		
		.word 0x471D6A6D			@xC5 = D0.S16[2] xC3 = D0.S16[3]			
		.word 0x764130FC			@xC6 = D1.S16[0] xC2 = D1.S16[1]		
		.word 0x00005A82			@xC4 = D1.S32[1]/D1.S32[2]	
	.align 4
VP6DEC_VO_Armv7IdctA: @PROC
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride

	adr			r12, W_table	
	vld1.s32 	{q0}, [r12]!
		
	Col8_4x8  
 	mov		pc,lr
	@ENDP
		
	.align 4
VP6DEC_VO_Armv7IdctB: @PROC
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride

	adr			r12, W_table	
	vld1.s32 	{q0}, [r12]!
			
	Col8_8x8  
 	mov		pc,lr
	@ENDP

 
	.align 4	
VP6DEC_VO_Armv7IdctC: @PROC	
@ r0 = Block, r1 = dst, r2 = dst_stride, r3 = Src, r12 = [sp] = src_stride
	ldrsh	r12, [r0]			
	cmp			r3, #0
	add	r12, r12, #15	
	asr	r12, r12, #5	
@	OutD = (INT16)((INT32)(input[0]+15)>>5)@	
	beq			IDCT_CPY_NOSRC	

	vdup.s16	q15, r12
	mov	r12, #0
	strh	r12, [r0]		
	ldr	r12,[sp]	@src_stride
	vld1.64 {d0}, [r3], r12 
	vld1.64 {d1}, [r3], r12 
	vld1.64 {d2}, [r3], r12 
	vld1.64 {d3}, [r3], r12 
	vld1.64 {d4}, [r3], r12 
	vld1.64 {d5}, [r3], r12 
	vld1.64 {d6}, [r3], r12 
	vld1.64 {d7}, [r3]
 
	vaddw.u8	q8, q15, d0
	vaddw.u8	q9, q15, d1
	vaddw.u8	q10, q15, d2
	vaddw.u8	q11, q15, d3
	vaddw.u8	q4, q15, d4
	vaddw.u8	q5, q15, d5
	vaddw.u8	q6, q15, d6
	vaddw.u8	q7, q15, d7

	vqmovun.s16	d16, q8
	vqmovun.s16	d18, q9
	vqmovun.s16	d20, q10
	vqmovun.s16	d22, q11
	vqmovun.s16	d8, q4
	vqmovun.s16	d10, q5
	vqmovun.s16	d12, q6
	vqmovun.s16	d14, q7		
						
	vst1.64	{d16}, [r1], r2
	vst1.64	{d18}, [r1], r2		
	vst1.64	{d20}, [r1], r2
	vst1.64	{d22}, [r1], r2	
	vst1.64	{d8}, [r1], r2
	vst1.64	{d10}, [r1], r2	
	vst1.64	{d12}, [r1], r2
	vst1.64	{d14}, [r1]
	mov		pc,lr	
	


IDCT_CPY_NOSRC:	
@	v = SAT((128 + OutD))@		
	add	r12, r12, #128
	usat	r12, #8, r12
	vdup.u8	d10, r12
	mov	r12, #0
	strh	r12, [r0]	
	vst1.64 {d10}, [r1], r2   
	vst1.64 {d10}, [r1], r2
	vst1.64 {d10}, [r1], r2
	vst1.64 {d10}, [r1], r2
	vst1.64 {d10}, [r1], r2
	vst1.64 {d10}, [r1], r2
	vst1.64 {d10}, [r1], r2
	vst1.64 {d10}, [r1]	
	mov		pc,lr		        
	@ENDP
	
						


	.END