;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;vo_postProcessEnc_asm(
;		&exc[i_subfr],
;		code,
;		gain_code,
;		pitch_fac,
;		tempShift);
;{
;	for (i = 0; i < L_SUBFR; i++);
;	{
;		L_temp = L_mult (exc[i + i_subfr], pitch_fac);
;		L_temp = L_mac (L_temp, code[i], gain_code);
;		L_temp = L_shl (L_temp, tempShift);
;		exc[i + i_subfr] = round (L_temp);           
;	}
;}
	    AREA	|.text|, CODE, READONLY
        EXPORT  vo_postProcessEnc_asm 
         
vo_postProcessEnc_asm     FUNCTION

        STMFD          r13!, {r4 - r12, r14}  
        MOV            r8, #0x8000
        MOV            r7, r0                               ;copy &exc[i_subfr] address
        VDUP.S32       Q12, r8    
        
        VLD1.S16       {D0, D1, D2, D3}, [r7]!
        VLD1.S16       {D4, D5, D6, D7}, [r7]!
        VLD1.S16       {D8, D9}, [r7]!

        VDUP.S16       D22, r3                             ;tmp1 --> pitch_fac
        VDUP.S16       D23, r2                             ;tmp2 --> gain_code


        VLD1.S16       {D10, D11, D12, D13}, [r1]!
        VLD1.S16       {D14, D15, D16, D17}, [r1]!
        VLD1.S16       {D18, D19}, [r1]!

        VQDMULL.S16    Q10, D0, D22
        VQDMLAL.S16    Q10, D10, D23
        VQSHL.S32      Q10, Q10, #2
        VADDHN.S32     D0, Q10, Q12


        VQDMULL.S16    Q10, D1, D22
        VQDMLAL.S16    Q10, D11, D23
        VQSHL.S32      Q10, Q10, #2
        VADDHN.S32     D1, Q10, Q12


        VQDMULL.S16    Q10, D2, D22
        VQDMLAL.S16    Q10, D12, D23
        VQSHL.S32      Q10, Q10, #2
        VADDHN.S32     D2, Q10, Q12


        VQDMULL.S16    Q10, D3, D22
        VQDMLAL.S16    Q10, D13, D23
        VQSHL.S32      Q10, Q10, #2
        VADDHN.S32     D3, Q10, Q12


        VQDMULL.S16    Q10, D4, D22
        VQDMLAL.S16    Q10, D14, D23
        VQSHL.S32      Q10, Q10, #2
        VADDHN.S32     D4, Q10, Q12


        VQDMULL.S16    Q10, D5, D22
        VQDMLAL.S16    Q10, D15, D23
        VQSHL.S32      Q10, Q10, #2
        VADDHN.S32     D5, Q10, Q12


        VQDMULL.S16    Q10, D6, D22
        VQDMLAL.S16    Q10, D16, D23
        VQSHL.S32      Q10, Q10, #2
        VADDHN.S32     D6, Q10, Q12


        VQDMULL.S16    Q10, D7, D22
        VQDMLAL.S16    Q10, D17, D23
        VQSHL.S32      Q10, Q10, #2
        VADDHN.S32     D7, Q10, Q12


        VQDMULL.S16    Q10, D8, D22
        VQDMLAL.S16    Q10, D18, D23
        VQSHL.S32      Q10, Q10, #2
        VADDHN.S32     D8, Q10, Q12


        VQDMULL.S16    Q10, D9, D22
        VQDMLAL.S16    Q10, D19, D23
        VQSHL.S32      Q10, Q10, #2
        VADDHN.S32     D9, Q10, Q12


        VST1.S16       {Q0, Q1}, [r0]!
        VST1.S16       {Q2, Q3}, [r0]!
        VST1.S16        Q4, [r0]!                  

vo_postProcessEnc_asm_end 
 
        LDMFD      r13!, {r4 - r12, r15}
    
        ENDFUNC
        END
