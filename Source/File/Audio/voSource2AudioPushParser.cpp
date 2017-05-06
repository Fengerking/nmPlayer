#include "CMp3PushParser.h"
#include "voLog.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* _VONAMESPACE*/

VO_U32 voAudioPushParserInit(VO_HANDLE * phHandle, VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pCallback )
{
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = new CMp3PushParser();
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->Init(pSource, nFlag, pCallback);
	if (uRet != VO_RET_SOURCE2_OK)
	{
		pSrc->Uninit();
		delete pSrc;
		return uRet;
	}

	*phHandle = (VO_HANDLE)pSrc;
	return uRet;
}


VO_U32 voAudioPushParserUninit(VO_HANDLE hHandle)
{
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	VO_U32 uRet = pSrc->Uninit();

	delete	pSrc;
	return uRet;
}


VO_U32 voAudioPushParserOpen(VO_HANDLE hHandle)
{	
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->Open();
	return uRet;
}

VO_U32 voAudioPushParserClose(VO_HANDLE hHandle)
{
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->Close();
	return uRet;
}


VO_U32 voAudioPushParserStart(VO_HANDLE hHandle)
{
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->Start();
	return uRet;
}

VO_U32 voAudioPushParserPause(VO_HANDLE hHandle)
{
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->Pause();
	return uRet;
}

VO_U32 voAudioPushParserStop(VO_HANDLE hHandle)
{
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->Stop();
	return uRet;
}

VO_U32 voAudioPushParserSeek(VO_HANDLE hHandle, VO_U64* pTimeStamp)
{
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->Seek(pTimeStamp);
	return uRet;
}

VO_U32 voAudioPushParserGetDuration(VO_HANDLE hHandle,VO_U64 * pDuration)
{
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->GetDuration(pDuration);
	return uRet;
}


VO_U32 voAudioPushParserGetSample(VO_HANDLE hHandle , VO_SOURCE2_TRACK_TYPE nOutPutType , VO_PTR pSample)
{
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->GetSample(nOutPutType, pSample);
	return uRet;
}

VO_U32 voAudioPushParserGetProgramCount(VO_HANDLE hHandle, VO_U32 *pStreamCount)
{
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->GetProgramCount(pStreamCount);
	return uRet;
}

VO_U32 voAudioPushParserGetProgramInfo(VO_HANDLE hHandle, VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo)
{
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->GetProgramInfo(nProgram, pProgramInfo);
	return uRet;
}

VO_U32 voAudioPushParserGetCurTrackInfo(VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo )
{
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->GetCurTrackInfo(nTrackType, ppTrackInfo);
	return uRet;
}

VO_U32 voAudioPushParserSelectProgram (VO_HANDLE hHandle, VO_U32 nProgram)
{
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->SelectProgram(nProgram);
	return uRet;
}

VO_U32 voAudioPushParserSelectStream (VO_HANDLE hHandle, VO_U32 nProgram)
{
	VO_U32	 uRet = VO_RET_SOURCE2_OK;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->SelectStream(nProgram);
	return uRet;
}

VO_U32 voAudioPushParserSelectTrack(VO_HANDLE hHandle, VO_U32 nTrack)
{
	VO_U32	 uRet = 0;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->SelectTrack(nTrack);
	return uRet;
}


VO_U32	voAudioPushParserGetDRMInfo(VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
	VO_U32	 uRet = 0;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->GetDRMInfo(ppDRMInfo);
	return uRet;
}


VO_U32	voAudioPushParserSendBuffer(VO_HANDLE hHandle, const VO_SOURCE2_SAMPLE& buffer)
{
	VO_U32	 uRet = 0;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->SendBuffer(buffer);
	return uRet;
}


VO_U32 voAudioPushParserGetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32	 uRet = 0;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->GetParam(nParamID, pParam);
	return uRet;
}

VO_U32 voAudioPushParserSetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32	 uRet = 0;
	CBaseAudioPushParser * pSrc = (CBaseAudioPushParser *)hHandle;
	if (!pSrc)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	uRet = pSrc->SetParam(nParamID, pParam);
	return uRet;

}

#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _VONAMESPACE */

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
VO_S32 VO_API voGetPushAudioAPI(VO_SOURCE2_API* pHandle)
{
	pHandle->Init		= voAudioPushParserInit;
	pHandle->Uninit 	= voAudioPushParserUninit;	
	pHandle->Open		= voAudioPushParserOpen;
	pHandle->Close		= voAudioPushParserClose;
	pHandle->Start		= voAudioPushParserStart;
	pHandle->Pause		= voAudioPushParserPause;
	pHandle->Stop		= voAudioPushParserStop;
	pHandle->Seek		= voAudioPushParserSeek;
	pHandle->GetDuration = voAudioPushParserGetDuration;
	pHandle->GetSample	 = voAudioPushParserGetSample;
	pHandle->GetProgramCount = voAudioPushParserGetProgramCount;
	pHandle->GetProgramInfo	= voAudioPushParserGetProgramInfo;
	pHandle->GetCurTrackInfo = voAudioPushParserGetCurTrackInfo;
	pHandle->SelectProgram= voAudioPushParserSelectProgram;
	pHandle->SelectStream = voAudioPushParserSelectStream;
	pHandle->SelectTrack	= voAudioPushParserSelectTrack;
	pHandle->GetDRMInfo = voAudioPushParserGetDRMInfo;
	pHandle->SendBuffer = voAudioPushParserSendBuffer;
	pHandle->GetParam	= voAudioPushParserGetParam;
	pHandle->SetParam	= voAudioPushParserSetParam;

	return VO_RET_SOURCE2_OK;
}
#if defined __cplusplus
}
#endif