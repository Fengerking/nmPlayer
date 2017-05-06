//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) 1996  Microsoft Corporation

Module Name:

        vopSes.cpp

Abstract:

        Base class for the encoder for one VOP session.

Author:

        Ming-Chieh Lee (mingcl@microsoft.com) 20-March-1996
        Chuang Gu (chuanggu@microsoft.com) 10-December-1999

Revision History:

*************************************************************************/
#include "xplatform_wmv.h"
#include "typedef.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "voWmvPort.h"
#include "motioncomp_wmv.h"
#include "tables_wmv.h"
#include "repeatpad_wmv.h"

#define phi1(a) ((U32_WMV)(a+iThr1) <= uThr2)

int EXPANDY_REFVOP, EXPANDUV_REFVOP;

#ifdef UNDER_CE
    extern SetKMode ();
#endif

Void_WMV DeblockMBRow_IVOP (tWMVDecInternalMember *pWMVDec,
                                      U8_WMV* ppxliY, 
                                      U8_WMV* ppxliU, 
                                      U8_WMV* ppxliV,
									  U32_WMV nMBs)
{
	I32_WMV iMBEndX = pWMVDec->m_uintNumMBX;
	I32_WMV iMBEndY = pWMVDec->m_uintNumMBY;
    I32_WMV  iStep = pWMVDec->m_iStepSize;
	Bool_WMV  bFlag0            = (nMBs == 0) || pWMVDec->m_pbStartOfSliceRow [ nMBs ];
	Bool_WMV  bFlag1            = ( ((nMBs + 1) == iMBEndY)) || pWMVDec->m_pbStartOfSliceRow[nMBs + 1];

    FilterEdgeShortTagMBRow    (pWMVDec, ppxliY, pWMVDec->m_iWidthPrevY,  iStep, iMBEndX, bFlag0, bFlag1);
    FilterEdgeShortTagBlockRow (pWMVDec, ppxliU, pWMVDec->m_iWidthPrevUV, iStep, iMBEndX, bFlag0, bFlag1);
    FilterEdgeShortTagBlockRow (pWMVDec, ppxliV, pWMVDec->m_iWidthPrevUV, iStep, iMBEndX, bFlag0, bFlag1);
}


Void_WMV DeblockSLFrame (tWMVDecInternalMember *pWMVDec,
                                      U8_WMV* ppxliY, 
                                      U8_WMV* ppxliU, 
                                      U8_WMV* ppxliV)// Frame-based deblocking
{
	I32_WMV iMBEndY = pWMVDec->m_uintNumMBY;
    I32_WMV nMBs = 0;  

    for ( nMBs = 0; nMBs < iMBEndY; nMBs ++) {
		DeblockMBRow_IVOP (pWMVDec,
                                  ppxliY + nMBs*pWMVDec->m_iMBSizeXWidthPrevY, 
                                  ppxliU + nMBs*pWMVDec->m_iBlkSizeXWidthPrevUV, 
                                  ppxliV + nMBs*pWMVDec->m_iBlkSizeXWidthPrevUV,
								  nMBs);
    }
  
}


Void_WMV FilterEdgeShortTagMBRow(tWMVDecInternalMember *pWMVDec, 
                                                 U8_WMV* ppxlcCenter, 
                                                 I32_WMV iPixelDistance, 
                                                 I32_WMV iStepSize, 
                                                 I32_WMV iMBsPerRow, 
                                                 Bool_WMV bFirst, 
                                                 Bool_WMV bLast)
{
    U8_WMV* pVhstart, *pVhstart2, *pVhstart2x;
    U8_WMV* pV5 = ppxlcCenter + 8*iPixelDistance, *pV5x = pV5 + 8*iPixelDistance;
    U8_WMV* pV5_2 = pV5 - 4*iPixelDistance, *pV5x_2 = pV5_2 + 8*iPixelDistance;

    I32_WMV iVertSize = 16;
    I32_WMV iVertOffset = 4;

    if (bFirst)
    {
        iVertSize = 20;
        iVertOffset = 0;
    }

    if (bLast)
            iVertSize -= 4;

    else
    {

        (*pWMVDec->m_pFilterHorizontalEdge)(pV5x, iPixelDistance, iStepSize, 4);
        pV5x += 4;
    }
    (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 4);
    pV5 += 4;

    // Loop through the macroblocks in the row filtering horiz. line first then vertical
    pVhstart = ppxlcCenter + iVertOffset*iPixelDistance + 3;
    pVhstart2 = ppxlcCenter - 1;
    pVhstart2x = pVhstart2 + 8*iPixelDistance;

    if (!bLast)
    {
        I32_WMV imbX ;
        for ( imbX = 0; imbX < iMBsPerRow - 1; imbX++) {
            (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 16);
            (*pWMVDec->m_pFilterHorizontalEdge)(pV5x, iPixelDistance, iStepSize, 16);
            (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize, iVertSize);
            (*pWMVDec->m_pFilterVerticalEdge)(pVhstart + 8, iPixelDistance, iStepSize, iVertSize);
            pV5 += 16;
            pV5x += 16;
            pVhstart += 16;
            pV5_2 += 16;
            pV5x_2 += 16;
            pVhstart2 += 16;
            pVhstart2x += 16;
        }
    }
    else
    {
        I32_WMV imbX ;
        for ( imbX = 0; imbX < iMBsPerRow - 1; imbX++) {
            (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 16);
            (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize, iVertSize);
            (*pWMVDec->m_pFilterVerticalEdge)(pVhstart + 8, iPixelDistance, iStepSize, iVertSize);
            pV5 += 16;
            pVhstart += 16;
            pV5_2 += 16;
            pV5x_2 += 16;
            pVhstart2 += 16;
            pVhstart2x += 16;
        }
    }

    (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 12);
    if (!bLast)
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5x, iPixelDistance, iStepSize, 12);
    (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize, iVertSize);

}

// Filter 8x8 boundaries for a block row
Void_WMV FilterEdgeShortTagBlockRow(tWMVDecInternalMember *pWMVDec, 
                                                U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                                                I32_WMV iBlocksPerRow, Bool_WMV bFirst, Bool_WMV bLast)
{    
    U8_WMV *pVhstart, *pVhstart2;
    U8_WMV *pV5 = ppxlcCenter + iPixelDistance*8, *pV5_2 = pV5 - iPixelDistance*4;

    I32_WMV iVertSize = 8;
    I32_WMV iVertOffset = 4;

    if (bFirst)
    {
        iVertSize = 12;
        iVertOffset = 0;
    }
    if (bLast)
        iVertSize -= 4;
    else
    {
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 4);
        pV5 += 4;
    }

    pVhstart = ppxlcCenter + iPixelDistance*iVertOffset + 3;
    pVhstart2 = ppxlcCenter - 1;

    // Loop through the blocks in the row filtering horiz line first then vertical
    if (!bLast)
    {
        I32_WMV iblkX;
        for ( iblkX = 0; iblkX < iBlocksPerRow - 1; iblkX++) 
        {
            (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 8);
            (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize,  iVertSize);
            pV5 += 8;
            pV5_2 += 8;
            pVhstart += 8;
            pVhstart2 += 8;
        }
    }
    else
    {
        I32_WMV iblkX;
        for ( iblkX = 0; iblkX < iBlocksPerRow - 1; iblkX++) 
        {
            (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize,  iVertSize);
            pV5_2 += 8;
            pVhstart += 8;
            pVhstart2 += 8;
        }
    }

    // Filter last four pixels in the horizontal line
    if (!bLast)
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 4);
}


static const I32_WMV s_iInv[] = {
    0, 262144, 131072, 87381, 65536, 52429, 43691, 37449,
    32768, 29127, 26214, 23831, 21845, 20165, 18725, 17476,
    16384, 15420, 14564, 13797, 13107, 12483, 11916, 11398,
    10923, 10486, 10082, 9709, 9362, 9039, 8738, 8456 ,
    8192, 7944, 7710, 7490, 7282, 7085, 6899, 6722,
    6554, 6394, 6242, 6096, 5958, 5825, 5699, 5578, 
    5461, 5350, 5243, 5140, 5041, 4946, 4855, 4766, 
    4681, 4599, 4520, 4443, 4369, 4297, 4228, 4161};



I32_WMV divroundnearest(I32_WMV i, I32_WMV iDenom)
{
    //assert(iDenom > 0 && iDenom < 64);
	if(iDenom <= 0 || iDenom >= 64)
		return 0;
    return ((i * s_iInv[iDenom] + 0x20000) >> 18);

}

Void_WMV SetDefaultDQuantSetting(tWMVDecInternalMember *pWMVDec)
{
    pWMVDec->m_bDQuantOn = FALSE_WMV;
    pWMVDec->m_bDQuantBiLevel = FALSE_WMV;
    pWMVDec->m_iPanning = 0;
    pWMVDec->m_iDQuantBiLevelStepSize = pWMVDec->m_iStepSize;
    return; 
}



I8_WMV s_iNextPixel[4] = {-2, 1, 2, 1};

// take out these C-code for xbox
//#ifndef WMV_OPT_LOOPFILTER_ARM
Void_WMV g_FilterHorizontalEdgeV9_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                                  I32_WMV iNumPixel)
{
    I32_WMV i, j;
    // Filter horizontal line       
    for ( j = 0; j < iNumPixel >> 2; j ++) {                                                                                                                                                                                                    
        
        pV5 += 2;
        for ( i = 0; i < 4; i++) {                                                                                                                                                                                                  
            I32_WMV v1, v2, v3, v4, v5, v6, v7, v8;
            I32_WMV v4_v5, a30, absA30;
            I32_WMV a, c;                
            
            U8_WMV *pVtmp = pV5;
            v5 = *pV5;
            pVtmp -= iPixelDistance;
            v4 = *pVtmp;
            pVtmp -= iPixelDistance;
            v3 = *pVtmp;
            pVtmp -= iPixelDistance;
            v2 = *pVtmp;
            pVtmp -= iPixelDistance;
            v1 = *pVtmp;
            pVtmp = pV5 + iPixelDistance;
            v6 = *pVtmp;        
            pVtmp += iPixelDistance;
            v7 = *pVtmp;
            pVtmp += iPixelDistance;
            v8 = *pVtmp;
            
            v4_v5 = v4 - v5;
            c = v4_v5/2;

            if (c == 0)
            {
                if (i == 0)
                {
                    pV5 += 2;
                    break;
                }
                else
                {
                    pV5 += s_iNextPixel[i];
                    continue;
                }
            }
            
            a30 = (2*(v3-v6) - 5*v4_v5 + 4) >> 3;

            absA30 = abs(a30);
            if (absA30 < iStepSize) {
                
                I32_WMV v2_v3 = v2 - v3;
                I32_WMV v6_v7 = v6 - v7;
                I32_WMV a31 = (2 * (v1-v4) - 5 * v2_v3 + 4) >> 3;                                 
                I32_WMV a32 = (2 * (v5-v8) - 5 * v6_v7 + 4) >> 3;                                 
                I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 
                
                if (iMina31_a32 < absA30){

                    I32_WMV dA30;
                    
                    if (((c & 0x80000000) ^ (a30 & 0x80000000)) != 0)
                    {
                        dA30 = (5*(absA30 - iMina31_a32)) >>3;
                        a = min(dA30, abs(c));
                        if (v4 < v5) a = -a;
                        *(pV5 - iPixelDistance) = v4 - a;
                        *(pV5) = v5 + a;
                    }
                }
                else if (i == 0)
                {
                    pV5 += 2;
                    break;
                }
            }
            else if (i == 0)
            {
                pV5 += 2;
                break;
            }
            
            pV5 += s_iNextPixel[i];
        }
    }
}

Void_WMV g_FilterHorizontalEdgeV9_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField)
{ // Simulator for Neon optimization.

    #define MASK_0x8000  0x8000

    I32_WMV i;
    I16_WMV v1[8], v2[8], v3[8], v4[8], v5[8], v6[8], v7[8], v8[8];
    I16_WMV absA30[8];
    I16_WMV a[8], c[8];    
	I16_WMV iMina31_a32[8];
	I16_WMV  Flag[8],validFlag[8];
	I32_WMV  Is8x8Done = uchBitField & 0x03;

    int start = 0;
    int stop = 8;
    if( ((uchBitField & 0x02)>>1)  == 0 )
    {
        start = 4; stop = 8;
    }
    if( (uchBitField & 0x01)  == 0 )
    {
        start = 0; stop = 4;
    }

	if ( 0 == Is8x8Done )
		goto DEBLOCK_H_8x4;

	{ // Load 8 rows src data for 8x8 lines.
		U8_WMV *pVtmp1 = pV5 - (iPixelDistance << 2);
		U8_WMV *pVtmp2 = pVtmp1 + iPixelDistance;
		U8_WMV *pVtmp3 = pVtmp2 + iPixelDistance;
		U8_WMV *pVtmp4 = pVtmp3 + iPixelDistance;
		U8_WMV *pVtmp5 = pVtmp4 + iPixelDistance;
		U8_WMV *pVtmp6 = pVtmp5 + iPixelDistance;
		U8_WMV *pVtmp7 = pVtmp6 + iPixelDistance;
		U8_WMV *pVtmp8 = pVtmp7 + iPixelDistance;

		for ( i = 0; i < 8; i ++) 
        {      
			v1[i] = pVtmp1[i];    v2[i] = pVtmp2[i];
			v3[i] = pVtmp3[i];    v4[i] = pVtmp4[i];
			v5[i] = pVtmp5[i];    v6[i] = pVtmp6[i];
			v7[i] = pVtmp7[i];    v8[i] = pVtmp8[i];
		}
	}

DEBLOCK_H_8x8:
	// Do filter.
	for ( i = 0; i < 8; i ++) 
    {  
        I16_WMV a30,flag1,flag2;
		I16_WMV  v4_v5 = v4[i] - v5[i];
        c[i] = abs(v4_v5)>>1;  

        if(c[i] == 0)
        {
            Flag[i] = 0;
            validFlag[i] = 0;
        }
        else
        {
            a30    = (2*(v3[i] - v6[i]) - 5 * v4_v5 + 4) >> 3;
            absA30[i] = abs(a30);
            flag1 = v4_v5 & MASK_0x8000; 
            flag2 = a30 & MASK_0x8000;
            Flag[i] = (flag1 != flag2) ? 1 : 0;
            if(Flag[i] == 0 && i != 2 && i != 6)
            {
                validFlag[i] = 0;
            }
            else
            {
                I16_WMV v2_v3 = v2[i] - v3[i];
                I16_WMV v6_v7 = v6[i] - v7[i];
                I16_WMV a31   = (2 * (v1[i] - v4[i]) - 5 * v2_v3 + 4) >> 3;                                 
                I16_WMV a32   = (2 * (v5[i] - v8[i]) - 5 * v6_v7 + 4) >> 3;    

                I16_WMV dA30;
                iMina31_a32[i] = min( abs(a31), abs(a32) ); 
                dA30 = (5*(absA30[i] - iMina31_a32[i])) >> 3;
                a[i] = min(dA30, c[i]);

			    if (v4[i] < v5[i]) 
				    a[i] = -a[i];
		        validFlag[i] = (absA30[i] < iStepSize);
		        validFlag[i] &= (iMina31_a32[i] < absA30[i]);
		    }
        }     
	}

	{
		I16_WMV flag2 = validFlag[2];
		I16_WMV flag6 = validFlag[6];

        //if( ((uchBitField & 0x02)>>1) == 0)
        //    goto SECOND;
		for ( i = 0; i < 4; i ++) {  // first segment
			validFlag[i] &= ((uchBitField & 0x02)>>1);
			validFlag[i] &= flag2; 
			validFlag[i] &= Flag[i];
		}
		for ( i = 4; i < 8; i ++) {  // second segment
			validFlag[i] &= (uchBitField & 0x01);
			validFlag[i] &= flag6;
			validFlag[i] &= Flag[i];
		}
	}

	// Pick out valid results and store them.
	{
		U8_WMV *pVtmp4 = pV5 - iPixelDistance;
		U8_WMV *pVtmp5 = pV5;
		for ( i = 0; i < 8; i ++) {   
			if (validFlag[i]) 
            {
				v4[i] -= a[i];
				v5[i] += a[i];
				pVtmp4[i] = (U8_WMV)v4[i];
				pVtmp5[i] = (U8_WMV)v5[i];	
			}
		}
	}

DEBLOCK_H_8x4:

	uchBitField >>= 4;

	if ( 0 == (uchBitField & 0x03) )
		return;

	{ // Load 8 rows src data for 8x4 lines.
		U8_WMV *pVtmp1 = pV5 - (iPixelDistance << 3);
		U8_WMV *pVtmp2 = pVtmp1 + iPixelDistance;
		U8_WMV *pVtmp3 = pVtmp2 + iPixelDistance;
		U8_WMV *pVtmp4 = pVtmp3 + iPixelDistance;
		U8_WMV *pVtmp5 = pVtmp4 + iPixelDistance;
		U8_WMV *pVtmp6 = pVtmp5 + iPixelDistance;
		U8_WMV *pVtmp7 = pVtmp6 + iPixelDistance;
		U8_WMV *pVtmp8 = pVtmp7 + iPixelDistance;

		if ( 0 != Is8x8Done )
        {
			for ( i = 0; i < 8; i ++) 
            {      
				v5[i] = v1[i];
				v6[i] = v2[i];
				v7[i] = v3[i];
				v8[i] = v4[i];
			}
		}
		else
        {
			for ( i = 0; i < 8; i ++)
            {      
				v5[i] = pVtmp5[i];
				v6[i] = pVtmp6[i];
				v7[i] = pVtmp7[i];
				v8[i] = pVtmp8[i];
			}
		}
		for ( i = 0; i < 8; i ++) 
        {      
			v1[i] = pVtmp1[i];
			v2[i] = pVtmp2[i];
			v3[i] = pVtmp3[i];
			v4[i] = pVtmp4[i];
		}
	}

	pV5 -= (iPixelDistance << 2);
	goto DEBLOCK_H_8x8;
}

Void_WMV g_FilterHorizontalEdgeV9Last8x4_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField)
{ // Simulator for Neon optimization.

#define MASK_0x8000  0x8000

    I32_WMV i;
    I16_WMV v1[8], v2[8], v3[8], v4[8], v5[8], v6[8], v7[8], v8[8];
    I16_WMV absA30[8];
    I16_WMV a[8], c[8];    
	I16_WMV iMina31_a32[8];
	I16_WMV  Flag[8],validFlag[8];
    I16_WMV flag2 ,flag6;
    //U8_WMV *pVtmp4 = pV5 - iPixelDistance;
	//U8_WMV *pVtmp5 = pV5;

    int start = 0;
    int stop = 8;
    if( ((uchBitField & 0x02)>>1)  == 0 )
    {
        start = 4; stop = 8;
    }
    if( (uchBitField & 0x01)  == 0 )
    {
        start = 0; stop = 4;
    }

	{ // Load 8 rows src data for 8x4 lines.
		U8_WMV *pVtmp1 = pV5 - (iPixelDistance << 2);
		U8_WMV *pVtmp2 = pVtmp1 + iPixelDistance;
		U8_WMV *pVtmp3 = pVtmp2 + iPixelDistance;
		U8_WMV *pVtmp4 = pVtmp3 + iPixelDistance;
		U8_WMV *pVtmp5 = pVtmp4 + iPixelDistance;
		U8_WMV *pVtmp6 = pVtmp5 + iPixelDistance;
		U8_WMV *pVtmp7 = pVtmp6 + iPixelDistance;
		U8_WMV *pVtmp8 = pVtmp7 + iPixelDistance;

		for (  i= start; i < stop; i ++) {      
			v1[i] = pVtmp1[i];    v2[i] = pVtmp2[i];
			v3[i] = pVtmp3[i];    v4[i] = pVtmp4[i];
			v5[i] = pVtmp5[i];    v6[i] = pVtmp6[i];
			v7[i] = pVtmp7[i];    v8[i] = pVtmp8[i];
		}
	}

	// Do filter.
	for ( i = start; i < stop; i ++) 
    {
		I16_WMV a30;
		I16_WMV v4_v5 = v4[i] - v5[i];
        c[i] = abs(v4_v5)>>1; 

        if(c[i] == 0)
        {
            Flag[i] = 0;
            validFlag[i] = 0;
        }
        else
        {
            I16_WMV flag1,flag2;
            a30 = (2*(v3[i] - v6[i]) - 5 * v4_v5 + 4) >> 3;
            absA30[i] = abs(a30);

            flag1 = v4_v5 & MASK_0x8000; //sign(v4_v5)
            flag2 = a30 & MASK_0x8000; //sign(a30)
            Flag[i] = (flag1 != flag2) ? 1 : 0;

            if(Flag[i] == 0 && i != 2 &&i != 6)
            {
                validFlag[i] = 0;
            }
            else
            {            
                I16_WMV v2_v3 = v2[i] - v3[i];
                I16_WMV v6_v7 = v6[i] - v7[i];
                I16_WMV a31   = (2 * (v1[i] - v4[i]) - 5 * v2_v3 + 4) >> 3;                                 
                I16_WMV a32   = (2 * (v5[i] - v8[i]) - 5 * v6_v7 + 4) >> 3;    

                I16_WMV dA30;
                iMina31_a32[i] = min( abs(a31), abs(a32) ); 
                dA30 = (5*(absA30[i] - iMina31_a32[i])) >> 3;
                a[i] = min(dA30, c[i]);
		        if (v4[i] < v5[i]) 
			        a[i] = -a[i];
		        validFlag[i] = (absA30[i] < iStepSize);
		        validFlag[i] &= (iMina31_a32[i] < absA30[i]);
            }
        } 
	}

    flag2 = validFlag[2];
	flag6 = validFlag[6];

    for ( i = 0; i < 4; i ++)
    {
        validFlag[i] &= ((uchBitField & 0x02)>>1);
        validFlag[i] &= flag2; 
        validFlag[i] &= Flag[i];
    }
    for ( i = 4; i < 8; i ++)
    {
        validFlag[i] &= (uchBitField & 0x01);
        validFlag[i] &= flag6; 
        validFlag[i] &= Flag[i];
    }

	{
        U8_WMV *pVtmp4 = pV5 - iPixelDistance;
		U8_WMV *pVtmp5 = pV5;
		for ( i = start; i < stop; i ++) 
        {  // first segment
            if (validFlag[i]) 
            {
				v4[i] -= a[i];
				v5[i] += a[i];
				pVtmp4[i] = (U8_WMV)v4[i];
				pVtmp5[i] = (U8_WMV)v5[i];	
			}
		}
	}
}

