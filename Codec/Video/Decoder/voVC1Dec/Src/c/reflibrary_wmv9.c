//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#include "xplatform_wmv.h"
#include "limits.h"
#include "typedef.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
//#include "reflibrary.h"

Void_WMV CQ_DestroyQueue(tWMVDecInternalMember *pWMVDec, CQueue * pCQ);

///// CQueue class
tWMVDecodeStatus CQ_InitCQueue(tWMVDecInternalMember * pWMVDec, CQueue * pCQ, I32_WMV iNumElements)
{
   
    I32_WMV i;
    CQueueElement** ppCurr;
    CQueueElement* pQueueElement = NULL;

    if (pCQ == NULL_WMV)
        return WMV_InValidArguments;

    memset( pCQ, 0, sizeof(CQueue) );  // protect structs with malloced pointers from low memory bugs

    pCQ->m_iMaxQueueElements = iNumElements;
    pCQ->m_iCurrQueueElements = 0;

    // Queue up element objects at available queue
    
    ppCurr = &pCQ->m_pAvailHead;
    
    for (i = 0; i < iNumElements; i++)
    {
        pQueueElement = (CQueueElement*)wmvMalloc(pWMVDec, sizeof( CQueueElement), DHEAP_STRUCT);
        if (pQueueElement == NULL_WMV)
        {
            CQ_DestroyQueue(pWMVDec, pCQ);
            return WMV_BadMemory;
        }

        memset(pQueueElement, 0, sizeof( CQueueElement) );
        pQueueElement->m_pElement = NULL_WMV;
        *ppCurr = pQueueElement;
        ppCurr = &pQueueElement->m_pNext;
    }
    pCQ->m_pAvailTail = pQueueElement;
    pCQ->m_pAvailTail->m_pNext = NULL_WMV;

    pCQ->m_pUsedHead = pCQ->m_pUsedTail = NULL_WMV;

    return WMV_Succeeded;
}

Void_WMV CQ_DestroyQueue(tWMVDecInternalMember *pWMVDec, CQueue * pCQ)
{
    CQueueElement* pQueueElement;
    
    if (pCQ == NULL_WMV)
        return;

    pQueueElement = pCQ->m_pAvailHead;
    while (pQueueElement != NULL_WMV)
    {
        CQueueElement* pQueueElement2 = pQueueElement->m_pNext;
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO pQueueElement 1:%x",(U32_WMV)pQueueElement );
#endif
        wmvFree (pWMVDec, pQueueElement);
        pQueueElement = pQueueElement2;
    }	
    pCQ->m_pAvailHead = NULL_WMV;

    pQueueElement = pCQ->m_pUsedHead;
    while (pQueueElement != NULL_WMV)
    {
        CQueueElement* pQueueElement2 = pQueueElement->m_pNext;
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO pQueueElement 2:%x",(U32_WMV)pQueueElement );
#endif
        wmvFree (pWMVDec, pQueueElement);
        pQueueElement = pQueueElement2;
    }
    pCQ->m_pUsedHead = NULL_WMV;
}

Bool_WMV CQ_AddElement(CQueue * pCQ, Void_WMV *pElement, I32_WMV iElementNum)
{

    CQueueElement* pQueueElement;

    if (pCQ == NULL_WMV)
        return FALSE_WMV;

    if (pElement == NULL || iElementNum > pCQ->m_iCurrQueueElements)
        return FALSE_WMV;
    
    // Remove element object from available queue
    pQueueElement = pCQ->m_pAvailHead;

    pCQ->m_pAvailHead = pCQ->m_pAvailHead->m_pNext;
    if (pCQ->m_pAvailHead == NULL_WMV)
        pCQ->m_pAvailTail = NULL_WMV;

    pQueueElement->m_pElement = pElement;

    if (iElementNum == 0)
    {
        pQueueElement->m_pNext = pCQ->m_pUsedHead;
        pCQ->m_pUsedHead = pQueueElement;
        if (pCQ->m_pUsedTail == NULL_WMV)
            pCQ->m_pUsedTail = pQueueElement;    
    }
    else if (iElementNum == -1)
    {
        pQueueElement->m_pNext = NULL_WMV;
        pCQ->m_pUsedTail = pQueueElement;
        if (pCQ->m_pUsedHead == NULL_WMV)
            pCQ->m_pUsedHead = pQueueElement;
    }
    else
    {
        CQueueElement* pCurr = pCQ->m_pUsedHead;
        I32_WMV i;
        for (i = 0; i < iElementNum - 1; i++)
            pCurr = pCurr->m_pNext;
        pQueueElement->m_pNext = pCurr->m_pNext;
        pCurr->m_pNext = pQueueElement;
        if (pQueueElement->m_pNext == NULL_WMV)
            pCQ->m_pUsedTail = pQueueElement;
    }

    pCQ->m_iCurrQueueElements ++;

    return TRUE_WMV;
}

