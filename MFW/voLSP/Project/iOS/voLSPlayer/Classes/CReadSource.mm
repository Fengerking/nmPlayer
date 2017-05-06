/*
 *  CReadSource.cpp
 *
 *  Created by Lin Jun on 4/21/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CReadSource.h"
#include <unistd.h>

#include "FileDevice.h"
#include "CTSLiveSource.h"

//#define LOG_REC

#ifdef LOG_REC
FILE *fpRec = NULL;
#endif

int streamSrcCallback(void * pUserData, unsigned char * pData, int nDataLen)
{
	if(!pUserData)
		return 0;
	
#ifdef LOG_REC
	if( fpRec )
	{
		fwrite( pData, nDataLen , 1, fpRec );
		fflush( fpRec );
	}
#endif
	CReadSource* pSrc = (CReadSource*)pUserData;
	return pSrc->PushBuffer(pData, nDataLen);
}

CReadSource::CReadSource(void* pUserData, PushBufferCallBack pPushBufferCallBack)
:m_pStreamingSource(NULL)
,m_bProgramChanged(false)
,m_pUserData(pUserData)
,m_pPushBufferCallBack(pPushBufferCallBack)
{
#ifdef LOG_REC
	if (NULL == fpRec) {
		NSString *filePath = nil;
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/VideoDumpRec"];
		fpRec = fopen([filePath UTF8String], "wb");
		
		if (NULL == fpRec) {
			printf("hjf ------------------Open decdumprec file error!\n");
		}
	}
#endif
}

CReadSource::~CReadSource()
{
	if(m_pStreamingSource)
	{
		m_pStreamingSource->Stop();
		delete m_pStreamingSource;
	}
#ifdef LOG_REC
	if( fpRec )
 		fclose(fpRec);
#endif
}

int CReadSource::PushBuffer(unsigned char* pData, int nSize)
{
	int nRet = -1;
	
	if(m_pPushBufferCallBack)
	{
		VOMP_BUFFERTYPE cVOMP_BUFFERTYPE;
		memset(&cVOMP_BUFFERTYPE, 0, sizeof(VOMP_BUFFERTYPE));
		cVOMP_BUFFERTYPE.pBuffer = pData;
		
		cVOMP_BUFFERTYPE.nSize = nSize;
		
		if ((0 == nSize) || (NULL == pData)) {
			return VOMP_ERR_Pointer;
		}
		nRet = m_pPushBufferCallBack(m_pUserData, VO_BUFFER_INPUT_STREAM, &cVOMP_BUFFERTYPE);
	}
	else
	{
		printf("PushBuffer callback is NULL\n");
		return VOMP_ERR_Pointer;
	}
	
	return nRet;
}

int CReadSource::Start()
{
	if (m_pStreamingSource)
	{
		if(m_pStreamingSource->IsStatusPaused())
		{
			return m_pStreamingSource->Start();
		}
		else if(m_pStreamingSource->IsStatusRunning())
			return 0;
		
		return m_pStreamingSource->Start();
	}
	
	return 0;
}

int CReadSource::Pause()
{
	if (m_pStreamingSource)
	{
		if(m_pStreamingSource->IsStatusRunning())
			return m_pStreamingSource->Pause();
	}
	
	return 0;
}

int CReadSource::Stop()
{
	if (m_pStreamingSource)
	{
		bool bRet = m_pStreamingSource->Stop();
		return bRet?0:1;
	}
	
	return 0;
}

int CReadSource::SetStream(unsigned char* pUrl, int nPort, bool bFile)
{
	if(m_pStreamingSource)
	{
		m_pStreamingSource->Stop();
		delete m_pStreamingSource;
		m_pStreamingSource = NULL;
	}
	
	if (bFile) {
		// local file simulator source
		m_pStreamingSource = new CFileDevice(this, streamSrcCallback);
	}
	else {
		// live streaming source, TS need to implement this class
		m_pStreamingSource = new CTSLiveSource(this, streamSrcCallback);
	}
	
	bool bRet = m_pStreamingSource->Open(pUrl, nPort);
	return bRet?0:-1;
}