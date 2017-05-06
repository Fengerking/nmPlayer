//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#include "xplatform_wmv.h"
#include "typedef.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "constants_wmv.h"
#include "tables_wmv.h"
#include "strmdec_wmv.h"
#include "codehead.h"
#include "localhuffman_wmv.h"

#define TRACEVC1FILE "VC1_DEC_LOG.txt"


I32_WMV ReallocateStartCodeBuffer(tWMVDecInternalMember *pWMVDec, U32_WMV nOldBufferSize, U32_WMV nNewBufferSize)
{
    U8_WMV *pTempBuffer = NULL;

    pWMVDec->m_iParseStartCodeBufLen = nNewBufferSize + 4096;

    if (nOldBufferSize > 0) {

        pTempBuffer = (U8_WMV *)wmvMalloc(pWMVDec, nOldBufferSize * sizeof(U8_WMV), DHEAP_STRUCT);
        if (pTempBuffer == NULL)
            return WMV_BadMemory;
        
        memcpy(pTempBuffer, pWMVDec->m_pParseStartCodeBitstream, nOldBufferSize);
    }

    //FREE_PTR(pWMVDec, pWMVDec->m_pParseStartCodeBitstream);
    FREE_PTR(pWMVDec, pWMVDec->m_pParseStartCodeBuffer);

    pWMVDec->m_pParseStartCodeBuffer = (U8_WMV*)wmvMalloc(pWMVDec, 2*pWMVDec->m_iParseStartCodeBufLen+32, DHEAP_STRUCT);
    if (pWMVDec->m_pParseStartCodeBuffer == NULL_WMV) {
        return WMV_BadMemory;
    }

    pWMVDec->m_pParseStartCodeBitstream = pWMVDec->m_pParseStartCodeBuffer + ((pWMVDec->m_iParseStartCodeBufLen+31)&~31);

    if (nOldBufferSize > 0)
    {
        memcpy(pWMVDec->m_pParseStartCodeBitstream, pTempBuffer, nOldBufferSize);
        FREE_PTR(pWMVDec, pTempBuffer);
    }
    return WMV_Succeeded;
}

I32_WMV ParseStartCode ( tWMVDecInternalMember *pWMVDec, U8_WMV *  pBuffer, U32_WMV    dwBufferLen, U8_WMV *  *ppOutBuffer, U32_WMV    *pdwOutBufferLen, Bool_WMV * pbNotEndOfFrame)
{
    Bool_WMV bNotEndOfFrame = *pbNotEndOfFrame;
    Bool_WMV bFrameOnly = TRUE;
    U8_WMV *pTemp;
    U32_WMV uiFirstMBRow;
    Void_WMV * pOutBuffer1 = 0;
    Void_WMV * pOutBuffer2 = 0;
    U32_WMV dwTotalFrameBufferLen = 0;
    U32_WMV dwFrameBufferLen;

    U8_WMV type = 0;
    Bool_WMV bLastIDU = FALSE;
    Bool_WMV bFrameFieldSliceDetected = FALSE;
    Bool_WMV bCheckEOCStartCode = FALSE;
    
    U8_WMV *pInterBuffer = pWMVDec->m_pParseStartCodeBitstream;
    U8_WMV *pFrameBuffer = pWMVDec->m_pParseStartCodeBuffer;
    U32_WMV nInterBufLen = 0;
    U32_WMV i;

#ifdef STABILITY
	if(pInterBuffer == NULL || pFrameBuffer == NULL)
		return ICERR_ERROR;
#endif

#ifdef TRACEVC1
	if(1){
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp," ParseStartCode %d %d \n",pWMVDec->m_iFrmWidthSrc,pWMVDec->m_iFrmHeightSrc);
		fclose(fp);
	}
