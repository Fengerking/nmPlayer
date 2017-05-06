;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;*************************************************************** 

;void Syn_filt_1 (
;    Word16 a[],     /* (i)     : a[M+1] prediction coefficients   (M=10)  */
;    Word16 x[],     /* (i)     : input signal                             */
;    Word16 y[],     /* (o)     : output signal                            */
;    Word16 mem[]   /* (i/o)   : memory associated with this filtering.   * /    
;)
; r0 --- a[]
; r1 --- x[]
; r2 --- y[]
; r3 --- mem[]
;******************************************************************

        AREA    |.text|, CODE, READONLY
        EXPORT  Syn_filt_1
        IMPORT  myMemCopy

Syn_filt_1 PROC

        STMFD   r13!, {r4 - r11, r14} 
        SUB     r13, r13, #116                   
        MOV     r6, r0                           
        MOV     r5, r1                           
        MOV     r4, r2 
        MOV     r8, #0
;memcpy(tmp, mem, 20);
        ADD     r0, r13, #8
        STR     r3, [r13, #4]                 
        MOV     r2, #20                          
        MOV     r1, r3                          
        BL      myMemCopy 
                        
        ADD     r9, r13, #28                      ; yy = tmp +10
        
L10_4                           
        LDR     r12, [r6]                         ; tmpA = a[0], a[1]
        LDRSH   r10, [r5], #2                     ; get x[i]
        LDRSH   r11, [r9, #-2]                    ; yy[-1]        
        SMULBB  r14, r10, r12                     ; s = x[i] * a[0]

        LDRSH   r10, [r9, #-4]                    ; yy[-2]
        RSB     r11, r11, #0  
  
        LDR     r3,  [r6, #4]                     ; a[2], a[3]
        RSB     r10, r10, #0    
        
        SMLABT  r14, r11, r12, r14                ; s -= a[1] * yy[-1];

        LDRSH   r11, [r9, #-6]                    ; yy[-3] 
        SMLABB  r14, r10, r3, r14                 ; s -= a[2] * yy[-2] 

        RSB     r11, r11, #0
        LDRSH   r7,  [r9, #-8]                    ; yy[-4]   
        SMLABT  r14, r11, r3, r14                 ; s -= a[3] * yy[-3]    
        RSB     r7,  r7, #0
        
        LDR     r12, [r6, #8]                     ; a[4], a[5]
        LDRSH   r10, [r9, #-10]                   ; yy[-5]
        SMLABB  r14, r7, r12, r14                 ; s -= a[4] * yy[-4]
        RSB     r10, r10, #0
        LDRSH   r7, [r9, #-12]                    ; yy[-6]
        SMLABT  r14, r10, r12, r14                ; s -= a[5] * yy[-5]                
        RSB     r7, r7, #0
        LDR     r3, [r6, #12]                     ; a[6], a[7]
        LDRSH   r10, [r9, #-14]                   ; yy[-7]
        SMLABB  r14, r7, r3, r14                  ; s -= a[6] * yy[-6]
        RSB     r10, r10, #0      
        LDRSH   r7, [r9, #-16]                    ; yy[-8]
        SMLABT  r14, r10, r3, r14                 ; s -= a[7] * yy[-7]
        LDR     r12, [r6, #16]                    ; a[8], a[9]
        RSB     r7, r7, #0
        LDRSH   r10, [r9, #-18]                   ; yy[-9]
        SMLABB  r14, r7, r12, r14                 ; s -= a[8] * yy[-8]
        RSB     r10, r10, #0
        LDRSH   r7, [r9, #-20]                    ; yy[-10]
        SMLABT  r14, r10, r12, r14                ; s -= a[9] * yy[-9]
        LDRSH   r3, [r6, #20]                     ; a[10]
        RSB     r7, r7, #0
        SMLABB  r14, r7, r3, r14                  ; s -= a[10] * yy[-10]

        ;s = ASM_L_shl(s, 4)
        MOV     r10, r14
        MOV     r14, r10, LSL #4
        TEQ     r10, r14, ASR #4
        MOVNE   r11, #0x7fffffff
        EORNE   r14, r11, r10, ASR #31

        ;SSAT     r12, #32, r14, LSL #4

        MOV     r10, #0x8000
        QADD    r12, r14, r10
        ADD     r8, r8, #1
        MOV     r12, r12, ASR #16
        
        CMP     r8, #40
        STRH    r12, [r9], #2

        BLT     L10_4

        ADD     r1, r13, #28
        MOV     r2, #80                           
        MOV     r0, r4                           
        BL      myMemCopy 
                       
        LDR     r0, [r13, #4]
        ADD     r1, r4, #60
        MOV     r2, #20
        BL      myMemCopy 
        
  
        ADD     r13, r13, #116                    
        LDMFD   r13!, {r4 - r11, r15} 

        ENDP
        END 



