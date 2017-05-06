#include "CMTPSDK.h"
#include "CMFWvoME2.h"
#include "cmnMemory.h"
#include "CLiveSourceBase.h"
#include "CRecMng.h"
#include "voLog.h"


CMTPSDK::CMTPSDK(void)
:m_pMFW(NULL)
,m_pRec(NULL)
,m_pSrc(NULL)
,m_nChannelCount(0)
,m_pChannelInfo(NULL)
{
	cmnMemSet(0, &m_OpenParam, 0, sizeof(VO_MTP_OPEN_PARAM));
}

CMTPSDK::~CMTPSDK(void)
{
}

//
VO_VOID MFWStatusProc(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	((CMTPSDK*)pUserData)->ProcessMFWStatusCallback(nID, nParam1, nParam2);
}
VO_VOID CMTPSDK::ProcessMFWStatusCallback(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
}

//
VO_VOID MFWOutputCallback(VO_PTR pUserData, MFW_OUTPUT_BUFFER* pSample)
{
	((CMTPSDK*)pUserData)->ProcessMFWOutputCallback(pSample);
}

VO_VOID CMTPSDK::ProcessMFWOutputCallback(MFW_OUTPUT_BUFFER* pSample)
{
	if(m_OpenParam.nTransferMode == MTP_PUSH_MODE)
	{
		if (pSample->bVideo)
		{
			MTP_VIDEO_BUFFER video;
			video.buf.Time		= pSample->llTime;
			video.buf.ColorType	= (VO_IV_COLORTYPE)pSample->nColorType;
			video.buf.Buffer[0]	= pSample->pBuffer[0];
			video.buf.Buffer[1]	= pSample->pBuffer[1];
			video.buf.Buffer[2]	= pSample->pBuffer[2];
			video.buf.Stride[0]	= pSample->nStride[0];
			video.buf.Stride[1]	= pSample->nStride[1];
			video.buf.Stride[2]	= pSample->nStride[2];
			video.fmt			= (VO_VIDEO_FORMAT*)pSample->nReserved1;
			if (m_OpenParam.fSendData)
			{
				//pSample->nReserved1 is VO_VIDEO_FORMAT
				m_OpenParam.fSendData(m_OpenParam.pUserData, VO_MTP_OUTPUT_VIDEO, &video, 0);
			}
		}
		else
		{
			MTP_AUDIO_BUFFER buf;
			buf.pBuffer		= pSample->pBuffer[0];
			buf.nSize		= pSample->nSize;
			buf.llTime		= pSample->llTime;
			buf.fmt			= (VO_AUDIO_FORMAT*)pSample->nReserved1;

			if (m_OpenParam.fSendData)
			{
				m_OpenParam.fSendData(m_OpenParam.pUserData, VO_MTP_OUTPUT_AUDIO, &buf, 0);
			}
		}
	}
}


//
VO_VOID SrcStatusCallback(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	((CMTPSDK*)pUserData)->ProcessSrcStatusCallback(nID, nParam1, nParam2);
}
VO_VOID CMTPSDK::ProcessSrcStatusCallback(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	if (VO_LIVESRC_STATUS_SCAN_PROCESS == nID)
	{
		NotifyStatus(VO_MTP_STATUS_SCAN_PROCESS, nParam1, nParam2);
	}
	else if (VO_LIVESRC_STATUS_CHANNEL_STOP == nID)
	{
	}
	else if (VO_LIVESRC_STATUS_CHANNEL_START == nID)
	{
	}
	else if (VO_LIVESRC_STATUS_DEVICE_NOTREADY == nID)
	{
	}
	else if (VO_LIVESRC_STATUS_DEVICE_ERROR == nID)
	{
		NotifyStatus(VO_MTP_STATUS_DEVICE_ERROR, nParam1, nParam2);
	}
	else if (VO_LIVESRC_STATUS_NO_SIGNAL == nID)
	{
		NotifyStatus(VO_MTP_STATUS_NO_SIGNAL, nParam1, nParam2);
	}
	else if (VO_LIVESRC_STATUS_DEVICE_INIT_OK == nID)
	{
	}
	else if (VO_LIVESRC_STATUS_CODEC_CHANGED == nID)
	{
	}
	else if (VO_LIVESRC_STATUS_BITRATE_CHANGE == nID)
	{
	}
	else if (VO_LIVESRC_STATUS_NEED_FLUSH == nID)
	{
	}
}

