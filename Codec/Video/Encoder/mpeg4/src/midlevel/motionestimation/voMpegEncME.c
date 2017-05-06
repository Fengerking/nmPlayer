/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include "string.h"
#include "../../voMpegEnc.h"
#include "../voMpegEncPrediction.h"
#include "voMpegEncMEMethod.h"
#include "voMpegEncME.h"
#include "../voMpegEncMB.h"
#include "../../lowlevel/voMpegEncSad.h"
#include "../../common/src/voMpegMC.h"

#define ME_OFFSET(x, y, stride) ((x) + (y) * (stride))

static const VO_S32 me_lambda_table[32] =
{
    0, 2, 3, 5, 6, 8, 9, 11,
    12, 14, 15, 17, 18, 20, 21, 23,
    24, 26, 27, 29, 30, 32, 33, 35,
    36, 38, 39, 41, 42, 44, 45, 47
};

#define CALCULATEMVMAXSUM(i) \
        if (mv[i].x > max) \
            max = mv[i].x;\
        else if (-mv[i].x - 1 > max) \
            max = -mv[i].x - 1;\
        if (mv[i].y > max) \
            max = mv[i].y;\
        else if (-mv[i].y - 1 > max)\
            max = -mv[i].y - 1;\
        stat->mv_sum += mv[i].y * mv[i].y;\
        stat->mv_sum += mv[i].x * mv[i].x;

static __inline VO_VOID CheckCandidate16FourMV(const VO_S32 x, 
                                                            const VO_S32 y, 
                                                            ME_DATA * const me_data, 
                                                            const VO_U32 Direction)
{
	VO_U8 * reference;
	VO_U32 sad, t;
	VECTOR * current;
	VO_S32 xc, yc;
	VO_U32 tmp_sad[4];				
	
	if ( (x > me_data->max_dx) || (x < me_data->min_dx)
        || (y > me_data->max_dy) || (y < me_data->min_dy) )
        return;	

	reference = me_data->RefY + (x>>1) + (y>>1)*me_data->iEdgedWidth;
	current = me_data->current_best_MV;
	xc = x;     yc = y;	
	
	sad = Sad16_4MV(me_data->current_mb, reference, 16, me_data->iEdgedWidth, tmp_sad);
	t = CalculateMvBits(x, y, me_data->predMV, me_data->iFcode);
	sad += (me_data->lambda16 * t);

	if (me_data->chroma) {
		sad += CheckChromaSAD( (xc >> 1) + round_tab_79[xc & 0x3],
			                              (yc >> 1) + round_tab_79[yc & 0x3],
                                          me_data);
	}

	if (sad < me_data->iMinSAD[0]) {
		me_data->iMinSAD[0] = sad; current[0].x = x; current[0].y = y;
		me_data->dir = Direction;
	}

    UPDATAMEDATA(me_data,tmp_sad,current);
}

VO_VOID CheckCandidate16(const VO_S32 x, const VO_S32 y, ME_DATA * const me_data, const VO_U32 Direction)
{
	const VO_U8 * reference;
	VO_U32 sad, t;
	VECTOR * current;
	VO_S32 xc, yc;
	
	if ( (x > me_data->max_dx) || (x < me_data->min_dx)
		|| (y > me_data->max_dy) || (y < me_data->min_dy) )
        return;	
	
	reference = me_data->RefY + (x>>1) + (y>>1)*me_data->iEdgedWidth;
	current = me_data->current_best_MV;
	xc = x;     yc = y;	
	
	sad = Sad16(me_data->current_mb, reference, 16, me_data->iEdgedWidth, me_data->iMinSAD[0]);

	t = CalculateMvBits(x, y, me_data->predMV, me_data->iFcode);
	sad += (me_data->lambda16 * t);

	if (me_data->chroma) {
		sad += CheckChromaSAD((xc >> 1) + round_tab_79[xc & 0x3],
			(yc >> 1) + round_tab_79[yc & 0x3], me_data);
	}

	if (sad < me_data->iMinSAD[0]) {
		me_data->iMinSAD[0] = sad; current[0].x = x; current[0].y = y;
		me_data->dir = Direction;
	}
}

