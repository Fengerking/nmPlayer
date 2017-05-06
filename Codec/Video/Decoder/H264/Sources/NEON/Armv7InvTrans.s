	AREA	|text|, CODE
	
	EXPORT itrans_asm
	
itrans_asm PROC
;	avdNativeInt i,j,a,b,c,d;
;	avdNativeInt *m7 = &img->m7[0][0];
;	avdUInt8 *clip255 = &img->clip255[0];
;	StartRunTimeClock(DT_ITRANS);	
;	// horizontal, notice that coef[i][j], i & j reversed; 
;	i = BLOCK_SIZE;
;	do {
;		a = coef[0] + coef[2];
;		b = coef[0] - coef[2];
;		c = (coef[1]>>1) - coef[3];
;		d = coef[1] + (coef[3]>>1);		
;		m7[ 4-i] = a + d;
;		m7[ 8-i] = b + c;
;		m7[12-i] = b - c;
;		m7[16-i] = a - d;
;		coef += 4;
;	} while (--i);

	vld4.16	{d0[0], d1[0], d2[0], d3[0]},[r1]!  
	vld4.16	{d0[1], d1[1], d2[1], d3[1]},[r1]!  
	vld4.16	{d0[2], d1[2], d2[2], d3[2]},[r1]!  
	vld4.16	{d0[3], d1[3], d2[3], d3[3]},[r1]
	
	vaddl.s16	q3, d0, d2		;a
	vsubl.s16	q4, d0, d2		;b
	vshr.s16	d0, d1, #1
	vshr.s16	d2, d3, #1
	vaddl.s16	q5, d1, d2		;d
	vsubl.s16	q6, d0, d3		;c

	vadd.s32	q0, q3, q5		;img->m7[0] = a + d
	vsub.s32	q3, q3, q5		;img->m7[12] = a - d
	vadd.s32	q1, q4, q6		;img->m7[4] = b + c
	vsub.s32	q2, q4, q6		;img->m7[8] = b - c		
	
;	// vertical
;	j = BLOCK_SIZE;
;	m7[0]  += 32;
;	m7[4]  += 32;
;	m7[8]  += 32;
;	m7[12] += 32;
;	do {
;		a = (m7[0]     +  m7[2]);
;		b = (m7[0]     -  m7[2]);
;		c = (m7[1]>>1) -  m7[3];
;		d = m7[1]      + (m7[3]>>1);
;		// only good for little endian;
;		*(avdUInt32 *)outStart = 
;			(clip255[((a+d)>>6)+outStart[0]]) |
;			(clip255[((b+c)>>6)+outStart[1]]<<8) |
;			(clip255[((b-c)>>6)+outStart[2]]<<16) |
;			(clip255[((a-d)>>6)+outStart[3]]<<24);
;		outStart += outLnLength;
;		m7 += 4;
;	} while (--j);
	         
;	Transpose	
	vtrn.32	q0, q1
	vtrn.32	q2, q3	
	vswp.s32	d1, d4	
	vswp.s32	d3, d6	
	
	vmov.s32	q4, #32		
	vadd.s32	q0, q0, q4		;m7[0]  += 32;
			        
	vadd.s32	q4, q0, q2		;a = (m7[0] +  m7[2]);
	vsub.s32	q5, q0, q2		;b = (m7[0] -  m7[[2]);
	vshr.s32	q0, q1, #1
	vshr.s32	q2, q3, #1	
	vsub.s32	q7, q0, q3		;c = (i7[1]>>1) -  m7[3];
	vadd.s32	q2, q1, q2		;d = m7[1] + (m7[3]>>1);
	

	vadd.s32	q0, q4, q2		;a+d
	vsub.s32	q3, q4, q2		;a-d	
	vadd.s32	q1, q5, q7		;b+c
	vsub.s32	q2, q5, q7		;b-c	
	
;	Transpose	
	vtrn.32	q0, q1
	vtrn.32	q2, q3	
	vswp.s32	d1, d4	
	vswp.s32	d3, d6	
					
	;q6, q2, q4, q7
	
	vshrn.s32	d0, q0, #6
	vshrn.s32	d1, q1, #6
	vshrn.s32	d2, q2, #6		
	vshrn.s32	d3, q3, #6
		
	mov	r3, r0	
	 
	vld1.32	{d4[0]},[r0], r2  
	vld1.32	{d4[1]},[r0], r2 	
	vld1.32	{d6[0]},[r0], r2 
	vld1.32	{d6[1]},[r0]	

	vmovl.u8	q2, d4
	vmovl.u8	q3, d6
	
	vadd.s16	q0, q0, q2
	vadd.s16	q1, q1, q3
	
	vqmovun.s16	d0, q0
	vqmovun.s16	d1, q1
	
	vst1.32	{d0[0]},[r3], r2  
	vst1.32	{d0[1]},[r3], r2 	
	vst1.32	{d1[0]},[r3], r2 
	vst1.32	{d1[1]},[r3]	
	
 	mov		pc,lr    
      
	ENDP
	
	
	END	