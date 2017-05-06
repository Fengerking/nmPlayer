	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXPipe.c

	Contains:	memory operator implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-15		JBF			Create file

*******************************************************************************/
#include "voOMXPipe.h"

#ifdef _WIN32
#include <windows.h>
#elif defined LINUX
#  include <errno.h>
#  include <stdio.h>
#  include <unistd.h>
#  include <wchar.h>
#  include <string.h>
#endif // _WIN32

#include <stdlib.h>

CPresult voOMXPipeOpen (CPhandle* hContent, CPstring szURI, CP_ACCESSTYPE eAccess )
{
	OMX_PTR pHandle = NULL;

#ifdef _WIN32
	if (eAccess == CP_AccessRead)
		pHandle = CreateFile((LPCWSTR)szURI, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, (DWORD) 0, NULL);
	else if (eAccess == CP_AccessWrite)
		pHandle = CreateFile((LPCWSTR)szURI, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, (DWORD) 0, NULL);
	else
		pHandle = CreateFile((LPCWSTR)szURI, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (DWORD) 0, NULL);

	if (pHandle == INVALID_HANDLE_VALUE)
		return -1;

#elif defined LINUX
	FILE* pf = NULL;
	char* filename = szURI;
	if (eAccess == CP_AccessRead)
		pf = fopen(filename, "rb");
	else if (eAccess == CP_AccessWrite)
		pf = fopen(filename, "wb");
	else 
		pf = fopen(filename, "a+b");

	if (pf == NULL)
		return -1;

	pHandle = (VO_PTR)pf;
#endif

	*hContent = pHandle;

	return 0;
}

CPresult voOMXPipeClose (CPhandle hContent )
{
	return 0;
}

CPresult voOMXPipeCreate (CPhandle *hContent, CPstring szURI )
{
	return 0;
}

CPresult voOMXPipeCheckAvailableBytes (CPhandle hContent, CPuint nBytesRequested, CP_CHECKBYTESRESULTTYPE *eResult )
{
	if (hContent == 0)
		return 0;

#ifdef _WIN32
	return GetFileSize (hContent, NULL);
#elif defined LINUX

	VO_S32 r0 = 0, r = 0;
	//struct stat st;
	//r = fstat(fileno((FILE*)hContent), &st);   // not be supported by some devices
	r0 = ftell((FILE*)hContent);

	r = fseek((FILE*)hContent, 0L, SEEK_END);
	if (-1 == r)
		return -1;

	r = ftell((FILE*)hContent);
	if (-1 == r)
		return -1;

	fseek((FILE*)hContent, r0, SEEK_SET);

	return r;
#endif
}

CPresult voOMXPipeSetPosition (CPhandle  hContent, CPint nOffset, CP_ORIGINTYPE eOrigin)
{
	if (hContent == 0)
		return 0;

#ifdef _WIN32
	OMX_S32 sMove = FILE_BEGIN;
	if (eOrigin == CP_OriginBegin)
		sMove = FILE_BEGIN;
	else if (eOrigin == CP_OriginCur)
		sMove = FILE_CURRENT;
	else
		sMove = FILE_END;
	return SetFilePointer (hContent, nOffset, NULL, sMove);
#elif defined LINUX
	VO_S32 whence = 0;
	if (eOrigin == CP_OriginBegin)
		whence = SEEK_SET;
	else if (eOrigin == CP_OriginCur)
		whence = SEEK_CUR;
	else 
		whence = SEEK_END;
	return fseek((FILE*)hContent, nOffset, whence);
#endif
}

CPresult voOMXPipeGetPosition (CPhandle hContent, CPuint *pPosition)
{
	return 0;
}

CPresult voOMXPipeRead (CPhandle hContent, CPbyte *pData, CPuint nSize)
{
	if (hContent == 0 || pData == 0)
		return 0;

	OMX_U32 uRead = 0;
#ifdef _WIN32
	ReadFile (hContent, pData, nSize, &uRead, NULL);
#elif defined LINUX 
	uRead = fread(pBuffer, 1, nSize, (FILE*)hContent);
#endif

	return uRead;
}

CPresult voOMXPipeReadBuffer (CPhandle hContent, CPbyte **ppBuffer, CPuint *nSize, CPbool bForbidCopy)
{
	return 0;
}

CPresult voOMXPipeReleaseReadBuffer (CPhandle hContent, CPbyte *pBuffer)
{
	return 0;
}

CPresult voOMXPipeWrite (CPhandle hContent, CPbyte *data, CPuint nSize)
{
	if (hContent == 0 || data == 0)
		return 0;

	OMX_U32 uWrite = 0;
#ifdef _WIN32
	WriteFile (hContent, data, nSize, &uWrite, NULL);
#elif defined LINUX
	uWrite = fwrite(data,1, nSize, (FILE*)hContent);
#endif

	return uWrite;
}

CPresult voOMXPipeGetWriteBuffer (CPhandle hContent, CPbyte **ppBuffer, CPuint nSize)
{
	return 0;
}

CPresult voOMXPipeWriteBuffer (CPhandle hContent, CPbyte *pBuffer, CPuint nFilledSize)
{
	return 0;
}

CPresult voOMXPipeRegisterCallback (CPhandle hContent, CPresult (*ClientCallback)(CP_EVENTTYPE eEvent, CPuint iParam))
{
	return 0;
}

CPresult voOMXPipeFillPointer (CP_PIPETYPE * pPipe)
{
	if (pPipe == NULL)
		return -1;

	pPipe->Open					= voOMXPipeOpen;
	pPipe->Close				= voOMXPipeClose;
	pPipe->Create				= voOMXPipeCreate;
	pPipe->CheckAvailableBytes	= voOMXPipeCheckAvailableBytes;
	pPipe->SetPosition			= voOMXPipeSetPosition;
	pPipe->GetPosition			= voOMXPipeGetPosition;
	pPipe->Read					= voOMXPipeRead;
	pPipe->ReadBuffer			= voOMXPipeReadBuffer;
	pPipe->ReleaseReadBuffer	= voOMXPipeReleaseReadBuffer;
	pPipe->Write				= voOMXPipeWrite;
	pPipe->GetWriteBuffer		= voOMXPipeGetWriteBuffer;
	pPipe->WriteBuffer			= voOMXPipeWriteBuffer;
	pPipe->RegisterCallback		= voOMXPipeRegisterCallback;

	return 0;
}


