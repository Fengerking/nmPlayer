#include "CAviReader.h"
#include "CAviAudioTrack.h"
#include "OggFileDataStruct.h"
#include "fMacros.h"

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
File:		CAviAudioTrack.h

Contains:	The wrapper for avi audio track 

Written by:	East

Reference:	OpenDML AVI File Format Extensions

Change History (most recent first):
2006-09-20		East			Create file

*******************************************************************************/

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

//{00000001-0000-0010-8000-00aa00389b71}
VO_DEFINE_GUID(KSDATAFORMAT_SUBTYPE_PCM, 
			   0x00000001, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

//{00002000-0000-0010-8000-00aa00389b71}
VO_DEFINE_GUID(MEDIASUBTYPE_WAVE_DOLBY_AC3,
			   0x00002000, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

//{000000ff-0000-0010-8000-00aa00389b71}
VO_DEFINE_GUID(MEDIASUBTYPE_AAC,
			   0x000000ff, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

//{00002001-0000-0010-8000-00aa00389b71}
VO_DEFINE_GUID(MEDIASUBTYPE_WAVE_DTS,
			   0x00002001, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

CAviAudioTrack::CAviAudioTrack(VO_U8 btStreamNum, VO_U32 dwDuration, CAviReader *pReader, VO_MEM_OPERATOR* pMemOp)
	: CAviTrack(VOTT_AUDIO, btStreamNum, dwDuration, pReader, pMemOp)
	, m_dwAvgBytesPerSec(0)
	, m_ullTotalSize(0)
	, m_ullAudioTotalSize(0)
	, m_ullAudioCurrTotalSize(0)
	, m_dAudioVbrTimePerSample(0)
	, m_dwIndexOffset(0)
	, m_ptr_privatedata(NULL)
	, m_bPCM(VO_FALSE)
	, m_wFileBitsPerSample(0)
	, m_nFileBlockAlign(0)
	, m_nChannels(0)
	, m_nScale(0)
	,m_nRate(0)
{
}

CAviAudioTrack::~CAviAudioTrack()
{
	if( m_ptr_privatedata )
		delete []m_ptr_privatedata;

	m_ptr_privatedata = 0;
	m_privatedata_size = 0;
}

VO_U32 CAviAudioTrack::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	if(m_pIndex)
	{
		VO_U64 ullPosInFile = 0;
		VO_U32 dwSize = 0;
		if(!m_pIndex->GetEntryByIndex(m_dwCurrIndex, &ullPosInFile, &dwSize))
			return VO_ERR_SOURCE_END;

		pSample->Size = GET_SIZE(dwSize);
		if(pSample->Size > 0)
		{
			VO_S32 nRes = ReadFileContent(&pSample->Buffer, ullPosInFile, pSample->Size);

			if(0 == nRes)
				return VO_ERR_SOURCE_END;
			else if(2 == nRes)
				return VO_ERR_SOURCE_NEEDRETRY;
		}

		if(m_dAudioVbrTimePerSample)
		{
			pSample->Time = static_cast<VO_S64>( (m_dwCurrIndex + m_dwIndexOffset) * m_dAudioVbrTimePerSample );
			if(((VO_WAVEFORMATEX*)m_pPropBuffer)->nBlockAlign > 1 && pSample->Size > ((VO_WAVEFORMATEX*)m_pPropBuffer)->nBlockAlign)
				m_dwIndexOffset += (pSample->Size / ((VO_WAVEFORMATEX*)m_pPropBuffer)->nBlockAlign - 1);
		}
		else
		{
			if(m_dwAvgBytesPerSec)
			{
				pSample->Time = m_ullAudioCurrTotalSize * 1000 / m_dwAvgBytesPerSec;
			}
			else
			{
				if(m_nRate)
					pSample->Time = (m_dwCurrIndex*m_nScale)/m_nRate;
				else//the file doesn't provide any information to caculate the time so set the time with "0".
					pSample->Time = 0;
			}
			m_ullAudioCurrTotalSize += pSample->Size;
		}	

		pSample->Duration = 1;

		m_dwCurrIndex++;
	}
	else
	{
		VO_U32 rc = CAviTrack::GetSampleN(pSample);
		if(VO_ERR_SOURCE_OK != rc)
			return rc;
	}

	if(m_pReader->IsDivXDRM() && VO_ERR_DRM_OK != m_pReader->DRMData(VO_DRMDATATYPE_AUDIOSAMPLE, pSample->Buffer, pSample->Size & VO_MAXS32))
		return VO_ERR_SOURCE_ERRORDATA;

#ifndef _PCM_OUTPUT_RAWDATA
	if (m_bPCM)
	{
		if(24 == m_wFileBitsPerSample)
			PCM24216(pSample->Buffer, &pSample->Size, m_nChannels, m_nFileBlockAlign);
		else if(32 == m_wFileBitsPerSample)
			PCM32216(pSample->Buffer, &pSample->Size, m_nChannels, m_nFileBlockAlign);
	}
#endif	//_PCM_OUTPUT_RAWDATA

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviAudioTrack::SetPosN(VO_S64* pPos)
{
	if(!m_pIndex)
		return VO_ERR_SOURCE_SEEKFAIL;

	if(m_dAudioVbrTimePerSample)
	{
		m_dwCurrIndex = 0;
		m_dwIndexOffset = 0;

		VO_U32 dwCurrIndex = static_cast<VO_U32>(*pPos / m_dAudioVbrTimePerSample);
		VO_U32 dwSize = 0;
		while(m_dwCurrIndex + m_dwIndexOffset < dwCurrIndex)
		{
			if(!m_pIndex->GetEntryByIndex(m_dwCurrIndex, VO_NULL, &dwSize))
				break;

			dwSize = GET_SIZE(dwSize);
			if(((VO_WAVEFORMATEX*)m_pPropBuffer)->nBlockAlign > 1 && dwSize > ((VO_WAVEFORMATEX*)m_pPropBuffer)->nBlockAlign)
				m_dwIndexOffset += (dwSize / ((VO_WAVEFORMATEX*)m_pPropBuffer)->nBlockAlign - 1);

			m_dwCurrIndex++;
		}

		return VO_ERR_SOURCE_OK;
	}

	VO_U32 dwAudioTotalSize = static_cast<VO_U32>(*pPos * m_dwAvgBytesPerSec / 1000);
	if(!m_pIndex->GetEntryByTotalSize(dwAudioTotalSize, &m_dwCurrIndex))
		return VO_ERR_SOURCE_END;

	m_ullAudioCurrTotalSize = dwAudioTotalSize;
	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviAudioTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	VO_U32 nCodec = 0;
	VO_U32 nRes = GetCodec(&nCodec);
	if(VO_ERR_SOURCE_OK == nRes && VO_AUDIO_CodingOGG == nCodec)
	{
		if( !m_ptr_privatedata )
			perpare_ogg_headerdata();

		pHeadData->Length = m_privatedata_size;
		pHeadData->Buffer = m_ptr_privatedata;
	}
	else
	{
		pHeadData->Length = ((VO_WAVEFORMATEX*)m_pPropBuffer)->cbSize;
		pHeadData->Buffer = m_pPropBuffer + VO_WAVEFORMATEX_STRUCTLEN;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviAudioTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	if(pdwMaxSampleSize)
	{
		if(m_dwMaxSampleSize)
		{
			VO_WAVEFORMATEX* pwfe = (VO_WAVEFORMATEX*)m_pPropBuffer;
			*pdwMaxSampleSize = 10 * VO_MAX(pwfe->nSamplesPerSec * pwfe->nChannels * 2, 2048);
			if(m_dwMaxSampleSize < *pdwMaxSampleSize)
				*pdwMaxSampleSize = m_dwMaxSampleSize;
		}
		else
		{
			VO_WAVEFORMATEX* pwfe = (VO_WAVEFORMATEX*)m_pPropBuffer;
			*pdwMaxSampleSize = VO_MAX(pwfe->nSamplesPerSec * pwfe->nChannels * 2, 2048);
		}
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviAudioTrack::GetCodec(VO_U32* pCodec)
{
	VO_U32 dwFcc = 0;
	VO_U32 nRes = GetCodecCC(&dwFcc);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	if(pCodec)
	{
		if(VOTT_VIDEO == m_nType)
			*pCodec = fCodecGetVideoCodec(dwFcc);
		else if(VOTT_AUDIO == m_nType)
		{
			switch (dwFcc)
			{
			case 0xfffe:
				{
					VO_WAVEFORMATEXTENSIBLE* pp =( (VO_WAVEFORMATEXTENSIBLE*)m_pPropBuffer );
					if ( voIsEqualGUID(pp->SubFormat, KSDATAFORMAT_SUBTYPE_PCM) )
						*pCodec = VO_AUDIO_CodingPCM;
					else if ( voIsEqualGUID(pp->SubFormat, MEDIASUBTYPE_WAVE_DOLBY_AC3) )
						*pCodec = VO_AUDIO_CodingAC3;
					else if ( voIsEqualGUID(pp->SubFormat, MEDIASUBTYPE_AAC) )
						*pCodec = VO_AUDIO_CodingAAC;
					else if ( voIsEqualGUID(pp->SubFormat, MEDIASUBTYPE_WAVE_DTS) )
						*pCodec = VO_AUDIO_CodingDTS;

				}
				break;

			default:
				*pCodec = fCodecGetAudioCodec( static_cast<VO_U16>(dwFcc) );
			}

			if (*pCodec == VO_AUDIO_CodingPCM || *pCodec == VO_AUDIO_CodingADPCM)
				m_bPCM = VO_TRUE;
		}
		else
			*pCodec = dwFcc;
	}	

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviAudioTrack::GetCodecCC(VO_U32* pCC)
{
	*pCC = ((VO_WAVEFORMATEX*)m_pPropBuffer)->wFormatTag;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviAudioTrack::GetFirstFrame(VO_SOURCE_SAMPLE* pSample)
{
	if(m_pIndex)
	{
		VO_U64 ullPosInFile = 0;
		VO_U32 dwSize = 0;
		if(!m_pIndex->GetEntryByIndex(0, &ullPosInFile, &dwSize))
			return VO_ERR_SOURCE_END;

		pSample->Size = GET_SIZE(dwSize);
		pSample->Time = 0;

		if(pSample->Size > 0)
		{
			VO_S32 nRes = 0;
			nRes = m_pReader->ReadBuffer(m_pSampleData, ullPosInFile, pSample->Size);
			if(0 == nRes)
				return VO_ERR_SOURCE_END;
			else if(2 == nRes)
				return VO_ERR_SOURCE_NEEDRETRY;

			pSample->Buffer = m_pSampleData;
		}
		pSample->Duration = 1;
	}
	else
	{
		PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
		if(!m_pStreamFileIndex->GetEntry(VO_FALSE, 0, &pGet, NULL))
			return VO_ERR_SOURCE_END;

		pSample->Size = pGet->size;
		pSample->Time = 0;
		pSample->Duration = 1;

		if(2 == m_pGlobeBuffer->Peek(&pSample->Buffer, m_pSampleData, pGet->pos_in_buffer & VO_MAXS32, pGet->size))
			pSample->Buffer = m_pSampleData;
	}

	//if(m_pReader->IsDivXDRM() && VO_ERR_DRM_OK != m_pReader->DRMData(VO_DRMDATATYPE_AUDIOSAMPLE, pSample->Buffer, pSample->Size & VO_MAXS32))
	//	return VO_ERR_SOURCE_ERRORDATA;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviAudioTrack::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	VO_WAVEFORMATEX* pwfe = (VO_WAVEFORMATEX*)m_pPropBuffer;

#ifndef _PCM_OUTPUT_RAWDATA
	if(m_bPCM && pwfe->wBitsPerSample > 16)
	{
		//pwfe->nBlockAlign = pwfe->nBlockAlign * 16 / pwfe->wBitsPerSample;
		//pwfe->nAvgBytesPerSec = pwfe->nAvgBytesPerSec * 16 / pwfe->wBitsPerSample;
		pwfe->wBitsPerSample = 16;
	}
#endif	//_PCM_OUTPUT_RAWDATA

	pAudioFormat->Channels = pwfe->nChannels;
	pAudioFormat->SampleBits = pwfe->wBitsPerSample;
	pAudioFormat->SampleRate = pwfe->nSamplesPerSec;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviAudioTrack::GetWaveFormatEx(VO_WAVEFORMATEX** ppWaveFormatEx)
{
	*ppWaveFormatEx = (VO_WAVEFORMATEX*)m_pPropBuffer;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAviAudioTrack::GetBitrate(VO_U32* pdwBitrate)
{
	if(pdwBitrate)
		*pdwBitrate = ((VO_WAVEFORMATEX*)m_pPropBuffer)->nAvgBytesPerSec;

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CAviAudioTrack::AddFrame(CGFileChunk* pFileChunk, VO_U32 dwLen)
{
	if(!m_bInUsed || !m_pGlobeBuffer->HasIdleBuffer(dwLen))
		return pFileChunk->FSkip(dwLen);

	VO_U32 dwPosInBuffer = m_pGlobeBuffer->Add(pFileChunk, dwLen);
	if(VO_MAXU32 == dwPosInBuffer)
		return VO_FALSE;

	PBaseStreamMediaSampleIndexEntry pNew = m_pStreamFileIndex->NewEntry(static_cast<VO_U32>( m_dAudioVbrTimePerSample ? ((m_dwLastIndex + m_dwIndexOffset) * m_dAudioVbrTimePerSample) : (m_ullAudioTotalSize * 1000 / m_dwAvgBytesPerSec) ), 
		dwPosInBuffer, dwLen);
	if(!pNew)
		return VO_FALSE;

	m_pStreamFileIndex->Add(pNew);
	m_dwLastIndex++;
	if(((VO_WAVEFORMATEX*)m_pPropBuffer)->nBlockAlign > 1 && dwLen > ((VO_WAVEFORMATEX*)m_pPropBuffer)->nBlockAlign)
		m_dwIndexOffset += (dwLen / ((VO_WAVEFORMATEX*)m_pPropBuffer)->nBlockAlign - 1);
	m_ullAudioTotalSize += dwLen;

	return VO_TRUE;
}

VO_VOID CAviAudioTrack::OnIdx1(VO_U32 dwCurrIndexNum, PAviOriginalIndexEntry pEntry)
{
	CAviTrack::OnIdx1(dwCurrIndexNum, pEntry);

	m_ullTotalSize += pEntry->dwSize;
	if(m_pIndex)
		((CAviIdx1Index*)m_pIndex)->AddCount(dwCurrIndexNum);
}

VO_VOID CAviAudioTrack::perpare_ogg_headerdata()
{
	VO_S32 packet_size[3];
	memset( packet_size , 0 , sizeof( packet_size ) );

	VO_PBYTE ptr_pos = m_pPropBuffer + VO_WAVEFORMATEX_STRUCTLEN;
	VO_U32 uiHeadSize = ((VO_WAVEFORMATEX*)m_pPropBuffer)->cbSize;

	ptr_pos++;

	VO_S32 index = 0;
	while( index < 2 )
	{
		packet_size[index] = packet_size[index] + (VO_U32)*ptr_pos;

		if( *ptr_pos < 0xff )
			index++;

		ptr_pos++;
	}

	packet_size[index] = uiHeadSize - ( ptr_pos - (m_pPropBuffer + VO_WAVEFORMATEX_STRUCTLEN) ) - packet_size[0] - packet_size[1];

	m_privatedata_size = uiHeadSize - ( ptr_pos - (m_pPropBuffer + VO_WAVEFORMATEX_STRUCTLEN) ) + sizeof( VORBISFORMAT2 );
	m_ptr_privatedata = new VO_BYTE[ m_privatedata_size ];
	VORBISFORMAT2 * ptr_vorbis_head = (VORBISFORMAT2*)m_ptr_privatedata;
	VO_WAVEFORMATEX* pwfe = (VO_WAVEFORMATEX*)m_pPropBuffer;

	ptr_vorbis_head->BitsPerSample = 16;
	ptr_vorbis_head->Channels = pwfe->nChannels;
	ptr_vorbis_head->SamplesPerSec = pwfe->nSamplesPerSec;
	ptr_vorbis_head->HeaderSize[0] = packet_size[0];
	ptr_vorbis_head->HeaderSize[1] = packet_size[1];
	ptr_vorbis_head->HeaderSize[2] = packet_size[2];

	memcpy( m_ptr_privatedata + sizeof( VORBISFORMAT2 ) , ptr_pos , packet_size[0] + packet_size[1] + packet_size[2] );
}

VO_BOOL CAviAudioTrack::PCM24216(VO_PBYTE pBuffer, VO_U32* pdwSize, VO_U16 nChannels, VO_U16 nBlockAlign)
{
	if(!nChannels || !nBlockAlign)
		return VO_FALSE;

	VO_U16 wMinBlock = nChannels * 3;
	if(nBlockAlign % wMinBlock)
		return VO_FALSE;

	VO_S16* pDst = (VO_S16*)pBuffer;
	VO_PBYTE pSrc = pBuffer;
	VO_U32 dwNum = *pdwSize / wMinBlock;
	for(VO_U32 i = 0; i < dwNum; i++)
	{
		for(VO_U16 j = 0;  j < nChannels; j++)
		{
			*pDst++ = ((pSrc[2] << 8) | pSrc[1]);
			pSrc += 3;
		}
	}

	*pdwSize = 2 * nChannels * dwNum;
	return VO_TRUE;
}

VO_BOOL CAviAudioTrack::PCM32216(VO_PBYTE pBuffer, VO_U32* pdwSize, VO_U16 nChannels, VO_U16 nBlockAlign)
{
	if(!nChannels || !nBlockAlign)
		return VO_FALSE;

	VO_U16 wMinBlock = nChannels * 4;
	if(nBlockAlign % wMinBlock)
		return VO_FALSE;

	VO_S16* pDst = (VO_S16*)pBuffer;
	VO_PBYTE pSrc = pBuffer;
	VO_U32 dwNum = *pdwSize / wMinBlock;
	for(VO_U32 i = 0; i < dwNum; i++)
	{
		for(VO_U16 j = 0;  j < nChannels; j++)
		{
			*pDst++ = ((pSrc[3] << 8) | pSrc[2]);
			pSrc += 4;
		}
	}

	*pdwSize = 2 * nChannels * dwNum;
	return VO_TRUE;
}