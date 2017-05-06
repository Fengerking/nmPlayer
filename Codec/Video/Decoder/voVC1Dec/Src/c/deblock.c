//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************

#include "xplatform_wmv.h"
#include "limits.h"
#include "voWmvPort.h"
#include "typedef.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
 
#include <math.h>
#include "tables_wmv.h"

#ifdef macintosh
#   include "cpudetect_mac.h"
    extern Bool_WMV g_bSupportAltiVec_WMV;
#endif
extern tYUV420Frame_WMV*  buf_seq_ctl(FIFOTYPE *priv, tYUV420Frame_WMV* frame , const VO_U32 flag);

tWMVDecodeStatus CopyCurrToPost (tWMVDecInternalMember *pWMVDec)
{ 
    I32_WMV i;
#ifdef SHAREMEMORY
	U8_WMV *ppxliCurrQY = pWMVDec->m_pvopcRenderThis->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
	U8_WMV *ppxliCurrQU = pWMVDec->m_pvopcRenderThis->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	U8_WMV *ppxliCurrQV = pWMVDec->m_pvopcRenderThis->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	U8_WMV *ppxliPostQY,	*ppxliPostQU,*ppxliPostQV;
	//ZOU FIFO		 
	buf_seq_ctl(&pWMVDec->m_memManageFIFO, pWMVDec->m_pvopcRenderThis, FIFO_WRITE);
	pWMVDec->m_pfrmPostQ =  buf_seq_ctl(&pWMVDec->m_memManageFIFO, NULL, FIFO_READ);
	if(pWMVDec->m_pfrmPostQ == NULL)
	{
		reset_buf_seq(pWMVDec);
		return WMV_BadMemory;
	}
	pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
	pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
	pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;
	ppxliPostQY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
	ppxliPostQU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	ppxliPostQV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	pWMVDec->m_pfrmPostQ->m_frameType = pWMVDec->m_pvopcRenderThis->m_frameType;
	pWMVDec->m_pfrmPostQ->m_timeStamp = pWMVDec->m_pvopcRenderThis->m_timeStamp;
	pWMVDec->m_pfrmPostQ->m_iFrmWidthSrc = pWMVDec->m_pvopcRenderThis->m_iFrmWidthSrc;
	pWMVDec->m_pfrmPostQ->m_iFrmHeightSrc = pWMVDec->m_pvopcRenderThis->m_iFrmHeightSrc;

#else
    U8_WMV *ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliPostQY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliPostQU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliPostQV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
#endif

	

	for ( i = 0; i < pWMVDec->m_iHeightUV; i++) {
        ALIGNED32_FASTMEMCPY(ppxliPostQU, ppxliCurrQU, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
        ppxliPostQU += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQU += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostQV, ppxliCurrQV, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
        ppxliPostQV += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
        ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
    }

	return WMV_Succeeded;
}

tWMVDecodeStatus CopyCurrToPostBefore (tWMVDecInternalMember *pWMVDec)
{ 
    I32_WMV i;
#ifdef SHAREMEMORY
	U8_WMV *ppxliCurrQY = pWMVDec->m_pvopcRenderThis->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
	U8_WMV *ppxliCurrQU = pWMVDec->m_pvopcRenderThis->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	U8_WMV *ppxliCurrQV = pWMVDec->m_pvopcRenderThis->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	U8_WMV *ppxliPostQY,	*ppxliPostQU,*ppxliPostQV;
	//ZOU FIFO		 
	buf_seq_ctl(&pWMVDec->m_memManageFIFO, pWMVDec->m_pvopcRenderThis, FIFO_WRITE);
	pWMVDec->m_pfrmPostQ =  buf_seq_ctl(&pWMVDec->m_memManageFIFO, NULL, FIFO_READ);
	if(pWMVDec->m_pfrmPostQ == NULL)
	{
		reset_buf_seq(pWMVDec);
		return WMV_BadMemory;
	}
	pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
	pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
	pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;
	ppxliPostQY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
	ppxliPostQU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	ppxliPostQV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	pWMVDec->m_pfrmPostQ->m_frameType = pWMVDec->m_pvopcRenderThis->m_frameType;
	pWMVDec->m_pfrmPostQ->m_timeStamp = pWMVDec->m_pvopcRenderThis->m_timeStamp;
	pWMVDec->m_pfrmPostQ->m_iFrmWidthSrc = pWMVDec->m_pvopcRenderThis->m_iFrmWidthSrc;
	pWMVDec->m_pfrmPostQ->m_iFrmHeightSrc = pWMVDec->m_pvopcRenderThis->m_iFrmHeightSrc;

#else
    U8_WMV *ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliPostQY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliPostQU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliPostQV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
#endif

	

	//for ( i = 0; i < pWMVDec->m_iHeightUV; i++) {
 //       ALIGNED32_FASTMEMCPY(ppxliPostQU, ppxliCurrQU, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
 //       ppxliPostQU += pWMVDec->m_iWidthPrevUV;
 //       ppxliCurrQU += pWMVDec->m_iWidthPrevUV;
 //       ALIGNED32_FASTMEMCPY(ppxliPostQV, ppxliCurrQV, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
 //       ppxliPostQV += pWMVDec->m_iWidthPrevUV;
 //       ppxliCurrQV += pWMVDec->m_iWidthPrevUV;
 //       ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
 //       ppxliPostQY += pWMVDec->m_iWidthPrevY;
 //       ppxliCurrQY += pWMVDec->m_iWidthPrevY;
 //       ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
 //       ppxliPostQY += pWMVDec->m_iWidthPrevY;
 //       ppxliCurrQY += pWMVDec->m_iWidthPrevY;
 //   }

	return WMV_Succeeded;
}


tWMVDecodeStatus CopyCurrToPostAfter (tWMVDecInternalMember *pWMVDec)
{ 
    I32_WMV i;
#ifdef SHAREMEMORY
	U8_WMV *ppxliCurrQY = pWMVDec->m_pvopcRenderThis->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
	U8_WMV *ppxliCurrQU = pWMVDec->m_pvopcRenderThis->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	U8_WMV *ppxliCurrQV = pWMVDec-> m_pvopcRenderThis->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	U8_WMV *ppxliPostQY,	*ppxliPostQU,*ppxliPostQV;
	//ZOU FIFO		 
	//buf_seq_ctl(&pWMVDec->m_memManageFIFO, pWMVDec->m_pvopcRenderThis, FIFO_WRITE);
	//pWMVDec->m_pfrmPostQ =  buf_seq_ctl(&pWMVDec->m_memManageFIFO, NULL, FIFO_READ);
	if(pWMVDec->m_pfrmPostQ == NULL)
	{
		reset_buf_seq(pWMVDec);
		return WMV_BadMemory;
	}
	pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
	pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
	pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;
	ppxliPostQY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
	ppxliPostQU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	ppxliPostQV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	pWMVDec->m_pfrmPostQ->m_frameType = pWMVDec->m_pvopcRenderThis->m_frameType;
	pWMVDec->m_pfrmPostQ->m_timeStamp = pWMVDec->m_pvopcRenderThis->m_timeStamp;
	pWMVDec->m_pfrmPostQ->m_iFrmWidthSrc = pWMVDec->m_pvopcRenderThis->m_iFrmWidthSrc;
	pWMVDec->m_pfrmPostQ->m_iFrmHeightSrc = pWMVDec->m_pvopcRenderThis->m_iFrmHeightSrc;

#else
    U8_WMV *ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliPostQY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliPostQU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliPostQV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
#endif

	

	for ( i = 0; i < pWMVDec->m_iHeightUV; i++) {
        ALIGNED32_FASTMEMCPY(ppxliPostQU, ppxliCurrQU, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
        ppxliPostQU += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQU += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostQV, ppxliCurrQV, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
        ppxliPostQV += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
        ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
    }

	return WMV_Succeeded;
}

tWMVDecodeStatus CopyCurrToPost_new (tWMVDecInternalMember *pWMVDec)
{ 
    I32_WMV i;
#ifdef SHAREMEMORY
	U8_WMV *ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
	U8_WMV *ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	U8_WMV *ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	U8_WMV *ppxliPostQY,	*ppxliPostQU,*ppxliPostQV;
	//ZOU FIFO		 
	//buf_seq_ctl(&pWMVDec->m_memManageFIFO, pWMVDec->m_pfrmCurrQ, FIFO_WRITE);

	if(pWMVDec->m_pfrmCurrQ->m_frameType == BVOP)
		pWMVDec->m_pfrmPostQ = pWMVDec->m_pfrmCurrQ;
	else
		pWMVDec->m_pfrmPostQ =  buf_seq_ctl(&pWMVDec->m_memManageFIFO, NULL, FIFO_READ);

	if(pWMVDec->m_pfrmPostQ == NULL)
	{
		reset_buf_seq(pWMVDec);
		return WMV_BadMemory;
	}
	pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
	pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
	pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;
	ppxliPostQY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
	ppxliPostQU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	ppxliPostQV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	pWMVDec->m_pfrmPostQ->m_frameType = pWMVDec->m_pfrmCurrQ->m_frameType;
	pWMVDec->m_pfrmPostQ->m_timeStamp = pWMVDec->m_pfrmCurrQ->m_timeStamp;
	pWMVDec->m_pfrmPostQ->m_iFrmWidthSrc = pWMVDec->m_pfrmCurrQ->m_iFrmWidthSrc;
	pWMVDec->m_pfrmPostQ->m_iFrmHeightSrc = pWMVDec->m_pfrmCurrQ->m_iFrmHeightSrc;

#else
    U8_WMV *ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliPostQY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliPostQU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliPostQV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
#endif

	

	for ( i = 0; i < pWMVDec->m_iHeightUV; i++) {
        ALIGNED32_FASTMEMCPY(ppxliPostQU, ppxliCurrQU, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
        ppxliPostQU += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQU += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostQV, ppxliCurrQV, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
        ppxliPostQV += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
        ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
    }

	return WMV_Succeeded;
}


tWMVDecodeStatus CopyCurrToPostBefore_new (tWMVDecInternalMember *pWMVDec)
{ 
    I32_WMV i;
	U8_WMV *ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
	U8_WMV *ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	U8_WMV *ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	U8_WMV *ppxliPostQY,	*ppxliPostQU,*ppxliPostQV;
	//ZOU FIFO		 
	//buf_seq_ctl(&pWMVDec->m_memManageFIFO, pWMVDec->m_pfrmCurrQ, FIFO_WRITE);

	if(pWMVDec->m_pfrmCurrQ->m_frameType == BVOP)
		pWMVDec->m_pfrmPostQ = pWMVDec->m_pfrmCurrQ;
	else
		pWMVDec->m_pfrmPostQ =  buf_seq_ctl(&pWMVDec->m_memManageFIFO, NULL, FIFO_READ);

	if(pWMVDec->m_pfrmPostQ == NULL)
	{
		reset_buf_seq(pWMVDec);
		return WMV_BadMemory;
	}
	pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
	pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
	pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;
	ppxliPostQY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
	ppxliPostQU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	ppxliPostQV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	pWMVDec->m_pfrmPostQ->m_frameType = pWMVDec->m_pfrmCurrQ->m_frameType;
	pWMVDec->m_pfrmPostQ->m_timeStamp = pWMVDec->m_pfrmCurrQ->m_timeStamp;
	pWMVDec->m_pfrmPostQ->m_iFrmWidthSrc = pWMVDec->m_pfrmCurrQ->m_iFrmWidthSrc;
	pWMVDec->m_pfrmPostQ->m_iFrmHeightSrc = pWMVDec->m_pfrmCurrQ->m_iFrmHeightSrc;

	/*
	for ( i = 0; i < pWMVDec->m_iHeightUV; i++) {
        ALIGNED32_FASTMEMCPY(ppxliPostQU, ppxliCurrQU, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
        ppxliPostQU += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQU += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostQV, ppxliCurrQV, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
        ppxliPostQV += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
        ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
    }
	*/

	return WMV_Succeeded;
}


tWMVDecodeStatus CopyCurrToPostAfter_new (tWMVDecInternalMember *pWMVDec)
{ 
    I32_WMV i;
	U8_WMV *ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
	U8_WMV *ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	U8_WMV *ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	U8_WMV *ppxliPostQY,	*ppxliPostQU,*ppxliPostQV;

	pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
	pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
	pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;
	ppxliPostQY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
	ppxliPostQU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	ppxliPostQV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;

	
	for ( i = 0; i < pWMVDec->m_iHeightUV; i++) {
        ALIGNED32_FASTMEMCPY(ppxliPostQU, ppxliCurrQU, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
        ppxliPostQU += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQU += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostQV, ppxliCurrQV, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
        ppxliPostQV += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
        ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
    }

	return WMV_Succeeded;
}








