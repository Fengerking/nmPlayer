	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXMemory.c

	Contains:	memory operator implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-15		JBF			Create file

*******************************************************************************/
#ifdef _WIN32
#include "windows.h"
#endif // _WIN32

#include "voOMXMemory.h"

#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "voLog.h"

#ifdef _LINUX_ANDROID
#include <android/log.h>
#endif // _LINUX_ANDROID

#ifndef _LINUX
#pragma warning (disable : 4013)
#endif

//#define _VOOMXMEM_LEAK_CHECK

#ifdef _VOOMXMEM_LEAK_CHECK
#define			VOOMXMEM_MAXID		32
#define			VOOMXMEM_MAXTIME	4096

static int		g_voOMXMemInit = 0;
static int		g_voOMXMemModuleID[VOOMXMEM_MAXID];
static void *	g_voOMXMemBuffInfo[VOOMXMEM_MAXID][VOOMXMEM_MAXTIME];
static int		g_voOMXMemSizeInfo[VOOMXMEM_MAXID][VOOMXMEM_MAXTIME];

OMX_U32 voOMXMemRecInfo (int nID, void * pBuffer, int nSize, int nAlloc)
{
	int i = 0;
	int j = 0;
	
	if (nAlloc == 1)
	{
		for (i = 0; i < VOOMXMEM_MAXID; i++)
		{
			if (g_voOMXMemModuleID[i] == -1 || g_voOMXMemModuleID[i] == nID)
			{
				g_voOMXMemModuleID[i] = nID;
				for (j = 0; j < VOOMXMEM_MAXTIME; j++)
				{
					if (g_voOMXMemBuffInfo[i][j] == 0)
					{
						g_voOMXMemBuffInfo[i][j] = pBuffer;
						g_voOMXMemSizeInfo[i][j] = nSize;

						if (j == 22)
							j = 22;

						break;
					}
				}
				break;
			}
		}
	}
	else if (nAlloc == 0)
	{
		for (i = 0; i < VOOMXMEM_MAXID; i++)
		{
			if (g_voOMXMemModuleID[i] == nID)
			{
				for (j = 0; j < VOOMXMEM_MAXTIME; j++)
				{
					if (g_voOMXMemBuffInfo[i][j] == pBuffer)
					{
						g_voOMXMemBuffInfo[i][j] = NULL;
						g_voOMXMemSizeInfo[i][j] = 0;
						break;
					}
				}
				break;
			}
		}
	}

	return 0;
}
#endif // _VOOMXMEM_LEAK_CHECK

OMX_S32 voOMXMemShowStatus (void)
{
#ifdef _VOOMXMEM_LEAK_CHECK
	int			i = 0;
	int			j = 0;
	char		szDebug[256];

	for (i = 0; i < VOOMXMEM_MAXID; i++)
	{
		for (j = 0; j < VOOMXMEM_MAXTIME; j++)
		{
			if (g_voOMXMemBuffInfo[i][j] != NULL)
			{
				sprintf (szDebug, "****** OMX Memory Leak! Index: %04d Moudle ID: 0X%08X, Buffer: 0X%08X, Size: %d \r\n", j, g_voOMXMemModuleID[i], g_voOMXMemBuffInfo[i][j], g_voOMXMemSizeInfo[i][j]);
#ifdef _WIN32
				TCHAR wzLog[256];
				MultiByteToWideChar (CP_ACP, 0, szDebug, -1, wzLog, sizeof (wzLog));
				OutputDebugString (wzLog);
#elif defined _LINUX_ANDROID
				LOGE (szDebug);
#endif // _WIN32
			}
		}
	}

	g_voOMXMemInit = 0;
#endif // _VOOMXMEM_LEAK_CHECK

	return 0;
}


OMX_PTR voOMXMemAlloc (OMX_U32 uSize)
{
	if (uSize >= 0X80000000)
		return NULL;

	OMX_PTR pBuffer = malloc (uSize);

#ifdef _VOOMXMEM_LEAK_CHECK
	if (g_voOMXMemInit == 0)
	{
		int i = 0;
		int j = 0;
		for (i = 0; i < VOOMXMEM_MAXID; i++)
		{
			g_voOMXMemModuleID[i] = -1;
			for (j = 0; j < VOOMXMEM_MAXTIME; j++)
			{
				g_voOMXMemBuffInfo[i][j] = NULL;
				g_voOMXMemSizeInfo[i][j] = 0;
			}
		}

		g_voOMXMemInit = 1;
	}

	voOMXMemRecInfo (0, pBuffer, uSize, 1);
#endif // _VOOMXMEM_LEAK_CHECK

	return pBuffer;
}

OMX_U32 voOMXMemFree (OMX_PTR pMem)
{
	free (pMem);

#ifdef _VOOMXMEM_LEAK_CHECK
	voOMXMemRecInfo (0, pMem, 0, 0);
#endif // _VOOMXMEM_LEAK_CHECK

	return 0;
}

OMX_U32	voOMXMemSet (OMX_PTR pBuff, OMX_U8 uValue, OMX_U32 uSize)
{
	memset (pBuff, uValue, uSize);

	return 0;
}

OMX_U32	voOMXMemCopy (OMX_PTR pDest, OMX_PTR pSource, OMX_U32 uSize)
{
	memcpy (pDest, pSource, uSize);

	return 0;
}

OMX_U32	voOMXMemCheck (OMX_PTR pBuffer, OMX_U32 uSize)
{
	return 0;
}

OMX_S32 voOMXMemCompare (OMX_PTR pBuffer1, OMX_PTR pBuffer2, OMX_U32 uSize)
{
	return memcmp(pBuffer1, pBuffer2, uSize);
}
