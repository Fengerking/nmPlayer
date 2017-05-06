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
#include "voCMixDataSource.h"
#include "voOSFunc.h"

#include "voCLivePlayer.h"

#define LOG_TAG "voCMixDataSource"
#include "voLog.h"

typedef VO_S32 (VO_API * VOGETPARSERAPI) (VO_PARSER_API * pParser);

voCMixDataSource::voCMixDataSource(VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB)
	: voCDataSource (hInst, nFormat, pMemOP, pFileOP, pDrmCB)
	, m_hParser (NULL)
	, m_bStopped (false)
	, m_pLivePlayer (NULL)
	, m_nStartSendSysTime (0)
	, m_nStartSendBufTime (0)
{
	memset (&m_sParser, 0, sizeof (VO_PARSER_API));
	memset (&m_sBuffer, 0, sizeof (VO_PARSER_INPUT_BUFFER));
	memset (&m_sVideoBuff, 0, sizeof (VOMP_BUFFERTYPE));
	memset (&m_sAudioBuff, 0, sizeof (VOMP_BUFFERTYPE));

	m_sInitInfo.pProc = ParserCB;
	m_sInitInfo.pMemOP = pMemOP;
	m_sInitInfo.pUserData = this;
}

voCMixDataSource::~voCMixDataSource(void)
{
	if (m_hParser != NULL)
	{
		m_sParser.Close (m_hParser);
		m_hParser = NULL;
	}
}

VO_U32 voCMixDataSource::LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	if (LoadLib (m_hInst) == 0)
		return VO_ERR_FAILED;

	return 0;
}

VO_U32 voCMixDataSource::CloseSource (void)
{
	return voCDataSource::CloseSource();
}

VO_U32 voCMixDataSource::AddBuffer (int nSSType, VOMP_BUFFERTYPE * pBuffer)
{
//	voOS_Sleep (10);
	if (m_hParser == NULL)
		return VOMP_ERR_Status;

	if (m_pAudioTrack != NULL && m_pVideoTrack != NULL)
	{
		if (m_pAudioTrack->GetBuffTime () > m_llMaxBufferTime || m_pVideoTrack->GetBuffTime () > m_llMaxBufferTime)
		{
			return VOMP_ERR_Retry;
		}
	}
	else if (m_pVideoTrack != NULL)
	{
		if (m_pVideoTrack->GetBuffTime () > m_llMaxBufferTime)
		{
			return VOMP_ERR_Retry;
		}
	}
	else if (m_pAudioTrack != NULL)
	{
		if (m_pAudioTrack->GetBuffTime () > m_llMaxBufferTime)
		{
			return VOMP_ERR_Retry;
		}
	}

	m_nProcRC = VOMP_ERR_None;

	m_sBuffer.pBuf = pBuffer->pBuffer;
	m_sBuffer.nBufLen = pBuffer->nSize;
	if (pBuffer->nFlag == VOMP_FLAG_BUFFER_NEW_PROGRAM || pBuffer->nFlag == VOMP_FLAG_BUFFER_NEW_FORMAT)
		m_sBuffer.nFlag = VO_PARSER_FLAG_STREAM_CHANGED; 

	int nRC = 0;

	nRC = m_sParser.Process (m_hParser, &m_sBuffer);

//	if (m_nProcRC != VOMP_ERR_None)
//		VOLOGI ("Add Buffer Return %08X", m_nProcRC);

	return m_nProcRC;
}

VO_U32 voCMixDataSource::Start(void)
{
	m_bStopped = false;

	return 	voCDataSource::Start ();
}

VO_U32 voCMixDataSource::Stop(void)
{
	m_bStopped = true;

	return 	voCDataSource::Stop ();
}

void voCMixDataSource::ParserCB (VO_PARSER_OUTPUT_BUFFER* pData)
{
	if (pData == NULL)
		return;

	voCMixDataSource * pSource = (voCMixDataSource *)pData->pUserData;

	pSource->ParseData (pData);
}

