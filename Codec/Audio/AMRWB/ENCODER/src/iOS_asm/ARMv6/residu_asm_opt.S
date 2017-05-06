@/*
@*  Copyright 2003 ~ 2010 by Visualon software Incorporated.
@*  All rights reserved. Property of Visualon software Incorporated.
@*  Restricted rights to use, duplicate or disclose this code are
@*  granted through contract.
@*  
@*/

@/***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver         Description             Author
@*    -----------     --------     ------------------        -----------
@*    07-10-2008        1.0        File imported from        Huaping Liu
@*                                             
@**********************************************************************/
@void Residu(
@	    Word16 a[],                           /* (i) Q12 : prediction coefficients */
@	    Word16 x[],                           /* (i)     : speech (values x[-m..-1] are needed */
@	    Word16 y[],                           /* (o) x2  : residual signal       */
@	    Word16 lg                             /* (i)     : size of filtering     */
@	   )
@a[] --- r0
@x[] --- r1
@y[] --- r2
@lg  --- r3
        #include "voAMRWBEncID.h"
        .text   .align 4 
	.globl   _Residu_opt

_Residu_opt:

        stmfd   r13!, {r4 - r12, r14}

        ldrsh    r5, [r0], #2
        ldrsh    r6, [r0], #2
        pkhbt    r5, r6, r5, lsl #16                  @r5 --- a0, a1	

        ldrsh    r6, [r0], #2
	ldrsh    r7, [r0], #2
	pkhbt    r6, r7, r6, lsl #16                  @r6 --- a2, a3
	
        ldrsh    r7, [r0], #2
	ldrsh    r8, [r0], #2
	pkhbt    r7, r8, r7, lsl #16                  @r7 --- a4, a5

	ldrsh    r8, [r0], #2
	ldrsh    r9, [r0], #2
	pkhbt    r8, r9, r8, lsl #16                  @r8 --- a6, a7

	ldrsh    r9, [r0], #2
	ldrsh    r10, [r0], #2
	pkhbt    r9, r10, r9, lsl #16                 @r9 --- a8, a9

	ldrsh    r10, [r0], #2
	ldrsh    r11, [r0], #2
	pkhbt    r10, r11, r10, lsl #16               @r10 --- a10, a11

        ldrsh    r11, [r0], #2
	ldrsh    r12, [r0], #2
	pkhbt    r11, r12, r11, lsl #16               @r11 --- a12, a13
	
	ldrsh    r12, [r0], #2
	ldrsh    r4, [r0], #2
	pkhbt    r12, r4, r12, lsl #16                @r12 --- a14, a15
	

	stmfd   r13!, {r8 - r12}                     
	ldrh    r4, [r0], #2                         
        mov     r14, r3, asr #2                      
	add     r1, r1, #4
	mov     r0, r2
	orr     r14, r4, r14, lsl #16                

