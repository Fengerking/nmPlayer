/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		ThunkDownloader.cpp

	Contains:	CThunkDownloader class file

	Written by:	Aiven

	Change History (most recent first):
	2013-09-02		Aiven			Create file

*******************************************************************************/
#include "SDownloaderLog.h"
#include "CSourceIOUtility.h"
#include "ChunkDownloader.h"
#include "voOSFunc.h"
#include "StreamingDownloader.h"
#include "voToolUtility.h"
#include "voHLSManifestWriter.h"
#include <errno.h>

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define FILESIZE		1024

CChunkDownloader::CChunkDownloader(CDownloadList* pDownloadlist, CStreamingDownloader* pStreamingDownloader, VO_SOURCE2_EVENTCALLBACK* pEventCallbackFunc, VO_SOURCE2_IO_API* pIO, VO_PTCHAR pLocalDir)
:m_pIO(NULL)
,m_pManifestWriter(NULL)
,m_pDownloadList(NULL)
,m_pStreamingDownloader(NULL)
,m_nLastUpdateTime(0)
,m_nInterval(0)
,m_nDuration(0)
,m_bDownloadCancel(VO_TRUE)
{
	m_pDownloadList = pDownloadlist;
	m_pStreamingDownloader = pStreamingDownloader;
	m_pManifestWriter = new C_VoHLSManifestWriter();

	m_pEventCallback=pEventCallbackFunc;
	m_pIO = pIO;
	m_pLocalDir = pLocalDir;
}

CChunkDownloader::~CChunkDownloader()
{
	if(m_pManifestWriter){
		delete m_pManifestWriter;
		m_pManifestWriter=NULL;
	}
}

VO_U32 CChunkDownloader::Start()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	SD_LOGI("+start_updatethread" );
	m_bDownloadCancel = VO_FALSE;;
	m_nLastUpdateTime=0;
	m_nInterval=0;
	vo_thread::begin();
	SD_LOGI("-start_updatethread" );

	return ret;
}

VO_U32 CChunkDownloader::Stop()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	SD_LOGI( "+stop_updatethread" );
	m_bDownloadCancel = VO_TRUE;
	vo_thread::stop();
	SD_LOGI( "-stop_updatethread" );

	return ret;
}

void CChunkDownloader::thread_function()
{
	SD_LOGI( "thread_function Start!" );	
	DownloadThread();
}

VO_U32 CChunkDownloader::GetTrunkToDownload(CHUNKINFO** ppChunkinfo, DOWNLOAD_TYPE type)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	ret = m_pDownloadList->GetTrunkToDownload(ppChunkinfo, type);

	return ret;
}

VO_U32 CChunkDownloader::DownloadThread()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	CHUNKINFO* pChunkinfo = NULL;
	VO_BOOL bNeedUpdateManifest = VO_FALSE;

	SD_LOGI( "Download Thread Start" );

	while(!m_bDownloadCancel)
	{
		ret = GetTrunkToDownload(&pChunkinfo, FORWARD_TYPE);

		if(VO_RET_SOURCE2_END == ret){
			SD_LOGE("DownloadThread VO_RET_SOURCE2_END!");
			break;
		}else if(VO_RET_SOURCE2_NEEDRETRY == ret){
			SD_LOGE("DownloadThread VO_RET_SOURCE2_NEEDRETRY");
			continue;
		}else if(VO_RET_SOURCE2_OK != ret){
			SD_LOGE("DownloadThread error ret=%lu", ret);
			break;
		}

		if(bNeedUpdateManifest && IsNeedToWriteManifest(pChunkinfo))
		{
			ret=ProcessManifest(pChunkinfo);
			bNeedUpdateManifest = VO_FALSE;
		}
		else if(IsNeedToDownload(pChunkinfo))
		{
			ret=ProcessFileTrunk(pChunkinfo);
			bNeedUpdateManifest = (VO_RET_SOURCE2_OK == ret) ? VO_TRUE : VO_FALSE;
		}

		if(VO_RET_SOURCE2_OK!= ret){
			break;
		}
	}

	ProcessManifest(NULL, VO_TRUE);

	if(VO_RET_SOURCE2_END == ret){
		if(m_pEventCallback && (!m_bDownloadCancel)){
			m_pEventCallback->SendEvent(m_pEventCallback->pUserData, DOWNLOADER_EVENT_END, 0, 0);
		}
		SD_LOGE("Send EVENT VO_RET_SOURCE2_END!");
		ret = VO_RET_SOURCE2_OK;
	}

	SD_LOGI( "Download Thread End" );
	
	return ret;
}

