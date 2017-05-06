
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/

/*!
************************************************************************
*
* \file		avd_neighbor.h
*
* \brief
*		defines store neighboring infomation encoding/decoding
*
************************************************************************
*/
#ifndef _AVD_NEIGHBOR_H_
#define _AVD_NEIGHBOR_H_

#if  defined(USE_JOBPOOL)
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
#include <pthread.h>
#endif
#include "threadAPI.h"
#endif
//#include "H264_C_Type.h"
//////////////////////////////////////////////////////////////////////////
//				 neighbor 
//////////////////////////////////////////////////////////////////////////
// index to bit location of cpb and run/level array;
typedef enum
{
	COEFF_LUMA_INTRA16x16DC_IDX		= 0,
	COEFF_LUMA_START_IDX			= 1,
	COEFF_LUMA_END_IDX				= 16,
	COEFF_CHROMA_U_DC_IDX			= 17,
	COEFF_CHROMA_V_DC_IDX			= 18,
	COEFF_CHROMA_U_AC_START_IDX		= 19,
	COEFF_CHROMA_U_AC_END_IDX		= 22,
	COEFF_CHROMA_V_AC_START_IDX		= 23,
	COEFF_CHROMA_V_AC_END_IDX		= 26,
	NUMBER_COEFF_BLOCK				= 27,
	NUMBER_RUN_SIZE					= 28, // make it aligned; 
	INVALID_COEFF_BLOCK_IDX			= NUMBER_COEFF_BLOCK + 1,
	COEFF_BLOCK_TYPE_IDX_END2           = 0x7fffffff,

} COEFF_BLOCK_TYPE_IDX;

enum {
	NEIGHBOR_A					= 0,
	NEIGHBOR_B					= 1,
	NEIGHBOR_C					= 2,
	NEIGHBOR_D					= 3,

	NEIGHBORHOOD_SIZE			= 4
} ;
#define AVC_TRUE 1
#define AVC_FALSE 0
#define		LUMACOEFFICIENTMASK		0x1ffff
#define		UCOEFFICIENTMASK		0x790000
#define		UACCOEFFICIENTMASK		0x780000
#define		VCOEFFICIENTMASK		0x7840000
#define		VACCOEFFICIENTMASK		0x7800000
#define     ALLCBPNONZERO			0x2f


#define niGetCavlcNeighborStruct(mbInfo)	(&mbInfo->uNB.cavlcNeighbor)
#define niGetCabacNeighborStruct(mbInfo)	(&mbInfo->uNB.cabacNeighbor)

