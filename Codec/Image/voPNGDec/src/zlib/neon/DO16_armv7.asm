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
    EXPORT  |DO16_armv7|
    EXPORT  |DO32_armv7|
;void DO16_armv7 
; unsigned char* buf
; unsigned long  *alder
; unsigned long  *sum2
; unsigned long  len
;-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
|DO16_armv7| PROC
	push        {lr}	
	
	ldr			r4,=W_table16
	
	
	vld1.u32    d27,[r1] ;alder
	vld1.u8     q10,[r4]
	vld1.u32    d29,[r2] ;sum2

LOOP16	
	vld1.u8 	q0,[r0]!
	
	vaddl.u8    q1,d0,d1	
	vmull.u8    q11,d20,d0
	vpadd.u16   d2,d2,d3
	vmlal.u8    q11,d21,d1
	vpadd.u16   d2,d2
	vshl.u32    d26,d27,#4  ;d26 16*adler
	vpaddl.u16  d2,d2   ;d2: buf[0]+...+buf[15]	
	

	vpadd.u16   d22,d22,d23
	subs       r3,r3,#16
	vpadd.u16   d22,d22
	vadd.u32   d27,d27,d2    ;adler = adler + buf[0]+...+buf[15]
	vpaddl.u16   d22,d22	;d22 16*buf[0]+...+1*buf[15]	
	
	vadd.u32   d29,d29,d26  ;  sum2 + 16*adler
	vadd.u32   d29,d29,d22   ; sum2 = 16*buf[0]+...+1*buf[15] + 16*adler + sum2	
	
	bne        LOOP16
	
	vst1.u32   d27[0],[r1]
	vst1.u32   d29[0],[r2]
	
	pop             {pc}
	ENDP
	
|DO32_armv7| PROC
	push        {lr}	
	
	ldr			r4,=W_table32
	
	
	vld1.u32    d27,[r1] ;alder
	vld1.u8     q10,[r4]!
	vld1.u8     q7,[r4]!
	vld1.u32    d29,[r2] ;sum2

LOOP32	
	vld1.u8 	q0,[r0]!
	vld1.u8 	q5,[r0]!
	
	vaddl.u8    q1,d0,d1	
	vaddl.u8    q6,d10,d11	
	vpadd.u16   d2,d2,d3
	vpadd.u16   d12,d12,d13
	vpadd.u16   d2,d2
	vpadd.u16   d12,d12
	vpaddl.u16   d2,d2   ;d2: buf[0]+...+buf[15]	
	
	;vaddl.u8    q6,d10,d11	
	;vpadd.u16   d12,d12,d13
	;vpadd.u16   d12,d12
	vpaddl.u16  d12,d12   ;d12: buf[16]+...+buf[31]
	
	
	;vadd.u32   d2,d2,d12
	
	vmull.u8    q11,d20,d0
	vmull.u8    q12,d14,d10
	vmlal.u8    q11,d21,d1	
	vmlal.u8    q12,d15,d11
	
	vadd.u32   d2,d2,d12
	vpadd.u16   d22,d22,d23
	vpadd.u16   d24,d24,d25
	vpadd.u16   d22,d22
	vpadd.u16   d24,d24
	vpaddl.u16   d22,d22	;d22 16*buf[0]+...+1*buf[15]
	
	;vmull.u8    q12,d14,d10
	;vmlal.u8    q12,d15,d11	
	;vpadd.u16   d24,d24,d25
	;vpadd.u16   d24,d24
	vpaddl.u16   d24,d24	;d24 16*buf[0]+...+1*buf[15]
	
	;vadd.u32   d22,d22,d24
	
	vshl.u32    d26,d27,#5  ;d26 32*adler
	vadd.u32   d22,d22,d24
	subs       r3,r3,#32
	
	vadd.u32   d26,d22,d26  ; 16*buf[0]+...+1*buf[15] + 16*adler
	vadd.u32   d27,d27,d2    ;adler = adler + buf[0]+...+buf[15]
	vadd.u32   d29,d29,d26   ; sum2 = 16*buf[0]+...+1*buf[15] + 16*adler + sum2
	
	;vadd.u32   d27,d27,d2    ;adler = adler + buf[0]+...+buf[15]
	
	;subs       r3,r3,#32
	bne        LOOP32
	
	vst1.u32   d27[0],[r1]
	vst1.u32   d29[0],[r2]
	
	pop             {pc}
	ENDP	

		ALIGN 4	
W_table32	
		dcd 0x1D1E1F20
		dcd 0x191A1B1C
		DCD 0X15161718
		DCD 0X11121314
		
		dcd 0x0D0E0F10
		dcd 0x090A0B0C
		DCD 0X05060708
		DCD 0X01020304

W_table16
		dcd 0x0D0E0F10
		dcd 0x090A0B0C
		DCD 0X05060708
		DCD 0X01020304

	END