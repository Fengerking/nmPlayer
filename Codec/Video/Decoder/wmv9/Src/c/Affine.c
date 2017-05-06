//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#include "voWmvPort.h"
#include "xplatform_wmv.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "constants_wmv.h"
#include "tables_wmv.h"
#include "codehead.h"
#include "localhuffman_wmv.h"
#include "Affine.h"

#if !defined(_APOGEE_) && !defined(_MAC) && !defined(_TI_C55X_) && !defined(__arm)
//#include "malloc.h"
#include "memory.h"
#endif

#ifndef UNDER_CE
//#   include "assert.h"
#endif

#ifdef SH3
#   define SH3_COMPILER
#endif

#ifdef _WMV_TARGET_X86_
#   include "cpudetect.h"
#endif // _WMV_TARGET_X86_

#include "math.h"

//#pragma warning( disable: 4719 )

// ===========================================================================
// private functions
// ===========================================================================
#include "strmdec_wmv.h"
#include "repeatpad_wmv.h"

#ifndef WMV9_SIMPLE_ONLY

#define PanZoomFunParams U8_WMV  *pDst,     \
                         U8_WMV  *pTmpRef,  \
                         I16_WMV *pIx,      \
                         I16_WMV *pRx,      \
                         I16_WMV *ry4_4way, \
                         I32_WMV iOldWidth
                         
typedef Void_WMV (PanZoomFun)(PanZoomFunParams);

#ifdef _WMV_TARGET_X86_
    extern Void_WMV PanZoom4AdjacentPixelsMMX(PanZoomFunParams);
    extern Void_WMV PanZoom4RandomPixelsMMX(PanZoomFunParams);
    extern Void_WMV PanZoom4PixelsC(PanZoomFunParams);

    extern tWMVDecodeStatus Affine_PanMMX (HWMVDecoder* phWMVDecoder, double fX, double fY);
    //inline Void Affine_panMMXLine(I16_WMV *rx_4way, I32_WMV length,  I32_WMV iOldWidth, I16_WMV *ry4_4way, U8_WMV *dst,U8_WMV *src);
    extern Void_WMV panMMXLine(I16_WMV *rx_4way, I32_WMV length,  I32_WMV iOldWidth, I16_WMV *ry4_4way, U8_WMV *dst,U8_WMV *src);
    extern tWMVDecodeStatus Affine_PanMMXFading (HWMVDecoder* phWMVDecoder, double fX, double fY, double fFading);
    extern Void_WMV panZoomMMXLine(__int16 *pIx,__int16 *pRx,I32_WMV length, I32_WMV iOldWidth,__int16 *ry4_4way,PanZoomFun **pDispatchTable, U8_WMV *dst,U8_WMV *src);
    extern tWMVDecodeStatus Affine_PanZoomMMX (HWMVDecoder* phWMVDecoder, double fZoom, double fX, double fY);
    extern tWMVDecodeStatus Affine_StretchMMX (HWMVDecoder* phWMVDecoder, double fZoomX, double fZoomY, double fX, double fY);
    extern Void_WMV fadeMMXLine_Y(U8_WMV *pFrame,I32_WMV length_Y,I32_WMV fading);
    extern Void_WMV fadeMMXLine_UV(U8_WMV *pFrame,I32_WMV length_Y,I32_WMV fading);
    extern tWMVDecodeStatus Affine_PanZoomMMXFading ( HWMVDecoder* phWMVDecoder, 
            double fZoom, double fX, double fY, double fFading);
    tWMVDecodeStatus Affine_StretchMMXFading ( HWMVDecoder* phWMVDecoder, 
            double fZoomX, double fZoomY, double fX, double fY, double fFading);
#endif

#define BITMAP_WIDTH(width,bitCount) \
    (I32_WMV)((I32_WMV)(((((I32_WMV)width) * ((I32_WMV)bitCount)) + 31L) & (I32_WMV)~31L) / 8L)

tWMVDecodeStatus Affine_Init (  HWMVDecoder* phWMVDecoder,
                    int iOldWidth, 
                    int iOldHeight, 
                    int iNewWidth, 
                    int iNewHeight)
{
    tWMVDecInternalMember *pWMVDec; 
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_Init);

    if (iOldWidth  < 0)
        iOldWidth  = 0 - iOldWidth;
    if (iOldHeight < 0)
        iOldHeight = 0 - iOldHeight;
    if (iNewWidth  < 0)
        iNewWidth  = 0 - iNewWidth;
    if (iNewHeight < 0)
        iNewHeight = 0 - iNewHeight;

    pWMVDec->m_iOldWidth  = iOldWidth;
    pWMVDec->m_iOldHeight = iOldHeight;
    pWMVDec->m_iNewWidth  = iNewWidth;
    pWMVDec->m_iNewHeight = iNewHeight;

    pWMVDec->m_uiNumSlices = 1;

    return tWMVStatus;
}


#if defined(WMV_OPT_SPRITE) && !defined(WMV_OPT_SPRITE_ARM)

Void_WMV Affine_Add (const U8_WMV* pucSrcY,const U8_WMV* pucSrcU,const U8_WMV* pucSrcV,U8_WMV* pucDstY,U8_WMV* pucDstU,U8_WMV* pucDstV,I32_WMV iSize)
{
    I32_WMV i;
    const U32_WMV* puiSrcY = (U32_WMV*) pucSrcY;
    U32_WMV* puiDstY = (U32_WMV*) pucDstY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_Add);
    iSize >>= 2;
    for (i = iSize; i != 0; i--) {
        I32_WMV uiSrcU = (I32_WMV)(*pucSrcU++);
        I32_WMV uiSrcV = (I32_WMV)(*pucSrcV++);
        I32_WMV uiDstU = (I32_WMV)(*pucDstU);
        I32_WMV uiDstV = (I32_WMV)(*pucDstV);
        *puiDstY++ += *puiSrcY++;
        uiDstU += uiSrcU - 128;
        uiDstV += uiSrcV - 128;
        *pucDstU++ = (U8_WMV) uiDstU;
        *pucDstV++ = (U8_WMV) uiDstV;
    }
}


Void_WMV Affine_PanYLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcY, U8_WMV* pDstY)
{
    const I32_WMV factor_rx_ry = pWMVDec->factor_rx_ry;
    const I32_WMV rx = pWMVDec->rx;
    const I32_WMV ry = pWMVDec->ry;
    const I32_WMV rxXry = pWMVDec->rxXry;

    const U8_WMV* pSrcYNext = pSrcY + pWMVDec->m_iOldWidth;
    I32_WMV x = pWMVDec->iYLengthX;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_PanYLineC);

    for (; x != 0; x--) {
        //*pDstY++ = (unsigned char)((U00*factor_rx_ry + U01*ry + U10*rx 
        //                            + (((U00+U11-U01-U10)*rxXry) >> 7) ) >> 7);
        I32_WMV U00, U10, U01, U11, iDstY, itemp;
        U00 = pSrcY [0];
        iDstY = U00 * factor_rx_ry;
        U10 = pSrcY [1];       
        U01 = pSrcYNext [0];
        iDstY += U10 * rx;
        U11 = pSrcYNext [1];
        itemp = U00 - U01 - U10;
        iDstY += U01 * ry;
        itemp += U11;
        itemp *= rxXry;
        iDstY += itemp >> 7;
        iDstY >>= 7;

        *pDstY++ = iDstY; 

        pSrcY++;
        pSrcYNext++;
    }
}


Void_WMV Affine_StretchYLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcY, U8_WMV* pDstY)
{
    const I32_WMV factor = pWMVDec->factor;
    const I32_WMV ry = pWMVDec->ry;
    const I32_WMV lA = pWMVDec->lA;

    I32_WMV ox = pWMVDec->oxRef;
    I32_WMV x = pWMVDec->iYLengthX;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_StretchYLineC);

    for (; x != 0; x--) {
        I32_WMV U00, U10, U01, U11, rx;
        const U8_WMV* pTmp = pSrcY + (ox >> 11);
        U00 = pTmp [0];
        U10 = pTmp [1];
        U01 = (pTmp + pWMVDec->m_iOldWidth) [0];
        U11 = (pTmp + pWMVDec->m_iOldWidth) [1];
        rx = ox & 0x7FF;
        *pDstY++ = (unsigned char)((U00*(factor-rx-ry)+ U01*ry + U10*rx + (((U00+U11-U01-U10)*rx*ry) >> 11)) >> 11);
        ox += lA;
    }
}


Void_WMV Affine_StretchFadingYLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcY, U8_WMV* pDstY)
{
    const I32_WMV factor = pWMVDec->factor;
    const I32_WMV ry = pWMVDec->ry;
    const I32_WMV lA = pWMVDec->lA;
    const I32_WMV iFading = pWMVDec->iFading;

    I32_WMV ox = pWMVDec->oxRef;
    I32_WMV x = pWMVDec->iYLengthX;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_StretchFadingYLineC);

    for (; x != 0; x--) {
        I32_WMV U00, U10, U01, U11, rx;
        const U8_WMV* pTmp = pSrcY + (ox >> 11);
        U00 = pTmp [0];
        U10 = pTmp [1];
        U01 = (pTmp + pWMVDec->m_iOldWidth) [0];
        U11 = (pTmp + pWMVDec->m_iOldWidth) [1];
        rx = ox & 0x7FF;
        *pDstY++ = (unsigned char)((((U00*(factor-rx-ry)+ U01*ry + U10*rx + (((U00+U11-U01-U10)*rx*ry) >> 11)) >> 11)*iFading)>>8);;
        ox += lA;
    }
}


Void_WMV Affine_StretchUVLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcU, const U8_WMV* pSrcV, U8_WMV* pDstU, U8_WMV* pDstV)
{
    const I32_WMV factor = pWMVDec->factor;
    const I32_WMV ry = pWMVDec->ry;
    const I32_WMV lAx2 = pWMVDec->lAx2;

    I32_WMV ox = pWMVDec->oxRef;
    I32_WMV x = pWMVDec->iYLengthX;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_StretchUVLineC);

    for (; x != 0; x--) {
        I32_WMV U00, U10, U01, U11, rx, lrxry, rxXry;
        const U8_WMV* pTmp;

        rx = 0x7FF & (ox >> 1);
        lrxry = factor-rx-ry;
        rxXry = rx * ry;

        pTmp = pSrcU + (ox >> 12);
        U00 = pTmp [0];
        U10 = pTmp [1];
        U01 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [0];
        U11 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [1];
        *pDstU++ = (unsigned char)((U00*lrxry + U01*ry + U10*rx + (((U00+U11-U01-U10)*rxXry) >> 11)) >> 11);

        pTmp = pSrcV + (ox >> 12);
        U00 = pTmp [0];
        U10 = pTmp [1];
        U01 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [0];
        U11 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [1];
    
        *pDstV++ = (unsigned char)((U00*lrxry + U01*ry + U10*rx + (((U00+U11-U01-U10)*rxXry) >> 11)) >> 11);
            
        ox += lAx2;
    }
}


Void_WMV Affine_StretchFadingUVLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcU, const U8_WMV* pSrcV, U8_WMV* pDstU, U8_WMV* pDstV)
{
    const I32_WMV factor = pWMVDec->factor;
    const I32_WMV ry = pWMVDec->ry;
    const I32_WMV lAx2 = pWMVDec->lAx2;
    const I32_WMV iFading = pWMVDec->iFading;

    I32_WMV ox = pWMVDec->oxRef;
    I32_WMV x = pWMVDec->iYLengthX;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_StretchFadingUVLineC);

    for (; x != 0; x--) {
        I32_WMV U00, U10, U01, U11, rx, lrxry, rxXry;
        const U8_WMV* pTmp;

        rx = 0x7FF & (ox >> 1);
        lrxry = factor-rx-ry;
        rxXry = rx * ry;

        pTmp = pSrcU + (ox >> 12);
        U00 = pTmp [0];
        U10 = pTmp [1];
        U01 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [0];
        U11 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [1];
        *pDstU++ = (unsigned char)((((((U00*(lrxry) + U01*ry + U10*rx + (((U00+U11-U01-U10)*rxXry) >> 11)) >> 11)-128)*iFading)>>8)+128);

        pTmp = pSrcV + (ox >> 12);
        U00 = pTmp [0];
        U10 = pTmp [1];
        U01 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [0];
        U11 = (pTmp + (pWMVDec->m_iOldWidth >> 1)) [1];
    
        *pDstV++ = (unsigned char)((((((U00*(lrxry) + U01*ry + U10*rx + (((U00+U11-U01-U10)*rxXry) >> 11)) >> 11)-128)*iFading)>>8)+128);
            
        ox += lAx2;
    }
}


Void_WMV Affine_PanFadingYLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcY, U8_WMV* pDstY)
{
    const I32_WMV factor_rx_ry = pWMVDec->factor_rx_ry;
    const I32_WMV rx = pWMVDec->rx;
    const I32_WMV ry = pWMVDec->ry;
    const I32_WMV rxXry = pWMVDec->rxXry;
    const I32_WMV iFading = pWMVDec->iFading;
    const U8_WMV* pSrcYNext = pSrcY + pWMVDec->m_iOldWidth;
    I32_WMV x = pWMVDec->iYLengthX;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_PanFadingYLineC);

    for (; x != 0; x--) {
        I32_WMV U00 = pSrcY [0];
        I32_WMV U10 = pSrcY [1];
        I32_WMV U01 = pSrcYNext [0];
        I32_WMV U11 = pSrcYNext [1];
        pSrcY++;
        pSrcYNext++;
        *pDstY++ = (unsigned char)((((U00*factor_rx_ry + U01*ry + U10*rx 
                                    + (((U00+U11-U01-U10)*rxXry) >> 11)) >> 11) * iFading) >> 8 );
    }
}


Void_WMV Affine_PanUVLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcU, const U8_WMV* pSrcV, U8_WMV* pDstU, U8_WMV* pDstV)
{
    const I32_WMV rx_2 = pWMVDec->rx_2;
    const I32_WMV ry_2 = pWMVDec->ry_2;
    const I32_WMV rxry = pWMVDec->rxry;
    const I32_WMV lrxry = pWMVDec->lrxry;
    const U8_WMV* pSrcUVNext;
    I32_WMV x;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_PanUVLineC);

    pSrcUVNext = pSrcU + (pWMVDec->m_iOldWidth >> 1);
    x = pWMVDec->iUVLengthX;
    for (; x != 0; x--) {
        I32_WMV U00 = pSrcU [0];
        I32_WMV U10 = pSrcU [1];
        I32_WMV U01 = pSrcUVNext [0];
        I32_WMV U11 = pSrcUVNext [1];
        pSrcU++;
        pSrcUVNext++;
        *pDstU++ = (unsigned char)((U00*(lrxry) + U01*ry_2+ U10*rx_2+ 
                                (((U00+U11-U01-U10)*rxry) >> 7)) >> 7);
    }

    pSrcUVNext = pSrcV + (pWMVDec->m_iOldWidth >> 1);
    x = pWMVDec->iUVLengthX;
    for (; x != 0; x--) {
        I32_WMV U00 = pSrcV [0];
        I32_WMV U10 = pSrcV [1];
        I32_WMV U01 = pSrcUVNext [0];
        I32_WMV U11 = pSrcUVNext [1];
        pSrcV++;
        pSrcUVNext++;
        *pDstV++ = (unsigned char)((U00*(lrxry) + U01*ry_2+ U10*rx_2+ 
                                (((U00+U11-U01-U10)*rxry) >> 7)) >> 7);
    }
}


Void_WMV Affine_PanFadingUVLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcU, const U8_WMV* pSrcV, U8_WMV* pDstU, U8_WMV* pDstV)
{
    const I32_WMV rx_2 = pWMVDec->rx_2;
    const I32_WMV ry_2 = pWMVDec->ry_2;
    const I32_WMV rxry = pWMVDec->rxry;
    const I32_WMV lrxry = pWMVDec->lrxry;
    const I32_WMV iFading = pWMVDec->iFading;
    I32_WMV x;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_PanFadingUVLineC);

    x = pWMVDec->iUVLengthX;
    for (; x != 0; x--) {
        I32_WMV U00, U10, U01, U11;
        U00 = pSrcU [0];
        U10 = pSrcU [1];
        U01 = (pSrcU + (pWMVDec->m_iOldWidth >> 1)) [0];
        U11 = (pSrcU + (pWMVDec->m_iOldWidth >> 1)) [1];
        pSrcU++;
        *pDstU++ = (unsigned char)((((((U00*(lrxry) + U01*ry_2 + U10*rx_2 + (((U00+U11-U01-U10)*rxry) >> 11)) >> 11) - 128) * iFading) >> 8 ) + 128);
        
        U00 = pSrcV [0];
        U10 = pSrcV [1];
        U01 = (pSrcV + (pWMVDec->m_iOldWidth >> 1)) [0];
        U11 = (pSrcV + (pWMVDec->m_iOldWidth >> 1)) [1];
        pSrcV++;
        *pDstV++ = (unsigned char)((((((U00*(lrxry) + U01*ry_2 + U10*rx_2 + (((U00+U11-U01-U10)*rxry) >> 11)) >> 11) - 128) * iFading) >> 8 ) + 128);
    }
}

#endif // defined(WMV_OPT_SPRITE) && !defined(WMV_OPT_SPRITE_ARM)


tWMVDecodeStatus Affine_PanC (HWMVDecoder* phWMVDecoder, double fX, double fY)
{
    const long factor = 128;
    const long power = 7;
    const long power_1 = 8;
    const long mask = 0x7F;

    tWMVDecInternalMember *pWMVDec; 
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    int x, y, offset;
    const unsigned char *pTmp;
    const unsigned char *pTmpY;
    const unsigned char *pTmpU;
    const unsigned char *pTmpV;
    int iOldWidth_2, iNewWidth_2;
    long iStartX, iEndX, iStartY, iEndY;
    long iEndXm2;
    long iStartX_2;
    long iEndX_2  ;
    long iEndX_2m1;

    long lA; 
    long lC;
    long lE;
    long lF;
    long lAx2; 
    long iOldWidthxFactor ;
    long iOldHeightxFactor;
    unsigned char * pFrameYRef;
    unsigned char * pFrameURef;
    unsigned char * pFrameVRef;
    unsigned char * pFrameY;
    unsigned char * pFrameU;
    unsigned char * pFrameV;
    long oy ;
    long oxRef;
    long ox ;
    long rx ;
    long ry ;
    long ix ;
    long iy ;

    long iy_2 ;
    long ry_2 ;
    long ix_2 ;
    long rx_2 ;
    long rxry ;
    long lrxry;

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_PanC);

    pTmpY = (unsigned char *)pWMVDec->m_dibBitsY;
    pTmpU = (unsigned char *)pWMVDec->m_dibBitsU;
    pTmpV = (unsigned char *)pWMVDec->m_dibBitsV;
    iOldWidth_2 = pWMVDec->m_iOldWidth>>1;
    iNewWidth_2 = pWMVDec->m_iNewWidth>>1;

#   ifdef WMV_OPT_SPRITE
        // reduce floating point by noting that floor(-x) = -ceil(x)
        iStartX   = -(long)(floor(fX));
        if ( iStartX < 0 )
            iStartX = 0;
        if ( pWMVDec->m_iNewWidth < iStartX )
            iStartX = pWMVDec->m_iNewWidth;
        iEndX     = pWMVDec->m_iOldWidth - (long)ceil( fX );
        if ( pWMVDec->m_iNewWidth < iEndX )
            iEndX = pWMVDec->m_iNewWidth;
        if ( iEndX < 0 )
            iEndX = 0;

        iStartY   = -(long)(floor(fY));
        if ( iStartY < 0 )
            iStartY = 0;
        if ( pWMVDec->m_iNewHeight < iStartY )
            iStartY = pWMVDec->m_iNewHeight;
        iEndY     = pWMVDec->m_iOldHeight - (long)ceil( fY );
        if ( pWMVDec->m_iNewHeight < iEndY )
            iEndY = pWMVDec->m_iNewHeight;
        if ( iEndY < 0 )
            iEndY = 0;
#   else
        iStartX   = (long)(ceil(max( 0.0, -fX)));
        iStartX   = (long)(floor(min( (double)(pWMVDec->m_iNewWidth ), (double)(iStartX))));
        iEndX     = (long)(floor(min( (double)(pWMVDec->m_iNewWidth ), (double)(pWMVDec->m_iOldWidth )-fX)));
        iEndX     = (long)(ceil(max( 0.0, (double)(iEndX))));

        iStartY   = (long)(ceil(max( 0.0, -fY)));
        iStartY   = (long)(floor(min( (double)(pWMVDec->m_iNewHeight), (double)(iStartY))));
        iEndY     = (long)(floor(min( (double)(pWMVDec->m_iNewHeight), (double)(pWMVDec->m_iOldHeight)-fY)));
        iEndY     = (long)(ceil(max( 0.0, (double)(iEndY))));
#   endif

    iStartX   = (((iStartX + 1)>>1)<<1);
    iEndX     = (((iEndX      )>>1)<<1);
    iStartY   = (((iStartY + 1)>>1)<<1);
    iEndY     = (((iEndY      )>>1)<<1);
    if (iEndY < 2) 
        iEndY = 2;

    iEndXm2   = iEndX - 2;
    if (iEndXm2 < 0) 
        iEndXm2 = 0;

    iStartX_2 = iStartX>>1;
    iEndX_2   = iEndX  >>1;
    iEndX_2m1 = iEndX_2 - 1;
    if (iEndX_2m1 < 0) 
        iEndX_2m1 = 0;

    lA = (long)(factor); 
    lC = (long)(fX * factor);
    lE = (long)(factor);
    lF = (long)(fY * factor);
    lAx2 = (long)(2 * factor); 

    iOldWidthxFactor  = pWMVDec->m_iOldWidth * factor;
    iOldHeightxFactor = pWMVDec->m_iOldHeight * factor;

    pFrameYRef = pWMVDec->m_pFrameY;
    pFrameURef = pWMVDec->m_pFrameU;
    pFrameVRef = pWMVDec->m_pFrameV;

    oy = lE * iStartY +lF;
    oxRef = lC + lA * iStartX;
    ox = oxRef;
    rx = ox & mask;
    ry = oy & mask;
    ix = ox >> power;
    iy = oy >> power;

    iy_2 = oy >> power_1;
    ry_2 = (oy>>1) & mask;
    ix_2 = ox >> power_1;
    rx_2 = (ox>>1) & mask;
    rxry = rx_2*ry_2;
    lrxry = factor-rx_2-ry_2;

