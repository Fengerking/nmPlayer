#include "../../Src/c/voVC1DID.h"
	.section .text
	.global  voVC1InvTrans_8x8_Overlap_ARMV7
	.global  voVC1Copy_8x8_ARMV7
	.global  voVC1Copy_8x4_ARMV7
	.global  voVC1Copy_4x8_ARMV7
	.global  voVC1Copy_4x4_ARMV7
	
	.macro Col8_first4line	
	@ q12  S4*W4
@ q13  S0*W4 + 4
	vmull.s16	q12, d9, D1[2]
	vmull.s16	q13, d8, D1[3]
	vadd.s32	q13, q13, q1		@x0 = (x0 << 11) + 4@

@ A q14 = W1*S1 + W7*S7
@ B q15 = W7*S1 - W1*S7

	vmull.s16	q14, d11, D0[0]			@W7*S7
	vmlal.s16	q14, d10, D0[1]			@q14 = W1*S1 + W7*S7
	
	vmull.s16	q15, d10, D0[0]			@W7*S1
	vmlsl.s16	q15, d11, D0[1]			@q15 = W7*S1 - W1*S7
	
@	C q4 = W3*S3 + W5*S5
@	D q5 = W3*S5 - W5*S5	

	vmull.s16	q4, d14, D0[2]			@W3*S3
	vmlal.s16	q4, d15, D0[3]			@q4 = W3*S3 + W5*S5
	
	vmull.s16	q5, d15, D0[2]			@W3*S5
	vmlsl.s16	q5, d14, D0[3]			@q5 = W3*S5 - W5*S5
	
@	G q7 = W6*S6 + W2*S2
@	H q3 = W6*S2 - W2*S6

	vmull.s16	q7, d13, D1[0]			@W6*S6
	vmlal.s16	q7, d12, D1[1]			@q7 = W6*S6 + W2*S2
	
	vmull.s16	q3, d12, D1[0]			@W6*S2
	vmlsl.s16	q3, d13, D1[1]			@q3 = W6*S2 - W2*S6
	
@	E q6  = W4*S0 + W4*S4 + 4
@	F q13 = W4*S0 - W4*S4 + 4
	vadd.s32	q6, q13, q12		@W4*S0 + W4*S4 + 4
	vsub.s32	q13, q13, q12		@W4*S0 - W4*S4 + 4
		

	vadd.s32	q12, q14, q4		@q12 Cd = A + C
	vmul.s32    q14, q14,d4[0]@24576
	vmul.s32    q4, q4,d4[1]@21845
	vsub.s32	q14, q14, q4		@q14 Ad = A - C ---> Ad = 24576 *A - 21845*C	 
	
	vadd.s32	q4, q15, q5			@q4  Dd =  B + D	
	vmul.s32    q15, q15,d4[0]@24576 
	vmul.s32    q5, q5,d4[1]@21845 
	vsub.s32	q15, q15, q5		@q15 Bd = B - D ---> Bd = 24576 *B - 21845*D 
	
	vadd.s32	q5, q6, q7			@q5  Fd = E + G
	vsub.s32	q6, q6, q7			@q6  Ed = E - G
	
	vadd.s32	q7, q13, q3			@q7  Hd = F + H
	vsub.s32	q13, q13, q3		@q13 Gd = F - H

