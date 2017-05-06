@**************************************************************
@* Copyright 2003 ~ 2010 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver     Description             Author
@*    -----------     --------     -----------             ------
@*    08-12-2008        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************
        #include "voAMRNBEncID.h"
        .section  .text
	.global   Norm_corr_asm
	.extern   Convolve_asm
	.extern   Inv_sqrt1

Norm_corr_asm:

        STMFD   r13!, {r4-r11, r14}
        SUB     r13, r13, #108
        LDR     r4, [r13, #144]
        STR     r1, [r13, #80]                    
        STR     r2, [r13, #104]                   
        SUB     r4, r0, r4, LSL #1                
        SUB     r5, r3, #1
        STRH    r5, [r13, #84]                    
        MOV     r0, r3, ASR #1
        STRH    r0, [r13, #86] 

@===============================================================================
@    Convolve_asm (&exc[k], h, excf, L_subfr)
@===============================================================================
        MOV     r0, r4
        MOV     r1, r2
        ADD     r2, r13, #0
        BL      Convolve_asm
        
@===============================================================================
@    s = 0@  
@    for (j = 0@ j < L_subfr@ j++) {
@        s = L_mac (s, excf[j], excf[j])@
@    }
@===============================================================================
        LDRSH   r2, [r13, #86]                     
        ADD     r0, r13, #0
        LDR     r8, [r0], #4                       
        LDR     r14, [r0], #4
        MOV     r3, #0                            
        MOV     r9, #0                            
LOOP1:
        SMLALD  r3, r9, r8, r8
        LDR     r8, [r0], #4
        SMLALD  r3, r9, r14, r14      
        LDR     r14, [r0], #4
        SMLALD  r3, r9, r8, r8
	LDR     r8, [r0], #4
	SMLALD  r3, r9, r14, r14
	LDR     r14, [r0], #4
        SUBS    r2, r2, #4
        BNE     LOOP1

        MOV     r8,#0                         

        LDR     r7, [r13, #148]
        LDR     r6, [r13, #144]
        LDR     r5, [r13, #152]
        
        MOV     r14, #0xfe000000
        AND     r14, r3, r14
        ORRS    r9, r9, r14
        BEQ     INNER_LOOP1
        CMP     r3, #0x2000000
        BEQ     INNER_LOOP1 
        
Norm_Corr_Scale:

        MOV     r8, #2                           @r8=scaling
        LDRSH   r2, [r13, #86]                   @loop counter
        ADD     r3, r13, #0

Norm_Corr_Scale_loop:

        LDRSH   r0, [r3]
        LDRSH   r1, [r3, #2]
        SUBS    r2, r2, #1
        MOV     r0, r0, ASR #2
        MOV     r1, r1, ASR #2
        STRH    r0, [r3], #2
        STRH    r1, [r3], #2
        BNE     Norm_Corr_Scale_loop
        
INNER_LOOP1:

        SUBS    r7,r7,r6                       
        ADD     r5,r5,r6,LSL #1            
        BLE     L_LAST


L_CORR2:  
        ADD     r9, r13, #0
        LDRSH   r12, [r13, #84]
        LDR     r14, [r9], #4                  
        LDR     r3, [r13, #80]
        LDRSH   r10, [r4, #-2]!                
        SUB     r12, r12, #1                 
        
        LDR     r6, [r13, #104]                
        LDR     r2, [r3], #4                  
 
        STR     r4, [r13, #88]
        
        SUB     r6, r6, #2
        STR     r5, [r13, #92]
        STR     r7, [r13, #96]
        
        MOV     r0, #0
        MOV     r5, #0
        MOV     r11, #0        
        MOV     r7, #0
        STR     r8, [r13, #100]
        CMP     r8, #0
        BEQ     L_CORR21

@*********************************************************************************
ADD_LOOP:

        LDRH	  r4, [r6, #6]
	LDRH	  r1, [r6, #4]!
	ORR	  r4, r1, r4, LSL #16  
        SMLALD    r0, r5, r14, r14

        SMULBB    r1, r10, r4
        SMULBT    r8, r10, r4

        SMLALD    r11, r7, r2, r14

        LDR       r2, [r3], #4           
        SSAT      r1, #16, r1, ASR #14                               
        SSAT      r8, #16, r8, ASR #14           
                                            
        SUBS      r12, r12, #2                
        PKHBT     r1, r1, r8, LSL #16      
        QADD16    r1, r14, r1                                     
        LDR       r14, [r9], #4
        STRH      r1, [r9, #-6]             
        MOV       r1, r1, ASR #16            
        STRH      r1,[r9,#-4]

        BNE       ADD_LOOP
        
        B         L_CORR21_END
        
L_CORR21:

	LDRH	  r4, [r6, #6]
	LDRH	  r1, [r6, #4]!
	ORR	  r4, r1, r4, LSL #16
        SMLALD    r0, r5, r14, r14
        SMULBB    r1, r10, r4
        SMULBT    r8, r10, r4
        SMLALD    r11, r7, r2, r14
        LDR       r2, [r3], #4                  
        SSAT      r1, #16, r1, ASR #12           
        SSAT      r8, #16, r8, ASR #12          
                                                
        SUBS      r12, r12, #2                
        PKHBT     r1, r1, r8, LSL #16
        QADD16    r1, r14, r1                      
        LDR       r14, [r9], #4
        STRH      r1, [r9, #-6]                     
        MOV       r1, r1, ASR #16            
        STRH      r1, [r9, #-4]
        BNE       L_CORR21
        
L_CORR21_END:

        LDR       r8, [r13, #100]
        LDRSH     r4, [r6, #4]
        SMLALD    r0, r5, r14, r14    
        SMULBB    r1, r10, r4          
        SMLALD    r11, r7, r2, r14        
        CMP       r8, #0
        SSATEQ    r1, #16, r1, ASR #12
        SSATNE    r1, #16, r1, ASR #14   
        MOV       r2, #0xc0000000
        QADD16    r1, r14, r1
        CMP       r5, #0
        STRH      r1, [r9, #-2]
        BLT       L_CORR_NEG
        AND       r4, r0, r2
        ORRS      r5, r4, r5
        MVNNE     r0, #0x80000000
        BNE       L_SCAL_END
        B         L_SCAL1

L_CORR_NEG:      
        CMN       r5, #1
        BICEQS    r4, r2, r0
        MOVNE     r0,#0x80000000
        BNE       L_SCAL_END

L_SCAL1:     
        MOV       r0, r0, LSL #1

L_SCAL_END:  
        
        CMP       r7, #0
        BLT       L_SCAL2
        AND       r4, r11, r2
        ORRS      r7, r4, r7
        MVNNE     r11, #0x80000000
        BNE       L_SCAL2_END
        B         L_SCAL2_NORM

L_SCAL2:  

        CMN       r7, #1
        BICEQS    r4, r2, r11
        MOVNE     r11,#0x80000000
        BNE       L_SCAL2_END

L_SCAL2_NORM:     
        MOV       r11, r11, LSL #1

L_SCAL2_END:                  
        BL        Inv_sqrt1
        BIC       r11, r11, #1
        SMULWT    r2, r11, r0
        MOV       r0, r0, ASR #1
        BIC       r0, r0, #0x8000
        SMULBT    r0, r0, r11
        ADD       r0, r2, r0, ASR #15
        QADD      r2, r0, r0
        MOV       r0, #0x8000

        BIC       r2, r2, #1

        LDR       r4, [r13, #88]
        LDR       r5, [r13, #92]
        LDR       r7, [r13, #96]     
        CMP       r2, r0
        SUBGE     r2, r0, #1 
        CMN       r2, r0
        RSBLT     r2, r0, #0                       
        STRH      r2, [r5], #2                     

        MOV       r3, r10, ASR r8   
        SUBS      r7, r7, #1                        @loop2 counter
        STRH      r3, [r13, #0]
        BNE       L_CORR2

L_LAST:
        ADD       r9,r13,#0
        LDRSH     r12,[r13,#86]
        LDR       r3,[r13,#80]
        MOV       r0,#0
        MOV       r11,#0
        MOV       r2,#0
        MOV       r6, #0
        
L_NORM1:

        LDR     r14, [r9], #4                     
        LDR     r8, [r9], #4
        LDR     r1, [r3], #4                
        LDR     r4, [r3], #4

        SUBS    r12, r12, #2
        
        SMLALD  r0, r2, r14, r14
        SMLALD  r11, r6, r1, r14
        SMLALD  r0, r2, r8, r8
        SMLALD  r11, r6, r4, r8

        BNE     L_NORM1
        
        MOV     r14,#0xc0000000

        CMP     r2, #0
        BLT     INNER_LOOP3
        AND     r1, r0, r14
        ORRS    r2, r1, r2
        MVNNE   r0, #0x80000000
        BNE     INNER_LOOP4
        B       INNER_LOOP5

INNER_LOOP3:      
        CMN     r2, #1
        BICEQS  r1, r14, r0
        MOVNE   r0, #0x80000000
        BNE     INNER_LOOP4

INNER_LOOP5:     
        MOV     r0, r0, LSL #1

INNER_LOOP4:          
        CMP     r6, #0
        BLT     INNER_LOOP6
        AND     r1, r11, r14
        ORRS    r6, r1, r6
        MVNNE   r11, #0x80000000
        BNE     L_CORR4
        B       L_CORR3

INNER_LOOP6:      
        CMN     r6, #1
        BICEQS  r1, r14, r11
        MOVNE   r11, #0x80000000
        BNE     L_CORR4

L_CORR3:

        MOV     r11, r11, LSL #1

L_CORR4:
        BL      Inv_sqrt1

        BIC     r11, r11, #1
        SMULWT  r2, r11, r0
        MOV     r0, r0, ASR #1
        BIC     r0, r0, #0x8000
        SMULBT  r0, r0, r11
        ADD     r0, r2, r0, ASR #15
        QADD    r2, r0, r0
        MOV     r0, #0x8000
        BIC     r2, r2, #1

        CMP     r2, r0
        SUBGE   r2, r0, #1 
        CMN     r2, r0
        RSBLT   r2, r0, #0                       
        STRH    r2, [r5], #2                     

        ADD     r13, r13, #108
        LDMFD   r13!, {r4-r11, pc}
        
        .END

