#include <string.h>

#include "voaEngine.h"
#include "CVOMEPlayer.h"
//#include "CVOWPlayer.h"
#include "CVOMERecorder.h"
#include "CVOMEMetadataRetriever.h"
#include "CVOMETranscode.h"
#include "CDataSourcePlayer.h"

#undef LOG_TAG
#define LOG_TAG "voaEngine"
#include "voLog.h"

void * voaInit (int nFlag)
{
	VOLOGI ("voaInit Flag: %d", nFlag);

	if (nFlag == VOA_TYPE_PLAYER)
	{
		CVOMEPlayer * pPlayer = new CVOMEPlayer ();

		//CVOMEPlayer * pPlayer = new CVOWPlayer ();
		return pPlayer;
	}
	else if (nFlag == VOA_TYPE_RECORDER)
	{
		CVOMERecorder * pRecorder = new CVOMERecorder ();
		return pRecorder;
	}
	else if (nFlag == VOA_TYPE_METADATA)
	{
		CVOMEMetadataRetriever * pMetadata = new CVOMEMetadataRetriever ();
		return pMetadata;
	}
	else if (nFlag == VOA_TYPE_TRANSCODE)
	{
		CVOMERecorder * pRecorder = new CVOMETranscode ();
		return pRecorder;
	}
	else if (nFlag == VOA_TYPE_DATASOURCE)
	{
		CVOMEPlayer * pPlayer = new CDataSourcePlayer ();
		return pPlayer;
	}

	return NULL;
}

