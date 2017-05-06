@**************************************************************
@* Copyright 2009 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@/***************************** Change History**************************
@;* 
@;*    DD/MMM/YYYY     Code Ver         Description             Author
@;*    -----------     --------     ------------------        -----------
@;*    06-21-2010        1.0        File imported from        Huaping Liu
@;*                                             
@;**********************************************************************/
        #include "voAMRNBEncID.h"
        .text   .align  4
	.globl  _cor_h_asm
	.globl  _Inv_sqrt1

_cor_h_asm:
	
	stmfd    r13!, {r0 - r11,r14}
        sub      r13, r13, #84        
        mov      r6, r0              
        mov      r5, r1              
        mov      r4, r2              
                                    
        mov      r9, r6             
        mov      r1, #40                      
        ldr      r8, [r9], #4
        ldr      r7, [r9], #4        
        mov      r0, #4             
        mov      r2, #0             

add_LOOP1:                         

        smlald   r0, r2, r8, r8
        ldr      r8, [r9], #4
        smlald   r0, r2, r7, r7
        ldr      r7, [r9], #4
        smlald   r0, r2, r8, r8
        subs     r1, r1, #8 
        ldr      r8, [r9], #4
        smlald   r0, r2, r7, r7
        ldr      r7, [r9], #4
        bne      add_LOOP1


        ldr      r7, Cvalue1 
        mov      r8, #0xc0000000    
        cmp      r2, #0

        blt      L_SCAL1
        and      r1, r0, r8
        orrs     r2, r1, r2
        mvnne    r0, #0x80000000
        bne      L_SCAL2
        b        L_SCAL3

L_SCAL1:
        cmn      r2, #1
        biceqs   r1, r8, r0
        movne    r0, #0x80000000
        bne      L_SCAL2

L_SCAL3:
        mov      r0,r0,lsl #1

L_SCAL2:
        mov      r1, r0, asr #16           
        mov      r8, #0x8000              
        cmp      r1, r7               
        mov      r7, r13            
        beq      TEMP_LOOP      
        
L_COR_H_J:
        mov      r0, r0, asr #1          
        bl       _Inv_sqrt1
        ssat     r0, #16, r0, asr #9 
        ldr      r1, Cvalue2	
        mov      r9, r6                
        mov      r10, r13             
        smulbb   r0, r0, r1
        mov      r1, #40             
        ssat     r3, #16, r0, asr #15       
        ldr      r7, [r9], #4          
        mov      r8, r8, LSR #10
        ldr      r0, [r9],#4         

INNER_LOOP1:                    
                                      
        smulbb   r11, r7, r3  
        smulbb   r12, r0, r3                                       
        smultb   r7, r7, r3          
        smultb   r0, r0, r3       
        qadd     r11, r11, r8
        qadd     r12, r12, r8
        qadd     r7, r7, r8
        qadd     r0, r0, r8
        ssat     r11, #16, r11, asr #6        
        ssat     r12, #16, r12, asr #6    
        ssat     r7, #16, r7, asr #6
        ssat     r0, #16, r0, asr #6
        strh     r11, [r10], #2               
        strh     r7, [r10], #2
        strh     r12, [r10], #2
        ldr      r7, [r9], #4             
        strh     r0, [r10], #2        
        ldr      r0, [r9], #4
        subs     r1, r1, #4              
        bne      INNER_LOOP1      
        mov      r8, r8, lsl #10

TEMP_LOOP:
        mov      r7, r13                   
        mov      r2, #0                    
        mov      r0, #39                   
        mov      r1, #40                   
        
        ldr      r10, [r7], #4              
        ldr      r11, [r7], #4                  
        
        add      r12, r0, r0, lsl #2         
        add      r12, r4, r12, lsl #4        
        add      r12, r12, r0, lsl #1           

L_COR_FOR:                     
        smulbb   r3, r10, r10
        smultt   r10, r10, r10
        smulbb   r9, r11, r11
        qdadd    r2, r2, r3              
        qadd     r3, r2, r8             
        qdadd    r2, r2, r10
        qadd     r10, r2, r8
        mov      r3, r3, asr #16      
        strh     r3, [r12], #-82
        smultt   r11, r11, r11
        mov      r10, r10, asr #16
        strh     r10, [r12], #-82
        qdadd    r2, r2, r9
        qadd     r9, r2, r8
        qdadd    r2, r2, r11
        ldr      r10, [r7], #4          
        qadd     r11, r2, r8
        mov      r9, r9, asr #16
        strh     r9, [r12], #-82
        mov      r11, r11, asr #16
        subs     r1, r1, #4
        strh     r11, [r12], #-82       
        ldr      r11, [r7], #4         
        bne      L_COR_FOR
        mov      r3, #2             
        mov      r14, #0x8000       

INNER_LOOP5:
        mov      r1, #39        
        sub      r0, r1, r3, LSR #1  
        mov      r2, r3, LSR #1
        rsb      r8, r2, #40     
        cmp      r8, #0
        ble      COR_H_END      
        
        ldr      r4, [r13, #92]      
        ldr      r5, [r13, #88]      
        mov      r7, #80
        mov      r11, #3120           
        smlabb   r10, r0, r7, r4
        add      r11, r11, r4
        add      r11, r11, r0, lsl #1     
        mov      r9, r13                
        add      r10, r10, r1, lsl #1          
        add      r0, r5, r0, lsl #1          
        add      r1, r5, r1, lsl #1                
        ldrsh    r6, [r9, r3]               
        ldrsh    r7, [r9], #2               
        ldrsh    r4, [r0], #-2              
        ldrsh    r5, [r1], #-2              
        mov      r12, #0                   

INNER_LOOP3:
        smulbb   r7, r7, r6
        smulbb   r4, r4, r5                 
        qdadd    r12, r12, r7               
        qadd     r5, r12, r14       
        qadd     r4, r4, r4
        ldrsh    r6, [r9, r3]          
        smultt   r5, r5, r4             
        ldrsh    r7, [r9], #2               
        subs     r8, r8, #1                 
        ldrsh    r4, [r0], #-2              
        ssat     r2, #16, r5, asr #15
        ldrsh    r5, [r1], #-2              
        strh     r2, [r11], #-82
        strh     r2, [r10], #-82 
        bne      INNER_LOOP3            

COR_H_END:
        add      r3, r3, #2 
        cmp      r3, #80
        blt      INNER_LOOP5
        
        add      r13, r13, #84 
        ldmfd    r13!, {r0 - r11, r15}

Cvalue1:
        .word    32767

Cvalue2:
        .word    32440

        @.END


