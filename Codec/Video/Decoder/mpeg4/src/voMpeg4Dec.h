/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __MP4_DECODE_H
#define __MP4_DECODE_H

#include "voMpeg4DecGlobal.h"
#include "viMem.h"
#ifdef VODIVX3
#include "voDIVX3.h"
#define VO_ERR_DEC_MPEG4_HEADER		VO_ERR_DEC_DIVX3_HEADER
#define VO_ERR_DEC_MPEG4_I_FRAME	VO_ERR_DEC_DIVX3_I_FRAME 
#define VO_ERR_DEC_MPEG4_INTRA_MB	VO_ERR_DEC_DIVX3_INTRA_MB
#define VO_ERR_DEC_MPEG4_P_FRAME	VO_ERR_DEC_DIVX3_P_FRAME
#define	VO_ERR_DEC_MPEG4_B_FRAME	VO_ERR_DEC_DIVX3_B_FRAME
#else
#include "voMPEG4.h"
#endif


#ifdef ASP_GMC
#include "voMpeg4Gmc.h"
#endif //ASP_GMC

#include "voRVThread.h"

#if ENABLE_DEBLOCK
#define VO_BACK_MAX_ENUM_VALUE	0X7FFFFFFF
#if ENABLE_HWIMX31_DEBLOCK
#include "voiMX31pf.h"
#else//ENABLE_HWIMX31_DEBLOCK
typedef enum 
{
    VO_pfMode_Disabled,           /*!<No postfiltering*/
    VO_pfMode_MPEG4Deblock,       /*!< MPEG4 Deblock only*/
    VO_pfMode_MPEG4Dering,        /*!<MPEG4 Dering only*/
    VO_pfMode_MPEG4DeblockDering, /*!< MPEG4 Deblock and Dering*/
    VO_pfMode_H264Deblock,        /*!< H.264 Deblock*/
	MAX_BD_MOD                    = VO_BACK_MAX_ENUM_VALUE
} pVO_fMode;
#endif

typedef struct {
	VO_S8  mpeg4_thresh_tbl[512];
	VO_U8 mpeg4_abs_tbl[512];
} POSTPROC;

#endif //ENABLE_DEBLOCK



#define MB_X2	7
#define MB_Y2	7
#define MB_X	(1<<MB_X2)
#define MB_Y	(1<<MB_Y2)
#define BLOCK_SIZE 8

#define DC_LUM_MASK	((MB_X*2)*4-1)
#define DC_CHR_MASK (MB_X*2-1)
#define POSX(pos) ((pos) & (MB_X-1))
#define POSY(pos) ((pos) >> MB_X2)


typedef struct
{
	VO_U8 *y;
	VO_U8 *u;
	VO_U8 *v;
#if ENABLE_DEBLOCK
	VO_S32 *pQuant;
#if ENABLE_HWIMX31_DEBLOCK
	VO_U32 phyaddr;
#endif//ENABLE_HWIMX31_DEBLOCK
#endif
	VO_U32	nVOPType;
	VO_S64  nTime;
	VO_PTR	pUserData;
}
VO_IMGYUV;

////////////////////////////////////TBD DP
typedef VO_S32		 BLOCK_32BIT[6];

typedef struct{
	VO_S32			nLastMBNum;//the last mb number before marker
	VO_U8*			bNotCoded;
	VO_S16*			mcbpc;
	VO_U8*			quant;
	BLOCK_32BIT*	nDctDCDiff;
	VO_U8*			bACPred;
	VO_U8*			cbpy;
}DP_BUFFER;

#define MAXFRAMES 31

#define FIFO_NUM	(MAXFRAMES+1)
typedef struct {
  VO_IMGYUV *img_seq[FIFO_NUM];     /* Circular buffer */
  VO_U32 w_idx;				/* Index of write */
  VO_U32 r_idx;				/* Index of read */
}FIFOTYPE;

#define FRAMEDEFAULT	3
typedef struct 
{
	unsigned int nPictureWidth;					/*!< picture nWidth offered outside*/
	unsigned int nPictureHeight;				/*!< picture nHeight offered outside*/
	unsigned int nNumRefFrames;					/*!< number of reference and reconstruction  frames */
	unsigned int nPictureEXWidth;				/*!< including padding */
	unsigned int nPictureEXHeight;				/*!< including padding */
	unsigned int nPrivateMemSize;				/*!< size in byte */
	unsigned char *frameMem[MAXFRAMES][3];
	unsigned char *pPrivateMem;
}VO_VIDEO_INNER_MEM;


