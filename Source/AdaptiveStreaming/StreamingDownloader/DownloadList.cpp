/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		DownloadList.cpp

	Contains:	CDownloadList class file

	Written by:	Aiven

	Change History (most recent first):
	2013-09-02		Aiven			Create file

*******************************************************************************/

#include "SDownloaderLog.h"
#include "CSourceIOUtility.h"
#include "DownloadList.h"
#include "voToolUtility.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#if !defined(min)
#define min(x,y) ((x)>=(y)) ? (y):(x)
#endif


CDownloadList::CDownloadList(VO_U32 adaptivestream_type , VO_SOURCE2_LIB_FUNC * pLibOp, VO_TCHAR *pWorkPath, VO_LOG_PRINT_CB * pVologCB)
:m_pAsp(NULL)
,m_pIO(NULL)
{
	m_pAsp = new voAdaptiveStreamParserWrapper(adaptivestream_type , pLibOp, pWorkPath, pVologCB);
}

CDownloadList::~CDownloadList()
{
	if(m_pAsp){
		delete m_pAsp;
		m_pAsp = NULL;
	}

	ReleaseChunkList(&m_nChunklist);
}


VO_U32 CDownloadList::Open(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_SOURCE2_EVENTCALLBACK* pUpdateCallback)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;

	SD_LOGI("+Open");
	ret = PreProcessURL(pData);
	if( ret != VO_RET_SOURCE2_OK ){
		SD_LOGE("PreProcessURL---fail , ret=%lu", ret);
		return ret;
	}
	
	ret = m_pAsp->Init( pData , pUpdateCallback);
	if( ret != VO_RET_SOURCE2_OK ){
		SD_LOGE("m_pAsp->Init---fail , ret=%lu", ret);
		return ret;
	}

	ret = m_pAsp->Open();
	if( ret != VO_RET_SOURCE2_OK ){
		SD_LOGE("m_pAsp->Open---fail , ret=%lu", ret);
		return ret;
	}

	ret = m_pAsp->Start();
	if( ret != VO_RET_SOURCE2_OK ){
		SD_LOGE("m_pAsp->Start---fail , ret=%lu", ret);
		return ret;
	}

	ret = GenerateTheMianfestID(pData->szUrl);

	SD_LOGI("-Open---ret=%lu", ret);
	return ret;
}


VO_U32 CDownloadList::Close()
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;
	SD_LOGI("+Close");

	if(  m_pAsp )
	{
		m_pAsp->Close();
		m_pAsp->UnInit();
	}
	
	SD_LOGI("-Close");
	return ret;
}

VO_U32 CDownloadList::GetProgramInfo(VO_DATASOURCE_PROGRAM_INFO** ppProgramInfo)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;

	m_pAsp->GetProgramInfo(0, ppProgramInfo);

	return ret;

}

VO_U32 CDownloadList::SelectStream(VO_U32 nStreamID)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;
	
	ret = m_pAsp->SelectStream(nStreamID);

	return ret;

}

VO_U32 CDownloadList::SelectTrack(VO_U32 nTrackID, VO_SOURCE2_TRACK_TYPE nType)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;
	
	ret = m_pAsp->SelectTrack(nTrackID, nType);

	return ret;

}

VO_U32 CDownloadList::GetDuration(VO_U64* pDuration)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;
	
	ret = m_pAsp->GetDuration(pDuration);

	return ret;

}


VO_U32 CDownloadList::PreProcessURL(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;


	return ret;
}

VO_U32 CDownloadList::GenerateDownloadList()
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;

	VO_BOOL EndFlag = VO_FALSE;
	VO_U32 flag = 0;
	VO_U32 tmpflag = 0;
	VO_U32 type = 0;
	VO_BOOL IsOnlyOneTrack = VO_FALSE;
	SD_LOGI("+GenerateDownloadList");
	
	ReleaseChunkList(&m_nChunklist);
	VO_U64 TimeStamp =0;
	m_pAsp->Seek(&TimeStamp, VO_ADAPTIVESTREAMPARSER_SEEKMODE_OBSOLUTE);

	while(!EndFlag)
	{
		for(type = VO_SOURCE2_ADAPTIVESTREAMING_AUDIO; type < VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX; type++)
		{
			if(VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO == type){
				continue;
			}
		
			tmpflag = ConverToFlag((VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE)type);

			//continue if the track has end
			if(flag&tmpflag){
				continue;
			}

			IsOnlyOneTrack = (CHUNK_FLAG_TYPE_MAX == (flag|tmpflag)) ? VO_TRUE : VO_FALSE;
			
			if(VO_RET_SOURCE2_OK != PushTrunkToDownloadList((VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE)type, IsOnlyOneTrack)){
				flag |= tmpflag;
			}
		}

		EndFlag = (CHUNK_FLAG_TYPE_MAX == (CHUNK_FLAG_TYPE_MAX&flag)) ? VO_TRUE : VO_FALSE;
		AddWriteManifestFlag(EndFlag);
	}

	m_nDownloadIterator = m_nChunklist.begin();

	PrintChunkInfo(&m_nChunklist);
	SD_LOGI("-GenerateDownloadList");

	return ret;
}

