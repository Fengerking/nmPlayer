//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) 2001  Microsoft Corporation

Module Name:

        interpolate.cpp

Abstract:

        Multirate functions : downsampling, interpolation (half and
        quarter pel bicubic filters), multires motion estimation

Author:

        Sridhar Srinivasan (sridhsri@microsoft.com) 19-February-2001

Revision History:

*************************************************************************/
//#ifdef X9
#include "voWmvPort.h"
#include "xplatform_wmv.h"
#include "limits.h"
#include "typedef.h"
#include "wmvdec_member.h"
#ifdef _WMV_TARGET_X86_
#include "cpudetect.h"
#endif // _WMV_TARGET_X86_
#ifdef macintosh
#include "cpudetect_mac.h"
#include "motioncomp_altivec.h"
#include "interpolate_wmv9_altivec.h"
#endif
#include "tables_wmv.h"

#include "strmdec_wmv.h"
#include "motioncomp_wmv.h"
#include "idctdec_wmv2.h"
#include "repeatpad_wmv.h"
#include "postfilter_wmv.h"

#include "spatialpredictor_wmv.h"
#include "localhuffman_wmv.h"
#include "tables_wmv.h"
#include "wmvdec_function.h"

#include <math.h>

extern Bool_WMV g_bSupportAltiVec_WMV;
extern Bool_WMV g_bSupportWMMX_WMV;
#ifdef _WMMX_
Void_WMV g_InterpolateBlockBicubic_WMMX (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl);
Void_WMV g_AddError_WMMX(U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch, U8_WMV* pcClapTabMC);
#endif
Void_WMV g_InterpolateBlockBicubic_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl);
Void_WMV g_InterpolateBlockBilinear_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl);
#ifdef _WMV_TARGET_X86_
extern Void_WMV g_InterpolateBlockBicubic_MMX (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl);
extern Void_WMV g_InterpolateBlockBicubic_SSE2 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl);
extern Void_WMV g_InterpolateBlockBilinear_MMX (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl);
extern Void_WMV g_InterpolateBlockBilinear_SSE2 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl);
extern Void_WMV g_AddError_MMX(U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch, U8_WMV* pcClapTabMC);
extern Void_WMV g_AddError_SSE2(U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch, U8_WMV* pcClapTabMC);
#endif

Void_WMV g_AddError_C(U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch, U8_WMV* pcClapTabMC);
Void_WMV g_AddErrorIntra_C(U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch, U8_WMV* pcClapTabMC);
Void_WMV g_InterpolateVariableRefBicubic_MMX (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl);
Void_WMV g_PixelMean (const U8_WMV *pSrc1, I32_WMV iStride1, const U8_WMV *pSrc2, I32_WMV iStride2, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iWidth, I32_WMV iHeight);
#ifdef _WMV_TARGET_X86_
extern Void_WMV g_OverlapBlockVerticalEdge_MMX(I16_WMV *pInOut, I32_WMV iStride);
extern Void_WMV g_OverlapBlockHorizontalEdge_MMX(I16_WMV *pSrcTop, I16_WMV *pSrcCurr,
                                             I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,
                                             Bool_WMV bTop, Bool_WMV bCurrent, Bool_WMV bWindup);
extern Void_WMV g_PixelMean_MMX (const U8_WMV *pSrc1, I32_WMV iStride1, const U8_WMV *pSrc2, I32_WMV iStride2, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iWidth, I32_WMV iHeight);
extern Void_WMV g_PixelMean_SSE1 (const U8_WMV *pSrc1, I32_WMV iStride1, const U8_WMV *pSrc2, I32_WMV iStride2, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iWidth, I32_WMV iHeight);
#endif

void Round_MV_Luam_FRAMEMV(tWMVDecInternalMember *pWMVDec, I32_WMV*IX, I32_WMV*IY,I32_WMV eBlk,I32_WMV xindex,I32_WMV yindex)
{
	I32_WMV X,Y;
	X = 4 * 16 * pWMVDec->m_ncols+ xindex;
    Y = 4 * 16 * pWMVDec->m_nrows + yindex;
	*IX = X>>2;
	*IY = Y>>2;

	if ((1 == eBlk) || (3 == eBlk))
    {
        *IX += 8;
    }

    if ((2 == eBlk) || (3 == eBlk))
    {
         *IY += 8;    /* add a block height */
    }

	if( *IX <= -32)  //zou fix
		*IX=  -31;
	else	if( *IX >= pWMVDec->m_iWidthPrevY+32)
		*IX = pWMVDec->m_iWidthPrevY+31 ;
	
	if( *IY <= -32)  //zou fix
	{
		*IY = *IY+8;
	}
	else	if( *IY >= pWMVDec->m_iHeightPrevY+32)
		*IY = pWMVDec->m_iHeightPrevY+31 ;
}

void Round_MV_Chroma_FRAMEMV(tWMVDecInternalMember *pWMVDec, I32_WMV *IX, I32_WMV*IY,I32_WMV* F,I32_WMV xindex,I32_WMV yindex)
{
	I32_WMV X,Y;
	X = 4 * 8 * pWMVDec->m_ncols+ xindex;
    Y = 4 * 8 * pWMVDec->m_nrows + yindex;
	*IX = X>>2;
	*IY = Y>>2;

	if( *IX <= -16)  //zou fix
		*IX=  -15;
	else	if( *IX >= pWMVDec->m_iWidthPrevUV+16)
		*IX = pWMVDec->m_iWidthPrevUV+15 ;
	
	if( *IY <= -16)  //zou fix
	{
		*IY = *IY+4;
	}
	else	if( *IY >= pWMVDec->m_iHeightPrevUV+16)
		*IY = pWMVDec->m_iHeightPrevUV+15 ;
}

void Round_MV_Luam_FILEDMV(tWMVDecInternalMember *pWMVDec, I32_WMV *IX, I32_WMV*IY,I32_WMV* F,I32_WMV xindex,I32_WMV yindex)
{
	I32_WMV X,Y;
	X = 4 * 16 * pWMVDec->m_ncols+ xindex;
    Y = 4 * 16 * pWMVDec->m_nrows + yindex;
	*IX = X>>2;
	*IY = Y>>2;
	*F  = *IY & 1;
	*IY = *IY >> 1;

	if( *IX <= -32)  //zou fix
		*IX=  -31;
	else	if( *IX >= pWMVDec->m_iWidthPrevY+32)
		*IX = pWMVDec->m_iWidthPrevY+31 ;
	
	
	if( *IY <= -16)  //zou fix
		*IY=  -15;
	else	if( *IY >= pWMVDec->m_iHeightPrevY+16)
		*IY = pWMVDec->m_iHeightPrevY+15 ;
}

void Round_MV_Chroma_FILEDMV(tWMVDecInternalMember *pWMVDec, I32_WMV *IX, I32_WMV*IY,I32_WMV* F,I32_WMV xindex,I32_WMV yindex)
{
	I32_WMV X,Y;
	X = 4 * 8 * pWMVDec->m_ncols+ xindex;
    Y = 4 * 8 * pWMVDec->m_nrows + yindex;
	*IX = X>>2;
	*IY = Y>>2;
	*F  = *IY & 1;
	*IY = *IY >> 1;

	if( *IX <= -16)  //zou fix
		*IX=  -15;
	else	if( *IX >= pWMVDec->m_iWidthPrevUV+16)
		*IX = pWMVDec->m_iWidthPrevUV+15 ;
	
	
	if( *IY <= -8)  //zou fix
		*IY=  -7;
	else	if( *IY >= pWMVDec->m_iHeightPrevUV+8)
		*IY = pWMVDec->m_iHeightPrevUV+7 ;
}
 
