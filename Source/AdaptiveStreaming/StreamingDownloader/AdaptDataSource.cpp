/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		AdaptDataSource.cpp

	Contains:	AdaptDataSource class file

	Written by:	Aiven

	Change History (most recent first):
	2013-10-15		Aiven			Create file

*******************************************************************************/
#include "AdaptDataSource.h"
#include "SDownloaderLog.h"
#include "voProgramInfo.h"
#include "voToolUtility.h"
#include "StreamingDownloadStruct.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


const char* const VideoCodec[] = { "None", "MPEG2", "H263", "S263", "MPEG4", "H264", "WMV", "RealVideo", "MJPEG", "DIVX", "VP6", "VP8", "VP7", "VC1", "HEVC"};
const char* const AudioCodec[] = { "None", "PCM", "ADPCM", "AMRNB", "AMRWB", "AMRWBP", "QCELP13", "EVRC", "AAC", "AC3", "FLAC", "MP1", "MP3", "OGG",
							 "WMA", "RealAudio", "MIDI", "DRA", "G729", "EAC3", "APE", "ALAC", "DTS"};
const char* const Language[] = {	"Unknow", "English", "Simplified CHINESE", "Traditional Chinese", "Korean", "Japanese", "Spanish", "German"};

AdaptDataSource::AdaptDataSource()
:m_nAudioCount(0)
,m_nVideoCount(0)
,m_nSubtitleCount(0)
,m_nAudioSelected(-1)
,m_nVideoSelected(-1)
,m_nSubtitleSelected(-1)
,m_nCurPlayingAudioTrack(-1)
,m_nCurPlayingVideoTrack(-1)
,m_nCurPlayingSubtitleTrack(-1)
,m_nCurSelectedAudioTrack(-1)
,m_nCurSelectedVideoTrack(-1)
,m_nCurSelectedSubtitleTrack(-1)
,m_pProgramInfo(NULL)
{

	memset(m_pAudioTrackIndex, 0x0, sizeof(m_pAudioTrackIndex));
	memset(m_pVideoTrackIndex, 0x0, sizeof(m_pVideoTrackIndex));
	memset(m_pSubtitleTrackIndex, 0x0, sizeof(m_pSubtitleTrackIndex));

	memset(m_pAudioTrackLan, 0x0, sizeof(m_pAudioTrackLan));
	memset(m_pSubtitleTrackLan, 0x0, sizeof(m_pSubtitleTrackLan));

	memset(&m_szAudioProtery, 0x0, sizeof(m_szAudioProtery));
	memset(&m_szVideoProtery, 0x0, sizeof(m_szVideoProtery));
	memset(&m_szSubtitleProtery, 0x0, sizeof(m_szSubtitleProtery));

}

AdaptDataSource::~AdaptDataSource()
{
	ReleaseProgramInfoOP_T(m_pProgramInfo);
	m_pProgramInfo=NULL;

}


VO_U32 AdaptDataSource::GetVideoCount()
{
	VO_U32 count = 0;

	count = GetMediaCount(VO_SOURCE2_TT_VIDEO);
	SD_LOGI("GetVideoCount---count=%lu", count);

	return count;
}

VO_U32 AdaptDataSource::GetAudioCount ()
{
	VO_U32 count = 0;

	count = GetMediaCount(VO_SOURCE2_TT_AUDIO);
	SD_LOGI("GetAudioCount---count=%lu", count);

	return count;
}
    

VO_U32 AdaptDataSource::GetSubtitleCount()
{
	VO_U32 count = 0;

	count = GetMediaCount(VO_SOURCE2_TT_SUBTITLE);
	SD_LOGI("GetMediaCount---count=%lu", count);

	return count;
}
    

VO_U32 AdaptDataSource::SelectVideo(VO_S32 nIndex)
{

	return SelectMediaIndex(VO_SOURCE2_TT_VIDEO, nIndex);
}
    
VO_U32 AdaptDataSource::SelectAudio (VO_S32 nIndex)
{

	return SelectMediaIndex(VO_SOURCE2_TT_AUDIO, nIndex);
}
    
VO_U32 AdaptDataSource::SelectSubtitle(VO_S32 nIndex)
{

	return SelectMediaIndex(VO_SOURCE2_TT_SUBTITLE, nIndex);
}
   
VO_BOOL AdaptDataSource::IsVideoAvailable (VO_S32 nIndex)
{
	VO_BOOL bResult = VO_FALSE;

	if(IsMediaIndexAvailable(VO_SOURCE2_TT_VIDEO, nIndex))
		bResult = VO_TRUE;

	SD_LOGI("IsVideoAvailable---bResult=%d", bResult);
	
	return bResult;
}   

VO_BOOL AdaptDataSource::IsAudioAvailable (VO_S32 nIndex)
{
	VO_BOOL bResult = VO_FALSE;

	if(IsMediaIndexAvailable(VO_SOURCE2_TT_AUDIO, nIndex))
		bResult = VO_TRUE;
	
	SD_LOGI("IsAudioAvailable---bResult=%d", bResult);

	return bResult;
}

VO_BOOL AdaptDataSource::IsSubtitleAvailable (VO_S32 nIndex)
{
	VO_BOOL bResult = VO_FALSE;

	if(IsMediaIndexAvailable(VO_SOURCE2_TT_SUBTITLE, nIndex))
		bResult = VO_TRUE;

	SD_LOGI("IsSubtitleAvailable---IsSubtitleAvailable=%d", bResult);

	return bResult;
}
    
VO_U32 AdaptDataSource::CommitSelection ()
{

	return CommetSelection(1);
}  

VO_U32 AdaptDataSource::ClearSelection ()
{

	return CommetSelection(0);
}

