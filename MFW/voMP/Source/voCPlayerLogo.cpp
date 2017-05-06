	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCPlayerLogo.cpp

	Contains:	voCPlayerLogo data file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-07_10		JBF			Create file

*******************************************************************************/
#include "voOSFunc.h"
#include "cmnFile.h"
#include "voVideo.h"
#include "voAudio.h"
#include "voSource.h"
#include "voCDataSource.h"
#include "voCPlayerLogo.h"
#include "voCheck.h"

#if defined (_IOS) || defined (_MAC_OS)
#include "voOSFunc.h"
#include "voHalInfo.h"
#include <mach/mach.h>
#endif

#define LOG_TAG "voCPlayerLogo"
#include "voLog.h"

voCPlayerLogoInfo::voCPlayerLogoInfo(int nSourceType)
	:m_nSourceType(nSourceType)
	,m_nCPULoading(0)
	,m_nSysLoading(0)
	,m_nAPLoading(0)
	,m_nReadCount(0)
	,m_nReadIndex(0)
	,m_nGetStartTime(0)
	,m_nGetEndTime(0)
	,m_nOutTimeStamp(0)
	,m_nGetSuccess(1)
	,m_nDecStartTime(0)
	,m_nDecSuccess(1)
	,m_nDecEndTime(0)
	,m_nRdBufferNum(0)
	,m_nRdStartTime(0)
	,m_nRdEndTime(0)
	,m_nStartSystemTime(0)
	,m_nLastRenderTime(0)
	,m_pIntDecError(NULL)
	,m_nErrorLen(0)
	,m_pLogoInfo(NULL)
	,m_pLogBuffer(0)
	,m_nLogEnable(1)
	,m_nMaxDecoderTime(0)
	,m_nMaxRenderTime(0)
	,m_nSumDecoderTime(0)
	,m_nSumRenderTime(0)
	,m_nSourceDropSample(0)
	,m_nDecoderDropFrame(0)
	,m_nRnnderDropFrame(0)
	,m_nRenderFrame(0)
	,m_nDecoderFrameNumber(0)
	,m_nStatisPlaybackStartTime(0)
	,m_nStatisPlaybackEndTime(0)
	,m_nStatisPlaybackDurationTime(0)
	,m_nStatisTotalRenderFrame(0)
	,m_nStatisTotalDecoderFrame(0)
	,m_nStatisTotalSourceDropSample(0)
	,m_nStatisTotalDecoderDropFrame(0)
	,m_nStatisTotalRenderDropFrame(0)
	,m_nFrameRateCount(0)
	,m_nFrameRateValue(0)
	,m_nDecoderTotalTime(0)
	,m_nDecoderNumber(1)
	,m_nVideoRenderStartTime(0)
	,m_nVideoRenderEndTime(0)
	,m_pCPULoadInfo(NULL)
	,m_nUserCPUload(0)
	,m_nTotalCPUload(0)
	,m_nCPUThreadStart(0)
	,m_nStatus(0)
	,m_pWorkPath(NULL)
	,m_pLicenceCheckHandle(NULL)
	,m_nSrcTimeNumCount(0)
	,m_nDecTimeNumCount(0)
	,m_nRendTimeNumCount(0)
	,m_nJitterTimeNumCount(0)
	,m_nCachCount(0)
	,m_nMaxCachSize(3000)
	,m_pPerfDataHeader(NULL)
	,m_nLastUpdatePerfTime(0)
{
	m_nMaxCount = LOG_MAX_CATCH_TIME*30;

	memset(&m_sPerformData, 0, sizeof(VOMP_PERFORMANCEDATA));

	m_sPerformData.nLastTime = 3;
	m_sPerformData.nSourceTimeNum = 60;
	m_sPerformData.nCodecTimeNum = 60;
	m_sPerformData.nRenderTimeNum = 30;
	m_sPerformData.nJitterNum = 60;
	m_sPerformData.nCPULoad = -1;

	m_pCPULoadInfo = new CCPULoadInfo();

	memset (m_szSrcTimeNumSet, 0, sizeof (sSourceTimeNumber) * TIME_NUMBER_MAX);
	memset (m_szDecTimeNumSet, 0, sizeof (sDecoderTimeNumber) * TIME_NUMBER_MAX);
	memset (m_szRndTimeNumSet, 0, sizeof (sRenderTimeNumber) * TIME_NUMBER_MAX);
	memset (m_szJitterTimeNumSet, 0, sizeof (sJitterTimeNumber) *TIME_NUMBER_MAX);
//#ifdef _LINUX_ANDROID	
//#ifdef __VO_NDK__
//	char szProp[256];
//	memset (szProp, 0, 256);
//	__system_property_get (VOLOG_LEVEL_PROP, szProp);
//	if (strlen (szProp) > 0)
//		m_nLogEnable = atoi (szProp);
//#endif // __VO_NDK__
//#endif // _LINUX_ANDROID
}

voCPlayerLogoInfo::~voCPlayerLogoInfo()
{
	if(m_nLogEnable == 20111111 && m_pLogBuffer)
	{
#ifdef _LINUX_ANDROID	
		FILE *dump = NULL;
		if(m_nSourceType == 0)
			dump = fopen("/sdcard/PlayingAudioData.dat", "wb");
		else
			dump = fopen("/sdcard/PlayingVideoData.dat", "wb");
		if(dump)
		{
			int n = fwrite(m_pLogBuffer, 1, m_nReadCount*sizeof(VOMP_LOGOINFO), dump);
			fclose(dump);
		}
#elif defined _WIN32
		FILE *dump = NULL;

		char		strCharName[1024];
		VO_TCHAR    strFileName[1024];
		memset(strCharName , 0 , 1024);
		memset(strFileName , 0 , sizeof(VO_TCHAR) * 1024);

		voOS_GetAppFolder(strFileName , 1024);

		if(m_nSourceType == 0)
			vostrcat(strFileName , _T("PlayingAudioData.txt"));
		else
			vostrcat(strFileName , _T("PlayingVideoData.txt"));

	
		WideCharToMultiByte (CP_ACP, 0, strFileName, -1, strCharName, 1024 , NULL, NULL);

		dump = fopen(strCharName, "w");
		
		if(dump)
		{
			VOMP_LOGOINFO *pLogInfo = NULL;
			fprintf(dump , "nTimeStamp	nPlayingTime	nSystemTime	nInSystemTime	nGetSrcSize	nDecoderRet	nGetSrcTime	nDecSrcTime	nRenderSrcTime	CpuLoading	nRndBufCount	nBufFlag	nReserved16	nReserved\r");
			for(int i = 0 ; i < m_nReadCount ; i++)
			{
				pLogInfo = (VOMP_LOGOINFO *)(m_pLogBuffer + i * sizeof(VOMP_LOGOINFO));
				fprintf(dump , "%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d\r" , pLogInfo->nTimeStamp , pLogInfo->nPlayingTime, pLogInfo->nSystemTime,
					pLogInfo->nInSystemTime , pLogInfo->nGetSrcSize , pLogInfo->nDecoderRet , pLogInfo->nGetSrcTime , pLogInfo->nDecSrcTime , pLogInfo->nRenderSrcTime,
					pLogInfo->CpuLoading , pLogInfo->nRndBufCount , pLogInfo->nBufFlag , pLogInfo->nReserved16, pLogInfo->nReserved);
			}

			//int n = fwrite(m_pLogBuffer, 1, m_nReadCount*sizeof(VOMP_LOGOINFO), dump);
			fclose(dump);
		}
#endif
	}
	
	if(m_pLogBuffer)
	{
		delete []m_pLogBuffer;
		m_pLogBuffer = NULL;
	}

	if(m_pIntDecError)
	{
		delete []m_pIntDecError;
		m_pIntDecError = NULL;
	}

	if (m_pLicenceCheckHandle)
		voCheckLibUninit(m_pLicenceCheckHandle);

	if (m_pCPULoadInfo)
		delete m_pCPULoadInfo;

	voCAutoLock lock (&m_mtPerfUpdata);
	sPerfDataNode* pTmp = m_pPerfDataHeader;
	while (m_nCachCount > 0)
	{
		sPerfDataNode* pNext = pTmp->pNext;
		delete pTmp->pPerfData;
		delete pTmp;
		pTmp = pNext;
		m_nCachCount --;
	}
}

int voCPlayerLogoInfo::Start()
{
	m_nStatus = 1;

	return 0;
}

int voCPlayerLogoInfo::Stop()
{
	m_nStatus = 0;

	return 0;
}

