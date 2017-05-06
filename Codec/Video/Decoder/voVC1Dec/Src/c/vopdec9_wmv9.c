//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) 2001  Microsoft Corporation

Module Name:

        vopenc9.cpp

Abstract:

        X9 encoding functions

Author:

        Sridhar Srinivasan (sridhsri@microsoft.com) 10-September-2001

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

/****************************************************************************************
  SetTransformTypeHuffmanTable
****************************************************************************************/
Void_WMV SetTransformTypeHuffmanTable (tWMVDecInternalMember *pWMVDec, I32_WMV iStep)
{
    if (iStep < 5) {
        pWMVDec->m_pHuf4x4PatternDec = &pWMVDec->m_huf4x4PatternLowQPDec;
        pWMVDec->m_pHufMBXformTypeDec = &pWMVDec->m_hufMBXformTypeLowQPDec;
        pWMVDec->m_pHufBlkXformTypeDec = &pWMVDec->m_hufBlkXformTypeLowQPDec;
    }
    else if (iStep < 13) { // Mid rate
        pWMVDec->m_pHuf4x4PatternDec = &pWMVDec->m_huf4x4PatternMidQPDec;
        pWMVDec->m_pHufMBXformTypeDec = &pWMVDec->m_hufMBXformTypeMidQPDec;
        pWMVDec->m_pHufBlkXformTypeDec = &pWMVDec->m_hufBlkXformTypeMidQPDec;
    }
    else { // Low rate
        pWMVDec->m_pHuf4x4PatternDec = &pWMVDec->m_huf4x4PatternHighQPDec;
        pWMVDec->m_pHufMBXformTypeDec = &pWMVDec->m_hufMBXformTypeHighQPDec;
        pWMVDec->m_pHufBlkXformTypeDec = &pWMVDec->m_hufBlkXformTypeHighQPDec;
    }
}
/****************************************************************************************************
  DecodeMB_1MV : WMV3 1MV macroblock decode
****************************************************************************************************/

