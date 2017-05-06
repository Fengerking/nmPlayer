#ifdef WINCE
#ifdef _WIN32
#undef _WIN32	//tchar.h has redefinition
#endif // _WIN32
#endif//WINCE
#include "voSource.h"
#include "ASFHeadParser.h"
#include "utility.h"
#include "MediaStream.h"
#include "RTSPClientEngine.h"
#include "RTSPSource.h"
#include "RTSPTrack.h"
#include "fCC.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CRTSPTrack::CRTSPTrack(CRTSPSourceEX * pRTSPSource, CRTSPClientEngine * pRTSPClientEngine)
: m_bEndOfStream(false)
, m_pTrackName(NULL)
, m_dwTrackStartTime(0)
, m_dwTrackCurTime(0)
, m_IsNeedIFrame(false)
, m_pRTSPSource(pRTSPSource)
, m_pRTSPClientEngine(pRTSPClientEngine)
, m_pMediaStream(NULL)
, m_nearEnd(false)
, m_tryCount(0)
, m_livePauseRefTime(0)
, m_innerPosTime(0)
{
	m_livePausePos = 0;
	dump = 0;
	m_delayTime = 0;
	m_frameNum	= 0;
}
void	CRTSPTrack::ResetTrack()
{
	m_pMediaStream=NULL;
	m_tryCount=0;
	m_bEndOfStream=false;
	m_nearEnd = false;
	m_livePausePos=m_frameNum=m_delayTime=m_dwTrackStartTime=m_dwTrackCurTime=0;
	m_IsNeedIFrame=false;
	m_frames=0;
};
CRTSPTrack::~CRTSPTrack(void)
{
	SAFE_DELETE_ARRAY(m_pTrackName);
}

void CRTSPTrack::SetEndOfStream(bool isEnd)
{
	if(isEnd&&!m_bEndOfStream)
	{
		sprintf(CLog::formatString,"after try time,EndofStream,isAudio=%d\n",m_pMediaStream->IsAudioStream());
		CLog::Log.MakeLog(LL_RTSP_ERR,"buffer.txt",CLog::formatString);
	}
	m_bEndOfStream=isEnd;
};
VOFILEREADRETURNCODE CRTSPTrack::SetParameter(LONG lID, LONG lValue)
{
	return VORC_FILEREAD_NOT_IMPLEMENT;
}

