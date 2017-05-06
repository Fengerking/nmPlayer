//
//  CBaseServer.h
//  
//
//  Created by Jim Lin on 9/12/12.
//
//

#ifndef __CBaseServer_H__
#define __CBaseServer_H__

#include "voNPWrap.h"
#include "voCMutex.h"
#include "voRunRequestOnMain.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CHLSHttpStream;
class CHLSM3u8Stream;
class CHttpBaseStream;
class CHttpRequest;

class CBaseServer : public voRunRequestOnMain
{
public:
	CBaseServer();
	virtual ~CBaseServer(void);
	
public:
	virtual int Init();
	virtual int Uninit();
	virtual int Open(void* pSource, int nFlag, char* pPlaybackURL);
	virtual int Close();
	virtual int Run();
	virtual int Pause();
	virtual int Stop();
	virtual int GetParam(int nParamID, void* pValue);
	virtual int SetParam(int nParamID, void* pValue);
    virtual int IsReady();
    virtual int AddStream(CHttpBaseStream* pSteam);
    virtual int RemoveStream(CHttpBaseStream* pSteam);
    virtual int GetChunkDuration();
    virtual void SetChunkDuration(int nDuration);
    virtual int CreateM3U8();
    virtual int Flush();
    virtual void SetEOS(bool bEOS);
    
public:
    void                SetMaxPlaylistCount(int nCount);
    bool                IsStreamFull();
    
    //below two functions must after call Open
    CHLSM3u8Stream*     GetM3u8Stream();
    CHttpBaseStream*    GetUnavailableStream();
    
protected:
    virtual void        PrepareStream();
    
protected:
    int                 SendEvent(int nEventID, void* pParam1, void*pParam2);
    CHttpBaseStream*    QueryStream(CHttpRequest* pRequest);
    
    static int          OnSocketEvent(void * pUserData, int nID, void * pParam1, void * pParam2);
    virtual int         doSocketEvent(int nID, void * pParam1, void * pParam2);
    
protected:
    VONP_LISTENERINFO   m_NotifyServerEventInfo;
    CHLSM3u8Stream*     m_pM3u8Stream;
    
    CHLSHttpStream**	m_aStreamList;
    int                 m_nStreamCount;
    
    voCMutex            m_mtxStream;
    int                 m_nRequestM3u8Count;
    int                 m_nLastRequestM3u8Time;
    bool                m_bInitStream;
    bool                m_bEOS;
};

#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE


#endif /* defined(__CBaseServer_H__) */