void Round_MV_Luam(tWMVDecInternalMember *pWMVDec, I32_WMV *iMvX, I32_WMV *iMvY/*,int blk*/)
{
	int xIndex = pWMVDec->m_ncols<<4;
	int yIndex = pWMVDec->m_nrows<<4;
	//int BX = -8-(blk&2)*8;

	if( (*iMvX>>2) + xIndex <= -32)  //zou fix
		*iMvX =  ((-31-xIndex)<<2) ;
	else	if( (*iMvX>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)
		*iMvX = ((pWMVDec->m_iWidthPrevY+31 -xIndex)<<2);

	//if( (*iMvY>>2) + yIndex <= -32)  //zou fix
	//	*iMvY =  (((*iMvY +32)>>2-yIndex)<<2);
	//else	if( (*iMvY>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)
	//	*iMvY = ((pWMVDec->m_iHeightPrevY+31 -yIndex)<<2) ;
	
	//if( (*iMvX>>2) + xIndex <= -32)  //zou fix
	//	*iMvX =  ((-31-xIndex)<<2) ;
	//else	if( (*iMvX>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)
	//	*iMvX = ((pWMVDec->m_iWidthPrevY+31 -xIndex)<<2);

	if( (*iMvY>>2) + yIndex <= -32)  //zou fix
		*iMvY =  ((-31-yIndex)<<2);
	else	if( (*iMvY>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)
		*iMvY = ((pWMVDec->m_iHeightPrevY+31 -yIndex)<<2) ;
}

void Round_MV_Luam_FRAMVEMV(tWMVDecInternalMember *pWMVDec, I32_WMV *iMvX, I32_WMV *iMvY/*,int blk*/)
{
	int xIndex = pWMVDec->m_ncols<<4;
	int yIndex = pWMVDec->m_nrows<<4;
	//int BX = -8-(blk&2)*8;
	
	if( (*iMvX>>2) + xIndex <= -32)  //zou fix
		*iMvX =  ((-32-xIndex)<<2) ;
	else	if( (*iMvX>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)
		*iMvX = ((pWMVDec->m_iWidthPrevY+32 -xIndex)<<2);

	if( (*iMvY>>2) + yIndex <= -32)  //zou fix
		*iMvY =  ((-32-yIndex)<<2);
	else	if( (*iMvY>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)
		*iMvY = ((pWMVDec->m_iHeightPrevY+32 -yIndex)<<2) ;
}

/***********************************************************************
  PullBackMotionVector : Pull motion vector into repeatpad range
***********************************************************************/
Bool_WMV PullBackMotionVector (tWMVDecInternalMember *pWMVDec,
    I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY)
{
	if(!pWMVDec->m_bCodecIsWVC1 )  //zou fix
	{
    I32_WMV iX = *iMvX;
    I32_WMV iY = *iMvY;
    I32_WMV iXCoord = imbX << 4;
    I32_WMV iYCoord = imbY << 4;
    Bool_WMV bPullBack = FALSE;
    // this is required for correct rounding in repeatpad area
    I32_WMV iPosX = iXCoord + (iX >> 2);
    I32_WMV iPosY = iYCoord + (iY >> 2);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(PullBackMotionVector);
    
    if (iPosX < -16) {
        iPosX = -16; 
        bPullBack = TRUE;
    } else if (iPosX > (I32_WMV) pWMVDec->m_uintNumMBX << 4) {
        iPosX = pWMVDec->m_uintNumMBX << 4;
        bPullBack = TRUE;
    }

    if (iPosY < -16) {
        iPosY = -16;
        bPullBack = TRUE;
    } else if (iPosY > (I32_WMV) pWMVDec->m_uintNumMBY << 4) {
        iPosY = pWMVDec->m_uintNumMBY << 4;
        bPullBack = TRUE;
    }

    if (bPullBack) {
        iX = ((iPosX - iXCoord) << 2) + (iX & 3);
        iY = ((iPosY - iYCoord) << 2) + (iY & 3);
        *iMvX = iX;
        *iMvY = iY;
    }
    return bPullBack;
 }
	else
	{
		int X = *iMvX;
		int Y = *iMvY;
		int IX = imbX;
		int IY = imbY;//pPosition->SliceY + pPosition->Y;
		int Width  = pWMVDec->m_iWidthSource;
		int Height = pWMVDec->m_iHeightSource>>1;

		if(pWMVDec->m_bFieldMode)//323 		//if (pPosition->ePictureFormat == vc1_InterlacedField)
		{
			IY = IY << 1;
			Y  = Y<<1;
		}
		IX = (IX * 16) + (X >> 2);
		IY = (IY * 8)  + (Y >> 3); /* number of field lines */

		if (IX < -17)
			X -= 4*(IX+17);
		else if (IX > Width)
			X -= 4*(IX-Width);
		/* Need top pad of 38 and bottom pad of 38 */
		if (IY < -18)
			Y -= 8*(IY+18);
		else if (IY > Height+1)
			Y -= 8*(IY-Height-1);
		if(pWMVDec->m_bFieldMode)//323
		{
			Y = Y>>1;
		}
		*iMvX = X;
		*iMvY = Y;
		return 0;
	}


}

Bool_WMV PullBackMotionVector_UV (tWMVDecInternalMember* pWMVDec, I32_WMV*iMvX, I32_WMV*iMvY, I32_WMV imbX, I32_WMV imbY)  //zou fix  add this function
{   //zou fix 
	int X = *iMvX;
	int Y = *iMvY;
	int IX = imbX;
	int IY = imbY;//pPosition->SliceY + pPosition->Y;

	int Width  = pWMVDec->m_iWidthSource>>1;
	int Height = pWMVDec->m_iHeightSource>>2;

	 int iMinY = -8;
	 int iMaxY = Height;
	if(pWMVDec->m_bFieldMode)//323
    {
		 iMinY--;
		 iMaxY++;
        IY = IY << 1;
        Y  = Y<<1;
    }

	IX = (IX * 8) + (X >> 2);
	IY = (IY * 4)  + (Y >> 3); /* number of field lines */

	if (IX < -8)
		X -= 4*(IX+8);
	else if (IX > Width)
		X -= 4*(IX-Width);

	/* Need top pad of 38 and bottom pad of 38 */
	if (IY < iMinY)
		Y -= 8*(IY-iMinY);
	else if (IY > iMaxY)
		Y -= 8*(IY-iMaxY);

	//if (pPosition->ePictureFormat == vc1_InterlacedField)
	if(pWMVDec->m_bFieldMode)//323
    {
        Y = Y>>1;
    }

	*iMvX = X;
	*iMvY = Y;

	return 0;
}

/***********************************************************************
  PredictHybridMV :
  Get predictor and test to see if condition for hybrid MV is met
***********************************************************************/
//#pragma warning(disable: 4799)  // emms warning disable

I32_WMV PredictHybridMV (tWMVDecInternalMember *pWMVDec, I32_WMV iX, I32_WMV iY, Bool_WMV b1MV,
                                I32_WMV *iPredX, I32_WMV *iPredY)
{
    I32_WMV sum;
    I32_WMV iRow = pWMVDec->m_uintNumMBX << 1;
    I32_WMV iIndex = iY * iRow + iX;
    I16_WMV *pXMotion = pWMVDec->m_pXMotion;
    I16_WMV *pYMotion = pWMVDec->m_pYMotion;
    Bool_WMV bTopBndry = FALSE;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(PredictHybridMV);

    if (!(iY & 1)) {
        bTopBndry = (iY == 0) || pWMVDec->m_pbStartOfSliceRow [iY >> 1];
    }


#ifdef _WMV9AP_
    if (pWMVDec->m_bFieldMode) {
        *iPredX = iX;
        *iPredY = iY;
#ifndef PPCWMP
        PredictFieldModeMV (pWMVDec, iPredX, iPredY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, b1MV, bTopBndry);
#endif
    }
    else
#endif
    PredictMV (pWMVDec, iX, iY, (I32_WMV) b1MV, pWMVDec->m_uintNumMBX << 1, pWMVDec->m_uintNumMBY << 1,
               pXMotion, pYMotion, iPredX, iPredY, bTopBndry);

    if (iX == 0 || bTopBndry)
        return 0;


    sum = (pXMotion[iIndex - 1] == IBLOCKMV) ?
        abs (*iPredX) + abs (*iPredY) :
        abs (*iPredX - pXMotion[iIndex - 1]) +
        abs (*iPredY - pYMotion[iIndex - 1]);
     
        
    if (sum > 32)
        return 1;


    sum = (pXMotion[iIndex - iRow] == IBLOCKMV) ?
        abs (*iPredX) + abs (*iPredY) :
        abs (*iPredX - pXMotion[iIndex - iRow]) +
        abs (*iPredY - pYMotion[iIndex - iRow]);



    if (sum > 32)
        return 1;

    return 0;
}

/***********************************************************************
  PredictMV : 
  Predict motion vector from componentwise medians
  (iX, iY) is the input coordinate, and computed motion is returned
  in the reference
***********************************************************************/

#if !defined( WMV_OPT_COMMON_ARM)
#ifdef _EMB_WMV3_
//#pragma code_seg (EMBSEC_PML)
#endif
 I32_WMV medianof4 (I32_WMV a0, I32_WMV a1, I32_WMV a2, I32_WMV a3)
{
    I32_WMV  max, min;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(medianof4);
    max = min = a0;
    if (a1 > max)
        max = a1;
    else if (a1 < min)
        min = a1;
    if (a2 > max)
        max = a2;
    else if (a2 < min)
        min = a2;
    if (a3 > max)
        max = a3;
    else if (a3 < min)
        min = a3;

    return (a0 + a1 + a2 + a3 - max - min) / 2;
}
#endif //WMV_OPT_COMMON_ARM

#ifdef _EMB_WMV3_
//#pragma code_seg (EMBSEC_DEF)
#endif
/***********************************************************************
  PredictMV : Motion vector predictor
  Used by codec, as well as encoder-only functions in pyramid.cpp.
  iMVMode parameter can take on values of 0, 1 and 2
  0 : 4MV mode
  1 : 1MV mode, with block-level resolution MV arrays pMVX, pMVY
  2 : 1MV mode, with MB-level resolution MV arrays
***********************************************************************/
I32_WMV PredictMV (tWMVDecInternalMember * pWMVDec, I32_WMV iBlockX, I32_WMV iBlockY, I32_WMV iMVMode,
                               I32_WMV iNumBlockX, I32_WMV iNumBlockY,
                               const I16_WMV *pMVX, const I16_WMV *pMVY,
                               I32_WMV *iXMotion, I32_WMV *iYMotion, Bool_WMV bTopBndry)
{
    I32_WMV  iX1 = iBlockX << 5;  // original qpel coordinate
    I32_WMV  iY1 = iBlockY << 5;
    I32_WMV  iXa, iXb, iXc, iYa, iYb, iYc;
    I32_WMV  iBlocks;
    I32_WMV iMinCoordinate;
    I32_WMV iMaxX ;
    I32_WMV iMaxY ;
    
    I32_WMV iIndex = iBlockY * iNumBlockX + iBlockX - 1;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(PredictMV);
    
    // find motion predictor
    if (!bTopBndry) {
        if (iBlockX) {
            // left block
            iXc = pMVX[iIndex];
            iYc = pMVY[iIndex];
        }
        else {
            // special case - 1 MB wide frame, 1MV mode, copy from top
            if (pWMVDec->m_uintNumMBX == 1 && iMVMode > 0) {
                iIndex += 1 - iNumBlockX;
                *iXMotion = pMVX[iIndex];
                *iYMotion = pMVY[iIndex];
                goto End;
            }
            iXc = iYc = 0;
        }
        
        iIndex -= iNumBlockX;
        iIndex++;
        
        // top block
        iXa = pMVX[iIndex];
        iYa = pMVY[iIndex];
        
        switch (iMVMode) {
        case 0: // 4MV
            if (pWMVDec->m_iBetaRTMMismatchIndex) {
                // Beta content
                iIndex += 1 - (((((iBlockX ^ iBlockY) & 1) == 0) || (iBlockX >= (iNumBlockX - 1))) << 1);
            }
            else {
                // RTM content
                if (iBlockX)
                    iIndex += 1 - (((((iBlockX ^ iBlockY) & 1) == 0) || (iBlockX >= (iNumBlockX - 1))) << 1);
                else // First block column blocks use upper right predictor for B
                    iIndex ++;
            }    
            break;
            
        case 1: // 1MV, block based
            iIndex += (iBlockX < (iNumBlockX - 2)) * 3 - 1;
            break;
            
        case 2: // 1MV, MB based
            iIndex += ((iBlockX < iNumBlockX - 1) << 1) - 1;
            break;
        }
        
        iXb = pMVX[iIndex];
        iYb = pMVY[iIndex];
        
        iBlocks = (iXa == IBLOCKMV) + (iXb == IBLOCKMV) + (iXc == IBLOCKMV);
        
        if (iBlocks > 1) {
            *iXMotion = IBLOCKMV;
        }
        else {
            if (iBlocks == 1) {
                if (iXa == IBLOCKMV)
                    iXa = iYa = 0;
                else if (iXb == IBLOCKMV)
                    iXb = iYb = 0;
                else if (iXc == IBLOCKMV)
                    iXc = iYc = 0;
            }
            
            *iXMotion = medianof3 (iXa, iXb, iXc);
            *iYMotion = medianof3 (iYa, iYb, iYc);
        }
End:;
    }
    else if (iBlockX > 0) {
        *iXMotion = pMVX[iIndex];
        *iYMotion = pMVY[iIndex];    
    }
    else {
        *iXMotion = *iYMotion = 0;
    }
    
    if (*iXMotion == IBLOCKMV) {
        *iXMotion = *iYMotion = 0;
        return 1;
    }
    
    // limit motion vector
    iX1 += *iXMotion;
    iY1 += *iYMotion;
    iMinCoordinate = (iMVMode == 1) ? -60 : -28;  // -15 or -7 pixels
    iMaxX = (iNumBlockX << 5) - 4;  // x8 for block x4 for qpel
    iMaxY = (iNumBlockY << 5) - 4;
    
    if (iX1 < iMinCoordinate)
        *iXMotion += iMinCoordinate - iX1;
    else if (iX1 > iMaxX)
        *iXMotion += iMaxX - iX1;
    
    if (iY1 < iMinCoordinate)
        *iYMotion += iMinCoordinate - iY1;
    else if (iY1 > iMaxY)
        *iYMotion += iMaxY - iY1;
    
    return 0;
}

/*
static I32_WMV s_rndTbl[7]={-1,0,0,0,0,0,1};
static I32_WMV *sp_rndTbl = s_rndTbl + 3;
static I32_WMV s_aggressiveRndTbl[7]={-1,0,1,0,-1,0,1};
static I32_WMV *sp_aggressiveRndTbl = s_aggressiveRndTbl + 3;
*/

#ifdef _WMV9AP_
I32_WMV PredictMVB (tWMVDecInternalMember * pWMVDec, I32_WMV iBlockX, I32_WMV iBlockY, I32_WMV iMVMode,
                               I32_WMV iNumBlockX, I32_WMV iNumBlockY,
                               const I16_WMV *pMVX, const I16_WMV *pMVY,
                               I32_WMV *iXMotion, I32_WMV *iYMotion, Bool_WMV bTopBndry)
{
    I32_WMV  iX1 = iBlockX * 8 * 4;  // original qpel coordinate
    I32_WMV  iY1 = iBlockY * 8 * 4;
    I32_WMV  iXa, iXb, iXc, iYa, iYb, iYc;
    I32_WMV  iIndex = iBlockY * iNumBlockX + iBlockX - 1, iBlocks;
I32_WMV iMinCoordinate;
I32_WMV iMaxX ;
I32_WMV iMaxY ;



        // find motion predictor
    if (!bTopBndry) {

        // left block
        if (iBlockX) {
            iXc = pMVX[iIndex];
            iYc = pMVY[iIndex];
        }
        else {
            // special case - 1 MB wide frame, 1MV mode, copy from top
            if (pWMVDec->m_uintNumMBX == 1 && iMVMode > 0) {
                iIndex += 1 - iNumBlockX;
                *iXMotion = pMVX[iIndex];
                *iYMotion = pMVY[iIndex];
                goto End;
            }
            iXc = iYc = 0;
        }
        iIndex -= iNumBlockX;
        iIndex++;

        // top block
        iXa = pMVX[iIndex];
        iYa = pMVY[iIndex];
            
        switch (iMVMode) {
        case 0: // 4MV
            if (pWMVDec->m_iBetaRTMMismatchIndex) {
                // Beta content
                iIndex += 1 - ((((iBlockX ^ iBlockY) & 1) == 0) || (iBlockX >= (iNumBlockX - 1))) * 2;
            }
            else {
                // RTM content
                if (iBlockX)
                    iIndex += 1 - ((((iBlockX ^ iBlockY) & 1) == 0) || (iBlockX >= (iNumBlockX - 1))) * 2;
                else // First block column blocks use upper right predictor for B
                    iIndex ++;
            }
            break;
        case 1: // 1MV, block based
            iIndex += (iBlockX < (iNumBlockX - 2)) * 3 - 1;
            break;
        case 2: // 1MV, MB based
            iIndex += (iBlockX < iNumBlockX - 1) * 2 - 1;
            break;
        }

        iXb = pMVX[iIndex];
        iYb = pMVY[iIndex];
        iBlocks = (iXa == IBLOCKMV) + (iXb == IBLOCKMV) + (iXc == IBLOCKMV);
            
        if (iBlocks > 1) {
            *iXMotion = IBLOCKMV;
        }
        else {
            if (iBlocks == 1) {
                if (iXa == IBLOCKMV)
                    iXa = iYa = 0;
                else if (iXb == IBLOCKMV)
                    iXb = iYb = 0;
                else if (iXc == IBLOCKMV)
                    iXc = iYc = 0;
            }
            *iXMotion = medianof3 (iXa, iXb, iXc);
            *iYMotion = medianof3 (iYa, iYb, iYc);
        }
End:;
    }
    else if (iBlockX > 0) {
        *iXMotion = pMVX[iIndex];
        *iYMotion = pMVY[iIndex];
    }
    else {
        *iXMotion = *iYMotion = 0;
    }

    if (*iXMotion == IBLOCKMV) {
        *iXMotion = *iYMotion = 0;
        return 1;
    }

	if(pWMVDec->m_bCodecIsWVC1)
	{
		int X = *iXMotion;
		int Y = *iYMotion;
		int IX = iBlockX;
		int IY = iBlockY;
		int Width  = iNumBlockX;
		int Height = iNumBlockY;
		int Min;

		IX = IX * 16 * 4 + X;
        IY = IY * 16 * 4 + Y;
        Width  = (Width  * 16 - 1)*4;
        Height = (Height * 16 - 1)*4;
        if (0 & 1)
        {
            IX += 8*4;
        }
        if (0 & 2)
        {
            IY += 8*4;
        }
        Min = -15*4;

		if (IX < Min)
		{
			X -= (IX-Min);
		}
		else if (IX > Width)
		{
			X -= (IX-Width);
		}

		if (IY < Min)
		{
			Y -= (IY-Min);
		}
		else if (IY > Height)
		{
			Y -= (IY-Height);
		}

		*iXMotion = X;
        *iYMotion = Y;

		return 0;
	}
	else
	{
    // limit motion vector
    iX1 += *iXMotion;
    iY1 += *iYMotion;
     iMinCoordinate = (iMVMode == 1) ? -60 : -28;  // -15 or -7 pixels
     iMaxX = iNumBlockX * 8 * 2 * 4 - 4;  // x8 for block x4 for qpel
     iMaxY = iNumBlockY * 8 * 2 * 4 - 4;
    if (iX1 < iMinCoordinate)
        *iXMotion += iMinCoordinate - iX1;
    else if (iX1 > iMaxX)
        *iXMotion += iMaxX - iX1;
    if (iY1 < iMinCoordinate)
        *iYMotion += iMinCoordinate - iY1;
    else if (iY1 > iMaxY)
        *iYMotion += iMaxY - iY1;
    return 0;
  }
}


#endif
static I32_WMV s_RndTbl[] = { 0, 0, 0, 1 };

#define SIGN(a) ((a < 0) ? -1 : 1)
#define ABS(x)      (((x) >= 0) ? (x) : -(x))                           /** Absolute value of x */
static int vc1DERIVEMV_SignedMod(int a, int b)
{
    return(SIGN(a) * (ABS(a) % b));
}

/***********************************************************************
  ChromaMV : Given MB index return chroma MV in-place
***********************************************************************/
I32_WMV ChromaMV (tWMVDecInternalMember * pWMVDec, I32_WMV iX, I32_WMV iY, Bool_WMV b1MV)
{
    
    I32_WMV iX0B, iY0B, iXf, iYf;
    
    I32_WMV  iX0 = iX, iY0 = iY;
    I32_WMV  iIndex;
    I32_WMV  iXa, iXb, iXc, iYa, iYb, iYc, iXd, iYd, iBlocks;
    U32_WMV  uintNumMBX = pWMVDec->m_uintNumMBX;
    U32_WMV  uintNumMBY = pWMVDec->m_uintNumMBY;
    I16_WMV  *pXMotion = pWMVDec->m_pXMotion;
    I16_WMV  *pYMotion = pWMVDec->m_pYMotion;
    I32_WMV  iNumMBXtimes8 = (I32_WMV )((pWMVDec->m_uintNumMBX<<3));
    I32_WMV  iNumMBYtimes8 = (I32_WMV )((pWMVDec->m_uintNumMBY<<3));

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ChromaMV);

    iX += iX + 1;
    iY += iY + 1;
    // find motion predictor
    
    iIndex  = (iY * uintNumMBX << 1) + iX;
    
    // 1MV case
    if (b1MV) {
        if (pXMotion[iIndex] == IBLOCKMV) {
            goto IntraBlock;
        }
        
        iX = pXMotion[iIndex];
        iY = pYMotion[iIndex];
        goto BoundsCheck;
    }

    // count I blocks
    
    iXd = pXMotion[iIndex];
    iYd = pYMotion[iIndex];
    iIndex--;
    
    
    iXc = pXMotion[iIndex];
    iYc = pYMotion[iIndex];
    
    iIndex -= uintNumMBX << 1;
    
    iXb = pXMotion[iIndex];
    iYb = pYMotion[iIndex];

    iIndex++;
    
    iXa = pXMotion[iIndex];
    iYa = pYMotion[iIndex];

    iBlocks = (iXa == IBLOCKMV) + (iXb == IBLOCKMV) + (iXc == IBLOCKMV) + (iXd == IBLOCKMV);
    
    if (iBlocks > 2) {
        goto IntraBlock;
    }
    else {
        if (iBlocks == 1) {
            if (iXa == IBLOCKMV) {
                iX = medianof3 (iXd, iXb, iXc);
                iY = medianof3 (iYd, iYb, iYc);
            }
            else if (iXb == IBLOCKMV) {
                iX = medianof3 (iXd, iXa, iXc);
                iY = medianof3 (iYd, iYa, iYc);
            }
            else if (iXc == IBLOCKMV) {
                iX = medianof3 (iXd, iXb, iXa);
                iY = medianof3 (iYd, iYb, iYa);
            }
            
            else if (iXd == IBLOCKMV) {
                iX = medianof3 (iXa, iXb, iXc);
                iY = medianof3 (iYa, iYb, iYc);
            }
        }
        else if (iBlocks == 2) {
            iX = iY = 0;
            if (iXa != IBLOCKMV) {
                iX += iXa;
                iY += iYa;
            }
            if (iXb != IBLOCKMV) {
                iX += iXb;
                iY += iYb;
            }
            if (iXc != IBLOCKMV) {
                iX += iXc;
                iY += iYc;
            }
            if (iXd != IBLOCKMV) {
                iX += iXd;
                iY += iYd;
            }
            iX /= 2;
            iY /= 2;
        }
        else {
            iX = medianof4 (iXa, iXb, iXc, iXd);
            iY = medianof4 (iYa, iYb, iYc, iYd);
        }
    }
    
BoundsCheck:
    assert (pWMVDec->m_tFrmType == PVOP);

    if (pWMVDec->m_iNumBFrames > 0) {
        iIndex = iY0 * (I32_WMV) pWMVDec->m_uintNumMBX + iX0;
        iXf = iX; iYf = iY;
        
                
        // Don't need this pullback code for WMVA because
        // we will do the pullback when direct MV's are generated
        if (pWMVDec->m_cvCodecVersion == WMV3)
        {
            iX0B = (iX0<<3) + (iXf >> 2);   // full-pel resolution
            iY0B = (iY0<<3) + (iYf >> 2);
            if (iX0B < -8) 
                iXf -= (iX0B + 8) * 4;
            else if (iX0B > iNumMBXtimes8) 
                iXf -= (iX0B - iNumMBXtimes8) * 4;
            if (iY0B < -8) 
                iYf -= (iY0B + 8) * 4;
            else if (iY0B > iNumMBYtimes8)
                iYf -= (iY0B - iNumMBYtimes8) * 4;
        }

        pWMVDec->m_rgmv1[iIndex].m_vctTrueHalfPel.x = (I16_WMV)iXf;
        pWMVDec->m_rgmv1[iIndex].m_vctTrueHalfPel.y = (I16_WMV)iYf;

#ifdef _WMV9AP_
        if (pWMVDec->m_cvCodecVersion >= WMVA) // auto mode
        {
            I32_WMV iTopLeftBlkIndex = 2*iY0*2*pWMVDec->m_uintNumMBX + iX0*2;
            I32_WMV iBotLeftBlkIndex = iTopLeftBlkIndex + 2*pWMVDec->m_uintNumMBX;
            pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = (I16_WMV)iXf;
            pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1 ] = (I16_WMV)iXf;
            pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] = (I16_WMV)iXf;
            pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1 ] = (I16_WMV)iXf;
            pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex] = (I16_WMV)iYf;
            pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + 1 ] = (I16_WMV)iYf;
            pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex] = (I16_WMV)iYf;
            pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + 1 ] = (I16_WMV)iYf;
        }
