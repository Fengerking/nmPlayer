


#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#if !(defined (_IOS) || defined(_MAC_OS))
#define CALC_FPS 0
#define  SETAFFINITY 1
#define CALC_THUMBNAIL 0
#define NEW_CABAC 0
#define DECODE_BY_ROW 1
#endif

#define USE_IMG_MIRROR 1


#include "defines.h"
#include "parsetcommon.h"
#include "h264VdDump.h"													// for dump;
//#include "H264_D_API.h"
//#include		"voCheck.h"
#include "h264dec.h"
#include "voH264.h"
#include "sharedYUVMem.h"

#if defined(WIN32) || defined(WINCE)
#include		<windows.h>
#include		<winbase.h>
#endif

#ifdef USE_JOBPOOL
#include "threadAPI.h"
#endif
#if defined(X86_TEST) && defined(DEBUG_INFO)
#define TEST_TIME 0
#endif//
#if TEST_TIME
#ifdef TRACE_MEM
#undef TRACE_MEM
#endif
#define TIME_BEGIN(start)  (start)   = voH264GetSysTime();
#define TIME_END(start,sum)  if (img->dec_picture->fprofile)\
	img->dec_picture->fprofile->sum  += voH264GetSysTime() - (start);
typedef struct{
	int frameType;
	//read
	int readCBPSum;
	int readLumaDC16x16Sum;
	int readACSum;
	int readCDCSum;
	int iPredModeSum;
	int readCoefSum;
	int readMotionSum;
	int readIpModeSum;
	int readrunLevelSum;

	//dec
	int idctSum;
	int weightSum;
	int lmcSum;
	int cmcSum;
	int lmcSum2;
	int cmcSum2;
	int intra4x4Sum;
	int intra16Sum;
	int intra8x8Sum;
	int intracSum;

	//deblock
	int strcalSum;
	int filterSum;

	//summary
	int readSum;
	int readSliceSum;
	int decSum;
	int deblockSum;
	int total;
}TFrameProfile;
#define MAX_CHECK_FRAMES  1024
typedef struct{
	TFrameProfile list[MAX_CHECK_FRAMES];
	int curIdx;
	int refNum;
	int interlace;
	int mbaff;
	int profile;
	int cabac;
	int weight;
	int width;
	int height;
	int deblock_filter;
	int INum;
	int PNum;
	int BNum;
}TProfile;
#else//TEST_TIME
#define TIME_BEGIN(start)
#define TIME_END(start,sum)  
#endif//TEST_TIME
typedef struct{
	avdUInt8						b8Pdir[4];	// for 2bits for each block 1-4;
	avdUInt8						b8Mode[4]; // TBD: used in neighbor and deblocking;
	avdUInt8						b8Step[4]; //store the actual step of the p8x8, 2 (8x8)or 1 (4x4)
	avdUInt8						b8DirectDir[4]; //store the actual direction of direct space type
}TB8Info;
typedef struct _TMBBitStream{
	union {
		avdUInt8					mbIntra4x4Mode[16];
		TB8Info						b8Info;
	} uMBS;
	//(0-23)iTransFlags: 24 bits used for 24 blocks;
	//24:cbp==15
	//25:cbp>15
	//26-27:c_predmode
	//28-29:i16_predmode
	avdUInt32						iTransFlags;					

}TMBBitStream;
#if defined(RVDS)||defined(LINUX)
#define CALL_API
#else//RVDS
#define CALL_API __cdecl
#endif//RVDS
/*!
* typedef boolean value	 	
*/
typedef enum _VoiH264VdBoolean {
	VOIH264VD_FALSE				= 0,	
	VOIH264VD_TRUE				= 1,	
	VoiH264VdBoolean_END2           = 0x7fffffff,

} VoiH264VdBoolean;
/*!
*the stream type that the decoder supports	 	
*/
typedef enum _VoiH264FileFormat {
	VOIH264FM_ANNEXB				= 0,	/*!<the bitstream format defined in 14496-10 Annexb */
	VOIH264FM_14496_15				= 1,	/*!<the bitstream format defined in 14496-15,i.e. AVC file format */	
	VoiH264FileFormat_END2           = 0x7fffffff,

} VoiH264FileFormat;
typedef	int		VoiNumberErrors;

/*!
*																		
*		Structure of to size info of H.264								
*																		
*/
typedef struct _H264SizeInfo {
	unsigned int			frameHeight;	
	unsigned int			frameWidth;	
} H264SizeInfo;
/*!
* Structure of input parameters 
*/

/*!																										
*		The Macro denotes whether the call to the API is successful																										*									
*/
#define H264VDLIB_SUUCCEEDED(retCode) (retCode < VOIH264VD_FAILED_BEGIN)
/*!																										
*		Error codes and Macro for checking																										*									
*/


typedef enum _VoiH264VdReturnCode {
	VOIH264VD_SUCCEEDED								= 0x0001,
	VOIH264VD_DECODEAFRAME								= 0x0002,
	VOIH264VD_FAILED_BEGIN							= 0x4000,/*!<failed return code */
	VOIH264VD_UNKNOWN_FAILED						= VOIH264VD_FAILED_BEGIN,/*!<failed return code */

	VOIH264VD_OPEN_INPUT_FILE_FAILED				= VOIH264VD_FAILED_BEGIN + 0x1,/*!<invalid input parameters  */
	VOIH264VD_NULL_INPUT_POINTER					= VOIH264VD_FAILED_BEGIN + 0x2,/*!<invalid input parameters  */
	VOIH264VD_NULL_INPUT_PARAMETERS_POINTER			= VOIH264VD_FAILED_BEGIN + 0x3,/*!<invalid input parameters  */
	VOIH264VD_NULL_INPUT_BUFFER_POINTER				= VOIH264VD_FAILED_BEGIN + 0x4,/*!<invalid input parameters  */

	VOIH264VD_OPEN_OUTPUT_FILE_FAILED				= VOIH264VD_FAILED_BEGIN + 0x201,/*!<invalid output parameters  */
	VOIH264VD_NULL_OUTPUT_POINTER					= VOIH264VD_FAILED_BEGIN + 0x202,/*!<invalid output parameters  */

	VOIH264VD_H264_DATA_ERROR						= VOIH264VD_FAILED_BEGIN + 0x401,/*!<decoder errors  */
	VOIH264VD_INPUT_DATA_OVERFLOW					= VOIH264VD_FAILED_BEGIN + 0x401,/*!<decoder errors  */
	VOIH264VD_OUT_OF_MEMORY							= VOIH264VD_FAILED_BEGIN + 0x402,/*!<decoder errors  */
	VOIH264VD_FAILED_END							= 0x7fff, 
	VOIH264VD_FAILED_END2           = 0x7fffffff
} VoiH264VdReturnCode;
#define VO_H264FLAG_DEINTERLACE 0x00100000
typedef struct  
{
	VOCODECVIDEOBUFFER outData;
	int						  type;
	
	int						  flag;
	struct storable_picture			*picture;
	TTimeStamp				  time;
}OutDataQueueItem;
#define MAX_OUTDATA_QUEUE_SIZE 20
typedef struct
{
	OutDataQueueItem	queue[MAX_OUTDATA_QUEUE_SIZE];
	int					readIndex;
	int					writeIndex;
	int					frameNum;
}OutDataQueue;
// added by gtxia 2007-10-23
#if !BUILD_WITHOUT_C_LIB
#   include <stdio.h>
#endif 
//#define DBG_CHECK_SPEED							// turn off all output for speed check;
//#define SHOW_SNR

#define AVD_OBSOLETE_CODE // for code never used; TBD: delete it;
#if defined(WIN32)||defined(_WIN32_WCE)
  #define  snprintf _snprintf
#endif

struct img_par;
struct StorablePicture;

#ifdef SHOW_SNR
extern avdUInt8 **imgY_ref;                                //!< reference frame find snr
extern avdUInt8 ***imgUV_ref;
extern struct snr_par  *snr;
void report(struct snr_par *snr);
#endif //SHOW_SNR

//extern int FrameCount;
extern const avdUInt8 decode_scan[32];
extern const avdUInt8 inv_decode_scan[16];
extern const avdUInt8 avdNumLeadingZerosPlus1[256];
extern const avdUInt8 BLOCK_STEP[16];

// For MB level frame/field coding
//int  TopFieldForSkip_Y[16][16];
//int  TopFieldForSkip_UV[2][16][16];

//#define VERTICAL_SIZE 32
#define ET_SIZE 64      //!< size of error text buffer

//! Data Partitioning Modes
typedef enum
{
	PAR_DP_1,    //<! no data partitioning is supported
	PAR_DP_3,    //<! data partitioning with 3 partitions
	PAR_DP_TYPE_END2           = 0x7fffffff

} PAR_DP_TYPE;


//! Output File Types
typedef enum
{
	PAR_OF_ANNEXB,   //<! Current TML description
	PAR_OF_RTP,   //<! RTP Packet Output format
	//  PAR_OF_IFF    //<! Interim File Format
	PAR_OF_TYPE_END2           = 0x7fffffff,

} PAR_OF_TYPE;

//! definition of H.26L syntax elements
typedef enum {
	SE_HEADER,
	SE_PTYPE,
	SE_MBTYPE,
	SE_REFFRAME,
	SE_INTRAPREDMODE,
	SE_MVD,
	SE_CBP_INTRA,
	SE_LUM_DC_INTRA,
	SE_CHR_DC_INTRA,
	SE_LUM_AC_INTRA,
	SE_CHR_AC_INTRA,
	SE_CBP_INTER,
	SE_LUM_DC_INTER,
	SE_CHR_DC_INTER,
	SE_LUM_AC_INTER,
	SE_CHR_AC_INTER,
	SE_DELTA_QUANT_INTER,
	SE_DELTA_QUANT_INTRA,
	SE_BFRAME,
	SE_EOS,
	SE_MAX_ELEMENTS //!< number of maximum syntax elements, this MUST be the last one!
} SE_type;        // substituting the definitions in element.h

