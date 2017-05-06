/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		videoEditorType.h

Contains:	videoEditorType header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-18		Leon			Create file
*******************************************************************************/

#ifndef __videoEDITORType_H__
#define __videoEDITORType_H__

#include "voedtType.h"

#include "vompType.h"
#include "voAAC.h"
#include "voOSFunc.h"
#include "voSink.h"

#include "voLog.h"

#define AV_TIME_BASE            1000
#define AUDIO_BUFFER_SIZE		1024*100 //100K

//internal action mode
#define VOEDT_ACTIONMODE_THUMNAIL							0x000010 /* both for audio & video*/
#define VOEDT_ACTIONMODE_POSITION							0x000020
/*get /set param*/
//#define VOEDT_PID_PLAYER_AUDIO_CODEC					0x0E000001   /*Get ,indicate the audio codec int * */
//#define VOEDT_PID_PLAYER_VIDEO_CODEC					0X0E000002   /*Get, indicate the vidoe codec int * */
#define VOEDT_PID_PLAYER_PLAY_MODE						0X0E000003	  /*Get, indicate the clip play mode (play onlyaudio/onlyvideo/audio&video) int*   */
//#define VOEDT_PID_PLAYER_CLIP_TYPE							0X0E000004	  /*Get, indicate the clip type (audio/video/audio*video/text) int*  */
//#define VOEDT_PID_PLAYER_VIDEO_FORMAT					0X0E000005   /*Get, indicate the clip video_format VOEDT_VIDEO_FORMAT*   */
//#define VOEDT_PID_PLAYER_AUDIO_FORMAT					0X0E000006   /*Get, indicate the clip video_format VO_AUDIO_FORMAT*   */
//#define VOEDT_PID_PLAYER_THUMBNAIL_BUFFER			0X0E000007  /*Get, indicate the media thumbnail pointer, VO_VIDEO_BUFFER**  */
#define VOEDT_PID_PLAYER_DATA_BUFFER						0X0E000008  /*Get,indicate the player data buffer, the value is eauql to VOEDT_PID_PLAYER_THUMBNAIL_BUFFER , VO_PBYTE*   */
#define VOEDT_PID_PLAYER_CLIP_PARAM						0X0E000009	 /*Get indicate the palyer's clip param ,VOEDT_CLIP_PARAM*  */
#define VOEDT_PID_PLAYER_TEXT_FORMAT						0X0E000010	 /*Get/Set, indicate the palyer's clip param ,VOEDT_TEXT_FORMAT*  */
//#define VOEDT_PID_PLAYER_DURATION							0X0E0000011	 /*Get/Set, indicate the palyer's duration ,VO_U32*  */
#define VOEDT_PID_PLAYER_CLIP_NAME							0X0E000012	 /*Get/Set, indicate the palyer's name ,VO_TCHAR*  */
#define VOEDT_PID_PLAYER_OUTPUT_VIDEO_FORMAT	0x0E000020 /* Get/Set, indicate the video format from VOMP, VOMP_VIDEO_FORMAT* */
#define VOEDT_PID_PLAYER_LIB_OPERATOR					0x0E000021
#define VOEDT_PID_PLAYER_AUDIO_FORMAT_ORIGINAL 0X0E000022
#define VOEDT_PID_PLAYER_AUDIO_FORMAT_TARGENT  0X0E000023


