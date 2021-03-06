;
;  Copyright (c) 2010 The VP8 project authors. All Rights Reserved.
;
;  Use of this source code is governed by a BSD-style license and patent
;  grant that can be found in the LICENSE file in the root of the source
;  tree. All contributing project authors may be found in the AUTHORS
;  file in the root of the source tree.
;
		AREA |.text|, CODE, READONLY

    EXPORT  vp8_dequantize_b_loop_neon
    ;ARM
    ;REQUIRE8
    ;PRESERVE8

    
; r0    short *Q,
; r1    short *DQC
; r2    short *DQ
vp8_dequantize_b_loop_neon PROC
		stmdb   	sp!, {r4 - r8, lr}
    vld1.16         {q0, q1}, [r0]
    vld1.16         {q2, q3}, [r1]

    vmul.i16        q4, q0, q2
    vmul.i16        q5, q1, q3

    vst1.16         {q4, q5}, [r2]

    ldmia   	sp!, {r4 - r8, pc}
    ;bx             lr

    ENDP

    END
