	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CAacReader.cpp

Contains:	CAacReader class file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#include "CAacReader.h"
#include "fCC.h"
#include "CID3MetaData.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


DEFINE_USE_AAC_GLOBAL_VARIABLE(CAacReader)

#define PARSE_BLOCK_SIZE		0x8000		//	32KB

CAacReader::CAacReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseAudioReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_bADIF(VO_FALSE)
	, m_dwAvgBytesPerSec(0)
	, m_pHeadData(VO_NULL)
	, m_dwHeadData(0)
	, m_fpc(&m_memPool)
	, m_btChannels(0)
	, m_btSampleRateIndex(0xF)
	, m_dSampleTime(0.0)
	, m_dwCurrIndex(0)
{
	MemSet(m_btHeader, 0, sizeof(m_btHeader));
}

CAacReader::~CAacReader()
{
	m_fpc.Release();
	SAFE_MEM_FREE(m_pHeadData);
}

VO_U32 CAacReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseAudioReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	//check if ADIF file
	m_bADIF = VO_FALSE;
	VO_U32 fccHeader = 0;
	if(m_chunk.FRead(&fccHeader, 4) && FOURCC_ADIF == fccHeader)
		m_bADIF = VO_TRUE;

	if(m_bADIF)
	{
		if(!ReadADIFHeader())
			return VO_ERR_SOURCE_OPENFAIL;

		m_ullFileHeadSize = (VO_U32)(m_chunk.FGetFilePos());

		m_dwHeadData = (VO_U16)(m_ullFileHeadSize);
		m_pHeadData = NEW_BUFFER(m_dwHeadData);
		if(!m_pHeadData)
			return VO_ERR_OUTOF_MEMORY;
		m_chunk.FLocate(0);
		read_pointer(m_pHeadData, m_dwHeadData);

		m_dwDuration = (VO_U32)((m_ullFileSize - m_ullFileHeadSize) * 1000 / m_dwAvgBytesPerSec);
	}
	else
	{
		//return to file header!!
		m_chunk.FLocate(0);

		//skip ID3v2 information!!
		while(SkipID3V2Header()) {}

		VO_BYTE btHeader[6];
		VO_U32 dwFrameSize = 0;
		while(GetNextHeader(btHeader))
		{
			if(ParseHeader(btHeader, &dwFrameSize, VO_TRUE))
			{
				/*	AAAAAAAA AAAABCCD EEFFFFGH HHIJKLMM
					11111111 11111111 11111111 11110000
				*/
				MemCopy(m_btHeader, btHeader, 4);
				m_btHeader[3] &= 0xF0;

				m_ullFileHeadSize = (VO_U32)(m_chunk.FGetFilePos() - 6);

				if(dwFrameSize >= 6)
				{
					if(!m_chunk.FSkip(dwFrameSize - 6))
						break;

					//check next header!!
					if(!m_chunk.FRead(btHeader, sizeof(btHeader)))
						break;

					if(IsHeader(btHeader))	//this header is what we want to find!!
						break;
				}

				m_chunk.FLocate(m_ullFileHeadSize + 1);
			}
			else
				m_chunk.FBack(5);
		}

		if(VO_SOURCE_OPENPARAM_FLAG_OPENPD == (nSourceOpenFlags & 0xFF) && m_ullFileSize > 2*1024*1024)
			m_dwDuration = (VO_U32)((m_ullFileSize - m_ullFileHeadSize) * m_dSampleTime / dwFrameSize);
		else
		{
			DoScanFile((nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_EXACTSCAN) ? VO_TRUE : VO_FALSE);
			m_dwDuration = (VO_U32)(m_fpc.GetCount() * m_dSampleTime);
		}

		m_dwAvgBytesPerSec = (VO_U32)((m_ullFileSize - m_ullFileHeadSize) * 1000 / m_dwDuration);
	}
	
	if(-1 == FileSeek(m_hFile, m_ullFileHeadSize, VO_FILE_BEGIN))
		return VO_ERR_SOURCE_OPENFAIL;

	NewReadSampleBuffer();

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAacReader::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 rc = ReadSampleContent(pSample);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	pSample->Time = (VO_S64)(m_dwCurrIndex * m_dSampleTime);
	pSample->Duration = 1;

	m_dwCurrIndex++;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAacReader::SetPos(VO_S64* pPos)
{
	if(m_bADIF)
	{
		//only can seek to 0!!
		if(0 != *pPos || FileSeek(m_hFile, m_ullFileHeadSize, VO_FILE_BEGIN) < 0)
		{
			return VO_ERR_SOURCE_SEEKFAIL;
		}

		m_dwBufReaded = 0;
		m_dwBufLength = 0;

		return VO_ERR_SOURCE_OK;
	}

	m_dwBufLength = m_dwBufReaded = 0;
	PFramePos pFramePos = m_fpc.GetFrameByTime((VO_U32)(*pPos));
	if (!pFramePos) return VO_ERR_SOURCE_SEEKFAIL;
	VO_S64 llRes = FileSeek(m_hFile, pFramePos->file_pos, VO_FILE_BEGIN);
	if(llRes < 0)
	{
		return (-2 == llRes) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;
	}

	m_dwCurrIndex = (VO_U32)(*pPos / m_dSampleTime);
	*pPos = (VO_S64)(m_dwCurrIndex * m_dSampleTime);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAacReader::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Buffer = m_pHeadData;
	pHeadData->Length = m_dwHeadData;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAacReader::GetCodecCC(VO_U32* pCC)
{
	*pCC = m_bADIF ? AudioFlag_ADIF_AAC : AudioFlag_ADTS_AAC;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAacReader::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = m_btChannels;
	pAudioFormat->SampleRate = s_dwSamplingRates[m_btSampleRateIndex];
	pAudioFormat->SampleBits = 16;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CAacReader::GetBitrate(VO_U32* pdwBitrate)
{
	*pdwBitrate = m_dwAvgBytesPerSec;

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CAacReader::ReadSampleFromBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, VO_U32& dwReaded, VO_U32& dwSampleStart, VO_U32& dwSampleEnd, VO_BOOL* pbSync)
{
	if(m_bADIF)
	{
		dwSampleStart = 0;
		dwSampleEnd = dwReaded = ((dwLen < m_dwMaxSampleSize) ? dwLen : m_dwMaxSampleSize);
	}
	else
	{
		//make sure data is enough for reading the head
		if(dwLen < 6)
			return VO_FALSE;

		VO_U32 dwFrameSize = 0;
		dwSampleStart = 0;
		if(!ParseHeader(pBuffer, &dwFrameSize))
		{
			VO_PBYTE pHeader = FindHeadInBuffer(pBuffer, dwLen);
			if(!pHeader)
				return VO_FALSE;

			dwSampleStart = pHeader - pBuffer;
			if(!ParseHeader(pHeader, &dwFrameSize))
				return VO_FALSE;
		}

		if(dwFrameSize > dwLen)
			return VO_FALSE;

		dwSampleEnd = dwReaded = (dwSampleStart + dwFrameSize);
	}

	return VO_TRUE;
}

VO_BOOL CAacReader::ReadADIFHeader()
{
	VO_U8 bTmpData = 0;
	VO_U8 bTmpLen = 0;

	VO_U32 dwValue = 0;

	//copyright_id_present, 1 bit
	read_bits(dwValue, 1);
	if(dwValue)
		skip_bits(72);	//copyright_id, 72 bits

	//original_copy, 1 bit
	//home, 1 bit
	skip_bits(2);

	//bitstream_type, 1 bit
	read_bits(dwValue, 1);
	VO_U8 btBitstreamType = (VO_U8)(dwValue);

	//bitrate, 23 bits
	read_bits(m_dwAvgBytesPerSec, 23);
	m_dwAvgBytesPerSec /= 8;	//bit -> byte!!

	//num_program_config_elements, 4 bits
	read_bits(dwValue, 4);
	VO_U8 btProgramConfigs = (VO_U8)(dwValue);
	for(VO_U8 i = 0; i <= btProgramConfigs; i++)
	{
		if(!btBitstreamType)
			skip_bits(20);	//adif_buffer_fullness, 20 bits

		//element_instance_tag, 4 bits
		//profile, 2 bits
		skip_bits(6);

		//sampling_frequency_index, 4 bits
		read_bits(dwValue, 4);
		m_btSampleRateIndex = (VO_U8)(dwValue);

		//num_front_channel_elements, 4 bits
		read_bits(dwValue, 4);
		VO_U8 btFrontChannels = (VO_U8)(dwValue);

		//num_side_channel_elements, 4 bits
		read_bits(dwValue, 4);
		VO_U8 btSideChannels = (VO_U8)(dwValue);

		//num_back_channel_elements, 4 bits
		read_bits(dwValue, 4);
		VO_U8 btBackChannels = (VO_U8)(dwValue);

		//num_lfe_channel_elements, 2 bits
		read_bits(dwValue, 2);
		VO_U8 btLfeChannels = (VO_U8)(dwValue);

		//num_assoc_data_elements, 3 bits
		read_bits(dwValue, 3);
		VO_U8 btAssocDatas = (VO_U8)(dwValue);

		//num_valid_cc_elements, 4 bits
		read_bits(dwValue, 4);
		VO_U8 btValidCCs = (VO_U8)(dwValue);

		//mono_mixdown_present, 1 bit
		read_bits(dwValue, 1);
		if(dwValue)
			skip_bits(4);	//mono_mixdown_element_number, 4 bits

		//stereo_mixdown_present, 1 bit
		read_bits(dwValue, 1);
		if(dwValue)
			skip_bits(4);	//stereo_mixdown_element_number, 4 bits

		//matrix_mixdown_idx_present, 1 bit
		read_bits(dwValue, 1);
		if(dwValue)
			skip_bits(3);	//(matrix_mixdown_idx, 2 bits) && (pseudo_surround_enable, 1 bit)

		m_btChannels = 0;
		for(VO_U32 i = 0; i < btFrontChannels; i++)
		{
			//front_element_is_cpe, 1 bit
			read_bits(dwValue, 1);
			//front_element_tag_select, 4 bits
			skip_bits(4);

			if(dwValue)
				m_btChannels += 2;
			else
				m_btChannels++;
		}

		for(VO_U32 i = 0; i < btSideChannels; i++)
		{
			//side_element_is_cpe, 1 bit
			read_bits(dwValue, 1);
			//side_element_tag_select, 4 bits
			skip_bits(4);

			if(dwValue)
				m_btChannels += 2;
			else
				m_btChannels++;
		}

		for(VO_U32 i = 0; i < btBackChannels; i++)
		{
			//back_element_is_cpe, 1 bit
			read_bits(dwValue, 1);
			//back_element_tag_select, 4 bits
			skip_bits(4);

			if(dwValue)
				m_btChannels += 2;
			else
				m_btChannels++;
		}

		for(VO_U32 i = 0; i < btLfeChannels; i++)
		{
			//lfe_element_tag_select, 4 bits
			skip_bits(4);
			m_btChannels++;
		}

		//assoc_data_element_tag_select, 4 bits
		skip_bits(4 * btAssocDatas);
		//(cc_element_is_ind_sw, 1 bit) && (valid_cc_element_tag_select, 4 bits)
		skip_bits(5 * btValidCCs);

		//byte_alignment
		bTmpLen = 0;

		//comment_field_bytes, 1 byte
		VO_U8 btCommentFieldBytes = 0;
		read_byte(btCommentFieldBytes);
		skip(btCommentFieldBytes);	//comment_field_data
	}

	return VO_TRUE;
}

VO_BOOL CAacReader::ParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize, VO_BOOL bInit /* = VO_FALSE */)
{
	/*	0		 1		  2		   3        4        5        6
	AAAAAAAA AAAABCCD EEFFFFGH HHIJKLMM MMMMMMMM MMMNNNNN NNNNNNOO
	//Fixed Header of ADTS, not change from frame to frame
	A 12	Frame sync (all bits set) 
	B 1		ID, MPEG identifier, set to ¡®1¡¯.
	C 2		Layer, Indicates which layer is used. Set to ¡®00¡¯.
	D 1		Protection absent, Indicates whether error_check() data is present or not.
	E 2		Profile
	F 4		Sampling rate frequency index
	G 1		Private bit
	H 3		Channel configuration 
	I 1		Original/copy
	J 1		home

	//1.2.2	Variable Header of ADTS, maybe change from frame to frame
	K 1		copyright_identification_bit
	L 1		copyright_identification_start
	M 13	frame_length
	N 11	adts_buffer_fullness
	O 2		number_of_raw_data_blocks_in_frame
	*/

	if(bInit)
	{
		//check ID
//		if(!(pHeader[1] & 0x8))
//			return VO_FALSE;

		//check Layer
//		if(pHeader[1] & 0x6)
//			return VO_FALSE;

		//check Profile
		m_btProfile = pHeader[2] >> 6;
		//if(0x3 == m_btProfile)
			//return VO_FALSE;

		//check Sampling rate frequency index
		m_btSampleRateIndex = (pHeader[2] >> 2) & 0xF;
		//if(m_btSampleRateIndex > 0xB)
		//	return VO_FALSE;

		//check Channel configuration
		m_btChannels = s_btChannels[((pHeader[2] << 2) | (pHeader[3] >> 6)) & 0x07];

		m_dSampleTime = double(1024) * 1000 / s_dwSamplingRates[m_btSampleRateIndex];
	}
	else
	{
		if(!IsHeader(pHeader))
			return VO_FALSE;
	}

	if(pdwFrameSize)
	{
		*pdwFrameSize = ((pHeader[3] & 0x03) << 11) | (pHeader[4] << 3) | (pHeader[5] >> 5);
		if(*pdwFrameSize == 0)
			return VO_FALSE;
		if(*pdwFrameSize > m_dwMaxSampleSize)
			m_dwMaxSampleSize = *pdwFrameSize;
	}

	return VO_TRUE;
}

VO_BOOL CAacReader::GetNextHeader(VO_PBYTE pHeader)
{
	//0x11111111 1111XXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX
	read_pointer(pHeader, 1);
	while(true)
	{
		if(0xFF != pHeader[0])
		{
			read_pointer(pHeader, 1);
			continue;
		}

		read_pointer(pHeader + 1, 1);
		if(0xF0 != (pHeader[1] & 0xF0))
		{
			read_pointer(pHeader, 1);
			continue;
		}
		else
		{
			read_pointer(pHeader + 2, 4);

			return VO_TRUE;
		}
	}
}

VO_PBYTE CAacReader::FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen)
{
	if(dwLen < 6)
		return VO_NULL;

	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwLen - 5;
	while(pHead < pTail)
	{
		if(0xFF != pHead[0])
		{
			pHead++;
			continue;
		}

		//0x11111111 found!!
		if(0xF0 != (pHead[1] & 0xF0))
		{
			pHead += 2;
			continue;
		}

		//0x11111111 1111XXXX found!!
		if(ParseHeader(pHead, VO_NULL))
			return pHead;
		else
		{
			if(0xFF == pHead[1])
				pHead++;
			else
				pHead += 2;

			continue;
		}
	}

	return VO_NULL;
}

