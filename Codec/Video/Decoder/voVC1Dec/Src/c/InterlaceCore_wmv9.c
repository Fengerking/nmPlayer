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
#include "localhuffman_wmv.h"
#include "codehead.h"

I32_WMV PredictFieldBotMV (tWMVDecInternalMember *pWMVDec,
    I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    // find motion predictor
    I32_WMV iX = *iPredX;
    I32_WMV iY = *iPredY;

    I32_WMV iX1 = iX * 16 * 4;
    I32_WMV iY1 = iY * 16 * 4;
    I32_WMV iIndex = iY * pWMVDec->m_uintNumMBX * 4 + iX * 2;
    I32_WMV iMinCoordinate = -60;  // -15 pixels
    I32_WMV iMaxX = pWMVDec->m_uintNumMBX * 2 * 8 * 4 - 4;  // x8 for block x4 for qpel
    I32_WMV iMaxY = pWMVDec->m_uintNumMBY * 2 * 8 * 4 - 4;

    if (!bTopBndry) {        
        I32_WMV iTopX, iTopY;
        I32_WMV iLeftX, iLeftY;
        I32_WMV iTopRightX, iTopRightY;
        Bool_WMV bIntraTop, bIntraLeft, bIntraTopRight;

        // left block
        if (iX) {
            iLeftX = pFieldMvX [iIndex + 2 * pWMVDec->m_uintNumMBX - 2];
            iLeftY = pFieldMvY [iIndex + 2 * pWMVDec->m_uintNumMBX - 2];        
        } else {
            			// special case - 1MB wide frame
			if (pWMVDec->m_uintNumMBX == 1) {
				iX = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX + 2 * pWMVDec->m_uintNumMBX];
				iY = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX + 2 * pWMVDec->m_uintNumMBX];
				goto End;
			}
            iLeftX = 0;
            iLeftY = 0;
        }

        bIntraLeft = (iLeftX == IBLOCKMV);

        if (bIntraLeft) {
            iLeftX = iLeftY = 0;  // set it to zero for median
        }

        // top block
        iTopX = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX + 2 * pWMVDec->m_uintNumMBX];
        iTopY = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX + 2 * pWMVDec->m_uintNumMBX];

        bIntraTop = (iTopX == IBLOCKMV);

        if (bIntraTop) {
            iTopX = iTopY = 0;
        } 

        // top right block
        if (iX != (I32_WMV) pWMVDec->m_uintNumMBX - 1) {
            iTopRightX = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX + 2 + 2 * pWMVDec->m_uintNumMBX];
            iTopRightY = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX + 2 + 2 * pWMVDec->m_uintNumMBX];
        } else {
//            iTopRightX = iTopRightY = 0;
            iTopRightX = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX - 2 + 2 * pWMVDec->m_uintNumMBX];
            iTopRightY = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX - 2 + 2 * pWMVDec->m_uintNumMBX];
        }

        bIntraTopRight = (iTopRightX == IBLOCKMV);

        if (bIntraTopRight) {
            iTopRightX = iTopRightY = 0;
        }

        if (bIntraLeft + bIntraTop + bIntraTopRight > 1) {
            iX = IBLOCKMV;
        } else {
            iX = medianof3 (iLeftX, iTopX, iTopRightX);
            iY = medianof3 (iLeftY, iTopY, iTopRightY);
        }
End:;
    }    
    else if (iX) {
        // left predictor
        iY = pFieldMvY[iIndex + pWMVDec->m_uintNumMBX * 2 - 2];
        iX = pFieldMvX[iIndex + pWMVDec->m_uintNumMBX * 2 - 2];   
    }
    else {
        iX = iY = 0;
    }

    if (iX == IBLOCKMV) {
        iX = iY = 0;

        *iPredX = iX; *iPredY = iY;
        return 1;
    }

    // limit motion vector
    iX1 += iX;
    iY1 += iY;


    if (iX1 < iMinCoordinate)
        iX += iMinCoordinate - iX1;
    else if (iX1 > iMaxX)
        iX += iMaxX - iX1;

    if (iY1 < iMinCoordinate)
        iY += iMinCoordinate - iY1;
    else if (iY1 > iMaxY)
        iY += iMaxY - iY1;

    *iPredX = iX; *iPredY = iY;
    return 0;
}

I32_WMV PredictFieldTopMV (tWMVDecInternalMember *pWMVDec,
    I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    // find motion predictor
    I32_WMV iX = *iPredX;
    I32_WMV iY = *iPredY;
    I32_WMV iX1 = iX * 16 * 4;
    I32_WMV iY1 = iY * 16 * 4;

    I32_WMV iIndex  = iY * pWMVDec->m_uintNumMBX * 4 + iX * 2;
    I32_WMV iMinCoordinate = -60;  // -15 pixels
    I32_WMV iMaxX = pWMVDec->m_uintNumMBX * 2 * 8 * 4 - 4;  // x8 for block x4 for qpel
    I32_WMV iMaxY = pWMVDec->m_uintNumMBY * 2 * 8 * 4 - 4;

    if (!bTopBndry) {
        
        I32_WMV iTopX, iTopY;
        I32_WMV iLeftX, iLeftY;
        I32_WMV iTopRightX, iTopRightY;
        Bool_WMV bIntraTop, bIntraLeft, bIntraTopRight;

        // left block
        if (iX) {
            iLeftX = pFieldMvX [iIndex - 2];
            iLeftY = pFieldMvY [iIndex - 2];
        } else {
			// special case - 1MB wide frame
			if (pWMVDec->m_uintNumMBX == 1) {
				iX = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX];
				iY = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX];
				goto End;
			}
            iLeftX = 0;
            iLeftY = 0;
        }

        bIntraLeft = (iLeftX == IBLOCKMV);

        if (bIntraLeft) {
            iLeftX = iLeftY = 0;  // set it to zero for median
        } 

        // top block
        iTopX = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX];
        iTopY = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX];

        bIntraTop = (iTopX == IBLOCKMV);

        if (bIntraTop) {
            iTopX = iTopY = 0;
        }
        
        // top right block
        if (iX != (I32_WMV) pWMVDec->m_uintNumMBX - 1) {
            iTopRightX = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX + 2];
            iTopRightY = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX + 2];
        } else {
//            iTopRightX = iTopRightY = 0;
            iTopRightX = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX - 2];
            iTopRightY = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX - 2];
        }

        bIntraTopRight = (iTopRightX == IBLOCKMV);
        if (bIntraTopRight) {
            iTopRightX = iTopRightY = 0;
        } 

        if (bIntraLeft + bIntraTop + bIntraTopRight > 1) {
            iX = IBLOCKMV;
        } else {
            iX = medianof3 (iLeftX, iTopX, iTopRightX);
            iY = medianof3 (iLeftY, iTopY, iTopRightY);
        }
End:;
    } else if (iX) {
        // left predictor
        iY = pFieldMvY[iIndex - 2];
        iX = pFieldMvX[iIndex - 2];   
    } else {
        iX = iY = 0;
    }

    if (iX == IBLOCKMV) {
        iX = iY = 0;

        *iPredX = iX; *iPredY = iY;
        return 1;
    }


    // limit motion vector
    iX1 += iX;
    iY1 += iY;

    if (iX1 < iMinCoordinate)
        iX += iMinCoordinate - iX1;
    else if (iX1 > iMaxX)
        iX += iMaxX - iX1;

    if (iY1 < iMinCoordinate)
        iY += iMinCoordinate - iY1;
    else if (iY1 > iMaxY)
        iY += iMaxY - iY1;

    *iPredX = iX; *iPredY = iY;
    return 0;
}

