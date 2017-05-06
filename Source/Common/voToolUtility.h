
#ifndef __VO_TOOLUTILITY_H__
#define __VO_TOOLUTILITY_H__

#include "voYYDef_SrcCmn.h"
#include "string.h"
#include "time.h"
#include "voString.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "voSource2.h"
#include "voDSType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

// #ifdef _IOS 
// #define _VODSLOG_INFO
// #define _VODSLOG_ERROR
// #define _VODSLOG_WARNING
// #endif


static VO_VOID PrintDSTrackInfo(VO_DATASOURCE_TRACK_INFO *pTrackInfo)
{
	VOLOGI("****************************PrintDSTrackInfo+*********************************************");

	if(!pTrackInfo){
		return;
	}

	VOLOGI("uOutSideTrackID=%lu, uFPTrackID=%lu, uASTrackID=%lu, Sel=%lu, TrackType=%lu, Codec=%lu",
		pTrackInfo->uOutSideTrackID, pTrackInfo->uFPTrackID, pTrackInfo->uASTrackID, pTrackInfo->uSelInfo, pTrackInfo->uTrackType, pTrackInfo->uCodec);
	
	VOLOGI("Duration=%llu, ChunkCounts=%lu, Bitrate=%lu, HeadSize=%lu",
		pTrackInfo->uDuration, pTrackInfo->uChunkCounts, pTrackInfo->uBitrate, pTrackInfo->uHeadSize);

	if(VO_DATASOURCE_TT_AUDIO == pTrackInfo->uTrackType || VO_DATASOURCE_TT_AUDIOGROUP == pTrackInfo->uTrackType){
		VOLOGI("sAudioInfo.chLanguage=%s",pTrackInfo->sAudioInfo.chLanguage);
	}
	else if(VO_DATASOURCE_TT_SUBTITLE == pTrackInfo->uTrackType || VO_DATASOURCE_TT_SUBTITLEGROUP == pTrackInfo->uTrackType){
		VOLOGI("sSubtitleInfo.chLanguage=%s",pTrackInfo->sSubtitleInfo.chLanguage);
	}

	VOLOGI("****************************PrintDSTrackInfo-*********************************************");

}

static VO_VOID PrintDSStreamInfo(VO_DATASOURCE_STREAM_INFO *pStreamInfo)
{
	VOLOGI("****************************PrintDSStreamInfo+*********************************************");

	if(!pStreamInfo){
		return;
	}
	VOLOGI("StreamID=%lu, Sel=%lu, uBitrate=%lu, uTrackCount=%lu",
		pStreamInfo->uStreamID, pStreamInfo->uSelInfo, pStreamInfo->uBitrate, pStreamInfo->uTrackCount);

	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		PrintDSTrackInfo(pStreamInfo->ppTrackInfo[index]);
	}

	VOLOGI("****************************PrintDSStreamInfo-*********************************************");
	
}

static VO_VOID PrintDSProgramInfo(VO_DATASOURCE_PROGRAM_INFO *pProgramInfo)
{
	VOLOGI("****************************PrintDSProgramInfo+*********************************************");

	if(!pProgramInfo){
		return;
	}

	VOLOGI("ProgramID=%lu, Sel=%lu, Type=%lu, Name=%s,uStreamCount=%lu",
		pProgramInfo->uProgramID, pProgramInfo->uSelInfo, pProgramInfo->sProgramType, pProgramInfo->strProgramName, pProgramInfo->uStreamCount);

	for(VO_U32 index = 0; index < pProgramInfo->uStreamCount; index++)
	{
		PrintDSStreamInfo(pProgramInfo->ppStreamInfo[index]);
	}


	VOLOGI("****************************PrintDSProgramInfo-*********************************************");
}

