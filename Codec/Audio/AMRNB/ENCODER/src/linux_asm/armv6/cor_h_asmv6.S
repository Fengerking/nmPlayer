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
        .section  .text
	.global   cor_h_asm
	.extern   Inv_sqrt1

cor_h_asm:
	
	STMFD    r13!, {r0 - r11,r14}
        SUB      r13, r13, #84        
        MOV      r6, r0              
        MOV      r5, r1              
        MOV      r4, r2              
                                    
        MOV      r9, r6             
        MOV      r1, #40                      
        LDR      r8, [r9], #4
        LDR      r7, [r9], #4        
        MOV      r0, #4             
        MOV      r2, #0             

ADD_LOOP1:                         

        SMLALD   r0, r2, r8, r8
        LDR      r8, [r9], #4
        SMLALD   r0, r2, r7, r7
        LDR      r7, [r9], #4
        SMLALD   r0, r2, r8, r8
        SUBS     r1, r1, #8 
        LDR      r8, [r9], #4
        SMLALD   r0, r2, r7, r7
        LDR      r7, [r9], #4
        BNE      ADD_LOOP1


        LDR      r7, =32767 
        MOV      r8, #0xc0000000    
        CMP      r2, #0

        BLT      L_SCAL1
        AND      r1, r0, r8
        ORRS     r2, r1, r2
        MVNNE    r0, #0x80000000
        BNE      L_SCAL2
        B        L_SCAL3

L_SCAL1:
        CMN      r2, #1
        BICEQS   r1, r8, r0
        MOVNE    r0, #0x80000000
        BNE      L_SCAL2

L_SCAL3:
        MOV      r0,r0,LSL #1

L_SCAL2:
        MOV      r1, r0, ASR #16           
        MOV      r8, #0x8000              
        CMP      r1, r7               
        MOV      r7, r13            
        BEQ      TEMP_LOOP      
        
L_COR_H_J:
        MOV      r0, r0, ASR #1          
        BL       Inv_sqrt1
        SSAT     r0, #16, r0, ASR #9 
        LDR      r1, =32440	
        MOV      r9, r6                
        MOV      r10, r13             
        SMULBB   r0, r0, r1
        MOV      r1, #40             
        SSAT     r3, #16, r0, ASR #15       
        LDR      r7, [r9], #4          
        MOV      r8, r8, LSR #10
        LDR      r0, [r9],#4         

INNER_LOOP1:                    
                                      
        SMULBB   r11, r7, r3  
        SMULBB   r12, r0, r3                                       
        SMULTB   r7, r7, r3          
        SMULTB   r0, r0, r3       
        QADD     r11, r11, r8
        QADD     r12, r12, r8
        QADD     r7, r7, r8
        QADD     r0, r0, r8
        SSAT     r11, #16, r11, ASR #6        
        SSAT     r12, #16, r12, ASR #6    
        SSAT     r7, #16, r7, ASR #6
        SSAT     r0, #16, r0, ASR #6
        STRH     r11, [r10], #2               
        STRH     r7, [r10], #2
        STRH     r12, [r10], #2
        LDR      r7, [r9], #4             
        STRH     r0, [r10], #2        
        LDR      r0, [r9], #4
        SUBS     r1, r1, #4              
        BNE      INNER_LOOP1      
        MOV      r8, r8, LSL #10

TEMP_LOOP:
        MOV      r7, r13                   
        MOV      r2, #0                    
        MOV      r0, #39                   
        MOV      r1, #40                   
        
        LDR      r10, [r7], #4              
        LDR      r11, [r7], #4                  
        
        ADD      r12, r0, r0, LSL #2         
        ADD      r12, r4, r12, LSL #4        
        ADD      r12, r12, r0, LSL #1           

L_COR_FOR:                     
        SMULBB   r3, r10, r10
        SMULTT   r10, r10, r10
        SMULBB   r9, r11, r11
        QDADD    r2, r2, r3              
        QADD     r3, r2, r8             
        QDADD    r2, r2, r10
        QADD     r10, r2, r8
        MOV      r3, r3, ASR #16      
        STRH     r3, [r12], #-82
        SMULTT   r11, r11, r11
        MOV      r10, r10, ASR #16
        STRH     r10, [r12], #-82
        QDADD    r2, r2, r9
        QADD     r9, r2, r8
        QDADD    r2, r2, r11
        LDR      r10, [r7], #4          
        QADD     r11, r2, r8
        MOV      r9, r9, ASR #16
        STRH     r9, [r12], #-82
        MOV      r11, r11, ASR #16
        SUBS     r1, r1, #4
        STRH     r11, [r12], #-82       
        LDR      r11, [r7], #4         
        BNE      L_COR_FOR
        MOV      r3, #2             
        MOV      r14, #0x8000       

INNER_LOOP5:
        MOV      r1, #39        
        SUB      r0, r1, r3, LSR #1  
        MOV      r2, r3, LSR #1
        RSB      r8, r2, #40     
        CMP      r8, #0
        BLE      COR_H_END      
        
        LDR      r4, [r13, #92]      
        LDR      r5, [r13, #88]      
        MOV      r7, #80
        MOV      r11, #3120           
        SMLABB   r10, r0, r7, r4
        ADD      r11, r11, r4
        ADD      r11, r11, r0, LSL #1     
        MOV      r9, r13                
        ADD      r10, r10, r1, LSL #1          
        ADD      r0, r5, r0, LSL #1          
        ADD      r1, r5, r1, LSL #1                
        LDRSH    r6, [r9, r3]               
        LDRSH    r7, [r9], #2               
        LDRSH    r4, [r0], #-2              
        LDRSH    r5, [r1], #-2              
        MOV      r12, #0                   

INNER_LOOP3:
        SMULBB   r7, r7, r6
        SMULBB   r4, r4, r5                 
        QDADD    r12, r12, r7               
        QADD     r5, r12, r14       
        QADD     r4, r4, r4
        LDRSH    r6, [r9, r3]          
        SMULTT   r5, r5, r4             
        LDRSH    r7, [r9], #2               
        SUBS     r8, r8, #1                 
        LDRSH    r4, [r0], #-2              
        SSAT     r2, #16, r5, ASR #15
        LDRSH    r5, [r1], #-2              
        STRH     r2, [r11], #-82
        STRH     r2, [r10], #-82 
        BNE      INNER_LOOP3            

COR_H_END:
        ADD      r3, r3, #2 
        CMP      r3, #80
        BLT      INNER_LOOP5
        
        ADD      r13, r13, #84 
        LDMFD    r13!, {r0 - r11, r15}

        .END