Bool_WMV CQ_RemoveElement(CQueue * pCQ, Void_WMV **ppElement, I32_WMV iElementNum)
{
    CQueueElement* pQueueElement;

    if (ppElement == NULL_WMV ) 
        return FALSE_WMV;

    *ppElement = NULL_WMV;

    //When pCQ==NULL, set ppElement to NULL as well.
    if (pCQ == NULL_WMV ) 
        return FALSE_WMV;

     if( iElementNum >= pCQ->m_iCurrQueueElements)
          return FALSE_WMV;

    if (iElementNum == 0)
    {
        pQueueElement= pCQ->m_pUsedHead;
        pCQ->m_pUsedHead = pQueueElement->m_pNext;
        if (pCQ->m_pUsedHead == NULL_WMV)
            pCQ->m_pUsedTail = NULL_WMV;    
    }
    else if (iElementNum == -1 && pCQ->m_iCurrQueueElements == 1)
    {
        pQueueElement= pCQ->m_pUsedTail;
        pCQ->m_pUsedHead = pCQ->m_pUsedHead = NULL_WMV;
    }
    else
    {
        I32_WMV i, iElements = iElementNum;
        CQueueElement* pCurr;

        if (iElementNum == -1)
            iElements = pCQ->m_iCurrQueueElements - 1;
        pCurr = pCQ->m_pUsedHead;
        for (i = 0; i < iElements - 1; i++)
            pCurr = pCurr->m_pNext;
        pQueueElement = pCurr->m_pNext;
        pCurr->m_pNext = pQueueElement->m_pNext;
        if (iElementNum == -1)
            pCQ->m_pUsedTail = pCurr;
    }

    *ppElement = pQueueElement->m_pElement;

    pQueueElement->m_pNext = pCQ->m_pAvailHead;
    pCQ->m_pAvailHead = pQueueElement;
    if (pCQ->m_pAvailTail == NULL_WMV)
        pCQ->m_pAvailTail = pCQ->m_pAvailHead;

    pCQ->m_iCurrQueueElements --;

    return TRUE_WMV;
}

