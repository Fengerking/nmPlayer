@************************************************************************
@									                                    *
@	VisualOn, Inc. Confidential and Proprietary, 2010		            *
@	written by Rock							 	                        *
@***********************************************************************/
	@AREA	|.rdata|, DATA, READONLY
	#include "voYYDef_CC.h"
	.section .rdata
	@.extern	ccClip255	
	.global	cc_yuv420_to_rgb32_mb_s_r90_arm
	@AREA	|.text|, CODE, READONLY
	.section .text


@ccClip255_DATA_r:	.word       ccClip255
	.align 4
ccClip255_DATA_r_0:
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
	.byte  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,   0,   0,   0,   0
ccClip255_DATA_r:
	.byte  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,  14,  15  
	.byte  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31  
	.byte  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47  
	.byte  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63 
	.byte  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79
	.byte  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95 
	.byte  96,  97,  98,  99,  100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111 
	.byte  112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127 
	.byte  128,  129,  130,  131,  132,  133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143
	.byte  144,  145,  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,  157,  158,  159  
	.byte  160,  161,  162,  163,  164,  165,  166,  167,  168,  169,  170,  171,  172,  173,  174,  175  
	.byte  176,  177,  178,  179,  180,  181,  182,  183,  184,  185,  186,  187,  188,  189,  190,  191  
	.byte  192,  193,  194,  195,  196,  197,  198,  199,  200,  201,  202,  203,  204,  205,  206,  207 
	.byte  208,  209,  210,  211,  212,  213,  214,  215,  216,  217,  218,  219,  220,  221,  222,  223  
	.byte  224,  225,  226,  227,  228,  229,  230,  231,  232,  233,  234,  235,  236,  237,  238,  239 
	.byte  240,  241,  242,  243,  244,  245,  246,  247,  248,  249,  250,  251,  252,  253,  254,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255  
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255  
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255  
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255  
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255  
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255  
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255  
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
	.byte  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255 
