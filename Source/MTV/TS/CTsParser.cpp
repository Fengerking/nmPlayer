
#include "CTsParser.h"
#include "CDumper.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "voCheck.h"

#define THE_MAX_TIME_GAP    15000

CTsParser::CTsParser(VO_MEM_OPERATOR* pMemOp)
: CBaseParser(pMemOp)
, m_pTsCtrl(VO_NULL)
, m_nFirstTimeStamp(0xFFFFFFFFFFFFFFFFLL)
, program_time(0)
{
	InitAllAVCache();
    InitAllFileDump();
    ResetCheckRollbackContext();
    m_ulCurrentVideoPID = 0xffffffff;
    m_ulCurrentAudioPID = 0xffffffff;
	m_ulCurrentTextPID = 0xffffffff;
    m_uFlagForTimeStampOffset = 0;
    m_pHCheck = NULL;
}

CTsParser::~CTsParser(void)
{
    CloseAllFileDump();
	ResetAllAVCache();
	Close();
#ifdef _VO_CHK_LICENSE
    if(m_pHCheck != NULL)
    {
        voCheckLibUninit(m_pHCheck);
        m_pHCheck = NULL;
    }
#endif // _VO_CHK_LICENSE
	VOLOGUNINIT();
}

void CTsParser::OnParsed(VO_PARSER_OUTPUT_BUFFER* pBuf)
{
	((CTsParser*)pBuf->pUserData)->HandleParseResult(pBuf);
}

VO_BOOL CTsParser::PreprocessTimestamp(VO_U32 nOutputType, VO_MTV_FRAME_BUFFER* pBuf, VO_U32 ulPID)
{
	//return VO_TRUE;
	int    iIndex = 0xffff;
	CheckTimestampCache* pCache = NULL;

	if(pBuf == NULL)
	{
		return VO_TRUE;
	}

    iIndex = GetCacheIndexCacheByPID(ulPID);
	if(iIndex == 0xffff)
	{
		return VO_TRUE;
	}

    if(iIndex >= TS_MAX_TRACK_COUNT_AUDIO_VIDEO)
    {
		VOLOGI("Invalid cache data!");
		return VO_FALSE;
    }

	pCache = m_pAVCheckTimestampCache[iIndex];
	if(pCache == NULL)
	{
		return VO_TRUE;
	}
		

	if(pBuf->nStartTime != pCache->nLastTimestamp)
	{
		if(pCache->nBufCount == 0)
		{
			pCache->nLastTimestamp	= pBuf->nStartTime;
			return VO_TRUE;
		}
			
		// issue all cache
		pCache->CalculateAvgTS(pBuf->nStartTime);//(pBuf->nStartTime - pCache->nLastTimestamp)/(pCache->nBufCount+1);
		for(VO_U32 n=0; n<pCache->nBufCount; n++)
		{
			CDumper::WriteLog((char *)"Issue continuous %s frame, ts = %d", nOutputType == VO_PARSER_OT_AUDIO?"audio":"video", pCache->CheckTimestampBuf[n].nStartTime);
			
			IssueParseResult(nOutputType, &(pCache->CheckTimestampBuf[n]));
		}
			
		//pCache->nBufCount		= 0;
		pCache->Reset();
		pCache->nLastTimestamp	= pBuf->nStartTime;
		return VO_TRUE;
	}
	else
	{
		if(!pCache->InsertFrame(pBuf))
			VOLOGI("Cache is full!!!");
	}

	return VO_FALSE;
}

