#include "CSrc.h"
#if defined(DMEMLEAK)
#  include "dmemleak.h"
#endif
 
#include "CAVEncoder.h"
#include "CSourceSink.h"
#include "CMpeg4Encode.h"
#include "CRecorderEngine.h"
#include "vcamerabase.h"
#include "voOSFunc.h"


unsigned long g_Error = 0;

enum SECRET_PARAMETER
{
	VOID_REC_RENDERCONTROL		= 10001,
	VOID_REC_ENCODEFRMRATE		= 10002,
	VOID_REC_RENDERPERFORMANCE	= 10003,
	VOID_REC_VSFRMRATE			= 10004,
	//VOID_REC_RECORDTIME			= 10005,
	VOID_REC_IMAGEQUALITY		= 10006,
	VOID_REC_RENDER_THREAD		= 10007
};


CRecorderEngine::CRecorderEngine(void)
: m_pAudioSrc(0)
, m_pVideoSrcForEncode(0)
, m_pVideoSrcForPreview(0)
, m_pAVEncoder(0)
, m_pSrcSink(0)
, m_bRecording(false)
, m_dwLastRecordTime(0)
, m_pNotifyCB(0)
{
	//memset(&m_VideoSrcFunc, 0 , sizeof(m_VideoSrcFunc));
	//memset(&m_AudioSrcFunc, 0 , sizeof(m_AudioSrcFunc));
}

CRecorderEngine::~CRecorderEngine(void)
{

}


bool CRecorderEngine::Open()
{
	m_pSrcSink = new CSourceSink;
	m_pVideoSrcForEncode = new CSrc;
	m_pVideoSrcForEncode->SetSrcSink(m_pSrcSink);

	m_pAVEncoder = new CAVEncoder;
	m_pSrcSink->SetAVEncoder(m_pAVEncoder);

	CPerfTest::DumpLog("[REC] open rec engine\r\n");

	return true;
}


bool CRecorderEngine::StopRecord()
{
	if(!m_bRecording)
		return true;

	CPerfTest::DumpLog("[REC] stop recording\r\n");

	CSrc::SetSendStatus(true);

#ifdef _TEST_PERFORMANCE_
	TCHAR debug[100];
	VO_U32 dwStart = voOS_GetSysTime();
#endif

	//close encode
	if(m_pSrcSink)
		m_pSrcSink->StopRecord();

	m_bRecording = false;
	CSrc::SetSendStatus(false);

#ifdef _TEST_PERFORMANCE_
// 	_stprintf(debug, _T("CRecorderEngine::StopRecord() use time = %d.\n"), voOS_GetSysTime()-dwStart);
// 	OutputDebugString(debug);
#endif

	return true;
}


bool CRecorderEngine::Close()
{
	CPerfTest::DumpLog("[REC] close rec engine\r\n");

	//don't send any source data
	CSrc::SetSendStatus(true);

	//stop recording
	StopRecord();

	//close source sink
	m_pSrcSink->Close();
	if(m_pSrcSink)
	{
		delete m_pSrcSink;
		m_pSrcSink = 0;
	}

	//
	if(m_pAVEncoder)
	{
		delete m_pAVEncoder;
		m_pAVEncoder = 0;
	}

	if(m_pAudioSrc)
	{
		delete m_pAudioSrc;
		m_pAudioSrc = 0;
	}

	if(m_pVideoSrcForEncode)
	{
		delete m_pVideoSrcForEncode;
		m_pVideoSrcForEncode = 0;
	}

	if(m_pVideoSrcForPreview)
	{
		delete m_pVideoSrcForPreview;
		m_pVideoSrcForPreview = 0;
	}

	CPerfTest::CloseLog();

	return true;
}

