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
	   .globl    _downmix
	   .globl    _chantab

@void downmix(DEC_PARAM  *p_decparam, BUFF_PARAM *p_buff)
           @ r0 --- d_pecparam, 
	   @ r1 --- p_buff

@Structure, DEC_PARAM , Size 0x7c bytes, from ./hdr/vo_ac3_var.h
.set  DEC_PARAM_ac3_si     ,   0x4      @  pointer to AC3_INFO_SI
.set  DEC_PARAM_ac3_aub    ,   0x8      @  pointer to AC3_AB_DATA
.set  DEC_PARAM_channum    ,   0xe      @  Word16
.set  DEC_PARAM_outmod     ,   0x1c     @  Word16
.set  DEC_PARAM_outnchans  ,   0x20     @  Word16
@End of Structure DEC_PARAM

@Structure, BUFF_PARAM , Size 0x48d4 bytes, from ./hdr/vo_ac3_var.h
.set  BUFF_PARAM_dnmix_buf , 0xc00    @  array[6] of array[256] of Word32
@End of Structure BUFF_PARAM

@Structure, AC3_INFO_SI , Size 0x68 bytes, from ./hdr/vo_ac3_var.h
.set  AC3_INFO_SI_acmod   , 0xc      @  Word16
@End of Structure AC3_INFO_SI

@Structure, AC3_AB_DATA , Size 0x650 bytes, from ./hdr/vo_ac3_var.h
.set  AC3_AB_DATA_appgainrng     , 0x216    @  array[6] of Word16
.set  AC3_AB_DATA_dnmixtab       , 0x3fc    @  array[6] of array[6] of Word32
.set  AC3_AB_DATA_tcbuf          , 0x648    @  pointer to Word32
.set  AC3_AB_DATA_dnmixbufinu    , 0x3ee    @  array[6] of Word16
@End of Structure AC3_AB_DATA

