/** 
 * \file ccrrrender.h
 * \Dll file name: voCCRRR.dll The software CCRR name should be voSCCRRR.Dll
 * \brief VisualOn Color Conversion, Resize and Rotate for render API
 * \version 0.2
 * \date 12/12/2007 rewrited
 * \version 0.1
 * \date 04/23/2007 created
 * \author Bangfei Jin
 */

#ifndef _CCRRRENDER_H_
#define _CCRRRENDER_H_

#include "sdkbase.h"
//#include "vovideo.h"
#include "voCCRRR.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void * HVOCCRRRENDER;

/**
 * Color Conversion, Resize and Rotate specific return code 
 * \see VOCOMMONRETURNCODE
 */
typedef enum
{
	VORC_CCRR_UNSUPORT_INTYPE		= VORC_CCDEC_BASE | 0x0001,  /*!< Color Conversion unsuport input type */
	VORC_CCRR_UNSUPORT_OUTTYPE		= VORC_CCDEC_BASE | 0x0002,  /*!< Color Conversion unsuport output type */
	VORC_CCRR_UNSUPORT_ROTATION		= VORC_CCDEC_BASE | 0x0003,  /*!< Color Conversion unsuport rotation type*/
	VORC_CCRR_UNSUPORT_RESIZE  		= VORC_CCDEC_BASE | 0x0004,  /*!< Color Conversion unsuport resize*/
	VORC_CCRR_UNINI			  		= VORC_CCDEC_BASE | 0x0005,  /*!< Color Conversion uninitial error*/
	VORC_CCRR_PARAMETER_ERR	  		= VORC_CCDEC_BASE | 0x0006   /*!< Error input parameters*/
}VOCCRRRETURNCODE;

/*!
* Defination of rotation type of output  
*
*/
typedef enum
{
	CCRR_ROTATION_DISABLE		 = 0X0001,  /*!< No rotation */
	CCRR_ROTATION_90L			 = 0X0002,  /*!< Left rotation 90 */
	CCRR_ROTATION_90R            = 0X0004,  /*!< Right rotation 90 */
	CCRR_ROTATION_180            = 0X0008,  /*!< Rotation 180*/
	CCRR_FLIP_Y                  = 0X0010,   /*!< Flip Y */
	CCRR_FLIP_X                  = 0X0012   /*!< Flip X */
}VOCCRRROTATIONTYPE;

/*!
* Defination of resize quality 
*
*/
typedef enum
{
	CCRR_RESIZE_NEAREST_PIXEL	 = 2,  /*!< use nearest pixel */
	CCRR_RESIZE_LINER			 = 3,  /*!< use liner value */
}VOCCRRRESIZETYPE;


/**
 * Color Conversion specific parameter id 
 * \see VOCOMMONPARAMETERID
 */
typedef struct tagCCRRVideoSize 
{
	int		width;
	int		height;
} CCRRVideoSize, *PCCRRVideoSize;

typedef struct _CurrentImage
{
	VOCODECVIDEOBUFFER * pInput;
	VOCODECVIDEOBUFFER * pOutput;
}CurrentImage;

typedef enum
{
	VOID_CCRR_ENABLEOVERLAY		= VOID_CC_BASE | 0x0001,  /*!< parameter is LONG. 0: disable; 1: enable */
	VOID_CCRR_SUBMEDIATYPE		= VOID_CC_BASE | 0x0002,  /*!< parameter is LONG. GUID * */
	VOID_CCRR_VIDEO_SIZE		= VOID_CC_BASE | 0x0003,  /*!< video stream's width and height. PCCRRVideoSize * */
	VOID_CCRR_DEINTERLACE		= VOID_CC_BASE | 0x0004,  /*!< parameter is LONG. 0: disable; 1: enable */
	VOID_CCRR_RENDER_LAYER		= VOID_CC_BASE | 0x0005,  /*!< parameter is LONG. 0: PRIMARY; 1: OVERLAY */
	VOID_CCRR_RENDER_MODE		= VOID_CC_BASE | 0x0006,  /*!< parameter is LONG. 0: window mode; 1: full screen mode */
	VOID_CCRR_FILTER_STATUS		= VOID_CC_BASE | 0x0010,  /*!< parameter is LONG. 0: Stop, 1, Pause, 2 Run */
	VOID_CCRR_DDMEM_OPERATOR	= VOID_CC_BASE | 0x0020,  /*!< parameter is DDMemeory operator pointer */
	VOID_CCRR_OVERRIDE			= VOID_CC_BASE | 0x0030,  /*!< parameter is the direct draw support override or not. 1 support, */
	VOID_CCRR_DRAW_RECT			= VOID_CC_BASE | 0x0040,  /*!< parameter is RECT point */
	VOID_CCRR_SHOW_OVERLAY		= VOID_CC_BASE | 0x0050,  /*!< no parameter */
	VOID_CCRR_HIDE_OVERLAY		= VOID_CC_BASE | 0x0051,  /*!< no parameter */
	VOID_CCRR_CREATE_OVERLAY	= VOID_CC_BASE | 0x0052,  /*!< no parameter */
	VOID_CCRR_DESTROY_OVERLAY	= VOID_CC_BASE | 0x0053,  /*!< no parameter */
	VOID_CCRR_REDRAW			= VOID_CC_BASE | 0x0054,  /*!< no parameter */
	VOID_CCRR_CURRENTIMAGE		= VOID_CC_BASE | 0x0055,  /*!< parameter is CurrentImage* */
}
VOCCRRPARAMETERID;


