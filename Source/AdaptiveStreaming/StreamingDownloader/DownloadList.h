/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		DownloadList.h

	Contains:	CDownloadList class file

	Written by:	Aiven

	Change History (most recent first):
	2013-09-02		Aiven			Create file

*******************************************************************************/

#ifndef _DOWNLOAD_LIST_H
#define _DOWNLOAD_LIST_H

#include "voSource2.h"
#include "voAdaptiveStreamParserWrapper.h"
#include "voSource2_IO.h"
#include "list_T.h"
#include "StreamingDownloadStruct.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


#define LENGTH_64	64
class CDownloadList
{
public:
	CDownloadList(VO_U32 adaptivestream_type , VO_SOURCE2_LIB_FUNC * pLibOp, VO_TCHAR *pWorkPath, VO_LOG_PRINT_CB * pVologCB);
	~CDownloadList();

	VO_U32 Open(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_SOURCE2_EVENTCALLBACK* pUpdateCallback);
	VO_U32 Close();
	
	VO_U32 GetProgramInfo(VO_DATASOURCE_PROGRAM_INFO** ppProgramInfo);
	VO_U32 SelectStream(VO_U32 nStreamID);
	VO_U32 SelectTrack(VO_U32 nTrackID, VO_SOURCE2_TRACK_TYPE nType);
	VO_U32 GetDuration(VO_U64* pDuration);
	
	VO_U32 GenerateDownloadList();
	VO_U32 GetChunkList(list_T<CHUNKINFO*>** ppChunkList);
	VO_U32 GetTrunkToDownload(CHUNKINFO** ppTrunkinfo, DOWNLOAD_TYPE type);
	VO_U32 GetManifestID(VO_CHAR** pManifestID);

protected:
	VO_U32 PreProcessURL(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData);
	VO_U32 ConverToFlag(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE type);
	VO_U32 PushTrunkToDownloadList(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE type, VO_BOOL isOnlyonTrack);
	VO_U32 AddWriteManifestFlag(VO_BOOL bEndFlag);

	VO_U32 GetMinTrackDurationOfStream(VO_U64* nDuration);
	VO_U32 GetTrackDuration(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE type, VO_U64* pDuration);
	VO_U32 GenerateDownloadPath(CHUNKINFO*	pChunkinfo);
	VO_U32 GenerateFileName(CHUNKINFO*	pChunkinfo);
	VO_U32 GenerateTheMianfestID(VO_PCHAR pSource);
	VO_U32 GenerateFileSuffix(VO_PCHAR* ppSuffix);
	VO_U32 PrintChunkInfo(list_T<CHUNKINFO*>* pChunklist);
	VO_U32 ReleaseChunkList(list_T<CHUNKINFO*>*	pChunklist);
	VO_U32 BKDRHash(VO_CHAR* pStr);
private:
	voAdaptiveStreamParserWrapper*		m_pAsp;
	VO_SOURCE2_IO_API * 				m_pIO;

	VO_CHAR							m_sManifestID[LENGTH_64];

	list_T<CHUNKINFO*> 				m_nChunklist;	
	list_T<CHUNKINFO*>::iterator			m_nDownloadIterator;
};

#ifdef _VONAMESPACE
}
#endif


#endif//_DOWNLOAD_LIST_H



