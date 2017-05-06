
#ifndef __CASController_H__
#define __CASController_H__

#include "voSource2.h"
#include "voString.h"
#include "voCSemaphore.h"
#include "voLog.h"
#include "voString.h"


//#define _NEW_SOURCEBUFFER

#include "voSourceBufferManager.h"

#ifdef _DASH_SOURCE_
#include "vo_mpd_streaming.h"
#define ASController_CLASS vo_mpd_streaming
#endif	// _DASH_SOURCE_

#ifdef _ISS_SOURCE_
#include "vo_smooth_streaming.h"
#define ASController_CLASS vo_smooth_streaming
#endif	// _ISS_SOURCE_

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#ifdef _IOS
#ifdef _DASH_SOURCE_
namespace _DASH{
#endif	// _DASH_SOURCE_

#ifdef _ISS_SOURCE_
namespace _ISS{
#endif	// _ISS_SOURCE_
#endif	// _IOS



class CASController 
{
public:
	CASController();
	~CASController();

	VO_U32 Init(VO_PTR pSource , VO_U32 nFlag ,  VO_SOURCE2_INITPARAM * pInitParam  );
	VO_U32 Uninit();
	VO_U32 Open();
	VO_U32 Close();
	VO_U32 Start();
	VO_U32 Pause();
	VO_U32 Stop();
	VO_U32 Seek(VO_U64* pTimeStamp);
	VO_U32 GetDuration(VO_U64 * pDuration);
	VO_U32 GetSample(VO_U32 nOutPutType , VO_PTR pSample);

	VO_U32 GetProgramCount( VO_U32 *pProgramCount );
	VO_U32 GetProgramInfo( VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo );
	VO_U32 GetCurTrackInfo( VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo );
	VO_U32 SelectProgram( VO_U32 nProgram );
	VO_U32 SelectStream( VO_U32 nStream );
	VO_U32 SelectTrack( VO_U32 nTrack );

	VO_U32 GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo);
	VO_U32 GetParam(VO_U32 nParamID, VO_PTR pParam);
	VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);

protected:
	VO_CHAR *m_pstrSource;
	VO_U64 m_uPreChunkDelay;
	VO_U64 m_uTimeStamp;
	VO_U64 m_uVideoDelay;
	voSourceBufferManager m_SourceBufferManager;
protected:
	ASController_CLASS *m_pPlayer;
	VO_BOOL m_bStop;
	voCMutex m_lockMutex;
	FILE *ff;

	static VO_VOID	StatusOutData (VO_VOID * pUserData, VO_U32 id, VO_VOID *pBuffer);
	static	VO_VOID	SendOutData (VO_VOID * pUserData, VO_LIVESRC_SAMPLE * ptr_sample);

	VO_BOOL ConvertData (VO_PBYTE pOutData, VO_PBYTE pInData, VO_U32 nInSize);
};

#ifdef _IOS
#ifdef _ISS_SOURCE_
}
#endif	// _ISS_SOURCE_

#ifdef _DASH_SOURCE_
}
#endif	// _DASH_SOURCE_
#endif	// _IOS
#ifdef _VONAMESPACE
}
#endif

#endif //__CASController_H__

