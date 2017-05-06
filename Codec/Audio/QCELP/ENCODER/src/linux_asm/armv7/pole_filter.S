@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@**************************************************************
@void do_pole_filter( 
@		short                *input,
@		short                *output,
@		short                numsamples,
@		struct POLE_FILTER   *filter,
@		short                update_flag
@		)

#include   "voQcelpEncID.h"

	.section   .text 
        .global    do_pole_filter_asm
@******************************
@ constant
@******************************
.equ      MEM_OFFSET     , 0x4       
.equ      COEFF_OFFSET   , 0x8

@******************************
@ ARM register 
@******************************
@ *input            RN            0
@ *output           RN            1
@ numsamples        RN            2
@ *filter           RN            3
@ update_flag       RN            4


do_pole_filter_asm:

        STMFD      r13!, {r4 - r12, r14}
        MOV        r8,  #0                        @init i=0
        LDR        r4,  [r3, #0x4]                @get tmpbuf = filter->memory address
        LDR        r5,  [r3, #0x8]                @get pcoeff = filter->pole_coeff

LOOP:
        LDRSH      r6,  [r4, #18]                  @get tmpbuf[9]
        LDRSH      r7,  [r5, #18]                  @get pcoeff[9]
        LDRSH      r9,  [r4, #16]                  @get tmpbuf[8]
        LDRSH      r10, [r5, #16]                  @get pcoeff[8]
        MUL        r12, r6, r7                     @total = pcoeff[9] * tmpbuf[9]
        STRH       r9,  [r4, #18]                  @tmpbuf[9]= tmpbuf[8]
        MUL        r11,  r9, r10                   @total = L_add(total, pcoeff[8] * tmpbuf[8])
        LDRSH      r6,  [r4, #14]                  @get tmpbuf[7]
        QADD       r12, r12, r11


        LDRSH      r7,  [r5, #14]                  @get pcoeff[7]
        LDRSH      r9,  [r4, #12]                  @get tmpbuf[6]
        LDRSH      r10, [r5, #12]                  @get pcoeff[6]
        STRH       r6,  [r4, #16]                  @tmpbuf[8] = tmpbuf[7]
        MUL        r11, r6, r7
        LDRSH      r6,  [r4, #10]                  @get tmpbuf[5]
        QADD       r12, r12, r11
        MUL        r11, r9, r10
        STRH       r9,  [r4, #14]                  @tmpbuf[7] = tmpbuf[6]
        QADD       r12, r12, r11

       

        LDRSH      r7,  [r5, #10]                  @get pcoeff[5]
        LDRSH      r9,  [r4,  #8]                  @get tmpbuf[4]
        LDRSH      r10, [r5,  #8]                  @get pcoeff[4]
        STRH       r6,  [r4, #12]                  @tmpbuf[6] = tmpbuf[5]
        MUL        r11, r6, r7
        LDRSH      r6,  [r4, #6]                   @get tmpbuf[3]
        QADD       r12, r12, r11
        MUL        r11, r9, r10
        STRH       r9,  [r4, #10]                  @tmpbuf[5] = tmpbuf[4]
        QADD       r12, r12, r11



        LDRSH      r7,  [r5, #6]                   @get pcoeff[3]
        LDRSH      r9,  [r4, #4]                   @get tmpbuf[2]
        STRH       r6,  [r4, #8]                   @tmpbuf[4] = tmpbuf[3]
        MUL        r11, r6, r7
        LDRSH      r10, [r5, #4]                   @get pcoeff[2]
        QADD       r12, r12, r11
        MUL        r11, r9, r10
        STRH       r9,  [r4, #6]                   @tmpbuf[3] = tmpbuf[2]
        QADD       r12, r12, r11


        LDRSH      r6,  [r4, #2]                   @get tmpbuf[1]
        LDRSH      r7,  [r5, #2]                   @get pcoeff[1]
        LDRSH      r9,  [r4]                       @get tmpbuf[0]

        STRH       r6,  [r4, #4]                   @tmpbuf[2] = tmpbuf[1]
        MUL        r11, r6, r7
        LDRSH      r10, [r5]                       @get pcoeff[0]
        QADD       r12, r12, r11
        MUL        r11, r9, r10
        STRH       r9, [r4, #2]                    @tmpbuf[1] = tmpbuf[0]
        QADD       r12, r12, r11


        ADD        r11, r0, r8, LSL #1             @get the input[i]  address

        ADD        r6, r12, #0x800
        MOV        r12, r6, ASR #12                @total_s = ((total + 2048)>>12)                    
        LDRSH      r9, [r11]                       @get input[i]
        ADD        r8, r8, #1
        ADD        r12, r12, r9 
           
        @saturate
        LDR        r10, =0x7fff        
        MOV        r6, r12, ASR #15
        TEQ        r6, r12, ASR #31
        EORNE      r12, r10, r12, ASR #31

        STRH       r12, [r4]                       @tmpbuf[0] = output[i]
        STRH       r12, [r1], #2                      @store output[i]
    
        CMP        r8, r2
        BLT        LOOP
 
 
do_pole_filter_end: 
        LDMFD      r13!, {r4 - r12, r15} 
        .END
