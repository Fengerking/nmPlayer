    /************************************************************************
    *                                                                        *
    *        VisualOn, Inc. Confidential and Proprietary, 2003 -             *
    *                                                                        *
    ************************************************************************/
/*******************************************************************************
 *    File:        voOSPlayerData.h
 *
 *    Contains:    voOSPlayerData header file
 *
 *    Written by:    Jeff huang
 *
 *    Change History (most recent first):
 *    2012-10-04        Jeff            Create file
 *
 ******************************************************************************/

#ifndef __VO_OS_PLAYER_ADAPTER_H__
#define __VO_OS_PLAYER_ADAPTER_H__

#include "IOSBasePlayer.h"
#include "voOSPlayerData.h"

class COSBasePlayer;

class voOSPlayerAdapter
{
public:
    voOSPlayerAdapter();
    virtual ~voOSPlayerAdapter (void);

    virtual int            Init(int nPlayerType);
    virtual int            Uninit();
    
    virtual int            SetDataSource(void* pSource, int nFlag);

    virtual int            Run(void);
    virtual int            Pause(void);
    virtual int            Stop(void);
    virtual int            Close();
    virtual int            GetStatus(int* pStatus);
    virtual int            GetDuration(int* pDuration);
    
    virtual int            SetView(void* pView);
    
    virtual int            GetPos(int* pCurPos);
    virtual int            SetPos(int nCurPos);
    
    virtual int            GetParam(int nParamID, void* pParam);
    virtual int            SetParam(int nParamID, void* pParam);
    
    virtual int            GetSubtileSample (voSubtitleInfo * pSample);
    virtual int            GetSubLangNum(int *pNum);
    virtual int            GetSubLangItem(int Index, VOOSMP_SUBTITLE_LANGUAGE** pSubLangItem);
    virtual int            GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** pSubLangInfo);
    virtual int            SelectLanguage(int Index);
    
    virtual int		       GetSEISample(VOOSMP_SEI_INFO * pSample);

private:
    virtual int            ChangeNativeToVoPlayer();
    
    virtual int			   HandleEvent (int nID, void * pParam1, void * pParam2);
	static	int			   OnListener (void * pUserData, int nID, void * pParam1, void * pParam2);
    
private:
    void *                 m_pSource;
    int                    m_nFlag;
    
    COSBasePlayer *        m_pPlayer;
    int                    m_nPlayerType;
    void *                 m_pLogCB;
    VO_TCHAR               m_szPathLib[1024];
    
    voOSPlayerData         m_cData;
    
    VOOSMP_LISTENERINFO    m_cSendListener;
    VOOSMP_LISTENERINFO    m_cThisListener;
};

#endif // __VO_OS_PLAYER_ADAPTER_H__
