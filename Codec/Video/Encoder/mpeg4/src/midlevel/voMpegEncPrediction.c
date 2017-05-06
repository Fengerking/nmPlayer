/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "voMpegEncPrediction.h"
#include "../lowlevel/voMpegEncTable.h"
#include "voMpegEncBits.h"

static const VECTOR zeroMV = { 0, 0 };
VO_S32 const pred_mv_block [4][9] = { //l ,t,r
    {-1,0,1,  0,-1,2,1,-1,2},
    { 0,0,0,  0,-1,3,1,-1,2},
    {-1,0,3, 0,0,0,0,0,1},
    {0,0,2,0,0,0,0,0,1},
};

VO_S32  pred_mv_block_pos [4][3];

static VO_S32 __inline ReScale(VO_S32 pred_quant, VO_S32 curt_quant, VO_S32 coeff)
{
   if(coeff) {
       VO_S32 re_qcoeff,pred_qcoeff;
        pred_qcoeff = (coeff) * (pred_quant);

        if(pred_qcoeff>0)
            re_qcoeff = ((pred_qcoeff)+((curt_quant)>>1))/(curt_quant);
        else
            re_qcoeff = ((pred_qcoeff)-((curt_quant)>>1))/(curt_quant);

        return re_qcoeff;
    }
    else {
        return 0;
    }
}

//	get dc/ac prediction direction for a single block and place
//	predictor values into MB->pred_values[j][..]
//
VO_VOID GetPredACDC(MACROBLOCK * const mb,
			            const VO_U32 x,
			            const VO_U32 y,
			            const VO_U32 mb_width,
			            const VO_U32 block,
			            const VO_S16 * const qcoeff,
			            const VO_U32 current_quant,
			            const VO_S32 iDcScaler,
			            VO_S16 * const predictors,
			            VO_S16 * acdc_row, 
			            VO_S16 * acdc_col)
{
	VO_S16 left =0, top, tmp_diag, *tmp_pred;
	VO_S32 left_quant, top_quant;
	VO_S32 left_index, top_index, diag_index;
	VO_U32 *ac_dir = &mb->ac_dir;
	VO_U32 i, top_flag = 0, left_flag = 0;

	left_quant = current_quant;
    top_quant = current_quant;
	left_index = 0;
    top_index = 0;
    diag_index = 0;

	switch(block){
	case 0:
		left = *(acdc_col + 8);
		left_index = -1;
		top_index -= mb_width;
		diag_index = top_index - 1;
		break;
	case 1:
		acdc_row += 8;
		left = *(acdc_row - 8);
		top_index -= mb_width;
		diag_index = top_index;
		break;
	case 2:
		acdc_col +=  8;
		left = *(acdc_row - 24);
		left_index = -1;
		diag_index = left_index;
		break;
	case 3:
		acdc_row += 8;
		acdc_col += 8;
		left = *(acdc_row - 8);
		break;
	case 4:
	case 5:
		acdc_row += ((block-2)<<3);
		acdc_col += ((block-2)<<3);
		left = *(acdc_row - 32);
		left_index = -1;
		top_index -= mb_width;
		diag_index = top_index - 1;
		break;
	}

	top = *(acdc_row);
	tmp_diag = *(acdc_col) ;

	// left macroblock 
	if(left_index){
		if (!x || ((mb[left_index].mode != MODE_INTRA) &&
			(mb[left_index].mode != MODE_INTRA_Q))) {
			left = 1024;
			left_flag = 1;
		}else{
			left_quant = mb[left_index].quant;
		}
	}

	// top macroblock
	if(top_index){
		if (!y || ((mb[top_index].mode != MODE_INTRA) &&
			(mb[top_index].mode != MODE_INTRA_Q))) {
			top = 1024;	
			top_flag = 1;
		}else{
			top_quant = mb[top_index].quant;
		}
	}

	// diag macroblock 	
	if(diag_index){
		if (   ((!x) && left_index) || ((!y) && top_index) || 
			!(mb[diag_index].mode == MODE_INTRA || mb[diag_index].mode == MODE_INTRA_Q)) {
			tmp_diag = 1024;
		}
	}

    if(VOGETABS(left - tmp_diag) < VOGETABS(tmp_diag - top)){
		*ac_dir |= (1 << (block * 2));             // vertical
		tmp_diag = top;
		tmp_pred = acdc_row;
	}else{
		*ac_dir |= (2 << (block * 2));             // horizontal
		top_flag = left_flag;
		tmp_diag = left;
		tmp_pred = acdc_col;
		top_quant = left_quant;
	}

    *(predictors) = (VO_S16)(((tmp_diag)>0) ? ((tmp_diag)+((iDcScaler)>>1))/(iDcScaler) : ((tmp_diag)-((iDcScaler)>>1))/(iDcScaler));
	if(top_flag){
		memset(predictors + 1, 0, 14);
	}else{
		for (i = 1; i < 8; i++){
			*(predictors + i) = (VO_S16)ReScale(top_quant, current_quant, *(tmp_pred + i));
		}		
	}

	//store diag dc for prediction of next block
    *acdc_col = *(acdc_row);

	//store DC of current block 
    *acdc_row = (VO_S16)(*(qcoeff) * iDcScaler);
    VOCLAMP(*acdc_row, -2048, 2047);

    for (i = 1; i < 8; i ++){
        *(acdc_row + i) = *(qcoeff + i);
        *(acdc_col + i) = *(qcoeff + (i<<3));
    }
}