VO_U32 AdaptDataSource::GetVideoProperty (VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{

	return GetMediaProperty(VO_SOURCE2_TT_VIDEO, nIndex, ppProperty);
}

VO_U32 AdaptDataSource::GetAudioProperty (VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{

	return GetMediaProperty(VO_SOURCE2_TT_AUDIO, nIndex, ppProperty);
}
    
VO_U32 AdaptDataSource::GetSubtitleProperty (VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{

	return GetMediaProperty(VO_SOURCE2_TT_SUBTITLE, nIndex, ppProperty);
}    

VO_U32 AdaptDataSource::GetCurrTrackSelection (VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{

	return GetCurMediaTrack(pCurrIndex);
}

VO_U32 AdaptDataSource::GetCurrPlayingTrackIndex (VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{

	return GetCurPlayingMediaTrack(pCurrIndex);
}

VO_U32 AdaptDataSource::GetCurrSelectedTrackIndex (VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{

	return GetCurSelectedMediaTrack(pCurrIndex);
}

VO_U32 AdaptDataSource::SetProgramInfo (_PROGRAM_INFO* 	pProgramInfo)
{
	VO_U32	nRC = VO_RET_SOURCE2_OK;
	
	ReleaseProgramInfoOP_T(m_pProgramInfo);
	m_pProgramInfo=NULL;
	CopyProgramInfoOP_EX(pProgramInfo, &m_pProgramInfo);
	SetOutSideID(pProgramInfo, m_pProgramInfo);
	PrintProgramInfo(m_pProgramInfo);

	return nRC;
}

VO_U32 AdaptDataSource::GetProgramInfo (VO_SOURCE2_PROGRAM_INFO**	ppProgramInfo)
{
	VO_U32	nRC = VO_RET_SOURCE2_OK;

	*ppProgramInfo = m_pProgramInfo;

	return nRC;
}


VO_U32 AdaptDataSource::GetMediaCount(VO_U32 ssType)
{
	VO_U32 nRC = UpdateTrackInfo();

	if(nRC) 
		return nRC;

	if(ssType == VOOSMP_SS_AUDIO)
	{
		return m_nAudioCount;
	}
	else if(ssType == VOOSMP_SS_VIDEO)
	{
		return m_nVideoCount;
	}
	else if(ssType == VOOSMP_SS_SUBTITLE)
	{
		return m_nSubtitleCount;
	}
	else
	{
		return 0;
	}
}

VO_U32 AdaptDataSource::SelectMediaIndex(VO_U32 ssType, VO_S32 nIndex)
{
	SD_LOGI("@@@### ssType = %d, nIndex = %d, ACount: %d, VCount: %d, SCount: %d", ssType, nIndex, m_nAudioCount, m_nVideoCount, m_nSubtitleCount);

	VO_U32 nRC = VO_RET_SOURCE2_FAIL;


	if(ssType == VOOSMP_SS_AUDIO)
	{
		if ( nIndex >= (VO_S32)m_nAudioCount ) return nRC;

		m_nAudioSelected = nIndex;
		m_nCurSelectedAudioTrack = nIndex;
	}
	else if(ssType == VOOSMP_SS_VIDEO)
	{
		if ( nIndex >= (VO_S32)m_nVideoCount ) return nRC;

		m_nVideoSelected = nIndex;
		m_nCurSelectedVideoTrack = nIndex;
	}
	else if(ssType == VOOSMP_SS_SUBTITLE)
	{
		if ( nIndex >= (VO_S32)m_nSubtitleCount ) return nRC;

		m_nSubtitleSelected = nIndex;
		m_nCurSelectedSubtitleTrack = nIndex;
	}
	else
	{
		return VO_RET_SOURCE2_NOIMPLEMENT;
	}

	SD_LOGI("@@@### Select OK ssType = %d, nIndex = %d", ssType, nIndex);

	return VO_RET_SOURCE2_OK;
}

VO_U32 AdaptDataSource::IsMediaIndexAvailable(VO_U32 ssType, VO_S32 nIndex)
{
	if(ssType == VOOSMP_SS_SUBTITLE)
	{
		return IsSubtitleIndexAvaialbe( nIndex );
	}
	else if(ssType == VOOSMP_SS_AUDIO)
	{
		return IsAudioIndexAvaialbe ( nIndex );
	}
	else if(ssType == VOOSMP_SS_VIDEO)
	{
		return IsVideoIndexAvaialbe ( nIndex );
	}
	
	return 0;
}

VO_U32 AdaptDataSource::GetMediaProperty(VO_U32 ssType, VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY **pProperty)
{
	if(nIndex < 0)
		return VO_RET_SOURCE2_FAIL;

	VO_U32 nRC = UpdateTrackInfo();
	if(nRC) return nRC;

	VO_S32 nTrackIndex = -1;

	if(ssType == VOOSMP_SS_AUDIO)
	{
		nTrackIndex = m_pAudioTrackIndex[nIndex];
		ClearPropertyItem(&m_szAudioProtery);
	}
	else if(ssType == VOOSMP_SS_VIDEO)
	{
		nTrackIndex = m_pVideoTrackIndex[nIndex];
		ClearPropertyItem(&m_szVideoProtery);
	}
	else if(ssType == VOOSMP_SS_SUBTITLE)
	{
		nTrackIndex = m_pSubtitleTrackIndex[nIndex];

		ClearPropertyItem(&m_szSubtitleProtery);
	}
	else
	{
		return VO_RET_SOURCE2_FAIL;
	}


	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;	
	
	VO_U32 i, j;

	if(m_pProgramInfo == NULL || m_pProgramInfo->ppStreamInfo == NULL)
		return VO_RET_SOURCE2_FAIL;

	for(i = 0; i < m_pProgramInfo->uStreamCount; i++)
	{
		pStreamInfo = m_pProgramInfo->ppStreamInfo[i];

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
			continue;

		if(ssType == VOOSMP_SS_VIDEO)
		{
			if(nTrackIndex != (VO_U32)pStreamInfo->uStreamID)
				continue;
		}

		for(j = 0; j < pStreamInfo->uTrackCount; j++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[j];

			if(pTrackInfo == NULL)
				continue;

			if(ssType == VOOSMP_SS_VIDEO)
			{
				if(pTrackInfo->uTrackType == VOOSMP_SS_VIDEO || pTrackInfo->uTrackType == VOOSMP_SS_VIDEO_GROUP)
				{
					m_szVideoProtery.nPropertyCount = 5;
					m_szVideoProtery.ppItemProperties = new VOOSMP_SRC_TRACK_ITEM_PROPERTY*[5];
					if(m_szVideoProtery.ppItemProperties == NULL)
						return VO_RET_SOURCE2_FAIL;

					m_szVideoProtery.ppItemProperties[0] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[0] == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szVideoProtery.ppItemProperties[0], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[0]->szKey, "description");

					m_szVideoProtery.ppItemProperties[0]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[0]->pszProperty == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szVideoProtery.ppItemProperties[0]->pszProperty, 0, sizeof(char)*32);
					if(strlen(pTrackInfo->sVideoInfo.strVideoDesc) > 0)
					{
						strcpy(m_szVideoProtery.ppItemProperties[0]->pszProperty, pTrackInfo->sVideoInfo.strVideoDesc);

						SD_LOGI("video description: %s, strlen %d, pTrackInfo->sVideoInfo.strVideoDesc[0] %d", m_szVideoProtery.ppItemProperties[0]->pszProperty, strlen(pTrackInfo->sVideoInfo.strVideoDesc), 
							pTrackInfo->sVideoInfo.strVideoDesc[0]);
					}
					else
						sprintf(m_szVideoProtery.ppItemProperties[0]->pszProperty, "Video %d", nIndex);


					m_szVideoProtery.ppItemProperties[1] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[1] == NULL)
						return VO_RET_SOURCE2_FAIL;
						
					memset(m_szVideoProtery.ppItemProperties[1], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[1]->szKey, "codec");

					m_szVideoProtery.ppItemProperties[1]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[1]->pszProperty == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szVideoProtery.ppItemProperties[1]->pszProperty, 0, sizeof(char)*32);
					strcpy(m_szVideoProtery.ppItemProperties[1]->pszProperty, VideoCodec[pTrackInfo->uCodec]);

					m_szVideoProtery.ppItemProperties[2] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[2] == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szVideoProtery.ppItemProperties[2], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[2]->szKey, "bitrate");

					m_szVideoProtery.ppItemProperties[2]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[2]->pszProperty == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szVideoProtery.ppItemProperties[2]->pszProperty, 0, sizeof(char)*32);

					if(pTrackInfo->uBitrate)
						sprintf(m_szVideoProtery.ppItemProperties[2]->pszProperty, "%d bps", (VO_U32)pTrackInfo->uBitrate);
					else
						sprintf(m_szVideoProtery.ppItemProperties[2]->pszProperty, "%d bps", (VO_U32)pStreamInfo->uBitrate);


					m_szVideoProtery.ppItemProperties[3] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[3] == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szVideoProtery.ppItemProperties[3], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[3]->szKey, "width");

					m_szVideoProtery.ppItemProperties[3]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[3]->pszProperty == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szVideoProtery.ppItemProperties[3]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szVideoProtery.ppItemProperties[3]->pszProperty, "%d", (VO_U32)pTrackInfo->sVideoInfo.sFormat.Width);

					m_szVideoProtery.ppItemProperties[4] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szVideoProtery.ppItemProperties[4] == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szVideoProtery.ppItemProperties[4], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szVideoProtery.ppItemProperties[4]->szKey, "height");

					m_szVideoProtery.ppItemProperties[4]->pszProperty = new char[32];
					if(m_szVideoProtery.ppItemProperties[4]->pszProperty == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szVideoProtery.ppItemProperties[4]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szVideoProtery.ppItemProperties[4]->pszProperty, "%d", (VO_U32)pTrackInfo->sVideoInfo.sFormat.Height);

					*pProperty = &m_szVideoProtery;

					return VO_RET_SOURCE2_OK;
				}

				continue;
			}

			if((VO_U32)pTrackInfo->uTrackID == nTrackIndex)
			{
				if(pTrackInfo->uTrackType == VOOSMP_SS_AUDIO || pTrackInfo->uTrackType == VOOSMP_SS_AUDIO_GROUP)
				{
					m_szAudioProtery.nPropertyCount = 3;

					m_szAudioProtery.ppItemProperties = new VOOSMP_SRC_TRACK_ITEM_PROPERTY*[3];
					if(m_szAudioProtery.ppItemProperties == NULL)
						return VO_RET_SOURCE2_FAIL;

					m_szAudioProtery.ppItemProperties[0] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szAudioProtery.ppItemProperties[0] == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szAudioProtery.ppItemProperties[0], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szAudioProtery.ppItemProperties[0]->szKey, "description");

					m_szAudioProtery.ppItemProperties[0]->pszProperty = new char[32];
					if(m_szAudioProtery.ppItemProperties[0]->pszProperty == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szAudioProtery.ppItemProperties[0]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szAudioProtery.ppItemProperties[0]->pszProperty, "Audio %d", nIndex);


					m_szAudioProtery.ppItemProperties[1] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szAudioProtery.ppItemProperties[1] == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szAudioProtery.ppItemProperties[1], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szAudioProtery.ppItemProperties[1]->szKey, "language");

					m_szAudioProtery.ppItemProperties[1]->pszProperty = new char[32];
					if(m_szAudioProtery.ppItemProperties[1]->pszProperty == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szAudioProtery.ppItemProperties[1]->pszProperty, 0, sizeof(char)*32);

					if(strlen(pTrackInfo->sAudioInfo.chLanguage) > 0)
						strcpy(m_szAudioProtery.ppItemProperties[1]->pszProperty, (const char *)pTrackInfo->sAudioInfo.chLanguage);
					else
						strcpy(m_szAudioProtery.ppItemProperties[1]->pszProperty, "unknown");

					m_szAudioProtery.ppItemProperties[2] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szAudioProtery.ppItemProperties[2] == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szAudioProtery.ppItemProperties[2], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szAudioProtery.ppItemProperties[2]->szKey, "codec");

					m_szAudioProtery.ppItemProperties[2]->pszProperty = new char[32];
					if(m_szAudioProtery.ppItemProperties[2]->pszProperty == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szAudioProtery.ppItemProperties[2]->pszProperty, 0, sizeof(char)*32);
					strcpy(m_szAudioProtery.ppItemProperties[2]->pszProperty, AudioCodec[pTrackInfo->uCodec]);

					*pProperty = &m_szAudioProtery;

					return VO_RET_SOURCE2_OK;
				}
				else if(pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE || pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE_GROUP)
				{
					m_szSubtitleProtery.nPropertyCount = 3;

					m_szSubtitleProtery.ppItemProperties = new VOOSMP_SRC_TRACK_ITEM_PROPERTY*[3];
					if(m_szSubtitleProtery.ppItemProperties == NULL)
						return VO_RET_SOURCE2_FAIL;

					m_szSubtitleProtery.ppItemProperties[0] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szSubtitleProtery.ppItemProperties[0] == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szSubtitleProtery.ppItemProperties[0], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szSubtitleProtery.ppItemProperties[0]->szKey, "description");

					m_szSubtitleProtery.ppItemProperties[0]->pszProperty = new char[32];
					if(m_szSubtitleProtery.ppItemProperties[0]->pszProperty == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szSubtitleProtery.ppItemProperties[0]->pszProperty, 0, sizeof(char)*32);
					sprintf(m_szSubtitleProtery.ppItemProperties[0]->pszProperty, "SubTitle %d", nIndex);


					m_szSubtitleProtery.ppItemProperties[1] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szSubtitleProtery.ppItemProperties[1] == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szSubtitleProtery.ppItemProperties[1], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szSubtitleProtery.ppItemProperties[1]->szKey, "language");

					m_szSubtitleProtery.ppItemProperties[1]->pszProperty = new char[32];
					if(m_szSubtitleProtery.ppItemProperties[1]->pszProperty == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szSubtitleProtery.ppItemProperties[1]->pszProperty, 0, sizeof(char)*32);

					if(strlen(pTrackInfo->sSubtitleInfo.chLanguage) > 0)
						strcpy(m_szSubtitleProtery.ppItemProperties[1]->pszProperty, (const char *)pTrackInfo->sSubtitleInfo.chLanguage);
					else
						strcpy(m_szSubtitleProtery.ppItemProperties[1]->pszProperty, "unknown");

					m_szSubtitleProtery.ppItemProperties[2] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
					if(m_szSubtitleProtery.ppItemProperties[2] == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szSubtitleProtery.ppItemProperties[2], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

					strcpy(m_szSubtitleProtery.ppItemProperties[2]->szKey, "codec");

					m_szSubtitleProtery.ppItemProperties[2]->pszProperty = new char[32];
					if(m_szSubtitleProtery.ppItemProperties[2]->pszProperty == NULL)
						return VO_RET_SOURCE2_FAIL;
					memset(m_szSubtitleProtery.ppItemProperties[2]->pszProperty, 0, sizeof(char)*32);
					strcpy(m_szSubtitleProtery.ppItemProperties[2]->pszProperty, "unknown");

					*pProperty = &m_szSubtitleProtery;

					return VO_RET_SOURCE2_OK;
				}
			}

		}

		if(ssType == VOOSMP_SS_VIDEO)
		{
			if(nTrackIndex == (VO_U32)pStreamInfo->uStreamID)
			{
				m_szVideoProtery.nPropertyCount = 5;
				m_szVideoProtery.ppItemProperties = new VOOSMP_SRC_TRACK_ITEM_PROPERTY*[5];
				if(m_szVideoProtery.ppItemProperties == NULL)
					return VO_RET_SOURCE2_FAIL;

				m_szVideoProtery.ppItemProperties[0] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[0] == NULL)
					return VO_RET_SOURCE2_FAIL;
				memset(m_szVideoProtery.ppItemProperties[0], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[0]->szKey, "description");

				m_szVideoProtery.ppItemProperties[0]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[0]->pszProperty == NULL)
					return VO_RET_SOURCE2_FAIL;
				memset(m_szVideoProtery.ppItemProperties[0]->pszProperty, 0, sizeof(char)*32);
				strcpy(m_szVideoProtery.ppItemProperties[0]->pszProperty, "unknown");

				m_szVideoProtery.ppItemProperties[1] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[1] == NULL)
					return VO_RET_SOURCE2_FAIL;
				memset(m_szVideoProtery.ppItemProperties[1], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[1]->szKey, "codec");

				m_szVideoProtery.ppItemProperties[1]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[1]->pszProperty == NULL)
					return VO_RET_SOURCE2_FAIL;
				memset(m_szVideoProtery.ppItemProperties[1]->pszProperty, 0, sizeof(char)*32);
				strcpy(m_szVideoProtery.ppItemProperties[1]->pszProperty, VideoCodec[0]);

				m_szVideoProtery.ppItemProperties[2] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[2] == NULL)
					return VO_RET_SOURCE2_FAIL;
				memset(m_szVideoProtery.ppItemProperties[2], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[2]->szKey, "bitrate");

				m_szVideoProtery.ppItemProperties[2]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[2]->pszProperty == NULL)
					return VO_RET_SOURCE2_FAIL;
				memset(m_szVideoProtery.ppItemProperties[2]->pszProperty, 0, sizeof(char)*32);
				sprintf(m_szVideoProtery.ppItemProperties[2]->pszProperty, "%d bps", (VO_U32)pStreamInfo->uBitrate);

				m_szVideoProtery.ppItemProperties[3] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[3] == NULL)
					return VO_RET_SOURCE2_FAIL;
				memset(m_szVideoProtery.ppItemProperties[3], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[3]->szKey, "width");
				m_szVideoProtery.ppItemProperties[3]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[3]->pszProperty == NULL)
					return VO_RET_SOURCE2_FAIL;
				memset(m_szVideoProtery.ppItemProperties[3]->pszProperty, 0, sizeof(char)*32);
				strcpy(m_szVideoProtery.ppItemProperties[3]->pszProperty, "unknown");

				m_szVideoProtery.ppItemProperties[4] = new VOOSMP_SRC_TRACK_ITEM_PROPERTY();
				if(m_szVideoProtery.ppItemProperties[4] == NULL)
					return VO_RET_SOURCE2_FAIL;
				memset(m_szVideoProtery.ppItemProperties[4], 0, sizeof(VOOSMP_SRC_TRACK_ITEM_PROPERTY));

				strcpy(m_szVideoProtery.ppItemProperties[4]->szKey, "height");
				m_szVideoProtery.ppItemProperties[4]->pszProperty = new char[32];
				if(m_szVideoProtery.ppItemProperties[4]->pszProperty == NULL)
					return VO_RET_SOURCE2_FAIL;
				memset(m_szVideoProtery.ppItemProperties[4]->pszProperty, 0, sizeof(char)*32);
				strcpy(m_szVideoProtery.ppItemProperties[4]->pszProperty, "unknown");

				*pProperty = &m_szVideoProtery;

				return VO_RET_SOURCE2_OK;
			}
		}
	}

	return VO_RET_SOURCE2_FAIL;
}

