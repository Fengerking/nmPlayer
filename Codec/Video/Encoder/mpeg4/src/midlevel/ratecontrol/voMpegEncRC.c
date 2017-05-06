/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include "voMpegEncRC.h"
#include "../../lowlevel/voMpegEncTable.h"
#include "../../midlevel/voMpegMemmory.h"

//[1]¡°Scalable Rate Control for MPEG-4 Video¡±£¬
//[2]¡°MPEG-4 rate control for multiple video objects¡±£¬
//[3]¡°A Novel Rate Control Scheme for Video Over the Internet¡±
//[4]"Adaptive Basic Unit Layer Rate Control for JVT"

#if defined (OLD_RC)
//#define DEFAULT_INITIAL_QUANTIZER 4

#define DEFAULT_BITRATE 900000	/* 900kbps */
#define DEFAULT_DELAY_FACTOR 16
#define DEFAULT_AVERAGING_PERIOD 100
#define DEFAULT_BUFFER 100

VO_S32 MpegRCCreate( ENCHND *enc_hnd, 
                        RC_CREATE * rc_create, 
                        VO_S32 framerate_incr, 
                        VO_S32 framerate_base, 
                        const VO_U32 img_mbsize, 
                        RC_SINGLE ** handle )
{
	RC_SINGLE *rc;
	VO_S32 i;
	const VO_S32 bitrate = rc_create->bitrate;

    if(bitrate == 0) {
		*handle = NULL;
		return 0;
	}

	if (framerate_incr == 0)
		return -1;
	
	// Allocate internal rate control structure 
	if ((rc = (RC_SINGLE *)voMpegMalloc(enc_hnd, sizeof(RC_SINGLE), CACHE_LINE)) == NULL)
		return INVALID_INTERNAL_MEM_MALLOC;

	// Constants
	rc->byte_rate	=	(bitrate > 0) ? bitrate / 8 : DEFAULT_BITRATE / 8;
	rc->expected_framesize =(double) rc->byte_rate / ((double)framerate_base / framerate_incr);
	rc->reaction_delay_factor =	(rc_create->reaction_delay_factor > 0) ? rc_create->reaction_delay_factor : DEFAULT_DELAY_FACTOR;
	rc->averaging_period = (rc_create->averaging_period > 0) ? rc_create->averaging_period : DEFAULT_AVERAGING_PERIOD;
	rc->buffer = (rc_create->buffer > 0) ? rc_create->buffer : DEFAULT_BUFFER;

	rc->time = 0;
	rc->total_framesize = 0;
	rc->acc_quant = 4; //GetIniQuant(rc_create->bitrate);

	// Reset quant error accumulators 
	for (i = 0; i < 31; i++)
		rc->quant_error[i] = 0.0;

	// Last bunch of variables 
	rc->sequence_quality = 2.0 / (double) rc->acc_quant;
	rc->avg_framesize = rc->expected_framesize;
	//rc->fq_error = 0;

	// Bind the RC
	*handle = rc;
	return (0);
}

VO_S32 MpegRCDestroy(ENCHND *enc_hnd, RC_SINGLE * rc)
{
	voMpegFree(enc_hnd, rc);
	return (0);
}

VO_S32 MpegRCBefore(RC_SINGLE * rc,  const VO_S32 type, const VO_U32 codec_id)
{
	if(rc == NULL){
		rc->quant = (type == I_VOP) ? QPI: QPP;
	}else{
		rc->quant = rc->acc_quant;
		// limit to min/max range 
        VOCLAMP(rc->quant ,MIN_QP,MAX_QP);		
	}
	return rc->quant;
}

