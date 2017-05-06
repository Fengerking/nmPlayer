#ifndef __H264VDDUMP_H__
#define __H264VDDUMP_H__
#include "defines.h"
#define DUMP_SEQUENCE 							(1<<1) // sequence level dump;
#define DUMP_BMP      							(1<<2) // bmp (frame by frame);
#define DUMP_SLICE     							(1<<3) // slice (frame by frame);
#define DUMP_MEMORY								(1<<4) // memory usage;
#define DUMP_DEBLOCK							(1<<5) // result after deblocking;
#define DUMP_BITS								(1<<6) // result after deblocking;
#define DUMP_RUN_TIME							(1<<7) // dump runtime of diff functions;
#define DUMP_TIMESTAMP							(1<<8) // dump runtime of diff functions;
#define DUMP_SLICEDATA							(1<<9)
#define DUMP_ALL										0xffffffff // dump all;
#define DUMP_NONE										0					 // no dump;
// define DUMP version;

#ifdef DEBUG_INFO
#ifdef X86_TEST
#define DUMP_INFO264
#endif//LINUX
#endif//DEBUG_INFO
#if !defined(DUMP_INFO264)
#define DUMP_VERSION	0
#else//NDEBUG
#define DUMP_VERSION	(DUMP_SEQUENCE | DUMP_SLICE  | DUMP_DEBLOCK| DUMP_SLICEDATA)
#endif//NDEBUG
typedef enum
{
	DDT_D8  = 0,
	DDT_D16 = 1,
	DDT_D32 = 2
}DUMP_DataType;
void Dumploop(int dumpflag,struct _TMBsProcessor* info,char* name,int count,void* src,DUMP_DataType type);
void Dumploop2(int dumpflag,struct _TMBsProcessor* info,char* name,int count1,int count2,void* src,DUMP_DataType type,int stride);

#if DUMP_VERSION
extern int				_dumpSkipped;
extern int				_dumpStartFrameNo;
extern int				_dumpEndFrameNo;
#endif //DUMP_VERSION

#if DUMP_VERSION
#include <stdio.h>
#include <stdarg.h>
#if (DUMP_VERSION & DUMP_MEMORY)
extern FILE				*_dmpMemory;
extern int				_totalMemory;
#endif

#if (DUMP_VERSION & DUMP_SEQUENCE)
extern FILE				*_dmpSeqFp;
#endif
#if (DUMP_VERSION & DUMP_TIMESTAMP)
extern FILE				*_dmpTsFp;
#endif
#if (DUMP_VERSION & DUMP_BITS) | (DUMP_VERSION & DUMP_SEQUENCE)
#endif //(DUMP_VERSION & DUMP_BITS) | (DUMP_VERSION & DUMP_SEQUENCE)

#if (DUMP_VERSION & DUMP_BITS)
extern char 			_dmpBitsDir[256];
extern FILE				*_dmpBitsFp;
extern FILE				*_dmpAllBitsFp;
extern int				_dmpTotBits,
						_dmpSliceBits,
						_dmpTotalMBBits,		//!< includes bits not explicitly tracked, such as CBP, deltaQP, etc.
						_dmpMBTypeBits,			//!< MB, subMB types
						_dmpMBModeBits,			//!< intra chroma coding mode, Intra_4x4 mode bits
						_dmpMBIndexBits,		//!< reference indices
						_dmpMBMVDBits,
						_dmpMBResidualBits[3];
extern int				_dmpSliceMBTypeBits,	
						_dmpSliceMBModeBits,	
						_dmpSliceMBIndexBits,	
						_dmpSliceMBMVDBits,
						_dmpSliceMBResidualBits[3];
#endif

#if (DUMP_VERSION & DUMP_BMP)
extern char 			_dmpBmpDir[256];
#endif

#if (DUMP_VERSION )
extern char 			_dmpDblkDir[256];
extern FILE				*_dmpDblkFp[2];
#endif //(DUMP_VERSION & DUMP_SLICE)

