@//*@@@+++@@@@******************************************************************
@//
@// Microsoft Windows Media
@// Copyright (C) Microsoft Corporation. All rights reserved.
@//
@//*@@@---@@@@******************************************************************

@//************************************************************************
@//
@// Module Name:
@//
@//     affine_arm.s
@//
@// Abstract:
@// 
@//     ARM specific optimization for affine pan and zoom routines
@//
@//     Custom build with 
@//          armasm $(InputDir)\$(InputName).s $(OutDir)\$(InputName).obj
@//     and
@//          $(OutDir)\$(InputName).obj
@// 
@// Author:
@// 
@//     Chuang Gu (chuanggu@microsoft.com) Nov. 1, 2002
@//
@// Revision History:
@//
@//************************************************************************
#include "../c/voWMVDecID.h"
.include "wmvdec_member_arm.inc"
.include "xplatform_arm_asm.h"

@AREA COMMON, CODE, READONLY
.section .text

.if WMV_OPT_SPRITE_ARM==1

.global Affine_Add
.global Affine_PanYLineC
.global Affine_PanUVLineC
.global Affine_PanFadingYLineC
.global Affine_PanFadingUVLineC
.global Affine_StretchYLineC
.global Affine_StretchUVLineC
.global Affine_StretchFadingYLineC
.global Affine_StretchFadingUVLineC

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

WMV_LEAF_ENTRY Affine_Add

