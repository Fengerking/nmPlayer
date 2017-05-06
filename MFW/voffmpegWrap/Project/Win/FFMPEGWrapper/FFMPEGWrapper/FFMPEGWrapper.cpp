// FFMPEGWrapper.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "FFMPEGWrapper.h"


// 这是导出变量的一个示例
FFMPEGWRAPPER_API int nFFMPEGWrapper=0;

// 这是导出函数的一个示例。
FFMPEGWRAPPER_API int fnFFMPEGWrapper(void)
{
	return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 FFMPEGWrapper.h
CFFMPEGWrapper::CFFMPEGWrapper()
{
	return;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
//#include <dlfcn.h>

#include "voVideo.h"
#include "voAudio.h"

#if defined LOG_TAG
#undef LOG_TAG
#define LOG_TAG "OMXCompWrap"
#endif
#include "voLog.h"

#define ENABLE_GRAY 0
#define restrict 
#define ENABLE_SMALL 0
#define ENABLE_H264_DECODER 1
// 
// #include "dsputil.h"
// #include "h264.h"

#ifdef HAVE_AV_CONFIG_H
#undef HAVE_AV_CONFIG_H
#endif

#include "avcodec.h"
#include "./libavutil/pixdesc.h"
#include "./libswscale/swscale.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
void *av_malloc(size_t size);
void av_free(void *ptr);

void avcodec_register_all(void);
AVCodec *avcodec_find_decoder(enum CodecID id);
AVCodecContext *avcodec_alloc_context3(AVCodec *codec);
AVFrame *avcodec_alloc_frame(void);
void av_init_packet(AVPacket *pkt);
int avcodec_open2(AVCodecContext *avctx, AVCodec *codec, AVDictionary **options);
int avcodec_decode_video2(AVCodecContext *avctx, AVFrame *picture,
                         int *got_picture_ptr,
                         const AVPacket *avpkt);
int avcodec_close(AVCodecContext *avctx);
*/
typedef void (* AV_MALLOC) (size_t size);
typedef void (* AV_FREE) (void *);

typedef void (* AVCODEC_REGISTER_ALL) (void);
typedef AVCodec * (* AVCODEC_FIND_DECODER) (enum AVCodecID id);//Jeff CodecID
typedef AVCodecContext * (* AVCODEC_ALLOC_CONTEXT3) (AVCodec *codec);
typedef AVFrame * (* AVCODEC_ALLOC_FRAME) (void);
typedef void (* AV_INIT_PACKET) (AVPacket *pkt);
typedef int (* AVCODEC_OPEN2) (AVCodecContext *avctx, AVCodec *codec, AVDictionary **options);
typedef int (* AVCODEC_DECODE_VIDEO2) (AVCodecContext *avctx, AVFrame *picture,
                        				 int *got_picture_ptr,
                         					const AVPacket *avpkt);
typedef int (* AVCODEC_CLOSE) (AVCodecContext *avctx);


typedef struct {
    void *                  hDllU;
	void *					hDll;
	AV_MALLOC				fMalloc;
	AV_FREE					fFree;
	AVCODEC_REGISTER_ALL	fRegAll;
	AVCODEC_FIND_DECODER	fFindDec;
	AVCODEC_ALLOC_CONTEXT3	fAllocContext3;
	AVCODEC_ALLOC_FRAME		fAllocFrame;
	AV_INIT_PACKET			fInitPacket;
	AVCODEC_OPEN2			fOpen2;
	AVCODEC_DECODE_VIDEO2	fDecVideo2;
	AVCODEC_CLOSE			fClose;
} VO_FFMPEG_FUNC;

typedef struct {
	VO_FFMPEG_FUNC		sFunc;
	AVCodec	*			hCodec;		
	AVCodecContext *	hContext;		
	AVPacket			sPacket;
	AVFrame *			pFrame;	
	unsigned char *		pHeadData;
	int					nHeadSize;
	int					nCpuNum;
	int					nCodecType;
	long long			llPrevTime;
	
    struct SwsContext *	pSWS;	
	unsigned char *		pRGBData;
} VO_FFMPEG_CONTEXT;

void *	g_hvoffmepgDumpFile = NULL;

VO_U32 voffmpegVideoDecInitFFMPEG (VO_FFMPEG_CONTEXT * pffmpegContext);
VO_U32 voffmpegLoadLib (VO_FFMPEG_CONTEXT * pffmpegContext);
VO_U32 voffmpegVideoColorConvert (VO_FFMPEG_CONTEXT * pffmpegContext);

VO_U32 voffmpegVideoDecInit(VO_HANDLE * phDec,VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	VOLOGI ("Coding Type %d", vType);
	
	VO_FFMPEG_CONTEXT * pffmpegContext = (VO_FFMPEG_CONTEXT *) malloc (sizeof (VO_FFMPEG_CONTEXT));
	if (pffmpegContext == NULL)
		return VO_ERR_OUTOF_MEMORY;
		
	memset (pffmpegContext, 0, sizeof (VO_FFMPEG_CONTEXT));
	
	pffmpegContext->nCpuNum = 2;
	pffmpegContext->nCodecType = vType;
	
	voffmpegLoadLib (pffmpegContext);

	*phDec = pffmpegContext;
	
//	g_hvoffmepgDumpFile = fopen ("/mnt/sdcard/ffmpeg.264", "wb");
	
	return VO_ERR_NONE;
}

VO_U32 voffmpegVideoDecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{
	int nRC = 0;
	
	if (hDec == NULL)
		return VO_ERR_INVALID_ARG;
		
	VO_FFMPEG_CONTEXT * pffmpeg = (VO_FFMPEG_CONTEXT *)hDec;
	if (pffmpeg->hCodec == NULL)
	{
		nRC = voffmpegVideoDecInitFFMPEG (pffmpeg);
		if (nRC != VO_ERR_NONE)
			return nRC;
	}
	
//	VOLOGI ("vovovo Buffer %p, Size %d", pInput->Buffer, pInput->Length);
	if (pInput->Length == 0)
		return VO_ERR_INPUT_BUFFER_SMALL;
		
	if (pffmpeg->nHeadSize > 0 && pffmpeg->pHeadData != NULL)
	{
		memcpy (pffmpeg->pHeadData + pffmpeg->nHeadSize, pInput->Buffer, pInput->Length);
		pffmpeg->sPacket.data = pffmpeg->pHeadData;			
		pffmpeg->sPacket.size = pffmpeg->nHeadSize + pInput->Length;
		pffmpeg->nHeadSize = 0;
	}
	else
	{
		pffmpeg->sPacket.data = pInput->Buffer;	
		pffmpeg->sPacket.size = pInput->Length;
		if (pffmpeg->pHeadData != NULL)
		{
			free (pffmpeg->pHeadData);
			pffmpeg->pHeadData = NULL;
		}
	}		
	
	pffmpeg->sPacket.pts = pInput->Time;
	pffmpeg->sPacket.dts = pInput->Time - 1;
	
	return VO_ERR_NONE;
}

VO_U32 voffmpegVideoDecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo)
{	
	int nLen = 0;
	int nGotPic = 0;
	
	if (hDec == NULL)
		return VO_ERR_INVALID_ARG;
		
	VO_FFMPEG_CONTEXT * pffmpeg = (VO_FFMPEG_CONTEXT *)hDec;
	if (pffmpeg->hCodec == NULL)
		return VO_ERR_WRONG_STATUS;
		
	if (pffmpeg->sPacket.size <= 0)
		return VO_ERR_INPUT_BUFFER_SMALL;

	nLen = pffmpeg->sFunc.fDecVideo2(pffmpeg->hContext, pffmpeg->pFrame, &nGotPic, &pffmpeg->sPacket);
//	nLen = avcodec_decode_video2(pffmpeg->hContext, pffmpeg->pFrame, &nGotPic, &pffmpeg->sPacket);
///	VOLOGI ("Len %d,  Got Picture %d Buffer %p", nLen, nGotPic, pffmpeg->pFrame->data[0]);
	
	if (nLen < 0 || nGotPic <= 0)
		return VO_ERR_INPUT_BUFFER_SMALL;
		
	pOutBuffer->Buffer[0] = pffmpeg->pFrame->data[0];
	pOutBuffer->Stride[0] = pffmpeg->pFrame->linesize[0];	
	
	pOutBuffer->Buffer[1] = pffmpeg->pFrame->data[1];
	pOutBuffer->Stride[1] = pffmpeg->pFrame->linesize[1];	
	
	pOutBuffer->Buffer[2] = pffmpeg->pFrame->data[2];
	pOutBuffer->Stride[2] = pffmpeg->pFrame->linesize[2];	
	
	pOutBuffer->ColorType = VO_COLOR_YUV_PLANAR420;	
	
	pOutInfo->Format.Width = pffmpeg->pFrame->width;
	pOutInfo->Format.Height = pffmpeg->pFrame->height;
	pOutInfo->InputUsed = nLen;
	
	pffmpeg->sPacket.data += nLen;
	pffmpeg->sPacket.size -= nLen;
		
//	VOLOGI ("Time: %06d      %06d", (int)pffmpeg->pFrame->pts, (int)(pffmpeg->pFrame->pts - pffmpeg->llPrevTime));
	pffmpeg->llPrevTime = pffmpeg->pFrame->pts;
	
//	voffmpegVideoColorConvert (pffmpeg);

    static int iT = 0;
    iT += 33;
    pOutBuffer->Time = iT;
	
	return VO_ERR_NONE;
}

VO_U32 voffmpegVideoDecUninit(VO_HANDLE hDec)
{
	VOLOGI ("Handle %p", hDec);
	if (hDec == NULL)
		return VO_ERR_INVALID_ARG;

	VO_FFMPEG_CONTEXT * pffmpeg = (VO_FFMPEG_CONTEXT *)hDec;	

	if (pffmpeg->hCodec != NULL)
	{
		pffmpeg->sFunc.fClose (pffmpeg->hContext);	
		
		pffmpeg->sFunc.fFree (pffmpeg->hContext);
		pffmpeg->sFunc.fFree (pffmpeg->pFrame);
	}
	
	if (pffmpeg->sFunc.hDll != NULL)
	{
		//dlclose (pffmpeg->sFunc.hDll);
        FreeLibrary ((HMODULE)(pffmpeg->sFunc.hDll));
		pffmpeg->sFunc.hDll = NULL;
	}

    if (pffmpeg->sFunc.hDllU != NULL)
    {
        FreeLibrary ((HMODULE)(pffmpeg->sFunc.hDllU));
        pffmpeg->sFunc.hDllU = NULL;
    }
	
	if (pffmpeg->pHeadData != NULL)
		free (pffmpeg->pHeadData);

	if (pffmpeg->pRGBData != NULL)
		free (pffmpeg->pRGBData);
				
	free (pffmpeg);
		
	if (g_hvoffmepgDumpFile != NULL)
		fclose ((FILE *)g_hvoffmepgDumpFile);
				
	return VO_ERR_NONE;
	
}

VO_U32 voffmpegWrapSetParam(VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData)
{
	if (hDec == NULL)
		return VO_ERR_INVALID_ARG;
	VO_FFMPEG_CONTEXT * pffmpeg = (VO_FFMPEG_CONTEXT *)hDec;
		
	switch (uParamID)
	{
	case VO_PID_COMMON_HEADDATA:
		{
			VO_CODECBUFFER * pBuffer = (VO_CODECBUFFER *)pData;
			if (pffmpeg->pHeadData != NULL)
				free (pffmpeg->pHeadData);
			VOLOGI ("Head Data Info %p, size %d", pBuffer->Buffer, pBuffer->Length);
			pffmpeg->nHeadSize = pBuffer->Length;
			pffmpeg->pHeadData = (unsigned char *) malloc (1024 * 1024);
			memcpy (pffmpeg->pHeadData, pBuffer->Buffer, pBuffer->Length);
		}
		return VO_ERR_NONE;
		
	case VO_PID_COMMON_CPUNUM:
		{
			pffmpeg->nCpuNum = *(int *)pData;
			VOLOGI ("CPU Num %d", pffmpeg->nCpuNum);
		}
		return VO_ERR_NONE;

	default:
		break;
	}

	return VO_ERR_WRONG_PARAM_ID;
}

VO_U32 voffmpegWrapGetParam(VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData)
{
	return VO_ERR_WRONG_PARAM_ID;
}

//VO_S32 voGetvoffmpegVideoDecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
FFMPEGWRAPPER_API VO_S32 voGetH264DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	VOLOGI ("Flag %d", uFlag);
		
	pDecHandle->Init = voffmpegVideoDecInit;
	pDecHandle->SetInputData = voffmpegVideoDecSetInputData;
	pDecHandle->GetOutputData = voffmpegVideoDecGetOutputData;
	pDecHandle->Uninit = voffmpegVideoDecUninit;

	pDecHandle->SetParam = voffmpegWrapSetParam;
	pDecHandle->GetParam = voffmpegWrapGetParam;

	return VO_ERR_NONE;
}