VO_S32 CheckChromaSAD(const VO_S32 dx, const VO_S32 dy, ME_DATA * const me_data)
{
	VO_U32 sad;

	const VO_U32 stride = me_data->iEdgedWidth/2;
	//VO_S32 offset = (dx>>1) + (dy>>1)*stride;
	//VO_S32 next = 1;

	//it has been checked recently 
	if (dx == me_data->chroma_x && dy == me_data->chroma_y) 
		return me_data->chroma_SAD; 

	me_data->chroma_x = dx; 
    me_data->chroma_y = dy; /* backup */

	AllCopyBlock[me_data->rounding][MV_SUB(dx, dy)](me_data->RefU, me_data->me_chroma, stride, 8, 8);
	sad = Sad8(me_data->CurU, me_data->me_chroma, stride, 8);

	AllCopyBlock[me_data->rounding][MV_SUB(dx, dy)](me_data->RefV, me_data->me_chroma, stride, 8, 8);
	sad += Sad8(me_data->CurV, me_data->me_chroma, stride, 8);

	me_data->chroma_SAD = sad; /* backup, part 2 */
	return sad;
}

static __inline VO_U32 CalculateDMvBits(const VECTOR *dmv, VO_U32 fcode)
{
	VO_U32 bits;
	VO_S32 x, y;

    x = dmv->x;
    y = dmv->y;

    if(y==0 && x==0) {
        return 2;
    }
    else if(x==y) {
        bits = 2*(x != 0 ? fcode:0);
	    x = ((2*(x)&((x)>>31))-(x)); 
	    x >>= (fcode - 1);
        bits  += 2*r_mvtab[x+63];
    }
    else {
        bits = (x != 0 ? fcode:0);
        bits += (y != 0 ? fcode:0);

	    x = ((2*(x)&((x)>>31))-(x)); 
	    x >>= (fcode - 1);	
	    
	    y = ((2*(y)&((y)>>31))-(y)); 
	    y >>= (fcode - 1);

	    bits  += r_mvtab[x+63]+ r_mvtab[y+63];
    }
	return bits;
}

