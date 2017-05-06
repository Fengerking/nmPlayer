;
;  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
;
;  Use of this source code is governed by a BSD-style license
;  that can be found in the LICENSE file in the root of the source
;  tree. An additional intellectual property rights grant can be found
;  in the file PATENTS.  All contributing project authors may
;  be found in the AUTHORS file in the root of the source tree.
;


    EXPORT  |memset_zero_neon|
    ARM
    REQUIRE8
    PRESERVE8
    AREA ||.text||, CODE, READONLY, ALIGN=2
;memset(xd->qcoeff,0,sizeof(short)*400)
|memset_zero_neon| PROC
    stmdb       sp!, {lr}

    vst1.u8     {q0}, [r1]!    

    ldmia       sp!, {pc}
    ENDP        ;

    END
