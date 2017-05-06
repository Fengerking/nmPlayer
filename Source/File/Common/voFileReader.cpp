
#ifdef _PUSHPLAYPARSER


    #define LOG_TAG "voFileReader"
    #include "voLog.h"
    #include "vo_file_parser.h"

#if !defined(__arm) && !defined(_MAC_OS) && !defined(_IOS)
    VO_HANDLE g_hInst = VO_NULL;
#endif

    #if defined __cplusplus
    extern "C" {
    #endif

    #ifdef _WIN32
    #include <Windows.h>

    #ifndef _LIB
    BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
    {   
	    g_hInst = hModule;
	    return TRUE;
    }
    #endif

    #endif	//_WIN32

    VO_U32 VO_API voFROpen(VO_PTR* ppHandle, VO_SOURCE_OPENPARAM* pParam)
    {
	    vo_file_parser * ptr_obj = new vo_file_parser();
        
        if( !ptr_obj )
            return VO_ERR_OUTOF_MEMORY;

        *ppHandle = ptr_obj;

        return ptr_obj->open( pParam );
    }

    VO_U32 VO_API voFRClose(VO_PTR pHandle)
    {
        if(!pHandle)
            return VO_ERR_INVALID_ARG;

        vo_file_parser * ptr_obj = (vo_file_parser *)pHandle;

        return ptr_obj->close();
    }

    VO_U32 VO_API voFRGetFileInfo(VO_PTR pHandle, VO_SOURCE_INFO* pSourceInfo)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

        vo_file_parser * ptr_obj = (vo_file_parser *)pHandle;

        return ptr_obj->get_fileinfo( pSourceInfo );
    }

    VO_U32 VO_API voFRGetTrackInfo(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_TRACKINFO* pTrackInfo)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

        vo_file_parser * ptr_obj = (vo_file_parser *)pHandle;

        return ptr_obj->get_trackinfo( nTrack , pTrackInfo );
    }

    VO_U32 VO_API voFRGetSample(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_SAMPLE* pSample)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

        vo_file_parser * ptr_obj = (vo_file_parser *)pHandle;

        return (ptr_obj->*(ptr_obj->get_sample))( nTrack , pSample );
    }

    VO_U32 VO_API voFRSetPos(VO_PTR pHandle, VO_U32 nTrack, VO_S64* pPos)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

        vo_file_parser * ptr_obj = (vo_file_parser *)pHandle;

        return (ptr_obj->*(ptr_obj->set_pos))( nTrack , pPos );
    }

    VO_U32 VO_API voFRSetFileParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

        vo_file_parser * ptr_obj = (vo_file_parser *)pHandle;

        return ptr_obj->set_fileparam( uID , pParam );
    }

    VO_U32 VO_API voFRGetFileParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    vo_file_parser * ptr_obj = (vo_file_parser *)pHandle;

	    return ptr_obj->get_fileparam( uID , pParam );
    }

    VO_U32 VO_API voFRSetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

        vo_file_parser * ptr_obj = (vo_file_parser *)pHandle;

        return ptr_obj->set_trackparam( nTrack , uID , pParam );
    }

    VO_U32 VO_API voFRGetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

        vo_file_parser * ptr_obj = (vo_file_parser *)pHandle;

        return ptr_obj->get_trackparam( nTrack , uID , pParam );
    }

    #ifdef _AVI_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetAVIReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//_AVI_READER
    #ifdef _ASF_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetASFReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//_ASF_READER
    #ifdef _MP4_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetMP4ReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//_MP4_READER
    #ifdef _AUDIO_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetAudioReadAPI(VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag)
    #endif	//_AUDIO_READER
    #ifdef _RM_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetRealReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//_RM_READER
    #ifdef FLV_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetFLVReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//FLV_READER
    #ifdef _OGG_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetOGGReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif  //_OGG_READER
    #ifdef _MKV_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetMKVReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif  //_MKV_READER
    #ifdef _MPEG_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetMPGReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//_MPEG_READER
    #ifdef MTV_TS
    VO_EXPORT_FUNC VO_S32 VO_API voGetMTVReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//MTV_TS    
	//#ifdef _RAW_DATA_PARSER
 //   VO_EXPORT_FUNC VO_S32 VO_API voGetRawDataParserAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
	//#endif	//_RAW_DATA_PARSER
    {
	    pReadHandle->Open = voFROpen;
	    pReadHandle->Close = voFRClose;
	    pReadHandle->GetSourceInfo = voFRGetFileInfo;
	    pReadHandle->GetTrackInfo = voFRGetTrackInfo;
	    pReadHandle->GetSample = voFRGetSample;
	    pReadHandle->SetPos = voFRSetPos;
	    pReadHandle->SetSourceParam = voFRSetFileParam;
	    pReadHandle->GetSourceParam = voFRGetFileParam;
	    pReadHandle->SetTrackParam = voFRSetTrackParam;
	    pReadHandle->GetTrackParam = voFRGetTrackParam;

	    return VO_ERR_SOURCE_OK;
    }
    #if defined __cplusplus
    }
    #endif