#endif
    }
    
    iX = (iX + s_RndTbl[iX & 3]) >> 1;
    iY = (iY + s_RndTbl[iY & 3]) >> 1;
    if (pWMVDec->m_bUVHpelBilinear) { //zou ---> if (pPosition->FastUVMC)
        if(!pWMVDec->m_bCodecIsWVC1){
	   const int RndTbl[3] = {1, 0, -1};
	   iX = iX + RndTbl[1 + vc1DERIVEMV_SignedMod(iX, 2)];
	   iY = iY + RndTbl[1 + vc1DERIVEMV_SignedMod(iY, 2)];
        }
        if (iX & 1) {
            if (iX > 0) iX--;
            else iX++;
        }
        if (iY & 1) {
            if (iY > 0) iY--;
            else iY++;
        }
    }
    
  if(!pWMVDec->m_bCodecIsWVC1){
    iIndex = iY0 * (I32_WMV) uintNumMBX + iX0;
    iX0 = (iX0 << 3) + (iX >> 2);
    iY0 = (iY0 << 3) + (iY >> 2);
    if (iX0 < -8)
        iX -= (iX0 + 8) << 2;
    else if (iX0 > (I32_WMV) (uintNumMBX << 3))
        iX -= (iX0 - (I32_WMV) (uintNumMBX << 3)) << 2;
    if (iY0 < -8)
        iY -= (iY0 + 8) << 2;
    else if (iY0 > (I32_WMV) (uintNumMBY << 3))
        iY -= (iY0 - (I32_WMV) (uintNumMBY << 3)) << 2;
   }

    pWMVDec->m_pXMotionC[iIndex] = (I16_WMV)(I32_WMV) iX; 
    pWMVDec->m_pYMotionC[iIndex] = (I16_WMV)(I32_WMV) iY;
    
    return 0;
    
IntraBlock:
    
    iIndex = iY0 * (I32_WMV) uintNumMBX + iX0;
    iX = iY = IBLOCKMV;
    pWMVDec->m_pXMotionC[iIndex] = (I16_WMV)(I32_WMV) iX; 
    pWMVDec->m_pYMotionC[iIndex] = (I16_WMV)(I32_WMV) iY;
    if (pWMVDec->m_iNumBFrames>0 && pWMVDec->m_tFrmType == PVOP) {
        pWMVDec->m_rgmv1[iIndex].m_vctTrueHalfPel.x = 0;
        pWMVDec->m_rgmv1[iIndex].m_vctTrueHalfPel.y = 0;
#ifdef _WMV9AP_
                if (pWMVDec->m_cvCodecVersion >= WMVA) // auto mode
        {
            I32_WMV iTopLeftBlkIndex = 2*iY0*2*pWMVDec->m_uintNumMBX + iX0*2;
            I32_WMV iBotLeftBlkIndex = iTopLeftBlkIndex + 2*pWMVDec->m_uintNumMBX;
            pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = IBLOCKMV;
            pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1 ] = IBLOCKMV;
            pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] = IBLOCKMV;
            pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1 ] = IBLOCKMV;
            pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex] = IBLOCKMV;
            pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + 1 ] = IBLOCKMV;
            pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex] = IBLOCKMV;
            pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + 1 ] = IBLOCKMV;
        }
#endif
    }
    
    return 1;
}

/***********************************************************************
  SetupAlignedConsts : Align 64-bit constants to 32-byte boundaries
***********************************************************************/
//__int64* s_pAlignedFilterTaps;
//__int64 s_pFilterTapsBuf[8];

Void_WMV g_SetupAlignedConsts ()
{
    //DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_SetupAlignedConsts);
    //s_pAlignedFilterTaps = (__int64 *)(((DWORD_PTR)s_pFilterTapsBuf + 31) & ~31);
    //*(s_pAlignedFilterTaps + 0) = 0x0003000300030003; 
    //*(s_pAlignedFilterTaps + 1) = 0x0009000900090009; 
    //*(s_pAlignedFilterTaps + 2) = 0x0012001200120012; 
    //*(s_pAlignedFilterTaps + 3) = 0x0035003500350035;
}

/***********************************************************************
  InitBlockInterpolation : Initialize block interpolation routines
***********************************************************************/

Void_WMV InitBlockInterpolation (tWMVDecInternalMember * pWMVDec)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(InitBlockInterpolation);

#   ifdef _EMB_WMV3_
#       if defined(macintosh) || defined(_ARM_)
            if ((!g_bSupportAltiVec_WMV) && (!g_bSupportWMMX_WMV))
#       endif
        InitBlockInterpolation_EMB (pWMVDec);
#   endif

    if (g_bSupportWMMX_WMV) {
#ifdef _WMMX_
        pWMVDec->m_pInterpolateBlockBicubic = g_InterpolateBlockBicubic_WMMX;
        pWMVDec->m_pAddError = g_AddError_WMMX;
#endif
    } else {
        pWMVDec->m_pInterpolateBlockBicubic = g_InterpolateBlockBicubic_C;
        pWMVDec->m_pAddError = g_AddError_C;
    }

    pWMVDec->m_pInterpolateBlockBilinear = g_InterpolateBlockBilinear_C;  

    pWMVDec->m_pAddError = g_AddError_C;
	pWMVDec->m_pAddErrorIntra = g_AddErrorIntra_C;

    // Danger - this may need to move someplace???

    pWMVDec->m_pPixelMean = g_PixelMean;
    pWMVDec->m_pOverlapBlockVerticalEdge = g_OverlapBlockVerticalEdge;
    pWMVDec->m_pOverlapBlockHorizontalEdge = g_OverlapBlockHorizontalEdge_Fun;


#   if defined( _WMV_TARGET_X86_) || defined(_Embedded_x86)
        if (g_bSupportMMX_WMV) {
            pWMVDec->m_pInterpolateBlockBicubic = g_InterpolateBlockBicubic_MMX; 
            pWMVDec->m_pInterpolateBlockBilinear = g_InterpolateBlockBilinear_MMX;

            pWMVDec->m_pAddError = g_AddError_MMX;

            pWMVDec->m_pOverlapBlockVerticalEdge = g_OverlapBlockVerticalEdge_MMX; 
            pWMVDec->m_pOverlapBlockHorizontalEdge = g_OverlapBlockHorizontalEdge_MMX; 
            pWMVDec->m_pPixelMean = g_PixelMean_MMX;

        }
#    if !defined(WMV_DISABLE_SSE2)
        if (g_SupportSSE2()) {
            pWMVDec->m_pInterpolateBlockBicubic = g_InterpolateBlockBicubic_SSE2; 
            pWMVDec->m_pInterpolateBlockBilinear = g_InterpolateBlockBilinear_SSE2;
            pWMVDec->m_pAddError = g_AddError_SSE2;

        }
#    endif
#   endif

#   if defined(macintosh) && defined(_MAC_VEC_OPT)
        if (g_bSupportAltiVec_WMV)
        {
            pWMVDec->m_pInterpolateBlockBicubic = g_InterpolateBlockBicubic_AltiVec;
            pWMVDec->m_pInterpolateBlockBilinear = g_InterpolateBlockBilinear_AltiVec;
            pWMVDec->m_pAddError = g_AddError_AltiVec;
        }
#   endif //macintosh

#   ifndef WMV9_SIMPLE_ONLY
#		ifndef PPCWMP
			InitFieldInterpolation (pWMVDec);
#		endif
#   endif

    g_SetupAlignedConsts();
}




/***********************************************************************
  InterpolateBlock : Interpolate reference block 
***********************************************************************/

Void_WMV InterpolateBlock (tWMVDecInternalMember * pWMVDec, const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iDX, I32_WMV iDY, I32_WMV iFilterType)
{
    const U8_WMV *pSrc2 = pSrc + (iDY >> 2) * iSrcStride + (iDX >> 2);
    U8_WMV *pDst2 = pDst;
    I32_WMV iXFrac = iDX & 3;
    I32_WMV iYFrac = iDY & 3;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(InterpolateBlock);

    if (iXFrac == 0 && iYFrac == 0) {
#       if defined(_WMV_TARGET_X86_) || defined(macintosh)
            *(__int64 *) pDst2 = *(__int64 *) pSrc2;
            pDst2 += iDstStride; pSrc2 += iSrcStride;
                    *(__int64 *) pDst2 = *(__int64 *) pSrc2;
            pDst2 += iDstStride; pSrc2 += iSrcStride;
                    *(__int64 *) pDst2 = *(__int64 *) pSrc2;
            pDst2 += iDstStride; pSrc2 += iSrcStride;
                    *(__int64 *) pDst2 = *(__int64 *) pSrc2;
            pDst2 += iDstStride; pSrc2 += iSrcStride;
                    *(__int64 *) pDst2 = *(__int64 *) pSrc2;
            pDst2 += iDstStride; pSrc2 += iSrcStride;
                    *(__int64 *) pDst2 = *(__int64 *) pSrc2;
            pDst2 += iDstStride; pSrc2 += iSrcStride;
                    *(__int64 *) pDst2 = *(__int64 *) pSrc2;
            pDst2 += iDstStride; pSrc2 += iSrcStride;
                   *(__int64 *) pDst2 = *(__int64 *) pSrc2;
#       elif defined(WMV_OPT_COMMON_ARM)
            FASTCPY_8x8(pDst2, pSrc2, iDstStride, iSrcStride);
#       else 
            memcpy(pDst2, pSrc2, 8);
            DEBUG_CACHE_WRITE_BYTES(pDst2,8)  DEBUG_CACHE_READ_BYTES(pSrc2,8);
            pDst2 += iDstStride; pSrc2 += iSrcStride;
            memcpy(pDst2, pSrc2, 8);
            DEBUG_CACHE_WRITE_BYTES(pDst2,8)  DEBUG_CACHE_READ_BYTES(pSrc2,8);
            pDst2 += iDstStride; pSrc2 += iSrcStride;
            memcpy(pDst2, pSrc2, 8);
            DEBUG_CACHE_WRITE_BYTES(pDst2,8)  DEBUG_CACHE_READ_BYTES(pSrc2,8);
            pDst2 += iDstStride; pSrc2 += iSrcStride;
            memcpy(pDst2, pSrc2, 8);
            DEBUG_CACHE_WRITE_BYTES(pDst2,8)  DEBUG_CACHE_READ_BYTES(pSrc2,8);
            pDst2 += iDstStride; pSrc2 += iSrcStride;
            memcpy(pDst2, pSrc2, 8);
            DEBUG_CACHE_WRITE_BYTES(pDst2,8)  DEBUG_CACHE_READ_BYTES(pSrc2,8);
            pDst2 += iDstStride; pSrc2 += iSrcStride;
            memcpy(pDst2, pSrc2, 8);
            DEBUG_CACHE_WRITE_BYTES(pDst2,8)  DEBUG_CACHE_READ_BYTES(pSrc2,8);
            pDst2 += iDstStride; pSrc2 += iSrcStride;
            memcpy(pDst2, pSrc2, 8);
            DEBUG_CACHE_WRITE_BYTES(pDst2,8)  DEBUG_CACHE_READ_BYTES(pSrc2,8);
            pDst2 += iDstStride; pSrc2 += iSrcStride;
            memcpy(pDst2, pSrc2, 8);
            DEBUG_CACHE_WRITE_BYTES(pDst2,8)  DEBUG_CACHE_READ_BYTES(pSrc2,8);
#       endif
    }
    else
    {
        if (iFilterType == FILTER_BICUBIC)
            (*pWMVDec->m_pInterpolateBlockBicubic) (pSrc2, iSrcStride, pDst, iDstStride, iXFrac, iYFrac, pWMVDec->m_iRndCtrl);
        else
            (*pWMVDec->m_pInterpolateBlockBilinear) (pSrc2, iSrcStride, pDst, iDstStride, iXFrac, iYFrac, pWMVDec->m_iRndCtrl);
    }
}


