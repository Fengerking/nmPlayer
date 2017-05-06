@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
       #include "voAMRNBDecID.h"
       .text    
       .align 4
       .globl   _myMemCopy

_myMemCopy:
        stmfd      r13!, {r4-r6,r10-r12, r14}               
        cmp        r0,#0
        beq        Lable8
        cmp        r2,#3   		
        ble        Lable6
        and	   r6,r0,#3
        and	   r5,r1,#3
        cmp	   r6,r5			 
        bne	   Lable6
        cmp	   r6,#0				
        beq	   Lable2
	rsb        r6,r6,#4
	rsb        r5,r5,#4
Lable1:
        ldrb       r12,[r1],#1			 
        subs       r6,r6,#1
        strb	   r12,[r0],#1
        bne	   Lable1	
Lable2:
        mov	   r4,#12   	        	
        sub	   r2,r2,r5
        cmp	   r2,r4
        blt	   Lable4
Lable3:
        ldr	   r12,[r1],#4           	
        add	   r4,r4,#12    
        ldr        r11,[r1],#4  
        ldr        r10,[r1],#4 
        str	   r12,[r0],#4  
        cmp	   r4,r2
        str        r11,[r0],#4  
        str        r10,[r0],#4  
        blt	   Lable3
Lable4:
        sub	   r4,r4,#12             	
        cmp	   r4,r2
        beq	   Lable8
Lable5:
        ldrb	   r12,[r1],#1
        add	   r4,r4,#1
        cmp	   r4,r2
        strb	   r12,[r0],#1	
        beq	   Lable8
        b	   Lable5
Lable6:
        movs       r4,r2			
Lable7:
        ldrb       r12,[r1],#1
        beq        Lable8
        subs       r4,r4,#1
        strb       r12,[r0],#1
        bne        Lable7
Lable8:
        ldmfd      r13!, {r4-r6,r10-r12, r15} 
        @.end
              