VOFILEREADRETURNCODE CRTSPTrack::GetParameter(LONG lID, LONG * plValue)
{
	if(m_pMediaStream == NULL)
		return VORC_FILEREAD_UNKNOWN_FAILED;

	switch(lID)
	{
	case VO_PID_AUDIO_FORMAT:
		{
			if(m_pMediaStream->IsVideoStream()) 
				break;

			VO_AUDIO_FORMAT * pcaf = (VO_AUDIO_FORMAT *)plValue;
			pcaf->Channels = m_pMediaStream->AudioChannels();
			pcaf->SampleRate= m_pMediaStream->AudioSampleRate();
			pcaf->SampleBits = m_pMediaStream->AudioBits();
			return VORC_FILEREAD_OK;
		}
	case VOID_COM_AUDIO_FORMAT:
		{
			if(m_pMediaStream->IsVideoStream()) 
				break;
			
			VOCODECAUDIOFORMAT * pcaf = (VOCODECAUDIOFORMAT *)plValue;
			pcaf->channels = m_pMediaStream->AudioChannels();
			pcaf->sample_rate = m_pMediaStream->AudioSampleRate();
			pcaf->sample_bits = m_pMediaStream->AudioBits();
			return VORC_FILEREAD_OK;
		}
	case VOID_COM_AUDIO_CHANNELS:
		{
			if(m_pMediaStream->IsVideoStream()) 
				break;

			*((int *)plValue) = m_pMediaStream->AudioChannels();
			return VORC_FILEREAD_OK;
		}
	case VOID_COM_AUDIO_SAMPLERATE:
		{
			if(m_pMediaStream->IsVideoStream()) 
				break;
			
			*((int *)plValue) = m_pMediaStream->AudioSampleRate();
			return VORC_FILEREAD_OK;
		}
	case VOID_COM_VIDEO_FORMAT:
		{
			if(m_pMediaStream->IsAudioStream()) 
				break;
	
			VOCODECVIDEOFORMAT * pcvf = (VOCODECVIDEOFORMAT *)plValue;
			pcvf->width = m_pMediaStream->VideoWidth();
			pcvf->height = m_pMediaStream->VideoHeight();
			return VORC_FILEREAD_OK;
		}
	case VO_PID_VIDEO_FORMAT	:
		{
			if(m_pMediaStream->IsAudioStream()) 
				break;
			VO_VIDEO_FORMAT * pcvf = (VO_VIDEO_FORMAT *)plValue;
			pcvf->Width = m_pMediaStream->VideoWidth();
			pcvf->Height = m_pMediaStream->VideoHeight();
			return VORC_FILEREAD_OK;
		}
	case VOID_COM_VIDEO_WIDTH:
		{
			if(m_pMediaStream->IsAudioStream()) 
				break;

			*((int *)plValue) = m_pMediaStream->VideoWidth();
			return VORC_FILEREAD_OK;
		}
	case VOID_COM_VIDEO_HEIGHT:
		{
			if(m_pMediaStream->IsAudioStream()) 
				break;

			*((int *)plValue) = m_pMediaStream->VideoHeight();
			return VORC_FILEREAD_OK;
		}
	case VOID_INNER_BUFFERING_TIME:
		{
			if(m_pMediaStream)
				*plValue=m_pMediaStream->HaveBufferedMediaStreamTime();
			else
				*plValue=0;
			return VORC_FILEREAD_OK; 
		}
		break;
	case VOID_COM_DURATION:
		{
			float fDuration = m_pMediaStream->MediaStreamEndTime();
			if(fDuration == 0)
				fDuration = m_pRTSPClientEngine->StreamingDuration();

			*((__int64 *)plValue) = (__int64)(fDuration * 10000000);
			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_CODEC:
		{
			*((VOAVCODEC *)plValue) = GetCodec();
			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_CODEC_NAME:
	
		{
			
			*((VOFOURCC *)plValue) = GetCodecName();
			return VORC_FILEREAD_OK;
		}
	case VO_PID_SOURCE_CODECCC:
		{
			if(m_pMediaStream->IsVideoStream())
			{
				*((VOFOURCC *)plValue) = GetCodecName();
				return VORC_FILEREAD_OK;
			}
			//the following is audio
			switch (GetCodec())
			{
			case AC_AAC:
				*plValue = AudioFlag_ISOMPEG4_AAC;
				break;
			case AC_AMR:
				*plValue = AudioFlag_AMR_NB;
				break;
			case AC_AWB:
				*plValue = AudioFlag_AMR_WB;
				break;
			case AC_AWBP:
				*plValue = AudioFlag_AMR_WB_PLUS;
				break;
			case AC_QCELP:
				*plValue = AudioFlag_QCELP;
				break;
			case AC_WMA:
				*plValue = AudioFlag_AMR_NB;
				break;
			case AC_RA:
				*plValue = AudioFlag_RA_G2;
				break;
			default:
				*plValue = AudioFlag_RA_G2;
				break;
			}
			return VORC_FILEREAD_OK;
		}
	case VOID_COM_HEAD_DATA:
		{
			VOCODECDATABUFFER * pHeadData = (VOCODECDATABUFFER *)plValue;
			pHeadData->length = m_pMediaStream->GetCodecHeadData(pHeadData->buffer);
			CLog::EnablePrintTime(false);
			
			if((pHeadData->buffer) && (CUserOptions::UserOptions.m_bMakeLog>LOG_DATA))
			{
				if(GetMediaStream()->IsAudioStream())
				{
					CLog::Log.MakeLog(LL_SOCKET_ERR,"AudioHeadData.dat", pHeadData->buffer, pHeadData->length);
				}
				else
				{
					CLog::Log.MakeLog(LL_SOCKET_ERR,"VideoHeadData.dat", pHeadData->buffer, pHeadData->length);
				}
			}
			
			
			return VORC_FILEREAD_OK;
		}
	case VO_PID_SOURCE_BITMAPINFOHEADER:
		{
			CASFHeadParser* parser= m_pRTSPClientEngine->GetASFHeadParser();
			if (parser)
			{
				VOWMV9DECHEADER* head=parser->GetAsfVideoMediaType();
				if(head)
				{
					unsigned char** bitMapHead=(unsigned char**)plValue;
					*bitMapHead=head->pRawBitmapHead;
					return VORC_FILEREAD_OK;
				}
			}
			break;
		}
	case VO_PID_SOURCE_WAVEFORMATEX:
		{
			CASFHeadParser* parser= m_pRTSPClientEngine->GetASFHeadParser();
			if (parser)
			{
				WMAHeaderInfo* head=parser->GetAsfAudioMediaType();
				if(head)
				{
					unsigned char** wavForamt=(unsigned char**)plValue;
					*wavForamt=head->pRawWAVFormat;
					return VORC_FILEREAD_OK;
				}
			}
			break;
		}
	default:
		break;
	}

	return VORC_FILEREAD_NOT_IMPLEMENT;
}

VOFILEREADRETURNCODE CRTSPTrack::GetInfo(VOTRACKINFO * pTrackInfo)
{
	if(m_pMediaStream == NULL)
		return VORC_FILEREAD_UNKNOWN_FAILED;

	if(m_pMediaStream->IsVideoStream())
		pTrackInfo->uType = TT_RTSP_VIDEO;
	else if(m_pMediaStream->IsAudioStream())
		pTrackInfo->uType = TT_RTSP_AUDIO;
#ifdef DIMS_DATA
	else if(m_pMediaStream->IsDataStream())
		pTrackInfo->uType = TT_RTSP_DATA;
#endif//DIMS_DATA

	float fDuration = m_pMediaStream->MediaStreamEndTime();
	if(fDuration == 0)
		fDuration = m_pRTSPClientEngine->StreamingDuration();

	int duration2 = fDuration*1000;
	pTrackInfo->uDuration = duration2;

	pTrackInfo->uMaxSampleSize = GetMaxFrameSize();
	pTrackInfo->uSampleCount = 0;
	pTrackInfo->uCodec = GetCodec();
	return VORC_FILEREAD_OK;
}
void		CRTSPTrack::  SetLivePauseRefTime(int time)
{
	m_livePauseRefTime = time-GetTrackPlayPos();
	if(m_livePauseRefTime<0||m_livePauseRefTime>1000*1000)
		m_livePauseRefTime = 0;
	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{
		sprintf(CLog::formatString,"SetLivePauseRefTime %d\n",m_livePauseRefTime);
		if(m_pMediaStream->IsVideoStream())
			CLog::Log.MakeLog(LL_VIDEO_SAMPLE,"videoBuffer.txt",CLog::formatString);
		else if (m_pMediaStream->IsAudioStream())
			CLog::Log.MakeLog(LL_AUDIO_SAMPLE,"audioBuffer.txt",CLog::formatString);
	}
}
DWORD	CRTSPTrack::GetLastTimeInBuffer()
{
	//m_dwTrackStartTime+m_innerPosTime=old_m_dwTrackStartTime
	return m_pMediaStream->GetLastTimeInBuffer()+m_dwTrackStartTime+m_innerPosTime;
}
VOFILEREADRETURNCODE CRTSPTrack::GetSampleByIndex(VOSAMPLEINFO * pSampleInfo)
{
	if(m_pMediaStream == NULL||m_bEndOfStream)
		return VORC_FILEREAD_INDEX_END;
	VOFILEREADRETURNCODE rc;
	{

		rc = m_pMediaStream->GetMediaFrameByIndex(pSampleInfo);
				
		if(rc == VORC_FILEREAD_OK&&pSampleInfo->pBuffer)
		{
			m_dwTrackCurTime = pSampleInfo->uTime;
			m_frames++;
		}
	}
	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{
		dump++;
		if(rc == VORC_FILEREAD_OK&&pSampleInfo->pBuffer)//&&(dump&31)==0)
		{ 
			int refTime=0;
			NotifyEvent(HS_EVENT_GET_REFCLOCK,(int)(&refTime));
			sprintf(CLog::formatString,"%ld:I=%ld,%ld(%d+%ld=(%ld+%d)),p=%ld,ref=%d,r=%d,s=%ld\n",
				pSampleInfo->uIndex,pSampleInfo->uSize>>31,pSampleInfo->uTime,
				m_livePauseRefTime,m_dwTrackCurTime+m_innerPosTime,m_dwTrackCurTime,
				m_innerPosTime,GetTrackPlayPos(),refTime,
				m_pMediaStream->HaveBufferedMediaFrameCount(),pSampleInfo->uSize&0x7fffffff);
			if(m_pMediaStream->IsVideoStream())
				CLog::Log.MakeLog(LL_VIDEO_SAMPLE,"videoBuffer.txt",CLog::formatString);
			else if (m_pMediaStream->IsAudioStream())
				CLog::Log.MakeLog(LL_AUDIO_SAMPLE,"audioBuffer.txt",CLog::formatString);
#ifdef DIMS_DATA
			else if (m_pMediaStream->IsDataStream())
				CLog::Log.MakeLog(LL_VIDEO_TIME,"dimsBuffer.txt",CLog::formatString);
#endif//DIMS_DATA
			dump = 0;
		}
		else if(pSampleInfo->uSize==1)//Get Key frame fails
		{
			sprintf(CLog::formatString,"%ld:@@@@fail to get I frame\n",
				pSampleInfo->uIndex);
			if(m_pMediaStream->IsVideoStream())
				CLog::Log.MakeLog(LL_VIDEO_SAMPLE,"videoBuffer.txt",CLog::formatString);
			else if (m_pMediaStream->IsAudioStream())
				CLog::Log.MakeLog(LL_VIDEO_SAMPLE,"audioBuffer.txt",CLog::formatString);
		}
		if(rc == VORC_FILEREAD_INDEX_END)
		{
			sprintf(CLog::formatString,"EXIT:%s:currPlayTime=%ld,m_nearEnd=%d\n",m_pMediaStream->CodecName(),GetTrackPlayPos(), m_nearEnd);
			CLog::Log.MakeLog(LL_RTSP_ERR,"buffer.txt",CLog::formatString);
		
		}
	}
	return rc;
}

/*
VOFILEREADRETURNCODE CRTSPTrack::GetSampleByTime(VOSAMPLEINFO * pSampleInfo)
{
	return VORC_FILEREAD_NOT_IMPLEMENT;
}

bool CRTSPTrack::IsKeyFrame(int nIndex)
{
	return VORC_FILEREAD_NOT_IMPLEMENT;
}
*/

int CRTSPTrack::GetNextKeyFrame(int nIndex, int nDirectionFlag)
{
	if(m_pMediaStream == NULL)
		return 0;

	return m_pMediaStream->GetNextKeyFrame(nIndex, nDirectionFlag);
}

bool CRTSPTrack::SetPosition(const DWORD dwStart,int flushFlag)
{
	if(m_pMediaStream == NULL)
		return false;
	//store currpos as previous pos
	int currPos = GetTrackPlayPos();
	if(currPos<0)
		currPos = 0;
	int preStart=m_dwTrackStartTime;
	m_pMediaStream->StoreStreamPreviousPos(preStart);

	m_dwTrackStartTime = dwStart;//get the integer part because the Play(time),the time is also send as a tructated integer
	m_dwTrackCurTime = m_dwTrackStartTime;
	m_delayTime = 0;
	m_livePauseRefTime=0;
	m_pMediaStream->SetStreamStartTime(dwStart);
	if(flushFlag)
	{
		m_pMediaStream->FlushMediaStream();
		m_innerPosTime=0;
	}
	else
	{
		m_innerPosTime=preStart-dwStart;
	}
	if(m_dwTrackStartTime+200>GetTrackDuration())//it is end
		return false;
	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{
		//sprintf(CLog::formatString,""
	}
	return true;
}

unsigned long CRTSPTrack::GetMaxFrameSize()
{
	if(m_pMediaStream == NULL)
		return 0;

	if(m_pMediaStream->IsVideoStream())
		return 50*1024;
	else if(m_pMediaStream->IsAudioStream())
		return 10*1024;
#ifdef DIMS_DATA
	else if(m_pMediaStream->IsDataStream())
		return 16*1024;
#endif//DIMS_DATA
	return 0;
}

VOAVCODEC CRTSPTrack::GetCodec()
{
	if(m_pMediaStream == NULL)
		return VC_UNKNOWN;

	VOAVCODEC Codec = VC_UNKNOWN;
	if(m_pMediaStream->IsVideoStream())
	{
		if(m_pMediaStream->IsVideoMpeg4())
			Codec = VC_MPEG4;
		else if(m_pMediaStream->IsVideoH264())
			Codec = VC_H264;
		else if(m_pMediaStream->IsVideoH263())
			Codec = VC_H263;
		else if(m_pMediaStream->IsVideoRMVideo())
			Codec = VC_RV;//TODO:REAL VC_REALVIDEO
		else if(m_pMediaStream->IsVideoWMV())
			Codec = VC_WMV;
	}
	else if(m_pMediaStream->IsAudioStream())
	{
		if(m_pMediaStream->IsAudioAAC())
			Codec = AC_AAC;
		else if(m_pMediaStream->IsAudioAMRNB())
			Codec = AC_AMR;
		else if(m_pMediaStream->IsAudioAMRWB())
			Codec = AC_AWB;
		else if(m_pMediaStream->IsAudioAMRWBP())
			Codec = AC_AWBP;
		else if(m_pMediaStream->IsAudioMP3())//
			Codec = AC_MP3;
		else if(m_pMediaStream->IsAudioQCELP())
			Codec = AC_QCELP;
		else if(m_pMediaStream->IsAudioRMAudio())
			Codec = AC_RA;//TODO:REAL AC_REALAUDIO
		else if(m_pMediaStream->IsAudioWMA())
			Codec = AC_WMA;
		else if(m_pMediaStream->IsAudioG711())
			Codec = AC_ADPCM;
	}
	return Codec;
}

VOFOURCC CRTSPTrack::GetCodecName()
{
	if(m_pMediaStream == NULL)
		return 0x00;

	switch(GetCodec())
	{
	case VC_MPEG4:
		return FCC("MP4V");
	case VC_H264:
#ifndef H264_ANNEXB_FORMAT
		return 0x31435641;//FCC('AVC1');
#else
		return FCC("H264");
#endif
	case VC_H263:
		return FCC("H263");
	case VC_RV:
	case VC_WMV:
		return m_pMediaStream->GetCodecFCC();
	case AC_AAC:
		return 0xA106;//0xFF is ADTS
	case AC_AMR:
		return 0xFE;
	case AC_AWB:
		return 0x62776173;
	case AC_AWBP:
		return 0x62776173;
	case AC_MP3:
		return 0x55;
	case AC_QCELP:
		return 0x4537649a;
	case AC_RA:
	case AC_WMA:
		return m_pMediaStream->GetCodecFCC();
	default:
		return 0x00;
	}
}

DWORD CRTSPTrack::GetTrackDuration()
{
	DWORD dwTrackDuration = m_pMediaStream->MediaStreamEndTime() * 1000;
	if(dwTrackDuration == 0)
	{
		dwTrackDuration = m_pRTSPClientEngine->StreamingDuration() * 1000;
	}
	return dwTrackDuration;
}

void CRTSPTrack::  SetDelayTime(int time)
{
	m_delayTime = time;
	//m_dwTrackStartTime+=time;
	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{
		sprintf(CLog::formatString,"!!!!delay=%d,cur=%ld,setpos=%ld,r=%d\n",m_delayTime,m_dwTrackCurTime,m_dwTrackStartTime,m_pMediaStream->HaveBufferedMediaFrameCount());
		if(m_pMediaStream->IsVideoStream())
			CLog::Log.MakeLog(LL_VIDEO_SAMPLE,"videoBuffer.txt",CLog::formatString);
		else
			CLog::Log.MakeLog(LL_AUDIO_SAMPLE,"audioBuffer.txt",CLog::formatString);
	}
}
DWORD CRTSPTrack::GetTrackPlayPos() 
{
	//sprintf(CLog::formatString,"m_dwTrackCurTime=%ld\n",m_dwTrackCurTime);
	//CLog::Log.MakeLog(LL_RTSP_ERR,"pos.txt",CLog::formatString);
	return m_dwTrackCurTime;
}
int	CRTSPTrack::SetDiffTimeInPlayResp(float dwStart)
{
	if((m_dwTrackStartTime != dwStart) && (dwStart<GetTrackDuration()))
	{
		sprintf(CLog::formatString,"m_dwTrackStartTime=%ld,dwStart=%f\n",m_dwTrackStartTime,dwStart);
		CLog::Log.MakeLog(LL_RTSP_ERR,"buffering.txt",CLog::formatString);
		SetPosition(dwStart,0);
	}
	return 0;
}
void CRTSPTrack::SetTrackLivePausePos(int pos)
{
	m_dwTrackStartTime	= 0;//(dwStart/1000)*1000;//get the integer part because the Play(time),the time is also send as a tructated integer
	m_dwTrackCurTime	= 0;
}
bool CRTSPTrack::IsNearEnd(unsigned int nearValue)
{
	unsigned long duration = 0;	
	duration = GetTrackDuration();
	
	if(!duration)
		return false;
	
	long 	pos = GetTrackPlayPos();
	long	antiJitterBufTime = m_pMediaStream->GetAntiJitterBufferTime();
	long	position = pos + antiJitterBufTime;
	unsigned int 	diff = abs(duration - position);	
	
	bool 	isEnd =  diff<nearValue;
	if(isEnd)
		SLOG4(LL_RTSP_ERR,"buffering.txt","end_v=%d:pos=%ld (%ld+%ld)\n",m_pMediaStream->IsVideoStream(),position,pos,antiJitterBufTime);
	return isEnd;
}
bool CRTSPTrack::IsTrackPlayedComplete()
{
	return  (IsNearEnd(CUserOptions::UserOptions.m_nSourceEndTime));
}

void CRTSPTrack::SetMediaStream(CMediaStream * pMediaStream)
{
	m_pMediaStream = pMediaStream; 
	if(m_pMediaStream)
		m_pMediaStream->SetRTSPTrack(this);
}

void CRTSPTrack::SetTrackName(const char * trackName)
{
	SAFE_DELETE_ARRAY(m_pTrackName);

	int len = strlen(trackName) + 1;
	m_pTrackName = new char[len];TRACE_NEW("rtss_m_pTrackName",m_pTrackName);
	if(m_pTrackName == NULL)
		return;

	strcpy(m_pTrackName, trackName);
}
