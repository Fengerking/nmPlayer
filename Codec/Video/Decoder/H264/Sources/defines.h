
/*!
 **************************************************************************
 * \file defines.h
 *
 * \brief
 *    Headerfile containing some useful global definitions
 *
 * \author
 *    Detlev Marpe  
 *    Copyright (C) 2000 HEINRICH HERTZ INSTITUTE All Rights Reserved.
 *
 * \date
 *    21. March 2001
 **************************************************************************
 */

#ifndef _DEFINES_H_
#define _DEFINES_H_


//#define REFERENCE_CODE_COMPLIANT


// Notice: NEW_YUV_MEMORY is about 3% faster in C code;
// If turn on this flag, need to do the followings:
// TBD: redo MMX for (only for Pentium4);
 
#define NEW_YUV_MEMORY
#define NEW_GET_BLOCK

//#define REMOVE_OLD_SLICE_PAR
#define NEW_REF_BUF_MANAGE

#ifdef USE_JOBPOOL
#define Y_PADDING			0 // padding is not faster;
#define XY_PADDING			4 // padding is not faster;
#else
#define Y_PADDING			0 // padding is not faster;
#endif
#define UV_PADDING			(Y_PADDING>>1)

// define Profile
#define VOI_H264D_BASELINE	 						(1) // baseline 
#define VOI_H264D_MAIN      						(1<<1)//main profile;
#define VOI_H264D_EXTENDED     						(1<<2)//extent profile;
#define VOI_H264D_HIGH     						(1<<3)//extent profile;
#define VOI_H264D_ALL								0xffff//16 bits for OMAP

#define VOI_H264D_NON_BASELINE						(VOI_H264D_PROFILE != VOI_H264D_BASELINE)
/*
// selected Platform
#define		AVD_PENTIUM								(1<<1) // for general PC;
#define		AVD_INTEL_XSCALE						(1<<2)
#define     AVD_EVC_ARM9							(1<<3)
#define     AVD_RVDS_ARM9							(1<<4)
//#define		AVD_TI_C55x								(1<<5)
#define		AVD_TI_ARM9								(1<<6)
#define		AVD_EVC_ARM11							(1<<7)
#define		AVD_RVDS_ARM11							(1<<8)
#define		AVD_LINUX_X86							(1<<9)

//#define		AVD_PLATFORM	(0)
// select Platform
#ifdef WIN32
#define		AVD_PLATFORM							AVD_PENTIUM
#endif
#ifdef LINUX_PC
#define		AVD_PLATFORM							AVD_LINUX_X86
#endif//LINUX_PC
#ifdef _MAC_OS
#define		AVD_PLATFORM							AVD_LINUX_X86
#endif//_MAC_OS
#ifdef _IOS
#define		AVD_PLATFORM							AVD_LINUX_X86
#endif//_IOS
#ifdef ARM
#if defined(RVDS)||defined(LINUX)
#define		AVD_PLATFORM							AVD_RVDS_ARM9
#else//RVDS
#define		AVD_PLATFORM							AVD_EVC_ARM9
#endif//RVDS
#endif//ARM

#ifdef ARM11
#define		AVD_PLATFORM							AVD_EVC_ARM11
#define		VOI_MX31_ARM11					1
#define		ARM11_MX31_DEBLOCK				1
#if ARM11_MX31_DEBLOCK
#define		ARM11_MX31_DEBLOCK_USE_PHYMEM		1 //use physical memory
#endif//ARM11_MX31_DEBLOCK
#endif//ARM11
*/
//#define MAIN_PROFILE // for main profile 
//#if !defined(MAIN_PROFILE)

#define FEATURE_WEIGHTEDPRED 1


#ifdef YOUTUBE_HIGH_PROFILE
#define FEATURE_PARTMAP 1
#define FEATURE_T8x8  1
#define FEATURE_CABAC 1
#define FEATURE_BFRAME	1
//#define FEATURE_WEIGHTEDPRED 1
#define FEATURE_MATRIXQUANT  1
#define FEATURE_INTERLACE 1
#endif//YOUTUBE_HIGH_PROFILE



#define  OUTPUT_ASAP 1



#if FEATURE_BFRAME
#define VOI_H264D_PROFILE							VOI_H264D_ALL
#else//MAIN_PROFILE
#define VOI_H264D_PROFILE							VOI_H264D_BASELINE
#endif//MAIN_PROFILE


