#include "AdaptiveStreamingDRM.h"
#include "voStreamingDRM.h"
#include "voDRM2.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CAdaptiveStreamingDRM::CAdaptiveStreamingDRM(VO_SOURCEDRM_CALLBACK2* pDrmCB, VO_SOURCE2_IO_API* pIO, VO_PTR pReserved)
: m_pDRMCB(pDrmCB)
, m_pIO(pIO)
, m_eDRMSource(VO_DRM2SRC_CHUNK)
{
}

CAdaptiveStreamingDRM::~CAdaptiveStreamingDRM(void)
{
}

VO_U32 CAdaptiveStreamingDRM::PreprocessURL(const VO_CHAR* urlSrc, VO_CHAR* urlDes, VO_PTR pReserved)
{
	VO_DRM2_CONVERT_URL s_Convert_URL = {0};
	s_Convert_URL.urlSrc	= urlSrc;
	s_Convert_URL.urlDes	= urlDes;
	s_Convert_URL.pReserved	= pReserved;

	return m_pDRMCB ? m_pDRMCB->fCallback(m_pDRMCB->pUserData, VO_DRM_FLAG_CONVERTURL, &s_Convert_URL, m_eDRMSource) : VO_ERR_DRM2_NO_DRM_API;
}

VO_U32 CAdaptiveStreamingDRM::Info(VO_CHAR* szManifestURL, VO_BYTE* pManifest, VO_U32 uSizeManifest, VO_PTR pReserved)
{
	VO_DRM2_INFO_ADAPTIVESTREAMING s_infoDrm;
	memset( &s_infoDrm, 0, sizeof(VO_DRM2_INFO_ADAPTIVESTREAMING) );

	if (m_pDRMCB)
		strcpy(s_infoDrm.szDRMTYPE, m_pDRMCB->szDRMTYPE);
	s_infoDrm.pURL			= szManifestURL;
	s_infoDrm.pManifestData	= pManifest;
	s_infoDrm.uSizeManifest	= uSizeManifest;
	s_infoDrm.pAPI_IO		= m_pIO;
	s_infoDrm.pReserved		= pReserved;

	return m_pDRMCB ? m_pDRMCB->fCallback(m_pDRMCB->pUserData, VO_DRM_FLAG_DRMINFO, &s_infoDrm, m_eDRMSource) : VO_ERR_DRM2_NO_DRM_API;
}

VO_U32 CAdaptiveStreamingDRM::DataBegin(VO_U32 uIdentifer, VO_PTR pInfo)
{
	VO_DRM2_DATA s_dataDrm;
	memset( &s_dataDrm, 0, sizeof(VO_DRM2_DATA) );

	s_dataDrm.sDataInfo.nDataType	= VO_DRM2DATATYPE_CHUNK_BEGIN;
	s_dataDrm.sDataInfo.pInfo		= pInfo;
	s_dataDrm.nReserved[2]			= uIdentifer;

	return m_pDRMCB ? m_pDRMCB->fCallback(m_pDRMCB->pUserData, VO_DRM_FLAG_DRMDATA, &s_dataDrm, m_eDRMSource) : VO_ERR_DRM2_NO_DRM_API;
}

VO_U32 CAdaptiveStreamingDRM::DataProcess_Chunk(VO_U32 uIdentifer, VO_U64 nOffset, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData, VO_U32 *pnDesSize, VO_BOOL bChunkEnd, VO_PTR pAdditionalInfo)
{
	VO_DRM2_DATA s_dataDrm;
	memset( &s_dataDrm, 0, sizeof(VO_DRM2_DATA) );

	s_dataDrm.sDataInfo.nDataType	= VO_DRM2DATATYPE_CHUNK_PROCESSING;
	s_dataDrm.sDataInfo.pInfo		= pAdditionalInfo;
	s_dataDrm.pData					= pSrcData;
	s_dataDrm.nSize					= nSrcSize;
	s_dataDrm.ppDstData				= ppDesData;
	s_dataDrm.pnDstSize				= pnDesSize;
	if (bChunkEnd)
		s_dataDrm.nReserved[0]		= 1;
	s_dataDrm.nReserved[1]			= (VO_U32)&nOffset;
	s_dataDrm.nReserved[2]			= uIdentifer;

	return m_pDRMCB ? m_pDRMCB->fCallback(m_pDRMCB->pUserData, VO_DRM_FLAG_DRMDATA, &s_dataDrm, m_eDRMSource) : VO_ERR_DRM2_NO_DRM_API;
}

VO_U32 CAdaptiveStreamingDRM::DataEnd(VO_U32 uIdentifer, VO_PTR pInfo)
{
	VO_DRM2_DATA s_dataDrm;
	memset( &s_dataDrm, 0, sizeof(VO_DRM2_DATA) );

	s_dataDrm.sDataInfo.nDataType	= VO_DRM2DATATYPE_CHUNK_END;
	s_dataDrm.sDataInfo.pInfo		= pInfo;
	s_dataDrm.nReserved[2]			= uIdentifer;

	return m_pDRMCB ? m_pDRMCB->fCallback(m_pDRMCB->pUserData, VO_DRM_FLAG_DRMDATA, &s_dataDrm, m_eDRMSource) : VO_ERR_DRM2_NO_DRM_API;
}

