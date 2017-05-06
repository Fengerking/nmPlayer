    #include "../../Src/c/voVC1DID.h"
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 
    
    .IF WMV_OPT_MOTIONCOMP_ARM == 1
    
	@AREA	|.text|, CODE, READONLY
    .section .text
	
    .extern  g_AddError_EMB_Overflow_C
	.global	ARMV6_g_AddError_SSIMD
    .global  ARMV6_g_AddNull_SSIMD
  

@Void_WMV g_AddError_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   WMV_LEAF_ENTRY ARMV6_g_AddError_SSIMD
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@r0 == ppxlcDst
@r1 == pRef
@r2 == pErrorBuf
@r3 == iPitch
@r12==err_overflow
@r14==tmp
	
    stmdb     sp!, {r4 - r10, r14}
    FRAME_PROFILE_COUNT
    
@    I32_WMV iy@
@    U32_WMV err_overflow == 0@
@    U32_WMV u0,u1,u2,u3, y0,y1@
@    U32_WMV err_even, err_odd@
@    U32_WMV err_even1, err_odd1@   

	mov r12, #0
	mov r10, #8
AEE_Loop2:


@        u1 == pRef[0 + 5]@
@        u3 == pRef[1 + 5]@  
@        err_odd  == pErrorBuf[0 + 16]@
@        err_odd1  == pErrorBuf[1 + 16]@   

        ldrd r6, [ r2, #64] @      
        ldr r4, [ r1, #20] @
		ldr r5, [ r1, #24] @
		
@        u1 == u1 + err_odd -((err_odd  & 0x8000) << 1)@
@        err_overflow  |== u1@
@        u3 == u3 + err_odd1 -((err_odd1  & 0x8000) << 1)@
@        err_overflow  |== u3@

        ands r14, r6, #0x8000 
        and r9, r7, #0x8000
        subne r6, r6, r14, lsl #1
        add r8, r4, r6 
        orr r12, r12, r8                     
        sub r7, r7, r9, lsl #1
        add r9, r5, r7 
        
@        u0 == pRef[0]@
@        u2 == pRef[1]@ 
@        pRef +== 10@            
@        err_even == pErrorBuf[0]@
@        err_even1 == pErrorBuf[1]@   
@        pErrorBuf +== 2@ 
     
        ldrd r6, [ r2], #8@        
        ldrd r4, [ r1], #40@   
                                                 
@        u0 == u0 + err_even-((err_even & 0x8000) << 1)@
@        err_overflow  |== u0@
@        u2 == u2 + err_even1-((err_even1 & 0x8000) << 1)@
@        err_overflow  |== u2@

        ands r14, r6, #0x8000
        orr r12, r12, r9    
        subne r6, r6, r14, lsl #1
        add r4, r4, r6 
        ands r14, r7, #0x8000
        orr r12, r12, r4   
                        
        subne r7, r7, r14, lsl #1
        add r5, r5, r7 
        orr r12, r12, r5    

@        y0 == (u0) | ((u1) << 8)@
@        y1 == (u2) | ((u3) << 8)@
        orr r4, r4, r8, lsl #8
        orr r5, r5, r9, lsl #8
		
@        *(U32_WMV *)ppxlcDst == y0@
@        *(U32_WMV *)(ppxlcDst + 4)== y1@
@        ppxlcDst +== iPitch@ 

        subs	r10, r10, #1          
        strd r4, [r0], r3                                                        

        bgt AEE_Loop2
    @}

    @DEBUG_STATEMENT(g_AddErr_cnt++@)

    mov r14, #0xff00
    orr r14, r14, r14, lsl #16

    @if(err_overflow & 0xff00ff00)
    @{

    tst r14, r12
    ldmeqia     sp!, {r4 - r10, pc}

    

        @ppxlcDst -== 8*iPitch@
        sub r0, r0, r3, lsl #3

        @pRef -== 10*8@
        sub r1, r1, #80*4
        
        @pErrorBuf -== 16@
        sub r2, r2, #16*4

        @g_AddError_EMB_Overflow(ppxlcDst, pRef , pErrorBuf, iPitch)@

        
        bl g_AddError_EMB_Overflow_C
        
        ldmia     sp!, {r4 - r10, pc}
    
@  }
	
    WMV_ENTRY_END	@ARMV6_g_AddError_SSIMD
 

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@Void_WMV g_AddNull_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch)
@{
@    I32_WMV iy@
@    U32_WMV u0,u1,u2,u3, y0,y1@  
@    for (iy == 0@ iy < 8@ iy++) 
@    {     
@        u0 == pRef[0]@
@        u1 == pRef[0 + 5]@
@        u2 == pRef[1]@
@        u3 == pRef[1 + 5]@
@
@        pRef +== 10@
@        
@        y0 == (u0) | ((u1) << 8)@
@        y1 == (u2) | ((u3) << 8)@
@        
@        *(U32_WMV *)ppxlcDst == y0@
@        *(U32_WMV *)(ppxlcDst + 4)== y1@
@        ppxlcDst +== iPitch@
@    }
@}

    @AREA  |.text|, CODE
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    WMV_LEAF_ENTRY ARMV6_g_AddNull_SSIMD
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
@r0 == ppxlcDst
@r1 == pRef
@r2 == iPitch
@r3 == iy
@r4 == u0
@r5 == u1
@r12 == u2
@r14 == u3

    stmdb     sp!, {r4 - r11}
    FRAME_PROFILE_COUNT
	
	pld	[r1]		
	pld	[r1, #40]		

    ldr  r6, [r1, #20]			@u1
    ldr  r7, [r1, #24]	
    ldrd  r4, [r1], #40		@u0
    ldr  r10, [r1, #20]		@u1
    ldr  r11, [r1, #24]	
    ldrd  r8, [r1], #40		@u0 40
    
    orr r5, r5, r7, lsl #8	@y1 == (u2) | ((u3) << 8)@
	pld	[r1, #40]		
	pld	[r1, #80]		

    orr r4, r4, r6,  lsl #8	@y0 == (u0) | ((u1) << 8)@
    orr r9, r9, r11, lsl #8	@y1 == (u2) | ((u3) << 8)@

    strd r4, [r0], r2
    orr r8, r8, r10,  lsl #8	@y0 == (u0) | ((u1) << 8)@
    strd r8, [r0], r2
	
    ldr  r6, [r1, #20]			@u1	
    ldr  r7, [r1, #24]		
    ldrd  r4, [r1], #40		@u0 80
    ldr  r10, [r1, #20]			@u1
    ldr  r11, [r1, #24]		
    ldrd  r8, [r1], #40		@u0 120
    
    orr r5, r5, r7, lsl #8	@y1 == (u2) | ((u3) << 8)@
	pld	[r1, #40]		
	pld	[r1, #80]		

    orr r4, r4, r6,  lsl #8	@y0 == (u0) | ((u1) << 8)@
    orr r9, r9, r11, lsl #8	@y1 == (u2) | ((u3) << 8)@

    strd r4, [r0], r2
    orr r8, r8, r10,  lsl #8	@y0 == (u0) | ((u1) << 8)@
    strd r8, [r0], r2
	
    ldr  r6, [r1, #20]			@u1	
    ldr  r7, [r1, #24]		
    ldrd  r4, [r1], #40		@u0 160
    ldr  r10, [r1, #20]			@u1
    ldr  r11, [r1, #24]		
    ldrd  r8, [r1], #40		@u0 200
    
    orr r5, r5, r7, lsl #8	@y1 == (u2) | ((u3) << 8)@
	pld	[r1, #40]		
	pld	[r1, #80]		

    orr r4, r4, r6,  lsl #8	@y0 == (u0) | ((u1) << 8)@
    orr r9, r9, r11, lsl #8	@y1 == (u2) | ((u3) << 8)@

    strd r4, [r0], r2
    orr r8, r8, r10,  lsl #8	@y0 == (u0) | ((u1) << 8)@
    strd r8, [r0], r2

    ldr  r6, [r1, #20]			@u1
    ldr  r7, [r1, #24]		
    ldrd  r4, [r1], #40		@u0 240
    ldr  r10, [r1, #20]		@u1	
    ldr  r11, [r1, #24]		
    ldrd  r8, [r1]	@u0 280

    orr r5, r5, r7, lsl #8	@y1 == (u2) | ((u3) << 8)@
    orr r4, r4, r6,  lsl #8	@y0 == (u0) | ((u1) << 8)@

    orr r9, r9, r11, lsl #8	@y1 == (u2) | ((u3) << 8)@

    strd r4, [r0], r2
    orr r8, r8, r10,  lsl #8	@y0 == (u0) | ((u1) << 8)@
    strd r8, [r0]
 
    ldmia     sp!, {r4 - r11}
	mov	pc, r14

    WMV_ENTRY_END  @  ARMV6_g_AddNull_SSIMD
 
    
	.ENDIF  @    .IF WMV_OPT_MOTIONCOMP_ARM == 1
    
	.END
    