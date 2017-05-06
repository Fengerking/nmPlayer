@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@**************************************************************

@**********************************************************************
@  do_zero_filter_front  function
@**********************************************************************
@void do_zero_filter(
@		short               *input,
@		short               *output,
@		short               numsamples,
@		struct ZERO_FILTER  *filter,
@		short               update_flag 
@		)
@

        .section   .text
        .global    do_zero_filter_asm
@******************************
@ constant
@******************************


@******************************
@ ARM register 
@******************************
@ *input           RN           0
@ *output          RN           1
@ numsamples       RN           2
@ *filter          RN           3
@ update_flag      RN           4

do_zero_filter_asm:

        STMFD      r13!, {r4 - r12, r14}
        MOV        r8, #0    
        LDR        r5, [r3, #0x8]                      @ get coeff ptr
        LDR        r4, [r3, #0x4]                      @ get tmpbuf buffeer address


LOOP:
        ADD        r12, r0, r8, LSL #1                  @ get input[i] address
        LDRSH      r6,  [r5, #18]                       @ load coeffs[9]
        LDRSH      r7,  [r4, #18]                       @ load tmpbuf[9]
        LDRSH      r9,  [r5, #16]                       @ load coeffs[8]

        
        LDRSH      r11, [r12]                           @ load input[i]
        @L_shl
        LDRSH      r10, [r4, #16]                       @ load tmpbuf[8]        
        MOV        r11, r11, LSL #12
        MUL        r12, r6, r7                          @ coeffs[9] * tmpbuf[9]
        STRH       r10, [r4, #18]                       @ tmpbuf[9] = tmpbuf[8]
        QADD       r12, r12, r11          

        MUL        r11, r9, r10
        LDRSH      r6, [r5, #14]                        @ load coeffs[7]
        QADD       r12, r12, r11


        LDRSH      r7, [r4, #14]                        @ load tmpbuf[7]
        LDRSH      r9, [r5, #12]                        @ load coeffs[6]
        STRH       r7, [r4, #16]                        @ tmpbuf[8] = tmpbuf[7]
        MUL        r11, r6, r7
        LDRSH      r10,[r4, #12]                        @ load tmpbuf[6]
        QADD       r12, r12, r11
        MUL        r11, r9, r10
        STRH       r10, [r4, #14]                       @ tmpbuf[7] = tmpbuf[6]
        QADD       r12, r12, r11

        LDRSH      r6, [r5, #10]                        @ load coeffs[5]
        LDRSH      r7, [r4, #10]                        @ load tmpbuf[5]
        LDRSH      r9, [r5,  #8]                        @ load coeffs[4]

        STRH       r7, [r4, #12]                        @ tmpbuf[6] = tmpbuf[5]
        MUL        r11, r6, r7
        LDRSH      r10,[r4,  #8]                        @ load tmpbuf[4]
        QADD       r12, r12, r11
        MUL        r11, r9, r10
        STRH       r10, [r4, #10]                       @ tmpbuf[5] = tmpbuf[4]
        QADD       r12, r12, r11

        LDRSH      r6, [r5,  #6]                        @ load coeffs[3]
        LDRSH      r7, [r4,  #6]                        @ load tmpbuf[3]
        LDRSH      r9, [r5,  #4]                        @ load coeffs[2]

        STRH       r7, [r4,  #8]                        @ tmpbuf[4] = tmpbuf[3]
        MUL        r11, r6, r7
        LDRSH      r10,[r4,  #4]                        @ load tmpbuf[2]

        QADD       r12, r12, r11
        MUL        r11, r9, r10
        STRH       r10, [r4,  #6]                       @ tmpbuf[3] = tmpbuf[2]
        QADD       r12, r12, r11

        LDRSH      r6, [r5,  #2]                        @ load coeffs[1]
        LDRSH      r7, [r4,  #2]                        @ load tmpbuf[1]
        LDRSH      r9, [r5]                             @ load coeffs[0]

        STRH       r7, [r4, #4]                         @ tmpbuf[2] = tmpbuf[1]
        MUL        r11, r6, r7
        LDRSH      r10,[r4]                             @ load tmpbuf[0]
        QADD       r12, r12, r11
        MUL        r11, r9, r10
        STRH       r10, [r4,  #2]                       @ tmpbuf[1] = tmpbuf[0]
        QADD       r12, r12, r11

        ADD        r11, r0, r8, LSL #1                  @ get input[i] address
        LDRSH      r9, [r11]                            @ load input[i]

        STRH       r9, [r4]                             @ tmpbuf[0] = input[i]
        
        ADD        r11, r12, #0x800
        MOV        r12, r11, ASR #12                    @total = L_add(total, 2048) >>12
 
        ADD        r14, r1, r8, LSL #1                  @ get output[i] address
        ADD        r8, r8, #1
        @saturate(total)
        LDR        r7, =0x7fff        
        MOV        r6, r12, ASR #15
        TEQ        r6, r11, ASR #31
        EORNE      r12, r7, r12, ASR #31
   
        STRH       r12, [r14]                           @ store output[i]
        CMP        r8, r2
        BLT        LOOP

do_zero_filter_end:
 
        LDMFD      r13!, {r4 - r12, r15} 
        .END