static VO_VOID PrintTrackInfo(VO_SOURCE2_TRACK_INFO *pTrackInfo)
{
	VOLOGI("****************************PrintTrackInfo+*********************************************");
	if(!pTrackInfo){
		return;
	}

	VOLOGI("uTrackID=%lu, Sel=%lu, TrackType=%lu, Codec=%lu",
		pTrackInfo->uTrackID, pTrackInfo->uSelInfo, pTrackInfo->uTrackType, pTrackInfo->uCodec);
	
	VOLOGI("Duration=%llu, ChunkCounts=%lu, Bitrate=%lu, HeadSize=%lu",
		pTrackInfo->uDuration, pTrackInfo->uChunkCounts, pTrackInfo->uBitrate, pTrackInfo->uHeadSize);
	
	if(VO_DATASOURCE_TT_AUDIO == pTrackInfo->uTrackType || VO_DATASOURCE_TT_AUDIOGROUP == pTrackInfo->uTrackType){
		VOLOGI("sAudioInfo.chLanguage=%s",pTrackInfo->sAudioInfo.chLanguage);
	}
	else if(VO_DATASOURCE_TT_SUBTITLE == pTrackInfo->uTrackType || VO_DATASOURCE_TT_SUBTITLEGROUP == pTrackInfo->uTrackType){
		VOLOGI("sSubtitleInfo.chLanguage=%s",pTrackInfo->sSubtitleInfo.chLanguage);
	}

	VOLOGI("****************************PrintTrackInfo-*********************************************");

}

static VO_VOID PrintStreamInfo(VO_SOURCE2_STREAM_INFO *pStreamInfo)
{
	VOLOGI("****************************PrintStreamInfo+*********************************************");

	if(!pStreamInfo){
		return;
	}
	VOLOGI("StreamID=%lu, Sel=%lu, uBitrate=%lu, uTrackCount=%lu",
		pStreamInfo->uStreamID, pStreamInfo->uSelInfo, pStreamInfo->uBitrate, pStreamInfo->uTrackCount);

	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		PrintTrackInfo(pStreamInfo->ppTrackInfo[index]);
	}

	VOLOGI("****************************PrintStreamInfo-*********************************************");
	
}

static VO_VOID PrintProgramInfo(VO_SOURCE2_PROGRAM_INFO *pProgramInfo)
{
	VOLOGI("****************************PrintProgramInfo+*********************************************");

	if(!pProgramInfo){
		return;
	}

	VOLOGI("ProgramID=%lu, Sel=%lu, Type=%lu, Name=%s,uStreamCount=%lu",
		pProgramInfo->uProgramID, pProgramInfo->uSelInfo, pProgramInfo->sProgramType, pProgramInfo->strProgramName, pProgramInfo->uStreamCount);

	for(VO_U32 index = 0; index < pProgramInfo->uStreamCount; index++)
	{
		PrintStreamInfo(pProgramInfo->ppStreamInfo[index]);
	}


	VOLOGI("****************************PrintProgramInfo-*********************************************");
}

/*big_endian to little_endian*/
typedef VO_U64 u64;
typedef VO_U32 u32;
typedef VO_U16 u16;
#define BSWAP_16(x) \
	(u16) ( ((((u16)(x)) & 0x00ff) << 8) | \
			((((u16)(x)) & 0xff00) >> 8) )

#define BSWAP_32(x) \
	(u32) ( ((((u32)(x)) & 0x000000ff) << 24) | \
			((((u32)(x)) & 0x0000ff00) << 8)  | \
			((((u32)(x)) & 0x00ff0000) >> 8)  | \
			((((u32)(x)) & 0xff000000) >> 24) )

#define BSWAP_64(x) \
	(u64) ( ((((u64)(x)) & 0x00000000000000ffll) << 56) | \
			((((u64)(x)) & 0x000000000000ff00ll) << 40) | \
			((((u64)(x)) & 0x0000000000ff0000ll) << 24) | \
			((((u64)(x)) & 0x00000000ff000000ll) << 8)  | \
			((((u64)(x)) & 0x000000ff00000000ll) >> 8)  | \
			((((u64)(x)) & 0x0000ff0000000000ll) >> 24) | \
			((((u64)(x)) & 0x00ff000000000000ll) >> 40) | \
			((((u64)(x)) & 0xff00000000000000ll) >> 56) )

/*the format show for time. 
	time is a millisecond*/