#define niSetLumaOrIntra16x16LumaACAllCBPBitsNonZero(cbp)	(cbp |= 0x1fffe) // bits 1-16;
#define niIsMBAvailable(info, row, col) (info->mbAvail[row][(col)>>5]&(((avdUInt32)1)<<((col)&31)))
#define niSetMBAvailable(info, row, col) (info->mbAvail[row][(col)>>5] |= (((avdUInt32)1)<<((col)&31)))
#define niIsNeighborBAvailable(info) (info->nbBoundary4x4[NEIGHBOR_B][0]->neighborMB != NULL)
#define niIsNeighborAAvailable(info) (info->nbBoundary4x4[NEIGHBOR_A][0]->neighborMB != NULL)
#define niGetCavlcNumNonZerosLuma(mbInfo, i) ((niGetCavlcNeighborStruct(mbInfo)->numNonZerosLuma>>(((i)<<2)+i))&0x1f)
#define niSetCavlcNumNonZerosLuma(mbInfo, val, i) (niGetCavlcNeighborStruct(mbInfo)->numNonZerosLuma | (val<<(((i)<<2)+i)))
#define niGetCavlcNumNonZerosLumaLeft(mbInfo, i) ((niGetCavlcNeighborStruct(mbInfo)->numNonZerosLumaLeft>>(((i)<<2)+i))&0x1f)
#define niSetCavlcNumNonZerosLumaLeft(mbInfo, val, i) (niGetCavlcNeighborStruct(mbInfo)->numNonZerosLumaLeft | (val<<(((i)<<2)+i)))
#define niGetCabacB8PredDir(mbInfo, i) //((niGetCabacNeighborStruct(mbInfo)->b8Pdir>>((i)<<1))&3)
#define niSetCabacAllB8PredDir(mbInfo, m0, m1, m2, m3) //(niGetCabacNeighborStruct(mbInfo)->b8Pdir = (m0 | (m1<<2) | (m2<<4) | (m3<<6)))
#define niGetCabacSIBlock(mbInfo) (niGetCabacNeighborStruct(mbInfo)->siBlock);
#define niSetCabacSIBlock(mbInfo, val) (niGetCabacNeighborStruct(mbInfo)->siBlock = val);
#define niGetCabacCBP(mbInfo) (niGetCabacNeighborStruct(mbInfo)->flag1&0x0000003f)
#define niSetCabacCBP(mbInfo, val) (niGetCabacNeighborStruct(mbInfo)->flag1=(niGetCabacNeighborStruct(mbInfo)->flag1&0xffffffc0)| (val))
#define niGetCabacNonZeroIntraChromaMode(mbInfo) (niGetCabacNeighborStruct(mbInfo)->cbpBits&0x80000000)
#define niSetCabacNonZeroIntraChromaMode(mbInfo, val) (niGetCabacNeighborStruct(mbInfo)->cbpBits |= (val)<<31)
#define niResetCabacDirectBiDir(mbInfo) (niGetCabacNeighborStruct(mbInfo)->flag1&=0xfffffc3f)
#define niSetCabacDirectBiDir(mbInfo,i,val)  (niGetCabacNeighborStruct(mbInfo)->flag1|=((val)<<((i)+6)))
#define niGetCabacDirectBiDir(mbInfo,i) ((niGetCabacNeighborStruct(mbInfo)->flag1&0x000003c0)&(1<<((i)+6)))

typedef struct _avcCabacNeighbor {
	// TODO: change to MOTION_VECTOR;
	avdInt8			mvd[2][4][2]; //[list0/1][4x4boundary][X/Y];
	avdInt8			mvdLeft[2][4][2]; //[list0/1][4x4boundary][X/Y];
	// 27 bits to indicate if block coeffs are zero as defined in COEFF_BLOCK_TYPE_IDX;
	// it is computed by scanning real coeffs of blocks; this is allowed and handled
	// in readRunLevelArray_CABAC;
	//31:nonZeroIntraChromaMode; 
	avdUInt32				cbpBits; 
	// TBD: reorg the flags, only 10 bits needed;
	//avdUInt8				b8Pdir; // 2 bit for each 8x8 blocks;
	//0-5: cbp
	//6-9: directBiDir
	avdUInt32				flag1;
	//avdUInt8				siBlock; //: 1;
	//avdUInt8				nonZeroIntraChromaMode; // : 1; 
	// defined as in spec; this is used in both CAVLC and CABAC;
	// CABAC is supposed to be able to derive form cbpBits, but some
	// encoder mark cbp even there is no non-zero coeff in the blocks;
	//avdUInt8				cbp; // using (: 6) some problem in C55x; // defined as in spec;
} AVCCabacNeighbor;

#define niResetCavlcNiFlag2(mbInfo) {\
	AVCCavlcNeighbor* vlc = &(mbInfo)->uNB.cavlcNeighbor;\
	vlc->niFlag2 = 0;\
	vlc->numNonZerosLuma = 0;\
	vlc->numNonZerosLumaLeft = 0;}
	
#define niGetCavlcNumNonZerosChroma(mbInfo, i) ((niGetCavlcNeighborStruct(mbInfo)->niFlag2>>(((i)<<2)+16))&0xf)
#define niSetCavlcNumNonZerosChroma(mbInfo, v3, v2, u3, u2) \
			(niGetCavlcNeighborStruct(mbInfo)->niFlag2 |= (((v3)<<28) | ((v2)<<24) | ((u3)<<20) | ((u2)<<16)))

