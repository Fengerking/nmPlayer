#include "CAsfReader.h"
#include "CAsfVideoTrack.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CAsfVideoTrack::CAsfVideoTrack(VO_U8 btStreamNum, VO_U32 dwDuration, CAsfReader *pReader, VO_MEM_OPERATOR* pMemOp)
	: CAsfTrack(VOTT_VIDEO, btStreamNum, dwDuration, pReader, pMemOp)
	, m_iPosMaxKeyFrame(0)
	, m_iSizeMaxKeyFrame(0)
, m_uThumbnailCnt(0)
{
}

CAsfVideoTrack::~CAsfVideoTrack()
{
}

VO_U32 CAsfVideoTrack::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 iRet = 0;
	do 
	{
		iRet = CAsfTrack::GetSampleN(pSample);
		if (iRet != VO_ERR_SOURCE_OK)
			return iRet;
	} while (1 == pSample->Size);
	//just for test thumail
#if 0
	VO_SOURCE_THUMBNAILINFO* pThumbNailInfo =  new VO_SOURCE_THUMBNAILINFO;
	pThumbNailInfo->uFlag = VO_SOURCE_THUMBNAILMODE_INFOONLY;	
	GetThumbNail(pThumbNailInfo);
	GetThumbNail(pThumbNailInfo);
	//pThumbNailInfo->uFlag = VO_SOURCE_THUMBNAILMODE_DATA;	
	GetThumbNail(pThumbNailInfo);
	GetThumbNail(pThumbNailInfo);
	GetThumbNail(pThumbNailInfo);
	GetThumbNail(pThumbNailInfo);
	pThumbNailInfo->uFlag = VO_SOURCE_THUMBNAILMODE_DATA;
	GetThumbNail(pThumbNailInfo);
#endif

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfVideoTrack::GetFileIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp)
{
	return m_pReader->FileIndexGetIndexByTime(bForward, llTimeStamp);
}

VO_U64 CAsfVideoTrack::GetFilePosByFileIndex(VO_U32 dwFileIndex)
{
	return m_pReader->FileIndexGetFilePosByIndex(dwFileIndex);
}