FFMPEGWRAPPER_API VO_S32 voGetMPEG4DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	VOLOGI ("Flag %d", uFlag);
		
	pDecHandle->Init = voffmpegVideoDecInit;
	pDecHandle->SetInputData = voffmpegVideoDecSetInputData;
	pDecHandle->GetOutputData = voffmpegVideoDecGetOutputData;
	pDecHandle->Uninit = voffmpegVideoDecUninit;

	pDecHandle->SetParam = voffmpegWrapSetParam;
	pDecHandle->GetParam = voffmpegWrapGetParam;
			
	return VO_ERR_NONE;
}

VO_U32 voffmpegVideoDecInitFFMPEG (VO_FFMPEG_CONTEXT * pffmpegContext)
{
	if (pffmpegContext == NULL)
		return VO_ERR_INVALID_ARG;
	
	VO_FFMPEG_CONTEXT * pffmpeg = pffmpegContext;
	
	pffmpeg->sFunc.fRegAll ();	
	
	if (pffmpegContext->nCodecType == VO_VIDEO_CodingH264)
		pffmpegContext->hCodec = pffmpeg->sFunc.fFindDec (CODEC_ID_H264);
	else if (pffmpegContext->nCodecType == VO_VIDEO_CodingMPEG4)
		pffmpegContext->hCodec = pffmpeg->sFunc.fFindDec (CODEC_ID_MPEG4);
	else
		pffmpegContext->hCodec = pffmpeg->sFunc.fFindDec (CODEC_ID_H264);

			
	if (pffmpegContext->hCodec == NULL)
	{
		VOLOGE ("Cannot find H264 decoder!!");
		free (pffmpegContext);
		return VO_ERR_NOT_IMPLEMENT;
	}
	pffmpegContext->hContext = pffmpeg->sFunc.fAllocContext3 (pffmpegContext->hCodec);
	if (pffmpegContext->hContext == NULL)
	{
		VOLOGE ("Cannot alloc context!");
		free (pffmpegContext);
		return VO_ERR_NOT_IMPLEMENT;
	}
	pffmpegContext->pFrame = pffmpeg->sFunc.fAllocFrame ();
	if (pffmpegContext->pFrame == NULL)
	{
		VOLOGE ("Cannot alloc frame!!\r\n");
		free (pffmpegContext);
		return VO_ERR_NOT_IMPLEMENT;
	}	
	
	pffmpeg->sFunc.fInitPacket (&pffmpegContext->sPacket);

	if (pffmpegContext->nCpuNum > 1 && pffmpegContext->nCodecType == VO_VIDEO_CodingH264)
	{
		//for multi-frame setting
		pffmpegContext->hCodec->capabilities |= CODEC_CAP_FRAME_THREADS;
		pffmpegContext->hContext->flags &= ~(CODEC_FLAG_TRUNCATED|CODEC_FLAG_LOW_DELAY);
		pffmpegContext->hContext->flags2 &= ~CODEC_FLAG2_CHUNKS;
		pffmpegContext->hContext->thread_count = pffmpegContext->nCpuNum;
		pffmpegContext->hContext->skip_loop_filter = AVDISCARD_ALL;		
	}
	else
	{
		// we do not send complete frames 
		// if(codec->capabilities&CODEC_CAP_TRUNCATED)
		pffmpegContext->hContext->flags2|= CODEC_FLAG2_CHUNKS; 
	}

	// For some codecs, such as msmpeg4 and mpeg4, width and height
	// MUST be initialized there because this information is not
	// available in the bitstream. 
	// open it 
	if (pffmpeg->sFunc.fOpen2 (pffmpegContext->hContext, pffmpegContext->hCodec, NULL) < 0) 
	{
		VOLOGE ("could not open codec\n");
		free (pffmpegContext);
		return VO_ERR_NOT_IMPLEMENT;
	}
	
/*	
	{	
		H264Context * pH264 = pffmpegContext->hContext->priv_data;
		pH264->deblocking_filter = 1;
		
		MpegEncContext * pEnc = &pH264->s;
		//s->dsp.idct_permutation_type =1;
		dsputil_init(&pEnc->dsp, pffmpegContext->hContext);
	}	
*/		
	return VO_ERR_NONE;
}

