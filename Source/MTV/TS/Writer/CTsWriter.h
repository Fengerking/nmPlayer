#pragma once
#include "CBaseWriter.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CacheBufferList;
const VO_U32 MAX_CACHE_AUDIO_SAMPLE_COUNT = 20;
const VO_U32 MAX_CACHE_VIDEO_SAMPLE_COUNT = 20;
class CTsWriter : public CBaseWriter
{
public:
	CTsWriter(void);
	virtual ~CTsWriter(void);

public:
	virtual	VO_U32	Init(VO_TCHAR * pWorkingPath);
	virtual VO_U32	Uninit();
	virtual VO_U32	Open(VO_FILE_SOURCE* pFileSource , VO_SINK_OPENPARAM * pParam);
	virtual VO_U32	Close();
	virtual VO_U32	AddSample( VO_SINK_SAMPLE * pSample);
	virtual VO_U32	SetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32	GetParam(VO_U32 uID, VO_PTR pParam);

private:
	virtual CBaseAssembler* doCreateAssembler();
	VO_U32	AssembleSample(VO_SINK_SAMPLE * pSample,VO_BOOL bKeyFrame);
	VO_U32	AssembleAudioSample(VO_SINK_SAMPLE * pSample,VO_S64 llDelay,VO_BOOL bForece = VO_FALSE);
	CacheBufferList	*		m_pAudioCacheBuf;
	CacheBufferList	*		m_pVideoCacheBuf;
	VO_U32					m_nBufDuration;
	VO_SINK_SAMPLE			m_TmpAudioSample;
	VO_BOOL					m_bInputPTS;
	VO_PTR					m_hCheck;
	VO_BOOL					m_bDisbaleStep;

};

#ifdef _VONAMESPACE
}
#endif