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

#ifndef PPCWMP

#ifdef _WMV_TARGET_X86_
#include "cpudetect.h"
#endif // _WMV_TARGET_X86_

#ifdef _WMV9AP_

//
// Inter Codec 
//
static I32_WMV s_iMBXformTypeLUT[16] = {
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
};
static I32_WMV s_iSubblkPatternLUT[16] = {0, 1, 2, 3, 1, 2, 3, 0, 0, 1, 2, 3, 1, 2, 3, 0};

static I32_WMV s_iBlkXformTypeLUT[8] = {
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
};

Void_WMV ComputeFrameMVFromDiffMVInterlaceV2B (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV)
{
    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
    I32_WMV k = 2 * imbY * iXBlocks + 2 * imbX ;
    I32_WMV dX, dY; 
    Bool_WMV bTopBndry = (imbY == 0) || pWMVDec->m_pbStartOfSliceRow [imbY];

	I16_WMV *pXMotion=NULL, *pYMotion=NULL;
	if (pmbmd->m_mbType == FORWARD)
	{
		pXMotion = pWMVDec->m_pFieldMvX_FPred;
		pYMotion = pWMVDec->m_pFieldMvY_FPred;
	}
	else if (pmbmd->m_mbType == BACKWARD)
	{
		pXMotion = pWMVDec->m_pFieldMvX_BPred;
		pYMotion = pWMVDec->m_pFieldMvY_BPred;
	}

	if (pmbmd->m_mbType == FORWARD || pmbmd->m_mbType == BACKWARD)
	{
		PredictFrameOneMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, 
			&dX, &dY, pXMotion, pYMotion, bTopBndry);

		pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = pWMVDec->m_pXMotion [k + iXBlocks]
			= pWMVDec->m_pXMotion [k + iXBlocks + 1]
			= (I16_WMV)(((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
		pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = pWMVDec->m_pYMotion [k + iXBlocks]
			= pWMVDec->m_pYMotion [k + iXBlocks + 1]
			= (I16_WMV)(((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);
	}
	else if (pmbmd->m_mbType == DIRECT)
	{
		pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = pWMVDec->m_pXMotion [k + iXBlocks]
			= pWMVDec->m_pXMotion [k + iXBlocks + 1]
			= 0;
		pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = pWMVDec->m_pYMotion [k + iXBlocks]
			= pWMVDec->m_pYMotion [k + iXBlocks + 1]
			= 0;

	}
	else if (pmbmd->m_mbType == INTERPOLATE)
	{
		PredictFrameOneMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, 
			&dX, &dY, pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred, bTopBndry);
		pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = pWMVDec->m_pXMotion [k + iXBlocks]
			= pWMVDec->m_pXMotion [k + iXBlocks + 1]
			= (I16_WMV)(((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
		pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = pWMVDec->m_pYMotion [k + iXBlocks]
			= pWMVDec->m_pYMotion [k + iXBlocks + 1]
			= (I16_WMV)(((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

		PredictFrameOneMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, 
			&dX, &dY, pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred, bTopBndry);
		pWMVDec->m_pB411InterpX [k + 1] = pWMVDec->m_pB411InterpX [k] = pWMVDec->m_pB411InterpX [k + iXBlocks]
			= pWMVDec->m_pB411InterpX [k + iXBlocks + 1]
			= (I16_WMV)((((pInDiffMV + 1)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
		pWMVDec->m_pB411InterpY [k + 1] = pWMVDec->m_pB411InterpY [k] = pWMVDec->m_pB411InterpY [k + iXBlocks]
			= pWMVDec->m_pB411InterpY [k + iXBlocks + 1]
			= (I16_WMV)((((pInDiffMV + 1)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);
	}
	else
		assert(0);
#ifdef _GENERATE_DXVA_DATA_
    if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
        dxvaSetMV(pWMVDec->m_pXMotion [k], pWMVDec->m_pYMotion [k]);
#endif
}

Void_WMV ComputeFieldMVFromDiffMVInterlaceV2B (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV)
{
    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
    I32_WMV k = 2 * imbY * iXBlocks + 2 * imbX ;
    I32_WMV dX, dY; 
    Bool_WMV bTopBndry = (imbY == 0) || pWMVDec->m_pbStartOfSliceRow [imbY];

	I16_WMV *pXMotion =NULL, *pYMotion=NULL;
	if (pmbmd->m_mbType == FORWARD)
	{
		pXMotion = pWMVDec->m_pFieldMvX_FPred;
		pYMotion = pWMVDec->m_pFieldMvY_FPred;
	}
	else if (pmbmd->m_mbType == BACKWARD)
	{
		pXMotion = pWMVDec->m_pFieldMvX_BPred;
		pYMotion = pWMVDec->m_pFieldMvY_BPred;
	}

	if (pmbmd->m_mbType == FORWARD || pmbmd->m_mbType == BACKWARD)
	{
		PredictFieldTopMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY,
			&dX, &dY, pXMotion, pYMotion, bTopBndry);

		pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 
			(I16_WMV)(((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
		pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 
			(I16_WMV)(((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);
	}
	else if (pmbmd->m_mbType == DIRECT)
	{
		pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 0;
		pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 0;
	}
	else if (pmbmd->m_mbType == INTERPOLATE)
	{
		PredictFieldTopMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY,
			&dX, &dY, pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred, bTopBndry);
		pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 
			(I16_WMV)(((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
		pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 
			(I16_WMV)(((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

		PredictFieldTopMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY,
			&dX, &dY, pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred, bTopBndry);
		pWMVDec->m_pB411InterpX [k + 1] = pWMVDec->m_pB411InterpX [k] = 
			(I16_WMV)((((pInDiffMV + 1)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
		pWMVDec->m_pB411InterpY [k + 1] = pWMVDec->m_pB411InterpY [k] = 
			(I16_WMV)((((pInDiffMV + 1)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

	}
	else if (pmbmd->m_mbType == FORWARD_BACKWARD)
	{
		pXMotion = pWMVDec->m_pFieldMvX_FPred;
		pYMotion = pWMVDec->m_pFieldMvY_FPred;
		PredictFieldTopMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY,
			&dX, &dY, pXMotion, pYMotion, bTopBndry);

		pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 
			(I16_WMV)(((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
		pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 
			(I16_WMV)(((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);
	}
	else if (pmbmd->m_mbType == BACKWARD_FORWARD)
	{
		pXMotion = pWMVDec->m_pFieldMvX_BPred;
		pYMotion = pWMVDec->m_pFieldMvY_BPred;
		PredictFieldTopMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY,
			&dX, &dY, pXMotion, pYMotion, bTopBndry);

		pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 
			(I16_WMV)(((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
		pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 
			(I16_WMV)(((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);
	}
	else
		assert(0);
    k += iXBlocks;

	if (pmbmd->m_mbType == FORWARD || pmbmd->m_mbType == BACKWARD)
	{
		PredictFieldBotMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, 
			&dX, &dY, pXMotion, pYMotion, bTopBndry);

		pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 
			(I16_WMV)((((pInDiffMV + 2)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
		pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 
			(I16_WMV)((((pInDiffMV + 2)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);
	}
	else if (pmbmd->m_mbType == DIRECT)
	{
		pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 0;
		pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 0;
	}
	else if (pmbmd->m_mbType == INTERPOLATE)
	{
		PredictFieldBotMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, 
			&dX, &dY, pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred, bTopBndry);
		pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 
			(I16_WMV)((((pInDiffMV + 2)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
		pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 
			(I16_WMV)((((pInDiffMV + 2)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

		PredictFieldBotMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, 
			&dX, &dY, pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred, bTopBndry);
		pWMVDec->m_pB411InterpX [k + 1] = pWMVDec->m_pB411InterpX [k] = 
			(I16_WMV)((((pInDiffMV + 2 + 1)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
		pWMVDec->m_pB411InterpY [k + 1] = pWMVDec->m_pB411InterpY [k] = 
			(I16_WMV)((((pInDiffMV + 2 + 1)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);
	}
	else if (pmbmd->m_mbType == FORWARD_BACKWARD)
	{
		pXMotion = pWMVDec->m_pFieldMvX_BPred;
		pYMotion = pWMVDec->m_pFieldMvY_BPred;
		PredictFieldBotMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, 
			&dX, &dY, pXMotion, pYMotion, bTopBndry);

		pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 
			(I16_WMV)((((pInDiffMV + 2)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
		pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 
			(I16_WMV)((((pInDiffMV + 2)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

	}
	else if (pmbmd->m_mbType == BACKWARD_FORWARD)
	{
		pXMotion = pWMVDec->m_pFieldMvX_FPred;
		pYMotion = pWMVDec->m_pFieldMvY_FPred;
		PredictFieldBotMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, 
			&dX, &dY, pXMotion, pYMotion, bTopBndry);

		pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 
			(I16_WMV)((((pInDiffMV + 2)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
		pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 
			(I16_WMV)((((pInDiffMV + 2)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

	}

}

#ifndef WMV9_SIMPLE_ONLY
Void_WMV AssignDirectMode(tWMVDecInternalMember *pWMVDec,I32_WMV imbX, I32_WMV imbY, I32_WMV iTopLeftBlkIndex, I32_WMV iBotLeftBlkIndex, Bool_WMV bFieldMode)
{
	I16_WMV iX, iY, iX1, iY1;
    	I32_WMV  iDFTopFieldMvX, iDFTopFieldMvY, iDFBotFieldMvX, iDFBotFieldMvY;
	I32_WMV  iDBTopFieldMvX, iDBTopFieldMvY, iDBBotFieldMvX, iDBBotFieldMvY;

	iX = pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex];
	iY = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex];
	iX1 = pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex];
	iY1 = pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex];



	DirectModeMV (pWMVDec,iX, iY, FALSE, imbX, imbY, &iDFTopFieldMvX, &iDFTopFieldMvY,
					&iDBTopFieldMvX, &iDBTopFieldMvY);

	DirectModeMV (pWMVDec,iX1, iY1, FALSE, imbX, imbY, &iDFBotFieldMvX, &iDFBotFieldMvY,
					&iDBBotFieldMvX, &iDBBotFieldMvY);

	if (!bFieldMode)
	{
		SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
								pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
								pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
								0, 0, 0, 0,
								iDFTopFieldMvX, iDFTopFieldMvY, iDFTopFieldMvX, iDFTopFieldMvY,							// don't know these at decoder
								iDBTopFieldMvX, iDBTopFieldMvY, iDBTopFieldMvX, iDBTopFieldMvY,
								iTopLeftBlkIndex, iBotLeftBlkIndex);
	}
	else
	{
		SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
								pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
								pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
								0, 0, 0, 0,
								iDFTopFieldMvX, iDFTopFieldMvY, iDFBotFieldMvX, iDFBotFieldMvY,							// don't know these at decoder
								iDBTopFieldMvX, iDBTopFieldMvY, iDBBotFieldMvX, iDBBotFieldMvY,
								iTopLeftBlkIndex, iBotLeftBlkIndex);

	}
}
#endif

enum {FRAMEMV = 0, FRAMEMV_FIELD, FRAME4MV, FRAME4MV_FIELD, 
      FIELDMV, FIELDMV_FRAME, FIELD4MV, FIELD4MV_FRAME,
      INTRA_FRAME, INTRA_FIELD};


#ifndef WMV9_SIMPLE_ONLY

tWMVDecodeStatus decodeBInterlaceV2 (tWMVDecInternalMember *pWMVDec)
{
    I32_WMV result = 0;
    U8_WMV *ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliRef0Y = pWMVDec->m_ppxliRef0YPlusExp;
    U8_WMV *ppxliRef0U = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliRef0V = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliRef1Y = pWMVDec->m_ppxliRef1YPlusExp;
    U8_WMV *ppxliRef1U = pWMVDec->m_ppxliRef1U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliRef1V = pWMVDec->m_ppxliRef1V + pWMVDec->m_iWidthPrevUVXExpPlusExp;

    I16_WMV ppxliErrorY [16*16], ppxliErrorU [64], ppxliErrorV [64];
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    U32_WMV imbY, imbX;

    Bool_WMV bFrameInPostBuf = FALSE;

    if ((pWMVDec->m_iBNumerator * pWMVDec->m_iBFrameReciprocal) >> 7) {
        pWMVDec->m_mbtShortBSymbol = BACKWARD;
        pWMVDec->m_mbtLongBSymbol = FORWARD;
    }
    else {
        pWMVDec->m_mbtShortBSymbol = FORWARD;
        pWMVDec->m_mbtLongBSymbol = BACKWARD;
    }

    

    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    
    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];
       
    pWMVDec->m_iFilterType = FILTER_BICUBIC;
    
    HUFFMANGET_DBG_HEADER(":decodeP411",12);
    //DECODE_FRAME_TYPE_COUNT_CPU_CYCLES(12);

#ifdef _GENERATE_DXVA_DATA_
    if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
    {
        dxvaStartFrame(pWMVDec->m_bLoopFilter, 0, FALSE);
    }
#endif

    // Set transform type huffman pointers
	SetTransformTypeHuffmanTable (pWMVDec,pWMVDec->m_iStepSize);
   
    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {   

        U8_WMV *ppxliCurrQYMB = ppxliCurrQY;
        U8_WMV *ppxliCurrQUMB = ppxliCurrQU;
        U8_WMV *ppxliCurrQVMB = ppxliCurrQV;
        U8_WMV *ppxliRef0YMB = ppxliRef0Y;
        U8_WMV *ppxliRef0UMB = ppxliRef0U;
        U8_WMV *ppxliRef0VMB = ppxliRef0V;
        U8_WMV *ppxliRef1YMB = ppxliRef1Y;
        U8_WMV *ppxliRef1UMB = ppxliRef1U;
        U8_WMV *ppxliRef1VMB = ppxliRef1V;
		U8_WMV *ppxliRefYMB=NULL, *ppxliRefUMB=NULL, *ppxliRefVMB=NULL;
        U32_WMV iBlk;

        if(pWMVDec->m_bCodecIsWVC1){
			pWMVDec->m_nrows  = imbY;
		}

        if (pWMVDec->m_bSliceWMVA) {
           // if (pWMVDec->m_uiNumSlicesWMVA > 1 && pWMVDec->m_uiCurrSlice < pWMVDec->m_uiNumSlicesWMVA - 1) {
                if (pWMVDec->m_pbStartOfSliceRow[imbY]) {
#ifndef _TEST_DROP_SLICE_
                    pWMVDec->m_uiCurrSlice++;
                    BS_flush(pWMVDec->m_pbitstrmIn);
                    result = ReadSliceHeaderWMVA (pWMVDec, imbY);
                    pWMVDec->m_bFirstEscCodeInFrame = TRUE;
					if (result != ICERR_OK)
                        return WMV_Failed;
#else // _TEST_DROP_SLICE
                    if (g_iDropSliceFrame == (I32_WMV) pWMVDec->m_t) {
                        pWMVDec->m_uiCurrSlice++;
                        if (!g_pbDropSlice[pWMVDec->m_uiCurrSlice - 1]) { // if prev slice is not dropped, flush
                            pWMVDec->m_pbitstrmIn->flush ();
                        }
                        if (g_pbDropSlice[pWMVDec->m_uiCurrSlice]) {
                            U32_WMV i = 0;
                            while (i < pWMVDec->m_puiNumBytesOfSliceWMVA[pWMVDec->m_uiCurrSlice]) {
                                pWMVDec->m_pbitstrmIn->getBits (8);
                                i++;
                            }

                            U32_WMV uiStartRow, uiEndRow;
                            uiStartRow = pWMVDec->m_puiFirstMBRowOfSlice[pWMVDec->m_uiCurrSlice];
                            if (pWMVDec->m_uiCurrSlice == pWMVDec->m_uiNumSlicesWMVA - 1) {
                                uiEndRow = pWMVDec->m_uintNumMBY;
                            } else { 
                                uiEndRow = pWMVDec->m_puiFirstMBRowOfSlice[pWMVDec->m_uiCurrSlice + 1];
                            }

                            U32_WMV uiRow;
                            for (uiRow = uiStartRow; uiRow < uiEndRow; uiRow++) {
                                pmbmd += pWMVDec->m_uintNumMBX;
                                ppxliCurrQYMB += 16 * pWMVDec->m_uintNumMBX;
                                ppxliCurrQUMB += 8 * pWMVDec->m_uintNumMBX;
                                ppxliCurrQVMB += 8 * pWMVDec->m_uintNumMBX;
                                ppxliRef0YMB += 16 * pWMVDec->m_uintNumMBX;
                                ppxliRef0UMB += 8 * pWMVDec->m_uintNumMBX;
                                ppxliRef0VMB += 8 * pWMVDec->m_uintNumMBX;
                                ppxliRef1YMB += 16 * pWMVDec->m_uintNumMBX;
                                ppxliRef1UMB += 8 * pWMVDec->m_uintNumMBX;
                                ppxliRef1VMB += 8 * pWMVDec->m_uintNumMBX;
                                ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY;
                                ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV;
                                ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV;
                                ppxliRef0Y += pWMVDec->m_iMBSizeXWidthPrevY;
                                ppxliRef0U += pWMVDec->m_iBlkSizeXWidthPrevUV;
                                ppxliRef0V += pWMVDec->m_iBlkSizeXWidthPrevUV;
                                ppxliRef1Y += pWMVDec->m_iMBSizeXWidthPrevY;
                                ppxliRef1U += pWMVDec->m_iBlkSizeXWidthPrevUV;
                                ppxliRef1V += pWMVDec->m_iBlkSizeXWidthPrevUV;
                            }
                            imbY = uiRow - 1;
                            continue;
                        } else {
                            result = ReadSliceHeaderWMVA (imbY);
                            if (result != ICERR_OK)
                                return WMV_Failed;
                        }
                    } else {
                        pWMVDec->m_uiCurrSlice++;
                        pWMVDec->m_pbitstrmIn->flush ();
                        result = ReadSliceHeaderWMVA (imbY);
                        if (result != ICERR_OK)
                            return WMV_Failed;
                    }                   
#endif // _TEST_DROP_SLICE_

                }
            //}
        }

        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) { 
            Bool_WMV bCBPPresent, b1MVPresent;
            I32_WMV iTopLeftBlkIndex ;
            I32_WMV iBotLeftBlkIndex ;

            I32_WMV iTopXB, iTopYB, iBotXB, iBotYB;
			I32_WMV iTopX, iTopY, iBotX, iBotY;
			//Bool_WMV bTopBndry = (imbY == 0);
            Bool_WMV bTopBndry = (imbY == 0) || pWMVDec->m_pbStartOfSliceRow [imbY];

        if(pWMVDec->m_bCodecIsWVC1){
			pWMVDec->m_ncols = imbX;
}

            pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT
            pmbmd->m_bBlkXformSwitchOn = FALSE;
            pmbmd->m_rgbDCTCoefPredPattern2 [0] = 0;
            pmbmd->m_dctMd = INTER;
            pmbmd->m_chFieldDctMode = FALSE; 
#ifdef _GENERATE_DXVA_DATA_
            if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
            {
                dxvaNextMB();
                dxvaSetMotionForward();
            }
#endif
            iTopLeftBlkIndex = 2*imbY*2*pWMVDec->m_uintNumMBX + imbX*2;
            iBotLeftBlkIndex = iTopLeftBlkIndex + 2*pWMVDec->m_uintNumMBX;
            if (pWMVDec->m_iPrevIFrame == 1)
		    {
                pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex] = 0;
                pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] = pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex] = 0;
                pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1] = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + 1] = 0;
                pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1] = pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + 1] = 0;

		    }
			else
			{
				if (pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] == IBLOCKMV)
					pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex] = 0;
				if (pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1] == IBLOCKMV)
					pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1] = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + 1] = 0;
				if (pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] == IBLOCKMV)
					pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] = pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex] = 0;
				if (pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1] == IBLOCKMV)
					pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1] = pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + 1] = 0;
			}
			memset (pWMVDec->m_ppxliFieldMB, 0, 6 * 8 * 8 * sizeof(I16_WMV));

            if (pWMVDec->m_iSkipbitCodingMode == SKIP_RAW) {
                pmbmd->m_bSkip = BS_getBits(pWMVDec->m_pbitstrmIn,1);
            }

            DecodePMBModeInterlaceV2 (pWMVDec, pmbmd, &bCBPPresent, &b1MVPresent);

            if (pmbmd->m_chMBMode == MB_INTRA) {
                I32_WMV iCBPCY;
                pmbmd->m_dctMd = INTRA;
                pmbmd->m_mbType = BACKWARD;
                pmbmd->m_chFieldDctMode = (I8_WMV) BS_getBits(pWMVDec->m_pbitstrmIn,1);
                bCBPPresent = BS_getBits(pWMVDec->m_pbitstrmIn,1);
                if (bCBPPresent) {
                    iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufICBPCYDec,pWMVDec->m_pbitstrmIn) + 1;  

                } else {
                    iCBPCY = 0;
                }
                for (iBlk = 0; iBlk < 6; iBlk ++) { // set CBP
                    pmbmd->m_rgcBlockXformMode [iBlk] = XFORMMODE_8x8;
                    pmbmd->m_rgbCodedBlockPattern2 [5 - iBlk] = (iCBPCY & 0x1);
                    iCBPCY >>= 1;
                }

                // get AC Pred Flag
                pmbmd->m_rgbDCTCoefPredPattern2 [0] = (U8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn,1);

                if (pWMVDec->m_bDQuantOn) {
                    if (ICERR_OK != decodeDQuantParam (pWMVDec,pmbmd)) {
                        return WMV_Failed;
                    }        
                }   
            } else {

            if (pmbmd->m_bSkip) {
                    
                result = decodeBMBOverheadInterlaceV2 ( pWMVDec,
								pmbmd, imbX, imbY, bCBPPresent, b1MVPresent);

				if (pmbmd->m_mbType == FORWARD)
				{
					ppxliRefYMB = ppxliRef0YMB;
					ppxliRefUMB = ppxliRef0UMB;
					ppxliRefVMB = ppxliRef0VMB;
                                        if(pWMVDec->m_bCodecIsWVC1){
					pWMVDec->vo_ppxliRefYPlusExp = pWMVDec->m_ppxliRef0YPlusExp;
					pWMVDec->vo_ppxliRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
					pWMVDec->vo_ppxliRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;
                                        }
				}
				else
				{
					ppxliRefYMB = ppxliRef1YMB;
					ppxliRefUMB = ppxliRef1UMB;
					ppxliRefVMB = ppxliRef1VMB;
                                        if(pWMVDec->m_bCodecIsWVC1){
					pWMVDec->vo_ppxliRefYPlusExp = pWMVDec->m_ppxliRef1YPlusExp;
					pWMVDec->vo_ppxliRefU = pWMVDec->m_ppxliRef1U+ pWMVDec->m_iWidthPrevUVXExpPlusExp;
					pWMVDec->vo_ppxliRefV = pWMVDec->m_ppxliRef1V+ pWMVDec->m_iWidthPrevUVXExpPlusExp;
                                        }
				} 
                memset (pmbmd->m_rgbCodedBlockPattern2, 0, 6 * sizeof (U8_WMV));        
                memset (pWMVDec->m_pDiffMV, 0, sizeof(CDiffMV) * 4);  
                } else {
                    result = decodeBMBOverheadInterlaceV2 (pWMVDec,
									                    pmbmd, imbX, imbY, bCBPPresent, b1MVPresent);

					if (pmbmd->m_mbType == FORWARD)
					{
						ppxliRefYMB = ppxliRef0YMB;
						ppxliRefUMB = ppxliRef0UMB;
						ppxliRefVMB = ppxliRef0VMB;
                                                if(pWMVDec->m_bCodecIsWVC1){
						pWMVDec->vo_ppxliRefYPlusExp = pWMVDec->m_ppxliRef0YPlusExp;
						pWMVDec->vo_ppxliRefU = pWMVDec->m_ppxliRef0U+ pWMVDec->m_iWidthPrevUVXExpPlusExp;
						pWMVDec->vo_ppxliRefV = pWMVDec->m_ppxliRef0V+ pWMVDec->m_iWidthPrevUVXExpPlusExp;
                                                }
					}
					else
					{
						ppxliRefYMB = ppxliRef1YMB;
						ppxliRefUMB = ppxliRef1UMB;
						ppxliRefVMB = ppxliRef1VMB;
                                                if(pWMVDec->m_bCodecIsWVC1){
						pWMVDec->vo_ppxliRefYPlusExp = pWMVDec->m_ppxliRef1YPlusExp;
						pWMVDec->vo_ppxliRefU = pWMVDec->m_ppxliRef1U+ pWMVDec->m_iWidthPrevUVXExpPlusExp;
						pWMVDec->vo_ppxliRefV = pWMVDec->m_ppxliRef1V+ pWMVDec->m_iWidthPrevUVXExpPlusExp;
                                                }
					} 
                } 
			}
                if (ICERR_OK != result) {
                    return WMV_Failed;
            }

            if (pmbmd->m_dctMd == INTRA) {
                I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
                I32_WMV k = 2 * imbY * iXBlocks + 2 * imbX ;

                // set mv to intra
                pWMVDec->m_pXMotion [k] = pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k + iXBlocks]
                    = pWMVDec->m_pXMotion [k + iXBlocks + 1] = IBLOCKMV;

                pWMVDec->m_pYMotion [k] = pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k + iXBlocks]
                    = pWMVDec->m_pYMotion [k + iXBlocks + 1] = 0;


                for (iBlk = 0; iBlk < 6; iBlk++) {
                    pmbmd->m_rgcBlockXformMode [iBlk] = XFORMMODE_8x8;
                }

                if(pWMVDec->m_bCodecIsWVC1){
					pWMVDec->m_iOverlap =0;  //zou 322
					pmbmd->m_bOverlapIMB =0;
				}
                result = DecodeIMBInterlaceV2 (pWMVDec,
                    imbY, imbX, pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                    ppxliErrorY, ppxliErrorU, ppxliErrorV);
                if (ICERR_OK != result) {
                    return WMV_Failed;
                }
            } else { // pmbmd->m_dctMd == INTER
                if (pmbmd->m_chMBMode == MB_1MV) {
                    ComputeFrameMVFromDiffMVInterlaceV2B (pWMVDec, pmbmd, imbX, imbY, pWMVDec->m_pDiffMV); 
					if (pmbmd->m_mbType == FORWARD || pmbmd->m_mbType == BACKWARD)
						MotionCompFrameInterlaceV2 (pWMVDec,
							imbX, imbY, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB, 
							ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
					else
					{
						if (pmbmd->m_mbType == DIRECT)
							memset (pWMVDec->m_pDiffMV, 0, sizeof(CDiffMV) * 4); 
						MotionCompFrameInterlaceV2B (pWMVDec, 
							imbX, imbY, ppxliRef0YMB, ppxliRef0UMB, ppxliRef0VMB, 
							ppxliRef1YMB, ppxliRef1UMB, ppxliRef1VMB,
							ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB, 
							pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV, pmbmd->m_mbType);
					}
                } else if (pmbmd->m_chMBMode == MB_4MV) { 
                    assert (0); // should never get here!!
                    ComputeFrame4MVFromDiffMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY, pWMVDec->m_pDiffMV);
                    MotionCompFrame4MvInterlaceV2 (pWMVDec, 
                        imbX, imbY, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB, 
                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                } else if (pmbmd->m_chMBMode == MB_FIELD) {
                    ComputeFieldMVFromDiffMVInterlaceV2B (pWMVDec, pmbmd, imbX, imbY, pWMVDec->m_pDiffMV);
					if (pmbmd->m_mbType == FORWARD || pmbmd->m_mbType == BACKWARD)
						MotionCompFieldInterlaceV2 (pWMVDec, 
							imbX, imbY, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB, 
							ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
					else
					{
						if (pmbmd->m_mbType == DIRECT)
							memset (pWMVDec->m_pDiffMV, 0, sizeof(CDiffMV) * 4); 
						MotionCompFieldInterlaceV2B (pWMVDec, 
							imbX, imbY, ppxliRef0YMB, ppxliRef0UMB, ppxliRef0VMB, 
							ppxliRef1YMB, ppxliRef1UMB, ppxliRef1VMB,
							ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB, 
							pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV, pmbmd->m_mbType);
					}
                } else { //if (pmbmd->m_chMBMode == MB_FIELD4MV) {
                    assert (0); // should never get here!!
                    ComputeField4MVFromDiffMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY, pWMVDec->m_pDiffMV);
                    MotionCompField4MvInterlaceV2 (pWMVDec, 
                        imbX, imbY, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB, 
                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                }

                result = DecodePMBInterlaceV2 (pWMVDec, imbX, imbY, pmbmd); 
                if (ICERR_OK != result) {
                    return WMV_Failed;
                }

                if (pmbmd->m_chFieldDctMode == FALSE) {
                    AddErrorFrameInterlaceV2 (pWMVDec,
                        pWMVDec->m_ppxliFieldMB, ppxliCurrQYMB, ppxliCurrQUMB, 
                        ppxliCurrQVMB, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
                } else {
                    AddErrorFieldInterlaceV2 (pWMVDec,
                        pWMVDec->m_ppxliFieldMB, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                        pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
                }
            }
			
            if (pmbmd->m_chMBMode == MB_FIELD) 
			{
				if (pmbmd->m_mbType == FORWARD)
				{
					PredictFieldTopMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, &iTopXB, &iTopYB, 
						pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred, bTopBndry);
					PredictFieldBotMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, &iBotXB, &iBotYB, 
						pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred, bTopBndry);
                    SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
                        pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
                        pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
                        pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],
                        pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],
                        iTopXB, iTopYB, iBotXB, iBotYB,                            // don't know these at decoder
                        iTopLeftBlkIndex, iBotLeftBlkIndex);
				}
				else if (pmbmd->m_mbType == BACKWARD)
				{
					PredictFieldTopMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, &iTopX, &iTopY, 
						pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred, bTopBndry);
					PredictFieldBotMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, &iBotX, &iBotY, 
						pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred, bTopBndry);
                    SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
                        pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
                        pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
                        pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],
                        iTopX, iTopY, iBotX, iBotY,                            // don't know these at decoder
                        pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],
                        iTopLeftBlkIndex, iBotLeftBlkIndex);
				}
				else if (pmbmd->m_mbType == DIRECT)
					AssignDirectMode(pWMVDec, imbX, imbY, iTopLeftBlkIndex, iBotLeftBlkIndex, TRUE);
				else if (pmbmd->m_mbType == INTERPOLATE)
					SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
						pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
						pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
						pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],
						pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],						
						pWMVDec->m_pB411InterpX [iTopLeftBlkIndex], pWMVDec->m_pB411InterpY [iTopLeftBlkIndex], pWMVDec->m_pB411InterpX [iBotLeftBlkIndex], pWMVDec->m_pB411InterpY [iBotLeftBlkIndex],
						iTopLeftBlkIndex, iBotLeftBlkIndex);
				else if (pmbmd->m_mbType == FORWARD_BACKWARD)
					SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
						pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
						pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
						pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],						
						pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],							// don't know these at decoder
						pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],						
						iTopLeftBlkIndex, iBotLeftBlkIndex);
				else if (pmbmd->m_mbType == BACKWARD_FORWARD)
					SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
						pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
						pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
						pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],
						pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],		//forw					// don't know these at decoder
						pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],	//backw				
						iTopLeftBlkIndex, iBotLeftBlkIndex);
            } 
			else 
			{
				if (pmbmd->m_mbType == FORWARD)
				{
					PredictFieldTopMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, &iTopXB, &iTopYB, 
						pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred, bTopBndry);
					PredictFieldBotMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, &iBotXB, &iBotYB, 
						pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred, bTopBndry);
                    SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
                        pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
                        pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
                        pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],
                        pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],
                        iTopXB, iTopYB, iBotXB, iBotYB,                            // don't know these at decoder
                        iTopLeftBlkIndex, iBotLeftBlkIndex);
				}
				else if (pmbmd->m_mbType == BACKWARD)
				{
					PredictFieldTopMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, &iTopX, &iTopY, 
						pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred, bTopBndry);
					PredictFieldBotMVInterlaceV2 (pWMVDec,pmbmd, imbX, imbY, &iBotX, &iBotY, 
						pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred, bTopBndry);
                    SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
                        pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
                        pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
                        pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],
                        iTopX, iTopY, iBotX, iBotY,                            // don't know these at decoder
                        pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],
                        iTopLeftBlkIndex, iBotLeftBlkIndex);
				}
				else if (pmbmd->m_mbType == DIRECT)
					AssignDirectMode(pWMVDec, imbX, imbY, iTopLeftBlkIndex, iBotLeftBlkIndex, FALSE);
				else if (pmbmd->m_mbType == INTERPOLATE)
					SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
						pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
						pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
						pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],
						pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],						
						pWMVDec->m_pB411InterpX [iTopLeftBlkIndex], pWMVDec->m_pB411InterpY [iTopLeftBlkIndex], pWMVDec->m_pB411InterpX [iTopLeftBlkIndex], pWMVDec->m_pB411InterpY [iTopLeftBlkIndex],
						iTopLeftBlkIndex, iBotLeftBlkIndex);

            }
#if 0
			if(0)
            {
				if(pWMVDec->m_nBframes ==12)
				{
					FILE*fp=fopen("E:/MyResource/Video/clips/wmv test files/interfiled/vo_dump.txt","a");
					int i,j;
					fprintf(fp,"-----------------------%d %d -----------\n",imbY,imbX);
					for(i=0;i<16;i++)
					{
						for(j=0;j<16;j++)
						{
							fprintf(fp,"%x   ",ppxliCurrQYMB[i*pWMVDec->m_iWidthInternal+j]);
						}
						fprintf(fp,"\n");
					}
					for(i=0;i<8;i++)
                    {
						for(j=0;j<8;j++)
                        {
							fprintf(fp,"%x   ",ppxliCurrQUMB[i*pWMVDec->m_iWidthInternalUV+j]);
                        }
						fprintf(fp,"\n");
					}
					for(i=0;i<8;i++)
					{
						for(j=0;j<8;j++)
                        {
							fprintf(fp,"%x   ",ppxliCurrQVMB[i*pWMVDec->m_iWidthInternalUV+j]);
						}
						fprintf(fp,"\n");
					}
					fclose(fp);
            }
        }
#endif
            pmbmd->m_bSkip = FALSE;
            pmbmd++;
            ppxliCurrQYMB += 16;
            ppxliCurrQUMB += 8;
            ppxliCurrQVMB += 8;
            ppxliRef0YMB += 16;
            ppxliRef0UMB += 8;
            ppxliRef0VMB += 8;
            ppxliRef1YMB += 16;
            ppxliRef1UMB += 8;
            ppxliRef1VMB += 8;
        }

        // point to the starting location of the first MB of each row
        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliRef0Y += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliRef0U += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliRef0V += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliRef1Y += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliRef1U += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliRef1V += pWMVDec->m_iBlkSizeXWidthPrevUV;
    }

#ifdef _GENERATE_DXVA_DATA_
    if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
    {
        dxvaEndFrame(pWMVDec->m_uintNumMBX, pWMVDec->m_uintNumMBY, FALSE);
    }
#endif

    if (pWMVDec->m_bLoopFilter
#ifdef _GENERATE_DXVA_DATA_
        && !pWMVDec->m_pDMO
#endif        
        ) {
        LoopFilterRowPFrameInterlaceV2 (pWMVDec,
            0, pWMVDec->m_uintNumMBY,
            pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_rgmbmd);

        LoopFilterColPFrameInterlaceV2 (pWMVDec,
            0, pWMVDec->m_uintNumMBY,
            pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_rgmbmd);

    }
#if 0
	if(pWMVDec->m_nBframes == 1 && 0)
	{
		int i,j;
		FILE*fp=fopen("E:/MyResource/Video/clips/wmv test files/interfiled/vo_dump.txt","a");

		ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
		ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
		ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;

		for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++)
		{
			U8_WMV *ppxliCurrQYMB = ppxliCurrQY;
			U8_WMV *ppxliCurrQUMB = ppxliCurrQU;
			U8_WMV *ppxliCurrQVMB = ppxliCurrQV;
			for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++)
			{ 
				fprintf(fp,"-----------------------%d %d -----------\n",imbY,imbX);
				for(i=0;i<16;i++)
				{
					for(j=0;j<16;j++)
					{
						fprintf(fp,"%x   ",ppxliCurrQYMB[i*pWMVDec->m_iWidthInternal+j]);
					}
					fprintf(fp,"\n");
				}
				for(i=0;i<8;i++)
				{
					for(j=0;j<8;j++)
					{
						fprintf(fp,"%x   ",ppxliCurrQUMB[i*pWMVDec->m_iWidthInternalUV+j]);
					}
					fprintf(fp,"\n");
				}
				for(i=0;i<8;i++)
				{
					for(j=0;j<8;j++)
					{
						fprintf(fp,"%x   ",ppxliCurrQVMB[i*pWMVDec->m_iWidthInternalUV+j]);
					}
					fprintf(fp,"\n");
				}
				ppxliCurrQYMB += 16;
				ppxliCurrQUMB += 8;
				ppxliCurrQVMB += 8;
			}
			ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY;
			ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV;
			ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV;
		}	
		fclose(fp);
	}