VO_U32 CAsfVideoTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	VO_VIDEOINFOHEADER* pvih = (VO_VIDEOINFOHEADER*)m_pPropBuffer;
	pHeadData->Length = pvih->bmiHeader.biSize - sizeof(VO_BITMAPINFOHEADER);
	pHeadData->Buffer = m_pPropBuffer + sizeof(VO_VIDEOINFOHEADER);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfVideoTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	if(m_lMaxSampleSize <= 0)
	{
		VO_VIDEOINFOHEADER* pvih = (VO_VIDEOINFOHEADER*)m_pPropBuffer;
		m_lMaxSampleSize = pvih->bmiHeader.biWidth * pvih->bmiHeader.biHeight * 3 / 4;
	}

	if(pdwMaxSampleSize)
		*pdwMaxSampleSize = m_lMaxSampleSize;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfVideoTrack::GetCodecCC(VO_U32* pCC)
{
	*pCC = ((VO_VIDEOINFOHEADER*)m_pPropBuffer)->bmiHeader.biCompression;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfVideoTrack::GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat)
{
	VO_VIDEOINFOHEADER* pvih = (VO_VIDEOINFOHEADER*)m_pPropBuffer;
	pVideoFormat->Width = pvih->bmiHeader.biWidth;
	pVideoFormat->Height = pvih->bmiHeader.biHeight;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfVideoTrack::GetFrameTime(VO_U32* pdwFrameTime)
{
	if(pdwFrameTime)
	{
		if(m_dwFrameRate)
			*pdwFrameTime = 10000000 / m_dwFrameRate;
		else
		{
			VO_U32 dwCurrCount = m_pStreamFileIndex->GetCurrEntryCount();
			if(dwCurrCount < 2)
				return VO_ERR_NOT_IMPLEMENT;

			*pdwFrameTime = VO_U64(100) * (m_pStreamFileIndex->GetCurrEndTime() - m_pStreamFileIndex->GetCurrStartTime()) / (dwCurrCount - 1);
		}
	}
	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfVideoTrack::GetBitmapInfoHeader(VO_BITMAPINFOHEADER** ppBitmapInfoHeader)
{
	VO_VIDEOINFOHEADER* pvih = (VO_VIDEOINFOHEADER*)m_pPropBuffer;
	*ppBitmapInfoHeader = &pvih->bmiHeader;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfVideoTrack::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch (uID)
	{
	case VO_PID_SOURCE_GETTHUMBNAIL:
		{
			if ( !m_pReader->ExistIndex() )
				break;

			if(VOTT_VIDEO != m_nType)
				return VO_ERR_NOT_IMPLEMENT;

			VO_SOURCE_SAMPLE * pSample = (VO_SOURCE_SAMPLE*)pParam;

			for( VO_S32 i = 0 ; i < 10 ; i++ )
			{
				if (GetNextKeyFrameSize() != 0)
					break;
			}
			//-
			m_pReader->Flush();

			//do parse until get key frame entry!!
			CvoFileDataParser* pDataParser = m_pStreamFileReader->GetFileDataParserPtr();
			if(!pDataParser->SetStartFilePos(m_iPosMaxKeyFrame))
				return VO_ERR_SOURCE_END;

			PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
			VO_BOOL bRet = VO_FALSE;
			while(!bRet)
			{
				if(!pDataParser->Step())
					return VO_ERR_SOURCE_END;

				bRet = m_pStreamFileIndex->GetKeyFrameEntry(&pGet);
			}

			//sample ready!!
			pSample->Duration = 1;
			pSample->Time = pGet->time_stamp;

			pSample->Size = pGet->size;
			if(2 == m_pGlobeBuffer->Get(&pSample->Buffer, m_pThumbnailBuffer, pGet->pos_in_buffer & VO_MAXS32, pGet->size))
				pSample->Buffer = m_pThumbnailBuffer;

			return VO_ERR_SOURCE_OK;
		}
		break;
	}

	return CAsfTrack::GetParameter(uID, pParam);
}

VO_U32 CAsfVideoTrack::GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo) 
{
	VO_U32 nResult = VO_ERR_SOURCE_ERRORDATA;
	VOLOGW("GetThumbNail entry");
	if(!pThumbNailInfo)
	{
		VOLOGW("GetThumbNail error");
		return VO_ERR_SOURCE_ERRORDATA;
	}
	switch(pThumbNailInfo->uFlag)
	{
	case VO_SOURCE_THUMBNAILMODE_INFOONLY:
		{
			VOLOGW("GetThumbNailInfo entry");
			nResult = GetThumbNailInfo(pThumbNailInfo);
			VOLOGW("GetThumbNailInfo nResult %d",nResult);
		}
		break;
	case VO_SOURCE_THUMBNAILMODE_DATA:
		{
			VOLOGW("GetThumbNailBuffer entry");
			nResult = GetThumbNailBuffer(pThumbNailInfo);
		}
		break;
	default:
		break;
	}

	return nResult;
}
VO_U32  CAsfVideoTrack::GetThumbNailInfo(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo)
{
	if(!pThumbNailInfo){
		return VO_ERR_SOURCE_ERRORDATA;
	}
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;
	// for not ExistIndex file , generate just one frame for every track. when get thunmail, step() form parser to get the next key_frame
	if (!m_pReader->ExistIndex() )
	{
		m_uThumbnailCnt++;
		VO_S64 ullTime = VO_MAXS64; 
		CvoFileDataParser* pDataParser = m_pStreamFileReader->GetFileDataParserPtr();
		if (pDataParser == NULL)
		{
			return VO_ERR_SOURCE_END;
		}
		while (1)
		{
			if(!IsCannotGenerateIndex(pDataParser))
			{
				if (m_pStreamFileIndex->GetCurrEntryCount()>= m_uThumbnailCnt)
				{
					PBaseStreamMediaSampleIndexEntry pEntry;
					m_pStreamFileIndex->GetTail(&pEntry);
					VO_BOOL bFrameDropped = VO_FALSE;
					if (pEntry)
					{
						if (!pEntry->IsKeyFrame())
						{
							if(!pDataParser->Step())
								pDataParser->SetParseEnd(VO_TRUE);
							continue;
					//		return VO_ERR_SOURCE_END;
						}
						pThumbNailInfo->ullFilePos = pDataParser->GetCurrParseFilePos();
						pThumbNailInfo->ullTimeStamp = pEntry->time_stamp;
						pThumbNailInfo->uSampleSize = pEntry->size;
						pThumbNailInfo->uPrivateData =pEntry->pos_in_buffer;
						return VO_ERR_SOURCE_OK;
					}
				}
				if(!pDataParser->Step())
					pDataParser->SetParseEnd(VO_TRUE);
			}
			else
			{
				VOLOGE("can not generate more key frame:%d",m_uThumbnailCnt);
				return VO_ERR_SOURCE_END;
			}

		}
	}
	// end not ExistIndex,

	// for simpleindex   most of ASF file has simpleindex, when get thunmail ,just provide the size  for thunmailinfo
	if (GetNextKeyFrameSize() != 0)
		return VO_ERR_NOT_IMPLEMENT;
	CvoFileDataParser* pDataParser = m_pStreamFileReader->GetFileDataParserPtr();
	if(!pDataParser->SetStartFilePos(m_iPosMaxKeyFrame))
		return VO_ERR_SOURCE_END;
	pThumbNailInfo->uSampleSize = m_dwSizeCurrentKeyFrame;
	pThumbNailInfo->uPrivateData = m_iPosMaxKeyFrame;
	VOLOGW("GetThumbNailInfo uSampleSize %ld",pThumbNailInfo->uSampleSize);
	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfVideoTrack::GetThumbNailBuffer(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo)
{
	VO_U32 nResult = VO_ERR_SOURCE_ERRORDATA;
	if(!pThumbNailInfo){
		return nResult;
	}
	// don't have simple_index
	if (!m_pReader->ExistIndex())
	{
		VO_U32 pos_in_buffer = pThumbNailInfo->uPrivateData;
		VO_U32 size = pThumbNailInfo->uSampleSize;
		if(2 == m_pGlobeBuffer->Get(&pThumbNailInfo->pSampleData, m_pSampleData, pos_in_buffer & VO_MAXS32, size))
			pThumbNailInfo->pSampleData = m_pSampleData;
		nResult = VO_ERR_SOURCE_OK;
		return nResult;
	}
	//end for not exisit index

	//have simple_index
	if(0!=pThumbNailInfo->uSampleSize)
	{
		VO_U32 sample_size = (pThumbNailInfo->uSampleSize & 0x7FFFFFFF);
		VO_PBYTE pBuffer = m_pSampleData;
		m_pReader->Flush();
		CvoFileDataParser* pDataParser = m_pStreamFileReader->GetFileDataParserPtr();
		if(!pDataParser->SetStartFilePos(pThumbNailInfo->uPrivateData))
			return VO_ERR_SOURCE_END;
		PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
		VO_BOOL bRet = VO_FALSE;
		while(!bRet)
		{
			if(!pDataParser->Step())
				return VO_ERR_SOURCE_END;
			bRet = m_pStreamFileIndex->GetKeyFrameEntry(&pGet);
		}
		if(2 == m_pGlobeBuffer->Get(&pThumbNailInfo->pSampleData, m_pSampleData, pGet->pos_in_buffer & VO_MAXS32, pGet->size))
			pThumbNailInfo->pSampleData = m_pSampleData;
		pThumbNailInfo->uSampleSize = sample_size;
		pThumbNailInfo->ullFilePos = pDataParser->GetCurrParseFilePos();
		pThumbNailInfo->ullTimeStamp = pGet->time_stamp;
		m_ThumbnailBufferUsedSize = sample_size;
		nResult = VO_ERR_SOURCE_OK;
	}
	return nResult;
}
VO_U32 CAsfVideoTrack::GetNextKeyFrameSize()
{
	if(VO_MAXU32 == m_dwFileIndexInFastPlay)
		return VO_ERR_SOURCE_END;

	VO_U64 ullFilePos = GetFilePosByFileIndex(m_dwFileIndexInFastPlay);
	if(VO_MAXU64 == ullFilePos)
		return VO_ERR_SOURCE_END;
	m_pReader->GetSizeofNearKeyFrame(ullFilePos);

	if (m_iSizeMaxKeyFrame < m_dwSizeCurrentKeyFrame)
	{
		m_iSizeMaxKeyFrame = m_dwSizeCurrentKeyFrame;
		m_iPosMaxKeyFrame = ullFilePos;
	}

	VO_U64 ullNextFilePos = 0;
	do 
	{
		m_dwFileIndexInFastPlay++;
		ullNextFilePos = GetFilePosByFileIndex(m_dwFileIndexInFastPlay);
	} while (ullNextFilePos == ullFilePos);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfVideoTrack::GetSampleK(VO_SOURCE_SAMPLE* pSample)
{
	if(VOTT_VIDEO != m_nType)
		return VO_ERR_NOT_IMPLEMENT;

	if(VO_MAXU32 == m_dwFileIndexInFastPlay)
		return VO_ERR_SOURCE_END;

	VO_U64 ullFilePos = GetFilePosByFileIndex(m_dwFileIndexInFastPlay);
	if(VO_MAXU64 == ullFilePos)
		return VO_ERR_SOURCE_END;

	m_pReader->Flush();

	//do parse until get key frame entry!!
	CvoFileDataParser* pDataParser = m_pStreamFileReader->GetFileDataParserPtr();
	if(!pDataParser->SetStartFilePos(ullFilePos))
		return VO_ERR_SOURCE_END;

	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	VO_BOOL bRet = VO_FALSE;
	while(!bRet)
	{
		if(!pDataParser->Step())
			return VO_ERR_SOURCE_END;

		bRet = m_pStreamFileIndex->GetKeyFrameEntry(&pGet);
	}

	//sample ready!!
	pSample->Duration = 1;
	pSample->Time = pGet->time_stamp;

	VO_U32 dwMaxSampleSize = 0;
	GetMaxSampleSize(&dwMaxSampleSize);
	if(pGet->size > dwMaxSampleSize)
	{
		pSample->Size = 0;
		return VO_ERR_SOURCE_OK;
	}

	pSample->Size = pGet->size;
	if(2 == m_pGlobeBuffer->Get(&pSample->Buffer, m_pSampleData, pGet->pos_in_buffer & VO_MAXS32, pGet->size))
		pSample->Buffer = m_pSampleData;

	pSample->Size |= 0x80000000;

	VO_U64 ullNextFilePos = 0;
	do 
	{
		m_dwFileIndexInFastPlay++;
		ullNextFilePos = GetFilePosByFileIndex(m_dwFileIndexInFastPlay);
	} while (ullNextFilePos == ullFilePos);

	return VO_ERR_SOURCE_OK;
}
