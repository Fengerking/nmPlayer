/** 
 * \file voccrender.h
 * \brief VisualOn Color Conversion(YUV420 Planar to RGB565) for render API
 * \version 0.1
 * \date 12/6/2006 created
 * \author VisualOn
 */


#ifndef _VOCCRENDER_H_
#define _VOCCRENDER_H_

#include "sdkbase.h"
#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE) || defined(RVDS)
#  include "voCheck.h"
#  include "voIndex.h"
#elif defined(LINUX)
#  include <string.h>
#endif

#if defined(LINUX)
#	define __cdecl 
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)

typedef void* CC_HND;

/**
 * Color Conversion render return code 
 */
typedef enum
{
	CC_ERR  = -1,	/*!< error */
	CC_OK   = 0,	/*!< succeed */
}
CC_RC;

/*!
* Defination of rotation type of output  
*
*/
//typedef enum
//{
//	RT_DISABLE	    = 0,  /*!< No rotation */
//	RT_90L		    = 1,  /*!< Left rotation 90 */
//	RT_90R          = 2,  /*!< Right rotation 90 */
//}RTTYPE;

/*!
* Defination of input type  
*
*/
typedef enum
{
	YUV420PLANAR	    = 0,  /*!< YUV 420 Planar */
	RGB24			    = 1,  /*!< RGB24 */
	RGB565				= 2   /*!< RGB565 */
}INTYPE;

/**
 * \param hnd		[in] CC instance handle.
 * \param in_width	[in] input image width.
 * \param out_width [in] output image width.
 * \param in_height [in] input image height.
 * \param out_height[in] output image height.
 * \param rt		[in] rotation type.
 * \retval CC_OK Succeeded.
 */
extern CC_RC __cdecl cc_ini(CC_HND* hnd, int in_width, int out_width, int in_height, int out_height, RTTYPE rt, INTYPE in_type, INTYPE out_type);

/**
 * \param hnd		[in] CC instance handle.
 * \param y			[in] address of Y planar.
 * \param u			[in] address of U planar.
 * \param v			[in] address of V planar.
 * \param in_stride [in] iput image stride(byte unit).
 * \param dst		[in] address of RGB565 output.
 * \param out_stride[in] output image stride(byte unit).
 * \param mb_skip   [in] address of skiped mb infomation, Set NULL without skiped mb infomation.
 * \retval CC_OK Succeeded.
 */
extern CC_RC __cdecl cc_process(CC_HND hnd, unsigned char *y, unsigned char *u, unsigned char *v, int in_stride[3], unsigned char *dst, int out_stride, unsigned char *mb_skip);

/**
 * \param hnd		[in] CC instance handle.
 * \retval CC_OK Succeeded.
 */
extern CC_RC __cdecl cc_uini(CC_HND hnd);

extern void * voCCRRR_hModule;

#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif 