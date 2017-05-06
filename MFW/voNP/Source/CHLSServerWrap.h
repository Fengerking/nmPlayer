/*
 *  CNativePlayerWrap.h
 *
 *  Created by Lin Jun on 09/10/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _CHLSServerWrap_H__
#define _CHLSServerWrap_H__


#include "CBaseWrap.h"
#include "CHLSServer.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CHLSServerWrap : public CBaseWrap
{
public:
	CHLSServerWrap(VONP_LIB_FUNC* pLibOP);
	virtual ~CHLSServerWrap(void);
	
public:
	virtual int Init();
	virtual int Uninit();
	virtual int Open(void* pSource, int nFlag);
	virtual int Close();
	virtual int Run();
	virtual int Pause();
	virtual int Stop();
    virtual int SetPos(int nPos);
    virtual int SetView(void* pView);
    
protected:
    virtual bool    IsCanReadBuffer(bool bAudio);
    virtual bool    CheckBufferReady();
    virtual int     SendRawFrame(bool bAudio, VONP_BUFFERTYPE* pFrame);
    virtual int     OnFormatChanged();
    virtual int     OnReadBufEOS();
    
    virtual int     CreateNativePlayer();
    virtual void    doProcessPlayerReadyRun();
    virtual void    doProcessResume(void* hView);
    
    static int      OnServerEvent(void * pUserData, int nID, void * pParam1, void * pParam2);
    virtual int     doOnServerEvent(int nID, void * pParam1, void * pParam2);
    
private:
    bool            CheckServerBufferFull(long long llCurrTime);
    
    int             CreateServer();
    void            DestroyServer();
    
    void            DestroyOldInstance();
    
protected:
    CHLSServer*         m_pServer;
    CHttpBaseStream*    m_pCurrUnavailableStream;
    bool                m_bServerReady;
    CBaseNativePlayer*  m_pOldNativePlayer;
    CHLSServer*         m_pOldServer;
    voCMutex            m_mtSeek;
    int                 m_nMaxFrameBufDepth;
};

#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE


#endif //_CHLSServerWrap_H__
