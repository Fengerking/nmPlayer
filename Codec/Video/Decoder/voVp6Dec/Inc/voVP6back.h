/**                                                                                                                             
* \file         voVP6back.h                                                                                         
* \brief    API definition for the VisualOn MPEG-4 and H.263 decoder    
* \version 0.1  
*/

#ifndef _voVP6Back_D_H_
#define _voVP6Back_D_H_

#include "voVP6Port.h"
#include "pbdll.h"

#  include "voCheck.h"
#  include "voIndex.h"

#ifdef __cplusplus
extern "C" {
#endif
  
#ifdef RVDS
#define voCDECL
#else//RVDS
#define voCDECL __cdecl
#endif//RVDS

#define VO_BACK_MAX_ENUM_VALUE	0X7FFFFFFF


/*!
* Defination of decoded frame type
*
*/
typedef enum
{
        I_FRAME_D                = 0,   /*!< I frame */
        P_FRAME_D                = 1,   /*!< P frame */
        B_FRAME_D                = 2,   /*!< B frame */
        S_FRAME_D                = 3,   /*!< S frame*/
        NULL_FRAME_D             = 4,   /*!<No output data*/
        MAX_FRAME_D              = VO_BACK_MAX_ENUM_VALUE
}DEC_FRAME_TYPE;

typedef enum
{
        YUVPLANAR420           = 0,   /*!< I frame */
        RGB565	               = 1,   /*!< P frame */
		MAX_OUT_TYPE           = VO_BACK_MAX_ENUM_VALUE
}DEC_OUT_TYPE;

/*!
* Return error
*
* Negative presents failed, including invalid configure parameters
* unsupported feature and decoding failures
*
*/     
#define SUCCEEDED_D                                       0x00000000  
#define FAILED_D										  0X80000000
enum
{
 INVALID_DEC_FLAG                                 = FAILED_D | 0x0001,
 INVALID_IN_BUF_ADR                               = FAILED_D | 0x0002,
 INVALID_IN_BUF_LEN                               = FAILED_D | 0x0003,
 INVALID_DATA                                     = FAILED_D | 0x0004,

 INVALID_INTERNAL_MEM_MALLOC_D                    = FAILED_D | 0x0005,
 FAILED_NO_ENOUGH_DATA                            = FAILED_D | 0x0006,
 
 FAILED_UNSUPORT_FEATURE                          = FAILED_D | 0x0007,
 FAILED_UNSUPORT_GMC                              = FAILED_D | 0x0008,
 FAILED_UNSUPORT_INTERLACE                        = FAILED_D | 0x0009,
 FAILED_UNSUPORT_PARTITION                        = FAILED_D | 0x000a,

 FAILED_UNSUPORT_SAC                              = FAILED_D | 0x000b,
 FAILED_UNSUPORT_OBMC                             = FAILED_D | 0x000c,
 FAILED_UNSUPORT_PBFRAME                          = FAILED_D | 0x000d,

 FAILED_TO_DECODE_HEADER                          = FAILED_D | 0x000e,
 FAILED_TO_DECODE_I_FRAME                         = FAILED_D | 0x000f,
 FAILED_TO_DECODE_P_FRAME                         = FAILED_D | 0x0010,
 FAILED_TO_DECODE_B_FRAME                         = FAILED_D | 0x0011,
 FAILED_TO_DECODE_S_FRAME                         = FAILED_D | 0x0012,
 FAILED_TO_DECODE_INTRA_MB                        = FAILED_D | 0x0013,
 FAILED_TO_DECODE_INTER_MB                        = FAILED_D | 0x0014,

 FAILED_TO_PREFRAME_BUF						      = FAILED_D | 0x0015,
 FAILED_TO_OUT_OF_MEMORY						  = FAILED_D | 0X0016,
 INVALID_PARAMETER_ID                             = FAILED_D | 0x0017,
 MAX_FAILED     									= VO_BACK_MAX_ENUM_VALUE
};

/*!
* Decoded frame infomation structure
* 
* To return the infomation of the decoded frame
*
*/
typedef struct
{
		DEC_OUT_TYPE           out_type;				   /*!< 0-YUV420, 1-RGB565 */  
        UINT8          *image_y;                   /*!< Output image YUV420 Y*/
        UINT8          *image_u;                   /*!< Output image YUV420 U*/
        UINT8          *image_v;                   /*!< Output image YUV420 V*/
        UINT32                    image_width;                /*!< Output image width */      
        UINT32                    image_height;               /*!< Output image height */     
        UINT32                    image_stride_y;             /*!< Out buffer stride of Lum */
        UINT32                    image_stride_uv;            /*!< Out buffer stride of Chroma*/
		UINT8          *rgb_out;
		UINT32						rgb_stride;
        DEC_FRAME_TYPE         image_type;                 /*!< 0-I frame, 1-P frame, 2-B frame, 3-S frame, 4-Null frame */
		UINT32						IsGetFirstFrame;			/*!<0-don't get the first frame, 1-get the first frame*/
		UINT32					   error_frame;					/* 0- no error in current frame, there is error in current frame*/
		voint64_t				time;
} DEC_FRAME_INFO;

extern void* voVP6_hModule;
#ifdef __cplusplus
}
#endif

#endif
