#include "voMP3DecID.h"
	.global	voMP3DecSynthMono
	@AREA	.text, CODE, READONLY
	.section .text
voMP3DecSynthMono: @PROC
	stmdb   sp!, {r4 - r11, lr}

	mov		r14, r0	
	mov		r0, r2	

	mov		r12, #4096
	@	MC0M(0)          
	@	MC0M(1)
	@	MC0M(2)          
	@	MC0M(3)          
    ldr     r2, [r1, #0]		@ vLo = *(vb1+(x))@
	ldr     r3, [r1, #4]		@ vLo = *(vb1+(x))@
	ldr     r4, [r1, #8]		@ vLo = *(vb1+(x+2))@
	ldr     r5, [r1, #12]		@ vLo = *(vb1+(x+2))@
	ldr     r6, [r1, #80]  	    @ vHi = *(vb1+(23-(x+2)))@
	ldr     r7, [r1, #84]  	    @ vHi = *(vb1+(23-(x+2)))@
	ldr     r8, [r1, #88]  	    @ vHi = *(vb1+(23-(x)))@ 
	ldr     r9, [r1, #92]  	    @ vHi = *(vb1+(23-(x)))@ 
	ldrd    r10, [r0, #0]		@ c1 = *coef@		coef++@		c2 = *coef@		coef++@
	pkhbt	r2, r2, r9, lsl #16
	pkhbt	r3, r3, r8, lsl #16
	smlsd	r12, r2, r10, r12
	pkhbt	r8, r4, r7, lsl #16
	smlsd	r12, r3, r11, r12
	pkhbt	r9, r5, r6, lsl #16
	ldrd    r10, [r0, #8]
	ldr     r2,  [r1, #16]  
	ldr     r3,  [r1, #20]  
	smlsd	r12, r8, r10, r12
	ldr     r4,  [r1, #24]
	ldr     r5,  [r1, #28] 
	smlsd	r12, r9, r11, r12
	ldr     r6,  [r1, #64]
	ldr     r7,  [r1, #68]
	ldr     r8,  [r1, #72] 
	ldr     r9,  [r1, #76] 
	ldrd    r10, [r0, #16]
	pkhbt	r2, r2, r9, lsl #16
	pkhbt	r3, r3, r8, lsl #16
	smlsd	r12, r2, r10, r12
	pkhbt	r8, r4, r7, lsl #16
	smlsd	r12, r3, r11, r12
	pkhbt	r9, r5, r6, lsl #16
	ldrd    r10, [r0, #24]
	add		r1, r1, #4096
	smlsd	r12, r8, r10, r12
	add		r0, r0, #512
	smlsd	r12, r9, r11, r12


	ldrd    r10, [r0, #0]	
	@*(pcm + 0) = ClipToShort(sum1L, (21-CSHIFT + DEF_NFRACBITS))@
	@*(pcm + 1) = ClipToShort(sum1R, (21-CSHIFT + DEF_NFRACBITS))@
	ssat	r8, #16, r12, asr #13 
	ldr     r2,  [r1, #0]
	ldr     r3,  [r1, #4]
	strh    r8, [r14, #0] 		    
	
	mov		r12, #4096	  
	  
	ldr     r4,  [r1, #8]  
	ldr     r5,  [r1, #12]  
	pkhbt	r2, r2, r3, lsl #16
	ldr     r6,  [r1, #16]  
	ldr     r7,  [r1, #20]  
	smlad	r12, r2, r10, r12
	pkhbt	r4, r4, r5, lsl #16
	ldr     r8,  [r1, #24]  
	ldr     r9,  [r1, #28]  
	smlad   r12, r4, r11, r12
	pkhbt	r6, r6, r7, lsl #16
	ldrd    r10, [r0, #8] 
	pkhbt	r8, r8, r9, lsl #16
	smlad   r12, r6, r10, r12
	sub     r0, r0, #480
	smlad   r12, r8, r11, r12
	mov     r4, #15		

	@*(pcm + 0) = ClipToShort(sum1L, (21-CSHIFT + DEF_NFRACBITS))@
	ssat	r8, #16, r12, asr #13    
	sub     r1, r1, #3840
	strh    r8, [r14, #32] 
	add     r14, r14, #2       

M493:
	mov		r12, #4096
	mov		r5, #4096	

	@	MC2M(0)
	@	MC2M(1)
	@	MC2M(2)
	@	MC2M(3)
	ldr     r2, [r1, #0]    @ vLo = *(vb1+(x))@            
	ldr     r3, [r1, #4]    @ vLo = *(vb1+(x))@            
	ldr     r8, [r1, #88]   @ vHi = *(vb1+(23-(x)))@         
	ldr     r9, [r1, #92]   @ vHi = *(vb1+(23-(x)))@         
	ldrd    r10, [r0, #0]               
	pkhbt	r3, r3, r8, lsl #16
	pkhbt	r2, r2, r9, lsl #16
	ldr 	r6, [r1, #8]
	ldr 	r7, [r1, #12]
	smlsd	r12, r2, r10, r12
	smladx	r5, r2, r10, r5
	smlsd	r12, r3, r11, r12
	smladx	r5, r3, r11, r5
	ldr 	r2, [r1, #80]
	ldr 	r3, [r1, #84]
	ldrd    r10, [r0, #8]
	pkhbt	r7, r7, r2, lsl #16
	pkhbt	r6, r6, r3, lsl #16
	smlsd	r12, r7, r11, r12
	smladx	r5, r7, r11, r5
	smlsd	r12, r6, r10, r12
	smladx	r5, r6, r10, r5

	@	MC2M(4)
	@	MC2M(5)
	@	MC2M(6)
	@	MC2M(7)
	ldr     r2, [r1, #16]    @ vLo = *(vb1+(x))@            
	ldr     r3, [r1, #20]    @ vLo = *(vb1+(x))@            
	ldr     r8, [r1, #72]   @ vHi = *(vb1+(23-(x)))@         
	ldr     r9, [r1, #76]   @ vHi = *(vb1+(23-(x)))@         
	ldrd    r10, [r0, #16]               
	pkhbt	r3, r3, r8, lsl #16
	pkhbt	r2, r2, r9, lsl #16
	ldr 	r6, [r1, #24]
	ldr 	r7, [r1, #28]
	smlsd	r12, r2, r10, r12
	smladx	r5, r2, r10, r5
	smlsd	r12, r3, r11, r12
	smladx	r5, r3, r11, r5
	ldr 	r2, [r1, #64]
	ldr 	r3, [r1, #68]
	ldrd    r10, [r0, #24]	
	pkhbt	r7, r7, r2, lsl #16
	pkhbt	r6, r6, r3, lsl #16
	smladx	r5, r7, r11, r5
	smlsd	r12, r6, r10, r12
	add     r0, r0, #32
	smlsd	r12, r7, r11, r12
	smladx	r5, r6, r10, r5
	add     r1, r1, #256
	
	ssat	r2, #16, r12, asr #13          
 	ssat	r3, #16, r5, asr #13   
	mov     r6, r4, lsl #2  
	strh    r2, [r14, #0]            
	strh    r3, [r14, r6] 
		   
	sub     r4, r4, #1 
	add     r14, r14, #2 
	cmp     r4, #0  
	bgt     M493

	ldmia   sp!, {r4 - r11, pc}

	@ENDFUNC

	.global	voMP3DecSynthStereo
	@AREA	.text, CODE, READONLY

voMP3DecSynthStereo: @PROC
	stmdb   sp!, {r4 - r11, lr}
	sub     sp, sp, #0x8	
	
	str		r0, [sp, #4]	
	mov		r0, r2	
	
	mov		r12, #4096
	mov		r14, #4096
	@	MC0S(0)          
	@	MC0S(1)     
	ldrd    r10, [r0, #0]		@ c1 = *coef@		coef++@		c2 = *coef@		coef++@
	ldr     r2, [r1, #0]		@ vLo = *(vb1+(x))@
	ldr     r3, [r1, #4]		@ vLo = *(vb1+(x))@
	ldr     r4, [r1, #88]  	    @ vHi = *(vb1+(23-(x)))@ 
	ldr     r5, [r1, #92]  	    @ vHi = *(vb1+(23-(x)))@ 
	ldr     r6, [r1, #128]		@ vLo = *(vb1+32+(x))@
	ldr     r7, [r1, #132]		@ vLo = *(vb1+32+(x))@
	ldr     r8, [r1, #216] 		@ vHi = *(vb1+32+(23-(x)))@	
	ldr     r9, [r1, #220] 		@ vHi = *(vb1+32+(23-(x)))@	
	pkhbt	r2, r2, r5, lsl #16	
	pkhbt	r3, r3, r4, lsl #16
	smlsd	r12, r2, r10, r12
	pkhbt	r6, r6, r9, lsl #16
	pkhbt	r7, r7, r8, lsl #16
	smlsd	r14, r6, r10, r14
	smlsd	r12, r3, r11, r12
	smlsd	r14, r7, r11, r14

	@	MC0S(2)          
	@	MC0S(3) 
    ldrd    r10, [r0, #8]		@ c1 = *coef@		coef++@		c2 = *coef@		coef++@
	ldr     r2,  [r1, #8]		@ vLo = *(vb1+(x))@
	ldr     r3,  [r1, #12]		@ vLo = *(vb1+(x))@
	ldr     r4, [r1, #80]  	    @ vHi = *(vb1+(23-(x)))@ 
	ldr     r5, [r1, #84]  	    @ vHi = *(vb1+(23-(x)))@ 
	ldr     r6, [r1, #136]		@ vLo = *(vb1+32+(x))@
	ldr     r7, [r1, #140]		@ vLo = *(vb1+32+(x))@
	ldr     r8, [r1, #208] 		@ vHi = *(vb1+32+(23-(x)))@
	ldr     r9, [r1, #212] 		@ vHi = *(vb1+32+(23-(x)))@
	pkhbt	r2, r2, r5, lsl #16	
	pkhbt	r3, r3, r4, lsl #16
	smlsd	r12, r2, r10, r12
	pkhbt	r6, r6, r9, lsl #16
	pkhbt	r7, r7, r8, lsl #16
	smlsd	r14, r6, r10, r14
	smlsd	r12, r3, r11, r12
	smlsd	r14, r7, r11, r14
	
	@	MC0S(4)          
	@	MC0S(5) 
    ldrd    r10, [r0, #16]		@ c1 = *coef@		coef++@		c2 = *coef@		coef++@
	ldr     r2,  [r1, #16]		@ vLo = *(vb1+(x))@
	ldr     r3,  [r1, #20]		@ vLo = *(vb1+(x))@
	ldr     r4, [r1, #72]  	    @ vHi = *(vb1+(23-(x)))@ 
	ldr     r5, [r1, #76]  	    @ vHi = *(vb1+(23-(x)))@ 
	ldr     r6, [r1, #144]		@ vLo = *(vb1+32+(x))@
	ldr     r7, [r1, #148]		@ vLo = *(vb1+32+(x))@
	ldr     r8, [r1, #200] 		@ vHi = *(vb1+32+(23-(x)))@
	ldr     r9, [r1, #204] 		@ vHi = *(vb1+32+(23-(x)))@
	pkhbt	r2, r2, r5, lsl #16	
	pkhbt	r3, r3, r4, lsl #16
	smlsd	r12, r2, r10, r12
	pkhbt	r6, r6, r9, lsl #16
	pkhbt	r7, r7, r8, lsl #16
	smlsd	r14, r6, r10, r14
	smlsd	r12, r3, r11, r12
	smlsd	r14, r7, r11, r14
	
	@	MC0S(6)          
	@	MC0S(7) 
    ldrd    r10, [r0, #24]		@ c1 = *coef@		coef++@		c2 = *coef@		coef++@
	ldr     r2,  [r1, #24]		@ vLo = *(vb1+(x))@
	ldr     r3,  [r1, #28]		@ vLo = *(vb1+(x))@
	ldr     r4, [r1, #64]  	    @ vHi = *(vb1+(23-(x)))@ 
	ldr     r5, [r1, #68]  	    @ vHi = *(vb1+(23-(x)))@ 
	ldr     r6, [r1, #152]		@ vLo = *(vb1+32+(x))@
	ldr     r7, [r1, #156]		@ vLo = *(vb1+32+(x))@
	ldr     r8, [r1, #192] 		@ vHi = *(vb1+32+(23-(x)))@
	ldr     r9, [r1, #196] 		@ vHi = *(vb1+32+(23-(x)))@
	pkhbt	r2, r2, r5, lsl #16	
	pkhbt	r3, r3, r4, lsl #16
	smlsd	r12, r2, r10, r12
	pkhbt	r6, r6, r9, lsl #16
	pkhbt	r7, r7, r8, lsl #16
	smlsd	r14, r6, r10, r14
	smlsd	r12, r3, r11, r12
	smlsd	r14, r7, r11, r14
	
	ldr		r6, [sp, #4]	 

	@*(pcm + 0) = ClipToShort(sum1L, (21-CSHIFT + DEF_NFRACBITS))@
	@*(pcm + 1) = ClipToShort(sum1R, (21-CSHIFT + DEF_NFRACBITS))@
	ssat	r8, #16, r12, asr #13 
	ssat	r9, #16, r14, asr #13 

	add		r4, r0, #512
	pkhbt	r8, r8, r9, lsl #16   
	add		r5, r1, #4096		
	str     r8, [r6, #0] 	

	mov		r12, #4096
	mov		r14, #4096

	@MC1S(0)
	@MC1S(1)
	@MC1S(2)
	@MC1S(3)
	ldr     r2,  [r5, #0]    
	ldr     r3,  [r5, #4]    
	ldr     r8,  [r5, #128] 
	ldr     r9,  [r5, #132] 
	ldrd    r10, [r4, #0]     
	pkhbt	r2, r2, r3, lsl #16
	pkhbt	r8, r8, r9, lsl #16
	smlad	r12, r2, r10, r12
	smlad	r14, r8, r10, r14
	ldr     r2,  [r5, #8]    
	ldr     r3,  [r5, #12]    
	ldr     r8,  [r5, #136]  
	ldr     r9,  [r5, #140]  
	pkhbt	r2, r2, r3, lsl #16
	pkhbt	r8, r8, r9, lsl #16
	smlad	r12, r2, r11, r12
	smlad	r14, r8, r11, r14
	
	@MC1S(4)
	@MC1S(5)
	@MC1S(6)
	@MC1S(7)        
	ldr     r2,  [r5, #16]    
	ldr     r3,  [r5, #20]    
	ldr     r8,  [r5, #144] 
	ldr     r9,  [r5, #148] 
	ldrd    r10, [r4, #8] 	 
	pkhbt	r2, r2, r3, lsl #16
	pkhbt	r8, r8, r9, lsl #16
	smlad	r12, r2, r10, r12
	smlad	r14, r8, r10, r14
	ldr     r2,  [r5, #24]    
	ldr     r3,  [r5, #28]    
	ldr     r8,  [r5, #152]  
	ldr     r9,  [r5, #156]  
	pkhbt	r2, r2, r3, lsl #16
	pkhbt	r8, r8, r9, lsl #16
	smlad	r12, r2, r11, r12
	smlad	r14, r8, r11, r14   
	
	@*(pcm + 0) = ClipToShort(sum1L, (21-CSHIFT + DEF_NFRACBITS))@
	@*(pcm + 1) = ClipToShort(sum1R, (21-CSHIFT + DEF_NFRACBITS))@
	ssat	r8, #16, r12, asr #13 
	ssat	r9, #16, r14, asr #13 

	add     r7, r0, #32 
	pkhbt	r8, r8, r9, lsl #16   
	mov     r4, #15  
	str     r8, [r6, #64] 
	add     r5, r6, #4       	
	add     r6, r1, #256    

M1493:
	mov		r12, #4096
	mov		r14, #4096
	mov		r8, #4096
	mov		r9, #4096

	@	MC2S(0)
	@	MC2S(1)              
	ldr     r0, [r6, #0]                
	ldr     r1, [r6, #92]               
	ldr     r2, [r6, #128] 
	ldr     r3, [r6, #220]
	ldrd    r10, [r7, #0] 
	pkhbt	r0, r0, r1, lsl #16
	pkhbt	r2, r2, r3, lsl #16
	smlsd	r12, r0, r10, r12
	smlsd	r14, r2, r10, r14
	smladx	r8, r0, r10, r8
	smladx	r9, r2, r10, r9             
	ldr     r0, [r6, #4]                
	ldr     r1, [r6, #88]               
	ldr     r2, [r6, #132] 
	ldr     r3, [r6, #216]	  
	pkhbt	r0, r0, r1, lsl #16
	pkhbt	r2, r2, r3, lsl #16
	smlsd	r12, r0, r11, r12
	smlsd	r14, r2, r11, r14
	smladx	r8, r0, r11, r8
	smladx	r9, r2, r11, r9 

	@	MC2S(2)
	@	MC2S(3)              
	ldr     r0, [r6, #8]                
	ldr     r1, [r6, #84]               
	ldr     r2, [r6, #136] 
	ldr     r3, [r6, #212]
	ldrd    r10, [r7, #8] 
	pkhbt	r0, r0, r1, lsl #16
	pkhbt	r2, r2, r3, lsl #16
	smlsd	r12, r0, r10, r12
	smlsd	r14, r2, r10, r14
	smladx	r8, r0, r10, r8
	smladx	r9, r2, r10, r9             
	ldr     r0, [r6, #12]                
	ldr     r1, [r6, #80]               
	ldr     r2, [r6, #140]
	ldr     r3, [r6, #208]	  
	pkhbt	r0, r0, r1, lsl #16
	pkhbt	r2, r2, r3, lsl #16
	smlsd	r12, r0, r11, r12
	smlsd	r14, r2, r11, r14
	smladx	r8, r0, r11, r8
	smladx	r9, r2, r11, r9 

	@	MC2S(4)
	@	MC2S(5)              
	ldr     r0, [r6, #16]                
	ldr     r1, [r6, #76]               
	ldr     r2, [r6, #144] 
	ldr     r3, [r6, #204]
	ldrd    r10, [r7, #16] 
	pkhbt	r0, r0, r1, lsl #16
	pkhbt	r2, r2, r3, lsl #16
	smlsd	r12, r0, r10, r12
	smlsd	r14, r2, r10, r14
	smladx	r8, r0, r10, r8
	smladx	r9, r2, r10, r9             
	ldr     r0, [r6, #20]                
	ldr     r1, [r6, #72]               
	ldr     r2, [r6, #148]
	ldr     r3, [r6, #200]	  
	pkhbt	r0, r0, r1, lsl #16
	pkhbt	r2, r2, r3, lsl #16
	smlsd	r12, r0, r11, r12
	smlsd	r14, r2, r11, r14
	smladx	r8, r0, r11, r8
	smladx	r9, r2, r11, r9 

	@	MC2S(6)
	@	MC2S(7)              
	ldr     r0, [r6, #24]                
	ldr     r1, [r6, #68]               
	ldr     r2, [r6, #152]
	ldr     r3, [r6, #196]
	ldrd    r10, [r7, #24] 
	pkhbt	r0, r0, r1, lsl #16
	pkhbt	r2, r2, r3, lsl #16
	smlsd	r12, r0, r10, r12
	smlsd	r14, r2, r10, r14
	smladx	r8, r0, r10, r8
	smladx	r9, r2, r10, r9             
	ldr     r0, [r6, #28]                
	ldr     r1, [r6, #64]               
	ldr     r2, [r6, #156]
	ldr     r3, [r6, #192]	  
	pkhbt	r0, r0, r1, lsl #16
	pkhbt	r2, r2, r3, lsl #16
	add     r7, r7, #32
	smlsd	r12, r0, r11, r12
	smlsd	r14, r2, r11, r14
	add     r6, r6, #256 
	smladx	r8, r0, r11, r8
	smladx	r9, r2, r11, r9       
    
	ssat	r0, #16, r12, asr #13  
	ssat	r1, #16, r14, asr #13
	ssat	r2, #16, r8, asr #13  
	ssat	r3, #16, r9, asr #13 		
	pkhbt	r0, r0, r1, lsl #16 
	mov     r8, r4, lsl #3  
	pkhbt	r2, r2, r3, lsl #16 
	str     r0, [r5, #0]            
	str     r2, [r5, r8] 
			   
	sub     r4, r4, #1 
	add     r5, r5, #4 
	cmp     r4, #0  
	bgt     M1493

	add     sp, sp, #0x8
	ldmia   sp!, {r4 - r11, pc}

	@ENDFUNC

	.END
        