#if (DUMP_VERSION & DUMP_SLICE)
extern char 			_dmpSlcDir[256];
extern FILE				*_dmpSlcFp[2];
extern FILE				*_dmpSlcFpData[2];
#endif //(DUMP_VERSION & DUMP_SLICE)

#endif //!BUILD_WITHOUT_C_LIB

#if (DUMP_VERSION & DUMP_SLICE)
extern int				_dumpIP4 [10];
extern int				_dumpIP8 [5];
extern int				_dumpIP16[5];
extern int				_dumpMbMode[12];
extern int				_dumpIP4Tot [10];
extern int				_dumpIP8Tot [5];
extern int				_dumpIP16Tot[5];
extern int				_dumpMbModeTot[12];
extern char				IPredName4x4  [10][32];
extern char				IPredName16x16[5][32];
extern char				IPredName8x8  [5][32];
extern char				MBModeName    [12][32];
extern char				MCDirName     [3][32];
// for itransform type;
extern int				_dumpNumItrans;
extern int				_dumpItransTp		[6];
extern int				_dumpItransTpTot[6];
extern char				ItransTpName    [6][32];
// for itran all zero 4x4 and intra predict;
extern int				_dumpIntra4Y0Cof;
extern int				_dumpIntra4C0Cof;
extern int				_dumpIntra4YNon0Cof;
extern int				_dumpIntra4CNon0Cof;
extern int				_dumpIntra4Y0CofTot;
extern int				_dumpIntra4C0CofTot;
extern int				_dumpIntra4YNon0CofTot;
extern int				_dumpIntra4CNon0CofTot;
#endif //(DUMP_VERSION & DUMP_SLICE)

#if (DUMP_VERSION & DUMP_ALL)


void AvdLog(int logId,int rowIDX, char *format, ...);
#else //(DUMP_VERSION & DUMP_ALL)
#define AvdLog(...)
#endif //(DUMP_VERSION & DUMP_ALL)

#define DCORE_DUMP DUMP_DCORE

#if (DUMP_VERSION & DUMP_RUN_TIME)
#include		<windows.h>
#include		<winbase.h>

typedef enum {
	DT_TOTALTIME			=0,
	// read one block
	DT_READONEMB,
	DT_READMOTION,
	DT_READCBPNCOEFF,
	DT_READCOEFFMEMSET0,
	DT_READCBP,
	DT_READ16x16DC,
	DT_READCAVLC4x4,
	DT_PARSELUMACOEFF,
	DT_PARSELUMACOEFFLOOP,
	DT_READCHROMADC,
	DT_PARSECHROMACOEFFLOOP,
	// decode one block
	DT_DECODEONEMB,
	DT_GETBLOCKLUMA,
	DT_GETBLOCKCHROMA,
	DT_ITRANSLEFTHALF,
	DT_ITRANS,
	DT_INTRAPRED4x4,
	DT_INTRAPRED16x16,
	DT_INTRAPREDCHROMA,
	// postprocessing one frame
	DT_DEBLOCKING,
	DT_STOREPICDPB,
	DT_DPBFRPOSTPROC,
	// bit reading;
	DT_GETBITS,
	DT_SHOWBITS,

	DT_TOTAL_FUNCTIONS
} DumpTimeFunctionID;

extern FILE			*_dmpTime;
extern char			_dmpTimeFileName[256];
extern int			_dmpFunctionStartTime[DT_TOTAL_FUNCTIONS];
extern int			_dmpFunctionTotalTime[DT_TOTAL_FUNCTIONS];

void StartRunTimeClock(int funId);
void EndRunTimeClock(int funId);
void DumpTime();

#else // (DUMP_VERSION & DUMP_RUN_TIME)

#define StartRunTimeClock(funId) // do nothing;
#define EndRunTimeClock(funId) // do nothing;
#define DumpTime()
#endif //(DUMP_VERSION & DUMP_RUN_TIME)



#endif //__H264VDDUMP_H__
