#include "fCC.h"
#include "CMpegAudioTrack.h"
#include "voLog.h"
#include "CMpegReader.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define read_dword2(dw, p)\
{\
	VO_PBYTE pTmp = (VO_PBYTE)&dw;\
	memcpy(pTmp + 3, p, 1);\
	memcpy(pTmp + 2, p + 1, 1);\
	memcpy(pTmp + 1, p + 2, 1);\
	memcpy(pTmp, p + 3, 1);\
}
#if 0
#define read_pointer(p, l)\
{\
	if(!pFileChunk->FRead(p, l))\
	return VO_FALSE;\
}
#endif
DEFINE_USE_MP3_GLOBAL_VARIABLE(CMpegAudioTrack)

CMpegAudioTrack::CMpegAudioTrack(VO_U8 btStreamID, VO_U32 dwTimeOffset, VO_U32 dwDuration, CMpegReader *pReader, VO_MEM_OPERATOR* pMemOp, VO_U8 btSubStreamID)
	: CMpegTrack(VOTT_AUDIO, btStreamID, dwTimeOffset, dwDuration, pReader, pMemOp, btSubStreamID)
	, m_dwSampleRate(0)
	, m_wChannels(0)
	, m_btAudioLayer(0)
	, m_btVersion(0)
{
	ResetSample();
}

CMpegAudioTrack::~CMpegAudioTrack()
{
}

VO_U32 CMpegAudioTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	if(pdwMaxSampleSize)
	{
		VO_U32 CC = 0;
		GetCodecCC(&CC);
		if (CC != AudioFlag_MS_PCM)
		{
			*pdwMaxSampleSize = VO_MAX( (m_dwSampleRate * m_wChannels)/2 , 2048 );///<for non-PCM,too large buffer will waste more time
		}
		else
			*pdwMaxSampleSize = VO_MAX( m_dwSampleRate * m_wChannels * 2 , 2048 );
	}
	m_nSampleSize = (*pdwMaxSampleSize);
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegAudioTrack::GetCodecCC(VO_U32* pCC)
{
	if (m_btStreamNum == PRIVATE_STREAM_1)
	{
		if ((m_btSubStreamNum >= STREAM_TYPE_AUDIO_AC3_MIN_1 && m_btSubStreamNum <= STREAM_TYPE_AUDIO_AC3_MAX_1)
			||(m_btSubStreamNum >= STREAM_TYPE_AUDIO_AC3_MIN_2 && m_btSubStreamNum <= STREAM_TYPE_AUDIO_AC3_MAX_2))
		{
			*pCC = AudioFlag_AC3;
		}
		else if ((m_btSubStreamNum >= STREAM_TYPE_AUDIO_DTS_MIN_1 && m_btSubStreamNum <= STREAM_TYPE_AUDIO_DTS_MAX_1)
			||(m_btSubStreamNum >= STREAM_TYPE_AUDIO_DTS_MIN_2 && m_btSubStreamNum <= STREAM_TYPE_AUDIO_DTS_MAX_2))
		{
			*pCC = AudioFlag_DTS;
		}
		else if (m_btSubStreamNum >= STREAM_TYPE_AUDIO_LPCM_MIN && m_btSubStreamNum <= STREAM_TYPE_AUDIO_LPCM_MAX)
		{
			*pCC = AudioFlag_MS_PCM;
		}
		else
			*pCC = AudioFlag_MS_UNKNOWN; 
	}
	else
	{
		switch (m_btAudioLayer)
		{
		case 1:
		case 2:
			*pCC = AudioFlag_MP1;
			break;
		case 3:
			*pCC = AudioFlag_MP3;
			break;
		case 4:
			*pCC = AudioFlag_ADTS_AAC;
			break;
		default:
			*pCC = AudioFlag_MS_UNKNOWN;
			break;
		}
	}
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegAudioTrack::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = m_wChannels;
	pAudioFormat->SampleBits = 16;
	pAudioFormat->SampleRate = m_dwSampleRate;

	return VO_ERR_SOURCE_OK;
}

VO_VOID CMpegAudioTrack::Flush()
{
	ResetSample();
	CMpegTrack::Flush();
}

