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
    @EXPORT  |expend_84_armv7|
    @EXPORT  |expend_44_armv7|
	.section .text
	.align 4
	.global expend_84_armv7
	.global expend_44_armv7
@void expend_84_armv7 
@ unsigned char* sp
@ unsigned char* dp
@ unsigned long width
@-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
expend_84_armv7: @PROC
	push        {lr}
	vld1.u8 	q0,[r0]
	vdup.32		q1,d0[0]     
	vdup.32		q2,d0[1]  
	vdup.32		q3,d1[0]
	vdup.32		q4,d1[1]
	
	vst1.u8		q1,[r1]!
	vst1.u8		q1,[r1]!
	
	vst1.u8		q2,[r1]!
	vst1.u8		q2,[r1]!
	
	vst1.u8		q3,[r1]!
	vst1.u8		q3,[r1]!
	
	vst1.u8		q4,[r1]!
	vst1.u8		q4,[r1]!
		

	
	pop             {pc}
	@ENDP	
	
expend_44_armv7: @PROC
	push        {lr}
	vld1.u8 	q0,[r0]
	vdup.32		q1,d0[0]     
	vdup.32		q2,d0[1]  
	vdup.32		q3,d1[0]
	vdup.32		q4,d1[1]
	
	vst1.u8		q1,[r1]!	
	vst1.u8		q2,[r1]!	
	vst1.u8		q3,[r1]!	
	vst1.u8		q4,[r1]!
	
	pop             {pc}
	@ENDP

	@END