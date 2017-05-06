@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@**************************************************************
@**********************************************************************
@  do_zero_filter_front  function
@**********************************************************************
@void do_zero_filter(
@		short               *input,
@		short               *output,
@		short               numsamples,
@		struct ZERO_FILTER  *filter,
@		short               update_flag 
@		)
@

#include   "voQcelpEncID.h"

	.section  .text
        .global   do_zero_filter_asm
@******************************
@ ARM register 
@******************************
@ *input           RN           0
@ *output          RN           1
@ numsamples       RN           2
@ *filter          RN           3
@ update_flag      RN           4

do_zero_filter_asm:

        STMFD          r13!, {r4 - r12, r14}
        LDR            r5,  [r3, #0x8]                  @ get coeff ptr
        LDR            r4,  [r3, #0x4]                  @ get tmpbuf buffeer address
        MOV            r8,  #0 
	SUB            r4,  r4, #4                      @ get back 2 samples

        VLD1.S16       {D0, D1}, [r5]!                  @ coeffs[0] ~ coeffs[7]
        VLD1.S16       {D2}, [r5]!                      @ D2[0] --> coeffs[8], D2[1] --> coeffs[9]

        MOV            r12, r4
        VLD1.S16       {D4, D5}, [r4]!                  @ D4[2] --> tmpbuf[0], D4[3] --> tmpbuf[1] @ D5 --> tmpbuf[2] ~ tmpbuf[5]
        VLD1.S16       {D6}, [r4]!                      @ D6 --> tmpbuf[6] ~ tmpbuf[9]

LOOP:
	VLD1.S16       {D8, D9}, [r0]!                  @ input[0] ~ input[7]
	VREV64.16      D8, D8                           @ input[0], input[1], input[2], input[3]
        VREV64.16      D9, D9                           @ input[4], input[5], input[6], input[7]
	VSHLL.S16      Q6, D8, #13
	VSHLL.S16      Q7, D9, #13

	VQDMLAL.S16    Q6, D6, D2[1]                    @ (tmpbuf[9] ~ tmpbuf[6]) * coeffs[9]
        VQDMLAL.S16    Q7, D5, D2[1]                    @ (tmpbuf[5] ~ tmpbuf[2]) * coeffs[9]
        VQDMLAL.S16    Q6, D5, D1[1]                    @ (tmpbuf[5] ~ tmpbuf[2]) * coeffs[5]

	VEXT.16        D16, D5, D6, #3                  @ tmpbuf[8] ~ tmpbuf[5]
	VEXT.16        D17, D4, D5, #3                  @ tmpbuf[4] ~ tmpbuf[1]

	VQDMLAL.S16    Q6, D16, D2[0]                   @ (tmpbuf[8] ~ tmpbuf[5]) * coeffs[8]
	VQDMLAL.S16    Q7, D17, D2[0]                   @ (tmpbuf[4] ~ tmpbuf[1]) * coeffs[8]
	VQDMLAL.S16    Q6, D17, D1[0]                   @ (tmpbuf[4] ~ tmpbuf[1]) * coeffs[4]

	VEXT.16        D16, D5, D6, #2                  @ tmpbuf[7] ~ tmpbuf[4]
	VEXT.16        D17, D4, D5, #2                  @ tmpbuf[3] ~ tmpbuf[0]               

	VQDMLAL.S16    Q6, D16, D1[3]                   @ (tmpbuf[7] ~ tmpbuf[4]) * coeffs[7]
	VQDMLAL.S16    Q7, D17, D1[3]                   @ (tmpbuf[3] ~ tmpbuf[0]) * coeffs[7]
	VQDMLAL.S16    Q6, D17, D0[3]                   @ (tmpbuf[3] ~ tmpbuf[0]) * coeffs[3]

	VEXT.16        D16, D5, D6, #1                  @ tmpbuf[6] ~ tmpbuf[3]
	VEXT.16        D18, D8, D17, #3                 @ tmpbuf[2], tmpbuf[1], tmpbuf[0], input[0]

	VQDMLAL.S16    Q6, D16, D1[2]                   @ (tmpbuf[6] ~ tmpbuf[3]) * coeffs[6]
	VQDMLAL.S16    Q7, D18, D1[2]                   @ (tmpbuf[2], tmpbuf[1], tmpbuf[0], input[0]) * coeffs[6]
	VQDMLAL.S16    Q6, D18, D0[2]                   @ (tmpbuf[2], tmpbuf[1], tmpbuf[0], input[0]) * coeffs[2]

        VEXT.16        D16, D8, D17, #2                 @ tmpbuf[1], tmpbuf[0], input[0], input[1]
	VQDMLAL.S16    Q7, D16, D1[1]                   @ (tmpbuf[1], tmpbuf[0], input[0], input[1]) * coeffs[5]
	VQDMLAL.S16    Q6, D16, D0[1]                   @ (tmpbuf[1], tmpbuf[0], input[0], input[1]) * coeffs[1]

	VMOV.S16       D6, D16

	VEXT.16        D16, D8, D17, #1                 @ tmpbuf[0], input[0], input[1], input[2]
	VQDMLAL.S16    Q6, D16, D0[0]                   @ (tmpbuf[0], input[0], input[1], input[2]) * coeffs[0] 
	VQDMLAL.S16    Q7, D16, D1[0]                   @ (tmpbuf[0], input[0], input[1], input[2]) * coeffs[4]
	VQDMLAL.S16    Q7, D8, D0[3]                    @ (input[0], input[1], input[2], input[3]) * coeffs[3]

	VEXT.16        D16, D9, D8, #3                  @ input[1], input[2], input[3], input[4]
	VQDMLAL.S16    Q7, D16, D0[2]                   @ (input[1], input[2], input[3], input[4]) * coeffs[2]

	VEXT.16        D16, D9, D8, #2                  @ input[2], input[3], input[4], input[5]
	VQDMLAL.S16    Q7, D16, D0[1]                   @ (input[2], input[3], input[4], input[5]) * coeffs[1]

	VMOV.S16       D5, D16
	VEXT.16        D16, D9, D8, #1                  @ input[3], input[4], input[5], input[6]
	VQDMLAL.S16    Q7, D16, D0[0]                   @ (input[3], input[4], input[5], input[6]) * coeffs[0]

	VMOV.S16       r9, D9[0]                        @ input[7] 
	VMOV.S16       r10, D9[1]                       @ input[6]
	VMOV.S16       D4[2], r9                        
	VMOV.S16       D4[3], r10

        VQRSHRN.S32    D12, Q6, #13
        VQRSHRN.S32    D13, Q7, #13
	ADD            r8, r8, #8
        VREV64.16      D12, D12
	VREV64.16      D13, D13
        VST1.S16       {D12, D13}, [r1]!	
        CMP            r8, r2

        BLT            LOOP
	VST1.S16       {D4, D5, D6}, [r12]!

do_zero_filter_end:
 
        LDMFD          r13!, {r4 - r12, r15} 
        .END