Void_WMV g_FilterVerticalEdgeV9_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField)
{ // Simulator for Neon optimization.

#define MASK_0x8000  0x8000

    I32_WMV i;
    I16_WMV v1[8], v2[8], v3[8], v4[8], v5[8], v6[8], v7[8], v8[8];
    I16_WMV absA30[8];
    I16_WMV a[8], c[8];    
	I16_WMV iMina31_a32[8];
	I16_WMV  Flag[8],validFlag[8];
	I32_WMV  Is8x8Done = uchBitField & 0x03;


	if ( 0 == Is8x8Done )
		goto DEBLOCK_V_4x8;

	{ // Load 8 column src data for 8x8 lines.
		U8_WMV *pVtmp = pV5;
		for ( i = 0; i < 8; i ++) { 
			v1[i] = pVtmp[0];
			v2[i] = pVtmp[1];
			v3[i] = pVtmp[2];
			v4[i] = pVtmp[3];
			v5[i] = pVtmp[4];
			v6[i] = pVtmp[5];
			v7[i] = pVtmp[6];
			v8[i] = pVtmp[7];
			pVtmp += iPixelDistance;
		}
	}

DEBLOCK_V_8x8:

	// Do filter.
	for ( i = 0; i < 8; i ++)
    {     

		I16_WMV a30;
		I16_WMV  v4_v5 = v4[i] - v5[i];
        c[i]      = abs(v4_v5)>>1;  

        if(c[i] == 0)
        {
            Flag[i] = 0;
            validFlag[i] = 0;
        }
        else
        {
            I16_WMV flag1,flag2;
            a30    = (2*(v3[i] - v6[i]) - 5 * v4_v5 + 4) >> 3;
            absA30[i] = abs(a30);
            flag1 = v4_v5 & MASK_0x8000; //sign(v4_v5)
            flag2 = a30   & MASK_0x8000; //sign(a30)
            Flag[i] = (flag1 != flag2) ? 1 : 0;

            if(Flag[i] == 0 && i != 2 && i != 6)
            {
                validFlag[i] = 0;
            }
            else
		    {
                I16_WMV v2_v3 = v2[i] - v3[i];
                I16_WMV v6_v7 = v6[i] - v7[i];
                I16_WMV a31   = (2 * (v1[i] - v4[i]) - 5 * v2_v3 + 4) >> 3;                                 
                I16_WMV a32   = (2 * (v5[i] - v8[i]) - 5 * v6_v7 + 4) >> 3;    

                I16_WMV dA30;
                iMina31_a32[i] = min( abs(a31), abs(a32) ); 
                dA30 = (5*(absA30[i] - iMina31_a32[i])) >> 3;
                a[i] = min(dA30, c[i]);

			    if (v4[i] < v5[i]) 
				    a[i] = -a[i];
		        validFlag[i] = (absA30[i] < iStepSize);
		        validFlag[i] &= (iMina31_a32[i] < absA30[i]);
		    }
        }
	}

	{
		I16_WMV flag2 = validFlag[2];
		I16_WMV flag6 = validFlag[6];
		for ( i = 0; i < 4; i ++) {  // first segment
			validFlag[i] &= ((uchBitField & 0x02)>>1);
			validFlag[i] &= flag2; 
			validFlag[i] &= Flag[i];
		}
        
		for ( i = 4; i < 8; i ++) {  // second segment
			validFlag[i] &= (uchBitField & 0x01);
			validFlag[i] &= flag6;
			validFlag[i] &= Flag[i];
		}
	}

	// Pick out valid results and store them.
	{
		U8_WMV *pVtmp = pV5;
		for ( i = 0; i < 8; i ++) {   
			if (validFlag[i]) {
				v4[i] -= a[i];
				v5[i] += a[i];
				pVtmp[3] = (U8_WMV)v4[i];
				pVtmp[4] = (U8_WMV)v5[i];	
			}
			pVtmp += iPixelDistance; 
		}
	}

DEBLOCK_V_4x8:

	uchBitField >>= 4;

	if ( 0 == (uchBitField & 0x03) )
		return;

	{ // Load 8 columns src data for 4x8 columns.
		U8_WMV *pVtmp = pV5 - 4;
		if ( 0 != Is8x8Done ) {
			for ( i = 0; i < 8; i ++) {      
				v5[i] = v1[i];
				v6[i] = v2[i];
				v7[i] = v3[i];
				v8[i] = v4[i];
			}
		}
		else {
			for ( i = 0; i < 8; i ++) {      
				v5[i] = pVtmp[4];
				v6[i] = pVtmp[5];
				v7[i] = pVtmp[6];
				v8[i] = pVtmp[7];
				pVtmp += iPixelDistance;
			}
		}
		pVtmp = pV5 - 4;
		for ( i = 0; i < 8; i ++) {      
			v1[i] = pVtmp[0];
			v2[i] = pVtmp[1];
			v3[i] = pVtmp[2];
			v4[i] = pVtmp[3];
			pVtmp += iPixelDistance;
		}

	}
	pV5 -= 4;
	goto DEBLOCK_V_8x8;
}

Void_WMV g_FilterVerticalEdgeV9Last4x8_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField)
{ // Simulator for Neon optimization.

#define MASK_0x8000  0x8000

    I32_WMV i;
    I16_WMV v1[8], v2[8], v3[8], v4[8], v5[8], v6[8], v7[8], v8[8];
    I16_WMV absA30[8];
    I16_WMV a[8], c[8];
	I16_WMV iMina31_a32[8];
	I16_WMV  Flag[8],validFlag[8];
	//I32_WMV  Is8x8Done = uchBitField & 0x03;

	{ // Load 8 column src data for 8x8 lines.
		U8_WMV *pVtmp = pV5;
		for ( i = 0; i < 8; i ++) { 
			v1[i] = pVtmp[0];
			v2[i] = pVtmp[1];
			v3[i] = pVtmp[2];
			v4[i] = pVtmp[3];
			v5[i] = pVtmp[4];
			v6[i] = pVtmp[5];
			v7[i] = pVtmp[6];
			v8[i] = pVtmp[7];
			pVtmp += iPixelDistance;
		}
	}

	// Do filter.
	for ( i = 0; i < 8; i ++) {     

		I16_WMV a30;
		I16_WMV  v4_v5 = v4[i] - v5[i];
        c[i]      = abs(v4_v5)>>1;  
        if(c[i] == 0)
        {
            Flag[i] = 0;
            validFlag[i] = 0;
        }
        else
        {
             I16_WMV flag1,flag2;
            a30    = (2*(v3[i] - v6[i]) - 5 * v4_v5 + 4) >> 3;
            absA30[i] = abs(a30);
            flag1 = v4_v5 & MASK_0x8000; //sign(v4_v5)
            flag2 = a30   & MASK_0x8000; //sign(a30)
            Flag[i] = (flag1 != flag2) ? 1 : 0;
            if(Flag[i] == 0 && i != 2 && i != 6)
            {
                validFlag[i] = 0;
            }
            else
		    {
                I16_WMV v2_v3 = v2[i] - v3[i];
                I16_WMV v6_v7 = v6[i] - v7[i];
                I16_WMV a31   = (2 * (v1[i] - v4[i]) - 5 * v2_v3 + 4) >> 3;                                 
                I16_WMV a32   = (2 * (v5[i] - v8[i]) - 5 * v6_v7 + 4) >> 3;    

                I16_WMV dA30;
                iMina31_a32[i] = min( abs(a31), abs(a32) ); 
                dA30 = (5*(absA30[i] - iMina31_a32[i])) >> 3;
                a[i] = min(dA30, c[i]);

			    if (v4[i] < v5[i]) 
				    a[i] = -a[i];
		        validFlag[i] = (absA30[i] < iStepSize);
		        validFlag[i] &= (iMina31_a32[i] < absA30[i]);
		    }
        }    
	}

	{
		I16_WMV flag2 = validFlag[2];
		I16_WMV flag6 = validFlag[6];
		for ( i = 0; i < 4; i ++) {  // first segment
			validFlag[i] &= ((uchBitField & 0x02)>>1);
			validFlag[i] &= flag2; 
			validFlag[i] &= Flag[i];
		}
		for ( i = 4; i < 8; i ++) {  // second segment
			validFlag[i] &= (uchBitField & 0x01);
			validFlag[i] &= flag6;
			validFlag[i] &= Flag[i];
		}
	}

	// Pick out valid results and store them.
	{
		U8_WMV *pVtmp = pV5;
		for ( i = 0; i < 8; i ++) {   
			if (validFlag[i]) {
				v4[i] -= a[i];
				v5[i] += a[i];
				pVtmp[3] = (U8_WMV)v4[i];
				pVtmp[4] = (U8_WMV)v5[i];	
			}
			pVtmp += iPixelDistance; 
		}
	}
}

Void_WMV g_FilterVerticalEdgeV9_C (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                          I32_WMV iNumPixel)
{
    U8_WMV *pVh;
    I32_WMV i, j;
    for (j = 0; j < iNumPixel >> 2; j++)
    {
        // Filter vertical line
        pVhstart += (iPixelDistance*2);
        for (i = 0; i < 4; ++i) {
            I32_WMV v1, v2, v3, v4, v5, v6, v7, v8;       
            I32_WMV v4_v5, a30, absA30;
            I32_WMV a, c;                
 
            pVh = pVhstart;        
            
            v1 = pVh[1]; 
            v2 = pVh[2];
            v3 = pVh[3]; 
            v4 = pVh[4];
            v5 = pVh[5];
            v6 = pVh[6]; 
            v7 = pVh[7]; 
            v8 = pVh[8]; 
            
            v4_v5 = v4 - v5;
            c = v4_v5/2;
            
            if (c == 0)
            {
                if (i == 0)
                {
                    pVhstart += (2*iPixelDistance);
                    break;
                }
                else
                {
                    pVhstart += (s_iNextPixel[i]*iPixelDistance);
                    continue;
                }
            }

            a30 = (2*(v3-v6) - 5*v4_v5 + 4) >> 3;
            absA30 = abs(a30);
            if (absA30 < iStepSize) {
                
                I32_WMV v2_v3 = v2 - v3;
                I32_WMV v6_v7 = v6 - v7;
                I32_WMV a31 = (2 * (v1-v4) - 5 * v2_v3 + 4) >> 3;                                 
                I32_WMV a32 = (2 * (v5-v8) - 5 * v6_v7 + 4) >> 3;                                 
                I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 
                
                if (iMina31_a32 < absA30){

                    I32_WMV dA30;
                    if (((c & 0x80000000) ^ (a30 & 0x80000000)) != 0)
                    {
                        dA30 = (5*(absA30 - iMina31_a32)) >> 3;
                        a = min(dA30, abs(c));
                        if (v4 < v5) 
							a = -a;
                        pVh[4] = v4 - a;
                        pVh[5] = v5 + a;
                    }
                }
                else if (i == 0)
                {
                    pVhstart += (2*iPixelDistance);
                    break;
                }
            }
            else if (i == 0)
            {
                pVhstart += (2*iPixelDistance);
                break;
            }
            pVhstart += (s_iNextPixel[i]*iPixelDistance);                
        }
    }
}


