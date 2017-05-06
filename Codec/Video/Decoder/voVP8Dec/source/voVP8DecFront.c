/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include "stdlib.h"
#include "voVP8.h"
#include "voVP8DecFrame.h"
#include "voVP8Memory.h"

//
#include "stdio.h"
#ifdef LICENSEFILE  
#include "voCheck.h"
VO_HANDLE g_hVP8DecInst = NULL;
#endif

#ifndef VOWINXP
//#define VOANDROID_SPEED 1
#endif

#ifdef VOANDROID_SPEED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
long start =0,end =0,frames =0;
static unsigned long timeGetTime(){
  struct timeval tval;
  gettimeofday(&tval, NULL);
  return tval.tv_sec*1000 + tval.tv_usec/1000;
}
#endif // VOWINCE

//#define LOG_ENABLE 1
#ifdef LOG_ENABLE
//#define ANDROID_LOG 1
#ifdef VOANDROID
#define LOG_FILE "/data/local/vp8log.txt"
#define LOG_TAG "VOPlayer"
#include <utils/Log.h>
#  define __VOLOG(fmt, args...) ({LOGD("->%d: %s(): " fmt, __LINE__, __FUNCTION__, ## args);})
#  define __VOLOGFILE(fmt, args...) ({FILE *fp =fopen(LOG_FILE, "a"); fprintf(fp, "[ %s ]->%d: %s(): " fmt "\n", LOG_TAG, __LINE__, __FUNCTION__, ## args); fclose(fp);})
#elif VOWINXP
#define LOG_FILE "D:/log.txt"
#define LOG_TAG "VOPlayer"
#include "stdio.h"
#define __VOLOG(fmt, ...)  printf("[ %s ]->%d: %s(): "fmt"\n",LOG_TAG,__LINE__,__FUNCTION__,__VA_ARGS__);
#define __VOLOGFILE(fmt, ...) {FILE *fp =fopen(LOG_FILE, "a");fprintf(fp, "[ %s ]->%d: %s(): " fmt "\n", LOG_TAG, __LINE__, __FUNCTION__, __VA_ARGS__); fclose(fp);}
//fprintf(fp, "[ %s ]->%d: %s(): " fmt "\n", LOG_TAG, __LINE__, __FUNCTION__, __VA_ARGS__); fclose(fp);})
#endif
#else
#define __VOLOGFILE
#endif
//#define DUMP_VP8 1
#ifdef DUMP_VP8
#include "stdio.h"
#define MEM_VALUE_T int
#define MAU_T unsigned char
void mem_put_le16(void *vmem, MEM_VALUE_T val)
{
    MAU_T *mem = (MAU_T *)vmem;

    mem[0] = (val >>  0) & 0xff;
    mem[1] = (val >>  8) & 0xff;
}
 void mem_put_le32(void *vmem, MEM_VALUE_T val)
{
    MAU_T *mem = (MAU_T *)vmem;

    mem[0] = (val >>  0) & 0xff;
    mem[1] = (val >>  8) & 0xff;
    mem[2] = (val >> 16) & 0xff;
    mem[3] = (val >> 24) & 0xff;
}

void write_ivf_file_header(FILE *outfile,
                                  unsigned int fourcc,
                                  int frame_cnt)
{
    char header[32];

    header[0] = 'D';
    header[1] = 'K';
    header[2] = 'I';
    header[3] = 'F';
    mem_put_le16(header + 4,  0);                 /* version */
    mem_put_le16(header + 6,  32);                /* headersize */
    mem_put_le32(header + 8,  fourcc);            /* headersize */
    mem_put_le16(header + 12, 640);          /* width */
    mem_put_le16(header + 14, 480);          /* height */
    mem_put_le32(header + 16, 0); /* rate */
    mem_put_le32(header + 20, 0); /* scale */
    mem_put_le32(header + 24, 0);         /* length */
    mem_put_le32(header + 28, 0);                 /* unused */

    if(fwrite(header, 1, 32, outfile));
}

static void write_ivf_frame_header(FILE *outfile,
                                   long frame_sz)
{
    char             header[12];

    mem_put_le32(header, frame_sz);
    mem_put_le32(header + 4,0);
    mem_put_le32(header + 8,0);

    if(fwrite(header, 1, 12, outfile));
}