tWMVDecodeStatus DecodeMB_1MV_WMVA_Intra (tWMVDecInternalMember *pWMVDec,
                                                                    CWMVMBMode* pmbmd, 
                                                                    CWMVMBMode* pmbmd_lastrow,
                                                                    U8_WMV __huge* ppxlcCurrQYMB,
                                                                    U8_WMV __huge* ppxlcCurrQUMB,
                                                                    U8_WMV __huge* ppxlcCurrQVMB,
                                                                    I32_WMV iMBX, I32_WMV iMBY)
{
    U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    tWMVDecodeStatus result;
    UnionBuffer * ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
    I32_WMV iShift, iblk, iDirection;
    I16_WMV  *pIntra,* pIntra_lastrow, pPredScaled [16];
    I32_WMV iX, iY;
    Bool_WMV bResidual;
    DQuantDecParam *pDQ;
	Bool_WMV bDCACPredOn;
	I16_WMV *pPred = NULL;
	Bool_WMV bNoDQandNoSlice;

    EMB_PMainLoop  * pPMainLoop = &pWMVDec->m_EMB_PMainLoop; 
    EMB_PBMainLoop *pMainPB = &pPMainLoop->PB[0];
    //EMB_PBMainLoop *pThreadPB = &pPMainLoop->PB[1];
    
	bNoDQandNoSlice = (!pWMVDec->m_bDQuantOn) && (!pWMVDec->m_bSliceWMVA);
    pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP ];
        
    if (pWMVDec->m_bDCTTable_MB){
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    }
    else{
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
    }

    // Y-blocks first (4 blocks)    
    iX = iMBX;
    iY = iMBY;
    
    for (iblk = 0; iblk < 4; iblk++)
    {
		pPred = NULL;
        iX = (iMBX << 1) + (iblk & 1);
        iY = (iMBY << 1) + ((iblk & 2) >> 1);
        bResidual = rgCBP2[iblk];
        ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;        
        //pIntra = pWMVDec->m_pX9dct + ((iX + iY * (pWMVDec->m_uintNumMBX << 1)) << 4);

        pIntra = pWMVDec->m_pX9dct + ((iX + ((1 << 1) + ((iblk & 2) >> 1)) * (pWMVDec->m_uintNumMBX << 1)) << 4);
        pIntra_lastrow =  pIntra-16*(pWMVDec->m_uintNumMBX << 1);

         if((iMBY&1) == 0)
        {
            pIntra = pWMVDec->m_pX9dct + ((iX + ((1 << 1) + ((iblk & 2) >> 1))*(pWMVDec->m_uintNumMBX << 1)) << 4);
            pIntra_lastrow =  pIntra-16*(pWMVDec->m_uintNumMBX << 1);
        }
        else
        {
            if(iblk == 0 || iblk ==1)
                pIntra_lastrow = pIntra + 16*(pWMVDec->m_uintNumMBX << 1) ;
            else
                pIntra_lastrow = pIntra - 3*16*(pWMVDec->m_uintNumMBX << 1);

            pIntra = pIntra - 2*16*(pWMVDec->m_uintNumMBX << 1);//pPB->m_pX9dct + ((iX + ((0 << 1) + ((iblk & 2) >> 1))*pPB->m_iNumBlockX) << 4);
        }
        
        if (bNoDQandNoSlice) {
			bDCACPredOn = decodeDCTPredictionY_IFrameNoDQuantAndNoSlice (pWMVDec, pmbmd, 
                                                                                                        pIntra,
                                                                                                        pIntra_lastrow,
                                                                                                        iblk, iX, iY, 
				                                                                                        &iShift, &iDirection, &pPred);
		} else {
			bDCACPredOn = decodeDCTPredictionY (pWMVDec,
                                                                pmbmd,
                                                                pmbmd_lastrow,
                                                                pIntra,
                                                                pIntra_lastrow,
                                                                iblk, iX, iY, 
				                                                &iShift, &iDirection, pPredScaled);
			if (bDCACPredOn) 
				pPred = pPredScaled;
		}  

		result = DecodeInverseIntraBlockX9 (pWMVDec, ppIntraDCTTableInfo_Dec, iblk,
	                                            bResidual, pPred, iDirection, iShift, pIntra, pmbmd, pDQ); 

        (*pWMVDec->m_pIntraX9IDCT_Dec) ((UnionBuffer*)ppxliErrorQMB,
            (UnionBuffer*)ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);

        // overlapped transform copy
        if (pWMVDec->m_iOverlap & 1) {
            I32_WMV i1;
            I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 4;
            for ( i1 = 0; i1 < 8; i1++)
                //memcpy (pWMVDec->m_pIntraBlockRow0[iblk & 2] + (iMBX << 4) + ((iblk & 1) << 3) + i1 * iWidth1,
                memcpy (pMainPB->pThreadIntraBlockRow0[iblk & 2] + (iMBX << 4) + ((iblk & 1) << 3) + i1 * iWidth1, ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
                //memcpy (pThreadPB->pThreadIntraBlockRow0[iblk & 2] + (iMBX << 4) + ((iblk & 1) << 3) + i1 * iWidth1, ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
        }
     
        pWMVDec->m_pAddErrorIntra (ppxlcCurrQYMB, ppxlcCurrQYMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevY);        
        pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
       
        ppxlcCurrQYMB += ((iblk & 1) ? pWMVDec->m_iWidthPrevYxBlkMinusBlk : BLOCK_SIZE);   
    }

    // U-block
    pPred = NULL;
    //pIntra = pWMVDec->m_pX9dctU + ((iMBX + iMBY * pWMVDec->m_uintNumMBX) << 4);
    //pIntra = pWMVDec->m_pX9dctU + ((iMBX + 1 * pWMVDec->m_uintNumMBX) << 4);

    if((iMBY&1) == 0)
    {
        pIntra = pWMVDec->m_pX9dctU + ((iMBX + 1 * pWMVDec->m_uintNumMBX) << 4);
        pIntra_lastrow =  pIntra-16*pWMVDec->m_uintNumMBX;//pPB->m_pX9dctUV[iblk-4] + ((0 *pPB->m_pWMVDec->m_uintNumMBX + iX) << 4);

    }
    else
    {
        pIntra_lastrow = pWMVDec->m_pX9dctU+ ((1 * pWMVDec->m_uintNumMBX + iMBX) << 4);
        pIntra =  pIntra_lastrow-16* pWMVDec->m_uintNumMBX;//pPB->m_pX9dctUV[iblk-4] + ((0 *pPB->m_pWMVDec->m_uintNumMBX + iX) << 4);
    }



	if (bNoDQandNoSlice) {
		bDCACPredOn = decodeDCTPredictionUV_IFrameNoDQuantAndNoSlice (pWMVDec, pmbmd, pIntra,pIntra_lastrow, iMBX, iMBY, 
											&iShift, &iDirection, &pPred);
    } 
    else {
		bDCACPredOn = decodeDCTPredictionUV(pWMVDec, pmbmd, pmbmd_lastrow,pIntra,pIntra_lastrow, iMBX, iMBY, 
											&iShift, &iDirection, pPredScaled);
        if (bDCACPredOn) 
            pPred = pPredScaled;
    }

    ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;

    result = DecodeInverseIntraBlockX9 (pWMVDec, ppInterDCTTableInfo_Dec, iblk,  rgCBP2[4], pPred,
					                                    iDirection, iShift, pIntra, pmbmd, pDQ);

     (*pWMVDec->m_pIntraX9IDCT_Dec) ((UnionBuffer*)ppxliErrorQMB,
            (UnionBuffer*)ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);

    // overlapped transform copy
    if (pWMVDec->m_iOverlap & 1) {
        I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 3;
        I32_WMV i1;
        for ( i1 = 0; i1 < 8; i1++)
            //memcpy (pWMVDec->m_pIntraMBRowU0[0] + (iMBX << 3) + i1 * iWidth1,
            memcpy (pMainPB->pThreadIntraMBRowU0[0] + (iMBX << 3) + i1 * iWidth1,
            ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
    }
       
    pWMVDec->m_pAddErrorIntra(ppxlcCurrQUMB, ppxlcCurrQUMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV );
    
    pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;
   
   iblk++;
   
   // V-block
   pPred = NULL; 
   //pIntra = pWMVDec->m_pX9dctV + ((iMBX + iMBY * pWMVDec->m_uintNumMBX) << 4);
   //pIntra = pWMVDec->m_pX9dctV + ((iMBX + 1 * pWMVDec->m_uintNumMBX) << 4);
   if((iMBY&1) == 0)
    {
        pIntra = pWMVDec->m_pX9dctV + ((iMBX + 1 * pWMVDec->m_uintNumMBX) << 4);
        pIntra_lastrow =  pIntra-16*pWMVDec->m_uintNumMBX;//pPB->m_pX9dctUV[iblk-4] + ((0 *pPB->m_pWMVDec->m_uintNumMBX + iX) << 4);
    }
    else
    {
        pIntra_lastrow = pWMVDec->m_pX9dctV+ ((1 * pWMVDec->m_uintNumMBX + iMBX) << 4);
        pIntra =  pIntra_lastrow-16* pWMVDec->m_uintNumMBX;//pPB->m_pX9dctUV[iblk-4] + ((0 *pPB->m_pWMVDec->m_uintNumMBX + iX) << 4);
    }

   if (bNoDQandNoSlice) {
	   bDCACPredOn = decodeDCTPredictionUV_IFrameNoDQuantAndNoSlice (pWMVDec, pmbmd, pIntra, pIntra_lastrow,iMBX, iMBY, 
		   &iShift, &iDirection, &pPred);
   } else {
	   bDCACPredOn = decodeDCTPredictionUV(pWMVDec, pmbmd, pmbmd_lastrow,pIntra, pIntra_lastrow,iMBX, iMBY, 
		   &iShift, &iDirection, pPredScaled);	   
	   if (bDCACPredOn) pPred = pPredScaled;
   }
   ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;   
   result = DecodeInverseIntraBlockX9 (pWMVDec, ppInterDCTTableInfo_Dec, iblk,
                                                rgCBP2[5], pPred, iDirection, iShift, pIntra, pmbmd, pDQ);

    (*pWMVDec->m_pIntraX9IDCT_Dec) ((UnionBuffer*)ppxliErrorQMB,
            (UnionBuffer*)ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);

    if (pWMVDec->m_iOverlap & 1) {

       I32_WMV i1 ;
       I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 3;
       for ( i1 = 0; i1 < 8; i1++)
           //memcpy (pWMVDec->m_pIntraMBRowV0[0] + (iMBX << 3) + i1 * iWidth1,
           memcpy (pMainPB->pThreadIntraMBRowV0[0] + (iMBX << 3) + i1 * iWidth1,
           ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
   }
     
   pWMVDec->m_pAddErrorIntra(ppxlcCurrQVMB, ppxlcCurrQVMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV );

   pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;

    // reset pmbmd->m_bSkip
    pmbmd->m_bSkip = FALSE_WMV;
    
    return WMV_Succeeded;
}

//Void_WMV ScaleTopPredForDQuant (tWMVDecInternalMember *pWMVDec, I32_WMV iblk, I16_WMV *pPred,
//                                I16_WMV *pPredScaled, CWMVMBMode* pmbmd,CWMVMBMode* pmbmd_lastrow)
//{
//    if (iblk == 0 || iblk == 1 || iblk == 4 || iblk == 5) {
//        I32_WMV iTmp;
//        I32_WMV i ;
//        //iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP ].iDCStepSize;
//        iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd_lastrow)->m_iQP ].iDCStepSize;
//        pPredScaled [0] = (I16_WMV)divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP ].iDCStepSize);
//        for ( i = 1; i < 16; i++) {
//            //iTmp = pPred [i] * (pmbmd - pWMVDec->m_uintNumMBX)->m_iQP;
//            iTmp = pPred [i] * (pmbmd_lastrow)->m_iQP;
//            pPredScaled [i] = (I16_WMV)divroundnearest (iTmp, pmbmd->m_iQP);
//        }
//        pPredScaled[8] = pPredScaled[0];
//    } else {
//        I32_WMV i ;
//        for ( i = 0; i < 16; i++) {
//            pPredScaled [i] = pPred [i];
//        }
//    }
//}
//
//
//Void_WMV ScaleLeftPredForDQuant (tWMVDecInternalMember *pWMVDec, I32_WMV iblk, I16_WMV *pPred,
//    I16_WMV *pPredScaled, CWMVMBMode* pmbmd)
//{
//    if (iblk == 0 || iblk == 2 || iblk == 4 || iblk == 5) {
//        I32_WMV iTmp;
//        I32_WMV i ;
//        iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP ].iDCStepSize;
//        pPredScaled [0] = (I16_WMV)divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP ].iDCStepSize);
//        for ( i = 1; i < 16; i++) {
//            iTmp = pPred [i] * (pmbmd - 1)->m_iQP;
//            pPredScaled [i] = (I16_WMV)divroundnearest (iTmp, pmbmd->m_iQP);
//        }
//        pPredScaled[8] = pPredScaled[0];
//    } else {
//        I32_WMV i;
//        for ( i = 0; i < 16; i++) {
//            pPredScaled [i] = pPred [i];
//        }
//    }
//}
Void_WMV ScaleTopPredForDQuant (tWMVDecInternalMember *pWMVDec, I32_WMV iblk, I16_WMV *pPred,
                                I16_WMV *pPredScaled, CWMVMBMode* pmbmd,CWMVMBMode* pmbmd_lastrow)
{
    if (iblk == 0 || iblk == 1 || iblk == 4 || iblk == 5) {
        I32_WMV iTmp;
        I32_WMV i ;
        if(pmbmd_lastrow->m_iQP == pmbmd->m_iQP)
        {
            for ( i = 0; i < 16; i++)
                pPredScaled [i] = pPred [i];
        }
        else
        {
            iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[pmbmd_lastrow->m_iQP ].iDCStepSize;
            pPredScaled [0] = (I16_WMV)divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP ].iDCStepSize);
            for ( i = 1; i < 16; i++) {
                if(pPred [i]==0)
                    pPredScaled [i] = 0;
                else
                {
                    iTmp = pPred [i] * pmbmd_lastrow->m_iQP;
                    pPredScaled [i] = (I16_WMV)divroundnearest (iTmp, pmbmd->m_iQP);
                }
            }
            pPredScaled[8] = pPredScaled[0];
        }
    } else {
        I32_WMV i ;
        for ( i = 0; i < 16; i++) {
            pPredScaled [i] = pPred [i];
        }
    }
}


