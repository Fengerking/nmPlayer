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


#ifndef __VO_RealVideo_DEC_H_
#define __VO_RealVideo_DEC_H_

#include <voVideo.h>
#include <viMem.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)

#define MAX_FRAMES 8

	typedef struct
	{
		unsigned long   ulLength;	//header length
		unsigned long   ulMOFTag;	//media object format
		unsigned long   ulSubMOFTag;//codec id
		unsigned short   usWidth;	//frame width
		unsigned short   usHeight;	//frame height
		unsigned short   usBitCount;//Number of bits per pixel.
		unsigned short   usPadWidth;
		unsigned short   usPadHeight;
		unsigned long  ufFramesPerSecond;//Frame rate of the video
		unsigned long   ulOpaqueDataSize;
		unsigned char*    pOpaqueData;
	} VORV_FORMAT_INFO;

	/*
	* RV frame struct.
	*/
	typedef struct
	{
		unsigned long bIsValid;
		unsigned long ulOffset;
	} VORV_SEGMENT;

	typedef struct
	{
		unsigned long		ulDataLen;
		unsigned char*		pData;
		unsigned long             ulTimestamp;
		unsigned short             usSequenceNum;
		unsigned short            usFlags;
		unsigned long             bLastPacket;
		unsigned long             ulNumSegments;
		VORV_SEGMENT*				pSegment;
	} VORV_FRAME;

	typedef struct
	{
		unsigned long             ulNumSegments;
		VORV_SEGMENT*			  pSegment;
	} VORV_FRAME_HU;

#define VO_ERR_DEC_RV_BASE     VO_ERR_BASE | VO_INDEX_DEC_RV

	typedef enum
	{
		VORV_ERR_NOOUTBUFFER = -1,
		VORV_ERR_NOOUTBUFFER_MAX = VO_MAX_ENUM_VALUE
	}
	VORVDECRETURNCODE;

	typedef enum
	{
		VORV_FMT_RM		= 0,//rftt file format,like RV,RA,RM
		VORV_FMT_RAW	= 1,//raw bitstream for codec test
		VORV_FMT_MAX    = VO_MAX_ENUM_VALUE
	}VORV_FORMAT;

	typedef enum
	{
		VORV_G2	= 2,
		VORV_8	= 3,
		VORV_9	= 4,
		VORV_VERSION_MAX = VO_MAX_ENUM_VALUE
	}VORV_VERSION;

	typedef struct
	{
		VORV_VERSION version;
		int			 width;
		int			 height;
	}VORAW_INITParam;

	typedef struct
	{
		char* bits;//[in]
		int   size;
		VO_VIDEO_FRAMETYPE type;//[out]
	}VORV_FrameType;
	/**
	* RealVideo specific parameter id
	* \see VOCOMMONPARAMETERID
	*/
#define VO_PID_DEC_RealVideo_BASE                      VO_PID_COMMON_BASE | VO_INDEX_DEC_RV
	typedef enum
	{
		VO_PID_DEC_RealVideo_FORMAT		              = VO_PID_DEC_RealVideo_BASE | 0x0001,	 /*!<[IN] it is VORV_FORMAT*/
		VO_PID_DEC_RealVideo_FLUSH			          = VO_PID_DEC_RealVideo_BASE | 0x0002,  /*!<[IN] Notify the decoder to flush all the reserved frames in buffer,the params is a int */
		VO_PID_DEC_RealVideo_MAXOUTPUTSIZE	          = VO_PID_DEC_RealVideo_BASE | 0x0003,  /*!<[OUT]The max output size that the caller should support*/
		VO_PID_DEC_RealVideo_INIT_PARAM		          = VO_PID_DEC_RealVideo_BASE | 0x0004,	 /*!<[IN] it is same as VOID_COM_HEAD_DATA*/
		VO_PID_DEC_RealVideo_RAW_INIT_PARAM           = VO_PID_DEC_RealVideo_BASE | 0x0005,	 /*!<[IN] it is a pointer of VORAW_INITParam,only for raw bitstream test*/
		VO_PID_DEC_RealVideo_ENABLE_ADAPTIVE_DEBLOCK  = VO_PID_DEC_RealVideo_BASE | 0x0006,	 /*!<[IN] it is a int value.1:enable adaptive deblock,0:disable adaptive deblock,default is 0*/
		VO_PID_DEC_RealVideo_FrameType		          = VO_PID_DEC_RealVideo_BASE | 0x0007,  /*!<[IN/OUT],probe the frametype according to the input bitstream,it is a pointer of VORV_FrameType*/
		VO_PID_DEC_RealVideo_DeblockingFlage          = VO_PID_DEC_RealVideo_BASE | 0x0008,  /*!<[IN],it can set deblocking flag 0(no deblocking), 1(deblocking)*/
		VO_PID_DEC_RealVideo_GET_VIDEOFORMAT          = VO_PID_DEC_RealVideo_BASE | 0x0009,  /*!<[OUT],it can get video format*/
		VO_PID_DEC_RealVideo_YUV_MEM                  = VO_PID_DEC_RealVideo_BASE | 0x000a,  /*!<[IN] Notify the decoder to change YUVMEM .the params is a int */
		VO_PID_DEC_RealVideo_GET_VIDEO_WIDTH          = VO_PID_DEC_RealVideo_BASE | 0x000b,  /*!<[OUT],it can get video width*/
		VO_PID_DEC_RealVideo_GET_VIDEO_HEIGHT         = VO_PID_DEC_RealVideo_BASE | 0x000c,  /*!<[OUT],it can get video height*/
		VO_PID_DEC_RealVideo_Video_Querymem	          = VO_PID_DEC_RealVideo_BASE | 0x0010,   /*!<[IN] Set AMD memory parameter.the params is a int*/
		VO_PID_DEC_RealVideo_GETLASTOUTVIDEOFORMAT    = VO_PID_DEC_RealVideo_BASE | 0x0011,   /*!<[IN] Get video format of last frame.the parameter is a Boolean(LONG integer)*/
		VO_PID_DEC_RealVideo_GETLASTOUTVIDEOBUFFER    = VO_PID_DEC_RealVideo_BASE | 0x0012,   /*!<[IN] Get video buffer of last frame.the parameter is a Boolean(LONG integer)*/
		VO_PID_DEC_RealVideo_SHAREDMEMWAITOUTPUT      = VO_PID_DEC_RealVideo_BASE | 0x0013,   /*!<[IN] Get video no decoder.the parameter is a Boolean(LONG integer)*/
		VO_PID_DEC_RealVideo_DISABLEDEBLOCKING        = VO_PID_DEC_RealVideo_BASE | 0x0014,   /*!<[IN] it can disable deblocking (1:disable deblocking)*/
		VO_PID_DEC_RealVideo_VO_COLOR_YUV_420_PACK    = VO_PID_DEC_RealVideo_BASE | 0x0015,   /*!<[IN] You should set 1 if you want to get VO_COLOR_YUV_420_PACK  output format*/
		VO_PID_DEC_RealVideo_THREADS                  = VO_PID_DEC_RealVideo_BASE | 0x0016,   /*!<[IN] If you  set 1 for one thread, set 2 for two thread*/
		VO_PID_DEC_RealVideo_MAX                      = VO_MAX_ENUM_VALUE
	}
	VORealVideoPARAMETERID;

	/**
	* Get video decoder API interface
	* \param pDecHandle [IN/OUT] Return the MPEG4 Decoder API handle.
	* \param uFlag,reserved
	* \retval VO_ERR_OK Succeeded.
	*/
	VO_S32 VO_API voGetRVDecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);

#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* __VO_RealVideo_DEC_H_ */