Void_WMV DeblockRows_slice (tWMVDecInternalMember *pWMVDec, int bottom_row,
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,I32_WMV iNumRows, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilterFlagY8x4, U8_WMV* puchFilterFlagU8x4, U8_WMV* puchFilterFlagV8x4)
{
    I32_WMV iXY, iXUV,iX1Y,iX2Y,iX1UV,iX2UV,iRow, i,j;
    U8_WMV uchFlag, uchFlag8x4, *pFilterFlagY, *pFilterFlag8x4Y;
    U8_WMV *pFilterFlagU, *pFilterFlag8x4U,*pFilterFlagV, *pFilterFlag8x4V;
    U8_WMV * pCurr,*pCurrU,*pCurrV;
	I32_WMV iWidthPrevY, iWidthY, iStepSize; 
    I32_WMV iWidthPrevUV, iWidthUV; 
    
    //****
 	iWidthPrevY = pWMVDec->m_iWidthPrevY;
	iWidthY = pWMVDec->m_iWidthY;
	iStepSize = pWMVDec->m_iStepSize;
    iXY = (iWidthY + 31) >> 5;
    iX1Y = iXY/2;
    iX2Y = iXY-iX1Y;
    //*****
 	iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
	iWidthUV     = pWMVDec->m_iWidthUV;
    iXUV = (iWidthUV + 31) >> 5;
    iX1UV = iXUV/2;
    iX2UV = iXUV-iX1UV;
    //*****

    // First half cols. Filter Y boundary rows // Filter U V boundary rows
    for(iRow = 0; iRow < iNumRows; iRow++)
    {
        pCurr = ppxliY + (iWidthPrevY << 3)*iRow;
        pFilterFlagY = puchFilterFlagY + iRow*iXY;
        pFilterFlag8x4Y = puchFilterFlagY8x4 + iRow*iXY;
        for(i=0; i< iXY; i++)
        {
            uchFlag    = *pFilterFlagY ++;
            uchFlag8x4 = *pFilterFlag8x4Y ++;
            FilterRow_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevY, iStepSize);
            FilterRow_CtxA8 (pCurr + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevY, iStepSize);
            pCurr += 32;
        }
    }
    //Last 8x4 row of Y.
    pCurr = ppxliY + (iWidthPrevY << 3)*iNumRows - (iWidthPrevY << 2);
    pFilterFlag8x4Y = puchFilterFlagY8x4 + iNumRows*iXY;
    for(j=0; j< iXY; j++)  
    {
		uchFlag8x4 = *pFilterFlag8x4Y ++;
		FilterRowLast8x4_CtxA8 (pCurr, (uchFlag8x4 >> 4), iWidthPrevY, iStepSize);
		FilterRowLast8x4_CtxA8 (pCurr + 16, (uchFlag8x4 & 0xf), iWidthPrevY, iStepSize);
        pCurr += 32;
    }
    for(iRow=0; iRow < (iNumRows >> 1); iRow++)
    {
        pCurrU = ppxliU + (iWidthPrevUV << 3) *iRow;	
        pFilterFlagU = puchFilterFlagU + iRow * iXUV;
        pFilterFlag8x4U = puchFilterFlagU8x4+ iRow * iXUV;
        for(i=0;i< iXUV; i++)
        {
            uchFlag = *pFilterFlagU ++;
            uchFlag8x4 = *pFilterFlag8x4U ++;
            FilterRow_CtxA8 (pCurrU, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
            FilterRow_CtxA8 (pCurrU + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
            pCurrU += 32;
        }

        pCurrV = ppxliV + (iWidthPrevUV << 3)*iRow;	
        pFilterFlagV = puchFilterFlagV + iRow*iXUV;
        pFilterFlag8x4V = puchFilterFlagV8x4 + iRow*iXUV;
        for(i=0;i< iXUV; i++)
        {
            uchFlag = *pFilterFlagV ++;
            uchFlag8x4 = *pFilterFlag8x4V ++;
            FilterRow_CtxA8 (pCurrV, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
            FilterRow_CtxA8 (pCurrV + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
            pCurrV += 32;
        }
    }
    //Last 8x4 row of U.    
    pCurrU = ppxliU + (iWidthPrevUV << 3)*(iNumRows >> 1)- (iWidthPrevUV << 2);
    pFilterFlag8x4U = puchFilterFlagU8x4+ (iNumRows >> 1) * iXUV;
    for(i=0;i< iXUV; i++)  
    {
        uchFlag8x4 = *pFilterFlag8x4U ++;
		FilterRowLast8x4_CtxA8 (pCurrU, (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
		FilterRowLast8x4_CtxA8 (pCurrU + 16, (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
        pCurrU += 32;
    }
	//Last 8x4 row of V.
    pCurrV = ppxliV + (iWidthPrevUV << 3)*(iNumRows >> 1) - (iWidthPrevUV << 2);
    pFilterFlag8x4V = puchFilterFlagV8x4 + (iNumRows >> 1)*iXUV;
    for(i=0;i< iXUV; i++)  
    {
        uchFlag8x4 = *pFilterFlag8x4V ++;
		FilterRowLast8x4_CtxA8 (pCurrV, (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
		FilterRowLast8x4_CtxA8 (pCurrV + 16, (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
        pCurrV += 32;
    }
}

Void_WMV DeblockRows_mbrow_y (tWMVDecInternalMember *pWMVDec, int bottom_row,
    U8_WMV* ppxliY,
    U8_WMV* puchFilterFlagY,
	U8_WMV* puchFilterFlagY8x4)
{
    I32_WMV iXY, iXUV,iX1Y,iX2Y,iX1UV,iX2UV,iRow, i,j;
    U8_WMV uchFlag, uchFlag8x4, *pFilterFlagY, *pFilterFlag8x4Y;
    U8_WMV *pFilterFlagU, *pFilterFlag8x4U,*pFilterFlagV, *pFilterFlag8x4V;
    U8_WMV * pCurr,*pCurrU,*pCurrV;
	I32_WMV iWidthPrevY, iWidthY, iStepSize; 
    I32_WMV iWidthPrevUV, iWidthUV; 
    
    //****
 	iWidthPrevY = pWMVDec->m_iWidthPrevY;
	iWidthY = pWMVDec->m_iWidthY;
	iStepSize = pWMVDec->m_iStepSize;
    iXY = (iWidthY + 31) >> 5;
    iX1Y = iXY/2;
    iX2Y = iXY-iX1Y;

	//iRow = mb_row;

	if(!bottom_row)
	{
		//for(iRow = mb_row; iRow < mb_row+1; iRow++)
		{
			pCurr = ppxliY;
			pFilterFlagY = puchFilterFlagY /*+ iRow*iXY*/;
			pFilterFlag8x4Y = puchFilterFlagY8x4 /*+ iRow*iXY*/;
			for(i=0; i< iXY; i++)
			{
				uchFlag    = *pFilterFlagY ++;
				uchFlag8x4 = *pFilterFlag8x4Y ++;
				FilterRow_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevY, iStepSize);
				FilterRow_CtxA8 (pCurr + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevY, iStepSize);
				pCurr += 32;
			}
		}
	}
    //Last 8x4 row of Y.
	if(bottom_row)
	{
		pCurr = ppxliY - (iWidthPrevY << 2);
		pFilterFlag8x4Y = puchFilterFlagY8x4 /*+ (mb_row)*iXY*/;
		for(j=0; j< iXY; j++)  
		{
			uchFlag8x4 = *pFilterFlag8x4Y ++;
			FilterRowLast8x4_CtxA8 (pCurr, (uchFlag8x4 >> 4), iWidthPrevY, iStepSize);
			FilterRowLast8x4_CtxA8 (pCurr + 16, (uchFlag8x4 & 0xf), iWidthPrevY, iStepSize);
			pCurr += 32;
		}
	}
}

Void_WMV DeblockRows_mbrow_uv(tWMVDecInternalMember *pWMVDec, int bottom_row,
    U8_WMV* ppxliU, U8_WMV* ppxliV,
    U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilterFlagU8x4, U8_WMV* puchFilterFlagV8x4)
{
	I32_WMV iXY, iXUV,iX1Y,iX2Y,iX1UV,iX2UV,iRow, i,j;
    U8_WMV uchFlag, uchFlag8x4, *pFilterFlagY, *pFilterFlag8x4Y;
    U8_WMV *pFilterFlagU, *pFilterFlag8x4U,*pFilterFlagV, *pFilterFlag8x4V;
    U8_WMV * pCurr,*pCurrU,*pCurrV;
	I32_WMV iWidthPrevY, iWidthY, iStepSize; 
    I32_WMV iWidthPrevUV, iWidthUV; 
	    //*****
	iStepSize = pWMVDec->m_iStepSize;
 	iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
	iWidthUV     = pWMVDec->m_iWidthUV;
    iXUV = (iWidthUV + 31) >> 5;
    iX1UV = iXUV/2;
    iX2UV = iXUV-iX1UV;
    //*****

	if(!bottom_row)
	{
		//for(iRow= mb_row; iRow < mb_row+1; iRow++)
		{
			pCurrU = ppxliU;	
			pFilterFlagU = puchFilterFlagU /*+ iRow * iXUV*/;
			pFilterFlag8x4U = puchFilterFlagU8x4/*+ iRow * iXUV*/;
			for(i=0;i< iXUV; i++)
			{
				uchFlag = *pFilterFlagU ++;
				uchFlag8x4 = *pFilterFlag8x4U ++;
				FilterRow_CtxA8 (pCurrU, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
				FilterRow_CtxA8 (pCurrU + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
				pCurrU += 32;
			}

			pCurrV = ppxliV;	
			pFilterFlagV = puchFilterFlagV /*+ iRow*iXUV*/;
			pFilterFlag8x4V = puchFilterFlagV8x4 /*+ iRow*iXUV*/;
			for(i=0;i< iXUV; i++)
			{
				uchFlag = *pFilterFlagV ++;
				uchFlag8x4 = *pFilterFlag8x4V ++;
				FilterRow_CtxA8 (pCurrV, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
				FilterRow_CtxA8 (pCurrV + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
				pCurrV += 32;
			}
		}
	}

	if(bottom_row)
	{
		//Last 8x4 row of U.    
		pCurrU = ppxliU - (iWidthPrevUV << 2);
		pFilterFlag8x4U = puchFilterFlagU8x4/*+ (mb_row) * iXUV*/;
		for(i=0;i< iXUV; i++)  
		{
			uchFlag8x4 = *pFilterFlag8x4U ++;
			FilterRowLast8x4_CtxA8 (pCurrU, (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
			FilterRowLast8x4_CtxA8 (pCurrU + 16, (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
			pCurrU += 32;
		}
		//Last 8x4 row of V.
		pCurrV = ppxliV  - (iWidthPrevUV << 2);
		pFilterFlag8x4V = puchFilterFlagV8x4/* + (mb_row)*iXUV*/;
		for(i=0;i< iXUV; i++)  
		{
			uchFlag8x4 = *pFilterFlag8x4V ++;
			FilterRowLast8x4_CtxA8 (pCurrV, (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
			FilterRowLast8x4_CtxA8 (pCurrV + 16, (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
			pCurrV += 32;
		}
	}
}

Void_WMV DeblockColumns_mbrow_uv (tWMVDecInternalMember *pWMVDec, 
						U8_WMV* ppxliU, U8_WMV* ppxliV,
						I32_WMV iNumCols, I32_WMV iHeight, 
						U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
						U8_WMV* puchFilter4x8ColFlagU, U8_WMV* puchFilter4x8ColFlagV)
{
    I32_WMV iY, iCol;
    U8_WMV uchFlag, *puchFilterFlag, uchFlag4x8, *puchFilterFlag4x8;
    U8_WMV* pCurr,*pCurr2;
    I32_WMV iWidthPrevY, iWidthPrevUV;
	I32_WMV iStepSize;
	//I32_WMV iRow;
	//I32_WMV next_flag = (mb8x8_row%2);


    ppxliU -= 4;    ppxliV -= 4;
    iStepSize = pWMVDec->m_iStepSize;
    iWidthPrevUV  = pWMVDec->m_iWidthPrevUV;
	//U
	//iRow = mb8x8_row;
    {
		puchFilterFlag = puchFilterFlagU/*+iRow/2*/;
		puchFilterFlag4x8 = puchFilter4x8ColFlagU/*+iRow/2*/;
        pCurr = ppxliU;

		for(iCol = 0; iCol< (iNumCols >> 1); iCol++)
		{
			U8_WMV BitField;
			uchFlag    = *puchFilterFlag;
            uchFlag4x8 = *puchFilterFlag4x8;
			//if(iRow%2 == 0)
			{
				BitField = ((uchFlag >> 4)>>2) | (((uchFlag4x8 >> 4)>>2) << 4);

				if (BitField & 0x33) 
					g_FilterVerticalEdgeV9_Fun(pCurr, iWidthPrevUV, iStepSize, BitField);
			}		
			puchFilterFlag4x8 += (((iHeight >> 1) + 31) >> 5);
            puchFilterFlag += (((iHeight >> 1) + 31) >> 5);
            pCurr += 8;
		}

	
		{
			pCurr -= 4;
            uchFlag4x8 = *puchFilterFlag4x8;
			//if(iRow%2 == 0)
			{
				if (((uchFlag4x8) >> 4) & 0x0C)
					g_FilterVerticalEdgeV9Last4x8_Fun(pCurr, iWidthPrevUV, iStepSize, ((uchFlag4x8) >> 4) >> 2);
			}
		}
	}

	//V
	//iRow = mb8x8_row;
    {
		puchFilterFlag = puchFilterFlagV/*+iRow/2*/;
		puchFilterFlag4x8 = puchFilter4x8ColFlagV/*+iRow/2*/;
        pCurr = ppxliV;

		for(iCol = 0; iCol< (iNumCols >> 1); iCol++)
		{
			U8_WMV BitField;
			uchFlag    = *puchFilterFlag;
            uchFlag4x8 = *puchFilterFlag4x8;
			//if(iRow%2 == 0)
			{
				BitField = ((uchFlag >> 4)>>2) | (((uchFlag4x8 >> 4)>>2) << 4);

				if (BitField & 0x33) 
					g_FilterVerticalEdgeV9_Fun(pCurr, iWidthPrevUV, iStepSize, BitField);
			}
			
			puchFilterFlag4x8 += (((iHeight >> 1) + 31) >> 5);
            puchFilterFlag += (((iHeight >> 1) + 31) >> 5);
            pCurr += 8;
		}

		
		{
			pCurr -= 4;
            uchFlag4x8 = *puchFilterFlag4x8;
			//if(iRow%2 == 0)
			{
				if (((uchFlag4x8) >> 4) & 0x0C)
					g_FilterVerticalEdgeV9Last4x8_Fun(pCurr, iWidthPrevUV, iStepSize, ((uchFlag4x8) >> 4) >> 2);
			}
		}
	}
}


Void_WMV DeblockColumns_mbrow_y (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY,
    I32_WMV iNumCols, I32_WMV iHeight, 
    U8_WMV* puchFilterFlagY,
	U8_WMV* puchFilter4x8ColFlagY)
{
    I32_WMV iY, iCol;
    U8_WMV uchFlag, *puchFilterFlag, uchFlag4x8, *puchFilterFlag4x8;
    U8_WMV* pCurr,*pCurr2;
    I32_WMV iWidthPrevY, iWidthPrevUV;
	I32_WMV iStepSize;
	I32_WMV iRow;

    ppxliY -= 4;
    iStepSize = pWMVDec->m_iStepSize;

    // Filter Y boundary columns
	iWidthPrevY  = pWMVDec->m_iWidthPrevY;
	
	iY = (iHeight + 15) >> 4;
	//for(iRow = mb_row;iRow < mb_row+1;iRow ++)
    {
		//1
		puchFilterFlag = (puchFilterFlagY/*+iRow/2*/);
		puchFilterFlag4x8 = (puchFilter4x8ColFlagY/*+iRow/2*/);
        pCurr = ppxliY;
        for(iCol=0;iCol<iNumCols;iCol++)
        {
            uchFlag    = *puchFilterFlag;
            uchFlag4x8 = *puchFilterFlag4x8;
			FilterColumn_CtxA8 (pCurr, uchFlag >> 4, uchFlag4x8 >> 4, iWidthPrevY, iStepSize);
		    puchFilterFlag4x8 += ((iHeight + 31) >> 5);
		    puchFilterFlag += ((iHeight + 31) >> 5);
            pCurr += 8;
        }
        
        pCurr = ppxliY+8*iNumCols -4;
        uchFlag4x8 = *(puchFilter4x8ColFlagY+ ((iHeight + 31) >> 5)*iNumCols);
		FilterColumnLast4x8_CtxA8 (pCurr, uchFlag4x8 >> 4, iWidthPrevY, iStepSize);
	}
}

Void_WMV DeblockColumns_slice (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    I32_WMV iNumCols, I32_WMV iHeight, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilter4x8ColFlagY, U8_WMV* puchFilter4x8ColFlagU, U8_WMV* puchFilter4x8ColFlagV)
{
    I32_WMV iY, iCol;
    U8_WMV uchFlag, *puchFilterFlag, uchFlag4x8, *puchFilterFlag4x8;
    U8_WMV* pCurr,*pCurr2;
    I32_WMV iWidthPrevY, iWidthPrevUV;
	I32_WMV iStepSize;
	I32_WMV iRow;

    ppxliY -= 4;
    ppxliU -= 4;
    ppxliV -= 4;
    iStepSize = pWMVDec->m_iStepSize;

    // Filter Y boundary columns
	iWidthPrevY  = pWMVDec->m_iWidthPrevY;
	
	iY = (iHeight + 15) >> 4;
	for(iRow = 0;iRow < iY;iRow ++)
    {
		//1
		puchFilterFlag = (puchFilterFlagY+iRow/2);
		puchFilterFlag4x8 = (puchFilter4x8ColFlagY+iRow/2);
        pCurr = ppxliY + (iWidthPrevY << 4)*iRow;
        for(iCol=0;iCol<iNumCols;iCol++)
        {
            uchFlag    = *puchFilterFlag;
            uchFlag4x8 = *puchFilterFlag4x8;
			if(iRow%2 == 0)
				FilterColumn_CtxA8 (pCurr, uchFlag >> 4, uchFlag4x8 >> 4, iWidthPrevY, iStepSize);
			else
				FilterColumn_CtxA8 (pCurr,uchFlag & 0x0f, uchFlag4x8 & 0x0f, iWidthPrevY, iStepSize);

		    puchFilterFlag4x8 += ((iHeight + 31) >> 5);
		    puchFilterFlag += ((iHeight + 31) >> 5);
            pCurr += 8;
        }
        
        pCurr = ppxliY+ (iWidthPrevY << 4)*iRow+8*iNumCols -4;
        uchFlag4x8 = *(puchFilter4x8ColFlagY+iRow/2+ ((iHeight + 31) >> 5)*iNumCols);
		if(iRow%2 == 0)
			FilterColumnLast4x8_CtxA8 (pCurr, uchFlag4x8 >> 4, iWidthPrevY, iStepSize);
		else
			FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8 & 0x0f), iWidthPrevY, iStepSize);
	}


	//UV
#if 1
    iWidthPrevUV  = pWMVDec->m_iWidthPrevUV;
	iY = ((iHeight >> 1) + 31) >> 5;
    for(iRow = 0;iRow < iY;iRow ++)
    {
		puchFilterFlag = puchFilterFlagU+iRow;
		puchFilterFlag4x8 = puchFilter4x8ColFlagU+iRow;
        pCurr = ppxliU +  (iWidthPrevUV << 4)*2*iRow;
		pCurr2 = ppxliU +  (iWidthPrevUV << 4)*(2*iRow+1);

        for(iCol = 0; iCol< (iNumCols >> 1); iCol++)
        {
            uchFlag    = *puchFilterFlag;
            uchFlag4x8 = *puchFilterFlag4x8;
            FilterColumn_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag4x8 >> 4), iWidthPrevUV, iStepSize);
			FilterColumn_CtxA8 (pCurr2, (uchFlag & 0x0f), (uchFlag4x8 & 0x0f), iWidthPrevUV, iStepSize);
            puchFilterFlag4x8 += iY;
            puchFilterFlag += iY;
            pCurr += 8;
			pCurr2 += 8;
        }

		pCurr -= 4;
		pCurr2 -= 4;
        uchFlag4x8 = *puchFilterFlag4x8;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8) >> 4, iWidthPrevUV, iStepSize);
		FilterColumnLast4x8_CtxA8 (pCurr2, (uchFlag4x8 & 0x0f), iWidthPrevUV, iStepSize);
    }

    for(iRow = 0;iRow < iY;iRow ++)
    {
		puchFilterFlag = puchFilterFlagV+iRow;
		puchFilterFlag4x8 = puchFilter4x8ColFlagV+iRow;
        pCurr = ppxliV +  (iWidthPrevUV << 4)*2*iRow;
		pCurr2 = ppxliV +  (iWidthPrevUV << 4)*(2*iRow+1);
        for(iCol = 0; iCol < (iNumCols >> 1);iCol++ )
        {
            uchFlag    = *puchFilterFlag;
			uchFlag4x8 = *puchFilterFlag4x8;
			FilterColumn_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag4x8 >> 4), iWidthPrevUV, iStepSize);
			FilterColumn_CtxA8 (pCurr2, (uchFlag & 0x0f), (uchFlag4x8 & 0x0f), iWidthPrevUV, iStepSize);
			puchFilterFlag4x8 += iY;
			puchFilterFlag    += iY;
			pCurr += 8;
			pCurr2 += 8;
        }

		pCurr -= 4;
		pCurr2 -= 4;
        uchFlag4x8 = *puchFilterFlag4x8;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8) >> 4, iWidthPrevUV, iStepSize);
		FilterColumnLast4x8_CtxA8 (pCurr2, (uchFlag4x8 & 0x0f), iWidthPrevUV, iStepSize);
    }
#endif
}

//#endif //WMV_OPT_LOOPFILTER_ARM

Void_WMV DeblockRowsLeftCols (tWMVDecInternalMember *pWMVDec, int bottom_row,
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,I32_WMV iNumRows, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilterFlagY8x4, U8_WMV* puchFilterFlagU8x4, U8_WMV* puchFilterFlagV8x4)
{
    I32_WMV iXY, iXUV,iX1Y,iX2Y,iX1UV,iX2UV,iRow, i,j;
    U8_WMV uchFlag, uchFlag8x4, *pFilterFlagY, *pFilterFlag8x4Y;
    U8_WMV *pFilterFlagU, *pFilterFlag8x4U,*pFilterFlagV, *pFilterFlag8x4V;
    U8_WMV * pCurr,*pCurrU,*pCurrV;
	I32_WMV iWidthPrevY, iWidthY, iStepSize; 
    I32_WMV iWidthPrevUV, iWidthUV; 
    
    //****
 	iWidthPrevY = pWMVDec->m_iWidthPrevY;
	iWidthY = pWMVDec->m_iWidthY;
	iStepSize = pWMVDec->m_iStepSize;
    iXY = (iWidthY + 31) >> 5;
    iX1Y = iXY/2;
    iX2Y = iXY-iX1Y;
    //*****
 	iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
	iWidthUV     = pWMVDec->m_iWidthUV;
    iXUV = (iWidthUV + 31) >> 5;
    iX1UV = iXUV/2;
    iX2UV = iXUV-iX1UV;
    //*****

    // First half cols. Filter Y boundary rows // Filter U V boundary rows
    for(iRow = 0; iRow < iNumRows; iRow++)
    {
        pCurr = ppxliY + (iWidthPrevY << 3)*iRow;
        pFilterFlagY = puchFilterFlagY + iRow*iXY;
        pFilterFlag8x4Y = puchFilterFlagY8x4 + iRow*iXY;
        for(i=0; i< iX1Y; i++)
        {
            uchFlag    = *pFilterFlagY ++;
            uchFlag8x4 = *pFilterFlag8x4Y ++;
            FilterRow_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevY, iStepSize);
            FilterRow_CtxA8 (pCurr + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevY, iStepSize);
            pCurr += 32;
        }
    }
    //Last 8x4 row of Y.
    pCurr = ppxliY + (iWidthPrevY << 3)*iNumRows - (iWidthPrevY << 2);
    pFilterFlag8x4Y = puchFilterFlagY8x4 + iNumRows*iXY;
    for(j=0; j< iX1Y; j++)  
    {
		uchFlag8x4 = *pFilterFlag8x4Y ++;
		FilterRowLast8x4_CtxA8 (pCurr, (uchFlag8x4 >> 4), iWidthPrevY, iStepSize);
		FilterRowLast8x4_CtxA8 (pCurr + 16, (uchFlag8x4 & 0xf), iWidthPrevY, iStepSize);
        pCurr += 32;
    }
    for(iRow=0; iRow < (iNumRows >> 1); iRow++)
    {
        pCurrU = ppxliU + (iWidthPrevUV << 3) *iRow;	
        pFilterFlagU = puchFilterFlagU + iRow * iXUV;
        pFilterFlag8x4U = puchFilterFlagU8x4+ iRow * iXUV;
        for(i=0;i< iX1UV; i++)
        {
            uchFlag = *pFilterFlagU ++;
            uchFlag8x4 = *pFilterFlag8x4U ++;
            FilterRow_CtxA8 (pCurrU, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
            FilterRow_CtxA8 (pCurrU + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
            pCurrU += 32;
        }

        pCurrV = ppxliV + (iWidthPrevUV << 3)*iRow;	
        pFilterFlagV = puchFilterFlagV + iRow*iXUV;
        pFilterFlag8x4V = puchFilterFlagV8x4 + iRow*iXUV;
        for(i=0;i< iX1UV; i++)
        {
            uchFlag = *pFilterFlagV ++;
            uchFlag8x4 = *pFilterFlag8x4V ++;
            FilterRow_CtxA8 (pCurrV, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
            FilterRow_CtxA8 (pCurrV + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
            pCurrV += 32;
        }
    }
    //Last 8x4 row of U.    
    pCurrU = ppxliU + (iWidthPrevUV << 3)*(iNumRows >> 1)- (iWidthPrevUV << 2);
    pFilterFlag8x4U = puchFilterFlagU8x4+ (iNumRows >> 1) * iXUV;
    for(i=0;i< iX1UV; i++)  
    {
        uchFlag8x4 = *pFilterFlag8x4U ++;
		FilterRowLast8x4_CtxA8 (pCurrU, (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
		FilterRowLast8x4_CtxA8 (pCurrU + 16, (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
        pCurrU += 32;
    }
	//Last 8x4 row of V.
    pCurrV = ppxliV + (iWidthPrevUV << 3)*(iNumRows >> 1) - (iWidthPrevUV << 2);
    pFilterFlag8x4V = puchFilterFlagV8x4 + (iNumRows >> 1)*iXUV;
    for(i=0;i< iX1UV; i++)  
    {
        uchFlag8x4 = *pFilterFlag8x4V ++;
		FilterRowLast8x4_CtxA8 (pCurrV, (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
		FilterRowLast8x4_CtxA8 (pCurrV + 16, (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
        pCurrV += 32;
    }
}

Void_WMV DeblockRowsRightCols (tWMVDecInternalMember *pWMVDec, int bottom_row,
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,I32_WMV iNumRows, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilterFlagY8x4, U8_WMV* puchFilterFlagU8x4, U8_WMV* puchFilterFlagV8x4)
{
    I32_WMV iXY, iXUV,iX1Y,iX2Y,iX1UV,iX2UV,iRow, i,j;
    U8_WMV uchFlag, uchFlag8x4, *pFilterFlagY, *pFilterFlag8x4Y;
    U8_WMV *pFilterFlagU, *pFilterFlag8x4U,*pFilterFlagV, *pFilterFlag8x4V;
    U8_WMV * pCurr,*pCurrU,*pCurrV;
	I32_WMV iWidthPrevY, iWidthY, iStepSize; 
    I32_WMV iWidthPrevUV, iWidthUV; 
    
    //****
 	iWidthPrevY = pWMVDec->m_iWidthPrevY;
	iWidthY = pWMVDec->m_iWidthY;
	iStepSize = pWMVDec->m_iStepSize;
    iXY = (iWidthY + 31) >> 5;
    iX1Y = iXY/2;
    iX2Y = iXY-iX1Y;
    //*****
 	iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
	iWidthUV     = pWMVDec->m_iWidthUV;
    iXUV = (iWidthUV + 31) >> 5;
    iX1UV = iXUV/2;
    iX2UV = iXUV-iX1UV;
    //*****
    //Y
    for(iRow = 0; iRow < iNumRows; iRow++)  
    {
        pCurr = ppxliY + (iWidthPrevY << 3)*iRow +32*iX1Y;
        pFilterFlagY = puchFilterFlagY + iRow*iXY + iX1Y;
        pFilterFlag8x4Y = puchFilterFlagY8x4 + iRow*iXY + iX1Y;
        for(j=0; j< iX2Y; j++)
        {
            uchFlag    = *pFilterFlagY ++;
            uchFlag8x4 = *pFilterFlag8x4Y ++;
            FilterRow_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevY, iStepSize);
            FilterRow_CtxA8 (pCurr + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevY, iStepSize);
            pCurr += 32;
        }
    }
    //Last 8x4 row of Y.
    pCurr = ppxliY + (iWidthPrevY << 3)*iNumRows - (iWidthPrevY << 2) + 32*iX1Y;
    pFilterFlag8x4Y = puchFilterFlagY8x4 + iNumRows*iXY + iX1Y ;
    for(j=0; j< iX2Y; j++)  
    {
		uchFlag8x4 = *pFilterFlag8x4Y ++;
		FilterRowLast8x4_CtxA8 (pCurr, (uchFlag8x4 >> 4), iWidthPrevY, iStepSize);
		FilterRowLast8x4_CtxA8 (pCurr + 16, (uchFlag8x4 & 0xf), iWidthPrevY, iStepSize);
        pCurr += 32;
    }
    // UV 
    for(iRow=0; iRow < (iNumRows >> 1); iRow++)
    {
        pCurrU = ppxliU + (iWidthPrevUV << 3) *iRow + 32*iX1UV;	
        pFilterFlagU = puchFilterFlagU + iRow * iXUV + iX1UV;
        pFilterFlag8x4U = puchFilterFlagU8x4+ iRow * iXUV + iX1UV;
        for(i=0;i< iX2UV; i++)
        {
            uchFlag = *pFilterFlagU ++;
            uchFlag8x4 = *pFilterFlag8x4U ++;
            FilterRow_CtxA8 (pCurrU, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
            FilterRow_CtxA8 (pCurrU + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
            pCurrU += 32;
        }

        pCurrV = ppxliV + (iWidthPrevUV << 3)*iRow+ 32*iX1UV;	
        pFilterFlagV = puchFilterFlagV + iRow*iXUV + iX1UV;
        pFilterFlag8x4V = puchFilterFlagV8x4 + iRow*iXUV + iX1UV;
        for(i=0;i< iX2UV; i++)
        {
            uchFlag = *pFilterFlagV ++;
            uchFlag8x4 = *pFilterFlag8x4V ++;
            FilterRow_CtxA8 (pCurrV, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
            FilterRow_CtxA8 (pCurrV + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
            pCurrV += 32;
        }
    }
    //Last 8x4 row of U.    
    pCurrU = ppxliU + (iWidthPrevUV << 3)*(iNumRows >> 1)- (iWidthPrevUV << 2)+ 32*iX1UV;
    pFilterFlag8x4U = puchFilterFlagU8x4+ (iNumRows >> 1) * iXUV+ iX1UV;
    for(i=0;i< iX2UV; i++)  
    {
        uchFlag8x4 = *pFilterFlag8x4U ++;
		FilterRowLast8x4_CtxA8 (pCurrU, (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
		FilterRowLast8x4_CtxA8 (pCurrU + 16, (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
        pCurrU += 32;
    }
	//Last 8x4 row of V.
    pCurrV = ppxliV + (iWidthPrevUV << 3)*(iNumRows >> 1) - (iWidthPrevUV << 2)+ 32*iX1UV;
    pFilterFlag8x4V = puchFilterFlagV8x4 + (iNumRows >> 1)*iXUV + iX1UV;
    for(i=0;i< iX2UV; i++)  
    {
        uchFlag8x4 = *pFilterFlag8x4V ++;
		FilterRowLast8x4_CtxA8 (pCurrV, (uchFlag8x4 >> 4), iWidthPrevUV, iStepSize);
		FilterRowLast8x4_CtxA8 (pCurrV + 16, (uchFlag8x4 & 0xf), iWidthPrevUV, iStepSize);
        pCurrV += 32;
    }
}

Void_WMV DeblockColumnsUpRows (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    I32_WMV iNumCols, I32_WMV iHeight, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilter4x8ColFlagY, U8_WMV* puchFilter4x8ColFlagU, U8_WMV* puchFilter4x8ColFlagV)
{
    I32_WMV iY, iCol;
    U8_WMV uchFlag, *puchFilterFlag, uchFlag4x8, *puchFilterFlag4x8;
    U8_WMV* pCurr;
    I32_WMV iWidthPrevY, iWidthPrevUV;
	I32_WMV iStepSize;
	I32_WMV iRow;

    ppxliY -= 4;
    ppxliU -= 4;
    ppxliV -= 4;
    iStepSize = pWMVDec->m_iStepSize;

    // Filter Y boundary columns
	iWidthPrevY  = pWMVDec->m_iWidthPrevY;
	iY = (iHeight + 31) >> 5;

    for(iRow = 0;iRow < iY;iRow ++)
    {
		puchFilterFlag = (puchFilterFlagY+iRow);
		puchFilterFlag4x8 = (puchFilter4x8ColFlagY+iRow);
        pCurr = ppxliY + 2*(iWidthPrevY << 4)*iRow;
        for(iCol=0;iCol<iNumCols;iCol++)
        {
            uchFlag    = *puchFilterFlag;
            uchFlag4x8 = *puchFilterFlag4x8;
            FilterColumn_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag4x8 >> 4), iWidthPrevY, iStepSize);
		    puchFilterFlag4x8 += iY;
		    puchFilterFlag += iY;
            pCurr += 8;
        }
        
        pCurr = ppxliY+ 2*(iWidthPrevY << 4)*iRow+8*iNumCols -4;
        uchFlag4x8 = *(puchFilter4x8ColFlagY+iRow+iY*iNumCols);
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8) >> 4, iWidthPrevY, iStepSize);
    }

    iWidthPrevUV  = pWMVDec->m_iWidthPrevUV;
	iY = ((iHeight >> 1) + 31) >> 5;
    for(iRow = 0;iRow < iY;iRow ++)
    {
		puchFilterFlag = puchFilterFlagU+iRow;
		puchFilterFlag4x8 = puchFilter4x8ColFlagU+iRow;
        pCurr = ppxliU +  (iWidthPrevUV << 4)*2*iRow;

        for(iCol = 0; iCol< (iNumCols >> 1); iCol++)
        {
            uchFlag    = *puchFilterFlag;
            uchFlag4x8 = *puchFilterFlag4x8;
            FilterColumn_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag4x8 >> 4), iWidthPrevUV, iStepSize);
            puchFilterFlag4x8 += iY;
            puchFilterFlag += iY;
            pCurr += 8;
        }

		pCurr -= 4;
        uchFlag4x8 = *puchFilterFlag4x8;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8) >> 4, iWidthPrevUV, iStepSize);
    }

    for(iRow = 0;iRow < iY;iRow ++)
    {
		puchFilterFlag = puchFilterFlagV+iRow;
		puchFilterFlag4x8 = puchFilter4x8ColFlagV+iRow;
        pCurr = ppxliV +  (iWidthPrevUV << 4)*2*iRow;
        for(iCol = 0; iCol < (iNumCols >> 1);iCol++ )
        {
            uchFlag    = *puchFilterFlag;
			uchFlag4x8 = *puchFilterFlag4x8;
			FilterColumn_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag4x8 >> 4), iWidthPrevUV, iStepSize);
			puchFilterFlag4x8 += iY;
			puchFilterFlag    += iY;
			pCurr += 8;
        }

		pCurr -= 4;
        uchFlag4x8 = *puchFilterFlag4x8;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8) >> 4, iWidthPrevUV, iStepSize);
    }


}
Void_WMV DeblockColumnsDownRows (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    I32_WMV iNumCols, I32_WMV iHeight, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilter4x8ColFlagY, U8_WMV* puchFilter4x8ColFlagU, U8_WMV* puchFilter4x8ColFlagV)
{
    I32_WMV iY, iCol;
    U8_WMV uchFlag, *puchFilterFlag, uchFlag4x8, *puchFilterFlag4x8;
    U8_WMV* pCurr;
    I32_WMV iWidthPrevY, iWidthPrevUV;
	I32_WMV iStepSize;
	I32_WMV iRow;

    ppxliY -= 4;
    ppxliU -= 4;
    ppxliV -= 4;
    iStepSize = pWMVDec->m_iStepSize;

    // Filter Y boundary columns
	iWidthPrevY  = pWMVDec->m_iWidthPrevY;
	iY = (iHeight + 31) >> 5;
    for(iRow = 0;iRow < iY;iRow ++)
     {
        //*******//
		puchFilterFlag    = puchFilterFlagY+iRow;
		puchFilterFlag4x8 = puchFilter4x8ColFlagY+iRow;
        pCurr = ppxliY + 2*(iWidthPrevY << 4)*iRow + (iWidthPrevY << 4);
        for(iCol=0;iCol <iNumCols;iCol++)
        {
            uchFlag    = *puchFilterFlag;
            uchFlag4x8 = *puchFilterFlag4x8;
            FilterColumn_CtxA8 (pCurr, (uchFlag & 0x0f), (uchFlag4x8 & 0x0f), iWidthPrevY, iStepSize);
		    puchFilterFlag4x8 += iY;
		    puchFilterFlag    += iY;
            pCurr += 8;
        }

        pCurr = ppxliY+ 2*(iWidthPrevY << 4)*iRow+(iWidthPrevY << 4)+8*iNumCols -4;
        uchFlag4x8 = *(puchFilter4x8ColFlagY+iRow + iY*iNumCols);
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8 & 0x0f), iWidthPrevY, iStepSize);
    }

    iWidthPrevUV  = pWMVDec->m_iWidthPrevUV;
	iY = ((iHeight >> 1) + 31) >> 5;
    for(iRow = 0;iRow < iY;iRow ++)
    {
	    puchFilterFlag = puchFilterFlagU+iRow;
	    puchFilterFlag4x8 = puchFilter4x8ColFlagU+iRow;
        pCurr = ppxliU +  (iWidthPrevUV << 4)*(2*iRow+1);

	    for(iCol = 0; iCol< (iNumCols >> 1); iCol++)
        {
		    uchFlag    = *puchFilterFlag;
		    uchFlag4x8 = *puchFilterFlag4x8;
		    FilterColumn_CtxA8 (pCurr, (uchFlag & 0x0f), (uchFlag4x8 & 0x0f), iWidthPrevUV, iStepSize);
		    puchFilterFlag4x8 += iY;
		    puchFilterFlag += iY;
		    pCurr += 8;
	    }

	    pCurr -= 4;
        uchFlag4x8 = *puchFilterFlag4x8;
	    FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8 & 0x0f), iWidthPrevUV, iStepSize);
    }

    for(iRow = 0;iRow < iY;iRow ++)
    {
		puchFilterFlag    = puchFilterFlagV+iRow;
		puchFilterFlag4x8 = puchFilter4x8ColFlagV+iRow;
        pCurr = ppxliV +  (iWidthPrevUV << 4)*(2*iRow+1);

        for(iCol = 0; iCol < (iNumCols >> 1); iCol++ )
        {
            uchFlag = *puchFilterFlag;
			uchFlag4x8 = *puchFilterFlag4x8;
			FilterColumn_CtxA8 (pCurr, (uchFlag & 0x0f), (uchFlag4x8 & 0x0f), iWidthPrevUV, iStepSize);
			puchFilterFlag4x8 += iY;
			puchFilterFlag += iY;
			pCurr += 8;
        }

		pCurr -= 4;
        uchFlag4x8 = *puchFilterFlag4x8;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8 & 0x0f), iWidthPrevUV, iStepSize);
    }

}

Void_WMV FilterRow_CtxA8 (U8_WMV* pRow, U8_WMV uchBitField, U8_WMV uchBitField8x4, I32_WMV iPitch, I32_WMV iStepSize)
{
	U8_WMV BitField;
    //assert (!(uchBitField & 0xf0));
    //assert (!(uchBitField8x4 & 0xf0));
	if((uchBitField & 0xf0) || (uchBitField8x4 & 0xf0))
		return;

	//First 8 pixels.
	BitField = (uchBitField>>2) | ((uchBitField8x4>>2) << 4);
	if (BitField & 0x33) {
		g_FilterHorizontalEdgeV9_Fun(pRow, iPitch, iStepSize, BitField);
	}

	//Next 8 pixels.
	BitField = uchBitField | (uchBitField8x4 << 4);
	if (BitField & 0x33) {
		g_FilterHorizontalEdgeV9_Fun(pRow + 8, iPitch, iStepSize, BitField);
	}
}

Void_WMV FilterRowLast8x4_CtxA8 (U8_WMV* pRow, U8_WMV uchBitField8x4, I32_WMV iPitch, I32_WMV iStepSize)
{
    //assert (!(uchBitField8x4 & 0xf0));
	if((uchBitField8x4 & 0xf0))
		return;

	//First 8 pixels.
	if (uchBitField8x4 & 0x0C) {
		g_FilterHorizontalEdgeV9Last8x4_Fun(pRow, iPitch, iStepSize, uchBitField8x4 >> 2);
	}

	//Next 8 pixels.
	if (uchBitField8x4 & 0x03) {
		g_FilterHorizontalEdgeV9Last8x4_Fun(pRow + 8, iPitch, iStepSize, uchBitField8x4);
	}
}

Void_WMV FilterColumn_CtxA8 (U8_WMV* pCol, U8_WMV uchBitField, U8_WMV uchBitField4x8, I32_WMV iPitch, I32_WMV iStepSize)
{
	U8_WMV BitField;
    //assert (!(uchBitField & 0xf0));
    //assert (!(uchBitField4x8 & 0xf0));

	if((uchBitField & 0xf0) || (uchBitField4x8 & 0xf0))
		return;

	//First 8 pixels.
	BitField = (uchBitField>>2) | ((uchBitField4x8>>2) << 4);
	if (BitField & 0x33) {
		g_FilterVerticalEdgeV9_Fun(pCol, iPitch, iStepSize, BitField);
	}

	//Next 8 pixels.
	BitField = uchBitField | (uchBitField4x8 << 4);
	if (BitField & 0x33) {
		g_FilterVerticalEdgeV9_Fun(pCol + (iPitch<<3), iPitch, iStepSize, BitField);
	}
}


Void_WMV FilterColumnLast4x8_CtxA8 (U8_WMV* pCol, U8_WMV uchBitField4x8, I32_WMV iPitch, I32_WMV iStepSize)
{
    //assert (!(uchBitField4x8 & 0xf0));
	if((uchBitField4x8 & 0xf0))
		return;

	//First 8 pixels.
	if (uchBitField4x8 & 0x0C) {
		g_FilterVerticalEdgeV9Last4x8_Fun(pCol, iPitch, iStepSize, uchBitField4x8 >> 2);
	}

	//Next 8 pixels.
	if (uchBitField4x8 & 0x03) {
		g_FilterVerticalEdgeV9Last4x8_Fun(pCol + (iPitch<<3), iPitch, iStepSize, uchBitField4x8);
	}
}

#ifdef VOARMV6
Void_WMV FilterColumn (tWMVDecInternalMember *pWMVDec, U8_WMV* pCol, U8_WMV uchBitField, I32_WMV iPitch, I32_WMV iStepSize)
{
    //assert (!(uchBitField & 0xf0));
	if((uchBitField & 0xf0))
		return;
    switch (uchBitField)
    {
        case 0:
            break;
        case 1:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 12 * iPitch, iPitch, iStepSize, 4);
            break;
        case 2:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 8 * iPitch, iPitch, iStepSize, 4);
            break;
        case 3:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 8 * iPitch, iPitch, iStepSize, 8);
            break;
        case 4:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 4 * iPitch, iPitch, iStepSize, 4);
            break;
        case 5:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 4 * iPitch, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 12 * iPitch, iPitch, iStepSize, 4);
            break;
        case 6:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 4 * iPitch, iPitch, iStepSize, 8);
            break;
        case 7:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 4 * iPitch, iPitch, iStepSize, 12);
            break;
        case 8:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 4);
            break;
        case 9:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 12 * iPitch, iPitch, iStepSize, 4);
            break;
        case 10:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 8 * iPitch, iPitch, iStepSize, 4);
            break;
        case 11:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 8 * iPitch, iPitch, iStepSize, 8);
            break;
        case 12:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 8);
            break;
        case 13:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 8);
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 12 * iPitch, iPitch, iStepSize, 4);
            break;
        case 14:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 12);
            break;
        case 15:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 16);
            break;
    }
}
Void_WMV FilterRow (tWMVDecInternalMember *pWMVDec, U8_WMV* pRow, U8_WMV uchBitField, I32_WMV iPitch, I32_WMV iStepSize)
{
    //assert (!(uchBitField & 0xf0));
	if((uchBitField & 0xf0))
		return;
    switch (uchBitField)
    {
        case 0:
            break;
        case 1:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 12, iPitch, iStepSize, 4);
            break;
        case 2:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 8, iPitch, iStepSize, 4);
            break;
        case 3:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 8, iPitch, iStepSize, 8);
            break;
        case 4:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 4, iPitch, iStepSize, 4);
            break;
        case 5:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 4, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 12, iPitch, iStepSize, 4);
            break;
        case 6:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 4, iPitch, iStepSize, 8);
            break;
        case 7:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 4, iPitch, iStepSize, 12);
            break;
        case 8:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 4);
            break;
        case 9:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 12, iPitch, iStepSize, 4);
            break;
        case 10:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 8, iPitch, iStepSize, 4);
            break;
        case 11:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 8, iPitch, iStepSize, 8);
            break;
        case 12:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 8);
            break;
        case 13:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 8);
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 12, iPitch, iStepSize, 4);
            break;
        case 14:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 12);
            break;
        case 15:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 16);
            break;
    }
}
Void_WMV DeblockRows (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    I32_WMV iNumRows, U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV
)
{
    I32_WMV iX, iRow;
    U8_WMV uchFlag, *puchFilterFlag;
    U8_WMV * pCurr;
	I32_WMV iWidthPrevYUV, iWidthYUV, iStepSize;
    

    // Filter Y boundary rows
 	iWidthPrevYUV = pWMVDec->m_iWidthPrevY;
	iWidthYUV     = pWMVDec->m_iWidthY;
	iStepSize     = pWMVDec->m_iStepSize;
    puchFilterFlag = puchFilterFlagY;
	iRow = 0;
    do  
    {
        pCurr = ppxliY + iRow * 8 * iWidthPrevYUV;
		iX = (iWidthYUV + 31) >> 5;
        do
        {
            uchFlag = *puchFilterFlag ++;
            FilterRow (pWMVDec, pCurr, (uchFlag >> 4), iWidthPrevYUV, iStepSize);
            FilterRow (pWMVDec, pCurr + 16, (uchFlag & 0xf), iWidthPrevYUV, iStepSize);
            pCurr += 32;
			iX --;
        } while ( iX != 0 );

		iRow ++;
    } while ( iRow < iNumRows );

    // Filter U boundary rows
 	iWidthPrevYUV = pWMVDec->m_iWidthPrevUV;
	iWidthYUV     = pWMVDec->m_iWidthUV;
    puchFilterFlag = puchFilterFlagU;
  	iRow = 0;
	if(iNumRows>1)  //zou 330,pWMVDec->m_bCodecIsWVC1
	{
		do
		{
			pCurr = ppxliU + iRow * 8 * iWidthPrevYUV;
			iX = (iWidthYUV + 31) >> 5;
			do
			{
				uchFlag = *puchFilterFlag ++;
				FilterRow (pWMVDec, pCurr, (uchFlag >> 4), iWidthPrevYUV, iStepSize);
				FilterRow (pWMVDec, pCurr + 16, (uchFlag & 0xf), iWidthPrevYUV, iStepSize);
				pCurr += 32;

				iX --;
			} while ( iX != 0 );

			iRow ++;
		} while ( iRow < (iNumRows >> 1) );
	}

    // Filter V boundary rows
    puchFilterFlag = puchFilterFlagV;
  	iRow = 0;
	if(iNumRows>1)  //zou 330,pWMVDec->m_bCodecIsWVC1
	{
		do
		{
			pCurr = ppxliV + iRow * 8 * iWidthPrevYUV;
			iX = (iWidthYUV + 31) >> 5;
			do
			{
				uchFlag = *puchFilterFlag ++;
				FilterRow (pWMVDec, pCurr, (uchFlag >> 4), iWidthPrevYUV, iStepSize);
				FilterRow (pWMVDec, pCurr + 16, (uchFlag & 0xf), iWidthPrevYUV, iStepSize);
				pCurr += 32;

				iX --;
			} while ( iX != 0 );
	        
			iRow ++;
		} while ( iRow < (iNumRows >> 1) );
	}

}
Void_WMV DeblockColumns (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    I32_WMV iNumCols, I32_WMV iHeight, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV
)
{
    I32_WMV iY, iCol;
    U8_WMV uchFlag, *puchFilterFlag;
    U8_WMV* pCurr;
	I32_WMV iWidthPrevYUV, iStepSize;


    ppxliY -= 5;
    ppxliU -= 5;
    ppxliV -= 5;

    // Filter Y boundary columns
    puchFilterFlag = puchFilterFlagY;
	iWidthPrevYUV  = pWMVDec->m_iWidthPrevY;
	iStepSize      = pWMVDec->m_iStepSize;
	iCol = 0;
    do {
        pCurr = ppxliY + iCol * 8;
		iY = (iHeight + 31) >> 5;
        do {
            uchFlag = *puchFilterFlag ++;
            FilterColumn (pWMVDec, pCurr,(uchFlag >> 4), iWidthPrevYUV, iStepSize);
            FilterColumn (pWMVDec, pCurr + 16 * iWidthPrevYUV, (uchFlag & 0xf), iWidthPrevYUV, iStepSize);
            pCurr += (32 * iWidthPrevYUV);

			iY --;
        } while (iY != 0);

		iCol ++;
    } while (iCol < iNumCols);

    // Filter U boundary columns
    puchFilterFlag = puchFilterFlagU;
	iWidthPrevYUV  = pWMVDec->m_iWidthPrevUV;
	iCol = 0; 
    do {
        pCurr = ppxliU + iCol * 8;
		iY = ((iHeight >> 1) + 31) >> 5;
	if((iNumCols >> 1)>=1) //zou 401,pWMVDec->m_bCodecIsWVC1
	{
        do {
            uchFlag = *puchFilterFlag ++;
            FilterColumn (pWMVDec, pCurr, (uchFlag >> 4), iWidthPrevYUV, iStepSize);
            FilterColumn (pWMVDec, pCurr + 16 * iWidthPrevYUV, (uchFlag & 0xf), iWidthPrevYUV, iStepSize);
            pCurr += (32 * iWidthPrevYUV);
            
			iY --;
        } while (iY != 0);
	}

		iCol ++;
    } while (iCol < (iNumCols >> 1));

    // Filter V boundary columns
    puchFilterFlag = puchFilterFlagV;
	iCol = 0; 
    do {
        pCurr = ppxliV + iCol * 8;
		iY = ((iHeight >> 1) + 31) >> 5;
	if((iNumCols >> 1)>=1) //zou 401,pWMVDec->m_bCodecIsWVC1
	{
        do {
            uchFlag = *puchFilterFlag ++;
            FilterColumn (pWMVDec, pCurr, (uchFlag >> 4), iWidthPrevYUV, iStepSize);
            FilterColumn (pWMVDec, pCurr + 16 * iWidthPrevYUV, (uchFlag & 0xf), iWidthPrevYUV, iStepSize);
            pCurr += (32 * iWidthPrevYUV);
            
			iY --;
        } while (iY != 0);
	}

		iCol ++;
    } while (iCol < (iNumCols >> 1));
}
#else
Void_WMV DeblockRows (tWMVDecInternalMember *pWMVDec, int bottom_row,
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,I32_WMV iNumRows, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilterFlagY8x4, U8_WMV* puchFilterFlagU8x4, U8_WMV* puchFilterFlagV8x4)
{

    //pWMVDec->do_horizontal_deblock = 1;

    
        //The Left cols and Right cols are independent
        // First half cols. Filter Y U V boundary rows
        DeblockRowsLeftCols(pWMVDec, bottom_row,
                                    ppxliY, ppxliU, ppxliV, iNumRows, 
                                    puchFilterFlagY, puchFilterFlagU, puchFilterFlagV,
	                                puchFilterFlagY8x4, puchFilterFlagU8x4, puchFilterFlagV8x4);    

        //**********************************
        //Next half cols. Filter Y U V  boundary rows
        DeblockRowsRightCols(pWMVDec, bottom_row,
                                    ppxliY, ppxliU, ppxliV, iNumRows, 
                                    puchFilterFlagY, puchFilterFlagU, puchFilterFlagV,
	                                puchFilterFlagY8x4, puchFilterFlagU8x4, puchFilterFlagV8x4);
}
Void_WMV DeblockColumns (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    I32_WMV iNumCols, I32_WMV iHeight, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilter4x8ColFlagY, U8_WMV* puchFilter4x8ColFlagU, U8_WMV* puchFilter4x8ColFlagV)
{
    //pWMVDec->do_horizontal_deblock = 0;
   
    {
        DeblockColumnsUpRows(pWMVDec, 
            ppxliY, ppxliU, ppxliV,
            iNumCols, iHeight, 
            puchFilterFlagY, puchFilterFlagU, puchFilterFlagV,
	        puchFilter4x8ColFlagY, puchFilter4x8ColFlagU, puchFilter4x8ColFlagV);


        DeblockColumnsDownRows(pWMVDec, 
            ppxliY, ppxliU, ppxliV,
            iNumCols, iHeight, 
            puchFilterFlagY, puchFilterFlagU, puchFilterFlagV,
	        puchFilter4x8ColFlagY, puchFilter4x8ColFlagU, puchFilter4x8ColFlagV);
    }
    
}
#endif

