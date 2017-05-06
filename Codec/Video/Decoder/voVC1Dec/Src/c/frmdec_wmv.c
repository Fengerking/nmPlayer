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
#include "strmdec_wmv.h"
#include "codehead.h"
#include "localhuffman_wmv.h"
#include "repeatpad_wmv.h"

I32_WMV g_iNumShortVLC[] = {1, 1, 2, 1, 3, 1, 2};
I32_WMV g_iDenShortVLC[] = {2, 3, 3, 4, 4, 5, 5};
I32_WMV g_iNumLongVLC[] =  {3, 4, 1, 5, 1, 2, 3, 4, 5, 6, 1, 3, 5, 7};
I32_WMV g_iDenLongVLC[] =  {5, 5, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8};
extern const I32_WMV g_iBInverse[];
extern tYUV420Frame_WMV*  buf_seq_ctl(FIFOTYPE *priv, tYUV420Frame_WMV* frame , const VO_U32 flag);

I32_WMV DecodeFrac(tWMVDecInternalMember *pWMVDec, const I32_WMV iVal, const Bool_WMV bLong);
// Global tables

// Mapping array for QP index to QP. Used for 5QP deadzone quantizer.
#ifdef _NEW_QPMAPPING_
const I32_WMV g_iStepRemap[31] = {
         1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  
         3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 
        13, 14, 15, 16, 18, 20, 22, 24, 26, 28, 31};
#else
const I32_WMV g_iStepRemap[31] = {
         1,  1,  1,  1,  2,  3,  4,  5,  6,  7,  
         8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 
        18, 19, 20, 21, 22, 23, 24, 25, 27, 29, 31};
#endif