VO_U32 voffmpegLoadLib (VO_FFMPEG_CONTEXT * pffmpegContext)
{
	VO_FFMPEG_CONTEXT * pCxt = pffmpegContext;
	
	char	szLib[1024];
	char	szPackageName[1024];
	FILE * hFile = fopen("/proc/self/cmdline", "rb");
	if (hFile != NULL)
	{  
		fgets(szPackageName, 1024, hFile);
		fclose(hFile);
		if (strstr (szPackageName, "/system/bin/mediaserver") == NULL)
			sprintf(szLib, "/data/data/%s/lib/libffmpeg.so", szPackageName);
		else
			return VO_ERR_INVALID_ARG;
	}
	
	VOLOGI ("ffmpeg file %s", szLib);

	pCxt->sFunc.hDllU = LoadLibrary (_T("C:\\ProgramData\\VisualOn\\BrowserPlugin\\avutil-52.dll"));
	if (pCxt->sFunc.hDllU == NULL)
	{
        pCxt->sFunc.hDllU = LoadLibrary (_T("avutil-52.dll"));
        if (pCxt->sFunc.hDllU == NULL)
        {
            return -1;
        }
	}

    pCxt->sFunc.hDll = LoadLibrary (_T("C:\\ProgramData\\VisualOn\\BrowserPlugin\\avcodec-55.dll"));
    if (pCxt->sFunc.hDll == NULL)
    {
        pCxt->sFunc.hDll = LoadLibrary (_T("avcodec-55.dll"));
        if (pCxt->sFunc.hDll == NULL)
        {
            return -1;
        }
    }

	pCxt->sFunc.fMalloc = (AV_MALLOC)GetProcAddress ((HMODULE)(pCxt->sFunc.hDllU), "av_malloc");
	VOLOGI ("av_malloc %p", pCxt->sFunc.fMalloc);

	pCxt->sFunc.fFree = (AV_FREE)GetProcAddress ((HMODULE)(pCxt->sFunc.hDllU), "av_free");
	VOLOGI ("av_free %p", pCxt->sFunc.fFree);

	pCxt->sFunc.fRegAll = (AVCODEC_REGISTER_ALL)GetProcAddress ((HMODULE)(pCxt->sFunc.hDll), "avcodec_register_all");
	VOLOGI ("avcodec_register_all %p", pCxt->sFunc.fRegAll);

	pCxt->sFunc.fFindDec = (AVCODEC_FIND_DECODER)GetProcAddress ((HMODULE)(pCxt->sFunc.hDll), "avcodec_find_decoder");
	VOLOGI ("avcodec_find_decoder %p", pCxt->sFunc.fFindDec);
	
	pCxt->sFunc.fAllocContext3 = (AVCODEC_ALLOC_CONTEXT3)GetProcAddress ((HMODULE)(pCxt->sFunc.hDll), "avcodec_alloc_context3");
	VOLOGI ("avcodec_alloc_context3 %p", pCxt->sFunc.fAllocContext3);

	pCxt->sFunc.fAllocFrame = (AVCODEC_ALLOC_FRAME)GetProcAddress ((HMODULE)(pCxt->sFunc.hDll), "avcodec_alloc_frame");
	VOLOGI ("avcodec_alloc_frame %p", pCxt->sFunc.fAllocFrame);
	
	pCxt->sFunc.fInitPacket = (AV_INIT_PACKET)GetProcAddress ((HMODULE)(pCxt->sFunc.hDll), "av_init_packet");
	VOLOGI ("av_init_packet %p", pCxt->sFunc.fInitPacket);
	
	pCxt->sFunc.fOpen2 = (AVCODEC_OPEN2)GetProcAddress ((HMODULE)(pCxt->sFunc.hDll), "avcodec_open2");
	VOLOGI ("avcodec_open2 %p", pCxt->sFunc.fOpen2);
	
	pCxt->sFunc.fDecVideo2 = (AVCODEC_DECODE_VIDEO2)GetProcAddress ((HMODULE)(pCxt->sFunc.hDll), "avcodec_decode_video2");
	VOLOGI ("avcodec_decode_video2 %p", pCxt->sFunc.fDecVideo2);

	pCxt->sFunc.fClose = (AVCODEC_CLOSE)GetProcAddress ((HMODULE)(pCxt->sFunc.hDll), "avcodec_close");
	VOLOGI ("avcodec_close %p", pCxt->sFunc.fClose);	
	
	return VO_ERR_NONE;
}

