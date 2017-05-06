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
#include "voCDataSource.h"

#include "voOSFunc.h"

#define LOG_TAG "voCDataSource"
#include "voLog.h"

voCDataSource::voCDataSource(VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB)
	: CBaseSource (hInst, nFormat, pMemOP, pFileOP, pDrmCB)
	, m_nAudioTrack (-1)
	, m_nVideoTrack (-1)
	, m_nDropedVideo (0)
	, m_pVideoHeadData (NULL)
	, m_nVideoHeadSize (0)
	, m_pAudioHeadData (NULL)
	, m_nAudioHeadSize (0)
	, m_pVideoBufferData(NULL)
	, m_nVideoBufferSize(0)
	, m_pAudioBufferData(NULL)
	, m_nAudioBufferSize(0)
	, m_pAudioTrack (NULL)
	, m_pVideoTrack (NULL)
	, m_llMaxBufferTime (10000)
	, m_llMinBufferTime (1000)
	, m_bKeepAudioBuffer (false)
	, m_bVideoUpdown(false)
	, m_pReadBuffer (NULL)
{
	memset (&m_filInfo, 0, sizeof (m_filInfo));
	memset (&m_sAudioTrackInfo, 0, sizeof (m_sAudioTrackInfo));
	memset (&m_sVideoTrackInfo, 0, sizeof (m_sVideoTrackInfo));
	memset (&m_nVideoHeaderInfo, 0, sizeof(m_nVideoHeaderInfo));

	m_sAudioFormat.SampleRate = 44100;
	m_sAudioFormat.Channels = 2;
	m_sAudioFormat.SampleBits = 16;

	m_sVideoFormat.Width = 320;
	m_sVideoFormat.Height = 240;
}

voCDataSource::~voCDataSource ()
{
//	voCAutoLock lock (&m_mtBuffer);
	if (m_pAudioTrack != NULL)
	{
		delete m_pAudioTrack;
		m_pAudioTrack = NULL;
	}

	if (m_pVideoTrack != NULL)
	{
		delete m_pVideoTrack;
		m_pVideoTrack = NULL;
	}

	if (m_pVideoHeadData != NULL)
	{
		delete []m_pVideoHeadData;
		m_pVideoHeadData = NULL;
	}
	if (m_pAudioHeadData != NULL)
	{
		delete []m_pAudioHeadData;
		m_pAudioHeadData = NULL;
	}
	if (m_pVideoBufferData != NULL)
	{
		delete []m_pVideoBufferData;
		m_pVideoBufferData = NULL;
	}
	if (m_pAudioBufferData != NULL)
	{
		delete []m_pAudioBufferData;
		m_pAudioBufferData = NULL;
	}
}

VO_U32 voCDataSource::LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	voCAutoLock lock (&m_mtBuffer);

	m_pReadBuffer = (VOMP_READBUFFER_FUNC *)pSource;

	return VO_ERR_NONE;
}

VO_U32 voCDataSource::CloseSource (void)
{
	voCAutoLock lock (&m_mtBuffer);

	if (m_pAudioTrack != NULL)
	{
		delete m_pAudioTrack;
		m_pAudioTrack = NULL;
	}

	if (m_pVideoTrack != NULL)
	{
		delete m_pVideoTrack;
		m_pVideoTrack = NULL;
	}

	m_nAudioTrack = -1;
	m_nVideoTrack = -1;

	return VO_ERR_NONE;
}

VO_U32 voCDataSource::GetSourceInfo (VO_PTR pSourceInfo)
{
	voCAutoLock lock (&m_mtBuffer);

	memcpy (pSourceInfo, &m_filInfo, sizeof (m_filInfo));

	return 0;
}