int voaSetParam (void * hEngine, int nID, void * pValue1, void * pValue2)
{
	if (hEngine == NULL)
		return -1;
	if (nID >= VOAP_IDF_onFirstRef && nID <= VOAP_IDC_setDrmApi)
	{
		CVOMEPlayer * pPlayer = (CVOMEPlayer *)hEngine;
		switch (nID)
		{
			case VOAP_IDF_onFirstRef:
				VOLOGI ("VOAP_IDF_onFirstRef");
				return 0;

			case VOAP_IDF_initCheck:
				VOLOGI ("VOAP_IDF_initCheck");
				return pPlayer->Init ();

			case VOAP_IDF_setDataSourceURL:
				VOLOGI ("VOAP_IDF_setDataSourceURL");
				return pPlayer->SetSource ((const char *)pValue1 , (const char *)pValue2);

			case VOAP_IDF_setDataSourceID:
			{
				VOLOGI ("VOAP_IDF_setDataSourceID");
				VOA_SOURCE * pSource = (VOA_SOURCE *)pValue1;
				return pPlayer->SetSource (pSource->fd, pSource->offset, pSource->length);
			}

			case VOAP_IDF_prepare:
				VOLOGI ("VOAP_IDF_prepare");
				return pPlayer->Prepare ();

			case VOAP_IDF_prepareAsync:	
				VOLOGI ("VOAP_IDF_prepareAsync");
				return pPlayer->PrepareAsync ();

			case VOAP_IDF_start:
				VOLOGI ("VOAP_IDF_start");
				return pPlayer->Run ();

			case VOAP_IDF_stop:		
				VOLOGI ("VOAP_IDF_stop");
				return pPlayer->Stop ();

			case VOAP_IDF_seekTo:
			{
				VOLOGI ("VOAP_IDF_seekTo Pos is %d ", *(int *)pValue1);
				int nRC = pPlayer->SetPos (*(int *)pValue1);
				VOLOGI ("VOAP_IDF_seekTo Result %d ", nRC);
				return 0;
			}

			case VOAP_IDF_pause:
				VOLOGI ("VOAP_IDF_pause");
				return pPlayer->Pause ();

			case VOAP_IDF_setLooping:
				VOLOGI ("VOAP_IDF_setLooping = %d", *(int *)pValue1);
				return pPlayer->SetLoop (*(int *)pValue1);

			case VOAP_IDF_suspend:
				VOLOGI ("VOAP_IDF_suspend");
				return pPlayer->Suspend ();

			case VOAP_IDF_resume:
				VOLOGI ("VOAP_IDF_resume");
				return pPlayer->Resume ();

			case VOAP_IDF_ChangeSurface:
				VOLOGI ("VOAP_IDF_ChangeSurface");
				return pPlayer->ChangeSurface ((VOAUpdateSurfaceCallBack)pValue1, pValue2);

#ifdef _LINUX_ANDROID
			case VOAP_IDF_invoke:
				VOLOGI ("VOAP_IDF_invoke ");
				
				return pPlayer->Invoke ((android::Parcel *) pValue1, (android::Parcel *)pValue2);
#endif // _LINUX_ANDROID

			case VOAP_IDF_release:
				VOLOGI ("VOAP_IDF_release");
				return pPlayer->Uninit ();

			case VOAP_IDF_reset:
				VOLOGI ("VOAP_IDF_reset");
				return pPlayer->Uninit ();

			case VOAP_IDF_setCallBack:
				return pPlayer->SetCallBack ((VOACallBack)pValue1, pValue2);

			case VOAP_IDF_setFilePipe:	
				return pPlayer->SetFilePipe (pValue1);

			case VOAP_IDF_setSurface:	
				return pPlayer->SetSurface (pValue1);

			case VOAP_IDF_setVideoCodec:	
				return pPlayer->SendVideoCodec (*(OMX_VIDEO_CODINGTYPE *) pValue1);

			case VOAP_IDF_setAudioCodec:	
				return pPlayer->SendVideoCodec (*(OMX_VIDEO_CODINGTYPE *) pValue1);

			case VOAP_IDF_setVideoData:	
				return pPlayer->SendVideoData ((VOA_DATA_BUFFERTYPE *)pValue1, *(int *)pValue2);

			case VOAP_IDF_setAudioData:	
				return pPlayer->SendAudioData ((VOA_DATA_BUFFERTYPE *)pValue1, *(int *)pValue2);

			case VOAP_IDC_setDrmApi:
				return pPlayer->SetDrmApi(pValue1);

			default:
				break;
		}
	}
	else if (nID >= VOAM_IDF_setDataSourceURL && nID <= VOAM_IDF_setExtDrmApi)
	{
		CVOMEMetadataRetriever * pMetadata = (CVOMEMetadataRetriever *)hEngine;
		switch (nID)
		{
		case VOAM_IDF_setDataSourceURL:
			VOLOGI ("VOAM_IDF_setDataSourceURL");
			return pMetadata->setDataSource ((const char *)pValue1);

		case VOAM_IDF_setDataSourceID:
		{
			VOLOGI ("VOAM_IDF_setDataSourceID");
			VOA_SOURCE * pSource = (VOA_SOURCE *)pValue1;
			return pMetadata->setDataSource (pSource->fd, pSource->offset, pSource->length);
		}

		case VOAM_IDF_setMode:
			VOLOGI ("VOAM_IDF_setMode");
			return pMetadata->setMode ((int)pValue1);
		
		case VOAM_IDF_setFilePipe:
			VOLOGI ("VOAM_IDF_setFilePipe");
			return pMetadata->SetFilePipe (pValue1);

		case VOAM_IDF_setDrmApi:
			return pMetadata->SetDrmApi(pValue1);

		case VOAM_IDF_setExtDrmApi:
			VOLOGI("VOAM_IDF_setExtDrmApi Set LG Play ready DRM");
			return pMetadata->SetFileBasedDrmApi(pValue1);

		default:
			break;
		}
	}
	else if (nID >= VOAR_IDF_Init && nID <= VOAR_IDF_setFilePipe)
	{
		CVOMERecorder * pRec = (CVOMERecorder *)hEngine;
		switch (nID)
		{
		case VOAR_IDF_Init:
			VOLOGI ("VOAR_IDF_Init");
			return 0;

		case VOAR_IDF_setVideoData:
			//VOLOGI ("VOAR_IDF_setVideoData");
			return pRec->SendVideoData ((VOA_DATA_BUFFERTYPE *)pValue1, *(int*)pValue2);

		case VOAR_IDF_setAudioData:
			//VOLOGI ("VOAR_IDF_setAudioData");
			return pRec->SendAudioData ((VOA_DATA_BUFFERTYPE *)pValue1, *(int*)pValue2);

		case VOAR_IDF_setOutputFormat:
			VOLOGI ("VOAR_IDF_setOutputFormat");
			return pRec->setOutputFormat (*(output_format*)pValue1);

		case VOAR_IDF_setAudioEncoder:
			VOLOGI ("VOAR_IDF_setAudioEncoder");
			return pRec->setAudioEncoder (*(audio_encoder*)pValue1);

		case VOAR_IDF_setAudioFormat:
		{
			VOLOGI ("VOAR_IDF_setAudioFormat");
			VOA_AUDIO_FORMAT * pFormat = (VOA_AUDIO_FORMAT *)pValue1;
			return pRec->setAudioFormat (pFormat->nSampleRate, pFormat->nChannels);
		}

		case VOAR_IDF_setVideoEncoder:
			VOLOGI ("VOAR_IDF_setVideoEncoder");
			return pRec->setVideoEncoder (*(video_encoder*)pValue1);

		case VOAR_IDF_setColorType:
			VOLOGI ("VOAR_IDF_setColorType");
			return pRec->setColorType (*(VOA_COLORTYPE*)pValue1);

		case VOAR_IDF_setVideoSize:
			VOLOGI ("VOAR_IDF_setVideoSize");
			return pRec->setVideoSize (*(int*)pValue1, *(int*)pValue2);

		case VOAR_IDF_setFrameRate:
			VOLOGI ("VOAR_IDF_setFrameRate");
			return pRec->setVideoFrameRate (*(int*)pValue1);

		case VOAR_IDF_setOutputFile:
			VOLOGI ("VOAR_IDF_setOutputFile");
			return pRec->setOutputFile ((char*)pValue1);

		case VOAR_IDF_setOutputFileID:
		{
			VOLOGI ("VOAR_IDF_setOutputFileID");
			VOA_SOURCE * pSource = (VOA_SOURCE *)pValue1;
			return pRec->setOutputFile (pSource->fd, pSource->offset, pSource->length);
		}

		case VOAR_IDF_setParamters:
			VOLOGI ("VOAR_IDF_setParamters");
			return pRec->setParameters ((char*)pValue1);

		case VOAR_IDF_prepare:
			VOLOGI ("VOAR_IDF_prepare");
			return pRec->Prepare ();

		case VOAR_IDF_start:		
			VOLOGI ("VOAR_IDF_start");
			return pRec->Start ();

		case VOAR_IDF_pause:		
			VOLOGI ("VOAR_IDF_pause");
			return pRec->Pause ();

		case VOAR_IDF_stop:	
			VOLOGI ("VOAR_IDF_stop");
			return pRec->Stop ();

		case VOAR_IDF_close:	
			VOLOGI ("VOAR_IDF_close");
			return pRec->Close ();

		case VOAR_IDF_reset:		
			VOLOGI ("VOAR_IDF_reset");
			return 0;

		case VOAR_IDF_setVideoSource:
			VOLOGI ("VOAR_IDF_setVideoSource");
			return pRec->setVideoSource (*(video_source*)pValue1);

		case VOAR_IDF_setAudioSource:
			VOLOGI ("VOAR_IDF_setAudioSource");
			return pRec->setAudioSource (*(audio_source*)pValue1);

		case VOAR_IDF_setDataSource:
			VOLOGI ("VOAR_IDF_setDataSource");
			return pRec->SetDataSource ((char *)pValue1, *(int *)pValue2);

		case VOAR_IDF_setCallBack:
			return pRec->SetCallBack ((VOACallBack)pValue1, pValue2);

		case VOAR_IDF_setFilePipe:	
			return pRec->SetFilePipe (pValue1);

		default:
			break;
		}
	}
	return 0;
}

