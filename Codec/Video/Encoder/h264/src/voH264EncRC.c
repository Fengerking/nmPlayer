/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#define _ISOC99_SOURCE
#undef NDEBUG // always check asserts, the speed effect is far too small to disable them
#include <math.h>

#include "voH264EncGlobal.h"
#include "voH264EncRC.h"
#include "voH264EncME.h"



float EstimateQScale( H264ENC *pEncGlobal );

VO_S32 pRCreate( H264ENC *pEncGlobal )
{
  RateControl *rc;
  VO_S32 tmp_size;
  VO_S32 i;

  //CHECKED_MALLOCZERO( pEncGlobal->rc, sizeof(RateControl) );
  tmp_size = sizeof(RateControl);
  CHECK_SIZE(tmp_size);
  MEMORY_ALLOC( pEncGlobal->rc, pEncGlobal->buffer_total , tmp_size, pEncGlobal->buffer_used,RateControl *);
  rc = pEncGlobal->rc;

  //set FPS
  if(pEncGlobal->OutParam.fps_num > 0 )
	rc->fps = (float) pEncGlobal->OutParam.fps_num;
  else
	rc->fps = 25.0;

  //edit by Really Yang 110114
  rc->bitrate = pEncGlobal->OutParam.i_bitrate * 1.0;
  //end of edit
  rc->last_pictype_none_b = -1;

	/* FIXME ABR_INIT_QP is actually used only in CRF */
#define ABR_INIT_QP ( 24 )
  rc->i_qp_rate = .01;
  rc->i_qp = ABR_INIT_QP * rc->i_qp_rate;
  /* estimated ratio that produces a reasonable QP for the first I-frame */
  rc->cplxr_sum = .01 * pow( 7.0e5, 0.6 ) * pow( pEncGlobal->mb_pic, 0.5 );
  rc->target_bitrate_frame = 1.0 * rc->bitrate / rc->fps;
  rc->last_pictype_none_b = IDR_PIC_TYPE;

  rc->i_qp_offset = 6.0 * log(1.4) / log(2.0);//pEncGlobal->InternalParam.f_ip_factor
  rc->lstep = pow( 2,4 / 6.0 );

  for( i = 0; i < 5; i++ )
  {
	rc->last_qpscale[i] = 0.85 * pow(2.0, ( ABR_INIT_QP - 12.0 ) / 6.0);
	rc->qp_min[i] = 0.85 * pow(2.0, ( pEncGlobal->InternalParam.i_qp_min - 12.0 ) / 6.0);
	rc->qp_max[i] = 0.85 * pow(2.0, ( pEncGlobal->InternalParam.i_qp_max - 12.0 ) / 6.0);
  }

  pEncGlobal->rc = rc;
  pEncGlobal->i_cost_est = (VO_S32)(rc->target_bitrate_frame * 16);
	
  return 0;
}

/* Before encoding a frame, choose a QP for it */
void RCBefore( H264ENC *pEncGlobal, VO_S32 nOverHead )
{
  RateControl *p_rc = pEncGlobal->rc;
  float qp;
  p_rc->qpa_rc = 0;
  qp = (float)(12.0 + 6.0 * log(EstimateQScale( pEncGlobal )/0.85) / log(2.0));
  qp = (float)(AVCClips3F( qp, pEncGlobal->InternalParam.i_qp_min, pEncGlobal->InternalParam.i_qp_max ));	
  p_rc->f_qpm = qp;
  p_rc->qp = AVSClip3( (VO_S32)(qp + 0.5), 0, 51 );
  p_rc->i_qp   *= .95;
  p_rc->i_qp_rate *= .95;
  p_rc->i_qp_rate += 1;
  if(pEncGlobal->pic_type == P_PIC_TYPE)
	p_rc->i_qp += p_rc->qp;
  else
	p_rc->i_qp += p_rc->qp + p_rc->i_qp_offset;
  pEncGlobal->i_cost_est = 0;
  p_rc->last_pictype_none_b = pEncGlobal->pic_type;
}

void RCMBUpdate( H264ENC *pEncGlobal, H264ENC_L *pEncLocal )
{
  RateControl *rc = pEncGlobal->rc;
  rc->qpa_rc += rc->f_qpm;
}

VO_S32 RCMBQp( H264ENC *pEncGlobal )
{
  return pEncGlobal->rc->qp;
}


