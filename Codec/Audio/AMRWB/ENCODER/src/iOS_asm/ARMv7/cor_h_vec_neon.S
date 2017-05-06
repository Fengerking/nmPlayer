@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@*   VisualOn, Inc. Confidential and Proprietary, 2003-2010 
@*  
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@static void cor_h_vec_012(
@		Word16 h[],                           /* (i) scaled impulse response                 */
@		Word16 vec[],                         /* (i) scaled vector (/8) to correlate with h[] */
@		Word16 track,                         /* (i) track to use                            */
@		Word16 sign[],                        /* (i) sign vector                             */
@		Word16 rrixix[][NB_POS],              /* (i) correlation of h[x] with h[x]      */
@		Word16 cor_1[],                       /* (o) result of correlation (NB_POS elements) */
@		Word16 cor_2[]                        /* (o) result of correlation (NB_POS elements) */
@)
@r0 ---- h[]
@r1 ---- vec[]
@r2 ---- track
@r3 ---- sign[]
@r4 ---- rrixix[][NB_POS]
@r5 ---- cor_1[]
@r6 ---- cor_2[]
              #include "voAMRWBEncID.h"
              .text   .align 4
	      .globl   _cor_h_vec_0_asm
              .globl   _cor_h_vec_1_asm
              .globl   _cor_h_vec_2_asm
              .globl   _cor_h_vec_3_asm
              .globl   _search_ixiy_asm
              .globl   _Syn_filt_opt 

