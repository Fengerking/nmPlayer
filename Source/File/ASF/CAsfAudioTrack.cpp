#include "CAsfAudioTrack.h"
#include "fCC.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

CAsfAudioTrack::CAsfAudioTrack(VO_U8 btStreamNum, VO_U32 dwDuration, CAsfReader *pReader, VO_MEM_OPERATOR* pMemOp)
	: CAsfTrack(VOTT_AUDIO, btStreamNum, dwDuration, pReader, pMemOp)
{
}

CAsfAudioTrack::~CAsfAudioTrack()
{
}

VO_U32 CAsfAudioTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	VO_U32 uFourcc = 0;
	VO_U32 nRes = GetCodecCC(&uFourcc);
	if (VO_ERR_SOURCE_OK != nRes)
		return nRes;
	else if(uFourcc == AudioFlag_MPEG_RAW_AAC)
		return VO_ERR_SOURCE_OK;

	pHeadData->Length = ((VO_WAVEFORMATEX*)m_pPropBuffer)->cbSize;
	pHeadData->Buffer = m_pPropBuffer + VO_WAVEFORMATEX_STRUCTLEN;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfAudioTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	if(m_lMaxSampleSize <= 0)
	{
		VO_WAVEFORMATEX* pWaveFormatEx = (VO_WAVEFORMATEX*)m_pPropBuffer;
		m_lMaxSampleSize = pWaveFormatEx->nAvgBytesPerSec;
		if(m_lMaxSampleSize < pWaveFormatEx->nBlockAlign)
			m_lMaxSampleSize = pWaveFormatEx->nBlockAlign;
	}

	if(pdwMaxSampleSize)
		*pdwMaxSampleSize = m_lMaxSampleSize;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfAudioTrack::GetCodecCC(VO_U32* pCC)
{
	*pCC = ((VO_WAVEFORMATEX*)m_pPropBuffer)->wFormatTag;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfAudioTrack::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	VO_WAVEFORMATEX* pwfe = (VO_WAVEFORMATEX*)m_pPropBuffer;
	pAudioFormat->Channels = pwfe->nChannels;
	pAudioFormat->SampleBits = pwfe->wBitsPerSample;
	pAudioFormat->SampleRate = pwfe->nSamplesPerSec;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAsfAudioTrack::GetWaveFormatEx(VO_WAVEFORMATEX** ppWaveFormatEx)
{
	*ppWaveFormatEx = (VO_WAVEFORMATEX*)m_pPropBuffer;

	return VO_ERR_SOURCE_OK;
}
