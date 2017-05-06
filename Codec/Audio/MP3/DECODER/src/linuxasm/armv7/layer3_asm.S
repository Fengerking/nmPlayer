#include "voMP3DecID.h"
	.section .text	
	.global	voMP3Decidct9_2

voMP3Decidct9_2:	@PROC
	stmdb   sp!, {r4 - r11, lr}
M2844:
	mov		r14, r0
	ADR		r2, IDCT9_D
	
	VLD1.32		{D0, D1, D2, D3}, [r14]!
	VLD1.32		{D4, D5, D6, D7}, [r14]!
	VLD1.32		{D8}, [r14]!	
	
	VSUB.S32	D11, D0, D6				@ a1 = x0 - x6@       
    VSUB.S32	D12, D1, D5				@ a2 = x1 - x5@ 
	VADD.S32	D13, D1, D5             @ a3 = x1 + x5@ 
    VSUB.S32    D14, D2, D4             @ a4 = x2 - x4@
    VADD.S32    D15, D2, D4             @ a5 = x2 + x4@
    VADD.S32    D16, D2, D8             @ a6 = x2 + x8@  
    VADD.S32    D17, D1, D7             @ a7 = x1 + x7@
    VSHR.S32	D9, D6, #1
    VLD1.S32	{D2}, [r2]!
    VSUB.S32	D18, D16, D15           @ a8 = a6 - a5@ 
    VADD.S32    D22, D0, D9             @ a12 = x0 + (x6 >> 1)@
    VLD1.S32	{D4}, [r2]!	 
    VSUB.S32	D20, D12, D7            @ a10 = a2 - x7@ 
    VSUB.S32	D19, D13, D17           @ a9 = a3 - a7@  
    VLD1.S32	{D0[]}, [r2]
	VSUB.S32	D21, D14, D8            @ a11 = a4 - x8@  
    VQDMULH.S32 D1, D3, D2[0]			@ m1 =  MUL_32(c9_0, x3)@
	VQDMULH.S32	D7, D18, D2[1]			@ m7 =  MUL_32(c9_1, a8)@
	VQDMULH.S32	D30, D20, D2[0]			@ m3 =  MUL_32(c9_0, a10)@
	VQDMULH.S32 D5, D15, D2[1]			@ m5 =  MUL_32(c9_1, a5)@
	VQDMULH.S32	D6, D16, D4[0]			@ m6 =  MUL_32(c9_2, a6)@
	VQDMULH.S32	D8, D15, D4[0]			@ m8 =  MUL_32(c9_2, a5)@
	VADD.S32	D26, D5, D6				@ a16 = ( m5 + m6 ) << 1@
	VQDMULH.S32	D28, D13, D4[1]			@ m11 = MUL_32(c9_3, a3)@
	VSHR.S32	D31, D21, #1
	VQDMULH.S32	D9, D19, D4[1]			@ m9 =  MUL_32(c9_3, a9)@
	VQDMULH.S32	D10, D17, D0	    	@ m10 = MUL_32(c9_4, a7)@
	VQDMULH.S32	D12, D19, D0			@ m12 = MUL_32(c9_4, a9)@
	VSUB.S32	D17, D7, D8				@ a17 = ( m7 - m8 ) << 1@
	VADD.S32	D19, D9, D10			@ a19 = ( m9 + m10) << 1@
	
	VSUB.S32	D14, D22, D1			@ a14 = a12  -  (  m1 << 1)@
	VADD.S32	D13, D22, D1			@ a13 = a12  +  (  m1 << 1)@		
	VSUB.S32	D20, D28, D12			@ a20 = (m11 - m12) << 1@
	VADD.S32	D22, D13, D26			@ a22 = a13 + a16@
	VADD.S32	D23, D14, D26			@ a23 = a14 + a16@
	VADD.S32	D18, D26, D17			@ a18 = a16 + a17@
	VADD.S32	D24, D14, D17			@ a24 = a14 + a17@
	VADD.S32	D25, D13, D17			@ a25 = a13 + a17@
	VSUB.S32	D26, D14, D18			@ a26 = a14 - a18@
	VSUB.S32	D27, D13, D18			@ a27 = a13 - a18@
	VADD.S32	D15, D11, D31			@ a15 = a1   +  ( a11 >> 1)@
	VSUB.S32	D4, D11, D21            @ x4 = a1 - a11@			x[4] = x4@
	VADD.S32	D0, D22, D19			@ x0 = a22 + a19@			x[0] = x0@
	VSUB.S32	D21, D20, D19			@ a21 = a20 - a19@
	VADD.S32	D1, D15, D30			@ x1 = a15 + (m3 << 1)@		x[1] = x1@
	VADD.S32    D2, D24, D20			@ x2 = a24 + a20@			x[2] = x2@
	VSUB.S32	D3, D26, D21			@ x3 = a26 - a21@			x[3] = x3@
	VADD.S32	D5, D27, D21			@ x5 = a27 + a21@			x[5] = x5@
	VSUB.S32	D6, D25, D20			@ x6 = a25 - a20@			x[6] = x6@
	VSUB.S32	D7, D15, D30			@ x7 = a15 - (m3 << 1)@		x[7] = x7@
	VSUB.S32	D8, D23, D19			@ x8 = a23 - a19@			x[8] = x8@

	VUZP.S32	Q0, Q1
	VUZP.S32	Q2, Q3
	
	VST1.S32	{Q0}, [r0]!
	VST1.S32	{Q2}, [r0]!
	VST1.S32	D8[0], [r0]!
	VST1.S32	{Q1}, [r0]!
	VST1.S32	{Q3}, [r0]!
	VST1.S32	D8[1], [r0]!	
		
	ldmia   sp!, {r4 - r11, pc}
	@ENDP  @ idct9_2


	.global	AliasReduce
	@AREA	.text, CODE, READONLY

