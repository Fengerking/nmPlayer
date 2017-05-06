
#include "voAdaptiveStreamingControllerInfo.h"
#include "voLog.h"
#include "voToolUtility.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voAdaptiveStreamingControllerInfo"
#endif

voAdaptiveStreamingControllerInfo::voAdaptiveStreamingControllerInfo()
:m_StreamingType( VO_ADAPTIVESTREAMPARSER_STREAMTYPE_UNKOWN )
,m_pStreamingParser(0)
,m_pIO(0)
,m_pLibOP(0)
,m_pSampleCB(0)
,m_pEventCB(0)
,m_maxbitrate( 0xffffffff )
,m_IsVerifyInfoValid( VO_FALSE )
,m_pWorkPath(0)
,m_uStartTime(0)
,m_uMinDuration(0)
,m_pProgramInfoOP(NULL)
,m_pVologCB(0)
,m_ProgramType(VO_SOURCE2_STREAM_TYPE_VOD)
,m_pIOHttpCallback(NULL)
,m_bBuffering(VO_FALSE)
,m_bMinBitratePlaying(VO_TRUE)
,m_bBitrateChanging(VO_FALSE)
,m_bBAEnable(VO_TRUE)
,m_bVideoUpdateOn(VO_FALSE)
,m_bAudioUpdateOn(VO_FALSE)
,m_bSubtitleUpdateOn(VO_FALSE)
{
	memset( &m_VerifyInfo , 0 , sizeof( VO_SOURCE2_VERIFICATIONINFO ) );
	memset( m_pDRM, 0,  sizeof(VO_StreamingDRM_API*) * _DRMCOUNTS );
//	memset( m_pWorkPath, 0 , 1024 * sizeof( VO_TCHAR ) );
}

voAdaptiveStreamingControllerInfo::~voAdaptiveStreamingControllerInfo()
{
/*	if( m_pWorkPath )
		delete []m_pWorkPath;
*/
	m_pWorkPath = NULL;
}

VO_VOID voAdaptiveStreamingControllerInfo::SetProgramType( VO_SOURCE2_PROGRAM_TYPE type )
{
	VOLOGI("Get ProgramType:%d", type);
	m_ProgramType = type;
}

VO_VOID voAdaptiveStreamingControllerInfo::SetStreamingType( VO_ADAPTIVESTREAMPARSER_STREAMTYPE type )
{
	m_StreamingType = type;
}

VO_VOID voAdaptiveStreamingControllerInfo::SetAdaptiveStreamingParserPtr( voAdaptiveStreamParserWrapper * pParser )
{
	m_pStreamingParser = pParser;
}

VO_VOID voAdaptiveStreamingControllerInfo::SetIOPtr( VO_SOURCE2_IO_API * pAPI )
{
	m_pIO = pAPI;
}

VO_VOID voAdaptiveStreamingControllerInfo::SetLibOp( VO_SOURCE2_LIB_FUNC * pLibOp )
{
	m_pLibOP = pLibOp;
}

VO_VOID voAdaptiveStreamingControllerInfo::SetSampleCallback( VO_SOURCE2_SAMPLECALLBACK * pSampleCB )
{
	m_pSampleCB = pSampleCB;
}

VO_VOID voAdaptiveStreamingControllerInfo::SetEventCallback( VO_SOURCE2_EVENTCALLBACK * pEventCB )
{
	m_pEventCB = pEventCB;
}
VO_VOID voAdaptiveStreamingControllerInfo::SetIOEventCallback( VO_SOURCEIO_EVENTCALLBACK * pEventCB )
{
	m_pIOEventCB = pEventCB;
}
//For DRM
VO_VOID voAdaptiveStreamingControllerInfo::SetDRMPtr(VO_U32 nThreadType, VO_StreamingDRM_API * pDRM )
{
	if( pDRM->hHandle && pDRM->Init )
		m_pDRM[nThreadType] = pDRM;
	else
		m_pDRM[nThreadType] = 0;

}
VO_StreamingDRM_API * voAdaptiveStreamingControllerInfo::GetDRMPtr(VO_U32 nThreadType )
{
	return m_pDRM[nThreadType];
}


VO_VOID voAdaptiveStreamingControllerInfo::SetStartUTC( VO_U64 utc )
{
	m_StartUTC = utc;
}

