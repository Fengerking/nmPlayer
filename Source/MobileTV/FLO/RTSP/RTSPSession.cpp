#include <string.h>
#include "utility.h"
#include "network.h"
#include "MediaStream.h"

#include "RTSPSession.h"
#include "RTSPClientEngine.h"

#include "voLog.h"
#include "DxManager.h"
char * MediaTypeSupported[] = 
{
		"Video",
		"Audio"
		//"application"
};


CRTSPSession::CRTSPSession(CRTSPClientEngine* engine)
: m_sessionAddr(NULL)

, m_engine(engine)

{
	memset(m_mediaStreams, 0, sizeof(m_mediaStreams));
	ResetErrHandler();
	m_sessionAddr = new struct voSocketAddress;
	m_sessionAddr->family = VAF_INET4;
	m_mediaStreamCount = 0;
}

CRTSPSession::~CRTSPSession()
{
	SAFE_DELETE(m_sessionAddr);

	for(int i=0; i<m_mediaStreamCount; ++i)
	{
		SAFE_DELETE(m_mediaStreams[i]);
	}
}

bool CRTSPSession::ParseSDP(char * sessionDescription)
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
	}
	
	while(sdpline != NULL && sdpline[0] == 'm')
	{
		char * mediaType = GetMediaTypeSupported(sdpline);
		if(mediaType == NULL)
		{
			sdpline = GetNextMediaType(sdpline);
			continue;
		}
		
		CMediaStream * mediaStream = new CMediaStream(this);
		if(!mediaStream->Init(sdpline))
		{
			VOLOGE("CMediaStream Init Failure");

			SAFE_DELETE(mediaStream);
			mediaStream = NULL;
			continue;
		}

		if(mediaStream->IsAudioStream() || mediaStream->IsVideoStream())
		{				
			m_mediaStreams[m_mediaStreamCount++] = mediaStream;

			if(m_mediaStreamCount > 1) //Sometimes,the server will send an extra media,do not process it 
				break;
		}
		else
		{

		}
	}
	
	return m_mediaStreamCount > 0;
}

bool CRTSPSession::Init(char * sessionDescription, struct voSocketAddress * sessionAddr)
{
	if(m_sessionAddr == NULL)
	{
		m_sessionAddr = new struct voSocketAddress; 
	}

	if(sessionAddr != NULL)
	{
		memcpy(m_sessionAddr, sessionAddr, sizeof(voSocketAddress));
	}
	
	return ParseSDP(sessionDescription);
}
void CRTSPSession::ProcessErrPack()
{
	VOLOGI();
	if(m_errHandler.errPackNum==0)
		m_errHandler.timeBegin = voOS_GetSysTime();
	m_errHandler.errPackNum++;
	int timeelpased = voOS_GetSysTime()- m_errHandler.timeBegin;
	if(timeelpased>2000&&m_errHandler.errPackNum*1000/timeelpased > 20 )
	{
		VOLOGI("err pack(%d) in %d ms ",m_errHandler.errPackNum,timeelpased);
		ResetErrHandler();
		//NotifyEvent(HS_EVENT_PLAY_FAIL, E_DATA_GET_ERROR);
	}
}
char* CRTSPSession::GetMediaTypeSupported(char * sdpline_m)
{
	char mediaType[32];
	if(sscanf(sdpline_m, "m=%s", mediaType) == 1)
	{
		int mediaTypeCount = sizeof(MediaTypeSupported) / sizeof(char *);
		for(int i=0; i<mediaTypeCount; ++i)
		{
			if(_stricmp(MediaTypeSupported[i], mediaType) == 0)
				return MediaTypeSupported[i];
		}
	}

	return NULL;
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
		int ret =IVOSocket::GetPeerAddressByURL(remoteIPString, strlen(remoteIPString), m_sessionAddr);
		if(ret)
			break;

		return true;

	}while(0);

	return false;
}


CMediaStream * CRTSPSession::MediaStream(int mediaStreamIndex)
{
	if(mediaStreamIndex < 0 || mediaStreamIndex >= m_mediaStreamCount)
		return NULL;

	return m_mediaStreams[mediaStreamIndex];
}


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
