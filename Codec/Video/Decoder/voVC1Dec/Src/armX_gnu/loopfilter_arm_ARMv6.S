    #include "../../Src/c/voVC1DID.h"
    .include "wmvdec_member_arm.inc"
    .include "xplatform_arm_asm.h" 

    @AREA    |.text|, CODE
    .section .text

    .IF WMV_OPT_LOOPFILTER_ARM == 1
  
 
	.global  ARMV6_g_OverlapBlockHorizontalEdge


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    WMV_LEAF_ENTRY ARMV6_g_OverlapBlockHorizontalEdge

@ 5198 : {

	stmdb     sp!, {r4 - r12, lr}
    FRAME_PROFILE_COUNT
	sub       sp, sp, #0x24
M39663:
	mov       r6, r1
	mov       r5, r2
	str       r6, [sp, #0x1C]
	mov       r7, r3
	str       r5, [sp, #0x18]
	mov       r4, r0
	str       r7, [sp, #0xC]

@ 5199 :     I32_WMV  iRnd == 1@
@ 5200 :     pSrcTop +== 6 * iSrcStride@
@ 5201 : 
@ 5202 :     if (bTop & bCurrent) {

	ldr       r1, [sp, #0x50] @[#0x4C]
	add       r0, r5, r5, lsl #1
	add       r8, r4, r0, lsl #2
	ldr       r0, [sp, #0x54] @[#0x50]
	mov       r2, #1
	str       r2, [sp, #0x10]
	tst       r1, r0
	beq       L38874

@ 5203 :         I32_WMV i@
@ 5204 :         for ( i == 0@ i < 8@ i++) {

	ldr       r4, [sp, #0x4C] @[#0x48]
	mov       r1, #0
    mov       r12, #0
	mov       r1, r4, lsl #1
	sub       r0, r7, r1
	str       r0, [sp, #0x14]
	mov       r0, r5, lsl #1
	str       r0, [sp, #0x20]
	add       r11, r0, r6
	mov       lr, r6
	sub       r3, r8, r6
	sub       r10, r7, r4
	add       r9, r7, r4
L38876:

@ 5205 :             I32_WMV v0 == pSrcTop[i]@

	ldrsh     r7, [r3, +lr]

@ 5206 :             I32_WMV v1 == pSrcTop[i + iSrcStride]@
@ 5207 :             I32_WMV v2 == pSrcCurr[i]@

	ldrsh     r8, [lr], #2

@ 5208 :             I32_WMV v3 == pSrcCurr[i + iSrcStride]@
@ 5209 :             I32_WMV temp, k@
@ 5210 : 
@ 5211 :             temp == v0 + iRnd + 3@

	ldrsh     r5, [r11]
    add       r0, r7, r2
	add       r4, r0, #3

	ldrsh     r6, [r3, +r11]

@ 5212 :             k == ((7 * v2 - v3 + v1 + temp) >> 3) + g_iOverlapOffset@

	rsb       r0, r8, r8, lsl #3
    .if PLD_ENABLE==1
        PLD [r11, #32]
    .endif
	sub       r1, r0, r5
	add       r2, r1, r4
	add       r0, r2, r6
    .if PLD_ENABLE==1
        PLD [lr, #32]
    .endif
	mov       r1, r0, asr #3
	adds      r2, r1, #0x80
	ldr       r1, [sp, #0xC]

@ 5213 :             pDst[i] == (k < 0) ? 0 : ((k > 255) ? 255 : k)@
	usat		r2, #8, r2

@ 5214 :             k == ((6 * v0 + v3 + temp) >> 3) + g_iOverlapOffset@

	add       r0, r7, r7, lsl #1
	strb      r2, [r12, +r1]
        add r1, r3, r11
    .if PLD_ENABLE==1
        PLD [r1, #32]
    .endif   
	add       r1, r4, r0, lsl #1
	add       r2, r1, r5
	mov       r0, r2, asr #3
	adds      r1, r0, #0x80

@ 5215 :             pDst[i - 2 * iDstStride] == (k < 0) ? 0 : ((k > 255) ? 255 : k)@

    ldr       r2, [sp, #0x14]
	usat		r1, #8, r1

@ 5216 :             
@ 5217 :             temp == v3 + 4 - iRnd@
    ldr       r4, [sp, #0x10]
	add       r0, r5, r5, lsl #1
    strb      r1, [r2]
	sub       r1, r5, r4
	add       r4, r1, #4

@ 5218 :             k == ((6 * v3 + v0 + temp) >> 3) + g_iOverlapOffset@


        add r1, r3, lr
    .if PLD_ENABLE==1
        PLD [r1, #32]
    .endif
	add       r1, r4, r0, lsl #1
	add       r2, r1, r7
	mov       r0, r2, asr #3
	adds      r1, r0, #0x80

@ 5219 :             pDst[i + iDstStride] == (k < 0) ? 0 : ((k > 255) ? 255 : k)@
	usat		r1, #8, r1
	rsb       r0, r6, r6, lsl #3
	strb      r1, [r9, +r12]

@ 5220 :             k == ((7 * v1 - v0 + v2 + temp) >> 3) + g_iOverlapOffset@

	sub       r1, r0, r7
	add       r2, r1, r4
	add       r0, r2, r8
	mov       r1, r0, asr #3
	adds      r1, r1, #0x80

@ 5221 :             pDst[i - iDstStride] == (k < 0) ? 0 : ((k > 255) ? 255 : k)@
	usat		r1, #8, r1
	ldr       r2, [sp, #0x10]
    strb      r1, [r10, +r12]

@ 5222 : 
@ 5223 :             iRnd ^== 1@


	add       r12, r12, #1
	ldr       r0, [sp, #0x14]
	eor       r2, r2, #1
	str       r2, [sp, #0x10]
	add       r11, r11, #2
	add       r0, r0, #1
	str       r0, [sp, #0x14]
	cmp       r12, #8
	blt       L38876

@ 5224 :         }
@ 5225 : 
@ 5226 :         pSrcCurr +== 2 * iSrcStride@
@ 5227 :         pDst +== 2 * iDstStride@

	ldr       r7, [sp, #0x4C] @[#0x48]
	ldr       r1, [sp, #0xC]
	mov       r0, r7, lsl #1
	ldr       r10, [sp, #0x18]
	add       r6, r1, r0
	ldr       r8, [sp, #0x1C]

@ 5228 : 
@ 5229 :         if (!bWindup) {

	ldr       r0, [sp, #0x58] @[#0x54]
	add       r5, r8, r10, lsl #2
	cmp       r0, #0
	bne       L38911

@ 5230 :             I32_WMV i @
@ 5231 :             for (i == 0@ i < 4@ i++) {

	mov       r4, #4
L38887:

@ 5232 :                 I32_WMV j@
@ 5233 :                 for ( j == 0@ j < 8@ j++) {

@	mov       r3, #0
	mov       r2, r5
    .if PLD_ENABLE==1
        PLD [r2, #32]
    .endif
L38891:

@ 5234 :                     I32_WMV k == pSrcCurr[j] + g_iOverlapOffset@
@ 5235 :                     pDst[j] == (k < 0) ? 0 : ((k > 255) ? 255 : k)@
@num == 0~3	
	ldrsh     r0, [r2], #2
	ldrsh     r1, [r2], #2
	ldrsh     r8, [r2], #2
	ldrsh     r9, [r2], #2

	adds      r0, r0, #0x80
	adds      r1, r1, #0x80
	adds      r8, r8, #0x80
	adds      r9, r9, #0x80

		usat		r0, #8, r0
		usat		r1, #8, r1
		usat		r8, #8, r8
		usat		r9, #8, r9
	strb      r0, [r6, #0]
	strb      r1, [r6, #1]
	strb      r8, [r6, #2]
	strb      r9, [r6, #3]

@num == 4~7	
	ldrsh     r0, [r2], #2
	ldrsh     r1, [r2], #2
	ldrsh     r8, [r2], #2
	ldrsh     r9, [r2], #2

	adds      r0, r0, #0x80
	adds      r1, r1, #0x80
	adds      r8, r8, #0x80
	adds      r9, r9, #0x80

		usat		r0, #8, r0
		usat		r1, #8, r1
		usat		r8, #8, r8
		usat		r9, #8, r9
	strb      r0, [r6, #4]
	strb      r1, [r6, #5]
	strb      r8, [r6, #6]
	strb      r9, [r6, #7]

@	add       r3, r3, #1
@	cmp       r3, #8
@	blt       L38891

@ 5236 :                 }
@ 5237 :                 pSrcCurr +== iSrcStride@

	ldr       r0, [sp, #0x20]
	subs       r4, r4, #1

@ 5238 :                 pDst +== iDstStride@

	add       r6, r6, r7
	add       r5, r5, r0
	bne       L38887
L38911:

@ 5267 :         }
@ 5268 :     }
@ 5269 : }

	add       sp, sp, #0x24
	ldmia     sp!, {r4 - r12, pc}
L38874:

@ 5239 :             }
@ 5240 :         }
@ 5241 :     }
@ 5242 :     // remaining 2 of past
@ 5243 :     else if (bTop) {

	cmp       r1, #0
	beq       L38896

@ 5244 :         I32_WMV i@
@ 5245 :         pDst -== 2 * iDstStride@

	ldr       r7, [sp, #0x4C] @[#0x48]
	ldr       r0, [sp, #0xC]
	mov       r6, #2
	ldr       r10, [sp, #0x18]
	sub       r5, r0, r7, lsl #1
	mov       r4, r10, lsl #1
L38898:

@ 5246 :         for ( i == 0@ i < 2@ i++) {
@ 5247 :             I32_WMV j@
@ 5248 :             for ( j == 0@ j < 8@ j++) {

	mov       r2, r8
    .if PLD_ENABLE==1
        PLD [r2, #32]
    .endif
L38902:

@ 5249 :                 I32_WMV k == pSrcTop[j] + g_iOverlapOffset@
@ 5250 :                 pDst[j] == (k < 0) ? 0 : ((k > 255) ? 255 : k)@

@num == 0~3	
	ldrsh     r0, [r2], #2
	ldrsh     r1, [r2], #2
	ldrsh     r12, [r2], #2
	ldrsh     r9, [r2], #2

	adds      r0, r0, #0x80
	adds      r1, r1, #0x80
	adds      r12, r12, #0x80
	adds      r9, r9, #0x80

		usat		r0, #8, r0
		usat		r1, #8, r1
		usat		r12, #8, r12
		usat		r9, #8, r9
	strb      r0, [r5, #0]
	strb      r1, [r5, #1]
	strb      r12, [r5, #2]
	strb      r9, [r5, #3]

@num == 4~7	
	ldrsh     r0, [r2], #2
	ldrsh     r1, [r2], #2
	ldrsh     r12, [r2], #2
	ldrsh     r9, [r2], #2

	adds      r0, r0, #0x80
	adds      r1, r1, #0x80
	adds      r12, r12, #0x80
	adds      r9, r9, #0x80

		usat		r0, #8, r0
		usat		r1, #8, r1
		usat		r12, #8, r12
		usat		r9, #8, r9
	strb      r0, [r5, #4]
	strb      r1, [r5, #5]
	strb      r12, [r5, #6]
	strb      r9, [r5, #7]

	subs       r6, r6, #1

@ 5251 :             }
@ 5252 :             pSrcTop +== iSrcStride@

	add       r8, r8, r4

@ 5253 :             pDst +== iDstStride@

	add       r5, r5, r7
	bne       L38898

@ 5267 :         }
@ 5268 :     }
@ 5269 : }

	add       sp, sp, #0x24
	ldmia     sp!, {r4 - r12, pc}
L38896:

@ 5254 :         }
@ 5255 :     }
@ 5256 :     // remaining 6 of current
@ 5257 :     else if (bCurrent) {

	cmp       r0, #0
	beq       L38911

@ 5258 :         I32_WMV i@
@ 5259 :         for ( i == 0@ i < (bWindup ? 2 : 6)@ i++) {

	ldr       r1, [sp, #0x58] @[#0x54]
	ldr       r5, [sp, #0x4C] @[#0x48]
	mov       r6, #0
	ldr       r10, [sp, #0x18]
	mov       r11, #2
	ldr       r7, [sp, #0xC]
L38909:
	cmp       r1, #0
	mov       r0, r11
	moveq     r0, #6
	cmp       r6, r0
	bge       L38911

@ 5260 :             I32_WMV j@
@ 5261 :             for ( j == 0@ j < 8@ j++) {

	ldr       r8, [sp, #0x1C]
	mov       r3, r8
L38913:

@ 5262 :                 I32_WMV k == pSrcCurr[j] + g_iOverlapOffset@
@ 5263 :                 pDst[j] == (k < 0) ? 0 : ((k > 255) ? 255 : k)@
@num == 0~3	
	ldrsh     r0, [r3], #2
	ldrsh     r4, [r3], #2
	ldrsh     r12, [r3], #2
	ldrsh     r9, [r3], #2

	adds      r0, r0, #0x80
	adds      r4, r4, #0x80
	adds      r12, r12, #0x80
	adds      r9, r9, #0x80

		usat		r0, #8, r0
		usat		r4, #8, r4
		usat		r12, #8, r12
		usat		r9, #8, r9
	strb      r0, [r7, #0]
	strb      r4, [r7, #1]
	strb      r12, [r7, #2]
	strb      r9, [r7, #3]

@num == 4~7	
	ldrsh     r0, [r3], #2
	ldrsh     r4, [r3], #2
	ldrsh     r12, [r3], #2
	ldrsh     r9, [r3], #2

	adds      r0, r0, #0x80
	adds      r4, r4, #0x80
	adds      r12, r12, #0x80
	adds      r9, r9, #0x80

		usat		r0, #8, r0
		usat		r4, #8, r4
		usat		r12, #8, r12
		usat		r9, #8, r9
	strb      r0, [r7, #4]
	strb      r4, [r7, #5]
	strb      r12, [r7, #6]
	strb      r9, [r7, #7]

@ 5264 :             }
@ 5265 :             pSrcCurr +== iSrcStride@

	add       r8, r8, r10, lsl #1
	str       r8, [sp, #0x1C]

@ 5266 :             pDst +== iDstStride@

	add       r7, r7, r5
	add       r6, r6, #1
	b         L38909
M39664:
    WMV_ENTRY_END
	@@ENDP  @ ARMV6_g_OverlapBlockHorizontalEdge

	.ENDIF @ WMV_OPT_LOOPFILTER_ARM
    
  .END
  