#endif
    // if the destination bitmap pointer is NULL, don't do color conversion
  

    pWMVDec->m_bDisplay_AllMB = TRUE;
/*
    // Deinterlace
    if (pWMVDec->m_bHostDeinterlace){
//#ifdef _MultiThread_Decode_
//        DecodeMultiThreads(DEINTERLACE);
//#else    
        (*pWMVDec->m_pDeinterlaceProc)(pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliPostQY, 0, pWMVDec->m_iHeightPrevY, pWMVDec->m_iWidthPrevY, pWMVDec->m_iHeightPrevY);
//#endif
	    (*pWMVDec->m_pDeinterlaceProc)(pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliPostQU, 0, pWMVDec->m_iHeightPrevUV, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iHeightPrevUV);
	    (*pWMVDec->m_pDeinterlaceProc)(pWMVDec->m_ppxliCurrQV, pWMVDec->m_ppxliPostQV, 0, pWMVDec->m_iHeightPrevUV, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iHeightPrevUV);

        bFrameInPostBuf = TRUE;
    }
*/
   

    pWMVDec->m_bRenderFromPostBuffer = bFrameInPostBuf;

    pWMVDec->m_bCopySkipMBToPostBuf = TRUE; // PostBuf is not used in this frame so need to copy skipMB to PostBuf if PostBuf is used in next frame

    return WMV_Succeeded;
}

