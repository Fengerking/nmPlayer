	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2011		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseSocket.h

	Contains:	CBaseSocket header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-12-21		JBF			Create file

*******************************************************************************/
#ifndef __CBaseSocket_H__
#define __CBaseSocket_H__

#include "voThread.h"

#if defined (_IOS) || defined (_MAC_OS)
#include <sys/time.h>
#include <sys/socket.h>
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define	SO_BUFF_SIZE	8192


typedef int (* SOCKETEVENTCALLBACK) (void * pUserData, int nID, void * pParam1, void * pParam2);
typedef struct
{
	SOCKETEVENTCALLBACK     pListener;
	void*                   pUserData;
}SOCKET_EVENT_CALLBACK_INFO;

typedef enum
{
    SOCKET_EVENT_HTTP_REQUEST               = 0,    //pass CHttpRequest*
    SOCKET_EVENT_STREAM_EOS                 = 1,    //stream was sent
    SOCKET_EVENT_SOCKET_ACCEPT_FAILED       = 2,    //socket accept failed
}SOCKET_EVENT_ID;

class CBaseSocket
{
public:
	CBaseSocket (void);
	virtual ~CBaseSocket (void);

	virtual int		Create (void);
	virtual int		SetSocket (int nSocket);
	virtual int		Start (void);
	virtual int		Stop (void);
	virtual int		Close (void);

	virtual int		WriteBuff (unsigned char * pBuffer, int nBuffSize);

	virtual void	SetTimeOut (int nTimeOut) {m_tmout.tv_usec = nTimeOut * 1000;}
	virtual int		GetSocket (void) {return m_hSocket;}
    virtual void    SetEventCallback(SOCKET_EVENT_CALLBACK_INFO* pInfo);

protected:
	virtual int		OnRead (void);
	virtual int		OnWrite (void);
	virtual int		OnExcept (void);

	virtual int		DoneRead (void);
	virtual int		DoneWrite (void);

    int             SendEvent(int nID, void * pParam1, void * pParam2);

protected:
	int				m_hSocket;
	fd_set			m_fdsRead;
	fd_set			m_fdsWrite;
	fd_set			m_fdsExcept;
	timeval			m_tmout;

	unsigned char *	m_pBuffRead;
	int				m_nSizeRead;
	unsigned char *	m_pBuffWrite;
	int				m_nSizeWrite;

	static	int		SocketThreadProc (VO_PTR pParam);
	virtual int		CheckEventLoop (void);
	voThreadHandle	m_hThread;
	bool			m_bStop;
    
    SOCKET_EVENT_CALLBACK_INFO m_EventCallback;
};

#ifdef _VONAMESPACE
}
#endif // End define _VONAMESPACE

#endif // __CBaseSocket_H__