typedef enum {
	UVLC = 0,
	CABAC
} SymbolMode;


typedef enum {
	FRAME = 0,
	TOP_FIELD,
	BOTTOM_FIELD
} PictureStructure;           //!< New enum for field processing


typedef enum {
	P_SLICE = 0,
	B_SLICE,
	I_SLICE,
	SP_SLICE,
	SI_SLICE,
	SliceType_END2           = 0x7fffffff

} SliceType;

typedef enum{
	PRO_BASELINE			= 66,
	PRO_MAIN				= 77,
	PRO_EXTENT				= 88,
	PRO_HIGH				= 100,
	PRO_HIGH10				= 110,					/**< High 10 profile */
	PRO_HIGH422				= 122,					/**< High 4:2:2 profile */
	PRO_HIGHh444			= 144,					/**< High 4:4:4 profile */
	H264PROFILE_END2        = 0x7fffffff

}H264PROFILE;
#if COMBINE_ALL_PROFILE
typedef struct  
{
	//other params needed to save to gData after initbysps
	CALLBACK_OutputOneFrame			OutPutOneFrame;
	VOMEMORYOPERATOR				customMem;
	int								disableDeblock;
}SDKInParam;
typedef struct  
{
	//the params needed to get from gData after decode
	int								yPlnPitch;
	int								number;
	int								error;
	int								type;
	
}SDKOutParam;
#endif//COMBINE_ALL_PROFILE
/***********************************************************************
 * D a t a    t y p e s   f o r  C A B A C
 ***********************************************************************
 */
//! struct for context management
typedef struct
{
	//***** do not add, remove or change the order below;
	// assumption: offsets of state and MPS are 0 and 1 in biari_decode_symbol assmembly code;

	avdUInt8			state;         // index into state-table CP  
	avdUInt8			MPS;           // Least Probable Symbol 0/1 CP

	//***** do not add, remove or change the order above;
} BiContextType;

typedef BiContextType *BiContextTypePtr;

//modify for visualon 264

#define NUM_MB_TYPE_CTX  11
#define NUM_B8_TYPE_CTX  9
#define NUM_MV_RES_CTX   10
#define NUM_REF_NO_CTX   6
#define NUM_DELTA_QP_CTX 4
#define NUM_MB_AFF_CTX 4


typedef struct
{

	BiContextType mb_type_contexts [4][NUM_MB_TYPE_CTX];
	BiContextType b8_type_contexts [2][NUM_B8_TYPE_CTX];
	BiContextType mv_res_contexts  [2][NUM_MV_RES_CTX];
	BiContextType ref_no_contexts  [2][NUM_REF_NO_CTX];
	BiContextType delta_qp_contexts[NUM_DELTA_QP_CTX];
#if FEATURE_INTERLACE
	BiContextType mb_aff_contexts  [NUM_MB_AFF_CTX];
#endif//FEATURE_INTERLACE

} MotionInfoContexts;

#define NUM_IPR_CTX    2
#define NUM_CIPR_CTX   4
#define NUM_CBP_CTX    4
#define NUM_BCBP_CTX   4
#define NUM_MAP_CTX   15
#define NUM_LAST_CTX  15
#define NUM_ONE_CTX    5
#define NUM_ABS_CTX    5
#define NUM_TRANSFORM_SIZE_CTX 3

typedef struct
{
	BiContextType  ipr_contexts [NUM_IPR_CTX];
	BiContextType  cipr_contexts[NUM_CIPR_CTX]; 
	BiContextType  cbp_contexts [3][NUM_CBP_CTX];
	BiContextType  bcbp_contexts[NUM_BLOCK_TYPES][NUM_BCBP_CTX];
	BiContextType  map_contexts [NUM_BLOCK_TYPES][NUM_MAP_CTX];
	BiContextType  last_contexts[NUM_BLOCK_TYPES][NUM_LAST_CTX];
	BiContextType  one_contexts [NUM_BLOCK_TYPES][NUM_ONE_CTX];
	BiContextType  abs_contexts [NUM_BLOCK_TYPES][NUM_ABS_CTX];
#if FEATURE_INTERLACE
	BiContextType  fld_map_contexts [NUM_BLOCK_TYPES][NUM_MAP_CTX];
	BiContextType  fld_last_contexts[NUM_BLOCK_TYPES][NUM_LAST_CTX];
#endif//FEATURE_INTERLACE
#if FEATURE_T8x8
	BiContextType  transform_size_contexts [NUM_TRANSFORM_SIZE_CTX];
#endif//FEATURE_T8x8
} TextureInfoContexts;


//*********************** end of data type definition for CABAC *******************

/*! Buffer structure for decoded referenc picture marking commands */
typedef struct DecRefPicMarking_s
{
	struct DecRefPicMarking_s *Next;
	avdInt16 memory_management_control_operation;
	avdInt16 difference_of_pic_nums_minus1;
	avdInt16 long_term_pic_num;
	avdInt16 long_term_frame_idx;
	avdInt16 max_long_term_frame_idx_plus1;
} DecRefPicMarking_t;

//! Syntaxelement
typedef struct syntaxelement
{
	//! for mapping of UVLC to syntaxElement
	avdNativeInt	value1;                //!< numerical value of syntax element
	avdNativeInt	value2;                //!< for blocked symbols, e.g. run/level

	avdNativeInt	inf;                   //!< info part of UVLC code
	avdNativeInt	context;               //!< CABAC context
	avdNativeInt	k;                     //!< CABAC context for coeff_count,uv
	
	avdNativeInt	type;                  //!< type of syntax element for data part.

	avdNativeInt	len; 

	 //! used for CABAC: refers to actual coding method of each individual syntax element type
	//  void  (*reading)(struct syntaxelement *, struct inp_par *, struct img_par *, DecodingEnvironmentPtr);
#if TRACE
#define       TRACESTRING_SIZE 100           //!< size of trace string
	char          tracestring[TRACESTRING_SIZE]; //!< trace string
#endif
} SyntaxElement;

// Be careful: mbIs...()==TRUE returns nonZero(not 1), to save a shift;
// do not use a = mbIs...(), if a needs to be 1 if true;
// use a = mbIs...() ? 1 : 0;
// first 8 bits
//7th is SliceNR, which is used for detecting if the adjacent MB is in same slice, so keep one bit is enough
#define mbSetSliceNR(mb, val)		(((val)&0x01)?((mb)->mbFlag1 & 0x00004000):((mb)->mbFlag1 &= 0xffffbfff))
#define mbGetSliceNR(mb)			((mb)->mbFlag1 & 0x00004000)
#define mbMarkEi(mb)				//((mb)->mbFlag1 |= 0x4000)
#define mbUnMarkEi(mb)			//	((mb)->mbFlag1 &= 0xbfff)
#define mbIsEi(mb)				//	((mb)->mbFlag1 & 0x4000)
//1-6 is MbType
#define mbGetMbType(mb)				(((mb)->mbFlag1>>8)&0x3f)
#define mbGetMbType2(mb)				(((mb)->mbFlag2>>8)&0x3f)
#define mbSetMbType(mb, val)		(mb)->mbFlag1 = (((mb)->mbFlag1&0xffffc0ff) | ((val)<<8))
#define mbIsMbTypeZero(mb)			(!((mb)->mbFlag1&0x00003f00))
#define mbIsMbTypeNonZero(mb)		!mbIsMbTypeZero(mb)
#define mbSetMbFlag(mb1,mb2)		((mb1)->mbFlag1 =(mb2)->mbFlag1)
#if FEATURE_CABAC
#define mbSetMbB8Mode(mb1,mb2)		((mb1)->mbB8Mode =(mb2)->mbB8Mode)
#else//FEATURE_CABAC
#define mbSetMbB8Mode(mb1,mb2)	
#endif//FEATURE_CABAC
//8 is the 8x8 Flag
#define mbGetMbT8x8Flag(mb)				((mb)->mbFlag1 & 0x00008000)
#define mbMarkMbT8x8Flag(mb)				((mb)->mbFlag1 |= 0x00008000)
#define mbUnMarkMbT8x8Flag(mb)				((mb)->mbFlag1 &= 0xffff7fff)
#define mbIsMbT8x8Flag(mb)				(mbGetMbT8x8Flag(mb)!=0)
#define mbIsNotMbT8x8Flag(mb)				(!mbIsMbT8x8Flag(mb))

// 2nd 8 bits;
#define mbMarkSkipped(mb)			((mb)->mbFlag1 |= 0x00000040)
#define mbUnMarkSkipped(mb)			((mb)->mbFlag1 &= 0xffffffbf)
#define mbIsSkipped(mb)				(((mb)->mbFlag1 & 0x00000040)!=0)
#define mbGetQP(mb)					((mb)->mbFlag1&0x3f)
#define mbGetQP2(mb)				((mb)->mbFlag2&0x3f)
#define mbSetQP(mb, val)			(mb)->mbFlag1 = ((mb->mbFlag1&0xfffffc0) | val)
#define mbResetMB(mb)				(mb)->mbFlag1 = 0
#define mbSaveMB(mb)				(mb)->mbFlag2 = (mb)->mbFlag1

#define mbGetCBPBlk(mb)				(((mb)->mbFlag1>>16)&0x0000ffff)
#define mbGetCBPBlk2(mb)				(((mb)->mbFlag2>>16)&0x0000ffff)
#define mbSetCBPBlk(mb,val)			((mb)->mbFlag1|= ((val)<<16)&0xffff0000)
#define mbMarkCBPBlk(mb)			((mb)->mbFlag1|= 0xffff0000)
#define mbResetCBPBlk(mb)				((mb)->mbFlag1&= 0x0000ffff)