I32_WMV PredictFieldOneMV (tWMVDecInternalMember *pWMVDec,
    I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    // find motion predictor
    I32_WMV iX = *iPredX;
    I32_WMV iY = *iPredY;
    I32_WMV iX1 = iX * 16 * 4;
    I32_WMV iY1 = iY * 16 * 4;
    I32_WMV iIndex = iY * pWMVDec->m_uintNumMBX * 4 + iX * 2;

    I32_WMV iMinCoordinate = -60;  // -15 pixels
    I32_WMV iMaxX = pWMVDec->m_uintNumMBX * 2 * 8 * 4 - 4;  // x8 for block x4 for qpel
    I32_WMV iMaxY = pWMVDec->m_uintNumMBY * 2 * 8 * 4 - 4;

    if (!bTopBndry) { // not left or top boundary
        I32_WMV iTopX, iTopY, iTopX0, iTopY0, iTopX1, iTopY1;
        I32_WMV iLeftX, iLeftY, iLeftX0, iLeftY0, iLeftX1, iLeftY1;
        I32_WMV iTopRightX, iTopRightY, iTopRightX0, iTopRightY0, iTopRightX1, iTopRightY1;
        
        Bool_WMV bIntraLeft0, bIntraLeft1, bIntraLeft;
        Bool_WMV bIntraTop0, bIntraTop1, bIntraTop;
        Bool_WMV bIntraTopRight0, bIntraTopRight1, bIntraTopRight;

        // left block
        if (iX) {
            iLeftX0 = pFieldMvX [iIndex - 2];
            iLeftY0 = pFieldMvY [iIndex - 2];

            iLeftX1 = pFieldMvX [iIndex + 2 * pWMVDec->m_uintNumMBX - 2];
            iLeftY1 = pFieldMvY [iIndex + 2 * pWMVDec->m_uintNumMBX - 2];        
        } else {
			// special case - 1MB wide frame
			if (pWMVDec->m_uintNumMBX == 1) {
				iTopX0 = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX];
				iTopY0 = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX];

				iTopX1 = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX + 2 * pWMVDec->m_uintNumMBX];
				iTopY1 = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX + 2 * pWMVDec->m_uintNumMBX];

				bIntraTop0 = (iTopX0 == IBLOCKMV);
				bIntraTop1 = (iTopX1 == IBLOCKMV);

				if (bIntraTop0 && bIntraTop1) {
					iX = iY = IBLOCKMV;
				} else if (bIntraTop0) {
					iX = iTopX1;
					iY = iTopY1;
				} else if (bIntraTop1) {
					iX = iTopX0;
					iY = iTopY0;
				} else {
					iX = (iTopX0 + iTopX1 + 1) >> 1;
					iY = (iTopY0 + iTopY1 + 1) >> 1;
				}
				goto End;
			}


            iLeftX0 = 0;
            iLeftY0 = 0;

            iLeftX1 = 0;
            iLeftY1 = 0;
        }

        bIntraLeft0 = (iLeftX0 == IBLOCKMV);
        bIntraLeft1 = (iLeftX1 == IBLOCKMV);
        bIntraLeft = bIntraLeft0 && bIntraLeft1;

        if (bIntraLeft) {
            iLeftX = iLeftY = 0;  // set it to zero for median
        } else if (bIntraLeft0) {
            iLeftX = iLeftX1;
            iLeftY = iLeftY1;
        } else if (bIntraLeft1) {
            iLeftX = iLeftX0;
            iLeftY = iLeftY0;
        } else {
            iLeftX = (iLeftX0 + iLeftX1 + 1) >> 1;
            iLeftY = (iLeftY0 + iLeftY1 + 1) >> 1;
        }

        // top block
        iTopX0 = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX];
        iTopY0 = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX];

        iTopX1 = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX + 2 * pWMVDec->m_uintNumMBX];
        iTopY1 = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX + 2 * pWMVDec->m_uintNumMBX];

        bIntraTop0 = (iTopX0 == IBLOCKMV);
        bIntraTop1 = (iTopX1 == IBLOCKMV);
        bIntraTop = bIntraTop0 && bIntraTop1;

        if (bIntraTop) {
            iTopX = iTopY = 0;
        } else if (bIntraTop0) {
            iTopX = iTopX1;
            iTopY = iTopY1;
        } else if (bIntraTop1) {
            iTopX = iTopX0;
            iTopY = iTopY0;
        } else {
            iTopX = (iTopX0 + iTopX1 + 1) >> 1;
            iTopY = (iTopY0 + iTopY1 + 1) >> 1;
        }
        
        // top right block
        if (iX != (I32_WMV) pWMVDec->m_uintNumMBX - 1) {
            iTopRightX0 = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX + 2];
            iTopRightY0 = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX + 2];

            iTopRightX1 = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX + 2 + 2 * pWMVDec->m_uintNumMBX];
            iTopRightY1 = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX + 2 + 2 * pWMVDec->m_uintNumMBX];
        } else {
//            iTopRightX0 = iTopRightY0 = iTopRightX1 = iTopRightY1 = 0;
            iTopRightX0 = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX - 2];
            iTopRightY0 = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX - 2];

            iTopRightX1 = pFieldMvX [iIndex - 4 * pWMVDec->m_uintNumMBX - 2 + 2 * pWMVDec->m_uintNumMBX];
            iTopRightY1 = pFieldMvY [iIndex - 4 * pWMVDec->m_uintNumMBX - 2 + 2 * pWMVDec->m_uintNumMBX];
        }

        bIntraTopRight0 = (iTopRightX0 == IBLOCKMV);
        bIntraTopRight1 = (iTopRightX1 == IBLOCKMV);
        bIntraTopRight = bIntraTopRight0 && bIntraTopRight1;
        if (bIntraTopRight) {
            iTopRightX = iTopRightY = 0;
        } else if (bIntraTopRight0) {
            iTopRightX = iTopRightX1;
            iTopRightY = iTopRightY1;
        } else if (bIntraTopRight1) {
            iTopRightX = iTopRightX0;
            iTopRightY = iTopRightY0;
        } else {
            iTopRightX = (iTopRightX0 + iTopRightX1 + 1) >> 1;
            iTopRightY = (iTopRightY0 + iTopRightY1 + 1) >> 1;
        }

        if (bIntraLeft + bIntraTop + bIntraTopRight > 1) {
            iX = IBLOCKMV;
        } else {
            iX = medianof3 (iLeftX, iTopX, iTopRightX);
            iY = medianof3 (iLeftY, iTopY, iTopRightY);
        }        
End:;
    }  else if (iX) {
        // left predictor
        // left block
        
        I32_WMV iLeftX0, iLeftY0, iLeftX1, iLeftY1;
        Bool_WMV bIntraLeft0, bIntraLeft1, bIntraLeft;

        iLeftX0 = pFieldMvX [iIndex - 2];
        iLeftY0 = pFieldMvY [iIndex - 2];

        iLeftX1 = pFieldMvX [iIndex + 2 * pWMVDec->m_uintNumMBX - 2];
        iLeftY1 = pFieldMvY [iIndex + 2 * pWMVDec->m_uintNumMBX - 2];        
        
        bIntraLeft0 = (iLeftX0 == IBLOCKMV);
        bIntraLeft1 = (iLeftX1 == IBLOCKMV);
        bIntraLeft = bIntraLeft0 && bIntraLeft1;

        if (bIntraLeft) {
            iX = IBLOCKMV;  // set it to zero for median
        } else if (bIntraLeft0) {
            iX = iLeftX1;
            iY = iLeftY1;
        } else if (bIntraLeft1) {
            iX = iLeftX0;
            iY = iLeftY0;
        } else {
            iX = (iLeftX0 + iLeftX1 + 1) >> 1;
            iY = (iLeftY0 + iLeftY1 + 1) >> 1;
        }               
        
    } else {
        iX = iY = 0;
    }

    if (iX == IBLOCKMV) {
        iX = iY = 0;

        *iPredX = iX; *iPredY = iY;
        return 1;
    }

    // limit motion vector
    iX1 += iX;
    iY1 += iY;
    
    if (iX1 < iMinCoordinate)
        iX += iMinCoordinate - iX1;
    else if (iX1 > iMaxX)
        iX += iMaxX - iX1;

    if (iY1 < iMinCoordinate)
        iY += iMinCoordinate - iY1;
    else if (iY1 > iMaxY)
        iY += iMaxY - iY1;

#ifdef _RESTRICTED_MC_
    if ((iX & 3) && (iY & 3))
    {
        if (!(((iX & 3) == 2) && ((iY & 3) == 2)))
        {
            if ((iX & 3) == 1)
                iX--;
            else if ((iX & 3) == 3)
                iX++;
            if ((iY & 3) == 1)
                iY--;
            else if ((iY & 3) == 3)
                iY++;
        }
    }
#endif

    *iPredX = iX; *iPredY = iY;
    return 0;
}

// positive half of ChromaMvRound
static const I32_WMV iChromaMvRound [16] = {
    0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4};

I32_WMV ChromaMVInterlace411 (  tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, I32_WMV iMvX, I32_WMV iMvY, I32_WMV *iChromaMvX, I32_WMV *iChromaMvY)
{
    I32_WMV iSignMvX = (iMvX < 0) ? -1 : 1;
    I32_WMV iAbsMvX = (iMvX < 0) ? -iMvX : iMvX;
    I32_WMV iChromaMvXInt = iAbsMvX >> 4;
    I32_WMV iChromaMvXFrac = iChromaMvRound [iAbsMvX - (iChromaMvXInt << 4)];    
    I32_WMV iX0, iY0;
 
#ifdef STABILITY
    if(iMvX == IBLOCKMV)
        return -1;
#endif

    iMvX = ((iChromaMvXInt << 2) + iChromaMvXFrac) * iSignMvX;

    // bound check
    iX0 = imbX * 8 + (iMvX >> 2);
    iY0 = imbY * 8 + (iMvY >> 2);

    if (iX0 < -8) {
        iMvX -= (iX0 + 8) * 4;
    } else if (iX0 > (I32_WMV) pWMVDec->m_uintNumMBX * 8)
        iMvX -= (iX0 - (I32_WMV) pWMVDec->m_uintNumMBX * 8) * 4;

    if (iY0 < -8) {
        iMvY -= (iY0 + 8) * 4;
    } else if (iY0 > (I32_WMV) pWMVDec->m_uintNumMBY * 8) {
        iMvY -= (iY0 - (I32_WMV) pWMVDec->m_uintNumMBY * 8) * 4;
    }

    *iChromaMvX = iMvX;
    *iChromaMvY = iMvY;

    return 0;
    
}

//static Void_WMV CopyPixelCToPixelIBlock (
//    U8_WMV *pSrc, I16_WMV *pDst, I32_WMV iSrcOffset, I32_WMV iDstOffset, I32_WMV iNumY, I32_WMV iNumX)
//{
//    I32_WMV i, j;
//    for (i = 0; i < iNumY; i++) {
//        for (j = 0; j < iNumX; j++) {
//            pDst [j] = (I16_WMV) pSrc [j];
//        }
//        pDst += iDstOffset;
//        pSrc += iSrcOffset;
//    }
//}

static Void_WMV CopyPixelIToPixelCBlock (
    I16_WMV *pSrc, U8_WMV *pDst, I32_WMV iSrcOffset, I32_WMV iDstOffset, I32_WMV iNumY, I32_WMV iNumX)
{
    I32_WMV y, x;
    for (y = 0; y < iNumY; y++) {
        for (x = 0; x < iNumX; x++) {    
            pDst [x] = (U8_WMV) CLIP (pSrc [x]);            
        }
        pDst += iDstOffset;
        pSrc += iSrcOffset;
    }
}

//static Void_WMV CopyPixelIToPixelIBlock (
//    I16_WMV *pSrc, I16_WMV *pDst, I32_WMV iSrcOffset, I32_WMV iDstOffset, I32_WMV iNumY, I32_WMV iNumX)
//{
//    I32_WMV y, x;
//    for (y = 0; y < iNumY; y++) {
//        for (x = 0; x < iNumX; x++) {    
//            pDst [x] = pSrc [x];            
//        }
//        pDst += iDstOffset;
//        pSrc += iSrcOffset;
//    }
//}

Void_WMV ClearFieldMBInterlace411 (I16_WMV *ppxliFieldMB, I32_WMV iField) 
{
    if (iField == 2) {
        ALIGNED32_FASTMEMCLR (ppxliFieldMB, 0, 6 * 8 * 8 * sizeof(I16_WMV));
    } else if (iField == 0) { // Y0 Y1 U0 V0
        ALIGNED32_FASTMEMCLR (ppxliFieldMB, 0, 8 * 8 * sizeof(I16_WMV));
        ALIGNED32_FASTMEMCLR (ppxliFieldMB + 64, 0, 8 * 8 * sizeof(I16_WMV));
        ALIGNED32_FASTMEMCLR (ppxliFieldMB + 4 * 64, 0, 4 * 8 * sizeof(I16_WMV));
        ALIGNED32_FASTMEMCLR (ppxliFieldMB + 5 * 64, 0, 4 * 8 * sizeof(I16_WMV));
    } else {//if (iField == 1) { // Y2 Y3 U1 V1
        ALIGNED32_FASTMEMCLR (ppxliFieldMB + 2*64, 0, 8 * 8 * sizeof(I16_WMV));
        ALIGNED32_FASTMEMCLR (ppxliFieldMB + 3*64, 0, 8 * 8 * sizeof(I16_WMV));
        ALIGNED32_FASTMEMCLR (ppxliFieldMB + 4 * 64 + 4 * 8, 0, 4 * 8 * sizeof(I16_WMV));
        ALIGNED32_FASTMEMCLR (ppxliFieldMB + 5 * 64 + 4 * 8, 0, 4 * 8 * sizeof(I16_WMV));
    } 
}

