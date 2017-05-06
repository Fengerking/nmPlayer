
#include "voSource2ParserWrapper.h"
#include "voFile.h"
#include "voLog.h"
#include "fCC.h"
#include "voMetaData.h"
#include "voOSFunc.h"

#ifdef _AVI_READER
#include "CAviReader.h"
const VO_U32 g_dwFRModuleID = VO_INDEX_SRC_AVI;
#endif	//_AVI_READER
#ifdef _ASF_READER
#include "CAsfReader.h"
const VO_U32 g_dwFRModuleID = VO_INDEX_SRC_ASF;
#endif	//_ASF_READER
#ifdef _MP4_READER
#ifdef _USE_J3LIB
#include "CMp4Reader2.h"
#else	//_USE_J3LIB
#include "CMP4Reader.h"
#endif	//_USE_J3LIB
const VO_U32 g_dwFRModuleID = VO_INDEX_SRC_MP4;
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
const VO_U32 g_dwFRModuleID = VO_INDEX_SRC_AUDIO;
#endif	//_AUDIO_READER
#ifdef _RM_READER
#include "CRealReader.h"
const VO_U32 g_dwFRModuleID = VO_INDEX_SRC_REAL;
#endif	//_RM_READER
#ifdef FLV_READER
#include "CFlvReader.h"
const VO_U32 g_dwFRModuleID = VO_INDEX_SRC_FLASH;
#endif	//FLV_READER
#ifdef _OGG_READER
#include "oggreader.h"
const VO_U32 g_dwFRModuleID = VO_INDEX_SRC_OGG;
#endif  //_OGG_READER
#ifdef _MKV_READER
#include "CMKVFileReader.h"
const VO_U32 g_dwFRModuleID = VO_INDEX_SRC_MKV;
#endif  //_MKV_READER
#ifdef _MPEG_READER
#include "CMpegReader.h"
const VO_U32 g_dwFRModuleID = VO_INDEX_SRC_MPEG;
#endif	//_MPEG_READER
#ifdef MTV_TS
#include "CTsReader.h"
const VO_U32 g_dwFRModuleID = VO_INDEX_SRC_TSP;
#endif  //MTV_TS
#ifdef PD
#include "CBaseReader.h"
#endif

#ifdef  _RAW_DATA_PARSER
#include "H264RawData.h"
#include "H265RawData.h"
const VO_U32 g_dwFRModuleID = VO_INDEX_SRC_RAWDATA;
#endif //_RAW_DATA_PARSER

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

voSource2ParserWrapper::voSource2ParserWrapper()
: m_Handle(NULL)
, m_ullActualFileSize(0xFFFFFFFFFFFFFFFFLL)
{
	memset(&m_FileOpr,0,sizeof(VO_FILE_OPERATOR));
	memset(&m_SrcIO,0,sizeof(voSourceSwitch));
}

voSource2ParserWrapper::~voSource2ParserWrapper()
{
}

VO_U32 voSource2ParserWrapper::Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pParam )
{
	if ( NULL != pParam &&
		(pParam->uFlag & VO_SOURCE2_FLAG_INIT_IO) )
	{
		VO_SOURCE2_COMMONINITPARAM* p = static_cast<VO_SOURCE2_COMMONINITPARAM*>(pParam->pInitParam);

		if (m_SrcIO.oprIO2)
		{
			delete (VO_SOURCE2_IO_API*)m_SrcIO.oprIO2;
			m_SrcIO.oprIO2 = NULL;
		}

		m_SrcIO.oprIO2 = new VO_SOURCE2_IO_API;
		memcpy(m_SrcIO.oprIO2, p->pIO, sizeof(VO_SOURCE2_IO_API));
		VOLOGI("voSource2ParserWrapper::Init, begin to initial m_FileOpr and uninitial value: %p",m_FileOpr.Open);
		CSourceIOSwitchFillPointer(&m_FileOpr);
		VOLOGI("voSource2ParserWrapper::Init, the initialed value of m_FileOpr : %p",m_FileOpr.Open);
		//Set work path to I/O
		VO_SOURCE2_IO_API* OperIO2 = (VO_SOURCE2_IO_API*)m_SrcIO.oprIO2;
		OperIO2->SetParam(NULL,VO_PID_SOURCE2_WORKPATH,pParam->strWorkPath);
	}

	if(nFlag & VO_SOURCE2_FLAG_OPEN_THUMBNAIL)
	{
		m_nOpenFlag |= VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL;
	}
	return voSourceBaseWrapper::Init(pSource, nFlag, pParam);
}