#   ifdef WMV_OPT_SPRITE
        pWMVDec->power = power;
        pWMVDec->factor_rx_ry = factor - rx - ry;
        pWMVDec->rx = rx;
        pWMVDec->ry = ry;
        pWMVDec->rx_2 = rx_2;
        pWMVDec->ry_2 = ry_2;
        pWMVDec->rxry = rxry;
        pWMVDec->rxXry = rx * ry;
        pWMVDec->lrxry = lrxry;
        pWMVDec->iYLengthX = iEndX - 2 - iStartX;
        pWMVDec->iUVLengthX = iEndX_2 - 1 - iStartX_2;
#   endif

    for (y=0; y<iStartY; y++) {
        pFrameY = pFrameYRef;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        for (x=0; x<iNewWidth_2; x ++) {
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;
        y++;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
    }

    for (y=iStartY; y<iEndY-2; y++) {

        pFrameY = pFrameYRef;

        pTmpY = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*(iy+y-iStartY) + ix);
        for (x = 0; x < iStartX; x++) {
            *pFrameY++ = 16;
        }

#       ifndef WMV_OPT_SPRITE
            for (x=0; x < iEndX - 2 - iStartX; x++) {
                long U00 ;
                long U10 ;
                long U01 ;
                long U11 ;

                pTmp = (unsigned char *)(pTmpY + x);

                U00 = *(pTmp                  );
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + pWMVDec->m_iOldWidth);
                U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

                *pFrameY++ = (unsigned char)((U00*(factor-rx-ry) + U01*ry + U10*rx +
                                            (((U00+U11-U01-U10)*rx*ry) >> power)) >> power);
            }
#       else       
            if (pWMVDec->iYLengthX > 0) {
                Affine_PanYLineC (pWMVDec, pTmpY, pFrameY);
                pFrameY += pWMVDec->iYLengthX;
            }
#       endif //     WMV_OPT_SPRITE   

        if (iStartX <= iEndXm2) {
            for (x = iEndXm2; x < iEndX; x++) {
                pTmp = (unsigned char *)(pTmpY + x-iStartX);
                *pFrameY++ = *pTmp;
            }
        }

        for (x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 16;
        }

        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        offset = iOldWidth_2*(iy_2 + ((y-iStartY)>>1)) + ix_2;

        for (x=0; x<iStartX_2; x ++) {
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }

#       ifndef WMV_OPT_SPRITE
            for (x = 0; x < iEndX_2 - 1 - iStartX_2; x ++) {
                long U00 ;
                long U10 ;
                long U01 ;
                long U11 ;

                pTmp = (unsigned char *)(pTmpU + offset + x);

                U00 = *(pTmp);
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + iOldWidth_2);
                U11 = *(pTmp + 1 + iOldWidth_2);

                *pFrameU++ = (unsigned char)((U00*(lrxry) + U01*ry_2+ U10*rx_2+ 
                                    (((U00+U11-U01-U10)*rxry) >> power)) >> power);


                pTmp = (unsigned char *)(pTmpV + offset+ x);

                U00 = *(pTmp);
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + iOldWidth_2);
                U11 = *(pTmp + 1 + iOldWidth_2);

                *pFrameV++ = (unsigned char)((U00*(lrxry) + U01*ry_2+ U10*rx_2+ 
                                    (((U00+U11-U01-U10)*rxry)>>power)) >> power);
            }
#       else
            if (pWMVDec->iUVLengthX > 0) {
                Affine_PanUVLineC (pWMVDec, pTmpU + offset, pTmpV + offset, pFrameU, pFrameV);
                pFrameU += pWMVDec->iUVLengthX;
                pFrameV += pWMVDec->iUVLengthX;
            }
#       endif //WMV_OPT_SPRITE

        if (iStartX_2 <= iEndX_2m1) {
            for (x=iEndX_2m1; x<iEndX_2; x ++) {
                pTmp = (unsigned char *)(pTmpU + offset + x - iStartX_2);
                *pFrameU++ = *pTmp;
                pTmp = (unsigned char *)(pTmpV + offset + x - iStartX_2);
                *pFrameV++ = *pTmp;
            }
        }

        for (x=iEndX_2; x<iNewWidth_2; x ++) {
            
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }

        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;

        y++;
 
        pTmpY = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*(iy+y-iStartY) + ix);

        for (x=0; x<iStartX; x++) {
            *pFrameY++ = 16;
        }

#       ifndef WMV_OPT_SPRITE
            for (x=0; x < iEndX - 2 -iStartX; x++) {
                long U00 ;
                long U10 ;
                long U01 ;
                long U11 ;

                pTmp = (unsigned char *)(pTmpY + x);

                U00 = *(pTmp                  );
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + pWMVDec->m_iOldWidth);
                U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

                *pFrameY++ = (unsigned char)((U00*(factor-rx-ry)+ U01*ry + U10*rx 
                                            + (((U00+U11-U01-U10)*rx*ry) >> power)) >> power);

            }
#       else
            if (pWMVDec->iYLengthX > 0) {
                Affine_PanYLineC (pWMVDec, pTmpY, pFrameY);
                pFrameY += pWMVDec->iYLengthX;
            }
#       endif

        if (iStartX <= iEndXm2) {
            for (x=iEndXm2; x<iEndX; x++) {
                pTmp = (unsigned char *)(pTmpY + x-iStartX);
                *pFrameY++ = *pTmp;
            }
        }

        for (x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 16;
        }

        pFrameYRef += pWMVDec->m_iNewWidth;
    }

    if (iEndY >= ((iStartY + 2))) {
        for (y=iEndY-2; y<pWMVDec->m_iNewHeight; y++) {
            pFrameY = pFrameYRef;
            pTmpY = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*(iy+y-iStartY) + ix);
            for (x=0; x<pWMVDec->m_iNewWidth; x++) {
                if (((iy+y-iStartY)<pWMVDec->m_iOldHeight)&&((ix+x-iStartX)<pWMVDec->m_iOldWidth)&&((ix+x-iStartX)>=0))
                    *pFrameY++ = *(pTmpY+x-iStartX);
                else
                    *pFrameY++ = 16; 
            }
            pFrameU = pFrameURef;
            pFrameV = pFrameVRef;
            offset = iOldWidth_2*(iy_2 + ((y-iStartY)>>1)) + ix_2;
            for (x=0; x<(iNewWidth_2); x ++) {
                if ((iy_2 + ((y-iStartY)>>1)<(pWMVDec->m_iOldHeight>>1))&&((ix_2+x-iStartX_2)<iOldWidth_2)&&((ix_2+x-iStartX_2)>=0)) {
                    *pFrameU++ = *(pTmpU + offset +x-iStartX_2);
                    *pFrameV++ = *(pTmpV + offset +x-iStartX_2);
                }
                else {
                    *pFrameU++ = 128;
                    *pFrameV++ = 128;
                }
            }
            pFrameYRef += pWMVDec->m_iNewWidth;
            pFrameURef += iNewWidth_2;
            pFrameVRef += iNewWidth_2;

            pFrameY = pFrameYRef;
            y++;
            pTmpY = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*(iy+y-iStartY) + ix);
            for (x=0; x<pWMVDec->m_iNewWidth; x++) {
                if (((iy+y-iStartY)<pWMVDec->m_iOldHeight)&&((ix+x-iStartX)<pWMVDec->m_iOldWidth)&&((ix+x-iStartX)>=0))
                    *pFrameY++ = *(pTmpY+x-iStartX);
                else
                    *pFrameY++ = 16; 
            }

            pFrameYRef += pWMVDec->m_iNewWidth;
        }
    }

    return tWMVStatus;
}


tWMVDecodeStatus Affine_PanCFading (HWMVDecoder* phWMVDecoder, double fX, double fY, double fFading)
{
    int x, y, offset;
    unsigned char *pTmpRef;
    tWMVDecInternalMember *pWMVDec; 
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    unsigned char *pTmpY;
    unsigned char *pTmpU;
    unsigned char *pTmpV;

    int iOldWidth_2, iNewWidth_2;
    long iStartX, iEndX, iStartY, iEndY;
    long iStartX_2;
    long iEndX_2  ;
    long factor;
    long power;
    long power_1;
    long mask;
    long lA; 
    long lC;
    long lE;
    long lF;
    long lAx2; 
    long iOldWidthxFactor ;
    long iOldHeightxFactor;
    unsigned char * pFrameYRef;
    unsigned char * pFrameURef;
    unsigned char * pFrameVRef;
    unsigned char * pFrameY;
    unsigned char * pFrameU;
    unsigned char * pFrameV;
    long oy ;
    long oxRef;
    long ox ;
    long rx ;
    long ry ;
    long ix ;
    long iy ;

    long iy_2 ;
    long ry_2 ;
    long ix_2 ;
    long rx_2 ;
    long rxry ;
    long lrxry;
#   ifndef WMV_OPT_SPRITE
        unsigned char *pTmp;
        long U00 ;
        long U10 ;
        long U01 ;
        long U11 ;
#   endif //WMV_OPT_SPRITE
    unsigned long    iFading;

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_PanCFading);

    pTmpY = (unsigned char *)pWMVDec->m_dibBitsY;
    pTmpU = (unsigned char *)pWMVDec->m_dibBitsU;
    pTmpV = (unsigned char *)pWMVDec->m_dibBitsV;
    iOldWidth_2 = pWMVDec->m_iOldWidth>>1;
    iNewWidth_2 = pWMVDec->m_iNewWidth>>1;

#   ifdef WMV_OPT_SPRITE
        // reduce floating point by noting that floor(-x) = -ceil(x)
        iStartX   = -(long)(floor(fX));
        if ( iStartX < 0 )
            iStartX = 0;
        if ( pWMVDec->m_iNewWidth < iStartX )
            iStartX = pWMVDec->m_iNewWidth;
        iEndX     = pWMVDec->m_iOldWidth - (long)ceil( fX );
        if ( pWMVDec->m_iNewWidth < iEndX )
            iEndX = pWMVDec->m_iNewWidth;
        if ( iEndX < 0 )
            iEndX = 0;

        iStartY   = -(long)(floor(fY));
        if ( iStartY < 0 )
            iStartY = 0;
        if ( pWMVDec->m_iNewHeight < iStartY )
            iStartY = pWMVDec->m_iNewHeight;
        iEndY     = pWMVDec->m_iOldHeight - (long)ceil( fY );
        if ( pWMVDec->m_iNewHeight < iEndY )
            iEndY = pWMVDec->m_iNewHeight;
        if ( iEndY < 0 )
            iEndY = 0;
#   else
        iStartX   = (long)(ceil(max( 0.0, -fX)));
        iStartX   = (long)(floor(min( (double)(pWMVDec->m_iNewWidth ), (double)(iStartX))));
        iEndX     = (long)(floor(min( (double)(pWMVDec->m_iNewWidth ), (double)(pWMVDec->m_iOldWidth )-fX)));
        iEndX     = (long)(ceil(max( 0.0, (double)(iEndX))));

        iStartY   = (long)(ceil(max( 0.0, -fY)));
        iStartY   = (long)(floor(min( (double)(pWMVDec->m_iNewHeight), (double)(iStartY))));
        iEndY     = (long)(floor(min( (double)(pWMVDec->m_iNewHeight), (double)(pWMVDec->m_iOldHeight)-fY)));
        iEndY     = (long)(ceil(max( 0.0, (double)(iEndY))));
#   endif

    iStartX   = (((iStartX + 1)>>1)<<1);
    iEndX     = (((iEndX      )>>1)<<1);
    iStartY   = (((iStartY + 1)>>1)<<1);
    iEndY     = (((iEndY      )>>1)<<1);
    if (iEndY < 2) iEndY = 2;
    iStartX_2 = iStartX>>1;
    iEndX_2   = iEndX  >>1;

    factor = 2048;
    power  = 11;
    power_1= 12;
    mask   = 0x7FF;

    lA = (long)(factor); 
    lC = (long)(fX * factor);
    lE = (long)(factor);
    lF = (long)(fY * factor);
    lAx2 = (long)(2 * factor); 

    iOldWidthxFactor  = pWMVDec->m_iOldWidth*factor;
    iOldHeightxFactor = pWMVDec->m_iOldHeight*factor;

    //    unsigned char * pFrameYRef = m_pFrameY + m_iNewWidth * iStartY  + iStartX;
    //    unsigned char * pFrameURef = m_pFrameU + (m_iNewWidth>>1) * (iStartY >> 1) + (iStartX>>1);
    //    unsigned char * pFrameVRef = m_pFrameV + (m_iNewWidth>>1) * (iStartY >> 1) + (iStartX>>1);
    pFrameYRef = pWMVDec->m_pFrameY;
    pFrameURef = pWMVDec->m_pFrameU;
    pFrameVRef = pWMVDec->m_pFrameV;

    oy = lE*iStartY +lF;
    oxRef = lC + lA * iStartX;
    ox = oxRef;
    rx = ox & mask;
    ry = oy & mask;
    ix = ox >> power;
    iy = oy >> power;

    iy_2 = oy >> power_1;
    ry_2 = (oy>>1) & mask;
    ix_2 = ox >> power_1;
    rx_2 = (ox>>1) & mask;
    rxry = rx_2*ry_2;
    lrxry = factor-rx_2-ry_2;

    if (fFading > 1.0)
        fFading = 1.0;

    if (fFading < 0.0)
        fFading = 0.0;

    iFading = (long)(fFading * 256.0);

#   ifdef WMV_OPT_SPRITE
        pWMVDec->power = power;
        pWMVDec->factor_rx_ry = factor - rx - ry;
        pWMVDec->rx = rx;
        pWMVDec->ry = ry;
        pWMVDec->rx_2 = rx_2;
        pWMVDec->ry_2 = ry_2;
        pWMVDec->rxry = rxry;
        pWMVDec->rxXry = rx * ry;
        pWMVDec->lrxry = lrxry;
        pWMVDec->iYLengthX = iEndX - iStartX;
        pWMVDec->iUVLengthX = iEndX_2 - iStartX_2;
        pWMVDec->iFading = iFading;
#   endif

    for (y=0; y<iStartY; y++) {
        pFrameY = pFrameYRef;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        for (x=0; x<iNewWidth_2; x ++) {
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;
        y++;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
    }

    for (y=iStartY; y<iEndY-2; y++) {

        pFrameY = pFrameYRef;

        pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*(iy+y-iStartY) + ix);
        for (x=0; x<iStartX; x++) {
            *pFrameY++ = 0;
        }

#       ifndef WMV_OPT_SPRITE
            for (x=iStartX; x<iEndX; x++) {
                pTmp = (unsigned char *)(pTmpRef + x-iStartX);

                U00 = *(pTmp                  );
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + pWMVDec->m_iOldWidth);
                U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

                *pFrameY++ = (unsigned char)((((U00*(factor-rx-ry)+ U01*ry + U10*rx 
                                            + (((U00+U11-U01-U10)*rx*ry) >> power))>> power)*iFading) >> 8 );
            }
#       else
            if (pWMVDec->iYLengthX > 0) {
                Affine_PanFadingYLineC (pWMVDec, pTmpRef, pFrameY);
                pFrameY += pWMVDec->iYLengthX;
            }
#       endif // WMV_OPT_SPRITE

        for (x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0;
        }

        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        offset = iOldWidth_2*(iy_2 + (y>>1) - (iStartY>>1)) + ix_2;

        for (x=0; x<iStartX_2; x ++) {
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }

#       ifndef WMV_OPT_SPRITE
            for (x=iStartX_2; x<iEndX_2; x ++) {

                pTmp = (unsigned char *)(pTmpU + offset +x-iStartX_2);

                U00 = *(pTmp);
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + iOldWidth_2);
                U11 = *(pTmp + 1 + iOldWidth_2);

                *pFrameU++ = (unsigned char)
                    ((((((U00*(lrxry) + U01*ry_2 + U10*rx_2 + (((U00+U11-U01-U10)*rxry) >> power)) >> power) - 128) * iFading) >> 8 ) + 128);

                pTmp = (unsigned char *)(pTmpV + offset+ x-iStartX_2);

                U00 = *(pTmp);
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + iOldWidth_2);
                U11 = *(pTmp + 1 + iOldWidth_2);

                *pFrameV++ = (unsigned char)
                    ((((((U00*(lrxry) + U01*ry_2 + U10*rx_2 + (((U00+U11-U01-U10)*rxry)>>power)) >> power) - 128) * iFading) >> 8 ) + 128);
            }
#       else
            if (pWMVDec->iUVLengthX > 0) {
                Affine_PanFadingUVLineC (pWMVDec, pTmpU + offset, pTmpV + offset, pFrameU, pFrameV);
                pFrameU += pWMVDec->iUVLengthX;
                pFrameV += pWMVDec->iUVLengthX;
            }
#       endif

        for (x=iEndX_2; x<iNewWidth_2; x ++) {
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;

        y++;
 
        pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*(iy+y-iStartY) + ix);

        for (x=0; x<iStartX; x++) {
            *pFrameY++ = 0;
        }

#       ifndef WMV_OPT_SPRITE
            for (x=iStartX; x<iEndX; x++) {
                pTmp = (unsigned char *)(pTmpRef + x-iStartX);

                U00 = *(pTmp                  );
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + pWMVDec->m_iOldWidth);
                U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

                *pFrameY++ = (unsigned char)((((U00*(factor-rx-ry)+ U01*ry + U10*rx 
                                            + (((U00+U11-U01-U10)*rx*ry) >> power)) >> power)*iFading)>>8);
            }
#       else
            if ( pWMVDec->iYLengthX > 0) {
                Affine_PanFadingYLineC (pWMVDec, pTmpRef, pFrameY);
                pFrameY += pWMVDec->iYLengthX;
            }
#       endif //WMV_OPT_SPRITE

        for (x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0;
        }

        pFrameYRef += pWMVDec->m_iNewWidth;
    }

    if (iEndY >= ((iStartY + 2))) {
        for (y=iEndY-2; y<pWMVDec->m_iNewHeight; y++) {
            pFrameY = pFrameYRef;
            pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*(iy+y-iStartY) + ix);
            for (x=0; x<pWMVDec->m_iNewWidth; x++) {
                if (((iy+y-iStartY)<pWMVDec->m_iOldHeight)&&((ix+x-iStartX)<pWMVDec->m_iOldWidth)&&((ix+x-iStartX)>=0))
                    *pFrameY++ = (unsigned char) (( (long)(*(pTmpRef+x-iStartX))* iFading) >> 8);
                else
                    *pFrameY++ = 0; 
            }
            pFrameU = pFrameURef;
            pFrameV = pFrameVRef;
            offset = iOldWidth_2*(iy_2 + ((y-iStartY)>>1)) + ix_2;
            for (x=0; x<(iNewWidth_2); x ++) {
                if ((iy_2 + ((y-iStartY)>>1)<(pWMVDec->m_iOldHeight>>1))&&((ix_2+x-iStartX_2)<iOldWidth_2)&&((ix_2+x-iStartX_2)>=0)) {
                    *pFrameU++ = (unsigned char)(((( (long)(*(pTmpU + offset +x-iStartX_2))-128)* iFading) >> 8)+128);
                    *pFrameV++ = (unsigned char) (((( (long)(*(pTmpV + offset +x-iStartX_2))-128)* iFading) >> 8)+128);
                }
                else {
                    *pFrameU++ = 128;
                    *pFrameV++ = 128;
                }
            }
            pFrameYRef += pWMVDec->m_iNewWidth;
            pFrameURef += iNewWidth_2;
            pFrameVRef += iNewWidth_2;

            pFrameY = pFrameYRef;
            y++;
            pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*(iy+y-iStartY) + ix);
            for (x=0; x<pWMVDec->m_iNewWidth; x++) {
                if (((iy+y-iStartY)<pWMVDec->m_iOldHeight)&&((ix+x-iStartX)<pWMVDec->m_iOldWidth)&&((ix+x-iStartX)>=0))
                    *pFrameY++ = (unsigned char) (( (long)(*(pTmpRef+x-iStartX))* iFading) >> 8);
                else
                    *pFrameY++ = 0; 
            }
            pFrameYRef += pWMVDec->m_iNewWidth;
        }
    }

    return tWMVStatus;
}