Void_WMV CopyIntraFieldMBtoTopFieldInterlace411 (
    I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, U8_WMV *ppxliFrameMBU, 
    U8_WMV *ppxliFrameMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV)
{
    // Y0
    I16_WMV *pSrc = ppxliFieldMB;
    U8_WMV *pDst = ppxliFrameMBY;

    CopyPixelIToPixelCBlock (pSrc, pDst, 8, iOffsetY * 2, 8, 8);

    //Y1
    pSrc = ppxliFieldMB + 64;
    pDst = ppxliFrameMBY + 8;
    CopyPixelIToPixelCBlock (pSrc, pDst, 8, iOffsetY * 2, 8, 8);

    // U0
    pSrc = ppxliFieldMB + 4 * 64;
    pDst = ppxliFrameMBU;
    CopyPixelIToPixelCBlock (pSrc, pDst, 4, 2*iOffsetUV, 8, 4);

    // V0
    pSrc = ppxliFieldMB + 5 * 64;
    pDst = ppxliFrameMBV;
    CopyPixelIToPixelCBlock (pSrc, pDst, 4, 2*iOffsetUV, 8, 4);
}

Void_WMV CopyIntraFieldMBtoBotFieldInterlace411 (
    I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, U8_WMV *ppxliFrameMBU, 
    U8_WMV *ppxliFrameMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV)
{
    // Y2
    I16_WMV *pSrc = ppxliFieldMB + 2 * 64;
    U8_WMV *pDst = ppxliFrameMBY + iOffsetY;

    CopyPixelIToPixelCBlock (pSrc, pDst , 8, iOffsetY * 2, 8, 8);

    // Y3
    pSrc = ppxliFieldMB + 3 * 64;
    pDst = ppxliFrameMBY + iOffsetY + 8;
    CopyPixelIToPixelCBlock (pSrc, pDst , 8, iOffsetY * 2, 8, 8);

    // U1
    pSrc = ppxliFieldMB + 4 * 64 + 4*8;
    pDst = ppxliFrameMBU + iOffsetUV;
    CopyPixelIToPixelCBlock (pSrc, pDst, 4, 2*iOffsetUV, 8, 4);

    // V1
    pSrc = ppxliFieldMB + 5 * 64 + 4*8;
    pDst = ppxliFrameMBV + iOffsetUV;
    CopyPixelIToPixelCBlock (pSrc, pDst, 4, 2*iOffsetUV, 8, 4);
}

Void_WMV CopyIntraFieldMBtoFrameMBInterlace411 (I16_WMV *ppxliFieldMB,                                                
    U8_WMV *ppxliFrameMBY, U8_WMV *ppxliFrameMBU, U8_WMV *ppxliFrameMBV,
    I32_WMV iOffsetToNextRowY, I32_WMV iOffsetToNextRowUV, Bool_WMV bSeparateField)
{

    CopyPixelIToPixelCBlock (
        ppxliFieldMB, ppxliFrameMBY, 8, iOffsetToNextRowY, 8, 8);
    CopyPixelIToPixelCBlock (
        ppxliFieldMB + 64, ppxliFrameMBY + 8, 8, iOffsetToNextRowY, 8, 8);
    CopyPixelIToPixelCBlock (ppxliFieldMB + 2 * 64, ppxliFrameMBY + 
        8*iOffsetToNextRowY, 8, iOffsetToNextRowY, 8, 8);
    CopyPixelIToPixelCBlock (ppxliFieldMB + 3 * 64, ppxliFrameMBY + 
        8*iOffsetToNextRowY + 8, 8, iOffsetToNextRowY, 8, 8);
    //U
    CopyPixelIToPixelCBlock (ppxliFieldMB + 4 * 64, ppxliFrameMBU, 4, 
        iOffsetToNextRowUV, 16, 4);
    //V
    CopyPixelIToPixelCBlock (ppxliFieldMB + 5 * 64, ppxliFrameMBV, 4, 
        iOffsetToNextRowUV, 16, 4);
} 

//static Void_WMV CopyPixelIToPixelIBlock_8x8 (
//    I16_WMV *pSrc, I16_WMV *pDst, I32_WMV iSrcOffset, I32_WMV iDstOffset )
//{
//    I32_WMV y, x;
//    for (y = 0; y < 8; y++) {
//        for (x = 0; x < 4; x++) {    
//            ((unsigned int *)pDst) [x] = ((unsigned int *)pSrc) [x];            
//        }
//        pDst += iDstOffset;
//        pSrc += iSrcOffset;
//    }
//}

//Void_WMV CopyInterFieldMBtoTopFieldMBInterlace411 (
//    I16_WMV *ppxliFieldMB, I16_WMV *ppxliFrameMBY, I16_WMV *ppxliFrameMBU, 
//    I16_WMV *ppxliFrameMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV)
//{
//    // Y0
//    I16_WMV *pSrc = ppxliFieldMB;
//    I16_WMV *pDst = ppxliFrameMBY;
//	
//    CopyPixelIToPixelIBlock_8x8 (pSrc, pDst, 8, iOffsetY * 2);
//
//    //CopyPixelIToPixelIBlock (pSrc, pDst, 8, iOffsetY * 2, 8, 8);
//
//    //Y1
//    pSrc = ppxliFieldMB + 64;
//    pDst = ppxliFrameMBY + 8;
//    CopyPixelIToPixelIBlock_8x8 (pSrc, pDst, 8, iOffsetY * 2);
//
//    // CopyPixelIToPixelIBlock (pSrc, pDst, 8, iOffsetY * 2, 8, 8);
//
//    // U0
//    pSrc = ppxliFieldMB + 4 * 64;
//    pDst = ppxliFrameMBU;
//    CopyPixelIToPixelIBlock (pSrc, pDst, 4, 2*iOffsetUV, 8, 4);
//
//    // V0
//    pSrc = ppxliFieldMB + 5 * 64;
//    pDst = ppxliFrameMBV;
//    CopyPixelIToPixelIBlock (pSrc, pDst, 4, 2*iOffsetUV, 8, 4);
//
//}




//Void_WMV CopyInterFieldMBtoBotFieldMBInterlace411 (
//    I16_WMV *ppxliFieldMB, I16_WMV *ppxliFrameMBY,I16_WMV *ppxliFrameMBU, 
//    I16_WMV *ppxliFrameMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV)
//{
//    // Y2
//    I16_WMV *pSrc = ppxliFieldMB + 2 * 64;
//    I16_WMV *pDst = ppxliFrameMBY + iOffsetY;
//
//    CopyPixelIToPixelIBlock_8x8 (pSrc, pDst , 8, iOffsetY * 2);
//    // CopyPixelIToPixelIBlock (pSrc, pDst , 8, iOffsetY * 2, 8, 8);
//
//    // Y3
//    pSrc = ppxliFieldMB + 3 * 64;
//    pDst = ppxliFrameMBY + iOffsetY + 8;
//	    CopyPixelIToPixelIBlock_8x8 (pSrc, pDst , 8, iOffsetY * 2);
//
//    // CopyPixelIToPixelIBlock (pSrc, pDst , 8, iOffsetY * 2, 8, 8);
//
//    // U1
//    pSrc = ppxliFieldMB + 4 * 64 + 4*8;
//    pDst = ppxliFrameMBU + iOffsetUV;
//    CopyPixelIToPixelIBlock (pSrc, pDst, 4, 2*iOffsetUV, 8, 4);
//
//    // V1
//    pSrc = ppxliFieldMB + 5 * 64 + 4*8;
//    pDst = ppxliFrameMBV + iOffsetUV;
//    CopyPixelIToPixelIBlock (pSrc, pDst, 4, 2*iOffsetUV, 8, 4);
//
//}

Void_WMV AddErrorFrameInterlace411 (tWMVDecInternalMember *pWMVDec,
    I16_WMV *ppxliFieldMB, U8_WMV *ppxlcCurrY, U8_WMV *ppxlcCurrU, 
    U8_WMV *ppxlcCurrV, I32_WMV iOffsetY, I32_WMV iOffsetUV)
{
    U8_WMV *pDst;
    I16_WMV *pSrc;

    // Y0
    pDst = ppxlcCurrY;
    pSrc = ppxliFieldMB;
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, iOffsetY);

    // Y1
    pDst = ppxlcCurrY + 8;
    pSrc = ppxliFieldMB + 64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, iOffsetY);

    // Y2
    pDst = ppxlcCurrY + 8 * iOffsetY;
    pSrc = ppxliFieldMB + 2*64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, iOffsetY);

    // Y3
    pDst = ppxlcCurrY + 8 * iOffsetY + 8;
    pSrc = ppxliFieldMB + 3*64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, iOffsetY);

    // U0, U1
    pDst = ppxlcCurrU;
    pSrc = ppxliFieldMB + 4*64;    
    pWMVDec->m_pFieldAddError4x8 (pSrc, pDst, iOffsetUV);  
    pWMVDec->m_pFieldAddError4x8 (pSrc + 4*8, pDst + 8*iOffsetUV, iOffsetUV);

    // V0, V1
    pDst = ppxlcCurrV;
    pSrc = ppxliFieldMB + 5*64;    
    pWMVDec->m_pFieldAddError4x8 (pSrc, pDst, iOffsetUV);  
    pWMVDec->m_pFieldAddError4x8 (pSrc + 4*8, pDst + 8*iOffsetUV, iOffsetUV);
}

Void_WMV AddErrorTopFieldInterlace411 (tWMVDecInternalMember *pWMVDec,
    I16_WMV *ppxliFieldMB, U8_WMV *ppxlcCurrY, U8_WMV *ppxlcCurrU, 
    U8_WMV *ppxlcCurrV, I32_WMV iOffsetY, I32_WMV iOffsetUV)
{
    U8_WMV *pDst;
    I16_WMV *pSrc;
    // Y0
    pDst = ppxlcCurrY;
    pSrc = ppxliFieldMB;
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, 2*iOffsetY);

    // Y1
    pDst = ppxlcCurrY + 8;
    pSrc = ppxliFieldMB + 64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, 2*iOffsetY);

    // U0
    pDst = ppxlcCurrU;
    pSrc = ppxliFieldMB + 4*64;    
    pWMVDec->m_pFieldAddError4x8 (pSrc, pDst, 2*iOffsetUV);  


    // V0
    pDst = ppxlcCurrV;
    pSrc = ppxliFieldMB + 5*64;    
    pWMVDec->m_pFieldAddError4x8 (pSrc, pDst, 2*iOffsetUV);  
}

