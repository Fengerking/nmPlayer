;/*
;*  Copyright 2003 ~ 2010 by Visualon software Incorporated.
;*  All rights reserved. Property of Visualon software Incorporated.
;*  Restricted rights to use, duplicate or disclose this code are
;*  granted through contract.
;*  
;*/

;/***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver         Description             Author
;*    -----------     --------     ------------------        -----------
;*    07-10-2008        1.0        File imported from        Huaping Liu
;*                                             
;**********************************************************************/
;void Residu(
;	    Word16 a[],                           /* (i) Q12 : prediction coefficients */
;	    Word16 x[],                           /* (i)     : speech (values x[-m..-1] are needed */
;	    Word16 y[],                           /* (o) x2  : residual signal       */
;	    Word16 lg                             /* (i)     : size of filtering     */
;	   )
;a[] --- r0
;x[] --- r1
;y[] --- r2
;lg  --- r3

        AREA |.TEXT|, CODE, READONLY
	EXPORT Residu_opt

Residu_opt      FUNCTION

        STMFD   r13!, {r4 - r12, r14}

        LDRSH    r5, [r0], #2
        LDRSH    r6, [r0], #2
        PKHBT    r5, r6, r5, LSL #16                  ;r5 --- a0, a1	

        LDRSH    r6, [r0], #2
	LDRSH    r7, [r0], #2
	PKHBT    r6, r7, r6, LSL #16                  ;r6 --- a2, a3
	
        LDRSH    r7, [r0], #2
	LDRSH    r8, [r0], #2
	PKHBT    r7, r8, r7, LSL #16                  ;r7 --- a4, a5

	LDRSH    r8, [r0], #2
	LDRSH    r9, [r0], #2
	PKHBT    r8, r9, r8, LSL #16                  ;r8 --- a6, a7

	LDRSH    r9, [r0], #2
	LDRSH    r10, [r0], #2
	PKHBT    r9, r10, r9, LSL #16                 ;r9 --- a8, a9

	LDRSH    r10, [r0], #2
	LDRSH    r11, [r0], #2
	PKHBT    r10, r11, r10, LSL #16               ;r10 --- a10, a11

        LDRSH    r11, [r0], #2
	LDRSH    r12, [r0], #2
	PKHBT    r11, r12, r11, LSL #16               ;r11 --- a12, a13
	
	LDRSH    r12, [r0], #2
	LDRSH    r4, [r0], #2
	PKHBT    r12, r4, r12, LSL #16                ;r12 --- a14, a15
	

	STMFD   r13!, {r8 - r12}                     
	LDRH    r4, [r0], #2                         
        MOV     r14, r3, ASR #2                      
	ADD     r1, r1, #4
	MOV     r0, r2
	ORR     r14, r4, r14, LSL #16                

