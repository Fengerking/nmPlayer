@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************
#include "../../voH264EncID.h"	 
@AREA	|.text|, CODE
	 .text
 
	 .globl _Quant2x2Dc_ARMV7
	 .globl _Quant4x4_ARMV7
	 .globl _Quant4x4Dc_ARMV7
	 .globl _Dequant4x4_ARMV7
	 .globl _Dequant4x4Dc_ARMV7
	 
	 .globl _CoeffLast4_ARMV7
	 .globl _CoeffLast15_ARMV7
	 .globl _CoeffLast16_ARMV7
 
	 
	 .align 4

	 .macro M_QUANT_TWO @bias0, bias1, mf0, mf1, mf2, mf3, load_mf
		vadd.u16    q8,  q8,  $0
		vadd.u16    q9,  q9,  $1
	 .if $6 == 1
		vld1.64     {$2-$5}, [r1 ]!
	 .endif
		vmull.u16   q10, d16, $2
		vmull.u16   q11, d17, $3
		vmull.u16   q12, d18, $4
		vmull.u16   q13, d19, $5
		vshr.s16    q14, q14, #15
		vshr.s16    q15, q15, #15
		vshrn.u32   d16, q10, #16
		vshrn.u32   d17, q11, #16
		vshrn.u32   d18, q12, #16
		vshrn.u32   d19, q13, #16
		veor        q8,  q8,  q14
		veor        q9,  q9,  q15
		vsub.s16    q8,  q8,  q14
		vsub.s16    q9,  q9,  q15
		vorr        $0, q8,  q9
		vst1.64     {d16-d19}, [r0 ]!
	 .endmacro

	 .macro  M_QUANT_END @d
		vmov        r2,  r3,  $0
		orrs        r0,  r2,  r3
		movne       r0,  #1
		bx          lr
	 .endmacro
	 
	 .macro M_DEQUANT_START @mf_size, offset, dc
		mov         r3,  #0x2b
		mul         r3,  r3,  r2
		lsr         r3,  r3,  #8            @ i_qbits = i_qp / 6
		add         ip,  r3,  r3,  lsl #1
		sub         r2,  r2,  ip,  lsl #1   @ i_mf = i_qp % 6
	 .if $2 == 0
		add         r1,  r1,  r2, lsl #$0  @ dequant_mf[i_mf]
	 .else
		ldr         r1, [r1,  r2, lsl #$0] @ dequant_mf[i_mf][0][0]
	 .endif
		subs        r3,  r3,  #$1      @ 6 for 8x8
	 .endmacro

_Quant2x2Dc_ARMV7:
    vld1.64     {d0}, [r0 ]
    vdup.16     d2,  r2
    vdup.16     d1,  r1
    vabs.s16    d3,  d0
    vadd.u16    d3,  d3,  d2
    vmull.u16   q3,  d3,  d1
    vshr.s16    d0,  d0,  #15
    vshrn.u32   d3,  q3,  #16
    veor        d3,  d3,  d0
    vsub.s16    d3,  d3,  d0
    vst1.64     {d3}, [r0 ]
    M_QUANT_END d3
    

_Quant4x4Dc_ARMV7:  
    vld1.64     {d28-d31}, [r0 ]
    vdup.16     q0,  r2
    vdup.16     q2,  r1
    vabs.s16    q8,  q14
    vabs.s16    q9,  q15
    M_QUANT_TWO q0,  q0,  d4,  d5,  d4,  d5, 0
    vorr        d0,  d0,  d1
    M_QUANT_END d0
    

_Quant4x4_ARMV7:  
    vld1.64     {d28-d31}, [r0 ]
    vld1.64     {d0-d3}, [r2 ]
    vld1.64     {d4-d7}, [r1 ]
    vabs.s16    q8,  q14
    vabs.s16    q9,  q15
    M_QUANT_TWO q0,  q1,  d4,  d5,  d6,  d7, 0
    vorr        d0,  d0,  d1
    M_QUANT_END d0
    
    
@****************************************************************

_Dequant4x4_ARMV7: 
    M_DEQUANT_START 6, 4, 0         

    blt         dq_4x4_lable
    vdup.16     q15, r3
    
    vld1.32     {d16-d17}, [r1 ]!
    vld1.16     {d0-d3},   [r0 ]
    vld1.32     {d18-d19}, [r1 ]!
    vmovn.s32   d4,  q8
    vld1.32     {d20-d21}, [r1 ]!
    vmovn.s32   d5,  q9
    vld1.32     {d22-d23}, [r1 ]!
    vmovn.s32   d6,  q10
    vmovn.s32   d7,  q11
    vmul.s16    q0,  q0,  q2
    vmul.s16    q1,  q1,  q3
    vshl.s16    q0,  q0,  q15
    vshl.s16    q1,  q1,  q15
    vst1.16     {d0-d3},   [r0 ]!
    bx          lr

dq_4x4_lable:
    vdup.32     q15, r3
    rsb         r3,  r3,  #0
    mov         ip,  #1
    sub         r3,  r3,  #1
    lsl         ip,  ip,  r3

    vld1.32     {d16-d17}, [r1 ]!
    vdup.32     q10, ip
    vld1.32     {d18-d19}, [r1 ]!
    vshl.i64    q11, q10, #0
    vld1.32     {d16-d17}, [r1 ]!
    vmovn.s32   d4,  q8
    vld1.32     {d18-d19}, [r1 ]!
    vld1.16     {d0-d3},   [r0 ]
    vshl.i64    q12, q10, #0
    vshl.i64    q13, q10, #0
    vmovn.s32   d6,  q8
    vmovn.s32   d5,  q9
    vmovn.s32   d7,  q9

    vmlal.s16   q10, d0,  d4
    vmlal.s16   q11, d1,  d5
    vmlal.s16   q12, d2,  d6
    vmlal.s16   q13, d3,  d7
    vshl.s32    q10, q10, q15
    vshl.s32    q11, q11, q15
    vshl.s32    q12, q12, q15
    vshl.s32    q13, q13, q15

    vmovn.s32   d0,  q10
    vmovn.s32   d1,  q11
    vmovn.s32   d2,  q12
    vmovn.s32   d3,  q13
    vst1.16     {d0-d3},   [r0 ]!
    bx          lr
        
    
@****************************************************************

_Dequant4x4Dc_ARMV7 :
    M_DEQUANT_START 6, 6, 1
    blt         dq_4x4Dc_lable

    lsl         r1,  r1,  r3
    vdup.16     q2,  r1
    vld1.16     {d0-d3},   [r0]
    vdup.16     q15, r3

    vmul.s16    q0,  q0,  q2
    vmul.s16    q1,  q1,  q2
    vst1.16     {d0-d3},   [r0]
    bx          lr

dq_4x4Dc_lable:
    vdup.16     d4,  r1
    vdup.32     q15, r3
    rsb         r3,  r3,  #0
    mov         ip,  #1
    sub         r3,  r3,  #1
    lsl         ip,  ip,  r3

    vdup.32     q10, ip
    vdup.32     q11, ip
    vld1.16     {d0-d3},   [r0]
    vdup.32     q12, ip
    vdup.32     q13, ip

    vmlal.s16   q10, d0,  d4
    vmlal.s16   q11, d1,  d4
    vmlal.s16   q12, d2,  d4
    vmlal.s16   q13, d3,  d4
    vshl.s32    q10, q10, q15
    vshl.s32    q11, q11, q15
    vshl.s32    q12, q12, q15
    vshl.s32    q13, q13, q15

    vmovn.s32   d0,  q10
    vmovn.s32   d1,  q11
    vmovn.s32   d2,  q12
    vmovn.s32   d3,  q13
    vst1.16     {d0-d3},   [r0]
    bx          lr
    
@****************************************************************

_CoeffLast4_ARMV7 :
    ldrd        r2,  [r0]
    subs        r0,  r3,  #0
    movne       r0,  #2
    movne       r2,  r3
    lsrs        r2,  r2,  #16
    addne       r0,  r0,  #1
    bx          lr
    

_CoeffLast15_ARMV7 :
    sub         r0,  r0,  #2
    vld1.64     {d0-d3}, [r0]

    vtst.16     q0,  q0
    vtst.16     q1,  q1
    vshrn.u16   d0,  q0,  #8
    vshrn.u16   d1,  q1,  #8
    vshrn.u16   d0,  q0,  #4
    vclz.i32    d0,  d0
    vmov        r0,  r1,  d0
    mov         r3,  #7
    mov         r2,  #6                

    subs        r1,  r3,  r1,  lsr #2
    addge       r0,  r1,  #7           
    sublts      r0,  r2,  r0,  lsr #2
    movlt       r0,  #0
    bx          lr
    
    
_CoeffLast16_ARMV7 :
    vld1.64     {d0-d3}, [r0 ]

    vtst.16     q0,  q0
    vtst.16     q1,  q1
    vshrn.u16   d0,  q0,  #8
    vshrn.u16   d1,  q1,  #8
    vshrn.u16   d0,  q0,  #4
    vclz.i32    d0,  d0
    mov         ip,  #7
    mov         r3,  #7             
    vmov        r0,  r1,  d0

    subs        r1,  ip,  r1,  lsr #2
    addge       r0,  r1,  #8         
    sublts      r0,  r3,  r0,  lsr #2
    movlt       r0,  #0
    bx          lr
   
    
   @ .end