void CTsParser::HandleParseResult(VO_PARSER_OUTPUT_BUFFER* pBuf)
{
    if(pBuf == NULL)
    {
        return;
    }

	m_nStreamID = pBuf->nStreamID;
    VO_PARSER_STREAMINFO * pTrackInfo = ( VO_PARSER_STREAMINFO * )pBuf->pOutputData;
	VO_MTV_FRAME_BUFFER        varMtvBuffer = {0};

	switch (pBuf->nType)
	{
	case VO_PARSER_OT_AUDIO:
	case VO_PARSER_OT_VIDEO:
	case VO_PARSER_OT_TEXT:
		{
			// tag: 20100422 process the time stamp offset

            //Only Issue one Audio Track

			/*
			if(pBuf->nType == VO_PARSER_OT_AUDIO)
			{
                if(m_ulCurrentAudioPID == 0xffffffff)
				{
				    m_ulCurrentAudioPID = pBuf->nStreamID;
				}
				else
				{
				    if(m_ulCurrentAudioPID != pBuf->nStreamID)
					{
						return;
					}
				}
			}


			//Only Issue one Video Track
			if(pBuf->nType == VO_PARSER_OT_VIDEO)
			{
				if(m_ulCurrentVideoPID == 0xffffffff)
				{
					m_ulCurrentVideoPID = pBuf->nStreamID;
				}
				else
				{
					if(m_ulCurrentVideoPID != pBuf->nStreamID)
					{
						return;
					}
				}
			}

			//Only Issue one Video Track
			if(pBuf->nType == VO_PARSER_OT_TEXT)
			{
				if(m_ulCurrentTextPID == 0xffffffff)
				{
					m_ulCurrentTextPID = pBuf->nStreamID;
				}
				else
				{
					if(m_ulCurrentTextPID != pBuf->nStreamID)
					{
						return;
					}
				}
			}
			*/
            
            DumpFrame(pBuf);
			VO_MTV_FRAME_BUFFER* buf = (VO_MTV_FRAME_BUFFER*)pBuf->pOutputData;

			if (m_nFirstTimeStamp == 0xFFFFFFFFFFFFFFFFLL)
			{
				if (m_pTsCtrl->GetTimeStampOffset() != (0xFFFFFFFFFFFFFFFFLL) && m_pTsCtrl->GetTimeStampOffset() < buf->nStartTime)
				{
					m_nFirstTimeStamp = m_pTsCtrl->GetTimeStampOffset();
				}
				else
				{
					m_nFirstTimeStamp = buf->nStartTime;
					VOLOGI("the first time stamp:%lld, ", m_nFirstTimeStamp);
				}
			}
			if (buf->nStartTime >= m_nFirstTimeStamp)
			{
			    if(m_uFlagForTimeStampOffset != TS_SET_WITHOUT_TIMESTAMP_OFFSET)
                {         
    				buf->nStartTime -= m_nFirstTimeStamp;

    				if(pBuf->nType == VO_PARSER_OT_VIDEO)
    				{
    					m_ulVideoTimeStampRollbackCount = 0;
    				}

    				if(pBuf->nType == VO_PARSER_OT_AUDIO)
    				{
    					m_ulAudioTimeStampRollbackCount = 0;
    				}
                }
			}
			else
			{
			    if(m_uFlagForTimeStampOffset != TS_SET_WITHOUT_TIMESTAMP_OFFSET)
                {         
				    if((m_nFirstTimeStamp-buf->nStartTime) < THE_MAX_TIME_GAP )
				    {
					    buf->nStartTime = 0;
					    VOLOGI("buf->nStartTime is smaller than FirstTimeStamp");
				    }
				    else
				    {
					    IssueAllCacheFrame();
					    IssueParseResult(VO_PARSER_OT_TS_TIMESTAMP_ROLLBACK, &varMtvBuffer);
					    VOLOGI("TimeStamp rollback!, the New first timestamp::%lld", buf->nStartTime);
					    m_nFirstTimeStamp = buf->nStartTime;
					    buf->nStartTime = 0;
                    }
                }
                else
                {
				    if((m_nFirstTimeStamp-buf->nStartTime) > THE_MAX_TIME_GAP )
				    {
					    IssueAllCacheFrame();
					    IssueParseResult(VO_PARSER_OT_TS_TIMESTAMP_ROLLBACK, &varMtvBuffer);
					    VOLOGI("TimeStamp rollback!, the New first timestamp::%lld", buf->nStartTime);
					    m_nFirstTimeStamp = buf->nStartTime;
                    }
                }
			}

			if(!PreprocessTimestamp(pBuf->nType, (VO_MTV_FRAME_BUFFER*)pBuf->pOutputData, pBuf->nStreamID))
				return;

			IssueParseResult(pBuf->nType, pBuf->pOutputData);
		}
		break;
	case VO_PARSER_OT_ERROR:
	case VO_PARSER_OT_STREAMINFO:
		{
			VO_U32 t = voOS_GetSysTime() - program_time;
			m_nStreamID = pTrackInfo->ulStreamId;
			CDumper::WriteLog((char *)"Stream info detected use time = %d", t);

            if(pBuf->nType == VO_PARSER_OT_STREAMINFO)
			{
				if(pTrackInfo != NULL)
				{
					CreateDumpByStreamId(pTrackInfo->ulStreamId);
					if(pTrackInfo->nAudioCodecType == VO_AUDIO_Coding_MAX)
					{
						VOLOGI("The Track is ID3 private data in Ts");
						break;
					}
					else
					{
					    if(pTrackInfo->eMediaType ==  VO_PARSER_MEDIA_TYPE_EX_AUDIO)
					    {
							VOLOGI("Create Cache For Audio!");
							CreateCacheByPID(pTrackInfo->ulStreamId, VO_PARSER_OT_AUDIO);
						    m_ulCurrentWorkMode |= TS_PARSER_AUDIO_ONLY;
					    }

					    if(pTrackInfo->eMediaType == VO_PARSER_MEDIA_TYPE_EX_VIDEO)
					    {
						    VOLOGI("Create Cache For Video!");
							CreateCacheByPID(pTrackInfo->ulStreamId, VO_PARSER_OT_VIDEO);
						    m_ulCurrentWorkMode |= TS_PARSER_VIDEO_ONLY;
					    }
					}
				}
			}

			IssueParseResult(pBuf->nType, pBuf->pOutputData);
		}
		break;
	case VO_PARSER_OT_TS_PROGRAM_INFO:
		{
			VO_U32 t = voOS_GetSysTime() - program_time;
			CDumper::WriteLog((char *)"Program info detected use time = %d", t);

			IssueParseResult(pBuf->nType, pBuf->pOutputData);
		}
		break;
	case VO_PARSER_OT_PRIVATE_DATA:
		{
			VO_MTV_FRAME_BUFFER* buf = (VO_MTV_FRAME_BUFFER*)pBuf->pOutputData;
			if(buf->nSize > 3)
			{
				VOLOGI("TimeStamp in Private Data %lld", buf->nStartTime);
				VOLOGI("private data size:%d", buf->nSize);
				VOLOGI("%c%c%c, %c%c%c%c%c", buf->pData[0], buf->pData[1], buf->pData[2], buf->pData[54], buf->pData[55], buf->pData[56], buf->pData[57], buf->pData[58]);
			}

			DumpFrame(pBuf);
			IssueParseResult(pBuf->nType, pBuf->pOutputData);
			break;
		}
	default:
		{
			if (m_pCallback)
			{
				pBuf->pUserData	= m_pUserData;
				m_pCallback(pBuf);
			}
		}
		break;
	}
}