#define VOEDT_AUDIO_DEFAULT_VOLUME 50
#define VOEDT_AUDIO_MAX_VOLUME	   500
#define  VOEDT_MAX_CLIP_COUNT				10
//#define VOEDT_
//	nType_BREADCIRCLE = 0
//	,nType_CHESSBOARD
//	,nType_CONVOLUTE
//	,nType_KALEIDOSCOPE
//	,nType_STEEL
//	,nType_TREE
//	,nType_WINDOWS
//	,nType_ZOOMOUT
//	,Transition_NONE			//no effction
//}VO_U32;
#ifdef WIN32
const VO_CHAR MASKDATA[][255] = {"d:/data_cmp/EffectFiles/breadcircle.shp"
,"d:/data_cmp/EffectFiles/ChessBoard.shp"
,"d:/data_cmp/EffectFiles/convolute.shp"
,"d:/data_cmp/EffectFiles/kaleidoscope.shp"
,"d:/data_cmp/EffectFiles/Steel.shp"
,"d:/data_cmp/EffectFiles/tree.shp"
,"d:/data_cmp/EffectFiles/Windows.shp"
,"d:/data_cmp/EffectFiles/ZoomOut.shp"};
#else
const VO_CHAR MASKDATA[][255] = {"data/data/com.visualon.editor/app_EffectFiles/breadcircle.shp"
,"data/data/com.visualon.editor/app_EffectFiles/ChessBoard.shp"
,"data/data/com.visualon.editor/app_EffectFiles/convolute.shp"
,"data/data/com.visualon.editor/app_EffectFiles/kaleidoscope.shp"
,"data/data/com.visualon.editor/app_EffectFiles/Steel.shp"
,"data/data/com.visualon.editor/app_EffectFiles/tree.shp"
,"data/data/com.visualon.editor/app_EffectFiles/Windows.shp"
,"data/data/com.visualon.editor/app_EffectFiles/ZoomOut.shp"};
#endif
/*VideoEditor action mode*/
#define	VOEDT_ACTIONMODE_TRIM									0x000008

//SetDataSource ,id set text url 
#define VOEDT_PID_PLAYER_TEXT_URL								0X0000000B	/* Set text string , only for text  VO_CHAR*  */
#define VOEDT_PID_PLAYER_TEXT_BUFFER							0x0000000C

typedef struct  
{
	VO_CODECBUFFER *buffer;
	VO_VIDEO_FRAMETYPE pType;
	VO_U32 Duration;
	int nSampleType;
}VOEDT_ENCODER_SAMPLE;

typedef struct  
{
	VO_VOID *pData;
	VO_VOID *pUserData; 
}VOEDT_PLAYER_CALLBACK_BUFFER;

typedef struct 
{
	/*clip title or name*/
	VO_CHAR *cTitle;
	/*clip type ,audio \video\text*/
	int nClipType;	
	/*clip duration	*no use for text	*/
	VO_U32 nDuration;

	VOEDT_AUDIO_FORMAT audioFormat;
	VOEDT_VIDEO_FORMAT  videoFormat;
}VOEDT_CLIP_INFO;

typedef struct  
{
	int llTime_previous;
	VOMP_BUFFERTYPE *pBuffer;
	VO_U32 nType;
	float					  fTransitionValue;//transition step
	VOEDT_TRANSITION_EFFECT *pTransition;
	VOEDT_CLIP_POSITION_FORMAT originVFormat;//
	VOMP_AUDIO_FORMAT originAFormat;//
	VOEDT_CLIP_POSITION_FORMAT targetVFormat;
	VOMP_AUDIO_FORMAT targetAFormat;
}CPLAYER_A_SAMPLE;
typedef struct  CPLAYER_OUTPUT_BUFFER
{
	CPLAYER_A_SAMPLE *a_sample;
	int nID;//VOMP_PID_AUDIO_SAMPLE;VOMP_PID_VIDEO_SAMPLE;
	VO_VOID*		pUserData;
}CPLAYER_OUTPUT_BUFFER;

typedef struct  
{
	VOEDT_OUTPUT_SINK_PARAM     SinkParam;
	VOEDT_OUTPUT_VIDEO_PARAM	VideoParam;
	VOEDT_OUTPUT_AUDIO_PARAM	AudioParam;
}VOEDT_OUTPUT_PARAM;


typedef void (VO_API *VOEDTENCODERCALLBACKPROC)(VO_S32 nPid,VOEDT_PLAYER_CALLBACK_BUFFER *pValue);

#define CheckWidthHeight(w,h) \
			{	\
				h =  (h +15)&0xfff0; \
				w = (w+ 15)&0xfff0; \
			}

