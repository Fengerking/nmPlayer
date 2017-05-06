#include "voChannelItemThreadHLS.h"
#include "voOSFunc.h"
#include "CSourceIOUtility.h"
#include "voLog.h"
#include "voAdaptiveStreamingClassFactory.h"
#include "voSmartPointor.hpp"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voChannelItemThread"
#endif

#define READDATASIZE  300800//188*1600

#define _CheckFileFormatSeveralTimes
//#define _use_OldToProcessChunk
//#define _DUMP_CHUNKS

voChannelItemThreadHLS::voChannelItemThreadHLS(void)
:m_uCustomerTagChunkID(-1)
,m_pCustomerTagMap(NULL)
,m_nCustomerTagMapSize(16)
,m_nCustomerTagCount(0)
{
}

voChannelItemThreadHLS::~voChannelItemThreadHLS(void)
{
	if(m_pCustomerTagMap)
	{
		delete []m_pCustomerTagMap;
		m_pCustomerTagMap = NULL;
	}
		
}
VO_U32 voChannelItemThreadHLS::ToGetItem( VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item, VO_U32 nErrCode )
{
	VO_BOOL bFlag = VO_BOOL(m_ptr_curitem->Type != VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA 
							&& ptr_item->uFlag != VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTION);

#ifdef _USE_BUFFERCOMMANDER
	return GetItem_IV( ptr_item, bFlag, nErrCode );
#else
	return GetItem_III( ptr_item, bFlag, nErrCode );
#endif
}

VO_U32 voChannelItemThreadHLS::GetHeadData( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE  Type ) 
{
	if( Type == VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE )
		return m_pFileParser->CreateTrackInfo2( VO_SOURCE2_TT_SUBTITLE , NULL);
	return VO_RET_SOURCE2_FAIL;
}

