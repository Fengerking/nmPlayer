;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************
        EXPORT myMemCopy

       AREA    |.text|, CODE, READONLY

myMemCopy PROC

        stmfd   r13!, {r4-r6,r10-r12, r14}               
        cmp        r0,#0
        beq        L110
        cmp        r2,#3   		
        ble        L18
        and	   r6,r0,#3
        and	   r5,r1,#3
        cmp	   r6,r5			 
        bne	   L18
        cmp	   r6,#0				
        beq	   L14
	rsb        r6,r6,#4
	rsb        r5,r5,#4
L13
        ldrb       r12,[r1],#1			 
        subs       r6,r6,#1
        strb	   r12,[r0],#1
        bne	   L13	
L14
        mov	   r4,#12   	        	
        sub	   r2,r2,r5
        cmp	   r2,r4
        blt	   L16
L15
        ldr	   r12,[r1],#4           	
        add	   r4,r4,#12    
        ldr        r11,[r1],#4  
        ldr        r10,[r1],#4 
        str	   r12,[r0],#4  
        cmp	   r4,r2
        str        r11,[r0],#4  
        str        r10,[r0],#4  
        blt	   L15
L16
        sub	   r4,r4,#12             	
        cmp	   r4,r2
        beq	   L110
L17
        ldrb	   r12,[r1],#1
        add	   r4,r4,#1
        cmp	   r4,r2
        strb	   r12,[r0],#1	
        beq	   L110
        b	   L17
L18
        movs       r4,r2			
L19
        ldrb       r12,[r1],#1
        beq        L110
        subs       r4,r4,#1
        strb       r12,[r0],#1
        bne        L19
L110
        ldmfd   r13!, {r4-r6,r10-r12, r15}     

        ENDP
        END          

