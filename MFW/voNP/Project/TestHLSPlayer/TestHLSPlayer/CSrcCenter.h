//
//  CSrcCenter.h
//  CSrcCenter
//
//  Created by Jim Lin on 9/22/12.
//
//

#ifndef __CSrcCenter_H_
#define __CSrcCenter_H_

#include "OnStreamReadSrc.h"

class CSrcCenter
{
public:
    CSrcCenter ();
	virtual ~CSrcCenter (void);
    
    int			Init(void * pSource, int nFlag); // 0:use first file, 1:use second file
    int			Uninit(void);
    int 		Open(void);
    int 		Run (void);
    int 		Pause (void);
    int 		Stop (void);
    int 		Close (void);
    int 		Flush (void);
    int 		GetDuration (int * pDuration);
    int 		SetCurPos (int nCurPos);
    int 		GetParam (int nID, void * pValue);
    int 		SetParam (int nID, void * pValue);
    void*		GetReadBufPtr();
    void        SetVideoOnly(bool bVideoOnly){m_bVideoOnly = bVideoOnly;};
    void        SetAudioOnly(bool bAudioOnly){m_bAudioOnly = bAudioOnly;};
    
	static int			SrcReadAudio(void * pUserData, VOOSMP_BUFFERTYPE * pBuffer);
	static int			SrcReadVideo(void * pUserData, VOOSMP_BUFFERTYPE * pBuffer);
    
private:
    int			doSrcReadAudio(VOOSMP_BUFFERTYPE * pBuffer);
    int			doSrcReadVideo(VOOSMP_BUFFERTYPE * pBuffer);
    
    
private:
    OnStreamReadSrc*        m_pSrc1;
    OnStreamReadSrc*        m_pSrc2;
    
    OnStreamReadSrc*        m_pCurrSrc;
    VOOSMP_READBUFFER_FUNC  m_fReadBuf;
    int                     m_nTimeStampOffset;
    int                     m_nLastTimeStamp;
    bool                    m_bVideoOnly;
    bool                    m_bAudioOnly;
};

#endif /* defined(__CSrcCenter_H_) */