// encoder: subtract predictors from qcoeff[] and calculate S1/S2
//returns sum of coeefficients *saved* if prediction is enabled
//S1 = sum of all (qcoeff - prediction)
//S2 = sum of all qcoeff
//
VO_S32 CalcACDCCoeff( MACROBLOCK * pMB,
		                      VO_U32 block,
		                      VO_S16 *qcoeff,
		                      VO_U32 iDcScaler,
		                      VO_S16 *predictors )
{
	VO_U32 i;
	VO_S32 S1 = 0;

	qcoeff[0] -= predictors[0];

    if (((pMB->ac_dir & (ACDIR_MSK << (block<<1)))>>(block<<1)) == 1) {

        predictors[7] = qcoeff[7] - predictors[7];
        S1 = (VOGETABS(qcoeff[7])-VOGETABS(predictors[7]));

        for (i = 6; i >0; i -=3 ) {	
            VO_S16 tmp_q0 = qcoeff[i];
            VO_S16 tmp_q1 = qcoeff[i-1];
            VO_S16 tmp_q2 = qcoeff[i-2];

            VO_S16 tmp_p0 = predictors[i];
            VO_S16 tmp_p1 = predictors[i-1];
            VO_S16 tmp_p2 = predictors[i-2];

            tmp_p0 = tmp_q0 - tmp_p0;
            tmp_p1 = tmp_q1 - tmp_p1;
            tmp_p2 = tmp_q2 - tmp_p2;

            S1 += (VOGETABS(tmp_q0)-VOGETABS(tmp_p0));
            S1 += (VOGETABS(tmp_q1)-VOGETABS(tmp_p1));
            S1 += (VOGETABS(tmp_q2)-VOGETABS(tmp_p2));

            predictors[i] = tmp_p0;
            predictors[i-1] = tmp_p1;
            predictors[i-2] = tmp_p2;
	    }
    }
    else {

        predictors[7] = qcoeff[7<<3] - predictors[7];
        S1 = (VOGETABS(qcoeff[7<<3])-VOGETABS(predictors[7]));

        for (i = 6; i >0; i -=3 ) {	
            VO_S16 tmp_q0 = qcoeff[i<<3];
            VO_S16 tmp_q1 = qcoeff[(i-1)<<3];
            VO_S16 tmp_q2 = qcoeff[(i-2)<<3];

            VO_S16 tmp_p0 = predictors[i];
            VO_S16 tmp_p1 = predictors[i-1];
            VO_S16 tmp_p2 = predictors[i-2];

            tmp_p0 = tmp_q0 - tmp_p0;
            tmp_p1 = tmp_q1 - tmp_p1;
            tmp_p2 = tmp_q2 - tmp_p2;

            S1 += (VOGETABS(tmp_q0)-VOGETABS(tmp_p0));
            S1 += (VOGETABS(tmp_q1)-VOGETABS(tmp_p1));
            S1 += (VOGETABS(tmp_q2)-VOGETABS(tmp_p2));

            predictors[i] = tmp_p0;
            predictors[i-1] = tmp_p1;
            predictors[i-2] = tmp_p2;
	    }
    }    
	return (S1);
}

// returns the bits *saved* if prediction is enabled
VO_S32 CalACDCBits( MACROBLOCK * pMB,
		                    VO_U32 block,
		                    VO_S16 *qcoeff,
		                    VO_U32 iDcScaler,
		                    VO_S16 *predictors)
{
	const VO_S32 direction = (pMB->ac_dir & (ACDIR_MSK <<(block<<1)))>>(block<<1);
	VO_S16 tmp[8];
	VO_U32 i;
	VO_S32 Z1, Z2;

	// dc prediction
	qcoeff[0] -= predictors[0];

	// calc cost before ac prediction
	Z2 = CalcIntraCoeffBits(qcoeff, 0);

     if (direction == 1) {//col
        for (i = 1; i < 8; i++) {
		    tmp[i] = qcoeff[i ];
		    qcoeff[i] -= predictors[i];
		    predictors[i] = qcoeff[i];
	    }
        Z1 = CalcIntraCoeffBits(qcoeff, direction);

        for (i = 1; i < 8; i++)
            qcoeff[i] = tmp[i];
    }
    else {
        for (i = 1; i < 8; i++) {
		    tmp[i] = qcoeff[i << 3];
		    qcoeff[i << 3] -= predictors[i];
		    predictors[i] = qcoeff[i <<3];
	    }
        Z1 = CalcIntraCoeffBits(qcoeff, direction);
        for (i = 1; i < 8; i++)
            qcoeff[i << 3] = tmp[i];
    }

	return (Z2 - Z1);
}

