#include <stdio.h>
#include <stdlib.h>

#include "CASController.h"
#include "voOSFunc.h"
#include "cmnFile.h"
#include "voLog.h"
#include "vompType.h"
#include "voOSFunc.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef _IOS
#ifdef _DASH_SOURCE_
using namespace _DASH;
#endif

#ifdef _ISS_SOURCE_
using namespace _ISS;
#endif
#endif

CASController::CASController()
:
m_SourceBufferManager(1000,20000)
,m_uPreChunkDelay(0)
,m_uTimeStamp(0)
,m_uVideoDelay(0)
,m_bStop(VO_FALSE)
{
	m_pPlayer = NULL;
	m_pstrSource = NULL;
	m_pPlayer = new ASController_CLASS ();

	if(m_pPlayer) 
	{
		m_pPlayer->set_statuscallback (this, StatusOutData);
		m_pPlayer->set_datacallback (this, SendOutData);
	}
	ff= NULL;
	//VO_U32 xx = voOS_GetUTC();


	VOLOGI("1");
}

CASController::~CASController()
{
	if (m_pPlayer != NULL)

	{
		m_pPlayer->close();
		delete m_pPlayer;
		m_pPlayer = NULL;
	}
	if(m_pstrSource) 
		delete []m_pstrSource;

	m_SourceBufferManager.Flush();

	if(ff) fclose(ff);
}

VO_U32 CASController::Init(VO_PTR pSource , VO_U32 nFlag ,  VO_SOURCE2_INITPARAM * pInitParam  )
{
	VOLOGR("+Init");
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pPlayer == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	if(!pSource)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	
	if(m_pstrSource) 
		delete []m_pstrSource;

	VOLOGR("%s",(VO_CHAR*)pSource);
	int size = strlen((VO_CHAR*)pSource) ;
	m_pstrSource = new VO_CHAR[size +1];
	memset(m_pstrSource, 0x00, size+1);
	memcpy(m_pstrSource, pSource, size);
	VOLOGR("-Init");
    return ulRet;
}