#endif
    
    pWMVDec->m_bParseStartCodeSecondField = FALSE;
    pWMVDec->m_pInputBitstream->m_bPassingStartCode = 1;


    //result = SetSliceWMVA (pWMVDec, FALSE, 0, 0, pWMVDec->m_bParseStartCodeSecondField);
    if(SetSliceWMVA (pWMVDec, FALSE, 0, 0, pWMVDec->m_bParseStartCodeSecondField))
        return ICERR_ERROR;
    if ((pBuffer == 0) || (ppOutBuffer == 0) || (pdwOutBufferLen == 0))
        return ICERR_ERROR;

    if (dwBufferLen < 4 ) {
        memcpy(pInterBuffer, pBuffer, dwBufferLen);
        nInterBufLen += dwBufferLen;
    }

    if ((dwBufferLen + nInterBufLen) > pWMVDec->m_iParseStartCodeBufLen) {
        if (WMV_Succeeded != ReallocateStartCodeBuffer(pWMVDec, nInterBufLen, dwBufferLen + nInterBufLen))
            return ICERR_MEMORY;
        
        pInterBuffer = pWMVDec->m_pParseStartCodeBitstream;
        pFrameBuffer = pWMVDec->m_pParseStartCodeBuffer;
    }


restart0:
    if (dwBufferLen < 4 ) {

        if(bNotEndOfFrame == FALSE)
            bFrameOnly = TRUE;
        else
        {
            WMVDecCBGetData (pWMVDec, 0, (U8_WMV**) &pBuffer, 4, &dwBufferLen, &bNotEndOfFrame);
            pWMVDec->m_pInputBitstream->m_bNotEndOfFrame = bNotEndOfFrame;

            if ((dwBufferLen + nInterBufLen) > pWMVDec->m_iParseStartCodeBufLen) {
                if (WMV_Succeeded != ReallocateStartCodeBuffer(pWMVDec, nInterBufLen, dwBufferLen + nInterBufLen))
                    return ICERR_MEMORY;

                pInterBuffer = pWMVDec->m_pParseStartCodeBitstream;
                pFrameBuffer = pWMVDec->m_pParseStartCodeBuffer;
            }

            memcpy(pInterBuffer+nInterBufLen, pBuffer, dwBufferLen);
            nInterBufLen += dwBufferLen;

            pBuffer = pInterBuffer;
            dwBufferLen = nInterBufLen;
            goto restart0;

        }
    }
    else if ((((U8_WMV*)pBuffer)[0] == 0) && (((U8_WMV*)pBuffer)[1] == 0) && (((U8_WMV*)pBuffer)[2] == 1)) {
        bFrameOnly = FALSE;
    }
    else {
        bFrameOnly = TRUE;
    }

    pWMVDec->m_bDelaySetSliceWMVA = FALSE;
    pWMVDec->m_bParseStartCodeFrameOnly = bFrameOnly;

    if (bFrameOnly) {
        *pdwOutBufferLen = dwBufferLen; 
        *ppOutBuffer = pBuffer;
        *pbNotEndOfFrame = bNotEndOfFrame;
        return ICERR_OK;
    }

