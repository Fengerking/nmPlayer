#include "CSrc.h"
#include "../CSourceSink.h"

CSourceSink* CSrc::m_pSrcSink = 0;
bool CSrc::m_bClose = false;
bool CSrc::m_bAutoDrop = false;
bool CSrc::m_bPause = false;
bool CSrc::m_bHasPrevieweData = true;

#ifdef DROP_SRC
	unsigned long CSrc::m_dwLastFrmTime = 0;
	bool CSrc::m_bNeedDrop = false;
#endif

CSrc::CSrc(void)
{
}

CSrc::~CSrc(void)
{
}

void CSrc::SetHasPreview(bool bHas)
{
	m_bHasPrevieweData = bHas;	
}

void CSrc::PreviewVideoSrcCallback(VO_MMR_VSBUFFER* pBuffer)
{
	//if(m_pSrcSink && !m_bClose)
		//m_pSrcSink->RecvPreviewVideoSource(pBuffer);
}


void CSrc::RecordVideoSrcCallback(VO_MMR_VSBUFFER* pBuffer)
{
	//OutputDebugString(_T("++++Recv record data.\n"));


#ifdef DROP_SRC
	if(m_bAutoDrop)
	{
		if(pBuffer->start_time!=0 && m_bNeedDrop)
		{
			m_dwLastFrmTime = pBuffer->start_time;
			m_bNeedDrop = false;
			return;
		}
		else
		{
			if(pBuffer->start_time == 0)
				m_dwLastFrmTime = 0;
			else
				pBuffer->start_time = m_dwLastFrmTime;
			
			m_bNeedDrop = true;
		}
	}
#endif

	if(m_pSrcSink && !m_bClose && !m_bPause)
	{
		m_pSrcSink->RecvRecordVideoSource(pBuffer);
		
		//if(!m_bHasPrevieweData)
			//m_pSrcSink->RecvPreviewVideoSource(pBuffer);
	}
		

	//OutputDebugString(_T("CSrc recv video data.\n"));
}

void CSrc::AudioSrcCallback(VO_MMR_ASBUFFER* pBuffer)
{
	if(m_pSrcSink && !m_bClose && !m_bPause)
		m_pSrcSink->RecvAudioSource(pBuffer);
}

void CSrc::SetSendStatus(bool bClose)
{
	m_bClose = bClose;
}

void CSrc::Pause(bool bPause)
{
	m_bPause = bPause;
}


void CSrc::SetAutoDrop(bool bDrop)
{
	m_bAutoDrop = bDrop;
}



