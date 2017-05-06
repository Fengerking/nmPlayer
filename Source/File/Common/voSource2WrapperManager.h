#ifndef __VOSOURCE2WRAPPERMANAGER_H__
#define __VOSOURCE2WRAPPERMANAGER_H__

#include "voYYDef_filcmn.h"
#include "voSource2WrapperPushPlay.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class voSource2WrapperManager
{
public:
	voSource2WrapperManager(void);
	~voSource2WrapperManager(void);

	VO_U32 Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pParam );
	VO_U32 Uninit() { 
		VOLOGUNINIT();
		return m_pSourceWrapper->Uninit();
	}

	VO_U32 Open() { return m_pSourceWrapper->Open(); }
	VO_U32 Close() { return m_pSourceWrapper->Close(); }

	VO_U32 Start() { return m_pSourceWrapper->Start(); }
	VO_U32 Pause() { return m_pSourceWrapper->Pause(); }
	VO_U32 Stop() { return m_pSourceWrapper->Stop(); }

	VO_U32 GetSample( VO_SOURCE2_TRACK_TYPE nOutPutType , VO_PTR pSample ) { return m_pSourceWrapper->GetSample(nOutPutType, pSample); }
	VO_U32 GetDuration(VO_U64 * pDuration) { return m_pSourceWrapper->GetDuration(pDuration); }
	VO_U32 GetProgramCount( VO_U32 *pProgramCount) { return m_pSourceWrapper->GetProgramCount(pProgramCount); }
	VO_U32 GetProgramInfo( VO_U32 uProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo) { return m_pSourceWrapper->GetProgramInfo(uProgram, pProgramInfo); }
	VO_U32 GetCurTrackInfo( VO_SOURCE2_TRACK_TYPE eTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo ) { return m_pSourceWrapper->GetCurTrackInfo(eTrackType, ppTrackInfo); }
	VO_U32 GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo) { return m_pSourceWrapper->GetDRMInfo(ppDRMInfo); }

	VO_U32 Seek(VO_U64* pTimeStamp) { return m_pSourceWrapper->Seek(pTimeStamp); }
	VO_U32 SelectProgram( VO_U32 uProgram) { return m_pSourceWrapper->SelectProgram(uProgram); }
	VO_U32 SelectStream( VO_U32 uStream) { return m_pSourceWrapper->SelectStream(uStream); }
	VO_U32 SelectTrack( VO_U32 uTrack) { return m_pSourceWrapper->SelectTrack(uTrack); }
	VO_U32 SendBuffer(const VO_SOURCE2_SAMPLE& Buffer ) { return m_pSourceWrapper->SendBuffer(Buffer); }

	VO_U32 GetParam(VO_U32 nParamID, VO_PTR pParam) { return m_pSourceWrapper->GetParam(nParamID, pParam); }
	VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);

private:
	VO_U64					m_ullActualFileSize;
	voSourceBaseWrapper*	m_pSourceWrapper;
};


#ifdef _VONAMESPACE
}
#endif

#endif //__VOSOURCE2WRAPPERMANAGER_H__