tWMVDecodeStatus RL_SetupReferenceLibrary (tWMVDecInternalMember *pWMVDec, I32_WMV iSizeYplane,  
		I32_WMV iSizeUVplane, I32_WMV iNumRefFrames, I32_WMV iPad, I32_WMV iPlaneWidth, I32_WMV iPlaneHeight)
{	
	CReferenceLibrary * pRL = pWMVDec->m_cReferenceLibrary;
    tYUV420Frame_WMV* pFrame;
    I32_WMV i;
    tWMVDecodeStatus status;

    pRL->m_iCurrRefFrames = 0;
    pRL->m_iFramesChecked = 0;

#ifdef STABILITY
    if(pRL->m_cFrameQueue)
    {
        CQ_DestroyQueue(pWMVDec, pRL->m_cFrameQueue);
        FREE_PTR (pWMVDec, pRL->m_cFrameQueue);
    }
#endif
    pRL->m_cFrameQueue = (CQueue *)wmvMalloc(pWMVDec, sizeof( CQueue), DHEAP_STRUCT);
    if(pRL->m_cFrameQueue == NULL_WMV)
    {
        status = WMV_BadMemory;
        goto ERROR_EXIT;
    }
    
    status = CQ_InitCQueue(pWMVDec, pRL->m_cFrameQueue, iNumRefFrames);
    if (WMV_Succeeded != status)
        goto ERROR_EXIT;

    /* Allocate memory for frames and add to frame queue. */
	if( pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP ) 
	{
		VO_MEM_VIDEO_INFO videoMemInfo;
		I32_WMV iPad_Y = 0,iPad_UV = 0;

		videoMemInfo.Stride = iPlaneWidth;
		videoMemInfo.Height = iPlaneHeight;
		if(iPad) videoMemInfo.Height += 16;			//WMVA case.
		videoMemInfo.FrameCount = iNumRefFrames;
		videoMemInfo.ColorType = VO_COLOR_YUV_PLANAR420;

		pWMVDec->m_videoMemShare.Init(VO_INDEX_DEC_WMV, &videoMemInfo);

		pWMVDec->m_frameBufStrideY  = videoMemInfo.VBuffer[0].Stride[0];
		pWMVDec->m_frameBufStrideUV = videoMemInfo.VBuffer[0].Stride[1];
		pWMVDec->m_frameBufNum      = iNumRefFrames;

		if(iPad) {
			iPad_Y  = pWMVDec->m_frameBufStrideY << 2;
			iPad_UV = pWMVDec->m_frameBufStrideUV << 2;
		}

#if 0
		for(i = 0; i < FIFO_NUM; i++) {
			pWMVDec->m_memManageFIFO.img_seq[i]        = NULL;
			pWMVDec->m_memManageFIFO.img_seq_backup[i] = NULL;
		}
#endif

		for(i = 0; i < iNumRefFrames; i++){
			pFrame = (tYUV420Frame_WMV *)wmvMalloc (pWMVDec, sizeof (tYUV420Frame_WMV), DHEAP_STRUCT);
			if(pFrame==NULL_WMV){
				status = WMV_BadMemory;
				goto ERROR_EXIT;
			}
			pFrame->m_pucYPlane_Unaligned = (U8_WMV *)(videoMemInfo.VBuffer[i].Buffer[0]);
			pFrame->m_pucUPlane_Unaligned = (U8_WMV *)(videoMemInfo.VBuffer[i].Buffer[1]);
			pFrame->m_pucVPlane_Unaligned = (U8_WMV *)(videoMemInfo.VBuffer[i].Buffer[2]);
			pFrame->m_pucYPlane = (U8_WMV *)((U32_WMV)pFrame->m_pucYPlane_Unaligned + iPad_Y );
			pFrame->m_pucUPlane = (U8_WMV *)((U32_WMV)pFrame->m_pucUPlane_Unaligned + iPad_UV );
			pFrame->m_pucVPlane = (U8_WMV *)((U32_WMV)pFrame->m_pucVPlane_Unaligned + iPad_UV );
			pFrame->m_pCurrPlaneBase = pFrame->m_pucYPlane_Unaligned;
			pFrame->m_timeStamp = 0;
			pFrame->m_frameType = 0; 

			//pWMVDec->m_memManageFIFO.img_seq[i]        = pFrame;
			//pWMVDec->m_memManageFIFO.img_seq_backup[i] = pFrame;
			CQ_AddElement(pRL->m_cFrameQueue, (void *)pFrame, 0);
		}

		reset_buf_seq(pWMVDec);
	}
	else
	{
		U32_WMV iYSize = (iSizeYplane  + (iPad<<1) + 32) * sizeof (U8_WMV);
		U32_WMV iUSize = (iSizeUVplane + iPad      + 32) * sizeof (U8_WMV);
		U32_WMV iVSize = iUSize;
		U8_WMV *ppxlTmp;
		I32_WMV uv_offset;
		pWMVDec->m_framebase = (U8_WMV *)wmvMalloc (pWMVDec,(sizeof (tYUV420Frame_WMV)+iYSize+iUSize+iVSize+64)*iNumRefFrames, DHEAP_STRUCT);
		if(pWMVDec->m_framebase == NULL)
			return WMV_BadMemory;
		pWMVDec->framebaseused = 0;
		for (i = 0; i < iNumRefFrames; i++)
		{
			pFrame = (tYUV420Frame_WMV *)(pWMVDec->m_framebase + pWMVDec->framebaseused);
			pWMVDec->framebaseused += sizeof (tYUV420Frame_WMV);
			pWMVDec->framebaseused = (pWMVDec->framebaseused+31)&(~31);

#if 1	
			memset(pFrame, 0, sizeof (tYUV420Frame_WMV) );
			pFrame->m_pucYPlane_Unaligned = ppxlTmp = pWMVDec->m_framebase + pWMVDec->framebaseused;
			pFrame->m_pCurrPlaneBase = ppxlTmp;
			pWMVDec->framebaseused += iYSize+iUSize+iVSize;
			pWMVDec->framebaseused = (pWMVDec->framebaseused+31)&(~31);
			
			//Y
			pFrame->m_pucYPlane  = (U8_WMV *)(((U32_WMV)ppxlTmp + iPad + 31) & ~31);

			//U
			ppxlTmp  = pFrame->m_pucYPlane_Unaligned + iYSize;
			ppxlTmp += iPad>>1;
			uv_offset = ((U32_WMV)ppxlTmp) & 31 ;
			if(uv_offset != 0)
				uv_offset = 32 - uv_offset;
			pFrame->m_pucUPlane  = (U8_WMV *)((U32_WMV)ppxlTmp + uv_offset);

			//V
			ppxlTmp  = pFrame->m_pucYPlane_Unaligned + iYSize + iUSize;
			ppxlTmp += iPad>>1;
			uv_offset = ((U32_WMV)ppxlTmp) & 31 ;
			if(uv_offset != 0)
				uv_offset = 32 - uv_offset;

			pFrame->m_pucVPlane  = (U8_WMV *)((U32_WMV)ppxlTmp + uv_offset);
#endif
			pFrame->m_timeStamp = 0;
			pFrame->m_frameType = 0;
			CQ_AddElement(pRL->m_cFrameQueue, (void *)pFrame, 0);
			if(status!= WMV_Succeeded)
				goto ERROR_EXIT;			
		}

	}

    return WMV_Succeeded;

ERROR_EXIT:
    RL_CloseReferenceLibrary(pWMVDec);

    return status;
}

