@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@*   VisualOn, Inc. Confidential and Proprietary, 2003-2010 
@*  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@void Residu (
@    Word16 a[], /* (i)     : prediction coefficients                      */
@    Word16 x[], /* (i)     : speech signal                                */
@    Word16 y[], /* (o)     : residual signal                              */
@    Word16 lg   /* (i)     : size of filtering                            */
@)

@a[]        RN     r0
@x[]        RN     r1
@y[]        RN     r2
@lg         RN     r3
        #include "voAMRWBEncID.h"
	.text     .align 4
        .globl    _Residu_opt 

_Residu_opt:

        stmfd          r13!, {r4 - r12, r14} 
        sub            r7, r3, #4                       @i = lg - 4
        
        vld1.s16       {d0, d1, d2, d3}, [r0]!              @get all a[]  
	vld1.s16       {d4}, [r0]!
        vmov.s32       q8,  #0x8000
        
LOOP1:
        add            r9, r1, r7, lsl #1               @copy the address
        add            r10, r2, r7, lsl #1
        mov            r8, r9
        vld1.s16       d5, [r8]!                       @get x[i], x[i+1], x[i+2], x[i+3]
        vqdmull.s16    q10, d5, d0[0]                  @finish the first L_mult

        sub            r8, r9, #2                       @get the x[i-1] address
        vld1.s16       d5, [r8]! 
        vqdmlal.s16    q10, d5, d0[1]

        sub            r8, r9, #4                       @load the x[i-2] address
        vld1.s16       d5, [r8]!
        vqdmlal.s16    q10, d5, d0[2]

        sub            r8, r9, #6                       @load the x[i-3] address
        vld1.s16       d5, [r8]!     
        vqdmlal.s16    q10, d5, d0[3]                    

        sub            r8, r9, #8                       @load the x[i-4] address
        vld1.s16       d5, [r8]!     
        vqdmlal.s16    q10, d5, d1[0]  

        sub            r8, r9, #10                      @load the x[i-5] address
        vld1.s16       d5, [r8]!     
        vqdmlal.s16    q10, d5, d1[1] 

        sub            r8, r9, #12                      @load the x[i-6] address
        vld1.s16       d5, [r8]!     
        vqdmlal.s16    q10, d5, d1[2]  

        sub            r8, r9, #14                      @load the x[i-7] address
        vld1.s16       d5, [r8]!     
        vqdmlal.s16    q10, d5, d1[3]  

        sub            r8, r9, #16                      @load the x[i-8] address
        vld1.s16       d5, [r8]!     
        vqdmlal.s16    q10, d5, d2[0]  

        sub            r8, r9, #18                      @load the x[i-9] address
        vld1.s16       d5, [r8]!     
        vqdmlal.s16    q10, d5, d2[1]         
           
        sub            r8, r9, #20                      @load the x[i-10] address
        vld1.s16       d5, [r8]!     
        vqdmlal.s16    q10, d5, d2[2]  

	sub            r8, r9, #22                      @load the x[i-11] address
	vld1.s16       d5, [r8]!
	vqdmlal.s16    q10, d5, d2[3]

	sub            r8, r9, #24                      @load the x[i-12] address
	vld1.s16       d5, [r8]!
	vqdmlal.s16    q10, d5, d3[0]

	sub            r8, r9, #26                      @load the x[i-13] address
	vld1.s16       d5, [r8]!
	vqdmlal.s16    q10, d5, d3[1]

	sub            r8, r9, #28                      @load the x[i-14] address
	vld1.s16       d5, [r8]!
	vqdmlal.s16    q10, d5, d3[2]

	sub            r8, r9, #30                      @load the x[i-15] address
	vld1.s16       d5, [r8]!
	vqdmlal.s16    q10, d5, d3[3]

	sub            r8, r9, #32                      @load the x[i-16] address
	vld1.s16       d5, [r8]!
	vqdmlal.s16    q10, d5, d4[0]

        sub            r7, r7, #4                       @i-=4
        vqshl.s32      q10, q10, #4
        vqadd.s32      q10, q10, q8
        vshrn.s32      d5, q10, #16
        vst1.s16       d5, [r10]!
        cmp            r7,  #0

        bge            LOOP1

Residu_asm_end: 
 
        ldmfd      r13!, {r4 - r12, r15}
    
        @ENdFUNC
        @.ENd


