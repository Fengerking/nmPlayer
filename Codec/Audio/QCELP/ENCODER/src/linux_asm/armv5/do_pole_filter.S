@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@**************************************************************

@**********************************************************************
@  do_pole_filter_high  function
@**********************************************************************
@void do_pole_filter_high( 
@		short                *input,
@		short                *output,
@		short                numsamples,
@		struct POLE_FILTER   *filter,
@		short                update_flag
@		)

        .section  .text
        .global   do_pole_filter_high_asm
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

do_pole_filter_high_asm:

        STMFD      r13!, {r4 - r12, r14}
        MOV        r8, #0                              @i = 0 
        LDR        r4, [r3, #0x4]                      @get the filter->memory
        LDR        r5, [r3, #0x8]                      @get the filter->pole_coeff

        LDRSH      r6, [r4]                            @tmp2 = filter->memory[0]
        LDRSH      r7, [r4, #2]                        @tmp1 = filter->memory[1]
 
        LDRSH      r11, [r5]                           @para1 = filter->pole_coeff[0]
        LDRSH      r12, [r5, #2]                       @para2 = filter->pole_coeff[1]  
                     
        LDRSH      r9, [r4, #4]                        @tmp2_fra = filter->memory[2]
        LDRSH      r10, [r4, #6]                       @tmp1_fra = filter->memory[3]    

LOOP:
        MUL        r14, r7, r12                        @ total = tmp1 * para2
        MUL        r4, r10, r12                        @ total_fra = tmp1_fra * para2
        MLA        r14, r6, r11, r14                   @ total += tmp2 * para1
        MLA        r4, r9, r11, r4                     @ total_fra += tmp2_fra * para1
     
        LDR        r5, =0x2000                         @ get constant value 8192
        QADD       r4, r4, r5
        MOV        r7, r6                              @ tmp1 = tmp2 
        MOV        r4, r4, ASR #14                     @ total_fra =L_add(total_fra, 8192) >> 14
        QADD       r14, r14, r4                        @ total = L_add(total, total_fra)
        QADD       r4, r14, r5             
        MOV        r4, r4, ASR #14                     @ total_s = L_add(total, 8192) >>14

        MOV        r5, r4, LSL #14                     @ total_s << 14
        QSUB       r5, r14, r5                         @ total_fra = L_sub(total, total_s << 14)


        MOV        r10, r9                             @ tmp1_fra = tmp2_fra
        MOV        r9, r5                              @ tmp2_fra = total_fra
        @need store register is : r4 --- total_s , r6~r12 
        ADD        r5,  r0, r8, LSL #1                 @ get input[i] address

        LDRSH      r14, [r5]                           @ load input[i]
        ADD        r5, r1, r8, LSL #1                  @ get output[i] address
        @MOV        r6,  r14                            @ tmp2 = input[i]
        QADD       r14, r14, r4                        @ L_add(input[i], total_s)
        

        ADD        r8, r8, #1                          @ i++       
        STRH       r14, [r5]                           
        MOV        r6, r14

        CMP        r8, r2
        BLT        LOOP

        LDR        r4, [r3, #0x4]                      @ get the filter->memory
        STRH       r6,  [r4]                           @ filter->memory[0] = tmp2        
        STRH       r7,  [r4, #2]                       @ filter->memory[1] = tmp1
        STRH       r9,  [r4, #4]                       @ filter->memory[2] = tmp2_fra
        STRH       r10, [r4, #6]                       @ filter->memory[3] = tmp1_fra

do_pole_filter_high_end:
 
        LDMFD      r13!, {r4 - r12, r15} 
        .END