#define niGetCavlcNumNonZerosChromaLeft(mbInfo, i) ((niGetCavlcNeighborStruct(mbInfo)->niFlag2>>(((i)<<2)))&0xf)
#define niSetCavlcNumNonZerosChromaLeft(mbInfo, v3, v2, u3, u2) \
	(niGetCavlcNeighborStruct(mbInfo)->niFlag2 |= (((v3)<<12) | ((v2)<<8) | ((u3)<<4) | ((u2))))
#if 1
#define niSetCavlcSkippedOrLumaChromaAllACZeros(mbInfo, isSkip, cbp) \
			(niGetCavlcNeighborStruct(mbInfo)->numNonZerosLuma |= isSkip ? 0x1f000000 : ((((~(cbp))&0xf) | ((((cbp)>>4) != 2)<<4)))<<24)
#define niIsCavlcSkippedOrLumaAllACZeros(mbInfo, i) (niGetCavlcNeighborStruct(mbInfo)->numNonZerosLuma&(1<<((i)+24)))
#define niIsCavlcSkippedOrChromaAllACZeros(mbInfo) (niGetCavlcNeighborStruct(mbInfo)->numNonZerosLuma&0x10000000)
#else
#define niSetCavlcSkippedOrLumaChromaAllACZeros(mbInfo, isSkip, cbp) \
	(niGetCavlcNeighborStruct(mbInfo)->niFlag2 |= isSkip ? 0x1f : (((~cbp)&0xf) | (((cbp>>4) != 2)<<4)))
#define niIsCavlcSkippedOrLumaAllACZeros(mbInfo, i) (niGetCavlcNeighborStruct(mbInfo)->niFlag2&(1<<(i)))
#define niIsCavlcSkippedOrChromaAllACZeros(mbInfo) (niGetCavlcNeighborStruct(mbInfo)->niFlag2&0x10)
#endif
//#define niAssignCavlcSkipped(mbInfo)	niGetCavlcNeighborStruct(mbInfo)->numNonZerosLuma |= 0xf8000000; 
										


typedef struct _niFlag2DefinitionForReference {
	avdNativeUInt		numNonZerosChroma:16; // 16bits, 4bits*4(in order of V3V2U3U2) (>=0, <=15, dc always not counted) for each 4x4(U/V) on MB boundary;
	avdNativeUInt		numNonZerosChromaLeft:16;
	//avdNativeUInt		unUsed:11; 
	//avdNativeUInt		skippedOrChromaAllACZeros:1; 
	//avdNativeUInt		skippedOrLumaAllACZeros:4; // four bits for 4 8x8 blocks;
} niFlag2DefinitionForReference;

typedef struct _avcCavlcNeighbor {
	avdUInt32				numNonZerosLuma; // skippedOrChromaAllACZeros(31),skippedOrLumaAllACZeros:(30-27): only 5 bits(>=0, <=16) used for each 4x4 on MB boundary;
	avdUInt32				numNonZerosLumaLeft; 
	avdUInt32				niFlag2;
} AVCCavlcNeighbor;


#define niSetMbAddress(mbInfo, val)		//((mbInfo)->niFlag1 = (((mbInfo)->niFlag1&0xffffc000) | val))
#define niGetMbAddress(mbInfo)			//((mbInfo)->niFlag1&0x3fff)
//#define niMarkFrameMb(mbInfo)			 mbUnMarkMbField(mbInfo)
//#define niUnMarkFrameMb(mbInfo)			mbMarkMbField(mbInfo)
//#define niIsFrameMb(mbInfo)				!mbIsMbField(mbInfo)


