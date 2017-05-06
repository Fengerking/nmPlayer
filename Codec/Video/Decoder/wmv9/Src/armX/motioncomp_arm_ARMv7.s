
    INCLUDE wmvdec_member_arm.inc
    INCLUDE xplatform_arm_asm.h 
    IF UNDER_CE != 0
    INCLUDE kxarm.h
    ENDIF   
 
    AREA |.text|, CODE, READONLY

    IF WMV_OPT_MOTIONCOMP_ARM = 1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    EXPORT  ARMV7_g_MotionCompZeroMotion_WMV 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  


;Void_WMV MotionCompZeroMotion_WMV (
;    U8_WMV* ppxliCurrQYMB, 
;    U8_WMV* ppxliCurrQUMB, 
;    U8_WMV* ppxliCurrQVMB,
;    const U8_WMV* ppxliRefYMB, 
;    const U8_WMV* ppxliRefUMB, 
;    const U8_WMV* ppxliRefVMB,
;    I32_WMV iWidthY,
;    I32_WMV iWidthUV
;)
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    WMV_LEAF_ENTRY ARMV7_g_MotionCompZeroMotion_WMV
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;      
;r0: ppxliCurrQYMB
;r1: ppxliCurrQUMB
;r2: ppxliCurrQVMB
;r3: ppxliRefYMB
;r4: ppxliRefUMB
;r5: ppxliRefVMB
;r6: iWidthY
;r7: iWidthUV

OFFSET_registers_save	EQU		20
OFFSET_ppxliRefUMB		EQU		OFFSET_registers_save + 0
OFFSET_ppxliRefVMB		EQU		OFFSET_registers_save + 4		
OFFSET_iWidthY			EQU		OFFSET_registers_save + 8		
OFFSET_iWidthUV			EQU		OFFSET_registers_save + 12	
	

    STMFD   sp!, {r4 - r7, r14} 
    FRAME_PROFILE_COUNT

	pld			[r3]       
	ldr		r6, [sp, #OFFSET_iWidthY]
	ldr		r7, [sp, #OFFSET_iWidthUV]
	ldr		r4, [sp, #OFFSET_ppxliRefUMB]
	ldr		r5, [sp, #OFFSET_ppxliRefVMB]

; Y    
	pld			[r3, r6]       
	pld			[r3, r6, lsl #1]
	vld1.u32	{d0, d1}, [r3], r6
	pld			[r3, r6, lsl #1]
	vld1.u32	{d2, d3}, [r3], r6
	pld			[r3, r6, lsl #1]
	vld1.u32	{d4, d5}, [r3], r6
	pld			[r3, r6, lsl #1]
	vld1.u32	{d6, d7}, [r3], r6
	pld			[r3, r6, lsl #1]
	vld1.u32	{d8, d9}, [r3], r6
	pld			[r3, r6, lsl #1]
	vld1.u32	{d10, d11}, [r3], r6
	pld			[r3, r6, lsl #1]
	vld1.u32	{d12, d13}, [r3], r6
	pld			[r3, r6, lsl #1]
	vld1.u32	{d14, d15}, [r3], r6
	pld			[r3, r6, lsl #1]
	vld1.u32	{d16, d17}, [r3], r6
	pld			[r3, r6, lsl #1]
	vld1.u32	{d18, d19}, [r3], r6
	pld			[r3, r6, lsl #1]
	vld1.u32	{d20, d21}, [r3], r6
	pld			[r3, r6, lsl #1]
	vld1.u32	{d22, d23}, [r3], r6
	pld			[r3, r6, lsl #1]
	vld1.u32	{d24, d25}, [r3], r6
	pld			[r3, r6, lsl #1]
	pld			[r4]       
	pld			[r4, r7]       
	pld			[r4, r7, lsl #1]
	vld1.u32	{d26, d27}, [r3], r6
	vld1.u32	{d28, d29}, [r3], r6
	vld1.u32	{d30, d31}, [r3]	
	
	vst1.u32	{d0, d1}, [r0], r6
	vst1.u32	{d2, d3}, [r0], r6
	vst1.u32	{d4, d5}, [r0], r6
	vst1.u32	{d6, d7}, [r0], r6
	vst1.u32	{d8, d9}, [r0], r6
	vst1.u32	{d10, d11}, [r0], r6
	vst1.u32	{d12, d13}, [r0], r6
	vst1.u32	{d14, d15}, [r0], r6
	vst1.u32	{d16, d17}, [r0], r6
	vst1.u32	{d18, d19}, [r0], r6
	vst1.u32	{d20, d21}, [r0], r6
	vst1.u32	{d22, d23}, [r0], r6
	vst1.u32	{d24, d25}, [r0], r6
	vst1.u32	{d26, d27}, [r0], r6
	vst1.u32	{d28, d29}, [r0], r6
	vst1.u32	{d30, d31}, [r0]
	
; U, V    
	vld1.u32	d0, [r4], r7
	pld			[r4, r7, lsl #1]
	vld1.u32	d1, [r4], r7
	pld			[r4, r7, lsl #1]
	vld1.u32	d2, [r4], r7
	pld			[r4, r7, lsl #1]
	vld1.u32	d3, [r4], r7
	pld			[r4, r7, lsl #1]
	vld1.u32	d4, [r4], r7
	pld			[r4, r7, lsl #1]
	pld			[r5]       
	pld			[r5, r7]       
	pld			[r5, r7, lsl #1]
	vld1.u32	d5, [r4], r7
	vld1.u32	d6, [r4], r7
	vld1.u32	d7, [r4]
	vld1.u32	d8, [r5], r7
	pld			[r5, r7, lsl #1]
	vld1.u32	d9, [r5], r7
	pld			[r5, r7, lsl #1]
	vld1.u32	d10, [r5], r7
	pld			[r5, r7, lsl #1]
	vld1.u32	d11, [r5], r7
	pld			[r5, r7, lsl #1]
	vld1.u32	d12, [r5], r7
	pld			[r5, r7, lsl #1]
	vld1.u32	d13, [r5], r7
	vld1.u32	d14, [r5], r7
	vld1.u32	d15, [r5]
	
	vst1.u32	d0, [r1], r7
	vst1.u32	d1, [r1], r7
	vst1.u32	d2, [r1], r7
	vst1.u32	d3, [r1], r7
	vst1.u32	d4, [r1], r7
	vst1.u32	d5, [r1], r7
	vst1.u32	d6, [r1], r7
	vst1.u32	d7, [r1]
	vst1.u32	d8, [r2], r7
	vst1.u32	d9, [r2], r7
	vst1.u32	d10, [r2], r7
	vst1.u32	d11, [r2], r7
	vst1.u32	d12, [r2], r7
	vst1.u32	d13, [r2], r7
	vst1.u32	d14, [r2], r7
	vst1.u32	d15, [r2]

    LDMFD   sp!, {r4 - r7, PC}
	WMV_ENTRY_END
	

    ENDIF ;WMV_OPT_MOTIONCOMP_ARM

    END 