#define SECOND_DIGIT 10000000000ll
static void timeToSting(VO_U64 time, VO_CHAR *szTime)
{/*eg.time = 1362037895,080*/
	struct tm tm1;
	memset( szTime, 0x00, sizeof(szTime));
	VO_U32 mmsec = time/SECOND_DIGIT >0 ? time %1000 : 0;
	time_t time1 = time_t( time/SECOND_DIGIT >0 ? time /1000: time);
#ifndef WINCE
#ifdef WIN32
	tm1 = *localtime(&time1);
#else
	localtime_r(&time1, &tm1 );
#endif //WIN32
	sprintf( szTime, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d:%3.3lu [%lld]",
		tm1.tm_year+1900, tm1.tm_mon+1, tm1.tm_mday,
		tm1.tm_hour, tm1.tm_min,tm1.tm_sec,mmsec, time);
#else
	sprintf( szTime , "%lld" , time );
#endif //WINCE

}


static VO_BOOL CheckString( VO_CHAR * pData , VO_U32 nDataSize , const VO_CHAR *pDestString )
{
	VO_BOOL isChecked = VO_FALSE;
	if ( strstr( pData, pDestString) )
		isChecked = VO_TRUE;
	// #if WIN32
	// 	else if( wcsstr( (wchar_t*)ptr_buffer, L"<SmoothStreamingMedia" ) )
	// 		isISS = VO_TRUE;
	// #elif _LINUX
	else 
	{
		VO_CHAR *pBuffer1,*pBuffer2;
		pBuffer1 = pBuffer2 = pData;

		VO_U32 nSize = nDataSize > 200 ? 200: nDataSize;

		const VO_CHAR *pTag = pDestString;

		for(VO_U32 i =0 ; i< nSize; i++)
		{
			while( pTag[0] )
			{
				if( pBuffer1[0] != pTag[0] )
				{
					isChecked = VO_FALSE;
					break;
				}
				pBuffer1 += 2;
				pTag ++;
				isChecked = VO_TRUE;
			}
			if( isChecked )
				break;
			pTag = pDestString;
			pBuffer1 = ( pBuffer2 += 2 );
		} 
	}
	//#endif
	return isChecked;
}

// for string copy
static VO_TCHAR * DSStrCopy(VO_TCHAR **ppDest, VO_TCHAR *pSource )
{
	if( pSource == NULL )
		return NULL;
	VO_TCHAR *p = pSource;
	while( !(*p == 0 && *( p + 1 ) == 0) ) p += 2;
	
	VO_U32 len = ( (VO_U32)( p - pSource ) / 2);// vostrclen( pSource );

	*ppDest = new VO_TCHAR[len + 1];
	memset( *ppDest, 0, ( len + 1 ) * sizeof(VO_TCHAR));
	vostrcpy( *ppDest, pSource);
	return *ppDest;
}

inline VO_U32 RecordCostTime( VO_U32 &nPreTime, bool bBegin,const char *pLog, VO_U32 logRank = 5 )
{
	VO_U32 nTmp = voOS_GetSysTime();
	VO_U32 nGap = 0;
	if( bBegin )
	{
		nPreTime = nTmp;
	}
	else
	{
		nGap = nTmp - nPreTime;
		if(	pLog )
		{
			switch( logRank )
			{
			case 1:
			case 2:
			case 3:
			case 4:
				VOLOGI( "%s:%d ms",pLog, nGap );
				break;
			case 5:
			default:
				VOLOGR( "%s:%d ms",pLog, nGap );
				break;
			}
		}
		nPreTime = nTmp;
	}
	return nGap;
}

inline const char *TrackType2String(VO_SOURCE2_TRACK_TYPE  nTrackType)
{
	switch(nTrackType)
	{
	case VO_SOURCE2_TT_AUDIO:
		return "audio";
	case VO_SOURCE2_TT_VIDEO:
		return "video";
	case VO_SOURCE2_TT_SUBTITLE:
		return "subtitle";
	default:
		return "unknown";
	}
	return "unknown";
}


inline const char *ChunkType2String( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE  nChunkType)
{
	switch(nChunkType)
	{
	case VO_SOURCE2_ADAPTIVESTREAMING_AUDIO:
		return "\"audio\"";
	case VO_SOURCE2_ADAPTIVESTREAMING_VIDEO:
		return "\"video\"";
	case VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE:
		return "\"subtitle\"";
	case VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA:
		return "\"headdata\"";
	case VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX:
		return "\"SegmentIndex\"";
	case VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO:
		return "\"audio&video\"";
	case VO_SOURCE2_ADAPTIVESTREAMING_INITDATA:
		return "\"initdata\"";
	default:
		return "\"unknown\"";
	}
	return "\"unknown\"";
}