VO_BOOL CChunkDownloader::IsNeedToDownload(CHUNKINFO* pChunkinfo)
{
	VO_BOOL ret = VO_TRUE;

	if(!(pChunkinfo->Flag&CHUNK_FLAG_TYPE_MAX) ||(pChunkinfo->Flag&CHUNK_FLAG_STATUS_END) ){
		SD_LOGI("The type is not support---Flag=%x", pChunkinfo->Flag);
		return VO_FALSE;
	}

	FILE* pfile = NULL;
		
	memcpy(pChunkinfo->LoaclUrl,m_pLocalDir,strlen((char*)m_pLocalDir));

	strcat(pChunkinfo->LoaclUrl,"/");
	strcat(pChunkinfo->LoaclUrl,pChunkinfo->FileName);

	pfile = fopen(pChunkinfo->LoaclUrl, "r");

	if(pfile){
		ret = VO_FALSE;
		pChunkinfo->Flag |= CHUNK_FLAG_STATUS_DOWNLOAD_OK;
		SD_LOGE("Chunk already exit!---LoaclUrl=%s", pChunkinfo->LoaclUrl);
		fclose (pfile);
		pfile = NULL;
	}

	return ret;
}

VO_U32 CChunkDownloader::DownloadFileTrunk(CHUNKINFO* pChunkinfo, CHUNK_BUFFER* pChunkbuffer)
{
	VO_U32 ret = VO_SOURCE2_IO_FAIL;
	VO_HANDLE hIO = NULL;
	VO_U64	size = 0;
	VO_U32 nErrCode = 0;
	VO_ADAPTIVESTREAMPARSER_CHUNK* pChunk = pChunkinfo->pChunk;

	SD_LOGI("DownloadFileTrunk---start");
	
	voAutoIOInit init_obj( m_pIO , pChunkinfo->DownloadUrl , &hIO );
	if( init_obj.m_ret != VO_SOURCE2_IO_OK )
	{
		nErrCode = m_pIO->GetLastError(hIO);
		SD_LOGE("-AutoIO Init failed( errCode:%d )! %s",nErrCode, pChunkinfo->DownloadUrl);
		return VO_RET_SOURCE2_CHUNKDROPPED;
	}
	
	if( pChunk->ullChunkOffset != INAVALIBLEU64 )
	{
		VO_SOURCE2_IO_HTTPRANGE range;
		range.ullOffset = pChunk->ullChunkOffset;
		range.ullLength = pChunk->ullChunkSize;
		m_pIO->SetParam( hIO , VO_SOURCE2_IO_PARAMID_HTTPRANGE , &range );

		SD_LOGI( "Range: %lld , %lld" , range.ullOffset , range.ullLength );
	}
	
	voAutoIOOpen open_obj( m_pIO , hIO , VO_TRUE );
	if( open_obj.m_ret != VO_SOURCE2_IO_OK )
	{
		nErrCode = m_pIO->GetLastError(hIO);
		SD_LOGE("-AutoIO Open failed( errCode:%d )! %s",nErrCode, pChunkinfo->DownloadUrl);
		return VO_RET_SOURCE2_CHUNKDROPPED;
	}

	while(!m_bDownloadCancel)
	{
		pChunkbuffer->nSize = 0;
		ret = m_pIO->GetSize( hIO, &pChunkbuffer->nSize );

		if(VO_SOURCE2_IO_OK == ret){

			if(pChunkbuffer->pBuffer){
				delete pChunkbuffer->pBuffer;
				pChunkbuffer->pBuffer = NULL;
			}
			
			pChunkbuffer->pBuffer = new unsigned char[(VO_U32)pChunkbuffer->nSize+1024];
			ret = ReadFixedSize( m_pIO, hIO , pChunkbuffer->pBuffer, (VO_U32*)&pChunkbuffer->nSize, &m_bDownloadCancel);
			
			SD_LOGI("read file---nSize=%lu",pChunkbuffer->nSize);
		}

		if(VO_SOURCE2_IO_OK == ret){
			break;
		}

		if(VO_SOURCE2_IO_FAIL == ret){
			break;
		}
		
		voOS_SleepExitable(20, &m_bDownloadCancel);

	}

	SD_LOGI("Download  ret=0x%0x, DownloadUrl=%s", ret, pChunkinfo->DownloadUrl);

	return ret;
}

