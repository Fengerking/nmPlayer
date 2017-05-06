#include <string.h>
#include "voMpegEncPrediction.h"
#include "voMpegEncMB.h"
#include "voMpegEncBits.h"
#include "voMpegEncFrameProc.h"
#include "motionestimation/voMpegEncME.h"
#include "ratecontrol/voMpegEncRC.h"
#include <math.h>

#ifdef RECON_FILE
extern FILE *rc_file;
#endif

#define UPDATEIMGPOINT(local_cur, cur, x, y, stride){\
	local_cur.y = cur->y + ((y * stride + x) << 4);			\
	local_cur.uv[0] = cur->uv[0] + ((y * (stride >> 1) + x) << 3);	\
	local_cur.uv[1] = cur->uv[1] + ((y * (stride >> 1) + x) << 3);	\
}

#define CalcCBP CalcCbp_C
VO_S32 CalcCbp_C(const VO_S16 *qcoeff)
{
	VO_U32 i = 6;
	VO_U32 cbp = 0;
    VO_U64 *qcoeff64;
    VO_U32 *qcoeff32;

    for(i=0;i<6;i++) {
        qcoeff64 = (VO_U64*)qcoeff;	// the compiler doesn't really make this
		qcoeff32 = (VO_U32*)qcoeff;	// variables, just "addressing modes"

        cbp = cbp<<1;
        
        if (qcoeff[1] || qcoeff32[1]) {
			cbp++;
		}
        else if (qcoeff64[1] | qcoeff64[2] | qcoeff64[3]) {
			cbp++;
		}
        else if (qcoeff64[4] | qcoeff64[5] | qcoeff64[6] | qcoeff64[7]) {
			cbp++;
		}
        else if (qcoeff64[8] | qcoeff64[9] | qcoeff64[10] | qcoeff64[11]) {
			cbp++;
		}
        else if (qcoeff64[12] | qcoeff64[13] | qcoeff64[14] | qcoeff64[15]) {
			cbp++;
		}
		qcoeff += 64;
    }
	return cbp;
}

static __inline VO_VOID InitIntraMB(MACROBLOCK * pMB)
{
	// zero mv statistics 
    memset(pMB->mvs,0,4*sizeof(VECTOR));
    memset(pMB->sad8,0,4*sizeof(VO_U32));
    pMB->mode = MODE_INTRA;	
	pMB->dquant = 0;
	pMB->sad16  = 0;
	pMB->ac_dir = 0;
}