CColorConvert* CRecorderEngine::CreateColorConvert(VO_MMR_VSFORMAT* pFmt, VO_IV_COLORTYPE nOutputVideoType)
{
// 	ROTATIONTYPE rt = pFmt->rotation_type;
// 	CColorConvert* pCC = 0;
// 
// 	if(VOUYVY422_PACKED == pFmt->video_type && VORGB565_PACKED==nOutputVideoType)
// 	{
// 		pCC = new CYUV422PackedUYVY2RGB565;
// 	}
// 	else if(VOYUV_PLANAR420 == pFmt->video_type && VORGB565_PACKED==nOutputVideoType)
// 	{
// 		pCC = new CYUV420Planar2RGB565ColorConvert;
// 	}
// 	else if(VOUYVY422_PACKED == pFmt->video_type && VOYUV_PLANAR420==nOutputVideoType)
// 	{
// 		pCC = new CYUV422PackedUYVY2YUV420Planar;
// 	}
// 	else if(VOVYUY422_PACKED_2 == pFmt->video_type && VOYUV_PLANAR420==nOutputVideoType)
// 	{
// 		pCC = new CYUV422PackedVYUY2YUV420Planar;
// 	}
// 	else if(VORGB565_PACKED == pFmt->video_type && VOYUV_PLANAR420==nOutputVideoType)
// 	{
// 		pCC = new CRGB565Packed2YUV420Planar;
// 	}
// 	else if(VOVYUY422_PACKED_2 == pFmt->video_type && VORGB565_PACKED==nOutputVideoType)
// 	{
// 		pCC = new CYUV422Packed2RGB565;
// 	}
// 	else
// 		return 0;
// 
// 	pCC->Init(pFmt->width, pFmt->height, rt);
// 
// 	return pCC;

	return NULL;
}

bool CRecorderEngine::StartRecord()
{	
	if(m_bRecording)
		return false;

	g_Error = I_VORC_REC_NO_ERRORS;

	if((voOS_GetSysTime()-m_dwLastRecordTime) < 2000)
		return false;
	else
		m_dwLastRecordTime = voOS_GetSysTime();
	
	CPerfTest::DumpLog("[REC] start rec\r\n");

	m_bRecording = true;
	CSrc::Pause(false);
	CSrc::SetSendStatus(true);

	m_pAVEncoder->SetVideoSize(m_fmtRec.width, m_fmtRec.height);
	m_pSrcSink->SetRecordVideoSrcFormat(&m_fmtRec);

	if( (m_fmtRec.video_type!=VO_COLOR_YUV_PLANAR420) 
		&& (m_fmtRec.video_type != VO_COLOR_YVU_PLANAR420)
		&& (m_fmtRec.video_type != VO_COLOR_UYVY422_PACKED)
		&& (m_fmtRec.video_type != VO_COLOR_VYUY422_PACKED_2)
		&& (m_fmtRec.video_type != VO_COLOR_YUV_420_PACK_2)
		&& (m_fmtRec.video_type != VO_COLOR_RGB565_PACKED))
	{
		//CPerfTest::DumpLog("create cc\r\n");

		CColorConvert* pCC = CreateColorConvert(&m_fmtRec, VO_COLOR_YUV_PLANAR420);
		m_pSrcSink->SetEncoderCC(pCC);
	}


	if(!m_pSrcSink->StartRecord())
	{
		StopRecord();
		m_bRecording = false;
		CSrc::SetSendStatus(false);
		return false;
	}

	//if(m_VideoSrcFunc.getparameter)
	{
		long lRate = 15;
		//m_VideoSrcFunc.getparameter(m_VideoSrcFunc.instance, VOID_REC_FRAME_RATE, (long)&lRate);
		CSrc::SetAutoDrop(lRate>=20);
	}
	CSrc::SetSendStatus(false);
	
	return true;
}

bool CRecorderEngine::PauseRecord()
{
	if(!m_bRecording)
		return false;

	CSrc::Pause(true);
	m_pSrcSink->PauseRecord();
	return true;
}


bool CRecorderEngine::RestartRecord()
{
	if(!m_bRecording)
		return false;

	m_pSrcSink->RestartRecord();
	CSrc::Pause(false);
	return true;
}

