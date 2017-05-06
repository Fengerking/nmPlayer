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

//static I32_WMV s_iBlkXformTypeLUT[8] = {
//    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
//    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
//};

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
		return;//assert(0);
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
		return;//assert(0);
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

enum {FRAMEMV = 0, FRAMEMV_FIELD, FRAME4MV, FRAME4MV_FIELD, 
      FIELDMV, FIELDMV_FRAME, FIELD4MV, FIELD4MV_FRAME,
      INTRA_FRAME, INTRA_FIELD};


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
//20130118
	if(pWMVDec->bUseRef0Process == 1 && pWMVDec->CpuNumber > 1)
	{
		ppxliRef0Y = pWMVDec->m_pfrmRef0Process->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
		ppxliRef0U = pWMVDec->m_pfrmRef0Process->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
		ppxliRef0V = pWMVDec->m_pfrmRef0Process->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
		pWMVDec->m_ppxliRef0YPlusExp = ppxliRef0Y;
		pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Process->m_pucUPlane;
		pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Process->m_pucVPlane;
	 }


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

        //if(pWMVDec->m_bCodecIsWVC1){
		//	pWMVDec->m_nrows  = imbY;
		//}

        if (pWMVDec->m_bSliceWMVA) {
           // if (pWMVDec->m_uiNumSlicesWMVA > 1 && pWMVDec->m_uiCurrSlice < pWMVDec->m_uiNumSlicesWMVA - 1) {
                if (pWMVDec->m_pbStartOfSliceRow[imbY]) {
                    pWMVDec->m_uiCurrSlice++;
                    if(BS_flush(pWMVDec->m_pbitstrmIn))
                        return WMV_Failed;
                    result = ReadSliceHeaderWMVA (pWMVDec, imbY);
                    pWMVDec->m_bFirstEscCodeInFrame = TRUE;
					if (result != ICERR_OK)
                        return WMV_Failed;
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

            //if(pWMVDec->m_bCodecIsWVC1){
			//    pWMVDec->m_ncols = imbX;
           // }

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
                    //assert (0); // should never get here!!
					return WMV_Failed;
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
                    //assert (0); // should never get here!!
					return WMV_Failed;
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
			if(0){
				if(pWMVDec->m_nBframes ==12)
				{
					FILE*fp=fopen("E:/MyResource/Video/clips/wmv test files/interfiled/vo_dump.txt","a");
					int i,j;
					//fprintf(fp,"-----------------------%d %d -----------\n",imbY,imbX);
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

    if (pWMVDec->m_bLoopFilter) {
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
				//fprintf(fp,"-----------------------%d %d -----------\n",imbY,imbX);
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

   // pWMVDec->m_bRenderFromPostBuffer = bFrameInPostBuf;
    return WMV_Succeeded;
}

//I32_WMV DecodeBMBInterlaceV2 (tWMVDecInternalMember *pWMVDec,I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd)
//{
//    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];
//    U8_WMV *rgCBP = pmbmd->m_rgbCodedBlockPattern2;
//    U32_WMV iBlk;
//    Bool_WMV bFirstBlock = TRUE && pWMVDec->m_bMBXformSwitching;
//        
//    for (iBlk = 0; iBlk < 6; iBlk++) {
//        I32_WMV result = DecodePBlockInterlaceV2 (pWMVDec,
//            imbX, imbY, iBlk, pmbmd, pWMVDec->m_ppInterDCTTableInfo_Dec, pDQ,
//            &bFirstBlock);
//        if (ICERR_OK != result) {
//            return result;
//        }
//    }
//
//    return ICERR_OK;
//}



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
			//assert (pmbmd->m_chMBMode == MB_FIELD && pmbmd->m_mbType == INTERPOLATE);
			if(pmbmd->m_chMBMode != MB_FIELD || pmbmd->m_mbType != INTERPOLATE)
				return ICERR_ERROR;
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