	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CQcpReader.cpp

Contains:	CQcpReader class file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#include "CQcpReader.h"
#include "fCC.h"
#include "QcpFileGuid.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

DEFINE_USE_QCP_GLOBAL_VARIABLE(CQcpReader)

CQcpReader::CQcpReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseAudioReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_wFrameSize(0)
	, m_dwDataSize(0)
	, m_fpc(&m_memPool)
	, m_dwCurrIndex(0)
{
	MemSet(&m_Format, 0, sizeof(m_Format));
	MemSet(&m_VariableRate, 0, sizeof(m_VariableRate));
	MemSet(&m_wPacketSizes, 0, sizeof(m_wPacketSizes));
}

CQcpReader::~CQcpReader()
{
	m_fpc.Release();
}

VO_U32 CQcpReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseAudioReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	if(!ParseFileHeader())
		return VO_ERR_SOURCE_OPENFAIL;

	if(!m_wFrameSize && VO_SOURCE_OPENPARAM_FLAG_OPENPD != (nSourceOpenFlags & 0xFF))	//VBR file, we should scan file to get file index!!
		DoScanFile();

	if(-1 == FileSeek(m_hFile, m_ullFileHeadSize, VO_FILE_BEGIN))
		return VO_ERR_SOURCE_OPENFAIL;

	NewReadSampleBuffer();

	return VO_ERR_SOURCE_OK;
}

