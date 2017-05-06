
#include "CCmmbFileDataParser.h"
#include "voLog.h"
//#include "CPatternFinder.h"
#include "CDumper.h"

CCmmbFileDataParser::CCmmbFileDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
: CvoStreamFileDataParser(pFileChunk, pMemOp)
, m_nFrameCurrLen(0)
, m_pVideoFrameBuf(NULL)
, m_pMfsFrame(NULL)
{
	MemSet(&m_FileInfo, 0, sizeof(CMMB_FILE_INFO));
}

CCmmbFileDataParser::~CCmmbFileDataParser(void)
{
	Uninit();
}

VO_VOID	CCmmbFileDataParser::Init(VO_PTR pParam)
{
	Uninit();

	m_Dmx.Open();
	m_Dmx.SetReciever(this);
	m_FileInfo.nDuration = -1;
	m_FileInfo.nFirstTimeStamp = 0;

	m_pVideoFrameBuf	= (VO_PBYTE)MemAlloc(128*1024);
	m_pMfsFrame			= (VO_PBYTE)MemAlloc(128*1024);
}

VO_VOID	CCmmbFileDataParser::Uninit()
{
	m_Dmx.Close();

	if (m_pVideoFrameBuf)
	{
		MemFree(m_pVideoFrameBuf);
		m_pVideoFrameBuf = VO_NULL;
	}

	if (m_pMfsFrame)
	{
		MemFree(m_pMfsFrame);
		m_pMfsFrame = VO_NULL;
	}
}

//#define _OLD_GET_FILE_INFO

#ifndef _OLD_GET_FILE_INFO

CMMB_FILE_INFO* CCmmbFileDataParser::GetFileInfo()
{
	if (m_FileInfo.nDuration > 0)
	{
		return &m_FileInfo;
	}

	VO_U32 dwSync			= 0X01000000;
	VO_U64	llLastFramPos	= 0;
	VO_BOOL bFirstFrame		= VO_FALSE;

	VO_U32	dwFrmCount		= 0;
	VO_U64	llDuration1		= 0;
	VO_U64	llDuration2		= 0;

	while(!IsParseEnd() && !bFirstFrame)
	{
		VO_U32 dwTmp = 0;
		if(!m_pFileChunk->FRead(&dwTmp, 4))
			break;

		if (dwTmp == dwSync)
		{
			dwFrmCount++;

			m_pFileChunk->FBack(4);
			m_pFileChunk->FRead(m_pMfsFrame, 128*1024);

			if (!bFirstFrame)
			{
				m_Dmx.Demux((BYTE*)m_pMfsFrame, 128*1024);

				MultiplexFrame* pFrm = m_Dmx.GetMF();

				if (pFrm->multiplex_sub_frame.sub_frame_header[0].start_time_flag)
				{
					llDuration1 = pFrm->multiplex_sub_frame.sub_frame_header[0].start_time_stamp;
					m_FileInfo.nFirstTimeStamp = llDuration1;
					
					VO_U16 codec = pFrm->multiplex_sub_frame.sub_frame_header[0].audio_stream_param[0].algorithm_type;
					if(codec == AUDIO_DRA)
						m_FileInfo.nAudioCodec = VO_AUDIO_CodingDRA;
					else if (codec == AUDIO_HE_AAC || codec == AUDIO_AAC)
						m_FileInfo.nAudioCodec = VO_AUDIO_CodingAAC;
					else
						m_FileInfo.nAudioCodec = VO_AUDIO_CodingUnused;

					codec = pFrm->multiplex_sub_frame.sub_frame_header[0].video_stream_param[0].algorithm_type;
					if(codec == VIDEO_H264)
						m_FileInfo.nVideoCodec = VO_VIDEO_CodingH264;
					else
						m_FileInfo.nVideoCodec = VO_VIDEO_CodingUnused;

					m_FileInfo.nStreamID = pFrm->frame_header.mf_id;

					bFirstFrame = VO_TRUE;
					pFrm->multiplex_sub_frame.sub_frame_header[0].start_time_flag = 0;
					break;
				}
				else
					m_pFileChunk->FBack(128*1024 - 1);
			}
		}
		else
			m_pFileChunk->FBack(3);

	}

	CDumper::WriteLog("read count  %d", dwFrmCount);

	//if (llLastFramPos != 0)
	{
		int read = 128*1024;
		m_pFileChunk->FLocate(m_nFileSize-read);

		while (!IsParseEnd())
		{
			VO_U32 dwTmp = 0;
			if(!m_pFileChunk->FRead(&dwTmp, 4))
				break;

			if (dwTmp == dwSync)
			{
				m_pFileChunk->FBack(4);
				m_pFileChunk->FRead(m_pMfsFrame, 128*1024);

				m_Dmx.Demux((BYTE*)m_pMfsFrame, 128*1024);

				MultiplexFrame* pFrm = m_Dmx.GetMF();

				if (pFrm->multiplex_sub_frame.sub_frame_header[0].start_time_flag)
				{
					llDuration2 = pFrm->multiplex_sub_frame.sub_frame_header[0].start_time_stamp;
					break;
				}
				else
					m_pFileChunk->FBack(128*1024 - 1);
			}
			else
				m_pFileChunk->FBack(3);
		}

		llDuration1 = llDuration2 - llDuration1;
	}

	m_FileInfo.nDuration = (llDuration1 / 22500)*1000;
	m_pFileChunk->FLocate(0);

	CDumper::WriteLog("++++++++++++++++++++++++++++++ cmmb file duration is %d ++++++++++++++++++++++++++++++", m_FileInfo.nDuration/1000);

	return &m_FileInfo;
}

