	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		COMXQualcommDataSourcePlayer.h

	Contains:	COMXQualcommDataSourcePlayer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __COMXQualcommDataSourcePlayer_H__
#define __COMXQualcommDataSourcePlayer_H__



#include "vomeAPI.h"
#include "voString.h"

#include "COMXEngine.h"

#include "voFLOEngine.h"

typedef void* HANDLE;
#if !defined(VORTSPIMP)&&!defined(_WIN32_XP)
#define VO_MOBITV_USE_SHARE_LIB 1
#endif//#if !defined(VORTSPIMP)&&!defined(_WIN32_XP)



class COMXQualcommDataSourcePlayer : public COMXEngine
{
	
	static COMXQualcommDataSourcePlayer * g_pDataSourcePlayer;
		
public:
	// Used to control the image drawing
	COMXQualcommDataSourcePlayer (void);
	virtual ~COMXQualcommDataSourcePlayer (void);

	OMX_ERRORTYPE SetDisplayArea (OMX_VO_DISPLAYAREATYPE * pDisplay);

	OMX_ERRORTYPE Playback (VOME_SOURCECONTENTTYPE * pSource);
	OMX_ERRORTYPE Close (void);

	OMX_ERRORTYPE Run (void);
	OMX_ERRORTYPE Pause (void);
	OMX_ERRORTYPE Stop (void);

	OMX_ERRORTYPE GetDuration (OMX_S32 * pDuration);
	OMX_ERRORTYPE GetCurPos (OMX_S32 * pCurPos);
	OMX_ERRORTYPE SetCurPos (OMX_S32 nCurPos);



protected:
	OMX_COMPONENTTYPE *		m_pCompSrc;
	OMX_COMPONENTTYPE *		m_pCompAD;
	OMX_COMPONENTTYPE *		m_pCompVD;
	OMX_COMPONENTTYPE *		m_pCompAR;
	OMX_COMPONENTTYPE *		m_pCompVR;

	OMX_COMPONENTTYPE *		m_pCompClock;

	OMX_VO_DISPLAYAREATYPE	m_dspArea;

public:	
	
	
public:
	virtual OMX_U32		Release (void);
	
	
// ====================================================

protected:
	OMX_ERRORTYPE		CreateFLOEngine();
	OMX_ERRORTYPE		DestroyFLOEngine();
	
	
protected:
	static VO_S32 OnEvent(int nEventType, unsigned int nParam, void * pData);
	static VO_S32 OnFrame(VO_FLOENGINE_FRAME * pFrame);
	
public:
	static OMX_PTR	OMX_mmLoadLib (OMX_PTR	pUserData, OMX_STRING pLibName, OMX_S32 nFlag);
	static OMX_PTR	OMX_mmGetAddress (OMX_PTR	pUserData, OMX_PTR hLib, OMX_STRING pFuncName, OMX_S32 nFlag);
	static OMX_S32	OMX_mmFreeLib (OMX_PTR	pUserData, OMX_PTR hLib, OMX_S32 nFlag);

private:
#ifdef _WIN32
	HMODULE		m_libFLOEngine;
#else
	OMX_PTR		m_libFLOEngine;
#endif // _WIN32

	VO_HANDLE           m_hFLOEngine;
	VO_FLOENGINE_API    m_FLOEngineAPI;

	VO_FLOENGINE_CALLBACK m_FLOEngineCallBack;
	
	OMX_VO_LIB_OPERATOR		m_libOP;

private:
	char    m_szSource[2048];
};

#endif // __COMXQualcommDataSourcePlayer_H__