int voCPlayerLogoInfo::GetSourceStart (void* pSourceSample)
{
	if(m_nLogEnable == 0 || pSourceSample == NULL)
		return 0;

	voCAutoLock lock (&m_mtStatus);

	VO_SOURCE_SAMPLE* pSample = (VO_SOURCE_SAMPLE *)pSourceSample;

	if(m_nStartSystemTime == 0)
		m_nStartSystemTime = voOS_GetSysTime ();
	
	if(m_nGetSuccess)
	{
		m_nGetStartTime = voOS_GetSysTime () - m_nStartSystemTime;
		m_nGetSuccess = 0;
	}
	
	return 0;
}

int voCPlayerLogoInfo::GetSourceEnd (void* pSourceSample, int nRC)
{
	if(m_nLogEnable == 0 || pSourceSample == NULL)
		return 0;

	voCAutoLock lock (&m_mtStatus);

	if(nRC == VO_ERR_SOURCE_DROP_FRAME || nRC == VOMP_ERR_None)
	{
		VO_SOURCE_SAMPLE* pSample = (VO_SOURCE_SAMPLE *)pSourceSample;

		m_nGetEndTime = voOS_GetSysTime () - m_nStartSystemTime;
		m_nOutTimeStamp = (int)(pSample->Time);
		m_nGetSrcSize = pSample->Size;
		m_nGetSrcTime = m_nGetEndTime - m_nGetStartTime;

		if(m_nGetSrcTime > 0x7fff) 
			m_nGetSrcTime = 0x7fff;

		if(m_nReadCount == 0 && m_pLogBuffer == NULL)
		{
			m_pLogBuffer = new char[m_nMaxCount*sizeof(VOMP_LOGOINFO) + 1024];

			if(m_pLogBuffer)
			{
				memset(m_pLogBuffer, 0, m_nMaxCount*sizeof(VOMP_LOGOINFO) + 1024);
			}
		}

		if(m_pLogBuffer == NULL)
			return 0;

		m_nReadIndex = m_nReadCount%m_nMaxCount;

		m_pLogoInfo = (VOMP_LOGOINFO *)(m_pLogBuffer + m_nReadIndex*sizeof(VOMP_LOGOINFO));
		memset(m_pLogoInfo, 0, sizeof(VOMP_LOGOINFO));

		m_pLogoInfo->nGetSrcSize = m_nGetSrcSize;
		m_pLogoInfo->nGetSrcTime = m_nGetSrcTime;
		m_pLogoInfo->nInSystemTime = m_nGetEndTime;
		m_pLogoInfo->nTimeStamp = m_nOutTimeStamp;

		if(nRC == VO_ERR_SOURCE_DROP_FRAME)
		{
			m_pLogoInfo->nBufFlag |= SOURCE_DROP_FLAG;

			m_pLogoInfo->nSystemTime = voOS_GetSysTime () - m_nStartSystemTime;
		}

		m_pLogoInfo->CpuLoading = m_nAPLoading;
		m_pLogoInfo->nReserved16 = m_nSysLoading + m_nCPULoading;

		VOLOGR("GetVideo using Time %d, TimeStamp %d, m_nReadCount %d, CPU_Loading %d", (int)m_pLogoInfo->nGetSrcTime, m_nOutTimeStamp, m_nReadCount, m_nCPULoading);

		m_nGetSuccess = 1;
		m_nReadCount++;
		return 0;
	}
	else
	{
		return 0;
	}

	return 0;
}

int voCPlayerLogoInfo::DecSourceStart (void* pSourceDecInBuf, int nDrop)
{
	if(m_nLogEnable == 0 || m_pLogBuffer == NULL || pSourceDecInBuf == NULL || m_pLogoInfo == NULL)
		return 0;

	voCAutoLock lock (&m_mtStatus);

	if(nDrop)
	{
		m_pLogoInfo->nBufFlag |= DECODER_DROP_FLAG;
		m_pLogoInfo->nSystemTime = voOS_GetSysTime () - m_nStartSystemTime;
		return 0;
	}

	if(m_nDecSuccess)
	{
		m_nDecStartTime = voOS_GetSysTime () - m_nStartSystemTime;
		m_nDecSuccess = 0;
	}

	return 0;
}

int voCPlayerLogoInfo::DecSourceEnd (void* pSourceDecOutBuf, int nRC)
{
	if(m_nLogEnable == 0 || m_pLogBuffer == NULL || pSourceDecOutBuf == NULL || m_pLogoInfo == NULL)
		return 0;

	voCAutoLock lock (&m_mtStatus);

	if( nRC == VO_ERR_INPUT_BUFFER_SMALL)
	{
		return 0;
	}

	//VOLOGI("m_pLogoInfo %x, pSourceDecOutBuf %x", m_pLogoInfo, pSourceDecOutBuf);

	if(nRC != VO_ERR_NONE)
	{
		m_pLogoInfo->nDecoderRet = nRC;
		m_pLogoInfo->nSystemTime = voOS_GetSysTime () - m_nStartSystemTime;
		return 0;
	}

	if(m_nSourceType && ((VO_VIDEO_BUFFER*)pSourceDecOutBuf)->Buffer[0] == NULL)
		return 0;

	m_pLogoInfo->nDecoderRet = nRC;

	m_nDecEndTime = voOS_GetSysTime () - m_nStartSystemTime;

	int nDecTime = m_nDecEndTime - m_nDecStartTime;
	if(nDecTime > 0x7fff)  nDecTime = 0x7fff;
	
	m_pLogoInfo->nDecSrcTime = nDecTime;
	m_nDecSuccess = 1;

	VOLOGR("Dec Video using Time %d", (int)nDecTime);


	return 0;
}

int voCPlayerLogoInfo::RenderSourceStart (void * pSourceRenderInBuf, int nBuffer)
{
	if(m_nLogEnable == 0 || m_pLogBuffer == NULL || pSourceRenderInBuf == NULL)
		return 0;

	voCAutoLock lock (&m_mtStatus);

	if(nBuffer == 0)
	{
		VOMP_LOGOINFO* pLogoInfo = FindItemFromTimeStamp((int)((VO_VIDEO_BUFFER *)pSourceRenderInBuf)->Time);
		
		VOLOGR("Render Video drop %d", (int)nBuffer);
		
		if(pLogoInfo == NULL)
			return 0;

		pLogoInfo->nBufFlag |= RENDER_DROP_FLAG;
		pLogoInfo->nSystemTime = voOS_GetSysTime () - m_nStartSystemTime;

		return 0;
	}

	m_nRdBufferNum = nBuffer;
	m_nRdStartTime = voOS_GetSysTime () - m_nStartSystemTime;

	return 0;
}

int voCPlayerLogoInfo::RenderSourceEnd (void * pSourceRenderOutBuf, int nPlayTime, int nRC)
{
	if(m_nLogEnable == 0 || m_pLogBuffer == NULL || pSourceRenderOutBuf == NULL)
		return 0;

	voCAutoLock lock (&m_mtStatus);

	VOMP_LOGOINFO* pLogoInfo = FindItemFromTimeStamp((int)((VO_VIDEO_BUFFER *)pSourceRenderOutBuf)->Time);

	VOLOGR("FindItemFromTimeStamp %x, TimeStamp %d", (int)pLogoInfo, (int)((VO_VIDEO_BUFFER *)pSourceRenderOutBuf)->Time);

	if(pLogoInfo == NULL)
		return 0;

	m_nRdEndTime = voOS_GetSysTime () - m_nStartSystemTime;

	int nRndTime = 33;
	if(m_nLastRenderTime)
	{
		nRndTime = m_nRdEndTime - m_nLastRenderTime;
	}

	m_nLastRenderTime = m_nRdEndTime;	
	if(nRndTime > 0x7fff) nRndTime = 0x7fff;

	pLogoInfo->nRenderSrcTime = nRndTime;
	pLogoInfo->nSystemTime = m_nRdEndTime;
	pLogoInfo->nPlayingTime = nPlayTime;
	pLogoInfo->nRndBufCount = m_nRdBufferNum;

	UpdateRendTimeNumber( nRndTime );

	m_nSumRenderTime = m_nSumRenderTime + nRndTime;
	if (m_nMaxRenderTime < nRndTime)
		m_nMaxRenderTime = nRndTime;

	//m_nStatisTotalRenderFrame ++;
	m_nRenderFrame ++;

	VOLOGR ("GetSource Time: %d, Decoder Source Time: %d, Render Source Time: %d, Source To Render Time: %d, Video buffer Count: %d", pLogoInfo->nGetSrcTime, pLogoInfo->nDecSrcTime, pLogoInfo->nRenderSrcTime, pLogoInfo->nSystemTime - pLogoInfo->nInSystemTime, pLogoInfo->nRndBufCount);
	VOLOGR ("System Time: %d, Playting Time: %d, TimeStamp: %d", pLogoInfo->nSystemTime, pLogoInfo->nPlayingTime, pLogoInfo->nTimeStamp);

	return 0;
}

