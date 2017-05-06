#include "voChannelItemThreadDASH.h"
#include "voOSFunc.h"
#include "CSourceIOUtility.h"
#include "voLog.h"
#include "voAdaptiveStreamingClassFactory.h"
#include "voSmartPointor.hpp"
#include "voToolUtility.h"



#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voChannelItemThreadDASH"
#endif

#define ONEREADSIZE 10240
#define MAXREADSIZE 10485760

voChannelItemThreadDASH::voChannelItemThreadDASH(void)
:voChannelItemThread()
{
}

voChannelItemThreadDASH::~voChannelItemThreadDASH(void)
{
}


VO_VOID voChannelItemThreadDASH::Stop()
{
	voChannelItemThread::Stop();
}

VO_U32 voChannelItemThreadDASH::GetHeadData(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE Type)
{
	if( Type == VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE )
		return m_pFileParser->CreateTrackInfo2( VO_SOURCE2_TT_SUBTITLE , NULL);
	m_pFileParser->SetDrmApi(m_ptr_info->pInfo->GetDRMPtr( m_ptr_info->type ));
	return VO_RET_SOURCE2_OK;
}

VO_U32 voChannelItemThreadDASH::ToProcessChunk(VO_HANDLE hIO, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, VO_U32 *nReadsize)
{
	if(pChunk->Type != VO_SOURCE2_ADAPTIVESTREAMING_INITDATA)
	{
		return voChannelItemThread::ToProcessChunk(hIO, pChunk, nReadsize);
	}
	else
	{
		VO_U32 ret = VO_RET_SOURCE2_OK;
		VO_U32 readsize = 0;
		VO_U32 uOffset = 0;
		VO_U32 readed = 0;
		VO_BOOL bIsChunkEnd = VO_FALSE;
		VO_BOOL bFileFormatChecked = VO_FALSE;
		VO_PBYTE pTmpBuffer = NULL;
		
		VO_U32 max_bitrate = m_ptr_info->pInfo->GetMaxDownloadBitrate();
		VO_BOOL	min_bitrate_playback = m_ptr_info->pInfo->IsMinBitratePlaying();
		VO_BOOL ba_enable = m_ptr_info->pInfo->IsBAEnable();
		VOLOGI("min bitrate playback is %d,ba enable is %d",min_bitrate_playback,ba_enable);

		ret = BeginChunkDRM();
		if( ret == VO_RET_SOURCE2_OK )
		{
			readsize = ONEREADSIZE;
			while( !m_is_stop && readed < MAXREADSIZE)
			{
				VO_SOURCE2_IO_API *pIOApi = m_ptr_info->pInfo->GetIOPtr();
				voSmartPointor<VO_CHAR> buffer(new VO_CHAR[ONEREADSIZE + 1]);
				memset(buffer,0,(ONEREADSIZE + 1)*sizeof(VO_CHAR));
				pTmpBuffer = (VO_PBYTE) buffer;
				if(ba_enable && !min_bitrate_playback)
				{
					ret = ReadFixedSize(pIOApi, hIO, (VO_PBYTE)buffer, &readsize, &m_is_stop,(pChunk->ullDuration / ( pChunk->ullTimeScale / 1000. )) + 2000.);
				}
				else
				{
					ret = ReadFixedSize(pIOApi, hIO, (VO_PBYTE)buffer, &readsize, &m_is_stop);
				}
				if(ret == VO_SOURCE2_IO_EOS)
				{
					bIsChunkEnd = VO_TRUE;
					ret = VO_SOURCE2_IO_OK;
				}
				if(ret != VO_SOURCE2_IO_OK)
				{
					if(m_is_stop)
					{
						ret = VO_RET_SOURCE2_OK; //it is user abort ,so set return code to VO_RET_SOURCE2_OK
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
							ret = VO_RET_SOURCE2_CHUNKDROPPED;
							break;
						}
					}
				}
				readed = readed + readsize;
				
				VO_U32 dec_size = readsize;
				ret = ProcessChunkDRM_II((VO_PBYTE)buffer, uOffset, &dec_size, bIsChunkEnd);
				uOffset += readsize;
				if(ret != VO_RET_SOURCE2_OK)
				{
					VOLOGW( "ProcessChunkDRM_II Fail! 0x%08x" , ret );
					ret = VO_RET_SOURCE2_DRMERROR;
				//	SendDrmWarning(VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DRMERROR,pChunk,ret);
					break;
				}
				if(!bFileFormatChecked)
				{
					pTmpBuffer = buffer;
					ret = CreateFileParser_II(pChunk, &pTmpBuffer,&dec_size,VO_TRUE);
					if(ret == VO_RET_SOURCE2_INPUTDATASMALL)
					{
						VOLOGW("CreateFileParser return input small");
						ret = VO_RET_SOURCE2_OK;
						continue;
					}
					else if(ret != VO_RET_SOURCE2_OK)
					{
						if((ret & VO_ERR_LICENSE_ERROR) == VO_ERR_LICENSE_ERROR)
						{
							VOLOGE("CreateFileParser license check error,ret = 0x%08x",ret);
							SendLicenseCheckError(VO_SOURCE2_ADAPTIVESTREAMING_ERROREVENT_STREAMING_VOLIBLICENSEERROR, ret);
						}
						VOLOGW("-CreateFileParser_II Fail! %s", pChunk->szUrl);
						break;
					}
					bFileFormatChecked = VO_TRUE;
				}
				ret = ParseData(pTmpBuffer, dec_size, bIsChunkEnd, pChunk);
				if(ret != VO_RET_SOURCE2_OK)
				{
					if(ret == VO_RET_SOURCE2_INPUTDATASMALL)
					{
						VOLOGW("ParseData input data small,need more data");
						ret = VO_RET_SOURCE2_OK;
						continue;
					}
					else
					{
						VOLOGW("-ParseData Fail! %s", pChunk->szUrl);
						break;
					}
				}
				else
				{
					VOLOGI("Parse init data OK!");
					break;
				}
			}
		}
		else
		{
			VOLOGW( "-Start DRM Fail! "  );
			ret = VO_RET_SOURCE2_DRMERROR;
		//	SendDrmWarning(VO_SOURCE2_ADAPTIVESTREAMING_WARNINGEVENT_CHUNK_DRMERROR,pChunk,ret);
		}
		EndChunkDRM_II();
		*nReadsize = readed;
		if(readed >= MAXREADSIZE)
			VOLOGW("readed data is over MAXREADSIZE");
		return ret;
	}
}