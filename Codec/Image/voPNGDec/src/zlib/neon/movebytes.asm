;
;  Copyright (c) 2010 The VP8 project authors. All Rights Reserved.
;
;  Use of this source code is governed by a BSD-style license and patent
;  grant that can be found in the LICENSE file in the root of the source
;  tree. All contributing project authors may be found in the AUTHORS
;  file in the root of the source tree.

    ; ARM
    ; REQUIRE8
    ; PRESERVE8

    AREA    ||.text||, CODE, READONLY ; name this block of code
    EXPORT  |png_move_bytes_32_armv7|
    EXPORT  |png_move_bytes_16_armv7|
    EXPORT  |png_move_bytes_8_armv7|
    EXPORT  |png_move_bytes_1_16_armv7|
    EXPORT  |png_move_bytes_1_32_armv7|
    EXPORT  |png_move_bytes_2_32_armv7|
;void png_move_bytes_16_armv7 (unsigned char *sp,unsigned char *dp,unsigned long length)
;void png_move_bytes_8_armv7 (unsigned char *sp,unsigned char *dp,unsigned long length)
;
;
;-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
|png_move_bytes_32_armv7| PROC
	push            {lr}
	
LOOP32	
	vld1.u8 			q0,[r0]!
	vld1.u8 			q1,[r0]!
	subs          r2,r2,#32
	vst1.u8 			q0,[r1]!	
	vst1.u8 			q1,[r1]!
	bne LOOP32	
	
	pop             {pc}
	ENDP
|png_move_bytes_16_armv7| PROC
	push            {lr}
	
LOOP16	
	vld1.u8 			q0,[r0]!
	subs          r2,r2,#16
	vst1.u8 			q0,[r1]!	
	bne LOOP16	
	
	pop             {pc}
	ENDP
	
	
|png_move_bytes_8_armv7| PROC
	push            {lr}
	
	vld1.u8 			d0,[r0]!
	vst1.u8 			d0,[r1]!	
	
	pop             {pc}
	ENDP

|png_move_bytes_1_16_armv7| PROC
	push            {lr}

LOOP1_16
	vld1.u8 			d0,[r0]!
	vdup.u8       q1,d0[0]
	subs          r2,r2,#16
	vst1.u8 			q1,[r1]!	
	bne           LOOP1_16
	
	pop             {pc}
	ENDP
	
|png_move_bytes_1_32_armv7| PROC
	push            {lr}

LOOP1_32
	vld1.u8 			d0,[r0]!
	vdup.u8       q1,d0[0]
	subs          r2,r2,#32
	vst1.u8 			q1,[r1]!	
	vst1.u8 			q1,[r1]!
	bne           LOOP1_32
	
	pop             {pc}
	ENDP
	
|png_move_bytes_2_32_armv7| PROC
	push            {lr}

LOOP2_32
	vld1.u16 			d0,[r0]!
	vdup.u16      q1,d0[0]
	subs          r2,r2,#32
	vst1.u8 			q1,[r1]!	
	vst1.u8 			q1,[r1]!
	bne           LOOP2_32
	
	pop             {pc}
	ENDP
	

	END