Void_WMV AddErrorBotFieldInterlace411 (tWMVDecInternalMember *pWMVDec,
    I16_WMV *ppxliFieldMB, U8_WMV *ppxlcCurrY, U8_WMV *ppxlcCurrU, 
    U8_WMV *ppxlcCurrV, I32_WMV iOffsetY, I32_WMV iOffsetUV)
{
    U8_WMV *pDst;
    I16_WMV *pSrc;

    // Y2
    pDst = ppxlcCurrY + iOffsetY;
    pSrc = ppxliFieldMB + 2*64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, 2*iOffsetY);

    // Y3
    pDst = ppxlcCurrY + iOffsetY + 8;
    pSrc = ppxliFieldMB + 3*64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, 2*iOffsetY);

    // U1
    pDst = ppxlcCurrU + pWMVDec->m_iWidthPrevUV;
    pSrc = ppxliFieldMB + 4*64 + 4*8;    
    pWMVDec->m_pFieldAddError4x8 (pSrc, pDst, 2*iOffsetUV);

    // V1
    pDst = ppxlcCurrV + pWMVDec->m_iWidthPrevUV;
    pSrc = ppxliFieldMB + 5*64 + 4*8;    
    pWMVDec->m_pFieldAddError4x8 (pSrc, pDst, 2*iOffsetUV);
}

Void_WMV MotionCompFrameInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV)
{
    U8_WMV *pSrcY, *pSrcU, *pSrcV;
    I32_WMV iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    I32_WMV iMvX = pWMVDec->m_pXMotion [iMvIndex];
    I32_WMV iMvY = pWMVDec->m_pYMotion [iMvIndex];
    I32_WMV iChromaMvX, iChromaMvY;

    // exchanging order between ChromaMV computation and pullbackMotionVector 
    // computation to keep it the same as progressive coding.

    ChromaMVInterlace411 ( pWMVDec, imbX, imbY, iMvX, iMvY, &iChromaMvX, &iChromaMvY);

    // this is required for correct rounding in repeatpad area
    PullBackMotionVector ( pWMVDec, &iMvX, &iMvY, imbX, imbY);

    pSrcY = ppxliRefY + (iMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvX >> 2);
    pSrcU = ppxliRefU + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcV = ppxliRefV + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);

    // Y
    pWMVDec->m_pInterpolateMBRow ( pWMVDec, 
        pSrcY, pWMVDec->m_iWidthPrevY, ppxliReconY, pWMVDec->m_iWidthPrevY, iMvX, iMvY, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);

    // U0, U1
    pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
        pSrcU, pWMVDec->m_iWidthPrevUV, ppxliReconU, pWMVDec->m_iWidthPrevUV, iChromaMvX, iChromaMvY, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);

    // V0, V1
    pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
        pSrcV, pWMVDec->m_iWidthPrevUV, ppxliReconV, pWMVDec->m_iWidthPrevUV, iChromaMvX, iChromaMvY, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);
}


I32_WMV MotionCompFrameInterlace411_BFrame (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV, U8_WMV *ppxliRefY1, U8_WMV *ppxliRefU1, U8_WMV *ppxliRefV1,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV, MBType mbType)
{
    U8_WMV *pSrcY, *pSrcU, *pSrcV;
	U8_WMV *pSrcY1, *pSrcU1, *pSrcV1;
    I32_WMV iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    I32_WMV iMvX = pWMVDec->m_pXMotion [iMvIndex];
    I32_WMV iMvY = pWMVDec->m_pYMotion [iMvIndex];
	I32_WMV iMvXdirect = pWMVDec->m_pFieldMvX_Pbuf [iMvIndex];
	I32_WMV iMvYdirect = pWMVDec->m_pFieldMvY_Pbuf [iMvIndex];
	I32_WMV iMvXinterp = pWMVDec->m_pB411InterpX [iMvIndex];
	I32_WMV iMvYinterp = pWMVDec->m_pB411InterpY [iMvIndex];
    I32_WMV iChromaMvX, iChromaMvY;
	I32_WMV iChromaMvXinterp, iChromaMvYinterp;
    I32_WMV ret =0;

    ret=ChromaMVInterlace411 ( pWMVDec, imbX, imbY, iMvX, iMvY, &iChromaMvX, &iChromaMvY);
    if(ret)
        return -1;

    // this is required for correct rounding in repeatpad area
    PullBackMotionVector ( pWMVDec, &iMvX, &iMvY, imbX, imbY);
    
    pSrcY = ppxliRefY + (iMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvX >> 2);
    pSrcU = ppxliRefU + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcV = ppxliRefV + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);

    pSrcY1 = ppxliRefY1 + (iMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvX >> 2);
    pSrcU1 = ppxliRefU1 + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcV1 = ppxliRefV1 + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);

	if (mbType == BACKWARD)
	{
		pSrcY = pSrcY1; pSrcU = pSrcU1; pSrcV = pSrcV1;
	}

	if (mbType == FORWARD || mbType == BACKWARD)
	{
		// Y
		pWMVDec->m_pInterpolateMBRow ( pWMVDec, 
			pSrcY, pWMVDec->m_iWidthPrevY, ppxliReconY, pWMVDec->m_iWidthPrevY, iMvX, iMvY, 
			pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);

		// U0, U1
		pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
			pSrcU, pWMVDec->m_iWidthPrevUV, ppxliReconU, pWMVDec->m_iWidthPrevUV, iChromaMvX, iChromaMvY, 
			pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);

		// V0, V1
		pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
			pSrcV, pWMVDec->m_iWidthPrevUV, ppxliReconV, pWMVDec->m_iWidthPrevUV, iChromaMvX, iChromaMvY, 
			pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);
	}
	else
	{
		U8_WMV *ppxliForwardYMB = pWMVDec->m_pInterpY0, *ppxliBackwardYMB = pWMVDec->m_pInterpY1;
		U8_WMV *ppxliForwardUMB = pWMVDec->m_pInterpU0, *ppxliBackwardUMB = pWMVDec->m_pInterpU1;
		U8_WMV *ppxliForwardVMB = pWMVDec->m_pInterpV0, *ppxliBackwardVMB = pWMVDec->m_pInterpV1;

		if (mbType == DIRECT)
		{
            	I32_WMV    iFMVx, iFMVy, iFChromaMVx, iFChromaMVy;
			I32_WMV    iBMVx, iBMVy, iBChromaMVx, iBChromaMVy;

			//ChromaMVInterlace411 ( pWMVDec, imbX, imbY, iMvXdirect, iMvYdirect, &iChromaMvXdirect, &iChromaMvYdirect);
            DirectModeMV (pWMVDec, iMvXdirect, iMvYdirect, FALSE, imbX, imbY, &iFMVx, &iFMVy, &iBMVx, &iBMVy);

			ChromaMVInterlace411 (pWMVDec,imbX, imbY, iFMVx, iFMVy, &iFChromaMVx, &iFChromaMVy);
            ChromaMVInterlace411 (pWMVDec,imbX, imbY, iBMVx, iBMVy, &iBChromaMVx, &iBChromaMVy);


            // this is required for correct rounding in repeatpad area
			PullBackMotionVector (pWMVDec, &iFMVx, &iFMVy, imbX, imbY);
            PullBackMotionVector (pWMVDec, &iBMVx, &iBMVy, imbX, imbY);


			pSrcY = ppxliRefY + (iFMVy >> 2) * (pWMVDec->m_iWidthPrevY) + (iFMVx >> 2);
			pSrcU = ppxliRefU + (iFChromaMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iFChromaMVx >> 2);
			pSrcV = ppxliRefV + (iFChromaMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iFChromaMVx >> 2);

			pWMVDec->m_pInterpolateMBRow ( pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, ppxliForwardYMB, 16, iFMVx, iFMVy, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);
                    
			// U0, U1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcU, pWMVDec->m_iWidthPrevUV, ppxliForwardUMB, 4, iFChromaMVx, iFChromaMVy, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);

			// V0, V1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcV, pWMVDec->m_iWidthPrevUV, ppxliForwardVMB, 4, iFChromaMVx, iFChromaMVy, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);

			pSrcY1 = ppxliRefY1 + (iBMVy >> 2) * (pWMVDec->m_iWidthPrevY) + (iBMVx >> 2);
			pSrcU1 = ppxliRefU1 + (iBChromaMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iBChromaMVx >> 2);
			pSrcV1 = ppxliRefV1 + (iBChromaMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iBChromaMVx >> 2);

			pWMVDec->m_pInterpolateMBRow ( pWMVDec, pSrcY1, pWMVDec->m_iWidthPrevY, ppxliBackwardYMB, 16, iBMVx, iBMVy, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);

			// U0, U1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcU1, pWMVDec->m_iWidthPrevUV, ppxliBackwardUMB, 4, iBChromaMVx, iBChromaMVy, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);

			// V0, V1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcV1, pWMVDec->m_iWidthPrevUV, ppxliBackwardVMB, 4, iBChromaMVx, iBChromaMVy, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);


		}
		else if (mbType == INTERPOLATE)
		{
			ChromaMVInterlace411 ( pWMVDec, imbX, imbY, iMvXinterp, iMvYinterp, &iChromaMvXinterp, &iChromaMvYinterp);

            // this is required for correct rounding in repeatpad area
			PullBackMotionVector ( pWMVDec, &iMvXinterp, &iMvYinterp, imbX, imbY);
    
			pSrcY1 = ppxliRefY1 + (iMvYinterp >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvXinterp >> 2);
			pSrcU1 = ppxliRefU1 + (iChromaMvYinterp >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvXinterp >> 2);
			pSrcV1 = ppxliRefV1 + (iChromaMvYinterp >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvXinterp >> 2);
			pWMVDec->m_pInterpolateMBRow ( pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, ppxliForwardYMB, 16, iMvX, iMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);
                    
			// U0, U1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcU, pWMVDec->m_iWidthPrevUV, ppxliForwardUMB, 4, iChromaMvX, iChromaMvY, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);

			// V0, V1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcV, pWMVDec->m_iWidthPrevUV, ppxliForwardVMB, 4, iChromaMvX, iChromaMvY, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);


			pWMVDec->m_pInterpolateMBRow ( pWMVDec, pSrcY1, pWMVDec->m_iWidthPrevY, ppxliBackwardYMB, 16, iMvXinterp, iMvYinterp, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);

			// U0, U1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcU1, pWMVDec->m_iWidthPrevUV, ppxliBackwardUMB, 4, iChromaMvXinterp, iChromaMvYinterp, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);

			// V0, V1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcV1, pWMVDec->m_iWidthPrevUV, ppxliBackwardVMB, 4, iChromaMvXinterp, iChromaMvYinterp, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);

		}

        pWMVDec->m_pPixelMean (ppxliForwardYMB, 16, ppxliBackwardYMB, 16, 
            ppxliReconY, pWMVDec->m_iWidthPrevY, 16, 16);

        pWMVDec->m_pPixelMean (ppxliForwardUMB, 4, ppxliBackwardUMB, 4,
            ppxliReconU, pWMVDec->m_iWidthPrevUV, 4, 16); 

        pWMVDec->m_pPixelMean (ppxliForwardVMB, 4, ppxliBackwardVMB, 4,
            ppxliReconV, pWMVDec->m_iWidthPrevUV, 4, 16); 

	}
    return 0;
}

