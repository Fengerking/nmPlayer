/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/

#ifndef _CCINTERNAL_H__
#define _CCINTERNAL_H__

#include "voYYDef_CC.h"
#include "voCCRRR.h"
#include "voColorConversion.h"
#include "voMMCCRRSDID.h"

#ifdef LINUX_DEBUG
#define LOG_TAG "vomejni"
#define VOINFO(format, ...) { \
	LOGI("[david] %s()->%d: " format, __FUNCTION__, __LINE__, __VA_ARGS__); }
#endif // LINUX_DEBUG

#ifdef VOX86
//#include "pthread.h"
//#include "semaphore.h"
//#include "h265dec_RVThread.h"
#include "winpthreads.h"
#endif

#ifdef VOX86
#define MAX_THREADS 16

typedef enum{
   RGB32ResizeNoRot = 0,
   RGB32ResizeNoRotHalf,   	
   RGB32ResizeRot90l,
   RGB32ResizeRot90lHalf,
   RGB32ResizeRot90r,   
   RGB32ResizeRot90rHalf,   
   RGB32ResizeRot180,
   ARGB32ResizeNoRot,
   ARGB32ResizeNoRotHalf,	
   ARGB32ResizeRot90l,
   ARGB32ResizeRot90lHalf,
   ARGB32ResizeRot90r,   
   ARGB32ResizeRot90rHalf,	
   ARGB32ResizeRot180,
   RGB32NoResizeNoRot,
   RGB32NoResizeRot,
   ARGB32NoResizeNoRot,
   ARGB32NoResizeRot,   
};

#ifndef THREAD_FUNCTION
#define THREAD_FUNCTION void *
#endif

typedef struct {
      ClrConvData *conv_data;
      VO_S32 *  x_resize_tab;	
      VO_S32 *  y_resize_tab;	  
      struct CC_HND *cc_hnd;	  
      VO_S32 bExit;
       pthread_mutex_t  clrconv_mutex;  
      pthread_cond_t    clrconv_cond; 
      pthread_cond_t    cc_end_cond;	  
      VO_S32    thread_id[MAX_THREADS]; 	
       VO_S32    thdIdx; 
      volatile VO_S32    bRunning;
	 VO_S32   totBlkLine;
	 VO_S32   curBlkLine;
	VO_S32   height_remainder;
	VO_S32   completed;
	VO_S32   thdNum;
	VO_S32   thdBufSize;
}ClrConvEnv;
#endif

struct CC_HND{
	VO_S32 *x_resize_tab;
	VO_S32 *y_resize_tab;
	VO_U8  *yuv_nv12[2];
	VO_U8  *scale_buf[3];
	VO_U8  *pYUVBuffer;
	VO_U8  *pTotalBuffer;
	VO_S32 *yuv2rgbmatrix;
	VO_S32 ini_flg;
	VO_S32 ccrrr_init_flage;
	VO_PTR phCheck;
	ClrConvData conv_internal;	
	VO_HANDLE ClrConvFun;
#ifdef VOX86
        ClrConvEnv *ClrConvEnvP;
    VO_S32  ClrConvFunIndex;
#endif
};

typedef VOCCRETURNCODE(CLRCONV_FUNC)(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab, struct CC_HND * cc_hnd);
typedef CLRCONV_FUNC *CLRCONV_FUNC_PTR;

typedef void(YUV422INTERLACETORGB16_MB_FUNC)(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
											 const VO_S32* const param_tab, const VO_U32 isRGB565);
typedef YUV422INTERLACETORGB16_MB_FUNC *YUV422INTERLACETORGB16_MB_FUNC_PTR;

typedef void(CC_MB)(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v,const VO_S32 in_stride,VO_U8 *out_buf, const VO_S32 out_stride,
					VO_S32 width, VO_S32 height,const VO_S32 uin_stride, const VO_S32 vin_stride);
typedef CC_MB *CC_MB_PTR;