@	Bdd q3 = (181 * (Ad + Bd) + 128) >> 8@
@	Add q14 = (181 * (Ad - Bd) + 128) >> 8@
@Ad = 24576*A - 21845*C@
@Bd = 24576*B - 21845*D@
@Add = ((Ad - Bd + 16*1024) >>15)@
@Bdd = ((Ad + Bd + 16*1024) >> 15)@
	vmov.s32	q2, #0x4000
	vadd.s32	q3, q14, q15    @Ad + Bd
	vadd.s32	q3, q3, q2	    @(Ad + Bd + 16*1024)
	vshr.s32	q3, q3, #15		@q3 Bdd = (Ad + Bd + 16*1024) >> 8@
					
	@vadd.s32	q3, q14, q15    @Ad + Bd
	@vmul.s32	q3, q3, D1[1]   @(Ad + Bd)*181
	@vadd.s32	q3, q3, q1	    @(Ad + Bd)*181 + 128
	@vshr.s32	q3, q3, #8		@q3 Bdd = (181 * (Ad + Bd) + 128) >> 8@

	vsub.s32	q14, q14, q15	 
	vadd.s32	q14, q14, q2			
	vshr.s32	q14, q14, #15	  @q14 Add = (Ad - Bd + 16*1024) >> 8@
	@vsub.s32	q14, q14, q15	 
	@vmul.s32	q14, q14, D1[1]
	@vadd.s32	q14, q14, q1			
	@vshr.s32	q14, q14, #8	  @q14 Add = (181 * (Ad - Bd) + 128) >> 8@

	vadd.s32	q15, q5, q12		@Blk[ 0] = (idct_t)((x7 + x1) >> 8)@
	vsub.s32	q5, q5, q12			@Blk[56] = (idct_t)((x7 - x1) >> 8)@
	vadd.s32	q12, q7, q3			@Blk[8] = (idct_t)((x3 + x2) >> 8)@
	vsub.s32	q7, q7, q3			@Blk[48] = (idct_t)((x3 - x2) >> 8)@
	vadd.s32	q3, q13, q14		@Blk[16] = (idct_t)((x0 + x4) >> 8)@
	vsub.s32	q13, q13, q14		@Blk[40] = (idct_t)((x0 - x4) >> 8)@
	vadd.s32	q14, q6, q4			@Blk[24] = (idct_t)((x8 + x6) >> 8)@
	vsub.s32	q6, q6, q4			@Blk[32] = (idct_t)((x8 - x6) >> 8)@
	
	@vshrn.s32	d2, q15, #3  @0
	@vshrn.s32	d3, q12, #3  @8
	@vshrn.s32	d4, q3, #3   @16
	@vshrn.s32	d5, q14, #3  @24
	@vshrn.s32	d8, q6, #3   @32	
	@vshrn.s32	d12, q7, #3  @48
	@vshrn.s32	d14, q5, #3  @56
	@vshrn.s32	d10, q13, #3 @40
	.endm
	
	.macro Col8_last4line
	vld1.s16 	{d9}, [r12] @d9 24576  21845
@ q12  S4*W4
@ q13  S0*W4 + 64
	vmov.s32	q3, #4	
	vmull.s16	q12, d17, D1[2]
	vmull.s16	q13, d16, D1[3]
	vadd.s32	q13, q13, q3		

@ A q14 = W1*S1 + W7*S7
@ B q15 = W7*S1 - W1*S7
	vmull.s16	q14, d23, D0[0]			@W7*S7
	vmlal.s16	q14, d22, D0[1]			@q14 = W1*S1 + W7*S7
	
	vmull.s16	q15, d22, D0[0]			@W7*S1
	vmlsl.s16	q15, d23, D0[1]			@q15 = W7*S1 - W1*S7
	
@	C q8 = W3*S3 + W5*S5
@	D q11 = W3*S5 - W5*S5	
	vmull.s16	q8, d18, D0[2]			@W3*S3
	vmlal.s16	q8, d19, D0[3]			@q8 = W3*S3 + W5*S5
	
	vmull.s16	q11, d19, D0[2]			@W3*S5
	vmlsl.s16	q11, d18, D0[3]			@q11 = W3*S5 - W5*S5
	
@	G q9 = W6*S6 + W2*S2
@	H q3 = W6*S2 - W2*S6
	vmull.s16	q9, d21, D1[0]			@W6*S6 
	vmlal.s16	q9, d20, D1[1]			@q9 = W6*S6 + W2*S2
	
	vmull.s16	q3, d20, D1[0]			@W6*S2
	vmlsl.s16	q3, d21, D1[1]			@q3 = W6*S2 - W2*S6
	
