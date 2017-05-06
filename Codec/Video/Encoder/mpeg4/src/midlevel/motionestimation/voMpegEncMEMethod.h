/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _ESTIMATION_H_
#define _ESTIMATION_H_

#include "../../voMpegEnc.h"

#define VODIAMONDSIZE 2
// INTER bias for INTER/INTRA decision; mpeg4 spec suggests 2*nb
#define MV16_INTER_BIAS 	450
#define INITIAL_SKIP_THRESH			80
#define ME_EARLY_EXIT_THRESH		110
#define INITIAL_SKIP_THRESH_C		5
#define ME_EARLY_EXIT_THRESH_C		5
// Parameters which control inter/inter4v decision
#define IMV16X16				20 

extern const VO_S32 me_lambda_vec16[32];
/* fast ((A)/2)*2 */
#define VOMVEVEN(D,S)		{(D).x = ((((S).x)<0?((S).x)+1:((S).x)) & ~1); (D).y = ((((S).y)<0?((S).y)+1:((S).y)) & ~1);}
//#define VOMVEVEN(D,S)	 {(D).x = peventbl[(S).x]; (D).y = peventbl[(S).y];}

#define MVEQUAL(A,B) ( ((A).x)==((B).x) && ((A).y)==((B).y) )
static const VECTOR zeroMV = { 0, 0 };

#define UPDATAMEDATA(me_data,sad,current) \
    if (sad[0] < (me_data)->iMinSAD[1]) {\
	    (me_data)->iMinSAD[1] = sad[0]; current[1].x = x; current[1].y = y; }\
	if (sad[1] < (me_data)->iMinSAD[2]) {\
		(me_data)->iMinSAD[2] = sad[1]; current[2].x = x; current[2].y = y; }\
	if (sad[2] < (me_data)->iMinSAD[3]) {\
		(me_data)->iMinSAD[3] = sad[2]; current[3].x = x; current[3].y = y; }\
	if (sad[3] < (me_data)->iMinSAD[4]) {\
		(me_data)->iMinSAD[4] = sad[3]; current[4].x = x; current[4].y = y; }

#define CHECK_CANDIDATE(X,Y,D,F) { \
	CheckCandidate((X),(Y), me_data, (D), (F)); }

typedef struct
{
	VO_S32 max_dx, min_dx, max_dy, min_dy; /* maximum search range */

	/* data modified by CheckCandidates */
	VO_U32 iMinSAD[5];			/* smallest SADs found so far */
	VECTOR current_best_MV[5];		/* best vectors found so far */
	//VO_U32 temp[4];				/* temporary space */ zou delete
	VO_U32 dir;			/* 'direction', set when better vector is found */
	VO_S32 chroma_x, chroma_y, chroma_SAD; /* info to make ChromaSAD faster */

	/* general fields */
	VO_U32 rounding;			/* rounding type in use */
	VECTOR predMV;				/* vector which predicts current vector */

	VO_U8 * RefY, *RefU, *RefV;	/* reference pictures - N, V, H, HV, cU, cV */
	VO_U8 * CurY,* CurU, *CurV;	/* current picture - chroma planes */
	VO_U8 *ref_h, *ref_v,	*ref_hv;
	
	VO_U32 lambda16;			/* how much vector bits weight */
	VO_S32 iEdgedWidth;		/* picture's stride */
	VO_U32 iFcode;			/* current fcode */

	VO_S32 chroma;					/* should we include chroma SAD? */

	VO_U8 *current_mb;

	VO_U8 *me_chroma;
	VO_U32 skip_thresh;
	VO_U32 me_early_exit_thresh;
} ME_DATA;

typedef struct {
    VO_S32 x;
    VO_S32 y;
    VO_U32 Direction[4];
    VO_U32 sad[5];
}
ME_PARAM;

typedef VO_VOID(VoCheckFunc)(const VO_S32 x, const VO_S32 y,
						            ME_DATA * const Data,
						            const VO_U32 Direction );

typedef VO_VOID (VoHalfPixelFunc)(ME_DATA * const data, VO_U32 mb_threshhold);

VoCheckFunc CheckCandidate16; 

VO_S32 CheckChromaSAD(const VO_S32 dx, const VO_S32 dy, ME_DATA * const data);

//MainSearchFunc _me_DiamondSearch, DiamondSearch, _me_SquareSearch;
VO_VOID DiamondSearch(VO_S32 x, VO_S32 y, ME_DATA * const data,
				 VO_S32 bDirection, VoCheckFunc * const CheckCandidate);

VO_VOID CrossDiamondSearch(ME_DATA * const data, VoCheckFunc * const CheckCandidate);

VO_VOID HalfPixelRefine(ME_DATA * const data,  VO_U32 mb_threshhold);

VO_U32 GetMinFcode(const VO_S32 MVmax);

