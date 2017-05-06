
#include "voSourceBaseWrapper.h"
#include "voFile.h"
#include "voLog.h"
#include "fCC.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#ifdef H265_DUMP
const char * h265_dump_file = "D:\\doc\\h265\\h265_dump_file\\h265_dump_file.h265"; 
FILE *fp = NULL;
#endif

voSourceBaseWrapper::voSourceBaseWrapper()
:m_nFlag(0)
,m_nOpenFlag(0)
,m_ptr_drmcallback(0)
,m_ptr_EventCallback(NULL)
,m_ullDuration(0)
,m_uAudioTrack(0xffffffff)
,m_uVideoTrack(0xffffffff)
,m_uSubtitleTrack(0xffffffff)
,m_pAudioTrackInfo(0)
,m_pVideoTrackInfo(0)
,m_pSubtitleTrackInfo(0)
,m_pHeadData (NULL)
,m_uHeadSize (0)
,m_uNalLen (4)
,m_uNalWord (0X01000000)
,m_pVideoData (NULL)
,m_uVideoSize (0)
,m_uFrameSize (0)
,m_pAudioData (NULL)
,m_nAudioSize (0)
,m_pSubtitleData (NULL)
,m_nSubtitleSize (0)
,m_bAudioHeadDataSend(VO_FALSE)
,m_bVideoHeadDataSend(VO_FALSE)
,m_bSubtitleHeadDataSend(VO_FALSE)
,m_pStreamInfo(0)
,m_pProgramInfo(0)
,m_FileHandle(NULL)
,m_nCurrentTimeStamp(0 )
,m_pLibOp(NULL)
,m_pstrWorkPath(NULL)
,m_pVideoTimeStamp(0)
,m_pAudioTimeStamp(0)
,m_pSubtitleTimeStamp(0)

{
	memset( m_Url , 0 , sizeof( m_Url ) * sizeof( VO_CHAR ) );
	m_ptr_drmcallback = new VO_SOURCEDRM_CALLBACK2;
	memset(m_ptr_drmcallback,0,sizeof(VO_SOURCEDRM_CALLBACK2));
}

voSourceBaseWrapper::~voSourceBaseWrapper()
{
	if (m_ptr_drmcallback)
	{
		delete m_ptr_drmcallback;
		m_ptr_drmcallback = NULL;
	}
}

VO_U32 voSourceBaseWrapper::Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pParam )
{
	m_nFlag = nFlag;

	if (pParam != NULL && pParam->strWorkPath != NULL)
	{
		m_pstrWorkPath = new VO_TCHAR[vostrlen(pParam->strWorkPath) + 1];
		vostrcpy(m_pstrWorkPath,pParam->strWorkPath);
		m_pstrWorkPath[vostrlen(pParam->strWorkPath)] = 0;
	}
	
	if( nFlag & VO_SOURCE2_FLAG_OPEN_URL   )
	{
		vostrcpy( m_Url , (VO_TCHAR*)pSource );
		return VO_RET_SOURCE2_OK;
	}
	else if((nFlag & VO_SOURCE2_FLAG_OPEN_HANDLE) ||(nFlag &VO_SOURCE2_FLAG_OPEN_FILEIO))
	{
		m_FileHandle = pSource;
		return VO_RET_SOURCE2_OK;
	}

	return VO_RET_SOURCE2_FAIL;
}