RETURN_CODE EncodeIVOP( ENCHND * const enc_hnd, 
                                       BitStream * const bs, 
                                       const VO_S32 vt_quant )
{
	ENCLOCAL * const enc_local = &enc_hnd->enc_local;
    VO_S16 *qcoeff	   = (VO_S16*)enc_hnd->work_space;
	VO_S16 *dct_codes = (VO_S16*)enc_hnd->work_space + MB_SIZE;
    //VO_U32 *resync_start =  bs->tail;

	const VO_U32 al_flag = enc_local->algorithm_flag;	
    const VO_U32 mb_width = enc_hnd->mb_width;
	const VO_U32 mb_height = enc_hnd->mb_height;
    VO_S32 pre_mb_quant;
    VO_U32 x, y;	
#ifdef P_DOMAIN_RC
	VO_S16 *dct_coeff_mb;	
#endif //P_DOMAIN_RC
	enc_local->coding_type = I_VOP;
	enc_local->rounding = (enc_hnd->codec_id != VO_INDEX_ENC_H263);
	enc_hnd->resync_marker_bits = 17;

#ifdef P_DOMAIN_RC
	enc_hnd->sStat.mb_hr_bits = enc_hnd->sStat.mv_bits = \
		enc_hnd->sStat.mv_sum = enc_hnd->sStat.mv_num = enc_hnd->sStat.mv_all = 0;
	enc_hnd->sStat.text_bits = 0;
#endif
	//enc_hnd->sStat.mode_intra = enc_hnd->img_mbsize;
	enc_local->quant = MpegRCBefore((RC_SINGLE*)enc_hnd->rc_control, I_VOP, enc_hnd->codec_id);
#if defined(P_DOMAIN_RC)
	dct_coeff_mb = dct_codes;
	memset(((RC_SINGLE*)enc_hnd->rc_control)->pq_map, 0 , sizeof(PQ_MAP) * enc_hnd->img_mbsize);
	memset(((RC_SINGLE*)enc_hnd->rc_control)->pq_map_frame, 0 , sizeof(VO_S32) * 32);
//	memset(((RC_SINGLE*)enc_hnd->rc_control)->pq_map_frameP, 0 , sizeof(VO_S32) * 32);
	for (y = 0; y < mb_height; y++){
		for (x = 0; x < mb_width; x++) {
			Mpeg4Frame local_cur;

			UPDATEIMGPOINT(local_cur, enc_hnd->img_cur, x, y, enc_hnd->edged_width);
			MacroBlockTransDctIntra(&local_cur, enc_hnd->edged_width, dct_coeff_mb);
			IniIntraDctDTab((RC_SINGLE*)enc_hnd->rc_control, y * mb_width + x, dct_coeff_mb);

			dct_coeff_mb += MB_SIZE;
		}
	}
#endif	//P_DOMAIN_RC

    // Maybe TBD restrict the qp of intra frame
	pre_mb_quant = enc_local->quant; 
	
	if(enc_hnd->codec_id != VO_INDEX_ENC_H263)
		Mpeg4WriteVolHeader(enc_hnd, bs);
	
#if defined(P_DOMAIN_RC)
	RCIniFrame(&enc_hnd->sStat, (RC_SINGLE*)enc_hnd->rc_control, I_VOP, I_VOP, enc_hnd->enc_local.frame_mum, enc_hnd->max_Iframe );	
	enc_hnd->sStat.pic_hr_bits = 0;
#endif	
	enc_hnd->WritePicHeader(enc_local,enc_hnd->framerate_base, 
                                      enc_hnd->h263_pic_format, 
								      enc_hnd->img_width, 
                                      enc_hnd->img_height, 
                                      bs, 
                                      &enc_hnd->sStat);

	for (y = 0; y < mb_height; y++){
		for (x = 0; x < mb_width; x++) {
            Mpeg4Frame local_cur;
			MACROBLOCK *pMB = &enc_hnd->mbs[x + y * mb_width];

            InitIntraMB(pMB);
			UPDATEIMGPOINT(local_cur, enc_hnd->img_cur, x, y, enc_hnd->edged_width);			

			// get new mb quant and dquant and modify the mode
			pMB->quant = GetMBQP((RC_SINGLE*)enc_hnd->rc_control, 
                                            pre_mb_quant, 
                                            &pMB->dquant, 
                                            &pMB->mode, I_VOP);
            pre_mb_quant = pMB->quant;
			
			MacroBlockTransFormIntra(&local_cur,  
                                            enc_hnd->edged_width,
                                            pMB->quant,
				                            dct_codes, qcoeff, 
                                            enc_hnd->codec_id);
#ifdef MPEG4_RESYNC
			if(enc_hnd->vp_byte_size&&(VO_S32)(bs->tail - resync_start)>enc_hnd->vp_byte_size){
//				PadBitsAlways(bs, 1); /* next_start_code() */
				resync_start = bs->tail;
				VOMpeg4Resync(enc_hnd, bs, x, y, mb_width, pMB, 17);
			}
#endif//MPEG4_RESYNC			
			if(al_flag & VOI_ACDCPRED){
				ACDCPrediction(enc_hnd, pMB, al_flag, x, y, enc_hnd->mb_width, qcoeff);
			}

             pMB->cbp = CalcCBP(qcoeff);
			
			enc_hnd->EncIntraMB(enc_local,  pMB, qcoeff, bs, &enc_hnd->sStat);
#if defined(P_DOMAIN_RC)
			UpdateMBRC((RC_SINGLE*)enc_hnd->rc_control, &enc_hnd->sStat, pMB->quant, I_VOP);  
			dct_codes += MB_SIZE; 
#endif			
		}
	}

    enc_local->prev_mvsigma    = -1;
	enc_local->m_fcode = 2;
	enc_local->frame_mum = 0;

	PadBitsAlways(bs, enc_hnd->codec_id == VO_INDEX_ENC_MPEG4);	

	return MPEG_SUCCEEDED; // intra 
}