#ifdef USE_JOBPOOL
#define set_1bit(img, n)   ((n < 32) ? (img->statea |= (1 << n)) : ( (n < 64) ? (img->stateb |= (1 << (n - 32))) : ( (n < 96) ? (img->statec |= ( 1 << (n-64))):(img->stated |= ( 1 << (n-96))))))
#define clr_1bit(img, n)   ((n < 32) ? (img->statea &= (~(1 << n))) : ( (n < 64) ? (img->stateb &= (~(1 << (n - 32)))) : ((n < 96) ? (img->statec &= (~( 1 << (n-64)))):(img->stated &= (~( 1 << (n-96)))))))
#endif

#if FEATURE_INTERLACE
	#define mbSetMbFieldValue(mb, n)	(n ? mbMarkMbField(mb) : mbUnMarkMbField(mb))
	#define mbIsMbField(mb)			(((mb)->mbFlag1 & 0x00000080)!=0)
	
	#define mbMarkMbField(mb)		((mb)->mbFlag1 |= 0x00000080)
	#define mbUnMarkMbField(mb)		((mb)->mbFlag1 &= 0xffffff7f)

	#define mbSetMbFieldValue3(mb, n)	(n ? mbMarkMbField3(mb) : mbUnMarkMbField3(mb))
	#define mbIsMbField3(mb)			(((mb)->mbFlag2 & 0x00000080)!=0)
	#define mbMarkMbField3(mb)		((mb)->mbFlag2 |= 0x00000080)
	#define mbUnMarkMbField3(mb)		((mb)->mbFlag2 &= 0xffffff7f)
	
	#define IsUpField(mb) mbIsMbField3(mb)
	#define IsUpField2(info,currMB,upMB) ((IsNeighborBIsSelf(info))? mbIsMbField(currMB):mbIsMbField3(upMB))
	
#else // FEATURE_INTERLACE
	#define mbSetMbFieldValue(mb, n)	mbUnMarkMbField(mb)
	#define mbIsMbField(mb)			0
	#define mbIsMbField3(mb)			0
	#define mbMarkMbField(mb)		//((mb)->mbFlag1 |= 0x0080)
	#define mbUnMarkMbField(mb)	//	((mb)->mbFlag1 &= 0xff7f)
	#define IsUpField(mb) 0
	#define IsUpLeftField(mb) 0
	#define IsUpRightField(mb) 0
#endif //FEATURE_INTERLACE

// bit locations of mbFlag1 is defined as follows; we will use macro to access it;
typedef struct _mbFlag1DefinitionForReference {
	// first 8 bits;
	avdNativeUInt		t8x8Flag:1; 
	avdNativeUInt		sliceNR:1; 
	avdNativeUInt		mb_type:6; 
	// 2nd 8 bits;
	avdNativeUInt		mb_field:1; 
	avdNativeUInt		is_skipped:1; 
	avdNativeUInt		qp:6; 
} mbFlag1DefinitionForReference;
enum{
	B_DIRECT_TEMPORAL = 10,
	B_DIRECT_SPACE	  = 11,
};

#define mbGetB8Mode(mb, i) //((mb->mbB8Mode>>((i)<<2))&0xf)
#define mbGetB8ModeFromValue(val, i) //((val>>((i)<<2))&0xf)
#define mbSetMbB8FromImgB8(mb, imgB8Mode) //(mb->mbB8Mode = (imgB8Mode[0] | (imgB8Mode[1]<<4) | (imgB8Mode[2]<<8) | (imgB8Mode[3]<<12)))
//! Macroblock
#if 0
typedef struct macroblock
{
	avdUInt16			mbFlag1; // 16 bits;
	avdUInt16			cbp_blk;  //16bits, same as nonZeroLuma in encoder; 16 bits 4x4 Luma; raster-scan order;
#if FEATURE_CABAC
	avdUInt16			mbB8Mode; // 16 bits, 4 bits for each 8x8 blocks;
	//avdUInt8            slice_nr; // TBD: change to save slice;
	//avdUInt8			padding;
#endif //VOI_INPLACE_DEBLOCKING
} Macroblock;
#else
#define Macroblock  struct _MacroBlock
#endif
// ToBeCheck: GETBITSx86 is not much faster;
//#define GETBITSx86
//#define GETBITSMMX

#ifdef GETBITSx86
#undef GETBITSMMX
#endif //GETBITSx86
//! Bitstream
typedef struct
{
	// TBD: remove the assembly code dependency of the size of memory;
	//***** do not add, remove or change the order below;
	// assumption: offsets of Drange and Dvalue are 40 and 42 in biari_decode_symbol assmbly code;
	avdUInt8		*streamBuffer;      //!< actual codebuffer for read bytes
	avdUInt8		*currReadBuf;			// read position;
#ifdef GETBITSMMX
	avdInt64		buffer64;			// 32 bits buffer for quick ShowBits and GetBits;
#else //GETBITSMMX
	avdUInt32		bufferA;			// 32 bits buffer for quick ShowBits and GetBits;
	avdUInt32		bufferB;			// 32 bits buffer for quick ShowBits and GetBits;
#endif //GETBITSMMX
	avdNativeInt	bBitToGo;
	avdNativeInt	ei_flag;            //!< error indication, 0: no error, else unspecified error
	// UVLC Decoding
	avdInt32		frame_bitoffset;    //!< actual position in the codebuffer, bit-oriented, UVLC only
	avdInt32		bitstream_length;   //!< over codebuffer lnegth, avdUInt8 oriented, UVLC only
	// CABAC Decoding
	avdInt32		read_len;           //!< actual position in the codebuffer, CABAC only
	avdInt32		code_len;           //!< overall codebuffer length, CABAC only
	avdUInt16		Drange;
	avdUInt16		Dvalue;
	//***** do not add, remove or change the order above;

	// ErrorConcealment
} Bitstream;

//! DataPartition
typedef struct datapartition
{
	Bitstream           *bitstream;
#ifdef GETBITSMMX
	void				*reserved; // padding to make it 8 bytes aligned; 
#endif //GETBITSMMX
} DataPartition;
typedef avdInt16 TWeightArray[AVD_MOTION_INFO_SIZE][MAX_REFERENCE_PICTURES][3];
typedef avdInt16 TWeightArray2[AVD_MOTION_LIST_SIZE][MAX_REFERENCE_PICTURES][3];
//! Slice
#ifndef _IOS
#ifndef _MAC_OS
struct ImageParameters;
#endif // _IOS
#endif // _MAC_OS
typedef struct
{
	
	avdNativeUInt				structure;     //!< Identify picture structure type
	avdNativeUInt				direct_type;

	avdNativeUInt				start_mb_nr;   //!< MUST be set by NAL even in case of ei_flag == 1
	//avdNativeUInt				end_mb_nr;
	//avdNativeUInt				end_of_slice;
	//  int                 last_mb_nr;    //!< only valid when entropy coding == CABAC
	DataPartition				*partArr;      //!< array of partitions
	avdNativeUInt               ref_pic_list_reordering_flag_l0;
	MotionInfoContexts			*mot_ctx;      //!< pointer to struct of context models for use in CABAC
	TextureInfoContexts			*tex_ctx;      //!< pointer to struct of context models for use in CABAC
	avdNativeUInt               ref_pic_list_reordering_flag_l1;
	
	avdNativeUInt               pic_parameter_set_id;   //!<the ID of the picture parameter set the slice is reffering to
	
	avdNativeUInt				max_part_nr;
	avdNativeUInt				dp_mode;       //!< data partioning mode
	avdNativeUInt               qp;
	avdNativeUInt               picture_type;  //!< picture type
	avdNativeUInt               next_header;
	avdNativeUInt				ei_flag;
	avdNativeUInt				LFDisableIdc;
	avdNativeInt                LFAlphaC0Offset;  //!< Alpha and C0 offset for filtering slice
	avdNativeInt				LFBetaOffset;     //!< Beta offset for filtering slice
	struct avdNALU				*nalu;
	avdNativeInt                remapping_of_pic_nums_idc_l0[MAX_REFERENCE_FRAMES];
	avdNativeInt                abs_diff_pic_num_minus1_l0[MAX_REFERENCE_FRAMES];
	avdNativeInt                long_term_pic_idx_l0[MAX_REFERENCE_FRAMES];
	avdNativeInt                remapping_of_pic_nums_idc_l1[MAX_REFERENCE_FRAMES];
	avdNativeInt                abs_diff_pic_num_minus1_l1[MAX_REFERENCE_FRAMES];
	avdNativeInt                long_term_pic_idx_l1[MAX_REFERENCE_FRAMES];
} Slice;

#if SUPPORT_SOS
#define SLICELIST_NUM  24	//24 is enough
//extern Slice* SliceList[SLICELIST_NUM];
#endif//SUPPORT_SOS



#define AVD_LITTLE_ENDIAN
#ifdef AVD_LITTLE_ENDIAN
#define AVD_MAKEMV_XY32(x, y) ((y<<16) | (x & 0xffff))
#else
#define AVD_MAKEMV_XY32(x, y) ((x<<16) | (y & 0xffff))
#endif

typedef struct avdMotionVector
{
	avdInt16				x;
	avdInt16				y;
} AVDMotionVector;

#include "avd_neighbor.h"
// assume AVDMotionVector only have 32 bits;
#define AVD_ASSIGN_MV(a, b)			*(avdUInt32 *)(a) = *(avdUInt32 *)(b)
#define AVD_ASSIGN_MV2(a, x,y)			*(avdUInt32 *)(a) = ((((y)<<16)&0xffff0000)|((x)&0x0000ffff))

typedef struct avdIPDeblocker
{
	avdUInt8				strength[32];
	avdUInt8				leftBoundary[96];//0-15 TopMB_Luma,16-31 BottomMB_Luma,32-39 TopMB_ChromaU,40-47 bottomMB_ChromaU,48-63(TopV-BotV) 64-95(backup top)...
	avdUInt8				yuvD[4];//it is not used in Interlace mode
	avdUInt8				*topY;//(width+isInterlace*width)*(isInterlace+1) 4Y row (2 top + 2 bottom)
	avdUInt8				*topUV[2];//4u row (2 top + 2 bottom)followed by 4v row (2 top + 2 bottom)
} AVDIPDeblocker;

