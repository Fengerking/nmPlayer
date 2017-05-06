	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2011			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		PCMRawData.cpp

	Contains:	PCM raw data file parser class file.

	Written by:	Rodney Zhang

	Change History (most recent first):
	2012-02-02		Rodney		Create file

*******************************************************************************/

#include "PCMRawData.h"
#include "fCC.h"
#include "fCodec.h"
#include "voAudio.h"

    #define LOG_TAG "PCMRawData"
    #include "voLog.h"



// public methods
// ----------------------------------------------------------------------------
CPCMRawData::CPCMRawData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseReader(pFileOp, pMemOp, pLibOP, pDrmCB),
	m_dwBytesPerSecond(0),
	m_dwBytesPerSample(0),
	m_dwMaxSampleSize(0),
	m_dwCurrIndex(0),
	m_dwDuration(0),
	m_fileSize(0),
	m_pStreamBuffer(NULL)
{
}

CPCMRawData::~CPCMRawData(void)
{
	if (m_pStreamBuffer) SAFE_MEM_FREE(m_pStreamBuffer);
}

VO_U32 CPCMRawData::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseReader::Load(nSourceOpenFlags, pFileSource);
	if(rc != VO_ERR_SOURCE_OK)
		return rc;

	GetFileSize(&m_fileSize);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CPCMRawData::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 dwReaded;

	memset(m_pStreamBuffer, 0, m_dwMaxSampleSize);
	if (!m_chunk.FRead3(m_pStreamBuffer, m_dwBytesPerSample, &dwReaded)) {
		return VO_ERR_SOURCE_END;
	}
	
	BETOLE(m_pStreamBuffer, dwReaded);

	pSample->Buffer = m_pStreamBuffer;
	pSample->Duration = 1;
	pSample->Size = dwReaded;
	pSample->Time = m_dwCurrIndex * DURATION_PER_SAMPLE_BLOCKS;
	pSample->Flag = 0;
	m_dwCurrIndex++;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CPCMRawData::SetPos(VO_S64* pPos)
{
	if ( *pPos < 0 || *pPos >= m_dwDuration ) return VO_ERR_SOURCE_END;
	
	m_dwCurrIndex = (VO_U32)(*pPos / DURATION_PER_SAMPLE_BLOCKS);
	VO_S64 rc = FileSeek(m_hFile, m_dwCurrIndex * m_dwBytesPerSample, VO_FILE_BEGIN);
	if(rc < 0) {
		return (-2 == rc) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CPCMRawData::SetTrackParameter(VO_U32 uID, VO_PTR pParam)
{
	switch(uID)
	{
	case VO_PID_AUDIO_FORMAT:
		return SetPCMFormat((VO_AUDIO_FORMAT*)pParam);

	default:
		break;
	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CPCMRawData::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = m_PCMFormat.Channels;
	pAudioFormat->SampleBits = m_PCMFormat.SampleBits;
	pAudioFormat->SampleRate = m_PCMFormat.SampleRate;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CPCMRawData::GetTrackParameter(VO_U32 uID, VO_PTR pParam)
{
	switch(uID)
	{
	case VO_PID_AUDIO_FORMAT:
		return GetAudioFormat((VO_AUDIO_FORMAT*)pParam);

	case VO_PID_SOURCE_MAXSAMPLESIZE:
		*(VO_U32*)pParam = m_dwMaxSampleSize;
		return VO_ERR_SOURCE_OK;

	case VO_PID_SOURCE_CODECCC:
		return GetCodecCC((VO_U32*)pParam);

	default:
		break;
	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CPCMRawData::GetTrackInfo(VO_SOURCE_TRACKINFO* pTrackInfo)
{
	if(!pTrackInfo)
		return VO_ERR_INVALID_ARG;

	pTrackInfo->Start = 0;
	pTrackInfo->Duration = m_dwDuration;
	pTrackInfo->Type = VO_SOURCE_TT_AUDIO;

	VO_U32 nRes = GetCodec(&pTrackInfo->Codec);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	pTrackInfo->HeadData = VO_NULL;
	pTrackInfo->HeadSize = 0;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CPCMRawData::GetCodecCC(VO_U32* pCC)
{
	*pCC = AudioFlag_MS_PCM;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CPCMRawData::GetCodec(VO_U32* pCodec)
{
	VO_U32 dwPCMCC = 0;
	VO_U32 nRes = GetCodecCC(&dwPCMCC);
	if(nRes != VO_ERR_SOURCE_OK)
		return nRes;

	if(pCodec)
		*pCodec = fCodecGetAudioCodec((VO_U16)(dwPCMCC));

	return VO_ERR_SOURCE_OK;
}


// protected methods
//-----------------------------------------------------------------------------
// Big Endian To Little Endian.
VO_VOID CPCMRawData::BETOLE(VO_PTR pBEBuffer, VO_U32 dwSize)
{
	VO_BYTE tempByte;
	VO_PBYTE pBuffer = (VO_PBYTE)pBEBuffer;

	for (VO_U32 i = 0; i < dwSize; i += 2) {
		tempByte = pBuffer[i];
		pBuffer[i] = pBuffer[i+1];
		pBuffer[i+1] = tempByte;
	}
}

VO_U32 CPCMRawData::SetPCMFormat(VO_AUDIO_FORMAT* pPCMFormat)
{
	m_PCMFormat.Channels = pPCMFormat->Channels;
	m_PCMFormat.SampleBits = pPCMFormat->SampleBits;
	m_PCMFormat.SampleRate = pPCMFormat->SampleRate;

	m_nTracks = m_PCMFormat.Channels;
	m_dwMaxSampleSize = m_nTracks * (m_PCMFormat.SampleBits / 8);
	m_dwBytesPerSecond = m_PCMFormat.SampleRate * m_dwMaxSampleSize;
	m_dwDuration = (VO_U32)(m_fileSize / m_dwBytesPerSecond) * 1000;
	if (m_fileSize % m_dwBytesPerSecond) m_dwDuration++;

	m_dwBytesPerSample = m_dwBytesPerSecond / (1000 / DURATION_PER_SAMPLE_BLOCKS);
	m_dwBytesPerSample = m_dwBytesPerSample - (m_dwBytesPerSample % m_dwMaxSampleSize);
	m_dwMaxSampleSize = m_dwBytesPerSample;
	m_pStreamBuffer = NEW_BUFFER(m_dwMaxSampleSize);

	return VO_ERR_SOURCE_OK;
}