VO_U32 voSource2ParserWrapper::OnOpen()
{
	VO_FILE_SOURCE Source;
	memset( &Source , 0 , sizeof( VO_FILE_SOURCE ) );

	if(m_FileHandle != NULL)
	{
		Source.pSource = m_FileHandle;
		Source.nFlag = VO_FILE_TYPE_IMFBYTESTREAM;
	}
	else
	{
		Source.pSource = m_Url;
		Source.nFlag = VO_FILE_TYPE_NAME;
	}
	Source.nReserve = (VO_U32)&m_SrcIO;

	Source.nMode = VO_FILE_READ_ONLY;
	VO_FILE_OPERATOR* FileOpr = NULL;
	if (m_FileOpr.Open)
	{
		FileOpr = &m_FileOpr;

		VO_U32 uFlag = 0;
		if (m_SrcIO.bDrm)
		{
			uFlag |= VO_SOURCE2_IO_FLAG_OPEN_DRM;
		}
		VO_SOURCE2_IO_API* OperIO2 = (VO_SOURCE2_IO_API*)m_SrcIO.oprIO2;
		
		if (!OperIO2 || !(OperIO2->Init))
		{
			return VO_RET_SOURCE2_OPENFAIL;
		}
		VOLOGI("Start to init the source IO and source: %s",(VO_TCHAR *)Source.pSource);
		VO_U32 ret = OperIO2->Init(&m_SrcIO.phndIO2,Source.pSource,uFlag,NULL);
		if (ret != VO_SOURCE2_IO_OK || !m_SrcIO.phndIO2)
		{
			return VO_ERR_OUTOF_MEMORY;
		}
		VOLOGI("Init the source IO successfully");
		VOLOGI("VO_SOURCE2_IO_PARAMID_DRMPOINTOR %p", m_ptr_drmcallback);
		ret = OperIO2->SetParam(m_SrcIO.phndIO2, VO_SOURCE2_IO_PARAMID_DRMPOINTOR, m_ptr_drmcallback);
		if (ret) {
			VOLOGE("!Set DRM POINTOR 0x%08x", ret);
			return ret;
		}
		
		ret = OperIO2->SetParam(m_SrcIO.phndIO2, VO_PID_SOURCE2_ACTUALFILESIZE, &m_ullActualFileSize);
		if (ret) {
			VOLOGW("!Set ACTUAL FILE SIZE 0x%08x", ret);
		}
	}
#ifdef _AVI_READER
    CAviReader* pReader = new CAviReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
#endif	//_AVI_READER
#ifdef _ASF_READER
    CAsfReader* pReader = new CAsfReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
#endif	//_ASF_READER
#ifdef _MP4_READER
#ifdef _USE_J3LIB
    CMp4Reader2* pReader = new CMp4Reader2(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
#else	//_USE_J3LIB
    CMp4Reader* pReader = new CMp4Reader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
#endif	//_USE_J3LIB
#endif	//_MP4_READER
#ifdef _AUDIO_READER
    //check file format
    CFileFormatCheck checker(FileOpr,0);
    VO_FILE_FORMAT ff = checker.GetFileFormat(&Source, FLAG_CHECK_AUDIOREADER);
    //new file parser!!
    CBaseAudioReader* pReader = VO_NULL;
    if(VO_FILE_FFAUDIO_MP3 == ff)
	    pReader = new CMp3Reader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
    else if(VO_FILE_FFAUDIO_AAC == ff)
	    pReader = new CAacReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
    else if(VO_FILE_FFAUDIO_WAV == ff)
	    pReader = new CWavReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
    else if(VO_FILE_FFAUDIO_AMR == ff || VO_FILE_FFAUDIO_AWB == ff)
	    pReader = new CAmrReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
    else if(VO_FILE_FFAUDIO_QCP == ff)
	    pReader = new CQcpReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
    else if(VO_FILE_FFAUDIO_FLAC == ff)
	    pReader = new CFlacReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
    else if(VO_FILE_FFAUDIO_AU == ff)
	    pReader = new CAuReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
    else if( VO_FILE_FFAUDIO_APE == ff )
	    pReader = new ape_reader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
    else if(VO_FILE_FFAUDIO_AC3 == ff)
	    pReader = new CAC3Reader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
    else if(VO_FILE_FFAUDIO_DTS == ff)
	    pReader = new CDTSReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
    else
    {
	    VOLOGE("Unknow file format!!");
	    return VO_ERR_SOURCE_FORMATUNSUPPORT;
    }

#endif	//_AUDIO_READER
#ifdef _RAW_DATA_PARSER
		CBaseReader* pReader = VO_NULL;
#if 0
	    //check file format
		CFileFormatCheck checker(0,0);
		VO_FILE_FORMAT ff = checker.GetFileFormat(&Source, FLAG_CHECK_AUDIOREADER);
	    //new file parser!!
	    
		if(VO_FILE_FFVIDEO_H264 == ff)
	        pReader = new CH264RawData(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
		else if(VO_FILE_FFVIDEO_H265 == ff)
#endif
			if(Source.pSource)
			{
				
				VO_S32 iFileType = RawDataGetFileFormat((VO_PTCHAR)Source.pSource);
				//iFileType = VO_FILE_FFVIDEO_H265;
				if(VO_FILE_FFVIDEO_H264 == iFileType)
					pReader = new CH264RawData(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
				else if(VO_FILE_FFVIDEO_H265 == iFileType)
					pReader = new CH265RawData(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
				//if(0 == _stricmp(((char *)Source.pSource + (strlen((char *)Source.pSource)-4)),"H265"))
				//	pReader = new CH265RawData(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
				///else if(0 == _stricmp(((char *)Source.pSource + (strlen((char *)Source.pSource)-4)),"H264"))
				//	pReader = new CH264RawData(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
			}
#endif	//_RAW_DATA_PARSER
#ifdef _RM_READER
    CRealReader* pReader = new CRealReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
#endif	//_RM_READER
#ifdef FLV_READER
    CFlvReader* pReader = new CFlvReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
#endif	//FLV_READER
#ifdef _OGG_READER
    oggreader * pReader = new oggreader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
#endif  //_OGG_READER
#ifdef _MKV_READER
    CMKVFileReader * pReader = new CMKVFileReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
#endif  //_MKV_READER
#ifdef _MPEG_READER
    CMpegReader* pReader = new CMpegReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
#endif	//_MPEG_READER
#ifdef PD
	CBaseReader* pReader = 0;
#endif
#ifdef MTV_TS
	CTsReader* pReader = new CTsReader(FileOpr, 0, 0, (VO_SOURCEDRM_CALLBACK *)m_ptr_drmcallback);
#endif
    if(!pReader)
    {
	    VOLOGE("create reader fail!!");
	    return VO_ERR_OUTOF_MEMORY;
    }

	VO_U32 rc = pReader->init(m_nOpenFlag, m_pLibOp, g_dwFRModuleID,m_pstrWorkPath);
	if(VO_ERR_SOURCE_OK != rc)
	{
		VOLOGE("file parser init fail: 0x%08X!!", (unsigned int)rc);
		pReader->uninit();
		delete pReader;
		return rc;
	}

    rc = pReader->Load(m_nOpenFlag, &Source);
    //only OK/DRM can be playback
    if(VO_ERR_SOURCE_OK != rc && VO_ERR_SOURCE_CONTENTENCRYPT != rc)
    {
	    VOLOGE("file parser load fail: 0x%08X!!", (unsigned int)rc);
	    delete pReader;
	    return VO_RET_SOURCE2_OPENFAIL;
    }
    m_Handle = pReader;

    return VO_RET_SOURCE2_OK;
}

VO_U32 voSource2ParserWrapper::OnClose()
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader )
		return VO_RET_SOURCE2_OK;

	pReader->uninit();
    VO_U32 rc = pReader->Close();
    delete pReader;
	m_Handle = 0;

	VO_SOURCE2_IO_API *OperIO2 = (VO_SOURCE2_IO_API*)m_SrcIO.oprIO2;

	if (OperIO2 && OperIO2->UnInit)
	{
		OperIO2->UnInit(m_SrcIO.phndIO2);
		delete OperIO2;
		OperIO2 = NULL;
	}
    return VO_RET_SOURCE2_OK;
}


VO_U32 voSource2ParserWrapper::Open()
{
	VO_U32 dwRet = VO_RET_SOURCE2_OK;
	
	dwRet = voSourceBaseWrapper::Open();
	
	if ((m_nFlag & VO_SOURCE2_FLAG_OPEN_ASYNC) && 
		m_ptr_EventCallback &&
		m_ptr_EventCallback->SendEvent)
	{
		VOLOGI("Open 0x%08X", dwRet);  
		m_ptr_EventCallback->SendEvent(m_ptr_EventCallback->pUserData, VO_EVENTID_SOURCE2_OPENCOMPLETE, (VO_U32)&dwRet, 0);
	}
	
	return dwRet;
}


VO_U32 voSource2ParserWrapper::Seek(VO_U64* pTimeStamp)
{
	VO_U32 dwRet = VO_RET_SOURCE2_FAIL;
	VO_U32 VideoRet = VO_RET_SOURCE2_FAIL;
	VO_U32 AudioRet = VO_RET_SOURCE2_FAIL;
	VO_U32 SubtitleRet = VO_RET_SOURCE2_FAIL;

	 if (0xFFFFFFFF != m_uVideoTrack){
		 VideoRet = SetTrackPos(m_uVideoTrack, pTimeStamp);
	 }
	
	if (0xFFFFFFFF != m_uAudioTrack){
		AudioRet = SetTrackPos(m_uAudioTrack, pTimeStamp);
	}

	if (0xFFFFFFFF != m_uSubtitleTrack){
		SubtitleRet = SetTrackPos(m_uSubtitleTrack, pTimeStamp);
	}

	if((VO_RET_SOURCE2_OK == VideoRet ||VO_RET_SOURCE2_END == VideoRet) ||
		(VO_RET_SOURCE2_OK == AudioRet ||VO_RET_SOURCE2_END == AudioRet) ||
		(VO_RET_SOURCE2_OK == SubtitleRet || VO_RET_SOURCE2_END == SubtitleRet)){
		dwRet = VO_RET_SOURCE2_OK;
	}

	voSourceBaseWrapper::Seek(pTimeStamp);
	
	if ((m_nFlag & VO_SOURCE2_FLAG_OPEN_ASYNC) && 
		m_ptr_EventCallback &&
		m_ptr_EventCallback->SendEvent)
	{
		VOLOGI("Seek 0x%08X, TimeStamp %llu", dwRet, *pTimeStamp);  
		m_ptr_EventCallback->SendEvent(m_ptr_EventCallback->pUserData, VO_EVENTID_SOURCE2_SEEKCOMPLETE, (VO_U32)pTimeStamp, (VO_U32)&dwRet);
	}
	
	return dwRet;
}

VO_U32 voSource2ParserWrapper::OnGetSample(VO_SOURCE2_TRACK_TYPE nOutPutType , VO_SOURCE_SAMPLE * pSample)
{
	if(!m_Handle || !pSample)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
#if defined(_AUDIO_READER)

	if( nOutPutType != VO_SOURCE2_TT_AUDIO )
		return VO_RET_SOURCE2_TRACKNOTFOUND;

    CBaseAudioReader* pReader = (CBaseAudioReader*)m_Handle;
    if(pReader->IsEndOfStream())
    {
	    VOLOGI("track is end!!");

	    return VO_RET_SOURCE2_END;
    }

    VO_U32 rc = pReader->GetSample(pSample);
    if(VO_ERR_SOURCE_OK != rc)
    {
	    if(VO_ERR_SOURCE_END == rc)
		    pReader->SetEndOfStream(VO_TRUE);

	    VOLOGI("get sample error: 0x%08X!!", rc);

	    if(VO_ERR_SOURCE_END == rc)
			return VO_RET_SOURCE2_END;
		else
			return VO_RET_SOURCE2_FAIL;
    }
    pReader->CheckAudio(pSample);

#else
	VO_U32 uTrack = 0;

	if( nOutPutType == VO_SOURCE2_TT_VIDEO )
		uTrack = m_uVideoTrack;
	else if( nOutPutType == VO_SOURCE2_TT_AUDIO )
		uTrack = m_uAudioTrack;
	else if( nOutPutType == VO_SOURCE2_TT_SUBTITLE )
		uTrack = m_uSubtitleTrack;
	
	else
		return VO_RET_SOURCE2_OUTPUTNOTFOUND;

    CBaseReader* pReader = (CBaseReader*)m_Handle;
    CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrack);
    if(!pTrack || !pTrack->IsInUsed())
    {
	    // VOLOGI("error track index: %d!!", uTrack);

	    return VO_RET_SOURCE2_ERRORDATA;
    }

    if(pTrack->IsEndOfStream())
    {
	    VOLOGI("track is end!!");

	    return VO_RET_SOURCE2_END;
    }

	VO_U32 rc = pTrack->GetSample(pSample);

    if(VO_ERR_SOURCE_OK != rc)
    {
	    if(VO_ERR_SOURCE_END == rc)
		    pTrack->SetEndOfStream(VO_TRUE);

	    VOLOGI("get sample error: 0x%08X!!", rc);

		if(VO_ERR_SOURCE_END == rc)
			return VO_RET_SOURCE2_END;
		else
			return rc;
    }

    VO_TRACKTYPE eTrackType = pTrack->GetType();
    if(VOTT_VIDEO == eTrackType)
	    pReader->CheckVideo(pSample);
    else if(VOTT_AUDIO == eTrackType)
	    pReader->CheckAudio(pSample);
#endif	//_AUDIO_READER

#ifndef _AUDIO_READER
	pTrack->SetLastTimeStamp(pSample->Time);
#endif
    return VO_RET_SOURCE2_OK;
}

VO_U32 voSource2ParserWrapper::SelectTrack(VO_U32 nTrack)
{
	VOLOGE("nTrack==%d",nTrack);
/*
	CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
	if (!pTrack)
	{
		return VO_RET_SOURCE2_FAIL;
	}
	VO_U32 nOtherTrack = 0;
	if (pTrack->GetType() == VOTT_AUDIO)
	{
		nOtherTrack = m_uVideoTrack;
	}
	else if (pTrack->GetType() == VOTT_VIDEO)
	{
		nOtherTrack = m_uAudioTrack;
	}
	else if (pTrack->GetType() == VOTT_TEXT)
	{
		nOtherTrack = m_uSubtitleTrack;
	}
	else
		return VO_RET_SOURCE2_FORMATUNSUPPORT;

	CBaseTrack* pTmp = pReader->GetTrackByIndex(nOtherTrack);
	if (pTmp)
	{
		pTrack = pTmp;
	}
*/
	if (nTrack == m_uVideoTrack || nTrack == m_uAudioTrack || nTrack == m_uSubtitleTrack)
	{
		return VO_RET_SOURCE2_OK;
	}

	CBaseReader* pReader = (CBaseReader*)m_Handle;
	if (!pReader)
	{
		return VO_RET_SOURCE2_FAIL;
	}

	CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
	if (!pTrack)
	{
		return VO_RET_SOURCE2_FAIL;
	}

	VO_SOURCE_SELTRACK stTrack;
	stTrack.nIndex = nTrack;
	stTrack.bInUsed = VO_TRUE;
	stTrack.llTs = 0; //pTrack->GetLastTimeStamp();
	
	if(VO_ERR_SOURCE_OK == pReader->SetParameter(VO_PID_SOURCE_SELTRACK,&stTrack))
	{
		if (pTrack->GetType() == VOTT_AUDIO)
		{
			m_pAudioTrackInfo->uSelInfo = VO_SOURCE2_SELECT_SELECTABLE;///<make last selected track to selectable

			m_uAudioTrack = nTrack;
			m_pAudioTrackInfo = m_pStreamInfo->ppTrackInfo[nTrack];
			m_pAudioTrackInfo->uSelInfo = VO_SOURCE2_SELECT_SELECTED;///<make current track to selected
			m_bAudioHeadDataSend = VO_FALSE;
		}
		else if((pTrack->GetType() == VOTT_VIDEO))
		{
			m_pVideoTrackInfo->uSelInfo = VO_SOURCE2_SELECT_SELECTABLE;///<make last selected track to selectable
			m_uVideoTrack = nTrack;
			m_pVideoTrackInfo = m_pStreamInfo->ppTrackInfo[nTrack];
			m_pVideoTrackInfo->uSelInfo = VO_SOURCE2_SELECT_SELECTED; ///<make current track to selected

			m_bVideoHeadDataSend = VO_FALSE;
		}
		else if((pTrack->GetType() == VOTT_TEXT))
		{
			m_pSubtitleTrackInfo->uSelInfo = VO_SOURCE2_SELECT_SELECTABLE;///<make last selected track to selectable
			m_uSubtitleTrack = nTrack;
			m_pSubtitleTrackInfo = m_pStreamInfo->ppTrackInfo[nTrack];
			m_pSubtitleTrackInfo->uSelInfo = VO_SOURCE2_SELECT_SELECTED; ///<make current track to selected

			m_bSubtitleHeadDataSend = VO_FALSE;
		}

	}
	
	return VO_RET_SOURCE2_OK;
}


VO_U32 voSource2ParserWrapper::GetParam(VO_U32 nParamID, VO_PTR pParam)
{
	if(!m_Handle)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	if(VO_RET_SOURCE2_OK == voSourceBaseWrapper::GetParam(nParamID,pParam)){
		return VO_RET_SOURCE2_OK;
	}

	//Convert the paramID from Souce2 to Source, added by Aiven 2013-06-05
	switch(nParamID)
	{
	case VO_PID_SOURCE2_GETTHUMBNAIL:
		nParamID = VO_PID_SOURCE_GETTHUMBNAIL;
		break;
	case VO_PID_SOURCE2_METADATA_TITLE:
		nParamID = VO_PID_METADATA_TITLE;
		break;
	case VO_PID_SOURCE2_METADATA_ARTIST:
		nParamID = VO_PID_METADATA_ARTIST;
		break;
	case VO_PID_SOURCE2_METADATA_ALBUM:
		nParamID = VO_PID_METADATA_ALBUM;
		break;
	case VO_PID_SOURCE2_METADATA_GENRE:
		nParamID = VO_PID_METADATA_GENRE;
		break;
	case VO_PID_SOURCE2_METADATA_COMPOSER:
		nParamID = VO_PID_METADATA_COMPOSER;
		break;
	case VO_PID_SOURCE2_METADATA_TRACK:
		nParamID = VO_PID_METADATA_TRACK;
		break;
	case VO_PID_SOURCE2_METADATA_FRONTCOVER:
		nParamID = VO_PID_METADATA_FRONTCOVER;
		break;
	case VO_PID_SOURCE2_METADATA_GRACENOTETAGID:
		nParamID = VO_PID_METADATA_GRACENOTETAGID;
		break;
	case VO_PID_SOURCE2_METADATA_COMMENT:
		nParamID = VO_PID_METADATA_COMMENT;
		break;
	case VO_PID_SOURCE2_METADATA_DISK:
		nParamID = VO_PID_METADATA_DISK;
		break;
	case VO_PID_SOURCE2_METADATA_PUBLISHER:
		nParamID = VO_PID_METADATA_PUBLISHER;
		break;
	case VO_PID_SOURCE2_METADATA_ISRC:
		nParamID = VO_PID_METADATA_ISRC;
		break;
	case VO_PID_SOURCE2_METADATA_YEAR:
		nParamID = VO_PID_METADATA_YEAR;
		break;
	case VO_PID_SOURCE2_METADATA_BAND:
		nParamID = VO_PID_METADATA_BAND;
		break;
	case VO_PID_SOURCE2_METADATA_CONDUCTOR:
		nParamID = VO_PID_METADATA_CONDUCTOR;
		break;
	case VO_PID_SOURCE2_METADATA_REMIXER:
		nParamID = VO_PID_METADATA_REMIXER;
		break;
	case VO_PID_SOURCE2_METADATA_LYRICIST:
		nParamID = VO_PID_METADATA_LYRICIST;
		break;
	case VO_PID_SOURCE2_METADATA_RADIOSTATION:
		nParamID = VO_PID_METADATA_RADIOSTATION;
		break;
	case VO_PID_SOURCE2_METADATA_ORIGRELEASEYEAR:
		nParamID = VO_PID_METADATA_ORIGRELEASEYEAR;
		break;
	case VO_PID_SOURCE2_METADATA_OWNER:
		nParamID = VO_PID_METADATA_OWNER;
		break;
	case VO_PID_SOURCE2_METADATA_ENCODER:
		nParamID = VO_PID_METADATA_ENCODER;
		break;
	case VO_PID_SOURCE2_METADATA_ENCODESETTINGS:
		nParamID = VO_PID_METADATA_ENCODESETTINGS;
		break;
	case VO_PID_SOURCE2_METADATA_COPYRIGHT:
		nParamID = VO_PID_METADATA_COPYRIGHT;
		break;
	case VO_PID_SOURCE2_METADATA_ORIGARTIST:
		nParamID = VO_PID_METADATA_ORIGARTIST;
		break;
	case VO_PID_SOURCE2_METADATA_ORIGALBUM:
		nParamID = VO_PID_METADATA_ORIGALBUM;
		break;
	case VO_PID_SOURCE2_METADATA_COMPILATION:
		nParamID = VO_PID_METADATA_COMPILATION;
		break;
	case VO_PID_SOURCE2_LGE_VIDEO_CONTENT:
		nParamID = VO_PID_SOURCE_LGE_VIDEO_CONTENT;		
		break;
		
	default:
		break;
	}


	CBaseReader* pReader = (CBaseReader*)m_Handle;

	VOLOGI("+GetParameter. PID : 0x%08x", nParamID);
	VO_U32  nRc = pReader->GetParameter(nParamID, pParam);
	VOLOGI("-GetParameter. Res: 0x%08x", nRc);

	//convert the sampledata if the fourcc is AVC
	if((VO_PID_SOURCE_GETTHUMBNAIL == nParamID) && (VO_ERR_SOURCE_OK == nRc) && 
		pParam && (VO_SOURCE2_THUMBNAILMODE_DATA == ((VO_SOURCE2_THUMBNAILINFO*)pParam)->uFlag)){
		VO_SOURCE2_THUMBNAILINFO* info = (VO_SOURCE2_THUMBNAILINFO*)pParam;
		
		if( m_pVideoTrackInfo->uCodec == VO_VIDEO_CodingH264)
		{
			VO_U32 nForCC = *(VO_U32*)m_pVideoTrackInfo->strFourCC;
			if (FOURCC_AVC1 == nForCC)
			{
				if( VO_TRUE == ConvertData (info->pSampleData, info->uSampleSize ))
				{
					if (m_pVideoData != NULL)
					{
						info->pSampleData = m_pVideoData;
						info->uSampleSize = m_uVideoSize;
					}
				}
				else
					return VO_RET_SOURCE2_NEEDRETRY;
			}	
		}
		
	}

	if( nRc == VO_ERR_SOURCE_OK )
		return VO_RET_SOURCE2_OK;
	else
		return VO_RET_SOURCE2_FAIL;
}

VO_U32 voSource2ParserWrapper::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
	if(VO_RET_SOURCE2_OK == voSourceBaseWrapper::SetParam(nParamID,pParam)){
		return VO_RET_SOURCE2_OK;
	}

	switch( nParamID )
	{
	case VO_PID_SOURCE2_IOPOINTER:
		{
			if (pParam)
			{
				VOLOGI("got io");
				if (m_SrcIO.oprIO2)
				{
					delete (VO_SOURCE2_IO_API*)m_SrcIO.oprIO2;
					m_SrcIO.oprIO2 = NULL;
				}
				VOLOGI("Enter voSource2ParserWrapper::SetParam and ID = VO_PID_SOURCE2_IOPOINTER");
				m_SrcIO.oprIO2 = new VO_SOURCE2_IO_API;
				memcpy(m_SrcIO.oprIO2,pParam,sizeof(VO_SOURCE2_IO_API));
				VOLOGI("Begin to Inital m_FileOpr and value: %p",m_FileOpr);
				CSourceIOSwitchFillPointer(&m_FileOpr);
				VOLOGI("After Initaling m_FileOpr and value: %p",m_FileOpr);
				return VO_RET_SOURCE2_OK;
			}
		}
		break;
	case VO_PID_SOURCE2_DRMIO:
		{
			m_SrcIO.bDrm = *(VO_BOOL*)pParam;
		}
		break;
	}

	if( !m_Handle && nParamID != VO_PID_SOURCE2_DRMCALLBACK )
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	else if( !m_Handle && nParamID == VO_PID_SOURCE2_DRMCALLBACK )
		return VO_RET_SOURCE2_OK;

	CBaseReader* pReader = (CBaseReader*)m_Handle;
	if( pReader )
		return pReader->SetParameter(nParamID, pParam);
	else
		return VO_RET_SOURCE2_EMPTYPOINTOR;
}

