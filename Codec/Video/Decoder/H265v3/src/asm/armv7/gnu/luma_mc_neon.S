@ This file was created from a .asm file
@  using the ads2gas.pl script.
	.equ DO1STROUNDING, 0
@************************************************************************
@ VisualOn Proprietary
@ Copyright (c) 2012, VisualOn Incorporated. All rights Reserved
@
@ VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
@
@ All data and information contained in or disclosed by this document are
@ confidential and proprietary information of VisualOn, and all rights
@ therein are expressly reserved. By accepting this material, the
@ recipient agrees that this material and the information contained
@ therein are held in confidence and in trust. The material may only be
@ used and/or disclosed as authorized in a license agreement controlling
@ such use and disclosure.
@************************************************************************

#include     "h265dec_ASM_config.h"
#include "../../../h265dec_ID.h"
.text
.if MC_ASM_ENABLED==1
.p2align 2
    
    .global MC_InterLuma_4x4_neon
    .global MC_InterLuma_8x8_neon
    
    .global MC_InterLumaBi_4x4_neon
    .global MC_InterLumaBi_8x8_neon

    .type MC_InterLuma_4x4_neon, function
    .type MC_InterLuma_8x8_neon, function

    .type MC_InterLumaBi_4x4_neon, function
    .type MC_InterLumaBi_8x8_neon, function
    
LumaCoeff:
    .short       0,   0,   0,  64,   0,   0,   0,   0
    .short      -1,   4, -10,  58,  17,  -5,   1,   0
    .short      -1,   4, -11,  40,  40, -11,   4,  -1 
    .short       0,   1,  -5,  17,  58, -10,   4,  -1
    
@-------------------------------------------------------
 .macro Back_3Line
    
    sub             r0, r0, r1, lsl #1  @go back 3 lines of src data
    sub             r0, r0, r1 
 .endm
 
 .macro Back_3Column
    
    sub             r0, r0, #3       @go back 3 columns of src data
 .endm
 
 .macro Back_3Line_3Column
    
    sub             r0, r0, r1, lsl #1  @go back 3 lines of src data
    sub             r0, r0, r1
    
    sub             r0, r0, #3       @go back 3 columns of src data
 .endm

 .macro YDuplicateCoeff_8
    
    vdup.8          d0, d8[0]        @coeff[0]
    vdup.8          d1, d8[2]
    vdup.8          d2, d8[4]
    vdup.8          d3, d8[6]
    vdup.8          d4, d9[0]
    vdup.8          d5, d9[2]
    vdup.8          d6, d9[4]
    vdup.8          d7, d9[6]
 .endm
 
 .macro YDuplicateCoeff_16
    
    vdup.s16        d0, d8[0]        @coeff[0]
    vdup.s16        d1, d8[1]
    vdup.s16        d2, d8[2]
    vdup.s16        d3, d8[3]
    vdup.s16        d4, d9[0]
    vdup.s16        d5, d9[1]
    vdup.s16        d6, d9[2]
    vdup.s16        d7, d9[3]
 .endm
  
 .macro YLoadSource_FirstPass_11x4
    
    vld1.u8         {q4}, [r0], r1      @load first 4-line src data
    vld1.u8         {q5}, [r0], r1
    vld1.u8         {q6}, [r0], r1
    vld1.u8         {q7}, [r0], r1
 .endm
 
 .macro YLoadSource_FirstPass_11x3
    
    vld1.u8         {q4}, [r0], r1      @load the rest 3-line src data
    vld1.u8         {q5}, [r0], r1
    vld1.u8         {q6}, [r0], r1
 .endm
 
 .macro YInterpolate_FirstPass_4x4
    
    vshr.u64        d20, d8, #32        @construct src_ptr[1]
    vshr.u64        d21, d10, #32
    vshr.u64        d22, d12, #32
    vshr.u64        d23, d14, #32

    vzip.32         d20, d21            @put 2-line data in 1 register (src_ptr[1])
    vzip.32         d22, d23         

    vmull.u8        q8, d20, d4         @+(src_ptr[1] * coeff[4])
    vmull.u8        q9, d22, d4

    vext.8          d20, d8,  d9,  #5   @construct src_ptr[2]
    vext.8          d21, d10, d11, #5 
    vext.8          d22, d12, d13, #5 
    vext.8          d23, d14, d15, #5 

    vzip.32         d20, d21		    @put 2-line data together
    vzip.32         d22, d23

    vmlsl.u8        q8, d20, d5         @-(src_ptr[2] * coeff[5])
    vmlsl.u8        q9, d22, d5

    vext.8          d20, d8,  d9,  #6   @construct src_ptr[3]
    vext.8          d21, d10, d11, #6 
    vext.8          d22, d12, d13, #6 
    vext.8          d23, d14, d15, #6  

    vzip.32         d20, d21            @put 2-line data together
    vzip.32         d22, d23

    vmlal.u8        q8, d20, d6         @+(src_ptr[3] * coeff[6])
    vmlal.u8        q9, d22, d6

    vext.8          d20, d8,  d9,  #7   @construct src_ptr[4]
    vext.8          d21, d10, d11, #7 
    vext.8          d22, d12, d13, #7 
    vext.8          d23, d14, d15, #7 

    vzip.32         d20, d21            @put 2-line data in 1 register (src_ptr[4])
    vzip.32         d22, d23

    vmlsl.u8        q8, d20, d7         @-(src_ptr[4] * coeff[7])
    vmlsl.u8        q9, d22, d7   