I32_WMV DecodeBMBInterlaceV2 (tWMVDecInternalMember *pWMVDec,I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd)
{
    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];
    //U8_WMV *rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    U32_WMV iBlk;
    Bool_WMV bFirstBlock = TRUE && pWMVDec->m_bMBXformSwitching;
        
    for (iBlk = 0; iBlk < 6; iBlk++) {
        I32_WMV result = DecodePBlockInterlaceV2 (pWMVDec,
            imbX, imbY, iBlk, pmbmd, pWMVDec->m_ppInterDCTTableInfo_Dec, pDQ,
            &bFirstBlock);
        if (ICERR_OK != result) {
            return result;
        }
    }

    return ICERR_OK;
}

#endif //WMV9_SIMPLE_ONLY

I32_WMV DecodeFieldModeBFrame (tWMVDecInternalMember *pWMVDec)
{
    //I32_WMV result = 0;
	assert(0);	// for now this is not safe ...

    return ICERR_OK;
}

/****************************************************************************************
  decodePFieldMode : Field Picture P frame decoding
****************************************************************************************/
I32_WMV decodeBFieldMode (tWMVDecInternalMember *pWMVDec)
{
    I16_WMV MVx[4], MVy[4];
	I16_WMV MVxB[4], MVyB[4];
    I32_WMV result;


    U8_WMV __huge* ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevY >> 1);
    U8_WMV __huge* ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1);
    U8_WMV __huge* ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp + + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1);
    U8_WMV __huge *ppxliSameRefY, *ppxliSameRefU, *ppxliSameRefV, *ppxliOppRefY, *ppxliOppRefU, *ppxliOppRefV;
    U8_WMV __huge *ppxliRefYMB=NULL, *ppxliRefUMB=NULL, *ppxliRefVMB=NULL;
    U8_WMV __huge *ppxliRef1YMB=NULL, *ppxliRef1UMB=NULL, *ppxliRef1VMB=NULL;
    U8_WMV __huge *ppxliSameRef1Y, *ppxliSameRef1U, *ppxliSameRef1V, *ppxliOppRef1Y, *ppxliOppRef1U, *ppxliOppRef1V;
    U8_WMV __huge* ppxliCurrQYMB, *ppxliCurrQUMB, *ppxliCurrQVMB;
    U8_WMV __huge* ppxliSameRefYMB, *ppxliSameRefUMB, *ppxliSameRefVMB;
    U8_WMV __huge* ppxliOppRefYMB, *ppxliOppRefUMB, *ppxliOppRefVMB;
    U8_WMV __huge* ppxliSameRef1YMB, *ppxliSameRef1UMB, *ppxliSameRef1VMB;
    U8_WMV __huge* ppxliOppRef1YMB, *ppxliOppRef1UMB, *ppxliOppRef1VMB;

    I32_WMV iOffset;

    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    Bool_WMV bHalfPelMode;

    CoordI x, y;
    U32_WMV imbX, imbY;

    U32_WMV uiNumMBFromSliceBdry = pWMVDec->m_uintNumMBX + 1;

  //  if (pWMVDec->m_bSliceWMVA) {
        if (pWMVDec->m_bInterlaceV2 && pWMVDec->m_bFieldMode && pWMVDec->m_iCurrentTemporalField == 1)
            pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig + pWMVDec->m_uintNumMBY;        
        else {
            pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;
        }    
  //  }

#ifdef _ANALYZEMC_
    if (pWMVDec->m_t == g_iFrame)
        g_bAddError = FALSE;
    else
        g_bAddError = TRUE;
#endif

#ifdef _GENERATE_DXVA_DATA_
    if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
        dxvaStartFrame(pWMVDec->m_bLoopFilter, 0, FALSE);
#endif
    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];

    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];

	if ((pWMVDec->m_iBNumerator * pWMVDec->m_iBFrameReciprocal) >> 7) {
			pWMVDec->m_mbtShortBSymbol = BACKWARD;
			pWMVDec->m_mbtLongBSymbol = DIRECT;
	}
	else {
		pWMVDec->m_mbtShortBSymbol = DIRECT;
		pWMVDec->m_mbtLongBSymbol = BACKWARD;
	}

    
    if (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR)
        pWMVDec->m_iFilterType = FILTER_BILINEAR;
    else
        pWMVDec->m_iFilterType = FILTER_BICUBIC;

    bHalfPelMode = (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL || pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR);


#ifdef SIXTAP
    pWMVDec->m_iFilterType = FILTER_6TAP;