cc_yuv420_to_rgb32_mb_s_r90_arm: @PROC
		stmdb	sp!, {r4-r11, lr}		@ save regs used
		sub	sp, sp, #60					@ sp - 32
			
		adr	r14, ccClip255_DATA_r
		ldr	r4, [sp, #104]				@ r4 = src_stride
		str	r0,	[sp, #4]				@ [sp, #4] = width
		str	r1,	[sp, #8]				@ [sp, #8] = height						
		str	r2,	[sp, #12]				@ [sp, #12] = src_y

		ldr	r12, [sp, #100]	
		@ldr	r14, [r14]					@ r14 = ccClip255		
		str	r3,	 [sp, #16]				@ [sp, #16] = src_u
		str	r0,	 [sp, #48]					
		str	r14, [sp, #20]				@ [sp, #20] = ccClip255	
		str	r12, [sp, #44]				
RGB32_BIG_WHILE_R:
		ldr	r3, [sp, #116]				@ r3 = y_scale_par	
		ldr	r12,[sp, #112]				@ r12 = x_scale_par		
		ldr	r0, [sp, #12]				@ r0 = src_y
		ldr	r1, [sp, #16]				@ r1 = src_u		
		ldr	r6, [r3, #0]				
		ldr	r2, [sp, #96]				@ r2 = src_v
		ldr	r7, [r3, #12]
		ldr	r14, [sp, #44]		
		mul	r8, r4, r6					@y_scale_par[0] * src_stride				
		add	r5, r3, #24
		mul	r7, r4, r7		
		str	r5, [sp, #116]
		add	r5, r0, r7					@r5 = src_y1		
		add	r0, r0, r8					@r0 = src_y0
		mov	r6, r6, asr #1
		ldr	r7, [sp, #120]			    @uin_stride			
		str	r14, [sp, #100]			
		sub	r14, r14, #8                @dst -= 8@
		mul	r7, r6, r7					@((y_scale_par[0]>>1) * (src_stride>>1))
		str	r14, [sp, #44]			
		
		add	r1, r1, r7					@r1 = src_u0
		ldr	r7, [sp, #124]			    @uin_stride

		str	r1, [sp, #24]				@[sp, #24] = src_u0	
		mul	r7, r6, r7					@((y_scale_par[0]>>1) * (src_stride>>1))				
		add	r2, r2, r7					@r2 = src_v0
		str	r2, [sp, #28]				@[sp, #28] = src_v0	
RGB32_LIT_WHILE_R:								
		ldr	r7, [r12, #12]
		ldr	r6, [r12, #0]
		ldr	r9, =0x645a1				@r8 = ConstU2	
		add	r7, r7, #1
		add	r7, r6, r7

		mov	r7, r7, asr #2				@a1 = (a0 + a6 + 1)>>2@		
		ldrb	r8, [r2, r7]			@a3 = src_v0[a1]@	
		ldrb	r7, [r1, r7]			@a2 = src_u0[a1]@
		
		ldr	r1, =0x198937				@r8 = ConstV1
		ldr	r2, =0xD020C				@r9 = ConstV2
				
		sub	r8, r8, #128				@a3 -= 128@	
		mul	r1, r8, r1					@r1 = a0 = (a3 * ConstV1)@
		sub	r7, r7, #128				@a2 -= 128@		
		mul	r2, r8, r2					@r2 = a3 * ConstV2@
		
		ldr	r8, =0x2045a1				@r9 = ConstU1		
		mul	r9, r7, r9					@a2 *ConstU2
		str	r1, [sp, #32]				@a0 = [sp, #32]			
		add	r2, r2, r9					@r2 = a1 = (a3 * ConstV2 + a2 *ConstU2)@
		mul	r8, r7, r8					@r9 = a2 = (a2 * ConstU1)@	
		str	r2, [sp, #36]				@a1 = [sp, #36]			
		str	r8, [sp, #40]				@a2 = [sp, #40]
				
		ldr	r10, [r12, #4]				@r10 = aa				
		ldr	r11, [r12, #8]				@r11 = bb
		
		add	r8, r0, r6		
		ldrb	r1, [r8]				@src_y0[a6]
		ldrb	r7, [r8, r4]			@src_y0[a6+src_stride]			
		ldrb	r2, [r8,#1]!			@src_y0[a6+1]	
		ldrb	r9, [r8, r4]			@src_y0[a6+src_stride+1]
		
		mul	r1, r11, r1					@src_y0[a6]*bb
		add	r8, r5, r6		
		mla	r1, r2, r10, r1				@r1 = (src_y0[a6]*bb + src_y0[a6+1]*aa)
		ldrb	r2, [r8]				@src_y1[a6]		
		mul	r6, r10, r9					@src_y0[a6+src_stride+1]*aa		
		ldrb	r9, [r8, r4]			@src_y1[a6+src_stride]		
		ldrb	r14, [r8,#1]!			@src_y1[a6+1]		
		mla	r6, r7, r11, r6				@r6 = (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)
		ldrb	r7, [r8,r4]				@src_y1[a6+src_stride+1]		
		
		mul	r2, r11, r2					@src_y1[a6]*bb	
		ldr	r8, [r3, #4]				@aa0		
		mla	r2, r14, r10, r2			@r2 = (src_y1[a6]*bb + src_y1[a6+1]*aa)	
		ldr	r14, [r3, #8]				@bb0		
		mul	r7, r10, r7					@src_y1[a6+src_stride+1]*aa
		ldr	r10, [r3, #20]				@bb1		
		mla	r7, r9, r11, r7				@r7 = (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)
		ldr	r11, [r3, #16]				@aa1
		
		mul	r1, r14, r1	
		ldr	r9, =0x129fbe				@r8 = ConstY			
		mla	r1, r8, r6, r1				@((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)
		
		ldr	r14, [sp, #20]				@ [sp, #20] = ccClip63		
		mul	r2, r10, r2
		mov	r1, r1, asr #20		
		mla	r2, r7, r11, r2				@((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)

		sub	r1, r1, #16
		mul	r1, r9, r1
		
		mov	r2, r2, asr #20		
		sub	r2, r2, #16
		mul	r2, r9, r2

		ldr	r6, [sp, #32]				@a0 = [sp, #32]
		ldr	r7, [sp, #36]				@a1 = [sp, #36]			
		ldr	r8, [sp, #40]				@a2 = [sp, #40]	
		
		add	r9, r1, r6		
		sub	r10, r1, r7
		add	r11, r1, r8
		
		ldrb	r9,  [r14, r9,  asr #20]		
		ldrb	r10, [r14, r10, asr #20]	
		ldrb	r11, [r14, r11, asr #20]						
		
		mov	r9, r9, lsl #16
		orr	r9, r9, r10, lsl #8
		orr	r1, r9, r11					@a4 = ((ccClip255[(a4 + a0)>>20])<<16)|((ccClip255[(a4 - a1)>>20])<<8)|(ccClip255[(a4 + a2)>>20])@
		
		add	r9, r2, r6		
		sub	r10, r2, r7
		add	r11, r2, r8
		
		ldrb	r9, [r14, r9, asr #20]		
		ldrb	r10, [r14, r10, asr #20]	
		ldrb	r11, [r14, r11, asr #20]	
				
		mov	r9, r9, lsl #16
		orr	r9, r9, r10, lsl #8
		orr	r2, r9, r11					@a7 = ((ccClip255[(a5 + a0)>>20])<<16)|((ccClip255[(a5 - a1)>>20])<<8)|(ccClip255[(a5 + a2)>>20])@											

		ldr	r9, [sp, #100]
		ldr	r10,[sp, #108]
		str r1, [r9, #4]
		str r2, [r9], r10
		str	r9, [sp, #100]

		ldr	r6, [r12, #12]				@r9 = a6		
		ldr	r10, [r12, #16]				@r10 = aa				
		ldr	r11, [r12, #20]				@r11 = bb
		
		add	r8, r0, r6		
		ldrb	r1, [r8]				@src_y0[a6]
		ldrb	r7, [r8, r4]			@src_y0[a6+src_stride]			
		ldrb	r2, [r8,#1]!			@src_y0[a6+1]	
		ldrb	r9, [r8, r4]			@src_y0[a6+src_stride+1]
		
		mul	r1, r11, r1					@src_y0[a6]*bb
		add	r8, r5, r6		
		mla	r1, r2, r10, r1				@r1 = (src_y0[a6]*bb + src_y0[a6+1]*aa)
		ldrb	r2, [r8]				@src_y1[a6]		
		mul	r6, r10, r9					@src_y0[a6+src_stride+1]*aa		
		ldrb	r9, [r8, r4]			@src_y1[a6+src_stride]		
		ldrb	r14, [r8,#1]!			@src_y1[a6+1]		
		mla	r6, r7, r11, r6				@r6 = (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)
		ldrb	r7, [r8,r4]				@src_y1[a6+src_stride+1]		
		
		mul	r2, r11, r2					@src_y1[a6]*bb	
		ldr	r8, [r3, #4]				@aa0		
		mla	r2, r14, r10, r2			@r2 = (src_y1[a6]*bb + src_y1[a6+1]*aa)	
		ldr	r14, [r3, #8]				@bb0		
		mul	r7, r10, r7					@src_y1[a6+src_stride+1]*aa
		ldr	r10, [r3, #20]				@bb1		
		mla	r7, r9, r11, r7				@r7 = (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)
		ldr	r11, [r3, #16]				@aa1
		
		mul	r1, r14, r1	
		ldr	r9, =0x129fbe				@r8 = ConstY			
		mla	r1, r8, r6, r1				@((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)
		
		ldr	r14, [sp, #20]				@[sp, #20] = ccClip63		
		mul	r2, r10, r2
		mov	r1, r1, asr #20		
		mla	r2, r7, r11, r2				@((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)

		sub	r1, r1, #16
		mul	r1, r9, r1
		
		mov	r2, r2, asr #20		
		sub	r2, r2, #16
		mul	r2, r9, r2
				
										@a4 = ((ccClip255[(((a4 + a0)>>20))])<<16)|((ccClip255[(((a4 - a1)>>20))])<<8)|(ccClip255[(((a4 + a2)>>20))])@	
										@a5 = (((ccClip255[(((a5 + a0)>>20))])<<16)|((ccClip255[(((a5 - a1)>>20))])<<8)|(ccClip255[(((a5 + a2)>>20))]))@

		ldr	r6, [sp, #32]				@a0 = [sp, #32]
		ldr	r7, [sp, #36]				@a1 = [sp, #36]			
		ldr	r8, [sp, #40]				@a2 = [sp, #40]	
		
		add	r9, r1, r6		
		sub	r10, r1, r7
		add	r11, r1, r8
		
		ldrb	r9, [r14, r9, asr #20]		
		ldrb	r10, [r14, r10, asr #20]	
		ldrb	r11, [r14, r11, asr #20]						
		
		mov	r9, r9, lsl #16
		orr	r9, r9, r10, lsl #8
		orr	r1, r9, r11			        @a4 = ((ccClip255[(a4 + a0)>>20])<<16)|((ccClip255[(a4 - a1)>>20]))|(ccClip255[(a4 + a2)>>20])@
		
		add	r9, r2, r6		
		sub	r10, r2, r7
		add	r11, r2, r8
		
		ldrb	r9,  [r14, r9,  asr #20]		
		ldrb	r10, [r14, r10, asr #20]	
		ldrb	r11, [r14, r11, asr #20]	

		ldr	r6, [sp, #100]
		ldr	r7, [sp, #108]		
				
		mov	r9, r9, lsl #16
		orr	r9, r9, r10, lsl #8
		orr	r2, r9, r11 			    @a7 = ((ccClip255[(a5 + a0)>>20])<<16)|((ccClip255[(a5 - a1)>>20])<<8)|(ccClip255[(a5 + a2)>>20])@											
		
		str r1, [r6, #4]				
		str	r2, [r6], r7
		str	r6, [sp, #100]		
		ldr	r1, [sp, #4]				@ [sp, #4] = width
			
        @x_scale_par0 += 6@
		add	r12, r12, #24			
		
		subs	r1, r1, #2
		strne	r1, [sp, #4]
		ldrne	r1, [sp, #24]			@ [sp, #24] = src_u0	
		ldrne	r2, [sp, #28]			@ [sp, #28] = src_v0		

		bne		RGB32_LIT_WHILE_R

		ldr	r2, [sp, #8]				@ [sp, #8] = height
		ldr	r6, [sp, #48]						
		subs	r2, r2, #2	
		strne	r6, [sp, #4]					
		strne	r2, [sp, #8]
		bne		RGB32_BIG_WHILE_R				
	
        add     sp, sp, #60
		ldmia	sp!, {r4-r11, pc}		@ restore and return 

	@ENDP  @
		
.END