/*these two func is temporary for old streaming*/
VO_VOID	CASController::SendOutData (VO_VOID * pUserData, VO_LIVESRC_SAMPLE * ptr_sample)
{
	
	CASController * pASC = (CASController *)pUserData;
	
	VO_U32 type = 0;

	if(ptr_sample->nTrackID  == VO_LIVESRC_OUTPUT_AUDIO)
	{
		VOLOGR("Audio come,TimeStamp:%lld,Size:%d", ptr_sample->Sample.Time,ptr_sample->Sample.Size);

		type = VO_SOURCE2_TT_AUDIO;
	}
	else if(ptr_sample->nTrackID  == VO_LIVESRC_OUTPUT_VIDEO)
	{
		VOLOGR("Video come,TimeStamp:%lld,Size:%d", ptr_sample->Sample.Time,ptr_sample->Sample.Size);
		type = VO_SOURCE2_TT_VIDEO;

	}

	VO_SOURCE2_TRACK_INFO *ti = NULL;
	VO_SOURCE2_SAMPLE sm2;
	memset(&sm2, 0x00, sizeof(VO_SOURCE2_SAMPLE));
	VO_SOURCE_SAMPLE *sm = &ptr_sample->Sample;

	sm2.uDuration = sm->Duration;
	sm2.uSize = sm->Size;
	sm2.uTime = sm->Time;

	if( !(sm->Flag & VO_LIVESRC_FLAG_BUFFER_NEW_FORMAT) )
		sm2.pBuffer = sm->Buffer;

	VO_U32 uCodecID = 0;
	if(sm->Flag & VO_LIVESRC_FLAG_BUFFER_FORCE_FLUSH)
	{
		VOLOGR("VO_LIVESRC_FLAG_BUFFER_FORCE_FLUSH");
		sm2.uFlag = VO_SOURCE2_FLAG_SAMPLE_FORCE_FLUSH;
	}
	else if(sm->Flag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE)
	{
		sm2.uFlag = VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE;
	}
	else if(sm->Flag & VO_LIVESRC_FLAG_BUFFER_EOS)
	{
		VOLOGR("IsVideo(%d) EOS.",ptr_sample->nTrackID -1);

		sm2.uFlag = VO_SOURCE2_FLAG_SAMPLE_EOS;
	}
	else if(sm->Flag & VO_LIVESRC_FLAG_BUFFER_HEADDATA || sm->Flag & VO_LIVESRC_FLAG_BUFFER_NEW_FORMAT)
	{
		VOLOGI("new format");
		int infoSize = sizeof( VO_SOURCE2_TRACK_INFO ) + sm->Size;
		ti = ( VO_SOURCE2_TRACK_INFO * )new VO_BYTE[ infoSize ];
		memset(ti, 0x00, infoSize);

		ti->uCodec = ptr_sample->nCodecType;
	
		ti->uHeadSize = sm->Size;
		sm2.uSize = 0;
		sm2.uFlag = VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT;
		ti->uTrackID = 1;
		ti->uTrackType = (ptr_sample->nTrackID == VO_LIVESRC_OUTPUT_AUDIO?VO_SOURCE2_TT_AUDIO: VO_SOURCE2_TT_VIDEO) ;
		ti->pHeadData = ((VO_PBYTE)ti) + sizeof(VO_SOURCE2_TRACK_INFO);
		memcpy(ti->pHeadData, sm->Buffer, sm->Size);
		sm2.pFlagData = (VO_PBYTE) ti;
	}
	else if(sm->Flag & VO_LIVESRC_FLAG_BUFFER_KEYFRAME)
	{
		uCodecID = ptr_sample->nCodecType;

		sm2.uFlag = VO_SOURCE2_FLAG_SAMPLE_KEYFRAME;
	}
	else
		uCodecID = ptr_sample->nCodecType;

//AVC-->H264
	if(uCodecID == VO_VIDEO_CodingH264)
		pASC->ConvertData(sm2.pBuffer,sm2.pBuffer,sm2.uSize);

	while(pASC->m_SourceBufferManager.AddBuffer(type,&sm2) == VO_RET_SOURCE2_NEEDRETRY)
	{
		VOLOGR("add buffer retry.sm2:%lld",sm2.uTime);
		voOS_Sleep(20);
	}
	if(ti)
		delete []ti;
}

VO_VOID	CASController::StatusOutData (VO_VOID * pUserData,VO_U32 id, VO_VOID * pBuffer)
{
	CASController * pISS = (CASController *)pUserData;

	//pISS->m_fStatus (pISS->m_pUserData, id, (_vosdk_int)pBuffer, 0 );
}


VO_U32 CASController::Uninit()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pPlayer == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	 m_pPlayer->close();

    return ulRet;
}

VO_U32 CASController::Open()
{
	VOLOGR("+Open");
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pPlayer == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(m_pPlayer->SetDataSource(m_pstrSource))
	{
		ulRet = VO_RET_SOURCE2_OK;
	}
	else
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
		ulRet = VO_RET_SOURCE2_FAIL;
	}
	VOLOGR("-Open");
// 	Close();
// 		ulRet = VO_RET_SOURCE2_FAIL;
    return ulRet;
}

VO_U32 CASController::Close()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
	voCAutoLock lock (&m_lockMutex);
	if (m_pPlayer == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	m_pPlayer->close();
	m_bStop = VO_TRUE;

    return ulRet;
}
VO_U32 CASController::Start()
{
	VOLOGR("+Start");
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pPlayer == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	m_pPlayer->run();
		VOLOGR("-Start");
    return ulRet;
}
	
VO_U32 CASController::Pause()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pPlayer == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	m_pPlayer->pause();
    return ulRet;
}
	
VO_U32 CASController::Stop()
{
	VOLOGR("+Stop");
	voCAutoLock lock (&m_lockMutex);

	VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pPlayer == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	m_SourceBufferManager.Flush();

	m_pPlayer->close();
	m_bStop = VO_TRUE;
	VOLOGR("-Stop");
    return ulRet;
}
	
