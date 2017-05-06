	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseLiveSource.cpp

	Contains:	CBaseLiveSource class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "CBaseLiveSource.h"
#include "cmnMemory.h"

#define LOG_TAG "CBaseLiveSource"
#include "voLog.h"

CBaseLiveSource::CBaseLiveSource(VO_U16 type)
:m_nSrcType(type)
,m_hLiveSrc(NULL)
{
	cmnMemSet(0, &m_LiveSrcFunc, 0, sizeof(VO_LIVESRC_API));
}

CBaseLiveSource::~CBaseLiveSource(void)
{
	Close();
}

VO_U32 CBaseLiveSource::Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{	
    CBaseLiveSource::LoadLib(NULL);
	if(m_LiveSrcFunc.Open)
		return m_LiveSrcFunc.Open(&m_hLiveSrc, pUserData, fStatus, fSendData);

	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::Close()
{
	if(m_LiveSrcFunc.Close && m_hLiveSrc)
	{
		VO_U32 rc	=  m_LiveSrcFunc.Close(m_hLiveSrc);
		m_hLiveSrc	= NULL;
		return rc;
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::Scan(VO_LIVESRC_SCAN_PARAMEPTERS* pParam)
{
	if(m_LiveSrcFunc.Scan && m_hLiveSrc)
	{
		return m_LiveSrcFunc.Scan(m_hLiveSrc, pParam);
	}

	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::Cancel(VO_U32 nID)
{
	if(m_LiveSrcFunc.Cancel && m_hLiveSrc)
	{
		return m_LiveSrcFunc.Cancel(m_hLiveSrc, nID);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::Start()
{
	if(m_LiveSrcFunc.Start && m_hLiveSrc)
	{
		return m_LiveSrcFunc.Start(m_hLiveSrc);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::Pause()
{
	if(m_LiveSrcFunc.Start && m_hLiveSrc)
	{
		return m_LiveSrcFunc.Pause(m_hLiveSrc);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::Stop()
{
	if(m_LiveSrcFunc.Stop && m_hLiveSrc)
	{
		return m_LiveSrcFunc.Stop(m_hLiveSrc);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::SendBuffer(VOMP_BUFFERTYPE * pBuffer) {
	if(m_LiveSrcFunc.SendBuffer && m_hLiveSrc)
	{
		return m_LiveSrcFunc.SendBuffer(m_hLiveSrc, pBuffer);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::SetDataSource(VO_PTR pSource, VO_U32 nFlag)
{
	if(m_LiveSrcFunc.SetDataSource && m_hLiveSrc)
	{
		return m_LiveSrcFunc.SetDataSource(m_hLiveSrc, pSource, nFlag);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::Seek(VO_U64 nTimeStamp)
{
	if(m_LiveSrcFunc.Seek && m_hLiveSrc)
	{
		return m_LiveSrcFunc.Seek(m_hLiveSrc, nTimeStamp);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::GetChannel(VO_U32 * pCount, VO_LIVESRC_CHANNELINFO ** ppChannelInfo)
{
	if(m_LiveSrcFunc.GetChannel && m_hLiveSrc)
	{
		return m_LiveSrcFunc.GetChannel(m_hLiveSrc, pCount, ppChannelInfo);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::SetChannel(VO_S32 nChannelID)
{
	if(m_LiveSrcFunc.SetChannel && m_hLiveSrc)
	{
		return m_LiveSrcFunc.SetChannel(m_hLiveSrc, nChannelID);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::GetESG(VO_S32 nChannelID, VO_LIVESRC_ESG_INFO** ppESGInfo)
{
	if(m_LiveSrcFunc.GetESG && m_hLiveSrc)
	{
		return m_LiveSrcFunc.GetESG(m_hLiveSrc, nChannelID, ppESGInfo);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::GetParam(VO_U32 nParamID, VO_PTR pValue)
{
	if(m_LiveSrcFunc.GetParam && m_hLiveSrc)
	{
		return m_LiveSrcFunc.GetParam(m_hLiveSrc, nParamID, pValue);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CBaseLiveSource::SetParam(VO_U32 nParamID, VO_PTR pValue)
{
    VO_U32 ret = VO_ERR_LIVESRC_NULLPOINTER;

	if(m_LiveSrcFunc.SetParam && m_hLiveSrc)
	{
		return m_LiveSrcFunc.SetParam(m_hLiveSrc, nParamID, pValue);
	}
	return ret;
}

VO_U32 CBaseLiveSource::LoadLib (VO_HANDLE hInst)
{	
	// need add more...
	switch(m_nSrcType)
	{
	case VO_LIVESRC_FT_CMMB:
		{
#ifdef _WIN32
			vostrcpy(m_szDllFile, _T("voLiveSrcCMMB.dll"));
#elif defined _LINUX
			vostrcpy(m_szDllFile, _T("voLiveSrcCMMB.so"));
#elif defined _MAC_OS
			vostrcpy(m_szDllFile, _T("voLiveSrcCMMB.dylib"));			
#elif defined _IOS
#ifdef _IOS_LIVE_STREAMING
#ifdef _IOS_USE_LIVE_SRC_MODULE
			return voGetLiveSrcAPI(&m_LiveSrcFunc);
#else
			return -1;
#endif //_IOS_USE_LIVE_SRC_MODULE
#else
			return -1;
#endif // _IOS_LIVE_STREAMING
#endif
			vostrcpy(m_szAPIName, _T("voGetLiveSrcAPI"));
		}
		break;
	case VO_LIVESRC_FT_ATSCMH:
		{
#ifdef _WIN32
			vostrcpy(m_szDllFile, _T("voLiveSrcATSCMH.dll"));
#elif defined _LINUX
			vostrcpy(m_szDllFile, _T("voLiveSrcATSCMH.so"));
#elif defined _MAC_OS
			vostrcpy(m_szDllFile, _T("voLiveSrcATSCMH.dylib"));			
#elif defined _IOS
#ifdef _IOS_LIVE_STREAMING
#ifdef _IOS_USE_LIVE_SRC_MODULE
			return voGetLiveSrcAPI(&m_LiveSrcFunc);
#else
			return -1;
#endif //_IOS_USE_LIVE_SRC_MODULE
#else
			return -1;
#endif // _IOS_LIVE_STREAMING
#endif
			vostrcpy(m_szAPIName, _T("voGetLiveSrcAPI"));
		}
		break;
	case VO_LIVESRC_FT_DVBH:
		{
#ifdef _WIN32
			vostrcpy(m_szDllFile, _T("voLiveSrcDVBH.dll"));
#elif defined _LINUX
			vostrcpy(m_szDllFile, _T("voLiveSrcDVBH.so"));
#elif defined _MAC_OS
			vostrcpy(m_szDllFile, _T("voLiveSrcDVBH.dylib"));			
#elif defined _IOS
#ifdef _IOS_LIVE_STREAMING
#ifdef _IOS_USE_LIVE_SRC_MODULE
			return voGetLiveSrcAPI(&m_LiveSrcFunc);
#else
			return -1;
#endif //_IOS_USE_LIVE_SRC_MODULE
#else
			return -1;
#endif // _IOS_LIVE_STREAMING
#endif
			vostrcpy(m_szAPIName, _T("voGetLiveSrcAPI"));
		}
		break;

	case VO_LIVESRC_FT_HTTP_LS:
		{
#ifdef _WIN32
			vostrcpy(m_szDllFile, _T("voLiveSrcHLS.dll"));
#elif defined _LINUX
			vostrcpy(m_szDllFile, _T("voLiveSrcHLS.so"));
#elif defined _MAC_OS
			vostrcpy(m_szDllFile, _T("voLiveSrcHLS.dylib"));			
#elif defined _IOS
#ifdef _IOS_LIVE_STREAMING
#ifdef _IOS_USE_LIVE_SRC_MODULE
			return voGetLiveSrcAPI(&m_LiveSrcFunc);
#else
			return -1;
#endif //_IOS_USE_LIVE_SRC_MODULE
#else
			return -1;
#endif // _IOS_LIVE_STREAMING
#endif
			vostrcpy(m_szAPIName, _T("voGetHLSLiveSrcAPI"));
		}
		break;

	case VO_LIVESRC_FT_IIS_SS:
		{
#ifdef _WIN32
		vostrcpy(m_szDllFile, _T("voLiveSrcISS.dll"));
#elif defined _LINUX
		vostrcpy(m_szDllFile, _T("voLiveSrcISS.so"));
#elif defined _MAC_OS
		vostrcpy(m_szDllFile, _T("voLiveSrcISS.dylib"));			
#elif defined _IOS
#ifdef _IOS_LIVE_STREAMING
#ifdef _IOS_USE_LIVE_SRC_MODULE
			return voGetLiveSrcAPI(&m_LiveSrcFunc);
#else
			return -1;
#endif //_IOS_USE_LIVE_SRC_MODULE
#else
			return -1;
#endif // _IOS_LIVE_STREAMING
#endif
			vostrcpy(m_szAPIName, _T("voGetISSLiveSrcAPI"));
		}
		break;

case VO_LIVESRC_FT_DASH:
		{
#ifdef _WIN32
			vostrcpy(m_szDllFile, _T("voLiveSrcDASH.dll"));
#elif defined _LINUX
			vostrcpy(m_szDllFile, _T("voLiveSrcDASH.so"));
#elif defined _MAC_OS
			vostrcpy(m_szDllFile, _T("voLiveSrcDASH.dylib"));			
#elif defined _IOS
#ifdef _IOS_LIVE_STREAMING
#ifdef _IOS_USE_LIVE_SRC_MODULE
			return voGetLiveSrcAPI(&m_LiveSrcFunc);
#else
			return -1;
#endif //_IOS_USE_LIVE_SRC_MODULE
#else
			return -1;
#endif // _IOS_LIVE_STREAMING
#endif
			vostrcpy(m_szAPIName, _T("voGetDASHLiveSrcAPI"));
		}
		break;

	default:
		break;
	}
    
	VO_U32 rc = CDllLoad::LoadLib(hInst);
    
	if (m_pAPIEntry == NULL)
		return -1;

	VOGETLIVESRCAPI pAPI = (VOGETLIVESRCAPI) m_pAPIEntry;
	pAPI (&m_LiveSrcFunc);

	return rc;
}
