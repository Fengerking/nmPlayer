@**************************************************************
@* Copyright 2003~2010 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@****************************************************************
@Word16 Autocorr2 (
@    Word16 x[],            /* (i)    : Input signal (L_WINDOW)            */
@    Word16 r_h[],          /* (o)    : Autocorrelations  (msb)            */
@    Word16 r_l[],          /* (o)    : Autocorrelations  (lsb)            */
@    const Word16 wind[]    /* (i)    : window for LPC analysis (L_WINDOW) */
@)
        #include "voAMRNBEncID.h"
        .text   .align 4 
	.globl   _Autocorr_asm

_Autocorr_asm:

        stmfd      r13!, {r4 - r12, r14}  
        sub        r13,  r13, #488                      @get the y[L_WINDOW] space
        mov        r6,   r13                            @store y[L_WINDOW] address
        mov        r8,   #240                           @L_WINDOW = 240      

@ for(i = 0@ i < L_WINDOW@ i++)
@{
@   y[i] = mult_r(x[i], wind[i])@
@}
        mov        r7, #0x4000
LOOP1:
        ldr        r10, [r0], #4
        ldr        r11, [r3], #4
        subs       r8, r8, #2
        smlabb     r12, r10, r11, r7
	smlatt     r9,  r10, r11, r7
	ssat       r12, #16, r12, asr #15
	ssat       r9, #16, r9, asr #15
	strh       r12, [r6], #2
	strh       r9, [r6], #2

        ldr        r10, [r0], #4
        ldr        r11, [r3], #4
        subs       r8, r8, #2
        smlabb     r12, r10, r11, r7
	smlatt     r9,  r10, r11, r7
	ssat       r12, #16, r12, asr #15
	ssat       r9, #16, r9, asr #15
	strh       r12, [r6], #2
	strh       r9, [r6], #2

        bgt        LOOP1                            
        
@ do 
@ {
@ }while(overf !=0)

LOOP2:
        mov             r6,  r13                            @get y[i] first address
        mov             r8,  #240                           @L_WINDOW = 240 
        mov             r5, #0                              @sum = 0                              
LOOP3:
        ldrsh           r9,  [r6], #2
	ldrsh           r10, [r6], #2
	ldrsh           r11, [r6], #2
	ldrsh           r12, [r6], #2

	mul             r7, r9, r9
	mul             r9, r10, r10
	mul             r10, r11, r11
	mul             r11, r12, r12

	qdadd           r5, r5, r7
	qdadd           r5, r5, r9
	qdadd           r5, r5, r10
	qdadd           r5, r5, r11
                    
        subs            r8, r8, #4
        bgt             LOOP3
  
        cmp             r5, #0x7fffffff                   @ if(sum == MAX_32)
        bne             Lable

        mov             r6, r13                           @ get y[i] first address
        mov             r8, #240

LOOP4:
        ldrsh           r5, [r6]
	ldrsh           r7, [r6, #2]
	ldrsh           r9, [r6, #4]
	ldrsh           r10, [r6, #6]

	mov             r5, r5, asr #2
	mov             r7, r7, asr #2
	mov             r9, r9, asr #2
	mov             r10, r10, asr #2

	strh            r5, [r6], #2
	strh            r7, [r6], #2
	strh            r9, [r6], #2
	strh            r10, [r6], #2

        subs            r8, r8, #4
        bgt             LOOP4
        b               LOOP2
        
Lable:
        add             r5, r5, #1                        @ sum = L_add(sum, 1L)
        @norm = norm_l (sum)
        clz             r9, r5  
        sub             r4, r9, #1                             
        mov             r5, r5, lsl r4                    @ sum <<=norm

        mov             r10, r5, lsr #16
        strh            r10, [r1], #2                     @ r_h[0] = sum >>16
        sub             r11, r5, r10, lsl #16
        mov             r11, r11, lsr #1
        strh            r11, [r2], #2                     @r_l[0]=(sum - (r_h[0]<<16))>>1
 
        mov             r14, #240
        mov             r7, #1                             @ i = 1
	
LOOP5:                                                      @ for(i=1@ i <=m@ i++)
        mov             r8, #0                             @ sum = 0@
        mov             r6, r13                            @ get the y[] address
        sub             r10, r14, r7                       @ r10 --- L_WINDOW-i
        add             r12, r6, r7, lsl #1                @ get the y[i]
        mov             r11, #0                            @ j = 0
   
LOOP6:                                                      @ for(j=0@ j<L_WINDOW-i@ j++)
        ldrsh           r4, [r6], #2                            @ y[j]
        ldrsh           r5, [r12], #2                           @ y[j+i]
        add             r11, r11, #1
        mla             r8, r4, r5, r8                          @ sum += (y[j] * y[j+i])
        cmp             r11, r10
        blt             LOOP6

        mov             r8, r8, lsl r9                     @ sum <<=(norm +1)

        mov             r4, r8, lsr #16                    @ r_h[i] = sum >>16@
        strh            r4, [r1], #2
        sub             r5, r8, r4, lsl #16
        mov             r5, r5, lsr #1
        strh            r5, [r2], #2

        add             r7, r7, #1
        cmp             r7, #10
        ble             LOOP5
               
Autocorr_asm_end: 

        add             r13, r13, #488      
        ldmfd           r13!,{r4 - r12, r15}
        @ENDP   
        @.END
