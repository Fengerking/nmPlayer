	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMEEngine.h

	Contains:	CVOMEEngine header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __CVOMEEngine_H__
#define __CVOMEEngine_H__

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif // _WIN32

#include "vomeAPI.h"
#include "voString.h"

#include "CDllLoad.h"

/** Defines the major version of the OMX */
#define OMX_VERSION_MAJOR		1
/** Defines the minor version of the OMX */
#undef OMX_VERSION_MINOR
#define OMX_VERSION_MINOR		1
/** Defines the revision of the OMX */
#define OMX_VERSION_REVISION	0
/** Defines the step version of the OMX */
#define OMX_VERSION_STEP		0

/** Defines the major version of the core */
#define SPEC_VERSION_MAJOR  1
/** Defines the minor version of the core */
#define SPEC_VERSION_MINOR  0
/** Defines the revision of the core */
#define SPEC_REVISION      0
/** Defines the step version of the core */
#define SPEC_STEP          0

class CVOMEEngine : public CDllLoad
{
public:
	// Used to control the image drawing
	CVOMEEngine (void);
	virtual ~CVOMEEngine (void);

	OMX_ERRORTYPE Init (VOMECallBack pCallBack, OMX_PTR pUserData);
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
	OMX_ERRORTYPE GetComponentByName (OMX_STRING pCompName, OMX_COMPONENTTYPE ** ppComponent);
	OMX_ERRORTYPE GetComponentByIndex (OMX_U32 nIndex, OMX_COMPONENTTYPE ** ppComponent);

	OMX_ERRORTYPE SaveGraph (OMX_STRING pFile);
	OMX_ERRORTYPE OpenGraph (OMX_STRING pFile);


protected:
	virtual int			LoadModule (void);

	VOOMX_ENGINEAPI		m_OMXEng;
	VOOMX_COMPONENTAPI	m_OMXComp;

	OMX_PTR				m_hEngine;
};

#endif // __CVOMEEngine_H__