@	x8 = x0 + x1@
@	x0 -= x1@
	vadd.s32	q10, q13, q12		@E q10 = W4*S0 + W4*S4 + 64
	vsub.s32	q13, q13, q12		@F q13 = W4*S0 - W4*S4 + 64

	vadd.s32	q12, q14, q8		@q12 Cd = A + C
	@vsub.s32	q14, q14, q8		@q14 Ad = A - C ----> Ad = 24576 *A - 21845*C
	vmul.s32    q14, q14, d9[0]
	vmul.s32    q8,  q8,  d9[1]
	vsub.s32	q14, q14, q8
	
	vadd.s32	q8, q15, q11		@q8  Dd = B + D
	@vsub.s32	q15, q15, q11		@q15 Bd = B - D ----> Bd = 24576 *B - 21845*D 
	vmul.s32    q15, q15, d9[0]
	vmul.s32    q11, q11, d9[1]
	vsub.s32	q15, q15, q11
	
	vadd.s32	q11, q10, q9		@q11 Fd = E + G
	vsub.s32	q10, q10, q9		@q10 Ed = E - G
	
	vadd.s32	q9, q13, q3			@q9  Hd = F + H
	vsub.s32	q13, q13, q3		@q13 Gd = F - H
	
	vmov.s32	q0, #0x4000	
	vadd.s32    q3, q14, q15		
	vadd.s32    q3, q3,  q0
	vshr.s32	q3, q3,  #15         @q3 Bdd = (Ad + Bd + 16*1024) >> 15
	
	vsub.s32    q14,  q14, q15		
	vadd.s32    q14,  q14,  q0
	vshr.s32	q14,  q14,  #15       @q14 Add = (Ad - Bd + 16*1024) >> 15

	vadd.s32	q15, q11, q12		@Blk[ 0] = (idct_t)((Fd + Cd) >> 8)@
	vsub.s32	q11, q11, q12		@Blk[56] = (idct_t)((Fd - Cd) >> 8)@
	vadd.s32	q12, q9, q3			@Blk[8] = (idct_t)((Hd + Bdd) >> 8)@
	vsub.s32	q9, q9, q3			@Blk[48] = (idct_t)((Hd - Bdd) >> 8)@
	vadd.s32	q3, q13, q14		@Blk[16] = (idct_t)((Gd + Add) >> 8)@
	vsub.s32	q13, q13, q14		@Blk[40] = (idct_t)((Gd - Add) >> 8)@
	vadd.s32	q14, q10, q8		@Blk[24] = (idct_t)((Ed + Dd) >> 8)@
	vsub.s32	q10, q10, q8		@Blk[32] = (idct_t)((Ed - Dd) >> 8)@
	
	vshrn.s32	d9, q10,  #3 @32
	vshrn.s32	d11, q13, #3 @40
	vshrn.s32	d13, q9,  #3 @48
	vshrn.s32	d15, q11, #3 @56			
	vshrn.s32	d17, q15, #3 @0
	vshrn.s32	d19, q12, #3 @8	
	vshrn.s32	d21, q3,  #3 @16
	vshrn.s32	d23, q14, #3 @24	
	.endm
	
	.macro Row_8x8line
	vmov.s32	q3, #64	
	vmull.s16	q12, d17, D1[2]		  @q12 = S4*W4
	vmull.s16	q13, d16, D1[3]     
	vadd.s32	q13, q13, q3        @q13  S0*W4 + 64

	vmull.s16	q14, d23, D0[0]			@W7*S7
	vmlal.s16	q14, d22, D0[1]			@A q14 = W1*S1 + W7*S7	
	vmull.s16	q15, d22, D0[0]			@W7*S1
	vmlsl.s16	q15, d23, D0[1]			@B q15 = W7*S1 - W1*S7

	vmull.s16	q8, d18, D0[2]			@W3*S3
	vmlal.s16	q8, d19, D0[3]			@C q8 = W3*S3 + W5*S5	
	vmull.s16	q11, d19, D0[2]			@W3*S5
	vmlsl.s16	q11, d18, D0[3]			@D q11 = W3*S5 - W5*S5

	vmull.s16	q9, d21, D1[0]			@W6*S6
	vmlal.s16	q9, d20, D1[1]			@G q9 = W6*S6 + W2*S2	
	vmull.s16	q3, d20, D1[0]			@W6*S2
	vmlsl.s16	q3, d21, D1[1]			@H q3 = W6*S2 - W2*S6
		
	vadd.s32	q10, q13, q12		@E q10 = S4*W4 + S0*W4 + 64 
	vsub.s32	q13, q13, q12		@F q13 = S0*W4 - S4*W4 + 64    

	vadd.s32	q12, q14, q8		@q12 Cd = A + C     
	@vsub.s32	q14, q14, q8		@q14 Ad = A - C
	vmul.s32  q14, q14,d4[0]  @24576
	vmul.s32  q8,  q8, d4[1]  @21845
	vsub.s32	q14, q14, q8		@q14 Ad = A - C ---> Ad = 24576 *A - 21845*C
	
	vadd.s32	q8, q15, q11		@q8  Dd = B + D
	@vsub.s32	q15, q15, q11		@q15 Bd = B - D
	vmul.s32  q15, q15,d4[0]  @24576 
	vmul.s32  q11, q11,d4[1]  @21845 
	vsub.s32	q15, q15, q11		@q15 Bd = B - D ---> Bd = 24576 *B - 21845*D

	vadd.s32	q11, q10, q9		@q11 Fd = E + G
	vsub.s32	q10, q10, q9		@q10 Ed = E - G	
	
	vadd.s32	q9, q13, q3			@q9  Hd = F + H
	vsub.s32	q13, q13, q3		@q13 Gd = F - H
	
	vmov.s32	q2, #0x4000
	vadd.s32  q3, q14, q15    @Ad + Bd
	vadd.s32  q3, q3, q2
	vshr.s32	q3, q3, #15		  @q3 Bdd = (Ad + Bd + 16*1024) >> 8@
	
	vsub.s32	q14, q14, q15		@Ad - Bd	
	vadd.s32  q14, q14, q2
	vshr.s32	q14, q14, #15	  @q14 Add = (Ad - Bd + 16*1024) >> 8@

	vadd.s32	q15, q11, q12		@Blk[ 0] = (idct_t)((Fd + Cd) >> 8) 
	vsub.s32	q11, q11, q12		@Blk[56] = (idct_t)((Fd - Cd) >> 8) 
	vadd.s32	q12, q9, q3			@Blk[8] = (idct_t)((Hd + Bdd) >> 8) 
	vsub.s32	q9, q9, q3			@Blk[48] = (idct_t)((Hd - Bdd) >> 8)
	vadd.s32	q3, q13, q14		@Blk[16] = (idct_t)((Gd + Add) >> 8)
	vsub.s32	q13, q13, q14		@Blk[40] = (idct_t)((Gd - Add) >> 8)
	vadd.s32	q14, q10, q8		@Blk[24] = (idct_t)((Ed + Dd) >> 8) 
	vsub.s32	q10, q10, q8		@Blk[32] = (idct_t)((x8 - x6) >> 8)	
	
	vmov.s32	q2, #0x0001
	
	vadd.s32	q10,  q10,  q2			@Blk[32]
	vadd.s32	q13,  q13,  q2		  @Blk[40]
	vadd.s32	q9,   q9,   q2		  @Blk[48]
	vadd.s32	q11,  q11,  q2		  @Blk[56]
	
	vshrn.s32	d2, q10, #7    @32     
	vshrn.s32	d3, q13, #7    @40     
	vshrn.s32	d4, q9, #7     @48    
	vshrn.s32	d5, q11, #7    @56     
	vshrn.s32	d16, q15, #7   @0     
	vshrn.s32	d18, q12, #7	 @8     
	vshrn.s32	d20, q3, #7    @16     
	vshrn.s32	d22, q14, #7	 @24     

