/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

/*******************************************************************************
File:		CAviMpeg2VideoTrack.cpp

Contains:	The wrapper for avi mpeg2 video 

Written by:	East

Reference:	OpenDML AVI File Format Extensions

Change History (most recent first):
2006-09-20		East			Create file

*******************************************************************************/
#include "CAviMpeg2VideoTrack.h"
#include "CAviReader.h"

#ifdef _VONAMESPACE
	namespace _VONAMESPACE{
#endif

int voSearchMpeg2StartCode(unsigned char *input, int length)
{
	/*find 0x00000100 or 0x000001b3*/
	const int LenStartCode = 4;
	unsigned char *head = input, *end = input + length - LenStartCode;

	do {
		if (head[0]) {/* faster if most of bytes are not zero*/	
			head++;
			continue;
		}
		/* find 00xx*/
		if (head[1]){
			head += 2; // skip 2 bytes;
			continue;
		}
		/* find 0000xx */
		if (head[2] != 0x01){
			if(head[2] == 0){
				head += 1;
			}else{
				head += 3; /* skip 3 bytes */
			}
			continue;
		}
		/* find 000001xx*/
		head += 4; /* update pointer first, since it will be used in all cases below*/
		if (head[-1] == 0/* || head[-1] == 0xb3*/)
			return (int)(head - input - LenStartCode);
	} while (head < end);
	/* not finding start code */
	return -1;
}

#ifdef _VONAMESPACE
	}
#endif


#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

CAviMpeg2VideoTrack::CAviMpeg2VideoTrack(VO_U8 btStreamNum, VO_U32 dwDuration, CAviReader *pReader, VO_MEM_OPERATOR* pMemOp)
	: CAviVideoTrack(btStreamNum, dwDuration, pReader, pMemOp)
	, m_pBufSample(VO_NULL)
	, m_dwUsedLenofCurrentIndex(0)
	, m_bNeedComposeMpeg2(VO_FALSE)
	, m_bNeedDropBFrameAfterSeek(VO_FALSE)
	, m_bIsGetSampleFirstTime(VO_TRUE)
	, m_llPos(0)
{
}


CAviMpeg2VideoTrack::~CAviMpeg2VideoTrack(void)
{
	SAFE_MEM_FREE(m_pBufSample);
}