VO_U32 CDownloadList::ConverToFlag(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE type)
{
	VO_U32 flag = 0;

	switch(type)
	{
		case VO_SOURCE2_ADAPTIVESTREAMING_AUDIO:
			flag = CHUNK_FLAG_TYPE_AUDIO;
			break; 
		case VO_SOURCE2_ADAPTIVESTREAMING_VIDEO:
		case VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO:
			flag = CHUNK_FLAG_TYPE_VIDEO;
			break; 
		case VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA:
			flag = CHUNK_FLAG_TYPE_HEADDATA;
			break; 
		case VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE:
			flag = CHUNK_FLAG_TYPE_SUBTITLE;
			break; 
		case VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX:
			flag = CHUNK_FLAG_TYPE_SEGMENTINDEX;
			break; 
			
	}

	return flag;
}

VO_U32 CDownloadList::PushTrunkToDownloadList(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE type, VO_BOOL isOnlyonTrack)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;

	VO_U64	streamduration = 0;
	VO_U64	trackduration = 0;
	VO_ADAPTIVESTREAMPARSER_CHUNK* pChunk = NULL;

	while(VO_TRUE)
	{
	
		//here, we need to decide whether to continue.
		GetMinTrackDurationOfStream(&streamduration);
		GetTrackDuration(type, &trackduration);
		
		if(trackduration > streamduration){
			break;
		}
		
		CHUNKINFO* pinfo = new CHUNKINFO;
		memset(pinfo, 0x0, sizeof(CHUNKINFO));

		pChunk = NULL;
		ret = m_pAsp->GetChunk (type , &pChunk);
		
		if(pChunk){
			pinfo->pChunk = new VO_ADAPTIVESTREAMPARSER_CHUNK;			
			memcpy(pinfo->pChunk, pChunk, sizeof(VO_ADAPTIVESTREAMPARSER_CHUNK));
			if (pChunk->pStartExtInfo)
			{
				pinfo->pChunk->pStartExtInfo = new VO_ADAPTIVESTREAMPARSER_STARTEXT;
				memcpy(pinfo->pChunk->pStartExtInfo, pChunk->pStartExtInfo, sizeof(VO_ADAPTIVESTREAMPARSER_STARTEXT));
			}

		}
		
		pinfo->Flag |= ConverToFlag(type);

		if(VO_RET_SOURCE2_OK != ret){
			pinfo->Flag |= CHUNK_FLAG_STATUS_END;
			m_nChunklist.push_back(pinfo);
			break;
		}
	
		GenerateDownloadPath(pinfo);
		GenerateFileName(pinfo);
		m_nChunklist.push_back(pinfo);

		//if we only have one track, we should break out.
		if(isOnlyonTrack){
			break;
		}
	}

	return ret;

}


VO_U32 CDownloadList::AddWriteManifestFlag(VO_BOOL bEndFlag)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;

	CHUNKINFO* pinfo = new CHUNKINFO;
	memset(pinfo, 0x0, sizeof(CHUNKINFO));
	pinfo->Flag = CHUNK_FLAG_STATUS_MANIFEST;
	if(bEndFlag){
		pinfo->Flag |= CHUNK_FLAG_STATUS_END;
	}
	pinfo->pReserve = (VO_PTR)new VO_U64;
	*((VO_U64*)pinfo->pReserve) = 0;
	GetTrackDuration(VO_SOURCE2_ADAPTIVESTREAMING_VIDEO, (VO_U64*)(pinfo->pReserve));
	m_nChunklist.push_back(pinfo);

	return ret;
}