bool CRecorderEngine::SetParameter(long paramID, void* lValue)
{
	bool bRet = true;

	if(VO_MMR_PID_AUDIOENCODETYPE == paramID)
	{
		VO_U32 val = *(VO_U32*)lValue;
		bRet = m_pAVEncoder->SetAudioEncodeType(val);
	}
	else if(VO_MMR_PID_VIDEOENCODETYPE == paramID)
	{
		VO_U32 val = *(VO_U32*)lValue;
		bRet = m_pAVEncoder->SetVideoEncodeType(val);
	}
	/*else if(VOID_REC_VIDEOFILETYPE == paramID)
	{
		bRet = m_pAVEncoder->SetVideoFileType(lValue);
	}*/
	else if(VO_MMR_PID_CONTENTTYPE == paramID)
	{
		VO_U32 val = *(VO_U32*)lValue;
		m_pSrcSink->SetContentType(val);
	}
	else if(VO_MMR_PID_MPEG4BITRATE == paramID)
	{
		VO_U32 val = *(VO_U32*)lValue;
		m_pAVEncoder->SetMpeg4EncodeBitrate(val);
	}
	else if(VO_MMR_PID_H263BITRATE == paramID)
	{
		VO_U32 val = *(VO_U32*)lValue;
		m_pAVEncoder->SetH263EncodeBitrate(val);
	}
	else if(VO_MMR_PID_VIDEOENCODEFRMRATE == paramID)
	{
		VO_U32 val = *(VO_U32*)lValue;
		m_pAVEncoder->SetVideoEncodeFrameRate(val);
	}
	else if(VO_MMR_PID_RECORDTIMELIMIT == paramID)
	{
		VO_U32 val = *(VO_U32*)lValue;
		m_pSrcSink->SetRecTimeLimit(val*1000);
	}
	else if(VO_MMR_PID_RECORDSIZELIMIT == paramID)
	{
		VO_U32 val = *(VO_U32*)lValue;
		m_pSrcSink->SetRecSizeLimit(val*1024);
	}
	else if(VO_MMR_PID_VIDEOQUALITY == paramID)
	{
		VO_U32 val = *(VO_U32*)lValue;
		m_pSrcSink->SetVideoQuality(val);
	}
	//secret parameter
	else if(VOID_REC_RENDERCONTROL == paramID)
	{
		return false;
	}
	else if(1000 == paramID)
	{
		//m_pSrcSink->StopRender();
		//CaptureImg();
	}
	else if(VOID_REC_IMAGEQUALITY == paramID)
	{
		VO_U32 val = *(VO_U32*)lValue;
		m_pSrcSink->SetImageQuality(val);
	}
	else if(VO_MMR_PID_VIDEOSRCFMT == paramID)
	{
		VO_MMR_VSFORMAT* pFmt = (VO_MMR_VSFORMAT*)lValue;
		m_fmtRec.width = pFmt->width;
		m_fmtRec.height = pFmt->height;
		m_fmtRec.rotation_type = pFmt->rotation_type;
		m_fmtRec.video_type = pFmt->video_type;
	}
	else if(VO_MMR_PID_AUDIOSRCFMT == paramID)
	{
		VO_MMR_ASFORMAT* pFmt = (VO_MMR_ASFORMAT*)lValue;
		m_pSrcSink->SetAudioFmt(pFmt);
	}
	else if (VOID_REC_RENDER_THREAD == paramID)
	{
#ifdef _TEST_THREAD_PERFORMANCE_
		m_pSrcSink->m_hRenderThread = (HANDLE)lValue;
#endif
	}
		
	return bRet;
}


