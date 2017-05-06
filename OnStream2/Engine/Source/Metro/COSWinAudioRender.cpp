#include <ppltasks.h>
#include "COSWinAudioRender.h"
#include "voOSFunc.h"


using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace concurrency;

COSWinAudioRender::COSWinAudioRender(COSWinVomePlayer * pEngine)
	: m_pEngine(pEngine)
	, m_pOutRender (NULL)
	, m_pAudioBuffer (NULL)
	, m_pRenderThread (NULL)
	, m_pBuffer(NULL)
	, m_uRenderSize (0)
	, m_nStatus (0)
	, m_nRenderNum(0)
{
	m_sAudioFormat.Channels = 2;
	m_sAudioFormat.SampleRate = 44100;
	m_sAudioFormat.SampleBits = 16;

	m_pAudioBuffer =  new VOMP_BUFFERTYPE;

	m_uRenderSize = 44100*2*sizeof(short);
	m_pBuffer = new unsigned char[m_uRenderSize];
}

COSWinAudioRender::~COSWinAudioRender()
{
	Stop();

	while (m_pRenderThread != NULL)
		voOS_Sleep (10);	
	
	if(m_pOutRender) 
		delete m_pOutRender;

	if(m_pAudioBuffer)
		delete m_pAudioBuffer;

	if(m_pBuffer)
		delete m_pBuffer;
}

int	COSWinAudioRender::Start (void)
{
	if (m_nStatus == 1)
	{
		return 0;
	}
	else if (m_nStatus == 2)
	{
		if (m_pOutRender != NULL)
			m_pOutRender->Start();				
	}

	m_nStatus = 1;
	m_nRenderNum = 0;

	if(m_pRenderThread == NULL)
	{
		//VO_U32 ThdID;
		//voThreadCreate (&m_pRenderThread, &ThdID, (voThreadProc)AudioRenderThreadProc, this, 0);


		auto workItemDelegate = [this](IAsyncAction^ workItem)
		{
			AudioRenderThreadProc(this);
		};

		auto workItemHandler = ref new WorkItemHandler(workItemDelegate);
		ThreadPool::RunAsync(workItemHandler , WorkItemPriority::High);
		m_pRenderThread = (voThreadHandle)1;
	}

	return 0;
}

int COSWinAudioRender::Pause (void)
{
	if(m_pOutRender == NULL)
		return -1;

	m_nStatus = 2;
	
	return m_pOutRender->Pause();
}

int COSWinAudioRender::Stop (void)
{
	if(m_pOutRender == NULL)
		return -1;

	m_nStatus = 0;

	m_pOutRender->Stop();

	if(m_pOutRender) 
	{
		while (m_pRenderThread != NULL)
			voOS_Sleep (10);	
		delete m_pOutRender;
		m_pOutRender = NULL;
	}

	return 0;
}

int COSWinAudioRender::Flush (void)
{
	if(m_pOutRender == NULL)
		return -1;

	return 	m_pOutRender->Flush();
}

int COSWinAudioRender::AudioRenderThreadLoop (void)
{
	if(m_pOutRender == NULL)
	{
		m_pOutRender = new CXAudio2Render(NULL, NULL);
		m_pEngine->GetParam (VOOSMP_PID_AUDIO_FORMAT, &m_sAudioFormat);
		m_pOutRender->SetFormat ((VO_AUDIO_FORMAT *)&m_sAudioFormat);
		m_pOutRender->Start();
	}
	
	if(m_pAudioBuffer != NULL)
	{
		m_pAudioBuffer->pBuffer = m_pBuffer;
		m_pAudioBuffer->nSize = m_uRenderSize;
	}

	int nRC = m_pEngine->GetAudioBuffer(&m_pAudioBuffer);
	if (nRC == VOOSMP_ERR_None && m_pAudioBuffer != NULL)
	{
		if(m_nRenderNum == 0)
		{
			memset (&m_sAudioFormat, 0, sizeof (VOOSMP_AUDIO_FORMAT));
			m_pEngine->GetParam (VOOSMP_PID_AUDIO_FORMAT, &m_sAudioFormat);

			m_pOutRender->SetFormat ((VO_AUDIO_FORMAT *)&m_sAudioFormat);

			VO_U32 uRenderSize  = m_sAudioFormat.SampleRate*m_sAudioFormat.Channels*m_sAudioFormat.SampleBits/8;
			if(uRenderSize > m_uRenderSize)
			{
				m_uRenderSize = uRenderSize;

				BYTE *pTemp = m_pBuffer;
				m_pBuffer = new unsigned char[uRenderSize];

				memcpy(m_pBuffer , m_pAudioBuffer->pBuffer , m_pAudioBuffer->nSize);
				m_pAudioBuffer->pBuffer = m_pBuffer;

				delete pTemp;
			}

			if (m_pOutRender != NULL)
				m_pOutRender->Start();
		}
		
		m_nRenderNum++;

		if (m_pOutRender != NULL)
			m_pOutRender->Render((VO_PBYTE)m_pAudioBuffer->pBuffer, m_pAudioBuffer->nSize, m_pAudioBuffer->llTime, VO_TRUE);
	}
	else if (nRC == VOOSMP_ERR_FormatChange)
	{
		memset (&m_sAudioFormat, 0, sizeof (VOOSMP_AUDIO_FORMAT));
		m_pEngine->GetParam (VOOSMP_PID_AUDIO_FORMAT, &m_sAudioFormat);

		m_pOutRender->SetFormat ((VO_AUDIO_FORMAT *)&m_sAudioFormat);

		m_uRenderSize = m_sAudioFormat.SampleRate*m_sAudioFormat.Channels*m_sAudioFormat.SampleBits/8;
		if(m_pBuffer) 
		{
			delete m_pBuffer;
			m_pBuffer = NULL;
		}

		m_pBuffer = new unsigned char[m_uRenderSize];

		if (m_pOutRender != NULL)
			m_pOutRender->Start();
	}
	else
	{
		voOS_Sleep(2);
	}

	return 0;
}

int	COSWinAudioRender::AudioRenderThreadProc (void * pParam)
{
	COSWinAudioRender * pRender = (COSWinAudioRender *)pParam;

	while (pRender->m_nStatus == 1 || pRender->m_nStatus == 2)
	{
		if (pRender->m_nStatus == 2)
		{
			voOS_Sleep(2);
			continue;
		}

		pRender->AudioRenderThreadLoop ();
	}

	pRender->m_pRenderThread = NULL;


	return 0;
}