#ifdef WIN32
void voDumpYuv_I(tWMVDecInternalMember *pWMVDec,int nframes)
{
	unsigned char* Yplane = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
	unsigned char* Uplane = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	unsigned char* Vplane = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    char filename[250];

    sprintf(filename,"E:/MyResource/Video/clips/VC1/new/I_%d.yuv",nframes);


	//if(pWMVDec->m_nPframes == nframes && pWMVDec->m_tFrmType == PVOP)
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

#if 1
void voDumpFrameYuv(tWMVDecInternalMember *pWMVDec,int nframes)
{
	FILE*fp=NULL;
	int i=0;
	unsigned char* Yplane = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
	unsigned char* Uplane = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	unsigned char* Vplane = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    char filename[250];
#ifdef VOANDROID
	sprintf(filename,"/mnt/sdcard/smoothstream/IFrame/%d_%d_%d.yuv",nframes,(int)pWMVDec->m_uintNumMBX*16,(int)pWMVDec->m_uintNumMBY*16);
#else
	sprintf(filename,"E:/MyResource/Video/clips/VC1/ref/%d_I.yuv",nframes,(int)pWMVDec->m_uintNumMBX*16,(int)pWMVDec->m_uintNumMBY*16);
#endif
	fp=fopen(filename,"ab");

	for(i=0;i<pWMVDec->m_uintNumMBY*16;i++)
	{
		fwrite(Yplane, pWMVDec->m_uintNumMBX*16, 1, fp);
		Yplane+= pWMVDec->m_iWidthPrevY;
	}

	for(i=0;i<pWMVDec->m_uintNumMBY*8;i++)
	{
		fwrite(Uplane, pWMVDec->m_uintNumMBX*8, 1, fp);		
		Uplane += pWMVDec->m_iWidthPrevUV;				
	}

	for(i=0;i<pWMVDec->m_uintNumMBY*8;i++)
	{
		fwrite(Vplane, pWMVDec->m_uintNumMBX*8, 1, fp);
		Vplane += pWMVDec->m_iWidthPrevUV;
	}

	fclose(fp);
}
#endif
tWMVDecodeStatus WMVideoDecDecodeI (tWMVDecInternalMember *pWMVDec)
{
    tWMVDecodeStatus tWMVStatus;
    Bool_WMV bLeft,bTop,bLeftTop;
    U32_WMV ii,i;
    U32_WMV imbY, imbX;
    Bool_WMV bTopRowOfSlice = TRUE;
    U8_WMV* ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    CWMVMBMode* pmbmd_lastrow,*pmbmd_cur_ptr,*pmbmd_cur,*pmbmd_last;
    I16_WMV* piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
    I16_WMV** piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable;
    U32_WMV uiNumMBFromSliceBdry = pWMVDec->m_uintNumMBX + 1;

    pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;

    if (pWMVDec->m_iOverlap & 1) {
        for ( ii = 0; ii < (U32_WMV) pWMVDec->m_uintNumMB << 2; ii++)
            pWMVDec->m_pXMotion[ii] = IBLOCKMV;
        for (ii = 0; ii < (I32_WMV) pWMVDec->m_uintNumMB; ii++)
        {
            pWMVDec->m_pXMotionC[ii] = IBLOCKMV;
        }
    }

    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = 
    pWMVDec->m_ppInterDCTTableInfo_Dec[1] =
    pWMVDec->m_ppInterDCTTableInfo_Dec[2] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];

    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACIntraTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[1] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACIntraTableIndx[1]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[2] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACIntraTableIndx[2]];
    
    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];

    UpdateDCStepSize(pWMVDec, pWMVDec->m_iStepSize);

	//printf("pWMVDec->m_iOverlap =%d \n",pWMVDec->m_iOverlap);

    // decoder mb by mb
    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) 
	{
        U8_WMV* ppxliCodedY = ppxliCurrQY;
        U8_WMV* ppxliCodedU = ppxliCurrQU;
        U8_WMV* ppxliCodedV = ppxliCurrQV;
        Bool_WMV bNot1stRowInSlice;
        Bool_WMV bBottomRowOfSlice = (imbY == (pWMVDec->m_uintNumMBY - 1));

        if (!(imbY & 0x01)) { 
            piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
            piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable;
        }
#if 0 //201301
        else if (pWMVDec->m_bMultiresEnabled) {
            I32_WMV iWidthY = pWMVDec->m_rMultiResParams[0].iWidthY;
            piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred + (iWidthY>>4)*BLOCK_SIZE_TIMES2_TIMES6;
            piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable + (iWidthY>>4)*36;
        }
#endif

        bNot1stRowInSlice = (imbY != 0);

		//printf("pWMVDec->m_bSliceWMVA =%d \n",pWMVDec->m_bSliceWMVA);

        if (pWMVDec->m_bSliceWMVA) {            
            if (pWMVDec->m_pbStartOfSliceRow[imbY]) {
                I32_WMV result;
                pWMVDec->m_uiCurrSlice++;
                result = BS_flush (pWMVDec->m_pbitstrmIn);
                if(result)
                    return WMV_Failed;
                result = ReadSliceHeaderWMVA (pWMVDec, imbY);
                pWMVDec->m_bFirstEscCodeInFrame = TRUE;
                if (result != ICERR_OK)
                    return WMV_Failed;
            }           
            if (pWMVDec->m_pbStartOfSliceRow[imbY])
                bTopRowOfSlice = TRUE;
        }

        if (pWMVDec->m_bStartCode && pWMVDec->m_tFrmType != BIVOP) {
            if( SliceStartCode(pWMVDec, imbY)  != WMV_Succeeded)
                return WMV_Failed;
        }

#if 1       
        if((imbY&1) == 0)
        {
            pmbmd = pWMVDec->m_rgmbmd_cur; //pmbmd_cur
            pmbmd_lastrow = pWMVDec->m_rgmbmd_last;//pmbmd_last
        }
        else
        {
            pmbmd = pWMVDec->m_rgmbmd_last; //pmbmd_last
            pmbmd_lastrow = pWMVDec->m_rgmbmd_cur;  //pmbmd_cur      
        }

        for(i=0;i< pWMVDec->m_uintNumMBX;i++)
        {
            //int k=0;
            (pmbmd+i)->m_bSkip = (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_bSkip;
            (pmbmd+i)->m_mbType = (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_mbType;
            (pmbmd+i)->m_chMBMode = (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_chMBMode;
            (pmbmd+i)->m_iQP = (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_iQP;
            (pmbmd+i)->m_bBoundary = (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_bBoundary;
            (pmbmd+i)->m_bOverlapIMB = (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_bOverlapIMB;
            (pmbmd+i)->m_rgbDCTCoefPredPattern2[0] = (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_rgbDCTCoefPredPattern2[0];
        }
#endif

        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++)
        {
            uiNumMBFromSliceBdry++;
            //tWMVStatus = (*pWMVDec->m_pDecodeMBOverheadOfIVOP) 
			tWMVStatus = decodeMBOverheadOfIVOP_MSV(pWMVDec, pmbmd, pmbmd_lastrow,imbX, imbY); //sw:decodeMBOverheadOfIVOP_MSV() in headdec.c
            if (WMV_Succeeded != tWMVStatus)
                return tWMVStatus;
            
            if ((pWMVDec->m_iOverlap & 4) && !pmbmd->m_bOverlapIMB) 
            {
                 pWMVDec->m_iOverlap &= ~1;
                 pWMVDec->m_pXMotion[imbY * (I32_WMV) pWMVDec->m_uintNumMBX * 4 + imbX * 2] =
                 pWMVDec->m_pXMotion[imbY * (I32_WMV) pWMVDec->m_uintNumMBX * 4 + imbX * 2 + 1] =
                 pWMVDec->m_pXMotion[(imbY * 2 + 1) * (I32_WMV) pWMVDec->m_uintNumMBX * 2 + imbX * 2] =
                 pWMVDec->m_pXMotion[(imbY * 2 + 1) * (I32_WMV) pWMVDec->m_uintNumMBX * 2 + imbX * 2 + 1] =
                 pWMVDec->m_pXMotionC[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX] = 0;
                 //(pmbmd+imbX)->m_bIntra = 0;
             }
           
             bLeft = (imbX != 0) && (uiNumMBFromSliceBdry > 1);
             bTop = bNot1stRowInSlice && (uiNumMBFromSliceBdry > pWMVDec->m_uintNumMBX);
             bLeftTop = (imbX != 0) && bNot1stRowInSlice && (uiNumMBFromSliceBdry > (pWMVDec->m_uintNumMBX+1));
             //sw:DecodeIMBAcPred() in blkdec_wmv.c
             //tWMVStatus =/* (*pWMVDec->m_pDecodeIMBAcPred)*/
             tWMVStatus = DecodeIMBAcPred( pWMVDec,
                                                            pmbmd, 
                                                            pmbmd_lastrow, 
                                                            ppxliCodedY, ppxliCodedU, ppxliCodedV, 
                                                            piQuanCoefACPred, piQuanCoefACPredTable, 
                                                            bLeft, bTop, bLeftTop, 
                                                            (pWMVDec->m_bAdvancedProfile ? (imbX + (imbY << 16)) : imbX));  

             if (WMV_Succeeded != tWMVStatus)   
                 return tWMVStatus;
             
             if (pWMVDec->m_iOverlap & 4)
                 pWMVDec->m_iOverlap = 7;

             ppxliCodedY += MB_SIZE;
             ppxliCodedU += BLOCK_SIZE;
             ppxliCodedV += BLOCK_SIZE;                

             pmbmd++;
             pmbmd_lastrow++;
             piQuanCoefACPred += BLOCK_SIZE_TIMES2_TIMES6;
             piQuanCoefACPredTable += 36;
        } //for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++)

        if (pWMVDec->m_iOverlap & 1)
            OverlapMBRow (pWMVDec, imbY, ppxliCurrQY, ppxliCurrQU, ppxliCurrQV, bTopRowOfSlice, FALSE);

        bTopRowOfSlice = FALSE;
		
		if (imbY < pWMVDec->m_uintNumMBY - 1 && pWMVDec->m_pbStartOfSliceRow[imbY + 1])
			bBottomRowOfSlice = TRUE;

        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY; // point to the starting location of the first MB of each row
        ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row

        if (bBottomRowOfSlice && (pWMVDec->m_iOverlap & 1))
            OverlapMBRow (pWMVDec, imbY, ppxliCurrQY, ppxliCurrQU, ppxliCurrQV, FALSE, TRUE);

#if 1
		if (pWMVDec->m_bLoopFilter && imbY > 0 &&  pWMVDec->CpuNumber > 1)
		{
			U8_WMV* pY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
			U8_WMV* pU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
			U8_WMV* pV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;

			DeblockMBRow_IVOP (pWMVDec,
										  pY + (imbY-1)*pWMVDec->m_iMBSizeXWidthPrevY,
										  pU + (imbY-1)*pWMVDec->m_iBlkSizeXWidthPrevUV,
										  pV + (imbY-1)*pWMVDec->m_iBlkSizeXWidthPrevUV,
										  (imbY-1));
		}

		//repeat pre mb row
		if(imbY > 0 && pWMVDec->CpuNumber > 1 )
		{
			Repeatpad_mbrow(pWMVDec,imbY-1);
			pWMVDec->m_pfrmCurrQ->m_decodeprocess = imbY-1;
		}
#endif
    } //for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++)

	//deblock last mb row
#if 1
	if (pWMVDec->m_bLoopFilter && pWMVDec->CpuNumber > 1)
	{
		U8_WMV* pY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
		U8_WMV* pU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
		U8_WMV* pV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
		DeblockMBRow_IVOP (pWMVDec,
										  pY + (imbY-1)*pWMVDec->m_iMBSizeXWidthPrevY,
										  pU + (imbY-1)*pWMVDec->m_iBlkSizeXWidthPrevUV,
										  pV + (imbY-1)*pWMVDec->m_iBlkSizeXWidthPrevUV,
										  (imbY-1));
	}

	if(pWMVDec->CpuNumber > 1 )
		Repeatpad_mbrow(pWMVDec,imbY-1);
	if(pWMVDec->CpuNumber > 1 && pWMVDec->nInputFrameIndx > 1)
	{//tongbu
		volatile I32_WMV *ref_decodeprocess = &pWMVDec->m_pfrmRef0Q->m_decodeprocess;
		while(*ref_decodeprocess < pWMVDec->m_uintNumMBY+2) //control the finish order
			thread_sleep(0);			
	}
#endif

#ifndef MARK_POST
    if (pWMVDec->m_bLoopFilter && pWMVDec->CpuNumber <= 1)
	{
		DeblockSLFrame (pWMVDec,
                                      pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
                                      pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
                                      pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp);
	}
#endif

//#ifdef VOANDROID
	//voDumpYuv_I(pWMVDec, pWMVDec->m_nIframes);
	pWMVDec->m_nIframes++;
//#endif
     
    //pWMVDec->m_bRenderFromPostBuffer = FALSE_WMV;
//#ifdef TRACEVC1
//	voDumpFrameYuv(pWMVDec, 1);
//#endif
    
    return WMV_Succeeded;
}

Void_WMV UpdateDCStepSize(tWMVDecInternalMember *pWMVDec, I32_WMV iStepSize) 
{
    pWMVDec->m_iStepSize = iStepSize;
    if (pWMVDec->m_cvCodecVersion >= WMV3) 
    {        
        DQuantDecParam *pDQ;
        iStepSize = (2 * iStepSize - 1) + pWMVDec->m_bHalfStep;
        pDQ = &pWMVDec->m_prgDQuantParam [iStepSize];
        
        pWMVDec->m_bStepSizeIsEven = 0;
        pWMVDec->m_iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
        pWMVDec->m_iDoubleStepSize = pDQ->iDoubleStepSize;
		if (pDQ->iDCStepSize == 0)
			return;
		else
			pWMVDec->m_iDCStepSize = pWMVDec->m_iDCStepSizeC = pDQ->iDCStepSize;
        
        if (pWMVDec->m_iOverlap & 1) {

            pWMVDec->m_pAvgQuanDctCoefDec [0] = pWMVDec->m_pAvgQuanDctCoefDec [8] = 0;
            pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[8] = 0;
        }
        else {     
            pWMVDec->m_pAvgQuanDctCoefDec [0] = pWMVDec->m_pAvgQuanDctCoefDec [8] = (1024 + (pWMVDec->m_iDCStepSize >> 1)) / pWMVDec->m_iDCStepSize;
            pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[8] = (1024 + (pWMVDec->m_iDCStepSizeC >> 1)) / pWMVDec->m_iDCStepSizeC;
        }
        return;
    }

    pWMVDec->m_bStepSizeIsEven = ((pWMVDec->m_iStepSize & 1) == 0);
    pWMVDec->m_iStepMinusStepIsEven = pWMVDec->m_iStepSize - pWMVDec->m_bStepSizeIsEven;
    pWMVDec->m_iDoubleStepSize = pWMVDec->m_iStepSize << 1;
    pWMVDec->m_iDCStepSize = pWMVDec->m_iDCStepSizeC = 8;

    //if (pWMVDec->m_cvCodecVersion >= MP43 || pWMVDec->m_cvCodecVersion == MP4S) {
    if (pWMVDec->m_cvCodecVersion >= WMV1) {  //zouzh
        if (pWMVDec->m_iStepSize <= 4)   {
            pWMVDec->m_iDCStepSize = 8;
            pWMVDec->m_iDCStepSizeC = 8;
            if(/*pWMVDec->m_bNewDCQuant &&*/ pWMVDec->m_iStepSize <=2)
                pWMVDec->m_iDCStepSize = pWMVDec->m_iDCStepSizeC = pWMVDec->m_iStepSize << 1;
        }
        else if (pWMVDec->m_cvCodecVersion >= WMV1){
            pWMVDec->m_iDCStepSize = pWMVDec->m_iDCStepSizeC = (pWMVDec->m_iStepSize >> 1) + 6;
        }
        else if (pWMVDec->m_iStepSize <= 8)  {
            pWMVDec->m_iDCStepSize = 2 * pWMVDec->m_iStepSize;
            pWMVDec->m_iDCStepSizeC = (pWMVDec->m_iStepSize + 13) >> 1;
        }
        else if (pWMVDec->m_iStepSize <= 24) {
            pWMVDec->m_iDCStepSize = pWMVDec->m_iStepSize + 8;
            pWMVDec->m_iDCStepSizeC = (pWMVDec->m_iStepSize + 13) >> 1;
        }
        else {
            pWMVDec->m_iDCStepSize = 2 * pWMVDec->m_iStepSize - 16;
            pWMVDec->m_iDCStepSizeC = pWMVDec->m_iStepSize - 6;
        }
        if (pWMVDec->m_cvCodecVersion >= WMV1) { //zouzh
            pWMVDec->m_pAvgQuanDctCoefDec [0] = pWMVDec->m_pAvgQuanDctCoefDec [8] = (1024 + (pWMVDec->m_iDCStepSize >> 1)) / pWMVDec->m_iDCStepSize;
            pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[8] = (1024 + (pWMVDec->m_iDCStepSizeC >> 1)) / pWMVDec->m_iDCStepSizeC;
        }
    }
}


tWMVDecodeStatus WMVideoDecDecodeClipInfo (tWMVDecInternalMember *pWMVDec)
{
    if (pWMVDec->m_cvCodecVersion >= WMV2) {
        if (pWMVDec->m_bXintra8Switch)
            pWMVDec->m_bXintra8 = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    }
    return WMV_Succeeded;
}


tWMVDecodeStatus setSliceCode (tWMVDecInternalMember *pWMVDec, I32_WMV iSliceCode)
{
    // setSliceCode 
    if (!pWMVDec->m_fPrepared) 
    { 
        pWMVDec->m_fPrepared = TRUE_WMV;
        return WMV_Succeeded;
    }
    else
        return WMV_Succeeded;
}

/********************************************************************************
  X9SkipbitDecode2 - used for norm2 and diff2 skipbit modes
********************************************************************************/
Void_WMV X9SkipbitDecode2 (CWMVMBMode* pmbmd, I32_WMV iNumMB,
                              CInputBitStream_WMV * pIn)
{
    I32_WMV i;

    if (iNumMB & 1)
        (pmbmd++)->m_bSkip = BS_getBit(pIn);

    for ( i = iNumMB & 1; i < iNumMB; i += 2) {
        if (BS_getBit(pIn)) {
            if (BS_getBit(pIn))
                pmbmd->m_bSkip = (pmbmd + 1)->m_bSkip = 1;
            else {
                if (BS_getBit(pIn)) {
                    pmbmd->m_bSkip = 0;
                    (pmbmd + 1)->m_bSkip = 1;
                }
                else {
                    pmbmd->m_bSkip = 1;
                    (pmbmd + 1)->m_bSkip = 0;
                }
            }
        }
        else
            pmbmd->m_bSkip = (pmbmd + 1)->m_bSkip = 0;
        pmbmd += 2;
    }
}

/********************************************************************************
  DecodeNormDiff6Tile - used for norm6 and diff6 skipbit mode individual tiles
********************************************************************************/
tWMVDecodeStatus DecodeNormDiff6Tile (CInputBitStream_WMV * pIn, I32_WMV *a2On4Off,
                                       I32_WMV *aCount, I32_WMV *pi6)
{
    Bool_WMV bInvert = FALSE_WMV;
    I32_WMV  iCode;
    I32_WMV i6 = *pi6;

    // 4 stage decoding - read 1, 3, 1, 3 bits respectively
StartOfLocalFn6:
    // length 1 code
    if (BS_getBit(pIn)) {
        i6 = 0;
        goto EndOfLocalFn6;
    }
    iCode = BS_getBits(pIn,3);
    if (iCode > 1) {
        // length 4 codes
        i6 = 1 << (iCode - 2);
        goto EndOfLocalFn6;
    }
    if (iCode == 1) {
        // length 5 codes
        if (BS_getBit(pIn)) {
            // complementary codes - send to start
            if (bInvert)  // only one inversion possible
            {
                *pi6 = i6 ;
                return WMV_Failed;
            }
            bInvert = TRUE_WMV;
            goto StartOfLocalFn6;
        }
        else {
            // 3 on 3 off
            i6 = BS_getBits(pIn,5);
            iCode = aCount[i6];
            if (iCode == 2) {
                i6 |= 32;
            }
            else if (iCode == 3);  // do nothing
            else {
                *pi6 = i6 ;
                return WMV_Failed; // invalid symbol
            }
            goto EndOfLocalFn6;
        }
    }
    // length 8 codes
    iCode = BS_getBits(pIn,4);
    if (iCode == 0xff)  // non-existent codeword
    {
        *pi6 = i6 ;
        return WMV_Failed;
    }
    i6 = a2On4Off[iCode];

EndOfLocalFn6:
    if (bInvert)
        i6 ^= 0x3f;
    *pi6 = i6 ;
    return WMV_Succeeded;
}


/********************************************************************************
  DecodeNormDiff6 - used for norm6 and diff6 skipbit modes
********************************************************************************/
tWMVDecodeStatus DecodeNormDiff6 (tWMVDecInternalMember *pWMVDec)
{
    I32_WMV  i, j;
    // a2On4Off are consecutive indices which have 2 1s and 4 0s in binary rep
    static I32_WMV  a2On4Off[] = { 3, 5, 6, 9, 10, 12, 17, 18, 20, 24, 33, 34, 36, 40, 48 };
    // aCount is the number of 1s
    static I32_WMV   aCount[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
                      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5 };
    I32_WMV  iStart = 0, jStart = 0;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;

    if ((prvWMVModulus ((I32_WMV)(pWMVDec->m_uintNumMBY), 3) == 0) && prvWMVModulus ((I32_WMV)(pWMVDec->m_uintNumMBX), 3)) {
        // vertically oriented 2 x 3 tiles
        jStart = pWMVDec->m_uintNumMBX & 1;
        for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i += 3) {
            for (j = jStart; j < (I32_WMV) pWMVDec->m_uintNumMBX; j += 2) {

                I32_WMV  l = i * pWMVDec->m_uintNumMBX + j, i6;
                if (DecodeNormDiff6Tile (pWMVDec->m_pbitstrmIn, a2On4Off, aCount, &i6)
                    != WMV_Succeeded) {
                    // printf ("ERROR ");
                    return WMV_Failed;
                }
                (pmbmd + l)->m_bSkip = i6 & 1;      i6 >>= 1;
                (pmbmd + l + 1)->m_bSkip = i6 & 1;  i6 >>= 1;
                l += pWMVDec->m_uintNumMBX;
                (pmbmd + l)->m_bSkip = i6 & 1;      i6 >>= 1;
                (pmbmd + l + 1)->m_bSkip = i6 & 1;  i6 >>= 1;
                l += pWMVDec->m_uintNumMBX;
                (pmbmd + l)->m_bSkip = i6 & 1;      i6 >>= 1;
                (pmbmd + l + 1)->m_bSkip = i6 & 1;
            }
        }
    }
    else {
        // horizontally oriented 3 x 2 tiles
        iStart = pWMVDec->m_uintNumMBY & 1;
        jStart = prvWMVModulus ((I32_WMV)(pWMVDec->m_uintNumMBX), 3);
        for (i = iStart; i < (I32_WMV) pWMVDec->m_uintNumMBY; i += 2) {
            for (j = jStart; j < (I32_WMV) pWMVDec->m_uintNumMBX; j += 3) {
                I32_WMV  l = i * pWMVDec->m_uintNumMBX + j, i6;
                if (DecodeNormDiff6Tile (pWMVDec->m_pbitstrmIn, a2On4Off, aCount, &i6)
                    != WMV_Succeeded) {
                //  printf ("ERROR ");
                    return WMV_Failed;
                }
                (pmbmd + l)->m_bSkip = i6 & 1;      i6 >>= 1;
                (pmbmd + l + 1)->m_bSkip = i6 & 1;  i6 >>= 1;
                (pmbmd + l + 2)->m_bSkip = i6 & 1;  i6 >>= 1;
                l += pWMVDec->m_uintNumMBX;
                (pmbmd + l)->m_bSkip = i6 & 1;      i6 >>= 1;
                (pmbmd + l + 1)->m_bSkip = i6 & 1;  i6 >>= 1;
                (pmbmd + l + 2)->m_bSkip = i6 & 1;
            }
        }
    }
    // left column(s)
    for (j = 0; j < jStart; j++) {
        if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++)
                (pmbmd + i * pWMVDec->m_uintNumMBX + j)->m_bSkip = BS_getBit(pWMVDec->m_pbitstrmIn);
        }
        else
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++)
                (pmbmd + i * pWMVDec->m_uintNumMBX + j)->m_bSkip = 0;
    }
    // top row
    if (iStart) {
        if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
            for (j = jStart; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++)
                (pmbmd + j)->m_bSkip = BS_getBit(pWMVDec->m_pbitstrmIn);
        }
        else {
            for (j = jStart; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++)
                (pmbmd + j)->m_bSkip = 0;
        }
    }

    return WMV_Succeeded;
}