typedef struct _niFlag1DefinitionForReference {
	avdNativeUInt		intra4x4Mode:16; // 4 bits (0 to 16) for each 4x4;
	avdNativeUInt		intra4x4ModeLeft:16; // 4 bits (0 to 16) for each 4x4;
	//avdNativeUInt		unUsed:1;		
	//avdNativeUInt		frameMb:1; 
	//avdNativeUInt		mbAddress:14;  // 8192 for max size in H.264; 
} niFlag1DefinitionForReference;

// neighbor MB info
// Idx of 4x4 block on boundary will be
//			XXXX		XXX0             U   V      U   V
//	Luma:	XXXX	or	XXX1    Chroma:  XX  XX  or X0  X2
//			XXXX		XXX2			 01  23     X1  X3
//			0123		XXX3
#ifdef X86_TEST
#define DEBUG_POS 1
#endif//
typedef struct _MacroBlock { 
	// Note: different types of data saved depending on CAVLC/CABAC or deblocking filtering; 
	// numNonZerosLuma(Chroma) & AVCoder->nonZeroLuma, checking AC & DC, except AC only for Intra16x16 and chroma4x4, 
	// is used in CAVLC; cbp is defined as in spec;
	// cbpBits, seperating AC & DC by COEFF_BLOCK_TYPE_IDX (Luma4x4 in SubBlockScan order), is used for CABAC;
	avdUInt32						mbFlag1;
	avdUInt32						mbFlag2;//clone mbFlag1 
	
	// move yMB4x4 from AVCNeighbor4x4Boundary to here to speed up the performance;
	avdUInt32						niFlag1;
	
#if FEATURE_CABAC
	//avdUInt16						mbB8Mode;
	union {
		AVCCabacNeighbor			cabacNeighbor;
		AVCCavlcNeighbor			cavlcNeighbor;
	} uNB;
#else//FEATURE_CABAC
	union {
		AVCCavlcNeighbor			cavlcNeighbor;
	} uNB;
#endif//FEATURE_CABAC//TBD: optimize for baseline
	struct _TMBBitStream			mbBitStream;
	avdInt16						rowCof[384];//YUV
#ifdef DEBUG_POS//for debug
	int		mb_x;
	int	    mb_y;
	int		isField;
#endif//
} MacroBlock;

#if 0
#define niSetAllIntra4x4Mode(mbInfo, m0, m1, m2, m3) (mbInfo->niFlag1 |= ((m0<<16) | (m1<<20) | (m2<<24) | (m3<<28)))
#define niSetAllIntra4x4ModeDCPred(mbInfo)	(mbInfo->niFlag1 |= ((DC_PRED<<16) | (DC_PRED<<20) | (DC_PRED<<24) | (DC_PRED<<28)))
#define niSetIntra4x4Mode(mbInfo, i, mode)	(mbInfo->niFlag1 |= ((mode)<<((i+4)<<2)))
#define niGetIntra4x4Mode(mbInfo, i)		((mbInfo->niFlag1>>((i+4)<<2))&0xf)
#define niClearIntra4x4Mode(mbInfo)			(mbInfo->niFlag1 &= 0xffff)
#else
#define niSetAllIntra4x4Mode(mbInfo, m0, m1, m2, m3) (mbInfo->niFlag1 |= ((m0<<16) | (m1<<20) | (m2<<24) | (m3<<28)))
#define niSetAllIntra4x4ModeDCPred(mbInfo)	(mbInfo->niFlag1 |= ((DC_PRED<<16) | (DC_PRED<<20) | (DC_PRED<<24) | (DC_PRED<<28)))
#define niSetIntra4x4Mode(mbInfo, i, mode)	(mbInfo->niFlag1 |= ((mode)<<((i+4)<<2)))
#define niGetIntra4x4Mode(mbInfo, i)		((mbInfo->niFlag1>>((i+4)<<2))&0xf)