static VO_U32 EX_Release_Buffer(VOMP_BUFFERTYPE *buf)
{
	VO_VIDEO_BUFFER **v_buffer = (VO_VIDEO_BUFFER**)(&buf->pBuffer);
	if((*v_buffer))
	{
		switch((*v_buffer)->ColorType)
		{
		case VO_COLOR_ARGB32_PACKED:
		case VO_COLOR_RGB32_PACKED:
				if((*v_buffer)->Buffer[0])
					delete (*v_buffer)->Buffer[0];
				(*v_buffer)->Buffer[0]= NULL;
				break;
		case VO_COLOR_YUV_PLANAR420:
			for(int i= 0;i < 3; i++)
			{
				if((*v_buffer)->Buffer[i])
					delete (*v_buffer)->Buffer[i];
				(*v_buffer)->Buffer[i]= NULL;
			}
			break;
		}
		delete (*v_buffer);
		(*v_buffer) = NULL;
	}
	memset(buf, 0x00, sizeof(VOMP_BUFFERTYPE));
	return 0;
}
//return buffer size of the video frame
static VO_U32 EX_INIT_BUFFER3(VOMP_VIDEO_FORMAT vf, VOMP_BUFFERTYPE *buf, int flag = 1)
{
	VO_U32 nRc = VO_ERR_NONE;
	VO_U32 stride = ( vf.Width +15 ) & 0xFFF0;
	VO_U32 h = vf.Height;
	VO_U32 nFrameSize = 0;
//	VO_VIDEO_BUFFER *tmpvb = (VO_VIDEO_BUFFER*)buf->pBuffer;
	VO_VIDEO_BUFFER **v_buffer = (VO_VIDEO_BUFFER**)(&buf->pBuffer);
	switch(vf.Type)
	{
	case VO_COLOR_YUV_PLANAR420:
		{
			nFrameSize = stride * h * 3/2;
			
			if(*v_buffer != NULL)
			{
				if( (*v_buffer)->Stride[0] != stride || (*v_buffer)->Stride[1] != stride /2 || (*v_buffer)->Stride[2] != stride/2)
				{
					delete (*v_buffer)->Buffer[0];
					delete *v_buffer;
				}
				else 
					break;
			}
		
			*v_buffer = new VO_VIDEO_BUFFER;
			(*v_buffer)->Buffer[0] =  (*v_buffer)->Buffer[1] =  (*v_buffer)->Buffer[2] = NULL;
			(*v_buffer)->Stride[0] = (*v_buffer)->Stride[1] = (*v_buffer)->Stride[2] = 0;
			VO_PBYTE tmpbuf = new VO_BYTE[ nFrameSize];

			(*v_buffer)->Buffer[0]= tmpbuf;
			(*v_buffer)->Buffer[1]= tmpbuf + (stride) * (h);
			(*v_buffer)->Buffer[2] = (*v_buffer)->Buffer[1] + (stride) * (h) / 4;
			(*v_buffer)->Stride[0] = (stride);
			(*v_buffer)->Stride[1] = (stride)/2;
			(*v_buffer)->Stride[2] = (stride)/2;
			(*v_buffer)->ColorType = (VO_IV_COLORTYPE)vf.Type;
			break;
		}
	case VO_COLOR_RGB565_PACKED:
		{			
			nFrameSize = stride * h *2 ;
			if(*v_buffer!= NULL)
			{
				if( (*v_buffer)->Stride[0] != stride || (*v_buffer)->Stride[1] != 0 || (*v_buffer)->Stride[2] != 0)
					delete (*v_buffer)->Buffer[0];
				else 
					break;
			}
			
			*v_buffer = new VO_VIDEO_BUFFER;
			(*v_buffer)->Buffer[0] =  (*v_buffer)->Buffer[1] =  (*v_buffer)->Buffer[2] = NULL;
			(*v_buffer)->Stride[0] = (*v_buffer)->Stride[1] = (*v_buffer)->Stride[2] = 0;
			VO_PBYTE tmpbuf = new VO_BYTE[ nFrameSize];
		
			(*v_buffer)->Buffer[0]= tmpbuf;
			(*v_buffer)->Stride[0] = (stride *2);	
			(*v_buffer)->ColorType = (VO_IV_COLORTYPE)vf.Type;

			break;
		}
	case VO_COLOR_ARGB32_PACKED:
	case VO_COLOR_RGB32_PACKED:
		{
			stride = (( vf.Width +15 ) & 0xFFF0)  * 4;	
			nFrameSize = stride * vf.Height;
			if(flag == 1)
			{
				if(buf->pBuffer )delete []buf->pBuffer;
				buf->pBuffer = new VO_BYTE[nFrameSize];
				buf->pData = (VO_VOID*) &stride;
			}
			else
			{
				*v_buffer = new VO_VIDEO_BUFFER;
				(*v_buffer)->Buffer[0] =  (*v_buffer)->Buffer[1] =  (*v_buffer)->Buffer[2] = NULL;
				(*v_buffer)->Stride[0] = (*v_buffer)->Stride[1] = (*v_buffer)->Stride[2] = 0;
				VO_PBYTE tmpbuf = new VO_BYTE[ nFrameSize];

				(*v_buffer)->Buffer[0]= tmpbuf;
				(*v_buffer)->Stride[0] = stride ;	
				(*v_buffer)->ColorType = (VO_IV_COLORTYPE)vf.Type;
			}
		
		
			break;
		}
	}	
	buf->nSize = nFrameSize;
	return nFrameSize;
}
static VO_U32 EX_INIT_BUFFER2(VOMP_VIDEO_FORMAT vf, VOMP_BUFFERTYPE *buffertype)
{
	VO_U32 nRc = VO_ERR_NONE;
	VO_U32 stride = ( vf.Width +15 ) & 0xFFF0;
	VO_U32 h = vf.Height;

	switch(vf.Type)
	{
	case VO_COLOR_YUV_PLANAR420:
		{
			VO_VIDEO_BUFFER * buf = (VO_VIDEO_BUFFER*)buffertype;
			if(buf != NULL)
			{
				delete buf->Buffer[0];
				buf->Buffer[0] =  buf->Buffer[1] =  buf->Buffer[2] = NULL;
				buf->Stride[0] = buf->Stride[1] = buf->Stride[2] = 0;
				delete buf;
				buf = NULL;
			}
			buf = new VO_VIDEO_BUFFER;
			VO_U32 nFrameSize = stride * h * 3/2;
			VO_PBYTE tmpbuf = new VO_BYTE[ nFrameSize];
			buf->Buffer[0]= tmpbuf;
			buf->Buffer[1]= tmpbuf + (stride) * (h);
			buf->Buffer[2] = buf->Buffer[1] + (stride) * (h) / 4;
			buf->Stride[0] = (stride);
			buf->Stride[1] = (stride)/2;
			buf->Stride[2] = (stride)/2;
			buf->ColorType = (VO_IV_COLORTYPE)vf.Type;
			
			buffertype->nSize = nFrameSize;
			buffertype->pBuffer = (unsigned char*)buf;
			buffertype->pData = &stride;
			break;
		}
	case VO_COLOR_RGB565_PACKED:
		{
			VO_VIDEO_BUFFER * buf = (VO_VIDEO_BUFFER*)buffertype;
			if(buf != NULL)
			{
				delete buf->Buffer[0];
				buf->Buffer[0] =  buf->Buffer[1] =  buf->Buffer[2] = NULL;
				buf->Stride[0] = buf->Stride[1] = buf->Stride[2] = 0;
				delete buf;
				buf = NULL;
			}
			buf = new VO_VIDEO_BUFFER;
			VO_U32 nFrameSize = stride * h *2 ;
			VO_PBYTE tmpbuf = new VO_BYTE[ nFrameSize];
			buf->Buffer[0]= tmpbuf;
			buf->Buffer[1]= tmpbuf;
			buf->Buffer[2] = tmpbuf;
			buf->Stride[0] = (stride * 2);
			buf->Stride[1] = (stride * 2);
			buf->Stride[2] =(stride * 2);
			buf->ColorType = (VO_IV_COLORTYPE)vf.Type;

			buffertype->nSize = nFrameSize;
			buffertype->pBuffer = (unsigned char*)buf;
			buffertype->pData = &stride;
			break;
		}
	}	
	return nRc;
}
/*only for video and its type is YUV420*/
static int EX_INIT_BUFFER(int w,int h,VO_VIDEO_BUFFER *buf) 
{
								int nFrameSize = (w) * (h) * 3/2;
								VO_PBYTE tmpbuf = new VO_BYTE[ nFrameSize];
								if(!tmpbuf) return VO_ERR_OUTOF_MEMORY;
								buf->Buffer[0]= tmpbuf;
								memset(buf->Buffer[0],0x00,nFrameSize);
								buf->Buffer[1]= tmpbuf + (w) * (h);
								//memset(buf->Buffer[1],0x80,(w) * (h) /4);
								buf->Buffer[2] = buf->Buffer[1] + (w) * (h) / 4;
								//memset(buf->Buffer[2],0x80,(w) * (h) /4);
								buf->Stride[0] = (w);
								buf->Stride[1] = (w)/2;
								buf->Stride[2] = (w)/2;
								buf->ColorType = VO_COLOR_YUV_PLANAR420;
								return 0;
}

