/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CLiveSource.cpp

Contains:	CLiveSource class file

Written by:	

Change History (most recent first):


*******************************************************************************/
#include "voOSFunc.h"
#include "voLog.h"
#include "CLiveSource.h"

//#define DISABLE_AUDIO
//////////////////////////////////////////////////////////////////////////
VO_VOID VO_API cb_NofityStatus (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	CLiveSource* pLiveSource = (CLiveSource*)pUserData;
	if(pLiveSource)
		pLiveSource->NotifyStatus(nID, nParam1, nParam2);
}
VO_VOID VO_API cb_SendData(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{
	CLiveSource* pLiveSource = (CLiveSource*)pUserData;
	if(pLiveSource)
		pLiveSource->SendData(nOutputType, pData);
}
//////////////////////////////////////////////////////////////////////////

CLiveSource::CLiveSource(VO_PTR hInst, VO_U32 nFormat, 
						 VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB)
	:CBaseSource(hInst, nFormat, pMemOP, pFileOP, pDrmCB),
	m_pLiveSource(NULL),
	m_nAudioTrack(-1),
	m_nVideoTrack(-1),
	m_pVideoHeadData(NULL),
	m_nVideoHeadSize(0),
	m_pAudioHeadData(NULL),
	m_nAudioHeadSize(0),
	m_pAudioTrack(NULL),
	m_pVideoTrack(NULL),	
	m_llMaxBufferTime(10000),
	m_llMinBufferTime(1000),
	m_bStop(VO_FALSE)
{
	memset(m_URL, 0, sizeof(m_URL));
	memset(&m_filInfo, 0, sizeof(m_filInfo));
	memset(&m_sAudioTrackInfo, 0, sizeof(m_sAudioTrackInfo));
	memset(&m_sVideoTrackInfo, 0, sizeof(m_sVideoTrackInfo));
}

CLiveSource::~CLiveSource()
{
	CloseSource();
}

VO_U32	CLiveSource::LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	strcpy(m_URL, (const VO_CHAR*)pSource);
	m_pLiveSource = new CBaseLiveSource(GetLiveType(m_nFormat));

	m_pLiveSource->Open(this, cb_NofityStatus, cb_SendData);
	m_pLiveSource->SetDataSource(m_URL, 0);
	m_pLiveSource->Start();

	if(GetLiveType(m_nFormat) == VO_LIVESRC_FT_HTTP_LS)
	{
#ifndef DISABLE_AUDIO
		m_filInfo.Tracks = 2;
		m_nAudioTrack	 = 0;
		m_sAudioTrackInfo.Type  = VO_SOURCE_TT_AUDIO;
		m_sAudioTrackInfo.Codec = VO_AUDIO_CodingAAC;
		m_nVideoTrack	 = 1;
		m_sVideoTrackInfo.Type  = VO_SOURCE_TT_VIDEO;
		m_sVideoTrackInfo.Codec = VO_VIDEO_CodingH264;
#else
		m_filInfo.Tracks = 1;		
		m_nVideoTrack	 = 0;
		m_sVideoTrackInfo.Type  = VO_SOURCE_TT_VIDEO;
		m_sVideoTrackInfo.Codec = VO_VIDEO_CodingH264;	
#endif
	}
	if(m_pVideoTrack == NULL)
		m_pVideoTrack = new voCDataBufferList (102400, true);
#ifndef DISABLE_AUDIO
	if(m_pAudioTrack == NULL)
		m_pAudioTrack = new voCDataBufferList (40960, false);
#endif
	return 0;
}
VO_U32	CLiveSource::CloseSource (void)
{
	voCAutoLock lock (&m_mtBuffer);

	if(m_pLiveSource)
	{
		m_bStop = VO_TRUE;
		m_pLiveSource->Stop();
		m_pLiveSource->Close();
		m_bStop = VO_FALSE;
	}

	if (m_pAudioTrack != NULL)
		delete m_pAudioTrack;
	m_pAudioTrack = NULL;

	if (m_pVideoTrack != NULL)
		delete m_pVideoTrack;
	m_pVideoTrack = NULL;

	if (m_pVideoHeadData != NULL)
		delete []m_pVideoHeadData;
	if (m_pAudioHeadData != NULL)
		delete []m_pAudioHeadData;

	m_nAudioTrack = -1;
	m_nVideoTrack = -1;
	return 0;
}