VO_U32 CDownloadList::GetTrunkToDownload(CHUNKINFO** ppTrunkinfo, DOWNLOAD_TYPE type)
{
	VO_U32 ret =  VO_RET_SOURCE2_FAIL;


	switch(type)
	{
		case CURRENT_TYPE:
			if(m_nDownloadIterator != m_nChunklist.end()){
				(*ppTrunkinfo) = (CHUNKINFO*)(*m_nDownloadIterator);
				ret = VO_RET_SOURCE2_OK;
			}else{
				ret = VO_RET_SOURCE2_END;
			}
			break;
		case REWIND_TYPE:
			break;
		case FORWARD_TYPE:
			if(m_nDownloadIterator != m_nChunklist.end()){
				(*ppTrunkinfo) = (CHUNKINFO*)(*m_nDownloadIterator);
				m_nDownloadIterator++;
				ret = VO_RET_SOURCE2_OK;
			}else{
				ret = VO_RET_SOURCE2_END;
			}
			break;
	}


	return ret;

}


VO_U32 CDownloadList::GetChunkList(list_T<CHUNKINFO*>** ppChunkList)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;
	*ppChunkList = &m_nChunklist;
	return ret;
}


VO_U32 CDownloadList::GetManifestID(VO_CHAR** ppManifestID)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;

	*ppManifestID = m_sManifestID;

	return ret;
}

VO_U32 CDownloadList::GetMinTrackDurationOfStream(VO_U64* pDuration)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;
	VO_U64 tmpduration = 0;
	VO_U32 type = 0;
	*pDuration = 0;

	for(type = VO_SOURCE2_ADAPTIVESTREAMING_AUDIO; type < VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX; type++)
	{

		if(VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO == type){
			continue;
		}
	
		//continue if the track has end
		if(VO_RET_SOURCE2_END == GetTrackDuration((VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE)type, &tmpduration)){
			continue;
		}
	
		(*pDuration) = (0 == (*pDuration)) ? tmpduration: min((*pDuration), tmpduration);
	}

	return ret;
}


VO_U32 CDownloadList::GetTrackDuration(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE type, VO_U64* pDuration)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;
	VO_U64 duration = 0;

	list_T<CHUNKINFO *>::iterator trackiter;
	for(trackiter = m_nChunklist.begin(); trackiter != m_nChunklist.end(); ++trackiter)
	{
		CHUNKINFO *pTrunkInfo = (CHUNKINFO*)(*trackiter);

		if(!(pTrunkInfo->Flag&ConverToFlag(type))){
			continue;
		}

		if(pTrunkInfo->Flag&CHUNK_FLAG_STATUS_END){
			ret = VO_RET_SOURCE2_END;
			break;
		}

		VO_ADAPTIVESTREAMPARSER_CHUNK* Trunk = pTrunkInfo->pChunk;

		if(Trunk){
			duration+= Trunk->ullDuration;
		}else{
			VODS_VOLOGE("TRUNK IS NULL!");
		}

	}
	
	*pDuration = duration;


	return ret;
}

VO_U32 CDownloadList::GenerateDownloadPath(CHUNKINFO*	pChunkinfo)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;
	VO_ADAPTIVESTREAMPARSER_CHUNK* Trunk = pChunkinfo->pChunk;

	GetTheAbsolutePath(pChunkinfo->DownloadUrl , Trunk->szUrl , Trunk->szRootUrl );

	return ret;
}

VO_U32 CDownloadList::GenerateFileName(CHUNKINFO*	pChunkinfo)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;
	VO_ADAPTIVESTREAMPARSER_CHUNK* Trunk = pChunkinfo->pChunk;
	
	VO_CHAR	name[MAXURLLEN];
	VO_PCHAR	pName = name;
	memset(name, 0x0, MAXURLLEN);

	//First, add the manifest id
	memcpy(pName, m_sManifestID, strlen(m_sManifestID));
	pName+=strlen(m_sManifestID);
	*pName='_';
	pName++;

	//Second,add the type
	VO_PCHAR	pType = NULL;
	switch(Trunk->Type)
	{
	case VO_SOURCE2_ADAPTIVESTREAMING_AUDIO:
		pType=(VO_PCHAR)"AUDIO";
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_VIDEO:
	case VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO:
		pType=(VO_PCHAR)"VIDEO";
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE:
		pType=(VO_PCHAR)"SUBTITLE";
		break;
	}
	memcpy(pName, pType, strlen(pType));
	pName+=strlen(pType);

	//Third, add the stream id
	*pName='_';
	pName++;
	
	VO_CHAR streamid[10] = {0};