VO_U32 voSourceBaseWrapper::Uninit()
{
	if( m_pStreamInfo )
	{
		VO_U32 nTrackCnt = 0;
		while(nTrackCnt < m_pStreamInfo->uTrackCount)
		{
			if( m_pStreamInfo->ppTrackInfo[nTrackCnt]->pHeadData )
			{
				delete []m_pStreamInfo->ppTrackInfo[nTrackCnt]->pHeadData;
				m_pStreamInfo->ppTrackInfo[nTrackCnt]->pHeadData = NULL;
			}
			delete m_pStreamInfo->ppTrackInfo[nTrackCnt];
			m_pStreamInfo->ppTrackInfo[nTrackCnt] = NULL;

			nTrackCnt++;
		}
		if( m_pStreamInfo->ppTrackInfo )
		{
			delete []m_pStreamInfo->ppTrackInfo;
			m_pStreamInfo->ppTrackInfo = NULL;
		}
		delete m_pStreamInfo;
		m_pStreamInfo = NULL;
	}

	if( m_pProgramInfo )
	{
		if( m_pProgramInfo->ppStreamInfo )
		{
			delete []m_pProgramInfo->ppStreamInfo;
			m_pProgramInfo->ppStreamInfo = NULL;
		}

		delete m_pProgramInfo;
		m_pProgramInfo = NULL;
	}

	if( m_pHeadData )
		delete []m_pHeadData;

	if( m_pVideoData )
		delete []m_pVideoData;

	if( m_pAudioData )
		delete []m_pAudioData;

	if( m_pSubtitleData )
		delete []m_pSubtitleData;

	if(m_FileHandle != NULL)
		m_FileHandle = NULL;

	if (m_pstrWorkPath)
	{
		delete []m_pstrWorkPath;
		m_pstrWorkPath = NULL;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 voSourceBaseWrapper::Open()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	if(VO_RET_SOURCE2_OK != (ret = OnOpen()))
	{
		return ret;
	}
	if (VO_RET_SOURCE2_OK != (ret = OnGetDuration(&m_ullDuration)))
	{
		return ret;
	}
	VO_U32 uTraCnt = 0;
	if (VO_RET_SOURCE2_OK != (ret = OnGetTrackCnt(&uTraCnt)))
	{
		return ret;
	}

	CreateStreamInfo( uTraCnt );
	NotifyEvent( VO_EVENTID_SOURCE2_PROGRAMRESET, 0, 0);

    return VO_RET_SOURCE2_OK;
}

VO_U32 voSourceBaseWrapper::Close()
{
	OnClose();
    return VO_RET_SOURCE2_OK;
}

VO_U32 voSourceBaseWrapper::Start()
{
	return VO_RET_SOURCE2_OK;
}

VO_U32 voSourceBaseWrapper::Pause()
{
	return VO_RET_SOURCE2_OK;
}

VO_U32 voSourceBaseWrapper::Stop()
{
	m_nCurrentTimeStamp = 0;

	return VO_RET_SOURCE2_OK;
}

VO_U32 voSourceBaseWrapper::Seek(VO_U64* pTimeStamp)
{
	m_bVideoHeadDataSend = VO_FALSE;
	m_bAudioHeadDataSend = VO_FALSE;
	m_bSubtitleHeadDataSend = VO_FALSE;
	m_pVideoTimeStamp = ((*pTimeStamp) <= 0) ? 0 : ((*pTimeStamp) - 1);
	m_pAudioTimeStamp = ((*pTimeStamp) <= 0) ? 0 : ((*pTimeStamp) - 1);
	m_pSubtitleTimeStamp = ((*pTimeStamp) <= 0) ? 0 : ((*pTimeStamp) - 1);

	VOLOGI("Seek TimeStamp=%llu",m_pVideoTimeStamp);
	return VO_RET_SOURCE2_OK;
}

VO_U32 voSourceBaseWrapper::GetDuration(VO_U64 * pDuration)
{
	if (!pDuration)
	{
		return VO_RET_SOURCE2_ERRORDATA;
	}
	return OnGetDuration(pDuration);
}

VO_U32 voSourceBaseWrapper::GetSample(VO_SOURCE2_TRACK_TYPE nOutPutType , VO_PTR pSample )
{
/*
	if( nOutPutType == VO_SOURCE2_TT_SUBTITLE )
		return VO_RET_SOURCE2_OUTPUTNOTFOUND;
*/
	VO_SOURCE2_SAMPLE * ptr_sample = (VO_SOURCE2_SAMPLE *)pSample;
	VOLOGI("GetSample---nOutPutType=%lu",nOutPutType);
	if( !m_bVideoHeadDataSend && nOutPutType == VO_SOURCE2_TT_VIDEO )
	{
		m_bVideoHeadDataSend = VO_TRUE;

		if( m_uVideoTrack == 0xffffffff )
		{
			memset( ptr_sample , 0 , sizeof(VO_SOURCE2_SAMPLE) );
			ptr_sample->uFlag = VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE;
			return VO_RET_SOURCE2_OK;
		}
		memset( ptr_sample , 0 , sizeof(VO_SOURCE2_SAMPLE) );
		ptr_sample->pFlagData = (VO_PBYTE)m_pVideoTrackInfo;
		ptr_sample->uFlag = VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT;
		ptr_sample->uTime = m_pVideoTimeStamp;
		VOLOGI("Seek TimeStamp=%llu",m_pVideoTimeStamp);

		return VO_RET_SOURCE2_OK;
	}

	if( !m_bAudioHeadDataSend && nOutPutType == VO_SOURCE2_TT_AUDIO )
	{
		m_bAudioHeadDataSend = VO_TRUE;

		if( m_uAudioTrack == 0xffffffff )
		{
			memset( ptr_sample , 0 , sizeof(VO_SOURCE2_SAMPLE) );
			ptr_sample->uFlag = VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE;
			return VO_RET_SOURCE2_OK;
		}
		memset( ptr_sample , 0 , sizeof(VO_SOURCE2_SAMPLE) );
		ptr_sample->pFlagData = (VO_PBYTE)m_pAudioTrackInfo;
		ptr_sample->uFlag = VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT;
		ptr_sample->uTime = m_pAudioTimeStamp;
		VOLOGI("Seek TimeStamp=%llu",m_pAudioTimeStamp);

		return VO_RET_SOURCE2_OK;
	}

	if(!m_bSubtitleHeadDataSend && nOutPutType == VO_SOURCE2_TT_SUBTITLE )
	{
		m_bSubtitleHeadDataSend = VO_TRUE;

		if( m_uSubtitleTrack == 0xffffffff )
		{
			memset( ptr_sample , 0 , sizeof(VO_SOURCE2_SAMPLE) );
			ptr_sample->uFlag = VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE;
			return VO_RET_SOURCE2_OK;
		}
		memset( ptr_sample , 0 , sizeof(VO_SOURCE2_SAMPLE) );
		ptr_sample->pFlagData = (VO_PBYTE)m_pSubtitleTrackInfo;
		ptr_sample->uFlag = VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT;
		ptr_sample->uTime = m_pSubtitleTimeStamp;
		VOLOGI("Seek TimeStamp=%llu",m_pSubtitleTimeStamp);

		return VO_RET_SOURCE2_OK;

	}

	VO_SOURCE_SAMPLE sample;
	memset( &sample , 0 , sizeof( VO_SOURCE_SAMPLE ) );

	sample.Time = (VO_S64)ptr_sample->uTime;

	VO_U32 ret = VO_RET_SOURCE2_OK;
	if (VO_RET_SOURCE2_OK != (ret = OnGetSample(nOutPutType , &sample)))
	{
		return ret;
	}

	switch(nOutPutType)
	{
		case VO_SOURCE2_TT_VIDEO:
			m_pVideoTimeStamp = sample.Time+1;
			break;
		case VO_SOURCE2_TT_AUDIO:
			m_pAudioTimeStamp = sample.Time+1;
			break;
		case VO_SOURCE2_TT_SUBTITLE:
			m_pSubtitleTimeStamp = sample.Time+1;
			break;
	}

	ptr_sample->uDuration = sample.Duration;
	ptr_sample->uFlag = ( sample.Size & 0x80000000 ? VO_SOURCE2_FLAG_SAMPLE_KEYFRAME : 0 );
	ptr_sample->pBuffer = sample.Buffer;
	ptr_sample->uSize = sample.Size & 0x7fffffff;
	ptr_sample->uTime = sample.Time;

	if( VO_SOURCE2_TT_VIDEO == nOutPutType && m_pVideoTrackInfo->uCodec == VO_VIDEO_CodingH264)
	{
		VO_U32 nForCC = *(VO_U32*)m_pVideoTrackInfo->strFourCC;
		if (FOURCC_AVC1 == nForCC)
		{
			if( VO_TRUE == ConvertData (ptr_sample->pBuffer, ptr_sample->uSize ))
			{
				if (m_pVideoData != NULL)
				{
					ptr_sample->pBuffer = m_pVideoData;
					ptr_sample->uSize = m_uVideoSize;
				}
			}
			else
				return VO_RET_SOURCE2_NEEDRETRY;
		}	
	}

	if( VO_SOURCE2_TT_VIDEO == nOutPutType && m_pVideoTrackInfo->uCodec == VO_VIDEO_CodingH265)//h265 has the same visual box structure
	{

		VO_U32 nForCC = *(VO_U32*)m_pVideoTrackInfo->strFourCC;
		if (FOURCC_HVC1 == nForCC)
		{
#ifdef H265_DUMP
		if(fp)
		{
			fwrite(ptr_sample->pBuffer,1,ptr_sample->uSize,fp);
		}
		else
		{
			fp = fopen(h265_dump_file,"wb");
			fwrite(ptr_sample->pBuffer,1,ptr_sample->uSize,fp);
		}
#endif
#ifndef  PARSER_H265
			if( VO_TRUE == ConvertData (ptr_sample->pBuffer, ptr_sample->uSize ))
			{
				if (m_pVideoData != NULL)
				{
					ptr_sample->pBuffer = m_pVideoData;
					ptr_sample->uSize = m_uVideoSize;
				}
			}
			else
				return VO_RET_SOURCE2_NEEDRETRY;
#endif
		}

	}

	if ( VO_SOURCE2_TT_AUDIO == nOutPutType && m_pAudioTrackInfo->uCodec == VO_AUDIO_CodingAAC)
	{

		VO_U32 nForCC = *(VO_U32*)m_pAudioTrackInfo->strFourCC;
		if ((nForCC == 0XA106 || nForCC == 0xFF) && !IsADTSBuffer(ptr_sample->pBuffer, ptr_sample->uSize))
		{
			VO_U32 nHeadSize = m_nAudioSize;
			
			memset(m_pAudioData, 0x0, m_nAudioSize * 2);
			if(Config2ADTSHeader (m_pAudioTrackInfo->pHeadData, m_pAudioTrackInfo->uHeadSize, ptr_sample->uSize , m_pAudioData, &nHeadSize) != 0){
				nHeadSize = 0;
			}
			memcpy (m_pAudioData + nHeadSize, ptr_sample->pBuffer, ptr_sample->uSize );
			ptr_sample->uSize += nHeadSize;
			ptr_sample->pBuffer = m_pAudioData;
		}
	}
	
	if ( VO_SOURCE2_TT_SUBTITLE == nOutPutType)
	{
	}

	SetCurrentTimeStamp(ptr_sample->uTime, nOutPutType);
	return VO_RET_SOURCE2_OK;
}

VO_U32 voSourceBaseWrapper::GetProgramCount(VO_U32 *pProgramCount)
{
	*pProgramCount = 1;
	return VO_RET_SOURCE2_OK;
}

VO_U32 voSourceBaseWrapper::GetProgramInfo(VO_U32 uProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo)
{
	*pProgramInfo = m_pProgramInfo;
	return VO_RET_SOURCE2_OK;
}

VO_U32 voSourceBaseWrapper::GetCurTrackInfo(VO_SOURCE2_TRACK_TYPE eTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo )
{
	if( eTrackType == VO_SOURCE2_TT_VIDEO )
		*ppTrackInfo = m_pVideoTrackInfo;
	else if( eTrackType == VO_SOURCE2_TT_AUDIO )
		*ppTrackInfo = m_pAudioTrackInfo;
	else if( eTrackType == VO_SOURCE2_TT_SUBTITLE )
		*ppTrackInfo = m_pSubtitleTrackInfo;
	else
		return VO_RET_SOURCE2_OUTPUTNOTFOUND;

	return VO_RET_SOURCE2_OK;
}

VO_U32 voSourceBaseWrapper::SelectProgram(VO_U32 uProgram)
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 voSourceBaseWrapper::SelectStream(VO_U32 uStream)
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 voSourceBaseWrapper::SelectTrack(VO_U32 uTrack)
{	
	return VO_RET_SOURCE2_OK;
}

VO_U32 voSourceBaseWrapper::GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 voSourceBaseWrapper::SendBuffer(const VO_SOURCE2_SAMPLE& Buffer )
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 voSourceBaseWrapper::GetParam(VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32 nRet = VO_RET_SOURCE2_OK;
	
	switch( nParamID )
	{
	case VO_PID_SOURCE2_SEEKRANGEINFO:
		{
			if(!pParam){
				return VO_RET_SOURCE2_EMPTYPOINTOR;
			}			
			
			VO_SOURCE2_SEEKRANGE_INFO   *pDVRInfo = (VO_SOURCE2_SEEKRANGE_INFO *)pParam;

			pDVRInfo->ullStartTime = 0;
			OnGetFileLength(&pDVRInfo->ullEndTime);
			GetCurrentTimeStamp(&pDVRInfo->ullPlayingTime);	
			pDVRInfo->ullLiveTime = UNAVALIABLETIME ;
			VOLOGI("GetParam---SEEKRANGEINFO---ullStartTime=%llu, ullEndTime=%llu, ullPlayingTime=%llu, ullLiveTime=%llu",
				pDVRInfo->ullStartTime, pDVRInfo->ullEndTime, pDVRInfo->ullPlayingTime, pDVRInfo->ullLiveTime);
		}
		break;
	default:		
		nRet = VO_RET_SOURCE2_FAIL;
		break;
	}
	return nRet;
}

VO_U32 voSourceBaseWrapper::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32 nRet = VO_RET_SOURCE2_OK;

	switch( nParamID )
	{
	case VO_PID_SOURCE2_DRMCALLBACK:
		{
			if (pParam)
			{
				memcpy(m_ptr_drmcallback,pParam,sizeof(VO_SOURCEDRM_CALLBACK2));
			}
		}
		break;

	case VO_PID_COMMON_LOGFUNC:
		{
			VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
//			vologInit(pVologCB->pUserData, pVologCB->fCallBack);
		}
		break;
	
	case VO_PID_SOURCE2_EVENTCALLBACK:
		{
			m_ptr_EventCallback = (VO_SOURCE2_EVENTCALLBACK *)pParam;
		}
		break;
	default:
		{
			nRet = VO_RET_SOURCE2_FAIL;
		}
		break;
	}
	
	return nRet;
}

