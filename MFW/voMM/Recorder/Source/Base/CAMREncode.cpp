#include "CAMREncode.h"
#include "CDump.h"
#include "vcamerabase.h"
#include "cmnMemory.h"


typedef VO_S32 (VO_API * AMREncFuncEntry) (VO_AUDIO_CODECAPI * pEncHandle);


CAMREncode::CAMREncode(void)
{
	m_nInBufSize = 160*2;
	m_nOutBufSize = 32;
	m_dwFrameTime = 20;

#ifdef _WIN32
	vostrcpy(m_szDllFile, _T("voAMRNBEnc.dll"));
#elif defined LINUX
	vostrcpy(m_szDllFile, _T("voAMRNBEnc.so"));
#endif

	_tcscpy(m_szAPIName, _T("voGetAMRNBEncAPI"));
}

CAMREncode::~CAMREncode(void)
{
	m_MemOp.Alloc	= cmnMemAlloc;
	m_MemOp.Copy	= cmnMemCopy;
	m_MemOp.Free	= cmnMemFree;
	m_MemOp.Set		= cmnMemSet;
	m_MemOp.Check	= cmnMemCheck;
}

bool CAMREncode::Init (void)
{
	if (!LoadLib(NULL))
		return false;

	CPerfTest::DumpLog("[REC] load amr encode module ok\r\n");

	((AMREncFuncEntry)m_pAPIEntry)(&m_AmrNbEncodeFunc);

	VO_CODEC_INIT_USERDATA userData;
	userData.memflag = VO_IMF_PREALLOCATEDBUFFER;
	userData.memData = (VO_PTR)(&m_MemOp);

	int ret = m_AmrNbEncodeFunc.Init(&m_hCodec, VO_AUDIO_CodingAMRNB, &userData);


	VOAMRNBFRAMETYPE frameType	= VOAMRNB_RFC3267;
	VOAMRNBMODE mode			= VOAMRNB_MD122;
	ret = m_AmrNbEncodeFunc.SetParam(m_hCodec, VO_PID_AMRNB_FRAMETYPE, &frameType);
	ret	= m_AmrNbEncodeFunc.SetParam(m_hCodec, VO_PID_AMRNB_MODE, &mode);


	return true;
}

bool CAMREncode::Close (void)
{
	if (m_hCodec)
	{
		m_AmrNbEncodeFunc.Uninit(m_hCodec);
		m_hCodec = NULL;
	}

	return true;
}


long long CAMREncode::ReceiveSample(CSampleData * pSample, VO_IV_COLORTYPE& videoType)
{
	return I_VORC_REC_NO_ERRORS;
}

long long CAMREncode::ReceiveAudioSample (CSampleData * pSample)
{
	if (m_bStop)
		return I_VORC_REC_NO_ERRORS;

	long long ret = 0;
	VO_U32 dwStartTime, dwEndTime, dwLastTime;
	pSample->GetTime(dwStartTime, dwEndTime);
	dwLastTime = dwEndTime;

	VO_PBYTE pData = NULL;
	VO_PBYTE pInputData = NULL;
	VO_PBYTE pOutBuf = NULL;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outFormat;
	CSampleData * pAMRSample = NULL;

	int nSampleLen = pSample->GetData (&pData);
	if (nSampleLen <= 0)
		return I_VORC_REC_MEMORY_FULL;

	pInputData = pData;
	while (pData - pInputData < nSampleLen)
	{
		if(m_pDump)
			pAMRSample = m_pDump->GetAudioSampleData();

		if(pAMRSample == NULL)
			return I_VORC_REC_MEMORY_FULL;

		pAMRSample->AllocBuf (m_nOutBufSize);
		pAMRSample->GetBuffer (&pOutBuf);

		inData.Buffer	= pData;
		inData.Length	= m_nInBufSize;
		outData.Buffer	= pOutBuf;
		
		int ret = m_AmrNbEncodeFunc.SetInputData(m_hCodec, &inData);

		do {
				ret = m_AmrNbEncodeFunc.GetOutputData(m_hCodec, &outData, &outFormat);

				if(ret == 0)
				{
					//CPerfTest::DumpLog("[REC] amr encode ok\r\n");

					pAMRSample->SetDataLen(outData.Length);
					pData += m_nInBufSize;

					//last data
					if((pData-pInputData) >= nSampleLen)
					{
						dwEndTime = dwLastTime;
					}
					else
						dwEndTime = dwStartTime + m_dwFrameTime;

					if(dwEndTime > dwLastTime)
						dwEndTime = dwLastTime;

					pAMRSample->SetTime(dwStartTime, dwEndTime);

#ifdef _DUMP_LOG_
// 					char debug[256];
// 					sprintf(debug, "[Rec] input size = %d, output size = %d, time = %08d\r\n", inData.Length, outData.Length, dwStartTime);
// 					CPerfTest::DumpLog(debug);
#endif

					dwStartTime = dwEndTime;

					if(m_pDump)
					{
						ret = m_pDump->ReceiveAudioSample(pAMRSample);

						if(ret != I_VORC_REC_NO_ERRORS)
							return ret;
					}
				}

			} while(ret != VO_ERR_INPUT_BUFFER_SMALL);
	}

 	return I_VORC_REC_NO_ERRORS;
}


long long CPcmRawEncode::ReceiveAudioSample(CSampleData * pSample)
{
	if (m_bStop)
		return I_VORC_REC_NO_ERRORS;

	long long ret = 0;
	VO_U32 dwStartTime, dwEndTime;
	pSample->GetTime(dwStartTime, dwEndTime);

	VO_PBYTE pData = NULL;
	CSampleData * pDumpSample = NULL;

	int nSampleLen = pSample->GetData (&pData);
	if (nSampleLen <= 0)
		return I_VORC_REC_MEMORY_FULL;

	if(m_pDump)
		pDumpSample = m_pDump->GetAudioSampleData();

	if(pDumpSample == NULL)
		return I_VORC_REC_MEMORY_FULL;

	VO_PBYTE pOutBuf = NULL;
	pDumpSample->AllocBuf(nSampleLen);
	pDumpSample->GetBuffer(&pOutBuf);
	memcpy(pOutBuf, pSample->GetDataPoint(), nSampleLen);
	pDumpSample->SetTime(dwStartTime, dwEndTime);
	pDumpSample->SetDataLen(nSampleLen);

	ret = m_pDump->ReceiveAudioSample(pDumpSample);

	if(ret != I_VORC_REC_NO_ERRORS)
		return ret;

	return I_VORC_REC_NO_ERRORS;
}

