#include "ccConstant.h"
#include "voccrender.h"
#include "stdlib.h" 
#ifdef NEW_SDK
#include "ccrrrender.h"
#endif//NEW_SDK
extern int *y_tab;
extern int *x_tab;

#ifdef NEW_SDK
typedef struct
{
	int in_width;
	int out_width;
	int in_height;
	int out_height;
	int ini_flg;
	void* CC_HND;
	INTYPE in_type;
	INTYPE out_type;
	RTTYPE rt;
	VO_PTR phCheck;
}
CC_RRR;

void* voCCRRR_hModule = NULL;

#endif //NEW_SDK
CC_RC cc_ini(CC_HND* hnd, int in_width, int out_width, int in_height, int out_height, RTTYPE rt, INTYPE in_type, INTYPE out_type)
{
	CCHND* cc_hnd;
	int offset = 0;

	cc_hnd = (CCHND*)malloc(sizeof(CCHND));
	*hnd = (void*) cc_hnd;
	if(!cc_hnd)
		return CC_ERR;
	memset(cc_hnd, 0, sizeof(CCHND));
	cc_hnd->in_width = in_width;
	cc_hnd->in_height = in_height;
	cc_hnd->out_width = out_width;
	cc_hnd->out_height = out_height;
	cc_hnd->in_type = in_type;

	if((in_type == out_type) && (in_type==YUV420PLANAR))
	{
		cc_hnd->cc = cc_all[3][0][rt];
		return CC_OK;	
	}

	if((in_width!=out_width) || (in_height != out_height)){
		x_tab = cc_hnd->x_tab = (int*)malloc(((in_width/16) + out_width * 3 + 2)*sizeof(int));
		y_tab = cc_hnd->y_tab= (int*)malloc(((in_height/16) + out_height * 3 + 2)*sizeof(int));
		if(!cc_hnd->x_tab|| !cc_hnd->y_tab)
			goto error;

		init_scale((float)in_width/out_width, out_width, x_tab, in_width);
		init_scale((float)in_height/out_height, out_height, y_tab, in_height);
/**/
		if((((in_width>>1) == out_width) && ((in_height>>1) == out_height))
		  ||(((in_width>>2) == out_width) && ((in_height>>2) == out_height))
		  ||(((in_width>>3) == out_width) && ((in_height>>3) == out_height))
		  ||(((in_width>>4) == out_width) && ((in_height>>4) == out_height))
		  ||(((in_width>>5) == out_width) && ((in_height>>5) == out_height))
		  ||(((in_width>>6) == out_width) && ((in_height>>6) == out_height))
		  ||(((in_width>>7) == out_width) && ((in_height>>7) == out_height))
		  ||(((in_width>>8) == out_width) && ((in_height>>8) == out_height))
		  ||(((in_width>>9) == out_width) && ((in_height>>9) == out_height))
		  ||(((in_width>>10) == out_width) && ((in_height>>10) == out_height))		  
		  )
			offset = 1*3;
		else if(((in_width*3) == (out_width*4)) && ((in_height*3) == (out_height*4)))
			offset = 2*3;
		else if(((in_width*3) == (out_width*2)) && ((in_height*3) == (out_height*2)))
			offset = 3*3;
		else if(((in_width<<1) == out_width) && ((in_height<<1) == out_height))
			offset = 4*3;
		else
			offset = 0;
		offset +=rt;

		if(in_type==YUV420PLANAR)
		{
			if(rt == 3)
				cc_hnd->cc = cc_all[in_type][0][15];
			else
				cc_hnd->cc = cc_all[in_type][0][offset];
		}
		else
			cc_hnd->cc = cc_all[in_type][0][rt];
	}else{
		cc_hnd->cc = cc_all[in_type][1][rt];
	}
	return CC_OK;
error:
	if(cc_hnd->x_tab){
		free(cc_hnd->x_tab);
		cc_hnd->x_tab = 0;
	}
	if(cc_hnd->y_tab){
		free(cc_hnd->y_tab);
		cc_hnd->y_tab = 0;
	}
	return CC_ERR;
}

CC_RC cc_process(CC_HND hnd, unsigned char *y, unsigned char *u, unsigned char *v, int in_stride[3], 
				 unsigned char *dst, int out_stride, unsigned char *mb_skip)
{
	CCHND* cc_hnd = (CCHND*)hnd;

	cc_hnd->cc(y, u, v, cc_hnd->in_width, cc_hnd->in_height, in_stride,
		cc_hnd->out_width, cc_hnd->out_height, dst, out_stride, mb_skip);
	return CC_OK;
}