VO_U32	CLiveSource::GetSourceInfo (VO_PTR pSourceInfo)
{
	voCAutoLock lock (&m_mtBuffer);

	memcpy (pSourceInfo, &m_filInfo, sizeof (m_filInfo));
	return 0;
}
VO_U32	CLiveSource::GetTrackInfo (VO_U32 nTrack, VO_PTR pTrackInfo)
{
	voCAutoLock lock (&m_mtBuffer);

	if (nTrack == (VO_U32)m_nAudioTrack)
		memcpy (pTrackInfo, &m_sAudioTrackInfo, sizeof (m_sAudioTrackInfo));
	else
		memcpy (pTrackInfo, &m_sVideoTrackInfo, sizeof (m_sVideoTrackInfo));
	return 0;
}
VO_U32	CLiveSource::GetAudioFormat (VO_U32 nTrack, VO_AUDIO_FORMAT * pAudioFormat)
{
	voCAutoLock lock (&m_mtBuffer);

	pAudioFormat->SampleRate = 48000;
	pAudioFormat->Channels	 = 2;
	pAudioFormat->SampleBits = 16;
	return 0;
}
VO_U32	CLiveSource::GetVideoFormat (VO_U32 nTrack, VO_VIDEO_FORMAT * pVideoFormat)
{
	voCAutoLock lock (&m_mtBuffer);

	pVideoFormat->Width = 320;
	pVideoFormat->Height = 240;
	return 0;
}
VO_U32	CLiveSource::GetFrameType (VO_U32 nTrack)
{
	return 0;
}

VO_U32	CLiveSource::GetTrackData (VO_U32 nTrack, VO_PTR pTrackData)
{
	voCAutoLock lock (&m_mtBuffer);

	int					nRC = 0;
	VOMP_BUFFERTYPE		bufHead;
	VO_SOURCE_SAMPLE *	pSample = (VO_SOURCE_SAMPLE *)pTrackData;

	if (nTrack == (VO_U32)m_nAudioTrack)
	{
		if (m_pAudioTrack == NULL)
			return VO_ERR_WRONG_STATUS;

		if (!m_pAudioTrack->IsEOS ())
		{
			if (m_bKeepAudioBuffer && m_pAudioTrack->GetBuffTime () < m_llMinBufferTime)
				return VO_ERR_SOURCE_NEEDRETRY;

			if ( m_pAudioTrack->GetBuffTime () < 100)
			{
				m_bKeepAudioBuffer = true;
				return VO_ERR_SOURCE_NEEDRETRY;
			}
		}

		m_bKeepAudioBuffer = false;
		nRC = m_pAudioTrack->GetBuffer (&bufHead);
		if (nRC == VO_ERR_SOURCE_NEW_FORMAT)
			m_sAudioTrackInfo.Codec = (VO_U32)bufHead.pData;

		if ((bufHead.nFlag & VOMP_FLAG_BUFFER_HEADDATA) == VOMP_FLAG_BUFFER_HEADDATA)
		{
			if ( bufHead.pBuffer == NULL)
			{
				m_sAudioTrackInfo.HeadData = bufHead.pBuffer;
				m_sAudioTrackInfo.HeadSize = bufHead.nSize;
			}
			else
			{
				if (m_nAudioHeadSize < bufHead.nSize)
				{
					if (m_pAudioHeadData != NULL)
						delete []m_pAudioHeadData;
					m_pAudioHeadData = NULL;
				}

				m_nAudioHeadSize = bufHead.nSize;
				if (m_pAudioHeadData == NULL)
					m_pAudioHeadData = new VO_BYTE[m_nAudioHeadSize];
				memcpy (m_pAudioHeadData, bufHead.pBuffer, m_nAudioHeadSize);

				m_sAudioTrackInfo.HeadData = m_pAudioHeadData;
				m_sAudioTrackInfo.HeadSize = m_nAudioHeadSize;
			}
		}			
	}
	else
	{
		if (m_pVideoTrack == NULL)
			return VO_ERR_WRONG_STATUS;

		memset (&bufHead, 0, sizeof (bufHead));
		bufHead.llTime = pSample->Time;
		nRC = m_pVideoTrack->GetBuffer (&bufHead);
		if (nRC == VO_ERR_SOURCE_NEW_FORMAT)
			m_sVideoTrackInfo.Codec = (VO_U32)bufHead.pData;

		if ((bufHead.nFlag & VOMP_FLAG_BUFFER_HEADDATA) == VOMP_FLAG_BUFFER_HEADDATA)
		{
			if ( bufHead.pBuffer == NULL)
			{
				m_sVideoTrackInfo.HeadData = bufHead.pBuffer;
				m_sVideoTrackInfo.HeadSize = bufHead.nSize;
			}
			else
			{
				if (m_nVideoHeadSize < bufHead.nSize)
				{
					if (m_pVideoHeadData != NULL)
						delete []m_pVideoHeadData;
					m_pVideoHeadData = NULL;
				}

				m_nVideoHeadSize = bufHead.nSize;
				if (m_pVideoHeadData == NULL)
					m_pVideoHeadData = new VO_BYTE[m_nVideoHeadSize];
				memcpy (m_pVideoHeadData, bufHead.pBuffer, m_nVideoHeadSize);

				m_sVideoTrackInfo.HeadData = m_pVideoHeadData;
				m_sVideoTrackInfo.HeadSize = m_nVideoHeadSize;
			}
		}			
	}

	if (nRC == VO_ERR_NONE || nRC == VO_ERR_SOURCE_NEW_PROGRAM || nRC == VO_ERR_SOURCE_NEW_FORMAT)
	{
		pSample->Buffer = (VO_PBYTE)bufHead.pBuffer;
		pSample->Size = bufHead.nSize;
		pSample->Time = bufHead.llTime;
		pSample->Duration = 1;
	}
	return nRC;
}
VO_U32	CLiveSource::SetTrackPos (VO_U32 nTrack, VO_S64 * pPos)
{
	voCAutoLock lock (&m_mtBuffer);

	if (m_pVideoTrack != NULL)
		m_pVideoTrack->Flush ();
	if (m_pAudioTrack != NULL)
		m_pAudioTrack->Flush ();

	return 0;
}

