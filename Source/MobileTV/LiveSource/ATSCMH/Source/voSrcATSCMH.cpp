// voSrcATSCMH.cpp : Defines the exported functions for the DLL application.
//

#include "commonheader.h"
#include "vortpsrc.h"
#include "RTPSourceWarper.h"
//#include <crtdbg.h> 
//#define   _CRTDBG_MAP_ALLOC 

namespace RtpSrc{

	VO_U32 voSrcOpen(VO_HANDLE * phLiveSrc, VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
	{
	//	_CrtSetDbgFlag   (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		CRTPSourceWarper * pRTPSourceWraper= new CRTPSourceWarper();
		if(pRTPSourceWraper == NULL)
			return VORC_RTPSRC_OUT_OF_MEMORY;

		VORC_RTPSRC rc = pRTPSourceWraper->voRtpSrcOpen(pUserData,fStatus,fSendData);
		if(rc != VORC_RTPSRC_OK)
		{
			SAFE_DELETE(pRTPSourceWraper);
			return rc;
		}

		*phLiveSrc = pRTPSourceWraper;
		return VORC_RTPSRC_OK;
	}
	VO_U32 voSrcClose(VO_HANDLE hLiveSrc)
	{
		CRTPSourceWarper * pRTPSourceWraper = (CRTPSourceWarper *)hLiveSrc;
		if(pRTPSourceWraper == NULL)
			return VORC_RTPSRC_INVALID_ARG;

		VORC_RTPSRC rc = pRTPSourceWraper->voRtpSrcClose();
		SAFE_DELETE(pRTPSourceWraper);
		return rc;
	}
	VO_U32 voSrcScan(VO_HANDLE hLiveSrc, VO_LIVESRC_SCAN_PARAMEPTERS* pParam)
	{
		return 0;
	}
	VO_U32 voSrcCancel(VO_HANDLE hLiveSrc, VO_U32 nID)
	{
		return 0;
	}
	VO_U32 voSrcStart(VO_HANDLE hLiveSrc)
	{
		CRTPSourceWarper * pRTPSourceWraper = (CRTPSourceWarper *)hLiveSrc;
		if(pRTPSourceWraper == NULL)
			return VORC_RTPSRC_INVALID_ARG;

		return pRTPSourceWraper->voRtpSrcStart();
	}
	VO_U32 voSrcPause(VO_HANDLE hLiveSrc)
	{
		return 0;
	}
	VO_U32 voSrcStop(VO_HANDLE hLiveSrc)
	{
		CRTPSourceWarper * pRTPSourceWraper = (CRTPSourceWarper *)hLiveSrc;
		if(pRTPSourceWraper == NULL)
			return VORC_RTPSRC_INVALID_ARG;

		return pRTPSourceWraper->voRtpSrcStop();
	}
	VO_U32 voSrcSetDataSource(VO_HANDLE hLiveSrc, VO_PTR pSource, VO_U32 nFlag)
	{
		return 0;
	}
	VO_U32 voSrcSeek(VO_HANDLE hLiveSrc, VO_U64 nTimeStamp)
	{
		return 0;
	}
	VO_U32 voSrcGetChannel(VO_HANDLE hLiveSrc, VO_U32* pCount, VO_LIVESRC_CHANNELINFO** ppChannelInfo)
	{
		return 0;
	}
	VO_U32 voSrcSetChannel(VO_HANDLE hLiveSrc, VO_S32 nChannelID)
	{
		return 0;
	}
	VO_U32 voSrcGetESG(VO_HANDLE hLiveSrc, VO_S32 nChannelID, VO_LIVESRC_ESG_INFO** ppESGInfo)
	{
		return 0;
	}
	VO_U32 voSrcGetParam(VO_HANDLE hLiveSrc, VO_U32 nParamID, VO_PTR pParam)
	{
		CRTPSourceWarper * pRTPSourceWraper = (CRTPSourceWarper *)hLiveSrc;
		if(pRTPSourceWraper == NULL)
			return VORC_RTPSRC_INVALID_ARG;

		return pRTPSourceWraper->voRtpSrcGetParam(nParamID, pParam);
	}
	VO_U32 voSrcSetParam(VO_HANDLE hLiveSrc, VO_U32 nParamID, VO_PTR pParam)
	{
		CRTPSourceWarper * pRTPSourceWraper = (CRTPSourceWarper *)hLiveSrc;

		if(pRTPSourceWraper == NULL)
			return VORC_RTPSRC_INVALID_ARG;

		return pRTPSourceWraper->voRtpSrcSetParam(nParamID, pParam);
	}
}//namespace RtpSrc
VO_S32 VO_API voGetLiveSrcAPI(VO_LIVESRC_API* pHandle)
{
	(*pHandle).Open = RtpSrc::voSrcOpen;
	(*pHandle).Close = RtpSrc::voSrcClose;

	(*pHandle).Scan = RtpSrc::voSrcScan;
	(*pHandle).Pause = RtpSrc::voSrcPause;
	(*pHandle).Cancel = RtpSrc::voSrcCancel;
	(*pHandle).Start = RtpSrc::voSrcStart;
	(*pHandle).Stop = RtpSrc::voSrcStop;
	(*pHandle).SetDataSource = RtpSrc::voSrcSetDataSource;

	(*pHandle).Seek = RtpSrc::voSrcSeek;
	(*pHandle).GetChannel = RtpSrc::voSrcGetChannel;
	(*pHandle).SetChannel = RtpSrc::voSrcSetChannel;

	(*pHandle).GetESG = RtpSrc::voSrcGetESG;
	(*pHandle).GetParam = RtpSrc::voSrcGetParam;
	(*pHandle).SetParam = RtpSrc::voSrcSetParam;
	return VORC_RTPSRC_OK;
}