Void_WMV MotionCompTopFieldInterlace411_BFrame (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV, U8_WMV *ppxliRefY1, U8_WMV *ppxliRefU1, U8_WMV *ppxliRefV1,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV, MBType mbType)

{
    U8_WMV *pSrcY, *pSrcU, *pSrcV;
	U8_WMV *pSrcY1, *pSrcU1, *pSrcV1;
    I32_WMV iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    I32_WMV iMvX = pWMVDec->m_pXMotion [iMvIndex];
    I32_WMV iMvY = pWMVDec->m_pYMotion [iMvIndex];
	I32_WMV iMvXdirect = pWMVDec->m_pFieldMvX_Pbuf [iMvIndex];
	I32_WMV iMvYdirect = pWMVDec->m_pFieldMvY_Pbuf [iMvIndex];
    I32_WMV iChromaMvX, iChromaMvY;

	I32_WMV iMvXinterp = pWMVDec->m_pB411InterpX [iMvIndex];
	I32_WMV iMvYinterp = pWMVDec->m_pB411InterpY [iMvIndex];
	I32_WMV iChromaMvXinterp, iChromaMvYinterp;

    ChromaMVInterlace411 ( pWMVDec, imbX, imbY, iMvX, iMvY, &iChromaMvX, &iChromaMvY);

    // this is required for correct rounding in repeatpad area
    PullBackMotionVector ( pWMVDec, &iMvX, &iMvY, imbX, imbY);

    pSrcY = ppxliRefY + (iMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvX >> 2);
    pSrcU = ppxliRefU + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcV = ppxliRefV + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);

    pSrcY1 = ppxliRefY1 + (iMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvX >> 2);
    pSrcU1 = ppxliRefU1 + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcV1 = ppxliRefV1 + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);

	if (mbType == BACKWARD)
	{
		pSrcY = pSrcY1; pSrcU = pSrcU1; pSrcV = pSrcV1;
	}

	if (mbType == FORWARD || mbType == BACKWARD)
	{

		// top field Y
		pWMVDec->m_pInterpolateMBRow ( pWMVDec, 
			pSrcY, 2*pWMVDec->m_iWidthPrevY, ppxliReconY, 2*pWMVDec->m_iWidthPrevY, iMvX, iMvY, 
			pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);                     
                   
		// U0
		pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, pSrcU, 2*pWMVDec->m_iWidthPrevUV, ppxliReconU, 2*pWMVDec->m_iWidthPrevUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

		// V0
		pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, ppxliReconV, 2*pWMVDec->m_iWidthPrevUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
	}
	else
	{
		U8_WMV *ppxliForwardYMB = pWMVDec->m_pInterpY0, *ppxliBackwardYMB = pWMVDec->m_pInterpY1;
		U8_WMV *ppxliForwardUMB = pWMVDec->m_pInterpU0, *ppxliBackwardUMB = pWMVDec->m_pInterpU1;
		U8_WMV *ppxliForwardVMB = pWMVDec->m_pInterpV0, *ppxliBackwardVMB = pWMVDec->m_pInterpV1;

		if (mbType == DIRECT)
		{
            	I32_WMV    iFMVx, iFMVy, iFChromaMVx, iFChromaMVy;
			I32_WMV    iBMVx, iBMVy, iBChromaMVx, iBChromaMVy;
		//	ChromaMVInterlace411 ( pWMVDec, imbX, imbY, iMvXdirect, iMvYdirect, &iChromaMvXdirect, &iChromaMvYdirect);
            DirectModeMV (pWMVDec, iMvXdirect, iMvYdirect, FALSE, imbX, imbY, &iFMVx, &iFMVy, &iBMVx, &iBMVy);

			ChromaMVInterlace411 (pWMVDec, imbX, imbY, iFMVx, iFMVy, &iFChromaMVx, &iFChromaMVy);
            ChromaMVInterlace411 (pWMVDec, imbX, imbY, iBMVx, iBMVy, &iBChromaMVx, &iBChromaMVy);


            // this is required for correct rounding in repeatpad area

			PullBackMotionVector (pWMVDec, &iFMVx, &iFMVy, imbX, imbY);
            PullBackMotionVector (pWMVDec, &iBMVx, &iBMVy, imbX, imbY);


			pSrcY = ppxliRefY + (iFMVy >> 2) * (pWMVDec->m_iWidthPrevY) + (iFMVx >> 2);
			pSrcU = ppxliRefU + (iFChromaMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iFChromaMVx >> 2);
			pSrcV = ppxliRefV + (iFChromaMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iFChromaMVx >> 2);

			pWMVDec->m_pInterpolateMBRow ( pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, ppxliForwardYMB, 2*16, iFMVx, iFMVy, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
                    
			// U0, U1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcU, 2*pWMVDec->m_iWidthPrevUV, ppxliForwardUMB, 2*4, iFChromaMVx, iFChromaMVy, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			// V0, V1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcV, 2*pWMVDec->m_iWidthPrevUV, ppxliForwardVMB, 2*4, iFChromaMVx, iFChromaMVy, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			pSrcY1 = ppxliRefY1 + (iBMVy >> 2) * (pWMVDec->m_iWidthPrevY) + (iBMVx >> 2);
			pSrcU1 = ppxliRefU1 + (iBChromaMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iBChromaMVx >> 2);
			pSrcV1 = ppxliRefV1 + (iBChromaMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iBChromaMVx >> 2);

			pWMVDec->m_pInterpolateMBRow ( pWMVDec, pSrcY1, 2*pWMVDec->m_iWidthPrevY, ppxliBackwardYMB, 2*16, iBMVx, iBMVy, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			// U0, U1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcU1, 2*pWMVDec->m_iWidthPrevUV, ppxliBackwardUMB, 2*4, iBChromaMVx, iBChromaMVy, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			// V0, V1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcV1, 2*pWMVDec->m_iWidthPrevUV, ppxliBackwardVMB, 2*4, iBChromaMVx, iBChromaMVy, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);


		}
		else if (mbType == INTERPOLATE)
		{
			ChromaMVInterlace411 ( pWMVDec, imbX, imbY, iMvXinterp, iMvYinterp, &iChromaMvXinterp, &iChromaMvYinterp);

            // this is required for correct rounding in repeatpad area
			PullBackMotionVector ( pWMVDec, &iMvXinterp, &iMvYinterp, imbX, imbY);
    
			pSrcY1 = ppxliRefY1 + (iMvYinterp >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvXinterp >> 2);
			pSrcU1 = ppxliRefU1 + (iChromaMvYinterp >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvXinterp >> 2);
			pSrcV1 = ppxliRefV1 + (iChromaMvYinterp >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvXinterp >> 2);

			pWMVDec->m_pInterpolateMBRow ( pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, ppxliForwardYMB, 2*16, iMvX, iMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
                    
			// U0, U1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcU, 2*pWMVDec->m_iWidthPrevUV, ppxliForwardUMB, 2*4, iChromaMvX, iChromaMvY, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			// V0, V1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcV, 2*pWMVDec->m_iWidthPrevUV, ppxliForwardVMB, 2*4, iChromaMvX, iChromaMvY, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);


			pWMVDec->m_pInterpolateMBRow ( pWMVDec, pSrcY1, 2*pWMVDec->m_iWidthPrevY, ppxliBackwardYMB, 2*16, iMvXinterp, iMvYinterp, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			// U0, U1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcU1, 2*pWMVDec->m_iWidthPrevUV, ppxliBackwardUMB, 2*4, iChromaMvXinterp, iChromaMvYinterp, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			// V0, V1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcV1, 2*pWMVDec->m_iWidthPrevUV, ppxliBackwardVMB, 2*4, iChromaMvXinterp, iChromaMvYinterp, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

		}

        pWMVDec->m_pPixelMean (ppxliForwardYMB, 2*16, ppxliBackwardYMB, 2*16, 
            ppxliReconY, 2*pWMVDec->m_iWidthPrevY, 16, 8);

        pWMVDec->m_pPixelMean (ppxliForwardUMB, 2*4, ppxliBackwardUMB, 2*4,
            ppxliReconU, 2*pWMVDec->m_iWidthPrevUV, 4, 8); 

        pWMVDec->m_pPixelMean (ppxliForwardVMB, 2*4, ppxliBackwardVMB, 2*4,
            ppxliReconV, 2*pWMVDec->m_iWidthPrevUV, 4, 8); 

	}

}

Void_WMV MotionCompBotFieldInterlace411_BFrame (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY,
    U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV, U8_WMV *ppxliRefY1, U8_WMV *ppxliRefU1, U8_WMV *ppxliRefV1,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV, MBType mbType)
{ 
    U8_WMV *pSrcY, *pSrcU, *pSrcV;
    U8_WMV *pSrcY1, *pSrcU1, *pSrcV1;
    U8_WMV *pDstY, *pDstU, *pDstV;    
    I32_WMV iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2 + pWMVDec->m_uintNumMBX * 2;
    I32_WMV iMvX = pWMVDec->m_pXMotion [iMvIndex];
    I32_WMV iMvY = pWMVDec->m_pYMotion [iMvIndex];
	I32_WMV iMvXdirect = pWMVDec->m_pFieldMvX_Pbuf [iMvIndex];
	I32_WMV iMvYdirect = pWMVDec->m_pFieldMvY_Pbuf [iMvIndex];
    I32_WMV iChromaMvX, iChromaMvY;

	I32_WMV iMvXinterp = pWMVDec->m_pB411InterpX [iMvIndex];
	I32_WMV iMvYinterp = pWMVDec->m_pB411InterpY [iMvIndex];
	I32_WMV iChromaMvXinterp, iChromaMvYinterp;

    ChromaMVInterlace411 ( pWMVDec, imbX, imbY, iMvX, iMvY, &iChromaMvX, &iChromaMvY);

    // this is required for correct rounding in repeatpad area
    PullBackMotionVector ( pWMVDec, &iMvX, &iMvY, imbX, imbY);

    pSrcY = ppxliRefY + (iMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvX >> 2) + pWMVDec->m_iWidthPrevY;
    pSrcU = ppxliRefU + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2) + pWMVDec->m_iWidthPrevUV; 
    pSrcV = ppxliRefV + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2) + pWMVDec->m_iWidthPrevUV; 

    pDstY = ppxliReconY + pWMVDec->m_iWidthPrevY;
    pDstU = ppxliReconU + pWMVDec->m_iWidthPrevUV;
    pDstV = ppxliReconV + pWMVDec->m_iWidthPrevUV;

    pSrcY1 = ppxliRefY1 + (iMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvX >> 2) + pWMVDec->m_iWidthPrevY;
    pSrcU1 = ppxliRefU1 + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2) + pWMVDec->m_iWidthPrevUV; 
    pSrcV1 = ppxliRefV1 + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2) + pWMVDec->m_iWidthPrevUV; 

	if (mbType == BACKWARD)
	{
		pSrcY = pSrcY1; pSrcU = pSrcU1; pSrcV = pSrcV1;
	}

	if (mbType == FORWARD || mbType == BACKWARD)
	{

		// bot field Y
		pWMVDec->m_pInterpolateMBRow ( pWMVDec, 
			pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, iMvX, iMvY, 
			pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);                    

		// U1
		pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, pSrcU, 2*pWMVDec->m_iWidthPrevUV, pDstU, 2*pWMVDec->m_iWidthPrevUV,
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

		// V1
		pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, pDstV, 2*pWMVDec->m_iWidthPrevUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
	}
	else
	{
		U8_WMV *ppxliForwardY = pWMVDec->m_pInterpY0, *ppxliBackwardY = pWMVDec->m_pInterpY1;
		U8_WMV *ppxliForwardU = pWMVDec->m_pInterpU0, *ppxliBackwardU = pWMVDec->m_pInterpU1;
		U8_WMV *ppxliForwardV = pWMVDec->m_pInterpV0, *ppxliBackwardV = pWMVDec->m_pInterpV1;

		U8_WMV* ppxliForwardYMB = ppxliForwardY + 16; 
		U8_WMV* ppxliBackwardYMB = ppxliBackwardY + 16;
		U8_WMV* ppxliForwardUMB = ppxliForwardU + 4;
		U8_WMV* ppxliBackwardUMB = ppxliBackwardU + 4;
		U8_WMV* ppxliForwardVMB = ppxliForwardV + 4;
		U8_WMV* ppxliBackwardVMB = ppxliBackwardV + 4;

		if (mbType == DIRECT)
		{
            	I32_WMV    iFMVx, iFMVy, iFChromaMVx, iFChromaMVy;
			I32_WMV    iBMVx, iBMVy, iBChromaMVx, iBChromaMVy;

            DirectModeMV (pWMVDec, iMvXdirect, iMvYdirect, FALSE, imbX, imbY, &iFMVx, &iFMVy, &iBMVx, &iBMVy);

			ChromaMVInterlace411 (pWMVDec, imbX, imbY, iFMVx, iFMVy, &iFChromaMVx, &iFChromaMVy);
            ChromaMVInterlace411 (pWMVDec, imbX, imbY, iBMVx, iBMVy, &iBChromaMVx, &iBChromaMVy);


            // this is required for correct rounding in repeatpad area

			PullBackMotionVector (pWMVDec, &iFMVx, &iFMVy, imbX, imbY);
            PullBackMotionVector (pWMVDec, &iBMVx, &iBMVy, imbX, imbY);


			pSrcY = ppxliRefY + (iFMVy >> 2) * (pWMVDec->m_iWidthPrevY) + (iFMVx >> 2) + pWMVDec->m_iWidthPrevY;
			pSrcU = ppxliRefU + (iFChromaMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iFChromaMVx >> 2) + pWMVDec->m_iWidthPrevUV; 
			pSrcV = ppxliRefV + (iFChromaMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iFChromaMVx >> 2) + pWMVDec->m_iWidthPrevUV; 

			pWMVDec->m_pInterpolateMBRow ( pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, ppxliForwardYMB, 2*16, iFMVx, iFMVy, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
                    
			// U0, U1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcU, 2*pWMVDec->m_iWidthPrevUV, ppxliForwardUMB, 2*4, iFChromaMVx, iFChromaMVy, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			// V0, V1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcV, 2*pWMVDec->m_iWidthPrevUV, ppxliForwardVMB, 2*4, iFChromaMVx, iFChromaMVy, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			pSrcY1 = ppxliRefY1 + (iBMVy >> 2) * (pWMVDec->m_iWidthPrevY) + (iBMVx >> 2) + pWMVDec->m_iWidthPrevY;
			pSrcU1 = ppxliRefU1 + (iBChromaMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iBChromaMVx >> 2) + pWMVDec->m_iWidthPrevUV; 
			pSrcV1 = ppxliRefV1 + (iBChromaMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iBChromaMVx >> 2) + pWMVDec->m_iWidthPrevUV; 

			pWMVDec->m_pInterpolateMBRow ( pWMVDec, pSrcY1, 2*pWMVDec->m_iWidthPrevY, ppxliBackwardYMB, 2*16, iBMVx, iBMVy, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			// U0, U1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcU1, 2*pWMVDec->m_iWidthPrevUV, ppxliBackwardUMB, 2*4, iBChromaMVx, iBChromaMVy, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			// V0, V1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcV1, 2*pWMVDec->m_iWidthPrevUV, ppxliBackwardVMB, 2*4, iBChromaMVx, iBChromaMVy, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);


		}
		else if (mbType == INTERPOLATE)
		{
			ChromaMVInterlace411 ( pWMVDec, imbX, imbY, iMvXinterp, iMvYinterp, &iChromaMvXinterp, &iChromaMvYinterp);

            // this is required for correct rounding in repeatpad area
			PullBackMotionVector ( pWMVDec, &iMvXinterp, &iMvYinterp, imbX, imbY);
    
			pSrcY1 = ppxliRefY1 + (iMvYinterp >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvXinterp >> 2) + pWMVDec->m_iWidthPrevY;
			pSrcU1 = ppxliRefU1 + (iChromaMvYinterp >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvXinterp >> 2) + pWMVDec->m_iWidthPrevUV; 
			pSrcV1 = ppxliRefV1 + (iChromaMvYinterp >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvXinterp >> 2) + pWMVDec->m_iWidthPrevUV; 

			pWMVDec->m_pInterpolateMBRow ( pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, ppxliForwardYMB, 2*16, iMvX, iMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
                    
			// U0, U1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcU, 2*pWMVDec->m_iWidthPrevUV, ppxliForwardUMB, 2*4, iChromaMvX, iChromaMvY, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			// V0, V1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcV, 2*pWMVDec->m_iWidthPrevUV, ppxliForwardVMB, 2*4, iChromaMvX, iChromaMvY, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);


			pWMVDec->m_pInterpolateMBRow ( pWMVDec, pSrcY1, 2*pWMVDec->m_iWidthPrevY, ppxliBackwardYMB, 2*16, iMvXinterp, iMvYinterp, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			// U0, U1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcU1, 2*pWMVDec->m_iWidthPrevUV, ppxliBackwardUMB, 2*4, iChromaMvXinterp, iChromaMvYinterp, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

			// V0, V1
			pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, 
				pSrcV1, 2*pWMVDec->m_iWidthPrevUV, ppxliBackwardVMB, 2*4, iChromaMvXinterp, iChromaMvYinterp, 
				pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

		}

        pWMVDec->m_pPixelMean (ppxliForwardYMB, 2*16, ppxliBackwardYMB, 2*16, 
            pDstY, 2*pWMVDec->m_iWidthPrevY, 16, 8);

        pWMVDec->m_pPixelMean (ppxliForwardUMB, 2*4, ppxliBackwardUMB, 2*4,
            pDstU, 2*pWMVDec->m_iWidthPrevUV, 4, 8); 

        pWMVDec->m_pPixelMean (ppxliForwardVMB, 2*4, ppxliBackwardVMB, 2*4,
            pDstV, 2*pWMVDec->m_iWidthPrevUV, 4, 8); 
	}
}


Void_WMV MotionCompTopFieldInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV)
{
    U8_WMV *pSrcY, *pSrcU, *pSrcV;
    I32_WMV iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    I32_WMV iMvX = pWMVDec->m_pXMotion [iMvIndex];
    I32_WMV iMvY = pWMVDec->m_pYMotion [iMvIndex];
    I32_WMV iChromaMvX, iChromaMvY;

    ChromaMVInterlace411 ( pWMVDec, imbX, imbY, iMvX, iMvY, &iChromaMvX, &iChromaMvY);

    // this is required for correct rounding in repeatpad area
    PullBackMotionVector ( pWMVDec, &iMvX, &iMvY, imbX, imbY);

    pSrcY = ppxliRefY + (iMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvX >> 2);
    pSrcU = ppxliRefU + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcV = ppxliRefV + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);

    // top field Y
    pWMVDec->m_pInterpolateMBRow ( pWMVDec, 
        pSrcY, 2*pWMVDec->m_iWidthPrevY, ppxliReconY, 2*pWMVDec->m_iWidthPrevY, iMvX, iMvY, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);                     
                   
    // U0
    pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, pSrcU, 2*pWMVDec->m_iWidthPrevUV, ppxliReconU, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

    // V0
    pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, ppxliReconV, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

}



Void_WMV MotionCompBotFieldInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY,
    U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV)
{ 
    U8_WMV *pSrcY, *pSrcU, *pSrcV;
    U8_WMV *pDstY, *pDstU, *pDstV;    
    I32_WMV iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2 + pWMVDec->m_uintNumMBX * 2;
    I32_WMV iMvX = pWMVDec->m_pXMotion [iMvIndex];
    I32_WMV iMvY = pWMVDec->m_pYMotion [iMvIndex];
    I32_WMV iChromaMvX, iChromaMvY;

    ChromaMVInterlace411 ( pWMVDec, imbX, imbY, iMvX, iMvY, &iChromaMvX, &iChromaMvY);

    // this is required for correct rounding in repeatpad area
    PullBackMotionVector ( pWMVDec, &iMvX, &iMvY, imbX, imbY);

    pSrcY = ppxliRefY + (iMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvX >> 2) + pWMVDec->m_iWidthPrevY;
    pSrcU = ppxliRefU + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2) + pWMVDec->m_iWidthPrevUV; 
    pSrcV = ppxliRefV + (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2) + pWMVDec->m_iWidthPrevUV; 

    pDstY = ppxliReconY + pWMVDec->m_iWidthPrevY;
    pDstU = ppxliReconU + pWMVDec->m_iWidthPrevUV;
    pDstV = ppxliReconV + pWMVDec->m_iWidthPrevUV;

    // bot field Y
    pWMVDec->m_pInterpolateMBRow ( pWMVDec, 
        pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, iMvX, iMvY, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);                    

    // U1
    pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, pSrcU, 2*pWMVDec->m_iWidthPrevUV, pDstU, 2*pWMVDec->m_iWidthPrevUV,
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

    // V1
    pWMVDec->m_pInterpolate4ByteRow ( pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, pDstV, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
}


