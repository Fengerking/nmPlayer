	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CAuReader.cpp

Contains:	CAuReader class file

Written by:	East

Change History (most recent first):
2009-10-21		East			Create file

*******************************************************************************/
#include "CAuReader.h"
#include "fCC.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CAuReader::CAuReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseAudioReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_dwSampleSize(0)
	, m_dSampleTime(0.0)
	, m_wFileBitsPerSample(0)
	, m_nFileBlockAlign(0)
	, m_nFileAvgBytesPerSec(0)
	, m_dwCurrIndex(0)
{
	MemSet(&m_wfe, 0, sizeof(m_wfe));
}

CAuReader::~CAuReader()
{
}

VO_U32 CAuReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseAudioReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	if(!ParseFileHeader())
		return VO_ERR_SOURCE_OPENFAIL;

	m_dwMaxSampleSize = m_dwSampleSize;
	m_dwDuration = (VO_U32)((m_ullFileSize - m_ullFileHeadSize) * 1000 / m_nFileAvgBytesPerSec);
	m_dSampleTime = double(m_dwSampleSize) * 1000 / m_nFileAvgBytesPerSec;

	if(-1 == FileSeek(m_hFile, m_ullFileHeadSize, VO_FILE_BEGIN))
		return VO_ERR_SOURCE_OPENFAIL;

	m_pBuf = NEW_BUFFER(m_dwSampleSize);

	return VO_ERR_SOURCE_OK;
}

#define EXCHANGE_VALUE(b1, b2)	btTmp = b1;\
	b1 = b2;\
	b2 = btTmp
VO_U32 CAuReader::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	VO_S32 nRes = FileRead(m_hFile, m_pBuf, m_dwSampleSize);
	if(nRes <= 0)
		return (-2 == nRes) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;

	pSample->Buffer = m_pBuf;
	pSample->Size = nRes;

	pSample->Time = (VO_S64)(m_dwCurrIndex * m_dSampleTime);
	pSample->Duration = 1;

	//process PCM data
	if(AudioFlag_MS_PCM == m_wfe.wFormatTag)
	{
		switch(m_wFileBitsPerSample)
		{
		case 8:		//singed -> unsigned
			{
				VO_U32 i = 0;
				while(i < m_dwSampleSize)
				{
					m_pBuf[i] += 0x80;

					i++;
				}
			}
			break;

		case 16:	//big endian -> little endian
			{
				VO_U32 i = 0;
				VO_U8 btTmp = 0;
				while(i < m_dwSampleSize)
				{
					EXCHANGE_VALUE(m_pBuf[i], m_pBuf[i + 1]);

					i += 2;
				}
			}
			break;

		case 24:	//big endian -> little endian
			{
				VO_U32 i = 0;
				VO_U8 btTmp = 0;
				while(i < m_dwSampleSize)
				{
					EXCHANGE_VALUE(m_pBuf[i], m_pBuf[i + 2]);

					i += 3;
				}
			}
			break;

		case 32:	//big endian -> little endian
			{
				VO_U32 i = 0;
				VO_U8 btTmp = 0;
				while(i < m_dwSampleSize)
				{
					EXCHANGE_VALUE(m_pBuf[i], m_pBuf[i + 3]);
					EXCHANGE_VALUE(m_pBuf[i + 1], m_pBuf[i + 2]);

					i += 4;
				}
			}
			break;

		default:
			break;
		}

#ifndef _PCM_OUTPUT_RAWDATA
		if(24 == m_wFileBitsPerSample)
			PCM24216(pSample->Buffer, &pSample->Size, m_wfe.nChannels, m_nFileBlockAlign);
		else if(32 == m_wFileBitsPerSample)
			PCM32216(pSample->Buffer, &pSample->Size, m_wfe.nChannels, m_nFileBlockAlign);
#endif	//_PCM_OUTPUT_RAWDATA
	}

	m_dwCurrIndex++;
	return VO_ERR_SOURCE_OK;
}

VO_U32 CAuReader::SetPos(VO_S64* pPos)
{
	m_dwCurrIndex = (VO_U32)(m_nFileAvgBytesPerSec * (*pPos) / (1000 * m_dwSampleSize));
	if(-1 == FileSeek(m_hFile, m_ullFileHeadSize + m_dwCurrIndex * m_dwSampleSize, VO_FILE_BEGIN))
		return VO_ERR_SOURCE_END;

	*pPos = (VO_S64)(m_dwCurrIndex * m_dSampleTime);
	return VO_ERR_SOURCE_OK;
}