//	itoa(Trunk->sKeyID.uStreamID, streamid, 10);
	sprintf(streamid, "%d", Trunk->sKeyID.uStreamID); 
	memcpy(pName, streamid, strlen(streamid));
	pName+=strlen(streamid);

	//Fourth, add the track id
	*pName='_';
	pName++;
	
	VO_CHAR trackid[10] = {0};
//	itoa(Trunk->sKeyID.uTrackID, trackid, 10);
	sprintf(trackid, "%d", Trunk->sKeyID.uTrackID); 
	memcpy(pName, trackid, strlen(trackid));
	pName+=strlen(trackid);

	//Fifth, add the thunk id
	*pName='_';
	pName++;
	
	VO_CHAR chunkid[10] = {0};
//	itoa(Trunk->uChunkID, chunkid, 10);
	sprintf(chunkid, "%d", Trunk->uChunkID); 	
	memcpy(pName, chunkid, strlen(chunkid));
	pName+=strlen(chunkid);

	//Sixth,add the Suffix
	
	VO_PCHAR psuffix = NULL;
	GenerateFileSuffix(&psuffix);
	if(strlen(psuffix)){
		memcpy(pName, psuffix, strlen(psuffix));
		pName+=strlen(psuffix);
	}

	strcat(pChunkinfo->FileName, name);
/*
	memcpy(pName, Trunk->szUrl, strlen(Trunk->szUrl));
	pName+=strlen(Trunk->szUrl);
	
	VO_CHAR* poniter = NULL;
	poniter = strchr(name, '/');
	*poniter = '_';
	strcat(pChunkinfo->FileName, name);
*/	
//	GetTheAbsolutePath(pChunkinfo->DownloadUrl , Trunk->szUrl , Trunk->szRootUrl );
	return ret;
}

VO_U32 CDownloadList::GenerateTheMianfestID(VO_PCHAR pSource)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;
	VO_U32 id = 0;
	memset(m_sManifestID, 0x0, sizeof(m_sManifestID));
	id = BKDRHash(pSource);

//	itoa(id, m_sManifestID, 16);
	sprintf(m_sManifestID, "%0x", id); 	
	SD_LOGI("m_sManifestID=%s",m_sManifestID);
	return ret;
}


VO_U32 CDownloadList::GenerateFileSuffix(VO_PCHAR* ppSuffix)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;

	*ppSuffix=(VO_PCHAR)".ts";

	return ret;
}


VO_U32 CDownloadList::PrintChunkInfo(list_T<CHUNKINFO*>*	pChunklist)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;
	VO_U32 num =  0;

	list_T<CHUNKINFO *>::iterator trackiter;
	SD_LOGI("+PrintChunkInfo");
	
	for(trackiter = pChunklist->begin(); trackiter != pChunklist->end(); ++trackiter)
	{
		CHUNKINFO *pTrunkInfo = (CHUNKINFO*)(*trackiter);
		VO_ADAPTIVESTREAMPARSER_CHUNK* Trunk = pTrunkInfo->pChunk;
		
		SD_LOGI("Trunk num = %lu", num);
		num++;
		SD_LOGI("DownloadUrl = %s", pTrunkInfo->DownloadUrl);
		SD_LOGI("FileName = %s", pTrunkInfo->FileName);
		SD_LOGI("Flag = %0x", pTrunkInfo->Flag);
	}
	SD_LOGI("-PrintChunkInfo");

	return ret;
}

VO_U32 CDownloadList::ReleaseChunkList(list_T<CHUNKINFO*>*	pChunklist)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;

	list_T<CHUNKINFO *>::iterator trackiter;
	for(trackiter = pChunklist->begin(); trackiter != pChunklist->end(); ++trackiter)
	{
		CHUNKINFO *pTrunkInfo = (CHUNKINFO*)(*trackiter);
		if(pTrunkInfo->pChunk)
		{
			if (pTrunkInfo->pChunk->pStartExtInfo){
				delete pTrunkInfo->pChunk->pStartExtInfo;
			}
			delete pTrunkInfo->pChunk;
		}
		
		if(pTrunkInfo->pReserve){
			delete (VO_U64*)pTrunkInfo->pReserve;
			pTrunkInfo->pReserve = NULL;
		}

		delete pTrunkInfo;
	}
	pChunklist->clear();

	return ret;
}


// BKDR Hash Function
VO_U32 CDownloadList::BKDRHash(VO_CHAR* pStr)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*pStr)
    {
        hash = hash * seed + (*pStr++);
    }

    return (hash & 0x7FFFFFFF);
}