inline const char *ThreadType2String( THREADTYPE   nType)
{
	switch(nType)
	{
	case THREADTYPE_MEDIA_AUDIO:
		return "\"audio\"";
	case THREADTYPE_MEDIA_VIDEO:
		return "\"video\"";
	case THREADTYPE_MEDIA_SUBTITLE:
		return "\"subtitle\"";
	case THREADTYPE_MEDIA_MAX:
	default:
		return "\"unknown\"";
	}
	return "\"unknown\"";
}

#define One2Two(a) a, a

#define SpecialFlagCheck(nTrackType, uFlag, uTime, uRet) \
{ \
	if( VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT & uFlag ) \
	{ \
		VOLOGI(" %s meets VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT.(timestamp:%lld[0x%llx], Ret:%08x) ", TrackType2String(nTrackType), One2Two(uTime), uRet );\
	}\
	if( VO_SOURCE2_FLAG_SAMPLE_EOS & uFlag )\
	{\
		VOLOGI(" %s meets VO_SOURCE2_FLAG_SAMPLE_EOS.(timestamp:%lld[0x%llx], Ret:%08x) ", TrackType2String(nTrackType),One2Two(uTime), uRet );\
	}\
	if( VO_SOURCE2_FLAG_SAMPLE_FRAMESHOULDDROP & uFlag )\
	{\
		VOLOGI(" %s meets VO_SOURCE2_FLAG_SAMPLE_FRAMESHOULDDROP.(timestamp:%lld[0x%llx], Ret:%08x) ", TrackType2String(nTrackType), One2Two(uTime), uRet );\
	}\
	if( VO_SOURCE2_FLAG_SAMPLE_FORCE_FLUSH & uFlag )\
	{\
		VOLOGI(" %s meets VO_SOURCE2_FLAG_SAMPLE_FORCE_FLUSH.(timestamp:%lld[0x%llx], Ret:%08x) ", TrackType2String(nTrackType), One2Two(uTime), uRet );\
	}\
	if( VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE & uFlag )\
	{\
		VOLOGI(" %s meets VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE.(timestamp:%lld[0x%llx], Ret:%08x) ", TrackType2String(nTrackType), One2Two(uTime), uRet );\
	}\
	if( (VO_SOURCE2_FLAG_SAMPLE_KEYFRAME & uFlag) && ( nTrackType == VO_SOURCE2_TT_VIDEO ) )\
	{\
		VOLOGI(" %s meets VO_SOURCE2_FLAG_SAMPLE_KEYFRAME.(timestamp:%lld[0x%llx], Ret:%08x) ", TrackType2String(nTrackType), One2Two(uTime), uRet );\
	}\
	if(VO_SOURCE2_FLAG_SAMPLE_FRAMEDECODEONLY & uFlag)\
	{\
		VOLOGI(" %s meets VO_SOURCE2_FLAG_SAMPLE_FRAMEDECODEONLY.(timestamp:%lld[0x%llx], Ret:%08x) ", TrackType2String(nTrackType), One2Two(uTime), uRet );\
	}\
}
#ifndef _USE_BUFFERCOMMANDER
#define SKIPSPECIALSAMPLE(uFlag) \
{ \
	switch(uFlag) \
	{ \
	case VO_DATASOURCE_FLAG_CHUNK_BEGIN: \
	case VO_DATASOURCE_FLAG_CHUNK_END: \
		VOLOGI(" chunk meets Special Flag:0x%08x.Skip it. ", uFlag); \
		return VO_RET_SOURCE2_OK; \
	default: \
		; \
	} \
}
#endif