LOOP
        LDR     r10, [r1], #-4                       ;r10  --- x[3], x[2]
	MOV     r3,  #0
	LDR     r2,  [r1], #-4                       ;r2   --- x[1], x[0]

	MOV     r4,  #0
	MOV     r11, #0
	MOV     r12, #0

    	SMULTB  r11, r5, r10                         ;i3(0)  --- r11 = x[2] * a0    
	SMLAD   r4, r5, r2, r4
	SMLAD   r12, r5, r10, r12
        SMLABT  r11, r5, r2, r11                     ;i3(1)  --- r11 += x[1] * a0

	SMULTB  r3, r5, r2                           ;i1(0)  --- r3 = x[0] * a0
        SMLAD   r12, r6, r2, r12
	SMLATB  r11, r6, r2, r11                     ;i3(2)  --- r11 += x[0] * a2


	LDR     r2, [r1], #-4                        ;r2 ---- x[-1], x[-2]
	SMLABT  r3, r5, r2, r3                       ;i1(1) --- r3 += x[-1] * a1
	SMLAD   r4, r6, r2, r4
	SMLABT  r11, r6, r2, r11                     ;i3(3) --- r11 += x[-1] * a3
	SMLAD   r12, r7, r2, r12
        SMLATB  r3, r6, r2, r3                       ;i1(2) --- r3 += x[-2] * a2				
	SMLATB  r11,r7, r2, r11				
			
	
	LDR	r2,[r1],#-4
	SMLABT	r3,  r6, r2, r3	
	SMLAD   r4, r7, r2, r4
	SMLABT	r11, r7, r2, r11
        SMLAD   r12, r8, r2, r12				
	SMLATB	r3,  r7, r2, r3							
	SMLATB	r11, r8, r2, r11				
				
		
	LDR	r2, [r1],#-4
	SMLAD   r4, r8, r2, r4
	SMLABT	r3,  r7, r2, r3	

	SMLABT	r11, r8, r2, r11	
	SMLAD   r12, r9, r2, r12							
	SMLATB	r3,  r8, r2, r3							
	SMLATB	r11, r9, r2, r11				
							
	
	LDR	r2, [r1], #-4
	LDR	r10, [r13, #8]	
	SMLABT	r3, r8, r2, r3		
	SMLAD   r4, r9, r2, r4
	SMLABT	r11,r9, r2, r11	
	SMLAD   r12, r10, r2, r12											
	SMLATB	r3, r9, r2, r3							
	SMLATB	r11,r10, r2, r11			
			
					
	LDR	r2,[r1],#-4
	LDR	r8, [r13, #12]	
	SMLABT	r3, r9, r2, r3	
	SMLAD   r4, r10, r2, r4
	SMLABT	r11,r10, r2, r11
	SMLAD   r12, r8, r2, r12												
	SMLATB	r3, r10, r2, r3							
	SMLATB	r11,r8, r2, r11				
			
			
	
	LDR	r2,[r1],#-4
	LDR	r9, [r13, #16]	
	SMLABT	r3, r10, r2, r3	
	SMLAD   r4, r8, r2, r4
	SMLABT	r11,r8, r2, r11	
	SMLAD   r12, r9, r2, r12												
	SMLATB	r3, r8, r2, r3								
	SMLATB	r11,r9, r2, r11				
			
	

	LDR	r2,[r1],#-4

	SMLAD   r4, r9, r2, r4
	SMLABT	r3, r8, r2, r3				
	SMLABT	r11,r9, r2, r11				
	SMLABT	r12,r14, r2, r12			
	SMLATB	r3, r9, r2, r3				
	SMLABB	r11,r14, r2, r11

        LDR     r2,[r1],#44  
	LDR	r8, [r13]						
        
	SMLABT	r3, r9, r2, r3
	SMLABT	r4, r14, r2, r4
	SMLABB	r3, r14, r2, r3
	LDR	r9, [r13, #4]			
	
	QADD	r11,r11,r11					
	QADD	r12,r12,r12	
	QADD	r3,r3,r3					
	QADD	r4,r4,r4					
				
	QADD	r11,r11,r11					
	QADD	r12,r12,r12		
	QADD	r3,r3,r3					
	QADD	r4,r4,r4					
					
	QADD	r11,r11,r11					
	QADD	r12,r12,r12	
	QADD	r3,r3,r3					
	QADD	r4,r4,r4					
					
	QADD	r11,r11,r11					
	QADD	r12,r12,r12		
	QADD	r3,r3,r3					
	QADD	r4,r4,r4					
				
	
	MOV	r2,#0x8000

	QDADD	r11, r2, r11					
	QDADD	r12, r2, r12	
	QDADD	r3, r2, r3					
	QDADD	r4, r2, r4					
					
		
	MOV	r11, r11, ASR #16
	MOV	r12, r12, ASR #16	
	MOV	r3, r3, ASR #16
	MOV	r4, r4, ASR #16

	
	STRH	r3, [r0], #2
	STRH	r4, [r0], #2
	STRH	r11, [r0], #2
	STRH	r12, [r0], #2
	
	MOV	r2, r14, ASR #16
	SUB	r14, r14, #0x10000
	SUBS	r2, r2, #1

	BNE	LOOP	

	LDMFD	r13!, {r8 -r12}	
	LDMFD	r13!, {r4 -r12,pc}

        ENDFUNC
        END	 
        
	
	  


