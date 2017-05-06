
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		Lin2WinFile.h

	Contains:	Lin2WinFile header file

    Aim:        Make the linux file operate into Win file operate interface

    Warning:    Please use the CloseHandle to free the memory used by it

	Written by:	XiaGuangTai

	Change History (most recent first):
	2008-01-11		gtxia			Create file

*******************************************************************************/

#ifndef _LIN_2_WIN_FILE_H
#define _LIN_2_WIN_FILE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "vocrstypes.h"
	
HANDLE CreateFile(
  IN      LPCTSTR lpFileName,
  IN      DWORD dwDesiredAccess,
  IN      DWORD dwShareMode,
  IN_OPT  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  IN      DWORD dwCreationDisposition,
  IN      DWORD dwFlagsAndAttributes,
  IN_OPT  HANDLE hTemplateFile
);

BOOL WriteFile(
  IN         HANDLE hFile,
  IN         LPCVOID lpBuffer,
  IN         DWORD nNumberOfBytesToWrite,
  OUT_OPT    LPDWORD lpNumberOfBytesWritten,
  INOUT_OPT  LPOVERLAPPED lpOverlapped
);
	
BOOL ReadFile(
  IN         HANDLE hFile,
  OUT        LPVOID lpBuffer,
  IN         DWORD nNumberOfBytesToRead,
  OUT_OPT    LPDWORD lpNumberOfBytesRead,
  INOUT_OPT  LPOVERLAPPED lpOverlapped
);
	

DWORD SetFilePointer(
  IN         HANDLE hFile,
  IN         LONG lDistanceToMove,
  INOUT_OPT  PLONG lpDistanceToMoveHigh,
  IN         DWORD dwMoveMethod
);

BOOL SetEndOfFile(IN  HANDLE hFile, IN DWORD size);

UINT GetTempFileName(
  IN   LPCTSTR lpPathName,
  IN   LPCTSTR lpPrefixString,
  IN   UINT uUnique,
  OUT  LPTSTR lpTempFileName
);
	
DWORD GetFileSize(
  IN       HANDLE hFile,
  OUT_OPT  LPDWORD lpFileSizeHigh
);

BOOL DeleteFile(
  IN  LPCTSTR lpFileName
);


BOOL CreateDirectory(
  IN_OPT  LPCTSTR lpPathName,
  IN      LPSECURITY_ATTRIBUTES lpSecurityAttributes
);	
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif // _LIN_2_WIN_FILE_H

