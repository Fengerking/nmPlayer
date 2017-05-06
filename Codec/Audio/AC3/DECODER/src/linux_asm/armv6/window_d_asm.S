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
       .global   window_d
       .extern   chantab
       .extern   Lwindow_Interleave

@Structure, DECEXEC , Size 0x5c bytes, from ./hdr/vo_ac3_var.h
.equ    DECEXEC_ac3_buff , 0x8      @  pointer to BUFF_PARAM
@End of Structure DECEXEC

@Structure, BUFF_PARAM , Size 0x48d4 bytes, from ./hdr/vo_ac3_var.h
.equ    BUFF_PARAM_dnmix_buf  , 0xc00    @  array[6] of array[256] of Word32
.equ    BUFF_PARAM_delay_buf  , 0x2400   @  array[6] of array[128] of Word32
@ End of Structure BUFF_PARAM

@Structure, DEC_PARAM , Size 0x7c bytes, from ./hdr/vo_ac3_var.h
.equ    DEC_PARAM_outmod      , 0x1c     @  Word16
.equ    DEC_PARAM_pcmbufptr   , 0x64     @  array[6] of pointer to Word32
@End of Structure DEC_PARAM

@***********************************************
@void window_d(DSPshort chan, DECEXEC *p_decexec)
@***********************************************
@ r0 --- chan
@ r1 --- p_decexec, p_decparam

