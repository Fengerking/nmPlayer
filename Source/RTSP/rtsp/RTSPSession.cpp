
#include <string.h>

#include "utility.h"
#include "network.h"
#include "MediaStream.h"
//#include "RDTMediaStream.h"
#include "RTSPSession.h"
#include "RTSPClientEngine.h"
#include "ASFHeadParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

char * GetNextLine(char * text)
{
	do
	{
		if(text == NULL)
			break;

		for(char * p=text; *p != '\0'; ++p)
		{
			if(*p == '\r' || *p == '\n')
			{
				*p++ = '\0';
				if(*p == '\n') 
					*p++ = '\0';

				return p;
			}
		}

	}while(0);

	return NULL;
}

char * StringDup(const char * srcString)
{
	int len = strlen(srcString) + 1;
	char * p = new char[len];TRACE_NEW("rts_p ",p);
	strcpy(p, srcString);
	return p;
}

#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

const char * MediaTypeSupported[] = {
		"Video",
		"Audio"
		//"application"
	};
unsigned long CRTSPSession::m_pausedTime[2];
CRTSPSession::CRTSPSession(CRTSPClientEngine* engine)
: m_dropFrames(0)
, m_sessionAddr(NULL)
, m_sessionBandwidth(0)
, m_sessionEntityTag(NULL)
, m_sessionDuration(0)
, m_asfheadParser(NULL)
, m_applicationStream(NULL)
, m_engine(engine)
{
	memset(m_mediaStreams, 0, sizeof(m_mediaStreams));
	memset(m_sessionTitle,0,MAX_SESSION_TITLE_SIZE);
	m_sessionAddr = new struct sockaddr_storage;TRACE_NEW("rts_m_sessionAddr ",m_sessionAddr);
	m_sessionAddr->ss_family = AF_INET;
	m_mediaStreamCount = 0;
	//m_pausedTime[0]=m_pausedTime[1]=0;
}

