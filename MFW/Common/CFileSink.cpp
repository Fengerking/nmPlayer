	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "CFileSink.h"

#define LOG_TAG "CFileSink"
#include "voLog.h"

typedef VO_S32 (VO_API * VOGETFILESINKAPI) (VO_SINK_WRITEAPI * pReadHandle, VO_U32 uFlag);

CFileSink::CFileSink(VO_PTR hInst, VO_FILE_FORMAT nFileFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP) 
	: CBaseNode (hInst, pMemOP)
	, m_hSink (NULL)
	, m_nFormat (nFileFormat)
{
	m_paramOpen.nFlag   = 1;
	m_paramOpen.pMemOP  = pMemOP;
	m_paramOpen.pSinkOP = pFileOP;
}

CFileSink::~CFileSink ()
{
	Uninit ();
}

VO_U32 CFileSink::Init (VO_FILE_SOURCE* pSource, VO_U32 nAudioCoding, VO_U32 nVideoCoding)
{
	Uninit ();

	voCAutoLock lock (&m_Mutex);

	if (LoadLib (NULL) <= 0)
		return VO_ERR_FAILED;

	m_paramOpen.nAudioCoding = nAudioCoding;
	m_paramOpen.nVideoCoding = nVideoCoding;

	VOLOGI ("Codec Audio %d, Video %d ", (unsigned int)nAudioCoding, (unsigned int)nVideoCoding);

	VO_U32 nRC = m_apiSink.Open (&m_hSink, pSource, &m_paramOpen);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("m_apiSink.Open was failed! Result 0X%08X", (unsigned int)nRC);
		return nRC;
	}

	nRC = m_apiSink.SetParam (m_hSink, VO_PID_COMMON_LIBOP, m_pLibOP);

	return VO_ERR_NONE;
}

VO_U32 CFileSink::Uninit (void)
{
	voCAutoLock lock (&m_Mutex);

	if (m_hSink != NULL) 
	{
		m_apiSink.Close (m_hSink);
		m_hSink = NULL;
	}

	return VO_ERR_NONE;
}

VO_U32 CFileSink::Start(void)
{
	return VO_ERR_NONE;
}

VO_U32 CFileSink::Pause(void)
{
	return VO_ERR_NONE;
}

VO_U32 CFileSink::Stop(void)
{
	voCAutoLock lock (&m_Mutex);

	if (m_hSink != NULL)
	{
		m_apiSink.Close (m_hSink);
		m_hSink = NULL;
	}

	return VO_ERR_NONE;
}

VO_U32 CFileSink::AddSample (VO_SINK_SAMPLE * pSample)
{
	voCAutoLock lock (&m_Mutex);

//	VOLOGI ("Sample size %d, AV %d, time %d", pSample->Size, pSample->nAV, (int)pSample->Time);

	if (m_nFormat == VO_FILE_FFMOVIE_TS)
		pSample->Size = pSample->Size & 0X7FFFFFFF;

	if (m_hSink != NULL)
		m_apiSink.AddSample (m_hSink, pSample);

	return VO_ERR_NONE;
}

VO_U32 CFileSink::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	voCAutoLock lock (&m_Mutex);

	if (m_hSink != NULL)
		m_apiSink.SetParam (m_hSink, uParamID, pData);

	return VO_ERR_NONE;
}

VO_U32 CFileSink::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	voCAutoLock lock (&m_Mutex);

	if (m_hSink != NULL)
		m_apiSink.GetParam (m_hSink, uParamID, pData);

	return VO_ERR_NONE;
}


VO_U32 CFileSink::LoadLib (VO_HANDLE hInst)
{
	if (m_hDll != NULL)
		return 1;

	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;

	switch(m_nFormat)
	{
	case VO_FILE_FFAUDIO_MP3:
		strcpy (m_szCfgItem, "Sink_File_MP3");
		vostrcpy (m_szDllFile, _T("voAudioFW"));
		vostrcpy (m_szAPIName, _T("voGetMP3WriterAPI"));
		break;
	case VO_FILE_FFAUDIO_AAC:
		strcpy (m_szCfgItem, "Sink_File_AAC");
		vostrcpy (m_szDllFile, _T("voAudioFW"));
		vostrcpy (m_szAPIName, _T("voGetAACWriterAPI"));
		break;
	case VO_FILE_FFAUDIO_QCP:
		strcpy (m_szCfgItem, "Sink_File_QCP");
		vostrcpy (m_szDllFile, _T("voAudioFW"));
		vostrcpy (m_szAPIName, _T("voGetQCPWriterAPI"));
		break;
	case VO_FILE_FFAUDIO_AMR:
		strcpy (m_szCfgItem, "Sink_File_AMR");
		vostrcpy (m_szDllFile, _T("voAudioFW"));
		vostrcpy (m_szAPIName, _T("voGetAMRWriterAPI"));
		break;

	case VO_FILE_FFMOVIE_TS:
		strcpy (m_szCfgItem, "Sink_File_TS");
		vostrcpy (m_szDllFile, _T("voTsFW"));
		vostrcpy (m_szAPIName, _T("voGetTSWriterAPI"));
		break;

	default:
	case VO_FILE_FFVIDEO_MPEG2:
	case VO_FILE_FFVIDEO_H263:
	case VO_FILE_FFMOVIE_REAL:
	case VO_FILE_FFMOVIE_MP4:
		strcpy (m_szCfgItem, "Sink_File_MP4");
		vostrcpy (m_szDllFile, _T("voMP4FW"));
		vostrcpy (m_szAPIName, _T("voGetMP4WriterAPI"));
		break;
	}

	if (m_pConfig != NULL)
	{
		pDllFile = m_pConfig->GetItemText (m_szCfgItem, (char*)"File");
		pApiName = m_pConfig->GetItemText (m_szCfgItem, (char*)"Api");
	}

#if defined _WIN32
	if (pDllFile != NULL)
	{
		memset (m_szDllFile, 0, sizeof (m_szDllFile));
		MultiByteToWideChar (CP_ACP, 0, pDllFile, -1, m_szDllFile, sizeof (m_szDllFile));
	}
	vostrcat (m_szDllFile, _T(".Dll"));

	if (pApiName != NULL)
	{
		memset (m_szAPIName, 0, sizeof (m_szAPIName));
		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
	}
#elif defined LINUX
	if (pDllFile != NULL)
		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".so"));

	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);
#endif

	if (CBaseNode::LoadLib (m_hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

	VOGETFILESINKAPI pAPI = (VOGETFILESINKAPI) m_pAPIEntry;
	pAPI (&m_apiSink, 0);

	return 1;
}
