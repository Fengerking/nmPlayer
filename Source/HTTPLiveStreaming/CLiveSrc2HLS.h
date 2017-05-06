
#ifndef __CLiveSrc2HLS_H__
#define __CLiveSrc2HLS_H__

#include "voSource2.h"
#include "voString.h"
#include "voCSemaphore.h"
#include "voLog.h"
#include "voString.h"

#include "vo_http_live_streaming_new.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class CLiveSrc2HLS 
{
public:
	CLiveSrc2HLS();
	~CLiveSrc2HLS();

	VO_U32 Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam );
	VO_U32 Uninit();
	VO_U32 Open();
	VO_U32 Close();
	VO_U32 Start();
	VO_U32 Pause();
	VO_U32 Stop();
	VO_U32 Seek(VO_U64* pTimeStamp);
	VO_U32 GetDuration(VO_U64 * pDuration);
	VO_U32 GetSample(VO_U32 nOutPutType , VO_PTR pSample);
	VO_U32 GetProgramCount(VO_U32 *pProgramCount);
	VO_U32 GetProgramInfo(VO_U32 nStreamID, VO_SOURCE2_PROGRAM_INFO **ppStreamInfo);
	VO_U32 GetCurSelTrackInfo(VO_U32 nOutPutType , VO_SOURCE2_TRACK_INFO **ppTrackInfo );
	VO_U32 SelectProgram(VO_U32 nProgramID );
	VO_U32 SelectStream(VO_U32 nStreamID);	
	VO_U32 SelectTrack(VO_U32 nTrackID );
	VO_U32 GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo);
	VO_U32 SendBuffer(const VO_SOURCE2_SAMPLE& buffer);	
	VO_U32 GetParam(VO_U32 nParamID, VO_PTR pParam);
	VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);

protected:
	vo_http_live_streaming_new *	m_pHLS;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CLiveSrc2HLS_H__