tWMVDecodeStatus Affine_PanZoomC ( HWMVDecoder* phWMVDecoder,double fZoom, double fX, double fY)
{

    tWMVDecInternalMember *pWMVDec; 
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    unsigned char *pTmp, *pTmpRef;

    unsigned char *pTmpY;
    unsigned char *pTmpU;
    unsigned char *pTmpV;

    int x, y, offset, iOldWidth_2, iNewWidth_2;
    long iStartX, iEndX, iStartY, iEndY, iEndXX;
    long iStartX_2;
    long iEndX_2  ;
    long iEndXX_2;
    long factor;
    long power;
    long power_1;
    long mask;
    long lA; 
    long lC;
    long lE;
    long lF;
    long lAx2; 
    long iOldWidthxFactor ;
    long iOldHeightxFactor;
    unsigned char * pFrameYRef;
    unsigned char * pFrameURef;
    unsigned char * pFrameVRef;
    unsigned char * pFrameY;
    unsigned char * pFrameU;
    unsigned char * pFrameV;
    long oy ;
    long oxRef;
    long ox ;
    long ry ;
    long iy ;
    double fStartX, fEndX, fStartY, fEndY;

#   ifndef WMV_OPT_SPRITE
        double fEndXX;
        long U00 ;
        long U10 ;
        long U01 ;
        long U11 ;
        long ix ;
        long rx ;
        long rxry ;
        long lrxry;
#   endif

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_PanZoomC);

    pTmpY = (unsigned char *)pWMVDec->m_dibBitsY;
    pTmpU = (unsigned char *)pWMVDec->m_dibBitsU;
    pTmpV = (unsigned char *)pWMVDec->m_dibBitsV;
    iOldWidth_2 = pWMVDec->m_iOldWidth>>1;
    iNewWidth_2 = pWMVDec->m_iNewWidth>>1;

    if (fZoom) {
        fStartX   = -fX/fZoom;
        fEndX     = (((double)(pWMVDec->m_iOldWidth )-fX)/fZoom);
        if (fStartX > fEndX) {
            double fTmp = fStartX;
            fStartX = fEndX;
            fEndX = fTmp;
        }
#       ifdef WMV_OPT_SPRITE
        // reduce floating point
            iStartX   = (long)(ceil(fStartX));
            if ( iStartX < 0 )
                iStartX = 0;
            if ( pWMVDec->m_iNewWidth < iStartX )
                iStartX = pWMVDec->m_iNewWidth;
            iEndXX = (long)floor(fEndX + 0.999);
            if ( iEndXX > pWMVDec->m_iNewWidth )
                iEndXX = pWMVDec->m_iNewWidth;
            if ( iEndXX < 0 )
                iEndXX = 0;
            iEndX  = (long)floor(fEndX);
            if ( iEndX > pWMVDec->m_iNewWidth )
                iEndX = pWMVDec->m_iNewWidth;
            if ( iEndX < 0 )
                iEndX = 0;
#       else
            fStartX   = ceil(max( 0.0, fStartX));
            iStartX   = (long)(floor(min( (double)(pWMVDec->m_iNewWidth ), fStartX)));
            fEndXX    = floor(min( (double)(pWMVDec->m_iNewWidth ), fEndX + 0.999));
            fEndX     = floor(min( (double)(pWMVDec->m_iNewWidth ), fEndX));
            iEndX     = (long)(ceil(max( 0.0, fEndX)));
            iEndXX    = (long)(ceil(max( 0.0, fEndXX)));
#       endif

        fStartY   = -fY/fZoom;
        fEndY     = (((double)(pWMVDec->m_iOldHeight )-fY)/fZoom) + 1.0;
        if (fStartY > fEndY) {
            double fTmp = fStartY;
            fStartY = fEndY;
            fEndY = fTmp;
        }
#       ifdef WMV_OPT_SPRITE
            iStartY   = (long)ceil(fStartY);
            if ( iStartY < 0 )
                iStartY = 0;
            if ( pWMVDec->m_iNewHeight< iStartY )
                iStartY = pWMVDec->m_iNewHeight;
            iEndY     = (long)ceil(fEndY);
            if ( pWMVDec->m_iNewHeight < iEndY )
                iEndY = pWMVDec->m_iNewHeight;
            if ( iEndY < 0 )
                iEndY = 0;
#       else
            fStartY   = ceil(max( 0.0, fStartY));
            iStartY   = (long)(floor(min( (double)(pWMVDec->m_iNewHeight), fStartY)));
            fEndY     = floor(min( (double)(pWMVDec->m_iNewHeight), fEndY));
            iEndY     = (long)(ceil(max( 0.0, fEndY)));
#       endif
    }
    else {
#       ifndef WMV_OPT_SPRITE
            fStartX = fEndX = 0 ;
            fStartY = fEndY = 0 ;
#       endif
        iStartX = iEndX = 0 ;
        iStartY = iEndY = 0 ;
		return tWMVStatus;
    }
    iStartX   = (((iStartX + 1)>>1)<<1);
    iEndX     = (((iEndX      )>>1)<<1);
    iEndXX    = (((iEndXX     )>>1)<<1);
    iStartY   = (((iStartY + 1)>>1)<<1);
    iEndY     = (((iEndY      )>>1)<<1);
    if (iEndY < 2) iEndY = 2;
    iStartX_2 = iStartX>>1;
    iEndX_2   = iEndX  >>1;
    iEndXX_2  = iEndXX >>1;

    factor = 2048;
    power  = 11;
    power_1= 12;
    mask   = 0x7FF;

    lA = (long)(fZoom * factor); 
    lC = (long)(fX * factor);
    lE = (long)(fZoom * factor);
    lF = (long)(fY * factor);
    lAx2 = (long)(2*fZoom * factor); 

    iOldWidthxFactor  = pWMVDec->m_iOldWidth*factor;
    iOldHeightxFactor = pWMVDec->m_iOldHeight*factor;

    //    unsigned char * pFrameYRef = m_pFrameY + m_iNewWidth * iStartY  + iStartX;
    //    unsigned char * pFrameURef = m_pFrameU + (m_iNewWidth>>1) * (iStartY >> 1) + (iStartX>>1);
    //    unsigned char * pFrameVRef = m_pFrameV + (m_iNewWidth>>1) * (iStartY >> 1) + (iStartX>>1);
    pFrameYRef = pWMVDec->m_pFrameY;
    pFrameURef = pWMVDec->m_pFrameU;
    pFrameVRef = pWMVDec->m_pFrameV;

    oy = lE*iStartY +lF;
    oxRef = lC + lA * iStartX;

    if (oy < 0) 
        oy = 0;

    if (oxRef < 0) 
        oxRef = 0;

#   ifdef WMV_OPT_SPRITE
        pWMVDec->power = power;
        pWMVDec->power_1 = power_1;
        pWMVDec->factor = factor;
        pWMVDec->iYLengthX = iEndX - iStartX;
        pWMVDec->iUVLengthX = iEndX_2 - iStartX_2;
        pWMVDec->oxRef = oxRef;
        pWMVDec->mask = mask;
        pWMVDec->lA = lA;
        pWMVDec->lAx2 = lAx2;
#   endif

    for (y=0; y<iStartY; y++) {
        pFrameY = pFrameYRef;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        for (x=0; x<iNewWidth_2; x ++) {
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;
        y++;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
    }

    for (y=iStartY; y<iEndY-2; y++) {
        ox = oxRef;
        iy = oy >> power;
        ry = oy & mask;

        pFrameY = pFrameYRef;

        pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);
        for (x=0; x<iStartX; x++) {
            *pFrameY++ = 0; 
        }

#       ifndef WMV_OPT_SPRITE
            for (x=iStartX; x<iEndX; x++) {
                ix = ox >> power;
                rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);

                U00 = *(pTmp                  );
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + pWMVDec->m_iOldWidth);
                U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

                *pFrameY++ = (unsigned char)((U00*(factor-rx-ry)+ U01*ry + U10*rx 
                                            + (((U00+U11-U01-U10)*rx*ry) >> power)) >> power);
                ox += lA;
            }
#       else
            if (pWMVDec->iYLengthX > 0) {
                pWMVDec->ry = ry; 
                Affine_StretchYLineC (pWMVDec, pTmpRef, pFrameY);
                pFrameY += pWMVDec->iYLengthX;
            }
#       endif //WMV_OPT_SPRITE

        for (x=iEndX; x<iEndXX; x++) {
            long ix = ox >> power;
            pTmp = (unsigned char *)(pTmpRef + ix);
            *pFrameY++ = *(pTmp);
            ox += lA;
        }

        for (x=iEndXX; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 16;
        }

        ox = oxRef;

        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        iy = oy >> power_1;
        offset = iOldWidth_2*iy;
        ry = (oy>>1) & mask;

        for (x=0; x<iStartX_2; x ++) {
            *pFrameU++ = (unsigned char)128; 
            *pFrameV++ = (unsigned char)128;
        }

#       ifndef WMV_OPT_SPRITE
            for (x=iStartX_2; x<iEndX_2; x ++) {
                ix = ox >> power_1;
                rx = (ox>>1) & mask;
                rxry = rx*ry;
                lrxry = factor-rx-ry;

                pTmp = (unsigned char *)(pTmpU + offset + ix);

                U00 = *(pTmp);
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + iOldWidth_2);
                U11 = *(pTmp + 1 + iOldWidth_2);

                *pFrameU++ = (unsigned char)((U00*(lrxry) + U01*ry+ U10*rx+ 
                                    (((U00+U11-U01-U10)*rxry) >> power)) >> power);

                pTmp = (unsigned char *)(pTmpV + offset+ ix);

                U00 = *(pTmp);
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + iOldWidth_2);
                U11 = *(pTmp + 1 + iOldWidth_2);

                *pFrameV++ = (unsigned char)((U00*(lrxry) + U01*ry+ U10*rx+ 
                                    (((U00+U11-U01-U10)*rxry)>>power)) >> power);
                ox += lAx2;
            }
#       else
            if (pWMVDec->iUVLengthX > 0) {
                pWMVDec->ry = ry; 
                Affine_StretchUVLineC (pWMVDec, pTmpU + offset, pTmpV + offset, pFrameU, pFrameV);
                pFrameU += pWMVDec->iUVLengthX;  
                pFrameV += pWMVDec->iUVLengthX;
            }
#       endif //WMV_OPT_SPRITE

        for (x=iEndX_2; x<iEndXX_2; x ++) {
            long ix = ox >> power_1;

            pTmp = (unsigned char *)(pTmpU + offset + ix);
            *pFrameU++ = *(pTmp);

            pTmp = (unsigned char *)(pTmpV + offset + ix);
            *pFrameV++ = *(pTmp);

            ox += lAx2;
        }

        for (x=iEndXX_2; x<iNewWidth_2; x ++) {
            *pFrameU++ = (unsigned char)128; 
            *pFrameV++ = (unsigned char)128;
        }

        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;

        y++;
        oy += lE;
        ox = oxRef;
        iy = oy >> power;
        ry = oy & mask;
 
        pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);

        for (x=0; x<iStartX; x++) {
            *pFrameY++ = 0; 
        }

#       ifndef WMV_OPT_SPRITE
            for (x=iStartX; x<iEndX; x++) {
                long ix = ox >> power;
                long rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);

                U00 = *(pTmp                  );
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + pWMVDec->m_iOldWidth);
                U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

                *pFrameY++ = (unsigned char)((U00*(factor-rx-ry)+ U01*ry + U10*rx 
                                            + (((U00+U11-U01-U10)*rx*ry) >> power)) >> power);

                ox += lA;
            }
#       else
            if (pWMVDec->iYLengthX > 0) {
                pWMVDec->ry = ry; 
                Affine_StretchYLineC (pWMVDec, pTmpRef, pFrameY);
                pFrameY += pWMVDec->iYLengthX;
            }
#       endif //WMV_OPT_SPRITE

        for (x=iEndX; x<iEndXX; x++) {
            long ix = ox >> power;
            pTmp = (unsigned char *)(pTmpRef + ix);
            *pFrameY++ = *(pTmp);
            ox += lA;
        }

        for (x=iEndXX; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 16;
        }

        pFrameYRef += pWMVDec->m_iNewWidth;
        oy += lE;
    }

    ox = oxRef;
    iy = oy >> power;
    if (iy >= pWMVDec->m_iOldHeight)
        iy = pWMVDec->m_iOldHeight  - 1;

    ry = oy & mask;

    if (iEndY >= ((iStartY + 2))) {
        for (y=iEndY-2; y<iEndY; y++) {
            pFrameY = pFrameYRef;
            for (x=0; x<iStartX; x++) {
                *pFrameY++ = 0; 
            }
            pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);
            for ( x=iStartX; x<iEndX; x++) {
                long ix = ox >> power;
                //long rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);
                *pFrameY++ = *pTmp ; 
                ox += lA;
            }
            for ( x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
                *pFrameY++ = 0; 
            }
            pFrameU = pFrameURef;
            pFrameV = pFrameVRef;
            for (x=0; x<iStartX_2; x ++) {
                *pFrameU++ = 128;
                *pFrameV++ = 128;
            }
            ox = oxRef;
            iy = oy >> power_1;
            if (iy >= (pWMVDec->m_iOldHeight >> 1) )
                iy = (pWMVDec->m_iOldHeight >> 1) - 1;

            offset = iOldWidth_2*iy;
            ry = (oy>>1) & mask;
            for (x=iStartX_2; x<iEndX_2; x ++) {
                long ix = ox >> power_1;
                //long rx = (ox>>1) & mask;
                //long rxry = rx*ry;
                //long lrxry = factor-rx-ry;
                pTmp = (unsigned char *)(pTmpU + offset + ix);
                *pFrameU++ = *pTmp;
                pTmp = (unsigned char *)(pTmpV + offset + ix);
                *pFrameV++ = *pTmp;
                ox += lAx2;
            }
            for (x=iEndX_2; x<(iNewWidth_2); x ++) {
                *pFrameU++ = 128;
                *pFrameV++ = 128;
            }
            pFrameYRef += pWMVDec->m_iNewWidth;
            pFrameURef += iNewWidth_2;
            pFrameVRef += iNewWidth_2;

            pFrameY = pFrameYRef;
            y++;
            oy += lE;
            ox = oxRef;
            iy = oy >> power;
            if (iy >= pWMVDec->m_iOldHeight)
                iy = pWMVDec->m_iOldHeight - 1;

            ry = oy & mask;
            for ( x=0; x<iStartX; x++) {
                *pFrameY++ = 0; 
            }
            pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);
            for (x=iStartX; x<iEndX; x++) {
                long ix = ox >> power;
                //long rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);
                *pFrameY++ = *pTmp ; 
                ox += lA;
            }
            for (x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
                *pFrameY++ = 0; 
            }
            pFrameYRef += pWMVDec->m_iNewWidth;
        }
    }
    for (y=iEndY; y<pWMVDec->m_iNewHeight; y++) {
        pFrameY = pFrameYRef;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        for (x=0; x<iNewWidth_2; x ++) {
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;
        y++;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
    }

    return tWMVStatus;
}


tWMVDecodeStatus Affine_PanStretchC ( HWMVDecoder* phWMVDecoder, double fZoomX, double fX, double fZoomY, double fY)
{

    tWMVDecInternalMember *pWMVDec; 
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    unsigned char *pTmpRef;
    unsigned char *pTmp;
    unsigned char *pTmpY;
    unsigned char *pTmpU;
    unsigned char *pTmpV;

    int x, y, offset, iOldWidth_2, iNewWidth_2;
    long iStartX, iEndX, iStartY, iEndY, iEndXX;
    long iStartX_2;
    long iEndX_2, iEndXX_2  ;
    long factor;
    long power;
    long power_1;
    long mask;
    long lA; 
    long lC;
    long lE;
    long lF;
    long lAx2; 
    long iOldWidthxFactor ;
    long iOldHeightxFactor;
    unsigned char * pFrameYRef;
    unsigned char * pFrameURef;
    unsigned char * pFrameVRef;
    unsigned char * pFrameY;
    unsigned char * pFrameU;
    unsigned char * pFrameV;
    long oy ;
    long oxRef;
    long ox ;    
    long ry ;
    long iy ;
    long ix ;
    long rx ;

#   ifndef WMV_OPT_SPRITE
        long U10 ;
        long U11 ;
        long U00 ;
        long U01 ;
        long lrxry;
        long rxry ;
#   endif //WMV_OPT_SPRITE

    double fStartX, fEndX, fStartY, fEndY;

#   ifndef WMV_OPT_SPRITE
        double fEndXX;
#   endif

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_PanStretchC);

    pTmpY = (unsigned char *)pWMVDec->m_dibBitsY;
    pTmpU = (unsigned char *)pWMVDec->m_dibBitsU;
    pTmpV = (unsigned char *)pWMVDec->m_dibBitsV;
    iOldWidth_2 = pWMVDec->m_iOldWidth>>1;
    iNewWidth_2 = pWMVDec->m_iNewWidth>>1;

    if (fZoomX  && fZoomY) {
        fStartX   = -fX/fZoomX;
        fEndX     = (((double)(pWMVDec->m_iOldWidth )-fX)/fZoomX);
        if (fStartX > fEndX) {
            double fTmp = fStartX;
            fStartX = fEndX;
            fEndX = fTmp;
        }
#       ifdef WMV_OPT_SPRITE
            iStartX   = (long)ceil(fStartX);
            if ( iStartX < 0 )
                iStartX = 0;
            if ( pWMVDec->m_iNewWidth < iStartX )
                iStartX = pWMVDec->m_iNewWidth;
            iEndXX  = (long)floor(fEndX + 0.999);
            if ( pWMVDec->m_iNewWidth < iEndXX )
                iEndXX = pWMVDec->m_iNewWidth;
            if ( iEndXX < 0 )
                iEndXX = 0;
            iEndX      = (long)floor(fEndX);
            if ( pWMVDec->m_iNewWidth < iEndX )
                iEndX = pWMVDec->m_iNewWidth;
            if ( iEndX < 0 )
                iEndX = 0;
#       else
            fStartX   = ceil(max( 0.0, fStartX));
            iStartX   = (long)(floor(min( (double)(pWMVDec->m_iNewWidth ), fStartX)));
            fEndXX    = floor(min( (double)(pWMVDec->m_iNewWidth ), fEndX + 0.999));
            fEndX     = floor(min( (double)(pWMVDec->m_iNewWidth ), fEndX));
            iEndX     = (long)(ceil(max( 0.0, fEndX)));
            iEndXX    = (long)(ceil(max( 0.0, fEndXX)));
#       endif

        fStartY   = -fY/fZoomY;
        fEndY     = (((double)(pWMVDec->m_iOldHeight )-fY)/fZoomY) + 1.0;
        if (fStartY > fEndY) {
            double fTmp = fStartY;
            fStartY = fEndY;
            fEndY = fTmp;
        }

#       ifdef WMV_OPT_SPRITE
            iStartY   = (long)ceil(fStartY);
            if ( iStartY < 0 )
                iStartY = 0;
            if ( pWMVDec->m_iNewHeight < iStartY )
                iStartY = pWMVDec->m_iNewHeight;
            iEndY     = (long)floor(fEndY);
            if ( pWMVDec->m_iNewHeight < iEndY )
                iEndY = pWMVDec->m_iNewHeight;
            if ( iEndY < 0 )
                iEndY = 0;
#       else
            fStartY   = ceil(max( 0.0, fStartY));
            iStartY   = (long)(floor(min( (double)(pWMVDec->m_iNewHeight), fStartY)));
            fEndY     = floor(min( (double)(pWMVDec->m_iNewHeight), fEndY));
            iEndY     = (long)(ceil(max( 0.0, fEndY)));
#       endif
    }
    else {
#       ifndef WMV_OPT_SPRITE
            fStartX = fEndX = 0 ;
            fStartY = fEndY = 0 ;
#       endif
        iStartX = iEndX = 0 ;
        iStartY = iEndY = 0 ;
		return tWMVStatus;
    }
    iStartX   = (((iStartX + 1)>>1)<<1);
    iEndX     = (((iEndX      )>>1)<<1);
    iEndXX    = (((iEndXX     )>>1)<<1);
    iStartY   = (((iStartY + 1)>>1)<<1);
    iEndY     = (((iEndY      )>>1)<<1);
    if (iEndY < 2) iEndY = 2;
    iStartX_2 = iStartX>>1;
    iEndX_2   = iEndX  >>1;
    iEndXX_2  = iEndXX >>1;

    factor = 2048;
    power  = 11;
    power_1= 12;
    mask   = 0x7FF;

    lA = (long)(fZoomX * factor); 
    lC = (long)(fX * factor);
    lE = (long)(fZoomY * factor);
    lF = (long)(fY * factor);
    lAx2 = (long)(2*fZoomX * factor); 

    iOldWidthxFactor  = pWMVDec->m_iOldWidth*factor;
    iOldHeightxFactor = pWMVDec->m_iOldHeight*factor;

    //    unsigned char * pFrameYRef = m_pFrameY + m_iNewWidth * iStartY  + iStartX;
    //    unsigned char * pFrameURef = m_pFrameU + (m_iNewWidth>>1) * (iStartY >> 1) + (iStartX>>1);
    //    unsigned char * pFrameVRef = m_pFrameV + (m_iNewWidth>>1) * (iStartY >> 1) + (iStartX>>1);
    pFrameYRef = pWMVDec->m_pFrameY;
    pFrameURef = pWMVDec->m_pFrameU;
    pFrameVRef = pWMVDec->m_pFrameV;

    oy = lE*iStartY +lF;
    oxRef = lC + lA * iStartX;

    if (oy < 0) 
        oy = 0;

    if (oxRef < 0) 
        oxRef = 0;

#   ifdef WMV_OPT_SPRITE
        pWMVDec->power = power;
        pWMVDec->power_1 = power_1;
        pWMVDec->factor = factor;
        pWMVDec->iYLengthX = iEndX - iStartX;
        pWMVDec->iUVLengthX = iEndX_2 - iStartX_2;
        pWMVDec->oxRef = oxRef;
        pWMVDec->mask = mask;
        pWMVDec->lA = lA;
        pWMVDec->lAx2 = lAx2;