#define niSetAllIntra4x4ModeLeft(mbInfo, m0, m1, m2, m3) (mbInfo->niFlag1 |= ((m0) | (m1<<4) | (m2<<8) | (m3<<12)))
#define niSetAllIntra4x4ModeDCPredLeft(mbInfo)	(mbInfo->niFlag1 |= ((DC_PRED) | (DC_PRED<<4) | (DC_PRED<<8) | (DC_PRED<<12)))
#define niSetIntra4x4ModeLeft(mbInfo, i, mode)	(mbInfo->niFlag1 |= ((mode)<<((i)<<2)))
#define niGetIntra4x4ModeLeft(mbInfo, i)		((mbInfo->niFlag1>>((i)<<2))&0xf)
#define niClearIntra4x4Mode(mbInfo)			(mbInfo->niFlag1 = 0)
#endif

#define niSet4x4MVD(info, j, i, compIdx, val) (info->mvd4x4[j][i][compIdx] = val)
#define niGet4x4MVD(info, j, i, compIdx) (info->mvd4x4[j][i][compIdx])
#define niSet4x4RefIdx(info, j, i, ref) (info->refIdx4x4[j] |= (ref<<((i)<<2)))
#define niGet4x4RefIdx(info, j, i) ((info->refIdx4x4[j]>>((i)<<2)) & 0xf)
#define niReset4x4RefIdx(info) (info->refIdx4x4[0] = info->refIdx4x4[1] = info->refIdx4x4[2] = info->refIdx4x4[3] = 0) //(memset(&info->refIdx4x4[0], 0, 4 * sizeof(avdUInt16)))
#define niIs4x4Predicted(info, list, j, i) (((info->is4x4Predicted[list]>>(((j)<<2)+i))&1) == 1)
#define niSet4x4Predicted(info, list, j, i) (info->is4x4Predicted[list] |= (1<<(((j)<<2)+i)))
#define niReset4x4Predicted(info) (info->is4x4Predicted[0] = info->is4x4Predicted[1] = 0) //(memset(&info->is4x4Predicted[0], 0, 2 * sizeof(avdUInt16)))
#define niIs4x4BDirect(info, j, i) (((info->is4x4BDirect>>(((j)<<2)+i))&1) == 1)
#define niSet4x4BDirect(info, j, i) (info->is4x4BDirect |= (1<<(((j)<<2)+i)))
#define niSet4x4BDirect_k(info, k) (info->is4x4BDirect |= (1<<k))
#define niReset4x4BDirect(info) (info->is4x4BDirect = 0)
#define niINVALID_MB_ADDRESS			-1
#define niIsVaildMBAddress(addr) (addr != niINVALID_MB_ADDRESS)
#define AVD_INVALID_INTRAMODE				0xff

#define GetMB2(info,mb_x) ((info)->anotherProcessor->neighborMBs+(mb_x))
#define niSetTopCbpBlkFromBottomOfFullCbpBlk(info, cbpBlk, mb_x) 
#define niSetQPnIntraFlags(info, mb_x, qp, isIntra4x4, isIntra)
#define niIsTopCbpBlkNonZero2(info, mb_x, Idx4x4)  ((mbGetCBPBlk2(GetMB2(info,mb_x)))&(1<<(12+(Idx4x4))))	
#define niGetQP(info, mb_x)						(mbGetQP2(GetMB2(info,mb_x)))
#define niIsIntraMb2(info, mb_x)					(IS_INTRA2(mbGetMbType2(GetMB2(info,mb_x))))
#define niIsFrameMb2(info, mb_x)					(!mbIsMbField3(GetMB2(info,mb_x)))

#define niIsIntraMb(info, mb_x)						(IS_INTRA(GetMB2(info,mb_x)))
#define niIsIntra4x4Mb(info, mb_x)					(IS_4x4_INTRA(GetMB2(info,mb_x)))

