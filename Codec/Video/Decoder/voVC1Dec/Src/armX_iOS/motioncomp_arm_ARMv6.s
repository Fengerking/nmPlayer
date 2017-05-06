    #include "../../Src/c/voVC1DID.h"
    .include "wmvdec_member_arm.h"
    .include "xplatform_arm_asm.h" 
 
    @AREA |.text|, CODE, READONLY
     .text
     .align 4 

    .if WMV_OPT_MOTIONCOMP_ARM == 1

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    .globl  _ARMV6_g_MotionCompZeroMotion_WMV 
  .if PRO_VER != 0
	.globl _ARMV6_IntensityCompensation_asm
  .endif


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV MotionCompZeroMotion_WMV (
@    U8_WMV* ppxliCurrQYMB, 
@    U8_WMV* ppxliCurrQUMB, 
@    U8_WMV* ppxliCurrQVMB,
@    const U8_WMV* ppxliRefYMB, 
@    const U8_WMV* ppxliRefUMB, 
@    const U8_WMV* ppxliRefVMB,
@    I32_WMV iWidthY,
@    I32_WMV iWidthUV
@)

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@   Registers ARMV6_g_MotionCompZeroMotion_WMV
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  
pCurrQYMB				.req  r0
pRefYMB					.req  r1
iWidthY                 .req  r2
iWidthUV                .req  r3
uData0                  .req  r4
uData1                  .req  r5
uData2                  .req  r6
uData3                  .req  r7
uData4                  .req  r8
uData5                  .req  r9
uData6                  .req  r10
uData7                  .req  r11
 
pCurrQUMB				.req  r0
pRefUMB					.req  r1
  
pCurrQVMB				.req  r0
pRefVMB					.req  r1

@----Stack Usage----

@ppxliRefUMB
@ppxliRefVMB
@iWidthY
@iWidthUV
@register ...
@ppxliCurrQUMB 
@ppxliCurrQVMB