#   endif

    for (y=0; y<iStartY; y++) {
        pFrameY = pFrameYRef;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        for (x=0; x<iNewWidth_2; x ++) {
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;
        y++;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
    }

    for (y=iStartY; y<iEndY-2; y++) {
        ox = oxRef;
        iy = oy >> power;
        ry = oy & mask;

        pFrameY = pFrameYRef;

        pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);
        for (x=0; x<iStartX; x++) {
            *pFrameY++ = 0; 
        }

#       ifndef WMV_OPT_SPRITE
            for (x=iStartX; x<iEndX; x++) {
                ix = ox >> power;
                rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);

                U00 = *(pTmp                  );
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + pWMVDec->m_iOldWidth);
                U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

                *pFrameY++ = (unsigned char)((U00*(factor-rx-ry)+ U01*ry + U10*rx 
                                            + (((U00+U11-U01-U10)*rx*ry) >> power)) >> power);
                ox += lA;
            }
#       else
            if (pWMVDec->iYLengthX > 0) {
                pWMVDec->ry = ry; 
                Affine_StretchYLineC (pWMVDec, pTmpRef, pFrameY);
                pFrameY += pWMVDec->iYLengthX;
            }
#       endif //WMV_OPT_SPRITE

#       ifndef SH3_COMPILER
            for (x=iEndX; x<iEndXX; x++) {
                long ix = ox >> power;
                pTmp = (unsigned char *)(pTmpRef + ix);
                *pFrameY++ = *(pTmp);
                ox += lA;
            }

            for (x=iEndXX; x<pWMVDec->m_iNewWidth; x++) {
                *pFrameY++ = 16;
            }
#       else
            for (x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
                *pFrameY++ = 16;
            }
#       endif

        ox = oxRef;

        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        iy = oy >> power_1;
        offset = iOldWidth_2*iy;
        ry = (oy>>1) & mask;

        for (x=0; x<iStartX_2; x ++) {
            *pFrameU++ = (unsigned char)128; 
            *pFrameV++ = (unsigned char)128;
        }

#       ifndef WMV_OPT_SPRITE
            for (x=iStartX_2; x<iEndX_2; x ++) {
                ix = ox >> power_1;
                rx = (ox>>1) & mask;
                rxry = rx*ry;
                lrxry = factor-rx-ry;

                pTmp = (unsigned char *)(pTmpU + offset + ix);

                U00 = *(pTmp);
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + iOldWidth_2);
                U11 = *(pTmp + 1 + iOldWidth_2);

                *pFrameU++ = (unsigned char)((U00*(lrxry) + U01*ry+ U10*rx+ 
                                    (((U00+U11-U01-U10)*rxry) >> power)) >> power);

                pTmp = (unsigned char *)(pTmpV + offset+ ix);

                U00 = *(pTmp);
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + iOldWidth_2);
                U11 = *(pTmp + 1 + iOldWidth_2);

                *pFrameV++ = (unsigned char)((U00*(lrxry) + U01*ry+ U10*rx+ 
                                    (((U00+U11-U01-U10)*rxry)>>power)) >> power);
                ox += lAx2;
            }
#       else
            if (pWMVDec->iUVLengthX > 0) {
                pWMVDec->ry = ry; 
                Affine_StretchUVLineC (pWMVDec, pTmpU + offset, pTmpV + offset, pFrameU, pFrameV);
                pFrameU += pWMVDec->iUVLengthX;  
                pFrameV += pWMVDec->iUVLengthX;
            }
#       endif //WMV_OPT_SPRITE

#       ifndef SH3_COMPILER
            for (x=iEndX_2; x<iEndXX_2; x ++) {
                long ix = ox >> power_1;

                pTmp = (unsigned char *)(pTmpU + offset + ix);
                *pFrameU++ = *(pTmp);

                pTmp = (unsigned char *)(pTmpV + offset + ix);
                *pFrameV++ = *(pTmp);

                ox += lAx2;
            }
            for (x=iEndXX_2; x<iNewWidth_2; x ++) {
                *pFrameU++ = (unsigned char)128; 
                *pFrameV++ = (unsigned char)128;
            }
#       else
            for (x=iEndX_2; x<iNewWidth_2; x ++) {
                *pFrameU++ = (unsigned char)128; 
                *pFrameV++ = (unsigned char)128;
            }
#       endif

        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;

        y++;
        oy += lE;
        ox = oxRef;
        iy = oy >> power;
        ry = oy & mask;
 
        pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);

        for (x=0; x<iStartX; x++) {
            *pFrameY++ = 0; 
        }

#       ifndef WMV_OPT_SPRITE
            for (x=iStartX; x<iEndX; x++) {
                long ix = ox >> power;
                long rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);

                U00 = *(pTmp                  );
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + pWMVDec->m_iOldWidth);
                U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

                *pFrameY++ = (unsigned char)((U00*(factor-rx-ry)+ U01*ry + U10*rx 
                                            + (((U00+U11-U01-U10)*rx*ry) >> power)) >> power);

                ox += lA;
            }
#       else
            if (pWMVDec->iYLengthX > 0) {
                pWMVDec->ry = ry; 
                Affine_StretchYLineC (pWMVDec, pTmpRef, pFrameY);
                pFrameY += pWMVDec->iYLengthX;
            }
#       endif //WMV_OPT_SPRITE

#       ifndef SH3_COMPILER
            for (x=iEndX; x<iEndXX; x++) {
                long ix = ox >> power;
                pTmp = (unsigned char *)(pTmpRef + ix);
                *pFrameY++ = *(pTmp);
                ox += lA;
            }
            for (x=iEndXX; x<pWMVDec->m_iNewWidth; x++) {
                *pFrameY++ = 16;
            }
#       else
            for (x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
                *pFrameY++ = 16;
            }
#       endif
        pFrameYRef += pWMVDec->m_iNewWidth;
        oy += lE;
    }

    ox = oxRef;
    iy = oy >> power;
    if (iy >= pWMVDec->m_iOldHeight )
        iy = pWMVDec->m_iOldHeight - 1;

    ry = oy & mask;
    if (iEndY >= ((iStartY + 2))) {
        for (y=iEndY-2; y<iEndY; y++) {
            pFrameY = pFrameYRef;
            for (x=0; x<iStartX; x++) {
                *pFrameY++ = 0; 
            }
            pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);
            for ( x=iStartX; x<iEndX; x++) {
                ix = ox >> power;
                rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);
                *pFrameY++ = *pTmp ; 
                ox += lA;
            }
            for ( x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
                *pFrameY++ = 0; 
            }
            pFrameU = pFrameURef;
            pFrameV = pFrameVRef;
            for (x=0; x<iStartX_2; x ++) {
                *pFrameU++ = 128;
                *pFrameV++ = 128;
            }
            ox = oxRef;
            iy = oy >> power_1;
            if (iy >= (pWMVDec->m_iOldHeight >> 1) )
                iy = (pWMVDec->m_iOldHeight >> 1) - 1;

            offset = iOldWidth_2*iy;
            ry = (oy>>1) & mask;
            for (x=iStartX_2; x<iEndX_2; x ++) {
                long ix = ox >> power_1;
                //long rx = (ox>>1) & mask;
                //long rxry = rx*ry;
                //long lrxry = factor-rx-ry;
                pTmp = (unsigned char *)(pTmpU + offset + ix);
                *pFrameU++ = *pTmp;
                pTmp = (unsigned char *)(pTmpV + offset + ix);
                *pFrameV++ = *pTmp;
                ox += lAx2;
            }
            for (x=iEndX_2; x<(iNewWidth_2); x ++) {
                *pFrameU++ = 128;
                *pFrameV++ = 128;
            }
            pFrameYRef += pWMVDec->m_iNewWidth;
            pFrameURef += iNewWidth_2;
            pFrameVRef += iNewWidth_2;

            pFrameY = pFrameYRef;
            y++;
            oy += lE;
            ox = oxRef;
            iy = oy >> power;
            if (iy >= pWMVDec->m_iOldHeight )
                iy = pWMVDec->m_iOldHeight- 1;

            ry = oy & mask;
            for ( x=0; x<iStartX; x++) {
                *pFrameY++ = 0; 
            }
            pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);
            for (x=iStartX; x<iEndX; x++) {
                long ix = ox >> power;
                //long rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);
                *pFrameY++ = *pTmp ; 
                ox += lA;
            }
            for (x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
                *pFrameY++ = 0; 
            }
            pFrameYRef += pWMVDec->m_iNewWidth;
        }
    }
    for (y=iEndY; y<pWMVDec->m_iNewHeight; y++) {
        pFrameY = pFrameYRef;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        for (x=0; x<iNewWidth_2; x ++) {
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;
        y++;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
    }
    return tWMVStatus;
}


tWMVDecodeStatus Affine_PanZoomCFading ( HWMVDecoder* phWMVDecoder,double fZoom, double fX, double fY, double fFading)
{
    tWMVDecInternalMember *pWMVDec; 
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

    unsigned char *pTmp, *pTmpRef;

    unsigned char *pTmpY;
    unsigned char *pTmpU;
    unsigned char *pTmpV;

    int x, y, offset, iOldWidth_2, iNewWidth_2;
    long iStartX, iEndX, iStartY, iEndY, iEndXX, iEndXX_2;
    long iStartX_2;
    long iEndX_2  ;
    long factor;
    long power;
    long power_1;
    long mask;
    long lA; 
    long lC;
    long lE;
    long lF;
    long lAx2; 
    long iOldWidthxFactor ;
    long iOldHeightxFactor;
    unsigned char * pFrameYRef;
    unsigned char * pFrameURef;
    unsigned char * pFrameVRef;
    unsigned char * pFrameY;
    unsigned char * pFrameU;
    unsigned char * pFrameV;
    long oy ;
    long oxRef;
    long ox ;
    long ry ;
    long iy ;


    unsigned long    iFading;
    double fStartX, fEndX, fStartY, fEndY;
#   ifndef WMV_OPT_SPRITE
    double fEndXX  ;
    long rxry ;
    long lrxry;
    long ix ;
    long rx ;
    long U00 ;
    long U10 ;
    long U01 ;
    long U11 ;
#   endif

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_PanZoomCFading);

    pTmpY = (unsigned char *)pWMVDec->m_dibBitsY;
    pTmpU = (unsigned char *)pWMVDec->m_dibBitsU;
    pTmpV = (unsigned char *)pWMVDec->m_dibBitsV;
    iOldWidth_2 = pWMVDec->m_iOldWidth>>1;
    iNewWidth_2 = pWMVDec->m_iNewWidth>>1;
    //factor3 = max( min( fZoom, 1.0), 0.001);

    if (fZoom) {
        fStartX   = -fX/fZoom;
        fEndX     = (((double)(pWMVDec->m_iOldWidth )-fX)/fZoom);
        if (fStartX > fEndX) {
            double fTmp = fStartX;
            fStartX = fEndX;
            fEndX = fTmp;
        }
#       ifdef WMV_OPT_SPRITE
            iStartX   = (long)ceil(fStartX);
            if ( iStartX < 0 )
                iStartX = 0;
            if ( pWMVDec->m_iNewWidth < iStartX )
                iStartX = pWMVDec->m_iNewWidth;
            iEndXX  = (long)floor(fEndX + 0.999);
            if ( pWMVDec->m_iNewWidth < iEndXX )
                iEndXX = pWMVDec->m_iNewWidth;
            if ( iEndXX < 0 )
                iEndXX = 0;
            iEndX      = (long)floor(fEndX);
            if ( pWMVDec->m_iNewWidth < iEndX )
                iEndX = pWMVDec->m_iNewWidth;
            if ( iEndX < 0 )
                iEndX = 0;
#       else
            fStartX   = ceil(max( 0.0, fStartX));
            iStartX   = (long)(floor(min( (double)(pWMVDec->m_iNewWidth ), fStartX)));
            fEndXX    = floor(min( (double)(pWMVDec->m_iNewWidth ), fEndX + 0.999));
            fEndX     = floor(min( (double)(pWMVDec->m_iNewWidth ), fEndX));
            iEndX     = (long)(ceil(max( 0.0, fEndX)));
            iEndXX    = (long)(ceil(max( 0.0, fEndXX)));
#       endif

        fStartY   = -fY/fZoom;
        fEndY     = (((double)(pWMVDec->m_iOldHeight )-fY)/fZoom) + 1.0;

        if (fStartY > fEndY) {
            double fTmp = fStartY;
            fStartY = fEndY;
            fEndY = fTmp;
        }

#       ifdef WMV_OPT_SPRITE
            iStartY   = (long)ceil(fStartY);
            if ( iStartY < 0 )
                iStartY = 0;
            if ( pWMVDec->m_iNewHeight < iStartY )
                iStartY = pWMVDec->m_iNewHeight;
            iEndY     = (long)floor(fEndY);
            if ( pWMVDec->m_iNewHeight < iEndY )
                iEndY = pWMVDec->m_iNewHeight;
            if ( iEndY < 0 )
                iEndY = 0;
#       else
            fStartY   = (ceil(max( 0.0, fStartY)));
            iStartY   = (long)(floor(min( (double)(pWMVDec->m_iNewHeight), fStartY)));
            fEndY     = (floor(min( (double)(pWMVDec->m_iNewHeight), fEndY)));
            iEndY     = (long)(ceil(max( 0.0, fEndY)));
#       endif
    }
    else {
#       ifndef WMV_OPT_SPRITE
            fStartX = fEndX = 0 ;
            fStartY = fEndY = 0 ;
#       endif
        iStartX = iEndX = 0 ;
        iStartY = iEndY = 0 ;
		return tWMVStatus;
    }

    iStartX   = (((iStartX + 1)>>1)<<1);
    iEndX     = (((iEndX      )>>1)<<1);
    iEndXX    = (((iEndXX     )>>1)<<1);
    iStartY   = (((iStartY + 1)>>1)<<1);
    iEndY     = (((iEndY      )>>1)<<1);
    if (iEndY < 2) iEndY = 2;

    iStartX_2 = iStartX>>1;
    iEndX_2   = iEndX  >>1;
    iEndXX_2  = iEndXX >>1;

    factor = 2048;
    power  = 11;
    power_1= 12;
    mask   = 0x7FF;

    lA = (long)(fZoom * factor); 
    lC = (long)(fX * factor);
    lE = (long)(fZoom * factor);
    lF = (long)(fY * factor);
    lAx2 = (long)(2*fZoom * factor); 

    iOldWidthxFactor  = pWMVDec->m_iOldWidth*factor;
    iOldHeightxFactor = pWMVDec->m_iOldHeight*factor;

    //    unsigned char * pFrameYRef = m_pFrameY + m_iNewWidth * iStartY  + iStartX;
    //    unsigned char * pFrameURef = m_pFrameU + (m_iNewWidth>>1) * (iStartY >> 1) + (iStartX>>1);
    //    unsigned char * pFrameVRef = m_pFrameV + (m_iNewWidth>>1) * (iStartY >> 1) + (iStartX>>1);
    pFrameYRef = pWMVDec->m_pFrameY;
    pFrameURef = pWMVDec->m_pFrameU;
    pFrameVRef = pWMVDec->m_pFrameV;

    oy = lE*iStartY +lF;
    oxRef = lC + lA * iStartX;

    if (oy < 0) 
        oy = 0;

    if (oxRef < 0) 
        oxRef = 0;

    if (fFading > 1.0)
        fFading = 1.0;

    if (fFading < 0.0)
        fFading = 0.0;

    iFading = (long)(fFading * 256.0);

#   ifdef WMV_OPT_SPRITE
        pWMVDec->power = power;
        pWMVDec->power_1 = power_1;
        pWMVDec->factor = factor;
        pWMVDec->iYLengthX = iEndX - iStartX;
        pWMVDec->iUVLengthX = iEndX_2 - iStartX_2;
        pWMVDec->oxRef = oxRef;
        pWMVDec->mask = mask;
        pWMVDec->lA = lA;
        pWMVDec->lAx2 = lAx2;
        pWMVDec->iFading = iFading;
#   endif

    for (y=0; y<iStartY; y++) {
        pFrameY = pFrameYRef;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        for (x=0; x<iNewWidth_2; x ++) {
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;
        y++;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
    }

    for (y=iStartY; y<iEndY-2; y++) {
        ox = oxRef;
        iy = oy >> power;
        ry = oy & mask;

        pFrameY = pFrameYRef;

        pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);
        for (x=0; x<iStartX; x++) {
            *pFrameY++ = 0; 
        }

#	ifndef WMV_OPT_SPRITE
        for (x=iStartX; x<iEndX; x++) {
            ix = ox >> power;
            rx = ox & mask;
            pTmp = (unsigned char *)(pTmpRef + ix);

            U00 = *(pTmp                  );
            U10 = *(pTmp + 1              );
            U01 = *(pTmp     + pWMVDec->m_iOldWidth);
            U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

            *pFrameY++ = (unsigned char)((((U00*(factor-rx-ry)+ U01*ry + U10*rx 
                                        + (((U00+U11-U01-U10)*rx*ry) >> power)) >> power)*iFading)>>8);
            ox += lA;
        }
#	else
        if (pWMVDec->iYLengthX > 0) {
            pWMVDec->ry = ry; 
            Affine_StretchFadingYLineC (pWMVDec, pTmpRef, pFrameY);
            pFrameY += pWMVDec->iYLengthX;
        }
#	endif

        for (x=iEndX; x<iEndXX; x++) {
            long ix = ox >> power;
            pTmp = (unsigned char *)(pTmpRef + ix);
            *pFrameY++ = (unsigned char)(((long)(*pTmp)*iFading)>>8);
            ox += lA;
        }

        for (x=iEndXX; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }

        ox = oxRef;

        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        iy = oy >> power_1;
        offset = iOldWidth_2*iy;
        ry = (oy>>1) & mask;

        for (x=0; x<iStartX_2; x ++) {
            *pFrameU++ = (unsigned char)128; 
            *pFrameV++ = (unsigned char)128;
        }

#       ifndef WMV_OPT_SPRITE

        for (x=iStartX_2; x<iEndX_2; x ++) {
            ix = ox >> power_1;
            rx = (ox>>1) & mask;
            rxry = rx*ry;
            lrxry = factor-rx-ry;

            pTmp = (unsigned char *)(pTmpU + offset + ix);

            U00 = *(pTmp);
            U10 = *(pTmp + 1              );
            U01 = *(pTmp     + iOldWidth_2);
            U11 = *(pTmp + 1 + iOldWidth_2);

            *pFrameU++ = (unsigned char)
                ((((((U00*(lrxry) + U01*ry + U10*rx + (((U00+U11-U01-U10)*rxry) >> power)) >> power)-128)*iFading)>>8)+128);

            pTmp = (unsigned char *)(pTmpV + offset+ ix);

            U00 = *(pTmp);
            U10 = *(pTmp + 1              );
            U01 = *(pTmp     + iOldWidth_2);
            U11 = *(pTmp + 1 + iOldWidth_2);

            *pFrameV++ = (unsigned char)
                ((((((U00*(lrxry) + U01*ry + U10*rx + (((U00+U11-U01-U10)*rxry) >> power)) >> power)-128)*iFading)>>8)+128);
            ox += lAx2;
        }
#else
        if (pWMVDec->iUVLengthX > 0) {
            pWMVDec->ry = ry; 
            Affine_StretchFadingUVLineC (pWMVDec, pTmpU + offset, pTmpV + offset, pFrameU, pFrameV);
            pFrameU += pWMVDec->iUVLengthX;  
            pFrameV += pWMVDec->iUVLengthX;
        }
#endif

        for (x=iEndX_2; x<iEndXX_2; x ++) {
            long ix = ox >> power_1;

            pTmp = (unsigned char *)(pTmpU + offset + ix);
            *pFrameU++ = (unsigned char)(((((long)(*(pTmp))-128)*iFading)>>8)+128);

            pTmp = (unsigned char *)(pTmpV + offset + ix);
            *pFrameV++ = (unsigned char)(((((long)(*(pTmp))-128)*iFading)>>8)+128);

            ox += lAx2;
        }
        for (x=iEndXX_2; x<iNewWidth_2; x ++) {
            *pFrameU++ = (unsigned char)128; 
            *pFrameV++ = (unsigned char)128;
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;

        y++;
        oy += lE;
        ox = oxRef;
        iy = oy >> power;
        ry = oy & mask;
 
        pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);

        for (x=0; x<iStartX; x++) {
            *pFrameY++ = 0; 
        }
#   ifndef WMV_OPT_SPRITE
        for (x=iStartX; x<iEndX; x++) {
            long ix = ox >> power;
            long rx = ox & mask;
            pTmp = (unsigned char *)(pTmpRef + ix);

            U00 = *(pTmp                  );
            U10 = *(pTmp + 1              );
            U01 = *(pTmp     + pWMVDec->m_iOldWidth);
            U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

            *pFrameY++ = (unsigned char)((((U00*(factor-rx-ry)+ U01*ry + U10*rx 
                                        + (((U00+U11-U01-U10)*rx*ry) >> power)) >> power)*iFading)>>8);

            ox += lA;
        }
#   else
            if (pWMVDec->iYLengthX > 0) {
                pWMVDec->ry = ry; 
                Affine_StretchFadingYLineC (pWMVDec, pTmpRef, pFrameY);
                pFrameY += pWMVDec->iYLengthX;
            }
#	endif

        for (x=iEndX; x<iEndXX; x++) {
            long ix = ox >> power;
            pTmp = (unsigned char *)(pTmpRef + ix);
            *pFrameY++ = (unsigned char)(((long)(*(pTmp))*iFading)>>8);
            ox += lA;
        }
        for (x=iEndXX; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        oy += lE;
    }

    ox = oxRef;
    iy = oy >> power;
    if (iy >= pWMVDec->m_iOldHeight )
        iy = pWMVDec->m_iOldHeight - 1;

    ry = oy & mask;
    if (iEndY >= ((iStartY + 2))) {
        for (y=iEndY-2; y<iEndY; y++) {
            pFrameY = pFrameYRef;
            for (x=0; x<iStartX; x++) {
                *pFrameY++ = 0; 
            }
            pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);
            for (x=iStartX; x<iEndX; x++) {
                long ix = ox >> power;
                //long rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);
                *pFrameY++ = (unsigned char)(( (long)(*pTmp)*iFading)>>8) ; 
                ox += lA;
            }
            for ( x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
                *pFrameY++ = 0; 
            }
            pFrameU = pFrameURef;
            pFrameV = pFrameVRef;
            for (x=0; x<iStartX_2; x ++) {
                *pFrameU++ = 128;
                *pFrameV++ = 128;
            }
            ox = oxRef;
            iy = oy >> power_1;
            if (iy >= (pWMVDec->m_iOldHeight >> 1) )
                iy = (pWMVDec->m_iOldHeight >> 1) - 1;

            offset = iOldWidth_2*iy;
            ry = (oy>>1) & mask;
            for (x=iStartX_2; x<iEndX_2; x ++) {
                long ix = ox >> power_1;
                //long rx = (ox>>1) & mask;
                //long rxry = rx*ry;
                //long lrxry = factor-rx-ry;
                pTmp = (unsigned char *)(pTmpU + offset + ix);
                *pFrameU++ = (unsigned char) (((( (long)(*pTmp)-128)*iFading)>>8)+128);
                pTmp = (unsigned char *)(pTmpV + offset + ix);
                *pFrameV++ = (unsigned char)(((( (long)(*pTmp)-128)*iFading)>>8)+128);
                ox += lAx2;
            }
            for (x=iEndX_2; x<(iNewWidth_2); x ++) {
                *pFrameU++ = 128;
                *pFrameV++ = 128;
            }
            pFrameYRef += pWMVDec->m_iNewWidth;
            pFrameURef += iNewWidth_2;
            pFrameVRef += iNewWidth_2;

            pFrameY = pFrameYRef;
            y++;
            oy += lE;
            ox = oxRef;
            iy = oy >> power;
            if (iy >= pWMVDec->m_iOldHeight)
                iy = pWMVDec->m_iOldHeight- 1;

            ry = oy & mask;
            for ( x=0; x<iStartX; x++) {
                *pFrameY++ = 0; 
            }
            pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);
            for (x=iStartX; x<iEndX; x++) {
                long ix = ox >> power;
                //long rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);
                *pFrameY++ = (unsigned char) (( (long) (*pTmp)*iFading)>>8) ; 
                ox += lA;
            }
            for (x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
                *pFrameY++ = 0; 
            }
            pFrameYRef += pWMVDec->m_iNewWidth;
        }
    }
    for (y=iEndY; y<pWMVDec->m_iNewHeight; y++) {
        pFrameY = pFrameYRef;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        for (x=0; x<iNewWidth_2; x ++) {
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;
        y++;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
    }
    return tWMVStatus;
}