CC_RC cc_uini(CC_HND hnd )
{
	CCHND* cc_hnd = (CCHND*)hnd;
	if(cc_hnd){
		if(cc_hnd->x_tab){
			free(cc_hnd->x_tab);
			cc_hnd->x_tab = 0;
		}
		if(cc_hnd->y_tab){
			free(cc_hnd->y_tab);
			cc_hnd->y_tab = 0;
		}
		free(cc_hnd);
		cc_hnd = NULL;
	}

	return CC_OK;
}

#ifdef NEW_SDK
//	VO_U32 (VO_API * Init) (VO_HANDLE * phCCRRR, VO_PTR hView, VO_MEM_OPERATOR * pMemOP, VO_U32 nFlag);
VOCCRRRETURNCODE VOAPI voCCRRInit(VO_HANDLE * phCCRR, VO_PTR hView, VO_MEM_OPERATOR * pMemOP, VO_U32 nFlag)
{
	CC_RRR * phCR = NULL;
	*phCCRR = NULL;
//#ifndef DEBUG
//	if (CheckLicenseFile (NULL) < 0)
//		return (VOCCRRRETURNCODE) VORC_COM_LICENSE_LIMIT;
//#endif

	phCR = (CC_RRR*)malloc(sizeof(CC_RRR));
	if (phCR == NULL)
		return (VOCCRRRETURNCODE) VORC_COM_OUT_OF_MEMORY;
	memset(phCR, 0, sizeof(CC_RRR));
	*phCCRR = (HVOCCRRRENDER) phCR;
	//voCCRRR_hModule = NULL;

#ifndef No_CheckLib
	voCheckLibInit(&(phCR->phCheck), VO_INDEX_SNK_CCRRR, nFlag, voCCRRR_hModule);
#endif
	return (VOCCRRRETURNCODE) VORC_OK;
}

VOCCRRRETURNCODE VOAPI voCCRRGetName (HVOCCRRRENDER hCCRR, signed char * pName, int nLen)
{
	strcpy(pName, "VOCRRR_SOFT");
	return (VOCCRRRETURNCODE) VORC_OK;
}
//	VO_U32 (VO_API * GetInputType) (VO_HANDLE hCCRRR, VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
VOCCRRRETURNCODE VOAPI voCCRRGetInputType(VO_HANDLE hCCRR, VO_IV_COLORTYPE * pType, VO_U32 nIndex)
{
	if (nIndex == 0)
		*pType = VO_COLOR_RGB565_PACKED;
	else if (nIndex == 1)
		*pType = VO_COLOR_YUV_PLANAR420;
	else if (nIndex == 2)
		*pType = VO_COLOR_RGB888_PACKED;
	else
		return VORC_CCRR_UNSUPORT_INTYPE;

	return (VOCCRRRETURNCODE) VORC_OK;
}
VOCCRRRETURNCODE VOAPI voCCRRGetRotateType (VO_HANDLE hCCRR, int * pRotateType)
{
	*pRotateType = 	VO_RT_DISABLE |	VO_RT_90L | VO_RT_90R | VO_RT_180;
	return (VOCCRRRETURNCODE) VORC_OK;
}