#define LOOPMBROWS 1
void DeblockMBRow(tWMVDecInternalMember *pWMVDec,U32_WMV mb_row)
{
	U32_WMV firstmbrow_of_slice=0;
	U32_WMV bottomrow_of_slice=0;
	U32_WMV iSliceHead		=  pWMVDec->iSliceHead;
	U32_WMV iSliceHead_Pre	= pWMVDec->iSliceHead_Pre;
	U32_WMV i8OffsetY = 8 * pWMVDec->m_iWidthPrevY;
	U32_WMV i8OffsetUV = 8 * pWMVDec->m_iWidthPrevUV;
	I32_WMV  iNumMBX = pWMVDec->m_uintNumMBX;
	LOOPF_FLAG* pLoopFilterFlag = pWMVDec->m_pLoopFilterFlags;
	U8_WMV* ppxliCurrY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp; 
	U8_WMV* ppxliCurrU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
	U8_WMV* ppxliCurrV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;

	U8_WMV* puchFilter8x8RowFlagY = pWMVDec->m_puchFilter8x8RowFlagY0;
    U8_WMV* puchFilter8x8RowFlagU = pWMVDec->m_puchFilter8x8RowFlagU0;
    U8_WMV* puchFilter8x8RowFlagV = pWMVDec->m_puchFilter8x8RowFlagV0;
    U8_WMV* puchFilter8x4RowFlagY = pWMVDec->m_puchFilter8x4RowFlagY0;
    U8_WMV* puchFilter8x4RowFlagU = pWMVDec->m_puchFilter8x4RowFlagU0;
    U8_WMV* puchFilter8x4RowFlagV = pWMVDec->m_puchFilter8x4RowFlagV0;
    U8_WMV* puchFilter8x8ColFlagY = pWMVDec->m_puchFilter8x8ColFlagY0;
    U8_WMV* puchFilter8x8ColFlagU = pWMVDec->m_puchFilter8x8ColFlagU0;
    U8_WMV* puchFilter8x8ColFlagV = pWMVDec->m_puchFilter8x8ColFlagV0;
    U8_WMV* puchFilter4x8ColFlagY = pWMVDec->m_puchFilter4x8ColFlagY0;
    U8_WMV* puchFilter4x8ColFlagU = pWMVDec->m_puchFilter4x8ColFlagU0;
    U8_WMV* puchFilter4x8ColFlagV = pWMVDec->m_puchFilter4x8ColFlagV0;
	if(pWMVDec->m_pbStartOfSliceRow[mb_row] || mb_row ==0)
	{
		pWMVDec->iSliceHead_Pre = pWMVDec->iSliceHead;
		pWMVDec->iSliceHead = mb_row;

		iSliceHead		=  pWMVDec->iSliceHead;
		iSliceHead_Pre	= pWMVDec->iSliceHead_Pre;

		firstmbrow_of_slice = 1;
	}
	else
	{
		firstmbrow_of_slice = 0;				
	}
	
	//parpare flags
	if(firstmbrow_of_slice)
	{
		GenerateRowLoopFilterFlags_Y(pWMVDec,
				puchFilter8x8RowFlagY ,
				puchFilter8x4RowFlagY ,
				pLoopFilterFlag + (iSliceHead)*iNumMBX, firstmbrow_of_slice);

		GenerateRowLoopFilterFlags_UV(pWMVDec,
			puchFilter8x8RowFlagU,puchFilter8x8RowFlagV,
			puchFilter8x4RowFlagU,puchFilter8x4RowFlagV,
			pLoopFilterFlag  + (iSliceHead)*iNumMBX, firstmbrow_of_slice);
	}
	else
	{
		GenerateRowLoopFilterFlags_Y(pWMVDec,
				puchFilter8x8RowFlagY + (2*(mb_row-iSliceHead)-1)*((iNumMBX + 1) >> 1), 
				puchFilter8x4RowFlagY + 2*(mb_row-iSliceHead)*((iNumMBX + 1) >> 1),
				pLoopFilterFlag + (iSliceHead+(mb_row-iSliceHead))*iNumMBX,firstmbrow_of_slice);

		GenerateRowLoopFilterFlags_UV(pWMVDec,
			puchFilter8x8RowFlagU+((mb_row-iSliceHead)-1)*((iNumMBX + 3) >>2),
			puchFilter8x8RowFlagV+((mb_row-iSliceHead)-1)*((iNumMBX + 3) >>2),
			puchFilter8x4RowFlagU+(mb_row-iSliceHead)*((iNumMBX + 3) >>2), 
			puchFilter8x4RowFlagV+(mb_row-iSliceHead)*((iNumMBX + 3) >>2),
			pLoopFilterFlag  + (iSliceHead+(mb_row-iSliceHead))*iNumMBX, firstmbrow_of_slice);
	}

	GenerateColumnLoopFilterFlags_Y (pWMVDec,
			puchFilter8x8ColFlagY+((mb_row-iSliceHead)%2)*2*iNumMBX,
			puchFilter4x8ColFlagY+((mb_row-iSliceHead)%2)*2*iNumMBX,
			pLoopFilterFlag+(iSliceHead+(mb_row-iSliceHead))*iNumMBX, 1);

	GenerateColumnLoopFilterFlags_UV (pWMVDec,
		puchFilter8x8ColFlagU+ ((mb_row-iSliceHead)%2)*iNumMBX, 
		puchFilter8x8ColFlagV+ ((mb_row-iSliceHead)%2)*iNumMBX,
		puchFilter4x8ColFlagU+ ((mb_row-iSliceHead)%2)*iNumMBX, 
		puchFilter4x8ColFlagV+ ((mb_row-iSliceHead)%2)*iNumMBX,
		pLoopFilterFlag+(iSliceHead+(mb_row-iSliceHead))*iNumMBX, 1);

	//deblock pre mb row
	if((mb_row-iSliceHead))
	{
		DeblockRows_mbrow_y(pWMVDec, 0,
                    ppxliCurrY + i8OffsetY + (pWMVDec->m_iWidthPrevY << 3)*(2*((mb_row-iSliceHead)-1+iSliceHead)),
					puchFilter8x8RowFlagY + ((pWMVDec->m_iWidthY + 31) >> 5)*(2*((mb_row-iSliceHead)-1)),
					puchFilter8x4RowFlagY + ((pWMVDec->m_iWidthY + 31) >> 5)*(2*((mb_row-iSliceHead)-1)));
					//2*(mb_row-1));

		DeblockRows_mbrow_y(pWMVDec, 0,
                    ppxliCurrY + i8OffsetY + (pWMVDec->m_iWidthPrevY << 3)*(2*((mb_row-iSliceHead)-1+iSliceHead)+1),
					puchFilter8x8RowFlagY+ ((pWMVDec->m_iWidthY + 31) >> 5)*(2*((mb_row-iSliceHead)-1)+1),
					puchFilter8x4RowFlagY+ ((pWMVDec->m_iWidthY + 31) >> 5)*(2*((mb_row-iSliceHead)-1)+1));
					//2*(mb_row-1)+1);					

		DeblockRows_mbrow_uv(pWMVDec, 0,
                    ppxliCurrU + i8OffsetUV + (pWMVDec->m_iWidthPrevUV << 3) *((mb_row-iSliceHead)-1+iSliceHead),
					ppxliCurrV + i8OffsetUV + (pWMVDec->m_iWidthPrevUV << 3) *((mb_row-iSliceHead)-1+iSliceHead),
					puchFilter8x8RowFlagU+ ((pWMVDec->m_iWidthUV + 31) >> 5)*((mb_row-iSliceHead)-1),
					puchFilter8x8RowFlagV+ ((pWMVDec->m_iWidthUV + 31) >> 5)*((mb_row-iSliceHead)-1),
					puchFilter8x4RowFlagU+ ((pWMVDec->m_iWidthUV + 31) >> 5)*((mb_row-iSliceHead)-1),
					puchFilter8x4RowFlagV+ ((pWMVDec->m_iWidthUV + 31) >> 5)*((mb_row-iSliceHead)-1));
					//(mb_row-1));

		DeblockColumns_mbrow_y(pWMVDec,ppxliCurrY + 8+ (pWMVDec->m_iWidthPrevY << 4)*((mb_row-iSliceHead)-1+iSliceHead),
					pWMVDec->m_uintNumMBX * 2 - 1,1 << 4,
					puchFilter8x8ColFlagY+2*(((mb_row-iSliceHead)-1)%2)*iNumMBX,
					puchFilter4x8ColFlagY+2*(((mb_row-iSliceHead)-1)%2)*iNumMBX);//,(mb_row-1));

		DeblockColumns_mbrow_uv(pWMVDec,
							ppxliCurrU + 8+ (pWMVDec->m_iWidthPrevUV << 3)*((mb_row-iSliceHead)-1+iSliceHead),
							ppxliCurrV + 8+ (pWMVDec->m_iWidthPrevUV  << 3)*((mb_row-iSliceHead)-1+iSliceHead),
							pWMVDec->m_uintNumMBX * 2 - 1,1 << 4,
							puchFilter8x8ColFlagU+ (((mb_row-iSliceHead)-1)%2)*iNumMBX,
							puchFilter8x8ColFlagV+ (((mb_row-iSliceHead)-1)%2)*iNumMBX,
							puchFilter4x8ColFlagU+ (((mb_row-iSliceHead)-1)%2)*iNumMBX,
							puchFilter4x8ColFlagV+ (((mb_row-iSliceHead)-1)%2)*iNumMBX);//,(mb_row-1));
	}

	if(mb_row+1>0 && (pWMVDec->m_pbStartOfSliceRow[mb_row+1]||mb_row+1 == pWMVDec->m_uintNumMBY))//current is the last mb row of this silce 
	{
		DeblockRows_mbrow_y(pWMVDec, 0,
                        ppxliCurrY + i8OffsetY + (pWMVDec->m_iWidthPrevY << 3)*(2*((mb_row+1-iSliceHead)-1+iSliceHead)),
						puchFilter8x8RowFlagY+ ((pWMVDec->m_iWidthY + 31) >> 5)*(2*((mb_row+1-iSliceHead)-1)),
						puchFilter8x4RowFlagY+ ((pWMVDec->m_iWidthY + 31) >> 5)*(2*((mb_row+1-iSliceHead)-1)));

		DeblockRows_mbrow_y(pWMVDec, 1,
                    ppxliCurrY + i8OffsetY + (pWMVDec->m_iWidthPrevY << 3)*(2*((mb_row+1-iSliceHead)-1+iSliceHead)+1),
					puchFilter8x8RowFlagY+ ((pWMVDec->m_iWidthY + 31) >> 5)*(2*((mb_row+1-iSliceHead)-1)+1),
					puchFilter8x4RowFlagY+ ((pWMVDec->m_iWidthY + 31) >> 5)*(2*((mb_row+1-iSliceHead)-1)+1));			

		DeblockRows_mbrow_uv(pWMVDec, 1,
                    ppxliCurrU + i8OffsetUV + (pWMVDec->m_iWidthPrevUV << 3) *((mb_row+1-iSliceHead)-1+iSliceHead),
					ppxliCurrV + i8OffsetUV + (pWMVDec->m_iWidthPrevUV << 3) *((mb_row+1-iSliceHead)-1+iSliceHead),
					puchFilter8x8RowFlagU + ((pWMVDec->m_iWidthUV + 31) >> 5)*((mb_row+1-iSliceHead)-1),
					puchFilter8x8RowFlagV+ ((pWMVDec->m_iWidthUV + 31) >> 5)*((mb_row+1-iSliceHead)-1),
					puchFilter8x4RowFlagU+ ((pWMVDec->m_iWidthUV + 31) >> 5)*((mb_row+1-iSliceHead)-1),
					puchFilter8x4RowFlagV+ ((pWMVDec->m_iWidthUV + 31) >> 5)*((mb_row+1-iSliceHead)-1));

		DeblockColumns_mbrow_y(pWMVDec,ppxliCurrY + 8+ (pWMVDec->m_iWidthPrevY << 4)*((mb_row+1-iSliceHead)-1+iSliceHead),
					pWMVDec->m_uintNumMBX * 2 - 1,1 << 4,
					puchFilter8x8ColFlagY+2*(((mb_row+1-iSliceHead)-1)%2)*pWMVDec->m_uintNumMBX,
					puchFilter4x8ColFlagY+2*(((mb_row+1-iSliceHead)-1)%2)*pWMVDec->m_uintNumMBX);//,(mb_row-1));

		DeblockColumns_mbrow_uv(pWMVDec,
							ppxliCurrU + 8+ (pWMVDec->m_iWidthPrevUV << 3)*((mb_row+1-iSliceHead)-1+iSliceHead),
							ppxliCurrV + 8+ (pWMVDec->m_iWidthPrevUV  << 3)*((mb_row+1-iSliceHead)-1+iSliceHead),
							pWMVDec->m_uintNumMBX * 2 - 1,1 << 4,
							puchFilter8x8ColFlagU+ (((mb_row+1-iSliceHead)-1)%2)*pWMVDec->m_uintNumMBX,
							puchFilter8x8ColFlagV+ (((mb_row+1-iSliceHead)-1)%2)*pWMVDec->m_uintNumMBX,
							puchFilter4x8ColFlagU+ (((mb_row+1-iSliceHead)-1)%2)*pWMVDec->m_uintNumMBX,
							puchFilter4x8ColFlagV+ (((mb_row+1-iSliceHead)-1)%2)*pWMVDec->m_uintNumMBX);//,(mb_row-1));
	}
}
Void_WMV DeblockSLFrame_V9 (tWMVDecInternalMember *pWMVDec,U32_WMV uiMBStart, U32_WMV uiMBEnd)
{
	U32_WMV mb_row = 0;  
	U8_WMV* ppxliCurrY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
	U8_WMV* ppxliCurrU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	U8_WMV* ppxliCurrV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	LOOPF_FLAG* pLoopFilterFlag = pWMVDec->m_pLoopFilterFlags;
	
	for(mb_row = 0;mb_row< pWMVDec->m_uintNumMBY;mb_row++)
	{
		DeblockMBRow(pWMVDec,mb_row);			
	}
}

