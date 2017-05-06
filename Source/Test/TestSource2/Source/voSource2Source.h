#ifndef __VOSOURCE2SOURCE_H__

#define __VOSOURCE2SOURCE_H__

#include "CDllLoad.h"
#include "ISource2Interface.h"


class voSource2Source
	: public ISource2Interface
{
public:
	voSource2Source( const VO_TCHAR * str_modulename , const VO_TCHAR * str_apiname );
	virtual ~voSource2Source(void);

	virtual VO_U32 Init ( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam );
	virtual VO_U32 Uninit ();
	virtual VO_U32 Open ();
	virtual VO_U32 Close ();
	virtual VO_U32 Start ();
	virtual VO_U32 Pause ();
	virtual VO_U32 Stop ();
	virtual VO_U32 Seek ( VO_U64* pTimeStamp);
	virtual VO_U32 GetDuration ( VO_U64 * pDuration);
	virtual VO_U32 GetSample ( VO_SOURCE2_TRACK_TYPE nTrackType , VO_PTR pSample );
	virtual VO_U32 GetProgramCount ( VO_U32 *pProgramCount);
	virtual VO_U32 GetProgramInfo ( VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo);
	virtual VO_U32 GetCurTrackInfo ( VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo );
	virtual VO_U32 SelectProgram ( VO_U32 nProgram);
	virtual VO_U32 SelectStream ( VO_U32 nStream);
	virtual VO_U32 SelectTrack ( VO_U32 nTrack);
	virtual VO_U32 GetDRMInfo ( VO_SOURCE2_DRM_INFO **ppDRMInfo);
	virtual VO_U32 SendBuffer ( const VO_SOURCE2_SAMPLE& buffer );
	virtual VO_U32 GetParam ( VO_U32 nParamID, VO_PTR pParam);
	virtual VO_U32 SetParam ( VO_U32 nParamID, VO_PTR pParam);

protected:
	CDllLoad m_loader;
	VO_SOURCE2_API m_api;
	VO_HANDLE m_handle;
	
};


#endif