//	VO_U32 (VO_API * GetProperty) (VO_HANDLE hCCRRR, VO_CCRRR_PROPERTY * pProperty);
VOCCRRRETURNCODE VOAPI voCCRRGetProperty (VO_HANDLE hCCRR, VO_CCRRR_PROPERTY * pProperty)
{
	CC_RRR * phCR = (CC_RRR *) hCCRR;
	pProperty->nRender = 0;
	pProperty->nOverlay = 0;
	pProperty->nKeyColor = 0;
	pProperty->nRotate = VO_RT_DISABLE | VO_RT_90L | VO_RT_180;
	pProperty->nOutBuffer = 0;
	pProperty->nFlag = 0;

	strcpy(pProperty->szName, "VOCRRR_SOFT");
	return (VOCCRRRETURNCODE) VORC_OK;
}
//	VO_U32 (VO_API * SetColorType) (VO_HANDLE hCCRRR, VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor);
VOCCRRRETURNCODE VOAPI voCCRRSetVideoType(VO_HANDLE hCCRR, VO_IV_COLORTYPE lInType, VO_IV_COLORTYPE lOutType)
{
	CC_RRR * phCR = (CC_RRR *) hCCRR;
	if (phCR == NULL)
		return (VOCCRRRETURNCODE) VORC_COM_INVALID_ARG;

	if(lInType == VO_COLOR_YUV_PLANAR420){
		phCR->in_type = YUV420PLANAR;
	}else if(lInType == VO_COLOR_RGB565_PACKED){
		phCR->in_type = RGB565;
	}else if(lInType == VO_COLOR_RGB888_PACKED){
		phCR->in_type = RGB24;
	}else{
		return VORC_CCRR_UNSUPORT_INTYPE;
	}
	if(lOutType == VO_COLOR_YUV_PLANAR420){
		phCR->out_type = YUV420PLANAR;
	}else if(lOutType == VO_COLOR_RGB565_PACKED){
		phCR->out_type = RGB565;
	}else if(lOutType == VO_COLOR_RGB888_PACKED){
		phCR->out_type = RGB24;
	}else{
		return VORC_CCRR_UNSUPORT_INTYPE;
	}
	phCR->ini_flg |= (1<<5);
	if(phCR->ini_flg ==0x3f){
		if(phCR->CC_HND)
			cc_uini(phCR->CC_HND);
		cc_ini(&(phCR->CC_HND), phCR->in_width, phCR->out_width, phCR->in_height, phCR->out_height, phCR->rt, phCR->in_type, phCR->out_type);
	}
	return (VOCCRRRETURNCODE) VORC_OK;
}
//	VO_U32 (VO_API * SetCCRRSize)        (VO_HANDLE hCCRRR, VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate);
VOCCRRRETURNCODE VOAPI voCCRRSetVideoSize(VO_HANDLE hCCRR, VO_U32 * nInWidth, VO_U32 * nInHeight, VO_U32 * nOutWidth, VO_U32 * nOutHeight, VO_IV_RTTYPE nRotateType)
{
	CC_RRR * phCR = (CC_RRR *) hCCRR;
	if (phCR == NULL)
		return (VOCCRRRETURNCODE) VORC_COM_INVALID_ARG;

	phCR->in_width = *nInWidth;
	phCR->in_height = *nInHeight;
	phCR->ini_flg |= 1;
	phCR->ini_flg |= (1<<1);

	phCR->out_width = *nOutWidth ;
	phCR->out_height = *nOutHeight;
	phCR->ini_flg |= (1<<2);
	phCR->ini_flg |= (1<<3);

	switch (nRotateType)
	{
	case VO_RT_DISABLE:
		phCR->rt = 0;  /*!< No rotation */
		break;
	case VO_RT_90L:
		phCR->rt = 1;  /*!< Left rotation 90 */
		break;
	case VO_RT_90R:
		phCR->rt = 2;  /*!< Right rotation 90 */
		break;
	case VO_RT_180:
		phCR->rt = 3;  /*!< Right rotation 90 */
		break;
	default:
		return VORC_CCRR_UNSUPORT_ROTATION;
		break;
	}
	phCR->ini_flg |= (1<<4);

	if(phCR->ini_flg ==0x3f){
		if(phCR->CC_HND)
			cc_uini(phCR->CC_HND);
		cc_ini(&(phCR->CC_HND), phCR->in_width, phCR->out_width, phCR->in_height, phCR->out_height, phCR->rt, phCR->in_type, phCR->out_type);
	}
	return (VOCCRRRETURNCODE) VORC_OK;

}

VOCCRRRETURNCODE VOAPI voCCRRSetWindow(VO_HANDLE hCCRR, void * hWnd)
{
	return (VOCCRRRETURNCODE) VORC_FAIL;
}
//	VO_U32 (VO_API * GetOutputType) (VO_HANDLE hCCRRR, VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
VOCCRRRETURNCODE VOAPI voCCRRGetOutputType(VO_HANDLE hCCRR, VO_IV_COLORTYPE * pType, VO_U32 nIndex)
{
	if (nIndex == 0)
		*pType = VORGB565_PACKED;
	else
		return VORC_CCRR_UNSUPORT_OUTTYPE;

	return (VOCCRRRETURNCODE) VORC_OK;
}

