#include "COSWinVomePlayer.h"
#include "COSWinAudioRender.h"
#include "COSWinVideoRender.h"


COSWinVomePlayer::COSWinVomePlayer()
	: m_pAudioRender (NULL)
	, m_pVideoRender (NULL)
	, m_Window(nullptr)
{
}

COSWinVomePlayer::~COSWinVomePlayer ()
{
	Uninit();
}

int	COSWinVomePlayer::Init()
{
	int nRC = COSVomePlayer::Init();
	
	if(nRC != VOOSMP_ERR_None) 
		return nRC;

	int nRndNum = 6;
	nRC = COSVomeEngine::SetParam (VOMP_PID_VIDEO_RNDNUM, &nRndNum);
	if (nRC != VOMP_ERR_None) return ReturnCode(nRC);

	int nPlayMode = VOMP_PULL_MODE;
	nRC = COSVomeEngine::SetParam(VOMP_PID_AUDIO_PLAYMODE, &nPlayMode);
	if (nRC != VOOSMP_ERR_None) return ReturnCode(nRC);
	nRC = COSVomeEngine::SetParam(VOMP_PID_VIDEO_PLAYMODE, &nPlayMode);
	
	return nRC;
}

int	COSWinVomePlayer::Uninit()
{
	if( m_pAudioRender != NULL )
	{
		delete 	m_pAudioRender;
		m_pAudioRender = NULL;
	}

	if( m_pVideoRender != NULL)
	{
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}

	return COSVomePlayer::Uninit();
}

int COSWinVomePlayer::SetDataSource (void * pSource, int nFlag)
{
	if(m_pAudioRender != NULL)
	{
		delete m_pAudioRender;
		m_pAudioRender =NULL;
	}

	//if(m_pVideoRender != NULL)
	//{
	//	delete m_pVideoRender;
	//	m_pVideoRender = NULL;
	//}
	
	m_pAudioRender = new COSWinAudioRender(this);		
	if (m_pAudioRender == NULL)
		return VOOSMP_ERR_OutMemory;
	//m_pVideoRender = new COSWinVideoRender(this, m_Window);

	//if(m_pVideoRender != NULL)
	//	m_pVideoRender->SetRect(&m_rcDraw);

	return COSVomePlayer::SetDataSource(pSource, nFlag);
}

int COSWinVomePlayer::Run (void)
{
	if(m_pAudioRender)
		m_pAudioRender->Start();

	if(m_pVideoRender)
		m_pVideoRender->Start();

	return COSVomePlayer::Run();
}

int COSWinVomePlayer::Pause (void)
{
	if(m_pAudioRender)
		m_pAudioRender->Pause();

	if(m_pVideoRender)
		m_pVideoRender->Pause();

	return COSVomePlayer::Pause();
}

int COSWinVomePlayer::Stop (void)
{
	if(m_pAudioRender)
		m_pAudioRender->Stop();

	if(m_pVideoRender)
		m_pVideoRender->Stop();

	return COSVomePlayer::Stop();
}

int COSWinVomePlayer::Close (void)
{
	int nRC = Stop();

	if(m_pAudioRender != NULL)
	{
		delete m_pAudioRender;
		m_pAudioRender =NULL;
	}

	if(m_pVideoRender != NULL)
	{
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}

	return nRC;
}

int COSWinVomePlayer::Flush (void)
{
	if(m_pAudioRender)
		m_pAudioRender->Flush();

	return COSVomePlayer::Flush();
}

int COSWinVomePlayer::SetView(void* pView)
{
	m_Window = reinterpret_cast<Windows::UI::Core::CoreWindow^>(pView);

	if(m_pVideoRender == NULL)
	{
		m_pVideoRender = new COSWinVideoRender(this, m_Window);
		if (m_pVideoRender == NULL)
			return VOOSMP_ERR_OutMemory;
	}

	return VOOSMP_ERR_None;
}

int COSWinVomePlayer::SetParam (int nID, void * pValue)
{
	if(nID == VOOSMP_PID_DRAW_RECT)
	{
		VOOSMP_RECT	*pRect = (VOOSMP_RECT	*)pValue;
		m_rcDraw.bottom = pRect->nBottom;
		m_rcDraw.right  = pRect->nRight;
		m_rcDraw.top	= pRect->nTop;
		m_rcDraw.left	= pRect->nLeft;

		if(m_pVideoRender)
			m_pVideoRender->SetRect(&m_rcDraw);
	}
	
	
	return COSVomePlayer::SetParam(nID, pValue);
}