VO_U32 voffmpegVideoColorConvert (VO_FFMPEG_CONTEXT * pffmpegContext)
{
/*	
	AVFrame * 			pPic = pffmpegContext->pFrame;		
	VO_FFMPEG_CONTEXT * pffmpeg = pffmpegContext;
	
	if (pffmpeg->pRGBData == NULL)
	{
		pffmpeg->pRGBData = malloc (pPic->width * pPic->height * 4);
		
	    pffmpeg->pSWS = sws_getContext(pPic->width, pPic->height, PIX_FMT_YUV420P, 
	    									  pPic->width, pPic->height, PIX_FMT_RGB32,
	                        			 	  SWS_BILINEAR, NULL, NULL, NULL);
	                        			 	  
	   	VOLOGI ("W X H: %d X %d  RGB Data %p, SWS %p", pPic->width, pPic->height, pffmpeg->pRGBData, pffmpeg->pSWS);
	}
	
	uint8_t *rgb_src[4] = { pffmpeg->pRGBData, NULL, NULL, NULL };
    int rgb_stride[4]   = { 4 * pPic->width, 0, 0, 0 };

    uint8_t *src[4]     = { pffmpeg->pFrame->data[0], pffmpeg->pFrame->data[1], pffmpeg->pFrame->data[2], NULL};
    int stride[4]       = { pffmpeg->pFrame->linesize[0], pffmpeg->pFrame->linesize[1], pffmpeg->pFrame->linesize[2], 0};

	int nRC = sws_scale (pffmpeg->pSWS, src, stride, 0, pPic->height, rgb_src, rgb_stride);	
	
	unsigned char * pRGB = pffmpeg->pRGBData;
//	VOLOGI ("RC %d, RGB Buff: %02X%02X%02X%02X%02X%02X%02X%02X", nRC, pRGB[0], pRGB[1], pRGB[2], pRGB[3], pRGB[4], pRGB[5], pRGB[6], pRGB[7]);
*/		
	return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */