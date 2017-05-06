#include "voAdaptiveStreamingFileParser_aac.h"
#include "voAAC2.h"
#include "voDSType.h"
#include "voLog.h"
#include "voProgramInfo.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voAdaptiveStreamingFileParser_pa"
#endif

typedef VO_S32 ( VO_API *pvoGetSource2ParserAPI)(VO_SOURCE2_API* pParser);

voAdaptiveStreamingFileParser_pa::voAdaptiveStreamingFileParser_pa(VO_SOURCE2_SAMPLECALLBACK * pCallback, FileFormat ff, VO_SOURCE2_LIB_FUNC * pLibOp /* = 0 */)
:voAdaptiveStreamingFileParser( pCallback )
,m_ff(ff)
,m_pCurBuf(0)
,m_lasttimestamp(0)
,m_pTrackInfo(NULL)
{
	memset( &m_Api , 0 , sizeof(VO_SOURCE2_API) );

	if( pLibOp )
		SetLibOperator( ( VO_LIB_OPERATOR* )pLibOp );

	vostrcpy(m_szDllFile, _T("voAudioFR"));
	if(FileFormat_AAC == ff)
		vostrcpy(m_szAPIName, _T("voGetSource2AACAPI"));
	else if(FileFormat_MP3 == ff)
		vostrcpy(m_szAPIName, _T("voGetPushAudioAPI"));
	else
	{
		VOLOGE("adaptive streaming pure audio file parser just support AAC and MP3 currently");
	}

	ResetTrackInfo();
}

voAdaptiveStreamingFileParser_pa::~voAdaptiveStreamingFileParser_pa()
{
#ifdef _new_programinfo
	ReleaseTrackInfoOP_T( m_pTrackInfo );
	m_pTrackInfo = NULL;
#else
	delete m_pTrackInfo;
	m_pTrackInfo = NULL;
#endif
	FreeParser();
}

VO_U32 voAdaptiveStreamingFileParser_pa::LoadParser()
{
	if( LoadLib(NULL) == 0 )
		return VO_RET_SOURCE2_FAIL;
	VO_U32 ret = 0;
	pvoGetSource2ParserAPI pAPI = (pvoGetSource2ParserAPI) m_pAPIEntry;
	pAPI( &m_Api );

	if( m_Api.Init )
	{
		VO_SOURCE2_INITPARAM        init_param;
		VO_SOURCE2_SAMPLECALLBACK   sample_callback;


		sample_callback.pUserData = this;
		sample_callback.SendData = OnSample;
		init_param.uFlag = VO_PID_SOURCE2_SAMPLECALLBACK;
		init_param.pInitParam = &sample_callback;
		init_param.strWorkPath = m_pWorkPath;
		ret = m_Api.Init(&m_hModule, NULL, VO_PID_SOURCE2_SAMPLECALLBACK, &init_param);
		if(ret == 0)
		{
			ret = m_Api.Open( m_hModule );
		}
	}
	else
	{
		ret = VO_RET_SOURCE2_FAIL;
	}
	return ret;
}