/***********************************************************************
  Interpolation routines : C, MMX, etc versions of interpolation routines
***********************************************************************/

const I16_WMV gaSubsampleBilinear[] = {
    4, 0,       3, 1,
    2, 2,      1, 3,
};

const I16_WMV gaSubsampleBicubic[] = {
    0, 1, 0, 0,     -4, 53, 18, -3,
    -1, 9, 9, -1,   -3, 18, 53, -4
};

#ifndef DISABLE_UNUSED_CODE_INTERPOLATE
#ifndef WMV9_SIMPLE_ONLY
extern const I16_WMV gaSubsampleLong[] = {
    0, 0, 64, 0, 0, 0,      1, -5, 54, 18, -4, 0,//1, -6, 55, 17, -3, 0,//1, -8, 59, 13, -1, 0, //1, -5, 54, 18, -4, 0,
    1, -6, 37, 37, -6, 1,   0, -4, 18, 54, -5, 1,//0, -3, 17, 55, -6, 1,//0, -1, 13, 59, -8, 1, //0, -4, 18, 54, -5, 1
};

/***********************************************************************
  C version of interpolation routines
***********************************************************************/
Void_WMV  g_InterpolateBlock6Tap_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst,
                           I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl)
{
    I32_WMV i, j, k;
    I32_WMV pFilter[13];
    const I16_WMV *pH;
    const I16_WMV *pV;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlock6Tap_C);

    // make iXFrac, iYFrac in range 0...3 (for now, 4 is invalid)
    pSrc += (iYFrac >> 2) * iSrcStride + (iXFrac >> 2);
    iYFrac &= 3;
    iXFrac &= 3;
    iRndCtrl &= 1;

    pH = gaSubsampleLong + 6 * iXFrac;
    pV = gaSubsampleLong + 6 * iYFrac;
    for (j = 0; j < 8; j++) {
        for (i = 0; i < 13; i++) {
            const U8_WMV  *pT = pSrc + (i - 2) * iSrcStride - 2;
            pFilter[i] = pT[0] * pH[0] + pT[1] * pH[1] + pT[2] * pH[2] + pT[3] * pH[3] +
                pT[4] * pH[4] + pT[5] * pH[5];
            DEBUG_CACHE_READ_BYTES(pT,6);
        }
        for (i = 0; i < 8; i++) {
            k = (pFilter[i] * pV[0] + pFilter[1 + i] * pV[1] + pFilter[2 + i] * pV[2]
                + pFilter[3 + i] * pV[3] + pFilter[4 + i] * pV[4]
                + pFilter[5 + i] * pV[5] + (1 << 11) - iRndCtrl) >> 12;
            if (k < 0)
                k = 0;
            else if (k > 255)
                k = 255;
            pDst[iDstStride * i] = (U8_WMV) k;
            DEBUG_CACHE_WRITE_BYTES(pDst + iDstStride * i,1);
        }

        pSrc++;
        pDst++;
    }
}

#endif // WMV9_SIMPLE_ONLY
#endif //DISABLE_UNUSED_CODE_INTERPOLATE


#if 1
Void_WMV  g_InterpolateBlockBilinear_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl)
{
    I32_WMV i, j, k;
    const I16_WMV *pH = gaSubsampleBilinear + 2 * iXFrac;
    const I16_WMV *pV = gaSubsampleBilinear + 2 * iYFrac;
    I32_WMV pFilter[32 + 1];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBilinear_C);

    for (j = 0; j < 8; j++) {
        for (i = 0; i < 9; i++) {
            const U8_WMV  *pT = pSrc + i * iSrcStride;
            pFilter[i] = pT[0] * pH[0] + pT[1] * pH[1];
            DEBUG_CACHE_READ_BYTES(pT,9);
        }
        for (i = 0; i < 8; i++) {
            k = (pFilter[i] * pV[0] + pFilter[1 + i] * pV[1] + 8 - iRndCtrl) >> 4;
            if (k < 0)
                k = 0;
            else if (k > 255)
                k = 255;
            pDst[iDstStride * i] = (U8_WMV) k;
            DEBUG_CACHE_WRITE_BYTES(pDst + iDstStride * i,1);
        }

        pSrc++;
        pDst++;
    }
}
#else

Void_WMV  g_InterpolateVariableBlockBilinear_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, 
                                            I32_WMV iWidth, I32_WMV iHeight, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl)
{
    I32_WMV i, j, k;
    const I16_WMV *pH = gaSubsampleBilinear + 2 * iXFrac;
    const I16_WMV *pV = gaSubsampleBilinear + 2 * iYFrac;
    I32_WMV pFilter[32 + 1];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBilinear_C);

     for (j = 0; j < iHeight; j++) {
        for (i = 0; i < iWidth + 1; i++) {
            const U8_WMV  *pT = pSrc + i * iSrcStride;
            pFilter[i] = pT[0] * pH[0] + pT[1] * pH[1];
            DEBUG_CACHE_READ_BYTES(pT,9);
        }
        for (i = 0; i < iWidth; i++) {
            k = (pFilter[i] * pV[0] + pFilter[1 + i] * pV[1] + 8 - iRndCtrl) >> 4;
            if (k < 0)
                k = 0;
            else if (k > 255)
                k = 255;
            pDst[iDstStride * i] = (U8_WMV) k;
            DEBUG_CACHE_WRITE_BYTES(pDst + iDstStride * i,1);
        }

        pSrc++;
        pDst++;
    }
}

#endif

#if 1

Void_WMV g_InterpolateBlockBicubic_C_vertical (const U8_WMV *pSrc, I32_WMV iSrcStride,
                U8_WMV *pDst, I32_WMV iDstStride, const I16_WMV *pV, I32_WMV iShift, I32_WMV iRound)
{
    I32_WMV i, j, k;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            const U8_WMV  *pT = pSrc + iSrcStride * (i - 1) + j;
            k = (pT[0] * pV[0] + pT[iSrcStride] * pV[1] + pT[2 * iSrcStride] * pV[2]
                + pT[3 * iSrcStride] * pV[3] + iRound) >> iShift;
            if (k < 0)
                k = 0;
            else if (k > 255)
                k = 255;
            pDst[iDstStride * i + j] = (U8_WMV) k;
        }
    }
}

Void_WMV g_InterpolateBlockBicubic_C_horizontal (const U8_WMV *pSrc, I32_WMV iSrcStride,
                U8_WMV *pDst, I32_WMV iDstStride, const I16_WMV *pH, I32_WMV iShift, I32_WMV iRound)
{
    I32_WMV i, j, k;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            const U8_WMV  *pT = pSrc + iSrcStride * i + j - 1;
            k = (pT[0] * pH[0] + pT[1] * pH[1] + pT[2] * pH[2] + pT[3] * pH[3] + 
                iRound) >> iShift;
            if (k < 0)
                k = 0;
            else if (k > 255)
                k = 255;
            pDst[iDstStride * i + j] = (U8_WMV) k;
        }
    }
}

Void_WMV g_InterpolateBlockBicubic_C_VerHor (const U8_WMV *pSrc, I32_WMV iSrcStride,  //zou --->// zou  vc1INTERP_InterpPatchQuarterPelBicubicDiag
                U8_WMV *pDst, I32_WMV iDstStride, const I16_WMV *pH, const I16_WMV *pV, I32_WMV iShift, I32_WMV iRound1, I32_WMV iRound2)
{
    I16_WMV pFilter[8][32+3];
    int i, j, k;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 11; j++) {
            const U8_WMV  *pT = pSrc - iSrcStride - 1 + j;
            pFilter[i][j] = (pT[0] * pV[0] + pT[iSrcStride] * pV[1] + pT[2 * iSrcStride] * pV[2]
                + pT[3 * iSrcStride] * pV[3] + iRound1) >> iShift;

        }
        pSrc += iSrcStride;
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            k = (pFilter[i][j] * pH[0] + pFilter[i][1 + j] * pH[1] + pFilter[i][2 + j] * pH[2]
                + pFilter[i][3 + j] * pH[3] + iRound2) >> 7;
            if (k < 0)
                k = 0;
            else if (k > 255)
                k = 255;
            pDst[j] = (U8_WMV) k;
        }
        pDst += iDstStride;
    }
}

Void_WMV g_InterpolateBlockBicubic_C_Copy (const U8_WMV *pSrc, I32_WMV iSrcStride,
                U8_WMV *pDst, I32_WMV iDstStride)
{
    const U8_WMV  *pS = pSrc;
    U8_WMV  *pD = pDst;
    I32_WMV i;
    for (i = 0; i < 8; i++) {
        memcpy(pD, pS, 8);
        pD += iDstStride;
        pS += iSrcStride;
    }
}

/*static unsigned long long ggaSubsampleBicubic[4] ={0x0000 0000 0001 0000,                 
                                     0xFFFD00120035FFFC,    
                                     0xFFFF00090009FFFF,    
                                     0xFFFC00350012FFFD};  */  

static I16_WMV ggaSubsampleBicubic_2[16] =
{
    0x0000,0x001,0x0000,0x0000,  
    0xFFFC,0x0035,0x0012,0xFFFD,
    0xFFFF,0x0009,0x0009,0xFFFF,
    0xFFFD,0x0012,0x0035,0xFFFC
};

Void_WMV g_InterpolateBlockBicubic_C (const U8_WMV *pSrc, I32_WMV iSrcStride,
                U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl)
{
    const I16_WMV *pH;
    const I16_WMV *pV;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_C);

    //assert (iWidth < 32); // Due to pFilter size, we can only handle widths of <= 32

    //pH = (I16_WMV*) (ggaSubsampleBicubic + iXFrac);
    //pV = (I16_WMV*) (ggaSubsampleBicubic + iYFrac);
     pH = (I16_WMV*) (ggaSubsampleBicubic_2 + 4*iXFrac);
     pV = (I16_WMV*) (ggaSubsampleBicubic_2 + 4*iYFrac);

    if (iXFrac == 0) {
        if (iYFrac == 0) {
            g_InterpolateBlockBicubic_C_Copy (pSrc, iSrcStride, pDst, iDstStride);
        }
        else {
            // vertical filtering only
            I32_WMV iShift = (iYFrac == 2) ? 4 : 6;
            I32_WMV iRound = (1 << (iShift - 1)) - 1 + iRndCtrl;
            g_InterpolateBlockBicubic_C_vertical (pSrc, iSrcStride, pDst, iDstStride, pV, iShift, iRound);
        }
    }

    else if (iYFrac == 0) {
        // horizontal filtering only
        I32_WMV iShift = (iXFrac == 2) ? 4 : 6;
        I32_WMV iRound = (1 << (iShift - 1)) - iRndCtrl;
        g_InterpolateBlockBicubic_C_horizontal (pSrc, iSrcStride, pDst, iDstStride, pH, iShift, iRound);
    }
    else {
        I32_WMV iRound1;
        I32_WMV iRound2;

        I32_WMV iShift = ((iXFrac == 2) ? 4 : 6) + ((iYFrac == 2) ? 4 : 6);
        iShift -= 7;
        iRound1 = (1 << (iShift - 1)) - 1 + iRndCtrl;
        iRound2 = 64 - iRndCtrl;
        g_InterpolateBlockBicubic_C_VerHor (pSrc, iSrcStride, pDst, iDstStride, pH, pV, iShift, iRound1, iRound2);
    }
}

#ifdef _WMMX_
extern Void_WMV g_InterpolateBlockBicubic_WMMX_vertical (const U8_WMV *pSrc, I32_WMV iSrcStride,
                U8_WMV *pDst, I32_WMV iDstStride, const I16_WMV *pV, I32_WMV iShift, I32_WMV iRound);
extern Void_WMV g_InterpolateBlockBicubic_WMMX_horizontal (const U8_WMV *pSrc, I32_WMV iSrcStride,
                U8_WMV *pDst, I32_WMV iDstStride, const I16_WMV *pH, I32_WMV iShift, I32_WMV iRound);
extern Void_WMV g_InterpolateBlockBicubic_WMMX_VerHor (const U8_WMV *pSrc, I32_WMV iSrcStride,
                U8_WMV *pDst, I32_WMV iDstStride, const I16_WMV *pH, const I16_WMV *pV, I32_WMV iShift, I32_WMV iRound1, I32_WMV iRound2);
extern Void_WMV g_InterpolateBlockBicubic_WMMX_Copy (const U8_WMV *pSrc, I32_WMV iSrcStride,
                U8_WMV *pDst, I32_WMV iDstStride);