#ifdef COMBINE_ALL_PROFILE
#define  RENAME_BP  (VOI_H264D_PROFILE==VOI_H264D_BASELINE)
#endif//
#include "ncNameTab.h"
// block this for all versions;
#define VOI_H264D_BLOCK_EC
#define VOI_H264D_BLOCK_RTP


#if FEATURE_BFRAME
#if FEATURE_INTERLACE
	#define		AVD_MOTION_LIST_SIZE			6
#else//FEATURE_INTERLACE
	#define		AVD_MOTION_LIST_SIZE			2
#endif//FEATURE_INTERLACE
	#define		AVD_MOTION_INFO_SIZE			2
#else //FEATURE_BFRAME
	#define		AVD_MOTION_LIST_SIZE			1
	#define		AVD_MOTION_INFO_SIZE			1
#endif //FEATURE_BFRAME
//	#define		VOI_DIRECT_OUTPUT				1//output the decoded frame immediately,which benifit the DirectShow filter
	
#define VOI_H264D_BASELINE_SINGLE_SLICE
 	
#define VOI_INPLACE_DEBLOCKING
#define VOI_H264D_BLOCK_FMO
		
#define SUPPORT_SOS			1


// avdNative(U)Int is the original "int" in the platform; it could be 16 or 32 bits;
// Normally, it represents the fastest data type processed in the platform;
// So if local or non-arrayed data under 16 bits, we would use avdNative(U)Int;
typedef signed     char			avdInt8;                   //!<  8 bit unsigned
typedef unsigned char   avdUInt8;                   //!<  8 bit unsigned
typedef short           avdInt16;
typedef unsigned short  avdUInt16;
typedef int             avdInt32;
typedef unsigned int    avdUInt32;

typedef int				avdNativeInt; // int(16 bits) for OMAP, int(32 bits) for WinTel; 
typedef unsigned int	avdNativeUInt; // int(16 bits) for OMAP, int(32 bits) for WinTel; 
# define INT64_MIN		    (-9223372036854775807i64 - 1i64)
#ifdef _WIN32
		typedef __int64				avdInt64;
		typedef unsigned __int64	avdUInt64;
		
#else
		typedef long long avdInt64;
		typedef unsigned long long	avdUInt64;
		#define __cdecl				// define to nothing;
#endif
	
#define MAX_CODED_FRAME_SIZE	512000 //512k, 8000000         //!< bytes for one frame
	





#ifndef _WIN32
# define BUILD_WITHOUT_C_LIB 1
#endif

//#ifdef _MAC_OS
//# define BUILD_WITHOUT_C_LIB 0
//#endif

#if BUILD_WITHOUT_C_LIB
#define USE_DIVIDE_FUNC 1
#define malloc  voH264Malloc                       
#define calloc  voH264Calloc  
#define realloc voH264Realloc                     
#define free 	voH264Free                          
#define memcpy	voH264Memcpy                    
#define memmove	voH264Memmove                 
#define memset	voH264Memset
#define sprintf		voH264Sprintf
//#define assert(...)
//#define exit(...)
//#define snprintf(...)
//#define qsort	voH264Qsort
//#define longjmp voH264Longjmp
//#define setjmp  voH264Setjmp 
//#define printf  voH264Printf
//#define fprintf voH264Fprintf
//#define assert(...)
//#define exit(...)
//#define snprintf(...)
#define qsort	voH264Qsort
//#define longjmp voH264Longjmp
//#define setjmp  voH264Setjmp 
#define printf  voH264Printf
//#define fprintf voH264Fprintf
//#define fopen   voH264Fopen
//#define fflush  voH264Fflush
//#define fclose  voH264Fclose

//typedef int jmp_buf[64];
#define NULL 0
//#define size_t int
#else
//#define voH264IntDiv(a,b) ((a)/(b))
//#define voH264IntMod(a,b) ((a)%(b))
#endif//BUILD_WITHOUT_C_LIB



//TBD: This is too much memory, change it to link list, or allocated on demand;
#define MAXSPS  32
#define MAXPPS  256 // JM8.3 change from 128 to 256;

#define LIST_0 0
#define LIST_1 1
#define BOTH_LISTS 1

// CAVLC
#define LUMA              0
#define LUMA_INTRA16x16DC 1
#define LUMA_INTRA16x16AC 2