// If code reaches here, this ASF frame contains more than frame header and bFrameOnly = FALSE;
    while ((dwBufferLen >=  4) && !bLastIDU) {
restart:
        type = ((U8_WMV*)pBuffer)[3];
        if(!pWMVDec->m_bCodecIsWVC1){
           pOutBuffer1 = (U8_WMV*)pBuffer + 4;
        }
        else {
		if(type == SC_FIELD )  //zou 323
			pOutBuffer1 = (U8_WMV*)pBuffer;			
		else
			pOutBuffer1 = (U8_WMV*)pBuffer + 4;
        }

        if (dwBufferLen < 8) {
            bLastIDU = TRUE;
            pOutBuffer2 = (U8_WMV*)pBuffer + dwBufferLen;
        }
        else {

            for (i=4; i <= dwBufferLen-4; i++) {
                if ((((U8_WMV*)pBuffer)[i] == 0) && (((U8_WMV*)pBuffer)[i+1] == 0) && (((U8_WMV*)pBuffer)[i+2] == 1)) {  
                    pOutBuffer2 = (U8_WMV*)pBuffer + i;
                    i = dwBufferLen+1;
                }
            }
            if (i < dwBufferLen) {
                bLastIDU = TRUE;
                pOutBuffer2 = (U8_WMV*)pBuffer + dwBufferLen;
            }
        }

        pWMVDec->m_iParseStartCodeLastType = type;

        if (type == SC_FRAME || type == SC_FIELD || type == SC_SLICE)
            bFrameFieldSliceDetected = TRUE;
        if(bLastIDU== TRUE && bFrameFieldSliceDetected == FALSE && bNotEndOfFrame == TRUE)
        {
            U32_WMV dwBufferLenTmp = (U32_WMV)((U32_WMV )pOutBuffer2 - (U32_WMV )pBuffer);
            memcpy(pInterBuffer, pBuffer, dwBufferLenTmp);
            nInterBufLen = dwBufferLenTmp;

            WMVDecCBGetData (pWMVDec, 0, (U8_WMV**) &pBuffer, 4, &dwBufferLen, &bNotEndOfFrame);
            pWMVDec->m_pInputBitstream->m_bNotEndOfFrame = bNotEndOfFrame;

            if ((dwBufferLen + nInterBufLen) > pWMVDec->m_iParseStartCodeBufLen) {
                if (WMV_Succeeded != ReallocateStartCodeBuffer(pWMVDec, nInterBufLen, dwBufferLen  + nInterBufLen))
                    return ICERR_MEMORY;
                
                pInterBuffer = pWMVDec->m_pParseStartCodeBitstream;
                pFrameBuffer = pWMVDec->m_pParseStartCodeBuffer;
            }

            memcpy(pInterBuffer+nInterBufLen, pBuffer, dwBufferLen);
            nInterBufLen += dwBufferLen;

            pBuffer = pInterBuffer;
            dwBufferLen = nInterBufLen;
            
            bLastIDU = FALSE;
            goto restart;
        }

         if(bNotEndOfFrame && bLastIDU == TRUE)
        {
            U32_WMV dwFrameBufferLen = (U32_WMV)((U32_WMV)pOutBuffer2 - (U32_WMV)pOutBuffer1);

            U8_WMV * p = (U8_WMV *)pOutBuffer2;
            
            if( dwFrameBufferLen >2 && p[-1] == 1 && p[-2] == 0 && p[-3] == 0)
            {
                pWMVDec->m_iParseStartCodeRemLen = 3;
                pWMVDec->m_iParseStartCodeRem[0] = 0;
                pWMVDec->m_iParseStartCodeRem[1] = 0;
                pWMVDec->m_iParseStartCodeRem[2] = 1;
            }
            else if(dwFrameBufferLen >1 && p[-1] == 0 && p[-2] == 0)
            {
                pWMVDec->m_iParseStartCodeRemLen = 2;
                pWMVDec->m_iParseStartCodeRem[0] = 0;
                pWMVDec->m_iParseStartCodeRem[1] = 0;
            }
            else if(dwFrameBufferLen >0 && p[-1] == 0)
            {
                pWMVDec->m_iParseStartCodeRemLen = 1;
                pWMVDec->m_iParseStartCodeRem[0] = 0;
            }
            else
                pWMVDec->m_iParseStartCodeRemLen = 0;
            
            dwFrameBufferLen -= pWMVDec->m_iParseStartCodeRemLen;
            
            pOutBuffer2 = (U8_WMV *)pOutBuffer1 + dwFrameBufferLen;
            bCheckEOCStartCode = TRUE;
        }
        
		//wshao log.2010.12.30
		pWMVDec->m_pInputBitstream->m_pWMVDec = pWMVDec;

        if (type != SC_FRAME && type != SC_FIELD && type != SC_SLICE) {

            U32_WMV dwBufferLenTmp = (U32_WMV)((U32_WMV)pOutBuffer2 - (U32_WMV)pOutBuffer1);

            Void_WMV *pBufferTmp = pOutBuffer1;
            switch(type) {
                case SC_SEQ :
                    //switch to new sequence

					//printf("SC_SEQ\n");
                    BS_reset(pWMVDec->m_pInputBitstream, (U8_WMV*)pBufferTmp, dwBufferLenTmp, 0, pWMVDec->m_cvCodecVersion == WMVA);
                    if (ICERR_OK !=SwitchSequence(pWMVDec, TRUE))
					{
						pWMVDec->sequence_errorflag = 1;
                        return ICERR_ERROR;
					}
					pWMVDec->sequence_errorflag = 0;
					pWMVDec->bNewheader = 1;
                    break;

                case SC_ENTRY :
					//printf("SC_ENTRY \n");
                    BS_reset(pWMVDec->m_pInputBitstream, (U8_WMV*)pBufferTmp, dwBufferLenTmp, 0, pWMVDec->m_cvCodecVersion == WMVA);
                    if (ICERR_OK != SwitchEntryPoint(pWMVDec)) {
						pWMVDec->entrypoint_errorflag = 1;
                        return ICERR_ERROR;
                    }
					pWMVDec->entrypoint_errorflag = 0;
					pWMVDec->bNewheader = 1;                    
                    break;
                case SC_ENDOFSEQ:
                    break;
            }

        }
        else {
            dwFrameBufferLen = (U32_WMV)((U32_WMV)pOutBuffer2 - (U32_WMV)pOutBuffer1);

            if (type == SC_FIELD) 
                pWMVDec->m_bParseStartCodeSecondField = TRUE;

            if (type == SC_SLICE) 
            {                
                pTemp = (U8_WMV *) pOutBuffer1;
                if(dwFrameBufferLen > 1)
                {
                    
                    uiFirstMBRow = (U32_WMV) pTemp [0] * 2 + (((U32_WMV) pTemp [1] & 0x80) >> 7);
                    if(SetSliceWMVA (pWMVDec, TRUE, uiFirstMBRow, (U32_WMV) dwFrameBufferLen, pWMVDec->m_bParseStartCodeSecondField))
                        return ICERR_ERROR;
                }
                else if(bNotEndOfFrame == TRUE)
                {
                    pWMVDec->m_bDelaySetSliceWMVA = TRUE;

                    if (dwFrameBufferLen == 1) {
#ifdef STABILITY
                        if(pWMVDec->m_iParseStartCodeRemLen>2)
                            return ICERR_ERROR;
#endif
                        for (i = pWMVDec->m_iParseStartCodeRemLen; i > 0; i--)
                            pWMVDec->m_iParseStartCodeRem[i] = pWMVDec->m_iParseStartCodeRem[i-1];

                        pWMVDec->m_iParseStartCodeRem[0] = pTemp[0];
                        pWMVDec->m_iParseStartCodeRemLen += 1;
                        dwFrameBufferLen = 0;
                    }
                }
            }

            memcpy (pFrameBuffer + dwTotalFrameBufferLen, (U8_WMV *) pOutBuffer1, dwFrameBufferLen);
            dwTotalFrameBufferLen += dwFrameBufferLen;
        }
        dwBufferLen = dwBufferLen - (U32_WMV)((U32_WMV)pOutBuffer2 - (U32_WMV)pBuffer);
        pBuffer = (U8_WMV *)pOutBuffer2;
    }
   
    if(bNotEndOfFrame && bCheckEOCStartCode == FALSE)
    {
        U8_WMV * p = pFrameBuffer + dwTotalFrameBufferLen;
        
        if( dwTotalFrameBufferLen >2 && p[-1] == 1 && p[-2] == 0 && p[-3] == 0)
        {
            pWMVDec->m_iParseStartCodeRemLen = 3;
            pWMVDec->m_iParseStartCodeRem[0] = 0;
            pWMVDec->m_iParseStartCodeRem[1] = 0;
            pWMVDec->m_iParseStartCodeRem[2] = 1;
       }
        else if(dwTotalFrameBufferLen >1 && p[-1] == 0 && p[-2] == 0)
        {
            pWMVDec->m_iParseStartCodeRemLen = 2;
            pWMVDec->m_iParseStartCodeRem[0] = 0;
            pWMVDec->m_iParseStartCodeRem[1] = 0;
        }
        else if(dwTotalFrameBufferLen >0 && p[-1] == 0)
        {
            pWMVDec->m_iParseStartCodeRemLen = 1;
            pWMVDec->m_iParseStartCodeRem[0] = 0;
        }
        else
            pWMVDec->m_iParseStartCodeRemLen = 0;

        dwTotalFrameBufferLen -= pWMVDec->m_iParseStartCodeRemLen;
    }
    *ppOutBuffer = pFrameBuffer;
    *pdwOutBufferLen = dwTotalFrameBufferLen;
    *pbNotEndOfFrame = bNotEndOfFrame;
   
    return ICERR_OK;
}