static VO_VOID MacroBlockModeDecision(ME_DATA * const me_data, 
				                                Statistics *stat,
				                                MACROBLOCK * const mb,
				                                VO_S32 x, 
                                                VO_S32 y,
				                                VO_U32 mb_width,
				                                VO_S32 inter4v)
{
	VO_S32 mode = MODE_INTER;
	const VO_S32 quant = mb->quant;
	const VO_U32 lambda8 = me_lambda_table[quant];	
	
	VO_U32 sad = 0,sad_sam =0;
	VO_S32 InterBias = MV16_INTER_BIAS;

	// To check inter or inter 4v
	if (inter4v){
		VO_S32 i = 0, j;
		VECTOR pred_mv;
		VO_S32 sumx = 0, sumy = 0;
		
		for(j = 0; j < 4; j++){
			VECTOR blk_mv = me_data->current_best_MV[j + 1];
			pred_mv = GetBlockPredictMV(mb, mb_width, x, y, j);			
			i = CalculateMvBits(blk_mv.x, blk_mv.y, pred_mv, me_data->iFcode);
			me_data->iMinSAD[j + 1] += lambda8 * i;			
            mb->pmvs[j] = pred_mv;
			mb->mvs[j] = blk_mv;
			sumx += blk_mv.x;
			sumy += blk_mv.y;
			mb->sad8[j] = me_data->iMinSAD[j + 1];
		}
		
		if (me_data->chroma){
			me_data->iMinSAD[1] += CheckChromaSAD((sumx >> 3) + round_tab_76[sumx & 0xf],
				                                            (sumy >> 3) + round_tab_76[sumy & 0xf], me_data);
		}

        sad_sam = me_data->iMinSAD[1] + me_data->iMinSAD[2] 
            +	me_data->iMinSAD[3] + me_data->iMinSAD[4] + IMV16X16 * (VO_S32)quant;

        if (me_data->iMinSAD[0] < sad_sam ) {
            mode = MODE_INTER;
            sad = me_data->iMinSAD[0];
        } else {
            mode = MODE_INTER4V;
            sad = sad_sam;
	    }

	} else{ 
		me_data->iMinSAD[1] = MV_MAX_ERROR;
        mode = MODE_INTER;
        sad = me_data->iMinSAD[0];
	}

	// intra decision
	if (quant > 10) 
        InterBias += 60 * (quant - 10); // to make high quants work

	if (y != 0){
		if ((mb - mb_width)->mode == MODE_INTRA ) InterBias -= 80;
	}
	if (x != 0){
		if ((mb - 1)->mode == MODE_INTRA ) InterBias -= 80;
	}

	if (me_data->chroma) InterBias += 50; /* dev8(chroma) ??? <-- yes, we need dev8 (no big difference though) */

	if (InterBias < (VO_S32)sad) {
		VO_S32 deviation = Dev16(me_data->CurY, me_data->iEdgedWidth);
		if (deviation < (VO_S32)(sad - InterBias)){
			mode = MODE_INTRA;
			//stat->mode_intra++;
		}
	}    				
	mb->cbp = 63;
	mb->sad16 = sad;

	//Not coded decision	
	if (mode == MODE_INTER) {
#ifdef P_DOMAIN_RC
		VECTOR mv_d; 
#endif
		mb->mvs[0] = mb->mvs[1] = mb->mvs[2] = mb->mvs[3] = me_data->current_best_MV[0];	
        mb->pmvs[0] = me_data->predMV;
#ifdef P_DOMAIN_RC
		mv_d.x = me_data->current_best_MV[0].x - me_data->predMV.x;
		mv_d.y = me_data->current_best_MV[0].y - me_data->predMV.y;
		stat->mv_all += CalculateDMvBits(&mv_d, me_data->iFcode);
#endif
		// final skip decision
#ifdef SKIP_ME//check it later
		if (sad <(VO_U32) quant * me_data->skip_thresh){
			mb->cbp = 0;
		}
#endif

	}else if (mode == MODE_INTER4V){
#ifdef P_DOMAIN_RC
		VO_S32 i;
		for (i = 0; i < 4; i++) {
			VECTOR mv_d; 
			mv_d.x = me_data->current_best_MV[i].x - mb->pmvs[i].x;
			mv_d.y = me_data->current_best_MV[i].y - mb->pmvs[i].y;
			stat->mv_all += CalculateDMvBits(&mv_d, me_data->iFcode);
		}
#endif
        ;
	}else{	/* INTRA, NOT_CODED */
		SetInterMB(mb, 0, zeroMV, mode);
	}	
	mb->mode = mode;
}
	
extern VO_S32 pred_mv_block_pos [4][3];

