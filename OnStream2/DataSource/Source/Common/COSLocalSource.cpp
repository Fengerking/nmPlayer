#include "COSLocalSource.h"

#define  LOG_TAG    "COSLocalSource"
#include "voLog.h"

COSLocalSource::COSLocalSource(VO_SOURCE2_LIB_FUNC *pLibop, int	nFF)
	: COSBaseSource(pLibop)	
	, m_nFF(nFF)
{	
	UpdataAPI();
	LoadDll();
}

COSLocalSource::~COSLocalSource ()
{
	unLoadDll();
}

int	COSLocalSource::Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag)
{
	int nRC = COSBaseSource::Init(pSource, nFlag, pInitparam, nInitFlag);
	if(nRC)	return nRC;

	if(nFlag&VOOSMP_FLAG_SOURCE_URL)
	{
		if(m_pCSubtitle)
		{
			delete m_pCSubtitle;
			m_pCSubtitle = NULL;
		}

		m_pCSubtitle = new COSTimeText(m_pLibop, m_szPathLib);
		if(!m_pCSubtitle)
			return VOOSMP_ERR_OutMemory;

		m_pCSubtitle->Init(0, 0, 0);
		m_pCSubtitle->SetParam (ID_SET_JAVA_ENV, (void *)m_nJavaENV);

		int nRRC = m_pCSubtitle->SetTimedTextFilePath((VO_TCHAR *)pSource, SUBTITLE_PARSER_TYPE_UNKNOWN);
		if(nRRC)
		{
			delete m_pCSubtitle;
			m_pCSubtitle = NULL;
		}
	}

	return nRC;
}

int	COSLocalSource::Uninit()
{
	int nRC = COSBaseSource::Uninit();
	return nRC;
}

int COSLocalSource::GetParam (int nID, void * pValue)
{
	int nRC = COSBaseSource::GetParam(nID, pValue);
	return nRC;
}

int COSLocalSource::SetParam (int nID, void * pValue)
{
	int nRC = COSBaseSource::SetParam(nID, pValue);
	return nRC;
}

int COSLocalSource::HandleEvent (int nID, int nParam1, int nParam2)
{
	int nRC = COSBaseSource::HandleEvent (nID, nParam1, nParam2);
	if(nRC) return nRC;

	nRC = callBackEvent ();
	return nRC;
}

int COSLocalSource::LoadDll()
{
	if(m_pLibop == NULL)
		return -1;
	
	if (m_hDll != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	m_hDll = m_pLibop->LoadLib(m_pLibop->pUserData, m_szFileDll, 0);

	if (m_hDll == NULL)
	{
		VOLOGI("Load library %s failed", m_szFileDll);
		return -1;
	}

	VOGETLOCALSRCFRAPI pGetAPI = (VOGETLOCALSRCFRAPI) m_pLibop->GetAddress (m_pLibop->pUserData, m_hDll, m_szFileAPI, 0);

	if (pGetAPI == NULL)
	{
		VOLOGI("get API %s failed", m_szFileAPI);
		return -1;
	}

	pGetAPI(&m_SourceAPI, 0);

	if(m_SourceAPI.Init == NULL)
		return -1;

	return 0;
}

int COSLocalSource::UpdataAPI()
{
	memset(m_szFileDll, 0, 256);
	memset(m_szFileAPI, 0, 256);

	if (m_nFF == VOOSMP_SRC_FFMOVIE_AVI)
	{
		strcpy (m_szFileDll, "voAVIFR");
		strcpy (m_szFileAPI, "voGetAVI2ReadAPI");
	}
	else if (m_nFF == VOOSMP_SRC_FFMOVIE_MP4 || m_nFF == VOOSMP_SRC_FFMOVIE_MOV)
	{
		strcpy (m_szFileDll, "voMP4FR");
		strcpy (m_szFileAPI, "voGetMP42ReadAPI");
	}
	else if (m_nFF == VOOSMP_SRC_FFMOVIE_ASF)
	{
		strcpy (m_szFileDll, "voASFFR");
		strcpy (m_szFileAPI, "voGetASF2ReadAPI");
	}
	else if (m_nFF == VOOSMP_SRC_FFMOVIE_MPG)
	{
		strcpy (m_szFileDll, "voMPGFR");
		strcpy (m_szFileAPI, "voGetMPG2ReadAPI");
	}
	else if (m_nFF == VOOSMP_SRC_FFMOVIE_TS)
	{
		strcpy (m_szFileDll, "voTsParser");
		strcpy (m_szFileAPI, "voGetMTV2ReadAPI");
	}
	else if (m_nFF == VOOSMP_SRC_FFMOVIE_FLV)
	{
		strcpy (m_szFileDll, "voFLVFR");
		strcpy (m_szFileAPI, "voGetFLV2ReadAPI");
	}
	else if (m_nFF == VOOSMP_SRC_FFMOVIE_REAL)
	{
		strcpy (m_szFileDll, "voRealFR");
		strcpy (m_szFileAPI, "voGetReal2ReadAPI");
	}
	else if (m_nFF == VOOSMP_SRC_FFAUDIO_OGG)
	{
		strcpy (m_szFileDll, "voOGGFR");
		strcpy (m_szFileAPI, "voGetOGG2ReadAPI");
	}
	else if (m_nFF == VOOSMP_SRC_FFAUDIO_MIDI)
	{
		strcpy (m_szFileDll, "voMidiFR");
		strcpy (m_szFileAPI, "voGetMidi2ReadAPI");
	}
	else if( m_nFF == VOOSMP_SRC_FFMOVIE_MKV )
	{
		strcpy( m_szFileDll , "voMKVFR" );
		strcpy( m_szFileAPI , "voGetMKV2ReadAPI");
	}
	else if( m_nFF == VOOSMP_SRC_FFVIDEO_H264 || m_nFF == VOOSMP_SRC_FFVIDEO_H265)
	{
		strcpy( m_szFileDll , "voRawDataFR");
		strcpy( m_szFileAPI , "voGetRawDataParser2API");
	}
	else if( m_nFF == VOOSMP_SRC_FFAUDIO_PCM)
	{
		strcpy( m_szFileDll , "voRawDataFR");
		strcpy( m_szFileAPI , "voGetPCM2RawDataParserAPI");
	}
	else if ( (VOOSMP_SRC_FFAUDIO_AAC <= m_nFF) && (m_nFF <= VOOSMP_SRC_FFAUDIO_DTS) )
	{
		strcpy (m_szFileDll, "voAudioFR");
		strcpy (m_szFileAPI, "voGetAudio2ReadAPI");
	}
	else
	{
		strcpy (m_szFileDll, "voMP4FR");
		strcpy (m_szFileAPI, "voGetMP42ReadAPI");
	}

	return 0;
}