CRTSPSession::~CRTSPSession()
{
	SAFE_DELETE(m_sessionAddr);
	SAFE_DELETE_ARRAY(m_sessionEntityTag);
	SAFE_DELETE(m_applicationStream);
	SAFE_DELETE(m_asfheadParser);
	for(int i=0; i<m_mediaStreamCount; ++i)
	{
		SAFE_DELETE(m_mediaStreams[i]);
	}
}
bool CRTSPSession::ResumeLiveStream()
{
	int mediaCount = m_mediaStreamCount;
	if(mediaCount>2)
	{
		mediaCount = 2;
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@warnning:ResumeLiveStream StreamCount is great than 2!!\n");
	}
#define FORA_SECOND 0*100 //
	for(int i=0; i<m_mediaStreamCount; ++i)
	{
		m_mediaStreams[i]->SetPauseTime(m_pausedTime[i]+FORA_SECOND);
		if(CUserOptions::UserOptions.m_bMakeLog)
		{
			char strtime[128];
			sprintf(strtime,"************resumeLive time=%ld\n",m_pausedTime[i]);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", strtime);
			if(m_mediaStreams[i]->IsVideoStream())
				CLog::Log.MakeLog(LL_VIDEO_DATA,"videoTime.txt","\nResumeLiveStream()!!!!!!!!!!!!!!!!!!!!!!\n");
			else
				CLog::Log.MakeLog(LL_VIDEO_DATA,"audioTime.txt","\nResumeLiveStream()!!!!!!!!!!!!!!!!!!!!!!\n");
		}
		
	}
	return true;
}
bool  CRTSPSession::IsNeedRTCPSync()
{
	//one stream or on-demand does not need sync with RTCP
	if(m_mediaStreamCount<2||m_sessionDuration!=0)
		return false;
	
	return true;
}
void CRTSPSession::ResetPauseTime()
{ 
	return;
	m_pausedTime[0]= m_pausedTime[1]=0;
	if(CUserOptions::UserOptions.m_bMakeLog)
		for(int i=0; i<m_mediaStreamCount; ++i)
		{
			
			{
				
				if(m_mediaStreams[i]->IsVideoStream())
					CLog::Log.MakeLog(LL_VIDEO_DATA,"videoTime.txt","\nResetLiveStream()!!!!!!!!!!!!!!!!!!!!!!\n");
				else
					CLog::Log.MakeLog(LL_VIDEO_DATA,"audioTime.txt","\nResetLiveStream()!!!!!!!!!!!!!!!!!!!!!!\n");
			}

		}
};
bool CRTSPSession::PauseLiveStream()
{
	int mediaCount = m_mediaStreamCount;
	if(mediaCount>2)
	{
		mediaCount = 2;
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "@@@@warnning:PauseLiveStream StreamCount is great than 2!!\n");
	}

	for(int i=0; i<m_mediaStreamCount; ++i)
	{
		m_pausedTime[i]=m_mediaStreams[i]->GetPauseTime();
		if(CUserOptions::UserOptions.m_bMakeLog)
		{
			char strtime[128];
			sprintf(strtime,"*****************PauseLive time=%ld\n",m_pausedTime[i]);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", strtime);
		}
	}
	return true;
}
bool CRTSPSession::ParseASFSDP(char * sessionDescription)
{

		char * sdpline = sessionDescription;
		char * nextSDPLine = sessionDescription; 
		while(1)
		{
			sdpline = nextSDPLine;
			if(sdpline == NULL || sdpline[0] == 'm')
				break;

			nextSDPLine = GetNextLine(sdpline);

			if(ParseSDPParam_c(sdpline))
				continue;
			if(ParseSDPParam_s(sdpline))
				continue;
			if(ParseSDPParam_b_AS(sdpline))
				continue;
			if(ParseSDPAttribute_etag(sdpline))
				continue;
			if(ParseSDPAttribute_range(sdpline))
				continue;
			if(ParseSDPParam_bitrate(sdpline))
				continue;
			if(ParserSDPAttribute_ASFHead(sdpline))
				continue;

		}
		if(m_sessionBandwidth)
			CUserOptions::UserOptions.outInfo.clip_bitrate = m_sessionBandwidth;

		CLog::Log.MakeLog(LL_SOCKET_ERR,"flow.txt", "$$$CRTSPSession::Init step1 \r\n");

		int audioNum=0;
		int videoNum=0;
		while(sdpline != NULL && sdpline[0] == 'm')
		{
			if(!IsMediaTypeSupported(sdpline))
			{
				sdpline = GetNextMediaType(sdpline);
				continue;
			}
			if(m_mediaStreamCount>1)//Sometimes,the server will send an extra media,do not process it 
			{
				break;
			}
			if(_strnicmp(sdpline, "m=video",7) == 0&&videoNum==1)
			{
				sdpline = GetNextMediaType(sdpline);
				continue;
			}
			if(_strnicmp(sdpline, "m=audio",7) == 0&&audioNum==1)
			{
				sdpline = GetNextMediaType(sdpline);
				continue;
			}
			
			CMediaStream* mediaStream=m_engine->CreateOneMediaStream(this);TRACE_NEW("session:mediaStream",mediaStream);
			if(!mediaStream->Init(sdpline))
			{
				if(mediaStream->IsVideoStream())
					CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt", "@@@@@@The video codec init fail! \r\n");
				else if(mediaStream->IsAudioStream())
					CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt", "@@@@@@The audio codec init fail! \r\n");
#ifdef DIMS_DATA
				else if(mediaStream->IsDataStream())
					CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt", "@@@@@@The DIMS data codec init fail! \r\n");
#endif//DIMS_DATA
				else if(mediaStream->IsApplicationStream())
					CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt", "@@@@@@The application media init fail! \r\n");
				else
					CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt", "@@@@@@The media is not audio or video or application! \r\n");

				SAFE_DELETE(mediaStream);
				mediaStream=NULL;
				continue;
			}
			
			
			{
				if(mediaStream->IsAudioStream())
					audioNum++;
				else
					videoNum++;
				
				m_mediaStreams[m_mediaStreamCount++] = mediaStream;
				if(m_sessionBandwidth==0)
					CUserOptions::UserOptions.outInfo.clip_bitrate += mediaStream->GetB_AS();
				

			}
			
		}
		if(m_asfheadParser)
		{
			for(int i=0;i<m_mediaStreamCount;i++)
			{
				if(m_mediaStreams[i]->IsVideoStream())
				{
					VOWMV9DECHEADER* head=m_asfheadParser->GetAsfVideoMediaType();
					if(head)
					{
						//int size=sizeof(VOWMV9DECHEADER)+head->iExtSize;
						//
						//memcpy(codecHead,head,sizeof(VOWMV9DECHEADER));
						if(head->iExtSize>0)
						{
							unsigned char* codecHead = (unsigned char*)(m_mediaStreams[i]->GetCodecHeadBuffer(Len_Video_Format+head->iExtSize));
							memcpy(codecHead,head->pRawBitmapHead,head->iExtSize+Len_Video_Format);
						}
						m_mediaStreams[i]->SetCodecFCC(head->iCodecVer);
						m_mediaStreams[i]->SetVideoHW(head->iPicHorizSize,head->iPicVertSize);

					}
					else
					{
						CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","there is no wmv codec head\n");
						return false;
					}

				}
				else if(m_mediaStreams[i]->IsAudioStream())  
				{
					WMAHeaderInfo* head=m_asfheadParser->GetAsfAudioMediaType();
					if(head)
					{
						WMAHeaderInfo* codecHead=(WMAHeaderInfo*)m_mediaStreams[i]->GetCodecHeadBuffer(Len_Audio_Media_Type+head->iExtSize);
						memcpy(codecHead,head->pRawWAVFormat,(Len_Audio_Media_Type+head->iExtSize));
						
						m_mediaStreams[i]->SetCodecFCC(head->wFormatTag);
						m_mediaStreams[i]->SetAudioFormat(head->nChannels,head->nSamplesPerSec,head->nValidBitsPerSample);
					}
					else
					{
						CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","there is no wma codec head\n");
						return false;
					}

				}
			}
		}
		return m_mediaStreamCount > 0;
	
}