VO_U32 voSource2ParserWrapper::SetTrackPos( VO_U32 nTrack , VO_U64 * pPos )
{
	if(!m_Handle)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VOLOGI("%d track want to setpos %d!!", nTrack, VO_S32(*pPos));
#ifndef _AUDIO_READER
	CBaseReader* pReader = (CBaseReader*)m_Handle;
	CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
	if(!pTrack || !pTrack->IsInUsed())
	{
		VOLOGI("error track index: %d!!", nTrack);

		return VO_RET_SOURCE2_OUTPUTNOTFOUND;
	}

	if(VO_SOURCE_PM_PLAY == pTrack->GetPlayMode())
	{
		if(0xFFFFFFFF== pReader->GetSeekTrack())	//not initialize!!
		{
			VOLOGI("set seek track: %d!!", nTrack);

			pReader->SetSeekTrack(nTrack);
		}

		if(pReader->GetSeekTrack() == nTrack)	//the track will activate file set position
		{
			VOLOGI("%d track activate moveto %d!!", nTrack, VO_S32(*pPos));

			pReader->MoveTo((VO_S64)*pPos);
		}
	}

	pTrack->SetEndOfStream(VO_FALSE);

	VO_U32 rc = pTrack->SetPos((VO_S64*)pPos);
	if(VO_ERR_SOURCE_END == rc)
		pTrack->SetEndOfStream(VO_TRUE);
#else	//_AUDIO_READER
	CBaseAudioReader* pReader = (CBaseAudioReader*)m_Handle;
	pReader->SetEndOfStream(VO_FALSE);

	VO_U32 rc = pReader->SetPos((VO_S64*)pPos);
	if(VO_ERR_SOURCE_END == rc)
		pReader->SetEndOfStream(VO_TRUE);
#endif	//_AUDIO_READER
	VOLOGI("setpos %d return 0x%08X!!", VO_S32(*pPos), rc);

	if( VO_ERR_SOURCE_END == rc )
		return VO_RET_SOURCE2_END;
	else if( VO_ERR_SOURCE_OK == rc )
		return VO_RET_SOURCE2_OK;
	else
		return VO_RET_SOURCE2_FAIL;
}