Void_WMV g_InterpolateBlockBicubic_WMMX (const U8_WMV *pSrc, I32_WMV iSrcStride,
                U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl)
{
    const I16_WMV *pH;
    const I16_WMV *pV;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_C);

    //assert (iWidth < 32); // Due to pFilter size, we can only handle widths of <= 32

    //pH = (I16_WMV*) (ggaSubsampleBicubic + iXFrac);
    //pV = (I16_WMV*) (ggaSubsampleBicubic + iYFrac);
    pH = (I16_WMV*) (ggaSubsampleBicubic_2 + 4*iXFrac);
    pV = (I16_WMV*) (ggaSubsampleBicubic_2 + 4*iYFrac);

    if (iXFrac == 0) {
        if (iYFrac == 0) {
            g_InterpolateBlockBicubic_C_Copy (pSrc, iSrcStride, pDst, iDstStride);
        }
        else {
            // vertical filtering only
            I32_WMV iShift = (iYFrac == 2) ? 4 : 6;
            I32_WMV iRound = (1 << (iShift - 1)) - 1 + iRndCtrl;
            g_InterpolateBlockBicubic_WMMX_vertical (pSrc, iSrcStride, pDst, iDstStride, pV, iShift, iRound);
        }
    }

    else if (iYFrac == 0) {
        // horizontal filtering only
        I32_WMV iShift = (iXFrac == 2) ? 4 : 6;
        I32_WMV iRound = (1 << (iShift - 1)) - iRndCtrl;
        g_InterpolateBlockBicubic_WMMX_horizontal (pSrc, iSrcStride, pDst, iDstStride, pH, iShift, iRound);
    }
    else {
        I32_WMV iRound1;
        I32_WMV iRound2;

        I32_WMV iShift = ((iXFrac == 2) ? 4 : 6) + ((iYFrac == 2) ? 4 : 6);
        iShift -= 7;
        iRound1 = (1 << (iShift - 1)) - 1 + iRndCtrl;
        iRound2 = 64 - iRndCtrl;

        g_InterpolateBlockBicubic_WMMX_VerHor (pSrc, iSrcStride, pDst, iDstStride, pH, pV, iShift, iRound1, iRound2);
    }
}
#endif

#else

/*
static Void_WMV g_InterpolateVariableRefBicubic_C (const U8_WMV *pSrc, I32_WMV iSrcStride,
                U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl)

Void_WMV g_InterpolateBlockBicubic_C (const U8_WMV *pSrc, I32_WMV iSrcStride,
                U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl)
{

    I32_WMV i, j, k;
    const I16_WMV *pH;
    const I16_WMV *pV;
    I16_WMV pFilter[32+3];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_C);

    //assert (iWidth < 32); // Due to pFilter size, we can only handle widths of <= 32

    pH = gaSubsampleBicubic + 4 * iXFrac;
    pV = gaSubsampleBicubic + 4 * iYFrac;

    if (iXFrac == 0) {
        if (iYFrac == 0) {
            const U8_WMV  *pS = pSrc;
            U8_WMV  *pD = pDst;
            for (i = 0; i < iHeight; i++) {
                memcpy(pD, pS, iWidth);
                DEBUG_CACHE_WRITE_BYTES(pD,8);  DEBUG_CACHE_READ_BYTES(pS,8);
                pD += iDstStride;
                pS += iSrcStride;
            }
        }
        else {
            // vertical filtering only
            I32_WMV iShift = (iYFrac == 2) ? 4 : 6;
            I32_WMV iRound = (1 << (iShift - 1)) - 1 + iRndCtrl;
            for (i = 0; i < iHeight; i++) {
                for (j = 0; j < iWidth; j++) {
                    const U8_WMV  *pT = pSrc + iSrcStride * (i - 1) + j;
                    k = (pT[0] * pV[0] + pT[iSrcStride] * pV[1] + pT[2 * iSrcStride] * pV[2]
                        + pT[3 * iSrcStride] * pV[3] + iRound) >> iShift;
                    if (k < 0)
                        k = 0;
                    else if (k > 255)
                        k = 255;
                    pDst[iDstStride * i + j] = (U8_WMV) k;
                    DEBUG_CACHE_READ_BYTES(pT,1);
                    DEBUG_CACHE_READ_BYTES(pT+iSrcStride,1);
                    DEBUG_CACHE_READ_BYTES(pT+2*iSrcStride,1);
                    DEBUG_CACHE_READ_BYTES(pT+3*iSrcStride,1);
                    DEBUG_CACHE_WRITE_BYTES(pDst + iDstStride * i,1);  
                }
            }
        }
    }
    else if (iYFrac == 0) {
        // horizontal filtering only
        I32_WMV iShift = (iXFrac == 2) ? 4 : 6;
        I32_WMV iRound = (1 << (iShift - 1)) - iRndCtrl;
        for (i = 0; i < iHeight; i++) {
            for (j = 0; j < iWidth; j++) {
                const U8_WMV  *pT = pSrc + iSrcStride * i + j - 1;
                k = (pT[0] * pH[0] + pT[1] * pH[1] + pT[2] * pH[2] + pT[3] * pH[3] + 
                    iRound) >> iShift;
                if (k < 0)
                    k = 0;
                else if (k > 255)
                    k = 255;
                pDst[iDstStride * i + j] = (U8_WMV) k;
                DEBUG_CACHE_READ_BYTES(pT,1);
                DEBUG_CACHE_READ_BYTES(pT+iSrcStride,1);
                DEBUG_CACHE_READ_BYTES(pT+2*iSrcStride,1);
                DEBUG_CACHE_READ_BYTES(pT+3*iSrcStride,1);
                DEBUG_CACHE_WRITE_BYTES(pDst + iDstStride * i,1);  
            }
        }
    }
    else {
        I32_WMV iRound1;
        I32_WMV iRound2;

        I32_WMV iShift = ((iXFrac == 2) ? 4 : 6) + ((iYFrac == 2) ? 4 : 6);
        iShift -= 7;
        iRound1 = (1 << (iShift - 1)) - 1 + iRndCtrl;
        iRound2 = 64 - iRndCtrl;

         for (i = 0; i < iHeight; i++) {
            for (j = 0; j < iWidth+3; j++) {
                const U8_WMV  *pT = pSrc - iSrcStride - 1 + j;
                pFilter[j] = (pT[0] * pV[0] + pT[iSrcStride] * pV[1] + pT[2 * iSrcStride] * pV[2]
                    + pT[3 * iSrcStride] * pV[3] + iRound1) >> iShift;
                DEBUG_CACHE_READ_BYTES(pT,1);
                DEBUG_CACHE_READ_BYTES(pT+iSrcStride,1);
                DEBUG_CACHE_READ_BYTES(pT+2*iSrcStride,1);
                DEBUG_CACHE_READ_BYTES(pT+3*iSrcStride,1);
            }
            for (j = 0; j < iWidth; j++) {
                k = (pFilter[j] * pH[0] + pFilter[1 + j] * pH[1] + pFilter[2 + j] * pH[2]
                    + pFilter[3 + j] * pH[3] + iRound2) >> 7;
                if (k < 0)
                    k = 0;
                else if (k > 255)
                    k = 255;
                pDst[j] = (U8_WMV) k;
            }
            DEBUG_CACHE_WRITE_BYTES(pDst,8);  

            pSrc += iSrcStride;
            pDst += iDstStride;
        }
    }
}

static Void_WMV g_Interpolate20x20RefBicubic_C (const U8_WMV *pSrc, I32_WMV iSrcStride,
                U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl)
{
    g_InterpolateVariableRefBicubic_C (pSrc, iSrcStride, pDst, iDstStride, 20, 20, iXFrac, iYFrac, iRndCtrl);
}

Void_WMV  g_InterpolateBlockBicubic_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst,
                                    I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl)
{
    g_InterpolateVariableRefBicubic_C (pSrc, iSrcStride, pDst, iDstStride, 8, 8, iXFrac, iYFrac, iRndCtrl);
}
*/


#endif
/***********************************************************************
  AddError routines: C, MMX, etc versions
***********************************************************************/
#ifndef __MACVIDEO__
#define packFourPixels(x0,x1,x2,x3) ((U32_WMV)x3<<24)|((U32_WMV)x2<<16)|((U32_WMV)x1<<8)|(U32_WMV)x0
#else
#define packFourPixels(x0,x1,x2,x3) ((U32_WMV)x0<<24)|((U32_WMV)x1<<16)|((U32_WMV)x2<<8)|(U32_WMV)x3
#endif   
#ifdef BIG_ENDING 
#define packFourPixels(x0,x1,x2,x3) ((U32_WMV)x0<<24)|((U32_WMV)x1<<16)|((U32_WMV)x2<<8)|(U32_WMV)x3
#endif
#define CLIP(x) ((U32_WMV) x <= 255 ? x : (x < 0 ? 0: 255))

/***********************************************************************
  g_AddError : C version of AddError
***********************************************************************/

Void_WMV g_AddError_C(U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch, U8_WMV* pcClapTabMC)
{
    I32_WMV iy;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_AddError_C);
    
    for (iy = 0; iy < BLOCK_SIZE; iy++) {
        I32_WMV x0 = ppxliError [0] + ppxlcRef [0];
        I32_WMV x1 = ppxliError [1] + ppxlcRef [1];
        I32_WMV x2 = ppxliError [2] + ppxlcRef [2];
        I32_WMV x3 = ppxliError [3] + ppxlcRef [3];
        x0 =CLIP(x0); x1 = CLIP(x1); x2 = CLIP(x2); x3 = CLIP(x3);

        ppxlcDst[0] = x0;
        ppxlcDst[1] = x1;
        ppxlcDst[2] = x2;
        ppxlcDst[3] = x3;

        x0 = ppxliError [4] + ppxlcRef [4];
        x1 = ppxliError [5] + ppxlcRef [5];
        x2 = ppxliError [6] + ppxlcRef [6];
        x3 = ppxliError [7] + ppxlcRef [7];
        x0 =CLIP(x0); x1 = CLIP(x1); x2 = CLIP(x2); x3 = CLIP(x3);

        ppxlcDst[4] = x0;
        ppxlcDst[5] = x1;
        ppxlcDst[6] = x2;
        ppxlcDst[7] = x3;

        DEBUG_CACHE_READ_BYTES(ppxliError,8);
        DEBUG_CACHE_READ_BYTES(ppxlcRef,8);
        DEBUG_CACHE_WRITE_BYTES(ppxlcDst,8);

        ppxlcRef += iPitch;
        ppxlcDst += iPitch;
        ppxliError += BLOCK_SIZE;
    }
  
}

Void_WMV g_AddErrorIntra_C(U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch, U8_WMV* pcClapTabMC)
{
    I32_WMV iy;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_AddError_C);
    
    for (iy = 0; iy < BLOCK_SIZE; iy++) {
        I32_WMV x0 = ppxliError [0] + 128;
        I32_WMV x1 = ppxliError [1] + 128;
        I32_WMV x2 = ppxliError [2] + 128;
        I32_WMV x3 = ppxliError [3] + 128;
        x0 =CLIP(x0); x1 = CLIP(x1); x2 = CLIP(x2); x3 = CLIP(x3);

        ppxlcDst[0] = x0;
        ppxlcDst[1] = x1;
        ppxlcDst[2] = x2;
        ppxlcDst[3] = x3;

        x0 = ppxliError [4] + 128;
        x1 = ppxliError [5] + 128;
        x2 = ppxliError [6] + 128;
        x3 = ppxliError [7] + 128;
        x0 =CLIP(x0); x1 = CLIP(x1); x2 = CLIP(x2); x3 = CLIP(x3);

        ppxlcDst[4] = x0;
        ppxlcDst[5] = x1;
        ppxlcDst[6] = x2;
        ppxlcDst[7] = x3;

        DEBUG_CACHE_READ_BYTES(ppxliError,8);
        DEBUG_CACHE_READ_BYTES(ppxlcRef,8);
        DEBUG_CACHE_WRITE_BYTES(ppxlcDst,8);

        ppxlcRef += iPitch;
        ppxlcDst += iPitch;
        ppxliError += BLOCK_SIZE;
    }
  
}

/***********************************************************************
  GlobalWarp :
  Perform global transformations on reference frame, at frame level
  Ops include luminance scale / shift, global translation and scaling
***********************************************************************/
//#define WARPFILE


#ifdef WMV_OPT_INTENSITYCOMP_ARM
extern Void_WMV IntensityComp (U32_WMV* pScr, I32_WMV iFrameSize, const U8_WMV* pLUT);
#endif



#ifndef WMV9_SIMPLE_ONLY