// apply predictors[] to qcoeff
VO_VOID ApplyACDC(MACROBLOCK * pMB,
		            VO_U32 block,
		            VO_S16 *qcoeff,
		            VO_S16 *predictors)
{
	VO_U32 i;

    if((pMB->ac_dir & (ACDIR_MSK <<(block<<1)))>>(block<<1) == 1) {
        for (i = 1; i < 8; i++)
            qcoeff[i] = predictors[i];
    }
    else {
         for (i = 1; i < 8; i++)
            qcoeff[i<<3] = predictors[i];
    }
}

VO_VOID ACDCPrediction(ENCHND * enc_hnd,
			            MACROBLOCK *pMB,
			            const VO_U32 al_flag,
			            const VO_U32 x,
			            const VO_U32 y,
			            const VO_U32 mb_width,
			            VO_S16 * const qcoeff)
{

	VO_S32 j;
	VO_S32 dc_scaler, quant;
	VO_S32 S = 0;
	VO_S16 predictors[6][8];
    VO_S32 (*pCalBitsFunc)( MACROBLOCK * pMB, VO_U32 block, VO_S16 *qcoeff, VO_U32 iDcScaler, VO_S16 *predictors);
	VO_S16 * const acdc_row = (enc_hnd->pred_acdc_row + ((x + 1)<<5));
	VO_S16 * const acdc_col = enc_hnd->pred_acdc_col;

    if (al_flag & VOI_HQACPRED)
        pCalBitsFunc = CalACDCBits;
    else
        pCalBitsFunc = CalcACDCCoeff;

    quant = pMB->quant;
    dc_scaler = GeDcScaler(quant, 1);

    for (j = 0; j < 4; j++) {
				
		GetPredACDC(pMB, x, y, mb_width, j, &qcoeff[j * 64],
			                quant, dc_scaler,  predictors[j], 
                            acdc_row , acdc_col);	
        S += pCalBitsFunc(pMB, j, &qcoeff[j * 64], dc_scaler, predictors[j]);
	}

    dc_scaler = GeDcScaler(quant, 0);
	for (j = 4; j < 6; j++) {
				
		GetPredACDC(pMB, x, y, mb_width, j, &qcoeff[j * 64],
			            quant, dc_scaler,  predictors[j], 
                        acdc_row , acdc_col );		

		 S += pCalBitsFunc(pMB, j, &qcoeff[j * 64], dc_scaler, predictors[j]);
	}
	
	if (S<=0) {  //do'nt predict ac
		pMB->ac_dir = 0;
	}else{
        for (j = 0; j < 6; j++)
			ApplyACDC(pMB, j, &qcoeff[j * 64], predictors[j]);
	}	
} 

VECTOR GetBlockPredictMV(const MACROBLOCK * const mb,
		                            const VO_S32 mb_width,
		                            const VO_S32 x,
		                            const VO_S32 y,
		                            const VO_S32 block)
{
	VO_S32 lpos, lz, tpos, tz, rpos, rz;
	VO_S32 num_cand = 0, last_cand = 1;
	VECTOR pmv[4];	 //1:left , 2:top , 3:top-right 

	if (!x&&((block&1)==0)) {
		pmv[1] = zeroMV;
	} else{
		num_cand++;
        lpos = pred_mv_block_pos[block][0];
        lz =  pred_mv_block[block][2];
		pmv[1] = mb[lpos].mvs[lz];
	}

	if (!y&&(block<2)) {
		pmv[2] = zeroMV;
        pmv[3] = zeroMV;
	} else {
		num_cand++;
        tpos = pred_mv_block_pos[block][1];
        tz =  pred_mv_block[block][5];
		pmv[2] = mb[tpos].mvs[tz];
        last_cand = 2;
        if((x+(block<2))!=mb_width) {
            num_cand++;
            rpos = pred_mv_block_pos[block][2];
            rz =  pred_mv_block[block][8];
		    pmv[3] = mb[rpos].mvs[rz];
            last_cand = 3;
        }
        else
            pmv[3] = zeroMV;
	}

	if (num_cand > 1) {
		//get median value
        VOGETMEDIAN(pmv[0].x,pmv[1].x, pmv[2].x,pmv[3].x);
        VOGETMEDIAN(pmv[0].y,pmv[1].y, pmv[2].y,pmv[3].y);
		return pmv[0];
	}

	return pmv[last_cand];	
}