VO_VOID voAdaptiveStreamingControllerInfo::SetStartSysTime( VO_U64 systime )
{
	m_StartSysTime = systime;
}

VO_VOID voAdaptiveStreamingControllerInfo::SetMaxDownloadBitrate( VO_U32 max_bitrate )
{
	m_maxbitrate = max_bitrate;
}

VO_VOID voAdaptiveStreamingControllerInfo::SetVerificationInfo( VO_SOURCE2_VERIFICATIONINFO * pInfo )
{
	DestroyVerificationInfo();

	m_IsVerifyInfoValid = VO_TRUE;

	m_VerifyInfo.pUserData = pInfo->pUserData;


	m_VerifyInfo.pData = new VO_BYTE[ pInfo->uDataSize + 1 ];
	memset( m_VerifyInfo.pData , 0 , pInfo->uDataSize + 1 );
	memcpy( m_VerifyInfo.pData , pInfo->pData , pInfo->uDataSize );
	m_VerifyInfo.uDataSize = pInfo->uDataSize;
	m_VerifyInfo.uDataFlag = 0;

	m_VerifyInfo.szResponse = new VO_CHAR[ pInfo->uResponseSize + 1 ];
	memset( m_VerifyInfo.szResponse , 0 , pInfo->uResponseSize + 1 );
	memcpy( m_VerifyInfo.szResponse , pInfo->szResponse , pInfo->uResponseSize );
	m_VerifyInfo.uResponseSize = pInfo->uResponseSize;

	m_VerifyInfo.uDataFlag = pInfo->uDataFlag;
}

VO_VOID voAdaptiveStreamingControllerInfo::DestroyVerificationInfo()
{
	if( !m_IsVerifyInfoValid )
		return;

	if( m_VerifyInfo.pData )
	{
		if(m_VerifyInfo.uDataFlag == 0)
			delete [] ((VO_BYTE*)m_VerifyInfo.pData );
		else
		{
			VOLOGE( " Very important Information. VO_SOURCE2_VERIFICATIONINFO DataFlag is not equal to 0, Please Check immediately");
			VOLOGE( " Very important Information. VO_SOURCE2_VERIFICATIONINFO DataFlag is not equal to 0, Please Check immediately");
			VOLOGE( " Very important Information. VO_SOURCE2_VERIFICATIONINFO DataFlag is not equal to 0, Please Check immediately");
		}
	}
	if( m_VerifyInfo.szResponse )
		delete []m_VerifyInfo.szResponse;

	memset( &m_VerifyInfo , 0 , sizeof( VO_SOURCE2_VERIFICATIONINFO ) );

	m_IsVerifyInfoValid = VO_FALSE; 
}

VO_VOID voAdaptiveStreamingControllerInfo::SetWorkPath( VO_TCHAR * pWorkPath )
{ 
	m_pWorkPath = pWorkPath;
//	DSStrCopy( &m_pWorkPath, pWorkPath );
}

VO_VOID voAdaptiveStreamingControllerInfo::SetBuffering(VO_BOOL bBuffering)
{
	VOLOGI("Current %d, New %d", m_bBuffering, bBuffering);
	m_bBuffering = bBuffering;
}

VO_VOID	voAdaptiveStreamingControllerInfo::SetUpdateUrlOn(THREADTYPE type, VO_BOOL bUpdateOn)
{
	switch(type)
	{
		case THREADTYPE_MEDIA_AUDIO:
		{
			m_bAudioUpdateOn = bUpdateOn;
			break;
		}
		case THREADTYPE_MEDIA_VIDEO:
		{
			m_bVideoUpdateOn = bUpdateOn;
			break;
		}
		case THREADTYPE_MEDIA_SUBTITLE:
		{
			m_bSubtitleUpdateOn = bUpdateOn;
			break;
		}
		default:
			break;
	}
}

VO_BOOL	 voAdaptiveStreamingControllerInfo::IsUpdateUrlOn(THREADTYPE type)
{
	switch(type)
	{	
		case THREADTYPE_MEDIA_AUDIO:
		{
			return m_bAudioUpdateOn;
		}
		case THREADTYPE_MEDIA_VIDEO:
		{
			return m_bVideoUpdateOn;
		}
		case THREADTYPE_MEDIA_SUBTITLE:
		{
			return m_bSubtitleUpdateOn;
		}
		default:
			return VO_FALSE;
	}
}
