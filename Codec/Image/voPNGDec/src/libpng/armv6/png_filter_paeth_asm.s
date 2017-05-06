	AREA |.text|, CODE, READONLY
	EXPORT	png_filter_paeth_armv6
	
png_filter_paeth_armv6 FUNCTION
|$LN20@calc_paeth|
	stmdb       sp!, {r4 - r10, lr}
|$M38673|
	mov         r5, r1
	ldr         r6, [r0]         ;rp
	ldr         r8, [r0, #8]     ;pp
	ldr         r9, [r0, #4]     ;lp
	ldr         r10, [r0, #0xC]  ;cp  
	;cmp         r5, #0
	;ble         |$LN1@calc_paeth|
|$LL3@calc_paeth|
	ldrb        r7, [r8], #1      ;b   
	ldrb        r4, [r10], #1     ;c
	ldrb        r2, [r9], #1      ;a
	
	subs		lr,r7,r4
	rsbmi		lr,lr,#0          ;lr = pa [|b-c|]
	
	subs		r1,r2,r4
	rsbmi		r1,r1,#0          ;r1 = pb [|a-c|]	
	
	add         r3,r7,r2
	subs		r3,r3,r4,lsl #1
	rsbmi       r3, r3, #0        ; r3 = pc [|p+pc|]	
	
	cmp         lr, r1
	bgt         |$LN12@calc_paeth|
	cmp         lr, r3
	ble         |$LN14@calc_paeth|
|$LN12@calc_paeth|
	cmp         r1, r3
	movle       r2, r7
	movgt       r2, r4

|$LN14@calc_paeth|
	ldrb        r3, [r6]
	subs        r5, r5, #1
	add         r3, r3, r2
	strb        r3, [r6], #1
	
	bne         |$LL3@calc_paeth|
|$LN1@calc_paeth|
	ldmia       sp!, {r4 - r10, pc}
|$M38674|
	ENDFUNC
	END