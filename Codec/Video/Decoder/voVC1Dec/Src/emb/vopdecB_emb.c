//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************

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

#define TRACEVC1FILE "VC1_DEC_LOG.txt"

extern U32_WMV DCT_COEF1[6];
extern U32_WMV DCT_COEF2[2];


tWMVDecodeStatus DecodeBMB_EMB_Intra_Parser(EMB_BMainLoop  * pBMainLoop,
                                                        CWMVMBMode* pmbmd, 
                                                        CWMVMBMode* pmbmd_lastrow,
                                                        I32_WMV iMBX, I32_WMV iMBY,EMB_PBMainLoop* pPBB );
tWMVDecodeStatus DecodeBMB_EMB_Fast_Parser(EMB_BMainLoop  * pBMainLoop,
                                                           CWMVMBMode*            pmbmd, 
                                                           I32_WMV iMBX, I32_WMV iMBY,EMB_PBMainLoop *pPBB );
tWMVDecodeStatus DecodeBMB_EMB_Fast_MC(EMB_BMainLoop* pBMainLoop,
                                                            CWMVMBMode* pmbmd, 
                                                            I32_WMV iMBX, I32_WMV iMBY,EMB_PBMainLoop *pPBB);
tWMVDecodeStatus DecodeBMB_EMB_Fast_MC_new(EMB_BMainLoop* pBMainLoop,
                                                            CWMVMBMode* pmbmd, 
                                                            I32_WMV iMBX, I32_WMV iMBY,EMB_PBMainLoop *pPBB );
tWMVDecodeStatus DecodeBMB_EMB_Intra_MC(EMB_BMainLoop  * pBMainLoop,
                                                        CWMVMBMode* pmbmd, 
                                                        I32_WMV iMBX, I32_WMV iMBY,EMB_PBMainLoop *pPBB);
tWMVDecodeStatus DecodeBMB_EMB_Intra_MC_new(EMB_BMainLoop  * pBMainLoop,
                                                        CWMVMBMode* pmbmd, 
                                                        I32_WMV iMBX, I32_WMV iMBY,EMB_PBMainLoop *pPBB);

//void BackUpIDCTCoeff_BVOP(EMB_BMainLoop * pBMainLoop, I16_WMV *idct_error,int iMBX,int iMBY,int iblk,EMB_PBMainLoop *pPB)
//{
//    int i,j;
//    VODCTCOEFF *coeffbuf = (pPB->m_DctCoeff_Bak + iMBX);// + iMBY*pBMainLoop->PB.m_uintNumMBX);
//    I16_WMV * dst = coeffbuf->coeff;
//    I16_WMV * src = idct_error;
//    dst = dst +iblk*64;
//    for(i=0;i<8;i++)
//        memcpy(dst+i*8 ,src+i*8,8*sizeof(I16_WMV));
//}

//void BackUpDiffMV_BVOP(EMB_BMainLoop * pBMainLoop,int iMBX,int iMBY,EMB_PBMainLoop *pPB)
//{
//    int iblk=0;
//    CDiffMV_EMB * CurMBDiff = pPB->m_pDiffMV_EMB_Bak + iMBX*6;// + iMBY*pBMainLoop->PB.m_uintNumMBX*6;
//
//    for(iblk=0;iblk<6;iblk++)
//        memcpy(CurMBDiff+iblk,pPB->m_pDiffMV_EMB + iblk,sizeof(CDiffMV_EMB));
//}

void GetBackUpDiffMV_BVOP(EMB_BMainLoop * pBMainLoop,int iMBX,int iMBY,EMB_PBMainLoop *pPB)
{
    int iblk=0;
    CDiffMV_EMB * CurMBDiff = pPB->m_pDiffMV_EMB_Bak + iMBX*6;// + iMBY*pBMainLoop->PB.m_uintNumMBX*6;

    for(iblk=0;iblk<6;iblk++)
        pPB->pDiffMV_EMB_Bak[iblk] = *(CurMBDiff+iblk);
}

#ifdef WIN32
void voDumpYuv_B(tWMVDecInternalMember *pWMVDec,int nframes)
{
	unsigned char* Yplane = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
	unsigned char* Uplane = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	unsigned char* Vplane = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    char filename[250];

    sprintf(filename,"E:/MyResource/Video/clips/VC1/new/B_%d.yuv",nframes);


	if(pWMVDec->m_nBframes == nframes && pWMVDec->m_tFrmType == BVOP)
	{
		U32_WMV mb_cols =0, mb_rows=0;
		for(mb_rows=0;mb_rows<pWMVDec->m_uintNumMBY;mb_rows++)
		{
			unsigned char* Y = Yplane + mb_rows* pWMVDec->m_iMBSizeXWidthPrevY ;
			unsigned char *U = Uplane + mb_rows* pWMVDec->m_iBlkSizeXWidthPrevUV ;
			unsigned char *V = Vplane + mb_rows* pWMVDec->m_iBlkSizeXWidthPrevUV ;

			for(mb_cols=0;mb_cols<pWMVDec->m_uintNumMBX;mb_cols++)
			{
				FILE*fp=fopen(filename,"a");
				int i,j;
				fprintf(fp,"-----------------------%d %d -----------\n",mb_rows,mb_cols);
				for(i=0;i<16;i++)
				{
					for(j=0;j<16;j++)
					{
						fprintf(fp,"%x   ",Y[i*pWMVDec->m_iWidthInternal+j]);
					}
					fprintf(fp,"\n");
				}

				for(i=0;i<8;i++)
				{
					for(j=0;j<8;j++)
					{
						fprintf(fp,"%x   ",U[i*pWMVDec->m_iWidthInternalUV+j]);
					}
					fprintf(fp,"\n");
				}

				for(i=0;i<8;i++)
				{
					for(j=0;j<8;j++)
					{
						fprintf(fp,"%x   ",V[i*pWMVDec->m_iWidthInternalUV+j]);
					}
					fprintf(fp,"\n");
				}

				Y +=MB_SIZE;
				U +=BLOCK_SIZE;
				V +=BLOCK_SIZE;
				fclose(fp);
			}
		}
	}
}
#endif

FORCEINLINE Void_WMV  setCodedBlockPattern_EMB (CWMVMBMode* pmbmd, BlockNum blkn, Bool_WMV bisCoded) 
{
    pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) blkn - 1] =(U8_WMV) bisCoded;
}

tWMVDecodeStatus ProcessMBRow_Parser_BVOP(tWMVDecInternalMember *pWMVDec,
                                                            CWMVMBMode* pmbmd,
                                                            CWMVMBMode* pmbmd_lastrow,
                                                            I32_WMV imbY,I32_WMV y,
                                                            I32_WMV *bIntraFlag, I32_WMV *b1MVFlag,EMB_PBMainLoop *pPBB)
{
    CoordI x;
    I32_WMV imbX, iMB=imbY*pWMVDec->m_uintNumMBX;
    CMotionVector_X9_EMB* pmv1 = pWMVDec->m_rgmv1_EMB + imbY*pWMVDec->m_uintNumMBX ;   
    tWMVDecodeStatus result = WMV_Succeeded;
    EMB_BMainLoop * pBMainLoop = &pWMVDec->m_EMB_BMainLoop ;
    //EMB_PBMainLoop * pMainLoop = pPBB;//(EMB_PBMainLoop *)pBMainLoop ;  //1442 

    //memset(pPBB->m_pMbMode,0,sizeof(CWMVMBModeBack)*pWMVDec->m_uintNumMBX);

    if (pWMVDec->m_bSliceWMVA) 
    {
        if ( pWMVDec->m_pbStartOfSliceRow[imbY])
        {
            pWMVDec->m_uiCurrSlice++;
            if(BS_flush(pWMVDec->m_pbitstrmIn))
                return WMV_Failed;
            result = ReadSliceHeaderWMVA (pWMVDec, imbY);
            pWMVDec->m_bFirstEscCodeInFrame = TRUE;
            if (result != ICERR_OK)
                return WMV_Failed;
         }
    }

    for (x = 0, imbX = 0; imbX < (I32_WMV) pWMVDec->m_uintNumMBX; imbX++, x += MB_SIZE, iMB++)
    { 
        pPBB->m_rgiCurrBaseBK[1] = imbY*pWMVDec->m_uintNumMBX+imbX; 
        pPBB->m_pDiffMV_EMB =  pPBB->m_pDiffMV_EMB_Bak + imbX*6;

        if (pWMVDec->m_iPrevIFrame == 1)
            pmv1->m_vctTrueHalfPel.I32 = 0;
        
        pmbmd->m_bCBPAllZero = FALSE;
        pWMVDec->m_ncols = imbX;            
        // if skip set diff MV to zero
        memset (pPBB->m_pDiffMV_EMB, 0, 6 * sizeof(CDiffMV_EMB)); //zou mv
        
       if(pWMVDec->m_frameNum == 252 && imbY ==0 && imbX == 1 )
             imbX = imbX;

        result = decodeMBOverheadOfBVOP_WMV3_EMB (pBMainLoop, pmbmd, imbX, imbY,pPBB);            
        if (WMV_Succeeded != result)                 
            return result;

        pmbmd->m_chMBMode = MB_1MV;  

        bIntraFlag[imbX] = pPBB->m_pDiffMV_EMB->iIntra;

        if (pPBB->m_pDiffMV_EMB->iIntra == 0)
        {
            UMotion_EMB defPred[2];
            if (pmbmd->m_bCBPAllZero == TRUE)
                memset(pmbmd->m_rgcBlockXformMode, XFORMMODE_8x8, 6);

            { //0816 
                I32_WMV idfx, idfy, idbx, idby;
                DirectModeMV_Prog_BVOP (pWMVDec, pmbmd,
                                (I32_WMV)pmv1->m_vctTrueHalfPel.I16[X_INDEX], 
                                (I32_WMV)pmv1->m_vctTrueHalfPel.I16[Y_INDEX],
                                (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR),
                                imbX, imbY, &idfx, &idfy, &idbx, &idby);

                defPred[0].I16[X_INDEX] = idfx;
                defPred[0].I16[Y_INDEX] = idfy; 
                defPred[1].I16[X_INDEX] = idbx;
                defPred[1].I16[Y_INDEX] = idby;
            }             
            
            if (pmbmd->m_mbType != DIRECT)
            {
                I32_WMV iMask = FORWARD + 1 - pmbmd->m_mbType;
                I32_WMV i, k;
                UMotion_EMB pred[2];

                if(pmbmd->m_mbType != INTERPOLATE)
                {
                    pred[0].I32 = pPBB->m_pDiffMV_EMB[0].Diff.I32;
                    pred[1].I32 = pPBB->m_pDiffMV_EMB[1].Diff.I32;
                }
                else
                {
                    pred[1].I32 = pPBB->m_pDiffMV_EMB[0].Diff.I32;
                    pred[0].I32 = pPBB->m_pDiffMV_EMB[1].Diff.I32;
                }

                for(i = 0, k=0; i < 2; i++, iMask >>=1)
                {
                    if(!(iMask & 1)) {
                        pBMainLoop->m_pMotionArray[i][iMB].I32 = defPred[i].I32;
                        continue;
                    }
                    pBMainLoop->m_pMotionCurr = pBMainLoop->m_pMotionArray[i];

                    (*pBMainLoop->m_ppPredictMV_EMB[0]) (pBMainLoop, pPBB->m_iBlkMBBase[X_INDEX] ,  TRUE_WMV, 0,pPBB,imbX, imbY);  

                    if (!pmbmd->m_bSkip) {
                        pPBB->m_pDiffMV_EMB[i].Diff.I16[X_INDEX] = ((pPBB->m_iPred.I16[X_INDEX] + pred[k].I16[X_INDEX] + pPBB->m_iXMVRange) & pPBB->m_iXMVFlag) - pPBB->m_iXMVRange;
                        pPBB->m_pDiffMV_EMB[i].Diff.I16[Y_INDEX] = ((pPBB->m_iPred.I16[Y_INDEX] + pred[k].I16[Y_INDEX] + pPBB->m_iYMVRange) & pPBB->m_iYMVFlag) - pPBB->m_iYMVRange;
                    }
                    else {
                        pPBB->m_pDiffMV_EMB[i].Diff.I32 = pPBB->m_iPred.I32;
                    }
                    pBMainLoop->m_pMotionArray[i][iMB].I32 = pPBB->m_pDiffMV_EMB[i].Diff.I32;
                    //
                    if(!pPBB->m_pWMVDec->m_bCodecIsWVC1)
                        pPBB->m_pDiffMV_EMB[i].Diff.I32 = PredictBMVPullBackCheck_EMB(pBMainLoop, 0, pPBB->m_pDiffMV_EMB[i].Diff.I32,pPBB);
                    k++;
                }
            }
            else
            {                   
                pBMainLoop->m_pMotionArray[0][iMB].I32 = defPred[0].I32;
                pBMainLoop->m_pMotionArray[1][iMB].I32 = defPred[1].I32;

                if(pPBB->m_pWMVDec->m_bCodecIsWVC1)
                {
                    pPBB->m_pDiffMV_EMB[0].Diff.I32 = defPred[0].I32;
                    pPBB->m_pDiffMV_EMB[1].Diff.I32 = defPred[1].I32;
                }
                else
                {
                    pPBB->m_pDiffMV_EMB[0].Diff.I32 = PredictBMVPullBackCheck_EMB(pBMainLoop, 0, defPred[0].I32,pPBB);
                    pPBB->m_pDiffMV_EMB[1].Diff.I32 = PredictBMVPullBackCheck_EMB(pBMainLoop, 0, defPred[1].I32,pPBB);
                }
            }
            result = DecodeBMB_EMB_Fast_Parser(pBMainLoop,  pmbmd,  imbX, imbY,pPBB);  
            if (WMV_Succeeded != result)
                return result;
        }
        else {      // INTRA
             pPBB->m_pMotion[iMB].I32 = IBLOCKMV;
             pPBB->m_pMotionC[iMB].I32 = IBLOCKMV;  
             result = DecodeBMB_EMB_Intra_Parser(pBMainLoop, pmbmd, pmbmd_lastrow,imbX, imbY, pPBB );
             if (WMV_Succeeded != result)
                 return result; 
        }

        if(pWMVDec->m_bLoopFilter) 
        {
            ComputeLoopFilterFlags_EMB_MB(pWMVDec, pmbmd,pmbmd_lastrow,imbY,imbX);
        }

        //BackUpDiffMV_BVOP(pBMainLoop,imbX, imbY,pPBB);    

        pPBB->m_rgiCurrBaseBK[0] += 2;
        //pPBB->m_rgiCurrBaseBK[1] ++;         
        pPBB->m_iBlkMBBase[X_INDEX] += 2;            
	     //pPBB->m_iFrmMBOffset[0] += MB_SIZE;
        //pPBB->m_iFrmMBOffset[1] += BLOCK_SIZE; 
         //
        pmbmd++;
        pmbmd_lastrow++;
        pmv1++;
    }
    return WMV_Succeeded;
}


