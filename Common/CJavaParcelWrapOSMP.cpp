/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved
 
VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
 
All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/

/************************************************************************
 * @file CJavaParcelWrapOSMP.cpp
 * Convert some C struct data to Java Parcel object.
 *
 * This class base on CJavaParcelWrap
 *
 *
 * @author  Li Mingbo
 * @date    2012-2012 
 ************************************************************************/

#include "CJavaParcelWrapOSMP.h"
#include "CJavaParcelWrapOSMP.h"
#define  LOG_TAG    "CJavaParcelWrapOSMP"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_SRC_PROGRAM_INFO* pInfo)
{
//typedef struct  
//{
//	int								nProgramID;				/*!< the stream id created by our parser, it is unique in this source session */
//	int								nSelInfo;				/*!< Indicated if the Stream is selected and recommend or default */
//	char							szProgramName[256];		/*!< Name of the program */
//	VOOSMP_SRC_PROGRAM_TYPE			nProgramType;			/*!< Indicate if the Program is live or vod */
//	int								nStreamCount;			/*!< Stream Count */
//	VOOSMP_SRC_STREAM_INFO**		pStreamInfo;			/*!< Stream info */	
//}VOOSMP_SRC_PROGRAM_INFO;
	writeInt32((jint)pInfo->nProgramID);
	writeInt32((jint)pInfo->nSelInfo);
	write((unsigned char*)pInfo->szProgramName,256);
	writeInt32((jint)pInfo->nProgramType);
	writeInt32((jint)pInfo->nStreamCount);
	if(pInfo->nStreamCount>0)
	{
	//VOLOGI("5 pInfo->nStreamCount  %d",pInfo->nStreamCount);
		for(int i=0;i<pInfo->nStreamCount;i++)
		{
			if(pInfo->pStreamInfo[i]==NULL)
			{
				VOLOGI("ERROR: pInfo->pStreamInfo[i]==NULL");
				continue;
			}
			fillParcelData(pInfo->pStreamInfo[i]);
		}
	//VOLOGI("6 pInfo->nStreamCount  %d",pInfo->nStreamCount);
	}

	return 0;
}

int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_SRC_STREAM_INFO* pInfo)
{
//typedef struct  
//{
//	int							nStreamID;				/*!< the sub stream id created by our parser, it is unique in this source session */
//	int							nSelInfo;				/*!< Indicated if the Stream is selected and recommend or default */
//	int							nBitrate;				/*!< the bitrate of the stream */
//	int							nTrackCount;			/*!< track count if this sub stream */
//	VOOSMP_SRC_TRACK_INFO		**ppTrackInfo;			/*!< Track info */
//}VOOSMP_SRC_STREAM_INFO;
	writeInt32((jint)pInfo->nStreamID);
	writeInt32((jint)pInfo->nSelInfo);
	writeInt32((jint)pInfo->nBitrate);
	writeInt32((jint)pInfo->nTrackCount);
	//VOLOGI("1 pInfo->nTrackCount  %d",pInfo->nTrackCount);
	for(int i=0;i<pInfo->nTrackCount;i++)
	{
//		VOLOGI("2 pInfo->ppTrackInfo[i]  %d, type is %d",i, pInfo->ppTrackInfo[i]->nTrackType);
		if(pInfo->ppTrackInfo[i]==NULL)
		{
			VOLOGI("ERROR: pInfo->ppTrackInfo[i]==NULL");
			continue;
		}
		fillParcelData(pInfo->ppTrackInfo[i]);
		//VOLOGI("3 pInfo->ppTrackInfo[i]  %d",(int)(pInfo->ppTrackInfo[i]));
	}
	return 0;
}