bool CRecorderEngine::GetParameter(long paramID, void* pValue)
{
	if(!m_pAVEncoder || !m_pSrcSink)
		return false;

	bool bRet = false;

	if(VO_MMR_PID_AUDIOENCODETYPE == paramID)
	{
		bRet = m_pAVEncoder->GetAudioEncodeType((long*)pValue);
	}
	else if(VO_MMR_PID_VIDEOENCODETYPE == paramID)
	{
		bRet = m_pAVEncoder->GetVideoEncodeType((long*)pValue);
	}
	/*else if(VOID_REC_VIDEOFILETYPE == paramID)
	{
		bRet = m_pAVEncoder->GetVideoFileType(pValue);
	}*/
	else if(VO_MMR_PID_CONTENTTYPE == paramID)
	{
		*(VO_U32*)pValue = m_pSrcSink->GetContentType();
		bRet = true;
	}
	else if(VO_MMR_PID_MPEG4BITRATE == paramID)
	{
		*(VO_U32*)pValue = m_pAVEncoder->GetMpeg4EncodeBitrate();
	}
	else if(VO_MMR_PID_H263BITRATE == paramID)
	{
		*(VO_U32*)pValue = m_pAVEncoder->GetH263EncodeBitrate();
	}
	else if(VO_MMR_PID_RECORDTIMELIMIT == paramID)
	{
		if(m_pSrcSink)
		{
			*(VO_U32*)pValue = m_pSrcSink->GetRecTimeLimit();
		}
	}
	else if(VO_MMR_PID_RECORDSIZELIMIT == paramID)
	{
		if(m_pSrcSink)
		{
			//*pValue = m_pSrcSink->GetRecSizeLimit();
		}
	}
	else if(VO_MMR_PID_CURRFILESIZE == paramID)
	{
		*(VO_U32*)pValue = m_pAVEncoder->GetDumpSize();
	}
	//secret interface
	else if(VOID_REC_ENCODEFRMRATE == paramID)
	{
		//get frame rate
		bRet = m_pAVEncoder->GetFrameRate((float*)pValue);
	}
	else if(VOID_REC_VSFRMRATE == paramID)
	{
		//get real-time video source frame rate
		bRet = m_pSrcSink->GetVideoSrcFrameRate((float*)pValue);
	}
	else if(VOID_REC_RENDERPERFORMANCE == paramID)
	{
		return false;
	}
	else if(VO_MMR_PID_RECORDTIME == paramID)
	{
		//get current recording time
		//(*(long*)pValue) = m_bRecording?(voOS_GetSysTime()-m_dwLastRecordTime)/1000.0:0.0;
		
		if(m_pSrcSink)
			(*(VO_U32*)pValue) = m_bRecording?m_pSrcSink->GetCurrRecTime()/1000.0:0.0;
	}
	else if(VOID_REC_RENDERCONTROL == paramID)
	{
		//
		return false;
	}
		
	return bRet;
}

void CRecorderEngine::SetNotifyCallback(VOMMRecordCallBack pProc)
{
	m_pNotifyCB = pProc;

	if(m_pSrcSink)
		m_pSrcSink->SetNotifyProc(pProc);
	
	if(m_pAVEncoder)
		m_pAVEncoder->SetNotifyCallback(pProc);
}

void CRecorderEngine::SetFileName(TCHAR* pFileDir)
{
	DeleteTmpFile(pFileDir);

	if(m_pAVEncoder)
		m_pAVEncoder->SetVideoFileDirectory(pFileDir);

}

void CRecorderEngine::DeleteTmpFile(TCHAR* pFileDir)
{
#ifdef _WIN32
	if(pFileDir)
	{
		TCHAR szDir[MAX_PATH];
		_tcscpy(szDir, pFileDir);
		for (int i = _tcslen (szDir); i >= 0; i--)
		{
			if (szDir[i] == '\\')
			{
				szDir[i+1] = '\0';
				break;
			}
		}

		TCHAR szTmpFile[MAX_PATH];
		_tcscpy(szTmpFile, szDir);
		_tcscat(szTmpFile, _T("*.tmp"));

		bool bRet = true;
		WIN32_FIND_DATA  data;
		HANDLE hFind = FindFirstFile(szTmpFile, &data);

		while(hFind && bRet)
		{
			swprintf(szTmpFile, _T("%s%s"), szDir, data.cFileName);
			::DeleteFile(szTmpFile);
			bRet = FindNextFile(hFind, &data);
		}

		FindClose(hFind);
	}
#endif
}


void CRecorderEngine::RecvAudioSrc(VO_MMR_ASBUFFER* pBuffer)
{
	//if(m_bRecording)
		CSrc::AudioSrcCallback(pBuffer);
}

void CRecorderEngine::RecvVideoSrc(VO_MMR_VSBUFFER* pBuffer)
{
	if(m_bRecording)
		CSrc::RecordVideoSrcCallback(pBuffer);
}