VO_U32 CChunkDownloader::WriteFileTrunk(CHUNKINFO* pChunkinfo, CHUNK_BUFFER* pChunkbuffer)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	VO_U32 size = 0;	
	VO_PBYTE pbuffer = NULL;
	FILE* pfile = NULL;

	pfile = fopen(pChunkinfo->LoaclUrl, "wb");

	if(!pfile){
		SD_LOGE("WriteFileTrunk Open::errno=%d, error string:%s", errno, strerror(errno));
		return VO_RET_SOURCE2_FAIL;
	}
	
	size = (VO_U32)pChunkbuffer->nSize;
	pbuffer = pChunkbuffer->pBuffer;

	VO_U32 writesize = 0;
	while(size){

		if(size>=FILESIZE){
			writesize = fwrite (pbuffer , sizeof(char), FILESIZE, pfile);
		}else{
			writesize = fwrite (pbuffer , sizeof(char), size, pfile);
		}

		size -= writesize;
		pbuffer += writesize;

		if(!writesize){
			SD_LOGE("WriteFileTrunk Fail::errno=%d, error string:%s", errno, strerror(errno));
			ret = VO_RET_SOURCE2_FAIL;
			break;
		}
	}

	fclose (pfile);

	if(VO_RET_SOURCE2_OK != ret){
		VO_U32 count = 3;
		while(count){
		
			int result = remove( pChunkinfo->LoaclUrl );
			if (0 == result || (-1 == result && 2 == errno)){
				break;
			}
		
			voOS_Sleep(10);
			count--;
		}
		if(!count){
			SD_LOGE("remove errno=%d, error string:%s, LoaclUrl=%s", errno, strerror(errno), pChunkinfo->LoaclUrl);
		}
	}

	if(VO_RET_SOURCE2_OK == ret){
		pChunkinfo->Flag |= CHUNK_FLAG_STATUS_DOWNLOAD_OK;
	}
	else{
		pChunkinfo->Flag |= CHUNK_FLAG_STATUS_DOWNLOAD_FAIL;
	}

	return ret;
}

VO_BOOL CChunkDownloader::IsNeedToWriteManifest(CHUNKINFO* pChunkinfo)
{
	VO_BOOL ret = VO_FALSE;

	if(!(pChunkinfo->Flag&CHUNK_FLAG_STATUS_MANIFEST)){
		return VO_FALSE;
	}

	VO_ADAPTIVESTREAMPARSER_CHUNK* pChunk = pChunkinfo->pChunk;
	if(!m_nLastUpdateTime 
		|| ((voOS_GetSysTime() -m_nLastUpdateTime) > m_nInterval) 
		||(pChunkinfo->Flag&CHUNK_FLAG_STATUS_END)){
		m_nLastUpdateTime = voOS_GetSysTime();
		SD_LOGI("NeedToWriteManifest---m_nLastUpdateTime=%lu, m_nInterval =%lu, Flag=%x", m_nLastUpdateTime, m_nInterval, pChunkinfo->Flag);
		ret = VO_TRUE;
	}
	
	return ret;
}

VO_U32 CChunkDownloader::GenerateManifest(MANIFEST_GROUP** ppManifestGroup, VO_BOOL bEnd)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	list_T < CHUNKINFO * >* pChunkList = NULL;
	m_pDownloadList->GetChunkList(&pChunkList);
	
	list_T < MANIFEST_INFO * >* pManifestList = NULL;
	
	ret = m_pStreamingDownloader->GetManifestInfo(&pManifestList);

	VO_CHAR* pManifestID = NULL;
	if(VO_RET_SOURCE2_OK == ret){
		ret = m_pDownloadList->GetManifestID(&pManifestID);
	}
	
	if(VO_RET_SOURCE2_OK == ret){
		ret = m_pManifestWriter->WriteManifest(pManifestList, pChunkList, pManifestID, ppManifestGroup, bEnd);
	}

	return ret;
}