VO_U32 voCDataSource::GetTrackInfo (VO_U32 nTrack, VO_PTR pTrackInfo)
{
	voCAutoLock lock (&m_mtBuffer);

	if (nTrack == (VO_U32)m_nAudioTrack)
		memcpy (pTrackInfo, &m_sAudioTrackInfo, sizeof (m_sAudioTrackInfo));
	else
		memcpy (pTrackInfo, &m_sVideoTrackInfo, sizeof (m_sVideoTrackInfo));

	return 0;
}

VO_U32 voCDataSource::GetAudioFormat (VO_U32 nTrack, VO_AUDIO_FORMAT * pAudioFormat)
{
	if (pAudioFormat == NULL)
		return VO_ERR_INVALID_ARG;

	voCAutoLock lock (&m_mtBuffer);

	if(m_sAudioFormat.SampleRate)
		pAudioFormat->SampleRate = m_sAudioFormat.SampleRate;
	else
		pAudioFormat->SampleRate = 44100;

	if(m_sAudioFormat.Channels)
		pAudioFormat->Channels = m_sAudioFormat.Channels;
	else
		pAudioFormat->Channels = 2;

	if(m_sAudioFormat.SampleBits == 8 || m_sAudioFormat.SampleBits == 16 || m_sAudioFormat.SampleBits == 24 || m_sAudioFormat.SampleBits == 32)
		pAudioFormat->SampleBits = m_sAudioFormat.SampleBits;
	else
		pAudioFormat->SampleBits = 16;

	return 0;
}

VO_U32 voCDataSource::GetVideoFormat (VO_U32 nTrack, VO_VIDEO_FORMAT * pVideoFormat)
{
	if (pVideoFormat == NULL)
		return VO_ERR_INVALID_ARG;

	voCAutoLock lock (&m_mtBuffer);

	if(m_sVideoFormat.Width)
		pVideoFormat->Width = m_sVideoFormat.Width;
	else
		pVideoFormat->Width = 320;

	if(m_sVideoFormat.Height)
		pVideoFormat->Height = m_sVideoFormat.Height;
	else
		pVideoFormat->Height = 240;

	return 0;
}

