#pragma once

#include "CMKVAudioTrack.h"
#include "MKVInfoStructure.h"
#include "CMKVParser.h"
#ifdef _RAW_DUMP_
#include "CDumper.h"
#endif
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct ra_block_struct
{
	VO_PBYTE	pData;
	VO_U32		ulDataLen;
	VO_U32		ulTimestamp;
	VO_U32		ulDataFlags;
} ra_block;

typedef struct  
{
	unsigned long ulSampleRate;
	unsigned long ulActualRate;
	unsigned short usBitsPerSample;
	unsigned short usNumChannels;
	unsigned short usAudioQuality;
	unsigned short usFlavorIndex;
	unsigned long ulBitsPerFrame;
	unsigned long ulGranularity;
	unsigned long ulOpaqueDataSize;
	unsigned char*  pOpaqueData;
}VORA_FORMAT_INFO;

typedef struct
{
	VORA_FORMAT_INFO *format;
	unsigned long ulFourCC;
	void*  otherParams;//reserved field
	unsigned long otherParamSize;//reserved field
} VORA_INIT_PARAM;

struct SuperBlockBlockDataInfo
{
	VO_PBYTE pData;
	VO_U32 pos;
};

class CMKVAudioTrack_RealSpecialize : public CMKVAudioTrack
{
public:
	CMKVAudioTrack_RealSpecialize( VO_U8 btStreamNum, VO_U32 dwDuration , TracksTrackEntryStruct * pAudioInfo , CMKVFileReader * pReader, VO_MEM_OPERATOR* pMemOp );
	virtual ~CMKVAudioTrack_RealSpecialize();

	virtual VO_BOOL NewFrameArrived( MKVFrame * pFrame );
	virtual VO_VOID NewBlockGroupStarted();
	virtual VO_VOID NewReferenceBlockArrived();

	virtual VO_U32 GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32 GetSample(VO_SOURCE_SAMPLE* pSample);

	virtual VO_VOID CheckCacheAndTransferTempData();

	virtual VO_U32 GetCodecCC(VO_U32* pCC);

	virtual VO_U32 SetPosN(VO_S64* pPos);
	virtual VO_U32 SetPosK(VO_S64* pPos);
	virtual VO_U32 GetSampleN(VO_SOURCE_SAMPLE* pSample);

	virtual VO_VOID Flush();

	virtual VO_VOID SetIsAboutToSeek(){ m_isseeking_nokeyframe = VO_TRUE; };
	virtual VO_U16 GetInterleaveBlockSize() {return m_interleave_block_size;}

private:
	VO_VOID AnalyzeOpaqueHeader( VO_PBYTE pData , VO_U32 size );
	VO_VOID AnalyzeOpaqueHeader_Version4( VO_PBYTE pData , VO_U32 size );
	VO_VOID AnalyzeOpaqueHeader_Version5( VO_PBYTE pData , VO_U32 size );

	VO_BOOL AddNewInterleaveFrame( MKVFrame * pFrame );
	VO_BOOL AddNewSeekInterleaveNoKeyFrame( MKVFrame * pFrame );

	VO_VOID DeInterLeaveBlockData( MKVFrame * pFrame );
	VO_VOID DeInterLeaveBlockDataFromBuffer( VO_PBYTE pData );

	VO_U16 m_interleave_factor;
	VO_U16 m_interleave_block_size;
	VO_U16 m_interleave_frame_size;
	VO_U32 m_interleave_frames_perblock;
	VO_U32 m_interleave_pattern_size;

	VO_U8 m_is_interleave;
	VO_U8 m_has_interleave_pattern;

	VO_U16 * mp_interleave_pattern;

	VO_U32 m_current_superblocksize;

	VO_PBYTE m_pBlockBackUp;
	VO_BOOL m_isseeking_nokeyframe;

	SuperBlockBlockDataInfo * m_ppSuperBlockBlockDataInfo;
	VO_U32 m_current_blockdata_index;
	

	VO_PBYTE			m_pInitParam;
	VO_U32				m_dwInitParamSize;

	VO_U32 m_superblocksincache;

#ifdef _RAW_DUMP_
	CDumper	m_Dumper;
#endif
};

#ifdef _VONAMESPACE
}
#endif