Void_WMV ScaleLeftPredForDQuant (tWMVDecInternalMember *pWMVDec, I32_WMV iblk, I16_WMV *pPred,
    I16_WMV *pPredScaled, CWMVMBMode* pmbmd)
{
    if (iblk == 0 || iblk == 2 || iblk == 4 || iblk == 5) {
        I32_WMV iTmp;
        I32_WMV i ;
        if((pmbmd - 1)->m_iQP == pmbmd->m_iQP )
        {
            for ( i = 0; i < 16; i++)
                pPredScaled [i] = pPred [i];
        }
        else
        {
            iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP ].iDCStepSize;
            pPredScaled [0] = (I16_WMV)divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP ].iDCStepSize);        
            for ( i = 1; i < 16; i++) {
                if(pPred [i]==0)
                    pPredScaled [i] = 0;
                else
                {
                    iTmp = pPred [i] * (pmbmd - 1)->m_iQP;
                    pPredScaled [i] = (I16_WMV)divroundnearest (iTmp, pmbmd->m_iQP);
                }
            }
            pPredScaled[8] = pPredScaled[0];
        }        
    } else {
        I32_WMV i;
        for ( i = 0; i < 16; i++) {
            pPredScaled [i] = pPred [i];
        }
    }
}

Void_WMV ScaleDCPredForDQuant (tWMVDecInternalMember *pWMVDec, I32_WMV iblk, 
                                                CWMVMBMode* pmbmd, 
                                                CWMVMBMode* pmbmd_lastrow,
                                                I32_WMV *piTopDC, I32_WMV *piLeftDC, I32_WMV *piTopLeftDC)
{
    I32_WMV iTopDC = *piTopDC;
    I32_WMV iLeftDC = *piLeftDC;
    I32_WMV iTopLeftDC = *piTopLeftDC;
    
    if (iblk == 0 || iblk == 4 || iblk == 5) {
        I32_WMV iTmp;
        //I32_WMV iTopLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX - 1)->m_iQP ].iDCStepSize;
        //I32_WMV iTopDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP ].iDCStepSize;
        I32_WMV iTopLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd_lastrow - 1)->m_iQP ].iDCStepSize;
        I32_WMV iTopDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd_lastrow)->m_iQP ].iDCStepSize;

        I32_WMV iLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP ].iDCStepSize;
        I32_WMV iCurrDCStepSize = pWMVDec->m_prgDQuantParam[pmbmd->m_iQP ].iDCStepSize;
        iTmp = iTopLeftDC * iTopLeftDCStepSize;
        iTopLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iTopDC * iTopDCStepSize;
        iTopDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iLeftDC * iLeftDCStepSize;
        iLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
    } else if (iblk == 1) {
        I32_WMV iTmp;
        //I32_WMV iTopLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP ].iDCStepSize;
        //I32_WMV iTopDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP ].iDCStepSize;

        I32_WMV iTopLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd_lastrow)->m_iQP ].iDCStepSize;
        I32_WMV iTopDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd_lastrow)->m_iQP ].iDCStepSize;

        I32_WMV iCurrDCStepSize = pWMVDec->m_prgDQuantParam[pmbmd->m_iQP ].iDCStepSize;
        
        iTmp = iTopLeftDC * iTopLeftDCStepSize;
        iTopLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iTopDC * iTopDCStepSize;
        iTopDC = divroundnearest (iTmp, iCurrDCStepSize);
    } else if (iblk == 2) {
        I32_WMV iTmp;
        I32_WMV iTopLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP ].iDCStepSize;
        I32_WMV iLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP ].iDCStepSize;
        I32_WMV iCurrDCStepSize = pWMVDec->m_prgDQuantParam[pmbmd->m_iQP ].iDCStepSize;
        
        iTmp = iTopLeftDC * iTopLeftDCStepSize;
        iTopLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iLeftDC * iLeftDCStepSize;
        iLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
    } 
    
    *piTopDC = iTopDC ;
    *piLeftDC =   iLeftDC ;
    *piTopLeftDC =  iTopLeftDC ;
}