Void_WMV RL_CloseReferenceLibrary(tWMVDecInternalMember * pWMVDec)
{
	CReferenceLibrary * pRL = pWMVDec->m_cReferenceLibrary;
    tYUV420Frame_WMV* pFrame;

    CQ_RemoveElement(pRL->m_cFrameQueue, (Void_WMV **)&pFrame, 0);

	while (pFrame != NULL_WMV)
	{
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO m_pCurrPlaneBase  :%x",(U32_WMV)pFrame->m_pCurrPlaneBase );
#endif
		FREE_PTR (pWMVDec, pFrame->m_pCurrPlaneBase);

#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO pFrame  :%x",(U32_WMV)pFrame );
#endif
		FREE_PTR (pWMVDec, pFrame);
		CQ_RemoveElement(pRL->m_cFrameQueue, (Void_WMV **)&pFrame, 0);
	}

    CQ_DestroyQueue(pWMVDec, pRL->m_cFrameQueue);
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO m_cFrameQueue  :%x",(U32_WMV)pRL->m_cFrameQueue );
#endif
    FREE_PTR (pWMVDec, pRL->m_cFrameQueue);
}

Void_WMV RL_RemoveFrame (CReferenceLibrary * pRL, Void_WMV **ppFrame, I32_WMV iFrameNum)
{
    I32_WMV iElementsInQ = pRL->m_cFrameQueue->m_iCurrQueueElements;
    if (iElementsInQ == 0 || (iFrameNum > (iElementsInQ - 1)))
        *ppFrame = NULL_WMV;
    else
    {
        if ((iFrameNum >= 0) || (pRL->m_iCurrRefFrames == iElementsInQ))
            pRL->m_iCurrRefFrames --;
        CQ_RemoveElement(pRL->m_cFrameQueue, ppFrame, iFrameNum);
    }
}

Void_WMV RL_ResetLibrary(CReferenceLibrary * pRL) {pRL->m_iCurrRefFrames = 0;}