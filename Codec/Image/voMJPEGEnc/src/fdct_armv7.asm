;*****************************************************************************
;*																			*
;*		VisualOn, Inc. Confidential and Proprietary, 2003					*
;*																			*
;*****************************************************************************

	
	AREA	|.text|, CODE
	
	EXPORT	fdct_armv7

	ALIGN	8
					;	 98 ,  139 ,  181 ,  334
Const_table		DCW		0x0062,0x008b,0x00b5,0x014e

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;void fdct_int32(short *const block, short* qvaltab,short* invqvaltab)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
fdct_armv7 PROC

;lr: can't cover the data

;d0: const data
;tmp0 - tmp7   : tmp0 = d8; else = d1-d7
;tmp10 - tmp13 : d10-d13
;d20-d27       : input data

    stmdb   	sp!, {r4 - r12, lr}

	ldr			r12, =Const_table
	mov			r6, #8
	mov			r5, #16
	add			r7, r0, #8   ;222
	;vld1.32		d0, [r12] ;right?	
	vld1.16		d0, [r12] ;  ;223
	
ROW_LOOP	
	; process 4 columns per loop	
	vld4.16		{d20[0],d21[0],d22[0],d23[0]}, [r0], r5   
	vld4.16		{d24[0],d25[0],d26[0],d27[0]}, [r7], r5
	vld4.16		{d20[1],d21[1],d22[1],d23[1]}, [r0], r5
	vld4.16		{d24[1],d25[1],d26[1],d27[1]}, [r7], r5
	vld4.16		{d20[2],d21[2],d22[2],d23[2]}, [r0], r5
	vld4.16		{d24[2],d25[2],d26[2],d27[2]}, [r7], r5
	vld4.16		{d20[3],d21[3],d22[3],d23[3]}, [r0], r5
	vld4.16		{d24[3],d25[3],d26[3],d27[3]}, [r7], r5
	sub			r0, r0, r5,lsl #2
	sub			r7, r7, r5,lsl #2
	subs		r6, r6, #4

	vadd.s16	d8, d20, d27
	vsub.s16	d7, d20, d27
	vadd.s16	d1, d21, d26
	vsub.s16	d6, d21, d26
	vadd.s16	d2, d22, d25
	vsub.s16	d5, d22, d25
	vadd.s16	d3, d23, d24
	vsub.s16	d4, d23, d24
	
	vqadd.s16	d10, d8, d3	;tmp10
	vqsub.s16	d13, d8, d3	;tmp13
	vqadd.s16	d11, d1, d2	;tmp11
	vqsub.s16	d12, d1, d2	;tmp12	
	
	vqadd.s16   d20, d10, d11	;	dataptr[0] = tmp10 + tmp11;
	vqsub.s16   d24, d10, d11	;	dataptr[4] = tmp10 - tmp11;
	
	vqadd.s16	d29, d12, d13	;	tmp12 + tmp13
	vmull.s16	q8,  d29, d0[2] ;	(tmp12 + tmp13) * FIX_0_707106781	
	;
	vshr.s32	q8, q8, #8	;	
	vqshrn.s32	d29, q8,  #0	;	z1>>8      right?	
	;vshr.s16	d29, d29, #8	;	z1>>8      right?
	
	
	vqadd.s16   d22, d13, d29	;	dataptr[2] = tmp13 + z1;
	vqsub.s16   d26, d13, d29	;	dataptr[6] = tmp13 - z1;
	
	vqadd.s16	d10, d4, d5		;	tmp10 = tmp4 + tmp5;
	vqadd.s16	d11, d5, d6		;	tmp11 = tmp5 + tmp6;   
	vqadd.s16	d12, d6, d7		;	tmp12 = tmp6 + tmp7;  
	
	;	d29 = z5 = (((tmp10 - tmp12) * FIX_0_382683433) >> 8);  
	vqsub.s16	d29, d10, d12	;
	vmull.s16	q9,  d29, d0[0] ;	z5
	vshr.s32	q9, q9, #8	;
	vqshrn.s32	d29, q9,  #0	;		
	;vshr.s16	d29, d29, #8	;	z5>>8      right?
	
	;	d28 = z2 = ((tmp10 * FIX_0_541196100) >> 8) + z5; 
	vmull.s16	q9,  d10, d0[1]
	;vqshrn.s32	d28, q9,  #8	;	z2>>8      right? 
	vshr.s32	q9, q9, #8	;
	vqshrn.s32	d28, q9,  #0	;	      right?	
	;vshr.s16	d28, d28, #8	;	z2>>8      right?	
	vqadd.s16	d28, d28, d29		;
		
	;	d17 = z4 = ((tmp12 * FIX_1_306562965) >> 8) + z5; 
	vmull.s16	q9,  d12, d0[3]
	vshr.s32	q9, q9, #8	;
	vqshrn.s32	d17, q9,  #0	;	      right?	
	;vshr.s16	d17, d17, #8	;	z4>>8      right?	
	vqadd.s16	d17, d17, d29		
	
	;	d16 = z3 = ((tmp11 * FIX_0_707106781) >> 8);
	vmull.s16	q9,  d11, d0[2]
	vshr.s32	q9, q9, #8	;
	vqshrn.s32	d16, q9,  #0	;	      right?	
	;vshr.s16	d16, d16, #8	;	z3>>8      right?
	
	vqadd.s16	d15, d7, d16	;	d15 = z11 = tmp7 + z3;	
	vqsub.s16	d14, d7, d16    ;	d14 = z13 = tmp7 - z3;	
	
	;	dataptr[5] = z13 + z2;	
	;	dataptr[3] = z13 - z2;
	;	dataptr[1] = z11 + z4;
	;	dataptr[7] = z11 - z4;
	
	vqadd.s16   d25, d14, d28	;	dataptr[5] = z13 + z2;	
	vqsub.s16   d23, d14, d28	;	dataptr[3] = z13 - z2;
	
	vqadd.s16   d21, d15, d17	;	dataptr[1] = z11 + z4;
	vqsub.s16   d27, d15, d17	;	dataptr[7] = z11 - z4;	
	
	;	dataptr[0] = tmp10 + tmp11;
	;	dataptr[4] = tmp10 - tmp11;	
			
	;	z1 = (((tmp12 + tmp13) * FIX_0_707106781) >> 8);
	;	dataptr[2] = tmp13 + z1;  ; (tmp13*437+tmp12*181)>>8	
	;	dataptr[6] = tmp13 - z1;  ; (tmp13*75-tmp12*181)>>8	
	
	;	tmp10 = tmp4 + tmp5;	
	;	tmp11 = tmp5 + tmp6;
	;	tmp12 = tmp6 + tmp7;			
	;	z5 = (((tmp10 - tmp12) * FIX_0_382683433) >> 8); 
	;	z2 = ((tmp10 * FIX_0_541196100) >> 8) + z5; 
	;	z4 = ((tmp12 * FIX_1_306562965) >> 8) + z5; 
	;	z3 = ((tmp11 * FIX_0_707106781) >> 8); 			
	;	z11 = tmp7 + z3;	
	;	z13 = tmp7 - z3;
	;	dataptr[5] = z13 + z2;	
	;	dataptr[3] = z13 - z2;
	;	dataptr[1] = z11 + z4;
	;	dataptr[7] = z11 - z4;
	
	vst4.16		{d20[0],d21[0],d22[0],d23[0]}, [r0], r5
	vst4.16		{d24[0],d25[0],d26[0],d27[0]}, [r7], r5
	vst4.16		{d20[1],d21[1],d22[1],d23[1]}, [r0], r5
	vst4.16		{d24[1],d25[1],d26[1],d27[1]}, [r7], r5
	vst4.16		{d20[2],d21[2],d22[2],d23[2]}, [r0], r5
	vst4.16		{d24[2],d25[2],d26[2],d27[2]}, [r7], r5
	vst4.16		{d20[3],d21[3],d22[3],d23[3]}, [r0], r5
	vst4.16		{d24[3],d25[3],d26[3],d27[3]}, [r7], r5
	
	bgt			ROW_LOOP