VO_U32 CTsParser::Open(VO_PARSER_INIT_INFO* pParam)
{
	VO_U32 ulRet = 0;
    if(pParam == NULL)
    {
		return VO_ERR_PARSER_OPEN_FAIL;
    }
	VOLOGINIT(pParam->strWorkPath);
#ifdef _VO_CHK_LICENSE
	ulRet = voCheckLibInit(&m_pHCheck, VO_INDEX_SRC_TSP, VO_LCS_WORKPATH_FLAG,0,pParam->strWorkPath);
	VOLOGI("the ret value for check:%d", ulRet);
	if(m_pHCheck == NULL)
	{
		VOLOGI("The Ret Handle is NULL!");
	}
	if(ulRet != VO_ERR_NONE )
	{
		if(m_pHCheck != NULL)
		{
			voCheckLibUninit(m_pHCheck);
		}
		m_pHCheck = NULL;
		return ulRet;
	}
#endif // _VO_CHK_LICENSE   

	if(program_time == 0)
		program_time = voOS_GetSysTime();

	CBaseParser::Open(pParam);

	Close();
	
	m_pTsCtrl = new CTsParseCtroller;
    if(m_pTsCtrl == NULL)
    {
		return VO_ERR_PARSER_OPEN_FAIL;
    }

	// tag: 20100423
	m_pTsCtrl->SetParseType(PARSE_ALL);
	//m_pTsCtrl->SetParseType(PARSE_PLAYBACK);
	// end
	m_pTsCtrl->Open(CTsParser::OnParsed, this);

	return VO_ERR_PARSER_OK;
}

VO_U32 CTsParser::Close()
{
	if (m_pTsCtrl)
	{
		m_pTsCtrl->Close();
		delete m_pTsCtrl;
		m_pTsCtrl = VO_NULL;
	}

	CBaseParser::Close();
	return VO_ERR_PARSER_OK;
}