VO_U32 voChannelItemThreadHLS::ToProcessChunk(VO_HANDLE hIO, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, VO_U32 *nReadsize)
{
	VO_BOOL bHasGotData = VO_FALSE;
	VO_U32 readed = 0;
	VO_U32 readsize = READDATASIZE;
	VO_BOOL bFileFormatChecked = VO_FALSE;
	VO_BOOL	 bIsChunkEnd = VO_FALSE;

	VO_U32 ret = VO_RET_SOURCE2_OK;
	VO_PBYTE pTmpBuffer = NULL;
	VO_U32 uOffset = 0;

	VO_U32 max_bitrate = m_ptr_info->pInfo->GetMaxDownloadBitrate();
	VO_BOOL	min_bitrate_playback = m_ptr_info->pInfo->IsMinBitratePlaying();
	VO_BOOL ba_enable = m_ptr_info->pInfo->IsBAEnable();
	VOLOGI("min bitrate playback is %d,ba enable is %d",min_bitrate_playback,ba_enable);
	VOLOGI("Before BeginChunkDRM");
	ret = BeginChunkDRM();
	VOLOGI("After BeginChunkDRM.Ret:0x%08x", ret);
#ifdef _DUMP_CHUNKS
	char path[512];
	memset( path , 0 , sizeof(path) );
	sprintf( path , "/sdcard/hlsdump/%s.dat" , pChunk->szUrl );
	//sprintf( path , "C:\\Users\\zhang_xin\\Desktop\\hlsdump\\%s.dat" , pChunk->szUrl );
	FILE * fp = fopen( path , "wb+" );
#endif
	if( ret == VO_RET_SOURCE2_OK )
	{
		readsize = READDATASIZE;
		SendEventDownloadStartColletor(pChunk);

		while (ret == VO_RET_SOURCE2_OK && readsize == READDATASIZE && !m_is_stop )
		{
			if(m_ptr_info->pInfo->IsUpdateUrlOn(m_ptr_info->type) == VO_TRUE)
			{
				VOLOGW("Update url during processChunk happen!");
				m_nReadedSize = readed;
				return VO_RET_SOURCE2_UPDATEURL;
			}
			readsize = READDATASIZE;

			VO_SOURCE2_IO_API *pIOApi = m_ptr_info->pInfo->GetIOPtr();

			voSmartPointor< VO_CHAR > buffer( new VO_CHAR[READDATASIZE + 752] ); // suggested by Chenhui,temp add 752 for possible case,need modify it later
			pTmpBuffer = (VO_PBYTE) buffer;

			VO_U32 start = voOS_GetSysTime();
			VOLOGI("Before ReadFixedSize");
			if(ba_enable && !min_bitrate_playback && (pChunk->Type == VO_SOURCE2_ADAPTIVESTREAMING_VIDEO || pChunk->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO))
			{
		//		ret = ReadFixedSize( pIOApi , hIO , (VO_PBYTE)buffer , &readsize , &m_is_stop,(pChunk->ullDuration / ( pChunk->ullTimeScale / 1000. )) + 2000.);	
				ret = ReadFixedSize(m_ptr_info->pInfo->GetDownloadDataColletor(), pIOApi, hIO , (VO_PBYTE)buffer , &readsize, &m_is_stop,( pChunk->ullDuration / ( pChunk->ullTimeScale / 1000. ) + 2000 ), m_ptr_info->pInfo->GetIOEventCallback());	
			}
			else
			{
				ret = ReadFixedSize(m_ptr_info->pInfo->GetDownloadDataColletor(), pIOApi , hIO , (VO_PBYTE)buffer , &readsize , &m_is_stop);
			}
			VOLOGI("After ReadFixedSize,size is %d,ret is 0x%08x",readsize,ret);
			if(ret == VO_SOURCE2_IO_EOS)
			{
				bIsChunkEnd = VO_TRUE;
				ret = VO_SOURCE2_IO_OK;
			}
			VO_U32 costtime = voOS_GetSysTime() - start;
#ifdef _DUMP_CHUNKS
			if( fp )
				fwrite( buffer , 1 , readsize , fp );
#endif
			VO_BOOL bIsBuffering = m_ptr_info->pInfo->IsBuffering();
			VO_U32 uMinBufferDuration = m_ptr_info->pInfo->GetMinBufferDuration();
			VOLOGI("MinBufferDuration is %d", uMinBufferDuration);
			if(VO_FALSE == bIsBuffering && uMinBufferDuration >= 5000)   // 5000ms for now
			{
				VO_U32 shouldcost = readsize * 1000 / max_bitrate;
				if(costtime < shouldcost)
				{
					VO_U32 nSleepTime = (shouldcost - costtime) / 2;
					VOLOGI("Before sleep %d: cost time %d, should cost %d, RS %d, MB %d", nSleepTime, costtime, shouldcost, readsize, max_bitrate);
					if(nSleepTime <= 20)
						voOS_Sleep(nSleepTime);
					else
						voOS_SleepExitable(nSleepTime, &m_is_stop);
					VOLOGI("After sleep");
				}
			}

			if( ret != VO_SOURCE2_IO_OK )
			{
				if(m_is_stop)
				{
					ret = VO_RET_SOURCE2_OK;	//it is user abort,so set return code to VO_RET_SOURCE2_OK
				}
				else
				{
					if(ret == VO_DATASOURCE_RET_DOWNLOAD_SLOW)
					{
						VOLOGW("Download speed is slow,so abort it");
						break;
					}
					else
					{
						VOLOGW("-Download Fail! %s,return is %d", pChunk->szUrl,ret);
						if( !bHasGotData )
							ret = VO_RET_SOURCE2_CHUNKDROPPED;
						else
							ret = VO_RET_SOURCE2_CHUNKPARTERROR;
					}
				}
				break;
			}
			bHasGotData = VO_TRUE;

			readed = readed + readsize;
			if(readed <= m_nReadedSize)
			{
				VOLOGW("Duplicate data,ignore it!");
				continue;
			}

			VO_U32 dec_size = readsize;
			VOLOGI("Before first ProcessChunkDRM_II");
			ret = ProcessChunkDRM_II( (VO_PBYTE)buffer ,uOffset, &dec_size, bIsChunkEnd );
			VOLOGI("Afterst ProcessChunkDRM_II");
			uOffset += readsize;
			if( ret != VO_RET_SOURCE2_OK )
			{
				VOLOGE( "ProcessChunkDRM_II Fail! 0x%08x" , ret );
				ret = VO_RET_SOURCE2_DRMERROR;
				//SendDrmWarning(VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DRMERROR,pChunk,ret);
				break;
			}

			if( !bFileFormatChecked )
			{	
				pTmpBuffer = buffer;
				VOLOGI("Before CreateFileParser_II");
				ret = CreateFileParser_II( pChunk , &pTmpBuffer , &dec_size, VO_TRUE );
				VOLOGI("After CreateFileParser_II");
				if( ret == VO_RET_SOURCE2_INPUTDATASMALL )
				{
					ret = VO_RET_SOURCE2_OK;
					continue;
				}
				else if ( ret != VO_RET_SOURCE2_OK )
				{
					if((ret & VO_ERR_LICENSE_ERROR) == VO_ERR_LICENSE_ERROR)
					{
						VOLOGE("CreateFileParser license check error,ret = 0x%08x",ret);
						SendLicenseCheckError(VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_VOLIBLICENSEERROR, ret);
					}
					VOLOGW("-CreateFileParser_II Fail! %s", pChunk->szUrl);
					break;
				}

				if( m_pFileParser )
				{
					m_pFileParser->SetParam( VO_PARSER_OT_TS_WITHOUT_TIMESTAMP_OFFSET , 0 );
				}

				bFileFormatChecked = VO_TRUE;
			}
			VOLOGI("Before ParseData");
			ret = ParseData( pTmpBuffer , dec_size , bIsChunkEnd , pChunk );
			VOLOGI("After ParseData");
			if( ret != VO_RET_SOURCE2_OK )
			{
				VOLOGW("-ParseData Fail! %s", pChunk->szUrl);
				break;
			}
			//HLS BA, previous should not download complete.
			if( pChunk->uFlag & VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTION ) 
			{
				if( VO_SOURCE2_ADAPTIVESTREAMING_VIDEO == pChunk->Type && m_video_count > 30)
				{
					ret = VO_RET_SOURCE2_OK;
					break;
				}
				if( VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO == pChunk->Type && m_video_count > 30 && m_audio_count > 30)
				{
					ret = VO_RET_SOURCE2_OK;
					break;
				}
			}
		}
			if(m_pFileParser && pChunk->Type == VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO)
				m_pFileParser->OnChunkEnd();
	}
	else
	{
		VODS_VOLOGW( "-Start DRM Fail! "  );
		ret = VO_RET_SOURCE2_DRMERROR;
		//SendDrmWarning(VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DRMERROR,pChunk,ret);
	}
#ifdef _DUMP_CHUNKS
	if( fp )
		fclose( fp );
#endif
	VO_U64 uDownloadSize = 0;
	SendEventDownloadEndColletor(pChunk, uDownloadSize);
	if(uDownloadSize == 0)
		uDownloadSize = readed;
	EndChunkDRM_II();
	*nReadsize = uDownloadSize;
	return ret;
}

