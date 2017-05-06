/*
 *  CNativePlayerWrap.h
 *
 *  Created by Lin Jun on 09/10/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _CAVNativePlayer_H__
#define _CAVNativePlayer_H__


#include "CBaseNativePlayer.h"
#include "voRunRequestOnMain.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

class CAVNativePlayer : public CBaseNativePlayer, private voRunRequestOnMain
{
public:
	CAVNativePlayer();
	virtual ~CAVNativePlayer(void);
	
public:
	virtual int Init();
	virtual int Uninit();
	virtual int Open(void* pSource, int nFlag);
	virtual int Close();
	virtual int Run();
	virtual int Pause();
	virtual int Stop();
	virtual int SetPos(int nPos);
    virtual int GetBufferTime();
    virtual int ReplaceURL(unsigned char* pszNewURL);
    virtual int SetView(void* pView);
    virtual int EnableTrack(bool bAudio, bool bEnable);
protected:
    virtual void RunningRequestOnMain(int nID, void *pParam1, void *pParam2);
        
protected:
    char        m_szPlaybackURL[1024];    
    void*       m_pAVPlayer;
};


#endif //_CAVNativePlayer_H__
