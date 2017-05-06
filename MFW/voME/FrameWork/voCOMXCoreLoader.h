	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreLoader.h

	Contains:	voCOMXCoreLoader header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __voCOMXCoreLoader_H__
#define __voCOMXCoreLoader_H__

#include <OMX_Core.h>
#include <vomeAPI.h>

#ifdef _WIN32
#include <windows.h>
#pragma warning(disable: 4996) //Use strxxx_s to replace strxxx
#endif // _WIN32

#include "voCOMXBaseObject.h"

typedef	OMX_ERRORTYPE (* OMX_INIT) (void);
typedef OMX_ERRORTYPE (* OMX_DEINIT) (void);
typedef OMX_ERRORTYPE (* OMX_COMPONENTNAMEENUM) (OMX_STRING cComponentName, OMX_U32 nNameLength, OMX_U32 nIndex);
typedef OMX_ERRORTYPE (* OMX_GETHANDLE) (OMX_HANDLETYPE* pHandle, OMX_STRING cComponentName, OMX_PTR pAppData, OMX_CALLBACKTYPE* pCallBacks);
typedef OMX_ERRORTYPE (* OMX_FREEHANDLE) (OMX_HANDLETYPE hComponent);
typedef OMX_ERRORTYPE (* OMX_SETUPTUNNEL) (OMX_HANDLETYPE hOutput, OMX_U32 nPortOutput, OMX_HANDLETYPE hInput, OMX_U32 nPortInput);
typedef OMX_ERRORTYPE (* OMX_GETCONTENTPIPE) (OMX_HANDLETYPE *hPipe,OMX_STRING szURI);
typedef OMX_ERRORTYPE (* OMX_GETCOMPONENTSOFROLE) (OMX_STRING role, OMX_U32 *pNumComps, OMX_U8 **compNames);
typedef OMX_ERRORTYPE (* OMX_GETROLESOFCOMPONENT) (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles);
typedef OMX_ERRORTYPE (* OMX_SETWORKINGPATH) (OMX_STRING pWorkingPath);

#define VO_MAX_COREREF		8

typedef struct voCoreRef
{
	char			name[256];
	int				ref_num;

	voCoreRef() : ref_num(0) {strcpy(name, "");}
} voCoreRef;

class voCoreRefMgr
{
public:
	voCoreRefMgr();
	virtual ~voCoreRefMgr();

public:
	int				AddRef(const char* name);
	int				Release(const char* name);

protected:
	voCoreRef		m_Refs[VO_MAX_COREREF];
};

// wrapper for whatever critical section we have
class voCOMXCoreLoader : public voCOMXBaseObject
{
public:
    voCOMXCoreLoader(void);
    virtual ~voCOMXCoreLoader(void);

	virtual void		  	SetCallBack (VOMECallBack pCallBack, OMX_PTR pUserData);
	virtual OMX_ERRORTYPE 	SetCoreFile (OMX_STRING pFile, OMX_VO_LIB_OPERATOR * pLibOP, OMX_STRING pPrefix = NULL, OMX_STRING pWorkPath = NULL);

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
	char			m_szCoreFile[256];
	OMX_BOOL		m_bInit;

	char **			m_ppFuncName;
	int				m_nFuncCount;

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
	OMX_SETWORKINGPATH		m_fSetWorkingPath;

	VOMECallBack			m_pCallBack;
	OMX_PTR					m_pUserData;

	OMX_VO_LIB_OPERATOR *	m_pLibOP;

	static voCoreRefMgr		g_CoreRefMgr;
};

#endif //__voCOMXCoreLoader_H__
