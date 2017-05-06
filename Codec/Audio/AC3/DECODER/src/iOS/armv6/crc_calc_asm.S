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
	.globl  _crc_calc
	.globl  _crctab


@Structure, CRC_CALC_PL , Size 0x10 bytes, from ./hdr/vo_ac3_var.h

.set       CRC_CALC_PL_iptr,   0x4      @  pointer to Word16
.set       CRC_CALC_PL_count,  0xe      @  Word16

@End of Structure CRC_CALC_PL

@***************************************
@DOLBY_SIP crc_calc(DOLBY_SIP input_sip)
@***************************************

@ DOLBY_SIP Structure
@ typedef struct {
@       Word16  funcnum@          --- r0
@       Word16  status@           --- r1
@       void    *param_ptr@       --- r2
@ }DOLBY_SIP


_crc_calc:

        stmfd    r13!, {r4 - r12, r14}

        ldr      r7, [r2, #CRC_CALC_PL_iptr]            @r7 = iptr
        uxth     r1, r1, ROR #16                        @r1 = status --- syndrom
        ldrsh    r8, [r2, #CRC_CALC_PL_count]           @r8 = buflen
        ldr      r11,Table1 
        ldr      r9, Table2                             @ Load SIP_REV    
        ldrh     r3, [r7], #2                           @ bufptr[i]                         
        subs     r8, r8,#1
        blt      crc_calc_end 

@ for (i = buflen; i > 0;  i--)
Lable1:        

        orr     r1, r3, r1, LSL #16
        uxtb    r6, r1, ROR #24
        mov     r6, r6, LSL #1
        ldrh    r5, [r11, r6]                            @load crctab[(syndrome >> 8) & 0xff]
        eor     r1, r1, r5, LSL #8                                           

        uxtb    r6, r1, ROR #16
        mov     r6, r6, LSL #1
        ldrh    r3, [r7], #2   
        ldrh    r5, [r11, r6]                            @load crctab[(syndrome >> 8) & 0xff]                                               
        subs    r8, r8, #1
        eor     r1, r1, r5        
        
        bge     Lable1

crc_calc_end: 
 
        orr     r9, r9, r1, LSL #16
        mov     r10, #0                                  @ret_sip.param_ptr = NULLPTR
        str     r9, [r0, #0]
        str     r10, [r0, #4]                            @ret_sip.funcnum = SIP_REV
        ldmfd   r13!, {r4 - r12, r15}

        @ENDFUNC

Table1:
        .word      _crctab

Table2:
        .word      0x00000701                              @((7<<8) + 1)
  
	@.END