#ifndef _WMV9AP_
Void_WMV IntensityCompensation (tWMVDecInternalMember * pWMVDec)
{
#ifdef WARPFILE
    {
    FILE  *fRef = fopen ("ref-before", "ab");
    fwrite (pWMVDec->m_ppxliRef0Y, pWMVDec->m_iHeightPrevY, pWMVDec->m_iWidthPrevY, fRef);
    fwrite (pWMVDec->m_ppxliRef0U, pWMVDec->m_iHeightPrevUV, pWMVDec->m_iWidthPrevUV, fRef);
    fwrite (pWMVDec->m_ppxliRef0V, pWMVDec->m_iHeightPrevUV, pWMVDec->m_iWidthPrevUV, fRef);
    fclose (fRef);
    }
#endif
    if (pWMVDec->m_tFrmType == IVOP)
#ifdef WARPFILE
        goto End;
#else
        return;
#endif // WARPFILE
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(IntensityCompensation);

    // this block handles luminance scale / shift
    if (pWMVDec->m_bLuminanceWarp) {
        I32_WMV     i, j, iWidth, iHeight;
        U8_WMV  *pSrcY, *pSrcU, *pSrcV;
        U8_WMV  pLUT[256], pLUTUV[256];
        I32_WMV     iScale, iShift;
        
        if (pWMVDec->m_iLuminanceShift > 31)
            pWMVDec->m_iLuminanceShift -= 64;

        // remap luminance scale and shift
        if (pWMVDec->m_iLuminanceScale == 0) { 
            
            iScale = - 64;
            iShift = 255 * 64 - (pWMVDec->m_iLuminanceShift << 7);
        }
        else {
            iScale = pWMVDec->m_iLuminanceScale  + 32;
                 iShift = pWMVDec->m_iLuminanceShift * 64;

        }

        // printf ("FADE %5.2f %d\t", iScale * 0.01, iShift);

        // build LUTs
        for (i = 0; i < 256; i++) {
            j = (iScale * i + iShift + 32) >> 6;
            if (j > 255)
                j = 255;
            else if (j < 0)
                j = 0;
            pLUT[i] = (U8_WMV) j;

            j = (iScale * (i - 128) + 128 * 64 + 32) >>6;

            if (j > 255)
                j = 255;
            else if (j < 0)
                j = 0;
            pLUTUV[i] = (U8_WMV) j;
        }

        // set up source and destination arrays
        pSrcY = pWMVDec->m_ppxliRef0Y;
        pSrcU = pWMVDec->m_ppxliRef0U;
        pSrcV = pWMVDec->m_ppxliRef0V;

        iWidth = pWMVDec->m_iWidthPrevY;
        iHeight = pWMVDec->m_iHeightPrevY;
       
#ifdef WMV_OPT_INTENSITYCOMP_ARM
        IntensityComp ((U32_WMV*) pSrcY, iHeight * iWidth >> 4, pLUT);
#else
        for(i = 0; i < iHeight * iWidth; i++)
            *pSrcY++ = pLUT[*pSrcY];
#endif

        iWidth  = pWMVDec->m_iWidthPrevUV;
        iHeight = pWMVDec->m_iHeightPrevUV;

#ifdef WMV_OPT_INTENSITYCOMP_ARM
        IntensityComp ((U32_WMV*) pSrcU, iHeight * iWidth >> 4, pLUTUV);
        IntensityComp ((U32_WMV*) pSrcV, iHeight * iWidth >> 4, pLUTUV);
#else
        for (i = 0; i < iHeight * iWidth; i++) {
            *pSrcU++ = pLUTUV[*pSrcU];
            *pSrcV++ = pLUTUV[*pSrcV];
        }
#endif

        
    }
#ifdef WARPFILE
End:
        if (m_bLuminanceWarp) {
    {
    FILE  *fRef = fopen ("ref-after", "ab");
    fwrite (pWMVDec->m_ppxliRef0Y, pWMVDec->m_iHeightPrevY, pWMVDec->m_iWidthPrevY, fRef);
    fwrite (pWMVDec->m_ppxliRef0U, pWMVDec->m_iHeightPrevUV, pWMVDec->m_iWidthPrevUV, fRef);
    fwrite (pWMVDec->m_ppxliRef0V, pWMVDec->m_iHeightPrevUV, pWMVDec->m_iWidthPrevUV, fRef);
    fclose (fRef);
    }
#endif
}

#else
Void_WMV IntensityCompensationFieldMode (tWMVDecInternalMember * pWMVDec, I32_WMV iField, I32_WMV iLuminanceScale, I32_WMV iLuminanceShift)
{
    I32_WMV     i, j;
    U8_WMV  *pSrcY, *pSrcU, *pSrcV;
    U8_WMV  pLUT[256], pLUTUV[256];
    I32_WMV     iScale, iShift;
    if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP)
        return;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(IntensityCompensation);

    // this block handles luminance scale / shift

     if (iLuminanceShift > 31)
        iLuminanceShift -= 64;

    // remap luminance scale and shift
    if (iLuminanceScale == 0) { 
        iScale = - 64;
        iShift = 255 * 64 - iLuminanceShift * 2 * 64;
    }
    else {
        iScale = iLuminanceScale + 32;
        iShift = iLuminanceShift * 64;
    }

        // printf ("FADE %5.2f %d\t", iScale * 0.01, iShift);

        // build LUTs
        for (i = 0; i < 256; i++) {
            j = (iScale * i + iShift + 32) >> 6;
            if (j > 255)
                j = 255;
            else if (j < 0)
                j = 0;
            pLUT[i] = (U8_WMV) j;

            j = (iScale * (i - 128) + 128 * 64 + 32) >>6;

            if (j > 255)
                j = 255;
            else if (j < 0)
                j = 0;
            pLUTUV[i] = (U8_WMV) j;
        }

    if (pWMVDec->m_iCurrentTemporalField == 0) {
        pSrcY = pWMVDec->m_ppxliRef0Y + (iField - 4) * (pWMVDec->m_iWidthPrevY >> 1);
        pSrcU = pWMVDec->m_ppxliRef0U + (iField - 2) * (pWMVDec->m_iWidthPrevUV >> 1);
        pSrcV = pWMVDec->m_ppxliRef0V + (iField - 2) * (pWMVDec->m_iWidthPrevUV >> 1);
    }
    else {
                if (pWMVDec->m_bTopFieldFirst) {
        if (iField == 0) {
                pSrcY = pWMVDec->m_ppxliCurrQPlusExpY0 - pWMVDec->m_iWidthPrevYXExpPlusExp - 4 * (pWMVDec->m_iWidthPrevY >> 1);
                pSrcU = pWMVDec->m_ppxliCurrQPlusExpU0 - pWMVDec->m_iWidthPrevUVXExpPlusExp - 2 * (pWMVDec->m_iWidthPrevUV >> 1);
                pSrcV = pWMVDec->m_ppxliCurrQPlusExpV0 - pWMVDec->m_iWidthPrevUVXExpPlusExp - 2 * (pWMVDec->m_iWidthPrevUV >> 1);
        }
        else {
                pSrcY = pWMVDec->m_ppxliRef0Y - 3 * (pWMVDec->m_iWidthPrevY >> 1);
                pSrcU = pWMVDec->m_ppxliRef0U - (pWMVDec->m_iWidthPrevUV >> 1);
                pSrcV = pWMVDec->m_ppxliRef0V - (pWMVDec->m_iWidthPrevUV >> 1);
        }
    }
        else { // Bottom field first
            if (iField == 0) {
                pSrcY = pWMVDec->m_ppxliRef0Y - 4 * (pWMVDec->m_iWidthPrevY >> 1);
                pSrcU = pWMVDec->m_ppxliRef0U - 2 * (pWMVDec->m_iWidthPrevUV >> 1);
                pSrcV = pWMVDec->m_ppxliRef0V - 2 * (pWMVDec->m_iWidthPrevUV >> 1);
            }
            else {
                pSrcY = pWMVDec->m_ppxliCurrQPlusExpY0 - pWMVDec->m_iWidthPrevYXExpPlusExp - 3 * (pWMVDec->m_iWidthPrevY >> 1);
                pSrcU = pWMVDec->m_ppxliCurrQPlusExpU0 - pWMVDec->m_iWidthPrevUVXExpPlusExp - (pWMVDec->m_iWidthPrevUV >> 1);
                pSrcV = pWMVDec->m_ppxliCurrQPlusExpV0 - pWMVDec->m_iWidthPrevUVXExpPlusExp - (pWMVDec->m_iWidthPrevUV >> 1);
           }
        }
    }


    for (i = 0; i < pWMVDec->m_iHeightY + 36; i++) {
        for (j = 0; j < (pWMVDec->m_iWidthPrevY >> 1); j++)
            pSrcY[j] = pLUT[pSrcY[j]];
        pSrcY += pWMVDec->m_iWidthPrevY;
        
    }

    for (i = 0; i < pWMVDec->m_iHeightUV + 18; i++) {
        for (j = 0; j < (pWMVDec->m_iWidthPrevUV >> 1); j++) {
            pSrcU[j] = pLUTUV[pSrcU[j]];
            pSrcV[j] = pLUTUV[pSrcV[j]];
        }
        pSrcU += pWMVDec->m_iWidthPrevUV;
        pSrcV += pWMVDec->m_iWidthPrevUV;
        
    }
        
}


Void_WMV IntensityCompensation (tWMVDecInternalMember * pWMVDec)
{

    if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP)
        return;

    // this block handles luminance scale / shift
    if (pWMVDec->m_bLuminanceWarp) {
        I32_WMV     iScale, iShift;

		U8_WMV pLUT[256];
		U8_WMV pLUTUV[256];
		memset(pLUT, 0, 256);
		memset(pLUTUV, 0, 256);

	    if (pWMVDec->m_iLuminanceShift > 31)
            pWMVDec->m_iLuminanceShift -= 64;


        // remap luminance scale and shift
        if (pWMVDec->m_iLuminanceScale == 0) { 
            
            iScale = - 64;
            iShift = 255 * 64 - pWMVDec->m_iLuminanceShift * 2 * 64;
        }
        else {
            iScale = pWMVDec->m_iLuminanceScale  + 32;
                 iShift = pWMVDec->m_iLuminanceShift * 64;

        }
     
#if defined (ARM_C) || defined (_win32_) //sw test
	   {
           U8_WMV *pSrcY = NULL;
            U8_WMV *pSrcU = NULL;
            U8_WMV *pSrcV = NULL;
            I32_WMV iWidth = 0;
            I32_WMV iHeight = 0;
		   int  i, j;
		   for (i = 0; i < 256; i++) {
				j = (iScale * i + iShift + 32) >> 6;
				if (j > 255)
					j = 255;
				else if (j < 0)
					j = 0;
				pLUT[i] = (U8_WMV) j;

				j = (iScale * (i - 128) + 128 * 64 + 32) >>6;

				if (j > 255)
					j = 255;
				else if (j < 0)
					j = 0;
				pLUTUV[i] = (U8_WMV) j;
			}

			// set up source and destination arrays
			pSrcY = pWMVDec->m_ppxliRef0Y;
			pSrcU = pWMVDec->m_ppxliRef0U;
			pSrcV = pWMVDec->m_ppxliRef0V;

			iWidth = pWMVDec->m_iWidthPrevY;
			iHeight = pWMVDec->m_iHeightPrevY;
			for(i = 0; i < iHeight * iWidth; i++)
				*pSrcY++ = pLUT[*pSrcY];


			iWidth  = pWMVDec->m_iWidthPrevUV;
			iHeight = pWMVDec->m_iHeightPrevUV;

			for (i = 0; i < iHeight * iWidth; i++) {
				*pSrcU++ = pLUTUV[*pSrcU];
				*pSrcV++ = pLUTUV[*pSrcV];            
			}
	   }
#elif defined (_ARM_) 
        IntensityCompensation_asm_Fun(pWMVDec->m_ppxliRef0Y, pWMVDec->m_ppxliRef0U, pWMVDec->m_ppxliRef0V, 
        pWMVDec->m_iWidthPrevY * pWMVDec->m_iHeightPrevY, pWMVDec->m_iWidthPrevUV * pWMVDec->m_iHeightPrevUV, iScale, iShift);
#endif   
    }
    
}

#endif
//#ifdef _EMB_3FRAMES_

Void_WMV IntensityCompensation2AltRef (tWMVDecInternalMember * pWMVDec)
{

    if (pWMVDec->m_tFrmType == IVOP)
        return;
        
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(IntensityCompensation);

    // this block handles luminance scale / shift
    if (pWMVDec->m_bLuminanceWarp) {        
        I32_WMV     i, j, iWidth, iHeight;
        U8_WMV  *pSrcY, *pSrcU, *pSrcV;
        U8_WMV  *pDestY, *pDestU, *pDestV;
        U8_WMV  pLUT[256], pLUTUV[256];
        I32_WMV     iScale, iShift;

        if (pWMVDec->m_iLuminanceShift > 31)
            pWMVDec->m_iLuminanceShift -= 64;


        // remap luminance scale and shift
        if (pWMVDec->m_iLuminanceScale == 0) { 
            
            iScale = - 64;
            iShift = 255 * 64 - (pWMVDec->m_iLuminanceShift << 7);
        }
        else {
            iScale = pWMVDec->m_iLuminanceScale  + 32;
                 iShift = pWMVDec->m_iLuminanceShift * 64;

        }

        // printf ("FADE %5.2f %d\t", iScale * 0.01, iShift);

        // build LUTs
        for (i = 0; i < 256; i++) {
            j = (iScale * i + iShift + 32) >> 6;
            if (j > 255)
                j = 255;
            else if (j < 0)
                j = 0;
            pLUT[i] = (U8_WMV) j;

            j = (iScale * (i - 128) + 128 * 64 + 32) >>6;

            if (j > 255)
                j = 255;
            else if (j < 0)
                j = 0;
            pLUTUV[i] = (U8_WMV) j;
        }

        // set up source and destination arrays

        if(pWMVDec->m_bRef0InRefAlt == TRUE_WMV)
        {
            pSrcY = pWMVDec->m_pfrmRef0AltQ->m_pucYPlane;
            pSrcU = pWMVDec->m_pfrmRef0AltQ->m_pucUPlane;
            pSrcV = pWMVDec->m_pfrmRef0AltQ->m_pucVPlane;
        }
        else
        {
            pSrcY = pWMVDec->m_ppxliRef0Y;
            pSrcU = pWMVDec->m_ppxliRef0U;
            pSrcV = pWMVDec->m_ppxliRef0V;
            pWMVDec->m_bRef0InRefAlt = TRUE_WMV;
        }

        pDestY = pWMVDec->m_pfrmRef0AltQ->m_pucYPlane;
        pDestU = pWMVDec->m_pfrmRef0AltQ->m_pucUPlane;
        pDestV = pWMVDec->m_pfrmRef0AltQ->m_pucVPlane;

        iWidth = pWMVDec->m_iWidthPrevY;
        iHeight = pWMVDec->m_iHeightPrevY;
       

        for(i = 0; i < iHeight * iWidth; i++)
            *pDestY++ = pLUT[*pSrcY++];


        iWidth  = pWMVDec->m_iWidthPrevUV;
        iHeight = pWMVDec->m_iHeightPrevUV;

        for (i = 0; i < iHeight * iWidth; i++) {
            *pDestU++ = pLUTUV[*pSrcU++];
            *pDestV++ = pLUTUV[*pSrcV++];
        }

        
    }

}

//#endif

Void_WMV InterpolateYMB_X9 (tWMVDecInternalMember * pWMVDec, U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst,
                           I32_WMV iDstStride, I32_WMV iX, I32_WMV iY,
                           I32_WMV iFilterType, I32_WMV iRndCtrl)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(InterpolateYMB_X9);
    // block 0            
    InterpolateBlock (pWMVDec, pSrc, iSrcStride, pDst, iDstStride, iX, iY, iFilterType);

    // block 1 current
    pSrc += 8;
    InterpolateBlock (pWMVDec, pSrc, iSrcStride, pDst + 8, iDstStride, iX, iY, iFilterType);

    // block 3 reference
    pSrc += iSrcStride * 8;
    InterpolateBlock (pWMVDec, pSrc, iSrcStride, pDst + 8*iDstStride + 8, iDstStride, iX, iY, iFilterType);
           
    // block 2 reference
    pSrc -= 8;
    InterpolateBlock (pWMVDec, pSrc, iSrcStride, pDst + 8*iDstStride, iDstStride, iX, iY, iFilterType);
}
#endif //WMV9_SIMPLE_ONLY

/****************************************************************************************
  OverlapBlockVerticalEdge : overlap vertical block edge

  Filter coefficients:
  Forward
  inner pair: [7 37 -5 -7] // 32
  outer pair: [37 0 0 -5] // 32
  Inverse
  inner pair: [-2 14 2 2] // 8
  outer pair: [14 0 0 2]  // 8
****************************************************************************************/
#ifndef WMV_OPT_LOOPFILTER_ARM

#define OUTER_1 ((7 * v0 + v3 + iRnd + 3) >> 3)
#define OUTER_2 ((7 * v3 + v0 + 4 - iRnd) >> 3)
#define INNER_1 ((7 * v1 - v0 + v2 + v3 + 4 - iRnd) >> 3)
#define INNER_2 ((7 * v2 - v3 + v1 + v0 + iRnd + 3) >> 3)

