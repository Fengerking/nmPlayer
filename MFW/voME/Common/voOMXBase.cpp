	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXBase.cpp

	Contains:	component api cpp file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <string.h>

#if defined(_WIN32)
#  include <tchar.h>
#endif

#include <OMX_Component.h>

#include "voOMXBase.h"
#include "voOMXMemory.h"

OMX_ERRORTYPE voOMXBase_CheckHeader (OMX_PTR pHeader, OMX_U32 nSize)
{
	if (pHeader == NULL)
	  return OMX_ErrorBadParameter;

//	if(*((OMX_U32*)pHeader) != nSize)
//		return OMX_ErrorBadParameter;

	OMX_VERSIONTYPE* ver;
	ver = (OMX_VERSIONTYPE*)((char*)pHeader + sizeof(OMX_U32));
/*
	if(ver->s.nVersionMajor != OMX_VERSION_MAJOR)// || ver->s.nVersionMinor != OMX_VERSION_MINOR) 
		return OMX_ErrorVersionMismatch;
*/
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voOMXBase_SetHeader (OMX_PTR pHeader, OMX_U32 nSize, OMX_VERSIONTYPE * pVer)
{
	if (pHeader == NULL)
		return OMX_ErrorBadParameter;

	voOMXMemSet (pHeader, 0, nSize);

	OMX_VERSIONTYPE* ver = (OMX_VERSIONTYPE*)((char*)pHeader + sizeof(OMX_U32));
	*((OMX_U32*)pHeader) = nSize;

	if (pVer == NULL)
	{
		ver->s.nVersionMajor = OMX_VERSION_MAJOR;
		ver->s.nVersionMinor = OMX_VERSION_MINOR;
		ver->s.nRevision = OMX_VERSION_REVISION;
		ver->s.nStep = OMX_VERSION_STEP;
	}
	else
	{
		ver->s.nVersionMajor = pVer->s.nVersionMajor;
		ver->s.nVersionMinor =  pVer->s.nVersionMinor;
		ver->s.nRevision = pVer->s.nRevision;
		ver->s.nStep = pVer->s.nStep;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voOMXBase_CopyBufferHeader (OMX_BUFFERHEADERTYPE * pDest, OMX_BUFFERHEADERTYPE * pSour)
{
	if (pDest == NULL || pSour == NULL)
		return OMX_ErrorBadParameter;

	pDest->nFilledLen = pSour->nFilledLen;

	pDest->hMarkTargetComponent = pSour->hMarkTargetComponent;
	pDest->pMarkData = pSour->pMarkData;

	pDest->nTimeStamp = pSour->nTimeStamp;
	pDest->nTickCount = pSour->nTickCount;

	pDest->nFlags = pSour->nFlags;

	pDest->nOffset = pSour->nOffset;

	return OMX_ErrorNone;
}

