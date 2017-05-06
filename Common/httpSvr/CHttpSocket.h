	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2011		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CHttpSocket.h

	Contains:	CHttpSocket header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-12-21		JBF			Create file

*******************************************************************************/
#ifndef __CHttpSocket_H__
#define __CHttpSocket_H__

#include "CBaseSocket.h"

#include "CHttpRequest.h"
#include "CHttpRespond.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CHttpSocket : public CBaseSocket
{
public:
	CHttpSocket (CHttpBaseStream * pStream);
	virtual ~CHttpSocket (void);

	virtual int		Close (void);
    virtual int     SetStream(CHttpBaseStream * pStream);
    virtual bool    IsPartialContent();
    virtual bool    IsStreamSendComplete();

protected:
   	virtual int		DoneRead (void);
   	virtual int		DoneWrite (void);

protected:
	CHttpRequest *		m_pRequest;
	CHttpRespond *		m_pRespond;

	CHttpBaseStream *	m_pStream;

	long long			m_llSendSize;
    bool                m_bStreamEOS;
};

    // End define
#ifdef _VONAMESPACE
}
#endif

#endif // __CHttpSocket_H__