VO_U32	CMpegAudioTrack::ParExtentedStream(VO_PBYTE pData, VO_U32 wLen)
{
	VO_U32 nBufPos = 0;
	if ((wLen > 3) &&((m_btSubStreamNum >= STREAM_TYPE_AUDIO_AC3_MIN_1 && m_btSubStreamNum <= STREAM_TYPE_AUDIO_DTS_MAX_1)
		||(m_btSubStreamNum >= STREAM_TYPE_AUDIO_DTS_MIN_2 && m_btSubStreamNum <= STREAM_TYPE_AUDIO_LPCM_MAX)
		||(m_btSubStreamNum >= STREAM_TYPE_AUDIO_AC3_MIN_2 && m_btSubStreamNum <= STREAM_TYPE_AUDIO_AC3_MAX_2)))
	{
		nStreamNum = pData[nBufPos++];
		nFrameCnt = pData[nBufPos++];
		FrameDataOffset = pData[nBufPos++];
		FrameDataOffset <<= 8;
		FrameDataOffset |= pData[nBufPos++];
		if (m_btSubStreamNum >= STREAM_TYPE_AUDIO_LPCM_MIN && m_btSubStreamNum <= STREAM_TYPE_AUDIO_LPCM_MAX)
		{
			if (wLen < 7)
			{
				return 0;
			}
			VO_U8 nPara1;
			nPara1 = pData[nBufPos++];
			LPCMParameter1.emphasis = (nPara1&0x80)>>7;
			LPCMParameter1.mute = (nPara1&0x40)>>6;
			LPCMParameter1.FrmNum = (nPara1&0x1F);

			nPara1 = pData[nBufPos++];
			LPCMParameter2.Quantization = (nPara1&0xC0)>>6; 
			if (LPCMParameter2.Quantization != 0)
			{
				VOLOGI("not supported pcm SampleBits");
			}
			LPCMParameter2.Sample_rate = (nPara1&0x30)>>4;
			LPCMParameter2.channels = (nPara1&0x07);
			LPCMParameter3_Dynamic_Range = pData[nBufPos++];
		}
	}
	return nBufPos;
}

VO_BOOL CMpegAudioTrack::OnPacket(CGFileChunk* pFileChunk, VO_U32 wLen, VO_S64 dwTimeStamp,VO_BOOL beEncrpt)
{
	if (m_bSetExtData && !m_bInUsed)
	{
		return VO_TRUE;
	}
	if(MPEG_MAX_VALUE != m_ullCurrParseFilePos)
	{
		if(pFileChunk->FGetFilePos() < m_ullCurrParseFilePos)
		{
			pFileChunk->FSkip(wLen);
			return VO_TRUE;
		}
		else
			m_ullCurrParseFilePos = -1;
	}
	VO_U32 dwPosInBuffer = VO_MAXU32;

	VO_PBYTE pPESBuffer = pFileChunk->FGetFileContentPtr(wLen);
	if(!pPESBuffer)
	{
		if(!CheckPESBuffer())
			return VO_FALSE;

		if(!pFileChunk->FRead(m_pActPESBuffer, wLen))
			return VO_FALSE;

		pPESBuffer = m_pActPESBuffer;
	}
	if (m_btStreamNum == PRIVATE_STREAM_1 && pPESBuffer[0] != m_btSubStreamNum)
	{
		return VO_TRUE;
	}
	if (beEncrpt)
		m_pReader->DRMData_Widevine_PESData(pPESBuffer,wLen,NULL,NULL);

	if (m_btStreamNum == PRIVATE_STREAM_1)
	{
		VO_U32 ret = 0;
		if(!(ret = ParExtentedStream(pPESBuffer,wLen)))
			return VO_FALSE;
		pPESBuffer += ret;
		wLen -= ret;
	}
	if(!m_bSetExtData)
	{
		VO_U16 i = 0;
		if (m_btStreamNum == PRIVATE_STREAM_1 )
		{
			if(SetExtData(pPESBuffer ,wLen))
			{
				Init(0);
				SetInUsed(VO_TRUE);
				Prepare();

				m_bSetExtData = VO_TRUE;
			}
		}
		else if(wLen > 3)
		{
			while(i < wLen - 4)
			{
				if((pPESBuffer[i] == 0xff) && ((pPESBuffer[i + 1] & 0xf0) == 0xf0))
				{
					if(SetExtData(pPESBuffer + i, 4))
					{
						Init(0);
						SetInUsed(VO_TRUE);
						Prepare();

						m_bSetExtData = VO_TRUE;
					}

					break;
				}
				else
					i++;
			}
		}
		
		if (!m_bSetExtData)
		{
			return VO_TRUE;
		}
		VO_PBYTE pWrite = VO_NULL;
		dwPosInBuffer = m_pGlobeBuffer->GetForDirectWrite(&pWrite, wLen);
		if(VO_MAXU32 == dwPosInBuffer)
			return VO_FALSE;
		m_pGlobeBuffer->DirectWrite2(pPESBuffer, pWrite, 0, wLen);
	}
	else
	{
		if(!m_bInUsed)
			return pFileChunk->FSkip(wLen);

		if(!m_pGlobeBuffer->HasIdleBuffer(wLen))
			return VO_FALSE;
		if (m_btSubStreamNum >= STREAM_TYPE_AUDIO_LPCM_MIN && m_btSubStreamNum <= STREAM_TYPE_AUDIO_LPCM_MAX)
		{
			///<16 bit big endian to little endian
			VO_U16 *pTmpBuf = (VO_U16 *)pPESBuffer;
			for (VO_U32 nCnt =0;nCnt < wLen/2;nCnt++)
			{
				pTmpBuf[nCnt] = (pTmpBuf[nCnt] >> 8)|(pTmpBuf[nCnt]<<8);
			}
		}

		VO_PBYTE pWrite = VO_NULL;
		dwPosInBuffer = m_pGlobeBuffer->GetForDirectWrite(&pWrite, wLen);
		if(VO_MAXU32 == dwPosInBuffer)
			return VO_FALSE;
		m_pGlobeBuffer->DirectWrite2((VO_PBYTE)pPESBuffer, pWrite, 0, wLen);
	}

	if(m_btSubStreamNum >= STREAM_TYPE_AUDIO_LPCM_MIN && m_btSubStreamNum <= STREAM_TYPE_AUDIO_LPCM_MAX)
	{
		if (m_nCurBufSize == 0)
		{
			m_nPosBuf = dwPosInBuffer;
			m_nFirstTimeStamp = dwTimeStamp;
		}
		m_nCurBufSize += wLen;
		VO_U32 rc = IsWholeSample();
		if (rc == 0)
		{
			return VO_TRUE;
		}
		else
		{
			if (rc == 1)
			{
				dwPosInBuffer = m_nPosBuf;
				wLen = m_nCurBufSize;
				ResetSampleBuffer();
			}
			else
			{
				VO_U32 PosInBuffer = dwPosInBuffer;///<recode current buffer pos
				VO_U32 BufferLen = wLen;///<recode current buffer len
				dwPosInBuffer = m_nPosBuf;///<set current frame pos
				wLen = m_nSampleSize;///<set current frame len
				m_nPosBuf = (PosInBuffer+BufferLen)-(m_nCurBufSize- m_nSampleSize);///<set next frame buffer pos
				m_nCurBufSize = (m_nCurBufSize- m_nSampleSize);///<set next frame buffer len
			}
			VO_S64 nNextFrameTimeStamp = dwTimeStamp;
			dwTimeStamp = m_nFirstTimeStamp;
			m_nFirstTimeStamp = nNextFrameTimeStamp;
		}	

	}
	PBaseStreamMediaSampleIndexEntry pNew = m_pStreamFileIndex->NewEntry(CalcTrackTs(dwTimeStamp), dwPosInBuffer, wLen);
	if(!pNew)
		return VO_FALSE;
	if (CalculateTimeStamp(pNew))
	{
		PushTmpSampleToStreamFile(pNew);
	}
	//m_pStreamFileIndex->Add(pNew);
	return VO_TRUE;
}

