#include "CLiveSourceBase.h"
#include "cmnMemory.h"

CLiveSourceBase::CLiveSourceBase(VO_U16 type)
:m_nSrcType(type)
,m_hLiveSrc(NULL)
{
	cmnMemSet(0, &m_LiveSrcFunc, 0, sizeof(VO_LIVESRC_API));

	CLiveSourceBase::LoadLib(NULL);
}

CLiveSourceBase::~CLiveSourceBase(void)
{
}

VO_U32 CLiveSourceBase::Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{
	if(m_LiveSrcFunc.Open)
		return m_LiveSrcFunc.Open(&m_hLiveSrc, pUserData, fStatus, fSendData);

	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CLiveSourceBase::Close()
{
	if(m_LiveSrcFunc.Close && m_hLiveSrc)
	{
		VO_U32 rc	=  m_LiveSrcFunc.Close(m_hLiveSrc);
		m_hLiveSrc	= NULL;
		return rc;
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CLiveSourceBase::Scan(VO_LIVESRC_SCAN_PARAMEPTERS* pParam)
{
	if(m_LiveSrcFunc.Scan && m_hLiveSrc)
	{
		return m_LiveSrcFunc.Scan(m_hLiveSrc, pParam);
	}

	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CLiveSourceBase::Cancel(VO_U32 nID)
{
	if(m_LiveSrcFunc.Cancel && m_hLiveSrc)
	{
		return m_LiveSrcFunc.Cancel(m_hLiveSrc, nID);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CLiveSourceBase::Start()
{
	if(m_LiveSrcFunc.Start && m_hLiveSrc)
	{
		return m_LiveSrcFunc.Start(m_hLiveSrc);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CLiveSourceBase::Stop()
{
	if(m_LiveSrcFunc.Stop && m_hLiveSrc)
	{
		return m_LiveSrcFunc.Stop(m_hLiveSrc);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CLiveSourceBase::SetDataSource(VO_PTR pSource, VO_U32 nFlag)
{
	if(m_LiveSrcFunc.SetDataSource && m_hLiveSrc)
	{
		return m_LiveSrcFunc.SetDataSource(m_hLiveSrc, pSource, nFlag);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CLiveSourceBase::Seek(VO_U64 nTimeStamp)
{
	if(m_LiveSrcFunc.Seek && m_hLiveSrc)
	{
		return m_LiveSrcFunc.Seek(m_hLiveSrc, nTimeStamp);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CLiveSourceBase::GetChannel(VO_U32 * pCount, VO_LIVESRC_CHANNELINFO ** ppChannelInfo)
{
	if(m_LiveSrcFunc.GetChannel && m_hLiveSrc)
	{
		return m_LiveSrcFunc.GetChannel(m_hLiveSrc, pCount, ppChannelInfo);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CLiveSourceBase::SetChannel(VO_S32 nChannelID)
{
	if(m_LiveSrcFunc.SetChannel && m_hLiveSrc)
	{
		return m_LiveSrcFunc.SetChannel(m_hLiveSrc, nChannelID);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CLiveSourceBase::GetESG(VO_S32 nChannelID, VO_LIVESRC_ESG_INFO** ppESGInfo)
{
	if(m_LiveSrcFunc.GetESG && m_hLiveSrc)
	{
		return m_LiveSrcFunc.GetESG(m_hLiveSrc, nChannelID, ppESGInfo);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CLiveSourceBase::GetParam(VO_U32 nParamID, VO_PTR pValue)
{
	if(m_LiveSrcFunc.GetParam && m_hLiveSrc)
	{
		return m_LiveSrcFunc.GetParam(m_hLiveSrc, nParamID, pValue);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CLiveSourceBase::SetParam(VO_U32 nParamID, VO_PTR pValue)
{
	if(m_LiveSrcFunc.SetParam && m_hLiveSrc)
	{
		return m_LiveSrcFunc.SetParam(m_hLiveSrc, nParamID, pValue);
	}
	return VO_ERR_LIVESRC_NULLPOINTER;
}

VO_U32 CLiveSourceBase::LoadLib (VO_HANDLE hInst)
{
	// default api name
	vostrcpy(m_szAPIName, _T("voGetLiveSrcAPI"));

	// need add more...
	switch(m_nSrcType)
	{
	case VO_LIVESRC_FT_CMMB:
		{
#ifdef _WIN32
			vostrcpy(m_szDllFile, _T("voSrcCMMB.dll"));
#elif defined _LINUX
			vostrcpy(m_szDllFile, _T("libvoLiveSrcCMMB.so"));
#endif
			vostrcpy(m_szAPIName, _T("voGetLiveSrcAPI"));
		}
		break;
	case VO_LIVESRC_FT_ATSCMH:
		{
#ifdef _WIN32
			vostrcpy(m_szDllFile, _T("voSrcATSCMH.dll"));
#elif defined _LINUX
			vostrcpy(m_szDllFile, _T("libvoLiveSrcATSCMH.so"));
#endif
			vostrcpy(m_szAPIName, _T("voGetLiveSrcAPI"));
		}
		break;
	case VO_LIVESRC_FT_DVBH:
#ifdef _WIN32
			vostrcpy(m_szDllFile, _T("voSrcDVBH.dll"));
#elif defined _LINUX
			vostrcpy(m_szDllFile, _T("libvoLiveSrcDVBH.so"));
#endif
			vostrcpy(m_szAPIName, _T("voGetLiveSrcAPI"));
		break;
	case VO_LIVESRC_FT_HTTP_LS:
#ifdef _WIN32
		vostrcpy(m_szDllFile, _T("voLiveSrcHLS.dll"));
#elif defined _LINUX
		vostrcpy(m_szDllFile, _T("libvoLiveSrcHLS.so"));
#endif
		vostrcpy(m_szAPIName, _T("voGetLiveSrcAPI"));
		break;
	default:
		break;
	}



	VO_U32 rc =  CDllLoad::LoadLib(hInst);

	if(rc == 1)
	{
		VOGETLIVESRCAPI pAPI = (VOGETLIVESRCAPI) m_pAPIEntry;
		pAPI (&m_LiveSrcFunc);
	}

	return rc;
}
