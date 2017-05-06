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
 

#include "spatialpredictor_wmv.h"
#include "localhuffman_wmv.h"
#include "tables_wmv.h"
#include "wmvdec_function.h"

#include <math.h>

extern Bool_WMV g_bSupportAltiVec_WMV;
extern Bool_WMV g_bSupportWMMX_WMV;

Void_WMV g_InterpolateBlockBilinear_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl);

Void_WMV g_AddError_C(U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch );
Void_WMV g_AddErrorIntra_C(U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch);
Void_WMV g_InterpolateVariableRefBicubic_MMX (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl);
Void_WMV g_PixelMean (const U8_WMV *pSrc1, I32_WMV iStride1, const U8_WMV *pSrc2, I32_WMV iStride2, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iWidth, I32_WMV iHeight);

void Round_MV_Luam_FRAMEMV(tWMVDecInternalMember *pWMVDec, I32_WMV*IX, I32_WMV*IY,I32_WMV eBlk,
                           I32_WMV xindex,I32_WMV yindex,
                           U32_WMV ncols,	U32_WMV nrows)
{
	I32_WMV X,Y;
	X = 4 * 16 * ncols+ xindex;
    Y = 4 * 16 * nrows + yindex;
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

void Round_MV_Chroma_FRAMEMV(tWMVDecInternalMember *pWMVDec, 
                             I32_WMV *IX, I32_WMV*IY,
                             I32_WMV xindex,I32_WMV yindex,
                             U32_WMV ncols, U32_WMV nrows)
{
	I32_WMV X,Y;
	X = 4 * 8 * ncols+ xindex;
    Y = 4 * 8 * nrows + yindex;
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

void Round_MV_Luam_FILEDMV(tWMVDecInternalMember *pWMVDec, I32_WMV *IX, I32_WMV*IY,I32_WMV* F,
                           I32_WMV xindex,I32_WMV yindex,
                           I32_WMV ncols,I32_WMV nrows)
{
	I32_WMV X,Y;
	X = 4 * 16 * ncols+ xindex;
    Y = 4 * 16 * nrows + yindex;
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

void Round_MV_Chroma_FILEDMV(tWMVDecInternalMember *pWMVDec, 
                                            I32_WMV *IX, I32_WMV*IY,I32_WMV* F,
                                            I32_WMV xindex,I32_WMV yindex,
                                            I32_WMV imbX, I32_WMV imbY)
{
	I32_WMV X,Y;
	X = 4 * 8 * imbX+ xindex;
    Y = 4 * 8 * imbY + yindex;
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
 
//void Round_MV_Luam(tWMVDecInternalMember *pWMVDec, I32_WMV *iMvX, I32_WMV *iMvY,
//                   U32_WMV ncols,U32_WMV nrows)
//{
//	int xIndex = ncols<<4;
//	int yIndex = nrows<<4;
//
//	if( (*iMvX>>2) + xIndex <= -32)  //zou fix
//		*iMvX =  ((-31-xIndex)<<2) ;
//	else	if( (*iMvX>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)
//		*iMvX = ((pWMVDec->m_iWidthPrevY+31 -xIndex)<<2);
//
//	if( (*iMvY>>2) + yIndex <= -32)  //zou fix
//		*iMvY =  ((-31-yIndex)<<2);
//	else	if( (*iMvY>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)
//		*iMvY = ((pWMVDec->m_iHeightPrevY+31 -yIndex)<<2) ;
//}

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

	*iMvX = X;
	*iMvY = Y;

	return 0;
}

/***********************************************************************
  PredictHybridMV :
  Get predictor and test to see if condition for hybrid MV is met
***********************************************************************/

//#pragma warning(disable: 4799)  // emms warning disable

//#pragma code_seg (EMBSEC_PML)
 I32_WMV medianof4_C (I32_WMV a0, I32_WMV a1, I32_WMV a2, I32_WMV a3)
{
    I32_WMV  max, min;
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

//#pragma code_seg (EMBSEC_DEF)

static I32_WMV s_RndTbl[] = { 0, 0, 0, 1 };

#define SIGN(a) ((a < 0) ? -1 : 1)
#define ABS(x)      (((x) >= 0) ? (x) : -(x)) /** Absolute value of x */

/***********************************************************************
  InitBlockInterpolation : Initialize block interpolation routines
***********************************************************************/

Void_WMV InitBlockInterpolation (tWMVDecInternalMember * pWMVDec)
{
#if defined(macintosh) || defined(_ARM_)
    if ((!g_bSupportAltiVec_WMV) && (!g_bSupportWMMX_WMV))
#endif
    InitBlockInterpolation_EMB (pWMVDec);

    if (g_bSupportWMMX_WMV) {

    } else {
        //pWMVDec->m_pInterpolateBlockBicubic = g_InterpolateBlockBicubic_C;
        pWMVDec->m_pAddError = g_AddError_C;
    }

    //1258
    //pWMVDec->m_pInterpolateBlockBilinear = g_InterpolateBlockBilinear_C;  

    pWMVDec->m_pAddError = g_AddError_C;

	pWMVDec->m_pAddErrorIntra = g_AddErrorIntra_C;

    // Danger - this may need to move someplace???

    pWMVDec->m_pPixelMean = g_PixelMean;
    pWMVDec->m_pOverlapBlockVerticalEdge = g_OverlapBlockVerticalEdge_Fun;
    pWMVDec->m_pOverlapBlockHorizontalEdge = g_OverlapBlockHorizontalEdge_Fun;

    InitFieldInterpolation (pWMVDec);
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


Void_WMV  g_InterpolateBlockBilinear_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl)
{
    I32_WMV i, j, k;
    const I16_WMV *pH = gaSubsampleBilinear + 2 * iXFrac;
    const I16_WMV *pV = gaSubsampleBilinear + 2 * iYFrac;
    I32_WMV pFilter[32 + 1];

    for (j = 0; j < 8; j++) {
        for (i = 0; i < 9; i++) {
            const U8_WMV  *pT = pSrc + i * iSrcStride;
            pFilter[i] = pT[0] * pH[0] + pT[1] * pH[1];
        }
        for (i = 0; i < 8; i++) {
            k = (pFilter[i] * pV[0] + pFilter[1 + i] * pV[1] + 8 - iRndCtrl) >> 4;
            if (k < 0)
                k = 0;
            else if (k > 255)
                k = 255;
            pDst[iDstStride * i] = (U8_WMV) k;
        }

        pSrc++;
        pDst++;
    }
}
/***********************************************************************
  AddError routines: C, MMX, etc versions
***********************************************************************/
#define CLIP(x) ((U32_WMV) x <= 255 ? x : (x < 0 ? 0: 255))
/***********************************************************************
  g_AddError : C version of AddError
***********************************************************************/
Void_WMV g_AddError_C(U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch )
{
    I32_WMV iy;
    
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

        ppxlcRef += iPitch;
        ppxlcDst += iPitch;
        ppxliError += BLOCK_SIZE;
    }
  
}

Void_WMV g_AddErrorIntra_C(U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch )
{
    I32_WMV iy;
    
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
extern Void_WMV IntensityComp_ARMV4 (U32_WMV* pScr, I32_WMV iFrameSize, const U8_WMV* pLUT);
#endif

void voIntensityCompensation_C(U8_WMV* pSrcY,
							 U8_WMV* pSrcU,
							 U8_WMV* pSrcV,
							 I32_WMV iScale,
							 I32_WMV iShift,
							 I32_WMV stridey,
							 I32_WMV height)
{
	int i=0;
	 for (i = 0; i < stridey * height; i++)
	 {
		 U8_WMV Y = (U8_WMV)(pSrcY[i]);
		 pSrcY[i] = CLIP((iScale *Y + iShift + 32) >> 6);   
	 }

	 for (i = 0; i < (stridey * height/4); i++)
	 {
		 U8_WMV U = (U8_WMV)(pSrcU[i]);
		 U8_WMV V = (U8_WMV)(pSrcV[i]);

		 pSrcU[i] = CLIP((iScale * (U - 128) + 128 * 64 + 32) >>6);
		 pSrcV[i] = CLIP((iScale * (V - 128) + 128 * 64 + 32) >>6);
	 }
}

//
//U8_WMV pLUT[256];
//U8_WMV pLUTUV[256];

Void_WMV IntensityCompensation_mbrow (tWMVDecInternalMember * pWMVDec,U32_WMV mb_row)
{   
	EMB_PMainLoop  * pPMainLoop = &pWMVDec->m_EMB_PMainLoop;
    EMB_PBMainLoop *pPB = &pPMainLoop->PB[0];

	U8_WMV *pSrcY = (pPB->m_ppxliRefBlkBase[0] - pWMVDec->m_iWidthPrevYXExpPlusExp) + pWMVDec->m_iWidthPrevY*mb_row*16;//
	U8_WMV *pSrcU = (pPB->m_ppxliRefBlkBase[4] - pWMVDec->m_iWidthPrevUVXExpPlusExp) + pWMVDec->m_iWidthPrevUV*mb_row*8;
	U8_WMV *pSrcV = (pPB->m_ppxliRefBlkBase[5] - pWMVDec->m_iWidthPrevUVXExpPlusExp) + pWMVDec->m_iWidthPrevUV*mb_row*8;

	U8_WMV *pDstY = pWMVDec->m_pfrmRef0Process->m_pucYPlane + pWMVDec->m_iWidthPrevY*mb_row*16;
    U8_WMV *pDstU = pWMVDec->m_pfrmRef0Process->m_pucUPlane + pWMVDec->m_iWidthPrevUV*mb_row*8;
    U8_WMV *pDstV = pWMVDec->m_pfrmRef0Process->m_pucVPlane + pWMVDec->m_iWidthPrevUV*mb_row*8;

    I32_WMV iWidth = 0;
	I32_WMV  i, j,ky=0,ku=0,kv=0;

	if(pWMVDec->m_scaling)
	{
		pSrcY = pWMVDec->m_pfrmRef0Process->m_pucYPlane + pWMVDec->m_iWidthPrevY*mb_row*16;
		pSrcU = pWMVDec->m_pfrmRef0Process->m_pucUPlane + pWMVDec->m_iWidthPrevUV*mb_row*8;
		pSrcV = pWMVDec->m_pfrmRef0Process->m_pucVPlane + pWMVDec->m_iWidthPrevUV*mb_row*8;
	}

	if (pWMVDec->m_bLuminanceWarp) 
	{
		for(i=0;i<16*pWMVDec->m_iWidthPrevY;i++)
		{
			U8_WMV index = (U8_WMV)(pSrcY[i]);
			pDstY[i] = pWMVDec->m_pLUT[index];
		}
		
		for(i=0;i<8*pWMVDec->m_iWidthPrevUV;i++)
		{
			U8_WMV index = (U8_WMV)(pSrcU[i]);
			pDstU[i] = pWMVDec->m_pLUTUV[index];
			index = (U8_WMV)(pSrcV[i]);
			pDstV[i] = pWMVDec->m_pLUTUV[index];
		}
	}
	return;
}

Void_WMV IntensityCompensation_Parpare (tWMVDecInternalMember * pWMVDec)
{
	U32_WMV mb_row = 0;
	I32_WMV  i=0,j=0;    
    I32_WMV iScale, iShift;
    if (pWMVDec->m_iLuminanceShift > 31)
        pWMVDec->m_iLuminanceShift -= 64;

    // remap luminance scale and shift
    if (pWMVDec->m_iLuminanceScale == 0)
    {       
        iScale = - 64;
        iShift = 255 * 64 - pWMVDec->m_iLuminanceShift * 2 * 64;
    }
    else 
    {
        iScale = pWMVDec->m_iLuminanceScale  + 32;
        iShift = pWMVDec->m_iLuminanceShift * 64;
    }

    //pWMVDec->m_iLuminanceScale = iScale; //
    //pWMVDec->m_iLuminanceShift = iShift;

	for (i = 0; i < 256; i++)
	{
		j = (iScale * i + iShift + 32) >> 6;                
		pWMVDec->m_pLUT[i] = (U8_WMV) CLIP(j);

		j = (iScale * (i - 128) + 128 * 64 + 32) >>6;
		pWMVDec->m_pLUTUV[i] = (U8_WMV)CLIP(j);
	}
}
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
#define OUTER_1 ((7 * v0 + v3 + iRnd + 3) >> 3)
#define OUTER_2 ((7 * v3 + v0 + 4 - iRnd) >> 3)
#define INNER_1 ((7 * v1 - v0 + v2 + v3 + 4 - iRnd) >> 3)
#define INNER_2 ((7 * v2 - v3 + v1 + v0 + iRnd + 3) >> 3)

Void_WMV g_OverlapBlockVerticalEdge_C (I16_WMV *pInOut, I32_WMV iStride)
{
    I32_WMV iRnd = 1;
    I32_WMV ii ;

    for ( ii = 0; ii < 8; ii++) {
        I16_WMV v0 = pInOut[-2];
        I16_WMV v1 = pInOut[-1];
        I16_WMV v2 = pInOut[0];
        I16_WMV v3 = pInOut[1];

        pInOut[-2] = OUTER_1;
        pInOut[-1] = INNER_1;
        pInOut[0] = INNER_2;
        pInOut[1] = OUTER_2;

        /*pInOut[-2] = ((7 * v0 + v3 + iRnd + 3) >> 3);
        pInOut[-1] = ((7 * v1 - v0 + v2 + v3 + 4 - iRnd) >> 3);
        pInOut[0] = ((7 * v2 - v3 + v1 + v0 + iRnd + 3) >> 3);
        pInOut[1] = ((7 * v3 + v0 + 4 - iRnd) >> 3);*/

        iRnd ^= 1;
        
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
Void_WMV g_OverlapBlockHorizontalEdge_C ( I16_WMV *pSrcTop, I16_WMV *pSrcCurr,
                                                 I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,
                                                 Bool_WMV bTop, Bool_WMV bCurrent, Bool_WMV bWindup)
{
    I32_WMV  iRnd = 1;
    I32_WMV i ;
    I32_WMV ii, jj;

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


        pSrcCurr += 2 * iSrcStride;
        pDst += 2 * iDstStride;

        if (!bWindup) {
            for (i = 0; i < 4; i++) {
                for ( jj = 0; jj < 8; jj++) {
                    I32_WMV k = pSrcCurr[jj] + g_iOverlapOffset;
                    pDst[jj] = (k < 0) ? 0 : ((k > 255) ? 255 : k);
                }
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

            pSrcCurr += iSrcStride;
            pDst += iDstStride;
        }
    }
}

/****************************************************************************************
  OverlapMBRow : complete overlapped transform reconstruction for I blocks in row of MBs
  // TBD: speedup (e.g. skip if no intra blocks in current and previous rows)
****************************************************************************************/


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
    EMB_PMainLoop  * pPMainLoop = &pWMVDec->m_EMB_PMainLoop; 
    EMB_PBMainLoop *pMainPB = &pPMainLoop->PB[0];
    //EMB_PBMainLoop *pThreadPB = &pPMainLoop->PB[1];

    imbY += (bBottomRowOfSlice != FALSE);
    iBlockIndex = imbY * iXBlocks * 2;
    pMotion =  pWMVDec->m_pXMotion + iBlockIndex;
    pMotionUV =  pWMVDec->m_pXMotionC + imbY * (I32_WMV) pWMVDec->m_uintNumMBX;
	
	pIntraBlockRow = pMainPB->pThreadIntraBlockRow0;
	pIntraMBRowU = pMainPB->pThreadIntraMBRowU0;
	pIntraMBRowV = pMainPB->pThreadIntraMBRowV0;

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

    pMotion = pIntraBlockRow[1];
	pIntraBlockRow[1] = pIntraBlockRow[2];
	pIntraBlockRow[2] = pMotion;

	pMotion = pIntraMBRowU[0];
	pIntraMBRowU[0] = pIntraMBRowU[1];
	pIntraMBRowU[1] = pMotion;

	pMotion = pIntraMBRowV[0];
	pIntraMBRowV[0] = pIntraMBRowV[1];
	pIntraMBRowV[1] = pMotion;

}

/****************************************************************************************
  PixelMean : compute the "mean" of two arrays - width and height are multiples of 8
****************************************************************************************/

Void_WMV g_PixelMean (const U8_WMV *pSrc1, I32_WMV iStride1,
                                const U8_WMV *pSrc2, I32_WMV iStride2,
                                U8_WMV *pDst, I32_WMV iDstStride,
                                I32_WMV   iWidth, I32_WMV iHeight)
{
    I32_WMV i,j;
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
			*idbx = 2 * ((iXMotion * (iScaleFactor - 256) + 255) >> 9);
			*idby = 2 * ((iYMotion * (iScaleFactor - 256) + 255) >> 9);
		}
		else {
			*idbx = (iXMotion * (iScaleFactor - 256) + 128) >> 8;
			*idby = (iYMotion * (iScaleFactor - 256) + 128) >> 8;
		}
	}
	else if((pWMVDec->m_rgmbmd+imbY*pWMVDec->m_uintNumMBX + imbX)->m_mbType == BACKWARD)
	{
		if (bHpelMode) {
			*idfx = 2 * ((iXMotion * iScaleFactor + 255) >> 9);
			*idfy = 2 * ((iYMotion * iScaleFactor + 255) >> 9); 
		}
		else {
			*idfx = (iXMotion * iScaleFactor + 128) >> 8;
			*idfy = (iYMotion * iScaleFactor + 128) >> 8; 
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

    if (pWMVDec->m_bInterlaceV2)
	{
        return;
    }

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


Void_WMV DirectModeMV_Prog_BVOP (tWMVDecInternalMember * pWMVDec,
                                                CWMVMBMode*pmbmd,
                                                I32_WMV iXMotion, I32_WMV iYMotion, Bool_WMV bHpelMode,
                                                I32_WMV imbX, I32_WMV imbY,
                                                I32_WMV *idfx, I32_WMV *idfy, I32_WMV *idbx, I32_WMV *idby)
{
    I32_WMV  iScaleFactor = pWMVDec->m_iBNumerator * pWMVDec->m_iBFrameReciprocal;  // can be pulled into frame level
    I32_WMV iMinCoordinate = -60;  // -15 or -7 pixels
    I32_WMV iMaxX = pWMVDec->m_uintNumMBX * 64 - 4;  // x8 for block x4 for qpel
    I32_WMV iMaxY = pWMVDec->m_uintNumMBY * 64 - 4;
    I32_WMV iX1 ;
    I32_WMV iY1 ;

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
	if(pmbmd->m_mbType == FORWARD)
	{
		if (bHpelMode) 
		{
			*idbx = 2 * ((iXMotion * (iScaleFactor - 256) + 255) >> 9);
			*idby = 2 * ((iYMotion * (iScaleFactor - 256) + 255) >> 9);
		}
		else {
			*idbx = (iXMotion * (iScaleFactor - 256) + 128) >> 8;
			*idby = (iYMotion * (iScaleFactor - 256) + 128) >> 8;
		}
	}
	else if(pmbmd->m_mbType == BACKWARD)
	{
		if (bHpelMode) {
			*idfx = 2 * ((iXMotion * iScaleFactor + 255) >> 9);
			*idfy = 2 * ((iYMotion * iScaleFactor + 255) >> 9); 
		}
		else {
			*idfx = (iXMotion * iScaleFactor + 128) >> 8;
			*idfy = (iYMotion * iScaleFactor + 128) >> 8; 
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

    if (pWMVDec->m_bInterlaceV2)
	{
        return;
    }

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
	if(pmbmd->m_mbType == FORWARD)
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
	else if(pmbmd->m_mbType == BACKWARD)
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