/* After encoding one frame, save stats and update ratecontrol state */
VO_S32 RCMBEnd( H264ENC *pEncGlobal, VO_S32 nPicLen )
{
  RateControl *rc = pEncGlobal->rc;
  VO_S32 bits = nPicLen*8;
  rc->qpa_rc /= pEncGlobal->mb_pic;
  rc->cplxr_sum += bits * 0.85 * pow(2.0, ( rc->qpa_rc - 12.0 ) / 6.0) / rc->last_rceq;
  rc->target_bitrate_frame += rc->bitrate / rc->fps;
  rc->bits_used  += bits;
  //rc->bits_second += bits;
  return 0;
}


// update qscale for 1 frame based on actual bits used so far
float EstimateQScale( H264ENC *pEncGlobal )
{
  RateControl *p_rc = pEncGlobal->rc;
  VO_S32 pic_type = pEncGlobal->pic_type;
  VO_S64 bits_used = p_rc->bits_used;
  VO_S32 total_frame = pEncGlobal->nGFrameNum;
  double overflow_len = p_rc->bitrate * 2;
  double bits_expected = total_frame * p_rc->bitrate / p_rc->fps;
  double overflow=1, min_scale, max_scale;
  float qp_scale,compexity;
  //since we use last frame cost to estimate current frame,the estimate cost  for the first P frame after I frame is too large
  if(total_frame&&(p_rc->last_pictype_none_b == I_PIC_TYPE || p_rc->last_pictype_none_b == IDR_PIC_TYPE))
	pEncGlobal->i_cost_est >>= 3;
  p_rc->complexity_sum *= 0.5;
  p_rc->complexity_count *= 0.5;
  p_rc->complexity_sum += pEncGlobal->i_cost_est;
  p_rc->complexity_count ++;
  compexity = (float)(p_rc->complexity_sum / p_rc->complexity_count);
  qp_scale = (float)(pow( compexity, 0.4 ));
  p_rc->last_rceq = qp_scale;
  qp_scale /= (float)(p_rc->target_bitrate_frame / p_rc->cplxr_sum);
#if 0
	if(total_frame%(int)p_rc->fps == 0)
	{
		float dis = abs(p_rc->bits_second - p_rc->bitrate);
		dis *= 100;
		dis /= p_rc->bitrate;
		printf("\ncurrent bits:%lld, distence: %f\n",p_rc->bits_second,dis);
		p_rc->bits_second = 0;
	}
#endif
  // FIXME is it simpler to keep track of dExpectedLen in ratecontrol_end?
  if( bits_expected > 0 )
  {
	overflow_len *= AVC_MAX( 1, sqrt(total_frame/ p_rc->fps) );
	overflow = AVCClips3F( 1.0 + (bits_used - bits_expected) / overflow_len, .5, 2 );
	qp_scale *= (float)overflow;
  }
  //for I frame
  if( (pic_type == I_PIC_TYPE || pic_type == IDR_PIC_TYPE)&& pEncGlobal->OutParam.max_iframe_interval > 1
	 && p_rc->last_pictype_none_b == P_PIC_TYPE )
  {
	qp_scale = (float)(0.85 * pow(2.0, ( ( p_rc->i_qp / p_rc->i_qp_rate ) - 12.0 ) / 6.0));
	//qp_scale /= 1.4;//pEncGlobal->InternalParam.f_ip_factor
  }
  else if( pEncGlobal->nGFrameNum > 0 )//other frame
  {
	min_scale = p_rc->last_qpscale[pic_type] / p_rc->lstep;
	max_scale = p_rc->last_qpscale[pic_type] * p_rc->lstep;
	if( overflow > 1.1 && pEncGlobal->nGFrameNum > 3 )
	  max_scale *= p_rc->lstep;
	else if( overflow < 0.9 )
	  min_scale /= p_rc->lstep;
    qp_scale = (float)(AVCClips3F(qp_scale, min_scale, max_scale));
  }

  qp_scale = (float)(AVCClips3F(qp_scale, p_rc->qp_min[pic_type], p_rc->qp_max[pic_type]));
  p_rc->last_qpscale[pic_type] = qp_scale;

  if( pEncGlobal->nGFrameNum == 0 )
  {
	p_rc->last_qpscale[P_PIC_TYPE] = qp_scale ;//5h->InternalParam.f_ip_factor
	p_rc->complexity_sum /= 2;
  }
	
  return qp_scale;
}

