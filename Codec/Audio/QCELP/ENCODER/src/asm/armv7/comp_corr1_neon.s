;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;void comp_corr( 
;			   short *scal_sig,   /* i   : scaled signal.                          */
;			   short *target,
;			   short L_frame,     /* i   : length of frame to compute pitch        */
;			   short lag_max,     /* i   : maximum lag                             */
;			   short lag_min,     /* i   : minimum lag                             */
;			   int	*corr)       /* o   : correlation of selected lag             */


;******************************
; constant
;******************************


;*************************************************
; ARM register 
;*************************************************
;p_max1 = comp_corr1(cb_out_shifted, indata1, 10, CBLENGTH);

        AREA    |.text|, CODE, READONLY
	EXPORT  comp_corr10
	EXPORT  comp_corr40
	EXPORT  comp_corr40_pitch


comp_corr10  PROC	
        STMFD         r13!, {r4 - r12, r14}  
        MOV           r6,  #0x80000000
        ;for(i = lag_max; i >= lag_min; i--)
	VMOV.S16      D10, #0
        VLD1.S16      {D0, D1}, [r1]!
	VLD1.S16      {D2}, [r1]!                    ; load 10 p1
	VEXT.16       D2, D10, D2, #2

LOOP10
        RSB           r5, r3, #0                     ; -i
        ADD           r7, r0, r5, LSL #1             ; p1 = &scal_sig[-i]

        VLD1.S16      {D4, D5}, [r7]!
	VLD1.S16      {D6}, [r7]!
	VEXT.16       D6, D10, D6, #2
        VMULL.S16     Q4, D0, D4
	VMLAL.S16     Q4, D1, D5
	VMLAL.S16     Q4, D2, D6
	VADD.S32      D8, D8, D9
	VPADD.S32     D8, D8, D8
	VMOV.S32      r14, D8[0]
      
        CMP           r14,  r6
        MOVGT         r6,  r14
        MOVGT         r12, r3 
        SUB           r3, r3, #1
        CMP           r3, #1
        BGE           LOOP10

comp_corr10_end
                        
        MOV           r0, r12
        LDMFD         r13!, {r4 - r12, r15} 
        ENDP



comp_corr40  PROC	
        STMFD         r13!, {r4 - r12, r14}  
        MOV           r6,  #0x80000000
        ;for(i = lag_max; i >= lag_min; i--)
        VLD1.S16      {D0, D1, D2, D3}, [r1]!
	VLD1.S16      {D4, D5, D6, D7}, [r1]!                    ; load 32 p1
	VLD1.S16      {D8, D9}, [r1]!

LOOP40
        RSB           r5, r3, #0                     ; -i
        ADD           r7, r0, r5, LSL #1             ; p1 = &scal_sig[-i]

        VLD1.S16      {D10, D11, D12, D13}, [r7]!
	VLD1.S16      {D14, D15, D16, D17}, [r7]!
        VLD1.S16     {D18, D19}, [r7]!
   
        VMULL.S16     Q12, D0, D10
        VMLAL.S16     Q12, D1, D11
        VMLAL.S16     Q12, D2, D12
        VMLAL.S16     Q12, D3, D13
	VMLAL.S16     Q12, D4, D14
	VMLAL.S16     Q12, D5, D15
	VMLAL.S16     Q12, D6, D16
	VMLAL.S16     Q12, D7, D17
	VMLAL.S16     Q12, D8, D18
	VMLAL.S16     Q12, D9, D19
	VADD.S32      D24, D24, D25
	VPADD.S32     D24, D24, D24
	VMOV          r14, D24[0]
      
        CMP           r14,  r6
        MOVGT         r6,  r14
        MOVGT         r12, r3 
        SUB           r3, r3, #1
        CMP           r3, #1
        BGE           LOOP40

comp_corr40_end
                        
        MOV           r0, r12
        LDMFD         r13!, {r4 - r12, r15} 
        ENDP

comp_corr40_pitch  PROC

        STMFD         r13!, {r4 - r12, r14}  
        ;for(i = lag_max; i >= lag_min; i--)
        VLD1.S16      {D0, D1, D2, D3}, [r1]!
	VLD1.S16      {D4, D5, D6, D7}, [r1]!                    ; load 40 p1
	VLD1.S16      {D8, D9}, [r1]!
        
	LDR           r4, [sp, #40]
	LDR           r8, [sp, #44]

LOOP40P
        RSB           r5, r3, #0                     ; -i
        ADD           r7, r0, r5, LSL #1             ; p1 = &scal_sig[-i]
	ADD           r9, r8, r5, LSL #2

        VLD1.S16      {D10, D11, D12, D13}, [r7]!
	VLD1.S16      {D14, D15, D16, D17}, [r7]!
        VLD1.S16      {D18, D19}, [r7]!
   
        VMULL.S16     Q12, D0, D10
        VMLAL.S16     Q12, D1, D11
        VMLAL.S16     Q12, D2, D12
        VMLAL.S16     Q12, D3, D13
	VMLAL.S16     Q12, D4, D14
	VMLAL.S16     Q12, D5, D15
	VMLAL.S16     Q12, D6, D16
	VMLAL.S16     Q12, D7, D17
	VMLAL.S16     Q12, D8, D18
	VMLAL.S16     Q12, D9, D19
	VADD.S32      D24, D24, D25
	VPADD.S32     D24, D24, D24
	VMOV          r14, D24[0]
      
        SUB           r3, r3, #1
        CMP           r3, r4
	STR           r14, [r9]

        BGE           LOOP40P

comp_corr40P_end
                        
        MOV           r0, r12
        LDMFD         r13!, {r4 - r12, r15} 
        ENDP
        END
