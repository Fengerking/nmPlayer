/*
 *  CHLSServer.h
 *
 *  Created by Lin Jun on 09/10/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _CHLSServer_H__
#define _CHLSServer_H__


#include "voNPWrap.h"
#include "CListenSocket.h"
#include "CHLSM3u8Stream.h"
#include "CBaseServer.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

class CHLSServer : public CBaseServer
{
public:
	CHLSServer();
	virtual ~CHLSServer(void);
	
public:
	virtual int Init();
	virtual int Uninit();
	virtual int Open(void* pSource, int nFlag, char* pPlaybackURL);
	virtual int Close();
	virtual int Run();
    virtual int Flush();
    virtual int Stop();
    
protected:
    virtual int doSocketEvent(int nID, void * pParam1, void * pParam2);
    virtual void PrepareStream();
    
    virtual void    RunningRequestOnMain(int nID, void *pParam1, void *pParam2);

protected:
    CListenSocket*      m_pListenSocket;
    bool                m_bListenSocketFail;
};


#endif //_CHLSServer_H__