/****************************************************************************************************
  decodeDCTPredictionY,UV : find DCT-DCAC predictions for Y and U,V IMBs
****************************************************************************************************/
Bool_WMV decodeDCTPredictionY (tWMVDecInternalMember *pWMVDec,
                                            CWMVMBMode *pmbmd, 
                                            CWMVMBMode *pmbmd_lastrow,
                                            I16_WMV *pIntra, I16_WMV* pIntra_lastrow,
                                            I32_WMV iblk,
                                            I32_WMV iX, I32_WMV iY, 
                                            I32_WMV *piShift, I32_WMV *iDirection, I16_WMV *pPredScaled)
{
    Bool_WMV bACPredOn = TRUE_WMV;
    I32_WMV iStride = pWMVDec->m_uintNumMBX << 1;
    Bool_WMV   iShift = *piShift;
    I32_WMV  iWidth = iStride;
    I16_WMV *pPredIntraLeft, *pPredIntraTop, *pPred;
    Bool_WMV bDCACPred = FALSE_WMV;
    I32_WMV iIndex = iX + (iY * pWMVDec->m_uintNumMBX << 1);

   // CWMVMBMode *pmbmd_lastrow = NULL;

	iShift = 0;

    if (pWMVDec->m_tFrmType == BVOP ) {
        // if B frame, use MB packed motion vectors
        iIndex = (iX >> 1) + (iY >> 1) * (I32_WMV) pWMVDec->m_uintNumMBX;
        iWidth = (I32_WMV) pWMVDec->m_uintNumMBX;
    }
    
    pPred = pPredIntraLeft = pPredIntraTop = NULL_WMV;
    
    if (((pWMVDec->m_tFrmType == BVOP ) && (iY & 1)) ||
        (iY && ((pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) || pWMVDec->m_pXMotion[iIndex - iWidth] == IBLOCKMV))) {    

        if (((iY & 1) == 1) || !pWMVDec->m_pbStartOfSliceRow[iY>>1]) {
            pPred = pPredIntraTop = pIntra_lastrow;//pIntra - 16 * iStride;// + pWMVDec->m_iACRowPredOffset;
            iShift = pWMVDec->m_iACRowPredShift;
        }
    }
    
   if (((pWMVDec->m_tFrmType == BVOP  ) && (iX & 1)) ||
        (iX && ((pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) ||pWMVDec->m_pXMotion[iIndex - 1] == IBLOCKMV))) 
    {
        pPred = pPredIntraLeft = pIntra - 16;//pWMVDec->m_iACColPredOffset;
        iShift = pWMVDec->m_iACColPredShift;
        
    }
    if (pPredIntraLeft && pPredIntraTop) {
        I32_WMV iTopLeftDC = 0;
        I32_WMV iTopDC ;
        I32_WMV iLeftDC;
        if (((pWMVDec->m_tFrmType == BVOP ) && ((iX | iY) & 1)) ||
            ((pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) || pWMVDec->m_pXMotion[iIndex - iWidth - 1] == IBLOCKMV)) {

                           
            iTopLeftDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset - 16];
        }
        iTopDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset];
        iLeftDC = pPredIntraLeft[pWMVDec->m_iACColPredOffset];     

        //pmbmd_lastrow =  pmbmd - pWMVDec->m_uintNumMBX;
        ScaleDCPredForDQuant (pWMVDec, iblk, pmbmd, pmbmd_lastrow,&iTopDC, &iLeftDC, &iTopLeftDC);
        
        if (abs (iTopLeftDC - iLeftDC) < abs (iTopLeftDC - iTopDC)) {
            pPred = pPredIntraTop;
            iShift = pWMVDec->m_iACRowPredShift;
        }
    }
    
    if (pPred != NULL && !(pmbmd->m_rgbDCTCoefPredPattern2[0]))
        bACPredOn = FALSE_WMV;
    
    if (pPred != NULL_WMV) {
        bDCACPred = TRUE_WMV;
        if (iShift == pWMVDec->m_iACColPredShift) {
            ScaleLeftPredForDQuant (pWMVDec, iblk, pPred, pPredScaled, pmbmd);
        } else {
            //pmbmd_lastrow =  pmbmd - pWMVDec->m_uintNumMBX;
            ScaleTopPredForDQuant (pWMVDec, iblk, pPred, pPredScaled, pmbmd,pmbmd_lastrow);
        }
    }
    
    if (bACPredOn == FALSE_WMV) 
        iShift = -1;
    
    *iDirection = (iShift == pWMVDec->m_iACRowPredShift);
    *piShift =  iShift ;
    return bDCACPred;
}