int voCPlayerLogoInfo::Flush ()
{
	if(m_pLogBuffer == NULL)
		return 0;

	voCAutoLock lock (&m_mtStatus);

	if(m_pLogBuffer)
	{
		memset(m_pLogBuffer, 0, (m_nMaxCount)*sizeof(VOMP_LOGOINFO) + 1024);
	}

	m_nReadCount = 0;
	m_nReadIndex = 0;
	m_nGetStartTime = 0;
	m_nGetEndTime = 0;
	m_nOutTimeStamp = 0;
	m_nGetSuccess = 1;
	m_nDecStartTime = 0;
	m_nDecEndTime = 0;
	m_nRdStartTime = 0;
	m_nRdEndTime = 0;
	m_nStartSystemTime = 0;
	m_nLastRenderTime = 0;

	return 0;
}

int voCPlayerLogoInfo::GetParam (int nID, void * pValue)
{
	switch(nID)
	{
	case VOMP_PID_VIDEO_PERFORMANCE_OPTION:
		{
			int nRC = LicenceCheck();
			if ( nRC ) return nRC;

			if(pValue == NULL || m_nLogEnable == 0)
				nRC = VOMP_ERR_Implement;
			else
			{
				VOMP_PERFORMANCEDATA * pPerfData = (VOMP_PERFORMANCEDATA *)pValue;
				nRC = GetPerformance(pPerfData);

				VOLOGI("Get VOMP_PID_VIDEO_PERFORMANCE_OPTION CDN %d, RDN %d, DN %d, RN %d, CTN %d, RTN %d, JN %d, CL %d, TCL %d, F %d, WDT %d, WRT %d, \
					   ADT %d, ART %d, TPD %d, TCDN %d, TRDN %d, TDN %d, TRN %d", 
					   pPerfData->nCodecDropNum, pPerfData->nRenderDropNum, pPerfData->nDecodedNum, pPerfData->nRenderNum, pPerfData->nCodecTimeNum, pPerfData->nRenderTimeNum, 
					   pPerfData->nJitterNum, pPerfData->nCPULoad, pPerfData->nTotalCPULoad, pPerfData->nFrequency, pPerfData->nWorstDecodeTime, pPerfData->nWorstRenderTime, 
					   pPerfData->nAverageDecodeTime, pPerfData->nAverageRenderTime, pPerfData->nTotalPlaybackDuration, pPerfData->nTotalCodecDropNum, 
					   pPerfData->nTotalRenderDropNum, pPerfData->nTotalDecodedNum, pPerfData->nTotalRenderNum);
			}

			return nRC;
		}
		break;
	}
	
	return 0;
}

int voCPlayerLogoInfo::SetParam (int nID, void * pValue)
{
	voCAutoLock lock (&m_mtStatus);

	switch(nID)
	{
	case VOMP_PID_VIDEO_PERFORMANCE_ONOFF:
		{
			int nRC = LicenceCheck();
			if ( nRC ) return nRC;

			if(pValue)
				m_nLogEnable = *((int *)pValue);
		}
		break;

	case VOMP_PID_VIDEO_PERFORMANCE_CACHE:
		if(pValue)
		{
			m_nMaxCount = *((int *)pValue)*30;
			if(m_pLogBuffer)
			{
				delete m_pLogBuffer;
				m_pLogBuffer = NULL;
			}

			m_pLogBuffer = new char[m_nMaxCount*sizeof(VOMP_LOGOINFO) + 1024];

			if(m_pLogBuffer)
			{
				memset(m_pLogBuffer, 0, m_nMaxCount*sizeof(VOMP_LOGOINFO) + 1024);
			}
		}
		break;
	case VOMP_PID_VIDEO_PERFORMANCE_OPTION:
		{
			int nRC = LicenceCheck();
			if ( nRC ) return nRC;

			if(pValue)
			{
//				memcpy(&m_sPerformData, pValue, sizeof(VOMP_PERFORMANCEDATA));
				VOMP_PERFORMANCEDATA* pPerfData = (VOMP_PERFORMANCEDATA*)pValue;
				m_sPerformData.nLastTime = pPerfData->nLastTime;

				VOLOGI("Set VOMP_PID_VIDEO_PERFORMANCE_OPTION nLastTime %d, nSourceTimeNum %d, nCodecTimeNum %d, nRenderTimeNum %d, nJitterNum %d, nCPULoad %d", 
					m_sPerformData.nLastTime, m_sPerformData.nSourceTimeNum, m_sPerformData.nCodecTimeNum, m_sPerformData.nRenderTimeNum, m_sPerformData.nJitterNum, m_sPerformData.nCPULoad);
			}
		}
		break;
	case LOGO_CPU_USER_LOAD_VALUE:
		if(pValue)
	   		m_nCPULoading = *((int *)pValue);
		break;
	case LOGO_CPU_SYS_LOAD_VALUE:
		if(pValue)
	   		m_nSysLoading = *((int *)pValue);
		break;
	case LOGO_CPU_APP_LOAD_VALUE:
		if(pValue)
			m_nAPLoading = *((int *)pValue);
		break;
	}

	return 0;
}

VOMP_LOGOINFO* voCPlayerLogoInfo::FindItemFromTimeStamp(int nTimeStamp)
{
	
	if(m_nReadCount == 0)
		return 0;

	int mReadCount = (m_nReadCount - 1)%m_nMaxCount;
	int nSize = sizeof(VOMP_LOGOINFO);
	int nFound = 0;

	char* pLogInfo = m_pLogBuffer + nSize*mReadCount;
	
	for( ; mReadCount >= 0; mReadCount--)
	{
		if(*((int *)pLogInfo) == nTimeStamp)
		{
			nFound = 1;
			break;
		}

		if(*((int *)pLogInfo) < nTimeStamp - 1000)
				break;
		
		pLogInfo -= nSize;		
	}

	if(nFound)
		return (VOMP_LOGOINFO*)pLogInfo;

	pLogInfo += nSize;

	if(m_nReadCount > m_nMaxCount && *((int *)pLogInfo) > nTimeStamp - 500)
	{
		mReadCount = m_nMaxCount - 1;
		pLogInfo = m_pLogBuffer + nSize*mReadCount;

		for( ; mReadCount >= 0; mReadCount--)
		{
			if(*((int *)pLogInfo) == nTimeStamp)
			{
				nFound = 1;
				break;
			}

			if(*((int *)pLogInfo) < nTimeStamp - 1000)
				break;

			pLogInfo -= nSize;		
		}

		if(nFound)
			return (VOMP_LOGOINFO*)pLogInfo;
	}

	return 0;
}