VO_U32 AdaptDataSource::CommetSelection(VO_U32 bSelect)
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 AdaptDataSource::GetCurMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex)
{
	SD_LOGI("@@@### GetCurMediaTrack");

	if(pCurIndex == NULL)
		return VOOSMP_ERR_Pointer;

	VO_U32 nRC = VO_RET_SOURCE2_FAIL;

	nRC = UpdateTrackInfo();
	if(nRC) return nRC;

	pCurIndex->nCurrVideoIdx = -1;
	pCurIndex->nCurrAudioIdx = -1;
	pCurIndex->nCurrSubtitleIdx = -1;


	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;		
	VO_U32 i, j;
	VO_U32 n;

	if(m_pProgramInfo == NULL || m_pProgramInfo->ppStreamInfo == NULL)
		return VO_RET_SOURCE2_FAIL;

	for(i = 0; i < m_pProgramInfo->uStreamCount; i++)
	{
		pStreamInfo = m_pProgramInfo->ppStreamInfo[i];

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
			continue;

		if(!(pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED))
			continue;

		for(n = 0; n < m_nVideoCount; n++)
		{
			if((VO_U32)pStreamInfo->uStreamID == m_pVideoTrackIndex[n])
			{
				pCurIndex->nCurrVideoIdx = n;
				break;
			}
		}

		for(j = 0; j < pStreamInfo->uTrackCount; j++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[j];

			if(pTrackInfo == NULL)
				continue;

			if(pTrackInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)
			{
				if(pTrackInfo->uTrackType == VO_SOURCE2_TT_AUDIO || pTrackInfo->uTrackType == VO_SOURCE2_TT_AUDIOGROUP)
				{
					for(n = 0; n < m_nAudioCount; n++)
					{
						if((VO_U32)pTrackInfo->uTrackID == m_pAudioTrackIndex[n])
						{
							pCurIndex->nCurrAudioIdx = n;
							break;
						}
					}

					if(n == m_nAudioCount)
					{
						for(n = 0; n < m_nAudioCount; n++)
						{
							if(strcmp(pTrackInfo->sAudioInfo.chLanguage, m_pAudioTrackLan[n]) == 0)
							{
								pCurIndex->nCurrAudioIdx = n;
								break;
							}
						}
					}
				}
				else if(pTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLE || pTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLEGROUP)
				{
					for(n = 0; n < m_nSubtitleCount; n++)
					{
						if((VO_U32)pTrackInfo->uTrackID == m_pSubtitleTrackIndex[n])
						{
							pCurIndex->nCurrSubtitleIdx = n;
							break;
						}
					}

					if(n == m_nSubtitleCount)
					{
						for(n = 0; n < m_nAudioCount; n++)
						{
							if(strcmp(pTrackInfo->sSubtitleInfo.chLanguage, m_pSubtitleTrackLan[n]) == 0)
							{
								pCurIndex->nCurrSubtitleIdx = n;
								break;
							}
						}
					}
				}
			}
		}
	}

	SD_LOGI("@@@###  nCurrVideoIdx = %d, nCurrAudioIdx = %d. nCurrSubtitleIdx = %d", pCurIndex->nCurrVideoIdx, pCurIndex->nCurrAudioIdx, pCurIndex->nCurrSubtitleIdx);

	return VO_RET_SOURCE2_OK;
}

