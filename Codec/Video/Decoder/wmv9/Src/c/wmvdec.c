//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#include "xplatform_wmv.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "constants_wmv.h"
#include "tables_wmv.h"
#include "codehead.h"
#include "localhuffman_wmv.h"



#ifdef LOG_ENABLE
//#define ANDROID_LOG 1
#ifdef VOANDROID
#define LOG_FILE "/data/local/log.txt"
#define LOG_TAG "VOPlayer"
#include <utils/Log.h>
#  define __VOLOG(fmt, args...) ({LOGD("->%d: %s(): " fmt, __LINE__, __FUNCTION__, ## args);})
#  define __VOLOGFILE(fmt, args...) ({FILE *fp =fopen(LOG_FILE, "a"); fprintf(fp, "[ %s ]->%d: %s(): " fmt "\n", LOG_TAG, __LINE__, __FUNCTION__, ## args); fclose(fp);})
#elif VOWINXP
#define LOG_FILE "D:/log.txt"
#define LOG_TAG "VOPlayer"
#include "stdio.h"
#define __VOLOG(fmt, ...)  printf("[ %s ]->%d: %s(): "fmt"\n",LOG_TAG,__LINE__,__FUNCTION__,__VA_ARGS__);
#define __VOLOGFILE(fmt, ...) {FILE *fp =fopen(LOG_FILE, "a");fprintf(fp, "[ %s ]->%d: %s(): " fmt "\n", LOG_TAG, __LINE__, __FUNCTION__, __VA_ARGS__); fclose(fp);}
//fprintf(fp, "[ %s ]->%d: %s(): " fmt "\n", LOG_TAG, __LINE__, __FUNCTION__, __VA_ARGS__); fclose(fp);})
#endif
#else
#define __VOLOGFILE
#  define __VOLOG printf
#endif
 
//#define DISABLE_MEM_OPERATOR

/* Support WVC1 decoding. */
#define SUPPORT_WVC1

/*There is no license check lib of RVDS, so need time limitation.*/
#ifdef RVDS
#define TIME_LIMIT	//decoder stop working after 60 minutes.
#define MAX_TIME	(30*60*60)	//30fps*60seconds*60minutes
#endif

/* Enable license check. */
#if defined(NDEBUG) || defined(_NDEBUG)
//#define CHKLCS_ON
#endif

/* wmv internal memory leak checking. */
#if defined(DEBUG) || defined(_DEBUG)
//#define CHECK_MEM_LEAK	
#endif

#ifdef _VOLOG_INFO
//#define DEBUG_INPUT_DATA
#endif

#ifdef	DUMP_INPUT_DATA
#define DUMP_INPUT_DATA_FILE_NAME	"/sdcard/wmvInputRawDataDump.rcv"
//#define DUMP_INPUT_DATA_FILE_NAME	"d:\\Test\\wmvInputRawDataDump.rcv"
#endif

#ifdef CHECK_MEM_LEAK
#define MEM_LEAK_LOG_FILE	"D:\\Test\\WMV_Memory_log.txt"
//#define MEM_LEAK_LOG_FILE	"wmv_mem_leak_log_device.txt"
extern void WMVSetTraceFlag (unsigned int flag);
#endif

#ifdef CHKLCS_ON
#include "voCheck.h"
#endif

#ifdef WMVIMAGE_V2
#include "effects_wmv.h"
#endif

#include "voWmvPort.h"

#ifdef macintosh
#include "cpudetect_mac.h"
#endif


extern Bool_WMV g_bSupportAltiVec_WMV;
extern Bool_WMV g_bSupportWMMX_WMV;


VO_HANDLE g_WMV_hModule = NULL; 	//for CheckLcs lib.



//#ifndef _APOGEE_
#if !defined(_APOGEE_) && !defined(_MAC) && !defined(_TI_C55X_) && !defined(__arm)
//#   include "malloc.h"
#   include "memory.h"
#endif
#include "Affine.h"
#ifdef UNDER_CE
    //  #undef assert
    //  #define assert ASSERT
#else
//#   include "assert.h"
#endif


#ifdef _WMV_TARGET_X86_
#   include "cpudetect.h"
#endif // _WMV_TARGET_X86_
#include "math.h"

// define this to force a particular level at each I-frame.  For debugging/dev only
// #define _QUICK_DEBLOCK_POSTFILTER_

//#pragma warning( disable: 4719 )

// ===========================================================================
// private functions
// ===========================================================================
#include "strmdec_wmv.h"
#include "repeatpad_wmv.h"


I32_WMV YUV411_To_YUV420( U8_WMV **pSrcBuff, U8_WMV **pDstBuff, I32_WMV *iSrcStride, I32_WMV iW, I32_WMV iH );

//static Bool_WMV test_bit(char* pData,int offset)
//{
//    static int bit[8]={0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1};
//    pData+=offset /8;
//    offset=offset%8;
//    return( (*pData) & bit[offset]);
//}
//static Bool_WMV XBOX_TestVOLHead(char* pData,int pSize)
//{
//    const int LoopFilter=2+1+1+3+5;
//    const int MultiRes=LoopFilter+1+1;
//    return !test_bit(pData,LoopFilter) && ! test_bit(pData,MultiRes);
//}

I32_WMV WMVideoIsSpriteMode (HWMVDecoder hWMVDecoder)
{
    tWMVDecInternalMember *pWMVDec;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);
    if (hWMVDecoder == NULL_WMV)
        return 0;

#ifdef WMVIMAGE_V2
    return ( pWMVDec->m_bSpriteMode || pWMVDec->m_bSpriteModeV2);

#else
    return pWMVDec->m_bSpriteMode;
#endif

}

I32_WMV WMVideoSpriteWidth (HWMVDecoder hWMVDecoder)
{
    tWMVDecInternalMember *pWMVDec;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);
    if (hWMVDecoder == NULL_WMV)
        return 0;

    return pWMVDec->m_iFrmWidthSrc;

}

I32_WMV WMVideoSpriteHeight (HWMVDecoder hWMVDecoder)
{
    tWMVDecInternalMember *pWMVDec;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);
    if (hWMVDecoder == NULL_WMV)
        return 0;

    return pWMVDec->m_iFrmHeightSrc;

}


typedef struct
{
    int iBytesReadFrm;
    int iCounter;
    U8_WMV *pVideoBuffer;
}StreamingModeDataCtl;

StreamingModeDataCtl g_StrmModedataCtl;

static I32_WMV gcd(I32_WMV m, I32_WMV n)
{
	if (m == 0)
		return n;
	if (n == 0)
		return m;
	if (m < n)
	{
		int tmp = m;
		m = n;
		n = tmp;
	}
	while (n != 0)
	{
		int tmp = m % n;
		m = n;
		n = tmp;
	}
	return m;
}

