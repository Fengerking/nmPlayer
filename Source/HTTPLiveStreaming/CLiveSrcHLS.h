	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CLiveSrcHLS.h

	Contains:	CLiveSrcHLS header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-11-01		JBF			Create file

*******************************************************************************/
#ifndef __CLiveSrcHLS_H__
#define __CLiveSrcHLS_H__

#include "CLiveSrcBase.h"

#include "voString.h"
#include "voCSemaphore.h"
#include "voLog.h"
#include "voString.h"

#include "vo_http_live_streaming.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class CLiveSrcHLS : public CLiveSrcBase
{
public:
	CLiveSrcHLS(void);
	virtual ~CLiveSrcHLS(void);

	virtual VO_U32	Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData);
	virtual	VO_U32	Close();

	virtual	VO_U32 	Start();
	virtual	VO_U32 	Pause();
	virtual	VO_U32 	Stop();
	virtual	VO_U32 	SetDataSource(VO_PTR pSource, VO_U32 nFlag);
	virtual	VO_U32 	Seek(VO_U64 nTimeStamp);
	virtual	VO_U32 	GetParam(VO_U32 nParamID, VO_PTR pValue);
	virtual	VO_U32 	SetParam(VO_U32 nParamID, VO_PTR pValue);

	static	VO_VOID	SendOutData (VO_VOID * pUserData, VO_LIVESRC_SAMPLE * ptr_sample);
	static	VO_VOID	NotifyEvent (VO_VOID * pUserData, Event * pEvent);

protected:
	vo_http_live_streaming *	m_pHLS;
	VO_LIVESRC_SAMPLE			m_sVideoSample;
	VO_SOURCE_SAMPLE			m_videosample;
	VO_LIVESRC_SAMPLE			m_sAudioSample;
	VO_SOURCE_SAMPLE			m_audiosample;

};

#ifdef _VONAMESPACE
}
#endif


#endif //__CLiveSrcHLS_H__
