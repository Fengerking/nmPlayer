	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CMp3Reader.cpp

Contains:	CMp3Reader class file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#include "CMp3Reader.h"
#include "fCC.h"
#include "CID3MetaData.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

DEFINE_USE_MP3_GLOBAL_VARIABLE(CMp3Reader)

#define read_by_size(d, s) {\
	switch(s)\
	{\
	case 1:\
		{\
			VO_U8 b;\
			read_byte(b);\
			d = b;\
		}\
		break;\
	case 2:\
		{\
			VO_U16 w;\
			read_word(w);\
			d = w;\
		}\
		break;\
	case 3:\
		read_3byte_dword(d);\
		break;\
	case 4:\
		read_dword(d);\
		break;\
	default:\
		break;\
	}\
}
CMp3Reader::CMp3Reader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseAudioReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_fpc(&m_memPool)
	, m_Version(MPA_VERSION_Reserved)
	, m_Layer(MPA_LAYER_Reserved)
	, m_ChannelMode(MPA_CHANNEL_Stereo)
	, m_dwSamplesPerSec(0)
	, m_dwSamplesPerFrame(0)
	, m_dwAvgBytesPerSec(0)
	, m_dwFrameSize(0)
	, m_bLSF(VO_TRUE)
	, m_dwFrames(0)
	, m_dSampleTime(0.0)
	, m_dwCurrIndex(0)
{
	MemSet(m_btHeader, 0, sizeof(m_btHeader));

	//const head data!!
	MemSet(m_btHeadData, 0, sizeof(m_btHeadData));
	m_btHeadData[0] = m_btHeadData[8] = 1;
	m_btHeadData[7] = 8;	//block size = 0x800

	m_enDelays.X = m_enDelays.Y = 0;
}

CMp3Reader::~CMp3Reader()
{
	m_fpc.Release();
}

