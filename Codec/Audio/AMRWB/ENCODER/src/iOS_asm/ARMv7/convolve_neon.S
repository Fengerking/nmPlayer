@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@*   VisualOn, Inc. Confidential and Proprietary, 2003-2010 
@*  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@*void Convolve (
@*    Word16 x[],        /* (i)     : input vector                           */
@*    Word16 h[],        /* (i)     : impulse response                       */
@*    Word16 y[],        /* (o)     : output vector                          */
@*    Word16 L           /* (i)     : vector size                            */
@*)
@ 
@ r0 --- x[]
@ r1 --- h[]
@ r2 --- y[]
@ r3 --- L
        #include "voAMRWBEncID.h"
	.text   .align 4
        .globl   _voAMRWBEncConvolve_asm 

_voAMRWBEncConvolve_asm:

        stmfd          r13!, {r4 - r12, r14}  
        mov            r3,  #0                           @ n
	mov            r11, #0x8000
        
LOOP: 
        @mov            r8, #0                            @ s = 0
        add            r4, r1, r3, lsl #1                @ tmpH address
        add            r5, r3, #1                        @ i = n + 1
        mov            r6, r0
        ldrsh          r9,  [r6], #2                     @ *tmpX++
        ldrsh          r10, [r4]                         @ *tmpH--
        sub            r5, r5, #1
        vmov.s32       q10, #0 
        mul            r8,  r9, r10 

LOOP1:                    
        cmp            r5, #0
        ble            L1
        sub            r4, r4, #8
        mov            r9, r4
        vld1.s16       d0, [r6]!   
        vld1.s16       d1, [r9]!
        vrev64.16      d1, d1
        subs           r5, r5, #4  
        vmlal.s16      q10, d0, d1         
        b              LOOP1    
L1:                  
        vadd.s32       d20, d20, d21
        vpadd.s32      d20, d20, d20
        vmov.s32       r5, d20[0]
        add            r5, r5, r8
        add            r5, r11, r5, lsl #1
        mov            r5, r5, lsr #16                   @extract_h(s)
        add            r3, r3, #1
        strh           r5, [r2], #2                      @y[n]


        @mov            r8, #0
        add            r4, r1, r3, lsl #1                @tmpH address
        add            r5, r3, #1
        mov            r6, r0
        ldrsh          r9,  [r6], #2                     @ *tmpX++
        ldrsh          r10, [r4], #-2                     
        ldrsh          r12, [r6], #2
        ldrsh          r14, [r4]

        mul            r8, r9, r10
        sub            r5, r5, #2
        mla            r8, r12, r14, r8
        
        vmov.s32       q10, #0
LOOP2:
        cmp            r5, #0
        ble            L2
        sub            r4, r4, #8
        mov            r9, r4
        vld1.s16       d0, [r6]!   
        vld1.s16       d1, [r9]!
        subs           r5, r5, #4
        vrev64.16      d1, d1
        vmlal.s16      q10, d0, d1 
        b              LOOP2
L2:
        vadd.s32       d20, d20, d21
        vpadd.s32      d20, d20, d20
        vmov.s32       r5, d20[0]
        add            r8, r8, r5
        add            r8, r11, r8, lsl #1
        mov            r8, r8, lsr #16                   @extract_h(s)
        add            r3, r3, #1  
        strh           r8, [r2], #2                      @y[n]


        @mov            r8, #0
        add            r4, r1, r3, lsl #1
        add            r5, r3, #1
        mov            r6, r0
        ldrsh          r9,  [r6], #2
        ldrsh          r10, [r4], #-2
        ldrsh          r12, [r6], #2
        ldrsh          r14, [r4], #-2
        mul            r8, r9, r10
        ldrsh          r9,  [r6], #2
        ldrsh          r10, [r4]
        mla            r8, r12, r14, r8 
        sub            r5, r5, #3
        mla            r8, r9, r10, r8

        vmov.s32       q10, #0
LOOP3:
        cmp            r5, #0
        ble            L3
        sub            r4, r4, #8
        mov            r9, r4
        vld1.s16       d0, [r6]!   
        vld1.s16       d1, [r9]!
        vrev64.16      d1, d1
        subs           r5, r5, #4
        vmlal.s16      q10, d0, d1 
        b              LOOP3   

L3:
        vadd.s32       d20, d20, d21
        vpadd.s32      d20, d20, d20
        vmov.s32       r5, d20[0]
        add            r8, r8, r5
        add            r8, r11, r8, lsl #1
        mov            r8, r8, lsr #16                   @extract_h(s)
        add            r3, r3, #1
        strh           r8, [r2], #2                      @y[n]

        add            r5, r3, #1                        @ i = n + 1
        add            r4, r1, r5, lsl #1                @ tmpH address
        mov            r6, r0
        vmov.s32       q10, #0
LOOP4:                    
        cmp            r5, #0
        ble            L4
        sub            r4, r4, #8
        mov            r9, r4
        vld1.s16       d0, [r6]!   
        vld1.s16       d1, [r9]!
        vrev64.16      d1, d1
        subs           r5, r5, #4  
        vmlal.s16      q10, d0, d1         
        b              LOOP4    
L4:                  
        vadd.s32       d20, d20, d21
        vpadd.s32      d20, d20, d20
        vmov.s32       r5,  d20[0]
        add            r5, r11, r5, lsl #1
        mov            r5, r5, lsr #16                   @extract_h(s)
        add            r3, r3, #1
        strh           r5, [r2], #2                      @y[n]
        
        cmp            r3, #64
        blt            LOOP
                
Convolve_asm_end: 
 
        ldmfd      r13!, {r4 - r12, r15}
    
        @.end