Void_WMV g_OverlapBlockVerticalEdge (I16_WMV *pInOut, I32_WMV iStride)
{
    I32_WMV iRnd = 1;
    I32_WMV ii ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_OverlapBlockVerticalEdge);

    for ( ii = 0; ii < 8; ii++) {
        I16_WMV v0 = pInOut[-2];
        I16_WMV v1 = pInOut[-1];
        I16_WMV v2 = pInOut[0];
        I16_WMV v3 = pInOut[1];

        pInOut[-2] = OUTER_1;
        pInOut[-1] = INNER_1;
        pInOut[0] = INNER_2;
        pInOut[1] = OUTER_2;

        iRnd ^= 1;

        DEBUG_CACHE_READ_BYTES(pInOut-2,4);
        
        pInOut += iStride;
    }
}
/****************************************************************************************
  OverlapBlockHorizontalEdge : overlap horizontal block edge

  Filter coefficients:
  Forward
  inner pair: [7 37 -5 -7] // 32
  outer pair: [37 0 0 -5] // 32
  Inverse
  inner pair: [-2 14 2 2] // 8
  outer pair: [14 0 0 2]  // 8
****************************************************************************************/
const I32_WMV g_iOverlapOffset = 128;
Void_WMV g_OverlapBlockHorizontalEdge ( I16_WMV *pSrcTop, I16_WMV *pSrcCurr,
                                                 I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,
                                                 Bool_WMV bTop, Bool_WMV bCurrent, Bool_WMV bWindup)
{
    I32_WMV  iRnd = 1;
    I32_WMV i ;
    I32_WMV ii, jj;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_OverlapBlockHorizontalEdge);

    pSrcTop += 6 * iSrcStride;

    if (bTop && bCurrent) {
        for ( i = 0; i < 8; i++) {
            I16_WMV v0 = pSrcTop[i];
            I16_WMV v1 = pSrcTop[i + iSrcStride];
            I16_WMV v2 = pSrcCurr[i];
            I16_WMV v3 = pSrcCurr[i + iSrcStride];

            I32_WMV k = OUTER_1 + g_iOverlapOffset;
            pDst[i - 2 * iDstStride] = (k < 0) ? 0 : ((k > 255) ? 255 : k);
            k = OUTER_2 + g_iOverlapOffset;
            pDst[i + iDstStride] = (k < 0) ? 0 : ((k > 255) ? 255 : k);

            k = INNER_1 + g_iOverlapOffset;
            pDst[i - iDstStride] = (k < 0) ? 0 : ((k > 255) ? 255 : k);
            k = INNER_2 + g_iOverlapOffset;
            pDst[i] = (k < 0) ? 0 : ((k > 255) ? 255 : k);

            iRnd ^= 1;

        }
        DEBUG_CACHE_READ_BYTES(pSrcTop,8);
        DEBUG_CACHE_READ_BYTES(pSrcTop+iSrcStride,8);
        DEBUG_CACHE_READ_BYTES(pSrcCurr,8);
        DEBUG_CACHE_READ_BYTES(pSrcCurr+iSrcStride,8);
        DEBUG_CACHE_WRITE_BYTES(pDst - 2 * iDstStride,8);
        DEBUG_CACHE_WRITE_BYTES(pDst + iDstStride,8);
        DEBUG_CACHE_WRITE_BYTES(pDst - iDstStride,8);
        DEBUG_CACHE_WRITE_BYTES(pDst,8);

        pSrcCurr += 2 * iSrcStride;
        pDst += 2 * iDstStride;

        if (!bWindup) {
            for (i = 0; i < 4; i++) {
                for ( jj = 0; jj < 8; jj++) {
                    I32_WMV k = pSrcCurr[jj] + g_iOverlapOffset;
                    pDst[jj] = (k < 0) ? 0 : ((k > 255) ? 255 : k);
                }
                DEBUG_CACHE_READ_BYTES(pSrcCurr,8);
                DEBUG_CACHE_WRITE_BYTES(pDst,8);
                pSrcCurr += iSrcStride;
                pDst += iDstStride;
            }
        }
    }
    // remaining 2 of past
    else if (bTop) {
        pDst -= 2 * iDstStride;
        for ( ii = 0; ii < 2; ii++) {
            for ( jj = 0; jj < 8; jj++) {
                I32_WMV k = pSrcTop[jj] + g_iOverlapOffset;
                pDst[jj] = (k < 0) ? 0 : ((k > 255) ? 255 : k);
            }
            if(ii == 0)
            {
                pSrcTop += iSrcStride;
                pDst += iDstStride;
            }
        }
    }
    // remaining 6 of current
    else if (bCurrent) {
        I32_WMV ii;
        for ( ii = 0; ii < (bWindup ? 2 : 6); ii++) {
            for ( jj = 0; jj < 8; jj++) {
                I32_WMV k = pSrcCurr[jj] + g_iOverlapOffset;
                pDst[jj] = (k < 0) ? 0 : ((k > 255) ? 255 : k);
            }
            DEBUG_CACHE_READ_BYTES(pSrcCurr,8);
            DEBUG_CACHE_WRITE_BYTES(pDst,8);
            pSrcCurr += iSrcStride;
            pDst += iDstStride;
        }
    }
}
#endif //WMV_OPT_LOOPFILTER_ARM

/****************************************************************************************
  OverlapMBRow : complete overlapped transform reconstruction for I blocks in row of MBs
  // TBD: speedup (e.g. skip if no intra blocks in current and previous rows)
****************************************************************************************/
#ifndef _WMV9AP_
Void_WMV OverlapMBRow (tWMVDecInternalMember * pWMVDec, I32_WMV imbY, U8_WMV *ppxliRecnY, U8_WMV *ppxliRecnU,
                                   U8_WMV *ppxliRecnV, I32_WMV iMBYEnd, I32_WMV iMBYStart,
                                   I32_WMV iThreadID)
{
#ifdef XBOXGPU
    tGPUDecInternalMember* pGPUDec=g_getGpuDecoder(pWMVDec);
    tGPUInernalFrameMember* pGPUFrame=g_getCPUFrame(pGPUDec);
#endif    
    I32_WMV  ii, iXBlocks = (I32_WMV) pWMVDec->m_uintNumMBX << 1;
    I32_WMV  iStrideUV = (I32_WMV) pWMVDec->m_uintNumMBX << 3;
    I32_WMV  iStride = iStrideUV << 1;
    I32_WMV  iBlockIndex = imbY * iXBlocks << 1;
    I16_WMV  *pMotion = pWMVDec->m_pXMotion + iBlockIndex;
    I16_WMV  *pMotionUV = pWMVDec->m_pXMotionC + imbY * (I32_WMV) pWMVDec->m_uintNumMBX;
    I16_WMV  **pIntraBlockRow, **pIntraMBRowU, **pIntraMBRowV;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(OverlapMBRow);

    pIntraBlockRow = pWMVDec->m_pIntraBlockRow0;
    pIntraMBRowU = pWMVDec->m_pIntraMBRowU0;
    pIntraMBRowV = pWMVDec->m_pIntraMBRowV0;

    if (iThreadID) {
        pIntraBlockRow = pWMVDec->m_pIntraBlockRow1;
        pIntraMBRowU = pWMVDec->m_pIntraMBRowU1;
        pIntraMBRowV = pWMVDec->m_pIntraMBRowV1;
    }

    // as long as there are I blocks in the current rows, filter vertical edges
    if (imbY < iMBYEnd) {
        for (ii = 1; ii < iXBlocks; ii++) {
            if (pMotion[ii] == IBLOCKMV && pMotion[ii - 1] == IBLOCKMV) {
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraBlockRow[0] + (ii << 3), iStride);
            }
            if (pMotion[ii + iXBlocks] == IBLOCKMV && pMotion[ii + iXBlocks - 1] == IBLOCKMV) {
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraBlockRow[2] + (ii << 3), iStride);
            }
        }
        for (ii = 1; ii < (I32_WMV) pWMVDec->m_uintNumMBX; ii++) {
            if (pMotionUV[ii] == IBLOCKMV && pMotionUV[ii - 1] == IBLOCKMV) {
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraMBRowU[0] + (ii << 3), iStrideUV);
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraMBRowV[0] + (ii << 3), iStrideUV);
            }
        }
    }

    for (ii = 0; ii < iXBlocks; ii++) {
        Bool_WMV  bTop = (imbY > iMBYStart) && pMotion[ii - iXBlocks] == IBLOCKMV;
        Bool_WMV  bDn  = (imbY < iMBYEnd), bUp = FALSE;
        if (bDn) {
            bUp = pMotion[ii] == IBLOCKMV;
            bDn = pMotion[ii + iXBlocks] == IBLOCKMV;
        }

        if  (bTop || bUp) {
#ifdef XBOXGPU
            if(!pWMVDec->m_bXBOXUsingGPU)
#endif                
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[1] + (ii << 3), pIntraBlockRow[0] + (ii << 3),
                iStride, ppxliRecnY + (ii << 3), pWMVDec->m_iWidthPrevY, bTop, bUp, FALSE);
#ifdef XBOXGPU
            else
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[1] + (ii << 3), pIntraBlockRow[0] + (ii << 3),
                iStride, ppxliRecnY + (ii << 3), pGPUDec->m_iIPitchY, bTop, bUp, FALSE);            
#endif
            
        }
        if  (bUp || bDn) {
#ifdef XBOXGPU
            if(!pWMVDec->m_bXBOXUsingGPU)
#endif            
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[0] + (ii << 3), pIntraBlockRow[2] + (ii << 3),
                iStride, ppxliRecnY + ((pWMVDec->m_iWidthPrevY + ii) << 3), pWMVDec->m_iWidthPrevY, bUp, bDn, FALSE);
#ifdef XBOXGPU
            else
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[0] + (ii << 3), pIntraBlockRow[2] + (ii << 3),
                iStride, ppxliRecnY + ((pGPUDec->m_iIPitchY + ii) << 3), pGPUDec->m_iIPitchY, bUp, bDn, FALSE);
#endif            
        }
    }
    for (ii = 0; ii < (I32_WMV) pWMVDec->m_uintNumMBX; ii++) {
        Bool_WMV  bUp = (imbY > iMBYStart) && (pMotionUV[ii - (I32_WMV) pWMVDec->m_uintNumMBX] == IBLOCKMV);
        Bool_WMV  bDn = (imbY < iMBYEnd) && (pMotionUV[ii] == IBLOCKMV);

        if  (bUp || bDn) {
#ifdef XBOXGPU
            if(!pWMVDec->m_bXBOXUsingGPU){
                
#endif            
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraMBRowU[1] + (ii << 3), pIntraMBRowU[0] + (ii << 3),
                iStrideUV, ppxliRecnU + (ii << 3), pWMVDec->m_iWidthPrevUV, bUp, bDn, FALSE);
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraMBRowV[1] + (ii << 3), pIntraMBRowV[0] + (ii << 3),
                iStrideUV, ppxliRecnV + (ii << 3), pWMVDec->m_iWidthPrevUV, bUp, bDn, FALSE);
#ifdef XBOXGPU
            }
            else
            {
                
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraMBRowU[1] + (ii << 3), pIntraMBRowU[0] + (ii << 3),
                iStrideUV, ppxliRecnU + (ii << 3), pGPUDec->m_iIPitchUV, bUp, bDn, FALSE);
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraMBRowV[1] + (ii << 3), pIntraMBRowV[0] + (ii << 3),
                iStrideUV, ppxliRecnV + (ii << 3), pGPUDec->m_iIPitchUV, bUp, bDn, FALSE);
            }
#endif
        }
    }
    
    // switch pointers - use pMotion as a temp
    if (iThreadID == 0) {
        pMotion = pWMVDec->m_pIntraBlockRow0[1];
        pWMVDec->m_pIntraBlockRow0[1] = pWMVDec->m_pIntraBlockRow0[2];
        pWMVDec->m_pIntraBlockRow0[2] = pMotion;

        pMotion = pWMVDec->m_pIntraMBRowU0[0];
        pWMVDec->m_pIntraMBRowU0[0] = pWMVDec->m_pIntraMBRowU0[1];
        pWMVDec->m_pIntraMBRowU0[1] = pMotion;

        pMotion = pWMVDec->m_pIntraMBRowV0[0];
        pWMVDec->m_pIntraMBRowV0[0] = pWMVDec->m_pIntraMBRowV0[1];
        pWMVDec->m_pIntraMBRowV0[1] = pMotion;

#ifdef _EMB_WMV3_
#if defined(macintosh) || defined(_ARM_)
    if ((!g_bSupportAltiVec_WMV) && (!g_bSupportWMMX_WMV))
#endif
    {
        EMB_PBMainLoop  * pPBMainLoop = &pWMVDec->m_EMB_PMainLoop.PB;
        pPBMainLoop->m_pIntraBlockRow[0] = pPBMainLoop->m_pIntraBlockRow[1]  = pWMVDec->m_pIntraBlockRow0[0];
        pPBMainLoop->m_pIntraBlockRow[2] = pPBMainLoop->m_pIntraBlockRow[3]  = pWMVDec->m_pIntraBlockRow0[2];
        pPBMainLoop->m_pIntraBlockRow[4]  = pWMVDec->m_pIntraMBRowU0[0];
        pPBMainLoop->m_pIntraBlockRow[5]  = pWMVDec->m_pIntraMBRowV0[0];
    }
#endif
    }
    else {
        if (imbY == iMBYStart) {
            // move block row 0 to backup area 3
            pMotion = pWMVDec->m_pIntraBlockRow1[3];
            pWMVDec->m_pIntraBlockRow1[3] = pWMVDec->m_pIntraBlockRow1[0];
            pWMVDec->m_pIntraBlockRow1[0] = pMotion;

            // copy to backup for UV
            ALIGNED32_FASTMEMCPY (pWMVDec->m_pIntraMBRowU1[2], pWMVDec->m_pIntraMBRowU1[0], pWMVDec->m_uintNumMBX << 6);
            ALIGNED32_FASTMEMCPY (pWMVDec->m_pIntraMBRowV1[2], pWMVDec->m_pIntraMBRowV1[0], pWMVDec->m_uintNumMBX << 6);
        }
        pMotion = pWMVDec->m_pIntraBlockRow1[1];
        pWMVDec->m_pIntraBlockRow1[1] = pWMVDec->m_pIntraBlockRow1[2];
        pWMVDec->m_pIntraBlockRow1[2] = pMotion;

        pMotion = pWMVDec->m_pIntraMBRowU1[0];
        pWMVDec->m_pIntraMBRowU1[0] = pWMVDec->m_pIntraMBRowU1[1];
        pWMVDec->m_pIntraMBRowU1[1] = pMotion;

        pMotion = pWMVDec->m_pIntraMBRowV1[0];
        pWMVDec->m_pIntraMBRowV1[0] = pWMVDec->m_pIntraMBRowV1[1];
        pWMVDec->m_pIntraMBRowV1[1] = pMotion;
    }
}
#else