#define GetMBAffTopY(dbk,isTop,isLastRow) ((dbk)->topY+img->sizeInfo->width*((isLastRow)+(!(isTop))*2))
#define GetMBAffTopUV(dbk,isTop,isLastRow,i) ((dbk)->topUV[i]+(img->sizeInfo->width>>1)*((isLastRow)+(!(isTop))*2))
#define GetMBAffLeftY(dbk,isTop) ((dbk)->leftBoundary + (!(isTop))*16)
#define GetMBAffLeftUV(dbk,isTop,i) ((dbk)->leftBoundary + 32 + (i)*16 + (!(isTop))*8)
#define GetMBAffLeftYTopBackup(dbk,isTop) ((dbk)->leftBoundary + 64)
#define GetMBAffLeftUVTopBackup(dbk,isTop,i) ((dbk)->leftBoundary + 80 +  i*8)
//#define GetTopBottomMBY(dbk)  ((dbk)->topY+img->width*2)
//#define GetTopBottomMBUV(dbk,i)  ((dbk)->topUV[i]+img->width)
#define GetLeftMBY(dbk)  ((dbk)->leftBoundary)
//#define GetLeftBottomMBY(dbk)  ((dbk)->leftBoundary + 16)
#define GetLeftMBUV(dbk,i)  ((dbk)->leftBoundary+32+(i)*8)
//#define GetLeftBottomMBUV(dbk,i)  ((dbk)->leftBoundary+48+(i)*8)

// for b8pdir(2bits each B8 block); no predition use 3;

#define B8FWPRED    0
#define B8BWPRED    1
#define B8BIPRED    2
#define	B8NOPRED	3

extern const avdUInt8 avd_assignSE2partition[][SE_MAX_ELEMENTS];


#if FEATURE_BFRAME
	#define	HasNoB8FwPred(val) (val&1)
	#define	HasB8FwPred(val)   !(val&1)
	#define	HasNoB8BwPred(val) (!val || val == 3)
	#define	HasB8BwPred(val)   (val && val != 3)
	#define EqualFwPred(val)   (val == B8FWPRED)
	#define EqualBwPred(val)   (val == B8BWPRED)
#else //VOI_H264D_NON_BASELINE
	#define	HasNoB8FwPred(val) 0
	#define	HasB8FwPred(val)   1
	#define	HasNoB8BwPred(val) 1
	#define	HasB8BwPred(val)   0
	#define EqualFwPred(val)   1
	#define EqualBwPred(val)   0
#endif // VOI_H264D_NON_BASELINE

#define			AVD_SCRATCH_HEIGHT		24
#define			AVD_SCRATCH_WIDTH		24
#ifdef TRACE_MEM
typedef struct  
{
	int	 id;
	int  isOverFlow;
	int  isAlloc;
	int	 address;
	int  size;
	int  offset;
	
}VOMemTableItem;
#endif//TRACE_MEM
typedef struct
{
	avdUInt32				Size;				/*!< Buffer stride */
	avdInt32				Flag;
	void*				VBuffer;			/*!< user data pointer */
	void*				PBuffer;			/*!< user data pointer */
}VOMEM_INFO;
#define ENABLE_OTHER_ALLOC  0
typedef struct  
{
#if ENABLE_OTHER_ALLOC 
	void* (VOAPI *Alloc)(int size, unsigned int* pPhysAddr);
	int	 (VOAPI * SetFrameCount) (int nWidth, int nHeight, int nFrames);				/*!< return value > 0 OK, */
	void * (VOAPI * Alloc2)(int nWidth, int nHeight, VOCODECVIDEOBUFFER * pVideoInfo);
	
	void (VOAPI * Free) (void * pBuffer);												/*!< pBuffer was returned by Alloc */
	int  (VOAPI * Lock) (void * pBuffer);												/*!< pBuffer was returned by Alloc */
	void (VOAPI * Unlock) (void * pBuffer);												/*!< pBuffer was returned by Alloc */
	int  (VOAPI * SetParam) (void * pBuffer, int nID, int nValue);						/*!< pBuffer was returned by Alloc */
	int  (VOAPI * GetParam) (void * pBuffer, int nID, int * pValue);					/*!< pBuffer was returned by Alloc */

	void * (VOAPI * Alloc3)(void* handle,int size, unsigned int* pPhysAddr);
	void (VOAPI * Free3) (void* handle,void * pBuffer);
#endif
	//new SDK from
	int  (VOAPI * Alloc4) (int uID, VOMEM_INFO * pMemInfo);
	int (VOAPI * Free4) (int uID, void* pBuff);
	int (VOAPI * Set) (int uID, void* pBuff, unsigned char uValue, unsigned int uSize);
	int (VOAPI * Copy) (int uID, void* pDest, void* pSource, unsigned int uSize);
	int (VOAPI * Check) (int uID, void* pBuffer, unsigned int uSize);
}VOH264MEMOP;
#if 0
typedef struct
{
	/**IN PARAM*/
	long				Stride;				/*!< Buffer Stride */
	long				Height;				/*!< Buffer Height */
	long				ColorType;			/*!< Color Type,default:YUV_PLANAR420 */
	long				FrameCount;			/*!< Frame counts */
	long				Flag;				/*!< reserved: for special use */
	/**OUT PARAM*/
	void**			VBuffer;			/*!<virtual Address Buffer array */
	void**			PBuffer;			/*!<reserved: physical Address Buffer array */
	long**			actualStride;	/*!<reserved: Buffer actual stride*/
}
VOH264SHAREDMEMOP_INFO;


typedef struct
{
	unsigned long (VOAPI * Init) (long uID, VOH264SHAREDMEMOP_INFO * pVideoMem);
	unsigned long (VOAPI * GetBufByIndex) (long uID, long nIndex);
	unsigned long (VOAPI * Uninit) (long uID);
} VOH264SHAREDMEMOP;
enum{
	VOMEMRC_NO_YUV_BUFFER=-2233/*!< Hex:0xF747,when caller get the return,the usual handle is to resend the inputdata */
};
#else//0
typedef	VOMEM_VIDEO_INFO VOH264SHAREDMEMOP_INFO;
typedef	VOMEM_VIDEO_OPERATOR	VOH264SHAREDMEMOP;	
#endif//0
typedef struct  
{
	void* start;
	int	  offset;
	int	  size;
}GMemData;
typedef avdUInt32 (VOAPI  *_voCheckLibInit) (void** phCheck, avdUInt32 nID, avdUInt32 nFlag, void* hInst,void*);
typedef avdUInt32 (VOAPI  *_voCheckLibCheckVideo) (void* hCheck, void* pOutBuffer, void* pOutInfo);
typedef avdUInt32 (VOAPI  *_voCheckLibResetVideo) (void* hCheck, void* pOutBuffer);
typedef avdUInt32 (VOAPI  *_voCheckLibUninit) (void* hCheck);

typedef struct{

	_voCheckLibInit						init;
	_voCheckLibCheckVideo		checkVideo;
	_voCheckLibResetVideo			resetVideo;
	_voCheckLibUninit					unInit;
	void*										hCheck;
	void*								instance;
	avdUInt8*								lastVideo;
}TLicenseCheck2;
typedef struct _H264VdLibParam {
	

	unsigned char			*inBuf;	/*!< the buffer of the input data,It should be allocated by the client of the libary*/
	unsigned int			inBufDataLen; /*!<  input data length in inBuf at each H264ReceiveData();*/

	//VoiNumberErrors			(*OutputAFrame)(struct _H264VdLibParam *);/*!<  user-defined callback function when a frame is already decoded and UV data are ready in *outBufArr[3] for users to use;return 0, if no error; return others indicating errors, and decoder will stop;*/

	//VoiH264VdBoolean		endOfSequence;/*!<  turn on the flag after data of a complete sequence are sent to decoder;decoder will try to decode the last part of data; do not turn on thisflag, if you are not sure whether the data is complete or not.*/

	//void					*userData;/*!< for users to hook up a pointer to anything so that they can use it ;in OutputAFrame callback function if needed; */
	VoiH264FileFormat		fileFormat;


	unsigned char			*outBufArr[3];/*!<output buffer; for YUV420 output, outBufArr[0] = start of Y output, outBufArr[1] = start of U output, outBufArr[2] = start of V outputmemories of outBufArr[0 to 2] are allocated and deleted inside decoder;*/
	unsigned char			*deInterlaceOutBuf[3];
	unsigned int			outBufDataLen[3];/*!< corresponding output data lengths of outBufArr;;*/

	H264SizeInfo			sizeInfo;/*!< width and height of Luma;*/
	//below are for NEW API
	//int						IsOneFrameOutput;/*!it is for HTC API specially who dose not use call back function*/
	//VOH264FLUSHBUF			*flushBuf;	
	//int						IsGetFirstFrame;/*!<it is set value as 1,it will force the decoder to output the first frame immediately*/
	int						enableCopyInput;
	int						disableDeblock;
	int						enableOutputASAP;
	int						outputOUTPUTMODE;
	int						enableJump2I;
	int						enableFirstFrameNonIntra;
	int						enableParticialDecode;
	int						enableReuseValidSPS;
	int						enbaleSEI;
	int								deblockFlag;
	int								deblockRowNum;//for iMX31 pause mode
	VOH264SHAREDMEMOP*				sharedMem;
	void*							iMX31GlobalData;
	VOH264MEMOP*					customMem2;
	VOH264SHAREDMEMOP_INFO			*sharedMemInfo;
	int								allocatedSharedMem;
	int								multiCoreNum;
#ifdef SETAFFINITY
    int                             for_unittest;
#endif
	int								rowNumOfMBs;
	int								optFlag;
	int								cpuVersion;
	int								callbackThread;
	TTimeStamp						timeStamp;
} H264VdLibParam;
// image parameters