Void_WMV g_InitRepeatRefInfo (tWMVDecInternalMember *pWMVDec, Bool_WMV bAdvancedProfile)
{
    
    pWMVDec->pRepeatRef0Y = g_RepeatRef0Y; 
    pWMVDec->pRepeatRef0UV = g_RepeatRef0UV;

    EXPANDY_REFVOP = pWMVDec->EXPANDY_REFVOP;
    EXPANDUV_REFVOP = pWMVDec->EXPANDUV_REFVOP;

    if (bAdvancedProfile) {
        pWMVDec->pRepeatRef0Y = g_RepeatRef0Y_AP; 
        pWMVDec->pRepeatRef0UV = g_RepeatRef0UV_AP;
        return;
    }
}

Void_WMV g_RepeatRef0Y_AP (
    U8_WMV* ppxlcRef0Y,
    CoordI  iStart, 
    CoordI  iEnd,
    I32_WMV     iOldLeftOffet,
    Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthY,
    I32_WMV     iWidthYPlusExp,
    I32_WMV     iWidthPrevY,
    Bool_WMV    bProgressive
)
{
    const U8_WMV* ppxliOldLeft = ppxlcRef0Y + iOldLeftOffet;
    const U8_WMV* ppxliOldTopLn = ppxliOldLeft - EXPANDY_REFVOP;
    U8_WMV* ppxliOldRight = (U8_WMV *) ppxliOldLeft + iWidthY - 1;
    U8_WMV* ppxliNew = (U8_WMV*) ppxliOldTopLn;
    U8_WMV* ppxliNewPlusWidth;
    I32_WMV iTrueWidth = iWidthY + ((-iWidthY) & 0xf) + 2 * EXPANDY_REFVOP; // width + alignment + horizontal repeatpad
    I32_WMV iTrueWidthMinusExp = iTrueWidth - EXPANDY_REFVOP;
    I32_WMV iVertPad;
    CoordI y;
    I32_WMV iWidthPrevYDiv8 = iTrueWidth >> 3;
    U8_WMV* ppxliSrc;
    U8_WMV* ppxliDst;

    if (bProgressive)
        iVertPad = 36;  // 32 pixels of additional access + 4 pixels for interpolator (16 + 2 lines in interlace resolution)
    else
        iVertPad = 18;  // 16 pixels of additional access + 2 pixels for interpolator

    for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        U64 uipadValueLeft;
        U64 uipadValueRight;
        I32_WMV x;
        uipadValueLeft = *ppxliOldLeft;    
        uipadValueRight = *ppxliOldRight;
        uipadValueLeft |= (uipadValueLeft << 8);
        uipadValueRight |= (uipadValueRight << 8);
        uipadValueLeft |= (uipadValueLeft << 16);
        uipadValueRight |= (uipadValueRight << 16);
        uipadValueLeft |= (uipadValueLeft << 32);
        uipadValueRight |= (uipadValueRight << 32);
        ppxliNewPlusWidth = ppxliNew + iTrueWidthMinusExp; //+ iWidthYPlusExp;
        /** tight overlap on right **/
        for ( x = 0; x < ((-iWidthY) & 0xf); x++)
            ppxliOldRight[x + 1] = ppxliOldRight[0];
        for (x = 0; x < EXPANDY_REFVOP; x += 8) {
            *(U64 *)(ppxliNew + x) = uipadValueLeft;
            *(U64 *)(ppxliNewPlusWidth + x) = uipadValueRight;
        }
        ppxliNew += iWidthPrevY;
        ppxliOldLeft += iWidthPrevY;
        ppxliOldRight += iWidthPrevY;
    }    
    
    if (fTop) {
        U8_WMV* ppxliLeftTop = (U8_WMV *) ppxliOldTopLn - iVertPad * iWidthPrevY;
        for (y = 0; y < iVertPad; y++) {
            I32_WMV x;
            //memcpy (ppxliLeftTop, ppxliOldTopLn, iWidthPrevY);
            ppxliSrc = (U8_WMV*) ppxliOldTopLn;
            ppxliDst = (U8_WMV*) ppxliLeftTop;
            for ( x = 0; x < iWidthPrevYDiv8; x++) {
                *(U64 *) ppxliDst = *(U64 *) ppxliSrc;
                ppxliSrc += 8;
                ppxliDst += 8;
            }
            ppxliLeftTop += iWidthPrevY;
        }
    }
    if (fBottom) {
        const U8_WMV* ppxliOldBotLn = ppxliNew - iWidthPrevY;
        /** disabled
        iVertPad += iLastVert; // Part of hack ... remove when fix is ready.
        **/

        if (bProgressive)
            iVertPad += ((-iEnd) & 0x1f);
        else
            iVertPad += ((-iEnd) & 0xf);

        for (y = 0; y < iVertPad; y++) {
            I32_WMV x = 0;
            //memcpy (ppxliNew, ppxliOldBotLn, iWidthPrevY);
            ppxliSrc = (U8_WMV*) ppxliOldBotLn;
            ppxliDst = (U8_WMV*) ppxliNew;
            for ( x = 0; x < iWidthPrevYDiv8; x++) {
                *(U64 *) ppxliDst = *(U64 *) ppxliSrc;
                ppxliSrc += 8;
                ppxliDst += 8;
            }
            ppxliNew += iWidthPrevY;
        }
    }
}

Void_WMV g_RepeatRef0UV_AP (
    U8_WMV* ppxlcRef0U,
    U8_WMV* ppxlcRef0V,
    CoordI  iStart, CoordI iEnd,
    I32_WMV     iOldLeftOffet,
    Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthUV,
    I32_WMV     iWidthUVPlusExp,
    I32_WMV     iWidthPrevUV,
    Bool_WMV    bProgressive
)
{
    const U8_WMV* ppxliOldLeftU = ppxlcRef0U + iOldLeftOffet;
    const U8_WMV* ppxliOldLeftV = ppxlcRef0V + iOldLeftOffet;
    U8_WMV* ppxliOldRightU = (U8_WMV *) ppxliOldLeftU + iWidthUV - 1;
    U8_WMV* ppxliOldRightV = (U8_WMV *) ppxliOldLeftV + iWidthUV - 1;
    const U8_WMV* ppxliOldTopLnU = ppxliOldLeftU - EXPANDUV_REFVOP;
    const U8_WMV* ppxliOldTopLnV = ppxliOldLeftV - EXPANDUV_REFVOP;
    U8_WMV* ppxliNewU = (U8_WMV*) ppxliOldTopLnU;
    U8_WMV* ppxliNewV = (U8_WMV*) ppxliOldTopLnV;
    U8_WMV* ppxliNewUPlusWidth;
    U8_WMV* ppxliNewVPlusWidth;
    I32_WMV iTrueWidth = iWidthUV + ((-iWidthUV) & 7) + 2 * EXPANDUV_REFVOP; // width + alignment + horizontal repeatpad
    I32_WMV iTrueWidthMinusExp = iTrueWidth - EXPANDUV_REFVOP;
    I32_WMV iVertPad;
    CoordI y;

    I32_WMV iWidthPrevUVDiv8 = iTrueWidth >> 2;
    U8_WMV* ppxliSrcU;
    U8_WMV* ppxliSrcV;
    U8_WMV* ppxliDstU;
    U8_WMV* ppxliDstV;

    if (bProgressive)
        iVertPad = 18;  // 16 pixels of additional access + 2 pixels for interpolator (8 + 1 lines in interlace resolution)
    else
        iVertPad = 9;   // 8 pixels of additional access + 1 pixel for interpolator
    
    for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        U64 uipadValueLeftU, uipadValueLeftV;
        U64 uipadValueRightU, uipadValueRightV;
        I32_WMV x;
        uipadValueLeftU = *ppxliOldLeftU;
        uipadValueLeftV = *ppxliOldLeftV;
        uipadValueLeftU |= (uipadValueLeftU << 8);
        uipadValueLeftV |= (uipadValueLeftV << 8);
        uipadValueLeftU |= (uipadValueLeftU << 16);
        uipadValueLeftV |= (uipadValueLeftV << 16);
        uipadValueLeftU |= (uipadValueLeftU << 32);
        uipadValueLeftV |= (uipadValueLeftV << 32);
        uipadValueRightU = *ppxliOldRightU;
        uipadValueRightV = *ppxliOldRightV;
        uipadValueRightU |= (uipadValueRightU << 8);
        uipadValueRightV |= (uipadValueRightV << 8);
        uipadValueRightU |= (uipadValueRightU << 16);
        uipadValueRightV |= (uipadValueRightV << 16);
        uipadValueRightU |= (uipadValueRightU << 32);
        uipadValueRightV |= (uipadValueRightV << 32);
        ppxliNewUPlusWidth = ppxliNewU + iTrueWidthMinusExp; // + iWidthUVPlusExp;
        ppxliNewVPlusWidth = ppxliNewV + iTrueWidthMinusExp; //iWidthUVPlusExp;
        /** tight overlap on right **/
        for ( x = 0; x < ((-iWidthUV) & 0x7); x++) {
            ppxliOldRightU[x + 1] = ppxliOldRightU[0];
            ppxliOldRightV[x + 1] = ppxliOldRightV[0];
        }
        for (x = 0; x < EXPANDUV_REFVOP; x += 8) {
            *(U64 *)(ppxliNewU + x) = uipadValueLeftU;
            *(U64 *)(ppxliNewV + x) = uipadValueLeftV;
            *(U64 *)(ppxliNewUPlusWidth + x) = uipadValueRightU;
            *(U64 *)(ppxliNewVPlusWidth + x) = uipadValueRightV;
        }
        ppxliNewU += iWidthPrevUV;        
        ppxliNewV += iWidthPrevUV;
        ppxliOldLeftU += iWidthPrevUV;
        ppxliOldLeftV += iWidthPrevUV;
        ppxliOldRightU += iWidthPrevUV;
        ppxliOldRightV += iWidthPrevUV;
    }
    
    if (fTop) {
        U8_WMV* ppxliLeftTopU = (U8_WMV *) ppxliOldTopLnU - iVertPad * iWidthPrevUV;
        U8_WMV* ppxliLeftTopV = (U8_WMV *) ppxliOldTopLnV - iVertPad * iWidthPrevUV;
        for (y = 0; y < iVertPad; y++) {
            I32_WMV x;
            ppxliSrcU = (U8_WMV*) ppxliOldTopLnU;
            ppxliSrcV = (U8_WMV*) ppxliOldTopLnV;
            ppxliDstU = ppxliLeftTopU;
            ppxliDstV = ppxliLeftTopV;
            for ( x = 0; x < iWidthPrevUVDiv8; x++) {
                *(U32_WMV *) ppxliDstU = *(U32_WMV *) ppxliSrcU;
                ppxliSrcU += 4;
                ppxliDstU += 4;
                *(U32_WMV *) ppxliDstV = *(U32_WMV *) ppxliSrcV;
                ppxliSrcV += 4;
                ppxliDstV += 4;
            }
            ppxliLeftTopU += iWidthPrevUV;
            ppxliLeftTopV += iWidthPrevUV;
        }
    }
    if (fBottom) {
        /** disabled
        iVertPad += iLastVert; // Part of hack ... remove when fix is ready.
        **/
        I32_WMV x;
        const U8_WMV* ppxliOldBotLnU = ppxliNewU - iWidthPrevUV;
        const U8_WMV* ppxliOldBotLnV = ppxliNewV - iWidthPrevUV;

        if (bProgressive)
            iVertPad += ((-iEnd) & 0xf);
        else
            iVertPad += ((-iEnd) & 0x7);

        for (y = 0; y < iVertPad; y++) {
            ppxliSrcU = (U8_WMV*) ppxliOldBotLnU;
            ppxliSrcV = (U8_WMV*) ppxliOldBotLnV;
            ppxliDstU = ppxliNewU;
            ppxliDstV = ppxliNewV;
            for ( x = 0; x < iWidthPrevUVDiv8; x++) {
                *(U32_WMV *) ppxliDstU = *(U32_WMV *) ppxliSrcU;
                ppxliSrcU += 4;
                ppxliDstU += 4;
                *(U32_WMV *) ppxliDstV = *(U32_WMV *) ppxliSrcV;
                ppxliSrcV += 4;
                ppxliDstV += 4;
            }
            ppxliNewU += iWidthPrevUV;
            ppxliNewV += iWidthPrevUV;
        }
    }
}


#define EXTRACT0_ROWX4(x) ((x & 0xc0) << 0)
#define EXTRACT1_ROWX4(x) ((x & 0xc0) >> 2)
#define EXTRACT2_ROWX4(x) ((x & 0xc0) >> 4)
#define EXTRACT3_ROWX4(x) ((x & 0xc0) >> 6)
#define EXTRACT0_ROWX8(x) ((x & 0xc) << 4) 
#define EXTRACT1_ROWX8(x) ((x & 0xc) << 2) 
#define EXTRACT2_ROWX8(x) ((x & 0xc) >> 0) 
#define EXTRACT3_ROWX8(x) ((x & 0xc) >> 2) 

#define EXTRACT0_COLX4(x) ((x & 0x30) << 2)
#define EXTRACT1_COLX4(x) ((x & 0x30) << 0)
#define EXTRACT2_COLX4(x) ((x & 0x30) >> 2)
#define EXTRACT3_COLX4(x) ((x & 0x30) >> 4)
#define EXTRACT0_COLX8(x) ((x & 0x3) << 6) 
#define EXTRACT1_COLX8(x) ((x & 0x3) << 4) 
#define EXTRACT2_COLX8(x) ((x & 0x3) << 2) 
#define EXTRACT3_COLX8(x) ((x & 0x3) >> 0)

Void_WMV GenerateRowLoopFilterFlags_first_mbrow (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* puchFilterRowFlagY, U8_WMV* puchFilterRowFlagU, U8_WMV* puchFilterRowFlagV,
    U8_WMV* puchFilter8x4RowFlagY, U8_WMV* puchFilter8x4RowFlagU, U8_WMV* puchFilter8x4RowFlagV,
    LOOPF_FLAG* pLoopFilterFlag, I32_WMV iMBYStart, I32_WMV iMBYEnd, Bool_WMV bFirstRow)
{
    U8_WMV* puch8x4FilterRow0 = puchFilter8x4RowFlagY;
    U8_WMV* puch8x4FilterRow1 = puchFilter8x4RowFlagY + ((pWMVDec->m_uintNumMBX + 1) >> 1);
    U8_WMV* puch8x8FilterRow0 = puchFilterRowFlagY;
    U8_WMV* puch8x8FilterRow1 = puchFilterRowFlagY + ((pWMVDec->m_uintNumMBX + 1) >> 1);
    U8_WMV uch8x4Flag0, uch8x4Flag1, uch8x8Flag0, uch8x8Flag1;
    U8_WMV uch8x8FlagU, uch8x8FlagV, uch8x4FlagU, uch8x4FlagV;
    I32_WMV  i4MBX, iMBY;
    I32_WMV  iNumMBX = pWMVDec->m_uintNumMBX;
    
    uch8x4Flag0 = 0, uch8x4Flag1 = 0, uch8x8Flag0 = 0, uch8x8Flag1 = 0;
    uch8x8FlagU = 0, uch8x8FlagV = 0, uch8x4FlagU = 0, uch8x4FlagV = 0;
    
    if (bFirstRow)
    {
        iMBYStart = 1;
		i4MBX = 0;
      if(iNumMBX>3)//zou 401,pWMVDec->m_bCodecIsWVC1
      {
          do
          {
			    // Extract flag from pLoopFilterFlag
			    uch8x4Flag0  = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
				uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
				uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [0]);
                uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [1]);

                uch8x4Flag1  = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
				uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [2]);
                uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [3]);

                uch8x8Flag0  = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
				uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 1)->chFlag [2]);
                uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 1)->chFlag [3]);

                uch8x4FlagU  = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV  = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);       
                uch8x4FlagU |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [4]);
                uch8x4FlagV |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [5]);

                *puch8x4FilterRow0++ = uch8x4Flag0;
                *puch8x4FilterRow1++ = uch8x4Flag1;
                *puch8x8FilterRow0++ = uch8x8Flag0;

				//
                uch8x4Flag0 = EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [0]);
                uch8x4Flag0 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [1]);
				uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [0]);
                uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [1]);

                uch8x4Flag1 = EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [2]);
                uch8x4Flag1 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [3]);
				uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [2]);
                uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [3]);

                uch8x8Flag0 = EXTRACT0_ROWX8((pLoopFilterFlag + 2)->chFlag [2]);
                uch8x8Flag0 |= EXTRACT1_ROWX8((pLoopFilterFlag + 2)->chFlag [3]);
				uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 3)->chFlag [2]);
                uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [3]);

                uch8x4FlagU |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [4]);
                uch8x4FlagV |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [5]);
                uch8x4FlagU |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [4]);
                uch8x4FlagV |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [5]);


                *puch8x4FilterRow0++ = uch8x4Flag0;
                *puch8x4FilterRow1++ = uch8x4Flag1;
                *puch8x8FilterRow0++ = uch8x8Flag0;
                *puchFilter8x4RowFlagU++ = uch8x4FlagU;
                *puchFilter8x4RowFlagV++ = uch8x4FlagV;
                pLoopFilterFlag += 4;

            } while ( ++i4MBX < iNumMBX >> 2);
      }

        if (iNumMBX & 3)
        {
            uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
            uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
            uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
            uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
            uch8x4FlagU = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
            uch8x4FlagV = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);

            pLoopFilterFlag ++;

            if ((iNumMBX & 2) == 2)
            {
                uch8x4Flag0 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag0 |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag0 |= EXTRACT3_ROWX8(pLoopFilterFlag->chFlag [3]);
                uch8x4FlagU |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;
            }
            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;

            if ((iNumMBX & 3) == 3)
            {
                uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
                uch8x4FlagU |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;

                *puch8x4FilterRow0++ = uch8x4Flag0;
                *puch8x4FilterRow1++ = uch8x4Flag1;
                *puch8x8FilterRow0++ = uch8x8Flag0;
            }
            *puchFilter8x4RowFlagU++ = uch8x4FlagU;
            *puchFilter8x4RowFlagV++ = uch8x4FlagV;
        }
        puch8x8FilterRow1 = puch8x8FilterRow0 + ((iNumMBX + 1) >> 1);
        puch8x4FilterRow0 = puch8x4FilterRow1;
        puch8x4FilterRow1 = puch8x4FilterRow1 + ((iNumMBX + 1) >> 1);
    }
}