VO_U32 AdaptDataSource::GetCurPlayingMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex)
{
	SD_LOGI("@@@### GetCurPlayingMediaTrack");
	if (!pCurIndex)
		return VOOSMP_ERR_Pointer;

	VO_U32 nRC = UpdateTrackInfo();
	if(nRC) return nRC;

	pCurIndex->nCurrAudioIdx = m_nCurPlayingAudioTrack;
	pCurIndex->nCurrVideoIdx= m_nCurPlayingVideoTrack;
	pCurIndex->nCurrSubtitleIdx = m_nCurPlayingSubtitleTrack;
	
	SD_LOGI("@@@###  CurA = %ld, CurV = %ld. CurS = %ld", pCurIndex->nCurrAudioIdx, pCurIndex->nCurrVideoIdx, pCurIndex->nCurrSubtitleIdx);
	return VO_RET_SOURCE2_OK;
}

VO_U32 AdaptDataSource::GetCurSelectedMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex)
{
	SD_LOGI("@@@### GetCurSelectedMediaTrack");
	if (!pCurIndex)
		return VOOSMP_ERR_Pointer;

	pCurIndex->nCurrAudioIdx = m_nCurSelectedAudioTrack;
	pCurIndex->nCurrVideoIdx= m_nCurSelectedVideoTrack;
	pCurIndex->nCurrSubtitleIdx = m_nCurSelectedSubtitleTrack;
	SD_LOGI("@@@### 2 CurSelectedA = %ld, CurSelectedV = %ld. CurSelectedS = %ld", pCurIndex->nCurrAudioIdx, pCurIndex->nCurrVideoIdx, pCurIndex->nCurrSubtitleIdx);
		

	return VO_RET_SOURCE2_OK;
}

