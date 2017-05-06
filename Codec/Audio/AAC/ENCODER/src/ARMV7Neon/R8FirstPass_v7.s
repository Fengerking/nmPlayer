	EXPORT	|R8FirstPass|

	AREA	|.text|, CODE, READONLY

|R8FirstPass| PROC
	stmdb     sp!, {r4 - r11, lr}
|$M2649|
	ldr       r3, |$L2652|
	cmp       r1, #0
	
	VDUP.I32  Q15, r3	
	beq       |$L2125|
|$L2123|
	VLD1.I32		{d0, d1, d2, d3},	[r0]!
	VLD1.I32		{d8, d9, d10, d11},	[r0]!
		
	VADD.S32		d4, d0, d1		; ar = x[0] + x[2];ai = x[1] + x[3];
	VSUB.S32		d5, d0, d1		; br = x[0] - x[2];bi = x[1] - x[3];	
	VSUB.S32		d7, d2, d3		; dr = x[4] - x[6];di = x[5] - x[7];	
	VADD.S32		d6, d2, d3		; cr = x[4] + x[6];ci = x[5] + x[7];
	VREV64.I32		d7, d7	
	
	VADD.S32		Q0, Q2, Q3		; sr = ar + cr;si = ai + ci;vr = br + di;ti = bi + dr;
	VSUB.S32		Q1, Q2, Q3		; ur = ar - cr;ui = ai - ci;tr = br - di;vi = bi - dr;

	VREV64.I32		d3, d3	

	VADD.S32		d4, d8, d9		; ar = x[ 8] + x[10];ai = x[ 9] + x[11];
	VSUB.S32		d7, d10, d11	; dr = x[12] - x[14];di = x[13] - x[15];	
	VADD.S32		d6, d10, d11	; cr = x[12] + x[14];ci = x[13] + x[15];
	VREV64.I32		d7, d7	
	VSUB.S32		d5, d8, d9		; br = x[ 8] - x[10];bi = x[ 9] - x[11];
	
	VTRN.32			d1, d3	
	
	VADD.S32		Q4, Q2, Q3		; wr = (ar + cr) >> 1;wi = (ai + ci) >> 1;cr = br + di;ai = bi + dr;
	VSUB.S32		Q5, Q2, Q3		; yr = (ar - cr) >> 1;yi = (ai - ci) >> 1;ar = br - di;ci = bi - dr;
	
	VREV64.I32		d3, d3
	
	VSHR.S32		d8, d8, #1		 
	VSHR.S32		Q0, Q0, #1
	VREV64.I32		d10, d10
	VTRN.32			d11, d9
	VSHR.S32		Q1, Q1, #1
	VSHR.S32		d10, d10, #1
	VREV64.I32		d9, d9
	
	sub       		r0, r0, #0x40
	
	VADD.S32		d12, d0, d8
	VSUB.S32		d16, d0, d8	
	VADD.S32		d14, d2, d10
	VSUB.S32		d18, d2, d10
	
	VSUB.S32		d4, d11, d9
	VADD.S32		d5, d11, d9
	
	VREV64.I32		d18, d18
	
	VQDMULH.S32		Q3, Q2, Q15
	VTRN.32			d14, d18
	VTRN.32			d6, d7
	VREV64.I32		d18, d18	
	
	VSUB.S32		d15, d3, d6
	VREV64.I32		d7, d7
	VADD.S32		d19, d3, d6
	VADD.S32		d13, d1, d7
	VSUB.S32		d17, d1, d7
	
	VREV64.I32		d17, d17
	VTRN.32			d13, d17
	VREV64.I32		d17, d17
	
	sub       		r1, r1, #1	
	
	VST1.I32		{d12, d13, d14, d15}, [r0]!
	VST1.I32		{d16, d17, d18, d19}, [r0]!	


	cmp       		r1, #0
	bhi       		|$L2123|
|$L2125|

	ldmia     sp!, {r4 - r11, pc}
|$L2652|
	DCD       0x2d413ccd
|$M2650|

	ENDP  ; |R8FirstPass|

	EXPORT	|R4FirstPass|

	AREA	|.text|, CODE, READONLY

|R4FirstPass| PROC
	stmdb     	sp!, {r4 - r11, lr}
|$M3649|
	cmp       	r1, #0
	beq       	|$L3125|
|$L3123|
	VLD1.I32		{d0, d1}, [r0]!
	VLD1.I32		{d2, d3}, [r0]!	
	VADD.S32		d4, d0, d1
	VSUB.S32		d5, d0, d1
	VSUB.S32		d7, d2, d3
	VADD.S32		d6, d2, d3
	VREV64.I32		d7, d7
	
	VADD.S32		Q4, Q2, Q3
	VSUB.S32		Q5, Q2, Q3
	
	VREV64.I32		d11, d11
	sub       		r0, r0, #0x20
	VTRN.32			d9, d11
	sub       		r1, r1, #1	
	VREV64.I32		d11, d11
	cmp       		r1, #0	
	VST1.I32		{d8, d9, d10, d11}, [r0]!

	bhi       		|$L3123|
|$L3125|
	ldmia    		sp!, {r4 - r11, pc}
|$M3650|
	ENDP  ; |R4FirstPass|
	END