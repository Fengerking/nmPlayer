
#include "vo_smil.h"

#include "vo_ads_utility.h"
#include "CDllLoad.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif



struct VO_SMIL_INFO 
{
	CDllLoad loader;

	VO_HANDLE h_smil;
	VO_SMIL_API api;

	VO_ADSMANAGER_PLAYBACKINFOEX * ptr_info;
};

typedef VO_S32 ( VO_API * pvoGetSMILAPI)(VO_SMIL_API* pHandle);

//2.1 Download smil
//2.2 Send smil to smil parser and get the parsed result
//2.3 Re org the parsed result and get nessccary info

VO_VOID store_smil_info( VO_SMIL_INFO * ptr_info , VO_CBS_SMIL_VIDEO * ptr_smil_video );
VO_VOID convert_smilvideo2periodinfo( VO_ADSMANAGER_PLAYBACKPERIODEX * ptr_periodinfo , VO_CBS_SMIL_VIDEO * ptr_smil_video );

VO_U32 vo_smil_open( VO_PTR * ptr_h , VO_TCHAR * ptr_workingpath )
{
	VO_SMIL_INFO * ptr_info = new VO_SMIL_INFO;

	ptr_info->h_smil = 0;
	memset( &(ptr_info->api) , 0 , sizeof(VO_SMIL_API) );
	ptr_info->ptr_info = 0;

	vostrcpy( ptr_info->loader.m_szDllFile , _T("voSMILParser") );
	vostrcpy( ptr_info->loader.m_szAPIName , _T("voGetSMILAPI") );

	ptr_info->loader.SetWorkPath( ptr_workingpath );

	if(ptr_info->loader.LoadLib(NULL) == 0)
		return VO_RET_SOURCE2_FAIL;

	pvoGetSMILAPI getapi = (pvoGetSMILAPI)ptr_info->loader.m_pAPIEntry;

	if( !getapi )
	{
		return VO_RET_SOURCE2_FAIL;
	}

	getapi( &(ptr_info->api) );

	VO_U32 ret = 0;

	ret = ptr_info->api.Init( &(ptr_info->h_smil) , INIT_CBS_FLAG , ptr_workingpath );

	*ptr_h = ptr_info;

	return ret;
}

VO_U32 vo_smil_close( VO_PTR h )
{
	VO_SMIL_INFO * ptr_info = ( VO_SMIL_INFO * )h;

	if( !ptr_info )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	if( ptr_info->api.Init && ptr_info->h_smil )
		ptr_info->api.Uninit( ptr_info->h_smil );

	if( ptr_info && ptr_info->ptr_info && ptr_info->ptr_info->pPeriods )
		delete []ptr_info->ptr_info->pPeriods;

	if( ptr_info && ptr_info->ptr_info )
		delete ptr_info->ptr_info;

	delete ptr_info;

	return VO_RET_SOURCE2_OK;
}

VO_U32 vo_smil_parse_smilinfo( VO_PTR h , VO_CHAR * ptr_smil , VO_U32 size , VO_ADSMANAGER_PLAYBACKINFOEX ** pptr_info )
{
	VO_SMIL_INFO * ptr_info = ( VO_SMIL_INFO * )h;

	if( !ptr_info || !( ptr_info->api.Init ) )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_CBS_SMIL_VIDEO * ptr_smil_video = 0;
	VO_U32 ret = ptr_info->api.Parse( ptr_info->h_smil , (VO_PBYTE)ptr_smil , size , (VO_PTR*)&ptr_smil_video );

	if( ret != VO_RET_SOURCE2_OK )
		return ret;

	store_smil_info( ptr_info , ptr_smil_video );

	*pptr_info = ptr_info->ptr_info;

	return ret;
}

VO_VOID store_smil_info( VO_SMIL_INFO * ptr_info , VO_CBS_SMIL_VIDEO * ptr_smil_video )
{
	VO_U32 count = 0;

	VO_CBS_SMIL_VIDEO * ptr_pos = ptr_smil_video;

	while( ptr_pos )
	{
		count++;
		ptr_pos = ptr_pos->pNext;
	}

	ptr_info->ptr_info = new VO_ADSMANAGER_PLAYBACKINFOEX;

	ptr_info->ptr_info->nCounts = count;
	ptr_info->ptr_info->pPeriods = new VO_ADSMANAGER_PLAYBACKPERIODEX[ count ];

	ptr_pos = ptr_smil_video;
	count = 0;

	while( ptr_pos )
	{
		convert_smilvideo2periodinfo( &(ptr_info->ptr_info->pPeriods[count]) , ptr_pos );
		count++;
		ptr_pos = ptr_pos->pNext;
	}
}

VO_VOID convert_smilvideo2periodinfo( VO_ADSMANAGER_PLAYBACKPERIODEX * ptr_periodinfo , VO_CBS_SMIL_VIDEO * ptr_smil_video )
{
	ptr_periodinfo->ullStartTime = ptr_smil_video->clipBegin;
	ptr_periodinfo->ullEndTime = ptr_smil_video->clipEnd;
	strcpy( ptr_periodinfo->strPeriodURL , ptr_smil_video->pSrc );
	strcpy( ptr_periodinfo->strCaptionURL , ptr_smil_video->pClosedCaptionURL );
	strcpy( ptr_periodinfo->strGUID , ptr_smil_video->pGuid );
	ptr_periodinfo->isLive = ptr_smil_video->pIsLive;
	strcpy( ptr_periodinfo->strPeriodTitle , ptr_smil_video->pTitle );
	strcpy( ptr_periodinfo->strContentID , ptr_smil_video->pContantDAta );
	ptr_periodinfo->isEpisode = ptr_smil_video->bEpisodeFlag;
}


#ifdef _VONAMESPACE
}
#endif
