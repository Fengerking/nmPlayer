;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;*************************************************************** 

;void Syn_filt (
;    Word16 a[],
;    Word16 x[],
;    Word16 y[], 
;    Word16 mem[]  
;)
; r0 --- a[]
; r1 --- x[]
; r2 --- y[]
; r3 --- mem[]
;******************************************************************

        AREA    |.text|, CODE, READONLY
        EXPORT Syn_filt_2

Syn_filt_2 PROC

        STMFD   r13!, {r4 - r11, r14} 

        SUB     r13, r13, #116                   
        MOV     r6, r0                           
        MOV     r5, r1                           
        MOV     r4, r2 
        MOV     r8, #0

;memcpy(tmp, mem, 20);
        MOV     r0, r3  
        ADD     r1, r13, #8                      
        VLD1.S16    {D0, D1}, [r0]!
        LDRSH       r10, [r0], #2
        LDRSH       r11, [r0], #2
        VST1.S16    {D0, D1}, [r1]!
        STRH        r10, [r1], #2
        STRH        r11, [r1], #2	
                         
        ADD     r9, r13, #28                      ; yy = tmp +10

L10_4   
                          
        LDRSH   r12, [r6]                         ; tmpA = a[0]
        LDRSH   r10, [r5], #2                     ; get x[i]

        LDRSH   r11, [r9, #-2]                    ; yy[-1]
        LDRSH   r3,  [r6, #2]                     ; *(tmpA++)
        RSB     r3,  r3, #0
        SMULBB  r14, r10, r12                     ; s = x[i] * (*(tmpA++))
        
        LDRSH   r12, [r6, #4]                     
        LDRSH   r10, [r9, #-4]                    ; yy[-2]
        RSB     r12, r12, #0
        SMLABB  r14, r3, r11, r14                 ; s -=((*(tmpA++))* yy[-1]);
        
        LDRSH   r3,  [r6, #6]
        LDRSH   r11, [r9, #-6]  
        RSB     r3,  r3, #0        
        SMLABB  r14, r12, r10, r14
        
        LDRSH   r12, [r6, #8]
        LDRSH   r10, [r9, #-8]
        RSB     r12, r12, #0
        SMLABB  r14, r3, r11, r14
        
        LDRSH   r3,  [r6, #10]
        LDRSH   r11, [r9, #-10]
        RSB     r3,  r3, #0
        SMLABB  r14, r12, r10, r14
        
        LDRSH   r12, [r6, #12]
        LDRSH   r10, [r9, #-12]
        RSB     r12, r12, #0
        SMLABB  r14, r3, r11, r14
        
        LDRSH   r3,  [r6, #14]
        LDRSH   r11, [r9, #-14]
        RSB     r3,  r3, #0
        SMLABB  r14, r12, r10, r14
        
        LDRSH   r12, [r6, #16]
        LDRSH   r10, [r9, #-16]
        RSB     r12, r12, #0
        SMLABB  r14, r3, r11, r14
        
        LDRSH   r3,  [r6, #18]
        LDRSH   r11, [r9, #-18]
        RSB     r3,  r3, #0
        SMLABB  r14, r12, r10, r14
        
        LDRSH   r12, [r6, #20]
        LDRSH   r10, [r9, #-20]
        RSB     r12, r12, #0
        SMLABB  r14, r3, r11, r14
        
        SMLABB  r14, r12, r10, r14
        

        ;s = ASM_L_shl(s, 4)
        SSAT     r12, #32, r14, LSL #4
      
      
        MOV     r10, #0x8000
        QADD    r14, r12, r10
        ADD     r8, r8, #1
        MOV     r14, r14, ASR #16

        STRH    r14, [r9], #2
        STRH    r14, [r4], #2
        CMP     r8, #22
        BLT     L10_4


        ADD     r13, r13, #116        
        LDMFD   r13!, {r4 - r11, r15}  
   
        ENDP
        END