tWMVDecodeStatus ProcessMBRow_MC_BVOP(tWMVDecInternalMember *pWMVDec,CWMVMBMode* pmbmd,U32_WMV imbY,U32_WMV y,
                                                     I32_WMV *bIntraFlag, I32_WMV *b1MVFlag,EMB_PBMainLoop *pPBB,U32_WMV threadflag)
{
    CoordI x;
    I32_WMV imbX, iMB=imbY*pWMVDec->m_uintNumMBX; 
    tWMVDecodeStatus result = WMV_Succeeded;
    EMB_BMainLoop * pBMainLoop = &pWMVDec->m_EMB_BMainLoop ;
    EMB_PBMainLoop * pMainLoop = pPBB;//(EMB_PBMainLoop *)pBMainLoop ; 

    //CWMVMBMode* pmbmd_lastrow = NULL;

    //pPBB->m_iFrmMBOffset[0] -= MB_SIZE*pPBB->m_uintNumMBX;
    //pPBB->m_iFrmMBOffset[1] -= BLOCK_SIZE*pPBB->m_uintNumMBX; 

    for (x = 0, imbX = 0; imbX < (I32_WMV) pWMVDec->m_uintNumMBX; imbX++, x += MB_SIZE, iMB++)
    {            
        pWMVDec->m_ncols = imbX;        
        pmbmd->m_chMBMode = MB_1MV;  

        if(bIntraFlag[imbX]==0)
        {  
#if defined(VOARMV7) || defined(NEW_C)
            result = DecodeBMB_EMB_Fast_MC_new(pBMainLoop,  pmbmd,  imbX, imbY,pPBB); 
#else
            result = DecodeBMB_EMB_Fast_MC(pBMainLoop,  pmbmd,  imbX, imbY,pPBB);  
#endif
            if (WMV_Succeeded != result)
                return result;
        }
        else {      // INTRA
             pMainLoop->m_pMotion[iMB].I32 = IBLOCKMV;
             pMainLoop->m_pMotionC[iMB].I32 = IBLOCKMV;  

#if defined(VOARMV7) || defined(NEW_C)
             result = DecodeBMB_EMB_Intra_MC_new(pBMainLoop, pmbmd, imbX, imbY,pPBB);
#else
             result = DecodeBMB_EMB_Intra_MC(pBMainLoop, pmbmd, imbX, imbY,pPBB);
#endif

             if (WMV_Succeeded != result)
                 return result; 
        }

        pmbmd++;

    }
    return WMV_Succeeded;
}

/****************************************************************************************
  decodeB_Deblock : B frame stuff
****************************************************************************************/
//#pragma code_seg (EMBSEC_BML)

tWMVDecodeStatus decodeB_EMB (tWMVDecInternalMember *pWMVDec)
{
    tWMVDecodeStatus result;        
    CWMVMBMode* pmbmd ,*pmbmd_lastrow=NULL;         
    CoordI y; 
    U32_WMV imbY;
    EMB_BMainLoop * pBMainLoop = &pWMVDec->m_EMB_BMainLoop ;      
    Bool_WMV bFrameInPostBuf = FALSE; 
    EMB_PBMainLoop *pMainPBB = &pBMainLoop->PB[0];
    U32_WMV i=0;
	U32_WMV MCBuffer[320];

	pWMVDec->iSliceHead		= 0;
	pWMVDec->iSliceHead_Pre	= 0;
	pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;

    pMainPBB->prgMotionCompBuffer_EMB =  MCBuffer;// pWMVDec->m_rgMotionCompBuffer_EMB;

    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];    
    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];   
    
    if (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR)
        pWMVDec->m_iFilterType = FILTER_BILINEAR;
    else
        pWMVDec->m_iFilterType = FILTER_BICUBIC;
    
    if ((pWMVDec->m_iBNumerator * pWMVDec->m_iBFrameReciprocal) >> 7) {
        pWMVDec->m_mbtShortBSymbol = BACKWARD;
        pWMVDec->m_mbtLongBSymbol = FORWARD;
    }
    else {
        pWMVDec->m_mbtShortBSymbol = FORWARD;
        pWMVDec->m_mbtLongBSymbol = BACKWARD;
    }

