	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -     		*
	*																		*
	************************************************************************/
/*******************************************************************************
 *	File:		voOSNPPlayer.h
 *	
 *	Contains:	voOSNPPlayer header file
 *	
 *	Written by:	Jeff huang
 *	
 *	Change History (most recent first):
 *	2012-03-20		Jeff			Create file
 *	
 ******************************************************************************/

#ifndef __VO_OS_NP_PLAYER_H__
#define __VO_OS_NP_PLAYER_H__

#include "COMXALPlayer.h"
#include "voNPRenderIOS.h"
#include "voOSPlayerData.h"
#include "voPlayerView.h"

class voOSNPPlayer : public COMXALPlayer
{
public:
	voOSNPPlayer(voOSPlayerData *pData);
	virtual ~voOSNPPlayer (void);

	virtual int         Init();
	virtual int			Uninit();
    
    virtual int         SetView(void* pView);
	virtual int         SetDataSource (void * pSource, int nFlag);
    virtual int			Close();
    
	virtual int 		Run(void);
	virtual int 		Pause(void);
	virtual int 		Stop(void);
    
	virtual int         GetPos(int* pCurPos);
	virtual int 		SetPos(int nCurPos);

	virtual int			GetParam(int nParamID, void* pParam);
	virtual int			SetParam(int nParamID, void* pParam);
    
protected:
    virtual int         HandleEvent (int nID, void * pParam1, void * pParam2);
    
    virtual int			doProcessCCRenderQuery(int nQT, void * pParam1, void * pParam2);
	static  int			ProcessCCRenderQuery(void * pUserData, int nQT, void * pParam1, void * pParam2);
    
	virtual int			CreateCCRender (void);
    
    virtual int			SetDrawRect(VOOSMP_RECT* pRect);
    virtual int			UpdateDispType();
    virtual int			UpdateCCRect();
    
protected:
    voPlayerView *          m_pPlayerViewOld;
    voPlayerView *          m_pPlayerView;
	void *                  m_pCCRender;
    voNPRenderIOS		    m_cVideoRender;
    
    voOSPlayerData*         m_pData;
    bool                    m_bPaused;
};

#endif // __VO_OS_NP_PLAYER_H__