VO_U32 CASController::Seek(VO_U64* pTimeStamp)
{
	VOLOGR("+Seek");
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
	
	if (m_pPlayer == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	m_SourceBufferManager.Flush();

	VO_U32 tt = (VO_U32) *pTimeStamp;
	tt = m_pPlayer->set_pos(tt);
	m_SourceBufferManager.set_pos( *pTimeStamp );
	*pTimeStamp = (VO_U64)tt;
	VOLOGR("-Seek");
    return ulRet;
}
	
VO_U32 CASController::GetDuration(VO_U64 * pDuration)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pPlayer == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	*pDuration = m_pPlayer->GetDuration();
    return ulRet;
}
	
VO_U32 CASController::GetSample(VO_U32 nOutPutType , VO_PTR pSample )
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	voCAutoLock lock (&m_lockMutex);
	if(m_bStop)
		return VO_RET_SOURCE2_FAIL;
	if (m_pPlayer == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	VO_SOURCE2_SAMPLE * ps = (VO_SOURCE2_SAMPLE*)pSample;
	
	if(nOutPutType == VO_SOURCE2_TT_AUDIO)
	{
		m_uTimeStamp = ps->uTime;
	}
	else if(nOutPutType == VO_SOURCE2_TT_VIDEO)
	{
		m_uVideoDelay = ps->uDuration;
	}
	//VOLOGR("+GetSample.Type:%d Time:%lld",nOutPutType,uTime )

	ulRet = m_SourceBufferManager.GetBuffer(nOutPutType, pSample);
	if(ulRet == VO_RET_SOURCE2_OK&&nOutPutType == VO_SOURCE2_TT_VIDEO )
	{

		VOLOGR("Get Video OK.%lld",ps->uTime );
		
		ps->uFlag |= VO_SOURCE2_FLAG_SAMPLE_FRAMETYPEUNKNOWN;
		
		if(voOS_GetSysTime() - m_uPreChunkDelay >200)
		{
			m_pPlayer->set_VideoDelayTime(&m_uVideoDelay, m_uTimeStamp);

			m_uPreChunkDelay = voOS_GetSysTime();
		}
/*		if(!ff)
			ff = fopen("/sdcard/distretix_dump","wb");

		if(ff)
		{
			if(ps->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT)
			{
				VO_SOURCE2_TRACK_INFO *ti = (VO_SOURCE2_TRACK_INFO*)ps->pFlagData;
				fwrite(ti->pHeadData,1,ti->uHeadSize,ff);
			}
			else
				fwrite(ps->pBuffer,1,ps->uSize&0x7fffffff,ff);
		}
		*/
		
	}
	else if(ulRet == VO_RET_SOURCE2_OK&&nOutPutType == VO_SOURCE2_TT_AUDIO )
	{
		//VOLOGR("Get Audio OK.%lld",ps->uTime );
	}
	
// 	if(ulRet == VO_RET_SOURCE2_OK && nOutPutType == VO_SOURCE2_TT_VIDEO)
// 	{
// 		VO_SOURCE2_SAMPLE * ps = (VO_SOURCE2_SAMPLE*)pSample;
// 		if(ps->pReserve1 && (VO_SOURCE2_FLAG_SAMPLE_FRAMEDROPPED == *((VO_U32*)ps->pReserve1)))
// 		{
// 			VOLOGW("Drop Frame occured!")
// 			VO_U32 videoDelay = 1000;
// 			m_pPlayer->set_VideoDelayTime(&videoDelay, uTime );
// 			//m_pPlayer->set_VideoDelayTime(&videoDelay, ps->uTime );
// 		}
// 	}
#ifdef _VOLOG_RUN
	ps = (VO_SOURCE2_SAMPLE*)pSample;

	//VOLOGR("-GetSample.Rec:%x,%lld",ulRet,ps->uTime );
#endif
	
    return ulRet;
}