//weighted prediction
typedef struct{
	avdNativeUInt					luma_log2_weight_denom;
	avdNativeUInt					chroma_log2_weight_denom;
	avdInt16						wbp_weight[AVD_MOTION_LIST_SIZE][3][MAX_REFERENCE_PICTURES*MAX_REFERENCE_PICTURES]; //weight in [list][component][fw_index*bw_index] order
	avdNativeInt					wp_round_luma;
	avdNativeInt					wp_round_chroma;
	TWeightArray					wp_weight;
	TWeightArray2					wp_offset;
	//avdInt32						weightBuf[sizeof(TWeightArray)];        //TWeightArray is avdInt16, so the buf 
}TWeightInfo	;

// End JVT-D101
// POC200301: from unsigned int to int
typedef struct{


	avdInt32						poc_interval;
	avdInt32						prevPoc, currPoc;
	avdInt32						toppoc;      //poc for this top field // POC200301
	avdInt32						bottompoc;   //poc of bottom field of frame
	avdInt32						framepoc;    //poc of this frame // POC200301
	avdInt32						isNegtivePocExisted;
	// POC200301
	//the following is for slice header syntax elements of poc
	// for poc mode 0.
	avdInt32						PicOrderCntLsb;
	avdInt32						PicOrderCntMsb;
	avdInt32						PrevPicOrderCntMsb;
	avdInt32						PrevPicOrderCntLsb;
	
	// for POC mode 1:
	avdInt32						ExpectedPicOrderCnt, PicOrderCntCycleCnt, FrameNumInPicOrderCntCycle;
	avdInt32						ExpectedDeltaPerPicOrderCntCycle;
	avdInt32						delta_pic_order_cnt[3];
	avdInt32						delta_pic_order_cnt_bottom;
}TPOCInfo;
typedef struct{
	avdNativeUInt					slice_group_change_cycle;
	avdNativeInt					NumberOfSliceGroups;    // the number of slice groups -1 (0 == scan order, 7 == maximum)
	avdUInt8						*MbToSliceGroupMap;
	avdUInt8						*MapUnitToSliceGroupMap; 
	avdNativeUInt					prevMbToSliceGroupMapIdx;
	avdNativeUInt					prevSliceGroupMapType; //pps->pic_parameter_set_id;
	avdNativeUInt					sliceGroupMapUnchanged;	
}TFMOInfo;
typedef struct{
	void (*read_CBP_and_coeffs_from_NAL) (struct img_par *img,TMBsProcessor* info,Bitstream* bs);
	void (*read_motion_info_from_NAL) (struct img_par *img,TMBsProcessor* info,Bitstream* bs);
	avdNativeInt (*read_one_macroblock      ) (struct img_par *img,TMBsProcessor* info,Bitstream* bs);
	void (*interpret_mb_mode        ) (struct img_par *img,TMBsProcessor* info);
}TFuncInfo;
typedef struct{
	// could be one bit flag;
	avdNativeUInt					idr_flag;
	avdNativeUInt					frame_num;   //frame_num for this frame
	DecRefPicMarking_t				dec_ref_pic_marking_buffer[MAX_REFERENCE_FRAMES];                    //<! stores the memory management control operations
	avdNativeUInt					nal_reference_idc;                       //!< nal_reference_idc from NAL unit
	avdNativeUInt					idr_pic_id;
	
	// for signalling to the neighbour logic that this is a deblocker call
	avdNativeInt					last_has_mmco_5;
	avdNativeUInt					redundant_pic_cnt;
	avdNativeUInt					no_output_of_prior_pics_flag;
	avdNativeUInt					long_term_reference_flag;
	avdNativeUInt					adaptive_ref_pic_buffering_flag;

	avdNativeUInt					structure;
	avdNativeUInt					disposable_flag; //!< flag for disposable frame, 1:disposable
	avdInt32						totalFrameNumBeforeIDR;
	avdInt32						PreviousFrameNumOffset, FrameNumOffset;
	avdUInt32						PreviousFrameNum;
	avdUInt32						AbsFrameNum;
	avdNativeInt					cod_counter;                            //<! Current count of number of skipped macroblocks in a row
	
	avdNativeUInt					pre_frame_num;           //!< store the frame_num in the last decoded slice. For detecting gap in frame_num.
	avdInt32						MaxFrameNum; // > 0 & <= 2^16;
#if FEATURE_INTERLACE	
	avdNativeInt					last_pic_bottom_field;
	avdNativeInt					bottom_field_flag;
#endif//FEATURE_INTERLACE
#define  MAX_T35_SIZE  8192
	VO_CODECBUFFER					seiBuf;
}TSpecitialInfo;

typedef struct{
	avdUInt8						*in_buffer;	
	void*							out_buffer;
	long							hasNewInput;
	int								outNumber;
	int								inNumber;
	int								prevInStructure;
	int								sliceNum;//for debug dump trace
	int								IsSeekingNextIFrame;
	int								lengthSizeOfNALU;//for AVC format
	OutDataQueue					outDataQueue;//[MAX_OUTDATA_QUEUE_SIZE];
}TIOInfo;
	

typedef struct{	

	avdNativeInt					width;
	avdNativeInt					height;
	avdNativeInt					width_cr;                               //<! width chroma
	avdNativeInt					height_cr;                              //<! height chroma
	avdNativeInt					yPlnPitch;
	avdNativeInt					PicSizeInMbs;
	avdNativeInt					FrameSizeInMbs;
	avdNativeInt					PicWidthInMbs;
	avdNativeInt					PicHeightInMapUnits;
	avdNativeInt					FrameHeightInMbs;
	avdNativeInt					PicHeightInMbs;
}TSizeInfo;
typedef struct{	
	//avdNativeUInt					gbSizeX;
	//avdNativeUInt					gbSizeY;


	avdNativeInt					block_y;
	avdNativeInt					block_x;
	avdNativeInt					pix_y;
	avdNativeInt					pix_x;
	avdNativeInt					pix_c_y;
	avdNativeInt					pix_c_x;
	avdNativeInt					subblock_x;
	avdNativeInt					subblock_y;

}TPosInfo;



typedef struct{
	avdNativeUInt					is_v_block;
	avdNativeUInt					model_number;
	avdNativeInt					last_dquant;
	AVDMotionVector				mvd[2][BLOCK_MULTIPLE][BLOCK_MULTIPLE];      //!< indices correspond to [forw,backw][block_y][block_x][x,y]
}TCABACInfo;

typedef struct{
	// array for clipping;
	avdUInt8						*clip255;
	avdInt16						*clipHeight;
	avdInt16						*clipHeightCr;
	avdInt16						*clipHalfHeightCr;
	avdInt16						*clipWidth;
	avdInt16						*clipWidthCr;
	avdNativeInt					clipArrHeight;
	avdUInt8						*clipQP51;
}TCLIPInfo;


#define MAX_CORE_NUM 2


#define MAX_SLICE_NUM    32


#define mbSetCBPHasLuma(mbBits,val)		((val)?((mbBits)->iTransFlags |= 0x01000000):((mbBits)->iTransFlags &= 0xfeffffff))
#define mbCBPHasLuma(mbBits)		((mbBits)->iTransFlags & 0x01000000)
#define mbSetCBPHasChroma(mbBits,val)		((val)?((mbBits)->iTransFlags |= 0x02000000):((mbBits)->iTransFlags &= 0xfdffffff))
#define mbCBPHasChroma(mbBits)		((mbBits)->iTransFlags & 0x02000000)
#define mbSetCPredMode(mbBits,val)		((val)?((mbBits)->iTransFlags |= (val)<<26):((mbBits)->iTransFlags &= 0xf3ffffff))
#define mbGetCPredMode(mbBits)		(((mbBits)->iTransFlags & 0x0c000000)>>26)
#define mbSetI16PredMode(mbBits,val)		((val)?((mbBits)->iTransFlags |= (val)<<28):((mbBits)->iTransFlags &= 0xcfffffff))
#define mbGetI16PredMode(mbBits)		(((mbBits)->iTransFlags & 0x30000000)>>28)
#define mbResetItransFlag(mbBits)  //((mbBits)->iTransFlags &=0xff000000)
#define mbResetMBBSFlag(mbBits) ((mbBits)->iTransFlags=0)

typedef struct _TMBsParser{
	
	// do not add data infront of numRuns[28], we need it to be 8 bytes aligned;
	//TMBBitStream*					mbBitStream;		
	//AVDIPDeblocker					*ipDeblocker;
	avdUInt8						*numRuns;//[28]; // 28=NUMBER_RUN_SIZE, to determine neighbor number of runs
	avdUInt8						*mpr;//8 bytes aligned

	avdInt16						*cof; // cof[24*16]; avdInt16	cof[24][4][4];make sure it is 8byte aligned for WMMX<! correction coefficients from predicted
	//avdInt16						*rowCof; // for entire MBRow;
#if FEATURE_INTERLACE
	//TMBBitStream*					mbBitStreamBottom;
	//avdInt16						*rowCofBottom;
	//AVDIPDeblocker					*ipDeblockerBottom;
#endif//FEATURE_INTERLACE
	//TMBBitStream*					mbBitStreamActivate;
	//avdInt16						*rowCofActivate;
	//AVDIPDeblocker					*ipDeblockerActivate;
	avdUInt32						cbpBits; // store all zeros flags of NUMBER_COEFF_BLOCK coeff blocks;
	avdUInt32						cbp;
	avdNativeUInt					allrefzero;
	TPosInfo						posInfo;
	//avdUInt8						mpr[MB_BLOCK_SIZE][MB_BLOCK_SIZE]; //we can use m7[MPR_16]
#define MPR_ROW 21 
	avdInt32						m7[AVD_SCRATCH_HEIGHT][AVD_SCRATCH_WIDTH];        //<! final 4x4 block. Extended to 16x16 for ABT	
}TMBsParser;
enum{
	ETSDB_STOP	 = 0,
	ETSDB_START	 = 1,
	ETSDB_EXIT	 = 2,	
};