/*!
* Notify the windows size will be changed
*
*/
#define	VOCCRRID_BEGIN_CHANGEWNDOWSIZE	0X02000000	/*!< The window size will be changed*/
#define	VOCCRRID_END_CHANGEWNDOWSIZE	0X02000001	/*!< The window size was changedl */

/**
 * Initialize a Color Conversion Resize and Rotate instance use default settings.
 * \param phCCRR [out] Return the CCRR handle.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_OUT_OF_MEMORY Out of memory.
 */
//VOCCRRRETURNCODE VOAPI voCCRRInit(HVOCCRRRENDER *phCCRR);
VOCCRRRETURNCODE VOAPI voCCRRInit(VO_HANDLE * phCCRR, VO_PTR hView, VO_MEM_OPERATOR * pMemOP, VO_U32 nFlag);
/**
 * Get CCRR Name. (Like Titan, S2443, IMX31....)
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param pName [out] The.name of the CCRR
 * \param nLen [in] The name buffer length
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_INPUT_BUFFER_SMALL. The buffer is too small
 */
VOCCRRRETURNCODE VOAPI voCCRRGetName (HVOCCRRRENDER hCCRR, signed char * pName, int nLen);

/**
 * Get support input type
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param nIndex [in] Input index of type.
 * \param pType [out] supported input type. 
 * \retval VORC_OK Succeeded.
 */
//VOCCRRRETURNCODE VOAPI voCCRRGetInputType (HVOCCRRRENDER hCCRR, int nIndex, VOVIDEOTYPE * pType);
VOCCRRRETURNCODE VOAPI voCCRRGetInputType(VO_HANDLE hCCRR, VO_IV_COLORTYPE * pType, VO_U32 nIndex);
/**
 * Get support rotate type
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param pRotateType [out] supported rotate type  See VOCCRRROTATIONTYPE 
 * \retval VORC_OK Succeeded.
 */
//VOCCRRRETURNCODE VOAPI voCCRRGetRotateType (HVOCCRRRENDER hCCRR, int * pRotateType);
VOCCRRRETURNCODE VOAPI voCCRRGetRotateType (VO_HANDLE hCCRR, int * pRotateType);

VOCCRRRETURNCODE VOAPI voCCRRGetProperty(VO_HANDLE hCCRR, VO_CCRRR_PROPERTY * VO_CCRRR_PROPERTY);

VOCCRRRETURNCODE VOAPI voCCRRSetWindow(VO_HANDLE hCCRR, void * hWnd);
/**
 * Get support Output type
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param nIndex [in] Output index of type.
 * \param pType [out] supported Output type. 
 * \retval VORC_OK Succeeded.
 */
//VOCCRRRETURNCODE VOAPI voCCRRGetOutputType(HVOCCRRRENDER hCCRR, int nIndex, VOVIDEOTYPE * pType);
VOCCRRRETURNCODE VOAPI voCCRRGetOutputType(VO_HANDLE hCCRR, VO_IV_COLORTYPE * pType, VO_U32 nIndex);
/**
 * Get support Output type
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \retval VORC_OK. the output buffer will allocated by CCRR. CCRR will set the pointer in process.
 * \others, the output buffer will allocated by video render
 */