VO_U32 voSource2ParserWrapper::OnGetDuration(VO_U64 * pDuration)
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader || !pDuration )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_SOURCE_INFO SourceInfo;
	memset( &SourceInfo , 0 , sizeof( VO_SOURCE_INFO ) );
	pReader->GetInfo( &SourceInfo );

	*pDuration = SourceInfo.Duration;

	return VO_RET_SOURCE2_OK;
}

VO_U32 voSource2ParserWrapper::OnGetFileLength(VO_U64 * pLength)
{
	return OnGetDuration(pLength);
}


VO_U32 voSource2ParserWrapper::OnGetTrackCnt(VO_U32 * pTraCnt)
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_SOURCE_INFO SourceInfo;
	memset( &SourceInfo , 0 , sizeof( VO_SOURCE_INFO ) );
	pReader->GetInfo( &SourceInfo );

	*pTraCnt = SourceInfo.Tracks;

	return VO_RET_SOURCE2_OK;
}

VO_U32 voSource2ParserWrapper::OnGetTrackAvailable(VO_U32 uTrackIndex , VO_BOOL * beAlai)
{
	* beAlai = VO_TRUE;
#ifndef _AUDIO_READER

	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrackIndex);
	if (!pTrack)
	{
		* beAlai = VO_FALSE; 
	}