int voCPlayerLogoInfo::GetCurrPerformance(VOMP_PERFORMANCEDATA *pPerfData)
{
	int	inDuration = m_sPerformData.nLastTime;
	int inSourceTime = m_sPerformData.nSourceTimeNum;
	int inDecTime = m_sPerformData.nCodecTimeNum;
	int inRenderTime = m_sPerformData.nRenderTimeNum ;
	int inJitterTime = m_sPerformData.nJitterNum;
	int nCPULoad = m_sPerformData.nCPULoad;

	voCAutoLock lock (&m_mtStatus);

	if(pPerfData == NULL || m_pLogBuffer == NULL)
		return VOMP_ERR_Pointer;

	if(m_nReadCount == 0)
		return VOMP_ERR_Implement;
	
	if(pPerfData->nLastTime > 0 && pPerfData->nLastTime < 100)
		inDuration = pPerfData->nLastTime*1000;

	pPerfData->nLastTime = inDuration/1000;

	if(pPerfData->nSourceTimeNum > 0 && pPerfData->nSourceTimeNum < 500)
		inSourceTime = pPerfData->nSourceTimeNum;

	if(pPerfData->nCodecTimeNum > 0 && pPerfData->nCodecTimeNum < 500)
		inDecTime = pPerfData->nCodecTimeNum;

	if(pPerfData->nRenderTimeNum > 0 && pPerfData->nRenderTimeNum < 500)
		inRenderTime = pPerfData->nRenderTimeNum;

	if(pPerfData->nJitterNum > 0 && pPerfData->nJitterNum < 500)
		inJitterTime = pPerfData->nJitterNum;

	VOLOGR("pPerfData->nLastTime %d, inSourceTime %d, inDecTime %d, inRenderTime %d, inJitterTime %d", pPerfData->nLastTime, (int)inSourceTime, (int)inDecTime, inRenderTime, inJitterTime);

	int nSourceDropNum = 0;
	int nCodecDropNum = 0;
	int nRenderDropNum = 0;
	int nDecodedNum = 0;
	int nRenderNum = 0;
	int nSourceTimeNum = 0; 
	int nCodecTimeNum = 0;
	int nRenderTimeNum = 0;
	int nJitterNum = 0; 
	int nCodecErrorsNum = 0;
	int nWorstDecodeTime = 0;
	int nWorstRenderTime = 0;
	int nTotalDecoderTime = 0;
	int nTotalRenderTime = 0;
	int nLastSystem = 0;
	int nCpuTotal = 0;
	int nCpuAll = 0;
	int nSourceNum = 0;
	int nBeginTime = 0;

	int nErrorLen = (pPerfData->nLastTime > 0) ? pPerfData->nLastTime * 50 : 250;
	if(m_nErrorLen < nErrorLen)
	{
		if(m_pIntDecError)
		{
			delete []m_pIntDecError;
			m_pIntDecError = NULL;
		}

		m_nErrorLen = nErrorLen;
		m_pIntDecError = new int[m_nErrorLen];
	}

	memset(m_pIntDecError, 0, m_nErrorLen*sizeof(int));

	int mReadCount = (m_nReadCount - 1)%m_nMaxCount;
	int nSize = sizeof(VOMP_LOGOINFO);
	int nStartSystemTime = 0; 
	VOMP_LOGOINFO *pLogInfo = NULL;

	for( ; mReadCount >= 0; mReadCount--)
	{
		pLogInfo = (VOMP_LOGOINFO *)(m_pLogBuffer + nSize*mReadCount);

		if(pLogInfo->nSystemTime == 0 && mReadCount > 0)
		{
			VOLOGR("inDuration %d, pLogInfo->nSystemTime %d, mReadCount %d", (int)inDuration, (int)pLogInfo->nSystemTime, mReadCount);		
			continue;
		}

		break;
	}

	nBeginTime = pLogInfo->nSystemTime;
	nStartSystemTime = pLogInfo->nSystemTime - inDuration - 10;
	if(nStartSystemTime < 0)
		nStartSystemTime = 0;
	VOLOGR("inDuration %d, nStartSystemTime %d, SystemTime %d", (int)inDuration, (int)nStartSystemTime, voOS_GetSysTime ());

	for( ; mReadCount >= 0; mReadCount--)
	{
		pLogInfo = (VOMP_LOGOINFO *)(m_pLogBuffer + nSize*mReadCount);
		
		if(pLogInfo->nSystemTime == 0 && mReadCount > 0)
		{
			VOLOGR("inDuration %d, pLogInfo->nSystemTime %d, mReadCount %d", (int)inDuration, (int)pLogInfo->nSystemTime, mReadCount);		
			continue;
		}

		if(pLogInfo->nSystemTime <= (VO_U32)nStartSystemTime)
		{
			if(pLogInfo->nSystemTime > 0 || (pLogInfo->nSystemTime == 0 && nStartSystemTime == 0))
			{
				VOLOGR("inDuration %x, nStartSystemTime %d, pLogInfo->nSystemTime %d, SystemTime %d", (int)inDuration, (int)nStartSystemTime, pLogInfo->nSystemTime, voOS_GetSysTime ());
				break;
			}
		}
		
		if(pLogInfo->CpuLoading > 0)
		{
			nSourceNum++;
			nCpuTotal += pLogInfo->CpuLoading;
			nCpuAll += pLogInfo->nReserved16;
		}

		nLastSystem = pLogInfo->nSystemTime;
		
		if(pLogInfo->nBufFlag & SOURCE_DROP_FLAG)
		{
			nSourceDropNum++;
			VOLOGR("Source Drop...");
			continue;
		}

		if(pLogInfo->nGetSrcTime > inSourceTime)
			nSourceTimeNum++;

		if(pLogInfo->nBufFlag & DECODER_DROP_FLAG)
		{
			nCodecDropNum++;
			VOLOGR("Decoder Drop...");
			continue;
		}

		if(pLogInfo->nDecoderRet)
		{
			if(nCodecErrorsNum >= m_nErrorLen)
			{
				int* pTempDecErr = new int[m_nErrorLen*2];
				memset(pTempDecErr, 0, sizeof(int)*m_nErrorLen*2);
				memcpy(pTempDecErr, m_pIntDecError, nCodecErrorsNum*sizeof(int));
				m_nErrorLen = m_nErrorLen*2;

				if (m_pIntDecError)
					delete[]m_pIntDecError;
				m_pIntDecError = pTempDecErr;
			}
			if (m_pIntDecError)
			{
				m_pIntDecError[nCodecErrorsNum] = pLogInfo->nDecoderRet;
				nCodecErrorsNum++;
			}
			continue;
		}

		nDecodedNum++;
		nTotalDecoderTime += pLogInfo->nDecSrcTime;

		if(pLogInfo->nDecSrcTime > inDecTime)
			nCodecTimeNum++;

		if(pLogInfo->nDecSrcTime > nWorstDecodeTime)
			nWorstDecodeTime = pLogInfo->nDecSrcTime;

		if(pLogInfo->nBufFlag & RENDER_DROP_FLAG)
		{
			VOLOGR("Render drop...");
			nRenderDropNum++;
			continue;
		}

		nRenderNum++;
		nTotalRenderTime += pLogInfo->nRenderSrcTime;

		if(pLogInfo->nRenderSrcTime > inRenderTime)
			nRenderTimeNum++;

		if(pLogInfo->nRenderSrcTime > nWorstRenderTime)
			nWorstRenderTime = pLogInfo->nRenderSrcTime;

		if((pLogInfo->nTimeStamp - pLogInfo->nPlayingTime) > inJitterTime || (pLogInfo->nTimeStamp - pLogInfo->nPlayingTime) < -inJitterTime)
			nJitterNum++;
	}
	

	if(nLastSystem > nStartSystemTime && mReadCount <= 0 && m_nReadCount > m_nMaxCount)
	{
		mReadCount = m_nMaxCount - 1;
		for( ; mReadCount >= 0; mReadCount--)
		{
			pLogInfo = (VOMP_LOGOINFO *)(m_pLogBuffer + nSize*mReadCount);

			if(pLogInfo->nSystemTime == 0 && mReadCount > 0)
				continue;

			if(pLogInfo->nSystemTime <= (VO_U32)nStartSystemTime)
			{
				if(pLogInfo->nSystemTime > 0 || (pLogInfo->nSystemTime == 0 && nStartSystemTime == 0))
					break;
			}

			if(pLogInfo->CpuLoading > 0)
			{
				nSourceNum++;
				nCpuTotal += pLogInfo->CpuLoading;
				nCpuAll += pLogInfo->nReserved16;
			}

			if(pLogInfo->nBufFlag & SOURCE_DROP_FLAG)
			{
				nSourceDropNum++;
				continue;
			}

			if(pLogInfo->nGetSrcTime > inSourceTime)
				nSourceTimeNum++;

			if(pLogInfo->nBufFlag & DECODER_DROP_FLAG)
			{
				nCodecDropNum++;
				continue;
			}

			if(pLogInfo->nDecoderRet)
			{
				if(nCodecErrorsNum >= m_nErrorLen)
				{
					int* pTempDecErr = new int[m_nErrorLen*2];
					memset(pTempDecErr, 0, sizeof(int)*m_nErrorLen*2);
					memcpy(pTempDecErr, m_pIntDecError, nCodecErrorsNum*sizeof(int));
					m_nErrorLen = m_nErrorLen*2;

					if (m_pIntDecError)
						delete[]m_pIntDecError;
					m_pIntDecError = pTempDecErr;
				}
				if (m_pIntDecError)
				{
					m_pIntDecError[nCodecErrorsNum] = pLogInfo->nDecoderRet;
					nCodecErrorsNum++;
				}
				continue;
			}

			nDecodedNum++;
			nTotalDecoderTime += pLogInfo->nDecSrcTime;

			if(pLogInfo->nDecSrcTime > inDecTime)
				nCodecTimeNum++;

			if(pLogInfo->nDecSrcTime > nWorstDecodeTime)
				nWorstDecodeTime = pLogInfo->nDecSrcTime;

			if(pLogInfo->nBufFlag & RENDER_DROP_FLAG)
			{
				nRenderDropNum++;
				continue;
			}

			nRenderNum++;
			nTotalRenderTime += pLogInfo->nRenderSrcTime;

			if(pLogInfo->nRenderSrcTime > inRenderTime)
				nRenderTimeNum++;

			if(pLogInfo->nRenderSrcTime > nWorstRenderTime)
				nWorstRenderTime = pLogInfo->nRenderSrcTime;

			if((pLogInfo->nTimeStamp - pLogInfo->nPlayingTime) > inJitterTime || (pLogInfo->nTimeStamp - pLogInfo->nPlayingTime) < -inJitterTime)
				nJitterNum++;
		}
	}

	if(mReadCount <= 0 && (int)pLogInfo->nSystemTime > nStartSystemTime + 10)
	{
		pPerfData->nLastTime = (nBeginTime - pLogInfo->nSystemTime + 500)/1000;
	}

	pPerfData->nSourceDropNum = nSourceDropNum;
	pPerfData->nCodecDropNum = nCodecDropNum;
	pPerfData->nRenderDropNum = nRenderDropNum;
	pPerfData->nDecodedNum = nDecodedNum;
	pPerfData->nRenderNum = nRenderNum;
	pPerfData->nSourceTimeNum = nSourceTimeNum; 
	pPerfData->nCodecTimeNum = nCodecTimeNum;
	pPerfData->nRenderTimeNum = nRenderTimeNum;
	pPerfData->nJitterNum = nJitterNum; 
	pPerfData->nCodecErrorsNum = nCodecErrorsNum;
	pPerfData->nWorstDecodeTime = nWorstDecodeTime;
	pPerfData->nWorstRenderTime = nWorstRenderTime;
	pPerfData->nCodecErrors = m_pIntDecError;

	if(m_nDecoderFrameNumber)
		pPerfData->nAverageDecodeTime = m_nSumDecoderTime /m_nDecoderFrameNumber;
	if(nRenderNum)
		pPerfData->nAverageRenderTime = nTotalRenderTime/nRenderNum;

	VOLOGR ("nSourceDropNum: %d, nCodecDropNum: %d, nRenderDropNum: %d, nDecodedNum, %d, nRenderNum, %d", nSourceDropNum, nCodecDropNum, nRenderDropNum, nDecodedNum, nRenderNum);
	VOLOGR ("pPerfData->nMaxFrequency: %d, pPerfData->nFrequency: %d", pPerfData->nMaxFrequency, pPerfData->nFrequency);

	if(pPerfData->nCPULoad != 0)
		nCPULoad = pPerfData->nCPULoad;
	
	if(nCPULoad != -1 && nSourceNum > 0)
	{
		pPerfData->nCPULoad = nCpuTotal/nSourceNum;
		pPerfData->nTotalCPULoad = nCpuAll/nSourceNum;
	}

#ifdef _WIN32
	VO_U32 nSysCPU = 0;
	VO_U32 nUsrCPU = 0;
	VO_BOOL bDocCUPusage = VO_FALSE;
	int nTimeUsed1 = voOS_GetSysTime();
	voOS_GetCpuUsage( &nSysCPU , &nUsrCPU , &bDocCUPusage , 10 , 10);
	int nTimeUsed2 = voOS_GetSysTime();
	pPerfData->nTotalCPULoad = nUsrCPU + nUsrCPU;
	pPerfData->nCPULoad = nUsrCPU;
	VOLOGI("@@@### voOS_GetCpuUsage() used time = %d", (nTimeUsed2-nTimeUsed1));
#endif

	pPerfData->nSourceDropNum = m_nSourceDropSample;
	pPerfData->nCodecDropNum = m_nDecoderDropFrame;
	pPerfData->nRenderDropNum = m_nRnnderDropFrame;
	pPerfData->nDecodedNum = m_nDecoderFrameNumber;
	pPerfData->nRenderNum = m_nRenderFrame;

	if (pPerfData->nTotalRenderDropNum < 0)
		pPerfData->nTotalRenderDropNum = 0;
	if (pPerfData->nRenderDropNum < 0)
		pPerfData->nRenderDropNum = 0;

	pPerfData->nWorstDecodeTime =  m_nMaxDecoderTime;
	pPerfData->nWorstRenderTime =  m_nMaxRenderTime;

	m_nStatisTotalSourceDropSample = m_nStatisTotalSourceDropSample + m_nSourceDropSample;
	m_nStatisTotalDecoderDropFrame = m_nStatisTotalDecoderDropFrame +  m_nDecoderDropFrame;
	m_nStatisTotalRenderFrame = m_nStatisTotalRenderFrame + m_nRenderFrame;
	m_nStatisTotalDecoderFrame = m_nStatisTotalDecoderFrame + m_nDecoderFrameNumber;
	m_nStatisTotalRenderDropFrame = m_nStatisTotalRenderDropFrame + m_nRnnderDropFrame;

	m_nSumDecoderTime = 0;
	m_nSumRenderTime = 0;
	m_nMaxRenderTime = 0;
	m_nMaxDecoderTime = 0;
	m_nSourceDropSample = 0;
	m_nDecoderDropFrame = 0;
	m_nRnnderDropFrame = 0;
	m_nDecoderFrameNumber = 0;
	m_nRenderFrame = 0;
	m_nDecoderTotalTime = 0;

	return 0;
}