#else  //_PUSHPLAYPARSER


#include "voOSFunc.h"
#ifdef _IOS
    #ifdef _AVI_READER
    #include "CAviReader.h"
	#define g_dwFRModuleID		VO_INDEX_SRC_AVI
    #endif	//_AVI_READER
    #ifdef _ASF_READER
    #include "CAsfReader.h"
	#define g_dwFRModuleID		VO_INDEX_SRC_ASF
    #endif	//_ASF_READER
    #ifdef _MP4_READER
    #ifdef _USE_J3LIB
    #include "CMp4Reader2.h"
    #else	//_USE_J3LIB
    #include "CMP4Reader.h"
    #endif	//_USE_J3LIB
	#define g_dwFRModuleID		VO_INDEX_SRC_MP4
    #endif	//_MP4_READER
    #ifdef _AUDIO_READER
    #include "CFileFormatCheck.h"
    #include "CMp3Reader.h"
    #include "CAacReader.h"
    #include "CWavReader.h"
    #include "CFlacReader.h"
    #include "CQcpReader.h"
    #include "CAmrReader.h"
    #include "CAuReader.h"
    #include "ape_reader.h"
    #include "CAC3Reader.h"
	#include "DTSReader.h"
	#define g_dwFRModuleID		VO_INDEX_SRC_AUDIO
    #endif	//_AUDIO_READER
    #ifdef _RM_READER
    #include "CRealReader.h"
	#define g_dwFRModuleID		VO_INDEX_SRC_REAL
    #endif	//_RM_READER
    #ifdef FLV_READER
    #include "CFlvReader.h"
	#define g_dwFRModuleID		VO_INDEX_SRC_FLASH
    #endif	//FLV_READER
    #ifdef _OGG_READER
    #include "oggreader.h"
	#define g_dwFRModuleID		VO_INDEX_SRC_OGG
    #endif  //_OGG_READER
    #ifdef _MKV_READER
    #include "CMKVFileReader.h"
	#define g_dwFRModuleID		VO_INDEX_SRC_MKV
    #endif  //_MKV_READER
    #ifdef _MPEG_READER
    #include "CMpegReader.h"
	#define g_dwFRModuleID		VO_INDEX_SRC_MPEG
    #endif	//_MPEG_READER
    #ifdef MTV_TS
    #include "CTsReader.h"
	#define g_dwFRModuleID		VO_INDEX_SRC_DVBT
    #endif	//MTV_TS    