static void EX_COPY_BUFFER1(int h,VOMP_BUFFERTYPE *pbuf1,VOMP_BUFFERTYPE *pbuf2,int isVideo) 
{
	if(!pbuf2) return;

	memcpy(pbuf1->pBuffer,pbuf2->pBuffer,pbuf2->nSize);

	pbuf1->llTime = pbuf2->llTime;
	pbuf1->nFlag = pbuf2->nFlag;
	pbuf1->nReserve = pbuf2->nReserve;
	pbuf1->nSize = pbuf2->nSize;
	pbuf1->pData = pbuf2->pData;

}
static void EX_COPY_BUFFER(int h,VOMP_BUFFERTYPE *pbuf1,VOMP_BUFFERTYPE *pbuf2,int isVideo) 
{
	if(!pbuf2) return;
	if(isVideo)
	{
		VO_VIDEO_BUFFER *vbf1 = (VO_VIDEO_BUFFER*)pbuf1->pBuffer;
		VO_VIDEO_BUFFER *vbf2 = (VO_VIDEO_BUFFER*)pbuf2->pBuffer;
		int nh = (h);
		int len = 0; 
		if(vbf1->Stride[0] != vbf2->Stride[0])
		{
			len = vbf1->Stride[0];
			for(int height = 0;height < nh;height ++)
			{
				memcpy(vbf1->Buffer[0]+ height * vbf1->Stride[0],(vbf2->Buffer[0] + height * vbf2->Stride[0]),len);
			}
		}
		else
		{
			len = vbf1->Stride[0] * nh; 
			memcpy(vbf1->Buffer[0],vbf2->Buffer[0],len);
		}
		if(vbf1->Stride[1] != vbf2->Stride[1] || vbf1->Stride[2] != vbf2->Stride[2])
		{
			for(int height = 0;height < nh /2;height ++)
			{
				len = vbf1->Stride[1];
				memcpy(vbf1->Buffer[1]+ height * vbf1->Stride[1],vbf2->Buffer[1] + height * vbf2->Stride[1],len);
				len = vbf1->Stride[2]; 
				memcpy(vbf1->Buffer[2]+ height * vbf1->Stride[2],vbf2->Buffer[2] + height * vbf2->Stride[2],len);
			}
		}
		else
		{
			len = vbf1->Stride[1] * nh >>1; 
			memcpy(vbf1->Buffer[1],vbf2->Buffer[1],len);
			len = vbf1->Stride[2] * nh >>1; 
			memcpy(vbf1->Buffer[2],vbf2->Buffer[2],len);
		}
		vbf1->ColorType = vbf2->ColorType;
		vbf1->Time = vbf2->Time;
	}
	else
	{
		memcpy(pbuf1->pBuffer,pbuf2->pBuffer,pbuf2->nSize);
	}
	pbuf1->llTime = pbuf2->llTime;
	pbuf1->nFlag = pbuf2->nFlag;
	pbuf1->nReserve = pbuf2->nReserve;
	pbuf1->nSize = pbuf2->nSize;
	pbuf1->pData = pbuf2->pData;
							
}

#define SampleStamp_MIN(n0, n1, n2) \
			VO_BOOL(abs((n0) - (n1)) > abs((n0) - (n2)))


#define CHECK_FAIL(nRC) \
	if(nRC != VOEDT_ERR_NONE)\
	{ \
		VOLOGE("CHECK FAIL ,0X%08X",nRC);\
		return nRC;\
	}
//VO_BOOL SampleStamp_MIN(VO_U64 syncTimeStamp,VO_U32 previous,VO_U32 present)
//{
//	return VO_BOOL(abs(syncTimeStamp - previous) > abs(syncTimeStamp - present));
//}
#endif