	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vompCEngine.h

	Contains:	vompCEngine header file

	Written by:	Bangfei Jin
	Modify by:   Leon Huang

	Change History (most recent first):
	2008-04-30		JBF			Create file
	2011-03-03		Leon        Modify for Linux and Modify the way of loading dll 
*******************************************************************************/
#ifndef __vompCEngine_H__
#define __vompCEngine_H__

#ifdef _WIN32
	#include <windows.h>
	#include <tchar.h>
#endif

#include "CBaseNode.h"
#include "vompAPI.h"
#include "voString.h"
#include "videoEditorType.h"

class vompCEngine : public CBaseNode
{
public:
	// Used to control the image drawing
	vompCEngine (VO_PTR hInst,VO_MEM_OPERATOR * pMemOP);
	virtual ~vompCEngine (void);

	virtual VO_U32		Init ();
	virtual VO_U32		Uninit (void);

	virtual int 		SetDataSource (void * pSource, int nFlag);
	virtual int 		SendBuffer (bool bVideo, VOMP_BUFFERTYPE * pBuffer);
	virtual int 		Run (void);
	virtual int 		Pause (void);
	virtual int 		Stop (void);
	virtual int 		Flush (void);
	virtual int 		GetStatus (VOMP_STATUS * pStatus);
	virtual int 		GetDuration (int * pDuration);
	virtual int 		GetCurPos (int * pCurPos);
	virtual int 		SetCurPos (int nCurPos);
	virtual int 		GetParam (int nID, void * pValue);
	virtual int 		SetParam (int nID, void * pValue);


	static int			OnListener (void * pUserData, int nID, void * pParam1, void * pParam2);
	virtual int			HandleEvent (int nID, void * pParam1, void * pParam2);
	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	int					LoadDll (void);
	VOMP_FUNCSET m_funEnc;
	VO_HANDLE			m_hEnc;


#ifdef _WIN32
	HMODULE			m_hDll;
#elif defined _LINUX
	VO_PTR			m_hDll;
#elif defined _MAC_OS
	VO_PTR			m_hDll;
#elif defined _IOS
	VO_PTR			m_hDll;		
#elif defined __SYMBIAN32__
	RLibrary		m_cLibrary;
	VO_PTR			m_hDll;
#endif // _WIN32
//	void *				m_hPlay;
};

#endif // __vompCEngine_H__

/*
int vompInit (void ** phPlayer, VOMPListener pListener);
typedef int (* VOMPINIT) (void ** phPlayer, VOMPListener pListener);

int vompSetDataSource (void * hPlayer, const char * pURL, bool bSync = true);
typedef int (* VOMPSETDATASOURCE) (void * hPlayer, const char * pURL, bool bSync);

int vompSendBuffer (void * hPlayer, VOMP_BUFFERTYPE * pBuffer);
typedef int (* VOMPSENDBUFFER) (void * hPlayer, VOMP_BUFFERTYPE * pBuffer);

int vompRun (void * hPlayer, void);
typedef int (* VOMPRUN) (void * hPlayer, void);

int vompPause (void * hPlayer);
typedef int (* VOMPPAUSE) (void * hPlayer);

int vompStop (void * hPlayer);
typedef int (* VOMPSTOP) (void * hPlayer);

int vompGetStatus (void * hPlayer, VOMP_STATUS * pStatus);
typedef int (* VOMPGETSTATUS) (void * hPlayer, VOMP_STATUS * pStatus);

int vompGetDuration (void * hPlayer, int * pDuration);
typedef int (* VOMPGETDURATION) (void * hPlayer, int * pDuration);

int vompGetCurPos (void * hPlayer, int * pCurPos);
typedef int (* VOMPGETCURPOS) (void * hPlayer, int * pCurPos);

int vompSetCurPos (void * hPlayer, int nCurPos);
typedef int (* VOMPSETCURPOS) (void * hPlayer, int nCurPos);

int vompGetParam (void * hPlayer, int nID, void * pValue);
typedef int (* VOMPGETPARAM) (void * hPlayer, int nID, void * pValue);

int vompSetParam (void * hPlayer, int nID, void * pValue);
typedef int (* VOMPSETPARAM) (void * hPlayer, int nID, void * pValue);

int vompUninit (void * hPlayer);
typedef int (* VOMPUNINIT) (void * hPlayer);
*/