#define TOTRUN_NUM    15
#define RUNBEFORE_NUM  7


//--- block types for CABAC ----
#define LUMA_16DC       0
#define LUMA_16AC       1
#define LUMA_8x8        2
#define LUMA_8x4        3
#define LUMA_4x8        4
#define LUMA_4x4        5
#define CHROMA_DC       6
#define CHROMA_AC       7
#define NUM_BLOCK_TYPES 8


// #define _LEAKYBUCKET_

#define absm(A) ((A)<(0) ? (-(A)):(A))      //!< abs macro, faster than procedure

#define Clip1(a)            ((a)>255?255:((a)<0?0:(a)))
#define Clip3(min,max,val) (((val)<(min))?(min):(((val)>(max))?(max):(val)))
#define PSKIP         0
#define BSKIP_DIRECT  0
#define P16x16       1
#define P16x8        2
#define P8x16        3
#define P8x8    8
#define I4MB    9
#define I16MB   10
#define IBLOCK  11
#define SI4MB   12
#define I8MB 13
#define IPCM    14

// global macro
#define IS_INTRA(MB)    (mbGetMbType(MB)==I4MB  || mbGetMbType(MB)==I16MB ||mbGetMbType(MB)==IPCM||mbGetMbType(MB)==I8MB)
#define IS_INTRA2(MBtype)    (MBtype==I4MB || MBtype==I16MB ||MBtype==I8MB||MBtype==IPCM)

#define IS_4x4_INTRA(MB) (mbGetMbType(MB)==I4MB)
#define IS_4x4_INTRA2(MBType) (MBType==I4MB)
#define IS_8x8_INTRA(MB) (mbGetMbType(MB)==I8MB)
#define IS_8x8_INTRA2(MBType) (MBType==I8MB)
#define IS_16x16_INTRA(MB) (mbGetMbType(MB)==I16MB)
#define IS_16x16_INTRA2(MBType)  (MBType==I16MB)
#define IS_NEWINTRA(MB) (mbGetMbType(MB)==I16MB  || mbGetMbType(MB)==IPCM)
#define IS_NEWINTRA2(MBType) (MBType==I16MB  || MBType==IPCM)

#define IS_IPCM(MB)    (mbGetMbType(MB)==IPCM)
#define IS_IPCM2(MBType)    (MBType==IPCM)
#if 1//ndef VOI_INPLACE_DEBLOCKING
#define Get_MB_Address(Mb) (Mb - img->mb_data)
#define IS_INTRA_MB_ADDRESS(mb_address)  IS_INTRA(&img->mb_data[mb_address])
#define IS_INTRA_MB_ADDRESS2(mbInfo)  IS_INTRA(mbInfo)
#define IS_4x4_INTRA_MB_ADDRESS(mb_address)  IS_4x4_INTRA(&img->mb_data[mb_address])
#if FEATURE_T8x8
#define IS_4x4_INTRA_MB_ADDRESS2(mb_address)  (IS_4x4_INTRA(&img->mb_data[mb_address])||IS_8x8_INTRA(&img->mb_data[mb_address]))
#define IS_4x4_INTRA_MB_ADDRESS3(mbInfo)  (IS_4x4_INTRA(mbInfo)||IS_8x8_INTRA(mbInfo))

#else//FEATURE_T8x8
#define IS_4x4_INTRA_MB_ADDRESS2  IS_4x4_INTRA_MB_ADDRESS
#define IS_4x4_INTRA_MB_ADDRESS3(mbInfo)  (IS_4x4_INTRA(mbInfo))

#endif//FEATURE_T8x8

#endif // VOI_INPLACE_DEBLOCKING

#define IS_INTER(MB)   (mbGetMbType(MB)!=I4MB  && mbGetMbType(MB)!=I16MB  && mbGetMbType(MB)!=IPCM&&mbGetMbType(MB)!=I8MB)
#define IS_INTERMV(MB)  (mbGetMbType(MB)!=I4MB  && mbGetMbType(MB)!=I16MB  && mbGetMbType(MB)!=0 && mbGetMbType(MB)!=IPCM&&mbGetMbType(MB)!=I8MB)
#define IS_COPY(MB)     (mbGetMbType(MB)==0     && (img->type==P_SLICE || img->type==SP_SLICE))
#define IS_P8x8(MB)     (mbGetMbType(MB)==P8x8)