bool CRTSPSession::ParseSDP(char * sessionDescription)
{
	if(CUserOptions::UserOptions.streamType==ST_ASF)
		return ParseASFSDP(sessionDescription);

	char * sdpline = sessionDescription;
	char * nextSDPLine = sessionDescription; 

	while(1)
	{
		sdpline = nextSDPLine;
		if(sdpline == NULL || sdpline[0] == 'm')
			break;

		nextSDPLine = GetNextLine(sdpline);

		if(ParseSDPParam_c(sdpline))
			continue;
		if(ParseSDPParam_s(sdpline))
			continue;
		if(ParseSDPParam_b_AS(sdpline))
			continue;
		if(ParseSDPAttribute_etag(sdpline))
			continue;
		if(ParseSDPAttribute_range(sdpline))
			continue;
		if(ParseSDPParam_bitrate(sdpline))
			continue;
		
	}

	CUserOptions::UserOptions.outInfo.clip_bitrate = 0;
	
#if TRACE_FLOW_SDP
	CLog::Log.MakeLog(LL_CODEC_ERR,"flow.txt", "$$$CRTSPSession::Init step1 \r\n");
#endif//TRACE_FLOW	
	while(sdpline != NULL && sdpline[0] == 'm')
	{
		if(!IsMediaTypeSupported(sdpline))
		{
			sdpline = GetNextMediaType(sdpline);
			continue;
		}
		CMediaStream* mediaStream=m_engine->CreateOneMediaStream(this);TRACE_NEW("session:mediaStream",mediaStream);
		if(!mediaStream->Init(sdpline))
		{
			if(mediaStream->IsVideoStream())
				CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt", "@@@@@@The video codec init fail! \r\n");
			else if(mediaStream->IsAudioStream())
				CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt", "@@@@@@The audio codec init fail! \r\n");
#ifdef DIMS_DATA
			else if(mediaStream->IsDataStream())
				CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt", "@@@@@@The DIMS data codec init fail! \r\n");
#endif//DIMS_DATA
			else if(mediaStream->IsApplicationStream())
				CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt", "@@@@@@The application media init fail! \r\n");
			else
				CLog::Log.MakeLog(LL_CODEC_ERR,"codecErr.txt", "@@@@@@The media is not audio or video or application! \r\n");

			SAFE_DELETE(mediaStream);
			mediaStream=NULL;
			continue;
		}
		
		if(mediaStream->IsAudioStream()||mediaStream->IsVideoStream()
#ifdef DIMS_DATA
			||mediaStream->IsDataStream()
#endif//DIMS_DATA
			)
		{
			m_mediaStreams[m_mediaStreamCount++] = mediaStream;
			//if(m_sessionBandwidth==0)
				CUserOptions::UserOptions.outInfo.clip_bitrate += mediaStream->GetB_AS();
			if(mediaStream->IsVideoStream())
				CUserOptions::UserOptions.outInfo.clip_videoBitRate=mediaStream->GetB_AS();
			else
				CUserOptions::UserOptions.outInfo.clip_audioBitRate=mediaStream->GetB_AS();
			if(m_mediaStreamCount>1)//Sometimes,the server will send an extra media,do not process it 
			{

				break;
			}

		}
		else
		{
			m_applicationStream = mediaStream;
			
		}
	}
	return m_mediaStreamCount > 0;
}
bool CRTSPSession::Init(char * sessionDescription, struct sockaddr_storage * sessionAddr)
{
	
	if(m_sessionAddr == NULL)
	{
		m_sessionAddr = new struct sockaddr_storage;TRACE_NEW("rts_m_sessionAddr ",m_sessionAddr);
	}

	if(sessionAddr != NULL)
	{
		memcpy(m_sessionAddr, sessionAddr, sizeof(sockaddr_storage));
	}
	return ParseSDP(sessionDescription);
}