//#ifdef _NEW_SOURCEBUFFER

VO_U32 CASController::GetProgramCount( VO_U32 *pProgramCount )
{
	VOLOGR("+GetProgramCount");
	VO_U32   ulRet = VO_RET_SOURCE2_OK;
	if (m_pPlayer == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	*pProgramCount = 1;
	VOLOGR("-GetProgramCount");

	return ulRet;
}

VO_U32 CASController::GetProgramInfo( VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo )
{
	VOLOGR("+GetProgramInfo");
	VO_U32   ulRet = VO_RET_SOURCE2_OK;

	//return VO_RET_SOURCE2_NOIMPLEMENT;
	if (m_pPlayer == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	ulRet = m_pPlayer->GetProgramInfo(nProgram,ppProgramInfo);

#if 0
 	VOLOGR("ProgramInfo %p StreamCount %d",*ppProgramInfo, (*ppProgramInfo)->uStreamCount);
 	if(ulRet != VO_RET_SOURCE2_OK)
		return ulRet;
	for(int n =0; n < (*ppProgramInfo)->uStreamCount; n ++)
 	{
		VO_SOURCE2_STREAM_INFO *pStreamInfoX = (*ppProgramInfo)->ppStreamInfo[n];
		VOLOGR("StreamInfo %p BitRate:%d TrackCount:%d",pStreamInfoX,pStreamInfoX->uBitrate, pStreamInfoX->uTrackCount);
 		for(int s =0; s < pStreamInfoX->uTrackCount; s++)
 		{
			VO_SOURCE2_TRACK_INFO *pTrackInfo = pStreamInfoX->ppTrackInfo[s] ;
			VOLOGR("TrackInfo %p(%s) BitRate:%d (%d,%p)",pTrackInfo, pTrackInfo->uTrackType == VO_SOURCE2_TT_AUDIO? "a":"v",pTrackInfo->uBitrate, pTrackInfo->uHeadSize, pTrackInfo->pHeadData);
 			pTrackInfo->uHeadSize = 0;
 		}
 	}
#endif
	VOLOGR("-GetProgramInfo");

	return ulRet;

}

VO_U32 CASController::GetCurTrackInfo( VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo )
{
	
	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 CASController::SelectProgram( VO_U32 nProgram )
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 CASController::SelectStream( VO_U32 nStream )
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 CASController::SelectTrack( VO_U32 nTrack )
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}
// #else
// VO_U32 CASController::GetStreamCount(VO_U32 *pStreamCount)
// {
//     VO_U32   ulRet = VO_RET_SOURCE2_OK;
// 
// 	if (m_pPlayer == NULL)
// 	{
// 	    return VO_RET_SOURCE2_EMPTYPOINTOR;
// 	}
// 	
// 	*pStreamCount = m_pPlayer->GetTrackNumber();
// 	return ulRet;
// }
// 	
// VO_U32 CASController::GetStreamInfo(VO_U32 nStreamID, VO_SOURCE2_STREAM_INFO **ppStreamInfo)
// {
//     VO_U32   ulRet = VO_RET_SOURCE2_OK;
// 
// 	if (m_pPlayer == NULL)
// 	{
// 	    return VO_RET_SOURCE2_EMPTYPOINTOR;
// 	}
// 	return VO_RET_SOURCE2_NOIMPLEMENT;
// 	//ulRet = m_pPlayer->GetStreamInfo_HLS(nStreamID, ppStreamInfo);
// 	return ulRet;
// }
// 	
// VO_U32 CASController::GetCurSelTrackInfo(VO_U32 nOutPutType , VO_SOURCE2_TRACK_INFO **ppTrackInfo)
// {
//     VO_U32   ulRet = VO_RET_SOURCE2_OK;
// 
// 	if (m_pPlayer == NULL)
// 	{
// 	    return VO_RET_SOURCE2_EMPTYPOINTOR;
// 	}
// 
// 	//m_pPlayer->GetCurSelTrackInfo(nOutPutType, ppTrackInfo);
// 	return VO_RET_SOURCE2_NOIMPLEMENT;
// 	return ulRet;
// }
// 	
// VO_U32 CASController::SelectTrack(VO_U32 nStreamID , VO_U32 nSubStreamID , VO_U32 nTrackID )
// {
//     VO_U32   ulRet = VO_RET_SOURCE2_OK;
// 
// 	if (m_pPlayer == NULL)
// 	{
// 	    return VO_RET_SOURCE2_EMPTYPOINTOR;
// 	}
// 	return VO_RET_SOURCE2_NOIMPLEMENT;
// 	//ulRet = m_pPlayer->SelectTrack(nStreamID, nSubStreamID, nTrackID);
// 	return ulRet;
// }
// 
// #endif //_NEW_SOURCEBUFFER
	
VO_U32 CASController::GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pPlayer == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	return VO_RET_SOURCE2_NOIMPLEMENT;
	return ulRet;
}
	
VO_U32 CASController::GetParam(VO_U32 nParamID, VO_PTR pParam)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pPlayer == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	return ulRet;
}
	