tWMVDecodeStatus DecodeSkipBitX9 (tWMVDecInternalMember *pWMVDec, I32_WMV iMode)
{
    I32_WMV   i, j;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    I32_WMV iCodingMode;

    // whether majority skip or nonskip
    Bool_WMV  bDiff = FALSE_WMV, bInvert = BS_getBit(pWMVDec->m_pbitstrmIn);

    switch (BS_getBits(pWMVDec->m_pbitstrmIn,2)) {
    case 2:
        // norm 2 coding
        iCodingMode = SKIP_NORM2;   
        X9SkipbitDecode2 (pmbmd, (I32_WMV) pWMVDec->m_uintNumMB, pWMVDec->m_pbitstrmIn);
        break;
    case 3:
        // norm 6 coding (not done yet)
        iCodingMode = SKIP_NORM6;
        if (DecodeNormDiff6 (pWMVDec) != WMV_Succeeded)
            return WMV_Failed;
        break;
    case 1:
        // row / col skip coding modes
        if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
            // col skipbit mode
            iCodingMode = SKIP_COL;
            for (j = 0; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++) {
                if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
                    for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++)
                        (pmbmd + i * pWMVDec->m_uintNumMBX + j)->m_bSkip = BS_getBit(pWMVDec->m_pbitstrmIn);
                }
                else
                    for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++)
                        (pmbmd + i * pWMVDec->m_uintNumMBX + j)->m_bSkip = 0;
            }
        }
        else {
            // row skipbit mode
            iCodingMode = SKIP_ROW;
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++) {
                if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
                    for (j = 0; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++)
                        (pmbmd + i * pWMVDec->m_uintNumMBX + j)->m_bSkip = BS_getBit(pWMVDec->m_pbitstrmIn);
                }
                else
                    for (j = 0; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++)
                        (pmbmd + i * pWMVDec->m_uintNumMBX + j)->m_bSkip = 0;
            }
        }
        break;
    default:
        if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
            iCodingMode = SKIP_DIFF2;
            // diff 2 coding
            bDiff = TRUE_WMV;
            X9SkipbitDecode2 (pmbmd, (I32_WMV) pWMVDec->m_uintNumMB, pWMVDec->m_pbitstrmIn);
            goto EndOfLoop;
        }
        else {
            // diff6, raw
            if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
                iCodingMode = SKIP_DIFF6;
                // diff6
                bDiff = TRUE_WMV;
                DecodeNormDiff6 (pWMVDec);
            }
            else {
                // raw
                iCodingMode = SKIP_RAW;
            }
        }
    }

EndOfLoop:
    if (bDiff) {
        CWMVMBMode* pmb1 = pWMVDec->m_rgmbmd;
        // undo differential coding
        for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++) {
            for (j = 0; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++) {
                I32_WMV  iPred;
                if (i + j == 0)
                    iPred = bInvert;
                else if (i == 0)
                    iPred = (pmb1 - 1)->m_bSkip;
                else if (j == 0)
                    iPred = (pmb1 - (I32_WMV) pWMVDec->m_uintNumMBX)->m_bSkip;
                else {
                    if ((pmb1 - 1)->m_bSkip == (pmb1 - (I32_WMV) pWMVDec->m_uintNumMBX)->m_bSkip)
                        iPred = (pmb1 - 1)->m_bSkip;
                    else
                        iPred = bInvert;
                }
                pmb1->m_bSkip ^= iPred;
                pmb1++;
            }
        }
    }
    else {
        // conditional inversion - only for nondifferential coding
        if (bInvert)
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++)
                (pmbmd + i)->m_bSkip ^= 1;
    }

    if (iMode == 0) {
        pWMVDec->m_iSkipbitCodingMode = iCodingMode;
    } else {
        if (iMode == 5) 
            pWMVDec->m_iOverlapIMBCodingMode = iCodingMode;
        else if (iMode == 4) 
            pWMVDec->m_iACPredIFrameCodingMode = iCodingMode;
        else if (iMode == 3)
                    pWMVDec->m_iDirectCodingMode = iCodingMode;
        else if (iMode == 2)
            pWMVDec->m_iFieldDctIFrameCodingMode = iCodingMode;

        else
        pWMVDec->m_iMVSwitchCodingMode = iCodingMode;
    }

    return WMV_Succeeded;
}

tWMVDecodeStatus WMVideoDecDecodeFrameHead (tWMVDecInternalMember *pWMVDec)
{
    I32_WMV i;
    Bool_WMV bNewRef = FALSE_WMV;
    I32_WMV stepDecoded;
    tWMVDecodeStatus result;
       
	pWMVDec->m_iRangeState = 0;  //zou 621
    pWMVDec->m_bIsBChangedToI = FALSE;    
    if (pWMVDec->m_cvCodecVersion == WMV3) 
    { 
        U32_WMV iFrmCntMod4 ;
        if (pWMVDec->m_bSeqFrameInterpolation) 
            pWMVDec->m_bInterpolateCurrentFrame = BS_getBit(pWMVDec->m_pbitstrmIn); //INTERPFRM
        iFrmCntMod4 = BS_getBits  ( pWMVDec->m_pbitstrmIn,2);                //FRMCNT 
        if (pWMVDec->m_cvCodecVersion == WMV3 && pWMVDec->m_bPreProcRange) 
            pWMVDec->m_iRangeState = BS_getBits( pWMVDec->m_pbitstrmIn, 1);     //RANGEREDFRM          main profile only
    }    
    
    if (pWMVDec->m_cvCodecVersion == WMV3)
    {
        pWMVDec->m_iRefFrameNum = -1;
        if (BS_getBits  ( pWMVDec->m_pbitstrmIn,1) == 1) //PTYPE
            pWMVDec->m_tFrmType = PVOP;
        else
        {
            if (pWMVDec->m_iNumBFrames == 0)
            {
                if (!pWMVDec->m_bRefLibEnabled)
                    pWMVDec->m_tFrmType = IVOP;
                else 
                { // pWMVDec->m_bRefLibEnabled == TRUE
                    if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
                        pWMVDec->m_tFrmType = IVOP; // non-key IVOP
                    else 
                    {
                        if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1) {
                            pWMVDec->m_tFrmType = IVOP; //key IVOP
                            RL_ResetLibrary(pWMVDec-> m_cReferenceLibrary);
                        }
                        else {
                            pWMVDec->m_tFrmType = PVOP;
                            bNewRef = TRUE;
                        }
                    }
                }
            }
            else { // pWMVDec->m_bBFrameOn == TRUE
                if (!pWMVDec->m_bRefLibEnabled) {
                    if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
                        pWMVDec->m_tFrmType = IVOP;
                    else
                        pWMVDec->m_tFrmType = BVOP;
                }
                else {
                    if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
                        pWMVDec->m_tFrmType = BVOP;
                    else {
                        if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
                            pWMVDec->m_tFrmType = IVOP; // non-key IVOP
                        else
                        {
                            if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1) {
                                pWMVDec->m_tFrmType = IVOP; // key IVOP
                                RL_ResetLibrary(pWMVDec-> m_cReferenceLibrary);
                            }
                            else {
                                pWMVDec->m_tFrmType = PVOP;
                                bNewRef = TRUE;
                            }
                        }
                    }
                }  
                
                if (pWMVDec->m_tFrmType == BVOP)
                {
                    I32_WMV iShort = 0, iLong = 0;
                    iShort = BS_getBits(pWMVDec->m_pbitstrmIn,3);
                    if (iShort == 0x7)
                    {
                        iLong = BS_getBits(pWMVDec->m_pbitstrmIn,4);
                        if (iLong == 0xe)       // "hole" in VLC
                            return WMV_Failed;                
                        if (iLong == 0xf)
                            pWMVDec->m_bIsBChangedToI = TRUE;
                        else
                        {
                            if(DecodeFrac(pWMVDec, iLong + 112, TRUE))
                                return WMV_Failed;
                        }
                    }
                    else
                    {
                        if(DecodeFrac(pWMVDec, iShort, FALSE))
                            return WMV_Failed;
                    }
                }
            }
        }
    }
    else
    {
        pWMVDec->m_tFrmType = (tFrameType_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_PRED_TYPE);
    }   

    if (pWMVDec->m_bIsBChangedToI)
        pWMVDec->m_tFrmType = BIVOP;    

    if (pWMVDec->m_tFrmType  != IVOP && pWMVDec->m_tFrmType  != PVOP && pWMVDec->m_tFrmType  != BVOP && pWMVDec->m_tFrmType  != BIVOP)
        return WMV_Failed;
    
    if ((pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP)) {//BF
        BS_getBits  ( pWMVDec->m_pbitstrmIn, 7);
        if (BS_invalid ( pWMVDec->m_pbitstrmIn))
            return WMV_CorruptedBits;
    }
    
    stepDecoded = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_QUANTIZER);//PQINDEX
    if (BS_invalid ( pWMVDec->m_pbitstrmIn) )
        return WMV_CorruptedBits;
    
    if (pWMVDec->m_cvCodecVersion >= WMV3)
    {
        pWMVDec->m_iQPIndex = stepDecoded;
        if (stepDecoded <= MAXHALFQP)
            pWMVDec->m_bHalfStep = BS_getBits(pWMVDec->m_pbitstrmIn, 1); //HALFQP
        else
            pWMVDec->m_bHalfStep = FALSE;
        
        if (pWMVDec->m_bExplicitFrameQuantizer)
            pWMVDec->m_bUse3QPDZQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1); //PQUANTIZER
        
        if (!pWMVDec->m_bExplicitQuantizer) { // Implicit quantizer
            if (pWMVDec->m_iQPIndex <= MAX3QP) {
                pWMVDec->m_bUse3QPDZQuantizer = TRUE;
                stepDecoded = pWMVDec->m_iQPIndex;
            }
            else {
                pWMVDec->m_bUse3QPDZQuantizer = FALSE;
                stepDecoded = g_iStepRemap[pWMVDec->m_iQPIndex - 1];
            }
        }
        else // Explicit quantizer
            stepDecoded = pWMVDec->m_iQPIndex;
        
        pWMVDec->m_iStepSize = stepDecoded;
        
        pWMVDec->m_iOverlap = 0;
        if (pWMVDec->m_bSequenceOverlap && !(pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bIsBChangedToI) ) {
            if (pWMVDec->m_iStepSize >= 9)
                pWMVDec->m_iOverlap = 1;
            else if (pWMVDec->m_bAdvancedProfile && (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP))
                pWMVDec->m_iOverlap = 7;
        }

        if (pWMVDec->m_iOverlap) {
            pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 0;
            pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 0;
        }
        else {
            pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 128;
            pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 128;
            
        }
        pWMVDec->m_prgDQuantParam = pWMVDec->m_bUse3QPDZQuantizer ? pWMVDec->m_rgDQuantParam3QPDeadzone : pWMVDec->m_rgDQuantParam5QPDeadzone;
    }
    
    pWMVDec->m_iStepSize = stepDecoded;
    
    if ( pWMVDec->m_iStepSize <= 0 || pWMVDec->m_iStepSize > 31) {
        return WMV_CorruptedBits;
    }
    
    if (pWMVDec->m_iQPIndex <= MAXHIGHRATEQP && pWMVDec->m_cvCodecVersion == WMV3) {
        pWMVDec->m_pInterDCTTableInfo_Dec_Set[0] = &pWMVDec->InterDCTTableInfo_Dec_HghRate;
        pWMVDec->m_pIntraDCTTableInfo_Dec_Set[0] = &pWMVDec->IntraDCTTableInfo_Dec_HghRate;
    }
    
    if (bNewRef && pWMVDec->m_bRefLibEnabled)
        pWMVDec->m_iRefFrameNum = BS_getBits  ( pWMVDec->m_pbitstrmIn,1);    

    if (pWMVDec->m_bExtendedMvMode) {       //MVRANGE  
        pWMVDec->m_iMVRangeIndex = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        if (pWMVDec->m_iMVRangeIndex)
            pWMVDec->m_iMVRangeIndex +=  BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        if (pWMVDec->m_iMVRangeIndex == 2)
            pWMVDec->m_iMVRangeIndex += BS_getBits( pWMVDec->m_pbitstrmIn, 1);
#ifdef STABILITY
    if(pWMVDec->m_iMVRangeIndex > 3)
        return WMV_CorruptedBits;
#endif
        SetMVRangeFlag (pWMVDec, pWMVDec->m_iMVRangeIndex);
    }
    
    if (pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP) { //RESPIC
        if (pWMVDec->m_cvCodecVersion == WMV3 && pWMVDec->m_bMultiresEnabled) {
            if (pWMVDec->m_bYUV411)
                pWMVDec->m_iResIndex = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
            else
                pWMVDec->m_iResIndex = BS_getBits( pWMVDec->m_pbitstrmIn, 2);
        }
    }
    
    if (pWMVDec->m_iResIndex != 0) {
        ResetParams(pWMVDec, pWMVDec->m_iResIndex);
        ResetDecoderParams(pWMVDec);
    }
    
    //X9
    if (pWMVDec->m_bYUV411)
    {
        // get frame level INTERLACE flag 
        Bool_WMV bFieldCodingOn = BS_getBits  ( pWMVDec->m_pbitstrmIn,1);         
        if (bFieldCodingOn) {
            if (DecodeSkipBitX9(pWMVDec, 1) != WMV_Succeeded) 
                return WMV_Failed;  
            if (pWMVDec->m_iMVSwitchCodingMode != SKIP_RAW) {
                for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++)
                    pWMVDec->m_rgmbmd[i].m_chMBMode = (pWMVDec->m_rgmbmd[i].m_bSkip ? MB_FIELD : MB_1MV);
            }
        } else { // set to MB_1MV to be safe for FORCE_PROGRESSIVE
            I32_WMV i;
            for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++)
                pWMVDec->m_rgmbmd[i].m_chMBMode = MB_1MV;
        }
    }
    
    if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) 
    {
        tWMVDecodeStatus tWMVStatus;
        tWMVStatus = setSliceCode (pWMVDec, pWMVDec->m_iSliceCode);        //I32_WMV iSliceCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_SLICE_SIZE);
        if (tWMVStatus != WMV_Succeeded)
            return tWMVStatus;
        tWMVStatus = WMVideoDecDecodeClipInfo (pWMVDec);
        if(BS_invalid ( pWMVDec->m_pbitstrmIn) || tWMVStatus != WMV_Succeeded)
            return tWMVStatus;
        //pWMVDec->m_bDCPred_IMBInPFrame = FALSE_WMV;

        if (!pWMVDec->m_bXintra8)
        { //zouzh
            if (pWMVDec->m_bDCTTable_MB_ENABLED)
                pWMVDec->m_bDCTTable_MB = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);

            if (!pWMVDec->m_bDCTTable_MB)
            {                
                pWMVDec->m_rgiDCTACInterTableIndx[0] = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                if (pWMVDec->m_rgiDCTACInterTableIndx[0])
                    pWMVDec->m_rgiDCTACInterTableIndx[0] += BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                pWMVDec->m_rgiDCTACIntraTableIndx[0] = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                if (pWMVDec->m_rgiDCTACIntraTableIndx[0])
                    pWMVDec->m_rgiDCTACIntraTableIndx[0] += BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                pWMVDec->m_rgiDCTACIntraTableIndx[1] = pWMVDec->m_rgiDCTACIntraTableIndx[2] = pWMVDec->m_rgiDCTACIntraTableIndx[0];
            }
            pWMVDec->m_iIntraDCTDCTable = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
        }
            // Interlace 411 I Frame Dquant
        if ((pWMVDec->m_bAdvancedProfile || pWMVDec->m_bYUV411) && pWMVDec->m_iDQuantCodingOn)
            DecodeVOPDQuant (pWMVDec, FALSE_WMV);
        else
            SetDefaultDQuantSetting(pWMVDec);
        
        pWMVDec->m_iRndCtrl = 1;
    }
    else 
    {
        if (pWMVDec->m_cvCodecVersion == WMV3) {
            result = decodeVOPHead_WMV3(pWMVDec);
            if(result !=  WMV_Succeeded)
                return result;
        }
                
        if (pWMVDec->m_cvCodecVersion >= WMV1) 
        { //zouzh
            if (pWMVDec->m_bDCTTable_MB_ENABLED)
                pWMVDec->m_bDCTTable_MB = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
            
            if (!pWMVDec->m_bDCTTable_MB){
                pWMVDec->m_rgiDCTACInterTableIndx[0] = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                if (pWMVDec->m_rgiDCTACInterTableIndx[0])
                    pWMVDec->m_rgiDCTACInterTableIndx[0] += BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
            }
            
            pWMVDec->m_rgiDCTACIntraTableIndx[0] = 
            pWMVDec->m_rgiDCTACIntraTableIndx[1] = 
            pWMVDec->m_rgiDCTACIntraTableIndx[2] = 
            pWMVDec->m_rgiDCTACInterTableIndx[1] = 
            pWMVDec->m_rgiDCTACInterTableIndx[2] = pWMVDec->m_rgiDCTACInterTableIndx[0];            
            
            pWMVDec->m_iIntraDCTDCTable = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
        }  
        if (pWMVDec->m_tFrmType == PVOP)
            pWMVDec->m_iRndCtrl ^= 0x01;
    }
    
    // sanity check for B frames - see if it matches previously decoded P
    if (BS_invalid ( pWMVDec->m_pbitstrmIn))
        return WMV_CorruptedBits;
    return WMV_Succeeded;
}

