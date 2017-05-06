#pragma once
#include "CMFWBase.h"
#include "vompCEngine.h"


class CMFWvoME2 : public CMFWBase
{
public:
	CMFWvoME2(void);
	virtual ~CMFWvoME2(void);

public:
	virtual VO_U32 Open(MFW_OPEN_PARAM* pParam);
	virtual VO_U32 Close();
	virtual VO_U32 SetDataSource(VO_PTR pSource);
	virtual VO_U32 SendBuffer(MFW_SAMPLE* pSample);
	virtual VO_U32 SetDrawArea(VO_U16 nLeft, VO_U16 nTop, VO_U16 nRight, VO_U16 nBottom);
	virtual VO_U32 Seek(VO_U64 nPosition);
	virtual VO_U32 GetPosition(VO_U64* pPosition);
	virtual VO_U32 Start();
	virtual VO_U32 Stop();
	virtual VO_U32 Pause();

	virtual VO_U32 GetParam(VO_U32 nParamID, VO_PTR pParam);
	virtual VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);

	virtual VO_U32 OnTrackInfo(VO_LIVESRC_TRACK_INFO* pTrackInfo);
	virtual VO_VOID	Flush();

public:
	 int	   HandleEvent (int nID, void * pParam1, void * pParam2);

protected:
	vompCEngine*	m_pvoME2;
};