@row one
	vmov.s32	q3, #64	                            
	vmull.s16	q12, d9, D1[2]		  @q12 = S4*W4    
	vmull.s16	q13, d8, D1[3]                     
	vadd.s32	q13, q13, q3        @q13  S0*W4 + 64

	vmull.s16	q14, d11, D0[0]			@W7*S7                
	vmlal.s16	q14, d10, D0[1]			@A q14 = W1*S1 + W7*S7	
	vmull.s16	q15, d10, D0[0]			@W7*S1                
	vmlsl.s16	q15, d11, D0[1]			@B q15 = W7*S1 - W1*S7

	vmull.s16	q4, d14, D0[2]			@W3*S3                
	vmlal.s16	q4, d15, D0[3]			@C q4 = W3*S3 + W5*S5	
	vmull.s16	q5, d15, D0[2]			@W3*S5                
	vmlsl.s16	q5, d14, D0[3]			@D q5 = W3*S5 - W5*S5

	vmull.s16	q7, d13, D1[0]			@W6*S6               
	vmlal.s16	q7, d12, D1[1]			@G q7 = W6*S6 + W2*S2
	vmull.s16	q3, d12, D1[0]			@W6*S2               
	vmlsl.s16	q3, d13, D1[1]			@H q3 = W6*S2 - W2*S6

	vadd.s32	q6, q13, q12		@E q6  = S4*W4 + S0*W4 + 64
	vsub.s32	q13, q13, q12		@F q13 = S0*W4 - S4*W4 + 64

	vld1.s16 	{d0}, [r12] 		@d9 24576  21845

	vadd.s32	q12, q14, q4		@q12 Cd = A + C
	@vsub.s32	q14, q14, q4		@q14 Ad = A - C
	vmul.s32  q14, q14,d0[0]  @24576 
	vmul.s32  q4,  q4, d0[1]  @21845 
	vsub.s32	q14, q14, q4		 @q14 Ad = A - C ---> Ad = 24576 *A - 21845*C
	
	vadd.s32	q4, q15, q5			@q4  Dd = B + D
	@vsub.s32	q15, q15, q5		@q15 Bd = B - D  
	vmul.s32  q15, q15,d0[0]  @24576 
	vmul.s32  q5,  q5, d0[1]  @21845 
	vsub.s32	q15, q15, q5		 @q15 Bd = B - D ---> Bd = 24576 *B - 21845*D 	

	vadd.s32	q5, q6, q7			@q5 Fd = E + G
	vsub.s32	q6, q6, q7			@q6 Ed = E - G	
	
	vadd.s32	q7, q13, q3			@q7  Hd = F + H 
	vsub.s32	q13, q13, q3		@q13 Gd = F - H 
	
	vmov.s32	q0, #0x4000
	vadd.s32	q3, q14, q15		
	vadd.s32	q3, q3,  q0
	vshr.s32	q3, q3,  #15         @q3 Bdd = (Ad + Bd + 16*1024) >> 15
	
	vsub.s32	q14,  q14, q15		
	vadd.s32	q14,  q14,  q0
	vshr.s32	q14,  q14,  #15			@q14 Add = (Ad - Bd + 16*1024) >> 15 
	
	vmov.s32	q0, #0x0001

	vadd.s32	q15, q5, q12		@Blk[ 0] = (idct_t)((x7 + x1) >> 8)@
	vsub.s32	q5, q5, q12			@Blk[56] = (idct_t)((x7 - x1) >> 8)@
	vadd.s32	q12, q7, q3			@Blk[8] = (idct_t)((x3 + x2) >> 8)@
	vsub.s32	q7, q7, q3			@Blk[48] = (idct_t)((x3 - x2) >> 8)@
	vadd.s32	q3, q13, q14		@Blk[16] = (idct_t)((x0 + x4) >> 8)@
	vsub.s32	q13, q13, q14		@Blk[40] = (idct_t)((x0 - x4) >> 8)@
	vadd.s32	q14, q6, q4			@Blk[24] = (idct_t)((x8 + x6) >> 8)@
	vsub.s32	q6, q6, q4			@Blk[32] = (idct_t)((x8 - x6) >> 8)@
	
	
	vadd.s32	q6,  q6,  q0			@Blk[32]
	vadd.s32	q13, q13, q0		  @Blk[40]
	vadd.s32	q7,  q7,  q0		  @Blk[48]
	vadd.s32	q5,  q5,  q0		  @Blk[56]
	
	vshrn.s32	d17, q15, #7 @0
	vshrn.s32	d19, q12, #7 @8
	vshrn.s32	d21, q3,  #7 @16
	vshrn.s32	d23, q14, #7 @24
	vshrn.s32	d9,  q6,  #7 @32
	vshrn.s32	d13, q7,  #7 @48
	vshrn.s32	d15, q5,  #7 @56
	vshrn.s32	d11, q13, #7 @40	
	.endm	

	