int voCPlayerLogoInfo::ReadFile(const char*  pathname, char*  buffer, size_t  buffsize)
{ 
	VO_FILE_OPERATOR * pFileOP;
	
	if (g_fileOP.Open == NULL)
		cmnFileFillPointer ();
	pFileOP = &g_fileOP;

	VO_FILE_SOURCE filSource;
	memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
	filSource.pSource = (VO_PTR)pathname;
	filSource.nFlag = VO_FILE_TYPE_NAME;
	filSource.nMode = VO_FILE_READ_ONLY;

	VO_PTR hFile = pFileOP->Open (&filSource);
	if (hFile == NULL)
	{
		VOLOGI("hFile is NULL");
		return 0;
	}

	VO_U32		dwRead = 0;

	memset (buffer, 0, buffsize);
	dwRead = pFileOP->Read (hFile, (VO_U8 *)buffer, 1024);
	pFileOP->Close (hFile);

	return dwRead;
}

int voCPlayerLogoInfo::RestStasticPerfoData()
{
	voCAutoLock lock (&m_mtPerfUpdata);
	m_nSumDecoderTime = 0;
	m_nSumRenderTime = 0;

	m_nMaxRenderTime = 0;
	m_nMaxDecoderTime = 0;

	m_nSourceDropSample = 0;
	m_nDecoderDropFrame = 0;
	m_nRnnderDropFrame = 0;
	m_nDecoderFrameNumber = 0;
	m_nRenderFrame = 0;

	m_nStatisPlaybackDurationTime = 0;
	m_nStatisTotalRenderFrame = 0;
	m_nStatisTotalDecoderFrame = 0;
	m_nStatisTotalSourceDropSample = 0;
	m_nStatisTotalDecoderDropFrame = 0;
	m_nStatisTotalRenderDropFrame = 0;

	m_nCPUThreadStart = 0;

	sPerfDataNode* pTmp = m_pPerfDataHeader;
	while (m_nCachCount > 0)
	{
		sPerfDataNode* pNext = pTmp->pNext;
		delete pTmp->pPerfData;
		delete pTmp;
		pTmp = pNext;
		m_nCachCount --;
	}
	m_pPerfDataHeader = NULL;
	m_nCachCount = 0;

	m_nSrcTimeNumCount = 0;
	m_nDecTimeNumCount = 0;
	m_nRendTimeNumCount = 0;
	m_nJitterTimeNumCount = 0;
	memset (m_szSrcTimeNumSet, 0, sizeof (sSourceTimeNumber) * TIME_NUMBER_MAX);
	memset (m_szDecTimeNumSet, 0, sizeof (sDecoderTimeNumber) * TIME_NUMBER_MAX);
	memset (m_szRndTimeNumSet, 0, sizeof (sRenderTimeNumber) * TIME_NUMBER_MAX);
	memset (m_szJitterTimeNumSet, 0, sizeof (sJitterTimeNumber) *TIME_NUMBER_MAX);

	return 0;
}
int voCPlayerLogoInfo::LicenceCheck()
{
	if ( m_pLicenceCheckHandle )
		return VOMP_ERR_None;

	VO_U32 uRet = voCheckLibInit (&m_pLicenceCheckHandle, VO_INDEX_MFW_VOALS, VO_LCS_WORKPATH_FLAG, 0, m_pWorkPath); 
	if (uRet)
	{
		if (m_pLicenceCheckHandle)
			voCheckLibUninit(m_pLicenceCheckHandle);

		m_pLicenceCheckHandle = NULL;
		return VO_ERR_LICENSE_ERROR;
	}

	return VOMP_ERR_None;
}
void voCPlayerLogoInfo::SetWorkPath( VO_PTCHAR pWorkPath)
{
	m_pWorkPath = pWorkPath;
}