enum{

	DEBLOCK_BY_MB	= 0,
	DEBLOCK_BY_ROW	= 1,
};
typedef struct img_par
{
	H264VdLibParam					*vdLibPar;
	TFuncInfo						*funcInfo;
	TSpecitialInfo					*specialInfo;
	TPOCInfo						*pocInfo;
	TFMOInfo						*fmoInfo;
	TIOInfo							*ioInfo;
	TSizeInfo						*sizeInfo;
	TCABACInfo						*cabacInfo;
	TCLIPInfo						*clipInfo;
	
	
	avdNativeInt					cod_counter;
	// slice data;
	Slice							*currentSlice;                   //<! pointer to current Slice data struct
	avdNativeUInt					current_slice_nr;
	Slice*							SliceList[SLICELIST_NUM];
	avdNativeUInt					type;                                   //<! image type INTER/INTRA
	avdNativeUInt					deblockType;
	// for neighbor;
#ifdef USE_JOBPOOL
	TMBsProcessor					**mbsProcessor;
	TMBsProcessor				   *ori_mbsProcessor[MAX_CORE_NUM];
       TMCoreThreadParam                          *mcoreThreadParam[MAX_CORE_NUM];
#else	
	TMBsProcessor					*mbsProcessor[MAX_CORE_NUM];
#endif

	avdNativeInt					current_mb_nr; // bitstream order
	avdNativeUInt					qp;                                     //<! quant for the current frame
	avdUInt16						*dequant_coef_shift;
	avdUInt16						*dequant_coef_shift8;
	int								initMatrixDequantDone;

	// NOTICE: need to be 8 bytes aligned for WMMX;
	// global;
	struct avdSPS					**SeqParSet;
	struct avdPPS					**PicParSet;
	pic_parameter_set_rbsp_t		*active_pps;
	seq_parameter_set_rbsp_t		*active_sps;
	struct avdDPB					*dpb;
	struct storable_picture			*dec_picture;
	struct storable_picture			*prevOutPicture;//this is used for storing some poc related info, like user data
	avdNativeInt					Transform8x8Mode;

	struct storable_picture			**listX[AVD_MOTION_LIST_SIZE];
	avdNativeUInt					listXsize[AVD_MOTION_LIST_SIZE];
	avdNativeUInt					maxListSize;
	avdNativeUInt					num_ref_idx_l0_active;             //!< number of forward reference
	avdNativeUInt					num_ref_idx_l1_active;             //!< number of backward reference
	// for input buffer;
	//<! from app
	avdNativeUInt					apply_weights;
	TWeightInfo						*weightInfo;
	int								error;
	int								initDone;
	TLicenseCheck2					*licenseCheck;
	
	//WMMX it may cause mismatch between core lib and lib,Number Huang 20081201
	//the following two params are for notSaveAllMV to save memory
	int								notSaveAllMV;
	avdUInt8*						mvRefIdxBuf;
	//place the thread param at the end to make it safer, in order not to be rewritten by overflow.
	
#if 1//FEATURE_INTERLACE
	avdNativeUInt					field_pic_flag;
	avdNativeInt					MbaffFrameFlag;// For MB level frame/field coding	
	avdUInt8*						fieldBitFlags;//size = (picSizeOfMBS)
#endif//FEATURE_INTERLACE
	
#if 1//def WMMX
	avdInt16						*itransTemCoeff;
#endif//WMMX
	//do not add field below TRACE_MEM for consistent to core lib
#ifdef TRACE_MEM //keep it always in the end of the structure
#define MAX_MEM_ITEM 2048
#define PADDING_VALUE 0xfd
#define PADDING_LEN 4

#define		VOMR_LEAK	1
#define 	VOMR_MULTIPLE_FREE	2
#define 	VOMR_OVERWRITE		4


	VOMemTableItem voMemTable[MAX_MEM_ITEM];
	int ItemSize;
#else//TRACE_MEM
#define PADDING_LEN 0
#endif//TRACE_MEM
#if USE_GLOBALE_MEM
	GMemData gMem;
	GMemData gMem2;
#endif//USE_GLOBAL_MEM
#if TEST_TIME
	TProfile profile;
#endif//TEST_TIME
#ifdef USE_JOBPOOL
       avdInt32    frame_job_pos;
       avdInt32    for_unittest;
       avdInt32    job_cnt;
	   avdInt32    slice_start_mby;
	   avdInt32    slice_start_mbx;
	   avdInt32    next_slice_nr;
	   avdInt32    max_jobs;
	   avdInt32    is_cores_running;
	   avdInt32    total_job;
	   avdInt32    end_of_frame;
	   avdInt32    need_restart_of_frame;
	   avdUInt32  statea; 
	   avdUInt32  stateb;
	   avdUInt32  statec; 
	   avdUInt32  stated;
	   avdUInt32  end_of_core;
	   avdUInt32  err_end_of_core;
	   avdUInt32  restart_end_of_core;
	   avdInt32   slice_max_jobs;

	   avdUInt32  slices_flag;
	   avdInt32    curr_slice_jobs[MAX_SLICE_NUM];
	   avdInt32    min_slice_jobs[MAX_SLICE_NUM];
	   avdInt32    max_slice_jobs[MAX_SLICE_NUM];
	   struct img_par *curr_slice_img[MAX_SLICE_NUM];
	   int		   main_slice_job_pos;
	   int		   main_slice_pos;
	   int		   main_got_a_slice;
	   int         main_job_cnt;
	   int         is_new_pic;
	   int         need_clr_core;
	   avdInt32   last_job_flag[MAX_CORE_NUM];
	   avdNativeUInt	mirror_listXsize[MAX_SLICE_NUM][AVD_MOTION_LIST_SIZE];
	   struct StorablePicture **mirror_listX[MAX_SLICE_NUM][AVD_MOTION_LIST_SIZE];
	   struct StorablePicture ****m_listX;
	   
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
//       TMEvent JobEvent;
       TMEvent EndOfFrameEvent;
	 pthread_mutex_t JobMutex;
#endif
#if defined(WIN32) || defined(WINCE)
       HANDLE      JobMutex;
       HANDLE      JobEvent;
	   HANDLE      EndOfFrameEvent;
#endif
#endif	
#if 	CALC_FPS
       unsigned long  total_time;
       int            total_count;
	double         *pfps;
	int            *pgap;
	int            *ptype;
	 int            total_gap;
	int             WaitDecTimes;   
#endif
#if  CALC_THUMBNAIL
       int             decode_ms;
       int             parse_ms;
#endif
} ImageParameters;


#define mbIsMbField2(pos) (img->fieldBitFlags[pos])
#define mbSetMbFiled2(pos,value) (img->fieldBitFlags[pos]=(value))

#if FEATURE_MATRIXQUANT
typedef avdUInt8 TDequantArray[6][4][4];
typedef avdUInt16 TDequantArray_coef[3][6][4][4];
typedef avdUInt8  TDequant8Array[6][8][8];
typedef avdUInt16 TDequant8Array_coef[3][6][8][8];
#endif//#if FEATURE_MATRIXQUANT
#ifdef WMMX
#define H264D_IPP_DEBLOCK_PATCH
#endif//WMMX
#define YUV_BUF_ID 20081112
//extern ImageParameters *img;
// signal to noice ratio parameters
struct snr_par
{
  float snr_y;                                 //<! current Y SNR
  float snr_u;                                 //<! current U SNR
  float snr_v;                                 //<! current V SNR
  float snr_y1;                                //<! SNR Y(dB) first frame
  float snr_u1;                                //<! SNR U(dB) first frame
  float snr_v1;                                //<! SNR V(dB) first frame
  float snr_ya;                                //<! Average SNR Y(dB) remaining frames
  float snr_ua;                                //<! Average SNR U(dB) remaining frames
  float snr_va;                                //<! Average SNR V(dB) remaining frames
};


typedef struct pix_pos
{
  avdNativeInt		x;
  avdNativeInt		y;
  avdNativeInt		pos_x;
  avdNativeInt		pos_y;
  avdNativeInt		mb_addr;
  avdUInt8			available;
} PixelPos;

#if TRACE
FILE *p_trace;
#endif

//TBD: remove all struct img_par *img, struct inp_par *inp; also go through all files again;
// prototypes
//void init_conf(struct inp_par *inp, char *config_filename);

void malloc_slice(ImageParameters *img);
void free_slice(ImageParameters *img,Slice* currSLice);

avdNativeInt  decode_one_frame(ImageParameters *img,struct snr_par *snr);
avdNativeInt  read_new_slice(ImageParameters *img,struct avdNALU *nalu);
avdNativeInt  read_one_macroblock(ImageParameters *img,TMBsProcessor *info);
avdNativeInt  decode_one_macroblock(ImageParameters *img,TMBsProcessor *info);
avdNativeInt  exit_macroblock(ImageParameters *img,TMBsProcessor *info,avdNativeInt eos_bit);
avdNativeInt  intrapred(ImageParameters *img,TMBsProcessor *info,avdNativeInt img_x,avdNativeInt img_y);
avdNativeInt  intrapred_luma_16x16(ImageParameters *img,TMBsProcessor *info,avdNativeInt predmode);
avdNativeInt  sign(avdNativeInt a , avdNativeInt b);

void init_frame(ImageParameters *img);
void uninit_frame(ImageParameters *img);
int decode_one_slice(ImageParameters *img);
void set_ref_pic_num(ImageParameters *img);
void exit_slice(ImageParameters *img);
void start_macroblock(ImageParameters *img,TMBsProcessor *info);
void read_ipred_modes(ImageParameters *img,TMBsProcessor *info);
void decode_ipcm_mb(ImageParameters *img,TMBsProcessor *info);
void readMotionInfoFromNAL (ImageParameters *img,TMBsProcessor *info);
void readCBPandCoeffsFromNAL(ImageParameters *img,TMBsProcessor *info);
void readIPCMcoeffsFromNAL(ImageParameters *img,TMBsProcessor *info,struct datapartition *dP);
void init_decoding_engine_IPCM(ImageParameters *img);
void readIPCMBytes_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *sym, Bitstream *currStream);

