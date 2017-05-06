#ifndef _CMp3PushParser_H
#define _CMp3PushParser_H

#include "CBaseAudioPushParser.h"
#include "voAAC2.h"
#include "Mp3FileDataStruct.h"
#include "CID3Parser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define MP3_EXT_DATA_LEN		12

class CMp3PushParser :public CBaseAudioPushParser
{
public:
	CMp3PushParser();
	~CMp3PushParser();
	VO_VOID Reset();
public:
	VO_U32 Init(VO_PTR pSource, VO_U32 nFlag, VO_SOURCE2_INITPARAM * pParam);
	VO_U32 UnInit();
	VO_U32 SendBuffer(const VO_SOURCE2_SAMPLE& Buffer);
protected:
	DECLARE_USE_MP3_GLOBAL_VARIABLE
private:
	VO_U32		Process();
	VO_U32		FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData);
	VO_U32		FindID3Head(tagPushFrameInfo &FraInfo,VO_U64 ullEndPos);
	VO_U32		FindMp3Head(tagPushFrameInfo &FraInfo,VO_U64 ullEndPos);
	VO_U32		ParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize, VO_BOOL bInit = VO_FALSE);
	VO_BOOL		IsHeader(VO_PBYTE pHeader);
	VO_U32		ParseMp3Frame(tagPushFrameInfo FraInfo);
	VO_U32		ParseID3Frame(tagPushFrameInfo FraInfo);
	VO_U32		ProcessMp3Head();
	VO_BOOL		CheckID3FrameData(ID3Frame* pID3Frame);
private:
	VO_BOOL			m_bCheckHead;
	CID3Parser*		m_pID3Parser;
	MPA_VERSION		m_Version;
	MPA_LAYER		m_Layer;
	MPA_CHANNEL		m_ChannelMode;
	VO_BOOL			m_bLSF;					//true means lower sampling frequencies (=MPEG2/MPEG2.5)
	VO_U32			m_dwSamplesPerSec;
	VO_U32			m_dwSamplesPerFrame;
	double			m_dSampleTime;			//mp3 sample time is not integer
	VO_U32			m_dwAvgBytesPerSec;
	VO_BYTE			m_btHeader[4];
	VO_BYTE			m_btHeadData[MP3_EXT_DATA_LEN];
	VO_U64			m_ullTimeStamp;
};

#ifdef _VONAMESPACE
}
#endif

#endif //CMp3PushParser