VO_S32 MpegRCAfter(RC_SINGLE * rc, VO_S32 framerate_incr, VO_S32 framerate_base, VO_S32 length, VO_U32 type)
{
	VO_S64 deviation;
	VO_S32 acc_quant,quant = rc->quant;
	double overflow,averaging_period;
	double reaction_delay_factor;
	double quality_scale,	 base_quality, target_quality;
    VO_S32 delt_quant =0;
    
	if(rc == NULL)
		return (0);
	// Update time and total_framesize
	// TBD time and toal_size will owerflow ????
	rc->time += (double) framerate_incr / framerate_base;
	rc->total_framesize += length;

	// calculate the sequence quality
    averaging_period = (double) rc->averaging_period;
    rc->sequence_quality = rc->sequence_quality - rc->sequence_quality / averaging_period
        + 2.0 / (double)quant / averaging_period;

    // clamp the sequence quality to [10%, 100%]
	// to try to avoid using the highest
	// and lowest quantizers 'too' much 
    VOCLAMP(rc->sequence_quality,0.1,1.0);

	// factor this frame's size into the average framesize
	// but skip using ivops as they are usually very large
	// and as such, usually disrupt quantizer distribution
	if (type != I_VOP) {
		reaction_delay_factor = (double) rc->reaction_delay_factor;
		rc->avg_framesize -= rc->avg_framesize / reaction_delay_factor;
		rc->avg_framesize += length / reaction_delay_factor;
    }

	// calculate the quality_scale which will be used
	// to drag the target quality up or down, depending
	// on if avg_framesize is >= expected_framesize
    quality_scale = rc->expected_framesize / rc->avg_framesize;
    quality_scale *=quality_scale;

	// use the current sequence_quality as the
	// base_quality which will be dragged around 
	// 0.06452 = 6.452% quality (quant:31)
	base_quality = rc->sequence_quality;

    if(quality_scale < 1.0)
    {
        base_quality = 0.06452 + (base_quality - 0.06452) * quality_scale;
    }
    else
    {
        base_quality = 1.0 - (1.0 - base_quality) / quality_scale;
    }

    // Compute the deviation from expected total size 
	deviation = (VO_S64)(rc->total_framesize - rc->byte_rate * rc->time);
	overflow = -((double) deviation / (double) rc->buffer);

	//To avoid a large bitrate following still scenes
    VOCLAMP(overflow,-rc->expected_framesize, rc->expected_framesize);

	target_quality = base_quality + (base_quality - 0.06452) * overflow / rc->expected_framesize;

	// do clamp;
    //the acc_quant should clamp to quant 1-31(target_quality: 0.06452, 2.0)
    VOCLAMP(target_quality, 0.06452, 2.0);
	acc_quant = (VO_S32) (2.0 / target_quality);

	if (acc_quant > 0 && acc_quant < 31) 
    {
		rc->quant_error[acc_quant - 1] += 2.0 / target_quality - acc_quant;
		if (rc->quant_error[acc_quant - 1] >= 1.0) 
        {
			rc->quant_error[acc_quant - 1] -= 1.0;
			acc_quant++;
			rc->acc_quant++;
		}
	}

    delt_quant = acc_quant-rc->acc_quant;
	//let quantization change not too fast
    if (delt_quant > 5) 		
        acc_quant = rc->acc_quant + 3;
    else if (delt_quant > 3)
        acc_quant = rc->acc_quant + 2;
    else if(delt_quant > 1)
        acc_quant = rc->acc_quant + 1;
    else if(delt_quant < -5)
        acc_quant = rc->acc_quant - 3;
    else if(delt_quant < -3)
        acc_quant = rc->acc_quant - 2;
    else if(delt_quant < -1)
        acc_quant = rc->acc_quant - 1;

	rc->acc_quant = acc_quant;
	return (0);
}

