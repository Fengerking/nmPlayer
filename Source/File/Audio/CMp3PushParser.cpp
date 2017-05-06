
#include "CMp3PushParser.h"
#include "fCC.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

DEFINE_USE_MP3_GLOBAL_VARIABLE(CMp3PushParser)

#define read_pointer(p, l)\
{\
	VO_U32 ret = VO_RET_SOURCE2_OK;\
	if(VO_RET_SOURCE2_OK != (ret = m_hFile.Read(p, l)))\
		return ret;\
}

#define seek_pointer(l,r)\
{\
	VO_U32 ret = VO_RET_SOURCE2_OK;\
	if(VO_RET_SOURCE2_OK != (ret = m_hFile.Seek(l,r)))\
	return ret;\
}

const VO_CHAR* ID3_SUBHEAD_FOR_APPLE[] = {"com.apple.streaming.transportStreamTimestamp", "image/jpeg", "image/png", "image/gif"};
#define 	SUBHEAD_HLS_TIMESTAMP_LENGTH				45
#define 	SUBHEAD_CUSTOMER_PIC_JPEG_LENGTH			14
#define 	SUBHEAD_CUSTOMER_PIC_PNG_LENGTH			13
#define 	SUBHEAD_CUSTOMER_PIC_GIF_LENGTH			13

CMp3PushParser::CMp3PushParser()
: m_bCheckHead(VO_TRUE)
, m_Version(MPA_VERSION_Reserved)
, m_Layer(MPA_LAYER_Reserved)
, m_ChannelMode(MPA_CHANNEL_Stereo)
, m_bLSF(VO_TRUE)
, m_dwSamplesPerSec(0)
, m_dwSamplesPerFrame(0)
, m_dSampleTime(0.0)
, m_dwAvgBytesPerSec(0)
, m_ullTimeStamp(0)
{
	memset(m_btHeader, 0, sizeof(m_btHeader));
	memset(m_btHeadData, 0, sizeof(m_btHeadData));
	m_btHeadData[0] = m_btHeadData[8] = 1;
	m_btHeadData[7] = 8;	//block size = 0x800
	m_pID3Parser = new CID3Parser();
}

CMp3PushParser::~CMp3PushParser()
{
	if(m_pID3Parser){
		delete m_pID3Parser;
	}
}

VO_U32 CMp3PushParser::Init(VO_PTR pSource, VO_U32 nFlag, VO_SOURCE2_INITPARAM * pParam)
{
	CBaseAudioPushParser::Init(pSource, nFlag, pParam);
	if (pParam != NULL)
	{
		VO_SOURCE2_SAMPLECALLBACK *pCallBack = (VO_SOURCE2_SAMPLECALLBACK*)pParam->pInitParam;
		if (pCallBack != NULL)
		{
			m_pID3Parser->Init(pSource, nFlag, pCallBack);
		}
	}
	
	return VO_RET_SOURCE2_OK;
}

VO_U32 CMp3PushParser::UnInit()
{
	CBaseAudioPushParser::Uninit();
	m_pID3Parser->Uninit();
	
	return VO_RET_SOURCE2_OK;
}


VO_VOID CMp3PushParser::Reset()
{
	CBaseAudioPushParser::Reset();
	m_bCheckHead = VO_TRUE;
	if(m_pID3Parser){
		m_pID3Parser->Reset();
	}
	m_ullTimeStamp = 0;
}

VO_U32 CMp3PushParser::SendBuffer(const VO_SOURCE2_SAMPLE& Buffer)
{
	VO_U32 ret = m_hFile.Open(Buffer.pBuffer,Buffer.uSize);
	if (ret != VO_RET_SOURCE2_OK)
	{
		return ret;
	}
	Process();
	m_hFile.Close();
	return ret;
}

