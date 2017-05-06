/** 
 * \file RealVideobase.h
 * \brief VisualOn RealVideo ID & sturcture, for SetParameter/GetParameter
 * \version 0.1
 * \date 10/31/2006 created
 * \author VisualOn
 */


#ifndef __VO_RealVideo_BASE_H_
#define __VO_RealVideo_BASE_H_


#include "sdkbase.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif
#pragma pack(push, 4)
#include "RealCommonbase.h"
#define MAX_FRAMES 8

enum
{
	VORV_ERR_NOOUTBUFFER = -1,
};
typedef enum
{
	VORV_FMT_RM		= 0,//rftt file format,like RV,RA,RM
	VORV_FMT_RAW	= 1,//raw bitstream for codec test
}VORV_FORMAT;
typedef enum
{
	VORV_G2	= 2,
	VORV_8	= 3,
	VORV_9	= 4,
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
	VOFRAMETYPE type;//[out]
}VORV_FrameType;
/**
 * RealVideo specific parameter id 
 * \see VOCOMMONPARAMETERID
 */
#define VOID_RealVideo_BASE 0x00100000
typedef enum
{
	VOID_RealVideo_FORMAT		    = VOID_RealVideo_BASE | 0x0001,	 /*!<[IN] it is VORV_FORMAT*/
	VOID_RealVideo_FLUSH			= VOID_RealVideo_BASE | 0x0002,  /*!<[IN] Notify the decoder to flush all the reserved frames in buffer,the params is a int */	
	VOID_RealVideo_MAXOUTPUTSIZE	= VOID_RealVideo_BASE | 0x0003,  /*!<[OUT]The max output size that the caller should support*/	
	VOID_RealVideo_INIT_PARAM		= VOID_RealVideo_BASE | 0x0004,	 /*!<[IN] it is same as VOID_COM_HEAD_DATA*/
	VOID_RealVideo_RAW_INIT_PARAM   = VOID_RealVideo_BASE | 0x0005,	 /*!<[IN] it is a pointer of VORAW_INITParam,only for raw bitstream test*/
	VOID_RealVideo_ENABLE_ADAPTIVE_DEBLOCK    = VOID_RealVideo_BASE | 0x0006,	 /*!<[IN] it is a int value.1:enable adaptive deblock,0:disable adaptive deblock,default is 0*/
	VOID_RealVideo_FrameType		= VOID_RealVideo_BASE | 0x0007,  /*!<[IN/OUT],probe the frametype according to the input bitstream,it is a pointer of VORV_FrameType*/
	VOID_RealVideo_DeblockingFlage  = VOID_RealVideo_BASE | 0x0008,  /*!<[IN],it can set deblocking flag 0(no deblocking), 1(deblocking)*/
	VOID_RealVideo_FLUSH_LAST_FRAME	= VOID_RealVideo_BASE | 0x0009,  /*!<[IN] Notify the decoder to flush reserved frames  in buffer,display the frame .the params is a int */
	VOID_RealVideo_YUV_MEM          = VOID_RealVideo_BASE | 0x000a,  /*!<[IN] Notify the decoder to change YUVMEM .the params is a int */
	VOID_RealVideo_Video_Querymem	= VOID_RealVideo_BASE | 0x0010   /*!<[IN] Set AMD memory parameter.the params is a int*/
}
VORealVideoPARAMETERID;

typedef struct  
{
	unsigned int                    YUVFlage;
	VOMEMORYOPERATOR*               pMemoryoper;
	VODDMEMORYOPERATOR*             pDDMemoryoper;
}VOMemParameter;//huwei 20081117 add memecopy

#pragma pack(pop)

#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif


#endif /* __VO_RealVideo_BASE_H_ */