#endif
	return VO_RET_SOURCE2_OK;
}

VO_U32 voSource2ParserWrapper::OnGetTrackInfo(VO_U32 uTrackIndex , VO_SOURCE_TRACKINFO * pTrackInfo)
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader || !pTrackInfo)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
#ifndef _AUDIO_READER
	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrackIndex);
	ret = pTrack->GetInfo( pTrackInfo );
#else
	ret = ((CBaseAudioReader*)pReader)->GetTrackInfo(pTrackInfo);
#endif
	return ret;
}
VO_U32 voSource2ParserWrapper::OnGetTrackFourCC(VO_U32 uTrackIndex , VO_U32 * pFourCC)
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader || !pFourCC)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
#ifndef _AUDIO_READER
	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrackIndex);
	ret = pTrack->GetParameter(VO_PID_SOURCE_CODECCC, pFourCC);
#else
	ret = ((CBaseAudioReader*)pReader)->GetTrackParameter(VO_PID_SOURCE_CODECCC, pFourCC);
#endif
	return ret;
}
VO_U32 voSource2ParserWrapper::OnGetTrackMaxSampleSize(VO_U32 uTrackIndex , VO_U32 * pMaxSampleSize)
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader || !pMaxSampleSize)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
#ifndef _AUDIO_READER
	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrackIndex);
	ret = pTrack->GetParameter(VO_PID_SOURCE_MAXSAMPLESIZE, pMaxSampleSize);