.set OFFSET_register		  , 36
.set OFFSET_ppxliRefUMB		  , OFFSET_register + 0
.set OFFSET_ppxliRefVMB		  , OFFSET_register + 4	
.set OFFSET_iWidthY			  , OFFSET_register + 8	
.set OFFSET_iWidthUV		  , OFFSET_register + 12
.set OFFSET_ppxliCurrQUMB	  , -4	
.set OFFSET_ppxliCurrQVMB	  , -8	
	     
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    WMV_LEAF_ENTRY ARMV6_g_MotionCompZeroMotion_WMV
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@      
    stmfd   sp!, {r4 - r11, r14} 
    FRAME_PROFILE_COUNT

	str		r1, [sp, #OFFSET_ppxliCurrQUMB]
	str		r2, [sp, #OFFSET_ppxliCurrQVMB]
	mov		r1, r3
	ldr		r2, [sp, #OFFSET_iWidthY]
	ldr		r3, [sp, #OFFSET_iWidthUV]

@ Y    
@0,1 row
	@pld     [pRefYMB]       
	@pld     [pRefYMB, iWidthY]       
	@pld     [pRefYMB, iWidthY,lsl #1]
    ldrd    uData2, [pRefYMB, #8]
    ldrd    uData0, [pRefYMB],iWidthY
	@pld     [pRefYMB, iWidthY,lsl #1]
    ldrd    uData6, [pRefYMB, #8]
    ldrd    uData4, [pRefYMB],iWidthY
    
	@pld     [pRefYMB, iWidthY, lsl #1]
    strd    uData2, [pCurrQYMB, #8]
    strd    uData0, [pCurrQYMB],iWidthY
    
@2,3 row
    ldrd    uData2, [pRefYMB, #8]
    ldrd    uData0, [pRefYMB],iWidthY
	@pld     [pRefYMB, iWidthY, lsl #1]
    strd    uData6, [pCurrQYMB, #8]
    strd    uData4, [pCurrQYMB],iWidthY
    
    ldrd    uData6, [pRefYMB, #8]
    ldrd    uData4, [pRefYMB],iWidthY
	@pld     [pRefYMB, iWidthY, lsl #1]
    strd    uData2, [pCurrQYMB, #8]
    strd    uData0, [pCurrQYMB],iWidthY
    
@4,5 row
    ldrd    uData2, [pRefYMB, #8]
    ldrd    uData0, [pRefYMB],iWidthY
	@pld     [pRefYMB, iWidthY, lsl #1]
    strd    uData6, [pCurrQYMB, #8]
    strd    uData4, [pCurrQYMB],iWidthY
    
    ldrd    uData6, [pRefYMB, #8]
    ldrd    uData4, [pRefYMB],iWidthY
	@pld     [pRefYMB, iWidthY, lsl #1]
    strd    uData2, [pCurrQYMB, #8]
    strd    uData0, [pCurrQYMB],iWidthY
    
@6,7 row
    ldrd    uData2, [pRefYMB, #8]
    ldrd    uData0, [pRefYMB],iWidthY
	@pld     [pRefYMB, iWidthY, lsl #1]
    strd    uData6, [pCurrQYMB, #8]
    strd    uData4, [pCurrQYMB],iWidthY
    
    ldrd    uData6, [pRefYMB, #8]
    ldrd    uData4, [pRefYMB],iWidthY
	@pld     [pRefYMB, iWidthY, lsl #1]
    strd    uData2, [pCurrQYMB, #8]
    strd    uData0, [pCurrQYMB],iWidthY
    
@8,9 row
    ldrd    uData2, [pRefYMB, #8]
    ldrd    uData0, [pRefYMB],iWidthY
	@pld     [pRefYMB, iWidthY, lsl #1]
    strd    uData6, [pCurrQYMB, #8]
    strd    uData4, [pCurrQYMB],iWidthY
    
    ldrd    uData6, [pRefYMB, #8]
    ldrd    uData4, [pRefYMB],iWidthY
	@pld     [pRefYMB, iWidthY, lsl #1]
    strd    uData2, [pCurrQYMB, #8]
    strd    uData0, [pCurrQYMB],iWidthY
    
@10,11 row
    ldrd    uData2, [pRefYMB, #8]
    ldrd    uData0, [pRefYMB],iWidthY
	@pld     [pRefYMB, iWidthY, lsl #1]
    strd    uData6, [pCurrQYMB, #8]
    strd    uData4, [pCurrQYMB],iWidthY
    
    ldrd    uData6, [pRefYMB, #8]
    ldrd    uData4, [pRefYMB],iWidthY
	@pld     [pRefYMB, iWidthY, lsl #1]
    strd    uData2, [pCurrQYMB, #8]
    strd    uData0, [pCurrQYMB],iWidthY
    
@12,13 row
    ldrd    uData2, [pRefYMB, #8]
    ldrd    uData0, [pRefYMB],iWidthY
	@pld     [pRefYMB, iWidthY, lsl #1]
    strd    uData6, [pCurrQYMB, #8]
    strd    uData4, [pCurrQYMB],iWidthY
    
    ldrd    uData6, [pRefYMB, #8]
    ldrd    uData4, [pRefYMB],iWidthY
	@pld     [pRefYMB, iWidthY, lsl #1]
    strd    uData2, [pCurrQYMB, #8]
    strd    uData0, [pCurrQYMB],iWidthY
    
@14,15 row
    ldrd    uData2, [pRefYMB, #8]
    ldrd    uData0, [pRefYMB],iWidthY
    strd    uData6, [pCurrQYMB, #8]
    strd    uData4, [pCurrQYMB],iWidthY
    
    ldrd    uData6, [pRefYMB, #8]
    ldrd    uData4, [pRefYMB],iWidthY
    strd    uData2, [pCurrQYMB, #8]
    strd    uData0, [pCurrQYMB],iWidthY
    strd    uData6, [pCurrQYMB, #8]
    strd    uData4, [pCurrQYMB]

@U
	ldr		pRefUMB, [sp, #OFFSET_ppxliRefUMB]
	ldr		pCurrQUMB, [sp, #OFFSET_ppxliCurrQUMB]
@ row 0,1,2,3
	@pld     [pRefUMB]
	@pld     [pRefUMB, iWidthUV]
	@pld     [pRefUMB, iWidthUV, lsl #1]
    ldrd    uData0, [pRefUMB], iWidthUV
	@pld     [pRefUMB, iWidthUV, lsl #1]
    ldrd    uData2, [pRefUMB], iWidthUV
    ldrd    uData4, [pRefUMB], iWidthUV
	@pld     [pRefUMB, iWidthUV, lsl #1]
    ldrd    uData6, [pRefUMB], iWidthUV
	@pld     [pRefUMB, iWidthUV, lsl #1]
    strd    uData0, [pCurrQUMB], iWidthUV
    strd    uData2, [pCurrQUMB], iWidthUV
    strd    uData4, [pCurrQUMB], iWidthUV
    strd    uData6, [pCurrQUMB], iWidthUV
    
@ row 4,5,6,7
    ldrd    uData0, [pRefUMB], iWidthUV
	@pld     [pRefUMB, iWidthUV, lsl #1]
    ldrd    uData2, [pRefUMB], iWidthUV
	@pld     [pRefUMB, iWidthUV, lsl #1]
    ldrd    uData4, [pRefUMB], iWidthUV
    ldrd    uData6, [pRefUMB], iWidthUV
    strd    uData0, [pCurrQUMB], iWidthUV
    strd    uData2, [pCurrQUMB], iWidthUV
    strd    uData4, [pCurrQUMB], iWidthUV
    strd    uData6, [pCurrQUMB]
    
@ V 
	ldr		pRefVMB, [sp, #OFFSET_ppxliRefVMB]
	ldr		pCurrQVMB, [sp, #OFFSET_ppxliCurrQVMB]
@ row 0,1,2,3
	@pld     [pRefVMB]
	@pld     [pRefVMB, iWidthUV]
	@pld     [pRefVMB, iWidthUV, lsl #1]
    ldrd    uData0, [pRefVMB], iWidthUV
	@pld     [pRefVMB, iWidthUV, lsl #1]
    ldrd    uData2, [pRefVMB], iWidthUV
    ldrd    uData4, [pRefVMB], iWidthUV
	@pld     [pRefVMB, iWidthUV, lsl #1]
    ldrd    uData6, [pRefVMB], iWidthUV
	@pld     [pRefVMB, iWidthUV, lsl #1]
    strd    uData0, [pCurrQVMB], iWidthUV
    strd    uData2, [pCurrQVMB], iWidthUV
    strd    uData4, [pCurrQVMB], iWidthUV
    strd    uData6, [pCurrQVMB], iWidthUV
@ row 4,5,6,7
    ldrd    uData0, [pRefVMB], iWidthUV
	@pld     [pRefVMB, iWidthUV, lsl #1]
    ldrd    uData2, [pRefVMB], iWidthUV
	@pld     [pRefVMB, iWidthUV, lsl #1]
    ldrd    uData4, [pRefVMB], iWidthUV
    ldrd    uData6, [pRefVMB], iWidthUV
    strd    uData0, [pCurrQVMB], iWidthUV
    strd    uData2, [pCurrQVMB], iWidthUV
    strd    uData4, [pCurrQVMB], iWidthUV
    strd    uData6, [pCurrQVMB]
               
    ldmfd   sp!, {r4 - r11, PC}
	WMV_ENTRY_END	@ARMV6_g_MotionCompZeroMotion_WMV

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .if PRO_VER != 0

@ void IntensityCompensation_asm(U8_WMV  *pSrcY,U8_WMV *pSrcU,	U8_WMV *pSrcV, I32_WMV y_num, 
@				I32_WMV uv_num, I32_WMV iScale, I32_WMV iShift)
@{
@        I32_WMV     i, j, sum@
@        U8_WMV  pLUT[256], pLUTUV[256]@
@
@	sum == iScale * 128@
@	sum == 8224 - sum@
@	iShift == iShift + 32@
@        for (i == 0@ i < 256@ i++) 
@	{
@	     I32_WMV tmp@
@	     tmp == iScale * i@
@            j == ( tmp + iShift) >> 6@
@            if (j > 255)
@                j == 255@
@            else if (j < 0)
@                j == 0@
@            pLUT[i] == (U8_WMV) j@
@
@            j == (tmp + sum) >>6@
@            if (j > 255)
@                j == 255@
@            else if (j < 0)
@                j == 0@
@            pLUTUV[i] == (U8_WMV) j@
@        }
@                
@        for(i == 0@ i < y_num@ i++)
@            *pSrcY++ == pLUT[*pSrcY]@
@
@        for (i == 0@ i < uv_num@ i++) {
@            *pSrcU++ == pLUTUV[*pSrcU]@
@            *pSrcV++ == pLUTUV[*pSrcV]@
@        }
@} 

    WMV_LEAF_ENTRY ARMV6_IntensityCompensation_asm

     stmdb     sp!, {r4-r11, r14}
     
@     ldr	r14, [sp, #36]		@r14 == uv_num
     ldr	r12, [sp, #40]		@r12 == iScale
     ldr	r11, [sp, #44]		@r11 == iShift    r0 ==  pSrcY@ r1 == pSrcU@ r2 == pSrcV@ r3 == y_num@
     
     sub	sp, sp, #516
     mov	r10, #255
@	sum == iScale * 128@
@	sum == 8224 - sum@
@	iShift == iShift + 32@
@        for (i == 0@ i < 256@ i++) 
@	{
@	     I32_WMV tmp@
@	     tmp == iScale * i@
@            j == ( tmp + iShift) >> 6@
@            if (j > 255)
@                j == 255@
@            else if (j < 0)
@                j == 0@
@            pLUT[i] == (U8_WMV) j@
@
@            j == (tmp + sum) >>6@
@            if (j > 255)
@                j == 255@
@            else if (j < 0)
@                j == 0@
@            pLUTUV[i] == (U8_WMV) j@
@        }   
    mov	r4, r12, lsl #7  
    mov	r14, #1
    rsb	r4, r4, r14, lsl #13
    add	r4, r4, #32			@sum
    add	r11, r11, #32			@iShift
    add	sp, sp, #255			@pLUT[255]  
    add	r9, sp, #256			@pLUTUV[255]     
loop_256:
    mul	r5, r12, r10			@tmp1
    sub	r10, r10, #1
    mul	r14, r12, r10			@tmp2
                 
    add	r6, r5, r11
    mov	r6, r6, asr #6
	usat		r6, #8, r6
  	
    add	r7, r14, r11
    mov	r7, r7, asr #6
	usat		r7, #8, r7

    strb	r6, [sp], #-1
    strb	r7, [sp], #-1  
		    
    add	r6, r5, r4
    mov	r6, r6, asr #6
	usat		r6, #8, r6	
       
    add	r7, r14, r4
    mov	r7, r7, asr #6
	usat		r7, #8, r7
    strb	r6, [r9], #-1 
    strb	r7, [r9], #-1      
           
    subs	r10, r10, #1
    bgt	loop_256

	add	sp, sp, #1       
	add	r9, r9, #1   
    ldr	r14, [sp, #552]		@r14 == uv_num	@r9 == pLUTUV[0]		@sp == pLUT[0]
							@r0 ==  pSrcY@ r1 == pSrcU@ r2 == pSrcV@ r3 == y_num@
	
@        for(i == 0@ i < y_num@ i++)
@            *pSrcY++ == pLUT[*pSrcY]@

loop_y_num:
	ldrb	r4, [r0]
	ldrb	r5, [r0, #1]
	ldrb	r6, [r0, #2]
	ldrb	r7, [r0, #3]

	ldrb	r8, [sp, r4]
	ldrb	r4, [sp, r5]
	ldrb	r5, [sp, r6]
	ldrb	r6, [sp, r7]

	orr		r4, r8, r4, lsl #8
	orr		r4, r4, r5, lsl #16
	orr		r4, r4, r6, lsl #24

	ldrb	r8, [r0, #4]
	ldrb	r5, [r0, #5]
	ldrb	r6, [r0, #6]
	ldrb	r7, [r0, #7]

	ldrb	r12, [sp, r8]
	ldrb	r8, [sp, r5]
	ldrb	r5, [sp, r6]
	ldrb	r6, [sp, r7]

	orr		r12, r12, r8, lsl #8
	orr		r5, r12, r5, lsl #16
	orr		r5, r5, r6, lsl #24

	strd	r4, [r0], #8
    subs	r3, r3, #8
    bgt	loop_y_num
@
@        for (i == 0@ i < uv_num@ i++) {
@            *pSrcU++ == pLUTUV[*pSrcU]@
@            *pSrcV++ == pLUTUV[*pSrcV]@
@        }

loop_uv_num:
@u
	ldrb	r4, [r1]
	ldrb	r5, [r1, #1]
	ldrb	r6, [r1, #2]
	ldrb	r7, [r1, #3]

	ldrb	r8, [r9, r4]
	ldrb	r4, [r9, r5]
	ldrb	r5, [r9, r6]
	ldrb	r6, [r9, r7]

	orr		r4, r8, r4, lsl #8
	orr		r4, r4, r5, lsl #16
	orr		r4, r4, r6, lsl #24

	ldrb	r8, [r1, #4]
	ldrb	r5, [r1, #5]
	ldrb	r6, [r1, #6]
	ldrb	r7, [r1, #7]

	ldrb	r12, [r9, r8]
	ldrb	r8, [r9, r5]
	ldrb	r5, [r9, r6]
	ldrb	r6, [r9, r7]

	orr		r12, r12, r8, lsl #8
	orr		r5, r12, r5, lsl #16
	orr		r5, r5, r6, lsl #24

	strd	r4, [r1], #8
@v
	ldrb	r4, [r2]
	ldrb	r5, [r2, #1]
	ldrb	r6, [r2, #2]
	ldrb	r7, [r2, #3]

	ldrb	r8, [r9, r4]
	ldrb	r4, [r9, r5]
	ldrb	r5, [r9, r6]
	ldrb	r6, [r9, r7]

	orr		r4, r8, r4, lsl #8
	orr		r4, r4, r5, lsl #16
	orr		r4, r4, r6, lsl #24

	ldrb	r8, [r2, #4]
	ldrb	r5, [r2, #5]
	ldrb	r6, [r2, #6]
	ldrb	r7, [r2, #7]

	ldrb	r12, [r9, r8]
	ldrb	r8, [r9, r5]
	ldrb	r5, [r9, r6]
	ldrb	r6, [r9, r7]

	orr		r12, r12, r8, lsl #8
	orr		r5, r12, r5, lsl #16
	orr		r5, r5, r6, lsl #24

	strd	r4, [r2], #8
    subs	r14, r14, #8
    bgt	loop_uv_num
	     
                     
     add	sp, sp, #516                     
     ldmia     sp!, {r4-r11,pc}  
        
    .endif  @PRO_VER

	

    .endif @WMV_OPT_MOTIONCOMP_ARM

    @@.end 
