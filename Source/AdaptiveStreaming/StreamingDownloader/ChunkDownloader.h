/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		TrunkDownloader.cpp

	Contains:	CTrunkDownloader class file

	Written by:	Aiven

	Change History (most recent first):
	2013-09-02		Aiven			Create file

*******************************************************************************/


#ifndef _TRUNK_DOWNLOADER_H_
#define _TRUNK_DOWNLOADER_H_

#include "voSource2.h"
#include "vo_thread.h"
#include "voSource2_IO.h"
#include "StreamingDownloadStruct.h"
#include "DownloadList.h"
#include "ManifestWriter.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CStreamingDownloader;

class CChunkDownloader:public vo_thread
{
public:
	CChunkDownloader(CDownloadList* pDownloadlist, CStreamingDownloader* pStreamingDownloader, VO_SOURCE2_EVENTCALLBACK* pEventCallbackFunc, VO_SOURCE2_IO_API* pIO,VO_PTCHAR pLocalDir);
	~CChunkDownloader();
	
	VO_U32 Start();
	VO_U32 Stop();
protected:
	void thread_function();
	VO_U32 GetTrunkToDownload(CHUNKINFO** ppTrunkinfo, DOWNLOAD_TYPE type);
	VO_U32 DownloadThread();
	VO_BOOL IsNeedToDownload(CHUNKINFO* pTrunkinfo);
	VO_U32 DownloadFileTrunk(CHUNKINFO* pTrunkinfo, CHUNK_BUFFER* pTrunkbuffer);
	VO_U32 WriteFileTrunk(CHUNKINFO* pTrunkinfo, CHUNK_BUFFER* pTrunkbuffer);
	VO_BOOL IsNeedToWriteManifest(CHUNKINFO* pTrunkinfo);
	VO_U32 GenerateManifest(MANIFEST_GROUP** ppManifestGroup, VO_BOOL bEnd);
	VO_U32 WriteManifest(MANIFEST_GROUP* pManifestGroup);
	VO_U32 ProcessManifest(CHUNKINFO* pChunkinfo, VO_BOOL bEnd=VO_FALSE);
	VO_U32 ProcessFileTrunk(CHUNKINFO* pChunkinfo);
private:
	VO_SOURCE2_IO_API * 				m_pIO;
	CManifestWriter*					m_pManifestWriter;
	CDownloadList*					m_pDownloadList;
	CStreamingDownloader* 			m_pStreamingDownloader;
	
	VO_SOURCE2_EVENTCALLBACK*		m_pEventCallback;
	VO_TCHAR*						m_pLocalDir;

	VO_U32							m_nLastUpdateTime;
	VO_U32							m_nInterval;

	VO_U32							m_nDuration;

	VO_BOOL							m_bDownloadCancel;
};

#ifdef _VONAMESPACE
}
#endif

#endif//_TRUNK_DOWNLOADER_H_