Void_WMV GenerateRowLoopFilterFlags_Y (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* puchFilterRowFlagY,    U8_WMV* puchFilter8x4RowFlagY,
    LOOPF_FLAG* pLoopFilterFlag, Bool_WMV bFirstRow)
{
    U8_WMV* puch8x4FilterRow0 = puchFilter8x4RowFlagY;
    U8_WMV* puch8x4FilterRow1 = puchFilter8x4RowFlagY + ((pWMVDec->m_uintNumMBX + 1) >> 1);
    U8_WMV* puch8x8FilterRow0 = puchFilterRowFlagY;
    U8_WMV* puch8x8FilterRow1 = puchFilterRowFlagY + ((pWMVDec->m_uintNumMBX + 1) >> 1);
    U8_WMV uch8x4Flag0, uch8x4Flag1, uch8x8Flag0, uch8x8Flag1;
    I32_WMV  i4MBX, iMBY;
    I32_WMV  iNumMBX = pWMVDec->m_uintNumMBX;

	//pLoopFilterFlag  += iMBYStart*iNumMBX;
    
    uch8x4Flag0 = 0, uch8x4Flag1 = 0, uch8x8Flag0 = 0, uch8x8Flag1 = 0;
    
    if (bFirstRow)
    {
        //iMBYStart = 1;
		i4MBX = 0;
      if(iNumMBX>3)//zou 401,pWMVDec->m_bCodecIsWVC1
      {
          do
          {
			    // Extract flag from pLoopFilterFlag
			    uch8x4Flag0  = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
				uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
				uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [0]);
                uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [1]);

                uch8x4Flag1  = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
				uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [2]);
                uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [3]);

                uch8x8Flag0  = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
				uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 1)->chFlag [2]);
                uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 1)->chFlag [3]);

                *puch8x4FilterRow0++ = uch8x4Flag0;
                *puch8x4FilterRow1++ = uch8x4Flag1;
                *puch8x8FilterRow0++ = uch8x8Flag0;

				//
                uch8x4Flag0 = EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [0]);
                uch8x4Flag0 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [1]);
				uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [0]);
                uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [1]);

                uch8x4Flag1 = EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [2]);
                uch8x4Flag1 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [3]);
				uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [2]);
                uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [3]);

                uch8x8Flag0 = EXTRACT0_ROWX8((pLoopFilterFlag + 2)->chFlag [2]);
                uch8x8Flag0 |= EXTRACT1_ROWX8((pLoopFilterFlag + 2)->chFlag [3]);
				uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 3)->chFlag [2]);
                uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [3]);

                *puch8x4FilterRow0++ = uch8x4Flag0;
                *puch8x4FilterRow1++ = uch8x4Flag1;
                *puch8x8FilterRow0++ = uch8x8Flag0;
                pLoopFilterFlag += 4;

            } while ( ++i4MBX < iNumMBX >> 2);
      }

        if (iNumMBX & 3)
        {
            uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
            uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
            uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
            uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);

            pLoopFilterFlag ++;

            if ((iNumMBX & 2) == 2)
            {
                uch8x4Flag0 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag0 |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag0 |= EXTRACT3_ROWX8(pLoopFilterFlag->chFlag [3]);
                pLoopFilterFlag++;
            }
            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;

            if ((iNumMBX & 3) == 3)
            {
                uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
                pLoopFilterFlag++;

                *puch8x4FilterRow0++ = uch8x4Flag0;
                *puch8x4FilterRow1++ = uch8x4Flag1;
                *puch8x8FilterRow0++ = uch8x8Flag0;
            }
        }
        puch8x8FilterRow1 = puch8x8FilterRow0 + ((iNumMBX + 1) >> 1);
        puch8x4FilterRow0 = puch8x4FilterRow1;
        puch8x4FilterRow1 = puch8x4FilterRow1 + ((iNumMBX + 1) >> 1);
		return;
    }

    //for (iMBY = iMBYStart; iMBY < iMBYEnd; iMBY ++)
    {
		i4MBX = 0;
      if(iNumMBX>3)//zou 401,pWMVDec->m_bCodecIsWVC1
      {
        do
        {
            // Extract flag from pLoopFilterFlag
            uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
			uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [0]);
            uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [1]);

            uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
			uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [2]);
            uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [3]);

            uch8x8Flag0  = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [1]);
			uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 1)->chFlag [0]);
            uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 1)->chFlag [1]);

            uch8x8Flag1 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
            uch8x8Flag1 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
			uch8x8Flag1 |= EXTRACT2_ROWX8((pLoopFilterFlag + 1)->chFlag [2]);
            uch8x8Flag1 |= EXTRACT3_ROWX8((pLoopFilterFlag + 1)->chFlag [3]);

            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;
            *puch8x8FilterRow1++ = uch8x8Flag1;

            uch8x4Flag0 =  EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [1]);
			uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [0]);
            uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [1]);

            uch8x8Flag0 = EXTRACT0_ROWX8((pLoopFilterFlag + 2)->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_ROWX8((pLoopFilterFlag + 2)->chFlag [1]);
			uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 3)->chFlag [0]);
            uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [1]);

            uch8x4Flag1 = EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [3]);
			uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [2]);
            uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [3]);

            uch8x8Flag1 = EXTRACT0_ROWX8((pLoopFilterFlag + 2)->chFlag [2]);
            uch8x8Flag1 |= EXTRACT1_ROWX8((pLoopFilterFlag + 2)->chFlag [3]);
			uch8x8Flag1 |= EXTRACT2_ROWX8((pLoopFilterFlag + 3)->chFlag [2]);
            uch8x8Flag1 |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [3]);

            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;
            *puch8x8FilterRow1++ = uch8x8Flag1;
            pLoopFilterFlag += 4;
        } while ( ++i4MBX < iNumMBX >> 2 );
       }

        if (iNumMBX & 3)
        {
            uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
            uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [1]);
            uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
            uch8x8Flag1 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
            uch8x8Flag1 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);

            pLoopFilterFlag++;

            if ((iNumMBX & 2) == 2)
            {
                uch8x4Flag0 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x8Flag0 |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [0]);
                uch8x8Flag0 |= EXTRACT3_ROWX8(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag1 |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag1 |= EXTRACT3_ROWX8(pLoopFilterFlag->chFlag [3]);
                pLoopFilterFlag++;

            }
            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;
            *puch8x8FilterRow1++ = uch8x8Flag1;

            if ((iNumMBX & 3) == 3)
            {

                uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [0]);
                uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag1 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag1 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
                pLoopFilterFlag++;


                *puch8x4FilterRow0++ = uch8x4Flag0;
                *puch8x4FilterRow1++ = uch8x4Flag1;
                *puch8x8FilterRow0++ = uch8x8Flag0;
                *puch8x8FilterRow1++ = uch8x8Flag1;
            }
        }
        puch8x4FilterRow0 = puch8x4FilterRow1;
        puch8x4FilterRow1 = puch8x4FilterRow1 + ((iNumMBX + 1) >> 1);
        puch8x8FilterRow0 = puch8x8FilterRow1;
        puch8x8FilterRow1 = puch8x8FilterRow1 + ((iNumMBX + 1) >> 1);
    }
}

Void_WMV GenerateRowLoopFilterFlags_UV (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* puchFilterRowFlagU, U8_WMV* puchFilterRowFlagV,
    U8_WMV* puchFilter8x4RowFlagU, U8_WMV* puchFilter8x4RowFlagV,
    LOOPF_FLAG* pLoopFilterFlag,Bool_WMV bFirstRow)
{
    U8_WMV uch8x4Flag0, uch8x4Flag1, uch8x8Flag0, uch8x8Flag1;
    U8_WMV uch8x8FlagU, uch8x8FlagV, uch8x4FlagU, uch8x4FlagV;
    I32_WMV  i4MBX, iMBY;
    I32_WMV  iNumMBX = pWMVDec->m_uintNumMBX;
    
    uch8x4Flag0 = 0, uch8x4Flag1 = 0, uch8x8Flag0 = 0, uch8x8Flag1 = 0;
    uch8x8FlagU = 0, uch8x8FlagV = 0, uch8x4FlagU = 0, uch8x4FlagV = 0;
    
    if (bFirstRow)
    {
       // iMBYStart = 1;
		i4MBX = 0;
      if(iNumMBX>3)//zou 401,pWMVDec->m_bCodecIsWVC1
      {
          do
          {
			    // Extract flag from pLoopFilterFlag
                uch8x4FlagU  = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV  = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);       
                uch8x4FlagU |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [4]);
                uch8x4FlagV |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [5]);

				//
                uch8x4FlagU |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [4]);
                uch8x4FlagV |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [5]);
                uch8x4FlagU |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [4]);
                uch8x4FlagV |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [5]);

                *puchFilter8x4RowFlagU++ = uch8x4FlagU;
                *puchFilter8x4RowFlagV++ = uch8x4FlagV;
                pLoopFilterFlag += 4;

            } while ( ++i4MBX < iNumMBX >> 2);
      }

        if (iNumMBX & 3)
        {
            uch8x4FlagU = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
            uch8x4FlagV = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);
            pLoopFilterFlag ++;
            if ((iNumMBX & 2) == 2)
            {
                uch8x4FlagU |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;
            }

            if ((iNumMBX & 3) == 3)
            {
                uch8x4FlagU |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;
            }
            *puchFilter8x4RowFlagU++ = uch8x4FlagU;
            *puchFilter8x4RowFlagV++ = uch8x4FlagV;
        }
		return;
    }

    //for (iMBY = iMBYStart; iMBY < iMBYEnd; iMBY ++)
    {
		i4MBX = 0;
      if(iNumMBX>3)//zou 401,pWMVDec->m_bCodecIsWVC1
      {
        do
        {
            // Extract flag from pLoopFilterFlag
            uch8x4FlagU = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
            uch8x4FlagV = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);
			uch8x4FlagU |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [4]);
            uch8x4FlagV |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [5]);

            uch8x8FlagU = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [4]);
            uch8x8FlagV = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [5]);
            uch8x8FlagU |= EXTRACT1_ROWX8((pLoopFilterFlag + 1)->chFlag [4]);
            uch8x8FlagV |= EXTRACT1_ROWX8((pLoopFilterFlag + 1)->chFlag [5]);

            uch8x4FlagU |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [4]);
            uch8x4FlagV |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [5]);
			uch8x4FlagU |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [4]);
            uch8x4FlagV |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [5]);

            uch8x8FlagU |= EXTRACT2_ROWX8((pLoopFilterFlag + 2)->chFlag [4]);
            uch8x8FlagV |= EXTRACT2_ROWX8((pLoopFilterFlag + 2)->chFlag [5]);
            uch8x8FlagU |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [4]);
            uch8x8FlagV |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [5]);

            *puchFilterRowFlagU++ = uch8x8FlagU;
            *puchFilterRowFlagV++ = uch8x8FlagV;
            *puchFilter8x4RowFlagU++ = uch8x4FlagU;
            *puchFilter8x4RowFlagV++ = uch8x4FlagV;
            pLoopFilterFlag += 4;
        } while ( ++i4MBX < iNumMBX >> 2 );
       }

        if (iNumMBX & 3)
        {
            uch8x4FlagU = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
            uch8x4FlagV = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);
            uch8x8FlagU = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [4]);
            uch8x8FlagV = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [5]);

            pLoopFilterFlag++;

            if ((iNumMBX & 2) == 2)
            {
                uch8x4FlagU |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [5]);
                uch8x8FlagU |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [4]);
                uch8x8FlagV |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;

            }

            if ((iNumMBX & 3) == 3)
            {
                uch8x4FlagU |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [5]);
                uch8x8FlagU |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [4]);
                uch8x8FlagV |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;
            }
            *puchFilterRowFlagU++ = uch8x8FlagU;
            *puchFilterRowFlagV++ = uch8x8FlagV;
            *puchFilter8x4RowFlagU++ = uch8x4FlagU;
            *puchFilter8x4RowFlagV++ = uch8x4FlagV;
        }
    }
}

Void_WMV GenerateRowLoopFilterFlags (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* puchFilterRowFlagY, U8_WMV* puchFilterRowFlagU, U8_WMV* puchFilterRowFlagV,
    U8_WMV* puchFilter8x4RowFlagY, U8_WMV* puchFilter8x4RowFlagU, U8_WMV* puchFilter8x4RowFlagV,
    LOOPF_FLAG* pLoopFilterFlag, I32_WMV iMBYStart, I32_WMV iMBYEnd, Bool_WMV bFirstRow)
{
    U8_WMV* puch8x4FilterRow0 = puchFilter8x4RowFlagY;
    U8_WMV* puch8x4FilterRow1 = puchFilter8x4RowFlagY + ((pWMVDec->m_uintNumMBX + 1) >> 1);
    U8_WMV* puch8x8FilterRow0 = puchFilterRowFlagY;
    U8_WMV* puch8x8FilterRow1 = puchFilterRowFlagY + ((pWMVDec->m_uintNumMBX + 1) >> 1);
    U8_WMV uch8x4Flag0, uch8x4Flag1, uch8x8Flag0, uch8x8Flag1;
    U8_WMV uch8x8FlagU, uch8x8FlagV, uch8x4FlagU, uch8x4FlagV;
    I32_WMV  i4MBX, iMBY;
    I32_WMV  iNumMBX = pWMVDec->m_uintNumMBX;
    
    uch8x4Flag0 = 0, uch8x4Flag1 = 0, uch8x8Flag0 = 0, uch8x8Flag1 = 0;
    uch8x8FlagU = 0, uch8x8FlagV = 0, uch8x4FlagU = 0, uch8x4FlagV = 0;
    
    if (bFirstRow)
    {
        iMBYStart = 1;
		i4MBX = 0;
      if(iNumMBX>3)//zou 401,pWMVDec->m_bCodecIsWVC1
      {
          do
          {
			    // Extract flag from pLoopFilterFlag
			    uch8x4Flag0  = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
				uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
				uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [0]);
                uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [1]);

                uch8x4Flag1  = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
				uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [2]);
                uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [3]);

                uch8x8Flag0  = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
				uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 1)->chFlag [2]);
                uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 1)->chFlag [3]);

                uch8x4FlagU  = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV  = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);       
                uch8x4FlagU |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [4]);
                uch8x4FlagV |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [5]);

                *puch8x4FilterRow0++ = uch8x4Flag0;
                *puch8x4FilterRow1++ = uch8x4Flag1;
                *puch8x8FilterRow0++ = uch8x8Flag0;

				//
                uch8x4Flag0 = EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [0]);
                uch8x4Flag0 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [1]);
				uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [0]);
                uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [1]);

                uch8x4Flag1 = EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [2]);
                uch8x4Flag1 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [3]);
				uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [2]);
                uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [3]);

                uch8x8Flag0 = EXTRACT0_ROWX8((pLoopFilterFlag + 2)->chFlag [2]);
                uch8x8Flag0 |= EXTRACT1_ROWX8((pLoopFilterFlag + 2)->chFlag [3]);
				uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 3)->chFlag [2]);
                uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [3]);

                uch8x4FlagU |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [4]);
                uch8x4FlagV |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [5]);
                uch8x4FlagU |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [4]);
                uch8x4FlagV |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [5]);


                *puch8x4FilterRow0++ = uch8x4Flag0;
                *puch8x4FilterRow1++ = uch8x4Flag1;
                *puch8x8FilterRow0++ = uch8x8Flag0;
                *puchFilter8x4RowFlagU++ = uch8x4FlagU;
                *puchFilter8x4RowFlagV++ = uch8x4FlagV;
                pLoopFilterFlag += 4;

            } while ( ++i4MBX < iNumMBX >> 2);
      }

        if (iNumMBX & 3)
        {
            uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
            uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
            uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
            uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
            uch8x4FlagU = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
            uch8x4FlagV = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);

            pLoopFilterFlag ++;

            if ((iNumMBX & 2) == 2)
            {
                uch8x4Flag0 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag0 |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag0 |= EXTRACT3_ROWX8(pLoopFilterFlag->chFlag [3]);
                uch8x4FlagU |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;
            }
            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;

            if ((iNumMBX & 3) == 3)
            {
                uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
                uch8x4FlagU |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;

                *puch8x4FilterRow0++ = uch8x4Flag0;
                *puch8x4FilterRow1++ = uch8x4Flag1;
                *puch8x8FilterRow0++ = uch8x8Flag0;
            }
            *puchFilter8x4RowFlagU++ = uch8x4FlagU;
            *puchFilter8x4RowFlagV++ = uch8x4FlagV;
        }
        puch8x8FilterRow1 = puch8x8FilterRow0 + ((iNumMBX + 1) >> 1);
        puch8x4FilterRow0 = puch8x4FilterRow1;
        puch8x4FilterRow1 = puch8x4FilterRow1 + ((iNumMBX + 1) >> 1);
    }

    for (iMBY = iMBYStart; iMBY < iMBYEnd; iMBY ++)
    {
		i4MBX = 0;
      if(iNumMBX>3)//zou 401,pWMVDec->m_bCodecIsWVC1
      {
        do
        {
            // Extract flag from pLoopFilterFlag
            uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
			uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [0]);
            uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [1]);

            uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
			uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [2]);
            uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [3]);

            uch8x8Flag0  = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [1]);
			uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 1)->chFlag [0]);
            uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 1)->chFlag [1]);

            uch8x8Flag1 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
            uch8x8Flag1 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
			uch8x8Flag1 |= EXTRACT2_ROWX8((pLoopFilterFlag + 1)->chFlag [2]);
            uch8x8Flag1 |= EXTRACT3_ROWX8((pLoopFilterFlag + 1)->chFlag [3]);

            uch8x4FlagU = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
            uch8x4FlagV = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);
			uch8x4FlagU |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [4]);
            uch8x4FlagV |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [5]);

            uch8x8FlagU = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [4]);
            uch8x8FlagV = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [5]);
            uch8x8FlagU |= EXTRACT1_ROWX8((pLoopFilterFlag + 1)->chFlag [4]);
            uch8x8FlagV |= EXTRACT1_ROWX8((pLoopFilterFlag + 1)->chFlag [5]);

            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;
            *puch8x8FilterRow1++ = uch8x8Flag1;

            uch8x4Flag0 =  EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [1]);
			uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [0]);
            uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [1]);

            uch8x8Flag0 = EXTRACT0_ROWX8((pLoopFilterFlag + 2)->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_ROWX8((pLoopFilterFlag + 2)->chFlag [1]);
			uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 3)->chFlag [0]);
            uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [1]);

            uch8x4Flag1 = EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [3]);
			uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [2]);
            uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [3]);

            uch8x8Flag1 = EXTRACT0_ROWX8((pLoopFilterFlag + 2)->chFlag [2]);
            uch8x8Flag1 |= EXTRACT1_ROWX8((pLoopFilterFlag + 2)->chFlag [3]);
			uch8x8Flag1 |= EXTRACT2_ROWX8((pLoopFilterFlag + 3)->chFlag [2]);
            uch8x8Flag1 |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [3]);

            uch8x4FlagU |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [4]);
            uch8x4FlagV |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [5]);
			uch8x4FlagU |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [4]);
            uch8x4FlagV |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [5]);

            uch8x8FlagU |= EXTRACT2_ROWX8((pLoopFilterFlag + 2)->chFlag [4]);
            uch8x8FlagV |= EXTRACT2_ROWX8((pLoopFilterFlag + 2)->chFlag [5]);
            uch8x8FlagU |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [4]);
            uch8x8FlagV |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [5]);

            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;
            *puch8x8FilterRow1++ = uch8x8Flag1;

            *puchFilterRowFlagU++ = uch8x8FlagU;
            *puchFilterRowFlagV++ = uch8x8FlagV;
            *puchFilter8x4RowFlagU++ = uch8x4FlagU;
            *puchFilter8x4RowFlagV++ = uch8x4FlagV;
            pLoopFilterFlag += 4;
        } while ( ++i4MBX < iNumMBX >> 2 );
       }

        if (iNumMBX & 3)
        {
            uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
            uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [1]);
            uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
            uch8x8Flag1 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
            uch8x8Flag1 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
            uch8x4FlagU = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
            uch8x4FlagV = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);
            uch8x8FlagU = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [4]);
            uch8x8FlagV = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [5]);

            pLoopFilterFlag++;

            if ((iNumMBX & 2) == 2)
            {
                uch8x4Flag0 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x8Flag0 |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [0]);
                uch8x8Flag0 |= EXTRACT3_ROWX8(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag1 |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag1 |= EXTRACT3_ROWX8(pLoopFilterFlag->chFlag [3]);
                uch8x4FlagU |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [5]);
                uch8x8FlagU |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [4]);
                uch8x8FlagV |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;

            }
            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;
            *puch8x8FilterRow1++ = uch8x8Flag1;

            if ((iNumMBX & 3) == 3)
            {

                uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [0]);
                uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag1 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag1 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
                uch8x4FlagU |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [5]);
                uch8x8FlagU |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [4]);
                uch8x8FlagV |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;


                *puch8x4FilterRow0++ = uch8x4Flag0;
                *puch8x4FilterRow1++ = uch8x4Flag1;
                *puch8x8FilterRow0++ = uch8x8Flag0;
                *puch8x8FilterRow1++ = uch8x8Flag1;
            }
            *puchFilterRowFlagU++ = uch8x8FlagU;
            *puchFilterRowFlagV++ = uch8x8FlagV;
            *puchFilter8x4RowFlagU++ = uch8x4FlagU;
            *puchFilter8x4RowFlagV++ = uch8x4FlagV;
        }
        puch8x4FilterRow0 = puch8x4FilterRow1;
        puch8x4FilterRow1 = puch8x4FilterRow1 + ((iNumMBX + 1) >> 1);
        puch8x8FilterRow0 = puch8x8FilterRow1;
        puch8x8FilterRow1 = puch8x8FilterRow1 + ((iNumMBX + 1) >> 1);
    }
}

Void_WMV GenerateColumnLoopFilterFlags_Y (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* puchFilterColFlagY, U8_WMV* puchFilterColFlagY2,
    LOOPF_FLAG* pLoopFilterFlag, I32_WMV iMBHeight)
{
    U8_WMV* puch4x8FilterCol0 = puchFilterColFlagY2;
    U8_WMV* puch4x8FilterCol1 = puchFilterColFlagY2 + ((iMBHeight + 1) >> 1);
    U8_WMV* puch8x8FilterCol0 = puchFilterColFlagY;
    U8_WMV* puch8x8FilterCol1;
    U8_WMV uch4x8Flag0, uch4x8Flag1, uch8x8Flag0, uch8x8Flag1;
    U8_WMV uch8x8FlagU, uch8x8FlagV, uch4x8FlagU, uch4x8FlagV;
    LOOPF_FLAG* pLoopFilterFlag2 = pLoopFilterFlag;
    I32_WMV iMBX;
    I32_WMV i4MBY;
	I32_WMV iNumMBX = pWMVDec->m_uintNumMBX;

    uch4x8Flag0 = 0, uch4x8Flag1 = 0, uch8x8Flag0 = 0, uch8x8Flag1 = 0;
    uch8x8FlagU = 0, uch8x8FlagV = 0, uch4x8FlagU = 0, uch4x8FlagV = 0;

	i4MBY = 0;

 //first mb col
    if (iMBHeight & 3)
    {
        uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
        uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
        uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
        uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
        uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
        uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
        pLoopFilterFlag2 += iNumMBX;
        *puch4x8FilterCol0++ = uch4x8Flag0;
        *puch4x8FilterCol1++ = uch4x8Flag1;
        *puch8x8FilterCol0++ = uch8x8Flag0;
    }

    puch4x8FilterCol0 = puch4x8FilterCol1;
    puch4x8FilterCol1 = puch4x8FilterCol1 + ((iMBHeight + 1) >> 1);
    puch8x8FilterCol1 = puch8x8FilterCol0 + ((iMBHeight + 1) >> 1);
    pLoopFilterFlag ++;

    iMBX = 1;
	if(iNumMBX>1) //zou 401,pWMVDec->m_bCodecIsWVC1
	{
		do
		{
			pLoopFilterFlag2 = pLoopFilterFlag;
			i4MBY = 0; 
			if (iMBHeight & 3)
			{
				uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
				uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
				uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [0]);
				uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [2]);
				uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
				uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
				uch8x8Flag1 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
				uch8x8Flag1 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
				pLoopFilterFlag2 += iNumMBX;
				*puch4x8FilterCol0++ = uch4x8Flag0;
				*puch4x8FilterCol1++ = uch4x8Flag1;
				*puch8x8FilterCol0++ = uch8x8Flag0;
				*puch8x8FilterCol1++ = uch8x8Flag1;
			}
			puch4x8FilterCol0 = puch4x8FilterCol1;
			puch4x8FilterCol1 = puch4x8FilterCol1 + ((iMBHeight + 1) >> 1);
			puch8x8FilterCol0 = puch8x8FilterCol1;
			puch8x8FilterCol1 = puch8x8FilterCol1 + ((iMBHeight + 1) >> 1);
			pLoopFilterFlag ++;
    } while ( ++iMBX < iNumMBX );
   }
}

