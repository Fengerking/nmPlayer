/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#include "voMpeg4ACDC.h"
#include "voMpeg4MB.h"


void RescuePredict(VO_MPEG4_DEC* pDec, VO_S32 pos) 
{
	VO_S32 lumpos = 2*POSX(pos) + (POSY(pos) << (MB_X2+2));

	if (pos>=MB_X+1 && (pDec->frameMap[pos-MB_X-1] & RESCUE)) {
		// rescue -A- DC value
		pDec->dc_lum[(lumpos-MB_X*2) & DC_LUM_MASK] = 1024;
		pDec->dc_chr[0][(pos-MB_X) & DC_CHR_MASK] = 1024;
		pDec->dc_chr[1][(pos-MB_X) & DC_CHR_MASK] = 1024;
	}

	// left
	if (pos>=1 && (pDec->frameMap[pos-1] & RESCUE)) {
		// rescue -B- DC values
		pDec->dc_lum[(lumpos) & DC_LUM_MASK] = 1024;
		pDec->dc_lum[(lumpos+MB_X*2) & DC_LUM_MASK] = 1024;
		pDec->dc_chr[0][pos & DC_CHR_MASK] = 1024;
		pDec->dc_chr[1][pos & DC_CHR_MASK] = 1024;
		
		if (pDec->bACPred) {
			// rescue -B- AC values
			clear32(pDec->ac_left_lum);
			clear32(pDec->ac_left_chr);
		}
	}

	// top
	if (pos>=MB_X && (pDec->frameMap[pos-MB_X] & RESCUE)) {
		
		// rescue -C- DC values
		pDec->dc_lum[(lumpos+1-MB_X*2) & DC_LUM_MASK] = 1024;
		pDec->dc_lum[(lumpos+2-MB_X*2) & DC_LUM_MASK] = 1024;
		pDec->dc_chr[0][(pos+1-MB_X) & DC_CHR_MASK] = 1024;
		pDec->dc_chr[1][(pos+1-MB_X) & DC_CHR_MASK] = 1024;
		
		if (pDec->bACPred) {
			// rescue -C- AC values
			clear32(pDec->ac_top_lum[lumpos & (MB_X*2-1)]);
			clear32(pDec->ac_top_chr[pos & (MB_X-1)]);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//predict MV

#define MVTAB_0		0
#define MVTAB_1		14
#define MVTAB_2		14+96

#define _MV(mv,nBits) (mv<<8)|(nBits+1)

VOCONST VO_S16 MVtab[14+96+124] = {
//MVTAB_0
_MV(3,4),_MV(-3,4),_MV(2,3),_MV(2,3),_MV(-2,3),_MV(-2,3),_MV(1,2),_MV(1,2),
_MV(1,2),_MV(1,2),_MV(-1,2),_MV(-1,2),_MV(-1,2),_MV(-1,2),
//MVTAB_1
_MV(12,10),_MV(-12,10),
_MV(11,10),_MV(-11,10),_MV(10,9),_MV(10,9),_MV(-10,9),_MV(-10,9),_MV(9,9),_MV(9,9),
_MV(-9,9),_MV(-9,9),_MV(8,9),_MV(8,9),_MV(-8,9),_MV(-8,9),_MV(7,7),_MV(7,7),
_MV(7,7),_MV(7,7),_MV(7,7),_MV(7,7),_MV(7,7),_MV(7,7),_MV(-7,7),_MV(-7,7),
_MV(-7,7),_MV(-7,7),_MV(-7,7),_MV(-7,7),_MV(-7,7),_MV(-7,7),_MV(6,7),_MV(6,7),
_MV(6,7),_MV(6,7),_MV(6,7),_MV(6,7),_MV(6,7),_MV(6,7),_MV(-6,7),_MV(-6,7),
_MV(-6,7),_MV(-6,7),_MV(-6,7),_MV(-6,7),_MV(-6,7),_MV(-6,7),_MV(5,7),_MV(5,7),
_MV(5,7),_MV(5,7),_MV(5,7),_MV(5,7),_MV(5,7),_MV(5,7),_MV(-5,7),_MV(-5,7),
_MV(-5,7),_MV(-5,7),_MV(-5,7),_MV(-5,7),_MV(-5,7),_MV(-5,7),_MV(4,6),_MV(4,6),
_MV(4,6),_MV(4,6),_MV(4,6),_MV(4,6),_MV(4,6),_MV(4,6),_MV(4,6),_MV(4,6),
_MV(4,6),_MV(4,6),_MV(4,6),_MV(4,6),_MV(4,6),_MV(4,6),_MV(-4,6),_MV(-4,6),
_MV(-4,6),_MV(-4,6),_MV(-4,6),_MV(-4,6),_MV(-4,6),_MV(-4,6),_MV(-4,6),_MV(-4,6),
_MV(-4,6),_MV(-4,6),_MV(-4,6),_MV(-4,6),_MV(-4,6),_MV(-4,6),
//MVTAB_2
_MV(32,12),_MV(-32,12),
_MV(31,12),_MV(-31,12),_MV(30,11),_MV(30,11),_MV(-30,11),_MV(-30,11),_MV(29,11),_MV(29,11),
_MV(-29,11),_MV(-29,11),_MV(28,11),_MV(28,11),_MV(-28,11),_MV(-28,11),_MV(27,11),_MV(27,11),
_MV(-27,11),_MV(-27,11),_MV(26,11),_MV(26,11),_MV(-26,11),_MV(-26,11),_MV(25,11),_MV(25,11),
_MV(-25,11),_MV(-25,11),_MV(24,10),_MV(24,10),_MV(24,10),_MV(24,10),_MV(-24,10),_MV(-24,10),
_MV(-24,10),_MV(-24,10),_MV(23,10),_MV(23,10),_MV(23,10),_MV(23,10),_MV(-23,10),_MV(-23,10),
_MV(-23,10),_MV(-23,10),_MV(22,10),_MV(22,10),_MV(22,10),_MV(22,10),_MV(-22,10),_MV(-22,10),
_MV(-22,10),_MV(-22,10),_MV(21,10),_MV(21,10),_MV(21,10),_MV(21,10),_MV(-21,10),_MV(-21,10),
_MV(-21,10),_MV(-21,10),_MV(20,10),_MV(20,10),_MV(20,10),_MV(20,10),_MV(-20,10),_MV(-20,10),
_MV(-20,10),_MV(-20,10),_MV(19,10),_MV(19,10),_MV(19,10),_MV(19,10),_MV(-19,10),_MV(-19,10),
_MV(-19,10),_MV(-19,10),_MV(18,10),_MV(18,10),_MV(18,10),_MV(18,10),_MV(-18,10),_MV(-18,10),
_MV(-18,10),_MV(-18,10),_MV(17,10),_MV(17,10),_MV(17,10),_MV(17,10),_MV(-17,10),_MV(-17,10),
_MV(-17,10),_MV(-17,10),_MV(16,10),_MV(16,10),_MV(16,10),_MV(16,10),_MV(-16,10),_MV(-16,10),
_MV(-16,10),_MV(-16,10),_MV(15,10),_MV(15,10),_MV(15,10),_MV(15,10),_MV(-15,10),_MV(-15,10),
_MV(-15,10),_MV(-15,10),_MV(14,10),_MV(14,10),_MV(14,10),_MV(14,10),_MV(-14,10),_MV(-14,10),
_MV(-14,10),_MV(-14,10),_MV(13,10),_MV(13,10),_MV(13,10),_MV(13,10),_MV(-13,10),_MV(-13,10),
_MV(-13,10),_MV(-13,10)
};

static INLINE VO_S32 getMVData( VO_MPEG4_DEC* pDec, const VO_S16* table, VO_S32 fcode ) //max 13bits
{
	VO_S32 code,res;
	UPDATE_CACHE(pDec);

	code = SHOW_BITS(pDec,13);

	if (code >= 4096) {
		FLUSH_BITS(pDec,1);
		return 0;
	}
	
	if (code >= 512) 
		code = (code >> 8) - 2 + MVTAB_0;
	else if (code >= 128) 
		code = (code >> 2) - 32 + MVTAB_1;
	else
		code = code-4 + MVTAB_2; 

	code = table[code];
	FLUSH_BITS_LARGE(pDec,code & 255);
	code >>= 8;

	if (fcode) {
		code <<= fcode;
		res = GetBits(pDec,fcode);
		res -= (1 << fcode) - 1;
		if (code < 0)
			res = -res;
		code += res;
	}

	return code;
}

#define MEDIAN(p1, p2, p3) 	(p1 > p2 ? (p1 > p3 ? max(p2,p3) : p1) : (p2 > p3 ? max(p1,p3) : p2))
	

VO_S32 GetPMV(VO_S32 Block_Num,VO_S32 pos, VO_S32 slice_pos, VO_MPEG4_DEC *pDec)
{
	VO_S32 p1, p2, p3, temp;
	VO_S32 nResyncPos = pDec->nResyncPos;
	VO_S32* mv = &pDec->mv_buf[(pos & pDec->mv_bufmask)*4];
	VO_S32* mvlast = &pDec->mv_buf[((pos-MB_X) & pDec->mv_bufmask)*4];

	// we are allowed to access borders on left and right (always zero)
	pos -= slice_pos;
	switch (Block_Num)
	{
		case 0:
			if (pos == nResyncPos) 
				return 0;
			if (pos < nResyncPos+MB_X) 
			{
				if (pos != nResyncPos+MB_X-1)
					return mv[-4+1];

				if (POSX(pos)==0)
					return mvlast[4+2];

				p1 = mv[-4+1];
				p2 = 0;
				p3 = mvlast[4+2];
				break;
			}
// 			if (POSX(pos)==0)
// 			{
// 				p1 = 0;
// 			}
// 			else
// 			{
				p1 = mv[-4+1];
// 			}
			p2 = mvlast[2];
			p3 = mvlast[4+2];
			break;
		case 1:
			if (pos < nResyncPos+MB_X) 
			{
				if (pos != nResyncPos+MB_X-1)
					return mv[0];

				p1 = mv[0];
				p2 = 0;
				p3 = mvlast[4+2];
				break;
			}
			p1 = mv[0];
			p2 = mvlast[3];
			p3 = mvlast[4+2];
			break;	
		case 2:
			p1 = mv[-4+3];
			p2 = mv[0];
			p3 = mv[1];
			if (pos == nResyncPos)
				p1 = 0;
			break;
		default: // case 3
			p1 = mv[2];
			p2 = mv[0];
			p3 = mv[1];
			break;
	}

// 	temp = min(max(p1, p2), min(max(p2, p3), max(p1, p3)));
	temp = MEDIAN(p1,p2,p3);
	p1=(p1<<16)>>16;
	p2=(p2<<16)>>16;
	p3=(p3<<16)>>16;
// 	p1 = min(max(p1, p2), min(max(p2, p3), max(p1, p3)));
	p1 = MEDIAN(p1,p2,p3);

	return ((temp&0xFFFF0000)|(p1&0xFFFF));
}

static INLINE VO_S32 Div2Round(VO_S32 mv1, VO_S32 mv2)
{
	VO_S32 t1, t2;
	t1 = mv1<<16>>16;
	t2 = mv2<<16>>16;
	mv1>>=16;
	mv2>>=16;
	t1 = DIV2ROUND(t1+t2);
	mv1 = DIV2ROUND(mv1+mv2);
	return MAKEMV(t1,mv1);
}

VO_S32 GetPMV_interlace(VO_S32 Block_Num,VO_S32 pos, VO_S32 slice_pos, VO_MPEG4_DEC *pDec)
{
	VO_S32 p1, p2, p3, temp;
	VO_S32 nResyncPos = pDec->nResyncPos;
	VO_S32* mv = &pDec->mv_buf[(pos & pDec->mv_bufmask)*4];
	VO_S8 *bFieldPred = pDec->bFieldPred_buf+pos;
	VO_S32* mvlast = &pDec->mv_buf[((pos-MB_X) & pDec->mv_bufmask)*4];
	VO_S8 *bFieldPredlast = pDec->bFieldPred_buf+pos-MB_X;

	// we are allowed to access borders on left and right (always zero)
	pos -= slice_pos;
	switch (Block_Num)
	{
		case 0:
			if (pos == nResyncPos) 
				return 0;
			if (pos < nResyncPos+MB_X) 
			{
				if (pos != nResyncPos+MB_X-1)
				{
					if (bFieldPred[-1])
					{
						return Div2Round(mv[-4],mv[-2]);
					}
					return mv[-4+1];
				}

				if (POSX(pos)==0)
				{
					if (bFieldPredlast[1])
					{
						return Div2Round(mvlast[4],mvlast[6]);
					}
					return mvlast[4+2];
				}

				p1 = bFieldPred[-1] ? Div2Round(mv[-4],mv[-2]): mv[-4+1];
				p2 = 0;
				p3 = bFieldPredlast[1] ? Div2Round(mvlast[4],mvlast[6]): mvlast[4+2];
				break;
			}
			p1 = bFieldPred[-1] ? Div2Round(mv[-4],mv[-2]): mv[-4+1];
			p2 = bFieldPredlast[0] ? Div2Round(mvlast[0],mvlast[2]): mvlast[2];
			p3 = bFieldPredlast[1] ? Div2Round(mvlast[4],mvlast[6]): mvlast[4+2];
			break;
		case 1:
			if (pos < nResyncPos+MB_X) 
			{
				if (pos != nResyncPos+MB_X-1)
					return mv[0];

				p1 = mv[0];
				p2 = 0;
				p3 = bFieldPredlast[1] ? Div2Round(mvlast[4],mvlast[6]): mvlast[4+2];
				break;
			}
			p1 = mv[0];
			p2 = bFieldPredlast[0] ? Div2Round(mvlast[0],mvlast[2]): mvlast[3];
			p3 = bFieldPredlast[1] ? Div2Round(mvlast[4],mvlast[6]): mvlast[4+2];
			break;	
		case 2:
			if (pos == nResyncPos)
				p1 = 0;
			else
				p1 = bFieldPred[-1] ? Div2Round(mv[-4],mv[-2]): mv[-4+3];
			p2 = mv[0];
			p3 = mv[1];
			break;
		default: // case 3
			p1 = mv[2];
			p2 = mv[0];
			p3 = mv[1];
			break;
	}

// 	temp = min(max(p1, p2), min(max(p2, p3), max(p1, p3)));
	temp = MEDIAN(p1,p2,p3);
	p1=(p1<<16)>>16;
	p2=(p2<<16)>>16;
	p3=(p3<<16)>>16;
// 	p1 = min(max(p1, p2), min(max(p2, p3), max(p1, p3)));
	p1 = MEDIAN(p1,p2,p3);

	return ((temp&0xFFFF0000)|(p1&0xFFFF));
}

#if 1
VO_S32 GetMV(VO_S32 fcode,VO_S32 prev, VO_MPEG4_DEC* pDec, VO_S32 bField/*, VO_S32 nMBPos*/)
{
	VO_S32 high,low,range;
	VO_S32 mv_x, mv_y;

	--fcode;

	mv_x = getMVData(pDec,MVtab,fcode);
	mv_y = getMVData(pDec,MVtab,fcode); 


	range = 1 << (fcode+6);
	high = (range >> 1) - 1;
	low = -(range >> 1);

	mv_x += MVX(prev, 0);
	
	if (mv_x < low){
		mv_x += range;
	}
	
	if (mv_x > high){
		mv_x -= range;
	}
		
	if (!bField)
	{
		mv_y += MVY(prev, 0);
	}
	else
	{
		mv_y += MVY(prev, 0)/2;
		mv_y <<= 1;
	}
	
	if (mv_y < low){
		mv_y += range;
	}
	
	if (mv_y > high){
		mv_y -= range;
	}

#if 0
	{
		VO_S32 nXMBPos = POSX(nMBPos);
		VO_S32 nYMBPos = POSY(nMBPos);

		range = 5 + pDec->qpel;
		high = (VO_S32)(pDec->nMBWidth - nXMBPos) << range;
		low	 = (-(VO_S32)nXMBPos -1 ) << range;

		if (mv_x > high) {  
			mv_x = high; 
		} else if (mv_x < low) {
			mv_x = low;
		} 
		high = (VO_S32)(pDec->nMBHeight - nYMBPos) << range;
		low	 = (-(VO_S32)nYMBPos -1 ) << range;
	}

	if (mv_y > high) {  
		mv_y = high; 
	} else if (mv_y < low) {
		mv_y = low;
	} 

#endif
	return MAKEMV(mv_x,mv_y);
}
#else
VO_S32 GetMV(VO_S32 fcode,VO_S32 prev, VO_MPEG4_DEC* pDec)
{
	FILE *mvdebug;
	VO_S32 high,low,range;
	VO_S32 mv_x, mv_y;

	mvdebug = fopen("e:/mvdebug.txt", "a+");

	--fcode;

	mv_x = getMVData(pDec,MVtab,fcode);
	mv_y = getMVData(pDec,MVtab,fcode); 


	range = 1 << (fcode+6);
	high = (range >> 1) - 1;
	low = -(range >> 1);

	mv_x += MVX(prev);
//	fprintf(mvdebug,"\n mvx %d pmvx %d", mv_x, MVX(prev));

	if (mv_x < low)
		mv_x += range;
	
	if (mv_x > high)
		mv_x -= range;
		
	mv_y += MVY(prev);
//	fprintf(mvdebug,"\n mvy %d pmvy %d", mv_y, MVY(prev));
	
	if (mv_y < low)
		mv_y += range;
	
	if (mv_y > high)
		mv_y -= range;

	fclose(mvdebug);
	return MAKEMV(mv_x,mv_y);
}
#endif

VO_S32 CorrectMV(VO_S32 MV, VO_MPEG4_DEC* pDec, VO_S32 nXMBPos, VO_S32 nYMBPos)
{
	VO_S32 high,low,range;
	VO_S32 mv_x, mv_y;

	mv_x = MVX(MV, 0);
	mv_y = MVY(MV, 0);

	range = 5 + pDec->qpel;
	high = (VO_S32)(pDec->nMBWidth - nXMBPos) << range;
	low	 = (-(VO_S32)nXMBPos -1 ) << range;

	if (mv_x > high) {  
		mv_x = high; 
	} else if (mv_x < low) {
		mv_x = low;
	} 
	high = (VO_S32)(pDec->nMBHeight - nYMBPos) << range;
	low	 = (-(VO_S32)nYMBPos -1 ) << range;

	if (mv_y > high) {  
		mv_y = high; 
	} else if (mv_y < low) {
		mv_y = low;
	} 


	return MAKEMV(mv_x,mv_y);
}