void output_one_frame(FILE* outfile,VO_VIDEO_BUFFER outdata,VO_VIDEO_OUTPUTINFO outInfo,int flipuv)
{
	int i;
	unsigned char* buf_y = outdata.Buffer[0];
	unsigned char* buf_u = outdata.Buffer[1];
	unsigned char* buf_v = outdata.Buffer[2];
	unsigned int     stride_y =  outdata.Stride[0];
	unsigned int     stride_u =  outdata.Stride[1];
	unsigned int     stride_v =  outdata.Stride[2];
	unsigned int     width = outInfo.Format.Width;
	unsigned int     height = outInfo.Format.Height;

	for(i=0;i<height;i++)
	{
		fwrite(buf_y, 1, width, outfile);
		buf_y += stride_y;
	}

	if(!flipuv)
	{
		for(i=0;i<(height+1)/2;i++)
		{
			fwrite(buf_u, 1, (1+width)/2, outfile);
			buf_u += stride_u;
		}

		for(i=0;i<(height+1)/2;i++)
		{
			fwrite(buf_v, 1, (1+width)/2, outfile);
			buf_v += stride_v;
		}	
	}
	else
	{
		for(i=0;i<(height+1)/2;i++)
		{
			fwrite(buf_v, 1, (1+width)/2, outfile);
			buf_v += stride_v;
		}

		for(i=0;i<(height+1)/2;i++)
		{
			fwrite(buf_u, 1, (1+width)/2, outfile);
			buf_u += stride_u;
		}
	}
}
#endif