VO_U32 CAdaptiveStreamingDRM::Info_FR(VO_PTR pInfo, VO_DRM2_INFO_TYPE eInfoType, VO_PTR pReserved)
{
	VO_DRM2_INFO s_infoDrm;
	memset( &s_infoDrm, 0, sizeof(VO_DRM2_INFO) );

	s_infoDrm.nType			= VO_DRM2TYPE_UNKNOWN;
	s_infoDrm.pDrmInfo		= pInfo;
	s_infoDrm.nReserved[0]	= eInfoType;

	VO_DRM_CALLBACK_FLAG eflag = VO_DRM_FLAG_FLUSH;
	if (VO_DRM2_INFO_PROCECTION == eInfoType)
		eflag = VO_DRM_FLAG_DRM_PROCECTION;
	else
		eflag = VO_DRM_FLAG_DRM_TRACK_INFO;

	return m_pDRMCB ? m_pDRMCB->fCallback(m_pDRMCB->pUserData, eflag, &s_infoDrm, m_eDRMSource) : VO_ERR_DRM2_NO_DRM_API;
}

VO_U32 CAdaptiveStreamingDRM::DataProcess_FR(VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData, VO_U32 *pnDesSize, VO_DRM2_DATATYPE eDataType, VO_DRM2_DATAINFO_TYPE eInfoType, VO_PTR pAdditionalInfo)
{
	VO_DRM2_DATA s_dataDrm;
	memset( &s_dataDrm, 0, sizeof(VO_DRM2_DATA) );

	s_dataDrm.sDataInfo.nDataType	= eDataType;
	s_dataDrm.sDataInfo.pInfo		= pAdditionalInfo;
	s_dataDrm.sDataInfo.nReserved[0]= eInfoType;
	s_dataDrm.pData					= pSrcData;
	s_dataDrm.nSize					= nSrcSize;
	s_dataDrm.ppDstData				= ppDesData;
	s_dataDrm.pnDstSize				= pnDesSize;

	return m_pDRMCB ? m_pDRMCB->fCallback(m_pDRMCB->pUserData, VO_DRM_FLAG_DRMDATA, &s_dataDrm, m_eDRMSource) : VO_ERR_DRM2_NO_DRM_API;
}

VO_U32 CAdaptiveStreamingDRM::PreprocessPlaylist(VO_BYTE* pPlaylist, VO_U32 uSizeBuffer, VO_U32* puSizePlaylist, VO_PTR pReserved)
{
	if (NULL == m_pDRMCB)
		return VO_ERR_DRM2_NO_DRM_API;

	VO_DRM2_CONVERT_PLAYLIST s_ConvertPlaylist;
	memset( &s_ConvertPlaylist, 0, sizeof(VO_DRM2_CONVERT_PLAYLIST) );

	s_ConvertPlaylist.pPlaylist			= pPlaylist;
	s_ConvertPlaylist.uSizeBuffer		= uSizeBuffer;
	s_ConvertPlaylist.puSizePlaylist	= puSizePlaylist;
	s_ConvertPlaylist.pReserved			= pReserved;

	VO_U32 uRet = m_pDRMCB->fCallback(m_pDRMCB->pUserData, VO_DRM_FLAG_PLAYLIST, &s_ConvertPlaylist, m_eDRMSource);
	switch (uRet)
	{
	case VO_ERR_OUTPUT_BUFFER_SMALL:
		{
			return VO_RET_SOURCE2_OUTPUTDATASMALL;
		}
		break;

	case VO_ERR_NOT_IMPLEMENT:
		{
			return VO_RET_SOURCE2_NOIMPLEMENT;
		}
		break;

	default:
		return VO_RET_SOURCE2_FAIL;
	}
}

VO_U32 CAdaptiveStreamingDRM::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	if (NULL == pParam)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	switch (uID)
	{
	case VO_PID_AS_DRM2_NEW_API:
		{
			return m_pDRMCB ? m_pDRMCB->fCallback(m_pDRMCB->pUserData, VO_DRM_FLAG_NEWAPI, pParam, m_eDRMSource) : VO_ERR_DRM2_NO_DRM_API;
		}
		break;

	default:
		return VO_RET_SOURCE2_NOIMPLEMENT;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 CAdaptiveStreamingDRM::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	if (NULL == pParam)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	switch (uID)
	{
	case VO_PID_AS_DRM2_STREAMING_TYPE:
		{
			switch (*(VO_ADAPTIVESTREAMPARSER_STREAMTYPE*)pParam)
			{
			case VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS:
				{
					m_eDRMSource = VO_DRM2SRC_CHUNK_HLS;
				}
				break;

			case VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS:
				{
					m_eDRMSource = VO_DRM2SRC_CHUNK_SSTR;
				}
				break;

			case VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH:
				{
					m_eDRMSource = VO_DRM2SRC_CHUNK_DASH;
				}
				break;

			default:
				m_eDRMSource = VO_DRM2SRC_CHUNK;
			}
		}
		break;

	default:
		return VO_RET_SOURCE2_NOIMPLEMENT;
	}

	return VO_RET_SOURCE2_OK;
}