VO_VOID CMTPSDK::NotifyStatus(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	if (m_OpenParam.fStatus)
	{
		m_OpenParam.fStatus(m_OpenParam.pUserData, nID, nParam1, nParam2);
	}
}

//
VO_VOID SrcSendDataCallback(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{
	((CMTPSDK*)pUserData)->ProcessSrcSendDataCallback(nOutputType, pData);
}
VO_VOID CMTPSDK::ProcessSrcSendDataCallback(VO_U16 nOutputType, VO_PTR pData)
{
	if (VO_LIVESRC_OUTPUT_AUDIO == nOutputType)
	{
		if (m_pMFW)
		{
			VO_LIVESRC_SAMPLE* pSample		= (VO_LIVESRC_SAMPLE*)pData;
			VO_SOURCE_SAMPLE* src_sample	= &pSample->Sample;

			MFW_SAMPLE mfw_sample;
			mfw_sample.bVideo		= VO_FALSE;
			mfw_sample.llTime		= src_sample->Time;
			mfw_sample.nCodecType	= pSample->nCodecType;
			mfw_sample.pBuffer		= src_sample->Buffer;
			mfw_sample.nSize		= src_sample->Size;
			mfw_sample.nTrackID		= pSample->nTrackID;
			mfw_sample.nFlag		= pSample->Sample.Flag;
			mfw_sample.nReserve		= 0;

			m_pMFW->SendBuffer(&mfw_sample);
		}

	}
	else if (VO_LIVESRC_OUTPUT_VIDEO == nOutputType)
	{
		if (m_pMFW)
		{
			VO_LIVESRC_SAMPLE* pSample		= (VO_LIVESRC_SAMPLE*)pData;
			VO_SOURCE_SAMPLE* src_sample	= &pSample->Sample;

			MFW_SAMPLE mfw_sample;
			mfw_sample.bVideo		= VO_TRUE;
			mfw_sample.llTime		= src_sample->Time;
			mfw_sample.nCodecType	= pSample->nCodecType;
			mfw_sample.pBuffer		= src_sample->Buffer;
			mfw_sample.nSize		= src_sample->Size;
			mfw_sample.nTrackID		= pSample->nTrackID;
			mfw_sample.nFlag		= pSample->Sample.Flag;
			mfw_sample.nReserve		= 0;

			m_pMFW->SendBuffer(&mfw_sample);
		}
	}
	else if (VO_LIVESRC_OUTPUT_DATABLOCK == nOutputType)
	{
		VO_LIVESRC_DATA_SERVICE* pDataService = (VO_LIVESRC_DATA_SERVICE*)pData;
		if (m_OpenParam.fSendData)
		{
			VO_MTP_DATA_SERVICE data_service;
			data_service.pData		= pDataService->pData;
			data_service.nDataLen	= pDataService->nDataLen;
			data_service.nReserved	= pDataService->nReserved;
			m_OpenParam.fSendData(m_OpenParam.pUserData, VO_MTP_OUTPUT_DATABLOCK, &data_service, 0);
		}
	}
	else if (VO_LIVESRC_OUTPUT_TRACKINFO == nOutputType)
	{
		VO_LIVESRC_TRACK_INFO* pTrackInfo = (VO_LIVESRC_TRACK_INFO*)pData;

		if (m_pMFW)
		{
			m_pMFW->OnTrackInfo(pTrackInfo);
		}
	}
}


VO_U32 CMTPSDK::Open (VO_MTP_OPEN_PARAM* pOpenParam)
{
	Close();
	
	cmnMemCopy(0, &m_OpenParam, pOpenParam, sizeof(VO_MTP_OPEN_PARAM));
	
	VO_U32 rc	= OpenMFW();
	rc			= OpenSource();
	
	return rc;
}

VO_U32 CMTPSDK::Close ()
{
	CloseSource();

	CloseMFW();

	ReleaseChannelInfo();

	cmnMemSet(0, &m_OpenParam, 0, sizeof(VO_MTP_OPEN_PARAM));

	return VO_ERR_MTP_OK;;	
}

VO_U32 CMTPSDK::SetDataSource (VO_PTR pSource, VO_U32 nSourceFormat)
{
	if(!m_pMFW || !m_pSrc)
	{
		VOLOGI("[MTP] Set data source FAILED.");
		return VO_ERR_MTP_NULLPOINTER;
	}


	VO_U32 rc = m_pMFW->SetDataSource(NULL);

	// 0 is asynchronous, 1 is sync, just for HTTP Live Streaming
	rc = m_pSrc->SetDataSource(pSource, 0);

	if (rc != VO_ERR_LIVESRC_OK && rc!= VO_ERR_LIVESRC_NOIMPLEMENT)
	{
		return LiveSrcErr2MtpErr(rc);
	}


	return rc;
}

VO_U32 CMTPSDK::SetDrawArea (VO_U16 nLeft, VO_U16 nTop, VO_U16 nRight, VO_U16 nBottom)
{
	if(!m_pMFW)
		return VO_ERR_MTP_NULLPOINTER;

	VO_U32 rc = m_pMFW->SetDrawArea(nLeft, nTop, nRight, nBottom);

	return rc;
}

VO_U32 CMTPSDK::Scan (VO_MTP_SCAN_PARAMEPTERS* pParam)
{
	if(!m_pSrc)
		return VO_ERR_MTP_SRC_NOT_READY;

	VO_LIVESRC_SCAN_PARAMEPTERS param;
	param.pParam	= pParam->pParam;
	param.tEngine	= MtpType2SrcType(pParam->tSourceFormat);
	VO_U32 rc =  m_pSrc->Scan(&param);

	if (rc != VO_ERR_LIVESRC_OK)
	{
		return LiveSrcErr2MtpErr(rc);
	}

	return rc;
}

VO_U32 CMTPSDK::Cancel (VO_U32 nID)
{
	if(!m_pSrc)
		return VO_ERR_MTP_SRC_NOT_READY;
	VO_U32 rc = m_pSrc->Cancel(nID);

	if (rc != VO_ERR_LIVESRC_OK)
	{
		return LiveSrcErr2MtpErr(rc);
	}

	return rc;
}

VO_U32 CMTPSDK::Seek (VO_U64 nPosition)
{
	if(!m_pSrc)
		return VO_ERR_MTP_SRC_NOT_READY;

	VO_U32 rc = m_pSrc->Seek(nPosition);

	if (rc != VO_ERR_LIVESRC_OK)
	{
		return LiveSrcErr2MtpErr(rc);
	}

	if(!m_pMFW)
		return VO_ERR_MTP_NULLPOINTER;

	rc = m_pMFW->Seek(nPosition);

	return rc;
}

VO_U32 CMTPSDK::GetPosition (VO_U64* pPosition)
{
	if(!m_pMFW)
		return VO_ERR_MTP_NULLPOINTER;

	VO_U32 rc = m_pMFW->GetPosition(pPosition);

	return rc;
}

VO_U32 CMTPSDK::Start ()
{
	if(!m_pMFW)
		return VO_ERR_MTP_NULLPOINTER;

	VO_U32 rc = m_pMFW->Start();

	if (rc != VOMP_ERR_None)
	{
		VOLOGI("VOME start failed. error code = %x", rc);
		return rc;
	}

	if(!m_pSrc)
		return VO_ERR_MTP_SRC_NOT_READY;
	
	rc = m_pSrc->Start();

	if (rc != VO_ERR_LIVESRC_OK)
	{
		return LiveSrcErr2MtpErr(rc);
	}

	return rc;
}

VO_U32 CMTPSDK::Stop ()
{
	if(!m_pMFW)
		return VO_ERR_MTP_NULLPOINTER;

	VO_U32 rc = m_pMFW->Stop();

	if(!m_pSrc)
		return VO_ERR_MTP_SRC_NOT_READY;
	
	rc = m_pSrc->Stop();

	if (rc != VO_ERR_LIVESRC_OK)
	{
		return LiveSrcErr2MtpErr(rc);
	}
	return rc;
}

VO_U32 CMTPSDK::GetChannel (VO_U32* pCount, VO_MTP_CHANNELINFO** ppChannelInfo)
{
	if(!m_pSrc)
		return VO_ERR_MTP_SRC_NOT_READY;
		
	VO_U32 total	= 0;
	VO_LIVESRC_CHANNELINFO* pChannelList = NULL;
	VO_U32 rc		= m_pSrc->GetChannel(&total, &pChannelList);
	if(total <= 0 && rc != VO_ERR_LIVESRC_OK)
		return VO_ERR_MTP_NO_CHANNEL;

	ReleaseChannelInfo();

	m_nChannelCount	= total;
	m_pChannelInfo	= new VO_MTP_CHANNELINFO[total];

	for (VO_U32 n=0; n<total; n++)
	{
		m_pChannelInfo[n].hIcon			= pChannelList[n].hIcon;
		m_pChannelInfo[n].nChannelID	= pChannelList[n].nChannelID;
		m_pChannelInfo[n].nCurrent		= pChannelList[n].nCurrent;
		m_pChannelInfo[n].nType			= SrcType2MtpType(pChannelList[n].nType);
		m_pChannelInfo[n].pData			= pChannelList[n].pData;
		vostrcpy(m_pChannelInfo[n].szName, pChannelList[n].szName);
	}

	*pCount			= m_nChannelCount;
	*ppChannelInfo	= m_pChannelInfo;

	return rc;
}

VO_U32 CMTPSDK::SetChannel (VO_S32 nChannelID)
{
	if(!m_pSrc)
		return VO_ERR_MTP_SRC_NOT_READY;
	VO_U32 rc = m_pSrc->SetChannel(nChannelID);

	if (rc != VO_ERR_LIVESRC_OK)
	{
		return LiveSrcErr2MtpErr(rc);
	}

	if(m_pMFW)
		m_pMFW->Flush();

	return rc;
}

VO_U32 CMTPSDK::GetESG (VO_S32 nChannelID, VO_MTP_ESG_INFO* pESGInfo)
{
	if(!m_pSrc)
		return VO_ERR_MTP_SRC_NOT_READY;

	VO_LIVESRC_ESG_INFO* pInfo = NULL;
	//cmnMemSet(0, &info, 0, sizeof(VO_LIVESRC_ESG_INFO));
	VO_U32 rc = m_pSrc->GetESG(nChannelID, &pInfo);

	if (!pInfo || !pInfo->pEsgInfo)
		return VO_ERR_MTP_NO_ESG;

	pESGInfo->pEsgInfo		= pInfo->pEsgInfo;
	pESGInfo->tSourceFormat	= SrcType2MtpType(pInfo->tEngine);

	return rc;
}

VO_U32 CMTPSDK::AudioControl (VO_BOOL bSuspend)
{
	return VO_ERR_MTP_NOT_IMPLEMENT;
}

VO_U32 CMTPSDK::GetVolume (VO_U16* pVolume, VO_U16* pMaxVolume)
{
	return VO_ERR_MTP_NOT_IMPLEMENT;
}

VO_U32 CMTPSDK::SetVolume (VO_U16 nVolume)
{
	return VO_ERR_MTP_NOT_IMPLEMENT;
}

VO_U32 CMTPSDK::RecStart (TCHAR* pRecFile)
{
	if(m_pRec)
		return VO_ERR_MTP_FAIL;

	m_pRec = new CRecMng;
	return m_pRec->RecStart(pRecFile, REC_MP4);
}

VO_U32 CMTPSDK::RecStop ()
{
	if(m_pRec)
	{	
		m_pRec->RecStop();
		delete m_pRec;
		m_pRec = NULL;
		return VO_ERR_MTP_OK;
	}
		
	return VO_ERR_MTP_FAIL;
}

VO_U32 CMTPSDK::GetParam (VO_U32 nParamID, VO_PTR pParam)
{
	return VO_ERR_MTP_NOT_IMPLEMENT;
}

VO_U32 CMTPSDK::SetParam (VO_U32 nParamID, VO_PTR pParam)
{
	if (nParamID == VO_MTP_PID_COLOR_TYPE
		|| nParamID == VO_MTP_PID_AUDIO_SINK_BUFFTIME)
	{
		if (m_pMFW)
		{
			return m_pMFW->SetParam(nParamID, pParam);
		}
	}
	return VO_ERR_MTP_PARAM_ID_ERR;
}


VO_U32 CMTPSDK::OpenSource()
{
	m_pSrc		= new CLiveSourceBase(m_OpenParam.nSourceFormat);
	VO_U32 rc	= m_pSrc->Open(this, SrcStatusCallback, SrcSendDataCallback);
	return rc;
}

VO_U32 CMTPSDK::CloseSource()
{
	if (m_pSrc)
	{
		m_pSrc->Stop();
		m_pSrc->Close();
		delete m_pSrc;
		m_pSrc = NULL;
	}

	return VO_ERR_MTP_OK;
}

VO_U32 CMTPSDK::OpenMFW()
{
	m_pMFW				= new CMFWvoME2;

	MFW_OPEN_PARAM	param;
	param.fOutputData	= MFWOutputCallback;
	param.fStatus		= MFWStatusProc;
	param.hDrawWnd		= m_OpenParam.hDrawWnd;
	param.pUserData		= this;
	param.nTrasferMode	= m_OpenParam.nTransferMode;

	VO_U32 rc			= m_pMFW->Open(&param);
	
	return rc;
}

VO_U32 CMTPSDK::CloseMFW()
{
	if(m_pMFW)
	{
		m_pMFW->Close();
		delete m_pMFW;
		m_pMFW = NULL;
	}
	return VO_ERR_MTP_OK;
}

VO_U32 CMTPSDK::ReleaseChannelInfo()
{
	m_nChannelCount = 0;
	delete []m_pChannelInfo;
	m_pChannelInfo = NULL;
	return VO_ERR_MTP_OK;
}

VO_U32 CMTPSDK::ReleaseRecMng()
{
	if(m_pRec)
	{
		m_pRec->RecStop();
		delete m_pRec;
		m_pRec = NULL;
	}
	return VO_ERR_MTP_OK;
}


VO_LIVESRC_FORMATTYPE CMTPSDK::MtpType2SrcType(VO_MTP_SOURCE_FORMAT type)
{
	if (VO_MTP_FT_FILE == type)
	{
		return VO_LIVESRC_FT_FILE; 
	}
	else if(VO_MTP_FT_TDMB == type)
	{
		return VO_LIVESRC_FT_TDMB; 
	}
	else if(VO_MTP_FT_DAB == type)
	{
		return VO_LIVESRC_FT_DAB; 
	}
	else if(VO_MTP_FT_DVBH == type)
	{
		return VO_LIVESRC_FT_DVBH; 
	}
	else if(VO_MTP_FT_ISDBT == type)
	{
		return VO_LIVESRC_FT_ISDBT; 
	}
	else if(VO_MTP_FT_DVBT == type)
	{
		return VO_LIVESRC_FT_DVBT; 
	}
	else if(VO_MTP_FT_DMBT == type)
	{
		return VO_LIVESRC_FT_DMBT; 
	}
	else if(VO_MTP_FT_DVBHTS == type)
	{
		return VO_LIVESRC_FT_DVBHTS; 
	}
	else if(VO_MTP_FT_ATSCMH == type)
	{
		return VO_LIVESRC_FT_ATSCMH; 
	}
	else if(VO_MTP_FT_CMMB == type)
	{
		return VO_LIVESRC_FT_CMMB; 
	}
	else if(VO_MTP_FT_HTTP_LS == type)
	{
		return VO_LIVESRC_FT_HTTP_LS; 
	}
}

VO_MTP_SOURCE_FORMAT CMTPSDK::SrcType2MtpType(VO_LIVESRC_FORMATTYPE type)
{
	if (VO_LIVESRC_FT_FILE == type)
	{
		return VO_MTP_FT_FILE; 
	}
	else if(VO_LIVESRC_FT_TDMB == type)
	{
		return VO_MTP_FT_TDMB; 
	}
	else if(VO_LIVESRC_FT_DAB == type)
	{
		return VO_MTP_FT_DAB; 
	}
	else if(VO_LIVESRC_FT_DVBH == type)
	{
		return VO_MTP_FT_DVBH; 
	}
	else if(VO_LIVESRC_FT_ISDBT == type)
	{
		return VO_MTP_FT_ISDBT; 
	}
	else if(VO_LIVESRC_FT_DVBT == type)
	{
		return VO_MTP_FT_DVBT; 
	}
	else if(VO_LIVESRC_FT_DMBT == type)
	{
		return VO_MTP_FT_DMBT; 
	}
	else if(VO_LIVESRC_FT_DVBHTS == type)
	{
		return VO_MTP_FT_DVBHTS; 
	}
	else if(VO_LIVESRC_FT_ATSCMH == type)
	{
		return VO_MTP_FT_ATSCMH; 
	}
	else if(VO_LIVESRC_FT_CMMB == type)
	{
		return VO_MTP_FT_CMMB; 
	}
	else if(VO_LIVESRC_FT_HTTP_LS == type)
	{
		return VO_MTP_FT_HTTP_LS; 
	}
}

VO_U32 CMTPSDK::LiveSrcErr2MtpErr(VO_U32 nLiveSrcErr)
{
	if (VO_ERR_LIVESRC_FAIL == nLiveSrcErr)
	{
		return VO_ERR_MTP_FAIL;
	}
	else if (VO_ERR_LIVESRC_OUTOFMEMORY == nLiveSrcErr)
	{
		return VO_ERR_MTP_OUTOFMEMORY;
	}
	else if (VO_ERR_LIVESRC_NULLPOINTER == nLiveSrcErr)
	{
		return VO_ERR_MTP_NULLPOINTER;
	}
	else if (VO_ERR_LIVESRC_INVALIDARG == nLiveSrcErr)
	{
		return VO_ERR_MTP_INVALIDARG;
	}
	else if (VO_ERR_LIVESRC_LOADDRIVERFAILED == nLiveSrcErr)
	{
		return VO_ERR_MTP_LOAD_DRV_FAIL;
	}
	else if (VO_ERR_LIVESRC_NO_CHANNEL == nLiveSrcErr)
	{
		return VO_ERR_MTP_NO_CHANNEL;
	}
	else if (VO_ERR_LIVESRC_NO_DATA == nLiveSrcErr)
	{
		return VO_ERR_MTP_NO_DATA;
	}
	else if (VO_ERR_LIVESRC_NOIMPLEMENT == nLiveSrcErr)
	{
		return VO_ERR_MTP_NOT_IMPLEMENT;
	}
}