AliasReduce: @PROC
	stmdb     sp!, {r4 - r11, lr}
M2693:
  add		  r10, r0, #72
  cmp		  r1, #0      
  sub		  r11, r10, #32
  ADR		  r12, 	csa_table
  ble     	  L1604


	@#define INT_AA(j) \
    @        tmp0 = ptr[-1-j]@\
    @        tmp1 = ptr[   j]@\
    @        tmp2= MUL_32(tmp0 + tmp1, csa[0+4*j])@\
    @        ptr[-1-j] = 4*(tmp2 - MUL_32(tmp1, csa[2+4*j]))@\
    @        ptr[   j] = 4*(tmp2 + MUL_32(tmp0, csa[3+4*j]))@

	VLD4.32			{D8, D10, D12, D14}, [r12]!
	VLD4.32			{D9, D11, D13, D15}, [r12]!
	VLD4.32			{D16, D18, D20, D22}, [r12]!
	VLD4.32			{D17, D19, D21, D23}, [r12]!
	
L2691:
	VLD1.32	  		{D0, D1, D2, D3}, [r11]			@ tmp0
	VLD1.32			{D4, D5, D6, D7}, [r10]			@ tmp1	
	
	VREV64.32		Q1, Q1
	VREV64.32		Q0, Q0
		
	VSWP			D2, D3						@ tmp0
	VSWP			D0, D1						@ tmp0
	
	VADD.S32		Q12, Q1, Q2					@ tmp0 + tmp1
	VADD.S32		Q13, Q0, Q3					@ tmp0 + tmp1	
	
	VQDMULH.S32 	Q12, Q12, Q4				@ tmp2= MUL_32(tmp0 + tmp1, csa[0+4*j])
	VQDMULH.S32 	Q13, Q13, Q8				@ tmp2= MUL_32(tmp0 + tmp1, csa[0+4*j])
	
	VQDMULH.S32		Q2, Q2, Q6					@ MUL_32(tmp1, csa[2+4*j])
	VQDMULH.S32 	Q3, Q3, Q10					@ MUL_32(tmp1, csa[2+4*j])
	
	VQDMULH.S32 	Q1, Q1, Q7					@ MUL_32(tmp0, csa[3+4*j])
	VQDMULH.S32 	Q0, Q0, Q11					@ MUL_32(tmp0, csa[3+4*j])
		
	VSUB.S32		Q2, Q12, Q2
	VSUB.S32		Q3, Q13, Q3
	
	VADD.S32		Q12, Q12, Q1
	VADD.S32		Q13, Q13, Q0
	
	VQSHL.S32		Q1, Q2, #1
	VQSHL.S32		Q0, Q3, #1	

	VQSHL.S32		Q12, Q12, #1	
	
	VREV64.32		Q1, Q1
	VREV64.32		Q0, Q0
		
	VSWP.32			D2, D3						
	VSWP.32			D0, D1						
	
	VQSHL.S32		Q13, Q13, #1	
	
	VST1.32	  	{D0, D1, D2, D3}, [r11]					@ tmp0
	VST1.32			{D24, D25, D26, D27}, [r10]			@ tmp1	
	
	subs      r1, r1, #1
	add       r10, r10, #0x48
	add				r11, r11, #0x48
	bne       L2691

L1604:
	ldmia     sp!, {r4 - r11, pc}

csa_table:
	.word 0x36e12a00	
	.word 0xdf128041	
	.word 0x15f3aa41	
	.word 0xa8315641	
	.word 0x386e7600	
	.word 0xe1cf24a1	
	.word 0x1a3d9aa1	
	.word 0xa960aea1	
	.word 0x3cc6b740	
	.word 0xebf19fa1	
	.word 0x28b856e1	
	.word 0xaf2ae861	
	.word 0x3eeea040	
	.word 0xf45b88c1	
	.word 0x334a2901	
	.word 0xb56ce881	
	.word 0x3fb69040	
	.word 0xf9f27f19	
	.word 0x39a90f59	
	.word 0xba3beed9	
	.word 0x3ff23f40	
	.word 0xfd60d1e5	
	.word 0x3d531125	
	.word 0xbd6e92a5	
	.word 0x3ffe5940	
	.word 0xff175ee4	
	.word 0x3f15b824	
	.word 0xbf1905a4	
	.word 0x3fffe340	
	.word 0xffc3612f	
	.word 0x3fc3446f	
	.word 0xbfc37def	

IDCT9_D:
	.word     0x6ed9eba1
  .word     0x620dbe8b
  .word     0x163a1a7e
  .word     0x5246dd49
  .word     0x7e0e2e32	
	.word			0x0

	.END
	