//portected:

VO_U32 AdaptDataSource::UpdateTrackInfo()
{
	m_nAudioCount = 0;
	m_nVideoCount = 0;
	m_nSubtitleCount = 0;

	VO_U32 nRC = VO_RET_SOURCE2_FAIL;

	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;

	VO_U32 i, j;
	VO_U32 hasVideo = 0;
	VO_U32 hasAudio = 0;
	VO_U32 m = 0;
	VO_U32 nFound = 0;

	memset(m_pAudioTrackIndex, 0, sizeof(VO_U32)*128);
	memset(m_pVideoTrackIndex, 0, sizeof(VO_U32)*128);
	memset(m_pSubtitleTrackIndex, 0, sizeof(VO_U32)*128);
	memset(m_pAudioTrackLan, 0, sizeof(char)*2*256);
	memset(m_pSubtitleTrackLan, 0, sizeof(char)*2*256);

	//nRC = GetProgramInfo(0, &m_pProgramInfo);
	//if(nRC) return nRC;

	if(m_pProgramInfo == NULL || m_pProgramInfo->ppStreamInfo == NULL)
		return nRC;

	for(i = 0; i < m_pProgramInfo->uStreamCount; i++)
	{
		pStreamInfo = m_pProgramInfo->ppStreamInfo[i];

		if( pStreamInfo->uSelInfo & VOOSMP_SRC_TRACK_SELECT_SELECTED )
			m_nCurPlayingVideoTrack = m_nVideoCount;

		m_pVideoTrackIndex[m_nVideoCount] = (VO_U32)pStreamInfo->uStreamID;
		m_nVideoCount += 1;		

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
			continue;

		hasVideo = 0;
		hasAudio = 0;

		for(j = 0; j < pStreamInfo->uTrackCount; j++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[j];

			if(pTrackInfo == NULL)
				continue;

			if(pTrackInfo->uTrackType == VOOSMP_SS_AUDIO || pTrackInfo->uTrackType == VOOSMP_SS_AUDIO_GROUP)
			{
				nFound  = 0;

				for(m = 0; m < m_nAudioCount; m++)
				{
					if(m_pAudioTrackIndex[m] == (VO_U32)pTrackInfo->uTrackID)
					{
						nFound = 1;
						break;
					}

					if(strlen(pTrackInfo->sAudioInfo.chLanguage) > 0)
					{
						if(strcmp(m_pAudioTrackLan[m], pTrackInfo->sAudioInfo.chLanguage) == 0)
						{
							nFound = 1;
							break;
						}
					}
				}

				if(nFound == 0)
				{
					m_pAudioTrackIndex[m_nAudioCount] = (VO_U32)pTrackInfo->uTrackID;
					strcpy(m_pAudioTrackLan[m_nAudioCount], pTrackInfo->sAudioInfo.chLanguage);
					m_nAudioCount += 1;
					SD_LOGI ("@@@@###  pStreamInfoID = %ld, m_nAudioCount %ld, pTrackInfo->uTrackID %ld, language %s", pStreamInfo->uStreamID ,m_nAudioCount, pTrackInfo->uTrackID, m_pAudioTrackLan[m_nAudioCount]);
				}

				if( pTrackInfo->uSelInfo & VOOSMP_SRC_TRACK_SELECT_SELECTED )
				{
					if (nFound )
						m_nCurPlayingAudioTrack = m;
					else
						m_nCurPlayingAudioTrack = m_nAudioCount - 1;
				}

				hasAudio = 1;
			}
			else if((pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE || pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE_GROUP))
			{
				nFound  = 0;

				for(m = 0; m < m_nSubtitleCount; m++)
				{
					if(m_pSubtitleTrackIndex[m] == (VO_U32)pTrackInfo->uTrackID)
					{
						nFound = 1;
						break;
					}

					if(strlen(pTrackInfo->sSubtitleInfo.chLanguage) > 0)
					{
						if(strcmp(m_pSubtitleTrackLan[m], pTrackInfo->sSubtitleInfo.chLanguage) == 0)
						{
							nFound = 1;
							break;
						}
					}
				}

				if(nFound == 0)
				{
					m_pSubtitleTrackIndex[m_nSubtitleCount] = (VO_U32)pTrackInfo->uTrackID;
					strcpy(m_pSubtitleTrackLan[m_nSubtitleCount], pTrackInfo->sSubtitleInfo.chLanguage);
					m_nSubtitleCount += 1;
					SD_LOGI ("@@@@###  pStreamInfoID = %ld, m_nSubtitleCount %ld, pTrackInfo->uTrackID %ld, language %s", pStreamInfo->uStreamID ,m_nSubtitleCount, pTrackInfo->uTrackID, m_pSubtitleTrackLan[m_nSubtitleCount]);
				}

				if( pTrackInfo->uSelInfo & VOOSMP_SRC_TRACK_SELECT_SELECTED )
				{
					if ( nFound )
						m_nCurPlayingSubtitleTrack= m;
					else
						m_nCurPlayingSubtitleTrack = m_nSubtitleCount - 1;
				}
			}
		}	
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 AdaptDataSource::IsAvaialbeIndex(VO_S32 *paIndex, VO_S32* pvIndex, VO_S32* psIndex)
{
	VO_U32 nRC = 0;

	VO_S32 nAudioSelectTrack = *paIndex;
	VO_S32	nVideoSelectTrack = *pvIndex;
	VO_S32 nSubTitleSelectTrack = *psIndex;

	char sAudioTrackLan[256];
	char sSubtitleTrackLan[256];

	memset(sAudioTrackLan, 0, sizeof(char)*256);
	memset(sSubtitleTrackLan, 0, sizeof(char)*256);

	SD_LOGI ("A %d, V %d, S %d", *paIndex, *pvIndex, *psIndex);

	nRC = UpdateTrackInfo();
	if(nRC) return 0;

	if(*paIndex >=0)
	{
		if((VO_U32)(*paIndex) < m_nAudioCount)
		{
			nAudioSelectTrack = m_pAudioTrackIndex[*paIndex];
			strcpy(sAudioTrackLan, m_pAudioTrackLan[*paIndex]);
		}
		else
		{
			return 0;
		}
	}

	if(*pvIndex >=0)
	{
		if((VO_U32)(*pvIndex) < m_nVideoCount)
		{
			nVideoSelectTrack = m_pVideoTrackIndex[*pvIndex];
		}
		else
		{
			return 0;
		}
	}

	if(*psIndex >=0)
	{
		if((VO_U32)(*psIndex) < m_nSubtitleCount)
		{
			nSubTitleSelectTrack = m_pSubtitleTrackIndex[*psIndex];
			strcpy(sSubtitleTrackLan, m_pSubtitleTrackLan[*psIndex]);
		}
		else
		{
			return 0;
		}
	}
	
	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;	
	
	VO_U32 i, j;

	VO_U32 nFoundA = 0;
	VO_U32 nFoundV = 0;
	VO_U32 nFoundS = 0;

	if(m_pProgramInfo == NULL || m_pProgramInfo->ppStreamInfo == NULL)
		return 0;

	for(i = 0; i < m_pProgramInfo->uStreamCount; i++)
	{
		pStreamInfo = m_pProgramInfo->ppStreamInfo[i];

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL)
			continue;

		nFoundA = 0;
		nFoundV = 0;
		nFoundS = 0;

		if(m_nSubtitleCount == 0)		
			nFoundS = 1;
		if(nAudioSelectTrack < 0)		
			nFoundA = 1;
		if(nSubTitleSelectTrack < 0)	
			nFoundS = 1;
		if(nVideoSelectTrack < 0)		
			nFoundV = 1;

		if(nVideoSelectTrack >= 0 && (VO_U32)pStreamInfo->uStreamID != nVideoSelectTrack)
		{
			continue;
		}

		if((VO_U32)pStreamInfo->uStreamID == nVideoSelectTrack)
		{
			nFoundV = 1;
			*pvIndex = pStreamInfo->uStreamID;
		}

		if(nVideoSelectTrack == -1/* && (pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)*/)
		{
			/* -1 value note that, this is a auto select streaming, keep -1 and set to Controller */
			*pvIndex = nVideoSelectTrack;
		}

		for(j = 0; j < pStreamInfo->uTrackCount; j++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[j];

			if(pTrackInfo == NULL)
				continue;

			if(((VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_AUDIO || (VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_AUDIO_GROUP)) 
			{
				if((VO_U32)pTrackInfo->uTrackID == nAudioSelectTrack)
				{
					nFoundA = 1;
					
					if(nVideoSelectTrack == -1)
					{
						if(pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)
							*paIndex = (VO_U32)pTrackInfo->uTrackID;
					}
					else
					{
						*paIndex = (VO_U32)pTrackInfo->uTrackID;
					}
					continue;
				}
				
				if(strlen(sAudioTrackLan) > 0 && strcmp(pTrackInfo->sAudioInfo.chLanguage, sAudioTrackLan) == 0)
				{
					nFoundA = 1;
					if(nVideoSelectTrack == -1)
					{
						if(pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)
							*paIndex = (VO_U32)pTrackInfo->uTrackID;
					}
					else
					{
						*paIndex = (VO_U32)pTrackInfo->uTrackID;
					}
					continue;
				}
			}
			else if(((VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE || (VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE_GROUP))
			{
				if((VO_U32)pTrackInfo->uTrackID == nSubTitleSelectTrack)
				{
					nFoundS = 1;
					if(nVideoSelectTrack == -1)
					{
						if(pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)
							*psIndex = (VO_U32)pTrackInfo->uTrackID;
					}
					else
					{
						*psIndex = (VO_U32)pTrackInfo->uTrackID;
					}
					continue;
				}

				if(strlen(sSubtitleTrackLan) > 0 && strcmp(pTrackInfo->sSubtitleInfo.chLanguage, sSubtitleTrackLan) == 0)
				{
					nFoundS = 1;
					if(nVideoSelectTrack == -1)
					{
						if(pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)
							*psIndex = (VO_U32)pTrackInfo->uTrackID;
					}
					else
					{
						*psIndex = (VO_U32)pTrackInfo->uTrackID;
					}
					continue;
				}
			}
			else if((VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_MUX_GROUP)
			{
				if((nVideoSelectTrack == -1 && (pStreamInfo->uSelInfo&VOOSMP_SRC_TRACK_SELECT_SELECTED)) ||
					nVideoSelectTrack == (VO_U32)pStreamInfo->uStreamID)
				{
					if(nFoundA == 0)
						*paIndex = nAudioSelectTrack;
					
					if(nFoundS == 0)
						*psIndex = nSubTitleSelectTrack;
				}

				nFoundA = 1;
				nFoundS = 1;
			}
		}
        
        if(nVideoSelectTrack == -1)
        {
            if(nFoundA == 0 || nFoundS == 0)
                return 0;
        }

		SD_LOGI ("nVideoSelectTrack %d.nFoundA %d, nFoundV %d, nFoundS %d", nVideoSelectTrack, nFoundA, nFoundV, nFoundS);
		if(nVideoSelectTrack != -1 && nFoundA && nFoundV && nFoundS)
		{
			return (m_pProgramInfo->uProgramID << 16) | (*pvIndex << 8) | 1;
		}
	}

	if(nAudioSelectTrack == -2 && nVideoSelectTrack == -2)
		return 1;

	if(nAudioSelectTrack == -2 && nSubTitleSelectTrack == -2)
		return 1;

	if(nVideoSelectTrack == -2 && nSubTitleSelectTrack == -2)
		return 1;
    
    if(nVideoSelectTrack == -1)
    {
        if(nFoundA && nFoundV && nFoundS)
		{
			return 1;
		}
    }

	return 0;
}

VO_U32 AdaptDataSource::ClearPropertyItem(VOOSMP_SRC_TRACK_PROPERTY *pItemProperty)
{
	if(pItemProperty == NULL)
		return VO_RET_SOURCE2_OK;

	if(pItemProperty->nPropertyCount > 0 && pItemProperty->ppItemProperties)
	{
		VO_S32 n;

		for(n = 0; n < pItemProperty->nPropertyCount; n++)
		{
			if(pItemProperty->ppItemProperties[n])
			{
				if(pItemProperty->ppItemProperties[n]->pszProperty)
				{
					delete []pItemProperty->ppItemProperties[n]->pszProperty;
					pItemProperty->ppItemProperties[n]->pszProperty = NULL;
				}

				delete []pItemProperty->ppItemProperties[n];

				pItemProperty->ppItemProperties[n] = NULL;
			}
		}

		delete []pItemProperty->ppItemProperties;
		pItemProperty->ppItemProperties = NULL;
	}

	memset(pItemProperty, 0, sizeof(VOOSMP_SRC_TRACK_PROPERTY));

	return 0;
}

VO_U32 AdaptDataSource::IsAudioIndexAvaialbe( VO_S32 nAudioIndex)
{
	if (nAudioIndex < 0) return 0;
	if (UpdateTrackInfo()) return 0;

	VO_U32 nCheckResult = 0;
	VO_U32 i = 0;
	VO_BOOL find_stream = VO_FALSE;
	VO_S32 nAudioSelectTrackID = m_pAudioTrackIndex[nAudioIndex];
	VO_S32 nVideoSelectTrackID = m_pVideoTrackIndex[m_nCurSelectedVideoTrack];

	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;	

	for(i = 0; i < m_pProgramInfo->uStreamCount; i++)
	{
		pStreamInfo = m_pProgramInfo->ppStreamInfo[i];

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL )
			continue;
		
		for(VO_U32 j = 0; j < pStreamInfo->uTrackCount; j++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[j];

			if( pTrackInfo == NULL)
				continue;

			if(((VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_VIDEO 
				|| (VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_VIDEO_GROUP
				|| (VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_MUX_GROUP))
			{
				if((VO_U32)pTrackInfo->uTrackID == nVideoSelectTrackID)
				{
					find_stream = VO_TRUE;
					break;
				}
			}
		}

		if (find_stream)
		{
			break;
		}

	}

	if(find_stream)
	{
		for(VO_U32 j = 0; j < pStreamInfo->uTrackCount; j++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[j];
		
			if( pTrackInfo == NULL)
				continue;
		
			if(((VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_AUDIO || (VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_AUDIO_GROUP)) 
			{
				if((VO_U32)pTrackInfo->uTrackID == nAudioSelectTrackID)
				{
					nCheckResult  = 1;
				}
		
				if(strcmp(pTrackInfo->sAudioInfo.chLanguage,  m_pAudioTrackLan[nAudioIndex]) == 0)
				{
					nCheckResult  = 1;
				}
			}
		}

	}

	return nCheckResult;
}

VO_U32 AdaptDataSource::IsVideoIndexAvaialbe( VO_S32 nVideoIndex)
{
	return 1;
}

VO_U32 AdaptDataSource::IsSubtitleIndexAvaialbe( VO_S32 nSubtitleIndex)
{
	if (nSubtitleIndex < 0) return 0;
	if (UpdateTrackInfo()) return 0;

	VO_U32 nCheckResult = 0;
	VO_U32 i = 0;
	VO_BOOL find_stream = VO_FALSE;
	VO_S32 nSubtitleSelectTrackID = m_pSubtitleTrackIndex[nSubtitleIndex];
	VO_S32 nVideoSelectTrackID = m_pVideoTrackIndex[m_nCurSelectedVideoTrack];

	VO_SOURCE2_STREAM_INFO* pStreamInfo = NULL;
	VO_SOURCE2_TRACK_INFO* pTrackInfo = NULL;	

	for(i = 0; i < m_pProgramInfo->uStreamCount; i++)
	{
		pStreamInfo = m_pProgramInfo->ppStreamInfo[i];

		if(pStreamInfo == NULL || pStreamInfo->ppTrackInfo == NULL )
			continue;

		for(VO_U32 j = 0; j < pStreamInfo->uTrackCount; j++)
		{
			pTrackInfo = pStreamInfo->ppTrackInfo[j];
		
			if( pTrackInfo == NULL)
				continue;
		
			if(((VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_VIDEO 
				|| (VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_VIDEO_GROUP
				|| (VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_MUX_GROUP))
			{
				if((VO_U32)pTrackInfo->uTrackID == nVideoSelectTrackID)
				{
					find_stream = VO_TRUE;
					break;
				}
			}
		}

		if (find_stream)
		{
			break;
		}
		
	}
	
	for(VO_U32 j = 0; j < pStreamInfo->uTrackCount; j++)
	{
		pTrackInfo = pStreamInfo->ppTrackInfo[j];
	
		if( pTrackInfo == NULL)
			continue;
	
		if(((VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE || (VO_U32)pTrackInfo->uTrackType == VOOSMP_SS_SUBTITLE_GROUP)) 
		{
			if((VO_U32)pTrackInfo->uTrackID == nSubtitleSelectTrackID)
			{
				nCheckResult  = 1;
			}
	
			if(strcmp(pTrackInfo->sSubtitleInfo.chLanguage,  m_pSubtitleTrackLan[nSubtitleIndex]) == 0)
			{
				nCheckResult  = 1;
			}
		}
	}

	return nCheckResult;
}

/*
VO_U32 AdaptDataSource::IsSubtitleIndexAvaialbe( VO_S32 nSubtitleIndex)
{
	VO_S32 nAudioSelectTrack = m_nAudioSelected;
	VO_S32	nVideoSelectTrack = m_nVideoSelected;
	VO_S32 nSubTitleSelectTrack = m_nSubtitleSelected;

	nSubTitleSelectTrack = nSubtitleIndex;

	VO_U32 nStream = IsAvaialbeIndex(&nAudioSelectTrack, &nVideoSelectTrack, &nSubTitleSelectTrack);

	if(nStream)
		return 1;
	
	return 0;
}
*/
VO_VOID AdaptDataSource::SetOutSideID( _PROGRAM_INFO *pProgInfo, VO_SOURCE2_PROGRAM_INFO *pSourceProgInfo)
{
	if( !pProgInfo && !pSourceProgInfo)
		return ;

	for( VO_U32 i = 0; i < pProgInfo->uStreamCount; i++ )
	{
		for ( VO_U32 n = 0; n < pProgInfo->ppStreamInfo[i]->uTrackCount; n++ )
		{
			pSourceProgInfo->ppStreamInfo[i]->ppTrackInfo[n]->uTrackID = pProgInfo->ppStreamInfo[i]->ppTrackInfo[n]->uASTrackID;
		}
	}
}

VO_VOID AdaptDataSource::Reset()
{
	m_nAudioCount=0;
	m_nVideoCount=0;
	m_nSubtitleCount=0;

	memset(m_pAudioTrackIndex, 0x0, sizeof(m_pAudioTrackIndex));
	memset(m_pVideoTrackIndex, 0x0, sizeof(m_pVideoTrackIndex));
	memset(m_pSubtitleTrackIndex, 0x0, sizeof(m_pSubtitleTrackIndex));

	memset(m_pAudioTrackLan, 0x0, sizeof(m_pAudioTrackLan));
	memset(m_pSubtitleTrackLan, 0x0, sizeof(m_pSubtitleTrackLan));

	m_nAudioSelected=-1;
	m_nVideoSelected=-1;
	m_nSubtitleSelected=-1;
	
	m_nCurPlayingAudioTrack=-1;
	m_nCurPlayingVideoTrack=-1;
	m_nCurPlayingSubtitleTrack=-1;

	m_nCurSelectedAudioTrack=-1;
	m_nCurSelectedVideoTrack=-1;
	m_nCurSelectedSubtitleTrack=-1;

	ReleaseProgramInfoOP_T(m_pProgramInfo);
	m_pProgramInfo=NULL;

	ClearPropertyItem(&m_szAudioProtery);
	ClearPropertyItem(&m_szVideoProtery);
	ClearPropertyItem(&m_szSubtitleProtery);

}