#endif // SIXTAP
//printf ("QP:%d\n", pWMVDec->m_iStepSize);

    HUFFMANGET_DBG_HEADER(":decodeP_X9",9);
    //DECODE_FRAME_TYPE_COUNT_CPU_CYCLES(9);

    // update overlap related variables
    UpdateDCStepSize(pWMVDec, pWMVDec->m_iStepSize);

    // Set transform type huffman pointers
    SetTransformTypeHuffmanTable (pWMVDec,pWMVDec->m_iStepSize);

    pWMVDec->m_iOffsetToTopMB = pWMVDec->m_uintNumMBX;

    
  	ppxliSameRef1Y = pWMVDec->m_ppxliRef1YPlusExp + (pWMVDec->m_iWidthPrevY >> 1) * pWMVDec->m_iCurrentField;
	ppxliSameRef1U = pWMVDec->m_ppxliRef1U + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * pWMVDec->m_iCurrentField;
	ppxliSameRef1V = pWMVDec->m_ppxliRef1V + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * pWMVDec->m_iCurrentField;
	ppxliOppRef1Y = pWMVDec->m_ppxliRef1YPlusExp + (pWMVDec->m_iWidthPrevY >> 1) * pWMVDec->m_iCurrentField;
	ppxliOppRef1U = pWMVDec->m_ppxliRef1U + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * (pWMVDec->m_iCurrentField ^ 1);
	ppxliOppRef1V = pWMVDec->m_ppxliRef1V + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV>> 1) * (pWMVDec->m_iCurrentField ^ 1);


    ppxliSameRefY = pWMVDec->m_ppxliRef0YPlusExp + (pWMVDec->m_iWidthPrevY >> 1) * pWMVDec->m_iCurrentField;
    ppxliSameRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * pWMVDec->m_iCurrentField;
    ppxliSameRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * pWMVDec->m_iCurrentField;
    if (pWMVDec->m_iCurrentTemporalField == 0) {
        ppxliOppRefY = pWMVDec->m_ppxliRef0YPlusExp + (pWMVDec->m_iWidthPrevY >> 1) * pWMVDec->m_iCurrentField;
        ppxliOppRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * (pWMVDec->m_iCurrentField ^ 1);
        ppxliOppRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * (pWMVDec->m_iCurrentField ^ 1);
    }

    else {
		
        if (pWMVDec->m_bLuminanceWarpTop) {
            ppxliOppRefY = pWMVDec->m_ppxliRef0YPlusExp + (pWMVDec->m_iWidthPrevY >> 1);
            ppxliOppRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * (pWMVDec->m_iCurrentField ^ 1);
            ppxliOppRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * (pWMVDec->m_iCurrentField ^ 1);
		}
		else
		{
            ppxliOppRefY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp + (pWMVDec->m_iWidthPrevY >> 1) * pWMVDec->m_iCurrentField;
            ppxliOppRefU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * (pWMVDec->m_iCurrentField ^ 1);
            ppxliOppRefV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * (pWMVDec->m_iCurrentField ^ 1);
		}

        
    }

    if (pWMVDec->m_iCurrentField == 0)
        iOffset = 4;
    else
        iOffset = 2;

    if (bHalfPelMode)
        iOffset += (1 - 2 * pWMVDec->m_iCurrentField);



    
    for (imbY = 0, y = 0; imbY < pWMVDec->m_uintNumMBY; imbY++, y += MB_SIZE) {
        FUNCTION_PROFILE_DECL_START(fpInit,DECODEPMAININIT_PROFILE);
        pWMVDec->m_iOffsetToTopMB = -pWMVDec->m_iOffsetToTopMB;

        ppxliCurrQYMB = ppxliCurrQY + imbY * 16 * pWMVDec->m_iWidthPrevY;
        ppxliCurrQUMB = ppxliCurrQU + imbY * 8 * pWMVDec->m_iWidthPrevUV;
        ppxliCurrQVMB = ppxliCurrQV + imbY * 8 * pWMVDec->m_iWidthPrevUV;
        ppxliSameRefYMB = ppxliSameRefY + imbY * 16 * pWMVDec->m_iWidthPrevY;
        ppxliSameRefUMB = ppxliSameRefU + imbY * 8 * pWMVDec->m_iWidthPrevUV;
        ppxliSameRefVMB = ppxliSameRefV + imbY * 8 * pWMVDec->m_iWidthPrevUV;
        ppxliOppRefYMB = ppxliOppRefY + imbY * 16 * pWMVDec->m_iWidthPrevY;
        ppxliOppRefUMB = ppxliOppRefU + imbY * 8 * pWMVDec->m_iWidthPrevUV;
        ppxliOppRefVMB = ppxliOppRefV + imbY * 8 * pWMVDec->m_iWidthPrevUV;

        ppxliSameRef1YMB = ppxliSameRef1Y + imbY * 16 * pWMVDec->m_iWidthPrevY;
        ppxliSameRef1UMB = ppxliSameRef1U + imbY * 8 * pWMVDec->m_iWidthPrevUV;
        ppxliSameRef1VMB = ppxliSameRef1V + imbY * 8 * pWMVDec->m_iWidthPrevUV;
        ppxliOppRef1YMB = ppxliOppRef1Y + imbY * 16 * pWMVDec->m_iWidthPrevY;
        ppxliOppRef1UMB = ppxliOppRef1U + imbY * 8 * pWMVDec->m_iWidthPrevUV;
        ppxliOppRef1VMB = ppxliOppRef1V + imbY * 8 * pWMVDec->m_iWidthPrevUV;

       
        if (pWMVDec->m_bSliceWMVA) {
            /*
            U32_WMV uiNumFieldSlices;
            if (pWMVDec->m_iCurrentField == 0) {
                uiNumFieldSlices = pWMVDec->m_uiNumSlicesField1WMVA;
            } else {
                uiNumFieldSlices = pWMVDec->m_uiNumSlicesWMVA - pWMVDec->m_uiNumSlicesField1WMVA + 1;
            }
            */
            if (pWMVDec->m_iCurrentTemporalField == 1 && imbY == 0) {
                pWMVDec->m_uiCurrSlice++;
            }

            //if (uiNumFieldSlices > 1) {
                if (pWMVDec->m_pbStartOfSliceRow[imbY]) {
#ifndef _TEST_DROP_SLICE_
                    pWMVDec->m_uiCurrSlice++;
                    BS_flush(pWMVDec->m_pbitstrmIn);
                    result = ReadSliceHeaderWMVA (pWMVDec, imbY );
                    pWMVDec->m_bFirstEscCodeInFrame = TRUE;
                    if (result != ICERR_OK)
                        return result;
#else // _TEST_DROP_SLICE
                    if (g_iDropSliceFrame == (I32_WMV) pWMVDec->m_t) {
                        pWMVDec->m_uiCurrSlice++;
                        if (!g_pbDropSlice[pWMVDec->m_uiCurrSlice - 1]) { // if prev slice is not dropped, flush
                            pWMVDec->m_pbitstrmIn->flush ();
                        }
                        if (g_pbDropSlice[pWMVDec->m_uiCurrSlice]) {
                            U32_WMV i = 0;
                            while (i < pWMVDec->m_puiNumBytesOfSliceWMVA[pWMVDec->m_uiCurrSlice]) {
                                pWMVDec->m_pbitstrmIn->getBits (8);
                                i++;
                            }

                            U32_WMV uiStartRow, uiEndRow;
                            uiStartRow = pWMVDec->m_puiFirstMBRowOfSlice[pWMVDec->m_uiCurrSlice];
                            if (pWMVDec->m_uiCurrSlice == pWMVDec->m_uiNumSlicesWMVA - 1) {
                                uiEndRow = pWMVDec->m_uintNumMBY;
                            } else { 
                                uiEndRow = pWMVDec->m_puiFirstMBRowOfSlice[pWMVDec->m_uiCurrSlice + 1];
                            }
                            if (pWMVDec->m_iCurrentTemporalField == 1) {
                                uiStartRow -= pWMVDec->m_uintNumMBY;
                                uiEndRow -= pWMVDec->m_uintNumMBY;
                            }

                            U32_WMV uiRow;
                            for (uiRow = uiStartRow; uiRow < uiEndRow; uiRow++) {
                                pmbmd += pWMVDec->m_uintNumMBX;
                                y += MB_SIZE;
                            }
                            imbY = uiRow - 1;
                            y -= MB_SIZE;
                            continue;
                        } else {
                            result = ReadSliceHeaderWMVA (imbY );
                            if (result != ICERR_OK)
                                return result;
                        }
                    } else {
                        pWMVDec->m_uiCurrSlice++;
                        pWMVDec->m_pbitstrmIn->flush ();
                        result = ReadSliceHeaderWMVA (imbY );
                        if (result != ICERR_OK)
                            return result;
                    }                   
#endif // _TEST_DROP_SLICE_
                
                }
          //  }
        }

        FUNCTION_PROFILE_STOP(&fpInit);
        for (x = 0, imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++, x += MB_SIZE) {
            FUNCTION_PROFILE_DECL_START(fpPMBMode, DECODEMBMODE_PROFILE)
            I32_WMV iTopLeftBlkIndex;
            I32_WMV iBotLeftBlkIndex;
            I32_WMV iDMVX, iDMVY, iDMVXF, iDMVYF, iDMVXB, iDMVYB;
            Bool_WMV b1MV ;
            I32_WMV iPredX, iPredY;

            uiNumMBFromSliceBdry ++;
            pWMVDec->m_bMBHybridMV = FALSE;
#ifdef _GENERATE_DXVA_DATA_
            if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
                dxvaNextMB();
#endif      
			memset (pWMVDec->m_pDiffMV, 0, 4 * sizeof(CDiffMV));
#ifndef WMV9_SIMPLE_ONLY
            result = decodeMBOverheadOfBVOP_FieldPicture (pWMVDec, pmbmd, imbX, imbY);
#endif
                      

		    iTopLeftBlkIndex = 2*imbY*2*pWMVDec->m_uintNumMBX + imbX*2;
            iBotLeftBlkIndex = iTopLeftBlkIndex + 2*pWMVDec->m_uintNumMBX;

			
			if (pmbmd->m_mbType == DIRECT)
			{
				I32_WMV iMVOffset = ((pWMVDec->m_iCurrentField == 0) ? 0 : (2 * pWMVDec->m_uintNumMBY * 2 * pWMVDec->m_uintNumMBX));
				I16_WMV MVx[4];
				I16_WMV MVy[4];
				I32_WMV iAvMVx, iAvMVy;
				MVx[0] = pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + iMVOffset];
				MVy[0] = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + iMVOffset];
				MVx[1] = pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1 + iMVOffset];
				MVy[1] = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + 1 + iMVOffset];
				MVx[2] = pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + iMVOffset];
				MVy[2] = pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + iMVOffset];
				MVx[3] = pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1 + iMVOffset];
				MVy[3] = pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + 1 + iMVOffset];
				AverageMVFieldMode (pWMVDec, MVx, MVy, imbX, imbY, FALSE, &iAvMVx, &iAvMVy);

                if (iAvMVx == IBLOCKMV)
				{
					pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + iMVOffset] = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + iMVOffset] = 0;
					iDMVX = iDMVY = 0;
				}
				else
				{
					iDMVX = iAvMVx;
					iDMVY = iAvMVy;
				}
				DirectModeMV (pWMVDec,iDMVX, iDMVY, FALSE, imbX, imbY, &iDMVXF, &iDMVYF, &iDMVXB, &iDMVYB);
			}

			if (pmbmd->m_chMBMode == MB_1MV)
			{
				I16_WMV *pFieldMvX=NULL, *pFieldMvY=NULL;
                I32_WMV  iXBlocks = pWMVDec->m_uintNumMBX * 2;
                I32_WMV  k = imbY * 2 * iXBlocks + imbX * 2;
                MBType eDirection = 0;


				if (pmbmd->m_mbType == FORWARD || pmbmd->m_mbType == INTERPOLATE || pmbmd->m_mbType == DIRECT)
				{
					pFieldMvX = pWMVDec->m_pFieldMvX_FPred;
					pFieldMvY = pWMVDec->m_pFieldMvY_FPred;
                                        eDirection = FORWARD;

				}
				else if (pmbmd->m_mbType == BACKWARD)
				{
					pFieldMvX = pWMVDec->m_pFieldMvX_BPred;
					pFieldMvY = pWMVDec->m_pFieldMvY_BPred;
                                        eDirection = BACKWARD;
				}
				
				
				if (pWMVDec->m_pDiffMV->iIntra)
				{
					pWMVDec->m_pXMotion[k] = IBLOCKMV;
					pWMVDec->m_pYMotion[k] = 0;
				}
				else if (pmbmd->m_mbType == DIRECT)
				{
					pWMVDec->m_pFieldMvX_BPred[k] = (I16_WMV)iDMVXB; 
					pWMVDec->m_pFieldMvY_BPred[k] = (I16_WMV)iDMVYB; 
					pWMVDec->m_pXMotion[k] = pWMVDec->m_pFieldMvX_FPred[k] = (I16_WMV)iDMVXF; 
					pWMVDec->m_pYMotion[k] = pWMVDec->m_pFieldMvY_FPred[k] = (I16_WMV)iDMVYF;
				}
				else
				{
                    I32_WMV iPredX = imbX * 2;
					I32_WMV iPredY = imbY * 2;

				
					
					PredictMVFieldBPicture (pWMVDec, imbX * 2, imbY * 2, TRUE, TRUE, &iPredX, &iPredY, pFieldMvX, pFieldMvY, pWMVDec->m_pDiffMV->iY & 1, eDirection, bHalfPelMode);

					if (1) {//!pmbmd->m_bSkip) {
						pWMVDec->m_pXMotion[k] = (I16_WMV)(((iPredX + pWMVDec->m_pDiffMV->iX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
						pWMVDec->m_pYMotion[k] = (I16_WMV)(((iPredY + pWMVDec->m_pDiffMV->iY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);
					}
					else {
						pWMVDec->m_pXMotion[k] = (I16_WMV)iPredX;
						pWMVDec->m_pYMotion[k] = (I16_WMV)iPredY;
					}
					if (pmbmd->m_mbType == INTERPOLATE)
					{
						iPredX = imbX * 2;
						iPredY = imbY * 2;
						PredictMVFieldBPicture (pWMVDec, imbX * 2, imbY * 2, TRUE, TRUE, &iPredX, &iPredY, 
                            pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred, (pWMVDec->m_pDiffMV + 1)->iY & 1, BACKWARD, bHalfPelMode);
						if (1) {
							pWMVDec->m_pFieldMvX_BPred[k] = (I16_WMV)(((iPredX + (pWMVDec->m_pDiffMV + 1)->iX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
							pWMVDec->m_pFieldMvY_BPred[k] = (I16_WMV)(((iPredY + (pWMVDec->m_pDiffMV + 1)->iY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);
						}
						else
						{
							pWMVDec->m_pFieldMvX_BPred[k] = (I16_WMV)iPredX;
							pWMVDec->m_pFieldMvY_BPred[k] = (I16_WMV)iPredY;
						}
					}
				}
			}
			{
				I32_WMV iStride = (I32_WMV) pWMVDec->m_uintNumMBX * 2;
				I32_WMV iMVOffst = 2 * (iStride * imbY + imbX);
				pWMVDec->m_pXMotion[iMVOffst + 1] = pWMVDec->m_pXMotion[iMVOffst + iStride + 1] = pWMVDec->m_pXMotion[iMVOffst + iStride] = pWMVDec->m_pXMotion[iMVOffst];
				pWMVDec->m_pYMotion[iMVOffst + 1] = pWMVDec->m_pYMotion[iMVOffst + iStride + 1] = pWMVDec->m_pYMotion[iMVOffst + iStride] = pWMVDec->m_pYMotion[iMVOffst];
			}

            pWMVDec->m_pXMotionC [imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX] = 0;
            pWMVDec->m_pYMotionC [imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX] = 0;

            if (ICERR_OK != result) {
                FUNCTION_PROFILE_STOP(&fp);
                return result;
            }

            b1MV = (pmbmd->m_chMBMode == MB_1MV || pmbmd->m_chMBMode == MB_INTRA);

            if (b1MV) {
                I32_WMV iMVOffst = 4 * imbY * (I32_WMV) pWMVDec->m_uintNumMBX + 2 * imbX;
                I32_WMV iIntra;
                if (pmbmd->m_chMBMode == MB_INTRA) {
                    pWMVDec->m_pXMotion[iMVOffst] = pWMVDec->m_pXMotion[iMVOffst + 1] = 
                    pWMVDec->m_pXMotion[iMVOffst + 2 * pWMVDec->m_uintNumMBX] = pWMVDec->m_pXMotion[iMVOffst + 2 * pWMVDec->m_uintNumMBX + 1] = MVx[0] = IBLOCKMV;
                    pWMVDec->m_pYMotion[iMVOffst] = pWMVDec->m_pYMotion[iMVOffst + 1] = 
                    pWMVDec->m_pYMotion[iMVOffst + 2 * pWMVDec->m_uintNumMBX] = pWMVDec->m_pYMotion[iMVOffst + 2 * pWMVDec->m_uintNumMBX + 1] = MVy[0] = IBLOCKMV;
                }
                else {
                    I32_WMV iMVY;
                    if (bHalfPelMode) {
                        
                        MVx[0] = pWMVDec->m_pXMotion[iMVOffst] << 1;
                        iMVY = pWMVDec->m_pYMotion[iMVOffst];
                        if (iMVY & 1) {
                            iMVY = iMVY * 2 - iOffset;
                            iMVY = (iMVY & ~7) | 4 | ((iMVY & 7) >> 1);
                        }
                        else {
                            iMVY = iMVY * 2;
                            iMVY = (iMVY & ~7) | ((iMVY & 7) >> 1);
                        }
                        MVy[0] = (I16_WMV) iMVY;
                    }
                    else {
					    MVx[0] = pWMVDec->m_pXMotion[iMVOffst];
                        iMVY = pWMVDec->m_pYMotion[iMVOffst];
                        if (iMVY & 1) {
                            iMVY -= iOffset;
                            iMVY = (iMVY & ~7) | 4 | ((iMVY & 7) >> 1);
                        }
                        else
                            iMVY = (iMVY & ~7) | ((iMVY & 7) >> 1);
					    MVy[0] = (I16_WMV)iMVY;
                    }

				}
                if (MVy[0] & 4) {
					if (pmbmd->m_mbType == FORWARD || pmbmd->m_mbType == INTERPOLATE  || pmbmd->m_mbType == DIRECT)
					{
						ppxliRefYMB = ppxliOppRefYMB;
						ppxliRefUMB = ppxliOppRefUMB;
						ppxliRefVMB = ppxliOppRefVMB;
					}
					else if (pmbmd->m_mbType == BACKWARD)
					{
						ppxliRefYMB = ppxliOppRef1YMB;
						ppxliRefUMB = ppxliOppRef1UMB;
						ppxliRefVMB = ppxliOppRef1VMB;
					}
                }
                else {
					if (pmbmd->m_mbType == FORWARD || pmbmd->m_mbType == INTERPOLATE || pmbmd->m_mbType == DIRECT)
					{
						ppxliRefYMB = ppxliSameRefYMB;
						ppxliRefUMB = ppxliSameRefUMB;
						ppxliRefVMB = ppxliSameRefVMB;
					}
					else if (pmbmd->m_mbType == BACKWARD)
					{
						ppxliRefYMB = ppxliSameRef1YMB;
						ppxliRefUMB = ppxliSameRef1UMB;
						ppxliRefVMB = ppxliSameRef1VMB;
					}
                }
				if (pmbmd->m_mbType == INTERPOLATE  || pmbmd->m_mbType == DIRECT)
				{
                    I32_WMV iMVY ;
                    if (bHalfPelMode) {
                        MVxB[0] = pWMVDec->m_pFieldMvX_BPred[iMVOffst] << 1;
                        iMVY = pWMVDec->m_pFieldMvY_BPred[iMVOffst];
                        if (iMVY & 1) {
                            iMVY = iMVY * 2 - iOffset;
                            iMVY = (iMVY & ~7) | 4 | ((iMVY & 7) >> 1);
                        }
                        else {
                            iMVY = iMVY * 2;
                            iMVY = (iMVY & ~7) | ((iMVY & 7) >> 1);
                        }
                        MVyB[0] = (I16_WMV) iMVY;                    
                    }
                    else {
                        MVxB[0] = pWMVDec->m_pFieldMvX_BPred[iMVOffst];
                        iMVY = pWMVDec->m_pFieldMvY_BPred[iMVOffst];
                        if (iMVY & 1) {
                            iMVY -= iOffset;
                            iMVY = (iMVY & ~7) | 4 | ((iMVY & 7) >> 1);
                        }
                        else
                            iMVY = (iMVY & ~7) | ((iMVY & 7) >> 1);
					    MVyB[0] = (I16_WMV) iMVY;
                    }

					if (MVyB[0] & 4) 
					{
						ppxliRef1YMB = ppxliOppRef1YMB;
						ppxliRef1UMB = ppxliOppRef1UMB;
						ppxliRef1VMB = ppxliOppRef1VMB;
					}
					else
					{
						ppxliRef1YMB = ppxliSameRef1YMB;
						ppxliRef1UMB = ppxliSameRef1UMB;
						ppxliRef1VMB = ppxliSameRef1VMB;
					}
				}

                iIntra = (MVx[0] == IBLOCKMV);
                pmbmd ->m_rgcIntraFlag[0] = pmbmd ->m_rgcIntraFlag[1] = pmbmd ->m_rgcIntraFlag[2] = pmbmd ->m_rgcIntraFlag[3] =  (U8_WMV) iIntra;
                if (pmbmd->m_mbType != INTERPOLATE && pmbmd->m_mbType != DIRECT && pmbmd->m_bCBPAllZero == TRUE && !iIntra)
                {
                    pmbmd->m_rgcBlockXformMode[0] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[1] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[2] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[3] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;

                    if (!pWMVDec->m_pXMotion[iMVOffst] && !pWMVDec->m_pYMotion[iMVOffst]) {
                        // zero motion : always within bounds by definition
						if (pmbmd->m_mbType == FORWARD)
						{
							(*pWMVDec->m_pMotionCompZero) (
								ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
								ppxliSameRefYMB, ppxliSameRefUMB, ppxliSameRefVMB,
								pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
						}
						else if (pmbmd->m_mbType == BACKWARD)
						{
							(*pWMVDec->m_pMotionCompZero) (
								ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
								ppxliSameRef1YMB, ppxliSameRef1UMB, ppxliSameRef1VMB,
								pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
						}
                        pmbmd->m_bSkip = TRUE;
                        pmbmd->m_dctMd = INTER;
                    }
                    else
                    {
                        MotionCompFieldMode1MV (pWMVDec,
                            ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                            ppxliRefYMB, ppxliRefUMB, ppxliRefVMB,
                            imbX, imbY, MVx, MVy);
                        pmbmd->m_bSkip = FALSE;
                    }
                }
                else
				{
					if ((pmbmd->m_mbType == INTERPOLATE || pmbmd->m_mbType == DIRECT) && pmbmd->m_bCBPAllZero == TRUE)
					{
						pmbmd->m_rgcBlockXformMode[0] = XFORMMODE_8x8;
						pmbmd->m_rgcBlockXformMode[1] = XFORMMODE_8x8;
						pmbmd->m_rgcBlockXformMode[2] = XFORMMODE_8x8;
						pmbmd->m_rgcBlockXformMode[3] = XFORMMODE_8x8;
						pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;
						pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;
					}
					if (pmbmd->m_mbType == INTERPOLATE || pmbmd->m_mbType == DIRECT)
					{
						result = DecodeBMBFieldMode1MV (pWMVDec,
							pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB, 
							ppxliRefYMB, ppxliRefUMB, ppxliRefVMB,
							ppxliRef1YMB, ppxliRef1UMB, ppxliRef1VMB,
							imbX, imbY, MVx, MVy, MVxB, MVyB); 
					}
					else
					{
						result = DecodeMBFieldMode1MV (pWMVDec,
							pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB, 
							ppxliRefYMB, ppxliRefUMB, ppxliRefVMB,
							imbX, imbY, MVx, MVy); 
					}
				}
            }
            else {  // 4MV
				
                I32_WMV iBlockIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
                I32_WMV iMBIndex = imbY * pWMVDec->m_uintNumMBX + imbX;
                I32_WMV iblk, iCMVx, iCMVy;
                I16_WMV *pFieldMvX=NULL, *pFieldMvY=NULL;
                I32_WMV iX1 = imbX + imbX;
                I32_WMV iY1 = imbY + imbY;
                I32_WMV iIndex1 = iY1 * pWMVDec->m_uintNumMBX * 2 + iX1;
				I32_WMV iPredX, iPredY;

                assert(pmbmd->m_mbType == FORWARD || pmbmd->m_mbType == BACKWARD);

                pmbmd ->m_rgcIntraFlag[0] = (U8_WMV) pWMVDec->m_pDiffMV  -> iIntra;
                pmbmd ->m_rgcIntraFlag[1] = (U8_WMV)(pWMVDec->m_pDiffMV + 1) -> iIntra;
                pmbmd ->m_rgcIntraFlag[2] = (U8_WMV)(pWMVDec->m_pDiffMV + 2) -> iIntra;
                pmbmd ->m_rgcIntraFlag[3] = (U8_WMV)(pWMVDec->m_pDiffMV + 3) -> iIntra;

				
				if (pmbmd->m_mbType == FORWARD)
				{
					pFieldMvX = pWMVDec->m_pFieldMvX_FPred;
					pFieldMvY = pWMVDec->m_pFieldMvY_FPred;
				}
				else if (pmbmd->m_mbType == BACKWARD)
				{
					pFieldMvX = pWMVDec->m_pFieldMvX_BPred;
					pFieldMvY = pWMVDec->m_pFieldMvY_BPred;
				}
                
    

				PredictMVFieldBPicture (pWMVDec,iX1, iY1, FALSE, TRUE, &iPredX, &iPredY, pFieldMvX, pFieldMvY, pWMVDec->m_pDiffMV->iY & 1, pmbmd->m_mbType, FALSE);
				pFieldMvX[iIndex1] = pWMVDec->m_pXMotion[iIndex1] = (I16_WMV)(((iPredX + pWMVDec->m_pDiffMV->iX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
				pFieldMvY[iIndex1] = pWMVDec->m_pYMotion[iIndex1] = (I16_WMV)(((iPredY + pWMVDec->m_pDiffMV->iY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

				iX1++;  iIndex1++;
				PredictMVFieldBPicture (pWMVDec,iX1, iY1, FALSE, TRUE, &iPredX, &iPredY, pFieldMvX, pFieldMvY, (pWMVDec->m_pDiffMV + 1)->iY & 1, pmbmd->m_mbType, FALSE);
				pFieldMvX[iIndex1] = pWMVDec->m_pXMotion[iIndex1] = (I16_WMV)(((iPredX + (pWMVDec->m_pDiffMV + 1)->iX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
				pFieldMvY[iIndex1] = pWMVDec->m_pYMotion[iIndex1] = (I16_WMV)(((iPredY + (pWMVDec->m_pDiffMV + 1)->iY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

				iX1--;  iY1++;  iIndex1 += pWMVDec->m_uintNumMBX * 2 - 1;
				PredictMVFieldBPicture (pWMVDec,iX1, iY1, FALSE, TRUE, &iPredX, &iPredY, pFieldMvX, pFieldMvY, (pWMVDec->m_pDiffMV + 2)->iY & 1, pmbmd->m_mbType, FALSE);
				pFieldMvX[iIndex1] = pWMVDec->m_pXMotion[iIndex1] = (I16_WMV)(((iPredX + (pWMVDec->m_pDiffMV + 2)->iX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
				pFieldMvY[iIndex1] = pWMVDec->m_pYMotion[iIndex1] = (I16_WMV)(((iPredY + (pWMVDec->m_pDiffMV + 2)->iY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

				iX1++;  iIndex1++;
				PredictMVFieldBPicture (pWMVDec,iX1, iY1, FALSE, TRUE, &iPredX, &iPredY, pFieldMvX, pFieldMvY, (pWMVDec->m_pDiffMV + 3)->iY & 1, pmbmd->m_mbType, FALSE);
				pFieldMvX[iIndex1] = pWMVDec->m_pXMotion[iIndex1] = (I16_WMV)(((iPredX + (pWMVDec->m_pDiffMV + 3)->iX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
				pFieldMvY[iIndex1] = pWMVDec->m_pYMotion[iIndex1] = (I16_WMV)(((iPredY + (pWMVDec->m_pDiffMV + 3)->iY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

                // Decode chroma MVs
                for (iblk = 0; iblk < 4; iblk ++) {
                    I32_WMV  iMVY = pWMVDec->m_pYMotion[iBlockIndex + (iblk & 2) * pWMVDec->m_uintNumMBX + (iblk & 1)];
                    MVx[iblk] = pWMVDec->m_pXMotion[iBlockIndex + (iblk & 2) * pWMVDec->m_uintNumMBX + (iblk & 1)];
                    
                    if (iMVY & 1) {
                        iMVY -= iOffset;
                        iMVY = (iMVY & ~7) | 4 | ((iMVY & 7) >> 1);
                    }
                    else
                        iMVY = (iMVY & ~7) | ((iMVY & 7) >> 1);
                    MVy[iblk] = (I16_WMV) iMVY;
                }
                ChromaMVFieldMode (pWMVDec,MVx, MVy, imbX, imbY, b1MV, &iCMVx, &iCMVy);
                pWMVDec->m_pXMotionC[iMBIndex] = (I16_WMV)iCMVx;
                pWMVDec->m_pYMotionC[iMBIndex] = (I16_WMV)iCMVy;

				if (pmbmd->m_mbType == FORWARD)
					result = DecodeFieldMode4MV (pWMVDec,
						pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB, 
						ppxliSameRefYMB, ppxliSameRefUMB, ppxliSameRefVMB,
						ppxliOppRefYMB, ppxliOppRefUMB, ppxliOppRefVMB,
						imbX, imbY); 
				else
					result = DecodeFieldMode4MV (pWMVDec,
						pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB, 
						ppxliSameRef1YMB, ppxliSameRef1UMB, ppxliSameRef1VMB,
						ppxliOppRef1YMB, ppxliOppRef1UMB, ppxliOppRef1VMB,
						imbX, imbY); 
            }

            if (ICERR_OK != result) {
                return result;
            }
			
#ifndef WMV9_SIMPLE_ONLY
			if (pmbmd->m_mbType == FORWARD)
			{
				iPredX = imbX * 2;
				iPredY = imbY * 2;
				PredictMVFieldBPicture (pWMVDec,imbX * 2, imbY * 2, TRUE, FALSE, &iPredX, &iPredY, pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred, 0, BACKWARD, bHalfPelMode);
				if (pmbmd->m_chMBMode == MB_4MV)
				{
					pWMVDec->m_pFieldMvX_FPred [iTopLeftBlkIndex] = pWMVDec->m_pXMotion [iTopLeftBlkIndex];
					pWMVDec->m_pFieldMvX_FPred [iTopLeftBlkIndex + 1] = pWMVDec->m_pXMotion [iTopLeftBlkIndex + 1];
					pWMVDec->m_pFieldMvX_FPred [iBotLeftBlkIndex] = pWMVDec->m_pXMotion [iBotLeftBlkIndex];
					pWMVDec->m_pFieldMvX_FPred [iBotLeftBlkIndex + 1] = pWMVDec->m_pXMotion [iBotLeftBlkIndex + 1];
					pWMVDec->m_pFieldMvY_FPred [iTopLeftBlkIndex] = pWMVDec->m_pYMotion [iTopLeftBlkIndex];
					pWMVDec->m_pFieldMvY_FPred [iTopLeftBlkIndex + 1] = pWMVDec->m_pYMotion [iTopLeftBlkIndex + 1];
					pWMVDec->m_pFieldMvY_FPred [iBotLeftBlkIndex] = pWMVDec->m_pYMotion [iBotLeftBlkIndex];
					pWMVDec->m_pFieldMvY_FPred [iBotLeftBlkIndex + 1] = pWMVDec->m_pYMotion [iBotLeftBlkIndex + 1];

					pWMVDec->m_pFieldMvX_BPred [iTopLeftBlkIndex] = pWMVDec->m_pFieldMvX_BPred [iTopLeftBlkIndex + 1]
						= pWMVDec->m_pFieldMvX_BPred [iBotLeftBlkIndex] = pWMVDec->m_pFieldMvX_BPred [iBotLeftBlkIndex + 1] = (I16_WMV)iPredX;
					pWMVDec->m_pFieldMvY_BPred [iTopLeftBlkIndex] = pWMVDec->m_pFieldMvY_BPred [iTopLeftBlkIndex + 1]
						= pWMVDec->m_pFieldMvY_BPred [iBotLeftBlkIndex] = pWMVDec->m_pFieldMvY_BPred [iBotLeftBlkIndex + 1] = (I16_WMV)iPredY;
				}
				else
					SetMotionFieldBFrame (pWMVDec->m_pXMotion, pWMVDec->m_pYMotion,
						pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
						pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
						pWMVDec->m_pXMotion[iTopLeftBlkIndex], pWMVDec->m_pYMotion[iTopLeftBlkIndex], pWMVDec->m_pXMotion[iTopLeftBlkIndex], pWMVDec->m_pYMotion[iTopLeftBlkIndex],
						pWMVDec->m_pXMotion[iTopLeftBlkIndex], pWMVDec->m_pYMotion[iTopLeftBlkIndex], pWMVDec->m_pXMotion[iTopLeftBlkIndex], pWMVDec->m_pYMotion[iTopLeftBlkIndex],
						iPredX, iPredY, iPredX, iPredY,							// don't know these at decoder
						iTopLeftBlkIndex, iBotLeftBlkIndex);
			}
			else if (pmbmd->m_mbType == BACKWARD)
			{
				iPredX = imbX * 2;
				iPredY = imbY * 2;
				PredictMVFieldBPicture (pWMVDec,imbX * 2, imbY * 2, TRUE, FALSE, &iPredX, &iPredY, pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred, 0, FORWARD, bHalfPelMode);
				if (pmbmd->m_chMBMode == MB_4MV)
				{
					pWMVDec->m_pFieldMvX_BPred [iTopLeftBlkIndex] = pWMVDec->m_pXMotion [iTopLeftBlkIndex];
					pWMVDec->m_pFieldMvX_BPred [iTopLeftBlkIndex + 1] = pWMVDec->m_pXMotion [iTopLeftBlkIndex + 1];
					pWMVDec->m_pFieldMvX_BPred [iBotLeftBlkIndex] = pWMVDec->m_pXMotion [iBotLeftBlkIndex];
					pWMVDec->m_pFieldMvX_BPred [iBotLeftBlkIndex + 1] = pWMVDec->m_pXMotion [iBotLeftBlkIndex + 1];
					pWMVDec->m_pFieldMvY_BPred [iTopLeftBlkIndex] = pWMVDec->m_pYMotion [iTopLeftBlkIndex];
					pWMVDec->m_pFieldMvY_BPred [iTopLeftBlkIndex + 1] = pWMVDec->m_pYMotion [iTopLeftBlkIndex + 1];
					pWMVDec->m_pFieldMvY_BPred [iBotLeftBlkIndex] = pWMVDec->m_pYMotion [iBotLeftBlkIndex];
					pWMVDec->m_pFieldMvY_BPred [iBotLeftBlkIndex + 1] = pWMVDec->m_pYMotion [iBotLeftBlkIndex + 1];

					pWMVDec->m_pFieldMvX_FPred [iTopLeftBlkIndex] = pWMVDec->m_pFieldMvX_FPred [iTopLeftBlkIndex + 1]
						= pWMVDec->m_pFieldMvX_FPred [iBotLeftBlkIndex] = pWMVDec->m_pFieldMvX_FPred [iBotLeftBlkIndex + 1] = (I16_WMV)iPredX;
					pWMVDec->m_pFieldMvY_FPred [iTopLeftBlkIndex] = pWMVDec->m_pFieldMvY_FPred [iTopLeftBlkIndex + 1]
						= pWMVDec->m_pFieldMvY_FPred [iBotLeftBlkIndex] = pWMVDec->m_pFieldMvY_FPred [iBotLeftBlkIndex + 1] = (I16_WMV)iPredY;
				}
				else
					SetMotionFieldBFrame (pWMVDec->m_pXMotion, pWMVDec->m_pYMotion,
						pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
						pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
						pWMVDec->m_pXMotion[iTopLeftBlkIndex], pWMVDec->m_pYMotion[iTopLeftBlkIndex], pWMVDec->m_pXMotion[iTopLeftBlkIndex], pWMVDec->m_pYMotion[iTopLeftBlkIndex],
						iPredX, iPredY, iPredX, iPredY, // don't know these at decoder
						pWMVDec->m_pXMotion[iTopLeftBlkIndex], pWMVDec->m_pYMotion[iTopLeftBlkIndex], pWMVDec->m_pXMotion[iTopLeftBlkIndex], pWMVDec->m_pYMotion[iTopLeftBlkIndex],						
						iTopLeftBlkIndex, iBotLeftBlkIndex);

			}
			else if (pmbmd->m_mbType == INTERPOLATE || pmbmd->m_mbType == DIRECT)
			{
				SetMotionFieldBFrame (pWMVDec->m_pXMotion, pWMVDec->m_pYMotion,
					pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
					pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
					pWMVDec->m_pXMotion[iTopLeftBlkIndex], pWMVDec->m_pYMotion[iTopLeftBlkIndex], pWMVDec->m_pXMotion[iTopLeftBlkIndex], pWMVDec->m_pYMotion[iTopLeftBlkIndex],
					pWMVDec->m_pXMotion[iTopLeftBlkIndex], pWMVDec->m_pYMotion[iTopLeftBlkIndex], pWMVDec->m_pXMotion[iTopLeftBlkIndex], pWMVDec->m_pYMotion[iTopLeftBlkIndex],						
					pWMVDec->m_pFieldMvX_BPred[iTopLeftBlkIndex], pWMVDec->m_pFieldMvY_BPred[iTopLeftBlkIndex], pWMVDec->m_pFieldMvX_BPred[iTopLeftBlkIndex], pWMVDec->m_pFieldMvY_BPred[iTopLeftBlkIndex],
					iTopLeftBlkIndex, iBotLeftBlkIndex);

			}
#endif
            pmbmd++;
            ppxliCurrQYMB += MB_SIZE;
            ppxliCurrQUMB += BLOCK_SIZE;
            ppxliCurrQVMB += BLOCK_SIZE;
            ppxliSameRefYMB += MB_SIZE;
            ppxliSameRefUMB += BLOCK_SIZE;
            ppxliSameRefVMB += BLOCK_SIZE;
            ppxliOppRefYMB += MB_SIZE;
            ppxliOppRefUMB += BLOCK_SIZE;
            ppxliOppRefVMB += BLOCK_SIZE;

            ppxliSameRef1YMB += MB_SIZE;
            ppxliSameRef1UMB += BLOCK_SIZE;
            ppxliSameRef1VMB += BLOCK_SIZE;
            ppxliOppRef1YMB += MB_SIZE;
            ppxliOppRef1UMB += BLOCK_SIZE;
            ppxliOppRef1VMB += BLOCK_SIZE;
        }
       
    }

#ifdef _DISPLAY_MBTYPE_
    if (g_bDisplayVideoInfoBMP) 
        QueryMBType();
#endif

    // need to do this once more
    

#ifdef _GENERATE_DXVA_DATA_
    if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
        dxvaEndFrame(pWMVDec->m_uintNumMBX, pWMVDec->m_uintNumMBY, FALSE);
#endif

 #ifndef WMV9_SIMPLE_ONLY
   
    if (pWMVDec->m_bLoopFilter)
    {

        if (pWMVDec->m_bLoopFilter) {
            ComputeLoopFilterFlags_WMVA(pWMVDec);
#ifdef _GENERATE_DXVA_DATA_
            if (!pWMVDec->m_pDMO)      
#endif
            DeblockSLFrame_V9 (pWMVDec,
                pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevY >> 1), 
                pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1), 
                pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1),
                pWMVDec->m_puchFilter8x8RowFlagY0, pWMVDec->m_puchFilter8x8RowFlagU0, pWMVDec->m_puchFilter8x8RowFlagV0,
                pWMVDec->m_puchFilter8x4RowFlagY0, pWMVDec->m_puchFilter8x4RowFlagU0, pWMVDec->m_puchFilter8x4RowFlagV0,
                pWMVDec->m_puchFilter8x8ColFlagY0, pWMVDec->m_puchFilter8x8ColFlagU0, pWMVDec->m_puchFilter8x8ColFlagV0,
                pWMVDec->m_puchFilter4x8ColFlagY0, pWMVDec->m_puchFilter4x8ColFlagU0, pWMVDec->m_puchFilter4x8ColFlagV0,
                pWMVDec->m_pLoopFilterFlags, 0, pWMVDec->m_uiMBEnd0);
        }
        else
            DeblockSLFrame (pWMVDec,pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
                            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
                             FALSE, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);

    }

#endif

    // if the destination bitmap pointer is NULL, don't do color conversion
        
    pWMVDec->m_bDisplay_AllMB = (pWMVDec->m_bLoopFilter ||  pWMVDec->m_iResIndex != 0 || pWMVDec->m_iRefFrameNum != -1);

   
    pWMVDec->m_bRenderFromPostBuffer = FALSE;
     pWMVDec->m_bCopySkipMBToPostBuf = TRUE; // PostBuf is not used in this frame so need to copy skipMB to PostBuf if PostBuf is used in next frame


//#endif  // ifdef _GENERATE_DXVA_DATA_
    return ICERR_OK;
}

/****************************************************************************************************
  DecodeBMBFieldMode1MV : 1MV Field Mode macroblock decode
****************************************************************************************************/
I32_WMV DecodeBMBFieldMode1MV (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, 
    U8_WMV __huge* ppxlcCurrQYMB, U8_WMV __huge* ppxlcCurrQUMB, U8_WMV __huge* ppxlcCurrQVMB,
    U8_WMV __huge* ppxlcRefYMB, U8_WMV __huge* ppxlcRefUMB, U8_WMV __huge* ppxlcRefVMB,
	U8_WMV __huge* ppxlcRef1YMB, U8_WMV __huge* ppxlcRef1UMB, U8_WMV __huge* ppxlcRef1VMB,
    I32_WMV imbX, I32_WMV imbY, I16_WMV* MVx, I16_WMV* MVy, I16_WMV* MVxB, I16_WMV* MVyB
)
{
    //FUNCTION_PROFILE_DECL_START(fpDecode,DECODEPMB_PROFILE);
    U8_WMV* rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    Bool_WMV  bTop, bBottom, bLeft, bRight;
    DQuantDecParam *pDQ;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    Bool_WMV bFirstBlock = TRUE && pWMVDec->m_bMBXformSwitching;
    I32_WMV iSubblockPattern = pmbmd->m_iSubblockPattern;
    I32_WMV iXformType = pWMVDec->m_iFrameXformMode;

    UnionBuffer __huge* ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
    I32_WMV result;

    // Y-blocks first (4 blocks)

    I32_WMV iShift, iblk, iDirection;
    I16_WMV  *pIntra, pPredScaled [16];

    Bool_WMV bIntra = (pmbmd->m_chMBMode == MB_INTRA);
    I32_WMV iX = imbX, iY = imbY;
    Bool_WMV bSkipMB = !(imbX || imbY || bIntra);


    if (pWMVDec->m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;

    pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];

    if (pWMVDec->m_bDCTTable_MB){
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    }
    else{
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
    }

    
    // motion comp
    MotionCompFieldMode1MVB (pWMVDec,
        ppxlcCurrQYMB, ppxlcCurrQUMB, ppxlcCurrQVMB,
        ppxlcRefYMB, ppxlcRefUMB, ppxlcRefVMB,
		ppxlcRef1YMB, ppxlcRef1UMB, ppxlcRef1VMB,
        imbX, imbY, MVx, MVy, MVxB, MVyB);

    for (iblk = 0; iblk < 4; iblk++) {
        Bool_WMV bResidual = rgCBP[iblk];
        iX = imbX * 2 + (iblk & 1);
        iY = imbY * 2 + ((iblk & 2) >> 1);
        
        bSkipMB &= !bResidual;

        if (bIntra) {
            Bool_WMV bDCACPredOn ;
            I16_WMV *pPred = NULL;

            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;

            pIntra = pWMVDec->m_pX9dct + (iX + iY * pWMVDec->m_uintNumMBX * 2) * 16;
            bDCACPredOn = decodeDCTPredictionY (pWMVDec, pmbmd, pIntra, iblk, iX, iY, 
                &iShift, &iDirection, pPredScaled);

            
            if (bDCACPredOn) pPred = pPredScaled;
            result = DecodeInverseIntraBlockX9 (pWMVDec, ppIntraDCTTableInfo_Dec, iblk, bResidual, pPred,
                iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block

#ifdef _GENERATE_DXVA_DATA_
            if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
                dxvaDumpBlockedResiduals(ppxliErrorQMB);
#endif
#ifdef _ANALYZEMC_
            if (g_bAddError)
#endif
            pWMVDec->m_pAddError (ppxlcCurrQYMB, ppxlcCurrQYMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevY, pWMVDec->m_pcClapTable);
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
        }
        else if (rgCBP[iblk]) {
            if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
                I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec,pWMVDec->m_pbitstrmIn);
                iXformType = s_iBlkXformTypeLUT[iIndex];
                iSubblockPattern = s_iSubblkPatternLUT[iIndex];
            }

            pmbmd->m_rgcBlockXformMode[iblk] =(I8_WMV)iXformType;

            if (iXformType == XFORMMODE_8x8) {
                ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ); //get the quantized block      
                (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
            }

            else if (iXformType == XFORMMODE_8x4) {
                bTop = TRUE;
                bBottom = TRUE;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                memset(ppxliErrorQMB, 0, 256);
                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP[iblk] = iSubblockPattern;
                        bTop = (rgCBP[iblk] & 2);
                        bBottom = (rgCBP[iblk] & 1);
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP[iblk] = (U8_WMV)((bTop << 1) | bBottom);
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP[iblk] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bTop = (rgCBP[iblk] & 2);
                        bBottom = (rgCBP[iblk] & 1);
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP[iblk] = (U8_WMV)((bTop << 1) | bBottom);
                    }
                }

                if (bTop) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (bBottom) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block

                    (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
            }

            else if (iXformType == XFORMMODE_4x8) {
                bLeft = TRUE;
                bRight = TRUE;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                memset(ppxliErrorQMB, 0, 256);
                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP[iblk] = iSubblockPattern;
                        bLeft = rgCBP[iblk] & 2;
                        bRight = rgCBP[iblk] & 1;
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP[iblk] = (U8_WMV)((bLeft << 1) | bRight);
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP[iblk] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bLeft = rgCBP[iblk] & 2;
                        bRight = rgCBP[iblk] & 1;
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP[iblk] = (U8_WMV)((bLeft << 1) | bRight);
                    }
                }


                if (bLeft) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      

                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (bRight) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      

                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                } 
            }

            else if (iXformType == XFORMMODE_4x4)
            {
                I32_WMV i4x4Pattern ;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                memset(ppxliErrorQMB, 0, 256);

                i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec,pWMVDec->m_pbitstrmIn) + 1;
                rgCBP[iblk] = i4x4Pattern;

                if (i4x4Pattern & 8)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      

                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (i4x4Pattern & 4)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      

                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
                if (i4x4Pattern & 2)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      

                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 2);
                }
                if (i4x4Pattern & 1)
                {
                     result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      

					 (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 3);
                }
            }

            if (pWMVDec->m_pInterIDCT_Dec == g_IDCTDec_WMV2_Inter) {
                I32_WMV j1;
                for( j1= 0; j1 <64; j1++)
                    ppxliErrorQMB->i16[j1] = (I16_WMV) ppxliErrorQMB->i32[j1];
            }

#ifdef _GENERATE_DXVA_DATA_
            if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
                dxvaDumpBlockedResiduals(ppxliErrorQMB);
#endif
#ifdef _ANALYZEMC_
            if (g_bAddError)
#endif
            pWMVDec->m_pAddError (ppxlcCurrQYMB, ppxlcCurrQYMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevY, pWMVDec->m_pcClapTable);

            bFirstBlock = FALSE;
        }
        else
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;

        ppxlcCurrQYMB += ((iblk & 1) ? pWMVDec->m_iWidthPrevYxBlkMinusBlk : BLOCK_SIZE);
    }

    // UV
    pmbmd->m_rgcBlockXformMode[4] = pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;

    iX = imbX;
    iY = imbY;

    // U-block
    if (bIntra) {
        I16_WMV *pPred = NULL;
        ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
        bSkipMB = FALSE;

        pIntra = pWMVDec->m_pX9dctU + (imbX + imbY * pWMVDec->m_uintNumMBX) * 16;

        if (decodeDCTPredictionUV(pWMVDec,pmbmd, pIntra, imbX, imbY, &iShift, &iDirection, pPredScaled))
            pPred = pPredScaled;

        result = DecodeInverseIntraBlockX9 (pWMVDec,ppInterDCTTableInfo_Dec, 4, rgCBP[4], pPred,
                    iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE);
        

#ifdef _GENERATE_DXVA_DATA_
        if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
            dxvaDumpBlockedResiduals(ppxliErrorQMB);
#endif
#ifdef _ANALYZEMC_
        if (g_bAddError)
#endif
        pWMVDec->m_pAddError(ppxlcCurrQUMB, ppxlcCurrQUMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);

        pIntra = pWMVDec->m_pX9dctV + (imbX + imbY * pWMVDec->m_uintNumMBX) * 16;

        pPred = (decodeDCTPredictionUV(pWMVDec,pmbmd, pIntra, imbX, imbY, &iShift, &iDirection, pPredScaled)) ?
            pPredScaled : NULL;

        result = DecodeInverseIntraBlockX9 (pWMVDec,ppInterDCTTableInfo_Dec, 5, rgCBP[5], pPred,
                    iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE);

#ifdef _GENERATE_DXVA_DATA_
        if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
            dxvaDumpBlockedResiduals(ppxliErrorQMB);
#endif
#ifdef _ANALYZEMC_
        if (g_bAddError)
#endif
        pWMVDec->m_pAddError(ppxlcCurrQVMB, ppxlcCurrQVMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
    }
    else {
        I32_WMV iBlk;
        for ( iBlk = 4; iBlk < 6; iBlk++) if (rgCBP[iBlk]) {
            bSkipMB = FALSE;
            if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
                I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec,pWMVDec->m_pbitstrmIn);
                iXformType = s_iBlkXformTypeLUT[iIndex];
                iSubblockPattern = s_iSubblkPatternLUT[iIndex];
            }

            pmbmd->m_rgcBlockXformMode[iBlk] =(I8_WMV)iXformType;


            if (iXformType == XFORMMODE_8x8) {
                ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ); //get the quantized block 
                (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);          
            }

            else if (iXformType == XFORMMODE_8x4) {
                bTop = TRUE;
                bBottom = TRUE;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                memset(ppxliErrorQMB, 0, 256);
                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP[iBlk] = iSubblockPattern;
                        bTop = (rgCBP[iBlk] & 2);
                        bBottom = (rgCBP[iBlk] & 1);
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP[iBlk] = (U8_WMV)((bTop << 1) | bBottom);
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP[iBlk] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bTop = (rgCBP[iBlk] & 2);
                        bBottom = (rgCBP[iBlk] & 1);
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP[iBlk] = (U8_WMV)((bTop << 1) | bBottom);
                    }
                }

                if (bTop) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (bBottom) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
            }

            else if (iXformType == XFORMMODE_4x8) {
                bLeft = TRUE;
                bRight = TRUE;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                memset(ppxliErrorQMB, 0, 256);
                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP[iBlk] = iSubblockPattern;
                        bLeft = rgCBP[iBlk] & 2;
                        bRight = rgCBP[iBlk] & 1;
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP[iBlk] = (U8_WMV)((bLeft << 1) | bRight);
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP[iBlk] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bLeft = rgCBP[iBlk] & 2;
                        bRight = rgCBP[iBlk] & 1;
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP[iBlk] = (U8_WMV)((bLeft << 1) | bRight);
                    }
                }

                if (bLeft) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (bRight) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
            }

            else if (iXformType == XFORMMODE_4x4)
            {
                I32_WMV i4x4Pattern;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                memset(ppxliErrorQMB, 0, 256);

                i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec,pWMVDec->m_pbitstrmIn) + 1;
                rgCBP[iBlk] = i4x4Pattern;

                if (i4x4Pattern & 8)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (i4x4Pattern & 4)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
                if (i4x4Pattern & 2)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 2);
                }
                if (i4x4Pattern & 1)
                {
                     result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec,ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 3);
                }
            }

            if(pWMVDec->m_pInterIDCT_Dec == g_IDCTDec_WMV2_Inter) {
                I32_WMV j1;
                for( j1= 0; j1 <64; j1++)
                    ppxliErrorQMB->i16[j1] = (I16_WMV) ppxliErrorQMB->i32[j1];
            }

#ifdef _GENERATE_DXVA_DATA_
            if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
                dxvaDumpBlockedResiduals(ppxliErrorQMB);
#endif
#ifdef _ANALYZEMC_
            if (g_bAddError) {
                if (iBlk == 4)
                    pWMVDec->m_pAddError(ppxlcCurrQUMB, ppxlcCurrQUMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
                else
                    pWMVDec->m_pAddError(ppxlcCurrQVMB, ppxlcCurrQVMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
            }
#else
            if (iBlk == 4)
                pWMVDec->m_pAddError(ppxlcCurrQUMB, ppxlcCurrQUMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
            else
                pWMVDec->m_pAddError(ppxlcCurrQVMB, ppxlcCurrQVMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
#endif
            bFirstBlock = FALSE;
        }
    }

    // reset pmbmd->m_bSkip
    pmbmd->m_bSkip = bSkipMB;

    return ICERR_OK;
}

/****************************************************************************************
  MotionCompFieldMode1MV : macroblock motion compensation for 1MV field mode
****************************************************************************************/
Void_WMV MotionCompFieldMode1MVB (tWMVDecInternalMember *pWMVDec,
    U8_WMV *pDstY, U8_WMV *pDstU, U8_WMV *pDstV,
    U8_WMV *pRefY, U8_WMV *pRefU, U8_WMV *pRefV,
	U8_WMV *pRef1Y, U8_WMV *pRef1U, U8_WMV *pRef1V,
    I32_WMV imbX, I32_WMV imbY, I16_WMV *MVx, I16_WMV* MVy, I16_WMV *MVxB, I16_WMV *MVyB)
{
    if (MVx[0] == IBLOCKMV) {  // if I block
        I32_WMV ii ;

        for ( ii = 0; ii < 8; ii++) {
            memset (pDstY, 128, 16);
            memset (pDstY + pWMVDec->m_iWidthPrevY, 128, 16);
            pDstY += pWMVDec->m_iWidthPrevY * 2;
            memset (pDstU, 128, 8);
            memset (pDstV, 128, 8);
            pDstU += pWMVDec->m_iWidthPrevUV;
            pDstV += pWMVDec->m_iWidthPrevUV;
        }
        pWMVDec->m_pXMotionC[imbY * pWMVDec->m_uintNumMBX + imbX] = IBLOCKMV;
        pWMVDec->m_pYMotionC[imbY * pWMVDec->m_uintNumMBX + imbX] = IBLOCKMV;
    }
    else {  // Inter MB
        I32_WMV  iShiftX = MVx[0];
        I32_WMV  iShiftY = MVy[0];

		I32_WMV iShiftXB = MVxB[0];
		I32_WMV iShiftYB = MVyB[0];

		U8_WMV *ppxliForwardYMB = pWMVDec->m_pInterpY0, *ppxliBackwardYMB = pWMVDec->m_pInterpY1;
		U8_WMV *ppxliForwardUMB = pWMVDec->m_pInterpU0, *ppxliBackwardUMB = pWMVDec->m_pInterpU1;
		U8_WMV *ppxliForwardVMB = pWMVDec->m_pInterpV0, *ppxliBackwardVMB = pWMVDec->m_pInterpV1;
		memset(ppxliForwardYMB, 0, MB_SIZE * MB_SIZE);
		memset(ppxliBackwardYMB, 0, MB_SIZE * MB_SIZE);
		memset(ppxliForwardUMB, 0, BLOCK_SIZE * BLOCK_SIZE);
		memset(ppxliBackwardUMB, 0, BLOCK_SIZE * BLOCK_SIZE);
		memset(ppxliForwardVMB, 0, BLOCK_SIZE * BLOCK_SIZE);
		memset(ppxliBackwardVMB, 0, BLOCK_SIZE * BLOCK_SIZE);
        PullBackFieldModeMotionVector (pWMVDec, &iShiftX, &iShiftY, imbX, imbY);
        InterpolateMB (pWMVDec, pRefY + (pWMVDec->m_iWidthPrevY >> 1) * (iShiftY >> 2) + (iShiftX >> 2),
            pWMVDec->m_iWidthPrevY, ppxliForwardYMB, 16, iShiftX & 3, iShiftY & 3, pWMVDec->m_iFilterType);
        PullBackFieldModeMotionVector (pWMVDec, &iShiftXB, &iShiftYB, imbX, imbY);
        InterpolateMB (pWMVDec, pRef1Y + (pWMVDec->m_iWidthPrevY >> 1) * (iShiftYB >> 2) + (iShiftXB >> 2),
            pWMVDec->m_iWidthPrevY, ppxliBackwardYMB, 16, iShiftXB & 3, iShiftYB & 3, pWMVDec->m_iFilterType);
		pWMVDec->m_pPixelMean (ppxliForwardYMB, 16, ppxliBackwardYMB, 16, 
								pDstY, pWMVDec->m_iWidthPrevY, 16, 16);

        // Chroma channels        
        ChromaMVFieldMode (pWMVDec,MVx, MVy, imbX, imbY, TRUE, &iShiftX, &iShiftY);
		ChromaMVFieldMode (pWMVDec,MVxB, MVyB, imbX, imbY, TRUE, &iShiftXB, &iShiftYB);
        pWMVDec->m_pXMotionC[imbY * pWMVDec->m_uintNumMBX + imbX] = (I16_WMV)iShiftX;
        pWMVDec->m_pYMotionC[imbY * pWMVDec->m_uintNumMBX + imbX] = (I16_WMV)iShiftY;

        InterpolateBlock(pWMVDec,pRefU + pWMVDec->m_iWidthPrevUV * (iShiftY >> 2) + (iShiftX >> 2), 
            pWMVDec->m_iWidthPrevUV, ppxliForwardUMB, 8, iShiftX & 3, iShiftY & 3, FILTER_BILINEAR);
        InterpolateBlock(pWMVDec,pRef1U + pWMVDec->m_iWidthPrevUV * (iShiftYB >> 2) + (iShiftXB >> 2), 
            pWMVDec->m_iWidthPrevUV, ppxliBackwardUMB, 8, iShiftXB & 3, iShiftYB & 3, FILTER_BILINEAR);
        InterpolateBlock(pWMVDec,pRefV + pWMVDec->m_iWidthPrevUV * (iShiftY >> 2) + (iShiftX >> 2), 
            pWMVDec->m_iWidthPrevUV, ppxliForwardVMB, 8, iShiftX & 3, iShiftY & 3, FILTER_BILINEAR);
        InterpolateBlock(pWMVDec,pRef1V + pWMVDec->m_iWidthPrevUV * (iShiftYB >> 2) + (iShiftXB >> 2), 
            pWMVDec->m_iWidthPrevUV, ppxliBackwardVMB, 8, iShiftXB & 3, iShiftYB & 3, FILTER_BILINEAR);
		pWMVDec->m_pPixelMean (ppxliForwardUMB, 8, ppxliBackwardUMB, 8, 
								pDstU, pWMVDec->m_iWidthPrevUV, 8, 8);
		pWMVDec->m_pPixelMean (ppxliForwardVMB, 8, ppxliBackwardVMB, 8, 
								pDstV, pWMVDec->m_iWidthPrevUV, 8, 8);
    }
}

I32_WMV AverageMVFieldMode (tWMVDecInternalMember *pWMVDec,
    I16_WMV* piMVx, I16_WMV* piMVy, 
    I32_WMV imbX, I32_WMV imbY, Bool_WMV b1MV, 
    I32_WMV *piMVx2, I32_WMV *piMVy2)
{
    I32_WMV  iX=0, iY=0;
    I32_WMV iMVx = *piMVx2;
    I32_WMV iMVy = *piMVy2;

    if (b1MV) {
        // 1MV case
        iX = piMVx[0];
        iY = piMVy[0];
    }
    else {
        // 4MV case
        // count I blocks
        I32_WMV  iLMVx[4], iLMVy[4], iType[4], iIntraBlocks = 0, iSameFieldCount = 0, iOppFieldCount, i;

        for (i = 0; i < 4; i++) {
            iLMVx[i] = piMVx[i];
            iLMVy[i] = piMVy[i];
            if (iLMVx[i] == IBLOCKMV) {
                iIntraBlocks ++;
                iType[i] = 1;
            }
            else
            {
                I32_WMV iY = iLMVy[i];
                
                iLMVy[i] = iY;
                iType[i] = (iLMVy[i] & 1);
            }
        }
           
        if (iIntraBlocks > 2) {
            iX = IBLOCKMV;
            iY = IBLOCKMV;
        }
        else {
            // Use the motion vectors from the most prevalent field
            for (i = 0; i < 4; i++) {
                if (iType[i] == 0)
                    iSameFieldCount ++;
            }
            iOppFieldCount = 4 - iSameFieldCount - iIntraBlocks;

            if (iOppFieldCount > iSameFieldCount) {
                // Use opposite field as chroma reference
                for (i = 0; i < 4; i++) {
                    if (iType[i] == 0) {
                        iLMVx[i] = IBLOCKMV;
                        iIntraBlocks ++;
                    }
                }
            }
            else {
                // Use same field as chroma reference
                for (i = 0; i < 4; i++) {
                    if (iType[i] == 4) {
                        iLMVx[i] = IBLOCKMV;
                        iIntraBlocks ++;
                    }
                }
            }

            if (iIntraBlocks == 1) {
                if (iLMVx[0] == IBLOCKMV) {
                    iX = medianof3 (iLMVx[3], iLMVx[1], iLMVx[2]);
                    iY = medianof3 (iLMVy[3], iLMVy[1], iLMVy[2]);
                }
                else if (iLMVx[1] == IBLOCKMV) {
                    iX = medianof3 (iLMVx[3], iLMVx[0], iLMVx[2]);
                    iY = medianof3 (iLMVy[3], iLMVy[0], iLMVy[2]);
                }
                else if (iLMVx[2] == IBLOCKMV) {
                    iX = medianof3 (iLMVx[3], iLMVx[1], iLMVx[0]);
                    iY = medianof3 (iLMVy[3], iLMVy[1], iLMVy[0]);
                }
                else if (iLMVx[3] == IBLOCKMV) {
                    iX = medianof3 (iLMVx[0], iLMVx[1], iLMVx[2]);
                    iY = medianof3 (iLMVy[0], iLMVy[1], iLMVy[2]);
                }
            }
            else if (iIntraBlocks == 2) {
                iX = iY = 0;
                if (iLMVx[0] != IBLOCKMV) {
                    iX += iLMVx[0];
                    iY += iLMVy[0];
                }
                if (iLMVx[1] != IBLOCKMV) {
                    iX += iLMVx[1];
                    iY += iLMVy[1];
                }
                if (iLMVx[2] != IBLOCKMV) {
                    iX += iLMVx[2];
                    iY += iLMVy[2];
                }
                if (iLMVx[3] != IBLOCKMV) {
                    iX += iLMVx[3];
                    iY += iLMVy[3];
                }
                iX /= 2;
                iY /= 2;
            }
            else if (iIntraBlocks == 3) {
                if (iLMVx[0] != IBLOCKMV) {
                    iX = iLMVx[0];
                    iY = iLMVy[0];
                }
                else if (iLMVx[1] != IBLOCKMV) {
                    iX = iLMVx[1];
                    iY = iLMVy[1];
                }
                else if (iLMVx[2] != IBLOCKMV) {
                    iX = iLMVx[2];
                    iY = iLMVy[2];
                }
                else {
                    iX = iLMVx[3];
                    iY = iLMVy[3];
                }
            }
            else {
                iX = medianof4 (iLMVx[0], iLMVx[1], iLMVx[2], iLMVx[3]);
                iY = medianof4 (iLMVy[0], iLMVy[1], iLMVy[2], iLMVy[3]);
            }
        }
    }

	
    iMVx = iX;
    iMVy = iY;

    *piMVx2 = iMVx;
    *piMVy2 = iMVy;



    return (iMVx == IBLOCKMV);
}


Void_WMV PredictMVFieldBPicture (tWMVDecInternalMember *pWMVDec,I32_WMV iX, I32_WMV iY, Bool_WMV b1MV, Bool_WMV bMVPresent,
                                I32_WMV *iPredX, I32_WMV *iPredY, I16_WMV* pXMotion, I16_WMV* pYMotion, I32_WMV iMVPolarity, MBType eDirection, Bool_WMV bHalfPelMode)
{
    //I32_WMV iRow = pWMVDec->m_uintNumMBX * 2;
    //I32_WMV iIndex = iY * iRow + iX;
    I32_WMV iPredSameX, iPredSameY, iPredOppX, iPredOppY;
    I32_WMV iPredPolarity;
    //I32_WMV iSum1 = 0, iSum2 = 0;

#if 0 // This may break B frame coding ... need to check 
    iPredPolarity = PredictFieldModeMVEx (
        &iPredSameX, &iPredSameY, &iPredOppX, &iPredOppY,
        iX, iY,
        pXMotion, pYMotion, b1MV, iY == 0);
#endif
     // This may break B frame coding ... need to check -- CHECKBFRAME
    CMVPred cMVPred;
    if (eDirection == FORWARD) {
    PredictFieldModeForwardBMVEx (pWMVDec, 
        &cMVPred,
        iX, iY,
        pXMotion, pYMotion, b1MV, iY == 0, bHalfPelMode);
    }
    else {  // eDirection == BACKWARD
        PredictFieldModeBackwardBMVEx (pWMVDec, 
            &cMVPred,
            iX, iY,
            pXMotion, pYMotion, b1MV, iY == 0, bHalfPelMode);
    }


    iPredPolarity = cMVPred.iPolarity;
    iPredSameX = cMVPred.iPredSameX1;
    iPredSameY = cMVPred.iPredSameY1;
    iPredOppX = cMVPred.iPredOppX1;
    iPredOppY = cMVPred.iPredOppY1;

    if (bMVPresent && iMVPolarity) {
        // Use non-dominant polarity
        if (iPredPolarity) {
            *iPredX = iPredSameX;
            *iPredY = iPredSameY + 1;
        }
        else {
            *iPredX = iPredOppX;
            *iPredY = iPredOppY + 1;
        }
    }
    else {
        // Use dominant polarity
        if (iPredPolarity) {
            *iPredX = iPredOppX;
            *iPredY = iPredOppY;
        }
        else {
            *iPredX = iPredSameX;
            *iPredY = iPredSameY;
        }
    }
}

I32_WMV decodeBMBOverheadInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, Bool_WMV bCBPPresent, Bool_WMV b1MVPresent)

{
    I32_WMV iCBPCY = 0;
    U32_WMV iBlk;
    I32_WMV iTmpCBPCY;

    memset (pWMVDec->m_pDiffMV, 0, sizeof (CDiffMV) * 4);
	if (pWMVDec->m_tFrmType == BVOP && pmbmd->m_chMBMode != MB_INTRA)
	{
		if (pWMVDec->m_iDirectCodingMode == SKIP_RAW) 
		{
			pmbmd->m_mbType = (MBType)BS_getBits(pWMVDec->m_pbitstrmIn,1);
		}
		if (pmbmd->m_mbType == (MBType)0)       
		{
			I32_WMV iMixed = 0;

          //  if (pWMVDec->m_pDiffMV->iIntra)
          //      pmbmd->m_mbType = BACKWARD;
            if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 0)
                pmbmd->m_mbType = pWMVDec->m_mbtShortBSymbol;
            else if (BS_getBits(pWMVDec->m_pbitstrmIn,1) == 0)
                pmbmd->m_mbType = pWMVDec->m_mbtLongBSymbol;
            else
                pmbmd->m_mbType = INTERPOLATE;

			if (pmbmd->m_chMBMode == MB_FIELD && pmbmd->m_mbType != INTERPOLATE)
				iMixed = BS_getBits(pWMVDec->m_pbitstrmIn,1);

			if (iMixed && pmbmd->m_mbType == FORWARD)
				pmbmd->m_mbType = FORWARD_BACKWARD;
			else if (iMixed && pmbmd->m_mbType == BACKWARD)
				pmbmd->m_mbType = BACKWARD_FORWARD;
        }
	}
    	else if (pmbmd->m_chMBMode == MB_INTRA)
		pmbmd->m_mbType = BACKWARD;


    memset (pmbmd->m_rgbCodedBlockPattern2, 0, 6 * sizeof (U8_WMV));        
	if (pmbmd->m_bSkip)
		return ICERR_OK;

    if (bCBPPresent) {
        iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufPCBPCYDec,pWMVDec->m_pbitstrmIn) + 1;
    } else {
        iCBPCY = 0;
	}

    // set CBP
    iTmpCBPCY = iCBPCY;
    for (iBlk = 0; iBlk < 6; iBlk ++) {
        pmbmd->m_rgbCodedBlockPattern2 [5-iBlk] = (iTmpCBPCY & 0x1);
        iTmpCBPCY >>= 1;
    }
    
    if (pmbmd->m_mbType != DIRECT)
	{

    if (pmbmd->m_chMBMode == MB_1MV && pmbmd->m_mbType != INTERPOLATE) {
        if (b1MVPresent) {
            decodeMV_Progressive(pWMVDec, pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV);
        }
    } else if ((pmbmd->m_chMBMode == MB_FIELD && pmbmd->m_mbType != INTERPOLATE)
		|| (pmbmd->m_chMBMode == MB_1MV && pmbmd->m_mbType == INTERPOLATE)) {
        I32_WMV iMV = Huffman_WMV_get(pWMVDec->m_p2MVBP,pWMVDec->m_pbitstrmIn);
        if (iMV & 2) {
            decodeMV_Progressive(pWMVDec, pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV);
        }
        if (iMV & 1) {
			if (pmbmd->m_chMBMode == MB_FIELD)
				decodeMV_Progressive(pWMVDec, pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV + 2);
			else
				decodeMV_Progressive(pWMVDec, pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV + 1);
        }
    } else {
        I32_WMV i, iMV;
           assert (pmbmd->m_chMBMode == MB_FIELD && pmbmd->m_mbType == INTERPOLATE);
        iMV = Huffman_WMV_get(pWMVDec->m_p4MVBP,pWMVDec->m_pbitstrmIn);        
        for (i = 0; i < 4; i++) { 
            if (iMV & (1 << (3 - i))) {
                decodeMV_Progressive(pWMVDec, pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV + i);
            }
        }          
    }

    }

    if (pWMVDec->m_bDQuantOn && iCBPCY) {
        if (ICERR_OK != decodeDQuantParam (pWMVDec,pmbmd)) {
            return ICERR_ERROR;
        }
    }

    if (pWMVDec->m_bMBXformSwitching && iCBPCY) {
        I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufMBXformTypeDec,pWMVDec->m_pbitstrmIn);
        pmbmd->m_bBlkXformSwitchOn = (iIndex < 8);
        pmbmd->m_iMBXformMode = s_iMBXformTypeLUT[iIndex];
        pmbmd->m_iSubblockPattern = s_iSubblkPatternLUT[iIndex];
        pmbmd->m_bFirstBlockDecoded = FALSE;
    }
    
    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }

    return ICERR_OK;
}

Void_WMV PredictFieldModeForwardBMVEx (tWMVDecInternalMember *pWMVDec,
    CMVPred *pMVPred,
    I32_WMV iBlkX, I32_WMV iBlkY,
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV b1MV, Bool_WMV bTopBndry, Bool_WMV bHalfPelMode)
{
    // find motion predictor
    //I32_WMV iX1 = iBlkX * 8 * 4;
    //I32_WMV iY1 = iBlkY * 16 * 4 + pWMVDec->m_iCurrentField * 4;
    I32_WMV iX, iY;
    I32_WMV iXS = 0, iYS = 0, iXO = 0, iYO = 1 - 2 * pWMVDec->m_iCurrentField;

    I32_WMV iIndex  = iBlkY * pWMVDec->m_uintNumMBX * 2 + iBlkX;  // Index to block
    I32_WMV iMinX = -60;  // -15 pixels
    I32_WMV iMaxX = pWMVDec->m_uintNumMBX * 2 * 8 * 4 - 4;  // x8 for block x4 for qpel
    I32_WMV iSameFieldX [3] = {0}, iSameFieldY [3] = {0};
    I32_WMV iOppFieldX [3] = {0}, iOppFieldY [3] = {0};
    I32_WMV iSameFieldCount = 0, iOppFieldCount = 0, iCount = 0;
    Bool_WMV bValidLeft = FALSE, bValidTop = FALSE, bValidTopRight = FALSE;
    I32_WMV iLeftSameHybridX=0, iLeftSameHybridY=0, iTopSameHybridX=0, iTopSameHybridY=0;
    I32_WMV iLeftOppHybridX=0, iLeftOppHybridY=0, iTopOppHybridX=0, iTopOppHybridY=0;
    Bool_WMV bSameFieldHybridMV = FALSE, bOppFieldHybridMV = FALSE;
    I32_WMV iRes;
    if (bHalfPelMode)
        iRes = 2;
    else
        iRes = 4;

    iMinX = -15 * iRes;  // -15 pixels
    iMaxX = pWMVDec->m_uintNumMBX * 2 * 8 * iRes - iRes;  // x8 for block x4 for qpel


    iOppFieldY [2] = 1 - 2 * pWMVDec->m_iCurrentField;   // In case we only have 2 valid opposite field Y predictors we need to set the 
                                                // 3rd one to a default odd value. Otherwise the median operation could produce an even value.
                                              
    if (!bTopBndry) {
        
        I32_WMV iTopX, iTopY;
        I32_WMV iLeftX, iLeftY;
        I32_WMV iTopRightX, iTopRightY;
        Bool_WMV bIntraTop, bIntraLeft, bIntraTopRight;

        // left block
        if (iBlkX) {
            iLeftX = pFieldMvX [iIndex - 1];
            iLeftY = pFieldMvY [iIndex - 1];
            bValidLeft = TRUE;
        } else {
			// special case - 1MB wide frame
			if (pWMVDec->m_uintNumMBX == 1) {
				iX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX];
				iY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX];
                if (iX != IBLOCKMV) {
                    if (iY & 1) {
                        iXO = iX;
                        iYO = iY;
                        iOppFieldCount ++;
                    }
                    else {
                        iXS = iX;
                        iYS = iY;
                        iSameFieldCount ++;
                    }
                }
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
        iTopX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX];
        iTopY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX];
        bValidTop = TRUE;

        bIntraTop = (iTopX == IBLOCKMV);

        if (bIntraTop) {
            iTopX = iTopY = 0;
        }
        
        // top right block
        if (b1MV) {
            if (iBlkX != (I32_WMV) pWMVDec->m_uintNumMBX * 2 - 2) {
                iTopRightX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX + 2];
                iTopRightY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX + 2];
            } else {
                iTopRightX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX - 2];
                iTopRightY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX - 2];
            }
        }
        else {
            if (iBlkX) {
            I32_WMV iOffset = 1 - ((((iBlkX ^ iBlkY) & 1) == 0) || (iBlkX >= ((I32_WMV)pWMVDec->m_uintNumMBX * 2 - 1))) * 2;
            iTopRightX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX + iOffset];
            iTopRightY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX + iOffset];
            }
            else {
                iTopRightX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX + 1];
                iTopRightY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX + 1];
            }

        }
        bValidTopRight = TRUE;

        bIntraTopRight = (iTopRightX == IBLOCKMV);
        if (bIntraTopRight) {
            iTopRightX = iTopRightY = 0;
        } 

        if (bIntraLeft + bIntraTop + bIntraTopRight != 3) {

            if (pWMVDec->m_iCurrentTemporalField == 0) {
                if (bValidLeft && !bIntraLeft) {
                    if (iLeftY & 1) {
                        iLeftOppHybridX = iOppFieldX [iCount] = iLeftX;
                        iLeftOppHybridY = iOppFieldY [iCount] = iLeftY;
					    ScaleFarPredForward (pWMVDec, iLeftX, iLeftY, &iXS, &iYS, TRUE);
                        iLeftSameHybridX = iSameFieldX [iCount] = iXS;
                        iLeftSameHybridY = iSameFieldY [iCount++] = iYS;
                        iOppFieldCount++;
                    }
                    else {
                        iLeftSameHybridX = iSameFieldX [iCount] = iLeftX;
                        iLeftSameHybridY = iSameFieldY [iCount] = iLeftY;
					    ScaleNearPredForward (pWMVDec, iLeftX, iLeftY, &iXO, &iYO, TRUE);
                        iLeftOppHybridX = iOppFieldX [iCount] = iXO;
                        iLeftOppHybridY = iOppFieldY [iCount++] = iYO;
                        iSameFieldCount++;
                    }
                }
                if (bValidTop && !bIntraTop) {
                    if (iTopY & 1) {
                        iTopOppHybridX = iOppFieldX [iCount] = iTopX;
                        iTopOppHybridY = iOppFieldY [iCount] = iTopY;
					    ScaleFarPredForward (pWMVDec, iTopX, iTopY, &iXS, &iYS, TRUE);
                        iTopSameHybridX = iSameFieldX [iCount] = iXS;
                        iTopSameHybridY = iSameFieldY [iCount++] = iYS;
                        iOppFieldCount++;
                    }
                    else {
                        iTopSameHybridX = iSameFieldX [iCount] = iTopX;
                        iTopSameHybridY = iSameFieldY [iCount] = iTopY;
					    ScaleNearPredForward (pWMVDec, iTopX, iTopY, &iXO, &iYO, TRUE);
                        iTopOppHybridX = iOppFieldX [iCount] = iXO;
                        iTopOppHybridY = iOppFieldY [iCount++] = iYO;
                        iSameFieldCount++;
                    }
                }
                if (bValidTopRight && !bIntraTopRight) {
                    if (iTopRightY & 1) {
                        iOppFieldX [iCount] = iTopRightX;
                        iOppFieldY [iCount] = iTopRightY;
					    ScaleFarPredForward (pWMVDec, iTopRightX, iTopRightY, &iXS, &iYS, TRUE);
                        iSameFieldX [iCount] = iXS;
                        iSameFieldY [iCount++] = iYS;
                        iOppFieldCount++;
                    }
                    else {
                        iSameFieldX [iCount] = iTopRightX;
                        iSameFieldY [iCount] = iTopRightY;
					    ScaleNearPredForward (pWMVDec, iTopRightX, iTopRightY, &iXO, &iYO, TRUE);
                        iOppFieldX [iCount] = iXO;
                        iOppFieldY [iCount++] = iYO;
                        iSameFieldCount++;
                    }
                }
            }
            else { // we are coding second field
                if (bValidLeft && !bIntraLeft) {
                    if (iLeftY & 1) {
                        iLeftOppHybridX = iOppFieldX [iCount] = iLeftX;
                        iLeftOppHybridY = iOppFieldY [iCount] = iLeftY;
					    ScaleNearPredForward (pWMVDec, iLeftX, iLeftY, &iXS, &iYS, FALSE);
                        iLeftSameHybridX = iSameFieldX [iCount] = iXS;
                        iLeftSameHybridY = iSameFieldY [iCount++] = iYS;
                        iOppFieldCount++;
                    }
                    else {
                        iLeftSameHybridX = iSameFieldX [iCount] = iLeftX;
                        iLeftSameHybridY = iSameFieldY [iCount] = iLeftY;
					    ScaleFarPredForward (pWMVDec, iLeftX, iLeftY, &iXO, &iYO, FALSE);
                        iLeftOppHybridX = iOppFieldX [iCount] = iXO;
                        iLeftOppHybridY = iOppFieldY [iCount++] = iYO;
                        iSameFieldCount++;
                    }
                }
                if (bValidTop && !bIntraTop) {
                    if (iTopY & 1) {
                        iTopOppHybridX = iOppFieldX [iCount] = iTopX;
                        iTopOppHybridY = iOppFieldY [iCount] = iTopY;
					    ScaleNearPredForward (pWMVDec, iTopX, iTopY, &iXS, &iYS, FALSE);
                        iTopSameHybridX = iSameFieldX [iCount] = iXS;
                        iTopSameHybridY = iSameFieldY [iCount++] = iYS;
                        iOppFieldCount++;
                    }
                    else {
                        iTopSameHybridX = iSameFieldX [iCount] = iTopX;
                        iTopSameHybridY = iSameFieldY [iCount] = iTopY;
					    ScaleFarPredForward (pWMVDec, iTopX, iTopY, &iXO, &iYO, FALSE);
                        iTopOppHybridX = iOppFieldX [iCount] = iXO;
                        iTopOppHybridY = iOppFieldY [iCount++] = iYO;
                        iSameFieldCount++;
                    }
                }
                if (bValidTopRight && !bIntraTopRight) {
                    if (iTopRightY & 1) {
                        iOppFieldX [iCount] = iTopRightX;
                        iOppFieldY [iCount] = iTopRightY;
					    ScaleNearPredForward (pWMVDec, iTopRightX, iTopRightY, &iXS, &iYS, FALSE);
                        iSameFieldX [iCount] = iXS;
                        iSameFieldY [iCount++] = iYS;
                        iOppFieldCount++;
                    }
                    else {
                        iSameFieldX [iCount] = iTopRightX;
                        iSameFieldY [iCount] = iTopRightY;
					    ScaleFarPredForward (pWMVDec, iTopRightX, iTopRightY, &iXO, &iYO, FALSE);
                        iOppFieldX [iCount] = iXO;
                        iOppFieldY [iCount++] = iYO;
                        iSameFieldCount++;
                    }
                }
            }
            if (iCount > 1) {
                iXS = medianof3 (iSameFieldX [0], iSameFieldX [1], iSameFieldX [2]);
                iYS = medianof3 (iSameFieldY [0], iSameFieldY [1], iSameFieldY [2]);
                iXO = medianof3 (iOppFieldX [0], iOppFieldX [1], iOppFieldX [2]);
                iYO = medianof3 (iOppFieldY [0], iOppFieldY [1], iOppFieldY [2]);
            }
            else if (iCount == 1) {
                iXS = iSameFieldX [0];
                iYS = iSameFieldY [0];
                iXO = iOppFieldX [0];
                iYO = iOppFieldY [0];
            }
            else {
                iXS = 0;
                iYS = 0;
                iXO = 0;
                iYO = 1 - 2 * pWMVDec->m_iCurrentField;
            }
            // Decide whether to use hybrid predictor
            if (pWMVDec->m_tFrmType != BVOP && bValidLeft && !bIntraLeft && bValidTop && !bIntraTop) {
                // Decide for same field
                if ((abs (iXS - iLeftSameHybridX) + abs (iYS - iLeftSameHybridY)) > 32)
                    bSameFieldHybridMV = TRUE;
                else if ((abs (iXS - iTopSameHybridX) + abs (iYS - iTopSameHybridY)) > 32)
                    bSameFieldHybridMV = TRUE;

                // Decide for opposite field
                if ((abs (iXO - iLeftOppHybridX) + abs (iYO - iLeftOppHybridY)) > 32)
                    bOppFieldHybridMV = TRUE;
                else if ((abs (iXO - iTopOppHybridX) + abs (iYO - iTopOppHybridY)) > 32)
                    bOppFieldHybridMV = TRUE;
            }
        }

End:;
    } else if (iBlkX) {
        // left predictor
        iY = pFieldMvY[iIndex - 1];
        iX = pFieldMvX[iIndex - 1];

        if (iX != IBLOCKMV) {
            if (pWMVDec->m_iCurrentTemporalField == 0) {
                // First field
                if (iY & 1) {
                    iXO = iX;
                    iYO = iY;
                    ScaleFarPredForward (pWMVDec, iXO, iYO, &iXS, &iYS, TRUE);
                    iOppFieldCount ++;
                }
                else {
                    iXS = iX;
                    iYS = iY;
                    ScaleNearPredForward (pWMVDec, iXS, iYS, &iXO, &iYO, TRUE);
                    iSameFieldCount ++;
                }
            }
            else {
                                // Seccond field
                if (iY & 1) {
                    iXO = iX;
                    iYO = iY;
                    ScaleNearPredForward (pWMVDec, iXO, iYO, &iXS, &iYS, FALSE);
                    iOppFieldCount ++;
                }
                else {
                    iXS = iX;
                    iYS = iY;
                    ScaleFarPredForward (pWMVDec, iXS, iYS, &iXO, &iYO, FALSE);
                    iSameFieldCount ++;
                }

            }
        }
    } 

    pMVPred->bSameFieldHybridMV = bSameFieldHybridMV;
    pMVPred->bOppFieldHybridMV = bOppFieldHybridMV;
    if (bSameFieldHybridMV) {
        pMVPred->iPredSameX1 = iLeftSameHybridX;
        pMVPred->iPredSameY1 = iLeftSameHybridY;
        pMVPred->iPredSameX2 = iTopSameHybridX;
        pMVPred->iPredSameY2 = iTopSameHybridY;
    }
    else {
        pMVPred->iPredSameX1 = iXS;
        pMVPred->iPredSameY1 = iYS;
    }
    if (bOppFieldHybridMV) {
        pMVPred->iPredOppX1 = iLeftOppHybridX;
        pMVPred->iPredOppY1 = iLeftOppHybridY;
        pMVPred->iPredOppX2 = iTopOppHybridX;
        pMVPred->iPredOppY2 = iTopOppHybridY;
    }
    else {
        pMVPred->iPredOppX1 = iXO;
        pMVPred->iPredOppY1 = iYO;
    }
    if (pWMVDec->m_bTwoRefPictures) {
        if (iSameFieldCount > iOppFieldCount)
            pMVPred->iPolarity = 0;
        else
            pMVPred->iPolarity = 1;
        }
    else {
        if (pWMVDec->m_bUseOppFieldForRef)
            pMVPred->iPolarity = 1;
        else
            pMVPred->iPolarity = 0;
    }
}

Void_WMV PredictFieldModeBackwardBMVEx (tWMVDecInternalMember *pWMVDec,
    CMVPred *pMVPred,
    I32_WMV iBlkX, I32_WMV iBlkY,
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV b1MV, Bool_WMV bTopBndry, Bool_WMV bHalfPelMode)
{
    // find motion predictor
    //I32_WMV iX1 = iBlkX * 8 * 4;
    //I32_WMV iY1 = iBlkY * 16 * 4 + pWMVDec->m_iCurrentField * 4;
    I32_WMV iX, iY;
    I32_WMV iXS = 0, iYS = 0, iXO = 0, iYO = 1 - 2 * pWMVDec->m_iCurrentField;

    I32_WMV iIndex  = iBlkY * pWMVDec->m_uintNumMBX * 2 + iBlkX;  // Index to block
    I32_WMV iMinX = -60;  // -15 pixels
    I32_WMV iMaxX = pWMVDec->m_uintNumMBX * 2 * 8 * 4 - 4;  // x8 for block x4 for qpel
    I32_WMV iSameFieldX [3] = {0}, iSameFieldY [3] = {0};
    I32_WMV iOppFieldX [3] = {0}, iOppFieldY [3] = {0};
    I32_WMV iSameFieldCount = 0, iOppFieldCount = 0, iCount = 0;
    Bool_WMV bValidLeft = FALSE, bValidTop = FALSE, bValidTopRight = FALSE;
    I32_WMV iLeftSameHybridX =0, iLeftSameHybridY=0, iTopSameHybridX=0, iTopSameHybridY=0;
    I32_WMV iLeftOppHybridX=0, iLeftOppHybridY=0, iTopOppHybridX=0, iTopOppHybridY=0;
    Bool_WMV bSameFieldHybridMV = FALSE, bOppFieldHybridMV = FALSE;
    I32_WMV iRes;
    if (bHalfPelMode)
        iRes = 2;
    else
        iRes = 4;

    iMinX = -15 * iRes;  // -15 pixels
    iMaxX = pWMVDec->m_uintNumMBX * 2 * 8 * iRes - iRes;  // x8 for block x4 for qpel


    iOppFieldY [2] = 1 - 2 * pWMVDec->m_iCurrentField;   // In case we only have 2 valid opposite field Y predictors we need to set the 
                                                // 3rd one to a default odd value. Otherwise the median operation could produce an even value.
                                              
    if (!bTopBndry) {
        
        I32_WMV iTopX, iTopY;
        I32_WMV iLeftX, iLeftY;
        I32_WMV iTopRightX, iTopRightY;
        Bool_WMV bIntraTop, bIntraLeft, bIntraTopRight;

        // left block
        if (iBlkX) {
            iLeftX = pFieldMvX [iIndex - 1];
            iLeftY = pFieldMvY [iIndex - 1];
            bValidLeft = TRUE;
        } else {
			// special case - 1MB wide frame
			if (pWMVDec->m_uintNumMBX == 1) {
				iX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX];
				iY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX];
                if (iX != IBLOCKMV) {
                    if (iY & 1) {
                        iXO = iX;
                        iYO = iY;
                        iOppFieldCount ++;
                    }
                    else {
                        iXS = iX;
                        iYS = iY;
                        iSameFieldCount ++;
                    }
                }
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
        iTopX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX];
        iTopY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX];
        bValidTop = TRUE;

        bIntraTop = (iTopX == IBLOCKMV);

        if (bIntraTop) {
            iTopX = iTopY = 0;
        }
        
        // top right block
        if (b1MV) {
            if (iBlkX != (I32_WMV) pWMVDec->m_uintNumMBX * 2 - 2) {
                iTopRightX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX + 2];
                iTopRightY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX + 2];
            } else {
                iTopRightX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX - 2];
                iTopRightY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX - 2];
            }
        }
        else {
            if (iBlkX) {
            I32_WMV iOffset = 1 - ((((iBlkX ^ iBlkY) & 1) == 0) || (iBlkX >= ((I32_WMV)pWMVDec->m_uintNumMBX * 2 - 1))) * 2;
            iTopRightX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX + iOffset];
            iTopRightY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX + iOffset];
            }
            else {
                iTopRightX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX + 1];
                iTopRightY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX + 1];
            }

        }
        bValidTopRight = TRUE;

        bIntraTopRight = (iTopRightX == IBLOCKMV);
        if (bIntraTopRight) {
            iTopRightX = iTopRightY = 0;
        } 

        if (bIntraLeft + bIntraTop + bIntraTopRight != 3) {

            if (pWMVDec->m_iCurrentTemporalField == 0) {
                if (bValidLeft && !bIntraLeft) {
                    if (iLeftY & 1) {
                        iLeftOppHybridX = iOppFieldX [iCount] = iLeftX;
                        iLeftOppHybridY = iOppFieldY [iCount] = iLeftY;
					    ScaleFarPredBackward (pWMVDec, iLeftX, iLeftY, &iXS, &iYS, TRUE);
                        iLeftSameHybridX = iSameFieldX [iCount] = iXS;
                        iLeftSameHybridY = iSameFieldY [iCount++] = iYS;
                        iOppFieldCount++;
                    }
                    else {
                        iLeftSameHybridX = iSameFieldX [iCount] = iLeftX;
                        iLeftSameHybridY = iSameFieldY [iCount] = iLeftY;
					    ScaleNearPredBackward (pWMVDec, iLeftX, iLeftY, &iXO, &iYO, TRUE);
                        iLeftOppHybridX = iOppFieldX [iCount] = iXO;
                        iLeftOppHybridY = iOppFieldY [iCount++] = iYO;
                        iSameFieldCount++;
                    }
                }
                if (bValidTop && !bIntraTop) {
                    if (iTopY & 1) {
                        iTopOppHybridX = iOppFieldX [iCount] = iTopX;
                        iTopOppHybridY = iOppFieldY [iCount] = iTopY;
					    ScaleFarPredBackward (pWMVDec, iTopX, iTopY, &iXS, &iYS, TRUE);
                        iTopSameHybridX = iSameFieldX [iCount] = iXS;
                        iTopSameHybridY = iSameFieldY [iCount++] = iYS;
                        iOppFieldCount++;
                    }
                    else {
                        iTopSameHybridX = iSameFieldX [iCount] = iTopX;
                        iTopSameHybridY = iSameFieldY [iCount] = iTopY;
					    ScaleNearPredBackward (pWMVDec, iTopX, iTopY, &iXO, &iYO, TRUE);
                        iTopOppHybridX = iOppFieldX [iCount] = iXO;
                        iTopOppHybridY = iOppFieldY [iCount++] = iYO;
                        iSameFieldCount++;
                    }
                }
                if (bValidTopRight && !bIntraTopRight) {
                    if (iTopRightY & 1) {
                        iOppFieldX [iCount] = iTopRightX;
                        iOppFieldY [iCount] = iTopRightY;
					    ScaleFarPredBackward (pWMVDec, iTopRightX, iTopRightY, &iXS, &iYS, TRUE);
                        iSameFieldX [iCount] = iXS;
                        iSameFieldY [iCount++] = iYS;
                        iOppFieldCount++;
                    }
                    else {
                        iSameFieldX [iCount] = iTopRightX;
                        iSameFieldY [iCount] = iTopRightY;
					    ScaleNearPredBackward (pWMVDec, iTopRightX, iTopRightY, &iXO, &iYO, TRUE);
                        iOppFieldX [iCount] = iXO;
                        iOppFieldY [iCount++] = iYO;
                        iSameFieldCount++;
                    }
                }
            }
            else { // we are coding second field
                if (bValidLeft && !bIntraLeft) {
                    if (iLeftY & 1) {
                        iLeftOppHybridX = iOppFieldX [iCount] = iLeftX;
                        iLeftOppHybridY = iOppFieldY [iCount] = iLeftY;
					    ScaleNearPredBackward (pWMVDec, iLeftX, iLeftY, &iXS, &iYS, FALSE);
                        iLeftSameHybridX = iSameFieldX [iCount] = iXS;
                        iLeftSameHybridY = iSameFieldY [iCount++] = iYS;
                        iOppFieldCount++;
                    }
                    else {
                        iLeftSameHybridX = iSameFieldX [iCount] = iLeftX;
                        iLeftSameHybridY = iSameFieldY [iCount] = iLeftY;
					    ScaleFarPredBackward (pWMVDec, iLeftX, iLeftY, &iXO, &iYO, FALSE);
                        iLeftOppHybridX = iOppFieldX [iCount] = iXO;
                        iLeftOppHybridY = iOppFieldY [iCount++] = iYO;
                        iSameFieldCount++;
                    }
                }
                if (bValidTop && !bIntraTop) {
                    if (iTopY & 1) {
                        iTopOppHybridX = iOppFieldX [iCount] = iTopX;
                        iTopOppHybridY = iOppFieldY [iCount] = iTopY;
					    ScaleNearPredBackward (pWMVDec, iTopX, iTopY, &iXS, &iYS, FALSE);
                        iTopSameHybridX = iSameFieldX [iCount] = iXS;
                        iTopSameHybridY = iSameFieldY [iCount++] = iYS;
                        iOppFieldCount++;
                    }
                    else {
                        iTopSameHybridX = iSameFieldX [iCount] = iTopX;
                        iTopSameHybridY = iSameFieldY [iCount] = iTopY;
					    ScaleFarPredBackward (pWMVDec, iTopX, iTopY, &iXO, &iYO, FALSE);
                        iTopOppHybridX = iOppFieldX [iCount] = iXO;
                        iTopOppHybridY = iOppFieldY [iCount++] = iYO;
                        iSameFieldCount++;
                    }
                }
                if (bValidTopRight && !bIntraTopRight) {
                    if (iTopRightY & 1) {
                        iOppFieldX [iCount] = iTopRightX;
                        iOppFieldY [iCount] = iTopRightY;
					    ScaleNearPredBackward (pWMVDec, iTopRightX, iTopRightY, &iXS, &iYS, FALSE);
                        iSameFieldX [iCount] = iXS;
                        iSameFieldY [iCount++] = iYS;
                        iOppFieldCount++;
                    }
                    else {
                        iSameFieldX [iCount] = iTopRightX;
                        iSameFieldY [iCount] = iTopRightY;
					    ScaleFarPredBackward (pWMVDec, iTopRightX, iTopRightY, &iXO, &iYO, FALSE);
                        iOppFieldX [iCount] = iXO;
                        iOppFieldY [iCount++] = iYO;
                        iSameFieldCount++;
                    }
                }
            }
            if (iCount > 1) {
                iXS = medianof3 (iSameFieldX [0], iSameFieldX [1], iSameFieldX [2]);
                iYS = medianof3 (iSameFieldY [0], iSameFieldY [1], iSameFieldY [2]);
                iXO = medianof3 (iOppFieldX [0], iOppFieldX [1], iOppFieldX [2]);
                iYO = medianof3 (iOppFieldY [0], iOppFieldY [1], iOppFieldY [2]);
            }
            else if (iCount == 1) {
                iXS = iSameFieldX [0];
                iYS = iSameFieldY [0];
                iXO = iOppFieldX [0];
                iYO = iOppFieldY [0];
            }
            else {
                iXS = 0;
                iYS = 0;
                iXO = 0;
                iYO = 1 - 2 * pWMVDec->m_iCurrentField;
            }
            // Decide whether to use hybrid predictor
            if (pWMVDec->m_tFrmType != BVOP && bValidLeft && !bIntraLeft && bValidTop && !bIntraTop) {
                // Decide for same field
                if ((abs (iXS - iLeftSameHybridX) + abs (iYS - iLeftSameHybridY)) > 32)
                    bSameFieldHybridMV = TRUE;
                else if ((abs (iXS - iTopSameHybridX) + abs (iYS - iTopSameHybridY)) > 32)
                    bSameFieldHybridMV = TRUE;

                // Decide for opposite field
                if ((abs (iXO - iLeftOppHybridX) + abs (iYO - iLeftOppHybridY)) > 32)
                    bOppFieldHybridMV = TRUE;
                else if ((abs (iXO - iTopOppHybridX) + abs (iYO - iTopOppHybridY)) > 32)
                    bOppFieldHybridMV = TRUE;
            }
        }

