//
//  CHLSM3u8Stream.h
//  
//
//  Created by Jim Lin on 9/11/12.
//
//

#ifndef __CHLSM3u8Stream_H__
#define __CHLSM3u8Stream_H__

#include "CHttpStream.h"
#include "CPtrList.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef enum
{
    STREAM_FLAG_UNKNOWN         = 0,
    STREAM_FLAG_EOS             = 1,
    STREAM_FLAG_DISCONTINUITY   = 2
}HLS_STREAM_FLAG;

class CHLSHttpStream : public CHttpSinkStream
{
public:
	CHLSHttpStream (void * hUserData)
    :CHttpSinkStream(hUserData),m_bM3u8Stream(false),m_nStreamID(-1),m_nIdx(-1),m_nFlag(STREAM_FLAG_UNKNOWN){};
	virtual ~CHLSHttpStream (void){};
    
public:
    bool    IsM3u8Stream(){return m_bM3u8Stream;};
    void    SetID(int nID){m_nStreamID = nID;};
    int     GetID(){return m_nStreamID;};
    void    SetIdx(int nIdx){m_nIdx = nIdx;};
    int     GetIdx(){return m_nIdx;};
    void    SetFlag(int nFlag){m_nFlag = nFlag;};
    bool    IsEOS(){return m_nFlag&STREAM_FLAG_EOS;};
    bool    IsDiscontinuity(){return m_nFlag&STREAM_FLAG_DISCONTINUITY;};
    
protected:
    bool    m_bM3u8Stream;
    int     m_nStreamID;
    int     m_nIdx;
    int     m_nFlag;
};

class CHLSM3u8Stream : public CHLSHttpStream
{
public:
	CHLSM3u8Stream (void * hUserData);
	virtual ~CHLSM3u8Stream (void);
    
public:
    virtual bool    Open (void * pSource, int nFlag);
    virtual bool    Close (void);
    void            AddStream(CHLSHttpStream* pStream);
    void            RemoveStream(CHLSHttpStream* pStream);
    void            CreateM3u8();
    bool            IsReady();
    int             Flush();

    int             GetChunkDuration();
    void            SetChunkDuration(int nDuration);
    
    int             GetMaxChunkCount();
    int             GetAvailableStreamCount();
    int             GetPortNumber();
    char*           GetM3u8URL();
    
private:
    void            CreateRandomURL();
    void            UpdateM3U8(int nUpdateCount);
    int             IsNeedUpdateM3U8();
    int             GetMaxDuration();
    int             CheckSameStreamCount(CObjectList<CHLSHttpStream>* pSrcList, CObjectList<CHLSHttpStream>* pDstList);
    
    void            DumpChunk(CHLSHttpStream* pStream);
    void            DumpStreamListInfo(bool bM3u8List);
    char*           GetIPAddress();
    
private:
    char            m_szPlaybackURL[1024];
    int             m_nTargetDuration;
    
    voCMutex        m_mtxStream;
    CObjectList<CHLSHttpStream>     m_AvailableStreamList;
    CObjectList<CHLSHttpStream>     m_M3u8List;
    
    int             m_nCurrStreamIdx;
    static int      m_nPortNumber;
    
    bool            m_bLastChunk;
    
    int             m_nNotUpdateM3u8Time;
    char            m_szIPAddr[128];
};

#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE

#endif /* defined(__CHLSM3u8Stream_H__) */