VO_U32 CAuReader::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Length = 0;
	pHeadData->Buffer = VO_NULL;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAuReader::GetCodecCC(VO_U32* pCC)
{
	*pCC = m_wfe.wFormatTag;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAuReader::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = m_wfe.nChannels;
	pAudioFormat->SampleRate = m_wfe.nSamplesPerSec;
	pAudioFormat->SampleBits = m_wfe.wBitsPerSample;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAuReader::GetBitrate(VO_U32* pdwBitrate)
{
	*pdwBitrate = m_wfe.nAvgBytesPerSec;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAuReader::GetWaveFormatEx(VO_WAVEFORMATEX** ppWaveFormatEx)
{
	*ppWaveFormatEx = &m_wfe;

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CAuReader::ParseFileHeader()
{
	//check if AU format!!
	AUFileHdr hdr;
	read_fcc(hdr.magic);
	if(FOURCC_snd != hdr.magic)
		return VO_FALSE;

	use_big_endian_read

	read_dword(hdr.hdr_size);
	read_dword(hdr.data_size);
	read_dword(hdr.encoding);
	read_dword(hdr.sample_rate);
	read_dword(hdr.channels);

	//Process AUFileHdr
	switch(hdr.encoding)
	{
	case AU_ENCODING_ULAW_8:
		{
			m_wfe.wFormatTag = AudioFlag_MS_MULAW;
			m_wfe.wBitsPerSample = 8;
		}
		break;

	//PCM
	case AU_ENCODING_PCM_8:
	case AU_ENCODING_PCM_16:
	case AU_ENCODING_PCM_24:
	case AU_ENCODING_PCM_32:
		{
			m_wfe.wFormatTag = AudioFlag_MS_PCM;
			m_wfe.wBitsPerSample = (VO_U16)((hdr.encoding - AU_ENCODING_PCM_8 + 1) << 3);
		}
		break;

	//case AU_ENCODING_FLOAT:
	//case AU_ENCODING_DOUBLE:
	//	{
	//		m_wBitsPerSample = (m_Hdr.encoding - AU_ENCODING_FLOAT + 1) << 5;
	//		m_wFormatTag = AudioFlag_IEEE_Float;
	//	}
	//	break;

	//case AU_ENCODING_ADPCM_G721_32:
	//case AU_ENCODING_ADPCM_G722:
	//case AU_ENCODING_ADPCM_G723_24:
	//case AU_ENCODING_ADPCM_G723_40:
	//	break;

	case AU_ENCODING_ALAW_8:
		{
			m_wfe.wFormatTag = AudioFlag_MS_ALAW;
			m_wfe.wBitsPerSample = 8;
		}
		break;

	//Unsupported
	default:
		return VO_FALSE;
	}

	m_wfe.nChannels = (VO_U16)(hdr.channels);
	m_wfe.nSamplesPerSec = hdr.sample_rate;
	m_wfe.nBlockAlign = m_wfe.nChannels * (m_wfe.wBitsPerSample >> 3);
	m_wfe.nAvgBytesPerSec = m_wfe.nSamplesPerSec * (m_wfe.wBitsPerSample >> 3) * m_wfe.nChannels;
	//0.25s
	VO_U32 nBlockAlign = m_wfe.nBlockAlign;
	if(AudioFlag_MS_PCM == m_wfe.wFormatTag)
		nBlockAlign *= 64;
	m_dwSampleSize = (m_wfe.nAvgBytesPerSec / 4 + nBlockAlign - 1) / nBlockAlign * nBlockAlign;

	m_wFileBitsPerSample = m_wfe.wBitsPerSample;
	m_nFileBlockAlign = m_wfe.nBlockAlign;
	m_nFileAvgBytesPerSec = m_wfe.nAvgBytesPerSec;

#ifndef _PCM_OUTPUT_RAWDATA
	if(m_wfe.wBitsPerSample > 16)
	{
		m_wfe.nBlockAlign = m_wfe.nBlockAlign * 16 / m_wfe.wBitsPerSample;
		m_wfe.nAvgBytesPerSec = m_wfe.nAvgBytesPerSec * 16 / m_wfe.wBitsPerSample;
		m_wfe.wBitsPerSample = 16;
	}
#endif	//_PCM_OUTPUT_RAWDATA

	m_ullFileHeadSize = hdr.hdr_size;

	return VO_TRUE;
}