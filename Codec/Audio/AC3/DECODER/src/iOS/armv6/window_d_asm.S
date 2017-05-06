@*********************************************************************
@* Copyright 2003-2009 by VisualOn Software, Inc.
@* All modifications are confidential and proprietary information
@* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
@**********************************************************************
@***************************** Change History**************************
@* 
@*    DD/MMM/YYYY     Code Ver     Description             Author
@*    -----------     --------     -----------             ------
@*    08-12-2009        1.0        File imported from      Huaping Liu
@*                                             
@**********************************************************************  
       #include "voAC3DecID.h"
       .text
       .align 4
       .globl   _window_d
       .globl   _chantab
       .globl   _Lwindow_Interleave

@Structure, DECEXEC , Size 0x5c bytes, from ./hdr/vo_ac3_var.h
.set    DECEXEC_ac3_buff , 0x8      @  pointer to BUFF_PARAM
@End of Structure DECEXEC

@Structure, BUFF_PARAM , Size 0x48d4 bytes, from ./hdr/vo_ac3_var.h
.set    BUFF_PARAM_dnmix_buf  , 0xc00    @  array[6] of array[256] of Word32
.set    BUFF_PARAM_delay_buf  , 0x2400   @  array[6] of array[128] of Word32
@ End of Structure BUFF_PARAM

@Structure, DEC_PARAM , Size 0x7c bytes, from ./hdr/vo_ac3_var.h
.set    DEC_PARAM_outmod      , 0x1c     @  Word16
.set    DEC_PARAM_pcmbufptr   , 0x64     @  array[6] of pointer to Word32
@End of Structure DEC_PARAM

@***********************************************
@void window_d(DSPshort chan, DECEXEC *p_decexec)
@***********************************************
@ r0 --- chan
@ r1 --- p_decexec, p_decparam