voVC1InvTrans_8x8_Overlap_ARMV7:
@void voVC1InvTrans_8x8_Overlap_C
@r0           unsigned char*pDest,
@r1           int dststride,
@r2           unsigned char* pRef,                                            
@r3           short* pDestOverlap,
@r4           int refstride,
@r5           short *block,
@r6           int overlapstride)
	stmdb   sp!, {r4-r6,r12,lr}
	ldr			r4, [ sp, #20]  @ refstride
	ldr			r5, [ sp, #24]  @ block
	ldr			r6, [ sp, #28]  @ overlapstride
	@ldr			r12, =W_table
	ldr			r12, [ sp, #32]  @ table
	vld1.s16 	{q0}, [r12]!
	vld1.s16 	{d4}, [r12] @d4
	vmov.s32	q1, #4
	add         r6,r6,r6
	
	@load src data
	vld1.64	{q4}, [r5]!		@q4 x0 x1 d8-d9
	vld1.64	{q5}, [r5]!		@q5 x4 x5 d10-d11
	vld1.64	{q6}, [r5]!		@q6 x3 x2 d12-d13
	vld1.64	{q7}, [r5]!		@q7 x7 x6 d14-d15
	vld1.64	{q8}, [r5]!		@
	vld1.64	{q9}, [r5]!		@
	vld1.64	{q10}, [r5]!	@
	vld1.64	{q11}, [r5]		@
	
	vswp.s32	d9, d16		@q8  x0 x1 d16-17
	vswp.s32	d15, d18	@q11 x4 x5 d22-23
	vswp.s32	d13, d20	@q10 x3 x2 d20-21	
	vswp.s32	d11, d22	@q9  x7 x6 d18-19
	
	@first four line	
	Col8_first4line
	
	vshrn.s32	d2, q15, #3  @0
	vshrn.s32	d3, q12, #3  @8
	vshrn.s32	d4, q3, #3   @16
	vshrn.s32	d5, q14, #3  @24
	vshrn.s32	d8, q6, #3   @32	
	vshrn.s32	d12, q7, #3  @48
	vshrn.s32	d14, q5, #3  @56
	vshrn.s32	d10, q13, #3 @40
	
	@last four line
	Col8_last4line	
	
	@@@@@@@
	vswp.s32	d2, d16
	vswp.s32	d3, d18
	vswp.s32	d4, d20	
	vswp.s32	d5, d22
	
	@@@@@@@@@@@@@@
	@Transpose	
	vtrn.16 q8, q9
	vtrn.16 q10, q11
	vtrn.16 q4, q5
	vtrn.16 q6, q7
	vtrn.32 q8, q10
	vtrn.32 q9, q11
	vtrn.32 q4, q6
	vtrn.32 q5, q7
	
	vswp.s32	d19, d23
	vswp.s64	q9, q11
	vswp.s32	d11, d15
	
	@@@@@@@@
	@ldr			r12, =W_table
	ldr			r12, [ sp, #32]  @ table
	vld1.s16 	{q0}, [r12]!
	vld1.s16 	{d4}, [r12] @d4
	
	Row_8x8line		
	
	vswp.s32	d2, d8
	vswp.s32	d3, d10
	vswp.s32	d4, d12	
	vswp.s32	d5, d14
	
	@@@@@@write to Overlap@@@@
	vst1.16	{q8},  [r3], r6
	vst1.16	{q9},  [r3], r6		
	vst1.16	{q10}, [r3], r6
	vst1.16	{q11}, [r3], r6	
	vst1.16	{q4},  [r3], r6
	vst1.16	{q5},  [r3], r6	
	vst1.16	{q6},  [r3], r6
	vst1.16	{q7},  [r3]
	
	@@@@@@write to dest@@@@@@@
	cmp			r2, #0   @ref0
	beq			IDCT_CPY_8x8_OL
	vld1.64 {d0}, [r2], r4 
 	vld1.64 {d1}, [r2], r4
 	vld1.64 {d2}, [r2], r4 
 	vld1.64 {d3}, [r2], r4 
 	vld1.64 {d4}, [r2], r4 
 	vld1.64 {d5}, [r2], r4 
 	vld1.64 {d6}, [r2], r4 
 	vld1.64 {d7}, [r2]
 			
	vaddw.u8	q8, q8, d0
	vaddw.u8	q9, q9, d1
	vaddw.u8	q10, q10, d2
	vaddw.u8	q11, q11, d3
	vaddw.u8	q4, q4, d4
	vaddw.u8	q5, q5, d5
	vaddw.u8	q6, q6, d6
	vaddw.u8	q7, q7, d7
IDCT_CPY_8x8_OL:
	vqmovun.s16	d16, q8
	vqmovun.s16	d18, q9
	vqmovun.s16	d20, q10
	vqmovun.s16	d22, q11
	vqmovun.s16	d8, q4
	vqmovun.s16	d10, q5
	vqmovun.s16	d12, q6
	vqmovun.s16	d14, q7		
						
	vst1.64	{d16}, [r0], r1
	vst1.64	{d18}, [r0], r1		
	vst1.64	{d20}, [r0], r1
	vst1.64	{d22}, [r0], r1	
	vst1.64	{d8},  [r0], r1
	vst1.64	{d10}, [r0], r1	
	vst1.64	{d12}, [r0], r1
	vst1.64	{d14}, [r0]		
		
	@@@@@@@@@@@@@@@@@@@@@@@@@@	
	ldmia		sp!, {r4-r6,r12, pc}

voVC1Copy_8x8_ARMV7:
@void voVC1Copy_8x8_C(
@r0 unsigned char* pDst, 
@r1 int dststride, 
@r2 unsigned char *pRef0,
@r3 unsigned char *pRef1,
@r4 int refstride)
	stmdb   sp!, {r4,lr}
	ldr			r4, [ sp, #8]  	@refstride
	
	vld1.u64		d0, [r2], r4  	@pRef0
	vld1.u64		d2, [r2], r4
	vld1.u64		d4, [r2], r4
	vld1.u64		d6, [r2], r4
	vld1.u64		d8, [r2], r4
	vld1.u64		d10, [r2], r4
	vld1.u64		d12, [r2], r4
	vld1.u64		d14, [r2]
	
	cmp 	r3, #0
	beq		IDCT_CPY_REF_8x8
	vld1.u64		d1, [r3], r4  @pRef1
	vld1.u64		d3, [r3], r4
	vld1.u64		d5, [r3], r4
	vld1.u64		d7, [r3], r4
	vld1.u64		d9, [r3], r4
	vld1.u64		d11, [r3], r4
	vld1.u64		d13, [r3], r4
	vld1.u64		d15, [r3]	
	
	vrhadd.u8 d0, d0, d1
 	vrhadd.u8 d1, d2, d3
 	vrhadd.u8 d2, d4, d5
 	vrhadd.u8 d3, d6, d7
 	vrhadd.u8 d4, d8, d9
 	vrhadd.u8 d5, d10, d11
 	vrhadd.u8 d6, d12, d13
 	vrhadd.u8 d7, d14, d15
 	
 	vst1.u64   d0, [r0], r1
 	vst1.u64   d1, [r0], r1 
	vst1.u64   d2, [r0], r1
	vst1.u64   d3, [r0], r1
	vst1.u64   d4, [r0], r1
	vst1.u64   d5, [r0], r1
	vst1.u64   d6, [r0], r1
	vst1.u64   d7, [r0]
	
	ldmia	sp!, {r4,pc}
	
IDCT_CPY_REF_8x8:
	vst1.u64   d0, [r0], r1 
	vst1.u64   d2, [r0], r1
	vst1.u64   d4, [r0], r1
	vst1.u64   d6, [r0], r1
	vst1.u64   d8, [r0], r1
	vst1.u64   d10, [r0], r1
	vst1.u64   d12, [r0], r1
	vst1.u64   d14, [r0]
	ldmia	sp!, {r4,pc}
	
	
voVC1Copy_4x8_ARMV7:
@void voVC1Copy_8x8_C(
@r0 unsigned char* pDst, 
@r1 int dststride, 
@r2 unsigned char *pRef0,
@r3 unsigned char *pRef1,
@r4 int refstride)
	stmdb   sp!, {r4,lr}
	ldr			r4, [ sp, #8]  	@refstride
	
	vld1.u64		d0, [r2], r4  	@pRef0
	vld1.u64		d2, [r2], r4
	vld1.u64		d4, [r2], r4
	vld1.u64		d6, [r2], r4
	vld1.u64		d8, [r2], r4
	vld1.u64		d10, [r2], r4
	vld1.u64		d12, [r2], r4
	vld1.u64		d14, [r2]
	
	cmp 	r3, #0
	beq		IDCT_CPY_REF_4x8
	vld1.u64		d1, [r3], r4  @pRef1
	vld1.u64		d3, [r3], r4
	vld1.u64		d5, [r3], r4
	vld1.u64		d7, [r3], r4
	vld1.u64		d9, [r3], r4
	vld1.u64		d11, [r3], r4
	vld1.u64		d13, [r3], r4
	vld1.u64		d15, [r3]	
	
	vrhadd.u8 d0, d0, d1
 	vrhadd.u8 d1, d2, d3
 	vrhadd.u8 d2, d4, d5
 	vrhadd.u8 d3, d6, d7
 	vrhadd.u8 d4, d8, d9
 	vrhadd.u8 d5, d10, d11
 	vrhadd.u8 d6, d12, d13
 	vrhadd.u8 d7, d14, d15
 	
 	vst1.u32   {d0[0]}, [r0], r1
 	vst1.u32   {d1[0]}, [r0], r1 
	vst1.u32   {d2[0]}, [r0], r1
	vst1.u32   {d3[0]}, [r0], r1
	vst1.u32   {d4[0]}, [r0], r1
	vst1.u32   {d5[0]}, [r0], r1
	vst1.u32   {d6[0]}, [r0], r1
	vst1.u32   {d7[0]}, [r0]
	
	ldmia	sp!, {r4,pc}
	
IDCT_CPY_REF_4x8:	
	vst1.u32   {d0[0]}, [r0], r1 
	vst1.u32   {d2[0]}, [r0], r1
	vst1.u32   {d4[0]}, [r0], r1
	vst1.u32   {d6[0]}, [r0], r1
	vst1.u32   {d8[0]}, [r0], r1
	vst1.u32   {d10[0]}, [r0], r1
	vst1.u32   {d12[0]}, [r0], r1
	vst1.u32   {d14[0]}, [r0]
	ldmia	sp!, {r4,pc}
	
	
voVC1Copy_8x4_ARMV7:
@void voVC1Copy_8x8_C(
@r0 unsigned char* pDst, 
@r1 int dststride, 
@r2 unsigned char *pRef0,
@r3 unsigned char *pRef1,
@r4 int refstride)
	stmdb   sp!, {r4,lr}
	ldr			r4, [ sp, #8]  	@refstride
	
	vld1.u64		d0, [r2], r4  	@pRef0
	vld1.u64		d2, [r2], r4
	vld1.u64		d4, [r2], r4
	vld1.u64		d6, [r2]
	
	cmp 	r3, #0
	beq		IDCT_CPY_REF_8x4
	vld1.u64		d1, [r3], r4  @pRef1
	vld1.u64		d3, [r3], r4
	vld1.u64		d5, [r3], r4
	vld1.u64		d7, [r3]	
	
	vrhadd.u8 d0, d0, d1
 	vrhadd.u8 d1, d2, d3
 	vrhadd.u8 d2, d4, d5
 	vrhadd.u8 d3, d6, d7
 	
 	vst1.u64   d0, [r0], r1
 	vst1.u64   d1, [r0], r1 
	vst1.u64   d2, [r0], r1
	vst1.u64   d3, [r0]
	
	ldmia	sp!, {r4,pc}	
	
IDCT_CPY_REF_8x4:
	vst1.u64   d0, [r0], r1 
	vst1.u64   d2, [r0], r1
	vst1.u64   d4, [r0], r1
	vst1.u64   d6, [r0]
	ldmia	sp!, {r4,pc}
	
	
voVC1Copy_4x4_ARMV7:
@void voVC1Copy_8x8_C(
@r0 unsigned char* pDst, 
@r1 int dststride, 
@r2 unsigned char *pRef0,
@r3 unsigned char *pRef1,
@r4 int refstride)
	stmdb   sp!, {r4,lr}
	ldr			r4, [ sp, #8]  	@refstride
	
	vld1.u64		d0, [r2], r4  	@pRef0
	vld1.u64		d2, [r2], r4
	vld1.u64		d4, [r2], r4
	vld1.u64		d6, [r2]
	
	cmp 	r3, #0
	beq		IDCT_CPY_REF_4x4
	vld1.u64		d1, [r3], r4  @pRef1
	vld1.u64		d3, [r3], r4
	vld1.u64		d5, [r3], r4
	vld1.u64		d7, [r3]	
	
	vrhadd.u8 d0, d0, d1
 	vrhadd.u8 d1, d2, d3
 	vrhadd.u8 d2, d4, d5
 	vrhadd.u8 d3, d6, d7
 	
 	vst1.u32   {d0[0]}, [r0], r1
 	vst1.u32   {d1[0]}, [r0], r1 
	vst1.u32   {d2[0]}, [r0], r1
	vst1.u32   {d3[0]}, [r0]
	
	ldmia	sp!, {r4,pc}
	
IDCT_CPY_REF_4x4:
	vst1.u32   {d0[0]}, [r0], r1 
	vst1.u32   {d2[0]}, [r0], r1
	vst1.u32   {d4[0]}, [r0], r1
	vst1.u32   {d6[0]}, [r0]
	ldmia	sp!, {r4,pc}
	
	
		.align 4
W_table:	
		.word 0x00100004			@w7 = D0.S16[0]	w1 = D0.S16[1]	 @D0.S32[0]	
		.word 0x0009000f			@w3 = D0.S16[2] w5 = D0.S16[3]	 @D0.S32[1]		
		.word 0x00100006			@w6 = D1.S16[0] w2 = D1.S16[1]   @D1.S32[0]
		.word 0x000c000c      @w4 = D1.S16[2] w0 = D1.S16[3] 	 @D1.S32[1]	
		.word 0x00006000		                                   @D4.S32[0]
		.word 0x00005555		                                   @D4.S32[1]
		.word 0x00004000
	
	
	.end
	