VO_U32 CTsParser::Process(VO_PARSER_INPUT_BUFFER* pBuffer)
{
#ifdef _VO_CHK_LICENSE   
    if(m_pHCheck == NULL)
    {
		return VO_ERR_PARSER_OPEN_FAIL;
    }
#endif // _VO_CHK_LICENSE

	//VOLOGI("Recv data = %x, len = %d", pBuffer->pBuf[0], pBuffer->nBufLen);
    if(pBuffer == NULL)
    {
		return VO_ERR_PARSER_OK;
    }

	if (m_pTsCtrl)
	{
		// test code
		if (pBuffer->nFlag == VO_PARSER_FLAG_STREAM_CHANGED)
		{
			program_time = voOS_GetSysTime();
            ResetAllAVCache();

			m_ulCurrentAudioPID = 0xffffffff;
			m_ulCurrentVideoPID = 0xffffffff;

			//Reset the Check Rollback context 
			ResetCheckRollbackContext();
		}
		else if(pBuffer->nFlag == VO_PARSER_FLAG_STREAM_RESET_ALL)
		{
			program_time = voOS_GetSysTime();
			ResetAllAVCache();

			m_nFirstTimeStamp = 0xFFFFFFFFFFFFFFFFLL;
			
			m_ulCurrentAudioPID = 0xffffffff;
			m_ulCurrentVideoPID = 0xffffffff;

			ResetCheckRollbackContext();
			
			VOLOGI("Reset the FirstTimeStamp");
		}
		//end
		return m_pTsCtrl->Process(pBuffer);
	}

	return VO_ERR_PARSER_OK;
}

VO_U32 CTsParser::doSetParam(VO_U32 uID, VO_PTR pParam)
{
    if(uID == VO_PARSER_OT_TS_WITHOUT_TIMESTAMP_OFFSET)
    {
        m_uFlagForTimeStampOffset = TS_SET_WITHOUT_TIMESTAMP_OFFSET;
        VOLOGI("Set the Parameter without timestamp offset!");
    }

	if (m_pTsCtrl)
	{
		return m_pTsCtrl->SetParam(uID, pParam);
	}

	return VO_ERR_PARSER_FAIL;
}

VO_U32 CTsParser::doGetParam(VO_U32 uID, VO_PTR pParam)
{
	if (m_pTsCtrl)
	{
		return m_pTsCtrl->GetParam(uID, pParam);
	}

	return VO_ERR_PARSER_FAIL;
}

VO_U32 CTsParser::SelectProgram(VO_U32 uStreamID)
{
	if (m_pTsCtrl)
	{
		return m_pTsCtrl->SelectProgram(uStreamID);
	}

	return VO_ERR_PARSER_FAIL;
}

VO_VOID CTsParser::ResetCheckRollbackContext()
{
    m_ulCurrentWorkMode = TS_PARSER_UNKNOWN;
    m_ulAudioTimeStampRollbackCount = 0;
    m_ulVideoTimeStampRollbackCount = 0;
}


VO_VOID CTsParser::ResetForFirstTimeStampForRollback()
{
	program_time = voOS_GetSysTime();    
	ResetAllAVCacheData();
	m_nFirstTimeStamp = 0xFFFFFFFFFFFFFFFFLL;
	VOLOGI("Reset the FirstTimeStamp For TimeStamp rollback!");
}

VO_BOOL  CTsParser::CheckNeedResetFirstTime()
{
    switch(m_ulCurrentWorkMode)
	{
        case TS_PARSER_AUDIO_VIDEO:
		{
            if(m_ulAudioTimeStampRollbackCount>TS_MAX_ROLLBACK_COUNT && m_ulVideoTimeStampRollbackCount>TS_MAX_ROLLBACK_COUNT)
			{
				return VO_TRUE;
			}
			break;
		}

		case TS_PARSER_AUDIO_ONLY:
		{
			if(m_ulAudioTimeStampRollbackCount>TS_MAX_ROLLBACK_COUNT)
			{
				return VO_TRUE;
			}
			break;
		}
		case TS_PARSER_VIDEO_ONLY:
		{
			if(m_ulVideoTimeStampRollbackCount>TS_MAX_ROLLBACK_COUNT)
			{
				return VO_TRUE;
			}
			break;
		}
		default:
		{
			break;
		}
	}

	return VO_FALSE;
}