// Calculate the min/max range
// relative to the _MACROBLOCK_ position
static VO_VOID __inline GetSearchRange(ME_DATA * const me_data,
			                                    const VO_U32 x,
			                                    const VO_U32 y,
			                                    const VO_U32 width,
			                                    const VO_U32 height,
			                                    const VO_S32 codec_id) 
{
	VO_S32 k;
//	const VO_S32 search_range = 2 << ((3+fcode) > 5 ? 5: (3+fcode));//1 << (3+fcode);//1 << ((3+fcode) > 5 ? 5: (3+fcode));
	//const VO_S32 fcode = me_data->iFcode;
//	const VO_S32 search_range = ((codec_id == VO_INDEX_ENC_H263)? ((3+fcode) > 4 ? (2 <<4): (2 <<(3+fcode))):(2 << (4+fcode)));
	const VO_S32 search_range = (2 <<4);

	VO_S32 high = search_range - 1;
	VO_S32 low = -search_range;

	if(codec_id == VO_INDEX_ENC_H263){
		if(((x+1)<<4) == width)
			me_data->max_dx = 0;
		else
			me_data->max_dx = high;
		if(((y+1)<<4) == height)
			me_data->max_dy = 0;
		else
			me_data->max_dy = high;
		if(x == 0)
			me_data->min_dx = 0;
		else
			me_data->min_dx = low;
		if(y == 0)
			me_data->min_dy = 0;
		else
			me_data->min_dy = low;
	}else{
		k = (VO_S32)(width - (x<<4))<<1;
		me_data->max_dx = MIN(high, k);
		k = (VO_S32)(height -  (y<<4))<<1;
		me_data->max_dy = MIN(high, k);
		
		k = (-(VO_S32)((x+1)<<4))<<1;
		me_data->min_dx = MAX(low, k);
		k = (-(VO_S32)((y+1)<<4))<<1;
		me_data->min_dy = MAX(low, k);
	}
}

/* reversed mv.length table */
static const VO_S32 r_mvtab[64] = {
	12, 12, 12, 12, 12, 12, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 12,
	12, 11, 11, 11, 11, 11, 11, 10,
	10, 10, 10, 10, 10, 10, 10, 10,
	10, 10, 10, 10, 10, 9, 9, 9,
	7, 7, 7, 6, 4, 3, 2, 1
};

static __inline VO_U32 CalculateMvBits(VO_S32 x, VO_S32 y, const VECTOR pred, const VO_U32 iFcode)
{
	VO_U32 bits;

	x -= pred.x;
    y -= pred.y;

    if(x && y) { //x !=0 && y !=0
        bits = 2*iFcode;
	    x = -VOGETABS(x);
	    x >>= (iFcode - 1);		    
	    y = -VOGETABS(y);
	    y >>= (iFcode - 1);
	    bits  += r_mvtab[x+63]+ r_mvtab[y+63];
        return bits;
    }
    else if( x || y ) { //x !=0 || y !=0
        if( x ) {
            bits = iFcode+1;
            x = -VOGETABS(x);
	        x >>= (iFcode - 1);	
            bits  += r_mvtab[x+63];
        }
        else {
             bits  = 1 + iFcode;
            y = -VOGETABS(y);
	        y >>= (iFcode - 1);
            bits  += r_mvtab[y+63];
        }
	    return bits;
    }
    else {
         return 2;
    }
}

static __inline VO_VOID SetInterMB(MACROBLOCK *pMB, const VO_S32 sad, const VECTOR mv, const VO_S32 mode)
{
	pMB->mode = mode;
	pMB->mvs[0] = pMB->mvs[1] = pMB->mvs[2] = pMB->mvs[3] = mv;
	pMB->sad16 = pMB->sad8[0] = pMB->sad8[1] = pMB->sad8[2] = pMB->sad8[3] = sad;
	pMB->cbp = 0;
}

//check if given vector is equal to any vector checked before 
static __inline VO_S32 vector_repeats(const VECTOR * const pmv, const VO_U32 i)
{
	VO_U32 j;
	for (j = 0; j < i; j++)
		if (MVEQUAL(pmv[i], pmv[j])) return 1;
	return 0;
}

/*	make a binary mask that prevents diamonds/squares
	from checking a vector which has been checked as a prediction */
static __inline VO_S32 GenerateMask(const VECTOR * const pmv, const VO_U32 num, VO_U32 direction)
{
	VO_U32 mask = 255, j;
	VO_U32 current = direction;

	for (j = 0; j < num; j++) 
    {
		if (pmv[current].x == pmv[j].x) 
        {
			if (pmv[current].y == pmv[j].y + VODIAMONDSIZE)
				mask &= ~4;
            else if (pmv[current].y == pmv[j].y - VODIAMONDSIZE)
				mask &= ~8;
		}
        else if (pmv[current].y == pmv[j].y) 
        {
			if (pmv[current].x == pmv[j].x + VODIAMONDSIZE)
				mask &= ~1;
            else if (pmv[current].x == pmv[j].x - VODIAMONDSIZE)
				mask &= ~2;
		}
	}
	return mask;
}

#endif /* _ESTIMATION_H_ */