VOCCRRRETURNCODE VOAPI voCCRRGetOutputBuffer(VO_HANDLE hCCRR)
{
	return (VOCCRRRETURNCODE) VORC_FAIL;
}
//	VO_U32 (VO_API * GetVideoMemOP) (VO_HANDLE hCCRRR, VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP);
VOCCRRRETURNCODE VOAPI voCCRRGetMemoryOperator(VO_HANDLE hCCRR, VO_MEM_VIDEO_OPERATOR ** ppMemOp)
{
	*ppMemOp = 0;
	return (VOCCRRRETURNCODE) VORC_FAIL;
}

//	VO_U32 (VO_API * Process)       (VO_HANDLE hCCRRR, VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait);
VOCCRRRETURNCODE VOAPI voCCRRProcess(VO_HANDLE hCCRR, VO_VIDEO_BUFFER * pInput, VO_VIDEO_BUFFER * pOutput, VO_S64 nStart, VO_BOOL bWait)
{
	CC_RRR * phCR = (CC_RRR *) hCCRR;
	CCHND* cc_hnd;
	int i = 0;
	if (phCR == NULL)
		return (VOCCRRRETURNCODE) VORC_COM_INVALID_ARG;
	cc_hnd = (CCHND*)(phCR->CC_HND);
	
	if((pInput->Buffer[0] == NULL) || (pInput->Buffer[1] == NULL)
		 || (pInput->Buffer[2] == NULL) || (pOutput->Buffer[0] == NULL)
		 || (pInput->Buffer[2] == NULL) || (pOutput->Buffer[0] == NULL)
		 || (cc_hnd->in_width <= 0) || (cc_hnd->in_height <= 0)
		 || (cc_hnd->out_width <= 0) || (cc_hnd->out_height <= 0)
		 || (cc_hnd->out_width <= 0) || (cc_hnd->out_height <= 0)
		 || (pInput->Stride[0] <= 0) || (pInput->Stride[1] <= 0)
		 || (pInput->Stride[2] <= 0) || (pOutput->Stride[0] <= 0))
	{
		return VORC_CCRR_PARAMETER_ERR;
	}
	{	
	cc_hnd->cc(pInput->Buffer[0], pInput->Buffer[1], pInput->Buffer[2], cc_hnd->in_width, cc_hnd->in_height, pInput->Stride,
		cc_hnd->out_width, cc_hnd->out_height, pOutput->Buffer[0], pOutput->Stride[0], NULL);	
	}
#ifndef No_CheckLib
	voCheckLibRenderVideo(phCR->phCheck, pOutput);
#endif

	return (VOCCRRRETURNCODE) VORC_OK;
	
}
//	VO_U32 (VO_API * WaitDone) (VO_HANDLE hCCRRR);
VOCCRRRETURNCODE VOAPI voCCRRWaitDone(VO_HANDLE hCCRR)
{
	return (VOCCRRRETURNCODE) VORC_OK;
}
//	VO_U32 (VO_API * SetCallBack)       (VO_HANDLE hCCRRR, VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData);
VOCCRRRETURNCODE VOAPI voCCRRSetCallBack(VO_HANDLE hCCRRR, VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData)
{
	return (VOCCRRRETURNCODE) VORC_OK;
}
//	VO_U32 (VO_API * SetParam) (VO_HANDLE hCCRRR, VO_U32 nID, VO_PTR pValue);
VOCCRRRETURNCODE VOAPI voCCRRSetParameter(VO_HANDLE hCCRRR, VO_U32 nID, VO_PTR pValue)
{
	return (VOCCRRRETURNCODE) VORC_COM_WRONG_PARAM_ID;
}
//	VO_U32 (VO_API * GetParam) (VO_HANDLE hCCRRR, VO_U32 nID, VO_PTR pValue);
VOCCRRRETURNCODE VOAPI voCCRRGetParameter(VO_HANDLE hCCRRR, VO_U32 nID, VO_PTR pValue)
{
	return (VOCCRRRETURNCODE) VORC_COM_WRONG_PARAM_ID;
}
//	VO_U32 (VO_API * Uninit) (VO_HANDLE hCCRRR);
VOCCRRRETURNCODE VOAPI voCCRRUninit(VO_HANDLE hCCRR)
{
	CC_RRR * phCR = (CC_RRR *) hCCRR;

#ifndef No_CheckLib
	voCheckLibUninit(phCR->phCheck);
#endif

	if (phCR != NULL)
	{
		if(phCR->CC_HND)
			cc_uini(phCR->CC_HND);
		free(phCR);
		phCR = NULL;
	}
	return (VOCCRRRETURNCODE) VORC_OK;
}

#endif
