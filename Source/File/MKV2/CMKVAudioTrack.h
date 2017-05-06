#pragma once

#include "CMKVTrack.h"
#include "MKVInfoStructure.h"

struct VORBISFORMAT2 {
	VO_U32 Channels;
	VO_U32 SamplesPerSec;
	VO_U32 BitsPerSample;
	VO_U32 HeaderSize[3]; // Sizes of three init packets
};

enum PCM{
	PCM_BIT_16 = 2,
	PCM_BIT_24 = 3,
	PCM_BIT_32 = 4,
};

class CMKVAudioTrack : public CMKVTrack
{
public:
	CMKVAudioTrack( VO_U8 btStreamNum, VO_U32 dwDuration , TracksTrackEntryStruct * pAudioInfo , VO_MEM_OPERATOR* pMemOp );
	virtual ~CMKVAudioTrack();

	virtual VO_U32 GetHeadData(VO_CODECBUFFER* pHeadData);
	VO_U32 GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32 GetCodecCC(VO_U32* pCC);
	virtual VO_U32 GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);

	VO_VOID Flush();

	//VO_U32 SetPosN(VO_S64* pPos);
/*
	virtual VO_BOOL NewFrameArrived( MKVFrame * pFrame );
	virtual VO_VOID NewClusterStarted();
	virtual VO_VOID NewBlockStarted();
	virtual VO_VOID FileReadEnd();
	virtual VO_VOID NewClusterTimeCodeArrived( VO_S64 timecode );
	virtual VO_VOID NewBlockGroupStarted();
	virtual VO_VOID NewReferenceBlockArrived();
*/
	virtual VO_U32 GetSampleN(VO_SOURCE_SAMPLE* pSample);

protected:
	VO_BOOL BigToLittlePCM(VO_PBYTE pData, VO_U32 nSize, VO_U32 nBitDepth);	
	VO_VOID perpare_ogg_headerdata();
	static VO_BOOL SampleNotify(VO_PTR pUser, VO_PTR wParam , VO_PTR lParam );
protected:
	VO_S64 m_lastrelativetimecode;

	VO_PBYTE m_ptr_privatedata;
	VO_U32 m_privatedata_size;
};