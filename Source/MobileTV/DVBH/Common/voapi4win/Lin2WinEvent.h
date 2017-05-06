
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		Lin2WinEvent.h

	Contains:	Lin2WinEvent header file

    Aim:        Make the linux  imitate Win Event operate interface

    Warning:    Please use the CloseHandle to free the memory used by it

	Written by:	XiaGuangTai

	Change History (most recent first):
	2008-01-11		gtxia			Create file

*******************************************************************************/

#ifndef _LIN_2_WIN_EVENT_H
#define _LIN_2_WIN_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "vocrstypes.h"
	
HANDLE CreateEvent(
  IN_OPT  LPSECURITY_ATTRIBUTES lpEventAttributes,
  IN      BOOL bManualReset,
  IN      BOOL bInitialState,
  IN_OPT  LPCTSTR lpName
);

BOOL SetEvent(
 IN  HANDLE hEvent
);
	
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
	
#endif // _LIN_2_WIN_EVENT_H


	
