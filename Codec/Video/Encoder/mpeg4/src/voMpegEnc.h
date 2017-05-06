
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2005				*
	*																		*
	************************************************************************/

/*!
************************************************************************
*
* \file		voMpegEnc.h
*
* \brief
*		Internal header file for the VisualOn MPEG-4 encoder
*
************************************************************************
*/
 
#ifndef _ENCODER_H_
#define _ENCODER_H_

//#define NEW_SDK
#if defined(NDEBUG) || defined(_NDEBUG)
#define LICENSE_CHECK
#endif
#include "voMPEG4EncBack.h"
#include "lowlevel/voMpegEncWriteBits.h"
#include "voUtilits.h"

#if defined(DEBUG) || defined(_DEBUG)
//#define CAL_PSNR				//The PSNR data is at d:\Test\mpeg4enc_PNSR_log.txt.
//#define RECON_FILE			    //The YUV data is at d:\Test\recon1.yuv.
#endif

#if defined(CAL_PSNR) || defined(RECON_FILE)
#include "stdio.h"
#endif

//#define MAX_RAM_USED_CHECK	//Check how many RAM allocated and write the data into log file d:\Test\mpeg4enc_mem_log.txt.

///////////////
/**********************************************************************************
TBD clean up for API
*************************************************************************************/

//#define VT
//#define BROADCOM_VT
#define QPI			6
#define QPP			8
#define ACDIR_MSK	3


#ifdef NEW_SDK
#ifdef VT
#define P_DOMAIN_RC	
#else
#define OLD_RC	
#endif
#endif

//#define MPEG4_RESYNC	 1

/* global setting for encoder*/
#define SKIP_ME
#define SKIP_ME1

#define CONFIG_OK 0xFF00

/* --- macroblock modes --- */
#define MODE_INTER		0
#define MODE_INTER_Q 	1
#define MODE_INTER4V	    2
#define MODE_INTRA		3
#define MODE_INTRA_Q 	4
#define MODE_NOT_CODED	16
#define MODE_NOT_CODED_GMC 	17

/*MB_SIZE: 16*16+2*8*8 */
#define MB_SIZE			384

#define MAX_QP 31
#define	MIN_QP 2
/*
 * vop coding types
 * intra, prediction, backward, sprite, not_coded
 */
#define I_VOP	    0
#define P_VOP	    1
#define B_VOP	    2
#define S_VOP 	3
#define N_VOP	    4


//**********************************************
// algorithm flags
//**********************************************

#define VOI_HALFPEL              (1<< 1) /* use halfpel interpolation */
#define VOI_INTER4V              (1<< 2) /* use 4 motion vectors per MB */
#define VOI_TRELLISQUANT         (1<< 3) /* use trellis based R-D "optimal" quantization */
#define VOI_HQACPRED             (1<< 4) /* high quality ac prediction */

/* SAD operator based flags */
#define VOI_HALFPELREFINE16      (1<< 5)
#define VOI_HALFPELREFINE8       (1<< 6)
#define VOI_CHROMA_ME            (1<< 7) /* also use chroma for P_VOP/S_VOP ME */
#define VOI_RESTRICT_MV          (1<< 8)            
#define VOI_ADAPTIVE_IFRAME      (1<< 9)
#define VOI_ACDCPRED             (1<<10)

typedef VO_S32 bool;

#define CACHE_LINE 64

typedef struct
{
	VO_S32  packed;
	VO_S32  closed_gop;
	VO_S32  stats;
	VO_S32  gmc;
	VO_S32  qpel;
	VO_S32  interlace;
	VO_S32  mpeg_quant;
	VO_S32  vop_debug;
} INTERNAL_CONFIG;


/* single pass rate control
 * CBR and Constant quantizer modes */
typedef struct
{
	VO_S32			bitrate;               /* [in] bits per second */
	VO_U32		max_Iframe;	
	VO_U32		img_size;	
	VO_S32			reaction_delay_factor; /* [in] */
	VO_S32			averaging_period;      /* [in] */
	VO_S32			buffer;                /* [in] */
} RC_CREATE;

/*****************************************************************************
 * Structures
 ****************************************************************************/

typedef struct
{
	VO_S32 x;
	VO_S32 y;
}
VECTOR;

typedef struct
{
	VO_U8 *y;
	VO_U8 *uv[2];
}
Mpeg4Frame;

