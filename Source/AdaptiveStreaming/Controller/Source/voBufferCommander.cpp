
#include "voBufferCommander.h"
#include "voAdaptiveStreamingClassFactory.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voBufferCommander::voBufferCommander()
:m_nBufferingTime(5000)
,m_nStartBufferTime(4000)
,m_nMaxBufferTime(20000)
,m_pSamplebuffer(NULL)
{

	CreateAdaptiveStreamingBuffer( &m_pSamplebuffer , m_nBufferingTime , m_nMaxBufferTime , m_nStartBufferTime );

}

voBufferCommander::~voBufferCommander()
{
	DestroyAdaptiveStreamingBuffer(m_pSamplebuffer);
	m_pSamplebuffer = NULL;
}

VO_U32 voBufferCommander::Init()
{
	VODS_VOLOGI("+ Init");

	VO_U32 ret = VO_RET_SOURCE2_OK;

	m_DownloadBufferMgr.Init();
	m_DownloadBufferMgr.SetFollowingComponent(m_pSamplebuffer);
	VODS_VOLOGI("- Init. ret:%08x", ret);
	
	return ret;
}

VO_U32 voBufferCommander::UnInit()
{
	VODS_VOLOGI("+ UnInit");
	VO_U32 ret = VO_RET_SOURCE2_OK;
	
	m_DownloadBufferMgr.UnInit();
	m_pSamplebuffer->Flush();

	VODS_VOLOGI("- UnInit");

	return ret;
}

VO_U32 voBufferCommander::ChunkAgentInit(VO_VOID **ppAgent, VO_U32 uASID, VO_BOOL bSwitch)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = m_DownloadBufferMgr.ChunkAgentInit(ppAgent,uASID, bSwitch);
	return ret;
}
VO_U32 voBufferCommander::ChunkAgentUninit(VO_VOID *pAgent, VO_U32 uASID, CHUNK_STATUS Switch)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = m_DownloadBufferMgr.ChunkAgentUninit(pAgent,uASID, Switch);
	return ret;
}

VO_U32 voBufferCommander::PushData( VO_DATASOURCE_BUFFER_TYPE nBufferType, VO_SOURCE2_TRACK_TYPE nTrackType, VO_PTR pData)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = m_DownloadBufferMgr.PushData( nBufferType, nTrackType, pData);

	VODS_VOLOGI("Push Data. BufferType:%d, TrackType:%d, ret:%08x",nBufferType, nTrackType, ret);
	return ret;
}

VO_U32 voBufferCommander::GetSample(VO_SOURCE2_TRACK_TYPE nTrackType , VO_PTR pSample)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	if(m_pSamplebuffer)
		ret = m_pSamplebuffer->GetBuffer(nTrackType, pSample);
	return ret;
}

VO_U32 voBufferCommander::SetParam(VO_U32 nParamID, VO_PTR pParam)
{

	VO_U32 ret = VO_RET_SOURCE2_OK;
	switch(nParamID)
	{
	case VO_PID_SOURCE2_PROGRAM_TYPE:
		ret = SetFBBufferingStyle(*((VO_SOURCE2_PROGRAM_TYPE*)pParam));
		break;
	case VO_PID_SOURCE2_BUFFER_STARTBUFFERINGTIME:
		m_nStartBufferTime = *((VO_U32 *) pParam);
		ret = SetFBBufferTime(VO_BUFFER_SETTING_STARTBUFFERTIME, m_nStartBufferTime);
		break;
	case VO_PID_SOURCE2_BUFFER_BUFFERINGTIME:
		m_nBufferingTime = *((VO_U32 *) pParam);
		ret = SetFBBufferTime(VO_BUFFER_SETTING_BUFFERTIME, m_nBufferingTime);
		break;
	case VO_PID_SOURCE2_BUFFER_MAXBUFFERSIZE:
		if(m_pSamplebuffer)
		{
			m_nMaxBufferTime = *((VO_U32 *) pParam);
			VOLOGI("Set max buffer is %d",m_nMaxBufferTime);
			ret = m_pSamplebuffer->SetParameter(VO_BUFFER_PID_MAX_SIZE, &m_nMaxBufferTime);
		}
		break;
		
	case VO_DATASOURCE_PID_STREAMCOUNT:
		{
			VO_U32 count = *((VO_U32 *) pParam);
			VO_U32 uMaxTime = count == 1 ? 2000 : 30000;

			VOLOGI("Max Chunk Size %d", uMaxTime);
			m_DownloadBufferMgr.setCapacity(uMaxTime);
		}
		break;

	default:
		ret =  VO_RET_SOURCE2_NOIMPLEMENT;
		break;
	}
	return ret;
}

VO_U32 voBufferCommander::GetParam(VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	switch(nParamID)
	{
	case VO_BUFFER_PID_IS_RUN:
	case VO_BUFFER_PID_DURATION_A:
	case VO_BUFFER_PID_DURATION_V:
	case VO_BUFFER_PID_DURATION_T:
	case VO_BUFFER_PID_EFFECTIVE_DURATION:
		if( m_pSamplebuffer )
			ret = m_pSamplebuffer->GetParameter(nParamID, pParam);
		break;
	default:
		ret = VO_RET_SOURCE2_NOIMPLEMENT;
		break;
	}
	return ret;
}


VO_U32 voBufferCommander::SetFBBufferingStyle(VO_SOURCE2_PROGRAM_TYPE nType)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	VO_BUFFER_SETTING_SAMPLE sample;
	VO_BUFFERING_STYLE_TYPE type = VO_BUFFERING_A;
	sample.nType = VO_BUFFER_SETTING_BUFFERING_STYLE;
	if(VO_SOURCE2_STREAM_TYPE_VOD == nType)
		type = VO_BUFFERING_AV;

	sample.pObj = &type;
	if( m_pSamplebuffer )
	{
		VODS_VOLOGI("Set Buffering Style. Type %d",type);
		ret = m_pSamplebuffer->AddBuffer( VO_BUFFER_SETTINGS , &sample );
	}
	else
		ret = VO_RET_SOURCE2_FAIL;
	return ret;
}


VO_U32 voBufferCommander::SetFBBufferTime(VO_BUFFER_SETTINGS_TYPE type, VO_U32 nTime)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	VO_BUFFER_SETTING_SAMPLE sample;
	sample.nType = type;
	sample.pObj = (VO_S32*)&nTime;

	if(m_pSamplebuffer )
	{
		VODS_VOLOGI ("Set Buffer Time. Type:%d, Time:%d", type, nTime);
		m_pSamplebuffer->AddBuffer( VO_BUFFER_SETTINGS , &sample );
	}
	else
		ret = VO_RET_SOURCE2_FAIL;
	return ret;
}
VO_VOID voBufferCommander::Stop()
{
	m_DownloadBufferMgr.Stop();
	if(m_pSamplebuffer)
		m_pSamplebuffer->Flush();
}

VO_VOID voBufferCommander::Flush()
{
	VODS_VOLOGI("+ Flush");
	m_DownloadBufferMgr.Flush();
	if(m_pSamplebuffer)
		m_pSamplebuffer->Flush();
	VODS_VOLOGI("- Flush");
}

VO_VOID voBufferCommander::SetPosition( VO_U64 pos )
{
	if(m_pSamplebuffer)
		m_pSamplebuffer->set_pos(pos);
}

VO_U32 voBufferCommander::GetDuration(VO_BUFFER_PID_TYPE nType, VO_U64 *pDuration)
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	if(m_pSamplebuffer)
		ret = m_pSamplebuffer->GetParameter(nType, pDuration);
	return ret;
}