VO_U32 voChannelItemThreadHLS::SendCustomEvent( VO_U16 nOutputType, VO_PTR pData) 
{
	if( VO_SOURCE2_TT_CUSTOMTIMEDTAG != nOutputType )
		return VO_DATASOURCE_RET_OK;
	VO_SOURCE2_CUSTOMERTAG_TIMEDTAG* ptr_buffer = (VO_SOURCE2_CUSTOMERTAG_TIMEDTAG*)pData;
	VOLOGI("Customer tag ID3 timestamp before is %lld",ptr_buffer->ullTimeStamp);
	VO_U32 ret = CheckCustomTag(ptr_buffer->ullTimeStamp);
	if(ret == VO_DATASOURCE_RET_SHOULDSKIP)
		return ret;
	_SAMPLE pSample;
	pSample.uTime = ptr_buffer->ullTimeStamp;
	pSample.uSize = 0;
	pSample.uFlag = 0;
	pSample.pBuffer = NULL;
	m_ptr_info->pInfo->GetSampleCallback()->SendData(m_ptr_info->pInfo->GetSampleCallback()->pUserData , nOutputType , &pSample);
	ptr_buffer->ullTimeStamp = pSample.uTime;
	
	VOLOGI("Customer tag ID3 timestamp after is %lld",ptr_buffer->ullTimeStamp);

	VO_SOURCE2_IO_API *pIO = m_ptr_info->pInfo->GetIOPtr();
	VO_SOURCE2_EVENTCALLBACK *pEventCallback = m_ptr_info->pInfo->GetEventCallback();
	pEventCallback->SendEvent( pEventCallback->pUserData , VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CUSTOMTAG , VO_SOURCE2_CUSTOMERTAGID_TIMEDTAG,(VO_U32)pData);
	return VO_DATASOURCE_RET_SHOULDSKIP;
}

