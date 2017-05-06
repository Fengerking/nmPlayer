;	/************************************************************************
;	*																		*
;	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
;	*																		*
;	************************************************************************/

	AREA	|.text|, CODE, READONLY
        EXPORT	sbrasm1
        EXPORT	sbrasm2
        EXPORT	sbrasm3
        EXPORT	sbrasm4 

; void sbrasm1(int *XBuf, int *accBuf)
;   see comments in sbrhfgen.c

sbrasm1		FUNCTION
	
	STMFD		sp!, {r4-r11, r14}

	LDR		r3, [r0], #4*(1)
	LDR		r4, [r0], #4*(2*64-1)
	LDR		r5, [r0], #4*(1)
	LDR		r6, [r0], #4*(2*64-1)
	RSB		r14, r4, #0

	SMULL		r7, r8, r5, r3
	SMLAL		r7, r8, r6, r4
	SMULL		r9, r10, r3, r6
	SMLAL		r9, r10, r14, r5
	SMULL		r11, r12, r3, r3
	SMLAL		r11, r12, r4, r4

	ADD		r2, r1, #(4*6)
	STMIA		r2, {r7-r12}

	MOV		r7, #0
	MOV		r8, #0
	MOV		r9, #0
	MOV		r10, #0
	MOV		r11, #0
	MOV		r12, #0
	
	MOV		r2, #(16*2 + 6)