I32_WMV ParseStartCodeChunk ( tWMVDecInternalMember *pWMVDec, U8_WMV *  pBuffer, U32_WMV    dwBufferLen, U8_WMV *  *ppOutBuffer, U32_WMV    *pdwOutBufferLen, Bool_WMV bNotEndOfFrame)
{
    U8_WMV *pTemp;
    //Bool_WMV bSliceCodeFound = FALSE;
    U32_WMV uiFirstMBRow;
    Void_WMV * pOutBuffer1 = 0;
    Void_WMV * pOutBuffer2 = 0;
    U32_WMV dwTotalFrameBufferLen = 0;
    U32_WMV dwFrameBufferLen;
    //I32_WMV result = 0;
    
    U8_WMV type = 0;
    Bool_WMV bLastIDU = FALSE;
    //U8_WMV * pBuffer2 = NULL;
    U32_WMV iOffset;
    U32_WMV iSize;
    I32_WMV i;
    Bool_WMV bCheckEOCStartCode = FALSE;
    
    U8_WMV *pInterBuffer = pWMVDec->m_pParseStartCodeBitstream;
    U8_WMV *pFrameBuffer = pWMVDec->m_pParseStartCodeBuffer;
    
    if ((pBuffer == 0) || (ppOutBuffer == 0) || (pdwOutBufferLen == 0))
        return ICERR_ERROR;

    iSize = dwBufferLen + pWMVDec->m_iParseStartCodeRemLen;
    if (iSize > pWMVDec->m_iParseStartCodeBufLen) {
        if (WMV_Succeeded != ReallocateStartCodeBuffer(pWMVDec, 0, iSize))
            return ICERR_MEMORY;
        
        pInterBuffer = pWMVDec->m_pParseStartCodeBitstream;
        pFrameBuffer = pWMVDec->m_pParseStartCodeBuffer;
    }
    
    if (pWMVDec->m_bParseStartCodeFrameOnly) {
        *pdwOutBufferLen = dwBufferLen; 
        *ppOutBuffer = pBuffer;
        return ICERR_OK;
    }
    
    if(pWMVDec->m_iParseStartCodeRemLen > 0)
    {
        for(i = 0; i < pWMVDec->m_iParseStartCodeRemLen; i++)
        {
            pInterBuffer[i] = pWMVDec->m_iParseStartCodeRem[i];
        }
        
        memcpy(pInterBuffer+pWMVDec->m_iParseStartCodeRemLen, pBuffer, dwBufferLen);
        
        pBuffer = pInterBuffer;
        dwBufferLen += pWMVDec->m_iParseStartCodeRemLen;
        pWMVDec->m_iParseStartCodeRemLen = 0;
    }
    
    
    // If code reaches here, this ASF frame contains more than frame header and bFrameOnly = FALSE;
    type = pWMVDec->m_iParseStartCodeLastType;
    iOffset = 0;
    
    while ((dwBufferLen >=  iOffset) && !bLastIDU) {
        
        if(iOffset != 0)
        {
            type = ((U8_WMV*)pBuffer)[3];
        }
        
        pOutBuffer1 = (U8_WMV*)pBuffer + iOffset;
        
        
        if (dwBufferLen < (4+iOffset)) {
            bLastIDU = TRUE;
            pOutBuffer2 = (U8_WMV*)pBuffer + dwBufferLen;
        }
        else {
            unsigned long i;
            for (i=iOffset; i <= dwBufferLen-4; i++) {
                if ((((U8_WMV*)pBuffer)[i] == 0) && (((U8_WMV*)pBuffer)[i+1] == 0) && (((U8_WMV*)pBuffer)[i+2] == 1)) {  
                    pOutBuffer2 = (U8_WMV*)pBuffer + i;
                    i = dwBufferLen+1;
                }
            }
            if (i < dwBufferLen) {
                bLastIDU = TRUE;
                pOutBuffer2 = (U8_WMV*)pBuffer + dwBufferLen;
            }
        }
        
        if(bLastIDU == TRUE)
            pWMVDec->m_iParseStartCodeLastType = type;
        
        if(bNotEndOfFrame && bLastIDU == TRUE)
        {
            U32_WMV dwFrameBufferLen = (U32_WMV)((U32_WMV)pOutBuffer2 - (U32_WMV)pOutBuffer1);
            U8_WMV * p = (U8_WMV *)pOutBuffer2;
            
            if( dwFrameBufferLen >2 && p[-1] == 1 && p[-2] == 0 && p[-3] == 0)
            {
                pWMVDec->m_iParseStartCodeRemLen = 3;
                pWMVDec->m_iParseStartCodeRem[0] = 0;
                pWMVDec->m_iParseStartCodeRem[1] = 0;
                pWMVDec->m_iParseStartCodeRem[2] = 1;
            }
            else if(dwFrameBufferLen >1 && p[-1] == 0 && p[-2] == 0)
            {
                pWMVDec->m_iParseStartCodeRemLen = 2;
                pWMVDec->m_iParseStartCodeRem[0] = 0;
                pWMVDec->m_iParseStartCodeRem[1] = 0;
            }
            else if(dwFrameBufferLen >0 && p[-1] == 0)
            {
                pWMVDec->m_iParseStartCodeRemLen = 1;
                pWMVDec->m_iParseStartCodeRem[0] = 0;
            }
            else
                pWMVDec->m_iParseStartCodeRemLen = 0;
            
            dwFrameBufferLen -= pWMVDec->m_iParseStartCodeRemLen;
            
            pOutBuffer2 = (U8_WMV *)pOutBuffer1 + dwFrameBufferLen;
            
            bCheckEOCStartCode = TRUE;
        }
        
        if (type != SC_FRAME && type != SC_FIELD && type != SC_SLICE) {
            // To Do 
            switch(type) {

            case SC_ENDOFSEQ:
                break;
            default:
                //assert(0);
				return WMV_Failed;
                break;
            }
            
        }
        else {
            dwFrameBufferLen = (U32_WMV)((U32_WMV)pOutBuffer2 - (U32_WMV)pOutBuffer1);
          
            if(iOffset == 4 || pWMVDec->m_bDelaySetSliceWMVA == TRUE)
            {
               if (type == SC_FIELD) {
                    pWMVDec->m_bParseStartCodeSecondField = TRUE;                    
                }
                if (type == SC_SLICE) 
                {
                    pTemp = (U8_WMV *) pOutBuffer1;

                    if (iOffset == 0)
                    {
                        if (pWMVDec->m_bDelaySetSliceWMVA == TRUE)
                        {
                            if(dwFrameBufferLen > 1)
                            {
                                uiFirstMBRow = (U32_WMV) pTemp [0] * 2 + (((U32_WMV) pTemp [1] & 0x80) >> 7);
                                if(SetSliceWMVA (pWMVDec, TRUE, uiFirstMBRow, (U32_WMV) dwFrameBufferLen, pWMVDec->m_bParseStartCodeSecondField))
                                    return ICERR_ERROR;
                                pWMVDec->m_bDelaySetSliceWMVA = FALSE;
                            }
                            else 
                            {
                                if(bNotEndOfFrame == TRUE)
                                {
                                    if (dwFrameBufferLen == 1) {
#ifdef STABILITY
                                        if(pWMVDec->m_iParseStartCodeRemLen > 2)
                                            return ICERR_ERROR;
#endif
                                        for (i = pWMVDec->m_iParseStartCodeRemLen; i > 0; i--)
                                            pWMVDec->m_iParseStartCodeRem[i] = pWMVDec->m_iParseStartCodeRem[i-1];
                                        
                                        pWMVDec->m_iParseStartCodeRem[0] = pTemp[0];
                                        pWMVDec->m_iParseStartCodeRemLen += 1;
                                        dwFrameBufferLen = 0;
                                        pWMVDec->m_bDelaySetSliceWMVA = TRUE;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if(dwFrameBufferLen > 1)
                        {
                            uiFirstMBRow = (U32_WMV) pTemp [0] * 2 + (((U32_WMV) pTemp [1] & 0x80) >> 7);
                            if(SetSliceWMVA (pWMVDec, TRUE, uiFirstMBRow, (U32_WMV) dwFrameBufferLen, pWMVDec->m_bParseStartCodeSecondField))
                                return ICERR_ERROR;
                            pWMVDec->m_bDelaySetSliceWMVA = FALSE;
                        }
                        else 
                        {
                            if(bNotEndOfFrame == TRUE)
                            {
                                pWMVDec->m_bDelaySetSliceWMVA = TRUE;

                                if (dwFrameBufferLen == 1) {
#ifdef STABILITY
                                    if(pWMVDec->m_iParseStartCodeRemLen > 2)
                                        return ICERR_ERROR;
#endif
                                    for (i = pWMVDec->m_iParseStartCodeRemLen; i > 0; i--)
                                        pWMVDec->m_iParseStartCodeRem[i] = pWMVDec->m_iParseStartCodeRem[i-1];

                                    pWMVDec->m_iParseStartCodeRem[0] = pTemp[0];
                                    pWMVDec->m_iParseStartCodeRemLen += 1;
                                    dwFrameBufferLen = 0;
                                }
                            }
                        }
                    }
                }
            }
            memcpy (pFrameBuffer + dwTotalFrameBufferLen, (U8_WMV *) pOutBuffer1, dwFrameBufferLen);
            dwTotalFrameBufferLen += dwFrameBufferLen;
        };
        
        iOffset = 4;
        
        dwBufferLen = dwBufferLen - (U32_WMV)((U32_WMV)pOutBuffer2 - (U32_WMV)pBuffer);

        pBuffer = (U8_WMV *)pOutBuffer2;
    }
    
    
    if(bNotEndOfFrame && bCheckEOCStartCode == FALSE)
    {        
        U8_WMV * p = pFrameBuffer + dwTotalFrameBufferLen;        
        
        if( dwTotalFrameBufferLen >2 && p[-1] == 1 && p[-2] == 0 && p[-3] == 0)
        {
            pWMVDec->m_iParseStartCodeRemLen = 3;
            pWMVDec->m_iParseStartCodeRem[0] = 0;
            pWMVDec->m_iParseStartCodeRem[1] = 0;
            pWMVDec->m_iParseStartCodeRem[2] = 1;
        }
        else if(dwTotalFrameBufferLen >1 && p[-1] == 0 && p[-2] == 0)
        {
            pWMVDec->m_iParseStartCodeRemLen = 2;
            pWMVDec->m_iParseStartCodeRem[0] = 0;
            pWMVDec->m_iParseStartCodeRem[1] = 0;
        }
        else if(dwTotalFrameBufferLen >0 && p[-1] == 0)
        {
            pWMVDec->m_iParseStartCodeRemLen = 1;
            pWMVDec->m_iParseStartCodeRem[0] = 0;
        }
        else
            pWMVDec->m_iParseStartCodeRemLen = 0;
        
        dwTotalFrameBufferLen -= pWMVDec->m_iParseStartCodeRemLen;
    }
    
    *ppOutBuffer = pFrameBuffer;
    *pdwOutBufferLen = dwTotalFrameBufferLen;
    
    return ICERR_OK;
}