VO_S32 GetMBQP(RC_SINGLE * rc, const VO_S32 pre_mb_quant, VO_S32 *dquant, VO_S32 *mb_mode, VO_U32 pic_type )
{
	VO_S32 cur_mb_quant = rc->quant;
	VO_S32 dq = cur_mb_quant - pre_mb_quant;
	
	if(*mb_mode != MODE_INTER4V){
		//dq to [-2, +2]
		dq = MIN(2, MAX(dq, -2));
		cur_mb_quant = pre_mb_quant + dq;		
		cur_mb_quant = MIN(31, MAX(cur_mb_quant, 2));
		*dquant = dq;
        if(dq){
            *mb_mode  +=1;
            /*if(*mb_mode == MODE_INTER) 
                *mb_mode = MODE_INTER_Q;
            else if(*mb_mode == MODE_INTRA)
                *mb_mode = MODE_INTRA_Q;*/
        }
	}else{
		*dquant = 0;
	}
	return cur_mb_quant;
}

#elif defined (P_DOMAIN_RC)

#include "voMpegEncRC.h"
#include <math.h>

#define HIGH_RATE			1700000
#define GENERAL_RATE        800000 
#define LOW_RATE            400000
#define QP_CLIP 12

//__declspec(align(32)) 
static VO_U8 table_coeff_to_index[154]=
{	32,31,31,30,30,		30,29,29,28,28,		28,27,27,26,26,		26,25,25,24,24,		
	24,23,23,22,22,		22,21,21,20,20,		20,19,19,18,18,		18,17,17,16,16,
	16,15,15,14,14,		14,13,13,12,12,		12,11,11,10,10,		10, 9, 9, 8, 8,
	 8, 7, 7, 6, 6,		 6, 5, 5, 4, 4,		 4, 3, 3, 2, 2,		 2, 1, 1, 1, 2,

	 2, 2, 3, 3, 4,		4, 4, 5, 5,	6,		 6, 6, 7, 7, 8,		 8, 8, 9, 9,10,
	10,10,11,11,12,		12,12,13,13,14,		14,14,15,15,16,		16,16,17,17,18,
	18,18,19,19,20,		20,20,21,21,22,		22,22,23,23,24,		24,24,25,25,26,
	26,26,27,27,28,		28,28,29,29,30,		30,30,31,31};


//static VO_S32 Bt = 100000; //buffer size

#define SIZE_I_FRM 1.5
#define SIZE_P_FRM 1

VO_S32 BilinearLookup(VO_S32 *pq_map_frame, VO_S32 Pc, VO_S32 SmallSize, VO_S32 LargeSize)
{
	int Step = LargeSize - SmallSize;

	if(VOGETABS(Step) == 1)
	{
	  if(VOGETABS(Pc - pq_map_frame[LargeSize]) > VOGETABS(Pc - pq_map_frame[SmallSize]))
		  return SmallSize;
	  else
		  return LargeSize;        
	}

	if(Pc > pq_map_frame[SmallSize + Step/2])
        return BilinearLookup(pq_map_frame, Pc, SmallSize + Step/2, LargeSize);
	else
        return BilinearLookup(pq_map_frame, Pc, SmallSize, SmallSize + Step/2);
   	
}

