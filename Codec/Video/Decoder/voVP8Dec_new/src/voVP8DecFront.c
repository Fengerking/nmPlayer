/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "voVP8.h"
#include "vpx_image.h"
#include "voVP8Memory.h"
#include "voVP8DecFront.h"

extern vpx_codec_iface_t* vpx_codec_vp8_dx(void);
#define vp8_interface (vpx_codec_vp8_dx())

#ifdef LICENSEFILE  
#include "voCheck.h"
VO_HANDLE g_hVP8DecInst = NULL;
#endif

VO_U32 VO_API voVP8DecInit(VO_HANDLE *phCodec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	VO_VP8DEC_CTX   *decoder;
	VO_LIB_OPERATOR *pLibOP = NULL;
#ifdef LICENSEFILE 
	VO_PTR phLicenseCheck = NULL;
	VO_U32 err;
#endif
	long nCodecIdx;
	if(VO_VIDEO_CodingVP8 != vType)
	{
		return VO_ERR_DEC_VP8_CODEC_ID;
	}

	nCodecIdx = VO_INDEX_DEC_VP8;
#ifdef LICENSEFILE 
	if(pUserData){
		if(pUserData->memflag & 0XF0){//valid libOperator flag
			pLibOP = pUserData->libOperator;
		}
	}
	err = voCheckLibInit(&phLicenseCheck, VO_INDEX_DEC_VP8, pUserData?pUserData->memflag:0, g_hVP8DecInst,pLibOP);
	if(err != VO_ERR_NONE)
	{
		*phCodec = NULL;
		if(phLicenseCheck)
		{
			voCheckLibUninit(phLicenseCheck);
			phLicenseCheck = NULL;
		}
		return err;
	}
#endif

	if(pUserData){
		if((!(pUserData->memflag&0xF)) || (pUserData->memData == NULL)){
			pUserData = NULL;
		}
	}

	decoder = (VO_VP8DEC_CTX*)MallocMem(pUserData, nCodecIdx, sizeof(VO_VP8DEC_CTX), CACHE_LINE);
	if(!decoder)
		return VO_ERR_OUTOF_MEMORY;
	SetMem(pUserData, nCodecIdx, (VO_U8 *)decoder, 0, sizeof(VO_VP8DEC_CTX));

	 if(vpx_codec_dec_init(&decoder->vpxcodec, vp8_interface, NULL, 0))                    //
		 return VO_ERR_FAILED;

	decoder->pUserData = pUserData;
	decoder->nCodecIdx = nCodecIdx;

	decoder->nthreads  = 1;
	decoder->disable_deblock = 0;
#ifdef LICENSEFILE
	decoder->phCheck = phLicenseCheck;
#endif

	*phCodec = (VO_HANDLE)decoder;

	return VO_ERR_NONE;
}

VO_U32 VO_API voVP8DecUninit(VO_HANDLE hCodec)
{
	VO_VP8DEC_CTX *decoder = (VO_VP8DEC_CTX *)hCodec;
	if(decoder==NULL)
		return VO_ERR_INVALID_ARG;

#ifdef LICENSEFILE
	voCheckLibUninit(decoder->phCheck);// checkLib
#endif

	if(decoder)
	{
		 if(vpx_codec_destroy(&decoder->vpxcodec)) 
			 return VO_ERR_FAILED;

		FreeMem(decoder->pUserData, decoder->nCodecIdx, decoder);
	}

#ifdef VOANDROID_SPEED
	{
		FILE* fp=fopen("/data/local/vp8test/vp8speed.txt","a");
		fprintf(fp,"\n+++++++++++++++++++%f fps, frames = %d \n\n",((float)frames) / (float)((end)/1000.0),  frames);
		fclose(fp);
	}
#endif	

	return VO_ERR_NONE;
}

VO_U32 VO_API voVP8DecSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	VO_VP8DEC_CTX *decoder = (VO_VP8DEC_CTX *)hCodec;

	if(decoder==NULL)
		return VO_ERR_INVALID_ARG;

	decoder->framebuffer	= pInput->Buffer;
	decoder->framelength	= pInput->Length;
	decoder->nTimes = pInput->Time;
	decoder->m_inUserData  = pInput->UserData;
	
	return VO_ERR_NONE;
}