int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_SRC_TRACK_INFO* pInfo)
{
//typedef struct
//{
//	int							nTrackID;				/*!< the track id created by our parser, it is unique in this source session */
//	int                         nSelectInfo;			/*!< Indicated if the track is selected and recommend or default,refer to VOOSMP_SRC_TRACK_SELECT */
//
//	char						szFourCC[8];
//	VOOSMP_SOURCE_STREAMTYPE	nTrackType;				/*!< video/audio/subtitle */
//	int							nCodec;					/*!< codec type,refer to VOOSMP_AUDIO_CODINGTYPE and VOOSMP_VIDEO_CODINGTYPE  */	
//	long long					nDuration;				/*!< duration of this track */
//	
//	int							nChunkCounts;			/*!< chunks in the track */
//	int							nBitrate;				/*!< bitrate of this track */
//	
//	union
//	{
//		VOOSMP_SRC_AUDIO_INFO	AudioInfo;				/*!< audio info */
//		VOOSMP_SRC_VIDEO_INFO	VideoInfo;				/*!< video info */
//	};
//	
//	int							nHeadSize;				/*!< sequence head data size*/
//	char*						pHeadData;				/*!< sequence head data */
//}VOOSMP_SRC_TRACK_INFO;
	writeInt32((jint)pInfo->nTrackID);
	writeInt32((jint)pInfo->nSelectInfo);
	
	if(pInfo->szFourCC!=NULL)
	{
		write((unsigned char*)pInfo->szFourCC,8);
	}
	else
	{
		VOLOGI("pInfo->szFourCC is NULL");
		VO_BYTE bt[8];
		memset(bt,0,8);
		write(bt,8);
	}

	writeInt32((jint)pInfo->nTrackType);
	writeInt32((jint)pInfo->nCodec);
	writeInt32((jint)pInfo->nDuration);
	writeInt32((jint)pInfo->nChunkCounts);
	writeInt32((jint)pInfo->nBitrate);

	if((pInfo->nTrackType == VOOSMP_SS_AUDIO) || (pInfo->nTrackType == VOOSMP_SS_AUDIO_GROUP))
		fillParcelData(&pInfo->AudioInfo);

	if((pInfo->nTrackType == VOOSMP_SS_VIDEO) || (pInfo->nTrackType == VOOSMP_SS_VIDEO_GROUP))
		fillParcelData(&pInfo->VideoInfo);

	if((pInfo->nTrackType == VOOSMP_SS_SUBTITLE) || (pInfo->nTrackType == VOOSMP_SS_SUBTITLE_GROUP))
		fillParcelData(&pInfo->SubtitleInfo);

	jint nHead = pInfo->nHeadSize;
	if(pInfo->pHeadData == NULL)
		nHead = 0;
	writeInt32((jint)nHead);
	if(nHead>0)
	{
		//VOLOGI("2 pInfo->pHeadData =%d  ",(int)pInfo->pHeadData[0]);
		if(pInfo->pHeadData!=NULL)
		{
			write((unsigned char*)pInfo->pHeadData,pInfo->nHeadSize);
		}
		else
		{
			VOLOGI("pInfo->pHeadData is NULL");
			VO_BYTE * pb = new VO_BYTE[pInfo->nHeadSize];
			if(pb)
			{
				write((unsigned char*)pb,pInfo->nHeadSize);
				delete pb;
			}
		}
	//VOLOGI("3 pInfo->pHeadData =%d  ",(int)pInfo->pHeadData[0]);
	}
	return 0;
}

int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_SRC_VIDEO_INFO* pInfo)
{
//typedef struct
//{
//	VOOSMP_VIDEO_FORMAT sFormat;
//	int					nAngle;
//	void*				pAlign[3];
//} VOOSMP_SRC_VIDEO_INFO;
	fillParcelData(&pInfo->sFormat);
	writeInt32((jint)pInfo->nAngle);
	return 0;
}

int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_SRC_AUDIO_INFO* pInfo)
{
//typedef struct
//{
//	VOOSMP_AUDIO_FORMAT sFormat;
//	char				szLanguage[16];
//} VOOSMP_SRC_AUDIO_INFO;
	fillParcelData(&pInfo->sFormat);
	write((unsigned char*)pInfo->szLanguage,256);
	return 0;
}

int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_VIDEO_FORMAT* pInfo)
{
//typedef struct
//{
//	int					Width;		 /*!< Width */
//	int					Height;		 /*!< Height */
//	int					Type;		 /*!< Color type  */
//}VOOSMP_VIDEO_FORMAT;
	writeInt32((jint)pInfo->Width);
	writeInt32((jint)pInfo->Height);
	writeInt32((jint)pInfo->Type);
	return 0;
}

int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_AUDIO_FORMAT* pInfo)
{
//typedef struct
//{
//	int		SampleRate;  /*!< Sample rate */
//	int		Channels;    /*!< Channel count */
//	int		SampleBits;  /*!< Bits per sample */
//}VOOSMP_AUDIO_FORMAT;
	writeInt32((jint)pInfo->SampleRate);
	writeInt32((jint)pInfo->Channels);
	writeInt32((jint)pInfo->SampleBits);
	return 0;
}
int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_SRC_CHUNK_INFO* pInfo)
{
/**
 * Structure of chunk
 */
//typedef struct
//{
//	VOOSMP_SRC_CHUNK_TYPE                   nType;                          /*!<The type of this chunk */
//	char									szRootUrl[VOOSMP_MAX_URL_LEN];	/*!<The URL of manifest. It must be filled by parser. */
//	char									szUrl[VOOSMP_MAX_URL_LEN];		/*!<URL of this chunk , maybe relative URL */
//	unsigned long long						ullStartTime;                   /*!<The start offset time of this chunk , the unit of ( ullStartTime / ullTimeScale * 1000 ) should be ms */
//	unsigned long long						ullDuration;                    /*!<Duration of this chunk , the unit of ( ullDuration / ullTimeScale * 1000 ) should be ms */
//	unsigned long long						ullTimeScale;                   /*!<Time scale of this chunk */
//	unsigned int							uReserved1;                     /*!<Reserved 1 */
//	unsigned int							uReserved2;                     /*!<Reserved 1 */
//}VOOSMP_SRC_CHUNK_INFO;
	writeInt32((jint)pInfo->nType);
	write((unsigned char*)pInfo->szRootUrl,VOOSMP_MAX_URL_LEN);
	write((unsigned char*)pInfo->szUrl,VOOSMP_MAX_URL_LEN);
	writeInt64((jlong)pInfo->ullStartTime);
	writeInt32((jint)pInfo->uPeriodSequenceNumber);
	writeInt64((jlong)pInfo->ullDuration);
	writeInt64((jlong)pInfo->ullTimeScale);
	writeInt32((jint)pInfo->uReserved1);
	writeInt32((jint)pInfo->uReserved2);
	return 0;
}
int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_SRC_CUSTOMERTAG_TIMEDTAG* pInfo)
{
 /**
 * Structure of source customer timed tag
 */
//typedef struct  
//{
//	unsigned long long		ullTimeStamp;				/*!<Time stamp*/
//	
//	unsigned int			uSize;						/*!<Size */
//	void*					pData;						/*!<Data pointor */
//		
//	unsigned int			uFlag;						/*!<For further use */
//	void*					pReserve;					/*!<Data pointor */
//}VOOSMP_SRC_CUSTOMERTAG_TIMEDTAG;
	writeInt64((jlong)pInfo->ullTimeStamp);
	writeInt32((jint)pInfo->uSize);
	if(pInfo->uSize>0)
		write((unsigned char*)pInfo->pData,pInfo->uSize);
	writeInt32((jint)pInfo->uFlag);
	return 0;
}
int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_CLOCK_TIME_STAMP* pTimeStamp)
{
//typedef struct
//{
//    int nClockTimestampFlag;
//    int nCtType;
//    int nNuitFieldBasedFlag;
//    int nCountingType;
//    int nFullTimestampFlag;
//    int nDiscontinuityFlag;
//    int nCntDroppedFlag;
//    int nFrames;
//    int nSecondsValue;
//    int nMinutesValue;
//    int nHoursValue;
//    int nSecondsFlag;
//    int nMinutesFlag;
//    int nHoursFlag;
//    int nTimeOffset;
//}VOOSMP_CLOCK_TIME_STAMP;
	writeInt32((jint)pTimeStamp->nClockTimestampFlag);
	writeInt32((jint)pTimeStamp->nCtType);
	writeInt32((jint)pTimeStamp->nNuitFieldBasedFlag);
	writeInt32((jint)pTimeStamp->nCountingType);
	writeInt32((jint)pTimeStamp->nFullTimestampFlag);
	writeInt32((jint)pTimeStamp->nDiscontinuityFlag);
	writeInt32((jint)pTimeStamp->nCntDroppedFlag);
	writeInt32((jint)pTimeStamp->nFrames);
	writeInt32((jint)pTimeStamp->nSecondsValue);
	writeInt32((jint)pTimeStamp->nMinutesValue);
	writeInt32((jint)pTimeStamp->nHoursValue);
	writeInt32((jint)pTimeStamp->nSecondsFlag);
	writeInt32((jint)pTimeStamp->nMinutesFlag);
	writeInt32((jint)pTimeStamp->nHoursFlag);
	writeInt32((jint)pTimeStamp->nTimeOffset);
	return 0;
}
int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_SEI_PIC_TIMING* pPicTiming)
{
//typedef struct
//{
//    int nCpbDpbDelaysPresentFlag;
//    int nCpbRemovalDelay;
//    int nDpbOutputDelay;
//    int nPictureStructurePresentFlag;
//    int nPictureStructure;
//    int nNumClockTs;
//    VOOSMP_CLOCK_TIME_STAMP sClock[3];
//}VOOSMP_SEI_PIC_TIMING;
	writeInt32((jint)pPicTiming->nCpbDpbDelaysPresentFlag);
	writeInt32((jint)pPicTiming->nCpbRemovalDelay);
	writeInt32((jint)pPicTiming->nDpbOutputDelay);
	writeInt32((jint)pPicTiming->nPictureStructurePresentFlag);
	writeInt32((jint)pPicTiming->nPictureStructure);
	writeInt32((jint)pPicTiming->nNumClockTs);
	for(int i = 0;i<3;i++){
		fillParcelData(pPicTiming->sClock+i);
	}
	return 0;
}