VO_BOOL CMpegAudioTrack::SetExtData(VO_PBYTE pData, VO_U16 wSize)
{
	if (m_btStreamNum == PRIVATE_STREAM_1)
	{
		if ((m_btSubStreamNum >= STREAM_TYPE_AUDIO_AC3_MIN_1 && m_btSubStreamNum <= STREAM_TYPE_AUDIO_AC3_MAX_1)
			||(m_btSubStreamNum >= STREAM_TYPE_AUDIO_AC3_MIN_2 && m_btSubStreamNum <= STREAM_TYPE_AUDIO_AC3_MAX_2))
		{
			return ParAc3HeadData(pData,wSize);
		}
		else if ((m_btSubStreamNum >= STREAM_TYPE_AUDIO_DTS_MIN_1 && m_btSubStreamNum <= STREAM_TYPE_AUDIO_DTS_MAX_1)
			||(m_btSubStreamNum >= STREAM_TYPE_AUDIO_DTS_MIN_2 && m_btSubStreamNum <= STREAM_TYPE_AUDIO_DTS_MAX_2))
		{
			return ParDtsHeadData(pData,wSize);
		}
		else if (m_btSubStreamNum >= STREAM_TYPE_AUDIO_LPCM_MIN && m_btSubStreamNum <= STREAM_TYPE_AUDIO_LPCM_MAX)
		{
			m_dwSampleRate = LPCMParameter2.Sample_rate==0?48000:96000;
			m_wChannels = LPCMParameter2.channels+1;
			m_dwBitrate = (LPCMParameter2.Quantization*4)+16;
			return VO_TRUE;
		}
		else
			return VO_FALSE;
	}
	if(wSize < 4)
		return VO_FALSE;

	VO_U32 dwFrameHeader;
	read_dword2(dwFrameHeader, pData);

	m_btVersion = 1 - ((dwFrameHeader >> 19) & 0x1);
	m_btAudioLayer = 4 - ((dwFrameHeader >> 17) & 0x3);
	if (m_btAudioLayer > 4)
	{
		return VO_FALSE;
	}
	if (m_btAudioLayer == 4)
	{
		m_dwSampleRate = AdtsAACSample_rates[((dwFrameHeader>>10)&0x0F)];
		m_wChannels = ((dwFrameHeader>>6)&0x07)== 0 ? 2 : ((dwFrameHeader>>6)&0x07);
	}
	else
	{
		m_dwBitrate = 1000 * s_dwBitrates[m_btVersion][m_btAudioLayer - 1][(dwFrameHeader >> 12) & 0xf];
		m_dwSampleRate = s_dwSamplingRates[3 - m_btVersion][(dwFrameHeader >> 10) & 0x3];
		m_wChannels = (((dwFrameHeader >> 6) & 0x3) == 3) ? 1 : 2;
	}
	return VO_TRUE;
}

