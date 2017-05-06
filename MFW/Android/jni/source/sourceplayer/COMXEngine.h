	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		COMXEngine.h

	Contains:	COMXEngine header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __COMXEngine_H__
#define __COMXEngine_H__

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1

#include <windows.h>
#include <tchar.h>
#endif // _WIN32

#include "vomeAPI.h"
#include "voString.h"
//TBD:change the location as you need
#include "vomeAPPath.h"

class COMXEngine
{
public:
	// Used to control the image drawing
	COMXEngine (void);
	virtual ~COMXEngine (void);

	OMX_ERRORTYPE Init (VOMECallBack pCallBack, OMX_PTR pUserData);
	OMX_ERRORTYPE Uninit (){Release();}
	OMX_ERRORTYPE SetDisplayArea (OMX_VO_DISPLAYAREATYPE * pDisplay);
	OMX_ERRORTYPE GetParam (OMX_S32 nID, OMX_PTR pValue);
	OMX_ERRORTYPE SetParam (OMX_S32 nID, OMX_PTR pValue);

	OMX_ERRORTYPE Playback (VOME_SOURCECONTENTTYPE * pSource);
	OMX_ERRORTYPE Recorder (VOME_RECORDERFILETYPE * pRecFile);
	OMX_ERRORTYPE Snapshot (VOME_RECORDERIMAGETYPE * pRecImage);
	OMX_ERRORTYPE Close (void);

	OMX_ERRORTYPE Run (void);
	OMX_ERRORTYPE Pause (void);
	OMX_ERRORTYPE Stop (void);

	OMX_ERRORTYPE GetStatus (VOMESTATUS * pStatus);
	OMX_ERRORTYPE GetDuration (OMX_S32 * pDuration);
	OMX_ERRORTYPE GetCurPos (OMX_S32 * pCurPos);
	OMX_ERRORTYPE SetCurPos (OMX_S32 nCurPos);

	OMX_ERRORTYPE SetCompCallBack (OMX_CALLBACKTYPE * pCompCallBack);
	OMX_ERRORTYPE EnumComponentName (OMX_STRING pCompName, OMX_U32 nIndex);
	OMX_ERRORTYPE GetRolesOfComponent (OMX_STRING pCompName, OMX_U32 * pNumRoles, OMX_U8 ** ppRoles);
	OMX_ERRORTYPE GetComponentsOfRole (OMX_STRING pRole, OMX_U32 * pNumComps, OMX_U8 ** ppCompNames);
	OMX_ERRORTYPE AddComponent (OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent);
	OMX_ERRORTYPE RemoveComponent (OMX_COMPONENTTYPE * pComponent);
	OMX_ERRORTYPE ConnectPorts (OMX_COMPONENTTYPE * pOutputComp, OMX_U32 nOutputPort,
								OMX_COMPONENTTYPE * pInputComp, OMX_U32 nInputPort, OMX_BOOL bTunnel);
	OMX_ERRORTYPE SaveGraph (OMX_STRING pFile);
	OMX_ERRORTYPE OpenGraph (OMX_STRING pFile);


protected:
	OMX_U32		LoadDll (void);
	virtual OMX_U32		Release (void);

protected:
	VO_TCHAR		m_szDllFile[256];
#ifdef _WIN32
	HMODULE			m_hDll;
#elif defined LINUX
	OMX_PTR			m_hDll;
#endif

	VOOMX_ENGINEAPI		m_OMXEng;
	VOOMX_COMPONENTAPI	m_OMXComp;
	OMX_PTR				m_hEngine;

};

#endif // __COMXEngine_H__