typedef enum{
	EMP_WAIT_DATA_ERROR = 0,//at this status, do nothing until the status is normal
	EMP_WAIT_DATA	= 1,
	EMP_READING		= 2,
	EMP_DECODING	= 3,
#ifdef USE_JOBPOOL	
	EMP_WAITING_JOB  = 4, 
#endif
	//EMP_DEBLOCKING	= 4,
	//EMP_DONE		= 5,
	EMP_MAX			= 0x7fffffff
}EMPStatus;
#if 1//FEATURE_INTERLACE
//ref the Table6-4 - Specification of mbAddrN and yM
typedef enum{
	YPOS_YN					,
	YPOS_YN_PLUS_MAXH_DIV_2 ,
	YPOS_YN_DIV_2			,
	YPOS_YN_MUL_2			,
	YPOS_YN_MUL_2_SUB_MAXH	,
	YPOS_YN_MUL_2_PLUS_1	,
	YPOS_YN_MUL_2_SUB_15	,
}EYPOSIdx;
typedef enum{
	MBADDR_A_TOP	, 
	MBADDR_A_BOTTOM	, 
	MBADDR_B_TOP	, 
	MBADDR_B_BOTTOM	, 
	MBADDR_C_TOP	, 
	MBADDR_C_BOTTOM	, 
	MBADDR_D_TOP	, 
	MBADDR_D_BOTTOM	, 
	MBADDR_Curr_TOP	, 
	MBADDR_Curr_BOTTOM	 

}EMBAddr;

#ifdef USE_JOBPOOL
#define MAX_SLICES_IN_FRAME 16
typedef enum
{
      EMP_JOB_INIT = 0,
      EMP_JOB_DATA_READY = 1,
      EMP_JOB_DECODING = 2,
      EMP_JOB_FINISHED = 3,
      EMP_JOB_ERROR = 4, 
}EMPJOBStatus;
#endif

typedef struct{

	EMBAddr	     mbAddr;
	EYPOSIdx	 yPosIdx;
	int			 offset2CurMB;
}TMBAddrNYPos;
typedef TMBAddrNYPos TNeighborA[2];

#if defined(USE_JOBPOOL)
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
typedef struct mrevent {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    avdNativeInt triggered;
}TMEvent;
#endif
#endif


//Damn GCC,can not use void* instead of struct _TMBsProcessor*
typedef TMBAddrNYPos (*GetNeighborAFunc)(void* info,TNeighborA* neighborAs,int yN,int maxH);
TMBAddrNYPos  GetNeighborAFrame2Field(void* info,TNeighborA* neighborAs,int yN,int maxH);
TMBAddrNYPos  GetNeighborAField2Frame(void* info,TNeighborA* neighborAs,int yN,int maxH);

int GetYPosition(int yPosIdx,int yN,int maxH);
#define ResetNeighborA_MBAFF(info)  (info)->getNeighborAFunc=(info)->neighborA=NULL
#define IsNeedToCalcualteNeighborA_MBAFF(info) ((info)->getNeighborAFunc&&GetLeftMB(info)==NULL)
#define IsNeighborA_Available_MBAFF(info) 	((info)->getNeighborAFunc||GetLeftMB(info))			
#endif//FEATURE_INTERLACE
typedef struct{
	struct img_par			*img;
	struct _TMBsProcessor	*currInfo;
	int			status;
	int			errorID;
#ifdef  USE_JOBPOOL
       int                 job_cnt;
       int                 coreID;
	int                 slice_job_pos;
	int                 slice_pos;
	int                 got_a_slice;	  
	struct img_par			*decimg;
#if defined(WIN32) || defined(WINCE)	
       HANDLE        JobEvent;	
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
        TMEvent      JobEvent;	
#endif
#if defined (_IOS) || defined(_MAC_OS)
     pthread_t thread_id;
#endif
#if CALC_FPS	   
	int                 WaitDataTimes;
       int                  WaitDecTimes;
#endif
#endif
}TMCoreThreadParam;