VO_U32 CMp3Reader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseAudioReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	//skip ID3v2 information!!
	while(SkipID3V2Header()) {}

	VO_BOOL bFirst = VO_TRUE;
	VO_U8 btVBRFlag = 0;	// 0, no VBR header; 1, VBR with VBR header; 2 CBR with VBR header
	VO_BYTE btHeader[4];
	VO_U32 dwFrameSize = 0;
	while(GetNextHeader(btHeader))
	{
		if(ParseHeader(btHeader, &dwFrameSize, VO_TRUE))
		{
			// check VBR header first
			if(bFirst)
			{
				bFirst = VO_FALSE;
				VO_U64 ullOldPos = m_chunk.FGetFilePos();
				if(ReadVBRHeader())		//has VBR header, maybe CBR('Info') or VBR('Xing', 'VBRI')
				{
					if(m_fpc.GetCount() > 0)	//VBR
						btVBRFlag = 1;
					else
						btVBRFlag = 2;

					m_chunk.FLocate(m_ullFileHeadSize + 1);
					continue;
				}
				else
					m_chunk.FLocate(ullOldPos);
			}

			/*	AAAAAAAA AAABBCCD EEEEFFGH IIJJKLMM
				11111111 11111111 00001100 10001100	-> 0xFFFF0C8C
			*/
			MemCopy(m_btHeader, btHeader, sizeof(m_btHeader));
			m_btHeader[2] &= 0x0C;
			m_btHeader[3] &= 0x80;///<the copyright maybe changed,so we should not verify it

			m_ullFileHeadSize = (VO_U32)(m_chunk.FGetFilePos() - 4);

			if(dwFrameSize >= 4)
			{
				if(!m_chunk.FSkip(dwFrameSize - 4))
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
			m_chunk.FBack(3);

		m_Version = MPA_VERSION_Reserved;
		m_Layer = MPA_LAYER_Reserved;
	}

	if(MPA_VERSION_Reserved == m_Version || MPA_LAYER_Reserved == m_Layer || !m_dwAvgBytesPerSec)
		return VO_ERR_SOURCE_OPENFAIL;

	if(btVBRFlag == 1)
	{
		m_dwFrameSize = 0;
		if(!m_dwDuration)
			m_dwDuration = (VO_U32)((m_ullFileSize - m_ullFileHeadSize) * 1000 / m_dwAvgBytesPerSec);
	}
	else if(btVBRFlag == 2)
		m_dwFrameSize = dwFrameSize;
	else							//not has VBR header, must CBR file!!
	{
		if (m_ullFileSize == VO_U64(-1)) {
			m_dwFrames = 0;
			m_dwDuration = 0;
		} else {
			m_dwFrames = (VO_U32)((m_ullFileSize - m_ullFileHeadSize) / dwFrameSize);
			// some CBR file is not all same size frame, maybe one is 313, one is 314...
			// East 2010/10/12
			m_dwDuration = (VO_U32)((m_ullFileSize - m_ullFileHeadSize) * 1000 / m_dwAvgBytesPerSec);
		}
		m_dwFrameSize = dwFrameSize;
	}

	if(-1 == FileSeek(m_hFile, m_ullFileHeadSize, VO_FILE_BEGIN))
		return VO_ERR_SOURCE_OPENFAIL;
	
	NewReadSampleBuffer();
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp3Reader::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 rc = ReadSampleContent(pSample);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	pSample->Time = (VO_S64)(m_dwCurrIndex * m_dSampleTime);
	pSample->Duration = 1;

	m_dwCurrIndex++;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp3Reader::SetPos(VO_S64* pPos)
{
	m_dwCurrIndex = (VO_U32)(*pPos / m_dSampleTime);
	m_dwBufLength = m_dwBufReaded = 0;
	if(m_dwFrameSize)	//CBR
	{
		VO_S64 llRes = FileSeek(m_hFile, m_ullFileHeadSize + m_dwCurrIndex * m_dwFrameSize, VO_FILE_BEGIN);
		if(llRes < 0)
			return (-2 == llRes) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;
	}
	else				//VBR
	{
		PFramePos pFramePos = m_fpc.GetFrameByTime((VO_U32)(*pPos));
		VO_S64 llRes = FileSeek(m_hFile, pFramePos->file_pos, VO_FILE_BEGIN);
		if(llRes < 0)
			return (-2 == llRes) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;

		VO_S32 nToSkip = (VO_S32)((*pPos - pFramePos->media_time) / m_dSampleTime);
		if(nToSkip > 0)
		{
			//do actual skip!!
			VO_SOURCE_SAMPLE sample;
			VO_U32 rc = VO_ERR_SOURCE_OK;
			for(VO_S32 i = 0; i < nToSkip && (VO_ERR_SOURCE_OK == rc); i++)
				rc = ReadSampleContent(&sample);
		}
	}
	*pPos = (VO_S64)(m_dwCurrIndex * m_dSampleTime);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp3Reader::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Length = MP3_EXT_DATA_LEN;
	pHeadData->Buffer = m_btHeadData;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp3Reader::GetCodecCC(VO_U32* pCC)
{
	if(MPA_LAYER_1 == m_Layer || MPA_LAYER_2 == m_Layer)
		*pCC = AudioFlag_MP1;
	else
		*pCC = AudioFlag_MP3;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp3Reader::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = (MPA_CHANNEL_Single == m_ChannelMode) ? 1 : 2;
	pAudioFormat->SampleBits = 16;
	pAudioFormat->SampleRate = m_dwSamplesPerSec;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp3Reader::GetBitrate(VO_U32* pdwBitrate)
{
	*pdwBitrate = (m_dwFrameSize > 0 || m_dwDuration == 0) ? m_dwAvgBytesPerSec : (VO_U32)((m_ullFileSize - m_ullFileHeadSize) * 1000 / m_dwDuration);

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CMp3Reader::ParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize, VO_BOOL bInit /* = VO_FALSE */)
{
/*	0		 1		  2		   3
	AAAAAAAA AAABBCCD EEEEFFGH IIJJKLMM
	A 11 (31-21) Frame sync (all bits set) 
	B 2 (20,19) MPEG Audio version ID
	C 2 (18,17) Layer description
	D 1 (16) Protection bit
	E 4 (15,12) Bit rate index
	F 2 (11,10) Sampling rate frequency index (values are in Hz) bits MPEG1 MPEG2 MPEG2.5 
	G 1 (9) Padding bit
	H 1 (8) Private bit. It may be freely used for specific needs of an application, i.e. if it has to trigger some application specific events. 
	I 2 (7,6) Channel Mode
	J 2 (5,4) Mode extension (Only if Joint stereo) 
	K 1 (3) Copyright
	L 1 (2) Original
	M 2 (1,0) Emphasis
*/

	VO_BYTE btIdx = 0;
	VO_U32 dwBitrate = 0;
	if(bInit)
	{
		//version
		if(MPA_VERSION_Reserved == m_Version)
		{
			m_Version = MPA_VERSION((pHeader[1] >> 3) & 0x03);
			if(MPA_VERSION_Reserved == m_Version)
				return VO_FALSE;
		}
		else
		{
			if(m_Version != MPA_VERSION((pHeader[1] >> 3) & 0x03))
				return VO_FALSE;
		}
		m_bLSF = (MPA_VERSION_1 != m_Version) ? VO_TRUE : VO_FALSE;

		//layer
		if(MPA_LAYER_Reserved == m_Layer)
		{
			m_Layer = MPA_LAYER(3 - ((pHeader[1] >> 1) & 0x03));
			if(MPA_LAYER_Reserved == m_Layer)
				return VO_FALSE;
		}
		else
		{
			if(m_Layer != MPA_LAYER(3 - ((pHeader[1] >> 1) & 0x03)))
				return VO_FALSE;
		}

		//Protection bit unused!!

		//Sampling rate
		btIdx = (pHeader[2] >> 2) & 0x03;
		if(0x03 == btIdx)	//reserved
			return VO_FALSE;

		m_dwSamplesPerSec = s_dwSamplingRates[m_Version][btIdx];

		//Private bit unused!!

		//Channel Mode
		m_ChannelMode = MPA_CHANNEL((pHeader[3] >> 6) & 0x03);

		//Mode extension unused!!
		//Copyright unused!!
		//Original unused!!
		//Emphasis unused!!
		m_dwSamplesPerFrame = s_dwSamplesPerFrames[m_bLSF][m_Layer];
		m_dSampleTime = double(m_dwSamplesPerFrame) * 1000 / m_dwSamplesPerSec;

		//some variable parameter
		//bit rate
		btIdx = (pHeader[2] >> 4) & 0x0F;
		if(0x0 == btIdx)	// free
			return VO_FALSE;
		if(0xF == btIdx)	// bad
			return VO_FALSE;

		dwBitrate = s_dwBitrates[m_bLSF][m_Layer][btIdx] * 1000;	//KBit to Bit
		m_dwAvgBytesPerSec = dwBitrate / 8;
	}
	else
	{
		if(!IsHeader(pHeader))
			return VO_FALSE;

		//some variable parameter
		//bit rate
		btIdx = (pHeader[2] >> 4) & 0x0F;
		if(0x0 == btIdx)	// free
			return VO_FALSE;
		if(0xF == btIdx)	// bad
			return VO_FALSE;

		dwBitrate = s_dwBitrates[m_bLSF][m_Layer][btIdx] * 1000;	//KBit to Bit
	}

	if(pdwFrameSize)
	{
		//padding bit
		VO_BYTE btPaddingBit = (pHeader[2] >> 1) & 0x01;

		//frame size;
		if(MPA_LAYER_1 == m_Layer)
			*pdwFrameSize = (12 * dwBitrate / m_dwSamplesPerSec + btPaddingBit) * 4;
		else
			*pdwFrameSize = s_dwCoefficients[m_bLSF][m_Layer] * dwBitrate / m_dwSamplesPerSec + btPaddingBit;
	}

	return VO_TRUE;
}

VO_BOOL CMp3Reader::GetNextHeader(VO_PBYTE pHeader)
{
	//0x11111111 111XXXXX XXXXXXXX XXXXXXXX
	read_pointer(pHeader, 1);
	while(true)
	{
		if(0xFF != pHeader[0])
		{
			read_pointer(pHeader, 1);
			continue;
		}

		read_pointer(pHeader + 1, 1);
		if(0xE0 != (pHeader[1] & 0xE0))
		{
			read_pointer(pHeader, 1);
			continue;
		}
		else
		{
			read_pointer(pHeader + 2, 2);

			return VO_TRUE;
		}
	}
}

VO_PBYTE CMp3Reader::FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen)
{
	if(dwLen < 4) {
		return VO_NULL;
	}

	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwLen - 3;
	while(pHead < pTail)
	{
		if(0xFF != pHead[0])
		{
			pHead++;
			continue;
		}

		//0x11111111 found!!
		if(0xE0 != (pHead[1] & 0xE0))
		{
			pHead += 2;
			continue;
		}

		if (pHead[2] == 0 && pHead[3] == 0) {
			pHead += 4;
			continue;	
		}

		//0x11111111 111XXXXX found!!
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

VO_BOOL CMp3Reader::ReadVBRHeader()
{
	VO_MP3VBRHEADER_TYPE VBRHeaderType = VO_MP3VBRHEADER_NONE;
	if(!CheckVBRHeader(VBRHeaderType))
		return VO_FALSE;

	if(VO_MP3VBRHEADER_XING == VBRHeaderType)
		return ReadXINGHeader(VO_TRUE);
	else if(VO_MP3VBRHEADER_INFO == VBRHeaderType)
		return ReadXINGHeader(VO_FALSE);
	else
		return ReadVBRIHeader();
}

VO_BOOL CMp3Reader::CheckVBRHeader(VO_MP3VBRHEADER_TYPE& HeaderType)
{
	//check XING VBR header!!
	VO_U32 dwVBRHeaderFilePos = m_ullFileHeadSize + s_dwXINGOffsets[m_bLSF][MPA_CHANNEL_Single == m_ChannelMode];
	if(!m_chunk.FLocate(dwVBRHeaderFilePos))
		return VO_FALSE;

	VO_U32 fccID = 0;
	read_fcc(fccID);
	if(FOURCC_Info == fccID)
	{
		HeaderType = VO_MP3VBRHEADER_INFO;

		return VO_TRUE;
	}
	else if(FOURCC_Xing == fccID)
	{
		HeaderType = VO_MP3VBRHEADER_XING;

		return VO_TRUE;
	}

	//check VBRI VBR header!!
	dwVBRHeaderFilePos = m_ullFileHeadSize + 32 + MPA_HEADER_SIZE;
	if(!m_chunk.FLocate(dwVBRHeaderFilePos))
		return VO_FALSE;

	read_fcc(fccID);
	if(FOURCC_VBRI == fccID)
	{
		HeaderType = VO_MP3VBRHEADER_VBRI;

		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_BOOL CMp3Reader::ReadXINGHeader(VO_BOOL bIsRealVBR)
{
	//////////////////////////////////////////////////////////////////////////
	// XING VBR-Header
	// size	description
	// 4	'Xing' or 'Info'('Info' header is also added to CBR files)
	// 4	flags (indicates which fields are used)
	// 4	frames (optional)
	// 4	bytes (optional)
	// 100	TOC (optional)
	// 4	a VBR quality indicator: 0=best 100=worst (optional)
	//////////////////////////////////////////////////////////////////////////
	use_big_endian_read

	VO_U32 dwFlags = 0, dwBytes = 0;

	read_dword(dwFlags);

	//read frames!!
	//not include VRB header frame!!
	if(dwFlags & XING_FRAMES_FLAG)
		read_dword(m_dwFrames);

	//read bytes!!
	//not include ID3v2 information but include VBR header frame!!
	if(dwFlags & XING_BYTES_FLAG)
		read_dword(dwBytes);

	if(bIsRealVBR)
	{
		m_dwDuration = (VO_U32)(m_dwFrames * m_dSampleTime);
		if(dwFlags & XING_TOC_FLAG)
		{
			VO_BYTE btValue = 0;
			for(VO_U8 i = 0; i < 100; i++)
			{
				read_byte(btValue);
				m_fpc.Add(m_ullFileHeadSize + dwBytes * btValue / 255, m_dwDuration * i / 100);
			}
		}
		else	//no TOC(table-of_content)
		{
			for(VO_U8 i = 0; i < 100; i++)
				m_fpc.Add(m_ullFileHeadSize + dwBytes * i / 100, m_dwDuration * i / 100);
		}
	}
	else	//'Info'
	{
		m_dwDuration = (VO_U32)((m_ullFileSize - m_ullFileHeadSize) * 1000 / m_dwAvgBytesPerSec);
		if(dwFlags & XING_TOC_FLAG)
			skip(100);
	}

	//skip quality!!
	if(dwFlags & XING_BYTES_FLAG)
		skip(4);

	dwFlags = 0;
	read_fcc(dwFlags);
	if (dwFlags == FOURCC_LAME || dwFlags == FOURCC_GOGO) {
		skip(17);
		dwFlags = 0;
		read_3byte_dword(dwFlags);
		m_enDelays.X = dwFlags >> 12;
		m_enDelays.Y = dwFlags & 0xFFF;
	}

	return VO_TRUE;
}

VO_BOOL CMp3Reader::ReadVBRIHeader()
{
	//////////////////////////////////////////////////////////////////////////
	//FhG VBRI Header
	//size	description
	//4		'VBRI' (ID)
	//2		version
	//2		delay
	//2		quality
	//4		# bytes
	//4		# frames
	//2		table size (for TOC)
	//2		table scale (for TOC)
	//2		size of table entry (max. size = 4 byte (must be stored in an integer))
	//2		frames per table entry
	//??	dynamic table consisting out of frames with size 1-4
	//		whole length in table size! (for TOC)
	//////////////////////////////////////////////////////////////////////////
	use_big_endian_read

	VO_U16 wTableSize = 0, wTableScale = 0, wBytesPerEntry = 0, wFramesPerEntry = 0;
	//skip version, 2
	//skip delay, 2
	//skip quality, 2
	//skip bytes, 4
	skip(10);

	read_dword(m_dwFrames);
	m_dwDuration = (VO_U32)(m_dwFrames * m_dSampleTime);

	read_word(wTableSize);
	read_word(wTableScale);
	read_word(wBytesPerEntry);
	read_word(wFramesPerEntry);

	VO_U32 dwBytes = 0, dwTableEntry = 0;
	for(VO_U16 i = 0; i <= wTableSize; i++)
	{
		m_fpc.Add(m_ullFileHeadSize + dwBytes, (VO_U32)(i * wFramesPerEntry * m_dSampleTime));

		read_by_size(dwTableEntry, wBytesPerEntry);
		dwBytes += (dwTableEntry * wTableScale);
	}

	return VO_TRUE;
}

VO_BOOL CMp3Reader::IsHeader(VO_PBYTE pHeader)
{
	/*	AAAAAAAA AAABBCCD EEEEFFGH IIJJKLMM
	11111111 11111111 00001100 10001000	-> 0xFFFF0C80
	*/
	return (!MemCompare(m_btHeader, pHeader, 2) && m_btHeader[2] == (pHeader[2] & 0x0C) && m_btHeader[3] == (pHeader[3] & 0x80)) ? VO_TRUE : VO_FALSE;
}

VO_BOOL CMp3Reader::ReadSampleFromBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, VO_U32& dwReaded, VO_U32& dwSampleStart, VO_U32& dwSampleEnd, VO_BOOL* pbSync)
{
	if(dwLen < 16) {
		return VO_FALSE;
	}

	VO_U32 readBytes;		// actual read bytes from file.
	VO_PBYTE pHeader = FindHeadInBuffer(pBuffer, dwLen);
	while (!pHeader) {		// try to skip noise data in the file.
		if (m_chunk.FGetFilePos() >= m_ullFileSize)
			return VO_FALSE;
		if (!m_chunk.FRead3(pBuffer, dwLen, &readBytes))
			return VO_FALSE;
		pHeader = FindHeadInBuffer(pBuffer, dwLen);
	}

	VO_U32 dwFrameSize = 0;
	if(!ParseHeader(pHeader, &dwFrameSize)) {
		return VO_FALSE;
	}

	if(dwFrameSize > dwLen - (pHeader - pBuffer)) {
		return VO_FALSE;
	}

	dwSampleStart = pHeader - pBuffer;
	dwReaded = dwSampleEnd = dwSampleStart + dwFrameSize;

	return VO_TRUE;
}

VO_U32 CMp3Reader::InitMetaDataParser()
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

VO_U32 CMp3Reader::GetMediaTimeByFilePos(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	if(!m_dwFrameSize)	//VBR
		return VO_ERR_NOT_IMPLEMENT;

	pParam->llMediaTime = (pParam->llFilePos < (VO_S64)(m_ullFileHeadSize)) ? 0 : 
		(VO_S64)((pParam->llFilePos - m_ullFileHeadSize) / m_dwFrameSize * m_dSampleTime);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp3Reader::GetFilePosByMediaTime(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	if(!m_dwFrameSize)	//VBR
		return VO_ERR_NOT_IMPLEMENT;

	pParam->llFilePos = (VO_U32)(m_ullFileHeadSize + (pParam->llMediaTime / m_dSampleTime) * m_dwFrameSize);

	return VO_ERR_SOURCE_OK;
}
