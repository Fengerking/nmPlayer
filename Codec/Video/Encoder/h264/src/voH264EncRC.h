/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#ifndef _VOH264ENC_RC_H_
#define _VOH264ENC_RC_H_

VO_S32  pRCreate( H264ENC * );

void RCBefore( H264ENC *,VO_S32 nOverHead );
void RCMBUpdate( H264ENC *, H264ENC_L *pEncLocal );
VO_S32  RCMBQp( H264ENC * );
VO_S32  RCMBEnd( H264ENC *, VO_S32 nPicLen );
struct RateControl
{
  /* constants */
  double fps;
  double bitrate;

  /* current frame */
  VO_S32 qp;                     /* qp for current frame */
  float f_qpm;                /* qp for current macroblock: precise float for AQ */
  float qpa_rc;               /* average of macroblocks' qp before aq */

  /* ABR stuff */
  double last_rceq;
  double cplxr_sum;           /* sum of bits*qscale/rceq */
  double target_bitrate_frame;  /* target bitrate * window */
  double complexity_sum;
  double complexity_count;    
  double i_qp;          /* for determining I-frame quant */
  double i_qp_rate;
  double i_qp_offset;
  double last_qpscale[5];  /* last qscale for a specific pict type, used for max_diff & ipb factor stuff  */
  VO_S32 last_pictype_none_b;
    
  double qp_min[5];             /* min qscale by frame type */
  double qp_max[5];
  double lstep;               /* max change (multiply) in qscale per frame */
  VO_S64 bits_used;
  //VO_S64 bits_second;
};



#endif//_VOH264ENC_RC_H_