#else
    #ifdef _AVI_READER
    #include "CAviReader.h"
    VO_U32 g_dwFRModuleID = VO_INDEX_SRC_AVI;
    #endif	//_AVI_READER
    #ifdef _ASF_READER
    #include "CAsfReader.h"
    VO_U32 g_dwFRModuleID = VO_INDEX_SRC_ASF;
    #endif	//_ASF_READER
    #ifdef _MP4_READER
    #ifdef _USE_J3LIB
    #include "CMp4Reader2.h"
    #else	//_USE_J3LIB
    #include "CMP4Reader.h"
    #endif	//_USE_J3LIB
    VO_U32 g_dwFRModuleID = VO_INDEX_SRC_MP4;
    #endif	//_MP4_READER
    #ifdef _AUDIO_READER
    #include "CFileFormatCheck.h"
    #include "CMp3Reader.h"
    #include "CAacReader.h"
    #include "CWavReader.h"
    #include "CFlacReader.h"
    #include "CQcpReader.h"
    #include "CAmrReader.h"
    #include "CAuReader.h"
    #include "ape_reader.h"
    #include "CAC3Reader.h"
	#include "DTSReader.h"
    VO_U32 g_dwFRModuleID = VO_INDEX_SRC_AUDIO;
    #endif	//_AUDIO_READER
    #ifdef _RM_READER
    #include "CRealReader.h"
    VO_U32 g_dwFRModuleID = VO_INDEX_SRC_REAL;
    #endif	//_RM_READER
    #ifdef FLV_READER
    #include "CFlvReader.h"
    VO_U32 g_dwFRModuleID = VO_INDEX_SRC_FLASH;
    #endif	//FLV_READER
    #ifdef _OGG_READER
    #include "oggreader.h"
    VO_U32 g_dwFRModuleID = VO_INDEX_SRC_OGG;
    #endif  //_OGG_READER
    #ifdef _MKV_READER
    #include "CMKVFileReader.h"
    VO_U32 g_dwFRModuleID = VO_INDEX_SRC_MKV;
    #endif  //_MKV_READER
    #ifdef _MPEG_READER
    #include "CMpegReader.h"
    VO_U32 g_dwFRModuleID = VO_INDEX_SRC_MPEG;
    #endif	//_MPEG_READER
    #ifdef MTV_TS
    #include "CTsReader.h"
	VO_U32 g_dwFRModuleID = VO_INDEX_SRC_TSP;
    #endif	//MTV_TS       
	#ifdef  _RAW_DATA_PARSER
    #include "H264RawData.h"
	#include "H265RawData.h"
    VO_U32 g_dwFRModuleID = VO_INDEX_SRC_RAWDATA;
	#endif //_RAW_DATA_PARSER

#endif

    #define LOG_TAG "voFileReader"
    #include "voLog.h"

#if !defined(__arm) && !defined(_MAC_OS) && !defined(_IOS)
    VO_HANDLE g_hInst = VO_NULL;
#endif


#ifdef _WIN32
    #include <Windows.h>


#if defined __cplusplus
extern "C" {
#endif
    
    #ifndef _LIB
    BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
    {   
	    g_hInst = hModule;
	    return TRUE;
    }
    #endif
    
#if defined __cplusplus
    }
#endif

