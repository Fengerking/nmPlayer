/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2007		            *
*								 	                                    *
************************************************************************/

#ifndef __MPEG_DECODE_H
#define __MPEG_DECODE_H
#include "voMpeg2DID.h"
#include "voMpegPort.h"
#include "voMPEG2.h"
#include "voCheck.h"
#include "irdetoCallbacks.h"

#include "voMpegMC.h"


#ifdef IPP_EDIT
#include "ippi.h"
#include "ippvc.h"
#endif
// maximun picture size in macroblocks (16x16)
// example MB_X2=6 -> 2^6=64 macroblocks -> 1024 pixels

#ifdef __cplusplus
extern "C" 
{
#endif

//#define MPEG2_DEC
#ifdef MPEG2_DEC
#define IsFramePicture(dec)		1 
#define IsMpeg1(dec)			0
#else
#define IsFramePicture(dec)		(dec->picture_structure == PICT_FRAME)
#define IsMpeg1(dec)			(dec->codec_id == MPEG1_D)
#endif

#ifdef _DEBUG
//#define REF_DCT
#define FF_DUMP 1
#endif
#if FF_DUMP
#include <stdio.h>
#include <stdarg.h>
#define FF_DUMP_FRAME				(1<< 0)
#define BEFORE_MARKER				(1<< 1)
#define AFTER_MARKER				(1<< 2)
#define FF_DUMP_MV				(1<< 3)
#define FF_DUMP_FIELDS				(1<< 4)
#define FF_DUMP_ALL				(1<< 5)//0xffffffff


extern int ff_debug; 
extern FILE* fLog;

#ifndef DPRINTF
#define DPRINTF please_use_av_log
#endif

static __inline VO_VOID DPRINTF(int level, char *fmt, ...)
{
	if (ff_debug & level) {
		va_list args;
		char buf[1024];
		va_start(args, fmt);
		vsprintf(buf, fmt, args);
		va_end(args);
		//fprintf(fLog, "%s", buf);
		//vfprintf(fLog,buf,args);
		fputs(buf,fLog);
		fflush(fLog);
	}
}
#else
// #define DPRINTF(a,b,c)
// #define DPRINTF(a,b,c,d)
// #define DPRINTF(a,b,c,d,e)
// #define DPRINTF(a,b,c,d,e,f)
// #define DPRINTF(a,b,c,d,e,f,g)
#ifdef _WIN32
#define DPRINTF(fmt,...)
#else
#define DPRINTF(fmt,arg...)
#endif

#endif//FF_DUMP
#define FIFO_WRITE	0
#define FIFO_READ	1

#define MAXFRAMES   4
#define FIFO_NUM	(MAXFRAMES+1)

//#define MB_X2	6
//#define MB_Y2	6
#define MB_X2	7
#define MB_Y2	7
#define MB_X	(1<<MB_X2)
#define MB_Y	(1<<MB_Y2)

#define SEQ_END_CODE			0xB7
#define SEQ_START_CODE			0xB3
#define GOP_START_CODE			0xB8
#define PICTURE_START_CODE		0x00
#define SLICE_MIN_START_CODE	0x01
#define SLICE_MAX_START_CODE	0xAF
#define EXT_START_CODE			0xB5
#define USER_START_CODE			0xB2

#define I_VOP	1
#define P_VOP	2
#define B_VOP	3

#define POSX(pos) ((pos) & (MB_X-1))
#define POSY(pos) ((pos) >> MB_X2)

/*MB type*/
#define MB_QUANT	0x01
#define MB_FOR		0x02
#define MB_BACK		0x04
#define MB_PAT		0x08
#define MB_INTRA	0x10


#define PICT_TOP_FIELD     1
#define PICT_BOTTOM_FIELD  2
#define PICT_FRAME         3

/* motion type (for mpeg2) */
#define MT_FIELD 1
#define MT_FRAME 2
#define MT_16X8  2
#define MT_DMV   3


#define MVX(a)		(((a)<<16)>>16)
#define MVY(a)		((a)>>16)
#define MAKEMV(x,y)	(((y)<<16)|((x)&0xFFFF))


#define MV_X(v, dec) ((v<<16)>>(17+dec->ds_dec))
#define MV_Y(v) (v>>17)
#define MV_SUB(v, dec) ((v>>dec->ds_dec)&1)+((v>>15)&2)

#define MVDS_SUB(v, dec) ((v)&3)+((v>>14)&4)
#define DOWNSAMPLE_FLAG(dec)		dec->ds_dec

#ifdef BIG_ENDIAN
#define MVXIDX	1
#define MVYIDX	0
#else
#define MVXIDX	0
#define MVYIDX	1
#endif

extern VO_HANDLE g_hMPEG2DecInst;

typedef struct
{
	VO_U8 *y;
	VO_U8 *u;
	VO_U8 *v;
}
Image;

typedef struct
{
	VO_U8 *cur[3];
	VO_U8 *ref[3];
	VO_U8 *frame_dst[3];
	VO_S32* MVBack;
	VO_S32* MVFwd;
}
Mp2DecLocal;

typedef struct {             /*huwei 20090708 buffer_manage*/
	Image *img_seq[FIFO_NUM];/* Circular buffer */
	VO_U32 w_idx;		     /* Index of write */
	VO_U32 r_idx;			 /* Index of read */
}FifoType;

typedef enum
{
        MPEG1_D						= 0,  /*!< MPEG1 */
        MPEG2_D						= 1,  /*!< MPEG2 */
		MPEGMAX_D                   = VO_MAX_ENUM_VALUE
}CodecIdDec;

#define CHROMA_FORMAT			1
#define LOW_DELAY				3	
#define TOP_FIELD_FIRST			4	
#define FRAME_PRED_FRAME_DCT	5
#define CON_MOTION_VEC			6
#define Q_SCALE_TYPE			7
#define INTRA_VLC_FORT			8
#define ALTER_SCAN				9
#define REPEAT_FIRST_FIELD		10
#define CHROMA_420				11
#define PROGRESSIVE_FRAME		12
#define FIRST_FIELD				13
#define VALID_SEQ               14 

#define SEQ_EXTE_MASK			0xFFFFFFF0
#define PIC_EXTE_MASK			0xFFFFC20F

#define GET_FPFD(mpeg2_flag)((mpeg2_flag)&(1<<FRAME_PRED_FRAME_DCT))	
#define GET_Q_SCALE_TYPE(mpeg2_flag)((mpeg2_flag)&(1<<Q_SCALE_TYPE))	
#define GET_INTRA_VLC_FORT(mpeg2_flag)((mpeg2_flag)&(1<<INTRA_VLC_FORT))
#define GET_ALTER_SCAN(mpeg2_flag)((mpeg2_flag)&(1<<ALTER_SCAN))
#define GET_VALID_SEQ(mpeg2_flag)((mpeg2_flag)&(1<<VALID_SEQ))
#define GET_CON_MOTION_VEC(mpeg2_flag)((mpeg2_flag)&(1<<CON_MOTION_VEC))

#define SET_ALTER_SCAN(mpeg2_flag)((mpeg2_flag)|=(1<<ALTER_SCAN))
#define CLR_ALTER_SCAN(mpeg2_flag)((mpeg2_flag)&=(~(1<<ALTER_SCAN)))	
#define SET_FIRST_FIELD(mpeg2_flag)((mpeg2_flag)|=(1<<FIRST_FIELD))
#define CLR_FIRST_FIELD(mpeg2_flag)((mpeg2_flag)&=(~(1<<FIRST_FIELD)))	
#define SET_VALID_SEQ(mpeg2_flag)((mpeg2_flag)|=(1<<VALID_SEQ))

typedef IRDETO_CALLBACK_API VO_VOID (postIDCTCallback)(VO_S32 gopTimecode, VO_S32 temporalReference,VO_S32 pictureCodingType,
								VO_S32 horMacroBlockNum,VO_S32 verMacroBlockNum, VO_S32 blockNumber, VO_S16 *coefs);
typedef postIDCTCallback * postIDCTCallbackPtr;

/*
mpeg2_flag
progressive_sequence 1
chroma_format 2
low_delay 1
top_field_first 1
frame_pred_frame_dct 1
concealment_motion_vectors 1
q_scale_type 1
intra_vlc_format 1
alternate_scan 1
repeat_first_field 1
chroma_420_type 1
progressive_frame 1
first_field 1
*/

//YU_TBD: remove it, keep with frame buffer
typedef struct USER_DATA
{
	VO_S64 time_stamp;
	VO_PTR				UserData;  /*!< The user data for later use.*/
}USER_DATA;


typedef struct MpegDecode
{
	//TBD
	CodecIdDec codec_id;

	VO_U32 cache_a;
	VO_U32 cache_b;
	VO_S32 nBitPos;
	const VO_U8 *pBitPtr;
	const VO_U8 *pBitEnd;

	int img_width;
	int img_height;
	int img_stride;
	int img_stride_uv;
	int max_img_width;         //huwei 20101227 stability
	int max_img_height;
	int max_out_img_width;
	int max_out_img_height;
	int max_img_stride;
	int max_img_stride_uv;
	int uv_interlace_flage;
	int mb_w;
	int mb_h;

	int get_buffer_flage;	 //huwei 20101228 stability

	int ds_dec;
	int ds_Bframe;
	int amd_malloc_flag;

	
	int decPictureWidth;
	int decPictureHeight;	

	int out_img_width;
	int out_img_height;

	Image	*current_frame;
	Image	*reference_frame;
    Image   *Bframe;

	Image   *pDisplayFrame;//huwei 20090918 checkLib

	Image* displayFramePre;
	Image* decReferenceFrames[MAXFRAMES];

	VO_MEM_VIDEO_OPERATOR memoryShare;
	VO_MEM_OPERATOR       memoryOperator;

	FifoType privFIFO;/*buffer manage*/

	int intra_dc_precision;
	VO_U32 mpeg2_flag;

	int mv_type;
#define MV_TYPE_16X16       0   ///< 1 vector for the whole mb 
#define MV_TYPE_8X8         1   ///< 4 vectors (h263, mpeg4 4MV) 
#define MV_TYPE_16X8        2   ///< 2 vectors, one per 16x8 block  
#define MV_TYPE_FIELD       3   ///< 2 vectors, one per field  
#define MV_TYPE_DMV         4   ///< 2 vectors, special mpeg2 Dual Prime Vectors 

	VO_S32 mpeg_f_code[2][2];		
	int field_select[2][2];
	int picture_structure;
	int interlaced_dct;

	VO_U32 frame_num;
	VO_S32 first_Iframe_flage;
	VO_S32 previous_prediction_type;

	int qscale;
	int skip;
	int last_dc[3];
	short pred_mv[8]; //[foward/back][field][x/y]
	VO_S32 fmv[4][1+6];
	VO_S32 bmv[2][1+6]; // last y[2], new y[4], new uv[2]
	VO_S32 xmb_pos;
	VO_S32 ymb_pos;
	VO_S32 aspect_ratio;
	VO_S32 frame_rate;
	VO_S32 progressive_sequence;
	VO_S32 cur_progressive_frame;
	VO_S32 pre_progressive_frame;
	VO_S32 displayFrameProgressive;

	VO_S32 dmvector[2];
	VO_S32 mvx, mvy;

	VO_S32 outputMode;

	VO_U32  group_time_code;
	VO_U32  temporal_reference; 

// 	VO_S64   timeStamp;
// 	VO_S64   timeStampPre;
// 	VO_S64   outTimeStamp;
	USER_DATA user_data;
	USER_DATA user_data_pre;
	USER_DATA out_user_data;

	VO_PTR   phCheck;
	int display_ptype;

#ifdef IPP_EDITt
	VO_S16* pBuff;
#endif

	VO_S16* blockptr;

	VO_PTR	m_hDll;
	VO_S16* block_buffer;
	VO_U32  callback_flage;
	postIDCTCallbackPtr fp_postIDCTCallback;

	VO_U8 *mc_dst[3];
	VO_U8 *numLeadingZeros;

#ifdef SKIP_IDENTICAL_COPY
	VO_U8 *frBCopyFlag;
	VO_U8 *frPCopyFlag;
	int		frPCopySize32;
	int		lastRefId;
	int		firstBFrameAfterIP;
#endif //SKIP_IDENTICAL_COPY

	VO_VOID (*fp_copy_block_luma)(VO_U8 *, VO_U8 *, const VO_S32, const VO_S32, VO_S32);
	VO_VOID (*fp_copy_block_chroma)(VO_U8 *, VO_U8 *, const VO_S32, const VO_S32, VO_S32);

	const COPYBLOCK_PTR *all_copy_block;
	const addblock *all_add_block;

	int prediction_type;
	int frame_state; // -1:no info 0:decoding 1:need startframe
#define FRAME_BEGIN		0
#define FRAME_DECODING	1
#define FRAME_READY		2

//huwei 20110611 add new id
#define FRAME_FINISH    3

	int full_pixel[2];//[foward/back]

	VO_U8 zigzag[64];
	VO_U8 DefaultIntraMatrix[64];
	VO_U8 IntraMatrix[64];
	VO_U8 DefaultInterMatrix[64];
	VO_U8 InterMatrix[64];
} MpegDecode;

int dec_frame( MpegDecode* dec, VO_U8* Ptr, int Len , int header, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat);
VO_U32 get_frame_type(MpegDecode *dec);

#ifdef __cplusplus
}
#endif

#endif