static I32_WMV GetAspectRatio(tWMVDecInternalMember*pWMVDec)
{
	I32_WMV width,height;
	I32_WMV iaspectratio = pWMVDec->tDimensions.iAspectRatio;
	switch (iaspectratio)
	{
	case 2: //12:11
		{
			width = pWMVDec->m_iFrmWidthSrc*12;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 3: //10:11
		{
			width = pWMVDec->m_iFrmWidthSrc*10;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 4: //16:11
		{
			width = pWMVDec->m_iFrmWidthSrc*16;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 5: //40:33
		{
			width = pWMVDec->m_iFrmWidthSrc*40;
		    height = pWMVDec->m_iFrmHeightSrc*33;
			break;
		}
	case 6: //24:11
		{
			width = pWMVDec->m_iFrmWidthSrc*24;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 7: //20:11
		{
			width = pWMVDec->m_iFrmWidthSrc*20;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 8: //32:11
		{
			width = pWMVDec->m_iFrmWidthSrc*10;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 9: //80:33
		{
			width = pWMVDec->m_iFrmWidthSrc*80;
		    height = pWMVDec->m_iFrmHeightSrc*33;
			break;
		}
	case 10: //18:11
		{
			width = pWMVDec->m_iFrmWidthSrc*18;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 11: //15:11
		{
			width = pWMVDec->m_iFrmWidthSrc*15;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 12: //64:33
		{
			width = pWMVDec->m_iFrmWidthSrc*64;
		    height = pWMVDec->m_iFrmHeightSrc*33;
			break;
		}
	case 13: //160:99
		{
			width = pWMVDec->m_iFrmWidthSrc*10;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 15: //160:99
		{
			width = pWMVDec->m_iFrmWidthSrc*pWMVDec->tDimensions.iAspect15HorizSize;
		    height = pWMVDec->m_iFrmHeightSrc*pWMVDec->tDimensions.iAspect15VertSize;
			break;
		}
	default:
		return 0;
	}
	
	if (width == height)
	{
		return VO_RATIO_11;
	}
	else if (width*3 == height*4)
	{
		return VO_RATIO_43;
	}
	else if (width*9 == height*16)
	{
		return VO_RATIO_169;
	}
	else if (width == height*2)
	{
		return VO_RATIO_21;
	}
	else
	{
		int m = gcd(width, height);
		if(m)
		{
			width = width/m;
			height = height/m;
		}
		if(width > 0x7fff || height > 0x7fff)
		{
			width = (width + 512) >> 10;
			height = (height + 512) >> 10;
		}
		return ((width<<16)|height);
	}
}

void V9ECBReset(U8_WMV * pBuffer, int iSize)
{
    g_StrmModedataCtl.iBytesReadFrm = iSize;
    g_StrmModedataCtl.iCounter = 0;
    g_StrmModedataCtl.pVideoBuffer = pBuffer;
}

#define GETDATA_BLOCKSIZE 0x00070800

tWMVDecodeStatus V9ECBGetData (
								U32_WMV   uiUserData,
								U32_WMV   uintPadBeginning,
								U8_WMV  **ppchBuffer, 
								U32_WMV   uintUserDataLength,
								U32_WMV  *puintActualBufferLength,
								Bool_WMV *pbNotEndOfFrame
							  )
{
#ifdef __STREAMING_MODE_DECODE_
    if (g_StrmModedataCtl.iBytesReadFrm > 0) {
        I32_WMV iBytesAvailable;
        if (g_StrmModedataCtl.iBytesReadFrm > GETDATA_BLOCKSIZE) {
            *ppchBuffer = g_StrmModedataCtl.pVideoBuffer + g_StrmModedataCtl.iCounter * GETDATA_BLOCKSIZE - uintPadBeginning;
            g_StrmModedataCtl.iCounter++;
            g_StrmModedataCtl.iBytesReadFrm -= GETDATA_BLOCKSIZE;
            iBytesAvailable = GETDATA_BLOCKSIZE;
            * pbNotEndOfFrame = TRUE_WMV;
        } 
		else {
            *ppchBuffer = g_StrmModedataCtl.pVideoBuffer + g_StrmModedataCtl.iCounter * GETDATA_BLOCKSIZE - uintPadBeginning;
            g_StrmModedataCtl.iCounter = 0;
            iBytesAvailable = g_StrmModedataCtl.iBytesReadFrm;
            g_StrmModedataCtl.iBytesReadFrm = 0;
            * pbNotEndOfFrame = FALSE_WMV;
        }
        *puintActualBufferLength = iBytesAvailable + uintPadBeginning;
        return WMV_Succeeded;
    } 
	else {
        *ppchBuffer = NULL;
        *puintActualBufferLength = 0;
        *pbNotEndOfFrame = FALSE_WMV;
        return WMV_EndOfFrame;
    }
#else
    *ppchBuffer              = g_StrmModedataCtl.pVideoBuffer;
    *puintActualBufferLength = g_StrmModedataCtl.iBytesReadFrm;
    *pbNotEndOfFrame         = FALSE_WMV;
    return WMV_Succeeded;
#endif
}

tWMVDecodeStatus WMVDecCBGetData ( U32_WMV   uiUserData, 
								   U32_WMV   uintPadBeginning, 
								   U8_WMV  **ppchBuffer, 
								   U32_WMV   uintUserDataLength, 
                                   U32_WMV  *puintActualBufferLength, 
								   Bool_WMV *pbNotEndOfFrame 
								   )
{
	return V9ECBGetData ( uiUserData, uintPadBeginning, ppchBuffer, uintUserDataLength, puintActualBufferLength, pbNotEndOfFrame );
}

tWMVDecodeStatus GetOutputParam (
								HWMVDecoder   hWMVDecoder,
								U8_WMV      **pOutBuff,
								I32_WMV      *pStride
								)
{
    I32_WMV                StrideY  = 0, StrideU  = 0, StrideV = 0;
    I32_WMV                iOffsetX = 0, iOffsetY = 0;
    I32_WMV                iPadding = 64;   
    I32_WMV                iWidthSource =0;
    I32_WMV                iHeightSource =0;
	I32_WMV                iFullSrcWidth;
	I32_WMV                iFullSrcHeight;
	I32_WMV                iSrcUVStride;
	I32_WMV                iSrcYOffset;
	I32_WMV                iSrcUOffset;
	I32_WMV                iSrcVOffset;
	I32_WMV                uiInputYOffset;
	I32_WMV                uiInputUOffset;
	I32_WMV                uiInputVOffset;
	I32_WMV                iOffsetYSrc;
	I32_WMV                iOffsetUVSrc;
	U8_WMV                *ppxliRenderThisY = NULL;
	U8_WMV                *ppxliRenderThisU = NULL;
	U8_WMV                *ppxliRenderThisV = NULL;
    tWMVDecInternalMember *pWMVDec          = NULL; 

#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO GetOutputParam 1");
#endif

    if (hWMVDecoder == NULL_WMV) {
        return WMV_InValidArguments;
	}
	pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);

    if ( pWMVDec->m_bRenderDirect == TRUE_WMV) {
        pWMVDec->m_bRenderDirect =FALSE_WMV;

        pWMVDec->m_uiNumFramesLeftForOutput = 1;

        if(pWMVDec->m_bRenderDirectWithPrerender == TRUE_WMV)
        {
            pWMVDec->m_bRenderDirectWithPrerender = FALSE_WMV;
            goto prerender;
        }
        goto render;
    }
    
    if (pWMVDec->m_uiNumFramesLeftForOutput == 0) {
        return WMV_NoMoreOutput;
    }    
    
    pWMVDec->m_eFrameWasOutput = FrameDecOut_OutputDone;
    
    FUNCTION_PROFILE_START(&fp,WMVDECGETOUTPUT);
    
    if ( WMVideoIsSpriteMode (pWMVDec) ) { 

		ppxliRenderThisY = (U8_WMV*) (pWMVDec->m_pvopcRenderThis->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp);
		ppxliRenderThisU = (U8_WMV*) (pWMVDec->m_pvopcRenderThis->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp);
		ppxliRenderThisV = (U8_WMV*) (pWMVDec->m_pvopcRenderThis->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp);    
 		pOutBuff[0] = ppxliRenderThisY;
		pOutBuff[1] = ppxliRenderThisU;
		pOutBuff[2] = ppxliRenderThisV;
#ifdef VIDEO_MEM_SHARE
		if( pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP ) {
			pStride[0]  = pWMVDec->m_frameBufStrideY;
			pStride[1]  = pWMVDec->m_frameBufStrideUV;
			pStride[2]  = pWMVDec->m_frameBufStrideUV;
		}
		else 
#endif
		{
			iWidthSource = pWMVDec->m_iSpriteWidthDsiplay;
			pStride[0]  = iWidthSource;
			pStride[1]  = iWidthSource>>1;
			pStride[2]  = iWidthSource>>1;
		}
       
		pWMVDec->m_iRefBuffOffsetForCheck = pWMVDec->m_iWidthPrevYXExpPlusExp;
		pWMVDec->m_iRefBuffYStrideForCheck = iWidthSource;
        pWMVDec->m_uiNumFramesLeftForOutput--;

        return WMV_Succeeded;
    }
    
    pWMVDec->m_bRenderFromPostBuffer = FALSE_WMV;
    
prerender:

#ifndef WMV9_SIMPLE_ONLY
    if (pWMVDec->m_bPreProcRange) {
#ifdef _WMV9AP_
        if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {
#else
        if (pWMVDec->m_tFrmPredType == IVOP) {
#endif
            if (pWMVDec->m_iNumBFrames == 0)
                pWMVDec->m_iReconRangeState = pWMVDec->m_iRangeState;
            else if (pWMVDec->m_tFrmType != BVOP) {
                if (pWMVDec->m_iReconRangeState == 0 && pWMVDec->m_iRangeState == 1){
                    CopyRefToPrevPost (pWMVDec); // possible unnecessary copy for now - but cleaner implementation
                }
                if(pWMVDec->m_bEMB3FrmEnabled){
                    if(pWMVDec->m_bAltRefFrameEnabled){
                        AdjustReconRange2AltRef (pWMVDec);
                    }
                }
                else
                    AdjustReconRange (pWMVDec);
            }
        }
        if (pWMVDec->m_iRangeState != 0) { 
            if (pWMVDec->m_bRenderFromPostBuffer == FALSE) {
                CopyCurrToPost (pWMVDec);
                pWMVDec->m_bRenderFromPostBuffer = TRUE;
            }
            AdjustDecRange (pWMVDec);
        } 
    }
    
    if (pWMVDec->m_cvCodecVersion == WMVA && (pWMVDec->m_bRangeRedY_Flag || pWMVDec->m_bRangeRedUV_Flag)) {
        if (pWMVDec->m_bRenderFromPostBuffer == FALSE) {
            CopyCurrToPost (pWMVDec);
        }
        AdjustDecRangeWMVA (pWMVDec);
        pWMVDec->m_bRenderFromPostBuffer = TRUE;
    }

#endif //WMV9_SIMPLE_ONLY


    if (pWMVDec->m_iNumBFrames > 0) {
#ifndef _WMV9AP_
        pWMVDec->m_tFrmPredType = pWMVDec->m_tFrmType;
        pWMVDec->m_bPrevAnchor = (pWMVDec->m_tFrmType != BVOP) || (pWMVDec->m_bOmittedBFrame && pWMVDec->m_tFrmTypePriorFrame != BVOP);
#else
        pWMVDec->m_bPrevAnchor = (pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP);
#endif
    }

    PreGetOutput(pWMVDec);     // do all the necessary buffer swapping here...

render:

    ppxliRenderThisY = (U8_WMV*) (pWMVDec->m_pvopcRenderThis->m_pucYPlane);
    ppxliRenderThisU = (U8_WMV*) (pWMVDec->m_pvopcRenderThis->m_pucUPlane);
    ppxliRenderThisV = (U8_WMV*) (pWMVDec->m_pvopcRenderThis->m_pucVPlane);    

    if(pWMVDec->m_bSeqFrameWidthOrHeightChanged == TRUE && pWMVDec->m_iNumBFrames > 0 && pWMVDec->m_t > 1) {
        iWidthSource  = pWMVDec->m_iFrmWidthSrcOld;
        iHeightSource = pWMVDec->m_iFrmHeightSrcOld;
    }
    else {
        iWidthSource  = pWMVDec->m_iFrmWidthSrc;
        iHeightSource = pWMVDec->m_iFrmHeightSrc;
    }
            
    if (!pWMVDec->m_bSpriteMode) { 
        iWidthSource = ( (iWidthSource + 15) >> 4 ) << 4;
    }
    
    if (pWMVDec->m_bYUV411) {
        StrideY  = iWidthSource        + 64;
        StrideU  = (iWidthSource >> 2) + 32;
        StrideV  = StrideU;
        iOffsetX = 0;
        iOffsetY = 0;
    }
    else {
        StrideY  = 0;
        StrideU  = 0;
        StrideV  = 0;
        iOffsetX = 32;
        iOffsetY = 32;
    }
    
    if (pWMVDec->m_bYUV411) {
        ppxliRenderThisY += StrideY * 32 + 32;
        ppxliRenderThisU += StrideU * 16 + 16;
        ppxliRenderThisV += StrideV * 16 + 16;
    };

	iFullSrcHeight = iHeightSource + iPadding;
#ifdef VIDEO_MEM_SHARE
		if( pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP ) {
			iFullSrcWidth = pWMVDec->m_iWidthPrevY;
			iSrcUVStride  = pWMVDec->m_iWidthPrevUV;
		}
		else 
#endif
		{
			iFullSrcWidth  = iWidthSource  + iPadding;
			iSrcUVStride   = StrideU ? StrideU : (iFullSrcWidth >> 1);
		}

	iOffsetYSrc  = 0 * iFullSrcWidth;
	iOffsetUVSrc = iOffsetYSrc >> 2;

	uiInputYOffset = 0;
	uiInputUOffset = iFullSrcWidth   * iFullSrcHeight;
	uiInputVOffset = uiInputUOffset  + iSrcUVStride  * (iFullSrcHeight >> 1);
	iSrcYOffset    = iOffsetY        * iFullSrcWidth + iOffsetX;
	iSrcUOffset    = (iOffsetY >> 1) * iSrcUVStride  + (iOffsetX >> 1);
	iSrcVOffset    = iSrcUOffset;

	pOutBuff[0] = ppxliRenderThisY +  uiInputYOffset;
	if (ppxliRenderThisU) {
		 pOutBuff[1] = ppxliRenderThisU;
	} 
	else {
		 pOutBuff[1] = ppxliRenderThisY +  uiInputUOffset;
	}

	if (ppxliRenderThisV) {
		 pOutBuff[2] = ppxliRenderThisV;
	} 
	else {
		 pOutBuff[2] = ppxliRenderThisY +  uiInputVOffset;
	}

	pOutBuff[0] += iOffsetYSrc  + iSrcYOffset;
	pOutBuff[1] += iOffsetUVSrc + iSrcUOffset;
	pOutBuff[2] += iOffsetUVSrc + iSrcVOffset;
    if(pWMVDec->m_bCodecIsWVC1){
		pWMVDec->lastY = pWMVDec->m_ppxliCurrQY + iSrcYOffset;
		pWMVDec->lastU = pWMVDec->m_ppxliCurrQU + iSrcUOffset;
		pWMVDec->lastV = pWMVDec->m_ppxliCurrQV + iSrcVOffset;
    }
	pStride[0]   = iFullSrcWidth;
	pStride[1]   = iSrcUVStride;
	pStride[2]   = iSrcUVStride;

	pWMVDec->m_iRefBuffOffsetForCheck = (U32_WMV)pOutBuff[0] - (U32_WMV)pWMVDec->m_pvopcRenderThis->m_pucYPlane;
	pWMVDec->m_iRefBuffYStrideForCheck = iFullSrcWidth;


    if (!(pWMVDec->m_bSeqFrameWidthOrHeightChanged == TRUE && pWMVDec->m_iNumBFrames > 0 && pWMVDec->m_t != 0)) {
        pWMVDec->m_bSeqFrameWidthOrHeightChanged = FALSE;   
    }

    pWMVDec->m_bSeqFrameWidthOrHeightChanged = FALSE;   
    pWMVDec->m_uiNumFramesLeftForOutput--;

#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO GetOutputParam 2");
#endif

		return WMV_Succeeded;
}

tWMVDecodeStatus InitParam (
							HWMVDecoder  phWMVDecoder,
							U32_WMV      uiUserData,
							U32_WMV      uiFOURCCCompressed,
							Float_WMV    fltFrameRate,
							Float_WMV    fltBitRate,
							I32_WMV      iWidthSource,
							I32_WMV      iHeightSource,
							I32_WMV      iPostFilterLevel,
							U8_WMV      *pSequenceHeader, 
							U32_WMV      uipSequenceHeaderLength,
							Bool_WMV     bHostDeinterlace
							) 
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    Bool_WMV         bInterlaceYUV411;
    U8_WMV*          ppxlPHYSADDR;
    Bool_WMV         bPHYSMEM;

	tWMVDecInternalMember *pWMVDec    = (tWMVDecInternalMember*)phWMVDecoder;
	if( NULL == pWMVDec )
		return WMV_InValidArguments;

    ppxlPHYSADDR = NULL_WMV;
    bPHYSMEM     = FALSE_WMV;
    if (bPHYSMEM) {
        pWMVDec->m_ppxlPHYSADDR = ppxlPHYSADDR;
        pWMVDec->m_ppxlCurrPHYSADDR = pWMVDec->m_ppxlPHYSADDR + sizeof (tWMVDecInternalMember);
        pWMVDec->m_bPHYSMEM = TRUE_WMV;
    } else {
        pWMVDec->m_ppxlPHYSADDR = ppxlPHYSADDR;
        pWMVDec->m_bPHYSMEM = FALSE_WMV;
    }
    
#ifdef XDM
	pWMVDec->m_pInputBitstream = (CInputBitStream_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( CInputBitStream_WMV);
#else
    pWMVDec -> m_pInputBitstream = (CInputBitStream_WMV *)wmvMalloc(pWMVDec, sizeof( CInputBitStream_WMV), DHEAP_STRUCT);
    if (!pWMVDec -> m_pInputBitstream ) {
        return WMV_BadMemory;
    }
#endif 
    DEBUG_PROFILE_FRAME_INIT;

    BS_construct(pWMVDec -> m_pInputBitstream, uiUserData);

    bInterlaceYUV411 = FALSE_WMV;

    if (uiFOURCCCompressed == FOURCC_WMV3_WMV || 
        uiFOURCCCompressed == FOURCC_wmv3_WMV ||
        uiFOURCCCompressed == FOURCC_WMVP_WMV ||
        uiFOURCCCompressed == FOURCC_wmvp_WMV) 
    {       
        if ( pSequenceHeader != NULL_WMV && uipSequenceHeaderLength > 0) {
            pWMVDec->m_bYUV411 =  ((*pSequenceHeader & 0x20) == 0x20);
            pWMVDec->m_bSpriteMode = ((*pSequenceHeader & 0x10) == 0x10);
#ifdef WMV9_SIMPLE_ONLY
            if(pWMVDec->m_bYUV411 || pWMVDec->m_bSpriteMode)
                return WMV_UnSupportedCompressedFormat;
#endif
        }
    }

    pWMVDec->m_pSequenceHeader = pSequenceHeader;
    pWMVDec->m_uipSequenceHeaderLength = uipSequenceHeaderLength;
    pWMVDec->m_bHostDeinterlace = bHostDeinterlace;


    pWMVDec->m_pInputBitstream->m_pWMVDec = pWMVDec;
    
    tWMVStatus = WMVInternalMemberInit (
        pWMVDec, 
        uiUserData,
        uiFOURCCCompressed,
        fltFrameRate,
        fltBitRate,
        iWidthSource,
        iHeightSource,
        iPostFilterLevel,
        WMV_DispRotate0
        );

    if ( tWMVStatus == WMV_Succeeded ) {
        INIT_ADAPT_POST_LEVEL(pWMVDec);
    }
    
    return tWMVStatus;
}


Bool_WMV IsWVC1HeaderAndProcess(tWMVDecInternalMember *pWMVDec, VO_CODECBUFFER *pInput)
{
	I32_WMV iW,iH;
	U32_WMV iBuffCount = 0;
	U32_WMV uStartCode;
	U8_WMV  *pExt;
	U32_WMV iExtSize;

	//FILE *fp = NULL;
	//fp = fopen("d:\\test\\smoothStreaming_sequence_header_dump_qvga.rcv", "wb");

#if 0
	if(pInput->Buffer[iBuffCount++] != 0x25) {
		return FALSE;
	}
#else
	iBuffCount ++; //jump the first byte.
#endif
	uStartCode = pInput->Buffer[iBuffCount++];
	uStartCode = (uStartCode << 8) | pInput->Buffer[iBuffCount++];
	uStartCode = (uStartCode << 8) | pInput->Buffer[iBuffCount++];
	uStartCode = (uStartCode << 8) | pInput->Buffer[iBuffCount++];
	if(uStartCode != 0x0000010F) {
		return FALSE;
	}
	iBuffCount += 2;//jump
	iW = pInput->Buffer[iBuffCount++];
	iW = (iW << 8) | pInput->Buffer[iBuffCount];
	iW >>= 4;
	iH = pInput->Buffer[iBuffCount++];
	iH <<= (32-4);
	iH >>= (32-4);
	iH = (iH << 8) | pInput->Buffer[iBuffCount++];
	iW = iW*2 + 2;
	iH = iH*2 + 2;

	iExtSize = pInput->Length ;
	pExt     = (U8_WMV *)pWMVDec->m_pBMPINFOHEADER + sizeof(VO_BITMAPINFOHEADER);
	pWMVDec->m_pBMPINFOHEADER->biWidth  = iW;
	pWMVDec->m_pBMPINFOHEADER->biHeight = iH;
	pWMVDec->m_pBMPINFOHEADER->biCompression = FOURCC_WVC1_WMV;
	pWMVDec->m_pBMPINFOHEADER->biSize = sizeof(VO_BITMAPINFOHEADER) + iExtSize;

	//fwrite(pInput->Buffer, iExtSize, 1, fp);
	//fclose(fp);

	for ( iBuffCount = 0; iBuffCount < iExtSize; iBuffCount ++ ) {
		*pExt++ = pInput->Buffer[iBuffCount];
	}	

#ifdef DEBUG_STREAMING_2
	gWidth  = iW;
	gHeight = iH;
	gSwitchResolutionFlag = 1;
	{
		FILE *fp = NULL;
		FILE *fpCfg = NULL;

		fpCfg = fopen("d:\\test\\streaming_cfg.txt", "rb");
		if(fpCfg){
			fread(&gFileCount,4,1,fpCfg);
			fclose(fpCfg);	
		}

		gFileCount ++;
		fp = fopen("d:\\test\\log.txt", "a+");
		if(fp){
			fprintf(fp, "switch ID: %d, width: %d, height: %d\n", gFileCount, gWidth, gHeight);
			fclose(fp);	
		}

		fpCfg = fopen("d:\\test\\streaming_cfg.txt", "wb");
		if(fpCfg){
			fwrite(&gFileCount,4,1,fpCfg);          
			fclose(fpCfg);	
		}
	}
#endif
	return TRUE;
}

int dec_header ( HWMVDecoder hCodec, VO_CODECBUFFER *pInData )
{
    tWMVDecodeStatus       tWMVStatus  = WMV_Succeeded;
	tWMVDecInternalMember *pWMVDec     = (tWMVDecInternalMember*)hCodec;
	I32_WMV                iCodecVer;
	int                    iHdrSize;
	U8_WMV                *pHdr        = NULL;
	Bool_WMV               bHostDeinterlace;
	VO_BITMAPINFOHEADER   *pHeader = NULL;

	if( pWMVDec == NULL ) {
		return -1;
	}

	if( TRUE == IsWVC1HeaderAndProcess(pWMVDec,pInData) ){
		pHeader = pWMVDec->m_pBMPINFOHEADER;
	}
	else {
		pHeader = (VO_BITMAPINFOHEADER *)pInData->Buffer;
	}

	iCodecVer = pHeader->biCompression;

	pWMVDec->m_bCodecIsWVC1 = FALSE;
	if (iCodecVer == FOURCC_WVC1_WMV || iCodecVer == FOURCC_wvc1_WMV) {
		iCodecVer = FOURCC_WMVA_WMV;
		pWMVDec->m_bCodecIsWVC1 = TRUE;
	}

	if (iCodecVer == FOURCC_WMV3_WMV || 
		iCodecVer == FOURCC_wmv3_WMV || 
		iCodecVer == FOURCC_WMVA_WMV || 
		iCodecVer == FOURCC_wmva_WMV || 
		iCodecVer == FOURCC_WMVP_WMV || 
		iCodecVer == FOURCC_wmvp_WMV || 
		iCodecVer == FOURCC_WVP2_WMV || 
		iCodecVer == FOURCC_wvp2_WMV ) {
		pHdr     = (U8_WMV *)pHeader + sizeof(VO_BITMAPINFOHEADER);
		iHdrSize = pHeader->biSize - sizeof(VO_BITMAPINFOHEADER);
		bHostDeinterlace = TRUE_WMV;
	} else {
		pHdr     = NULL;
		iHdrSize = 0;
		bHostDeinterlace = FALSE_WMV;
	}

#ifdef DEBUG_INPUT_DATA
	{
		VOLOGI("\n VO_LOG_INFO iCodecVer:0x%x",iCodecVer);
		VOLOGI("\n VO_LOG_INFO biWidth:0x%x",pHeader->biWidth);
		VOLOGI("\n VO_LOG_INFO biHeight:0x%x",pHeader->biHeight);
		VOLOGI("\n VO_LOG_INFO iHdrSize:0x%x",iHdrSize);
	}
#endif

#ifdef DUMP_INPUT_DATA
	{
		pWMVDec->m_fpDumpData = fopen(pWMVDec->fileName, "a+");
		if(!pWMVDec->m_fpDumpData)
			return VO_ERR_FAILED;

		fwrite(&iCodecVer, 4, 1, pWMVDec->m_fpDumpData);	
		if(iHdrSize){
			fwrite(&iHdrSize, 4, 1, pWMVDec->m_fpDumpData);	
			fwrite(pHdr, iHdrSize, 1, pWMVDec->m_fpDumpData);	
		}
		fwrite(&pHeader->biHeight, 4, 1, pWMVDec->m_fpDumpData);	
		fwrite(&pHeader->biWidth, 4, 1, pWMVDec->m_fpDumpData);	

		fclose(pWMVDec->m_fpDumpData);
	}

#endif

	tWMVStatus = InitParam( pWMVDec,
							0, 
							iCodecVer, 
							0.0F,
							0.0F,
							pHeader->biWidth, 
							pHeader->biHeight,
							0,
							pHdr, 
							iHdrSize,
							bHostDeinterlace 
							);

	if ( tWMVStatus != WMV_Succeeded ) {
		return -1;
	}

    if (iCodecVer == FOURCC_MP4S_WMV || 
        iCodecVer == FOURCC_mp4s_WMV || 
        iCodecVer == FOURCC_WMV2_WMV || 
        iCodecVer == FOURCC_wmv2_WMV ||
        iCodecVer == FOURCC_WMVA_WMV || 
        iCodecVer == FOURCC_wmva_WMV ||
		iCodecVer == FOURCC_WMVP_WMV || 
		iCodecVer == FOURCC_wmvp_WMV || 
        iCodecVer == FOURCC_WVP2_WMV || 
        iCodecVer == FOURCC_wvp2_WMV ||
        iCodecVer == FOURCC_WMV3_WMV || 
        iCodecVer == FOURCC_wmv3_WMV)   {

		pHdr     = (U8_WMV *)pHeader + sizeof(VO_BITMAPINFOHEADER);
		iHdrSize = pHeader->biSize - sizeof(VO_BITMAPINFOHEADER);

		V9ECBReset( pHdr, iHdrSize );
		tWMVStatus = WMVideoDecDecodeSequenceHeader ( (HWMVDecoder *)&pWMVDec );
		if ( tWMVStatus != WMV_Succeeded) {
			return -1;
		}
	}

	return 0;
}

//#define DEBUG_STREAMING
//#define DEBUG_STREAMING_2

#if defined(DEBUG_STREAMING) || defined(DEBUG_STREAMING_2)
int gFrameCount = 0;
int gFileCount = 0;
int gWidth = 0;
int gHeight = 0;
int	gSwitchResolutionFlag = 0;
#endif


#ifdef RVDS
int dec_frame ( 
			   HWMVDecoder          hCodec, 
			   unsigned char       *pInputBuff, 
			   int                  lInBuffLen, 
			   VO_VIDEO_BUFFER     *pOutput, 
			   VO_VIDEO_OUTPUTINFO *pOutPutInfo 
			   )
{
    tWMVDecodeStatus       tWMVStatus  = WMV_Succeeded;
	tWMVDecInternalMember *pWMVDec     = (tWMVDecInternalMember*)hCodec;
	unsigned char         *pOutBuff[3] = {NULL};
	int                    stride[3]   = {0};
	unsigned char   	  *pInBuff	   = pInputBuff;
	I32_WMV				   iPicHorizSize, iPicVertSize;
	U16_WMV				   uiNumDecodedFrames;
#else
int dec_frame ( 
			   HWMVDecoder hCodec, 
			   U8_WMV *pInputBuff, 
			   int lInBuffLen, 
			   VO_VIDEO_BUFFER *pOutput, 
			   VO_VIDEO_OUTPUTINFO *pOutPutInfo 
			   )
{
    tWMVDecodeStatus       tWMVStatus  = WMV_Succeeded;
	tWMVDecInternalMember *pWMVDec     = (tWMVDecInternalMember*)hCodec;
	U8_WMV                 *pOutBuff[3] = {NULL};
	I32_WMV                 stride[3]   = {0};
	U8_WMV				  *pInBuff	   = pInputBuff;
	I32_WMV				   iPicHorizSize, iPicVertSize;
	U16_WMV				   uiNumDecodedFrames;
#endif
	
    if ( pWMVDec->m_bSeqFrameWidthOrHeightChanged == TRUE && 
		 pWMVDec->m_iNumBFrames > 0                       && 
		 pWMVDec->m_t > 1 
	   ) {
        iPicHorizSize = pWMVDec->m_iFrmWidthSrcOld;
        iPicVertSize  = pWMVDec->m_iFrmHeightSrcOld;
    }
    else {
        iPicHorizSize = pWMVDec->m_iFrmWidthSrc;
        iPicVertSize  = pWMVDec->m_iFrmHeightSrc;
    }



#ifdef CHKLCS_ON
	tWMVStatus = voCheckLibResetVideo (pWMVDec->m_phCheck, &pWMVDec->m_pPrevOutputBuff);
#endif

	V9ECBReset( pInBuff, (int)lInBuffLen );

#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO dec_frame 1");
#endif

	tWMVStatus = WMVideoDecDecodeData ( pWMVDec, &uiNumDecodedFrames );

#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO WMVideoDecDecodeData 1");
#endif
	pOutPutInfo->InputUsed = lInBuffLen;
	if ( tWMVStatus != WMV_Succeeded ) {
		pOutPutInfo->Format.Type = VO_VIDEO_FRAME_NULL;
		return 0;
	}

    if ( WMVideoIsSpriteMode (pWMVDec) ) { 
        iPicHorizSize = pWMVDec->m_iSpriteWidthDsiplay;
        iPicVertSize  = pWMVDec->m_iSpriteHeightDisplay;
    }

#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO dec_frame 2");
#endif

	tWMVStatus = GetOutputParam( pWMVDec, pOutBuff, stride );

#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO GetOutputParam 1");
#endif

	if( pWMVDec->m_outputMode == 0 && pWMVDec->m_frameNum == 0 && pWMVDec->m_iNumBFrames > 0) { //Delay first frame output.
		pOutput->Buffer[0]  = NULL;
		pOutput->Buffer[1]  = NULL;
		pOutput->Buffer[2]  = NULL;
	}
	else {
		pOutput->Buffer[0]  = pOutBuff[0];
		pOutput->Buffer[1]  = pOutBuff[1];
		pOutput->Buffer[2]  = pOutBuff[2];
	}
	pOutput->Stride[0]  = stride[0]; 
	pOutput->Stride[1]  = stride[1]; 
	pOutput->Stride[2]  = stride[2];
	pOutput->ColorType  = VO_COLOR_YUV_PLANAR420;
	pOutput->Time       = pWMVDec->m_pvopcRenderThis->m_timeStamp;
	pOutPutInfo->Format.Width    = iPicHorizSize;
	pOutPutInfo->Format.Height   = iPicVertSize; 
	pOutPutInfo->Flag            = 0;					//Input buffer is empty.No more output.
    pOutput->UserData = pWMVDec->m_pvopcRenderThis->m_UserData;

	switch(pWMVDec->m_pvopcRenderThis->m_frameType)
	{
	case	IVOP:
	case	PVOP:
	case	BVOP:
		pOutPutInfo->Format.Type = pWMVDec->m_pvopcRenderThis->m_frameType;
		break;
#ifdef _WMV9AP_
	case	BIVOP:
		pOutPutInfo->Format.Type = 0; //VO_VIDEO_FRAME_I
		break;
	case	SKIPFRAME:
		pOutPutInfo->Format.Type = 3; //VO_VIDEO_FRAME_S
		break;
#endif
	case	SPRITE:	//??
	default:
		pOutPutInfo->Format.Type = 1; //VO_VIDEO_FRAME_P
	}

#if 0 //zou
		pOutPutInfo->Format.Type = pWMVDec->m_pvopcRenderThis->m_frameType; // I,P,B,
		if(pOutPutInfo->Format.Type ==5 && pWMVDec->m_tFrmType == 5) //zzzzzzzzzzzzzh
			//pOutPutInfo->Format.Type = pWMVDec->m_tFrmTypePriorFrame; 
			pOutPutInfo->Format.Type =pWMVDec->m_PredTypePrev;
#endif

#ifndef VIDEO_MEM_SHARE_tmpVersion 
#ifdef VIDEO_MEM_SHARE
	if(pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP)
		buf_seq_ctl(&pWMVDec->m_memManageFIFO, pWMVDec->m_pvopcRenderThis, FIFO_WRITE);
#endif
#endif

	/* Convert YUV411 to YUV420. */
#ifndef VIDEO_MEM_SHARE_tmpVersion 
#ifdef VIDEO_MEM_SHARE
	if(pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP)
		if (pWMVDec->m_bYUV411) {
			pWMVDec->m_pfrmYUV411toYUV420Q = buf_seq_ctl(&pWMVDec->m_memManageFIFO, NULL, FIFO_READ);
			pOutput->Buffer[0]  = pWMVDec->m_pfrmYUV411toYUV420Q->m_pucYPlane;
			pOutput->Buffer[1]  = pWMVDec->m_pfrmYUV411toYUV420Q->m_pucUPlane;
			pOutput->Buffer[2]  = pWMVDec->m_pfrmYUV411toYUV420Q->m_pucVPlane;
			pOutput->Stride[0]  = iPicHorizSize; 
			pOutput->Stride[1]  = iPicHorizSize>>1; 
			pOutput->Stride[2]  = iPicHorizSize>>1;
			YUV411_To_YUV420(pOutBuff,pOutput->Buffer,stride,iPicHorizSize,iPicVertSize);
			buf_seq_ctl(&pWMVDec->m_memManageFIFO, pWMVDec->m_pfrmYUV411toYUV420Q, FIFO_WRITE);
		}
	else 
#endif
#endif
	{
		if (pWMVDec->m_bYUV411) {
			pOutput->Buffer[0]  = pWMVDec->m_pfrmYUV411toYUV420Q->m_pucYPlane;
			pOutput->Buffer[1]  = pWMVDec->m_pfrmYUV411toYUV420Q->m_pucUPlane;
			pOutput->Buffer[2]  = pWMVDec->m_pfrmYUV411toYUV420Q->m_pucVPlane;
			pOutput->Stride[0]  = iPicHorizSize; 
			pOutput->Stride[1]  = iPicHorizSize>>1; 
			pOutput->Stride[2]  = iPicHorizSize>>1;
			YUV411_To_YUV420(pOutBuff,pOutput->Buffer,stride,iPicHorizSize,iPicVertSize);
		}
	}

#ifdef CHKLCS_ON
	//pWMVDec->m_pPrevOutputBuff = pOutput;
    pWMVDec->m_pPrevOutputBuff.Buffer[0] = pOutput->Buffer[0];
    pWMVDec->m_pPrevOutputBuff.Buffer[1] = pOutput->Buffer[1];
    pWMVDec->m_pPrevOutputBuff.Buffer[2] = pOutput->Buffer[2];
    pWMVDec->m_pPrevOutputBuff.Stride[0] = pOutput->Stride[0]; 
    pWMVDec->m_pPrevOutputBuff.Stride[1] = pOutput->Stride[1];
    pWMVDec->m_pPrevOutputBuff.Stride[2] = pOutput->Stride[2];
    pWMVDec->m_pPrevOutputBuff.ColorType = pOutput->ColorType;
    pWMVDec->m_pPrevOutputBuff.Time = pOutput->Time;
	voCheckLibCheckVideo (pWMVDec->m_phCheck, pOutput, &pOutPutInfo->Format);
#endif

	return lInBuffLen;
}

tWMVDecodeStatus WMVDecGetFrameType(tWMVDecInternalMember *pWMVDec, VO_CODECBUFFER*	pInput)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
	U32_WMV uiBufferLengthFromCaller = 0;
	U8_WMV* pucBufferBits = NULL;
	Bool_WMV bNotEndOfFrame = TRUE;   

	V9ECBReset( pInput->Buffer, (int)pInput->Length );

#ifndef WMV9_SIMPLE_ONLY
    if (WMVideoIsSpriteMode (pWMVDec)) { //Sprite Mode

		WMVDecCBGetData (pWMVDec->m_uiUserData, 0, (U8_WMV**) &pucBufferBits, 4, &uiBufferLengthFromCaller, &bNotEndOfFrame);
		if (bNotEndOfFrame){
			if (uiBufferLengthFromCaller == 0 || pucBufferBits == NULL){
				FUNCTION_PROFILE_STOP(&fp);
				return WMV_BrokenFrame;
			}
		}

#ifndef _WMV9AP_
		BS_reset (pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, bNotEndOfFrame);
#else
		if (pWMVDec->m_cvCodecVersion == WMVA) {
			ParseStartCode(pWMVDec, pucBufferBits, uiBufferLengthFromCaller, &pucBufferBits, &uiBufferLengthFromCaller, &bNotEndOfFrame);
			//BS_reset (pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
		};

		assert(!(uiBufferLengthFromCaller == 0 && bNotEndOfFrame  == FALSE));

		BS_reset (pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
#endif //_WMV9AP_

		DEBUG_SHOW_FRAME_INPUT(pWMVDec,uiBufferLengthFromCaller, pucBufferBits)
	    
		pWMVDec->m_tFrmPredType    = (tFrameType_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, NUMBITS_VOP_PRED_TYPE_WMV2);
		pWMVDec->m_iNumMotionVectorSets = BS_getBits(pWMVDec->m_pbitstrmIn, 1) + 1;
	    
		if (pWMVDec->m_tFrmPredType == IVOP) {
	        
#ifdef _WMV9AP_
			if (pWMVDec->m_cvCodecVersion != WMVA) 
			{
				tWMVStatus = WMVideoDecDecodeFrameHead2 (pWMVDec);
				if (tWMVStatus != WMV_Succeeded) {
					return tWMVStatus;
				}
			}
			else {
				 DecodeVOPType_WMVA2 (pWMVDec);

				/* Not Skip Frame, if DO, need find out how to deal with such situation. */
				assert(pWMVDec->m_tFrmType != SKIPFRAME);
			}
#else //_WMV9AP_
			{
				tWMVDecodeStatus tWMVStatus = WMVideoDecDecodeFrameHead2 (pWMVDec);
				if (tWMVStatus != WMV_Succeeded) {
					FUNCTION_PROFILE_STOP(&fp);
					return tWMVStatus;
				}
			}
#endif //_WMV9AP_
		} 
		else {
			// Not a IVOP, so just an interpolated frame
#if 1	/* wshao 2011.3.24.
		 * APP will drop BVOP sometimes,for example,when seeking. 
		 * So we can't mask all non-IVOP as BVOP for WMVP/WVP2 format stream(it is picture sequence,not real video),
		 * because these non-IVOP are not real BVOP which can't be dropped.In fact they play PVOP role.
		 */
			pWMVDec->m_tFrmType = PVOP;
#else
			pWMVDec->m_tFrmType = BVOP;        
#endif
		}
    } 

	else 
#endif //WMV9_SIMPLE_ONLY

	{ // not Sprite Mode
    
		WMVDecCBGetData (pWMVDec->m_uiUserData, 0, (U8_WMV**) &pucBufferBits, 4, &uiBufferLengthFromCaller, &bNotEndOfFrame);
		if (bNotEndOfFrame) {
			if (uiBufferLengthFromCaller == 0 || pucBufferBits == NULL) {
				FUNCTION_PROFILE_STOP(&fp);
				return WMV_BrokenFrame;
			}
		}
    
#ifdef _WMV9AP_
		if (pWMVDec->m_cvCodecVersion == WMVA) {
			ParseStartCode(pWMVDec, pucBufferBits, uiBufferLengthFromCaller, &pucBufferBits, &uiBufferLengthFromCaller, &bNotEndOfFrame);
			//BS_reset (pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
		};
		BS_reset (pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
#endif
    
#ifndef _WMV9AP_
		tWMVStatus = WMVideoDecDecodeFrameHead2 (pWMVDec);
		if (tWMVStatus != WMV_Succeeded) {
			FUNCTION_PROFILE_STOP(&fp);
			return tWMVStatus;
		}
#else
		if (pWMVDec->m_cvCodecVersion != WMVA) {
			tWMVStatus = WMVideoDecDecodeFrameHead2 (pWMVDec);
			if (tWMVStatus != WMV_Succeeded) {
				return tWMVStatus;
			}
		} 
		else {       
			WMVFrameType iFrameType;

			if (pWMVDec->m_bInterlacedSource) {
			 if (0 == BS_getBits(pWMVDec->m_pbitstrmIn, 1))
				  iFrameType = PROGRESSIVE;     // Progressive
			else {
				if (0 == BS_getBits(pWMVDec->m_pbitstrmIn, 1))
						iFrameType = INTERLACEFRAME; // Frame Interlace
					else    
						iFrameType = INTERLACEFIELD; // Field interlace
				}
			}
			else 
				iFrameType = PROGRESSIVE;    // Progressive       

			pWMVDec->m_bInterlaceV2 = pWMVDec->m_bFieldMode = FALSE;
			if (iFrameType == PROGRESSIVE){// Progressive Frame
				DecodeVOPType_WMVA2 (pWMVDec);
			} 
			else if (iFrameType == INTERLACEFRAME){//Frame Interlace 
#ifdef PPCWMP
				return WMV_UnSupportedCompressedFormat;
#else
				DecodeVOPType_WMVA2 (pWMVDec);
#endif
			} 
			else {
#ifdef PPCWMP
				return WMV_UnSupportedCompressedFormat;
#else
				DecodeVOPTypeFieldPicture (pWMVDec);
				pWMVDec->m_tFrmType = pWMVDec->m_FirstFieldType;//or pWMVDec->m_SecondFieldType
#endif
			}
		}
#endif
	}

	return WMV_Succeeded;
}


#ifndef XDM
VO_U32 VO_API voWMV9DecInit(VO_HANDLE * phDec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{ 
    tWMVDecInternalMember *gData = NULL_WMV; 
	VO_MEM_OPERATOR		  *memOp;

#if defined(CHKLCS_ON)
	VO_PTR pLicHandle;
	VO_U32 err;
	VO_LIB_OPERATOR *pLibOp = NULL;
	if(pUserData) {
		if(pUserData->memflag & 0xF0)
			pLibOp = pUserData->libOperator;
	}
	err = voCheckLibInit(&pLicHandle, VO_INDEX_DEC_WMV, pUserData?pUserData->memflag:0, g_WMV_hModule, pLibOp);
	if(err != VO_ERR_NONE)
	{
		*phDec = NULL;
		if(pLicHandle)
		{
			voCheckLibUninit(pLicHandle);
			pLicHandle = NULL;
		}
		return err;
	}
#endif

#ifdef CHECK_MEM_LEAK
	WMVSetTraceFlag(1);
	prvHeapMeasureStart(MEM_LEAK_LOG_FILE);
#endif

#ifdef VO_LOG_INFO_traceFunction
	VOLOGI("\n VO_LOG_INFO voWMV9DecInit 1");
#endif

#ifdef DISABLE_MEM_OPERATOR
	pUserData = NULL; //force NULL memory operator
#endif

	memOp = NULL;
	if(pUserData) {
		if((pUserData->memflag & 0x0F) == VO_IMF_USERMEMOPERATOR) {
			memOp = (VO_MEM_OPERATOR*)pUserData->memData;
		}
	}

    gData = (tWMVDecInternalMember*) wmvMallocHandle (memOp, sizeof (tWMVDecInternalMember), DHEAP_STRUCT);
    
    if (gData == NULL_WMV) {
        *phDec = NULL_WMV;  
        return VO_ERR_INVALID_ARG;
    }
    memset( gData, 0, sizeof (tWMVDecInternalMember));  // clear here simplies error handling in low memory case
    *phDec = (VO_HANDLE) gData;    // if an error occurs from here on, return this pointer so we can free what might have been malloced already

	memset( &gData->m_MemOperator, 0, sizeof(MemOpteratorInternal) );
	gData->m_bIsHeadDataParsed = FALSE_WMV;
	if(pUserData) 
		gData->m_MemOperator.Type = NORMAL_MEM_OP;
	else
		gData->m_MemOperator.Type = NONE_MEM_OP;

#ifdef WIN32
	memset( gData->m_SeqHeadInfoString, 0, 2048 );
#endif

	gData->m_memOp = memOp ;
	gData->m_pBMPINFOHEADER = (VO_BITMAPINFOHEADER*) wmvMalloc (gData, sizeof (VO_BITMAPINFOHEADER)+200, DHEAP_STRUCT);

#ifdef CHKLCS_ON
	gData->m_phCheck = pLicHandle;
#endif


#ifdef DUMP_INPUT_DATA
	strcpy(gData->fileName,DUMP_INPUT_DATA_FILE_NAME );
	gData->m_fpDumpData = fopen(gData->fileName, "wb");
	if(!gData->m_fpDumpData)
		return VO_ERR_FAILED;
	fclose(gData->m_fpDumpData);
#endif

#ifdef VO_LOG_INFO_traceFunction
	VOLOGI("\n VO_LOG_INFO voWMV9DecInit 2");
#endif

	gData->m_frameNum = 0;
	return VO_ERR_NONE;
}
#endif


VO_U32 VO_API voWMV9DecSetParameter(VO_HANDLE hDec, VO_S32 nID, VO_PTR lValue)
{
	I32_WMV iInDataLen;
	tWMVDecInternalMember *gData = (tWMVDecInternalMember*)hDec;

#ifdef VO_LOG_INFO_traceFunction
	VOLOGI("\n VO_LOG_INFO voWMV9DecSetParameter 1");
#endif

	if( gData == NULL )
		return VO_ERR_INVALID_ARG;


	switch( nID ) {
	case VO_PID_COMMON_HEADDATA:
		{
			if( !gData->m_bIsHeadDataParsed ) {
				VO_CODECBUFFER *pInData = (VO_CODECBUFFER *)lValue;
				iInDataLen = dec_header( gData, pInData );
				gData->m_bIsHeadDataParsed = TRUE_WMV;
				if ( -1 == iInDataLen ) {
#ifdef VO_LOG_INFO_traceFunction
					VOLOGI("\n VO_LOG_INFO voWMV9DecSetParameter error 1");
#endif
					return VO_ERR_FAILED;
				}
			}
		}
		break;
#ifdef VIDEO_MEM_SHARE
	case VO_PID_VIDEO_VIDEOMEMOP:
		{
			VO_MEM_VIDEO_OPERATOR *pMemShare     = (VO_MEM_VIDEO_OPERATOR *)lValue;
			gData->m_MemOperator.Type			 = VIDEO_MEM_SHARE_OP;
			gData->m_videoMemShare.GetBufByIndex = pMemShare->GetBufByIndex;
			gData->m_videoMemShare.Init          = pMemShare->Init;
			gData->m_videoMemShare.Uninit        = pMemShare->Uninit;
		}
		break;
#endif
	case VO_PID_COMMON_FLUSH:
#ifdef VIDEO_MEM_SHARE
		{
			//if(gData->m_MemOperator.Type == VIDEO_MEM_SHARE_OP)
				reset_buf_seq(gData);

            gData->m_uiNumFramesLeftForOutput = 0;
            gData->m_frameNum = 0;
		}
#endif
		break;
	case VO_PID_VIDEO_OUTPUTMODE:
		{
			gData->m_outputMode = *(U32_WMV*)lValue; //0 or 1
			break;
		}

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

#ifdef VO_LOG_INFO_traceFunction
	VOLOGI("\n VO_LOG_INFO voWMV9DecSetParameter 2");
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voWMV9DecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{
	tWMVDecInternalMember *gData = (tWMVDecInternalMember*)hDec;

#ifdef VO_LOG_INFO_traceFunction
	VOLOGI("\n VO_LOG_INFO voWMV9DecSetInputData 1");
#endif

	if( NULL == gData )
		return VO_ERR_INVALID_ARG;

	gData->m_pInputFrameData = (void *)pInput;
	gData->m_InBuffLenLeft   = pInput->Length;

#ifdef VO_LOG_INFO_traceFunction
	VOLOGI("\n VO_LOG_INFO voWMV9DecSetInputData 2");
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voWMV9DecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutPutInfo)
{
	int						iInDataLen;
	VO_CODECBUFFER		   *pInput;
	tWMVDecInternalMember*  gData = (tWMVDecInternalMember*)hDec;

#ifdef VO_LOG_INFO_traceFunction
	VOLOGI("\n VO_LOG_INFO voWMV9DecGetOutputData 1,frame:%d",gData->m_frameNum);
#endif

#ifdef _DEBUG
	if(gData->m_frameNum == 105)
		gData->m_frameNum += 0;

//	printf("\nframe %d",gData->m_frameNum);
#endif

	if( NULL == gData )
		return VO_ERR_INVALID_ARG;

	pInput = (VO_CODECBUFFER *)gData->m_pInputFrameData;

#ifdef TIME_LIMIT
	if(gData->m_frameNum > MAX_TIME) {
		return VO_ERR_NOT_IMPLEMENT;
	}
#endif

	if ( gData->m_InBuffLenLeft <= 0 ) {
#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO voWMV9DecGetOutputData error 1,frame:%d",gData->m_frameNum);
#endif
		return VO_ERR_INPUT_BUFFER_SMALL;
	}

	/* 
	 * If it is VC1 sequence header, don't process it. 
	 * Smooth streaming will deliver the sequnce header again when calling first voWMV9DecGetOutputData(),so don't process it.
	 */
	if( gData->m_bCodecIsWVC1 ) {
		if( pInput->Buffer[1] == 0x00 && pInput->Buffer[2] == 0x00 && pInput->Buffer[3] == 0x01 && pInput->Buffer[4] == 0x0F )
			return VO_ERR_INPUT_BUFFER_SMALL;
	}

#ifdef DEBUG_INPUT_DATA
	{
		VOLOGI("\n VO_LOG_INFO frame:%d, Length:0x%x",gData->m_frameNum, pInput->Length);
		VOLOGI("\n VO_LOG_INFO start 8 bytes:0x%x 0x%x 0x%x 0x%x ",pInput->Buffer[0],pInput->Buffer[1],pInput->Buffer[2],pInput->Buffer[3]);
		VOLOGI("0x%x 0x%x 0x%x 0x%x",pInput->Buffer[4],pInput->Buffer[5],pInput->Buffer[6],pInput->Buffer[7]);
	}
#endif

#ifdef DUMP_INPUT_DATA
	{
		int m = 0;
		gData->m_fpDumpData = fopen(gData->fileName, "ab+");
		if(!gData->m_fpDumpData)
			return VO_ERR_FAILED;

		fwrite(&pInput->Length, 4, 1, gData->m_fpDumpData);	
		fwrite(&m, 4, 1, gData->m_fpDumpData);	
		fwrite(pInput->Buffer, pInput->Length, 1, gData->m_fpDumpData);	

		fclose(gData->m_fpDumpData);
	}

#endif

	gData->m_inTimeStamp = pInput->Time;
    gData->m_inUserData = pInput->UserData;
	iInDataLen = dec_frame ( gData, pInput->Buffer, pInput->Length, pOutput, pOutPutInfo );
	gData->m_InBuffLenLeft -= pInput->Length;

	gData->m_frameNum ++;

	if ( 0 == iInDataLen ) {
#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO voWMV9DecGetOutputData error 2,frame:%d",gData->m_frameNum);
#endif
		return VO_ERR_FAILED;
	}

#ifdef VO_LOG_INFO_traceFunction
	VOLOGI("\n VO_LOG_INFO voWMV9DecGetOutputData 2");
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voWMV9DecGetParameter(VO_HANDLE hDec, VO_S32 nID, VO_PTR plValue)
{
	tWMVDecInternalMember*  gData  = (tWMVDecInternalMember*)hDec;
	VO_CODECBUFFER*			pInput = (VO_CODECBUFFER*)plValue;

#ifdef VO_LOG_INFO_traceFunction
	VOLOGI("\n VO_LOG_INFO voWMV9DecGetParameter 1");
#endif

	if( NULL == gData )
		return VO_ERR_INVALID_ARG;

	switch( nID ) {
	case VO_PID_VIDEO_FRAMETYPE:
		{
			tFrameType_WMV FrmTypeBackup  = gData->m_tFrmType;
			if( WMV_Succeeded != WMVDecGetFrameType(gData, pInput) ) {
#ifdef VO_LOG_INFO_traceFunction
	VOLOGI("\n VO_LOG_INFO voWMV9DecGetParameter error 1,frame:%d",gData->m_frameNum);
#endif
				return VO_ERR_FAILED;
			}

            if(gData->m_tFrmType == BIVOP)
                gData->m_tFrmType = BVOP;
			if(gData->m_tFrmType > BVOP) {//APP just need to know if it is B frame,other frame type is not care.
				pInput->Time = (int)IVOP;
			}
			else {
				pInput->Time = (int)gData->m_tFrmType; // I,P,B,
			}
			
			gData->m_tFrmType = FrmTypeBackup;
		}
		break;
#ifdef WIN32_DEG
	case VO_PID_COMMON_HeadInfo:
		{
			VO_HEAD_INFO *pHeadInfo = (VO_HEAD_INFO *)plValue;
			if( !gData->m_bIsHeadDataParsed ) {
				sprintf(pHeadInfo->Description, "\n*** Error: It has not set VO_PID_COMMON_HEADDATA.Must set it before getting head info. ***");
				pHeadInfo->Size = strlen(pHeadInfo->Description);
				return VO_ERR_FAILED;
			}
			else {
				strcpy(pHeadInfo->Description, gData->m_SeqHeadInfoString);
				pHeadInfo->Size = strlen(gData->m_SeqHeadInfoString);
			}
		}
		break;
#endif
	case VO_PID_DEC_WMV_GET_LASTVIDEOINFO:
		{
			VO_VIDEO_OUTPUTINFO *pOutPutInfo = (VO_VIDEO_OUTPUTINFO *)plValue;
			if ( gData->m_bSeqFrameWidthOrHeightChanged == TRUE && 
				 gData->m_iNumBFrames > 0                       && 
				 gData->m_t > 1 
			   ) {
				pOutPutInfo->Format.Width  = gData->m_iFrmWidthSrcOld;
				pOutPutInfo->Format.Height = gData->m_iFrmHeightSrcOld;
			}
			else {
				pOutPutInfo->Format.Width  = gData->m_iFrmWidthSrc;
				pOutPutInfo->Format.Height = gData->m_iFrmHeightSrc;
			}
		}
		break;
	case VO_PID_DEC_WMV_GET_LASTVIDEOBUFFER:
		{
			VO_VIDEO_BUFFER	*pOutBuf = (VO_VIDEO_BUFFER *)plValue;
			U8_WMV                 *pOutBuff[3] = {NULL};
			I32_WMV                 stride[3]   = {0};

			if(gData->m_iNumBFrames > 0){
				gData->m_uiNumFramesLeftForOutput = 1;
				GetOutputParam( gData, pOutBuff, stride );
				pOutBuf->Stride[0] = stride[0];
				pOutBuf->Stride[1] = stride[1];
				pOutBuf->Stride[2] = stride[2];
				pOutBuf->ColorType = VO_COLOR_YUV_PLANAR420;

				if(gData->m_bRenderFromPostBuffer)  
					gData->m_pvopcRenderThis = gData->m_pfrmPostPrevQ ; //gData->m_pfrmRef0Q;
				else
				{
					if(gData->m_tFrmType == BVOP)
						gData->m_pvopcRenderThis = gData->m_pfrmRef1Q;
					else
						gData->m_pvopcRenderThis = gData->m_pfrmCurrQ;
				}
				if(gData->m_pvopcRenderThis)
				{
					pOutBuf->Buffer[0] = gData->m_pvopcRenderThis->m_pucYPlane + gData->m_iWidthPrevYXExpPlusExp;
					pOutBuf->Buffer[1] = gData->m_pvopcRenderThis->m_pucUPlane + gData->m_iWidthPrevUVXExpPlusExp;
					pOutBuf->Buffer[2] = gData->m_pvopcRenderThis->m_pucVPlane + gData->m_iWidthPrevUVXExpPlusExp;
				}
				else
				{
					pOutBuf->Buffer[0] = NULL;
					pOutBuf->Buffer[1] = NULL;
					pOutBuf->Buffer[2] = NULL;
				}
			}
			else { //No last frame to output.
				pOutBuf->Buffer[0] = NULL;
				pOutBuf->Buffer[1] = NULL;
				pOutBuf->Buffer[2] = NULL;
			}
		}
		break;
    case VO_PID_VIDEO_ASPECTRATIO:
		{
			*((VO_S32*)plValue) = GetAspectRatio(gData);
		}
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

#ifdef VO_LOG_INFO_traceFunction
	VOLOGI("\n VO_LOG_INFO voWMV9DecGetParameter 2");
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voWMV9DecUninit(VO_HANDLE hDec)
{
	tWMVDecInternalMember *gData = (tWMVDecInternalMember*)hDec;

#ifdef VO_LOG_INFO_traceFunction
	VOLOGI("\n VO_LOG_INFO voWMV9DecUninit 1");
#endif

	if( NULL == gData )
		return VO_ERR_INVALID_ARG;

#ifdef CHKLCS_ON
	voCheckLibUninit (gData->m_phCheck);
#endif

	WMVideoDecClose ( gData );

#ifdef CHECK_MEM_LEAK
	prvWriteMemTraceLog();
#endif

#ifdef VO_LOG_INFO_traceFunction
	VOLOGI("\n VO_LOG_INFO voWMV9DecUninit 2");
#endif

	return VO_ERR_NONE;
}


I32_WMV YUV411_To_YUV420( U8_WMV **pSrcBuff, U8_WMV **pDstBuff, I32_WMV *iSrcStride, I32_WMV iW, I32_WMV iH )
{
	I32_WMV i,j,m,width,height2;
	U8_WMV *pSrc,*pDst;
	U8_WMV *pSrc2,*pDst2;
	I32_WMV	srcStrideUV,dstStrideUV;

	if(0 == (iW&15)) { // (iW % 16) == 0

		/* Y. Just copy */
		pSrc = pSrcBuff[0];
		pDst = pDstBuff[0];
		for(i = 0; i< iH; i++ ) {
			for(j = 0; j < iW; j += 16 ) {
				*(U32_WMV *)(pDst+j)    = *(U32_WMV *)(pSrc+j); 
				*(U32_WMV *)(pDst+j+4)  = *(U32_WMV *)(pSrc+j+4); 
				*(U32_WMV *)(pDst+j+8)  = *(U32_WMV *)(pSrc+j+8); 
				*(U32_WMV *)(pDst+j+12) = *(U32_WMV *)(pSrc+j+12); 
			}
			pSrc += iSrcStride[0];
			pDst += iW;
		}

		width   = iW>>2;
		height2 = iH>>1;
		srcStrideUV = iSrcStride[1];
		dstStrideUV = iW>>1;

		/* U,V */
		pSrc  = pSrcBuff[1];
		pDst  = pDstBuff[1];
		pSrc2 = pSrcBuff[2];
		pDst2 = pDstBuff[2];
		for(i = 0; i< height2; i++ ) {
			for(j = 0,m = 0; j< width; j += 4,m += 8  ) {
				pDst[m+0]  = pDst[m+1]  = (pSrc[j+0]  + pSrc[j+0+srcStrideUV]  + 1) >> 1;
				pDst[m+2]  = pDst[m+3]  = (pSrc[j+1]  + pSrc[j+1+srcStrideUV]  + 1) >> 1;
				pDst[m+4]  = pDst[m+5]  = (pSrc[j+2]  + pSrc[j+2+srcStrideUV]  + 1) >> 1;
				pDst[m+6]  = pDst[m+7]  = (pSrc[j+3]  + pSrc[j+3+srcStrideUV]  + 1) >> 1;
				pDst2[m+0] = pDst2[m+1] = (pSrc2[j+0] + pSrc2[j+0+srcStrideUV] + 1) >> 1;
				pDst2[m+2] = pDst2[m+3] = (pSrc2[j+1] + pSrc2[j+1+srcStrideUV] + 1) >> 1;
				pDst2[m+4] = pDst2[m+5] = (pSrc2[j+2] + pSrc2[j+2+srcStrideUV] + 1) >> 1;
				pDst2[m+6] = pDst2[m+7] = (pSrc2[j+3] + pSrc2[j+3+srcStrideUV] + 1) >> 1;
			}
			pSrc  += (srcStrideUV<<1);
			pDst  += dstStrideUV;
			pSrc2 += (srcStrideUV<<1);
			pDst2 += dstStrideUV;
		}
	}
	else { // (iW % 8) == 0

		/* Y. Just copy */
		pSrc = pSrcBuff[0];
		pDst = pDstBuff[0];
		for(i = 0; i< iH; i++ ) {
			for(j = 0; j < iW; j += 8 ) {
				*(U32_WMV *)(pDst+j)   = *(U32_WMV *)(pSrc+j); 
				*(U32_WMV *)(pDst+j+4) = *(U32_WMV *)(pSrc+j+4); 
			}
			pSrc += iSrcStride[0];
			pDst += iW;
		}

		width   = iW>>2;
		height2 = iH>>1;
		srcStrideUV = iSrcStride[1];
		dstStrideUV = iW>>1;

		/* U,V */
		pSrc  = pSrcBuff[1];
		pDst  = pDstBuff[1];
		pSrc2 = pSrcBuff[2];
		pDst2 = pDstBuff[2];
		for(i = 0; i< height2; i++ ) {
			for(j = 0,m = 0; j< width; j += 2,m += 4  ) {
				pDst[m+0]  = pDst[m+1]  = (pSrc[j+0]  + pSrc[j+0+srcStrideUV]  + 1) >> 1;
				pDst[m+2]  = pDst[m+3]  = (pSrc[j+1]  + pSrc[j+1+srcStrideUV]  + 1) >> 1;
				pDst2[m+0] = pDst2[m+1] = (pSrc2[j+0] + pSrc2[j+0+srcStrideUV] + 1) >> 1;
				pDst2[m+2] = pDst2[m+3] = (pSrc2[j+1] + pSrc2[j+1+srcStrideUV] + 1) >> 1;
			}
			pSrc  += (srcStrideUV<<1);
			pDst  += dstStrideUV;
			pSrc2 += (srcStrideUV<<1);
			pDst2 += dstStrideUV;
		}
	}

	return 0;
}


#ifdef XDM
tWMVDecodeStatus WMVideoDecInit (
    HWMVDecoder* phWMVDecoder,
    U32_WMV uiUserData,
    U32_WMV uiFOURCCCompressed,
    Float_WMV fltFrameRate,
    Float_WMV fltBitRate,
    I32_WMV iWidthSource,
    I32_WMV iHeightSource,
    I32_WMV iPostFilterLevel,
    U8_WMV  *pSequenceHeader, 
    U32_WMV uipSequenceHeaderLength,
    Bool_WMV bHostDeinterlace,
	void  *pPrivateBuff,
	void  *pFrameBuff
) 
#else
tWMVDecodeStatus WMVideoDecInit (
    HWMVDecoder* phWMVDecoder,
    U32_WMV uiUserData,
    U32_WMV uiFOURCCCompressed,
    Float_WMV fltFrameRate,
    Float_WMV fltBitRate,
    I32_WMV iWidthSource,
    I32_WMV iHeightSource,
    I32_WMV iPostFilterLevel,
    U8_WMV  *pSequenceHeader, 
    U32_WMV uipSequenceHeaderLength,
    Bool_WMV bHostDeinterlace
) 
#endif
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 
    Bool_WMV  bInterlaceYUV411;
    U8_WMV* ppxlPHYSADDR;
    Bool_WMV bPHYSMEM;

    ppxlPHYSADDR = NULL_WMV;
    bPHYSMEM = FALSE_WMV;

	pWMVDec = (tWMVDecInternalMember *)(*phWMVDecoder);
    //memset( pWMVDec, 0, sizeof (tWMVDecInternalMember)); 
	memset( &pWMVDec->m_MemOperator, 0, sizeof(MemOpteratorInternal) );
	pWMVDec->m_MemOperator.Type = NONE_MEM_OP;

	if (bPHYSMEM) {
        pWMVDec->m_ppxlPHYSADDR = ppxlPHYSADDR;
        pWMVDec->m_ppxlCurrPHYSADDR = pWMVDec->m_ppxlPHYSADDR + sizeof (tWMVDecInternalMember);
        pWMVDec->m_bPHYSMEM = TRUE_WMV;
    } else {
        pWMVDec->m_ppxlPHYSADDR = ppxlPHYSADDR;
        pWMVDec->m_bPHYSMEM = FALSE_WMV;
    }
    
    
#ifdef XDM
	pWMVDec -> m_pInputBitstream = (CInputBitStream_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( CInputBitStream_WMV);
#else
    pWMVDec -> m_pInputBitstream = (CInputBitStream_WMV *)wmvMalloc(pWMVDec, sizeof( CInputBitStream_WMV), DHEAP_STRUCT);
    if (!pWMVDec -> m_pInputBitstream ) {
        return WMV_BadMemory;
    }
#endif 
    DEBUG_PROFILE_FRAME_INIT;

    BS_construct(pWMVDec -> m_pInputBitstream, uiUserData);

    bInterlaceYUV411 = FALSE_WMV;

    if (uiFOURCCCompressed == FOURCC_WMV3_WMV || 
        uiFOURCCCompressed == FOURCC_wmv3_WMV ||
        uiFOURCCCompressed == FOURCC_WMVP_WMV ||
        uiFOURCCCompressed == FOURCC_wmvp_WMV) 
    {       
        if ( pSequenceHeader != NULL_WMV && uipSequenceHeaderLength > 0) {
            pWMVDec->m_bYUV411 =  ((*pSequenceHeader & 0x20) == 0x20);
            pWMVDec->m_bSpriteMode = ((*pSequenceHeader & 0x10) == 0x10);
#ifdef WMV9_SIMPLE_ONLY
            if(pWMVDec->m_bYUV411 || pWMVDec->m_bSpriteMode)
                return WMV_UnSupportedCompressedFormat;
#endif
        }
    }

    pWMVDec->m_pSequenceHeader = pSequenceHeader;
    pWMVDec->m_uipSequenceHeaderLength = uipSequenceHeaderLength;
    pWMVDec->m_bHostDeinterlace = bHostDeinterlace;


    pWMVDec->m_pInputBitstream->m_pWMVDec = pWMVDec;
    
    //memset (pWMVDec, 0, sizeof (tWMVDecInternalMember));
    tWMVStatus = WMVInternalMemberInit (
        pWMVDec, 
        uiUserData,
        uiFOURCCCompressed,
        fltFrameRate,
        fltBitRate,
        iWidthSource,
        iHeightSource,
        iPostFilterLevel,
        WMV_DispRotate0
        );

    if ( tWMVStatus == WMV_Succeeded ) {
        INIT_ADAPT_POST_LEVEL(pWMVDec);
    }
    
    return tWMVStatus;
}



tWMVDecodeStatus WMVideoDecClose (HWMVDecoder phWMVDecoder)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO WMVideoDecClose 1");
#endif

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;

    DEBUG_PROFILE_FRAME_OUTPUT_LOG;

    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    CLOSE_ADAPT_POST_LEVEL(pWMVDec);

    tWMVStatus = WMVInternalMemberDestroy (pWMVDec);
    COVERAGE_FREE ();


    if (pWMVDec->m_ppxlPHYSADDR) {
#ifdef _WMMX_
        VirtualFree (pWMVDec->m_ppxlPHYSADDR, PHYSMEMSIZE,MEM_RELEASE);
        if (!pWMVDec->m_bPHYSMEM) {
            memset(pWMVDec,0,sizeof(tWMVDecInternalMember));
            wmvFree (pWMVDec, pWMVDec);
        }
#endif
    }
    else {

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO WMVideoDecClose 2");
#endif

#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO pWMVDec :%x",(U32_WMV)pWMVDec );
#endif
		wmvFree (pWMVDec, pWMVDec);
    }
    pWMVDec = NULL_WMV;  
    // We have no way to clear the caller'd pointer to this structure
    // so if he calls us again with this pointer we have just free'd, we are sunk.

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO WMVideoDecClose 3");
#endif

    return tWMVStatus;
}

/* This is for WMVP/WVP2 format file. */
tWMVDecodeStatus WMVideoDecClose2 (HWMVDecoder phWMVDecoder)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO WMVideoDecClose2 1");
#endif

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;

    DEBUG_PROFILE_FRAME_OUTPUT_LOG;

    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    CLOSE_ADAPT_POST_LEVEL(pWMVDec);
    tWMVStatus = WMVInternalMemberDestroy (pWMVDec);
    COVERAGE_FREE ();

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO WMVideoDecClose2 2");
#endif

    return tWMVStatus;
}



// ASF Binding enum
#define ASFBINDING_SIZE                   1   // size of ASFBINDING is 1 byte     
#define ASFBINDING_RESERVED               1   // bit 0 - reserved
#define ASFBINDING_NO_BFRAME              2   // bit 1 - when 0, b frame might be present
#define ASFBINDING_NO_SLICECODE           4   // bit 2 - when 0, slice might be present    
#define ASFBINDING_NO_MULTIPLEENTRY       8   // bit 3 - when 0, mult. entry headers might be present    
#define ASFBINDING_NO_MULTIPLESEQ        16   // bit 4 - when 0, mult. sequence headers might be present    
#define ASFBINDING_NO_INTERLACESOURCE    32   // bit 5 - when 0, interlace seq might be present

tWMVDecodeStatus WMVideoDecDecodeSequenceHeader (HWMVDecoder* phWMVDecoder)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    U32_WMV uiBufferLengthFromCaller;
    U8_WMV* pucBufferBits = NULL;
    Bool_WMV bNotEndOfFrame = TRUE;
    tWMVDecInternalMember *pWMVDec;
    U32_WMV uiUserData; 
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMVideoDecDecodeSequenceHeader);

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (*phWMVDecoder);

    uiUserData = pWMVDec->m_uiUserData;
    WMVDecCBGetData (pWMVDec->m_uiUserData, 0, (U8_WMV**) &pucBufferBits, 4, &uiBufferLengthFromCaller, &bNotEndOfFrame);
    if (bNotEndOfFrame){
        if (uiBufferLengthFromCaller == 0 || pucBufferBits == NULL){
            return WMV_BrokenFrame;
        }
    }

    pWMVDec->m_bNoSliceCodeInSequence = FALSE_WMV;
    pWMVDec->m_bNoBFrameInSequence = FALSE_WMV;
    pWMVDec->m_bNoMultipleEntryInSequence = FALSE_WMV;          // not used yet
    pWMVDec->m_bNoMultipleSequenceHeaderInSequence = FALSE_WMV; // not used yet

    if (pWMVDec->m_cvCodecVersion == WMVA) {
        
        U8_WMV ucAsfBindingFlags = *pucBufferBits;
        if (0 == (ucAsfBindingFlags & ASFBINDING_RESERVED) ) {
            // protect pre-Cescent releases from too many changes flagged in the ASF Binding
            // but don'tenable this test quite yet as the bit was not set correctly by the encoder between 4/22 and 4/27 or so.
            // return WMV_UnSupportedCompressedFormat;
        }

        if (ucAsfBindingFlags & ASFBINDING_NO_BFRAME) {
            pWMVDec->m_bNoBFrameInSequence = TRUE_WMV; //pWMVDec->m_iNumBFrames = 0; // used as a boolean
        } else {
            pWMVDec->m_bNoBFrameInSequence = FALSE_WMV; //pWMVDec->m_iNumBFrames = 1; // used as a boolean
        }

        if (ucAsfBindingFlags & ASFBINDING_NO_SLICECODE) {
            // there is no slice code
            pWMVDec->m_bNoSliceCodeInSequence = TRUE_WMV;
        } else {
            // slice code
            pWMVDec->m_bNoSliceCodeInSequence = FALSE_WMV;
        }
        
        if (ucAsfBindingFlags & ASFBINDING_NO_INTERLACESOURCE) {
            // progressive source
            pWMVDec->m_bNoInterlaceSource = TRUE_WMV;
        } else {
            // interlace source
            pWMVDec->m_bNoInterlaceSource = FALSE_WMV;
        }

        uiBufferLengthFromCaller = uiBufferLengthFromCaller - ASFBINDING_SIZE;
        
        if (uiBufferLengthFromCaller == 0) {
            WMVDecCBGetData (pWMVDec->m_uiUserData, 0, (U8_WMV**) &pucBufferBits, 4, &uiBufferLengthFromCaller, &bNotEndOfFrame);
            BS_reset(pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, 
                uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
        } else {
            BS_reset(pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits + ASFBINDING_SIZE, 
                uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
        }
    } else {
        BS_reset(pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, 
            bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
    }
    
    pWMVDec->m_pInputBitstream->m_bNotEndOfFrame = bNotEndOfFrame;
    tWMVStatus = WMVideoDecDecodeSequenceHeaderInternal (*phWMVDecoder, pucBufferBits, uiBufferLengthFromCaller);

    if (tWMVStatus != WMV_Succeeded)
        return tWMVStatus;
#   ifdef WMV9_SIMPLE_ONLY
        if (WMVideoIsSpriteMode (*phWMVDecoder)) {
            return WMV_UnSupportedCompressedFormat;
        }
#   else //WMV9_SIMPLE_ONLY
        if (WMVideoIsSpriteMode (*phWMVDecoder)) {
            I32_WMV ispriteWidth = WMVideoSpriteWidth (*phWMVDecoder);
            I32_WMV ispriteHeight = WMVideoSpriteHeight (*phWMVDecoder);
            I32_WMV ispriteDispWidth = ((tWMVDecInternalMember*)(*phWMVDecoder))->m_iWidthSource;
            I32_WMV ispriteDispHeight = ((tWMVDecInternalMember*)(*phWMVDecoder))->m_iHeightSource;
            U8_WMV  *pSequenceHeader = ((tWMVDecInternalMember*)(*phWMVDecoder))->m_pSequenceHeader;
            U32_WMV uipSequenceHeaderLength = ((tWMVDecInternalMember*)(*phWMVDecoder))->m_uipSequenceHeaderLength;
            Bool_WMV bHostDeinterlace = ((tWMVDecInternalMember*)(*phWMVDecoder))->m_bHostDeinterlace;

            I32_WMV iFrameRate = WMVideoDecGetFrameRate(*phWMVDecoder);
            if ( iFrameRate <= 0 )
                iFrameRate = 30;

#ifdef WMVIMAGE_V2
            if (pWMVDec->m_bSpriteModeV2)
            {
#ifdef XDM
				tWMVDecInternalMember *pBase = (tWMVDecInternalMember*)(*phWMVDecoder);
                WMVideoDecClose2 (*phWMVDecoder);
                tWMVStatus  = WMVideoDecInit (&pBase, uiUserData, FOURCC_WVP2_WMV, (Float_WMV) iFrameRate, 0.0F, ispriteWidth, 
					ispriteHeight, 0, pSequenceHeader, uipSequenceHeaderLength, bHostDeinterlace,pWMVDec->m_MemXDM.pPrivateBuff,pWMVDec->m_MemXDM.pFrameBuff );
				*phWMVDecoder = pBase;
#else
                WMVideoDecClose2 (*phWMVDecoder);
                tWMVStatus  = WMVideoDecInit (phWMVDecoder, uiUserData, FOURCC_WVP2_WMV, (Float_WMV) iFrameRate, 0.0F, ispriteWidth, ispriteHeight, 0, pSequenceHeader, uipSequenceHeaderLength, bHostDeinterlace);
#endif     
            }
            else
#endif //WMVIMAGE_V2
            {
#ifdef XDM
				tWMVDecInternalMember *pBase = (tWMVDecInternalMember*)(*phWMVDecoder);
                WMVideoDecClose2 (*phWMVDecoder);
                tWMVStatus  = WMVideoDecInit (&pBase, uiUserData, FOURCC_WVP2_WMV, (Float_WMV) iFrameRate, 0.0F, ispriteWidth, 
					ispriteHeight, 0, pSequenceHeader, uipSequenceHeaderLength, bHostDeinterlace,pWMVDec->m_MemXDM.pPrivateBuff,pWMVDec->m_MemXDM.pFrameBuff);
				*phWMVDecoder = pBase;
#else
                WMVideoDecClose2 (*phWMVDecoder);
                tWMVStatus  = WMVideoDecInit (phWMVDecoder, uiUserData, FOURCC_WMV3_WMV, (Float_WMV) iFrameRate, 0.0F, ispriteWidth, ispriteHeight, 0, pSequenceHeader, uipSequenceHeaderLength, bHostDeinterlace);
#endif
            }

            if (tWMVStatus != WMV_Succeeded)
                return tWMVStatus;
            ((tWMVDecInternalMember*)(*phWMVDecoder))->m_iSpriteWidthDsiplay    =   ispriteDispWidth;              ; 
            ((tWMVDecInternalMember*)(*phWMVDecoder))->m_iSpriteHeightDisplay   =   ispriteDispHeight;   

            if (pWMVDec->m_cvCodecVersion == WMVA) {
                // remove asf binding
                //U8_WMV ucAsfBindingFlags = *pucBufferBits;
                uiBufferLengthFromCaller = uiBufferLengthFromCaller - ASFBINDING_SIZE;
                BS_reset(pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits + ASFBINDING_SIZE, 
                    uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
            } else {
                BS_reset(pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, 
                    bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
            }

            tWMVStatus = WMVideoDecDecodeSequenceHeaderInternal (*phWMVDecoder, pucBufferBits, uiBufferLengthFromCaller);
        } else {
            INIT_ADAPT_POST_LEVEL(pWMVDec);     // re-adjust adaption based on updated frame rate info 
        }
#   endif //WMV9_SIMPLE_ONLY
    return tWMVStatus;
}

tWMVDecodeStatus WMVideoDecDecodeSequenceHeaderInternal (HWMVDecoder hWMVDecoder, U8_WMV* pucBufferBits, U32_WMV uiBufferLengthFromCaller)
{

    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec;

    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);

    if (pWMVDec->m_cvCodecVersion == WMV3 || pWMVDec->m_cvCodecVersion == WMVA)
    {
        //I32_WMV iEncWidth = pWMVDec->m_iFrmWidthSrc;
        //I32_WMV iEncHeight = pWMVDec->m_iFrmHeightSrc;
        tWMVStatus = decodeVOLHead_WMV3 (pWMVDec, uiBufferLengthFromCaller, 0, 0, 0, 0);
        
        return tWMVStatus;

        /*
        if(pWMVDec->m_cvCodecVersion == WMVA)
        {
            
            if (iEncWidth != pWMVDec->m_iFrmWidthSrc || iEncHeight != pWMVDec->m_iFrmHeightSrc) {
                pWMVDec->m_bSeqFrameWidthOrHeightChanged = TRUE;
                if (iEncWidth * iEncHeight < pWMVDec->m_iFrmWidthSrc * pWMVDec->m_iFrmHeightSrc) {
                    pWMVDec->m_bLargerSeqBufferRequired = TRUE;
                }
                SwitchSequence(pWMVDec, FALSE);   //re-init only, skip volhead decoding
            }


        }
        */
    }

    if (pWMVDec->m_cvCodecVersion == WMV2)
        decodeVOLHead (pWMVDec);
#ifndef WMV789_ONLY
    else // MP4S
        decodeVOLHeadMP4S (pWMVDec);
#endif

    return tWMVStatus;
}


#ifdef _COUNT_CYCLES_
    __int64 DecodeAndLockTmr = 0;
    __int64 HrTmrGet ()
    {
    __asm rdtsc;
    }
#endif


I32_WMV WMVideoDecGetFrameRate (HWMVDecoder hWMVDecoder) {
    //tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 

    if (hWMVDecoder == NULL_WMV)
        return 0;

    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);

    return pWMVDec->m_iFrameRate;
}


#ifndef WMV9_SIMPLE_ONLY
tWMVDecodeStatus DecodeAffineCoefficients (
    HWMVDecoder hWMVDecoder,
    float* pfltA, 
    float* pfltB, 
    float* pfltC, 
    float* pfltD, 
    float* pfltE, 
    float* pfltF, 
    float* pfltFading
) {
    // bitstream pattern:
    // 00: translation only, 01: translation and same zooming factor, 10: translation and zooming, 11: generic
    unsigned long  uiA, uiB, uiC, uiD, uiE, uiF, uiFd;
    //tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 
    U32_WMV bFadingExist;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeAffineCoefficients);

    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);

    pWMVDec->m_iAffinePattern = BS_getBits(pWMVDec->m_pbitstrmIn, 2);
    if (pWMVDec->m_iAffinePattern == 0) { // translation only
        *pfltA = 1.0f; 
        *pfltB = 0.0f; 
        *pfltD = 0.0f; 
        *pfltE = 1.0f; 
        // SH3 eVC3.0 compiler got confused with the following syntax so make it easier on it.
        // uiC = (((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15) + (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        uiC  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiC +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        uiF  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiF +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        *pfltC = (float) ((float) uiC / (float) (0x8000) - (float) (0x4000));
        *pfltF = (float) ((float) uiF / (float) (0x8000) - (float) (0x4000));
    }
    else if (pWMVDec->m_iAffinePattern == 1) { // translation & same zooming factors only
        *pfltB = 0.0f; 
        *pfltD = 0.0f; 
        uiA  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiA +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        uiC  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiC +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        uiF  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiF +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        *pfltA = *pfltE = (float) uiA / (float) (0x8000) - (float) (0x4000);
        *pfltC = (float) uiC / (float) (0x8000) - (float) (0x4000);
        *pfltF = (float) uiF / (float) (0x8000) - (float) (0x4000);
    }
    else if (pWMVDec->m_iAffinePattern == 2) { // translation & zooming factors only
        *pfltB = 0.0f; 
        *pfltD = 0.0f; 
        // SH3 eVC3.0 compiler got confused with the following syntax so make it easier on it.
        // uiC = (((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15) + (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        uiA  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15; 
        uiA +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        uiC  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiC +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        uiE  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiE +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        uiF  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiF +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        *pfltA = (float) uiA / (float) (0x8000) - (float) (0x4000);
        *pfltC = (float) uiC / (float) (0x8000) - (float) (0x4000);
        *pfltE = (float) uiE / (float) (0x8000) - (float) (0x4000);
        *pfltF = (float) uiF / (float) (0x8000) - (float) (0x4000);
    }
    else {
        uiA  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiA +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        uiB  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiB +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        uiC  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiC +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        uiD  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiD +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        uiE  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiE +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        uiF  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiF +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        *pfltA = (float) uiA / (float) (0x8000) - (float) (0x4000);
        *pfltB = (float) uiB / (float) (0x8000) - (float) (0x4000);
        *pfltC = (float) uiC / (float) (0x8000) - (float) (0x4000);
        *pfltD = (float) uiD / (float) (0x8000) - (float) (0x4000);
        *pfltE = (float) uiE / (float) (0x8000) - (float) (0x4000);
        *pfltF = (float) uiF / (float) (0x8000) - (float) (0x4000);
    }

    bFadingExist = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    if (bFadingExist) {
        uiFd  = ((unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15))<<15;
        uiFd +=  (unsigned long) BS_getBits(pWMVDec->m_pbitstrmIn, 15);
        *pfltFading = (float) uiFd / (float) (0x8000) - (float) (0x4000);
    }
    else
        *pfltFading = 1.0f;

    //  Disable mirror for now.
    if ((*pfltA < 0.0f) ||(*pfltE < 0.0f))
        return WMV_Failed;

    return WMV_Succeeded;
}

#endif


// ****************************************************************************************************

#ifdef _SPRITE_DUMPFRAME_
FILE * g_hDump;
#endif
#ifndef WMV9_SIMPLE_ONLY

tWMVDecodeStatus WMSpriteDecDecodeDataInternal (HWMVDecoder hWMVDecoder, I32_WMV iWidthDsiplay, I32_WMV iHeightDisplay)
{
    tWMVDecodeStatus result;
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 
    U32_WMV uiBufferLengthFromCaller = 0;
    U8_WMV* pucBufferBits = NULL;
    Bool_WMV bNotEndOfFrame = TRUE;

#ifdef WMVIMAGE_V2
    I32_WMV iPrevNumMotionVectorSets;
#endif //WMVIMAGE_V2

#ifndef WMV_OPT_SPRITE
    unsigned char * pTmp1, *pTmp2;
    long i;
    long iSize;
#endif //WMV_OPT_SPRITE


    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMSpriteDecDecodeDataInternal);

    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);
    
    if (!WMVideoIsSpriteMode (pWMVDec))
        return WMV_Failed;
    
    if ( pWMVDec->m_eFrameWasOutput > FrameDecOut_OutputDone ) {
        // enum { FrameDecOut_None, FrameDecOut_OutputOptional, FrameDecOut_OutputDone, FrameDecOut_Decoded, FrameDecOut_Dropped };
        pWMVDec->m_iNumFrameDropped++;
        DEBUG_PROFILE_FRAME_SET_FLAGS(1);
    }
    
    pWMVDec->m_eFrameWasOutput = FrameDecOut_None;
    
    if(pWMVDec->m_uiNumFramesLeftForOutput>0)
    {
        tWMVStatus =  WMVideoDecGetOutputInternal ( hWMVDecoder,
            pWMVDec->m_uiFOURCCOutput,
            pWMVDec->m_uiBitsPerPixelOutput,
            pWMVDec->m_uiWidthBMPOutput,
            pWMVDec->m_iDispRotateDegree,
            NULL_WMV);
        pWMVDec->m_uiNumFramesLeftForOutput = 0;
    }
    
    WMVDecCBGetData (pWMVDec->m_uiUserData, 0, (U8_WMV**) &pucBufferBits, 4, &uiBufferLengthFromCaller, &bNotEndOfFrame);
    if (bNotEndOfFrame){
        if (uiBufferLengthFromCaller == 0 || pucBufferBits == NULL){
            FUNCTION_PROFILE_STOP(&fp);
            return WMV_BrokenFrame;
        }
    }

    assert(!(uiBufferLengthFromCaller == 0));

#ifndef _WMV9AP_
    BS_reset (pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, bNotEndOfFrame);
#else

    if (pWMVDec->m_cvCodecVersion == WMVA) {
        ParseStartCode(pWMVDec, pucBufferBits, uiBufferLengthFromCaller, &pucBufferBits, &uiBufferLengthFromCaller, &bNotEndOfFrame);
        //BS_reset (pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
    };

    assert(!(uiBufferLengthFromCaller == 0 && bNotEndOfFrame  == FALSE));

    BS_reset (pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
    pWMVDec->m_pInputBitstream->m_bNotEndOfFrame = bNotEndOfFrame;

#endif //_WMV9AP_

    DEBUG_SHOW_FRAME_INPUT(pWMVDec,uiBufferLengthFromCaller, pucBufferBits)
    DEBUG_PROFILE_FRAME_SET_FLAGS(pWMVDec->m_fDecodedI ? 0 : 4);



    //    pWMVDec->m_fDecodeOnly = TRUE;
    pWMVDec->m_bRenderFromPostBuffer = TRUE;
    pWMVDec->m_bRenderDirect = FALSE_WMV;
    
    pWMVDec->m_t ++;
    
    pWMVDec->m_tFrmPredType    = (tFrameType_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, NUMBITS_VOP_PRED_TYPE_WMV2);

#ifdef WMVIMAGE_V2
    iPrevNumMotionVectorSets = pWMVDec->m_iNumMotionVectorSets;
#endif //WMVIMAGE_V2

    pWMVDec->m_iNumMotionVectorSets = BS_getBits(pWMVDec->m_pbitstrmIn, 1) + 1;
    
    if (pWMVDec->m_tFrmPredType == IVOP) {
        
        pWMVDec->m_fDecodedI = TRUE;
        
        pWMVDec->m_bFirstEscCodeInFrame = TRUE;
        
        pWMVDec->m_iWidthPrevY   = pWMVDec->m_iWidthY;
        pWMVDec->m_iHeightPrevY  = pWMVDec->m_iHeightY;
        pWMVDec->m_iWidthPrevUV  = pWMVDec->m_iWidthUV;
        pWMVDec->m_iHeightPrevUV = pWMVDec->m_iHeightUV;
        pWMVDec->m_iWidthPrevYxBlkMinusBlk = (pWMVDec->m_iWidthPrevY * BLOCK_SIZE) - BLOCK_SIZE;
        pWMVDec->m_uintNumMBX = pWMVDec->m_iWidthY >> 4; // divided by 16, MB size
        pWMVDec->m_uintNumMBY = pWMVDec->m_iHeightY >> 4;
        pWMVDec->m_iMBSizeXWidthPrevY      = pWMVDec->m_iWidthY << 4; // * MB_SIZE
        pWMVDec->m_iBlkSizeXWidthPrevUV    = pWMVDec->m_iWidthUV << 3; // * BLOCK_SIZE
        pWMVDec->m_iWidthPrevYXExpPlusExp  = 0;
        pWMVDec->m_iWidthPrevUVXExpPlusExp = 0;
        
        Affine_Init ((HWMVDecoder*)hWMVDecoder, pWMVDec->m_iWidthPrevY, pWMVDec->m_iFrmHeightSrc, iWidthDsiplay, iHeightDisplay);

        pWMVDec->m_iSpriteWidthDsiplay    =   iWidthDsiplay              ; 
        pWMVDec->m_iSpriteHeightDisplay   =   iHeightDisplay             ;
        pWMVDec->m_iSpriteWidthDsiplayMB  = ((iWidthDsiplay  + 15)/16)*16;
        pWMVDec->m_iSpriteHeightDisplayMB = ((iHeightDisplay + 15)/16)*16;
        
        pWMVDec->m_uiNumProcessors = 1;
        initMultiThreadVars_Dec ((tWMVDecInternalMember *)hWMVDecoder);
        
        if (pWMVDec->m_cvCodecVersion >= WMV3)
        {
            pWMVDec->m_pInterDCTTableInfo_Dec_Set[0] = &pWMVDec->InterDCTTableInfo_Dec_Talking;
            pWMVDec->m_pIntraDCTTableInfo_Dec_Set[0] = &pWMVDec->IntraDCTTableInfo_Dec_Talking;
        
            // reset mode
            pWMVDec->m_iSkipbitCodingMode = NOT_DECIDED;
            pWMVDec->m_iMVSwitchCodingMode = NOT_DECIDED;
            pWMVDec->m_iACPredIFrameCodingMode = NOT_DECIDED;
            //pWMVDec->m_bFieldCodingOn = FALSE;
            pWMVDec->m_iDirectCodingMode = NOT_DECIDED;
           // pWMVDec->m_bPrevRefInPrevPost = FALSE;
            pWMVDec->m_bRenderFromPostBuffer = FALSE_WMV;
            pWMVDec->m_iOverlapIMBCodingMode = NOT_DECIDED;
        }
        
        if (!pWMVDec->m_bKeepPrevImage)
            WMVideoDecSwapCurrAndRef (pWMVDec, !pWMVDec->m_iNumBFrames);
        
#ifdef _WMV9AP_
        if (pWMVDec->m_cvCodecVersion != WMVA) 
        {
            tWMVStatus = WMVideoDecDecodeFrameHead (pWMVDec);
            if (tWMVStatus != WMV_Succeeded) {
                return tWMVStatus;
            }
        }
        else 
        {
            I32_WMV iWMVAFrameType;
			I32_WMV result;
            assert (!(pWMVDec->m_bInterlacedSource) );

            iWMVAFrameType = 0;     // Progressive

            pWMVDec->m_bInterlaceV2 = pWMVDec->m_bFieldMode = FALSE;

            // Progressive Frame
            pWMVDec->m_pZigzagInvRotated = grgiZigzagInvRotated_NEW_Prog;
            pWMVDec->m_pZigzagInv = grgiZigzagInv_NEW_Prog;
            pWMVDec->m_p8x4ZigzagInv = grgi8x4ZigzagInv_Prog;
            pWMVDec->m_p4x8ZigzagInv = grgi4x8ZigzagInv_Prog;
            pWMVDec->m_p8x4ZigzagInvRotated = grgi8x4ZigzagInvRotated_Prog;
            pWMVDec->m_p4x8ZigzagInvRotated = grgi4x8ZigzagInvRotated_Prog;
            pWMVDec->m_p4x4ZigzagInv = grgi4x4ZigzagInv_Prog;
            pWMVDec->m_p4x4ZigzagInvRotated = grgi4x4ZigzagInvRotated_Prog;
            decideMMXRoutines_ZigzagOnly(pWMVDec);

            pWMVDec->m_bInterlaceV2 = FALSE; 
            pWMVDec->m_iCurrentField = 0;
            SwapMVTables (pWMVDec); // set tables    

            result = decodeVOPHeadProgressiveWMVA (pWMVDec);
			tWMVStatus = WMV_Succeeded;
			if ( result != ICERR_OK) 
				tWMVStatus = WMV_Failed;

        //Not Skip Frame, if DO, need find out how to deal with such situation.
        assert(pWMVDec->m_tFrmType != SKIPFRAME);

            #ifdef _EMB_SSIMD_IDCT_
            EMB_InitZigZagTable_SSIMD(pWMVDec->m_p8x4ZigzagScanOrder, pWMVDec->m_p8x4ZigzagSSIMD, 32);
            EMB_InitZigZagTable_SSIMD(pWMVDec->m_p4x4ZigzagScanOrder, pWMVDec->m_p4x4ZigzagSSIMD, 16);
            #endif //_EMB_SSIMD_IDCT_
        }
#else //_WMV9AP_
		{
			tWMVDecodeStatus tWMVStatus = WMVideoDecDecodeFrameHead (pWMVDec);
			if (tWMVStatus != WMV_Succeeded) {
				FUNCTION_PROFILE_STOP(&fp);
				return tWMVStatus;
			}
		}
#endif //_WMV9AP_
        
#ifdef PPCWMP
        if (pWMVDec->m_bSpriteModeV2)
            pWMVDec->m_iPostProcessMode = 0;
        else
#endif
            pWMVDec->m_iPostProcessMode = 2;

        //Not Intra8 Mode, otherwise, need call tWMVStatus = WMVideoDecDecodeI_X8 (pWMVDec);
        assert (!(pWMVDec->m_bXintra8));
        

		tWMVStatus = WMVideoDecDecodeI (pWMVDec);
        
        if (tWMVStatus != WMV_Succeeded) {
            return tWMVStatus;
        }
        
        
        SetupPostFilterMode (pWMVDec);
        if (pWMVDec->m_bDeblockOn) { // 
            unsigned char * ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
            unsigned char * ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
            unsigned char * ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
            unsigned char * ppxliPostQY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
            unsigned char * ppxliPostQU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
            unsigned char * ppxliPostQV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
            int i;


            START_POST_LEVEL_POST_TIME(pWMVDec);
            for (i = 0; i < pWMVDec->m_iHeightUV; i++) {
                ALIGNED32_FASTMEMCPY(ppxliPostQU, ppxliCurrQU, pWMVDec->m_iWidthPrevUV);
                ppxliPostQU += pWMVDec->m_iWidthPrevUV;
                ppxliCurrQU += pWMVDec->m_iWidthPrevUV;
                ALIGNED32_FASTMEMCPY(ppxliPostQV, ppxliCurrQV, pWMVDec->m_iWidthPrevUV);
                ppxliPostQV += pWMVDec->m_iWidthPrevUV;
                ppxliCurrQV += pWMVDec->m_iWidthPrevUV;
                ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY);
                ppxliPostQY += pWMVDec->m_iWidthPrevY;
                ppxliCurrQY += pWMVDec->m_iWidthPrevY;
                ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY);
                ppxliPostQY += pWMVDec->m_iWidthPrevY;
                ppxliCurrQY += pWMVDec->m_iWidthPrevY;
            }
            
            DeblockFrame (pWMVDec, 0, pWMVDec->m_uiMBEnd0);
            
            {
                unsigned char* ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
                unsigned char* ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
                unsigned char* ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
                unsigned char* ppxliPostQY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
                unsigned char* ppxliPostQU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
                unsigned char* ppxliPostQV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
                int i;
                for (i = 0; i < pWMVDec->m_iHeightUV; i++) {
                    ALIGNED32_FASTMEMCPY(ppxliCurrQU, ppxliPostQU, pWMVDec->m_iWidthPrevUV);
                    ppxliPostQU += pWMVDec->m_iWidthPrevUV;
                    ppxliCurrQU += pWMVDec->m_iWidthPrevUV;
                    ALIGNED32_FASTMEMCPY(ppxliCurrQV, ppxliPostQV, pWMVDec->m_iWidthPrevUV);
                    ppxliPostQV += pWMVDec->m_iWidthPrevUV;
                    ppxliCurrQV += pWMVDec->m_iWidthPrevUV;
                    ALIGNED32_FASTMEMCPY(ppxliCurrQY, ppxliPostQY, pWMVDec->m_iWidthPrevY);
                    ppxliPostQY += pWMVDec->m_iWidthPrevY;
                    ppxliCurrQY += pWMVDec->m_iWidthPrevY;
                    ALIGNED32_FASTMEMCPY(ppxliCurrQY, ppxliPostQY, pWMVDec->m_iWidthPrevY);
                    ppxliPostQY += pWMVDec->m_iWidthPrevY;
                    ppxliCurrQY += pWMVDec->m_iWidthPrevY;
                }
            }
            STOP_POST_LEVEL_POST_TIME(pWMVDec);
        }
        
#       ifdef _SPRITE_DUMPFRAME_
        {
            U8_WMV * ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
            U8_WMV * ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
            U8_WMV * ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
            
            if(g_hDump == NULL)
                g_hDump = fopen("spriteDumpPK.raw", "wb");
            fwrite(ppxliCurrQY, 1, pWMVDec->m_iWidthPrevY*pWMVDec->m_iHeightY, g_hDump);             
            fwrite(ppxliCurrQU, 1, pWMVDec->m_iWidthPrevUV*pWMVDec->m_iHeightUV, g_hDump);
            fwrite(ppxliCurrQV, 1, pWMVDec->m_iWidthPrevUV*pWMVDec->m_iHeightUV, g_hDump);
        }
#       endif

    } else {
        // Not a IVOP, so just an interpolated frame
        if (!pWMVDec->m_fDecodedI) {
            return WMV_Failed;
        }
#if 1	/* wshao 2011.3.24.
		 * APP will drop BVOP sometimes,for example,when seeking. 
		 * So we can't mask all non-IVOP as BVOP for WMVP/WVP2 format stream(it is picture sequence,not real video),
		 * because these non-IVOP are not real BVOP which can't be dropped.In fact they play PVOP role.
		 */
        pWMVDec->m_tFrmType = PVOP;
#else
        pWMVDec->m_tFrmType = BVOP;
#endif
        HUFFMANGET_DBG_HEADER(":decodeSprite",15);
    }
    
	pWMVDec->m_pfrmCurrQ->m_timeStamp = pWMVDec->m_inTimeStamp;
	pWMVDec->m_pfrmCurrQ->m_frameType = pWMVDec->m_tFrmType;
    pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData;
    
    if (pWMVDec->m_iNumMotionVectorSets != 1 && pWMVDec->m_iNumMotionVectorSets != 2)
        return WMV_Failed;
    
    // bitstream pattern:
    // 00: translation only, 01: translation and same zooming factor, 10: translation and zooming, 11: generic
#   ifdef _WMV_TARGET_X86_
        if (g_bSupportMMX_WMV)
            _asm emms; //Make sure floating point calc works for building the table
#   endif


#ifdef WMVIMAGE_V2
    if(pWMVDec->m_outputMode  ) //zou 418
    {
        if(pWMVDec->m_tFrmType == IVOP)
        {
            pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
            pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
            pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmCurrQ->m_pucVPlane;
        }
    }

    if (pWMVDec->m_bSpriteModeV2)
    {
        result = WMSpriteApplyEffectsV2(hWMVDecoder, iPrevNumMotionVectorSets);
        if (WMV_Succeeded != result) {
            return result;
        }
    }
    else
#endif //WMVIMAGE_V2
    {
        float fltA1, fltB1, fltC1, fltD1, fltE1, fltF1, fltFading1;
        float fltA2, fltB2, fltC2, fltD2, fltE2, fltF2, fltFading2;

        #ifndef WMV_OPT_SPRITE

        result = DecodeAffineCoefficients (hWMVDecoder, &fltA1, &fltB1, &fltC1, &fltD1, &fltE1, &fltF1, &fltFading1);
        if (WMV_Succeeded != result) {
            return result;
        }

        if (pWMVDec->m_iAffinePattern == 0) {
            Affine_Transform (
                (HWMVDecoder*)hWMVDecoder,
                pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliCurrQV,
                pWMVDec->m_ppxliPostQY, pWMVDec->m_ppxliPostQU, pWMVDec->m_ppxliPostQV,
                1.0f, 0.0, fltC1, 0.0f, 1.0f, fltF1, fltFading1
                );
        }
        else if (pWMVDec->m_iAffinePattern == 1) {
            Affine_Transform (
                (HWMVDecoder*)hWMVDecoder,
                pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliCurrQV,
                pWMVDec->m_ppxliPostQY, pWMVDec->m_ppxliPostQU, pWMVDec->m_ppxliPostQV,
                fltA1, 0.0f, fltC1, 0.0f, fltA1, fltF1, fltFading1
                );
        }
        else {
            Affine_Transform (
                (HWMVDecoder*)hWMVDecoder,
                pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliCurrQV,
                pWMVDec->m_ppxliPostQY, pWMVDec->m_ppxliPostQU, pWMVDec->m_ppxliPostQV,
                fltA1, fltB1, fltC1, fltD1, fltE1, fltF1, fltFading1
                );
        }
        DEBUG_SHOW_FRAME_BUFFERS(pWMVDec->m_ppxliPostQY,pWMVDec->m_ppxliCurrQY,NULL);
    
        if (pWMVDec->m_iNumMotionVectorSets == 2) {
            result = DecodeAffineCoefficients (hWMVDecoder, &fltA2, &fltB2, &fltC2, &fltD2, &fltE2, &fltF2, &fltFading2);
        
            if (pWMVDec->m_iAffinePattern == 0) {
                Affine_Transform (
                    (HWMVDecoder*)hWMVDecoder,
                    pWMVDec->m_ppxliRef0Y, pWMVDec->m_ppxliRef0U, pWMVDec->m_ppxliRef0V,
                    pWMVDec->m_ppxliPostPrevQY, pWMVDec->m_ppxliPostPrevQU, pWMVDec->m_ppxliPostPrevQV,
                    1.0f, 0.0f, fltC2, 0.0f, 1.0f, fltF2, fltFading2
                    );
            }
            else if (pWMVDec->m_iAffinePattern == 1) {
                Affine_Transform (
                    (HWMVDecoder*)hWMVDecoder,
                    pWMVDec->m_ppxliRef0Y, pWMVDec->m_ppxliRef0U, pWMVDec->m_ppxliRef0V,
                    pWMVDec->m_ppxliPostPrevQY, pWMVDec->m_ppxliPostPrevQU, pWMVDec->m_ppxliPostPrevQV,
                    fltA2, 0.0f, fltC2, 0.0f, fltA2, fltF2, fltFading2
                    );
            }
            else {
                Affine_Transform (
                    (HWMVDecoder*)hWMVDecoder,
                    pWMVDec->m_ppxliRef0Y, pWMVDec->m_ppxliRef0U, pWMVDec->m_ppxliRef0V,
                    pWMVDec->m_ppxliPostPrevQY, pWMVDec->m_ppxliPostPrevQU, pWMVDec->m_ppxliPostPrevQV,
                    fltA2, fltB2, fltC2, fltD2, fltE2, fltF2, fltFading2
                    );
            }
        
            pTmp1 = pWMVDec->m_ppxliPostQY;
            pTmp2 = pWMVDec->m_ppxliPostPrevQY;
            iSize = pWMVDec->m_iNewWidth*pWMVDec->m_iNewHeight;
            for (i=0; i<iSize; i++) {
                int tmp ;
            
                tmp = *pTmp1 +*pTmp2;
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                *pTmp1 = (U8_WMV)tmp;
                pTmp1++;
                pTmp2++;
            }
        
            pTmp1 = pWMVDec->m_ppxliPostQU;
            pTmp2 = pWMVDec->m_ppxliPostPrevQU;
            iSize = pWMVDec->m_iNewWidth*pWMVDec->m_iNewHeight/4;
            for (i=0; i<iSize; i++) {
                int tmp = *pTmp1 + *pTmp2 - 128;
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                *pTmp1 = (U8_WMV)tmp;//*pTmp1 + *pTmp2 - 128;
            
                pTmp1++;
                pTmp2++;
            }
        
            pTmp1 = pWMVDec->m_ppxliPostQV;
            pTmp2 = pWMVDec->m_ppxliPostPrevQV;
            iSize = pWMVDec->m_iNewWidth*pWMVDec->m_iNewHeight/4;
            for (i=0; i<iSize; i++) {
                int tmp = *pTmp1 + *pTmp2 - 128;
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                *pTmp1 = (U8_WMV)tmp;//*pTmp1 + *pTmp2 - 128;
                pTmp1++;
                pTmp2++;
            }
        DEBUG_SHOW_FRAME_BUFFERS(pWMVDec->m_ppxliPostQY,pWMVDec->m_ppxliCurrQY,pWMVDec->m_ppxliRef0Y);
        }


        #else //WMV_SPRITE_OPT
    
        #   ifdef _WMV_TARGET_X86_
                if (g_bSupportMMX_WMV)
                    _asm emms; //Make sure floating point calc works for building the table
        #   endif

        result = DecodeAffineCoefficients (hWMVDecoder, &fltA1, &fltB1, &fltC1, &fltD1, &fltE1, &fltF1, &fltFading1);
        if (WMV_Succeeded != result) {
            return result;
        }

        DEBUG_SHOW_FRAME_BUFFERS(pWMVDec->m_ppxliPostQY,NULL,NULL);

        if (pWMVDec->m_fltA1 != fltA1 ||
            pWMVDec->m_fltB1 != fltB1 ||
            pWMVDec->m_fltC1 != fltC1 ||
            pWMVDec->m_fltD1 != fltD1 ||
            pWMVDec->m_fltE1 != fltE1 ||
            pWMVDec->m_fltF1 != fltF1 ||
            pWMVDec->m_fltFading1 != fltFading1) {
            pWMVDec->m_fltA1 = fltA1;
            pWMVDec->m_fltB1 = fltB1;
            pWMVDec->m_fltC1 = fltC1;
            pWMVDec->m_fltD1 = fltD1;
            pWMVDec->m_fltE1 = fltE1;
            pWMVDec->m_fltF1 = fltF1;
            pWMVDec->m_fltFading1 = fltFading1;
            if (pWMVDec->m_iAffinePattern == 0) {
                Affine_Transform (
                    (HWMVDecoder*)hWMVDecoder,
                    pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliCurrQV,
                    pWMVDec->m_ppxliPostQY, pWMVDec->m_ppxliPostQU, pWMVDec->m_ppxliPostQV,
                    1.0, 0.0, fltC1, 0.0, 1.0, fltF1, fltFading1
                    );
            }
            else if (pWMVDec->m_iAffinePattern == 1) {
                Affine_Transform (
                    (HWMVDecoder*)hWMVDecoder,
                    pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliCurrQV,
                    pWMVDec->m_ppxliPostQY, pWMVDec->m_ppxliPostQU, pWMVDec->m_ppxliPostQV,
                    fltA1, 0.0, fltC1, 0.0, fltA1, fltF1, fltFading1
                    );
            }
            else {
                Affine_Transform (
                    (HWMVDecoder*)hWMVDecoder,
                    pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliCurrQV,
                    pWMVDec->m_ppxliPostQY, pWMVDec->m_ppxliPostQU, pWMVDec->m_ppxliPostQV,
                    fltA1, fltB1, fltC1, fltD1, fltE1, fltF1, fltFading1
                    );
            }
            DEBUG_SHOW_FRAME_BUFFERS(pWMVDec->m_ppxliPostQY,pWMVDec->m_ppxliCurrQY,NULL);
        }

     //   if(pWMVDec->m_outputMode) //zou 418
     //   {
     //       pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
     //       pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
     //       pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmCurrQ->m_pucVPlane;
	    //}

        if (pWMVDec->m_iNumMotionVectorSets == 2) {
        #   ifdef _WMV_TARGET_X86_
                if (g_bSupportMMX_WMV)
                    _asm emms; //Make sure floating point calc works for building the table
        #   endif
            result = DecodeAffineCoefficients (hWMVDecoder, &fltA2, &fltB2, &fltC2, &fltD2, &fltE2, &fltF2, &fltFading2);
            if (pWMVDec->m_fltA2 != fltA2 ||
                pWMVDec->m_fltB2 != fltB2 ||
                pWMVDec->m_fltC2 != fltC2 ||
                pWMVDec->m_fltD2 != fltD2 ||
                pWMVDec->m_fltE2 != fltE2 ||
                pWMVDec->m_fltF2 != fltF2 ||
                pWMVDec->m_fltFading2 != fltFading2) {
                pWMVDec->m_fltA2 = fltA2;
                pWMVDec->m_fltB2 = fltB2;
                pWMVDec->m_fltC2 = fltC2;
                pWMVDec->m_fltD2 = fltD2;
                pWMVDec->m_fltE2 = fltE2;
                pWMVDec->m_fltF2 = fltF2;
                pWMVDec->m_fltFading2 = fltFading2;        
                if (pWMVDec->m_iAffinePattern == 0) {
                    Affine_Transform (
                        (HWMVDecoder*)hWMVDecoder,
                        pWMVDec->m_ppxliRef0Y, pWMVDec->m_ppxliRef0U, pWMVDec->m_ppxliRef0V,
                        pWMVDec->m_ppxliPostPrevQY, pWMVDec->m_ppxliPostPrevQU, pWMVDec->m_ppxliPostPrevQV,
                        1.0, 0.0, fltC2, 0.0, 1.0, fltF2, fltFading2
                        );
                }
                else if (pWMVDec->m_iAffinePattern == 1) {
                    Affine_Transform (
                        (HWMVDecoder*)hWMVDecoder,
                        pWMVDec->m_ppxliRef0Y, pWMVDec->m_ppxliRef0U, pWMVDec->m_ppxliRef0V,
                        pWMVDec->m_ppxliPostPrevQY, pWMVDec->m_ppxliPostPrevQU, pWMVDec->m_ppxliPostPrevQV,
                        fltA2, 0.0, fltC2, 0.0, fltA2, fltF2, fltFading2
                        );
                }
                else {
                    Affine_Transform (
                        (HWMVDecoder*)hWMVDecoder,
                        pWMVDec->m_ppxliRef0Y, pWMVDec->m_ppxliRef0U, pWMVDec->m_ppxliRef0V,
                        pWMVDec->m_ppxliPostPrevQY, pWMVDec->m_ppxliPostPrevQU, pWMVDec->m_ppxliPostPrevQV,
                        fltA2, fltB2, fltC2, fltD2, fltE2, fltF2, fltFading2
                        );
                }
            
                Affine_Add (
                    pWMVDec->m_ppxliPostPrevQY,
                    pWMVDec->m_ppxliPostPrevQU,
                    pWMVDec->m_ppxliPostPrevQV,
                    pWMVDec->m_ppxliPostQY,
                    pWMVDec->m_ppxliPostQU,
                    pWMVDec->m_ppxliPostQV,
                    pWMVDec->m_iNewWidth * pWMVDec->m_iNewHeight);
                DEBUG_SHOW_FRAME_BUFFERS(pWMVDec->m_ppxliPostQY,pWMVDec->m_ppxliCurrQY,pWMVDec->m_ppxliRef0Y);
            }
        }
    
        #endif // WMV_OPT_SPRITE
    }
        
	pWMVDec->m_pfrmPostQ->m_timeStamp = pWMVDec->m_pfrmCurrQ->m_timeStamp;
	pWMVDec->m_pfrmPostQ->m_frameType = pWMVDec->m_pfrmCurrQ->m_frameType;
    pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData;

    pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmPostQ;
    pWMVDec->m_uiNumFramesLeftForOutput = 1; // 1 frame ready to output after decoding.
    pWMVDec->m_eFrameWasOutput = FrameDecOut_Decoded;


    return WMV_Succeeded;
}
#endif

Void_WMV checkEndOfFrame(tWMVDecInternalMember *pWMVDec)
{   
    if(pWMVDec->m_pInputBitstream->m_bNotEndOfFrame)
    {
        U8_WMV* pucBufferBits;
        Bool_WMV bNotEndOfFrame;
        U32_WMV uSize;

        bNotEndOfFrame = pWMVDec->m_pInputBitstream->m_bNotEndOfFrame;
        while(bNotEndOfFrame)
        {
            CInputBitStream_WMV * pThis = pWMVDec->m_pInputBitstream;
            WMVDecCBGetData (pThis->m_uiUserData, 0, &pucBufferBits, 4, &uSize, &bNotEndOfFrame);
            pThis->m_bNotEndOfFrame = bNotEndOfFrame;
            if (pThis->m_pWMVDec->m_cvCodecVersion == WMVA) {
                U8_WMV * p = pucBufferBits;
                //uSize -= pThis->m_pWMVDec->m_iParseStartCodeRemLen;
                ParseStartCodeChunk(pThis->m_pWMVDec, p, uSize, &p, &uSize, bNotEndOfFrame);
                //assert(p == pucBufferBits);
            }
        }
    }
}

I32_WMV SwitchEntryPoint (tWMVDecInternalMember *pWMVDec)
{
    I32_WMV result;
    I32_WMV iPrevEncWidth;
    I32_WMV iPrevEncHeight;

    pWMVDec->m_bSwitchingEntryPoint = TRUE;

        ResetConditionalVariablesForSequenceSwitch (pWMVDec);  // Rename this function to ResetConditionalVariablesForEntrySwitch
                                                    // once we switch over to new entry header
     iPrevEncWidth = pWMVDec->m_iFrmWidthSrc;
     iPrevEncHeight = pWMVDec->m_iFrmHeightSrc;

    result = DecodeEntryPointHeader(pWMVDec, NULL, NULL);
    if (ICERR_OK != result) {
        return result;
    }

    //printf("Encoding size: %d x %d\n", pWMVDec->m_iFrmWidthSrc, pWMVDec->m_iFrmHeightSrc);

    //Only after entry-point header can we know the real encoding size.
    //Now we can adjust pointer and variables.
    if (pWMVDec->m_iFrmWidthSrc != iPrevEncWidth || pWMVDec->m_iFrmHeightSrc != iPrevEncHeight) {
        pWMVDec->m_bSeqFrameWidthOrHeightChanged = TRUE;
    } else {
        pWMVDec->m_bSeqFrameWidthOrHeightChanged = FALSE;
    }

    result = InitDataMembers_WMVA (pWMVDec, pWMVDec->m_iFrmWidthSrc, pWMVDec->m_iFrmHeightSrc);
    
    pWMVDec->m_bSwitchingEntryPoint = TRUE;
    
    return result;
}

// ****************************************************************************************************

tWMVDecodeStatus WMVideoDecDecodeData (HWMVDecoder hWMVDecoder, U16_WMV* puiNumDecodedFrames )
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 

    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember *)hWMVDecoder;
    DEBUG_PROFILE_FRAME_DECODE_DATA_START;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMVideoDecDecodeData);
    START_POST_LEVEL_DECODE_TIME(pWMVDec);

#ifndef WMV9_SIMPLE_ONLY
    if (WMVideoIsSpriteMode (hWMVDecoder)) {
        tWMVStatus = WMSpriteDecDecodeDataInternal (hWMVDecoder, 
                                                    pWMVDec->m_iSpriteWidthDsiplay, 
                                                    pWMVDec->m_iSpriteHeightDisplay);
        *puiNumDecodedFrames = 1;
    } else
#endif
        tWMVStatus = WMVideoDecDecodeDataInternal (hWMVDecoder, puiNumDecodedFrames);

    STOP_POST_LEVEL_DECODE_TIME(pWMVDec);
    DEBUG_PROFILE_FRAME_DECODE_DATA_STOP;
    return tWMVStatus;
}

// ****************************************************************************************************


tWMVDecodeStatus WMVideoDecDecodeDataInternal (HWMVDecoder hWMVDecoder, U16_WMV* puiNumDecodedFrames)
{
    FUNCTION_PROFILE(fp)
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 
    U32_WMV uiBufferLengthFromCaller = 0;
    U8_WMV* pucBufferBits = NULL;
    Bool_WMV bNotEndOfFrame = TRUE;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMVideoDecDecodeDataInternal);

    *puiNumDecodedFrames = 0;
    
#   ifdef _COUNT_CYCLES_
        DecodeAndLockTmr -= HrTmrGet ();
#   endif

#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO WMVideoDecDecodeDataInternal 1");
#endif
    
    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;

    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);
    
    FUNCTION_PROFILE_START(&fp,WMVDECODEDATA_PROFILE);
    
    if ( pWMVDec->m_eFrameWasOutput > FrameDecOut_OutputDone ) {
        // enum { FrameDecOut_None, FrameDecOut_OutputOptional, FrameDecOut_OutputDone, FrameDecOut_Decoded, FrameDecOut_Dropped };
        pWMVDec->m_iNumFrameDropped++;
        DEBUG_PROFILE_FRAME_SET_FLAGS(1);
    }
    
    pWMVDec->m_eFrameWasOutput = FrameDecOut_None;
    
    if(pWMVDec->m_uiNumFramesLeftForOutput>0) {
        tWMVStatus =  WMVideoDecGetOutputInternal ( 
                            hWMVDecoder,
                            pWMVDec->m_uiFOURCCOutput,
                            pWMVDec->m_uiBitsPerPixelOutput,
                            pWMVDec->m_uiWidthBMPOutput,
                            pWMVDec->m_iDispRotateDegree,
                            NULL_WMV);
                           
        pWMVDec->m_uiNumFramesLeftForOutput = 0;
    }
    
#ifndef WMV9_SIMPLE_ONLY
#   ifdef WMV_OPT_COMBINE_LF_CC
        if (pWMVDec->m_cvCodecVersion != WMV3) {
            if (pWMVDec->m_bNeedLoopFilterOnCurrFrm) {
                DeblockSLFrame_V8 (pWMVDec, pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
                                   pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                                   pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
                                   FALSE, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);

                pWMVDec->m_bNeedLoopFilterOnCurrFrm = FALSE_WMV;
            }
        }
#   endif
#endif
    
    WMVDecCBGetData (pWMVDec->m_uiUserData, 0, (U8_WMV**) &pucBufferBits, 4, &uiBufferLengthFromCaller, &bNotEndOfFrame);
    if (bNotEndOfFrame) {
        if (uiBufferLengthFromCaller == 0 || pucBufferBits == NULL) {
            FUNCTION_PROFILE_STOP(&fp);
            return WMV_BrokenFrame;
        }
    }

    DEBUG_SHOW_FRAME_INPUT(pWMVDec,uiBufferLengthFromCaller,pucBufferBits)
    DEBUG_PROFILE_FRAME_SET_FLAGS(pWMVDec->m_fDecodedI ? 0 : 4);
    
    if (uiBufferLengthFromCaller == 0) {
        pWMVDec->m_eFrameWasOutput = FrameDecOut_OutputOptional;
        pWMVDec->m_bRenderDirect = TRUE_WMV;
        return WMV_Succeeded;
    }
    else if (uiBufferLengthFromCaller == 1 && bNotEndOfFrame == FALSE_WMV) {
        assert(pWMVDec->m_cvCodecVersion >= WMV3) ;

#ifdef _WMV9AP_
        if (pWMVDec->m_cvCodecVersion == WMVA ){

        }
        else{
            if (pWMVDec->m_iNumBFrames == 0) //getNumBFrames
            {
                pWMVDec->m_bRenderDirect = TRUE_WMV;
                pWMVDec->m_eFrameWasOutput = FrameDecOut_OutputOptional;
                return WMV_Succeeded;
            }
            else{ 
                decodeSkipP(pWMVDec);
                pWMVDec->m_bRenderDirect = TRUE_WMV;
                pWMVDec->m_eFrameWasOutput = FrameDecOut_OutputOptional;
                return WMV_Succeeded;
            }
        }
#else //_WMV9AP_
        if (pWMVDec->m_iNumBFrames == 0) {
            //getNumBFrames
            pWMVDec->m_bRenderDirect = TRUE_WMV;
            pWMVDec->m_eFrameWasOutput = FrameDecOut_OutputOptional;
            return WMV_Succeeded;
        }
        else{ 
            decodeSkipP(pWMVDec);
            pWMVDec->m_bRenderDirect = TRUE_WMV;
            pWMVDec->m_eFrameWasOutput = FrameDecOut_OutputOptional;
            return WMV_Succeeded;
        }
#endif //_WMV9AP_
    }
    
#ifdef _WMV9AP_
    if (pWMVDec->m_cvCodecVersion == WMVA) {
        ParseStartCode(pWMVDec, pucBufferBits, uiBufferLengthFromCaller, &pucBufferBits, &uiBufferLengthFromCaller, &bNotEndOfFrame);
        //BS_reset (pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
    };
    if (uiBufferLengthFromCaller == 0 && bNotEndOfFrame  == FALSE) 
    {
        pWMVDec->m_bRenderDirect = TRUE_WMV;
        return WMV_Succeeded;
    }
    BS_reset (pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
    pWMVDec->m_pInputBitstream->m_bNotEndOfFrame = bNotEndOfFrame;
#endif

    pWMVDec->m_t++;
    pWMVDec->m_bRenderDirect = FALSE_WMV;
    
            //Is this the right place? 
    pWMVDec->m_uiCurrSlice = 0;

    pWMVDec->m_bSkipThisFrame = FALSE;
    if ( !pWMVDec->m_bOmittedBFrame )
        pWMVDec->m_tFrmTypePriorFrame = pWMVDec->m_tFrmType;
    else
        pWMVDec->m_bOmittedBFrame = FALSE_WMV;
    
#   ifdef _EMB_WMV3_
#if defined(macintosh) || defined(_ARM_)
    if ((!g_bSupportAltiVec_WMV) && (!g_bSupportWMMX_WMV))
#endif
    {
        if ((pWMVDec->m_cvCodecVersion == WMV3) ||
           (pWMVDec->m_cvCodecVersion == WMVA && !pWMVDec->m_bInterlaceV2)) {
            if((pWMVDec->m_bRTMContent == FALSE_WMV) 
                || (pWMVDec->m_pInterIDCT_Dec == g_IDCTDec_WMV2_Inter)
                ||(pWMVDec->m_uintNumMBX == 1) 
                ||(pWMVDec->m_iBetaRTMMismatchIndex > 0)
                )
            {
                assert(pWMVDec->m_pDecodeP == WMVideoDecDecodeP_X9);
            }
            else {
                if (pWMVDec->m_cvCodecVersion != WMVA || 
                    (pWMVDec->m_bNoSliceCodeInSequence && pWMVDec->m_bNoBFrameInSequence)) {
                    pWMVDec->m_pDecodeP = WMVideoDecDecodeP_X9_EMB;
                    pWMVDec->m_pDecodeB = decodeB_EMB;
                }
            }
        }
    }
#   endif
    
    // Reset DCT tables
    if (pWMVDec->m_cvCodecVersion >= WMV3)
    {
        pWMVDec->m_pInterDCTTableInfo_Dec_Set[0] = &pWMVDec->InterDCTTableInfo_Dec_Talking;
        pWMVDec->m_pIntraDCTTableInfo_Dec_Set[0] = &pWMVDec->IntraDCTTableInfo_Dec_Talking;
        
        // reset mode
        pWMVDec->m_iSkipbitCodingMode = NOT_DECIDED;
        pWMVDec->m_iMVSwitchCodingMode = NOT_DECIDED;
        pWMVDec->m_iACPredIFrameCodingMode = NOT_DECIDED;
        //pWMVDec->m_bFieldCodingOn = FALSE;
        pWMVDec->m_iDirectCodingMode = NOT_DECIDED;
       // pWMVDec->m_bPrevRefInPrevPost = FALSE;
        pWMVDec->m_bRenderFromPostBuffer = FALSE_WMV;
        pWMVDec->m_iOverlapIMBCodingMode = NOT_DECIDED;
    }
    
#ifdef _WMV9AP_
    if (pWMVDec->m_cvCodecVersion != WMVA) {

#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO WMVideoDecDecodeFrameHead 1");
#endif
		tWMVStatus = WMVideoDecDecodeFrameHead (pWMVDec);

#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO WMVideoDecDecodeFrameHead 2");
#endif
		if (tWMVStatus != WMV_Succeeded) {
			FUNCTION_PROFILE_STOP(&fp);
			return tWMVStatus;
		}
    } 
	else {       
        WMVFrameType iFrameType;

        if (pWMVDec->m_bInterlacedSource) {
         if (0 == BS_getBits(pWMVDec->m_pbitstrmIn, 1))
              iFrameType = PROGRESSIVE;     // Progressive
        else {
            if (0 == BS_getBits(pWMVDec->m_pbitstrmIn, 1))
                    iFrameType = INTERLACEFRAME; // Frame Interlace
                else    
                    iFrameType = INTERLACEFIELD; // Field interlace
            }
        }
        else 
            iFrameType = PROGRESSIVE;    // Progressive       

		/* It will crash with VC1 INTERLACEFIELD mode,so don't support it temperarily. */
		if(iFrameType == INTERLACEFIELD && pWMVDec->m_bCodecIsWVC1) {
			printf("\nInterlace field mode,not fully support now.");
            return WMV_Failed;
		}

        pWMVDec->m_bInterlaceV2 = pWMVDec->m_bFieldMode = FALSE;
        if (iFrameType == PROGRESSIVE){// Progressive Frame
            pWMVDec->m_pZigzagInvRotated = grgiZigzagInvRotated_NEW_Prog;
            pWMVDec->m_pZigzagInv = grgiZigzagInv_NEW_Prog;
            pWMVDec->m_p8x4ZigzagInv = grgi8x4ZigzagInv_Prog;
            pWMVDec->m_p4x8ZigzagInv = grgi4x8ZigzagInv_Prog;
            pWMVDec->m_p8x4ZigzagInvRotated = grgi8x4ZigzagInvRotated_Prog;
            pWMVDec->m_p4x8ZigzagInvRotated = grgi4x8ZigzagInvRotated_Prog;
            pWMVDec->m_p4x4ZigzagInv = grgi4x4ZigzagInv_Prog;
            pWMVDec->m_p4x4ZigzagInvRotated = grgi4x4ZigzagInvRotated_Prog;
            decideMMXRoutines_ZigzagOnly(pWMVDec);

            pWMVDec->m_bInterlaceV2 = FALSE; 
            pWMVDec->m_bFieldMode = FALSE;
            pWMVDec->m_iCurrentField = 0;
            SwapMVTables (pWMVDec); // set tables    

            if (ICERR_OK != decodeVOPHeadProgressiveWMVA (pWMVDec))
                return WMV_Failed;

            if (pWMVDec->m_tFrmType == SKIPFRAME) {
                Repeatpad (pWMVDec);
                checkEndOfFrame(pWMVDec);
                pWMVDec->m_bRenderDirect = TRUE_WMV;
                pWMVDec->m_bRenderDirectWithPrerender = TRUE_WMV;
				pWMVDec->m_pfrmCurrQ->m_timeStamp = pWMVDec->m_inTimeStamp;
				pWMVDec->m_pfrmCurrQ->m_frameType = pWMVDec->m_tFrmType;
                pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData;
                return WMV_Succeeded;
            }
        } 
		else if (iFrameType == INTERLACEFRAME){//Frame Interlace 
            pWMVDec->m_pZigzagInvRotated = grgiZigzagInvRotated_NEW_Interlace;
            pWMVDec->m_pZigzagInv = grgiZigzagInv_NEW_Interlace;
            pWMVDec->m_p8x4ZigzagInv = grgi8x4ZigzagInv_Interlace;
            pWMVDec->m_p4x8ZigzagInv = grgi4x8ZigzagInv_Interlace;
            pWMVDec->m_p8x4ZigzagInvRotated = grgi8x4ZigzagInvRotated_Interlace;
            pWMVDec->m_p4x8ZigzagInvRotated = grgi4x8ZigzagInvRotated_Interlace;
            pWMVDec->m_p4x4ZigzagInv = grgi4x4ZigzagInv_Interlace;
            pWMVDec->m_p4x4ZigzagInvRotated = grgi4x4ZigzagInvRotated_Interlace;
            decideMMXRoutines_ZigzagOnly(pWMVDec);

            pWMVDec->m_bInterlaceV2 = TRUE;
            pWMVDec->m_iCurrentField = 0;
            SwapMVTables (pWMVDec); // set tables
#ifdef PPCWMP
            return WMV_UnSupportedCompressedFormat;
#else
			if(decodeVOPHeadInterlaceV2 (pWMVDec) != ICERR_OK)
                return WMV_Failed;
#endif

            if (pWMVDec->m_tFrmType == SKIPFRAME) 
            {
                Repeatpad (pWMVDec);
                checkEndOfFrame(pWMVDec);
                pWMVDec->m_bRenderDirect = TRUE_WMV;
                pWMVDec->m_bRenderDirectWithPrerender = TRUE_WMV;
				pWMVDec->m_pfrmCurrQ->m_timeStamp = pWMVDec->m_inTimeStamp;
				pWMVDec->m_pfrmCurrQ->m_frameType = pWMVDec->m_tFrmType; //pWMVDec->m_tFrmTypePriorFrame;//
                pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData;
                return WMV_Succeeded;
            }

        } 
		else {
            pWMVDec->m_pZigzagInvRotated = grgiZigzagInvRotated_NEW_Interlace;
            pWMVDec->m_pZigzagInv = grgiZigzagInv_NEW_Interlace;
            pWMVDec->m_p8x4ZigzagInv = grgi8x4ZigzagInv_Interlace;
            pWMVDec->m_p4x8ZigzagInv = grgi4x8ZigzagInv_Interlace;
            pWMVDec->m_p8x4ZigzagInvRotated = grgi8x4ZigzagInvRotated_Interlace;
            pWMVDec->m_p4x8ZigzagInvRotated = grgi4x8ZigzagInvRotated_Interlace;
            pWMVDec->m_p4x4ZigzagInv = grgi4x4ZigzagInv_Interlace;
            pWMVDec->m_p4x4ZigzagInvRotated = grgi4x4ZigzagInvRotated_Interlace;
            decideMMXRoutines_ZigzagOnly(pWMVDec);

            pWMVDec->m_bInterlaceV2 = TRUE;
            pWMVDec->m_bFieldMode = TRUE;
            SwapMVTables (pWMVDec); // set tables
#ifdef PPCWMP
            return WMV_UnSupportedCompressedFormat;
#else
            decodeVOPHeadFieldPicture (pWMVDec);
            if (ICERR_OK != DecodeFieldModeFrame (pWMVDec))
                return WMV_Failed;
#endif
            goto PostDecode;
        }
        #ifdef _EMB_SSIMD_IDCT_
        EMB_InitZigZagTable_SSIMD(pWMVDec->m_p8x4ZigzagScanOrder, pWMVDec->m_p8x4ZigzagSSIMD, 32);
        EMB_InitZigZagTable_SSIMD(pWMVDec->m_p4x4ZigzagScanOrder, pWMVDec->m_p4x4ZigzagSSIMD, 16);
        #endif
    }
#else
    tWMVStatus = WMVideoDecDecodeFrameHead (pWMVDec);
    if (tWMVStatus != WMV_Succeeded) {
        FUNCTION_PROFILE_STOP(&fp);
        return tWMVStatus;
    }
#endif

    pWMVDec->m_bFirstEscCodeInFrame = TRUE_WMV;
    pWMVDec->m_bBFrameDropped = FALSE_WMV;
    
    // Can't do anything unless we've seen an I-Picture go by

#ifdef _WMV9AP_
    if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP)
#else
    if (pWMVDec->m_tFrmPredType == IVOP)
#endif
    {
        pWMVDec->m_fDecodedI = TRUE_WMV;
        
#ifndef WMV9_SIMPLE_ONLY
        if (pWMVDec->m_bBFrameOn) {
            if (pWMVDec->m_iNumBFrames > 0) {
                if (pWMVDec->m_iBState == -3) {
                    assert (pWMVDec->m_tFrmType != BVOP); // otherwise we are screwed!
                    pWMVDec->m_iBState = 1;
                }
#ifdef _WMV9AP_
                else if (pWMVDec->m_tFrmType == BIVOP) {
#else
                else if (pWMVDec->m_tFrmType == BVOP) {
#endif
                    // b frame coded as I
                    if (pWMVDec->m_iBState == 1) {
						if(pWMVDec->m_bCodecIsWVC1 && pWMVDec->m_IBPFlag ==1)//zou 422,fix I B I issue.
						{
							pWMVDec->m_uiNumFramesLeftForOutput = 1;
						}
						else
						{
							pWMVDec->m_bSkipThisFrame = TRUE;
							pWMVDec->m_uiNumFramesLeftForOutput = 1; // 1 frame ready to output after decoding.
							checkEndOfFrame(pWMVDec);
							pWMVDec->m_bRenderDirect = TRUE_WMV;
							pWMVDec->m_eFrameWasOutput = FrameDecOut_OutputOptional;
							return WMV_Succeeded;
						}
                    }
                    
                    if (pWMVDec->m_bPrevAnchor) {
                        swapCurrRef1Pointers (pWMVDec);
#ifndef _WMV9AP_
#                       ifdef  _MultiThread_Decode_ 
                            DecodeMultiThreads (REPEATPAD);
#                       else
                            FUNCTION_PROFILE_DECL_START(fpRep,DECODEREPEATPAD_PROFILE);
                            (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRef1Y,0,pWMVDec->m_iHeightYRepeatPad,pWMVDec->m_iWidthPrevYXExpPlusExp,TRUE,TRUE,pWMVDec->m_iWidthYRepeatPad,pWMVDec->m_iWidthYPlusExp,pWMVDec->m_iWidthPrevY);
                            (*g_pRepeatRef0UV)  (pWMVDec->m_ppxliRef1U,pWMVDec->m_ppxliRef1V,0,pWMVDec->m_iHeightUVRepeatPad,pWMVDec->m_iWidthPrevUVXExpPlusExp,TRUE,TRUE,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
                            FUNCTION_PROFILE_STOP(&fpRep)
#                       endif
#endif   //_WMV9AP_
                    }
                }
                else {
                    if (pWMVDec->m_iBState == 1) {               
						if(pWMVDec->m_bCodecIsWVC1 && pWMVDec->m_IBPFlag ==1)//zou 422
						{
							 pWMVDec->m_iBState=0;
							 swapRef0Ref1Pointers(pWMVDec);
						}
						else
						{
							pWMVDec->m_iBState=0;
							swapCurrRef1Pointers (pWMVDec);
							swapRef0Ref1Pointers(pWMVDec);
						}
#ifndef _WMV9AP_
                        if(pWMVDec->EXPANDY_REFVOP != 0) {
                            (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRef0Y,0,pWMVDec->m_iHeightYRepeatPad,pWMVDec->m_iWidthPrevYXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthYRepeatPad,pWMVDec->m_iWidthYPlusExp,pWMVDec->m_iWidthPrevY);
#                           ifndef OUTPUT_GRAY4
                                (*g_pRepeatRef0UV) (pWMVDec->m_ppxliRef0U,pWMVDec->m_ppxliRef0V,0,pWMVDec->m_iHeightUVRepeatPad,pWMVDec->m_iWidthPrevUVXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
#                           endif
                        }
#endif //_WMV9AP_
                    }
                    else {
                        if (pWMVDec->m_bPrevAnchor) {
                            swapCurrRef1Pointers (pWMVDec);
#ifndef _WMV9AP_
                            if(pWMVDec->EXPANDY_REFVOP != 0) {
                                (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRef1Y,0,pWMVDec->m_iHeightYRepeatPad,pWMVDec->m_iWidthPrevYXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthYRepeatPad,pWMVDec->m_iWidthYPlusExp,pWMVDec->m_iWidthPrevY);
#                               ifndef OUTPUT_GRAY4
                                    (*g_pRepeatRef0UV) (pWMVDec->m_ppxliRef1U,pWMVDec->m_ppxliRef1V,0,pWMVDec->m_iHeightUVRepeatPad,pWMVDec->m_iWidthPrevUVXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
#                               endif
                            }
#endif //_WMV9AP_
                        }
                        swapRef0Ref1Pointers(pWMVDec);
                    }
//#ifndef _WMV9AP_
                    if(pWMVDec->m_cvCodecVersion != WMVA)
                        pWMVDec->m_iPrevIFrame = 1;
//#endif
                    pWMVDec->m_iDroppedPFrame = 0;
                }
            }   // if (pWMVDec->m_iNumBFrames > 0)
            else
                WMVideoDecSwapCurrAndRef (pWMVDec, !pWMVDec->m_iNumBFrames);                        
        }
#endif //  WMV9_SIMPLE_ONLY
    }
    else if (!pWMVDec->m_fDecodedI) {
        checkEndOfFrame(pWMVDec);
        FUNCTION_PROFILE_STOP(&fp);
        return WMV_NoKeyFrameDecoded;
    }
   

#ifdef _WMV9AP_
    if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {
#else
    if (pWMVDec->m_tFrmPredType == IVOP) {
#endif
//#ifdef _EMB_3FRAMES_
         if(pWMVDec->m_bEMB3FrmEnabled && pWMVDec->m_tFrmType != BIVOP)
            pWMVDec->m_bRef0InRefAlt = FALSE_WMV;
//#endif


#ifndef VIDEO_MEM_SHARE_tmpVersion 
#ifdef VIDEO_MEM_SHARE
		if(pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP) {
			pWMVDec->m_pfrmCurrQ = buf_seq_ctl(&pWMVDec->m_memManageFIFO, NULL, FIFO_READ); 
			pWMVDec->m_ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
			pWMVDec->m_ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
			pWMVDec->m_ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane;
		}
#endif
#endif

		pWMVDec->m_pfrmCurrQ->m_timeStamp = pWMVDec->m_inTimeStamp;
		pWMVDec->m_pfrmCurrQ->m_frameType = pWMVDec->m_tFrmType;
        pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData;

#ifndef WMV9_SIMPLE_ONLY
        if (pWMVDec->m_bMultiresEnabled)
            HandleResolutionChange (pWMVDec);        
#endif 

#ifndef WMV9_SIMPLE_ONLY

#ifdef _WMV9AP_
        if (pWMVDec->m_bInterlaceV2) {
#ifdef PPCWMP
            tWMVStatus = WMV_UnSupportedCompressedFormat;
#else
            tWMVStatus = decodeIInterlaceV2 (pWMVDec);
#endif //PPCWMP
        }
        else if (pWMVDec->m_bXintra8)
#else
        if (pWMVDec->m_bXintra8)
#endif
            tWMVStatus = WMVideoDecDecodeI_X8 (pWMVDec);
        else if (pWMVDec->m_bYUV411) {
#ifdef PPCWMP
            tWMVStatus = WMV_UnSupportedCompressedFormat;
#else
            tWMVStatus = decodeIInterlace411 (pWMVDec);
#endif
        }
        else
#endif
            tWMVStatus = WMVideoDecDecodeI (pWMVDec); //WMVideoDecDecodeI (pWMVDec);

#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO WMVideoDecDecodeI ok");
#endif

        
#ifdef _WMV9AP_
        if (pWMVDec->m_tFrmType == IVOP)
            pWMVDec->m_PredTypePrev = IVOP;

#endif
        if (tWMVStatus != WMV_Succeeded) {
#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO WMVideoDecDecodeI error");
#endif
            FUNCTION_PROFILE_STOP(&fp);
            return tWMVStatus;
        }
                
#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO WMVideoDecDecodeClipInfo start");
#endif

       if (pWMVDec->m_cvCodecVersion != MP4S) { //m_uiFOURCCCompressed 
            if (pWMVDec->m_cvCodecVersion == MP42 || pWMVDec->m_cvCodecVersion == MP43) 
                tWMVStatus = WMVideoDecDecodeClipInfo (pWMVDec);
            if (tWMVStatus != WMV_Succeeded) {
                FUNCTION_PROFILE_STOP(&fp);
                return tWMVStatus;
            }
#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO WMVideoDecDecodeClipInfo end");
#endif


#ifdef _WMV9AP_
        if (pWMVDec->m_cvCodecVersion >= WMVA && pWMVDec->m_tFrmType != BIVOP && pWMVDec->m_iNumBFrames > 0) 
        {
            U32_WMV imbY, imbX;
            for ( imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++)
                for ( imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++)
                {
                    I32_WMV iTopLeftBlkIndex = 2*imbY*2*pWMVDec->m_uintNumMBX + imbX*2;
                    I32_WMV iBotLeftBlkIndex = iTopLeftBlkIndex + 2*pWMVDec->m_uintNumMBX;
                    pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = IBLOCKMV;
                    pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1 ] = IBLOCKMV;
                    pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] = IBLOCKMV;
                    pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1 ] = IBLOCKMV;
                    pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex] = IBLOCKMV;
                    pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + 1 ] = IBLOCKMV;
                    pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex] = IBLOCKMV;
                    pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + 1 ] = IBLOCKMV;
                    pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.x = 0;
                    pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.y = 0; 
                }
        }

		if (pWMVDec->m_iNumBFrames > 0 && pWMVDec->m_tFrmType != BIVOP)
        {
            U32_WMV imbY, imbX;
            for ( imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++)
                for ( imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++)
                    pWMVDec->m_rgmbmd1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_dctMd = INTRA;
        }
#endif

#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO WMVideoDecDecodeDataInternal 4");
#endif

        }
    }
    else { // P-VOP
        WMVideoDecAssignMotionCompRoutines (pWMVDec);
        
        if (!pWMVDec->m_bBFrameOn || pWMVDec->m_tFrmType == PVOP) {
            if (pWMVDec->m_iNumBFrames == 0) {
                WMVideoDecSwapCurrAndRef (pWMVDec, !pWMVDec->m_iNumBFrames);

#ifndef _WMV9AP_
                if (!pWMVDec->m_bMultiresEnabled || pWMVDec->m_iResIndex == pWMVDec->m_iResIndexPrev) {
                    if(pWMVDec->EXPANDY_REFVOP != 0) {
                        (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRef0Y,0,pWMVDec->m_iHeightYRepeatPad,pWMVDec->m_iWidthPrevYXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthYRepeatPad,pWMVDec->m_iWidthYPlusExp,pWMVDec->m_iWidthPrevY);
#                       ifndef OUTPUT_GRAY4
                            (*g_pRepeatRef0UV) (pWMVDec->m_ppxliRef0U,pWMVDec->m_ppxliRef0V,0,pWMVDec->m_iHeightUVRepeatPad,pWMVDec->m_iWidthPrevUVXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
#                       endif
                    }
                }
                // m_PredTypePrev = PVOP;
#else
                pWMVDec->m_PredTypePrev = PVOP;
                if ((!pWMVDec->m_bMultiresEnabled || pWMVDec->m_iResIndex == pWMVDec->m_iResIndexPrev) 
#ifdef WMV_OPT_COMBINE_LF_CC
                    && pWMVDec->m_cvCodecVersion < WMV3 
#endif
                ){
                    if(pWMVDec->EXPANDY_REFVOP != 0) {
                        (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRef0Y,0,pWMVDec->m_iHeightYRepeatPad,pWMVDec->m_iWidthPrevYXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthYRepeatPad,pWMVDec->m_iWidthYPlusExp,pWMVDec->m_iWidthPrevY, TRUE);
#                       ifndef OUTPUT_GRAY4
                            (*g_pRepeatRef0UV) (pWMVDec->m_ppxliRef0U,pWMVDec->m_ppxliRef0V,0,pWMVDec->m_iHeightUVRepeatPad,pWMVDec->m_iWidthPrevUVXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV, TRUE);
#                       endif
                    }
                }
#endif
            }
#ifndef WMV9_SIMPLE_ONLY
            else {
                if (pWMVDec->m_iBState == 1) {
					if(pWMVDec->m_bCodecIsWVC1 && pWMVDec->m_IBPFlag ==1)//zou 422
					{
						 pWMVDec->m_iBState=0;
						swapRef0Ref1Pointers(pWMVDec);
					}
					else
					{
						pWMVDec->m_iBState=0;
						swapCurrRef1Pointers (pWMVDec);
						swapRef0Ref1Pointers(pWMVDec);
					}
#ifndef _WMV9AP_
                    if (!pWMVDec->m_bMultiresEnabled || pWMVDec->m_iResIndex == pWMVDec->m_iResIndexPrev) {
                        if(pWMVDec->EXPANDY_REFVOP != 0) {
                            (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRef0Y,0,pWMVDec->m_iHeightYRepeatPad,pWMVDec->m_iWidthPrevYXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthYRepeatPad,pWMVDec->m_iWidthYPlusExp,pWMVDec->m_iWidthPrevY);
#                           ifndef OUTPUT_GRAY4
                                (*g_pRepeatRef0UV) (pWMVDec->m_ppxliRef0U,pWMVDec->m_ppxliRef0V,0,pWMVDec->m_iHeightUVRepeatPad,pWMVDec->m_iWidthPrevUVXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
#                           endif
                        }
                    }
#endif
                } else {
                    if (pWMVDec->m_bPrevAnchor) {
                        swapCurrRef1Pointers (pWMVDec);

#ifndef _WMV9AP_
                        if (!pWMVDec->m_bMultiresEnabled || pWMVDec->m_iResIndex == pWMVDec->m_iResIndexPrev) {
                            if(pWMVDec->EXPANDY_REFVOP != 0) {
                                (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRef1Y,0,pWMVDec->m_iHeightYRepeatPad,pWMVDec->m_iWidthPrevYXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthYRepeatPad,pWMVDec->m_iWidthYPlusExp,pWMVDec->m_iWidthPrevY);
#                               ifndef OUTPUT_GRAY4
                                    (*g_pRepeatRef0UV) (pWMVDec->m_ppxliRef1U,pWMVDec->m_ppxliRef1V,0,pWMVDec->m_iHeightUVRepeatPad,pWMVDec->m_iWidthPrevUVXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
#                               endif
                            }
                        }
#endif
                    }
                    swapRef0Ref1Pointers(pWMVDec);
                    
                }
            }   // if not pWMVDec->m_iNumBFrames == 0
#endif // WMV9_SIMPLE_ONLY
            
            pWMVDec->m_iDroppedPFrame = 0;

#ifdef _WMV9AP_
            pWMVDec->m_PredTypePrev = PVOP;
#endif

//#ifdef _EMB_3FRAMES_
            if(pWMVDec->m_bEMB3FrmEnabled)
                pWMVDec->m_bRef0InRefAlt = FALSE_WMV;
//#endif
            //  m_PredTypePrev = PVOP;
            
#ifndef VIDEO_MEM_SHARE_tmpVersion 
#ifdef VIDEO_MEM_SHARE
			if(pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP) {
				pWMVDec->m_pfrmCurrQ = buf_seq_ctl(&pWMVDec->m_memManageFIFO, NULL, FIFO_READ); 
				pWMVDec->m_ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
				pWMVDec->m_ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
				pWMVDec->m_ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane;
			}
#endif
#endif

		pWMVDec->m_pfrmCurrQ->m_timeStamp = pWMVDec->m_inTimeStamp;
		pWMVDec->m_pfrmCurrQ->m_frameType = pWMVDec->m_tFrmType;
        pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData;
 		

#ifndef _WMV9AP_
#ifdef WMV9_SIMPLE_ONLY
            tWMVStatus = (*pWMVDec->m_pDecodeP) (pWMVDec); //WMVideoDecDecodeP (pWMVDec);
#else
            if (pWMVDec->m_cvCodecVersion == WMV3) {
                if (pWMVDec->m_bMultiresEnabled)
                    HandleResolutionChange (pWMVDec);                
                if (pWMVDec->m_iNumBFrames)
                    CopyRefToPrevPost(pWMVDec);       

                if(pWMVDec->m_bEMB3FrmEnabled)
                {
                    if(pWMVDec->m_bAltRefFrameEnabled)
                    {
                        AdjustReconRange2AltRef (pWMVDec);
                        IntensityCompensation2AltRef(pWMVDec);
                    }
                }
                else
                {
#ifdef _WMV_TARGET_X86_
#    if (g_bSupportMMX_WMV)
                    AdjustReconRange_MMX (pWMVDec);
     else
                    AdjustReconRange (pWMVDec);
#    endif
#else
                    AdjustReconRange (pWMVDec);
#endif
                    IntensityCompensation(pWMVDec);
                }

                if (pWMVDec->m_bYUV411) {
#ifdef PPCWMP
                    tWMVStatus = WMV_UnSupportedCompressedFormat;
#else
                    tWMVStatus = decodePInterlace411 (pWMVDec);
#endif
                }
                else 
                    tWMVStatus = (*pWMVDec->m_pDecodeP) (pWMVDec); //WMVideoDecDecodeP (pWMVDec);
            } else
                tWMVStatus = (*pWMVDec->m_pDecodeP) (pWMVDec); //WMVideoDecDecodeP (pWMVDec);
#endif //  WMV9_SIMPLE_ONLY
            
#else //_WMV9AP_

            if (pWMVDec->m_cvCodecVersion >= WMV3) {
#ifndef WMV9_SIMPLE_ONLY
                if (pWMVDec->m_bMultiresEnabled)
                    HandleResolutionChange (pWMVDec);

                                                
#ifndef VIDEO_MEM_SHARE_tmpVersion 
#ifdef VIDEO_MEM_SHARE                
				if(pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP){
					if (pWMVDec->m_iNumBFrames) {
						pWMVDec->m_pfrmPostPrevQ = buf_seq_ctl(&pWMVDec->m_memManageFIFO, NULL, FIFO_READ); 
						CopyRefToPrevPost(pWMVDec);
						buf_seq_ctl(&pWMVDec->m_memManageFIFO, pWMVDec->m_pfrmPostPrevQ, FIFO_WRITE);
						pWMVDec->m_pfrmPostPrevQ->m_timeStamp = pWMVDec->m_pfrmRef0Q->m_timeStamp;
						pWMVDec->m_pfrmPostPrevQ->m_frameType = pWMVDec->m_pfrmRef0Q->m_frameType;
                        pWMVDec->m_pfrmPostPrevQ->m_UserData = pWMVDec->m_pfrmRef0Q->m_UserData;
					}
				}
#endif
#endif
				{
					if (pWMVDec->m_iNumBFrames) {
						CopyRefToPrevPost(pWMVDec);
						pWMVDec->m_pfrmPostPrevQ->m_timeStamp = pWMVDec->m_pfrmRef0Q->m_timeStamp;
						pWMVDec->m_pfrmPostPrevQ->m_frameType = pWMVDec->m_pfrmRef0Q->m_frameType;
                        pWMVDec->m_pfrmPostPrevQ->m_UserData = pWMVDec->m_pfrmRef0Q->m_UserData;
					}
				}

                if(pWMVDec->m_bEMB3FrmEnabled)
                {
                    if(pWMVDec->m_bAltRefFrameEnabled)
                    {
                        AdjustReconRange2AltRef (pWMVDec);
                        IntensityCompensation2AltRef(pWMVDec);
                    }
                }
                else
                {
                    AdjustReconRange (pWMVDec);
                    IntensityCompensation(pWMVDec);
                }
#endif

                pWMVDec->m_nPframes ++;
                if (pWMVDec->m_bInterlaceV2) {
#ifdef PPCWMP
                    tWMVStatus = WMV_UnSupportedCompressedFormat;                  
#else
                    tWMVStatus = decodePInterlaceV2 (pWMVDec);
#endif //PPCWMP
                } else if (pWMVDec->m_bYUV411) {
#ifdef WMV9_SIMPLE_ONLY
                    tWMVStatus = WMV_UnSupportedCompressedFormat;
#else
#ifdef PPCWMP
                    tWMVStatus = WMV_UnSupportedCompressedFormat;
#else
                    tWMVStatus = decodePInterlace411 (pWMVDec);
#endif
#endif
                }
                else 
                    tWMVStatus = (*pWMVDec->m_pDecodeP) (pWMVDec); // WMVideoDecDecodeP_X9_EMB() in vopdec9_wmv9_emb.c
            } else
                tWMVStatus = (*pWMVDec->m_pDecodeP) (pWMVDec); // WMVideoDecDecodeP() in frmdec_wmv.c 
			
#endif //_WMV9AP_
            if (tWMVStatus != WMV_Succeeded) {
                FUNCTION_PROFILE_STOP(&fp);
                return tWMVStatus;
            }
        }
#ifndef WMV9_SIMPLE_ONLY
        else if (pWMVDec->m_tFrmType == BVOP) {
            // B frame
            pWMVDec->m_nBframes++;
            if (pWMVDec->m_iBState == 1) {
				 if(pWMVDec->m_bCodecIsWVC1)//zou 422
				 {
					  pWMVDec->m_uiNumFramesLeftForOutput = 1;
					  pWMVDec->m_IBPFlag = 1;
				 }
				 else
				 {
					pWMVDec->m_bSkipThisFrame = TRUE;
#ifndef _WMV9AP_
					pWMVDec->m_tFrmPredType = pWMVDec->m_tFrmType;
#endif
					pWMVDec->m_uiNumFramesLeftForOutput = 1; // 1 frame ready to output after decoding.
					// ?? can I do this ??? pWMVDec->m_bRenderDirect == TRUE_WMV
					checkEndOfFrame(pWMVDec);
					pWMVDec->m_bRenderDirect = TRUE_WMV;
					pWMVDec->m_eFrameWasOutput = FrameDecOut_OutputOptional;
					return WMV_Succeeded;
				 }
            }
            
            if (pWMVDec->m_iDroppedPFrame) {
                // risky!! this means we MUST be in VBR and the frame was "skipped" not "dropped"
                pWMVDec->m_iDroppedPFrame = 0;
                if (pWMVDec->m_bPrevAnchor) {
                    // even more caution!!! means we have a long skip sequence
                    swapCurrRef1Pointers (pWMVDec);

#ifndef _WMV9AP_
                    if (!pWMVDec->m_bMultiresEnabled || pWMVDec->m_iResIndex == pWMVDec->m_iResIndexPrev) {
                        FUNCTION_PROFILE_DECL_START(fpRep,DECODEREPEATPAD_PROFILE);
                        (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRef1Y,0,pWMVDec->m_iHeightYRepeatPad,pWMVDec->m_iWidthPrevYXExpPlusExp,TRUE,TRUE,pWMVDec->m_iWidthYRepeatPad,pWMVDec->m_iWidthYPlusExp,pWMVDec->m_iWidthPrevY);
                        (*g_pRepeatRef0UV)  (pWMVDec->m_ppxliRef1U,pWMVDec->m_ppxliRef1V,0,pWMVDec->m_iHeightUVRepeatPad,pWMVDec->m_iWidthPrevUVXExpPlusExp,TRUE,TRUE,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
                        FUNCTION_PROFILE_STOP(&fpRep)
                    }
#endif
				}
				swapRef0Ref1Pointers(pWMVDec);                
                // propagate ref to current -- because these 2 were very similar which is why we VBR skipped
                ALIGNED32_FASTMEMCPY (pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliRef0Y, pWMVDec->m_iWidthPrevY * pWMVDec->m_iHeightPrevY);
                ALIGNED32_FASTMEMCPY (pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliRef0U, (pWMVDec->m_iWidthPrevY * pWMVDec->m_iHeightPrevY) >> 2);
                ALIGNED32_FASTMEMCPY (pWMVDec->m_ppxliCurrQV, pWMVDec->m_ppxliRef0V, (pWMVDec->m_iWidthPrevY * pWMVDec->m_iHeightPrevY) >> 2);
                
                // whether or not this was true before, it must be true now ...
                pWMVDec->m_bPrevAnchor = TRUE;
            }
            
            if (pWMVDec->m_bPrevAnchor) {
                swapCurrRef1Pointers (pWMVDec);    
#ifndef _WMV9AP_
                if (!pWMVDec->m_bMultiresEnabled || pWMVDec->m_iResIndex == pWMVDec->m_iResIndexPrev) {
                    if(pWMVDec->EXPANDY_REFVOP != 0) {
                        (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRef1Y,0,pWMVDec->m_iHeightYRepeatPad,pWMVDec->m_iWidthPrevYXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthYRepeatPad,pWMVDec->m_iWidthYPlusExp,pWMVDec->m_iWidthPrevY);
#                           ifndef OUTPUT_GRAY4
                            (*g_pRepeatRef0UV) (pWMVDec->m_ppxliRef1U,pWMVDec->m_ppxliRef1V,0,pWMVDec->m_iHeightUVRepeatPad,pWMVDec->m_iWidthPrevUVXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
#                           endif
                    }
                }
#endif
            }
        
#ifndef VIDEO_MEM_SHARE_tmpVersion 
#ifdef VIDEO_MEM_SHARE
			if(pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP) {
				pWMVDec->m_pfrmCurrQ = buf_seq_ctl(&pWMVDec->m_memManageFIFO, NULL, FIFO_READ); 
				pWMVDec->m_ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
				pWMVDec->m_ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
				pWMVDec->m_ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane;
			}
#endif
#endif

			pWMVDec->m_pfrmCurrQ->m_timeStamp = pWMVDec->m_inTimeStamp;
			pWMVDec->m_pfrmCurrQ->m_frameType = pWMVDec->m_tFrmType;
            pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData;

#ifdef _WMV9AP_

			pWMVDec->m_PredTypePrev = PVOP;
			if(pWMVDec->m_bEMB3FrmEnabled && pWMVDec->m_bAltRefFrameEnabled)
				{
					 AdjustReconRange2AltRef (pWMVDec);
				}

			if (pWMVDec->m_bYUV411 || pWMVDec->m_bInterlaceV2) {
	#ifdef PPCWMP
				tWMVStatus = WMV_UnSupportedCompressedFormat;
	#else
				if (pWMVDec->m_bInterlaceV2) 
					tWMVStatus = decodeBInterlaceV2 (pWMVDec);
				else
					tWMVStatus = decodeBInterlace411 (pWMVDec);
	#endif
				}
			else {
				if(pWMVDec->m_pDecodeB)
					tWMVStatus = (*pWMVDec->m_pDecodeB)(pWMVDec); 
				else
					tWMVStatus = WMV_Failed;

			}
#else
            if(pWMVDec->m_bEMB3FrmEnabled && pWMVDec->m_bAltRefFrameEnabled)
            {
                 AdjustReconRange2AltRef (pWMVDec);
            }
            if (pWMVDec->m_bYUV411) {
#ifdef PPCWMP
                tWMVStatus = WMV_UnSupportedCompressedFormat;
#else
                tWMVStatus = decodeBInterlace411 (pWMVDec);
#endif
            } else {
                tWMVStatus = (*pWMVDec->m_pDecodeB) (pWMVDec);
             //   tWMVStatus = decodeB_Deblock (pWMVDec); 
            }
#endif

            }
#endif // WMV9_SIMPLE_ONLY

            pWMVDec->m_bDisplay_AllMB = TRUE;
            pWMVDec->m_bCopySkipMBToPostBuf = TRUE; 
        } // if BVOP
        
        if (pWMVDec->m_tFrmType == PVOP)
            pWMVDec->m_iPrevIFrame = 0;
        
        if (WMV_Succeeded != tWMVStatus) {
            return tWMVStatus;
        }
        
PostDecode:
    pWMVDec->m_uiNumFramesLeftForOutput = 1; // 1 frame ready to output after decoding.
    *puiNumDecodedFrames = 1;
    pWMVDec->m_eFrameWasOutput = pWMVDec->m_bOmittedBFrame ? FrameDecOut_OutputOptional : FrameDecOut_Decoded;
        
#ifdef _WMV9AP_
    /** repeatpad - do this only when field mode is off - field mode repeatpad is done in DecodeFieldModeFrame **/
    if ((pWMVDec->m_tFrmType == PVOP || pWMVDec->m_tFrmType == IVOP) && (!pWMVDec->m_bInterlaceV2 || !pWMVDec->m_bFieldMode) 
#ifdef WMV_OPT_COMBINE_LF_CC
       && pWMVDec->m_cvCodecVersion >= WMV3
#endif
       )
        Repeatpad (pWMVDec);
#endif

    checkEndOfFrame(pWMVDec);

#ifdef VO_LOG_INFO_traceFunction
		VOLOGI("\n VO_LOG_INFO WMVideoDecDecodeDataInternal ok");
#endif

    FUNCTION_PROFILE_STOP(&fp);
    return WMV_Succeeded;
}


tWMVDecodeStatus WMVideoDecReset (HWMVDecoder hWMVDecoder)
{
    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;

    return WMVideoDecResetBState(hWMVDecoder);
}


tWMVDecodeStatus  WMVideoDecResetBState( HWMVDecoder hWMVDecoder) 
{ 
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 
    
    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);
    
    pWMVDec->m_fDecodedI = 0;
    pWMVDec->m_uiNumFramesLeftForOutput = 0;
    pWMVDec->m_eFrameWasOutput = FrameDecOut_None;
    
    if (pWMVDec->m_cvCodecVersion == WMV3)
    {
        pWMVDec->m_t = 0;
        pWMVDec->m_tFrmType = IVOP; pWMVDec->m_iBState = -3; pWMVDec->m_iPrevIFrame = 0;
        pWMVDec->m_bPrevAnchor = FALSE; pWMVDec->m_bBFrameOn = TRUE;
        pWMVDec->m_iDroppedPFrame = 0; pWMVDec->m_bIsBChangedToI = FALSE;
        // sils 20020727 
        // Some discontinuities are due to user controlled seeks and some are from the player being behind and skipping to the next key frame.
        // without knowning, we can't count this as a skipped frame as it will adapt the post processing
        // ideally, we could know the difference and count only the player behind ones...
        // pWMVDec->m_iNumFrameDropped++;
        // but we can stop omitting B frames any time we have a discontinuity 
        //pWMVDec->m_iOmitNextBFDecode = 0;
        //pWMVDec->m_iOmitNextBFOutput = 0;
        pWMVDec->m_bOmittedBFrame = FALSE;
        if (pWMVDec->m_bSpriteMode) {
            I32_WMV iSizeYplane, iSizeUVplane;
            
            iSizeYplane = (pWMVDec->m_iWidthY + pWMVDec->EXPANDY_REFVOP * 2) * (pWMVDec->m_iHeightY + pWMVDec->EXPANDY_REFVOP * 2);
            iSizeUVplane = ((pWMVDec->m_iWidthUV) + pWMVDec->EXPANDUV_REFVOP * 2) * ((pWMVDec->m_iHeightUV)+ pWMVDec->EXPANDUV_REFVOP * 2);
            
            ALIGNED32_FASTMEMCLR (pWMVDec->m_ppxliCurrQY,  0,iSizeYplane);
            ALIGNED32_FASTMEMSET (pWMVDec->m_ppxliCurrQU,128,iSizeUVplane);
            ALIGNED32_FASTMEMSET (pWMVDec->m_ppxliCurrQV,128,iSizeUVplane);
            
            ALIGNED32_FASTMEMCLR (pWMVDec->m_ppxliRef0Y,  0,iSizeYplane);
            ALIGNED32_FASTMEMSET (pWMVDec->m_ppxliRef0U,128,iSizeUVplane);
            ALIGNED32_FASTMEMSET (pWMVDec->m_ppxliRef0V,128,iSizeUVplane);
        }
    }
    return tWMVStatus;
}

Void_WMV PreGetOutput (tWMVDecInternalMember *pWMVDec)
{
    Bool_WMV bChangedNonPostToPostBuffer = FALSE;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(PreGetOutput);

    if(pWMVDec->m_bCodecIsWVC1){
		pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmCurrQ;
    }
    // Most generic case (normal I/P codec), lest we forget ...
    if (!(pWMVDec->m_iNumBFrames > 0 && (pWMVDec->m_bSkipThisFrame || pWMVDec->m_iDroppedPFrame || pWMVDec->m_bOmittedBFrame))) {
        if (pWMVDec->m_bRenderFromPostBuffer)
            pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmPostQ;
        else
            pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmCurrQ;
    }

#ifdef _WMV9AP_
    if (pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP) {
#else
    if (pWMVDec->m_tFrmPredType != BVOP) {
#endif
        if (pWMVDec->m_t > 1 && pWMVDec->m_bOldRenderFromPostBuffer == FALSE && pWMVDec->m_bRenderFromPostBuffer == TRUE)
            bChangedNonPostToPostBuffer = TRUE;
        pWMVDec->m_bOldRenderFromPostBuffer = pWMVDec->m_bRenderFromPostBuffer;
    }

    if (pWMVDec->m_cvCodecVersion != WMVA) {
        pWMVDec->m_bSeqFrameWidthOrHeightChanged = FALSE_WMV;
    }
    
    if (pWMVDec->m_iNumBFrames != 0 && (pWMVDec->m_t == 1)
        && (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == PVOP)) {
        if( pWMVDec->m_bRenderFromPostBuffer) {
            SwapPostAndPostPrev(pWMVDec);                  // store this frame away
            pWMVDec->m_bPrevRefInPrevPost = TRUE;
            pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmPostPrevQ;   // and display it
        }
        else
            pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmCurrQ;
    }
    else if (pWMVDec->m_iNumBFrames != 0 && (pWMVDec->m_bSkipThisFrame || pWMVDec->m_iDroppedPFrame || pWMVDec->m_bOmittedBFrame)) {
        // don't disturb anything if we are skipping it intentionally ...
        // in the case of dropped P, we show the prev. P
        // but no buffer swapping cos nothing came in
        if (pWMVDec->m_iDroppedPFrame)  
        {
                    
            if (pWMVDec->m_tFrmType == SKIPFRAME && pWMVDec->m_bPrevAnchor) {
                if (pWMVDec->m_bRenderFromPostBuffer) {
                    pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmPostQ;
                } else {
                    pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmCurrQ;
                }
            } else    // in this case B frames have swapped buffers ...
                pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmRef0Q;
        }
    
    }
    else if (pWMVDec->m_iNumBFrames != 0 // anchor frame case ...
#ifdef _WMV9AP_
             && (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == PVOP)) {
#else
             && (pWMVDec->m_tFrmPredType == IVOP || pWMVDec->m_tFrmPredType == PVOP)) {
#endif
        if (pWMVDec->m_bPrevRefInPrevPost) {
            pWMVDec->m_pvopcRenderThis =  pWMVDec->m_pfrmPostPrevQ;    // render the last anchor
            pWMVDec->m_bPrevRefInPrevPost = FALSE;
            if (pWMVDec->m_bRenderFromPostBuffer)
            {
                SwapPostAndPostPrev (pWMVDec);
                pWMVDec->m_bPrevRefInPrevPost = TRUE;
            }
        }
        else if (pWMVDec->m_bRenderFromPostBuffer) {
            SwapPostAndPostPrev(pWMVDec);              // store this frame away if it wasn't dropped
            pWMVDec->m_bPrevRefInPrevPost = TRUE;
            if (bChangedNonPostToPostBuffer)    // reset this to the correct post buf
                pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmRef0Q;
            else
                pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmPostQ;   
        }
        else { // render from PostPrev iff there was a global change (luminance/xrange)
            pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmRef0Q;    // render the last anchor
        }
    }

	if(pWMVDec->m_outputMode){
		pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmCurrQ;
	}
}

extern  const I32_WMV g_iBInverse[];

I32_WMV DecodeEntryPointHeader (tWMVDecInternalMember *pWMVDec, I32_WMV *piPIC_HORIZ_SIZE, I32_WMV *piPIC_VERT_SIZE)
{
    I32_WMV i;
    I32_WMV ihrd_fullness_N;
    U8_WMV pucHrdFullness[32];



    pWMVDec->m_bBrokenLink = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bClosedEntryPoint = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bPanScanPresent = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bRefDistPresent = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bLoopFilter = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bUVHpelBilinear  = BS_getBits(pWMVDec->m_pbitstrmIn, 1);   //zou --->pState->sPosition.FastUVMC = (FLAG)Value;
    pWMVDec->m_bExtendedMvMode = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_iDQuantCodingOn = BS_getBits(pWMVDec->m_pbitstrmIn, 2);
    pWMVDec->m_bXformSwitch = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bSequenceOverlap = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    if(pWMVDec->m_bCodecIsWVC1){
		pWMVDec->vo_OverLapFilter = pWMVDec->m_bSequenceOverlap;
    }
    pWMVDec->m_bExplicitSeqQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    if (pWMVDec->m_bExplicitSeqQuantizer)
        pWMVDec->m_bUse3QPDZQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    else 
        pWMVDec->m_bExplicitFrameQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1);


    //printf("pWMVDec->m_bHRD_PARAM_FLAG =  %d \n", pWMVDec->m_bHRD_PARAM_FLAG); 
    if (pWMVDec->m_bHRD_PARAM_FLAG) {
#ifdef DUMPLOG
        if(pWMVDec->m_pFileLog && pWMVDec->m_ihrd_num_leaky_buckets) {
            fprintf(pWMVDec->m_pFileLog, "HRD ");
        }
#endif
        for (i = 0; i < pWMVDec->m_ihrd_num_leaky_buckets; i++){
            ihrd_fullness_N = BS_getBits(pWMVDec->m_pbitstrmIn, 8) + 1;
            pucHrdFullness[i] = (U8_WMV)(ihrd_fullness_N - 1);
#ifdef DUMPLOG
            if(pWMVDec->m_pFileLog) {
            fprintf(pWMVDec->m_pFileLog, "%d ", ihrd_fullness_N);
            }
#endif
        }
    }

    if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) {
        // New coded size 
        pWMVDec->m_iFrmWidthSrc  = 2 * BS_getBits(pWMVDec->m_pbitstrmIn, 12) + 2;
        pWMVDec->m_iFrmHeightSrc   = 2 * BS_getBits(pWMVDec->m_pbitstrmIn, 12) + 2;
        if (piPIC_HORIZ_SIZE != NULL && piPIC_VERT_SIZE != NULL) {
            *piPIC_HORIZ_SIZE  = pWMVDec->m_iFrmWidthSrc;
            *piPIC_VERT_SIZE   = pWMVDec->m_iFrmHeightSrc;
        }

#ifdef DUMPLOG
        if(pWMVDec->m_pFileLog) {
        fprintf(pWMVDec->m_pFileLog,"\nEncSize %d %d\n", pWMVDec->m_iFrmWidthSrc, pWMVDec->m_iFrmHeightSrc);
        }
#endif //DUMPLOG



#ifdef _DUMPFRAME_
        //close previous dump file and create a new one
        fclose(fpDumpDec);
        g_iNewSeqCounter ++;
        Char cDumpFileName[80] = "dumpdec";
        Char cIndex[10];
        strncat(cDumpFileName, _itoa(g_iNewSeqCounter, cIndex, 10), 2);
        strncat(cDumpFileName, ".yuv", 4);
        fpDumpDec = fopen(cDumpFileName, "wb");
        if (!fpDumpDec) {
            //printf("Failed to create dump file %s.\n", cDumpFileName);
            return ICERR_ERROR;
        }
#endif

    }
    else {
        // Use max. coded size 
        pWMVDec->m_iFrmWidthSrc  = pWMVDec->m_iMaxFrmWidthSrc;
        pWMVDec->m_iFrmHeightSrc   = pWMVDec->m_iMaxFrmHeightSrc;
    }


    if (pWMVDec->m_bExtendedMvMode)
        pWMVDec->m_bExtendedDeltaMvMode = BS_getBits(pWMVDec->m_pbitstrmIn, 1);



    pWMVDec->m_bRangeRedY_Flag = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    //printf("pWMVDec->m_bRangeRedY_Flag =  %d \n", pWMVDec->m_bRangeRedY_Flag);
    if (pWMVDec->m_bRangeRedY_Flag) {
        pWMVDec->m_iRangeRedY = BS_getBits(pWMVDec->m_pbitstrmIn, 3) + 1;
    } else {
        pWMVDec->m_iRangeRedY = 0;
    }
    pWMVDec->m_bRangeRedUV_Flag = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    if (pWMVDec->m_bRangeRedUV_Flag) {
        pWMVDec->m_iRangeRedUV = BS_getBits(pWMVDec->m_pbitstrmIn, 3) + 1;
    } else {
        pWMVDec->m_iRangeRedUV = 0;
    }


#ifdef DUMPLOG
    if(pWMVDec->m_pFileLog) {
    fprintf(pWMVDec->m_pFileLog, "RANGE %d %d %d %d\n", 
            pWMVDec->m_bRangeRedY_Flag, pWMVDec->m_iRangeRedY, pWMVDec->m_bRangeRedUV_Flag, pWMVDec->m_iRangeRedUV);
    }
#endif //DUMPLOG

#ifdef DUMPLOG
    if(pWMVDec->m_pFileLog) {
        fprintf(pWMVDec->m_pFileLog, "FLAGS %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
            pWMVDec->m_bBrokenLink,
            pWMVDec->m_bClosedEntryPoint,
            pWMVDec->m_bPanScanPresent,
            pWMVDec->m_bRefDistPresent,
            pWMVDec->m_bLoopFilter,
            pWMVDec->m_bUVHpelBilinear,
            pWMVDec->m_bExtendedMvMode,
            pWMVDec->m_iDQuantCodingOn,
            pWMVDec->m_bXformSwitch,
            pWMVDec->m_bSequenceOverlap,
            pWMVDec->m_bExplicitSeqQuantizer,
            pWMVDec->m_bUse3QPDZQuantizer,
            pWMVDec->m_bExplicitFrameQuantizer,
            pWMVDec->m_bExtendedDeltaMvMode);
    }
#endif //DUMPLOG


    if (BS_invalid (pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }

    /*
    if (pWMVDec->m_iDumpFrameDecLen == 1 && pWMVDec->m_fpFrameLenDec) {
        fprintf(pWMVDec->m_fpFrameLenDec, "    Range Redux: %d, %d\n", pWMVDec->m_iRangeRedY, pWMVDec->m_iRangeRedUV);
        fflush(pWMVDec->m_fpFrameLenDec);
    }
    */

    pWMVDec->m_iBFrameReciprocal = g_iBInverse[pWMVDec->m_iNumBFrames];
    pWMVDec->m_bExplicitQuantizer = pWMVDec->m_bExplicitSeqQuantizer || pWMVDec->m_bExplicitFrameQuantizer;
    SetMVRangeFlag (pWMVDec, 0);

    return ICERR_OK;

}

Void_WMV SetSliceWMVA (tWMVDecInternalMember *pWMVDec, Bool_WMV bSliceWMVA, U32_WMV uiFirstMBRow, U32_WMV uiNumBytesSlice, Bool_WMV bSecondField)
{
    pWMVDec->m_bSliceWMVA = bSliceWMVA;
    if (pWMVDec->m_bSliceWMVA == FALSE) {
        pWMVDec->m_puiFirstMBRowOfSlice[0] = 0;
        pWMVDec->m_uiNumSlicesWMVA = 1;
        memset (pWMVDec->m_puiFirstMBRowOfSlice, 0, sizeof (U32_WMV) * pWMVDec->m_iSliceBufSize);
        memset (pWMVDec->m_puiNumBytesOfSliceWMVA, 0, sizeof (U32_WMV) * pWMVDec->m_iSliceBufSize);
        memset (pWMVDec->m_pbStartOfSliceRowOrig, 0, sizeof(Bool_WMV)* pWMVDec->m_iSliceBufSize);
        pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;
        pWMVDec->m_uiCurrSlice = 0;
        pWMVDec->m_uiNumBytesFrameHeaderSlice = 0;
        pWMVDec->m_uiNumBytesFieldHeaderSlice = 0;
        return;
    }
    
    assert(((I32_WMV)uiFirstMBRow) < pWMVDec->m_iSliceBufSize);

    pWMVDec->m_pbStartOfSliceRowOrig[uiFirstMBRow] = 1;

    if (bSecondField) {
        pWMVDec->m_puiFirstMBRowOfSlice[pWMVDec->m_uiNumSlicesWMVA + 1] = uiFirstMBRow;
        pWMVDec->m_puiNumBytesOfSliceWMVA[pWMVDec->m_uiNumSlicesWMVA + 1] = uiNumBytesSlice;
    }
    else {
        pWMVDec->m_puiFirstMBRowOfSlice[pWMVDec->m_uiNumSlicesWMVA] = uiFirstMBRow;
        pWMVDec->m_puiNumBytesOfSliceWMVA[pWMVDec->m_uiNumSlicesWMVA] = uiNumBytesSlice;
    }
    pWMVDec->m_uiNumSlicesWMVA++;
}

tWMVDecodeStatus WMVideoDecGetSrcResolution(HWMVDecoder hWMVDecoder, 
            I32_WMV * piEncodedDisplaySizeX, I32_WMV * piEncodedDisplaySizeY)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 
    I32_WMV iWidth, iHeight;


    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);

    if(pWMVDec->m_bSeqFrameWidthOrHeightChanged == TRUE && pWMVDec->m_iNumBFrames > 0 && pWMVDec->m_t > 1)
    {
        iWidth  = pWMVDec->m_iFrmWidthSrcOld;
        iHeight = pWMVDec->m_iFrmHeightSrcOld;

    }
    else
    {
        iWidth  = pWMVDec->m_iFrmWidthSrc;
        iHeight = pWMVDec->m_iFrmHeightSrc;

    }

    if(piEncodedDisplaySizeX != NULL)
    {
        *piEncodedDisplaySizeX = iWidth;
    }

    if(piEncodedDisplaySizeY != NULL)
    {
        *piEncodedDisplaySizeY = iHeight;
    }

    return tWMVStatus;

}

// *****************************************************************************

tWMVDecodeStatus WMVideoDecGetOutputInternal (
    HWMVDecoder                 hWMVDecoder,
    U32_WMV                     uiFOURCCOutput,
    U16_WMV                     uiBitsPerPixelOutput,
    U32_WMV                     uiWidthBMPOutput,
    tWMVDecodeDispRotateDegree  iDispRotateDegree,
    U8_WMV*                     pucDecodedOutput
)
{
    FUNCTION_PROFILE(fp)
    //tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 
    
    
    if (pucDecodedOutput) {
		return WMV_Succeeded;
	}

    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;

    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);
	
    if (WMVideoIsSpriteMode (pWMVDec))
        pWMVDec->m_bRenderFromPostBuffer = TRUE_WMV;
    else
        pWMVDec->m_bRenderFromPostBuffer = FALSE_WMV;

#ifndef WMV9_SIMPLE_ONLY
    if (pWMVDec->m_bPreProcRange) {

#ifdef _WMV9AP_
        if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {
#else
        if (pWMVDec->m_tFrmPredType == IVOP) {
#endif
            if (pWMVDec->m_iNumBFrames == 0)
                pWMVDec->m_iReconRangeState = pWMVDec->m_iRangeState;
            else if (pWMVDec->m_tFrmType != BVOP) {
                if (pWMVDec->m_iReconRangeState == 0 && pWMVDec->m_iRangeState == 1)
                    CopyRefToPrevPost (pWMVDec); // possible unnecessary copy for now - but cleaner implementation
//#ifdef _EMB_3FRAMES_
            if(pWMVDec->m_bEMB3FrmEnabled) {
                if(pWMVDec->m_bAltRefFrameEnabled) {
                    AdjustReconRange2AltRef (pWMVDec);
                }
            }
            else
//#else
                AdjustReconRange (pWMVDec);
//#endif
            }
        }
        if (pWMVDec->m_iRangeState != 0) {
            if (pWMVDec->m_bRenderFromPostBuffer == FALSE) {
                CopyCurrToPost (pWMVDec);
                pWMVDec->m_bRenderFromPostBuffer = TRUE;
            }
            AdjustDecRange (pWMVDec);
        } 						
    }
    
    if (pWMVDec->m_bMultiresEnabled && pWMVDec->m_iResIndex != 0) {
        I32_WMV iSaveIndexPrev;
        I32_WMV iSaveIndex;
        
        // If current resolution is different than full, reset the video frame 
        // dimensions to full resolution. When we parse the frame header we
        // always assume that the dimension parameters are full res.
        ResetParams(pWMVDec, 0);
        ResetDecoderParams(pWMVDec);
        iSaveIndexPrev = pWMVDec->m_iResIndexPrev;
        iSaveIndex = pWMVDec->m_iResIndex;
        pWMVDec->m_iResIndexPrev = pWMVDec->m_iResIndex;
        pWMVDec->m_iResIndex = 0;

        if (pWMVDec->m_bRenderFromPostBuffer ) {
            UpsampleRefFrame(pWMVDec,
                pWMVDec->m_ppxliPostQY + pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevYXExpPlusExp, 
                pWMVDec->m_ppxliPostQU + pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevUVXExpPlusExp, 
                pWMVDec->m_ppxliPostQV + pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevUVXExpPlusExp, 
                pWMVDec->m_ppxliMultiresY + pWMVDec->m_iWidthPrevYXExpPlusExp,
                pWMVDec->m_ppxliMultiresU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                pWMVDec->m_ppxliMultiresV + pWMVDec->m_iWidthPrevUVXExpPlusExp
                );            
            SwapMultiresAndPost(pWMVDec);
        }
        else {
            UpsampleRefFrame(pWMVDec,
                pWMVDec->m_ppxliCurrQY + pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevYXExpPlusExp, 
                pWMVDec->m_ppxliCurrQU + pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevUVXExpPlusExp, 
                pWMVDec->m_ppxliCurrQV + pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevUVXExpPlusExp, 
                pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp,
                pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp
                );
        }
        pWMVDec->m_iResIndexPrev = iSaveIndexPrev;
        pWMVDec->m_iResIndex = iSaveIndex;
        pWMVDec->m_bRenderFromPostBuffer = TRUE_WMV;
    }
    
    if (pWMVDec->m_iNumBFrames > 0) {
#ifndef _WMV9AP_
        pWMVDec->m_tFrmPredType = pWMVDec->m_tFrmType;
        pWMVDec->m_bPrevAnchor = (pWMVDec->m_tFrmType != BVOP) || (pWMVDec->m_bOmittedBFrame && pWMVDec->m_tFrmTypePriorFrame != BVOP);
   
#else
        pWMVDec->m_bPrevAnchor = (pWMVDec->m_tFrmType != BVOP) || (pWMVDec->m_bOmittedBFrame && pWMVDec->m_tFrmTypePriorFrame != BVOP);
#endif
    }
#endif //WMV9_SIMPLE_ONLY
    pWMVDec->m_bRefreshBMP = TRUE_WMV; // repaint next frame if this frame is NULL
    PreGetOutput(pWMVDec);     // do all the necessary buffer swapping here ...
    
    pWMVDec->m_uiNumFramesLeftForOutput--;
    
    DEBUG_PROFILE_FRAME_GET_OUTPUT_STOP;
    return WMV_Succeeded;
}


// ===========================================================================
// private functions, inits
// ===========================================================================

tWMVDecodeStatus WMVInternalMemberInit (
    tWMVDecInternalMember *pWMVDec,
    U32_WMV uiUserData,
    U32_WMV uiFOURCCCompressed,
    Float_WMV fltFrameRate,
    Float_WMV fltBitRate,
    I32_WMV iWidthSource,
    I32_WMV iHeightSource,
    I32_WMV iPostFilterLevel,
    tWMVDecodeDispRotateDegree iDispRotateDegree
    ) 
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    I32_WMV iFrameRate = (I32_WMV) fltFrameRate;

    pWMVDec->m_uiUserData = uiUserData;
    tWMVStatus = VodecConstruct (pWMVDec, uiFOURCCCompressed, iFrameRate, iWidthSource, iHeightSource, pWMVDec -> m_pInputBitstream );
    if (tWMVStatus != WMV_Succeeded)
        return tWMVStatus;

//#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)
//    g_InitDstDitherMap_WMV ();
//#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)

    pWMVDec->m_uiNumFramesLeftForOutput = 0;
    pWMVDec->m_uiFOURCCCompressed = uiFOURCCCompressed;
    pWMVDec->m_fltFrameRate = fltFrameRate;
    pWMVDec->m_fltBitRate = fltBitRate;
    pWMVDec->m_iPostFilterLevel = iPostFilterLevel;
#   ifdef _SUPPORT_POST_FILTERS_ 
        if (pWMVDec->m_iPostFilterLevel > _TOP_POST_FILTER_LEVEL_) 
            pWMVDec->m_iPostFilterLevel = _TOP_POST_FILTER_LEVEL_;
        else if (pWMVDec->m_iPostFilterLevel < -1)
            pWMVDec->m_iPostFilterLevel = -1;
#   endif // _SUPPORT_POST_FILTERS_ 
    pWMVDec->m_iDispRotateDegree = iDispRotateDegree;
    if (pWMVDec->m_iDispRotateDegree != WMV_DispRotate0 && pWMVDec->m_iDispRotateDegree != WMV_DispFlip)
        pWMVDec->m_iDispRotateDegree = WMV_DispRotate0;

    // size info
    pWMVDec->m_iWidthSource = pWMVDec->m_iFrmWidthSrc;
    pWMVDec->m_iHeightSource = pWMVDec->m_iFrmHeightSrc;

    pWMVDec->m_iWidthInternal = pWMVDec->m_iWidthPrevY;
    pWMVDec->m_iWidthInternalUV = pWMVDec->m_iWidthPrevUV; 
    pWMVDec->m_iHeightInternal = pWMVDec->m_iHeightPrevY;
    pWMVDec->m_iHeightInternalUV = pWMVDec->m_iHeightPrevUV;
    pWMVDec->m_iWidthInternalTimesMB = pWMVDec->m_iWidthInternal * MB_SIZE;
    pWMVDec->m_iWidthInternalUVTimesBlk = pWMVDec->m_iWidthInternalUV * BLOCK_SIZE;
    pWMVDec->m_bSizeMBAligned = (pWMVDec->m_iWidthSource == pWMVDec->m_iWidthInternal && pWMVDec->m_iHeightSource == pWMVDec->m_iHeightInternal);

#if defined(OUTPUT_ALL_FORMATS) || defined(OUTPUT_RGB12) || defined(OUTPUT_GRAY4) || defined(OUTPUT_RGB16)
#ifdef _ARM_
    pWMVDec->m_iXscalePrefechMBDistY = (15 * pWMVDec->m_iWidthInternal) + 16 + (2 * pWMVDec->EXPANDY_REFVOP);
    pWMVDec->m_iXscalePrefechMBDistUV = (7 * pWMVDec->m_iWidthInternalUV) + 8 + (2 * pWMVDec->EXPANDUV_REFVOP);                
#endif
#endif

    pWMVDec->m_uiNumMBX = pWMVDec->m_iWidthY >> 4; // divided by 16, MB size
    pWMVDec->m_uiNumMBY = pWMVDec->m_iHeightY >> 4;
    pWMVDec->m_uiNumMB = pWMVDec->m_uiNumMBX * pWMVDec->m_uiNumMBY;

    // tables
    pWMVDec->m_rgiClapTabDec = &(g_rgiClapTabDecWMV [384]); // 384 + 256 + 384 = 1024 (256 is the valid region).

#ifdef _WMV_TARGET_X86_
    // Read the cpu speed now while we are not under real-time contraints
    g_CPUClock ();
#endif


#ifdef DYNAMIC_EDGEPAD
    tWMVStatus = edgePadInit(pWMVDec, &pWMVDec->m_tEdgePad );
#endif

    pWMVDec->m_hAsyncProcess = NULL;
    pWMVDec->m_hSyncEvent = NULL;
    pWMVDec->m_tWMVDecodeStatus = tWMVStatus;    // Decode status from last async function

    return tWMVStatus;
}


tWMVDecodeStatus WMVInternalMemberDestroy (tWMVDecInternalMember *pWMVDec)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO WMVInternalMemberDestroy 1");
#endif

#ifdef DYNAMIC_EDGEPAD
    edgePadDestroy(&pWMVDec->m_tEdgePad);
#endif
        tWMVStatus = VodecDestruct(pWMVDec);
    return tWMVStatus;
}

#ifdef VIDEO_MEM_SHARE
tYUV420Frame_WMV*  buf_seq_ctl(FIFOTYPE *priv, tYUV420Frame_WMV* img , const U32_WMV flag)
{
    tYUV420Frame_WMV *temp = NULL;
    U32_WMV w_idx = 0;

    w_idx = priv->w_idx;    /* Save the old index before proceeding */
    if (flag == FIFO_WRITE){ /*write data*/
		if(img) {
			/* Save it to buffer */
			if (((priv->w_idx + 1) % FIFO_NUM) == priv->r_idx) {
				/* Adjust read index since buffer is full */
				/* Keep the latest one and drop the oldest one */
				priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
			}
			priv->img_seq[priv->w_idx] = img;
			priv->w_idx = (priv->w_idx + 1) % FIFO_NUM;
		}
    }else{/*read data*/
		if ((priv->r_idx == w_idx)){
            return NULL;/*there is no free buffer*/
		}else{
            temp = priv->img_seq[priv->r_idx];
            priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
        }
    }
	return temp;
}

/* After flush*/
void  reset_buf_seq(tWMVDecInternalMember *pWMVDec)
{
	FIFOTYPE *priv = &pWMVDec->m_memManageFIFO;  

	priv->r_idx = 0;
	priv->w_idx = pWMVDec->m_frameBufNum;
}
#endif

#ifdef _SUPPORT_POST_FILTERS_

// define ADAPT_POSTLEVEL_TUNE to determine initialization values for fltNeededQPC4Level[...] and other tuning paramters
//#define ADAPT_POSTLEVEL_TUNE

// define FORCE_ADAPT_POSTLEVEL_MS to get adaption even if it is not needed
//#define FORCE_ADAPT_POSTLEVEL_MS 4000

#ifdef ADAPT_POSTLEVEL_TUNE
    static __int64 g_i64QFCPostLevel[_TOP_POST_FILTER_LEVEL_+1];
    static int     g_iMaxQFCPostLevel[_TOP_POST_FILTER_LEVEL_+1];
    static double  g_dSumSquaresQFCPostLevel[_TOP_POST_FILTER_LEVEL_+1];
    static int     g_iCountPostLevel[_TOP_POST_FILTER_LEVEL_+1];
#endif

//#define ANNOTATE_LEVEL 6
#if (defined(PROFILE_FRAMES) || defined(PROFILE_FRAMES_TERSE)) && defined(ANNOTATE_LEVEL)
    void WMVFPLogAnnotate(const int iRegLevel, const void* const pAnnotate,const void* const pAppend, const int iT, const void* const pTail) {
        if ( ANNOTATE_LEVEL >= iRegLevel ) {
            DEBUG_PROFILE_FRAME_ANNOTATE(pAnnotate,pAppend,iT,pTail); 
        }
    }
#   define ANNOTATE_IF_REGKEY(iRegLevel,szAnnotate,szAppend,iT,szTail) WMVFPLogAnnotate((iRegLevel),szAnnotate,szAppend,(iT),szTail);
#   pragma warning ( disable : 4022 )
#else   // defined(PROFILE_FRAMES) || defined(PROFILE_FRAMES_TERSE)
#   define ANNOTATE_IF_REGKEY(iRegLevel,szAnnotate,szAppend,iT,szTail) 
#endif  // defined(PROFILE_FRAMES) || defined(PROFILE_FRAMES_TERSE)

#if defined(PROFILE_FRAMES) || defined(PROFILE_FRAMES_TERSE)
#   define PROFILE_FRAMES_ONLY(a) a
#else // defined(PROFILE_FRAMES) || defined(PROFILE_FRAMES_TERSE)
#   define PROFILE_FRAMES_ONLY(a)
#endif // defined(PROFILE_FRAMES) || defined(PROFILE_FRAMES_TERSE)


void initAdaptPostLevel(tWMVDecInternalMember *pWMVDec) {
    __int64 i64QPCFrequency = 0;
    I32_WMV   i, iCPUMHz, iCPUFeatures, iFrameRate;
    Float_WMV fltScalebySizeMHzQPC;
    // Typically we can not expect to get more than 40% of the machine without getting skipped frames
    const I32_WMV  iVideoCPUNumerator = 1, iVideoCPUDemoninator = 2;// go for 50% because we are greedily optimistic :O)...
#   if defined(_ARM_)
        // 0.2 SA1110 IPaq             progressive 28000 38000
        // 0.4 XScale Asus             progressive 38000 48000
        // in other words, PXA255 takes 1.25 times the QPC cyles to get this done but has 4X the cycles, so is essentially 3.2X faster
        static const I32_WMV iNeededQPC4LevelProgressive[5] = { 0, 36000, 46000, 56000, 60000 }; 
        //  PXA255 (ARMv5 numbers)  multiply by 10/8
#   else // !defined(_ARM_)
        // Empirical data used to determine conservative model parameters
        // 3.2G P4                         progressive 27000 34000  43000  48000
        // 2.2G P4  interlace 13000 16000; progressive 24000 30500  39300  46883
        // 1.8G AMD interlace 16200 18378; progressive 22000 27000  34000  39000
        // 1.0G P3  interlace 13000 15000; progressive 20000 25000  34000  38500
        // 0.2G P2/1.2 erlace 13000 16000; progressive 21000 28000  43000  49000
        // 0.2G P2/raw erlace 16000 19000; progressive 25000 33000  52000  59000 (mmx only - these get divided by 1.2)
        // 0.4G K6  interlace 14600 16500; progressive 22000 27000  37000  42000
        // 0.4G K6  interlace 17500 19800; progressive 26000 32000  44000  50000 (mmx only - these get divided by 1.2)
        // The via C3 is known to have bad perf per clock cycle.  What's a guy to do but skip some frames in the beginning?
        // 0.8G viaC3 terlace 39000 43000; progressive 59000 80000 107000 117000 (mmx only - these get divided by 1.2)
        // 0.8G Transmeta uses MMX only    progressive 26240 36169  64899  73002 (mmx only - these get divided by 1.2)
        // Some Mac Numbers indicating it is OK to use x86 numbers for the Mac for progressive but not interlace.
        // 0.45G G4 interlace 20000 26000; progressive 19000 26000  33000 39000
        // 0.73G G4 interlace 27000 30000; progressive 27000 33000  44000 51000
        // 0.87G G4 interlace 24000 30000; progressive 23000 30000  38000 45000
        static const I32_WMV iNeededQPC4LevelProgressive[5] = { 0, 28000, 34000, 44000, 50000 }; 
#   endif // !defined(_ARM_)
#   if defined(_ARM_)
        static const I32_WMV iNeededQPC4LevelInterlace[5]   = { 0, 26000, 33000, 40000, 43000 };  // Only vertical edges get filtered
#   elif defined(macintosh)
        static const I32_WMV iNeededQPC4LevelInterlace[5]   = { 0, 27000, 30000, 30000, 30000 };  
#   else 
        static const I32_WMV iNeededQPC4LevelInterlace[5]   = { 0, 20000, 23000, 23000, 23000 };
#   endif

#   ifdef _WMV_TARGET_X86_
        if (g_SupportMMX())
            _asm emms; //Make sure floating point calc works for building the table
#   endif // _WMV_TARGET_X86_

    pWMVDec->m_bOK2AdaptPostLevel = 1;
    pWMVDec->m_i64QPCDecode = 0;
    pWMVDec->m_i64QPCPost = 0;
    pWMVDec->m_bInTimeDecode = 0;
    pWMVDec->m_bInTimePost = 0;
    pWMVDec->m_iCountFrames = 0;
    pWMVDec->m_iNumFrameDropped = 0;
    pWMVDec->m_iCountSkippedFrames = 0;
    pWMVDec->m_iPostLevelPrior2DownAdapt = -1;
    pWMVDec->m_dwTickCountWhenAdaptedDownPostLevel = (pWMVDec->m_dwTickCountWhenLastUpdatedRegKey = GetTickCount()) - 150;

#   ifdef macintosh
        i64QPCFrequency = 1000000000;
        pWMVDec->m_iQPCPerMS = 1000000000/1000;  // nanoseconds to milliseconds
#   else
        if ( QueryPerformanceFrequency( (LARGE_INTEGER *) &i64QPCFrequency )  == 0 )
            pWMVDec->m_bOK2AdaptPostLevel = 0;
        pWMVDec->m_iQPCPerMS = (I32_WMV)(i64QPCFrequency/1000);     // FYI: 3686 for XScale PXA255.
#   endif // not macintosh

    iFrameRate = (I32_WMV)(pWMVDec->m_iFrameRate > 0 ? pWMVDec->m_iFrameRate : 30);
    pWMVDec->m_iQPC2DecodeAvgFrame =  (I32_WMV)( i64QPCFrequency * iVideoCPUNumerator / ( iVideoCPUDemoninator * iFrameRate ) );

    pWMVDec->m_iMSPerFrame = 1000/iFrameRate;
    if ( pWMVDec->m_iMSPerFrame <= 0 )
        pWMVDec->m_iMSPerFrame = 33;

    // This CPU MHz is used only to adjust the prediction of how much Post Processing can get done.
    // So it can get adjusted in the registry to account for slower or faster processors than the ones listed.
    iCPUMHz = prvWMVAccessRegistry(ADAPTMHZ, 'r', 0);
    if ( iCPUMHz < 100 || 32000 < iCPUMHz ) {
#       if defined(_XSC_) && defined(WMV_OPT_COMMON_ARM)
            iCPUMHz =  IsProcessorFeaturePresentWMV(WM_PF_ARM_INTEL_XSCALE) ? 400 : ( IsProcessorFeaturePresentWMV(WM_PF_ARM_V4) ? 206 : 100 );
#       elif defined(WMV_OPT_COMMON_ARM)
            iCPUMHz = IsProcessorFeaturePresentWMV(WM_PF_ARM_V4)  ? 206 : 100;
#       elif defined(_WMV_TARGET_X86_)
            iCPUMHz = g_CPUClock();
#       elif defined(macintosh)
            iCPUMHz = g_MacCPUClock();
#       else
            iCPUMHz = 100;  // conservative value which should be OK for even SmartPhones running at 70-120 MHz.
#       endif
    }
    if ( iCPUMHz < 100 ) {
#       if defined(_WMV_TARGET_X86_) && defined(UNDER_CE)
            iCPUMHz = 800;
#       elif defined(_WMV_TARGET_X86_)
            iCPUMHz = 1200;
#       else
            iCPUMHz = 100;
#       endif
    }

#   if defined(_ARM_) || defined(_MIPS_) || defined(_SH4_)
        fltScalebySizeMHzQPC = (Float_WMV)( (pWMVDec->m_iWidthY/320.0F) * (pWMVDec->m_iHeightY/240.0F) * ( 400.0F / iCPUMHz ) * ( pWMVDec->m_iQPCPerMS / 3600.0F ) );
        // allow this floating point at initialization time.  This value could be scaled by 2^23 and be an integer but not worth the trouble.
#   else
        fltScalebySizeMHzQPC = (Float_WMV)( (pWMVDec->m_iWidthY/640.0F) * (pWMVDec->m_iHeightY/480.0F) * ( 1200.0F / iCPUMHz ) * ( pWMVDec->m_iQPCPerMS / 3600.0F ) );
#   endif

    pWMVDec->m_iRegkeyAdapt = prvWMVAccessRegistry(ADAPTPPM, 'r', 0);
    pWMVDec->m_iCountCPUCyclesRegKey = -1;
    if ( pWMVDec->m_iRegkeyAdapt == 0 )
        pWMVDec->m_bOK2AdaptPostLevel = 0;
    else if ( pWMVDec->m_iRegkeyAdapt > 15 )
        pWMVDec->m_iQPC2DecodeAvgFrame = (I32_WMV)( pWMVDec->m_iQPC2DecodeAvgFrame * (((pWMVDec->m_iRegkeyAdapt>>4) & 0xffff ) / 256.0) );  // let it be scaled in the field

    iCPUFeatures = 0;
#   ifdef  _WMV_TARGET_X86_
        if (g_SupportMMX()) 
            iCPUFeatures = 1;
        if (g_SupportSSE1())
            iCPUFeatures = 2;
        if (g_SupportSSE2())
            iCPUFeatures = 3;
#       if 0
            // we did this on the desktop code but it is a mistake because m_iQPC2DecodeAvgFrame interacts with other factors in the dynamic adaption
            // incorporate into adjustment to m_iNeededQPCUnitsperFrame4Level below // 20031231
            switch ( iCPUFeatures ) {
                case 0: pWMVDec->m_iQPC2DecodeAvgFrame = (I32_WMV)( 0.64 * pWMVDec->m_iQPC2DecodeAvgFrame );
                        break;
                case 1: pWMVDec->m_iQPC2DecodeAvgFrame = (I32_WMV)( 0.81 * pWMVDec->m_iQPC2DecodeAvgFrame );
                        break;
                case 2: break;  // "normal" is SSE1 support
                case 3: // Fast P4s tend to have higher QPC counts.  If and when this is fixed, we will be too conservative
                        if ( iCPUMHz > 1950 )
                            pWMVDec->m_iQPC2DecodeAvgFrame = (I32_WMV)( 0.95 * pWMVDec->m_iQPC2DecodeAvgFrame );;
                default: break;
            }
#       endif
#   elif defined(_XSC_)
        iCPUFeatures = IsProcessorFeaturePresentWMV(WM_PF_ARM_INTEL_XSCALE);
#   else
        // add other processor specific feature detection here
#   endif // _WMV_TARGET_X86_

#   if defined(_ARM_) || defined(_MIPS_)
        pWMVDec->m_iQPC2DecodeAvgFrame4Up = ( 922 * pWMVDec->m_iQPC2DecodeAvgFrame ) >> 10;           // hysteresis 0.9 = 922/1024
#   else
        pWMVDec->m_iQPC2DecodeAvgFrame4Up = (I32_WMV)( 0.9 * pWMVDec->m_iQPC2DecodeAvgFrame );        // hysteresis 
#   endif

    // Init our moving average and the BorP one used by B-Frame Dropping
    for( pWMVDec->m_idxLast8QPCDecodes=0; pWMVDec->m_idxLast8QPCDecodes<8; pWMVDec->m_idxLast8QPCDecodes++ ) {
        pWMVDec->m_iLast8QPCDecodes[pWMVDec->m_idxLast8QPCDecodes] = pWMVDec->m_iQPC2DecodeAvgFrame;
    }
    pWMVDec->m_iSumLast8QPCDecodes = 8 * pWMVDec->m_iQPC2DecodeAvgFrame;
    pWMVDec->m_idxLast8QPCDecodes = 0;

    DEBUG_PROFILE_FRAME_MESSAGE(NULL);      // clear prior message as we are sometimes inited more than once.
    ANNOTATE_IF_REGKEY(2, TEXT("m_iNeededQPCUnitsperFrame4Level: "),NULL,0,NULL);
    for(i=0; i<=_TOP_POST_FILTER_LEVEL_;i++) {
        const I32_WMV* const piNeededQPC4Level = pWMVDec->m_bYUV411 ? iNeededQPC4LevelInterlace : iNeededQPC4LevelProgressive;
        pWMVDec->m_iNeededQPCUnitsperFrame4Level[i] = (I32_WMV)( piNeededQPC4Level[i] * fltScalebySizeMHzQPC );
#       ifdef  _WMV_TARGET_X86_
            switch ( iCPUFeatures ) {
                case 0: pWMVDec->m_iNeededQPCUnitsperFrame4Level[i] = (I32_WMV)( pWMVDec->m_iNeededQPCUnitsperFrame4Level[i] * 1.95 );
                        break;
                case 1: pWMVDec->m_iNeededQPCUnitsperFrame4Level[i] = (I32_WMV)( pWMVDec->m_iNeededQPCUnitsperFrame4Level[i] * 1.48 );
                        break;
                case 3: // Fast P4s tend to have higher QPC counts.  If and when this is fixed, we will be too conservative
                        if ( iCPUMHz > 1950 )
                            pWMVDec->m_iNeededQPCUnitsperFrame4Level[i] = (I32_WMV)( pWMVDec->m_iNeededQPCUnitsperFrame4Level[i] * 1.05 );
                        if ( iCPUMHz > 1600 && i < 3)
                            pWMVDec->m_iNeededQPCUnitsperFrame4Level[i] = (I32_WMV)( pWMVDec->m_iNeededQPCUnitsperFrame4Level[i] * 1.10 );
                default: break;
            }
#       elif defined(WMV_OPT_COMMON_ARM)
            if ( iCPUFeatures > 0 ) {
                // XScale needs more cycles due to its deeper pipeline (empirical ratio based on comparing a PXA255 at 400 MHz with a SA1110 at 206 MHz
                pWMVDec->m_iNeededQPCUnitsperFrame4Level[i] = (I32_WMV)( (pWMVDec->m_iNeededQPCUnitsperFrame4Level[i] * 10) >> 3 );  // 1.25
            }
#       else
            // adjust for CPU feature sets here for other processors
#       endif //  _WMV_TARGET_X86_
#       ifdef _MultiThread_Decode_
            if (pWMVDec->m_uiNumProcessors>1) {
                // be conservative since it this is a hyperthreaded machine or a slow memory bus machine, the benifit of 2 procs is limited.
                pWMVDec->m_iNeededQPCUnitsperFrame4Level[i] = (I32_WMV)( pWMVDec->m_iNeededQPCUnitsperFrame4Level[i] / (i<=2 ? 1.4 : 1.6) ); 
            }
#       endif // _MultiThread_Decode_
        ANNOTATE_IF_REGKEY(2,TEXT(""),NULL,pWMVDec->m_iNeededQPCUnitsperFrame4Level[i],TEXT(", "))
        pWMVDec->m_i64SumQPCUsed4Level[i] = 0;
        pWMVDec->m_iCountQPCUsed4Level[i] = 0;
#       ifdef ADAPT_POSTLEVEL_TUNE
            g_i64QFCPostLevel[i] = 0;
            g_iMaxQFCPostLevel[i] = 0;
            g_dSumSquaresQFCPostLevel[i] = 0;
            g_iCountPostLevel[i] = 0;
#       endif // ADAPT_POSTLEVEL_TUNE
    }
    pWMVDec->m_iNeededQPCUnitsperFrame4Level[_TOP_POST_FILTER_LEVEL_+1] = pWMVDec->m_iNeededQPCUnitsperFrame4Level[_TOP_POST_FILTER_LEVEL_];
    ANNOTATE_IF_REGKEY(2,TEXT("and m_iQPC2DecodeAvgFrame = "),NULL,pWMVDec->m_iQPC2DecodeAvgFrame,TEXT("  ") );
#   ifdef  _WMV_TARGET_X86_
        {
        static const TCHAR * szCPUFeatures[]={TEXT("ANSI ("), TEXT("MMX ("), TEXT("SSE1 ("), TEXT("SSE2 (") };
        //ANNOTATE_IF_REGKEY(2,"CPU Feature level: ",szCPUFeatures[iCPUFeatures],iCPUFeatures,")\n" );
        ANNOTATE_IF_REGKEY(2,szCPUFeatures[iCPUFeatures],NULL,iCPUFeatures,TEXT(") "));
        }
#   elif (defined(_ARM_) || defined(__arm)) && defined(WMV_OPT_COMMON_ARM)
        if ( IsProcessorFeaturePresentWMV(WM_PF_ARM_V4) ) {
            ANNOTATE_IF_REGKEY(2,TEXT(" StrongARM = "),NULL,IsProcessorFeaturePresentWMV(WM_PF_ARM_V4),TEXT(" "));
        }
        if ( IsProcessorFeaturePresentWMV(WM_PF_ARM_V5) ) {
            ANNOTATE_IF_REGKEY(2,TEXT(" ARMv5 = "),NULL,IsProcessorFeaturePresentWMV(WM_PF_ARM_V5),TEXT(" "));
        }
        if ( IsProcessorFeaturePresentWMV(WM_PF_ARM_INTEL_XSCALE) ) {
            ANNOTATE_IF_REGKEY(2,TEXT(" XScale = "),NULL,IsProcessorFeaturePresentWMV(WM_PF_ARM_INTEL_XSCALE),TEXT(" "));
        }
        if ( IsProcessorFeaturePresentWMV(WM_PF_ARM_INTEL_WMMX) ) {
            ANNOTATE_IF_REGKEY(2,TEXT(" WMMX = "),NULL,IsProcessorFeaturePresentWMV(WM_PF_ARM_INTEL_WMMX),TEXT(" "));
        }
#   endif //  _WMV_TARGET_X86_

    // finally decide the initial deblocking level.
    WMVDecideDeblockLevel (pWMVDec, pWMVDec->m_iWidthY, pWMVDec->m_iHeightY);
}

void closeAdaptPostLevel(tWMVDecInternalMember *pWMVDec) {
    // Write registry to inform people like Ming who like to confirm what the DLL is doing.
#ifdef ADAPT_POSTLEVEL_TUNE
    Float_WMV fltScalebySizeMHzQPC;
    FILE* fileTuneLog = fopen("\\PostLevelTuneLog.txt", "a");
    if ( fileTuneLog != NULL ) {
        int i;
        I32_WMV iCPUFeatures = 0;
        I32_WMV iWidth = pWMVDec->m_iWidthY, iHeight = pWMVDec->m_iHeightY;
        I32_WMV iCPUMHz = 0;

        iCPUMHz = prvWMVAccessRegistry(ADAPTMHZ, 'r', 0);
        if ( iCPUMHz < 100 || 32000 < iCPUMHz ) {
#           if defined(_XSC_) && defined(WMV_OPT_COMMON_ARM)
                iCPUMHz =  IsProcessorFeaturePresentWMV(WM_PF_ARM_INTEL_XSCALE) ? 400 : ( IsProcessorFeaturePresentWMV(WM_PF_ARM_V4) ? 206 : 100 );
#           elif defined(WMV_OPT_COMMON_ARM)
                iCPUMHz = IsProcessorFeaturePresentWMV(WM_PF_ARM_V4)  ? 206 : 100;
#           elif defined(_WMV_TARGET_X86_)
                iCPUMHz = g_CPUClock();
#           elif defined(macintosh)
                iCPUMHz = g_MacCPUClock();
#           else
                iCPUMHz = 100;  // conservative value which should be OK for even SmartPhones running at 70-120 MHz.
#           endif
        }
        if ( iCPUMHz < 100 ) {
#           if defined(_WMV_TARGET_X86_) && defined(UNDER_CE)
                iCPUMHz = 800;
#           elif defined(_WMV_TARGET_X86_)
                iCPUMHz = 1200;
#           else
                iCPUMHz = 100;
#           endif
        }

#       ifdef  _WMV_TARGET_X86_
            if (g_SupportMMX())
                _asm emms; //Make sure floating point calc works for building the table
            if (g_SupportMMX()) 
                iCPUFeatures = 1;
            if (g_SupportSSE1())
                iCPUFeatures = 2;
            if (g_SupportSSE2())
                iCPUFeatures = 3;
#       endif //  _WMV_TARGET_X86_

#   if defined(_ARM_) || defined(_MIPS_)
        fltScalebySizeMHzQPC = (Float_WMV)( (pWMVDec->m_iWidthY/320.0F) * (pWMVDec->m_iHeightY/240.0F) * ( 400.0F / iCPUMHz ) * ( pWMVDec->m_iQPCPerMS / 3600.0F ) );
#   else
        fltScalebySizeMHzQPC = (Float_WMV)( (pWMVDec->m_iWidthY/640.0F) * (pWMVDec->m_iHeightY/480.0F) * ( 1200.0F / iCPUMHz ) * ( pWMVDec->m_iQPCPerMS / 3600.0F ) );
#   endif

        fprintf( fileTuneLog, "\n\
ScalebySizeSpeedQPC = %.7f, CPUClock = %d, m_iQPCPerMS = %d, CPUFeatures = %d\n\
Post Avg: ", 
            fltScalebySizeMHzQPC, iCPUMHz, pWMVDec->m_iQPCPerMS, iCPUFeatures);
        for( i = 0; i<=_TOP_POST_FILTER_LEVEL_; i++ ) {
            if ( g_iCountPostLevel[i]>1 ) {
                double dMean = ((double)g_i64QFCPostLevel[i])/g_iCountPostLevel[i];
                double dVariance = (g_dSumSquaresQFCPostLevel[i] - g_iCountPostLevel[i]*dMean)/(g_iCountPostLevel[i]-1);
                double dStdDev = dVariance > 0 ? sqrt(dVariance) : 0;
                double dMax;
                dMean   /= fltScalebySizeMHzQPC;
                dStdDev /=  fltScalebySizeMHzQPC;
                dMax = g_iMaxQFCPostLevel[i] / fltScalebySizeMHzQPC;
                fprintf( fileTuneLog, "%d: %.0f+-%.0f [%.0f](%d) ", i, dMean, dStdDev, dMax, g_iCountPostLevel[i] );
            }
        }
        fprintf( fileTuneLog, "\n" );
        fclose( fileTuneLog );
    }
#   pragma message(  __FILE__ "(2968) : Warning OK to test but do not checkin : ADAPT_POSTLEVEL_TUNE" ) 
#else  // ADAPT_POSTLEVEL_TUNE
    {
        I32_WMV i;
        for (i=1; i<=_TOP_POST_FILTER_LEVEL_;i++) {
            if ( pWMVDec->m_iCountQPCUsed4Level[pWMVDec->m_iPostProcessMode] > 50 ) {
                I32_WMV iAvgQPCUsedPerFrame = (I32_WMV)( pWMVDec->m_i64SumQPCUsed4Level[pWMVDec->m_iPostProcessMode] / pWMVDec->m_iCountQPCUsed4Level[pWMVDec->m_iPostProcessMode]);
                I32_WMV iCountMB = (pWMVDec->m_iWidthY*pWMVDec->m_iHeightY)>>8;
                if ( iAvgQPCUsedPerFrame >= 50 && iCountMB >= 50 ) {
                    // don;t trust numbers which are too small.
                    I32_WMV iAvgQPCUsedPerMB = iAvgQPCUsedPerFrame/iCountMB;
                    prvWMVAccessRegistry(QPCLEVEL1+i-1, 'w', iAvgQPCUsedPerMB);
                }
            }
        }
    }
    if ( pWMVDec->m_iCountSkippedFrames > 0 ) {
        ANNOTATE_IF_REGKEY(2,TEXT("Total Skipped "),NULL,pWMVDec->m_iCountSkippedFrames,TEXT(" ") );
        ANNOTATE_IF_REGKEY(2,TEXT("out of "),NULL,pWMVDec->m_iCountFrames,TEXT(" ") );
        ANNOTATE_IF_REGKEY(2,TEXT("with final m_iQPC2DecodeAvgFrame = "),NULL,pWMVDec->m_iQPC2DecodeAvgFrame,TEXT("  ") );
    }
#endif // ADAPT_POSTLEVEL_TUNE
}


void adaptPostLevel(tWMVDecInternalMember *pWMVDec) {
    // heuristic is that you have 40% of the per frame time to decode, optionally post process, and render the output.
    // model levels 1..4 post processing times in terms of screen size, cpu clock speed, and number of processors.
    // adaption:
    //   1. adapt down if not enough time left
    //   2. adapt back up quickly if we quickly recover
    //   3. stay at level for at least T milliseconds before adapting up.
    // Note that we do not change global levels, but adjust boolean flags 
    DWORD dwNow;
    static const DWORD dwaMsStableBeforeAdaptingUp[5] = { 250, 750, 1250, 1750, 2250 };
    I32_WMV iTopLevel, iAdaptedPostLevel, iAdaptSingleFrame = 0;
#   if !defined(ADAPT_POSTLEVEL_TUNE)
        I32_WMV iQPCDecode, iAvgLast8QPCDecode, iQPCPost;
#   endif // !defined(ADAPT_POSTLEVEL_TUNE)
    PROFILE_FRAMES_ONLY(I32_WMV iAdaptReason = 0;)// really only for log
#   if defined(FORCE_ADAPT_POSTLEVEL_MS)
        static DWORD dwTickCountSinceLastForced = 0;
        static int iForceLevel = 0;
        if ( dwTickCountSinceLastForced == 0 )
            dwTickCountSinceLastForced = GetTickCount();
#   endif

    pWMVDec->m_iCountFrames++;
    // limit top level to 4 for x86 or a smaller number for other processors
    // also use half the value for interlace since deringging of interlace is not supported.
    iTopLevel = pWMVDec->m_bYUV411 ? (_TOP_POST_FILTER_LEVEL_>>1) : _TOP_POST_FILTER_LEVEL_;

    if ( (pWMVDec->m_iCountFrames) < 5 ) {
        stopPostLevelDecodeTime(pWMVDec); 
        pWMVDec->m_i64QPCDecode = 0;
        pWMVDec->m_i64QPCPost = 0;
        pWMVDec->m_iNumFrameDropped = 0;
        if ( pWMVDec->m_iCountFrames <= 1 )
            pWMVDec->m_dwTickCountWhenAdaptedDownPostLevel = GetTickCount();
        startPostLevelDecodeTime(pWMVDec);
        return;
    }

    dwNow = GetTickCount();
    if ( dwNow > pWMVDec->m_dwTickCountWhenLastUpdatedRegKey + 8000 ) {
        // every 8 seconds go see if anything has changed
        I32_WMV iForceMode;
        pWMVDec->m_dwTickCountWhenLastUpdatedRegKey = dwNow;
        iForceMode = prvWMVAccessRegistry(FPPM, 'r', 0);                      // Force Post Process Mode
        pWMVDec->m_iRegkeyAdapt = prvWMVAccessRegistry(ADAPTPPM, 'r', 0);     // Adapt Post Process Mode
        if (0 <= iForceMode && iForceMode <= _TOP_POST_FILTER_LEVEL_) {
            pWMVDec->m_bCPUQmoved = TRUE;
            pWMVDec->m_iPostProcessMode = iForceMode;
            SetupPostFilterMode ( pWMVDec );
            pWMVDec->m_bOK2AdaptPostLevel = pWMVDec->m_iRegkeyAdapt == 2;
        } 
    }

    if ( !pWMVDec->m_bOK2AdaptPostLevel )
        return;

#   ifdef  _WMV_TARGET_X86_
#       if !defined(UNDER_CE) && !defined(MIMIC_CE_ON_DESKTOP)
            if (g_SupportMMX())
                _asm emms; //Make sure floating point calc work
#       endif // !defined(UNDER_CE) && !defined(MIMIC_CE_ON_DESKTOP)
#   endif //  _WMV_TARGET_X86_

    stopPostLevelDecodeTime(pWMVDec); 
    assert( !pWMVDec->m_bInTimePost );

#ifndef ADAPT_POSTLEVEL_TUNE

    // first test for lowering the target level
    iAdaptedPostLevel = pWMVDec->m_iAdaptSingleFrame>0 ? pWMVDec->m_iAdaptSingleFrame : pWMVDec->m_iPostProcessMode;
    iQPCDecode   = (I32_WMV)pWMVDec->m_i64QPCDecode;
    pWMVDec->m_iSumLast8QPCDecodes +=  iQPCDecode - pWMVDec->m_iLast8QPCDecodes[pWMVDec->m_idxLast8QPCDecodes];
    pWMVDec->m_iLast8QPCDecodes[pWMVDec->m_idxLast8QPCDecodes++] = iQPCDecode;
    pWMVDec->m_idxLast8QPCDecodes &= 7;
    iAvgLast8QPCDecode = pWMVDec->m_iSumLast8QPCDecodes>>3;
    iQPCPost = (I32_WMV)pWMVDec->m_i64QPCPost;

    ANNOTATE_IF_REGKEY(5,TEXT(""),NULL,pWMVDec->m_iCountFrames,TEXT(", "));
    ANNOTATE_IF_REGKEY(5,TEXT("Dec = "),NULL,iQPCDecode,TEXT(", "));
    ANNOTATE_IF_REGKEY(5,TEXT("Avg8 = "),NULL,iAvgLast8QPCDecode,TEXT(", "));
    ANNOTATE_IF_REGKEY(5,TEXT("Post<"),NULL,pWMVDec->m_iPostProcessMode,TEXT("> "));
    ANNOTATE_IF_REGKEY(5,TEXT("= "),NULL,iQPCPost,TEXT("  "));

    while ( iAdaptedPostLevel > 0 && (iQPCDecode + pWMVDec->m_iNeededQPCUnitsperFrame4Level[iAdaptedPostLevel]) > pWMVDec->m_iQPC2DecodeAvgFrame ) {
        if ( pWMVDec->m_iFramesSinceLastAdapt > 7 && (iAvgLast8QPCDecode + pWMVDec->m_iNeededQPCUnitsperFrame4Level[iAdaptedPostLevel]) > pWMVDec->m_iQPC2DecodeAvgFrame ) {
            iAdaptedPostLevel--;    // We are slow on average
            PROFILE_FRAMES_ONLY( iAdaptReason = 4; )
        } else if (  (iQPCDecode + pWMVDec->m_iNeededQPCUnitsperFrame4Level[iAdaptedPostLevel]) > (pWMVDec->m_iQPC2DecodeAvgFrame<<1) ) {
            iAdaptedPostLevel--;    // We are quite slow right now
            iAdaptSingleFrame = pWMVDec->m_iAdaptSingleFrame>0 ? 0 : pWMVDec->m_iPostProcessMode;    // adapt down, but only for this frame unless it happens twice in a row
            PROFILE_FRAMES_ONLY( iAdaptReason = 5; )
        } else 
            break;
    }

    // Second, test for raising the level if we are in a time window where raising is allowed
    if ( iAdaptedPostLevel == pWMVDec->m_iPostProcessMode 
        && iAdaptedPostLevel < iTopLevel 
        && ( pWMVDec->m_avSyncInfo.m_eAVSyncState <= WMVSyncAV_AllOK 
             || ( pWMVDec->m_avSyncInfo.m_eAVSyncState == WMVSyncAV_BehindOK && pWMVDec->m_avSyncInfo.m_iMSOffset > -(pWMVDec->m_iMSPerFrame>>1) ) )
        ) { 
        // player says the A/V sync is in pretty good shape, so consider adapting up.
        DWORD dwTickCountSinceLastDownAdapted = dwNow - pWMVDec->m_dwTickCountWhenAdaptedDownPostLevel;
        I32_WMV iUpStep = 1;
        I32_WMV iQPCAvailable = pWMVDec->m_iPostProcessMode < pWMVDec->m_iPostLevelPrior2DownAdapt ? pWMVDec->m_iQPC2DecodeAvgFrame : pWMVDec->m_iQPC2DecodeAvgFrame4Up;
#       if defined(_WMV_TARGET_X86_)
            I32_WMV iNeededQPC4NextHigher =  pWMVDec->m_iNeededQPCUnitsperFrame4Level[iAdaptedPostLevel+iUpStep] - pWMVDec->m_iNeededQPCUnitsperFrame4Level[iAdaptedPostLevel];
            if ( (iAdaptedPostLevel+iUpStep) < iTopLevel && iNeededQPC4NextHigher <= (pWMVDec->m_iQPCPerMS>>1) ) {
                // adapting up just 1 step won't cost us any CPU, so try adapting up 2
                iUpStep++;
            }
#       endif // defined(_WMV_TARGET_X86_)
        if ( ( iQPCDecode + pWMVDec->m_iNeededQPCUnitsperFrame4Level[iAdaptedPostLevel+iUpStep] )         < iQPCAvailable && 
             ( iAvgLast8QPCDecode + pWMVDec->m_iNeededQPCUnitsperFrame4Level[iAdaptedPostLevel+iUpStep] ) < iQPCAvailable &&
             ( dwTickCountSinceLastDownAdapted > dwaMsStableBeforeAdaptingUp[iAdaptedPostLevel] || pWMVDec->m_iPostProcessMode < pWMVDec->m_iPostLevelPrior2DownAdapt ) ) {
            // we adapted down long enough ago that it is safe to try to adapt up again or we just down adapted
            iAdaptedPostLevel += iUpStep;
            if ( dwTickCountSinceLastDownAdapted > dwaMsStableBeforeAdaptingUp[iTopLevel] ) {
                pWMVDec->m_dwTickCountWhenAdaptedDownPostLevel = dwNow - 500;  // slow down further up adaption 
            }
            PROFILE_FRAMES_ONLY( iAdaptReason = 6; )
            // describe why we up-adapt at this frame
            ANNOTATE_IF_REGKEY(6,TEXT(" UP("),NULL,iQPCDecode + pWMVDec->m_iNeededQPCUnitsperFrame4Level[iAdaptedPostLevel],TEXT(" "));
            ANNOTATE_IF_REGKEY(6,TEXT("< "),NULL,iQPCAvailable,TEXT(" &&"));
            ANNOTATE_IF_REGKEY(6,TEXT(" "),NULL,iAvgLast8QPCDecode + pWMVDec->m_iNeededQPCUnitsperFrame4Level[iAdaptedPostLevel],TEXT(" "));
            ANNOTATE_IF_REGKEY(6,TEXT("<"),NULL,iQPCAvailable,TEXT(" && "));
            ANNOTATE_IF_REGKEY(6,TEXT("("),NULL,dwTickCountSinceLastDownAdapted,TEXT(" "));
            ANNOTATE_IF_REGKEY(6,TEXT(">"),NULL,dwaMsStableBeforeAdaptingUp[pWMVDec->m_iPostProcessMode],TEXT(" ||"));
            ANNOTATE_IF_REGKEY(6,TEXT(" "),NULL,pWMVDec->m_iPostProcessMode < pWMVDec->m_iPostLevelPrior2DownAdapt,TEXT("))"));
        } else {
            // describe why we did not up-adapt at this frame
            ANNOTATE_IF_REGKEY(6,TEXT(" up("),NULL,iQPCDecode + pWMVDec->m_iNeededQPCUnitsperFrame4Level[iAdaptedPostLevel+iUpStep],TEXT(" "));
            ANNOTATE_IF_REGKEY(6,TEXT("< "),NULL,iQPCAvailable,TEXT(" &&"));
            ANNOTATE_IF_REGKEY(6,TEXT(" "),NULL,iAvgLast8QPCDecode + pWMVDec->m_iNeededQPCUnitsperFrame4Level[iAdaptedPostLevel+iUpStep],TEXT(" "));
            ANNOTATE_IF_REGKEY(6,TEXT("<"),NULL,iQPCAvailable,TEXT(" && "));
            ANNOTATE_IF_REGKEY(6,TEXT("("),NULL,dwTickCountSinceLastDownAdapted,TEXT(" "));
            ANNOTATE_IF_REGKEY(6,TEXT(">"),NULL,dwaMsStableBeforeAdaptingUp[pWMVDec->m_iPostProcessMode],TEXT(" ||"));
            ANNOTATE_IF_REGKEY(6,TEXT(" "),NULL,pWMVDec->m_iPostProcessMode < pWMVDec->m_iPostLevelPrior2DownAdapt,TEXT("))"));
        }
        if ( pWMVDec->m_iPostProcessMode < pWMVDec->m_iPostLevelPrior2DownAdapt ) {
            pWMVDec->m_iPostLevelPrior2DownAdapt--;      // each frame, lower target range for quick up-adapt
        }
        if (iAdaptedPostLevel == pWMVDec->m_iPostProcessMode) {
            // consider adapting the estimate of the Post Processing based on decode average

        }
    }

    if ( iAdaptedPostLevel < pWMVDec->m_iPostProcessMode ) {
        // describe why we down adapted here
        ANNOTATE_IF_REGKEY(6,TEXT(" dn("),NULL,iQPCDecode,TEXT(","));
        ANNOTATE_IF_REGKEY(6,TEXT(" "),NULL,pWMVDec->m_iNeededQPCUnitsperFrame4Level[iAdaptedPostLevel],TEXT(","));
        ANNOTATE_IF_REGKEY(6,TEXT(" "),NULL,pWMVDec->m_iQPC2DecodeAvgFrame,TEXT(","));
        ANNOTATE_IF_REGKEY(6,TEXT(" "),NULL,iAvgLast8QPCDecode - iQPCPost,TEXT(") "));

    }

    if ( pWMVDec->m_i64QPCPost > 0 && 0 < pWMVDec->m_iPostProcessMode && pWMVDec->m_iPostProcessMode <= iTopLevel ) {
        // track total time used for post processing by level
        pWMVDec->m_i64SumQPCUsed4Level[pWMVDec->m_iPostProcessMode]  += iQPCPost;
        pWMVDec->m_iCountQPCUsed4Level[pWMVDec->m_iPostProcessMode]++;
    }

#else  // ADAPT_POSTLEVEL_TUNE

    // use this to empirically determine fltNeededQPC4Level.  Test several processor types.
    if ( pWMVDec->m_i64QPCPost > 0 ) {
        g_i64QFCPostLevel[pWMVDec->m_iPostProcessMode] += pWMVDec->m_i64QPCPost;
        if ( g_iMaxQFCPostLevel[pWMVDec->m_iPostProcessMode] < (int)pWMVDec->m_i64QPCPost )
            g_iMaxQFCPostLevel[pWMVDec->m_iPostProcessMode] = (int)pWMVDec->m_i64QPCPost;
        g_dSumSquaresQFCPostLevel[pWMVDec->m_iPostProcessMode] += ( pWMVDec->m_i64QPCPost * (double)pWMVDec->m_i64QPCPost );
        g_iCountPostLevel[pWMVDec->m_iPostProcessMode]++;
        pWMVDec->m_i64QPCPost = 0;
    }
    iAdaptedPostLevel = pWMVDec->m_iPostProcessMode + 1;
    if ( iAdaptedPostLevel > iTopLevel )
        iAdaptedPostLevel = 0;

#endif // ADAPT_POSTLEVEL_TUNE

#if defined(FORCE_ADAPT_POSTLEVEL_MS)
    if ( iAdaptedPostLevel == pWMVDec->m_iPostProcessMode && (dwNow-dwTickCountSinceLastForced) > FORCE_ADAPT_POSTLEVEL_MS && pWMVDec->m_iCountFrames > 60 ) {
        assert( iTopLevel > 0 );
        do {
            iAdaptedPostLevel = iForceLevel;
            if ( (iForceLevel += 2) > iTopLevel )
                iForceLevel -= (iTopLevel+1);           // 0, 2, 4, 1, 3, ...
            if ( iAdaptedPostLevel == iForceLevel ) {
                // when iTopLevel is not 4, the above trick does not work so well.
                if ( (iForceLevel += 1) > iTopLevel )
                    iForceLevel -= (iTopLevel+1);
                if ( iAdaptedPostLevel == iForceLevel ) {
                    iAdaptedPostLevel = iTopLevel>0 ? iTopLevel-1 : 0;
                    break;
                }
            }
        } while ( (!( 0 <= iAdaptedPostLevel && iAdaptedPostLevel <= iTopLevel )) || (iAdaptedPostLevel == pWMVDec->m_iPostProcessMode) );
        dwTickCountSinceLastForced = dwNow;
        ANNOTATE_IF_REGKEY(2,TEXT("Force "),NULL,0,NULL);
        PROFILE_FRAMES_ONLY( iAdaptReason = 7; )
    } 
#   pragma message(  __FILE__ "(3252) : Warning OK to test but do not checkin : FORCE_ADAPT_POSTLEVEL_MS" ) 
#endif

     // now change level as appropriate
    if ( iAdaptedPostLevel != pWMVDec->m_iPostProcessMode || pWMVDec->m_iAdaptSingleFrame>0 ) {
        // now change state 
#       define cFlagDeblock 1
#       define cFlagDering 2
#       define cFlagFastDeblock 4
        static const int iPostLevelFlags[5] = { 0,  cFlagDeblock+cFlagFastDeblock, cFlagDeblock, cFlagDering+cFlagDeblock+cFlagFastDeblock, cFlagDering+cFlagDeblock };
        ANNOTATE_IF_REGKEY(2,TEXT("Adapt level from "),NULL,pWMVDec->m_iPostProcessMode,TEXT(" "));
        ANNOTATE_IF_REGKEY(2,TEXT("to "),NULL,iAdaptedPostLevel,TEXT(" "));
        ANNOTATE_IF_REGKEY(2,TEXT("at "),NULL,pWMVDec->m_iCountFrames,TEXT(".  "));
        pWMVDec->m_bDeblockOn = (iPostLevelFlags[iAdaptedPostLevel] & cFlagDeblock) != 0;
        pWMVDec->m_bDeringOn  = (iPostLevelFlags[iAdaptedPostLevel] & cFlagDering) != 0;
        if ( (iPostLevelFlags[pWMVDec->m_iPostProcessMode] & cFlagFastDeblock) != (iPostLevelFlags[iAdaptedPostLevel] & cFlagFastDeblock) )
            g_InitPostFilter ( (iPostLevelFlags[iAdaptedPostLevel] & cFlagFastDeblock) == cFlagFastDeblock, pWMVDec->m_bYUV411 );
        if ( iAdaptedPostLevel < pWMVDec->m_iPostProcessMode ) {
            pWMVDec->m_dwTickCountWhenAdaptedDownPostLevel = dwNow;
            if ( iAdaptedPostLevel > 0 ) {
                pWMVDec->m_dwTickCountWhenAdaptedDownPostLevel -= (dwaMsStableBeforeAdaptingUp[pWMVDec->m_iPostProcessMode] - dwaMsStableBeforeAdaptingUp[iAdaptedPostLevel]);
            }
            if ( pWMVDec->m_iPostLevelPrior2DownAdapt < (pWMVDec->m_iPostProcessMode << 2) )
                pWMVDec->m_iPostLevelPrior2DownAdapt = pWMVDec->m_iPostProcessMode << 2;
        }
        pWMVDec->m_iPostFilterLevel = pWMVDec->m_iPostProcessMode = iAdaptedPostLevel;
        pWMVDec->m_iAdaptSingleFrame = iAdaptSingleFrame;
        pWMVDec->m_iFramesSinceLastAdapt = 0;
        DEBUG_PROFILE_FRAME_SET_FLAGS(iAdaptReason<<8);
        assert(  ( pWMVDec->m_bDeblockOn != 0 ) == ( pWMVDec->m_iPostProcessMode > 0 ) );
    } else {
        pWMVDec->m_iFramesSinceLastAdapt++;
    }

    // restart Decode Timer based from adapt point. 
    // this means that everything after the adapt point gets counted for the next frame, but it is relatively constant per frame.
    pWMVDec->m_i64QPCDecode = 0;
    pWMVDec->m_i64QPCPost = 0;
    startPostLevelDecodeTime(pWMVDec);
}

#endif // _SUPPORT_POST_FILTERS_
#if defined(_XBOX) && (defined(WMV9_SIMPLE_ONLY) || defined(WMV789_ONLY))
/*
int g_SupportMMX()
{
    return TRUE_WMV;
}

int g_SupportSSE1()
{
    return TRUE_WMV;
}

int g_SupportSSE2()
{
    return FALSE_WMV;
}
*/
int g_CPUClock()
{
    return 733;
}
#endif


VO_S32 VO_API yyGetWMVDecFunc (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	VO_VIDEO_DECAPI *pWMV9Dec = pDecHandle;

	if(!pWMV9Dec)
		return VO_ERR_INVALID_ARG;

	pWMV9Dec->Init          = voWMV9DecInit;
	pWMV9Dec->Uninit        = voWMV9DecUninit;
	pWMV9Dec->SetParam      = voWMV9DecSetParameter;
	pWMV9Dec->GetParam      = voWMV9DecGetParameter;
	pWMV9Dec->SetInputData  = voWMV9DecSetInputData;
	pWMV9Dec->GetOutputData = voWMV9DecGetOutputData;

	return VO_ERR_NONE;
}