VO_U32 VO_API voVP8DecGetOutputData(VO_HANDLE hCodec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutPutInfo)
{
	VO_VP8DEC_CTX *decoder = (VO_VP8DEC_CTX *)hCodec;
	vpx_codec_iter_t  iter = NULL;
    vpx_image_t    *img;	
	vpx_codec_err_t ret = VO_ERR_NONE;

#ifdef VOANDROID_SPEED
		start = timeGetTime();
#endif

	if(decoder==NULL)
		return VO_ERR_INVALID_ARG;
#ifdef LICENSEFILE
	voCheckLibResetVideo(decoder->phCheck, decoder->pOutVideoBuf);
#endif
	ret = vpx_codec_decode(&decoder->vpxcodec, decoder->framebuffer, decoder->framelength, NULL, 0);

	if( ret == VPX_CODEC_OK && (img = vpx_codec_get_frame(&decoder->vpxcodec, &iter)))
	{
			 pOutput->Buffer[0] = img->planes[PLANE_Y];
			 pOutput->Buffer[1] = img->planes[PLANE_U];
			 pOutput->Buffer[2] = img->planes[PLANE_V];
			 pOutput->Stride[0] = img->stride[PLANE_Y];
			 pOutput->Stride[1] = img->stride[PLANE_U];
			 pOutput->Stride[2] = img->stride[PLANE_V];
			 pOutput->ColorType = VO_COLOR_YUV_PLANAR420;
			 pOutPutInfo->Format.Width  = img->d_w;
			 pOutPutInfo->Format.Height = img->d_h;
			 pOutput->UserData = decoder->m_inUserData;			 
#if USE_FRAME_THREAD
			 pOutPutInfo->Format.Type =  (VO_VIDEO_FRAMETYPE)(img->type);
			 pOutPutInfo->Flag = img->Flag;
			 pOutput->Time = img->Time;
#else
			 pOutPutInfo->Format.Type =  (VO_VIDEO_FRAMETYPE)(decoder->framebuffer[0]& 1);
			 pOutput->Time = decoder->nTimes;
#endif
	}
	else
	{
		pOutput->Buffer[0] = NULL;
		pOutput->Buffer[1] = NULL;
		pOutput->Buffer[2] = NULL;
		pOutPutInfo->Format.Width = 0;
		pOutPutInfo->Format.Height =0;
		pOutput->Time = decoder->nTimes;
		pOutPutInfo->Format.Type =	VO_VIDEO_FRAME_NULL;
		pOutPutInfo->Flag = 0;
	}
	decoder->framebuffer = NULL;
	decoder->framelength = 0;
	 
#ifdef DUMP_VP8

	{
		FILE* fp =fopen("E:/MyResource/Video/clips/VP8/error/mkv_vp8_raw.ivf","ab");
		write_ivf_frame_header(fp, decoder->framelength);
		fwrite(decoder->framebuffer,decoder->framelength,1,fp);
		fclose(fp);
	}
#endif 
#ifdef LICENSEFILE
	 decoder->pOutVideoBuf = pOutput;
	voCheckLibCheckVideo(decoder->phCheck, pOutput,&pOutPutInfo->Format);
#endif
#ifdef VOANDROID_SPEED
		end += timeGetTime() - start;
		frames++;
#endif
	return VO_ERR_NONE;
}

VO_U32 VO_API voVP8DecProcess(VO_HANDLE hCodec, VO_CODECBUFFER *pInData, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat)
{
	VO_VP8DEC_CTX *decoder = (VO_VP8DEC_CTX *)hCodec;
	if (!decoder)
	{
		return VO_ERR_INVALID_ARG;
	}

	return VO_ERR_NONE;
}


VO_U32 VO_API voVP8DecSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	VO_VP8DEC_CTX *decoder = (VO_VP8DEC_CTX *)hCodec;
	if (!decoder)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	switch (nID)
	{
	case VO_PID_DEC_VP8_THREADS:
	case VO_PID_COMMON_CPUNUM:
		{
			VO_S32 nCPU = *((VO_S32*)plValue);
			VO_S32 maxThdNum;
#if USE_FRAME_THREAD
			maxThdNum = MAX_THREAD_NUM2;
#else
			maxThdNum = 1;
#endif
			if (decoder->pbi)	//if the decoder has already created, cpu num can't be changed
			{
				return VO_ERR_WRONG_STATUS;
			}
			if (nCPU <= 0)
			{
				return VO_ERR_INVALID_ARG;
			}
			if (nCPU > maxThdNum)
			{
				nCPU = maxThdNum;
				*((VO_S32*)plValue) = nCPU; //put back the value really set
			}

			decoder->nthreads = nCPU;
			decoder->vpxcodec.config.dec->threads = nCPU;
		}

		break;
	case VO_PID_DEC_VP8_DISABLEDEBLOCK:
		decoder->disable_deblock = *((VO_S32*)plValue);
		break;
	case VO_ID_VP8_FLUSH_PICS:
		if (decoder->pbi)	//if the decoder has already created, cpu num can't be changed
		{
			return VO_ERR_WRONG_STATUS;
		}
		if (decoder->nthreads > 1) //only for multi-thread ???
		{
			decoder->vpxcodec.config.dec->flush = 1;
			return vpx_codec_decode(&decoder->vpxcodec, decoder->framebuffer, decoder->framelength, NULL, 0);
		}
		break;
	default:
		return VO_ERR_NONE;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voVP8DecGetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	VO_VP8DEC_CTX *decoder = (VO_VP8DEC_CTX *)hCodec;
	if (!decoder)
	{
		return VO_ERR_INVALID_ARG;
	}

	return VO_ERR_NONE;
}

EXPORT VO_S32 VO_API voGetVP8DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	VO_VIDEO_DECAPI *pVP8Dec = pDecHandle;

	if(!pVP8Dec)
		return VO_ERR_INVALID_ARG;

	pVP8Dec->Init   = voVP8DecInit;
	pVP8Dec->Uninit = voVP8DecUninit;
	pVP8Dec->SetParam = voVP8DecSetParameter;
	pVP8Dec->GetParam = voVP8DecGetParameter;
	pVP8Dec->SetInputData = voVP8DecSetInputData;
	pVP8Dec->GetOutputData = voVP8DecGetOutputData;

	return VO_ERR_NONE;

}