typedef struct {
	VO_CODEC_INIT_USERDATA *pMemOp;
	VO_MEM_VIDEO_OPERATOR *pFrameMemOp;	/* outside memory operator*/
	VO_U32	nCodecIdx;
}VO_VIDEO_MEM_SERVER;

typedef struct{	
	VO_U32 nCodecIdx;
	VO_VIDEO_INNER_MEM vInnerMem;
	VO_CODEC_INIT_USERDATA * pUserData;
	VO_CODECBUFFER  vInBuf;
	VO_U32			nUsedLen;
#ifdef VODIVX3
	VO_U32			nWidth;
	VO_U32			nHeight;
#endif
#if ENABLE_LICENSE_CHECK
	VO_PTR phLicenseCheck;
	VO_VIDEO_BUFFER *pOutVideoBuf;
#endif
	void *pBackDec;
}VO_MPEG4DEC_FRONT;

// extern VO_U32 pDec->nMpeg4Thd;
// extern VO_S32 pDec->nMpeg4ThdMinMB;
// extern VO_S32 pDec->nMpeg4ThdMaxMB;

struct VO_MPEG4_DEC;
struct VO_MPEG4_DEC_LOCAL;
typedef void (*FuncReconMB)(struct VO_MPEG4_DEC*, struct VO_MPEG4_DEC_LOCAL*, VO_S32);

typedef struct VO_MPEG4_DEC_LOCAL
{
	VO_U8* mc_data;

	VO_S32 gmcmb;
	VO_S32 bFieldDct;
	VO_S8 *scanType;	//6 every MB
	VO_S16* blockptr;	//6*64 every MB
	VO_S8* blockLen;	//6 every MB
	VO_S32* MVBack;		//6 every MB
	VO_S32* MVFwd;		//6 every MB
	VO_S32 *pMBQuant;

	VO_U32 voERR;
	FuncReconMB fpReconMB_i;
	VO_S32 nMBPos;
	VO_S32 fprev0,fprev1;
	VO_S32 bprev0,bprev1;

}
VO_MPEG4_DEC_LOCAL;

typedef struct
{
	VO_S32 nMBCount;
	VO_S8 *bFieldDct;
	VO_S8 *gmcmb;
	VO_S32 *MVBack;
	VO_S32 *MVFwd;
	FuncReconMB *fpReconMB;
	VO_S8 *scanType;
	VO_S8 *blockLen;
	VO_S16 *blockptr;
	VO_S32 *pMBPos;
	VO_U8 *mc_data;
// 	VO_S8 bFieldDct[MAX_MB_TASK_NUM];
// 	VO_S8 gmcmb[MAX_MB_TASK_NUM];
// 	VO_S32 MVBack[MAX_MB_TASK_NUM*6];
// 	VO_S32 MVFwd[MAX_MB_TASK_NUM*6];
// 	FuncReconMB fpReconMB[MAX_MB_TASK_NUM];
// 	VO_S8 scanType[MAX_MB_TASK_NUM*6];
// 	VO_S8 blockLen[MAX_MB_TASK_NUM*6];
// 	VO_S16 blockptr[MAX_MB_TASK_NUM*6*64];
// 	VO_S32 pMBPos[MAX_MB_TASK_NUM];
// 	VO_U8 mc_data[BLOCK_SIZE*BLOCK_SIZE];
}VO_MPEG4_VOP_CONTEXT;

