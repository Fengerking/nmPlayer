	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -     		*
	*																		*
	************************************************************************/
/*******************************************************************************
 *	File:		voOSPlayer.h
 *	
 *	Contains:	voOSPlayer header file
 *	
 *	Written by:	Jeff huang
 *	
 *	Change History (most recent first):
 *	2012-03-20		Jeff			Create file
 *	
 ******************************************************************************/

#ifndef __VO_OS_PLAYER_H__
#define __VO_OS_PLAYER_H__

#include "COSVomePlayer.h"
#include "voBaseRender.h"
#include "voOSPlayerData.h"

class CVideoRenderManager;
class CAudioRenderManager;

class voOSPlayer : public COSVomePlayer
{
public:
	voOSPlayer(voOSPlayerData *pData);
	virtual ~voOSPlayer (void);

	virtual int         Init();
	virtual int			Uninit();
    
    virtual int         SetView(void* pView);
	virtual int         SetDataSource (void * pSource, int nFlag);
    virtual int			Close();
    
	virtual int 		Run(void);
	virtual int 		Pause(void);
	virtual int 		Stop(void);
    
	virtual int		    GetPos (int * pCurPos);
	virtual int		    SetPos (int nCurPos);
    
	virtual int			GetParam(int nParamID, void* pParam);
	virtual int			SetParam(int nParamID, void* pParam);
    
protected:
    virtual int         HandleEvent (int nID, void * pParam1, void * pParam2);
    
    virtual int			doProcessRenderQuery(QueryType nQT, void * pParam1, void * pParam2);
	static  int			ProcessRenderQuery(void * pUserData, QueryType nQT, void * pParam1, void * pParam2);
    
    virtual int			doProcessCCRenderQuery(int nQT, void * pParam1, void * pParam2);
	static  int			ProcessCCRenderQuery(void * pUserData, int nQT, void * pParam1, void * pParam2);
    
    virtual int			CreateAudioRender (void);
	virtual int			CreateVideoRender (void);
	virtual int			CreateCCRender (void);
    
    virtual int			GetAudioSample(VOMP_BUFFERTYPE** pOutBuffer);
	virtual int			GetVideoSample(VOMP_BUFFERTYPE** pOutBuffer);
	virtual int			GetAudioFormat(VOMP_AUDIO_FORMAT* pFmt);
	virtual int			GetVideoFormat(VOMP_VIDEO_FORMAT* pFmt);
    
//	virtual int			GetAudioBufferTime (void);
//	virtual int			GetVideoBufferTime (void);
    
    virtual int			SetDrawRect(VOOSMP_RECT* pRect);
    virtual int			UpdateDispType();
    virtual int			UpdateCCRect();
    
protected:
	static void *			vomtGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag);
	static void *			vomtLoadLib (void * pUserData, char * pLibName, int nFlag);
	static int				vomtFreeLib (void * pUserData, void * hLib, int nFlag);
    
    virtual void*           voOnLoadLib (void * pUserData, char * pLibName, int nFlag);
    
protected:
    
	CAudioRenderManager *		m_pAudioRender;
	CVideoRenderManager *		m_pVideoRender;
    
	void *                      m_pCCRender;
    
    VOMP_LIB_FUNC		        m_sLibFunc;
    
	int						    m_nPlayMode;
    
    voOSPlayerData*             m_pData;
    bool                        m_bPaused;
    
#if defined(_USE_OLD_AUDIO_RENDER)
    VO_BOOL                     m_bEnableAudioRefClock;
#endif
};

#endif // __VO_OS_PLAYER_H__