@ 84   : {
	stmdb     sp!, {r4 - r11, lr}
	@FRAME_PROFILE_COUNT
M38575:
		mov       r4, r0
		mov       r8, r1
		mov       r9, r2
		mov       r11, r3
		@ 85   :     I32_WMV i@
		@ 86   :     const U32_WMV* puiSrcY = (U32_WMV*) pucSrcY@
		@ 87   :     U32_WMV* puiDstY = (U32_WMV*) pucDstY@
		@ 88   :     iSize >>= 2@

		ldr       r0, [sp, #0x2C]
		movs      lr, r0, asr #2

		@ 89   :     for (i = iSize@ i != 0@ i--) {

			beq       L37135
				ldr       r10, [sp, #0x28]
				sub       r5, r4, r11
				ldr       r7, [sp, #0x24]
L37133:

				@ 90   :         I32_WMV uiSrcU = (I32_WMV)(*pucSrcU++)@
				@ 91   :         I32_WMV uiSrcV = (I32_WMV)(*pucSrcV++)@
				@ 92   :         I32_WMV uiDstU = (I32_WMV)(*pucDstU)@
				@ 93   :         I32_WMV uiDstV = (I32_WMV)(*pucDstV)@
				@ 94   :         *puiDstY++ += *puiSrcY++@

				ldr       r0, [r11]

				.if PLD_ENABLE==1
				PLD   [r11, #32]
				.endif

				subs       lr, lr, #1
				ldrb      r3, [r8], #1
				@cmp       lr, #0
				ldr       r1, [r5, +r11]
				ldrb      r2, [r7]
				ldrb      r6, [r9], #1
				add       r1, r1, r0
				ldrb      r4, [r10]

				add   r0, r5, r11
				.if PLD_ENABLE==1
				PLD   [r0, #32]
				PLD   [r7, #32]
				PLD   [r10, #32]
				.endif
				add       r0, r2, r3
				str       r1, [r11], #4

				@ 95   :         uiDstU += uiSrcU - 128@
				@ 96   :         uiDstV += uiSrcV - 128@
				@ 97   :         *pucDstU++ = (U8_WMV) uiDstU@

				add       r1, r0, #0x80
				add       r0, r4, r6
				strb      r1, [r7], #1
				@and       r2, r1, #0xFF

				@ 98   :         *pucDstV++ = (U8_WMV) uiDstV@

				add       r1, r0, #0x80
				@and       r2, r1, #0xFF
				strb      r1, [r10], #1
				bne       L37133
L37135:
				@ 99   :     }
	@ 100  : }

	ldmia     sp!, {r4 - r11, pc}

	WMV_ENTRY_END
	
	@ENDP  @ |Affine_Add|

	@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	WMV_LEAF_ENTRY Affine_PanYLineC

	@ 104  : {
		stmdb     sp!, {r4 - r12, lr}
		@FRAME_PROFILE_COUNT
			sub       sp, sp, #4
			mov       r3, r0
			mov       r12, r2
			mov       r8, r1

			@ 105  :     const I32_WMV factor_rx_ry = pWMVDec->factor_rx_ry@

			ldr       lr, [r3, #tagWMVDecInternalMember_factor_rx_ry] @[r0, #0xFC]

			@ 106  :     const I32_WMV rx = pWMVDec->rx@
			@ 107  :     const I32_WMV ry = pWMVDec->ry@

			ldr       r11, [r3, #tagWMVDecInternalMember_rx] @ [r1]
			ldr       r9, [r3, #tagWMVDecInternalMember_ry] @ [r0, #4]

			@ 108  :     const I32_WMV rxXry = pWMVDec->rxXry@
			@ 109  : 
			@ 110  :     const U8_WMV* pSrcYNext = pSrcY + pWMVDec->m_iOldWidth@

			ldr       r7, [r3, #tagWMVDecInternalMember_rxXry] @ [r1, #0x10]
			ldr       r1, [r3, #tagWMVDecInternalMember_m_iOldWidth] @ [r0, #0x34]

			@ 111  :     I32_WMV x = pWMVDec->iYLengthX@

			add       r5, r1, r8
			ldr       r1, [r3, #tagWMVDecInternalMember_iYLengthX] @ [r0, #0xEC]
			movs      r10, r1


			ldrb      r3, [r5]
			ldrb      r0, [r5, #1]!
			ldrb      r6, [r8]
			ldrb      r4, [r8, #1]!

			@ 112  :     for (@ x != 0@ x--) {

				beq       L37161

L37159:

					@ 113  :         I32_WMV U00, U10, U01, U11, iDstY, itemp@
					@ 114  :         U00 = pSrcY [0]@
					@ 115  :         iDstY = U00 * factor_rx_ry@
					@ 116  :         U10 = pSrcY [1]@       
					@ 117  :         U01 = pSrcYNext [0]@




					@ 118  :         iDstY += U10 * rx@
					@ 119  :         U11 = pSrcYNext [1]@
					@ 120  :         itemp = U00 - U01 - U10@
					@ 121  :         iDstY += U01 * ry@
					@ 122  :         itemp += U11@
					@ 123  :         itemp *= rxXry@
					@ 124  :         iDstY += itemp >> 7@
					@ 125  :         iDstY >>= 7@
					@ 126  : 
					@ 127  :         *pDstY++ = iDstY@ 


					mul		  r2, r6, lr		@U00 * factor_rx_ry
					add		  r1, r3, r4
					sub		  r1, r6, r1
					add		  r0, r0, r1

					mul       r1, r0, r7		@(U00 - U01 - U10+U00) * rxXry
					.if PLD_ENABLE==1
					PLD [r5, #32]
					.endif
					subs      r10, r10, #1   
					ldrb      r6, [r8]


					MLA       r2, r3, r9, r2	@U01 * ry
					ldrb      r3, [r5]
					ldrb      r0, [r5, #1]!


					MLA       r2, r4, r11, r2	@U10*rx
					.if PLD_ENABLE==1
					PLD [r8, #32]
					.endif
					ldrb      r4, [r8, #1]!


					ADD       r2, r2, r1, asr #7
					mov       r2, r2, asr #7
					strb      r2, [r12], #1

					bne       L37159
L37161:

					@ 128  : 
					@ 129  :         pSrcY++@
					@ 130  :         pSrcYNext++@
					@ 131  : 
					@ 132  :         //*pDstY++ = (unsigned char)((U00*factor_rx_ry + U01*ry + U10*rx 
					@ 133  :         //                            + (((U00+U11-U01-U10)*rxXry) >> 7) ) >> 7)@
					@ 134  :     }
		@ 135  : }

		add       sp, sp, #4
		ldmia     sp!, {r4 - r12, pc}
		WMV_ENTRY_END
		
		@ENDP  @ |Affine_PanYLineC|

		@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

		WMV_LEAF_ENTRY Affine_PanUVLineC

		@ 279  : {

			stmdb     sp!, {r4 - r12, lr}
			@FRAME_PROFILE_COUNT
				sub       sp, sp, #0xC
				mov       r10, r0

				@ 280  :     const I32_WMV rx_2 = pWMVDec->rx_2@

				ldr       r11, [r10, #tagWMVDecInternalMember_rx_2] @ [r0, #8]
				str       r2, [sp]		@pSrcV
				str		  r0, [sp, #4]	@pWMVDec
				mov       r8, r1		@pSrcU
				mov		  r12, r3		@pDestU

				@ 281  :     const I32_WMV ry_2 = pWMVDec->ry_2@

				ldr       r9, [r10, #tagWMVDecInternalMember_ry_2] @ [r1, #0xC]

				@ 282  :     const I32_WMV rxry = pWMVDec->rxry@
				@ 283  :     const I32_WMV lrxry = pWMVDec->lrxry@

				ldr       r2, [r10, #tagWMVDecInternalMember_m_iOldWidth] @ [r0, #0x34]
				ldr       lr, [r10, #tagWMVDecInternalMember_lrxry] @ [r1, #0x18]
				ldr       r7, [r10, #tagWMVDecInternalMember_rxry] @ [r0, #0x14]

				@ 284  :     const U8_WMV* pSrcUVNext@
				@ 285  :     I32_WMV x@
				@ 286  : 
				@ 287  :     pSrcUVNext = pSrcU + (pWMVDec->m_iOldWidth >> 1)@
				@ 288  :     x = pWMVDec->iUVLengthX@

				add       r5, r8, r2, asr #1
				ldr       r1, [r10, #tagWMVDecInternalMember_iUVLengthX] @ [r0, #0xF0]

				ldrb      r6, [r8]
				ldrb      r3, [r5]
				ldrb      r0, [r5, #1]!
				ldrb      r4, [r8, #1]!

				movs      r10, r1


				@ 289  :     for (@ x != 0@ x--) {

					beq       L37321

L37311:

						mul		  r2, r6, lr		@U00 * lrxry
						add		  r1, r3, r4
						sub		  r1, r6, r1
						add		  r0, r0, r1

						mul       r1, r0, r7		@(U00 - U01 - U10+U00) * rxXry
						.if PLD_ENABLE==1
						PLD [r5, #32]
						.endif
						subs      r10, r10, #1   
						ldrb      r6, [r8]


						MLA       r2, r3, r9, r2	@U01 * ry
						ldrb      r3, [r5]
						ldrb      r0, [r5, #1]!


						MLA       r2, r4, r11, r2	@U10*rx
						.if PLD_ENABLE==1
						PLD [r8, #32]
						.endif
						ldrb      r4, [r8, #1]!


						ADD       r2, r2, r1, asr #7
						mov       r2, r2, asr #7
						strb      r2, [r12], #1

						bne       L37311
						@
						@Start V
						@
						ldr		  r0, [sp, #4]	@pWMVDec
						ldr       r8, [sp]		@pSrcV
						ldr		  r12, [sp, #0x34]		@pDestV
						ldr       r2, [r0, #tagWMVDecInternalMember_m_iOldWidth] @ [r0, #0x34]
						ldr       r10, [r0, #tagWMVDecInternalMember_iUVLengthX] @ [r0, #0xF0]
						add       r5, r8, r2, asr #1

						ldrb      r6, [r8]
						ldrb      r3, [r5]
						ldrb      r0, [r5, #1]!
						ldrb      r4, [r8, #1]!

L37319:

						mul		  r2, r6, lr		@U00 * lrxry
						add		  r1, r3, r4
						sub		  r1, r6, r1
						add		  r0, r0, r1

						mul       r1, r0, r7		@(U00 - U01 - U10+U00) * rxXry
						.if PLD_ENABLE==1
						PLD [r5, #32]
						.endif
						subs      r10, r10, #1   
						ldrb      r6, [r8]


						MLA       r2, r3, r9, r2	@U01 * ry
						ldrb      r3, [r5]
						ldrb      r0, [r5, #1]!


						MLA       r2, r4, r11, r2	@U10*rx
						.if PLD_ENABLE==1
						PLD [r8, #32]
						.endif
						ldrb      r4, [r8, #1]!


						ADD       r2, r2, r1, asr #7
						mov       r2, r2, asr #7
						strb      r2, [r12], #1

						bne       L37319


L37321:

						@ 311  :     }
			@ 312  : }

			add       sp, sp, #0xC
			ldmia     sp!, {r4 - r12, pc}
			WMV_ENTRY_END
			
			@ENDP  @ |Affine_PanUVLineC|
			@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

			WMV_LEAF_ENTRY Affine_PanFadingYLineC

			@ 257  : {

				stmdb     sp!, {r4 - r12, lr}
				@FRAME_PROFILE_COUNT
					sub       sp, sp, #8
					mov       r3, r0
					mov       r12, r2
					mov       r8, r1

					@ 258  :     const I32_WMV factor_rx_ry = pWMVDec->factor_rx_ry@

					ldr       r0, [r3, #tagWMVDecInternalMember_factor_rx_ry] @ [r0, #0xFC]

					@ 259  :     const I32_WMV rx = pWMVDec->rx@

					ldr       lr, [r3, #tagWMVDecInternalMember_rx] @ [r1]

					@ 260  :     const I32_WMV ry = pWMVDec->ry@
					@ 261  :     const I32_WMV rxXry = pWMVDec->rxXry@

					str       r0, [sp, #4]
					ldr       r9, [r3, #tagWMVDecInternalMember_ry] @ [r0, #4]

					@ 262  :     const I32_WMV iFading = pWMVDec->iFading@


					ldr       r7, [r3, #tagWMVDecInternalMember_rxXry] @ [r1, #0x10]

					@ 263  :     const U8_WMV* pSrcYNext = pSrcY + pWMVDec->m_iOldWidth@


					ldr       r10, [r3, #tagWMVDecInternalMember_iFading] @ [r0, #0x1C]
					ldr       r0, [r3, #tagWMVDecInternalMember_m_iOldWidth] @  [r1, #0x34]

					@ 264  :     I32_WMV x = pWMVDec->iYLengthX@


					add       r5, r0, r8
					ldr       r0, [r3, #tagWMVDecInternalMember_iYLengthX] @ [r1, #0xEC]
					movs      r11, r0

					@ 265  :     for (@ x != 0@ x--) {

						ldrb      r3, [r5]
							ldrb      r6, [r8]
							ldrb      r4, [r8, #1]!
							ldrb      r0, [r5, #1]!

							beq       L37288
L37286:

							@ 266  :         I32_WMV U00 = pSrcY [0]@
							@ 267  :         I32_WMV U10 = pSrcY [1]@
							@ 268  :         I32_WMV U01 = pSrcYNext [0]@
							@ 269  :         I32_WMV U11 = pSrcYNext [1]@



							@ 270  :         pSrcY++@
							@ 271  :         pSrcYNext++@
							@ 272  :         *pDstY++ = (unsigned char)((((U00*factor_rx_ry + U01*ry + U10*rx 
												@ 273  :                                     + (((U00+U11-U01-U10)*rxXry) >> 11)) >> 11) * iFading) >> 8 )@


							mul		  r2, r3, r9			@U01*ry
							add       r1, r4, r3
							sub       r1, r6, r1
							add       r0, r0, r1			@(U00+U11-U01-U10)

							mul       r1, r0, r7			@(U00+U11-U01-U10)*rxXry
							ldr       r3, [sp, #4]			@factor_rx_ry
							subs      r11, r11, #1

							MUL       r0, r4, lr			@U10*rx
							add       r1, r2, r1, asr #11

							.if PLD_ENABLE==1
							PLD   [r5, #32]
							PLD   [r8, #32]
							.endif

							MLA       r0, r3, r6, r0		@U00*factor_rx_ry
							ldrb      r3, [r5]
							ldrb      r6, [r8]

							ADD       r0, r0, r1
							mov       r2, r0, asr #11

							mul       r1, r2, r10			@*iFading
							ldrb      r4, [r8, #1]!
							ldrb      r0, [r5, #1]!

							mov       r2, r1, asr #8
							strb      r2, [r12], #1
							bne       L37286

L37288:

							@ 274  :     }
				@ 275  : }

				add       sp, sp, #8
				ldmia     sp!, {r4 - r12, pc}
				WMV_ENTRY_END
				
				@ENDP  @ |Affine_PanFadingYLineC|
				@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

				WMV_LEAF_ENTRY Affine_PanFadingUVLineC

				@ 316  : {

					stmdb     sp!, {r4 - r12, lr}
					@FRAME_PROFILE_COUNT
						sub       sp, sp, #0x10
						mov       r9, r0
						mov       r6, r1
						mov       r11, r2
						mov       r7, r3

						@ 317  :     const I32_WMV rx_2 = pWMVDec->rx_2@

						ldr       r0, [r9, #tagWMVDecInternalMember_rx_2] @ [r0, #8]

						@ 318  :     const I32_WMV ry_2 = pWMVDec->ry_2@

						str       r0, [sp, #4]
						ldr       r0, [r9, #tagWMVDecInternalMember_ry_2] @ [r1, #0xC]

						@ 319  :     const I32_WMV rxry = pWMVDec->rxry@
						@ 320  :     const I32_WMV lrxry = pWMVDec->lrxry@


						ldr       r10, [r9, #tagWMVDecInternalMember_lrxry] @ [r1, #0x18]

						@ 321  :     const I32_WMV iFading = pWMVDec->iFading@
						@ 322  :     I32_WMV x@
						@ 323  : 
						@ 324  :     x = pWMVDec->iUVLengthX@


						str       r0, [sp]
						ldr       r8, [r9, #tagWMVDecInternalMember_rxry] @ [r0, #0x14]
						ldr       r0, [r9, #tagWMVDecInternalMember_iFading] @ [r0, #0x1C]
						str       r0, [sp, #8]
						ldr       r0, [r9, #tagWMVDecInternalMember_iUVLengthX] @ [r1, #0xF0]
						cmp       r0, #0
						mov       r12, r0

						@ 325  :     for (@ x != 0@ x--) {

							beq       L37346

								@ 317  :     const I32_WMV rx_2 = pWMVDec->rx_2@

								ldr       lr, [sp, #0x38] @ [sp, #0x34]
								ldrb      r5, [r6, #1]
								ldrb      r4, [r6]
								ldr       r1, [r9, #tagWMVDecInternalMember_m_iOldWidth] @ [r0, #0x34]
								mov       r2, r6
								ldrb      r3, [r2, +r1, asr #1]!	@U01

L37344:

								@ 326  :         I32_WMV U00, U10, U01, U11@
								@ 327  :         U00 = pSrcU [0]@
								@ 328  :         U10 = pSrcU [1]@
								@ 329  :         U01 = (pSrcU + (pWMVDec->m_iOldWidth >> 1)) [0]@


								@    ldr       r1, [r9, #tagWMVDecInternalMember_m_iOldWidth] @ [r0, #0x34]
								@    mov       r2, r6


								@ 330  :         U11 = (pSrcU + (pWMVDec->m_iOldWidth >> 1)) [1]@
								@ 331  :         pSrcU++@


								@    ldrb      r3, [r2, +r1, asr #1]!	@U01
								ldrb      r0, [r2, #1]				@U11

								.if PLD_ENABLE==1
								PLD   [r2, #32]
								.endif

								@ 332  :         *pDstU++ = (unsigned char)((((((U00*(lrxry) + U01*ry_2 + U10*rx_2 + (((U00+U11-U01-U10)*rxry) >> 11)) >> 11) - 128) * iFading) >> 8 ) + 128)@

								add       r1, r5, r3
								sub       r1, r4, r1
								add       r2, r1, r0		@U00+U11-U01-U10
								mul       r0, r2, r8
								add       r6, r6, #1
								ldr       r2, [sp]

								mov       r0, r0, asr #11

								MLA       r0, r4, r10, r0
								LDR       r4, [sp, #4]

								MUL       r1, r5, r4
								.if PLD_ENABLE==1
								PLD   [r11, #32]
								.endif

								MLA       r0, r3, r2, r0
								ldr       r3, [sp, #8]
								ldrb      r5, [r11, #1]
								ADD       r0, r0, r1

								mov       r0, r0, asr #11
								sub       r2, r0, #0x80

								mul       r4, r2, r3
								mov       r2, r11
								ldr       r1, [r9, #tagWMVDecInternalMember_m_iOldWidth] @ [r0, #0x34]
								mov       r4, r4, asr #8
								add       r4, r4, #0x80
								strb      r4, [r7], #1

								@ 333  :         
								@ 334  :         U00 = pSrcV [0]@
								@ 335  :         U10 = pSrcV [1]@
								@ 336  :         U01 = (pSrcV + (pWMVDec->m_iOldWidth >> 1)) [0]@

								ldrb      r3, [r2, +r1, asr #1]!


								@ 337  :         U11 = (pSrcV + (pWMVDec->m_iOldWidth >> 1)) [1]@
								@ 338  :         pSrcV++@


								ldrb      r0, [r2, #1]

								@ 339  :         *pDstV++ = (unsigned char)((((((U00*(lrxry) + U01*ry_2 + U10*rx_2 + (((U00+U11-U01-U10)*rxry) >> 11)) >> 11) - 128) * iFading) >> 8 ) + 128)@

								ldrb      r4, [r11]
								add       r1, r3, r5
								sub       r1, r0, r1
								add       r2, r1, r4

								mul       r0, r2, r8
								add       r11, r11, #1
								subs      r12, r12, #1
								mov       r0, r0, asr #11

								MLA       r1, r4, r10, r0
								ldr       r0, [sp]
								ldr       r2, [sp, #4]
								.if PLD_ENABLE==1
								PLD   [r11, #32]
								.endif

								MLA       r0, r3, r0, r1
								ldrb      r4, [r6]					@next r4
								ldr       r1, [r9, #tagWMVDecInternalMember_m_iOldWidth] @ next r1

								MLA       r3, r5, r2, r0
								.if PLD_ENABLE==1
								PLD   [r6, #32]
								.endif
								ldr       r0, [sp, #8]
								ldrb      r5, [r6, #1]				@next r5

								mov       r3, r3, asr #11
								sub       r3, r3, #0x80

								mul       r0, r3, r0
								mov       r2, r6
								ldrb      r3, [r2, +r1, asr #1]!	@U01

								mov       r0, r0, asr #8
								add       r0, r0, #0x80
								strb      r0, [lr], #1

								bne       L37344
L37346:

								@ 340  :     }
					@ 341  : }

					add       sp, sp, #0x10
					ldmia     sp!, {r4 - r12, pc}
					WMV_ENTRY_END
					
					@ENDP  @ |Affine_PanFadingUVLineC|
					@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

					WMV_LEAF_ENTRY Affine_StretchYLineC

					@ 140  : {

						stmdb     sp!, {r4 - r12, lr}
						@FRAME_PROFILE_COUNT
							sub       sp, sp, #0xC
							mov       r9, r0
							str       r2, [sp]
							mov       r8, r1
							mov       r12, #0x7F, 28
							orr       r12, r12, #0xF

							@ 141  :     const I32_WMV factor = pWMVDec->factor@

							@add       r0, r9, #0x36, 24
							ldr       r0, [r9, #tagWMVDecInternalMember_lA] @ [r0, #0x28]
							ldr       r10, [r9, #tagWMVDecInternalMember_factor] @ [r0, #0x30]

							@ 142  :     const I32_WMV ry = pWMVDec->ry@
							@ 143  :     const I32_WMV lA = pWMVDec->lA@


							ldr       r11, [r9, #tagWMVDecInternalMember_ry] @ [r1, #4]

							@ 144  : 
							@ 145  :     I32_WMV ox = pWMVDec->oxRef@

							ldr       r3, [r9, #tagWMVDecInternalMember_oxRef] @ [r1, #0x20]
							str       r0, [sp, #8]

							@ 146  :     I32_WMV x = pWMVDec->iYLengthX@


							ldr       r1, [r9, #tagWMVDecInternalMember_iYLengthX] @ [r0, #0xEC]
							str       r3, [sp, #4]
							movs      lr, r1

							@ 147  :     for (@ x != 0@ x--) {

								beq       L37182

									ldr       r1, [r9, #tagWMVDecInternalMember_m_iOldWidth] @ [r0, #0x34]
									mov       r2, r8
									ldrb      r5, [r2, +r3, asr #11]!
									ldrb      r7, [r1, +r2]!

L37180:

									@ 148  :         I32_WMV U00, U10, U01, U11, rx@
									@ 149  :         const U8_WMV* pTmp = pSrcY + (ox >> 11)@

									@    ldr       r1, [r9, #tagWMVDecInternalMember_m_iOldWidth] @ [r0, #0x34]
									@    mov       r2, r8

									@ 150  :         U00 = pTmp [0]@

									@    ldrb      r5, [r2, +r3, asr #11]!

									@ 151  :         U10 = pTmp [1]@
									@ 152  :         U01 = (pTmp + pWMVDec->m_iOldWidth) [0]@
									@ 153  :         U11 = (pTmp + pWMVDec->m_iOldWidth) [1]@
									@ 154  :         rx = ox & 0x7FF@


									@    ldrb      r7, [r1, +r2]!
									ldrb      r6, [r2, #1]
									ldrb      r0, [r1, #1]
									and       r4, r3, r12 @ r0

									@ 155  :         *pDstY++ = (unsigned char)((U00*(factor-rx-ry)+ U01*ry + U10*rx + (((U00+U11-U01-U10)*rx*ry) >> 11)) >> 11)@

									add		  r3, r6, r7
									sub       r3, r5, r3
									add       r0, r3, r0
									mul       r3, r0, r4

									.if PLD_ENABLE==1
									PLD [r1, #32]
									PLD [r2, #32]
									.endif

									sub       r0, r10, r4
									mul       r1, r3, r11

									@ 156  :         ox += lA@

									ldr       r3, [sp, #4]

									sub       r2, r0, r11
									subs      lr, lr, #1										@end
									mov       r1, r1, asr #11
									MLA       r0, r2, r5, r1

									ldr       r2, [sp, #8]
									ldr       r1, [r9, #tagWMVDecInternalMember_m_iOldWidth]	@next loop begin

									MLA       r0, r4, r6, r0
									add       r3, r3, r2										@end
									str       r3, [sp, #4]
									ldr       r4, [sp]

									MLA       r0, r11, r7, r0

									mov       r2, r8											@next loop begin
									ldrb      r5, [r2, +r3, asr #11]!
									ldrb      r7, [r1, +r2]!									@next loop begin

									mov       r6, r0, asr #11
									strb      r6, [r4], #1
									str       r4, [sp]


									bne       L37180
L37182:

									@ 157  :     }
						@ 158  : }

						add       sp, sp, #0xC
						ldmia     sp!, {r4 - r12, pc}
						WMV_ENTRY_END
						@ENDP  @ |Affine_StretchYLineC|
						

						@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


						WMV_LEAF_ENTRY Affine_StretchFadingYLineC

						@ 162  : {

							stmdb     sp!, {r4 - r12, lr}
							@FRAME_PROFILE_COUNT
								sub       sp, sp, #0x10
								mov       r9, r0
								ldr       r0, [r9, #tagWMVDecInternalMember_lA] @ [r0, #0x28]
								ldr       r10, [r9, #tagWMVDecInternalMember_factor] @ [r0, #0x30]
								ldr       lr, [r9, #tagWMVDecInternalMember_ry] @ [r1, #4]
								str       r0, [sp, #0xC]
								mov       r12, r2
								ldr       r11, [r9, #tagWMVDecInternalMember_iFading] @ [r1, #0x1C]
								ldr       r3, [r9, #tagWMVDecInternalMember_oxRef] @ [r0, #0x20]
								ldr       r0, [r9, #tagWMVDecInternalMember_iYLengthX] @ [r1, #0xEC]
								mov       r8, r1

								@ 163  :     const I32_WMV factor = pWMVDec->factor@
								@ 164  :     const I32_WMV ry = pWMVDec->ry@
								@ 165  :     const I32_WMV lA = pWMVDec->lA@
								@ 166  :     const I32_WMV iFading = pWMVDec->iFading@
								@ 167  : 
								@ 168  :     I32_WMV ox = pWMVDec->oxRef@
								@ 169  :     I32_WMV x = pWMVDec->iYLengthX@

								str       r3, [sp, #4]
								cmp       r0, #0
								str       r0, [sp, #8]

								@ 170  :     for (@ x != 0@ x--) {

									beq       L37205

										mov       r2, r8
										ldr       r1, [r9, #tagWMVDecInternalMember_m_iOldWidth] @ [r0, #0x34]

L37203:

										@ 171  :         I32_WMV U00, U10, U01, U11, rx@
										@ 172  :         const U8_WMV* pTmp = pSrcY + (ox >> 11)@

										@    mov       r2, r8

										@ 173  :         U00 = pTmp [0]@
										@ 174  :         U10 = pTmp [1]@
										@ 175  :         U01 = (pTmp + pWMVDec->m_iOldWidth) [0]@

										@    ldr       r1, [r9, #tagWMVDecInternalMember_m_iOldWidth] @ [r0, #0x34]

										ldrb      r5, [r2, +r3, asr #11]!	@U00
										.if PLD_ENABLE==1
										PLD [r2, #32]
										.endif

										@ 176  :         U11 = (pTmp + pWMVDec->m_iOldWidth) [1]@
										@ 177  :         rx = ox & 0x7FF@

										mov       r0, #0x7F, 28
										orr       r0, r0, #0xF
										ldrb      r7, [r1, +r2]!	@ U01
										and       r4, r3, r0
										mul		  r3, lr, r4		@ rx*ry
										ldrb      r6, [r2, #1]		@ U10

										@ 178  :         *pDstY++ = (unsigned char)((((U00*(factor-rx-ry)+ U01*ry + U10*rx + (((U00+U11-U01-U10)*rx*ry) >> 11)) >> 11)*iFading)>>8)@@

										ldrb      r0, [r1, #1]		@ U11
										.if PLD_ENABLE==1
										PLD [r1, #32]
										.endif
										add       r1, r6, r7
										mul		  r6, r4, r6		@ U10*rx
										sub       r0, r0, r1
										add       r0, r0, r5
										mul		  r3, r0, r3		@(U00+U11-U01-U10)*rx*ry)
										sub       r0, r10, r4		@factor-rx

										@ 179  :         ox += lA@

										ldr       r4, [sp, #4]		@ox
										MLA		  r6, lr, r7, r6	@U10*ry
										sub       r1, r0, lr		@factor-rx-ry

										ldr       r7, [sp, #0xC]	@lA
										ldr       r2, [sp, #8]		@x
										add       r6, r6, r3, asr #11	@+ (>>11)
										MLA       r0, r1, r5, r6	@U00*(factor-rx-ry)

										subs      r2, r2, #1
										add       r3, r4, r7		@ox+lA
										str       r3, [sp, #4]
										mov		  r0, r0, asr #11	@ >>11
										mul       r7, r0, r11		@*iFading

										str       r2, [sp, #8]
										mov       r2, r8
										ldr       r1, [r9, #tagWMVDecInternalMember_m_iOldWidth] @ [r0, #0x34]

										mov       r0, r7, asr #8
										strb      r0, [r12], #1
										bne       L37203
L37205:

										@ 180  :     }
							@ 181  : }

							add       sp, sp, #0x10
							ldmia     sp!, {r4 - r12, pc}
							WMV_ENTRY_END
							@ENDP  @ |Affine_StretchFadingYLineC|
							

							@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

							WMV_LEAF_ENTRY Affine_StretchUVLineC

							@ 185  : {

								stmdb     sp!, {r4 - r12, lr}
								@FRAME_PROFILE_COUNT
									sub       sp, sp, #0x14
									mov       r12, r0
									mov       r7, r1
									mov       lr, r2
									mov       r11, r3

									@ 186  :     const I32_WMV factor = pWMVDec->factor@

									mov       r3, r0
									ldr       r0, [r3, #tagWMVDecInternalMember_lAx2] @ [r0, #0x2C]
									ldr       r6, [r3, #tagWMVDecInternalMember_factor] @ [r0, #0x30]

									@ 187  :     const I32_WMV ry = pWMVDec->ry@
									@ 188  :     const I32_WMV lAx2 = pWMVDec->lAx2@

									ldr       r2, [r3, #tagWMVDecInternalMember_ry] @ [r1, #4]

									@ 189  : 
									@ 190  :     I32_WMV ox = pWMVDec->oxRef@

									str       r0, [sp, #0x10]

									@ 191  :     I32_WMV x = pWMVDec->iYLengthX@

									ldr       r5, [r3, #tagWMVDecInternalMember_oxRef] @ [r1, #0x20]
									ldr       r1, [r3, #tagWMVDecInternalMember_iYLengthX] @ [r0, #0xEC]
									str       r2, [sp, #8]
									str       r5, [sp, #4]
									cmp       r1, #0
									str       r1, [sp, #0xC]

									@ 192  :     for (@ x != 0@ x--) {

										beq       L37231

											@ 209  :         U10 = pTmp [1]@
											@ 210  :         U01 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [0]@

L38663:

											@ 193  :         I32_WMV U00, U10, U01, U11, rx, lrxry, rxXry@
											@ 194  :         const U8_WMV* pTmp@
											@ 195  : 
											@ 196  :         rx = 0x7FF & (ox >> 1)@

											mov       r0, #0x7F, 28
											orr       r0, r0, #0xF
											and       r10, r0, r5, asr #1

											@ 197  :         lrxry = factor-rx-ry@

											sub       r0, r6, r10		@factor-rx
											sub       r9, r0, r2		@factor-rx-ry

											@ 198  :         rxXry = rx * ry@

											mul       r8, r10, r2
											ldr       r1, [r12, #tagWMVDecInternalMember_m_iOldWidth] @ [r0, #0x34]

											@ 199  : 
											@ 200  :         pTmp = pSrcU + (ox >> 12)@
											@ 201  :         U00 = pTmp [0]@

											mov       r2, r7
											ldrb      r4, [r2, +r5, asr #12]!		@

											@ 202  :         U10 = pTmp [1]@
											@ 203  :         U01 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [0]@

											ldrb      r5, [r2, #1]
											.if PLD_ENABLE==1
											PLD [r2, #32]
											.endif


											ldrb      r3, [r2, +r1, asr #1]!

											@ 204  :         U11 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [1]@
											@ 205  :         *pDstU++ = (unsigned char)((U00*lrxry + U01*ry + U10*rx + (((U00+U11-U01-U10)*rxXry) >> 11)) >> 11)@

											ldrb      r0, [r2, #1]
											.if PLD_ENABLE==1
											PLD [r2, #32]
											.endif

											mul       r2, r4, r9		@ U00*lrxry

											add		  r1, r3, r5
											sub       r1, r0, r1
											add       r0, r1, r4		@(U00+U11-U01-U10)

											MLA       r2, r5, r10, r2	@ U10*rx
											ldr       r5, [sp, #8]
											mul       r1, r0, r8		@(U00+U11-U01-U10)*rxXry
											ldr       r4, [sp, #4]		@
											mov       r0, lr
											MLA       r2, r3, r5, r2	@ U01*ry

											ldrb      r4, [r0, +r4, asr #12]!
											add       r1, r2, r1, asr #11@	>>11

											@ 206  : 
											@ 207  :         pTmp = pSrcV + (ox >> 12)@


											@ 208  :         U00 = pTmp [0]@
											@ 209  :         U10 = pTmp [1]@
											@ 210  :         U01 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [0]@

											ldrb      r5, [r0, #1]
											ldr       r3, [r12, #tagWMVDecInternalMember_m_iOldWidth] @ [r0, #0x34]
											.if PLD_ENABLE==1
											PLD [r0, #32]
											.endif

											mov       r1, r1, asr #11
											strb      r1, [r11], #1			@save U.
											ldrb      r3, [r0, +r3, asr #1]!

											@ 211  :         U11 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [1]@
											@ 212  :     
											@ 213  :         *pDstV++ = (unsigned char)((U00*lrxry + U01*ry + U10*rx + (((U00+U11-U01-U10)*rxXry) >> 11)) >> 11)@

											ldrb      r2, [r0, #1]
											.if PLD_ENABLE==1
											PLD [r0, #32]
											.endif

											sub       r1, r4, r3
											sub       r1, r1, r5
											add       r0, r2, r1
											mul       r1, r0, r8
											ldr       r2, [sp, #8]
											ldr       r8, [sp, #0xC]
											mov       r0, r1, asr #11
											MLA       r0, r4, r9, r0
											subs      r8, r8, #1
											str       r8, [sp, #0xC]
											MUL       r4, r5, r10

											@ 214  :             
											@ 215  :         ox += lAx2@

											ldr       r9, [sp, #0x3C] @ [sp, #0x38]
											ldr       r10, [sp, #0x10]
											MLA       r0, r3, r2, r0
											ldr       r5, [sp, #4]
											add       r5, r5, r10
											str       r5, [sp, #4]
											ADD       r0, r0, r4
											mov       r4, r0, asr #11
											strb      r4, [r9], #1
											str       r9, [sp, #0x3C] @ [sp, #0x38] 
											cmp		  r8, #0
											bne       L38663

L37231:

											@ 216  :     }
								@ 217  : }

								add       sp, sp, #0x14
								ldmia     sp!, {r4 - r12, pc}
								WMV_ENTRY_END
								@ENDP  @ |Affine_StretchUVLineC|
								

								@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

								WMV_LEAF_ENTRY Affine_StretchFadingUVLineC

								@ 261  : {

									stmdb     sp!, {r4 - r11, lr}
									sub       sp, sp, #0x14
										mov       r5, r0
										str       r5, [sp, #4]
										mov       r8, r1
										str       r2, [sp, #8]
										mov       r11, r3

										@ 262  :     const I32_WMV factor = pWMVDec->factor@
										@ 263  :     const I32_WMV ry = pWMVDec->ry@
										@ 264  :     const I32_WMV lAx2 = pWMVDec->lAx2@

										ldr       r12, [r5, #0xA8]

										@ 265  :     const I32_WMV iFading = pWMVDec->iFading@
										@ 266  : 
										@ 267  :     I32_WMV ox = pWMVDec->oxRef@
										@ 268  :     I32_WMV x = pWMVDec->iYLengthX@

										mov       r1, #0x7F, 28
										@	str       r0, [sp, #0x10]
										ldr       r0, [r5, #0x80]
										ldr       r9, [r5, #0x9C]
										orr       r1, r1, #0xF
										str       r0, [sp]					@ry
										ldr       r6, [r5, #0x98]
										ldr       r4, [r5, #0x68]
										ldr       lr, [r5, #0xAC]			@factor
										str       r6, [sp, #0xC]
										movs      r10, r4
										and       r7, r1, r9, asr #1		@rx

										@ 269  :     DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_StretchFadingUVLineC)@
										@ 270  : 
										@ 271  :     for (@ x != 0@ x--) {

												beq       L46074

												mov       r1, r8
												ldr       r0, [r5, #0xB0]			@pWMVDec->m_iOldWidth
												ldrb      r4, [r1, +r9, asr #12]!	@U00

L46060:

												@ 272  :         I32_WMV U00, U10, U01, U11, rx, lrxry, rxXry@
												@ 273  :         const U8_WMV* pTmp@
												@ 274  : 
												@ 275  :         rx = 0x7FF & (ox >> 1)@
												@ 276  :         lrxry = factor-rx-ry@
												@ 277  :         rxXry = rx * ry@
												@ 278  : 
												@ 279  :         pTmp = pSrcU + (ox >> 12)@

												@	mov       r1, r8

												@ 280  :         U00 = pTmp [0]@
												@ 281  :         U10 = pTmp [1]@
												@ 282  :         U01 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [0]@

												@	ldrb      r4, [r1, +r9, asr #12]!	@U00
												@   ldr       r0, [r5, #0xB0]			@pWMVDec->m_iOldWidth
												@    .if PLD_ENABLE==1
												@        pld   [r1, #32]
												@    .endif

												ldrb      r6, [r1, #1]				@U10
												ldrb      r5, [r1, +r0, asr #1]!	@U01

												@ 283  :         U11 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [1]@
												@ 284  :         
												@ 285  : 		*pDstU++ = (unsigned char)((((((U00*(lrxry) + U01*ry + U10*rx + (((U00+U11-U01-U10)*rxXry) >> 11)) >> 11)-128)*iFading)>>8)+128)@          

												ldrb      r0, [r1, #1]				@U11
												mul       r2, r6, r7				@U10*rx

												.if PLD_ENABLE==1
												pld   [r1, #32]
												.endif


												add       r1, r6, r5
												sub       r1, r0, r1
												add       r0, r1, r4				@(U00+U11-U01-U10)

												@ 292  :     for (@ x != 0@ x--) {

													mul       r3, r0, r7				@(U00+U11-U01-U10)*rx
														ldr       r1, [sp]					@ry
														@    ldr       r12, [sp, #0x10]			@lAx2
														sub       r0, lr, r7				@factor-rx
														add       r9, r9, r12				@ox+lAx2
														mul       r6, r3, r1				@(U00+U11-U01-U10)*rx*ry

														sub       r0, r0, r1				@factor-rx-ry
														mla       r3, r0, r4, r2			@(factor-rx-ry)*U00+U10*rx

														mov       r0, #0x7F, 28
														orr       r0, r0, #0xF
														ldr       r4, [sp, #0xC]			@iFading
														mla       r3, r5, r1, r3			@U01*ry+(factor-rx-ry)*U00+U10*rx

														and       r7, r0, r9, asr #1		@0x7FF & (ox >> 1)
														ldr       r5, [sp, #4]				@pWMVDec
														mov       r1, r8

														add       r3, r3, r6, asr #11		@+(U00+U11-U01-U10)*rx*ry>>11
														ldr       r0, [r5, #0xB0]			@pWMVDec->m_iOldWidth
														mov       r3, r3, asr #11
														sub       r3, r3, #0x80
														mul       r6, r3, r4				@*iFading

														subs      r10, r10, #1				@x
														ldrb      r4, [r1, +r9, asr #12]!	@U00
														.if PLD_ENABLE==1
														pld   [r1, #32]
														.endif


														mov       r6, r6, asr #8
														add       r6, r6, #0x80
														strb      r6, [r11], #1

														bhi       L46060

														@ 261  : {
															@
																@calculate V frame
																@

																ldr       r8, [sp, #8]				@pSrcV
																ldr       r11, [sp, #0x38]			@pDstV

																ldr       r10, [r5, #0x68]			@x
																ldr       r9, [r5, #0x9C]			@ox

																mov       r1, #0x7F, 28
																orr       r1, r1, #0xF
																and       r7, r1, r9, asr #1		@rx

																mov       r1, r8
																ldr       r0, [r5, #0xB0]			@pWMVDec->m_iOldWidth
																ldrb      r4, [r1, +r9, asr #12]!	@U00

L48379:

																ldrb      r6, [r1, #1]				@U10
																ldrb      r5, [r1, +r0, asr #1]!	@U01

																@ 283  :         U11 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [1]@
																@ 284  :         
																@ 285  : 		*pDstU++ = (unsigned char)((((((U00*(lrxry) + U01*ry + U10*rx + (((U00+U11-U01-U10)*rxXry) >> 11)) >> 11)-128)*iFading)>>8)+128)@          

																ldrb      r0, [r1, #1]				@U11
																mul       r2, r6, r7				@U10*rx

																.if PLD_ENABLE==1
																pld   [r1, #32]
																.endif


																add       r1, r6, r5
																sub       r1, r0, r1
																add       r0, r1, r4				@(U00+U11-U01-U10)

																@ 292  :     for (@ x != 0@ x--) {

																	mul       r3, r0, r7				@(U00+U11-U01-U10)*rx
																		ldr       r1, [sp]					@ry
																		@   ldr       r12, [sp, #0x10]			@lAx2
																		sub       r0, lr, r7				@factor-rx
																		add       r9, r9, r12				@ox+lAx2
																		mul       r6, r3, r1				@(U00+U11-U01-U10)*rx*ry

																		sub       r0, r0, r1				@factor-rx-ry
																		mla       r3, r0, r4, r2			@(factor-rx-ry)*U00+U10*rx

																		mov       r0, #0x7F, 28
																		orr       r0, r0, #0xF
																		ldr       r4, [sp, #0xC]			@iFading
																		mla       r3, r5, r1, r3			@U01*ry+(factor-rx-ry)*U00+U10*rx

																		and       r7, r0, r9, asr #1		@0x7FF & (ox >> 1)
																		ldr       r5, [sp, #4]				@pWMVDec
																		mov       r1, r8

																		add       r3, r3, r6, asr #11		@+(U00+U11-U01-U10)*rx*ry>>11
																		ldr       r0, [r5, #0xB0]			@pWMVDec->m_iOldWidth
																		mov       r3, r3, asr #11
																		sub       r3, r3, #0x80
																		mul       r6, r3, r4				@*iFading

																		subs      r10, r10, #1				@x
																		ldrb      r4, [r1, +r9, asr #12]!	@U00
																		.if PLD_ENABLE==1
																		pld   [r1, #32]
																		.endif

																		mov       r6, r6, asr #8
																		add       r6, r6, #0x80
																		strb      r6, [r11], #1

																		bhi       L48379
L46074:
																		@ 309  :     }
															@ 310  : }

															add       sp, sp, #0x14
																ldmia     sp!, {r4 - r11, pc}

																WMV_ENTRY_END
																@ENDP  @ |Affine_StretchFadingUVLineC|


																@    .global end_affine_arm
																@end_affine_arm
																@    nop

																@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

																.endif @ WMV_OPT_SPRITE_ARM= 1

																.end 