static __inline VECTOR GetCandidate(VECTOR * const pmv, 
                                                VO_S32 x, 
                                                VO_S32 y, 
                                                VO_S32 mb_width, 
                                                VO_S32 mb_height,
                                                const MACROBLOCK * const pMB)
{	
    VO_S32 lpos, tpos, rpos;
	//VO_S32 num_cand = 0, last_cand = 1;
    VECTOR pmv0 = zeroMV;

    if(x >0 && y > 0) {
        if ((x + 1) < mb_width) {
            lpos = pred_mv_block_pos[0][0];
            tpos = pred_mv_block_pos[0][1];
            rpos = pred_mv_block_pos[0][2];

		    pmv[3] = pMB[lpos].mvs[1];        
		    pmv[4] = pMB[tpos].mvs[2];           
		    pmv[5] = pMB[rpos].mvs[2];

            VOGETMEDIAN(pmv0.x,pmv[3].x, pmv[4].x, pmv[5].x);
            VOGETMEDIAN(pmv0.y,pmv[3].y, pmv[4].y,pmv[5].y);
            //*****
            VOMVEVEN(pmv[5],pmv[5]);
            VOMVEVEN(pmv[4],pmv[4]);
            VOMVEVEN(pmv[3],pmv[3]);
            VOMVEVEN(pmv[2],pMB->mvs[0]);
            pmv[1] = zeroMV;
            VOMVEVEN(pmv[0],pmv0);
            if (y < mb_height-1)		
                VOMVEVEN(pmv[6],(pMB+1+mb_width)->mvs[0])
	        else 
		        pmv[6] = zeroMV;
            return pmv0;
        }
        else {
             lpos = pred_mv_block_pos[0][0];
             tpos = pred_mv_block_pos[0][1];
             pmv[1] = pMB[lpos].mvs[1];     
             pmv[2] = pMB[tpos].mvs[2];      
             VOGETMEDIAN(pmv0.x,pmv[1].x, pmv[2].x,0);
             VOGETMEDIAN(pmv0.y,pmv[1].y, pmv[2].y,0);
             //*****
             pmv[6] = zeroMV;
             pmv[5] = zeroMV;
             VOMVEVEN(pmv[4],pmv[2]);
             VOMVEVEN(pmv[3],pmv[1]);
             VOMVEVEN(pmv[2],pMB->mvs[0]);
             pmv[1] = zeroMV;
             VOMVEVEN(pmv[0],pmv0);
             return pmv0;            
        }        
    }
    else if(x > 0 && y == 0 ) {
        lpos = pred_mv_block_pos[0][0];
		pmv0 = pMB[lpos].mvs[1];
        pmv[5] = zeroMV;
        pmv[4] = zeroMV;
        VOMVEVEN(pmv[3],pmv0);
        VOMVEVEN(pmv[2],pMB->mvs[0]);
        pmv[1] = zeroMV;                    
        pmv[0] =pmv[3];    

        if (x < mb_width-1)		
            VOMVEVEN(pmv[6],(pMB+1+mb_width)->mvs[0])
	    else 
		    pmv[6] = zeroMV;

       return pmv0;
    }
    else if(x ==0 && y > 0 ) {
        tpos = pred_mv_block_pos[0][1];
        rpos = pred_mv_block_pos[0][2];
        pmv[4]= pMB[tpos].mvs[2];
        pmv[5] = pMB[rpos].mvs[2]; 
        VOGETMEDIAN(pmv[0].x,0, pmv[4].x,pmv[5].x);
        VOGETMEDIAN(pmv[0].y,0, pmv[4].y,pmv[5].y);
        pmv0 =  pmv[0];
        VOMVEVEN(pmv[5],pmv[5]);
        VOMVEVEN(pmv[4],pmv[4]);
        pmv[3] = zeroMV;
        VOMVEVEN(pmv[2],pMB->mvs[0]);
        pmv[1] = zeroMV;
        VOMVEVEN(pmv[0],pmv0);
        if (y < mb_height-1)	
            VOMVEVEN(pmv[6],(pMB+1+mb_width)->mvs[0])
	    else 
		    pmv[6] = zeroMV;
        return pmv0;
    }
    else if (x == 0 && y == 0) {
		pmv[0] = pmv[1]  = pmv[3] = pmv[4] = pmv[5] = zeroMV;
        VOMVEVEN(pmv[6],(pMB+1+mb_width)->mvs[0]);
        VOMVEVEN(pmv[2],pMB->mvs[0]);
       return pmv0;
	}
    else
     return zeroMV;
}


