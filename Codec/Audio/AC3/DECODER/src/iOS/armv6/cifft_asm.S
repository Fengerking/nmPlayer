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
	   .globl   _cifft
	   .globl   _brxmix


@Structure, DEC_PARAM , Size 0x7c bytes, from ./hdr/vo_ac3_var.h
.set       DEC_PARAM_bswitch, 0x10     @  Word16
@End of Structure DEC_PARAM

@void cifft(DEC_PARAM  *p_decparam, BUFF_PARAM *p_buff)
         @r0 --- *p_decparam
	 @r1 --- *p_buff

_cifft:
         stmfd         r13!, {r4 - r12, r14}
	 sub           r13, r13, #0x20
	 ldr           r2, [r0, #DEC_PARAM_bswitch]      @p_decparam->bswitch
	 mov           r4, r1                            @p_buff->fftbuf
	 cmp           r2, #0
	 beq           Lable1
	 @ r4 --- p_buff->fftbuf
	 mov           r0, #0x40                         @fftn = N/4
	 mov           r1, #0x3                          @fftnlg2m3 = FFTNLG2M3 - 1
	 b             Lable2
         	   
Lable1:
	 add           r3, r4, #0x200                    @fftiptr = p_buff->fftbuf + N/2
	 str           r4, [sp, #0x8]                    @push fftrptr      
	 str           r3, [sp, #0x4]                    @push fftiptr
 
	 @r0 --- fftn, r1 --- fftnlg2m3, r2 --- nstep, r3 --- fftiptr, r4 --- fftrptr
@for(m = nstep@ m > 0@ m--) -- the branch one time
         @ Do first Radix-4 Pass
@for(i = fftn/4, i > 0@ i--)

         mov           r6, #0x20                         @i = fftn/4

LOOP1:	 
         ldr           r2, [r4]                          @ar = *bfyrptr1
	 ldr           r5, [r3]                          @ai = *bfyiptr1
	 ldr           r7, [r4, #0x80]                   @br = *bfyrptr2
	 ldr           r8, [r3, #0x80]                   @bi = *bfyiptr2
	 ldr           r9, [r4, #0x100]                  @cr = *bfyrptr3
	 ldr           r10, [r3, #0x100]                 @ci = *bfyiptr3
	 ldr           r11, [r4, #0x180]                 @dr = *bfyrptr4
	 ldr           r12, [r3, #0x180]                 @di = *bfyiptr4

	 add           r0, r2, r9                        @arcr = ar + cr
	 add           r1, r7, r11                       @brdr = br + dr
	 add           r14, r0, r1
	 sub           r0, r0, r1

	 str           r14, [r4]
	 str           r0, [r4, #0x80]
	 add           r0, r5, r10                       @aici = ai + ci
	 add           r1, r8, r12                       @bidi = bi + di
	 add           r14, r0, r1
	 sub           r0, r0, r1
	 str           r14, [r3]
	 str           r0, [r3, #0x80]

	 sub           r0, r5, r10                       @aici = ai - ci
	 sub           r1, r7, r11                       @brdr = br - dr
         add           r14, r0, r1
	 sub           r0, r0, r1
	 str           r14, [r3, #0x100]
	 str           r0, [r3, #0x180]

         sub           r0, r2, r9                        @arcr = ar - cr
	 sub           r1, r8, r12                       @bidi = bi - di
	 sub           r14, r0, r1
	 add           r0, r0, r1
	 str           r14, [r4, #0x100]
	 str           r0, [r4, #0x180]

	 add           r3, r3, #4                        @*bfyiptr++
	 add           r4, r4, #4                        @*byfrptr++
	 subs          r6, r6, #1
	 bgt           LOOP1

	 @Do all Radix-2 passes except first two and last      
         ldr           r3, [sp, #0x4]                    @fftiptr
	 ldr           r4, [sp, #0x8]                    @fftrptr
	 ldr           r5, Table                         @brxmixptr = brxmix
 
         mov           r7, #0x4                          @j = gp	 
LOOP3:
         mov           r8, #0x10                         @i = bg = fftn/8
         ldr           r0, [r5], #4                      @cr_t = *brxmixptr++
	 ldr           r1, [r5], #4                      @ci_t = *brxmixptr++

LOOP4:
	 ldr           r11, [r4, #0x40]                  @br
	 ldr           r12, [r3, #0x40]                  @bi
         ldr           r9, [r4]                          @ar
	 ldr           r10, [r3]                         @ai
	 smmul         r2, r0, r11
	 smmul         r14, r1, r12
	 add           r2, r2, r2
	 sub           r2, r2, r14, LSL #1               @rtemp
	 smmul         r14, r1, r11
	 smmul         r11, r0, r12
	 add           r14, r14, r14
	 add           r14, r14, r11, LSL #1             @itemp
	 add           r11, r9, r2                       @ar + rtemp
	 add           r12, r10, r14                     @ai + itemp
	 str           r11, [r4, #0x40]                  @*bfyrptr2++
	 str           r12, [r3, #0x40]                  @*bfyiptr2++
	 sub           r11, r9, r2                       @ar - rtemp
	 sub           r12, r10, r14                     @ai - itemp
	 subs          r8, r8, #1
	 str           r11, [r4], #4                     @*bfyrptr1++
	 str           r12, [r3], #4                     @*bfyiptr1++

	 bgt           LOOP4
	 add           r3, r3, #0x40                     @bfyiptr1 += bg
	 add           r4, r4, #0x40                     @bfyrptr1 += bg
	 subs          r7, r7, #0x1
	 bgt           LOOP3

         ldr           r3, [sp, #0x4]                    @fftiptr
	 ldr           r4, [sp, #0x8]                    @fftrptr
	 ldr           r5, Table                         @brxmixptr = brxmix
 
         mov           r7, #0x8                          @j = gp	 
LOOP31:
         mov           r8, #0x8                         @i = bg = fftn/8
         ldr           r0, [r5], #4                      @cr_t = *brxmixptr++
	 ldr           r1, [r5], #4                      @ci_t = *brxmixptr++

LOOP41:
	 ldr           r11, [r4, #0x20]                  @br
	 ldr           r12, [r3, #0x20]                  @bi
         ldr           r9, [r4]                          @ar
	 ldr           r10, [r3]                         @ai
	 smmul         r2, r0, r11
	 smmul         r14, r1, r12
	 add           r2, r2, r2
	 sub           r2, r2, r14, LSL #1               @rtemp
	 smmul         r14, r1, r11
	 smmul         r11, r0, r12
	 add           r14, r14, r14
	 add           r14, r14, r11, LSL #1             @itemp
	 add           r11, r9, r2                       @ar + rtemp
	 add           r12, r10, r14                     @ai + itemp
	 str           r11, [r4, #0x20]                  @*bfyrptr2++
	 str           r12, [r3, #0x20]                  @*bfyiptr2++
	 sub           r11, r9, r2                       @ar - rtemp
	 sub           r12, r10, r14                     @ai - itemp
	 subs          r8, r8, #1
	 str           r11, [r4], #4                     @*bfyrptr1++
	 str           r12, [r3], #4                     @*bfyiptr1++

	 bgt           LOOP41
	 add           r3, r3, #0x20                     @bfyiptr1 += bg
	 add           r4, r4, #0x20                     @bfyrptr1 += bg
	 subs          r7, r7, #0x1
	 bgt           LOOP31

         ldr           r3, [sp, #0x4]                    @fftiptr
	 ldr           r4, [sp, #0x8]                    @fftrptr
	 ldr           r5, Table                         @brxmixptr = brxmix
 
         mov           r7, #0x10                          @j = gp	 
LOOP32:
         mov           r8, #0x4                         @i = bg = fftn/8
         ldr           r0, [r5], #4                      @cr_t = *brxmixptr++
	 ldr           r1, [r5], #4                      @ci_t = *brxmixptr++

LOOP42:
	 ldr           r11, [r4, #0x10]                  @br
	 ldr           r12, [r3, #0x10]                  @bi
         ldr           r9, [r4]                          @ar
	 ldr           r10, [r3]                         @ai
	 smmul         r2, r0, r11
	 smmul         r14, r1, r12
	 add           r2, r2, r2
	 sub           r2, r2, r14, LSL #1               @rtemp
	 smmul         r14, r1, r11
	 smmul         r11, r0, r12
	 add           r14, r14, r14
	 add           r14, r14, r11, LSL #1             @itemp
	 add           r11, r9, r2                       @ar + rtemp
	 add           r12, r10, r14                     @ai + itemp
	 str           r11, [r4, #0x10]                  @*bfyrptr2++
	 str           r12, [r3, #0x10]                  @*bfyiptr2++
	 sub           r11, r9, r2                       @ar - rtemp
	 sub           r12, r10, r14                     @ai - itemp
	 subs          r8, r8, #1
	 str           r11, [r4], #4                     @*bfyrptr1++
	 str           r12, [r3], #4                     @*bfyiptr1++

	 bgt           LOOP42
	 add           r3, r3, #0x10                     @bfyiptr1 += bg
	 add           r4, r4, #0x10                     @bfyrptr1 += bg
	 subs          r7, r7, #0x1
	 bgt           LOOP32
	 
         ldr           r3, [sp, #0x4]                    @fftiptr
	 ldr           r4, [sp, #0x8]                    @fftrptr
	 ldr           r5, Table                         @brxmixptr = brxmix
 
         mov           r7, #0x20                          @j = gp	 
LOOP33:
         mov           r8, #0x2                         @i = bg = fftn/8
         ldr           r0, [r5], #4                      @cr_t = *brxmixptr++
	 ldr           r1, [r5], #4                      @ci_t = *brxmixptr++

LOOP43:
	 ldr           r11, [r4, #0x8]                  @br
	 ldr           r12, [r3, #0x8]                  @bi
         ldr           r9, [r4]                          @ar
	 ldr           r10, [r3]                         @ai
	 smmul         r2, r0, r11
	 smmul         r14, r1, r12
	 add           r2, r2, r2
	 sub           r2, r2, r14, LSL #1               @rtemp
	 smmul         r14, r1, r11
	 smmul         r11, r0, r12
	 add           r14, r14, r14
	 add           r14, r14, r11, LSL #1             @itemp
	 add           r11, r9, r2                       @ar + rtemp
	 add           r12, r10, r14                     @ai + itemp
	 str           r11, [r4, #0x8]                  @*bfyrptr2++
	 str           r12, [r3, #0x8]                  @*bfyiptr2++
	 sub           r11, r9, r2                       @ar - rtemp
	 sub           r12, r10, r14                     @ai - itemp
	 subs          r8, r8, #1
	 str           r11, [r4], #4                     @*bfyrptr1++
	 str           r12, [r3], #4                     @*bfyiptr1++

	 bgt           LOOP43
	 add           r3, r3, #0x8                     @bfyiptr1 += bg
	 add           r4, r4, #0x8                     @bfyrptr1 += bg
	 subs          r7, r7, #0x1
	 bgt           LOOP33 

@Do last Radix-2 pass
         ldr           r3, [sp, #0x4]                    @fftiptr
	 ldr           r4, [sp, #0x8]                    @fftrptr
	 ldr           r5, Table                         @brxmixptr = brxmix
         mov           r0, #0x40                         @i = fftn/2

LOOP5:
         ldr           r6, [r4], #4                      @ar = *bfyrptr1++
	 ldr           r7, [r3], #4                      @ai = *bfyiptr1++
	 ldr           r8, [r4], #-4                     @br = *bfyrptr1--
	 ldr           r9, [r3], #-4                     @bi = *bfyiptr1--
	 ldr           r10, [r5], #4                     @cr_t = *brxmixptr++
	 ldr           r11, [r5], #4                     @ci_t = *brxmixptr++

	 smmul         r12, r10, r8
	 smmul         r14, r11, r9
	 add           r12, r12, r12
	 sub           r12, r12, r14, LSL #1             @rtemp

	 smmul         r14, r11, r8
	 smmul         r8, r10, r9
	 add           r14, r14, r14
	 add           r14, r14, r8, LSL #1              @itemp

	 sub           r8, r6, r12
	 sub           r9, r7, r14
	 add           r10, r6, r12
	 add           r11, r7, r14
	 str           r8, [r4], #4
	 str           r9, [r3], #4
	 str           r10, [r4], #4
	 str           r11, [r3], #4
	 subs          r0, r0, #0x1
	 bgt           LOOP5
	 b             cifft_end

Lable2:
	 add           r3, r4, #0x300                    @fftiptr = p_buff->fftbuf + N/2
	 str           r4, [sp, #0x8]                    @push fftrptr      
	 str           r3, [sp, #0x4]                    @push fftiptr

	 mov           r0, #2
	 str           r0, [sp, #0x10]                   @ nstep

@for(m = nstep@ m > 0@ m--) -- the branch one time

SW_LOOP:

         @ Do first Radix-4 Pass
@for(i = fftn/4, i > 0@ i--)

         mov           r6, #0x10                         @i = fftn/4
         ldr           r4, [sp, #0x8]
         ldr           r3, [sp, #0x4]
SW_LOOP1:	 
         ldr           r2, [r4]                          @ar = *bfyrptr1
	 ldr           r5, [r3]                          @ai = *bfyiptr1
	 ldr           r7, [r4, #0x40]                   @br = *bfyrptr2
	 ldr           r8, [r3, #0x40]                   @bi = *bfyiptr2
	 ldr           r9, [r4, #0x80]                   @cr = *bfyrptr3
	 ldr           r10, [r3, #0x80]                  @ci = *bfyiptr3
	 ldr           r11, [r4, #0xC0]                  @dr = *bfyrptr4
	 ldr           r12, [r3, #0xC0]                  @di = *bfyiptr4

	 add           r0, r2, r9                        @arcr = ar + cr
	 add           r1, r7, r11                       @brdr = br + dr
	 add           r14, r0, r1
	 sub           r0, r0, r1

	 str           r14, [r4]
	 str           r0, [r4, #0x40]
	 add           r0, r5, r10                       @aici = ai + ci
	 add           r1, r8, r12                       @bidi = bi + di
	 add           r14, r0, r1
	 sub           r0, r0, r1
	 str           r14, [r3]
	 str           r0, [r3, #0x40]

	 sub           r0, r5, r10                       @aici = ai - ci
	 sub           r1, r7, r11                       @brdr = br - dr
         add           r14, r0, r1
	 sub           r0, r0, r1
	 str           r14, [r3, #0x80]
	 str           r0, [r3, #0xC0]

         sub           r0, r2, r9                        @arcr = ar - cr
	 sub           r1, r8, r12                       @bidi = bi - di
	 sub           r14, r0, r1
	 add           r0, r0, r1
	 str           r14, [r4, #0x80]
	 str           r0, [r4, #0xC0]

	 add           r3, r3, #4                        @*bfyiptr++
	 add           r4, r4, #4                        @*byfrptr++
	 subs          r6, r6, #1
	 bgt           SW_LOOP1

	 @Do all Radix-2 passes except first two and last      
         ldr           r3, [sp, #0x4]                    @fftiptr
	 ldr           r4, [sp, #0x8]                    @fftrptr
	 ldr           r5, Table                         @brxmixptr = brxmix
 
         mov           r7, #0x4                          @j = gp	 
SW_LOOP3:
         mov           r8, #0x8                          @i = bg = fftn/8
         ldr           r0, [r5], #4                      @cr_t = *brxmixptr++
	 ldr           r1, [r5], #4                      @ci_t = *brxmixptr++

SW_LOOP4:
	 ldr           r11, [r4, #0x20]                  @br
	 ldr           r12, [r3, #0x20]                  @bi
         ldr           r9, [r4]                          @ar
	 ldr           r10, [r3]                         @ai
	 smmul         r2, r0, r11
	 smmul         r14, r1, r12
	 add           r2, r2, r2
	 sub           r2, r2, r14, LSL #1               @rtemp
	 smmul         r14, r1, r11
	 smmul         r11, r0, r12
	 add           r14, r14, r14
	 add           r14, r14, r11, LSL #1             @itemp
	 add           r11, r9, r2                       @ar + rtemp
	 add           r12, r10, r14                     @ai + itemp
	 str           r11, [r4, #0x20]                  @*bfyrptr2++
	 str           r12, [r3, #0x20]                  @*bfyiptr2++
	 sub           r11, r9, r2                       @ar - rtemp
	 sub           r12, r10, r14                     @ai - itemp
	 subs          r8, r8, #1
	 str           r11, [r4], #4                     @*bfyrptr1++
	 str           r12, [r3], #4                     @*bfyiptr1++

	 bgt           SW_LOOP4
	 add           r3, r3, #0x20                     @bfyiptr1 += bg
	 add           r4, r4, #0x20                     @bfyrptr1 += bg
	 subs          r7, r7, #0x1
	 bgt           SW_LOOP3

         ldr           r3, [sp, #0x4]                    @fftiptr
	 ldr           r4, [sp, #0x8]                    @fftrptr
	 ldr           r5, Table                         @brxmixptr = brxmix
 
         mov           r7, #0x8                          @j SW_LOOP5= gp

SW_LOOP31:
         mov           r8, #0x4                         @i = bg = fftn/8
         ldr           r0, [r5], #4                      @cr_t = *brxmixptr++
	 ldr           r1, [r5], #4                      @ci_t = *brxmixptr++

SW_LOOP41:
	 ldr           r11, [r4, #0x10]                  @br
	 ldr           r12, [r3, #0x10]                  @bi
         ldr           r9, [r4]                          @ar
	 ldr           r10, [r3]                         @ai
	 smmul         r2, r0, r11
	 smmul         r14, r1, r12
	 add           r2, r2, r2
	 sub           r2, r2, r14, LSL #1               @rtemp
	 smmul         r14, r1, r11
	 smmul         r11, r0, r12
	 add           r14, r14, r14
	 add           r14, r14, r11, LSL #1             @itemp
	 add           r11, r9, r2                       @ar + rtemp
	 add           r12, r10, r14                     @ai + itemp
	 str           r11, [r4, #0x10]                  @*bfyrptr2++
	 str           r12, [r3, #0x10]                  @*bfyiptr2++
	 sub           r11, r9, r2                       @ar - rtemp
	 sub           r12, r10, r14                     @ai - itemp
	 subs          r8, r8, #1
	 str           r11, [r4], #4                     @*bfyrptr1++
	 str           r12, [r3], #4                     @*bSW_LOOP5fyiptr1++

	 bgt           SW_LOOP41
	 add           r3, r3, #0x10                     @bfyiptr1 += bg
	 add           r4, r4, #0x10                     @bfyrptr1 += bg
	 subs          r7, r7, #0x1
	 bgt           SW_LOOP31

         ldr           r3, [sp, #0x4]                    @fftiptr
	 ldr           r4, [sp, #0x8]                    @ffSW_LOOP5trptr
	 ldr           r5, Table                         @brxmixptr = brxmix
 
         mov           r7, #0x10                          @j = gp

SW_LOOP32:
         mov           r8, #0x2                         @i = bg = fftn/8
         ldr           r0, [r5], #4                      @crSW_LOOP5_t = *brxmixptr++
	 ldr           r1, [r5], #4                      @ci_t = *brxmixptr++

SW_LOOP42:
	 ldr           r11, [r4, #0x8]                  @br
	 ldr           r12, [r3, #0x8]                  @bi
         ldr           r9, [r4]                          @ar
	 ldr           r10, [r3]                         @ai
	 smmul         r2, r0, r11
	 smmul         r14, r1, r12
	 add           r2, r2, r2
	 sub           r2, r2, r14, LSL #1               @rtemp
	 smmul         r14, r1, r11
	 smmul         r11, r0, r12
	 add           r14, r14, r14
	 add           r14, r14, r11, LSL #1             @itemp
	 add           r11, r9, r2                       @ar + rtemp
	 add           r12, r10, r14                     @ai + itemp
	 str           r11, [r4, #0x8]                  @*bfyrptr2++
	 str           r12, [r3, #0x8]                  @*bfyiptr2++
	 sub           r11, r9, r2                       @ar - rtemp
	 sub           r12, r10, r14                     @ai - itemp
	 subs          r8, r8, #1
	 str           r11, [r4], #4                     @*bfyrptr1++
	 str           r12, [r3], #4                     @*bfyiptr1++

	 bgt           SW_LOOP42
	 add           r3, r3, #0x8                     @bfyiptr1 += bg
	 add           r4, r4, #0x8                     @bfyrptr1 += bg
	 subs          r7, r7, #0x1
	 bgt           SW_LOOP32
	 
@Do last Radix-2 pass
         ldr           r3, [sp, #0x4]                    @fftiptr
	 ldr           r4, [sp, #0x8]                    @fftrptr
	 ldr           r5, Table                         @brxmixptr = brxmix
         mov           r0, #0x20                         @i = fftn/2

SW_LOOP5:
         ldr           r6, [r4], #4                      @ar = *bfyrptr1++
	 ldr           r7, [r3], #4                      @ai = *bfyiptr1++
	 ldr           r8, [r4], #-4                     @br = *bfyrptr1--
	 ldr           r9, [r3], #-4                     @bi = *bfyiptr1--
	 ldr           r10, [r5], #4                     @cr_t = *brxmixptr++
	 ldr           r11, [r5], #4                     @ci_t = *brxmixptr++

	 smmul         r12, r10, r8
	 smmul         r14, r11, r9
	 add           r12, r12, r12
	 sub           r12, r12, r14, LSL #1             @rtemp

	 smmul         r14, r11, r8
	 smmul         r8, r10, r9
	 add           r14, r14, r14
	 add           r14, r14, r8, LSL #1              @itemp

	 sub           r8, r6, r12
	 sub           r9, r7, r14
	 add           r10, r6, r12
	 add           r11, r7, r14
	 str           r8, [r4], #4
	 str           r9, [r3], #4
	 str           r10, [r4], #4
	 str           r11, [r3], #4
	 subs          r0, r0, #0x1
	 bgt           SW_LOOP5

	 ldr           r0, [sp, #0x10]
	 ldr           r4, [sp, #0x8]                    @p_buff->fftbuf
	 subs          r0, r0, #1
	 add           r6, r4, #0x100                    @p_buff->fftbuf + N/4
	 add           r7, r4, #0x200                    @p_buff->fftbuf + N/2
	 str           r6, [sp, #0x8]
	 str           r7, [sp, #0x4]
	 str           r0, [sp, #0x10]
	 bgt           SW_LOOP


cifft_end:
         add           r13, r13, #0x20
	 ldmfd         r13!, {r4 - r12, r15}
	 @ENDFUNC

Table:
         .word         _brxmix

	 @.END