tWMVDecodeStatus Affine_PanStretchCFading ( HWMVDecoder* phWMVDecoder, double fZoomX, double fX, double fZoomY, double fY, double fFading)
{
    tWMVDecInternalMember *pWMVDec; 
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

    unsigned char *pTmpRef;
    unsigned char *pTmp;
    unsigned char *pTmpY;
    unsigned char *pTmpU;
    unsigned char *pTmpV;

    int x, y, offset, iOldWidth_2, iNewWidth_2;
    long iStartX, iEndX, iStartY, iEndY, iEndXX;
    long iStartX_2;
    long iEndX_2, iEndXX_2;
    long factor;
    long power;
    long power_1;
    long mask;
    long lA; 
    long lC;
    long lE;
    long lF;
    long lAx2; 
    long iOldWidthxFactor ;
    long iOldHeightxFactor;
    unsigned char * pFrameYRef;
    unsigned char * pFrameURef;
    unsigned char * pFrameVRef;
    unsigned char * pFrameY;
    unsigned char * pFrameU;
    unsigned char * pFrameV;
    long oy ;
    long oxRef;
    long ox ;
    long ry ;
    long iy ;

#   ifndef WMV_OPT_SPRITE
        long U00 ;
        long U10 ;
        long U01 ;
        long U11 ;
        long ix ;
        long rx ;
        long lrxry;
        long rxry ;
#   endif //WMV_OPT_SPRITE

    unsigned long    iFading;
    double fStartX, fEndX, fStartY, fEndY;

#   ifndef WMV_OPT_SPRITE
        double fEndXX  ;
#   endif

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_PanStretchCFading);

    pTmpY = (unsigned char *)pWMVDec->m_dibBitsY;
    pTmpU = (unsigned char *)pWMVDec->m_dibBitsU;
    pTmpV = (unsigned char *)pWMVDec->m_dibBitsV;
    iOldWidth_2 = pWMVDec->m_iOldWidth>>1;
    iNewWidth_2 = pWMVDec->m_iNewWidth>>1;

    if (fZoomX  && fZoomY) {
        
        fStartX   = -fX/fZoomX;
        fEndX     = (((double)(pWMVDec->m_iOldWidth )-fX)/fZoomX);
        if (fStartX > fEndX) {
            double fTmp = fStartX;
            fStartX = fEndX;
            fEndX = fTmp;
        }
#       ifdef WMV_OPT_SPRITE
            iStartX   = (long)ceil(fStartX);
            if ( iStartX < 0 )
                iStartX = 0;
            if ( pWMVDec->m_iNewWidth < iStartX )
                iStartX = pWMVDec->m_iNewWidth;
            iEndXX  = (long)floor(fEndX + 0.999);
            if ( pWMVDec->m_iNewWidth < iEndXX )
                iEndXX = pWMVDec->m_iNewWidth;
            if ( iEndXX < 0 )
                iEndXX = 0;
            iEndX      = (long)floor(fEndX);
            if ( pWMVDec->m_iNewWidth < iEndX )
                iEndX = pWMVDec->m_iNewWidth;
            if ( iEndX < 0 )
                iEndX = 0;
#       else
            fStartX   = (ceil(max( 0.0, fStartX)));
            iStartX   = (long)(floor(min( (double)(pWMVDec->m_iNewWidth), fStartX)));
            fEndXX    = floor(min( (double)(pWMVDec->m_iNewWidth ), fEndX + 0.999));
            fEndX     = (floor(min( (double)(pWMVDec->m_iNewWidth ), fEndX)));
            iEndX     = (long)(ceil(max( 0.0, fEndX)));
            iEndXX    = (long)(ceil(max( 0.0, fEndXX)));
#       endif

        fStartY   = -fY/fZoomY;
        fEndY     = (((double)(pWMVDec->m_iOldHeight )-fY)/fZoomY) + 1.0;

        if (fStartY > fEndY) {
            double fTmp = fStartY;
            fStartY = fEndY;
            fEndY = fTmp;
        }

#       ifdef WMV_OPT_SPRITE
            iStartY   = (long)ceil(fStartY);
            if ( iStartY < 0 )
                iStartY = 0;
            if ( pWMVDec->m_iNewHeight < iStartY )
                iStartY = pWMVDec->m_iNewHeight;
            iEndY     = (long)floor(fEndY);
            if ( pWMVDec->m_iNewHeight < iEndY )
                iEndY = pWMVDec->m_iNewHeight;
            if ( iEndY < 0 )
                iEndY = 0;
#       else
            fStartY   = (ceil(max( 0.0, fStartY)));
            iStartY   = (long)(floor(min( (double)(pWMVDec->m_iNewHeight), fStartY)));
            fEndY     = (floor(min( (double)(pWMVDec->m_iNewHeight), fEndY)));
            iEndY     = (long)(ceil(max( 0.0, fEndY)));
#       endif
    }
    else {
#       ifndef WMV_OPT_SPRITE
            fStartX = fEndX = 0 ;
            fStartY = fEndY = 0 ;
#       endif
        iStartX = iEndX = 0 ;
        iStartY = iEndY = 0 ;
		return tWMVStatus;
    }

    iStartX   = (((iStartX + 1)>>1)<<1);
    iEndX     = (((iEndX      )>>1)<<1);
    iEndXX    = (((iEndXX     )>>1)<<1);
    iStartY   = (((iStartY + 1)>>1)<<1);
    iEndY     = (((iEndY      )>>1)<<1);
    if (iEndY < 2) iEndY = 2;

    iStartX_2 = iStartX>>1;
    iEndX_2   = iEndX  >>1;
    iEndXX_2  = iEndXX >>1;

    factor = 2048;
    power  = 11;
    power_1= 12;
    mask   = 0x7FF;

    lA = (long)(fZoomX * factor); 
    lC = (long)(fX * factor);
    lE = (long)(fZoomY * factor);
    lF = (long)(fY * factor);
    lAx2 = (long)(2*fZoomX * factor); 

    iOldWidthxFactor  = pWMVDec->m_iOldWidth*factor;
    iOldHeightxFactor = pWMVDec->m_iOldHeight*factor;

    //    unsigned char * pFrameYRef = m_pFrameY + m_iNewWidth * iStartY  + iStartX;
    //    unsigned char * pFrameURef = m_pFrameU + (m_iNewWidth>>1) * (iStartY >> 1) + (iStartX>>1);
    //    unsigned char * pFrameVRef = m_pFrameV + (m_iNewWidth>>1) * (iStartY >> 1) + (iStartX>>1);
    pFrameYRef = pWMVDec->m_pFrameY;
    pFrameURef = pWMVDec->m_pFrameU;
    pFrameVRef = pWMVDec->m_pFrameV;

    oy = lE*iStartY +lF;
    oxRef = lC + lA * iStartX;

    if (oy < 0) 
        oy = 0;

    if (oxRef < 0) 
        oxRef = 0;

    if (fFading > 1.0)
        fFading = 1.0;

    if (fFading < 0.0)
        fFading = 0.0;

    iFading = (long)(fFading * 256.0);

#   ifdef WMV_OPT_SPRITE
        pWMVDec->power = power;
        pWMVDec->power_1 = power_1;
        pWMVDec->factor = factor;
        pWMVDec->iYLengthX = iEndX - iStartX;
        pWMVDec->iUVLengthX = iEndX_2 - iStartX_2;
        pWMVDec->oxRef = oxRef;
        pWMVDec->mask = mask;
        pWMVDec->lA = lA;
        pWMVDec->lAx2 = lAx2;
        pWMVDec->iFading = iFading;
#   endif

    for (y=0; y<iStartY; y++) {
        pFrameY = pFrameYRef;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        for (x=0; x<iNewWidth_2; x ++) {
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;
        y++;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
    }

    for (y=iStartY; y<iEndY-2; y++) {
        ox = oxRef;
        iy = oy >> power;
        ry = oy & mask;

        pFrameY = pFrameYRef;

        pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);
        for (x=0; x<iStartX; x++) {
            *pFrameY++ = 0; 
        }

#       ifndef WMV_OPT_SPRITE
            for (x=iStartX; x<iEndX; x++) {
                ix = ox >> power;
                rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);

                U00 = *(pTmp                  );
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + pWMVDec->m_iOldWidth);
                U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

                *pFrameY++ = (unsigned char)((((U00*(factor-rx-ry)+ U01*ry + U10*rx 
                                            + (((U00+U11-U01-U10)*rx*ry) >> power)) >> power)*iFading)>>8);
                ox += lA;
            }
#       else
            if (pWMVDec->iYLengthX > 0) {
                pWMVDec->ry = ry; 
                Affine_StretchFadingYLineC (pWMVDec, pTmpRef, pFrameY);
                pFrameY += pWMVDec->iYLengthX;
            }
#       endif //WMV_OPT_SPRITE

        for (x=iEndX; x<iEndXX; x++) {
            long ix = ox >> power;
            pTmp = (unsigned char *)(pTmpRef + ix);
            *pFrameY++ = (unsigned char)(((long)(*(pTmp))*iFading)>>8);
            ox += lA;
        }

        for (x=iEndXX; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }

        ox = oxRef;

        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        iy = oy >> power_1;
        offset = iOldWidth_2*iy;
        ry = (oy>>1) & mask;

        for (x=0; x<iStartX_2; x ++) {
            *pFrameU++ = (unsigned char)128; 
            *pFrameV++ = (unsigned char)128;
        }

#       ifndef WMV_OPT_SPRITE
            for (x=iStartX_2; x<iEndX_2; x ++) {
                ix = ox >> power_1;
                rx = (ox>>1) & mask;
                rxry = rx*ry;
                lrxry = factor-rx-ry;

                pTmp = (unsigned char *)(pTmpU + offset + ix);

                U00 = *(pTmp);
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + iOldWidth_2);
                U11 = *(pTmp + 1 + iOldWidth_2);

                *pFrameU++ = (unsigned char)
                    ((((((U00*(lrxry) + U01*ry + U10*rx + (((U00+U11-U01-U10)*rxry) >> power)) >> power)-128)*iFading)>>8)+128);

                pTmp = (unsigned char *)(pTmpV + offset+ ix);

                U00 = *(pTmp);
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + iOldWidth_2);
                U11 = *(pTmp + 1 + iOldWidth_2);

                *pFrameV++ = (unsigned char)
                    ((((((U00*(lrxry) + U01*ry + U10*rx + (((U00+U11-U01-U10)*rxry) >> power)) >> power)-128)*iFading)>>8)+128);
                ox += lAx2;
            }
#       else
            if (pWMVDec->iUVLengthX > 0) {
                pWMVDec->ry = ry; 
                Affine_StretchFadingUVLineC (pWMVDec, pTmpU + offset, pTmpV + offset, pFrameU, pFrameV);
                pFrameU += pWMVDec->iUVLengthX;  
                pFrameV += pWMVDec->iUVLengthX;
            }
#       endif

        for (x=iEndX_2; x<iEndXX_2; x ++) {
            long ix = ox >> power_1;

            pTmp = (unsigned char *)(pTmpU + offset + ix);
            *pFrameU++ = (unsigned char)(((((long)(*(pTmp))-128)*iFading)>>8)+128);

            pTmp = (unsigned char *)(pTmpV + offset + ix);
            *pFrameV++ = (unsigned char)(((((long)(*(pTmp))-128)*iFading)>>8)+128);

            ox += lAx2;
        }

        for (x=iEndXX_2; x<iNewWidth_2; x ++) {
            *pFrameU++ = (unsigned char)128; 
            *pFrameV++ = (unsigned char)128;
        }

        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;

        y++;
        oy += lE;
        ox = oxRef;
        iy = oy >> power;
        ry = oy & mask;
 
        pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);

        for (x=0; x<iStartX; x++) {
            *pFrameY++ = 0; 
        }

#   ifndef WMV_OPT_SPRITE
            for (x=iStartX; x<iEndX; x++) {
                long ix = ox >> power;
                long rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);

                U00 = *(pTmp                  );
                U10 = *(pTmp + 1              );
                U01 = *(pTmp     + pWMVDec->m_iOldWidth);
                U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

                *pFrameY++ = (unsigned char)((((U00*(factor-rx-ry)+ U01*ry + U10*rx 
                                            + (((U00+U11-U01-U10)*rx*ry) >> power)) >> power)*iFading)>>8);

                ox += lA;
            }
#       else
            if (pWMVDec->iYLengthX > 0) {
                pWMVDec->ry = ry; 
                Affine_StretchFadingYLineC (pWMVDec, pTmpRef, pFrameY);
                pFrameY += pWMVDec->iYLengthX;
            }
#       endif //WMV_OPT_SPRITE

        for (x=iEndX; x<iEndXX; x++) {
            long ix = ox >> power;
            pTmp = (unsigned char *)(pTmpRef + ix);
            *pFrameY++ = (unsigned char)(((long)(*(pTmp))*iFading)>>8);
            ox += lA;
        }
        for (x=iEndXX; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        oy += lE;
    }

    ox = oxRef;
    iy = oy >> power;
    if (iy >= pWMVDec->m_iOldHeight )
        iy = pWMVDec->m_iOldHeight - 1;

    ry = oy & mask;
    if (iEndY >= ((iStartY + 2))) {
        for (y=iEndY-2; y<iEndY; y++) {
            pFrameY = pFrameYRef;
            for (x=0; x<iStartX; x++) {
                *pFrameY++ = 0; 
            }
            pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);
            for ( x=iStartX; x<iEndX; x++) {
                long ix = ox >> power;
                //long rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);
                *pFrameY++ = (unsigned char)(( (long)(*pTmp)*iFading)>>8) ; 
                ox += lA;
            }
            for ( x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
                *pFrameY++ = 0; 
            }
            pFrameU = pFrameURef;
            pFrameV = pFrameVRef;
            for (x=0; x<iStartX_2; x ++) {
                *pFrameU++ = 128;
                *pFrameV++ = 128;
            }
            ox = oxRef;
            iy = oy >> power_1;
            if (iy >= (pWMVDec->m_iOldHeight >> 1) )
                iy = (pWMVDec->m_iOldHeight >> 1) - 1;

            offset = iOldWidth_2*iy;
            ry = (oy>>1) & mask;
            for (x=iStartX_2; x<iEndX_2; x ++) {
                long ix = ox >> power_1;
                //long rx = (ox>>1) & mask;
                //long rxry = rx*ry;
                //long lrxry = factor-rx-ry;
                pTmp = (unsigned char *)(pTmpU + offset + ix);
                *pFrameU++ = (unsigned char) (((( (long)(*pTmp)-128)*iFading)>>8)+128);
                pTmp = (unsigned char *)(pTmpV + offset + ix);
                *pFrameV++ = (unsigned char)(((( (long)(*pTmp)-128)*iFading)>>8)+128);
                ox += lAx2;
            }
            for (x=iEndX_2; x<(iNewWidth_2); x ++) {
                *pFrameU++ = 128;
                *pFrameV++ = 128;
            }
            pFrameYRef += pWMVDec->m_iNewWidth;
            pFrameURef += iNewWidth_2;
            pFrameVRef += iNewWidth_2;

            pFrameY = pFrameYRef;
            y++;
            oy += lE;
            ox = oxRef;
            iy = oy >> power;
            if (iy >= pWMVDec->m_iOldHeight)
                iy = pWMVDec->m_iOldHeight - 1;

            ry = oy & mask;
            for ( x=0; x<iStartX; x++) {
                *pFrameY++ = 0; 
            }
            pTmpRef = (unsigned char *)(pWMVDec->m_dibBitsY + pWMVDec->m_iOldWidth*iy);
            for (x=iStartX; x<iEndX; x++) {
                long ix = ox >> power;
                //long rx = ox & mask;
                pTmp = (unsigned char *)(pTmpRef + ix);
                *pFrameY++ = (unsigned char)(( (long)(*pTmp)*iFading)>>8) ; 
                ox += lA;
            }
            for (x=iEndX; x<pWMVDec->m_iNewWidth; x++) {
                *pFrameY++ = 0; 
            }
            pFrameYRef += pWMVDec->m_iNewWidth;
        }
    }
    for (y=iEndY; y<pWMVDec->m_iNewHeight; y++) {
        pFrameY = pFrameYRef;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameU = pFrameURef;
        pFrameV = pFrameVRef;
        for (x=0; x<iNewWidth_2; x ++) {
            *pFrameU++ = 128;
            *pFrameV++ = 128;
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
        pFrameURef += iNewWidth_2;
        pFrameVRef += iNewWidth_2;

        pFrameY = pFrameYRef;
        y++;
        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            *pFrameY++ = 0; 
        }
        pFrameYRef += pWMVDec->m_iNewWidth;
    }
    return tWMVStatus;
}

tWMVDecodeStatus Affine_TransformFloat1 (
		HWMVDecoder* phWMVDecoder, 
        double fA, double fB, double fC,
        double fD, double fE, double fF)
{
    tWMVDecInternalMember *pWMVDec; 
    unsigned char *pTmpY;
    unsigned char *pTmpU;
    unsigned char *pTmpV;
    int iOldWidth_2;
    unsigned char * pFrameY;
    unsigned char * pFrameU;
    unsigned char * pFrameV;
    double fTmp;
	int y;

    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_TransformFloat1);

    pTmpY = (unsigned char *)pWMVDec->m_dibBitsY;
    pTmpU = (unsigned char *)pWMVDec->m_dibBitsU;
    pTmpV = (unsigned char *)pWMVDec->m_dibBitsV;
    iOldWidth_2 = pWMVDec->m_iOldWidth/2;
    pFrameY = pWMVDec->m_pFrameY;
    pFrameU = pWMVDec->m_pFrameU;
    pFrameV = pWMVDec->m_pFrameV;

    for (y=0; y<pWMVDec->m_iNewHeight; y++) {
        double ox = fB*y +fC - fA;
        double oy = fE*y +fF - fD;
        int doyuv = 1;
		int x;

        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            ox += fA;
            oy += fD;
            if ((ox >= 1.0) && (ox < (pWMVDec->m_iOldWidth-1)) && (oy >= 1.0) && (oy < (pWMVDec->m_iOldHeight-1))) {
                int ix = (int)ox;
                int iy = (int)oy;
                double rx = ox - (double)ix;
                double ry = oy - (double)iy;
                unsigned char *pTmp = (unsigned char *)(pWMVDec->m_dibBitsY + ix  + pWMVDec->m_iOldWidth*iy);

                double U00 = *(pTmp                  );
                double U10 = *(pTmp + 1              );
                double U01 = *(pTmp     + pWMVDec->m_iOldWidth);

                fTmp = (U00*(1-rx-ry) + U01*ry + U10*rx);
                if (fTmp > 255.0) 
                    fTmp = 255.0;
                *pFrameY++ = (unsigned char)(fTmp);

                if (doyuv) {
					int offset;
					double rxry;
					double lrxry;

                    ix = (int)ox/2;
                    iy = (int)oy/2;
                    rx = ox/2 - (double)ix;
                    ry = oy/2 - (double)iy;
                    offset = ix + iOldWidth_2*iy;
                    rxry = rx*ry;
                    lrxry = 1-rx-ry;

                    pTmp = (unsigned char *)(pTmpU + offset);

                    U00 = *(pTmp);
                    U10 = *(pTmp + 1              );
                    U01 = *(pTmp     + iOldWidth_2);

                    fTmp = (U00*(lrxry) + U01*ry+ U10*rx);
                    if (fTmp > 255.0) 
                        fTmp = 255.0;
                    *pFrameU ++ = (unsigned char)(fTmp);

                    pTmp = (unsigned char *)(pTmpV + offset);

                    U00 = *(pTmp);
                    U10 = *(pTmp + 1              );
                    U01 = *(pTmp     + iOldWidth_2);

                    fTmp = (U00*(lrxry) + U01*ry+ U10*rx);
                    if (fTmp > 255.0) 
                        fTmp = 255.0;
                    *pFrameV ++ = (unsigned char)(fTmp);
                    
                    doyuv = 0;
                }
                else 
                    doyuv = 1;
            }
        }

        y++;
        ox = fB*y +fC - fA;
        oy = fE*y +fF - fD;

        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            ox += fA;
            oy += fD;
            if ((ox >= 0) && (ox < pWMVDec->m_iOldWidth) && (oy >= 0) && (oy < pWMVDec->m_iOldHeight)) {
                int ix = (int)ox;
                int iy = (int)oy;
                double rx = ox - (double)ix;
                double ry = oy - (double)iy;
                unsigned char *pTmp = (unsigned char *)(pWMVDec->m_dibBitsY + ix  + pWMVDec->m_iOldWidth*iy);
                double U00 = *(pTmp);
                double U01 = *(pTmp     + pWMVDec->m_iOldWidth);
                double U10 = *(pTmp + 1              );

                fTmp = (U00*(1-rx-ry)+ U01*ry + U10*rx);
                if (fTmp > 255.0) 
                    fTmp = 255.0;
                *pFrameY++ = (unsigned char)(fTmp);

            }
        }
    }
    return tWMVStatus;
}