typedef void(CC_MB_NEW)(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v,const VO_S32 in_stride,VO_U8 *out_buf,const VO_S32 out_stride, 
						VO_S32 width, VO_S32 height,const VO_S32 uin_stride, const VO_S32 vin_stride, VO_S32 * const param_tab);
typedef CC_MB_NEW *CC_MB_NEW_PTR;

typedef void(YUV420PLANARTOYUV420_MB_ROTATION_FUNC)(VO_U8 *y, VO_U8 *u, VO_U8 *v,VO_S32 in_stride,VO_S32 uin_stride,
													VO_S32 vin_stride,VO_U8 *dsty,VO_U8 *dstu, VO_U8 *dstv);
typedef YUV420PLANARTOYUV420_MB_ROTATION_FUNC *YUV420PLANARTOYUV420_MB_ROTATION_PTR;




//extern VO_S32 *yuv2rgbmatrix;
extern VO_S32 ditherrb[2][2];
extern VO_S32 ditherg[2][2];

#define TP_IS_PLANAR_420(type) (type == VO_COLOR_YUV_PLANAR420 || type == VO_COLOR_YVU_PLANAR420)
#define TP_IS_PLANAR(type) (type == VO_COLOR_YUV_PLANAR420 || type == VO_COLOR_YVU_PLANAR420 ||\
							type == VO_COLOR_YUV_PLANAR422_12 || type == VO_COLOR_YVU_PLANAR422_12||\
							type == VO_COLOR_YUV_PLANAR444 || type == VO_COLOR_YUV_PLANAR422_21||\
							type == VO_COLOR_YUV_PLANAR411 || type == VO_COLOR_YUV_PLANAR411V||\
							type == VO_COLOR_RGB888_PLANAR)
#define TP_IS_RGB565_PACKD(type)(type == VO_COLOR_RGB565_PACKED || type == VO_COLOR_RGB555_PACKED)

#define TP_IS_PLANAR_420_422(type) (type == VO_COLOR_YUV_PLANAR420 || type == VO_COLOR_YVU_PLANAR420 ||\
	                                type == VO_COLOR_YUV_PLANAR422_12 || type == VO_COLOR_YVU_PLANAR422_12)

#define SWAP_SRC(src1, src2){\
{\
	VO_U8* tmp;\
	tmp = (src1);\
	(src1) = (src2);\
	(src2) = tmp;\
}\
}

/**
 * Initialize a Color Conversion Resize and Rotate instance use default settings.
 * \param phCCRR [out] Return the CCRR handle.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_OUT_OF_MEMORY Out of memory.
 */
VO_U32 VO_API voCCRRInit(VO_HANDLE * phCCRR, VO_PTR hView, VO_MEM_OPERATOR * pMemOP, VO_U32 nFlag);

/**
 * Get CCRR Name. (Like Titan, S2443, IMX31....)
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param pName [out] The.name of the CCRR
 * \param nLen [in] The name buffer length
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_INPUT_BUFFER_SMALL. The buffer is too small
 */
VO_U32 VO_API voCCRRGetName (VO_HANDLE hCCRR, signed char * pName, int nLen);

/**
 * Get support input type
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param nIndex [in] Input index of type.
 * \param pType [out] supported input type. 
 * \retval VORC_OK Succeeded.
 */
VO_U32 VO_API voCCRRGetInputType(VO_HANDLE hCCRR, VO_IV_COLORTYPE * pType, VO_U32 nIndex);

/**
 * Get support rotate type
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param pRotateType [out] supported rotate type  See VOCCRRROTATIONTYPE 
 * \retval VORC_OK Succeeded.
 */
VO_U32 VO_API voCCRRGetRotateType (VO_HANDLE hCCRR, int * pRotateType);

VO_U32 VO_API voCCRRGetProperty(VO_HANDLE hCCRR, VO_CCRRR_PROPERTY * VO_CCRRR_PROPERTY);

