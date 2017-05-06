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
        .globl    _bitunp_rj
        .globl    _bitunp_lj
        .globl    _bitskip
        
@Structure, AC3_BSPK , Size 0x10 bytes, from ./hdr/vo_ac3_var.h
.set    AC3_BSPK_pkptr            , 0x4      @  pointer to Word16
.set    AC3_BSPK_pkbitptr         , 0x8      @  Word16
.set    AC3_BSPK_pkdata           , 0xa      @  Word16
@End of Structure AC3_BSPK

@*******************************************************************************
@void bitunp_rj(DSPshort *dataptr, DSPshort numbits, AC3_BSPK *p_bstrm)
@*******************************************************************************

_bitunp_rj:

        stmfd    sp!, {r4 - r10, r14}                     
        ldr      r4,[r2,#AC3_BSPK_pkptr]                      @r5 = pkptr
        ldrh     r5,[r2,#AC3_BSPK_pkbitptr]                   @r5 = pkptr     
        ldrh     r9,[r4]
        ldrh     r10,[r4,#2]     
        orr      r6,r10,r9,LSL #0x10
        
        rsb      r7,r1,#0x20                                    @r7  = 32-r1
        mov      r6,r6,LSL r5
        mov      r6,r6,LSR r7
              
        add      r8,r5,r1
        and      r5,r8,#0xF                                      
        subs     r8,r8,r5
                  
        ldrneh   r9,[r4,#2]!
        strh     r6,[r0]

        str      r4,[r2,#AC3_BSPK_pkptr]                      @r4 = pkptr
        strh     r5,[r2,#AC3_BSPK_pkbitptr]
        strneh   r9,[r2,#AC3_BSPK_pkdata]

        ldmfd    sp!, {r4-r10,pc}
        @ENDFUNC               

@*******************************************************************************
@void bitunp_lj(DSPshort *dataptr, DSPshort numbits,AC3_BSPK *p_bstrm)
@*******************************************************************************

_bitunp_lj:

        stmfd    sp!, {r4-r10, r14}                    
        ldr      r4,[r2,#AC3_BSPK_pkptr]                      @r5 = pkptr
        ldrh     r5,[r2,#AC3_BSPK_pkbitptr]                   @r5 = pkptr
        ldrh     r9,[r4]
        ldrh     r10,[r4,#2]     
        orr       r6,r10,r9,LSL #0x10
     
        rsb      r7,r1,#0x20                                    @r7  = 32-r1
        sub      r8,r7,r5                                       @r8  = 32-r10-r2
        mov      r6,r6,LSR r8
        mov      r6,r6,LSL r7
        mov      r6,r6,LSR #16
        
        add      r8,r5,r1
        and      r5,r8,#0xF                                      
        subs     r8,r8,r5
                 
        ldrneh   r9,[r4,#2]!
        strh     r6,[r0]

        str      r4,[r2,#AC3_BSPK_pkptr]                      @r4 = pkptr
        strh     r5,[r2,#AC3_BSPK_pkbitptr]
        strneh   r9,[r2,#AC3_BSPK_pkdata]

        ldmfd    sp!, {r4-r10,pc}
        @ENDFUNC               
        
        
@*******************************************************************************
@void bitskip(DSPshort numbits, AC3_BSPK *p_bstrm)
@*******************************************************************************
_bitskip:

        stmfd    sp!, {r4-r12,r14}                      

        ldr      r4,[r1,#AC3_BSPK_pkptr]                      @r5 = pkptr
        ldrh     r5,[r1,#AC3_BSPK_pkbitptr]                   @r5 = pkptr

        add      r6,r5,r0
        and      r5,r6,#0xF
        mov      r6,r6,LSR #4                                    

        mov      r6,r6,LSL #1      
        ldrh     r7,[r4,r6]!

        str      r4,[r1,#AC3_BSPK_pkptr]                      @r4 = pkptr
        strh     r5,[r1,#AC3_BSPK_pkbitptr]
        strh     r7,[r1,#AC3_BSPK_pkdata]

        ldmfd    sp!, {r4-r12,pc}
        @ENDFUNC          
        
        @ENDIF
        @.END
	        
	        