CV1_Loop_Start
	MOV		r3, r5
	LDR		r5, [r0], #4*(1)
	MOV		r4, r6
	LDR		r6, [r0], #4*(2*64-1)
	RSB		r14, r4, #0
	
	SMLAL		r7, r8, r5, r3
	SMLAL		r7, r8, r6, r4
	SMLAL		r9, r10, r3, r6
	SMLAL		r9, r10, r14, r5
	SMLAL		r11, r12, r3, r3
	SMLAL		r11, r12, r4, r4

	SUBS		r2, r2, #1
	BNE		CV1_Loop_Start

	STMIA		r1, {r7-r12}

	LDR		r0, [r1, #4*(6)]	
	LDR		r2,  [r1, #4*(7)]	
	RSB		r3, r3, #0
	ADDS		r7, r0, r7	
	ADC		r8, r2,  r8	
	SMLAL		r7, r8, r5, r3
	SMLAL		r7, r8, r6, r14

	LDR		r0, [r1, #4*(8)]	
	LDR		r2,  [r1, #4*(9)]	
	ADDS		r9, r0, r9	
	ADC		r10, r2,  r10	
	SMLAL		r9, r10, r3, r6
	SMLAL		r9, r10, r4, r5
	
	LDR		r0, [r1, #4*(10)]	
	LDR		r2,  [r1, #4*(11)]	
	ADDS		r11, r0, r11	
	ADC		r12, r2,  r12	
	RSB		r0, r3, #0
	SMLAL		r11, r12, r3, r0
	RSB		r2,  r4, #0
	SMLAL		r11, r12, r4, r2
	
	ADD		r1, r1, #(4*6)
	STMIA		r1, {r7-r12}

	LDMFD		sp!, {r4-r11, pc}
	ENDFUNC



sbrasm2		FUNCTION
	
	STMFD	        sp!, {r4-r11, r14}
	
	MOV		r7, #0
	MOV		r8, #0
	MOV		r9, #0
	MOV		r10, #0
	
	LDR		r3, [r0], #4*(1)
	LDR		r4, [r0], #4*(2*64-1)
	LDR		r5, [r0], #4*(1)
	LDR		r6, [r0], #4*(2*64-1)
	MOV		r2, #(16*2 + 6)

CV2_Loop_Start
	LDR		r11, [r0], #4*(1)
	LDR		r12, [r0], #4*(2*64-1)
	RSB		r14, r4, #0

	SMLAL		r7, r8, r11, r3
	SMLAL		r7, r8, r12, r4
	SMLAL		r9, r10, r3, r12
	SMLAL		r9, r10, r14, r11
	
	MOV		r3, r5
	MOV		r4, r6
	MOV		r5, r11
	MOV		r6, r12

	SUBS		r2, r2, #1
	BNE		CV2_Loop_Start

	STMIA		r1, {r7-r10}

	LDMFD		sp!, {r4-r11, pc}
	ENDFUNC



		GBLA	FBITS_OUT_QMFS
FBITS_OUT_QMFS	SETA	(14 - (1+2+3+2+1) - (2+3+2) + 6 - 1)

		GBLA	RND_VAL
RND_VAL			SETA	(1 << (FBITS_OUT_QMFS - 1))

; void QMFSynthesisConv(int *cPtr, int *delay, int dIdx, short *outbuf, int channelNum);
;   see comments in sbrqmf.c

sbrasm4		FUNCTION
	
	STMFD		sp!, {r4-r11, r14}

	LDR		r9,  [r13, #4*9]	; we saved 9 registers on stack
	MOV		r5, r2, lsl #7		; dOff0 = 128*dIdx
	SUBS		r6, r5, #1		; dOff1 = dOff0 - 1
	ADDLT		r6, r6, #1280	; if (dOff1 < 0) then dOff1 += 1280
	MOV		r4, #64

SRC_Loop_Start
	LDR		r10, [r0], #4
	LDR		r12, [r0], #4
	LDR		r11, [r1, r5, lsl #2]
	LDR		r14, [r1, r6, lsl #2]
	SMULL		r7, r8, r10, r11
	SUBS		r5, r5, #256
	ADDLT		r5, r5, #1280
	SMLAL		r7, r8, r12, r14
	SUBS		r6, r6, #256
	ADDLT		r6, r6, #1280

	LDR		r10, [r0], #4
	LDR		r12, [r0], #4
	LDR		r11, [r1, r5, lsl #2]
	LDR		r14, [r1, r6, lsl #2]
	SMLAL		r7, r8, r10, r11
	SUBS		r5, r5, #256
	ADDLT		r5, r5, #1280
	SMLAL		r7, r8, r12, r14
	SUBS		r6, r6, #256
	ADDLT		r6, r6, #1280

	LDR		r10, [r0], #4
	LDR		r12, [r0], #4
	LDR		r11, [r1, r5, lsl #2]
	LDR		r14, [r1, r6, lsl #2]
	SMLAL		r7, r8, r10, r11
	SUBS		r5, r5, #256
	ADDLT		r5, r5, #1280
	SMLAL		r7, r8, r12, r14
	SUBS		r6, r6, #256
	ADDLT		r6, r6, #1280

	LDR		r10, [r0], #4
	LDR		r12, [r0], #4
	LDR		r11, [r1, r5, lsl #2]
	LDR		r14, [r1, r6, lsl #2]
	SMLAL		r7, r8, r10, r11
	SUBS		r5, r5, #256
	ADDLT		r5, r5, #1280
	SMLAL		r7, r8, r12, r14
	SUBS		r6, r6, #256
	ADDLT		r6, r6, #1280

	LDR		r10, [r0], #4
	LDR		r12, [r0], #4
	LDR		r11, [r1, r5, lsl #2]
	LDR		r14, [r1, r6, lsl #2]
	SMLAL		r7, r8, r10, r11
	SUBS		r5, r5, #256
	ADDLT		r5, r5, #1280
	SMLAL		r7, r8, r12, r14
	SUBS		r6, r6, #256
	ADDLT		r6, r6, #1280

	ADD		r5, r5, #1
	SUB		r6, r6, #1
	
	ADD		r8, r8, #RND_VAL
	MOV		r8, r8, asr #FBITS_OUT_QMFS
	MOV		r7, r8, asr #31
	CMP		r7, r8, asr #15
	EORNE		r8, r7, #0x7f00	
	EORNE		r8, r8, #0x00ff
	STRH		r8, [r3, #0]
	ADD		r3, r3, r9, lsl #1
   
   	SUBS		r4, r4, #1
	BNE		SRC_Loop_Start

	LDMFD		sp!, {r4-r11, pc}
	ENDFUNC


;void QMFAnalysisConv(int *cTab, int *r1, int r2, int *r3)
;   see comments in sbrqmf.c

sbrasm3		FUNCTION
	
	STMFD		sp!, {r4-r11, r14}

	MOV		r6, r2, lsl #5		; dOff0 = 32*r2
	ADD		r6, r6, #31			; dOff0 = 32*r2 + 31
	ADD		r4, r0, #4*(164)	; r4 = r0 + 164
	
	; special first pass (flip sign for cTab[384], cTab[512])
	LDR		r11, [r0], #4
	LDR		r14, [r0], #4
	LDR		r12, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	LDR		r2, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	SMULL		r7, r8, r11, r12
	SMULL		r9, r10, r14, r2

	LDR		r11, [r0], #4
	LDR		r14, [r0], #4
	LDR		r12, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	LDR		r2, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	SMLAL		r7, r8, r11, r12
	SMLAL		r9, r10, r14, r2

	LDR		r11, [r0], #4
	LDR		r14, [r4], #-4
	LDR		r12, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	LDR		r2, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	SMLAL		r7, r8, r11, r12
	SMLAL		r9, r10, r14, r2

	LDR		r11, [r4], #-4
	LDR		r14, [r4], #-4
	LDR		r12, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	LDR		r2, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	RSB		r11, r11, #0
	SMLAL		r7, r8, r11, r12
	SMLAL		r9, r10, r14, r2

	LDR		r11, [r4], #-4
	LDR		r14, [r4], #-4
	LDR		r12, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	LDR		r2, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	RSB		r11, r11, #0
	SMLAL		r7, r8, r11, r12
	SMLAL		r9, r10, r14, r2

	STR		r10, [r3, #4*32]
	STR		r8, [r3], #4
	SUB		r6, r6, #1

	MOV		r5, #31

SRC_Loop_Start1

	LDR		r11, [r0], #4
	LDR		r14, [r0], #4
	LDR		r12, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	LDR		r2, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	SMULL		r7, r8, r11, r12
	SMULL		r9, r10, r14, r2

	LDR		r11, [r0], #4
	LDR		r14, [r0], #4
	LDR		r12, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	LDR		r2, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	SMLAL		r7, r8, r11, r12
	SMLAL		r9, r10, r14, r2

	LDR		r11, [r0], #4
	LDR		r14, [r4], #-4
	LDR		r12, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	LDR		r2, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	SMLAL		r7, r8, r11, r12
	SMLAL		r9, r10, r14, r2

	LDR		r11, [r4], #-4
	LDR		r14, [r4], #-4
	LDR		r12, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	LDR		r2, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	SMLAL		r7, r8, r11, r12
	SMLAL		r9, r10, r14, r2

	LDR		r11, [r4], #-4
	LDR		r14, [r4], #-4
	LDR		r12, [r1, r6, lsl #2]
	SUBS	 	r6, r6, #32
	ADDLT		r6, r6, #320
	LDR		r2, [r1, r6, lsl #2]
	SUBS		r6, r6, #32
	ADDLT		r6, r6, #320
	SMLAL		r7, r8, r11, r12
	SMLAL		r9, r10, r14, r2

	STR		r10, [r3, #4*32]
	STR		r8, [r3], #4
	SUB		r6, r6, #1
	
	SUBS		r5, r5, #1
	BNE		SRC_Loop_Start1

	LDMFD		sp!, {r4-r11, pc}
	ENDFUNC

	END
