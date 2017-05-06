/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#ifndef _CBR_RC_H
#define _CBR_RC_H
#include "../../voMpegEnc.h"
#include "../../voMpegProtect.h"

#if defined (OLD_RC)
typedef struct
{
	VO_S32  quant;
	VO_S32  reaction_delay_factor;
	VO_S32  averaging_period;
	VO_S32  buffer;
	VO_S32  byte_rate;// bytes per second
	double   expected_framesize;
	double   time;
	VO_S64  total_framesize;
	VO_S32  acc_quant;
	double   sequence_quality;
	double   avg_framesize;
	double   quant_error[31];
	//double   fq_error;
}RC_SINGLE;

//VO_S32 GetIniQuant(VO_U32 bitrate);
VO_S32 MpegRCCreate(ENCHND *enc_hnd, 
                       RC_CREATE *param, 
                       VO_S32 framerate_incr, 
                       VO_S32 framerate_base, 
                       const VO_U32 img_mbsize, 
                       RC_SINGLE ** handle );

VO_S32 MpegRCDestroy(ENCHND *enc_hnd, RC_SINGLE * rc);

VO_S32 MpegRCBefore(RC_SINGLE * rc, const VO_S32 type, const VO_U32 codec_id);

VO_S32 MpegRCAfter(RC_SINGLE * rc, 
                    VO_S32 framerate_incr, 
                    VO_S32 framerate_base, 
                    VO_S32 length, 
                    VO_U32 type);

VO_S32 GetMBQP(RC_SINGLE * rc, 
                       const VO_S32 pre_mb_quant, 
                       VO_S32 *dquant, 
                       VO_S32 *mode, 
                       VO_U32 pic_type );

#elif defined (P_DOMAIN_RC)

typedef  struct
{
	VO_U32 mb_type;
	VO_U32 pq_map_mb[32];
} PQ_MAP;

typedef struct
{
	PQ_MAP *pq_map;
	VO_U32 pq_map_frame[32];
	VO_U32 Qnz[32];
	VO_U32 Qz[32];
	VO_U32 Qz_map[32];
	VO_S32 iquant;
	VO_S32 img_mbsize;
	VO_S32 qp_min;
	VO_S32 qp_max;
	
	VO_S32 Rt;/*target bit of a frame */	
	VO_S32 R;
	VO_S32 B;
	VO_S32 Rm;  // the bits of encoded 
	VO_S32 Nm;  // the number of encoded MBs
	VO_S32 Pm;  // the number of zero of current MB
    VO_S32 B_min;
    VO_S32 B_max;
	VO_S32 B_longtime; 
    VO_S32 Rti;//iTarget_Bitrate for current frame
    VO_S32 Rtp;
    float theta;
	float theta_pre;
	VO_S32 quality;
	VO_S32 qpall;
}RC_SINGLE;

VO_VOID UpdateRateControl(int bits);

VO_S32 MpegRCCreate(ENCHND *enc_hnd, 
                              RC_CREATE * rc_create, 
                              VO_S32 framerate_incr, 
                              VO_S32 framerate_base, 
                              const VO_U32 img_mbsize, 
                              RC_SINGLE ** handle);

VO_S32 MpegRCDestroy(ENCHND *enc_hnd, RC_SINGLE * rc);


VO_VOID UpdateMBRC(RC_SINGLE *rc, Statistics *stat, VO_S32 QP, VO_S32 pic_type);
VO_S32 MpegRCAfter(RC_SINGLE * rc, 
                            VO_S32 framerate_incr, 
                            VO_S32 framerate_base, 
                            VO_S32 length, 
                            VO_S32 type);


VO_U32 RCIniFrame(Statistics *stat, 
                          RC_SINGLE * rc,  
                          const VO_S32 cur_frame_type, 
                          const VO_S32 pre_frame_type,
                          VO_S32 frame_num,
                          VO_S32 iframe_num);

VO_VOID IniIntraDctDTab(RC_SINGLE *rc, const VO_S32 pos, VO_S16 *dct_coeff_mb);

VO_S32 MpegRCBefore(RC_SINGLE * rc,  const VO_S32 type, const VO_U32 codec_id);
VO_S32 GetMBQP(RC_SINGLE * rc,
                        const VO_S32 pre_mb_quant, 
				        VO_S32 *dquant, 
                        VO_S32 *mb_mode,
                        VO_S32 pic_type);
VO_VOID IniInterDctDTab(RC_SINGLE *rc, const VO_S32 pos, VO_S16 *dct_coeff_mb);
VO_S32 SkipFrame(RC_SINGLE * rc);

extern VO_S32 DQtab[5];


#endif
#endif //<!_CBR_RC_H