VO_U32 VO_API voVP8DecInit(VO_HANDLE *phCodec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	vpx_codec_ctx_t   *decoder;
	VO_LIB_OPERATOR *pLibOP = NULL;	
	long nCodecIdx;
#ifdef LICENSEFILE 
	VO_U32 err;
	VO_PTR phLicenseCheck = NULL;
#endif
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

	decoder = (vpx_codec_ctx_t*)MallocMem(pUserData, nCodecIdx, sizeof(vpx_codec_ctx_t), CACHE_LINE);
	if(!decoder)
		return VO_ERR_OUTOF_MEMORY;
	SetMem(pUserData, nCodecIdx, (VO_U8 *)decoder, 0, sizeof(vpx_codec_ctx_t));

	decoder->pUserData = pUserData;
	decoder->nCodecIdx = nCodecIdx;

	decoder->nthreads  = 1;
	decoder->disable_deblock = 0;
#ifdef LICENSEFILE
	decoder->phCheck = phLicenseCheck;
#endif

	*phCodec = (VO_HANDLE)decoder;

	

#ifdef DUMP_VP8
	{
		FILE* fp =fopen("E:/MyResource/Video/clips/VP8/error/mkv_vp8_raw.ivf","wb");
		write_ivf_file_header(fp,0x30385056,0);
		fclose(fp);
	}		
#endif

#ifdef VOANDROID_SPEED
	start =0;
	end =0;
	frames =0;
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voVP8DecProcess(VO_HANDLE hCodec, VO_CODECBUFFER *pInData, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat)
{
	return VO_ERR_NONE;
}

VO_U32 VO_API voVP8DecUninit(VO_HANDLE hCodec)
{
	vpx_codec_ctx_t *decoder = (vpx_codec_ctx_t *)hCodec;
	if(decoder==NULL)
		return VO_ERR_INVALID_ARG;

#ifdef LICENSEFILE
	voCheckLibUninit(decoder->phCheck);// checkLib
#endif

	if(decoder)
	{
		if(decoder->pbi)
			vp8dx_remove_decompressor(decoder->pbi);
		FreeMem(decoder->pUserData, decoder->nCodecIdx, decoder);
	}

#ifdef VOANDROID_SPEED
	{
		FILE* fp=fopen("/data/local/vp8test/vp8speed.txt","a");
		fprintf(fp,"\n+++++++++++++++++++%f fps, frames = %d \n\n",((float)frames) / (float)((end)/1000.0),  frames);
		//fprintf(fp,"\n+++++++++++++++++++\n");//%d , frames = %d \n\n",e,  frames);
		fclose(fp);


		//printf("\n+++++++++++++++++++%d , %d, frames = %d \n\n",e, end, frames);
		//__VOLOGFILE("\n+++++++++++++++++++%f fps, frames = %d \n\n",((float)frames) / (float)((end)/1000.0),  frames);

	}
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voVP8DecSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{

	vpx_codec_ctx_t *decoder = (vpx_codec_ctx_t *)hCodec;

	switch(nID)
	{
	case VO_PID_COMMON_CPUNUM:
	case VO_PID_DEC_VP8_THREADS:
		{
			decoder->nthreads = *(int*)plValue;
			if(decoder->nthreads <= 0)
				decoder->nthreads=1;
			if(decoder->nthreads >2)
				decoder->nthreads =2;
		}
		break;
	case VO_PID_DEC_VP8_DISABLEDEBLOCK:
		{
			decoder->disable_deblock = *(int*)plValue;
		}
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voVP8DecGetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	//vpx_codec_ctx_t *pDec = (vpx_codec_ctx_t *)hCodec;
	switch(nID) 
	{
	case VO_PID_VIDEO_ASPECTRATIO:
		{
			*(VO_S32 *)plValue = 0;
		}
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}


	return VO_ERR_NONE;
}

VO_U32 VO_API voVP8DecSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	vpx_codec_ctx_t *decoder = (vpx_codec_ctx_t *)hCodec;

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
	vpx_codec_iter_t  iter = NULL;
    vpx_image_t    *img;
	vpx_codec_ctx_t *decoder = (vpx_codec_ctx_t *)hCodec;
	vpx_codec_err_t ret = VO_ERR_NONE;

#ifdef VOANDROID_SPEED
		start = timeGetTime();
#endif

	if(decoder==NULL)
		return VO_ERR_INVALID_ARG;
#ifdef LICENSEFILE
	voCheckLibResetVideo(decoder->phCheck, decoder->pOutVideoBuf);
#endif
	if(decoder->framebuffer)
		ret = VO_VP8_Decoder(decoder, decoder->framebuffer, decoder->framelength, NULL, 0);
	else
		return VO_ERR_INVALID_ARG;

	if( ret == VPX_CODEC_OK)
	{
		img = VO_VP8_GetFrame(decoder, &iter);
		if(img)
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
			 pOutPutInfo->Format.Type =  (VO_VIDEO_FRAMETYPE)(decoder->framebuffer[0]& 1);

			 pOutput->Time = decoder->nTimes;
			 pOutput->UserData = decoder->m_inUserData;

			 
		 }
	}
	else
	{
		pOutPutInfo->Format.Width = 0;
		 pOutPutInfo->Format.Height =0;
		  pOutput->Time = decoder->nTimes;

		pOutPutInfo->Format.Type =	VO_VIDEO_FRAME_NULL;
	}
	

	//__VOLOGFILE("decoder->nTimes :%d  %d",decoder->nTimes,pOutPutInfo->Format.Type);
	 
#ifdef DUMP_VP8

	{
		FILE* fp =fopen("E:/MyResource/Video/clips/VP8/error/mkv_vp8_raw.ivf","ab");
		write_ivf_frame_header(fp, decoder->framelength);
		fwrite(decoder->framebuffer,decoder->framelength,1,fp);
		fclose(fp);
	}
	 /*{	
			FILE*fp =fopen("/data/local/mkv_vp8_raw.yuv","ab");
			output_one_frame(fp,*pOutput,*pOutPutInfo,0);
			fclose(fp);
	}*/
#endif 
	 pOutPutInfo->Flag = 0;
#ifdef LICENSEFILE
	 decoder->pOutVideoBuf = pOutput;
	voCheckLibCheckVideo(decoder->phCheck, pOutput,&pOutPutInfo->Format);
#endif
#ifdef VOANDROID_SPEED
		end += timeGetTime() - start;
		//printf(" ----frame %d--%d----\n",frames,timeGetTime() - start);
		frames++;
#endif
	return ret;
}

VO_S32 VO_API voGetVP8DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
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