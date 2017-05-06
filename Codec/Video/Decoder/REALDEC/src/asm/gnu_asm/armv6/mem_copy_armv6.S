@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2010		            *
@								 	                                    *
@***********************************************************************/

#include "../../voASMPort.h"

	@AREA	|.text|, CODE
	.section  .text
	
	.global	ARMV6_CopyBlock8x8
	.global	ARMV6_AddBlock8x8
	.global	ARMV6_CopyBlock16x16
	.global	ARMV6_AddBlock16x16

@-----------------------------------------------------------------------------------------------------
@void RV_FASTCALL CopyBlock8x8(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch)
@-----------------------------------------------------------------------------------------------------
   .align 8
ARMV6_CopyBlock8x8:	@PROC
		stmdb sp!,{r4-r11,lr}

		ldrd	r4,[r0], r2
		ldrd	r6,[r0], r2
		ldrd	r8,[r0], r2
		ldrd	r10,[r0], r2
		 	
		strd	r4,[r1], r3
		strd	r6,[r1], r3
		strd	r8,[r1], r3
		strd	r10,[r1], r3	
		
		ldrd	r4,[r0], r2
		ldrd	r6,[r0], r2
		ldrd	r8,[r0], r2
		ldrd	r10,[r0]
		
		strd	r4,[r1], r3
		strd	r6,[r1], r3
		strd	r8,[r1], r3
		strd	r10,[r1]
	
    ldmia sp!,{r4-r11,pc}
	@ENDP	

@-----------------------------------------------------------------------------------------------------
@void RV_FASTCALL AddBlock8x8(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch)
@-----------------------------------------------------------------------------------------------------
   .align 8
ARMV6_AddBlock8x8:	@PROC
		stmdb   sp!,{r4-r12,lr}
    
		mov     r4,#1
		mov     r5,r4,lsl #8
		orr     r6,r4,r5
		mov     r7,r6,lsl #16
		orr     r12,r6,r7 
		mov     lr,#8
AddBlock8x8Loop:
		ldrd 	  r4,[r0], r3
		ldrd	  r6,[r0], r3
		ldrd	  r8,[r1], r3
		ldrd	  r10,[r1], r3
		
		uqadd8  r4, r4, r12
		uqadd8  r5, r5, r12
		uqadd8  r6, r6, r12
		uqadd8  r7, r7, r12
		
		uhadd8	r4, r4, r8	
		uhadd8	r5, r5, r9
		uhadd8	r6, r6, r10
		uhadd8	r7, r7, r11
	  		 	
		strd	r4,[r2], r3
		strd	r6,[r2], r3
		
		subs	lr, lr, #2
		bgt		AddBlock8x8Loop
	
		ldmia sp!,{r4-r12,pc}

	@ENDP	

@-----------------------------------------------------------------------------------------------------
@void RV_FASTCALL CopyBlock16x16(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch)
@-----------------------------------------------------------------------------------------------------
   .align 8
ARMV6_CopyBlock16x16:	@PROC
    stmdb     sp!,{r4-r12,lr}
    
    ldrd	    r6,[r0, #8]      @0 1
		ldrd	    r4,[r0], r2
		ldrd	    r10,[r0, #8]
		ldrd	    r8,[r0], r2
		 	
		strd	    r6,[r1, #8]
		strd	    r4,[r1], r3
		strd	    r10,[r1, #8]
		strd	    r8,[r1], r3  
		
		ldrd	    r6,[r0, #8]      @2 3
		ldrd	    r4,[r0], r2
		ldrd	    r10,[r0, #8]
		ldrd	    r8,[r0], r2
		 	
		strd	    r6,[r1, #8]
		strd	    r4,[r1], r3
		strd	    r10,[r1, #8]
		strd	    r8,[r1], r3 
		
		ldrd	    r6,[r0, #8]      @4 5
		ldrd	    r4,[r0], r2
		ldrd	    r10,[r0, #8]
		ldrd	    r8,[r0], r2
		 	
		strd	    r6,[r1, #8]
		strd	    r4,[r1], r3
		strd	    r10,[r1, #8]
		strd	    r8,[r1], r3  
		
		ldrd	    r6,[r0, #8]      @6 7 
		ldrd	    r4,[r0], r2
		ldrd	    r10,[r0, #8]
		ldrd	    r8,[r0], r2
		 	
		strd	    r6,[r1, #8]
		strd	    r4,[r1], r3
		strd	    r10,[r1, #8]
		strd	    r8,[r1], r3  
		
		ldrd	    r6,[r0, #8]      @8 9
		ldrd	    r4,[r0], r2
		ldrd	    r10,[r0, #8]
		ldrd	    r8,[r0], r2
		 	
		strd	    r6,[r1, #8]
		strd	    r4,[r1], r3
		strd	    r10,[r1, #8]
		strd	    r8,[r1], r3 
		
		ldrd	    r6,[r0, #8]     @10 11
		ldrd	    r4,[r0], r2
		ldrd	    r10,[r0, #8]
		ldrd	    r8,[r0], r2
		 	
		strd	    r6,[r1, #8]
		strd	    r4,[r1], r3
		strd	    r10,[r1, #8]
		strd	    r8,[r1], r3   
		
		ldrd	    r6,[r0, #8]     @12 13
		ldrd	    r4,[r0], r2
		ldrd	    r10,[r0, #8]
		ldrd	    r8,[r0], r2
		 	
		strd	    r6,[r1, #8]
		strd	    r4,[r1], r3
		strd	    r10,[r1, #8]
		strd	    r8,[r1], r3 
		
		ldrd	    r6,[r0, #8]     @14 15
		ldrd	    r4,[r0], r2
		ldrd	    r10,[r0, #8]
		ldrd	    r8,[r0]
		 	
		strd	    r6,[r1, #8]
		strd	    r4,[r1], r3
		strd	    r10,[r1, #8]
		strd	    r8,[r1] 
	
    ldmia     sp!,{r4-r12,pc}
	@ENDP	

@-----------------------------------------------------------------------------------------------------
@void RV_FASTCALL AddBlock16x16(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch)
@-----------------------------------------------------------------------------------------------------
   .align 8
ARMV6_AddBlock16x16:	@PROC
    stmdb   sp!,{r4-r12,lr}
   
    mov     r4,#1
	mov     r5,r4,lsl #8
	orr     r6,r4,r5
	mov     r7,r6,lsl #16
	orr     r12,r6,r7 
    mov     lr,#16 
AddBlock16x16Loop:
    ldrd	  r6,[r0, #8]      
	ldrd	  r4,[r0], r3
	ldrd	  r10,[r1, #8]
	ldrd	  r8,[r1], r3
	
	uqadd8  r4, r4, r12
	uqadd8  r5, r5, r12
	uqadd8  r6, r6, r12
	uqadd8  r7, r7, r12
		
	uhadd8	r4, r4, r8	
	uhadd8	r5, r5, r9
	uhadd8	r6, r6, r10  
	uhadd8	r7, r7, r11
	  
	strd	  r6,[r2, #8]
	strd	  r4,[r2], r3
			
	subs      lr,lr,#1 
	bgt       AddBlock16x16Loop
	
    ldmia     sp!,{r4-r12,pc}

	@ENDP	