tWMVDecodeStatus Affine_TransformFloat2 ( HWMVDecoder* phWMVDecoder, double fA, double fB, double fC, double fD, double fE, double fF)
{
    tWMVDecInternalMember *pWMVDec; 
    unsigned char *pTmpY;
    unsigned char *pTmpU;
    unsigned char *pTmpV;
    int iOldWidth_2;
    unsigned char * pFrameY;
    unsigned char * pFrameU;
    unsigned char * pFrameV;
    long x, y, offset;
    double fTmp;
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_TransformFloat2);

    pTmpY = (unsigned char *)pWMVDec->m_dibBitsY;
    pTmpU = (unsigned char *)pWMVDec->m_dibBitsU;
    pTmpV = (unsigned char *)pWMVDec->m_dibBitsV;
    iOldWidth_2 = pWMVDec->m_iOldWidth/2;
    pFrameY = pWMVDec->m_pFrameY;
    pFrameU = pWMVDec->m_pFrameU;
    pFrameV = pWMVDec->m_pFrameV;

    for (y=0; y<pWMVDec->m_iNewHeight; y++) {
        double ox = fB*y +fC - fA;
        double oy = fE*y +fF - fD;
        int doyuv = 1;

        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            ox += fA;
            oy += fD;
            if ((ox >= 1.0) && (ox < (pWMVDec->m_iOldWidth-1)) && (oy >= 1.0) && (oy < (pWMVDec->m_iOldHeight-1))) {
                int ix = (int)ox;
                int iy = (int)oy;
                double rx = ox - (double)ix;
                double ry = oy - (double)iy;
                double X2 = rx*rx;
                double X3 = rx*rx*rx;
                double Y2 = ry*ry;
                double Y3 = ry*ry*ry;

                unsigned char *pTmp = (unsigned char *)(pWMVDec->m_dibBitsY + ix  + pWMVDec->m_iOldWidth*iy);

                double U_1_1 = *(pTmp - 1 -   pWMVDec->m_iOldWidth);
                double U0_1  = *(pTmp     -   pWMVDec->m_iOldWidth);
                double U1_1  = *(pTmp + 1 -   pWMVDec->m_iOldWidth);
                double U2_1  = *(pTmp + 2 -   pWMVDec->m_iOldWidth);
                double U_10  = *(pTmp - 1                );
                double U00   = *(pTmp                    );
                double U10   = *(pTmp + 1                );
                double U20   = *(pTmp + 2                );
                double U_11  = *(pTmp - 1 +   pWMVDec->m_iOldWidth);
                double U01   = *(pTmp     +   pWMVDec->m_iOldWidth);
                double U11   = *(pTmp + 1 +   pWMVDec->m_iOldWidth);
                double U21   = *(pTmp + 2 +   pWMVDec->m_iOldWidth);
                double U_12  = *(pTmp - 1 + 2*pWMVDec->m_iOldWidth);
                double U02   = *(pTmp     + 2*pWMVDec->m_iOldWidth);
                double U12   = *(pTmp + 1 + 2*pWMVDec->m_iOldWidth);
                double U22   = *(pTmp + 2 + 2*pWMVDec->m_iOldWidth);

                double A =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 - 2*U_10 +  3*U00 -  3*U10 + 2*U20 + 2*U_11 -  3*U01 + 3*U11 - 2*U21 -   U_12 + 2*U02 - 2*U12 +   U22;
                double B =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 8*U_10 - 11*U00 +  7*U10 - 4*U20 - 8*U_11 + 11*U01 - 7*U11 + 4*U21 + 4*U_12 - 7*U02 + 5*U12 - 2*U22)/2.0;
                double C =(-4*U_1_1 + 8*U0_1 - 8*U1_1 + 4*U2_1 + 7*U_10 - 11*U00 + 11*U10 - 7*U20 - 5*U_11 +  7*U01 - 7*U11 + 5*U21 + 2*U_12 - 4*U02 + 4*U12 - 2*U22)/2.0;
                double D =    U_1_1 -   U0_1                   - 2*U_10 +    U00 +    U10         + 2*U_11 -    U01 -   U11         -   U_12 +   U02                ;
                double E =( 8*U_1_1 -14*U0_1 +10*U1_1 - 4*U2_1 -14*U_10 + 20*U00 - 13*U10 + 7*U20 +10*U_11 - 13*U01 + 8*U11 - 5*U21 - 4*U_12 + 7*U02 - 5*U12 + 2*U22)/2.0;
                double F =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 -   U_10 +    U00 -    U10 +   U20          +    U01 -   U11                                         ;
                double G =(         -   U0_1                            +  3*U00                           -  3*U01                          +   U02                )/2.0;
                double H =(-4*U_1_1 + 4*U0_1                   + 7*U_10 -  4*U00 -  3*U10         - 5*U_11 +  2*U01 + 3*U11         + 2*U_12 - 2*U02                )/2.0;
                double I =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 4*U_10 -  4*U00 +  2*U10 - 2*U20          -  3*U01 + 3*U11                                         )/2.0;
                double J =(                                    -   U_10 +  3*U00 -  3*U10 +   U20                                                                   )/2.0;
                double K =(         + 2*U0_1                            -  5*U00                           +  4*U01                          -   U02                )/2.0;
                double L =    U_1_1 -   U0_1                   -   U_10 +    U00                                                                                     ;
                double M =(                                    + 2*U_10 -  5*U00 +  4*U10 -   U20                                                                   )/2.0;
                double N =(         -   U0_1                                                               +    U01                                                 )/2.0;
                double O =(                                    -   U_10          +    U10                                                                           )/2.0;
                double P =                                              +    U00                                                                                    ;


                fTmp = A*Y3*X3 + B*Y3*X2 + C*Y2*X3 + D*Y3*rx + E*Y2*X2 + F*ry*X3 +
                       G*Y3 + H*Y2*rx + I*ry*X2 + J*X3 + K*Y2 + L*ry*rx + M*X2 + N*ry + O*rx + P;

                if (fTmp > 255.0) 
                    fTmp = 255.0;
                if (fTmp < 0.0) 
                    fTmp = 0.0;

                *pFrameY++ = (unsigned char)(fTmp) ;

                if (doyuv) {
                    ix = (int)ox/2;
                    iy = (int)oy/2;
                    rx = ox/2 - (double)ix;
                    ry = oy/2 - (double)iy;
                    X2 = rx*rx;
                    X3 = rx*rx*rx;
                    Y2 = ry*ry;
                    Y3 = ry*ry*ry;

                    offset = ix + iOldWidth_2*iy;

                    pTmp = (unsigned char *)(pWMVDec->m_dibBitsU + offset);

                    U_1_1 = *(pTmp - 1 -   iOldWidth_2);
                    U0_1  = *(pTmp     -   iOldWidth_2);
                    U1_1  = *(pTmp + 1 -   iOldWidth_2);
                    U2_1  = *(pTmp + 2 -   iOldWidth_2);
                    U_10  = *(pTmp - 1                );
                    U00   = *(pTmp                    );
                    U10   = *(pTmp + 1                );
                    U20   = *(pTmp + 2                );
                    U_11  = *(pTmp - 1 +   iOldWidth_2);
                    U01   = *(pTmp     +   iOldWidth_2);
                    U11   = *(pTmp + 1 +   iOldWidth_2);
                    U21   = *(pTmp + 2 +   iOldWidth_2);
                    U_12  = *(pTmp - 1 + 2*iOldWidth_2);
                    U02   = *(pTmp     + 2*iOldWidth_2);
                    U12   = *(pTmp + 1 + 2*iOldWidth_2);
                    U22   = *(pTmp + 2 + 2*iOldWidth_2);

                    A =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 - 2*U_10 +  3*U00 -  3*U10 + 2*U20 + 2*U_11 -  3*U01 + 3*U11 - 2*U21 -   U_12 + 2*U02 - 2*U12 +   U22;
                    B =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 8*U_10 - 11*U00 +  7*U10 - 4*U20 - 8*U_11 + 11*U01 - 7*U11 + 4*U21 + 4*U_12 - 7*U02 + 5*U12 - 2*U22)/2.0;
                    C =(-4*U_1_1 + 8*U0_1 - 8*U1_1 + 4*U2_1 + 7*U_10 - 11*U00 + 11*U10 - 7*U20 - 5*U_11 +  7*U01 - 7*U11 + 5*U21 + 2*U_12 - 4*U02 + 4*U12 - 2*U22)/2.0;
                    D =    U_1_1 -   U0_1                   - 2*U_10 +    U00 +    U10         + 2*U_11 -    U01 -   U11         -   U_12 +   U02                ;
                    E =( 8*U_1_1 -14*U0_1 +10*U1_1 - 4*U2_1 -14*U_10 + 20*U00 - 13*U10 + 7*U20 +10*U_11 - 13*U01 + 8*U11 - 5*U21 - 4*U_12 + 7*U02 - 5*U12 + 2*U22)/2.0;
                    F =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 -   U_10 +    U00 -    U10 +   U20          +    U01 -   U11                                         ;
                    G =(         -   U0_1                            +  3*U00                           -  3*U01                          +   U02                )/2.0;
                    H =(-4*U_1_1 + 4*U0_1                   + 7*U_10 -  4*U00 -  3*U10         - 5*U_11 +  2*U01 + 3*U11         + 2*U_12 - 2*U02                )/2.0;
                    I =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 4*U_10 -  4*U00 +  2*U10 - 2*U20          -  3*U01 + 3*U11                                         )/2.0;
                    J =(                                    -   U_10 +  3*U00 -  3*U10 +   U20                                                                   )/2.0;
                    K =(         + 2*U0_1                            -  5*U00                           +  4*U01                          -   U02                )/2.0;
                    L =    U_1_1 -   U0_1                   -   U_10 +    U00                                                                                     ;
                    M =(                                    + 2*U_10 -  5*U00 +  4*U10 -   U20                                                                   )/2.0;
                    N =(         -   U0_1                                                               +    U01                                                 )/2.0;
                    O =(                                    -   U_10          +    U10                                                                           )/2.0;
                    P =                                              +    U00                                                                                    ;

                    fTmp = A*Y3*X3 + B*Y3*X2 + C*Y2*X3 + D*Y3*rx + E*Y2*X2 + F*ry*X3 +
                       G*Y3 + H*Y2*rx + I*ry*X2 + J*X3 + K*Y2 + L*ry*rx + M*X2 + N*ry + O*rx + P;

                    if (fTmp > 255.0) 
                        fTmp = 255.0;
                    if (fTmp < 0.0) 
                        fTmp = 0.0;

                    *pFrameU ++ = (unsigned char)(fTmp);

                    pTmp = (unsigned char *)(pWMVDec->m_dibBitsV + offset);
 
                    U_1_1 = *(pTmp - 1 -   iOldWidth_2);
                    U0_1  = *(pTmp     -   iOldWidth_2);
                    U1_1  = *(pTmp + 1 -   iOldWidth_2);
                    U2_1  = *(pTmp + 2 -   iOldWidth_2);
                    U_10  = *(pTmp - 1                );
                    U00   = *(pTmp                    );
                    U10   = *(pTmp + 1                );
                    U20   = *(pTmp + 2                );
                    U_11  = *(pTmp - 1 +   iOldWidth_2);
                    U01   = *(pTmp     +   iOldWidth_2);
                    U11   = *(pTmp + 1 +   iOldWidth_2);
                    U21   = *(pTmp + 2 +   iOldWidth_2);
                    U_12  = *(pTmp - 1 + 2*iOldWidth_2);
                    U02   = *(pTmp     + 2*iOldWidth_2);
                    U12   = *(pTmp + 1 + 2*iOldWidth_2);
                    U22   = *(pTmp + 2 + 2*iOldWidth_2);

                    A =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 - 2*U_10 +  3*U00 -  3*U10 + 2*U20 + 2*U_11 -  3*U01 + 3*U11 - 2*U21 -   U_12 + 2*U02 - 2*U12 +   U22;
                    B =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 8*U_10 - 11*U00 +  7*U10 - 4*U20 - 8*U_11 + 11*U01 - 7*U11 + 4*U21 + 4*U_12 - 7*U02 + 5*U12 - 2*U22)/2.0;
                    C =(-4*U_1_1 + 8*U0_1 - 8*U1_1 + 4*U2_1 + 7*U_10 - 11*U00 + 11*U10 - 7*U20 - 5*U_11 +  7*U01 - 7*U11 + 5*U21 + 2*U_12 - 4*U02 + 4*U12 - 2*U22)/2.0;
                    D =    U_1_1 -   U0_1                   - 2*U_10 +    U00 +    U10         + 2*U_11 -    U01 -   U11         -   U_12 +   U02                ;
                    E =( 8*U_1_1 -14*U0_1 +10*U1_1 - 4*U2_1 -14*U_10 + 20*U00 - 13*U10 + 7*U20 +10*U_11 - 13*U01 + 8*U11 - 5*U21 - 4*U_12 + 7*U02 - 5*U12 + 2*U22)/2.0;
                    F =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 -   U_10 +    U00 -    U10 +   U20          +    U01 -   U11                                         ;
                    G =(         -   U0_1                            +  3*U00                           -  3*U01                          +   U02                )/2.0;
                    H =(-4*U_1_1 + 4*U0_1                   + 7*U_10 -  4*U00 -  3*U10         - 5*U_11 +  2*U01 + 3*U11         + 2*U_12 - 2*U02                )/2.0;
                    I =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 4*U_10 -  4*U00 +  2*U10 - 2*U20          -  3*U01 + 3*U11                                         )/2.0;
                    J =(                                    -   U_10 +  3*U00 -  3*U10 +   U20                                                                   )/2.0;
                    K =(         + 2*U0_1                            -  5*U00                           +  4*U01                          -   U02                )/2.0;
                    L =    U_1_1 -   U0_1                   -   U_10 +    U00                                                                                     ;
                    M =(                                    + 2*U_10 -  5*U00 +  4*U10 -   U20                                                                   )/2.0;
                    N =(         -   U0_1                                                               +    U01                                                 )/2.0;
                    O =(                                    -   U_10          +    U10                                                                           )/2.0;
                    P =                                              +    U00                                                                                    ;

                    fTmp = A*Y3*X3 + B*Y3*X2 + C*Y2*X3 + D*Y3*rx + E*Y2*X2 + F*ry*X3 +
                       G*Y3 + H*Y2*rx + I*ry*X2 + J*X3 + K*Y2 + L*ry*rx + M*X2 + N*ry + O*rx + P;

                    if (fTmp > 255.0) 
                        fTmp = 255.0;
                    if (fTmp < 0.0) 
                        fTmp = 0.0;

                    *pFrameV ++ = (unsigned char)(fTmp);
                    
                    doyuv = 0;
                }
                else 
                    doyuv = 1;
            }
            else {
                *pFrameY++ = 0 ;
                if (doyuv) {
                    *pFrameU ++ = 128;
                    *pFrameV ++ = 128;
                    doyuv       = 0;
                }
                else
                    doyuv       = 1;
            }
        }

        y++;
        ox = fB*y +fC - fA;
        oy = fE*y +fF - fD;

        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            ox += fA;
            oy += fD;
            if ((ox >= 1.0) && (ox < (pWMVDec->m_iOldWidth-1)) && (oy >= 1.0) && (oy < (pWMVDec->m_iOldHeight-1))) {
                int ix = (int)ox;
                int iy = (int)oy;
                double rx = ox - (double)ix;
                double ry = oy - (double)iy;
                double X2 = rx*rx;
                double X3 = rx*rx*rx;
                double Y2 = ry*ry;
                double Y3 = ry*ry*ry;
                unsigned char *pTmp = (unsigned char *)(pWMVDec->m_dibBitsY + ix  + pWMVDec->m_iOldWidth*iy);
                double U_1_1 = *(pTmp - 1 -   pWMVDec->m_iOldWidth);
                double U0_1  = *(pTmp     -   pWMVDec->m_iOldWidth);
                double U1_1  = *(pTmp + 1 -   pWMVDec->m_iOldWidth);
                double U2_1  = *(pTmp + 2 -   pWMVDec->m_iOldWidth);
                double U_10  = *(pTmp - 1                );
                double U00   = *(pTmp                    );
                double U10   = *(pTmp + 1                );
                double U20   = *(pTmp + 2                );
                double U_11  = *(pTmp - 1 +   pWMVDec->m_iOldWidth);
                double U01   = *(pTmp     +   pWMVDec->m_iOldWidth);
                double U11   = *(pTmp + 1 +   pWMVDec->m_iOldWidth);
                double U21   = *(pTmp + 2 +   pWMVDec->m_iOldWidth);
                double U_12  = *(pTmp - 1 + 2*pWMVDec->m_iOldWidth);
                double U02   = *(pTmp     + 2*pWMVDec->m_iOldWidth);
                double U12   = *(pTmp + 1 + 2*pWMVDec->m_iOldWidth);
                double U22   = *(pTmp + 2 + 2*pWMVDec->m_iOldWidth);

                double A =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 - 2*U_10 +  3*U00 -  3*U10 + 2*U20 + 2*U_11 -  3*U01 + 3*U11 - 2*U21 -   U_12 + 2*U02 - 2*U12 +   U22;
                double B =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 8*U_10 - 11*U00 +  7*U10 - 4*U20 - 8*U_11 + 11*U01 - 7*U11 + 4*U21 + 4*U_12 - 7*U02 + 5*U12 - 2*U22)/2.0;
                double C =(-4*U_1_1 + 8*U0_1 - 8*U1_1 + 4*U2_1 + 7*U_10 - 11*U00 + 11*U10 - 7*U20 - 5*U_11 +  7*U01 - 7*U11 + 5*U21 + 2*U_12 - 4*U02 + 4*U12 - 2*U22)/2.0;
                double D =    U_1_1 -   U0_1                   - 2*U_10 +    U00 +    U10         + 2*U_11 -    U01 -   U11         -   U_12 +   U02                ;
                double E =( 8*U_1_1 -14*U0_1 +10*U1_1 - 4*U2_1 -14*U_10 + 20*U00 - 13*U10 + 7*U20 +10*U_11 - 13*U01 + 8*U11 - 5*U21 - 4*U_12 + 7*U02 - 5*U12 + 2*U22)/2.0;
                double F =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 -   U_10 +    U00 -    U10 +   U20          +    U01 -   U11                                         ;
                double G =(         -   U0_1                            +  3*U00                           -  3*U01                          +   U02                )/2.0;
                double H =(-4*U_1_1 + 4*U0_1                   + 7*U_10 -  4*U00 -  3*U10         - 5*U_11 +  2*U01 + 3*U11         + 2*U_12 - 2*U02                )/2.0;
                double I =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 4*U_10 -  4*U00 +  2*U10 - 2*U20          -  3*U01 + 3*U11                                         )/2.0;
                double J =(                                    -   U_10 +  3*U00 -  3*U10 +   U20                                                                   )/2.0;
                double K =(         + 2*U0_1                            -  5*U00                           +  4*U01                          -   U02                )/2.0;
                double L =    U_1_1 -   U0_1                   -   U_10 +    U00                                                                                     ;
                double M =(                                    + 2*U_10 -  5*U00 +  4*U10 -   U20                                                                   )/2.0;
                double N =(         -   U0_1                                                               +    U01                                                 )/2.0;
                double O =(                                    -   U_10          +    U10                                                                           )/2.0;
                double P =                                              +    U00                                                                                    ;


                fTmp = A*Y3*X3 + B*Y3*X2 + C*Y2*X3 + D*Y3*rx + E*Y2*X2 + F*ry*X3 +
                       G*Y3 + H*Y2*rx + I*ry*X2 + J*X3 + K*Y2 + L*ry*rx + M*X2 + N*ry + O*rx + P;

                if (fTmp > 255.0) 
                    fTmp = 255.0;
                if (fTmp < 0.0) 
                    fTmp = 0.0;

                *pFrameY++ = (unsigned char)(fTmp) ;

            }
            else
                *pFrameY++ = 0;

        }
    }
    return tWMVStatus;
}