tWMVDecodeStatus WMVideoDecDecodeFrameHead2 (tWMVDecInternalMember *pWMVDec)
{
    Bool_WMV bNewRef = FALSE_WMV;
    I32_WMV iNumBitsFrameType;
    
	pWMVDec->m_iRangeState = 0; //zou 621
    pWMVDec->m_bIsBChangedToI = FALSE;
    
    if (pWMVDec->m_cvCodecVersion == WMV3) { 
        U32_WMV iFrmCntMod4 ;
        if (pWMVDec->m_bSeqFrameInterpolation) {
            pWMVDec->m_bInterpolateCurrentFrame = BS_getBit(pWMVDec->m_pbitstrmIn);
        }
        iFrmCntMod4 = BS_getBits  ( pWMVDec->m_pbitstrmIn,2);
              
        
        if (pWMVDec->m_cvCodecVersion == WMV3 && pWMVDec->m_bPreProcRange) {
            pWMVDec->m_iRangeState = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        }        
        
    }    
    
    if (pWMVDec->m_cvCodecVersion == WMV3)
    {
        pWMVDec->m_iRefFrameNum = -1;
        if (BS_getBits  ( pWMVDec->m_pbitstrmIn,1) == 1)
            pWMVDec->m_tFrmType = PVOP;
        else
        {
            if (pWMVDec->m_iNumBFrames == 0) 
            {
                if (!pWMVDec->m_bRefLibEnabled)
                    pWMVDec->m_tFrmType = IVOP;
                else { // pWMVDec->m_bRefLibEnabled == TRUE
                    if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
                        pWMVDec->m_tFrmType = IVOP; // non-key IVOP
                    else 
                    {
                        if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1) 
                        {
                            pWMVDec->m_tFrmType = IVOP; //key IVOP
                            RL_ResetLibrary(pWMVDec-> m_cReferenceLibrary);
                        }
                        else
                        {
                            pWMVDec->m_tFrmType = PVOP;
                            bNewRef = TRUE;
                        }
                    }
                }
            }
            else
            { // pWMVDec->m_bBFrameOn == TRUE
                if (!pWMVDec->m_bRefLibEnabled)
                {
                    if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
                        pWMVDec->m_tFrmType = IVOP;
                    else
                        pWMVDec->m_tFrmType = BVOP;
                }
                else 
                {
                    if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
                            pWMVDec->m_tFrmType = BVOP;
                    else 
                    {
                        if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
                            pWMVDec->m_tFrmType = IVOP; // non-key IVOP
                        else
                        {
                            if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1) 
                            {
                                pWMVDec->m_tFrmType = IVOP; // key IVOP
                                RL_ResetLibrary(pWMVDec-> m_cReferenceLibrary);
                            }
                            else 
                            {
                                pWMVDec->m_tFrmType = PVOP;
                                bNewRef = TRUE;
                            }
                        }
                    }
                } 
            }
        }
    }
    else
    {
        iNumBitsFrameType = (pWMVDec->m_cvCodecVersion == WMV2) ? NUMBITS_VOP_PRED_TYPE_WMV2 :  NUMBITS_VOP_PRED_TYPE;
        pWMVDec->m_tFrmType = (tFrameType_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, iNumBitsFrameType);
     }

    if (pWMVDec->m_bIsBChangedToI)
        pWMVDec->m_tFrmType = BIVOP;

    if (pWMVDec->m_tFrmType  != IVOP && pWMVDec->m_tFrmType  != PVOP && pWMVDec->m_tFrmType  != BVOP && pWMVDec->m_tFrmType  != BIVOP)
        return WMV_Failed;
        
    return WMV_Succeeded;
}

Void_WMV WMVideoDecSwapCurrAndRef (tWMVDecInternalMember *pWMVDec, Bool_WMV bSwapPostBuffers)
{
#ifdef SHAREMEMORY
	pWMVDec->m_pfrmRef0Q = pWMVDec->m_pfrmCurrQ;
	pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
    pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
    pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmCurrQ->m_pucVPlane;
    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
#else
    tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmCurrQ;
    pWMVDec->m_pfrmCurrQ = pWMVDec->m_pfrmRef0Q;
    pWMVDec->m_pfrmRef0Q = pTmp;
    pWMVDec->m_ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
    pWMVDec->m_ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
    pWMVDec->m_ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane;
    pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmRef0Q->m_pucYPlane;
    pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Q->m_pucUPlane;
    pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Q->m_pucVPlane;
    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
#endif
	
    // swap frame pointers for post processing
    if (bSwapPostBuffers)
        SwapPostAndPostPrev(pWMVDec);
}

Void_WMV SwapPostAndPostPrev(tWMVDecInternalMember *pWMVDec)
{
#ifndef SHAREMEMORY
    tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmPostQ;

    pWMVDec->m_pfrmPostQ = pWMVDec->m_pfrmPostPrevQ;
    pWMVDec->m_pfrmPostPrevQ = pTmp;

    pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
    pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
    pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;

    pWMVDec->m_ppxliPostPrevQY = pWMVDec->m_pfrmPostPrevQ->m_pucYPlane ;
    pWMVDec->m_ppxliPostPrevQU = pWMVDec->m_pfrmPostPrevQ->m_pucUPlane ;
    pWMVDec->m_ppxliPostPrevQV = pWMVDec->m_pfrmPostPrevQ->m_pucVPlane;
#endif
}


static I32_WMV s_pXformLUT[4] = {XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_4x8, XFORMMODE_4x4};


tWMVDecodeStatus decodeVOPHead_WMV3 (tWMVDecInternalMember *pWMVDec)
{
    tWMVDecodeStatus status;
    Bool_WMV bRepeat = FALSE_WMV;
    CWMVMBMode* pmbmd;
    I32_WMV tabindex=0,tabindex1 = 0;
    
    pWMVDec->m_bLuminanceWarp = FALSE_WMV;

    //printf("decodeVOPHead_WMV3 %d \n",pWMVDec->m_tFrmType);
   

    if (pWMVDec->m_tFrmType == BVOP ) 
    {//zou  --->/* Motion vector mode 1 */        
        if (pWMVDec->m_bYUV411) 
            pWMVDec->m_iX9MVMode = ALL_1MV;
        else 
            pWMVDec->m_iX9MVMode = BS_getBit( pWMVDec->m_pbitstrmIn) ? ALL_1MV : ALL_1MV_HALFPEL_BILINEAR;
    } 
    else
    {
        if (pWMVDec->m_bYUV411) 
        {
            pWMVDec->m_iX9MVMode = ALL_1MV;
            if (BS_getBit( pWMVDec->m_pbitstrmIn)) 
                pWMVDec->m_bLuminanceWarp = TRUE;
        }
        else 
        {
RepeatMVRead:
            if (pWMVDec->m_iStepSize > 12)
            { // low rate modes               
                if (BS_getBit(pWMVDec->m_pbitstrmIn))
                    pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL_BILINEAR;
                else if (BS_getBit(pWMVDec->m_pbitstrmIn))
                    pWMVDec->m_iX9MVMode = ALL_1MV;
                else if (BS_getBit(pWMVDec->m_pbitstrmIn))
                    pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL;
                else {
                    if (pWMVDec->m_tFrmType == BVOP)
                        pWMVDec->m_iX9MVMode = MIXED_MV;
                    else if (bRepeat || !BS_getBit(pWMVDec->m_pbitstrmIn))
                            pWMVDec->m_iX9MVMode = MIXED_MV;
                    else 
                    {
                        bRepeat = TRUE_WMV;
                        pWMVDec->m_bLuminanceWarp = TRUE_WMV;
                        goto RepeatMVRead;
                    }
                }
            }
            else 
            {
                // high rate modes  //zou -->vc1DECPIC_P_Picture_High_Rate_Motion_Vector_Mode_2
                if (BS_getBit(pWMVDec->m_pbitstrmIn))
                    pWMVDec->m_iX9MVMode = ALL_1MV;
                else if (BS_getBit(pWMVDec->m_pbitstrmIn))
                    pWMVDec->m_iX9MVMode = MIXED_MV;
                else if (BS_getBit(pWMVDec->m_pbitstrmIn))
                    pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL;
                else {
                    if (pWMVDec->m_tFrmType == BVOP )
                        pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL_BILINEAR;
                    else if (bRepeat || !BS_getBit(pWMVDec->m_pbitstrmIn))
                            pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL_BILINEAR;
                    else
                    {
                        bRepeat = TRUE_WMV;
                        pWMVDec->m_bLuminanceWarp = TRUE_WMV;
                        goto RepeatMVRead;
                    }
                }
            }
        
        }
        // reset variables that may not be read
        // pWMVDec->m_iGlobalXPad = 0;
        
        if ((pWMVDec->m_iWMV3Profile == WMV3_SIMPLE_PROFILE) && pWMVDec->m_bLuminanceWarp)
            return WMV_Failed; 
    }
    
    if (pWMVDec->m_bLuminanceWarp) 
    {
        pWMVDec->m_iLuminanceScale = BS_getBits(pWMVDec->m_pbitstrmIn,6);
        pWMVDec->m_iLuminanceShift = BS_getBits(pWMVDec->m_pbitstrmIn,6);
    }

    // if global warp exists, disable skip MB
    //memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) pWMVDec->m_uintNumMB);
    
    // decode 1/4 MV flag
    pmbmd = pWMVDec->m_rgmbmd;
    if (pWMVDec->m_iX9MVMode == MIXED_MV) 
    {
        I32_WMV i;
        if ((status=DecodeSkipBitX9(pWMVDec, 1)) != WMV_Succeeded)  // decoded into pWMVDec->m_bSkip bit
            return status;
        if (pWMVDec->m_iMVSwitchCodingMode != SKIP_RAW) {
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                (pmbmd + i) ->m_chMBMode = ((pmbmd + i) ->m_bSkip ? MB_4MV : MB_1MV);  // copied into pWMVDec->m_chMBMode
            }
        }
    }
    else
    {
        I32_WMV i;
        if (!pWMVDec->m_bYUV411) {
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                (pmbmd + i) ->m_chMBMode = MB_1MV;  // copied into pWMVDec->m_chMBMode
            }
        }
    }
    
    if (pWMVDec->m_bBFrameOn) {
        if (pWMVDec->m_tFrmType == BVOP) {
            if (DecodeSkipBitX9(pWMVDec,3) != WMV_Succeeded)   // decoded into m_bSkip bit
                return WMV_Failed;
            
            if (pWMVDec->m_iDirectCodingMode != SKIP_RAW) {
                I32_WMV i;
                pmbmd = pWMVDec->m_rgmbmd;
                for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                    if (pmbmd->m_bSkip)
                        pmbmd->m_mbType = DIRECT;
                    else
                        pmbmd->m_mbType = (MBType)0;
                    pmbmd++;
                }
            }
        }
    }
    
    // decode skip MB flag
    if ((status=DecodeSkipBitX9(pWMVDec, 0)) != WMV_Succeeded)  // decoded into pWMVDec->m_bSkip bit
        return status;  //zou -->/* Macroblock skip bitplane */
    
    //pWMVDec->m_bCODFlagOn = 1;
    // read MV and CBP codetable indices

    tabindex = BS_getBits(pWMVDec->m_pbitstrmIn,2);
    tabindex1 = BS_getBits(pWMVDec->m_pbitstrmIn,2);

    //printf("---->%d %d \n",tabindex,tabindex1);

#ifdef STABILITY
    if(tabindex>7 || tabindex1 >3)
        return -1;
