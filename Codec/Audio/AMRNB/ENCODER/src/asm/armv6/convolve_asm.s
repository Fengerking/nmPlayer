;**************************************************************
;* Copyright 2003~2009 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;*void Convolve (
;*    Word16 x[],        /* (i)     : input vector                           */
;*    Word16 h[],        /* (i)     : impulse response                       */
;*    Word16 y[],        /* (o)     : output vector                          */
;*    Word16 L           /* (i)     : vector size                            */
;*)
;*************************************
;        ARM Register
;*************************************
; r0  ---  x[]
; r1  ---  h[]
; r2  ---  y[]
; r3  ---  L
        
        AREA  |.text|, CODE, READONLY 
        EXPORT    Convolve_asm    

Convolve_asm

        STMFD      sp!, {r4-r11, r14}
        SUB        r13, r13, #32
        STR        r2, [r13, #20]
        STR        r1, [r13, #28]
        STR        r3, [r13, #24]
        MOV        r7, #0
	LDRH	   r12, [r0, #2]                   ;r12--->x[1]<<16+x[0]
	LDRH	   r4, [r0], #4
	ORR	   r12, r4, r12, LSL #16     
        LDRH	   r14, [r0, #2]                   ;r14--->x[3]<<16+x[2] 
	MOV	   r4, #0
	LDRH	   r6, [r0], #4
	ORR        r14, r6, r14, LSL #16 
        LDR        r2, [r1], #4                    ;r2--->h[1]<<16+h[0]
	MOV        r6, #0
        LDR        r3, [r1], #4                    ;r3--->h[3]<<16+h[2]           
        STR        r12, [r13, #8]
        STR        r14, [r13, #12]
        SMULBB     r4, r12, r2                     
        SMLALDX    r6, r7, r12, r2                  
        MOV        r8, #0
        MOV        r9, #0
        MOV        r10, #0
        MOV        r11, #0        
        SSAT       r4, #16, r4, ASR #12             
        STR        r0, [r13, #16] 
        LDR        r0, [r13, #20]        
        MOV        r6, r6, LSR #12 
        ORR        r6, r6, r7, LSL #20
        SSAT       r6, #16, r6
        STRH       r4, [r0]  
        SMLALBB    r8, r9, r12, r3          
        STRH       r6, [r0, #2]                            
        SMLALTT    r8, r9, r12, r2    
        SMLALDX    r10, r11, r12, r3           
        SMLALBB    r8, r9, r14, r2
        SMLALDX    r10, r11, r14, r2
    
        MOV        r8, r8,LSR #12
        ORR        r8,r8,r9,LSL #20
        SSAT       r8,#16,r8
        
        MOV        r10, r10, LSR #12
        ORR        r10, r10, r11, LSL #20
        SSAT       r10, #16, r10      
        STRH       r8, [r0,#4]        
        MOV        r1, #40
        SUB        r1, r1, #4     
        STR        r1, [r13, #24]        
        STRH       r10, [r0, #6]        
        LDR        r1, [r13, #28]         ;r1 = &h[]
        ADD        r0, r0, #78
        STR        r0, [r13, #20]         ;r0 = &y[n]
        ADD        r1, r1, #72            ;r1-->h[n]

LOOP1
        LDR        r12,[r13,#8]
        LDR        r14,[r13,#12]      
        MOV        r4,#0
        MOV        r5,#0
        LDR        r2,[r1]
        LDR        r3,[r1,#4]
        MOV        r10,#0
        MOV        r11,#0   
        MOV        r8,#0     
        SMLALDX    r10,r11,r12,r3   
        MOV        r9,#0
        SMLALDX    r10,r11,r14,r2

        SMLALBB    r8,r9,r12,r3      
        MOV        r7,#0   
        SMLALTT    r8,r9,r12,r2  
        MOV        r6,#0       
        SMLALBB    r8,r9,r14,r2      
        LDR        r3,[r1,#-4]     
    
        SMLALDX    r6,r7,r12,r2      
        SMLALBB    r4,r5,r12,r2
        SMLALTT    r8,r9,r14,r3          
        SMLALTT    r4,r5,r12,r3            
        LDR        r2,[r1,#-8]!                 
        SMLALDX    r6,r7,r14,r3   
        SMLALBB    r4,r5,r14,r3
        LDR        r0,[r13,#16]         ;r0--->&x[i]        
      
        SMLALTT    r4,r5,r14,r2
                
	LDRH	   r12,[r0,#2]
	LDRH	   r14,[r0],#4
	ORR	   r12,r14,r12,LSL #16
	LDR        r14,[r13,#24]
        RSB        r14,r14,#40
         
        RSBS       r14,r14,#36             ;r14-->i1--
        BLE        LOOP2_END
                           
LOOP2
        SMLALDX    r6,r7,r12,r2
        SMLALDX    r10,r11,r12,r3      
	LDRH	   r2,[r1]
	LDRH	   r3,[r1,#-2]!
	ORR	   r2,r3,r2,LSL #16
        SMLALDX    r4,r5,r12,r2
	LDRH	   r3,[r1,#6]
	LDRH	   r2,[r1,#4]
	ORR	   r3,r2,r3,LSL #16
        SMLALDX    r8,r9,r12,r3
        
	LDRH	   r12,[r0,#2]
	LDRH	   r2,[r0],#4
	ORR	   r12,r2,r12,LSL #16         
        LDR        r2,[r1,#-2]!
        LDR        r3,[r1,#4]
        SUBS       r14,r14,#2      
        BNE        LOOP2

LOOP2_END
  
        SMLALBB    r4,r5,r12,r2          
        SMLALDX    r6,r7,r12,r2         
        SMLALBB    r8,r9,r12,r3     
        MOV        r4,r4,LSR #12
        ORR        r4,r4,r5,LSL #20      
        LDR        r1,[r13,#20]
        SSAT       r4,#16,r4        
        SMLALDX    r10,r11,r12,r3       
        STRH       r4,[r1,#-6]   
        SMLALTT    r8,r9,r12,r2             
	LDRH       r12,[r0,#2]
	LDRH	   r4,[r0],#4
	ORR	   r12,r4,r12,LSL #16
        MOV        r6,r6,LSR #12
        ORR        r6,r6,r7,LSL #20
        SSAT       r6,#16,r6
        SMLALBB    r8,r9,r12,r2
        STRH       r6,[r1,#-4]       
        SMLALDX    r10,r11,r12,r2  
        MOV        r8,r8,LSR #12
        ORR        r8,r8,r9,LSL #20
        SSAT       r8,#16,r8
 
        MOV        r10,r10,LSR #12
        ORR        r10,r10,r11,LSL #20
        SSAT       r10,#16,r10         
        STRH       r8,[r1,#-2]       
        LDR        r4,[r13,#24]    
        STRH       r10,[r1],#-8    
        STR        r1,[r13,#20]
        LDR        r1,[r13,#28]
        SUBS       r4,r4,#4
 
        STR        r4,[r13,#24]
        ADD        r1,r1,r4,LSL #1
        
        BNE        LOOP1
            
LOOP1_END 

        ADD        r13, r13, #32
        LDMFD      sp!, {r4-r11, r15}
        ENDP                
        END