VO_U32 CChunkDownloader::WriteManifest(MANIFEST_GROUP* pManifestGroup)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	_VODS_CHECK_NULL_POINTER_ONE_(pManifestGroup, VO_RET_SOURCE2_OK);
	
	MANIFEST_INFO**	pInfo =pManifestGroup->ppInfo;
	VO_S32 i = pManifestGroup->nCount-1;
	while(VO_TRUE)
	{
		if(i < 0){
			break;
		}

		MANIFEST_INFO*	pTmp = pInfo[i];

		FILE* pfile = NULL;
		VO_PBYTE pbuffer = NULL;
		VO_U32 size = 0;
		
		VO_CHAR name[MAXURLLEN];
		memset(name, 0x0, MAXURLLEN);
		memcpy(name,m_pLocalDir,strlen((char*)m_pLocalDir));
		strcat(name,"/");
		strcat(name,pTmp->szUrl);


		VO_CHAR tmpname[MAXURLLEN];
		memset(tmpname, 0x0, MAXURLLEN);
		memcpy(tmpname,m_pLocalDir,strlen((char*)m_pLocalDir));
		strcat(tmpname,"/");
		strcat(tmpname,"manifest.tmp");


		pfile = fopen(tmpname, "wb");

		if(!pfile){
			SD_LOGE("WriteManifest Open errno=%d, error string:%s", errno, strerror(errno));
			return VO_RET_SOURCE2_FAIL;
		}
		
		size = (VO_U32)pTmp->nLength;
		pbuffer = pTmp->pManifestData;

		VO_U32 writesize = 0;
		while(size){

			if(size>=FILESIZE){
				writesize = fwrite (pbuffer , sizeof(char), FILESIZE, pfile);
			}else{
				writesize = fwrite (pbuffer , sizeof(char), size, pfile);
			}
			
			size -= writesize;
			pbuffer += writesize;
			
			if(!writesize){
				ret = VO_RET_SOURCE2_FAIL;
				SD_LOGE("WriteManifest Fail::errno=%d, error string:%s", errno, strerror(errno));
				break;
			}
		}

		fclose (pfile);

		if(VO_RET_SOURCE2_OK != ret){
			return ret;
		}

		VO_U32 count = 10;
		while(count){

			int result = remove( name );
			if (0 == result || (-1 == result && 2 == errno)){
				break;
			}
		
			voOS_Sleep(10);
			count--;
		}
		if(!count){
			SD_LOGE("remove errno=%d, error string:%s, filename=%s", errno, strerror(errno), name);
			return VO_RET_SOURCE2_FAIL;
		}


		count = 10;
		while(count){
			
			if (!rename( tmpname , name )){
				break;
			}
		
			voOS_Sleep(10);
			count--;
		}

		if(!count){
			SD_LOGE("rename errno=%d, error string:%s, filename=%s", errno, strerror(errno), name);
			return VO_RET_SOURCE2_FAIL;
		}

		i--;
	}
	
	return ret;
}


