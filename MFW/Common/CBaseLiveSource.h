	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseLiveSource.h

	Contains:	CBaseLiveSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-11-01		JBF			Create file

*******************************************************************************/
#ifndef __CBaseLiveSource_H__
#define __CBaseLiveSource_H__

#include "voLiveSource.h"
#include "CDllLoad.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

class CBaseLiveSource : public CDllLoad
{
public:
	CBaseLiveSource(VO_U16 type);
	virtual ~CBaseLiveSource(void);

public:
	virtual VO_U32 	Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData);
	virtual VO_U32 	Close();
	virtual VO_U32 	Scan(VO_LIVESRC_SCAN_PARAMEPTERS* pParam);
	virtual VO_U32 	Cancel(VO_U32 nID);
	virtual VO_U32 	Start();
	virtual VO_U32 	Pause();
	virtual VO_U32 	Stop();
	virtual VO_U32	SendBuffer(VOMP_BUFFERTYPE * pBuffer);
	virtual VO_U32 	SetDataSource(VO_PTR pSource, VO_U32 nFlag);
	virtual VO_U32 	Seek(VO_U64 nTimeStamp);
	virtual VO_U32 	GetChannel(VO_U32 * pCount, VO_LIVESRC_CHANNELINFO ** ppChannelInfo);
	virtual VO_U32 	SetChannel(VO_S32 nChannelID);
	virtual VO_U32 	GetESG(VO_S32 nChannelID, VO_LIVESRC_ESG_INFO** ppESGInfo);
	virtual VO_U32 	GetParam(VO_U32 nParamID, VO_PTR pValue);
	virtual VO_U32 	SetParam(VO_U32 nParamID, VO_PTR pValue);

public:
	virtual VO_U32	LoadLib (VO_HANDLE hInst);

protected:
	VO_U16				m_nSrcType;
	VO_HANDLE			m_hLiveSrc;
	VO_LIVESRC_API		m_LiveSrcFunc;
};

#endif // __CBaseLiveSource_H__