#ifdef _USE_BUFFERCOMMANDER
#define SpecialFlagCheck_DS(nTrackType, uFlag, uTime, uRet) \
{\
	if( VO_DATASOURCE_FLAG_CHUNK_BEGIN & uFlag )\
	{\
		VOLOGI(" chunk meets VO_DATASOURCE_FLAG_CHUNK_BEGIN.(timestamp:%lld[0x%llx], Ret:%08x) ", TrackType2String(nTrackType), One2Two(uTime), uRet );\
	}\
	if( VO_DATASOURCE_FLAG_CHUNK_END & uFlag )\
	{\
		VOLOGI(" chunk meets VO_DATASOURCE_FLAG_CHUNK_END.(timestamp:%lld[0x%llx], Ret:%08x) ", TrackType2String(nTrackType), One2Two(uTime), uRet );\
	}\
	if( VO_DATASOURCE_FLAG_FORCEOUTPUT & uFlag )\
	{\
	VOLOGI(" chunk meets VO_DATASOURCE_FLAG_FORCEOUTPUT.(timestamp:%lld[0x%llx], Ret:%08x) ", TrackType2String(nTrackType), One2Two(uTime), uRet );\
	}\
	if( VO_DATASOURCE_FLAG_CHUNK_SWITCH & uFlag )\
	{\
	VOLOGI(" chunk meets VO_DATASOURCE_FLAG_CHUNK_SWITCH.(timestamp:%lld[0x%llx], Ret:%08x) ", TrackType2String(nTrackType), One2Two(uTime), uRet );\
	}\
}
#endif

static VO_VOID DSSample2Source2Sample( VO_DATASOURCE_SAMPLE *pDSSample, VO_SOURCE2_SAMPLE **pSource2Sample )
{
	VO_SOURCE2_SAMPLE *pDest = *pSource2Sample;
	pDest->uTime		= pDSSample->uTime;
	pDest->uDuration	= pDSSample->uDuration;
	pDest->uFlag		= pDSSample->uFlag;
	pDest->pFlagData	= pDSSample->pFlagData;

	pDest->uSize		= pDSSample->uSize;
	pDest->pBuffer		= pDSSample->pBuffer;
	pDest->pReserve1	= pDSSample->pReserve1;
	pDest->pReserve2	= NULL;				//don`t expose it out
}


#if !defined __VO_WCHAR_T__
#define __VO_WCHAR_T__
typedef unsigned short vowchar_t;
#endif
#define BYTE_1_REP          0x80   
#define BYTE_2_REP          0x800 
#define BYTE_3_REP          0x10000 
#define BYTE_4_REP          0x200000 

