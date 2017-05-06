;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;*************************************************************** 
;void Filt_7k(
;     Word16 signal[],                      /* input:  signal                  */
;     Word16 lg,                            /* input:  length of input         */
;     Word16 mem[]                          /* in/out: memory (size=30)        */
;)
;******************************************************************
; r0    ---  signal[]
; r1    ---  lg
; r2    ---  mem[] 
 
          AREA |.text|, CODE, READONLY
          EXPORT Filt_7k_asm
          IMPORT fir_7k

Filt_7k_asm FUNCTION
          STMFD   		r13!, {r4 - r12, r14} 
          SUB    		r13, r13, #240              ; x[L_SUBFR16k + (L_FIR - 1)]
          MOV     		r8, r0                      ; copy signal[] address
          MOV     		r4, r1                      ; copy lg address
          MOV     		r5, r2                      ; copy mem[] address

          MOV     		r0, r2
          MOV     		r1, r13

	  VLD1.S16              {D0, D1, D2, D3}, [r0]!
	  VLD1.S16              {D4, D5, D6, D7}, [r0]!

	  VST1.S16              {D0, D1, D2, D3}, [r1]!
	  VST1.S16              {D4, D5, D6}, [r1]!
	  VST1.S16              D7[0], [r1]!
	  VST1.S16              D7[1], [r1]!

          LDR     		r10, Lable1                 ; get fir_7k address     

          ADD     		r6, r13, #60                ; get x[L_FIR - 1] address
          MOV     		r0, r8
          MOV     		r1, r6
	  VLD1.S16              {D0, D1, D2, D3}, [r0]!
	  VLD1.S16              {D4, D5, D6, D7}, [r0]!
	  VLD1.S16              {D8, D9, D10, D11}, [r0]!
	  VLD1.S16              {D12, D13, D14, D15}, [r0]!
	  VLD1.S16              {D16, D17, D18, D19}, [r0]!

	  VST1.S16              {D0, D1, D2, D3}, [r1]!
	  VST1.S16              {D4, D5, D6, D7}, [r1]!
	  VST1.S16              {D8, D9, D10, D11}, [r1]!
	  VST1.S16              {D12, D13, D14, D15}, [r1]!
	  VST1.S16              {D16, D17, D18, D19}, [r1]!

          STR     		r5, [sp, #-4]               ; PUSH  r5 to stack

          MOV                   r3, r8
          ; not use registers: r4, r10, r12, r14, r5
          MOV     		r4, r13 
          MOV     	        r5, #0                           ; i = 0
          ; r4 --- x[i], r10 ---- fir_7k
          VLD1.S16              {Q0, Q1}, [r10]!           ;fir_6k_7k[0]  ~ fir_6k_7k[15]
	  VLD1.S16              {Q2, Q3}, [r10]!           ;fir_6k_7k[16] ~ fir_6k_7k[31]
          VMOV.S16              D7[3], r5                        ;set fir_6k_7K = 0

	  VLD1.S16              {Q4, Q5}, [r4]!            ;x[0]  ~ x[15]
	  VLD1.S16              {Q6, Q7}, [r4]!            ;x[16] ~ X[31]
	  VLD1.S16              {Q8}, [r4]! 
          VMOV.S16              Q15, #0

LOOP_7K
          VMULL.S16             Q9, D8, D0[0]                
          VMULL.S16             Q10, D9, D1[0] 
          VMULL.S16             Q11, D9, D0[0]               
          VMULL.S16             Q12, D10, D1[0]
          VEXT.8                Q4, Q4, Q5, #2
          VMLAL.S16             Q9, D10, D2[0]
          VMLAL.S16             Q10, D11, D3[0]
          VMLAL.S16             Q11, D11, D2[0]
          VMLAL.S16             Q12, D12, D3[0]    
          VEXT.8                Q5, Q5, Q6, #2
          VMLAL.S16             Q9, D12, D4[0]
          VMLAL.S16             Q10, D13, D5[0]
          VMLAL.S16             Q11, D13, D4[0]
          VMLAL.S16             Q12, D14, D5[0]
          VEXT.8                Q6, Q6, Q7, #2
          VMLAL.S16             Q9, D14, D6[0]
          VMLAL.S16             Q10, D15, D7[0]
          VMLAL.S16             Q11, D15, D6[0]
	  VMLAL.S16             Q12, D16, D7[0]
	  VEXT.8  		Q7, Q7, Q8, #2 

	  VMLAL.S16 		Q9, D8, D0[1] 
	  VMLAL.S16		Q10, D9, D1[1]
	  VEXT.8 		Q8, Q8, Q15,#2 
	  VMLAL.S16 		Q11, D9, D0[1] 
	  VMLAL.S16 		Q12, D10, D1[1]
	  VEXT.8  		Q4, Q4, Q5, #2
	  VMLAL.S16 		Q9, D10, D2[1]
	  VMLAL.S16 		Q10, D11, D3[1]
	  VMLAL.S16 		Q11, D11, D2[1]
	  VMLAL.S16 		Q12, D12, D3[1]    
	  VEXT.8  		Q5, Q5, Q6, #2
	  VMLAL.S16 		Q9, D12, D4[1]
	  VMLAL.S16 		Q10, D13, D5[1]
	  VMLAL.S16 		Q11, D13, D4[1]
	  VMLAL.S16 		Q12, D14, D5[1]
	  VEXT.8  		Q6, Q6, Q7, #2
	  VMLAL.S16 		Q9, D14, D6[1]
	  VMLAL.S16 		Q10, D15, D7[1]
	  VMLAL.S16 		Q11, D15, D6[1]
	  VMLAL.S16 		Q12, D16, D7[1]
	  VEXT.8  		Q7, Q7, Q8, #2 

	  VMLAL.S16 		Q9, D8, D0[2] 
	  VMLAL.S16 		Q10, D9, D1[2]
	  VEXT.8 		Q8, Q8, Q15, #2 
	  VMLAL.S16 		Q11, D9, D0[2]
	  VMLAL.S16 		Q12, D10, D1[2]
	  VEXT.8  		Q4, Q4, Q5, #2
	  VMLAL.S16 		Q9, D10, D2[2]
	  VMLAL.S16 		Q10, D11, D3[2]
	  VMLAL.S16 		Q11, D11, D2[2]
	  VMLAL.S16 		Q12, D12, D3[2]    
	  VEXT.8  		Q5, Q5, Q6, #2
	  VMLAL.S16 		Q9, D12, D4[2]
	  VMLAL.S16 		Q10, D13, D5[2]
	  VMLAL.S16 		Q11, D13, D4[2]
	  VMLAL.S16 		Q12, D14, D5[2]
	  VEXT.8  		Q6, Q6, Q7, #2
	  VMLAL.S16 		Q9, D14, D6[2]
	  VMLAL.S16 		Q10, D15, D7[2]
	  VMLAL.S16 		Q11, D15, D6[2]
	  VMLAL.S16 		Q12, D16, D7[2]
	  VEXT.8  		Q7, Q7, Q8, #2 

	  VMLAL.S16 		Q9, D8, D0[3] 
	  VMLAL.S16 		Q10, D9, D1[3]
	  VEXT.8 		Q8, Q8, Q15,#2 
	  VMLAL.S16 		Q11, D9, D0[3] 
	  VMLAL.S16 		Q12, D10, D1[3]
	  VEXT.8  		Q4, Q4, Q5, #2
	  VMLAL.S16 		Q9, D10, D2[3]
	  VMLAL.S16 		Q10, D11, D3[3]
	  VMLAL.S16 		Q11, D11, D2[3]
	  VMLAL.S16 		Q12, D12, D3[3]    
	  VEXT.8  		Q5, Q5, Q6, #2
	  VMLAL.S16 		Q9, D12, D4[3]
	  VMLAL.S16 		Q10, D13, D5[3]
	  VMLAL.S16 		Q11, D13, D4[3]
	  VMLAL.S16 		Q12, D14, D5[3]
	  VEXT.8  		Q6, Q6, Q7, #2
	  VMLAL.S16 		Q9, D14, D6[3]
	  VMLAL.S16 		Q10, D15, D7[3]
	  VMLAL.S16 		Q11, D15, D6[3]
	  VMLAL.S16 		Q12, D16, D7[3]
	  VEXT.8 		Q7, Q7, Q8, #2     

	  VMOV.S16  		D8, D9
	  VEXT.8 		Q8, Q8, Q15,#2 
	  VMOV.S16  		D9, D10
	  VADD.S32  		Q9, Q9, Q10
	  VMOV.S16  		D10, D11
	  VMOV.S16  		D11, D12
	  VADD.S32  		Q11, Q11, Q12
	  VMOV.S16  		D12, D13
	  VQRSHRN.S32 		D28, Q9, #15
	  VMOV.S16  		D13, D14
	  VMOV.S16  		D14, D15
	  VQRSHRN.S32 		D29, Q11, #15
	  VMOV.S16  		D15, D16

	  VLD1.S16  		{Q8}, [r4]!
	  ADD                   r5, r5, #8
	  CMP   		r5, #80
	  VST1.S16  		{D28, D29}, [r3]!

	  BLT     		LOOP_7K	      
  
          ADD     		r0, r13, #160               ;x + lg	  
          LDR		     	r1, [sp, #-4]               ;mem address

	  VLD1.S16              {D0, D1, D2, D3}, [r0]!
	  VLD1.S16              {D4, D5, D6, D7}, [r0]!

	  VST1.S16              {D0, D1, D2, D3}, [r1]!
	  VST1.S16              {D4, D5, D6}, [r1]!
	  VST1.S16              D7[0], [r1]!
	  VST1.S16              D7[1], [r1]!
                    
Filt_7k_end
          ADD     		r13, r13, #240  		     
          LDMFD   		r13!, {r4 - r12, r15} 
 
Lable1
          DCD  		fir_7k
          ENDFUNC
          END