#endif
    pWMVDec->m_pHufMVTable_V9 = pWMVDec->m_pHufMVTables[tabindex];  //zou   --->/* Motion vector huffman table */
    pWMVDec->m_pHufNewPCBPCYDec = pWMVDec->m_pHufPCBPCYTables[tabindex1]; //zou ---> /* Coded block pattern table */
    
    // 0: off
    // 100 : On All EdgeMB Profile
    // 101 : On 2D Panning Profile 
    // 110: On 1D Panning Profile
    // 1110:PerMB MQuant
    // 1111:Bilevel MQuant
    
    if (pWMVDec->m_iDQuantCodingOn)
        DecodeVOPDQuant (pWMVDec, TRUE_WMV); //zou --->vc1DECPIC_UnpackVOPDQUANTParams(pParams, pSeqParams, pBitstream);
    
    if (pWMVDec->m_bXformSwitch) {
        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
            pWMVDec->m_bMBXformSwitching = FALSE_WMV;
            tabindex = BS_getBits(pWMVDec->m_pbitstrmIn,2);
#ifdef STABILITY
            if(tabindex>3)
                return -1;
#endif
            pWMVDec->m_iFrameXformMode = s_pXformLUT[tabindex];  //zou -->/* Macroblock level transform type flag */  ???
        }
        else
            pWMVDec->m_bMBXformSwitching = TRUE_WMV;
    }
    else
        pWMVDec->m_bMBXformSwitching = FALSE_WMV;

    return WMV_Succeeded;
}



Void_WMV DecodeVOPDQuant (tWMVDecInternalMember *pWMVDec, Bool_WMV bPFrame)
{
    //Bool_WMV bNewDQuantSetting = FALSE;
    
    if (pWMVDec->m_iDQuantCodingOn == 2){
        pWMVDec->m_bDQuantOn = TRUE;
        pWMVDec->m_iPanning = 0x0f;
        pWMVDec->m_iDQuantBiLevelStepSize = decodeDQuantStepsize(pWMVDec, 1);
        return; 
    }

    pWMVDec->m_bDQuantBiLevel = FALSE_WMV;
    pWMVDec->m_iPanning = 0;
    pWMVDec->m_bDQuantOn = BS_getBit ( pWMVDec->m_pbitstrmIn); // turn on DQuant
    if (pWMVDec->m_bDQuantOn) {
        U32_WMV iPanningProfile;
        U32_WMV iPanningCode;
        iPanningProfile = BS_getBits ( pWMVDec->m_pbitstrmIn, 2);
        if (iPanningProfile == 0){
            pWMVDec->m_iPanning = 0x0f;
        }
        else if (iPanningProfile == 1){
            iPanningCode = BS_getBits ( pWMVDec->m_pbitstrmIn, 2);
            if (iPanningCode != 3)
                pWMVDec->m_iPanning = (0x03 << iPanningCode);
            else 
                pWMVDec->m_iPanning = 9; // BL
        }
        else if (iPanningProfile == 2){ // 
            iPanningCode = BS_getBits ( pWMVDec->m_pbitstrmIn, 2);
            pWMVDec->m_iPanning = (1 << iPanningCode);
        }
        else if (iPanningProfile == 3){
            pWMVDec->m_bDQuantBiLevel = BS_getBit( pWMVDec->m_pbitstrmIn);
        }
        if (pWMVDec->m_bDQuantBiLevel || pWMVDec->m_iPanning) {
            //pWMVDec->m_iDQuantBiLevelStepSize = BS_getBits ( pWMVDec, 5);
            pWMVDec->m_iDQuantBiLevelStepSize = decodeDQuantStepsize(pWMVDec, 1);
        }
    }
}


const I32_WMV g_iBInverse[] = { 256, 128, 85, 64, 51, 43, 37, 32 };


I32_WMV decodeSequenceHead_Advanced (tWMVDecInternalMember *pWMVDec, 
                                                        U32_WMV uiFormatLength,
                                                        I32_WMV *piPIC_HORIZ_SIZE, 
                                                        I32_WMV *piPIC_VERT_SIZE, 
                                                        I32_WMV *piDISP_HORIZ_SIZE, 
                                                        I32_WMV *piDISP_VERT_SIZE)
{
    I32_WMV iProfile ;
    
    I32_WMV iASPECT_RATIO = 0, iASPECT_HORIZ_SIZE =0, iASPECT_VERT_SIZE=0;
    I32_WMV iFRAMERATENR, iFRAMERATEDR, iFRAMERATEEXP;
    I32_WMV iCOLOR_PRIM, iTRANSFER_CHAR, iMATRIX_COEF;
    I32_WMV ibit_rate_exponent, ibuffer_size_exponent;
    I32_WMV ihrd_rate_N, ihrd_buffer_N;
    I32_WMV i;

    tWMVDimensions *tDimensions= &pWMVDec->tDimensions;
    tWMVHRD ptHRD[17];
	memset(tDimensions, 0, sizeof(tWMVDimensions));
	memset(ptHRD, 0, 17*sizeof(tWMVHRD));

    //pWMVDec->m_bRndCtrlOn = TRUE;
    pWMVDec->m_bRangeRedY_Flag = FALSE; // Resetting range red flags at the beginning of new advance sequence header
    pWMVDec->m_bRangeRedUV_Flag = FALSE;

    // Non-conditional sequence header fields
    iProfile = BS_getBits(pWMVDec->m_pbitstrmIn, 2);
    if(iProfile!= 3)
        return ICERR_ERROR;
        
    pWMVDec->m_iWMV3Profile = WMV3_ADVANCED_PROFILE;
    pWMVDec->m_iLevel = BS_getBits(pWMVDec->m_pbitstrmIn, 3);
    pWMVDec->m_iChromaFormat = BS_getBits(pWMVDec->m_pbitstrmIn, 2);
    pWMVDec->m_dFrameRate = BS_getBits(pWMVDec->m_pbitstrmIn, 3);  //FRMRTQ_POSTPROC
    pWMVDec->m_iBitRate = BS_getBits(pWMVDec->m_pbitstrmIn, 5); //BITRTQ_POSTPROC)
    pWMVDec->m_bPostProcInfoPresent = BS_getBits(pWMVDec->m_pbitstrmIn, 1);

    // Max coded size for sequence
    pWMVDec->m_iMaxFrmWidthSrc = 2 * BS_getBits(pWMVDec->m_pbitstrmIn, 12) + 2;
    pWMVDec->m_iMaxFrmHeightSrc = 2 * BS_getBits(pWMVDec->m_pbitstrmIn, 12) + 2;
	if(pWMVDec->m_iMaxFrmWidthSrc > 2500 || pWMVDec->m_iMaxFrmHeightSrc > 2500)
	{
		pWMVDec->m_iMaxFrmWidthSrc = 0;
		pWMVDec->m_iMaxFrmHeightSrc = 0;
		return -1;
	}
	if (piPIC_HORIZ_SIZE != NULL && piPIC_VERT_SIZE != NULL) {
        *piPIC_HORIZ_SIZE  = pWMVDec->m_iMaxFrmWidthSrc;
        *piPIC_VERT_SIZE   = pWMVDec->m_iMaxFrmHeightSrc;
    } 

	//printf(" pWMVDec->m_iMaxFrmWidthSrc = %d    pWMVDec->m_iMaxFrmHeightSrc = %d \n",pWMVDec->m_iMaxFrmWidthSrc,  pWMVDec->m_iMaxFrmHeightSrc);
    
	//PULLDOWN is a 1-bit syntax element that shall indicate if the syntax elements RPTFRM, or TFF and RFF are present
	//in picture headers
    pWMVDec->m_bBroadcastFlags = BS_getBits(pWMVDec->m_pbitstrmIn, 1); //PULLDOWN
    pWMVDec->m_bInterlacedSource = BS_getBits(pWMVDec->m_pbitstrmIn, 1);//INTERLACE
    pWMVDec->m_bTemporalFrmCntr = BS_getBits(pWMVDec->m_pbitstrmIn, 1); //Frame Counter Flag 
    pWMVDec->m_bSeqFrameInterpolation = BS_getBits(pWMVDec->m_pbitstrmIn, 1);//FINTERPFLAG Frame Interpolation Flag
    BS_getBits(pWMVDec->m_pbitstrmIn, 1); //RESERVED
    // beta=0/RTM=1 flag - no handling for now
    BS_getBits(pWMVDec->m_pbitstrmIn, 1); //PSF Progressive Segmented Frame

    // Placeholders for values parsed from sequence header but not currently used in this implementation of the decoder
    // Display extension
    if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) 
	{ //DISPLAY_EXT
        I32_WMV iDISP_HORIZ_SIZE = 2*BS_getBits(pWMVDec->m_pbitstrmIn, 14) + 2;//DISP_HORIZ_SIZE
        I32_WMV iDISP_VERT_SIZE = 2*BS_getBits(pWMVDec->m_pbitstrmIn, 14) + 2; //DISP_VERT_SIZE
        if (piDISP_HORIZ_SIZE != NULL && piDISP_VERT_SIZE != NULL) {
            *piDISP_HORIZ_SIZE = iDISP_HORIZ_SIZE;
            *piDISP_VERT_SIZE  = iDISP_VERT_SIZE;
        }
        tDimensions->bDispSizeFlag = TRUE_WMV;
        tDimensions->iDispHorizSize = iDISP_HORIZ_SIZE ;
        tDimensions->iDispVertSize  = iDISP_VERT_SIZE ;

        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)){ //ASPECT_RATIO_FLAG
            iASPECT_RATIO = BS_getBits(pWMVDec->m_pbitstrmIn, 4); //ASPECT_RATIO
            if (iASPECT_RATIO == 15) {
                iASPECT_HORIZ_SIZE  = BS_getBits(pWMVDec->m_pbitstrmIn, 8) + 1;//ASPECT_HORIZ_SIZE
                iASPECT_VERT_SIZE  = BS_getBits(pWMVDec->m_pbitstrmIn, 8) + 1; //ASPECT_VERT_SIZE
            }

            tDimensions->bAspectRatioFlag = TRUE_WMV;
            tDimensions->iAspectRatio = iASPECT_RATIO ;
            tDimensions->iAspect15HorizSize = iASPECT_HORIZ_SIZE;
            tDimensions->iAspect15VertSize = iASPECT_VERT_SIZE  ;

        }

        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)){ //FRAMERATE_FLAG
            tDimensions->bFrameRateFlag = TRUE_WMV;
            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) { //FRAMERATEIND
                iFRAMERATEEXP = BS_getBits(pWMVDec->m_pbitstrmIn, 16) + 1;//FRAMERATEEXP
                tDimensions->bFrameRateInd = TRUE_WMV;
                tDimensions->iFrameRateExp = iFRAMERATEEXP;
            }
            else {
                iFRAMERATENR = BS_getBits(pWMVDec->m_pbitstrmIn, 8);//FRAMERATENR
                iFRAMERATEDR = BS_getBits(pWMVDec->m_pbitstrmIn, 4);//FRAMERATEIDR

                tDimensions->iFrameRateNR = iFRAMERATENR ;
                tDimensions->iFrameRateDR = iFRAMERATEDR ;
            }
        }
        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)){//COLOR_FORMAT_FLAG
            iCOLOR_PRIM = BS_getBits(pWMVDec->m_pbitstrmIn, 8);//COLOR_PRIM
            iTRANSFER_CHAR = BS_getBits(pWMVDec->m_pbitstrmIn, 8);//TRANSFER_CHAR
            iMATRIX_COEF = BS_getBits(pWMVDec->m_pbitstrmIn, 8);//MATRIX_COEF

            tDimensions->bColorFormatFlag = TRUE_WMV;
            tDimensions->iColorPrim = iCOLOR_PRIM ;
            tDimensions->iTransferChar = iTRANSFER_CHAR ;
            tDimensions->iMatrixCoef = iMATRIX_COEF ;
        }
    }
    
    pWMVDec->m_bHRD_PARAM_FLAG = BS_getBits(pWMVDec->m_pbitstrmIn, 1);//HRD_PARAM_FLAG
    if (pWMVDec->m_bHRD_PARAM_FLAG) {
		if(pWMVDec->m_bCodecIsWVC1) {
			pWMVDec->m_ihrd_num_leaky_buckets = BS_getBits(pWMVDec->m_pbitstrmIn, 5); //HRD_NUM_LEAKY_BUCKETS
		}
		else {
			pWMVDec->m_ihrd_num_leaky_buckets = BS_getBits(pWMVDec->m_pbitstrmIn, 5) + 1;
		}
        ibit_rate_exponent = BS_getBits(pWMVDec->m_pbitstrmIn, 4) + 6; //BIT_RATE_EXPONENT
        ibuffer_size_exponent = BS_getBits(pWMVDec->m_pbitstrmIn, 4) + 4; //BUFFER_SIZE_EXPONENT

        tDimensions->iNumLeakyBuckets = pWMVDec->m_ihrd_num_leaky_buckets ;
        tDimensions->iBitRateExponent = ibit_rate_exponent ;
        tDimensions->iBufferSizeExponent = ibuffer_size_exponent ;
        tDimensions->pHRD = pWMVDec->m_ihrd_num_leaky_buckets>0 ? ptHRD : NULL;

#ifdef STABILITY
        if(pWMVDec->m_ihrd_num_leaky_buckets > 17)
            return -1;
#endif

        for (i = 0; i < pWMVDec->m_ihrd_num_leaky_buckets; i++){
            ihrd_rate_N = BS_getBits(pWMVDec->m_pbitstrmIn, 16);
            ihrd_buffer_N = BS_getBits(pWMVDec->m_pbitstrmIn, 16);
            ptHRD[i].ui16HrdRate   = ihrd_rate_N;
            ptHRD[i].ui16HrdBuffer = ihrd_buffer_N;
        }

    }

