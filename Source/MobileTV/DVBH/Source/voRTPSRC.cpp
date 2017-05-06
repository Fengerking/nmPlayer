#include "commonheader.h"
#include "ipnetwork.h"
#include "RTPSource.h"

namespace src {

VORC_RTPSRC Open(VO_PTR * ppHandle, char * szSDPData, VO_RTPSRC_INIT_INFO * pInitInfo)
{
	if(!InitWinsock())
		return VORC_RTPSRC_BAD_NETWORK;

	CRTPSource * pRTPSource = new CRTPSource();
	if(pRTPSource == NULL)
		return VORC_RTPSRC_OUT_OF_MEMORY;

	VORC_RTPSRC rc = pRTPSource->Open(szSDPData, pInitInfo);
	if(rc != VORC_RTPSRC_OK)
	{
		SAFE_DELETE(pRTPSource);
		return rc;
	}

	*ppHandle = pRTPSource;

	return VORC_RTPSRC_OK;
}

VORC_RTPSRC Close(VO_PTR pHandle)
{
	CRTPSource * pRTPSource = (CRTPSource *)pHandle;
	if(pRTPSource == NULL)
		return VORC_RTPSRC_INVALID_ARG;

	VORC_RTPSRC rc = pRTPSource->Close();
	SAFE_DELETE(pRTPSource);

	CleanupWinsock();

	return rc;
}

VORC_RTPSRC Start(VO_PTR pHandle)
{
	CRTPSource * pRTPSource = (CRTPSource *)pHandle;
	if(pRTPSource == NULL)
		return VORC_RTPSRC_INVALID_ARG;

	return pRTPSource->Start();
}

VORC_RTPSRC Stop(VO_PTR pHandle)
{
	CRTPSource * pRTPSource = (CRTPSource *)pHandle;
	if(pRTPSource == NULL)
		return VORC_RTPSRC_INVALID_ARG;

	return pRTPSource->Stop();
}

VORC_RTPSRC SetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	CRTPSource * pRTPSource = (CRTPSource *)pHandle;
	if(pRTPSource == NULL)
		return VORC_RTPSRC_INVALID_ARG;

	return pRTPSource->SetParam(uID, pParam);
}

VORC_RTPSRC GetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	CRTPSource * pRTPSource = (CRTPSource *)pHandle;
	if(pRTPSource == NULL)
		return VORC_RTPSRC_INVALID_ARG;

	return pRTPSource->GetParam(uID, pParam);
}

VORC_RTPSRC SendData(VO_PTR pHandle, VO_PBYTE pData, VO_U32 nSize)
{
	CRTPSource * pRTPSource = (CRTPSource *)pHandle;
	if(pRTPSource == NULL)
		return VORC_RTPSRC_INVALID_ARG;

	return pRTPSource->SendData(pData, nSize);
}

} //namespace src

VORC_RTPSRC VO_API voGetRTPSourceAPI(VO_RTPSRC_API * pRTPSource)
{
	pRTPSource->Open = src::Open;
	pRTPSource->Close = src::Close;
	pRTPSource->Start = src::Start;
	pRTPSource->Stop = src::Stop;
	pRTPSource->SetParam = src::SetParam;
	pRTPSource->GetParam = src::GetParam;
	pRTPSource->SendData = src::SendData;

	return VORC_RTPSRC_OK;
}
