;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;void ex_ltp(Word16 exc_enhanced[],
;            Word16 code[], 
;            Word16 *ptr,
;            Word16 pitch_fac, 
;            Word16 gain_code,
;            Word16 tmp_shift)

	AREA	|.text|, CODE, READONLY
        EXPORT  ex_ltp 

;******************************
; ARM register 
;******************************
; exc_enhanced[]        RN         0
; code[]                RN         1
; *ptr[]                RN         2
; pitch_fac             RN         3
; gain_code             RN         4
; tmp_shift             RN         5

ex_ltp    PROC
        STMFD            r13!, {r4 - r12, r14}  
        LDR               r4, [r13, #40] 
        LDR               r5, [r13, #44]
        MOV               r8, #40
        MOV               r6, r2 
        
        VLD1.S16          {D2, D3, D4, D5}, [r6]!
        VLD1.S16          {D6, D7, D8, D9}, [r6]!
        VLD1.S16          {D10, D11}, [r6]!

        VLD1.S16          {D12, D13, D14, D15}, [r1]!
        VLD1.S16          {D16, D17, D18, D19}, [r1]!
        VLD1.S16          {D20, D21}, [r1]!
   
        VST1.S16          {D2, D3, D4, D5}, [r0]! 
        VST1.S16          {D6, D7, D8, D9}, [r0]!
        VST1.S16          {D10, D11}, [r0]!

        
        VMOV.S16           D0[0], r3  
        VMOV.S16           D0[1], r4 
        VDUP.S32           Q14, r5  
        VMOV.S32           Q13, #0x8000


        VQDMULL.S16        Q11, D2, D0[0]                      
        VQDMLAL.S16        Q11, D12, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 
 
        VQDMULL.S16        Q11, D3, D0[0]                      
        VQDMLAL.S16        Q11, D13, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

        VQDMULL.S16        Q11, D4, D0[0]                      
        VQDMLAL.S16        Q11, D14, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

        VQDMULL.S16        Q11, D5, D0[0]                      
        VQDMLAL.S16        Q11, D15, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

        VQDMULL.S16        Q11, D6, D0[0]                      
        VQDMLAL.S16        Q11, D16, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

        VQDMULL.S16        Q11, D7, D0[0]                      
        VQDMLAL.S16        Q11, D17, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

        VQDMULL.S16        Q11, D8, D0[0]                      
        VQDMLAL.S16        Q11, D18, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]!  

        VQDMULL.S16        Q11, D9, D0[0]                      
        VQDMLAL.S16        Q11, D19, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

        VQDMULL.S16        Q11, D10, D0[0]                      
        VQDMLAL.S16        Q11, D20, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]!    

        VQDMULL.S16        Q11, D11, D0[0]                      
        VQDMLAL.S16        Q11, D21, D0[1]
        VQSHL.S32          Q11, Q11, Q14
        VQADD.S32          Q11, Q11, Q13
        VSHRN.S32          D24, Q11, #16        
        VST1.S16           D24, [r2]! 

ex_ltp_end 
 
        LDMFD              r13!, {r4 - r12, r15}
        ENDP
        END
