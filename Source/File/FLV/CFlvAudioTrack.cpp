#include "CFlvReader.h"
#include "CFlvAudioTrack.h"
#include "fCC.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

CFlvAudioTrack::CFlvAudioTrack(VO_U8 btStreamNum, VO_U32 dwDuration, PFlvAudioInfo pAudioInfo, CFlvReader* pReader, VO_MEM_OPERATOR* pMemOp)
	: CFlvTrack(VOTT_AUDIO, btStreamNum, dwDuration, pReader, pMemOp)
	, m_pAudioInfo(pAudioInfo)
	, m_pWaveFormatEx(VO_NULL)
{
	m_lMaxSampleSize = m_pAudioInfo->dwBytesPerSec < 10 ? 1000 : m_pAudioInfo->dwBytesPerSec ;
	if(!m_lMaxSampleSize)
		m_lMaxSampleSize = m_pReader->GetFileBitrate();

	VO_U32 codecCC = 0;
	VO_U32 dwRet = GetCodecCC(&codecCC);
	if (0 != dwRet)
		return ;

	if (AudioFlag_MS_PCM == codecCC && m_lMaxSampleSize)
		m_pAudioInfo->nChannels		= static_cast<VO_U16>(m_lMaxSampleSize / m_pAudioInfo->nSamplesPerSec / 2);
	else
		m_pAudioInfo->nChannels		= m_pAudioInfo->nChannels;
}

CFlvAudioTrack::~CFlvAudioTrack()
{
	if (m_pWaveFormatEx)
		delete m_pWaveFormatEx;
}

VO_U32 CFlvAudioTrack::GetCodecCC(VO_U32* pCC)
{
	switch(m_pAudioInfo->btCodecID)
	{
	case 0:	//uncompressed
		*pCC = AudioFlag_MS_PCM;
		break;

	case 1:	//ADPCM
		*pCC = AudioFlag_IMA_ADPCM;
		break;

	case 2:	//MP3
		*pCC = AudioFlag_MP3;
		break;

	case 10:
		*pCC = AudioFlag_AAC;
		break;
	case 11:
		*pCC = AudioFlag_ADTS_AAC;
		break;

	case 5:	//Nellymoser 8kHz mono
	case 6:	//Nellymoser
	default:
		*pCC = 0;
		break;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvAudioTrack::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = m_pAudioInfo->nChannels;
	pAudioFormat->SampleBits = m_pAudioInfo->wBitsPerSample;
	pAudioFormat->SampleRate = m_pAudioInfo->nSamplesPerSec;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvAudioTrack::GetBitrate(VO_U32* pdwBitrate)
{
	if(pdwBitrate)
		*pdwBitrate = m_pAudioInfo->dwBytesPerSec;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvAudioTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Buffer = m_pAudioInfo->ptr_header_data;
	pHeadData->Length = m_pAudioInfo->header_size;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvAudioTrack::GetWaveFormatEx(VO_WAVEFORMATEX** ppWaveFormatEx)
{
	if (!m_pWaveFormatEx)
	{
		VO_PBYTE tmp = new VO_BYTE[VO_WAVEFORMATEX_STRUCTLEN + 1];
		memset(tmp, 0, VO_WAVEFORMATEX_STRUCTLEN + 1);
		m_pWaveFormatEx = (VO_WAVEFORMATEX*)tmp;

		VO_U32 codecCC = 0;
		VO_U32 dwRet = GetCodecCC(&codecCC);
		if (0 != dwRet)
			return dwRet;

		//m_pWaveFormatEx->wFormatTag		= codecCC;
		m_pWaveFormatEx->nChannels			= m_pAudioInfo->nChannels;
		m_pWaveFormatEx->nSamplesPerSec		= m_pAudioInfo->nSamplesPerSec;
		m_pWaveFormatEx->nAvgBytesPerSec	= m_pAudioInfo->dwBytesPerSec;
		m_pWaveFormatEx->nBlockAlign		= 1;
		m_pWaveFormatEx->wBitsPerSample		= m_pAudioInfo->wBitsPerSample;
		m_pWaveFormatEx->cbSize				= 1;

		if (m_pAudioInfo->header_size)
			memcpy(tmp + VO_WAVEFORMATEX_STRUCTLEN, m_pAudioInfo->ptr_header_data, 1);
	}

	*ppWaveFormatEx = m_pWaveFormatEx;

	return VO_ERR_SOURCE_OK;
}