@discard content in d9 d11 d13 d15, and let low part of q5 q7 in
@the high part of q4 q6 for parallel.
    vswp            d9, d10
    vswp            d13, d14

    vshr.u64        q10, q4, #24        @construct src_ptr[0]
    vshr.u64        q11, q6, #24
  
    vzip.32         d20, d21            @put 2-line data in 1 register (src_ptr[0])
    vzip.32         d22, d23

    vmlal.u8        q8, d20, d3         @+(src_ptr[0] * coeff[3])
    vmlal.u8        q9, d22, d3                  

    vshr.u64        q10, q4, #16        @construct src_ptr[-1]
    vshr.u64        q11, q6, #16

    vzip.32         d20, d21            @put 2-line data in 1 register (src_ptr[-1])
    vzip.32         d22, d23

    vmlsl.u8        q8, d20, d2         @-(src_ptr[-1] * coeff[2])
    vmlsl.u8        q9, d22, d2         

    vshr.u64        q10, q4, #8         @construct src_ptr[-2]
    vshr.u64        q11, q6, #8

    vzip.32         d20, d21            @put 2-line data in 1 register (src_ptr[-2])
    vzip.32         d22, d23

    vmlal.u8        q8, d20, d1         @+(src_ptr[-2] * coeff[1])
    vmlal.u8        q9, d22, d1 

    vzip.32         d8, d9              @construct src_ptr[-3], and put 2-line data in 1 register (src_ptr[-3])
    vzip.32         d12, d13

    vmlsl.u8        q8, d8, d0          @-(src_ptr[-3] * coeff[0]), sum of all (src_data*coeff)
    vmlsl.u8        q9, d12, d0 
 .endm
 
 .macro YBitShift_FirstPass_4x4
    
    vmov.s16		q10, #0x2000        @construct offset = -8192(-0x2000H)
    vmov.s16        q11, #0x2000
	     
    vsub.s16        q10, q8, q10        @sum of src_ptr[i]*coeff[i] + offset >> shift (shift = 0)
    vsub.s16        q11, q9, q11
 .endm
 
 .macro YInterpolate_FirstPassNext_4x4
    
    vshr.u64        d24, d8, #32        @construct src_ptr[1] 
    vshr.u64        d25, d10, #32
    vshr.u64        d26, d12, #32
    vshr.u64        d27, d14, #32

    vzip.32         d24, d25            @put 2-line data in 1 register (src_ptr[1])
    vzip.32         d26, d27         

    vmull.u8        q8, d24, d4         @+(src_ptr[1] * coeff[4])
    vmull.u8        q9, d26, d4 

    vext.8          d24, d8,  d9,  #5   @construct src_ptr[2]
    vext.8          d25, d10, d11, #5 
    vext.8          d26, d12, d13, #5 
    vext.8          d27, d14, d15, #5   

    vzip.32         d24, d25		    @put 2-line data together
    vzip.32         d26, d27		  

    vmlsl.u8        q8, d24, d5         @-(src_ptr[2] * coeff[5])
    vmlsl.u8        q9, d26, d5

    vext.8          d24, d8,  d9,  #6   @construct src_ptr[3]
    vext.8          d25, d10, d11, #6 
    vext.8          d26, d12, d13, #6 
    vext.8          d27, d14, d15, #6 

    vzip.32         d24, d25            @put 2-line data together
    vzip.32         d26, d27

    vmlal.u8        q8, d24, d6         @+(src_ptr[3] * coeff[6])
    vmlal.u8        q9, d26, d6

    vext.8          d24, d8,  d9,  #7   @construct src_ptr[4]
    vext.8          d25, d10, d11, #7 
    vext.8          d26, d12, d13, #7 
    vext.8          d27, d14, d15, #7 

    vzip.32         d24, d25            @put 2-line data in 1 register (src_ptr[4])
    vzip.32         d26, d27

    vmlsl.u8        q8, d24, d7         @-(src_ptr[4] * coeff[7])
    vmlsl.u8        q9, d26, d7   

@discard content in d9 d11 d13 d15, and let low part of q5 q7 in
@the high part of q4 q6 for parallel.
    vswp            d9, d10
    vswp            d13, d14

    vshr.u64        q12, q4, #24        @construct src_ptr[0]
    vshr.u64        q13, q6, #24
 
    vzip.32         d24, d25            @put 2-line data in 1 register (src_ptr[0]) 
    vzip.32         d26, d27

    vmlal.u8        q8, d24, d3         @+(src_ptr[0] * coeff[3])
    vmlal.u8        q9, d26, d3                  

    vshr.u64        q12, q4, #16        @construct src_ptr[-1]
    vshr.u64        q13, q6, #16

    vzip.32         d24, d25            @put 2-line data in 1 register (src_ptr[-1])
    vzip.32         d26, d27

    vmlsl.u8        q8, d24, d2         @-(src_ptr[-1] * coeff[2])
    vmlsl.u8        q9, d26, d2         

    vshr.u64        q12, q4, #8         @construct src_ptr[-2]
    vshr.u64        q13, q6, #8

    vzip.32         d24, d25            @put 2-line data in 1 register (src_ptr[-2])
    vzip.32         d26, d27

    vmlal.u8        q8, d24, d1         @+(src_ptr[-2] * coeff[1])
    vmlal.u8        q9, d26, d1 

    vzip.32         d8, d9              @construct src_ptr[-3], and put 2-line data in 1 register (src_ptr[-3])
    vzip.32         d12, d13

    vmlsl.u8        q8, d8, d0          @-(src_ptr[-3] * coeff[0]), sum of all (src_data*coeff)
    vmlsl.u8        q9, d12, d0         
 .endm
 
 .macro YBitShift_FirstPassNext_4x4
    
    vmov.s16        q12, #0x2000        @construct offset = -8192(-0x2000H)
    vmov.s16        q13, #0x2000        
     
    vsub.s16        q12, q8, q12        @sum of src_ptr[i]*coeff[i] + offset >> shift (shift = 0)	
    vsub.s16        q13, q9, q13 
 .endm
 
 .macro YInterpolate_FirstPassEnd_4x3
 
    vshr.u64        d28, d8, #32        @construct src_ptr[1] 
    vshr.u64        d29, d10, #32
    vshr.u64        d30, d12, #32

    vzip.32         d28, d29            @put 2-line data in 1 register (src_ptr[1])

    vmull.u8        q8, d28, d4         @+(src_ptr[1] * coeff[4])
    vmull.u8        q9, d30, d4

    vext.8          d28, d8,  d9,  #5   @construct src_ptr[2]
    vext.8          d29, d10, d11, #5 
    vext.8          d30, d12, d13, #5           

    vzip.32         d28, d29            @put 2-line data together

    vmlsl.u8        q8, d28, d5         @-(src_ptr[2] * coeff[5])
    vmlsl.u8        q9, d30, d5

    vext.8          d28, d8,  d9,  #6   @construct src_ptr[3]
    vext.8          d29, d10, d11, #6 
    vext.8          d30, d12, d13, #6 

    vzip.32         d28, d29	        @put 2-line data together

    vmlal.u8        q8, d28, d6         @+(src_ptr[3] * coeff[6])
    vmlal.u8        q9, d30, d6         

    vext.8          d28, d8,  d9,  #7   @construct src_ptr[4]
    vext.8          d29, d10, d11, #7 
    vext.8          d30, d12, d13, #7 

    vzip.32         d28, d29            @put 2-line data in 1 register (src_ptr[4])

    vmlsl.u8        q8, d28, d7         @-(src_ptr[4] * coeff[7])
    vmlsl.u8        q9, d30, d7 

@discard content in d9 d11 d13 d15, and let low part of q5 q7 in
@the high part of q4 q6 for parallel.
    vswp            d9, d10

    vshr.u64        q14, q4, #24        @construct src_ptr[0]
    vshr.u64        q15, q6, #24
 
    vzip.32         d28, d29            @put 2-line data in 1 register (src_ptr[0]) 

    vmlal.u8        q8, d28, d3         @+(src_ptr[0] * coeff[3])
    vmlal.u8        q9, d30, d3            

    vshr.u64        q14, q4, #16        @construct src_ptr[-1]
    vshr.u64        q15, q6, #16

    vzip.32         d28, d29            @put 2-line data in 1 register (src_ptr[-1])

    vmlsl.u8        q8, d28, d2         @-(src_ptr[-1] * coeff[2])
    vmlsl.u8        q9, d30, d2  

    vshr.u64        q14, q4, #8         @construct src_ptr[-2]
    vshr.u64        q15, q6, #8

    vzip.32         d28, d29            @put 2-line data in 1 register (src_ptr[-2])
@+(src_ptr[-2] * coeff[1])
    vmlal.u8        q8, d28, d1
    vmlal.u8        q9, d30, d1 