#define SIGMASK_1_1         0x80
#define SIGMASK_2_1         0xe0
#define SIGMASK_3_1         0xf0
#define SIGMASK_4_1         0xf8
static int vowcslen(const vowchar_t* str)
{   
	int len = 0;
	while (*str != '\0') {
		str++;
		len++;
	}
	return len;
} 
static int VO_UnicodeToUTF8(const vowchar_t *szSrc, const int nSrcLen, char *strDst, const int nDstLen) 
{

	VOLOGR("Enter to VO_UnicodeToUTF8 file size: %d ", nDstLen);

	int is = 0, id = 0;
	const vowchar_t *ps = szSrc;
	unsigned char *pd = (unsigned char*)strDst;

	if (nDstLen <= 0)
		return 0;

	for (is = 0; is < nSrcLen; is++) {
		if (BYTE_1_REP > ps[is]) { /* 1 byte utf8 representation */
			if (id + 1 < nDstLen) {
				pd[id++] = (unsigned char)ps[is];
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_2_REP > ps[is]) {
			if (id + 2 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 6 | 0xc0);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_3_REP > ps[is]) { /* 3 byte utf8 representation */
			if (id + 3 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 12 | 0xe0);
				pd[id++] = (unsigned char)(((ps[is] >> 6)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_4_REP > ps[is]) { /* 4 byte utf8 representation */
			if (id + 4 < nDstLen) {
				pd[id++] = (unsigned char)(VO_U32(ps[is]) >> 18 | 0xf0);
				pd[id++] = (unsigned char)(((ps[is] >> 12)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)(((ps[is] >> 6)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} /* @todo Handle surrogate pairs */
	}

	pd[id] = '\0'; /* Terminate string */

	return id; /* This value is in bytes */
}

static VO_VOID ConvertUnicodeToUTF8( VO_CHAR *pSourceBuffer,  VO_U32 uSize, VO_CHAR **ppDestBuffer, VO_U32 *pDestSize )
{
	
	if( *ppDestBuffer ){
		delete[] *ppDestBuffer;
		*ppDestBuffer = NULL;
	}
	
	VO_CHAR *pDBuf = *ppDestBuffer = new VO_CHAR[uSize+1];
	memcpy(pDBuf, (VO_CHAR*)pSourceBuffer, uSize);
	pDBuf[uSize] = '\0';

	if( pDBuf[0] == (VO_CHAR)0xff && pDBuf[1] == (VO_CHAR)0xfe)
	{
#if defined _WIN32

		int size = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pDBuf, -1, NULL, NULL, NULL, NULL );

		VO_CHAR *content_temp = NULL;
		content_temp = new VO_CHAR[ size + 1 ];
		memset( content_temp , 0 , size + 1 );

		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pDBuf, -1, content_temp, size, NULL, NULL );
		*pDestSize = size; 

#else
		VO_CHAR *content_temp = new VO_CHAR[ uSize + 1 ];
		memset( content_temp , 0 , uSize + 1 );

		int SrcLen = vowcslen((vowchar_t *)pDBuf);

		VO_UnicodeToUTF8((vowchar_t *)pDBuf, SrcLen, content_temp, uSize);
		*pDestSize = uSize; 

#endif
		delete []pDBuf;
		pDBuf = content_temp;
	}

}

#ifdef _VOLOG_INFO 
#define VODS_VOLOGI VOLOGI 
#else  
#ifdef _VODSLOG_INFO
#define VODS_VOLOGI(fmt, args...) \
{ 	\
	char szLog[1024]; \
	snprintf(szLog, 1024, "@@@VODSLOG Info %s  %s  %d  " fmt "\n", strrchr (__FILE__, '/'), __FUNCTION__, __LINE__, ##args); \
	printf(szLog); \
}
#else
#define VODS_VOLOGI(fmt, ...) 
#endif
#endif 

#ifdef _VOLOG_ERROR 
#define VODS_VOLOGE VOLOGE 
#else  
#ifdef _VODSLOG_ERROR
#define VODS_VOLOGE(fmt, args...) \
{	\
	char szLog[1024]; \
	snprintf(szLog, 1024, "@@@VODSLOG Error %s  %s  %d  " fmt "\n", strrchr (__FILE__, '/'), __FUNCTION__, __LINE__, ##args); \
	printf(szLog); \
}
#else
#define VODS_VOLOGE(fmt, ...) 
#endif
#endif 

#ifdef _VOLOG_WARNING 
#define VODS_VOLOGW VOLOGW 
#else  
#ifdef _VODSLOG_WARNING 
#define VODS_VOLOGW(fmt, args...) \
{ \
	char szLog[1024]; \
	snprintf(szLog, 1024, "@@@VODSLOG Warning %s  %s  %d  " fmt "\n", strrchr (__FILE__, '/'), __FUNCTION__, __LINE__, ##args); \
	printf(szLog); \
}
#else
#define VODS_VOLOGW(fmt, ...) 
#endif
#endif 

#define _CHECKEDINTERVALTIME 1000 * 5
class CIntervalPrintf
{
public:
	CIntervalPrintf(){ m_nPreTime = 0;}
	bool operator()(const VO_CHAR *pFmt, ...)
	{
		if( voOS_GetSysTime() - m_nPreTime > _CHECKEDINTERVALTIME )
		{
			VO_CHAR 			szVOLOGTempString[1024]; 
#if defined _IOS || defined _MAC_OS
			__builtin_va_list  list;
			__builtin_va_start (list , pFmt);
			vsnprintf (szVOLOGTempString , 1024, pFmt , list );
			__builtin_va_end( list );
#else
			va_list list;
			va_start (list , pFmt);
			vsprintf (szVOLOGTempString , pFmt , list );
			va_end( list );
#endif			
			VOLOGI("%s. It will be showed after %d ms",szVOLOGTempString, _CHECKEDINTERVALTIME );

			m_nPreTime = voOS_GetSysTime();
			return true;
		}
		return false;
	}
protected:
	VO_U32 m_nPreTime;
};

// End define
#ifdef _VONAMESPACE
}
#else
#endif /* _VONAMESPACE */

#endif