VO_BOOL voSourceBaseWrapper::ConvertData (VO_PBYTE pData, VO_U32 nSize)
{
	if (m_pHeadData == NULL)
		return VO_TRUE;

	VO_PBYTE pBuffer = pData;
	VO_U32	 nFrameLen = 0;

	m_uVideoSize = 0;

	int i = 0;
	while (pBuffer - pData + m_uNalLen < nSize)
	{
		nFrameLen = *pBuffer++;
		for (i = 0; i < (int)m_uNalLen - 1; i++)
		{
			nFrameLen = nFrameLen << 8;
			nFrameLen += *pBuffer++;
		}

		if(nFrameLen > nSize)
			return VO_FALSE;

		if (m_uNalLen == 3 || m_uNalLen == 4)
		{
			memcpy ((pBuffer - m_uNalLen), &m_uNalWord, m_uNalLen);
		}
		else
		{
			memcpy (m_pVideoData + m_uVideoSize, &m_uNalWord, 4);
			m_uVideoSize += 4;
			memcpy (m_pVideoData + m_uVideoSize, pBuffer, nFrameLen);
			m_uVideoSize += nFrameLen;
		}

		pBuffer += nFrameLen;
	}

	return VO_TRUE;
}

VO_BOOL voSourceBaseWrapper::ConvertHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize)
{
	if (nHeadSize < 12)
		return VO_TRUE;

	if (m_pHeadData != NULL)
		return VO_TRUE;

	if((0 == pHeadData[0] && 0 == pHeadData[1]) && ((0 == pHeadData[2] && 1 == pHeadData[3]) || 1 == pHeadData[2])){
		return VO_TRUE;
	}

	char* pData = (char *)pHeadData;
	int numOfPictureParameterSets;
	int configurationVersion = pData[0];
	int AVCProfileIndication = pData[1];
	int profile_compatibility = pData[2];
	int AVCLevelIndication  = pData[3];
	configurationVersion = configurationVersion;
	AVCProfileIndication = AVCProfileIndication;
	profile_compatibility = profile_compatibility;
	AVCLevelIndication = AVCLevelIndication;
	m_uNalLen =  (pData[4]&0x03)+1;
	int nNalLen = m_uNalLen;

	m_uFrameSize = (m_uFrameSize > nHeadSize) ? m_uFrameSize : nHeadSize;
	
	if (m_uNalLen == 3)
		m_uNalWord = 0X010000;
	if (m_uNalLen < 3)
	{
		m_pVideoData = new VO_BYTE[512 + m_uFrameSize];
		nNalLen = 4;
	}

	m_pHeadData = new VO_BYTE[512 + m_uFrameSize];
	m_uHeadSize = 0;

	int i = 0;
	int numOfSequenceParameterSets = pData[5]&0x1f;
	VO_U8 * pBuffer = (VO_U8*)pData+6;
	for (i=0; i< numOfSequenceParameterSets; i++)
	{
		int sequenceParameterSetLength = (pBuffer[0]<<8)|pBuffer[1];
		pBuffer+=2;

		memcpy (m_pHeadData + m_uHeadSize, &m_uNalWord, nNalLen);
		m_uHeadSize += nNalLen;

		memcpy (m_pHeadData + m_uHeadSize, pBuffer, sequenceParameterSetLength);
		m_uHeadSize += sequenceParameterSetLength;

		pBuffer += sequenceParameterSetLength;
	}

	numOfPictureParameterSets = *pBuffer++;
	for (i=0; i< numOfPictureParameterSets; i++)
	{
		int pictureParameterSetLength = (pBuffer[0]<<8)|pBuffer[1];
		pBuffer+=2;

		memcpy (m_pHeadData + m_uHeadSize, &m_uNalWord, nNalLen);
		m_uHeadSize += nNalLen;

		memcpy (m_pHeadData + m_uHeadSize, pBuffer, pictureParameterSetLength);
		m_uHeadSize += pictureParameterSetLength;

		pBuffer += pictureParameterSetLength;
	}

	return VO_TRUE;
}
VO_BOOL voSourceBaseWrapper::HEVC_ConvertHeadData(VO_PBYTE pHeadData, VO_U32 nHeadSize)
{
	 if (nHeadSize < 12)
	  return VO_TRUE;

	 if (m_pHeadData != NULL)
	  return VO_TRUE;

	 VO_PBYTE pData = pHeadData;
	 m_uNalLen =  (pData[21]&0x03)+1;
	 VO_U32 nNalLen = m_uNalLen;
	 m_uFrameSize = (m_uFrameSize > nHeadSize) ? m_uFrameSize : nHeadSize;

	 if (m_uNalLen == 3)
	  m_uNalWord = 0X010000;
	 if (m_uNalLen < 3)
	 {
		  m_pVideoData = new VO_BYTE[512 + m_uFrameSize];
		  nNalLen = 4;
	 }

	 m_pHeadData = new VO_BYTE[512 + m_uFrameSize];
	 m_uHeadSize = 0;

	 VO_U8 numOfArrays = pData[22];

	 pData += 23;
	 if(numOfArrays)
	 {
		  for(int arrNum = 0; arrNum < numOfArrays; arrNum++)
		  {
			   VO_U8 nal_type = 0;
			   nal_type = pData[0]&0x3F;
			   pData += 1;
			   switch(nal_type)
			   {
			   case 33://sps
				{
			     
					 VO_U16 numOfSequenceParameterSets = 0;
					 numOfSequenceParameterSets = ((numOfSequenceParameterSets|pData[0]) << 8)|pData[1];
					 pData += 2;
					 for(int i = 0; i < numOfSequenceParameterSets; i++)
					 {
					  memcpy (m_pHeadData + m_uHeadSize, &m_uNalWord, nNalLen);
					  m_uHeadSize += nNalLen;
					  VO_U16 sequenceParameterSetLength = pData[0];
					  sequenceParameterSetLength = (sequenceParameterSetLength << 8)|pData[1];
					  pData += 2;
					  memcpy (m_pHeadData + m_uHeadSize, pData, sequenceParameterSetLength);
					  m_uHeadSize += sequenceParameterSetLength;

					  pData += sequenceParameterSetLength;

					 }
				}
				break;
			   case 34://pps
				{
			     
					 VO_U16 numofPictureParameterSets = pData[0];
					 numofPictureParameterSets = (numofPictureParameterSets << 8)|pData[1];
					 pData += 2;

					 for(int i = 0; i < numofPictureParameterSets; i++)
					 {
					  memcpy (m_pHeadData + m_uHeadSize, &m_uNalWord, nNalLen);
					  m_uHeadSize += nNalLen;
					  VO_U16 pictureParameterSetLength = pData[0];
					  pictureParameterSetLength = (pictureParameterSetLength << 8)|pData[1];
					  pData += 2;
					  memcpy (m_pHeadData + m_uHeadSize, pData, pictureParameterSetLength);
					  m_uHeadSize += pictureParameterSetLength;
					  pData += pictureParameterSetLength;
					 }
					}
					break;
			   case 32: //aps
				{
					 VO_U16 numofAdaptationParameterSets = pData[0];
					 numofAdaptationParameterSets = (numofAdaptationParameterSets << 8)|pData[1];
					 pData += 2;

					 for(int i = 0; i < numofAdaptationParameterSets; i++)
					 {
					  memcpy (m_pHeadData + m_uHeadSize, &m_uNalWord, nNalLen);
					  m_uHeadSize += nNalLen;
					  VO_U16 adaptationParameterSetLength = pData[0];
					  adaptationParameterSetLength = (adaptationParameterSetLength << 8)|pData[1];
					  pData += 2;
					  memcpy (m_pHeadData + m_uHeadSize, pData, adaptationParameterSetLength);
					  m_uHeadSize += adaptationParameterSetLength;
					  pData += adaptationParameterSetLength;
					 }
				}
				break;
			   default://just skip the data block
				{
					 VO_U16 numofskippingParameter = pData[0];
					 numofskippingParameter = (numofskippingParameter << 8)|pData[1];
					 pData += 2;
					 for(int i = 0; i < numofskippingParameter; i++)
					 {
					  VO_U16 adaptationParameterSetLength = pData[0];
					  adaptationParameterSetLength = (adaptationParameterSetLength << 8)|pData[1];
					  pData += 2;
					  pData += adaptationParameterSetLength;
					 }

				}
				break;
			   }
		  }
	 }

	return VO_TRUE;
}
VO_S32 voSourceBaseWrapper::Config2ADTSHeader(VO_PBYTE pConfig, VO_U32 uConlen,VO_U32 uFramelen
											  ,VO_PBYTE pAdtsBuf,VO_U32 *pAdtsLen)
{
	VO_S32 object, sampIdx, sampFreq, chanNum;

	if(pConfig == NULL || uConlen < 2 || pAdtsBuf == NULL || *pAdtsLen < 7)
		return -1;

	object = pConfig[0] >> 3;
	if(object > 5)
	{
		return -2;
	}

	if(object == 0) object += 1;

	object--;

	sampIdx = ((pConfig[0] & 7) << 1) | (pConfig[1] >> 7);
	if(sampIdx == 0x0f)
	{
		int idx;

		if(uConlen < 5)
			return -3;

		sampFreq = ((pConfig[1]&0x7f) << 17) | (pConfig[2] << 9) | ((pConfig[3] << 1)) | (pConfig[4] >> 7);

		for (idx = 0; idx < 12; idx++) {
			if (sampFreq == AAC_SampRateTab[idx]) {
				sampIdx = idx;
				break;
			}
		}

		if (idx == 12)
			return -4;

		chanNum = (pConfig[4]&0x78) >> 3;
	}
	else
	{
		chanNum = (pConfig[1]&0x78) >> 3;
	}

	if(chanNum > 7)
		return -5;

	pAdtsBuf[0] = 0xFF; pAdtsBuf[1] = 0xF9;

	pAdtsBuf[2] = (unsigned char)((object << 6) | (sampIdx << 2) | ((chanNum&4)>>2));

	uFramelen += 7;

	if(uFramelen > 0x1FFF)
		return -6;

	pAdtsBuf[3] = (chanNum << 6) | (uFramelen >> 11);
	pAdtsBuf[4] = (uFramelen & 0x7FF) >> 3;
	pAdtsBuf[5] = ((uFramelen & 7) << 5) | 0x1F;
	pAdtsBuf[6] = 0xFC;	

	*pAdtsLen = 7;

	return 0;
}