VO_VOID voChannelItemThreadHLS::DoDrop(VO_U32 &nFlag)
{
	nFlag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
}

VO_U32 voChannelItemThreadHLS::CheckCustomTag(VO_U64 nTimestamp)
{
	if(!m_pCustomerTagMap)
	{
		m_pCustomerTagMap = new VO_U64[m_nCustomerTagMapSize];
		if(!m_pCustomerTagMap)
		{
			VOLOGE("Malloc customer tag map error");
			return VO_RET_SOURCE2_EMPTYPOINTOR;
		}
		memset(m_pCustomerTagMap, 0, sizeof(VO_U64) *m_nCustomerTagMapSize );
	}
	
	if(m_uCustomerTagChunkID == _VODS_INT64_MAX)
	{
		m_uCustomerTagChunkID = m_uChunkID;
		m_pCustomerTagMap[m_nCustomerTagCount++] = nTimestamp;
	}
	else
	{
		VOLOGI("Customer tag chunk id is %lld,previous is %lld",m_uChunkID,m_uCustomerTagChunkID);
		if(m_uCustomerTagChunkID != m_uChunkID)
		{
			ResetCustomerTagMap();
			m_uCustomerTagChunkID = m_uChunkID;
			m_pCustomerTagMap[m_nCustomerTagCount++] = nTimestamp;
		}
		else
		{
			for(int i = 0; i < m_nCustomerTagCount; i++)
			{
				VO_S64 delta = (nTimestamp > m_pCustomerTagMap[i]) ? (nTimestamp - m_pCustomerTagMap[i]) : (m_pCustomerTagMap[i] - nTimestamp);
				VOLOGI("nTimestamp is %lld, tag map timestamp is %lld",nTimestamp,m_pCustomerTagMap[i]);
				if(delta < 5)
				{
					VOLOGW("The same Chunk`s same Customer tag,nTimestamp is %lld,tag map timestamp is %lld",nTimestamp,m_pCustomerTagMap[i]);
					return VO_DATASOURCE_RET_SHOULDSKIP;
				}
			}
			m_pCustomerTagMap[m_nCustomerTagCount++] = nTimestamp;
			if(m_nCustomerTagCount > m_nCustomerTagMapSize)
			{
				VOLOGW("tag map is overflow,don`t check this tag");
				return VO_RET_SOURCE2_ERRORDATA;
			}
		}
	}
	return VO_DATASOURCE_RET_OK;
}

VO_U32 voChannelItemThreadHLS::ResetCustomerTagMap()
{
	if(!m_pCustomerTagMap)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	VOLOGI("Customer tag reset!");
	memset(m_pCustomerTagMap, 0, sizeof(VO_U64) *m_nCustomerTagMapSize );
	m_nCustomerTagCount = 0;
	m_uCustomerTagChunkID = -1;
	return VO_RET_SOURCE2_OK;
}