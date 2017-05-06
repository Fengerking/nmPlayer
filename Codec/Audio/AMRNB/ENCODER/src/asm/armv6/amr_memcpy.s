;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;****************************************************************

       AREA     |.text|, CODE, READONLY
       EXPORT   myMemCopy

myMemCopy PROC

        stmfd      r13!, {r4-r6,r10-r12, r14}               
        cmp        r0,#0
        beq        L1_10
        cmp        r2,#3   		
        ble        L1_8
        and	   r6,r0,#3
        and	   r5,r1,#3
        cmp	   r6,r5			 
        bne	   L1_8
        cmp	   r6,#0				
        beq	   L1_4
	rsb        r6,r6,#4
	rsb        r5,r5,#4
L1_3
        ldrb       r12,[r1],#1			 
        subs       r6,r6,#1
        strb	   r12,[r0],#1
        bne	   L1_3	
L1_4
        mov	   r4,#12   	        	
        sub	   r2,r2,r5
        cmp	   r2,r4
        blt	   L1_6
L1_5
        ldr	   r12,[r1],#4           	
        add	   r4,r4,#12    
        ldr        r11,[r1],#4  
        ldr        r10,[r1],#4 
        str	   r12,[r0],#4  
        cmp	   r4,r2
        str        r11,[r0],#4  
        str        r10,[r0],#4  
        blt	   L1_5
L1_6
        sub	   r4,r4,#12             	
        cmp	   r4,r2
        beq	   L1_10
L1_7
        ldrb	   r12,[r1],#1
        add	   r4,r4,#1
        cmp	   r4,r2
        strb	   r12,[r0],#1	
        beq	   L1_10
        b	   L1_7
L1_8
        movs       r4,r2			
L1_9
        ldrb       r12,[r1],#1
        beq        L1_10
        subs       r4,r4,#1
        strb       r12,[r0],#1
        bne        L1_9
L1_10
        ldmfd   r13!, {r4-r6,r10-r12, r15} 
        ENDP
        END
              