int voaGetParam (void * hEngine, int nID, void * pValue1, void * pValue2)
{
	if (hEngine == NULL)
		return -1;

	if (nID >= VOAP_IDF_onFirstRef && nID <= VOAP_IDC_RenderAudio)
	{
		CVOMEPlayer * pPlayer = (CVOMEPlayer *)hEngine;
		switch (nID)
		{
			case VOAP_IDF_isPlaying:
			{
				//VOLOGI ("VOAP_IDF_isPlaying");
				*(int *)pValue1 = pPlayer->IsPlaying ();
				return 0;
			}

			case VOAP_IDF_setLooping:
			{
				VOLOGI ("VOAP_IDF_setLooping = %d", *(int *)pValue1);
				return pPlayer->SetLoop (*(int *)pValue1);
			}

			case VOAP_IDF_getCurrentPosition:
			{
				//VOLOGI ("VOAP_IDF_getCurrentPosition");
				return pPlayer->GetPos ((int *)pValue1);
			}

			case VOAP_IDF_getDuration:
			{
				//VOLOGI ("VOAP_IDF_getCurrentPosition");
				return pPlayer->GetDuration ((int *)pValue1);
			}

			case VOAP_IDF_playerType:
			{
				*(int *)pValue1 = 4; // VO_PLAYER
				return 0;
			}

#ifdef _LINUX_ANDROID
			case VOAP_IDF_isSeekable:
			{
				VOLOGI ("VOAP_IDF_getMetadata");
				*(int *)pValue1 =  pPlayer->GetSeekAble();
				return 0;
			}
#endif // _LINUX_ANDROID

			default:
				break;
		}
	}
	else if (nID >= VOAM_IDF_setDataSourceURL && nID <= VOAM_IDF_setFilePipe)
	{
		CVOMEMetadataRetriever * pMetadata = (CVOMEMetadataRetriever *)hEngine;
		switch (nID)
		{
		case VOAM_IDF_getMode:
			VOLOGI ("VOAM_IDF_getMode");
			return pMetadata->getMode ((int *)pValue1);

		case VOAM_IDF_captureFrame:
		{
			VOLOGI ("VOAM_IDF_captureFrame");
			VOA_GETFRAMEATTIME_PARAM param;
			memset(&param, 0, sizeof(param));
			param.nPosition = -1;

			//stony remark resolution specify code, since it will cause video data cut
			param.nFlags = VOA_GETFRAMEATTIME_FLAG_FORBIDBLACKFRAME;
			param.nFlags = VOA_GETFRAMEATTIME_FLAG_SPECIFYRESOLUTION | VOA_GETFRAMEATTIME_FLAG_FORBIDBLACKFRAME;
			param.nWidth = 160;
			param.nHeight = 120;
			*(void**)pValue1 = pMetadata->getFrameAtTime(&param);
			return 0;
		}

		case VOAM_IDF_getFrameAtTime:
		{
			VOA_GETFRAMEATTIME_PARAM * pGetFrameAtTime = (VOA_GETFRAMEATTIME_PARAM *)pValue2;
			VOLOGI ("VOAM_IDF_getFrameAtTime position %d, flags 0x%08X, width %d, height %d", 
				pGetFrameAtTime->nPosition, pGetFrameAtTime->nFlags, pGetFrameAtTime->nWidth, pGetFrameAtTime->nHeight);
			*(void**)pValue1 = pMetadata->getFrameAtTime(pGetFrameAtTime);
			return 0;
		}

		case VOAM_IDF_extractAlbumArt:
		{
			VOLOGI ("VOAM_IDF_extractAlbumArt");
			*(void**)pValue1 = pMetadata->extractAlbumArt ();
			return 0;
		}

		case VOAM_IDF_extractMetadata:
		{
			const char* pResult = pMetadata->extractMetadata ((int)pValue1);
			if(pResult)
			{
				VOLOGI ("VOAM_IDF_extractMetadata %d %s", (int)pValue1, pResult);
			}
			else
			{
				VOLOGW ("VOAM_IDF_extractMetadata %d %s", (int)pValue1, pResult);
			}
			*(const char**)pValue2 = pResult;
			return 0;
		}

		default:
			break;
		}
	}
	else if (nID >= VOAR_IDF_Init && nID <= VOAR_IDF_setFilePipe)
	{
		CVOMERecorder * pRecorder = (CVOMERecorder *)hEngine;
		switch (nID)
		{
		case VOAR_IDF_getMaxAmplitude:
			VOLOGI ("VOAR_IDF_getMaxAmplitude");
			return pRecorder->getMaxAmplitude ((int *)pValue1);

		default:
			break;
		}
	}

	return -1;
}

int voaUninit (void * hEngine, int nFlag)
{
	VOLOGI ("Flag: %d", nFlag);

	if (nFlag == VOA_TYPE_PLAYER)
	{
		CVOMEPlayer * pPlayer = (CVOMEPlayer *)hEngine;
		pPlayer->Uninit ();

		delete pPlayer;
	}
	else if (nFlag == VOA_TYPE_RECORDER)
	{
		CVOMERecorder * pRecorder = (CVOMERecorder *)hEngine;
		delete pRecorder;
	}
	else if (nFlag == VOA_TYPE_METADATA)
	{
		CVOMEMetadataRetriever * pMetadata = (CVOMEMetadataRetriever *)hEngine;
		delete pMetadata;
	}
	else if (nFlag == VOA_TYPE_TRANSCODE)
	{
		CVOMETranscode * pRecorder = (CVOMETranscode *)hEngine;
		delete pRecorder;
	}
	else if (nFlag == VOA_TYPE_DATASOURCE)
	{
		CDataSourcePlayer * pPlayer = (CDataSourcePlayer *)hEngine;
		pPlayer->Uninit ();
		delete pPlayer;
	}

	return 0;
}
