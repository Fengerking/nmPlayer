;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;**************************************************************

;**********************************************************************
;  do_zero_filter_front  function
;**********************************************************************
;void do_zero_filter_front(
;		short               *input,
;		short               *output,
;		short               numsamples,
;		struct ZERO_FILTER  *filter,
;		short               update_flag 
;		)
;
	AREA	|.text|, CODE, READONLY

        EXPORT   do_zero_filter_front_asm
;******************************
; constant
;******************************


;******************************
; ARM register 
;******************************
; *input           RN           0
; *output          RN           1
; numsamples       RN           2
; *filter          RN           3
; update_flag      RN           4

do_zero_filter_front_asm     FUNCTION

        STMFD           r13!, {r4 - r12, r14}
        MOV             r10, #0    
        LDR             r5, [r3, #0x8]                      ; get coeff ptr
        LDR             r4, [r3, #0x4]                      ; get tmpbuf buffeer address

	VLD1.S16        {D0}, [r5]!                         ; XX, XX, temp2, temp1
        MOV             r12, r4

        VLD1.S16        {D1}, [r4]!                         ; XX, XX, tmpbuf[1], tmpbuf[0]	

LOOP
        VLD1.S16        {D2, D3}, [r0]!                     ; input[0] ~ input[7]

	VSHLL.S16       Q6, D2, #13
	VSHLL.S16       Q7, D3, #13

	VREV64.16       D2, D2                              ; input[0], input[1], input[2], input[3]
	VREV64.16       D3, D3                              ; input[4], input[5], input[6], input[7]
        
        VEXT.16         D4, D2, D1, #2                      ; tmpbuf[1], tmpbuf[0], input[0], input[1]
	VEXT.16         D5, D2, D1, #1                      ; tmpbuf[0], input[0], input[1], input[2]
	VEXT.16         D6, D3, D2, #2                      ; input[2], input[3], input[4], input[5]
	VEXT.16         D7, D3, D2, #1                      ; input[3], input[4], input[5], input[6]

	VREV64.16       D4, D4
	VREV64.16       D5, D5
	VREV64.16       D6, D6
	VREV64.16       D7, D7

	VMOV.S16        r7, D3[0]                           ; input[7]
	VMOV.S16        r8, D3[1]                           ; input[6]
       
	VQDMLAL.S16     Q6, D4, D0[1]                       ; * temp2
	VQDMLAL.S16     Q7, D6, D0[1]

	VQDMLAL.S16     Q6, D5, D0[0]                       ; * temp1
	VQDMLAL.S16     Q7, D7, D0[0]                       

	VMOV.S16        D1[0], r7
	VMOV.S16        D1[1], r8

	VQRSHRN.S32     D12, Q6, #13
	VQRSHRN.S32     D13, Q7, #13
	ADD             r10, r10, #8
	VST1.S16        {D12, D13}, [r1]!

 
        CMP             r10, r2
        BLT             LOOP

        VST1.S16        {D1}, [r12]!

do_zero_filter_front_end
 
        LDMFD           r13!, {r4 - r12, r15} 
        ENDFUNC
        END


