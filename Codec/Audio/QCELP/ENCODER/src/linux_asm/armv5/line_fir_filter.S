@**************************************************************
@* Copyright 2008 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@**************************************************************

@**********************************************************************
@  do_fir_linear_filter  function
@**********************************************************************
@void do_fir_linear_filter( 
@		short               *input,
@		short               *output,
@		short               numsamples,
@		struct ZERO_FILTER  *filter,
@		short               update_flag 
@		)

        .section  .text
        .global   do_fir_linear_filter_asm
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

do_fir_linear_filter_asm:

        STMFD      r13!, {r4 - r12, r14}
        MOV        r8, #0    
        LDR        r4, [r3, #0x4]                     @ get tmpbuf buffeer address
        LDR        r5, [r3, #0x8]                     @ get coeffs ptr

LOOP:
        ADD        r12, r0, r8, LSL #1                @ get input[i] address
        LDRSH      r6, [r4, #30]                      @ load tmpbuf[15]
        LDRSH      r11, [r12]                         @ load input[i]
        LDRSH      r7, [r4, #28]                      @ load tmpbuf[14]
        LDRSH      r9, [r5]                           @ load coeffs[0]
        ADD        r10, r6, r11                       @ tmpbuf[15] + input[i]
        STRH       r7, [r4, #30]                      @ tmpbuf[15] = tmpbuf[14]

        @total = (tmpbuf[15] + input[i])*coeffs[0]@
        MUL        r12, r10, r9                       @ (tmpbuf[15] + input[i]) *coeffs[0]
        
        LDRSH      r6, [r4]                           @ load tmpbuf[0]
        LDRSH      r11, [r4, #26]                     @ load tmpbuf[13]
        LDRSH      r9, [r5, #2]                       @ load coeffs[1]
        LDRSH      r14, [r4, #2]                      @ load tmpbuf[1]
        ADD        r10, r7, r6                        @ tmpbuf[14] + tmpbuf[0]
        STRH       r11, [r4, #28]                     @ tmpbuf[14] = tmpbuf[13]
        MUL        r7, r10, r9  
        @total = L_add(total, (tmpbuf[14] + tmpbuf[0])*coeffs[1])@ 
        LDRSH      r9, [r5, #4]                       @ load coeffs[2]       
        QADD       r12, r12, r7  

                 
 
        LDRSH      r10, [r4, #24]                     @ load tmpbuf[12]
        ADD        r6, r11, r14               
        MUL        r11, r6, r9
        STRH       r10, [r4, #26]                     @ tmpbuf[13] = tmpbuf[12]
        LDRSH      r9, [r5, #6]                       @ load coeffs[3]
        LDRSH      r6, [r4, #4]                       @ load tmpbuf[2]
        LDRSH      r7, [r4, #24]                      @ load tmpbuf[12]

        @total = L_add(total, (tmpbuf[13] + tmpbuf[1])*coeffs[2])@
        QADD       r12, r12, r11

        LDRSH      r14, [r4, #22]                     @ load tmpbuf[11]
        ADD        r11, r6, r7                        @ tmpbuf[12] + tmpbuf[2]
        MUL        r6, r11, r9               
        LDRSH      r9, [r5, #8]                       @ load coeffs[4]
        QADD       r12, r12, r6

        STRH       r14, [r4, #24]                     @ tmpbuf[12] = tmpbuf[11]
        LDRSH      r6, [r4, #6]                       @ load tmpbuf[3]
        LDRSH      r7, [r4, #8]                       @ load tmpbuf[4]
        LDRSH      r10, [r4, #20]                     @ load tmpbuf[10]
        ADD        r11, r14, r6                       @ tmpbuf[11] + tmpbuf[3]
        MUL        r6, r11, r9
        STRH       r10, [r4, #22]                     @ tmpbuf[11] = tmpbuf[10]
        QADD       r12, r12, r6


        LDRSH      r9, [r5, #10]                      @ load coeffs[5]
        ADD        r11, r7, r10                       @ tmpbuf[10] + tmpbuf[4]
        LDRSH      r6, [r4, #10]                      @ load tmpbuf[5]
        LDRSH      r10, [r4, #18]                     @ load tmbbuf[9]
        MUL        r7, r11, r9
        STRH       r10, [r4, #20]                     @ tmpbuf[10] = tmpbuf[9]
        QADD       r12, r12, r7

        LDRSH      r7, [r4, #12]                      @ load tmpbuf[6]

        LDRSH      r9, [r5, #12]                      @ load coeffs[6]
        ADD        r11, r6, r10                       @ tmpbuf[9] + tmpbuf[5]
        LDRSH      r10, [r4, #16]                     @ load tmpbuf[8]
        MUL        r6, r11, r9
        LDRSH      r9, [r5, #14]                      @ load coeffs[7]

        QADD       r12, r12, r6 
   
        STRH       r10, [r4, #18]                     @ tmpbuf[9] = tmpbuf[8]
        ADD        r11, r7, r10                       @ tmpbuf[8] + tmpbuf[6]
        MUL        r7, r11, r9 
        LDRSH      r6, [r4, #14]                      @ load tmpbuf[7]               
        QADD       r12, r12, r7



        LDRSH      r9, [r5, #16]                      @ load coeffs[8]
        LDRSH      r7, [r4, #12]                      @ load tmpbuf[6]
        STRH       r6, [r4, #16]                      @ tmpbuf[8] = tmpbuf[7]
        LDRSH      r10, [r4, #10]                     @ load tmpbuf[5]
        MUL        r11, r6, r9                        @ tmpbuf[7] * coeffs[8]
        STRH       r7, [r4, #14]                      @ tmpbuf[7] = tmpbuf[6]
        QADD       r12, r12, r11  
  

        LDRSH      r6, [r4, #8]                       @ load tmpbuf[4]
        LDRSH      r9, [r4, #6]                       @ load tmpbuf[3]
        STRH       r10, [r4, #12]                     @ tmpbuf[6] = tmpbuf[5]
        STRH       r6, [r4, #10]                      @ tmpbuf[5] = tmpbuf[4]
        STRH       r9, [r4, #8]                       @ tmpbuf[4] = tmpbuf[3]
        LDRSH      r7, [r4, #4]                       @ load tmpbuf[2]
        LDRSH      r6, [r4, #2]                       @ load tmpbuf[1]
        LDRSH      r10, [r4]                          @ load tmpbuf[0]
        STRH       r7, [r4, #6]                       @ tmpbuf[3] = tmpbuf[2]
        STRH       r6, [r4, #4]                       @ tmpbuf[2] = tmpbuf[1]
        STRH       r10, [r4, #2]                      @ tmpbuf[1] = tmpbuf[0]

        LDR        r7, =0x2000

        ADD        r10, r0, r8, LSL #1                @ input buffer address
        ADD        r11, r1, r8, LSL #1                @ output buffer address
        LDRSH      r6, [r10]                          @ load input[i]

   
        QADD       r12, r12, r7                       @total = L_add(total, 8192)>>14
        STRH       r6, [r4]
        MOV        r12, r12, ASR #14    
  

        @saturate
        LDR        r7, =0x7fff        
        MOV        r6, r12, ASR #15
        TEQ        r6, r12, ASR #31
        EORNE      r12, r7, r12, ASR #31
      
        ADD        r8, r8, #1
        STRH       r12, [r11]
        CMP        r8, r2    
        BLT        LOOP

do_fir_linear_filter_end:
 
        LDMFD      r13!, {r4 - r12, r15} 
        .END