static __inline VO_VOID MotionStatsPVOP(VO_S32 * const MVmax, 
				                                  Statistics * const stat,
				                                  const MACROBLOCK * const pMB)
{
	const VECTOR * const mv = pMB->mvs;
	VO_S32 max = *MVmax;

    if(pMB->mode == MODE_INTER4V)
    {
        (stat->mv_num) += 4;
        CALCULATEMVMAXSUM(0);
        CALCULATEMVMAXSUM(1);
        CALCULATEMVMAXSUM(2);
        CALCULATEMVMAXSUM(3);
        CALCULATEMVMAXSUM(0);
        (stat->mv_num)++;
		*MVmax = max;
    }
    else if(pMB->mode == MODE_INTER)
    {
        (stat->mv_num)++;
        CALCULATEMVMAXSUM(0);        
		*MVmax = max;
    }
}


static VO_S32 CheckStartMB(MACROBLOCK *pMB, ME_DATA* pData, VECTOR *pStartMV)
{
	VECTOR start_mv = *pStartMV;
	VO_S32 iEdgedWidth = pData->iEdgedWidth, i;

	pMB->sad16 = Sad16_4MV(pData->CurY, 
                                        pData->RefY + ME_OFFSET(start_mv.x>>1, start_mv.y>>1, iEdgedWidth),
		                                iEdgedWidth, 
                                        iEdgedWidth,
                                        pMB->sad8 );

    CopyBlock16x16(pData->CurY, pData->current_mb, iEdgedWidth, 16, 16);	

	if (pData->chroma) {
		pData->chroma_SAD = Sad8(pData->CurU, pData->RefU + ME_OFFSET(start_mv.x>>2, start_mv.y>>2, iEdgedWidth>>1), iEdgedWidth>>1, iEdgedWidth>>1)
			+ Sad8(pData->CurV, pData->RefV + ME_OFFSET(start_mv.x>>2, start_mv.y>>2, iEdgedWidth>>1), iEdgedWidth>>1, iEdgedWidth>>1);
		pMB->sad16 += pData->chroma_SAD;
		pData->chroma_x = start_mv.x >> 1;
		pData->chroma_y = start_mv.y >> 1;
	}
	
	i = CalculateMvBits(start_mv.x, start_mv.y, pData->predMV, pData->iFcode);
	pMB->sad16 += (pData->lambda16 * i);

	// initial skip decision 
    if (pMB->dquant || pMB->sad16 >= pMB->quant * pData->skip_thresh){
        pData->dir = 0;		

        pData->current_best_MV[0] = 
            pData->current_best_MV[1] = 
            pData->current_best_MV[2] = 
            pData->current_best_MV[3] =
            pData->current_best_MV[4] = start_mv;
		
		pData->iMinSAD[0] = pMB->sad16;
		pData->iMinSAD[1] = pMB->sad8[0];
		pData->iMinSAD[2] = pMB->sad8[1];
		pData->iMinSAD[3] = pMB->sad8[2];
		pData->iMinSAD[4] = pMB->sad8[3];
		return 0;
    }
    else {
        if(start_mv.x  || start_mv.y ){ //TBD to check the skip mode, mv == 0??
            SetInterMB(pMB, pMB->sad16, start_mv, MODE_INTER);
            pMB->pmvs[0] = pData->predMV;
			return 1;	
        }
        else {
            SetInterMB(pMB, pMB->sad16, zeroMV, MODE_NOT_CODED);
			return 1;
        }
    }
}

VO_U32 CandidateSearch( const VO_S32 x, 
                                const VO_S32 y, 
                                VO_S32 threshhold,
                                ME_DATA* pData, 
                                VECTOR *pmv,       
                                VoCheckFunc * CheckCandidate)
{
	VO_U32 i;
    VO_S32 mask;

	for (i = 1; i < 7; i++){
		if (!vector_repeats(pmv, i)) {
			CheckCandidate(pmv[i].x, pmv[i].y, pData, i );			
			if (pData->iMinSAD[0] < (VO_U32)threshhold) 
				return 1;	
		}
	}

	//all vectors pmv[0..i-1] have been checked
    mask = GenerateMask(pmv, i, pData->dir);  
    DiamondSearch(pData->current_best_MV->x, pData->current_best_MV->y, pData, mask, CheckCandidate);	
    return 0;
}

