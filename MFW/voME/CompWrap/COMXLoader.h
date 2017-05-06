	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		COMXLoader.h

	Contains:	COMXLoader header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __COMXLoader_H__
#define __COMXLoader_H__

#include <OMX_Core.h>
#include <OMX_Component.h>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

typedef	OMX_ERRORTYPE (* OMX_INIT) (void);
typedef OMX_ERRORTYPE (* OMX_DEINIT) (void);
typedef OMX_ERRORTYPE (* OMX_COMPONENTNAMEENUM) (OMX_STRING cComponentName, OMX_U32 nNameLength, OMX_U32 nIndex);
typedef OMX_ERRORTYPE (* OMX_GETHANDLE) (OMX_HANDLETYPE* pHandle, OMX_STRING cComponentName, OMX_PTR pAppData, OMX_CALLBACKTYPE* pCallBacks);
typedef OMX_ERRORTYPE (* OMX_FREEHANDLE) (OMX_HANDLETYPE hComponent);
typedef OMX_ERRORTYPE (* OMX_SETUPTUNNEL) (OMX_HANDLETYPE hOutput, OMX_U32 nPortOutput, OMX_HANDLETYPE hInput, OMX_U32 nPortInput);
typedef OMX_ERRORTYPE (* OMX_GETCONTENTPIPE) (OMX_HANDLETYPE *hPipe,OMX_STRING szURI);
typedef OMX_ERRORTYPE (* OMX_GETCOMPONENTSOFROLE) (OMX_STRING role, OMX_U32 *pNumComps, OMX_U8 **compNames);
typedef OMX_ERRORTYPE (* OMX_GETROLESOFCOMPONENT) (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles);




// wrapper for whatever critical section we have
class COMXLoader
{
public:
    COMXLoader(void);
    virtual ~COMXLoader(void);

	virtual OMX_COMPONENTTYPE * CreateComponent (OMX_STRING pCoreFile, OMX_STRING	pCompName, OMX_PTR pAppData, OMX_CALLBACKTYPE* pCallBack);

	OMX_ERRORTYPE OMX_Init (void);
	OMX_ERRORTYPE OMX_Deinit (void);
	OMX_ERRORTYPE OMX_ComponentNameEnum (OMX_STRING cComponentName, OMX_U32 nNameLength, OMX_U32 nIndex);
	OMX_ERRORTYPE OMX_GetHandle (OMX_HANDLETYPE* pHandle, OMX_STRING cComponentName, OMX_PTR pAppData, OMX_CALLBACKTYPE* pCallBacks);
	OMX_ERRORTYPE OMX_FreeHandle (OMX_HANDLETYPE hComponent);
	OMX_ERRORTYPE OMX_SetupTunnel (OMX_HANDLETYPE hOutput, OMX_U32 nPortOutput, OMX_HANDLETYPE hInput, OMX_U32 nPortInput);
	OMX_ERRORTYPE OMX_GetContentPipe (OMX_HANDLETYPE *hPipe,OMX_STRING szURI);
	OMX_ERRORTYPE OMX_GetComponentsOfRole (OMX_STRING role, OMX_U32 *pNumComps, OMX_U8 **compNames);
	OMX_ERRORTYPE OMX_GetRolesOfComponent (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles);

protected:
	OMX_ERRORTYPE LoadLib (OMX_STRING pCoreFile);
	void          getFuncName(char* pOutName, const char* pInName, const char* inPrex=NULL,  const int nBuffLen = 256);

protected:
	VO_CHAR					m_szCoreFile[256];
	VO_CHAR					m_szCompName[256];
	OMX_COMPONENTTYPE *		m_pComponent;

#ifdef _WIN32
	HMODULE			m_hCoreFile;
#else
	OMX_PTR			m_hCoreFile;
#endif // _WIN32

	OMX_INIT				m_fInit;
	OMX_DEINIT				m_fDeinit;
	OMX_COMPONENTNAMEENUM	m_fNameEnum;
	OMX_GETHANDLE			m_fGetHandle;
	OMX_FREEHANDLE			m_fFreeHandle;
	OMX_SETUPTUNNEL			m_fSetupTunnel;
	OMX_GETCONTENTPIPE		m_fGetPipe;
	OMX_GETCOMPONENTSOFROLE	m_fGetCompRole;
	OMX_GETROLESOFCOMPONENT	m_fGetRoleComp;
};

typedef struct VO_COMPWRAP_OMX_LOADER {
    char						CoreName[128];
	COMXLoader *				Loader;
} VO_COMPWRAP_OMX_LOADER;

#endif //__COMXLoader_H__