#endif	//_WIN32


        
#ifdef _VONAMESPACE
        namespace _VONAMESPACE {
#else
#ifdef __cplusplus
            extern "C" {
#endif /* __cplusplus */
#endif


    VO_U32 VO_API voFROpen(VO_PTR* ppHandle, VO_SOURCE_OPENPARAM* pParam)
    {

	    if(VO_SOURCE_OPENPARAM_FLAG_OPENLOCALFILE != (pParam->nFlag & 0xFF) && 
		    (VO_SOURCE_OPENPARAM_FLAG_OPENPD != (pParam->nFlag & 0xFF))
			&& (VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL != (pParam->nFlag & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL)))
	    {
		    VOLOGE("source open flags unvalid!!");
		    return VO_ERR_INVALID_ARG;
	    }

	    if(VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR != (pParam->nFlag & 0xFF00))
	    {
		    VOLOGE("source operator flags unvalid!!");
		    return VO_ERR_INVALID_ARG;
	    }

    #ifdef _AVI_READER
	    CAviReader* pReader = new CAviReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    #endif	//_AVI_READER
    #ifdef _ASF_READER
	    CAsfReader* pReader = new CAsfReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    #endif	//_ASF_READER
    #ifdef _MP4_READER
    #ifdef _USE_J3LIB
	    CMp4Reader2* pReader = new CMp4Reader2((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    #else	//_USE_J3LIB
	    CMp4Reader* pReader = new CMp4Reader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    #endif	//_USE_J3LIB
    #endif	//_MP4_READER
    #ifdef _AUDIO_READER
	    //check file format
	    CFileFormatCheck checker((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP);
	    VO_FILE_FORMAT ff = checker.GetFileFormat((VO_FILE_SOURCE*)pParam->pSource, FLAG_CHECK_AUDIOREADER);
	    //new file parser!!
	    CBaseAudioReader* pReader = VO_NULL;
	    if(VO_FILE_FFAUDIO_MP3 == ff)
		    pReader = new CMp3Reader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
	    else if(VO_FILE_FFAUDIO_AAC == ff)
		    pReader = new CAacReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
	    else if(VO_FILE_FFAUDIO_WAV == ff)
		    pReader = new CWavReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
	    else if(VO_FILE_FFAUDIO_AMR == ff || VO_FILE_FFAUDIO_AWB == ff)
		    pReader = new CAmrReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
	    else if(VO_FILE_FFAUDIO_QCP == ff)
		    pReader = new CQcpReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
	    else if(VO_FILE_FFAUDIO_FLAC == ff)
		    pReader = new CFlacReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
	    else if(VO_FILE_FFAUDIO_AU == ff)
		    pReader = new CAuReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
	    else if( VO_FILE_FFAUDIO_APE == ff )
		    pReader = new ape_reader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
	    else if(VO_FILE_FFAUDIO_AC3 == ff)
		    pReader = new CAC3Reader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
	    else if(VO_FILE_FFAUDIO_DTS == ff)
		    pReader = new CDTSReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
	    else
	    {
		    VOLOGE("Unknow file format!!");
		    return VO_ERR_SOURCE_FORMATUNSUPPORT;
	    }

    #endif	//_AUDIO_READER
    #ifdef _RM_READER
	    CRealReader* pReader = new CRealReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    #endif	//_RM_READER
    #ifdef FLV_READER
	    CFlvReader* pReader = new CFlvReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    #endif	//FLV_READER
    #ifdef _OGG_READER
	    oggreader * pReader = new oggreader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    #endif  //_OGG_READER
    #ifdef _MKV_READER
	    CMKVFileReader * pReader = new CMKVFileReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    #endif  //_MKV_READER
    #ifdef _MPEG_READER
	    CMpegReader* pReader = new CMpegReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    #endif	//_MPEG_READER
    #ifdef MTV_TS//MTV_TS
	    CTsReader* pReader =new  CTsReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);;
    #endif  //MTV_TS
    
    #ifdef _RAW_DATA_PARSER
	    //check file format
	    CFileFormatCheck checker((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP);
	    VO_FILE_FORMAT ff = checker.GetFileFormat((VO_FILE_SOURCE*)pParam->pSource, FLAG_CHECK_AUDIOREADER);
	    //new file parser!!
	    CBaseReader* pReader = VO_NULL;
		if(VO_FILE_FFVIDEO_H264 == ff)
	        pReader = new CH264RawData((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
		else if(VO_FILE_FFVIDEO_H265 == ff)
			pReader = new CH265RawData((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    #endif	//_RAW_DATA_PARSER
	    if(!pReader)
	    {
		    VOLOGE("create reader fail!!");
		    return VO_ERR_OUTOF_MEMORY;
	    }

		VO_U32 rc = pReader->init(pParam->nFlag, pParam->pLibOP, g_dwFRModuleID,pParam->pstrWorkpath);
		if(VO_ERR_SOURCE_OK != rc)
		{
			VOLOGE("file parser init fail: 0x%08X!!", (unsigned int)rc);
			pReader->uninit();
			delete pReader;
			return rc;
		}

	    rc = pReader->Load(pParam->nFlag, (VO_FILE_SOURCE*)pParam->pSource);
	    //only OK/DRM can be playback
	    if(VO_ERR_SOURCE_OK != rc && VO_ERR_SOURCE_CONTENTENCRYPT != rc)
	    {
		    VOLOGE("file parser load fail: 0x%08X!!", (unsigned int)rc);
		    delete pReader;
		    return rc;
	    }
	    *ppHandle = pReader;
	    return rc;
    }

    VO_U32 VO_API voFRClose(VO_PTR pHandle)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    CBaseReader* pReader = (CBaseReader*)pHandle;
		pReader->uninit();
	    VO_U32 rc = pReader->Close();
	    delete pReader;
	    return rc;
    }

    VO_U32 VO_API voFRGetFileInfo(VO_PTR pHandle, VO_SOURCE_INFO* pSourceInfo)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    CBaseReader* pReader = (CBaseReader*)pHandle;
	    return pReader->GetInfo(pSourceInfo);
    }

    VO_U32 VO_API voFRGetTrackInfo(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_TRACKINFO* pTrackInfo)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

    #ifndef _AUDIO_READER
	    CBaseReader* pReader = (CBaseReader*)pHandle;
	    CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
	    if(!pTrack)
		    return VO_ERR_INVALID_ARG;
	    //VOLOGR("+voFRGetTrackInfo. PID : 0x%08x", nTrack);
		VO_U32 nRc = pTrack->GetInfo(pTrackInfo);
		//	VOLOGR("+voFRGetTrackInfo. PID : 0x%08x", nTrack);
	    return nRc;
    #else	//_AUDIO_READER
	    CBaseAudioReader* pReader = (CBaseAudioReader*)pHandle;
	    return pReader->GetTrackInfo(pTrackInfo);
    #endif	//_AUDIO_READER
    }

    VO_U32 VO_API voFRGetSample(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_SAMPLE* pSample)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

    #if defined(_AUDIO_READER)
	    CBaseAudioReader* pReader = (CBaseAudioReader*)pHandle;
	    if(pReader->IsEndOfStream())
	    {
		    VOLOGR("track is end!!");

		    return VO_ERR_SOURCE_END;
	    }

	    VO_U32 rc = pReader->GetSample(pSample);
	    if(VO_ERR_SOURCE_OK != rc)
	    {
		    if(VO_ERR_SOURCE_END == rc)
			    pReader->SetEndOfStream(VO_TRUE);

		    VOLOGR("get sample error: 0x%08X!!", rc);

		    return rc;
	    }

	    pReader->CheckAudio(pSample);
	//#elif defined(_RAW_DATA_PARSER)
	//	CH264RawData *pRawDataParser = (CH264RawData*)pHandle;
	//	return pRawDataParser->GetSample(pSample);
    #else
	    CBaseReader* pReader = (CBaseReader*)pHandle;
	    CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
	    if(!pTrack || !pTrack->IsInUsed())
	    {
		    VOLOGR("error track index: %d!!", nTrack);

		    return VO_ERR_INVALID_ARG;
	    }

	    if(pTrack->IsEndOfStream())
	    {
		    VOLOGR("track is end!!");

		    return VO_ERR_SOURCE_END;
	    }

	    pSample->Flag = 0;

		VO_U32 rc = pTrack->GetSample(pSample);

	    if(VO_ERR_SOURCE_OK != rc)
	    {
		    if(VO_ERR_SOURCE_END == rc)
			    pTrack->SetEndOfStream(VO_TRUE);

		    VOLOGR("get sample error: 0x%08X!!", rc);

		    return rc;
	    }

	    VO_TRACKTYPE nTrackType = pTrack->GetType();
	    if(VOTT_VIDEO == nTrackType)
		    pReader->CheckVideo(pSample);
	    else if(VOTT_AUDIO == nTrackType)
		    pReader->CheckAudio(pSample);
    #endif	//_AUDIO_READER

//	    VOLOGR("track type %d getsample ok, size: 0x%08X, time: %lld!!", (VO_U32)pTrack->GetType(), pSample->Size, pSample->Time);

	    return VO_ERR_SOURCE_OK;
    }

    VO_U32 VO_API voFRSetPos(VO_PTR pHandle, VO_U32 nTrack, VO_S64* pPos)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    VOLOGR("%d track want to setpos %d!!", nTrack, VO_S32(*pPos));
    #ifndef _AUDIO_READER
	    CBaseReader* pReader = (CBaseReader*)pHandle;
	    CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
	    if(!pTrack || !pTrack->IsInUsed())
	    {
		    VOLOGR("error track index: %d!!", nTrack);

		    return VO_ERR_INVALID_ARG;
	    }

	    if(VO_SOURCE_PM_PLAY == pTrack->GetPlayMode())
	    {
		    if(0xffffffff == pReader->GetSeekTrack())	//not initialize!!
		    {
			    VOLOGR("set seek track: %d!!", nTrack);

			    pReader->SetSeekTrack(nTrack);
		    }

		    if(pReader->GetSeekTrack() == nTrack)	//the track will activate file set position
		    {
			    VOLOGR("%d track activate moveto %d!!", nTrack, VO_S32(*pPos));

			    pReader->MoveTo(*pPos);
		    }
	    }

	    pTrack->SetEndOfStream(VO_FALSE);

	    VO_U32 rc = pTrack->SetPos(pPos);
	    if(VO_ERR_SOURCE_END == rc)
		    pTrack->SetEndOfStream(VO_TRUE);
    #else	//_AUDIO_READER
	    CBaseAudioReader* pReader = (CBaseAudioReader*)pHandle;
	    pReader->SetEndOfStream(VO_FALSE);

	    VO_U32 rc = pReader->SetPos(pPos);
	    if(VO_ERR_SOURCE_END == rc)
		    pReader->SetEndOfStream(VO_TRUE);
    #endif	//_AUDIO_READER
	    VOLOGR("setpos %d return 0x%08X!!", VO_S32(*pPos), rc);

	    return rc;
    }

    VO_U32 VO_API voFRSetFileParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    CBaseReader* pReader = (CBaseReader*)pHandle;
	    return pReader->SetParameter(uID, pParam);
    }

    VO_U32 VO_API voFRGetFileParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    CBaseReader* pReader = (CBaseReader*)pHandle;
//  		VO_U32 xID = VO_PID_METADATA_COMPOSER;
//  		VO_FILE_MEDIATIMEFILEPOS ffm;
//  		ffm.llFilePos = 1024 *1000;
//  		VO_U32 nRc1 = pReader->GetParameter(xID, &ffm);

	    VOLOGR("+GetParameter. PID : 0x%08x", uID);
	    VO_U32  nRc = pReader->GetParameter(uID, pParam);
	    VOLOGR("-GetParameter. Res: 0x%08x", nRc);

	     return nRc;
    }

    VO_U32 VO_API voFRSetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

    #ifndef _AUDIO_READER
	    CBaseReader* pReader = (CBaseReader*)pHandle;
	    CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
	    if(!pTrack)
		    return VO_ERR_INVALID_ARG;

	    return pTrack->SetParameter(uID, pParam);
    #else	//_AUDIO_READER
	    CBaseAudioReader* pReader = (CBaseAudioReader*)pHandle;
	    return pReader->SetTrackParameter(uID, pParam);
    #endif	//_AUDIO_READER
    }

    VO_U32 VO_API voFRGetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

    #ifndef _AUDIO_READER
	    CBaseReader* pReader = (CBaseReader*)pHandle;
	    //nTrack = 2;
	    //uID =0x41000001;
	    CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
	    if(!pTrack)
		    return VO_ERR_INVALID_ARG;

		VOLOGR("+GetParameter.TrackID:%d, PID : 0x%08x",nTrack, uID);
	    VO_U32 nRc=  pTrack->GetParameter(uID, pParam);
	    VOLOGR("-GetParameter.TrackID:%d, RES : 0x%08x",nTrack, nRc);
	    return nRc;
    #else	//_AUDIO_READER
	    CBaseAudioReader* pReader = (CBaseAudioReader*)pHandle;
	    return pReader->GetTrackParameter(uID, pParam);
    #endif	//_AUDIO_READER
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

#if defined __cplusplus
            extern "C" {
#endif
    #ifdef _AVI_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetAVIReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//_AVI_READER
    #ifdef _ASF_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetASFReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//_ASF_READER
    #ifdef _MP4_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetMP4ReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//_MP4_READER
    #ifdef _AUDIO_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetAudioReadAPI(VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag)
    #endif	//_AUDIO_READER
    #ifdef _RM_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetRealReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//_RM_READER
    #ifdef FLV_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetFLVReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//FLV_READER
    #ifdef _OGG_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetOGGReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif  //_OGG_READER
    #ifdef _MKV_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetMKVReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif  //_MKV_READER
    #ifdef _MPEG_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetMPGReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//_MPEG_READER
    #ifdef MTV_TS//MTV_TS
    VO_EXPORT_FUNC VO_S32 VO_API voGetMTVReadAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//MTV_TS
    #ifdef _RAW_DATA_PARSER
    VO_EXPORT_FUNC VO_S32 VO_API voGetRawDataParserAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    #endif	//_RAW_DATA_PARSER
    {
	    pReadHandle->Open = voFROpen;
	    pReadHandle->Close = voFRClose;
	    pReadHandle->GetSourceInfo = voFRGetFileInfo;
	    pReadHandle->GetTrackInfo = voFRGetTrackInfo;
	    pReadHandle->GetSample = voFRGetSample;
	    pReadHandle->SetPos = voFRSetPos;
	    pReadHandle->SetSourceParam = voFRSetFileParam;
	    pReadHandle->GetSourceParam = voFRGetFileParam;
	    pReadHandle->SetTrackParam = voFRSetTrackParam;
	    pReadHandle->GetTrackParam = voFRGetTrackParam;

	    return VO_ERR_SOURCE_OK;
    }
    #if defined __cplusplus
    }
    #endif


#endif //_PUSHPLAYPARSER

#ifdef _SOURCE2

#include "voSource2.h"
#include "voSource2WrapperManager.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
#endif
        
/*
#ifdef _IOS
#ifdef _ASF_READER
	using namespace _ASF;
#endif
#ifdef _AUDIO_READER
	using namespace _AUDIO;
#endif
#ifdef _AVI_READER
	using namespace _AVI;
#endif
#ifdef FLV_READER
	using namespace _FLV_READER;
#endif
#ifdef _MKV_READER
	using namespace _MKV;
#endif
#ifdef _MP4_READER
	using namespace MP4;
#endif
#ifdef _MPEG_READER
	using namespace _MPEG;
#endif
#ifdef _OGG_READER
	using namespace _OGG;
#endif
#ifdef _RM_READER
	using namespace _RM;
#endif
#ifdef MTV_TS
	using namespace TS;
#endif
#endif	// end of _IOS
*/

	VO_U32 Source2_Init(VO_HANDLE * phHandle, VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pParam )
	{
		voSource2WrapperManager * ptr_obj = new voSource2WrapperManager;
		*phHandle = ptr_obj;
		return ptr_obj->Init( pSource , nFlag , pParam );

	}

	VO_U32 Source2_Uninit(VO_HANDLE hHandle)
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		VO_U32 ret = ptr_obj->Uninit();

		delete ptr_obj;

		return ret;
	}

	VO_U32 Source2_Open( VO_HANDLE hHandle )
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->Open();
	}

	VO_U32 Source2_Close( VO_HANDLE hHandle )
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->Close();
	}

	VO_U32 Source2_Start(VO_HANDLE hHandle)
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->Start();
	}

	VO_U32 Source2_Pause(VO_HANDLE hHandle)
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->Pause();
	}

	VO_U32 Source2_Stop(VO_HANDLE hHandle)
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->Stop();
	}

	VO_U32 Source2_Seek(VO_HANDLE hHandle, VO_U64* pTimeStamp)
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->Seek( pTimeStamp );
	}

	VO_U32 Source2_GetDuration(VO_PTR hHandle, VO_U64 * pDuration)
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->GetDuration( pDuration );
	}

	VO_U32 Source2_GetSample( VO_HANDLE hHandle , VO_SOURCE2_TRACK_TYPE nOutPutType , VO_PTR pSample )
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->GetSample( nOutPutType , pSample );
	}

	VO_U32 Source2_GetProgramCount(VO_HANDLE hHandle, VO_U32 *pProgramCount)
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->GetProgramCount( pProgramCount );
	}

	VO_U32 Source2_GetProgramInfo(VO_HANDLE hHandle, VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo)
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->GetProgramInfo( nProgram , pProgramInfo );
	}

	VO_U32 Source2_GetCurTrackInfo( VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo )
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->GetCurTrackInfo( nTrackType , ppTrackInfo );
	}

	VO_U32 Source2_SelectProgram(VO_HANDLE hHandle, VO_U32 nProgram)
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->SelectProgram( nProgram );
	}

	VO_U32 Source2_SelectStream(VO_HANDLE hHandle, VO_U32 nStream)
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->SelectStream( nStream );
	}

	VO_U32 Source2_SelectTrack(VO_HANDLE hHandle, VO_U32 nTrack)
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->SelectTrack(nTrack);
	}

	VO_U32 Source2_GetDRMInfo(VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO **ppDRMInfo)
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->GetDRMInfo( ppDRMInfo );
	}

	VO_U32 Source2_SendBuffer(VO_HANDLE hHandle, const VO_SOURCE2_SAMPLE& buffer )
	{	
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->SendBuffer( buffer );
	}

	VO_U32 Source2_GetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
	{
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->GetParam( nParamID , pParam );
	}

	VO_U32 Source2_SetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
	{
		if(nParamID == VO_PID_COMMON_LOGFUNC)
		{
			VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
//			vologInit(pVologCB->pUserData, pVologCB->fCallBack);
		}
		if(!hHandle)
			return VO_RET_SOURCE2_FAIL;
		voSource2WrapperManager * ptr_obj = (voSource2WrapperManager *)hHandle;
		return ptr_obj->SetParam( nParamID , pParam );
	}
        