static __inline VO_VOID PredictFcode(Statistics * stat, ENCLOCAL * enc_local)
{
	int sigma;
	const VO_S32 search_range = (16 << enc_local->m_fcode);
	
	if (stat->mv_num == 0)
		stat->mv_num = 1;
	
	sigma = ((int)sqrt(1024*stat->mv_sum / stat->mv_num))>>5;	
	if ((3 * sigma > search_range) && (enc_local->m_fcode <= 5) ){
		enc_local->m_fcode++;
	}else if ((5 * sigma < search_range)
		&& (4 * enc_local->prev_mvsigma < search_range)
		&& (enc_local->m_fcode >= 2) ){
		enc_local->m_fcode--;
	}
	enc_local->prev_mvsigma = sigma;
}

static __inline VO_VOID UpdateACDCBuf(ENCHND * enc_hnd, const VO_U32 x)
{
	VO_S16 * const row = enc_hnd->pred_acdc_row + ((x + 1) << 5);
	VO_S16 * const col = enc_hnd->pred_acdc_col;
	//update the left_top pixel
    col[0] = row[8];
    col[16] = row[16];
    col[24] = row[24];	
}

static __inline VO_VOID ModeDecisionRefine(MACROBLOCK *pMB)
{
	VO_S32 not_code;

    if (pMB->dquant != 0) {
        pMB->mode = MODE_INTER_Q;     
    }
    else {
	    not_code = (pMB->cbp == 0) && (pMB->mode == MODE_INTER) && \
		    ((pMB->mvs->x | pMB->mvs->y) == 0);
    	
	    if( not_code){ //TBD check the skip_possible
		    pMB->mode = MODE_NOT_CODED;	
	    }
    }	
}

