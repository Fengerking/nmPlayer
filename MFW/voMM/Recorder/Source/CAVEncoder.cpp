#include "CAVEncoder.h"
#include "CSampleData.h"
#include "CMpeg4Encode.h"
#include "CMpeg4Dump.h"
#include "CAMREncode.h"
#include "vcamerabase.h"
#include "CAmrDump.h"



#if defined(DMEMLEAK)
#  include "dmemleak.h"
#endif 

extern unsigned long g_Error;

//#define _NO_AUDIO_DUMP_

CAVEncoder::CAVEncoder(void)
: m_pCurrDump(0)
, m_pCurrAudioEncode(0)
, m_pCurrVideoEncode(0)
, m_lCurrVideoFileType(0)
, m_lCurrVideoEncodeType(VO_VIDEO_CodingMPEG4)
, m_lCurrAudioEncodeType(VO_AUDIO_CodingAMRNB)
, m_nVideoWidth(0)
, m_nVideoHeight(0)
, m_dwTotalEncodeVideoCount(0)
, m_dwLastVideoEncodeTime(0)
, m_dwTotalAudioRawDataLen(0)
, m_dwMpeg4EncodeBitrate(100)
, m_dwH263EncodeBitrate(100)
, m_nContentType(BOTH_REC)
, m_nImageQuality(75)
, m_nEncodeQuality(VO_ENC_MID_QUALITY)
, m_nVideoFrmRate(15)
, m_pNotifyCB(NULL)
, m_pUserData(NULL)
{
	memset(m_szImgFileDir, 0, sizeof(m_szImgFileDir));
	memset(m_szVideoFileDir, 0, sizeof(m_szVideoFileDir));
}

void CAVEncoder::SetImageQuality(int nQuality)
{
	m_nImageQuality = nQuality;
}


CAVEncoder::~CAVEncoder(void)
{
	Release();
}

void CAVEncoder::SetContentType(int nType)
{
	m_nContentType = nType;
}

int	CAVEncoder::GetContentType()
{
	return m_nContentType;
}

bool CAVEncoder::IsRecordAudio()
{
	return (m_nContentType&BOTH_REC)||(m_nContentType&ONLY_REC_AUDIO);
}


bool CAVEncoder::IsRecordVideo()
{
	return (m_nContentType&BOTH_REC)||(m_nContentType&ONLY_REC_VIDEO);
}

unsigned long CAVEncoder::RecvVideoSample(CSampleData* pSample, VO_IV_COLORTYPE& videoType)
{
	CEncode* pVE = GetCurrVideoEncode();

	if(pVE)
	{
		unsigned long ret = pVE->ReceiveSample(pSample, videoType);

		if(ret == I_VORC_REC_NO_ERRORS)
		{
			//CPerfTest::DumpLog("encode ok\r\n");
			m_dwTotalEncodeVideoCount++;
			unsigned long dwStart = 0;
			pSample->GetTime(dwStart, m_dwLastVideoEncodeTime);
			return I_VORC_REC_NO_ERRORS;
		}
		else
		{
// 			if(ret == I_VORC_REC_MEMORY_FULL)
// 			{
// 				if(!(g_Error & VORC_REC_MEMORY_FULL))
// 				{
// 					g_Error |= VORC_REC_MEMORY_FULL;
// 					
// 				}	
// 			}
// 			else if(ret == VORC_REC_DISK_FULL)
// 			{
// 				if(!(g_Error & VORC_REC_DISK_FULL))
// 				{
// 					g_Error |= VORC_REC_DISK_FULL;
// 				}
// 			}
// 			else if(ret == VORC_REC_WRITE_FAILED)
// 			{
// 				if(!(g_Error & VORC_REC_WRITE_FAILED))
// 				{
// 					g_Error |= VORC_REC_WRITE_FAILED;
// 				}
// 			}

			g_Error |= ret;

			return g_Error;
		}
	}
}

long long CAVEncoder::GetDumpSize()
{
	long long dwSize = 0;

	if(m_pCurrDump)
	{
		dwSize = m_pCurrDump->GetCurrFileSize();
	}
		

	return dwSize+16*1024;
}

void CAVEncoder::SetNotifyCallback(VOMMRecordCallBack pProc)
{
	m_pNotifyCB = pProc;
}


