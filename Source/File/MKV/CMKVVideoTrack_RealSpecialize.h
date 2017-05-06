#pragma once

#include "CMKVVideoTrack.h"
#include "MKVInfoStructure.h"
#include "CMKVParser.h"
#ifdef _RAW_DUMP_
#include "CDumper.h"
#endif
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define HX_RVTRVIDEO_ID				0x52565452  /* 'RVTR' (for rv20 codec) */
#define HX_RVTR_RV30_ID				0x52565432  /* 'RVT2' (for rv30 codec) */
#define HX_RV20VIDEO_ID				0x52563230  /* 'RV20' */
#define HX_RV30VIDEO_ID				0x52563330  /* 'RV30' */
#define HX_RV40VIDEO_ID				0x52563430  /* 'RV40' */
#define HX_RVG2VIDEO_ID				0x52564732  /* 'RVG2' (raw TCK format) */
#define HX_RV89COMBO_ID				0x54524F4D  /* 'TROM' (raw TCK format) */

typedef struct
{
	unsigned long   ulLength;
	unsigned long   ulMOFTag;
	unsigned long   ulSubMOFTag;
	unsigned short   usWidth;
	unsigned short   usHeight;
	unsigned short   usBitCount;
	unsigned short   usPadWidth;
	unsigned short   usPadHeight;
	unsigned long  ufFramesPerSecond;
	unsigned long   ulOpaqueDataSize;
	unsigned char*    pOpaqueData;
} VORV_FORMAT_INFO;

typedef struct
{
	VORV_FORMAT_INFO* format;
} VORV_INIT_PARAM;

typedef struct rv_segment_struct
{
	VO_BOOL		bIsValid;		//always VO_TRUE
	VO_U32		ulOffset;
} rv_segment;

typedef struct rv_frame_struct
{
	VO_U32		ulDataLen;
	VO_PBYTE	pData;
	VO_U32		ulTimestamp;
	VO_U16		usSequenceNum;	//always 0
	VO_U16		usFlags;
	VO_BOOL		bLastPacket;	//always VO_FALSE
	VO_U32		ulNumSegments;
	rv_segment*	pSegment;
} rv_frame;

class CMKVVideoTrack_RealSpecialize : public CMKVVideoTrack
{
public:
	CMKVVideoTrack_RealSpecialize( VO_U8 btStreamNum, VO_U32 dwDuration , TracksTrackEntryStruct * pAudioInfo , CMKVFileReader * pReader, VO_MEM_OPERATOR* pMemOp );
	virtual ~CMKVVideoTrack_RealSpecialize();

	virtual VO_BOOL NewFrameArrived( MKVFrame * pFrame );

	virtual VO_U32 GetSample(VO_SOURCE_SAMPLE* pSample);

	virtual VO_U32 GetHeadData(VO_CODECBUFFER* pHeadData);

	virtual VO_U32 GetCodec(VO_U32* pCodec);
	VO_U32 GetParameter(VO_U32 uID, VO_PTR pParam);
protected:
	VO_VOID AnalyzeOpaqueHeader( VO_PBYTE pData , VO_U32 size );
	VO_U32 CalculateTimeCode( VO_U32 count );
	virtual VO_U32 GetCodecCC(VO_U32* pCC);
	virtual VO_U32 GetThumbNailInfo(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);
	virtual VO_U32 GetThumbNailBuffer(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);
	
	VO_U64 m_currenttimestamp;

	VO_PBYTE m_pInitParam;
	VO_U32 m_dwInitParamSize;
private:
	VO_S32 	m_nMaxEncoderFrameSize;

#ifdef _RAW_DUMP_	
	CDumper	m_Dumper;
#endif
};

#ifdef _VONAMESPACE
}
#endif
