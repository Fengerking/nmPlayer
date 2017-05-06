	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		DTSReader.cpp

	Contains:	DTS file parser class file.

	Written by:	Rodney Zhang

	Change History (most recent first):
	2012-03-05		Rodney		Create file

*******************************************************************************/

#include "DTSReader.h"
#include "fCC.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CDTSReader::CDTSReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseAudioReader(pFileOp, pMemOp, pLibOP, pDrmCB),
	m_dwBitRate(0),
	m_dwFrames(0),
	m_dwSamples(0),
	m_dwDurationPerFrame(0),
	m_dwFrameIndex(0)

{
	memset(&dtsFormat, 0, sizeof(dtsFormat));
	memset(&dtsFrameHeader, 0, sizeof(DTSFRAMEHEADER));
}

CDTSReader::~CDTSReader(void)
{
}

VO_U32 CDTSReader::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = dtsFormat.Channels;
	pAudioFormat->SampleBits = dtsFormat.SampleBits;
	pAudioFormat->SampleRate = dtsFormat.SampleRate;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CDTSReader::GetBitrate(VO_U32* pdwBitrate)
{
	*pdwBitrate = m_dwBitRate;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CDTSReader::GetCodecCC(VO_U32* pCC)
{
	*pCC = AudioFlag_DTS;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CDTSReader::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Length = 0;
	pHeadData->Buffer = VO_NULL;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CDTSReader::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	if(!FileRead(m_hFile, m_pBuf, m_dwMaxSampleSize))
		return VO_ERR_SOURCE_END;

	pSample->Buffer = m_pBuf;
	pSample->Size = m_dwMaxSampleSize;
	pSample->Time = m_dwFrameIndex * m_dwDurationPerFrame;
	pSample->Duration = m_dwDurationPerFrame;
	m_dwFrameIndex++;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CDTSReader::SetPos(VO_S64* pPos)
{
	if (*pPos < 0 || *pPos >= m_dwDuration)
		return VO_ERR_SOURCE_END;

	m_dwFrameIndex = (VO_U32)(*pPos / m_dwDurationPerFrame);
	VO_S64 dwPos = m_dwFrameIndex * dtsFrameHeader.value.frameByteSize;
	VO_S64 rc = FileSeek(m_hFile, dwPos, VO_FILE_BEGIN);
	if(rc < 0) {
		return (-2 == rc) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CDTSReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseAudioReader::Load(nSourceOpenFlags, pFileSource);
	if (VO_ERR_SOURCE_OK != rc)
		return rc;

	if (!ReadDTSHeader())
		return VO_ERR_SOURCE_OPENFAIL;
	
	m_ullFileHeadSize = (VO_U32)(m_chunk.FGetFilePos());
	m_dwFrames = (VO_U32)(m_ullFileSize / dtsFrameHeader.value.frameByteSize);
	m_dwSamples = m_dwFrames * 32 * dtsFrameHeader.value.nblks;
	m_dwDuration = (m_dwSamples / dtsFormat.SampleRate) * 1000;
	m_dwDurationPerFrame = m_dwDuration / m_dwFrames;
	m_dwMaxSampleSize = dtsFrameHeader.value.frameByteSize;
	m_pBuf = NEW_BUFFER(m_dwMaxSampleSize);

	return VO_ERR_SOURCE_OK;
}


VO_BOOL CDTSReader::ReadDTSHeader()
{
	// check synchronous word
	VO_BYTE syncWord[4];
	if (!m_chunk.FRead(&syncWord, 4)) 
		return VO_FALSE;
	dtsFrameHeader.dwSyncWord = MAKEFOURCC(syncWord[3], syncWord[2], syncWord[1], syncWord[0]);	// big endian
	if (dtsFrameHeader.dwSyncWord != 0x7FFE8001) 
		return VO_FALSE;

	// read frame header and translate it.
	if (!m_chunk.FRead(&dtsFrameHeader.frameHeader, 11)) 
		return VO_FALSE;

	// Get DTS header value.
	dtsFrameHeader.value.frameType = (dtsFrameHeader.frameHeader[0] >> 7) & 0x1;
	dtsFrameHeader.value.deficitSampleCount = (dtsFrameHeader.frameHeader[0] >> 2) & 0x1F;
	dtsFrameHeader.value.crcFlag = (dtsFrameHeader.frameHeader[0] >> 1) & 0x1;
	dtsFrameHeader.value.nblks = ((dtsFrameHeader.frameHeader[0] & 0x1) << 6) | ((dtsFrameHeader.frameHeader[1] >> 2) & 0x3F);
	if (dtsFrameHeader.value.nblks < 5) return VO_FALSE;
	dtsFrameHeader.value.nblks++;
	dtsFrameHeader.value.frameByteSize = ((dtsFrameHeader.frameHeader[1] & 0x3) << 12) | (dtsFrameHeader.frameHeader[2] << 4) | ((dtsFrameHeader.frameHeader[3] >> 4) & 0xF);
	if (dtsFrameHeader.value.frameByteSize < 95) return VO_FALSE;
	dtsFrameHeader.value.frameByteSize++;
	dtsFrameHeader.value.audioMode = ((dtsFrameHeader.frameHeader[3] & 0xF) << 2) | ((dtsFrameHeader.frameHeader[4] >> 6) & 0x3);
	dtsFrameHeader.value.sampleFrequency = (dtsFrameHeader.frameHeader[4] >> 2) & 0xF;
	dtsFrameHeader.value.rate = ((dtsFrameHeader.frameHeader[4] & 0x3) << 3) | ((dtsFrameHeader.frameHeader[5] >> 5) & 0x7);
	dtsFrameHeader.value.mixFlag = (dtsFrameHeader.frameHeader[5] >> 4) & 0x1;
	dtsFrameHeader.value.dynfFlag = (dtsFrameHeader.frameHeader[5] >> 3) & 0x1;
	dtsFrameHeader.value.timeStampFlag = (dtsFrameHeader.frameHeader[5] >> 2) & 0x1;
	dtsFrameHeader.value.auxFlag = (dtsFrameHeader.frameHeader[5] >> 1) & 0x1;
	dtsFrameHeader.value.hdcd = dtsFrameHeader.frameHeader[5] & 0x1;
	dtsFrameHeader.value.extAudioID = (dtsFrameHeader.frameHeader[6] >> 5) & 0x7;
	dtsFrameHeader.value.extAudioFlag = (dtsFrameHeader.frameHeader[6] >> 4) & 0x1;
	dtsFrameHeader.value.aspfFlag = (dtsFrameHeader.frameHeader[6] >> 3) & 0x1;
	dtsFrameHeader.value.lfeFlag = (dtsFrameHeader.frameHeader[6] >> 1) & 0x3;
	dtsFrameHeader.value.hFlag = dtsFrameHeader.frameHeader[6] & 0x1;
	dtsFrameHeader.value.hCRC = (dtsFrameHeader.frameHeader[7] << 8) | dtsFrameHeader.frameHeader[8];
	dtsFrameHeader.value.filts = (dtsFrameHeader.frameHeader[9] >> 7) & 0x1;
	dtsFrameHeader.value.verNum = (dtsFrameHeader.frameHeader[9] >> 3) & 0xF;
	dtsFrameHeader.value.christ = (dtsFrameHeader.frameHeader[9] >> 1) & 0x3;
	dtsFrameHeader.value.pcmSampleBits = ((dtsFrameHeader.frameHeader[9] & 0x1) << 2) | ((dtsFrameHeader.frameHeader[10] >> 6) & 0x3);
	dtsFrameHeader.value.sumFlag = (dtsFrameHeader.frameHeader[10] >> 5) & 0x1;
	dtsFrameHeader.value.sumsFlag = (dtsFrameHeader.frameHeader[10] >> 4) & 0x1;
	dtsFrameHeader.value.dialNorm = dtsFrameHeader.frameHeader[10] & 0xF;

	// Get DTS format.
	switch (dtsFrameHeader.value.audioMode) {
	case 0:
		dtsFormat.Channels = 1;
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		dtsFormat.Channels = 2;
		break;
	case 5:
	case 6:
		dtsFormat.Channels = 3;
		break;
	case 7:
	case 8:
		dtsFormat.Channels = 4;
		break;
	case 9:
		dtsFormat.Channels = 5;
		break;
	case 10:
	case 11:
	case 12:
		dtsFormat.Channels = 6;
		break;
	case 13:
		dtsFormat.Channels = 7;
		break;
	case 14:
	case 15:
		dtsFormat.Channels = 8;
		break;
	default:
		dtsFormat.Channels = 0;
		break;
	}
	if (dtsFormat.Channels == 0)
		return VO_FALSE;

	// Is LFE channel existing?
	switch (dtsFrameHeader.value.lfeFlag) {
	case 1:
	case 2:
		dtsFormat.Channels++;
	}

	switch (dtsFrameHeader.value.sampleFrequency) {
	case 1:
		dtsFormat.SampleRate = 8000;
		break;
	case 2:
		dtsFormat.SampleRate = 16000;
		break;
	case 3:
		dtsFormat.SampleRate = 32000;
		break;
	case 6:
		dtsFormat.SampleRate = 11025;
		break;
	case 7:
		dtsFormat.SampleRate = 22050;
		break;
	case 8:
		dtsFormat.SampleRate = 44100;
		break;
	case 11:
		dtsFormat.SampleRate = 12000;
		break;
	case 12:
		dtsFormat.SampleRate = 24000;
		break;
	case 13:
		dtsFormat.SampleRate = 48000;
		break;
	default:
		dtsFormat.SampleRate = -1;
		break;
	}
	if (dtsFormat.SampleRate == -1)
		return VO_FALSE;

	m_dwBitRate = dtsBitRate[dtsFrameHeader.value.rate];
	// Actual Bit Rate on DVD-Video Discs(Kbit/s).
	switch (dtsFrameHeader.value.rate) {
	case 15:
		m_dwBitRate = 754500;
		break;
	case 24:
		m_dwBitRate = 1509750;
		break;
	}

	switch (dtsFrameHeader.value.pcmSampleBits) {
	case 0:
	case 1:
		dtsFormat.SampleBits = 16;
		break;
	case 2:
	case 3:
		dtsFormat.SampleBits = 20;
		break;
	case 5:
	case 6:
		dtsFormat.SampleBits = 24;
		break;
	default:
		dtsFormat.SampleBits = -1;
		break;
	}
	if (dtsFormat.SampleBits == -1)
		return VO_FALSE;

	return VO_TRUE;
}