VO_VOID voSourceBaseWrapper::CreateStreamInfo( VO_U32 nTracks )
{
	VO_AUDIO_FORMAT AudioFormat;
	VO_VIDEO_FORMAT VideoFormat;
//	VO_VIDEO_FORMAT SubtitleFormat;
	
	VO_U32 uAudioCnt = 0;
	VO_U32 uSubtitleCnt = 0;

	m_pStreamInfo = new VO_SOURCE2_STREAM_INFO;
	memset( m_pStreamInfo , 0 , sizeof( VO_SOURCE2_STREAM_INFO ) );
	m_pStreamInfo->uStreamID = 0x47;
	m_pStreamInfo->uTrackCount = nTracks;
	m_pStreamInfo->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
	m_pStreamInfo->uBitrate = GetBitrate();
	m_pStreamInfo->ppTrackInfo = new VO_SOURCE2_TRACK_INFO *[nTracks];
	memset(m_pStreamInfo->ppTrackInfo,0,sizeof(VO_SOURCE2_TRACK_INFO *) * nTracks);

	m_pProgramInfo = new VO_SOURCE2_PROGRAM_INFO;
	memset( m_pProgramInfo , 0 , sizeof( VO_SOURCE2_PROGRAM_INFO ) );
	m_pProgramInfo->uProgramID = 0x48;
	m_pProgramInfo->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
	m_pProgramInfo->uStreamCount = 1;
	m_pProgramInfo->sProgramType = VO_SOURCE2_STREAM_TYPE_VOD;	
	m_pProgramInfo->ppStreamInfo = new VO_SOURCE2_STREAM_INFO *[1];
	m_pProgramInfo->ppStreamInfo[0] = m_pStreamInfo;

	for( VO_U32 i = 0 ; i < nTracks ; i++ )
	{
		VO_BOOL bAvaliable = VO_FALSE;
		if(VO_RET_SOURCE2_OK != OnGetTrackAvailable(i , &bAvaliable) || bAvaliable == VO_FALSE)
		{
			m_pStreamInfo->ppTrackInfo[i] = new VO_SOURCE2_TRACK_INFO;
			memset( m_pStreamInfo->ppTrackInfo[i] , 0 , sizeof( VO_SOURCE2_TRACK_INFO ) );
			m_pStreamInfo->ppTrackInfo[i]->uSelInfo = VO_SOURCE2_SELECT_DISABLE;
			continue;
		}
		memset(&AudioFormat,0,sizeof(VO_AUDIO_FORMAT));
		memset(&VideoFormat,0,sizeof(VO_VIDEO_FORMAT));

		VO_BOOL bUpSideDown = VO_FALSE;
		VO_SOURCE_TRACKINFO TrackInfo;
		memset( &TrackInfo , 0 , sizeof(VO_SOURCE_TRACKINFO) );

		VO_U32 uFourCC = 0;
		VO_U32 uMaxSampleSize = 0;

		OnGetTrackInfo(i , &TrackInfo);
		OnGetTrackFourCC(i , &uFourCC);
		OnGetTrackMaxSampleSize(i , &uMaxSampleSize);

		VO_SOURCE2_TRACK_INFO ** ptr_sel = 0;

		ptr_sel = &m_pStreamInfo->ppTrackInfo[i];

		if( TrackInfo.Type == VO_SOURCE_TT_AUDIO)
		{
			if ( m_uAudioTrack == 0xffffffff )
			{
				m_uAudioTrack = i;
				VOLOGI("CreateStreamInfo---m_uAudioTrack=%ld",i);
			}
			
			OnGetAudioFormat(i , &AudioFormat);

			if( TrackInfo.Codec == VO_AUDIO_CodingWMA || TrackInfo.Codec == VO_AUDIO_CodingADPCM )
			{
				VO_U32 nRc = 0;

				nRc = OnGetWaveFormatEx(i , &TrackInfo.HeadData);

				if (nRc == VO_RET_SOURCE2_OK)
				{
					VO_WAVEFORMATEX * pWaveFormat = (VO_WAVEFORMATEX *)TrackInfo.HeadData;
					TrackInfo.HeadSize = VO_WAVEFORMATEX_STRUCTLEN + pWaveFormat->cbSize;
				}

			}
		}
		else if( TrackInfo.Type == VO_SOURCE_TT_VIDEO )
		{
			if (m_uVideoTrack == 0xffffffff)
			{
				m_uVideoTrack = i;
				VOLOGI("CreateStreamInfo---m_uVideoTrack=%ld",i);
			}
			
			OnGetVideoFormat(i , &VideoFormat);

			if( TrackInfo.Codec == VO_VIDEO_CodingH264 )
			{
				if(FOURCC_AVC1 == uFourCC)
				{
					m_uFrameSize = uMaxSampleSize;
					ConvertHeadData( TrackInfo.HeadData , TrackInfo.HeadSize );
					TrackInfo.HeadData = m_pHeadData;
					TrackInfo.HeadSize = m_uHeadSize;
				}
			}
			else if(TrackInfo.Codec == VO_VIDEO_CodingH265)
			{
				if(FOURCC_HVC1 == uFourCC)
				{
					m_uFrameSize = uMaxSampleSize;
#ifndef  PARSER_H265
					HEVC_ConvertHeadData(TrackInfo.HeadData , TrackInfo.HeadSize );
					TrackInfo.HeadData = m_pHeadData;
					TrackInfo.HeadSize = m_uHeadSize;
#ifdef H265_DUMP
					if(fp)
					{
						fwrite(TrackInfo.HeadData,1,TrackInfo.HeadSize,fp);
					}
					else
					{
						fp = fopen(h265_dump_file,"wb");
						fwrite(TrackInfo.HeadData,1,TrackInfo.HeadSize,fp);
					}
#endif
#endif

				}
			}
			else if( TrackInfo.Codec == VO_VIDEO_CodingWMV || TrackInfo.Codec == VO_VIDEO_CodingVC1 )
			{
				if(FOURCC_WVC1 != uFourCC)	// if WVC1 and QCM hardware decoder, use sequence data directly, East 20110408
				{
					VO_U32 ret = OnGetVideoBitMapInfoHead(i , &TrackInfo.HeadData);
					if (ret == VO_RET_SOURCE2_OK)
					{
						VO_BITMAPINFOHEADER * pBmpInfo = (VO_BITMAPINFOHEADER *)TrackInfo.HeadData;
						TrackInfo.HeadSize = pBmpInfo->biSize;
					}
				}
			}
			else if (TrackInfo.Codec == VO_VIDEO_CodingVP6)
			{
				OnGetVideoUpSideDown(i , &bUpSideDown);
			}
		}
		else if(TrackInfo.Type == VO_SOURCE_TT_SUBTITLE)
		{
			if ( m_uSubtitleTrack == 0xffffffff )
			{
				m_uSubtitleTrack = i;
				VOLOGI("CreateStreamInfo---m_uSubtitleTrack=%ld",i);
			}
			
//			OnGetSubtitleFormat(i , &SubtitleFormat);
		}


		if( ptr_sel )
		{
			(*ptr_sel) = new VO_SOURCE2_TRACK_INFO;
			memset( (*ptr_sel) , 0 , sizeof( VO_SOURCE2_TRACK_INFO ) );
			*(VO_U32 *)((*ptr_sel)->strFourCC) = uFourCC;
			if (i == m_uAudioTrack || i == m_uVideoTrack || i == m_uSubtitleTrack)
			{
				VOLOGE("%d be selected",i);
				(*ptr_sel)->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
			}
			else
			{
				VOLOGE("%d be selectable",i);
				(*ptr_sel)->uSelInfo = VO_SOURCE2_SELECT_SELECTABLE;
			}
			(*ptr_sel)->uTrackID				= i;
			switch (TrackInfo.Type)
			{
			case VO_SOURCE_TT_VIDEO:
				{
					(*ptr_sel)->uTrackType = VO_SOURCE2_TT_VIDEO;
				}
				break;
			case VO_SOURCE_TT_AUDIO:
				{
					(*ptr_sel)->uTrackType = VO_SOURCE2_TT_AUDIO;
				}
				break;
			case VO_SOURCE_TT_IMAGE:
				{
					(*ptr_sel)->uTrackType = VO_SOURCE2_TT_IMAGE;
				}
				break;
			case VO_SOURCE_TT_STRAM:
				{
					(*ptr_sel)->uTrackType = VO_SOURCE2_TT_STREAM;
				}
				break;
			case VO_SOURCE_TT_SCRIPT:
				{
					(*ptr_sel)->uTrackType = VO_SOURCE2_TT_SCRIPT;
				}
				break;
			case VO_SOURCE_TT_HINT:
				{
					(*ptr_sel)->uTrackType = VO_SOURCE2_TT_HINT;
				}
				break;
			case VO_SOURCE_TT_RTSP_VIDEO:
				{
					(*ptr_sel)->uTrackType = VO_SOURCE2_TT_RTSP_VIDEO;
				}
				break;
			case VO_SOURCE_TT_RTSP_AUDIO:
				{
					(*ptr_sel)->uTrackType = VO_SOURCE2_TT_RTSP_AUDIO;
				}
				break;
			case VO_SOURCE_TT_SUBTITLE:
				{
					(*ptr_sel)->uTrackType = VO_SOURCE2_TT_SUBTITLE;
				}
				break;
			case VO_SOURCE_TT_TELETEXT:
				{
					(*ptr_sel)->uTrackType = VO_SOURCE2_TT_MAX;
				}
				break;
			case VO_SOURCE_TT_RICHMEDIA:
				{
					(*ptr_sel)->uTrackType = VO_SOURCE2_TT_RICHMEDIA;
				}
				break;
			default:
				{
					(*ptr_sel)->uTrackType = VO_SOURCE2_TT_MAX;
				}
				break;
			}
			(*ptr_sel)->uCodec					= TrackInfo.Codec;
			(*ptr_sel)->uDuration				= TrackInfo.Duration;
			(*ptr_sel)->uHeadSize				= TrackInfo.HeadSize;
			(*ptr_sel)->uChunkCounts			= 1;
			if (bUpSideDown)
				(*ptr_sel)->sVideoInfo.uAngle	= VO_SOURCE2_VIDEO_ANGLE_UPSIDEDOWN;
			
			if( (*ptr_sel)->uHeadSize )
			{
				(*ptr_sel)->pHeadData = new VO_BYTE[ (*ptr_sel)->uHeadSize ];
				memcpy( (*ptr_sel)->pHeadData , TrackInfo.HeadData , TrackInfo.HeadSize );
			}
			VO_CHAR *pLang = NULL;
			OnGetLanguage(i , &pLang);
			if (TrackInfo.Type == VO_SOURCE_TT_AUDIO)
			{
				uAudioCnt++;
				
				if (pLang && strlen(pLang) > 0)
				{
					strcpy((*ptr_sel)->sAudioInfo.chLanguage,pLang);
				}
				else
				{
					memcpy((*ptr_sel)->sAudioInfo.chLanguage,"Audio",5);
					(*ptr_sel)->sAudioInfo.chLanguage[5] = 0x30 + uAudioCnt;
				}
				(*ptr_sel)->sAudioInfo.sFormat = AudioFormat;
				
				if (TrackInfo.Codec == VO_AUDIO_CodingAAC && (*(VO_U32 *)((*ptr_sel)->strFourCC) == 0XA106 || *(VO_U32 *)((*ptr_sel)->strFourCC) == 0XFF))
				{
					if (uMaxSampleSize > m_nAudioSize)
					{
						m_pAudioData = new VO_BYTE[uMaxSampleSize * 2];
						m_nAudioSize = uMaxSampleSize;
					}
				}
			}
			else if (TrackInfo.Type == VO_SOURCE_TT_VIDEO)
			{
				(*ptr_sel)->sVideoInfo.sFormat = VideoFormat;
			}
			else if(TrackInfo.Type == VO_SOURCE_TT_SUBTITLE)
			{
				uSubtitleCnt++;
				
				if (pLang && strlen(pLang) > 0)
				{
					strcpy((*ptr_sel)->sSubtitleInfo.chLanguage,pLang);
				}
				else
				{
					memcpy((*ptr_sel)->sSubtitleInfo.chLanguage,"Subtitle",8);
					(*ptr_sel)->sSubtitleInfo.chLanguage[8] = 0x30 + uSubtitleCnt;
				}
			
//				(*ptr_sel)->sSubtitleInfo.sFormat = SubtitleFormat;

			}
		}
	}
	if (m_uAudioTrack != 0xffffffff)
	{
		m_pAudioTrackInfo = m_pStreamInfo->ppTrackInfo[m_uAudioTrack];
	}
	if (m_uVideoTrack != 0xffffffff)
	{
		m_pVideoTrackInfo = m_pStreamInfo->ppTrackInfo[m_uVideoTrack];
	}

	if (m_uSubtitleTrack != 0xffffffff)
	{
		m_pSubtitleTrackInfo = m_pStreamInfo->ppTrackInfo[m_uSubtitleTrack];
	}

	PrintProgramInfo(m_pProgramInfo);
	

}