// these are C-routines. We know xbox has mmx, so we take out these C code.
Void_WMV g_FieldAddError8x8 (I16_WMV *pSrc, U8_WMV *pDst, I32_WMV iDstOffset)
{
    I32_WMV i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            I16_WMV iTmp = pSrc [j] + pDst [j];
            iTmp = CLIP (iTmp);
            pDst [j] = (U8_WMV) iTmp;
        }
        pDst += iDstOffset;
        pSrc += 8;
    }
}

Void_WMV g_FieldAddError4x8 (I16_WMV *pSrc, U8_WMV *pDst, I32_WMV iDstOffset)
{
    I32_WMV i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            I16_WMV iTmp = pSrc [j] + pDst [j];
            iTmp = CLIP (iTmp);
            pDst [j] = (U8_WMV) iTmp;
        }
        pDst += iDstOffset;
        pSrc += 4;
    }
}

// iNumCoef is either 32 or 64
//Void_WMV g_FieldBlockSub128 (I16_WMV *pBlock, I32_WMV iNumCoef) 
//{
//    I32_WMV i;
//    for (i = 0; i < iNumCoef; i++) {
//        pBlock [i] -= 128;
//    }
//}

// iNumCoef is either 32 or 64
Void_WMV g_FieldBlockAdd128 (I16_WMV *pBlock, I32_WMV iNumCoef) 
{
    I32_WMV i;
    for (i = 0; i < iNumCoef; i++) {
        pBlock [i] += 128;
    }
}