_window_d:
            
           stmfd           r13!, {r4 - r12, r14}
	   sub             r13, r13, #0x20

	   mov             r7, r0                               @chan
	   str             r0, [sp, #0x4]                      @push chan
	   ldr             r6, [r1, #DECEXEC_ac3_buff]          @p_buff
	   ldr             r3, [r1]                             @p_decparam
	   @r6 ----pcmptr1 = p_buff->fftbuf, p_buff
	   mov             r5, #0xFF
	   add             r4, r6, r5, LSL #2                   @pcmptr2 = p_buff->fftbuf + N -1

	   str             r3,  [sp, #0x8]                      @push p_decparam
           str             r6,  [sp, #0xc]                      @push p_buff	   
	   add             r8, r6, #BUFF_PARAM_dnmix_buf        @p_buff->dnmix_buf[] address/*	Copy PCM samples to output buffer */
	   add             r9, r8, r7, LSL #10                   @p_buff->dnmix_buf[chan] address

	   add             r10, r6, #BUFF_PARAM_delay_buf       @p_buff->delay_buf[] address
	   add             r11, r10, r7, LSL #9                 @p_buff->delay_buf[chan] address

	   str             r9,  [sp, #0x10]                     @push p_buff->dnmix_buf[chan]
	   str             r11, [sp, #0x14]                      @push p_buff->delay_buf[chan]

	   @r6--- pcmptr1, r4 --- pcmptr2, r9 --- p_buff->dnmix_buf[chan], r11 --- p_buff->delay_buf[chan]

           ldr             r8, Table1                          @winptr1
	   mov             r0, #0xc0
	   add             r10, r9, r0, LSL #2                   @p_buff->dnmix_buf[chan] + 3*N/4
           mov             r0, #0x3F               
	   add             r9, r9, r0, LSL #2                    @p_buff->dnmix_buf[chan] + N/4-1
	   mov             r0, #0x7F
	   add             r2, r11, r0, LSL #2                   @p_buff->delay_buf[chan] + N/2-1

	   @r6, r4, r8, r2, r11, r9, r10

	   mov             r0, #64

Windows_Loop:

           @samp = (- Vo_Multi32((*windptr2), (*delayptr1)) - Vo_Multi32((*windptr1), (*dnmixptr1)))<<1@
           ldr             r1, [r8], #4                          @*winptr1 
	   ldr             r3, [r8], #4                          @*winptr2
	   ldr             r5, [r10], #4                         @*dnmixptr1
	   ldr             r7, [r11], #4                         @*delayptr1
	   smmul           r12, r1, r5                           @Vo_Multi32((*windptr1), (*dnmixptr1))
           smmul           r14, r3, r7                           @Vo_Multi32((*windptr2), (*delayptr1))
           add             r12, r12, r12
	   add             r12, r12, r14, LSL #1               
	   smmul           r1, r1, r7                            @Vo_Multi32((*windptr1), (*delayptr1++))
	   smmul           r3, r3, r5                            @Vo_Multi32((*windptr2), (*dnmixptr1++))
	   mov             r14, #0
	   add             r3, r3, r3
	   rsb             r12, r12, r14
	   sub             r14, r3, r1, LSL #1
	   add             r12, r12, r12
	   add             r14, r14, r14
	   str             r12, [r6], #4                         @*pcmptr1++ = samp
	   str             r14, [r4], #-4                        @*pcmptr2-- = samp

           ldr             r1, [r8], #4                          @*winptr1
           ldr             r3, [r8], #4                          @*winptr2
           ldr             r5, [r9], #-4                          @*dnmixptr2
           ldr             r7, [r2], #-4                          @*delayptr2

           smmul           r12, r1, r5                           @Vo_Multi32((*windptr1), (*dnmixptr2))
           smmul           r14, r3, r7                           @Vo_Multi32((*windptr2), (*delayptr2))	   
           	   
           add             r12, r12, r12
	   add             r12, r12, r14, LSL #1
	   smmul           r1, r1, r7                            @Vo_Multi32((*windptr1), (*delayptr2--))
	   smmul           r3, r3, r5                            @Vo_Multi32((*windptr2), (*dnmixptr2--))
	   add             r12, r12, r12
	   add             r1, r1, r1
	   add             r3, r3, r3
	   str             r12, [r6], #4                         @*pcmptr1++ = samp
	   sub             r14, r1, r3
	   subs            r0, r0, #1                            @count--
	   add             r14, r14, r14
	   str             r14, [r4], #-4                        @*pcmptr2-- = samp

	   bgt             Windows_Loop

	   @Update delay buffers
  
	   ldr             r9, [sp, #0x10]                        @pull p_buff->dnmix_buf[chan]  
	   mov             r1, #0x40	
           mov             r0, #32                               @N/8
	   add             r9, r9, r1, LSL #2                    @p_buff->dnmix_buf[chan] + (N/4)
           ldr             r11,[sp, #0x14]                        @pull p_buff->delay_buf[chan]
Up_Loop:
           ldr             r3,  [r9], #4
	   ldr             r5,  [r9], #4
	   ldr             r7,  [r9], #4
	   ldr             r12, [r9], #4
	   str             r3,  [r11], #4
	   str             r5,  [r11], #4
	   str             r7,  [r11], #4
	   str             r12, [r11], #4
	   subs            r0, r0, #1
	   bgt             Up_Loop

	   @Copy PCM samples to output buffer
	   mov             r0, #0x6
	   ldr             r3, [sp, #0x8]                        @pull p_decparam
	   ldr             r5, [sp, #0x4]                       @pull chan
	   ldr             r9, [sp, #0xc]                        @p_buff->fftbuf
           ldr             r7, Table2                            @load chantab
           ldrsh           r12, [r3, #DEC_PARAM_outmod]          @p_decparam->outmod
	   add             r14, r3, #DEC_PARAM_pcmbufptr         @p_decparam->pcmbufptr[]

	   mul             r12, r12, r0
	   add             r5, r5, r12      
	   add             r11, r7, r5, LSL #1                   @chantab[p_decparam->outmod][chan]@
	   mov             r0, #64
	   ldrsh           r11, [r11]                            @outchan
	   mov             r10, r11, LSL #2
	   ldr             r11, [r14, r10]                         @p_decpara->pcmbufptr[outchan]

Copy_Loop:
           ldr             r2, [r9], #4
	   ldr             r3, [r9], #4
	   ldr             r4, [r9], #4
	   ldr             r5, [r9], #4
	   str             r2, [r11], #0x18
           str             r3, [r11], #0x18
	   str             r4, [r11], #0x18
	   str             r5, [r11], #0x18
	   subs            r0, r0, #1
	   bgt             Copy_Loop

window_d_end:

           add             r13, r13, #0x20         
           ldmfd           r13!, {r4 - r12, r15}

Table1:
           .word           _Lwindow_Interleave

Table2:
           .word           _chantab

	   @ENDFUNC
	   @.END 
  