bool CRTSPSession::IsMediaTypeSupported(char * sdpline_m)
{
	char mediaType[32];
	if(sscanf(sdpline_m, "m=%s", mediaType) == 1)
	{
		int mediaTypeCount = sizeof(MediaTypeSupported) / sizeof(char *);
		for(int i=0; i<mediaTypeCount; ++i)
		{
			if(_stricmp(MediaTypeSupported[i], mediaType) == 0)
				return true;
		}
	}

	return false;
}

char * CRTSPSession::GetNextMediaType(char * sdpline)
{
	char * nextSDPLine_m = sdpline;
	while(1)
	{
		nextSDPLine_m = GetNextLine(nextSDPLine_m);
		if(nextSDPLine_m == NULL || nextSDPLine_m[0] == 'm')
			break;
	}
	return nextSDPLine_m;
}
/*
bool CRTSPSession::AddSessionMembership()
{
	for(int i=0; i<m_mediaStreamCount; ++i)
	{
		if(!m_mediaStreams[i]->AddMembership())
			return false;
	}
	return true;
}

void CRTSPSession::DropSessionMembership()
{
	for(int i=0; i<m_mediaStreamCount; ++i)
	{
		m_mediaStreams[i]->DropMembership();
	}
}
*/
bool CRTSPSession::ParseSDPParam_s(char * sdpParam)
{
	if(sscanf(sdpParam, "s=%[^\r\n]", m_sessionTitle) == 1)
	{
		CUserOptions::UserOptions.outInfo.clip_title = m_sessionTitle;
		sprintf(CLog::formatString,"clip-title:%s\n",m_sessionTitle);
		CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt",CLog::formatString);
		return true;
	}

	return false;
}
bool CRTSPSession::ParseSDPParam_c(char * sdpParam)
{
	do
	{
		if(_strnicmp(sdpParam, "c=IN IP", 7) != 0)
			break;

		char remoteIPString[64] = {0};
		if(sscanf(sdpParam, "c=IN IP4 %[^/ ]", remoteIPString) == 1)
		{// "c=IN IP4 *.*.*.*/ttl"
			if(_stricmp(remoteIPString, "224") < 0)
				break;
		}
		else if(sscanf(sdpParam, "c=IN IP6 %s", remoteIPString) == 1)
		{// "c=IN IP6 *:*:*:*:*:*:*:*"

			if(_stricmp(remoteIPString, "FF") < 0)
				break;
		}

		m_sessionAddr->family		=  VAF_UNSPEC;
		m_sessionAddr->port			=  0;
		m_sessionAddr->protocol		=  VPT_UDP;
		m_sessionAddr->sockType		=  VST_SOCK_DGRAM;
		int ret =IVOSocket::GetPeerAddressByURL(remoteIPString,strlen(remoteIPString),m_sessionAddr);
		if(ret)
			break;

		return true;

	}while(0);

	return false;
}