VO_U32 voAdaptiveStreamingFileParser_pa::FreeParser()
{
	if( m_Api.Init && m_hModule )
	{
		m_Api.Close( m_hModule );
		m_Api.Uninit( m_hModule );
		m_hModule = 0;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 voAdaptiveStreamingFileParser_pa::Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer )
{
	if( m_Api.Init && m_hModule )
	{
		VO_SOURCE2_SAMPLE sample;
		memset( &sample , 0 , sizeof(VO_SOURCE2_SAMPLE) );

		m_pCurBuf = pBuffer;

		VO_PBYTE ptr_buffer = pBuffer->pBuffer;
		VO_U32 sendsize = 0;

		while( sendsize < pBuffer->uSize )
		{
			sample.pBuffer = pBuffer->pBuffer + sendsize;
			
			VO_U32 tosend = pBuffer->uSize - sendsize < 16 * 1024 ? pBuffer->uSize - sendsize : 16 * 1024;

			sample.uSize = tosend;

			sendsize = sendsize + 16 * 1024;

			m_Api.SendBuffer( m_hModule , sample );
		}

		return VO_RET_SOURCE2_OK;
	}

	return VO_RET_SOURCE2_EMPTYPOINTOR;
}

VO_S32 voAdaptiveStreamingFileParser_pa::OnSample(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{
	voAdaptiveStreamingFileParser_pa * ptr_obj = (voAdaptiveStreamingFileParser_pa *)pUserData;
	_SAMPLE    sample = {0}; 
	memset( &sample, 0x00, sizeof( _SAMPLE ) );
	sample.uFPTrackID = 0;
	sample.uASTrackID = ptr_obj->m_uASTrackID;

	VO_BOOL *pNew = NULL;
		pNew = &ptr_obj->m_new_audio;
	if( nOutputType == VO_SOURCE2_TT_AUDIO )
	{	
		//pNew = &ptr_obj->m_new_audio;

		VO_SOURCE2_SAMPLE * ptr_sample = ( VO_SOURCE2_SAMPLE * )pData;
		sample.pBuffer = ptr_sample->pBuffer;
		sample.pFlagData = ptr_sample->pFlagData;
		sample.pReserve1 = ptr_sample->pReserve1;
		sample.pReserve2 = ptr_sample->pReserve2;
		sample.uDuration = ptr_sample->uDuration;
		sample.uFlag = ptr_sample->uFlag;
		sample.uSize = ptr_sample->uSize;
		sample.uTime = ptr_sample->uTime;
		/*AAC gives a special ID by controller*/

		sample.uTime = ptr_obj->m_lasttimestamp = ptr_sample->uTime = ptr_sample->uTime + ptr_obj->m_offset;
		VOLOGR("sample.uTime:%lld,ptr_sample->uTime:%lld,ptr_obj->m_offset:%lld", sample.uTime, ptr_sample->uTime, ptr_obj->m_offset );


		/*new format, it should be held until the next sample.*/
		if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT )
		{
			VO_SOURCE2_TRACK_INFO * ptr_info = ( VO_SOURCE2_TRACK_INFO * )ptr_sample->pFlagData;

			_TRACK_INFO *pTrackInfo = NULL;
			CopyTrackInfoOP_EX2( ptr_info, &pTrackInfo );
			pTrackInfo->uASTrackID = ptr_obj->m_uASTrackID;
			pTrackInfo->uTrackType = VO_SOURCE2_TT_AUDIO;
			ptr_obj->StoreTrackInfo( VO_SOURCE2_TT_AUDIO, pTrackInfo );
			ReleaseTrackInfoOP_T( pTrackInfo );
			
			ptr_obj->m_new_audio = VO_TRUE;
			return VO_RET_SOURCE2_OK;
		}
	}
	else if( nOutputType == VO_SOURCE2_TT_HINT )
	{
		ID3Frame* pID3Frame = (ID3Frame*)pData;

		if(pID3Frame != NULL)
		{
			switch(pID3Frame->nSubHeadFlag)
			{
			case SUBHEAD_ID3_PRIV:
				{
					PrivStruct *priv = (PrivStruct*)pID3Frame->pSubStruct;

					if( strcmp((VO_CHAR*)priv->pOwnerIdentifier, "com.apple.streaming.transportStreamTimestamp") != 0 )
						return VO_RET_SOURCE2_OK;
					if( ptr_obj->m_offset == _VODS_INT64_MAX )
					{
						ptr_obj->m_offset = 0;

						ptr_obj->m_offset = ((VO_U64)( priv->pPrivateData[0] )) << 56;
						ptr_obj->m_offset |= ((VO_U64)( priv->pPrivateData[1] )) << 48;
						ptr_obj->m_offset |= ((VO_U64)( priv->pPrivateData[2] )) << 40;
						ptr_obj->m_offset |= ((VO_U64)( priv->pPrivateData[3] )) << 32;
						ptr_obj->m_offset |= ((VO_U64)( priv->pPrivateData[4] )) << 24;
						ptr_obj->m_offset |= ((VO_U64)( priv->pPrivateData[5] )) << 16;
						ptr_obj->m_offset |= ((VO_U64)( priv->pPrivateData[6] )) << 8;
						ptr_obj->m_offset |= ((VO_U64)( priv->pPrivateData[7] )) << 0;

						ptr_obj->m_offset = ptr_obj->m_offset & 0x1FFFFFFFFll;
						ptr_obj->m_offset /= 90; //90,000 / 1000
						ptr_obj->m_lasttimestamp = ptr_obj->m_offset;

						VOLOGI(" First AAC TimeStamp: %lld ", ptr_obj->m_offset );
					}
					return VO_RET_SOURCE2_OK;
				}
				break;
			case SUBHEAD_ID3_APIC:
				{
					ApicStruct *priv = (ApicStruct*)pID3Frame->pSubStruct;

					sample.uFlag = VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE;
					sample.uSize = priv->nPictureDataLength;
					if( sample.uSize > 0 )
						sample.pBuffer = priv->pPictureData;
					sample.uTime= ptr_obj->m_lasttimestamp;
				}      
				break;
			default:
				return VO_RET_SOURCE2_NOIMPLEMENT;
			}//endof switch(pID3Frame->nSubHead)
		}
		else
			return VO_RET_SOURCE2_EMPTYPOINTOR;//endof if(pID3Frame != NULL)
	}//endof else if( nOutputType == VO_SOURCE2_TT_HINT )
	

/*ATTENTION£ºpure audio should must first send its trackinfo for sending video track unavailible.
the first sample for AAC maybe a hit sample, so it can not be sent trackinfo because it is not a audio/video/subtitle. 
*/
#if 1
	ptr_obj->TryToSendTrackInfo( VO_SOURCE2_TT_AUDIO, sample.uTime, sample.uFPTrackID );
#else
	if( pNew && *pNew )
	{
		ptr_obj->SendTrackInfoList(sample.uTime > 1 ? 0 : sample.uTime -1);
		ptr_obj->SendMediaInfo( VO_SOURCE2_TT_AUDIO,sample.uTime == 0 ? 0 : sample.uTime -1, sample.uFPTrackID, 0,0);//VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT, 0  );
		*pNew = VO_FALSE;
	}
#endif
	return ptr_obj->m_pSampleCallback->SendData( ptr_obj->m_pSampleCallback->pUserData , VO_SOURCE2_TT_AUDIO , &sample );
}

VO_SOURCE2_MEDIATYPE voAdaptiveStreamingFileParser_pa::GetMeidaType()
{
	return VO_SOURCE2_MT_PUREAUDIO;
}
