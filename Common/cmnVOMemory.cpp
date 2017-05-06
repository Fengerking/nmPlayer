	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		cmnMemory.c

	Contains:	memory operator implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-15		JBF			Create file

*******************************************************************************/
#ifdef _WIN32
#include "windows.h"
#endif // _WIN32

#ifdef __SYMBIAN32__
#include <stdlib.h>
#elif defined _IOS
#include <stdlib.h>
#elif defined _MAC_OS
#include <stdlib.h>
#else
#include <malloc.h>
#endif // __SYMBIAN32__

#include "cmnMemory.h"
#include "voString.h"
#include "voCMutex.h"

#if defined LINUX
#include <string.h>
#endif

#define LOG_TAG "cmnVOMemory"
#include "voLog.h"

//#define _VOMEM_LEAK_CHECK

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

VO_MEM_OPERATOR		g_memOP;

#ifdef _VOMEM_LEAK_CHECK
#define			VOMEM_MAXID		128
#define			VOMEM_MAXTIME	4096

static int		g_voMemInit = 0;
static int		g_voMemModuleID[VOMEM_MAXID];
static void *	g_voMemBuffInfo[VOMEM_MAXID][VOMEM_MAXTIME];
static int		g_voMemSizeInfo[VOMEM_MAXID][VOMEM_MAXTIME];


VO_U32 cmnMemRecInfo (int nID, void * pBuffer, int nSize, int nAlloc)
{
	int i = 0;
	int j = 0;
	int nRC = 0;
	
	if (nAlloc == 1)
	{
		for (i = 0; i < VOMEM_MAXID; i++)
		{
			if (g_voMemModuleID[i] == -1 || g_voMemModuleID[i] == nID)
			{
				g_voMemModuleID[i] = nID;
				for (j = 0; j < VOMEM_MAXTIME; j++)
				{
					if (g_voMemBuffInfo[i][j] == 0)
					{
						g_voMemBuffInfo[i][j] = pBuffer;
						g_voMemSizeInfo[i][j] = nSize;
						nRC = 1;
						break;
					}
				}
				break;
			}
		}
	}
	else if (nAlloc == 0)
	{
		for (i = 0; i < VOMEM_MAXID; i++)
		{
			if (g_voMemModuleID[i] == nID)
			{
				for (j = 0; j < VOMEM_MAXTIME; j++)
				{
					if (g_voMemBuffInfo[i][j] == pBuffer)
					{
						g_voMemBuffInfo[i][j] = NULL;
						g_voMemSizeInfo[i][j] = 0;
						nRC = 1;
						break;
					}
				}
				break;
			}
		}
	}

	return nRC;
}
#endif // _VOMEM_LEAK_CHECK

VO_S32 cmnMemShowStatus (void)
{
#ifdef _VOMEM_LEAK_CHECK
	int			i = 0;
	int			j = 0;
	char		szDebug[256];
	VO_TCHAR	wzLog[256];


	for (i = 0; i < VOMEM_MAXID; i++)
	{
		for (j = 0; j < VOMEM_MAXTIME; j++)
		{
			if (g_voMemBuffInfo[i][j] != NULL)
			{
				sprintf (szDebug, "****** Memory Leak! Index: %04d Moudle ID: 0X%08X, Buffer: 0X%08X, Size: %d \r\n", j, g_voMemModuleID[i], g_voMemBuffInfo[i][j], g_voMemSizeInfo[i][j]);
#ifdef _WIN32
				MultiByteToWideChar (CP_ACP, 0, szDebug, -1, wzLog, sizeof (wzLog));
				OutputDebugString (wzLog);
#elif defined _LINUX_ANDROID
				LOGE (szDebug);
#endif // _WIN32
			}
		}
	}

	g_voMemInit = 0;
#endif // _VOMEM_LEAK_CHECK

	return 0;
}

VO_U32 cmnMemAlloc (VO_S32 uID,  VO_MEM_INFO * pMemInfo)
{
#ifdef _VOMEM_LEAK_CHECK
	if (g_voMemInit == 0)
	{
		int i = 0;
		int j = 0;
		for (i = 0; i < VOMEM_MAXID; i++)
		{
			g_voMemModuleID[i] = -1;
			for (j = 0; j < VOMEM_MAXTIME; j++)
			{
				g_voMemBuffInfo[i][j] = NULL;
				g_voMemSizeInfo[i][j] = 0;
			}
		}

		g_voMemInit = 1;
	}
#endif // _VOMEM_LEAK_CHECK

	if (!pMemInfo)
		return VO_ERR_INVALID_ARG;

	pMemInfo->VBuffer = malloc (pMemInfo->Size);

#ifdef _VOMEM_LEAK_CHECK
	if (cmnMemRecInfo (uID, pMemInfo->VBuffer, pMemInfo->Size, 1) == 0)
		VOLOGW ("Alloc Mem :cmnMemRecInfo error");

//	if (uID == VO_INDEX_DEC_WMV)
//		VOLOGI ("Alloc Mem : ID: 0X%08X,  Buffer: 0X%08X  Size %d", uID, pMemInfo->VBuffer, pMemInfo->Size);
#endif // _VOMEM_LEAK_CHECK

	return 0;
}

VO_U32 cmnMemFree (VO_S32 uID, VO_PTR pMem)
{
	free (pMem);

#ifdef _VOMEM_LEAK_CHECK
	if (cmnMemRecInfo (uID, pMem, 0, 0) == 0)
		VOLOGW ("Free Mem :cmnMemRecInfo error");

//	if (uID == VO_INDEX_DEC_WMV)
//		VOLOGI ("Free Mem : ID: 0X%08X,  Buffer: 0X%08X", uID, pMem);
#endif // _VOMEM_LEAK_CHECK

	return 0;
}

VO_U32	cmnMemSet (VO_S32 uID, VO_PTR pBuff, VO_U8 uValue, VO_U32 uSize)
{
	memset (pBuff, uValue, uSize);

	return 0;
}

VO_U32	cmnMemCopy (VO_S32 uID, VO_PTR pDest, VO_PTR pSource, VO_U32 uSize)
{
	memcpy (pDest, pSource, uSize);

	return 0;
}

VO_U32	cmnMemCheck (VO_S32 uID, VO_PTR pBuffer, VO_U32 uSize)
{
	return 0;
}

VO_S32 cmnMemCompare (VO_S32 uID, VO_PTR pBuffer1, VO_PTR pBuffer2, VO_U32 uSize)
{
	return memcmp(pBuffer1, pBuffer2, uSize);
}

VO_U32	cmnMemMove (VO_S32 uID, VO_PTR pDest, VO_PTR pSource, VO_U32 uSize)
{
	memmove (pDest, pSource, uSize);

	return 0;
}

VO_S32 cmnMemFillPointer (VO_S32 uID)
{
	g_memOP.Alloc = cmnMemAlloc;
	g_memOP.Free = cmnMemFree;
	g_memOP.Set = cmnMemSet;
	g_memOP.Copy = cmnMemCopy;
	g_memOP.Check = cmnMemCheck;
	g_memOP.Compare = cmnMemCompare;
	g_memOP.Move = cmnMemMove;

	return 0;
}
    
#ifdef _VONAMESPACE
}
#endif