VO_S32 MpegRCCreate(ENCHND *enc_hnd, RC_CREATE * rc_create, VO_S32 framerate_incr, VO_S32 framerate_base, const VO_U32 img_mbsize, RC_SINGLE ** handle)
{
	RC_SINGLE *rc;
	float target_frame_rate;
	const VO_U32 bitrate  = rc_create->bitrate;
	const VO_U32 img_size = rc_create->img_size;

	if(bitrate == 0){
		*handle = NULL;
		return 0;
	}

	if (framerate_incr == 0){
		return -1;
	}
	
	/* Allocate internal rate control structure */
	if ((rc = (RC_SINGLE *)voMpegMalloc(enc_hnd, sizeof(RC_SINGLE), CACHE_LINE)) == NULL)
		return INVALID_INTERNAL_MEM_MALLOC;


	if (bitrate <= LOW_RATE) {
		if (img_size <= 25344) { //QCIF  176*144
			rc->iquant = 4;
		}else if (img_size <= 101376) {//CIF 352*288*/
			rc->iquant = 4;//original 4
		}else {
			rc->iquant = 8;//original 8
		}
	}else if (bitrate <= GENERAL_RATE) {
		if (img_size <= 25344) { //QCIF 176*144
			rc->iquant = 2;
		}else if (img_size <= 101376) {//CIF 352*288
			rc->iquant = 4;//original 4
		}else {
			rc->iquant = 4;//original 8
		}
	}else if (bitrate <= HIGH_RATE) {
		if (img_size <= 25344) { // QCIF 176*144
			rc->iquant = 2; // 12 to 16
		}else if (img_size <= 101376) {// CIF 352*288
			rc->iquant = 4;   //origianl 4
		}else {
			rc->iquant = 8;//origial 4
		}
	}else{
		if (img_size <= 101376) {//CIF 352*288
			rc->iquant = 4;   //origianl 4
		}
		else {
			rc->iquant = 4;//origial 4
		}
	}
	
	rc->img_mbsize = img_mbsize;
	rc->B_longtime = 0;
	rc->B_max = bitrate>>1; // set virtual buffer size will effect dela
	rc->B = (VO_S32)(0.2*rc->B_max);//initial buffer level

    //	rc->B = 0;
	//maloc memory for pq_map
	if(!(rc->pq_map = (PQ_MAP *)voMpegMalloc(enc_hnd, img_mbsize * sizeof(PQ_MAP), CACHE_LINE)))
		return (-1);

	target_frame_rate = (float)(framerate_base / framerate_incr); 

	rc->Rt = (VO_S32) (bitrate / target_frame_rate);
#ifdef BROADCOM_VT
	rc->Rti = rc->Rt;
	rc->Rtp = rc->Rt;
#else
	rc->Rti = (VO_S32)(SIZE_I_FRM * (VO_U32)(rc->Rt * rc_create->max_Iframe)/(rc_create->max_Iframe + SIZE_I_FRM - 1));
	rc->Rtp = (VO_S32)((rc->Rt * rc_create->max_Iframe)/(rc_create->max_Iframe + SIZE_I_FRM - 1));
#endif
	rc->theta_pre = 7.0f; //7.0  average value

	//Bind the RC
	*handle = rc;
	return (0);
}

VO_S32 MpegRCDestroy(ENCHND *enc_hnd, RC_SINGLE * rc)
{
	if(rc->pq_map){
		voMpegFree(enc_hnd, rc->pq_map);
		rc->pq_map = NULL;
	}

	voMpegFree(enc_hnd, rc);
	return (0);
}

VO_S32 GetMBQP(RC_SINGLE * rc,
                        const VO_S32 pre_mb_quant, 
				        VO_S32 *dquant, 
                        VO_S32 *mb_mode,
                        VO_S32 pic_type)
{
	VO_S32 cur_mb_quant, dq, Pc;	

    //the number of zero DCT coeffs of remain MBs  should be ...
	Pc = 384 * (rc->img_mbsize - rc->Nm) - (VO_S32)((rc->R - rc->Rm) / rc->theta);  
	cur_mb_quant = MIN(MAX(BilinearLookup((VO_S32 *)rc->pq_map_frame, Pc, 1, 31), 2), 31);

	if(rc->quality > QP_CLIP){
		if(cur_mb_quant > (QP_CLIP + 2)){
			cur_mb_quant = QP_CLIP + 2;
		}else if(cur_mb_quant < (QP_CLIP - 2)){
			cur_mb_quant = QP_CLIP - 2;
		}
	}

	if(pic_type == I_VOP){
		if(cur_mb_quant>10)
			cur_mb_quant = 10;
	}else{
		if(cur_mb_quant>12)
			cur_mb_quant = 12;
	}
	

//	if(!first_mb){
		dq = cur_mb_quant - pre_mb_quant;
//	cur_mb_quant = 30;	
//	dq=0;
		if(*mb_mode == MODE_INTRA || *mb_mode == MODE_INTER){
			/* dq to [-2, +2] */
			dq = MIN(2, MAX(dq, -2));
			cur_mb_quant = pre_mb_quant + dq;
			
			cur_mb_quant = MIN(31, MAX(cur_mb_quant, 2));
			*dquant = dq;
		}else{
			*dquant = 0;
			cur_mb_quant = pre_mb_quant;
		}
		
		if(*dquant){
			if(*mb_mode == MODE_INTRA){
				*mb_mode = MODE_INTRA_Q;
			}else if(*mb_mode == MODE_INTER){
				*mb_mode = MODE_INTER_Q;
			}
		}
//	}
	rc->qpall += cur_mb_quant;

	if(rc->qp_min>cur_mb_quant)
		rc->qp_min = cur_mb_quant;
	if(rc->qp_max<cur_mb_quant)
		rc->qp_max = cur_mb_quant;

	return cur_mb_quant;	
}

