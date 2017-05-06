#pragma once
#include "voLiveSource.h"
#include "CDLLLoad.h"

class CLiveSourceBase : public CDllLoad
{
public:
	CLiveSourceBase(VO_U16 type);
	virtual ~CLiveSourceBase(void);

public:
	VO_U32 Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData);
	VO_U32 Close();
	VO_U32 Scan(VO_LIVESRC_SCAN_PARAMEPTERS* pParam);
	VO_U32 Cancel(VO_U32 nID);
	VO_U32 Start();
	VO_U32 Stop();
	VO_U32 SetDataSource(VO_PTR pSource, VO_U32 nFlag);
	VO_U32 Seek(VO_U64 nTimeStamp);
	VO_U32 GetChannel(VO_U32 * pCount, VO_LIVESRC_CHANNELINFO ** ppChannelInfo);
	VO_U32 SetChannel(VO_S32 nChannelID);
	VO_U32 GetESG(VO_S32 nChannelID, VO_LIVESRC_ESG_INFO** ppESGInfo);
	VO_U32 GetParam(VO_U32 nParamID, VO_PTR pValue);
	VO_U32 SetParam(VO_U32 nParamID, VO_PTR pValue);


public:
	virtual VO_U32 LoadLib (VO_HANDLE hInst);
protected:
	VO_U16				m_nSrcType;
	VO_HANDLE			m_hLiveSrc;
	VO_LIVESRC_API		m_LiveSrcFunc;
};