void copyblock_sp(ImageParameters *img,TMBsProcessor *info,avdNativeInt block_x,avdNativeInt block_y);
void itrans_old(ImageParameters *img,TMBsProcessor *info,avdNativeInt ioff, avdNativeInt joff, avdNativeInt i0, avdNativeInt j0);
void itrans_sp_chroma(ImageParameters *img,TMBsProcessor *info,avdNativeInt ll);
void itrans(ImageParameters *img,TMBsProcessor *info,avdUInt8 *outStart,avdInt16 *coef, avdNativeInt outLen);
void itrans_LeftHalf(ImageParameters *img,TMBsProcessor *info,avdUInt8 *outStart,avdInt16 *coef, avdNativeInt outLen);
void itrans_sp(ImageParameters *img,TMBsProcessor *info,avdNativeInt ioff,avdNativeInt joff,avdNativeInt i0, avdNativeInt j0);
void itrans_2(ImageParameters *img,TMBsProcessor *info);
void itrans8x8(ImageParameters *img,TMBsProcessor *info,avdUInt8 *outStart, avdInt16 *coef, avdNativeInt outLnLength);
void intrapred_chroma_uv(ImageParameters *img,TMBsProcessor *info);
#ifdef TRACE_MEM
void trace_new(ImageParameters *img,int id,int address,int size,int offset);
void trace_delete(ImageParameters *img, int address);
void trace_overflow(ImageParameters *img);
void trace_memIssues(ImageParameters *img);
#else
#define trace_new(img,id,address,size,offset)
#define trace_delete(img,address)
#define trace_overflow(img)
#define trace_memIssues(img)
#endif
// SLICE function pointers
extern avdNativeInt  (*nal_startcode_follows) (ImageParameters *img,TMBsProcessor* info,avdNativeInt eos_bit);
// NAL functions TML/CABAC bitstream
avdNativeInt  uvlc_startcode_follows(ImageParameters *img,TMBsProcessor* info,avdNativeInt eos_bit);
avdNativeInt  cabac_startcode_follows(ImageParameters *img,TMBsProcessor* info,avdNativeInt eos_bit);

void free_Partition(ImageParameters *img,Bitstream *currStream);

// ErrorConcealment
void reset_ec_flags(ImageParameters *img);

void avd_error(ImageParameters *img,char *text, int code);
int  is_new_picture(ImageParameters *img);
void init_old_slice(ImageParameters *img);


// dynamic mem allocation
avdInt32  init_global_buffers(ImageParameters *img);
void free_global_buffers(ImageParameters *img);

void frame_postprocessing(ImageParameters *img);
void field_postprocessing(ImageParameters *img);
void initArray4AllMacroblock(ImageParameters *img);
void decode_slice(ImageParameters *img,avdNativeInt current_header);

#define PAYLOAD_TYPE_IDERP 8
//avdInt32 RBSPtoSODB(ImageParameters *img,avdUInt8 *streamBuffer, avdInt32 last_byte_pos);
avdInt32 RBSPtoSODB(avdUInt8 *streamBuffer, avdInt32 last_byte_pos);
avdInt32 EBSPtoRBSP(ImageParameters *img,avdUInt8 *streamBuffer, avdInt32 end_bytepos, avdInt32 begin_bytepos);

// For MB level frame/field coding
//void init_super_macroblock(ImageParameters *img,TMBsProcessor *info);
//void exit_super_macroblock(ImageParameters *img,TMBsProcessor *info);
//void decode_one_Copy_topMB(ImageParameters *img,TMBsProcessor *info);

//void SetOneRefMV();
void fill_wp_params(ImageParameters *img);

void FreePartition (ImageParameters *img,DataPartition *dp, avdNativeInt n);
DataPartition *AllocPartition(ImageParameters *img,int n);
void OutPutCallBack(ImageParameters *img);
void DpbBaselinePlusBFrMBRowPostProc(ImageParameters *img);
void uninit_frame2(ImageParameters *img);
void InitRefBuffer(ImageParameters *img);
VoiH264VdReturnCode deInitDec(ImageParameters *img);
VoiH264VdReturnCode initDec(ImageParameters *img);
void preprocess_one_frame(ImageParameters *img);
void preprocess_one_pic(ImageParameters *img);
void process_one_slice(ImageParameters *img,struct avdNALU *nalu);
void postprocess_one_pic(ImageParameters *img);
void postprocess_one_frame(ImageParameters *img);
void InPlaceDeblockMBs(ImageParameters *img,TMBsProcessor *info);
void InitClipArrs(ImageParameters *img);
void *voH264AlignedMalloc(ImageParameters *img,int id, int size);
void voH264AlignedFree (ImageParameters *img,void *alignedPt);
void InitBitStream(Bitstream *str, avdUInt8 *buf, avdInt32 len);
void readRunLevelArray_CABAC (ImageParameters *img,TMBsProcessor *info,SyntaxElement *se, Bitstream *bs, avdNativeInt *level, avdNativeInt *runarr, avdNativeInt *numCoeff);
void fill_frame_num_gap(ImageParameters *img);
void UpdateSize(ImageParameters *img,seq_parameter_set_rbsp_t *sps);

void GetBlockLumaNxN(ImageParameters *img,TMBsProcessor *info,avdNativeInt x_pos, avdNativeInt y_pos, struct storable_picture* refPic, avdUInt8 *outStart, avdNativeInt outLnLength,int gbSizeX,int gbSizeY);
void GetBlockChromaNxN(ImageParameters *img,TMBsProcessor *info,avdNativeInt xOffset, avdNativeInt yOffset, struct storable_picture* refPic, int uv,avdUInt8 *byteOutStart, avdNativeInt outLnLength, avdInt16 *clpHt,int gbSizeX,int gbSizeY);
void SILumaAvgBlock(ImageParameters *img,TMBsProcessor *info,avdNativeInt xIdx, avdNativeInt yIdx, avdUInt8 *tmp_block,int gbSizeX,int gbSizeY);
void SIChromaAvgBlock(ImageParameters *img,TMBsProcessor *info,avdNativeInt xTmp, avdNativeInt yTmp, avdUInt8 **imgUV2D, avdUInt8 *tmp_block,int gbSizeX,int gbSizeY);
void DeblockFrame(ImageParameters *img) ;

#ifdef VOI_INPLACE_DEBLOCKING
void SetInPlaceIntraMBStrength(ImageParameters *img,TMBsProcessor *info,avdInt8 * strength);
void InitInPlaceInterMBStrength(ImageParameters *img,TMBsProcessor *info);
void SetInPlaceInterMBStrength(ImageParameters *img,TMBsProcessor *info,avdInt8 * strength,Macroblock *mb);
void InPlaceDeblockMBs(ImageParameters *img,TMBsProcessor *info);
#endif //VOI_INPLACE_DEBLOCKING

#if SUPPORT_SOS
void InitSliceList(ImageParameters *img);
void DeInitSliceList(ImageParameters *img);
#endif //SUPPORT_SOS
typedef struct{
	int heapType;
	int currProfile;
	H264VdLibParam*  params;	
	ImageParameters* img;	
}GlobalH264Data;

#define PREALLOCATE_BUF 20100114
#if 1
#define  AVD_ALIGNED32_MEMSET(pt, val, size )\
{ \
	avdUInt32* dst2 = (avdUInt32*)(pt); \
	avdNativeInt	size32 = (size); \
	avdUInt32		val32  = (val); \
	while (size32-->0)\
	{\
		*dst2++ = val32;\
	}\
}
#define  AVD_ALIGNED32_MEMCPY(dst, src, size )\
{ \
	avdUInt32* dst2 = (avdUInt32*)(dst); \
	avdUInt32* src2 = (avdUInt32*)(src); \
	avdNativeInt	size32 = (size); \
	while (size32-->0)\
	{\
		*dst2++ = *src2++;\
	}\
}
#define  AVD_ALIGNED32_MEMCPY4(dst, src)\
{ \
	avdUInt32* dst2 = (avdUInt32*)(dst); \
	avdUInt32* src2 = (avdUInt32*)(src); \
	*dst2++ = *src2++;\
	*dst2++ = *src2++;\
	*dst2++ = *src2++;\
	*dst2 = *src2;\
}
#define  AVD_ALIGNED32_MEMCPY2(dst, src)\
{ \
	avdUInt32* dst2 = (avdUInt32*)(dst); \
	avdUInt32* src2 = (avdUInt32*)(src); \
	*dst2++ = *src2++;\
	*dst2 = *src2;\
}
#else
void __inline	AVD_ALIGNED32_MEMSET(void* pt, avdUInt32	 val32, avdNativeInt size32 )
{ 
	avdUInt32* dst2 = (avdUInt32*)(pt); 
	while (size32-->0)
	{
		*dst2++ = val32;
	}
}
void __inline	AVD_ALIGNED32_MEMCPY(void* dst, void* src, avdNativeInt size32 )
{ 
	avdUInt32* dst2 = (avdUInt32*)(dst); 
	avdUInt32* src2 = (avdUInt32*)(src); 
	while (size32-->0)
	{
		*dst2++ = *src2++;
	}
}
#endif
#if 1
// good for Big Endian only;
#define  AVD_SAVE_4CHARS(outStart, tmpa, tmpb, tmpc, tmpd) \
			*(avdUInt32 *)outStart = (tmpa | (tmpb<<8) | (tmpc<<16) | (tmpd<<24)); \
			outStart += 4;
#else // 1
#define  AVD_SAVE_4CHARS(outStart, tmpa, tmpb, tmpc, tmpd) \
			*outStart++ = tmpa; \
			*outStart++ = tmpb; \
			*outStart++ = tmpc; \
			*outStart++ = tmpd;