void voCPlayerLogoInfo::UpdataPerformanceData(bool bForceUpdate /* = false */)
{
#ifdef _WIN32
	/* Win32 platform now not use this new procedure */
	/* TODO code */
	return;
#endif
	voCAutoLock lock (&m_mtPerfUpdata);

	int nTimePassed = voOS_GetSysTime() - m_nLastUpdatePerfTime;
	if (nTimePassed > 100 || bForceUpdate)
	{
		sPerfDataNode* pTmpNode = new sPerfDataNode;
		VOMP_PERFORMANCEDATA* pTmpPerfData = new VOMP_PERFORMANCEDATA;
		if (pTmpNode == NULL || pTmpPerfData == NULL) return;

		memset( pTmpPerfData, 0, sizeof (VOMP_PERFORMANCEDATA) );
		pTmpPerfData->nLastTime = nTimePassed;
		GetCurrPerformance( pTmpPerfData );

		pTmpNode->nDuration = nTimePassed;
		pTmpNode->pPerfData = pTmpPerfData;

		pTmpNode->nSrcTimeNumCount = 0;
		pTmpNode->nDecTimeNumCount = 0;
		pTmpNode->nRendTimeNumCount = 0;
		pTmpNode->nJitterTimeNumCount = 0;
		memset (pTmpNode->szSrcTimeNumSet, 0, sizeof (sSourceTimeNumber) * TIME_NUMBER_MAX);
		memset (pTmpNode->szDecTimeNumSet, 0, sizeof (sDecoderTimeNumber) * TIME_NUMBER_MAX);
		memset (pTmpNode->szRndTimeNumSet, 0, sizeof (sRenderTimeNumber) * TIME_NUMBER_MAX);
		memset (pTmpNode->szJitterTimeNumSet, 0, sizeof (sJitterTimeNumber) *TIME_NUMBER_MAX);

		if (m_nSrcTimeNumCount > 0)
		{
			pTmpNode->nSrcTimeNumCount = m_nSrcTimeNumCount;
			memcpy (pTmpNode->szSrcTimeNumSet, m_szSrcTimeNumSet, sizeof (sSourceTimeNumber) * m_nSrcTimeNumCount);
			m_nSrcTimeNumCount = 0;
		}
		if (m_nDecTimeNumCount > 0)
		{
			pTmpNode->nDecTimeNumCount = m_nDecTimeNumCount;
			memcpy (pTmpNode->szDecTimeNumSet, m_szDecTimeNumSet, sizeof (sDecoderTimeNumber) * m_nDecTimeNumCount);
			m_nDecTimeNumCount = 0;
		}
		if (m_nRendTimeNumCount > 0)
		{
			pTmpNode->nRendTimeNumCount = m_nRendTimeNumCount;
			memcpy (pTmpNode->szRndTimeNumSet, m_szRndTimeNumSet, sizeof (sRenderTimeNumber) * m_nRendTimeNumCount);
			m_nRendTimeNumCount = 0;
		}
		if (m_nJitterTimeNumCount > 0)
		{
			pTmpNode->nJitterTimeNumCount = m_nJitterTimeNumCount;
			memcpy (pTmpNode->szJitterTimeNumSet, m_szJitterTimeNumSet, sizeof (sJitterTimeNumber) * m_nJitterTimeNumCount);
			m_nJitterTimeNumCount = 0;
		}

		if (m_pPerfDataHeader != NULL)
		{
			pTmpNode->pNext = m_pPerfDataHeader;
			m_pPerfDataHeader = pTmpNode;
		}
		else
		{
			m_pPerfDataHeader = pTmpNode;
			pTmpNode->pNext = NULL;
		}
		m_nCachCount++;

		int nOverFlow = m_nCachCount - m_nMaxCachSize;
		for (int i = 0; i < nOverFlow; ++i)
		{
			int nCurr = m_nCachCount ;
			sPerfDataNode* pTmp = m_pPerfDataHeader;
			while (--nCurr > 0) 
			{
				pTmp = pTmp->pNext;
			}

			delete pTmp->pPerfData;
			delete pTmp;
			pTmp = NULL;

			m_nCachCount --;
		}

		if (m_nCPUThreadStart == 0 && m_pCPULoadInfo )
		{
			m_pCPULoadInfo->InitCPUProc();
			m_nCPUThreadStart = 1;
			m_pCPULoadInfo->UpdateCPUProc();
		}
		m_nLastUpdatePerfTime = voOS_GetSysTime();
	}
}


