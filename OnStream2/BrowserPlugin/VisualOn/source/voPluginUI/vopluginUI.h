	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voPlugInUI.h

	Contains:	data type define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-01-10		JBF			Create file

*******************************************************************************/
#ifndef __voPlugInUI_H__
#define __voPlugInUI_H__

#include <tchar.h>
#include "voOnStreamType.h"
#include "CvoOnStreamMP.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32
#	define VOPUI_API __cdecl
#else
#	define VOPUI_API
#endif //_WIN32

/**
 * PlugIn UI Control Init param
 */
typedef struct voUIIINTPARAM
{
	// The instance handle
	void *						hInst;

	// The View pointer
	void *						hView;

	// The user data
	void *						hUserData;

	// Player Func Set
	CvoOnStreamMP *				pOSMPPlayer;

	// Engine handle 
	void *						hHandle;

	// Notify the command to client
	int		(VOPUI_API * NotifyCommand) (void * pUserData, int nID, void * pValue1, void * pValue2);

	int							nBrowserType; // 0: IE; 1: Chrome; 2: FF

	TCHAR           szWorkingPath[MAX_PATH];

	voUIIINTPARAM()
	{
		hInst = NULL;
		hView = NULL;
		hUserData = NULL;
		pOSMPPlayer = NULL;
		hHandle = NULL;
		NotifyCommand = NULL;
		nBrowserType = 0;
		_tcscpy(szWorkingPath,_T(""));
	}
} VOPUI_INIT_PARAM;


/**
 * PlugIn UI function set
 */
typedef struct
{
	/**
	 * Init the Overlay UI
	 * \param pInitParam	[IN] Init Param
	 * \param ppHandle	[OUT] Return the Overlay UI handle
	 * \retval 0 Succeeded.
	 */
	int (VOPUI_API * Init) (VOPUI_INIT_PARAM * pInitParam, void ** phHandle);

	/**
	 * Uninit the Overlay UI
	 * \param hHandle	[IN] The Overlay UI handle which return by Init.
	 * \retval 0 Succeeded.
	 */
	int (VOPUI_API * Uninit) (void * hHandle);

	/**
	 * Get the view handle
	 * \param hHandle		[IN] The Overlay UI handle which return by Init.
	 * \retval 0 Succeeded.
	 */
	void * (VOPUI_API * GetView) (void * hHandle);

	/**
	 * Show or hide the UI View.
	 * \param hHandle		[IN] The Overlay UI handle which return by Init.
	 * \param bShow			[IN] Show or Hide view.
	 * \param bFullScreen	[IN] Show full screen or not.
	 * \retval 0 Succeeded.
	 */
	int (VOPUI_API * ShowFullScreen) (void * hHandle);

	/**
	 * Show or Hide the controls
	 * \param hHandle		[IN] The Overlay UI handle which return by Init.
	 * \param bShow			[IN] Show or Hide controls.
	 * \retval 0 Succeeded.
	 */
	int (VOPUI_API * ShowControls) (void * hHandle, bool bShow);

	/**
	 * Set Param
	 * \param hHandle		[IN] The Overlay UI handle which return by Init.
	 * \param nID			[IN] The param ID.
	 * \param pParam		[IN] The param value depend on the ID.
	 * \retval 0 Succeeded.
	 */
	int (VOPUI_API * SetParam) (void * hHandle, int nID, void * pParam);

	/**
	 * Get Param
	 * \param hHandle		[IN] The Overlay UI handle which return by Init.
	 * \param nID			[IN] The param ID.
	 * \param pParam		[Out] The param value depend on the ID.
	 * \retval 0 Succeeded.
	 */
	int (VOPUI_API * GetParam) (void * hHandle, int nID, void * pParam);

} VOPUI_OVERLAY_API;

// get the overlay UI API function set.
int		voGetPlugInUIAPI (VOPUI_OVERLAY_API * pAPI, int uFlag);
typedef int (* VOGETPLUGINUIAPI) (VOPUI_OVERLAY_API * pAPI, int uFlag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voPlugInUI_H__