VO_U32 CMpegAudioTrack::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	if (uID == VO_PID_SOURCE_MPLAYERID)
	{
		if (!pParam || m_btAudioLayer > 4)
		{
			return VO_ERR_SOURCE_ERRORDATA;
		}
		*(VO_U8*)pParam = m_btAudioLayer;
		return VO_ERR_SOURCE_OK;
	}
	return CMpegTrack::GetParameter(uID,pParam);
}

VO_U32 PCMSamplePack::IsWholeSample()
{
	return (m_nCurBufSize == m_nSampleSize)?1:(m_nCurBufSize < m_nSampleSize)?0:2;
}

VO_BOOL PCMSamplePack::ResetSample()
{
	ResetSampleBuffer();
	m_nFirstTimeStamp = -1;
	return VO_TRUE;
}


VO_VOID PCMSamplePack::ResetSampleBuffer()
{
	m_nPosBuf = 0;
	m_nCurBufSize = 0;
}

VO_BOOL CMpegAudioTrack::ParAc3HeadData(VO_PBYTE pData, VO_U32 wLen)
{
	VO_U32 wTmpLen = 0;
	while(wTmpLen < wLen - 1)
	{
		if (pData[wTmpLen] != 0x0B)
		{
			wTmpLen++;
			continue;
		}
		if (pData[wTmpLen+1] != 0x77)
		{
			wTmpLen += 2;
			continue;
		}
		break;
	}

	if (wLen -  wTmpLen < 5)
	{
		return VO_FALSE;
	}
	wTmpLen += 4;
	VO_U8 fscode = pData[wTmpLen] >> 6;
	VO_U8 frmsizcod =  (pData[wTmpLen++] & 0x3F);
	VO_U8 bsid = (pData[wTmpLen++] >> 3);
	if (bsid > 16)
	{
		VOLOGE("Not Supported AC3");
		return VO_FALSE;
	}

	VO_U8 nSkipBit = 0;
	VO_U8 acmode = (pData[wTmpLen] >> 5);
	nSkipBit += 3;

	if(acmode == 2) {
		nSkipBit += 2;
	} else {
		if((acmode & 1) && (acmode != 1))
			nSkipBit += 2;
		if(acmode & 4)
			nSkipBit += 2;
	}
	VO_U8 lfeon = (pData[wTmpLen]>>(7-nSkipBit)) & 0x1;

	VO_U8 sr_shift = VOMAX(bsid, 8) - 8;

	m_dwSampleRate = VO_AC3_SampleRate_Tab[fscode] >> sr_shift;
	m_wChannels	= VO_AC3_Channels_Tab[acmode] + lfeon;
	return VO_TRUE;
}
VO_BOOL CMpegAudioTrack::ParDtsHeadData(VO_PBYTE pData, VO_U32 wLen)
{
	VO_U32 wTmpLen = 0;
	while(wTmpLen < wLen - 3)
	{
		if (pData[wTmpLen] != 0x7F)
		{
			wTmpLen++;
			continue;
		}
		if (pData[wTmpLen+1] != 0xFE)
		{
			wTmpLen += 2;
			continue;
		}
		if (pData[wTmpLen+2] != 0x80)
		{
			wTmpLen += 3;
			continue;
		}
		if (pData[wTmpLen+3] != 0x01)
		{
			wTmpLen += 4;
			continue;
		}
		break;
	}
	if (wLen -  wTmpLen < 11)
	{
		return VO_FALSE;
	}
	wTmpLen += 7;

	VO_U16 amode = pData[wTmpLen++] & 0xF;
	amode = (amode << 2) | (pData[wTmpLen] >> 6);

	VO_U8 sfreq = (pData[wTmpLen++] >> 2) & 0xF;

	wTmpLen++;

	VO_U8 lfeFlag = (pData[wTmpLen] >> 1) & 0x3;
	m_dwSampleRate = VO_DTS_SampleRate_Tab[sfreq];
	
	m_wChannels = amode > 0x10 ? 0: VO_DTS_Channels_Tab[amode];
	if (lfeFlag == 1 || lfeFlag == 2)
	{
		m_wChannels++;
	}
	return VO_TRUE;
}