Void_WMV GenerateColumnLoopFilterFlags_UV (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* puchFilterColFlagU, U8_WMV* puchFilterColFlagV,
    U8_WMV* puchFilterColFlagU2, U8_WMV* puchFilterColFlagV2,
    LOOPF_FLAG* pLoopFilterFlag, I32_WMV iMBHeight)
{
    U8_WMV uch4x8Flag0, uch4x8Flag1, uch8x8Flag0, uch8x8Flag1;
    U8_WMV uch8x8FlagU, uch8x8FlagV, uch4x8FlagU, uch4x8FlagV;
    LOOPF_FLAG* pLoopFilterFlag2 = pLoopFilterFlag;
    I32_WMV iMBX;
    I32_WMV i4MBY;
	I32_WMV iNumMBX = pWMVDec->m_uintNumMBX;

    uch4x8Flag0 = 0, uch4x8Flag1 = 0, uch8x8Flag0 = 0, uch8x8Flag1 = 0;
    uch8x8FlagU = 0, uch8x8FlagV = 0, uch4x8FlagU = 0, uch4x8FlagV = 0;

	i4MBY = 0;

 //first mb col
  if(iMBHeight>3) //zou 330,pWMVDec->m_bCodecIsWVC1
  {
    do
    {
        // Extract flag from pLoopFilterFlag
        uch4x8FlagU = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [5]);

        pLoopFilterFlag2 += iNumMBX;

        uch4x8FlagU |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [5]);

        pLoopFilterFlag2 += iNumMBX;

        uch4x8FlagU |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [5]);

        pLoopFilterFlag2 += iNumMBX;

        uch4x8FlagU |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [5]);

        pLoopFilterFlag2 += iNumMBX;

        *puchFilterColFlagU2++ = uch4x8FlagU;
        *puchFilterColFlagV2++ = uch4x8FlagV;

    } while ( ++i4MBY < iMBHeight >> 2 );
   }

    if (iMBHeight & 3)
    {
        uch4x8FlagU = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [5]);
        pLoopFilterFlag2 += iNumMBX;

        if ((iMBHeight & 2) == 2)
        {
            uch4x8FlagU |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;
        }

        if ((iMBHeight & 3) == 3)
        {
            uch4x8FlagU |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [5]);
        }
        *puchFilterColFlagU2++ = uch4x8FlagU;
        *puchFilterColFlagV2++ = uch4x8FlagV;
    }
    pLoopFilterFlag ++;

    iMBX = 1;

  if(iNumMBX>1) //zou 401,pWMVDec->m_bCodecIsWVC1
  {
    do
    {
        pLoopFilterFlag2 = pLoopFilterFlag;
		i4MBY = 0;
      if(iMBHeight>3)   //zou 330,pWMVDec->m_bCodecIsWVC1
      {
        do
        {
            // Extract flag from pLoopFilterFlag
            uch4x8FlagU = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;
            uch4x8FlagU |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;
            uch4x8FlagU |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;
            uch4x8FlagU |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;
            *puchFilterColFlagU++ = uch8x8FlagU;
            *puchFilterColFlagV++ = uch8x8FlagV;
            *puchFilterColFlagU2++ = uch4x8FlagU;
            *puchFilterColFlagV2++ = uch4x8FlagV;
        } while ( ++i4MBY < iMBHeight >> 2 );
       }

        if (iMBHeight & 3)
        {
            uch4x8FlagU = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;

            if ((iMBHeight & 2) == 2)
            {
                uch4x8FlagU |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [4]);
                uch4x8FlagV |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [5]);
                uch8x8FlagU |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [4]);
                uch8x8FlagV |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [5]);
                pLoopFilterFlag2 += iNumMBX;
            }

            if ((iMBHeight & 3) == 3)
            {
                uch4x8FlagU |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [4]);
                uch4x8FlagV |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [5]);
                uch8x8FlagU |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [4]);
                uch8x8FlagV |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [5]);
            }
            *puchFilterColFlagU++ = uch8x8FlagU;
            *puchFilterColFlagV++ = uch8x8FlagV;
            *puchFilterColFlagU2++ = uch4x8FlagU;
            *puchFilterColFlagV2++ = uch4x8FlagV;
        }
        pLoopFilterFlag ++;
    } while ( ++iMBX < iNumMBX );
   }
}


Void_WMV GenerateColumnLoopFilterFlags (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* puchFilterColFlagY, U8_WMV* puchFilterColFlagU, U8_WMV* puchFilterColFlagV,
    U8_WMV* puchFilterColFlagY2, U8_WMV* puchFilterColFlagU2, U8_WMV* puchFilterColFlagV2,
    LOOPF_FLAG* pLoopFilterFlag, I32_WMV iMBHeight)
{
    U8_WMV* puch4x8FilterCol0 = puchFilterColFlagY2;
    U8_WMV* puch4x8FilterCol1 = puchFilterColFlagY2 + ((iMBHeight + 1) >> 1);
    U8_WMV* puch8x8FilterCol0 = puchFilterColFlagY;
    U8_WMV* puch8x8FilterCol1;
    U8_WMV uch4x8Flag0, uch4x8Flag1, uch8x8Flag0, uch8x8Flag1;
    U8_WMV uch8x8FlagU, uch8x8FlagV, uch4x8FlagU, uch4x8FlagV;
    LOOPF_FLAG* pLoopFilterFlag2 = pLoopFilterFlag;
    I32_WMV iMBX;
    I32_WMV i4MBY;
	I32_WMV iNumMBX = pWMVDec->m_uintNumMBX;

    uch4x8Flag0 = 0, uch4x8Flag1 = 0, uch8x8Flag0 = 0, uch8x8Flag1 = 0;
    uch8x8FlagU = 0, uch8x8FlagV = 0, uch4x8FlagU = 0, uch4x8FlagV = 0;

	i4MBY = 0;

 //first mb col
  if(iMBHeight>3) //zou 330,pWMVDec->m_bCodecIsWVC1
  {
    do
    {
        // Extract flag from pLoopFilterFlag
        
        uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
        uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
        uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
        uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
        uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
        uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
        uch4x8FlagU = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [5]);

        pLoopFilterFlag2 += iNumMBX;

        uch4x8Flag0 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [0]);
        uch4x8Flag0 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [2]);
        uch4x8Flag1 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [1]);
        uch4x8Flag1 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [3]);
        uch8x8Flag0 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [1]);
        uch8x8Flag0 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [3]);
        uch4x8FlagU |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [5]);

        *puch4x8FilterCol0++ = uch4x8Flag0;
        *puch4x8FilterCol1++ = uch4x8Flag1;
        *puch8x8FilterCol0++ = uch8x8Flag0;
        pLoopFilterFlag2 += iNumMBX;

        uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
        uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
        uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
        uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
        uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
        uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
        uch4x8FlagU |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [5]);

        pLoopFilterFlag2 += iNumMBX;

        uch4x8Flag0 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [0]);
        uch4x8Flag0 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [2]);
        uch4x8Flag1 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [1]);
        uch4x8Flag1 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [3]);
        uch8x8Flag0 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [1]);
        uch8x8Flag0 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [3]);
        uch4x8FlagU |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [5]);

        pLoopFilterFlag2 += iNumMBX;

        *puch4x8FilterCol0++ = uch4x8Flag0;
        *puch4x8FilterCol1++ = uch4x8Flag1;
        *puch8x8FilterCol0++ = uch8x8Flag0;
        *puchFilterColFlagU2++ = uch4x8FlagU;
        *puchFilterColFlagV2++ = uch4x8FlagV;

    } while ( ++i4MBY < iMBHeight >> 2 );
   }

    if (iMBHeight & 3)
    {
        uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
        uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
        uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
        uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
        uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
        uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
        uch4x8FlagU = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [5]);
        pLoopFilterFlag2 += iNumMBX;

        if ((iMBHeight & 2) == 2)
        {
            uch4x8Flag0 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag0 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag0 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;
        }
        *puch4x8FilterCol0++ = uch4x8Flag0;
        *puch4x8FilterCol1++ = uch4x8Flag1;
        *puch8x8FilterCol0++ = uch8x8Flag0;

        if ((iMBHeight & 3) == 3)
        {
            uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [5]);

            *puch4x8FilterCol0++ = uch4x8Flag0;
            *puch4x8FilterCol1++ = uch4x8Flag1;
            *puch8x8FilterCol0++ = uch8x8Flag0;
        }
        *puchFilterColFlagU2++ = uch4x8FlagU;
        *puchFilterColFlagV2++ = uch4x8FlagV;
    }

    puch4x8FilterCol0 = puch4x8FilterCol1;
    puch4x8FilterCol1 = puch4x8FilterCol1 + ((iMBHeight + 1) >> 1);
    puch8x8FilterCol1 = puch8x8FilterCol0 + ((iMBHeight + 1) >> 1);
    pLoopFilterFlag ++;

    iMBX = 1;

  if(iNumMBX>1) //zou 401,pWMVDec->m_bCodecIsWVC1
  {
    do
    {
        pLoopFilterFlag2 = pLoopFilterFlag;
		i4MBY = 0;
      if(iMBHeight>3)   //zou 330,pWMVDec->m_bCodecIsWVC1
      {
        do
        {
            // Extract flag from pLoopFilterFlag
            uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag1 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag1 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;
            uch4x8Flag0 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch8x8Flag0 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [0]);
            uch8x8Flag0 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag1 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag1 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [5]);

            *puch4x8FilterCol0++ = uch4x8Flag0;
            *puch4x8FilterCol1++ = uch4x8Flag1;
            *puch8x8FilterCol0++ = uch8x8Flag0;
            *puch8x8FilterCol1++ = uch8x8Flag1;
            pLoopFilterFlag2 += iNumMBX;
            uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag1 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag1 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;
            uch4x8Flag0 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch8x8Flag0 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [0]);
            uch8x8Flag0 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag1 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag1 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;

            *puch4x8FilterCol0++ = uch4x8Flag0;
            *puch4x8FilterCol1++ = uch4x8Flag1;
            *puch8x8FilterCol0++ = uch8x8Flag0;
            *puch8x8FilterCol1++ = uch8x8Flag1;
            *puchFilterColFlagU++ = uch8x8FlagU;
            *puchFilterColFlagV++ = uch8x8FlagV;
            *puchFilterColFlagU2++ = uch4x8FlagU;
            *puchFilterColFlagV2++ = uch4x8FlagV;
        } while ( ++i4MBY < iMBHeight >> 2 );
       }

        if (iMBHeight & 3)
        {
            uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag1 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag1 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;

            if ((iMBHeight & 2) == 2)
            {
                uch4x8Flag0 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [0]);
                uch4x8Flag0 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [2]);
                uch8x8Flag0 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [0]);
                uch8x8Flag0 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [2]);
                uch4x8Flag1 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [1]);
                uch4x8Flag1 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [3]);
                uch8x8Flag1 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [1]);
                uch8x8Flag1 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [3]);
                uch4x8FlagU |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [4]);
                uch4x8FlagV |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [5]);
                uch8x8FlagU |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [4]);
                uch8x8FlagV |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [5]);
                pLoopFilterFlag2 += iNumMBX;
            }
            *puch4x8FilterCol0++ = uch4x8Flag0;
            *puch4x8FilterCol1++ = uch4x8Flag1;
            *puch8x8FilterCol0++ = uch8x8Flag0;
            *puch8x8FilterCol1++ = uch8x8Flag1;

            if ((iMBHeight & 3) == 3)
            {
                uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
                uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
                uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [0]);
                uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [2]);
                uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
                uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
                uch8x8Flag1 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
                uch8x8Flag1 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
                uch4x8FlagU |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [4]);
                uch4x8FlagV |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [5]);
                uch8x8FlagU |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [4]);
                uch8x8FlagV |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [5]);

                *puch4x8FilterCol0++ = uch4x8Flag0;
                *puch4x8FilterCol1++ = uch4x8Flag1;
                *puch8x8FilterCol0++ = uch8x8Flag0;
                *puch8x8FilterCol1++ = uch8x8Flag1;
            }
            *puchFilterColFlagU++ = uch8x8FlagU;
            *puchFilterColFlagV++ = uch8x8FlagV;
            *puchFilterColFlagU2++ = uch4x8FlagU;
            *puchFilterColFlagV2++ = uch4x8FlagV;
        }
        puch4x8FilterCol0 = puch4x8FilterCol1;
        puch4x8FilterCol1 = puch4x8FilterCol1 + ((iMBHeight + 1) >> 1);
        puch8x8FilterCol0 = puch8x8FilterCol1;
        puch8x8FilterCol1 = puch8x8FilterCol1 + ((iMBHeight + 1) >> 1);
        pLoopFilterFlag ++;
    } while ( ++iMBX < iNumMBX );
   }
}


#ifdef WMV_OPT_INTENSITYCOMP_ARM
extern const U8_WMV g_iDownAdjustRecon [256];
extern const U8_WMV g_iUpAdjustRecon [256];
extern Void_WMV IntensityComp_ARMV4 (U32_WMV* pScr, I32_WMV iFrameSize, const U8_WMV* pLUT);
#endif
// used to adjust recon

Void_WMV AdjustReconRange_mbrow(tWMVDecInternalMember *pWMVDec,U32_WMV mb_row)
{
	EMB_PMainLoop  * pPMainLoop = &pWMVDec->m_EMB_PMainLoop;
    EMB_PBMainLoop *pPB = &pPMainLoop->PB[0];

	U8_WMV *pRefY = (pPB->m_ppxliRefBlkBase[0] - pWMVDec->m_iWidthPrevYXExpPlusExp) + pWMVDec->m_iWidthPrevY*mb_row*16; 
    U8_WMV *pRefU = (pPB->m_ppxliRefBlkBase[4] - pWMVDec->m_iWidthPrevUVXExpPlusExp) + pWMVDec->m_iWidthPrevUV*mb_row*8; 
    U8_WMV *pRefV = (pPB->m_ppxliRefBlkBase[5] - pWMVDec->m_iWidthPrevUVXExpPlusExp) + pWMVDec->m_iWidthPrevUV*mb_row*8;
	U8_WMV *pRefDstY = pWMVDec->m_pfrmRef0Process->m_pucYPlane + pWMVDec->m_iWidthPrevY*mb_row*16;
    U8_WMV *pRefDstU = pWMVDec->m_pfrmRef0Process->m_pucUPlane + pWMVDec->m_iWidthPrevUV*mb_row*8;
    U8_WMV *pRefDstV = pWMVDec->m_pfrmRef0Process->m_pucVPlane + pWMVDec->m_iWidthPrevUV*mb_row*8;
	U32_WMV i=0;
	if(pWMVDec->m_scaling == 1)
    {
		{
			for(i=0;i<pWMVDec->m_iWidthPrevY*16;i++)
				pRefDstY [i] = ((pRefY [i] - 128) >> 1) + 128;

			for(i=0;i<pWMVDec->m_iWidthPrevUV*8;i++)
			{
				pRefDstU [i] = ((pRefU [i] - 128) >> 1) + 128;
                pRefDstV [i] = ((pRefV [i] - 128) >> 1) + 128;
			}
		}
	}
	else if(pWMVDec->m_scaling == 2)
    {
		{
			for(i=0;i<pWMVDec->m_iWidthPrevY*16;i++)
			{
				I32_WMV iTmp = (((I32_WMV)pRefY [i] - 128) << 1) + 128;
                pRefDstY [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);
			}

			for(i=0;i<pWMVDec->m_iWidthPrevUV*8;i++)
			{
				I32_WMV iTmp = (((I32_WMV)pRefU [i] - 128) << 1) + 128;
                pRefDstU [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);
                iTmp = (((I32_WMV)pRefV [i] - 128) << 1) + 128;
                pRefDstV [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);
			}
		}
	}

}

Void_WMV AdjustDecRange_UP(tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iFrmSrcAreaY = pWMVDec->m_iWidthPrevY * pWMVDec->m_iHeightPrevY/2;
    I32_WMV iFrmSrcAreaUV = pWMVDec->m_iWidthPrevUV * pWMVDec->m_iHeightPrevUV/2;
#ifdef WMV_OPT_INTENSITYCOMP_ARM
    IntensityComp_ARMV4((U32_WMV*)pWMVDec->m_ppxliPostQY, iFrmSrcAreaY >> 4, g_iUpAdjustRecon);
    IntensityComp_ARMV4((U32_WMV*)pWMVDec->m_ppxliPostQU, iFrmSrcAreaUV >> 4, g_iUpAdjustRecon);
    IntensityComp_ARMV4((U32_WMV*)pWMVDec->m_ppxliPostQV, iFrmSrcAreaUV >> 4, g_iUpAdjustRecon);
#else
    I32_WMV i, iTmp;
    for (i = 0; i < iFrmSrcAreaY; i++)
    {
        iTmp = (((I32_WMV)pWMVDec->m_ppxliPostQY [i] - 128) << 1) + 128;
        pWMVDec->m_ppxliPostQY [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);//g_rgiClapTabDec [iTmp];
    }
    for (i = 0; i < iFrmSrcAreaUV; i++){
        iTmp = (((I32_WMV)pWMVDec->m_ppxliPostQU [i] - 128) << 1) + 128;
        pWMVDec->m_ppxliPostQU [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);//g_rgiClapTabDec [iTmp];
        iTmp = (((I32_WMV)pWMVDec->m_ppxliPostQV [i] - 128) << 1) + 128;
        pWMVDec->m_ppxliPostQV [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);//g_rgiClapTabDec [iTmp];
    }
#endif
}


Void_WMV AdjustDecRange_DOWN(tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iFrmSrcAreaY = pWMVDec->m_iWidthPrevY * pWMVDec->m_iHeightPrevY;
    I32_WMV iFrmSrcAreaUV = pWMVDec->m_iWidthPrevUV * pWMVDec->m_iHeightPrevUV;
    I32_WMV starty = pWMVDec->m_iWidthPrevY * pWMVDec->m_iHeightPrevY/2;
    I32_WMV startuv = pWMVDec->m_iWidthPrevUV * pWMVDec->m_iHeightPrevUV/2;

#ifdef WMV_OPT_INTENSITYCOMP_ARM //TODO NEON ZOU
    IntensityComp_ARMV4((U32_WMV*)(pWMVDec->m_ppxliPostQY+starty) , (iFrmSrcAreaY/2) >> 4, g_iUpAdjustRecon);
    IntensityComp_ARMV4((U32_WMV*)(pWMVDec->m_ppxliPostQU+startuv), (iFrmSrcAreaUV/2) >> 4, g_iUpAdjustRecon);
    IntensityComp_ARMV4((U32_WMV*)(pWMVDec->m_ppxliPostQV+startuv), (iFrmSrcAreaUV/2) >> 4, g_iUpAdjustRecon);
#else
    I32_WMV i, iTmp;
    for (i = starty; i < iFrmSrcAreaY; i++)
    {
        iTmp = (((I32_WMV)pWMVDec->m_ppxliPostQY [i] - 128) << 1) + 128;
        pWMVDec->m_ppxliPostQY [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);//g_rgiClapTabDec [iTmp];
    }

    for (i = startuv; i < iFrmSrcAreaUV; i++){
        iTmp = (((I32_WMV)pWMVDec->m_ppxliPostQU [i] - 128) << 1) + 128;
        pWMVDec->m_ppxliPostQU [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);//g_rgiClapTabDec [iTmp];
        iTmp = (((I32_WMV)pWMVDec->m_ppxliPostQV [i] - 128) << 1) + 128;
        pWMVDec->m_ppxliPostQV [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);//g_rgiClapTabDec [iTmp];
    }
#endif
}

// used to adjust output
Void_WMV AdjustDecRange(tWMVDecInternalMember *pWMVDec)
{
    //zou mark
#ifdef MARK_POST //POST
    return;
#endif
   
#ifndef SHAREMEMORY
    if (pWMVDec->m_iRangeState == 1)
#endif
    {
		AdjustDecRange_UP(pWMVDec);
		AdjustDecRange_DOWN(pWMVDec);     
    }
}

Void_WMV g_memcpy_C(U8_WMV* ppxliLeftTop, U8_WMV* ppxliSrc, I32_WMV iWidthPrevY, I32_WMV iTrueWidth, I32_WMV iVertPad)
{
    I32_WMV a,b,c,d;
	I32_WMV iRowNum = 0;
	U8_WMV* ppxliDst;
   
    do
    {
		iRowNum = iVertPad;
    	ppxliDst = (U8_WMV*) ppxliLeftTop;	        

		a = *(U32_WMV *) &ppxliSrc[0];
        b = *(U32_WMV *) &ppxliSrc[4];
        c = *(U32_WMV *) &ppxliSrc[8];
        d = *(U32_WMV *) &ppxliSrc[12];	 

        do
        {
			*(U32_WMV *) &ppxliDst[0] = a;	  
			*(U32_WMV *) &ppxliDst[4] = b;
			*(U32_WMV *) &ppxliDst[8] = c;
			*(U32_WMV *) &ppxliDst[12] = d;		    		      	          	
			ppxliDst += iWidthPrevY;
        } while ( --iRowNum != 0 );

		ppxliSrc     += 16;
        ppxliLeftTop += 16;
		iTrueWidth   -= 16;
	} while ( iTrueWidth >= 16 );	

	if (iTrueWidth != 0) {	//last 8 columns.
		//assert( iTrueWidth == 8 ); //iTrueWidth must be 8 here.
		if(iTrueWidth != 8)
			return;
		iRowNum = iVertPad;
		ppxliDst = (U8_WMV*) ppxliLeftTop;	        
		a = *(U32_WMV *) &ppxliSrc[0];
		b = *(U32_WMV *) &ppxliSrc[4];
		do 
		{
			*(U32_WMV *) &ppxliDst[0] = a;	  
			*(U32_WMV *) &ppxliDst[4] = b;
			ppxliDst += iWidthPrevY;
		} while ( --iRowNum != 0 );	
	}
}

Void_WMV g_RepeatRef0Y_LeftRight_C ( 
	const U8_WMV* pLeft, 
	const U8_WMV* pRight, 
	U8_WMV* pDst, 
	I32_WMV iWidthPrevY,	
	I32_WMV iWidthYPlusExp,  
	I32_WMV iRowNum
)
{
	U32_WMV uipadValueLeft;
	U32_WMV uipadValueRight;
	U8_WMV * pDstPlusWidth = pDst + iWidthYPlusExp;

	do
	{ // x-direction interpolation

		uipadValueLeft  = *pLeft; 
		uipadValueRight = *pRight;
		uipadValueLeft  |= (uipadValueLeft << 8);
		uipadValueRight |= (uipadValueRight << 8);
		uipadValueLeft  |= (uipadValueLeft << 16);
		uipadValueRight |= (uipadValueRight << 16);

		// left Y
		((U32_WMV *)pDst)[0]  = uipadValueLeft;
		((U32_WMV *)pDst)[1]  = uipadValueLeft;
		((U32_WMV *)pDst)[2]  = uipadValueLeft;
		((U32_WMV *)pDst)[3] = uipadValueLeft;		    
		((U32_WMV *)pDst)[4] = uipadValueLeft;
		((U32_WMV *)pDst)[5] = uipadValueLeft;
		((U32_WMV *)pDst)[6] = uipadValueLeft;
		((U32_WMV *)pDst)[7] = uipadValueLeft;		
		// right Y
		((U32_WMV *)pDstPlusWidth)[0] = uipadValueRight;
		((U32_WMV *)pDstPlusWidth)[1] = uipadValueRight;
		((U32_WMV *)pDstPlusWidth)[2] = uipadValueRight;
		((U32_WMV *)pDstPlusWidth)[3] = uipadValueRight;		    
		((U32_WMV *)pDstPlusWidth)[4] = uipadValueRight;
		((U32_WMV *)pDstPlusWidth)[5] = uipadValueRight;
		((U32_WMV *)pDstPlusWidth)[6] = uipadValueRight;
		((U32_WMV *)pDstPlusWidth)[7] = uipadValueRight;		    
			
		pDst		  += iWidthPrevY;
		pDstPlusWidth += iWidthPrevY;
		pLeft		  += iWidthPrevY;
		pRight		  += iWidthPrevY;
	//} while ( --iRowNum != 0 );
	} while ( --iRowNum >= 0 );
}