End:;
    } else if (iBlkX) {
        // left predictor
        iY = pFieldMvY[iIndex - 1];
        iX = pFieldMvX[iIndex - 1];

        if (iX != IBLOCKMV) {
             if (pWMVDec->m_iCurrentTemporalField == 0) {
                if (iY & 1) {
                    iXO = iX;
                    iYO = iY;
                    ScaleFarPredBackward (pWMVDec, iXO, iYO, &iXS, &iYS, TRUE);
                    iOppFieldCount ++;
                }
                else {
                    iXS = iX;
                    iYS = iY;
                    ScaleNearPredBackward (pWMVDec, iXS, iYS, &iXO, &iYO, TRUE);
                    iSameFieldCount ++;
                }

            }
            else {
                if (iY & 1) {
                    iXO = iX;
                    iYO = iY;
                    ScaleNearPredBackward (pWMVDec, iXO, iYO, &iXS, &iYS, FALSE);
                    iOppFieldCount ++;
                }
                else {
                    iXS = iX;
                    iYS = iY;
                    ScaleFarPredBackward (pWMVDec, iXS, iYS, &iXO, &iYO, FALSE);
                    iSameFieldCount ++;
                }

            }
        }
    } 

    pMVPred->bSameFieldHybridMV = bSameFieldHybridMV;
    pMVPred->bOppFieldHybridMV = bOppFieldHybridMV;
    if (bSameFieldHybridMV) {
        pMVPred->iPredSameX1 = iLeftSameHybridX;
        pMVPred->iPredSameY1 = iLeftSameHybridY;
        pMVPred->iPredSameX2 = iTopSameHybridX;
        pMVPred->iPredSameY2 = iTopSameHybridY;
    }
    else {
        pMVPred->iPredSameX1 = iXS;
        pMVPred->iPredSameY1 = iYS;
    }
    if (bOppFieldHybridMV) {
        pMVPred->iPredOppX1 = iLeftOppHybridX;
        pMVPred->iPredOppY1 = iLeftOppHybridY;
        pMVPred->iPredOppX2 = iTopOppHybridX;
        pMVPred->iPredOppY2 = iTopOppHybridY;
    }
    else {
        pMVPred->iPredOppX1 = iXO;
        pMVPred->iPredOppY1 = iYO;
    }
    if (pWMVDec->m_bTwoRefPictures) {
        if (iSameFieldCount > iOppFieldCount)
            pMVPred->iPolarity = 0;
        else
            pMVPred->iPolarity = 1;
        }
    else {
        if (pWMVDec->m_bUseOppFieldForRef)
            pMVPred->iPolarity = 1;
        else
            pMVPred->iPolarity = 0;
    }
}
#endif //#ifdef _WMV9AP_

#endif //PPCWMP