VO_VOID   CTsParser::IssueAllCacheFrame()
{
    VO_U64   ulEndTime = 0;
	CheckTimestampCache* pCache = NULL;


    for(int i=0; i<m_ulCurrentCacheCount; i++)
	{
		pCache = m_pAVCheckTimestampCache[i];
		if(pCache != NULL)
		{
			if(pCache->nBufCount != 0)
			{
				ulEndTime = pCache->nLastTimestamp+1;
				pCache->CalculateAvgTS(ulEndTime);
				for(VO_U32 n=0; n<pCache->nBufCount; n++)
				{
					IssueParseResult(pCache->ulType, &(pCache->CheckTimestampBuf[n]));
				}

				pCache->Reset();
			}
		}
	}

	return ;
}

void   CTsParser::ResetAllAVCache()
{
    for(int i=0; i<TS_MAX_TRACK_COUNT_AUDIO_VIDEO; i++)
	{
		if(m_pAVCheckTimestampCache[i] != NULL)
		{
			delete m_pAVCheckTimestampCache[i];
			m_pAVCheckTimestampCache[i] = NULL;
		}
	}

	m_ulCurrentCacheCount = 0;
}

void   CTsParser::InitAllAVCache()
{
	for(int i=0; i<TS_MAX_TRACK_COUNT_AUDIO_VIDEO; i++)
	{
        m_pAVCheckTimestampCache[i] = NULL;
	}

    m_ulCurrentCacheCount= 0;
}


void   CTsParser::ResetAllAVCacheData()
{
	for(int i=0; i<TS_MAX_TRACK_COUNT_AUDIO_VIDEO; i++)
	{
		if(m_pAVCheckTimestampCache[i] != NULL)
		{
			m_pAVCheckTimestampCache[i]->Reset();
		}
	}
}

int          CTsParser::GetCacheIndexCacheByPID(VO_U32  ulPID)
{
    int    iIndex = 0xffff;
	for(int i=0; i<TS_MAX_TRACK_COUNT_AUDIO_VIDEO; i++)
	{
		if(m_pAVCheckTimestampCache[i] != NULL && m_pAVCheckTimestampCache[i]->ulPID == ulPID)
		{
			iIndex = i;
			break;
		}
	}

	return iIndex;
}

void      CTsParser::CreateCacheByPID(VO_U32  ulPID, VO_U32 ulType)
{
	CheckTimestampCache*   pCache = NULL;
    VO_U32                 ulIndex = 0;
	int                    iIndex = 0;

    iIndex = GetCacheIndexCacheByPID(ulPID);
	VOLOGI("The iIndex value:%d", iIndex);
	if(iIndex != 0xffff )
	{
		delete m_pAVCheckTimestampCache[iIndex];
        m_pAVCheckTimestampCache[iIndex] = NULL;
	}
	else
    {
		iIndex = m_ulCurrentCacheCount;
    }


	m_pAVCheckTimestampCache[iIndex] = new CheckTimestampCache;
	if(m_pAVCheckTimestampCache[iIndex] == NULL)
	{
		VOLOGI("Create Cache Error!");
		return;
	}

	m_pAVCheckTimestampCache[iIndex]->ulPID = ulPID;
    m_pAVCheckTimestampCache[iIndex]->ulType = ulType;
	m_pAVCheckTimestampCache[iIndex]->Reset();
	m_ulCurrentCacheCount++;
}

VO_VOID         CTsParser::InitAllFileDump()
{
#ifdef _DUMP_FRAME
	int iIndex= 0;
    for(iIndex=0; iIndex<0x2000; iIndex++)
    {
        m_pFileFrameData[iIndex] = NULL;
		m_pFileFrameSize[iIndex] = NULL;
        m_pFileFrameTimeStamp[iIndex] = NULL;
    }
#endif
}