unsigned long CAVEncoder::RecvAudioSample(CSampleData* pSample)
{
	CEncode* pAE = GetCurrAudioEncode();

	if(pAE)
	{
		m_dwTotalAudioRawDataLen += pSample->GetDataLen();
		unsigned long ret = pAE->ReceiveAudioSample(pSample);

		if(ret == I_VORC_REC_NO_ERRORS)
		{
			return I_VORC_REC_NO_ERRORS;
		}
		else
		{
// 			if(ret == VORC_REC_MEMORY_FULL)
// 			{
// 				if(!(g_Error & VORC_REC_MEMORY_FULL))
// 				{
// 					g_Error |= VORC_REC_MEMORY_FULL;
// 				}	
// 			}
// 			else if(ret == VORC_REC_DISK_FULL)
// 			{
// 				if(!(g_Error & VORC_REC_DISK_FULL))
// 				{
// 					g_Error |= VORC_REC_DISK_FULL;
// 				}
// 			}
// 			else if(ret == VORC_REC_WRITE_FAILED)
// 			{
// 				if(!(g_Error & VORC_REC_WRITE_FAILED))
// 				{
// 					g_Error |= VORC_REC_WRITE_FAILED;
// 				}
// 			}
			
			g_Error |= ret;

			return g_Error;
		}
	}
}


void CAVEncoder::Init()
{
	/*GetCurrDump();
	GetCurrAudioEncode();
	GetCurrVideoEncode();
	*/
}


unsigned long CAVEncoder::StartEncode()
{
	m_dwLastVideoEncodeTime		= 0;
	m_dwTotalEncodeVideoCount	= 0;
	m_dwTotalAudioRawDataLen	= 0;
	
	CEncode* pVE = NULL;
	CEncode* pAE = NULL;
	
	CPerfTest::DumpLog("[REC] begin to create file writer\r\n");

	CDump* pDump = GetCurrDump();
	
	if(pDump)
	{
		if(!pDump->Start())
		{
			StopEncode();
			return I_VORC_REC_FW_INIT_ERROR;
		}
	}
	else
	{
		if(m_pNotifyCB)
			m_pNotifyCB(m_pUserData, VO_MMR_MSG_FILEWRITER_INIT_FAILED, 0);

		StopEncode();
		return I_VORC_REC_FW_INIT_ERROR;
	}


	if(IsRecordVideo())
	{
		CPerfTest::DumpLog("[REC] begin to create video encode\r\n");

		pVE = GetCurrVideoEncode();

		if(pVE)
		{
			if(!pVE->Start())
			{
				StopEncode();
				return I_VORC_REC_VIDEO_ENC_INT_FAILED;
			}
		}
		else
		{
			if(m_pNotifyCB)
				m_pNotifyCB(m_pUserData, VO_MMR_MSG_VIDEOENCODE_INIT_FAILED, 0);

			StopEncode();
			return I_VORC_REC_VIDEO_ENC_INT_FAILED;
		}
	}


	if(IsRecordAudio())
	{
		CPerfTest::DumpLog("[REC] begin to create audio encode\r\n");

		pAE = GetCurrAudioEncode();

		if(pAE)
		{
			if(!pAE->Start())
			{
				StopEncode();
				return I_VORC_REC_AUDIO_ENC_INT_FAILED;
			}
		}
		else
		{
			if(m_pNotifyCB)
				m_pNotifyCB(m_pUserData, VO_MMR_MSG_AUDIOENCODE_INIT_FAILED, 0);

			StopEncode();
			return I_VORC_REC_AUDIO_ENC_INT_FAILED;
		}
	}



	return I_VORC_REC_NO_ERRORS;
}


void CAVEncoder::StopEncode()
{
	if(m_pCurrVideoEncode)
	{
		m_pCurrVideoEncode->Stop();
		m_pCurrVideoEncode->Close();
		delete m_pCurrVideoEncode;
		m_pCurrVideoEncode = 0;
	}

	
	if(m_pCurrAudioEncode)
	{
		m_pCurrAudioEncode->Stop();
		m_pCurrAudioEncode->Close();
		delete m_pCurrAudioEncode;
		m_pCurrAudioEncode = 0;
	}

	if(m_pCurrDump)
	{
		SetRealFrameRate();
		
		m_pCurrDump->Stop();
		delete m_pCurrDump;
		m_pCurrDump = 0;
	}
}

