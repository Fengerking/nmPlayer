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
           .section  .text
	   .global   cifft
	   .extern   brxmix


@Structure, DEC_PARAM , Size 0x7c bytes, from ./hdr/vo_ac3_var.h
.equ       DEC_PARAM_bswitch, 0x10     @  Word16
@End of Structure DEC_PARAM

@void cifft(DEC_PARAM  *p_decparam, BUFF_PARAM *p_buff)
         @r0 --- *p_decparam
	 @r1 --- *p_buff

cifft:
         STMFD         r13!, {r4 - r12, r14}
	 SUB           r13, r13, #0x20
	 LDR           r2, [r0, #DEC_PARAM_bswitch]      @p_decparam->bswitch
	 MOV           r4, r1                            @p_buff->fftbuf
	 CMP           r2, #0
	 BEQ           Lable1
	 @ r4 --- p_buff->fftbuf
	 MOV           r0, #0x40                         @fftn = N/4
	 MOV           r1, #0x3                          @fftnlg2m3 = FFTNLG2M3 - 1
	 B             Lable2
         	   
Lable1:
	 ADD           r3, r4, #0x200                    @fftiptr = p_buff->fftbuf + N/2
	 STR           r4, [sp, #0x8]                    @push fftrptr      
	 STR           r3, [sp, #0x4]                    @push fftiptr
 
	 @r0 --- fftn, r1 --- fftnlg2m3, r2 --- nstep, r3 --- fftiptr, r4 --- fftrptr
@for(m = nstep@ m > 0@ m--) -- the branch one time
         @ Do first Radix-4 Pass
@for(i = fftn/4, i > 0@ i--)

         MOV           r6, #0x20                         @i = fftn/4

LOOP1:	 
         LDR           r2, [r4]                          @ar = *bfyrptr1
	 LDR           r5, [r3]                          @ai = *bfyiptr1
	 LDR           r7, [r4, #0x80]                   @br = *bfyrptr2
	 LDR           r8, [r3, #0x80]                   @bi = *bfyiptr2
	 LDR           r9, [r4, #0x100]                  @cr = *bfyrptr3
	 LDR           r10, [r3, #0x100]                 @ci = *bfyiptr3
	 LDR           r11, [r4, #0x180]                 @dr = *bfyrptr4
	 LDR           r12, [r3, #0x180]                 @di = *bfyiptr4

	 ADD           r0, r2, r9                        @arcr = ar + cr
	 ADD           r1, r7, r11                       @brdr = br + dr
	 ADD           r14, r0, r1
	 SUB           r0, r0, r1

	 STR           r14, [r4]
	 STR           r0, [r4, #0x80]
	 ADD           r0, r5, r10                       @aici = ai + ci
	 ADD           r1, r8, r12                       @bidi = bi + di
	 ADD           r14, r0, r1
	 SUB           r0, r0, r1
	 STR           r14, [r3]
	 STR           r0, [r3, #0x80]

	 SUB           r0, r5, r10                       @aici = ai - ci
	 SUB           r1, r7, r11                       @brdr = br - dr
         ADD           r14, r0, r1
	 SUB           r0, r0, r1
	 STR           r14, [r3, #0x100]
	 STR           r0, [r3, #0x180]

         SUB           r0, r2, r9                        @arcr = ar - cr
	 SUB           r1, r8, r12                       @bidi = bi - di
	 SUB           r14, r0, r1
	 ADD           r0, r0, r1
	 STR           r14, [r4, #0x100]
	 STR           r0, [r4, #0x180]

	 ADD           r3, r3, #4                        @*bfyiptr++
	 ADD           r4, r4, #4                        @*byfrptr++
	 SUBS          r6, r6, #1
	 BGT           LOOP1

	 @Do all Radix-2 passes except first two and last      
         LDR           r3, [sp, #0x4]                    @fftiptr
	 LDR           r4, [sp, #0x8]                    @fftrptr
	 ADR           r5, Table                         @brxmixptr = brxmix
 
         MOV           r7, #0x4                          @j = gp	 
LOOP3:
         MOV           r8, #0x10                         @i = bg = fftn/8
         LDR           r0, [r5], #4                      @cr_t = *brxmixptr++
	 LDR           r1, [r5], #4                      @ci_t = *brxmixptr++

LOOP4:
	 LDR           r11, [r4, #0x40]                  @br
	 LDR           r12, [r3, #0x40]                  @bi
         LDR           r9, [r4]                          @ar
	 LDR           r10, [r3]                         @ai
	 SMMUL         r2, r0, r11
	 SMMUL         r14, r1, r12
	 ADD           r2, r2, r2
	 SUB           r2, r2, r14, LSL #1               @rtemp
	 SMMUL         r14, r1, r11
	 SMMUL         r11, r0, r12
	 ADD           r14, r14, r14
	 ADD           r14, r14, r11, LSL #1             @itemp
	 ADD           r11, r9, r2                       @ar + rtemp
	 ADD           r12, r10, r14                     @ai + itemp
	 STR           r11, [r4, #0x40]                  @*bfyrptr2++
	 STR           r12, [r3, #0x40]                  @*bfyiptr2++
	 SUB           r11, r9, r2                       @ar - rtemp
	 SUB           r12, r10, r14                     @ai - itemp
	 SUBS          r8, r8, #1
	 STR           r11, [r4], #4                     @*bfyrptr1++
	 STR           r12, [r3], #4                     @*bfyiptr1++

	 BGT           LOOP4
	 ADD           r3, r3, #0x40                     @bfyiptr1 += bg
	 ADD           r4, r4, #0x40                     @bfyrptr1 += bg
	 SUBS          r7, r7, #0x1
	 BGT           LOOP3

         LDR           r3, [sp, #0x4]                    @fftiptr
	 LDR           r4, [sp, #0x8]                    @fftrptr
	 ADR           r5, Table                         @brxmixptr = brxmix
 
         MOV           r7, #0x8                          @j = gp	 
LOOP31:
         MOV           r8, #0x8                         @i = bg = fftn/8
         LDR           r0, [r5], #4                      @cr_t = *brxmixptr++
	 LDR           r1, [r5], #4                      @ci_t = *brxmixptr++

LOOP41:
	 LDR           r11, [r4, #0x20]                  @br
	 LDR           r12, [r3, #0x20]                  @bi
         LDR           r9, [r4]                          @ar
	 LDR           r10, [r3]                         @ai
	 SMMUL         r2, r0, r11
	 SMMUL         r14, r1, r12
	 ADD           r2, r2, r2
	 SUB           r2, r2, r14, LSL #1               @rtemp
	 SMMUL         r14, r1, r11
	 SMMUL         r11, r0, r12
	 ADD           r14, r14, r14
	 ADD           r14, r14, r11, LSL #1             @itemp
	 ADD           r11, r9, r2                       @ar + rtemp
	 ADD           r12, r10, r14                     @ai + itemp
	 STR           r11, [r4, #0x20]                  @*bfyrptr2++
	 STR           r12, [r3, #0x20]                  @*bfyiptr2++
	 SUB           r11, r9, r2                       @ar - rtemp
	 SUB           r12, r10, r14                     @ai - itemp
	 SUBS          r8, r8, #1
	 STR           r11, [r4], #4                     @*bfyrptr1++
	 STR           r12, [r3], #4                     @*bfyiptr1++

	 BGT           LOOP41
	 ADD           r3, r3, #0x20                     @bfyiptr1 += bg
	 ADD           r4, r4, #0x20                     @bfyrptr1 += bg
	 SUBS          r7, r7, #0x1
	 BGT           LOOP31

         LDR           r3, [sp, #0x4]                    @fftiptr
	 LDR           r4, [sp, #0x8]                    @fftrptr
	 ADR           r5, Table                         @brxmixptr = brxmix

         MOV           r7, #0x10                          @j = gp	 
LOOP32:
         MOV           r8, #0x4                         @i = bg = fftn/8
         LDR           r0, [r5], #4                      @cr_t = *brxmixptr++
	 LDR           r1, [r5], #4                      @ci_t = *brxmixptr++

LOOP42:
	 LDR           r11, [r4, #0x10]                  @br
	 LDR           r12, [r3, #0x10]                  @bi
         LDR           r9, [r4]                          @ar
	 LDR           r10, [r3]                         @ai
	 SMMUL         r2, r0, r11
	 SMMUL         r14, r1, r12
	 ADD           r2, r2, r2
	 SUB           r2, r2, r14, LSL #1               @rtemp
	 SMMUL         r14, r1, r11
	 SMMUL         r11, r0, r12
	 ADD           r14, r14, r14
	 ADD           r14, r14, r11, LSL #1             @itemp
	 ADD           r11, r9, r2                       @ar + rtemp
	 ADD           r12, r10, r14                     @ai + itemp
	 STR           r11, [r4, #0x10]                  @*bfyrptr2++
	 STR           r12, [r3, #0x10]                  @*bfyiptr2++
	 SUB           r11, r9, r2                       @ar - rtemp
	 SUB           r12, r10, r14                     @ai - itemp
	 SUBS          r8, r8, #1
	 STR           r11, [r4], #4                     @*bfyrptr1++
	 STR           r12, [r3], #4                     @*bfyiptr1++

	 BGT           LOOP42
	 ADD           r3, r3, #0x10                     @bfyiptr1 += bg
	 ADD           r4, r4, #0x10                     @bfyrptr1 += bg
	 SUBS          r7, r7, #0x1
	 BGT           LOOP32
	 
         LDR           r3, [sp, #0x4]                    @fftiptr
	 LDR           r4, [sp, #0x8]                    @fftrptr
	 ADR           r5, Table                         @brxmixptr = brxmix
 
         MOV           r7, #0x20                          @j = gp	 
LOOP33:
         MOV           r8, #0x2                         @i = bg = fftn/8
         LDR           r0, [r5], #4                      @cr_t = *brxmixptr++
	 LDR           r1, [r5], #4                      @ci_t = *brxmixptr++

LOOP43:
	 LDR           r11, [r4, #0x8]                  @br
	 LDR           r12, [r3, #0x8]                  @bi
         LDR           r9, [r4]                          @ar
	 LDR           r10, [r3]                         @ai
	 SMMUL         r2, r0, r11
	 SMMUL         r14, r1, r12
	 ADD           r2, r2, r2
	 SUB           r2, r2, r14, LSL #1               @rtemp
	 SMMUL         r14, r1, r11
	 SMMUL         r11, r0, r12
	 ADD           r14, r14, r14
	 ADD           r14, r14, r11, LSL #1             @itemp
	 ADD           r11, r9, r2                       @ar + rtemp
	 ADD           r12, r10, r14                     @ai + itemp
	 STR           r11, [r4, #0x8]                  @*bfyrptr2++
	 STR           r12, [r3, #0x8]                  @*bfyiptr2++
	 SUB           r11, r9, r2                       @ar - rtemp
	 SUB           r12, r10, r14                     @ai - itemp
	 SUBS          r8, r8, #1
	 STR           r11, [r4], #4                     @*bfyrptr1++
	 STR           r12, [r3], #4                     @*bfyiptr1++

	 BGT           LOOP43
	 ADD           r3, r3, #0x8                     @bfyiptr1 += bg
	 ADD           r4, r4, #0x8                     @bfyrptr1 += bg
	 SUBS          r7, r7, #0x1
	 BGT           LOOP33 

@Do last Radix-2 pass
         LDR           r3, [sp, #0x4]                    @fftiptr
	 LDR           r4, [sp, #0x8]                    @fftrptr
	 ADR           r5, Table                         @brxmixptr = brxmix
         MOV           r0, #0x40                         @i = fftn/2

LOOP5:
         LDR           r6, [r4], #4                      @ar = *bfyrptr1++
	 LDR           r7, [r3], #4                      @ai = *bfyiptr1++
	 LDR           r8, [r4], #-4                     @br = *bfyrptr1--
	 LDR           r9, [r3], #-4                     @bi = *bfyiptr1--
	 LDR           r10, [r5], #4                     @cr_t = *brxmixptr++
	 LDR           r11, [r5], #4                     @ci_t = *brxmixptr++

	 SMMUL         r12, r10, r8
	 SMMUL         r14, r11, r9
	 ADD           r12, r12, r12
	 SUB           r12, r12, r14, LSL #1             @rtemp

	 SMMUL         r14, r11, r8
	 SMMUL         r8, r10, r9
	 ADD           r14, r14, r14
	 ADD           r14, r14, r8, LSL #1              @itemp

	 SUB           r8, r6, r12
	 SUB           r9, r7, r14
	 ADD           r10, r6, r12
	 ADD           r11, r7, r14
	 STR           r8, [r4], #4
	 STR           r9, [r3], #4
	 STR           r10, [r4], #4
	 STR           r11, [r3], #4
	 SUBS          r0, r0, #0x1
	 BGT           LOOP5
	 B             cifft_end

Lable2:
	 ADD           r3, r4, #0x300                    @fftiptr = p_buff->fftbuf + N/2
	 STR           r4, [sp, #0x8]                    @push fftrptr      
	 STR           r3, [sp, #0x4]                    @push fftiptr

	 MOV           r0, #2
	 STR           r0, [sp, #0x10]                   @ nstep

@for(m = nstep@ m > 0@ m--) -- the branch one time

SW_LOOP:

         @ Do first Radix-4 Pass
@for(i = fftn/4, i > 0@ i--)

         MOV           r6, #0x10                         @i = fftn/4
         LDR           r4, [sp, #0x8]
         LDR           r3, [sp, #0x4]
SW_LOOP1:	 
         LDR           r2, [r4]                          @ar = *bfyrptr1
	 LDR           r5, [r3]                          @ai = *bfyiptr1
	 LDR           r7, [r4, #0x40]                   @br = *bfyrptr2
	 LDR           r8, [r3, #0x40]                   @bi = *bfyiptr2
	 LDR           r9, [r4, #0x80]                   @cr = *bfyrptr3
	 LDR           r10, [r3, #0x80]                  @ci = *bfyiptr3
	 LDR           r11, [r4, #0xC0]                  @dr = *bfyrptr4
	 LDR           r12, [r3, #0xC0]                  @di = *bfyiptr4

	 ADD           r0, r2, r9                        @arcr = ar + cr
	 ADD           r1, r7, r11                       @brdr = br + dr
	 ADD           r14, r0, r1
	 SUB           r0, r0, r1

	 STR           r14, [r4]
	 STR           r0, [r4, #0x40]
	 ADD           r0, r5, r10                       @aici = ai + ci
	 ADD           r1, r8, r12                       @bidi = bi + di
	 ADD           r14, r0, r1
	 SUB           r0, r0, r1
	 STR           r14, [r3]
	 STR           r0, [r3, #0x40]

	 SUB           r0, r5, r10                       @aici = ai - ci
	 SUB           r1, r7, r11                       @brdr = br - dr
         ADD           r14, r0, r1
	 SUB           r0, r0, r1
	 STR           r14, [r3, #0x80]
	 STR           r0, [r3, #0xC0]

         SUB           r0, r2, r9                        @arcr = ar - cr
	 SUB           r1, r8, r12                       @bidi = bi - di
	 SUB           r14, r0, r1
	 ADD           r0, r0, r1
	 STR           r14, [r4, #0x80]
	 STR           r0, [r4, #0xC0]

	 ADD           r3, r3, #4                        @*bfyiptr++
	 ADD           r4, r4, #4                        @*byfrptr++
	 SUBS          r6, r6, #1
	 BGT           SW_LOOP1
	 B				CIFFT_TABLE_BACK
	 
Table:
         @.word           brxmix
	.word	0x80000001,   0x00000000,   0x00000000,   0x80000001,   0xa57d8667,   0xa57d8667,   0x5a827999,   0xa57d8667   
	.word	0x89be50c5,   0xcf043ab4,   0x30fbc54c,   0x89be50c5,   0xcf043ab4,   0x89be50c5,   0x7641af3b,   0xcf043ab4   
	.word	0x8275a0c2,   0xe70747c4,   0x18f8b83c,   0x8275a0c2,   0xb8e3131a,   0x9592675d,   0x6a6d98a3,   0xb8e3131a   
	.word	0x9592675d,   0xb8e3131a,   0x471cece6,   0x9592675d,   0xe70747c4,   0x8275a0c2,   0x7d8a5f3e,   0xe70747c4   
	.word	0x809dc972,   0xf3742ca3,   0x0c8bd35d,   0x809dc972,   0xaecc336d,   0x9d0dfe55,   0x62f201ab,   0xaecc336d   
	.word	0x8f1d343b,   0xc3a94591,   0x3c56ba6f,   0x8f1d343b,   0xdad7f3a3,   0x8582faa6,   0x7a7d055a,   0xdad7f3a3   
	.word	0x8582faa6,   0xdad7f3a3,   0x25280c5d,   0x8582faa6,   0xc3a94591,   0x8f1d343b,   0x70e2cbc5,   0xc3a94591   
	.word	0x9d0dfe55,   0xaecc336d,   0x5133cc93,   0x9d0dfe55,   0xf3742ca3,   0x809dc972,   0x7f62368e,   0xf3742ca3   
	.word	0x80277874,   0xf9b82684,   0x0647d97c,   0x80277874,   0xaa0a5b2f,   0xa1288378,   0x5ed77c88,   0xaa0a5b2f   
	.word	0x8c4a1430,   0xc945dfed,   0x36ba2013,   0x8c4a1430,   0xd4e0cb15,   0x877b7bee,   0x78848412,   0xd4e0cb15   
	.word	0x83d60413,   0xe0e60686,   0x1f19f97a,   0x83d60413,   0xbe31e19c,   0x9235f2ed,   0x6dca0d13,   0xbe31e19c   
	.word	0x99307ee1,   0xb3c0200e,   0x4c3fdff2,   0x99307ee1,   0xed37ef92,   0x8162aa05,   0x7e9d55fb,   0xed37ef92   
	.word	0x8162aa05,   0xed37ef92,   0x12c8106e,   0x8162aa05,   0xb3c0200e,   0x99307ee1,   0x66cf811f,   0xb3c0200e   
	.word	0x9235f2ed,   0xbe31e19c,   0x41ce1e64,   0x9235f2ed,   0xe0e60686,   0x83d60413,   0x7c29fbed,   0xe0e60686   
	.word	0x877b7bee,   0xd4e0cb15,   0x2b1f34eb,   0x877b7bee,   0xc945dfed,   0x8c4a1430,   0x73b5ebd0,   0xc945dfed   
	.word	0xa1288378,   0xaa0a5b2f,   0x55f5a4d1,   0xa1288378,   0xf9b82684,   0x80277874,   0x7fd8878c,   0xf9b82684 

CIFFT_TABLE_BACK:	
	 @Do all Radix-2 passes except first two and last      
         LDR           r3, [sp, #0x4]                    @fftiptr
	 LDR           r4, [sp, #0x8]                    @fftrptr
	 ADR           r5, Table                         @brxmixptr = brxmix
 
         MOV           r7, #0x4                          @j = gp	 
SW_LOOP3:
         MOV           r8, #0x8                          @i = bg = fftn/8
         LDR           r0, [r5], #4                      @cr_t = *brxmixptr++
	 LDR           r1, [r5], #4                      @ci_t = *brxmixptr++

SW_LOOP4:
	 LDR           r11, [r4, #0x20]                  @br
	 LDR           r12, [r3, #0x20]                  @bi
         LDR           r9, [r4]                          @ar
	 LDR           r10, [r3]                         @ai
	 SMMUL         r2, r0, r11
	 SMMUL         r14, r1, r12
	 ADD           r2, r2, r2
	 SUB           r2, r2, r14, LSL #1               @rtemp
	 SMMUL         r14, r1, r11
	 SMMUL         r11, r0, r12
	 ADD           r14, r14, r14
	 ADD           r14, r14, r11, LSL #1             @itemp
	 ADD           r11, r9, r2                       @ar + rtemp
	 ADD           r12, r10, r14                     @ai + itemp
	 STR           r11, [r4, #0x20]                  @*bfyrptr2++
	 STR           r12, [r3, #0x20]                  @*bfyiptr2++
	 SUB           r11, r9, r2                       @ar - rtemp
	 SUB           r12, r10, r14                     @ai - itemp
	 SUBS          r8, r8, #1
	 STR           r11, [r4], #4                     @*bfyrptr1++
	 STR           r12, [r3], #4                     @*bfyiptr1++

	 BGT           SW_LOOP4
	 ADD           r3, r3, #0x20                     @bfyiptr1 += bg
	 ADD           r4, r4, #0x20                     @bfyrptr1 += bg
	 SUBS          r7, r7, #0x1
	 BGT           SW_LOOP3

         LDR           r3, [sp, #0x4]                    @fftiptr
	 LDR           r4, [sp, #0x8]                    @fftrptr
	 ADR           r5, Table                         @brxmixptr = brxmix
 
         MOV           r7, #0x8                          @j SW_LOOP5= gp

SW_LOOP31:
         MOV           r8, #0x4                         @i = bg = fftn/8
         LDR           r0, [r5], #4                      @cr_t = *brxmixptr++
	 LDR           r1, [r5], #4                      @ci_t = *brxmixptr++

SW_LOOP41:
	 LDR           r11, [r4, #0x10]                  @br
	 LDR           r12, [r3, #0x10]                  @bi
         LDR           r9, [r4]                          @ar
	 LDR           r10, [r3]                         @ai
	 SMMUL         r2, r0, r11
	 SMMUL         r14, r1, r12
	 ADD           r2, r2, r2
	 SUB           r2, r2, r14, LSL #1               @rtemp
	 SMMUL         r14, r1, r11
	 SMMUL         r11, r0, r12
	 ADD           r14, r14, r14
	 ADD           r14, r14, r11, LSL #1             @itemp
	 ADD           r11, r9, r2                       @ar + rtemp
	 ADD           r12, r10, r14                     @ai + itemp
	 STR           r11, [r4, #0x10]                  @*bfyrptr2++
	 STR           r12, [r3, #0x10]                  @*bfyiptr2++
	 SUB           r11, r9, r2                       @ar - rtemp
	 SUB           r12, r10, r14                     @ai - itemp
	 SUBS          r8, r8, #1
	 STR           r11, [r4], #4                     @*bfyrptr1++
	 STR           r12, [r3], #4                     @*bSW_LOOP5fyiptr1++

	 BGT           SW_LOOP41
	 ADD           r3, r3, #0x10                     @bfyiptr1 += bg
	 ADD           r4, r4, #0x10                     @bfyrptr1 += bg
	 SUBS          r7, r7, #0x1
	 BGT           SW_LOOP31

         LDR           r3, [sp, #0x4]                    @fftiptr
	 LDR           r4, [sp, #0x8]                    @ffSW_LOOP5trptr
	 ADR           r5, Table                         @brxmixptr = brxmix
 
         MOV           r7, #0x10                          @j = gp

SW_LOOP32:
         MOV           r8, #0x2                         @i = bg = fftn/8
         LDR           r0, [r5], #4                      @crSW_LOOP5_t = *brxmixptr++
	 LDR           r1, [r5], #4                      @ci_t = *brxmixptr++

SW_LOOP42:
	 LDR           r11, [r4, #0x8]                  @br
	 LDR           r12, [r3, #0x8]                  @bi
         LDR           r9, [r4]                          @ar
	 LDR           r10, [r3]                         @ai
	 SMMUL         r2, r0, r11
	 SMMUL         r14, r1, r12
	 ADD           r2, r2, r2
	 SUB           r2, r2, r14, LSL #1               @rtemp
	 SMMUL         r14, r1, r11
	 SMMUL         r11, r0, r12
	 ADD           r14, r14, r14
	 ADD           r14, r14, r11, LSL #1             @itemp
	 ADD           r11, r9, r2                       @ar + rtemp
	 ADD           r12, r10, r14                     @ai + itemp
	 STR           r11, [r4, #0x8]                  @*bfyrptr2++
	 STR           r12, [r3, #0x8]                  @*bfyiptr2++
	 SUB           r11, r9, r2                       @ar - rtemp
	 SUB           r12, r10, r14                     @ai - itemp
	 SUBS          r8, r8, #1
	 STR           r11, [r4], #4                     @*bfyrptr1++
	 STR           r12, [r3], #4                     @*bfyiptr1++

	 BGT           SW_LOOP42
	 ADD           r3, r3, #0x8                     @bfyiptr1 += bg
	 ADD           r4, r4, #0x8                     @bfyrptr1 += bg
	 SUBS          r7, r7, #0x1
	 BGT           SW_LOOP32
	 
@Do last Radix-2 pass
         LDR           r3, [sp, #0x4]                    @fftiptr
	 LDR           r4, [sp, #0x8]                    @fftrptr
	 ADR           r5, Table                         @brxmixptr = brxmix
         MOV           r0, #0x20                         @i = fftn/2

SW_LOOP5:
         LDR           r6, [r4], #4                      @ar = *bfyrptr1++
	 LDR           r7, [r3], #4                      @ai = *bfyiptr1++
	 LDR           r8, [r4], #-4                     @br = *bfyrptr1--
	 LDR           r9, [r3], #-4                     @bi = *bfyiptr1--
	 LDR           r10, [r5], #4                     @cr_t = *brxmixptr++
	 LDR           r11, [r5], #4                     @ci_t = *brxmixptr++

	 SMMUL         r12, r10, r8
	 SMMUL         r14, r11, r9
	 ADD           r12, r12, r12
	 SUB           r12, r12, r14, LSL #1             @rtemp

	 SMMUL         r14, r11, r8
	 SMMUL         r8, r10, r9
	 ADD           r14, r14, r14
	 ADD           r14, r14, r8, LSL #1              @itemp

	 SUB           r8, r6, r12
	 SUB           r9, r7, r14
	 ADD           r10, r6, r12
	 ADD           r11, r7, r14
	 STR           r8, [r4], #4
	 STR           r9, [r3], #4
	 STR           r10, [r4], #4
	 STR           r11, [r3], #4
	 SUBS          r0, r0, #0x1
	 BGT           SW_LOOP5

	 LDR           r0, [sp, #0x10]
	 LDR           r4, [sp, #0x8]                    @p_buff->fftbuf
	 SUBS          r0, r0, #1
	 ADD           r6, r4, #0x100                    @p_buff->fftbuf + N/4
	 ADD           r7, r4, #0x200                    @p_buff->fftbuf + N/2
	 STR           r6, [sp, #0x8]
	 STR           r7, [sp, #0x4]
	 STR           r0, [sp, #0x10]
	 BGT           SW_LOOP


cifft_end:
         ADD           r13, r13, #0x20
	 LDMFD         r13!, {r4 - r12, r15}
	 @ENDFUNC


	 .END