;q0: const data
;q15: #1<<4
;q14: #1<<17 
;tmp0 - tmp7:tmp0 = d0 ; else = d1-d7
;tmp10 - tmp13 : d10-d13
;d20-d27       : input data

	;r1= qvaltab = quant_tbl->Quant_tbl_ptrs[ctr]->quantval;
	;r2= invqvaltab = quant_tbl->Quant_tbl_ptrs[ctr]->invquantval;

	sub			r0, r0, r5,lsl #3
	mov			r6, #8
	mov			r5, #16
	;vmov.u32	q15, #1<<4
	;vmov.u32	q14, #1<<17 
COL_LOOP
	
	; process 4 columns per loop
	
	;	tmp0 = blkptr[0] + blkptr[56];
	;	tmp7 = blkptr[0] - blkptr[56];
	;	tmp1 = blkptr[8] + blkptr[48];
	;	tmp6 = blkptr[8] - blkptr[48];
	;	tmp2 = blkptr[16] + blkptr[40];
	;	tmp5 = blkptr[16] - blkptr[40];
	;	tmp3 = blkptr[24] + blkptr[32];
	;	tmp4 = blkptr[24] - blkptr[32];

	add			r7, r0, r5, lsl #2
	vld1.32		d20, [r0], r5
	vld1.32		d24, [r7], r5
	vld1.32		d21, [r0], r5
	vld1.32		d25, [r7], r5
	vld1.32		d22, [r0], r5
	vld1.32		d26, [r7], r5
	vld1.32		d23, [r0]
	vld1.32		d27, [r7]
	
	sub			r0, r0, #64-16
	subs		r6, r6, #4


	vadd.s16	d2, d22, d25
	vsub.s16	d5, d22, d25
	vadd.s16	d3, d23, d24
	vsub.s16	d4, d23, d24
	vadd.s16	d1, d21, d26
	vsub.s16	d6, d21, d26
	vadd.s16	d8, d20, d27
	vsub.s16	d7, d20, d27

	;	tmp10 = tmp0 + tmp3;
	;	tmp13 = tmp0 - tmp3;
	;	tmp11 = tmp1 + tmp2;
	;	tmp12 = tmp1 - tmp2;
	
	vadd.s16	d10, d8, d3			;tmp10
	vsub.s16	d13, d8, d3			;tmp13
	vadd.s16	d11, d1, d2			;tmp11
	vsub.s16	d12, d1, d2			;tmp12
	
	;ddata = tmp10 + tmp11;
	;qval = qvaltab[0];
	;invqval = invqvaltab[0];
	;Quant(0);
	;/*if(abs(ddata) < qval)					
	;{										
		;dataptr[0] = 0;						
	;}										
	;else									
	;{										
		;ddata = ddata * invqval >> 16;		
		;dataptr[0] = ddata;					
	;}*/
	vld1.32		d28, [r1]     ; qval[0]
	vld1.32		d29, [r2]     ; invqval[0]	
	vadd.s16	d15, d10, d11		;ddata
	vabs.s16	d31, d15            ;abs(ddata)
	vcge.s16	d31, d31, d28       ;>=		
	vmull.s16	 q8,  d15, d29  ;	ddata * invqval	
	vqshrn.s32	 d20, q8,  #16	;	>> 16   right?	
	vand.s16	 d20, d20, d31  
	
	; ddata = tmp10 - tmp11;
	; qval = qvaltab[32];
	; invqval = invqvaltab[32];
	; Quant(32);
	add         r3,r1 ,#64
	add         r4,r2 ,#64	
	vld1.32		d28, [r3]     ; qval[32]
	vld1.32		d29, [r4]     ; invqval[32]
	
	vsub.s16	d15, d10, d11		;ddata
	vabs.s16	d31, d15            ;abs(ddata)
	vcge.s16	d31, d31, d28       ;>=	 mask	
	vmull.s16	 q8,  d15, d29  ;	ddata * invqval	
	vqshrn.s32	 d24, q8,  #16	;	>> 16   right?	
	vand.s16	 d24, d24, d31  
	
	;z1 = (((tmp12 + tmp13) * FIX_0_707106781) >> 8); 
	;ddata = tmp13 + z1;
	;qval = qvaltab[16];
	;invqval = invqvaltab[16];
	;Quant(16);
	add         r3,r1 ,#32
	add         r4,r2 ,#32
	vld1.32		d28, [r3]		; qval[16]
	vld1.32		d29, [r4]		; invqval[16]	
	vadd.s16	d15, d12, d13	
	vmull.s16	q8,  d15, d0[2]	;(tmp12 + tmp13) * FIX_0_707106781
	vshr.s32	q8, q8, #8
	vqshrn.s32	d15, q8,  #0	;	      right?	
	;vshr.s16	d15, d15, #8	;	z1>>8      right?
	
	vadd.s16	d14,  d15, d13	     ;d14 = ddata
	vabs.s16	d31,  d14
	vcge.s16	 d31, d31, d28      ;>=	 mask	
	vmull.s16	 q8,  d14, d29		;	ddata * invqval	
	vqshrn.s32	 d22, q8,  #16		;	>> 16   right?	
	vand.s16	 d22, d22, d31   
	
	;ddata = tmp13 - z1;
	;qval = qvaltab[48];
	;invqval = invqvaltab[48];
	;Quant(48);	
	add         r3,r1 ,#96
	add         r4,r2 ,#96
	vld1.32		d28, [r3]     ; qval[48]
	vld1.32		d29, [r4]     ; invqval[48]		
	vsub.s16	d14,  d13, d15	    ;  d14 = ddata (tmp13-z1)
	vabs.s16	d31,  d14
	vcge.s16	 d31, d31, d28      ;	>=	 mask	
	vmull.s16	 q8,  d14, d29		;	ddata * invqval	
	vqshrn.s32	 d26, q8,  #16		;	>> 16   right?	
	vand.s16	 d26, d26, d31 
	
	;tmp10 = tmp4 + tmp5;
	;tmp11 = tmp5 + tmp6;
	;tmp12 = tmp6 + tmp7;	
	
	vadd.s16	d10, d4, d5			;tmp10
	vadd.s16	d11, d5, d6			;tmp11
	vadd.s16	d12, d6, d7			;tmp12	
			
	;z5 = (((tmp10 - tmp12) * FIX_0_382683433) >> 8);
	vsub.s16    d31, d10,d12
	vmull.s16	q8,  d31, d0[0]
	vshr.s32	q8, q8, #8
	vqshrn.s32	d31, q8,  #0	;	      right?	
	;vshr.s16	d31, d31, #8	;	z5>>8      right?
	 
	;z2 = ((tmp10 * FIX_0_541196100) >> 8) + z5; 	
	vmull.s16	q8,  d10, d0[1]
	vshr.s32	q8, q8, #8
	vqshrn.s32	d15, q8,  #0	;	      right?	
	;vshr.s16	d15, d15, #8	;
	vadd.s16    d15, d15, d31      ;z2 = d15
	
	;z4 = ((tmp12 * FIX_1_306562965) >> 8) + z5;
	vmull.s16	q8,  d12, d0[3]
	vshr.s32	q8, q8, #8
	vqshrn.s32	d14, q8,  #0	;	      right?	
	;vshr.s16	d14, d14, #8	;	
	
	vadd.s16    d14, d14, d31      ;z4 = d14
	
	;z3 = ((tmp11 * FIX_0_707106781) >> 8);	
	vmull.s16	q8,  d11, d0[2]
	vshr.s32	q8, q8, #8	
	vqshrn.s32	d31, q8,  #0	;	      right?	
	;vshr.s16	d31, d31, #8	;  z3
			
	vadd.s16    d11, d7,d31        ;z11 = tmp7 + z3;
	vsub.s16    d13, d7,d31        ;z13 = tmp7 - z3;
	
	;ddata = z13 + z2;
	;qval = qvaltab[40];
	;invqval = invqvaltab[40];
	;/*if(abs(ddata) < qval)					
	;{										
		;dataptr[40] = 0;						
	;}										
	;else									
	;{										
		;ddata = ddata * invqval >> 16;		
		;dataptr[40] = ddata;					
	;}*/
	add         r3,r1 ,#80
	add         r4,r2 ,#80
	vld1.32		d28, [r3]		; qval[40]
	vld1.32		d29, [r4]		; invqval[40]
	vadd.s16    d12, d13, d15	    ; ddata	
	vabs.s16	d31,  d12           ; abs(ddata)
	vcge.s16	 d31, d31, d28      ; >=	 mask	
	vmull.s16	 q8,  d12, d29		; ddata * invqval	
	vqshrn.s32	 d25, q8,  #16		; >> 16   right?	
	vand.s16	 d25, d25, d31      ;dataptr[40]
	
	;ddata = z13 - z2;
	;qval = qvaltab[24];
	;invqval = invqvaltab[24];
	;Quant(24);
	add         r3,r1 ,#48
	add         r4,r2 ,#48
	vld1.32		d28, [r3]		; qval[24]
	vld1.32		d29, [r4]		; invqval[24]	
	vsub.s16    d12, d13, d15	    ; ddata	
	vabs.s16	d31,  d12           ; abs(ddata)
	vcge.s16	 d31, d31, d28      ; >=	 mask	
	vmull.s16	 q8,  d12, d29		; ddata * invqval	
	vqshrn.s32	 d23, q8,  #16		; >> 16   right?	
	vand.s16	 d23, d23, d31      ;dataptr[24]
	
	;ddata = z11 + z4;
	;qval = qvaltab[8];
	;invqval = invqvaltab[8];
	;Quant(8);
	add         r3,r1 ,#16
	add         r4,r2 ,#16
	vld1.32		d28, [r3]		; qval[8]
	vld1.32		d29, [r4]		; invqval[8]	
	vadd.s16    d12, d11, d14	    ; ddata	
	vabs.s16	d31,  d12           ; abs(ddata)
	vcge.s16	 d31, d31, d28      ; >=	 mask	
	vmull.s16	 q8,  d12, d29		; ddata * invqval	
	vqshrn.s32	 d21, q8,  #16		; >> 16   right?	
	vand.s16	 d21, d21, d31      ;dataptr[8]
	
	;ddata = z11 - z4;
	;qval = qvaltab[56];
	;invqval = invqvaltab[56];
	;Quant(56);
	add         r3,r1 ,#112
	add         r4,r2 ,#112
	vld1.32		d28, [r3]		; qval[56]
	vld1.32		d29, [r4]		; invqval[56]	
	vsub.s16    d12, d11, d14	    ; ddata	
	vabs.s16	d31,  d12           ; abs(ddata)
	vcge.s16	 d31, d31, d28      ; >=	 mask	
	vmull.s16	 q8,  d12, d29		; ddata * invqval	
	vqshrn.s32	 d27, q8,  #16		; >> 16   right?	
	vand.s16	 d27, d27, d31      ;dataptr[56]
	
	add         r1,r1,#8         ;222
	add         r2,r2,#8         ;222

	vst1.32		d20, [r0], r5
	vst1.32		d21, [r0], r5
	vst1.32		d22, [r0], r5
	vst1.32		d23, [r0], r5
	vst1.32		d24, [r0], r5	
	vst1.32		d25, [r0], r5
	vst1.32		d26, [r0], r5
	vst1.32		d27, [r0]
	subgt		r0, r0, #128-16-8 ;222
	
	bgt			COL_LOOP
	
	;mov		pc, lr 
	ldmia   	sp!, {r4 - r12, pc} 
	ENDP	;fdct_int32


	END