VO_U32 CASController::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;


    VOLOGR("the nParamID:%d", nParamID);
	if (m_pPlayer == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	switch(nParamID)
	{
	    case VO_PID_SOURCE2_LIBOP:
		{		
			m_pPlayer->SetLibOp((VO_LIB_OPERATOR*) pParam);
			m_SourceBufferManager.SetLibOp( (VO_SOURCE2_LIB_FUNC *) pParam );
			break;
		}
		case VO_PID_SOURCE2_DRMCALLBACK:
		{
			m_pPlayer->SetDrmCallBack(pParam);
			break;
		}
		case VO_PID_LIVESRC_VIDEODELAY:
		{
			m_pPlayer->set_VideoDelayTime (pParam);
			break;
		}
		case VO_PID_COMMON_LOGFUNC:
			{				
				m_pPlayer->SetParam(nParamID, pParam);
			}
			break;
		case VO_PID_SOURCE2_BACAP:
			{
				m_pPlayer->SetParam(nParamID, pParam);
				break;
			}
#ifdef _USE_VERIFYCALLBACK_
		case VO_PID_SOURCE2_HTTPVERIFICATIONCALLBACK:
			{
				m_pPlayer->SetVerifyCallBack(pParam);
				break;
			}
		case VO_PID_SOURCE2_DOHTTPVERIFICATION:
			{
				ulRet = m_pPlayer->DoVerifyCallBack(pParam);
				break;				
			}
#endif
		default:
		{
			break;
		}
	}

	return ulRet;
}
	


VO_BOOL CASController::ConvertData (VO_PBYTE pOutData, VO_PBYTE pInData, VO_U32 nInSize)
{
	if (pOutData == NULL)
		return VO_TRUE;

	VO_PBYTE pBuffer = pInData;
	VO_U32	 nFrameLen = 0;
	VO_U32 nNalLen = 4;
	VO_U32 nNalWord = 0X01000000;
	VO_U32 nVideoSize = 0;

	int i = 0;
	while (pBuffer - pInData + nNalLen < nInSize)
	{
		nFrameLen = *pBuffer++;
		for (i = 0; i < (int)nNalLen - 1; i++)
		{
			nFrameLen = nFrameLen << 8;
			nFrameLen += *pBuffer++;
		}

		if(nFrameLen > nInSize)
			return VO_FALSE;

		if (nNalLen == 3 || nNalLen == 4)
		{
			memcpy ((pBuffer - nNalLen), &nNalWord, nNalLen);
		}
		else
		{
			memcpy (pOutData + nVideoSize, &nNalWord, 4);
			nVideoSize += 4;
			memcpy (pOutData + nVideoSize, pBuffer, nFrameLen);
			nVideoSize += nFrameLen;
		}

		pBuffer += nFrameLen;
	}

	return VO_TRUE;
}