void CAVEncoder::SetRealFrameRate()
{
	if(m_pCurrDump)
	{
		float fr = 0.0;

		if(IsRecordAudio())
		{
			//should check the input source type, for example, pcm or AMR

			if(m_dwTotalAudioRawDataLen == 0)
				fr = 0.;
			else
				fr = m_dwTotalEncodeVideoCount/((m_dwTotalAudioRawDataLen*8)/(8000.*16.));
		}
		else if(IsRecordVideo())
		{
			if(m_dwLastVideoEncodeTime == 0)
				fr = 0.;
			else
				fr = m_dwTotalEncodeVideoCount/m_dwLastVideoEncodeTime*1000.;
		}

		//now, only AVI writer need call this function
		m_pCurrDump->SetRealFrameRate(fr);
	}
}


CDump* CAVEncoder::GetCurrDump()
{
	if(m_pCurrDump)
		return m_pCurrDump;

	if(m_nContentType & ONLY_REC_AUDIO)
	{
		m_pCurrDump = new CAmrDump;
	}
	else
	{
		if(VO_VIDEO_CodingMPEG4 == m_lCurrVideoEncodeType)
		{
			m_pCurrDump = new CMpeg4Dump;
		}
// 		else if(VO_VIDEO_CodingH263 == m_lCurrVideoEncodeType)
// 		{
// 			m_pCurrDump = new CH263Dump;
// 		}
// 		else if(VO_VIDEO_CodingMJPEG == m_lCurrVideoEncodeType)
// 		{
// 			m_pCurrDump = new CMJpegDump;
// 		}
	}


	m_pCurrDump->SetName(m_szVideoFileDir);

	if(!m_pCurrDump->Init())
	{
		m_pCurrDump->Stop();
		delete m_pCurrDump;
		m_pCurrDump = 0;
		return 0;
	}

	long long ret = m_pCurrDump->CreateVideoTrack (m_nVideoWidth, m_nVideoHeight);

	if(ret != I_VORC_REC_NO_ERRORS)
	{
		m_pCurrDump->Stop();
		delete m_pCurrDump;
		m_pCurrDump = 0;
		return 0;
	}

	if(IsRecordAudio())
	{
		int nCodec = 0;
// 		if(m_lCurrAudioEncodeType == VO_AUDIO_CodingPCM)
// 			nCodec = FF_AUDIO_WAV;
// 		else
// 			nCodec = FF_AUDIO_AMR;

		ret = m_pCurrDump->CreateAudioTrack(8000, 1, 16, nCodec);

		if(ret != I_VORC_REC_NO_ERRORS)
		{
			m_pCurrDump->Stop();
			delete m_pCurrDump;
			m_pCurrDump = 0;
			return 0;
		}
	}

	return m_pCurrDump;
}


CEncode* CAVEncoder::GetCurrVideoEncode()
{
	if(m_pCurrVideoEncode)
		return m_pCurrVideoEncode;

	if(VO_VIDEO_CodingMPEG4 == m_lCurrVideoEncodeType)
	{
		m_pCurrVideoEncode = new CMpeg4Encode;
	}
// 	else if(VO_VIDEO_CodingH263 == m_lCurrVideoEncodeType)
// 	{
// 		m_pCurrVideoEncode = new CH263Encode;
// 	}
// 	else if(VO_VIDEO_CodingMJPEG == m_lCurrVideoEncodeType)
// 	{
// 		m_pCurrVideoEncode = new CMjpegEncode;
// 	}

	if(!m_pCurrVideoEncode->Init())
	{
		delete m_pCurrVideoEncode;
		m_pCurrVideoEncode = 0;
		return NULL;
	}

	m_pCurrVideoEncode->SetEncodeQuality(m_nEncodeQuality);
	m_pCurrVideoEncode->SetVideoInfo(m_nVideoFrmRate, m_nVideoFrmRate, m_dwMpeg4EncodeBitrate);

	if(m_pCurrVideoEncode)
	{
		m_pCurrVideoEncode->SetVideoSize(m_nVideoWidth, m_nVideoHeight);

		int nInfoLen = 0;
		unsigned char* pInfo = new unsigned char[120];
		bool ret = m_pCurrVideoEncode->GetHeaderInfo(&pInfo, nInfoLen);
		CDump* pDump = GetCurrDump();
		
		if(pDump)
		{
			ret = pDump->SetHeaderInfo(pInfo, nInfoLen);
			m_pCurrVideoEncode->SetDump(pDump);
		}

		delete []pInfo;
		return m_pCurrVideoEncode;
	}

	return 0;
}