Void_WMV OverlapMBRow (tWMVDecInternalMember * pWMVDec, I32_WMV imbY, U8_WMV *ppxliRecnY, U8_WMV *ppxliRecnU,
                      U8_WMV *ppxliRecnV, Bool_WMV bTopRowOfSlice, Bool_WMV bBottomRowOfSlice)
{
    I32_WMV  ii, iXBlocks = (I32_WMV) pWMVDec->m_uintNumMBX * 2;
    I32_WMV  iStrideUV = (I32_WMV) pWMVDec->m_uintNumMBX * 8;
    I32_WMV  iStride = iStrideUV * 2;
    I32_WMV  iBlockIndex ;
    I16_WMV  *pMotion;
    I16_WMV  *pMotionUV;
    I16_WMV  **pIntraBlockRow, **pIntraMBRowU, **pIntraMBRowV;



    imbY += (bBottomRowOfSlice != FALSE);

    iBlockIndex = imbY * iXBlocks * 2;
    pMotion = pWMVDec->m_pXMotion + iBlockIndex;
    pMotionUV = pWMVDec->m_pXMotionC + imbY * (I32_WMV) pWMVDec->m_uintNumMBX;
	
	pIntraBlockRow = pWMVDec->m_pIntraBlockRow0;
	pIntraMBRowU = pWMVDec->m_pIntraMBRowU0;
	pIntraMBRowV = pWMVDec->m_pIntraMBRowV0;



    // as long as there are I blocks in the current rows, filter vertical edges
    if (!bBottomRowOfSlice) {
        for (ii = 0; ii < iXBlocks; ii++) {
            if (ii && pMotion[ii] == IBLOCKMV && pMotion[ii - 1] == IBLOCKMV) {
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraBlockRow[0] + ii * 8, iStride);
            }
            if (ii && pMotion[ii + iXBlocks] == IBLOCKMV && pMotion[ii + iXBlocks - 1] == IBLOCKMV) {
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraBlockRow[2] + ii * 8, iStride);
            }
        }
        for (ii = 0; ii < (I32_WMV) pWMVDec->m_uintNumMBX; ii++) {
            if (ii && pMotionUV[ii] == IBLOCKMV && pMotionUV[ii - 1] == IBLOCKMV) {
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraMBRowU[0] + ii * 8, iStrideUV);
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraMBRowV[0] + ii * 8, iStrideUV);
            }
        }
    }

    for (ii = 0; ii < iXBlocks; ii++) {
        Bool_WMV  bTop = (!bTopRowOfSlice) && pMotion[ii - iXBlocks] == IBLOCKMV;
        Bool_WMV  bDn  = (!bBottomRowOfSlice), bUp = FALSE;
        if (bDn) {
            bUp = pMotion[ii] == IBLOCKMV;
            bDn = pMotion[ii + iXBlocks] == IBLOCKMV;
        }

        if  (bTop || bUp) {
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[1] + ii * 8, pIntraBlockRow[0] + ii * 8,
                iStride, ppxliRecnY + ii * 8, pWMVDec->m_iWidthPrevY, bTop, bUp, FALSE);
        }

        if  (bUp || bDn) {
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[0] + ii * 8, pIntraBlockRow[2] + ii * 8,
                iStride, ppxliRecnY + pWMVDec->m_iWidthPrevY * 8 + ii * 8, pWMVDec->m_iWidthPrevY, bUp, bDn, FALSE);
        }
    }
    for (ii = 0; ii < (I32_WMV) pWMVDec->m_uintNumMBX; ii++) {
        Bool_WMV  bUp = (!bTopRowOfSlice) && (pMotionUV[ii - (I32_WMV) pWMVDec->m_uintNumMBX] == IBLOCKMV);
        Bool_WMV  bDn = (!bBottomRowOfSlice) && (pMotionUV[ii] == IBLOCKMV);
        if  (bUp || bDn) {
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraMBRowU[1] + ii * 8, pIntraMBRowU[0] + ii * 8,
                iStrideUV, ppxliRecnU + ii * 8, pWMVDec->m_iWidthPrevUV, bUp, bDn, FALSE);
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraMBRowV[1] + ii * 8, pIntraMBRowV[0] + ii * 8,
                iStrideUV, ppxliRecnV + ii * 8, pWMVDec->m_iWidthPrevUV, bUp, bDn, FALSE);
        }
    }
    
    // switch pointers - use pMotion as a temp

		pMotion = pWMVDec->m_pIntraBlockRow0[1];
		pWMVDec->m_pIntraBlockRow0[1] = pWMVDec->m_pIntraBlockRow0[2];
		pWMVDec->m_pIntraBlockRow0[2] = pMotion;

		pMotion = pWMVDec->m_pIntraMBRowU0[0];
		pWMVDec->m_pIntraMBRowU0[0] = pWMVDec->m_pIntraMBRowU0[1];
		pWMVDec->m_pIntraMBRowU0[1] = pMotion;

		pMotion = pWMVDec->m_pIntraMBRowV0[0];
		pWMVDec->m_pIntraMBRowV0[0] = pWMVDec->m_pIntraMBRowV0[1];
		pWMVDec->m_pIntraMBRowV0[1] = pMotion;

}


#endif

/****************************************************************************************
  PixelMean : compute the "mean" of two arrays - width and height are multiples of 8
****************************************************************************************/

Void_WMV g_PixelMean (const U8_WMV *pSrc1, I32_WMV iStride1,
                                const U8_WMV *pSrc2, I32_WMV iStride2,
                                U8_WMV *pDst, I32_WMV iDstStride,
                                I32_WMV   iWidth, I32_WMV iHeight)
{
    I32_WMV i,j;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_PixelMean);
    for ( i = 0; i < iHeight; i++) {
        for ( j = 0; j < iWidth; j++) {
          I32_WMV tmp = ((I32_WMV)pSrc1[j] + (I32_WMV)pSrc2[j] + 1) >> 1;
            pDst[j] = (U8_WMV) tmp;
        }
        pSrc1 += iStride1;
        pSrc2 += iStride2;
        pDst += iDstStride;
    }
}

Void_WMV DirectModeMV (tWMVDecInternalMember * pWMVDec, I32_WMV iXMotion, I32_WMV iYMotion, Bool_WMV bHpelMode,
                                   I32_WMV imbX, I32_WMV imbY,
                                   I32_WMV *idfx, I32_WMV *idfy, I32_WMV *idbx, I32_WMV *idby)
{
    I32_WMV  iScaleFactor = pWMVDec->m_iBNumerator * pWMVDec->m_iBFrameReciprocal;  // can be pulled into frame level
    I32_WMV iMinCoordinate = -60;  // -15 or -7 pixels
    I32_WMV iMaxX = pWMVDec->m_uintNumMBX * 64 - 4;  // x8 for block x4 for qpel
    I32_WMV iMaxY = pWMVDec->m_uintNumMBY * 64 - 4;
    I32_WMV iX1 ;
    I32_WMV iY1 ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DirectModeMV);

    // check if not intra (later)
  if(!pWMVDec->m_bCodecIsWVC1){
    if (bHpelMode) {
        *idfx = 2 * ((iXMotion * iScaleFactor + 255) >> 9);
        *idfy = 2 * ((iYMotion * iScaleFactor + 255) >> 9); 
        *idbx = 2 * ((iXMotion * (iScaleFactor - 256) + 255) >> 9);
        *idby = 2 * ((iYMotion * (iScaleFactor - 256) + 255) >> 9);
    }
    else {
        *idfx = (iXMotion * iScaleFactor + 128) >> 8;
        *idfy = (iYMotion * iScaleFactor + 128) >> 8; 
        *idbx = (iXMotion * (iScaleFactor - 256) + 128) >> 8;
        *idby = (iYMotion * (iScaleFactor - 256) + 128) >> 8;
    }
  }
  else {
    //zou fix
	if((pWMVDec->m_rgmbmd+imbY*pWMVDec->m_uintNumMBX + imbX)->m_mbType == FORWARD)
	{
		if (bHpelMode) 
		{
			//*idfx = 2 * ((iXMotion * iScaleFactor + 255) >> 9);
			//*idfy = 2 * ((iYMotion * iScaleFactor + 255) >> 9); 
			*idbx = 2 * ((iXMotion * (iScaleFactor - 256) + 255) >> 9);
			*idby = 2 * ((iYMotion * (iScaleFactor - 256) + 255) >> 9);
		}
		else {
			//*idfx = (iXMotion * iScaleFactor + 128) >> 8;
			//*idfy = (iYMotion * iScaleFactor + 128) >> 8; 
			*idbx = (iXMotion * (iScaleFactor - 256) + 128) >> 8;
			*idby = (iYMotion * (iScaleFactor - 256) + 128) >> 8;
		}
	}
	else if((pWMVDec->m_rgmbmd+imbY*pWMVDec->m_uintNumMBX + imbX)->m_mbType == BACKWARD)
	{
		if (bHpelMode) {
			*idfx = 2 * ((iXMotion * iScaleFactor + 255) >> 9);
			*idfy = 2 * ((iYMotion * iScaleFactor + 255) >> 9); 
			//*idbx = 2 * ((iXMotion * (iScaleFactor - 256) + 255) >> 9);
			//*idby = 2 * ((iYMotion * (iScaleFactor - 256) + 255) >> 9);
		}
		else {
			*idfx = (iXMotion * iScaleFactor + 128) >> 8;
			*idfy = (iYMotion * iScaleFactor + 128) >> 8; 
			//*idbx = (iXMotion * (iScaleFactor - 256) + 128) >> 8;
			//*idby = (iYMotion * (iScaleFactor - 256) + 128) >> 8;
		}
	}
	else
	{
		if (bHpelMode) {
			*idfx = 2 * ((iXMotion * iScaleFactor + 255) >> 9);
			*idfy = 2 * ((iYMotion * iScaleFactor + 255) >> 9); 
			*idbx = 2 * ((iXMotion * (iScaleFactor - 256) + 255) >> 9);
			*idby = 2 * ((iYMotion * (iScaleFactor - 256) + 255) >> 9);
		}
		else {
			*idfx = (iXMotion * iScaleFactor + 128) >> 8;
			*idfy = (iYMotion * iScaleFactor + 128) >> 8; 
			*idbx = (iXMotion * (iScaleFactor - 256) + 128) >> 8;
			*idby = (iYMotion * (iScaleFactor - 256) + 128) >> 8;
		}
	}
  }

#ifdef _WMV9AP_
    if (pWMVDec->m_bInterlaceV2)
	{
        return;
    }
#endif

    // limit motion vector - can be accelerated by applying only to affected blocks
    // around periphery of frame

  if(!pWMVDec->m_bCodecIsWVC1){
    imbX *= 64; // original qpel coordinate
    imbY *= 64;
   
     iX1 = imbX + *idfx;
     iY1 = imbY + *idfy;

    if (iX1 < iMinCoordinate)
        *idfx = iMinCoordinate - imbX;
    else if (iX1 > iMaxX)
        *idfx = iMaxX - imbX;

    if (iY1 < iMinCoordinate)
        *idfy = iMinCoordinate - imbY;
    else if (iY1 > iMaxY)
        *idfy = iMaxY - imbY;

    iX1 = imbX + *idbx;
    iY1 = imbY + *idby;

    if (iX1 < iMinCoordinate)
        *idbx = iMinCoordinate - imbX;
    else if (iX1 > iMaxX)
        *idbx = iMaxX - imbX;

    if (iY1 < iMinCoordinate)
        *idby = iMinCoordinate - imbY;
    else if (iY1 > iMaxY)
        *idby = iMaxY - imbY;
  }
  else {
	if((pWMVDec->m_rgmbmd+imbY*pWMVDec->m_uintNumMBX + imbX)->m_mbType == FORWARD)
	{
		imbX *= 64; // original qpel coordinate
		imbY *= 64;	

		iX1 = imbX + *idbx;
		iY1 = imbY + *idby;

		if (iX1 < iMinCoordinate)
			*idbx = iMinCoordinate - imbX;
		else if (iX1 > iMaxX)
			*idbx = iMaxX - imbX;

		if (iY1 < iMinCoordinate)
			*idby = iMinCoordinate - imbY;
		else if (iY1 > iMaxY)
			*idby = iMaxY - imbY;
	}
	else if((pWMVDec->m_rgmbmd+imbY*pWMVDec->m_uintNumMBX + imbX)->m_mbType == BACKWARD)
	{
		imbX *= 64; // original qpel coordinate
		imbY *= 64;	   

		 iX1 = imbX + *idfx;
		 iY1 = imbY + *idfy;

		if (iX1 < iMinCoordinate)
			*idfx = iMinCoordinate - imbX;
		else if (iX1 > iMaxX)
			*idfx = iMaxX - imbX;

		if (iY1 < iMinCoordinate)
			*idfy = iMinCoordinate - imbY;
		else if (iY1 > iMaxY)
			*idfy = iMaxY - imbY;
	}
	else
	{
		// limit motion vector - can be accelerated by applying only to affected blocks
		// around periphery of frame
		imbX *= 64; // original qpel coordinate
		imbY *= 64;	   

		 iX1 = imbX + *idfx;
		 iY1 = imbY + *idfy;

		if (iX1 < iMinCoordinate)
			*idfx = iMinCoordinate - imbX;
		else if (iX1 > iMaxX)
			*idfx = iMaxX - imbX;

		if (iY1 < iMinCoordinate)
			*idfy = iMinCoordinate - imbY;
		else if (iY1 > iMaxY)
			*idfy = iMaxY - imbY;

		iX1 = imbX + *idbx;
		iY1 = imbY + *idby;

		if (iX1 < iMinCoordinate)
			*idbx = iMinCoordinate - imbX;
		else if (iX1 > iMaxX)
			*idbx = iMaxX - imbX;

		if (iY1 < iMinCoordinate)
			*idby = iMinCoordinate - imbY;
		else if (iY1 > iMaxY)
			*idby = iMaxY - imbY;
	}
  }
}




//#pragma warning(default: 4799)  // emms warning disable

/***********************************************************************
  ChromaMV_B : Converts luma motion vector to chroma (sort of div by 2)
***********************************************************************/
Void_WMV ChromaMV_B (tWMVDecInternalMember * pWMVDec, I32_WMV *iX, I32_WMV *iY)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ChromaMV_B);
    // weak coercion to half pel positions
    *iX = (*iX + s_RndTbl[(*iX) & 3]) >> 1;
    *iY = (*iY + s_RndTbl[(*iY) & 3]) >> 1;
    // strong coercion to half pel positions
    if (pWMVDec->m_bUVHpelBilinear) {
        if ((*iX) & 1) {
            if (*iX > 0) *iX = *iX - 1;
            else *iX = *iX + 1;
        }
        if ((*iY) & 1) {
            if (*iY > 0) *iY = *iY - 1;
            else *iY = *iY + 1;
        }
    }
}


//#endif // X9