bool MotionEstimation(ENCHND * const enc_hnd)
{
    ME_DATA me_data;
	const Mpeg4Frame *const img_cur = enc_hnd->img_cur;
	const Mpeg4Frame *const img_ref = enc_hnd->img_ref;

	MACROBLOCK * const pMBs = enc_hnd->mbs;
	ENCLOCAL * const enc_local = &enc_hnd->enc_local;
	const VO_U32 codec_id = enc_hnd->codec_id;

	const VO_U32 mb_width = enc_hnd->mb_width;
	const VO_U32 mb_height = enc_hnd->mb_height;
	const VO_U32 img_stride = enc_hnd->edged_width;
	const VO_U32 al_flag = enc_local->algorithm_flag;
	VO_S32 intra_limit;
	VO_S32 MVmax = 0;
	VO_U32 x, y;
	VO_S32  intramodenum = 0;
	VO_S32 quant;
	
	VECTOR pmv[7], start_mv;
	
	VoCheckFunc * CheckCandidate;
    VO_U32 inter4v;

	Statistics * const stat = &enc_hnd->sStat;

	if(al_flag & VOI_ADAPTIVE_IFRAME){
		intra_limit = (mb_width * mb_height) >>1;
	}else{
		intra_limit = mb_width * mb_height;
	}
	
	/* some pre-initialized thingies for SearchP */

	quant = enc_local->quant; 
	
	memset(&me_data, 0, sizeof(ME_DATA));
	me_data.iEdgedWidth = img_stride;
	me_data.iFcode = enc_local->m_fcode;
	me_data.rounding = enc_local->rounding;
	me_data.chroma = al_flag & VOI_CHROMA_ME;

	me_data.current_mb = enc_hnd->work_space;
	me_data.me_chroma = enc_hnd->work_space + 256;

	me_data.ref_v = me_data.me_chroma;
	me_data.ref_h = me_data.ref_v + 24 * 17;
	me_data.ref_hv = me_data.ref_h + 24 * 17;

	me_data.skip_thresh = INITIAL_SKIP_THRESH + (me_data.chroma ? INITIAL_SKIP_THRESH_C : 0);
	me_data.me_early_exit_thresh = ME_EARLY_EXIT_THRESH + (me_data.chroma? ME_EARLY_EXIT_THRESH_C : 0);
	
	CheckCandidate = al_flag & VOI_INTER4V ? CheckCandidate16FourMV: CheckCandidate16;

    inter4v = (al_flag & VOI_INTER4V);

	stat->mv_sum = stat->mv_num = 0;

	for (y = 0; y < mb_height; y++)	{
        VO_U32 mbh_pos = y * mb_width;
        VO_U32 pixel_h_pos = y * img_stride;
		for (x = 0; x < mb_width; x++)	{
			MACROBLOCK *pMB = &pMBs[x + mbh_pos];
			VO_S32 offset = (x + pixel_h_pos) << 4;
            VO_U32 mb_threshhold;

            inter4v =( inter4v && (pMB->dquant == 0));			
			quant = pMB->quant = quant + pMB->dquant;
			pMB->ac_dir =0;
			me_data.CurY = img_cur->y + offset;
			me_data.RefY = img_ref->y + offset;
			offset = ((x + (pixel_h_pos>>1) ) << 3);
			me_data.CurV = img_cur->uv[1] + offset;
			me_data.CurU = img_cur->uv[0] + offset;
			me_data.RefU = img_ref->uv[0] + offset;
			me_data.RefV = img_ref->uv[1] + offset;
			
			me_data.lambda16 = me_lambda_vec16[quant];			
			me_data.chroma_x = me_data.chroma_y = 0;

            //because it is fixed value,move it out of the loop;
			GetSearchRange(&me_data, x, y, enc_hnd->img_width, enc_hnd->img_height, codec_id);

			 me_data.predMV =  GetCandidate(pmv, x, y, enc_hnd->mb_width, enc_hnd->mb_height, pMB);
			//check for the predict MV
			start_mv.x = pmv[0].x;
			start_mv.y = pmv[0].y; 

			// maybe out of the range
            if(start_mv.x < me_data.min_dx )
                start_mv.x = me_data.min_dx;
            else
                start_mv.x = MIN(start_mv.x,me_data.max_dx);

             if(start_mv.y < me_data.min_dy )
                start_mv.y = me_data.min_dy;
            else
                start_mv.y = MIN(start_mv.y,me_data.max_dy);

            if(!CheckStartMB(pMB, &me_data, &start_mv)){
                mb_threshhold =  (VO_U32)pMB->quant * me_data.me_early_exit_thresh;

                if(!CandidateSearch(x,  y, mb_threshhold, &me_data, pmv, CheckCandidate)) {
                    //To check early exit
                    if (me_data.iMinSAD[0] >= mb_threshhold)
                    {
                        if(al_flag & VOI_HALFPELREFINE16)
                            HalfPixelRefine(&me_data, mb_threshhold);
                    }
                }							
			    //MB model decision
			    MacroBlockModeDecision(&me_data, &enc_hnd->sStat, pMB, x, y, enc_hnd->mb_width, inter4v);
			    if(pMB->mode == MODE_INTRA){
				    if((intramodenum ++)>intra_limit)
					    return 1;
			    }
            }
			MotionStatsPVOP(&MVmax, stat, pMB);	
		}
	}
	
	enc_local->fcode_for = GetMinFcode(MVmax);

    //if(enc_local->fcode_for != 1)
    // printf(" \n MVmax %d  fcode_for = %d \n",MVmax,enc_local->fcode_for);
	
	return 0;
}