//Void_WMV g_CopyMBRow (U8_WMV *pSrc, I32_WMV iSrcStride, 
//    U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iNumRows)
//{
//    I32_WMV i;
//    for (i = 0; i < iNumRows; i++) {
//        memcpy (pDst, pSrc, 16);
//        pDst += iDstStride;
//        pSrc += iSrcStride;
//    }
//}

Void_WMV g_Copy4ByteRow (U8_WMV *pSrc, I32_WMV iSrcStride, 
    U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iNumRows)
{
    I32_WMV i;
    for (i = 0; i < iNumRows; i++) {
        memcpy (pDst, pSrc, 4);
        pDst += iDstStride;
        pSrc += iSrcStride;
    }
}


extern const I16_WMV gaSubsampleBicubic[];

Void_WMV  g_InterpolateFieldMB (tWMVDecInternalMember *pWMVDec, U8_WMV *pSrc, I32_WMV iSrcStride, 
    U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac,
    I32_WMV iFilterType, I32_WMV iRndCtrl, I32_WMV iNumRows)
{     
    I32_WMV i, j, k;
    const I16_WMV *pH;
    const I16_WMV *pV;

    //assert (iFilterType == FILTER_BICUBIC);
	if(iFilterType != FILTER_BICUBIC)
		return;
    iYFrac &= 3;
    iXFrac &= 3;    
    pH = gaSubsampleBicubic + 4 * iXFrac;
    pV = gaSubsampleBicubic + 4 * iYFrac;
    
    if (iXFrac == 0 && iYFrac == 0) {
        for (i = 0; i < iNumRows; i++) {
            memcpy (pDst, pSrc, 16 * sizeof(U8_WMV));
            pDst += iDstStride;
            pSrc += iSrcStride;
        }
    } else if (iXFrac == 0) {
        // vertical filtering only
        I32_WMV iShift = (iYFrac == 2) ? 4 : 6;
        I32_WMV iRound = (1 << (iShift - 1)) - 1 + iRndCtrl;
        
        for (i = 0; i < iNumRows; i++) {
            for (j = 0; j < 16; j++) {
                U8_WMV  *pT = pSrc + iSrcStride * (i - 1) + j;
                k = (pT[0] * pV[0] + pT[iSrcStride] * pV[1] + pT[2 * iSrcStride] * pV[2]
                    + pT[3 * iSrcStride] * pV[3] + iRound) >> iShift;
                if (k < 0)
                    k = 0;
                else if (k > 255)
                    k = 255;
                pDst[iDstStride * i + j] = (U8_WMV) k;
            }
        }
    } else if (iYFrac == 0) {
        // horizontal filtering only
        I32_WMV iShift = (iXFrac == 2) ? 4 : 6;
        I32_WMV iRound = (1 << (iShift - 1)) - iRndCtrl;
        
        for (i = 0; i < iNumRows; i++) {
            for (j = 0; j < 16; j++) {
                U8_WMV  *pT = pSrc + iSrcStride * i + j - 1;
                k = (pT[0] * pH[0] + pT[1] * pH[1] + pT[2] * pH[2] + pT[3] * pH[3] + 
                    iRound) >> iShift;
                if (k < 0)
                    k = 0;
                else if (k > 255)
                    k = 255;
                pDst[iDstStride * i + j] = (U8_WMV) k;
            }
        }
    } else {
        const I32_WMV pFilterRowStride = 32;
        I16_WMV pFilter[16 * 32 + 32]; // need 16 rows by 19 columns
        I16_WMV* pF = (I16_WMV*) (((VO_U32)pFilter + 31) & ~31); // reset
        I32_WMV iShift, iRound1, iRound2;

        iShift = ((iXFrac == 2) ? 4 : 6) + ((iYFrac == 2) ? 4 : 6);
        iShift -= 7;
        iRound1 = (1 << (iShift - 1)) - 1 + iRndCtrl;
        iRound2 = (1 << 6) - iRndCtrl;

        for (i = 0; i < iNumRows; i++) {
            for (j = 0; j < 19; j++) {
                U8_WMV  *pT = pSrc - iSrcStride - 1 + j;
                pF[j] = (pT[0] * pV[0] + pT[iSrcStride] * pV[1] + pT[2 * iSrcStride] * pV[2]
                    + pT[3 * iSrcStride] * pV[3] + iRound1) >> iShift;
            }
            pSrc += iSrcStride;
            pF += pFilterRowStride;
        }

        pF = (I16_WMV*) (((VO_U32)pFilter + 31) & ~31); // reset        
        for (i = 0; i < iNumRows; i++) {
            for (j = 0; j < 16; j++) {
                k = (pF[j] * pH[0] + pF[1 + j] * pH[1] + pF[2 + j] * pH[2]
                    + pF[3 + j] * pH[3] + iRound2) >> 7;
                if (k < 0)
                    k = 0;
                else if (k > 255)
                    k = 255;
                pDst[j] = (U8_WMV) k;                
            }
            pF += pFilterRowStride;
            pDst += iDstStride;
        }            
    }
}

Void_WMV g_Interpolate4ByteRow (tWMVDecInternalMember *pWMVDec, U8_WMV *pSrc, I32_WMV iSrcStride, 
    U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac,
    I32_WMV iFilterType, I32_WMV iRndCtrl, I32_WMV iNumRows)
{     
    I32_WMV i, j, k;
    const I16_WMV *pH;
    const I16_WMV *pV;

    //assert (iFilterType == FILTER_BICUBIC);
	if(iFilterType != FILTER_BICUBIC)
		return;
    iYFrac &= 3;
    iXFrac &= 3;    
    pH = gaSubsampleBicubic + 4 * iXFrac;
    pV = gaSubsampleBicubic + 4 * iYFrac;
    
    if (iXFrac == 0 && iYFrac == 0) {
        g_Copy4ByteRow (pSrc, iSrcStride, 
            pDst, iDstStride, iNumRows);
    } else if (iXFrac == 0) {
        // vertical filtering only
        I32_WMV iShift = (iYFrac == 2) ? 4 : 6;
        I32_WMV iRound = (1 << (iShift - 1)) - 1 + iRndCtrl;
        
        for (i = 0; i < iNumRows; i++) {
            for (j = 0; j < 4; j++) {
                U8_WMV  *pT = pSrc + iSrcStride * (i - 1) + j;
                k = (pT[0] * pV[0] + pT[iSrcStride] * pV[1] + pT[2 * iSrcStride] * pV[2]
                    + pT[3 * iSrcStride] * pV[3] + iRound) >> iShift;
                if (k < 0)
                    k = 0;
                else if (k > 255)
                    k = 255;
                pDst[iDstStride * i + j] = (U8_WMV) k;
            }
        }
    } else if (iYFrac == 0) {
        // horizontal filtering only
        I32_WMV iShift = (iXFrac == 2) ? 4 : 6;
        I32_WMV iRound = (1 << (iShift - 1)) - iRndCtrl;
        
        for (i = 0; i < iNumRows; i++) {
            for (j = 0; j < 4; j++) {
                U8_WMV  *pT = pSrc + iSrcStride * i + j - 1;
                k = (pT[0] * pH[0] + pT[1] * pH[1] + pT[2] * pH[2] + pT[3] * pH[3] + 
                    iRound) >> iShift;
                if (k < 0)
                    k = 0;
                else if (k > 255)
                    k = 255;
                pDst[iDstStride * i + j] = (U8_WMV) k;
            }
        }
    } else {
        const I32_WMV pFilterRowStride = 32;
        I16_WMV pFilter[16 * 32 + 32]; // need 16 rows by 19 columns
        I16_WMV* pF = (I16_WMV*) (((VO_U32)pFilter + 31) & ~31); // reset
        I32_WMV iShift, iRound1, iRound2;

        iShift = ((iXFrac == 2) ? 4 : 6) + ((iYFrac == 2) ? 4 : 6);
        iShift -= 7;
        iRound1 = (1 << (iShift - 1)) - 1 + iRndCtrl;
        iRound2 = (1 << 6) - iRndCtrl;
        
        for (i = 0; i < iNumRows; i++) {
            for (j = 0; j < 4+3; j++) {
                U8_WMV  *pT = pSrc - iSrcStride - 1 + j;
                pF[j] = (pT[0] * pV[0] + pT[iSrcStride] * pV[1] + pT[2 * iSrcStride] * pV[2]
                    + pT[3 * iSrcStride] * pV[3] + iRound1) >> iShift;
            }
            pSrc += iSrcStride;
            pF += pFilterRowStride;
        }

        pF = (I16_WMV*) (((VO_U32)pFilter + 31) & ~31); // reset        
        for (i = 0; i < iNumRows; i++) {
            for (j = 0; j < 4; j++) {
                k = (pF[j] * pH[0] + pF[1 + j] * pH[1] + pF[2 + j] * pH[2]
                    + pF[3 + j] * pH[3] + iRound2) >> 7;
                if (k < 0)
                    k = 0;
                else if (k > 255)
                    k = 255;
                pDst[j] = (U8_WMV) k;                
            }
            pF += pFilterRowStride;
            pDst += iDstStride;
        }            
    }
}

