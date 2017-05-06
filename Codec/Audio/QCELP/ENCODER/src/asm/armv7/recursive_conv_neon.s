;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;**************************************************************

;**********************************************************************
;  initial_recursive_conv  function
;**********************************************************************
;void initial_recursive_conv(
;		short   *resid,
;		short   length,
;		short   *impulse_response
;               )

	AREA	|.text|, CODE, READONLY

        EXPORT   initial_recursive_conv_asm
;******************************
; constant
;******************************


;******************************
; ARM register 
;******************************
; *resid                  RN           0
; length                  RN           1
; *impulse_response       RN           2

initial_recursive_conv_asm     FUNCTION

          STMFD      r13!, {r4 - r12, r14}
          SUB        r3, r1, #1                       ; i = length - 1
        
LOOP_I    
          ADD        r4, r0, r3, LSL #1               ; get resid[i] address  
          ADD        r8, r2, #2                       ; impulse_response + 1  
          ADD        r9, r4, #2                       ; outdata = &resid[i] + 1     
          LDRSH      r6, [r4]                         ; resid0 = resid[i]
          SUB        r5, r1, r3                       ; length - i
          CMP        r6, #0
          BEQ        LOOP_C
          CMP        r5, #20                          ; get the curindex value       
          MOVLE      r7, r5
          MOVGT      r7, #20
          SUB        r10, r7, #1                      ; j = curindex - 1  
          ;r6 --- resid0,  r8 --- indata, r9 --- outdata, r10 --- j
          LDR        r7, =0x2000
                 
LOOP_J
          LDRSH      r11, [r8], #2                    ; get *indata
          LDRSH      r12, [r9]                        ; get *outdta
          MUL        r5, r11, r6                      ; resid0 * (*indata)
          SUBS       r10, r10, #1
          ADD        r14, r5, r7
          MOV        r14, r14, ASR #14                ; (resid0 * (*indata) + 8192)>>14
          ADD        r14, r14, r12
          STRH       r14, [r9], #2                    
          BGT        LOOP_J

LOOP_C   
          SUBS       r3, r3, #1
          BGE        LOOP_I                  
          
initial_recursive_conv_end
 
          LDMFD      r13!, {r4 - r12, r15} 
          ENDFUNC
   


;void recursive_conv_10(
;		short   *resid,
;		short   *impulse_response,
;		short   length 
;		)


	AREA	|.text|, CODE, READONLY
        EXPORT   recursive_conv_10_Opt
        EXPORT   recursive_conv_40_Opt

;******************************
; constant
;******************************


;******************************
; ARM register 
;******************************
; *resid                  RN           0
; *impulse_response       RN           1
; length                  RN           2

recursive_conv_10_Opt    FUNCTION

          STMFD         r13!, {r4 - r12, r14}
          ADD           r5, r0, #2                         ;outdata = resid + 1;
          CMP           r2, #20
          MOVGT         r2, #20 
          LDRSH         r3,  [r0]                          ;get the resid[0]
          ADD           r4,  r1, #2                        ;indata = impulse_response + 1
          CMP           r3, #0
          BEQ           recursive_conv_10_end
          MOV           r6, r5
	  VMOV.S16      D0[0], r3

          VLD1.S16      {D2, D3, D4}, [r4]!
          VLD1.S16      {D6, D7, D8}, [r6]!

          VMULL.S16     Q8, D2, D0[0]
          VMULL.S16     Q9, D3, D0[0]
          VMULL.S16     Q10, D4, D0[0]

          VQRSHRN.S32   D16, Q8, #14
          VQRSHRN.S32   D17, Q9, #14
          VQRSHRN.S32   D18, Q10, #14

          VADD.S16      D6, D6, D16
          VADD.S16      D7, D7, D17
          VADD.S16      D8, D8, D18

          VST1.S16      {D6, D7}, [r5]!
          VST1.S16      D8[0], [r5]!
          VST1.S16      D8[1], [r5]!	  
 
recursive_conv_10_end
          LDMFD         r13!, {r4 - r12, r15} 

          ENDFUNC
    
recursive_conv_40_Opt    FUNCTION

          STMFD         r13!, {r4 - r12, r14}
          ADD           r5, r0, #2                         ;outdata = resid + 1;
          CMP           r2, #20
          MOVGT         r2, #20 
          LDRSH         r3,  [r0]                          ;get the resid[0]
          ADD           r4,  r1, #2                        ;indata = impulse_response + 1
          CMP           r3, #0
          BEQ           recursive_conv_40_end

          MOV           r6, r5
	  VMOV.S16      D0[0], r3

          VLD1.S16      {D2, D3, D4, D5}, [r4]!
	  VLD1.S16      {D6}, [r4]!
          VLD1.S16      {D8, D9, D10, D11}, [r6]!
          VLD1.S16      {D12}, [r6]!

          VMULL.S16     Q8, D2, D0[0]
          VMULL.S16     Q9, D3, D0[0]
          VMULL.S16     Q10, D4, D0[0]
	  VMULL.S16     Q11, D5, D0[0]
	  VMULL.S16     Q12, D6, D0[0]

          VQRSHRN.S32   D16, Q8, #14
          VQRSHRN.S32   D17, Q9, #14
          VQRSHRN.S32   D18, Q10, #14
	  VQRSHRN.S32   D19, Q11, #14
	  VQRSHRN.S32   D20, Q12, #14

          VADD.S16      D8, D8, D16
          VADD.S16      D9, D9, D17
          VADD.S16      D10, D10, D18
	  VADD.S16      D11, D11, D19
	  VADD.S16      D12, D12, D20

          VST1.S16      {D8, D9, D10, D11}, [r5]!
          VST1.S16      {D12}, [r5]!	 
           
recursive_conv_40_end
          LDMFD      r13!, {r4 - r12, r15} 
          ENDFUNC

          END


