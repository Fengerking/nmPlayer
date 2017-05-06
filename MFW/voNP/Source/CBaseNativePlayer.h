/*
 *  CNativePlayerWrap.h
 *
 *  Created by Lin Jun on 09/10/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _CBaseNativePlayer_H__
#define _CBaseNativePlayer_H__

#include "voNPWrap.h"

typedef enum
{
    NATIVE_PLAYER_EVENT_POS_UPDATE     = 0,
    NATIVE_PLAYER_EVENT_START_RUN      = 1,
    NATIVE_PLAYER_EVENT_START_BUFFER   = 2,
    NATIVE_PLAYER_EVENT_STOP_BUFFER    = 3,
}NATIVE_PLAYER_EVENT_ID;

class CBaseNativePlayer
{
public:
	CBaseNativePlayer();
	virtual ~CBaseNativePlayer(void);
	
public:
	virtual int Init();
	virtual int Uninit();
	virtual int SetView(void* pView);
	virtual int Open(void* pSource, int nFlag);
	virtual int Close();
	virtual int Run();
	virtual int Pause();
	virtual int Stop();
	virtual int GetPos();
	virtual int SetPos(int nPos);
	virtual int GetParam(int nParamID, void* pValue);
	virtual int SetParam(int nParamID, void* pValue);
    virtual int GetBufferTime();
    virtual int ReplaceURL(unsigned char* pszNewURL);
    virtual int EnableTrack(bool bAudio, bool bEnable);
    
protected:
    int SendEvent(int nEventID, void* pParam1, void*pParam2);
    
protected:
    void*               m_pView;
    VONP_LISTENERINFO   m_NotifyEventInfo;
    int                 m_nCurrPos;
};


#endif //_CBaseNativePlayer_H__
