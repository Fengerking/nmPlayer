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
    EXPORT  |reset_prefix_64_armv7|
    EXPORT  |reset_prefix_32_armv7|
    EXPORT  |reset_prefix_16_armv7|
    EXPORT  |reset_prefix_8_armv7|
    EXPORT  |copy_mem16x16_neon|
    EXPORT  |move_bytes_32_armv7| 
;void png_reset_prefix_armv7 (unsigned char *dp,unsigned long length)
;void copy_mem16x16_neon( unsigned char *src, int src_stride, unsigned char *dst, int dst_stride)
;-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
|move_bytes_32_armv7| PROC
	push            {lr}
	
LOOP312	
	vld1.u8 			q0,[r0]!
	vld1.u8 			q1,[r0]!
	subs          r2,r2,#32
	vst1.u8 			q0,[r1]!	
	vst1.u8 			q1,[r1]!
	bne LOOP312	
	
	pop             {pc}
	ENDP
|reset_prefix_64_armv7| PROC
	push            {lr}
	
	
	ldr         r2,  =W_table
	vld1.u32   q0,[r2] 
	vld1.u32   q1,[r2]
	vld1.u32   q2,[r2]
	vld1.u32   q3,[r2]

LOOP

	subs        r1,r1,#64
	vst1.u32   	q0,[r0]!
	vst1.u32   	q1,[r0]!
	vst1.u32   	q2,[r0]!
	vst1.u32   	q3,[r0]!
	vst1.u32   	q0,[r0]!
	vst1.u32   	q1,[r0]!
	vst1.u32   	q2,[r0]!
	vst1.u32   	q3,[r0]!
	vst1.u32   	q0,[r0]!
	vst1.u32   	q1,[r0]!
	vst1.u32   	q2,[r0]!
	vst1.u32   	q3,[r0]!
	vst1.u32   	q0,[r0]!
	vst1.u32   	q1,[r0]!
	vst1.u32   	q2,[r0]!
	vst1.u32   	q3,[r0]!	
	
	bne LOOP
	
	pop             {pc}
	ENDP

|reset_prefix_32_armv7| PROC
	push            {lr}		
	ldr         r2,  =W_table
	vld1.u32   q0,[r2] 
	vld1.u32   q1,[r2]
LOOP32
	subs        r1,r1,#32
	vst1.u32   	q0,[r0]!
	vst1.u32   	q1,[r0]!
	vst1.u32   	q0,[r0]!
	vst1.u32   	q1,[r0]!
	vst1.u32   	q0,[r0]!
	vst1.u32   	q1,[r0]!
	vst1.u32   	q0,[r0]!
	vst1.u32   	q1,[r0]!	
	bne LOOP32	
	pop             {pc}
	ENDP
	
|reset_prefix_16_armv7| PROC
	push            {lr}		
	ldr         r2,  =W_table
	vld1.u32   q0,[r2] 
	vld1.u32   q1,[r2]
LOOP16
	subs        r1,r1,#16
	vst1.u32   	q0,[r0]!
	vst1.u32   	q1,[r0]!
	vst1.u32   	q0,[r0]!
	vst1.u32   	q1,[r0]!	
	bne LOOP16	
	pop             {pc}
	ENDP
	
|reset_prefix_8_armv7| PROC
	push            {lr}		
	ldr         r2,  =W_table
	vld1.u32   q0,[r2] 
	vld1.u32   q1,[r2]
LOOP8
	subs        r1,r1,#8
	vst1.u32   	q0,[r0]!
	vst1.u32   	q1,[r0]!	
	bne LOOP8
	pop             {pc}
	ENDP

|copy_mem16x16_neon| PROC

    vld1.u8     {q0}, [r0], r1
    vld1.u8     {q1}, [r0], r1
    vld1.u8     {q2}, [r0], r1
    vst1.u8     {q0}, [r2], r3
    vld1.u8     {q3}, [r0], r1
    vst1.u8     {q1}, [r2], r3
    vld1.u8     {q4}, [r0], r1
    vst1.u8     {q2}, [r2], r3
    vld1.u8     {q5}, [r0], r1
    vst1.u8     {q3}, [r2], r3
    vld1.u8     {q6}, [r0], r1
    vst1.u8     {q4}, [r2], r3
    vld1.u8     {q7}, [r0], r1
    vst1.u8     {q5}, [r2], r3
    vld1.u8     {q8}, [r0], r1
    vst1.u8     {q6}, [r2], r3
    vld1.u8     {q9}, [r0], r1
    vst1.u8     {q7}, [r2], r3
    vld1.u8     {q10}, [r0], r1
    vst1.u8     {q8}, [r2], r3
    vld1.u8     {q11}, [r0], r1
    vst1.u8     {q9}, [r2], r3
    vld1.u8     {q12}, [r0], r1
    vst1.u8     {q10}, [r2], r3
    vld1.u8     {q13}, [r0], r1
    vst1.u8     {q11}, [r2], r3
    vld1.u8     {q14}, [r0], r1
    vst1.u8     {q12}, [r2], r3
    vld1.u8     {q15}, [r0], r1
    vst1.u8     {q13}, [r2], r3
    vst1.u8     {q14}, [r2], r3
    vst1.u8     {q15}, [r2], r3

    mov     pc, lr

   ENDP  ; |copy_mem16x16_neon|
	
	ALIGN 4	
W_table	
		dcd 0x00001002		
		dcd 0x00001002
		dcd 0x00001002		
		dcd 0x00001002

	END