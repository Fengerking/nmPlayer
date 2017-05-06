@**************************************************************
@* Copyright 2003~2009 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@*void Convolve (
@*    Word16 x[],        /* (i)     : input vector                           */
@*    Word16 h[],        /* (i)     : impulse response                       */
@*    Word16 y[],        /* (o)     : output vector                          */
@*    Word16 L           /* (i)     : vector size                            */
@*)
@*************************************
@        ARM Register
@*************************************
@ r0  ---  x[]
@ r1  ---  h[]
@ r2  ---  y[]
@ r3  ---  L
        #include "voAMRNBEncID.h"        
        .text   .align 4 
        .globl   _Convolve_asm    

_Convolve_asm:

        stmfd      sp!, {r4-r11, r14}
        sub        r13, r13, #32
        str        r2, [r13, #20]
        str        r1, [r13, #28]
        str        r3, [r13, #24]
        mov        r7, #0
	ldrh	   r12, [r0, #2]                   @r12--->x[1]<<16+x[0]
	ldrh	   r4, [r0], #4
	orr	   r12, r4, r12, lsl #16     
        ldrh	   r14, [r0, #2]                   @r14--->x[3]<<16+x[2] 
	mov	   r4, #0
	ldrh	   r6, [r0], #4
	orr        r14, r6, r14, lsl #16 
        ldr        r2, [r1], #4                    @r2--->h[1]<<16+h[0]
	mov        r6, #0
        ldr        r3, [r1], #4                    @r3--->h[3]<<16+h[2]           
        str        r12, [r13, #8]
        str        r14, [r13, #12]
        smulbb     r4, r12, r2                     
        smlaldx    r6, r7, r12, r2                  
        mov        r8, #0
        mov        r9, #0
        mov        r10, #0
        mov        r11, #0        
        ssat       r4, #16, r4, ASR #12             
        str        r0, [r13, #16] 
        ldr        r0, [r13, #20]        
        mov        r6, r6, lsr #12 
        orr        r6, r6, r7, lsl #20
        ssat       r6, #16, r6
        strh       r4, [r0]  
        smlalbb    r8, r9, r12, r3          
        strh       r6, [r0, #2]                            
        smlaltt    r8, r9, r12, r2    
        smlaldx    r10, r11, r12, r3           
        smlalbb    r8, r9, r14, r2
        smlaldx    r10, r11, r14, r2
    
        mov        r8, r8,lsr #12
        orr        r8,r8,r9,lsl #20
        ssat       r8,#16,r8
        
        mov        r10, r10, lsr #12
        orr        r10, r10, r11, lsl #20
        ssat       r10, #16, r10      
        strh       r8, [r0,#4]        
        mov        r1, #40
        sub        r1, r1, #4     
        str        r1, [r13, #24]        
        strh       r10, [r0, #6]        
        ldr        r1, [r13, #28]         @r1 = &h[]
        add        r0, r0, #78
        str        r0, [r13, #20]         @r0 = &y[n]
        add        r1, r1, #72            @r1-->h[n]

LOOP1:
        ldr        r12,[r13,#8]
        ldr        r14,[r13,#12]      
        mov        r4,#0
        mov        r5,#0
        ldr        r2,[r1]
        ldr        r3,[r1,#4]
        mov        r10,#0
        mov        r11,#0   
        mov        r8,#0     
        smlaldx    r10,r11,r12,r3   
        mov        r9,#0
        smlaldx    r10,r11,r14,r2

        smlalbb    r8,r9,r12,r3      
        mov        r7,#0   
        smlaltt    r8,r9,r12,r2  
        mov        r6,#0       
        smlalbb    r8,r9,r14,r2      
        ldr        r3,[r1,#-4]     
    
        smlaldx    r6,r7,r12,r2      
        smlalbb    r4,r5,r12,r2
        smlaltt    r8,r9,r14,r3          
        smlaltt    r4,r5,r12,r3            
        ldr        r2,[r1,#-8]!                 
        smlaldx    r6,r7,r14,r3   
        smlalbb    r4,r5,r14,r3
        ldr        r0,[r13,#16]         @r0--->&x[i]        
      
        smlaltt    r4,r5,r14,r2
                
	ldrh	   r12,[r0,#2]
	ldrh	   r14,[r0],#4
	orr	   r12,r14,r12,lsl #16
	ldr        r14,[r13,#24]
        rsb        r14,r14,#40
         
        rsbs       r14,r14,#36             @r14-->i1--
        ble        LOOP2_END
                           
LOOP2:
        smlaldx    r6,r7,r12,r2
        smlaldx    r10,r11,r12,r3      
	ldrh	   r2,[r1]
	ldrh	   r3,[r1,#-2]!
	orr	   r2,r3,r2,lsl #16
        smlaldx    r4,r5,r12,r2
	ldrh	   r3,[r1,#6]
	ldrh	   r2,[r1,#4]
	orr	   r3,r2,r3,lsl #16
        smlaldx    r8,r9,r12,r3
        
	ldrh	   r12,[r0,#2]
	ldrh	   r2,[r0],#4
	orr	   r12,r2,r12,lsl #16         
        ldr        r2,[r1,#-2]!
        ldr        r3,[r1,#4]
        subs       r14,r14,#2      
        bne        LOOP2

LOOP2_END:
  
        smlalbb    r4,r5,r12,r2          
        smlaldx    r6,r7,r12,r2         
        smlalbb    r8,r9,r12,r3     
        mov        r4,r4,lsr #12
        orr        r4,r4,r5,lsl #20      
        ldr        r1,[r13,#20]
        ssat       r4,#16,r4        
        smlaldx    r10,r11,r12,r3       
        strh       r4,[r1,#-6]   
        smlaltt    r8,r9,r12,r2             
	ldrh       r12,[r0,#2]
	ldrh	   r4,[r0],#4
	orr	   r12,r4,r12,lsl #16
        mov        r6,r6,lsr #12
        orr        r6,r6,r7,lsl #20
        ssat       r6,#16,r6
        smlalbb    r8,r9,r12,r2
        strh       r6,[r1,#-4]       
        smlaldx    r10,r11,r12,r2  
        mov        r8,r8,lsr #12
        orr        r8,r8,r9,lsl #20
        ssat       r8,#16,r8
 
        mov        r10,r10,lsr #12
        orr        r10,r10,r11,lsl #20
        ssat       r10,#16,r10         
        strh       r8,[r1,#-2]       
        ldr        r4,[r13,#24]    
        strh       r10,[r1],#-8    
        str        r1,[r13,#20]
        ldr        r1,[r13,#28]
        subs       r4,r4,#4
 
        str        r4,[r13,#24]
        add        r1,r1,r4,lsl #1
        
        bne        LOOP1
            
LOOP1_END: 

        add        r13, r13, #32
        ldmfd      sp!, {r4-r11, r15}        
                
        @.END