#if FEATURE_INTERLACE
	#define ISFRAMEMB(Mb)				(!mbIsMbField(Mb))//(!img->field_pic_flag && (!cabacInfo->MbaffFrameFlag || !mbIsMbField(Mb)))
	#define GetMbAffFrameFlag(img)		((img->MbaffFrameFlag!=0) ? 1 : 0)
#else//FEATURE_INTERLACE
#define ISFRAMEMB(Mb)				1
#define GetMbAffFrameFlag(obj)		0
#endif//FEATURE_INTERLACE

#if FEATURE_BFRAME
#define IS_DIRECT(MB)				(mbGetMbType(MB)==0     && (img->type==B_SLICE ))
#define IS_B8x8(MB)					(img->type==B_SLICE && mbGetMbType(MB)==P8x8)
#else //FEATURE_BFRAME
#define IS_DIRECT(MB)				0
#define IS_B8x8(MB)					0
#endif //FEATURE_BFRAME
#if FEATURE_CABAC
	#define GetEIFlag(obj)				0//(obj->ei_flag)
	#define IsVLCCoding(pps)			(pps->entropy_coding_mode_flag == UVLC)
#else//FEATURE_CABAC
#define GetEIFlag(obj)				0
#define IsVLCCoding(pps)			1
#endif//FEATURE_CABAC


// Quantization parameter range

#define MIN_QP          0
#define MAX_QP          51

#define BLOCK_SIZE      4
#define MB_BLOCK_SIZE   16
#define BLOCK_SIZE_8x8		8
#define BLOCK_SIZE_CHROMA	2
#define NO_INTRA_PMODE  9        //!< #intra prediction modes
/* 4x4 intra prediction modes */
#define VERT_PRED             0
#define HOR_PRED              1
#define DC_PRED               2
#define DIAG_DOWN_LEFT_PRED   3
#define DIAG_DOWN_RIGHT_PRED  4
#define VERT_RIGHT_PRED       5
#define HOR_DOWN_PRED         6
#define VERT_LEFT_PRED        7
#define HOR_UP_PRED           8

#define PRED_MODE_NOT_VERT_OR_HORUP(predmode)	(predmode & 7)
#define PRED_MODE_IS_DIAGDOWNRIGHT_OR_VERTRIGHT_OR_HOR_DOWN(predmode)	(predmode > DIAG_DOWN_LEFT_PRED && predmode < VERT_LEFT_PRED)
// 16x16 intra prediction modes
#define VERT_PRED_16    0
#define HOR_PRED_16     1
#define DC_PRED_16      2
#define PLANE_16        3

// 8x8 chroma intra prediction modes
#define DC_PRED_8       0
#define HOR_PRED_8      1
#define VERT_PRED_8     2
#define PLANE_8         3

#define SKIPCurSlice  -1			//to support slide of sequence,mark the head of non-Intra slice as SKIPCurSlice
#define EOS             1                       //!< End Of Sequence
#define SOP             2                       //!< Start Of Picture
#define SOS             3                       //!< Start Of Slice

#define DECODING_OK     0
#define SEARCH_SYNC     1
#define PICTURE_DECODED 2

#define MAX_REFERENCE_PICTURES 32               //!< H264 allows 32 fields
#define MAX_REFERENCE_FRAMES  16
#define INVALIDINDEX  (-13579)

#if 1//ndef WIN32
#define max(a, b)      ((a) > (b) ? (a) : (b))  //!< Macro returning max value
#define min(a, b)      ((a) < (b) ? (a) : (b))  //!< Macro returning min value
#endif

#define TMP_BLOCK_SHIFT 4
#define MVPRED_MEDIAN   0
#define MVPRED_L        1
#define MVPRED_U        2
#define MVPRED_UR       3

#define DECODE_COPY_MB				0
#define DECODE_MB					1
#define DECODE_END_OF_SLICE			2
//#define DECODE_MB_BFRAME 2

#define BLOCK_MULTIPLE      (MB_BLOCK_SIZE/BLOCK_SIZE)

//Start code and Emulation Prevention need this to be defined in identical manner at encoder and decoder
#define ZEROBYTES_SHORTSTARTCODE 2 //indicates the number of zero bytes in the short start-code prefix