#else


CMMB_FILE_INFO* CCmmbFileDataParser::GetFileInfo()
{
	if (m_FileInfo.nDuration > 0)
	{
		return &m_FileInfo;
	}

	VO_U32 dwSync			= 0X01000000;
	VO_U64	llLastFramPos	= 0;
	VO_BOOL bFirstFrame		= VO_FALSE;

	VO_U32	dwFrmCount		= 0;
	VO_U64	llDuration1		= 0;
	VO_U64	llDuration2		= 0;

	while(!IsParseEnd())
	{
		dwFrmCount++;

		VO_U32 dwTmp = 0;
		if(!m_pFileChunk->FRead(&dwTmp, 4))
			break;

		if (dwTmp == dwSync)
		{
			dwFrmCount++;

			VO_U64 pos		= m_pFileChunk->FGetFilePos() - 4;
			llLastFramPos	= pos;

			if (!bFirstFrame)
			{
				bFirstFrame = VO_TRUE;

				VO_PBYTE pData = m_pFileChunk->FGetFileContentPtr(0);
				pData -= 4;

				m_Dmx.Demux((BYTE*)pData, 128*1024);

				MultiplexFrame* pFrm = m_Dmx.GetMF();

				if (pFrm->multiplex_sub_frame.sub_frame_header[0].start_time_flag)
				{
					llDuration1 = pFrm->multiplex_sub_frame.sub_frame_header[0].start_time_stamp;
					m_FileInfo.nFirstTimeStamp = llDuration1;
				}

				VO_U16 codec = pFrm->multiplex_sub_frame.sub_frame_header[0].audio_stream_param[0].algorithm_type;
				if(codec == AUDIO_DRA)
					m_FileInfo.nAudioCodec = VO_AUDIO_CodingDRA;
				else if (codec == AUDIO_HE_AAC || codec == AUDIO_AAC)
					m_FileInfo.nAudioCodec = VO_AUDIO_CodingAAC;
				else
					m_FileInfo.nAudioCodec = VO_AUDIO_CodingUnused;

				codec = pFrm->multiplex_sub_frame.sub_frame_header[0].video_stream_param[0].algorithm_type;
				if(codec == VIDEO_H264)
					m_FileInfo.nVideoCodec = VO_VIDEO_CodingH264;
				else
					m_FileInfo.nVideoCodec = VO_VIDEO_CodingUnused;

				m_FileInfo.nStreamID = pFrm->frame_header.mf_id;
				
				//VOLOGI("find first cmmb frame at: %u", pos);
			}

			//VOLOGI("find cmmb flag at: %u", pos);
		}
	}

	CDumper::WriteLog("read count  %d", dwFrmCount);

	if (llLastFramPos != 0)
	{
		m_pFileChunk->FLocate(llLastFramPos);

		VO_PBYTE pData = m_pFileChunk->FGetFileContentPtr(0);

		if (!pData)
		{
			
		}

		m_Dmx.Demux((BYTE*)pData, 128*1024);
		MultiplexFrame* pFrm = m_Dmx.GetMF();

		if (pFrm->multiplex_sub_frame.sub_frame_header[0].start_time_flag)
		{
			llDuration2 = pFrm->multiplex_sub_frame.sub_frame_header[0].start_time_stamp;
		}

		llDuration1 = llDuration2 - llDuration1;

// 		VOLOGI("find last cmmb frame at: %u", llLastFramPos, dwFrmCount);
// 		VOLOGI("toatal cmmb frame count: %d", dwFrmCount);
	}

	m_FileInfo.nDuration = (llDuration1 / 22500)*1000;

	m_pFileChunk->FLocate(0);

	return &m_FileInfo;
}