VO_VOID UpdateMBRC(RC_SINGLE *rc, Statistics *stat, VO_S32 QP, VO_S32 pic_type)
{
	VO_S32 i, Num, img_mbsize = rc->img_mbsize;
	
	Num = rc->Nm;
	
	/* update Pm and Rm */	

	rc->Pm +=  rc->pq_map[Num].pq_map_mb[QP];
	rc->Rm = stat->text_bits;
	rc->Nm++;

	/* update DCT coefficient distribution */
	for(i = 0; i < 32; i++) 
        rc->pq_map_frame[i] -= rc->pq_map[Num].pq_map_mb[i];

//(rc->Nm>= (VO_S32)(img_mbsize*0.1)) &&
	if(((rc->Nm * 384 - rc->Rm) != 0)&&rc->Rm){
		float theta_m;
		theta_m = (float) ((float)rc->Rm/ (float)(rc->Nm * 384 -rc->Pm));
		rc->theta  = ((img_mbsize * rc->Nm * theta_m) + (img_mbsize - rc->Nm) * (rc->Nm * theta_m + img_mbsize*rc->theta_pre))/\
				(img_mbsize * img_mbsize);

	//	rc->theta = (float) ((float)rc->Rm/ (float)(rc->Nm * 384 -rc->Pm)); 
	}
}

VO_S32 MpegRCAfter(RC_SINGLE * rc, VO_S32 framerate_incr, VO_S32 framerate_base, VO_S32 length, VO_S32 type)
{
	rc->theta_pre = rc->theta;

	/* uptate buffer */
	rc->B += (length<<3);
	rc->B -= rc->Rt;
	return 0;

}