VO_U32 voCDataSource::GetTrackData (VO_U32 nTrack, VO_PTR pTrackData)
{
	voCAutoLock lock (&m_mtBuffer);

	VO_U32				nRC = 0;
	VO_S32				nSize = 0;
	VOMP_BUFFERTYPE		bufHead;
	VO_SOURCE_SAMPLE *	pSample = (VO_SOURCE_SAMPLE *)pTrackData;

	if (nTrack == (VO_U32)m_nAudioTrack)
	{
		if (m_pReadBuffer != NULL)
		{
			memset (&bufHead, 0, sizeof (bufHead));
			bufHead.llTime = pSample->Time;
			nRC = m_pReadBuffer->ReadAudio (m_pReadBuffer->pUserData, &bufHead);
			if (nRC == VO_ERR_SOURCE_NEEDRETRY || nRC == VOMP_ERR_Retry)
			{
				voOS_Sleep (1);
				return VO_ERR_SOURCE_NEEDRETRY;
			}
		}
		else
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
			
			if(nRC == VO_ERR_SOURCE_NEEDRETRY)
				return VO_ERR_SOURCE_NEEDRETRY;
		}

		if ((bufHead.nFlag & VOMP_FLAG_BUFFER_HEADDATA) == VOMP_FLAG_BUFFER_HEADDATA)
		{
			nSize = bufHead.nSize&0x7fffffff;

			if ( bufHead.pBuffer == NULL)
			{
				m_sAudioTrackInfo.HeadData = bufHead.pBuffer;
				m_sAudioTrackInfo.HeadSize = nSize;
			}
			else
			{
				if (m_nAudioHeadSize < nSize)
				{
					if (m_pAudioHeadData != NULL)
						delete []m_pAudioHeadData;
					m_pAudioHeadData = NULL;
				}

				m_nAudioHeadSize = nSize;
				if (m_pAudioHeadData == NULL)
					m_pAudioHeadData = new VO_BYTE[m_nAudioHeadSize];

				if(m_pAudioHeadData != NULL)
				{
					memcpy (m_pAudioHeadData, bufHead.pBuffer, m_nAudioHeadSize);

					m_sAudioTrackInfo.HeadData = m_pAudioHeadData;
					m_sAudioTrackInfo.HeadSize = m_nAudioHeadSize;
				}
				else
				{
					m_sAudioTrackInfo.HeadData = NULL;
					m_sAudioTrackInfo.HeadSize = 0;
				}
				
			}

			nRC = VO_ERR_SOURCE_NEW_FORMAT;
		}

		if ((bufHead.nFlag & VOMP_FLAG_BUFFER_NEW_FORMAT) == VOMP_FLAG_BUFFER_NEW_FORMAT)
		{
			// added by gtxia 
			if((bufHead.nFlag & VOMP_FLAG_BUFFER_ADD_DRMDATA) == VOMP_FLAG_BUFFER_ADD_DRMDATA)
			{
				mIsADrmDataAppended = true;
			}

			if(bufHead.pData)
			{
				VOMP_TRACK_INFO* pTrackInfo = (VOMP_TRACK_INFO *)bufHead.pData;
				m_sAudioTrackInfo.Codec = pTrackInfo->nCodec;
				m_sAudioFormat.SampleRate = pTrackInfo->sAudioInfo.SampleRate;
				m_sAudioFormat.Channels = pTrackInfo->sAudioInfo.Channels;
				m_sAudioFormat.SampleBits = pTrackInfo->sAudioInfo.SampleBits;
				
				if ( pTrackInfo->pHeadData == NULL)
				{
					m_sAudioTrackInfo.HeadData = NULL;
					m_sAudioTrackInfo.HeadSize = 0;
				}
				else
				{
					if (m_nAudioHeadSize < pTrackInfo->nHeadSize)
					{
						if (m_pAudioHeadData != NULL)
							delete []m_pAudioHeadData;
						m_pAudioHeadData = NULL;
					}
					
					m_nAudioHeadSize = pTrackInfo->nHeadSize;
					if (m_pAudioHeadData == NULL)
						m_pAudioHeadData = new VO_BYTE[m_nAudioHeadSize];

					if(m_pAudioHeadData != NULL)
					{
						memcpy (m_pAudioHeadData, pTrackInfo->pHeadData, m_nAudioHeadSize);

						m_sAudioTrackInfo.HeadData = m_pAudioHeadData;
						m_sAudioTrackInfo.HeadSize = m_nAudioHeadSize;
					}
					else
					{
						m_sAudioTrackInfo.HeadData = NULL;
						m_sAudioTrackInfo.HeadSize = 0;
					}
					
				}
			}
			else
			{
				m_sAudioTrackInfo.HeadData = 0;
				m_sAudioTrackInfo.HeadSize = 0;
			}

			nRC = VO_ERR_SOURCE_NEW_FORMAT;
		}

		if((bufHead.nFlag & VOMP_FLAG_BUFFER_NEW_PROGRAM) == VOMP_FLAG_BUFFER_NEW_PROGRAM)
		{
			if(bufHead.pData)
			{
				VOMP_TRACK_INFO* pTrackInfo = (VOMP_TRACK_INFO *)bufHead.pData;
				m_sAudioTrackInfo.Codec = pTrackInfo->nCodec;
				m_sAudioFormat.SampleRate = pTrackInfo->sAudioInfo.SampleRate;
				m_sAudioFormat.Channels = pTrackInfo->sAudioInfo.Channels;
				m_sAudioFormat.SampleBits = pTrackInfo->sAudioInfo.SampleBits;
				
				if ( pTrackInfo->pHeadData == NULL)
				{
					m_sAudioTrackInfo.HeadData = NULL;
					m_sAudioTrackInfo.HeadSize = 0;
				}
				else
				{					
					if (m_nAudioHeadSize < pTrackInfo->nHeadSize)
					{
						if (m_pAudioHeadData != NULL)
							delete []m_pAudioHeadData;
						m_pAudioHeadData = NULL;
					}

					m_nAudioHeadSize = pTrackInfo->nHeadSize;
					if (m_pAudioHeadData == NULL)
						m_pAudioHeadData = new VO_BYTE[m_nAudioHeadSize];

					if(m_pAudioHeadData != NULL)
					{
						memcpy (m_pAudioHeadData, pTrackInfo->pHeadData, m_nAudioHeadSize);

						m_sAudioTrackInfo.HeadData = m_pAudioHeadData;
						m_sAudioTrackInfo.HeadSize = m_nAudioHeadSize;
					}
					else
					{
						m_sAudioTrackInfo.HeadData = NULL;
						m_sAudioTrackInfo.HeadSize = 0;
					}
					
				}
			}
			else
			{
				m_sAudioTrackInfo.HeadData = 0;
				m_sAudioTrackInfo.HeadSize = 0;
			}

			nRC = VO_ERR_SOURCE_NEW_PROGRAM;
		}

		if((bufHead.nFlag & VOMP_FLAG_BUFFER_TIME_RESET) == VOMP_FLAG_BUFFER_TIME_RESET)
		{
			if(nRC == VO_ERR_SOURCE_NEW_FORMAT || nRC == VO_ERR_SOURCE_NEW_PROGRAM)
				nRC = VO_ERR_SOURCE_NEW_PROGRAM;
			else
				nRC = VO_ERR_SOURCE_TIME_RESET;
		}
		
		if (nRC == VO_ERR_NONE || nRC == VO_ERR_SOURCE_NEW_PROGRAM || nRC == VO_ERR_SOURCE_NEW_FORMAT  || nRC == VO_ERR_SOURCE_TIME_RESET)
		{
			nSize = bufHead.nSize&0x7fffffff;
			
			if (m_nAudioBufferSize < nSize)
			{
				if (m_pAudioBufferData != NULL)
					delete []m_pAudioBufferData;
				m_pAudioBufferData = NULL;
				
				m_nAudioBufferSize = nSize;
			}

			if (m_pAudioBufferData == NULL)
				m_pAudioBufferData = new VO_BYTE[m_nAudioBufferSize];

			if (m_pAudioBufferData == NULL)
				return VO_ERR_OUTOF_MEMORY;

			if(bufHead.pBuffer)
				memcpy (m_pAudioBufferData, bufHead.pBuffer, nSize);
			
			pSample->Buffer = m_pAudioBufferData;
			pSample->Size = nSize;
			pSample->Time = bufHead.llTime;
			pSample->Duration = 1;
		}
	}
	else
	{
		if (m_pReadBuffer != NULL)
		{
			memset (&bufHead, 0, sizeof (bufHead));
			bufHead.llTime = pSample->Time;
			bufHead.nReserve = pSample->Duration;
			nRC = m_pReadBuffer->ReadVideo (m_pReadBuffer->pUserData, &bufHead);
			if (nRC == VO_ERR_SOURCE_NEEDRETRY || nRC == VOMP_ERR_Retry)
			{
				voOS_Sleep (1);
				return VO_ERR_SOURCE_NEEDRETRY;
			}
		}
		else
		{
			if (m_pVideoTrack == NULL)
				return VO_ERR_WRONG_STATUS;

			memset (&bufHead, 0, sizeof (bufHead));
			bufHead.llTime = pSample->Time;
			nRC = m_pVideoTrack->GetBuffer (&bufHead);
			if(nRC == VO_ERR_SOURCE_NEEDRETRY)
				return VO_ERR_SOURCE_NEEDRETRY;
		}

		if ((bufHead.nFlag & VOMP_FLAG_BUFFER_HEADDATA) == VOMP_FLAG_BUFFER_HEADDATA)
		{
			nSize = bufHead.nSize&0x7fffffff;
			
			if ( bufHead.pBuffer == NULL)
			{
				m_sVideoTrackInfo.HeadData = bufHead.pBuffer;
				m_sVideoTrackInfo.HeadSize = nSize;
			}
			else
			{
				if (m_nVideoHeadSize < nSize)
				{
					if (m_pVideoHeadData != NULL)
						delete []m_pVideoHeadData;
					m_pVideoHeadData = NULL;
				}

				m_nVideoHeadSize = nSize;
				if (m_pVideoHeadData == NULL)
					m_pVideoHeadData = new VO_BYTE[m_nVideoHeadSize];
				if (m_pVideoHeadData == NULL)
					return VO_ERR_OUTOF_MEMORY;

				memcpy (m_pVideoHeadData, bufHead.pBuffer, m_nVideoHeadSize);

				m_sVideoTrackInfo.HeadData = m_pVideoHeadData;
				m_sVideoTrackInfo.HeadSize = m_nVideoHeadSize;
			}

			nRC = VO_ERR_SOURCE_NEW_FORMAT;
		}	
		
		if ((bufHead.nFlag & VOMP_FLAG_BUFFER_NEW_FORMAT) == VOMP_FLAG_BUFFER_NEW_FORMAT)
		{
			// added by gtxia 
			if((bufHead.nFlag & VOMP_FLAG_BUFFER_ADD_DRMDATA) == VOMP_FLAG_BUFFER_ADD_DRMDATA)
			{
				VOLOGE("bufHead.nFlag = %x\n", bufHead.nFlag);
				mIsVDrmDataAppended = true;
			}

			if(bufHead.pData)
			{
				VOMP_TRACK_INFO* pTrackInfo = (VOMP_TRACK_INFO *)bufHead.pData;
				m_sVideoTrackInfo.Codec = pTrackInfo->nCodec;
				m_sVideoFormat.Width = pTrackInfo->sVideoInfo.Width;
				m_sVideoFormat.Height = pTrackInfo->sVideoInfo.Height;
				m_sVideoFormat.Type = (VO_VIDEO_FRAMETYPE)pTrackInfo->sVideoInfo.Type;

				if(m_sVideoTrackInfo.Codec == VOMP_VIDEO_CodingVP6)
				{
					m_bVideoUpdown = (pTrackInfo->nFourCC == 51) ? true : false;
				}
				
				if ( pTrackInfo->pHeadData == NULL)
				{
					m_sVideoTrackInfo.HeadData = NULL;
					m_sVideoTrackInfo.HeadSize = 0;
				}
				else
				{
					if (m_nVideoHeadSize < pTrackInfo->nHeadSize)
					{
						if (m_pVideoHeadData != NULL)
							delete []m_pVideoHeadData;
						m_pVideoHeadData = NULL;
					}

					m_nVideoHeadSize = pTrackInfo->nHeadSize;
					if (m_pVideoHeadData == NULL)
						m_pVideoHeadData = new VO_BYTE[m_nVideoHeadSize];
					if (m_pVideoHeadData == NULL)
						return VO_ERR_OUTOF_MEMORY;

					memcpy (m_pVideoHeadData, pTrackInfo->pHeadData, m_nVideoHeadSize);

					m_sVideoTrackInfo.HeadData = m_pVideoHeadData;
					m_sVideoTrackInfo.HeadSize = m_nVideoHeadSize;
				}
			}
			else
			{
				m_sVideoTrackInfo.HeadData = 0;
				m_sVideoTrackInfo.HeadSize = 0;
			}

			nRC = VO_ERR_SOURCE_NEW_FORMAT;
		}
		
		if((bufHead.nFlag & VOMP_FLAG_BUFFER_NEW_PROGRAM) == VOMP_FLAG_BUFFER_NEW_PROGRAM)
		{
			if(bufHead.pData)
			{
				VOMP_TRACK_INFO* pTrackInfo = (VOMP_TRACK_INFO *)bufHead.pData;
				m_sVideoTrackInfo.Codec = pTrackInfo->nCodec;
				m_sVideoFormat.Width = pTrackInfo->sVideoInfo.Width;
				m_sVideoFormat.Height = pTrackInfo->sVideoInfo.Height;
				m_sVideoFormat.Type = (VO_VIDEO_FRAMETYPE)pTrackInfo->sVideoInfo.Type;

				if ( pTrackInfo->pHeadData == NULL)
				{
					m_sVideoTrackInfo.HeadData = NULL;
					m_sVideoTrackInfo.HeadSize = 0;
				}
				else
				{
					if (m_nVideoHeadSize < pTrackInfo->nHeadSize)
					{
						if (m_pVideoHeadData != NULL)
							delete []m_pVideoHeadData;
						m_pVideoHeadData = NULL;
					}

					m_nVideoHeadSize = pTrackInfo->nHeadSize;
					if (m_pVideoHeadData == NULL)
						m_pVideoHeadData = new VO_BYTE[m_nVideoHeadSize];
					if (m_pVideoHeadData == NULL)
						return VO_ERR_OUTOF_MEMORY;

					memcpy (m_pVideoHeadData, pTrackInfo->pHeadData, m_nVideoHeadSize);

					m_sVideoTrackInfo.HeadData = m_pVideoHeadData;
					m_sVideoTrackInfo.HeadSize = m_nVideoHeadSize;
				}
			}
			else
			{
				m_sVideoTrackInfo.HeadData = 0;
				m_sVideoTrackInfo.HeadSize = 0;
			}

			nRC = VO_ERR_SOURCE_NEW_PROGRAM;
		}

		if((bufHead.nFlag & VOMP_FLAG_BUFFER_DELAY_TO_DROP) == VOMP_FLAG_BUFFER_DELAY_TO_DROP)
		{
			m_nDropedVideo = 1;
			nRC = VO_ERR_SOURCE_DROP_FRAME;
		}

		if((bufHead.nFlag & VOMP_FLAG_BUFFER_TIME_RESET) == VOMP_FLAG_BUFFER_TIME_RESET)
		{
			if(nRC == VO_ERR_SOURCE_NEW_FORMAT || nRC == VO_ERR_SOURCE_NEW_PROGRAM)
				nRC = VO_ERR_SOURCE_NEW_PROGRAM;
			else
				nRC = VO_ERR_SOURCE_TIME_RESET;
		}

		if (nRC == VO_ERR_NONE || nRC == VO_ERR_SOURCE_NEW_PROGRAM || nRC == VO_ERR_SOURCE_NEW_FORMAT || nRC == VO_ERR_SOURCE_TIME_RESET)
		{
#ifndef _WIN32
			nSize = bufHead.nSize&0x7fffffff;

			if (m_nVideoBufferSize < nSize)
			{
				if (m_pVideoBufferData != NULL)
					delete []m_pVideoBufferData;
				m_pVideoBufferData = NULL;
				
				m_nVideoBufferSize = nSize;
			}

			if (m_pVideoBufferData == NULL)
				m_pVideoBufferData = new VO_BYTE[m_nVideoBufferSize];
			if(m_pVideoBufferData == NULL)
				return VO_ERR_OUTOF_MEMORY;
			if(bufHead.pBuffer)
				memcpy (m_pVideoBufferData, bufHead.pBuffer, nSize);
			
			pSample->Buffer = m_pVideoBufferData;
			pSample->Size = nSize;
#else
			pSample->Buffer = bufHead.pBuffer;
			pSample->Size = bufHead.nSize;
#endif

			pSample->Time = bufHead.llTime;
			pSample->Duration = 1;
		}
	}

	if (nRC == VO_ERR_NONE || nRC == VO_ERR_SOURCE_NEW_PROGRAM || nRC == VO_ERR_SOURCE_NEW_FORMAT  || nRC == VO_ERR_SOURCE_TIME_RESET)
	{
		
		//VOLOGI ("bufHead.nFlag %x", (int)bufHead.nFlag);		
		
		if(bufHead.nFlag & VOMP_FLAG_BUFFER_KEYFRAME)
		{
			pSample->Size	|= 0x80000000;
		}

		if((bufHead.nFlag & VOMP_FLAG_BUFFER_DROP_FRAME) == VOMP_FLAG_BUFFER_DROP_FRAME || m_nDropedVideo)
		{
			pSample->Flag |= VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED;
		}

		if((bufHead.nFlag & VOMP_FLAG_VIDEO_EFFECT_ON) == VOMP_FLAG_VIDEO_EFFECT_ON)
		{
			pSample->Flag |= VOMP_FLAG_VIDEO_EFFECT_ON;
		}

		if((bufHead.nFlag & VOMP_FLAG_BUFFER_FRAME_DECODE_ONLY) == VOMP_FLAG_BUFFER_FRAME_DECODE_ONLY)
		{
			pSample->Flag |= VOMP_FLAG_BUFFER_FRAME_DECODE_ONLY;
		}

		m_nDropedVideo = 0;
	}

	return nRC;
}