VOCCRRRETURNCODE VOAPI voCCRRGetOutputBuffer(VO_HANDLE hCCRR);

/**
 * Get pointer of memory operating functions.
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param ppMemOp [out] memory operator pointer
 * \retval VORC_OK Succeeded.
 */
//VOCCRRRETURNCODE VOAPI voCCRRGetMemoryOperator(HVOCCRRRENDER hCCRR, VOMEMORYOPERATOR** ppMemOp);
VOCCRRRETURNCODE VOAPI voCCRRGetMemoryOperator(VO_HANDLE hCCRR, VO_MEM_VIDEO_OPERATOR ** ppMemOp);

/**
 * Set input type
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param lInType [in] input video type. 
 * \param lOutType [in] output video type. 
 * \retval VORC_OK Succeeded.
 */

//VOCCRRRETURNCODE VOAPI voCCRRSetVideoType(HVOCCRRRENDER hCCRR, VOVIDEOTYPE lInType, VOVIDEOTYPE lOutType);
VOCCRRRETURNCODE VOAPI voCCRRSetVideoType(VO_HANDLE hCCRR, VO_IV_COLORTYPE lInType, VO_IV_COLORTYPE lOutType);
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

//VOCCRRRETURNCODE VOAPI voCCRRSetVideoSize(HVOCCRRRENDER hCCRR, int nInWidth, int nInHeight, int nMBWidth, int nMBHeight, 
//										  int nOutWidth, int nOutHeight, VOCCRRROTATIONTYPE nRotateType);
VOCCRRRETURNCODE VOAPI voCCRRSetVideoSize(VO_HANDLE hCCRR, VO_U32 * nInWidth, VO_U32 * nInHeight, VO_U32 * nOutWidth, VO_U32 * nOutHeight, VO_IV_RTTYPE nRotateType);

/**
 * Set display window
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param hDispWnd [in] Display window
 * \retval VORC_OK, the CCRR need window to render video directly, no need output buffer.
 * \retval others, the CCRR need output buffer.
 */
VOCCRRRETURNCODE VOAPI voCCRRSetWindow(HVOCCRRRENDER hCCRR, void * hWnd);

/**
 * Process CCRR
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param pInput [in] intput data
 * \param pOutput [in/out] output data
 * \param pMB [in] input mb info
 * \param bWait [in] wait done or not
 * \retval VORC_OK Succeeded.
 */
//VOCCRRRETURNCODE VOAPI voCCRRProcess(HVOCCRRRENDER hCCRR, VOCODECVIDEOBUFFER * pInput, VOCODECVIDEOBUFFER * pOutput, unsigned char * pMB, int bWait);
VOCCRRRETURNCODE VOAPI voCCRRProcess(VO_HANDLE hCCRR, VO_VIDEO_BUFFER * pInput, VO_VIDEO_BUFFER * pOutput, VO_S64 nStart, VO_BOOL bWait);
/**
 * wait process done
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \retval VORC_OK Succeeded.
 */
//VOCCRRRETURNCODE VOAPI voCCRRWaitDone(HVOCCRRRENDER hCCRR);
VOCCRRRETURNCODE VOAPI voCCRRWaitDone(VO_HANDLE hCCRR);
/**
 * Set parameter of the CCRR instance.
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param nID [in] Parameter ID
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
VOCCRRRETURNCODE VOAPI voCCRRSetParameter(VO_HANDLE hCCRRR, VO_U32 nID, VO_PTR pValue);
/**
 * Set parameter of the CCRR instance.
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \param nID [in] Parameter ID
 * \param plValue[out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
VOCCRRRETURNCODE VOAPI voCCRRGetParameter(VO_HANDLE hCCRRR, VO_U32 nID, VO_PTR pValue);

/**
 * Uninit instance
 * \param hCCRR [in] CCRR instance handle, returned by voCCRRInit().
 * \retval VORC_OK Succeeded.
 */
//VOCCRRRETURNCODE VOAPI voCCRRUninit(HVOCCRRRENDER hCCRR);
VOCCRRRETURNCODE VOAPI voCCRRUninit(VO_HANDLE hCCRR);

//	VO_U32 (VO_API * SetCallBack) (VO_HANDLE hCCRRR, VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData);
VOCCRRRETURNCODE VOAPI voCCRRSetCallBack(VO_HANDLE hCCRRR, VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData);
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // _CCRRRENDER_H_