int voCPlayerLogoInfo::GetPerformance(VOMP_PERFORMANCEDATA *pPerfData)
{
#ifdef _WIN32
	return GetCurrPerformance(pPerfData);
#endif

	UpdataPerformanceData(true);

	voCAutoLock lock (&m_mtPerfUpdata);

	VOLOGI("@@@### cfi_jni GetPerfData nLastTime = %d, m_nCachCount = %d", pPerfData->nLastTime, m_nCachCount);
	if (pPerfData && m_nCachCount > 0 && m_nStatus)
	{
		int nLastTime = pPerfData->nLastTime * 1000;

		int nTimeAccum = 0;
		int nSourceDropNum = 0;
		int nCodecDropNum = 0;
		int nRenderDropNum = 0;
		int nDecodedNum = 0;
		int nRenderNum = 0;
		int nMaxFrequency = 0;
		int nWorstDecodeTime = 0;
		int nWorstRenderTime = 0;
		int nAverageDecodeTime = 0;
		int nAverageRenderTime = 0;

		int nInSrcExceedTime = pPerfData->nSourceTimeNum;
		int nInDecExceedTime = pPerfData->nCodecTimeNum;
		int nInRndExceedTime = pPerfData->nRenderTimeNum;
		int nInJitterExceedTime = pPerfData->nJitterNum;

		VOLOGI("@@@### InSrcTime = %d, InDecTime = %d, InRndTime = %d, InJitterTime = %d", nInSrcExceedTime, nInDecExceedTime, nInRndExceedTime, nInJitterExceedTime);

		int nInSrcExceedNumber = 0;
		int nInDecExceedNumber = 0;
		int nInRndExceedNumber = 0;
		int nInJitterExceedNumber = 0;

		int i;
		sPerfDataNode* pPerfNode = m_pPerfDataHeader;
		for (i = 0; i < m_nCachCount ; ++i)
		{
			nSourceDropNum = nSourceDropNum + pPerfNode->pPerfData->nSourceDropNum;
			nCodecDropNum = nCodecDropNum + pPerfNode->pPerfData->nCodecDropNum;
			nRenderDropNum = nRenderDropNum + pPerfNode->pPerfData->nRenderDropNum;
			nDecodedNum = nDecodedNum + pPerfNode->pPerfData->nDecodedNum;
			nRenderNum = nRenderNum + pPerfNode->pPerfData->nRenderNum;

			if (nMaxFrequency < pPerfNode->pPerfData->nMaxFrequency) 
				nMaxFrequency = pPerfNode->pPerfData->nMaxFrequency;
			if (nWorstDecodeTime < pPerfNode->pPerfData->nWorstDecodeTime) 
				nWorstDecodeTime = pPerfNode->pPerfData->nWorstDecodeTime;
			if (nWorstRenderTime < pPerfNode->pPerfData->nWorstRenderTime) 
				nWorstRenderTime = pPerfNode->pPerfData->nWorstRenderTime;

			nAverageDecodeTime = nAverageDecodeTime + pPerfNode->pPerfData->nAverageDecodeTime;
			nAverageRenderTime = nAverageRenderTime + pPerfNode->pPerfData->nAverageRenderTime;

			for (int n = 0; n < pPerfNode->nSrcTimeNumCount; ++n)
				if ((pPerfNode->szSrcTimeNumSet[n]).TimeUsed > nInSrcExceedTime) nInSrcExceedNumber++;
			for (int n = 0; n < pPerfNode->nDecTimeNumCount; ++n)
				if ((pPerfNode->szDecTimeNumSet[n]).TimeUsed > nInDecExceedTime) nInDecExceedNumber++;
			for (int n = 0; n < pPerfNode->nRendTimeNumCount; ++n)
				if ((pPerfNode->szRndTimeNumSet[n]).TimeUsed > nInRndExceedTime) nInRndExceedNumber++;
			for (int n = 0; n < pPerfNode->nJitterTimeNumCount; ++n)
				if ((pPerfNode->szJitterTimeNumSet[n]).TimeUsed > nInJitterExceedTime) nInJitterExceedNumber++;

			if(i == m_nCachCount - 1)	// last node
				break;

			nTimeAccum = nTimeAccum + pPerfNode->nDuration;
			if((nTimeAccum >= nLastTime - 100) && (abs(nTimeAccum - nLastTime) < abs(nTimeAccum + pPerfNode->pNext->nDuration - nLastTime)))
				break;

			pPerfNode = pPerfNode->pNext;
		}

		VOLOGI("@@###  i = %d, LastDuration = %d", i, nTimeAccum);

		if (i > 0)
		{
			nAverageDecodeTime = nAverageDecodeTime / i;
			nAverageRenderTime = nAverageRenderTime  / i;
		}

		pPerfData->nSourceDropNum = nSourceDropNum;
		pPerfData->nCodecDropNum = nCodecDropNum;
		pPerfData->nRenderDropNum = nRenderDropNum;
		pPerfData->nDecodedNum = nDecodedNum;
		pPerfData->nRenderNum = nRenderNum;
		pPerfData->nMaxFrequency = nMaxFrequency;
		pPerfData->nSourceTimeNum = nInSrcExceedNumber;
		pPerfData->nCodecTimeNum = nInDecExceedNumber;
		pPerfData->nRenderTimeNum = nInRndExceedNumber;
		pPerfData->nJitterNum = nInJitterExceedNumber;
		pPerfData->nWorstDecodeTime = nWorstDecodeTime;
		pPerfData->nWorstRenderTime = nWorstRenderTime;
		pPerfData->nAverageDecodeTime = nAverageDecodeTime;
		pPerfData->nAverageRenderTime = nAverageRenderTime;

		pPerfData->nTotalPlaybackDuration = (int)m_nStatisPlaybackDurationTime;
		pPerfData->nTotalSourceDropNum = m_nStatisTotalSourceDropSample;
		pPerfData->nTotalCodecDropNum = m_nStatisTotalDecoderDropFrame;
		pPerfData->nTotalDecodedNum = m_nStatisTotalDecoderFrame;
		pPerfData->nTotalRenderNum = m_nStatisTotalRenderFrame;
		pPerfData->nTotalRenderDropNum = m_nStatisTotalRenderDropFrame;

		if (pPerfData->nTotalRenderDropNum < 0)
			pPerfData->nTotalRenderDropNum = 0;

		int nOutSys = 0;
		int nOutUsr = 0;
		int	nOutAP = 0;
		int rc = m_pCPULoadInfo->InitCPUProc();
		if(rc == 0)
		{
			rc = m_pCPULoadInfo->GetCPULoad(&nOutSys, &nOutUsr, &nOutAP);

			m_nUserCPUload = nOutAP;
			m_nTotalCPUload = nOutSys + nOutUsr;
			m_pCPULoadInfo->UpdateCPUProc();
		}
		pPerfData->nCPULoad = m_nUserCPUload;
		pPerfData->nTotalCPULoad = m_nTotalCPUload;

#ifdef _LINUX_ANDROID
		char cpuinfo[1024];
	    int  cpuinfo_len;
		int mMaxCpuSpeed;
		int mCurCpuSpeed;

		memset(cpuinfo, 0, 1024);
		cpuinfo_len = ReadFile("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", cpuinfo, 1024);
		
		if(cpuinfo_len > 0)
			pPerfData->nMaxFrequency = atoi(cpuinfo);
			//sscanf(cpuinfo, "%d", &pPerfData->nMaxFrequency );
		else
			pPerfData->nMaxFrequency = 0;

		memset(cpuinfo, 0, 1024);
		cpuinfo_len = ReadFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", cpuinfo, 1024);
		if(cpuinfo_len > 0)
			pPerfData->nFrequency = atoi(cpuinfo);
			//sscanf(cpuinfo, "%d", &pPerfData->nFrequency);
		else
			pPerfData->nMaxFrequency = 0;
#elif defined _IOS
	    VO_CPU_Info cInfo;
	    get_cpu_info(&cInfo);
	    pPerfData->nFrequency = cInfo.mMaxCpuSpeed;
	    pPerfData->nMaxFrequency = cInfo.mMaxCpuSpeed;
#endif
	}
	else
	{
		int nLastTime = pPerfData->nLastTime;
		memset( pPerfData, 0, sizeof (VOMP_PERFORMANCEDATA) );
		pPerfData->nLastTime = nLastTime;
		int nOutSys = 0;
		int nOutUsr = 0;
		int	nOutAP = 0;
		int rc = m_pCPULoadInfo->InitCPUProc();					
		if(rc == 0)
		{
			rc = m_pCPULoadInfo->GetCPULoad(&nOutSys, &nOutUsr, &nOutAP);

			m_nUserCPUload = nOutAP;
			m_nTotalCPUload = nOutSys + nOutUsr;
			m_pCPULoadInfo->UpdateCPUProc();
		}
		pPerfData->nCPULoad = m_nUserCPUload;
		pPerfData->nTotalCPULoad = m_nTotalCPUload;
		VOLOGI("@@@### Analytics finished playback !");
	}

	return 0;
}

void voCPlayerLogoInfo::UpdateSrcTimeNumber(int nTime)
{
	if (m_nSrcTimeNumCount >= TIME_NUMBER_MAX)
		m_nSrcTimeNumCount = 0;
	m_szSrcTimeNumSet[m_nSrcTimeNumCount].nDuration = 0;
	m_szSrcTimeNumSet[m_nSrcTimeNumCount].TimeUsed = nTime;
	m_nSrcTimeNumCount++;
}

void voCPlayerLogoInfo::UpdateDecTimeNumber(int nTime)
{
	if (m_nDecTimeNumCount >= TIME_NUMBER_MAX)
		m_nDecTimeNumCount = 0;
	m_szDecTimeNumSet[m_nDecTimeNumCount].nDuration = 0;
	m_szDecTimeNumSet[m_nDecTimeNumCount].TimeUsed = nTime;
	m_nDecTimeNumCount++;
}

void voCPlayerLogoInfo::UpdateRendTimeNumber(int nTime)
{
	if (m_nRendTimeNumCount >= TIME_NUMBER_MAX)
		m_nRendTimeNumCount = 0;
	m_szRndTimeNumSet[m_nRendTimeNumCount].nDuration = 0;
	m_szRndTimeNumSet[m_nRendTimeNumCount].TimeUsed = nTime;
	m_nRendTimeNumCount++;
}

void voCPlayerLogoInfo::UpdateJitterTimeNumber(int nTime)
{
	if (m_nJitterTimeNumCount >= TIME_NUMBER_MAX)
		m_nJitterTimeNumCount = 0;
	m_szJitterTimeNumSet[m_nJitterTimeNumCount].nDuration = 0;
	m_szJitterTimeNumSet[m_nJitterTimeNumCount].TimeUsed = nTime;
	m_nJitterTimeNumCount++;
}

CCPULoadInfo::CCPULoadInfo()
	:m_nCount(0)
{
	memset(&m_szCPUInfoOld, 0, sizeof(CpuInfo));
	memset(&m_szCPUInfoCur, 0, sizeof(CpuInfo));
	memset(&m_szProcAPInfoOld, 0, sizeof(ProcInfo));
	memset(&m_szProcAPInfoCur, 0, sizeof(ProcInfo));
    
#if defined (_IOS) || defined (_MAC_OS)
    prevCpuInfo = 0;
    prevCpuInfoNum = 0;
    
    curCpuInfo = 0;
    curCpuInfoNum = 0;
#endif
}


CCPULoadInfo::~CCPULoadInfo()
{
#if defined (_IOS) || defined (_MAC_OS)
    if(prevCpuInfo) {
        size_t prevCpuInfoSize = sizeof(integer_t) * prevCpuInfoNum;
        vm_deallocate(mach_task_self(), (vm_address_t)prevCpuInfo, prevCpuInfoSize);
        prevCpuInfo = 0;
    }
    
    if(curCpuInfo) {
        size_t curCpuInfoSize = sizeof(integer_t) * curCpuInfoNum;
        vm_deallocate(mach_task_self(), (vm_address_t)curCpuInfo, curCpuInfoSize);
        curCpuInfo = 0;
    }
#endif
}

int CCPULoadInfo::InitCPUProc()
{
#if defined (_IOS) || defined (_MAC_OS)
    if (curCpuInfo) {
        size_t curCpuInfoSize = sizeof(integer_t) * curCpuInfoNum;
        vm_deallocate(mach_task_self(), (vm_address_t)curCpuInfo, curCpuInfoSize);
    }
    
    natural_t numCPUsU = 0U;
    kern_return_t err = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &numCPUsU, &curCpuInfo, &curCpuInfoNum);
    if(err != KERN_SUCCESS) {
        curCpuInfo = 0;
        return -1;
    }
    
    return 0;