//#ifdef WIN32
//	{
//		I8_WMV cTmp[60];
//		strcat(pWMVDec->m_SeqHeadInfoString, "\r\nProfile: Advanced Profile(AP)");
//		strcat(pWMVDec->m_SeqHeadInfoString, "\r\nLevel: ");
//		if(pWMVDec->m_iLevel <= 4) sprintf(cTmp,"L%d", pWMVDec->m_iLevel);
//		else sprintf(cTmp,"SMPTE Resevered");
//		strcat(pWMVDec->m_SeqHeadInfoString, cTmp);
//
//		if(pWMVDec->m_iNumBFrames == 1)	strcat(pWMVDec->m_SeqHeadInfoString, "\r\nB frame: Contained");
//		else strcat(pWMVDec->m_SeqHeadInfoString, "\r\nB frame: Not Contained");
//	}
//#endif

    if (BS_invalid (pWMVDec->m_pbitstrmIn)) {
//#ifdef WIN32
//		strcat(pWMVDec->m_SeqHeadInfoString, "\r\n*** Error:There is no enough bits for decoding sequence header. ***");
//#endif
        return ICERR_ERROR;
    }

    // Set up variables based on sequence parameters
    pWMVDec->m_dFrameRate = 4 * pWMVDec->m_dFrameRate + 2;  // midpoint of bin
    pWMVDec->m_iFrameRate = (I32_WMV)pWMVDec->m_dFrameRate;
    pWMVDec->m_iBitRate = 64 * pWMVDec->m_iBitRate + 32;
    //g_iBframeDist = pWMVDec->m_iNumBFrames;
    pWMVDec->m_iBFrameReciprocal = g_iBInverse[pWMVDec->m_iNumBFrames];
    //pWMVDec->m_bExplicitQuantizer = pWMVDec->m_bExplicitSeqQuantizer || pWMVDec->m_bExplicitFrameQuantizer;

    SetMVRangeFlag (pWMVDec, 0);

    //initAdaptPostLevel();   // re-init now that we know the frame rate
    //initBFOmission();       // init BF Omit logic
    
	// old/unused stuff
	//pWMVDec->m_bMixedPel = pWMVDec->m_bFrmHybridMVOn = FALSE;
	pWMVDec->m_bYUV411 = FALSE; //BS_getBits(pWMVDec->m_pbitstrmIn, 1); 
	pWMVDec->m_bSpriteMode = FALSE; //BS_getBits(pWMVDec->m_pbitstrmIn, 1);
	pWMVDec->m_bXintra8Switch = pWMVDec->m_bXintra8 = FALSE; //BS_getBits(pWMVDec->m_pbitstrmIn, 1);
	pWMVDec->m_bMultiresEnabled  = FALSE; //BS_getBits(pWMVDec->m_pbitstrmIn, 1);
	pWMVDec->m_b16bitXform  = TRUE; //BS_getBits(pWMVDec->m_pbitstrmIn, 1);
	pWMVDec->m_bDCTTable_MB_ENABLED  = FALSE; //BS_getBits(pWMVDec->m_pbitstrmIn, 1);
	// may have to override transform function pointers
#if 0
	decide_vc1_routines (pWMVDec);
	m_InitFncPtrAndZigzag(pWMVDec);
#endif

	//pWMVDec->m_pSp->m_pIDCT_Dec = pWMVDec->m_pIntraX9IDCT_Dec;
	//pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantize16;
	// The following bit determines wthe encoder used to generate the content:
	// 0: Beta 1: RTM. Due to bugs, each version produces an incompatible
	// bitstream. Parsing this value allows the decoder to properly decode the corresponding version.
	// NOTE: default at this point is: pWMVDec->m_bBetaContent = TRUE, pWMVDec->m_bRTMContent = FALSE
	// Depending on the detected encoder version, these values are overidden as follows:
	pWMVDec->m_iBetaRTMMismatchIndex = 1;
	//pWMVDec->m_bRTMContent = TRUE;
	//pWMVDec->m_bBetaContent = FALSE;
	pWMVDec->m_iBetaRTMMismatchIndex = 0;


//#ifdef WIN32
//	{
//		I8_WMV cTmp[60];
//		strcat(pWMVDec->m_SeqHeadInfoString, "\r\nFrame Rate: ");
//		sprintf(cTmp,"%d", pWMVDec->m_iFrameRate);
//		strcat(pWMVDec->m_SeqHeadInfoString, cTmp);
//
//		strcat(pWMVDec->m_SeqHeadInfoString, "\r\nBit Rate: ");
//		sprintf(cTmp,"%d", pWMVDec->m_iBitRate);
//		strcat(pWMVDec->m_SeqHeadInfoString, cTmp);
//
//     strcat(pWMVDec->m_SeqHeadInfoString, "\r\nSprite Mode: No");
//
//		if(pWMVDec->m_bMultiresEnabled == 0)	strcat(pWMVDec->m_SeqHeadInfoString, "\r\nMulti-Resolution: Disabled");
//		else strcat(pWMVDec->m_SeqHeadInfoString, "\r\nMulti-Resolution: Enabled");
//
//		if(pWMVDec->m_bYUV411 == 1)	strcat(pWMVDec->m_SeqHeadInfoString, "\r\nYUV411 Format: Yes");
//	}
//#endif

    return ICERR_OK;
}


tWMVDecodeStatus decodeVOLHeadVC1 (tWMVDecInternalMember *pWMVDec, 
                                   U32_WMV uiFormatLength, 
                                   I32_WMV *piPIC_HORIZ_SIZE, 
                                   I32_WMV *piPIC_VERT_SIZE , 
                                   I32_WMV *piDISP_HORIZ_SIZE,
                                   I32_WMV *piDISP_VERT_SIZE)
{
    // read WMV3 profile
    I32_WMV iProfile;                         // C0 00 00 00
    Bool_WMV bValidProfile = TRUE;
    if (pWMVDec->m_cvCodecVersion == WMVA){
        I32_WMV result;
        //ignore start code prefix
        if(BS_getBits(pWMVDec->m_pbitstrmIn, 8))
            return WMV_Failed;
        if(BS_getBits(pWMVDec->m_pbitstrmIn, 8))
            return WMV_Failed;
        if(BS_getBits(pWMVDec->m_pbitstrmIn, 8)!=1)
            return WMV_Failed;
        if(BS_getBits(pWMVDec->m_pbitstrmIn, 8)!=SC_SEQ)
            return WMV_Failed;

        result = decodeSequenceHead_Advanced(pWMVDec, 
            uiFormatLength, 
            piPIC_HORIZ_SIZE, 
            piPIC_VERT_SIZE, 
            piDISP_HORIZ_SIZE, 
            piDISP_VERT_SIZE);
        if (result != ICERR_OK) 
		{
			pWMVDec->sequence_errorflag = 1;
            return WMV_Failed;
		}
		pWMVDec->sequence_errorflag = 0;
        
		//zou 66
        //if (pWMVDec->m_iMaxFrmWidthSrc * pWMVDec->m_iMaxFrmHeightSrc > pWMVDec->m_iMaxPrevSeqFrameArea) {
        //    //if max encoding size is greater than the display size in ASF header.
        //    pWMVDec->m_iMaxPrevSeqFrameArea = pWMVDec->m_iMaxFrmWidthSrc * pWMVDec->m_iMaxFrmHeightSrc;
        //    FreeFrameAreaDependentMemory(pWMVDec);
        //    result = initFrameAreaDependentMemory(pWMVDec, pWMVDec->m_iMaxFrmWidthSrc, pWMVDec->m_iMaxFrmHeightSrc);
        //    if (ICERR_OK != result)
        //        return WMV_Failed;
        //}

        if(BS_flush (pWMVDec->m_pbitstrmIn))
            return WMV_Failed;

        if(BS_getBits(pWMVDec->m_pbitstrmIn, 8))
            return WMV_Failed;
        if(BS_getBits(pWMVDec->m_pbitstrmIn, 8))
            return WMV_Failed;
        if(BS_getBits(pWMVDec->m_pbitstrmIn, 8)!=1)
            return WMV_Failed;
        if(BS_getBits(pWMVDec->m_pbitstrmIn, 8)!=SC_ENTRY)
            return WMV_Failed;

        result = SwitchEntryPoint(pWMVDec); 
		if (result != ICERR_OK) 
		{
			pWMVDec->entrypoint_errorflag = 1;
			return WMV_Failed;
		}

		pWMVDec->entrypoint_errorflag = 0;

		//zou 66
		if (pWMVDec->m_iMaxFrmWidthSrc * pWMVDec->m_iMaxFrmHeightSrc > pWMVDec->m_iMaxPrevSeqFrameArea) 
		{
			//if max encoding size is greater than the display size in ASF header.
			pWMVDec->m_iMaxPrevSeqFrameArea = pWMVDec->m_iMaxFrmWidthSrc * pWMVDec->m_iMaxFrmHeightSrc;
			FreeFrameAreaDependentMemory(pWMVDec);
			result = initFrameAreaDependentMemory(pWMVDec, pWMVDec->m_iMaxFrmWidthSrc, pWMVDec->m_iMaxFrmHeightSrc);
			if (ICERR_OK != result)
				return WMV_Failed;
		}

		pWMVDec->sequence_errorflag = 0;
        return WMV_Succeeded;
    }

   // read WMV3 profile
    iProfile = BS_getBits(pWMVDec->m_pbitstrmIn, 2);    
    
    if (iProfile == 0) {
        pWMVDec->m_iWMV3Profile = WMV3_SIMPLE_PROFILE;
    } else if (iProfile == 1) {
        pWMVDec->m_iWMV3Profile = WMV3_MAIN_PROFILE;
    } else if (iProfile == 2) {
        pWMVDec->m_iWMV3Profile = WMV3_PC_PROFILE;
    }

    pWMVDec->m_bYUV411 = BS_getBits(pWMVDec->m_pbitstrmIn, 1);                // 20 00 00 00
    pWMVDec->m_bSpriteMode = BS_getBits(pWMVDec->m_pbitstrmIn,1);                       // 10 00 00 00

    //printf(" \n\n *******  m_bYUV411 =%d  m_bSpriteMode= %d \n", pWMVDec->m_bYUV411,pWMVDec->m_bSpriteMode);
    
    pWMVDec->m_iFrameRate = BS_getBits(pWMVDec->m_pbitstrmIn,3);  // midpoint of bin    // 0E 00 00 00
    
    pWMVDec->m_iBitRate = BS_getBits(pWMVDec->m_pbitstrmIn,5);                          // 01 F0 00 00
    pWMVDec->m_iFrameRate = 4 * pWMVDec->m_iFrameRate + 2;  // midpoint of bin
    pWMVDec->m_iBitRate = 64 * pWMVDec->m_iBitRate + 32;
    
    //pWMVDec->m_bRndCtrlOn = TRUE_WMV;
    pWMVDec->m_bLoopFilter = BS_getBits(pWMVDec->m_pbitstrmIn,1);                       // 00 08 00 00
    

    //for WMV3 m_bXintra8Switch should reserved;
    pWMVDec->m_bXintra8Switch = BS_getBits(pWMVDec->m_pbitstrmIn,1);                    // 00 04 00 00
    
    pWMVDec->m_bMultiresEnabled = BS_getBits(pWMVDec->m_pbitstrmIn,1);                  // 00 02 00 00

    pWMVDec->m_b16bitXform = BS_getBits(pWMVDec->m_pbitstrmIn,1);                       // 00 01 00 00
    
    pWMVDec->m_bUVHpelBilinear = BS_getBits(pWMVDec->m_pbitstrmIn,1);                   // 00 00 80 00
    
    pWMVDec->m_bExtendedMvMode = BS_getBits(pWMVDec->m_pbitstrmIn,1); 
                 // 00 00 40 00

    pWMVDec->m_iDQuantCodingOn = BS_getBits(pWMVDec->m_pbitstrmIn,2);                   // 00 00 30 00
    
    // common to main, simple, interlace
    pWMVDec->m_bXformSwitch = BS_getBits(pWMVDec->m_pbitstrmIn,1);                      // 00 00 08 00
    pWMVDec->m_bDCTTable_MB_ENABLED = BS_getBits(pWMVDec->m_pbitstrmIn,1);              // 00 00 04 00
    
    pWMVDec->m_bSequenceOverlap = BS_getBits( pWMVDec->m_pbitstrmIn, 1);                // 00 00 02 00
    pWMVDec->m_bStartCode = BS_getBits(pWMVDec->m_pbitstrmIn, 1);                       // 00 00 01 00
    
    
    pWMVDec->m_bPreProcRange = BS_getBits( pWMVDec->m_pbitstrmIn, 1);                   // 00 00 00 80
    
    pWMVDec->m_iNumBFrames = BS_getBits( pWMVDec->m_pbitstrmIn, 3);                     // 00 00 00 70

    //28
    pWMVDec->m_bExplicitSeqQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1);            // 00 00 00 08
    if (pWMVDec->m_bExplicitSeqQuantizer)
        pWMVDec->m_bUse3QPDZQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1);           // 00 00 00 04
    else 
        pWMVDec->m_bExplicitFrameQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1);      // 00 00 00 04
    pWMVDec->m_bExplicitQuantizer = pWMVDec->m_bExplicitSeqQuantizer || pWMVDec->m_bExplicitFrameQuantizer;
    
    pWMVDec->m_bSeqFrameInterpolation = BS_getBits(pWMVDec->m_pbitstrmIn, 1);           // 00 00 00 02


    // Verify Profile
    if (!pWMVDec->m_bSpriteMode) 
    {
        if (pWMVDec->m_iWMV3Profile == WMV3_SIMPLE_PROFILE)
        {
            bValidProfile = (pWMVDec->m_bXintra8Switch == FALSE) &&
                (pWMVDec->m_b16bitXform == TRUE) &&
                (pWMVDec->m_bUVHpelBilinear == TRUE) &&
                (pWMVDec->m_bStartCode == FALSE) &&
                (pWMVDec->m_bLoopFilter == FALSE) &&
                (pWMVDec->m_bYUV411 == FALSE) &&
                (pWMVDec->m_bMultiresEnabled == FALSE) &&
                (pWMVDec->m_iDQuantCodingOn == 0) &&
                (pWMVDec->m_iNumBFrames == 0) &&
                (pWMVDec->m_bPreProcRange == FALSE);
            
        } 
		else if (pWMVDec->m_iWMV3Profile == WMV3_MAIN_PROFILE) 
        {
            bValidProfile = (pWMVDec->m_bXintra8Switch == FALSE) &&
                (pWMVDec->m_b16bitXform == TRUE);
        }
		else if (pWMVDec->m_iWMV3Profile == WMV3_PC_PROFILE)
        {
			pWMVDec->sequence_errorflag = 1;
           return WMV_UnSupportedCompressedFormat; // no feature restrictions for complex profile.
        }
        pWMVDec->sequence_errorflag = 0;
        if (!bValidProfile) 
		{
			pWMVDec->sequence_errorflag = 1;
            return WMV_UnSupportedCompressedFormat;
		}
		pWMVDec->sequence_errorflag = 0;
    }
    
    pWMVDec->m_iBFrameReciprocal = g_iBInverse[pWMVDec->m_iNumBFrames];
    
    if (pWMVDec->m_bYUV411)
        pWMVDec->m_bXintra8Switch = pWMVDec->m_bXintra8 = FALSE;
    
    SetMVRangeFlag (pWMVDec, 0);
    
    // old/unused stuff
    //pWMVDec->m_bMixedPel = pWMVDec->m_bFrmHybridMVOn = FALSE_WMV;
    
    // may have to override transform function pointers