_cor_h_vec_0_asm:				

	stmfd	        r13!,{r4 -r12,r14}		
	ldr		r7,[r13,#40]			
	ldr		r5,[r13,#44]			
	str		r0,[r13,#40]			
	str		r1,[r13,#44]			
	add		r3,r3,r2,lsl #1			
	mov		r14,#15
	ldr		r4,[r7,r2,lsl #2]		
	
vec_outer0:	
	ldr		r0,[r13,#40]			
	ldr		r1,[r13,#44]			
	rsb		r12,r2,#63			
	mov		r12,r12,asr #2			
	add		r1,r1,r2,lsl #1			
        mrs 	        r11,CPSR@
        bic		r11,r11,#0x08000000
        msr 	        CPSR_f,r11			
	sub		r6,r6,r6			
	sub		r7,r7,r7			
	ldrh	        r10,[r1],#2						
	ldrh	        r11,[r1],#2					

vec_inner0:		
 	ldr		r8,[r0],#4		
	ldr		r9,[r0],#4		
	smlabb		r6,r8,r10,r6		
	ldrh		r10,[r1],#2		
	smlabb		r7,r8,r11,r7		
	smlatb		r6,r8,r11,r6			
	ldrh		r11,[r1],#2		
	smlatb		r7,r8,r10,r7		
	smlabb		r6,r9,r10,r6		
	ldrh		r10,[r1],#2		
	smlabb		r7,r9,r11,r7		
	smlatb		r6,r9,r11,r6		
	ldrh		r11,[r1],#2		
	smlatb		r7,r9,r10,r7		
	subs		r12,r12,#1		
	bne		vec_inner0

	ldr		r8,[r0],#4			
	ldr		r9,[r0],#4			
	smlabb		r6,r8,r10,r6			   
	ldrh		r10,[r1],#2			
	smlabb		r7,r8,r11,r7			
	smlatb		r6,r8,r11,r6			
	ldrh		r11,[r1],#2			
	smlatb		r7,r8,r10,r7			
	smlabb		r6,r9,r10,r6			
	ldrh		r10,[r1],#2			
	smlabb		r7,r9,r11,r7			
	smlatb		r6,r9,r11,r6			

	
     	mrs		r11,CPSR@
        mov		r11,r11,asr #27
        and		r11,r11,#1
	cmp		r11,#1
	mvneq		r6,#0x80000000
	mvneq		r7,#0x80000000
	mov		r12,#32768			
	qadd		r6,r6,r6	
	qadd		r7,r7,r7
	qdadd		r8,r12,r6		
	qdadd		r9,r12,r7		
	ldrh		r10,[r3],#2			
	ldrh		r11,[r3],#6			
	subs		r14,r14,#1			
	ldrh		r6,[r4],#32			
	ldrh		r7,[r4],#-30			
	smultb		r8,r8,r10			
	smultb		r9,r9,r11			
	qadd		r8,r8,r8
	qadd		r9,r9,r9
	mov		r6,r6,lsl #16	
	mov		r7,r7,lsl #16	
	
	qadd		r8,r8,r6
	qadd		r9,r9,r7
	mov		r8,r8,asr #16	
	mov		r9,r9,asr #16	
	strh		r8,[r5],#32
	strh		r9,[r5],#-30
	add		r2,r2,#4
	bne		vec_outer0
					
	ldr		r0,[r13,#40]				
	ldr		r1,[r13,#44]				
	sub		r6,r6,r6				
	add		r1,r1,r2,lsl #1				
	sub		r7,r7,r7				
	ldrh		r10,[r1],#2				
	ldrh		r11,[r1],#2				
	ldr		r8,[r0],#4				
	ldr		r9,[r0],#4				
	smlabb		r6,r8,r10,r6				
	ldrh		r10,[r1],#2				
	smlabb		r7,r8,r11,r7				
	smlatb		r6,r8,r11,r6				
	ldrh		r11,[r1],#2				
	smlatb		r7,r8,r10,r7				
	smlabb		r6,r9,r10,r6				
	ldrh		r10,[r1],#2				
	smlabb		r7,r9,r11,r7				
	smlatb		r6,r9,r11,r6				
	qadd		r6,r6,r6	
	qadd		r7,r7,r7
	qdadd		r8,r12,r6				
	qdadd		r9,r12,r7			
	ldrh		r10,[r3],#2			
	ldrh		r11,[r3],#6			
	ldrh		r6,[r4],#32			
	ldrh		r7,[r4],#-30			
	smultb		r8,r8,r10			
	smultb		r9,r9,r11			
	qadd		r8,r8,r8
	qadd		r9,r9,r9
	mov		r6,r6,lsl #16	
	mov		r7,r7,lsl #16	
	qadd		r8,r8,r6
	qadd		r9,r9,r7
	mov		r8,r8,asr #16	
	mov		r9,r9,asr #16	
	strh		r8,[r5],#32
	strh		r9,[r5],#-30

	ldmfd		r13!,{r4 -r12,pc}		 
        
	
_cor_h_vec_1_asm:				

	stmfd		r13!,{r4 -r12,r14}	
	ldr		r7,[r13,#40]		
	ldr		r5,[r13,#44]		
	
	str		r0,[r13,#40]		
	str		r1,[r13,#44]		
	
	add		r3,r3,r2,lsl #1		
	mov		r14,#15
		
	ldr		r4,[r7,r2,lsl #2]	
	
vec_outer1:	
	ldr		r0,[r13,#40]		
	ldr		r1,[r13,#44]		
	rsb		r12,r2,#63		
	mov		r12,r12,asr #2		
	add		r1,r1,r2,lsl #1		
        mrs 		r11,CPSR@
        bic		r11,r11,#0x08000000
        msr 		CPSR_f,r11@
	sub		r6,r6,r6		
	sub		r7,r7,r7		
	ldrh		r10,[r1],#2		
	ldrh		r11,[r1],#2		


vec_inner1:	
	ldr		r8,[r0],#4		
	ldr		r9,[r0],#4		
	smlabb		r6,r8,r10,r6		
	ldrh		r10,[r1],#2		
	smlabb		r7,r8,r11,r7		
	smlatb		r6,r8,r11,r6			
	ldrh		r11,[r1],#2		
	smlatb		r7,r8,r10,r7		
	smlabb		r6,r9,r10,r6		
	ldrh		r10,[r1],#2		
	smlabb		r7,r9,r11,r7		
	smlatb		r6,r9,r11,r6		
	ldrh		r11,[r1],#2		
	smlatb		r7,r9,r10,r7		
	subs		r12,r12,#1			
	bne		vec_inner1

	ldr		r8,[r0],#4			
	ldr		r9,[r0],#4			
	smlabb		r6,r8,r10,r6			
	ldrh		r10,[r1],#2			
	smlabb		r7,r8,r11,r7			
	smlatb		r6,r8,r11,r6			
	smlatb		r7,r8,r10,r7			
	smlabb		r6,r9,r10,r6			
     	mrs		r11,CPSR@
        mov		r11,r11,asr #27
        and		r11,r11,#1
	cmp		r11,#1
	mvneq		r6,#0x80000000
	mvneq		r7,#0x80000000
	mov		r12,#32768			
	qadd		r6,r6,r6	
	qadd		r7,r7,r7
	qdadd		r8,r12,r6			
	qdadd		r9,r12,r7		
	ldrh		r10,[r3],#2			
	ldrh		r11,[r3],#6			
	subs		r14,r14,#1			
	ldrh		r6,[r4],#32			
	ldrh		r7,[r4],#-30			
	smultb		r8,r8,r10			
	smultb		r9,r9,r11			
	qadd		r8,r8,r8
	qadd		r9,r9,r9
	mov		r6,r6,lsl #16	
	mov		r7,r7,lsl #16	
	qadd		r8,r8,r6
	qadd		r9,r9,r7
	mov		r8,r8,asr #16	
	mov		r9,r9,asr #16	
	strh		r8,[r5],#32
	strh		r9,[r5],#-30
	add		r2,r2,#4
	bne		vec_outer1
					
	ldr		r0,[r13,#40]				@get h[]
	ldr		r1,[r13,#44]				@get vec[]
	sub		r6,r6,r6					@clear accumulator
	add		r1,r1,r2,lsl #1				@p2 = vec + pos
	sub		r7,r7,r7					@clear accumulator
	ldrh		r10,[r1],#2					@tmp = *p2++	
	ldrh		r11,[r1],#2					@tmp = *p2++
	ldr		r8,[r0],#4					@temp= *p1++
	ldr		r9,[r0],#4					@temp= *p1++
	smlabb		r6,r8,r10,r6				@L_temp1 = L_multbb(temp,tmp)
	ldrh		r10,[r1],#2					@tmp = *p2++
	smlabb		r7,r8,r11,r7				@L_temp2 = L_multbb(temp,tmp)
	smlatb		r6,r8,r11,r6				@L_temp1 = L_mactb(L_temp11,temp,tmp)
	smlatb		r7,r8,r10,r7				@L_temp2 = L_mactb(L_temp21,temp,tmp)
	smlabb		r6,r9,r10,r6				@L_temp1 = L_mactb(L_temp11,temp,tmp)
	qadd		r6,r6,r6	
	qadd		r7,r7,r7
	qdadd		r8,r12,r6					@L_sum1 = L_dadd_sat(_round,L_temp11)@
	qdadd		r9,r12,r7					@L_sum2 = L_dadd_sat(_round,L_temp21)@
	ldrh		r10,[r3],#2					@tmp = *sign++
	ldrh		r11,[r3],#6					@tmp = *sign++
	ldrh		r6,[r4],#32					@tmp = *p0++
	ldrh		r7,[r4],#-30				@tmp = *p0++
	smultb		r8,r8,r10					@L_multtb_sat(L_sum1, (Word32)*sign)
	smultb		r9,r9,r11					@L_multtb_sat(L_sum2, (Word32)*sign)
	qadd		r8,r8,r8
	qadd		r9,r9,r9
	mov		r6,r6,lsl #16	
	mov		r7,r7,lsl #16	
	qadd		r8,r8,r6
	qadd		r9,r9,r7
	mov		r8,r8,asr #16	
	mov		r9,r9,asr #16	
	strh		r8,[r5],#32
	strh		r9,[r5],#-30

	ldmfd		r13!,{r4 -r12,pc}


_cor_h_vec_2_asm:

	stmfd		r13!,{r4 -r12,r14}	
	ldr		r7,[r13,#40]		
	ldr		r5,[r13,#44]		
	str		r0,[r13,#40]		
	str		r1,[r13,#44]		
	add		r3,r3,r2,lsl #1		
	mov		r14,#15
	ldr		r4,[r7,r2,lsl #2]	
	
vec_outer2:	
	ldr		r0,[r13,#40]		
	ldr		r1,[r13,#44]		
	rsb		r12,r2,#63		
	mov		r12,r12,asr #2		
	add		r1,r1,r2,lsl #1		
        mrs 		r11,CPSR@
        bic		r11,r11,#0x08000000
        msr 		CPSR_f,r11@
	sub		r6,r6,r6		
	sub		r7,r7,r7		
	ldrh		r10,[r1],#2		
	ldrh		r11,[r1],#2		

vec_inner2:	
	ldr		r8,[r0],#4		
	ldr		r9,[r0],#4		
	smlabb		r6,r8,r10,r6		
	ldrh		r10,[r1],#2		
	smlabb		r7,r8,r11,r7		
	smlatb		r6,r8,r11,r6		
	ldrh		r11,[r1],#2		
	smlatb		r7,r8,r10,r7		
	smlabb		r6,r9,r10,r6		
	ldrh		r10,[r1],#2		
	smlabb		r7,r9,r11,r7		
	smlatb		r6,r9,r11,r6		
	ldrh		r11,[r1],#2		
	smlatb		r7,r9,r10,r7		
	subs		r12,r12,#1		
	bne		vec_inner2

	ldr		r8,[r0],#4		
  	smlabb		r6,r8,r10,r6		
	smlabb		r7,r8,r11,r7		
	smlatb		r6,r8,r11,r6		
	mrs		r11,CPSR@		
    	mov		r11,r11,asr #27
   	and		r11,r11,#1
	cmp		r11,#1
	mvneq		r6,#0x80000000
	mvneq		r7,#0x80000000
	mov		r12,#32768			
	qadd		r6,r6,r6	
	qadd		r7,r7,r7
	qdadd		r8,r12,r6			
	qdadd		r9,r12,r7			
	ldrh		r10,[r3],#2			
	ldrh		r11,[r3],#6			
	subs		r14,r14,#1			
	ldrh		r6,[r4],#32			
	ldrh		r7,[r4],#-30			
	smultb		r8,r8,r10			
	smultb		r9,r9,r11			
	qadd		r8,r8,r8
	qadd		r9,r9,r9
	mov		r6,r6,lsl #16	
	mov		r7,r7,lsl #16	
	qadd		r8,r8,r6
	qadd		r9,r9,r7
	mov		r8,r8,asr #16	
	mov		r9,r9,asr #16	
	strh		r8,[r5],#32
	strh		r9,[r5],#-30
	add		r2,r2,#4
	bne		vec_outer2

	ldr		r0,[r13,#40]			
	ldr		r1,[r13,#44]			
	sub		r6,r6,r6			
	add		r1,r1,r2,lsl #1			
	sub		r7,r7,r7			
	ldrh		r10,[r1],#2			
	ldrh		r11,[r1],#2			
	ldr		r8,[r0],#4			
	smlabb		r6,r8,r10,r6			
	smlabb		r7,r8,r11,r7			
	smlatb		r6,r8,r11,r6			
	qadd		r6,r6,r6	
	qadd		r7,r7,r7
	qdadd		r8,r12,r6			
	qdadd		r9,r12,r7			
	ldrh		r10,[r3],#2			
	ldrh		r11,[r3],#6			
	ldrh		r6,[r4],#32			
	ldrh		r7,[r4],#-30			
	smultb		r8,r8,r10			
	smultb		r9,r9,r11			
	qadd		r8,r8,r8
	qadd		r9,r9,r9
	mov		r6,r6,lsl #16	
	mov		r7,r7,lsl #16	
	qadd		r8,r8,r6
	qadd		r9,r9,r7
	mov		r8,r8,asr #16	
	mov		r9,r9,asr #16	
	strh		r8,[r5],#32
	strh		r9,[r5],#-30
	
	ldmfd		r13!,{r4 -r12,pc}	

_cor_h_vec_3_asm:

	stmfd		r13!,{r4 -r12,r14}		
	ldr		r7,[r13,#40]			
	ldr		r5,[r13,#44]			
	str		r0,[r13,#40]			
	str		r1,[r13,#44]			
	mov		r14,#15
	ldr		r4,[r7]	
			
vec_outer3:	
	ldr		r0,[r13,#40]			
	ldr		r1,[r13,#44]			
	rsb		r12,r2,#64			
	mov		r12,r12,asr #2			
	add		r1,r1,r2,lsl #1			
        mrs 		r11,CPSR			
        bic		r11,r11,#0x08000000
        msr 		CPSR_f,r11			
	sub		r6,r6,r6			
	sub		r7,r7,r7			
	ldrh		r10,[r1],#-6			
	ldrh		r11,[r1],#8			

vec_inner3:		
	ldr		r8,[r0],#4			
	ldr		r9,[r0],#4			
	smlabb		r6,r8,r10,r6			
	ldrh		r10,[r1],#-6			
	smlabb		r7,r8,r11,r7			
	ldrh		r11,[r1],#8			
	smlatb		r6,r8,r10,r6			
	ldrh		r10,[r1],#-6			
	smlatb		r7,r8,r11,r7			
	ldrh		r11,[r1],#8			
	smlabb		r6,r9,r10,r6			
	ldrh		r10,[r1],#-6			
	smlabb		r7,r9,r11,r7			
	ldrh		r11,[r1],#8			
	smlatb		r6,r9,r10,r6			
	ldrh		r10,[r1],#-6			
	smlatb		r7,r9,r11,r7			
	ldrh		r11,[r1],#8			
	subs		r12,r12,#1			
	bne		vec_inner3

	sub		r1,r1,#6
	ldr		r8,[r0],#4			
	ldr		r9,[r0],#4			
	smlabb		r6,r8,r10,r6			
	ldrh		r10,[r1],#2			
	smlabb		r7,r8,r11,r7			
	ldrh		r11,[r1],#2			
	smlatb		r7,r8,r10,r7			
	ldrh		r10,[r1],#2			
	smlabb		r7,r9,r11,r7			
	smlatb		r7,r9,r10,r7			
     	mrs		r11,CPSR@
        mov		r11,r11,asr #27
        and		r11,r11,#1
	cmp		r11,#1
	mvneq		r6,#0x80000000
	mvneq		r7,#0x80000000
	mov		r12,#32768			
	qadd		r6,r6,r6	
	qadd		r7,r7,r7
	qdadd		r8,r12,r6			
	qdadd		r9,r12,r7			
	ldrh		r11,[r3],#6			
	ldrh		r10,[r3],#2			
	subs		r14,r14,#1			
	ldrh		r7,[r4],#96			
	ldrh		r6,[r4],#-94			
	smultb		r8,r8,r10			
	smultb		r9,r9,r11			
	qadd		r8,r8,r8
	qadd		r9,r9,r9
	mov		r6,r6,lsl #16	
	mov		r7,r7,lsl #16	
	qadd		r8,r8,r6
	qadd		r9,r9,r7
	mov		r8,r8,asr #16	
	mov		r9,r9,asr #16	
	strh		r8,[r5],#32
	strh		r9,[r5],#-30
	add		r2,r2,#4
	bne		vec_outer3
					
	ldr		r0,[r13,#40]			
	ldr		r1,[r13,#44]			
	sub		r6,r6,r6			
	add		r1,r1,r2,lsl #1			
	sub		r7,r7,r7			
	ldrh		r10,[r1],#-6
	ldrh		r11,[r1],#2
	ldr		r8,[r0],#4			
	ldr		r9,[r0],#4			
	smlabb		r6,r8,r10,r6			
	ldrh		r10,[r1],#2			
	smlabb		r7,r8,r11,r7			
	ldrh		r11,[r1],#2			
	smlatb		r7,r8,r10,r7			
	ldrh		r10,[r1],#2			
	smlabb		r7,r9,r11,r7			
	smlatb		r7,r9,r10,r7			
	qadd		r6,r6,r6	
	qadd		r7,r7,r7
	qdadd		r8,r12,r6			
	qdadd		r9,r12,r7			
	ldrh		r11,[r3],#6			
	ldrh		r10,[r3],#2			
	ldrh		r7,[r4],#96			
	ldrh		r6,[r4],#-94			
	smultb		r8,r8,r10			
	smultb		r9,r9,r11			
	qadd		r8,r8,r8
	qadd		r9,r9,r9
	mov		r6,r6,lsl #16	
	mov		r7,r7,lsl #16	
	qadd		r8,r8,r6
	qadd		r9,r9,r7
	mov		r8,r8,asr #16	
	mov		r9,r9,asr #16	
	strh		r8,[r5],#32
	strh		r9,[r5],#-30
	
	ldmfd		r13!,{r4 -r12,pc}


_search_ixiy_asm:
	stmfd		r13!, {r0 -r12,r14}			
	ldr		r6, [r13, #68]				
	cmp		r6, #3
	subeq		r7, r7, r7				
	addne		r7, r6, #1				
	add		r5, r3, r6, lsl #9				
	add		r4, r2, #32				
	mov		r3, r2					
	add		r2, r1, r6, lsl #1				
	add		r1, r0, r6, lsl #1				
	add		r0, r0, r7, lsl #1				
	orr		r6, r6, r7, lsl #16			
	ldr		r7, [r13, #76]				
	str		r2, [r13, #8]				
	str		r1, [r13, #12]				
	ldrsh		r7, [r7]					
	str		r6, [r13, #68]				
	mov		r7, r7, lsl #16				
	add		r7, r7,#0x8000				
	str		r7, [r13, #0]				
	ldr		r7, [r13, #72]				
	ldrsh		r7, [r7]					
	ldr		r12, [r13, #64]				
	sub		r12,r12,#8				
	str		r7,[r13,#4]				
	mvn		r8,#0x80000000				
	mov		r8,r8,asr #14				
	str		r12,[r13,#64]				
	mov		r6,#16		
	orr		r14,r6,r6,lsl #16			
		
stable1:
	ldr		r1,[r13,#12]				
	ldr		r2,[r13,#8]				
	ldr		r9,[r13,#0]				
	ldr		r10,[r13,#4]				
	ldrsh		r11,[r1],#8				
	add		r10,r10,r11				
	ldrsh		r11,[r3],#2				
	mov		r11,r11,lsl #13				
	qadd		r9,r9,r11				
	ldrsh		r11,[r2],#8				
	ldr		r12,[r13,#64]				
	str		r1,[r13,#12]				
	str		r2,[r13,#8]				
	cmp		r11,r12
	addge		r5,r5,#32				
	bge		stable3
	mov		r7,#16
					
stable2:
	ldrsh		r11,[r0],#8				
	ldrsh		r12,[r4],#2				
	ldrsh		r2,[r5],#2				
	add		r11,r10,r11				
	mov		r12,r12,lsl #13				
	qadd		r12,r9,r12				
	mov		r2,r2,lsl #14				
	smulbb		r1,r11,r11				
	qadd		r12,r12,r2				
	mov		r11,r1,asr #15				
	smultb		r2,r8,r11				
	smulbt		r1,r8,r12				
	mov		r12,r12,asr #16				
	cmp		r2,r1					
	orrgt		r8,r11,r12,lsl #16		
	orrgt		r14,r6,r7,lsl #16		
	subs		r7,r7,#1			
	bne		stable2

	sub		r4,r4,#32			
	sub		r0,r0,#128			
stable3:	
	subs		r6,r6,#1			
	bne		stable1
	mov		r11,#16
	ldr		r1,[r13,#68]			
	ldr		r2,[r13,#72]			
	ldr		r3,[r13,#76]			
	ldr		r4,[r13,#56]			
	ldr		r5,[r13,#60]			
	sub		r0,r0,r1,asr #15		
	orr		r7,r11,r11,lsl #16		
	sub		r14,r7,r14
	add		r14,r1,r14,lsl #2		
	mov		r7,r14,asr #16			
	mov		r14,r14,lsl #16
	mov		r6,r14,asr #16			
	mov		r12,r6,lsl #1
	mov		r14,r7,lsl #1	
	ldrsh		r10,[r0,r12]			
	ldrsh		r11,[r0,r14]			
	strh		r6,[r4]				
	strh		r7,[r5]				
	mov		r6,r8,asr #16			
	ldrsh		r7,[r2]				
	add		r10,r10,r11			
	add		r7,r7,r10				
	strh		r6,[r3]					
	strh		r7,[r2]						
	ldmfd		r13!,{r0 -r12,pc}



_Syn_filt_opt:

	stmfd		r13!,{r4-r12,r14}
	mov		r6,r3
	mov		r7,#0x8000	 
	mov		r4,#65280	 
	orr		r4,r4,r4,asr #8	

	mov		r3,#0
	
	ldrh		r9, [r0], #2			
	ldrh		r10, [r0], #2			
	sub		r10, r3, r10			
	and		r10, r10, r4
	orr		r9, r10, r9, lsl #15
	
	ldrh		r10, [r0], #2			
	ldrh		r11, [r0], #2			
	sub		r10, r3, r10			
	sub		r11, r3, r11
	and		r11, r11, r4
	orr		r10, r11, r10, lsl #16
	
	ldrh		r11, [r0], #2			
	ldrh		r12, [r0], #2			
	sub		r11, r3, r11			
	sub		r12, r3, r12
	and		r12, r12, r4
	orr		r11, r12, r11, lsl #16

	ldrh		r12, [r0], #2			
	ldrh		r14, [r0], #2			
	sub		r12, r3, r12			
	sub		r14, r3, r14
	and		r14, r14, r4
	orr		r12, r14, r12, lsl #16
	
	stmfd		r13!,{r9-r12}			
	
	ldrh		r11, [r0], #2			
	ldrh		r12, [r0], #2			
	sub		r11, r3, r11			
	sub		r12, r3, r12
	and		r12, r12, r4
	orr		r11, r12, r11, lsl #16
	
	ldrh		r12, [r0], #2			
	ldrh		r14, [r0], #2			
	sub		r12, r3, r12			
	sub		r14, r3, r14
	and		r14, r14, r4
	orr		r12, r14, r12, lsl #16
	
	stmfd		r13!,{r11,r12}			

	ldrh		r11, [r0], #2			
	ldrh		r12, [r0], #2			
	sub		r11, r3, r11			
	sub		r12, r3, r12
	and		r12, r12, r4
	orr		r11, r12, r11, lsl #16

	ldrh		r12, [r0], #2			
	ldrh		r14, [r0], #2			
	sub		r12, r3, r12			
	sub		r14, r3, r14
	and		r14, r14, r4
	orr		r12, r14, r12, lsl #16
	
	stmfd		r13!,{r11,r12}			

	
	ldrh		r14,[r0],#2			
	sub		r14,r3,r14			
	and		r14,r14,r4			
	mov		r2,r2,asr #2			
	orr		r14,r14,r2,lsl #16
	
	ldr		r4,[r13,#72]			
	ldr		r0,[r13,#76]			

	ldrh		r5,[r4],#2			
	strh		r5,[r0],#2				
	ldrh		r5,[r4],#2				
	strh		r5,[r0],#2				
	ldrh		r5,[r4],#2				
	strh		r5,[r0],#2				
	ldrh		r5,[r4],#2				
	strh		r5,[r0],#2				
	
	ldrh		r5,[r4],#2				
	strh		r5,[r0],#2				
	ldrh		r5,[r4],#2				
	strh		r5,[r0],#2				
	ldrh		r5,[r4],#2				
	strh		r5,[r0],#2				
	ldrh		r5,[r4],#2				
	strh		r5,[r0],#2				

	ldrh		r5,[r4],#2				
	strh		r5,[r0],#2				
	ldrh		r5,[r4],#2				
	strh		r5,[r0],#2				
	ldrh		r5,[r4],#2				
	strh		r5,[r0],#2				
	ldrh		r5,[r4],#2				
	strh		r5,[r0],#2				

	ldrh		r5,[r4],#2				
	strh		r5,[r0],#2			
	ldrh		r5,[r4],#2			
	strh		r5,[r0],#2			
	ldrh		r5,[r4],#2			
	strh		r5,[r0],#2			
	ldrh		r5,[r4],#2			
	strh		r5,[r0],#-2			
outer_loop:
	ldr		r11,[r13,#24]			
	ldr		r12,[r13,#28]			
	ldrh		r3,[r1],#2			
	smultb		r4,r9,r3			
	ldrh		r3,[r1],#2			
	smultb		r5,r9,r3			
	ldrh		r3,[r1],#2			
	smultb		r2,r9,r3			
	ldrh		r3,[r1],#2			
	smultb		r8,r9,r3			
	
	ldr		r3,[r0],#-4
	smlabt		r4,r9,r3,r4			
	smlatt		r5,r10,r3,r5			
	smlabt		r2,r10,r3,r2			
	smlatt		r8,r11,r3,r8			
	smlatb		r4,r10,r3,r4			
	smlabb		r5,r10,r3,r5			
	smlatb		r2,r11,r3,r2			
	smlabb		r8,r11,r3,r8			

	ldr		r3,[r0],#-4
	smlabt		r4,r10,r3,r4			
	smlatt		r5,r11,r3,r5			
	smlabt		r2,r11,r3,r2			
	smlatt		r8,r12,r3,r8			
	smlatb		r4,r11,r3,r4			
	smlabb		r5,r11,r3,r5			
	smlatb		r2,r12,r3,r2			
	smlabb		r8,r12,r3,r8			

	ldr		r3,[r0],#-4
	smlabt		r4,r11,r3,r4			
	smlatt		r5,r12,r3,r5			
	ldr		r11,	[r13,#8]		
	smlabt		r2,r12,r3,r2			
	smlatt		r8,r11,r3,r8			
	smlatb		r4,r12,r3,r4			
	smlabb		r5,r12,r3,r5			
	smlatb		r2,r11,r3,r2			
	smlabb		r8,r11,r3,r8			

	ldr		r3,[r0],#-4
	smlabt		r4,r12,r3,r4			
	ldr		r12,[r13,#12]			
	smlatt		r5,r11,r3,r5			
	smlabt		r2,r11,r3,r2			
	smlatt		r8,r12,r3,r8			
	smlatb		r4,r11,r3,r4			
	smlabb		r5,r11,r3,r5			
	smlatb		r2,r12,r3,r2			
	smlabb		r8,r12,r3,r8			

	ldr		r3,[r0],#-4
	smlabt		r4,r11,r3,r4			
	smlatt		r5,r12,r3,r5			
	ldr		r11,[r13,#0]			
	smlabt		r2,r12,r3,r2			
	smlatt		r8,r11,r3,r8			
	smlatb		r4,r12,r3,r4			
	smlabb		r5,r12,r3,r5			
	smlatb		r2,r11,r3,r2			
	smlabb		r8,r11,r3,r8			

	ldr		r3,[r0],#-4
	smlabt		r4,r12,r3,r4			
	smlatt		r5,r11,r3,r5			
	ldr		r12,[r13,#4]			
	smlabt		r2,r11,r3,r2			
	smlatt		r8,r12,r3,r8			
	smlatb		r4,r11,r3,r4			
	smlabb		r5,r11,r3,r5			
	smlatb		r2,r12,r3,r2			
	smlabb		r8,r12,r3,r8			

	ldr		r3,[r0],#-4
	smlabt		r4,r11,r3,r4			
	smlatt		r5,r12,r3,r5			
	smlabt		r2,r12,r3,r2			
	smlabt		r8,r14,r3,r8			
	smlatb		r4,r12,r3,r4			
	smlabb		r5,r12,r3,r5			
	smlabb		r2,r14,r3,r2			

	ldr		r3,[r0],#32
	smlabt		r4,r12,r3,r4			
	smlabt		r5,r14,r3,r5			
	smlabb		r4,r14,r3,r4			
	
	qadd		r4,r4,r4
	qadd		r4,r4,r4
	qadd		r4,r4,r4
	qdadd		r4,r7,r4
	mov		r4,r4,asr #16			

	smlabb		r5,r9,r4,r5			
	smlatb		r2,r10,r4,r2			
	smlabb		r8,r10,r4,r8			
	qadd		r5,r5,r5
	qadd		r5,r5,r5
	qadd		r5,r5,r5
	qdadd		r5,r7,r5
	mov		r5,r5,asr #16			
	
	smlabb		r2,r9,r5,r2			
	smlatb		r8,r10,r5,r8			
	qadd		r2,r2,r2
	qadd		r2,r2,r2
	qadd		r2,r2,r2
	qdadd		r2,r7,r2
	mov		r2,r2,asr #16			
	
	smlabb		r8,r9,r2,r8			
	qadd		r8,r8,r8
	qadd		r8,r8,r8
	qadd		r8,r8,r8
	qdadd		r8,r7,r8
	mov		r8,r8,asr #16			
	
	strh		r4,[r0], #2			
	strh		r4,[r6], #2			
	strh		r5,[r0], #2			
	strh		r5,[r6], #2			
	strh		r2,[r0], #2			
	strh		r2,[r6], #2			
	strh		r8,[r0], #-2			
	strh		r8,[r6], #2			

	mov 		r2, r14, asr #16
	sub		r14, r14, #0x10000		
	subs		r2,r2,#1
	bne		outer_loop

	mov 		r6,	r0			
	ldr		r0,[r13,#80]			
	
	subs		r0,r0,#1
	
	bne		Lable

	ldr		r5,[r13,#72]			
	sub		r6,r6,#28			

	ldrh		r3,[r6],#2
	strh		r3,[r5],#2
	ldrh		r3,[r6],#2
	strh		r3,[r5],#2
	ldrh		r3,[r6],#2
	strh		r3,[r5],#2
	ldrh		r3,[r6],#2
	strh		r3,[r5],#2

	ldrh		r3,[r6],#2
	strh		r3,[r5],#2
	ldrh		r3,[r6],#2
	strh		r3,[r5],#2
	ldrh		r3,[r6],#2
	strh		r3,[r5],#2
	ldrh		r3,[r6],#2
	strh		r3,[r5],#2

	ldrh		r3,[r6],#2
	strh		r3,[r5],#2
	ldrh		r3,[r6],#2
	strh		r3,[r5],#2
	ldrh		r3,[r6],#2
	strh		r3,[r5],#2
	ldrh		r3,[r6],#2
	strh		r3,[r5],#2

	ldrh		r3,[r6],#2
	strh		r3,[r5],#2
	ldrh		r3,[r6],#2
	strh		r3,[r5],#2
	ldrh		r3,[r6],#2
	strh		r3,[r5],#2
	ldrh		r3,[r6],#2
	strh		r3,[r5],#2

Lable:	
 	ldmfd		r13!,{r5-r12}
	ldmfd		r13!,{r4-r12,pc}	

        @.END	  