VO_U32 voSourceBaseWrapper::NotifyEvent( VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2 )
{
	if( m_ptr_EventCallback )
		return m_ptr_EventCallback->SendEvent( m_ptr_EventCallback->pUserData , nID , nParam1 , nParam2 );
	else
	{
		VOLOGE( "NotifyEvent---Empty event callback!" );
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
}

VO_BOOL voSourceBaseWrapper::IsADTSBuffer(VO_PBYTE pAdtsBuf, VO_U32 buffersize)
{
	VO_U32 framesize = 0;

	if(!pAdtsBuf){
		return VO_FALSE;
	}

	if(buffersize < 7){
		return VO_FALSE;
	}

	if(0xFF == pAdtsBuf[0] && (pAdtsBuf[1] & 0xF0)){
		framesize = ((pAdtsBuf[3] & 0x03) << 11) | (pAdtsBuf[4] << 3) | (pAdtsBuf[5] >> 5);
		return (framesize == buffersize)?VO_TRUE:VO_FALSE;
	}

	return VO_FALSE;
}

VO_U32 voSourceBaseWrapper::SetCurrentTimeStamp(VO_U64 Timestamp, VO_SOURCE2_TRACK_TYPE nOutPutType)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;

	if(VO_SOURCE2_TT_AUDIO == nOutPutType){
		m_nCurrentTimeStamp = Timestamp;
	}
	else if(VO_SOURCE2_TT_VIDEO == nOutPutType && (VO_RET_SOURCE2_OK != GetCurTrackInfo(VO_SOURCE2_TT_AUDIO , &pTrackInfo))){
		m_nCurrentTimeStamp = Timestamp;
	}else{
		nResult = VO_RET_SOURCE2_FAIL;
	}

	return nResult;
}