#else
	ret = ((CBaseAudioReader*)pReader)->GetTrackParameter(VO_PID_SOURCE_MAXSAMPLESIZE, pMaxSampleSize);
#endif
	return ret;
}

VO_U32 voSource2ParserWrapper::OnGetAudioFormat(VO_U32 uTrackIndex , VO_AUDIO_FORMAT * pAudioFormat)
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader || !pAudioFormat)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
#ifndef _AUDIO_READER
	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrackIndex);
	ret = pTrack->GetParameter(VO_PID_AUDIO_FORMAT,pAudioFormat);
#else
	ret = ((CBaseAudioReader*)pReader)->GetTrackParameter(VO_PID_AUDIO_FORMAT, pAudioFormat);
#endif
	return ret;
}

VO_U32 voSource2ParserWrapper::OnGetVideoFormat(VO_U32 uTrackIndex , VO_VIDEO_FORMAT * pVideoFormat)
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader || !pVideoFormat)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
#ifndef _AUDIO_READER
	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrackIndex);
	ret = pTrack->GetParameter(VO_PID_VIDEO_FORMAT,pVideoFormat);
#endif
	return ret;
}
/*
VO_U32 voSource2ParserWrapper::OnGetSubtitleFormat(VO_U32 uTrackIndex , VO_VIDEO_FORMAT * pVideoFormat)
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader || !pVideoFormat)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
#ifndef _AUDIO_READER
	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrackIndex);
	ret = pTrack->GetParameter(VO_PID_VIDEO_FORMAT,pVideoFormat);
#endif
	return ret;
}
*/


