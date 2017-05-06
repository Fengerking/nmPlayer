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
	 .globl   _idctsc
	 .globl   _idctsc2
	 .globl   _z1mix
	 .globl   _z2mix
	 .globl   _bitrevary

@Structure, DEC_PARAM , Size 0x7c bytes, from ./hdr/vo_ac3_var.h
.set  DEC_PARAM_ac3_aub         ,0x8      @  pointer to AC3_AB_DATA
.set  DEC_PARAM_bswitch         ,0x10     @  Word16
@End of Structure DEC_PARAM

@Structure, AC3_AB_DATA , Size 0x650 bytes, from ./hdr/vo_ac3_var.h
.set  AC3_AB_DATA_tcbuf         ,0x648    @  pointer to Word32
@End of Structure AC3_AB_DATA


@void idctsc(DEC_PARAM  *p_decparam, BUFF_PARAM *p_buff)
         @r0 --- *p_decparam
	 @r1 --- *p_buff 

_idctsc:

         stmfd         r13!, {r4 - r12, r14}
         sub           r13, r13, #0x20
	 ldr           r2, [r0, #DEC_PARAM_bswitch]      @p_decparam->bswitch
         ldr           r3, [r0, #DEC_PARAM_ac3_aub]      @AC3_AB_DATA *p_aublk
	 mov           r4, r1                            @p_buff->fftbuf
	 ldr           r5, [r3, #AC3_AB_DATA_tcbuf]      @p_aublk->tcbuf
	 cmp           r2, #0
	 beq           Lable1
	 @r4---p_buff->fftbuf,  r5---p_aublk->tcbuf
	 mov           r6, #0xC0                         @3*N/4
	 mov           r7, #0x40                         @N/4
	 mov           r8, #0x80                         @N/2
	 mov           r9, #0xFE                         @N-2
	 add           r10, r4, r6, LSL #2               @fftiptr
	 add           r11, r4, r7, LSL #2               @fftrptr2
	 add           r12, r4, r8, LSL #2               @fftiptr2
         add           r6, r5, r9, LSL #2                @tcrptr
	 ldr           r7, Table2                        @mixptr

	 mov           r0, #0x40                         @count = N/4
	 @r0---count, r4---fftrptr, r5---tciptr, r7---mixptr
	 @r10---fftiptr, r11---fftrptr2, r12---fftiptr2, r6---tcrptr
LOOP1:
         ldr           r1, [r7], #4                      @cr = *mixptr++
	 ldr           r2, [r7], #4                      @ci = *mixptr++
	 ldr           r3, [r6], #4                      @ar = *trcptr++
	 ldr           r8, [r5], #4                      @ai = *triptr++
         smmul         r9, r1, r3     
	 smmul         r14, r2, r8
         add           r9, r9, r9
         sub           r14, r9, r14, LSL #1
         smmul         r9, r2, r3
         smmul         r3, r1, r8
         add           r9, r9, r9
         add           r9, r9, r3, LSL #1
         str           r14, [r4], #4                     @*fftrptr++
       
         ldr           r3, [r6], #-4                     @ar = *tcrptr--
         ldr           r8, [r5], #-4                     @ai = *tciptr--
	 str           r9, [r10], #4                     @*fftiptr++

         smmul         r9, r1, r3     
	 smmul         r14, r2, r8
         add           r9, r9, r9
         sub           r14, r9, r14, LSL #1
         smmul         r9, r2, r3
         smmul         r3, r1, r8
         add           r9, r9, r9
         add           r9, r9, r3, LSL #1
         str           r14, [r11], #4                    @*fftrptr2++
       
         sub           r6, r6, #16
	 add           r5, r5, #16
         subs          r0, r0, #1
	 str           r9, [r12], #4                     @*fftiptr2++
	 bgt           LOOP1
         b             idctsc_end
Lable1:
         @r4---p_buff->fftbuf,  r5---p_aublk->tcbuf
	 mov           r6, #0x80                         @N/2
         mov           r7, #0xFF                         @N-1
         add           r8, r4, r6, LSL #2                @fftiptr
         add           r9, r5, r7, LSL #2                @tcrptr
         ldr           r10, Table1                       @mixptr = z1mix

	 mov           r0, #0x40                         @count = N/4

LOOP2:
         ldr           r1, [r10], #4                     @cr = *mixptr++
	 ldr           r2, [r10], #4                     @ci = *mixptr++
	 ldr           r3, [r9], #-8                     @ar = *tcrptr
	 ldr           r11,[r5], #8                      @ai = *tciptr
         smmul         r12, r1, r3
	 smmul         r14, r2, r11
	 add           r12, r12, r12
	 sub           r14, r12, r14, LSL #1
         smmul         r12, r1, r11
         smmul         r1, r2, r3
         str           r14, [r4], #4                     @*fftrptr++
	 add           r2, r1, r1
	 add           r3, r2, r12, LSL #1
         ldr           r1, [r10], #4                     @cr = *mixptr++
	 str           r3, [r8], #4                      @*fftiptr++

	 ldr           r2, [r10], #4                     @ci = *mixptr++
	 ldr           r3, [r9], #-8                     @ar = *tcrptr
	 ldr           r11,[r5], #8                      @ai = *tciptr
         smmul         r12, r1, r3
	 smmul         r14, r2, r11
	 add           r12, r12, r12
	 sub           r14, r12, r14, LSL #1
         smmul         r12, r1, r11
         smmul         r1, r2, r3
         str           r14, [r4], #4                     @*fftrptr++
	 add           r2, r1, r1
	 add           r3, r2, r12, LSL #1
         subs          r0, r0, #1
	 str           r3, [r8], #4                      @*fftiptr++   	 

	 bgt           LOOP2

idctsc_end:
         add           r13, r13, #0x20
	 ldmfd         r13!, {r4 - r12, r15}
         @ENDFUNC


@void idctsc2(DEC_PARAM  *p_decparam, BUFF_PARAM *p_buff)
         @r0 --- *p_decparam
	 @r1 --- *p_buff

_idctsc2:
    
         stmfd         r13!, {r4 - r12, r14}
	 sub           r13, r13, #0x20
	 ldr           r2, [r0, #DEC_PARAM_bswitch]       @p_decparam->bswitch
         ldr           r3, [r0, #DEC_PARAM_ac3_aub]       @AC3_AB_DATA *p_aublk
	 mov           r4, r1                             @p_buff->fftbuf
	 ldr           r5, [r3, #AC3_AB_DATA_tcbuf]       @p_aublk->tcbuf
	 @r4---p_buff->fftbuf,  r5---p_aublk->tcbuf
         cmp           r2, #0
         beq           Lable2
	 mov           r0, #0xC0                          @3*N/4
	 mov           r1, #0x40                          @N/4
	 mov           r2, #0x80                          @N/2
	 @add           r6, r4, r0, LSL #2                 @fftiptr
	 @add           r7, r4, r1, LSL #2                 @fftrptr2
	 @add           r8, r4, r2, LSL #2                 @fftiptr2
	 add           r9, r5, r0, LSL #2                 @tciptr
	 add           r10, r5, r1, LSL #2                @tcrptr2
	 add           r11, r5, r2, LSL #2                @tciptr2
	 ldr           r12, Table2                        @mixptr = z2mix
	 ldr           r3,  Table3                        @bitrevary[]
	 mov           r0, #0x0                              @count = 0

LOOP3:
         ldr           r1, [r12], #4                      @cr = *mixptr++
	 ldr           r2, [r12], #4                      @ci = *mixptr++
	 add           r6, r9, r0, LSL #2                 @&bitrevary[2 * count]
         ldrsh         r7, [r6]                           @index
         add           r8, r4, r7, LSL #1
         ldr           r7, [r8]	                          @ar = fftrptr[index]
	 ldr           r6, [r8, #0x300]                   @ai = fftiptr[index]
	 str           r8, [sp, #0x4]                     @push fftrptr[index]
	 smmul         r8, r1, r7
	 smmul         r14, r2, r6
	 add           r8, r8, r8
	 sub           r8, r8, r14, LSL #1
	 str           r8, [r5], #4                       @tcrptr++
	 smmul         r8, r1, r6
	 smmul         r14, r2, r7
	 add           r8, r8, r8
	 add           r8, r8, r14, LSL #1
	 ldr           r14, [sp, #0x4]                    @pull fftrptr[index]
         
	 str           r8, [r9], #4                       @tciptr++

	 ldr           r7, [r14, #0x100]                  @ar = fftrptr2[index]
	 ldr           r6, [r14, #0x200]                  @ai = fftiptr2[index]
	 smmul         r8, r1, r7
	 smmul         r14, r2, r6
	 add           r8, r8, r8
	 sub           r8, r8, r14, LSL #1
	 str           r8, [r10], #4                      @tcrptr2++
	 smmul         r8, r1, r6
	 smmul         r14, r2, r7
	 add           r8, r8, r8
	 add           r8, r8, r14, LSL #1
         add           r0, r0, #1
	 str           r8, [r11], #4                      @tciptr2++
	 cmp           r0, #0x40
    
         blt           LOOP3
         b             idctsc2_end	 
Lable2:
         mov           r0, #0x80                           
	 add           r6, r4, r0, LSL #2                 @fftiptr
	 add           r7, r5, r0, LSL #2                 @tciptr 
	 ldr           r8, Table1                         @mixptr
	 ldr           r9, Table3                         @bitrevary
         mov           r0, #0x0                          @count = 0
LOOP4:
	 ldr           r1, [r8], #4                       @cr = *mixptr++
	 ldrsh         r3, [r9], #2                       @index
	 ldr           r2, [r8], #4                       @ci = *mixptr++
         add           r10, r4, r3, LSL #2
	 add           r11, r6, r3, LSL #2
	 ldr           r3, [r10]                          @ar = fftrptr[index]
	 ldr           r10, [r11]                         @ai = fftiptr[index]

	 smmul         r11, r1, r3
	 smmul         r12, r2, r10
	 add           r11, r11, r11
	 sub           r11, r11, r12, LSL #1

	 smmul         r12, r1, r10
	 smmul         r1, r2, r3
	 add           r12, r12, r12
	 add           r12, r12, r1, LSL #1
	 str           r11, [r5], #4                      @*tcrptr++
	 add           r0, r0, #1
	 str           r12, [r7], #4                      @*tciptr++
	 cmp           r0, #0x80
	 blt           LOOP4

idctsc2_end:

         add           r13, r13, #0x20
	 ldmfd         r13!, {r4 - r12, r15}
	 @ENDFUNC

Table1:
         .word           _z1mix

Table2: 
         .word           _z2mix

Table3:
         .word           _bitrevary

	 @.END
         























