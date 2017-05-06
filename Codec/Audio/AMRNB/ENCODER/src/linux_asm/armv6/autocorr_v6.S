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
        .section   .text 
	.global   Autocorr_asm

Autocorr_asm:

        STMFD      r13!, {r4 - r12, r14}  
        SUB        r13,  r13, #488                      @get the y[L_WINDOW] space
        MOV        r6,   r13                            @store y[L_WINDOW] address
        MOV        r8,   #240                           @L_WINDOW = 240      

@ for(i = 0@ i < L_WINDOW@ i++)
@{
@   y[i] = mult_r(x[i], wind[i])@
@}
        MOV        r7, #0x4000
LOOP1:
        LDR        r10, [r0], #4
        LDR        r11, [r3], #4
        SUBS       r8, r8, #2
        SMLABB     r12, r10, r11, r7
	SMLATT     r9,  r10, r11, r7
	SSAT       r12, #16, r12, ASR #15
	SSAT       r9, #16, r9, ASR #15
	STRH       r12, [r6], #2
	STRH       r9, [r6], #2

        LDR        r10, [r0], #4
        LDR        r11, [r3], #4
        SUBS       r8, r8, #2
        SMLABB     r12, r10, r11, r7
	SMLATT     r9,  r10, r11, r7
	SSAT       r12, #16, r12, ASR #15
	SSAT       r9, #16, r9, ASR #15
	STRH       r12, [r6], #2
	STRH       r9, [r6], #2

        BGT        LOOP1                            
        
@ do 
@ {
@ }while(overf !=0)

LOOP2:
        MOV             r6,  r13                            @get y[i] first address
        MOV             r8,  #240                           @L_WINDOW = 240 
        MOV             r5, #0                              @sum = 0                              
LOOP3:
        LDRSH           r9,  [r6], #2
	LDRSH           r10, [r6], #2
	LDRSH           r11, [r6], #2
	LDRSH           r12, [r6], #2

	MUL             r7, r9, r9
	MUL             r9, r10, r10
	MUL             r10, r11, r11
	MUL             r11, r12, r12

	QDADD           r5, r5, r7
	QDADD           r5, r5, r9
	QDADD           r5, r5, r10
	QDADD           r5, r5, r11
                    
        SUBS            r8, r8, #4
        BGT             LOOP3
  
        CMP             r5, #0x7fffffff                   @ if(sum == MAX_32)
        BNE             Lable

        MOV             r6, r13                           @ get y[i] first address
        MOV             r8, #240

LOOP4:
        LDRSH           r5, [r6]
	LDRSH           r7, [r6, #2]
	LDRSH           r9, [r6, #4]
	LDRSH           r10, [r6, #6]

	MOV             r5, r5, ASR #2
	MOV             r7, r7, ASR #2
	MOV             r9, r9, ASR #2
	MOV             r10, r10, ASR #2

	STRH            r5, [r6], #2
	STRH            r7, [r6], #2
	STRH            r9, [r6], #2
	STRH            r10, [r6], #2

        SUBS            r8, r8, #4
        BGT             LOOP4
        B               LOOP2
        
Lable:
        ADD             r5, r5, #1                        @ sum = L_add(sum, 1L)
        @norm = norm_l (sum)
        CLZ             r9, r5  
        SUB             r4, r9, #1                             
        MOV             r5, r5, LSL r4                    @ sum <<=norm

        MOV             r10, r5, LSR #16
        STRH            r10, [r1], #2                     @ r_h[0] = sum >>16
        SUB             r11, r5, r10, LSL #16
        MOV             r11, r11, LSR #1
        STRH            r11, [r2], #2                     @r_l[0]=(sum - (r_h[0]<<16))>>1
 
        MOV             r14, #240
        MOV             r7, #1                             @ i = 1
	
LOOP5:                                                      @ for(i=1@ i <=m@ i++)
        MOV             r8, #0                             @ sum = 0@
        MOV             r6, r13                            @ get the y[] address
        SUB             r10, r14, r7                       @ r10 --- L_WINDOW-i
        ADD             r12, r6, r7, LSL #1                @ get the y[i]
        MOV             r11, #0                            @ j = 0
   
LOOP6:                                                      @ for(j=0@ j<L_WINDOW-i@ j++)
        LDRSH           r4, [r6], #2                            @ y[j]
        LDRSH           r5, [r12], #2                           @ y[j+i]
        ADD             r11, r11, #1
        MLA             r8, r4, r5, r8                          @ sum += (y[j] * y[j+i])
        CMP             r11, r10
        BLT             LOOP6

        MOV             r8, r8, LSL r9                     @ sum <<=(norm +1)

        MOV             r4, r8, LSR #16                    @ r_h[i] = sum >>16@
        STRH            r4, [r1], #2
        SUB             r5, r8, r4, LSL #16
        MOV             r5, r5, LSR #1
        STRH            r5, [r2], #2

        ADD             r7, r7, #1
        CMP             r7, #10
        BLE             LOOP5
               
Autocorr_asm_end: 

        ADD             r13, r13, #488      
        LDMFD           r13!,{r4 - r12, r15}
        @ENDP   
        .END