#endif
    
	int nRC = GetCPUInfo(&m_szCPUInfoCur);
	nRC |= GetAPProcInfo(&m_szProcAPInfoCur);
	return nRC;
}

int CCPULoadInfo::UpdateCPUProc()
{
#if defined (_IOS) || defined (_MAC_OS)
    if (prevCpuInfo) {
        size_t prevCpuInfoSize = sizeof(integer_t) * prevCpuInfoNum;
        vm_deallocate(mach_task_self(), (vm_address_t)prevCpuInfo, prevCpuInfoSize);
    }
    
    prevCpuInfo = curCpuInfo;
    prevCpuInfoNum = curCpuInfoNum;
    curCpuInfo = 0;
    curCpuInfoNum = 0;
    
    return 0;
#endif

// 	VOLOGI("Curr u %d, n %d, s %d, i %d, iow %d, irq %d, sirq %d, Old u %d, n %d, s %d, i %d, iow %d, irq %d, sirq %d", 
// 		m_szCPUInfoCur.utime, m_szCPUInfoCur.ntime, m_szCPUInfoCur.stime, m_szCPUInfoCur.itime, m_szCPUInfoCur.iowtime, m_szCPUInfoCur.irqtime, m_szCPUInfoCur.sirqtime, 
// 		m_szCPUInfoOld.utime, m_szCPUInfoOld.ntime, m_szCPUInfoOld.stime, m_szCPUInfoOld.itime, m_szCPUInfoOld.iowtime, m_szCPUInfoOld.irqtime, m_szCPUInfoOld.sirqtime);

	memcpy(&m_szCPUInfoOld, &m_szCPUInfoCur, sizeof(CpuInfo));
	memcpy(&m_szProcAPInfoOld, &m_szProcAPInfoCur, sizeof(ProcInfo));
	m_nCount++;
	return 0;
}

int CCPULoadInfo::GetCPULoad(int* pOutSys, int* pOutUsr, int* pOutAP) {
#if defined (_IOS) || defined (_MAC_OS)
    if ((0 == curCpuInfo) || (0 == prevCpuInfo)) {
        return -1;
    }
    
    int nCpu = voOS_GetCPUNum();
    if (0 == nCpu) {
        nCpu = 1;
    }
    
    float systemUsedPer = 0;
    float userUsedPer = 0;
    
    for (unsigned i = 0U; i < nCpu; ++i) {
        
        float userUsed = curCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_USER] - prevCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_USER];
        
        float systemUsed = (curCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_SYSTEM] - prevCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_SYSTEM])
        + (curCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_NICE] - prevCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_NICE]);
        
        float totalCpu = userUsed + systemUsed + (curCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_IDLE] - prevCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_IDLE]);
        
        if (0 != totalCpu) {
            systemUsedPer = (systemUsed / totalCpu) + systemUsedPer;
            userUsedPer = (userUsed / totalCpu) + userUsedPer;
        }
    }
    
    *pOutSys = (systemUsedPer / nCpu) * 100;
    *pOutUsr = (userUsedPer / nCpu) * 100;
	*pOutAP = *pOutUsr;
    
    return 0;
#endif
    
	int	nCpuUsr = 0;
	int nCpuSys = 0;
	int nCpuIdle = 0;
	int nCpuAP = 0;

#if defined(_LINUX) 
	long unsigned total_delta_time = 0;;
	
	//GetCPUInfo(&m_szCPUInfoCur);
	total_delta_time = (m_szCPUInfoCur.utime + m_szCPUInfoCur.ntime + m_szCPUInfoCur.stime + m_szCPUInfoCur.itime
                        + m_szCPUInfoCur.iowtime + m_szCPUInfoCur.irqtime + m_szCPUInfoCur.sirqtime)
                     - (m_szCPUInfoOld.utime + m_szCPUInfoOld.ntime + m_szCPUInfoOld.stime + m_szCPUInfoOld.itime
                        + m_szCPUInfoOld.iowtime + m_szCPUInfoOld.irqtime + m_szCPUInfoOld.sirqtime);

 	//VOLOGI("cfi_jni Curr u %d, n %d, s %d, i %d, iow %d, irq %d, sirq %d, total_delta_time %d",
 	//	m_szCPUInfoCur.utime, m_szCPUInfoCur.ntime, m_szCPUInfoCur.stime, m_szCPUInfoCur.itime, m_szCPUInfoCur.iowtime, m_szCPUInfoCur.irqtime, m_szCPUInfoCur.sirqtime, total_delta_time);

	m_szProcAPInfoCur.delta_utime = m_szProcAPInfoCur.utime - m_szProcAPInfoOld.utime;
	m_szProcAPInfoCur.delta_stime = m_szProcAPInfoCur.stime - m_szProcAPInfoOld.stime;
	m_szProcAPInfoCur.delta_time = m_szProcAPInfoCur.delta_utime + m_szProcAPInfoCur.delta_stime;

	if (total_delta_time > 0)
	{
		nCpuUsr = ((m_szCPUInfoCur.utime + m_szCPUInfoCur.ntime) - (m_szCPUInfoOld.utime + m_szCPUInfoOld.ntime)) * 100  / total_delta_time;
		nCpuSys = (m_szCPUInfoCur.stime - m_szCPUInfoOld.stime) * 100 / total_delta_time + 1;
		nCpuAP = m_szProcAPInfoCur.delta_time * 100 / total_delta_time;
		nCpuUsr = (nCpuUsr ? nCpuUsr : 1);
		nCpuSys = (nCpuSys ? nCpuSys : 1);
		nCpuAP = (nCpuAP ? nCpuAP : 1);

		VOLOGI("cfi_jni u %d, s %d, ap: %d", nCpuUsr, nCpuSys, nCpuAP);
	}
	else if (total_delta_time < 0)
	{
	total_delta_time = (m_szCPUInfoCur.utime + m_szCPUInfoCur.ntime + m_szCPUInfoCur.stime + m_szCPUInfoCur.itime
                        + m_szCPUInfoCur.iowtime + m_szCPUInfoCur.irqtime + m_szCPUInfoCur.sirqtime);

		nCpuUsr = (m_szCPUInfoCur.utime + m_szCPUInfoCur.ntime) * 100  / total_delta_time + 1;
		nCpuSys = (m_szCPUInfoCur.stime + m_szCPUInfoCur.iowtime + m_szCPUInfoCur.irqtime + m_szCPUInfoCur.sirqtime) * 100 / total_delta_time + 1;
		//nCpuIdle = m_szCPUInfoCur.itime * 100 / total_delta_time;
		nCpuAP = (m_szProcAPInfoCur.utime + m_szProcAPInfoCur.stime) * 100 / total_delta_time + 1; 
		VOLOGI("cfi_jni u %d, s %d, ap: %d", nCpuUsr, nCpuSys, nCpuAP);
	}
#endif
    
	* pOutUsr = nCpuUsr;
	* pOutSys = nCpuSys;
	* pOutAP = nCpuAP;
    
	return 0;
}

int CCPULoadInfo::GetCPUInfo(CpuInfo* pCPUInfo)
{
#if defined(_LINUX) 
	FILE* pFile = NULL;
	pFile = fopen("/proc/stat", "r");
	if(!pFile)
	{
		VOLOGE("Can not open the /proc/stat and set the values\n");
		return -1;
	}
	fscanf(pFile, "cpu  %lu %lu %lu %lu %lu %lu %lu", &pCPUInfo->utime, &pCPUInfo->ntime, &pCPUInfo->stime,
		&pCPUInfo->itime, &pCPUInfo->iowtime, &pCPUInfo->irqtime, &pCPUInfo->sirqtime);
	fclose(pFile);
	return 0;
#elif defined _IOS
    return 0;
#else
	return -1;
#endif


}

int CCPULoadInfo::GetAPProcInfo(ProcInfo* pProcInfo)
{
#if defined(_LINUX) 
	FILE *file;
	char buf[1024], *open_paren, *close_paren;
	int res, idx;

	file = fopen("/proc/self/stat", "r");
	if (!file) return -1;
	fgets(buf, 1024, file);
	fclose(file);

	/* Split at first '(' and last ')' to get process name. */
	open_paren = strchr(buf, '(');
	close_paren = strrchr(buf, ')');
	if (!open_paren || !close_paren) return -1;

	/* Scan rest of string. */
	sscanf(close_paren + 1, " %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d "
		"%lu %lu %*d %*d %*d %*d %*d %*d %*d %lu %ld "
		"%*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %d",
		&pProcInfo->state, &pProcInfo->utime, &pProcInfo->stime, &pProcInfo->vss, &pProcInfo->rss, &pProcInfo->prs);

	return 0;
#elif defined _IOS
	return 0;
#else
	return -1;
#endif
}
