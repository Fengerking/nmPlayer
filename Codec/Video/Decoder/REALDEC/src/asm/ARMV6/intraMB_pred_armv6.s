;*****************************************************************************
;*																			*
;*		VisualOn, Inc. Confidential and Proprietary, 2010					*
;*																			*
;*****************************************************************************

	AREA	|.text|, CODE
	
	EXPORT	ARMV6_pred16x16_plane
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
;static void pred16x16_plane_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)        ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;     
ARMV6_pred16x16_plane PROC
        STMFD    sp!,{r4-r12,lr}
		SUB		 sp,sp,#0x20
		MOV		 r10,sp			;topp
		ADD		 r4,sp,#0x10	;leftp

		SUB		 r6,r0,r2		;(src-src_stride)
		LDRB	 r5,[r6,#-1]	;t0
	
		LDR		 r7,[r6,#0]
		LDR		 r8,[r6,#4]
		STR		 r7,[r10,#0]	;topp[0]
		STR		 r8,[r10,#4]	;topp[1]
		LDR		 r7,[r6,#8]
		LDR		 r8,[r6,#12]
		STR		 r7,[r10,#8]	;topp[2]
		STR		 r8,[r10,#12]	;topp[3]
		
		SUB		 r6,r0,#1  		;pp = src - 1;
		
		MOV		 r12,#4
loop1	LDRB	 r7,[r6],r2
		LDRB	 r8,[r6],r2
		LDRB	 r9,[r6],r2
		LDRB	 lr,[r6],r2
		ORR		 r7,r7,r8,LSL #8
		ORR		 r7,r7,r9,LSL #16
		ORR		 r7,r7,lr,LSL #24
		STR		 r7,[r4],#4
		SUBS	 r12,r12,#1
		BGT		 loop1
								
		
		LDRB	 r4,[r10,#8]		;r12=iH,r3=tp
		LDRB	 r6,[r10,#6]
		LDRB	 r7,[r10,#9]
		LDRB	 r8,[r10,#5]
		SUB		 r12,r4,r6
		SUB		 r9,r7,r8
		ADD		 r12,r12,r9,LSL #1

		LDRB	 r4,[r10,#10]
		LDRB	 r6,[r10,#4]
		LDRB	 r7,[r10,#11]
		LDRB	 r8,[r10,#3]
		SUB		 r9,r4,r6
		ADD		 r9,r9,r9,LSL #1
		ADD		 r12,r12,r9
		SUB		 r9,r7,r8
		ADD		 r12,r12,r9,LSL #2

		LDRB	 r4,[r10,#12]
		LDRB	 r6,[r10,#2]
		LDRB	 r7,[r10,#13]
		LDRB	 r8,[r10,#1]
		SUB		 r9,r4,r6
		ADD		 r9,r9,r9,LSL #2
		ADD		 r12,r12,r9
		SUB		 r9,r7,r8
		ADD		 r8,r9,r9,LSL #2
		ADD		 r9,r9,r8
		ADD		 r12,r12,r9

		LDRB	 r4,[r10,#14]
		LDRB	 r6,[r10,#0]
		LDRB	 r7,[r10,#15]
		SUB		 r9,r4,r6
		RSB		 r9,r9,r9,LSL #3
		ADD		 r12,r12,r9
		SUB		 r9,r7,r5
		ADD		 r12,r12,r9,LSL #3


		ADD		 r10,sp,#0x10
		LDRB	 r4,[r10,#8]		;lr=iV,r10=lp
		LDRB	 r6,[r10,#6]
		LDRB	 r7,[r10,#9]
		LDRB	 r8,[r10,#5]
		SUB		 lr,r4,r6
		SUB		 r9,r7,r8
		ADD		 lr,lr,r9,LSL #1

		LDRB	 r4,[r10,#10]
		LDRB	 r6,[r10,#4]
		LDRB	 r7,[r10,#11]
		LDRB	 r8,[r10,#3]
		SUB		 r9,r4,r6
		ADD		 r9,r9,r9,LSL #1
		ADD		 lr,lr,r9
		SUB		 r9,r7,r8
		ADD		 lr,lr,r9,LSL #2

		LDRB	 r4,[r10,#12]
		LDRB	 r6,[r10,#2]
		LDRB	 r7,[r10,#13]
		LDRB	 r8,[r10,#1]
		SUB		 r9,r4,r6
		ADD		 r9,r9,r9,LSL #2
		ADD		 lr,lr,r9
		SUB		 r9,r7,r8
		ADD		 r8,r9,r9,LSL #2
		ADD		 r9,r9,r8
		ADD		 lr,lr,r9

		LDRB	 r4,[r10,#14]
		LDRB	 r6,[r10,#0]
		LDRB	 r7,[r10,#15]		
		SUB		 r9,r4,r6
		RSB		 r9,r9,r9,LSL #3
		ADD		 lr,lr,r9
		SUB		 r9,r7,r5
		ADD		 lr,lr,r9,LSL #3

		LDRB	 r8,[sp,#15]		;tp[15]
		ADD		 r10,r12,r12,ASR #2	
		MOV		 r10,r10,ASR #4		;b
		ADD		 r4,lr,lr,ASR #2
		MOV		 r4,r4,ASR #4		;c

		ADD		 r8,r8,r7			;a
		ADD		 r5,r10,r4
		RSB		 r5,r5,r5,LSL #3
		RSB		 r5,r5,#16
		ADD		 r5,r5,r8,LSL #4	;lTemp1

		MOV		 r12,#16
loop2	MOV		 r7,r5				;temp
		MOV		 lr,#16
loop3	USAT     r8,#8,r7,ASR #5
		
		ADD		r7,r7,r10
		USAT    r9,#8,r7,ASR #5

		ADD		r7,r7,r10
		USAT    r0,#8,r7,ASR #5
		ORR		r8,r8,r9,LSL #8

		ADD		r7,r7,r10
		USAT    r9,#8,r7,ASR #5
		ORR		r8,r8,r0,LSL #16
		SUBS	lr,lr,#4
		ORR		r8,r8,r9,LSL #24
		
		STR		r8,[r1],#4	
		ADD		r7,r7,r10
		BGT		loop3
		ADD		r5,r5,r4
		SUBS	r12,r12,#1
		BGT		loop2

        ADD      sp,sp,#0x20
        LDMFD    sp!,{r4-r12,pc}          
        
       ENDP