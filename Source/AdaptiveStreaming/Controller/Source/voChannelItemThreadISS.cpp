#include "voChannelItemThreadISS.h"
#include "voOSFunc.h"
#include "CSourceIOUtility.h"
#include "voLog.h"
#include "voAdaptiveStreamingClassFactory.h"
#include "voSmartPointor.hpp"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voChannelItemThreadISS"
#endif

voChannelItemThreadISS::voChannelItemThreadISS(void)
:pTrackInfo(NULL)
{
}

voChannelItemThreadISS::~voChannelItemThreadISS(void)
{
	if(pTrackInfo)
	{
		ReleaseTrackInfoOP_T(pTrackInfo);
		pTrackInfo = NULL;
	}
}
VO_U32 voChannelItemThreadISS::GetHeadData(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE Type)
{
	_TRACK_INFO *pTmpTrackInfo = NULL;

	VO_U32 nRet = VO_RET_SOURCE2_OK;

	nRet |= m_ptr_info->pInfo->GetAdaptiveStreamingParserPtr()->GetCurTrackInfo(m_trackType, &pTmpTrackInfo);
	CopyTrackInfoOP_T(pTmpTrackInfo,&pTrackInfo);
	if( !pTrackInfo )
	{
		VOLOGW("Get CurTrackInfo error.");
		return nRet;
	}
#if defined VOLOGR
	char c[1024];
	memset(c, 0, 1024);
	for(int i = 0; i< pTrackInfo->uHeadSize ;i++)
	{
		sprintf(&c[i*2], "%2.2x",pTrackInfo->pHeadData[i]);
	}
	VOLOGR("HeadData: %s",c);
#endif
	
	if( nRet != VO_RET_SOURCE2_OK || !pTrackInfo )
	{
		VOLOGE( "Get Current Track Info Error.%x", nRet);
		return VO_RET_SOURCE2_FAIL;
	}
	nRet = m_pFileParser->CreateTrackInfo2( m_trackType, pTrackInfo);
	if(nRet != VO_RET_SOURCE2_OK)
	{
		VOLOGE("CreateTrackInfo2 error,nRet is 0x%08x",nRet);
		return nRet;
	}
	m_pFileParser->SetDrmHeadData(m_trackType, VO_FALSE);
	VO_StreamingDRM_API *pDRM = m_ptr_info->pInfo->GetDRMPtr( m_ptr_info->type );

	if( pDRM && pDRM->hHandle )
	{
		VO_U32 nDestSize = 0;
		nRet = pDRM->DataProcess_FR( pDRM->hHandle, NULL, pTrackInfo->uHeadSize, NULL, &nDestSize, VO_DRM2DATATYPE_SEQUENCEHEADER,VO_DRM2_DATAINFO_UNKNOWN,NULL );
		if(nRet == VO_ERR_DRM2_OUTPUT_BUF_SMALL)
		{
			VO_PBYTE pNewHeadData =  new VO_BYTE[nDestSize];
			VO_PBYTE pNewOutHeadData = NULL;
			memset(pNewHeadData,0,nDestSize);
			if(pTrackInfo->pHeadData)
			{
				memcpy(pNewHeadData,pTrackInfo->pHeadData,pTrackInfo->uHeadSize);
				delete []pTrackInfo->pHeadData;
				pTrackInfo->pHeadData = NULL;
				nRet = pDRM->DataProcess_FR( pDRM->hHandle , pNewHeadData,pTrackInfo->uHeadSize,  &pNewOutHeadData, &nDestSize, VO_DRM2DATATYPE_SEQUENCEHEADER,VO_DRM2_DATAINFO_UNKNOWN,NULL );
				if(nRet == VO_RET_SOURCE2_OK)
				{
					pTrackInfo->uHeadSize = nDestSize;
					pTrackInfo->pHeadData = pNewOutHeadData;
					m_pFileParser->SetDrmHeadData(m_trackType, VO_TRUE);
					
				}
			}
		}
	}
	m_pFileParser->StoreTrackInfo(m_trackType,pTrackInfo);

	return nRet;
}