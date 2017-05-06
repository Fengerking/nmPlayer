@
@  Copyright (c) 2010 The VP8 project authors. All Rights Reserved.
@
@  Use of this source code is governed by a BSD-style license and patent
@  grant that can be found in the LICENSE file in the root of the source
@  tree. All contributing project authors may be found in the AUTHORS
@  file in the root of the source tree.

    @ ARM
    @ REQUIRE8
    @ PRESERVE8

    @AREA    ||.text||, CODE, READONLY @ name this block of code
    @EXPORT  |png_filter_value_paeth_armv7|
    @EXPORT  |png_filter_value_paeth_4_armv7|
	.section .text
	.align 4
	.global png_filter_value_paeth_armv7
	.global png_filter_value_paeth_4_armv7
	
@void png_filter_value_paeth_8 (unsigned char *cp,unsigned char *lp,
@unsigned char *rp)
@-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
png_filter_value_paeth_armv7:@ PROC
	 push            {lr,r4}         @    d0       d1
	 
	 ldr    r4,=W_table
	 vld1.u8 d28,[r4]
	 
LOOP:
	 vld1.u8     {q0}, [r0]  @q0:  rgba rgba rgba rgba
	 							  @    d2       d3
	 vld1.u8     {q1}, [r1]  @q1:  rgba rgba rgba rgba
	 @vld1.u8     {d20}, [r2] @d20: rgba xxxx
	 
	 vmovl.u8    q10,d0
	 vmovl.u8    q11,d2
	 vabd.s16    d4,d21,d20  @ |b-c|
	 vabd.s16    d5,d22,d20  @ |a-c|
	 vadd.s16    d7,d22,d21  @ a+b
	 vsub.s16    d6,d7,d20   @ a+b-c
	 vabd.s16    d6,d6,d20   @ |a+b-c-c|
	 
	 vcle.s16    d8,d4,d5   @ pa pb
	 vcle.s16    d9,d4,d6   @ pa pc
	 vcle.s16    d10,d5,d6   @ pb pc	
	  
	 vcgt.s16    d11,d4,d5   @ pa pb
	 vcgt.s16    d12,d4,d6   @ pa pc
	 vcgt.s16    d13,d5,d6   @ pb pc
	 
	 vand.s16    d8,d8,d9       @make0 a
	 vand.s16    d9,d11,d10     @make1 b 
	 vand.s16    d10,d12,d13    @make0 c  
	 
	 vand.s16    d22,d22,d8  @a
	 vand.s16    d21,d21,d9  @b 
	 vand.s16    d20,d20,d10 @c
	 
	 vadd.s16	d30,d20,d21 
	 vadd.s16	d30,d30,d22 
	 
	 vadd.s16	d30,d30,d23
	 @vmovn.s16  d30,q15
	 @vmovl.u8  q15,d30
	 vand.s16   d30,d30,d28
	 
	 
	 @@@@@@@@@@@@@@@@@@@@@@@@@@@@
	 @vshl.u64    q0,q0,#32
	 @vshl.u64    q1,q1,#32 
	 vext.32      d0,d0,d1,#1
	 vext.32      d2,d2,d3,#1
	 
	 vmovl.u8   q10,d0
	 vmovl.u8   q11,d2
	 vmov.64    d22,d30
	 
	 vabd.s16    d4,d21,d20  @ |b-c|
	 vabd.s16    d5,d22,d20  @ |a-c|
	 vadd.s16    d7,d22,d21  @ a+b
	 vsub.s16    d6,d7,d20   @ a+b-c
	 vabd.s16    d6,d6,d20   @ |a+b-c-c|
	 
	 vcle.s16    d8,d4,d5   @ pa pb
	 vcle.s16    d9,d4,d6   @ pa pc
	 vcle.s16    d10,d5,d6   @ pb pc	
	  
	 vcgt.s16    d11,d4,d5   @ pa pb
	 vcgt.s16    d12,d4,d6   @ pa pc
	 vcgt.s16    d13,d5,d6   @ pb pc
	 
	 vand.s16    d8,d8,d9       @make0 a
	 vand.s16    d9,d11,d10     @make1 b 
	 vand.s16    d10,d12,d13    @make0 c  
	 
	 vand.s16    d22,d22,d8  @a
	 vand.s16    d21,d21,d9  @b 
	 vand.s16    d20,d20,d10 @c
	 
	 vadd.s16	d31,d20,d21 
	 vadd.s16	d31,d31,d22	 
	 
	 vadd.s16	d31,d31,d23
	 @@@@@@@@@@@@@@@@@@@@@@@@@@@	 
	 vmovn.u16  d0,q15	 	  
	 vst1.u8   d0,[r2]
	 subs r3,r3,#8	 
	 add  r0,r0,#8
	 add  r1,r1,#8
	 add  r2,r2,#8
	 bne LOOP 
	 pop             {pc,r4}

	@ENDP 
    
png_filter_value_paeth_4_armv7 @PROC
	 push            {lr}
	 vld1.u8     {d0}, [r0] @d0: rgba rgba
	 vld1.u8     {d2}, [r1] @d2: rgba xxxx
	 vld1.u8     {d20}, [r2] @d20: rgba xxxx
	 
	 vmovl.u8    q0,d0
	 vmovl.u8    q1,d2 
	 
	 vabd.s16    d4,d1,d0  @ |b-c|
	 vabd.s16    d5,d2,d0  @ |a-c|
	 
	 vadd.s16    d7,d2,d1  @ a+b
	 vsub.s16    d6,d7,d0  @ a+b-c
	 vabd.s16    d6,d6,d0  @ |a+b-c-c|
	 @ d0 d1 d2 -->a  b  c	
	 @ d4 d5 d6 -->pa pb pc 
	 vcle.s16    d8,d4,d5   @ pa pb
	 vcle.s16    d9,d4,d6   @ pa pc
	 vcle.s16    d10,d5,d6   @ pb pc
	 @ d7 d8 d9 -->
	 vcgt.s16    d11,d4,d5   @ pa pb
	 vcgt.s16    d12,d4,d6   @ pa pc
	 vcgt.s16    d13,d5,d6   @ pb pc
	 
	 vand.s16    d8,d8,d9      @make0 a
	 vand.s16    d9,d11,d10     @make1 b 
	 vand.s16    d10,d12,d13    @make0 c  
	 
	 vand.s16    d2,d2,d8  @a
	 vand.s16    d1,d1,d9  @b
	 vand.s16    d0,d0,d10 @c
	 
	 vadd.s16	d0,d0,d1 
	 vadd.s16	d0,d0,d2 
	 
	 vmovn.u16  d0,q0
	 
	 vswp.64    d2,d3
	 vmovn.u16  d2,q1
	 
	 vadd.u8    d0,d0,d2
	 
	 vtrn.32    d20, d22	 
	 vtrn.32    d0, d22
	 
	 	  
	 vst1.u8   d0,[r2]	 
	 pop             {pc}

	@ENDP 
    
		.ALIGN 4	
W_table:	
		.word 0x00FF00FF
		.word 0x00FF00FF
    @END
 