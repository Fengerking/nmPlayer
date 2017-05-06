
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		Lin2WinThread.h

	Contains:	Lin2WinThread header file

    Aim:        Make the linux Thread operate into Win Thread operate interface

    Warning:    Please use the CloseHandle to free the memory used by it

	Written by:	XiaGuangTai

	Change History (most recent first):
	2008-01-11		gtxia			Create file

*******************************************************************************/

#ifndef _LIN_2_WIN_THREAD_H
#define _LIN_2_WIN_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "vocrstypes.h"

/* Please use the CloseHandle to free the memory used by it.
 */
HANDLE CreateThread(
  IN_OPT   LPSECURITY_ATTRIBUTES lpThreadAttributes,
  IN       SIZE_T dwStackSize,
  IN       LPTHREAD_START_ROUTINE lpStartAddress,
  IN_OPT   LPVOID lpParameter,
  IN       DWORD dwCreationFlags,
  OUT_OPT  LPDWORD lpThreadId,
  IN       int     nPriority = 0
);

DWORD GetThreadId(IN  HANDLE Thread);
	
DWORD GetCurrentThreadId(void);
	
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif // _LIN_2_WIN_THREAD_H


	