//for the Address on DSP External Memory (CIF SIZE)
#define PAGE_SIZE              0x10000 //64K

#define ARM_PTR_Address		0x138000
#define Bits_Buffers_Address		0x100000
#define Current_Picture_Address		0x80000	//?153K

//#define Input_Buffer_Address		0x140000	//?64K
//#define MB_Buffer_Address		0x110000	//<32K
//#define Reference_Picture_Address	0x118000	
//#define Global_Buffers_Address		0x120000	//?21K
//#if BUILD_WITHOUT_C_LIB

#define ERROR_RET_CHECK 1

#if ERROR_RET_CHECK
//#define ERROR_RET_VOID	return;
//#define ERROR_RET_INT	return img->error;
#define CHECK_ERR_RET_VOID	if(img->error<0) return;
#define CHECK_ERR_RET_NULL	if(img->error<0) return NULL;
#define CHECK_ERR_RET_INT	if(img->error<0) return img->error;
#define CHECK_ERR_BREAK		if(img->error<0) break;
#define AVD_ERROR_CHECK(a,b,c)	\
	{\
		avd_error(a,b,c);\
		if(img->error<0) return img->error;\
	}

#define AVD_ERROR_CHECK2(a,b,c)	\
	{\
		avd_error(a,b,c);\
		if(img->error<0) return ;\
	}
#define AVD_ERROR_CHECKNULL(a,b,c)	\
	{\
	avd_error(a,b,c);\
	if(img->error<0) return NULL;\
	}

#else//ERROR_RET_CHECK
//#define ERROR_RET_VOID
//#define ERROR_RET_INT	
#define CHECK_ERR_RET_VOID	
#define CHECK_ERR_RET_INT	

#endif//ERROR_RET_CHECK


#define WARN_GENERAL    100
#define ERROR_Resilience	0
#define Is_ERR_Resilience(code) (((signed int)code)>ERROR_Resilience)
#define ERROR_InvalidMV				2
#define ERROR_InvalidOutPOC			 21//force it output	
#define WARN_PREV_FRAME_SLICE_LOST  31
#define ERROR_InvalidPOCType			-1
#define ERROR_InvalidNumRefFrame		-2
#define ERROR_LFDisableIdc			-3
#define ERROR_QP				-4
#define ERROR_SliceType				-5
#define ERROR_IntraMBAIsNULL			-6
#define ERROR_IntraModeIsNULL			-7
#define ERROR_InvalidNumSliceGroupMapUnit	-8
#define ERROR_IDRHasRemainedFrame		-9
#define ERROR_FirstFrameIsNotIntra		10
#define ERRPR_DecPictureIsNotNULL		-11
#define ERROR_InvalidB8Mode			-13
#define ERROR_InvalidIntraPredMode		-14
#define ERROR_InvalidIntraPredCMode		-15
#define ERROR_InvalidRefIndex			-16 
#define ERROR_InvalidMbQPDelta			-17
#define ERROR_InvalidDpbBufSize			-18
#define ERROR_RemappingPicIdc			-19
#define ERROE_InvalidFrameStoreType		-20
#define ERROR_TOOMuchReorderIDC			-22
#define ERROR_InvalidMCO				-23
#define ERROR_InvalidMBMode				-24
#define ERROR_InvalidBitstream			-25 

#define ERROR_DPBIsNULL					-101
#define ERROR_RefBufNotAllocated		-102
#define ERROR_IntraMBBIsNULL			-103
#define ERROR_NotSupportIPCM			-104
#define ERROR_REFLISTGREAT16			-105
#define ERROR_InvalidSEI				-106
#define ERROR_REFFrameIsNULL			-107
#define ERROR_InvalidSEQParSetID		-108
#define ERRPR_DecSliceLoss				-109
#define ERROR_InvalidPicStruct			-110
#define ERROR_NULLPOINT					-111
#define ERROR_TooBigSEISize				-112

#define ERROR_PPSIsNULL					-200
#define ERROR_SPSIsNULL					-201
#define ERROR_InvalidPPS				-202
#define ERROR_InvalidSPS				-203

#define ERROR_MEMExit					-300

#define ERROR_NotSupportProfile			-400
#define ERROR_NotSupportFMO				-401