#endif


VO_BOOL CCmmbFileDataParser::StepB()
{
	VO_U32 dwSync = 0X01000000;
	if(!m_pFileChunk->FRead(m_pMfsFrame, 4))
		return VO_FALSE;

	static VO_U32 dwRec = 0;
	dwRec += 4;

	if (!MemCompare(m_pMfsFrame, &dwSync, 4))
	{
		dwRec = 0;

		//VO_U64 pos = m_pFileChunk->FGetFilePos() - 4;
		//VOLOGI("find cmmb flag at: %u", pos);

		if (!m_pFileChunk->FRead(m_pMfsFrame+4, 15))
			return VO_FALSE;

		VO_U32 nLen = m_Dmx.CalMfsLen(m_pMfsFrame);

		if(nLen <= 0)
		{
			m_pFileChunk->FBack(3);
			return VO_TRUE;
		}

		if (!m_pFileChunk->FRead(m_pMfsFrame+19, nLen-19))
			return VO_FALSE;

		VO_PBYTE pData = m_pMfsFrame;

		static VO_U32 count = 0;
		//VOLOGI("begin parse cmmb MFS: No.%d - len %d\n", ++count, nLen);

		m_Dmx.Demux(pData, nLen);

		//VOLOGI("MF ID = %d", m_Dmx.GetMF()->frame_header.mf_id);
	}
	else
	{
		m_pFileChunk->FBack(3);
		//VOLOGI("scan count = %d", dwRec);
	}

	return VO_TRUE;
}

void CCmmbFileDataParser::WhenParsedVideoFrame(VideoUnitBuf* pVideo)
{
	if (!m_fODCallback)
	{
		return;
	}

	for(int n=0; n<pVideo->buf_count; n++)
	{
		if(FRAME_WHOLE == pVideo->frame[n].pos)
		{
			m_nFrameCurrLen = pVideo->frame[n].dwLen;
			MemCopy(m_pVideoFrameBuf, pVideo->frame[n].pData, pVideo->frame[n].dwLen);

			CMMB_FRAME_BUFFER buf;
			buf.bVideo		= VO_TRUE;
			buf.pData		= m_pVideoFrameBuf;
			buf.nDataLen	= m_nFrameCurrLen;
			buf.nTimeStamp	= CheckTimestamp(pVideo->frame[n].dwTimeStamp);
			// 20100616
			buf.bKeyFrame	= pVideo->frame[n].type == FRAME_I ? VO_TRUE:VO_FALSE;
			buf.bKeyFrame	= m_FrameChecker.AnalyseData(m_pVideoFrameBuf, m_nFrameCurrLen);

			if(m_fODCallback)
				m_fODCallback(m_pODUser, m_pODUserData, &buf);
		}
		else if(FRAME_BEGIN == pVideo->frame[n].pos)
		{
			MemCopy(m_pVideoFrameBuf, pVideo->frame[n].pData, pVideo->frame[n].dwLen);
			m_nFrameCurrLen = pVideo->frame[n].dwLen;
		}
		else if(FRAME_MID == pVideo->frame[n].pos)
		{
			MemCopy(m_pVideoFrameBuf+m_nFrameCurrLen, pVideo->frame[n].pData, pVideo->frame[n].dwLen);
			m_nFrameCurrLen += pVideo->frame[n].dwLen;
		}
		else if(FRAME_END == pVideo->frame[n].pos)
		{
			MemCopy(m_pVideoFrameBuf+m_nFrameCurrLen, pVideo->frame[n].pData, pVideo->frame[n].dwLen);
			m_nFrameCurrLen += pVideo->frame[n].dwLen;

			CMMB_FRAME_BUFFER buf;
			buf.bVideo		= VO_TRUE;
			buf.pData		= m_pVideoFrameBuf;
			buf.nDataLen	= m_nFrameCurrLen;
			buf.nTimeStamp	= CheckTimestamp(pVideo->frame[n].dwTimeStamp);

			// 20100616
			//buf.bKeyFrame	= pVideo->frame[n].type == FRAME_I ? VO_TRUE:VO_FALSE;
			buf.bKeyFrame	= m_FrameChecker.AnalyseData(m_pVideoFrameBuf, m_nFrameCurrLen);

			//VOLOGI("parse %s sample-> %02x %02x %02x %02x, ts:%u", "VIDEO", m_pVideoFrameBuf[0], m_pVideoFrameBuf[1], m_pVideoFrameBuf[2], m_pVideoFrameBuf[3], (pVideo->frame[n].dwTimeStamp*1000)/22500);

			if(m_fODCallback)
				m_fODCallback(m_pODUser, m_pODUserData, &buf);
		}		
	}
}