bool CRTSPSession::ParseSDPParam_b_AS(char * sdpParam)
{
	if(sscanf(sdpParam, "b=AS:%u", &m_sessionBandwidth) == 1)
	{
		//sprintf(CLog::formatString,"clip-bitrate:%u\n",m_sessionBandwidth);
		//CLog::Log.MakeLog(LL_SDP_ERR,"flow.txt",CLog::formatString);
		return true;
	}

	return false;
}
bool CRTSPSession::ParseSDPParam_bitrate(char * sdpParam)
{
	unsigned int bitrate = 0;
	if(sscanf(sdpParam, "a=AvgBitRate:integer;%u", &bitrate) == 1)
	{
		//CUserOptions::UserOptions.outInfo.clip_bitrate = bitrate;
		//sprintf(CLog::formatString,"clip-bitrate:%u\n",bitrate);
		//CLog::Log.MakeLog(LL_SDP_ERR,"flow.txt",CLog::formatString);
		return true;
	}

	return false;
}
bool CRTSPSession::ParserSDPAttribute_ASFHead(char* sdpAttr)
{
	do
	{
		//a=pgmpu:data:application/vnd.ms.wms-hdr.asfv1;base64,
		//sdpAttr+=25;
		if(_strnicmp(sdpAttr+25, "vnd.ms.wms-hdr.asfv1", 20) != 0) //a=pgmpu:data:application/
			break;
		sdpAttr += 53;
		int len=strlen(sdpAttr);
		int headLen=len;
		unsigned char* asfHead = new unsigned char[len];TRACE_NEW("rts_asfHead ",asfHead);
		voBaseSixFourDecodeEx((unsigned char*)sdpAttr, asfHead, &headLen);
		m_asfheadParser = new CASFHeadParser();TRACE_NEW("rts_m_asfheadParser",m_asfheadParser);
		m_asfheadParser->Parse(asfHead,headLen);
		SAFE_DELETE(asfHead);
		
		return true;

	}while(0);

	return false;
}
bool CRTSPSession::ParseSDPAttribute_etag(char * sdpAttr)
{
	do
	{
		if(_strnicmp(sdpAttr, "a=etag:", 7) != 0) 
			break;

		sdpAttr += 7;
		m_sessionEntityTag = StringDup(sdpAttr);

		return true;

	}while(0);

	return false;
}

bool CRTSPSession::ParseSDPAttribute_range(char * sdpAttr)
{
	float begin=0,end=0;//some MMS live range is 6.0-6.0

	if(CUserOptions::UserOptions.streamType==ST_ASF)
	{
		if(sscanf(sdpAttr, "a=range:npt=%g - %g", &begin,&end) ==2)
		{
			if(abs(begin-end)<2)
				m_sessionDuration = 0;
			else
				m_sessionDuration = end-begin;
			return true;
		}
	}
	if(sscanf(sdpAttr, "a=range:npt=%g - %g", &begin, &end) == 2 || sscanf(sdpAttr, "a=range: npt:%g - %g", &begin, &end) == 2)
	{
		//a=range:npt=0-  63.44533
		m_sessionDuration = end - begin;

#define INVALID_SESSION_DURATION(a)	(a<0 || a>(24*60*60))

		if(INVALID_SESSION_DURATION(m_sessionDuration))
			m_sessionDuration = 0;
			
		return true;
	}
	//CLog::Log.MakeLog(LL_SDP_ERR,"sdpStatus.txt","@@@@fail to parse session range\n");
	return false;
}

float CRTSPSession::SessionDuration()
{
	for(int i=0; i<m_mediaStreamCount; ++i)
	{
		m_sessionDuration = max(m_sessionDuration, m_mediaStreams[i]->MediaStreamEndTime());
		//make sure the media duration and session duratuion is same
		m_mediaStreams[i]->SetMediaStreamEndTime(m_sessionDuration);	
	}
	return m_sessionDuration; 
}

CMediaStream * CRTSPSession::MediaStream(int mediaStreamIndex)
{
	if(mediaStreamIndex < 0 || mediaStreamIndex >= m_mediaStreamCount)
		return NULL;

	return m_mediaStreams[mediaStreamIndex];
}