VO_BOOL CAacReader::IsHeader(VO_PBYTE pHeader)
{
	/*Fixed Header of ADTS, not change from frame to frame
		0		 1		  2		   3        4        5        6
	AAAAAAAA AAAABCCD EEFFFFGH HHIJKLMM MMMMMMMM MMMNNNNN NNNNNNOO
	11111111 11111111 11111111 11110000 00000000 00000000 00000000
	*/

	return (!MemCompare(m_btHeader, pHeader, 3) && m_btHeader[3] == (pHeader[3] & 0xF0)) ? VO_TRUE : VO_FALSE;
}

VO_BOOL CAacReader::DoScanFile(VO_BOOL bExactScan)
{
	VO_PBYTE pFileBuf = NEW_BUFFER(PARSE_BLOCK_SIZE);
	if(!pFileBuf)
		return VO_FALSE;

	VO_U64 ullMediaDataSize = m_ullFileSize - m_ullFileHeadSize;
	VO_U32 dwSkipBlock = ((ullMediaDataSize > 0x80000) && (VO_FALSE == bExactScan)) ? (VO_U32)(ullMediaDataSize / 0xC0000) : 0;

	FileSeek(m_hFile, m_ullFileHeadSize, VO_FILE_BEGIN);

	VO_PBYTE pHeader = VO_NULL;
	VO_U32 dwFileReadPos = m_ullFileHeadSize;
	VO_U32 dwTotalFrames = 0;
	VO_U32 dwReadTimes = 0;

	VO_S32 nReaded = PARSE_BLOCK_SIZE;
	while(nReaded == PARSE_BLOCK_SIZE)	//can read from file!!
	{
		VO_S32 nRes = -2;
		while(-2 == nRes)
			nRes = FileRead(m_hFile, pFileBuf, PARSE_BLOCK_SIZE);

		if(nRes <= 0)
			break;
		nReaded = nRes;

		pHeader = FindHeadInBuffer(pFileBuf, nReaded);
		if(!pHeader)
			continue;

		VO_U32 dwFrameCount = 0;
		VO_U32 dwFrameSize = 0;
		VO_U32 dwStartPos = dwFileReadPos + (pHeader - pFileBuf);
		VO_U32 dwPos = dwStartPos;
		while(true)
		{
			if(!ParseHeader(pHeader, &dwFrameSize))
			{
				if(nReaded > pHeader - pFileBuf)
				{
					pHeader = FindHeadInBuffer(pHeader, nReaded - (pHeader - pFileBuf));
					if(!pHeader || !ParseHeader(pHeader, &dwFrameSize))
						break;
				}
				else
					break;
			}

			if(!m_fpc.Add(dwPos, (VO_U32)(dwTotalFrames * m_dSampleTime)))
				return VO_FALSE;

			dwFrameCount++;
			dwTotalFrames++;

			dwPos += dwFrameSize;
			pHeader += dwFrameSize;

			if(pHeader - pFileBuf >= nReaded - 6)
			{
				dwReadTimes++;
				if(nReaded == PARSE_BLOCK_SIZE)
				{
					if(0 == dwSkipBlock || dwReadTimes < 6 || dwPos + PARSE_BLOCK_SIZE * 6 >= m_ullFileSize)
					{
						pHeader -= dwFrameSize;
						VO_S32 nOffset = dwFrameSize + (pHeader - pFileBuf) - nReaded;
						if(nOffset)
							FileSeek(m_hFile, nOffset, VO_FILE_CURRENT);

						dwFileReadPos += nOffset;
						break;
					}
					else
					{
						VO_U32 dwGuessFrameSize = (dwPos - dwStartPos) / dwFrameCount;
						VO_U32 dwOffsetSize = PARSE_BLOCK_SIZE * dwSkipBlock;
						VO_U32 dwNewFrameCount = (dwOffsetSize - (pHeader - pFileBuf - nReaded)) / dwGuessFrameSize;

						for(VO_U32 i = 0; i < dwNewFrameCount; i++)
						{
							dwPos += dwGuessFrameSize;
							dwTotalFrames++;

							if(!m_fpc.Add(dwPos, (VO_U32)(dwTotalFrames * m_dSampleTime)))
								return VO_FALSE;
						}

						FileSeek(m_hFile, dwOffsetSize, VO_FILE_CURRENT);
						dwFileReadPos += dwOffsetSize;
						break;
					}
				}
				else
					break;
			}
		}
		dwFileReadPos += nReaded;
	}

	MemFree(pFileBuf);

	return VO_TRUE;
}

VO_U32 CAacReader::InitMetaDataParser()
{
	m_pMetaDataParser = new CID3MetaData(m_pFileOp, m_pMemOp);
	if(!m_pMetaDataParser)
		return VO_ERR_OUTOF_MEMORY;

	MetaDataInitInfo initInfo;
	initInfo.eType = VO_METADATA_ID3;
	initInfo.ullFilePositions[0] = VO_MAXU64;
	initInfo.ullFilePositions[1] = VO_MAXU64;
	initInfo.ullFileSize = m_ullFileSize;
	initInfo.dwReserved = 0;
	initInfo.hFile = m_hFile;

	return m_pMetaDataParser->Load(&initInfo);
}

VO_U32 CAacReader::IsNeedScan(VO_BOOL* pIsNeedScan)
{
	*pIsNeedScan = VO_TRUE;

	return VO_ERR_SOURCE_OK;
}