Bool_WMV decodeDCTPredictionY_IFrameNoDQuantAndNoSlice (tWMVDecInternalMember *pWMVDec,
                                                                                    CWMVMBMode *pmbmd, 
                                                                                    I16_WMV *pIntra, 
                                                                                    I16_WMV* pIntra_lastrow,
                                                                                    I32_WMV iblk,
                                                                                    I32_WMV iX, I32_WMV iY, 
                                                                                    I32_WMV *piShift, 
                                                                                    I32_WMV *iDirection, 
                                                                                    I16_WMV **pPredScaled)
{
    Bool_WMV bACPredOn = TRUE_WMV;
    //I32_WMV iStride = pWMVDec->m_uintNumMBX << 1;
    Bool_WMV iShift;
    //I32_WMV  iWidth = iStride;
    I16_WMV *pPredIntraLeft, *pPredIntraTop, *pPred;
    Bool_WMV bDCACPred = FALSE_WMV;

	iShift = 0;    
    pPred = pPredIntraLeft = pPredIntraTop = NULL_WMV;

    if (iY) {
        pPred = pPredIntraTop = pIntra_lastrow;//pIntra - 16 * iStride;
        iShift = pWMVDec->m_iACRowPredShift;
    }

	if (iX) {
        pPred = pPredIntraLeft = pIntra - 16;
        iShift = pWMVDec->m_iACColPredShift;        
    }

    if (pPredIntraLeft && pPredIntraTop) {
        I32_WMV iTopLeftDC = 0;
        I32_WMV iTopDC ;
        I32_WMV iLeftDC;
                           
        iTopLeftDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset - 16];
        iTopDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset];
        iLeftDC = pPredIntraLeft[pWMVDec->m_iACColPredOffset];                                          
        if (abs (iTopLeftDC - iLeftDC) < abs (iTopLeftDC - iTopDC)) {
            pPred = pPredIntraTop;
            iShift = pWMVDec->m_iACRowPredShift;
        }
    }
    
    if (pPred != NULL && !(pmbmd->m_rgbDCTCoefPredPattern2[0]))
        bACPredOn = FALSE_WMV;
    
    if (pPred != NULL_WMV)
        bDCACPred = TRUE_WMV;

    *pPredScaled = pPred;
    if (bACPredOn == FALSE_WMV) 
        iShift = -1;
    
	*piShift = iShift;
    *iDirection = (iShift == pWMVDec->m_iACRowPredShift);
    return bDCACPred;
}