VO_U32 voCDataSource::SetTrackPos (VO_U32 nTrack, VO_S64 * pPos)
{
	//voCAutoLock lock (&m_mtBuffer);

	//if (m_pVideoTrack != NULL)
	//	m_pVideoTrack->Flush ();
	//if (m_pAudioTrack != NULL)
	//	m_pAudioTrack->Flush ();

	return 0;
}

VO_U32 voCDataSource::GetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue)
{
	voCAutoLock lock (&m_mtBuffer);

	if (nTrack == (VO_U32)m_nVideoTrack)
	{
		if (nID == VO_PID_SOURCE_BITMAPINFOHEADER)
		{
			if (m_sVideoTrackInfo.Codec == VO_VIDEO_CodingWMV)
			{
				*(VO_PBYTE *)pValue = NULL;

				return VO_ERR_NONE;
			}
		}
		else if(nID == VO_PID_VIDEO_UPSIDEDOWN)
		{
			*(bool *)pValue = m_bVideoUpdown;

			return VO_ERR_NONE;		
		}
	}
	
	return CBaseSource::GetTrackParam (nTrack, nID, pValue);
}

VO_U32 voCDataSource::AddTrack (const VO_PTR pSource, int nCodec, bool bAudio)
{
	voCAutoLock lock (&m_mtBuffer);

	if (bAudio)
	{
		if (m_pAudioTrack == NULL && pSource == NULL)
			m_pAudioTrack = new voCDataBufferList (40960, 0);

		m_nAudioTrack = m_filInfo.Tracks;
		m_filInfo.Tracks++;

		m_sAudioTrackInfo.Type = VO_SOURCE_TT_AUDIO;
		m_sAudioTrackInfo.Codec = nCodec;
	}
	else
	{
		if (m_pVideoTrack == NULL && pSource == NULL)
			m_pVideoTrack = new voCDataBufferList (102400, 1);

		m_nVideoTrack = m_filInfo.Tracks;
		m_filInfo.Tracks++;
		m_sVideoTrackInfo.Type = VO_SOURCE_TT_VIDEO;
		m_sVideoTrackInfo.Codec = nCodec;
	}

	return 0;
}

