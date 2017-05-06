	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2011		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CListenSocket.h

	Contains:	CListenSocket header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-12-21		JBF			Create file

*******************************************************************************/
#ifndef __CListenSocket_H__
#define __CListenSocket_H__

#include "CBaseSocket.h"
#include "CHttpStream.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CListenSocket	: public CBaseSocket
{
public:
	CListenSocket (void * hUserData, int nPortNum);
	virtual ~CListenSocket (void);

	virtual int		Create (void);
	virtual int		Open (void * pSource, int nType);

	virtual int		Stop (void);
 	virtual int		Close (void);

	virtual int		SetParam(int nID, void * pParam);
	virtual int		GetParam(int nID, void * pParam);
    
    virtual int     SetStream(CHttpBaseStream * pStream);
    virtual int     Flush();

protected:
	virtual int		OnRead (void);
		
	virtual int		DoneRead (void);

	virtual int		CheckEventLoop (void);

protected:
	int					m_nPortNum;

	int					m_nMaxClient;
	CBaseSocket **		m_aClientSocket;
	int					m_nConnectClientNum;

	CHttpBaseStream *	m_pStream;

	void *				m_hUserData;
    
    voCMutex            m_cClientLock;
};
    
#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE

#endif // __CListenSocket_H__