typedef struct
{
	VECTOR			mvs[4];
	VO_S32			mode;
	VO_S32			quant;					/* absolute quant */
	VO_S32			dquant;
	VECTOR			pmvs[4];
	VO_U32		sad8[4];			/* SAD values for inter4v-VECTORs */
	VO_U32		sad16;				/* SAD value for inter-VECTOR */
	VO_U32		cbp;
	VO_U32		ac_dir;			/* ACDC prediction direction last 12 bit for six block, 2 bit for one block*/
}
MACROBLOCK;

typedef struct
{
	VO_U32		algorithm_flag;
	VO_S32			quant;
	VO_U32		rounding;
	VO_U32		frame_mum;
	VO_S64			ref_stamp;
	VO_S64			cur_stamp;
	VO_U32		seconds;
	VO_U32		ticks;
	VO_U32		coding_type;
	VO_U32		m_fcode;
	VO_U32		fcode_for;
	VO_S32			prev_mvsigma;
} ENCLOCAL;


typedef struct
{
#ifdef P_DOMAIN_RC
	VO_U32		text_bits;
#endif
	VO_U32		pic_hr_bits;
	VO_U32		mb_hr_bits;
	VO_U32		mv_bits;
	VO_U32		mv_sum;
	VO_U32		mv_all;
	VO_U32		mv_num;
	VO_U32		iNumBitsMBHeader;
	VO_U32		iNumBitsMBBody;
	VO_S32			mode_intra;
	VO_S32			mode_inter;
#ifdef _DEBUG
	VO_S32			d_framenum; 
	VO_S32			skip_mb;
	VO_S32			mode_inter4v;
	VO_S32			mode_not;
	VO_S32			canmv_earlyexit;
	VO_S32			diamond_earlyexit;
#endif
} Statistics;


typedef struct
{
#ifdef LICENSE_CHECK
	VO_PTR phLicenseCheck;
#endif
	VO_U32		codec_id;
	VO_U32		config;
#ifdef NEW_SDK
	//TBD
	CUSTOM_CONFIG	custom_config;
	FRAME_INFO_CONFIG frame_info;
#endif
	VO_U32		img_width;
	VO_U32		img_height;
	VO_U32		edged_width;
	VO_U32		edged_height;
	VO_U32		mb_width;
	VO_U32		mb_height;
	VO_U32		img_mbsize;
	VO_U32		h263_pic_format; 
	VO_S32     framerate_incr;
	VO_U32		framerate_base;
	VO_U32		max_Iframe;
	VO_S32     force_iframe_num;
#ifdef VT
#define FORCE_INTRA_NUM	1
#else
#define FORCE_INTRA_NUM	5
#endif

	VO_VOID(*WritePicHeader)(ENCLOCAL * const enc_local, VO_U32 framerate_base,VO_U32	pic_format,
						VO_U32 img_width, VO_U32 img_height,BitStream * const bs,Statistics *sStat);
	VO_VOID(*EncInterMB)(ENCLOCAL * const , MACROBLOCK * , 
					 VO_S16 *, BitStream * , Statistics * ,Mpeg4Frame *, VO_S32 );
	VO_VOID(*EncIntraMB)(ENCLOCAL * const , MACROBLOCK * , 
					 VO_S16 *, BitStream * , Statistics * );
	VO_S32			resync_marker_bits;
	VO_S32			vp_byte_size;
	VO_S32			mb_num_len;
	VO_S16			*pred_acdc_row;
	VO_S16			*pred_acdc_col;
	Mpeg4Frame			*img_cur;
	Mpeg4Frame			*img_ref;
	VO_U8			*work_space;
	RC_CREATE		*rc_control;
	ENCLOCAL		enc_local;
	MACROBLOCK		*mbs;
//	VO_S32			*mb_length;
	Statistics		sStat;
	
	VO_MEM_OPERATOR	*memOperater;
#ifdef CAL_PSNR
	Mpeg4Frame			img_ori;
#endif
} ENCHND;


extern __inline VO_U32 GeDcScaler(VO_U32 quant, VO_U32 lum)
{
	if (quant < 5)
		return 8;
	
	if (quant < 25 && !lum)
		return (quant + 13) / 2;
	
	if (quant < 9)
		return 2 * quant;
	
	if (quant < 25)
		return quant + 8;
	
	if (lum)
		return 2 * quant - 16;
	else
		return quant - 6;
}

static VO_U32 __inline log2bin(VO_U32 value)
{
	VO_S32 n = 0;
	
	while (value) {
		value >>= 1;
		n++;
	}
	return n;
}

/* useful macros */

#endif