window_d:
            
           STMFD           r13!, {r4 - r12, r14}
	   SUB             r13, r13, #0x20

	   MOV             r7, r0                               @chan
	   STR             r0, [sp, #0x4]                      @push chan
	   LDR             r6, [r1, #DECEXEC_ac3_buff]          @p_buff
	   LDR             r3, [r1]                             @p_decparam
	   @r6 ----pcmptr1 = p_buff->fftbuf, p_buff
	   MOV             r5, #0xFF
	   ADD             r4, r6, r5, LSL #2                   @pcmptr2 = p_buff->fftbuf + N -1

	   STR             r3,  [sp, #0x8]                      @push p_decparam
           STR             r6,  [sp, #0xc]                      @push p_buff	   
	   ADD             r8, r6, #BUFF_PARAM_dnmix_buf        @p_buff->dnmix_buf[] address/*	Copy PCM samples to output buffer */
	   ADD             r9, r8, r7, LSL #10                   @p_buff->dnmix_buf[chan] address

	   ADD             r10, r6, #BUFF_PARAM_delay_buf       @p_buff->delay_buf[] address
	   ADD             r11, r10, r7, LSL #9                 @p_buff->delay_buf[chan] address

	   STR             r9,  [sp, #0x10]                     @push p_buff->dnmix_buf[chan]
	   STR             r11, [sp, #0x14]                      @push p_buff->delay_buf[chan]

	   @r6--- pcmptr1, r4 --- pcmptr2, r9 --- p_buff->dnmix_buf[chan], r11 --- p_buff->delay_buf[chan]

       ADR             r8, Table1                          @winptr1
	   MOV             r0, #0xc0
	   ADD             r10, r9, r0, LSL #2                   @p_buff->dnmix_buf[chan] + 3*N/4
           MOV             r0, #0x3F               
	   ADD             r9, r9, r0, LSL #2                    @p_buff->dnmix_buf[chan] + N/4-1
	   MOV             r0, #0x7F
	   ADD             r2, r11, r0, LSL #2                   @p_buff->delay_buf[chan] + N/2-1

	   @r6, r4, r8, r2, r11, r9, r10

	   MOV             r0, #64

Windows_Loop:

           @samp = (- Vo_Multi32((*windptr2), (*delayptr1)) - Vo_Multi32((*windptr1), (*dnmixptr1)))<<1@
           LDR             r1, [r8], #4                          @*winptr1 
	   LDR             r3, [r8], #4                          @*winptr2
	   LDR             r5, [r10], #4                         @*dnmixptr1
	   LDR             r7, [r11], #4                         @*delayptr1
	   SMMUL           r12, r1, r5                           @Vo_Multi32((*windptr1), (*dnmixptr1))
           SMMUL           r14, r3, r7                           @Vo_Multi32((*windptr2), (*delayptr1))
           ADD             r12, r12, r12
	   ADD             r12, r12, r14, LSL #1               
	   SMMUL           r1, r1, r7                            @Vo_Multi32((*windptr1), (*delayptr1++))
	   SMMUL           r3, r3, r5                            @Vo_Multi32((*windptr2), (*dnmixptr1++))
	   MOV             r14, #0
	   ADD             r3, r3, r3
	   RSB             r12, r12, r14
	   SUB             r14, r3, r1, LSL #1
	   ADD             r12, r12, r12
	   ADD             r14, r14, r14
	   STR             r12, [r6], #4                         @*pcmptr1++ = samp
	   STR             r14, [r4], #-4                        @*pcmptr2-- = samp

           LDR             r1, [r8], #4                          @*winptr1
           LDR             r3, [r8], #4                          @*winptr2
           LDR             r5, [r9], #-4                          @*dnmixptr2
           LDR             r7, [r2], #-4                          @*delayptr2

           SMMUL           r12, r1, r5                           @Vo_Multi32((*windptr1), (*dnmixptr2))
           SMMUL           r14, r3, r7                           @Vo_Multi32((*windptr2), (*delayptr2))	   
           	   
           ADD             r12, r12, r12
	   ADD             r12, r12, r14, LSL #1
	   SMMUL           r1, r1, r7                            @Vo_Multi32((*windptr1), (*delayptr2--))
	   SMMUL           r3, r3, r5                            @Vo_Multi32((*windptr2), (*dnmixptr2--))
	   ADD             r12, r12, r12
	   ADD             r1, r1, r1
	   ADD             r3, r3, r3
	   STR             r12, [r6], #4                         @*pcmptr1++ = samp
	   SUB             r14, r1, r3
	   SUBS            r0, r0, #1                            @count--
	   ADD             r14, r14, r14
	   STR             r14, [r4], #-4                        @*pcmptr2-- = samp

	   BGT             Windows_Loop

	   @Update delay buffers
  
	   LDR             r9, [sp, #0x10]                        @pull p_buff->dnmix_buf[chan]  
	   MOV             r1, #0x40	
           MOV             r0, #32                               @N/8
	   ADD             r9, r9, r1, LSL #2                    @p_buff->dnmix_buf[chan] + (N/4)
           LDR             r11,[sp, #0x14]                        @pull p_buff->delay_buf[chan]
Up_Loop:
           LDR             r3,  [r9], #4
	   LDR             r5,  [r9], #4
	   LDR             r7,  [r9], #4
	   LDR             r12, [r9], #4
	   STR             r3,  [r11], #4
	   STR             r5,  [r11], #4
	   STR             r7,  [r11], #4
	   STR             r12, [r11], #4
	   SUBS            r0, r0, #1
	   BGT             Up_Loop

	   @Copy PCM samples to output buffer
	   MOV             r0, #0x6
	   LDR             r3, [sp, #0x8]                        @pull p_decparam
	   LDR             r5, [sp, #0x4]                       @pull chan
	   LDR             r9, [sp, #0xc]                        @p_buff->fftbuf
           ADR             r7, Table2                            @load chantab
           LDRSH           r12, [r3, #DEC_PARAM_outmod]          @p_decparam->outmod
	   ADD             r14, r3, #DEC_PARAM_pcmbufptr         @p_decparam->pcmbufptr[]

	   MUL             r12, r12, r0
	   ADD             r5, r5, r12      
	   ADD             r11, r7, r5, LSL #1                   @chantab[p_decparam->outmod][chan]@
	   MOV             r0, #64
	   LDRSH           r11, [r11]                            @outchan
	   MOV             r10, r11, LSL #2
	   LDR             r11, [r14, r10]                         @p_decpara->pcmbufptr[outchan]

Copy_Loop:
           LDR             r2, [r9], #4
	   LDR             r3, [r9], #4
	   LDR             r4, [r9], #4
	   LDR             r5, [r9], #4
	   STR             r2, [r11], #0x18
           STR             r3, [r11], #0x18
	   STR             r4, [r11], #0x18
	   STR             r5, [r11], #0x18
	   SUBS            r0, r0, #1
	   BGT             Copy_Loop

window_d_end:

           ADD             r13, r13, #0x20         
           LDMFD           r13!, {r4 - r12, r15}
Table2:
           @.word           chantab
	.short	0,2,5,-1,-1,-1
	.short	1,5,-1,-1,-1,-1
	.short	0,2,5,-1,-1,-1
	.short	0,1,2,5,-1,-1
	.short	0,2,3,5,-1,-1
	.short	0,1,2,3,5,-1
	.short	0,2,3,4,5,-1
	.short	0,1,2,3,4,5
Table1:
           @.word           Lwindow_Interleave
	.word	0x000474c9,   0x7fffffeb,   0x0007fdfc,   0x7fffffbf,	0x000c0272,   0x7fffff6e,   0x0010a35a,   0x7ffffeea   
	.word	0x0015f4ae,   0x7ffffe1c,   0x001c079d,   0x7ffffced,	0x0022ecc1,   0x7ffffb3b,   0x002ab4c5,   0x7ffff8df  
	.word	0x003370ab,   0x7ffff5a8,   0x003d31e5,   0x7ffff15e,	0x00480a5e,   0x7fffebb9,   0x00540c86,   0x7fffe466   
	.word	0x00614b4d,   0x7fffdb04,   0x006fda24,   0x7fffcf20,	0x007fccfa,   0x7fffc031,   0x00913839,   0x7fffad9e   
	.word	0x00a430c2,   0x7fff96b0,   0x00b8cbe7,   0x7fff7a98,	0x00cf1f67,   0x7fff586a,   0x00e74166,   0x7fff2f17   
	.word	0x0101486b,   0x7ffefd6b,   0x011d4b53,   0x7ffec20c,	0x013b614e,   0x7ffe7b74,   0x015ba1d8,   0x7ffe27eb   
	.word	0x017e24af,   0x7ffdc588,   0x01a301c8,   0x7ffd5228,	0x01ca514e,   0x7ffccb6d,   0x01f42b91,   0x7ffc2eb6   
	.word	0x0220a901,   0x7ffb791b,   0x024fe223,   0x7ffaa76c,	0x0281ef88,   0x7ff9b626,   0x02b6e9c0,   0x7ff8a170   
	.word	0x02eee953,   0x7ff76518,   0x032a06b4,   0x7ff5fc8c,	0x03685a36,   0x7ff462d1,   0x03a9fc01,   0x7ff29283   
	.word	0x03ef0407,   0x7ff085ce,   0x043789f9,   0x7fee3666,	0x0483a539,   0x7feb9d83,   0x04d36cd0,   0x7fe8b3dd   
	.word	0x0526f762,   0x7fe571a5,   0x057e5b22,   0x7fe1ce82,	0x05d9adc3,   0x7fddc188,   0x06390471,   0x7fd94139   
	.word	0x069c73c1,   0x7fd4437b,   0x07040fa7,   0x7fcebd98,	0x076feb67,   0x7fc8a438,   0x07e0198d,   0x7fc1eb5e   
	.word	0x0854abe1,   0x7fba8664,   0x08cdb355,   0x7fb267fc,	0x094b4003,   0x7fa98227,   0x09cd6119,   0x7f9fc63b   
	.word	0x0a5424d5,   0x7f9524db,   0x0adf9875,   0x7f898dfd,	0x0b6fc82f,   0x7f7cf0e3,   0x0c04bf25,   0x7f6f3c20   
	.word	0x0c9e875d,   0x7f605d97,   0x0d3d29b7,   0x7f504280,	0x0de0ade1,   0x7f3ed767,   0x0e891a52,   0x7f2c082e   
	.word	0x0f367440,   0x7f17c016,   0x0fe8bf99,   0x7f01e9bf,	0x109ffef6,   0x7eea6f2e,   0x115c339f,   0x7ed139d6   
	.word	0x121d5d79,   0x7eb6329a,   0x12e37b07,   0x7e9941d6,	0x13ae8964,   0x7e7a4f68,   0x147e843a,   0x7e5942ba   
	.word	0x155365c4,   0x7e3602c7,   0x162d26c4,   0x7e107628,	0x170bbe87,   0x7de8831d,   0x17ef22db,   0x7dbe0f9b   
	.word	0x18d74814,   0x7d910156,   0x19c42107,   0x7d613dcb,	0x1ab59f0a,   0x7d2eaa52,   0x1babb1f8,   0x7cf92c27   
	.word	0x1ca6482d,   0x7cc0a87a,   0x1da54e89,   0x7c85047b,	0x1ea8b076,   0x7c46256e,   0x1fb057e7,   0x7c03f0b1   
	.word	0x20bc2d5e,   0x7bbe4bd6,   0x21cc17ee,   0x7b751ca8,	0x22dffd44,   0x7b284943,   0x23f7c1aa,   0x7ad7b81d   
	.word	0x2513480e,   0x7a83501d,   0x2632720c,   0x7a2af8a3,	0x27551ff2,   0x79ce99a0,   0x287b30c9,   0x796e1b9f   
	.word	0x29a48263,   0x790967d9,   0x2ad0f15e,   0x78a06844,	0x2c005936,   0x783307a2,   0x2d329449,   0x77c1318f   
	.word	0x2e677be6,   0x774ad293,   0x2f9ee85d,   0x76cfd82d,	0x30d8b103,   0x765030e5,   0x3214ac48,   0x75cbcc59   
	.word	0x3352afc1,   0x75429b46,   0x34929036,   0x74b48f9c,	0x35d421b1,   0x74219c86,   0x37173790,   0x7389b676   
	.word	0x385ba492,   0x72ecd334,   0x39a13ae6,   0x724ae9e1,   0x3ae7cc3f,   0x71a3f30c,   0x3c2f29e1,   0x70f7e8af   
	.word	0x3d7724b6,   0x7046c644,   0x3ebf8d5b,   0x6f9088c1,   0x40083434,   0x6ed52eaa,   0x4150e97e,   0x6e14b80e   
	.word	0x42997d5d,   0x6d4f2694,   0x43e1bff4,   0x6c847d7b,   0x45298172,   0x6bb4c19f,   0x46709226,   0x6adff97f   
	.word	0x47b6c28f,   0x6a062d39,   0x48fbe372,   0x69276695,   0x4a3fc5e8,   0x6843b0fa,   0x4b823b71,   0x675b1978   
	.word	0x4cc31606,   0x666daec1,   0x4e02282c,   0x657b8128,   0x4f3f44ff,   0x6484a2a1,   0x507a404d,   0x638926b8   
	.word	0x51b2ee9b,   0x62892294,   0x52e92541,   0x6184ace9,   0x541cba71,   0x607bddfb,   0x554d8549,   0x5f6ecf8e   
	.word	0x567b5de7,   0x5e5d9ce6,   0x57a61d6f,   0x5d4862ba,   0x58cd9e20,   0x5c2f3f2f,   0x59f1bb60,   0x5b1251c7   



	   @ENDFUNC
	   .END 
  