Bool_WMV decodeDCTPredictionUV(tWMVDecInternalMember *pWMVDec, 
                                            CWMVMBMode* pmbmd, 
                                            CWMVMBMode* pmbmd_lastrow,
                                            I16_WMV*     pIntra, I16_WMV*     pIntra_lastrow,
                                            I32_WMV imbX,     I32_WMV imbY, 
                                            I32_WMV     *piShift,
                                            I32_WMV *iDirection, I16_WMV *pPredScaled)

{
    Bool_WMV bACPredOn = TRUE_WMV;
    Bool_WMV     iShift=  *piShift;
    I16_WMV *pPredIntraLeft, *pPredIntraTop, *pPred;
    Bool_WMV bDCACPred = FALSE_WMV;
    //CWMVMBMode* pmbmd_lastrow = NULL;
    
    pPred = pPredIntraLeft = pPredIntraTop = NULL_WMV;
    iShift = 0;

   if (imbY && !pWMVDec->m_pbStartOfSliceRow[imbY]) {
        if (pWMVDec->m_pXMotionC[(imbY - 1) * (I32_WMV) pWMVDec->m_uintNumMBX + imbX] == IBLOCKMV || ( pWMVDec->m_tFrmType  == IVOP || pWMVDec->m_tFrmType  == BIVOP)) {
            pPred = pPredIntraTop = pIntra_lastrow;//pIntra - 16 * pWMVDec->m_uintNumMBX;// + pWMVDec->m_iACRowPredOffset;
            iShift = pWMVDec->m_iACRowPredShift;
        }
    }
    if (imbX) 
    {
        if (pWMVDec->m_pXMotionC[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX - 1] == IBLOCKMV
            || (pWMVDec->m_tFrmType  == IVOP || pWMVDec->m_tFrmType  == BIVOP))
        {
            pPred = pPredIntraLeft = pIntra - 16;// - pWMVDec->m_iACColPredOffset;
            iShift = pWMVDec->m_iACColPredShift;
        }
    }
    
    if (pPredIntraLeft && pPredIntraTop) {
        I32_WMV    iTopLeftDC = 0;
        I32_WMV iTopDC;
        I32_WMV iLeftDC;
        
        if (pWMVDec->m_pXMotionC[(imbY - 1) * (I32_WMV) pWMVDec->m_uintNumMBX + imbX - 1] == IBLOCKMV || (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType  == BIVOP))

            iTopLeftDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset - 16];
        
        iTopDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset];
        iLeftDC = pPredIntraLeft[pWMVDec->m_iACColPredOffset];   

        //pmbmd_lastrow =  pmbmd - pWMVDec->m_uintNumMBX;
        ScaleDCPredForDQuant (pWMVDec, 4, pmbmd,pmbmd_lastrow, &iTopDC, &iLeftDC, &iTopLeftDC);
        
        if (abs (iTopLeftDC - iLeftDC) < abs (iTopLeftDC - iTopDC)) {
            pPred = pPredIntraTop;
            iShift = pWMVDec->m_iACRowPredShift;
        }
    }
    
    if (  pPred != NULL_WMV && ! (pmbmd->m_rgbDCTCoefPredPattern2[0] )) {
        bACPredOn = FALSE_WMV;
        //        iShift = -1; // no AC Pred
    }
    
    if (pPred != NULL_WMV) 
        bDCACPred = TRUE_WMV;
    
    if (bDCACPred) {
        if (iShift == pWMVDec->m_iACColPredShift) {
            ScaleLeftPredForDQuant (pWMVDec, 4, pPred, pPredScaled, pmbmd);
        } else {
            //pmbmd_lastrow =  pmbmd - pWMVDec->m_uintNumMBX;
            ScaleTopPredForDQuant (pWMVDec, 4, pPred, pPredScaled, pmbmd,pmbmd_lastrow);
        }
    }
    
    if (bACPredOn == FALSE_WMV) 
        iShift = -1;
    *iDirection = (iShift == pWMVDec->m_iACRowPredShift);
    *piShift =  iShift;
    
    return bDCACPred;
}