_downmix:
        
           stmfd       r13!, {r4 - r12, r14}
	   sub         r13, r13, #0x20
	   ldr         r2, [r0, #DEC_PARAM_ac3_aub]      @AC3_AB_DATA *p_aublk
	   ldr         r3, [r0, #DEC_PARAM_ac3_si]       @AC3_INFO_SI *ac3_si

           @ Do downmixing
           ldr         r8, =0x216 
           ldrsh       r4, [r3, #AC3_INFO_SI_acmod]      @ac3_si->acmod
	   ldrsh       r5, [r0, #DEC_PARAM_channum]      @p_decparam->channum
	   ldrsh       r6, [r0, #DEC_PARAM_outnchans]    @p_decparam->outnchans
	   ldrsh       r10, [r0, #DEC_PARAM_outmod]      @p_decparam->outmod
	   add         r14, r2, r8                       @p_aublk->appgainrng


	   ldr         r8, Table                         @chantab[]
           mov         r7, #0x6
	   mul         r4, r4, r7
	   mul         r11, r10, r7
	   add         r7, r4, r5
	   add         r9, r8, r7, LSL #1                @&chantab[ac3_si->acmod][p_decparam->channum]@
           add         r7, r8, r11, LSL #1               @&chantab[p_decparam->outmod][0]
	   add         r11, r14, r5, LSL #1              
           ldrsh       r12, [r9]                         @inchan
	   ldrsh       r5, [r11]                         @gainrng

	   add         r8, r1, #BUFF_PARAM_dnmix_buf     @p_buff->dnmix_buf[]
	   add         r10, r2, #AC3_AB_DATA_dnmixtab    @p_aublk->dnmixtab[][]

	   @r7 --- chantabptr, r12 --- inchan, r6 --- p_decparam->outnchans
	   @r5 --- gainrng, r8 --- p_buff->dnmix_buf[], r2 --- p_aublk, r10 --- p_aublk->dnmixtab[][]

	   mov         r0, #0x0                          @chan=0
LOOP:
	   ldrsh       r1, [r7], #0x2                    @outchan = *chantabptr
	   mov         r11, #0x6
	   mul         r3, r11, r1
           add         r3, r3, r12
	   add         r3, r10, r3, LSL #2               @&p_aublk->dnmixtab[outchan][inchan]
           ldr         r11, [r3]                         @p_aublk->dnmixtab[outchan][inchan]
           cmp         r5, #0x0
	   moveq       r11, r11, LSL #1                  @p_aublk->dnmixtab[outchan][inchan]<<1
	   subne       r4, r5, #1
	   movne       r11, r11, LSR r4                  @p_aublk->dnmixtab[outchan][inchan] >> (gainrng - 1)
	   movne       r11, r11

	   cmp         r11, #0
	   beq         Lable1

	   @r8 --- p_buff->dnmix_buf[], r2 --- p_aublk, r0 --- chan
	   @r7 --- chantabptr, r10 --- p_aublk->dnmixtab[][], r5 --- gainrng, 
	   @r6 --- p_decparam->outchans, r12 --- inchan, r14 --- dnmixfac
	   str         r2, [sp, #0x4]                    @push p_aublk
  
           ldr         r9, =0x3ee
	   ldr         r1, [r2, #AC3_AB_DATA_tcbuf]      @tcptr = p_aublk->tcbuf
	   add         r3, r8, r0, LSL #10               @dnmixptr = p_buff->dnmix_buf[chan]
	   add         r4, r2, r9                        @p_aublk->dnmixbufinu
	   mov         r9, #0x100                        @count = 256
	   add         r14, r4, r0, LSL #1               @&p_aublk->dnmixbufinu[chan]

	   str         r14, [sp, #0x8]                   @push &p_aublk->dnmixbufinu[chan]
	   str         r6,  [sp, #0xc]                   @push &p_decparam->outnchans
	   str         r10, [sp, #0x10]                  @push p_aublk->dnmixtab[][]

	   ldrsh       r2, [r14]                         @p_aublk->dnmixbufinu[chan]
	   cmp         r2, #0
	   bne         LOOP2

	   @r1 --- tcptr, r3 --- dnmixptr, r11 --- dnmixfac, 
LOOP1:
           ldr         r2, [r1], #4
	   ldr         r4, [r1], #4
	   ldr         r6, [r1], #4
	   ldr         r10, [r1], #4
	   smmul       r2, r2, r11
	   smmul       r4, r4, r11
	   smmul       r6, r6, r11
	   smmul       r10, r10, r11
	   add         r2, r2, r2
	   add         r4, r4, r4
	   add         r6, r6, r6
	   add         r10, r10, r10
	   str         r2, [r3], #4
	   str         r4, [r3], #4
	   str         r6, [r3], #4
	   str         r10, [r3], #4
	   subs        r9, r9, #0x4
	   bgt         LOOP1
	   b           Lable

LOOP2:
           ldr         r2, [r1], #4
	   ldr         r4, [r1], #4
	   ldr         r14, [r3]
	   smmul       r2, r2, r11
	   smmul       r4, r4, r11
	   ldr         r6, [r1], #4
	   ldr         r10, [r1], #4
	   add         r14, r14, r2, LSL #1
	   smmul       r6, r6, r11
	   smmul       r10, r10, r11
	   str         r14, [r3], #4
	   ldr         r2, [r3]
	   add         r2, r2, r4, LSL #1
	   str         r2, [r3], #4
	   ldr         r2, [r3]
	   add         r2, r2, r6, LSL #1
	   str         r2, [r3], #4
	   ldr         r2, [r3]
	   add         r2, r2, r10, LSL #1
	   str         r2, [r3], #4
	   subs        r9, r9, #0x4
	   bgt         LOOP2

Lable:
	   ldr         r2, [sp, #0x4]                    @pull p_aublk
	   ldr         r14, [sp, #0x8]                   @pull &p_aublk->dnmixbufinu[chan]
	   ldr         r6,  [sp, #0xc]                   @pull &p_decparam->outnchans
	   ldr         r10, [sp, #0x10]                  @pull p_aublk->dnmixtab[][] 
           mov         r4, #0x1
           strh        r4, [r14]                         @p_aublk->dnmixbufinu[chan] = 1@

Lable1:
	   
           add         r0, r0, #0x1
	   cmp         r0, r6
	   blt         LOOP

downmix_end:
           add         r13, r13, #0x20
	   ldmfd       r13!, {r4 - r12, r15}

	   @ENDFUNC

Table:
           .word       _chantab

	   @.END