VO_U32 voCDataSource::AddBuffer (int nSSType, VOMP_BUFFERTYPE * pBuffer)
{
	voCAutoLock lock (&m_mtBuffer);

	int nRC = VOMP_ERR_Implement;
	if (nSSType == VOMP_SS_Video)
	{
		if (m_pVideoTrack != NULL)
		{
			if (pBuffer->nFlag & VOMP_FLAG_BUFFER_FORCE_FLUSH)
			{
				VOLOGI ("Force Flush!");
				m_pVideoTrack->Flush ();
				return VOMP_ERR_None;
			}

			if (m_pVideoTrack->GetBuffTime () > m_llMaxBufferTime)
			{
				if (m_pAudioTrack != NULL)
				{
					if (m_pAudioTrack->GetBuffTime () > m_llMinBufferTime)
					{
						//VOLOGI ("Overflow video, m_pVideoTrack->GetBuffTime (): %d, m_pAudioTrack->GetBuffTime (): %d", m_pVideoTrack->GetBuffTime (), m_pAudioTrack->GetBuffTime ());
						//VOLOGI ("Overflow video, m_pVideoTrack->GetBuffCount (): %d, m_pAudioTrack->GetBuffCount (): %d", m_pVideoTrack->GetBuffCount (), m_pAudioTrack->GetBuffCount ());
						return VOMP_ERR_Implement;
					}
				}
				else
				{
					//VOLOGI ("Overflow video, m_pVideoTrack->GetBuffTime (): %d", m_pVideoTrack->GetBuffTime ());
					//VOLOGI ("Overflow video, m_pVideoTrack->GetBuffCount (): %d", m_pVideoTrack->GetBuffCount ());
					return VOMP_ERR_Implement;
				}
			}

			nRC = m_pVideoTrack->AddBuffer (pBuffer);
		}
	}
	else
	{
		if (m_pAudioTrack != NULL)
		{
			
			if (pBuffer->nFlag & VOMP_FLAG_BUFFER_FORCE_FLUSH)
			{
				VOLOGI ("Force Flush!");
				m_pAudioTrack->Flush ();
				return VOMP_ERR_None;
			}

			if (m_pAudioTrack->GetBuffTime () > m_llMaxBufferTime)
			{
				if (m_pVideoTrack != NULL)
				{
					if (m_pVideoTrack->GetBuffTime () > 0)
					{
						//VOLOGI ("Overflow audio, m_pVideoTrack->GetBuffTime (): %d, m_pAudioTrack->GetBuffTime (): %d", m_pVideoTrack->GetBuffTime (), m_pAudioTrack->GetBuffTime ());
						//VOLOGI ("Overflow audio, m_pVideoTrack->GetBuffCount (): %d, m_pAudioTrack->GetBuffCount (): %d", m_pVideoTrack->GetBuffCount (), m_pAudioTrack->GetBuffCount ());
						return VOMP_ERR_Implement;
					}
				}
				else
				{
					//VOLOGI ("Overflow audio, m_pVideoTrack->GetBuffTime (): %d, m_pAudioTrack->GetBuffTime (): %d", m_pVideoTrack->GetBuffTime (), m_pAudioTrack->GetBuffTime ());
					//VOLOGI ("Overflow audio, m_pVideoTrack->GetBuffCount (): %d, m_pAudioTrack->GetBuffCount (): %d", m_pVideoTrack->GetBuffCount (), m_pAudioTrack->GetBuffCount ());
					return VOMP_ERR_Implement;
				}
			}

			nRC = m_pAudioTrack->AddBuffer (pBuffer);
		}
	}

	if (nRC == VO_ERR_NONE)
		return VOMP_ERR_None;

	return VOMP_ERR_Implement;
}

VO_U64 voCDataSource::GetBufferTime (bool bAudio)
{
	voCAutoLock lock (&m_mtBuffer);

	VO_U64 nBuffTime = 0;
	if (bAudio && m_pAudioTrack != NULL)
		nBuffTime = (VO_U64)m_pAudioTrack->GetBuffTime ();
	else if (m_pVideoTrack != NULL)
		nBuffTime = (VO_U64)m_pVideoTrack->GetBuffTime ();

	return nBuffTime;
}

VO_U32 voCDataSource::Flush (void)
{
	voCAutoLock lock (&m_mtBuffer);

	if (m_pAudioTrack != NULL)
		m_pAudioTrack->Flush ();

	if (m_pVideoTrack != NULL)
		m_pVideoTrack->Flush ();

	m_bKeepAudioBuffer = true;

	return 0;
}