Bool_WMV decodeDCTPredictionUV_IFrameNoDQuantAndNoSlice(tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I16_WMV*     pIntra, I16_WMV*     pIntra_lastrow,
    I32_WMV imbX,     I32_WMV imbY, 
    I32_WMV *piShift,
    I32_WMV *iDirection, I16_WMV **pPredScaled)

{
    Bool_WMV bACPredOn = TRUE_WMV;
    Bool_WMV iShift;
    I16_WMV *pPredIntraLeft, *pPredIntraTop, *pPred;
    Bool_WMV bDCACPred = FALSE_WMV;
    
    pPred = pPredIntraLeft = pPredIntraTop = NULL_WMV;
    iShift = 0;

	if (imbY) {
        pPred = pPredIntraTop = pIntra_lastrow;//pIntra - 16 * pWMVDec->m_uintNumMBX;// + pWMVDec->m_iACRowPredOffset;
        iShift = pWMVDec->m_iACRowPredShift;
    }
    if (imbX) {
        pPred = pPredIntraLeft = pIntra - 16;// - pWMVDec->m_iACColPredOffset;
        iShift = pWMVDec->m_iACColPredShift;
    }
    
    if (pPredIntraLeft && pPredIntraTop) {
        I32_WMV iTopLeftDC = 0;
        I32_WMV iTopDC;
        I32_WMV iLeftDC;
        
        iTopLeftDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset - 16];
        iTopDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset];
        iLeftDC = pPredIntraLeft[pWMVDec->m_iACColPredOffset];                                        
        
        if (abs (iTopLeftDC - iLeftDC) < abs (iTopLeftDC - iTopDC)) {
            pPred = pPredIntraTop;
            iShift = pWMVDec->m_iACRowPredShift;
        }
    }
    
    if (  pPred != NULL_WMV && ! (pmbmd->m_rgbDCTCoefPredPattern2[0] )) {
        bACPredOn = FALSE_WMV;
        //        iShift = -1; // no AC Pred
    }
    
    if (pPred != NULL_WMV) 
        bDCACPred = TRUE_WMV;
    
	*pPredScaled = pPred;
    if (bACPredOn == FALSE_WMV) 
        iShift = -1;

    *iDirection = (iShift == pWMVDec->m_iACRowPredShift);
    *piShift =  iShift;
    
    return bDCACPred;
}