VO_U32 CAviMpeg2VideoTrack::GetSampleB(VO_SOURCE_SAMPLE* pSample)
{
	VO_BOOL bDdFrame = VO_FALSE;
	if(m_pIndex)
	{
		if(!IndexBuffering_Fill())
			return VO_ERR_SOURCE_END;

		//process notify!!
		VO_U32 dwOptSkip = static_cast<VO_U32>(VO_U64(pSample->Time) * 10000 / m_ullAvgTimePerFrame);
		VO_U32 dwActSkip = 0;
		PAviIndexBufferingEntry pGet = m_pHead;
		if(dwOptSkip > m_dwCurrIndex)
		{
			dwOptSkip -= m_dwCurrIndex;
			if(dwOptSkip > m_dwCount)
				dwOptSkip = m_dwCount;

			PAviIndexBufferingEntry pTmp = m_pHead;
			for(VO_U32 i = 0; i < dwOptSkip; i++)
			{
				if(IS_KEY_FRAME(pTmp->size))
				{
					dwActSkip = i;
					pGet = pTmp;
				}
				pTmp = pTmp->next;
			}
		}

		if(dwActSkip > 0)
			pSample->Flag |= VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED;

		VO_S32 nRes = 0;
		if(IS_DD_FRAME(pGet->size))
			bDdFrame = VO_TRUE;
		if(bDdFrame)
		{
			nRes = ReadFileContent(&pSample->Buffer, pGet->dd_pos_in_file, pGet->dd_size);

			if(0 == nRes)
				return VO_ERR_SOURCE_END;
			else if(2 == nRes)
				return VO_ERR_SOURCE_NEEDRETRY;

			MemCopy(m_ddInfo, pSample->Buffer, sizeof(m_ddInfo));
		}

		pSample->Size = GET_SIZE(pGet->size);
		pSample->Time = (m_dwCurrIndex + dwActSkip) * m_ullAvgTimePerFrame / 10000;

		if(pSample->Size > 0)
		{
			nRes = ReadFileContent(&pSample->Buffer, pGet->pos_in_file, pSample->Size);

			if(0 == nRes)
				return VO_ERR_SOURCE_END;
			else if(2 == nRes)
				return VO_ERR_SOURCE_NEEDRETRY;
		}

		if(IS_KEY_FRAME(pGet->size))
			pSample->Size |= 0x80000000;

		pSample->Duration = 1;

		IndexBuffering_Remove(dwActSkip + 1);
		m_dwCurrIndex += (dwActSkip + 1);
	}
	else
	{
		VO_U32 rc = CAviTrack::GetSampleN(pSample);
		if(VO_ERR_SOURCE_OK != rc)
			return rc;

		if(pSample->Size > 0 && 0xFF == *pSample->Buffer)
			bDdFrame = VO_TRUE;
		if(bDdFrame)
		{
			//encrypt data
			MemCopy(m_ddInfo, pSample->Buffer + 1, sizeof(m_ddInfo));

			pSample->Buffer += (VO_DIVXDRM_DD_INFO_LENGTH + 1);
			pSample->Size = ((pSample->Size & VO_MAXS32) - (VO_DIVXDRM_DD_INFO_LENGTH + 1)) | (pSample->Size & 0x80000000);
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviMpeg2VideoTrack::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	if (m_bIsGetSampleFirstTime)
	{
		if (!IsInUsed())
			return VO_ERR_SOURCE_END;

		m_bIsGetSampleFirstTime = VO_FALSE;

		if (GetCodec(&m_dwCodec) == 0 && VO_VIDEO_CodingMPEG2 == m_dwCodec)
		{
			m_pBufSample = NEW_BUFFER(0x40000);
			if (m_pBufSample == NULL)
				return VO_ERR_OUTOF_MEMORY;

			VO_SOURCE_SAMPLE sample = {0};
			for (int i = 0; i < 5; i++)
			{
				if (GetSampleB(&sample) == 0 && sample.Size > 0)
				{
					if (sample.Buffer[0] != 0
						|| sample.Buffer[1] != 0
						|| sample.Buffer[2] != 1)
					{
						m_bNeedComposeMpeg2 = VO_TRUE;

						break;
					}
				}
			}

			if (VO_FALSE == m_bNeedComposeMpeg2)
			{
				SAFE_MEM_FREE(m_pBufSample);
			}

			SetPosN(&m_llPos);
		}
	}

	if (m_bNeedComposeMpeg2)
		return GetSampleMpeg2(pSample);
	else
		return GetSampleB(pSample);
}


VO_U32 CAviMpeg2VideoTrack::GetSampleMpeg2(VO_SOURCE_SAMPLE* pSample)
{
	if(m_pIndex)
	{
		if(!IndexBuffering_Fill())
			return VO_ERR_SOURCE_END;

		//process notify!!
		VO_U32 dwOptSkip = static_cast<VO_U32>(VO_U64(pSample->Time) * 10000 / m_ullAvgTimePerFrame);
		VO_U32 dwActSkip = 0;
		PAviIndexBufferingEntry pGet = m_pHead;
		if(dwOptSkip > m_dwCurrIndex)
		{
			dwOptSkip -= m_dwCurrIndex;
			if(dwOptSkip > m_dwCount)
				dwOptSkip = m_dwCount;

			PAviIndexBufferingEntry pTmp = m_pHead;
			for(VO_U32 i = 0; i < dwOptSkip; i++)
			{
				if(IsKeyFrame_Mpeg2(pTmp->pos_in_file, pTmp->size))
				{
					dwActSkip = i;
					pGet = pTmp;
				}
				pTmp = pTmp->next;
			}
		}

		if(dwActSkip > 0)
			pSample->Flag |= VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED;

		VO_S32 nRes = 0;
		VO_S32 CheckStart = 0;
		VO_U32 dwSize = 0;
		VO_PBYTE pBuffer = VO_NULL;
		do 
		{
			dwSize = GET_SIZE(pGet->size) - m_dwUsedLenofCurrentIndex;
			if(dwSize > 0)
			{
				nRes = ReadFileContent(&pBuffer, pGet->pos_in_file + m_dwUsedLenofCurrentIndex, dwSize);

				if(0 == nRes)
					return VO_ERR_SOURCE_END;
				else if(2 == nRes)
					return VO_ERR_SOURCE_NEEDRETRY;

				CheckStart = voSearchMpeg2StartCode(pBuffer, dwSize);
				if (CheckStart > 0)
				{
					pBuffer += CheckStart;
					dwSize -= CheckStart;
				}
				else if (-1 == CheckStart)
				{
					IndexBuffering_Remove(dwActSkip + 1);
					m_dwCurrIndex += (dwActSkip + 1);
					m_dwUsedLenofCurrentIndex = 0;
					dwActSkip = 0;

					pGet = m_pHead;
				}
			}
		} while (-1 == CheckStart);

		pSample->Time = (m_dwCurrIndex + dwActSkip) * m_ullAvgTimePerFrame / 10000;
		pSample->Size = 0;

		VO_S32 OffsetLen = voSearchMpeg2StartCode(pBuffer + 4, dwSize - 4);
		if (-1 != OffsetLen)
			OffsetLen += 4;

		do 
		{
			if (OffsetLen >= 0)
			{

				MemCopy( (m_pBufSample + pSample->Size), pBuffer, dwSize );
				pSample->Size += OffsetLen;
				m_dwUsedLenofCurrentIndex += (OffsetLen + CheckStart);

				break;
			}
			else
			{
				MemCopy( (m_pBufSample + pSample->Size), pBuffer, dwSize );
				pSample->Size += dwSize;

				CheckStart = 0;
				m_dwUsedLenofCurrentIndex = 0;

				do 
				{
					IndexBuffering_Remove(dwActSkip + 1);
					m_dwCurrIndex += (dwActSkip + 1);
					if (0 == m_dwCount)
						break;

					pGet = m_pHead;

					dwSize = GET_SIZE(pGet->size) - m_dwUsedLenofCurrentIndex;
					if(dwSize > 0)
					{
						pBuffer = VO_NULL;
						nRes = ReadFileContent(&pBuffer, pGet->pos_in_file + m_dwUsedLenofCurrentIndex, dwSize);

						if(0 == nRes)
							return VO_ERR_SOURCE_END;
						else if(2 == nRes)
							return VO_ERR_SOURCE_NEEDRETRY;
					}
				} while (!dwSize);

				OffsetLen = voSearchMpeg2StartCode(pBuffer, dwSize);
			}
		} while (0 != OffsetLen);

		if((m_pBufSample[5] & 0x38) == 8)//I Frame
		{
			pSample->Size |= 0x80000000;
		}

		pSample->Duration	= 1;
		pSample->Buffer		= m_pBufSample;
	}
	else
	{
		VO_U32 rc = CAviTrack::GetSampleN(pSample);
		if(VO_ERR_SOURCE_OK != rc)
			return rc;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviMpeg2VideoTrack::IsKeyFrame_Mpeg2(VO_U64 ullPos, VO_U32 uiSize)
{
	VO_S32 nRes = 0;
	VO_U32 dwSize = GET_SIZE(uiSize) ;
	VO_PBYTE pBuffer = VO_NULL;
	if(dwSize <= 0)
		return 0;

	nRes = ReadFileContent(&pBuffer, ullPos, dwSize);
	if(0 == nRes)
		return VO_ERR_SOURCE_END;
	else if(2 == nRes)
		return VO_ERR_SOURCE_NEEDRETRY;

	VO_S32 CheckStart = voSearchMpeg2StartCode(pBuffer, dwSize);
	if (CheckStart > 0)
	{
		pBuffer += CheckStart;
		dwSize -= CheckStart;

		if (dwSize >= 6)
		{
			if ((pBuffer[5] & 0x38) == 0x8)
				return 1;
		}
	}

	return 0;
}


VO_U32 CAviMpeg2VideoTrack::SetPosN(VO_S64* pPos)
{
	if(!m_pIndex)
		return VO_ERR_SOURCE_SEEKFAIL;

	m_dwCurrIndex = 0;
	m_dwUsedLenofCurrentIndex = 0;

	VO_U32 dwOptIndex = static_cast<VO_U32>(*pPos * (double)10000 / m_ullAvgTimePerFrame + 0.5);
	for(VO_U32 i = 0; i <= dwOptIndex; i++)
	{
		AviIndexBufferingEntry entryPeek = {0};

		if(!m_pIndex->GetEntryByIndex(i, &entryPeek.pos_in_file, &entryPeek.size))
			return VO_ERR_SOURCE_END;

		if (m_bNeedComposeMpeg2)
		{
			if (1 == IsKeyFrame_Mpeg2(entryPeek.pos_in_file, entryPeek.size))
			{
				m_dwCurrIndex = i;
			} 
		}
		else
		{
			if(IS_KEY_FRAME(entryPeek.size))
				m_dwCurrIndex = i;
		}

	}

	if (m_bNeedComposeMpeg2 && *pPos > 0)
		m_bNeedDropBFrameAfterSeek = VO_TRUE;

	m_dwCount = 0;
	m_pHead = m_pTail = m_pList;
	m_bNoIndex = VO_FALSE;

	*pPos = m_dwCurrIndex * m_ullAvgTimePerFrame / 10000;

	m_llPos = *pPos;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviMpeg2VideoTrack::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch (uID)
	{
	case VO_PID_SOURCE_GETTHUMBNAIL:
		{
			if(VOTT_VIDEO != m_nType)
				return VO_ERR_NOT_IMPLEMENT;

			VO_SOURCE_SAMPLE * ptr_sample = (VO_SOURCE_SAMPLE*)pParam;

			VO_BOOL bFirstFrame = VO_TRUE;

			for( VO_S32 i = 0 ; i < m_ThumbnallScanCount ; i++ )
			{
				VO_SOURCE_SAMPLE sample;
				sample.Time = 0;
				VO_U32 ret = 0;
				while(1)
				{
					ret = GetSampleN( &sample );
					if ((VO_ERR_SOURCE_OK != ret) || (sample.Size & 0x80000000) || bFirstFrame)
					{
						bFirstFrame = VO_FALSE;
						break;
					}
				}
				if(VO_ERR_SOURCE_OK == ret)
				{
					VO_S32 sample_size = static_cast<VO_S32>(sample.Size & VO_MAXS32);
					if( sample_size > m_ThumbnailBufferUsedSize && sample_size <= m_ThumbnailBufferSize )
					{
						memcpy( m_pThumbnailBuffer , sample.Buffer , sample_size );
						m_ThumbnailBufferUsedSize = sample_size;
					}
				} 
			}

			ptr_sample->Buffer = m_pThumbnailBuffer;
			ptr_sample->Size = m_ThumbnailBufferUsedSize;

			return VO_ERR_SOURCE_OK;
		}
		break;

	}

	return CAviVideoTrack::GetParameter(uID, pParam);
}


