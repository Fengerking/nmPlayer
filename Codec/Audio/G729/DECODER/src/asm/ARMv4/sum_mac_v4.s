;**************************************************************
;* Copyright 2008 by VisualOn Software, Inc.
;* All modifications are confidential and proprietary information
;* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
;***************************************************************   

         AREA   |.text|, CODE, READONLY
	     EXPORT sum_mac_v4   
	           
sum_mac_v4 PROC
        stmdb   sp!, {r4 - r12, lr}       
        and     r8, r2, #7                        
        subs    r2, r2, r8
        beq     L1_5                             
                       
L1_2                          
L1_3                          
        ldrsh		r3, [r1],#2
        ldrsh		r6, [r1],#2
	MUL		r4, r3, r3
        MOVS            r4, r4, LSL #1
        MOVVS           r4, #0x7fffffff
        ADDS            r0, r0, r4
        bvc             NO1
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO1
	MUL		r5, r6, r6
        ldrsh		r7, [r1],#2
        MOVS            r5, r5, LSL #1
        MOVVS           r5, #0x7fffffff
        ADDS            r0, r0, r5
        bvc             NO2
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO2
	MUL		r4, r7, r7
        ldrsh		r3, [r1],#2
        MOVS            r4, r4, LSL #1
        MOVVS           r4, #0x7fffffff
        ADDS            r0, r0, r4
        bvc             NO3
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO3
	MUL		r5, r3, r3
        ldrsh           r6, [r1],#2
        MOVS            r5, r5, LSL #1
        MOVVS           r5, #0x7fffffff
        ADDS            r0, r0, r5
        bvc             NO4
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO4
	MUL		r5, r6, r6
        ldrsh		r7, [r1],#2
        MOVS            r5, r5, LSL #1
        MOVVS           r5, #0x7fffffff
        ADDS            r0, r0, r5
        bvc             NO5
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO5
	MUL		r4, r7, r7
        ldrsh		r3, [r1],#2
        MOVS            r4, r4, LSL #1
        MOVVS           r4, #0x7fffffff
        ADDS            r0, r0, r4
        bvc             NO6
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO6
	MUL		r5, r3, r3
        ldrsh           r6, [r1],#2
        MOVS            r5, r5, LSL #1
        MOVVS           r5, #0x7fffffff
        ADDS            r0, r0, r5
        bvc             NO7
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO7
	MUL		r5, r6, r6
        MOVS            r5, r5, LSL #1
        MOVVS           r5, #0x7fffffff
        ADDS            r0, r0, r5
        bvc             NO8
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO8
        subs		r2, r2, #8
	bne		L1_3
L1_5  
	cmp			r8, #0                            
        beq			L1_9                             
L1_6                          
L1_7                          
        ldrsh		r3, [r1], #2 
        subs		r8, r8, #1
	MUL		r5, r3, r3
        MOVS            r5, r5, LSL #1
        MOVVS           r5, #0x7fffffff
        ADDS            r0, r0, r5
        bvc             NO9
        movmi           r0, #0x7fffffff
        movpl           r0, #0x80000000
NO9 
        subs		r8, r8, #1
        bne		L1_7                                                                         
L1_9  
                       
        ldmia     sp!, {r4 - r12, pc}   
        endp

        end    



      

