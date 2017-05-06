@*****************************************************************************
@*																			*
@*		VisualOn, Inc. Confidential and Proprietary, 2010					*
@*																			*
@*****************************************************************************

	#include "../../voASMPort.h"
	
	@AREA	|.text|, CODE
	.section  .text
	
	.global	ARMV6_pred16x16_plane
	
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@	
@static void pred16x16_plane_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)        @
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@     
ARMV6_pred16x16_plane : @PROC
        stmfd    sp!,{r4-r12,lr}
        sub		 sp,sp,#0x20
				mov		 r10,sp			@topp
				add		 r4,sp,#0x10	@leftp

				sub		 r6,r0,r2		@(src-src_stride)
				ldrb	 r5,[r6,#-1]	@t0
	
				ldr		 r7,[r6,#0]
				ldr		 r8,[r6,#4]
				str		 r7,[r10,#0]	@topp[0]
				str		 r8,[r10,#4]	@topp[1]
				ldr		 r7,[r6,#8]
				ldr		 r8,[r6,#12]
				str		 r7,[r10,#8]	@topp[2]
				str		 r8,[r10,#12]	@topp[3]
		
				sub		 r6,r0,#1  		@pp = src - 1@
		
				mov		 r12,#4
loop1:	ldrb	 r7,[r6],r2
				ldrb	 r8,[r6],r2
				ldrb	 r9,[r6],r2
				ldrb	 lr,[r6],r2
				orr		 r7,r7,r8,lsl #8
				orr		 r7,r7,r9,lsl #16
				orr		 r7,r7,lr,lsl #24
				str		 r7,[r4],#4
				subs	 r12,r12,#1
				bgt		 loop1
								
		
				ldrb	 r4,[r10,#8]		@r12=iH,r3=tp
				ldrb	 r6,[r10,#6]
				ldrb	 r7,[r10,#9]
				ldrb	 r8,[r10,#5]
				sub		 r12,r4,r6
				sub		 r9,r7,r8
				add		 r12,r12,r9,lsl #1

				ldrb	 r4,[r10,#10]
				ldrb	 r6,[r10,#4]
				ldrb	 r7,[r10,#11]
				ldrb	 r8,[r10,#3]
				sub		 r9,r4,r6
				add		 r9,r9,r9,lsl #1
				add		 r12,r12,r9
				sub		 r9,r7,r8
				add		 r12,r12,r9,lsl #2

				ldrb	 r4,[r10,#12]
				ldrb	 r6,[r10,#2]
				ldrb	 r7,[r10,#13]
				ldrb	 r8,[r10,#1]
				sub		 r9,r4,r6
				add		 r9,r9,r9,lsl #2
				add		 r12,r12,r9
				sub		 r9,r7,r8
				add		 r8,r9,r9,lsl #2
				add		 r9,r9,r8
				add		 r12,r12,r9

				ldrb	 r4,[r10,#14]
				ldrb	 r6,[r10,#0]
				ldrb	 r7,[r10,#15]
				sub		 r9,r4,r6
				rsb		 r9,r9,r9,lsl #3
				add		 r12,r12,r9
				sub		 r9,r7,r5
				add		 r12,r12,r9,lsl #3


				add		 r10,sp,#0x10
				ldrb	 r4,[r10,#8]		@lr=iV,r10=lp
				ldrb	 r6,[r10,#6]
				ldrb	 r7,[r10,#9]
				ldrb	 r8,[r10,#5]
				sub		 lr,r4,r6
				sub		 r9,r7,r8
				add		 lr,lr,r9,lsl #1

				ldrb	 r4,[r10,#10]
				ldrb	 r6,[r10,#4]
				ldrb	 r7,[r10,#11]
				ldrb	 r8,[r10,#3]
				sub		 r9,r4,r6
				add		 r9,r9,r9,lsl #1
				add		 lr,lr,r9
				sub		 r9,r7,r8
				add		 lr,lr,r9,lsl #2

				ldrb	 r4,[r10,#12]
				ldrb	 r6,[r10,#2]
				ldrb	 r7,[r10,#13]
				ldrb	 r8,[r10,#1]
				sub		 r9,r4,r6
				add		 r9,r9,r9,lsl #2
				add		 lr,lr,r9
				sub		 r9,r7,r8
				add		 r8,r9,r9,lsl #2
				add		 r9,r9,r8
				add		 lr,lr,r9

				ldrb	 r4,[r10,#14]
				ldrb	 r6,[r10,#0]
				ldrb	 r7,[r10,#15]		
				sub		 r9,r4,r6
				rsb		 r9,r9,r9,lsl #3
				add		 lr,lr,r9
				sub		 r9,r7,r5
				add		 lr,lr,r9,lsl #3

				ldrb	 r8,[sp,#15]		@tp[15]
				add		 r10,r12,r12,asr #2	
				mov		 r10,r10,asr #4		@b
				add		 r4,lr,lr,asr #2
				mov		 r4,r4,asr #4		@c

				add		 r8,r8,r7			@a
				add		 r5,r10,r4
				rsb		 r5,r5,r5,lsl #3
				rsb		 r5,r5,#16
				add		 r5,r5,r8,lsl #4	@lTemp1

				mov		 r12,#16
loop2:	mov		 r7,r5				@temp
				mov		 lr,#16
loop3:	usat   r8,#8,r7,asr #5
		
				add		 r7,r7,r10
				usat   r9,#8,r7,asr #5

				add		 r7,r7,r10
				usat   r0,#8,r7,asr #5
				orr		 r8,r8,r9,lsl #8

				add		 r7,r7,r10
				usat   r9,#8,r7,asr #5
				orr		 r8,r8,r0,lsl #16
				subs	 lr,lr,#4
				orr		 r8,r8,r9,lsl #24
		
				str		 r8,[r1],#4	
				add		 r7,r7,r10
				bgt		 loop3
				add		 r5,r5,r4
				subs	 r12,r12,#1
				bgt		 loop2

        add      sp,sp,#0x20
        ldmfd    sp!,{r4-r12,pc}          
        
       @ENDP