int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_SRC_SUBTITLE_INFO* pInfo)
{
	write((unsigned char*)pInfo->szLanguage,256);
	writeInt32((jint)pInfo->nCodingType);
	write((unsigned char*)pInfo->Align, 8);

//	VOLOGI("Parser subtitle info, szlang is %s, size is %d .", pInfo->szLanguage,sizeof(pInfo->szLanguage));
	return 0;

}

int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_SRC_IO_FAILED_REASON_DESCRIPTION* pIOFailedDesc, const char* url)
{

	writeInt32((jint)pIOFailedDesc->reason);

	jstring str = m_env->NewStringUTF(pIOFailedDesc->pszResponse);
	writeString(str);

	jstring str2 = m_env->NewStringUTF(url);
	writeString(str2);

	m_env->DeleteLocalRef(str);
	m_env->DeleteLocalRef(str2);


	return 0;
}
int CJavaParcelWrapOSMP::fillParcelData(VOOSMP_PCMBUFFER* pInfo)
{

//	typedef struct
	//{
//		/*!< the timestamp of buffer */
//		long long nTimestamp;
//		
		/*!< the size of buffer in bytes */
//		int  nBufferSize;
//		
//		/*!< return buffer array */
//		unsigned char * pBuffer;
//	}VOOSMP_PCMBUFFER;

	writeInt64((jlong)pInfo->nTimestamp);
	writeInt32((jint)pInfo->nBufferSize);
	if(pInfo->nBufferSize>0)
	write((unsigned char*)pInfo->pBuffer,pInfo->nBufferSize);


	return 0;
}

int CJavaParcelWrapOSMP :: fillParcelData(VOOSMP_MODULE_VERSION* pInfo)
{

//typedef struct
//{
  //  int     nModuleType;    /*!< [in]Indicator module type,refer to VOOSMP_MODULE_TYPE*/
    //char*   pszVersion;     /*!< [out]Output the version information */
//}VOOSMP_MODULE_VERSION;

	writeInt32((jint)pInfo->nModuleType);
	jstring str = m_env->NewStringUTF(pInfo->pszVersion);
	writeString(str);
	return 0;
}

int CJavaParcelWrapOSMP :: fillParcelData(VOOSMP_SRC_DRM_INIT_DATA_RESPONSE* pInfo)
{

//typedef struct
//{
//    char*               pData;      // Data of DRM init info
//    unsigned int        nDataSize;  // Size of DRM init info
//}VOOSMP_SRC_DRM_INIT_DATA;

	writeInt32((jint)pInfo->uSize);
	if(pInfo->uSize>0)
		write((unsigned char*)pInfo->pData,pInfo->uSize);

	return 0;
}

int CJavaParcelWrapOSMP::fillParcelData(VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO* pInfo)
{
//typedef struct
//{
//	VO_U32 uPercentage;
//	VO_U32 uPeriodID;
//	VO_U64 ullElapsedTime;
//}VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO;
	writeInt32((jint)pInfo->uPercentage);
	writeInt32((jint)pInfo->uPeriodID);
	writeInt64((jlong)pInfo->ullElapsedTime);
	return 0;
}

int CJavaParcelWrapOSMP::fillParcelData(VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO* pInfo)
{
//typedef struct
//	VO_ADSMANAGER_ACTION nAction;
//	VO_U32 uPeriodID;
//	VO_U64 ullElapsedTime;
//}VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO;
	writeInt32((jint)pInfo->nAction);
	writeInt32((jint)pInfo->uPeriodID);
	writeInt64((jlong)pInfo->ullElapsedTime);
	return 0;
}