VO_U32 CChunkDownloader::ProcessManifest(CHUNKINFO* pChunkinfo, VO_BOOL bEnd)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	MANIFEST_GROUP* pManifestGroup = NULL;
	
	if(VO_RET_SOURCE2_OK != GenerateManifest(&pManifestGroup, bEnd)){
		SD_LOGE("GenerateManifest ERROR!---m_bDownloadCancel=%d", m_bDownloadCancel);		
		if(m_pEventCallback && (!m_bDownloadCancel)){
			m_pEventCallback->SendEvent(m_pEventCallback->pUserData, DOWNLOADER_EVENT_GENERATEMANIFEST_FAIL, 0, 0);
		}
		m_bDownloadCancel = VO_TRUE;
		return VO_RET_SOURCE2_FAIL;
	}else{
		SD_LOGI("GenerateManifest OK!");
	}
	
	if(VO_RET_SOURCE2_OK != WriteManifest(pManifestGroup)){
		SD_LOGE("WriteManifest ERROR!---m_bDownloadCancel=%d", m_bDownloadCancel);		
		if(m_pEventCallback && (!m_bDownloadCancel)){
			m_pEventCallback->SendEvent(m_pEventCallback->pUserData, DOWNLOADER_EVENT_WRITEMANIFEST_FAIL, 0, 0);
		}
		m_bDownloadCancel = VO_TRUE;
		return VO_RET_SOURCE2_FAIL;
	}else{
		SD_LOGI("WriteManifest OK!");	
		if(m_pEventCallback){
			VO_CHAR manifestPath[FILENAME_LENGTH];
			memset(manifestPath, 0x0, FILENAME_LENGTH);

			memcpy(manifestPath,m_pLocalDir,strlen((VO_CHAR*)m_pLocalDir));
			strcat(manifestPath,"/");
			strcat(manifestPath,pManifestGroup->strMaster);
			
			if(pChunkinfo){
				VO_U32 tmpduration = (VO_U32)(*((VO_U64*)pChunkinfo->pReserve));
				m_nDuration = (m_nDuration < tmpduration) ? tmpduration : m_nDuration;
			}
			if(m_pEventCallback && (!m_bDownloadCancel)){
				m_pEventCallback->SendEvent(m_pEventCallback->pUserData, DOWNLOADER_EVENT_MANIFESTUPDATE, (VO_U32)manifestPath, (VO_U32)&m_nDuration);				
			}			
		}
	}

	return ret;
}

VO_U32 CChunkDownloader::ProcessFileTrunk(CHUNKINFO* pChunkinfo)
{
	VO_U32 nRetry = 3;
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	CHUNK_BUFFER	Trunkbuffer;

	memset(&Trunkbuffer, 0x0, sizeof(CHUNK_BUFFER));
	
	while(nRetry&&(!m_bDownloadCancel)){
		nResult = DownloadFileTrunk(pChunkinfo, &Trunkbuffer);
	
		if(VO_SOURCE2_IO_OK == nResult){
			break;
		}
		
		voOS_SleepExitable(200, &m_bDownloadCancel);
		nRetry--;
		SD_LOGI("DownloadFileTrunk	Remain counts=%lu, DownloadUrl=%s, m_bDownloadCancel=%d", nRetry, pChunkinfo->DownloadUrl, m_bDownloadCancel);
	}
	
	if(VO_SOURCE2_IO_OK != nResult){
		SD_LOGE("DownloadFileTrunk ERROR!  DownloadUrl=%s, m_bDownloadCancel=%d", pChunkinfo->DownloadUrl, m_bDownloadCancel);		
		if(m_pEventCallback && (!m_bDownloadCancel)){
			m_pEventCallback->SendEvent(m_pEventCallback->pUserData, DOWNLOADER_EVENT_DOWNLOADCHUNK_FAIL, 0, 0);
		}
		m_bDownloadCancel = VO_TRUE;
		if(Trunkbuffer.pBuffer){
			delete Trunkbuffer.pBuffer;
		}
		return VO_RET_SOURCE2_FAIL;
	}else{
		SD_LOGI("Download OK---DownloadUrl=%s", pChunkinfo->DownloadUrl);
	}
	
	if(VO_RET_SOURCE2_OK != WriteFileTrunk(pChunkinfo, &Trunkbuffer)){
		SD_LOGE("WriteFileTrunk ERROR!LoaclUrl=%s, m_bDownloadCancel=%d", pChunkinfo->LoaclUrl, m_bDownloadCancel);		
		if(m_pEventCallback && (!m_bDownloadCancel)){
			m_pEventCallback->SendEvent(m_pEventCallback->pUserData, DOWNLOADER_EVENT_WRITECHUNK_FAIL, 0, 0);
		}
		m_bDownloadCancel = VO_TRUE;
		if(Trunkbuffer.pBuffer){
			delete Trunkbuffer.pBuffer;
		}		
		return VO_RET_SOURCE2_FAIL;
	}else{
		VO_U32 interval = pChunkinfo->pChunk->ullDuration/3;
		m_nInterval = (0 == m_nInterval || interval < m_nInterval) ? interval : m_nInterval;
		SD_LOGI("WriteFileTrunk OK---LoaclUrl=%s, m_nInterval=%lu", pChunkinfo->LoaclUrl, m_nInterval);
	}
	
	if(Trunkbuffer.pBuffer){
		delete Trunkbuffer.pBuffer;
	}		

	return nResult;
}