RETURN_CODE EncodePVOP(ENCHND * const enc_hnd, BitStream * const bs)
{
    Mpeg4Frame *const img_ref			= enc_hnd->img_ref;
	Mpeg4Frame *const img_cur			= enc_hnd->img_cur;
	ENCLOCAL * const enc_local		= &enc_hnd->enc_local;
	const VO_U32 mb_width			= enc_hnd->mb_width;
	const VO_U32 mb_height		= enc_hnd->mb_height;
	const VO_S32 edged_width		= enc_hnd->edged_width;
    //const VO_S32 pre_frame_type    = enc_local->coding_type;
    const VO_U32 al_flag			= enc_local->algorithm_flag;
	VO_S32 pre_mb_quant;
    VO_U32 x, y;
	
#ifdef P_DOMAIN_RC
	VO_S16 *dct_coeff_mb;
    const VO_S32 pre_frame_type    = enc_local->coding_type;
#endif
	//VO_U32 *resync_start =  bs->tail;
	//VO_S32 resync_pos = 0;

	VO_S16 *qcoeff    = (VO_S16*)enc_hnd->work_space;
	VO_S16 *dct_codes = (VO_S16*)enc_hnd->work_space + 64 * 6;

	enc_local->coding_type = P_VOP;

	if(enc_hnd->codec_id == VO_INDEX_ENC_MPEG4)
		enc_local->rounding = 1 - enc_local->rounding;

	enc_hnd->sStat.mv_sum = enc_hnd->sStat.mv_num = 0;
	//enc_hnd->sStat.mode_intra =
    enc_hnd->sStat.mv_all = 0;

	enc_local->quant = MpegRCBefore((RC_SINGLE*)enc_hnd->rc_control, P_VOP, enc_hnd->codec_id);

	//dp = 0 for me now
	for (x = 0; x < mb_height; ++x){
		for (y = 0; y < mb_width; ++y) {
			enc_hnd->mbs[x * mb_width + y].dquant = 0;
		}	
	}	
		
	if(al_flag & VOI_RESTRICT_MV){		
		FrameSetEdge(img_ref, edged_width, enc_hnd->img_width, enc_hnd->img_height);
	}

	if(MotionEstimation(enc_hnd)){
//#ifndef VT   // fixed 2011.7.14 
		enc_local->coding_type = I_VOP;
		enc_hnd->force_iframe_num = FORCE_INTRA_NUM;
		return MPEG_SUCCEEDED;
//#endif
	}

	pre_mb_quant = enc_local->quant; 

#if defined(P_DOMAIN_RC)
	dct_coeff_mb = dct_codes;  

	memset(((RC_SINGLE*)enc_hnd->rc_control)->pq_map, 0 , sizeof(PQ_MAP) * enc_hnd->img_mbsize);
	memset(((RC_SINGLE*)enc_hnd->rc_control)->pq_map_frame, 0 , sizeof(VO_S32) * 32);
//	memset(((RC_SINGLE*)enc_hnd->rc_control)->pq_map_frameP, 0 , sizeof(VO_S32) * 32);

	for (y = 0; y < mb_height; y++) {
		for (x = 0; x < mb_width; x++) {
			
			MACROBLOCK *pMB = &enc_hnd->mbs[x + y * mb_width];
			Mpeg4Frame local_cur, local_ref;
			
//			if(x + y * mb_width == 230)
//				x = x;
			UPDATEIMGPOINT(local_cur, img_cur, x, y, edged_width);
			UPDATEIMGPOINT(local_ref, img_ref, x, y, edged_width);
		
			if (pMB->mode == MODE_INTRA) {
				InitIntraMB(pMB);
				MacroBlockTransDctIntra(&local_cur, enc_hnd->edged_width, dct_coeff_mb);
				IniIntraDctDTab((RC_SINGLE*)enc_hnd->rc_control, y * mb_width + x, dct_coeff_mb);		
				dct_coeff_mb += MB_SIZE;
//				iiii++;
				continue;
			}//else{
			//	UpdateACDCBuf(enc_hnd, x);
			//}
			
			MacroBlockMotionCompensation(&local_ref, &local_cur, pMB, 
				dct_coeff_mb, edged_width, enc_local->rounding);
			
			if (pMB->cbp != 0) {
				MacroBlockFDCT(dct_coeff_mb);
//				IniInterDctDTab((RC_SINGLE*)enc_hnd->rc_control, y * mb_width + x, dct_coeff_mb);
			}else{
//				memset(&((RC_SINGLE*)(enc_hnd->rc_control))->pq_map[y * mb_width + x], 0, sizeof(VO_S32)*32);
				memset(dct_coeff_mb, 0, sizeof(VO_S16)*384);
			}
			IniInterDctDTab((RC_SINGLE*)enc_hnd->rc_control, y * mb_width + x, dct_coeff_mb);
			
			dct_coeff_mb += MB_SIZE;
		}
	}	

#endif	

#if defined (P_DOMAIN_RC)
	pre_mb_quant = enc_local->quant = RCIniFrame(&enc_hnd->sStat, (RC_SINGLE*)enc_hnd->rc_control, P_VOP
		, pre_frame_type, enc_hnd->enc_local.frame_mum, enc_hnd->max_Iframe);
	enc_hnd->sStat.pic_hr_bits = 0;
#endif

	enc_hnd->WritePicHeader(enc_local, 
                                    enc_hnd->framerate_base, 
                                    enc_hnd->h263_pic_format, 
								    enc_hnd->img_width, 
                                    enc_hnd->img_height, 
                                    bs, 
                                    &enc_hnd->sStat);
#if defined (P_DOMAIN_RC)
	enc_hnd->sStat.text_bits = enc_hnd->sStat.mb_hr_bits = enc_hnd->sStat.mv_bits = 0;
#endif

	for (y = 0; y < mb_height; y++) {
		for (x = 0; x < mb_width; x++) {
            Mpeg4Frame local_cur, local_ref;			
			MACROBLOCK *pMB = &enc_hnd->mbs[x + y * mb_width];
			UPDATEIMGPOINT(local_cur, img_cur, x, y, edged_width);
			UPDATEIMGPOINT(local_ref,  img_ref, x, y, edged_width);

             if( y == 1 && x==31)
				x = x;

			//get new mb quant
			pMB->quant = GetMBQP((RC_SINGLE*)enc_hnd->rc_control, pre_mb_quant, &pMB->dquant, &pMB->mode, P_VOP );

            pre_mb_quant = pMB->quant;

#ifdef MPEG4_RESYNC					
			if(enc_hnd->vp_byte_size&&(VO_S32)(bs->tail - resync_start)>enc_hnd->vp_byte_size){
				resync_start = bs->tail;
				VOMpeg4Resync(enc_hnd, bs, x, y, mb_width, pMB, 16 + enc_local->fcode_for);
				resync_pos = mb_width;
			}
#endif// MPEG4_RESYNC

			if (pMB->mode == MODE_INTRA || pMB->mode == MODE_INTRA_Q) {

				MacroBlockTransFormIntra(&local_cur, 
                                                    edged_width,
                                                    pMB->quant, 
                                                    dct_codes, 
                                                    qcoeff, 
                                                    enc_hnd->codec_id);
				
				if(al_flag & VOI_ACDCPRED)
					ACDCPrediction(enc_hnd, pMB, al_flag, x, y, mb_width, qcoeff);

                 pMB->cbp = CalcCBP(qcoeff);
								
				enc_hnd->EncIntraMB(enc_local,  pMB, qcoeff, bs, &enc_hnd->sStat);

#if defined(P_DOMAIN_RC)
				UpdateMBRC((RC_SINGLE*)enc_hnd->rc_control, &enc_hnd->sStat, pMB->quant, P_VOP);   
				dct_codes += MB_SIZE;  
#endif			
				continue;
			}else{
				UpdateACDCBuf(enc_hnd, x);
			}
#ifndef P_DOMAIN_RC			
			MacroBlockMotionCompensation(&local_ref, 
                                                    &local_cur, 
                                                    pMB, 
                                                    dct_codes, 
                                                    edged_width, 
                                                    enc_local->rounding);
#endif			
			if (pMB->cbp)
				pMB->cbp = MacroBlockTransFormInter(&local_cur, 
                                                                    edged_width,
                                                                    pMB->quant, 
                                                                    dct_codes, 
                                                                    qcoeff, 
                                                                    al_flag, 
                                                                    enc_hnd->codec_id);
			ModeDecisionRefine(pMB);
			if(pMB->mode == MODE_NOT_CODED){
				MBSkip(bs);
			}else{
#ifdef MPEG4_RESYNC
				if(resync_pos){
					if(pMB->mode == MODE_INTER4V){
						RGetPMVBlock(pMB, mb_width, x, y, resync_pos== mb_width, resync_pos);
					}else{
						RGetPMVMacroBlock(pMB, mb_width, x, y, resync_pos== mb_width, resync_pos);
					}
				}
#endif //MPEG4_RESYNC
				enc_hnd->EncInterMB(enc_local, pMB, qcoeff, bs, &enc_hnd->sStat, &local_cur, edged_width);					
			}

#if defined(P_DOMAIN_RC)
			UpdateMBRC((RC_SINGLE*)enc_hnd->rc_control, &enc_hnd->sStat, pMB->quant, P_VOP);  
			dct_codes += MB_SIZE;
#endif	//P_DOMAIN_RC
#ifdef MPEG4_RESYNC
			if(resync_pos)
				resync_pos--;
#endif //MPEG4_RESYNC
		}
	}		
	PadBitsAlways(bs,enc_hnd->codec_id == VO_INDEX_ENC_MPEG4);	
	PredictFcode(&enc_hnd->sStat, &enc_hnd->enc_local);	
	return MPEG_SUCCEEDED;
}