VO_U32 VO_API voCCRRSetWindow(VO_HANDLE hCCRR, void * hWnd);
/**
 * Get support Output type
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param nIndex [in] Output index of type.
 * \param pType [out] supported Output type. 
 * \retval VORC_OK Succeeded.
 */

VO_U32 VO_API voCCRRGetOutputType(VO_HANDLE hCCRR, VO_IV_COLORTYPE * pType, VO_U32 nIndex);
/**
 * Get support Output type
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \retval VORC_OK. the output buffer will allocated by CCRR. CCRR will set the pointer in process.
 * \others, the output buffer will allocated by video render
 */
VO_U32 VO_API voCCRRGetOutputBuffer(VO_HANDLE hCCRR);

/**
 * Get pointer of memory operating functions.
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param ppMemOp [out] memory operator pointer
 * \retval VORC_OK Succeeded.
 */
VO_U32 VO_API voCCRRGetMemoryOperator(VO_HANDLE hCCRR, VO_MEM_VIDEO_OPERATOR ** ppMemOp);

/**
 * Set input type
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param lInType [in] input video type. 
 * \param lOutType [in] output video type. 
 * \retval VORC_OK Succeeded.
 */
VO_U32 VO_API voCCRRSetVideoType(VO_HANDLE hCCRR, VO_IV_COLORTYPE lInType, VO_IV_COLORTYPE lOutType);

/**
 * Set input size
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param nInWidth [in] input video width. 
 * \param nInHeight [in] input video height. 
 * \param nMBWidth [in] input video MB width. 
 * \param nMBHeight [in] input video MB height. 
 * \param nOutWidth [in] output video width. 
 * \param nOutHeight [in] output video height. 
 * \param nRotateType [in] rotate type 
 * \retval VORC_OK Succeeded.
 */
VO_U32 VO_API voCCRRSetVideoSize(VO_HANDLE hCCRR, VO_U32 * nInWidth, VO_U32 * nInHeight, VO_U32 * nOutWidth, VO_U32 * nOutHeight, VO_IV_RTTYPE nRotateType);

/**
 * Set display window
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param hDispWnd [in] Display window
 * \retval VORC_OK, the CCRR need window to render video directly, no need output buffer.
 * \retval others, the CCRR need output buffer.
 */
VO_U32 VO_API voCCRRSetWindow(VO_HANDLE hCCRR, void * hWnd);

/**
 * Process CCRR
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param pInput [in] intput data
 * \param pOutput [in/out] output data
 * \param pMB [in] input mb info
 * \param bWait [in] wait done or not
 * \retval VORC_OK Succeeded.
 */
VO_U32 VO_API voCCRRProcess(VO_HANDLE hCCRR, VO_VIDEO_BUFFER * pInput, VO_VIDEO_BUFFER * pOutput, VO_S64 nStart, VO_BOOL bWait);

/**
 * wait process done
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \retval VORC_OK Succeeded.
 */
VO_U32 VO_API voCCRRWaitDone(VO_HANDLE hCCRR);

/**
 * Set parameter of the CCRR instance.
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param nID [in] Parameter ID
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
VO_U32 VO_API voCCRRSetParameter(VO_HANDLE hCCRRR, VO_U32 nID, VO_PTR pValue);

/**
 * Set parameter of the CCRR instance.
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param nID [in] Parameter ID
 * \param plValue[out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
VO_U32 VO_API voCCRRGetParameter(VO_HANDLE hCCRRR, VO_U32 nID, VO_PTR pValue);

/**
 * Uninit instance
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \retval VORC_OK Succeeded.
 */
VO_U32 VO_API voCCRRUninit(VO_HANDLE hCCRR);

VO_U32 VO_API voCCRRSetCallBack(VO_HANDLE hCCRRR, VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData);


#ifdef VOX86
extern THREAD_FUNCTION ClrConvProc(void *);
#endif

#endif //_CCINTERNAL_H__
