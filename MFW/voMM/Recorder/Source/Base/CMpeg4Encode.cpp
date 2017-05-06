#include "CMpeg4Encode.h"
#include "vcamerabase.h"
#include "CDump.h"


#if defined(DMEMLEAK)
#  include "dmemleak.h"
#endif

//#define _TEST_ENCODE_TIME

typedef VO_S32 (VO_API * Mpeg4EncFuncEntry) (VO_VIDEO_ENCAPI * pEncHandle);

CMpeg4Encode::CMpeg4Encode() 
{
#ifdef _WIN32
	vostrcpy(m_szDllFile, _T("voMPEG4Enc.dll"));
#elif defined LINUX
	vostrcpy(m_szDllFile, _T("voMPEG4Enc.so"));
#endif

	_tcscpy(m_szAPIName, _T("voGetMPEG4EncAPI"));

	memset(&m_Mpeg4EncFunc, 0, sizeof(VO_VIDEO_ENCAPI));
}

CMpeg4Encode::~CMpeg4Encode(void)
{
	if (m_pEncodeData != NULL)
		delete []m_pEncodeData;
}

bool CMpeg4Encode::SetVideoSize(VO_U32 dwWidth, VO_U32 dwHeight)
{
	m_dwVideoWidth = dwWidth;
	m_dwVideoHeight = dwHeight;

	if (m_pEncodeData != NULL)
		delete []m_pEncodeData;

	m_pEncodeData = new VO_BYTE[dwWidth * dwHeight];

	if (!m_Mpeg4EncFunc.SetParam || !m_hCodec)
	{
		return true;
	}

	m_Mpeg4EncFunc.SetParam(m_hCodec, VO_PID_ENC_MPEG4_WIDTH, &dwWidth);
	m_Mpeg4EncFunc.SetParam(m_hCodec, VO_PID_ENC_MPEG4_HEIGHT, &dwHeight);
	
	return true;
}

bool CMpeg4Encode::SetEncodeQuality (int nQuality)
{
	m_Mpeg4EncFunc.SetParam(m_hCodec, VO_PID_ENC_MPEG4_VIDEO_QUALITY, &nQuality);
	
	return true;
}

bool CMpeg4Encode::SetVideoInfo(int nFrmRate, int nKeyFrmInterval, long long nBitrate)
{
	m_Mpeg4EncFunc.SetParam(m_hCodec, VO_PID_ENC_MPEG4_BITRATE, &nBitrate);
	float val = nFrmRate;
	m_Mpeg4EncFunc.SetParam(m_hCodec, VO_PID_ENC_MPEG4_FRAMERATE, &val);
	m_Mpeg4EncFunc.SetParam(m_hCodec, VO_PID_ENC_MPEG4_KEY_FRAME_INTERVAL, &nKeyFrmInterval);

	return true;
}

bool CMpeg4Encode::Init(void)
{
	if (!LoadLib(NULL))
		return false;

	CPerfTest::DumpLog("[REC] load mepeg4 encode module ok\r\n");

	((Mpeg4EncFuncEntry)m_pAPIEntry)(&m_Mpeg4EncFunc);

	if(!m_Mpeg4EncFunc.Init)
		return false;

	m_Mpeg4EncFunc.Init(&m_hCodec, VO_VIDEO_CodingMPEG4, NULL);

	if(!m_hCodec)
		return false;

 	int nEncQuality     = VO_ENC_MID_QUALITY;
 	m_Mpeg4EncFunc.SetParam(m_hCodec, VO_PID_ENC_MPEG4_VIDEO_QUALITY, &nEncQuality);

	return true;
}

bool CMpeg4Encode::GetHeaderInfo(unsigned char** pInfo, int& nInfoLen)
{
// 	VOCODECDATABUFFER header;
// 	header.buffer = new unsigned char[80];
// 
// 	VOMPEG4ENCRETURNCODE ret = m_Mpeg4EncFuc.voMPEG4EncGetParameter(VOID_MPEG4_VOL_HEADER, (long int)&header);
// 
// 	if(VORC_OK == ret)
// 	{
// 		nInfoLen = header.length;
// 		memcpy(*pInfo, header.buffer, header.length);
// 	}
// 
// 	delete []header.buffer;
// 
// 	return VORC_OK == ret;

	return false;
}

bool CMpeg4Encode::Close (void)
{
// 	return m_Mpeg4EncFuc.voMPEG4EncUninit();

	if (m_hCodec)
	{
		m_Mpeg4EncFunc.Uninit(m_hCodec);
		m_hCodec = NULL;
	}

	return true;
}


#ifdef _TEST_ENCODE_TIME
int nRecordCount = 0;
unsigned long arRecord[1500][3];
void Dump()
{

	TCHAR debug[128];
	for(int n = 0; n<nRecordCount; n++)
	{
		_stprintf(debug, _T("Mpeg4 enc: %4d - %4d, duration = %4d,I Frm = %s.\n"), arRecord[n][0], arRecord[n][1], arRecord[n][1]-arRecord[n][0], arRecord[n][2]==1? _T("Yes"):_T("No"));
		OutputDebugString(debug);
	}
}
#endif


long long CMpeg4Encode::ReceiveSample (CSampleData * pSample, VO_IV_COLORTYPE& videoType)
{
	if(m_bStop)
		return I_VORC_REC_NO_ERRORS;

	VO_PBYTE pData = NULL;

	if(pSample->GetData(&pData) <= 0)
		return I_VORC_REC_MEMORY_FULL;

	VO_U32 dwStart, dwEnd;
	pSample->GetTime(dwStart, dwEnd);

	int ret;
	VO_VIDEO_FRAMETYPE type;
	VO_VIDEO_BUFFER inBuffer;
	VO_CODECBUFFER outBuffer;
	
	inBuffer.Buffer[0]	= pData;
	inBuffer.ColorType	= videoType;
	outBuffer.Buffer	= m_pEncodeData;

	ret = m_Mpeg4EncFunc.Process(m_hCodec, &inBuffer, &outBuffer, &type);

	if(ret != VO_ERR_NONE)
		return I_VORC_REC_ENC_FAILED;

	if(m_pDump)
	{
		CSampleData * pMpeg4Sample = NULL;
		pMpeg4Sample = m_pDump->GetVideoSampleData();
		if(pMpeg4Sample == NULL)
			return I_VORC_REC_MEMORY_FULL;

		pMpeg4Sample->SetData(outBuffer.Buffer, outBuffer.Length);
		pMpeg4Sample->SetTime(dwStart, dwEnd);

		//printf("send file writer video data: start = %08d, end = %08d\n", dwStart, dwEnd);


		pMpeg4Sample->SetSyncPoint(type == VO_VIDEO_FRAME_I);

#ifdef _TEST_ENCODE_TIME
		arRecord[nRecordCount][0] = dwStart;
		arRecord[nRecordCount][1] = dwEnd;
		arRecord[nRecordCount][2] = type == VO_VIDEO_FRAME_I?1:0;
		nRecordCount++;
#endif

		return m_pDump->ReceiveVideoSample(pMpeg4Sample);
	}

	return I_VORC_REC_NO_ERRORS;
}
