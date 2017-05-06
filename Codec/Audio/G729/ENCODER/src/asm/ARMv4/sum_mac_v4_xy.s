;**************************************************************
;* Copyright 2003~2009 by VisualOn Corporation, Inc.
;* All modifications are confidential and proprietary information
;* of Visualon Corporation, Inc. ALL RIGHTS RESERVED.
;****************************************************************
;#if OPT_ASMV4 //second opt
;        s = sum_mac_v4_xy(s, xn, s_excf, L_subfr);
;#else	
;		for (j = 0; j < L_subfr; j++) {
;			s = L_mac (s, xn[j], s_excf[j]);
;		}
;#endif
; r0 --- s
; r1 --- xn
; r2 --- s_excf

       AREA    |.text|, CODE, READONLY
       EXPORT sum_mac_v4_xy


sum_mac_v4_xy    PROC

       STMDB      sp!, {r4 - r12, lr}
       and     r8, r3, #7                        
       subs    r3, r3, r8                     
       beq     L5                             
L2                         
L3                          
        ldrsh		r10, [r1],#2
        ldrsh           r7, [r2],#2

        ldrsh		r6, [r1],#2 
        ldrsh           r9, [r2],#2
	MUL		r4, r10, r7
        MOVS            r4, r4, LSL #1
        MOVVS           r4, #0x7fffffff
        ADDS            r0, r0, r4
        bvc             NO1
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO1
	MUL		r5, r6, r9
        ldrsh		r7, [r1],#2
        MOVS            r5, r5, LSL #1
        ldrsh           r10, [r2],#2
        MOVVS           r5, #0x7fffffff
        ADDS            r0, r0, r5
        bvc             NO2
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO2
	MUL		r4, r7, r10
        ldrsh		r6, [r1],#2
        MOVS            r4, r4, LSL #1
        ldrsh           r9, [r2],#2
        MOVVS           r4, #0x7fffffff
        ADDS            r0, r0, r4
        bvc             NO3
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO3
	MUL		r5, r6, r9
        ldrsh		r7, [r1],#2
        MOVS            r5, r5, LSL #1
        ldrsh           r10, [r2],#2
        MOVVS           r5, #0x7fffffff
        ADDS            r0, r0, r5
        bvc             NO4
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO4
	MUL		r4, r7, r10
        ldrsh		r6, [r1],#2
        MOVS            r4, r4, LSL #1
        ldrsh           r9, [r2],#2
        MOVVS           r4, #0x7fffffff
        ADDS            r0, r0, r4
        bvc             NO5
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO5
	MUL		r5, r6, r9
        ldrsh		r7, [r1],#2
        MOVS            r5, r5, LSL #1
        ldrsh           r10, [r2],#2
        MOVVS           r5, #0x7fffffff
        ADDS            r0, r0, r5
        bvc             NO6
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO6
	MUL		r4, r7, r10
        ldrsh		r6, [r1],#2
        MOVS            r4, r4, LSL #1
        ldrsh           r9, [r2],#2
        MOVVS           r4, #0x7fffffff
        ADDS            r0, r0, r4
        bvc             NO7
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO7
	MUL		r5, r6, r9
        MOVS            r5, r5, LSL #1
        MOVVS           r5, #0x7fffffff
        ADDS            r0, r0, r5
        bvc             NO8
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO8
        subs		r3, r3, #8
	bne		L3
L5  
	cmp			r8, #0                            
        beq			L9                             
L6                         
L7                          
        ldrsh		r10, [r1], #2 
        ldrsh           r7, [r2], #2

	MUL		r5, r10, r7
        MOVS            r5, r5, LSL #1
        MOVVS           r5, #0x7fffffff
        ADDS            r0, r0, r5
        bvc             NO9
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO9 
        subs		r8, r8, #1
        bne		L7                                                                         
L9  
        LDMIA      sp!, {r4 - r12, pc}
        ENDP
        END
