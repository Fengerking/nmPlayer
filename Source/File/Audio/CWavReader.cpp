	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CWavReader.cpp

Contains:	CWavReader class file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#include "CWavReader.h"
#include "fCC.h"

#define LOG_TAG "CWavReader"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CWavReader::CWavReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseAudioReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_pbFormat(VO_NULL)
	, m_extFormatSize(0)
	, m_dwSampleSize(0)
	, m_dwDataSize(0)
	, m_wFileBitsPerSample(0)
	, m_nFileBlockAlign(0)
	, m_nFileAvgBytesPerSec(0)
	, m_dSampleTime(0.0)
	, m_dwCurrIndex(0)
{
}

CWavReader::~CWavReader()
{
	SAFE_MEM_FREE(m_pbFormat);
}

VO_U32 CWavReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseAudioReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	if(!ParseFileHeader())
		return VO_ERR_SOURCE_OPENFAIL;

	m_dwMaxSampleSize = m_dwSampleSize;
	m_dwDuration = VO_U64(m_dwDataSize) * 1000 / m_nFileAvgBytesPerSec;
	m_dSampleTime = double(m_dwSampleSize) * 1000 / m_nFileAvgBytesPerSec;

	m_pBuf = NEW_BUFFER(m_dwSampleSize);

	if (IsDTS() && (((VO_WAVEFORMATEX*)m_pbFormat)->wFormatTag == AudioFlag_MS_PCM))
		((VO_WAVEFORMATEX*)m_pbFormat)->wFormatTag = AudioFlag_DTS;

	if(-1 == FileSeek(m_hFile, m_ullFileHeadSize, VO_FILE_BEGIN))
		return VO_ERR_SOURCE_OPENFAIL;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CWavReader::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	//check data size
	VO_U32 dwCurrDataSize = m_dwCurrIndex * m_dwSampleSize;
	if(dwCurrDataSize >= m_dwDataSize)
		return VO_ERR_SOURCE_END;

	VO_U32 dwSampleSize = m_dwSampleSize;
	if(dwCurrDataSize + dwSampleSize > m_dwDataSize)
		dwSampleSize = m_dwDataSize - dwCurrDataSize;

	VO_S32 nRes = FileRead(m_hFile, m_pBuf, dwSampleSize);
	if(nRes <= 0)
		return (-2 == nRes) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;

	pSample->Buffer = m_pBuf;
	pSample->Size = nRes;

	pSample->Time = (VO_S64)(m_dwCurrIndex * m_dSampleTime);
	pSample->Duration = 1;

	m_dwCurrIndex++;
	return VO_ERR_SOURCE_OK;
}