void voCMixDataSource::ParseData (VO_PARSER_OUTPUT_BUFFER* pData)
{
	if (pData->nType == VO_PARSER_OT_STREAMINFO)
	{
		VO_PARSER_STREAMINFO * pInfo = (VO_PARSER_STREAMINFO *)pData->pOutputData;
		if (pInfo->nAudioCodecType > 0)
		{
			m_sAudioTrackInfo.Codec = pInfo->nAudioCodecType;

			m_sAudioBuff.pBuffer = (unsigned char *)pInfo->pAudioExtraData;
			m_sAudioBuff.nSize = pInfo->nAudioExtraSize;
			m_sAudioBuff.llTime = 0;
			m_sAudioBuff.nFlag = VOMP_FLAG_BUFFER_HEADDATA;

			if (m_pLivePlayer == NULL)
				voCDataSource::AddBuffer (VOMP_SS_Audio, &m_sAudioBuff);
			else
				m_pLivePlayer->SendAudioSample (&m_sAudioBuff);
		}
		else if (pInfo->nVideoCodecType > 0)
		{
			m_sVideoTrackInfo.Codec = pInfo->nVideoCodecType;

			m_sVideoBuff.pBuffer = (unsigned char *)pInfo->pVideoExtraData;
			m_sVideoBuff.nSize = pInfo->nVideoExtraSize;
			m_sVideoBuff.llTime = 0;
			m_sVideoBuff.nFlag = VOMP_FLAG_BUFFER_HEADDATA;

			if (m_pLivePlayer == NULL)
				voCDataSource::AddBuffer (VOMP_SS_Video, &m_sVideoBuff);
			else
				m_pLivePlayer->SendVideoSample (&m_sVideoBuff);
		}
	}
	else if (pData->nType == VO_PARSER_OT_AUDIO)
	{
		VO_MTV_FRAME_BUFFER * pBuffer = (VO_MTV_FRAME_BUFFER *)pData->pOutputData;

		m_sAudioBuff.pBuffer = pBuffer->pData;
		m_sAudioBuff.nSize = pBuffer->nSize;
		m_sAudioBuff.llTime = pBuffer->nStartTime;
		m_sAudioBuff.nFlag = 0;

		if (m_pLivePlayer == NULL)
			m_nProcRC = voCDataSource::AddBuffer (VOMP_SS_Audio, &m_sAudioBuff);
		else
			m_nProcRC = m_pLivePlayer->SendAudioSample (&m_sAudioBuff);
		while (m_nProcRC != VOMP_ERR_None)
		{
			voOS_Sleep (5);
			if (m_bStopped)
				return;

			if (m_pLivePlayer == NULL)
				m_nProcRC = voCDataSource::AddBuffer (VOMP_SS_Audio, &m_sAudioBuff);
			else
			{
				m_nProcRC = m_pLivePlayer->SendAudioSample (&m_sAudioBuff);
				if (m_nProcRC != VOMP_ERR_Retry)
					break;
			}
		}
	}
	else if (pData->nType == VO_PARSER_OT_VIDEO)
	{
		VO_MTV_FRAME_BUFFER * pBuffer = (VO_MTV_FRAME_BUFFER *)pData->pOutputData;

		m_sVideoBuff.pBuffer = pBuffer->pData;
		m_sVideoBuff.nSize = pBuffer->nSize;
		m_sVideoBuff.llTime = pBuffer->nStartTime;
		m_sVideoBuff.nFlag = 0;

		if (m_pLivePlayer == NULL)
			m_nProcRC = voCDataSource::AddBuffer (VOMP_SS_Video, &m_sVideoBuff);
		else
			m_nProcRC = m_pLivePlayer->SendVideoSample (&m_sVideoBuff);
		while (m_nProcRC != VOMP_ERR_None)
		{
			voOS_Sleep (5);
			if (m_bStopped)
				return;

			if (m_pLivePlayer == NULL)
				m_nProcRC = voCDataSource::AddBuffer (VOMP_SS_Video, &m_sVideoBuff);
			else
			{
				m_nProcRC = m_pLivePlayer->SendVideoSample (&m_sVideoBuff);
				if (m_nProcRC != VOMP_ERR_Retry)
					break;
			}
		}

		if (m_nStartSendSysTime == 0 || m_nStartSendBufTime > m_sVideoBuff.llTime)
		{
			m_nStartSendSysTime = (VO_S64)voOS_GetSysTime ();
			m_nStartSendBufTime = m_sVideoBuff.llTime;
		}
//		while ((voOS_GetSysTime () - m_nStartSendSysTime) < (m_sVideoBuff.llTime - m_nStartSendBufTime + 200 + (rand () % 1000)))
//			voOS_Sleep (2);

	}
	else if (pData->nType == VO_PARSER_OT_EPG)
	{
	}
	else if (pData->nType == VO_PARSER_OT_TEXT)
	{
	}
	else if (pData->nType == VO_PARSER_OT_MEDIATYPE)
	{
	}
}

VO_U32 voCMixDataSource::LoadLib (VO_HANDLE hInst)
{
	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;

	if (m_nFormat == VO_FILE_FFMOVIE_TS)
	{
		strcpy (m_szCfgItem, "Source_File_TS");
		vostrcpy (m_szDllFile, _T("voTsParser"));
		vostrcpy (m_szAPIName, _T("voGetParserAPI"));
	}
	else if (m_nFormat == VO_FILE_FFMOVIE_CMMB)
	{
		strcpy (m_szCfgItem, "Source_File_CMMB");
		vostrcpy (m_szDllFile, _T("voCMMBParser"));
		vostrcpy (m_szAPIName, _T("voGetMTVReadAPI"));
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
#elif defined __SYMBIAN32__
	if (pDllFile != NULL)
		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".dll"));

	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);
#elif defined _MAC_OS
	if (pDllFile != NULL)
		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".dylib"));
	
	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);	
#endif

#ifdef _IOS
	
	switch (m_nFormat) {
#if _TS_READER
		case VO_FILE_FFMOVIE_TS:
			voGetParserAPI(&m_sParser);
			break;
#endif
		default:
			return 0;
			break;
	}
	
#else
	if (CBaseNode::LoadLib (m_hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

	VOGETPARSERAPI pAPI = (VOGETPARSERAPI) m_pAPIEntry;
	pAPI (&m_sParser);
#endif
	
	if (m_sParser.Open == NULL)
		return 0;

	m_sParser.Open (&m_hParser, &m_sInitInfo);
	if (m_hParser == NULL)
		return 0;

	return 1;
}