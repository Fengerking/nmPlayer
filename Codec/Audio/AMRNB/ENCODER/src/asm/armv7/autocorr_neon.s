;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;Word16 Autocorr2 (
;    Word16 x[],            /* (i)    : Input signal (L_WINDOW)            */
;    Word16 r_h[],          /* (o)    : Autocorrelations  (msb)            */
;    Word16 r_l[],          /* (o)    : Autocorrelations  (lsb)            */
;    const Word16 wind[]    /* (i)    : window for LPC analysis (L_WINDOW) */
;)

	AREA	|.text|, CODE, READONLY
        EXPORT  Autocorr_asm 

Autocorr_asm     PROC

        STMFD      r13!, {r4 - r12, r14}  
        SUB        r13,  r13, #488                     ;get the y[L_WINDOW] space
        MOV        r6,   r13                            ;store y[L_WINDOW] address
        MOV        r8,   #240                           ;L_WINDOW = 240      

; for(i = 0; i < L_WINDOW; i++)
;{
;   y[i] = mult_r(x[i], wind[i]);
;}
LOOP1
        VMOV.S32        Q10, #0x4000
        VMOV.S32        Q11, #0x4000
        VMOV.S32        Q12, #0x4000
        VMOV.S32        Q13, #0x4000
        VMOV.S32        Q14, #0x4000


        VLD1.S16       {D0, D1, D2, D3}, [r0]!                         ;load ten x[]
        VLD1.S16       {D4, D5, D6, D7}, [r0]!
        VLD1.S16       {D8, D9}, [r0]!
        
        VLD1.S16       {D10, D11, D12, D13}, [r3]!                     ; load ten wind[]
        VLD1.S16       {D14, D15, D16, D17}, [r3]!
        VLD1.S16       {D18, D19}, [r3]!
        
                            
        VMLAL.S16       Q10, D0, D10
        VMLAL.S16       Q11, D1, D11
        VMLAL.S16       Q12, D2, D12
        VMLAL.S16       Q13, D3, D13
        VMLAL.S16       Q14, D4, D14

        VSHRN.S32       D0, Q10, #15                                    ;get five y[i]
        VSHRN.S32       D1, Q11, #15
        VSHRN.S32       D2, Q12, #15
        VSHRN.S32       D3, Q13, #15
        VSHRN.S32       D4, Q14, #15
        
        VMLAL.S16       Q10, D5, D15
        VMLAL.S16       Q11, D6, D16
        VMLAL.S16       Q12, D7, D17
        VMLAL.S16       Q13, D8, D18
        VMLAL.S16       Q14, D9, D19
        
        VSHRN.S32       D5, Q10, #15
        VSHRN.S32       D6, Q11, #15
        VSHRN.S32       D7, Q12, #15
        VSHRN.S32       D8, Q13, #15
        VSHRN.S32       D9, Q14, #15
        
        VST1.S16        {D0, D1, D2, D3}, [r6]!
        VST1.S16        {D4, D5, D6, D7}, [r6]!
        VST1.S16        {D8, D9}, [r6]!
        SUBS            r8, r8, #40 
        BGT             LOOP1                            
        
; do 
; {
; }while(overf !=0)

LOOP2
        VMOV.S32        Q10,  #0                            ;sum = 0
        MOV             r6,  r13                           ;get y[i] first address
        MOV             r8,  #240                          ;L_WINDOW = 240 
        
LOOP3
        VLD1.S16        {D0, D1, D2, D3}, [r6]!
        VLD1.S16        {D4, D5, D6, D7}, [r6]!
        VLD1.S16        {D8, D9, D10, D11}, [r6]!
        VLD1.S16        {D12, D13, D14, D15}, [r6]!
        VLD1.S16        {D16, D17, D18, D19}, [r6]!
        
        VQDMLAL.S16     Q10, D0, D0
        VQDMLAL.S16     Q10, D1, D1
        VQDMLAL.S16     Q10, D2, D2
        VQDMLAL.S16     Q10, D3, D3
        VQDMLAL.S16     Q10, D4, D4
        VQDMLAL.S16     Q10, D5, D5
        VQDMLAL.S16     Q10, D6, D6
        VQDMLAL.S16     Q10, D7, D7
        VQDMLAL.S16     Q10, D8, D8
        VQDMLAL.S16     Q10, D9, D9
        VQDMLAL.S16     Q10, D10, D10
        VQDMLAL.S16     Q10, D11, D11
        VQDMLAL.S16     Q10, D12, D12
        VQDMLAL.S16     Q10, D13, D13
        VQDMLAL.S16     Q10, D14, D14
        VQDMLAL.S16     Q10, D15, D15
        VQDMLAL.S16     Q10, D16, D16
        VQDMLAL.S16     Q10, D17, D17
        VQDMLAL.S16     Q10, D18, D18
        VQDMLAL.S16     Q10, D19, D19
                
        SUBS            r8, r8, #80
        BGT             LOOP3 
        VQADD.S32       D20, D20, D21       
        
        VMOV.S32        r5, D20[0]
        VMOV.S32        r10, D20[1]

        QADD            r5, r5, r10                      ; r5 --- sum
  
        CMP             r5, #0x7fffffff                           ; if(sum == MAX_32)
        BNE             Lable

        MOV             r6, r13                           ; get y[i] first address
        MOV             r7, r13                           ; store the result
        MOV             r8, #240