struct _MBRow_t;
typedef struct VO_MPEG4_DEC 
{
	VO_HEAD_INFO *pHeadInfo;	/*PC version debug*/
	VO_VIDEO_MEM_SERVER vMemSever;
	VO_U32				nMemConfig;/* default zero, max memory size*/
	VO_U8				*pMemBase;
	VO_IMGYUV			frameBuffer[MAXFRAMES];
	FIFOTYPE			vFrameBufFIFO;
	VO_U32				nFrameBufNum;
	VO_IMGYUV			*pCurFrame;
	VO_IMGYUV			*pRefFrame;
	VO_IMGYUV			*pOutFrame;

	//for ReconMB use
	VO_IMGYUV			*pCurFrameMB;
	VO_IMGYUV			*pRefFrameMB;
	VO_IMGYUV			*pDstFrameMB;

	/*global setting*/
	VO_S32 nCodecId;
	VO_S32 nWidth;
	VO_S32 nHeight;
	VO_S32 nLumEXWidth;
	VO_S32 nLumEXHeight;
	VO_S32 nChrEXWidth;
	VO_S32 nChrEXHeight;
	VO_S32 nMBWidth;
	VO_S32 nMBHeight;
	VO_S32 nMBBits; //log2(nMBWidth*nMBHeight)
	VO_S32 nPosEnd;
	VO_S32 nTrueWidth;
	VO_S32 nTrueHeight;
	VO_S32 nAspectRadio;
	
	/*bitstream */
	VO_U32 cache_a;
	VO_U32 cache_b;
	VO_S32 nBitPos;
	const VO_U8 *pBitPtr;
	const VO_U8 *pBitEnd;
	VO_S64 nTime;
	VO_PTR	pUserData;

	VO_S32 nOutMode;	/* 0-display order 1-encoded order*/
	VO_S32 nResyncPos;
	VO_S32 nMBError;
	VO_S32 nFrame;
	VO_S32 nLastRefFrame;	// frame number of last refframe
	VO_S32 nMapOfs;			// nMapOfs + (frameMap[pos] >> 1) is the last time that block was updated
	VO_S32 nCurFrameMap;	// (frame - nMapOfs) << 1

	VO_U32 (*fpGetHdr)( struct VO_MPEG4_DEC* , const VO_S32);
	VO_U32 (*fpDecIVOP)( struct VO_MPEG4_DEC* );
	VO_U32 (*fpDecPVOP)( struct VO_MPEG4_DEC* );
	VO_U32 (*fpDecBVOP)( struct VO_MPEG4_DEC* );

	// vol
	VO_S32 nTimeIncResolution;
	VO_S32 nTimeIncBits;
	VO_S32 nQuantPrecision;
	VO_S32 nQuantType;
	VO_S32 nSprite;
	VO_S32 nSpriteWarpPoints;
	VO_S32 nSpriteWarpgmc_accuracy;
	VO_S32 nSpriteBrightnessChange;
	VO_S32 nFlipRounding;
	VO_S32 qpel;
	VO_U32 nFrameError;

	VO_S8 bLowDelay;
	VO_S8 validvol;
	VO_S8 bInterlaced;
	VO_S8 alter_ver_scan;
	VO_S8 long_vectors;
	VO_S8 aic;
	VO_S8 loop_filter;
	VO_S8 bSliceStructuredH263;
	VO_S8 h263_alt_inter_vlc;
	VO_S8 h263_modified_quant;
	VO_S8 align00;
	VO_S8 align01;
	VO_S32 rounding;
	VO_U8 *chroma_qscale_table;  ///< qscale -> chroma_qscale (h263)
	
	// vop
	VO_S32 nVOPType;
	VO_S32 nQuant;
	VO_S32 nFcode;
	VO_S32 nBcode;
#define RESYNC_MARK_BITS 17
	VO_S32 nResyncMarkerBits;
	VO_S32 nIntraDCThreshold;

	VO_S32 dropable;// S263
	
	// macroblock
	VO_S32 nVopCtxtSize;
	VO_MPEG4_VOP_CONTEXT vopCtxt;
// 	FuncReconMB* fpReconMB;
// 	VO_S32 *pMBPos;
// 	VO_S8 *gmcmb;
// 	VO_S8 *bFieldDct;
// 	VO_S8 *scanType;
// 	VO_S16* blockptr;
// 	VO_S8* blockLen;
// 	VO_U8* mc_data;
	VO_S32 bACPred;
	VO_S32 nCBP;
	VO_S32 (*vld_block)( struct VO_MPEG4_DEC* pDec, const VO_U16 *table, const VO_U8 *scan, VO_S32 len, idct_t* block, VO_S32 mb_type); 
//TBD
	// motion compensation buffer
	VO_S32 mv_bufmask;
	VO_S32* mv_buf; //32bytes aligned
// 	VO_S32* MVBack;
// 	VO_S32* MVFwd;
 	VO_S8* bFieldPred_buf;

	// b-frame
	VO_S32 time_pp;
	VO_S32 time_bp;
	VO_S32 last_reftime;

	VO_S64 TRB;
	VO_S64 TRB_TRD;

//	VO_S64 time_cur;

	VO_S32 predict_dir;

	VO_U8 quant[2][64]; // first intra, second inter
	DP_BUFFER* DP_Buffer;

#ifdef VODIVX3
	// msmpeg4
	VO_S32 use_coded_bit;
	VO_S32 dc_lum_scaler;
	VO_S32 dc_chr_scaler;
	VO_S32 slice_pos;
	VO_S32 slice_height;
	VO_S32 ByteRate;
	VO_U8* rlmax;

	// vlc pointer
	VO_U16* dc_chr_vlc;
	VO_U16* dc_lum_vlc;
	VO_U16* mv_vlc;
	VO_U16* rl_vlc[2];
	VO_U8* rlmax_vlc[2];

	// vlc tables
	VO_U16* mb_inter_vlc;
	VO_U16* mb_intra_vlc;
	VO_U16* dc_chr_table[2];
	VO_U16* dc_lum_table[2];
	VO_U16* mv_table[2];
	VO_U16* rl_table[6];
#endif

	VO_S16 dc_lum[4*MB_X*2];		//[4][double nWidth row]
	VO_S16 ac_left_lum[2][8];		//[lower/upper][8]
	VO_S16 ac_top_lum[2*MB_X][8]; //[double witdh row][8]

	VO_S16 dc_chr[2][2*MB_X];		//[U/V][2][normal nWidth row]
	VO_S16 ac_left_chr[2][8];		//[U/V][8]
	VO_S16 ac_top_chr[MB_X][2][8];//[normal nWidth row][U/V][8]
	VO_S32 quantrow[MB_X];
	VO_U8 frameMap[MB_X*MB_Y]; // when the last time the block changed (and resuce needed flag)
#ifdef VODIVX3	
	VO_S8 codedmap[MB_X*MB_Y]; // only for msmpeg4 (must be at the end!)
#endif
#ifdef ASP_GMC
	VO_S32 warp_point[3];
	VO_VIDEO_GMC_DATA vGmcData;
#endif
	//YU TBD
#if ENABLE_DEBLOCK
	VO_S32 pf_flag; //0-disable, 1-software, 2-hardware
	VO_S32 pf_mode;
	POSTPROC postproc;
#if ENABLE_HWIMX31_DEBLOCK
	void *pf_handle;
	VO_U32 qp_phyaddr;
#endif//ENABLE_HWIMX31_DEBLOCK
#endif//ENABLE_DEBLOCK

	//for multi-thread
	struct _MBRow_t *MBr;
	pthread_mutex_t parserMutex;
	pthread_mutex_t readyMutex;
	pthread_mutex_t finishMutex;

	pthread_cond_t parserCond;
	pthread_cond_t readyCond;
	pthread_cond_t finishCond;

	volatile VO_U32 nParserErr;
	volatile VO_S32 nParserMBPos;
	volatile VO_S32 nWaiter;
	volatile VO_S32 nParser;
	volatile VO_S32 bEndVop;
	volatile VO_S32 nFinish;
	VO_S32 *volatile pParserMBQuant;
	volatile VO_S32 vfprev0;
	volatile VO_S32 vfprev1;
	volatile VO_S32 vbprev0;
	volatile VO_S32 vbprev1;

	VO_U32 nMpeg4Thd;
	VO_S32 nMpeg4ThdMinMB;
	VO_S32 nMpeg4ThdMaxMB;

} VO_MPEG4_DEC;


typedef struct _MBRow_t
{
	pthread_t thread_id;
	volatile VO_S32 runCMD;		//run or stop, controlled by parser thread 
	volatile VO_S32 bRunning;	//Is it running?
	VO_U32 (* volatile fpVOP)(struct _MBRow_t *);
	VO_MPEG4_DEC *pDec;
	VO_MPEG4_VOP_CONTEXT *vopCtxt;
// 	VO_S32 nMBPos;		//show what MB in processing, nMBPos=-1 shows it's not active yet
// 	pthread_cond_t * pCond;
// 	VO_S32 bLastRow;	//Is it the last row thread which start when parser finish?
// 	VO_U32 (*fpGetCond)(struct _MBRow_t *, volatile FuncReconMB **);
}MBRow_t;

#endif