VO_VOID MacroBlockMotionCompensation(const Mpeg4Frame * const ref,
					                 Mpeg4Frame * const cur,
					                 const MACROBLOCK * const mb,
					                 VO_S16 * const dct_codes,
					                 VO_U32 stride,
					                 VO_U32 rounding)
					 
{
	VO_S32 dx, dy, sumx =0, sumy = 0;
	const VO_U32 half_stride = (stride>>1);

    //quick copy for early SKIP
	if (mb->mode == MODE_NOT_CODED ) {	
		dx = mb->mvs[0].x;
		dy = mb->mvs[0].y;

		CopyBlock16x16(ref->y, cur->y, stride, stride, 16);
	}else{
		// ordinary compensation 
		VO_S32 k;
        VO_U8 *dst, *src;
		for(k = 0; k < 4; k++){			
			dx = mb->mvs[k].x;
			dy = mb->mvs[k].y;
			sumx += dx;
			sumy += dy;

			src = ref->y + (VO_S32)(((dy>>1) + ((k&2)<<2))* stride + (dx>>1) + ((k&1)<<3));
			dst = cur->y + (VO_S32)(((k&2)<<2)* stride + ((k&1)<<3));

			AllSubBlock[rounding][MV_SUB(dx, dy)](src, &dct_codes[k<<6], dst, stride);
		}
	}

	if(mb->mode == MODE_INTER4V){
		dx = (sumx >> 3) + round_tab_76[sumx & 0xf];
		dy = (sumy >> 3) + round_tab_76[sumy & 0xf];
	}else{
		dx = (dx >> 1) + round_tab_79[dx & 0x3];
		dy = (dy >> 1) + round_tab_79[dy & 0x3];
	}

	AllSubBlock[rounding][MV_SUB(dx, dy)](ref->uv[0] + (VO_S32)((dy>>1) * half_stride + (dx>>1)), 
		&dct_codes[256], cur->uv[0], half_stride);

	AllSubBlock[rounding][MV_SUB(dx, dy)](ref->uv[1] + (VO_S32)((dy>>1) * half_stride + (dx>>1)), 
		&dct_codes[320], cur->uv[1], half_stride);

}


