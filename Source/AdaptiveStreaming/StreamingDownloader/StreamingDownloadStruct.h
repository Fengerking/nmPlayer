/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		StreamingDownloadStruct.h

	Contains:	StreamingDownloadStruct class file

	Written by:	Aiven

	Change History (most recent first):
	2013-09-13		Aiven			Create file

*******************************************************************************/
#ifndef _STREAMINGDOWNLOAD_H_
#define _STREAMINGDOWNLOAD_H_
#include "voSource2.h"
#include "voAdaptiveStreamParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


#define DOWNLOADER_EVENT_BASE						0xF0000000
#define DOWNLOADER_EVENT_END						(DOWNLOADER_EVENT_BASE|0x00000001)
#define DOWNLOADER_EVENT_MANIFESTUPDATE			(DOWNLOADER_EVENT_BASE|0x00000002)
#define DOWNLOADER_EVENT_DOWNLOADCHUNK_FAIL	(DOWNLOADER_EVENT_BASE|0x00000003)
#define DOWNLOADER_EVENT_WRITECHUNK_FAIL			(DOWNLOADER_EVENT_BASE|0x00000004)
#define DOWNLOADER_EVENT_GENERATEMANIFEST_FAIL	(DOWNLOADER_EVENT_BASE|0x00000005)
#define DOWNLOADER_EVENT_WRITEMANIFEST_FAIL		(DOWNLOADER_EVENT_BASE|0x00000006)

#define CHUNK_FLAG_BASE								0x00000000
#define CHUNK_FLAG_STATUS_DOWNLOAD_OK			(CHUNK_FLAG_BASE|0x0001)
#define CHUNK_FLAG_STATUS_DOWNLOAD_FAIL			(CHUNK_FLAG_BASE|0x0002)
#define CHUNK_FLAG_STATUS_END						(CHUNK_FLAG_BASE|0x0004)

#define CHUNK_FLAG_STATUS_MANIFEST					(CHUNK_FLAG_BASE|0x8000)		//the flag to generate the manifest

#define CHUNK_FLAG_TYPE_AUDIO						(CHUNK_FLAG_BASE|0x00010000)
#define CHUNK_FLAG_TYPE_VIDEO						(CHUNK_FLAG_BASE|0x00020000)
//#define CHUNK_FLAG_TYPE_AUDIOVIDEO				(CHUNK_FLAG_BASE|0x00040000)
#define CHUNK_FLAG_TYPE_HEADDATA					(CHUNK_FLAG_BASE|0x00080000)
#define CHUNK_FLAG_TYPE_SUBTITLE					(CHUNK_FLAG_BASE|0x00100000)
#define CHUNK_FLAG_TYPE_SEGMENTINDEX				(CHUNK_FLAG_BASE|0x00200000)
#define CHUNK_FLAG_TYPE_MAX						(CHUNK_FLAG_TYPE_AUDIO|CHUNK_FLAG_TYPE_VIDEO|CHUNK_FLAG_TYPE_HEADDATA|CHUNK_FLAG_TYPE_SUBTITLE)


#define FILENAME_LENGTH								256

typedef struct
{
	VO_CHAR		szRootUrl[MAXURLLEN];							/*!< The URL of parent playlist */
	VO_CHAR		szUrl[MAXURLLEN];								/*!< The URL of the playlist , maybe relative URL */
	
	VO_PBYTE 	pManifestData;
	VO_U32		nLength;
//	VO_PCHAR	pName;		
}MANIFEST_INFO;


typedef struct
{
	MANIFEST_INFO**	ppInfo;
	VO_U32			nCount;
	VO_CHAR			strMaster[FILENAME_LENGTH];								/*!< The master name of hls playlist */	
}MANIFEST_GROUP;


typedef struct
{
	VO_ADAPTIVESTREAMPARSER_CHUNK* pChunk;
	VO_CHAR	DownloadUrl[MAXURLLEN];
	VO_CHAR LoaclUrl[MAXURLLEN];
	VO_CHAR	FileName[FILENAME_LENGTH];
	VO_U32	Flag;	
	VO_PTR	pReserve;
}CHUNKINFO;


typedef enum
{
	CURRENT_TYPE	=0,
	REWIND_TYPE,
	FORWARD_TYPE
}DOWNLOAD_TYPE;

typedef struct
{
	VO_PBYTE	pBuffer;
	VO_U64	nSize;
}CHUNK_BUFFER;

#ifdef _VONAMESPACE
}
#endif

#endif//_STREAMINGDOWNLOAD_H_