@-(src_ptr[-3] * coeff[0]), sum of all (src_data*coeff)
    vzip.32         d8, d9
         
    vmlsl.u8        q8, d8, d0
    vmlsl.u8        q9, d12, d0   
 .endm
 
 .macro YBitShift_FirstPassEnd_4x3
    
    vmov.s16        q14, #0x2000        @construct offset = -8192(-0x2000H)
    vmov.s16        q15, #0x2000        
     
    vsub.s16        q14, q8, q14        @sum of src_ptr[i]*coeff[i] + offset >> shift (shift = 0)	
    vsub.s16        q15, q9, q15    
 .endm
 
 .macro YLoadSource_SecondPassOnly_4x11
 
    vld1.u8         {d26}, [r0], r1     @load src data
    vld1.u8         {d27}, [r0], r1
    vzip.32         d26, d27            @put 2-line data in 1 register
    vld1.u8         {d27}, [r0], r1
    vld1.u8         {d28}, [r0], r1
    vzip.32         d27, d28
    vld1.u8         {d28}, [r0], r1
    vld1.u8         {d29}, [r0], r1
    vzip.32         d28, d29
    vld1.u8         {d29}, [r0], r1
    vld1.u8         {d30}, [r0], r1
    vzip.32         d29, d30
    vld1.u8         {d30}, [r0], r1
    vld1.u8         {d31}, [r0], r1
    vzip.32         d30, d31
    vld1.u8         {d31}, [r0]
                
    vext.8          d20, d26, d27, #4   @prepare other intermediate src arrays.  
    vext.8          d21, d27, d28, #4
    vext.8          d22, d28, d29, #4
    vext.8          d23, d29, d30, #4
    vext.8          d24, d30, d31, #4
 .endm
 
 .macro YInterpolate_SecondPass1st_4x2
 
    vmull.s16       q5, d24, d4        @+(src_ptr[1] * coeff[4])
    vmull.s16       q6, d25, d4
 
    vmlal.s16       q5, d25, d5        @-(src_ptr[2] * coeff[5])  
    vmlal.s16       q6, d26, d5     

    vmlal.s16       q5, d26, d6        @+(src_ptr[3] * coeff[6])
    vmlal.s16       q6, d27, d6

    vmlal.s16       q5, d27, d7        @-(src_ptr[4] * coeff[7])
    vmlal.s16       q6, d28, d7            

    vmlal.s16       q5, d23, d3        @+(src_ptr[0] * coeff[3])
    vmlal.s16       q6, d24, d3

    vmlal.s16       q5, d22, d2        @-(src_ptr[-1] * coeff[2])
    vmlal.s16       q6, d23, d2

    vmlal.s16       q5, d21, d1        @+(src_ptr[-2] * coeff[1])
    vmlal.s16       q6, d22, d1

    vmlal.s16       q5, d20, d0        @-(src_ptr[-3] * coeff[0]), sum of all (src_data*coeff)
    vmlal.s16       q6, d21, d0
 .endm
 
 .macro YBitShift_SecondPass1st_4x2
    
    vmov.u32        q4,  #0x80000   
         
    vadd.s32        q5, q4
    vadd.s32        q6, q4
    
    vqrshrun.s32    d18, q5, #12        @shift/round/saturate to u8, at this time only q9 is available.
    vqrshrun.s32    d19, q6, #12
 .endm
  
 .macro YStoreResult_1st_4x2
    
    vqmovn.u16       d18, q9

    vst1.32         {d18[0]}, [r2], r3
    vst1.32         {d18[1]}, [r2], r3  
 .endm
 
 .macro YInterpolate_SecondPass2nd_4x2
    
    vmull.s16       q7, d26, d4         @+(src_ptr[1] * coeff[4])
    vmull.s16       q8, d27, d4
   
    vmlal.s16       q7, d27, d5         @-(src_ptr[2] * coeff[5])
    vmlal.s16       q8, d28, d5     

    vmlal.s16       q7, d28, d6         @+(src_ptr[3] * coeff[6])
    vmlal.s16       q8, d29, d6

    vmlal.s16       q7, d29, d7         @-(src_ptr[4] * coeff[7])
    vmlal.s16       q8, d30, d7    

    vmlal.s16       q7, d25, d3         @+(src_ptr[0] * coeff[3])
    vmlal.s16       q8, d26, d3

    vmlal.s16       q7, d24, d2         @-(src_ptr[-1] * coeff[2])
    vmlal.s16       q8, d25, d2

    vmlal.s16       q7, d23, d1         @+(src_ptr[-2] * coeff[1])
    vmlal.s16       q8, d24, d1

    vmlal.s16       q7, d22, d0         @-(src_ptr[-3] * coeff[0]), sum of all (src_data*coeff)
    vmlal.s16       q8, d23, d0 
 .endm
  
 .macro YBitShift_SecondPass2nd_4x2
    
    vadd.s32        q7, q4
    vadd.s32        q8, q4
    
    vqrshrun.s32    d18, q7, #12        @shift/round/saturate to u8, at this time only q9 is available.
    vqrshrun.s32    d19, q8, #12
 .endm
  
 .macro YStoreResult_2nd_4x2
    
    vqmovn.u16       d19, q9
    
    vst1.32         {d19[0]}, [r2], r3
    vst1.32         {d19[1]}, [r2]
 .endm
    
 .macro YInterpolate_SecondPassOnly_4x4
    
    vmull.u8        q8,  d28, d4        @+(src_ptr[1] * coeff[4])
    vmull.u8        q9,  d29, d4

    vmlsl.u8        q8,  d22, d5        @-(src_ptr[2] * coeff[5])
    vmlsl.u8        q9,  d23, d5
    		    
    vmlal.u8        q8,  d29, d6        @+(src_ptr[3] * coeff[6])
    vmlal.u8        q9,  d30, d6 

    vmlsl.u8        q8,  d23, d7        @-(src_ptr[4] * coeff[7])
    vmlsl.u8        q9,  d24, d7

    vmlal.u8        q8,  d21, d3        @+(src_ptr[0] * coeff[3])
    vmlal.u8        q9,  d22, d3

    vmlsl.u8        q8,  d27, d2        @-(src_ptr[-1] * coeff[2])
    vmlsl.u8        q9,  d28, d2

    vmlal.u8        q8,  d20, d1        @+(src_ptr[-2] * coeff[1])
    vmlal.u8        q9,  d21, d1

    vmlsl.u8        q8,  d26, d0        @-(src_ptr[-3] * coeff[0]), sum of all (src_data*coeff)
    vmlsl.u8        q9,  d27, d0
 .endm
 
 .macro YBitShift_Only_4x4
    
    vqrshrun.s16    d27, q8, #6  @shift/round/saturate to u8        
    vqrshrun.s16    d28, q9, #6
 .endm
 
 .macro YStoreResult_Only_4x4
    
    add             r0, r2, r3   @dst+1*dstStride
    add             r1, r0, r3   @dst+2*dstStride
    add             r4, r1, r3   @dst+3*dstStride

    vst1.32         {d27[0]}, [r2]
    vst1.32         {d27[1]}, [r0]
    vst1.32         {d28[0]}, [r1]
    vst1.32         {d28[1]}, [r4]
 .endm

