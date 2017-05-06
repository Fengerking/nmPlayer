
#ifndef __ISOURCE2INTERFACE_H__

#define __ISOURCE2INTERFACE_H__

#include "voSource2.h"

class ISource2Interface
{
public:
	virtual VO_U32 Init ( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam ) = 0;
	virtual VO_U32 Uninit () = 0;
	virtual VO_U32 Open () = 0;
	virtual VO_U32 Close () = 0;
	virtual VO_U32 Start () = 0;
	virtual VO_U32 Pause () = 0;
	virtual VO_U32 Stop () = 0;
	virtual VO_U32 Seek ( VO_U64* pTimeStamp) = 0;
	virtual VO_U32 GetDuration ( VO_U64 * pDuration) = 0;
	virtual VO_U32 GetSample ( VO_SOURCE2_TRACK_TYPE nTrackType , VO_PTR pSample ) = 0;
	virtual VO_U32 GetProgramCount ( VO_U32 *pProgramCount) = 0;
	virtual VO_U32 GetProgramInfo ( VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo) = 0;
	virtual VO_U32 GetCurTrackInfo ( VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo ) = 0;
	virtual VO_U32 SelectProgram ( VO_U32 nProgram) = 0;
	virtual VO_U32 SelectStream ( VO_U32 nStream) = 0;
	virtual VO_U32 SelectTrack ( VO_U32 nTrack) = 0;
	virtual VO_U32 GetDRMInfo ( VO_SOURCE2_DRM_INFO **ppDRMInfo) = 0;
	virtual VO_U32 SendBuffer ( const VO_SOURCE2_SAMPLE& buffer ) = 0;
	virtual VO_U32 GetParam ( VO_U32 nParamID, VO_PTR pParam) = 0;
	virtual VO_U32 SetParam ( VO_U32 nParamID, VO_PTR pParam) = 0;
};


#endif