//
//Add Fading
//
tWMVDecodeStatus Affine_TransformFadingFloat2 (HWMVDecoder* phWMVDecoder,
        double fA, double fB, double fC,
        double fD, double fE, double fF, double fFading)
{
    tWMVDecInternalMember *pWMVDec; 

    unsigned char *pTmpY;
    unsigned char *pTmpU; 
    unsigned char *pTmpV; 
    double fTmp;

    int iOldWidth_2;
    unsigned char * pFrameY; 
    unsigned char * pFrameU;
    unsigned char * pFrameV;
	int y;

    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_TransformFloat2);

    iOldWidth_2 = pWMVDec->m_iOldWidth/2;
    pFrameY = pWMVDec->m_pFrameY;
    pFrameU = pWMVDec->m_pFrameU;
    pFrameV = pWMVDec->m_pFrameV;

    pTmpY = (unsigned char *)pWMVDec->m_dibBitsY;
    pTmpU = (unsigned char *)pWMVDec->m_dibBitsU;
    pTmpV = (unsigned char *)pWMVDec->m_dibBitsV;

    for (y=0; y<pWMVDec->m_iNewHeight; y++) {
        double ox = fB*y +fC - fA;
        double oy = fE*y +fF - fD;
        int doyuv = 1;
		int x;

        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            ox += fA;
            oy += fD;
            if ((ox >= 1.0) && (ox < (pWMVDec->m_iOldWidth-1)) && (oy >= 1.0) && (oy < (pWMVDec->m_iOldHeight-1))) {
                int ix = (int)ox;
                int iy = (int)oy;
                double rx = ox - (double)ix;
                double ry = oy - (double)iy;
                double X2 = rx*rx;
                double X3 = rx*rx*rx;
                double Y2 = ry*ry;
                double Y3 = ry*ry*ry;

			    unsigned char *pTmp = (unsigned char *)(pWMVDec->m_dibBitsY + ix  + pWMVDec->m_iOldWidth*iy);

                double U_1_1 = *(pTmp - 1 -   pWMVDec->m_iOldWidth);
                double U0_1  = *(pTmp     -   pWMVDec->m_iOldWidth);
                double U1_1  = *(pTmp + 1 -   pWMVDec->m_iOldWidth);
                double U2_1  = *(pTmp + 2 -   pWMVDec->m_iOldWidth);
                double U_10  = *(pTmp - 1                );
                double U00   = *(pTmp                    );
                double U10   = *(pTmp + 1                );
                double U20   = *(pTmp + 2                );
                double U_11  = *(pTmp - 1 +   pWMVDec->m_iOldWidth);
                double U01   = *(pTmp     +   pWMVDec->m_iOldWidth);
                double U11   = *(pTmp + 1 +   pWMVDec->m_iOldWidth);
                double U21   = *(pTmp + 2 +   pWMVDec->m_iOldWidth);
                double U_12  = *(pTmp - 1 + 2*pWMVDec->m_iOldWidth);
                double U02   = *(pTmp     + 2*pWMVDec->m_iOldWidth);
                double U12   = *(pTmp + 1 + 2*pWMVDec->m_iOldWidth);
                double U22   = *(pTmp + 2 + 2*pWMVDec->m_iOldWidth);

                double A =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 - 2*U_10 +  3*U00 -  3*U10 + 2*U20 + 2*U_11 -  3*U01 + 3*U11 - 2*U21 -   U_12 + 2*U02 - 2*U12 +   U22;
                double B =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 8*U_10 - 11*U00 +  7*U10 - 4*U20 - 8*U_11 + 11*U01 - 7*U11 + 4*U21 + 4*U_12 - 7*U02 + 5*U12 - 2*U22)/2.0;
                double C =(-4*U_1_1 + 8*U0_1 - 8*U1_1 + 4*U2_1 + 7*U_10 - 11*U00 + 11*U10 - 7*U20 - 5*U_11 +  7*U01 - 7*U11 + 5*U21 + 2*U_12 - 4*U02 + 4*U12 - 2*U22)/2.0;
                double D =    U_1_1 -   U0_1                   - 2*U_10 +    U00 +    U10         + 2*U_11 -    U01 -   U11         -   U_12 +   U02                ;
                double E =( 8*U_1_1 -14*U0_1 +10*U1_1 - 4*U2_1 -14*U_10 + 20*U00 - 13*U10 + 7*U20 +10*U_11 - 13*U01 + 8*U11 - 5*U21 - 4*U_12 + 7*U02 - 5*U12 + 2*U22)/2.0;
                double F =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 -   U_10 +    U00 -    U10 +   U20          +    U01 -   U11                                         ;
                double G =(         -   U0_1                            +  3*U00                           -  3*U01                          +   U02                )/2.0;
                double H =(-4*U_1_1 + 4*U0_1                   + 7*U_10 -  4*U00 -  3*U10         - 5*U_11 +  2*U01 + 3*U11         + 2*U_12 - 2*U02                )/2.0;
                double I =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 4*U_10 -  4*U00 +  2*U10 - 2*U20          -  3*U01 + 3*U11                                         )/2.0;
                double J =(                                    -   U_10 +  3*U00 -  3*U10 +   U20                                                                   )/2.0;
                double K =(         + 2*U0_1                            -  5*U00                           +  4*U01                          -   U02                )/2.0;
                double L =    U_1_1 -   U0_1                   -   U_10 +    U00                                                                                     ;
                double M =(                                    + 2*U_10 -  5*U00 +  4*U10 -   U20                                                                   )/2.0;
                double N =(         -   U0_1                                                               +    U01                                                 )/2.0;
                double O =(                                    -   U_10          +    U10                                                                           )/2.0;
                double P =                                              +    U00                                                                                    ;


                fTmp = A*Y3*X3 + B*Y3*X2 + C*Y2*X3 + D*Y3*rx + E*Y2*X2 + F*ry*X3 +
                       G*Y3 + H*Y2*rx + I*ry*X2 + J*X3 + K*Y2 + L*ry*rx + M*X2 + N*ry + O*rx + P;
                fTmp *= fFading;
                if (fTmp > 255.0) 
                    fTmp = 255.0;
                if (fTmp < 0.0) 
                    fTmp = 0.0;

                *pFrameY++ = (unsigned char)(fTmp) ;

                if (doyuv) {
					int offset;

                    ix = (int)ox/2;
                    iy = (int)oy/2;
                    rx = ox/2 - (double)ix;
                    ry = oy/2 - (double)iy;
                    X2 = rx*rx;
                    X3 = rx*rx*rx;
                    Y2 = ry*ry;
                    Y3 = ry*ry*ry;

                    offset = ix + iOldWidth_2*iy;

                    pTmp = (unsigned char *)(pWMVDec->m_dibBitsU + offset);

                    U_1_1 = *(pTmp - 1 -   iOldWidth_2);
                    U0_1  = *(pTmp     -   iOldWidth_2);
                    U1_1  = *(pTmp + 1 -   iOldWidth_2);
                    U2_1  = *(pTmp + 2 -   iOldWidth_2);
                    U_10  = *(pTmp - 1                );
                    U00   = *(pTmp                    );
                    U10   = *(pTmp + 1                );
                    U20   = *(pTmp + 2                );
                    U_11  = *(pTmp - 1 +   iOldWidth_2);
                    U01   = *(pTmp     +   iOldWidth_2);
                    U11   = *(pTmp + 1 +   iOldWidth_2);
                    U21   = *(pTmp + 2 +   iOldWidth_2);
                    U_12  = *(pTmp - 1 + 2*iOldWidth_2);
                    U02   = *(pTmp     + 2*iOldWidth_2);
                    U12   = *(pTmp + 1 + 2*iOldWidth_2);
                    U22   = *(pTmp + 2 + 2*iOldWidth_2);

                    A =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 - 2*U_10 +  3*U00 -  3*U10 + 2*U20 + 2*U_11 -  3*U01 + 3*U11 - 2*U21 -   U_12 + 2*U02 - 2*U12 +   U22;
                    B =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 8*U_10 - 11*U00 +  7*U10 - 4*U20 - 8*U_11 + 11*U01 - 7*U11 + 4*U21 + 4*U_12 - 7*U02 + 5*U12 - 2*U22)/2.0;
                    C =(-4*U_1_1 + 8*U0_1 - 8*U1_1 + 4*U2_1 + 7*U_10 - 11*U00 + 11*U10 - 7*U20 - 5*U_11 +  7*U01 - 7*U11 + 5*U21 + 2*U_12 - 4*U02 + 4*U12 - 2*U22)/2.0;
                    D =    U_1_1 -   U0_1                   - 2*U_10 +    U00 +    U10         + 2*U_11 -    U01 -   U11         -   U_12 +   U02                ;
                    E =( 8*U_1_1 -14*U0_1 +10*U1_1 - 4*U2_1 -14*U_10 + 20*U00 - 13*U10 + 7*U20 +10*U_11 - 13*U01 + 8*U11 - 5*U21 - 4*U_12 + 7*U02 - 5*U12 + 2*U22)/2.0;
                    F =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 -   U_10 +    U00 -    U10 +   U20          +    U01 -   U11                                         ;
                    G =(         -   U0_1                            +  3*U00                           -  3*U01                          +   U02                )/2.0;
                    H =(-4*U_1_1 + 4*U0_1                   + 7*U_10 -  4*U00 -  3*U10         - 5*U_11 +  2*U01 + 3*U11         + 2*U_12 - 2*U02                )/2.0;
                    I =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 4*U_10 -  4*U00 +  2*U10 - 2*U20          -  3*U01 + 3*U11                                         )/2.0;
                    J =(                                    -   U_10 +  3*U00 -  3*U10 +   U20                                                                   )/2.0;
                    K =(         + 2*U0_1                            -  5*U00                           +  4*U01                          -   U02                )/2.0;
                    L =    U_1_1 -   U0_1                   -   U_10 +    U00                                                                                     ;
                    M =(                                    + 2*U_10 -  5*U00 +  4*U10 -   U20                                                                   )/2.0;
                    N =(         -   U0_1                                                               +    U01                                                 )/2.0;
                    O =(                                    -   U_10          +    U10                                                                           )/2.0;
                    P =                                              +    U00                                                                                    ;

                    fTmp = A*Y3*X3 + B*Y3*X2 + C*Y2*X3 + D*Y3*rx + E*Y2*X2 + F*ry*X3 +
                       G*Y3 + H*Y2*rx + I*ry*X2 + J*X3 + K*Y2 + L*ry*rx + M*X2 + N*ry + O*rx + P;

                    fTmp = (fTmp-128.0)*fFading + 128.0;
                    if (fTmp > 255.0) 
                        fTmp = 255.0;
                    if (fTmp < 0.0) 
                        fTmp = 0.0;

                    *pFrameU ++ = (unsigned char)(fTmp);

                    pTmp = (unsigned char *)(pWMVDec->m_dibBitsV + offset);
 
                    U_1_1 = *(pTmp - 1 -   iOldWidth_2);
                    U0_1  = *(pTmp     -   iOldWidth_2);
                    U1_1  = *(pTmp + 1 -   iOldWidth_2);
                    U2_1  = *(pTmp + 2 -   iOldWidth_2);
                    U_10  = *(pTmp - 1                );
                    U00   = *(pTmp                    );
                    U10   = *(pTmp + 1                );
                    U20   = *(pTmp + 2                );
                    U_11  = *(pTmp - 1 +   iOldWidth_2);
                    U01   = *(pTmp     +   iOldWidth_2);
                    U11   = *(pTmp + 1 +   iOldWidth_2);
                    U21   = *(pTmp + 2 +   iOldWidth_2);
                    U_12  = *(pTmp - 1 + 2*iOldWidth_2);
                    U02   = *(pTmp     + 2*iOldWidth_2);
                    U12   = *(pTmp + 1 + 2*iOldWidth_2);
                    U22   = *(pTmp + 2 + 2*iOldWidth_2);

                    A =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 - 2*U_10 +  3*U00 -  3*U10 + 2*U20 + 2*U_11 -  3*U01 + 3*U11 - 2*U21 -   U_12 + 2*U02 - 2*U12 +   U22;
                    B =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 8*U_10 - 11*U00 +  7*U10 - 4*U20 - 8*U_11 + 11*U01 - 7*U11 + 4*U21 + 4*U_12 - 7*U02 + 5*U12 - 2*U22)/2.0;
                    C =(-4*U_1_1 + 8*U0_1 - 8*U1_1 + 4*U2_1 + 7*U_10 - 11*U00 + 11*U10 - 7*U20 - 5*U_11 +  7*U01 - 7*U11 + 5*U21 + 2*U_12 - 4*U02 + 4*U12 - 2*U22)/2.0;
                    D =    U_1_1 -   U0_1                   - 2*U_10 +    U00 +    U10         + 2*U_11 -    U01 -   U11         -   U_12 +   U02                ;
                    E =( 8*U_1_1 -14*U0_1 +10*U1_1 - 4*U2_1 -14*U_10 + 20*U00 - 13*U10 + 7*U20 +10*U_11 - 13*U01 + 8*U11 - 5*U21 - 4*U_12 + 7*U02 - 5*U12 + 2*U22)/2.0;
                    F =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 -   U_10 +    U00 -    U10 +   U20          +    U01 -   U11                                         ;
                    G =(         -   U0_1                            +  3*U00                           -  3*U01                          +   U02                )/2.0;
                    H =(-4*U_1_1 + 4*U0_1                   + 7*U_10 -  4*U00 -  3*U10         - 5*U_11 +  2*U01 + 3*U11         + 2*U_12 - 2*U02                )/2.0;
                    I =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 4*U_10 -  4*U00 +  2*U10 - 2*U20          -  3*U01 + 3*U11                                         )/2.0;
                    J =(                                    -   U_10 +  3*U00 -  3*U10 +   U20                                                                   )/2.0;
                    K =(         + 2*U0_1                            -  5*U00                           +  4*U01                          -   U02                )/2.0;
                    L =    U_1_1 -   U0_1                   -   U_10 +    U00                                                                                     ;
                    M =(                                    + 2*U_10 -  5*U00 +  4*U10 -   U20                                                                   )/2.0;
                    N =(         -   U0_1                                                               +    U01                                                 )/2.0;
                    O =(                                    -   U_10          +    U10                                                                           )/2.0;
                    P =                                              +    U00                                                                                    ;

                    fTmp = A*Y3*X3 + B*Y3*X2 + C*Y2*X3 + D*Y3*rx + E*Y2*X2 + F*ry*X3 +
                       G*Y3 + H*Y2*rx + I*ry*X2 + J*X3 + K*Y2 + L*ry*rx + M*X2 + N*ry + O*rx + P;

                    if (fTmp > 255.0) 
                        fTmp = 255.0;
                    if (fTmp < 0.0) 
                        fTmp = 0.0;

                    fTmp = (fTmp-128.0)*fFading + 128.0;
                    *pFrameV ++ = (unsigned char)(fTmp);
                    
                    doyuv = 0;
                }
                else 
                    doyuv = 1;
            }
            else {
                *pFrameY++ = 0 ;
                if (doyuv) {
                    *pFrameU ++ = 128;
                    *pFrameV ++ = 128;
                    doyuv       =   0;
                }
                else 
                    doyuv       = 1;
            }
        }

        y++;
        ox = fB*y +fC - fA;
        oy = fE*y +fF - fD;

        for (x=0; x<pWMVDec->m_iNewWidth; x++) {
            ox += fA;
            oy += fD;
            if ((ox >= 1.0) && (ox < (pWMVDec->m_iOldWidth-1)) && (oy >= 1.0) && (oy < (pWMVDec->m_iOldHeight-1))) {
                int ix = (int)ox;
                int iy = (int)oy;
                double rx = ox - (double)ix;
                double ry = oy - (double)iy;
                double X2 = rx*rx;
                double X3 = rx*rx*rx;
                double Y2 = ry*ry;
                double Y3 = ry*ry*ry;

				unsigned char *pTmp = (unsigned char *)(pWMVDec->m_dibBitsY + ix  + pWMVDec->m_iOldWidth*iy);

                double U_1_1 = *(pTmp - 1 -   pWMVDec->m_iOldWidth);
                double U0_1  = *(pTmp     -   pWMVDec->m_iOldWidth);
                double U1_1  = *(pTmp + 1 -   pWMVDec->m_iOldWidth);
                double U2_1  = *(pTmp + 2 -   pWMVDec->m_iOldWidth);
                double U_10  = *(pTmp - 1                );
                double U00   = *(pTmp                    );
                double U10   = *(pTmp + 1                );
                double U20   = *(pTmp + 2                );
                double U_11  = *(pTmp - 1 +   pWMVDec->m_iOldWidth);
                double U01   = *(pTmp     +   pWMVDec->m_iOldWidth);
                double U11   = *(pTmp + 1 +   pWMVDec->m_iOldWidth);
                double U21   = *(pTmp + 2 +   pWMVDec->m_iOldWidth);
                double U_12  = *(pTmp - 1 + 2*pWMVDec->m_iOldWidth);
                double U02   = *(pTmp     + 2*pWMVDec->m_iOldWidth);
                double U12   = *(pTmp + 1 + 2*pWMVDec->m_iOldWidth);
                double U22   = *(pTmp + 2 + 2*pWMVDec->m_iOldWidth);

                double A =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 - 2*U_10 +  3*U00 -  3*U10 + 2*U20 + 2*U_11 -  3*U01 + 3*U11 - 2*U21 -   U_12 + 2*U02 - 2*U12 +   U22;
                double B =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 8*U_10 - 11*U00 +  7*U10 - 4*U20 - 8*U_11 + 11*U01 - 7*U11 + 4*U21 + 4*U_12 - 7*U02 + 5*U12 - 2*U22)/2.0;
                double C =(-4*U_1_1 + 8*U0_1 - 8*U1_1 + 4*U2_1 + 7*U_10 - 11*U00 + 11*U10 - 7*U20 - 5*U_11 +  7*U01 - 7*U11 + 5*U21 + 2*U_12 - 4*U02 + 4*U12 - 2*U22)/2.0;
                double D =    U_1_1 -   U0_1                   - 2*U_10 +    U00 +    U10         + 2*U_11 -    U01 -   U11         -   U_12 +   U02                ;
                double E =( 8*U_1_1 -14*U0_1 +10*U1_1 - 4*U2_1 -14*U_10 + 20*U00 - 13*U10 + 7*U20 +10*U_11 - 13*U01 + 8*U11 - 5*U21 - 4*U_12 + 7*U02 - 5*U12 + 2*U22)/2.0;
                double F =    U_1_1 - 2*U0_1 + 2*U1_1 -   U2_1 -   U_10 +    U00 -    U10 +   U20          +    U01 -   U11                                         ;
                double G =(         -   U0_1                            +  3*U00                           -  3*U01                          +   U02                )/2.0;
                double H =(-4*U_1_1 + 4*U0_1                   + 7*U_10 -  4*U00 -  3*U10         - 5*U_11 +  2*U01 + 3*U11         + 2*U_12 - 2*U02                )/2.0;
                double I =(-4*U_1_1 + 7*U0_1 - 5*U1_1 + 2*U2_1 + 4*U_10 -  4*U00 +  2*U10 - 2*U20          -  3*U01 + 3*U11                                         )/2.0;
                double J =(                                    -   U_10 +  3*U00 -  3*U10 +   U20                                                                   )/2.0;
                double K =(         + 2*U0_1                            -  5*U00                           +  4*U01                          -   U02                )/2.0;
                double L =    U_1_1 -   U0_1                   -   U_10 +    U00                                                                                     ;
                double M =(                                    + 2*U_10 -  5*U00 +  4*U10 -   U20                                                                   )/2.0;
                double N =(         -   U0_1                                                               +    U01                                                 )/2.0;
                double O =(                                    -   U_10          +    U10                                                                           )/2.0;
                double P =                                              +    U00                                                                                    ;


                fTmp = A*Y3*X3 + B*Y3*X2 + C*Y2*X3 + D*Y3*rx + E*Y2*X2 + F*ry*X3 +
                       G*Y3 + H*Y2*rx + I*ry*X2 + J*X3 + K*Y2 + L*ry*rx + M*X2 + N*ry + O*rx + P;

                fTmp *= fFading;
                if (fTmp > 255.0) 
                    fTmp = 255.0;
                if (fTmp < 0.0) 
                    fTmp = 0.0;

                *pFrameY++ = (unsigned char)(fTmp) ;

            }
            else
                *pFrameY++ = 0;

        }
    }
    return tWMVStatus;
}