Void_WMV ComputeDQuantDecParam (tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iStepSize;
    I32_WMV iDCStepSize ;
   
    for (iStepSize = 1; iStepSize < 63; iStepSize++) {
        DQuantDecParam *pDQ = &pWMVDec->m_rgDQuantParam3QPDeadzone [iStepSize];
        I32_WMV iDoubleStepSize = (iStepSize + 1);
        //I32_WMV iHalfStepSize = (iStepSize >> 1);

        pDQ->iDoubleStepSize = iDoubleStepSize;
        pDQ->iStepMinusStepIsEven = 0;
        pDQ->i2DoublePlusStepSize = iDoubleStepSize;
        pDQ->i2DoublePlusStepSizeNeg = -1 * pDQ->i2DoublePlusStepSize;

        iDCStepSize = (iStepSize + 1) >> 1;
        if (iDCStepSize <= 4) {
            pDQ->iDCStepSize = 8;
            if(/*pWMVDec->m_bNewDCQuant &&*/ iDCStepSize <= 2) {
                pDQ->iDCStepSize = 2 * iDCStepSize;
            }
        } else {  
            pDQ->iDCStepSize = (iDCStepSize >> 1) + 6;
        }
    }
    
    for (iStepSize = 1; iStepSize < 63; iStepSize++) {
        DQuantDecParam *pDQ = &pWMVDec->m_rgDQuantParam5QPDeadzone [iStepSize];
        I32_WMV iDoubleStepSize;
        I32_WMV iHalfStepSize;

        iDoubleStepSize = (iStepSize + 1);
        iHalfStepSize = (iStepSize >> 1);

        pDQ->iDoubleStepSize = iDoubleStepSize;

        if (pWMVDec->m_cvCodecVersion >= WMV3) {
            pDQ->iStepMinusStepIsEven = (iStepSize + 1) >> 1;
           
            pDQ->i2DoublePlusStepSize = iDoubleStepSize + pDQ->iStepMinusStepIsEven;
        } else {
            I32_WMV iStepSize2 = (iStepSize + 1) >> 1;
            pDQ->iStepMinusStepIsEven = iStepSize2 - ((iStepSize2 & 1) == 0);
            pDQ->i2DoublePlusStepSize = iDoubleStepSize + pDQ->iStepMinusStepIsEven;      
        }
            
        pDQ->i2DoublePlusStepSizeNeg = -1 * pDQ->i2DoublePlusStepSize;

        iDCStepSize = (iStepSize + 1) >> 1;
        if (iDCStepSize <= 4) {
            pDQ->iDCStepSize = 8;
            if(/*pWMVDec->m_bNewDCQuant && */iDCStepSize <= 2) 
                pDQ->iDCStepSize = 2 * iDCStepSize;

        } else {  
            pDQ->iDCStepSize = (iDCStepSize >> 1) + 6;
        }
    }           
}