static Void_WMV g_InterpolateBlockRow (tWMVDecInternalMember *pWMVDec, U8_WMV *pSrc, I32_WMV iSrcStride, 
    U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac,
    I32_WMV iFilterType, I32_WMV iRndCtrl, I32_WMV iNumRows)
{     
    I32_WMV i, j, k;
    const I16_WMV *pH;
    const I16_WMV *pV;

    //assert (iFilterType == FILTER_BICUBIC);
	if(iFilterType != FILTER_BICUBIC)
		return;
    iYFrac &= 3;
    iXFrac &= 3;    
    pH = gaSubsampleBicubic + 4 * iXFrac;
    pV = gaSubsampleBicubic + 4 * iYFrac;
    
    if (iXFrac == 0 && iYFrac == 0) {
        for (i = 0; i < iNumRows; i++) {
            memcpy (pDst, pSrc, 8 * sizeof(U8_WMV));
            pDst += iDstStride;
            pSrc += iSrcStride;
        }
    } else if (iXFrac == 0) {
        // vertical filtering only
        I32_WMV iShift = (iYFrac == 2) ? 4 : 6;
        I32_WMV iRound = (1 << (iShift - 1)) - 1 + iRndCtrl;
        
        for (i = 0; i < iNumRows; i++) {
            for (j = 0; j < 8; j++) {
                U8_WMV  *pT = pSrc + iSrcStride * (i - 1) + j;
                k = (pT[0] * pV[0] + pT[iSrcStride] * pV[1] + pT[2 * iSrcStride] * pV[2]
                    + pT[3 * iSrcStride] * pV[3] + iRound) >> iShift;
                if (k < 0)
                    k = 0;
                else if (k > 255)
                    k = 255;
                pDst[iDstStride * i + j] = (U8_WMV) k;
            }
        }
    } else if (iYFrac == 0) {
        // horizontal filtering only
        I32_WMV iShift = (iXFrac == 2) ? 4 : 6;
        I32_WMV iRound = (1 << (iShift - 1)) - iRndCtrl;
        
        for (i = 0; i < iNumRows; i++) {
            for (j = 0; j < 8; j++) {
                U8_WMV  *pT = pSrc + iSrcStride * i + j - 1;
                k = (pT[0] * pH[0] + pT[1] * pH[1] + pT[2] * pH[2] + pT[3] * pH[3] + 
                    iRound) >> iShift;
                if (k < 0)
                    k = 0;
                else if (k > 255)
                    k = 255;
                pDst[iDstStride * i + j] = (U8_WMV) k;
            }
        }
    } else {
        const I32_WMV pFilterRowStride = 32;
        I16_WMV pFilter[16 * 32 + 32]; // need 16 rows by 19 columns
        I16_WMV* pF = (I16_WMV*) (((I32_WMV)pFilter + 31) & ~31); // reset
        I32_WMV iShift, iRound1, iRound2;
		U8_WMV  *pT;

        iShift = ((iXFrac == 2) ? 4 : 6) + ((iYFrac == 2) ? 4 : 6);
        iShift -= 7;
        iRound1 = (1 << (iShift - 1)) - 1 + iRndCtrl;
        iRound2 = (1 << 6) - iRndCtrl;
        
        for (i = 0; i < iNumRows; i++) {
            for (j = 0; j < 8+3; j++) {
                pT = pSrc - iSrcStride - 1 + j;
                pF[j] = (I16_WMV)((pT[0] * pV[0] + pT[iSrcStride] * pV[1] + pT[2 * iSrcStride] * pV[2]
                    + pT[3 * iSrcStride] * pV[3] + iRound1) >> iShift);
            }
            pSrc += iSrcStride;
            pF += pFilterRowStride;
        }

        pF = (I16_WMV*) (((I32_WMV)pFilter + 31) & ~31); // reset        
        for (i = 0; i < iNumRows; i++) {
            for (j = 0; j < 8; j++) {
                k = (pF[j] * pH[0] + pF[1 + j] * pH[1] + pF[2 + j] * pH[2]
                    + pF[3 + j] * pH[3] + iRound2) >> 7;
                if (k < 0)
                    k = 0;
                else if (k > 255)
                    k = 255;
                pDst[j] = (U8_WMV) k;                
            }
            pF += pFilterRowStride;
            pDst += iDstStride;
        }            
    }
}

static I16_WMV gaSubsampleBilinear[] = {
    4, 0,       3, 1,
    2, 2,      1, 3,
};

static Void_WMV g_InterpolateBlockRowBilinear (tWMVDecInternalMember *pWMVDec, U8_WMV *pSrc, I32_WMV iSrcStride, 
    U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac,
    I32_WMV iFilterType, I32_WMV iRndCtrl, I32_WMV iNumRows)
{     
    I32_WMV i, j, k;
    const I32_WMV iWidth = 8;
    I32_WMV pFilter[32 + 1];
    I16_WMV *pH;
    I16_WMV *pV;

    iYFrac &= 3;
    iXFrac &= 3;    
    pH = gaSubsampleBilinear + 2 * iXFrac;
    pV = gaSubsampleBilinear + 2 * iYFrac;

    for (j = 0; j < iWidth; j++) {
        for (i = 0; i < iNumRows + 1; i++) {
            const U8_WMV  *pT = pSrc + i * iSrcStride;
            pFilter[i] = pT[0] * pH[0] + pT[1] * pH[1];
        }
        for (i = 0; i < iNumRows; i++) {
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

static Void_WMV g_Interpolate4ByteRowBilinear (tWMVDecInternalMember *pWMVDec, U8_WMV *pSrc, I32_WMV iSrcStride, 
    U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac,
    I32_WMV iFilterType, I32_WMV iRndCtrl, I32_WMV iNumRows)
{     
    I32_WMV i, j, k;
    const I32_WMV iWidth = 4;
    I32_WMV pFilter[32 + 1];
    I16_WMV *pH;
    I16_WMV *pV;
    iYFrac &= 3;
    iXFrac &= 3;    
    pH = gaSubsampleBilinear + 2 * iXFrac;
    pV = gaSubsampleBilinear + 2 * iYFrac;

    for (j = 0; j < iWidth; j++) {
        for (i = 0; i < iNumRows + 1; i++) {
            const U8_WMV  *pT = pSrc + i * iSrcStride;
            pFilter[i] = pT[0] * pH[0] + pT[1] * pH[1];
        }
        for (i = 0; i < iNumRows; i++) {
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

Void_WMV InitFieldInterpolation (tWMVDecInternalMember *pWMVDec)
{

    pWMVDec->m_pInterpolateMBRow = g_InterpolateFieldMB;
    pWMVDec->m_pInterpolate4ByteRow = g_Interpolate4ByteRow;
    //pWMVDec->m_pCopyMBRow = g_CopyMBRow;
    
    pWMVDec->m_pInterpolateBlockRow = g_InterpolateBlockRow;
    pWMVDec->m_pInterpolateBlockRowBilinear = g_InterpolateBlockRowBilinear;
    pWMVDec->m_pInterpolate4ByteRowBilinear = g_Interpolate4ByteRowBilinear;

    //pWMVDec->m_pFieldBlockSub128 = g_FieldBlockSub128;
    pWMVDec->m_pFieldBlockAdd128 = g_FieldBlockAdd128;
    pWMVDec->m_pFieldAddError8x8 = g_FieldAddError8x8;
    pWMVDec->m_pFieldAddError4x8 = g_FieldAddError4x8;

}

Void_WMV LoopFilterIFrameInterlace411 (tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV)
{
    U8_WMV* pCurr;
    U32_WMV imbX, imbY;
    U32_WMV uiMBStart = 0;
    U32_WMV uiMBEnd  = pWMVDec->m_uintNumMBY;

    // Filter Y boundary columns
    ppxliY = ppxliY + 8 - 5;
    for (imbY = uiMBStart; imbY < uiMBEnd; imbY++) {
        pCurr = ppxliY + imbY * (16 * pWMVDec->m_iWidthPrevY);
        for (imbX = 0; imbX < 2 * pWMVDec->m_uintNumMBX - 1; imbX++) {
            (*pWMVDec->m_pFilterVerticalEdge) (pCurr, pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 16);
            pCurr += 8;                        
        }
    }

    // Filter U boundary columns
    ppxliU = ppxliU + 4 - 5;
    for (imbY = uiMBStart; imbY < uiMBEnd; imbY++) {
        pCurr = ppxliU + imbY * (16 * pWMVDec->m_iWidthPrevUV); 
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX - 1; imbX++) {            
            (*pWMVDec->m_pFilterVerticalEdge) (pCurr, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 16);
            pCurr += 4;
        }
    }

    // Filter V boundary columns
    ppxliV = ppxliV + 4 - 5;
    for (imbY = uiMBStart; imbY < uiMBEnd; imbY++) {
        pCurr = ppxliV + imbY * (16 * pWMVDec->m_iWidthPrevUV); 
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX - 1; imbX++) {            
            (*pWMVDec->m_pFilterVerticalEdge) (pCurr, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 16);
            pCurr += 4;
        }
    }
}