int CJavaParcelWrapOSMP :: fillParcelData(VO_ADSMANAGER_PLAYBACKPERIOD* pInfo)
{

//typedef struct
//{
//	VO_U32 uID;
//	VO_ADSMANAGER_PERIODTYPE nPeriodType;	//the period type
//	VO_CHAR strPeriodURL[MAXURLLEN];		//the period URL
//	VO_U64 ullStartTime;					//the period start time
//	VO_U64 ullEndTime;						//the period end time
//	VO_CHAR strCaptionURL[MAXURLLEN];		//the caption/subtitle URL
//	VO_CHAR strPeriodTitle[MAXURLLEN];
//	VO_CHAR strPeriodID[64];
//	VO_BOOL isLive;
//   VO_BOOL isEpisode;
//}VO_ADSMANAGER_PLAYBACKPERIOD;
	writeInt32((jint)pInfo->uID);
	writeInt32((jint)pInfo->nPeriodType);
	jstring str = m_env->NewStringUTF(pInfo->strPeriodURL);
	writeString(str);
	writeInt64((jlong)pInfo->ullStartTime);
	writeInt64((jlong)pInfo->ullEndTime);
	jstring str2 = m_env->NewStringUTF(pInfo->strCaptionURL);
	writeString(str2);
	jstring str3 = m_env->NewStringUTF(pInfo->strPeriodTitle);
	writeString(str3);
	jstring str4 = m_env->NewStringUTF(pInfo->strPeriodID);
	writeString(str4);
	writeInt32((jint)pInfo->isLive);
	writeInt32((jint)pInfo->isEpisode);
	return 0;
}

int CJavaParcelWrapOSMP :: fillParcelData(VO_ADSMANAGER_PLAYBACKINFO* pInfo)
{

//typedef struct 
//{
//	VO_U32 nCounts;								//Indicate the counts of periods
//	VO_ADSMANAGER_PLAYBACKPERIOD *pPeriods;		//Period list
//}VO_ADSMANAGER_PLAYBACKINFO;

	writeInt32((jint)pInfo->nCounts);
	for(int i = 0;i<(int)pInfo->nCounts;i++){
		fillParcelData(&pInfo->pPeriods[i]);
	}
	return 0;
}

int CJavaParcelWrapOSMP :: fillParcelData(VOOSMP_SRC_CHUNK_SAMPLE* pInfo)
{
	writeInt32((jint)pInfo->nChunkFlag);
	writeInt64((jlong)pInfo->ullChunkStartTime);
	writeInt32((jint)pInfo->uPeriodSequenceNumber);
    writeInt64((jlong)pInfo->ullPeriodFirstChunkStartTime);
    
    jlong sampleTimeStamp = 0L;
    if (NULL != pInfo->pullSampleTimeStamp) {
        sampleTimeStamp = *(pInfo->pullSampleTimeStamp);
    }
	writeInt64((jlong)sampleTimeStamp);
    writeInt32((jint)(pInfo->pullSampleTimeStamp));
	return 0;
}

int CJavaParcelWrapOSMP :: fillParcelData(VO_ADSMANAGER_SEEKINFO* pInfo)
{
	writeInt32((jlong)pInfo->uAdsSequnceNumber);
	writeInt64((jlong)pInfo->ullContentSeekPos);
	return 0;
}

int CJavaParcelWrapOSMP :: fillParcelData(VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO* pInfo)
{

//typedef struct  
//{
//	VO_U32 uPeriodID;
//	VO_U64 ullTimePassed;
//}VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO;


	writeInt32((jint)pInfo->uPeriodID);
	writeInt64((jlong)pInfo->ullTimePassed);
	return 0;
}

int CJavaParcelWrapOSMP :: fillParcelData(VO_ADSMANAGER_TRACKINGEVENT_INFO* pInfo)
{

//typedef struct  
//{
//	VO_U32 uPeriodID;
//	VO_U32 uUrlCount;
//	VO_CHAR ** ppUrl;
//}VO_ADSMANAGER_TRACKINGEVENT_INFO;


	writeInt32((jint)pInfo->uPeriodID);
	writeInt32((jint)pInfo->uUrlCount);
	for(int i = 0;i<(int)pInfo->uUrlCount;i++){
		jstring str = m_env->NewStringUTF(pInfo->ppUrl[i]);
		writeString(str);
	}
	return 0;
}

#ifdef _VONAMESPACE
}
#endif
