#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "Lin2WinBase.h"
#include "Lin2WinFile.h"

HANDLE CreateFile(LPCTSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,HANDLE hTemplateFile)
{
	if(lpFileName)
	{
		int nFd = -1;
		int nFlag = 0, nMode =0;
		
		if((GENERIC_READ|GENERIC_WRITE) == dwDesiredAccess )
		{
			nFlag |= O_RDWR;
		}
		else if(GENERIC_READ == dwDesiredAccess)
		{
			nFlag |= O_RDONLY;
		}
		else if(GENERIC_WRITE == dwDesiredAccess)
		{
			nFlag |= O_RDWR;
		}
		
		if(dwCreationDisposition == CREATE_NEW)
			nFlag |= O_EXCL;
		if(dwCreationDisposition == CREATE_ALWAYS)
			nFlag |= O_CREAT;
		if(dwCreationDisposition == TRUNCATE_EXISTING)
			nFlag |= O_TRUNC;
		if(dwCreationDisposition == OPEN_ALWAYS)
			nFlag |= O_CREAT | O_APPEND;
		nMode = 0640;
		
		nFd = open(lpFileName, nFlag, nMode);
#if defined(_DEBUG)
		if(nFd ==-1)
		{
			printf("can not open the file the reason is %s\n", strerror(errno));
			return NULL;
		}
#endif
		lh_info* pInfo = (lh_info*)malloc(sizeof(struct lh_info));
		memset(pInfo, 0, sizeof(lh_info));
		pInfo->mType = LH_FILE;
		pInfo->mData.mFd = nFd;
		return pInfo;
	}
	return NULL;
}

static lh_info* get_handle(HANDLE hFile)
{
	lh_info* pInfo = (lh_info*)(hFile);
	assert(pInfo->mType == LH_FILE);
	return pInfo;
} 
BOOL WriteFile(HANDLE hFile,LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,LPOVERLAPPED lpOverlapped)
{
	if(hFile)
	{
		lh_info* pInfo = get_handle(hFile);
		*lpNumberOfBytesWritten = write(pInfo->mData.mFd, lpBuffer, nNumberOfBytesToWrite);
		return (*lpNumberOfBytesWritten>=0);
	}
	return FALSE;
}
	
BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead,  LPOVERLAPPED lpOverlapped )
{
	if(hFile)
	{
		lh_info* pInfo = get_handle(hFile);
		*lpNumberOfBytesRead = read(pInfo->mData.mFd, lpBuffer, nNumberOfBytesToRead);
		return (*lpNumberOfBytesRead >= 0);
	}
	return FALSE;
}

	

DWORD SetFilePointer(HANDLE hFile, LONG lDistanceToMove,  PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
	lh_info* pInfo = get_handle(hFile);
	int whence = SEEK_SET;
	if(dwMoveMethod == FILE_BEGIN)
		whence = SEEK_SET;
	if(dwMoveMethod == FILE_CURRENT)
		whence = SEEK_CUR;
	if(dwMoveMethod == FILE_END)
		whence = SEEK_END;
	// at present only used the 2^32 operation, comment by gtxia 
	return lseek(pInfo->mData.mFd, lDistanceToMove, whence);
}

BOOL SetEndOfFile(HANDLE hFile, DWORD size)
{
	lh_info* pInfo = get_handle(hFile);
	
	return (ftruncate(pInfo->mData.mFd, size) == 0);
}

UINT GetTempFileName(LPCTSTR lpPathName,LPCTSTR lpPrefixString,UINT uUnique,LPTSTR lpTempFileName)
{
	char* pName = tempnam(lpPathName, lpPrefixString);
	if(lpTempFileName && pName)
	{
		strcpy(lpTempFileName, pName);
		free(pName);
	}
	return 0;
}
	
DWORD GetFileSize(HANDLE hFile,LPDWORD lpFileSizeHigh)
{
	if(hFile)
	{
		lh_info* pInfo = get_handle(hFile);
		struct stat buf;
		fstat(pInfo->mData.mFd, &buf);
		return buf.st_size;
	}
	return 0;
}

BOOL DeleteFile(LPCTSTR lpFileName)
{
	if(lpFileName)
	{
		return (remove(lpFileName)==0);
	}
	return FALSE;
	
}

BOOL CreateDirectory(LPCTSTR lpPathName,LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	return (mkdir(lpPathName, 0640) == 0);
}
	
