@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@**************************************************************

@**********************************************************************
@  do_zero_filter_front  function
@**********************************************************************
@void do_zero_filter_front(
@		short               *input,
@		short               *output,
@		short               numsamples,
@		struct ZERO_FILTER  *filter,
@		short               update_flag 
@		)
@
        .section  .text
        .global   do_zero_filter_front_asm
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

do_zero_filter_front_asm:

        STMFD      r13!, {r4 - r12, r14}
        MOV        r8, #0    
        LDR        r5, [r3, #0x8]                      @ get coeff ptr
        LDR        r4, [r3, #0x4]                      @ get tmpbuf buffeer address

        LDRSH      r9,  [r5]                           @ temp1 = filter->zero_coeff[0]
        LDRSH      r10, [r5, #2]                       @ temp2 = filter->zero_coeff[1]

LOOP:
        ADD        r7, r0, r8, LSL #1                  @ get input[i] address
        LDRSH      r6, [r4, #2]                        @ load tmpbuf[1]
        LDRSH      r11, [r7]                           @ load input[i]
        @L_shl
        MUL        r12, r10, r6                         @ temp2 * (int)tmpbuf[1]
        MOV        r11, r11, LSL #12
        QADD       r11, r11, r12
        LDRSH      r6, [r4]                             @ load tmpbuf[0]
        ADD        r3, r1, r8, LSL #1                  @ get output[i] address
        ADD        r8, r8, #1
        MUL        r12, r9, r6                          @ temp1 * (int)tmpbuf[0]
        STRH       r6, [r4, #2]                         @ tmpbuf[1] = tmpbuf[0]
        QADD       r11, r11, r12
        LDRSH      r14, [r7]                            @ load input[i]
        STRH       r14, [r4]                            @ tmpbuf[0] = input[i]
        ADD        r12, r11, #0x800
        MOV        r11, r12, ASR #12                    @total = L_add(total, 2048) >>12


        @saturate(total)
        LDR        r7, =0x7fff        
        MOV        r6, r11, ASR #15
        TEQ        r6, r11, ASR #31
        EORNE      r11, r7, r11, ASR #31
   
        STRH       r11, [r3]                           @ store output[i]

        CMP        r8, r2
        BLT        LOOP

do_zero_filter_front_end:
 
        LDMFD      r13!, {r4 - r12, r15} 
        .END