#endif //1
#ifndef ARM
#ifdef _DEBUG	//detect memory leak
#define _CRTDBG_MAP_ALLOC
#include		<stdlib.h>
#ifndef LINUX
#include		<crtdbg.h>
#endif
#endif//_DEBUG
#endif//ARM

//modified by gtxia 2007-10-23
//#define IMPOSSIBLE_POINTER_UPRANGE  0x0000ffff
#ifndef _WIN32_WCE
#ifdef LINUX
//#define IS_VALID_POINTER(pointer) ((int)(pointer)>0x0000ffff)
#define IS_VALID_POINTER(pointer) ((avdUInt32)(pointer)>0x0000ffff)
#else//LINUX
#define IS_VALID_POINTER(pointer) ((int)(pointer)>0x0)
#endif//LINUX
#else//RVDS
#define IS_VALID_POINTER(pointer) ((int)(pointer)>0x0000ffff)
#endif//RVDS
# if defined(_WIN32) || defined(WIN32) || defined(_WIN32_WCE)
   void __inline CHECK_POINTER(ImageParameters *img,TMBsProcessor *info,void* pointer,char* message)
   {
	  if(!IS_VALID_POINTER(pointer))
	  {
		  char tmpBuf[256];
		  sprintf(tmpBuf,"%X,%s",(pointer),(message));
		  AVD_ERROR_CHECK2(img,tmpBuf,ERROR_NULLPOINT);
	  }
   }
# else 
#   define  CHECK_POINTER(img,pointer,message) \
	{ \
	if(!IS_VALID_POINTER((pointer))) {\
		AvdLog2(LL_INFO,"Invalid Pointer:%X,%s\n",(pointer),(message));\
		AVD_ERROR_CHECK2(img,(message),ERROR_NULLPOINT);}\
	}
#  endif
/*!
* The calling convention is __cdecl	 	
*/

#define INSAMESLICE(mb,info) ((info)&&(mb)?mbGetSliceNR(mb)==mbGetSliceNR(info):0)  //for MBAFF dual core, this is not safe 2011-08-18 

#ifdef __cplusplus//1.0 API
extern "C" {
#endif
	/*!
	* initialize decoder and hook up user-defined input parameters
	* This function should be called first.
	* \param inParam [in/out] user-defined input parameters;users modify data in input section of param to communicate with decoder; users getdata from output section of param.   
	* \retval use H264VDLIB_SUUCCEEDED(retCode)to denote whether the call is successful
	*/
	VoiH264VdReturnCode			CALL_API H264VdLibStartDecode(ImageParameters *img,H264VdLibParam *inParam);
	/*!
	* notify decoder to receive data and decode one frame if there is enough data in the buffer 
	* \retval use H264VDLIB_SUUCCEEDED(retCode)to denote whether the call is successful
	*/
	VoiH264VdReturnCode			CALL_API H264ReceiveData(ImageParameters *img);
	/*!
	* clean up decoder;
	* \retval use H264VDLIB_SUUCCEEDED(retCode)to denote whether the call is successful
	*/
	VoiH264VdReturnCode			CALL_API H264VdLibEndDecode(ImageParameters *img);
	/*!
	* notify decoder to flush the decoded frame in buffer;
	* \retval use H264VDLIB_SUUCCEEDED(retCode)to denote whether the call is successful
	*/
	VoiH264VdReturnCode			CALL_API H264Flushing(ImageParameters *img);
	/*!
	* utility function to get the size info of the stream
	* \param buf	the buffer containing the size info
	* \param bufSize the buffer size
	* \param outProbeResult [OUT] the result of the size info
	* \retval use H264VDLIB_SUUCCEEDED(retCode)to denote whether the call is successful
	*/
	VoiH264VdReturnCode			CALL_API H264ProbeStream(char *buf, int bufSize,H264SizeInfo *outProbeResult);
#ifdef __cplusplus
}


#endif
void IMX31_AddOneFrame(ImageParameters *img,unsigned char* virtualBuf,unsigned long phyBuf);
void ARM11_MX31Deblock(ImageParameters *img);
void ARM11_MX31DeblockUnInit(ImageParameters *img);
int ARM11_MX31DeblockInit(ImageParameters *img);
void ARM11_MX31SetDBStrength(ImageParameters *img,int rowcount);

void interpret_mb_mode_B(ImageParameters *img,TMBsProcessor *info);
void interpret_mb_mode_P(ImageParameters *img,TMBsProcessor *info);
void interpret_mb_mode_I(ImageParameters *img,TMBsProcessor *info);
avdNativeInt decode_one_macroblock_b_slice(ImageParameters *img,TMBsProcessor *info);
avdNativeInt decode_one_macroblock_p_slice(ImageParameters *img,TMBsProcessor *info);
avdNativeInt decode_one_macroblock(ImageParameters *img,TMBsProcessor *info);
void read_motion_info_from_NAL_b_slice_CAVLC (ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
void read_motion_info_from_NAL_pb_slice_CAVLC (ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
void read_motion_info_from_NAL_b_slice_CABAC (ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
void read_motion_info_from_NAL_pb_slice_CABAC (ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
void read_CBP_and_coeffs_from_NAL_CABAC(ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
void read_CBP_and_coeffs_from_NAL_CAVLC(ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
avdNativeInt read_one_macroblock_pb_slice_CAVLC(ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
avdNativeInt read_one_macroblock_i_slice_CAVLC(ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
avdNativeInt read_one_macroblock_pb_slice_CABAC(ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
avdNativeInt read_one_macroblock_i_slice_CABAC(ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
void read_ipred_modes_CAVLC(ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
void read_ipred_modes_CABAC(ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
void readIPCMcoeffsFromNAL_CABAC(ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
void readIPCMcoeffsFromNAL_CAVLC(ImageParameters *img,TMBsProcessor *info,Bitstream* bs);
int		Alloc_Neighbor_Info(ImageParameters *img);
int		Free_Neighbor_Info(ImageParameters *img);
int		Init_Slice_Neighbor_Info(ImageParameters *img);
int		Init_MB_Neighbor_Info(ImageParameters *img,TMBsProcessor *info,int isParsing);
int		Finish_MB_Neighbor_Info(ImageParameters *img,TMBsProcessor *info);
int		Init_Coding_Picture_MB_Data(ImageParameters *img,TMBsProcessor *info);
int		Init_MB_Neighbor_InfoMBAff(ImageParameters *img,TMBsProcessor *info, int isParsing);

void InitNewPicProcessor(ImageParameters *img);
void ExitNewPicProcessor(ImageParameters *img);
void InitNewSliceProcessor(ImageParameters *img);
void ExitNewSliceProcessor(ImageParameters *img);
TMBsProcessor* GetMainProcessor(ImageParameters *img);
void SetProcessorStatus(ImageParameters *img,TMBsProcessor* info,EMPStatus status);
EMPStatus GetProcessorStatus(TMBsProcessor* info);
void	SetMainProcessor(ImageParameters *img,TMBsProcessor* info);
int		IsReadyRead(ImageParameters *img,TMBsProcessor* info,TMBsProcessor* another);
int		IsReadyDecode(ImageParameters *img,TMBsProcessor* info,TMBsProcessor* another);
void	Wait2(ImageParameters	*img,TMBsProcessor* info,int ms);
int processmbs(ImageParameters *img,TMBsProcessor* info);

#define GetMBsParser(info) ((info)->mbsParser)
#define GetMBBits(MB) (&((MB)->mbBitStream))
#define GetRowCof(MB) (((MB)->rowCof))
#define GetPosInfo(info) (&(info->mbsParser->posInfo))
#define GetPredInfo  GetMBBits
#define GetIPDeblocker(info) (info->ipDeblocker)
//#define GetIPDeblockerBottom(info) (info->mbsParser->ipDeblockerBottom)
#define GetMBCofBuf(info) (info->mbsParser->cof)
#define GetCurPos(info,sizeInfo) ((info)->mb_y*sizeInfo->PicWidthInMbs+(info)->mb_x)
#define GetMBY(info) ((info)->mb_y)
#define GetMBX(info) ((info)->mb_x)
#define CurrMBisTop(info) ((info)->currMBisBottom == 0)
#define GetTopMB(info,x) (info->neighborMBs+(x))
#define GetBottomMB(info,x) (info->neighborMBsBottom+(x))
#define GetPicStructure(img)  ((img)->specialInfo->structure)
#ifdef USE_JOBPOOL 
void CreateDCoreThread(ImageParameters *img,avdNativeInt coreID);
void  ExitDCoreThread(ImageParameters *img,avdNativeInt coreID);
#else
void  CreateDCoreThread(ImageParameters *img,TMBsProcessor* info);
void  ExitDCoreThread(ImageParameters *img,TMBsProcessor* info);
#endif
void  PushOneOutData(ImageParameters *img,OutDataQueueItem* item);
void PopOneOutData(ImageParameters *img,OutDataQueueItem* item);
int IsThereOtherOutput(ImageParameters *img);
void CheckRef(ImageParameters *img,int listoffset,int idx);
#define GetCurTmpBuf(info) 	((info)->mbsParser->m7)
#define GetTmpBuf0(img) 	((img)->mbsProcessor[0]->mbsParser->m7)
#define GetTmpBuf1(img) 	((img)->mbsProcessor[1]->mbsParser->m7)
#define COF_SIZE (24*16)
#define iClip3(low,high,x) (min(max(x,low),high))
#if 1
#define iClip255(x) clip255[(x)]
#else
static int __inline iClip255(int x) 
{
	return iClip3(0,255,x);
}
#endif
int voH264IntDiv2(int numerator, int denominator);
extern const int SubWidthC[4];
extern const int SubHeightC[4];
#if FEATURE_INTERLACE
int ExitOneMBAff(ImageParameters *img,TMBsProcessor *info);
#else//FEATURE_INTERLACE
#define ExitOneMBAff(a,b)
#endif//FEATURE_INTERLACE
//#define DUMP_DCORE (info->mb_y&1),
#define DUMP_DCORE ((info->mb_y&3)<2),
#endif//global.h

















