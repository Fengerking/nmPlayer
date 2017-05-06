@
@  Copyright (c) 2010 The VP8 project authors. All Rights Reserved.
@
@  Use of this source code is governed by a BSD-style license and patent
@  grant that can be found in the LICENSE file in the root of the source
@  tree. All contributing project authors may be found in the AUTHORS
@  file in the root of the source tree.
	.section .text
	.global png_rgb_rgba_row_armv7
	.global png_rgb_argb_row_armv7
	.global png_gray_rgb_row_armv7
    
@void png_rgb_rgba_row_armv7 (unsigned char *sp,unsigned char *dp,unsigned long length)
@void png_rgb_rgba_row_armv7 (unsigned char *sp,unsigned char *dp,unsigned long length)
@-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
png_rgb_rgba_row_armv7: @PROC
	push            {lr}
	ldr         r3,=W_table
	
	vld3.u8     {d0[0],d1[0],d2[0]},[r0]!
	vld3.u8     {d0[1],d1[1],d2[1]},[r0]!
	vld3.u8     {d0[2],d1[2],d2[2]},[r0]!
	vld3.u8     {d0[3],d1[3],d2[3]},[r0]!
	vld1.u8     d3                 ,[r3]
	
	
	vst4.u8    {d0[0],d1[0],d2[0],d3[0]},[r1]!
	vst4.u8    {d0[1],d1[1],d2[1],d3[1]},[r1]!
	vst4.u8    {d0[2],d1[2],d2[2],d3[2]},[r1]!
	vst4.u8    {d0[3],d1[3],d2[3],d3[3]},[r1]!	
	
	pop             {pc}
	@ENDP
	
png_rgb_argb_row_armv7: @PROC
	push            {lr}
	ldr         r3,=W_table
	
	vld3.u8     {d1[0],d2[0],d3[0]},[r0]!
	vld3.u8     {d1[1],d2[1],d3[1]},[r0]!
	vld3.u8     {d1[2],d2[2],d3[2]},[r0]!
	vld3.u8     {d1[3],d2[3],d3[3]},[r0]!
	vld1.u8     d0                 ,[r3]
	
	
	vst4.u8    {d0[0],d1[0],d2[0],d3[0]},[r1]!
	vst4.u8    {d0[1],d1[1],d2[1],d3[1]},[r1]!
	vst4.u8    {d0[2],d1[2],d2[2],d3[2]},[r1]!
	vst4.u8    {d0[3],d1[3],d2[3],d3[3]},[r1]!	
	
	pop             {pc}
	@ENDP
	
png_gray_rgb_row_armv7:@ PROC
	push            {lr}
	
	vld1.u8     {d0},[r0]
	vmov         d1,d0
	vmov         d2,d0	
	
	vst3.u8     {d0[0],d1[0],d2[0]},[r1]!
	vst3.u8     {d0[1],d1[1],d2[1]},[r1]!
	vst3.u8     {d0[2],d1[2],d2[2]},[r1]!
	vst3.u8     {d0[3],d1[3],d2[3]},[r1]!
	vst3.u8     {d0[4],d1[4],d2[4]},[r1]!
	vst3.u8     {d0[5],d1[5],d2[5]},[r1]!
	vst3.u8     {d0[6],d1[6],d2[6]},[r1]!
	vst3.u8     {d0[7],d1[7],d2[7]},[r1]!
	
	pop             {pc}
	@ENDP
	
	
	
	
	
	.ALIGN 4	
W_table:
		.word 0xFFFFFFFF		
		.word 0xFFFFFFFF
	@END