VO_U64 CCmmbFileDataParser::CheckTimestamp(VO_U64 time)
{
	if (m_FileInfo.nFirstTimeStamp == 0)
		return time;

	if (time < m_FileInfo.nFirstTimeStamp)
	{
		return 0; 
	}

	return time - m_FileInfo.nFirstTimeStamp;
	
}

void CCmmbFileDataParser::WhenParsedAudioFrame(AudioUnitBuf* pAudio)
{
	if (!m_fODCallback)
	{
		return;
	}

	for (VO_U16 n=0; n<pAudio->buf_count; n++)
	{
		CMMB_FRAME_BUFFER buf;
		buf.bVideo		= VO_FALSE;
		buf.pData		= pAudio->frame[n].pData;
		buf.nDataLen	= pAudio->frame[n].dwLen;
		buf.nTimeStamp	= CheckTimestamp(pAudio->frame[n].dwTimeStamp);
		buf.bKeyFrame	= VO_FALSE;

		if (pAudio->frame[n].pData[0] != 0x21)
		{
			int n =0;
		}

		if(m_fODCallback)
		{
			m_fODCallback(m_pODUser, m_pODUserData, &buf);	
		}
	}
}

VO_VOID CCmmbFileDataParser::DumpAAC(VO_PBYTE pData, VO_U32 dwLen)
{
	static VO_U16 count = 0;
	static FILE* f = VO_NULL;

	if (!f)
	{
		f = fopen("c:\\dump.aac", "wb");
	}

// 	if (count > 200)
// 	{
// 		fclose(f);
// 		return;
// 	}

	static char head1[7];
	int temp = dwLen + 7;

	head1[0] = (char)0xff;
	head1[1] = (char)0xf9;

	/*
	const int sample_rates[] =
	{
	96000, 88200, 64000, 48000, 
	44100, 32000,24000, 22050, 
	16000, 12000, 11025, 8000,
	0, 0, 0, 0
	};

	*/
	// 	int sr_index = rtp_aac_get_sr_index(aac_param_ptr->sample_rate);
	// 	head1[2] = (0x01<<6)|(sr_index<<2)|0x00;

	head1[2] = 0x58;

	head1[3] = (char)0x40;// 0x80 profile
	head1[4] = (temp>>3)&0xff;
	head1[5] = ((temp&0x07)<<5|0x1f);
	head1[6] = (char)0xfc;

	fwrite(head1, 1, sizeof(head1), f);

	fwrite(pData, 1, dwLen, f);

	count++;
}

VO_BOOL	CCmmbFileDataParser::GetFileInfo(VO_SOURCE_INFO* pSourceInfo)
{
	CMMB_FILE_INFO* pInfo = GetFileInfo();

	if (pInfo->nDuration > 0)
	{
		pSourceInfo->Duration	= pInfo->nDuration;

		if (pInfo->nVideoCodec != VO_VIDEO_CodingUnused)
		{
			pSourceInfo->Tracks++;
		}

		if (pInfo->nAudioCodec != VO_AUDIO_CodingUnused)
		{
			pSourceInfo->Tracks++;
		}

		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_S64 CCmmbFileDataParser::GetFirstTimeStamp()
{
	return m_FileInfo.nFirstTimeStamp;
}

VO_S64 CCmmbFileDataParser::Reset()
{
	return 0;
}