#ifdef _VONAMESPACE
    }
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _VONAMESPACE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	#ifdef _AVI_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetAVI2ReadAPI(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag)
    #endif	//_AVI_READER
    #ifdef _ASF_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetASF2ReadAPI(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag)
    #endif	//_ASF_READER
    #ifdef _MP4_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetMP42ReadAPI(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag)
    #endif	//_MP4_READER
    #ifdef _AUDIO_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetAudio2ReadAPI(VO_SOURCE2_API * pReadHandle, VO_U32 uFlag)
    #endif	//_AUDIO_READER
    #ifdef _RM_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetReal2ReadAPI(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag)
    #endif	//_RM_READER
    #ifdef FLV_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetFLV2ReadAPI(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag)
    #endif	//FLV_READER
    #ifdef _OGG_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetOGG2ReadAPI(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag)
    #endif  //_OGG_READER
    #ifdef _MKV_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetMKV2ReadAPI(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag)
    #endif  //_MKV_READER
    #ifdef _MPEG_READER
    VO_EXPORT_FUNC VO_S32 VO_API voGetMPG2ReadAPI(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag)
    #endif	//_MPEG_READER
    #ifdef MTV_TS
    VO_EXPORT_FUNC VO_S32 VO_API voGetMTV2ReadAPI(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag)
    #endif	//MTV_TS    
    #ifdef _RAW_DATA_PARSER
    VO_EXPORT_FUNC VO_S32 VO_API voGetRawDataParser2API(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag)
    #endif	//_RAW_DATA_PARSER
    {
        
		pReadHandle->Init = Source2_Init;
		pReadHandle->Uninit = Source2_Uninit;
		pReadHandle->Open = Source2_Open;
		pReadHandle->Close = Source2_Close;
		pReadHandle->Start = Source2_Start;
		pReadHandle->Pause = Source2_Pause;
		pReadHandle->Stop = Source2_Stop;
		pReadHandle->Seek = Source2_Seek;
		pReadHandle->GetDuration = Source2_GetDuration;
		pReadHandle->GetSample = Source2_GetSample;
		pReadHandle->GetProgramCount = Source2_GetProgramCount;
		pReadHandle->GetProgramInfo = Source2_GetProgramInfo;
		pReadHandle->GetCurTrackInfo = Source2_GetCurTrackInfo;
		pReadHandle->SelectProgram = Source2_SelectProgram;
		pReadHandle->SelectStream = Source2_SelectStream;
		pReadHandle->SelectTrack = Source2_SelectTrack;
		pReadHandle->GetDRMInfo = Source2_GetDRMInfo;
		pReadHandle->SendBuffer = Source2_SendBuffer;
		pReadHandle->GetParam = Source2_GetParam;
		pReadHandle->SetParam = Source2_SetParam;
	    return VO_ERR_SOURCE_OK;
    }
	
#if defined __cplusplus
}
#endif

#endif //_SOURCE2