LOOP:

        ldr     r10, [r1], #-4                       @r10  --- x[3], x[2]
	mov     r3,  #0
	ldr     r2,  [r1], #-4                       @r2   --- x[1], x[0]

	mov     r4,  #0
	mov     r11, #0
	mov     r12, #0

    	smultb  r11, r5, r10                         @i3(0)  --- r11 = x[2] * a0    
	smlad   r4, r5, r2, r4
	smlad   r12, r5, r10, r12
        smlabt  r11, r5, r2, r11                     @i3(1)  --- r11 += x[1] * a0

	smultb  r3, r5, r2                           @i1(0)  --- r3 = x[0] * a0
        smlad   r12, r6, r2, r12
	smlatb  r11, r6, r2, r11                     @i3(2)  --- r11 += x[0] * a2


	ldr     r2, [r1], #-4                        @r2 ---- x[-1], x[-2]
	smlabt  r3, r5, r2, r3                       @i1(1) --- r3 += x[-1] * a1
	smlad   r4, r6, r2, r4
	smlabt  r11, r6, r2, r11                     @i3(3) --- r11 += x[-1] * a3
	smlad   r12, r7, r2, r12
        smlatb  r3, r6, r2, r3                       @i1(2) --- r3 += x[-2] * a2				
	smlatb  r11,r7, r2, r11				
			
	
	ldr	r2,[r1],#-4
	smlabt	r3,  r6, r2, r3	
	smlad   r4, r7, r2, r4
	smlabt	r11, r7, r2, r11
        smlad   r12, r8, r2, r12				
	smlatb	r3,  r7, r2, r3							
	smlatb	r11, r8, r2, r11				
				
		
	ldr	r2, [r1],#-4
	smlad   r4, r8, r2, r4
	smlabt	r3,  r7, r2, r3	

	smlabt	r11, r8, r2, r11	
	smlad   r12, r9, r2, r12							
	smlatb	r3,  r8, r2, r3							
	smlatb	r11, r9, r2, r11				
							
	
	ldr	r2, [r1], #-4
	ldr	r10, [r13, #8]	
	smlabt	r3, r8, r2, r3		
	smlad   r4, r9, r2, r4
	smlabt	r11,r9, r2, r11	
	smlad   r12, r10, r2, r12											
	smlatb	r3, r9, r2, r3							
	smlatb	r11,r10, r2, r11			
			
					
	ldr	r2,[r1],#-4
	ldr	r8, [r13, #12]	
	smlabt	r3, r9, r2, r3	
	smlad   r4, r10, r2, r4
	smlabt	r11,r10, r2, r11
	smlad   r12, r8, r2, r12												
	smlatb	r3, r10, r2, r3							
	smlatb	r11,r8, r2, r11				
			
			
	
	ldr	r2,[r1],#-4
	ldr	r9, [r13, #16]	
	smlabt	r3, r10, r2, r3	
	smlad   r4, r8, r2, r4
	smlabt	r11,r8, r2, r11	
	smlad   r12, r9, r2, r12												
	smlatb	r3, r8, r2, r3								
	smlatb	r11,r9, r2, r11				
			
	

	ldr	r2,[r1],#-4

	smlad   r4, r9, r2, r4
	smlabt	r3, r8, r2, r3				
	smlabt	r11,r9, r2, r11				
	smlabt	r12,r14, r2, r12			
	smlatb	r3, r9, r2, r3				
	smlabb	r11,r14, r2, r11

        ldr     r2,[r1],#44  
	ldr	r8, [r13]						
        
	smlabt	r3, r9, r2, r3
	smlabt	r4, r14, r2, r4
	smlabb	r3, r14, r2, r3
	ldr	r9, [r13, #4]			
	
	qadd	r11,r11,r11					
	qadd	r12,r12,r12	
	qadd	r3,r3,r3					
	qadd	r4,r4,r4					
				
	qadd	r11,r11,r11					
	qadd	r12,r12,r12		
	qadd	r3,r3,r3					
	qadd	r4,r4,r4					
					
	qadd	r11,r11,r11					
	qadd	r12,r12,r12	
	qadd	r3,r3,r3					
	qadd	r4,r4,r4					
					
	qadd	r11,r11,r11					
	qadd	r12,r12,r12		
	qadd	r3,r3,r3					
	qadd	r4,r4,r4					
				
	
	mov	r2,#0x8000

	qdadd	r11, r2, r11					
	qdadd	r12, r2, r12	
	qdadd	r3, r2, r3					
	qdadd	r4, r2, r4					
					
		
	mov	r11, r11, asr #16
	mov	r12, r12, asr #16	
	mov	r3, r3, asr #16
	mov	r4, r4, asr #16

	
	strh	r3, [r0], #2
	strh	r4, [r0], #2
	strh	r11, [r0], #2
	strh	r12, [r0], #2
	
	mov	r2, r14, asr #16
	sub	r14, r14, #0x10000
	subs	r2, r2, #1

	bne	LOOP	

	ldmfd	r13!, {r8 -r12}	
	ldmfd	r13!, {r4 -r12,pc}

        @.END	 
        
	
	  