VO_U32 RCIniFrame(Statistics *stat, RC_SINGLE * rc,  const VO_S32 cur_frame_type,
					const VO_S32 pre_frame_type, VO_S32 frame_num, VO_S32 iframe_num)
{	
	VO_S32  i, j;
    VO_U32 Pc, quant ;
	const VO_S32 img_mbsize = rc->img_mbsize;

	rc->Nm = rc->Rm = rc->Pm = 0;
	rc->qp_min = 31;
	rc->qp_max = 0;

	if( cur_frame_type == I_VOP){// I_VOP
		rc->R = (VO_S32)(rc->Rti + rc->Rt * (0.2 - ((float)rc->B / (float)rc->B_max)));// - (VO_S32)(0.2 * rc->B) + (VO_S32)(0.4 * rc->B_longtime);
		rc->R -= (stat->pic_hr_bits + (img_mbsize<<3));	//(img_mbsize<<3) estimation for mb header
	}else{ // P_VOP
		//under_bits = 
		rc->R = (VO_S32)(rc->Rtp + rc->Rt*(0.2 + (float)((rc->Rti - rc->Rt)*(iframe_num-frame_num))/(rc->B_max*(iframe_num-1)) 
            - ((float)rc->B/(float)rc->B_max) ));
		//if( cur_frame_type == I_VOP)
		//	rc->R -= (stat->pic_hr_bits + (img_mbsize<<3));
		//else
			rc->R -= (stat->pic_hr_bits + stat->mb_hr_bits + stat->mv_all);
		}

	rc->theta = rc->theta_pre;

	for(i = 0; i < img_mbsize; i++){
		for(j = 1; j < 32; j++){
			//if(rc->pq_map[i].mb_type == MODE_INTRA){/* INTRA*/
				rc->pq_map[i].pq_map_mb[j] += rc->pq_map[i].pq_map_mb[j - 1];
				rc->pq_map_frame[j] += rc->pq_map[i].pq_map_mb[j];
			//}else{/* INTER and INTER_4MV*/
			//	rc->pq_map[i].pq_map_mb[j] += rc->pq_map[i].pq_map_mb[j - 1];
			//	rc->pq_map_frameP[j] += rc->pq_map[i].pq_map_mb[j];
			//}
		}
	}

//	rc->quality = rc->pq_map_frameI[31] + rc->pq_map_frameP[31];
	//if(cur_frame_type == P_VOP){
		Pc = 384 * rc->img_mbsize - (VO_S32)(rc->R / rc->theta);
		quant = (MIN(MAX(BilinearLookup((VO_S32 *)rc->pq_map_frame, Pc,1, 31),2),31));

		if(quant > QP_CLIP){
			quant = QP_CLIP;
			rc->R = (VO_S32)((img_mbsize * 384 - rc->pq_map_frame[QP_CLIP])*rc->theta) ;
		}
		rc->quality = quant;
		
		//if((quant < 4) && (under_bits<0)){
		//	quant = 4;
		//	rc->RP  = (VO_S32)((rc->mb_inter * 384 - rc->pq_map_frameP[4])*rc->theta);
		//}
		
		//if(quant > 11){
		//	quant = 11;
		//	rc->RP  = (VO_S32)((rc->mb_inter * 384 - rc->pq_map_frameP[11])*rc->theta) ;
		//}
//		quant = 30;
	//}

	//printf("\n start=%d type=%d, : %f, ", frame_num, cur_frame_type, rc->theta);
	return quant;
}


VO_S32 MpegRCBefore(RC_SINGLE * rc,  const VO_S32 type, const VO_U32 codec_id)
{	
	VO_S32 quant;//, m;
	
	if( type == I_VOP){// I_VOP
		quant = (codec_id == VO_INDEX_ENC_MPEG4) ? 6 :6;
	}else{ // P_VOP
		quant = rc->qpall / rc->img_mbsize;
	}
	if(quant > QP_CLIP)
		quant = QP_CLIP;

	rc->qpall = 0;
	return quant;
}

VO_VOID IniIntraDctDTab(RC_SINGLE *rc, const VO_S32 pos, VO_S16 *dct_coeff_mb)
{
    VO_S32 i;
    VO_S32 *pq_map_mb = (VO_S32 *)(rc->pq_map[pos].pq_map_mb);
	
    rc->pq_map[pos].mb_type = MODE_INTRA;

    for (i = 0; i < MB_SIZE; i++) {
        VO_S32 m = *(dct_coeff_mb + i);
					
        m=VOGETABS(m);
        m =  (m+1) >> 1;
        if(m < 32) 
            pq_map_mb[m]++;				
    }
}

VO_VOID IniInterDctDTab(RC_SINGLE *rc, const VO_S32 pos, VO_S16 *dct_coeff_mb)
{
    VO_S32 i;
    VO_S32 *pq_map_mb = (VO_S32 *)(rc->pq_map[pos].pq_map_mb);
	
    rc->pq_map[pos].mb_type = MODE_INTER;

    for (i = 0; i < MB_SIZE; i++){
        VO_S32 m;
		
        m = (*(dct_coeff_mb + i)) + 77; 	
        if((m < 154) && (m > 0))
            pq_map_mb[table_coeff_to_index[m]] ++;		
    }
}

VO_S32 SkipFrame(RC_SINGLE * rc)
{
    return (rc->B > rc->B_max);
}
#endif