#ifdef MPEG4_RESYNC
VO_VOID RGetPMVBlock( MACROBLOCK * const mb,
		                const VO_S32 mb_width,
		                const VO_S32 x,
		                const VO_S32 y,
		                VO_S32 left_resync,
		                VO_S32 top_resync)
{
	VO_S32 lpos, lz, tpos, tz, rpos, rz;
	VO_S32 num_cand, last_cand;
	VECTOR pmv[4];	/* left neighbour, top neighbour, top-right neighbour */
	VO_S32 block;
	VO_S32 top_right_resync;
	VO_S32 left_resync_bak = left_resync;

	for(block = 0; block < 4; block++){
		num_cand = 0, last_cand = 1;

		switch (block) {
		case 0:
			top_right_resync = top_resync - 1;
			break;
		case 1:
			left_resync = 0;
			top_right_resync = top_resync - 1;
			break;
		case 2:
			left_resync = left_resync_bak;
			top_resync = 0;
			top_right_resync = 0;
			break;
		default:
			top_resync = 0;
			top_right_resync = 0;
			left_resync = 0;
		}
		
		
		if (left_resync||!x&&(block&1)==0) {//left
			pmv[1] = zeroMV;
		} else{
			num_cand++;
            lz =  pred_mv_block[block][2];
            lpos   = pred_mv_block_pos[block][0];
			pmv[1] = mb[lpos].mvs[lz];
		}
		
		if (top_resync||!y&&(block<2)) {//top
			pmv[2] = zeroMV;
		} else {
			num_cand++;
			last_cand = 2;
            tpos  = pred_mv_block_pos[block][1];
            tz =  pred_mv_block[block][5];
			pmv[2] = mb[tpos].mvs[tz];
		}
		
		if (top_right_resync||!y&&(block<2) || ((x+(block<2))==mb_width)) {//top-right
			pmv[3] = zeroMV;
		} else{
			num_cand++;
			last_cand = 3;
            rz =  pred_mv_block[block][8];
            rpos  = pred_mv_block_pos[block][2];
			pmv[3] = mb[rpos].mvs[rz];
		}
		
		// return the median vector		
		if (num_cand > 1) {
			//get median 
            VOGETMEDIAN(pmv[0].x,pmv[1].x, pmv[2].x,pmv[3].x);
            VOGETMEDIAN(pmv[0].y,pmv[1].y, pmv[2].y,pmv[3].y);			
			mb->pmvs[block] = pmv[0];
		}else{
			mb->pmvs[block] = pmv[last_cand];
		}
	}
}

VO_VOID RGetPMVMacroBlock(MACROBLOCK * const mb,
		                        const VO_S32 mb_width,
		                        const VO_S32 x,
		                        const VO_S32 y,
		                        VO_S32 left_resync,
		                        VO_S32 top_resync)
{
	VO_S32 lpos, lz, tpos, tz, rpos, rz;
	VO_S32 num_cand = 0, last_cand = 1;
	VECTOR * const pmv = mb->pmvs;

    if (x == 0 && y == 0) {
		pmv[0] = pmv[1] = pmv[2] = pmv[3] = zeroMV;
		return;
	}

	if (!left_resync&&x > 0) {
		num_cand++;
		last_cand = 1;
         lz =  pred_mv_block[0][2];
         lpos   = pred_mv_block_pos[0][0];
		pmv[1] = mb[lpos].mvs[lz];
	} else {
		pmv[1] = zeroMV;
	}

	if (!top_resync&&y > 0) {
		num_cand++;
		last_cand = 2;
        tpos  = pred_mv_block_pos[0][1];
        tz =  pred_mv_block[0][5];
		pmv[2]= mb[tpos].mvs[tz];
	} else {
		pmv[2] = zeroMV;
	}

	if (!(top_resync-1)&&(y > 0) && (x + 1) < mb_width) {
		num_cand++;
		last_cand = 3;
        rz =  pred_mv_block[0][8];
        rpos  = pred_mv_block_pos[0][2];
		pmv[3] = mb[rpos].mvs[rz];
	} else {
		pmv[3] = zeroMV;
	}

	// if only one valid candidate preictor, the invalid candiates are set to the canidate
	if (num_cand == 1) {
		pmv[0] = pmv[last_cand];
		return;
	}

	//get median value
	VOGETMEDIAN(pmv[0].x,pmv[1].x, pmv[2].x,pmv[3].x);
    VOGETMEDIAN(pmv[0].y,pmv[1].y, pmv[2].y,pmv[3].y);		

}

#endif //MPEG4_RESYNC