tWMVDecodeStatus Affine_TransformFadingInt1 (
        HWMVDecoder* phWMVDecoder,
        double fA, double fB, double fC,
        double fD, double fE, double fF, double fFading)
{
    tWMVDecInternalMember *pWMVDec; 

    unsigned char *pTmpY;
    unsigned char *pTmpU;
    unsigned char *pTmpV;
    int iOldWidth_2;
    unsigned char * pFrameY;
    unsigned char * pFrameU;
    unsigned char * pFrameV;
    double factor1;
    double factor2;
    double factor3;

    long factor;
    long power;
    long power_1;
    long mask;
    long iFading;

    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;

    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_TransformFadingInt1);

    pTmpY = (unsigned char *)pWMVDec->m_dibBitsY;
    pTmpU = (unsigned char *)pWMVDec->m_dibBitsU;
    pTmpV = (unsigned char *)pWMVDec->m_dibBitsV;
    iOldWidth_2 = pWMVDec->m_iOldWidth/2;
    pFrameY = pWMVDec->m_pFrameY;
    pFrameU = pWMVDec->m_pFrameU;
    pFrameV = pWMVDec->m_pFrameV;
    factor1 = sqrt(fA*fA + fB*fB);
    factor2 = sqrt(fD*fD + fE*fE);
    factor3 = max( min( min(factor1,factor2), 1.0), 0.001);

    iFading = (long)(fFading * 256+0.5);
    iFading = min(256, max(iFading, 0));

    if (factor3 < .25) {
        factor = 512;
        power  = 9;
        power_1= 10;
        mask   = 0x1FF;
    }
    else if (factor3 < .5) {
        factor = 256;
        power  = 8;
        power_1= 9;
        mask   = 0xFF;
    }
    else {
        factor = 128;
        power  = 7;
        power_1= 8;
        mask   = 0x7F;
    }

    //From Here - all integers
    {
        long lA = (long)(fA * factor); 
        long lB = (long)(fB * factor);
        long lC = (long)(fC * factor);
        long lD = (long)(fD * factor);
        long lE = (long)(fE * factor);
        long lF = (long)(fF * factor);

        long iOldWidthxFactor  = pWMVDec->m_iOldWidth*factor;
        long iOldHeightxFactor = pWMVDec->m_iOldHeight*factor;

		int y;

        for (y=0; y<pWMVDec->m_iNewHeight; y++) {
            long ox = lB*y +lC - lA;
            long oy = lE*y +lF - lD;
            int doyuv = 1;
			int x;

            for (x=0; x<pWMVDec->m_iNewWidth; x++) {
                ox += lA;
                oy += lD;
                if ((ox >= 0) && (ox < iOldWidthxFactor) && (oy >= 0) && (oy < iOldHeightxFactor)) {
                    long ix = ox >> power;
                    long iy = oy >> power;
                    long rx = ox & mask;
                    long ry = oy & mask;
                    unsigned char *pTmp = (unsigned char *)(pWMVDec->m_dibBitsY + ix  + pWMVDec->m_iOldWidth*iy);

                    long U00 = *(pTmp                  );
                    long U10 = *(pTmp + 1              );
                    long U01 = *(pTmp     + pWMVDec->m_iOldWidth);
     
                    *pFrameY = (unsigned char)( (( (U00*(factor-rx-ry)+ U01*ry + U10*rx ) * iFading)>>8) >> power);
                    pFrameY++;
                    if (doyuv) {
						int offset;
						long lrxry;

                        ix = ox >> power_1;
                        iy = oy >> power_1;
                        rx = (ox>>1) & mask;
                        ry = (oy>>1) & mask;
                        offset = ix + iOldWidth_2*iy;
                        lrxry = factor-rx-ry;

                        pTmp = (unsigned char *)(pTmpU + offset);

                        U00 = *(pTmp);
                        U10 = *(pTmp + 1              );
                        U01 = *(pTmp     + iOldWidth_2);

                        *pFrameU = (unsigned char)((((U00*(lrxry) + U01*ry+ U10*rx) * iFading)>>8) >> power);

                        pFrameU++;
                        pTmp = (unsigned char *)(pTmpV + offset);

                        U00 = *(pTmp);
                        U10 = *(pTmp + 1              );
                        U01 = *(pTmp     + iOldWidth_2);

                        *pFrameV = (unsigned char)((((U00*(lrxry) + U01*ry+ U10*rx) * iFading)>>8) >> power);
                        pFrameV++;
                        doyuv = 0;
                    }
                    else 
                        doyuv = 1;
                }
            }

            y++;
            ox = lB*y +lC - lA;
            oy = lE*y +lF - lD;

            for (x=0; x<pWMVDec->m_iNewWidth; x++) {
                ox += lA;
                oy += lD;
                if ((ox >= 0) && (ox < iOldWidthxFactor) && (oy >= 0) && (oy < iOldHeightxFactor)) {
                    long ix = ox >> power;
                    long iy = oy >> power;
                    long rx = ox & mask;
                    long ry = oy & mask;
                    unsigned char *pTmp = (unsigned char *)(pWMVDec->m_dibBitsY + ix  + pWMVDec->m_iOldWidth*iy);

                    long U00 = *(pTmp                  );
                    long U10 = *(pTmp + 1              );
                    long U01 = *(pTmp     + pWMVDec->m_iOldWidth);

                    *pFrameY++ = (unsigned char)((((U00*(factor-rx-ry)+ U01*ry + U10*rx) * iFading)>>8) >> power);

                }
            }
        }
    }
    return tWMVStatus;
}

tWMVDecodeStatus Affine_TransformInt1 (
        HWMVDecoder* phWMVDecoder,
        double fA, double fB, double fC,
        double fD, double fE, double fF)
{
    tWMVDecInternalMember *pWMVDec; 

    unsigned char *pTmpY;
    unsigned char *pTmpU;
    unsigned char *pTmpV;
    int iOldWidth_2;
    unsigned char * pFrameY;
    unsigned char * pFrameU;
    unsigned char * pFrameV;
    double factor1;
    double factor2;
    double factor3;

    long factor;
    long power;
    long power_1;
    long mask;

    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;

    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_TransformInt1);

    pTmpY = (unsigned char *)pWMVDec->m_dibBitsY;
    pTmpU = (unsigned char *)pWMVDec->m_dibBitsU;
    pTmpV = (unsigned char *)pWMVDec->m_dibBitsV;
    iOldWidth_2 = pWMVDec->m_iOldWidth/2;
    pFrameY = pWMVDec->m_pFrameY;
    pFrameU = pWMVDec->m_pFrameU;
    pFrameV = pWMVDec->m_pFrameV;
    factor1 = sqrt(fA*fA + fB*fB);
    factor2 = sqrt(fD*fD + fE*fE);
    factor3 = max( min( min(factor1,factor2), 1.0), 0.001);

    if (factor3 < .25) {
        factor = 512;
        power  = 9;
        power_1= 10;
        mask   = 0x1FF;
    }
    else if (factor3 < .5) {
        factor = 256;
        power  = 8;
        power_1= 9;
        mask   = 0xFF;
    }
    else {
        factor = 128;
        power  = 7;
        power_1= 8;
        mask   = 0x7F;
    }

    //From Here - all integers
    {
        long lA = (long)(fA * factor); 
        long lB = (long)(fB * factor);
        long lC = (long)(fC * factor);
        long lD = (long)(fD * factor);
        long lE = (long)(fE * factor);
        long lF = (long)(fF * factor);

        long iOldWidthxFactor  = pWMVDec->m_iOldWidth*factor;
        long iOldHeightxFactor = pWMVDec->m_iOldHeight*factor;

		int y;

        for (y=0; y<pWMVDec->m_iNewHeight; y++) {
            long ox = lB*y +lC - lA;
            long oy = lE*y +lF - lD;
            int doyuv = 1;
			int x;

            for (x=0; x<pWMVDec->m_iNewWidth; x++) {
                ox += lA;
                oy += lD;
                if ((ox >= 0) && (ox < iOldWidthxFactor) && (oy >= 0) && (oy < iOldHeightxFactor)) {
                    long ix = ox >> power;
                    long iy = oy >> power;
                    long rx = ox & mask;
                    long ry = oy & mask;
                    unsigned char *pTmp = (unsigned char *)(pWMVDec->m_dibBitsY + ix  + pWMVDec->m_iOldWidth*iy);

                    long U00 = *(pTmp                  );
                    long U10 = *(pTmp + 1              );
                    long U01 = *(pTmp     + pWMVDec->m_iOldWidth);
     
                    *pFrameY = (unsigned char)((U00*(factor-rx-ry)+ U01*ry + U10*rx ) >> power);
                    pFrameY++;
                    if (doyuv) {
						int offset;
						long lrxry;

                        ix = ox >> power_1;
                        iy = oy >> power_1;
                        rx = (ox>>1) & mask;
                        ry = (oy>>1) & mask;
                        offset = ix + iOldWidth_2*iy;
                        lrxry = factor-rx-ry;

                        pTmp = (unsigned char *)(pTmpU + offset);

                        U00 = *(pTmp);
                        U10 = *(pTmp + 1              );
                        U01 = *(pTmp     + iOldWidth_2);

                        *pFrameU = (unsigned char)((U00*(lrxry) + U01*ry+ U10*rx) >> power);

                        pFrameU++;
                        pTmp = (unsigned char *)(pTmpV + offset);

                        U00 = *(pTmp);
                        U10 = *(pTmp + 1              );
                        U01 = *(pTmp     + iOldWidth_2);

                        *pFrameV = (unsigned char)((U00*(lrxry) + U01*ry+ U10*rx) >> power);
                        pFrameV++;
                        doyuv = 0;
                    }
                    else 
                        doyuv = 1;
                }
            }

            y++;
            ox = lB*y +lC - lA;
            oy = lE*y +lF - lD;

            for (x=0; x<pWMVDec->m_iNewWidth; x++) {
                ox += lA;
                oy += lD;
                if ((ox >= 0) && (ox < iOldWidthxFactor) && (oy >= 0) && (oy < iOldHeightxFactor)) {
                    long ix = ox >> power;
                    long iy = oy >> power;
                    long rx = ox & mask;
                    long ry = oy & mask;
                    unsigned char *pTmp = (unsigned char *)(pWMVDec->m_dibBitsY + ix  + pWMVDec->m_iOldWidth*iy);

                    long U00 = *(pTmp                  );
                    long U10 = *(pTmp + 1              );
                    long U01 = *(pTmp     + pWMVDec->m_iOldWidth);

                    *pFrameY++ = (unsigned char)((U00*(factor-rx-ry)+ U01*ry + U10*rx) >> power);

                }
            }
        }
    }
    return tWMVStatus;
}

tWMVDecodeStatus Affine_TransformInt2 (
        HWMVDecoder* phWMVDecoder,
        double fA, double fB, double fC,
        double fD, double fE, double fF)
{
    tWMVDecInternalMember *pWMVDec; 

    unsigned char *pTmpY;
    unsigned char *pTmpU;
    unsigned char *pTmpV;
    int iOldWidth_2;
    unsigned char * pFrameY;
    unsigned char * pFrameU;
    unsigned char * pFrameV;
    double factor1;
    double factor2;
    double factor3;

    long factor;
    long power;
    long power_1;
    long mask;

    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_TransformInt2);


    pTmpY = (unsigned char *)pWMVDec->m_dibBitsY;
    pTmpU = (unsigned char *)pWMVDec->m_dibBitsU;
    pTmpV = (unsigned char *)pWMVDec->m_dibBitsV;
    iOldWidth_2 = pWMVDec->m_iOldWidth/2;
    pFrameY = pWMVDec->m_pFrameY;
    pFrameU = pWMVDec->m_pFrameU;
    pFrameV = pWMVDec->m_pFrameV;
    factor1 = sqrt(fA*fA + fB*fB);
    factor2 = sqrt(fD*fD + fE*fE);
    factor3 = max( min( min(factor1,factor2), 1.0), 0.001);

    if (factor3 < .25) {
        factor = 512;
        power  = 9;
        power_1= 10;
        mask   = 0x1FF;
    }
    else if (factor3 < .5) {
        factor = 256;
        power  = 8;
        power_1= 9;
        mask   = 0xFF;
    }
    else {
        factor = 128;
        power  = 7;
        power_1= 8;
        mask   = 0x7F;
    }

    //From here, All Integers
    {
        long lA = (long)(fA * factor); 
        long lB = (long)(fB * factor);
        long lC = (long)(fC * factor);
        long lD = (long)(fD * factor);
        long lE = (long)(fE * factor);
        long lF = (long)(fF * factor);
		int  y;

        long iOldWidthxFactor  = pWMVDec->m_iOldWidth*factor;
        long iOldHeightxFactor = pWMVDec->m_iOldHeight*factor;
        for (y=0; y<pWMVDec->m_iNewHeight; y++) {
            long ox = lB*y +lC - lA;
            long oy = lE*y +lF - lD;
            int doyuv = 1;
			int x;

            for (x=0; x<pWMVDec->m_iNewWidth; x++) {
                ox += lA;
                oy += lD;
                if ((ox >= 0) && (ox < iOldWidthxFactor) && (oy >= 0) && (oy < iOldHeightxFactor)) {
                    long ix = ox >> power;
                    long iy = oy >> power;
                    long rx = ox & mask;
                    long ry = oy & mask;
                    unsigned char *pTmp = (unsigned char *)(pWMVDec->m_dibBitsY + ix  + pWMVDec->m_iOldWidth*iy);

                    long U00 = *(pTmp                  );
                    long U10 = *(pTmp + 1              );
                    long U01 = *(pTmp     + pWMVDec->m_iOldWidth);
                    long U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

                    *pFrameY = (unsigned char)((U00*(factor-rx-ry)+ U01*ry + U10*rx 
                                                + (((U00+U11-U01-U10)*rx*ry) >> power)) >> power);
                    pFrameY++;
                    if (doyuv) {
						int offset;
						long rxry;
						long lrxry;

                        ix = ox >> power_1;
                        iy = oy >> power_1;
                        rx = (ox>>1) & mask;
                        ry = (oy>>1) & mask;
                        offset = ix + iOldWidth_2*iy;
                        rxry = rx*ry;
                        lrxry = factor-rx-ry;

                        pTmp = (unsigned char *)(pTmpU + offset);

                        U00 = *(pTmp);
                        U10 = *(pTmp + 1              );
                        U01 = *(pTmp     + iOldWidth_2);
                        U11 = *(pTmp + 1 + iOldWidth_2);

                        *pFrameU = (unsigned char)((U00*(lrxry) + U01*ry+ U10*rx+ 
                                            (((U00+U11-U01-U10)*rxry) >> power)) >> power);

                        pFrameU++;
                        pTmp = (unsigned char *)(pTmpV + offset);

                        U00 = *(pTmp);
                        U10 = *(pTmp + 1              );
                        U01 = *(pTmp     + iOldWidth_2);
                        U11 = *(pTmp + 1 + iOldWidth_2);

                        *pFrameV = (unsigned char)((U00*(lrxry) + U01*ry+ U10*rx+ 
                                            (((U00+U11-U01-U10)*rxry)>>power)) >> power);
                        pFrameV++;
                        doyuv = 0;
                    }
                    else 
                        doyuv = 1;
                }
            }

            y++;
            ox = lB*y +lC - lA;
            oy = lE*y +lF - lD;

            for (x=0; x<pWMVDec->m_iNewWidth; x++) {
                ox += lA;
                oy += lD;
                if ((ox >= 0) && (ox < iOldWidthxFactor) && (oy >= 0) && (oy < iOldHeightxFactor)) {
                    long ix = ox >> power;
                    long iy = oy >> power;
                    long rx = ox & mask;
                    long ry = oy & mask;
                    unsigned char *pTmp = (unsigned char *)(pWMVDec->m_dibBitsY + ix  + pWMVDec->m_iOldWidth*iy);

                    long U00 = *(pTmp                  );
                    long U10 = *(pTmp + 1              );
                    long U01 = *(pTmp     + pWMVDec->m_iOldWidth);
                    long U11 = *(pTmp + 1 + pWMVDec->m_iOldWidth);

                    *pFrameY++ = (unsigned char)((U00*(factor-rx-ry)+ U01*ry + U10*rx 
                                                + (((U00+U11-U01-U10)*rx*ry) >> power)) >> power);

                }
            }
        }
    }
    return tWMVStatus;
}

long CalculateCoeffs(double fOX1, double fOY1, double fNX1, double fNY1,
                     double fOX2, double fOY2, double fNX2, double fNY2,
                     double fOX3, double fOY3, double fNX3, double fNY3,
                     double *pfA, double *pfB, double *pfC,
                     double *pfD, double *pfE, double *pfF)
{
	double fDelta;

    *pfA = 0.0;
    *pfB = 0.0;
    *pfC = 0.0;
    *pfD = 0.0;
    *pfE = 0.0;
    *pfF = 0.0;

    fDelta = (fNX1 - fNX2)*(fNY1 - fNY3) - (fNX1 - fNX3)*(fNY1 - fNY2);
    if (fDelta == 0.0)
        return 1;

    *pfA = ((fOX1 - fOX2)*(fNY1 - fNY3) - (fOX1 - fOX3)*(fNY1 - fNY2)) / fDelta;
    *pfB = ((fOX1 - fOX3)*(fNX1 - fNX2) - (fOX1 - fOX2)*(fNX1 - fNX3)) / fDelta;
    *pfC = fOX1 - fNX1 * (*pfA) - fNY1 * (*pfB);
    *pfD = ((fOY1 - fOY2)*(fNY1 - fNY3) - (fOY1 - fOY3)*(fNY1 - fNY2)) / fDelta;
    *pfE = ((fOY1 - fOY3)*(fNX1 - fNX2) - (fOY1 - fOY2)*(fNX1 - fNX3)) / fDelta;
    *pfF = fOY1 - fNX1 * (*pfD) - fNY1 * (*pfE);
    
    return 0;
}

tWMVDecodeStatus Affine_Transform(HWMVDecoder* phWMVDecoder,
                      const unsigned char* pucSrcY,
                      const unsigned char* pucSrcU,
                      const unsigned char* pucSrcV,
                      unsigned char *pFrameY,
                      unsigned char *pFrameU,
                      unsigned char *pFrameV,
                      double fA, double fB, double fC,
                      double fD, double fE, double fF, double fFadding)
{

    tWMVDecInternalMember *pWMVDec; 
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Affine_Transform);

    if ((pucSrcY == 0)||(pucSrcU == 0)||(pucSrcV == 0)||
        (pFrameY == 0)||(pFrameU == 0)||(pFrameV == 0)  )
        return WMV_InValidArguments;

    pWMVDec->m_pFrameY     = pFrameY; 
    pWMVDec->m_pFrameU     = pFrameU; 
    pWMVDec->m_pFrameV     = pFrameV; 
    pWMVDec->m_dibBitsY   = pucSrcY;
    pWMVDec->m_dibBitsU   = pucSrcU;
    pWMVDec->m_dibBitsV   = pucSrcV;

    if ((fB == 0.0) &&(fD == 0.0)) {
        if (fA == fE) {
            if (fA == 1.0) {
                if (fFadding == 1.0) {
#                   ifdef _WMV_TARGET_X86_
                        if(g_bSupportMMX_WMV)
                            Affine_PanMMX(phWMVDecoder, fC, fF);
                        else
#                   endif
                    Affine_PanC(phWMVDecoder, fC, fF);
                }
                else {
#                   ifdef _WMV_TARGET_X86_
                        if(g_bSupportMMX_WMV)
                            Affine_PanMMXFading(phWMVDecoder, fC, fF, fFadding);
                        else
#                   endif
                    Affine_PanCFading(phWMVDecoder, fC, fF, fFadding);
                }
            }
            else {
                if (fFadding == 1.0) {
#                   ifdef _WMV_TARGET_X86_
                        if(g_bSupportMMX_WMV)
                            Affine_PanZoomMMX(phWMVDecoder, fA, fC, fF);
                        else 
#                   endif
                    Affine_PanZoomC(phWMVDecoder, fA, fC, fF);
                }
                else {
#                   ifdef _WMV_TARGET_X86_
                        if(g_bSupportMMX_WMV)
                            Affine_PanZoomMMXFading(phWMVDecoder, fA, fC, fF, fFadding);
                        else 
#                   endif
                    Affine_PanZoomCFading(phWMVDecoder, fA, fC, fF, fFadding);
                }
            }
        }
        else {
            if (fFadding == 1.0) {
#               ifdef _WMV_TARGET_X86_
                    if(g_bSupportMMX_WMV)
                        Affine_StretchMMX(phWMVDecoder, fA, fC, fE, fF);
                    else 
#               endif
                Affine_PanStretchC(phWMVDecoder, fA, fC, fE, fF);
            }
            else {
#               ifdef _WMV_TARGET_X86_
                    if(g_bSupportMMX_WMV)
                        Affine_StretchMMXFading(phWMVDecoder, fA, fC, fE, fF, fFadding);
                    else 
#               endif
                Affine_PanStretchCFading(phWMVDecoder, fA, fC, fE, fF, fFadding);
            }
        }
    }
    else {
        if (fFadding == 1.0) {
#ifdef WMV_TARGET_X86
	        Affine_TransformFloat2(phWMVDecoder, fA, fB, fC, fD, fE, fF);
#else
	        Affine_TransformFadingInt1(phWMVDecoder, fA, fB, fC, fD, fE, fF, fFadding);
#endif
		}
		else
		{
#ifdef WMV_TARGET_X86
	        Affine_TransformFadingFloat2(phWMVDecoder, fA, fB, fC, fD, fE, fF, fFadding);
#else
	        Affine_TransformFadingInt1(phWMVDecoder, fA, fB, fC, fD, fE, fF, fFadding);
#endif
		}
    }
    return tWMVStatus;
}


#define factor 0x800

Void_WMV PanZoom4PixelsC(PanZoomFunParams)
{
    unsigned char *pTmp;
    I32_WMV rx,ry,rxry,lrxry,t1,t2,t3,t4;
    U32_WMV i;
    long U00;
    long U10;
    long U01;
    long U11;
    ry = ry4_4way[0];

    for (i = 0; i < 4; i++)
    {
        pTmp = pTmpRef + pIx[i];
        rx = pRx[i];
        rxry = ((rx*ry));
        lrxry = ((factor>>4)-rx-(ry));

        U00 = *(pTmp                  );
        U10 = *(pTmp + 1              );
        U01 = *(pTmp     + iOldWidth);
        U11 = *(pTmp + 1 + iOldWidth);

        t1 = ((U00+U11-U01-U10)<<2)*(rxry);
        t2 = U00*(lrxry);
        t3 = U01*(ry);
        t4 = U10*(rx);

        t1 >>= 16;
        t2 >>= 7;
        t3 >>= 7;
        t4 >>= 7;

        *pDst++ = (unsigned char)(t2 + t3 + t4 + t1);
    }
}
#undef factor

#endif //WMV9_SIMPLE_ONLY
