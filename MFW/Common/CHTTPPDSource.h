	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CHTTPPDSource.h

	Contains:	CHTTPPDSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __CHTTPPDSource_H__
#define __CHTTPPDSource_H__



#include "CRTSPSource.h"
#include "voPD.h"

//#define MS_HTTP

#ifdef MS_HTTP
#include "voPreHTTP.h"
#endif

#ifdef MS_HTTP
typedef VO_S32 (VO_API *pvoGetPreHTTPAPI)(VO_PREHTTP_API * pHandle, VO_U32 uFlag);
#endif

class CHTTPPDSource : public CRTSPSource
{
public:
	CHTTPPDSource (VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB);
	virtual ~CHTTPPDSource (void);

	virtual VO_U32		LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);
	virtual VO_U32		CloseSource (void);

	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);

	virtual VO_U32		GetTrackData (VO_U32 nTrack, VO_PTR pTrackData);
	virtual VO_U32		SetTrackPos (VO_U32 nTrack, VO_S64 * pPos);

	virtual VOPDInitParam*	GetPDInitParam() {return &m_initParam;}

protected:
	virtual int			LoadHTTPSource (void);
	virtual int			HandleStreamEvent (long EventCode, long * EventParam1);

	virtual void		CheckMotoProxySetting (void);

#ifdef MS_HTTP
	virtual VO_U32		CheckHTTPType(VO_LIVESRC_FORMATTYPE *type);
#endif

protected:
	VOPDInitParam		m_initParam;
	VO_TCHAR			m_szHTTPURL[1024*2];

	voCMutex			m_csHTTPStatus;
};

#endif // __CHTTPPDSource_H__
