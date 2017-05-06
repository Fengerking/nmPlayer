#include "voLog.h"
#include "voLiveSourcePlus.h"
#include "vompType.h"

#ifdef _ISS_SOURCE_
#include "CLiveSrcISSPlus.h"
#define SOURCE_CLASS CLiveSrcISSPlus
#endif

#define SOUORCE_OBJ ((SOURCE_CLASS*)hLiveSrc)

#define MODULEVOSRCOPEN(name) vosrcOpen##name
#define vosrcOpen(name) MODULEVOSRCOPEN(name)

#define MODULEVOSRCCLOSE(name) vosrcClose##name
#define vosrcClose(name) MODULEVOSRCCLOSE(name)

#define MODULEINIT(name) vosrcInit##name
#define vosrcInit(name) MODULEINIT(name)

#define MODULEUNINIT(name) vosrcUnInit##name
#define vosrcUnInit(name) MODULEUNINIT(name)

#define MODULEVOSRCGETPARAM(name) vosrcGetParam##name
#define vosrcGetParam(name) MODULEVOSRCGETPARAM(name)

#define MODULEVOSRCSETPARAM(name) vosrcSetParam##name
#define vosrcSetParam(name) MODULEVOSRCSETPARAM(name)

#define MODULEHeaderParser(name) vosrcHeaderParser##name
#define vosrcHeaderParser(name) MODULEHeaderParser(name)

#define MODULECHUNKPARSER(name) vosrcChunkParser##name
#define vosrcChunkParser(name) MODULECHUNKPARSER(name)

#define MODULEGETSTREAMCOUNTS(name) vosrcGetStreamCounts##name
#define vosrcGetStreamCounts(name) MODULEGETSTREAMCOUNTS(name)

#define MODULEGETSTREAMINFO(name) vosrcGetStreamInfo##name
#define vosrcGetStreamInfo(name) MODULEGETSTREAMINFO(name)

#define MODULEGETDRMINFO(name) vosrcGetDRMInfo##name
#define vosrcGetDRMInfo(name) MODULEGETDRMINFO(name)

#define MODULEGetPlayList(name) vosrcGetPlayList##name
#define vosrcGetPlayList(name) MODULEGetPlayList(name)


#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#else
	#if defined __cplusplus
		extern "C" {
	#endif
#endif

VO_U32 vosrcInit(MODULENAME) (VO_HANDLE * phLiveSrc, VO_LIVESRC_CALLBACK *pCallback)
{
	SOURCE_CLASS * pSrc = new SOURCE_CLASS();
	if (pSrc == NULL)
		return VO_ERR_LIVESRC_OUTOFMEMORY;

	pSrc->Init(pCallback);

	*phLiveSrc = (VO_HANDLE)pSrc;

	return VO_ERR_LIVESRC_OK;
}
VO_U32 vosrcUnInit(MODULENAME) (VO_HANDLE hLiveSrc)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;
	return pSrc->UnInit();
}

VO_U32 vosrcHeaderParser(MODULENAME) (VO_HANDLE hLiveSrc, VO_PBYTE pData, VO_U32 uSize)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;

	VO_U32 ret = pSrc->HeaderParser(pData, uSize);

	return ret;
}

VO_U32 vosrcChunkParser(MODULENAME) (VO_HANDLE hLiveSrc, VO_SOURCE_TRACKTYPE trackType,VO_PBYTE pData, VO_U32 uSize)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;

	VO_U32 ret = pSrc->ChunkParser(trackType, pData, uSize);
	return ret;
}
VO_U32 vosrcGetStreamCounts(MODULENAME) (VO_HANDLE hLiveSrc, VO_U32 *pStreamCounts)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;

	VO_U32 ret = pSrc->GetStreamCounts(pStreamCounts);
	return ret;
}
VO_U32 vosrcGetStreamInfo(MODULENAME) (VO_HANDLE hLiveSrc, VO_U32 nStreamID, VO_SOURCE2_STREAM_INFO **ppStreamInfo)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;

	VO_U32 ret = pSrc->GetStreamInfo(nStreamID, ppStreamInfo);
	return ret;
}
VO_U32 vosrcGetDRMInfo(MODULENAME) (VO_HANDLE hLiveSrc, VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;

	VO_U32 ret = pSrc->GetDRMInfo(ppDRMInfo);
	return ret;
}

VO_U32 vosrcGetParam(MODULENAME) (VO_HANDLE hLiveSrc, VO_U32 nParamID, VO_PTR pValue)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;

	return pSrc->GetParam(nParamID, pValue);
}

VO_U32 vosrcSetParam(MODULENAME) (VO_HANDLE hLiveSrc, VO_U32 nParamID, VO_PTR pValue)
{

	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;

	return pSrc->SetParam(nParamID, pValue);
}

VO_U32 vosrcGetPlayList(MODULENAME)  (VO_HANDLE hLiveSrc, FragmentItem **ppPlaylist)
{
	if (hLiveSrc == NULL)
		return VO_ERR_LIVESRC_NULLPOINTER;

	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hLiveSrc;

	return pSrc->GetPlayList(ppPlaylist);
}
#ifdef _ISS_SOURCE_
VO_S32 VO_API voGetISSLiveSrcPlusAPI(VO_LIVESRC_PLUS_API* pHandle)   
#else
VO_S32 VO_API voGetLiveSrcPlusAPI(VO_LIVESRC_PLUS_API* pHandle)   
#endif
{
	pHandle->Init			= vosrcInit(MODULENAME);
	pHandle->Uninit		= vosrcUnInit(MODULENAME);
	pHandle->GetParam	= vosrcGetParam(MODULENAME) ;
	pHandle->SetParam	= vosrcSetParam(MODULENAME) ;
	pHandle->ChunkParser = vosrcChunkParser(MODULENAME) ;
	pHandle->HeaderParser= vosrcHeaderParser(MODULENAME) ;
	pHandle->GetDRMInfo = vosrcGetDRMInfo(MODULENAME) ;
	pHandle->GetStreamCounts = vosrcGetStreamCounts(MODULENAME) ;
	pHandle->GetStreamInfo = vosrcGetStreamInfo(MODULENAME) ;
	pHandle->GetPlayList = vosrcGetPlayList(MODULENAME) ;
	return VO_ERR_LIVESRC_OK;
} 

#ifdef _VONAMESPACE
#else
#if defined __cplusplus
  }
#endif
#endif