VO_U32 voSource2ParserWrapper::OnGetWaveFormatEx(VO_U32 uTrackIndex , VO_PBYTE * pExData)
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader || !pExData)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
#ifndef _AUDIO_READER
	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrackIndex);
	ret = pTrack->GetParameter(VO_PID_SOURCE_WAVEFORMATEX, pExData);
#else
	ret = ((CBaseAudioReader*)pReader)->GetTrackParameter(VO_PID_SOURCE_WAVEFORMATEX, pExData);
#endif
	return ret;
}
VO_U32 voSource2ParserWrapper::OnGetVideoBitMapInfoHead(VO_U32 uTrackIndex , VO_PBYTE * pBitMap)
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader || !pBitMap)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
#ifndef _AUDIO_READER
	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrackIndex);
	ret = pTrack->GetParameter(VO_PID_SOURCE_BITMAPINFOHEADER,pBitMap);
#endif
	return ret;
}
VO_U32 voSource2ParserWrapper::OnGetVideoUpSideDown(VO_U32 uTrackIndex , VO_BOOL * pUpSideDown)
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader || !pUpSideDown)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
#ifndef _AUDIO_READER
	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrackIndex);
	ret = pTrack->GetParameter(VO_PID_VIDEO_UPSIDEDOWN,pUpSideDown);
#endif
	return ret;
}
VO_U32 voSource2ParserWrapper::OnGetLanguage(VO_U32 uTrackIndex , VO_CHAR ** pLanguage)
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader || !pLanguage)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = VO_RET_SOURCE2_OK;
#ifndef _AUDIO_READER
	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrackIndex);
	ret = pTrack->GetParameter(VO_PID_SOURCE_TRACK_LANGUAGE,pLanguage);
#endif
	return ret;
}

VO_U32 voSource2ParserWrapper::GetBitrate()
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;
	VO_U32 bitrate = 0;
	VO_S64 FileSize = 0;
	VO_U32 duration = 0;
	
	if( !pReader)
		return 0;
	
	duration = pReader->GetDuration();
	pReader->GetParameter(VO_PID_SOURCE_FILESIZE, (VO_PTR)&FileSize);

	if(duration){
		bitrate = ((VO_U32)FileSize)/duration*8000;
	}


	return bitrate;
}