VO_U32 CQcpReader::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 rc = ReadSampleContent(pSample);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	pSample->Time = m_dwCurrIndex * QCP_SAMPLETIME;
	pSample->Duration = 1;

	m_dwCurrIndex++;

	//VOLOGI("size: 0x%08X, time: %d!!!!!!!!!!!!!!!!!!!!", pSample->Size, pSample->Time);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CQcpReader::SetPos(VO_S64* pPos)
{
	m_dwCurrIndex = (VO_U32)(*pPos / QCP_SAMPLETIME);
	m_dwBufLength = m_dwBufReaded = 0;
	if(m_wFrameSize)	//CBR
	{
		VO_S64 llRes = FileSeek(m_hFile, m_ullFileHeadSize + m_dwCurrIndex * m_wFrameSize, VO_FILE_BEGIN);
		if(llRes < 0)
			return (-2 == llRes) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;
	}
	else				//VBR
	{
		PFramePos pFramePos = m_fpc.GetFrameByTime((VO_U32)(*pPos));
		VO_S64 llRes = FileSeek(m_hFile, pFramePos->file_pos, VO_FILE_BEGIN);
		if(llRes < 0)
			return (-2 == llRes) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;
	}
	*pPos = m_dwCurrIndex * QCP_SAMPLETIME;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CQcpReader::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Buffer = VO_NULL;
	pHeadData->Length = 0;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CQcpReader::GetCodecCC(VO_U32* pCC)
{
	if(voIsEqualGUID(GUID_CODEC_QCELP13, m_Format.guidCodec) || voIsEqualGUID(GUID_CODEC_QCELP13_1, m_Format.guidCodec))
		*pCC = AudioFlag_QCELP;
	else if(voIsEqualGUID(GUID_CODEC_EVRC, m_Format.guidCodec))
		*pCC = AudioFlag_EVRC;
	else
		*pCC = AudioFlag_MS_UNKNOWN;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CQcpReader::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = 1;
	pAudioFormat->SampleBits = m_Format.wBitsPerSample;
	pAudioFormat->SampleRate = m_Format.wSamplesPerSec;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CQcpReader::GetBitrate(VO_U32* pdwBitrate)
{
	*pdwBitrate = VO_S64(m_dwDataSize) * 1000 / m_dwDuration;

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CQcpReader::ParseFileHeader()
{
	//check if QLCM format!!
	VO_U32 MaxFrameLength;

	RiffList rlFile;
	read_fcc(rlFile.fcc);
	if(FOURCC_RIFF != rlFile.fcc)
		return VO_FALSE;

	read_dword_l(rlFile.size);
	read_fcc(rlFile.fccListType);
	if(FOURCC_QLCM != rlFile.fccListType)
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

	//process format information!!
	read_byte(m_Format.btMajorVersion);
	read_byte(m_Format.btMinorVersion);
	read_guid(m_Format.guidCodec);

	if(voIsEqualGUID(GUID_CODEC_QCELP13, m_Format.guidCodec) || voIsEqualGUID(GUID_CODEC_QCELP13_1, m_Format.guidCodec))
	{
		MemCopy(m_wPacketSizes, (VO_PTR)s_wQCPPacketSizes, sizeof(m_wPacketSizes));
		MaxFrameLength = 35;
	}
	else if(voIsEqualGUID(GUID_CODEC_EVRC, m_Format.guidCodec))
	{
		MemCopy(m_wPacketSizes, (VO_PTR)s_wEVCPacketSizes, sizeof(m_wPacketSizes));
		MaxFrameLength = 23;
	}
	else
		return VO_FALSE;

	read_word_l(m_Format.wCodecVersion);
	read_pointer(m_Format.btCodecName, sizeof(m_Format.btCodecName));
	read_word_l(m_Format.wAvgBitsPerSec);
	read_word_l(m_Format.wMaxPacketSize);

	m_dwMaxSampleSize = m_Format.wMaxPacketSize;

	if(MaxFrameLength > m_dwMaxSampleSize)
		m_dwMaxSampleSize = MaxFrameLength;

	read_word_l(m_Format.wSamplesPerBlock);
	read_word_l(m_Format.wSamplesPerSec);
	read_word_l(m_Format.wBitsPerSample);
	skip(QCP_FORMAT_SKIP_SIZE);

	m_VariableRate.dwVariableRate = 1;
	m_VariableRate.dwPacketNum = 0x7FFFFFFF;

	read_fcc(rcChunk.fcc);
	read_dword_l(rcChunk.size);
	while(FOURCC_DATA != rcChunk.fcc && FOURCC_data != rcChunk.fcc)
	{
		if(FOURCC_vrat == rcChunk.fcc)
		{
			read_dword_l(m_VariableRate.dwVariableRate);
			read_dword_l(m_VariableRate.dwPacketNum);
		}
		else
			skip(rcChunk.size);

		read_fcc(rcChunk.fcc);
		read_dword_l(rcChunk.size);
	}

	m_dwDataSize = rcChunk.size;
	m_ullFileHeadSize = (VO_U32)(m_chunk.FGetFilePos());
	if(QCP_PACKETDATA_FIXEDSIZE == m_VariableRate.dwVariableRate)
	{
		m_wFrameSize = m_Format.wMaxPacketSize;
		if(!m_VariableRate.dwPacketNum)
			m_VariableRate.dwPacketNum = m_dwDataSize / m_wFrameSize;
	}
	else
		m_wFrameSize = 0;
	m_dwDuration = m_VariableRate.dwPacketNum * QCP_SAMPLETIME;

	return VO_TRUE;
}

VO_BOOL CQcpReader::DoScanFile()
{
	if(!m_chunk.FLocate(m_ullFileHeadSize))
		return VO_FALSE;

	VO_U32 dwDataPos = m_ullFileHeadSize, dwCount = 0, wPacketSize = 0;
	VO_BYTE btHeader;
	while(dwCount < m_VariableRate.dwPacketNum && dwDataPos < m_dwDataSize + m_ullFileHeadSize)
	{
		if(!m_chunk.FRead(&btHeader, 1))
			break;

		if(!m_fpc.Add(dwDataPos, (dwCount++) * QCP_SAMPLETIME))
			return VO_FALSE;

		wPacketSize = m_wPacketSizes[btHeader];
		if(wPacketSize == 0)
		{
			dwCount--;
			break;
		}

		dwDataPos += wPacketSize;
		if(!m_chunk.FSkip(wPacketSize - 1))
			break;
	}
	
	if(dwCount != m_VariableRate.dwPacketNum)
	{
		m_VariableRate.dwPacketNum = dwCount;
		m_dwDuration = m_VariableRate.dwPacketNum * QCP_SAMPLETIME;
	}

	return VO_TRUE;
}

VO_BOOL CQcpReader::ReadSampleFromBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, VO_U32& dwReaded, VO_U32& dwSampleStart, VO_U32& dwSampleEnd, VO_BOOL* pbSync)
{
	if(dwLen < 1)
		return VO_FALSE;

	VO_U16 wPacketSize = m_wPacketSizes[pBuffer[0]];
	if(wPacketSize > dwLen)
		return VO_FALSE;
	if(wPacketSize == 0)
		return VO_FALSE;

	dwSampleStart = 0;
	dwReaded = dwSampleEnd = wPacketSize;

	return VO_TRUE;
}

VO_U32 CQcpReader::IsNeedScan(VO_BOOL* pIsNeedScan)
{
	*pIsNeedScan = (0 == m_wFrameSize) ? VO_TRUE : VO_FALSE;

	return VO_ERR_SOURCE_OK;
}