VO_U32 voSourceBaseWrapper::GetCurrentTimeStamp(VO_U64* pTimestamp)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	if(!pTimestamp){
		return VO_RET_SOURCE2_FAIL;
	}

	*pTimestamp = m_nCurrentTimeStamp;
	return nResult;
}

VO_VOID voSourceBaseWrapper::PrintTrackInfo(VO_SOURCE2_TRACK_INFO *pTrackInfo)
{
	VOLOGI("****************************PrintTrackInfo*********************************************");

	if(!pTrackInfo){
		return;
	}

	VOLOGI("uOutSideTrackID=%lu, Sel=%lu, TrackType=%lu, Codec=%lu",
		pTrackInfo->uTrackID, pTrackInfo->uSelInfo, pTrackInfo->uTrackType, pTrackInfo->uCodec);
	
	VOLOGI("Duration=%llu, ChunkCounts=%lu, Bitrate=%lu, HeadSize=%lu",
		pTrackInfo->uDuration, pTrackInfo->uChunkCounts, pTrackInfo->uBitrate, pTrackInfo->uHeadSize);

	VOLOGI("*****************************************************************************************");

}

VO_VOID voSourceBaseWrapper::PrintStreamInfo(VO_SOURCE2_STREAM_INFO *pStreamInfo)
{
	VOLOGI("****************************PrintStreamInfo*********************************************");

	if(!pStreamInfo){
		return;
	}
	VOLOGI("StreamID=%lu, Sel=%lu, uBitrate=%lu, uTrackCount=%lu",
		pStreamInfo->uStreamID, pStreamInfo->uSelInfo, pStreamInfo->uBitrate, pStreamInfo->uTrackCount);

	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		PrintTrackInfo(pStreamInfo->ppTrackInfo[index]);
	}

	VOLOGI("*******************************************************************************************");
	
}

VO_VOID voSourceBaseWrapper::PrintProgramInfo(VO_SOURCE2_PROGRAM_INFO *pProgramInfo)
{
	VOLOGI("****************************PrintProgramInfo*********************************************");

	if(!pProgramInfo){
		return;
	}

	VOLOGI("ProgramID=%lu, Sel=%lu, Type=%lu, Name=%s,uStreamCount=%lu",
		pProgramInfo->uProgramID, pProgramInfo->uSelInfo, pProgramInfo->sProgramType, pProgramInfo->strProgramName, pProgramInfo->uStreamCount);

	for(VO_U32 index = 0; index < pProgramInfo->uStreamCount; index++)
	{
		PrintStreamInfo(pProgramInfo->ppStreamInfo[index]);
	}


	VOLOGI("*********************************************************************************************");
}