VO_U32 CMp3PushParser::ParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize, VO_BOOL bInit /* = VO_FALSE */)
{
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

		memcpy(m_btHeader, pHeader, sizeof(m_btHeader));
		m_btHeader[1] &= 0xFE;///<the protection_bit maybe changed
		m_btHeader[2] &= 0x0C;
		m_btHeader[3] &= 0x80;///<the copyright maybe changed,so we should not verify it
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

VO_BOOL CMp3PushParser::IsHeader(VO_PBYTE pHeader)
{
	/*	AAAAAAAA AAABBCCD EEEEFFGH IIJJKLMM
	11111111 11111111 00001100 10001000	-> 0xFFFF0C80
	*/
	if (m_btHeader[0] == pHeader[0] && m_btHeader[1] == (pHeader[1] & 0xFE))
	{
		if (m_btHeader[2] == (pHeader[2] & 0x0C) && m_btHeader[3] == (pHeader[3] & 0x80))
		{
			return VO_TRUE;
		}	
	}
	return VO_FALSE;
}


VO_U32 CMp3PushParser::ProcessMp3Head()
{
	VO_SOURCE2_SAMPLE 	Sample = {0};
	VO_SOURCE2_TRACK_INFO TrackInfo = {0};

	VOLOGR( "GenerateTheAACHeadData-start");

	Sample.uFlag=VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT;
	Sample.pFlagData= &TrackInfo;

	TrackInfo.uTrackID = 0;
	TrackInfo.uSelInfo = VO_SOURCE2_SELECT_SELECTED;
	*(VO_U32 *)TrackInfo.strFourCC = (MPA_LAYER_1 == m_Layer || MPA_LAYER_2 == m_Layer)?AudioFlag_MP1:AudioFlag_MP3;
	TrackInfo.uTrackType = VO_SOURCE2_TT_AUDIO;
	TrackInfo.uCodec = VO_AUDIO_CodingMP3;

	TrackInfo.sAudioInfo.sFormat.Channels = (MPA_CHANNEL_Single == m_ChannelMode) ? 1 : 2;
	TrackInfo.sAudioInfo.sFormat.SampleBits = 16;
	TrackInfo.sAudioInfo.sFormat.SampleRate = m_dwSamplesPerSec;
	TrackInfo.pHeadData = m_btHeadData;
	TrackInfo.uHeadSize = MP3_EXT_DATA_LEN;
	
	if (m_CallBack.SendData)
	{
		m_CallBack.SendData(m_CallBack.pUserData, VO_SOURCE2_TT_AUDIO, (VO_PTR)&Sample);
	}
	
	
	return VO_RET_SOURCE2_OK;
}
VO_U32 CMp3PushParser::FindMp3Head(tagPushFrameInfo &FraInfo,VO_U64 ullEndPos)
{
	VO_U64 ullCurPos = 0;
	VO_BYTE pHead[4];
	while (1)
	{
		m_hFile.Position(ullCurPos);
		if (ullCurPos > ullEndPos)
		{
			return VO_RET_SOURCE2_OK;
		}
		read_pointer(pHead,1);
		if(0xFF != pHead[0])
		{
			continue;
		}

		read_pointer(pHead+1,1);
		if(0xE0 != (pHead[1] & 0xE0))
		{
			continue;
		}

		read_pointer(pHead+2,2);
		if (pHead[2] == 0 && pHead[3] == 0) {
			continue;	
		}

		//0x11111111 111XXXXX found!!
		VO_U32 uFrameLen = 0;
		if(ParseHeader(pHead, &uFrameLen,m_bCheckHead))
		{
			if (m_bCheckHead)
			{
				m_bCheckHead = VO_FALSE;
				ProcessMp3Head();
			}
			m_hFile.Position(FraInfo.ullFramePos);
			FraInfo.ullFramePos -=4;
			FraInfo.ullFrameLen = uFrameLen;
			break;
		}
		else
		{
			if(0xFF == pHead[1])
			{
				seek_pointer(-3,VO_SOURCE2_IO_POS_CURRENT);
			}
			else
			{
				seek_pointer(-2,VO_SOURCE2_IO_POS_CURRENT);
			}
			continue;
		}
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CMp3PushParser::FindID3Head(tagPushFrameInfo &FraInfo,VO_U64 ullEndPos)
{	
	VO_BYTE pHead[3];
	VO_U64 ullCurPos = 0;
	while(1)
	{
		m_hFile.Position(ullCurPos);
		if (ullCurPos > ullEndPos)
		{
			return VO_RET_SOURCE2_OK;
		}
		read_pointer(pHead,1);
		if(0x49 != pHead[0])
		{
			continue;
		}

		read_pointer(pHead+1,1);
		if(0x44 != pHead[1])
		{
			continue;
		}

		read_pointer(pHead+2,1);
		if(0x33 != pHead[2])
		{
			continue;
		}

		seek_pointer(-3,VO_SOURCE2_IO_POS_CURRENT);
		m_hFile.Position(FraInfo.ullFramePos);
		ID3v2Header ID3Header;
		read_pointer((VO_PBYTE)&ID3Header,sizeof(ID3v2Header));
		VO_U32 uSize = CALCULATE_SIZE_SYNCHSAFE(ID3Header.btSize);

		FraInfo.ullFrameLen = (ID3V2_HEADER_LEN + uSize);
		break;
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CMp3PushParser::ParseMp3Frame(tagPushFrameInfo FraInfo)
{
	VO_SOURCE2_SAMPLE pSample = {0};

	seek_pointer(FraInfo.ullFramePos + FraInfo.ullFrameLen,VO_SOURCE2_IO_POS_BEGIN);

	pSample.uTime = (VO_U64)m_ullTimeStamp;
	pSample.uDuration = (VO_U32)m_dSampleTime;
	pSample.uFlag = VO_FALSE;
	pSample.uSize = FraInfo.ullFrameLen;
	pSample.pBuffer = m_hFile.GetDirectMemory()+FraInfo.ullFramePos;

	if (m_CallBack.SendData)
		m_CallBack.SendData(m_CallBack.pUserData, VO_SOURCE2_TT_AUDIO, (VO_PTR)&pSample);
	
	m_ullTimeStamp += m_dSampleTime;

	return VO_RET_SOURCE2_OK;
}


VO_BOOL CMp3PushParser::CheckID3FrameData(ID3Frame* pID3Frame)
{
	VOLOGR( "CheckID3FrameData");
/*
	if(!memcmp("PRIV", pID3Frame->pFrameHead, sizeof(pID3Frame->pFrameHead)) &&
		!memcmp(ID3_SUBHEAD_FOR_APPLE[SUBHEAD_HLS_TIMESTAMP-SUBHEAD_BASE], pID3Frame->pFrameData, strlen(ID3_SUBHEAD_FOR_APPLE[SUBHEAD_HLS_TIMESTAMP-SUBHEAD_BASE]))){
			VO_U32	length = SUBHEAD_HLS_TIMESTAMP_LENGTH;
//			pID3Frame->nSubHead = SUBHEAD_HLS_TIMESTAMP;
			pID3Frame->pFrameData = pID3Frame->pFrameData + length;
			pID3Frame->nDataLength -= length;
	}
	else if(!memcmp("APIC", pID3Frame->pFrameHead, sizeof(pID3Frame->pFrameHead)) &&
		!memcmp(ID3_SUBHEAD_FOR_APPLE[SUBHEAD_CUSTOMER_PIC_JPEG-SUBHEAD_BASE], (pID3Frame->pFrameData+1), strlen(ID3_SUBHEAD_FOR_APPLE[SUBHEAD_CUSTOMER_PIC_JPEG-SUBHEAD_BASE]))){
			VO_U32	length = SUBHEAD_CUSTOMER_PIC_JPEG_LENGTH;
//			pID3Frame->nSubHead = SUBHEAD_CUSTOMER_PIC_JPEG;
			pID3Frame->pFrameData = pID3Frame->pFrameData + length;
			pID3Frame->nDataLength -= length;
	}
	else if(!memcmp("APIC", pID3Frame->pFrameHead, sizeof(pID3Frame->pFrameHead)) &&
		!memcmp(ID3_SUBHEAD_FOR_APPLE[SUBHEAD_CUSTOMER_PIC_PNG-SUBHEAD_BASE], (pID3Frame->pFrameData+1), strlen(ID3_SUBHEAD_FOR_APPLE[SUBHEAD_CUSTOMER_PIC_PNG-SUBHEAD_BASE]))){
			VO_U32	length = SUBHEAD_CUSTOMER_PIC_PNG_LENGTH;
//			pID3Frame->nSubHead = SUBHEAD_CUSTOMER_PIC_PNG;
			pID3Frame->pFrameData = pID3Frame->pFrameData + length;
			pID3Frame->nDataLength -= length;
	}
	else if(!memcmp("APIC", pID3Frame->pFrameHead, sizeof(pID3Frame->pFrameHead)) &&
		!memcmp(ID3_SUBHEAD_FOR_APPLE[SUBHEAD_CUSTOMER_PIC_GIF-SUBHEAD_BASE], (pID3Frame->pFrameData+1), strlen(ID3_SUBHEAD_FOR_APPLE[SUBHEAD_CUSTOMER_PIC_GIF-SUBHEAD_BASE]))){
			VO_U32	length = SUBHEAD_CUSTOMER_PIC_GIF_LENGTH;
//			pID3Frame->nSubHead = SUBHEAD_CUSTOMER_PIC_GIF;
			pID3Frame->pFrameData = pID3Frame->pFrameData + length;
			pID3Frame->nDataLength -= length;
	}
*/
	return VO_TRUE;
}

VO_U32 CMp3PushParser::ParseID3Frame(tagPushFrameInfo FraInfo)
{
	seek_pointer(FraInfo.ullFramePos + FraInfo.ullFrameLen,VO_SOURCE2_IO_POS_BEGIN);

	VO_PBYTE	pbuffer = m_hFile.GetDirectMemory() + FraInfo.ullFramePos;
	VO_U32		nSize = FraInfo.ullFrameLen;
//	PID3v2Frame  pFrame = NULL;
//	ID3Frame	ID3Frame;

	HEADPOINT Point = {0};
	Point.nLength = nSize;
	Point.nStartPos = 0;
	
	m_pID3Parser->ParseTheID3Data(&Point, pbuffer);
/*
	m_pID3Parser->Reset();
	if(ERR_Valid_Data == m_pID3Parser->Parse(&pbuffer, nSize)){
		if(!m_pID3Parser->FindFirstFrame(&pFrame)){
			return VO_RET_SOURCE2_OK;
		}

		while(pFrame){
			memset(&ID3Frame, 0x0, sizeof(ID3Frame));

			memcpy(ID3Frame.pFrameHead, pFrame->Header.btID, sizeof(pFrame->Header.btID));
			ID3Frame.nDataLength = CALCULATE_SIZE_SYNCHSAFE(pFrame->Header.btSize);
			if(ID3Frame.nDataLength){
				ID3Frame.pFrameData = pFrame->pData;
			}
			CheckID3FrameData(&ID3Frame);
			if (m_CallBack.SendData)
				m_CallBack.SendData(m_CallBack.pUserData, VO_SOURCE2_TT_HINT, (VO_PTR)&ID3Frame);
			
			pFrame = pFrame->pNext;
		}
	}

*/	
	return VO_RET_SOURCE2_OK;
}

VO_U32 CMp3PushParser::Process()
{
	tagPushFrameInfo Mp3Frame;
	tagPushFrameInfo ID3Frame;
	VO_U32 ret = VO_RET_SOURCE2_OK;
	VO_U64 ullPos = 0;
	while (ret == VO_RET_SOURCE2_OK)
	{
		Mp3Frame.ullFramePos = 0x7FFFFFFFFFFFFFFFLL;
		ID3Frame.ullFramePos = 0x7FFFFFFFFFFFFFFFLL;

		m_hFile.Position(ullPos);
		///<find the mp3 head postion
		VO_U32 uRetMp3 = FindMp3Head(Mp3Frame,0x7FFFFFFFFFFFFFFFLL);
		///<seek file to the begining
		m_hFile.Seek(ullPos,VO_SOURCE2_IO_POS_BEGIN);
		///<find the ID3 head postion
		VO_U32 uRetID3 = FindID3Head(ID3Frame,Mp3Frame.ullFramePos);
		if (VO_RET_SOURCE2_OK != uRetMp3 && VO_RET_SOURCE2_OK != uRetID3)
		{
			ret = VO_RET_SOURCE2_NEEDRETRY;
			break;
		}
		if (Mp3Frame.ullFramePos == 0x7FFFFFFFFFFFFFFFLL && ID3Frame.ullFramePos == 0x7FFFFFFFFFFFFFFFLL)
		{
			m_hFile.Size(ullPos);
			break;
		}
		if (Mp3Frame.ullFramePos < ID3Frame.ullFramePos )
		{
			ret = ParseMp3Frame(Mp3Frame);
		}
		else
		{
			ret = ParseID3Frame(ID3Frame);
		}
	}
	m_hFile.Seek(ullPos,VO_SOURCE2_IO_POS_BEGIN);
	return ret;
}