Void_WMV g_RepeatRef0UV_LeftRight_C ( 
	const U8_WMV* pLeftU, 
	const U8_WMV* pRightU, 
	const U8_WMV* pLeftV, 
	const U8_WMV* pRightV, 
	U8_WMV* pDstU, 
	U8_WMV* pDstV, 
	I32_WMV iWidthPrevUV,	
	I32_WMV iWidthUVPlusExp,  
	I32_WMV iRowNum
)
{
	U32_WMV uipadValueLeft;
	U32_WMV uipadValueRight;
	U8_WMV * pDstUPlusWidth = pDstU + iWidthUVPlusExp;
	U8_WMV * pDstVPlusWidth = pDstV + iWidthUVPlusExp;

	do 
    { // x-direction interpolation
        uipadValueLeft = *pLeftU;
        uipadValueLeft |= (uipadValueLeft << 8);
        uipadValueLeft |= (uipadValueLeft << 16);

        uipadValueRight = *pRightU;
        uipadValueRight |= (uipadValueRight << 8);
        uipadValueRight |= (uipadValueRight << 16);
		
		// left U
        ((U32_WMV *)pDstU)[0] = uipadValueLeft;
        ((U32_WMV *)pDstU)[1] = uipadValueLeft;
        ((U32_WMV *)pDstU)[2] = uipadValueLeft;
        ((U32_WMV *)pDstU)[3] = uipadValueLeft;            
		// right U
        ((U32_WMV *)pDstUPlusWidth)[0] = uipadValueRight;
        ((U32_WMV *)pDstUPlusWidth)[1] = uipadValueRight;
        ((U32_WMV *)pDstUPlusWidth)[2] = uipadValueRight;
        ((U32_WMV *)pDstUPlusWidth)[3] = uipadValueRight;  

        uipadValueLeft = *pLeftV;
        uipadValueLeft |= (uipadValueLeft << 8);
        uipadValueLeft |= (uipadValueLeft << 16);
		
        uipadValueRight = *pRightV;
        uipadValueRight |= (uipadValueRight << 8);
        uipadValueRight |= (uipadValueRight << 16);

		// left V
        ((U32_WMV *)pDstV)[0] = uipadValueLeft;
        ((U32_WMV *)pDstV)[1] = uipadValueLeft;
        ((U32_WMV *)pDstV)[2] = uipadValueLeft;
        ((U32_WMV *)pDstV)[3] = uipadValueLeft;            
		// right V
        ((U32_WMV *)pDstVPlusWidth)[0] = uipadValueRight;
        ((U32_WMV *)pDstVPlusWidth)[1] = uipadValueRight;
        ((U32_WMV *)pDstVPlusWidth)[2] = uipadValueRight;
        ((U32_WMV *)pDstVPlusWidth)[3] = uipadValueRight;            

		pDstU += iWidthPrevUV;       
        pDstV += iWidthPrevUV;      
		pDstUPlusWidth += iWidthPrevUV;       
        pDstVPlusWidth += iWidthPrevUV;      

        pLeftU += iWidthPrevUV;
        pLeftV += iWidthPrevUV;
        pRightU += iWidthPrevUV;
        pRightV += iWidthPrevUV;
    } while ( --iRowNum != 0 );
}


Void_WMV g_RepeatRef0Y (
    U8_WMV* ppxlcRef0Y,
    CoordI  iStart, 
    CoordI  iEnd,
    I32_WMV     iOldLeftOffet,
    Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthY,
    I32_WMV     iWidthYPlusExp,
    I32_WMV     iWidthPrevY,
    Bool_WMV    bProgressive
)
{
	const U8_WMV* ppxliOldLeft = ppxlcRef0Y + iOldLeftOffet;
	const U8_WMV* ppxliOldRight = ppxliOldLeft + iWidthY - 1;
	const U8_WMV* ppxliOldTopLn = ppxliOldLeft - EXPANDY_REFVOP;
	U8_WMV* ppxliNew = (U8_WMV*) ppxliOldTopLn;
	
	I32_WMV iVertPad = EXPANDY_REFVOP;
	I32_WMV iTrueWidth = iWidthPrevY;
	//I32_WMV iLastVert = 0;  // Hack ... remove once fix is ready

	
	CoordI y = iEnd - iStart;


	g_RepeatRef0Y_LeftRight_Fun(ppxliOldLeft, ppxliOldRight, ppxliNew, iWidthPrevY, iWidthYPlusExp, y);
	ppxliNew += y * iWidthPrevY;

    {
		U8_WMV* ppxliSrc;

		if (fTop) 
		{
			U8_WMV* ppxliLeftTop = ppxlcRef0Y;
			ppxliSrc = (U8_WMV*) ppxliOldTopLn;		
			g_memcpy_Fun(ppxliLeftTop, ppxliSrc, iWidthPrevY, iTrueWidth, iVertPad);		        
		}

		if (fBottom) 
		{
			const U8_WMV* ppxliOldBotLn = ppxliNew - iWidthPrevY;            
	        ppxliSrc = (U8_WMV*) ppxliOldBotLn;
			g_memcpy_Fun(ppxliNew, ppxliSrc, iWidthPrevY, iTrueWidth, iVertPad);	    	    	
	     }
	}
}


Void_WMV g_RepeatRef0UV (
    U8_WMV* ppxlcRef0U,
    U8_WMV* ppxlcRef0V,
    CoordI  iStart, CoordI iEnd,
    I32_WMV     iOldLeftOffet,
    Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthUV,
    I32_WMV     iWidthUVPlusExp,
    I32_WMV     iWidthPrevUV,
    Bool_WMV    bProgressive

)
{
    const U8_WMV* ppxliOldLeftU = ppxlcRef0U + iOldLeftOffet;
    const U8_WMV* ppxliOldLeftV = ppxlcRef0V + iOldLeftOffet;
    const U8_WMV* ppxliOldRightU = ppxliOldLeftU + iWidthUV - 1;
    const U8_WMV* ppxliOldRightV = ppxliOldLeftV + iWidthUV - 1;
    const U8_WMV* ppxliOldTopLnU = ppxliOldLeftU - EXPANDUV_REFVOP;
    const U8_WMV* ppxliOldTopLnV = ppxliOldLeftV - EXPANDUV_REFVOP;
    U8_WMV* ppxliNewU = (U8_WMV*) ppxliOldTopLnU;
    U8_WMV* ppxliNewV = (U8_WMV*) ppxliOldTopLnV;
    
    CoordI y = iEnd - iStart;
    
    I32_WMV iLastVert = 0; // Hack


    I32_WMV iVertPad = EXPANDUV_REFVOP;
    I32_WMV iTrueWidth = iWidthPrevUV;
   

	g_RepeatRef0UV_LeftRight_Fun ( ppxliOldLeftU, ppxliOldRightU, ppxliOldLeftV, ppxliOldRightV, 
							ppxliNewU, ppxliNewV, iWidthPrevUV,	iWidthUVPlusExp, y);
	ppxliNewU += y * iWidthPrevUV;
	ppxliNewV += y * iWidthPrevUV;

   {      
        //I32_WMV iWidthPrevUVDiv4 = iTrueWidth >> 2;

        U8_WMV* ppxliSrcU;
        U8_WMV* ppxliSrcV;
        if (fTop)
        {
            U8_WMV* ppxliLeftTopU = ppxlcRef0U;
            U8_WMV* ppxliLeftTopV = ppxlcRef0V;

            ppxliSrcU = (U8_WMV*) ppxliOldTopLnU;
            ppxliSrcV = (U8_WMV*) ppxliOldTopLnV;
			g_memcpy_Fun(ppxliLeftTopU,ppxliSrcU, iWidthPrevUV, iTrueWidth, iVertPad);
			g_memcpy_Fun(ppxliLeftTopV,ppxliSrcV, iWidthPrevUV, iTrueWidth, iVertPad);			    	       
        }
        if (fBottom) 
        {
            const U8_WMV* ppxliOldBotLnU = ppxliNewU - iWidthPrevUV;
            const U8_WMV* ppxliOldBotLnV = ppxliNewV - iWidthPrevUV;

            iVertPad += iLastVert;  // Hack
            ppxliSrcU = (U8_WMV*) ppxliOldBotLnU;
            ppxliSrcV = (U8_WMV*) ppxliOldBotLnV;

			g_memcpy_Fun(ppxliNewU,ppxliSrcU, iWidthPrevUV, iTrueWidth, iVertPad);
			g_memcpy_Fun(ppxliNewV,ppxliSrcV, iWidthPrevUV, iTrueWidth, iVertPad);	
        }
   }
}
void AdjustDecRangeWMVA_UP(tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iFrmSrcAreaY = pWMVDec->m_iWidthPrevY * (pWMVDec->m_iHeightPrevY/2);
    I32_WMV iFrmSrcAreaUV = pWMVDec->m_iWidthPrevUV * (pWMVDec->m_iHeightPrevUV/2);
    I32_WMV iTmp, i, iMul;
    if (pWMVDec->m_bRangeRedY_Flag)
    {
        //assert (pWMVDec->m_iRangeRedY > 0 && pWMVDec->m_iRangeRedY < 9);
		if(pWMVDec->m_iRangeRedY <= 0 && pWMVDec->m_iRangeRedY >= 9)
			return;
        iMul = pWMVDec->m_iRangeRedY + 8;
//#ifdef VOARMV7
//		voAdjustDecRangeWMVA_ARMV7(pWMVDec->m_ppxliPostQY,
//													pWMVDec->m_iWidthPrevY,
//													iMul,
//													pWMVDec->m_round);
//#else
        for (i = 0; i < iFrmSrcAreaY; i++) 
        {
            iTmp = (((((I32_WMV)pWMVDec->m_ppxliPostQY [i] - 128) * iMul) + pWMVDec->m_round)>> 3) + 128;
            pWMVDec->m_ppxliPostQY [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);//g_rgiClapTabDec [iTmp];
        }
//#endif
    }

    if (pWMVDec->m_bRangeRedUV_Flag) 
    {
        //assert (pWMVDec->m_iRangeRedUV > 0 && pWMVDec->m_iRangeRedUV < 9);
		if(pWMVDec->m_iRangeRedUV <= 0 && pWMVDec->m_iRangeRedUV >= 9)
			return;
        iMul = pWMVDec->m_iRangeRedUV + 8;
        for (i = 0; i < iFrmSrcAreaUV; i++) 
        {
            iTmp = (((((I32_WMV)pWMVDec->m_ppxliPostQU [i] - 128) * iMul) +pWMVDec->m_round)>> 3) + 128;
            pWMVDec->m_ppxliPostQU [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);//g_rgiClapTabDec [iTmp];
            iTmp = (((((I32_WMV)pWMVDec->m_ppxliPostQV [i] - 128) * iMul ) + pWMVDec->m_round)>> 3) + 128;
            pWMVDec->m_ppxliPostQV [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);//g_rgiClapTabDec [iTmp];
        }
    }
}

void AdjustDecRangeWMVA_DOWN(tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iFrmSrcAreaY = pWMVDec->m_iWidthPrevY * (pWMVDec->m_iHeightPrevY);
    I32_WMV iFrmSrcAreaUV = pWMVDec->m_iWidthPrevUV * (pWMVDec->m_iHeightPrevUV);
    I32_WMV iTmp, i, iMul;
    U32_WMV start =0;
    if (pWMVDec->m_bRangeRedY_Flag)
    {
        //assert (pWMVDec->m_iRangeRedY > 0 && pWMVDec->m_iRangeRedY < 9);
		if((pWMVDec->m_iRangeRedY <= 0 && pWMVDec->m_iRangeRedY >= 9))
			return;
        start = (pWMVDec->m_iWidthPrevY *(pWMVDec->m_iHeightPrevY-pWMVDec->m_iHeightPrevY/2));
        iMul = pWMVDec->m_iRangeRedY + 8;
        for (i = start; i < iFrmSrcAreaY; i++) 
        {
            iTmp = (((((I32_WMV)pWMVDec->m_ppxliPostQY [i] - 128) * iMul) + pWMVDec->m_round)>> 3) + 128;
            pWMVDec->m_ppxliPostQY [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);//g_rgiClapTabDec [iTmp];
        }
    }

    if (pWMVDec->m_bRangeRedUV_Flag) 
    {
        //assert (pWMVDec->m_iRangeRedUV > 0 && pWMVDec->m_iRangeRedUV < 9);
		if(pWMVDec->m_iRangeRedUV <= 0 || pWMVDec->m_iRangeRedUV >= 9)
			return;
        start = (pWMVDec->m_iWidthPrevUV *(pWMVDec->m_iHeightPrevUV-pWMVDec->m_iHeightPrevUV/2));
        iMul = pWMVDec->m_iRangeRedUV + 8;
        for (i = start; i < iFrmSrcAreaUV; i++) 
        {
            iTmp = (((((I32_WMV)pWMVDec->m_ppxliPostQU [i] - 128) * iMul) + pWMVDec->m_round)>> 3) + 128;
            pWMVDec->m_ppxliPostQU [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);//g_rgiClapTabDec [iTmp];
            iTmp = (((((I32_WMV)pWMVDec->m_ppxliPostQV [i] - 128) * iMul ) + pWMVDec->m_round)>> 3) + 128;
            pWMVDec->m_ppxliPostQV [i] = (U8_WMV) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);//g_rgiClapTabDec [iTmp];
        }
    }
}
Void_WMV AdjustDecRangeWMVA (tWMVDecInternalMember *pWMVDec)
{    
    //I32_WMV iFrmSrcAreaY = pWMVDec->m_iWidthPrevY * pWMVDec->m_iHeightPrevY;
    //I32_WMV iFrmSrcAreaUV = pWMVDec->m_iWidthPrevUV * pWMVDec->m_iHeightPrevUV;

     pWMVDec->m_round= 0;

#ifdef MARK_POST  //POST
    return;
#endif

    if(pWMVDec->m_bCodecIsWVC1)
        pWMVDec->m_round = 4;
	
    //assert (pWMVDec->m_cvCodecVersion == WMVA);
#ifdef STABILITY
    if(pWMVDec->m_cvCodecVersion != WMVA)
        return;
#endif
    

    {
        AdjustDecRangeWMVA_UP(pWMVDec);
        AdjustDecRangeWMVA_DOWN(pWMVDec);
    }
}



Void_WMV SetMotionFieldBFrame (I16_WMV *pMvX, I16_WMV *pMvY,
                           I16_WMV *pFMvX, I16_WMV *pFMvY,
                           I16_WMV *pBMvX, I16_WMV *pBMvY,
                           I32_WMV iTopMvX, I32_WMV iTopMvY, I32_WMV iBotMvX, I32_WMV iBotMvY,
                           I32_WMV iFTopMvX, I32_WMV iFTopMvY, I32_WMV iFBotMvX, I32_WMV iFBotMvY,
                           I32_WMV iBTopMvX, I32_WMV iBTopMvY, I32_WMV iBBotMvX, I32_WMV iBBotMvY,
                           I32_WMV iTopLeftBlkIndex, I32_WMV iBotLeftBlkIndex
                           )
{

    pMvX [iTopLeftBlkIndex] = (I16_WMV) iTopMvX;
    pMvY [iTopLeftBlkIndex] = (I16_WMV) iTopMvY;                    
    pMvX [iTopLeftBlkIndex + 1] = (I16_WMV) iTopMvX;
    pMvY [iTopLeftBlkIndex + 1] = (I16_WMV) iTopMvY;                    
    pMvX [iBotLeftBlkIndex] = (I16_WMV) iBotMvX;
    pMvY [iBotLeftBlkIndex] = (I16_WMV) iBotMvY;
    pMvX [iBotLeftBlkIndex + 1] = (I16_WMV) iBotMvX;
    pMvY [iBotLeftBlkIndex + 1] = (I16_WMV) iBotMvY;
    
    // backward
    pBMvX [iTopLeftBlkIndex] = (I16_WMV) iBTopMvX;
    pBMvY [iTopLeftBlkIndex] = (I16_WMV) iBTopMvY;                  
    pBMvX [iTopLeftBlkIndex + 1] = (I16_WMV) iBTopMvX;
    pBMvY [iTopLeftBlkIndex + 1] = (I16_WMV) iBTopMvY;                  
    pBMvX [iBotLeftBlkIndex] = (I16_WMV) iBBotMvX;
    pBMvY [iBotLeftBlkIndex] = (I16_WMV) iBBotMvY;
    pBMvX [iBotLeftBlkIndex + 1] = (I16_WMV) iBBotMvX;
    pBMvY [iBotLeftBlkIndex + 1] = (I16_WMV) iBBotMvY;
    
    // forward
    pFMvX [iTopLeftBlkIndex] = (I16_WMV) iFTopMvX;
    pFMvY [iTopLeftBlkIndex] = (I16_WMV) iFTopMvY;                  
    pFMvX [iTopLeftBlkIndex + 1] = (I16_WMV) iFTopMvX;
    pFMvY [iTopLeftBlkIndex + 1] = (I16_WMV) iFTopMvY;                  
    pFMvX [iBotLeftBlkIndex] = (I16_WMV) iFBotMvX;
    pFMvY [iBotLeftBlkIndex] = (I16_WMV) iFBotMvY;
    pFMvX [iBotLeftBlkIndex + 1] = (I16_WMV) iFBotMvX;
    pFMvY [iBotLeftBlkIndex + 1] = (I16_WMV) iFBotMvY;
    
}

Void_WMV Repeatpad_mbrow(tWMVDecInternalMember *pWMVDec,U32_WMV imbY)
{
	int i=0;		
	for(i=0;i<16;i++)
	{
		(*pWMVDec->pRepeatRef0Y) (pWMVDec->m_ppxliCurrQY +((imbY)*16+i)*pWMVDec->m_iWidthPrevY,
			((imbY)*16+i), ((imbY)*16+i)+1, 
			pWMVDec->m_iWidthPrevYXExpPlusExp,
			((imbY)*16+i)==0, ((imbY)*16+i)+1 == pWMVDec->m_iHeightYRepeatPad, 
			pWMVDec->m_iWidthYRepeatPad, 
			pWMVDec->m_iWidthYPlusExp, 
			pWMVDec->m_iWidthPrevY, 
			!pWMVDec->m_bInterlaceV2);
	}

	for(i=0;i<8;i++)
	{
		(*pWMVDec->pRepeatRef0UV) (pWMVDec->m_ppxliCurrQU +((imbY)*8+i)*pWMVDec->m_iWidthPrevUV,
			pWMVDec->m_ppxliCurrQV +((imbY)*8+i)*pWMVDec->m_iWidthPrevUV, 
			((imbY)*8+i), ((imbY)*8+i)+1, pWMVDec->m_iWidthPrevUVXExpPlusExp,
			((imbY)*8+i) == 0 , ((imbY)*8+i)+1 == pWMVDec->m_iHeightUVRepeatPad, pWMVDec->m_iWidthUVRepeatPad, pWMVDec->m_iWidthUVPlusExp, pWMVDec->m_iWidthPrevUV, !pWMVDec->m_bInterlaceV2);
	}
}

Void_WMV Repeatpad (tWMVDecInternalMember *pWMVDec)
{
    if (pWMVDec->m_bInterlaceV2 ) {
        // repeat pad the bottom field of the reference frame

        (*pWMVDec->pRepeatRef0Y) (
            pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevY, 0, pWMVDec->m_iHeightYRepeatPad >> 1,
            pWMVDec->m_iWidthPrevYXExpPlusExp, TRUE, TRUE, pWMVDec->m_iWidthYRepeatPad,
            pWMVDec->m_iWidthYPlusExp, pWMVDec->m_iWidthPrevY << 1, !pWMVDec->m_bInterlaceV2);
        (*pWMVDec->pRepeatRef0UV) (
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUV, pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUV,
            0, pWMVDec->m_iHeightUVRepeatPad >> 1, pWMVDec->m_iWidthPrevUVXExpPlusExp, TRUE, TRUE,
            pWMVDec->m_iWidthUVRepeatPad, pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV << 1, !pWMVDec->m_bInterlaceV2);

        // repeat pad the top field of the reference frame
        (*pWMVDec->pRepeatRef0Y) (
            pWMVDec->m_ppxliCurrQY, 0, pWMVDec->m_iHeightYRepeatPad >> 1,
            pWMVDec->m_iWidthPrevYXExpPlusExp, TRUE, TRUE, pWMVDec->m_iWidthYRepeatPad,
            pWMVDec->m_iWidthYPlusExp, pWMVDec->m_iWidthPrevY << 1, !pWMVDec->m_bInterlaceV2);
        (*pWMVDec->pRepeatRef0UV) (
            pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliCurrQV,
            0, pWMVDec->m_iHeightUVRepeatPad >> 1, pWMVDec->m_iWidthPrevUVXExpPlusExp, TRUE, TRUE,
            pWMVDec->m_iWidthUVRepeatPad, pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV << 1, !pWMVDec->m_bInterlaceV2);
    }
    else
	{
		int mb_row =0;
		for(mb_row = 0;mb_row<pWMVDec->m_iHeightYRepeatPad;mb_row+=2)
		{
			(*pWMVDec->pRepeatRef0Y) (pWMVDec->m_ppxliCurrQY +mb_row*pWMVDec->m_iWidthPrevY,
					mb_row, mb_row+1, 
					pWMVDec->m_iWidthPrevYXExpPlusExp,
					mb_row==0, mb_row+1 == pWMVDec->m_iHeightYRepeatPad, 
					pWMVDec->m_iWidthYRepeatPad, 
					pWMVDec->m_iWidthYPlusExp, 
					pWMVDec->m_iWidthPrevY, 
					!pWMVDec->m_bInterlaceV2);

			(*pWMVDec->pRepeatRef0Y) (pWMVDec->m_ppxliCurrQY +(mb_row+1)*pWMVDec->m_iWidthPrevY,
					mb_row+1, mb_row+2, 
					pWMVDec->m_iWidthPrevYXExpPlusExp,
					mb_row+1==0, mb_row+2 == pWMVDec->m_iHeightYRepeatPad, 
					pWMVDec->m_iWidthYRepeatPad, 
					pWMVDec->m_iWidthYPlusExp, 
					pWMVDec->m_iWidthPrevY, 
					!pWMVDec->m_bInterlaceV2);

			(*pWMVDec->pRepeatRef0UV) (pWMVDec->m_ppxliCurrQU +(mb_row>>1)*pWMVDec->m_iWidthPrevUV,
					pWMVDec->m_ppxliCurrQV +(mb_row>>1)*pWMVDec->m_iWidthPrevUV, 
					mb_row>>1, (mb_row>>1)+1, pWMVDec->m_iWidthPrevUVXExpPlusExp,
					(mb_row>>1) == 0 , (mb_row>>1)+1 == pWMVDec->m_iHeightUVRepeatPad, pWMVDec->m_iWidthUVRepeatPad, pWMVDec->m_iWidthUVPlusExp, pWMVDec->m_iWidthPrevUV, !pWMVDec->m_bInterlaceV2);

		}
	}

}