typedef struct _TMBsProcessor {
	//MacroBlock				*allocNMBs;
	MacroBlock				*neighborMBs;	//for MBAFF,it is the top
	//avdNativeInt			prevMBY;
	struct _TMBsParser*		mbsParser;
	struct avdIPDeblocker	*ipDeblocker;
	// short and signed char arrays addressing mode are much slower than others;				
	MacroBlock				*neighborABCD2[4];
#if 1//FEATURE_INTERLACE
	
	TNeighborA				*neighborA;//  
	GetNeighborAFunc		getNeighborAFunc;
	TMBAddrNYPos			neighborABCDPos[4];// starting y position of neighbor MB;
	MacroBlock				*neighborMBsBottom;//
	avdNativeInt			currMBisBottom;
#endif//FEATURE_INTERLACE
	MacroBlock				*neighborMBsActivate;//it is the neighborMBs(Top) or the neighborMBsBottom

	MacroBlock				*currMB;
	EMPStatus				status;
	//avdNativeInt			currMBNum;
	avdNativeInt			mb_y;
	avdNativeInt			mb_x;
	avdNativeInt			end_of_slice;
	avdNativeInt			flag;
	avdNativeInt			dec_mbaff_pos;//used for MBAff
	avdNativeInt			dec_mb_num;
	avdNativeInt			read_mb_num;
	avdNativeInt			start_mb_nr;//store the slice start_mb_nr, which is not thread safe
	struct _TMBsProcessor	*anotherProcessor;

	TMCoreThreadParam*				mcoreThreadParam;
#ifdef USE_JOBPOOL
       EMPJOBStatus                  job_state;
       avdNativeInt                    decNum;
	avdNativeInt                    isFirstOfSlice;	   
	   avdNativeInt                  current_slice_nr;
	   avdNativeInt                 curr_job_pos;
	avdNativeInt                    last_job;   
#endif
} TMBsProcessor;


#define IsTopMBAffNeighbor(info,pos) (((info)->neighborABCDPos[pos].mbAddr&1)==0)
#define IsNeighborBIsSelf(info) ((info)->neighborABCDPos[NEIGHBOR_B].mbAddr==MBADDR_Curr_TOP)
#define IsNeighborDIsA(info) ((info)->neighborABCDPos[NEIGHBOR_D].mbAddr<=MBADDR_A_BOTTOM)
#define GetLeftMBAFF(info) ((info)->neighborA)
#define GetLeftMB(info) ((info)->neighborABCD2[NEIGHBOR_A])
#define GetUpMB(info) ((info)->neighborABCD2[NEIGHBOR_B])
#define GetUpRightMB(info) ((info)->neighborABCD2[NEIGHBOR_C])
#define GetUpLeftMB(info) ((info)->neighborABCD2[NEIGHBOR_D])
#define GetLeftMBYPos(info) ((info)->neighborABCDPos[NEIGHBOR_A])
#define GetUpMBYPos(info) ((info)->neighborABCDPos[NEIGHBOR_B])
#define GetUpRightMBYPos(info) ((info)->neighborABCDPos[NEIGHBOR_C])
#define GetUpLeftMBYPos(info) ((info)->neighborABCDPos[NEIGHBOR_D])
#define GetCurrMB(info,x) (&info->neighborMBsActivate[x]) 
#define GetChromaYW(nBY) (((nBY) + 8)&7)
#define GetLumaYW(nBY) (((nBY) + 16)&15)

void	NeighborAB_Inference_CABAC(struct img_par *img,TMBsProcessor* info,MacroBlock* nMBs[2]);
MacroBlock* GetNeighborMB(struct img_par *img,TMBsProcessor *info,int addr);
MacroBlock *GetLeftMBAffMB2(struct img_par *img,TMBsProcessor *info,int yOff,int maxH, int* outYW);
#define RECALCULATE_MBAFF_ADDR 0x12345678
#define MAIN_PROCESSOR_FLAG 1234567

#endif //_AVD_NEIGHBOR_H_

