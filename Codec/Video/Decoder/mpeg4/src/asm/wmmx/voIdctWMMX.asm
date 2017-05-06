;************************************************************************
;									                                    *
;	VisualOn, Inc. Confidential and Proprietary, 2005		            *
;								 	                                    *
;***********************************************************************/

	AREA	|.text|, CODE

	EXPORT WmmxIdctC
;	EXPORT wmmx_Add_Block
;	EXPORT wmmx_Add_Block2
;	EXPORT wmmx_CopyBlock16x16
;	EXPORT wmmx_CopyBlock8x8
	ALIGN 16

	ALIGN 16
;WmmxIdctC(int v,VO_U8 * Dst,int DstStride, VO_U8 * Src)
WmmxIdctC PROC
	stmdb	sp!, {r4}		; save regs used
	ldr	r4, [sp, #4]	;SrcStride
	cmp r0,#0
	bgt const8x8add
	blt const8x8sub
	cmp r1,r3
	beq const8x8done

	macro
	const8x8copyrow
	wldrd   wr1,[r3]
	add		r3,r3,r4
	wldrd   wr2,[r3]
	add		r3,r3,r4
	wstrd   wr1,[r1]
	add		r1,r1,r2
	wstrd   wr2,[r1]
	add		r1,r1,r2
	mend
	
	const8x8copyrow
	const8x8copyrow
	const8x8copyrow
	const8x8copyrow

const8x8done
	ldmia	sp!, {r4}		; restore and return 
	mov pc,lr

const8x8add	
	macro
	const8x8addrow
	wldrd   wr1,[r3]
	add		r3,r3,r4
	wldrd   wr2,[r3]
	add		r3,r3,r4
	waddbus wr1,wr1,wr0
	waddbus wr2,wr2,wr0
	wstrd   wr1,[r1]
	add		r1,r1,r2
	wstrd   wr2,[r1]
	add		r1,r1,r2
	mend

	tbcstb  wr0,r0
	const8x8addrow
	const8x8addrow
	const8x8addrow
	const8x8addrow
	ldmia	sp!, {r4}		; restore and return 
	mov pc,lr

const8x8sub
	macro
	const8x8subrow
	wldrd   wr1,[r3]
	add		r3,r3,r4
	wldrd   wr2,[r3]
	add		r3,r3,r4
	wsubbus wr1,wr1,wr0
	wsubbus wr2,wr2,wr0
	wstrd   wr1,[r1]
	add		r1,r1,r2
	wstrd   wr2,[r1]
	add		r1,r1,r2
	mend

	rsb r0,r0,#0
	tbcstb  wr0,r0
	const8x8subrow
	const8x8subrow
	const8x8subrow
	const8x8subrow
	ldmia	sp!, {r4}		; restore and return 
	mov pc,lr

	endp


wmmx_Add_Block
;Add_Block(block, dst, dst_stride, src, src_stride);
;void Add_Block(short *src, unsigned char *dst, int dst_stride, unsigned char *src1, VO_S32 src_stride)
;{
;	int i;
;		for (i=0; i<8; i++)
;		{
;			dst[0] = Clip[src[0] + src1[0]];
;			dst[1] = Clip[src[1] + src1[1]];
;			dst[2] = Clip[src[2] + src1[2]];
;			dst[3] = Clip[src[3] + src1[3]];
;			dst[4] = Clip[src[4] + src1[4]];
;			dst[5] = Clip[src[5] + src1[5]];
;			dst[6] = Clip[src[6] + src1[6]];
;			dst[7] = Clip[src[7] + src1[7]];
;			src += 8;
;			src1 += src_stride;
;			dst += dst_stride;
;		}
;}
	ldr	r12, [sp]	;SrcStride

loop_begain

	wldrd   wr2,[r3]
	add		r3, r3, r12				;src1 += src_stride;
	wldrd   wr6,[r3]
	wldrd   wr0,[r0], #8
	wldrd   wr1,[r0], #8
	wldrd   wr4,[r0], #8
	wldrd   wr5,[r0], #8

	wunpckelub	wr3, wr2
	wunpckehub	wr2, wr2
	waddhss	wr0, wr0, wr3
	waddhss	wr1, wr1, wr2

	wunpckelub	wr7, wr6
	wunpckehub	wr6, wr6
	waddhss	wr4, wr4, wr7
	waddhss	wr5, wr5, wr6

	wpackhus	wr0, wr0, wr1
	wpackhus	wr4, wr4, wr5
	add		r3, r3, r12				;src1 += src_stride;

	wstrd   wr0,[r1]
	add		r1, r1, r2				;dst += dst_stride;
	wstrd   wr4,[r1]
	add		r1, r1, r2				;dst += dst_stride;	


;1
	wldrd   wr2,[r3]
	add		r3, r3, r12				;src1 += src_stride;
	wldrd   wr6,[r3]
	wldrd   wr0,[r0], #8
	wldrd   wr1,[r0], #8
	wldrd   wr4,[r0], #8
	wldrd   wr5,[r0], #8

	wunpckelub	wr3, wr2
	wunpckehub	wr2, wr2
	waddhss	wr0, wr0, wr3
	waddhss	wr1, wr1, wr2

	wunpckelub	wr7, wr6
	wunpckehub	wr6, wr6
	waddhss	wr4, wr4, wr7
	waddhss	wr5, wr5, wr6

	wpackhus	wr0, wr0, wr1
	wpackhus	wr4, wr4, wr5
	add		r3, r3, r12				;src1 += src_stride;

	wstrd   wr0,[r1]
	add		r1, r1, r2				;dst += dst_stride;
	wstrd   wr4,[r1]
	add		r1, r1, r2				;dst += dst_stride;	

;2
	wldrd   wr2,[r3]
	add		r3, r3, r12				;src1 += src_stride;
	wldrd   wr6,[r3]
	wldrd   wr0,[r0], #8
	wldrd   wr1,[r0], #8
	wldrd   wr4,[r0], #8
	wldrd   wr5,[r0], #8

	wunpckelub	wr3, wr2
	wunpckehub	wr2, wr2
	waddhss	wr0, wr0, wr3
	waddhss	wr1, wr1, wr2

	wunpckelub	wr7, wr6
	wunpckehub	wr6, wr6
	waddhss	wr4, wr4, wr7
	waddhss	wr5, wr5, wr6

	wpackhus	wr0, wr0, wr1
	wpackhus	wr4, wr4, wr5
	add		r3, r3, r12				;src1 += src_stride;

	wstrd   wr0,[r1]
	add		r1, r1, r2				;dst += dst_stride;
	wstrd   wr4,[r1]
	add		r1, r1, r2				;dst += dst_stride;	


;3
	wldrd   wr2,[r3]
	add		r3, r3, r12				;src1 += src_stride;
	wldrd   wr6,[r3]
	wldrd   wr0,[r0], #8
	wldrd   wr1,[r0], #8
	wldrd   wr4,[r0], #8
	wldrd   wr5,[r0], #8

	wunpckelub	wr3, wr2
	wunpckehub	wr2, wr2
	waddhss	wr0, wr0, wr3
	waddhss	wr1, wr1, wr2

	wunpckelub	wr7, wr6
	wunpckehub	wr6, wr6
	waddhss	wr4, wr4, wr7
	waddhss	wr5, wr5, wr6

	wpackhus	wr0, wr0, wr1
	wpackhus	wr4, wr4, wr5
	add		r3, r3, r12				;src1 += src_stride;

	wstrd   wr0,[r1]
	add		r1, r1, r2				;dst += dst_stride;
	wstrd   wr4,[r1]
	add		r1, r1, r2				;dst += dst_stride;	
	mov pc,lr


wmmx_Add_Block2
;Add_Block(block, dst, dst_stride, src, src_stride);
;void Add_Block(short *src, unsigned char *dst, int dst_stride)
;{
;	int i;
;		for (i=0; i<8; i++)
;		{
;			dst[0] = Clip[src[0]];
;			dst[1] = Clip[src[1]];
;			dst[2] = Clip[src[2]];
;			dst[3] = Clip[src[3]];
;			dst[4] = Clip[src[4]];
;			dst[5] = Clip[src[5]];
;			dst[6] = Clip[src[6]];
;			dst[7] = Clip[src[7]];
;			src += 8;
;			dst += dst_stride;
;		}
;}

	wldrd   wr0,[r0], #8
	wldrd   wr1,[r0], #8
	wldrd   wr2,[r0], #8
	wldrd   wr3,[r0], #8
	wldrd   wr4,[r0], #8
	wldrd   wr5,[r0], #8
	wldrd   wr6,[r0], #8
	wldrd   wr7,[r0], #8

	wpackhus	wr0, wr0, wr1
	wpackhus	wr2, wr2, wr3
	wpackhus	wr4, wr4, wr5
	wpackhus	wr6, wr6, wr7

	wstrd   wr0,[r1]
	add		r1, r1, r2				;dst += dst_stride;
	wstrd   wr2,[r1]
	add		r1, r1, r2				;dst += dst_stride;	
	wstrd   wr4,[r1]
	add		r1, r1, r2				;dst += dst_stride;
	wstrd   wr6,[r1]
	add		r1, r1, r2				;dst += dst_stride;	

;1
	wldrd   wr0,[r0], #8
	wldrd   wr1,[r0], #8
	wldrd   wr2,[r0], #8
	wldrd   wr3,[r0], #8
	wldrd   wr4,[r0], #8
	wldrd   wr5,[r0], #8
	wldrd   wr6,[r0], #8
	wldrd   wr7,[r0], #8

	wpackhus	wr0, wr0, wr1
	wpackhus	wr2, wr2, wr3
	wpackhus	wr4, wr4, wr5
	wpackhus	wr6, wr6, wr7

	wstrd   wr0,[r1]
	add		r1, r1, r2				;dst += dst_stride;
	wstrd   wr2,[r1]
	add		r1, r1, r2				;dst += dst_stride;	
	wstrd   wr4,[r1]
	add		r1, r1, r2				;dst += dst_stride;
	wstrd   wr6,[r1]
	add		r1, r1, r2				;dst += dst_stride;	

	mov pc,lr

wmmx_CopyBlock16x16
;// Dst[p] = Src[p]
;void CCopy16X16(VO_U8 *Src, VO_U8 *Dst, const VO_S32 SrcStride, const VO_S32 DstStride)
;{
;	VO_U8 *SrcEnd = Src + 16*SrcStride;
;	VO_U32 a,b,c,d;
;	do
;	{
;		a=((VO_U32*)Src)[0];
;		b=((VO_U32*)Src)[1];
;		c=((VO_U32*)Src)[2];
;		d=((VO_U32*)Src)[3];
;		((VO_U32*)Dst)[0]=a;
;		((VO_U32*)Dst)[1]=b;
;		((VO_U32*)Dst)[2]=c;
;		((VO_U32*)Dst)[3]=d;
;		Dst += DstStride;
;		Src += SrcStride;
;	}
;	while (Src != SrcEnd);
;}
;0
	wldrd   wr0,[r0]
	wldrd   wr1,[r0, #8]
	add		r0, r0, r2
	wldrd   wr2,[r0]
	wldrd   wr3,[r0, #8]
	add		r0, r0, r2
	wldrd   wr4,[r0]
	wldrd   wr5,[r0, #8]
	add		r0, r0, r2
	wldrd   wr6,[r0]
	wldrd   wr7,[r0, #8]
	add		r0, r0, r2


	wstrd   wr0,[r1]
	wstrd   wr1,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr2,[r1]
	wstrd   wr3,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr4,[r1]
	wstrd   wr5,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr6,[r1]
	wstrd   wr7,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
;1
	wldrd   wr0,[r0]
	wldrd   wr1,[r0, #8]
	add		r0, r0, r2
	wldrd   wr2,[r0]
	wldrd   wr3,[r0, #8]
	add		r0, r0, r2
	wldrd   wr4,[r0]
	wldrd   wr5,[r0, #8]
	add		r0, r0, r2
	wldrd   wr6,[r0]
	wldrd   wr7,[r0, #8]
	add		r0, r0, r2


	wstrd   wr0,[r1]
	wstrd   wr1,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr2,[r1]
	wstrd   wr3,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr4,[r1]
	wstrd   wr5,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr6,[r1]
	wstrd   wr7,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
;2
	wldrd   wr0,[r0]
	wldrd   wr1,[r0, #8]
	add		r0, r0, r2
	wldrd   wr2,[r0]
	wldrd   wr3,[r0, #8]
	add		r0, r0, r2
	wldrd   wr4,[r0]
	wldrd   wr5,[r0, #8]
	add		r0, r0, r2
	wldrd   wr6,[r0]
	wldrd   wr7,[r0, #8]
	add		r0, r0, r2


	wstrd   wr0,[r1]
	wstrd   wr1,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr2,[r1]
	wstrd   wr3,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr4,[r1]
	wstrd   wr5,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr6,[r1]
	wstrd   wr7,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
;3
	wldrd   wr0,[r0]
	wldrd   wr1,[r0, #8]
	add		r0, r0, r2
	wldrd   wr2,[r0]
	wldrd   wr3,[r0, #8]
	add		r0, r0, r2
	wldrd   wr4,[r0]
	wldrd   wr5,[r0, #8]
	add		r0, r0, r2
	wldrd   wr6,[r0]
	wldrd   wr7,[r0, #8]
	add		r0, r0, r2


	wstrd   wr0,[r1]
	wstrd   wr1,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr2,[r1]
	wstrd   wr3,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr4,[r1]
	wstrd   wr5,[r1, #8]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr6,[r1]
	wstrd   wr7,[r1, #8]	
	mov pc,lr

;// Dst[p] = Src[p]
;void CCopy8X8(VO_U8 *Src, VO_U8 *Dst, const VO_S32 SrcStride, const VO_S32 DstStride)
;{
;	VO_U8 *SrcEnd = Src + 8*SrcStride;
;	VO_U32 a,b,c,d;
;	do
;	{
;		a=((VO_U32*)Src)[0];
;		b=((VO_U32*)Src)[1];
;		Src += SrcStride;
;		c=((VO_U32*)Src)[0];
;		d=((VO_U32*)Src)[1];
;		Src += SrcStride;
;		((VO_U32*)Dst)[0]=a;
;		((VO_U32*)Dst)[1]=b;
;		Dst += DstStride;
;		((VO_U32*)Dst)[0]=c;
;		((VO_U32*)Dst)[1]=d;
;		Dst += DstStride;
;	}
;	while (Src != SrcEnd);
;}
wmmx_CopyBlock8x8

	wldrd   wr0,[r0]
	add		r0, r0, r2
	wldrd   wr1,[r0]
	add		r0, r0, r2
	wldrd   wr2,[r0]
	add		r0, r0, r2
	wldrd   wr3,[r0]
	add		r0, r0, r2
	wldrd   wr4,[r0]
	add		r0, r0, r2
	wldrd   wr5,[r0]
	add		r0, r0, r2
	wldrd   wr6,[r0]
	add		r0, r0, r2
	wldrd   wr7,[r0]
	add		r0, r0, r2

	wstrd   wr0,[r1]
	add		r1, r1, r3				;dst += dst_stride;
	wstrd   wr1,[r1]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr2,[r1]
	add		r1, r1, r3				;dst += dst_stride;
	wstrd   wr3,[r1]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr4,[r1]
	add		r1, r1, r3				;dst += dst_stride;
	wstrd   wr5,[r1]
	add		r1, r1, r3				;dst += dst_stride;	
	wstrd   wr6,[r1]
	add		r1, r1, r3				;dst += dst_stride;
	wstrd   wr7,[r1]

	mov pc,lr
	end