#if 0
    m_InitIDCT_Dec (pWMVDec, TRUE);
    decide_vc1_routines (pWMVDec);
    m_InitFncPtrAndZigzag(pWMVDec);
#endif
	//pWMVDec->m_pSp->m_pIDCT_Dec = pWMVDec->m_pIntraX9IDCT_Dec;        
    // IW pWMVDec->m_pSp->m_pIDCT_Dec = pWMVDec->m_pIntraX9IDCT_Dec;
    //pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantize16;  
   
    pWMVDec->m_iBetaRTMMismatchIndex = 1;
    
    if (BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)== 1 && !BS_invalid(pWMVDec->m_pbitstrmIn)) {    // 00 00 00 01
        // RTM content
        //pWMVDec->m_bRTMContent = TRUE;
        pWMVDec->m_iBetaRTMMismatchIndex = 0;
    }
    
    if (uiFormatLength == 5){
        Bool_WMV bVCMInfoPresent;
        pWMVDec->m_bPostProcInfoPresent = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        bVCMInfoPresent = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
    }
    return WMV_Succeeded;
}

I32_WMV decodeVOPHeadProgressiveWMVA (tWMVDecInternalMember *pWMVDec)    
{
    I32_WMV stepDecoded;
    I32_WMV iMVRangeIndex = 0;
    I32_WMV iNumWindowsPresent = 0;
    CPanScanInfo sPanScanInfo;

    DecodeVOPType_WMVA (pWMVDec);

    if (pWMVDec->m_tFrmType == SKIPFRAME) {
        if (pWMVDec->m_bBroadcastFlags){
            if (pWMVDec->m_bInterlacedSource) {
                pWMVDec->m_bTopFieldFirst = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
                pWMVDec->m_bRepeatFirstField = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
            }
            else// Progressive source
                pWMVDec->m_iRepeatFrameCount = BS_getBits(pWMVDec->m_pbitstrmIn, 2);
        }
        // Pan-scan parameters
        if (pWMVDec->m_bPanScanPresent) {
            DecodePanScanInfo (pWMVDec, &sPanScanInfo, &iNumWindowsPresent);
        }

        return ICERR_OK;
    }

    if (pWMVDec->m_tFrmType != IVOP && pWMVDec->m_tFrmType != PVOP && pWMVDec->m_tFrmType != BIVOP && pWMVDec->m_tFrmType != BVOP)
        return ICERR_ERROR;

    if (pWMVDec->m_bTemporalFrmCntr)
    { //I32_WMV iTime =
         BS_getBits( pWMVDec->m_pbitstrmIn, 8);  //zou  -->VC1_BITS_TFCNTR
    }

    if (pWMVDec->m_bBroadcastFlags){
        if (pWMVDec->m_bInterlacedSource) {
            pWMVDec->m_bTopFieldFirst = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
            pWMVDec->m_bRepeatFirstField = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        }
        else // Progressive source
        {
            pWMVDec->m_iRepeatFrameCount = BS_getBits( pWMVDec->m_pbitstrmIn, 2);
        }
	}

 // Pan-scan parameters
    if (pWMVDec->m_bPanScanPresent) {
        DecodePanScanInfo (pWMVDec, &sPanScanInfo, &iNumWindowsPresent);
    }

    pWMVDec->m_iRndCtrl = BS_getBits( pWMVDec->m_pbitstrmIn, 1);

    if (pWMVDec->m_bInterlacedSource)
    {
        pWMVDec->m_bProgressive420 = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
    }


    if (pWMVDec->m_bSeqFrameInterpolation)
        pWMVDec->m_bInterpolateCurrentFrame = BS_getBits( pWMVDec->m_pbitstrmIn, 1 );   

    // Decode B frac
    if (pWMVDec->m_tFrmType == BVOP)
    {
        I32_WMV iShort = 0, iLong = 0;
        iShort = BS_getBits( pWMVDec->m_pbitstrmIn,  3);
        if (iShort == 0x7)
        {
            iLong = BS_getBits( pWMVDec->m_pbitstrmIn,  4);

            if (iLong == 0xe)       // "hole" in VLC
                return ICERR_ERROR;

            if(DecodeFrac(pWMVDec, iLong + 112, TRUE))
                return ICERR_ERROR;

        }
        else
        {
            if(DecodeFrac(pWMVDec, iShort, FALSE))
               return ICERR_ERROR;
        }

    }
   
// Check to see if this supposed to be in WMVA
//    if (pWMVDec->m_cvCodecVersion >= WMV2 && (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP)) {
//        I32_WMV iBufferFullPercent = BS_getBits( pWMVDec->m_pbitstrmIn, 7);
//        if (BS_invalid(pWMVDec->m_pbitstrmIn))
//            return ICERR_ERROR;
//    }
    
    stepDecoded = BS_getBits( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_QUANTIZER);  //zou --->VC1_BITS_PQINDEX
    
    pWMVDec->m_iQPIndex = stepDecoded;

    if (stepDecoded <= MAXHALFQP)
        pWMVDec->m_bHalfStep = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
    else
        pWMVDec->m_bHalfStep = FALSE;

    if (pWMVDec->m_bExplicitFrameQuantizer)
        pWMVDec->m_bUse3QPDZQuantizer = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
    
    if (!pWMVDec->m_bExplicitQuantizer) { // Implicit quantizer   //zou difference
        if (pWMVDec->m_iQPIndex <= MAX3QP) {
            pWMVDec->m_bUse3QPDZQuantizer = TRUE;
            stepDecoded = pWMVDec->m_iQPIndex;
        }
        else {
            pWMVDec->m_bUse3QPDZQuantizer = FALSE;
            stepDecoded = g_iStepRemap[pWMVDec->m_iQPIndex - 1];
        }
    }
    else // Explicit quantizer
        stepDecoded = pWMVDec->m_iQPIndex;

    pWMVDec->m_iStepSize = stepDecoded;

    pWMVDec->m_iOverlap = 0;
    if (pWMVDec->m_bSequenceOverlap && pWMVDec->m_tFrmType != BVOP) {
        if (pWMVDec->m_iStepSize >= 9)
            pWMVDec->m_iOverlap = 1;
        else if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP)
            pWMVDec->m_iOverlap = 7;
        // pWMVDec->m_iOverlap last 3 bits: [MB switch=1/frame switch=0][sent=1/implied=0][on=1/off=0]
    }

    if (pWMVDec->m_bPostProcInfoPresent)
        pWMVDec->m_iPostProcLevel = BS_getBits( pWMVDec->m_pbitstrmIn, 2);   //zou -->vc1_PostProcessingNone

    if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {
        // AC Pred flag
        if (DecodeSkipBitX9 (pWMVDec, 4) == ICERR_ERROR) {  //zou --->eResult = vc1DECBITPL_ReadBitplane(pState, &pParams->sBPPredictAC, pBitstream);
            return ICERR_ERROR;
        }
        if (pWMVDec->m_iACPredIFrameCodingMode != SKIP_RAW) {
            I32_WMV i;
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                pWMVDec->m_rgmbmd[i].m_rgbDCTCoefPredPattern2 [0] = (pWMVDec->m_rgmbmd[i].m_bSkip ? TRUE 
                    : FALSE);
            }
        }

        if (pWMVDec->m_iOverlap & 2) {
            if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0) // off
                pWMVDec->m_iOverlap = 0;
            else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0) // on
                pWMVDec->m_iOverlap = 1;
            else {// MB switch - decode bitplane
                if (DecodeSkipBitX9 (pWMVDec, 5) == ICERR_ERROR) 
                    return ICERR_ERROR;   
                if (pWMVDec->m_iOverlapIMBCodingMode != SKIP_RAW) {
                    I32_WMV i;
                    for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                        pWMVDec->m_rgmbmd[i].m_bOverlapIMB = (pWMVDec->m_rgmbmd[i].m_bSkip ? TRUE : FALSE); 
                    }
                }
            }
        }
    }

    if (pWMVDec->m_iOverlap & 1) {
        pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 0;
        pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 0;
    }
    else {
        pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 128;
        pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 128;
    }

    pWMVDec->m_prgDQuantParam = pWMVDec->m_bUse3QPDZQuantizer ? pWMVDec->m_rgDQuantParam3QPDeadzone : pWMVDec->m_rgDQuantParam5QPDeadzone;

    //g_iStepsize = pWMVDec->m_iQPIndex * 2 + pWMVDec->m_bHalfStep;
    
    pWMVDec->m_iStepSize = stepDecoded;
    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }


    pWMVDec->m_iStepSize = stepDecoded;
    if (stepDecoded <= 0 || stepDecoded > 31)
            return ICERR_ERROR;

    if (pWMVDec->m_iQPIndex <= MAXHIGHRATEQP) {
        // If QP (picture quant) is <= MAXQP_3QPDEADZONE then we substitute the highrate
        // inter coeff table for the talking head table among the 3 possible tables that
        // can be used to code the coeff data
        pWMVDec->m_pInterDCTTableInfo_Dec_Set[0] = &pWMVDec->InterDCTTableInfo_Dec_HghRate;
        pWMVDec->m_pIntraDCTTableInfo_Dec_Set[0] = &pWMVDec->IntraDCTTableInfo_Dec_HghRate;
    }
    
	if (pWMVDec->m_tFrmType != IVOP && pWMVDec->m_tFrmType != BIVOP) {
		if (pWMVDec->m_bExtendedMvMode) {
			iMVRangeIndex = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
			if (iMVRangeIndex)
				iMVRangeIndex += BS_getBits( pWMVDec->m_pbitstrmIn, 1);
			if (iMVRangeIndex == 2)
				iMVRangeIndex += BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        
		}
	}
#ifdef STABILITY
    if(iMVRangeIndex > 3)
        return WMV_CorruptedBits;
#endif
    SetMVRangeFlag (pWMVDec, iMVRangeIndex);

	//printf("[threads%d]   OK 7_3 \n");

    if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {
        I32_WMV result = setSliceCode (pWMVDec, pWMVDec->m_iSliceCode);
        //I32_WMV iSliceCode = BS_getBits( pWMVDec->m_pbitstrmIn, NUMBITS_SLICE_SIZE);
        if (result == ICERR_ERROR) {
            return ICERR_ERROR;
        }
      //  result = DecodeClipInfo (pWMVDec);
        if(BS_invalid(pWMVDec->m_pbitstrmIn) || result != ICERR_OK) {
            return ICERR_ERROR;
        }
        //pWMVDec->m_bDCPred_IMBInPFrame = FALSE;
 
        // If pWMVDec->m_bDCTTable_MB_ENABLED is on
        //if (pWMVDec->m_bDCTTable_MB_ENABLED){
        //    pWMVDec->m_bDCTTable_MB = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        //}

            
        // DCT Table swtiching, I and P index are coded separately.
        // Can be jointly coded using the following table. 
        // IP Index : Code
        // 00       : 00, 
        // 11       : 01, 
        // 01       : 100,
        // 10       : 101,
        // 02       : 1100,
        // 12       : 1101,
        // 20       : 1110, 
        // 21       : 11110
        // 22       : 11111
        pWMVDec->m_rgiDCTACInterTableIndx[0] = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        if (pWMVDec->m_rgiDCTACInterTableIndx[0]){
            pWMVDec->m_rgiDCTACInterTableIndx[0] += BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        }
        pWMVDec->m_rgiDCTACIntraTableIndx[0] = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        if (pWMVDec->m_rgiDCTACIntraTableIndx[0]){
            pWMVDec->m_rgiDCTACIntraTableIndx[0] += BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        }
        pWMVDec->m_rgiDCTACIntraTableIndx[1] = pWMVDec->m_rgiDCTACIntraTableIndx[2] = pWMVDec->m_rgiDCTACIntraTableIndx[0];
            
        pWMVDec->m_iIntraDCTDCTable = BS_getBits( pWMVDec->m_pbitstrmIn, 1);

        // I Frame Dquant
        if (pWMVDec->m_iDQuantCodingOn)
            DecodeVOPDQuant (pWMVDec, FALSE);
        else
            SetDefaultDQuantSetting(pWMVDec);
        
        
    }
    else {
        I32_WMV result;

		if(pWMVDec->nInputFrameIndx == 124)
			pWMVDec->nInputFrameIndx =pWMVDec->nInputFrameIndx;

        result = decodeVOPHead_WMV3(pWMVDec);

        if (result)
		{
			printf("[thread]***re %d*******%d****** \n",result, pWMVDec->nthreads,pWMVDec->nInputFrameIndx);

            return ICERR_ERROR;
		}
        
        //if (pWMVDec->m_bDCTTable_MB_ENABLED){
        //    pWMVDec->m_bDCTTable_MB = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        //}

        //if (!pWMVDec->m_bDCTTable_MB){
        pWMVDec->m_rgiDCTACInterTableIndx[0] = BS_getBits( pWMVDec->m_pbitstrmIn, 1);  //zou --->/* Frame level transform AC coding set index */
        if (pWMVDec->m_rgiDCTACInterTableIndx[0]){
            pWMVDec->m_rgiDCTACInterTableIndx[0] += BS_getBits( pWMVDec->m_pbitstrmIn, 1);
            //   }
        }
        pWMVDec->m_rgiDCTACIntraTableIndx[0] = pWMVDec->m_rgiDCTACIntraTableIndx[1] = pWMVDec->m_rgiDCTACIntraTableIndx[2] = 
        pWMVDec->m_rgiDCTACInterTableIndx[1] = pWMVDec->m_rgiDCTACInterTableIndx[2] = pWMVDec->m_rgiDCTACInterTableIndx[0];
        pWMVDec->m_iIntraDCTDCTable = BS_getBits( pWMVDec->m_pbitstrmIn, 1);   //zou -->  /* Intra transform DC table */       
    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }

    return ICERR_OK;
}