LOOP4
        VLD1.S16        {D0, D1, D2, D3}, [r6]!
        VLD1.S16        {D4, D5, D6, D7}, [r6]!
        VLD1.S16        {D8, D9, D10, D11}, [r6]!
        VLD1.S16        {D12, D13, D14, D15}, [r6]!
        VLD1.S16        {D16, D17, D18, D19}, [r6]!
                
        VSHR.S16        Q0, Q0, #2
        VSHR.S16        Q1, Q1, #2                        ; y[i] >>= 2
        VSHR.S16        Q2, Q2, #2
        VSHR.S16        Q3, Q3, #2
        VSHR.S16        Q4, Q4, #2
        VSHR.S16        Q5, Q5, #2
        VSHR.S16        Q6, Q6, #2                        ; y[i] >>= 2
        VSHR.S16        Q7, Q7, #2
        VSHR.S16        Q8, Q8, #2
        VSHR.S16        Q9, Q9, #2
        
        VST1.S16        {Q0, Q1}, [r7]!
        VST1.S16        {Q2, Q3}, [r7]!
        VST1.S16        {Q4, Q5}, [r7]!
        VST1.S16        {Q6, Q7}, [r7]!
        VST1.S16        {Q8, Q9}, [r7]!
        
        SUBS            r8, r8, #80
        BGT             LOOP4
        B               LOOP2
        
Lable
        ADD             r5, r5, #1                        ; sum = L_add(sum, 1L)
        ;norm = norm_l (sum)
        CLZ             r9, r5  
        SUB             r4, r9, #1                             
        MOV             r5, r5, LSL r4                    ; sum <<=norm

        MOV             r10, r5, LSR #16
        STRH            r10, [r1], #2                     ; r_h[0] = sum >>16
        SUB             r11, r5, r10, LSL #16
        MOV             r11, r11, LSR #1
        STRH            r11, [r2], #2                     ;r_l[0]=(sum - (r_h[0]<<16))>>1
 
        MOV             r14, #240
        MOV             r7, #1                             ; i = 1
LOOP5                                                      ; for(i=1; i <=m; i++)
        MOV             r8, #0                             ; sum = 0;
        MOV             r6, r13                            ; get the y[] address
        SUB             r10, r14, r7                       ; r10 --- L_WINDOW-i
        ADD             r12, r6, r7, LSL #1                ; get the y[i]
        MOV             r11, #0                            ; j = 0
   
LOOP6                                                      ; for(j=0; j<L_WINDOW-i; j++)
        LDRSH           r4, [r6], #2                            ; y[j]
        LDRSH           r5, [r12], #2                           ; y[j+i]
        ADD             r11, r11, #1
        MLA             r8, r4, r5, r8                          ; sum += (y[j] * y[j+i])
        CMP             r11, r10
        BLT             LOOP6

        MOV             r8, r8, LSL r9                     ; sum <<=(norm +1)

        MOV             r4, r8, LSR #16                    ; r_h[i] = sum >>16;
        STRH            r4, [r1], #2
        SUB             r5, r8, r4, LSL #16
        MOV             r5, r5, LSR #1
        STRH            r5, [r2], #2

        ADD             r7, r7, #1
        CMP             r7, #10
        BLE             LOOP5
               
Autocorr_asm_end  

        ADD             r13, r13, #488      
        LDMFD           r13!,{r4 - r12, r15}
    
        ENDP
        END