VO_U32	CLiveSource::GetSourceParam (VO_U32 nID, VO_PTR pValue)
{
	return 0;
}
VO_U32	CLiveSource::SetSourceParam (VO_U32 nID, VO_PTR pValue)
{
	return 0;
}

VO_U32	CLiveSource::GetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue)
{
	if(nTrack == (VO_U32)m_nAudioTrack)
	{
		if(nID == VO_PID_SOURCE_CODECCC)
			m_pLiveSource->GetParam(VO_PID_LIVESRC_AUDIO_FOURCC, pValue);
		else
			return VO_ERR_NOT_IMPLEMENT;
	}
	else
	{
		if(nID == VO_PID_SOURCE_CODECCC)
			m_pLiveSource->GetParam(VO_PID_LIVESRC_VIDEO_FOURCC, pValue);
		else if(nID == VO_PID_SOURCE_BITMAPINFOHEADER)
		{
			if (m_sVideoTrackInfo.Codec == VO_VIDEO_CodingWMV)
			{
				*(VO_PBYTE *)pValue = NULL;
				return VO_ERR_NONE;
			}
		}
		else
			return VO_ERR_NOT_IMPLEMENT;
	}	
	return 0;
}
VO_U32	CLiveSource::SetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue)
{
	return CBaseSource::SetTrackParam(nTrack, nID, pValue);
}

VO_VOID	CLiveSource::NotifyStatus ( VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2 )
{

}
VO_VOID	CLiveSource::SendData ( VO_U16 nOutputType, VO_PTR pData )
{
	int errType = VO_ERR_BASE;

	VO_LIVESRC_SAMPLE * ptr_sample = ( VO_LIVESRC_SAMPLE * )pData;

	VOMP_BUFFERTYPE buffer;
	memset( &buffer , 0 , sizeof( VOMP_BUFFERTYPE ) );
	buffer.llTime = ptr_sample->Sample.Time;
	buffer.nFlag = ptr_sample->Sample.Flag;
	buffer.nSize = ptr_sample->Sample.Size;
	buffer.pBuffer = ptr_sample->Sample.Buffer;
	buffer.pData = (void *)ptr_sample->nCodecType;

	while (errType != VO_ERR_NONE)
	{
		if(m_bStop)
			return;
		if( nOutputType == VO_LIVESRC_OUTPUT_AUDIO )
		{
#ifndef DISABLE_AUDIO
			errType = AddBuffer (false, &buffer);
#else
			errType = VO_ERR_NONE;
#endif
		}
		else if( nOutputType == VO_LIVESRC_OUTPUT_VIDEO )
			errType = AddBuffer (true, &buffer);

		if (errType != VO_ERR_NONE)
			voOS_Sleep(1000);
	}
}
//////////////////////////////////////////////////////////////////////////
VO_LIVESRC_FORMATTYPE CLiveSource::GetLiveType( VO_U32 nType )
{
	switch( nType )
	{
	case VO_FILE_FFSTREAMING_HLS:
		VOLOGI( "Live Streaming!" );
		return VO_LIVESRC_FT_HTTP_LS;
	case VO_FILE_FFSTREAMING_SSS:
		VOLOGI( "Smooth Streaming!" );
		return VO_LIVESRC_FT_IIS_SS;
	default:
		VOLOGE( "What is this? %x" , nType );
		return VO_LIVESRC_FT_FILE;
	}
}

VO_U32 CLiveSource::AddBuffer(bool bVideo, VOMP_BUFFERTYPE * pBuffer)
{
	voCAutoLock lock (&m_mtBuffer);

	int nRC = VO_ERR_NONE;
	if (bVideo)
	{
		if (m_pVideoTrack != NULL)
		{
			if (m_pVideoTrack->GetBuffTime () > m_llMaxBufferTime)
			{
				if (m_pAudioTrack != NULL)
				{
					if (m_pAudioTrack->GetBuffTime () > m_llMinBufferTime)
					{
						return VO_ERR_FAILED;
					}
				}
#ifndef DISABLE_AUDIO
				else
				{
					return VO_ERR_FAILED;
				}
#endif
			}

			nRC = m_pVideoTrack->AddBuffer (pBuffer);
		}
	}
	else
	{
		if (m_pAudioTrack != NULL)
		{
			if (m_pAudioTrack->GetBuffTime () > m_llMaxBufferTime)
			{
				return VO_ERR_FAILED;
			}			
			nRC = m_pAudioTrack->AddBuffer (pBuffer);
		}
	}

	return nRC;
}