VO_VOID         CTsParser::ResetAllFileDump()
{
#ifdef _DUMP_FRAME
	int iIndex= 0;
	for(iIndex=0; iIndex<0x2000; iIndex++)
	{
		if(m_pFileFrameData[iIndex] != NULL)
		{
			fclose(m_pFileFrameData[iIndex]);
            m_pFileFrameData[iIndex] = NULL;
		}

		if(m_pFileFrameSize[iIndex] != NULL)
		{
			fclose(m_pFileFrameSize[iIndex]);
			m_pFileFrameSize[iIndex] = NULL;
		}

		if(m_pFileFrameTimeStamp[iIndex] != NULL)
		{
			fclose(m_pFileFrameTimeStamp[iIndex]);
			m_pFileFrameTimeStamp[iIndex] = NULL;
		}
	}
#endif
}

VO_VOID         CTsParser::CreateDumpByStreamId(VO_U32 ulPID)
{
#ifdef _DUMP_FRAME
	VO_CHAR    strDumpPath[256] = {0};
    FILE*      pFile = NULL;
    
    if(ulPID >= 0x2000)
    {
        return;
    }

	sprintf(strDumpPath, "C:\\Dump\\streaming_%d_data.dat", ulPID);
    pFile = fopen(strDumpPath, "wb");
	if(pFile != NULL)
    {
		m_pFileFrameData[ulPID] = pFile;
	}
   
    memset(strDumpPath, 0, 256);
	sprintf(strDumpPath, "C:\\Dump\\streaming_%d_size.dat", ulPID);
	pFile = fopen(strDumpPath, "wb");
	if(pFile != NULL)
	{
		m_pFileFrameSize[ulPID] = pFile;
	}

    memset(strDumpPath, 0, 256);
	sprintf(strDumpPath, "C:\\Dump\\streaming_%d_timestamp.dat", ulPID);
	pFile = fopen(strDumpPath, "wb");
	if(pFile != NULL)
	{
		m_pFileFrameTimeStamp[ulPID] = pFile;
	}
#endif    
}

VO_VOID         CTsParser::DumpFrame(VO_PARSER_OUTPUT_BUFFER* pBuf)
{
#ifdef _DUMP_FRAME
    FILE*   pFile = NULL;
    VO_U32  ulTimeStamp = 0;
    VO_U32  ulSize = 0;
	VO_MTV_FRAME_BUFFER*   pFrame = NULL;
    if(pBuf == NULL)
	{
		return;
	}

	pFrame = (VO_MTV_FRAME_BUFFER*)pBuf->pOutputData;
    if(pFrame == NULL)
	{
		return;
	}

    pFile = m_pFileFrameData[pBuf->nStreamID];
	fwrite(pFrame->pData, 1, pFrame->nSize, pFile);
	fflush(pFile);

	ulSize = pFrame->nSize;
	pFile = m_pFileFrameSize[pBuf->nStreamID];
	fwrite((VO_BYTE *)(&ulSize)+3, 1, 1, pFile);
	fwrite((VO_BYTE *)(&ulSize)+2, 1, 1, pFile);
	fwrite((VO_BYTE *)(&ulSize)+1, 1, 1, pFile);
	fwrite((VO_BYTE *)(&ulSize)+0, 1, 1, pFile);
	fflush(pFile);

	ulTimeStamp = pFrame->nStartTime;
	pFile = m_pFileFrameTimeStamp[pBuf->nStreamID];
	fwrite((VO_BYTE *)(&ulTimeStamp)+3, 1, 1, pFile);
	fwrite((VO_BYTE *)(&ulTimeStamp)+2, 1, 1, pFile);
	fwrite((VO_BYTE *)(&ulTimeStamp)+1, 1, 1, pFile);
	fwrite((VO_BYTE *)(&ulTimeStamp)+0, 1, 1, pFile);
	fflush(pFile);
#endif
}

VO_VOID         CTsParser::CloseAllFileDump()
{
#ifdef _DUMP_FRAME
	int iIndex= 0;
	for(iIndex=0; iIndex<0x2000; iIndex++)
	{
		if(m_pFileFrameData[iIndex] != NULL)
		{
			fclose(m_pFileFrameData[iIndex]);
			m_pFileFrameData[iIndex] = NULL;
		}

		if(m_pFileFrameSize[iIndex] != NULL)
		{
			fclose(m_pFileFrameSize[iIndex]);
			m_pFileFrameSize[iIndex] = NULL;
		}

		if(m_pFileFrameTimeStamp[iIndex] != NULL)
		{
			fclose(m_pFileFrameTimeStamp[iIndex]);
			m_pFileFrameTimeStamp[iIndex] = NULL;
		}
	}
#endif
}