I32_WMV DecodePanScanInfo (tWMVDecInternalMember *pWMVDec, CPanScanInfo *pPanScanInfo, I32_WMV *piNumWindowsPresent)
{
    I32_WMV iNumWindows = 0;
    I32_WMV i;

    if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) {
        if (pWMVDec->m_bInterlacedSource) {
            if (pWMVDec->m_bBroadcastFlags)
                iNumWindows = 2 + pWMVDec->m_bRepeatFirstField;
            else
                iNumWindows = 2;      
        }
        else {
            if (pWMVDec->m_bBroadcastFlags)
                iNumWindows = 1 + pWMVDec->m_iRepeatFrameCount;
            else
                iNumWindows = 1; 
        }
        for (i = 0; i < iNumWindows; i++) {
            I32_WMV iBits;
            iBits = BS_getBits(pWMVDec->m_pbitstrmIn, 16);
            pPanScanInfo->sWindowInfo[i].iHorizOffset = (iBits << 2) | BS_getBits(pWMVDec->m_pbitstrmIn, 2);
            iBits = BS_getBits(pWMVDec->m_pbitstrmIn, 16);
            pPanScanInfo->sWindowInfo[i].iVertOffset = (iBits << 2) | BS_getBits(pWMVDec->m_pbitstrmIn, 2);
            pPanScanInfo->sWindowInfo[i].iWidth = BS_getBits(pWMVDec->m_pbitstrmIn, 14);
            pPanScanInfo->sWindowInfo[i].iHeight = BS_getBits(pWMVDec->m_pbitstrmIn, 14);

        }
    }

    *piNumWindowsPresent = iNumWindows;
    return ICERR_OK;
}

Void_WMV DecodeVOPType_WMVA (tWMVDecInternalMember *pWMVDec)
{
    
    // I        : 110
    // P        : 0
    // B        : 10
    // BI       : 1110
    // Skipped  : 1111
    
        if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
            pWMVDec->m_tFrmType = PVOP;
        else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
            pWMVDec->m_tFrmType = BVOP;
        else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
            pWMVDec->m_tFrmType = IVOP;
        else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
            pWMVDec->m_tFrmType = BIVOP;
        else
        {
            pWMVDec->m_tFrmType = SKIPFRAME;
            pWMVDec->m_iDroppedPFrame = 1;
			//201318
			pWMVDec->m_bRenderDirect = TRUE_WMV;
            //PreGetOutput(pWMVDec);
        }
}

Void_WMV DecodeVOPType_WMVA2 (tWMVDecInternalMember *pWMVDec)
{
    
    // I        : 110
    // P        : 0
    // B        : 10
    // BI       : 1110
    // Skipped  : 1111
    
    if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
        pWMVDec->m_tFrmType = PVOP;
    else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
        pWMVDec->m_tFrmType = BVOP;
    else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
        pWMVDec->m_tFrmType = IVOP;
    else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
        pWMVDec->m_tFrmType = BIVOP;
    else
        pWMVDec->m_tFrmType = SKIPFRAME;
}


//check zigzag scan arrays before each frame
//used only in WMVA, where interlace and progressive modes have different scan arrays,
Void_WMV decideMMXRoutines_ZigzagOnly (tWMVDecInternalMember *pWMVDec)
{
    pWMVDec->m_bRotatedIdct = 1;
    pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInvRotated;
    pWMVDec->m_p8x4ZigzagScanOrder = pWMVDec->m_p8x4ZigzagInvRotated;
    pWMVDec->m_p4x8ZigzagScanOrder = pWMVDec->m_p4x8ZigzagInvRotated;        
    pWMVDec->m_p4x4ZigzagScanOrder = pWMVDec->m_p4x4ZigzagInvRotated;   

}

//Void_WMV SwapRefAndPost(tWMVDecInternalMember *pWMVDec)
//{
//    tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmPostQ;
//
//    pWMVDec->m_pfrmPostQ = pWMVDec->m_pfrmRef0Q;
//    pWMVDec->m_pfrmRef0Q = pTmp;
//
//    pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmRef0Q->m_pucYPlane;
//    pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Q->m_pucUPlane;
//    pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Q->m_pucVPlane;
//
//    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
//
//    pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
//    pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
//    pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;
//}


Void_WMV SwapCurrAndPost(tWMVDecInternalMember *pWMVDec)
{
    tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmPostQ;

    pWMVDec->m_pfrmPostQ = pWMVDec->m_pfrmCurrQ;
    pWMVDec->m_pfrmCurrQ = pTmp;

    pWMVDec->m_ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
    pWMVDec->m_ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
    pWMVDec->m_ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane;

    pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
    pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
    pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;
}


Void_WMV SwapMultiresAndPost(tWMVDecInternalMember *pWMVDec)
{
    tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmPostQ;

    pWMVDec->m_pfrmPostQ = pWMVDec->m_pfrmMultiresQ;
    pWMVDec->m_pfrmMultiresQ = pTmp;

    pWMVDec->m_ppxliMultiresY = pWMVDec->m_pfrmMultiresQ->m_pucYPlane;
    pWMVDec->m_ppxliMultiresU = pWMVDec->m_pfrmMultiresQ->m_pucUPlane;
    pWMVDec->m_ppxliMultiresV = pWMVDec->m_pfrmMultiresQ->m_pucVPlane;

    pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
    pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
    pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;
}
extern const I32_WMV g_iBInverse[];

I32_WMV DecodeFrac(tWMVDecInternalMember *pWMVDec, const I32_WMV iVal, const Bool_WMV bLong)
{
    if (bLong)
    {
        if(iVal <= 111 && iVal >= 126)
            return -1;
        pWMVDec->m_iBNumerator   = g_iNumLongVLC[iVal - 112];
        pWMVDec->m_iBDenominator = g_iDenLongVLC[iVal - 112];
    }
    else
    {
         if(iVal < 0 && iVal >= 7)
            return -1;
        pWMVDec->m_iBNumerator   = g_iNumShortVLC[iVal];
        pWMVDec->m_iBDenominator = g_iDenShortVLC[iVal];
    }

    pWMVDec->m_iBFrameReciprocal = g_iBInverse[pWMVDec->m_iBDenominator - 1]; // this is messy, but
                                                            // seems like "the" place
                                                            // to reset this
    return 0;
}

/****************************************************************************************************
  decodeSkipP : callback for doing the right things when skipped P is encountered
****************************************************************************************************/

tWMVDecodeStatus decodeSkipP (tWMVDecInternalMember *pWMVDec)
{
    // Can't do anything unless we've seen an I-Picture go by
    if (!pWMVDec->m_fDecodedI) {
        return WMV_Failed;
    }
        
    pWMVDec->m_tFrmType  = pWMVDec->m_PredTypePrev = PVOP;      // render the last P frame, this is the right time

    pWMVDec->m_iDroppedPFrame = 1;       // need this for rendering - buffer swaps
    
    return WMV_Succeeded;
}

/****************************************************************************************************
  CopyRefToPrevPost : backup contents of Ref0 to prev post (for rendering)
****************************************************************************************************/


//Void_WMV CopyRefToPrevPost (tWMVDecInternalMember *pWMVDec)
//{
//#ifndef SHAREMEMORY
//    if (pWMVDec->m_bPrevRefInPrevPost || pWMVDec->m_bRenderFromPostBuffer)
//        return; // ref already in prev post
//    // Copy the reference frame because the decode might change the reference frame and we may use the unchanged for output.. 
//     pWMVDec->m_bPrevRefInPrevPost = TRUE;
//#ifdef STABILITY
//	 if(pWMVDec->m_ppxliRef0Y == NULL || pWMVDec->m_ppxliRef0U == NULL || pWMVDec->m_ppxliRef0V == NULL)
//		 return;
//	 if(pWMVDec->m_ppxliPostPrevQY == NULL || pWMVDec->m_ppxliPostPrevQU == NULL || pWMVDec->m_ppxliPostPrevQV == NULL)
//		 return;
//#endif
//
//    ALIGNED32_FASTMEMCPY (pWMVDec->m_ppxliPostPrevQY, pWMVDec->m_ppxliRef0Y, pWMVDec->m_rMultiResParams[0].iWidthPrevY * pWMVDec->m_rMultiResParams[0].iHeightPrevY);
//    ALIGNED32_FASTMEMCPY (pWMVDec->m_ppxliPostPrevQU, pWMVDec->m_ppxliRef0U, (pWMVDec->m_rMultiResParams[0].iWidthPrevY * pWMVDec->m_rMultiResParams[0].iHeightPrevY) >> 2);
//    ALIGNED32_FASTMEMCPY (pWMVDec->m_ppxliPostPrevQV, pWMVDec->m_ppxliRef0V, (pWMVDec->m_rMultiResParams[0].iWidthPrevY * pWMVDec->m_rMultiResParams[0].iHeightPrevY) >> 2);
//#endif
//}

I32_WMV HandleResolutionChange (tWMVDecInternalMember *pWMVDec)
{//WMV3 
    if (pWMVDec->m_iResIndex != pWMVDec->m_iResIndexPrev) 
	{
		//if (pWMVDec->m_iNumBFrames && pWMVDec->m_iResIndexPrev == 0)
		//	CopyRefToPrevPost (pWMVDec);

		if (pWMVDec->m_iNumBFrames || pWMVDec->m_tFrmType == PVOP || pWMVDec->m_tFrmType == BVOP)
		{ 
			I32_WMV iWidthPrevYXExpPlusExp = pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevYXExpPlusExp;
			I32_WMV iWidthPrevUVXExpPlusExp = pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevUVXExpPlusExp;

#ifdef STABILITY
			if(pWMVDec->m_iResIndex + pWMVDec->m_iResIndexPrev >5)
				return -1;
			if(pWMVDec->m_iResIndexPrev >3)
				return -1;
#endif
			if (pWMVDec->m_iResIndex > pWMVDec->m_iResIndexPrev)
			{
				if (!pWMVDec->m_bMBAligned)
				{
					// If the reference frame needs to be downsampled (due to a resolution change) then we
					// need to pad the reference frame prior to downsampling if the downsampled horiz. or
					// vert. Y dimension are not multiples of 16.
					(*pWMVDec->pRepeatRef0Y) (pWMVDec->m_ppxliRef0Y, 0, pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iHeightY, iWidthPrevYXExpPlusExp, TRUE, TRUE,
                        pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthY, pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthYPlusExp,
                        pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevY, !pWMVDec->m_bInterlaceV2);
                
					(*pWMVDec->pRepeatRef0UV) (pWMVDec->m_ppxliRef0U, pWMVDec->m_ppxliRef0V, 0, pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iHeightUV, 
                        iWidthPrevUVXExpPlusExp, TRUE, TRUE, pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthUV,
                        pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthUVPlusExp,  
                        pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevUV, !pWMVDec->m_bInterlaceV2);
				}
            
                DownsampleRefFrame(pWMVDec,
                    pWMVDec->m_ppxliRef0Y + iWidthPrevYXExpPlusExp, 
                    pWMVDec->m_ppxliRef0U + iWidthPrevUVXExpPlusExp,
                    pWMVDec->m_ppxliRef0V + iWidthPrevUVXExpPlusExp,                    
                    pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp,
                    pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                    pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp);
            }
            else
                UpsampleRefFrame(pWMVDec,
                    pWMVDec->m_ppxliRef0Y + iWidthPrevYXExpPlusExp, 
                    pWMVDec->m_ppxliRef0U + iWidthPrevUVXExpPlusExp,
                    pWMVDec->m_ppxliRef0V + iWidthPrevUVXExpPlusExp,                   
                    pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp,
                    pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                    pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp);
        
           // memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) pWMVDec->m_uintNumMB);
            WMVideoDecSwapCurrAndRef (pWMVDec, FALSE);
        
            // Peform repeat pad for new resized reference frame
            (*pWMVDec->pRepeatRef0Y) (pWMVDec->m_ppxliRef0Y,0,pWMVDec->m_iHeightYRepeatPad,pWMVDec->m_iWidthPrevYXExpPlusExp,TRUE,TRUE,pWMVDec->m_iWidthYRepeatPad,pWMVDec->m_iWidthYPlusExp,pWMVDec->m_iWidthPrevY, !pWMVDec->m_bInterlaceV2);
            (*pWMVDec->pRepeatRef0UV)  (pWMVDec->m_ppxliRef0U,pWMVDec->m_ppxliRef0V,0,pWMVDec->m_iHeightUVRepeatPad,pWMVDec->m_iWidthPrevUVXExpPlusExp,TRUE,TRUE,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV, !pWMVDec->m_bInterlaceV2);
            
            //if (!pWMVDec->m_bRenderFromPostBuffer && pWMVDec->m_iResIndexPrev == 0)
            //{
            //    // Need this so that GetPrevOutput gets the correct frame
            //    SwapCurrAndPost (pWMVDec);
            //    pWMVDec->m_bRenderFromPostBuffer = TRUE;
            //}
        }
    }      
    pWMVDec->m_iResIndexPrev = pWMVDec->m_iResIndex;
    return 0;
}





