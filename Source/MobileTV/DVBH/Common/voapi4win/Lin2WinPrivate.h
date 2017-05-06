
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		Lin2WinBase.h

	Contains:	Lin2WinBase header file

    Aim:        provide some private interface used by this library

	Written by:	XiaGuangTai

	Change History (most recent first):
	2008-01-11		gtxia			Create file

*******************************************************************************/
#ifndef _LIN_2_WIN_PRIVATE_H
#define _LIN_2_WIN_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "vocrstypes.h"

	
BOOL _close_event_handle(HANDLE hEvent);
BOOL _close_thread_handle(HANDLE hThread);
BOOL _close_file_handle(HANDLE hFile);	

DWORD _wait_event_handle(HANDLE hEvt, DWORD  dwMillSeconds);
DWORD _wait_thread_handle(HANDLE hThread, DWORD dwMillSeconds);
	
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
	
#endif // _LIN_2_WIN_PRIVATE_H
	
