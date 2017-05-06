;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************

;*void Convolve (
;*    Word16 x[],        /* (i)     : input vector                           */
;*    Word16 h[],        /* (i)     : impulse response                       */
;*    Word16 y[],        /* (o)     : output vector                          */
;*    Word16 L           /* (i)     : vector size                            */
;*)

	AREA	|.text|, CODE, READONLY
        EXPORT Convolve_asm 

Convolve_asm     FUNCTION

        STMFD          r13!, {r4 - r12, r14}  
        MOV            r3,  #0                           ; n
        
LOOP 
        MOV            r8, #0                            ; s = 0
        ADD            r4, r1, r3, LSL #1                ; tmpH address
        ADD            r5, r3, #1                        ; i = n + 1
        MOV            r6, r0
        LDRSH          r9,  [r6], #2                     ; *tmpX++
        LDRSH          r10, [r4]                         ; *tmpH--
        SUB            r5, r5, #1
        VMOV.S32       Q10, #0 
        MLA            r8,  r9, r10, r8 

LOOP1                    
        CMP            r5, #0
        BLE            L1
        SUB            r4, r4, #8
        MOV            r9, r4
        VLD1.S16       D0, [r6]!   
        VLD1.S16       D1, [r9]!
        VREV64.16      D1, D1
        SUBS           r5, r5, #4  
        VMLAL.S16      Q10, D0, D1         
        B              LOOP1    
L1                  
        VADD.S32       D20, D20, D21
        VPADD.S32      D20, D20, D20
        VMOV.S32       r5, D20[0]
        ADD            r5, r5, r8
        MOV            r5, r5, LSL #4
        MOV            r5, r5, LSR #16                   ;extract_h(s)
        ADD            r3, r3, #1
        STRH           r5, [r2], #2                      ;y[n]


        MOV        r8, #0
        ADD            r4, r1, r3, LSL #1                ;tmpH address
        ADD            r5, r3, #1
        MOV            r6, r0
        LDRSH          r9,  [r6], #2                     ; *tmpX++
        LDRSH          r10, [r4], #-2                     
        LDRSH          r12, [r6], #2
        LDRSH          r14, [r4]

        MLA        r8, r9, r10, r8
        SUB            r5, r5, #2
        MLA            r8, r12, r14, r8
        
        VMOV.S32       Q10, #0
LOOP2
        CMP            r5, #0
        BLE            L2
        SUB            r4, r4, #8
        MOV            r9, r4
        VLD1.S16       D0, [r6]!   
        VLD1.S16       D1, [r9]!
        SUBS           r5, r5, #4
        VREV64.16      D1, D1
        VMLAL.S16      Q10, D0, D1 
        B              LOOP2
L2
        VADD.S32       D20, D20, D21
        VPADD.S32      D20, D20, D20
        VMOV.S32       r5, D20[0]
        ADD            r8, r8, r5
        MOV            r8, r8, LSL #4
        MOV            r8, r8, LSR #16                   ;extract_h(s)
        ADD            r3, r3, #1  
        STRH           r8, [r2], #2                      ;y[n]


        MOV            r8, #0
        ADD            r4, r1, r3, LSL #1
        ADD            r5, r3, #1
        MOV            r6, r0
        LDRSH          r9,  [r6], #2
        LDRSH          r10, [r4], #-2
        LDRSH          r12, [r6], #2
        LDRSH          r14, [r4], #-2
        MLA            r8, r9, r10, r8
        LDRSH          r9,  [r6], #2
        LDRSH          r10, [r4]
        MLA            r8, r12, r14, r8 
        SUB            r5, r5, #3
        MLA            r8, r9, r10, r8

        VMOV.S32       Q10, #0
LOOP3
        CMP            r5, #0
        BLE            L3
        SUB            r4, r4, #8
        MOV            r9, r4
        VLD1.S16       D0, [r6]!   
        VLD1.S16       D1, [r9]!
        VREV64.16      D1, D1
        SUBS           r5, r5, #4
        VMLAL.S16      Q10, D0, D1 
        B              LOOP3   

L3
        VADD.S32       D20, D20, D21
        VPADD.S32      D20, D20, D20
        VMOV.S32       r5, D20[0]
        ADD            r8, r8, r5
        MOV            r8, r8, LSL #4
        MOV            r8, r8, LSR #16                   ;extract_h(s)
        ADD            r3, r3, #1
        STRH           r8, [r2], #2                      ;y[n]

        ADD            r5, r3, #1                        ; i = n + 1
        ADD            r4, r1, r5, LSL #1                ; tmpH address
        MOV            r6, r0
        VMOV.S32       Q10, #0
LOOP4                    
        CMP            r5, #0
        BLE            L4
        SUB            r4, r4, #8
        MOV            r9, r4
        VLD1.S16       D0, [r6]!   
        VLD1.S16       D1, [r9]!
        VREV64.16      D1, D1
        SUBS           r5, r5, #4  
        VMLAL.S16      Q10, D0, D1         
        B              LOOP4    
L4                  
        VADD.S32       D20, D20, D21
        VPADD.S32      D20, D20, D20
        VMOV.S32       r5,  D20[0]
        MOV            r5, r5, LSL #4
        MOV            r5, r5, LSR #16                   ;extract_h(s)
        ADD            r3, r3, #1
        STRH           r5, [r2], #2                      ;y[n]
        
        CMP            r3, #40
        BLT            LOOP
                
Convolve_asm_end 
 
        LDMFD      r13!, {r4 - r12, r15}
    
        ENDFUNC
        END