@ r0    unsigned char  *src,
@ r1    int  srcStride,
@ r2    unsigned char  dst,
@ r3    int  dstStride,
@ stack(r4) int xFrac,
@ stack(r5) int yFrac
MC_InterLuma_4x4_neon: @ PROC
    push            {r4-r5, lr}
    
    adrl            r12, LumaCoeff
    ldr             r4, [sp, #12]       @load parameters from stack
    ldr             r5, [sp, #16]       @load parameters from stack
    
    cmp             r4, #0              @skip first_pass filter if xoffset=0
    beq             Luma_secondpass_filter4x4_only

    cmp             r5, #0              @skip second_pass filter if yoffset=0
    beq             Luma_firstpass_filter4x4_only  
    
    add             r4, r12, r4, lsl #4 @calculate filter location
    vld1.16         {q4}, [r4]          @load coeff_x
    vabs.s16        q4, q4
    
    YDuplicateCoeff_8

    Back_3Line_3Column        
    
    YLoadSource_FirstPass_11x4       
    
    pld             [r0]
    pld             [r0, r1]    
                  
    YInterpolate_FirstPass_4x4

    YBitShift_FirstPass_4x4
    
@First Pass on following 4-line data.
    YLoadSource_FirstPass_11x4
         
    pld             [r0]
    pld             [r0, r1]
          
    YInterpolate_FirstPassNext_4x4
                    
    YBitShift_FirstPassNext_4x4
	     
@First Pass on the last 3-line data.
    YLoadSource_FirstPass_11x3

@recomand that the constructed src_ptr should use q14 q15                  
    YInterpolate_FirstPassEnd_4x3
   
    YBitShift_FirstPassEnd_4x3   

@Second pass, use q10~q15 16-bit first pass result to perform 
@an execution of vertical 8-tap interpolation filter.  
    add             r5, r12, r5, lsl #4 
    vld1.16         {q4}, [r5]          @load coeff_y   

    YDuplicateCoeff_16  

@ first & second row for second pass.         
    YInterpolate_SecondPass1st_4x2
    
    YBitShift_SecondPass1st_4x2
         
    YStoreResult_1st_4x2 
  
@ third & last row for second pass.  
    YInterpolate_SecondPass2nd_4x2
         
    YBitShift_SecondPass2nd_4x2
         
    YStoreResult_2nd_4x2
    
    pop             {r4-r5, pc}
    
@------------------------------------------------------- 
Luma_firstpass_filter4x4_only:

    add             r4, r12, r4, lsl #4 @calculate filter location
    vld1.16         {q4}, [r4]          @load coeff
    vabs.s16        q4, q4
    
    YDuplicateCoeff_8

    Back_3Column

    YLoadSource_FirstPass_11x4

    YInterpolate_FirstPass_4x4          

    YBitShift_Only_4x4
    
    YStoreResult_Only_4x4
    
    pop             {r4-r5, pc}
    
@-------------------------------------------------------
Luma_secondpass_filter4x4_only:
@register layout diagram (8-tap interpolation filter): (low bit <---> high bit)
@    ________________________________________________________________
@   |x(-3,0) x(-3,1) x(-3,2) x(-3,3)||x(-2,0) x(-2,1) x(-2,2) x(-2,3)|
@   |_______________________________||_______________________________|----> {d26}
@   |x(-2,0) x(-2,1) x(-2,2) x(-2,3)||x(-1,0) x(-1,1) x(-1,2) x(-1,3)|----> {d20}
@   |_______________________________||_______________________________|
@   |x(-1,0) x(-1,1) x(-1,2) x(-1,3)||x(0,0)  x(0,1)  x(0,2)  x(0,3) |
@   |_______________________________||_______________________________|----> {d27}
@   |x(0,0)  x(0,1)  x(0,2)  x(0,3) ||x(1,0)  x(1,1)  x(1,2)  x(1,3) |----> {d21}
@   |_______________________________||_______________________________|
@   |x(1,0)  x(1,1)  x(1,2)  x(1,3) ||x(2,0)  x(2,1)  x(2,2)  x(2,3) |
@   |_______________________________||_______________________________|----> {d28}
@   |x(2,0)  x(2,1)  x(2,2)  x(2,3) ||x(3,0)  x(3,1)  x(3,2)  x(3,3) |----> {d22}
@   |_______________________________||_______________________________|
@   |x(3,0)  x(3,1)  x(3,2)  x(3,3) ||x(4,0)  x(4,1)  x(4,2)  x(4,3) |
@   |_______________________________||_______________________________|----> {d29}
@   |x(4,0)  x(4,1)  x(4,2)  x(4,3) ||x(5,0)  x(5,1)  x(5,2)  x(5,3) |----> {d23}
@   |_______________________________||_______________________________|
@   |x(5,0)  x(5,1)  x(5,2)  x(5,3) ||x(6,0)  x(6,1)  x(6,2)  x(6,3) |
@   |_______________________________||_______________________________|----> {d30}
@   |x(6,0)  x(6,1)  x(6,2)  x(6,3) ||x(7,0)  x(7,1)  x(7,2)  x(7,3) |----> {d24}
@   |_______________________________||_______________________________|
@   |x(7,0)  x(7,1)  x(7,2)  x(7,3) ||         dummy register        |
@   |_______________________________||_______________________________|----> {d31}
@
    add             r5, r12, r5, lsl #4 @calculate filter location
    vld1.16         {q4}, [r5]          @load coeff
    vabs.s16        q4, q4      
         
    YDuplicateCoeff_8
         
    Back_3Line
         
    YLoadSource_SecondPassOnly_4x11

    YInterpolate_SecondPassOnly_4x4 		 		 		 

    YBitShift_Only_4x4
    
    YStoreResult_Only_4x4

    pop             {r4-r5, pc}
    
@ ENDP

@-------------------------------------------------------
 .macro YBitShift_SecondPass1stBi_4x2

    vqshrn.s32     d20, q5, #6
    vqshrn.s32     d21, q6, #6
 .endm
 
 .macro YStoreResult_SecondPass1stBi_4x2 
    
    lsl             r3, #1
    add             r0, r2, r3   @dst+1*dstStride
 
    vst1.16        {d20}, [r2]
    vst1.16        {d21}, [r0]
 .endm
  
 .macro YBitShift_SecondPass2ndBi_4x2
    
    vqshrn.s32     d22, q7, #6
    vqshrn.s32     d23, q8, #6
 .endm
  
 .macro YStoreResult_SecondPass2ndBi_4x2
    
    add             r1, r0, r3   @dst+2*dstStride
    add             r4, r1, r3   @dst+3*dstStride
 
    vst1.16        {d22}, [r1]
    vst1.16        {d23}, [r4]
 .endm

 .macro YBitShift_OnlyBi_4x4    
    
    vmov.s16        q10, #0x2000        @construct offset = -8192(-0x2000H)
 
    vsub.s16        q8, q8, q10
    vsub.s16        q9, q9, q10
 .endm
 
 .macro YStoreResult_OnlyBi_4x4
    
    lsl             r3, #1

    vst1.16         d16, [r2], r3
    vst1.16         d17, [r2], r3
    vst1.16         d18, [r2], r3
    vst1.16         d19, [r2], r3
 .endm

@-------------------------------------------------------    

@ r0    unsigned char  *src,
@ r1    int  srcStride,
@ r2    short  *dst,
@ r3    int  dstStride,
@ stack(r4) int xFrac,
@ stack(r5) int yFrac
MC_InterLumaBi_4x4_neon: @ PROC
    push            {r4-r5, lr}
    
    adrl            r12, LumaCoeff
    ldr             r4, [sp, #12]       @load parameters from stack
    ldr             r5, [sp, #16]       @load parameters from stack
    
    cmp             r4, #0              @skip first_pass filter if xoffset=0
    beq             Luma_bi_secondpass_filter4x4_only

    cmp             r5, #0              @skip second_pass filter if yoffset=0
    beq             Luma_bi_firstpass_filter4x4_only  
    
    add             r4, r12, r4, lsl #4 @calculate filter location
    vld1.16         {q4}, [r4]          @load coeff_x
    vabs.s16        q4, q4
    
    YDuplicateCoeff_8

    Back_3Line_3Column        
        
    YLoadSource_FirstPass_11x4       
         
    pld             [r0]
    pld             [r0, r1]    
                  
    YInterpolate_FirstPass_4x4

    YBitShift_FirstPass_4x4
    
    YLoadSource_FirstPass_11x4
         
    pld             [r0]
    pld             [r0, r1]
          
    YInterpolate_FirstPassNext_4x4
                    
    YBitShift_FirstPassNext_4x4
	
    YLoadSource_FirstPass_11x3
              
    YInterpolate_FirstPassEnd_4x3
   
    YBitShift_FirstPassEnd_4x3
    
    add             r5, r12, r5, lsl #4 
    vld1.16         {q4}, [r5]          @load coeff_y   

    YDuplicateCoeff_16  
        
    YInterpolate_SecondPass1st_4x2
    
    YBitShift_SecondPass1stBi_4x2
         
    YStoreResult_SecondPass1stBi_4x2
 
    YInterpolate_SecondPass2nd_4x2
         
    YBitShift_SecondPass2ndBi_4x2
         
    YStoreResult_SecondPass2ndBi_4x2
    
    pop             {r4-r5, pc}  
@----------------------------------------------------------------
Luma_bi_firstpass_filter4x4_only:

    add             r4, r12, r4, lsl #4 @calculate filter location
    vld1.16         {q4}, [r4]         @load coeff
    vabs.s16        q4, q4
    
    YDuplicateCoeff_8

    Back_3Column

    YLoadSource_FirstPass_11x4

    YInterpolate_FirstPass_4x4 
    
    YBitShift_OnlyBi_4x4
    
    YStoreResult_OnlyBi_4x4
    
    pop             {r4-r5, pc}
@----------------------------------------------------------------
Luma_bi_secondpass_filter4x4_only:

    add             r5, r12, r5, lsl #4 @calculate filter location
    vld1.16         {q4}, [r5]         @load coeff
    vabs.s16        q4, q4
    
    YDuplicateCoeff_8
         
    Back_3Line
         
    YLoadSource_SecondPassOnly_4x11

    YInterpolate_SecondPassOnly_4x4 
    
    YBitShift_OnlyBi_4x4
    
    YStoreResult_OnlyBi_4x4
    
    pop             {r4-r5, pc}

@ ENDP     
    

@------------------------------------------------------- 
 .macro YLoadSource_FirstPass_15x2
    
    vld1.u8         {q4}, [r0], r1      @load following 2-lines src data
    vld1.u8         {q5}, [r0], r1
    
    pld             [r0]
    pld             [r0, r1] 
 .endm

 .macro YLoadSource_FirstPass_15x4
 
    vld1.u8         {q4}, [r0], r1      @load 4-line src data
    vld1.u8         {q5}, [r0], r1
    vld1.u8         {q6}, [r0], r1
    vld1.u8         {q7}, [r0], r1
    
    pld             [r0]
    pld             [r0, r1]
 .endm
 
 .macro YInterpolate_FirstPass_8x1
    
    vext.8          d10, d8, d9, #4     @+(src_ptr[1]*coeff[4])
    vmull.u8        q15, d10, d4
         
    vext.8          d10, d8, d9, #5     @-(src_ptr[2]*coeff[5])
    vmlsl.u8        q15, d10, d5
         
    vext.8          d10, d8, d9, #6     @+(src_ptr[3]*coeff[6])
    vmlal.u8        q15, d10, d6
              
    vext.8          d10, d8, d9, #7     @-(src_ptr[4]*coeff[7])
    vmlsl.u8        q15, d10, d7
         
    vext.8          d10, d8, d9, #3
    vmlal.u8        q15, d10, d3
         
    vext.8          d10, d8, d9, #2
    vmlsl.u8        q15, d10, d2
         
    vext.8          d10, d8, d9, #1
    vmlal.u8        q15, d10, d1
         
    vmlsl.u8        q15, d8, d0
 .endm
 
 .macro YBitShift_FirstPass_8x1
    
    vmov.s16        q6, #0x2000
    vsub.s16        q15, q15, q6
 .endm
 
 .macro YInterpolate_FirstPass1st_8x2
    
    vext.8          d28, d8, d9, #4     @construct src_ptr[1], +(src_ptr[1]*coeff[4])
    vext.8          d29, d10, d11, #4
    vmull.u8        q12, d28, d4        
    vmull.u8        q13, d29, d4
         
    vext.8          d28, d8, d9, #5     @construct src_ptr[2], -(src_ptr[2]*coeff[5])
    vext.8          d29, d10, d11, #5
    vmlsl.u8        q12, d28, d5      
    vmlsl.u8        q13, d29, d5 
         
    vext.8          d28, d8, d9, #6     @construct src_ptr[3], +(src_ptr[3]*coeff[6])
    vext.8          d29, d10, d11, #6
    vmlal.u8        q12, d28, d6     
    vmlal.u8        q13, d29, d6    
         
    vext.8          d28, d8, d9, #7     @construct src_ptr[4], -(src_ptr[4]*coeff[7])
    vext.8          d29, d10, d11, #7  
    vmlsl.u8        q12, d28, d7      
    vmlsl.u8        q13, d29, d7
         
    vext.8          d28, d8, d9, #3     @construct src_ptr[0], +(src_ptr[0]*coeff[3])
    vext.8          d29, d10, d11, #3
    vmlal.u8        q12, d28, d3      
    vmlal.u8        q13, d29, d3  
         
    vext.8          d28, d8, d9, #2     @construct src_ptr[-1], -(src_ptr[-1]*coeff[2])
    vext.8          d29, d10, d11, #2
    vmlsl.u8        q12, d28, d2      
    vmlsl.u8        q13, d29, d2              
         
    vext.8          d28, d8, d9, #1     @construct src_ptr[-2], +(src_ptr[-2]*coeff[1])
    vext.8          d29, d10, d11, #1
    vmlal.u8        q12, d28, d1       
    vmlal.u8        q13, d29, d1
         
    vmlsl.u8        q12, d8, d0         @-(src_ptr[-3]*coeff[0])
    vmlsl.u8        q13, d10, d0
 .endm
 
 .macro YBitShift_FirstPass1st_8x2
    
    vmov.s16        q14, #0x2000
    vsub.s16        q12, q12, q14
    vsub.s16        q13, q13, q14
 .endm
 
 .macro YInterpolate_FirstPass2nd_8x2
 
    vext.8          d12, d8, d9, #4     @construct src_ptr[1], +(src_ptr[1]*coeff[4])
    vext.8          d13, d10, d11, #4
    vmull.u8        q14, d12, d4        
    vmull.u8        q15, d13, d4 
         
    vext.8          d12, d8, d9, #5     @construct src_ptr[2], -(src_ptr[2]*coeff[5])
    vext.8          d13, d10, d11, #5
    vmlsl.u8        q14, d12, d5      
    vmlsl.u8        q15, d13, d5 
         
    vext.8          d12, d8, d9, #6     @construct src_ptr[3], +(src_ptr[3]*coeff[6])
    vext.8          d13, d10, d11, #6
    vmlal.u8        q14, d12, d6     
    vmlal.u8        q15, d13, d6    
         
    vext.8          d12, d8, d9, #7     @construct src_ptr[4], -(src_ptr[4]*coeff[7])
    vext.8          d13, d10, d11, #7  
    vmlsl.u8        q14, d12, d7      
    vmlsl.u8        q15, d13, d7
         
    vext.8          d12, d8, d9, #3     @construct src_ptr[0], +(src_ptr[0]*coeff[3])
    vext.8          d13, d10, d11, #3
    vmlal.u8        q14, d12, d3      
    vmlal.u8        q15, d13, d3  
         
    vext.8          d12, d8, d9, #2     @construct src_ptr[-1], -(src_ptr[-1]*coeff[2])
    vext.8          d13, d10, d11, #2
    vmlsl.u8        q14, d12, d2      
    vmlsl.u8        q15, d13, d2              
         
    vext.8          d12, d8, d9, #1     @construct src_ptr[-2], +(src_ptr[-2]*coeff[1])
    vext.8          d13, d10, d11, #1
    vmlal.u8        q14, d12, d1       
    vmlal.u8        q15, d13, d1
         
    vmlsl.u8        q14, d8, d0         @-(src_ptr[-3]*coeff[0])
    vmlsl.u8        q15, d10, d0
 .endm
 
 .macro YBitShift_FirstPass2nd_8x2
    
    vmov.s16        q6, #0x2000
    vsub.s16        q14, q14, q6
    vsub.s16        q15, q15, q6
 .endm
 
 .macro YInterpolate_FirstPassOnly_8x4
    
    vext.8          d24, d8, d9, #4      @construct src_ptr[1]
    vext.8          d25, d10, d11, #4
    vext.8          d26, d12, d13, #4
    vext.8          d27, d14, d15, #4

    vmull.u8        q8, d24, d4          @+(src_ptr[1]*coeff[4])
    vmull.u8        q9, d25, d4
    vmull.u8        q10, d26, d4
    vmull.u8        q11, d27, d4
         
    vext.8          d24, d8, d9, #5      @construct src_ptr[2] 
    vext.8          d25, d10, d11, #5
    vext.8          d26, d12, d13, #5
    vext.8          d27, d14, d15, #5   

    vmlsl.u8        q8, d24, d5          @-(src_ptr[2]*coeff[5])
    vmlsl.u8        q9, d25, d5
    vmlsl.u8        q10, d26, d5
    vmlsl.u8        q11, d27, d5   

    vext.8          d24, d8, d9, #6      @construct src_ptr[3]
    vext.8          d25, d10, d11, #6
    vext.8          d26, d12, d13, #6
    vext.8          d27, d14, d15, #6   

    vmlal.u8        q8, d24, d6          @+(src_ptr[3]*coeff[6])
    vmlal.u8        q9, d25, d6
    vmlal.u8        q10, d26, d6
    vmlal.u8        q11, d27, d6     
         
    vext.8          d24, d8, d9, #7      @construct src_ptr[4]
    vext.8          d25, d10, d11, #7
    vext.8          d26, d12, d13, #7
    vext.8          d27, d14, d15, #7   

    vmlsl.u8        q8, d24, d7          @-(src_ptr[4]*coeff[7])
    vmlsl.u8        q9, d25, d7
    vmlsl.u8        q10, d26, d7
    vmlsl.u8        q11, d27, d7  
    
    vext.8          d24, d8, d9, #3      @construct src_ptr[0]
    vext.8          d25, d10, d11, #3
    vext.8          d26, d12, d13, #3
    vext.8          d27, d14, d15, #3

    vmlal.u8        q8, d24, d3          @+(src_ptr[0]*coeff[3])
    vmlal.u8        q9, d25, d3
    vmlal.u8        q10, d26, d3
    vmlal.u8        q11, d27, d3    
           
    vext.8          d24, d8, d9, #2      @construct src_ptr[-1]
    vext.8          d25, d10, d11, #2
    vext.8          d26, d12, d13, #2
    vext.8          d27, d14, d15, #2

    vmlsl.u8        q8, d24, d2          @-(src_ptr[-1]*coeff[2])
    vmlsl.u8        q9, d25, d2
    vmlsl.u8        q10, d26, d2
    vmlsl.u8        q11, d27, d2               

    vext.8          d24, d8, d9, #1      @construct src_ptr[-2]
    vext.8          d25, d10, d11, #1
    vext.8          d26, d12, d13, #1
    vext.8          d27, d14, d15, #1

    vmlal.u8        q8, d24, d1          @+(src_ptr[-2]*coeff[1])
    vmlal.u8        q9, d25, d1
    vmlal.u8        q10, d26, d1
    vmlal.u8        q11, d27, d1 
         
    vmlsl.u8        q8, d8, d0           @-(src_ptr[-3]*coeff[0])
    vmlsl.u8        q9, d10, d0
    vmlsl.u8        q10, d12, d0
    vmlsl.u8        q11, d14, d0
 .endm
  
 .macro YBitShift_FirstPass_8x4
 
    vmov.s16        q12, #0x2000
         
    vsub.s16        q8, q8, q12
    vsub.s16        q9, q9, q12
    vsub.s16        q10, q10, q12
    vsub.s16        q11, q11, q12
 .endm
     
 .macro YBitShift_FirstPassOnly_8x4
    
    vqrshrun.s16    d24, q8, #6           @shift/round/saturate to u8
    vqrshrun.s16    d25, q9, #6
    vqrshrun.s16    d26, q10, #6
    vqrshrun.s16    d27, q11, #6
 .endm
  
 .macro YStoreResult_FirstPassOnly_8x4
    
    vst1.u8         {d24}, [r2], r3        @store result
    vst1.u8         {d25}, [r2], r3
    vst1.u8         {d26}, [r2], r3
    vst1.u8         {d27}, [r2], r3
 .endm
  
 .macro YLoadSource_SecondPassOnly_8x15
    
    vld1.u8         {d16}, [r0], r1
    vld1.u8         {d17}, [r0], r1
    vld1.u8         {d18}, [r0], r1
    vld1.u8         {d19}, [r0], r1
    vld1.u8         {d20}, [r0], r1
    vld1.u8         {d21}, [r0], r1
    vld1.u8         {d22}, [r0], r1
    vld1.u8         {d23}, [r0], r1
    vld1.u8         {d24}, [r0], r1
    vld1.u8         {d25}, [r0], r1
    vld1.u8         {d26}, [r0], r1
    vld1.u8         {d27}, [r0], r1
    vld1.u8         {d28}, [r0], r1
    vld1.u8         {d29}, [r0], r1
    vld1.u8         {d30}, [r0], r1
 .endm
 
 .macro YInterpolate_SecondPassOnly_8x4
    
    vmull.u8        q4, d20, d4    @+(src_ptr[1]*coeff[4])
    vmull.u8        q5, d21, d4
    vmull.u8        q6, d22, d4
    vmull.u8        q7, d23, d4
         
    vmlsl.u8        q4, d21, d5    @-(src_ptr[2]*coeff[5])
    vmlsl.u8        q5, d22, d5
    vmlsl.u8        q6, d23, d5
    vmlsl.u8        q7, d24, d5
         
    vmlal.u8        q4, d22, d6    @+(src_ptr[3]*coeff[6])
    vmlal.u8        q5, d23, d6
    vmlal.u8        q6, d24, d6
    vmlal.u8        q7, d25, d6
         
    vmlsl.u8        q4, d23, d7    @-(src_ptr[4]*coeff[7])
    vmlsl.u8        q5, d24, d7
    vmlsl.u8        q6, d25, d7
    vmlsl.u8        q7, d26, d7
         
    vmlal.u8        q4, d19, d3    @+(src_ptr[0]*coeff[3])
    vmlal.u8        q5, d20, d3
    vmlal.u8        q6, d21, d3
    vmlal.u8        q7, d22, d3
         
    vmlsl.u8        q4, d18, d2    @-(src_ptr[-1]*coeff[2])
    vmlsl.u8        q5, d19, d2
    vmlsl.u8        q6, d20, d2
    vmlsl.u8        q7, d21, d2
         
    vmlal.u8        q4, d17, d1    @+(src_ptr[-2]*coeff[1])
    vmlal.u8        q5, d18, d1
    vmlal.u8        q6, d19, d1
    vmlal.u8        q7, d20, d1
         
    vmlsl.u8        q4, d16, d0    @-(src_ptr[-3]*coeff[0])
    vmlsl.u8        q5, d17, d0
    vmlsl.u8        q6, d18, d0
    vmlsl.u8        q7, d19, d0
 .endm
  
 .macro YBitShift_SecondPassOnly_8x4
 
    vqrshrun.s16    d8, q4, #6     @shift/round/saturate to u8
    vqrshrun.s16    d10, q5, #6
    vqrshrun.s16    d12, q6, #6
    vqrshrun.s16    d14, q7, #6
 .endm
  
 .macro YStoreResult_SecondPassOnly_8x4
    
    vst1.u8         {d8}, [r2], r3    @store result
    vst1.u8         {d10}, [r2], r3
    vst1.u8         {d12}, [r2], r3
    vst1.u8         {d14}, [r2], r3
 .endm
 
 .macro YInterpolate_SecondPass_8x1
 
    vmull.s16       q4, d16, d0
    vmull.s16       q5, d17, d0
         
    vmlal.s16       q4, d18, d1
    vmlal.s16       q5, d19, d1
         
    vmlal.s16       q4, d20, d2     
    vmlal.s16       q5, d21, d2  
         
    vmlal.s16       q4, d22, d3     
    vmlal.s16       q5, d23, d3 
         
    vmlal.s16       q4, d24, d4     
    vmlal.s16       q5, d25, d4
         
    vmlal.s16       q4, d26, d5     
    vmlal.s16       q5, d27, d5
         
    vmlal.s16       q4, d28, d6     
    vmlal.s16       q5, d29, d6
         
    vmlal.s16       q4, d30, d7     
    vmlal.s16       q5, d31, d7   
 .endm
 
 .macro YBitShift_SecondPass_8x1
    
    vmov.u32        q6,  #0x80000
    vadd.s32        q4, q6
    vadd.s32        q5, q6
       
    vqrshrun.s32    d12, q4, #12     @shift/round/saturate to u8
    vqrshrun.s32    d13, q5, #12
 .endm
  
 .macro YStoreResult_SecondPass_8x1
    
    vqmovn.u16      d14, q6
    vst1.u8         {d14}, [r2], r3    @store result  
 .endm
    
@ r0    unsigned char  *src,
@ r1    int  srcStride,
@ r2    unsigned char  dst,
@ r3    int  dstStride,
@ stack(r4) int xFrac,
@ stack(r5) int yFrac
MC_InterLuma_8x8_neon: @ PROC
    push            {r4-r5, lr}

    adrl            r12, LumaCoeff
    ldr             r4, [sp, #12]       @load parameters from stack
    ldr             r5, [sp, #16]       @load parameters from stack

    cmp             r4, #0              @skip first_pass filter if xoffset=0
    beq             Luma_secondpass_filter8x8_only

    cmp             r5, #0              @skip second_pass filter if yoffset=0
    beq             Luma_firstpass_filter8x8_only  
    
    add             r4, r12, r4, lsl #4 @calculate filter_x location
    add             r5, r12, r5, lsl #4 @calculate filter_y location
    
    vld1.16         {q4}, [r4]          @load coeff_x
    vabs.s16        q4, q4
         
    YDuplicateCoeff_8
         
    Back_3Line_3Column
         
    YLoadSource_FirstPass_15x4  
                
    YInterpolate_FirstPassOnly_8x4
         
    YBitShift_FirstPass_8x4
         
@ load next 2 lines and do horizontal 16-bit prediction
    YLoadSource_FirstPass_15x2    
         
    YInterpolate_FirstPass1st_8x2
         
    YBitShift_FirstPass1st_8x2
         
@ load next 2 lines and do horizontal 16-bit prediction again
    YLoadSource_FirstPass_15x2       
    
    YInterpolate_FirstPass2nd_8x2
         
    YBitShift_FirstPass2nd_8x2

@ now q8~q15 is in use and should be handled properly.
    mov             lr, #8
luma_second_8x8_loop_neon:
    vpush           {q0, q1, q2, q3}
    
    vld1.16         {q4}, [r5]          @load coeff_y
    YDuplicateCoeff_16
         
    YInterpolate_SecondPass_8x1  
         
    YBitShift_SecondPass_8x1
    
    YStoreResult_SecondPass_8x1
    
    vpop            {q0, q1, q2, q3}  @restore coeff_x 
         
    cmp             lr, #1
    beq             exit
    
    vmov            q8, q9
    vmov            q9, q10
    vmov            q10, q11
    vmov            q11, q12
    vmov            q12, q13
    vmov            q13, q14
    vmov            q14, q15

@ Calulate next 1 line horizontal 16-bit temp interpolation value.        
    vld1.u8         {q4}, [r0], r1      @load following 1-lines src data
         
    YInterpolate_FirstPass_8x1
         
    YBitShift_FirstPass_8x1
         
    sub             lr, lr, #1
    b               luma_second_8x8_loop_neon

exit:         
    pop             {r4-r5, pc}
    
@-------------------------------------------------------    
Luma_firstpass_filter8x8_only:
    add             r4, r12, r4, lsl #4      @calculate filter location
    vld1.16         {q4}, [r4]       @load coeff_x
    vabs.s16        q4, q4
    
    Back_3Column
    
@prepare filter coeff   
    YDuplicateCoeff_8 
         
    mov             lr, #2            @loop counter    
@First pass: output_height lines x output_width columns (8x8)     
luma_first_only_8x8_loop_neon:
    
    YLoadSource_FirstPass_15x4
         
    YInterpolate_FirstPassOnly_8x4
    
    YBitShift_FirstPassOnly_8x4
         
    YStoreResult_FirstPassOnly_8x4
         
    subs            lr, lr, #1    
    bne             luma_first_only_8x8_loop_neon
     
    pop             {r4-r5, pc}    
    
@-------------------------------------------------------    
Luma_secondpass_filter8x8_only:
    add             r5, r12, r5, lsl #4
    vld1.16         {q4}, [r5]       @load coeff
    vabs.s16        q4, q4 
         
    Back_3Line
         
@prepare coeff
    YDuplicateCoeff_8
@load src data
    YLoadSource_SecondPassOnly_8x15

@Second pass: 8x8
    mov             lr, #2                  @loop counter  
luma_second_only_8x8_loop_neon:

    YInterpolate_SecondPassOnly_8x4
         
    YBitShift_SecondPassOnly_8x4
         
    YStoreResult_SecondPassOnly_8x4
         
    vmov            q8, q10
    vmov            q9, q11
    vmov            q10, q12
    vmov            q11, q13
    vmov            q12, q14
    vmov            q13, q15
         
    subs            lr, lr, #1   
    bne             luma_second_only_8x8_loop_neon

    pop             {r4-r5, pc}

@ ENDP    

@----------------------------------------------------------------
 .macro YBitShift_SecondPassBi_8x1
 
    @vmov.u32        q6,  #0x80000
    @vadd.s32        q4, q6
    @vadd.s32        q5, q6
       
    @vqrshrun.s32    d12, q4, #12     @shift/round/saturate to u8
    @vqrshrun.s32    d13, q5, #12
    
    vqshrn.s32       d12, q4, #6
    vqshrn.s32       d13, q5, #6
 .endm
 
 .macro YStoreResult_SecondPassBi_8x1
    
    vst1.16          {q6}, [r2], r3
 .endm
 
 .macro YStoreResult_FirstPassOnlyBi_8x4
    
    vst1.16         {d16, d17}, [r2], r3
    vst1.16         {d18, d19}, [r2], r3
    vst1.16         {d20, d21}, [r2], r3
    vst1.16         {d22, d23}, [r2], r3
 .endm
 
 .macro YBitShift_SecondPassOnlyBi_8x4
    
    vmov.s16        q8, #0x2000        @construct offset = -8192(-0x2000H)
    
    vsub.s16        q4, q4, q8
    vsub.s16        q5, q5, q8
    vsub.s16        q6, q6, q8
    vsub.s16        q7, q7, q8
 .endm
 
 .macro YStoreResult_SecondPassOnlyBi_8x4
    
    vst1.16         {d8, d9}, [r2], r3
    vst1.16         {d10, d11}, [r2], r3
    vst1.16         {d12, d13}, [r2], r3
    vst1.16         {d14, d15}, [r2], r3
 .endm
 
@ r0    unsigned char  *src,
@ r1    int  srcStride,
@ r2    short  *dst,
@ r3    int  dstStride,
@ stack(r4) int xFrac,
@ stack(r5) int yFrac
MC_InterLumaBi_8x8_neon: @ PROC
    push            {r4-r5, lr}

    adrl            r12, LumaCoeff
    ldr             r4, [sp, #12]       @load parameters from stack
    ldr             r5, [sp, #16]       @load parameters from stack

    lsl             r3, #1
    
    cmp             r4, #0              @skip first_pass filter if xoffset=0
    beq             Luma_bi_secondpass_filter8x8_only

    cmp             r5, #0              @skip second_pass filter if yoffset=0
    beq             Luma_bi_firstpass_filter8x8_only
    
    add             r4, r12, r4, lsl #4 @calculate filter_x location
    add             r5, r12, r5, lsl #4 @calculate filter_y location
    
    vld1.16         {q4}, [r4]          @load coeff_x
    vabs.s16        q4, q4
         
    YDuplicateCoeff_8
         
    Back_3Line_3Column
         
    YLoadSource_FirstPass_15x4  
                
    YInterpolate_FirstPassOnly_8x4
         
    YBitShift_FirstPass_8x4
         
@ load next 2 lines and do horizontal 16-bit prediction
    YLoadSource_FirstPass_15x2    
         
    YInterpolate_FirstPass1st_8x2
         
    YBitShift_FirstPass1st_8x2
         
@ load next 2 lines and do horizontal 16-bit prediction again
    YLoadSource_FirstPass_15x2       
    
    YInterpolate_FirstPass2nd_8x2
         
    YBitShift_FirstPass2nd_8x2

@ now q8~q15 is in use and should be handled properly.
    mov             lr, #8
luma_bi_second_8x8_loop_neon:
    vpush           {q0, q1, q2, q3}
    
    vld1.16         {q4}, [r5]          @load coeff_y
    YDuplicateCoeff_16
         
    YInterpolate_SecondPass_8x1  
         
    YBitShift_SecondPassBi_8x1
    
    YStoreResult_SecondPassBi_8x1
    
    vpop            {q0, q1, q2, q3}  @restore coeff_x 
         
    cmp             lr, #1
    beq             bi_exit
    
    vmov            q8, q9
    vmov            q9, q10
    vmov            q10, q11
    vmov            q11, q12
    vmov            q12, q13
    vmov            q13, q14
    vmov            q14, q15

@ Calulate next 1 line horizontal 16-bit temp interpolation value.        
    vld1.u8         {q4}, [r0], r1      @load following 1-lines src data
         
    YInterpolate_FirstPass_8x1
         
    YBitShift_FirstPass_8x1
         
    sub             lr, lr, #1
    b               luma_bi_second_8x8_loop_neon

bi_exit:
    pop             {r4-r5, pc}
    
@----------------------------------------------------------------    
Luma_bi_firstpass_filter8x8_only:
    add             r4, r12, r4, lsl #4 @calculate filter location
    vld1.16         {q4}, [r4]          @load coeff_x
    vabs.s16        q4, q4
    
    Back_3Column
      
    YDuplicateCoeff_8                   @prepare filter coeff 
         
    mov             lr, #2              @loop counter         
luma_bi_first_only_8x8_loop_neon:
    
    YLoadSource_FirstPass_15x4
         
    YInterpolate_FirstPassOnly_8x4
    
    YBitShift_FirstPass_8x4
         
    YStoreResult_FirstPassOnlyBi_8x4
         
    subs            lr, lr, #1    
    bne             luma_bi_first_only_8x8_loop_neon
     
    pop             {r4-r5, pc}   
@---------------------------------------------------------------- 
Luma_bi_secondpass_filter8x8_only:
    add             r5, r12, r5, lsl #4
    vld1.16         {q4}, [r5]       @load coeff
    vabs.s16        q4, q4 
         
    Back_3Line
    
    YDuplicateCoeff_8

    YLoadSource_SecondPassOnly_8x15

    mov             lr, #2                  @loop counter
luma_bi_second_only_8x8_loop_neon:

    YInterpolate_SecondPassOnly_8x4
         
    YBitShift_SecondPassOnlyBi_8x4
         
    YStoreResult_SecondPassOnlyBi_8x4
         
    vmov            q8, q10
    vmov            q9, q11
    vmov            q10, q12
    vmov            q11, q13
    vmov            q12, q14
    vmov            q13, q15
         
    subs            lr, lr, #1   
    bne             luma_bi_second_only_8x8_loop_neon

    pop             {r4-r5, pc}

@ ENDP
@----------------------------------------------------------------
	.section	.note.GNU-stack,"",%progbits
.endif    @.if MC_ASM_ENABLED==1