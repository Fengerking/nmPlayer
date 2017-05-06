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
        .text   .align 4
	.globl   _Norm_corr_asm
	.globl   _Convolve_asm
	.globl   _Inv_sqrt1

_Norm_corr_asm:

        stmfd   r13!, {r4-r11, r14}
        sub     r13, r13, #108
        ldr     r4, [r13, #144]
        str     r1, [r13, #80]                    
        str     r2, [r13, #104]                   
        sub     r4, r0, r4, lsl #1                
        sub     r5, r3, #1
        strh    r5, [r13, #84]                    
        mov     r0, r3, asr #1
        strh    r0, [r13, #86] 

@===============================================================================
@    Convolve_asm (&exc[k], h, excf, L_subfr)
@===============================================================================
        mov     r0, r4
        mov     r1, r2
        add     r2, r13, #0
        bl      _Convolve_asm
        
@===============================================================================
@    s = 0@  
@    for (j = 0@ j < L_subfr@ j++) {
@        s = L_mac (s, excf[j], excf[j])@
@    }
@===============================================================================
        ldrsh   r2, [r13, #86]                     
        add     r0, r13, #0
        ldr     r8, [r0], #4                       
        ldr     r14, [r0], #4
        mov     r3, #0                            
        mov     r9, #0                            
LOOP1:
        smlald  r3, r9, r8, r8
        ldr     r8, [r0], #4
        smlald  r3, r9, r14, r14      
        ldr     r14, [r0], #4
        smlald  r3, r9, r8, r8
	ldr     r8, [r0], #4
	smlald  r3, r9, r14, r14
	ldr     r14, [r0], #4
        subs    r2, r2, #4
        bne     LOOP1

        mov     r8,#0                         

        ldr     r7, [r13, #148]
        ldr     r6, [r13, #144]
        ldr     r5, [r13, #152]
        
        mov     r14, #0xfe000000
        and     r14, r3, r14
        orrs    r9, r9, r14
        beq     INNER_LOOP1
        cmp     r3, #0x2000000
        beq     INNER_LOOP1 
        
Norm_Corr_Scale:

        mov     r8, #2                           @r8=scaling
        ldrsh   r2, [r13, #86]                   @loop counter
        add     r3, r13, #0

Norm_Corr_Scale_loop:

        ldrsh   r0, [r3]
        ldrsh   r1, [r3, #2]
        subs    r2, r2, #1
        mov     r0, r0, asr #2
        mov     r1, r1, asr #2
        strh    r0, [r3], #2
        strh    r1, [r3], #2
        bne     Norm_Corr_Scale_loop
        
INNER_LOOP1:

        subs    r7,r7,r6                       
        add     r5,r5,r6,lsl #1            
        ble     L_LAST


L_Corr2:  
        add     r9, r13, #0
        ldrsh   r12, [r13, #84]
        ldr     r14, [r9], #4                  
        ldr     r3, [r13, #80]
        ldrsh   r10, [r4, #-2]!                
        sub     r12, r12, #1                 
        
        ldr     r6, [r13, #104]                
        ldr     r2, [r3], #4                  
 
        str     r4, [r13, #88]
        
        sub     r6, r6, #2
        str     r5, [r13, #92]
        str     r7, [r13, #96]
        
        mov     r0, #0
        mov     r5, #0
        mov     r11, #0        
        mov     r7, #0
        str     r8, [r13, #100]
        cmp     r8, #0
        beq     L_Corr21

@*********************************************************************************
add_LOOP:

        ldrh	  r4, [r6, #6]
	ldrh	  r1, [r6, #4]!
	orr	  r4, r1, r4, lsl #16  
        smlald    r0, r5, r14, r14

        smulbb    r1, r10, r4
        smulbt    r8, r10, r4

        smlald    r11, r7, r2, r14

        ldr       r2, [r3], #4           
        ssat      r1, #16, r1, asr #14                               
        ssat      r8, #16, r8, asr #14           
                                            
        subs      r12, r12, #2                
        pkhbt     r1, r1, r8, lsl #16      
        qadd16    r1, r14, r1                                     
        ldr       r14, [r9], #4
        strh      r1, [r9, #-6]             
        mov       r1, r1, asr #16            
        strh      r1,[r9,#-4]

        bne       add_LOOP
        
        b         L_Corr21_END
        
L_Corr21:

	ldrh	  r4, [r6, #6]
	ldrh	  r1, [r6, #4]!
	orr	  r4, r1, r4, lsl #16
        smlald    r0, r5, r14, r14
        smulbb    r1, r10, r4
        smulbt    r8, r10, r4
        smlald    r11, r7, r2, r14
        ldr       r2, [r3], #4                  
        ssat      r1, #16, r1, asr #12           
        ssat      r8, #16, r8, asr #12          
                                                
        subs      r12, r12, #2                
        pkhbt     r1, r1, r8, lsl #16
        qadd16    r1, r14, r1                      
        ldr       r14, [r9], #4
        strh      r1, [r9, #-6]                     
        mov       r1, r1, asr #16            
        strh      r1, [r9, #-4]
        bne       L_Corr21
        
L_Corr21_END:

        ldr       r8, [r13, #100]
        ldrsh     r4, [r6, #4]
        smlald    r0, r5, r14, r14    
        smulbb    r1, r10, r4          
        smlald    r11, r7, r2, r14        
        cmp       r8, #0
        ssateq    r1, #16, r1, asr #12
        ssatne    r1, #16, r1, asr #14   
        mov       r2, #0xc0000000
        qadd16    r1, r14, r1
        cmp       r5, #0
        strh      r1, [r9, #-2]
        blt       L_Corr_NEG
        and       r4, r0, r2
        orrs      r5, r4, r5
        mvnne     r0, #0x80000000
        bne       L_SCAL_END
        b         L_SCAL1

L_Corr_NEG:      
        cmn       r5, #1
        biceqs    r4, r2, r0
        movne     r0,#0x80000000
        bne       L_SCAL_END

L_SCAL1:     
        mov       r0, r0, lsl #1

L_SCAL_END:  
        
        cmp       r7, #0
        blt       L_SCAL2
        and       r4, r11, r2
        orrs      r7, r4, r7
        mvnne     r11, #0x80000000
        bne       L_SCAL2_END
        b         L_SCAL2_NORM

L_SCAL2:  

        cmn       r7, #1
        biceqs    r4, r2, r11
        movne     r11,#0x80000000
        bne       L_SCAL2_END

L_SCAL2_NORM:     
        mov       r11, r11, lsl #1

L_SCAL2_END:                  
        bl        _Inv_sqrt1
        bic       r11, r11, #1
        smulwt    r2, r11, r0
        mov       r0, r0, asr #1
        bic       r0, r0, #0x8000
        smulbt    r0, r0, r11
        add       r0, r2, r0, asr #15
        qadd      r2, r0, r0
        mov       r0, #0x8000

        bic       r2, r2, #1

        ldr       r4, [r13, #88]
        ldr       r5, [r13, #92]
        ldr       r7, [r13, #96]     
        cmp       r2, r0
        subge     r2, r0, #1 
        cmn       r2, r0
        rsblt     r2, r0, #0                       
        strh      r2, [r5], #2                     

        mov       r3, r10, asr r8   
        subs      r7, r7, #1                        @loop2 counter
        strh      r3, [r13, #0]
        bne       L_Corr2

L_LAST:
        add       r9,r13,#0
        ldrsh     r12,[r13,#86]
        ldr       r3,[r13,#80]
        mov       r0,#0
        mov       r11,#0
        mov       r2,#0
        mov       r6, #0
        
L_NORM1:

        ldr     r14, [r9], #4                     
        ldr     r8, [r9], #4
        ldr     r1, [r3], #4                
        ldr     r4, [r3], #4

        subs    r12, r12, #2
        
        smlald  r0, r2, r14, r14
        smlald  r11, r6, r1, r14
        smlald  r0, r2, r8, r8
        smlald  r11, r6, r4, r8

        bne     L_NORM1
        
        mov     r14,#0xc0000000

        cmp     r2, #0
        blt     INNER_LOOP3
        and     r1, r0, r14
        orrs    r2, r1, r2
        mvnne   r0, #0x80000000
        bne     INNER_LOOP4
        b       INNER_LOOP5

INNER_LOOP3:      
        cmn     r2, #1
        biceqs  r1, r14, r0
        movne   r0, #0x80000000
        bne     INNER_LOOP4

INNER_LOOP5:     
        mov     r0, r0, lsl #1

INNER_LOOP4:          
        cmp     r6, #0
        blt     INNER_LOOP6
        and     r1, r11, r14
        orrs    r6, r1, r6
        mvnne   r11, #0x80000000
        bne     L_Corr4
        b       L_Corr3

INNER_LOOP6:      
        cmn     r6, #1
        biceqs  r1, r14, r11
        movne   r11, #0x80000000
        bne     L_Corr4

L_Corr3:

        mov     r11, r11, lsl #1

L_Corr4:
        bl      _Inv_sqrt1

        bic     r11, r11, #1
        smulwt  r2, r11, r0
        mov     r0, r0, asr #1
        bic     r0, r0, #0x8000
        smulbt  r0, r0, r11
        add     r0, r2, r0, asr #15
        qadd    r2, r0, r0
        mov     r0, #0x8000
        bic     r2, r2, #1

        cmp     r2, r0
        subge   r2, r0, #1 
        cmn     r2, r0
        rsblt   r2, r0, #0                       
        strh    r2, [r5], #2                     

        add     r13, r13, #108
        ldmfd   r13!, {r4-r11, pc}
        
        @.END