CEncode* CAVEncoder::GetCurrAudioEncode()
{
	if(m_pCurrAudioEncode)
		return m_pCurrAudioEncode;

	if(VO_AUDIO_CodingAMRNB == m_lCurrAudioEncodeType)
	{
		m_pCurrAudioEncode = new CAMREncode;
	}
// 	else if(VO_AUDIO_CodingPCM == m_lCurrAudioEncodeType)
// 	{
// 		m_pCurrAudioEncode = new CPcmRawEncode;
// 	}
// 	else
// 	{
// 	}

	if(m_pCurrAudioEncode)
	{
		if(!m_pCurrAudioEncode->Init())
		{
			delete m_pCurrAudioEncode;
			m_pCurrAudioEncode = NULL;
			return NULL;
		}

		CDump* pDump = GetCurrDump();
		m_pCurrAudioEncode->SetDump(pDump);
	}

	return m_pCurrAudioEncode;
}


void CAVEncoder::SetVideoSize(int nWidth, int nHeight)
{
	m_nVideoWidth = nWidth;
	m_nVideoHeight = nHeight;
}


bool CAVEncoder::SetVideoFileType(long fType)
{
	bool bRet = true;

/*	if(FILEMPEG4 == fType 
		|| FILE3GP == fType)
	{
	}
	else
*/	{
		bRet = false;
#ifdef _DEBUG
		::MessageBox(0, _T("Can't support the video file type."), _T(""), MB_OK);
#endif
	}
		
	if(bRet)
		m_lCurrVideoFileType = fType;

	return bRet;
}

bool CAVEncoder::SetVideoEncodeType(long vType)
{
	m_lCurrVideoEncodeType = vType;

	return true;
}

bool CAVEncoder::SetAudioEncodeType(long aType)
{
	m_lCurrAudioEncodeType = aType;

	return true;
}

bool CAVEncoder::GetVideoFileType(long* pType)
{
	*pType = m_lCurrVideoFileType;
	return true;
}

bool CAVEncoder::GetVideoEncodeType(long* pType)
{
	*pType = m_lCurrVideoEncodeType;
	return true;
}


bool CAVEncoder::GetAudioEncodeType(long* pType)
{
	*pType = m_lCurrAudioEncodeType;
	return true;
}


bool CAVEncoder::GetFrameRate(float* pFrameRate)
{
	//real-time encode frame rate 
	*pFrameRate = m_dwLastVideoEncodeTime!=0?((float)(m_dwTotalEncodeVideoCount*1000)/(float)m_dwLastVideoEncodeTime):0.0;

	return true;
}


void CAVEncoder::Release()
{
	if(m_pCurrDump)
	{
		delete m_pCurrDump;
		m_pCurrDump = 0;
	}

	if(m_pCurrAudioEncode)
	{
		m_pCurrAudioEncode->Close();
		delete m_pCurrAudioEncode;
		m_pCurrAudioEncode = 0;
	}

	if(m_pCurrVideoEncode)
	{
		m_pCurrVideoEncode->Close();
		delete m_pCurrVideoEncode;
		m_pCurrVideoEncode = 0;
	}
}

void CAVEncoder::SetImgFileDirectory(TCHAR* pFileDir)
{
	_tcscpy(m_szImgFileDir, pFileDir);
}

void CAVEncoder::SetVideoFileDirectory(TCHAR* pFileDir)
{
	_tcscpy(m_szVideoFileDir, pFileDir);
}

void CAVEncoder::SetEncodeQuality(int nQuality)
{
	m_nEncodeQuality = nQuality;
}

void CAVEncoder::SetMpeg4EncodeBitrate(unsigned long dwBitrate)
{
	m_dwMpeg4EncodeBitrate = dwBitrate;
}

unsigned long CAVEncoder::GetMpeg4EncodeBitrate()
{
	return m_dwMpeg4EncodeBitrate;
}

void CAVEncoder::SetH263EncodeBitrate(unsigned long dwBitrate)
{
	m_dwH263EncodeBitrate = dwBitrate;
}

unsigned long CAVEncoder::GetH263EncodeBitrate()
{
	return m_dwH263EncodeBitrate;
}


void CAVEncoder::SetVideoEncodeFrameRate(int nFrmRate)
{
	m_nVideoFrmRate = nFrmRate;
}