VO_U32 CWavReader::SetPos(VO_S64* pPos)
{
	m_dwCurrIndex = (VO_U32)(m_nFileAvgBytesPerSec * (*pPos) / (1000 * m_dwSampleSize));
	*pPos = (VO_S64)(m_dwCurrIndex * m_dSampleTime);

	if(-1 == FileSeek(m_hFile, m_ullFileHeadSize + m_dwCurrIndex * m_dwSampleSize, VO_FILE_BEGIN))
		return VO_ERR_SOURCE_END;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CWavReader::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Length = m_extFormatSize;
	pHeadData->Buffer = m_pbFormat;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CWavReader::GetCodecCC(VO_U32* pCC)
{
	// Get the actual coding value when it is a AudioFlag_WAVE_EXT file.
	if (((VO_WAVEFORMATEX*)m_pbFormat)->wFormatTag == AudioFlag_WAVE_EXT) {
		((VO_WAVEFORMATEX*)m_pbFormat)->wFormatTag = (VO_U16)(*(m_pbFormat + VO_WAVEFORMATEX_STRUCTLEN + 6));
	}

	*pCC = ((VO_WAVEFORMATEX*)m_pbFormat)->wFormatTag;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CWavReader::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	VO_WAVEFORMATEX* pwfe = (VO_WAVEFORMATEX*)m_pbFormat;
	pAudioFormat->Channels = pwfe->nChannels;
	pAudioFormat->SampleRate = pwfe->nSamplesPerSec;
	pAudioFormat->SampleBits = pwfe->wBitsPerSample;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CWavReader::GetBitrate(VO_U32* pdwBitrate)
{
	*pdwBitrate = ((VO_WAVEFORMATEX*)m_pbFormat)->nAvgBytesPerSec;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CWavReader::GetWaveFormatEx(VO_WAVEFORMATEX** ppWaveFormatEx)
{
	*ppWaveFormatEx = (VO_WAVEFORMATEX*)m_pbFormat;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CWavReader::DTSSyncCode( const VO_U8 *p_buf )
{
    // 14 bits, little endian version of the bitstream
    if( p_buf[0] == 0xff && p_buf[1] == 0x1f &&
        p_buf[2] == 0x00 && p_buf[3] == 0xe8 &&
        (p_buf[4] & 0xf0) == 0xf0 && p_buf[5] == 0x07 )
    {
        return VO_TRUE;
    }
    // 14 bits, big endian version of the bitstream
    else if( p_buf[0] == 0x1f && p_buf[1] == 0xff &&
             p_buf[2] == 0xe8 && p_buf[3] == 0x00 &&
             p_buf[4] == 0x07 && (p_buf[5] & 0xf0) == 0xf0 )
    {
        return VO_TRUE;
    }
    // 16 bits, big endian version of the bitstream
    else if( p_buf[0] == 0x7f && p_buf[1] == 0xfe &&
             p_buf[2] == 0x80 && p_buf[3] == 0x01 )
    {
        return VO_TRUE;
    }
    // 16 bits, little endian version of the bitstream
    else if( p_buf[0] == 0xfe && p_buf[1] == 0x7f &&
             p_buf[2] == 0x01 && p_buf[3] == 0x80 )
    {
        return VO_TRUE;
    }
    // DTS-HD
    else if( p_buf[0] == 0x64 && p_buf[1] ==  0x58 &&
             p_buf[2] == 0x20 && p_buf[3] ==  0x25 )
    {
        return VO_TRUE;
    }

    return VO_FALSE;
}

VO_BOOL	CWavReader::IsDTS()
{
	if(-1 == FileSeek(m_hFile, m_ullFileHeadSize, VO_FILE_BEGIN))
		return VO_FALSE;

	VO_S32 nRes = FileRead(m_hFile, m_pBuf, m_dwSampleSize);
	VO_U8 *pByte = m_pBuf;
	VO_U8 *pBoundary = m_pBuf + m_dwSampleSize - 6;

	while (!DTSSyncCode(pByte) && pByte < pBoundary) pByte++;

	return (pByte < pBoundary) ? VO_TRUE : VO_FALSE;
}

VO_BOOL CWavReader::ParseFileHeader()
{
	//check if WAVE format!!
	RiffList rlFile;
	read_fcc(rlFile.fcc);
	if(FOURCC_RIFF != rlFile.fcc)
		return VO_FALSE;

	read_dword_l(rlFile.size);
	read_fcc(rlFile.fccListType);
	if(FOURCC_WAVE != rlFile.fccListType)
		return VO_FALSE;

	//find 'fmt ' chunk!!
	RiffChunk rcChunk;
	while(true)
	{
		read_fcc(rcChunk.fcc);
		read_dword_l(rcChunk.size);
		if(FOURCC_fmt == rcChunk.fcc)
			break;
		else
			skip(rcChunk.size);
	}

	if(rcChunk.size < VO_WAVEFORMAT_STRUCTLEN)
		return VO_FALSE;

	VO_U64 ullFmtEndPos = m_chunk.FGetFilePos() + rcChunk.size;
	
	VO_U32 cbSize = rcChunk.size > 18 ? (rcChunk.size - 18) : 0;
	m_extFormatSize = cbSize + sizeof(VO_WAVEFORMATEX);

	m_pbFormat = NEW_BUFFER(m_extFormatSize);
	MemSet(m_pbFormat, 0, m_extFormatSize);

	VO_WAVEFORMATEX* pwfe = (VO_WAVEFORMATEX*)m_pbFormat;
	if (m_chunk.FRead(&pwfe->wFormatTag, 2) == VO_FALSE)
		return VO_FALSE;
	if (m_chunk.FRead(&pwfe->nChannels, 2) == VO_FALSE)
		return VO_FALSE;
	if (m_chunk.FRead(&pwfe->nSamplesPerSec, 4) == VO_FALSE)
		return VO_FALSE;
	if (m_chunk.FRead(&pwfe->nAvgBytesPerSec, 4) == VO_FALSE)
		return VO_FALSE;
	if (m_chunk.FRead(&pwfe->nBlockAlign, 2) == VO_FALSE)
		return VO_FALSE;
	if (m_chunk.FRead(&pwfe->wBitsPerSample, 2) == VO_FALSE)
		return VO_FALSE;
	if (cbSize)
	{
		if (m_chunk.FRead(&pwfe->cbSize, 2) == VO_FALSE)
			return VO_FALSE;
		if (m_chunk.FRead((VO_PBYTE)pwfe+sizeof(VO_WAVEFORMATEX), cbSize) == VO_FALSE)
			return VO_FALSE;
	}
	else
		pwfe->cbSize = 0;

	if (m_chunk.FLocate(ullFmtEndPos) == VO_FALSE)
		return VO_FALSE;
	
	if(pwfe->nChannels >= 10) return VO_FALSE;

	if(AudioFlag_IMA_ADPCM == pwfe->wFormatTag || AudioFlag_MS_ADPCM == pwfe->wFormatTag)	//ADPCM(IMA, MS)
		m_dwSampleSize = pwfe->nBlockAlign;
	else
	{
		VO_U32 nBlockAlign = VO_U32(pwfe->nBlockAlign) * 64;
		m_dwSampleSize = (pwfe->nAvgBytesPerSec / 4 + nBlockAlign - 1) / nBlockAlign * nBlockAlign;
	}

	m_wFileBitsPerSample = pwfe->wBitsPerSample;
	m_nFileBlockAlign = pwfe->nBlockAlign;
	m_nFileAvgBytesPerSec = pwfe->nAvgBytesPerSec;

	// get actual wave data size
	VO_S64 dataSize;
	GetFileSize(&dataSize);

	//find 'data' chunk!!
	while(true)
	{
		read_fcc(rcChunk.fcc);
		read_dword_l(rcChunk.size);
		if(FOURCC_DATA == rcChunk.fcc || FOURCC_data == rcChunk.fcc)
			break;
		else
			skip(rcChunk.size);
	}

	m_ullFileHeadSize = (VO_U32)(m_chunk.FGetFilePos());

	dataSize -= m_ullFileHeadSize;
	if (rcChunk.size > dataSize)
		m_dwDataSize = (VO_U32)dataSize;
	else
		m_dwDataSize = rcChunk.size;

	return VO_TRUE;
}