//#ifdef STABILITY
//    if(pWMVDec->m_pAltTables->m_iNContexts>11)
//        return -1;
//#endif
    
    // SET_NEW_FRAME
    pWMVDec->m_iHalfPelMV =
        (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL || pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR);
    //t_AltTablesSetNewFrame(pWMVDec->m_pAltTables, pWMVDec->m_iStepSize, 1);        
    // Set transform type huffman pointers
    if (pWMVDec->m_iStepSize < 5) {
        pWMVDec->m_pHuf4x4PatternDec = &pWMVDec->m_huf4x4PatternLowQPDec;
        pWMVDec->m_pHufMBXformTypeDec = &pWMVDec->m_hufMBXformTypeLowQPDec;
        pWMVDec->m_pHufBlkXformTypeDec = &pWMVDec->m_hufBlkXformTypeLowQPDec;
    }
    else if (pWMVDec->m_iStepSize < 13) { // Mid rate
        pWMVDec->m_pHuf4x4PatternDec = &pWMVDec->m_huf4x4PatternMidQPDec;
        pWMVDec->m_pHufMBXformTypeDec = &pWMVDec->m_hufMBXformTypeMidQPDec;
        pWMVDec->m_pHufBlkXformTypeDec = &pWMVDec->m_hufBlkXformTypeMidQPDec;
    }
    else { // Low rate
        pWMVDec->m_pHuf4x4PatternDec = &pWMVDec->m_huf4x4PatternHighQPDec;
        pWMVDec->m_pHufMBXformTypeDec = &pWMVDec->m_hufMBXformTypeHighQPDec;
        pWMVDec->m_pHufBlkXformTypeDec = &pWMVDec->m_hufBlkXformTypeHighQPDec;
    }         

    //pmbmd = pWMVDec->m_rgmbmd;        
    
    if (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR)
        pWMVDec->m_iFilterType = FILTER_BILINEAR;
    else
        pWMVDec->m_iFilterType = FILTER_BICUBIC;


    pWMVDec->m_iHalfPelMV =  (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL || pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR);
   
	//B VOP may use m_rgmv1_EMB which updated by prevoius P VOP
	if(pWMVDec->m_tFrmTypePrevious == IVOP)
	{
		pWMVDec->m_rgmv1_EMB = pWMVDec->m_pfrmCurrQ->m_rgmv1_EMB;
		if (pWMVDec->m_cvCodecVersion >= WMVA && pWMVDec->m_rgmv1_EMB)
		{
			U32_WMV imbY, imbX;
			for ( imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++)
				for ( imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++)
				{
					pWMVDec->m_rgmv1_EMB[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.I16[X_INDEX] = 0;
					pWMVDec->m_rgmv1_EMB[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.I16[Y_INDEX] = 0;
				}
		}
	}
	if(pWMVDec->m_tFrmTypePrevious == PVOP)
		pWMVDec->m_rgmv1_EMB = pWMVDec->m_pfrmRef1Q->m_rgmv1_EMB;
	// SET_NEW_FRAME
    //t_AltTablesSetNewFrame(pWMVDec->m_pAltTables, pWMVDec->m_iStepSize, 1);
    SetupMVDecTable_EMB(pWMVDec);   

#if 0
	printf("pWMVDec->m_tFrmTypePrevious  =%d 0x%x  0x%x  %d %d\n",pWMVDec->m_tFrmTypePrevious,
									pWMVDec->m_pfrmRef0Q,
									pWMVDec->m_pfrmRef1Q,
									pWMVDec->m_pfrmRef0Q->m_decodeprocess,
									pWMVDec->m_pfrmRef1Q->m_decodeprocess);
#endif


#ifdef USE_FRAME_THREAD
			//TODO tongbu TEST
#if 0
			if(pWMVDec->CpuNumber > 1)
			{
				volatile I32_WMV *ref_decodeprocess = &pWMVDec->m_pfrmRef1Q->m_decodeprocess;
				while(*ref_decodeprocess <= (I32_WMV)pWMVDec->m_uintNumMBY)
					bkprintf("b");
			}
#endif
#endif

    SetupEMBBMainLoopCtl(pWMVDec,pMainPBB);
    //SetupEMBBMainLoopCtl(pWMVDec,pThreadPBB);

	//printf("====BVOP   ref0 = 0x%x, ref1 = 0x%x \n",pWMVDec->m_pfrmRef0Q,pWMVDec->m_pfrmRef1Q);
	//memset(pWMVDec->m_rgmbmd_cur,0,pWMVDec->m_uintNumMBX*sizeof(CWMVMBMode*));
	//memset(pWMVDec->m_rgmbmd_last,0,pWMVDec->m_uintNumMBX*sizeof(CWMVMBMode*));

    {
        for (imbY = 0, y = 0; imbY < pWMVDec->m_uintNumMBY; imbY++)
        {   
            pWMVDec->m_nrows = imbY;

#ifdef USE_FRAME_THREAD
			//TODO tongbu
			if(pWMVDec->CpuNumber > 1)
			{  // Since BVOP will use "m_rgmv1_EMB" which calcualted by the proviouse PVOP.

				if(pWMVDec->m_tFrmTypePrevious == IVOP)
				{
					volatile I32_WMV *ref_decodeprocess = &pWMVDec->m_pfrmRef0Q->m_decodeprocess;
					while((I32_WMV)imbY >= *ref_decodeprocess)
					{
						thread_sleep(0);
						bkprintf("b");
					}
				}
				else if(pWMVDec->m_tFrmTypePrevious == PVOP)
				{
					volatile I32_WMV *ref_decodeprocess = &pWMVDec->m_pfrmRef1Q->m_decodeprocess;
					while((I32_WMV)imbY >= *ref_decodeprocess)
					{
						thread_sleep(0);
						bkprintf("b");
					}
				}
			}
#endif

            pMainPBB->m_rgiCurrBaseBK[0] = 2*imbY*(pWMVDec->m_uintNumMBX<<1);
            pMainPBB->m_iBlkMBBase[Y_INDEX] = (I16_WMV)(2*imbY);
            pMainPBB->m_iBlkMBBase[X_INDEX] = 0;    

            if(imbY ==2 )
                imbY = imbY;


            if((imbY&1) == 0)
            {
                pmbmd = pWMVDec->m_rgmbmd_cur;
                pmbmd_lastrow = pWMVDec->m_rgmbmd_last;
            }
            else
            {
                pmbmd = pWMVDec->m_rgmbmd_last;
                pmbmd_lastrow = pWMVDec->m_rgmbmd_cur;
            }
            for(i=0;i< pWMVDec->m_uintNumMBX;i++)
            {
                (pmbmd+i)->m_bSkip =  (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_bSkip;
                (pmbmd+i)->m_mbType =  (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_mbType;
                (pmbmd+i)->m_iQP =  pMainPBB->m_iQP;
                (pmbmd+i)->m_bBoundary =  (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_bBoundary;
                (pmbmd+i)->m_bOverlapIMB = (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_bOverlapIMB;
            }

           result = ProcessMBRow_Parser_BVOP(pWMVDec,
                                                pmbmd, 
                                                pmbmd_lastrow,
                                                imbY,0,
                                                pMainPBB->bIntraFlag,
                                                NULL,pMainPBB);
           if(result)
               return WMV_Failed;

		 

           result = ProcessMBRow_MC_BVOP   (pWMVDec, pmbmd, imbY,0, 
                                                    pMainPBB->bIntraFlag,
                                                    NULL,pMainPBB,0);
            if(result)
                return WMV_Failed;

            pBMainLoop->m_ppPredictMV_EMB = pBMainLoop->m_rgfPredictMV_EMB + 1;

			if(pWMVDec->CpuNumber > 1 && pWMVDec->m_bLoopFilter )
			{
				DeblockMBRow(pWMVDec,  imbY);
			}

#ifdef USE_FRAME_THREAD
			if(imbY > 0)
			{
				pWMVDec->m_pfrmCurrQ->m_decodeprocess = imbY-1;
				tbprintf("[thread%d]  BVOP decode process %d %d\n",pWMVDec->nthreads,pWMVDec->m_pfrmCurrQ->m_decodeprocess,pWMVDec->nInputFrameIndx);
			}
#endif
        }
    }

    //voDumpYuv_B(pWMVDec,pWMVDec->m_nBframes); 

#ifdef TRACEVC1
	{
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp," MC finished  \n");
		fclose(fp);
	}
#endif

#ifndef MARK_POST
	if(pWMVDec->CpuNumber <= 1)
	{
		if (pWMVDec->m_bLoopFilter) 
			DeblockSLFrame_V9 (pWMVDec, 0, pWMVDec->m_uiMBEnd0);
	}
#endif
    //pWMVDec->m_bRenderFromPostBuffer = bFrameInPostBuf;    
    pWMVDec->m_iFilterType = FILTER_BICUBIC;


    pWMVDec->error_flag_thread = 0;
    pWMVDec->error_flag_main = 0;

#ifdef TRACEVC1
	{
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp," B   LOOP  finished  \n");
		fclose(fp);
	}
#endif
   
    //voDumpYuv_B(pWMVDec,pWMVDec->m_nBframes);   

#ifdef USE_FRAME_THREAD
	pWMVDec->m_pfrmCurrQ->m_decodeprocess = MAX_MB_HEIGHT; //+2: pad
	voprintf("[thread%d]  BVOP decode process %d \n",pWMVDec->nthreads,pWMVDec->m_pfrmCurrQ->m_decodeprocess);

	if(pWMVDec->CpuNumber > 1 )
	{
		volatile I32_WMV *ref_decodeprocess = &pWMVDec->m_pfrmRef1Q->m_decodeprocess;
		while(*ref_decodeprocess < pWMVDec->m_uintNumMBY+2) //control the finish order
			;
	}

#endif

	if(pWMVDec->bUseRef0Process)
		 pWMVDec->m_pfrmRef0Process->m_decodeprocess = MAX_MB_HEIGHT;

#if 0
	 FREE_PTR (pWMVDec, pMainPBB->bIntraFlag); 
#endif

    return WMV_Succeeded;
}

//#pragma code_seg (EMBSEC_BML2)
Bool_WMV decodeDCTPredictionB_EMB (EMB_BMainLoop * pBMainLoop,
                                   CWMVMBMode *pmbmd, 
                                   CWMVMBMode *pmbmd_lastrow,
                                   I16_WMV *pIntra, 
                                   I16_WMV *pIntra_lastrow,
                                   I32_WMV iblk,
                                   I32_WMV iX, I32_WMV iY, I32_WMV *piShift, I32_WMV *iDirection, I16_WMV *pPredScaled,EMB_PBMainLoop* pPBB )

{
    
    Bool_WMV bACPredOn = TRUE_WMV;
    I32_WMV iUV = iblk >>2;
    //EMB_PBMainLoop * pMainLoop = ( EMB_PBMainLoop *) pBMainLoop;
    //I32_WMV iStride = pPBB->m_iNumBlockX >> iUV;
    Bool_WMV   iShift = 0; //= *piShift;
    I32_WMV  iWidth  = pPBB->m_iNumBlockX >> 1;
    I16_WMV *pPredIntraLeft, *pPredIntraTop, *pPred;
    Bool_WMV bDCACPred = FALSE_WMV;
    UMotion_EMB *pMotion = pPBB->m_pMotion;
    I32_WMV iIndex = pPBB->m_rgiCurrBaseBK[1];

    pPred = pPredIntraLeft = pPredIntraTop = NULL_WMV;

    if(iUV) pMotion = pPBB->m_pMotionC;

    if(pPBB->m_pWMVDec->m_bSliceWMVA)
    {
       if(!iUV)
       {
            if (((pPBB->m_pWMVDec->m_tFrmType == BVOP ) && (iY & 1)) ||
                (iY && ((pPBB->m_pWMVDec->m_tFrmType == IVOP || pPBB->m_pWMVDec->m_tFrmType == BIVOP) || pMotion[iIndex - iWidth].I32 == IBLOCKMV))) {    

            if (((iY & 1) == 1) || !pPBB->m_pWMVDec->m_pbStartOfSliceRow[iY>>1]) {
                pPred = pPredIntraTop = pIntra_lastrow;//pIntra - 16 * iStride;// + pWMVDec->m_iACRowPredOffset;
                iShift = pPBB->m_pWMVDec->m_iACRowPredShift;
            }
            }
       }
       else
       {
           if (iY && !pPBB->m_pWMVDec->m_pbStartOfSliceRow[iY]) {
               if (pMotion[(iY - 1) * (I32_WMV) pPBB->m_pWMVDec->m_uintNumMBX + iX].I32 == IBLOCKMV
                   || ( pPBB->m_pWMVDec->m_tFrmType  == IVOP || pPBB->m_pWMVDec->m_tFrmType  == BIVOP)) {

                   pPred = pPredIntraTop = pIntra_lastrow;//pIntra - 16 * pPBB->m_pWMVDec->m_uintNumMBX;
                   iShift = pPBB->m_pWMVDec->m_iACRowPredShift;
               }
           }
       }
    }
    else 
    {
        if ((iY & !iUV)||(iY && (pMotion[iIndex - iWidth].I32 == IBLOCKMV))) 
        {    
            pPred = pPredIntraTop = pIntra_lastrow;//pIntra - 16 * iStride;// + pWMVDec->m_iACRowPredOffset;
            iShift = pPBB->m_iACRowPredShift;
        }
    }
    
     if(iblk > 4) 
         iblk = 4;

     if ((iX & !iUV)||(iX && (pMotion[iIndex - 1].I32 == IBLOCKMV))) 
    {
        pPred = pPredIntraLeft = pIntra - 16;//pWMVDec->m_iACColPredOffset;
        iShift = pPBB->m_iACColPredShift;
    }
    if (pPredIntraLeft && pPredIntraTop) {
        I32_WMV iTopLeftDC = 0;
        I32_WMV iTopDC ;
        I32_WMV iLeftDC;
        I32_WMV iDiff0, iDiff1;
        if (((iX | iY) & !iUV) || (pMotion[(iIndex - iWidth - 1)].I32== IBLOCKMV) ) {                
            iTopLeftDC = pPredIntraTop[pPBB->m_iACRowPredOffset - 16];
        }
        iTopDC = pPredIntraTop[pPBB->m_iACRowPredOffset];
        iLeftDC = pPredIntraLeft[pPBB->m_iACColPredOffset];              


        //pmbmd_lastrow = pmbmd-pPBB->m_pWMVDec->m_uintNumMBX;
        ScaleDCPredForDQuant (pPBB->m_pWMVDec, iblk, pmbmd, pmbmd_lastrow,&iTopDC, &iLeftDC, &iTopLeftDC);
        
        iDiff0 = iTopLeftDC - iLeftDC;
        iDiff1 = iTopLeftDC - iTopDC;
        if (abs (iDiff0) < abs (iDiff1)) {
            pPred = pPredIntraTop;
            iShift = pPBB->m_iACRowPredShift;
        }
    }
   
    
    if (pPred != NULL_WMV) {

        if(!pmbmd->m_rgbDCTCoefPredPattern2[0])
            bACPredOn = FALSE_WMV;

        bDCACPred = TRUE_WMV;
        if (iShift == pPBB->m_iACColPredShift) {
            ScaleLeftPredForDQuant (pPBB->m_pWMVDec, iblk, pPred, pPredScaled, pmbmd);
        } else {
            //pmbmd_lastrow =  pmbmd - pPBB->m_pWMVDec->m_uintNumMBX;
            ScaleTopPredForDQuant (pPBB->m_pWMVDec, iblk, pPred, pPredScaled, pmbmd,pmbmd_lastrow);
        }
    }
    
    if (bACPredOn == FALSE_WMV) 
        iShift = -1;
    
    *iDirection = (pPred == pPredIntraTop);
    *piShift =  iShift ;
    return bDCACPred;
}

//#pragma code_seg (EMBSEC_BML2)

tWMVDecodeStatus DecodeBMB_EMB_Intra_Parser(EMB_BMainLoop  * pBMainLoop,
                                                        CWMVMBMode* pmbmd, 
                                                        CWMVMBMode* pmbmd_lastrow,
                                                        I32_WMV iMBX, I32_WMV iMBY, EMB_PBMainLoop* pPBB )
{
    U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    UnionBuffer * ppxliErrorQMB;
    tWMVDecodeStatus result = WMV_Succeeded;
    I32_WMV  iShift, iblk, iDirection;
    I16_WMV  *pIntra,*pIntra_lastrow, pPredScaled[16];
    U8_WMV * pDst;
    I32_WMV iUV;  
    I32_WMV iX;
    I32_WMV iY;

    Bool_WMV bDCACPredOn;
    I16_WMV *pPred = NULL_WMV;
    CDCTTableInfo_Dec** ppDCTTableInfo_Dec;

    //I32_WMV iXformType = pPBB->m_iFrameXformMode;
    DQuantDecParam *pDQ = &pPBB->m_prgDQuantParam [pmbmd->m_iQP ];

    CWMVMBModeBack *pMbMode =  pPBB->m_pMbMode + iMBX ;//+ iMBY*pPBB->m_uintNumMBX;
        
    if (pPBB->m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pPBB->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pPBB->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    }
    else {
        ppInterDCTTableInfo_Dec = pPBB->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pPBB->m_ppIntraDCTTableInfo_Dec;
    } 
#ifdef STABILITY
    if(pPBB->m_pX9dct==NULL)
        return -1;
#endif

    for (iblk = 0; iblk < 6; iblk++) 
    {
        Bool_WMV bResidual = rgCBP2[iblk];
        iUV = iblk >> 2;         
		if(!iUV)
			pDst = pPBB->m_ppxliCurrBlkBase[iblk] + iMBY*16*pPBB->m_iWidthPrev[iUV]+iMBX*16;
		else
			pDst = pPBB->m_ppxliCurrBlkBase[iblk] + iMBY*8*pPBB->m_iWidthPrev[iUV]+iMBX*8;
        //pDst = pPBB->m_ppxliCurrBlkBase[iblk] + pPBB->m_iFrmMBOffset[iUV];        
        pPred = NULL_WMV;
        if(!iUV)
        {
            iX = (iMBX << 1) + (iblk & 1);
            iY = (iMBY << 1) + ((iblk & 2) >> 1);                
            //pIntra = pPBB->m_pX9dct + ((pPBB->m_rgiCurrBaseBK[0] + pPBB->m_rgiBlkIdx[iblk]) << 4);
            pIntra = pPBB->m_pX9dct + ((iX + ((1 << 1) + ((iblk & 2) >> 1))*pPBB->m_iNumBlockX) << 4);
            pIntra_lastrow =  pIntra-16*pPBB->m_iNumBlockX;
            ppDCTTableInfo_Dec = ppIntraDCTTableInfo_Dec;

            if((iMBY&1) == 0)
            {
                pIntra = pPBB->m_pX9dct + ((iX + ((1 << 1) + ((iblk & 2) >> 1))*pPBB->m_iNumBlockX) << 4);
                pIntra_lastrow =  pIntra-16*pPBB->m_iNumBlockX;
            }
            else
            {
                if(iblk == 0 || iblk ==1)
                    pIntra_lastrow = pIntra + 16*pPBB->m_iNumBlockX ;
                else
                    pIntra_lastrow = pIntra - 3*16*pPBB->m_iNumBlockX;

                pIntra = pIntra - 2*16*pPBB->m_iNumBlockX;//pPB->m_pX9dct + ((iX + ((0 << 1) + ((iblk & 2) >> 1))*pPB->m_iNumBlockX) << 4);
            }

        }
        else
        {
            iX = iMBX; iY = iMBY;                
            //pIntra = pPBB->m_pX9dctUV[iblk-4] + ((pPBB->m_rgiCurrBaseBK[1]) << 4);   
            //pIntra = pPBB->m_pX9dctUV[iblk-4] + ((1 *pPBB->m_pWMVDec->m_uintNumMBX + iX) << 4);

            if((iMBY&1) == 0)
            {
                pIntra = pPBB->m_pX9dctUV[iblk-4] + ((1 *pPBB->m_pWMVDec->m_uintNumMBX + iX) << 4);
                pIntra_lastrow =  pIntra-16*pPBB->m_pWMVDec->m_uintNumMBX;//pPB->m_pX9dctUV[iblk-4] + ((0 *pPB->m_pWMVDec->m_uintNumMBX + iX) << 4);
            }
            else
            {
                pIntra_lastrow = pPBB->m_pX9dctUV[iblk-4] + ((1 *pPBB->m_pWMVDec->m_uintNumMBX + iX) << 4);
                pIntra =  pIntra_lastrow-16*pPBB->m_pWMVDec->m_uintNumMBX;//pPB->m_pX9dctUV[iblk-4] + ((0 *pPB->m_pWMVDec->m_uintNumMBX + iX) << 4);
            }

            ppDCTTableInfo_Dec = ppInterDCTTableInfo_Dec;
        }
        
        bDCACPredOn = decodeDCTPredictionB_EMB (pBMainLoop, pmbmd,pmbmd_lastrow, pIntra, pIntra_lastrow,iblk, iX, iY, 
                &iShift, &iDirection, pPredScaled,pPBB);
        
        ppxliErrorQMB = pPBB->m_rgiCoefReconBuf;
        if (bDCACPredOn) pPred = pPredScaled;
        result = (tWMVDecodeStatus)(result | DecodeInverseIntraBlockX9_EMB (pPBB->m_pWMVDec,
            ppDCTTableInfo_Dec,
            iblk,
            pPBB->m_pZigzagScanOrder, bResidual, pPred,
            iDirection, iShift, pIntra, pmbmd, pDQ,
            (I16_WMV*)(&pMbMode->pBackUpErrorInter[iblk][0]))); //get the quantized block
        
        pPBB->m_iDCTHorzFlags= pPBB->m_pWMVDec->m_iDCTHorzFlags; 
        
        //assert(pPBB->m_pWMVDec->m_pIntraX9IDCT_Dec == g_IDCTDec16_WMV3);
		if(pPBB->m_pWMVDec->m_pIntraX9IDCT_Dec != g_IDCTDec16_WMV3)
			return WMV_Failed;
        pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
#if !defined(VOARMV7) && !defined(NEW_C)
		pMbMode->pBackUpDCTHorzFlagsInter[iblk][0] = pPBB->m_iDCTHorzFlags;
#endif

    }

    pmbmd->m_bSkip = FALSE_WMV;

    return result ;
}
#if defined(VOARMV7)|| defined(NEW_C)
tWMVDecodeStatus DecodeBMB_EMB_Intra_MC_new(EMB_BMainLoop  * pBMainLoop,
                                                        CWMVMBMode* pmbmd, 
                                                        I32_WMV iMBX, I32_WMV iMBY,EMB_PBMainLoop *pPBB)
{
    tWMVDecodeStatus result = WMV_Succeeded;
    I32_WMV  iblk;
    U8_WMV * pDst;
    I32_WMV iUV;  

    CWMVMBModeBack *pMbMode = pPBB->m_pMbMode + iMBX;// + iMBY*pPBB->m_uintNumMBX;

    if(1)
    {
        UnionBuffer  *pTmpErrorQMB;

        for (iblk = 0; iblk < 6; iblk++) 
        {
            iUV = iblk >> 2;

            pTmpErrorQMB = &pMbMode->pBackUpErrorInter[iblk][0];//&pmbmd->pBackUpError[iblk];
            if(1){
                DQuantDecParam *pDQ = &pPBB->m_prgDQuantParam [pmbmd->m_iQP ];
                I32_WMV iDoubleStepSize = pDQ->iDoubleStepSize;
                I32_WMV iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
                I32_WMV iDCStepSize = pDQ->iDCStepSize;
                I16_WMV *rgiCoefRecon = (I16_WMV*)(&pMbMode->pBackUpErrorInter[iblk][0]);//(&pmbmd->pBackUpError[iblk]);
                I32_WMV iValue0, iValue1,i;

		        iValue0 = (rgiCoefRecon[0] * iDCStepSize);
		        iValue1 = rgiCoefRecon[1];
		        if(iValue1) {
			        int signMask = iValue1 >> 31;
			        iValue1 = iDoubleStepSize * iValue1+ (signMask ^ iStepMinusStepIsEven) - signMask;
		        }
        	
		        *(I32_WMV *) rgiCoefRecon = (iValue0&0xffff)  + (iValue1<<16);
		        for (i = 2; i < 64; i += 2) {
			        iValue0 = rgiCoefRecon[i];
			        if (iValue0) {
				        int signMask = iValue0 >> 31; // 0 or FFFFFFFF
				        iValue0 = (I16_WMV) ((I32_WMV) iDoubleStepSize * iValue0 + (signMask ^ iStepMinusStepIsEven) - signMask);
			        }
			        iValue1 = rgiCoefRecon[i+1];
			        if (iValue1) {
				        int signMask = iValue1 >> 31; // 0 or FFFFFFFF
				        iValue1 = (I16_WMV) ((I32_WMV) iDoubleStepSize * iValue1 + (signMask ^ iStepMinusStepIsEven) - signMask);
			        }
        	        
			        *(I32_WMV *) (rgiCoefRecon + i) = (iValue0&0xffff)  + (iValue1<<16);
		        }
            }

			if(!iUV)
				pDst = pPBB->m_ppxliCurrBlkBase[iblk] + iMBY*16*pPBB->m_iWidthPrev[iUV]+iMBX*16;
			else
				pDst = pPBB->m_ppxliCurrBlkBase[iblk] + iMBY*8*pPBB->m_iWidthPrev[iUV]+iMBX*8;

            voVC1InvTrans_8x8(pDst,
                pPBB->m_iWidthPrev[iUV],
                (U8_WMV*)pBMainLoop->m_rgIntraMotionCompBuffer_EMB,
                NULL,
                40,
                pTmpErrorQMB->i16,
				DCT_COEF1,
				DCT_COEF2);
        }
    }

    pmbmd->m_bSkip = FALSE_WMV;

    return result ;
}

#else
tWMVDecodeStatus DecodeBMB_EMB_Intra_MC(EMB_BMainLoop  * pBMainLoop,
                                                        CWMVMBMode* pmbmd, 
                                                        I32_WMV iMBX, I32_WMV iMBY,EMB_PBMainLoop *pPBB)
{
    tWMVDecodeStatus result = WMV_Succeeded;
    I32_WMV  iblk;
    U8_WMV * pDst;
    I32_WMV iUV;  

    CWMVMBModeBack *pMbMode = pPBB->m_pMbMode + iMBX;// + iMBY*pPBB->m_uintNumMBX;

    if(1)
    {
        UnionBuffer  *pTmpErrorQMB;

        for (iblk = 0; iblk < 6; iblk++) 
        {
            iUV = iblk >> 2;

            pTmpErrorQMB = &pMbMode->pBackUpErrorInter[iblk][0];//&pmbmd->pBackUpError[iblk];
            if(1){
                DQuantDecParam *pDQ = &pPBB->m_prgDQuantParam [pmbmd->m_iQP ];
                I32_WMV iDoubleStepSize = pDQ->iDoubleStepSize;
                I32_WMV iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
                I32_WMV iDCStepSize = pDQ->iDCStepSize;
                I16_WMV *rgiCoefRecon = (I16_WMV*)(&pMbMode->pBackUpErrorInter[iblk][0]);//(&pmbmd->pBackUpError[iblk]);
                I32_WMV iValue0, iValue1,i;

		        iValue0 = (rgiCoefRecon[0] * iDCStepSize);
		        iValue1 = rgiCoefRecon[1];
		        if(iValue1) {
			        int signMask = iValue1 >> 31;
			        iValue1 = iDoubleStepSize * iValue1+ (signMask ^ iStepMinusStepIsEven) - signMask;
		        }
        	
		        *(I32_WMV *) rgiCoefRecon = iValue0 + (iValue1<<16);
		        for (i = 2; i < 64; i += 2) {
			        iValue0 = rgiCoefRecon[i];
			        if (iValue0) {
				        int signMask = iValue0 >> 31; // 0 or FFFFFFFF
				        iValue0 = (I16_WMV) ((I32_WMV) iDoubleStepSize * iValue0 + (signMask ^ iStepMinusStepIsEven) - signMask);
			        }
			        iValue1 = rgiCoefRecon[i+1];
			        if (iValue1) {
				        int signMask = iValue1 >> 31; // 0 or FFFFFFFF
				        iValue1 = (I16_WMV) ((I32_WMV) iDoubleStepSize * iValue1 + (signMask ^ iStepMinusStepIsEven) - signMask);
			        }
        	        
			        *(I32_WMV *) (rgiCoefRecon + i) = iValue0 + (iValue1<<16);
		        }
            }

            g_IDCTDec16_WMV3_Fun ( pTmpErrorQMB, pTmpErrorQMB, BLOCK_SIZE, pMbMode->pBackUpDCTHorzFlagsInter[iblk][0] );

			if(!iUV)
				pDst = pPBB->m_ppxliCurrBlkBase[iblk] + iMBY*16*pPBB->m_iWidthPrev[iUV]+iMBX*16;
			else
				pDst = pPBB->m_ppxliCurrBlkBase[iblk] + iMBY*8*pPBB->m_iWidthPrev[iUV]+iMBX*8;

            //pDst = pPBB->m_ppxliCurrBlkBase[iblk] + pPBB->m_iFrmMBOffset[iUV];
            g_AddError_SSIMD_Fun(pDst, 
                                            pBMainLoop->m_rgIntraMotionCompBuffer_EMB, 
                                            pTmpErrorQMB->i32,
                                            pPBB->m_iWidthPrev[iUV]);
        }
    }

    pmbmd->m_bSkip = FALSE_WMV;

    return result ;
}


#endif
//static int g_mc_cnt[4][4];
//#pragma code_seg (EMBSEC_BML)
I32_WMV g_MotionCompB_EMB (EMB_BMainLoop *pBMainLoop,  U8_WMV * pDest,  I32_WMV iRefIdx, 
                            I32_WMV iYUV,EMB_PBMainLoop *pPBB,
                            U32_WMV ncols,	U32_WMV nrows)
{
    I32_WMV iUV = iYUV>0;
    Bool_WMV b1MV = !iUV;
    I32_WMV  iFilterType = pPBB->m_iFilterType[iUV];
    U8_WMV  *pRef;
    CDiffMV_EMB * pDiffMV = pPBB->pDiffMV_EMB_Bak + iRefIdx + (iYUV<<1);
    I32_WMV  iShiftX = pDiffMV->Diff.I16[X_INDEX];
    I32_WMV  iShiftY = pDiffMV->Diff.I16[Y_INDEX];
    int xIndex = ncols<<4;
    int yIndex = nrows<<4;

    I32_WMV iXFrac, iYFrac;
    tWMVDecInternalMember *pWMVDec = pPBB->m_pWMVDec;

    if( pWMVDec->m_bCodecIsWVC1) //0816
    {
        if(!iYUV)
        {
            PullBackMotionVector (pWMVDec, &iShiftX, &iShiftY, ncols, nrows);
            if( (iShiftX>>2) + xIndex <= -32)  //zou fix
			    iShiftX =  (-32-xIndex)<<2;
		    else if( (iShiftX>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)
			    iShiftX = (pWMVDec->m_iWidthPrevY+32 -xIndex)<<2;

		    if((iShiftY>>2) + yIndex <= -32)
			    iShiftY =  (-32-yIndex)<<2;
		    else if((iShiftY>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)
			    iShiftY = (pWMVDec->m_iHeightPrevY+32 -yIndex)<<2;

            pPBB->pDiffMV_EMB_Bak[ iRefIdx + (Y_EMB<<1)].Diff.I16[X_INDEX] = iShiftX;
            pPBB->pDiffMV_EMB_Bak[ iRefIdx + (Y_EMB<<1)].Diff.I16[Y_INDEX] = iShiftY;
        }
        else
        {
            ChromaMV_B (pWMVDec, &iShiftX, &iShiftY);
            if( (iShiftX>>2) + xIndex/2 <= -16)  //zou fix
			    iShiftX =  (-16-xIndex/2)<<2;
		    else if( (iShiftX>>2) +xIndex/2 >= pWMVDec->m_iWidthPrevUV+16)
			    iShiftX = (pWMVDec->m_iWidthPrevUV+16 -xIndex/2)<<2;

		    if((iShiftY>>2) + yIndex/2 <= -16)
			    iShiftY =  (-16-yIndex/2)<<2;
		    else if((iShiftY>>2) +yIndex/2 >= pWMVDec->m_iHeightPrevUV+16)
			    iShiftY = (pWMVDec->m_iHeightPrevUV+16 -yIndex/2)<<2;
        }
    }
    else
    {
#ifdef STABILITY
        if(!iUV)
        {
            if( ((iShiftX>>2) + xIndex <= -32) ||  ( (iShiftX>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)  )
                return -1;
             if(((iShiftY>>2) + yIndex <= -32) ||  ((iShiftY>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)  )
                return -1;
        }
        else
        {
             if(( (iShiftX>>2) + xIndex/2 <= -16) ||  ((iShiftX>>2) +xIndex/2 >= pWMVDec->m_iWidthPrevUV+16)  )
                return -1;
              if(((iShiftY>>2) + yIndex/2 <= -16) ||  ((iShiftY>>2) +yIndex/2 >= pWMVDec->m_iHeightPrevUV+16)  )
                return -1;
        }
#endif
    } 

    if(!iUV)
    {
        pRef = pPBB->m_ppxliRefBlkBase[iRefIdx + (iYUV<<1)] + nrows*16*pWMVDec->m_iWidthPrevY+ncols*16 ;
        //+ pPBB->m_iFrmMBOffset[iUV];     
    }
    else
    {
        pRef = pPBB->m_ppxliRefBlkBase[iRefIdx + (iYUV<<1)] + nrows*8*pWMVDec->m_iWidthPrevUV+ncols*8;
    }
    pRef = pRef + (iShiftY >> 2) * pPBB->m_iWidthPrev[iUV] + (iShiftX >> 2);
    

#ifdef STABILITY_NONE
    if(pPBB->m_iFrmMBOffset[0] != nrows*16*pWMVDec->m_iWidthPrevY+ncols*16  
        || pPBB->m_iFrmMBOffset[1] != nrows*8*pWMVDec->m_iWidthPrevUV+ncols*8)
    {
        return -1;
    }
#endif
    
    iXFrac = iShiftX&3;
    iYFrac = iShiftY&3;
    
    if(iFilterType == FILTER_BICUBIC)
    {
        I_SIMD ret;

        ret = (*pPBB->m_pInterpolateBicubic[iXFrac][iYFrac]) 
                (pRef, pPBB->m_iWidthPrev[iUV], pDest, pPBB, iXFrac, iYFrac, b1MV, pPBB->m_rgiNewVertTbl[iUV][b1MV]);

        if(ret) {
            g_InterpolateBlockBicubicOverflow_EMB(pRef, pPBB->m_iWidthPrev[iUV], pDest,  iXFrac,  iYFrac,  pPBB->m_iRndCtrl, b1MV);
        }
    }
    else
    {
        if(iXFrac|iYFrac)
            (*pPBB->m_pInterpolateBlockBilinear[iXFrac][iYFrac])
                ( pRef, pPBB->m_iWidthPrev[iUV], pDest, iXFrac, iYFrac, pPBB->m_iRndCtrl, b1MV);
        else
                g_InterpolateBlock_00_SSIMD_Fun(pRef, pPBB->m_iWidthPrev[iUV], pDest, pPBB, iXFrac, iYFrac,  b1MV, 0);

    }

    return 0;

}

I32_WMV g_MotionCompB_EMB_new (EMB_BMainLoop *pBMainLoop,  
                               U8_WMV * pDest,
                               I32_WMV iDstStride,
                               I32_WMV iRefIdx, 
                               I32_WMV iYUV,
                               EMB_PBMainLoop *pPBB,
                               U32_WMV ncols,	
                               U32_WMV nrows)
{
	VO_S32 ref_pixel_row = 0;
    I32_WMV iUV = iYUV>0;
    Bool_WMV b1MV = !iUV;
    I32_WMV  iFilterType = pPBB->m_iFilterType[iUV];
    U8_WMV  *pRef;
    CDiffMV_EMB * pDiffMV = pPBB->pDiffMV_EMB_Bak + iRefIdx + (iYUV<<1);
    I32_WMV  iShiftX = pDiffMV->Diff.I16[X_INDEX];
    I32_WMV  iShiftY = pDiffMV->Diff.I16[Y_INDEX];
    int xIndex = ncols<<4;
    int yIndex = nrows<<4;

    I32_WMV iXFrac, iYFrac;
    tWMVDecInternalMember *pWMVDec = pPBB->m_pWMVDec;

#ifdef USE_FRAME_THREAD
	 volatile I32_WMV *ref_decodeprocess = NULL; 


	 if(pWMVDec->bUseRef0Process)
	 {
		if(iRefIdx == 0)
		{
			 ref_decodeprocess = &pWMVDec->m_pfrmRef0Process->m_decodeprocess;
			 //printf("pWMVDec->m_pfrmRef0Process->m_decodeprocess = %d \n",pWMVDec->m_pfrmRef0Process->m_decodeprocess);
		}
		 else if(iRefIdx == 1)
			 ref_decodeprocess = &pWMVDec->m_pfrmRef1Q->m_decodeprocess;
	 }
	 else
	 {
		 if(iRefIdx == 0)
			 ref_decodeprocess = &pWMVDec->m_pfrmRef0Q->m_decodeprocess;
		 else if(iRefIdx == 1)
			 ref_decodeprocess = &pWMVDec->m_pfrmRef1Q->m_decodeprocess;
	 }

#endif

    if( pWMVDec->m_bCodecIsWVC1) //0816
    {
        if(!iYUV)
        {
            PullBackMotionVector (pWMVDec, &iShiftX, &iShiftY, ncols, nrows);
            if( (iShiftX>>2) + xIndex <= -32)  //zou fix
			    iShiftX =  (-32-xIndex)<<2;
		    else if( (iShiftX>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)
			    iShiftX = (pWMVDec->m_iWidthPrevY+32 -xIndex)<<2;

		    if((iShiftY>>2) + yIndex <= -32)
			    iShiftY =  (-32-yIndex)<<2;
		    else if((iShiftY>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)
			    iShiftY = (pWMVDec->m_iHeightPrevY+32 -yIndex)<<2;

            pPBB->pDiffMV_EMB_Bak[ iRefIdx + (Y_EMB<<1)].Diff.I16[X_INDEX] = iShiftX;
            pPBB->pDiffMV_EMB_Bak[ iRefIdx + (Y_EMB<<1)].Diff.I16[Y_INDEX] = iShiftY;
        }
        else
        {
            ChromaMV_B (pWMVDec, &iShiftX, &iShiftY);
            if( (iShiftX>>2) + xIndex/2 <= -16)  //zou fix
			    iShiftX =  (-16-xIndex/2)<<2;
		    else if( (iShiftX>>2) +xIndex/2 >= pWMVDec->m_iWidthPrevUV+16)
			    iShiftX = (pWMVDec->m_iWidthPrevUV+16 -xIndex/2)<<2;

		    if((iShiftY>>2) + yIndex/2 <= -16)
			    iShiftY =  (-16-yIndex/2)<<2;
		    else if((iShiftY>>2) +yIndex/2 >= pWMVDec->m_iHeightPrevUV+16)
			    iShiftY = (pWMVDec->m_iHeightPrevUV+16 -yIndex/2)<<2;
        }
    }
    else
    {
#ifdef STABILITY
        if(!iUV)
        {
            if( ((iShiftX>>2) + xIndex <= -32) ||  ( (iShiftX>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)  )
                return -1;
             if(((iShiftY>>2) + yIndex <= -32) ||  ((iShiftY>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)  )
                return -1;
        }
        else
        {
             if(( (iShiftX>>2) + xIndex/2 <= -16) ||  ((iShiftX>>2) +xIndex/2 >= pWMVDec->m_iWidthPrevUV+16)  )
                return -1;
              if(((iShiftY>>2) + yIndex/2 <= -16) ||  ((iShiftY>>2) +yIndex/2 >= pWMVDec->m_iHeightPrevUV+16)  )
                return -1;
        }
#endif
    } 

	if((iShiftX >> 2) > 16)
		iShiftX = iShiftX;

//tongbu
    if(!iUV)
    {
#ifdef USE_FRAME_THREAD
		if(pWMVDec->CpuNumber > 1)
		{
			ref_pixel_row = (nrows)*16+ (iShiftY >> 2) + 20; //20 for imv MV 20*20 of ref is needed.
			while((I32_WMV)(ref_pixel_row/16  ) >= *ref_decodeprocess)
			{
				thread_sleep(0);
				bkprintf("by");
			}
		}
#endif
        pRef = pPBB->m_ppxliRefBlkBase[iRefIdx + (iYUV<<1)] + nrows*16*pWMVDec->m_iWidthPrevY+ncols*16 ; 
    }
    else
    {
#ifdef USE_FRAME_THREAD
		if(pWMVDec->CpuNumber > 1)
		{
			ref_pixel_row = (nrows)*8+ (iShiftY >> 2) + 12; //12 for imv MV 12*12 of ref is needed.
			while((I32_WMV)(ref_pixel_row/8) >= *ref_decodeprocess)
			{
				thread_sleep(0);
				bkprintf("buv"); //width< 480  sleep(0);   width > 900 idle ; 
			}
		}
#endif
        pRef = pPBB->m_ppxliRefBlkBase[iRefIdx + (iYUV<<1)] + nrows*8*pWMVDec->m_iWidthPrevUV+ncols*8;
    }
    pRef = pRef + (iShiftY >> 2) * pPBB->m_iWidthPrev[iUV] + (iShiftX >> 2);

	 //if(pPBB->m_pWMVDec->m_frameNum == 65  && nrows == 0 && ncols==3 && iRefIdx==0 && !iUV)
	 //{
		//ncols =ncols;
		//printf("%d %d %d %d\n",iShiftX,iShiftY,pWMVDec->m_pfrmRef0Process->m_decodeprocess);
	 //}  


#ifdef STABILITY_NONE
    if(pPBB->m_iFrmMBOffset[0] != nrows*16*pWMVDec->m_iWidthPrevY+ncols*16  
        || pPBB->m_iFrmMBOffset[1] != nrows*8*pWMVDec->m_iWidthPrevUV+ncols*8)
    {
        return -1;
    }
#endif
    
    iXFrac = iShiftX&3;
    iYFrac = iShiftY&3;
    
    if(iFilterType == FILTER_BICUBIC)
    {
        I_SIMD ret;
        ret = (*pPBB->m_pInterpolateBicubic_new[iXFrac][iYFrac])
                    (pRef, pPBB->m_iWidthPrev[iUV], pDest, iDstStride, pPBB, iXFrac, iYFrac, b1MV);
    }
    else
    {
         if(iXFrac|iYFrac) 
            pPBB->m_pInterpolateBlockBilinear_new[iXFrac][iYFrac]
                ( pRef, pPBB->m_iWidthPrev[iUV], pDest, iDstStride, iXFrac, iYFrac, pPBB->m_iRndCtrl, b1MV);         
        else
           pPBB->m_pInterpolateBlock_00_new(pRef, pPBB->m_iWidthPrev[iUV], pDest, iDstStride, pPBB, iXFrac, iYFrac,  b1MV);

    }

    return 0;

}

extern int z0,z1;
//#pragma code_seg (EMBSEC_BML)
tWMVDecodeStatus DecodeBMB_EMB_Fast_Parser(EMB_BMainLoop  * pBMainLoop,
                                   CWMVMBMode*            pmbmd, 
                                   I32_WMV iMBX, I32_WMV iMBY ,EMB_PBMainLoop *pPBB)
{
    U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    UnionBuffer * ppxliErrorQMB;
    tWMVDecodeStatus result = WMV_Succeeded;
    I32_WMV  iblk;
    I32_WMV iUV;
    Bool_WMV bFirstBlock =  pPBB->m_bMBXformSwitching;
    U32_WMV iSubblockPattern = pmbmd->m_iSubblockPattern;	

    I32_WMV iXformType = pPBB->m_iFrameXformMode;
    DQuantDecParam *pDQ;   
    //int keepidct = 0;
    CWMVMBModeBack *pMbMode = pPBB->m_pMbMode + iMBX ;//+ iMBY*pPBB->m_uintNumMBX;

    pDQ = &pPBB->m_prgDQuantParam [pmbmd->m_iQP ];
        
    if (pPBB->m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;        
    if (pPBB->m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pPBB->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pPBB->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    }
    else {
        ppInterDCTTableInfo_Dec = pPBB->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pPBB->m_ppIntraDCTTableInfo_Dec;
    }    

    for (iblk = 0; iblk < 6; iblk++) {
        Bool_WMV bResidual = rgCBP2[iblk];
        iUV = iblk >> 2;        
        if(bResidual)
        {
            if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
                I32_WMV iIndex = Huffman_WMV_get(pPBB->m_pHufBlkXformTypeDec, pPBB->m_pbitstrmIn);
                iXformType = pPBB->m_iBlkXformTypeLUT[iIndex];
                iSubblockPattern = pPBB->m_iSubblkPatternLUT[iIndex];
            }
            
            pmbmd->m_rgcBlockXformMode[iblk] =(I8_WMV)iXformType;
            pMbMode->pBackUpFormType[iblk] = (I8_WMV)iXformType;

            
            if (iXformType == XFORMMODE_8x8) 
            {
                ppxliErrorQMB = pPBB->m_rgiCoefReconBuf;

#if defined(VOARMV7)||defined(NEW_C)
                result = DecodeInverseInterBlockQuantize16_EMB_new(pPBB, ppInterDCTTableInfo_Dec,
                    XFORMMODE_8x8, pDQ,
                    (I16_WMV*)(&pMbMode->pBackUpErrorInter[iblk][0]));
				pMbMode->pBackUpDCTRunFlags[iblk][0] = pPBB->m_iDctRunFlags;
				//if(pPBB->m_iDctRunFlags <= 0)
				//	z0++;
				//else
				//	z1++;
#else
                result = DecodeInverseInterBlockQuantize16_EMB(pPBB, ppInterDCTTableInfo_Dec,
                    XFORMMODE_8x8, pDQ,
                    (I16_WMV*)(&pMbMode->pBackUpErrorInter[iblk][0]));
				pMbMode->pBackUpDCTHorzFlagsInter[iblk][0] = pPBB->m_iDCTHorzFlags;
#endif                                 
            }
            else 
            {
                I32_WMV i, iLoopTop, iLoopMask, iPattern;
                I32_WMV iIdx;
                ppxliErrorQMB = pPBB->m_ppxliErrorQ;                
                if (iXformType == XFORMMODE_8x4 || iXformType == XFORMMODE_4x8) {
                    if (pmbmd->m_bBlkXformSwitchOn || bFirstBlock) {
                        iPattern = iSubblockPattern;
                    }
                    else {
                        if (BS_getBit(pPBB->m_pbitstrmIn) == 1) {
                            iPattern = BS_getBit(pPBB->m_pbitstrmIn) + 1;
                        } else
                            iPattern = 3;
                    }
                    iLoopTop = 2;
                    iLoopMask = 2;                    
                }
                else {
                    iPattern = Huffman_WMV_get(pPBB->m_pHuf4x4PatternDec, pPBB->m_pbitstrmIn) + 1;
                    iLoopTop = 4;
                    iLoopMask = 8;
                }
                
                iIdx = iXformType-1;
                rgCBP2[iblk] = (U8_WMV)iPattern;  

                pMbMode->pBackUpLoopMask[iblk] = iLoopMask;
                pMbMode->pBackUpLoopTop[iblk] = iLoopTop;                
                for( i=0; i<iLoopTop; i++ ) {
                    if ( iPattern & (iLoopMask>>i) )
                    {
#if defined(VOARMV7)||defined(NEW_C)
                        result = DecodeInverseInterBlockQuantize16_EMB_new(pPBB, ppInterDCTTableInfo_Dec,
                            iXformType, pDQ,(I16_WMV*)(&pMbMode->pBackUpErrorInter[iblk][i]));
						pMbMode->pBackUpDCTRunFlags[iblk][i] = pPBB->m_iDctRunFlags;
#else
                        result = DecodeInverseInterBlockQuantize16_EMB(pPBB, ppInterDCTTableInfo_Dec,
                            iXformType, pDQ,(I16_WMV*)(&pMbMode->pBackUpErrorInter[iblk][i]));
						pMbMode->pBackUpDCTHorzFlagsInter[iblk][i] = pPBB->m_iDCTHorzFlags;
#endif                           
                    }
                    else
                    {
#if !defined(VOARMV7)&&!defined(NEW_C)
                        pMbMode->pBackUpIDCTClearOffset[iIdx][i] = pPBB->m_uiIDCTClearOffset[iIdx][i];
#endif
                    }
                }
            }
            bFirstBlock = FALSE;
        }
        else {
            pMbMode->pBackUpFormType[iblk] = 5;
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
        }     
    } 
    return result;
}

#if defined(VOARMV7)||defined(NEW_C)
tWMVDecodeStatus DecodeBMB_EMB_Fast_MC_new(EMB_BMainLoop* pBMainLoop,
                                                            CWMVMBMode* pmbmd, 
                                                            I32_WMV iMBX, I32_WMV iMBY,EMB_PBMainLoop *pPBB )
{
    U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
    tWMVDecodeStatus result = WMV_Succeeded;
    I32_WMV  i,iblk,iUV;
    UMotion_EMB iPred;
    U8_WMV * pDst;
	
    U32_WMV ppxliInterpolate0YMB[160+80], ppxliInterpolate1YMB[160+80];    
    pPBB->m_pSSIMD_DstOffSet = pBMainLoop->m_SSIMD_DstLUT_B;  

    pPBB->pMotionCompBuffer_EMB[0] = ppxliInterpolate0YMB;
    pPBB->pMotionCompBuffer_EMB[1] = ppxliInterpolate1YMB;

    GetBackUpDiffMV_BVOP(pBMainLoop,iMBX,iMBY,pPBB);

	if(iMBX == 2)
		iMBX = iMBX;

    {
       I32_WMV iMask = pBMainLoop->m_iMBTypeMask[pmbmd->m_mbType-1];
       U8_WMV * pTbl = pBMainLoop->m_RndTbl;
       for(i = 0; i < 2; i++, iMask>>=1)
        {
           U32_WMV * pDest;
           I32_WMV iX, iY; 
           if(!(iMask &1))
               continue;     		

            pDest = pPBB->pMotionCompBuffer_EMB[i];
            result = g_MotionCompB_EMB_new (pBMainLoop,  (U8_WMV *)pDest, 40, i, Y_EMB,pPBB,iMBX, iMBY);     
            if(result)
                return -1;

			//{
			//	if(pPBB->m_pWMVDec->m_frameNum == 65  && iMBY == 0 && iMBX==3 && i==0)
			//	{
			//		VO_U8* tmp = (VO_U8*)pDest;			

			//		if(*(tmp+11*40) != 183)
			//		{
			//			iMBY = iMBY;
			//			printf("FUCK00  %d  %d\n",*(tmp+11*40),i);
			//		}			
			//	}
			//}
            iX = pPBB->pDiffMV_EMB_Bak[ i + (Y_EMB<<1)].Diff.I16[X_INDEX];
            iY = pPBB->pDiffMV_EMB_Bak[ i + (Y_EMB<<1)].Diff.I16[Y_INDEX];
            if(!pPBB->m_pWMVDec->m_bCodecIsWVC1)
            {
                iX = (iX + pTbl[iX & 3]) >> 1;
                iY = (iY + pTbl[iY & 3]) >> 1;        
                if (pPBB->m_bUVHpelBilinear) {
                    if (iX > 0) iX -= iX & 1 ;
                    else iX+=  iX & 1;
                    if (iY > 0) iY -= iY & 1 ;
                    else iY+=  iY & 1;
                }
            }
            iPred.I16[X_INDEX] = iX;
            iPred.I16[Y_INDEX] = iY;                 
            pPBB->pDiffMV_EMB_Bak[ i + (U_EMB<<1)].Diff.U32 =
            pPBB->pDiffMV_EMB_Bak[ i + (V_EMB<<1)].Diff.U32 = iPred.U32;       
            result = g_MotionCompB_EMB_new (pBMainLoop,  (U8_WMV *)(pDest + 160), 40, i, U_EMB,pPBB,iMBX, iMBY);
            if(result)
                return -1;
            result = g_MotionCompB_EMB_new (pBMainLoop,  (U8_WMV *)(pDest + 162), 40, i, V_EMB,pPBB,iMBX, iMBY);
            if(result)
                return -1;
        }
    }

    for (iblk = 0; iblk < 6; iblk++) 
    {
        U8_WMV*pRef0=NULL,*pRef1=NULL;
        //Bool_WMV bResidual = rgCBP2[iblk];
        CWMVMBModeBack *pMbMode =  pPBB->m_pMbMode + iMBX ;               
        I32_WMV iXformType= pMbMode->pBackUpFormType[iblk];
        iUV = iblk >> 2;
		if(!iUV)
			pDst = pPBB->m_ppxliCurrBlkBase[iblk] + iMBY*16*pPBB->m_iWidthPrev[iUV]+iMBX*16;
		else
			pDst = pPBB->m_ppxliCurrBlkBase[iblk] + iMBY*8*pPBB->m_iWidthPrev[iUV]+iMBX*8;
        if(pmbmd->m_mbType == DIRECT || pmbmd->m_mbType == INTERPOLATE )
        {
            pRef0 = (U8_WMV*)(pPBB->pMotionCompBuffer_EMB[FORWARD_EMB]+pPBB->m_pSSIMD_DstOffSet[iblk]);
            pRef1 = (U8_WMV*)(pPBB->pMotionCompBuffer_EMB[BACKWARD_EMB]+pPBB->m_pSSIMD_DstOffSet[iblk]);
        }
        else
        {
            pRef0 = (U8_WMV*)(pPBB->pMotionCompBuffer_EMB[FORWARD - pmbmd->m_mbType ]+ pPBB->m_pSSIMD_DstOffSet[iblk]);
            pRef1 = NULL;
        }

        if(iXformType == 5)
        {
            voVC1Copy_8x8(pDst,pPBB->m_iWidthPrev[iUV],pRef0,pRef1,40);

			//if(pPBB->m_pWMVDec->m_frameNum == 65  && iMBY == 0 && iMBX==3 && iblk == 2)
			//{
			//	VO_U8* tmp = pDst;			

			//	if(*(tmp+3*240) != 183)
			//	{
			//		iMBY = iMBY;
			//		printf("FUCK %d %d %d\n",*(tmp+3*240),*(pRef0+3*40),*(pRef1+3*40));
			//	}			
			//}

        }
        else if (iXformType == XFORMMODE_8x8) 
        {   
            UnionBuffer  *pSrc = &pMbMode->pBackUpErrorInter[iblk][0];   
#ifdef VOIDC_DC
			if(pMbMode->pBackUpDCTRunFlags[iblk][0] == 0)
				voVC1InvTrans_8x8_DC(pDst,pPBB->m_iWidthPrev[iUV],pRef0,pRef1,40,pSrc->i16,DCT_COEF1,DCT_COEF2); 
			else
#endif
				voVC1InvTrans_8x8(pDst,pPBB->m_iWidthPrev[iUV],pRef0,pRef1,40,pSrc->i16,DCT_COEF1,DCT_COEF2);     
        }
        else
        {
            I32_WMV iLoopMask = pMbMode->pBackUpLoopMask[iblk];
            I32_WMV iLoopTop = pMbMode->pBackUpLoopTop[iblk];
            I8_WMV iPattern = rgCBP2[iblk];
            U8_WMV*pRefTmp0=NULL,*pRefTmp1=NULL;

            for( i=0; i<iLoopTop; i++ ) 
            {   
                UnionBuffer  *pSrc = &pMbMode->pBackUpErrorInter[iblk][i];
				if(!iUV)
					pDst = pPBB->m_ppxliCurrBlkBase[iblk] + iMBY*16*pPBB->m_iWidthPrev[iUV]+iMBX*16;
				else
					pDst = pPBB->m_ppxliCurrBlkBase[iblk] + iMBY*8*pPBB->m_iWidthPrev[iUV]+iMBX*8;

                //pDst = pPBB->m_ppxliCurrBlkBase[iblk] + pPBB->m_iFrmMBOffset[iUV];
                pRefTmp0 = pRef0;
                pRefTmp1 = pRef1;
                if(iXformType == XFORMMODE_4x4)
                {
                    pDst += (i>>1)*pPBB->m_iWidthPrev[iUV]*4+(i&1)*4;
                    pRefTmp0 += (i>>1)*40*4+(i&1)*4;
                    pRefTmp1 = pRefTmp1 ? (pRefTmp1+(i>>1)*40*4+(i&1)*4):NULL; 
                    if ( iPattern & (iLoopMask>>i) )
					{
#ifdef VOIDC_DC
						if(pMbMode->pBackUpDCTRunFlags[iblk][i] == 0)
							voVC1InvTrans_4x4_DC(pDst,pPBB->m_iWidthPrev[iUV],pRefTmp0,pRefTmp1,40,pSrc->i16,DCT_COEF1,DCT_COEF2);
						else
#endif
							voVC1InvTrans_4x4(pDst,pPBB->m_iWidthPrev[iUV],pRefTmp0,pRefTmp1,40,pSrc->i16,DCT_COEF1,DCT_COEF2);
					}
                    else
                        voVC1Copy_4x4(pDst,pPBB->m_iWidthPrev[iUV],pRefTmp0,pRefTmp1,40);
                }
                else if(iXformType == XFORMMODE_8x4)
                {
                    pDst += (i&1)*pPBB->m_iWidthPrev[iUV]*4;
                    pRefTmp0 += (i&1)*40*4;
                    pRefTmp1 = pRefTmp1 ? (pRefTmp1+(i&1)*40*4):NULL;                               
                    if ( iPattern & (iLoopMask>>i) )
					{
#ifdef VOIDC_DC
						if(pMbMode->pBackUpDCTRunFlags[iblk][i] == 0)
							voVC1InvTrans_8x4_DC(pDst, pPBB->m_iWidthPrev[iUV],pRefTmp0,pRefTmp1,40,pSrc->i16,DCT_COEF1,DCT_COEF2);
						else
#endif
							voVC1InvTrans_8x4(pDst, pPBB->m_iWidthPrev[iUV],pRefTmp0,pRefTmp1,40,pSrc->i16,DCT_COEF1,DCT_COEF2);
					}
                    else
                        voVC1Copy_8x4(pDst,pPBB->m_iWidthPrev[iUV],pRefTmp0,pRefTmp1,40);
                }
                else if(iXformType == XFORMMODE_4x8)
                {
                    pDst  += (i&1)*4;
                    pRefTmp0 += (i&1)*4;
                    pRefTmp1 = pRefTmp1? (pRefTmp1+(i&1)*4):NULL;    
                    if ( iPattern & (iLoopMask>>i) )
					{
#ifdef VOIDC_DC
						if(pMbMode->pBackUpDCTRunFlags[iblk][i] == 0)
							voVC1InvTrans_4x8_DC(pDst, pPBB->m_iWidthPrev[iUV],pRefTmp0,pRefTmp1,40,pSrc->i16,DCT_COEF1,DCT_COEF2); 
						else
#endif
							voVC1InvTrans_4x8(pDst, pPBB->m_iWidthPrev[iUV],pRefTmp0,pRefTmp1,40,pSrc->i16,DCT_COEF1,DCT_COEF2); 
					}
                    else
                        voVC1Copy_4x8(pDst,pPBB->m_iWidthPrev[iUV],pRefTmp0,pRefTmp1,40);
                }                      
	        }
        }
    }

    return result;
}
#else
tWMVDecodeStatus DecodeBMB_EMB_Fast_MC(EMB_BMainLoop* pBMainLoop,
                                                            CWMVMBMode* pmbmd, 
                                                            I32_WMV iMBX, I32_WMV iMBY,EMB_PBMainLoop *pPBB )
{
    U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
    tWMVDecodeStatus result = WMV_Succeeded;
    I32_WMV  iblk,iUV;
    UMotion_EMB iPred;
    U8_WMV * pDst;
	
    U32_WMV ppxliInterpolate0YMB[160+80], ppxliInterpolate1YMB[160+80];    
    pPBB->m_pSSIMD_DstOffSet = pBMainLoop->m_SSIMD_DstLUT_B;
    //pBMainLoop->m_pMotionCompBuffer_EMB[FORWARD_EMB] = ppxliInterpolate0YMB;
    //pBMainLoop->m_pMotionCompBuffer_EMB[BACKWARD_EMB] = ppxliInterpolate1YMB;  

    pPBB->pMotionCompBuffer_EMB[0] = ppxliInterpolate0YMB;
    pPBB->pMotionCompBuffer_EMB[1] = ppxliInterpolate1YMB;

    GetBackUpDiffMV_BVOP(pBMainLoop,iMBX,iMBY,pPBB);

    {
       I32_WMV iMask = pBMainLoop->m_iMBTypeMask[pmbmd->m_mbType-1];
       int i;
       U8_WMV * pTbl = pBMainLoop->m_RndTbl;
       for(i = 0; i < 2; i++, iMask>>=1)
        {
           U32_WMV * pDest;
           I32_WMV iX, iY; 
           if(!(iMask &1))
               continue;           
            pDest = pPBB->pMotionCompBuffer_EMB[i];//pBMainLoop->m_pMotionCompBuffer_EMB[i];
            result = g_MotionCompB_EMB (pBMainLoop,  (U8_WMV *)pDest, i, Y_EMB,pPBB,iMBX, iMBY);     
            if(result)
                return -1;
            iX = pPBB->pDiffMV_EMB_Bak[ i + (Y_EMB<<1)].Diff.I16[X_INDEX];
            iY = pPBB->pDiffMV_EMB_Bak[ i + (Y_EMB<<1)].Diff.I16[Y_INDEX];
            if(!pPBB->m_pWMVDec->m_bCodecIsWVC1)
            {
                iX = (iX + pTbl[iX & 3]) >> 1;
                iY = (iY + pTbl[iY & 3]) >> 1;        
                if (pPBB->m_bUVHpelBilinear) {
                    if (iX > 0) iX -= iX & 1 ;
                    else iX+=  iX & 1;
                    if (iY > 0) iY -= iY & 1 ;
                    else iY+=  iY & 1;
                }
            }
            iPred.I16[X_INDEX] = iX;
            iPred.I16[Y_INDEX] = iY;                 
            pPBB->pDiffMV_EMB_Bak[ i + (U_EMB<<1)].Diff.U32 =
            pPBB->pDiffMV_EMB_Bak[ i + (V_EMB<<1)].Diff.U32 = iPred.U32;       
            result = g_MotionCompB_EMB (pBMainLoop,  (U8_WMV *)(pDest + 160), i, U_EMB,pPBB,iMBX, iMBY);
            if(result)
                return -1;
            result = g_MotionCompB_EMB (pBMainLoop,  (U8_WMV *)(pDest + 162), i, V_EMB,pPBB,iMBX, iMBY);
            if(result)
                return -1;
        }
    }

    {
        //VODCTCOEFF *coeffbuf = (pPBB->m_DctCoeff_Bak + iMBX );//+ iMBY*pBMainLoop->PB.m_uintNumMBX);
        I16_WMV *idcterror =0; ;//= coeffbuf->coeff;
        for (iblk = 0; iblk < 6; iblk++) 
        {
            Bool_WMV bResidual = rgCBP2[iblk];
            iUV = iblk >> 2;

			if(!iUV)
				pDst = pPBB->m_ppxliCurrBlkBase[iblk] + iMBY*16*pPBB->m_iWidthPrev[iUV]+iMBX*16;
			else
				pDst = pPBB->m_ppxliCurrBlkBase[iblk] + iMBY*8*pPBB->m_iWidthPrev[iUV]+iMBX*8;

            if(1){//**********/
                //int keepidct = 0;
                //if(keepidct)
                //{
                //    VODCTCOEFF *coeffbuf = (pPBB->m_DctCoeff_Bak + iMBX);
                //    I16_WMV *dcterror = coeffbuf->coeff;
                //    idcterror = dcterror+iblk*64;
                //}
                //else
                {
                    CWMVMBModeBack *pMbMode =  pPBB->m_pMbMode + iMBX ;//+ iMBY*pPBB->m_uintNumMBX;                
                    I32_WMV iXformType= pMbMode->pBackUpFormType[iblk];
                    if(iXformType == 5)
                        ;
                    else if (iXformType == XFORMMODE_8x8) 
                    {
                        UnionBuffer  *pTmpError = &pMbMode->pBackUpErrorInter[iblk][0];//&pMbMode->pBackUpError[iblk];
                        g_IDCTDec16_WMV3_Fun ( pTmpError, pTmpError, BLOCK_SIZE, pMbMode->pBackUpDCTHorzFlagsInter[iblk][0] );
                        idcterror =  pTmpError->i16;
                    }
                    else
                    {
                        I32_WMV i=0;
                        I32_WMV iIdx = iXformType-1;
                        I32_WMV iLoopMask = pMbMode->pBackUpLoopMask[iblk];
                        I32_WMV iLoopTop = pMbMode->pBackUpLoopTop[iblk];
                        I8_WMV iPattern = rgCBP2[iblk];
                        UnionBuffer  *pTmpError = &pMbMode->pBackUpErrorInter[iblk][0];//
                        //UnionBuffer  *pTmpError = &pMbMode->pBackUpError[iblk];

                        for( i=0; i<iLoopTop; i++ ) 
                        {                       
					        if ( iPattern & (iLoopMask>>i) ) 
                            {                           
                                //UnionBuffer *pDst = pPBB->m_ppxliErrorQ;
                                UnionBuffer  *pSrc = &pMbMode->pBackUpErrorInter[iblk][i];
                                result = (*pPBB->m_pDecodeIdct[iIdx])(pTmpError,pSrc,BLOCK_SIZE,pMbMode->pBackUpDCTHorzFlagsInter[iblk][i] ,i);
					        }
					        else
					        {
						        g_SubBlkIDCTClear_EMB(pTmpError, pMbMode->pBackUpIDCTClearOffset[iIdx][i]);
					        }
				        }
                        idcterror = pTmpError->i16;// pPB->m_ppxliErrorQ->i16;
                    }
                }
            }
            //**********/
            if(bResidual && idcterror)
            {
                if(pmbmd->m_mbType == DIRECT || pmbmd->m_mbType == INTERPOLATE )
                    g_AddErrorB_SSIMD_Fun(pDst, 
                                            pPBB->pMotionCompBuffer_EMB[FORWARD_EMB],
                                            pPBB->pMotionCompBuffer_EMB[BACKWARD_EMB],
                                            (I32_WMV*)idcterror,
                                            pPBB->m_iWidthPrev[iUV], 
                                            pPBB->m_pSSIMD_DstOffSet[iblk]);
                else
                {
#ifdef STABILITY
                    if(FORWARD - pmbmd->m_mbType > 1)
                        return -1;
#endif
                    g_AddError_SSIMD_Fun(pDst, 
                                                    pPBB->pMotionCompBuffer_EMB[FORWARD - pmbmd->m_mbType ]+ pPBB->m_pSSIMD_DstOffSet[iblk], 
                                                   (I32_WMV*)idcterror,
                                                   pPBB->m_iWidthPrev[iUV]);
                }
            }
            else
            {
                 if(pmbmd->m_mbType == DIRECT || pmbmd->m_mbType == INTERPOLATE)
                    g_AddNullB_SSIMD_Fun(pDst, 
                                            pPBB->pMotionCompBuffer_EMB[FORWARD_EMB],
                                            pPBB->pMotionCompBuffer_EMB[BACKWARD_EMB],
                                            pPBB->m_pSSIMD_DstOffSet[iblk], 
                                            pPBB->m_iWidthPrev[iUV]);
                else
                {
#ifdef STABILITY
                    if(FORWARD - pmbmd->m_mbType > 1)
                        return -1;
#endif
                    g_AddNull_SSIMD_Fun(pDst, 
                                                pPBB->pMotionCompBuffer_EMB[FORWARD - pmbmd->m_mbType] + pPBB->m_pSSIMD_DstOffSet[iblk], 
                                                pPBB->m_iWidthPrev[iUV]);
                 }
            }        
        }
    }

    return result;
}
#endif
/****************************************************************************************
  DecodeBMB : B macroblock level decoding
****************************************************************************************/


static I32_WMV s_iMBXformTypeLUT[16] = {
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
};
static I32_WMV s_iSubblkPatternLUT[16] = {0, 1, 2, 3, 1, 2, 3, 0, 0, 1, 2, 3, 1, 2, 3, 0};

/********************************************************************************
 decodeMBOverheadOfPVOP_WMV3_EMB : WMV3 macroblock header decoding for B frames
********************************************************************************/

//#pragma code_seg (EMBSEC_BML)

tWMVDecodeStatus decodeMBOverheadOfBVOP_WMV3_EMB (EMB_BMainLoop * pBMainLoop,CWMVMBMode* pmbmd, I32_WMV iMBX, I32_WMV iMBY,EMB_PBMainLoop *pPBB)
{
    I32_WMV iCBPCY = 0;//, iPos=0, iTemp=0;
    Bool_WMV bSendTableswitchMode = FALSE;
    Bool_WMV bSendXformswitchMode = FALSE;

    CDiffMV_EMB *pDiffMV = pPBB->m_pDiffMV_EMB ;

#ifdef STABILITY
    if(pPBB->m_pDiffMV_EMB == NULL)
        return -1;
#endif

    pmbmd->m_iQP = pPBB->m_iQP;// starting QP - maybe changed by DQUANT      
    pmbmd->m_dctMd = INTER;
    pmbmd->m_iDCTTable_MB_Index = 0;
    pmbmd->m_bBlkXformSwitchOn = FALSE;


    pmbmd->m_chMBMode = MB_1MV; // always for B  
      
    if (pBMainLoop->m_iDirectCodingMode == SKIP_RAW) 
        pmbmd->m_mbType = (MBType)BS_getBit(pPBB->m_pbitstrmIn);  
    
    if (pPBB->m_iSkipbitCodingMode == SKIP_RAW) {
        pmbmd->m_bSkip = BS_getBit(pPBB->m_pbitstrmIn);
    }
    
    bSendTableswitchMode = FALSE;
    bSendXformswitchMode = FALSE;

    {
        int iblk=0,Blocks=0;
        if( pmbmd->m_chMBMode == MB_1MV)
            Blocks = 1;
        else
            Blocks = 4;
        for(iblk=0;iblk<Blocks;iblk++)
        {
            I32_WMV iY = (iMBY<<1) + (iblk>>1);

            pPBB->m_pWMVDec->xblock[iblk] = (iMBX <<1) + (iblk&1);
            pPBB->m_pWMVDec->yblock[iblk] = (iMBY <<1) + (iblk>>1);

            pPBB->m_pWMVDec->bTopBndry[iblk] = 0;            
            if (!(iY & 1))
                pPBB->m_pWMVDec->bTopBndry[iblk] = ((iY == 0) || pPBB->m_pWMVDec->m_pbStartOfSliceRow [iY >> 1]);
        } 
    }
    
    // 1MV - read motion vector    
    if (pmbmd->m_mbType == DIRECT)
        pDiffMV->iLast = 1;
    else {
        if (!pmbmd->m_bSkip)
            decodeMV_V9_EMB(pPBB, pPBB->m_pHufMVTable_V9,  pDiffMV);    
        
        if (pmbmd->m_mbType == (MBType)0)
        {
            if (pPBB->m_pDiffMV_EMB->iIntra)
                pmbmd->m_mbType = BACKWARD;
            else if (BS_getBit(pPBB->m_pbitstrmIn) == 0)
                pmbmd->m_mbType = pBMainLoop->m_mbtShortBSymbol;
            else if (BS_getBit(pPBB->m_pbitstrmIn) == 0)
                pmbmd->m_mbType = pBMainLoop->m_mbtLongBSymbol;
            else
                pmbmd->m_mbType = INTERPOLATE;
        }
    }
    
    if (pmbmd->m_bSkip) {
        memset (pmbmd->m_rgbCodedBlockPattern2, 0, 6 * sizeof (U8_WMV));
        return WMV_Succeeded;
    }
    
    // skip to end if last = 0
    if (pDiffMV->iLast == 0) {
        pmbmd->m_bCBPAllZero = TRUE;
        if (pPBB->m_bDQuantOn && pDiffMV->iIntra) {
            if (WMV_Succeeded != decodeDQuantParam (pPBB->m_pWMVDec, pmbmd)) {
                return WMV_Failed;
            }
        }
        // One MV Intra AC Prediction Flag
        if (pDiffMV->iIntra)
            pmbmd->m_rgbDCTCoefPredPattern2 [ALLBLOCK] = (U8_WMV)BS_getBit(pPBB->m_pbitstrmIn);              
        goto End;
    }
    
    if (pmbmd->m_mbType == INTERPOLATE)
    {
        decodeMV_V9_EMB(pPBB, pPBB->m_pHufMVTable_V9,  (pDiffMV+1));    
        
        // INTRA is invalid in INTERPOLATE mode
        if ((pDiffMV + 1)->iIntra || pDiffMV->iIntra)
            return WMV_Failed;
        
        // skip to end if last = 0
        if ((pDiffMV + 1)->iLast == 0) {
            pmbmd->m_bCBPAllZero = TRUE;
            goto End;
        }
    }
    
    bSendTableswitchMode = pPBB->m_bDCTTable_MB;
    bSendXformswitchMode = pPBB->m_bMBXformSwitching && !pDiffMV->iIntra;
    // One MV Intra AC Prediction Flag
    if (pDiffMV->iIntra)
        pmbmd->m_rgbDCTCoefPredPattern2 [ALLBLOCK] = (U8_WMV)BS_getBit(pPBB->m_pbitstrmIn);              
    
    // read CBP
    iCBPCY = Huffman_WMV_get(pPBB->m_pHufNewPCBPCYDec, pPBB->m_pbitstrmIn);
    
    // decode DQUANT
    if (pPBB->m_bDQuantOn && WMV_Succeeded != decodeDQuantParam (pPBB->m_pWMVDec, pmbmd))
        return WMV_Failed;
    
    pmbmd->m_bCBPAllZero = FALSE;
    
    if (bSendTableswitchMode) {
        I32_WMV iDCTMBTableIndex = BS_getBit(pPBB->m_pbitstrmIn);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit(pPBB->m_pbitstrmIn); 
#ifdef STABILITY
        if(iDCTMBTableIndex<0 || iDCTMBTableIndex >2)
            return -1;
#endif
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }
    
End:
    if (bSendXformswitchMode)
    {
        I32_WMV iIndex = Huffman_WMV_get(pPBB->m_pHufMBXformTypeDec, pPBB->m_pbitstrmIn);
        pmbmd->m_bBlkXformSwitchOn = (iIndex < 8);
        pmbmd->m_iMBXformMode = s_iMBXformTypeLUT[iIndex];
        pmbmd->m_iSubblockPattern = s_iSubblkPatternLUT[iIndex];
        pmbmd->m_bFirstBlockDecoded = FALSE;
    }
    
    setCodedBlockPattern2 (pmbmd, Y_BLOCK1, iCBPCY);

    if(pPBB->m_bHalfPelMV)
    {
        pPBB->m_pDiffMV_EMB->Diff.I16[0] <<= 1;
        pPBB->m_pDiffMV_EMB->Diff.I16[1] <<= 1;
        (pPBB->m_pDiffMV_EMB+1)->Diff.I16[0] <<= 1;
        (pPBB->m_pDiffMV_EMB+1)->Diff.I16[1] <<= 1;
    }
    
    return WMV_Succeeded;
}

//#pragma code_seg (EMBSEC_BML)
Void_WMV PredictMV_EMB (EMB_BMainLoop  * pBMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk,EMB_PBMainLoop *pPBB,
                        U32_WMV ncols, U32_WMV nrows)
{
//    I32_WMV sum, i;
    I32_WMV iNumBlockX = pPBB->m_iNumBlockX;
    I32_WMV iIndex2 = pPBB->m_rgiCurrBaseBK[1] + pPBB->m_rgiBlkIdx[iblk];  //iY * iNumBlockX + iX;
    UMotion_EMB * pMotion = pBMainLoop->m_pMotionCurr;
    UMotion_EMB iVa, iVb, iVc, iPred, uiPred1;
    I32_WMV  iBlocks;
    I32_WMV iIndex = iIndex2 - 1;
    
#ifdef STABILITY
    if(pPBB->m_rgiBlkIdx[iblk] != 0)
        return;
#endif

    if(pPBB->m_pWMVDec->bTopBndry[iblk])
    {
        if(iX)
            iPred.U32 = pMotion[iIndex].U32;
        else
            iPred.U32 = 0;

        if (iPred.U32 == IBLOCKMV) {
            pPBB->m_iPred.I32 = 0;
            return;
        }

        goto CLAMP;
    }



    iX >>=1;

    iNumBlockX = iNumBlockX >>1;
    // find motion predictor
    iVc.U32 = 0;
    if (iX) 
    {
        // left block
        iVc.U32 = pMotion[iIndex].U32;
    }
    
    iIndex = iIndex2 - (iNumBlockX);
    
    
    // top block
    iVa.U32 = pMotion[iIndex].U32;
    
    iIndex += ((iX < (iNumBlockX - 1)) << 1) - 1;
    
    iVb.U32 = pMotion[iIndex].U32;


    iBlocks = 0;
    if(iVa.U32 == IBLOCKMV) 
        iBlocks ++;
    if(iVb.U32 == IBLOCKMV)
        iBlocks ++;
    if(iVc.U32 == IBLOCKMV)
        iBlocks ++;


    if(iBlocks > 0)
        goto nonZeroIBlocks;

nonZero1IBlocksRet:

        iPred.U32 = medianof3 (iVa.I32, iVb.I32,iVc.I32)>>16; 
        
        uiPred1.U32 = medianof3 (iVa.I32<<16, iVb.I32<<16, iVc.I32<<16); 
        
        iPred.U32 <<= 16;
        
        iPred.U32 |= ((U32_WMV)uiPred1.U32) >>16;
CLAMP:
        if(pPBB->m_pWMVDec->m_bCodecIsWVC1)//0816
        {
            int X =  (short)(iPred.U32&0xffff) ;//*iXMotion;
		    int Y = (short)(iPred.U32>>16);//*iYMotion;
		    int IX = ncols;
		    int IY = nrows;
		    int Width  = pPBB->m_pWMVDec->m_uintNumMBX;
		    int Height = pPBB->m_pWMVDec->m_uintNumMBY;
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

            iPred.U32 = (X&0xffff)|(Y<<16);

            pPBB->m_iPred.I32 = iPred.U32;

		    //*iXMotion = X;
            //*iYMotion = Y;
        }
        else
        {
            if(pPBB->m_pWMVDec->m_bCodecIsWVC1)
                pPBB->m_iPred.I32 =  iPred.I32;
            else
                pPBB->m_iPred.I32 = iPred.U32 = PredictBMVPullBackCheck_EMB(pBMainLoop, 1, iPred.I32,pPBB);
        }
        
        return;
    

nonZeroIBlocks:

  
    if (iBlocks == 1) {
            if (iVa.U32 == IBLOCKMV)
                iVa.U32 = 0;
            else if (iVb.U32== IBLOCKMV)
                iVb.U32= 0;
            else if (iVc.U32 == IBLOCKMV)
                iVc.U32= 0;
            goto nonZero1IBlocksRet;
    }
    else
    {
         pPBB->m_iPred.U32 = iPred.U32  = 0;
    }
    
}



//#pragma code_seg (EMBSEC_DEF)
I32_WMV PredictBMVPullBack_EMB(EMB_BMainLoop  * pBMainLoop,  I32_WMV bBfrm, I32_WMV iPred,EMB_PBMainLoop *pPBB)
{
    
    I32_WMV iX1, iY1;
    I32_WMV iBFrm2 = (bBfrm == 2);
    I32_WMV iXPred = (iPred<<16)>>16;
    I32_WMV iYPred = iPred >>16;
    I32_WMV iX, iY;
    I32_WMV iNumBlockX ;
    I32_WMV iNumBlockY ;
    I32_WMV iMinCoordinate, iMaxX, iMaxY;
    Bool_WMV bUV = 0;


    bBfrm = bBfrm &1;

    iNumBlockX = pPBB->m_iNumBlockX >>bBfrm;
    iNumBlockY = pPBB->m_iNumBlockY >>bBfrm;

    if(bBfrm == 0)
        bUV = 1;
    
    
    iX = pPBB->m_iBlkMBBase[X_INDEX]>>bBfrm;
    iY = pPBB->m_iBlkMBBase[Y_INDEX]>>bBfrm ;
    
    iX1 = (iX << 5) + iXPred;
    iY1 = (iY << 5 ) + iYPred;
    iMinCoordinate = ((-28 -bUV*4)<<bUV);  // -15 or -7 pixels
    iMaxX = (iNumBlockX << 5) - 4 + bUV*4;  // x8 for block x4 for qpel
    iMaxY = (iNumBlockY << 5) - 4 + bUV*4;
    
    if(iBFrm2 > 0)
    {
        iMinCoordinate += 4;
        iMaxX -= 4;
        iMaxY -= 4;
    }
    else
    {
        iX1 &= ~(bUV*((2<<1)-1));
        iY1 &= ~(bUV*((2<<1)-1));
    }

    
    if (iX1 < iMinCoordinate)
    {
        iXPred += iMinCoordinate - iX1;
    }
    else if (iX1 > iMaxX)
    {
        iXPred += iMaxX - iX1;
    }
    
    if (iY1 < iMinCoordinate)
    {
        iYPred += iMinCoordinate - iY1;
    }
    else if (iY1 > iMaxY)
    {

        iYPred += iMaxY - iY1;
    }
    
    iPred = (iXPred&0xffff) |  (iYPred <<16);
    
    return iPred;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV PredictBMVPullBackCheck_EMB(EMB_BMainLoop  * pBMainLoop,  I32_WMV bBfrm, I32_WMV iPred,EMB_PBMainLoop *pPBB)
{
    I32_WMV iShift = 5 - (bBfrm&1);
    I32_WMV iCord = (I32_WMV)(pPBB->m_iBlkMBBase[0] | (pPBB->m_iBlkMBBase[1]<<16)); //(*(I32_WMV *)(pPBB->m_iBlkMBBase));
    I32_WMV  iV1 = (iCord <<iShift) + (pBMainLoop->m_iMVMin[bBfrm]) + iPred;
    I32_WMV iV2 = pBMainLoop->m_iMVMax[bBfrm]  - (iCord <<iShift) -  iPred;
    I32_WMV iSign = (iPred &0x8000)<<1;


    iV1 -= iSign;
    
    if((iV1|iV2)&0x80008000)
    {
        iPred = PredictBMVPullBack_EMB(pBMainLoop, bBfrm, iPred,pPBB);
    }
    
    return iPred;
    
}


//#pragma code_seg (EMBSEC_BML)
Void_WMV PredictMVTop_EMB (EMB_BMainLoop  * pBMainLoop, I32_WMV iX,  Bool_WMV b1MV, 
                           I32_WMV iblk,EMB_PBMainLoop *pPBB,U32_WMV ncols, U32_WMV nrows)
{   
    UMotion_EMB * pMotion = pBMainLoop->m_pMotionCurr;
    UMotion_EMB  iPred;    
    iPred.U32 = 0;    
    if (iX != 0)
    {        
        I32_WMV iIndex = pPBB->m_rgiCurrBaseBK[1] + pPBB->m_rgiBlkIdx[iblk] -1;  //iY * iNumBlockX + iX;
        I32_WMV iTmp = pMotion[iIndex].I32;        
        if(iTmp != IBLOCKMV)
        {   
            if(pPBB->m_pWMVDec->m_bCodecIsWVC1)//0816
            {
                int X = pMotion[iIndex].I16[0];
		        int Y = pMotion[iIndex].I16[1];
		        int IX = ncols;
		        int IY = nrows;
		        int Width  = pPBB->m_pWMVDec->m_uintNumMBX;
		        int Height = pPBB->m_pWMVDec->m_uintNumMBY;
		        int Min;

		        IX = IX * 16 * 4 + X;
                IY = IY * 16 * 4 + Y;
                Width  = (Width  * 16 - 1)*4;
                Height = (Height * 16 - 1)*4;

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
                iPred.I16[0] = X;
                iPred.I16[1] = Y;
            }
            else      
            {
                iPred.U32 = PredictBMVPullBackCheck_EMB(pBMainLoop, 1,  iTmp, pPBB);
            }
        }        
    }   
    pPBB->m_iPred.I32 = iPred.I32;    
}

extern const I16_WMV gaSubsampleBicubic[];
//#pragma code_seg (EMBSEC_DEF)
Void_WMV InitEMBBMainLoopCtl(tWMVDecInternalMember *pWMVDec,EMB_PBMainLoop *pPBB)
{
    I32_WMV i,j;
    EMB_BMainLoop * pBMainLoop = &pWMVDec->m_EMB_BMainLoop;
    
    pPBB->m_pWMVDec = pWMVDec;
    
    pPBB->m_pDiffMV_EMB = pWMVDec->m_pDiffMV_EMB;
    pPBB->m_pbitstrmIn = pWMVDec->m_pbitstrmIn;
    
    pPBB->m_pMotion = pWMVDec->m_pMotion;
    pPBB->m_pMotionC = pWMVDec->m_pMotionC;

    pBMainLoop->m_pMotionArray[0] = pWMVDec->m_pMotion;
    pBMainLoop->m_pMotionArray[1] = pWMVDec->m_pMotionC;   

    pPBB->m_rgmv1_EMB = pWMVDec->m_rgmv1_EMB;
    
    pPBB->m_iNumBlockX = pWMVDec->m_uintNumMBX<<1;
    pPBB->m_iNumBlockY = pWMVDec->m_uintNumMBY<<1;
    pPBB->m_rgiBlkIdx[0] = 0;
    pPBB->m_rgiBlkIdx[1] = 1;
    pPBB->m_rgiBlkIdx[2] = pPBB->m_iNumBlockX;
    pPBB->m_rgiBlkIdx[3] = pPBB->m_iNumBlockX+1;
    
    pPBB->m_iWidthY = pPBB->m_iNumBlockX<<3;
    pPBB->m_iHeightY = pPBB->m_iNumBlockY<<3;
    pPBB->m_iWidthUV = pPBB->m_iNumBlockX<<2;
    pPBB->m_iHeightUV = pPBB->m_iNumBlockY<<2;
    
    pPBB->m_iWidthPrevY = pWMVDec->m_iWidthPrevY;
    pPBB->m_iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
    pPBB->m_iHeightPrevY = pWMVDec->m_iHeightPrevY;
    pPBB->m_iHeightPrevUV = pWMVDec->m_iHeightPrevUV;
    
    pPBB->m_iWidthPrev[0] = pPBB->m_iWidthPrevY;
    pPBB->m_iWidthPrev[1] = pPBB->m_iWidthPrevUV;
        
    pPBB->m_rgiCoefReconBuf = pWMVDec->m_rgiCoefReconBuf;
    pPBB->m_pX9dct = pWMVDec->m_pX9dct;
    pPBB->m_pX9dctUV[0] = pWMVDec->m_pX9dctU;
    pPBB->m_pX9dctUV[1]= pWMVDec->m_pX9dctV;

   
    pBMainLoop->m_iMBTypeMask[FORWARD-1] = 0x1;
    pBMainLoop->m_iMBTypeMask[BACKWARD-1] = 0x2;
    
    pBMainLoop->m_iMBTypeMask[INTERPOLATE-1] = 0x3;
    pBMainLoop->m_iMBTypeMask[DIRECT-1] = 0x3;
    
    pPBB->m_ppxliErrorQ = pWMVDec->m_ppxliErrorQ;
   

    pPBB->m_pDecodeIdct[XFORMMODE_8x4 -1] = g_IDCT8x4_WMV3_Fun;
    pPBB->m_pDecodeIdct[XFORMMODE_4x8 -1] = g_IDCT4x8_WMV3_Fun;
    pPBB->m_pDecodeIdct[XFORMMODE_4x4 -1] = g_IDCT4x4_WMV3_Fun;

    pPBB->m_pInterpolateBlock_00_new = g_InterpolateBlock_00_SSIMD_Fun_new;
   
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            pPBB->m_pInterpolateBlockBilinear [i][j] = g_InterpolateBlockBilinear_SSIMD_Fun;
            pPBB->m_pInterpolateBlockBilinear_new[i][j] = g_InterpolateBlockBilinear_SSIMD_Fun_new;
        }
    }


#if defined(_ARM_)  
    pPBB->m_pInterpolateBlockBilinear [0][2] = g_InterpolateBlockBilinear_SSIMD_01_Fun;
    pPBB->m_pInterpolateBlockBilinear [2][0] = g_InterpolateBlockBilinear_SSIMD_10_Fun;
    pPBB->m_pInterpolateBlockBilinear [2][2] = g_InterpolateBlockBilinear_SSIMD_11_Fun;
#endif

#ifdef VOARMV7
     for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            pPBB->m_pInterpolateBlockBilinear_new[i][j] = g_InterpolateBlockBilinear_SSIMD_Fun_new;
        }
    }
     pPBB->m_pInterpolateBlockBilinear_new[0][2] = g_InterpolateBlockBilinear_SSIMD_01_Fun_new;
     pPBB->m_pInterpolateBlockBilinear_new[2][0] = g_InterpolateBlockBilinear_SSIMD_10_Fun_new;
     pPBB->m_pInterpolateBlockBilinear_new[2][2] = g_InterpolateBlockBilinear_SSIMD_11_Fun_new;
#endif

    //pPBB->m_iFrmMBOffset[0] = 0;
    //pPBB->m_iFrmMBOffset[1] = 0;
    
    for(i = 0; i< 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            if(i == (XFORMMODE_8x4-1))
            {
                pPBB->m_uiIDCTClearOffset[i][j] = (j<<3);
                if(j == 1)   break;
            }
            if(i == (XFORMMODE_4x8-1))
            {
                pPBB->m_uiIDCTClearOffset[i][j] = j + 0xc0;
                if(j == 1)   break;
            }
            if(i == (XFORMMODE_4x4-1))
            {
                pPBB->m_uiIDCTClearOffset[i][j] =  ((j&2)<<2) + (j&1)+ 0x40;
                
            }
        }
    }
    

    {
        I32_WMV ii;
        U32_WMV  * pData32 = pBMainLoop->m_rgIntraMotionCompBuffer_EMB;
        for ( ii = 0; ii < 8; ii++)
        {
#if defined(VOARMV7) || defined(NEW_C)
            pData32[0] = pData32[1]  = pData32[2] = pData32[3]  = 0x80808080;
#else
            pData32[0] = pData32[1]  = pData32[5] = pData32[6]  = 0x00800080;
#endif
            pData32 += 10;
        }
    }
       

    pBMainLoop->m_RndTbl[0] = 0;
    pBMainLoop->m_RndTbl[1] = 0;
    pBMainLoop->m_RndTbl[2] = 0;
    pBMainLoop->m_RndTbl[3] = 1;

    pBMainLoop->m_SSIMD_DstLUT_B[0] = 0;
    pBMainLoop->m_SSIMD_DstLUT_B[1] = 2;
    pBMainLoop->m_SSIMD_DstLUT_B[2] = 80;
    pBMainLoop->m_SSIMD_DstLUT_B[3] = 80+2;
    pBMainLoop->m_SSIMD_DstLUT_B[4] = 160;
    pBMainLoop->m_SSIMD_DstLUT_B[5] = 160+2;


    pPBB->m_iBlkXformTypeLUT[0] =  XFORMMODE_8x8, 
    pPBB->m_iBlkXformTypeLUT[1] = pPBB->m_iBlkXformTypeLUT[2] = pPBB->m_iBlkXformTypeLUT[3] =  XFORMMODE_8x4;
    pPBB->m_iBlkXformTypeLUT[4] = pPBB->m_iBlkXformTypeLUT[5] = pPBB->m_iBlkXformTypeLUT[6]= XFORMMODE_4x8;
    pPBB->m_iBlkXformTypeLUT[7] =   XFORMMODE_4x4;
    
    pPBB->m_iSubblkPatternLUT[0] = 0;
    pPBB->m_iSubblkPatternLUT[1] = 1;
    pPBB->m_iSubblkPatternLUT[2] = 2;
    pPBB->m_iSubblkPatternLUT[3] = 3;
    pPBB->m_iSubblkPatternLUT[4] = 1;
    pPBB->m_iSubblkPatternLUT[5] = 2;
    pPBB->m_iSubblkPatternLUT[6] = 3;
    pPBB->m_iSubblkPatternLUT[7] = 0;
    
    pPBB->m_iNumCoefTbl[0] = 64;
    pPBB->m_iNumCoefTbl[1] = 32;
    pPBB->m_iNumCoefTbl[2] = 32;
    pPBB->m_iNumCoefTbl[3] = 0;
    pPBB->m_iNumCoefTbl[4] = 16;

    for (i=0; i<64; i++)
    {
        if (i < 8)
            pPBB->m_IDCTShortCutLUT[i] = 0;
        else
            pPBB->m_IDCTShortCutLUT[i] = 1 << (i & 0x7);
    }

    for(i = 0; i< 16; i++)
    {
        pPBB->m_rgiSubsampleBicubic[i] = (I8_WMV)gaSubsampleBicubic[i];
    }

    pPBB->m_rgiSubsampleBicubic[0] = 0x00;
    pPBB->m_rgiSubsampleBicubic[1] = 0x80;
    pPBB->m_rgiSubsampleBicubic[2] = 0x00;
    pPBB->m_rgiSubsampleBicubic[3] = 0x80;    
    //*(U32_WMV *)pPBB->m_rgiSubsampleBicubic = 0x80008000;

    pPBB-> m_uiffff07ff =  0xffff07ff;
    pPBB-> m_uiffff7fff =  0xffff7fff;
    pPBB-> m_uiffff1fff =  0xffff1fff;;
    pPBB-> m_uic000c000 = 0xc000c000;
    pPBB-> m_uif000f000 = 0xf000f000;
    pPBB-> m_ui00ff00ff = 0x00ff00ff;

     pBMainLoop->m_iMVMin[0] = 64 + (64<<16);
     pBMainLoop->m_iMVMax[0] = (pPBB->m_iNumBlockX << 5)  + (((pPBB->m_iNumBlockY << 5))<<16);;
     pBMainLoop->m_iMVMin[2] = 60 + (60<<16);
     pBMainLoop->m_iMVMax[2] = (pPBB->m_iNumBlockX << 5) -4 + (((pPBB->m_iNumBlockY << 5)-4)<<16);


     pBMainLoop->m_iMVMin[1] = 28 + (28<<16);
     pBMainLoop->m_iMVMax[1] = (pPBB->m_iNumBlockX << 4) - 4 + (((pPBB->m_iNumBlockY << 4) - 4)<<16);

    {
        I32_WMV iUV, b1MV;

    for(iUV=0; iUV<2; iUV++)
        for(b1MV=0; b1MV<2; b1MV++)
        {
            I32_WMV iSrcStride = pPBB->m_iWidthPrevY;

            I32_WMV iParam1 = 40, iParam2 = 20, iParam3 = 3;


            if(iUV == 1)
                iSrcStride = pPBB->m_iWidthPrevUV;

            pPBB->m_rgiNewVertTbl[iUV][b1MV][0] = 3*iSrcStride + (iSrcStride<<(3+b1MV)) - 1 ;
            pPBB->m_rgiNewVertTbl[iUV][b1MV][1] = (iParam1 <<(3+b1MV)) - iParam2;
            pPBB->m_rgiNewVertTbl[iUV][b1MV][2] = 3*iSrcStride + (iSrcStride<<(3+b1MV)) - iParam3 ;
            pPBB->m_rgiNewVertTbl[iUV][b1MV][3] = (iParam1 <<(3+b1MV)) + 16;
        }
    }
}


//#pragma code_seg (EMBSEC_DEF)
Void_WMV SetupEMBBMainLoopCtl(tWMVDecInternalMember *pWMVDec,EMB_PBMainLoop *pPBB)
{
     EMB_BMainLoop * pBMainLoop = &pWMVDec->m_EMB_BMainLoop;

     InitEMBBMainLoopCtl(pWMVDec,pPBB);

     pPBB->m_rgiMVDecLUT_EMB = pWMVDec->m_rgiMVDecLUT_EMB;
     pPBB->m_iQP= ((pWMVDec->m_iStepSize << 1) - 1) + pWMVDec->m_bHalfStep;
     pPBB->m_iSkipbitCodingMode = pWMVDec->m_iSkipbitCodingMode;
     pPBB->m_pHufMVTable_V9 = pWMVDec->m_pHufMVTable_V9;
     pPBB->m_bDQuantOn = pWMVDec->m_bDQuantOn;
     pPBB->m_bDCTTable_MB = pWMVDec->m_bDCTTable_MB;
     pPBB->m_bMBXformSwitching = pWMVDec->m_bMBXformSwitching;
     pPBB->m_bHalfPelMV =  (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL) 
             | (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR);

     
     pPBB->m_pHufNewPCBPCYDec = pWMVDec->m_pHufNewPCBPCYDec;
     pPBB->m_pHufMBXformTypeDec = pWMVDec->m_pHufMBXformTypeDec;
     pPBB->m_rgiCurrBaseBK[0] = 0;
     pPBB->m_rgiCurrBaseBK[1] = 0;
     pPBB->m_rgiBlkIdx[4] =
     pPBB->m_rgiBlkIdx[5] = 0;
     
     pPBB->m_iXMVRange = pWMVDec->m_iXMVRange;
     pPBB->m_iYMVRange = pWMVDec->m_iYMVRange;
     pPBB->m_iXMVFlag = pWMVDec->m_iXMVFlag;
     pPBB->m_iYMVFlag = pWMVDec->m_iYMVFlag;
     pPBB->m_iLogXRange = pWMVDec->m_iLogXRange;
     pPBB->m_iLogYRange = pWMVDec->m_iLogYRange;
     
     pPBB->m_iNumBFrames = pWMVDec->m_iNumBFrames;
     pPBB->m_bUVHpelBilinear = pWMVDec->m_bUVHpelBilinear;
     
     pPBB->m_iFrameXformMode = pWMVDec->m_iFrameXformMode;
     pPBB->m_prgDQuantParam = pWMVDec->m_prgDQuantParam;
     pPBB->m_pInterDCTTableInfo_Dec_Set = pWMVDec->m_pInterDCTTableInfo_Dec_Set;
     pPBB->m_pIntraDCTTableInfo_Dec_Set = pWMVDec->m_pIntraDCTTableInfo_Dec_Set;
     pPBB->m_ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
     pPBB->m_ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
     pPBB->m_iDCTHorzFlags= pWMVDec->m_iDCTHorzFlags;
     pPBB->m_iOverlap= pWMVDec->m_iOverlap;
     
     pPBB->m_pZigzagScanOrder= pWMVDec->m_pZigzagScanOrder;
     pPBB->m_iFilterType[0] =  pWMVDec->m_iFilterType;
     pPBB->m_iFilterType[1] = FILTER_BILINEAR;

     pPBB->m_iRndCtrl  = (pWMVDec->m_iRndCtrl <<16)|pWMVDec->m_iRndCtrl ;
     
	 //201318
 #ifndef VOARMV6
	 if(pWMVDec->bUseRef0Process == 1)//zou 30130307
	 {
		 pPBB->m_ppxliRefBlkBase[0] = pWMVDec->m_pfrmRef0Process->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
		 pPBB->m_ppxliRefBlkBase[1] = pWMVDec->m_ppxliRef1Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
		 pPBB->m_ppxliRefBlkBase[2] = pWMVDec->m_pfrmRef0Process->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
		 pPBB->m_ppxliRefBlkBase[3] = pWMVDec->m_ppxliRef1U + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
		 pPBB->m_ppxliRefBlkBase[4] = pWMVDec->m_pfrmRef0Process->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
		 pPBB->m_ppxliRefBlkBase[5] = pWMVDec->m_ppxliRef1V + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
	 }
	 else
#endif
	 {
		 pPBB->m_ppxliRefBlkBase[0] = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
		 pPBB->m_ppxliRefBlkBase[1] = pWMVDec->m_ppxliRef1Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
		 pPBB->m_ppxliRefBlkBase[2] = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
		 pPBB->m_ppxliRefBlkBase[3] = pWMVDec->m_ppxliRef1U + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
		 pPBB->m_ppxliRefBlkBase[4] = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
		 pPBB->m_ppxliRefBlkBase[5] = pWMVDec->m_ppxliRef1V + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
	 }
     
     pPBB->m_ppxliCurrBlkBase[0] = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
     pPBB->m_ppxliCurrBlkBase[1] = pPBB->m_ppxliCurrBlkBase[0] + 8;
     pPBB->m_ppxliCurrBlkBase[2] = pPBB->m_ppxliCurrBlkBase[0] + pWMVDec->m_iWidthPrevY*8;
     pPBB->m_ppxliCurrBlkBase[3] = pPBB->m_ppxliCurrBlkBase[0] + pWMVDec->m_iWidthPrevY*8 + 8;
     pPBB->m_ppxliCurrBlkBase[4] = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
     pPBB->m_ppxliCurrBlkBase[5] = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp; 

  
     pBMainLoop->m_bHalfPelBiLinear = pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR;
     pBMainLoop->m_iDirectCodingMode = pWMVDec->m_iDirectCodingMode;
     pBMainLoop->m_mbtShortBSymbol  = pWMVDec->m_mbtShortBSymbol;
     pBMainLoop->m_mbtLongBSymbol =  pWMVDec->m_mbtLongBSymbol;
     pBMainLoop->m_iScaleFactor = pWMVDec->m_iBNumerator * pWMVDec->m_iBFrameReciprocal;   //new

     pBMainLoop->m_rgfPredictMV_EMB[0] = PredictMVTop_EMB;
     pBMainLoop->m_rgfPredictMV_EMB[1] = PredictMV_EMB;
     pBMainLoop->m_rgfPredictMV_EMB[2] = PredictMV_EMB;
     pBMainLoop->m_ppPredictMV_EMB = pBMainLoop->m_rgfPredictMV_EMB;

     pPBB->m_iBlkMBBase[X_INDEX] = pPBB->m_iBlkMBBase[Y_INDEX] = 0;
     
     pPBB->m_iACColPredShift =  pWMVDec->m_iACColPredShift;
     pPBB->m_iACRowPredShift =  pWMVDec->m_iACRowPredShift;
     pPBB->m_iACColPredOffset =  pWMVDec->m_iACColPredOffset;
     pPBB->m_iACRowPredOffset =  pWMVDec->m_iACRowPredOffset;

     pPBB->m_pHufBlkXformTypeDec = pWMVDec->m_pHufBlkXformTypeDec;
     pPBB->m_pHuf4x4PatternDec = pWMVDec->m_pHuf4x4PatternDec;

     pPBB->m_pZigzag[ XFORMMODE_8x4 -1] = pWMVDec->m_p8x4ZigzagSSIMD;
     pPBB->m_pZigzag[ XFORMMODE_4x8 -1] = pWMVDec->m_p4x8ZigzagScanOrder;
     pPBB->m_pZigzag[ XFORMMODE_4x4 -1] = pWMVDec->m_p4x4ZigzagSSIMD;

     pPBB-> m_ui001f001fPiRndCtl = 0x001f001f + pPBB->m_iRndCtrl;
     pPBB-> m_ui000f000fPiRndCtl = 0x000f000f + pPBB->m_iRndCtrl;
     pPBB-> m_ui00070007PiRndCtl = 0x00070007 + pPBB->m_iRndCtrl;
     pPBB-> m_ui00030003PiRndCtl = 0x00030003 + pPBB->m_iRndCtrl;
     pPBB-> m_ui00200020MiRndCtl = 0x00200020 - pPBB->m_iRndCtrl;
     pPBB-> m_ui00400040MiRndCtl = 0x00400040 - pPBB->m_iRndCtrl;
     pPBB-> m_ui00080008MiRndCtl = 0x00080008 - pPBB->m_iRndCtrl;


     pPBB->m_uintNumMBX = pWMVDec->m_uintNumMBX;
}