#define ENABLE_STRICT_MB_START_CHECK  1
#define ENABLE_STRICT_IDR_CHECK  0
#define ENABLE_REUSE_VALID_SPS   0
#define FORCE_USE_STABLE_SPS  0
#ifdef BASELINE_PLUS_B
#define ENABLE_CHECK_REF_FRAMENUM		1
#define ENABLE_PARTICAIL_DECODE  0
#define ENABLE_FIRST_FRAME_NON_INTRA		0
#define ENABLE_STRICT_GAP_CHECK  1

#define STORE_ERROR_FRAME 1

#define ENABLE_SEEK_TO_NEXTI  1

#else//BASELINE_PLUS_B
#define ENABLE_CHECK_REF_FRAMENUM		0
#define ENABLE_PARTICAIL_DECODE  0
#define ENABLE_FIRST_FRAME_NON_INTRA		0
#define ENABLE_STRICT_GAP_CHECK  0
#define ENABLE_SEEK_TO_NEXTI	 1
#endif//BASELINE_PLUS_B

#define OUTPUT_ERROR_FLAG 1121  //this flag indicates that the output is disabled because the frame is not a well decoded
#define SPS_PPS_BEEN_READY 1000
#if ENABLE_TRACE
#define TRACE_ERROR_DETAIL 1
#else 
#define TRACE_ERROR_DETAIL 0
#endif

#define ROBUST_CHECK_REF_BUFFER 1
#define SafevoH264AlignedFree(img,a) \
	{voH264AlignedFree((img),(a));\
	(a) = NULL;}

#include "annexb.h"
#define FORE_CREATE_MOTION_INFO  1
#define FORCE_OUT_WARN_INFO      199
#define VO_INT_MIN  0x80000000

#ifdef VOI_INPLACE_DEBLOCKING //save one row is only available for baseline

//OPT options
#define  OPT_ASSUME_DIRECTTYPE_IS_CONSISTENT 0 //do not assume it
#define  OPT_DISABLE_CHROMA_VECTOR  1
#define COMBINE_4_BYTES_TO32(a,b,c,d) (((a)&0x000000ff)|((b<<8)&0x0000ff00)|((c<<16)&0x00ff0000)|((d<<24)&0xff000000))
#define COMBINE_2_BYTES_TO16(a,b) (((a)&0x000000ff)|((b<<8)&0x0000ff00))
#define COMBINE_2_SHORTS_TO32(a,b) (((a)&0x0000ffff)|((b<<16)&0xffff0000))
#define MUL_MB_DEC 1
#define MUL_ROW_DEBLOCK !MUL_MB_DEC
#define SIMUL_DCORE 0 //for test
#define USE_DUAL_CORE 1
#if USE_DUAL_CORE
//#define INPLACE_DEBLOCK_BY_MB 1
//#define SAVE_ALL_MV	 1
#endif//USE_DUAL_CORE
#define SAVE_ALL_MV		FEATURE_BFRAME 	//save all mv if it has B frame, because of the temporal mode
#ifdef DISABLE_MCORE
#define DISABLE_MT 1
#endif//
#define DISABLE_DECODE 0//this is only for test
#define DISABLE_MC 0 //this is only for test
#define DISABLE_DB 0 //this is only for test
#endif//#ifdef VOI_INPLACE_DEBLOCKING
#if FEATURE_INTERLACE
#if !(defined( _MAC_OS) || defined(_IOS)) && defined(NDK_BUILD)
#define USE_SEINTERLACE 1
#endif//!(defined( _MAC_OS) || defined(_IOS))
#define USE_SEINTERLACE_MBAFF 1
#define USE_FULL_MC 0
#define TEST_DEINTERLACE 0
#endif//FEATURE_INTERLACE
#define TTimeStamp avdUInt64
#define TEST_QCORE 0
#if TEST_QCORE
#define CHECK_THREAD_NEON (info->mb_y&3)==1
//#define WP_ASM 1
#else//TEST_QCORE
#ifdef ARM_ASM


#define MC_L_ASM 1
#define MC_C_ASM 1
#define DB_ASM 1
#if DB_ASM
#define LUMA_DB_ASM 1
#define CHROMA_DB_ASM 1
#endif//DB_ASM
#define ITRANS_ASM 1
#define SIAVG_ASM 1
#define WP_ASM 1
#define NEW_WP_ASM 0
#endif//ARM_ASM
#endif//TEST_QCORE
#endif//DEFINES_H




















