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

#if 0
void ReOrgnaizeCompBuffer_U82U16(U8_WMV *buffer)
{
    int i=0,j=0;
    for(i=0;i<16;i++)
    {
        U8_WMV* pSrcRow = buffer+i*40;
        U16_WMV pDstTmp[20];

        pDstTmp[0] = pSrcRow[0];
        pDstTmp[1] = pSrcRow[2];
        pDstTmp[2] = pSrcRow[4];
        pDstTmp[3] = pSrcRow[6];
        pDstTmp[4] = pSrcRow[8];
        pDstTmp[5] = pSrcRow[10];
        pDstTmp[6] = pSrcRow[12];
        pDstTmp[7] = pSrcRow[14];
        pDstTmp[8] = pSrcRow[16];
        pDstTmp[9] = pSrcRow[18];

        pDstTmp[10] = pSrcRow[1];
        pDstTmp[11] = pSrcRow[3];
        pDstTmp[12] = pSrcRow[5];
        pDstTmp[13] = pSrcRow[7];
        pDstTmp[14] = pSrcRow[9];
        pDstTmp[15] = pSrcRow[11];
        pDstTmp[16] = pSrcRow[13];
        pDstTmp[17] = pSrcRow[15]; 
        pDstTmp[18] = pSrcRow[17];
        pDstTmp[19] = pSrcRow[19]; 

        memcpy(pSrcRow,pDstTmp,40);
    }
}

void ReOrgnaizeCompBuffer8x8_U82U16(U8_WMV *pSrc, U8_WMV *pDst)
{
    int i=0,j=0;
    for(i=0;i<8;i++)
    {
        U8_WMV* pSrcRow = pSrc+i*40;
        U16_WMV pDstTmp[20];

        pDstTmp[0] = pSrcRow[0];
        pDstTmp[1] = pSrcRow[2];
        pDstTmp[2] = pSrcRow[4];
        pDstTmp[3] = pSrcRow[6];
        pDstTmp[4] = pSrcRow[8];
        pDstTmp[5] = pSrcRow[10];
        pDstTmp[6] = pSrcRow[12];
        pDstTmp[7] = pSrcRow[14];
        pDstTmp[8] = pSrcRow[16];
        pDstTmp[9] = pSrcRow[18];

        memcpy(pSrcRow,pDstTmp,40);
    }
}

void IdctTranspose(U16_WMV* idctin,U16_WMV* idctout)
{
    U16_WMV tmp[64],iy=0;
    U16_WMV* psrc = (U16_WMV*)idctin;
    U16_WMV* pdst = (U16_WMV*)tmp;

    for (iy = 0; iy < BLOCK_SIZE; iy++) //×ªÖÃ
    {
        pdst[0] = psrc[0];   pdst[1] = psrc[8];
        pdst[2] = psrc[16]; pdst[3] = psrc[24];
        pdst[4] = psrc[32]; pdst[5] = psrc[40]; 
        pdst[6] = psrc[48]; pdst[7] = psrc[56];   
        psrc += 1;
        pdst += 8;
    }  
    memcpy(idctout,tmp,64*2);
}
#endif
void GetBackUpDiffMV(EMB_PMainLoop * pPMainLoop,int iMBX,int iMBY,EMB_PBMainLoop *pPB)
{
    int iblk=0;
    CDiffMV_EMB * CurMBDiff = pPB->m_pDiffMV_EMB_Bak + iMBX*6;

    for(iblk=0;iblk<6;iblk++)
        pPB->pDiffMV_EMB_Bak[iblk] = *(CurMBDiff+iblk);
}

#ifdef WIN32
void voDumpYuv_P(tWMVDecInternalMember *pWMVDec,int nframes)
{
	unsigned char* Yplane = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
	unsigned char* Uplane = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	unsigned char* Vplane = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    char filename[250];

    sprintf(filename,"E:/MyResource/Video/clips/VC1/new/P_%d.yuv",nframes);


	if(pWMVDec->m_nPframes == nframes && pWMVDec->m_tFrmType == PVOP)
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

#ifdef VOARMV7
Void_WMV ARMV7_IntraBlockDequant8x8(I32_WMV iDCStepSize, I32_WMV iDoubleStepSize, I32_WMV iStepMinusStepIsEven, I16_WMV *rgiCoefRecon);
#endif

#define WMV_ESC_Decoding()                                                                 \
    if (pWMVDec->m_bFirstEscCodeInFrame){                                                  \
    decodeBitsOfESCCode (pWMVDec);                                                     \
    pWMVDec->m_bFirstEscCodeInFrame = FALSE_WMV;                                       \
    }                                                                                      \
    uiRun = BS_getBits ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_RUN);              \
    if (BS_getBit ( pWMVDec->m_pbitstrmIn)) /* escape decoding */                          \
    iLevel = -1 * BS_getBits ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);  \
    else                                                                                   \
iLevel = BS_getBits ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);                      


VO_VOID AdjustReconRange_MB(U8_WMV* pSrc,U32_WMV src_stride,U8_WMV* pDst,U32_WMV dst_stride,U32_WMV blocksize,U32_WMV scaledown)
{
	VO_U32 i=0,j=0;
	if(scaledown)
	{
		for(i=0;i<blocksize;i++)
		{
			for(j=0;j<blocksize;j++)
			{
				pDst [j] = ((pSrc [j] - 128) >> 1) + 128;
			}
			pSrc += src_stride;
			pDst += dst_stride;
		}
	}
	else
	{
		for(i=0;i<blocksize;i++)
		{
			for(j=0;j<blocksize;j++)
			{
				VO_S32 iTmp = (((VO_S32)pSrc [j] - 128) << 1) + 128;
                pDst [j] = (VO_U8) ((iTmp < 0) ? 0 : (iTmp > 255) ? 255 : iTmp);
			}
			pSrc += src_stride;
			pDst += dst_stride;
		}
	}
}
VO_VOID IntensityCompensation_MB(U8_WMV* pSrc,U32_WMV src_stride,U8_WMV* pDst,U32_WMV dst_stride,U8_WMV* pInstyTable,U32_WMV blocksize)
{
	VO_U32 i=0,j=0;
	for(i=0;i<blocksize;i++)
	{
		for(j=0;j<blocksize;j++)
		{
			U8_WMV index = (U8_WMV)(pSrc[j]);
			pDst[i*dst_stride+j] = pInstyTable[index];
		}
		pSrc +=src_stride;
	}
}


//#pragma code_seg (EMBSEC_DEF)
Void_WMV EMB_InitZigZagTable_SSIMD(U8_WMV * pZigzag, U8_WMV * pNewZigZag, I32_WMV iNumEntries)
{
    I32_WMV i;
    
    if(pZigzag != NULL)
    {
        for(i = 0; i< iNumEntries; i++)
        {
            U8_WMV iLoc = pZigzag[i];
            
            iLoc = ((iLoc&~3)<<1)|(iLoc&3);
            
            pNewZigZag[i] = iLoc;
        }
    }
}

Void_WMV ResetCoefBufferNull_C(I32_WMV * pBuffer)
{
    
}


//#pragma code_seg (EMBSEC_PML)
Void_WMV ResetCoefBuffer8x8_C(I32_WMV * pBuffer)
{

    I32_WMV i;
    for( i = 0; i<32;i += 4)
    {
        pBuffer[i] = 0;
        pBuffer[i+1] = 0;
        pBuffer[i+2] = 0;
        pBuffer[i+3] = 0;
    }

}

//#pragma code_seg (EMBSEC_PML)
Void_WMV ResetCoefBuffer8x4_C(I32_WMV * pBuffer)
{
    I32_WMV i;
    for( i = 0; i<32;i += 4)
    {
        pBuffer[i] = 0;
        pBuffer[i+1] = 0;
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV ResetCoefBuffer4x8_C(I32_WMV * pBuffer)
{

    I32_WMV i;
    for( i = 0; i<16;i += 4)
    {
        pBuffer[i] = 0;
        pBuffer[i+1] = 0;
        pBuffer[i+2] = 0;
        pBuffer[i+3] = 0;
    }

}
//#pragma code_seg (EMBSEC_PML)
Void_WMV ResetCoefBuffer4x4_C(I32_WMV * pBuffer)
{
    pBuffer[0] = 0;
    pBuffer[0+1] = 0;
    pBuffer[0+4] = 0;
    pBuffer[0+5] = 0;
    pBuffer[0+8] = 0;
    pBuffer[0+1+8] = 0;
    pBuffer[0+4+8] = 0;
    pBuffer[0+5+8] = 0;
}

extern Void_WMV ResetCoefBuffer8x8_ARMV4(I32_WMV * pBuffer);
extern Void_WMV ResetCoefBuffer8x4_ARMV4(I32_WMV * pBuffer);
extern Void_WMV ResetCoefBuffer4x8_ARMV4(I32_WMV * pBuffer);
extern Void_WMV ResetCoefBufferNull_ARMV4(I32_WMV * pBuffer);
extern Void_WMV ResetCoefBuffer4x4_ARMV4(I32_WMV * pBuffer);
Void_WMV ResetCoefBufferNull_ARMV4(I32_WMV * pBuffer)
{    
}

#if defined(_ARM_)
Void_WMV (*g_ResetCoefBuffer[5])(I32_WMV * pBuffer)
= { ResetCoefBuffer8x8_ARMV4, ResetCoefBuffer8x4_ARMV4, ResetCoefBuffer4x8_ARMV4, ResetCoefBufferNull_ARMV4, ResetCoefBuffer4x4_ARMV4};
#else
Void_WMV (*g_ResetCoefBuffer[5])(I32_WMV * pBuffer)
= { ResetCoefBuffer8x8_C, ResetCoefBuffer8x4_C, ResetCoefBuffer4x8_C, ResetCoefBufferNull_C, ResetCoefBuffer4x4_C};
#endif


U8_WMV g_IDCTShortCutLUT[] =
{
    0,0,0,0,0,0,0,0,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        
};


//#pragma code_seg (EMBSEC_DEF)
tWMVDecodeStatus DecodeInverseInterBlockQuantize16_level2_EMB(
                                                              tWMVDecInternalMember *pWMVDec,
                                                              CDCTTableInfo_Dec* InterDCTTableInfo_Dec,
                                                              U32_WMV uiNumCoefs,
                                                              U32_WMV * puiCoefCounter,
                                                              I32_WMV * pvalue,
                                                              I32_WMV * pSignBit,
                                                              U32_WMV * plIndex
                                                              )
{    
    
    Huffman_WMV* hufDCTACDec = InterDCTTableInfo_Dec -> hufDCTACDec;
    U32_WMV uiTCOEF_ESCAPE = InterDCTTableInfo_Dec -> iTcoef_ESCAPE;
    U32_WMV uiStartIndxOfLastRun = InterDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1;

    I16_WMV * rgLevelRunAtIndx = InterDCTTableInfo_Dec -> combined_levelrun;
    U32_WMV lIndex;
    I32_WMV signBit = *pSignBit;
    U32_WMV uiRun;
    I32_WMV iLevel;
	CInputBitStream_WMV*pbitstrmIn = pWMVDec->m_pbitstrmIn;
    
    if(signBit != 0) {
        // ESC + '1' + VLC
        lIndex = Huffman_WMV_get(hufDCTACDec, pbitstrmIn);
        if (lIndex == uiTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
            return WMV_CorruptedBits;
        }
        
        
        uiRun= (U8_WMV)rgLevelRunAtIndx[lIndex];
        iLevel= rgLevelRunAtIndx[lIndex] >>8;
        
        if (lIndex >= uiStartIndxOfLastRun) {
            iLevel += (I8_WMV) InterDCTTableInfo_Dec -> puiLastNumOfLevelAtRun[uiRun];
        }
        else
            iLevel += (I8_WMV) InterDCTTableInfo_Dec -> puiNotLastNumOfLevelAtRun[uiRun];
        
        signBit = -  (I32_WMV)BS_getBit(pbitstrmIn); 
    }
    else if (BS_getBit(pbitstrmIn)){
        
        // ESC + '10' + VLC
        lIndex = Huffman_WMV_get(hufDCTACDec, pbitstrmIn);
        if (lIndex == uiTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
            return WMV_CorruptedBits;
        }
        
        uiRun= (U8_WMV)rgLevelRunAtIndx[lIndex];
        iLevel= rgLevelRunAtIndx[lIndex] >>8;
        if (lIndex >= uiStartIndxOfLastRun) {
            uiRun += (InterDCTTableInfo_Dec -> puiLastNumOfRunAtLevel[iLevel] + 1);
        }
        else
            uiRun += (InterDCTTableInfo_Dec -> puiNotLastNumOfRunAtLevel[iLevel] + 1);
        
        signBit = -  (I32_WMV)BS_getBit(pbitstrmIn); 
    }
    else{
        // ESC + '00' + FLC
        // g_dbg_cnt3++;
        lIndex = uiStartIndxOfLastRun -1 + (Bool_WMV) BS_getBit(pbitstrmIn); // escape decoding
        
        if (pWMVDec->m_cvCodecVersion >= WMV1) {  // == WMV1 || WMV2
            //WMV_ESC_Decoding();
			if (pWMVDec->m_bFirstEscCodeInFrame){  
				decodeBitsOfESCCode (pWMVDec); 
				pWMVDec->m_bFirstEscCodeInFrame = FALSE_WMV;
			} 
			uiRun = BS_getBits (pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_RUN);  
			if (BS_getBit (pbitstrmIn))
				iLevel = -1 * BS_getBits (pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);
			else
				iLevel = BS_getBits (pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);
        }
        else {
            uiRun = BS_getBits(pbitstrmIn,NUMBITS_ESC_RUN);          
            iLevel = (I8_WMV)BS_getBits(pbitstrmIn,NUMBITS_ESC_LEVEL);
        }
        
        signBit = ((I32_WMV) iLevel) >> 31;
        
        if(iLevel <0)
            iLevel = -iLevel;
    }
    
    (*puiCoefCounter) += uiRun;
    
    if (BS_invalid(pbitstrmIn) || ((*puiCoefCounter) >= uiNumCoefs)) {
        return WMV_CorruptedBits;
    }
    
    *pvalue = iLevel;
    *pSignBit = signBit ;
    
    *plIndex = lIndex;
    
    return WMV_Succeeded;
}


//#pragma code_seg (EMBSEC_PML2)
I32_WMV DecodeIntra16_level1_EMB(
                                 Huffman_WMV* hufDCTACDec, 
                                 CInputBitStream_WMV * pBitStrm, 
                                 I16_WMV * rgLevelRunAtIndx,
                                 U8_WMV * pZigzagInv,
                                 I16_WMV * rgiCoefRecon,
                                 I32_WMV uiTCOEF_ESCAPE,
                                 U32_WMV uiNumCoefs,
                                 I32_WMV uiStartIndxOfLastRun,
                                 U32_WMV * puiCoefCounter,
                                 I32_WMV * pvalue,
                                 U32_WMV  * puiRun,
                                 I32_WMV * pSignBit,
                                 I32_WMV *piDCTHorzFlags
                                 )
{
    
    I32_WMV lIndex;
    Bool_WMV bDone;
    U32_WMV uiRun;
    I32_WMV iLevel;
    I32_WMV signBit;
    U32_WMV uiCoefCounter = *puiCoefCounter;
    I32_WMV iDCTHorzFlags = *piDCTHorzFlags;
    I32_WMV value;
    I32_WMV iIndex;
    
    do {
        //lIndex = Huffman_WMV_get_peek(hufDCTACDec, pBitStrm, &signBit);
        lIndex = Huffman_WMV_get(hufDCTACDec, pBitStrm);
        signBit = - (I32_WMV)BS_getBit(pBitStrm);
        
        bDone = ( lIndex == uiTCOEF_ESCAPE);
        
        
        uiRun= (U8_WMV)rgLevelRunAtIndx[lIndex];
        iLevel= rgLevelRunAtIndx[lIndex] >>8;
        
        uiCoefCounter += uiRun;
        
        bDone |= (uiCoefCounter >= uiNumCoefs);
        // bDone |= BS_invalid(pWMVDec->m_pbitstrmIn) ;
        bDone |= ( lIndex >= uiStartIndxOfLastRun);
        iIndex =  pZigzagInv [uiCoefCounter];
        
        iDCTHorzFlags |=  g_IDCTShortCutLUT[iIndex];
        
        value = ( iLevel ^ signBit) - signBit;
        
        ( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)value;
        
        uiCoefCounter++;
    } while (bDone == 0) ; //!bIsLastRun);
    
    
    *puiCoefCounter = uiCoefCounter;
    *pvalue  = value;
    *puiRun = uiRun;
    *pSignBit = signBit;
    *piDCTHorzFlags = iDCTHorzFlags;
    
    return lIndex;
}
/*
static int g_dbg_cnt5;
static int g_dbg_cnt6;
*/
//#if !defined(WMV_OPT_DQUANT_ARM)
//#pragma code_seg (EMBSEC_PML)
tWMVDecodeStatus DecodeInverseInterBlockQuantize16_EMB(EMB_PBMainLoop* pMainLoop, 
                                                       CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, 
                                                       I32_WMV XFormType,
                                                       DQuantDecParam *pDQ,
                                                       I16_WMV* buffer)
{
    I32_WMV iDoubleStepSize = pDQ->iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
    CDCTTableInfo_Dec* InterDCTTableInfo_Dec = ppInterDCTTableInfo_Dec[0];
    Huffman_WMV* hufDCTACDec = InterDCTTableInfo_Dec -> hufDCTACDec;
    I16_WMV * rgLevelRunAtIndx = InterDCTTableInfo_Dec -> combined_levelrun;
    // I8_WMV* rgLevelAtIndx = InterDCTTableInfo_Dec -> pcLevelAtIndx;
    // U8_WMV* rgRunAtIndx = InterDCTTableInfo_Dec -> puiRunAtIndx;
    I32_WMV uiStartIndxOfLastRun = InterDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1;
    I32_WMV  uiTCOEF_ESCAPE = InterDCTTableInfo_Dec -> iTcoef_ESCAPE;
    U32_WMV uiRun; // = 0;
    I32_WMV  iLevel; // = 0;
    U32_WMV uiCoefCounter = 0;
    I32_WMV signBit;
    I32_WMV value;
    I32_WMV iDCTHorzFlags;
    I32_WMV iIndex;
    I16_WMV *rgiCoefRecon = buffer;//(I16_WMV*)pMainLoop->m_rgiCoefReconBuf;
    I32_WMV lIndex;
    U32_WMV uiNumCoefs = pMainLoop->m_iNumCoefTbl[XFormType];    
    Bool_WMV bNotDone;
    tWMVDecodeStatus result;
    U8_WMV *pZigzagInv; 
 

    //uiNumCoefs = 64;

    if (XFormType == XFORMMODE_8x8)
        pZigzagInv = pMainLoop->m_pZigzagScanOrder;
    else
        pZigzagInv = pMainLoop->m_pZigzag[XFormType-1];
    
    g_ResetCoefBuffer[XFormType] ((I32_WMV *)rgiCoefRecon);
    
    // memset (rgiCoefRecon, 0, 128*sizeof(I16_WMV));
    iDCTHorzFlags = 0;
    
    do {
        
        do {
            lIndex = Huffman_WMV_get(hufDCTACDec, pMainLoop->m_pbitstrmIn);
            signBit = - (I32_WMV)BS_getBit(pMainLoop->m_pbitstrmIn); 
            //lIndex = Huffman_WMV_get_peek(hufDCTACDec, pWMVDec->m_pbitstrmIn, &signBit);
            
            bNotDone = ( lIndex != uiTCOEF_ESCAPE);
            //   uiRun = rgRunAtIndx[lIndex];
            //    iLevel = rgLevelAtIndx[lIndex];
            
            uiRun= (U8_WMV)rgLevelRunAtIndx[lIndex];
            iLevel= rgLevelRunAtIndx[lIndex] >>8;
            
            uiCoefCounter += uiRun;
            
            bNotDone &= (uiCoefCounter < uiNumCoefs);
            // bDone |= BS_invalid(pWMVDec->m_pbitstrmIn) ;
            bNotDone &= ( lIndex < uiStartIndxOfLastRun);
            iIndex =  pZigzagInv [uiCoefCounter];
            
            iDCTHorzFlags |=  pMainLoop->m_IDCTShortCutLUT[iIndex];

            {
                value = ((iDoubleStepSize * iLevel + iStepMinusStepIsEven) ^ signBit) - signBit;

                 //if(value)
                 {
                    if((iIndex&1)== 0) {
                        *(I32_WMV *)( rgiCoefRecon + iIndex ) += (I32_WMV)value;
                    }
                    else {
        #ifndef _BIG_ENDIAN_
                        ( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)value;
        #else
                        *(I32_WMV *)( rgiCoefRecon + (iIndex-1)) += (I32_WMV)value << 16;
        #endif //_BIG_ENDIAN_
                    }
                }
            }
            
            uiCoefCounter++;
        } while (bNotDone) ; //!bIsLastRun);
        
        uiCoefCounter--;
        
        if (BS_invalid(pMainLoop->m_pbitstrmIn) | (uiCoefCounter >= uiNumCoefs)) {
            return WMV_CorruptedBits;
        }
        if(lIndex != uiTCOEF_ESCAPE) {
#ifdef STABILITY
            if(lIndex < uiStartIndxOfLastRun)
                return WMV_CorruptedBits;
#endif
            break;
        }
        
        //undo the damage first
        
        if (XFormType < 8)
        {
            if((pZigzagInv [uiCoefCounter]&1)== 0) {
                *(I32_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] ) -= (I32_WMV)value;
            }
            else {
#ifndef _BIG_ENDIAN_            
                ( *(I16_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] )) -= (I16_WMV)value;
#else
                ( *(I32_WMV *)( rgiCoefRecon + (pZigzagInv [uiCoefCounter&63]-1) )) -= (I32_WMV)value << 16;
#endif //_BIG_ENDIAN_
            }
        }
       
        uiCoefCounter -= (U8_WMV)rgLevelRunAtIndx[lIndex];
      
        
        result = DecodeInverseInterBlockQuantize16_level2_EMB(pMainLoop->m_pWMVDec, 
            InterDCTTableInfo_Dec,
            uiNumCoefs,
            &uiCoefCounter,
            &iLevel,
            &signBit,
            (U32_WMV*)&lIndex
            );
        
        if(result != WMV_Succeeded)
            return result;       
        
        iIndex =  pZigzagInv [uiCoefCounter];
        iDCTHorzFlags |=  pMainLoop->m_IDCTShortCutLUT[iIndex];
        
        value = ((iDoubleStepSize * iLevel + iStepMinusStepIsEven) ^ signBit) - signBit;
        
        // we have a conditional negative depending on the sign bit. If 
        // sign == -1 or 1, we want 
        // output = sign * (iDoubleStepSize * iLevel + iStepMinusStepIsEven)
        // however, sign is 0 or 1.  note we essentially have a conditional negative.
        // recall -X = X' + 1 
        // we generate a mask from the sign bit
       

        if((iIndex&1)== 0) {
            *(I32_WMV *)( rgiCoefRecon + iIndex ) += (I32_WMV)value;
        }
        else {
#ifndef _BIG_ENDIAN_
            ( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)value;
#else
            *(I32_WMV *)( rgiCoefRecon + (iIndex-1)) += (I32_WMV)value << 16;
#endif  //_BIG_ENDIAN_
        }


        uiCoefCounter++;
    } while (lIndex < uiStartIndxOfLastRun) ; //!bIsLastRun);
    
    // Save the DCT row flags. This will be passed to the IDCT routine
    pMainLoop->m_iDCTHorzFlags = iDCTHorzFlags;

    return WMV_Succeeded;
}

tWMVDecodeStatus DecodeInverseInterBlockQuantize16_EMB_new(EMB_PBMainLoop* pMainLoop, 
                                                       CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, 
                                                       I32_WMV XFormType,
                                                       DQuantDecParam *pDQ,
                                                       I16_WMV* buffer)
{
    I32_WMV iDoubleStepSize = pDQ->iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
    CDCTTableInfo_Dec* InterDCTTableInfo_Dec = ppInterDCTTableInfo_Dec[0];
    Huffman_WMV* hufDCTACDec = InterDCTTableInfo_Dec -> hufDCTACDec;
    I16_WMV * rgLevelRunAtIndx = InterDCTTableInfo_Dec -> combined_levelrun;
    // I8_WMV* rgLevelAtIndx = InterDCTTableInfo_Dec -> pcLevelAtIndx;
    // U8_WMV* rgRunAtIndx = InterDCTTableInfo_Dec -> puiRunAtIndx;
    I32_WMV uiStartIndxOfLastRun = InterDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1;
    I32_WMV  uiTCOEF_ESCAPE = InterDCTTableInfo_Dec -> iTcoef_ESCAPE;
    U32_WMV uiRun; // = 0;
    I32_WMV  iLevel; // = 0;
    U32_WMV uiCoefCounter = 0;
    I32_WMV signBit;
    I32_WMV value;
    I32_WMV iDCTHorzFlags;
    I32_WMV iIndex;
    I16_WMV *rgiCoefRecon = buffer;//(I16_WMV*)pMainLoop->m_rgiCoefReconBuf;
    I32_WMV lIndex;
    U32_WMV uiNumCoefs = pMainLoop->m_iNumCoefTbl[XFormType];    
    Bool_WMV bNotDone;
    tWMVDecodeStatus result;
    U8_WMV *pZigzagInv; 
 

    //uiNumCoefs = 64;

    if (XFormType == XFORMMODE_8x8)
        pZigzagInv = pMainLoop->m_pZigzagScanOrder;
    else
        pZigzagInv = pMainLoop->m_pZigzag[XFormType-1];
    
    g_ResetCoefBuffer[XFormType] ((I32_WMV *)rgiCoefRecon);
    
    // memset (rgiCoefRecon, 0, 128*sizeof(I16_WMV));
    iDCTHorzFlags = 0;
    
    do {
        
        do {
            lIndex = Huffman_WMV_get(hufDCTACDec, pMainLoop->m_pbitstrmIn);
            signBit = - (I32_WMV)BS_getBit(pMainLoop->m_pbitstrmIn); 
            //lIndex = Huffman_WMV_get_peek(hufDCTACDec, pWMVDec->m_pbitstrmIn, &signBit);
            
            bNotDone = ( lIndex != uiTCOEF_ESCAPE);
            //   uiRun = rgRunAtIndx[lIndex];
            //    iLevel = rgLevelAtIndx[lIndex];
            
            uiRun= (U8_WMV)rgLevelRunAtIndx[lIndex];
            iLevel= rgLevelRunAtIndx[lIndex] >>8;
            
            uiCoefCounter += uiRun;
            
            bNotDone &= (uiCoefCounter < uiNumCoefs);
            bNotDone &= ( lIndex < uiStartIndxOfLastRun);
            iIndex =  pZigzagInv [uiCoefCounter];
            
            iDCTHorzFlags |=  pMainLoop->m_IDCTShortCutLUT[iIndex];

            {//1246
                value = ((iDoubleStepSize * iLevel + iStepMinusStepIsEven) ^ signBit) - signBit;
				( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)value;
            }
            
            uiCoefCounter++;
        } while (bNotDone) ; //!bIsLastRun);
        
        uiCoefCounter--;
        
        if (BS_invalid(pMainLoop->m_pbitstrmIn) | (uiCoefCounter >= uiNumCoefs)) {
            return WMV_CorruptedBits;
        }
        if(lIndex != uiTCOEF_ESCAPE) {
#ifdef STABILITY
            if(lIndex < uiStartIndxOfLastRun)
                return WMV_CorruptedBits;
#endif
            break;
        }
        
        //undo the damage first        
        if (XFormType < 8)
        {  
			(*(I16_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] )) -= (I16_WMV)value;
        }    

        uiCoefCounter -= (U8_WMV)rgLevelRunAtIndx[lIndex];      
        
        result = DecodeInverseInterBlockQuantize16_level2_EMB(pMainLoop->m_pWMVDec, 
            InterDCTTableInfo_Dec,
            uiNumCoefs,
            &uiCoefCounter,
            &iLevel,
            &signBit,
            (U32_WMV*)&lIndex );
        
        if(result != WMV_Succeeded)
            return result;       
        
        iIndex =  pZigzagInv [uiCoefCounter];
        iDCTHorzFlags |=  pMainLoop->m_IDCTShortCutLUT[iIndex];
        
        value = ((iDoubleStepSize * iLevel + iStepMinusStepIsEven) ^ signBit) - signBit;       
		( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)value;

        uiCoefCounter++;
    } while (lIndex < uiStartIndxOfLastRun) ; //!bIsLastRun);
    
    // Save the DCT row flags. This will be passed to the IDCT routine
    pMainLoop->m_iDCTHorzFlags = iDCTHorzFlags;
	pMainLoop->m_iDctRunFlags = iIndex;

    return WMV_Succeeded;
}
//#endif //WMV_OPT_DQUANT_ARM

// static int g_dbg_cnt_error;
// static int g_dbg_cnt_noerror;
//static int g_dbg_cnt;
//#pragma code_seg (EMBSEC_PML)
I32_WMV g_MotionComp_X9_EMB (EMB_PMainLoop *pPMainLoop,I32_WMV irows,I32_WMV icols, I32_WMV iblk, Bool_WMV b1MV,EMB_PBMainLoop *pPB)

{
    I32_WMV iUV = iblk>>2;
    I32_WMV  iFilterType = pPB->m_iFilterType[iUV];
    U8_WMV  *pRef;
    CDiffMV_EMB * pDiffMV =  &pPB->pDiffMV_EMB_Bak[iblk];
    I32_WMV  iShiftX;
    I32_WMV  iShiftY;
    tWMVDecInternalMember *pWMVDec = pPB->m_pWMVDec;
    I32_WMV iRows = irows;
    I32_WMV iCols = icols; 
     int xIndex = iCols<<4;
     int yIndex = iRows<<4;

    I32_WMV iXFrac, iYFrac;

	iShiftX = pDiffMV->Diff.I16[X_INDEX];
	iShiftY = pDiffMV->Diff.I16[Y_INDEX];

    if( pWMVDec->m_bCodecIsWVC1) //0815
    { //Y 
       
        if(!iUV)
        {            
            PullBackMotionVector (pWMVDec, &iShiftX, &iShiftY, iCols, iRows);
            if( (iShiftX>>2) + xIndex <= -32)  //zou fix
			    iShiftX =  (-32-xIndex)<<2;
		    else	if( (iShiftX>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)
			    iShiftX = (pWMVDec->m_iWidthPrevY+32 -xIndex)<<2;

		    if((iShiftY>>2) + yIndex <= -32)
			    iShiftY =  (-32-yIndex)<<2;
		    else	if((iShiftY>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)
			    iShiftY = (pWMVDec->m_iHeightPrevY+32 -yIndex)<<2;
        }
        else
        {
           //int iIndex = iRows * (I32_WMV) pWMVDec->m_uintNumMBX + iCols;
           PullBackMotionVector_UV (pWMVDec, &iShiftX, &iShiftY, iCols, iRows);  //zou fix 

		    if( (iShiftX>>2) + xIndex/2 <= -16)   //zou fix
			    iShiftX =  (-16-xIndex/2)<<2;
		    else	if( (iShiftX>>2) +xIndex/2 >= pWMVDec->m_iWidthPrevUV+16)
			    iShiftX = (pWMVDec->m_iWidthPrevUV+16 -xIndex/2)<<2;

		    if((iShiftY>>2) + yIndex/2 <= -16)
			    iShiftY =  (-16-yIndex/2)<<2;
		    else	if((iShiftY>>2) +yIndex/2 >= pWMVDec->m_iHeightPrevUV+16)
			    iShiftY = (pWMVDec->m_iHeightPrevUV+16 -yIndex/2)<<2;
        }

         /*pRef = pPB->m_ppxliRefBlkBase[iblk] + pPB->m_iFrmMBOffset[iUV];
         pRef = pRef + (iShiftY >> 2) * pPB->m_iWidthPrev[iUV] + (iShiftX >> 2);*/
    }
    else
    {
        //pRef = pPB->m_ppxliRefBlkBase[iblk] +pPB->m_iFrmMBOffset[iUV];
        //pRef = pRef + (iShiftY >> 2) * pPB->m_iWidthPrev[iUV] + (iShiftX >> 2);
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
        pRef = pPB->m_ppxliRefBlkBase[iblk]  + iRows*16*pWMVDec->m_iWidthPrevY+iCols*16 ;
    }
    else
    {
        pRef = pPB->m_ppxliRefBlkBase[iblk]  + iRows*8*pWMVDec->m_iWidthPrevUV+iCols*8;
    }
    pRef = pRef + (iShiftY >> 2) * pPB->m_iWidthPrev[iUV] + (iShiftX >> 2);

    //pRef = pPB->m_ppxliRefBlkBase[iblk] + pPB->m_iFrmMBOffset[iUV];
    //pRef = pRef + (iShiftY >> 2) * pPB->m_iWidthPrev[iUV] + (iShiftX >> 2);
#ifdef STABILITY_NONE
    if(pPB->m_iFrmMBOffset[0] != iRows*16*pWMVDec->m_iWidthPrevY+iCols*16  
        || pPB->m_iFrmMBOffset[1] != iRows*8*pWMVDec->m_iWidthPrevUV+iCols*8)
    {
        return -1;
    }
#endif
    
    iXFrac = iShiftX&3;
    iYFrac = iShiftY&3;
    
    //g_dbg_cnt++;

    if(iFilterType == FILTER_BICUBIC)
    {
        I_SIMD ret;
		//sw: src:pRef, dst:m_rgMotionCompBuffer_EMB
        ret = (*pPB->m_pInterpolateBicubic[iXFrac][iYFrac])
                (pRef, pPB->m_iWidthPrev[iUV], (U8_WMV *)pPB->prgMotionCompBuffer_EMB, pPB, iXFrac, iYFrac, b1MV, pPB->m_rgiNewVertTbl[iUV][b1MV]);
                /*(pRef, pPB->m_iWidthPrev[iUV], (U8_WMV *)pPMainLoop->m_rgMotionCompBuffer_EMB, pPB, iXFrac, iYFrac, b1MV, pPB->m_rgiNewVertTbl[iUV][b1MV]);*/

            if(ret) {
                g_InterpolateBlockBicubicOverflow_EMB(pRef, pPB->m_iWidthPrev[iUV], (U8_WMV *)pPB->prgMotionCompBuffer_EMB,  iXFrac,  iYFrac,  pPB->m_iRndCtrl, b1MV);
            }
    }
    else
    {
        if(iXFrac|iYFrac) {
            pPB->m_pInterpolateBlockBilinear[iXFrac][iYFrac]( pRef, pPB->m_iWidthPrev[iUV], (U8_WMV *)pPB->prgMotionCompBuffer_EMB, iXFrac, iYFrac, pPB->m_iRndCtrl, b1MV);         
        }
        else
           pPB->m_pInterpolateBlock_00(pRef, pPB->m_iWidthPrev[iUV], (U8_WMV *)pPB->prgMotionCompBuffer_EMB, pPB, iXFrac, iYFrac,  b1MV, 0);
    }
    return 0;

}

//extern U8_WMV pLUT[256];
//extern U8_WMV pLUTUV[256];
I32_WMV g_MotionComp_X9_EMB_new (EMB_PMainLoop *pPMainLoop,
                                 U8_WMV* pDst,
                                 I32_WMV iDstStride,
                                 I32_WMV irows,
                                 I32_WMV icols,
                                 I32_WMV iblk, 
                                 Bool_WMV b1MV,
                                 EMB_PBMainLoop *pPB)

{
	VO_S32 ref_pixel_row = 0;
    I32_WMV iUV = iblk>>2;
    I32_WMV  iFilterType = pPB->m_iFilterType[iUV];
    U8_WMV  *pRef;
    CDiffMV_EMB * pDiffMV =  &pPB->pDiffMV_EMB_Bak[iblk];
    I32_WMV  iShiftX;
    I32_WMV  iShiftY;
    tWMVDecInternalMember *pWMVDec = pPB->m_pWMVDec;
    I32_WMV iRows = irows;
    I32_WMV iCols = icols; 
     int xIndex = iCols<<4;
     int yIndex = iRows<<4;
	 U8_WMV ref_pro[20*20];
	 I32_WMV i=0,j=0,w=12,h=12;
	 U8_WMV *pReference = NULL,*PTab=NULL,*pRefTMP =NULL;
	 U32_WMV ref_stride = 0;
	 I32_WMV iXFrac, iYFrac;   
 #ifdef USE_FRAME_THREAD
	 volatile I32_WMV *ref_decodeprocess = &(pWMVDec->m_pfrmRef0Q->m_decodeprocess);
#endif

	iShiftX = pDiffMV->Diff.I16[X_INDEX];
	iShiftY = pDiffMV->Diff.I16[Y_INDEX];

    if( pWMVDec->m_bCodecIsWVC1) //0815
    { //Y        
        if(!iUV)
        { 
            PullBackMotionVector (pWMVDec, &iShiftX, &iShiftY, iCols, iRows);
            if( (iShiftX>>2) + xIndex <= -32)  //zou fix
			    iShiftX =  (-32-xIndex)<<2;
		    else	if( (iShiftX>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)
			    iShiftX = (pWMVDec->m_iWidthPrevY+32 -xIndex)<<2;

		    if((iShiftY>>2) + yIndex <= -32)
			    iShiftY =  (-32-yIndex)<<2;
		    else	if((iShiftY>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)
			    iShiftY = (pWMVDec->m_iHeightPrevY+32 -yIndex)<<2;
        }
        else
        {			
           //int iIndex = iRows * (I32_WMV) pWMVDec->m_uintNumMBX + iCols;
           PullBackMotionVector_UV (pWMVDec, &iShiftX, &iShiftY, iCols, iRows);  //zou fix 

		    if( (iShiftX>>2) + xIndex/2 <= -16)   //zou fix
			    iShiftX =  (-16-xIndex/2)<<2;
		    else	if( (iShiftX>>2) +xIndex/2 >= pWMVDec->m_iWidthPrevUV+16)
			    iShiftX = (pWMVDec->m_iWidthPrevUV+16 -xIndex/2)<<2;

		    if((iShiftY>>2) + yIndex/2 <= -16)
			    iShiftY =  (-16-yIndex/2)<<2;
		    else	if((iShiftY>>2) + yIndex/2 >= pWMVDec->m_iHeightPrevUV+16)
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

	//TODO tongbu
    if(!iUV)
    {
#ifdef USE_FRAME_THREAD
		if(pWMVDec->CpuNumber > 1)
		{
			ref_pixel_row = (iRows)*16+ (iShiftY >> 2) + 20; //20 for imv MV 20*20 of ref is needed.
			while((I32_WMV)(ref_pixel_row/16) >= *ref_decodeprocess)
			{
				thread_sleep(0);
				bkprintf("p1");
			}
		}
#endif
        pRef = pPB->m_ppxliRefBlkBase[iblk]  + iRows*16*pWMVDec->m_iWidthPrevY+iCols*16 ;
    }
    else
    {
#ifdef USE_FRAME_THREAD
		if(pWMVDec->CpuNumber > 1)
		{
			ref_pixel_row = (iRows)*8+ (iShiftY >> 2) + 12; //12 for imv MV 20*20 of ref is needed.
			while((I32_WMV)(ref_pixel_row/8) >= *ref_decodeprocess)
			{
				thread_sleep(0);
				bkprintf("p2");
			}
		}
#endif
        pRef = pPB->m_ppxliRefBlkBase[iblk]  + iRows*8*pWMVDec->m_iWidthPrevUV+iCols*8;
    }
    pRef = pRef + (iShiftY >> 2) * pPB->m_iWidthPrev[iUV] + (iShiftX >> 2);
	pReference = pRef;
	ref_stride = pPB->m_iWidthPrev[iUV];

	//ZOU TODO
	if (pWMVDec->m_bLuminanceWarp|| pWMVDec->m_scaling ) 
	{
		U32_WMV stride=0;
		if(!iUV)
		 {
			 if(b1MV)
				 w = h = 20;
			 PTab = pWMVDec->m_pLUT;
		 }
		 else
			 PTab = pWMVDec->m_pLUTUV;

		pRefTMP =  pRef-pPB->m_iWidthPrev[iUV]-1;
		stride = pPB->m_iWidthPrev[iUV];

		if(pWMVDec->m_scaling){
			AdjustReconRange_MB(pRefTMP,stride, ref_pro,20,w,pWMVDec->m_scaling==1);			
			pRefTMP = ref_pro;
			stride = 20;
		}
       
		if(pWMVDec->m_bLuminanceWarp)
		{
			if(PTab == NULL)
				return -1;
			IntensityCompensation_MB(pRefTMP,stride,ref_pro,20,PTab,w);
		}

		pReference = ref_pro+20+1;
		ref_stride = 20;
	}
    
    iXFrac = iShiftX&3;
    iYFrac = iShiftY&3;
   
    //new MC
    if(iFilterType == FILTER_BICUBIC)
    {
        I_SIMD ret;
		//sw: src:pRef, dst:m_rgMotionCompBuffer_EMB
        ret = (*pPB->m_pInterpolateBicubic_new[iXFrac][iYFrac])
                    (pReference, ref_stride, pDst, iDstStride, pPB, iXFrac, iYFrac, b1MV);
        //z0++;
    }
    else
    {
        if(iXFrac|iYFrac) 
            pPB->m_pInterpolateBlockBilinear_new[iXFrac][iYFrac]
                ( pReference, ref_stride, pDst, iDstStride, iXFrac, iYFrac, pPB->m_iRndCtrl, b1MV);         
        else
           pPB->m_pInterpolateBlock_00_new(pReference,ref_stride, pDst, iDstStride, pPB, iXFrac, iYFrac,  b1MV);
        //z1++;
    }

    return 0;
}



#define INTRADCYTCOEF_ESCAPE_MSV 119                            // see table.13/H.263
#define INTRADCUVTCOEF_ESCAPE_MSV 119    
// see table.13/H.263

//#pragma code_seg (EMBSEC_PML2)
//This is not really an MMX routine but works along side the MMX idct
tWMVDecodeStatus DecodeInverseIntraBlockX9_EMB(tWMVDecInternalMember *pWMVDec,
                                               CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
                                               I32_WMV iblk,   U8_WMV *pZigzagInv,   Bool_WMV bResidual,
                                               I16_WMV *pPred, I32_WMV iDirection, I32_WMV iShift,
                                               I16_WMV *pIntra, CWMVMBMode* pmbmd,
                                               DQuantDecParam *pDQ,
                                               I16_WMV* buffer
                                               )
{
    I32_WMV iDoubleStepSize = pDQ->iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
    I32_WMV iDCStepSize = pDQ->iDCStepSize;
    I32_WMV iValue0, iValue1;
    //Align on cache line
    I16_WMV *rgiCoefRecon = (I16_WMV*)buffer;//(&pMbMode->pBackUpErrorInter[iblk][0]);//(I16_WMV*)(&pmbmd->pBackUpError[iblk]);

    I32_WMV i;
    tWMVDecodeStatus result;
    
    memset (rgiCoefRecon, 0, (size_t) (BLOCK_SQUARE_SIZE_2>>1));
    
    // g_dbg_cnt_intra_total++;
    
    if (iblk < 4)
        decodeIntraDCAcPredMSV (pWMVDec, &result, pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV, iDCStepSize);
    else 
        decodeIntraDCAcPredMSV (pWMVDec, &result, pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV, iDCStepSize);
    // decodeIntraDCAcPredMSV sets pWMVDec->m_rgiCoefRecon[0] be to the DC component
    // but pWMVDec->m_rgiCoefRecon is I32_WMV array and we are using it as a 16-bit array
    // through rgiCoefRecon,  so we need to zero out rgiCoefRecon[1], also,
    // i set rgiCoefRecon[0] by typecasting just in case some other endian order   
    // is used...  
    
    pWMVDec->m_iDCTHorzFlags = 0;
    
    rgiCoefRecon[0] = (I16_WMV) pWMVDec->m_rgiCoefRecon[0];
    rgiCoefRecon[1] = 0;  
    
    if (WMV_Succeeded != result) {
        return result;
    }
    
    if (bResidual) {
        CDCTTableInfo_Dec* IntraDCTTableInfo_Dec = ppIntraDCTTableInfo_Dec[0];         
        Huffman_WMV* hufDCTACDec = IntraDCTTableInfo_Dec -> hufDCTACDec;
        
        I16_WMV * rgLevelRunAtIndx = IntraDCTTableInfo_Dec -> combined_levelrun;
        
        U32_WMV uiStartIndxOfLastRun = IntraDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1;
        U32_WMV uiTCOEF_ESCAPE = IntraDCTTableInfo_Dec -> iTcoef_ESCAPE;
        U32_WMV uiNumCoefs = 64;
        U32_WMV uiRun; // = 0;
        I32_WMV iLevel; // = 0;
        U32_WMV uiCoefCounter = 1;
        I32_WMV iDCTHorzFlags = 0;
        I32_WMV signBit;
        I32_WMV iIndex;
        U32_WMV lIndex;           
        
        do
        {
            lIndex = DecodeIntra16_level1_EMB( hufDCTACDec, 
                                                            pWMVDec->m_pbitstrmIn, 
                                                            rgLevelRunAtIndx, 
                                                            pZigzagInv,
                                                            rgiCoefRecon,
                                                            uiTCOEF_ESCAPE,
                                                            uiNumCoefs,
                                                            uiStartIndxOfLastRun,
                                                            &uiCoefCounter,
                                                            &iLevel,
                                                            &uiRun,
                                                            &signBit,
                                                            &iDCTHorzFlags );
            
            uiCoefCounter--;
            
            if (BS_invalid(pWMVDec->m_pbitstrmIn) | (uiCoefCounter >= uiNumCoefs)) {
                return WMV_CorruptedBits;
            }
            if(lIndex != uiTCOEF_ESCAPE) {
#ifdef STABILITY
                if(lIndex < uiStartIndxOfLastRun)
                    return WMV_CorruptedBits;
#endif
                break;
            }
            
            //undo the damage first
            
            ( *(I16_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] )) -= (I16_WMV)iLevel;
            
            uiCoefCounter -= uiRun;
            
            //g_dbg_cnt6++;
            
            result = DecodeInverseInterBlockQuantize16_level2_EMB(pWMVDec, 
                                                                                    IntraDCTTableInfo_Dec,
                                                                                    uiNumCoefs,
                                                                                    &uiCoefCounter,
                                                                                    &iLevel,
                                                                                    &signBit,
                                                                                    &lIndex );
            
            if(result != WMV_Succeeded)
                return result;           
            
            iIndex =  pZigzagInv [uiCoefCounter];
            iDCTHorzFlags |=  g_IDCTShortCutLUT[iIndex];
            
            iLevel = ((iLevel ) ^ signBit) - signBit;
            
            // we have a conditional negative depending on the sign bit. If 
            // sign == -1 or 1, we want 
            // output = sign * (iDoubleStepSize * iLevel + iStepMinusStepIsEven)
            // however, sign is 0 or 1.  note we essentially have a conditional negative.
            // recall -X = X' + 1 
            // we generate a mask from the sign bit
            
            ( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)iLevel;
            
            uiCoefCounter++;
        } while (lIndex < uiStartIndxOfLastRun) ; //!bIsLastRun);
        // Save the DCT row flags. This will be passed to the IDCT routine
        pWMVDec->m_iDCTHorzFlags = iDCTHorzFlags;
    }
    
    // add prediction of dct-ac
    if (pPred)
    {        
        if (iDirection) {
            pPred += pWMVDec->m_iACRowPredOffset;
        }    
        else { // left
            pPred += pWMVDec->m_iACColPredOffset;
        }    
        
        rgiCoefRecon[0] += pPred[0];
        if (iShift != -1) {           
            I32_WMV i;
            
#ifdef STABILITY
            if(iShift != 3&& iShift!= 0)
                return WMV_CorruptedBits;
#endif
            for ( i = 1; i < 8; i++)
                rgiCoefRecon[i << iShift] += pPred[i];
            
            pWMVDec->m_iDCTHorzFlags |= 1;
        }
    }
    
    for ( i = 0; i < 8; i++)
    {
        pIntra[i] = rgiCoefRecon[i];
        pIntra[i + 8] = rgiCoefRecon[i << 3];
    }
    

    // Dequantize DCT-DC --> move to mc part
#if 0  // VOARMV7
	//Disable this v7 optimization now,because there is bug. For example,file mml0057_352x288.rcv .
	ARMV7_IntraBlockDequant8x8(iDCStepSize,iDoubleStepSize,iStepMinusStepIsEven,rgiCoefRecon);
#else    
	if(0){
        //I32_WMV iDoubleStepSize = pDQ->iDoubleStepSize;
        //I32_WMV iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
        //I32_WMV iDCStepSize = pDQ->iDCStepSize;

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
#endif	//VOARMV7W
    
    return WMV_Succeeded;
}

//static int g_dbg_cnt = 0;

// static int g_dbg_cnt_total;
//#pragma code_seg (EMBSEC_PML)
tWMVDecodeStatus DecodeMB_X9_Fast_Parser (EMB_PMainLoop  * pPMainLoop,
                                                       CWMVMBMode* pmbmd, 
                                                       CWMVMBMode* pmbmd_lastrow,
                                                       I32_WMV iMBX, I32_WMV iMBY,
                                                       Bool_WMV b1MV,EMB_PBMainLoop *pPB)
{   
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    tWMVDecodeStatus result = WMV_Succeeded;
    I32_WMV iblk;
    Bool_WMV bSkipMB = TRUE_WMV;
    const I32_WMV iCurrMB = pPB->m_rgiCurrBaseBK[1];
    I32_WMV i1MVCnt = b1MV;
    CDiffMV_EMB * pDiffMV = pPB->m_pDiffMV_EMB;
    Bool_WMV bFirstBlock =  pPB->m_bMBXformSwitching;
    U32_WMV iSubblockPattern = pmbmd->m_iSubblockPattern;
    I32_WMV iXformType = pPB->m_iFrameXformMode;
    DQuantDecParam *pDQ = &pPB->m_prgDQuantParam [pmbmd->m_iQP ];    

    //int keepidct = 0;

    CWMVMBModeBack *pMbMode = pPB->m_pMbMode + iMBX;//+ iMBY*pPB->m_uintNumMBX;
    
    if (pPB->m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;
    if (pPB->m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pPB->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pPB->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    }
    else {
        ppInterDCTTableInfo_Dec = pPB->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pPB->m_ppIntraDCTTableInfo_Dec;
    }

    for (iblk = 0; iblk < 6; iblk++) {
        U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
        UnionBuffer * ppxliErrorQMB;
        I32_WMV iIntra = 0, iShift, iDirection;
        I16_WMV  *pIntra, *pIntra_lastrow,pPredScaled[16];
        //U8_WMV * pDst;
        I32_WMV iUV;
        Bool_WMV bResidual = rgCBP2[iblk];

		iUV = iblk >> 2;
        i1MVCnt -= iUV;

        iIntra = pDiffMV->iIntra;
        pDiffMV++;
        if (!iUV &&(iCurrMB||iblk || iIntra || bResidual ))
            bSkipMB = FALSE_WMV;
        
        if (iIntra)
        {
            I32_WMV iX, iY;
            Bool_WMV bDCACPredOn;
            I16_WMV *pPred = NULL_WMV;
            CDCTTableInfo_Dec** ppDCTTableInfo_Dec;
            
            if(!iUV)
            {
                iX = (iMBX << 1) + (iblk & 1);
                iY = (iMBY << 1) + ((iblk & 2) >> 1);
                //pIntra = pPB->m_pX9dct + ((pPB->m_rgiCurrBaseBK[0] + pPB->m_rgiBlkIdx[iblk]) << 4);
#ifdef STABILITY
                if(pPB->m_pX9dct==NULL)
                    return -1;
#endif
                pIntra = pPB->m_pX9dct + ((iX + ((1 << 1) + ((iblk & 2) >> 1))*pPB->m_iNumBlockX) << 4);
                pIntra_lastrow =  pIntra-16*pPB->m_iNumBlockX;
                ppDCTTableInfo_Dec = ppIntraDCTTableInfo_Dec;

                if((iMBY&1) == 0)
                {
                    pIntra = pPB->m_pX9dct + ((iX + ((1 << 1) + ((iblk & 2) >> 1))*pPB->m_iNumBlockX) << 4);
                    pIntra_lastrow =  pIntra-16*pPB->m_iNumBlockX;
                }
                else
                {
                    if(iblk == 0 || iblk ==1)
                        pIntra_lastrow = pIntra + 16*pPB->m_iNumBlockX ;
                    else
                        pIntra_lastrow = pIntra - 3*16*pPB->m_iNumBlockX;

                    pIntra = pIntra - 2*16*pPB->m_iNumBlockX;//pPB->m_pX9dct + ((iX + ((0 << 1) + ((iblk & 2) >> 1))*pPB->m_iNumBlockX) << 4);
                }
            }
            else
            {
                if(iMBY > 0)
                    iMBY = iMBY;
                bSkipMB = FALSE_WMV;
                iX = iMBX; iY = iMBY;
                //pIntra = pPB->m_pX9dctUV[iblk-4] + ((pPB->m_rgiCurrBaseBK[1]) << 4);
                //pIntra = pPB->m_pX9dctUV[iblk-4] + ((1 *pPB->m_pWMVDec->m_uintNumMBX + iX) << 4);
                //pIntra_lastrow =  pIntra-16*pPB->m_pWMVDec->m_uintNumMBX;//pPB->m_pX9dctUV[iblk-4] + ((0 *pPB->m_pWMVDec->m_uintNumMBX + iX) << 4);
                ppDCTTableInfo_Dec = ppInterDCTTableInfo_Dec;

                if((iMBY&1) == 0)
                {
                    pIntra = pPB->m_pX9dctUV[iblk-4] + ((1 *pPB->m_pWMVDec->m_uintNumMBX + iX) << 4);
                    pIntra_lastrow =  pIntra-16*pPB->m_pWMVDec->m_uintNumMBX;//pPB->m_pX9dctUV[iblk-4] + ((0 *pPB->m_pWMVDec->m_uintNumMBX + iX) << 4);

                }
                else
                {
                    pIntra_lastrow = pPB->m_pX9dctUV[iblk-4] + ((1 *pPB->m_pWMVDec->m_uintNumMBX + iX) << 4);
                    pIntra =  pIntra_lastrow-16*pPB->m_pWMVDec->m_uintNumMBX;//pPB->m_pX9dctUV[iblk-4] + ((0 *pPB->m_pWMVDec->m_uintNumMBX + iX) << 4);
                }
                
            }

            bDCACPredOn = decodeDCTPrediction_EMB (pPMainLoop,
                                                                        pmbmd,pmbmd_lastrow, 
                                                                        pIntra, pIntra_lastrow,
                                                                        iblk, iX, iY, &iShift, 
                                                                        &iDirection, pPredScaled,pPB);            
            
            ppxliErrorQMB = pPB->m_rgiCoefReconBuf;
            if (bDCACPredOn) 
                pPred = pPredScaled;
            result = (tWMVDecodeStatus)(result | DecodeInverseIntraBlockX9_EMB (pPB->m_pWMVDec,
                                        ppDCTTableInfo_Dec,
                                        iblk,
                                        pPB->m_pZigzagScanOrder, bResidual, pPred,
                                        iDirection, iShift, pIntra, pmbmd, pDQ, (I16_WMV*)(&pMbMode->pBackUpErrorInter[iblk][0]))); //get the quantized block
            pPB->m_iDCTHorzFlags= pPB->m_pWMVDec->m_iDCTHorzFlags;
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;   
#if !defined(VOARMV7)&&!defined(NEW_C)
			pMbMode->pBackUpDCTHorzFlagsInter[iblk][0] = pPB->m_iDCTHorzFlags;
#endif
        }
		else
        {
			if (rgCBP2[iblk]) 
            {
				if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) 
                {
					I32_WMV iIndex = Huffman_WMV_get(pPB->m_pHufBlkXformTypeDec, pPB->m_pbitstrmIn);
					iXformType = pPB->m_iBlkXformTypeLUT[iIndex];
					iSubblockPattern = pPB->m_iSubblkPatternLUT[iIndex];
				}
				pmbmd->m_rgcBlockXformMode[iblk] =(I8_WMV)iXformType;
                pMbMode->pBackUpFormType[iblk] = (I8_WMV)iXformType;

				if (iXformType == XFORMMODE_8x8)
                {
					ppxliErrorQMB = pPB->m_rgiCoefReconBuf;      

                    pPB->m_rgiCoefReconBuf = (&pMbMode->pBackUpErrorInter[iblk][0]);

#if defined(VOARMV7)||defined(NEW_C)
                    result = DecodeInverseInterBlockQuantize16_EMB_new(pPB, ppInterDCTTableInfo_Dec,
                        XFORMMODE_8x8, pDQ,
                        (I16_WMV*)(&pMbMode->pBackUpErrorInter[iblk][0]));
					pMbMode->pBackUpDCTRunFlags[iblk][0] = pPB->m_iDctRunFlags;
#else
                    result = DecodeInverseInterBlockQuantize16_EMB(pPB, ppInterDCTTableInfo_Dec,
                        XFORMMODE_8x8, pDQ,
                        (I16_WMV*)(&pMbMode->pBackUpErrorInter[iblk][0]));
					pMbMode->pBackUpDCTHorzFlagsInter[iblk][0] = pPB->m_iDCTHorzFlags;
#endif					
                } 
                else
                {
					I32_WMV i, iLoopTop, iLoopMask, iPattern;
					I32_WMV iIdx;	                
					ppxliErrorQMB = pPB->m_ppxliErrorQ;
					if (iXformType == XFORMMODE_8x4 || iXformType == XFORMMODE_4x8) 
                    {
						if (pmbmd->m_bBlkXformSwitchOn || bFirstBlock)
							iPattern = iSubblockPattern;
						else
                            if (BS_getBit(pPB->m_pbitstrmIn) == 1) 
								iPattern = BS_getBit(pPB->m_pbitstrmIn) + 1;
							else
								iPattern = 3;
						iLoopTop = 2;
						iLoopMask = 2;	                    
					}
					else {
						iPattern = Huffman_WMV_get(pPB->m_pHuf4x4PatternDec, pPB->m_pbitstrmIn) + 1;
						iLoopTop = 4;
						iLoopMask = 8;
					}
	                
					iIdx = iXformType-1;
					rgCBP2[iblk] = (U8_WMV)iPattern;

                    pMbMode->pBackUpLoopMask[iblk] = iLoopMask;
                    pMbMode->pBackUpLoopTop[iblk] = iLoopTop;                    

					//p_DecodeInterError = pPB->m_pDecodeInterError[iIdx];
					for( i=0; i<iLoopTop; i++ ) {
						if ( iPattern & (iLoopMask>>i) ) {                            
                            //UnionBuffer *pDst = pPB->m_ppxliErrorQ;
                            //UnionBuffer  *pSrc = pPB->m_rgiCoefReconBuf;
#if defined(VOARMV7)||defined(NEW_C)
							
                            result = DecodeInverseInterBlockQuantize16_EMB_new(pPB, ppInterDCTTableInfo_Dec, 
                                iXformType, pDQ,(I16_WMV*)(&pMbMode->pBackUpErrorInter[iblk][i]));   
							pMbMode->pBackUpDCTRunFlags[iblk][i] = pPB->m_iDctRunFlags;
#else
                            result = DecodeInverseInterBlockQuantize16_EMB(pPB, ppInterDCTTableInfo_Dec, 
                                iXformType, pDQ,(I16_WMV*)(&pMbMode->pBackUpErrorInter[iblk][i]));  
							pMbMode->pBackUpDCTHorzFlagsInter[iblk][i] = pPB->m_iDCTHorzFlags;
#endif
						}
						else
						{
#if !defined(VOARMV7)&&!defined(NEW_C)
                            pMbMode->pBackUpIDCTClearOffset[iIdx][i] = pPB->m_uiIDCTClearOffset[iIdx][i];
#endif
						}
					}
				}

                /*if(keepidct) 
                    BackUpIDCTCoeff(pPMainLoop,ppxliErrorQMB->i16,iMBX,iMBY,iblk,pPB);*/

                bFirstBlock = FALSE;
			}
			else {
				pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
                pMbMode->pBackUpFormType[iblk] = 5;
			}
		}        
    }
    pmbmd->m_bSkip = bSkipMB;
    
    return result;
}
#if defined(VOARMV7)||defined(NEW_C)
tWMVDecodeStatus DecodeMB_X9_Fast_IdctMC_new (EMB_PMainLoop  * pPMainLoop,
                                                       CWMVMBMode* pmbmd, 
                                                       I32_WMV iMBX, I32_WMV iMBY,
                                                       Bool_WMV b1MV,EMB_PBMainLoop *pPB)
{
    int i,j;
    CDiffMV_EMB * pDiffMV = &pPB->pDiffMV_EMB_Bak[0];
    tWMVDecodeStatus result = WMV_Succeeded;
	//tWMVDecInternalMember *pWMVDec = pPB->m_pWMVDec;
    I32_WMV iblk;
    I32_WMV i1MVCnt = b1MV;   
    //int keepidct = 0;
    CWMVMBModeBack *pMbMode =  pPB->m_pMbMode + iMBX;// + iMBY*pPB->m_uintNumMBX;

    pPB->m_pSSIMD_DstOffSet = pPMainLoop->m_SSIMD_DstLUT_BK;

    //MC
    if( !pDiffMV->iIntra && i1MVCnt == 1) {  // non Intra and 1 MV, do MC 
        U8_WMV *pMCBufferTmp = (U8_WMV*)pPB->prgMotionCompBuffer_EMB;
        result = g_MotionComp_X9_EMB_new (pPMainLoop, pMCBufferTmp, 40 ,iMBY,iMBX,0, 1,pPB);
        if(result)
            return -1;
        i1MVCnt += b1MV;
        pPB->m_pSSIMD_DstOffSet = pPMainLoop->m_SSIMD_DstLUT_1MV;
    }

    for (iblk = 0; iblk < 6; iblk++) 
    {
        U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
        I32_WMV iIntra = 0;
        U8_WMV * pDst;
        I32_WMV iUV;
		//I32_WMV dstoffect=0;

        pDiffMV = &pPB->pDiffMV_EMB_Bak[iblk];

		iUV = iblk >> 2;
        i1MVCnt -= iUV;

		//dstoffect = iMBY*16*pPB->m_iWidthPrev[iUV]+iMBX*16;

        iIntra = pDiffMV->iIntra;
		//pDst = pPB->m_ppxliCurrBlkBase[iblk] + pPB->m_iFrmMBOffset[iUV];
		if(!iUV)
			pDst = pPB->m_ppxliCurrBlkBase[iblk] + iMBY*16*pPB->m_iWidthPrev[iUV]+iMBX*16;//pPB->m_iFrmMBOffset[iUV];
		else
			pDst = pPB->m_ppxliCurrBlkBase[iblk] + iMBY*8*pPB->m_iWidthPrev[iUV]+iMBX*8;
        
        if (iIntra)
        {
            U8_WMV  * pData = (U8_WMV  *)pPB->prgMotionCompBuffer_EMB;
            UnionBuffer  *pTmpError = &pMbMode->pBackUpErrorInter[iblk][0];//&pmbmd->pBackUpError[iblk];
            for(i=0;i<8;i++)
                for(j=0;j<8;j++)
                    pData[i*40+j] = 0x80;  

            if(1){
                DQuantDecParam *pDQ = &pPB->m_prgDQuantParam [pmbmd->m_iQP ];
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
        	
		        *(I32_WMV *) rgiCoefRecon = (iValue0&0xffff) + (iValue1<<16);
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
        	        
			        *(I32_WMV *) (rgiCoefRecon + i) = (iValue0&0xffff) + (iValue1<<16);
		        }
            }

            if(pPB->m_iOverlap)
            {
                I32_WMV  iWidthOverlap = (I32_WMV) pPB->m_iWidthY>>iUV;
                I16_WMV *pDstOvlap = (I16_WMV*) (pPB->m_pIntraBlockRow[iblk] + ((((iMBX << 4) + ((iblk & 1) << 3))>>iUV)&~0x7));
                U8_WMV*   pRef   = (U8_WMV*)pPB->prgMotionCompBuffer_EMB;
                //signed short blk[64];
                //memcpy(blk,pTmpError->i16,64*sizeof(signed short));

                voVC1InvTrans_8x8_Overlap(pDst,pPB->m_iWidthPrev[iUV],pRef,pDstOvlap,40,pTmpError->i16,iWidthOverlap,DCT_COEF1);
            }
            else
            {
				voVC1InvTrans_8x8(pDst,pPB->m_iWidthPrev[iUV],pData,NULL,40,pTmpError->i16,DCT_COEF1,DCT_COEF2);
            }  
        }
		else
        {     
            
            I16_WMV* idcterror = NULL;
            {             
                I32_WMV iXformType= pMbMode->pBackUpFormType[iblk];
                if(iXformType == 5) 
                {//1246  
                    if((i1MVCnt < 2))
                    {
                        //U8_WMV *pMCBufferTmp = (U8_WMV*)pPB->prgMotionCompBuffer_EMB;
                        result = g_MotionComp_X9_EMB_new (pPMainLoop, pDst, pPB->m_iWidthPrev[iUV] ,iMBY,iMBX,iblk, 0,pPB);                     
                        if(result)
                            return -1;
                    }
                    else
                    {
                        //int x,y;  
                        U8_WMV* pRef = (U8_WMV  *)(pPB->prgMotionCompBuffer_EMB + pPB->m_pSSIMD_DstOffSet[iblk]);
                        U8_WMV* pDstRow = pDst;

                         voVC1Copy_8x8(pDstRow, pPB->m_iWidthPrev[iUV], pRef,NULL,40);
                    }
                }
                else if (iXformType == XFORMMODE_8x8) 
                {
                    UnionBuffer  *pTmpError = &pMbMode->pBackUpErrorInter[iblk][0];
                    if((i1MVCnt < 2))
                    {
                        U8_WMV *pMCBufferTmp = (U8_WMV*)pPB->prgMotionCompBuffer_EMB;
                        result = g_MotionComp_X9_EMB_new (pPMainLoop, pMCBufferTmp, 40 ,iMBY,iMBX,iblk, 0,pPB);
                        if(result)
                            return -1;
                    }                    

			        if (rgCBP2[iblk])			
                    {
#ifdef VOIDC_DC
						if(pMbMode->pBackUpDCTRunFlags[iblk][0] == 0)
							voVC1InvTrans_8x8_DC(pDst,
											pPB->m_iWidthPrev[iUV], 
											(U8_WMV*)(pPB->prgMotionCompBuffer_EMB + pPB->m_pSSIMD_DstOffSet[iblk]),
											NULL,
											40,
											pTmpError->i16,
											DCT_COEF1,DCT_COEF2);
						else
#endif
							voVC1InvTrans_8x8(pDst,
											pPB->m_iWidthPrev[iUV], 
											(U8_WMV*)(pPB->prgMotionCompBuffer_EMB + pPB->m_pSSIMD_DstOffSet[iblk]),
											NULL,
											40,
											pTmpError->i16,
											DCT_COEF1,DCT_COEF2);
                    }
                }
                else
                {
                    I32_WMV i=0;
                    I32_WMV iLoopMask = pMbMode->pBackUpLoopMask[iblk];
                    //I32_WMV iLoopTop = pMbMode->pBackUpLoopTop[iblk];
                    I8_WMV iPattern = rgCBP2[iblk];
                    UnionBuffer  *pTmpError = &pMbMode->pBackUpErrorInter[iblk][0];  
                    idcterror = pTmpError->i16;

                    if(iXformType == XFORMMODE_4x4)
                    {  
                        if((i1MVCnt < 2))
                        {
                            U8_WMV *pMCBufferTmp = (U8_WMV*)pPB->prgMotionCompBuffer_EMB;
                            result = g_MotionComp_X9_EMB_new (pPMainLoop, pMCBufferTmp, 40 ,iMBY,iMBX,iblk, 0,pPB);
                            if(result)
                                return -1;
                        }

                        for( i=0; i<4; i++ ) 
                        {
                             I16_WMV* pIn0 = pMbMode->pBackUpErrorInter[iblk][i].i16;
                             U8_WMV* pRef = (U8_WMV*)(pPB->prgMotionCompBuffer_EMB + pPB->m_pSSIMD_DstOffSet[iblk]);
                             //pDst = pPB->m_ppxliCurrBlkBase[iblk] + pPB->m_iFrmMBOffset[iUV];
							 if(!iUV)
								 pDst = pPB->m_ppxliCurrBlkBase[iblk] + iMBY*16*pPB->m_iWidthPrev[iUV]+iMBX*16;
							 else
								 pDst = pPB->m_ppxliCurrBlkBase[iblk] + iMBY*8*pPB->m_iWidthPrev[iUV]+iMBX*8;

                             pRef +=(i>>1)*40*4+(i&1)*4;
                             pDst +=(i>>1)*pPB->m_iWidthPrev[iUV]*4+(i&1)*4;
                             if ( iPattern & (iLoopMask>>i) )
                             {
#ifdef VOIDC_DC
								 if(pMbMode->pBackUpDCTRunFlags[iblk][i] == 0)
									 voVC1InvTrans_4x4_DC(pDst,pPB->m_iWidthPrev[iUV],pRef,NULL,40,pIn0,DCT_COEF1,DCT_COEF2);
								 else
#endif
									 voVC1InvTrans_4x4(pDst,pPB->m_iWidthPrev[iUV],pRef,NULL,40,pIn0,DCT_COEF1,DCT_COEF2);
                             }
                             else
                             {                                
                                 voVC1Copy_4x4(pDst,pPB->m_iWidthPrev[iUV],pRef,NULL,40);
                             }
                        }
                    }
                    else if(iXformType == XFORMMODE_4x8)
                    {
                        if((i1MVCnt < 2))
                        {
                            U8_WMV *pMCBufferTmp = (U8_WMV*)pPB->prgMotionCompBuffer_EMB;
                            result = g_MotionComp_X9_EMB_new (pPMainLoop, pMCBufferTmp, 40 ,iMBY,iMBX,iblk, 0,pPB);
                            if(result)
                                return -1;
                        }
                        for( i=0; i< 2; i++ ) 
                        {
                            I16_WMV* pIn = pMbMode->pBackUpErrorInter[iblk][i].i16;
                            U8_WMV* pRef = (U8_WMV*)(pPB->prgMotionCompBuffer_EMB + pPB->m_pSSIMD_DstOffSet[iblk]);
                            if(!iUV)
								 pDst = pPB->m_ppxliCurrBlkBase[iblk] + iMBY*16*pPB->m_iWidthPrev[iUV]+iMBX*16;
							 else
								 pDst = pPB->m_ppxliCurrBlkBase[iblk] + iMBY*8*pPB->m_iWidthPrev[iUV]+iMBX*8;

                            pRef += (i&1)*4;
                            pDst += (i&1)*4;
                            if ( iPattern & (iLoopMask>>i) )
                            {
#ifdef VOIDC_DC
								if(pMbMode->pBackUpDCTRunFlags[iblk][i] == 0)
									voVC1InvTrans_4x8_DC(pDst,pPB->m_iWidthPrev[iUV], pRef,NULL,40,pIn,DCT_COEF1,DCT_COEF2);
								else
#endif
									voVC1InvTrans_4x8(pDst,pPB->m_iWidthPrev[iUV], pRef,NULL,40,pIn,DCT_COEF1,DCT_COEF2);
                            }
                            else
                            {
                                voVC1Copy_4x8(pDst,pPB->m_iWidthPrev[iUV],pRef,NULL,40);
                            }
                        }  
                    }
                    else if(iXformType == XFORMMODE_8x4)
                    {
                        if((i1MVCnt < 2))
                        {
                            U8_WMV *pMCBufferTmp = (U8_WMV*)pPB->prgMotionCompBuffer_EMB;
                            result = g_MotionComp_X9_EMB_new (pPMainLoop, pMCBufferTmp, 40 ,iMBY,iMBX,iblk, 0,pPB);
                            if(result)
                                return -1;
                        }
                        for( i=0; i< 2; i++ ) 
                        {
                             I16_WMV* pIn = pMbMode->pBackUpErrorInter[iblk][i].i16;
                             U8_WMV* pRef = (U8_WMV*)(pPB->prgMotionCompBuffer_EMB + pPB->m_pSSIMD_DstOffSet[iblk]);
							 if(!iUV)
								 pDst = pPB->m_ppxliCurrBlkBase[iblk] + iMBY*16*pPB->m_iWidthPrev[iUV]+iMBX*16;
							 else
								 pDst = pPB->m_ppxliCurrBlkBase[iblk] + iMBY*8*pPB->m_iWidthPrev[iUV]+iMBX*8;

                             pRef += (i&1)*40*4;
                             pDst += (i&1)*pPB->m_iWidthPrev[iUV]*4;

                             if ( iPattern & (iLoopMask>>i) ) 
                             {
#ifdef VOIDC_DC
								 if(pMbMode->pBackUpDCTRunFlags[iblk][i] == 0)
									 voVC1InvTrans_8x4_DC(pDst,pPB->m_iWidthPrev[iUV],pRef,NULL,40,pIn,DCT_COEF1,DCT_COEF2);
								 else
#endif
									 voVC1InvTrans_8x4(pDst,pPB->m_iWidthPrev[iUV],pRef,NULL,40,pIn,DCT_COEF1,DCT_COEF2);
                             }
                             else
                             {
                                 voVC1Copy_8x4(pDst,pPB->m_iWidthPrev[iUV],pRef,NULL,40);
                             }                             
                        }
                    }
                }
            }
		}        
    } 

    return result;
}



#else
tWMVDecodeStatus DecodeMB_X9_Fast_IdctMC (EMB_PMainLoop  * pPMainLoop,
                                                       CWMVMBMode* pmbmd, 
                                                       I32_WMV iMBX, I32_WMV iMBY,
                                                       Bool_WMV b1MV,EMB_PBMainLoop *pPB)
{
    CDiffMV_EMB * pDiffMV = &pPB->pDiffMV_EMB_Bak[0];
    tWMVDecodeStatus result = WMV_Succeeded;
    I32_WMV iblk;
    I32_WMV i1MVCnt = b1MV;   
    //int keepidct = 0;
    CWMVMBModeBack *pMbMode =  pPB->m_pMbMode + iMBX;// + iMBY*pPB->m_uintNumMBX;

    pPB->m_pSSIMD_DstOffSet = pPMainLoop->m_SSIMD_DstLUT_BK;

    //MC
    if( !pDiffMV->iIntra && i1MVCnt == 1) {  // non Intra and 1 MV, do MC 
        result = g_MotionComp_X9_EMB (pPMainLoop, iMBY,iMBX, 0, 1,pPB);
        if(result)
            return -1;
        i1MVCnt += b1MV;
        pPB->m_pSSIMD_DstOffSet = pPMainLoop->m_SSIMD_DstLUT_1MV;
    }

    for (iblk = 0; iblk < 6; iblk++) {
        U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
        I32_WMV iIntra = 0;
        U8_WMV * pDst;
        I32_WMV iUV;   

    if(iMBX == 3 && iMBY == 3 && iblk == 3)
            iblk =iblk;

        pDiffMV = &pPB->pDiffMV_EMB_Bak[iblk];

		iUV = iblk >> 2;
        i1MVCnt -= iUV;

        iIntra = pDiffMV->iIntra;
		if(!iUV)
			pDst = pPB->m_ppxliCurrBlkBase[iblk] + iMBY*16*pPB->m_iWidthPrev[iUV]+iMBX*16;
		else
			pDst = pPB->m_ppxliCurrBlkBase[iblk] + iMBY*8*pPB->m_iWidthPrev[iUV]+iMBX*8;
        
        if (iIntra)
        {
            UnionBuffer  *pTmpError = &pMbMode->pBackUpErrorInter[iblk][0];//&pmbmd->pBackUpError[iblk];


            if(1){
                DQuantDecParam *pDQ = &pPB->m_prgDQuantParam [pmbmd->m_iQP ];
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

            g_IDCTDec16_WMV3_Fun( pTmpError, pTmpError, BLOCK_SIZE, pMbMode->pBackUpDCTHorzFlagsInter[iblk][0] );

            /*Should deal with the same situation here when in SSIMDxxx */
            {
                U32_WMV  * pData32 = pPB->prgMotionCompBuffer_EMB;
                const U32_WMV uiData = 0x00800080;
                pData32[0]  = pData32[1]   = pData32[5]  = pData32[6]   = uiData;
                pData32[10] = pData32[11]  = pData32[15] = pData32[16]  = uiData;
                pData32[20] = pData32[21]  = pData32[25] = pData32[26]  = uiData;
                pData32[30] = pData32[31]  = pData32[35] = pData32[36]  = uiData;
                pData32[40] = pData32[41]  = pData32[45] = pData32[46]  = uiData;
                pData32[50] = pData32[51]  = pData32[55] = pData32[56]  = uiData;
                pData32[60] = pData32[61]  = pData32[65] = pData32[66]  = uiData;
                pData32[70] = pData32[71]  = pData32[75] = pData32[76]  = uiData;
            }

            pPB->m_pAddError(pDst, 
                                pPB->prgMotionCompBuffer_EMB + pPB->m_pSSIMD_DstOffSet[iblk], 
                                pTmpError->i32,
                                pPB->m_iWidthPrev[iUV]);

            if (pPB->m_iOverlap)
            {
                I32_WMV  iWidth1 = (I32_WMV) pPB->m_iWidthY>>iUV;
                U16_WMV * pDst1 = (U16_WMV*) (pPB->m_pIntraBlockRow[iblk] + ((((iMBX << 4) + ((iblk & 1) << 3))>>iUV)&~0x7));
                U16_WMV * pSrc = (U16_WMV*)pTmpError->i16;

#if  defined (WMV_OPT_COMMON_ARM)
                memcpyOddEven_ARMV4 (pDst1, pSrc, iWidth1);
#else
                I32_WMV i1;
                for ( i1 = 0; i1 < 8; i1++) {
                    pDst1[0] = pSrc[0]; pDst1[2] = pSrc[1];
                    pDst1[4] = pSrc[2]; pDst1[6] = pSrc[3];
                    pDst1[1] = pSrc[32]; pDst1[3] = pSrc[33];
                    pDst1[5] = pSrc[34]; pDst1[7] = pSrc[35];
                    pSrc += 4;
                    pDst1 += iWidth1;
                }
#endif   
            }                        
        }
		else
        {            
            I16_WMV* idcterror = NULL;
            //if(keepidct)
            //{
            //    VODCTCOEFF *coeffbuf = (pPB->m_DctCoeff_Bak + iMBX);
            //    I16_WMV *dcterror = coeffbuf->coeff;
            //    idcterror = dcterror+iblk*64;
            //}
            //else
            {             
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
                    UnionBuffer  *pTmpError = &pMbMode->pBackUpErrorInter[iblk][0];//&pMbMode->pBackUpError[iblk];   

                    for( i=0; i<iLoopTop; i++ ) 
                    {                       
					    if ( iPattern & (iLoopMask>>i) ) 
                        {                           
                            //UnionBuffer *pDst = pPB->m_ppxliErrorQ;
                            UnionBuffer *pSrc = &pMbMode->pBackUpErrorInter[iblk][i];
                            result = (*pPB->m_pDecodeIdct[iIdx])(pTmpError,pSrc,BLOCK_SIZE,pMbMode->pBackUpDCTHorzFlagsInter[iblk][i] ,i);
					    }
					    else
					    {
						    pPB->m_pSubBlkIDCTClear(pTmpError, pMbMode->pBackUpIDCTClearOffset[iIdx][i]);
					    }
				    }
                    idcterror = pTmpError->i16;// pPB->m_ppxliErrorQ->i16;
                }
            }

            if((i1MVCnt < 2))
            {
                result = g_MotionComp_X9_EMB(pPMainLoop,  iMBY,iMBX,iblk, 0,pPB);
                if(result)
                    return -1;
            }

			if (rgCBP2[iblk] && idcterror)			
				pPB->m_pAddError(pDst, 
                                        pPB->prgMotionCompBuffer_EMB + pPB->m_pSSIMD_DstOffSet[iblk], 
                                        (I32_WMV*)idcterror,
                                        pPB->m_iWidthPrev[iUV]);
			else
				pPB->m_pAddNull(pDst, 
                                      pPB->prgMotionCompBuffer_EMB + pPB->m_pSSIMD_DstOffSet[iblk], 
                                      pPB->m_iWidthPrev[iUV]);
		}
        
    } 
    return result;
}
#endif

//static int g_dbg_cnt20, g_dbg_cnt21, g_dbg_cnt22, g_dbg_cnt23;
//#pragma code_seg (EMBSEC_PML)
tWMVDecodeStatus decodeMBOverheadOfPVOP_WMV3_EMB (EMB_PMainLoop * pPMainLoop,  CWMVMBMode* pmbmd, 
                                                  I32_WMV iMBX, I32_WMV iMBY,EMB_PBMainLoop *pPB)
{
    I32_WMV iCBPCY = 0;//,  iPos=0, iTemp=0;
    Bool_WMV bSendTableswitchMode = FALSE_WMV;
    Bool_WMV bSendXformswitchMode = FALSE_WMV;

    CDiffMV_EMB *pDiffMV = pPB->m_pDiffMV_EMB;

#ifdef STABILITY
    if(pPB->m_pDiffMV_EMB == NULL)
        return -1;
#endif
    
    pmbmd->m_iQP = pPB->m_iQP; // starting QP - maybe changed by DQUANT
    pmbmd->m_dctMd = INTER;
    pmbmd->m_iDCTTable_MB_Index = 0;
    pmbmd->m_bBlkXformSwitchOn = FALSE_WMV;
    
    if (pPB->m_iMVSwitchCodingMode == SKIP_RAW) 
        pmbmd->m_chMBMode = (BS_getBit(pPB->m_pbitstrmIn) ? MB_4MV : MB_1MV);
    
    if (pPB->m_iSkipbitCodingMode == SKIP_RAW)
        pmbmd->m_bSkip = BS_getBit(pPB->m_pbitstrmIn);

    {
        int iblk=0,Blocks=0;
        if( pmbmd->m_chMBMode == MB_1MV)
            Blocks = 1;
        else
            Blocks = 4;
        for(iblk=0;iblk<Blocks;iblk++)
        {
            I32_WMV iY = (iMBY <<1) + (iblk>>1);
            pPB->m_pWMVDec->xblock[iblk] = (iMBX<<1) + (iblk&1);
            pPB->m_pWMVDec->yblock[iblk] = (iMBY <<1) + (iblk>>1);
            pPB->m_pWMVDec->bTopBndry[iblk] = 0;            
            if (!(iY & 1))
                pPB->m_pWMVDec->bTopBndry[iblk] = ((iY == 0) || pPB->m_pWMVDec->m_pbStartOfSliceRow [iY >> 1]);
        } 
    }
////////////////////////////////////////////////////////////////

    
    if (pmbmd->m_bSkip) {
        memset(pmbmd->m_rgbCodedBlockPattern2, 0, 6*sizeof(U8_WMV));
        // handle hybrid MV (needed for skipbit too)
        if (pmbmd->m_chMBMode == MB_1MV)
        {
            memset (pDiffMV, 0, sizeof(CDiffMV_EMB));
            if ((*pPMainLoop->m_ppPredictHybridMV_EMB[0]) (pPMainLoop, pPB->m_iBlkMBBase[X_INDEX] + 0,  TRUE, 0,pPB))
                pDiffMV->iHybrid = 1 + (I8_WMV) BS_getBit(pPB->m_pbitstrmIn);
            else
                pDiffMV->iHybrid = 0;            
        }
        else 
        {
            I32_WMV iblk;
			I16_WMV *iBlkMBBase = pPB->m_iBlkMBBase;
			CInputBitStream_WMV* pbitstrmIn = pPB->m_pbitstrmIn;
            memset (pDiffMV, 0, sizeof(CDiffMV_EMB) << 2);
            for ( iblk = 0; iblk < 4; iblk++)
            {
                if ((*pPMainLoop->m_ppPredictHybridMV_EMB[iblk>>1]) (pPMainLoop, iBlkMBBase[X_INDEX] + (iblk & 1),  FALSE, iblk,pPB))
                    (pDiffMV + iblk)->iHybrid = 1 + (I8_WMV) BS_getBit(pbitstrmIn);
                else
                    (pDiffMV + iblk)->iHybrid = 0;
                // hybrid MV - need to compute actual motion vectors too!
                ComputeMVFromDiffMV_EMB (pPMainLoop, iblk,0,pPB);
            }
        }
        goto OneMVEnd;
    }
    
    if (pmbmd->m_chMBMode == MB_1MV)
    {
        // 1MV - read motion vector
        decodeMV_V9_EMB(pPB, pPB->m_pHufMVTable_V9, pDiffMV);  
        // hybrid MV
        pDiffMV->iHybrid = 0;
        if ( (*pPMainLoop->m_ppPredictHybridMV_EMB[0]) (pPMainLoop, pPB->m_iBlkMBBase[X_INDEX] + 0,  TRUE, 0,pPB)
            && !pDiffMV->iIntra)
            pDiffMV->iHybrid = 1 + (I8_WMV) BS_getBit(pPB->m_pbitstrmIn);

        // skip to end if last = 0
        if (pDiffMV->iLast == 0)
        {
            pmbmd->m_bCBPAllZero = TRUE_WMV;
            if (pPB->m_bDQuantOn && pDiffMV->iIntra)
            {
                if (WMV_Succeeded != decodeDQuantParam (pPB->m_pWMVDec, pmbmd)) 
                    return WMV_CorruptedBits; 
            }
            // One MV Intra AC Prediction Flag
            if (pDiffMV->iIntra)
                pmbmd->m_rgbDCTCoefPredPattern2[ 0 ] = (U8_WMV)BS_getBit (pPB->m_pbitstrmIn);              
            
            goto End;
        }
        bSendTableswitchMode = pPB->m_bDCTTable_MB;
        bSendXformswitchMode = pPB->m_bMBXformSwitching && !pDiffMV->iIntra;
        
        // One MV Intra AC Prediction Flag
        if (pDiffMV->iIntra)
            pmbmd->m_rgbDCTCoefPredPattern2[0]  =  (U8_WMV)BS_getBit (pPB->m_pbitstrmIn);   
    }
    
    // read CBP
    iCBPCY = Huffman_WMV_get (pPB->m_pHufNewPCBPCYDec, pPB->m_pbitstrmIn);    
    if (pPB->m_bDQuantOn && pmbmd->m_chMBMode == MB_1MV) {
        if (WMV_Succeeded != decodeDQuantParam (pPB->m_pWMVDec, pmbmd)) {
            return WMV_CorruptedBits;
        }
    }
    
    pmbmd->m_bCBPAllZero = FALSE_WMV;    
    if (pmbmd->m_chMBMode == MB_4MV) 
    {        
        // 4MV - read motion vectors
        I32_WMV iCBPCY_V8 = iCBPCY & 0x30;
        I32_WMV iIntraYUV = 0;
        I32_WMV iIntraChroma = 0;
        I32_WMV iIntraPred = 0;
        I32_WMV iblk;
        Bool_WMV bDecodeDQ = FALSE;
        I16_WMV *iBlkMBBase = pPB->m_iBlkMBBase;
		CInputBitStream_WMV* pbitstrmIn = pPB->m_pbitstrmIn;
        for ( iblk = 0; iblk < 4; iblk++) 
        {
            I32_WMV iIntra;            
            if (iCBPCY & (1 << iblk)) 
                decodeMV_V9_EMB(pPB, pPB->m_pHufMVTable_V9, (pDiffMV + iblk));    
            else
                memset (pDiffMV + iblk, 0, sizeof (CDiffMV_EMB));
            // hybrid MV
            (pDiffMV + iblk)->iHybrid = 0;
            if ((*pPMainLoop->m_ppPredictHybridMV_EMB[(iblk>>1)]) (pPMainLoop, iBlkMBBase[X_INDEX] + (iblk & 1),  FALSE, iblk,pPB)
                && !((pDiffMV + iblk)->iIntra))
                (pDiffMV + iblk)->iHybrid = 1 + (I8_WMV) BS_getBit(pbitstrmIn);

            // hybrid MV - need to compute actual motion vectors too!
            iIntra = ComputeMVFromDiffMV_EMB (pPMainLoop, iblk,0,pPB);
            if (iIntra)
                iIntraPred |= IsIntraPredY_EMB(pPB->m_pWMVDec,iblk, 
                                                        pPB->m_pWMVDec->xblock[iblk],
                                                        pPB->m_pWMVDec->yblock[iblk]);
            
            // enable transform switching if necessary
            iIntraChroma += iIntra;
            iIntraYUV |= (iIntra << iblk);
            iCBPCY_V8 |= ((pDiffMV + iblk)->iLast << iblk);
        }
        iIntraYUV |= (iIntraChroma < 3)? 0 : 0x30;
        bSendTableswitchMode = (pPB->m_bDCTTable_MB && iCBPCY_V8);
        bSendXformswitchMode = (pPB->m_bMBXformSwitching && (~iIntraYUV & iCBPCY_V8));
        bDecodeDQ |= (iCBPCY_V8 != 0) || (iIntraChroma != 0);

        if (iIntraChroma > 2) 
            iIntraPred |= IsIntraPredUV_EMB(pPB->m_pWMVDec, iMBX, iMBY);
        
        iCBPCY = iCBPCY_V8;
        if (pPB->m_bDQuantOn && bDecodeDQ) {
            if (WMV_Succeeded != decodeDQuantParam (pPB->m_pWMVDec, pmbmd)) {
                return WMV_CorruptedBits;
            }
        }
        
        if (iIntraPred) 
            pmbmd->m_rgbDCTCoefPredPattern2[0]  = (U8_WMV)BS_getBit (pPB->m_pbitstrmIn);              
    }
    
    // 0 :  0
    // 1 : 10
    // 2 : 11
    if (bSendTableswitchMode)
    {
        I32_WMV iDCTMBTableIndex = BS_getBit(pPB->m_pbitstrmIn);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit(pPB->m_pbitstrmIn); 
#ifdef STABILITY
        if(iDCTMBTableIndex<0 || iDCTMBTableIndex >2)
            return -1;
#endif
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }
    
End:
    if (bSendXformswitchMode)
    {
        I32_WMV iIndex = Huffman_WMV_get (pPB->m_pHufMBXformTypeDec, pPB->m_pbitstrmIn);
        pmbmd->m_bBlkXformSwitchOn = (iIndex < 8);
        iIndex &= 0x7;
        pmbmd->m_iMBXformMode = pPB->m_iBlkXformTypeLUT[iIndex];
        pmbmd->m_iSubblockPattern = pPB->m_iSubblkPatternLUT[iIndex];
        pmbmd->m_bFirstBlockDecoded = FALSE;
    }

    setCodedBlockPattern2 (pmbmd, Y_BLOCK1, iCBPCY);

OneMVEnd:    
    if (pmbmd->m_chMBMode == MB_1MV)
    {
        CDiffMV_EMB * pInDiffMV = pPB->m_pDiffMV_EMB;
        UMotion_EMB * pMotion;
        I32_WMV iData;
        
        ComputeMVFromDiffMV_EMB (pPMainLoop, 0,TRUE,pPB);

        pMotion = pPB->m_pMotion + pPB->m_rgiCurrBaseBK[0];

        iData = pMotion->I32;
        pMotion++;
        pMotion->I32 = iData;
        pMotion += pPB->m_iNumBlockX;
        pMotion->I32 = iData;
        pMotion--;
        pMotion->I32 = iData;
        
        (pInDiffMV+3)->iIntra = 
        (pInDiffMV+2)->iIntra = 
        (pInDiffMV+1)->iIntra = (pInDiffMV)->iIntra ;

    } 
    
    if (BS_invalid(pPB->m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }
    
    return WMV_Succeeded;
}


//#pragma code_seg (EMBSEC_DEF)
I32_WMV PredictMVPullBack_EMB(EMB_PMainLoop  * pPMainLoop,  I32_WMV b1MV, I32_WMV iblk, I32_WMV iPred,EMB_PBMainLoop *pPB)  //-----PullBackMotionVector
{    
    I32_WMV iX1, iY1;    
    I32_WMV iXPred = (iPred<<16)>>16;
    I32_WMV iYPred = iPred >>16;
    I32_WMV iX, iY;
    I32_WMV iNumBlockX = pPB->m_iNumBlockX;
    I32_WMV iNumBlockY = pPB->m_iNumBlockY;
    I32_WMV iMinCoordinate, iMaxX, iMaxY;
    Bool_WMV bUV = 0;    
    if(b1MV == 4)
    {
        iXPred <<= 1;
        iYPred <<= 1;
        bUV = 1;
    }    
    if(b1MV == 2)
    {
        bUV = 1;
    }
    
    iX = pPB->m_iBlkMBBase[X_INDEX] + (iblk&1);
    iY = pPB->m_iBlkMBBase[Y_INDEX] + ((iblk&2)>>1);
    
    iX1 = (iX << 5) + iXPred;
    iY1 = (iY << 5 ) + iYPred;
    iMinCoordinate = (b1MV == 1) ? ((-60 -bUV*4)<<bUV) : ((-28 -bUV*4)<<bUV);  // -15 or -7 pixels
    iMaxX = (iNumBlockX << 5) - 4 + bUV*4;  // x8 for block x4 for qpel
    iMaxY = (iNumBlockY << 5) - 4 + bUV*4;

    iX1 &= ~(bUV*((b1MV<<1)-1));
    iY1 &= ~(bUV*((b1MV<<1)-1));

    
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
    
    if(b1MV == 4)
    {
        iXPred >>= 1;
        iYPred >>= 1;
    }
    
    iPred = (iXPred&0xffff) |  (iYPred <<16);
    
    return iPred;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV PredictMVPullBackCheck_EMB(EMB_PMainLoop  * pPMainLoop,  I32_WMV b1MV, I32_WMV iblk, I32_WMV iPred,EMB_PBMainLoop *pPB)
{
    I32_WMV iShift = 5 - (b1MV>>2);
    //I32_WMV iCord = (*(I32_WMV *)(pPB->m_iBlkMBBase))+ (iblk&1) + ((iblk&2)<<15);
    I32_WMV iCord = (I32_WMV)(pPB->m_iBlkMBBase[0] | (pPB->m_iBlkMBBase[1]<<16))+ (iblk&1) + ((iblk&2)<<15);
    I32_WMV iV1 = (iCord <<iShift) + (pPMainLoop->m_PredictMVLUT.U.m_PullBack[b1MV].m_uiMin) + iPred;
    I32_WMV iV2 = pPMainLoop->m_PredictMVLUT.U.m_PullBack[b1MV].m_uiMax  - (iCord <<iShift) -  iPred;
    I32_WMV iSign = (iPred &0x8000)<<1;
   
    {
        iV1 -= iSign;
        
        if((iV1|iV2)&0x80008000)
        {
            iPred = PredictMVPullBack_EMB(pPMainLoop, b1MV, iblk, iPred,pPB);
        }
    }
    
    return iPred;    
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV PredictHybridMV_EMB (EMB_PMainLoop  * pPMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk,EMB_PBMainLoop *pPB)
{
    I32_WMV iNumBlockX = pPB->m_iNumBlockX;
    I32_WMV iIndex2 = pPB->m_rgiCurrBaseBK[0] + pPB->m_rgiBlkIdx[iblk];  //iY * iNumBlockX + iX;
    UMotion_EMB * pMotion = pPB->m_pMotion;
    UMotion_EMB iVa, iVb, iVc, iPred, uiPred1, uiRef;
    I32_WMV  iBlocks;
    I32_WMV iIndex = iIndex2 - 1;

    if(pPB->m_pWMVDec->bTopBndry[iblk] & pPB->m_pWMVDec->m_bSliceWMVA)
    {
        if (iX > 0)
            iVc.U32 = pMotion[iIndex].U32;
        else
             iVc.U32  = 0;
         pPB->m_iPred.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, b1MV, iblk, iVc.U32,pPB);   
        if (iVc.I16[0] == IBLOCKMV)
            iVc.U32  = 0;
        return 0;
    }
    
    // find motion predictor
    // left block   
    iVc.U32 = 0;
    if (iX)       
        iVc.U32 = pMotion[iIndex].U32; 
    else
    {  //// special case - 1 MB wide frame, 1MV mode, copy from top
        if(pPB->m_pWMVDec->m_bCodecIsWVC1)
        {
            if (pPB->m_pWMVDec->m_uintNumMBX == 1 && b1MV > 0) {
                iIndex += 1 - iNumBlockX;
                 iVc.U32 = pMotion[iIndex].U32;
                 if (iVc.U32 == IBLOCKMV) 
                     pPB->m_iPred.I32 = 0;
                 return 0;
            }
            iVc.U32 = 0;
        }
    }
    iIndex = iIndex2 - iNumBlockX;     
    // top block
    iVa.U32 = pMotion[iIndex].U32; 
/******/
    {
        I32_WMV iBlockX = pPB->m_pWMVDec->xblock[iblk];
        I32_WMV iBlockY = pPB->m_pWMVDec->yblock[iblk];

        switch (b1MV) {
        case 0: // 4MV
            if (pPB->m_pWMVDec->m_iBetaRTMMismatchIndex) {
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
    }
/******/
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
        pPB->m_iPred.I32 = iPred.U32 = PredictMVPullBackCheck_EMB(pPMainLoop, b1MV, iblk, iPred.I32,pPB);    
        
nonZero2IBlocksRet:    
    
    if (iX == 0)
        return 0;

    {
        I32_WMV dx, dy;
        I32_WMV sum;
        dx = iPred.I16[X_INDEX];
        dy = iPred.I16[Y_INDEX];
        uiRef.I32 = pMotion[iIndex2 - 1].I32;
        if(uiRef.I32 != IBLOCKMV) 
        {
            dx -=uiRef.I16[X_INDEX];
            dy -= uiRef.I16[Y_INDEX];
        }
        
        sum = abs (dx) + abs (dy);
        if (sum > 32)
            return 1;

        uiRef.I32 = pMotion[iIndex2 - iNumBlockX].I32;
        if(uiRef.I32 != IBLOCKMV) 
        {
            dx = iPred.I16[X_INDEX] - uiRef.I16[X_INDEX];
            dy = iPred.I16[Y_INDEX] - uiRef.I16[Y_INDEX];
        }
        else
        {
            dx = iPred.I16[X_INDEX]; 
            dy = iPred.I16[Y_INDEX];
        }
        sum = abs (dx) + abs (dy);
        if (sum > 32)
            return 1;
        return 0;
    }  
nonZeroIBlocks:  
    if (iBlocks == 1) 
    {
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
         pPB->m_iPred.U32 = iPred.U32  = 0;
         goto nonZero2IBlocksRet;
    }
    
    return 0;
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV PredictHybridMVTop_EMB (EMB_PMainLoop  * pPMainLoop, I32_WMV iX,  Bool_WMV b1MV,  I32_WMV iblk,EMB_PBMainLoop *pPB)
{    
    UMotion_EMB * pMotion = pPB->m_pMotion;
    UMotion_EMB  iPred;
    {    
        iPred.U32 = 0;        
        if (iX != 0)
        {
            I32_WMV iIndex = pPB->m_rgiCurrBaseBK[0] + pPB->m_rgiBlkIdx[iblk] -1;  //iY * iNumBlockX + iX;
            I32_WMV iTmp = pMotion[iIndex].I32;            
            if(iTmp != IBLOCKMV)
            {
                iPred.U32 = PredictMVPullBackCheck_EMB(pPMainLoop, b1MV, iblk, iTmp,pPB);
            }
        }        
        pPB->m_iPred.I32 = iPred.I32;
    }
    
    return 0;
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV ComputeMVFromDiffMV_EMB (EMB_PMainLoop *pPMainLoop, I32_WMV  iblk,I8_WMV b1MV,EMB_PBMainLoop *pPB)
{
    I32_WMV  k = pPB->m_rgiCurrBaseBK[0] +pPB->m_rgiBlkIdx[iblk];
    I32_WMV  dX, dY, iIBlock = 0;
    CDiffMV_EMB * pInDiffMV = pPB->m_pDiffMV_EMB + iblk;
    
    // 1MV non-anchor blocks get assigned anchor block MV
    // find prediction, and add
    
    if (pInDiffMV->iIntra) {
        iIBlock = 1;
        pInDiffMV->Diff.I32 = pPB->m_pMotion[k].I32 = IBLOCKMV;
    }
    else  {
        if (pInDiffMV->iHybrid == 0) 
        {
            if(pPB->m_pWMVDec->bTopBndry[iblk]  &&  pPB->m_pWMVDec->m_bSliceWMVA)
            {
                if(pPB->m_pWMVDec->xblock[iblk])
                {                 
                    dX = pPB->m_pMotion[k-1].I16[X_INDEX];
                    dY = pPB->m_pMotion[k-1].I16[Y_INDEX];
                     if (dX == IBLOCKMV) {
                         dX= dY = 0;
                     }
                }
                else
                    dX= dY = 0;
            }
            else
            {
                if(pPB->m_pWMVDec->bTopBndry[iblk])
                {
                    if(pPB->m_pWMVDec->xblock[iblk])
                    {
                        dX = pPB->m_iPred.I16[X_INDEX];
                        dY = pPB->m_iPred.I16[Y_INDEX];
                    }
                    else
                        dX= dY = 0;
                }
                else
                {
                    if(pPB->m_iBlkMBBase[X_INDEX] + (iblk & 1))
                    {
                        dX = pPB->m_iPred.I16[X_INDEX];
                        dY = pPB->m_iPred.I16[Y_INDEX];
                    }
                    else
                    {
                        if (pPB->m_pWMVDec->m_uintNumMBX == 1 && b1MV>0) {

                            int k2 = k-1;
                            k2 += 1- (pPB->m_pWMVDec->m_uintNumMBX<<1);
                            dX = pPB->m_pMotion[k2].I16[X_INDEX];
                            dY = pPB->m_pMotion[k2].I16[Y_INDEX];
                             if (dX == IBLOCKMV)
                                 dX = dY = 0;
                        }
                        else
                        {
                            dX = pPB->m_iPred.I16[X_INDEX];
                            dY = pPB->m_iPred.I16[Y_INDEX];
                        }
                    }
                }               
            }

            {
                I32_WMV  iX1 = (pPB->m_pWMVDec->xblock[iblk]) << 5;  // original qpel coordinate
                I32_WMV  iY1 = (pPB->m_pWMVDec->yblock[iblk]) << 5;
                I32_WMV iMinCoordinate;
                I32_WMV iMaxX ;
                I32_WMV iMaxY ;

                iX1 += dX;
                iY1 += dY;
                iMinCoordinate = (b1MV == 1) ? -60 : -28;  // -15 or -7 pixels
                iMaxX = (pPB->m_iNumBlockX << 5) - 4;  // x8 for block x4 for qpel
                iMaxY = (pPB->m_iNumBlockY << 5) - 4;
                
                if (iX1 < iMinCoordinate)
                    dX += iMinCoordinate - iX1;
                else if (iX1 > iMaxX)
                    dX += iMaxX - iX1;
                
                if (iY1 < iMinCoordinate)
                    dY += iMinCoordinate - iY1;
                else if (iY1 > iMaxY)
                    dY += iMaxY - iY1;
            }

        }
        else {
            I32_WMV iIdxStep = pPB->m_rgiBlkIdx[pInDiffMV->iHybrid ];
            UMotion_EMB * pMotion = pPB->m_pMotion + k - iIdxStep ;
            dX = pMotion->I16[X_INDEX];
            dY = pMotion->I16[Y_INDEX];      
            if (dX == IBLOCKMV) {
                dX = dY = 0;
            }
        }
        
        {
            I32_WMV iShift = pPB->m_bHalfPelMV;
            I32_WMV iRange = pPB->m_iXMVRange;
            I32_WMV iFlag = pPB->m_iXMVFlag;

            pInDiffMV->Diff.I16[X_INDEX] = (((pInDiffMV->Diff.I16[X_INDEX]<<iShift) + dX + iRange) & iFlag) - iRange;

            iRange = pPB->m_iYMVRange;
            iFlag = pPB->m_iYMVFlag;

            pInDiffMV->Diff.I16[Y_INDEX] = (((pInDiffMV->Diff.I16[Y_INDEX]<<iShift) + dY + iRange) & iFlag) - iRange;

            pPB->m_pMotion[k].I32 = pInDiffMV->Diff.I32;
        }
    }
    
    return iIBlock;
}

//static int g_dbg_cnt, g_dbg_cnt1, g_dbg_cnt2, g_dbg_cnt3, g_dbg_cnt4, g_dbg_cnt5, g_dbg_cnt6;

//#pragma code_seg (EMBSEC_PML2)
I32_WMV median4less(EMB_PMainLoop *pPMainLoop,EMB_PBMainLoop *pPB)
{
    I32_WMV i,j;
    CDiffMV_EMB * pInDiffMV = pPB->m_pDiffMV_EMB;
    UMotion_EMB uiV, iParams[4];

    i = 0;
	j = 0;
    do
    {
        I32_WMV data =  (pInDiffMV+i)->Diff.I32;
        if(data != IBLOCKMV)
        {
            iParams[j++].I32 = data;
        }
    } while ( ++i < 4 );
    
    
    if(j ==3)
    {
        uiV.I16[X_INDEX] = medianof3(iParams[0].I16[X_INDEX], iParams[1].I16[X_INDEX], iParams[2].I16[X_INDEX]);
        uiV.I16[Y_INDEX] = medianof3(iParams[0].I16[Y_INDEX], iParams[1].I16[Y_INDEX], iParams[2].I16[Y_INDEX]);
    }
    else if(j== 2)
    {
        uiV.I16[X_INDEX] = (iParams[0].I16[X_INDEX] + iParams[1].I16[X_INDEX] )/2;
        uiV.I16[Y_INDEX] = (iParams[0].I16[Y_INDEX] + iParams[1].I16[Y_INDEX] )/2;
    }
    else
    {        
        uiV.I32 = IBLOCKMV;
    }    
    

    if(pPB->m_pWMVDec->m_bCodecIsWVC1) //0817
        ;
    else
    {
        i = 0;
        do
        {
            if(!(pInDiffMV+i)->iIntra)
            {            
                (pInDiffMV+i)->Diff.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 2, 0, (pInDiffMV+i)->Diff.I32,pPB);
            }
        }while ( ++i < 4 );
    }

    return uiV.I32;
    
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV ChromaMV_EMB (EMB_PMainLoop *pPMainLoop, I32_WMV iX, I32_WMV iY, Bool_WMV b1MV,EMB_PBMainLoop *pPB)
{
    CDiffMV_EMB * pInDiffMV = pPB->m_pDiffMV_EMB;
    I32_WMV  iIndex;
    UMotion_EMB uiV, uiV2;
    if(pPB->m_pWMVDec->m_tFrmType != PVOP)
        return -1;

    if(b1MV) 
    {
        uiV.I32 =pInDiffMV->Diff.I32;
        if(uiV.I32 != IBLOCKMV)
        {
            if(pPB->m_pWMVDec->m_bCodecIsWVC1) //0815
                ;
            else
                (pInDiffMV)->Diff.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 2, 0, uiV.I32,pPB);
        }
        else
        {
            iIndex = pPB->m_rgiCurrBaseBK[1];
            pPB->m_pMotionC[iIndex].I32 = IBLOCKMV;    
            (pInDiffMV+5)->iIntra = (pInDiffMV+4)->iIntra = 1; 
            if (pPB->m_iNumBFrames>0)
                pPB->m_rgmv1_EMB[iIndex].m_vctTrueHalfPel.U32 = 0;
            return 1;
        }
    }
    else
    {
        if((pInDiffMV->Diff.I32 != IBLOCKMV) && ((pInDiffMV+1)->Diff.I32 != IBLOCKMV) &&
            ((pInDiffMV+2)->Diff.I32 != IBLOCKMV) && ((pInDiffMV+3)->Diff.I32 != IBLOCKMV) )
        {     
            uiV.I16[X_INDEX] = medianof4(pInDiffMV[0].Diff.I16[X_INDEX], pInDiffMV[1].Diff.I16[X_INDEX], pInDiffMV[2].Diff.I16[X_INDEX], pInDiffMV[3].Diff.I16[X_INDEX]);
            uiV.I16[Y_INDEX] = medianof4(pInDiffMV[0].Diff.I16[Y_INDEX], pInDiffMV[1].Diff.I16[Y_INDEX], pInDiffMV[2].Diff.I16[Y_INDEX], pInDiffMV[3].Diff.I16[Y_INDEX]);
            
            pInDiffMV = pPB->m_pDiffMV_EMB;
            if(pPB->m_pWMVDec->m_bCodecIsWVC1)
            {
                ;
            }
            else
            {
                (pInDiffMV+3)->Diff.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 2, 0, (pInDiffMV+3)->Diff.I32,pPB);
                (pInDiffMV+2)->Diff.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 2, 0, (pInDiffMV+2)->Diff.I32,pPB);
                (pInDiffMV+1)->Diff.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 2, 0, (pInDiffMV+1)->Diff.I32,pPB);
                (pInDiffMV+0)->Diff.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 2, 0, (pInDiffMV+0)->Diff.I32,pPB);
            }
        }
        else
        {
            //one of the comp is intra.
            uiV.I32 = median4less(pPMainLoop,pPB);
            if(uiV.I32 == IBLOCKMV) 
            {
                iIndex = pPB->m_rgiCurrBaseBK[1];
                pPB->m_pMotionC[iIndex].I32 = IBLOCKMV;    
                (pInDiffMV+5)->iIntra = (pInDiffMV+4)->iIntra = 1;    
                if (pPB->m_iNumBFrames>0) {
                    pPB->m_rgmv1_EMB[iIndex].m_vctTrueHalfPel.U32 = 0;
                }
                return 1;
            }            
        }        
    }

    iX = uiV.I16[X_INDEX];
    iY = uiV.I16[Y_INDEX];
    iIndex = pPB->m_rgiCurrBaseBK[1];
        
    if (pPB->m_iNumBFrames > 0) {  

        if(pPB->m_pWMVDec->m_bCodecIsWVC1)
            pPB->m_rgmv1_EMB[iIndex].m_vctTrueHalfPel.U32 = uiV.U32;
        else
        {
            uiV2.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 4, 0, uiV.I32,pPB);
            pPB->m_rgmv1_EMB[iIndex].m_vctTrueHalfPel.U32 = uiV2.U32; 
        }
    }

    {
        U8_WMV * pTbl = pPMainLoop->m_PredictMVLUT.U.U.m_RndTbl;
        iX = (iX + pTbl[iX & 3]) >> 1;
        iY = (iY + pTbl[iY & 3]) >> 1;
    }

    if (pPB->m_bUVHpelBilinear) {
        if (iX > 0)
            iX -= iX & 1 ;
        else 
            iX+=  iX & 1;

        if (iY > 0) 
            iY -= iY & 1 ;
        else 
            iY+=  iY & 1;
    }       
        
    uiV2.I16[X_INDEX] = iX;
    uiV2.I16[Y_INDEX] = iY;      

    if(pPB->m_pWMVDec->m_bCodecIsWVC1)//0815
        ;
    else
        uiV2.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 4, 0, uiV2.I32,pPB);   

    (pInDiffMV+5)->Diff.I32 = 
    (pInDiffMV+4)->Diff.I32 =  
    pPB->m_pMotionC[iIndex].I32 = (I32_WMV) uiV2.I32;     

    (pInDiffMV+5)->iIntra = 
    (pInDiffMV+4)->iIntra = 0;

    return 0;
}


static I32_WMV gFineMV[] = {0,2,3,4,5,8};
//static I32_WMV aMVOffsets[] = {0,1,3,7,15,31};


//static int g_dbg_cnt7, g_dbg_cnt8, g_dbg_cnt9, g_dbg_cnt10;

//#pragma code_seg (EMBSEC_PML)
Void_WMV decodeMV_V9_EMB(EMB_PBMainLoop * pMainLoop, Huffman_WMV* pHufMVTable, CDiffMV_EMB *pDiffMV)
{
    I32_WMV iFine=0; 
    I32_WMV len;
    CInputBitStream_WMV *pbitstrmIn = pMainLoop->m_pbitstrmIn;
    I32_WMV iIndex = Huffman_WMV_get(pHufMVTable, pbitstrmIn);
    I32_WMV iTemp;

    
    ++iIndex;
    // last symbol    
    iTemp = iIndex >= 37;    
    pDiffMV->iLast = iTemp;    
    if(iTemp)  iIndex -= 37;    
    pDiffMV->iIntra = 0;

    if(iIndex == 0)
    {
        pDiffMV->Diff.I32 = 0; 
    }
    else if(iIndex < 35)
    {
        I32_WMV i, iMask;
        I32_WMV iSign;
        I32_WMV iData = pMainLoop->m_rgiMVDecLUT_EMB[iIndex] ;
        for(i = 0; i < 2; i++, iData = iData >> 8)
        {
            len = iData&0xf;
            if(len>0)
                iFine = BS_getBits(pbitstrmIn,len);
            else
                iFine = 0;
            iSign = - (iFine & 1);
            iMask = (1<<((iData>>4)&0xf))-1;
            pDiffMV->Diff.I16[i] = (iSign ^ ((iFine >> 1) + iMask)) - iSign;       
        }
    }
    else if(iIndex == 36)
    {
        pDiffMV->iIntra = 1;
        pDiffMV->Diff.I32 = 0;
    }
    else
    {
        I32_WMV iLenX = pMainLoop->m_iLogXRange - pMainLoop->m_bHalfPelMV;
        I32_WMV iLenY = pMainLoop->m_iLogYRange - pMainLoop->m_bHalfPelMV;
        I32_WMV iData = BS_getBits(pbitstrmIn, iLenX + iLenY);
        pDiffMV->Diff.I16[X_INDEX] = (iData>>iLenY)&((1<<iLenX)-1);
        pDiffMV->Diff.I16[Y_INDEX] = (iData)&((1<<iLenY)-1);
    }
}

//static int g_dbg_cnt1, g_dbg_cnt2;
//#pragma code_seg (EMBSEC_PML2)
Bool_WMV decodeDCTPrediction_EMB (EMB_PMainLoop * pPMainLoop,
                                   CWMVMBMode *pmbmd, 
                                   CWMVMBMode *pmbmd_lastrow,
                                   I16_WMV *pIntra, 
                                   I16_WMV *pIntra_lastrow,
                                   I32_WMV iblk,
                                   I32_WMV iX, I32_WMV iY, 
                                   I32_WMV *piShift, I32_WMV *iDirection, 
                                   I16_WMV *pPredScaled,EMB_PBMainLoop *pPB)

{
    
    Bool_WMV bACPredOn = TRUE_WMV;
    I32_WMV iUV = iblk >>2;
    I32_WMV iStride = pPB->m_iNumBlockX >> iUV;
    Bool_WMV   iShift = 0; //= *piShift;
    I16_WMV *pPredIntraLeft, *pPredIntraTop, *pPred;
    Bool_WMV bDCACPred = FALSE_WMV;
    UMotion_EMB *pMotion = pPB->m_pMotion;
    I32_WMV iIndex = pPB->m_rgiCurrBaseBK[iUV] + pPB->m_rgiBlkIdx[iblk];

    pPred = pPredIntraLeft = pPredIntraTop = NULL_WMV;

    if(iUV) pMotion = pPB->m_pMotionC;
    

    if(pPB->m_pWMVDec->m_bSliceWMVA)
    {
        if(!iUV)
        {
             if (((pPB->m_pWMVDec->m_tFrmType == BVOP) && (iY & 1)) ||
                 (iY && ((pPB->m_pWMVDec->m_tFrmType == IVOP || pPB->m_pWMVDec->m_tFrmType == BIVOP) || pMotion[iIndex - iStride].I32 == IBLOCKMV)))  
            {    
                 if (((iY & 1) == 1) || !pPB->m_pWMVDec->m_pbStartOfSliceRow[iY>>1]) {
                    pPred = pPredIntraTop = pIntra_lastrow;//pIntra - 16 * iStride;
                    iShift = pPB->m_iACRowPredShift;
                }
            }
        }
        else
        {
             if (iY && !pPB->m_pWMVDec->m_pbStartOfSliceRow[iY]) {
                 if (pMotion[(iY - 1) * (I32_WMV) pPB->m_pWMVDec->m_uintNumMBX + iX].I32 == IBLOCKMV 
                     || ( pPB->m_pWMVDec->m_tFrmType  == IVOP || pPB->m_pWMVDec->m_tFrmType  == BIVOP)) 
                 {
                     pPred = pPredIntraTop = pIntra_lastrow;//pIntra - 16 * pPB->m_pWMVDec->m_uintNumMBX; 
                    iShift = pPB->m_pWMVDec->m_iACRowPredShift;
                 }
             }
        }
    }
    else
    {
         if ((iY && (pMotion[iIndex - iStride].I32 == IBLOCKMV))) 
        {    
            pPred = pPredIntraTop = pIntra_lastrow;//pIntra - 16 * iStride;
            iShift = pPB->m_iACRowPredShift;
        }
    }
    
     if(iblk > 4) iblk = 4;
  //  if ((iX && (pWMVDec->m_pMotion[iIndex - 1].I16[X_INDEX] == IBLOCKMV))) 
     if ((iX && (pMotion[iIndex - 1].I32 == IBLOCKMV))) 
    {
        pPred = pPredIntraLeft = pIntra - 16;//pWMVDec->m_iACColPredOffset;
        iShift = pPB->m_iACColPredShift;
    }
    if (pPredIntraLeft && pPredIntraTop) {
        I32_WMV iTopLeftDC = 0;
        I32_WMV iTopDC ;
        I32_WMV iLeftDC;
        I32_WMV iDiff0, iDiff1;
        if ((pMotion[(iIndex - iStride - 1)].I32== IBLOCKMV) ) {                
            iTopLeftDC = pPredIntraTop[pPB->m_iACRowPredOffset - 16];
        }
        iTopDC = pPredIntraTop[pPB->m_iACRowPredOffset];
        iLeftDC = pPredIntraLeft[pPB->m_iACColPredOffset];    


        //pmbmd_lastrow =  pmbmd - pPB->m_pWMVDec->m_uintNumMBX;
        ScaleDCPredForDQuant (pPB->m_pWMVDec, iblk, pmbmd,pmbmd_lastrow, &iTopDC, &iLeftDC, &iTopLeftDC);
        
        iDiff0 = iTopLeftDC - iLeftDC;
        iDiff1 = iTopLeftDC - iTopDC;
        if (abs (iDiff0) < abs (iDiff1)) {
            pPred = pPredIntraTop;
            iShift = pPB->m_iACRowPredShift;
        }
    }
    
    if (pPred != NULL_WMV) {

        if(!pmbmd->m_rgbDCTCoefPredPattern2[0])
            bACPredOn = FALSE_WMV;

        bDCACPred = TRUE_WMV;
        if (iShift == pPB->m_iACColPredShift) {
            ScaleLeftPredForDQuant (pPB->m_pWMVDec, iblk, pPred, pPredScaled, pmbmd);
        } else {
            //pmbmd_lastrow =  pmbmd - pWMVDec->m_uintNumMBX;
            ScaleTopPredForDQuant (pPB->m_pWMVDec, iblk, pPred, pPredScaled, pmbmd,pmbmd_lastrow);
        }
    }
    
    if (bACPredOn == FALSE_WMV) 
        iShift = -1;
    
    *iDirection = (pPred == pPredIntraTop);
    *piShift =  iShift ;
    return bDCACPred;
}
 
 //#pragma code_seg (EMBSEC_PML2)
 I32_WMV IsIntraPredUV_EMB (tWMVDecInternalMember *pWMVDec, I32_WMV iMBX, I32_WMV iMBY)
 {
     I32_WMV iPred = 0;
     if (iMBY) {
         if (pWMVDec->m_pMotionC[(iMBY - 1) * (I32_WMV) pWMVDec->m_uintNumMBX + iMBX].I16[X_INDEX] == IBLOCKMV)
             iPred = 1;
     }
     if (iMBX) {
         if (pWMVDec->m_pMotionC[iMBY * (I32_WMV) pWMVDec->m_uintNumMBX + iMBX - 1].I16[X_INDEX] == IBLOCKMV)
             iPred = 1;
     }
     
     return iPred;
 }
 
 //#pragma code_seg (EMBSEC_PML)
 I32_WMV  IsIntraPredY_EMB (tWMVDecInternalMember *pWMVDec, I32_WMV iblk,I32_WMV iX, I32_WMV iY)
 {
     I32_WMV iWidth = pWMVDec->m_uintNumMBX << 1;
     I32_WMV iIndex = iX + (iY * pWMVDec->m_uintNumMBX << 1);
     I32_WMV iPred = 0;

     if(pWMVDec->m_bSliceWMVA)
     {  
         if (!pWMVDec->bTopBndry[iblk]  && (pWMVDec->m_pMotion[(iIndex - iWidth)].I16[X_INDEX] == IBLOCKMV))   
            iPred = 1;
     }
     else
         if (iY  && (pWMVDec->m_pMotion[(iIndex - iWidth)].I16[X_INDEX] == IBLOCKMV))   
            iPred = 1;
     
     if (iX && (pWMVDec->m_pMotion[(iIndex - 1)].I16[X_INDEX] == IBLOCKMV))         
         iPred = 1;
     
     return iPred;
 }
 
 //#pragma code_seg (EMBSEC_DEF)
 Void_WMV SetupMVDecTable_EMB(tWMVDecInternalMember *pWMVDec)
 {
     I32_WMV iTemp;
     //I32_WMV iFine=0; 
     const I32_WMV  iNm1 = 5;
     I32_WMV iHalfpelMV;
     I32_WMV iIndex1, iIndex2;
     I32_WMV len;
     I32_WMV iIndex;
     I32_WMV data, data1;
     
     for(iIndex = 0; iIndex < 36; iIndex++)
     {
         iTemp = pWMVDec->m_iHalfPelMV;
         iIndex1 = prvWMVModulus  ((I32_WMV)(iIndex), (I32_WMV)(iNm1 + 1));
         iHalfpelMV = iTemp && (iIndex1 == iNm1);
         len = gFineMV[iIndex1] - iHalfpelMV;
         if(len <0) len = 0;
         
         data = len | (iIndex1<<4);
         
         iIndex2 = prvWMVDivision (iIndex , (iNm1 + 1));
         iHalfpelMV = iTemp && (iIndex2 == iNm1);
         len = gFineMV[iIndex2] - iHalfpelMV;
         if(len <0) len = 0;
         
         data1 = len | (iIndex2<<4);
         
         pWMVDec->m_rgiMVDecLUT_EMB[iIndex] = data | (data1<<8);
         
         
     }
 }

extern const I16_WMV gaSubsampleBicubic[];
//#pragma code_seg (EMBSEC_DEF)
Void_WMV InitEMBPMainLoopCtl(tWMVDecInternalMember *pWMVDec,EMB_PBMainLoop *pPB)
{
    I32_WMV i,j;
    EMB_PMainLoop * pPMainLoop = &pWMVDec->m_EMB_PMainLoop;
    //EMB_PBMainLoop * pMainLoop = (EMB_PBMainLoop * )pPMainLoop;

    pPB->m_pWMVDec = pWMVDec;
    
    pPB->m_pDiffMV_EMB = pWMVDec->m_pDiffMV_EMB;
    pPB->m_pbitstrmIn = pWMVDec->m_pbitstrmIn;
    
    pPB->m_pMotion = pWMVDec->m_pMotion;
    pPB->m_pMotionC = pWMVDec->m_pMotionC;
    
    pPB->m_iNumBlockX = pWMVDec->m_uintNumMBX<<1;
    pPB->m_iNumBlockY = pWMVDec->m_uintNumMBY<<1;
    pPB->m_rgiBlkIdx[0] = 0;
    pPB->m_rgiBlkIdx[1] = 1;
    pPB->m_rgiBlkIdx[2] = pPB->m_iNumBlockX;
    pPB->m_rgiBlkIdx[3] = pPB->m_iNumBlockX+1;
    
    pPB->m_iWidthY = pPB->m_iNumBlockX<<3;
    pPB->m_iHeightY = pPB->m_iNumBlockY<<3;
    pPB->m_iWidthUV = pPB->m_iNumBlockX<<2;
    pPB->m_iHeightUV = pPB->m_iNumBlockY<<2;
    
    pPB->m_iWidthPrevY = pWMVDec->m_iWidthPrevY;
    pPB->m_iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
    pPB->m_iHeightPrevY = pWMVDec->m_iHeightPrevY;
    pPB->m_iHeightPrevUV = pWMVDec->m_iHeightPrevUV;
    
    pPB->m_iWidthPrev[0] = pPB->m_iWidthPrevY;
    pPB->m_iWidthPrev[1] = pPB->m_iWidthPrevUV;
    
    pPMainLoop->m_rgMotionCompBuffer_EMB = pWMVDec->m_rgMotionCompBuffer_EMB;
    
    pPB->m_rgiCoefReconBuf = pWMVDec->m_rgiCoefReconBuf;
    pPB->m_pX9dct = pWMVDec->m_pX9dct;
    pPB->m_pX9dctUV[0] = pWMVDec->m_pX9dctU;
    pPB->m_pX9dctUV[1]= pWMVDec->m_pX9dctV;
    
    pPB->m_rgmv1_EMB = pWMVDec->m_rgmv1_EMB;

    {
        I32_WMV iX, iblk, b1MV, iOffset, iX2;
        
        for(iX=0; iX < 3; iX++)
        {
            for(b1MV = 0; b1MV < 2; b1MV++)
            {
                for(iblk=0; iblk<4; iblk++)
                {
                    if(b1MV == 0) 
                    {
                        iX2 = iX + (iblk&1);
                        if (iX2)
                            iOffset = 1 - (((iblk == 0 || iblk == 3) || (iX == 2 && iblk == 1)) << 1);
                        else // First block column blocks use upper right predictor for B
                            iOffset = 1;
                        pPMainLoop->m_PredictMVLUT.m_rgcBOffset[iX*5 + b1MV*4 + iblk] = iOffset;
                    }
                    else
                    {
                        iOffset = (iX < (2)) * 3 - 1;            
                        pPMainLoop->m_PredictMVLUT.m_rgcBOffset[iX*5 + 4] = iOffset;
                    }
                }
            }
        }
    }
    
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[0].m_uiMax  = (pPB->m_iNumBlockX << 5) - 4 + (((pPB->m_iNumBlockY << 5) - 4)<<16);
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[1].m_uiMax  = (pPB->m_iNumBlockX << 5) - 4 + (((pPB->m_iNumBlockY << 5) - 4)<<16);
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[2].m_uiMax  = (pPB->m_iNumBlockX << 5)  + (((pPB->m_iNumBlockY << 5))<<16);
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[4].m_uiMax  = (pPB->m_iNumBlockX << 4)  + (((pPB->m_iNumBlockY << 4))<<16);
    
    pPB->m_ppxliErrorQ = pWMVDec->m_ppxliErrorQ;
    
    ///*pPB->m_pDecodeInterError[XFORMMODE_8x4 -1] = g_DecodeInterError8x4_SSIMD_Fun;
    //pPB->m_pDecodeInterError[XFORMMODE_4x8 -1] = g_DecodeInterError4x8_SSIMD_Fun;
    //pPB->m_pDecodeInterError[XFORMMODE_4x4 -1] = g_DecodeInterError4x4_SSIMD_Fun;*/
    //pPB->m_pDecodeInterError8x8				 = g_DecodeInterError8x8_SSIMD_Fun;

    pPB->m_pDecodeIdct[XFORMMODE_8x4 -1] = g_IDCT8x4_WMV3_Fun;
    pPB->m_pDecodeIdct[XFORMMODE_4x8 -1] = g_IDCT4x8_WMV3_Fun;
    pPB->m_pDecodeIdct[XFORMMODE_4x4 -1] = g_IDCT4x4_WMV3_Fun;

    pPB->m_pIDCTDec16_WMV3 = g_IDCTDec16_WMV3_Fun;
    pPB->m_pAddError = g_AddError_SSIMD_Fun;
    //pPB->m_pAddError_new = g_AddError_SSIMD_Fun_new;

    pPB->m_pAddNull = g_AddNull_SSIMD_Fun;

    pPB->m_pInterpolateBlock_00 = g_InterpolateBlock_00_SSIMD_Fun;
    pPB->m_pInterpolateBlock_00_new = g_InterpolateBlock_00_SSIMD_Fun_new;
    pPB->m_pSubBlkIDCTClear = g_SubBlkIDCTClear_EMB;



#if  defined(_ARM_)
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            pPB->m_pInterpolateBlockBilinear[i][j] = g_InterpolateBlockBilinear_SSIMD_Fun;
        }
    }
    pPB->m_pInterpolateBlockBilinear[0][2] = g_InterpolateBlockBilinear_SSIMD_01_Fun;
	pPB->m_pInterpolateBlockBilinear[2][0] = g_InterpolateBlockBilinear_SSIMD_10_Fun;
	pPB->m_pInterpolateBlockBilinear[2][2] = g_InterpolateBlockBilinear_SSIMD_11_Fun;

#else
	for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            pPB->m_pInterpolateBlockBilinear[i][j] = g_InterpolateBlockBilinear_SSIMD_Fun;
            pPB->m_pInterpolateBlockBilinear_new[i][j] = g_InterpolateBlockBilinear_SSIMD_Fun_new;
        }
    }

    pPB->m_pInterpolateBlockBilinear[0][2] = g_InterpolateBlockBilinear_SSIMD_01_Fun;
	pPB->m_pInterpolateBlockBilinear[2][0] = g_InterpolateBlockBilinear_SSIMD_10_Fun;
	pPB->m_pInterpolateBlockBilinear[2][2] = g_InterpolateBlockBilinear_SSIMD_11_Fun;

#endif

#if VOARMV7
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            pPB->m_pInterpolateBlockBilinear_new[i][j] = g_InterpolateBlockBilinear_SSIMD_Fun_new;
        }
    }

     pPB->m_pInterpolateBlockBilinear_new[0][2] = g_InterpolateBlockBilinear_SSIMD_01_Fun_new;
     pPB->m_pInterpolateBlockBilinear_new[2][0] = g_InterpolateBlockBilinear_SSIMD_10_Fun_new;
     pPB->m_pInterpolateBlockBilinear_new[2][2] = g_InterpolateBlockBilinear_SSIMD_11_Fun_new;

#endif

    //pPB->m_iFrmMBOffset[0] = 0;
    //pPB->m_iFrmMBOffset[1] = 0;
    
    for(i = 0; i< 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            if(i == (XFORMMODE_8x4-1))
            {
                pPB->m_uiIDCTClearOffset[i][j] = (j<<3);
                if(j == 1)   break;
            }
            if(i == (XFORMMODE_4x8-1))
            {
                pPB->m_uiIDCTClearOffset[i][j] = j + 0xc0;
                if(j == 1)   break;
            }
            if(i == (XFORMMODE_4x4-1))
            {
                pPB->m_uiIDCTClearOffset[i][j] =  ((j&2)<<2) + (j&1)+ 0x40;
                
            }
        }
    }
    
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[0].m_uiMin = 28 + (28<<16);
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[1].m_uiMin = 60 + (60<<16);
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[2].m_uiMin = 64 + (64<<16);
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[4].m_uiMin = 32 + (32<<16);
    
    pPMainLoop->m_PredictMVLUT.U.U.m_RndTbl[0] = 0;
    pPMainLoop->m_PredictMVLUT.U.U.m_RndTbl[1] = 0;
    pPMainLoop->m_PredictMVLUT.U.U.m_RndTbl[2] = 0;
    pPMainLoop->m_PredictMVLUT.U.U.m_RndTbl[3] = 1;
    
    pPB->m_iBlkXformTypeLUT[0] =  XFORMMODE_8x8, 
    pPB->m_iBlkXformTypeLUT[1] = pPB->m_iBlkXformTypeLUT[2] = pPB->m_iBlkXformTypeLUT[3] =  XFORMMODE_8x4;
    pPB->m_iBlkXformTypeLUT[4] = pPB->m_iBlkXformTypeLUT[5] = pPB->m_iBlkXformTypeLUT[6]= XFORMMODE_4x8;
    pPB->m_iBlkXformTypeLUT[7] =   XFORMMODE_4x4;
    
    pPB->m_iSubblkPatternLUT[0] = 0;
    pPB->m_iSubblkPatternLUT[1] = 1;
    pPB->m_iSubblkPatternLUT[2] = 2;
    pPB->m_iSubblkPatternLUT[3] = 3;
    pPB->m_iSubblkPatternLUT[4] = 1;
    pPB->m_iSubblkPatternLUT[5] = 2;
    pPB->m_iSubblkPatternLUT[6] = 3;
    pPB->m_iSubblkPatternLUT[7] = 0;


    pPMainLoop->m_SSIMD_DstLUT_1MV[0] = 0;
    pPMainLoop->m_SSIMD_DstLUT_1MV[1] = 2;
    pPMainLoop->m_SSIMD_DstLUT_1MV[2] = 80;
    pPMainLoop->m_SSIMD_DstLUT_1MV[3] = 80+2;
    pPMainLoop->m_SSIMD_DstLUT_1MV[4] = 0;
    pPMainLoop->m_SSIMD_DstLUT_1MV[5] = 0;


    
    pPMainLoop->m_SSIMD_DstLUT_BK[0] = 0;
    pPMainLoop->m_SSIMD_DstLUT_BK[1] = 0;
    pPMainLoop->m_SSIMD_DstLUT_BK[2] = 0;
    pPMainLoop->m_SSIMD_DstLUT_BK[3] = 0;
    pPMainLoop->m_SSIMD_DstLUT_BK[4] = 0;
    pPMainLoop->m_SSIMD_DstLUT_BK[5] = 0;
    
    pPB->m_iNumCoefTbl[0] = 64;
    pPB->m_iNumCoefTbl[1] = 32;
    pPB->m_iNumCoefTbl[2] = 32;
    pPB->m_iNumCoefTbl[3] = 0;
    pPB->m_iNumCoefTbl[4] = 16;

    for (i=0; i<64; i++)
    {
        if (i < 8)
            pPB->m_IDCTShortCutLUT[i] = 0;
        else
            pPB->m_IDCTShortCutLUT[i] = 1 << (i & 0x7);
    }

         //memcpy(pPMainLoop ->m_rgiSubsampleBicubic, gaSubsampleBicubic, sizeof(I16_WMV)*16);
    for(i = 0; i< 16; i++)
    {
        pPB->m_rgiSubsampleBicubic[i] = (I8_WMV)gaSubsampleBicubic[i];
    }

    pPB->m_rgiSubsampleBicubic[0] = 0x00;
    pPB->m_rgiSubsampleBicubic[1] = 0x80;
    pPB->m_rgiSubsampleBicubic[2] = 0x00;
    pPB->m_rgiSubsampleBicubic[3] = 0x80;
    
   // *(U32_WMV *)pPB->m_rgiSubsampleBicubic = 0x80008000;

    pPB->m_uiffff07ff =  0xffff07ff;
    pPB->m_uiffff7fff =  0xffff7fff;
    pPB->m_uiffff1fff =  0xffff1fff;;
    pPB->m_uic000c000 = 0xc000c000;
    pPB->m_uif000f000 = 0xf000f000;;
    pPB->m_ui00ff00ff = 0x00ff00ff;;


    {
        I32_WMV iUV, b1MV;

        for(iUV=0; iUV<2; iUV++)
            for(b1MV=0; b1MV<2; b1MV++) {
                I32_WMV iSrcStride = pPB->m_iWidthPrevY;

                    I32_WMV iParam1 = 40, iParam2 = 20, iParam3 = 3;

                if(iUV == 1)
                    iSrcStride = pPB->m_iWidthPrevUV;

                pPB->m_rgiNewVertTbl[iUV][b1MV][0] = 3*iSrcStride + (iSrcStride<<(3+b1MV)) -1 ;
                pPB->m_rgiNewVertTbl[iUV][b1MV][1] = (iParam1 <<(3+b1MV)) - iParam2;
                pPB->m_rgiNewVertTbl[iUV][b1MV][2] = 3*iSrcStride + (iSrcStride<<(3+b1MV)) - iParam3 ;
                pPB->m_rgiNewVertTbl[iUV][b1MV][3] = (iParam1 <<(3+b1MV)) + 16;
            }
        }
}

//#pragma code_seg (EMBSEC_DEF)
Void_WMV SetupEMBPMainLoopCtl(tWMVDecInternalMember *pWMVDec,EMB_PBMainLoop *pPB)
{
    EMB_PMainLoop * pPMainLoop = &pWMVDec->m_EMB_PMainLoop;
    //EMB_PBMainLoop * pMainLoop = (EMB_PBMainLoop * )pPMainLoop;

    InitEMBPMainLoopCtl(pWMVDec,pPB);

    pPB->m_rgiMVDecLUT_EMB = pWMVDec->m_rgiMVDecLUT_EMB;
    pPB->m_iQP= ((pWMVDec->m_iStepSize << 1) - 1) + pWMVDec->m_bHalfStep;
    pPB->m_iMVSwitchCodingMode = pWMVDec->m_iMVSwitchCodingMode;
    pPB->m_iSkipbitCodingMode = pWMVDec->m_iSkipbitCodingMode;
     pPB->m_pHufMVTable_V9 = pWMVDec->m_pHufMVTable_V9;
     pPB->m_bDQuantOn = pWMVDec->m_bDQuantOn;
     pPB->m_bDCTTable_MB = pWMVDec->m_bDCTTable_MB;
     pPB->m_bMBXformSwitching = pWMVDec->m_bMBXformSwitching;
     pPB->m_bHalfPelMV = 
         (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL) | (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR);
     pPB->m_pHufNewPCBPCYDec = pWMVDec->m_pHufNewPCBPCYDec;
     pPB->m_pHufMBXformTypeDec = pWMVDec->m_pHufMBXformTypeDec;
     pPB->m_rgiCurrBaseBK[0] = 0;
     pPB->m_rgiCurrBaseBK[1] = 0;
     pPB->m_rgiBlkIdx[4] =
     pPB->m_rgiBlkIdx[5] = 0;
     
     pPB->m_iXMVRange = pWMVDec->m_iXMVRange;
     pPB->m_iYMVRange = pWMVDec->m_iYMVRange;
     pPB->m_iXMVFlag = pWMVDec->m_iXMVFlag;
     pPB->m_iYMVFlag = pWMVDec->m_iYMVFlag;
     pPB->m_iLogXRange = pWMVDec->m_iLogXRange;
     pPB->m_iLogYRange = pWMVDec->m_iLogYRange;
     
     pPB->m_iNumBFrames = pWMVDec->m_iNumBFrames;
     pPB->m_bUVHpelBilinear = pWMVDec->m_bUVHpelBilinear;
     
     pPB->m_iFrameXformMode = pWMVDec->m_iFrameXformMode;
     pPB->m_prgDQuantParam = pWMVDec->m_prgDQuantParam;
     pPB->m_pInterDCTTableInfo_Dec_Set = pWMVDec->m_pInterDCTTableInfo_Dec_Set;
     pPB->m_pIntraDCTTableInfo_Dec_Set = pWMVDec->m_pIntraDCTTableInfo_Dec_Set;
     pPB->m_ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
     pPB->m_ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
     pPB->m_iDCTHorzFlags= pWMVDec->m_iDCTHorzFlags;
     pPB->m_iOverlap= pWMVDec->m_iOverlap;
     
     pPB->m_pZigzagScanOrder= pWMVDec->m_pZigzagScanOrder;
     pPB->m_iFilterType[0] =  pWMVDec->m_iFilterType;
     pPB->m_iFilterType[1] = FILTER_BILINEAR;

     pPB->m_iRndCtrl  = (pWMVDec->m_iRndCtrl <<16)|pWMVDec->m_iRndCtrl ;
     
     pPB->m_ppxliRefBlkBase[0] = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;//pWMVDec->m_ppxliRef0YPlusExp;
     pPB->m_ppxliRefBlkBase[1] = pPB->m_ppxliRefBlkBase[0] + 8;
     pPB->m_ppxliRefBlkBase[2] = pPB->m_ppxliRefBlkBase[0] + pWMVDec->m_iWidthPrevY*8;
     pPB->m_ppxliRefBlkBase[3] = pPB->m_ppxliRefBlkBase[0] + pWMVDec->m_iWidthPrevY*8 + 8;
     pPB->m_ppxliRefBlkBase[4] = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
     pPB->m_ppxliRefBlkBase[5] = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
     

     pPB->m_ppxliCurrBlkBase[0] = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
     pPB->m_ppxliCurrBlkBase[1] = pPB->m_ppxliCurrBlkBase[0] + 8;
     pPB->m_ppxliCurrBlkBase[2] = pPB->m_ppxliCurrBlkBase[0] + pWMVDec->m_iWidthPrevY*8;
     pPB->m_ppxliCurrBlkBase[3] = pPB->m_ppxliCurrBlkBase[0] + pWMVDec->m_iWidthPrevY*8 + 8;
     pPB->m_ppxliCurrBlkBase[4] = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
     pPB->m_ppxliCurrBlkBase[5] = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
     
     pPMainLoop->m_rgfPredictHybridMV_EMB[0] = PredictHybridMVTop_EMB;
     pPMainLoop->m_rgfPredictHybridMV_EMB[1] = PredictHybridMV_EMB;
     pPMainLoop->m_rgfPredictHybridMV_EMB[2] = PredictHybridMV_EMB;
     pPMainLoop->m_ppPredictHybridMV_EMB = pPMainLoop->m_rgfPredictHybridMV_EMB;

     pPB->m_iBlkMBBase[X_INDEX] = pPB->m_iBlkMBBase[Y_INDEX] = 0;
     
     pPB->m_pIntraBlockRow[0] = pPB->m_pIntraBlockRow[1]  = pPB->pThreadIntraBlockRow0[0];
     pPB->m_pIntraBlockRow[2] = pPB->m_pIntraBlockRow[3]  = pPB->pThreadIntraBlockRow0[2];
     pPB->m_pIntraBlockRow[4]  = pPB->pThreadIntraMBRowU0[0];
     pPB->m_pIntraBlockRow[5]  = pPB->pThreadIntraMBRowV0[0];
     
     pPB->m_iACColPredShift =  pWMVDec->m_iACColPredShift;
     pPB->m_iACRowPredShift =  pWMVDec->m_iACRowPredShift;
     pPB->m_iACColPredOffset =  pWMVDec->m_iACColPredOffset;
     pPB->m_iACRowPredOffset =  pWMVDec->m_iACRowPredOffset;

     pPB->m_pHufBlkXformTypeDec = pWMVDec->m_pHufBlkXformTypeDec;
     pPB->m_pHuf4x4PatternDec = pWMVDec->m_pHuf4x4PatternDec;
     
     pPB->m_pZigzag[ XFORMMODE_8x4 -1] = pWMVDec->m_p8x4ZigzagSSIMD;
     pPB->m_pZigzag[ XFORMMODE_4x8 -1] = pWMVDec->m_p4x8ZigzagScanOrder;
     pPB->m_pZigzag[ XFORMMODE_4x4 -1] = pWMVDec->m_p4x4ZigzagSSIMD;

     pPB->m_ui001f001fPiRndCtl = 0x001f001f + pPB->m_iRndCtrl;
     pPB->m_ui000f000fPiRndCtl = 0x000f000f + pPB->m_iRndCtrl;
     pPB->m_ui00070007PiRndCtl = 0x00070007 + pPB->m_iRndCtrl;
     pPB->m_ui00030003PiRndCtl = 0x00030003 + pPB->m_iRndCtrl;
     pPB->m_ui00200020MiRndCtl = 0x00200020 - pPB->m_iRndCtrl;
     pPB->m_ui00400040MiRndCtl = 0x00400040 - pPB->m_iRndCtrl;
     pPB->m_ui00080008MiRndCtl = 0x00080008 - pPB->m_iRndCtrl;

     pPB->m_uintNumMBX =  pWMVDec->m_uintNumMBX;
}

//#pragma code_seg (EMBSEC_PML)       
#if defined(VOARMV7)|| defined(NEW_C)
tWMVDecodeStatus MotionCompMB_WMV3_EMB_new (EMB_PMainLoop * pPMainLoop,I32_WMV iMBY,I32_WMV iMBX,EMB_PBMainLoop *pPB)
{
    tWMVDecodeStatus result = WMV_Succeeded;
    U8_WMV *pDst;
	
    //pDst = pPB->m_ppxliCurrBlkBase[0] + pPB->m_iFrmMBOffset[0];
	pDst = pPB->m_ppxliCurrBlkBase[0] + iMBY*16*pPB->m_iWidthPrevY+iMBX*16;

    result = g_MotionComp_X9_EMB_new (pPMainLoop, pDst, pPB->m_iWidthPrev[0],iMBY,iMBX,0, 1,pPB);
    if(result)
        return -1;


    pDst = pPB->m_ppxliCurrBlkBase[4] + iMBY*8*pPB->m_iWidthPrevUV+iMBX*8;
    result = g_MotionComp_X9_EMB_new (pPMainLoop, pDst, pPB->m_iWidthPrev[1], iMBY,iMBX, 4, 0,pPB);
    if(result)
        return -1;

    pDst = pPB->m_ppxliCurrBlkBase[5] + iMBY*8*pPB->m_iWidthPrevUV+iMBX*8;
    result = g_MotionComp_X9_EMB_new (pPMainLoop, pDst, pPB->m_iWidthPrev[1], iMBY,iMBX, 5, 0,pPB);

    return result;
}
#else
tWMVDecodeStatus MotionCompMB_WMV3_EMB (EMB_PMainLoop * pPMainLoop,I32_WMV iMBY,I32_WMV iMBX,EMB_PBMainLoop *pPB)
{
    tWMVDecodeStatus result = WMV_Succeeded;
    I32_WMV iblk;
    I32_WMV iUV; 
    U8_WMV *pDst;

    result = g_MotionComp_X9_EMB (pPMainLoop,  iMBY,iMBX,0, 1,pPB);

    if(result)
        return -1;
    pPB->m_pSSIMD_DstOffSet = pPMainLoop->m_SSIMD_DstLUT_1MV;

    for(iblk = 0;iblk < 6; iblk++)
    {
        iUV = iblk>>2;
        if (iUV) { 
            result = g_MotionComp_X9_EMB (pPMainLoop, iMBY,iMBX, iblk, 0,pPB);
            if(result)
                return -1;
        }

		if(!iUV)
			pDst = pPB->m_ppxliCurrBlkBase[iblk] + iMBY*16*pPB->m_iWidthPrev[iUV]+iMBX*16;
		else
			pDst = pPB->m_ppxliCurrBlkBase[iblk] + iMBY*8*pPB->m_iWidthPrev[iUV]+iMBX*8;
        pPB->m_pAddNull(pDst, pPB->prgMotionCompBuffer_EMB + pPB->m_pSSIMD_DstOffSet[iblk], pPB->m_iWidthPrev[iUV]);

    }
    return result;
}


#endif
tWMVDecodeStatus Step1_Paser(tWMVDecInternalMember *pWMVDec,
                             CWMVMBMode* pmbmd,
                             CWMVMBMode* pmbmd_lastrow,
                            U32_WMV imbX,U32_WMV imbY,U32_WMV x,
                            I32_WMV *iIntra,I32_WMV *b1MV,EMB_PBMainLoop *pPB)
{
    EMB_PMainLoop  * pPMainLoop = &pWMVDec->m_EMB_PMainLoop;
    tWMVDecodeStatus result;

    //pWMVDec->m_ncols = imbX;
    //pWMVDec->m_bMBHybridMV = FALSE_WMV;

    result = decodeMBOverheadOfPVOP_WMV3_EMB (pPMainLoop, pmbmd, imbX, imbY,pPB);
    if (WMV_Succeeded != result)
        return result;
    
    *b1MV = (pmbmd->m_chMBMode == MB_1MV);          
    *iIntra = ChromaMV_EMB(pPMainLoop, imbX, imbY, *b1MV,pPB); 
    if(*iIntra == -1)
        return -1;

    //*******step 0 parser*******//
    if (*b1MV)
    {                
        if (pmbmd->m_bCBPAllZero == TRUE && !*iIntra)
        {
            I32_WMV iMVOffst = 4 * imbY * (I32_WMV) pWMVDec->m_uintNumMBX + 2 * imbX;
            if (pWMVDec->m_bLoopFilter) {
                memset(pmbmd->m_rgbCodedBlockPattern2, 0, 6);
                memset(pmbmd->m_rgcBlockXformMode, XFORMMODE_8x8, 6*sizeof(pmbmd->m_rgcBlockXformMode[0]));
            }

            if (!pWMVDec->m_pMotion[iMVOffst].I16[X_INDEX] && !pWMVDec->m_pMotion[iMVOffst].I16[Y_INDEX])
            {                       
                pmbmd->m_bSkip = TRUE_WMV;
                pmbmd->m_dctMd = INTER;
                result = WMV_Succeeded;
            }
            else
            {
                pmbmd->m_bSkip = FALSE_WMV;
                result = WMV_Succeeded;
            }
        }
        else
        {
            result = DecodeMB_X9_Fast_Parser (pPMainLoop, pmbmd,pmbmd_lastrow, imbX, imbY, TRUE_WMV,pPB);
			 if (WMV_Succeeded != result)
				return result;
        }
    }
    else {  // 4MV
        result = DecodeMB_X9_Fast_Parser (pPMainLoop, pmbmd,pmbmd_lastrow, imbX, imbY, FALSE_WMV,pPB);
		 if (WMV_Succeeded != result)
			 return result;
    }
    return result;
}

tWMVDecodeStatus ComputeLoopFilterFlags_EMB_MB(tWMVDecInternalMember *pWMVDec, 
                                               CWMVMBMode* pmbmd,
                                               CWMVMBMode* pmbmd_lastrow,
                                               U32_WMV imbY,U32_WMV imbX)
{
    I32_WMV iMBX, iMBY;
    Bool_WMV bTop;

    iMBY = imbY;
    iMBX = imbX;

    if (pWMVDec->m_cvCodecVersion == WMVA)
    {     
        if(pWMVDec->m_bCodecIsWVC1)
            bTop = (iMBY == 0 || pWMVDec->m_pbStartOfSliceRow[iMBY]);
        else
            bTop = (iMBY == 0);

        //for (iMBX = 0; iMBX < (I32_WMV) pWMVDec->m_uintNumMBX; iMBX++)
        {
            U8_WMV *pCBP = pmbmd->m_rgbCodedBlockPattern2;
            //U8_WMV *pCBPTop = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgbCodedBlockPattern2;
            U8_WMV *pCBPTop = (pmbmd_lastrow)->m_rgbCodedBlockPattern2;
            U8_WMV *pCBPLeft = (pmbmd - 1)->m_rgbCodedBlockPattern2;
            I8_WMV *pBlockXformMode = pmbmd->m_rgcBlockXformMode;
            //I8_WMV *pBlockXformModeTop  = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgcBlockXformMode;
            I8_WMV *pBlockXformModeTop  = (pmbmd_lastrow)->m_rgcBlockXformMode;
            I8_WMV *pBlockXformModeLeft = (pmbmd - 1)->m_rgcBlockXformMode;
            ComputeMBLoopFilterFlags_WMVA_EMB (pWMVDec, 
                                                            iMBX, iMBY, pmbmd->m_chMBMode,
                                                            pBlockXformMode, 
                                                            pBlockXformModeTop, 
                                                            pBlockXformModeLeft,
                                                            pCBP, pCBPTop, pCBPLeft, 
                                                            bTop, iMBX == 0);
        }
    }
    else
    {
        //for (iMBX = 0; iMBX < (I32_WMV) pWMVDec->m_uintNumMBX; iMBX++) 
        {
            U8_WMV *pCBP                = pmbmd->m_rgbCodedBlockPattern2;
            //U8_WMV *pCBPTop             = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgbCodedBlockPattern2;
            U8_WMV *pCBPTop             = (pmbmd_lastrow)->m_rgbCodedBlockPattern2;
            U8_WMV *pCBPLeft            = (pmbmd - 1)->m_rgbCodedBlockPattern2;
            I8_WMV *pBlockXformMode     = pmbmd->m_rgcBlockXformMode;
            //I8_WMV *pBlockXformModeTop  = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgcBlockXformMode;
            I8_WMV *pBlockXformModeTop  = (pmbmd_lastrow)->m_rgcBlockXformMode;
            I8_WMV *pBlockXformModeLeft = (pmbmd - 1)->m_rgcBlockXformMode;
            
            ComputeMBLoopFilterFlags_WMV9_EMB (pWMVDec, 
                                                                iMBX, iMBY, pmbmd->m_chMBMode,
                                                                pBlockXformMode, 
                                                                pBlockXformModeTop, 
                                                                pBlockXformModeLeft,
                                                                pCBP, pCBPTop, pCBPLeft, 
                                                                iMBY == 0, iMBX == 0);
            
        }
    }
    return 0;
}

tWMVDecodeStatus ProcessMBRow_MC(tWMVDecInternalMember *pWMVDec,CWMVMBMode* pmbmd,I32_WMV imbY,
                                                     I32_WMV *bIntraFlag, I32_WMV *b1MVFlag,EMB_PBMainLoop *pPB, U32_WMV threadflag )
{
    int x,imbX;
    tWMVDecodeStatus result = WMV_Succeeded;
    I32_WMV iIntra, b1MV;
    EMB_PMainLoop  * pPMainLoop = &pWMVDec->m_EMB_PMainLoop;
    //EMB_PBMainLoop *pPBOther =&pPMainLoop->PB[threadflag^1];

    for (x = 0, imbX = 0; imbX < pPB->m_uintNumMBX; imbX++, x += MB_SIZE)
    {   

        GetBackUpDiffMV(pPMainLoop,imbX,imbY,pPB);
        iIntra = bIntraFlag[imbX];
        b1MV = b1MVFlag[imbX];
        if (b1MV)
        { 
            if (pmbmd->m_bCBPAllZero == TRUE && !iIntra)
            {
                I32_WMV iMVOffst = 4 * imbY * (I32_WMV)pPB->m_uintNumMBX + 2 * imbX;
                if (!pPB->m_pMotion[iMVOffst].I16[X_INDEX] && !pPB->m_pMotion[iMVOffst].I16[Y_INDEX])
                { 					
					//ZOU TODO
					//TODO tongbu
#ifdef USE_FRAME_THREAD
					if(pWMVDec->CpuNumber > 1)
					{
						volatile I32_WMV *ref_decodeprocess = &pWMVDec->m_pfrmRef0Q->m_decodeprocess;
						while((I32_WMV)imbY>=*ref_decodeprocess)
						{
							thread_sleep(0);
							bkprintf("p3");
						}
					}
#endif
#ifndef VOARMV6
					if (pWMVDec->m_bLuminanceWarp || pWMVDec->m_scaling)//zou 20130307
					{
						U32_WMV i,j;
						U8_WMV RefY[16*16],RefU[8*8],RefV[8*8];
						U8_WMV* pSrcY=NULL,*pSrcU = NULL,*pSrcV = NULL;
						U32_WMV stride_y=pPB->m_iWidthPrev[0],stride_uv=pPB->m_iWidthPrev[1];						

						pSrcY = pPB->m_ppxliRefBlkBase[0] + imbY*16*pPB->m_iWidthPrev[0]+imbX*16;
						pSrcU = pPB->m_ppxliRefBlkBase[4] + imbY*8*pPB->m_iWidthPrev[1]+imbX*8;
						pSrcV = pPB->m_ppxliRefBlkBase[5] + imbY*8*pPB->m_iWidthPrev[1]+imbX*8;
						if(pWMVDec->m_scaling)
						{
							AdjustReconRange_MB(pSrcY,pPB->m_iWidthPrev[0], RefY,16,16,pWMVDec->m_scaling==1);
							AdjustReconRange_MB(pSrcU,pPB->m_iWidthPrev[1], RefU,8,8,pWMVDec->m_scaling==1);
							AdjustReconRange_MB(pSrcV,pPB->m_iWidthPrev[1], RefV,8,8,pWMVDec->m_scaling==1);
							pSrcY = RefY; pSrcU = RefU; pSrcV = RefV;
							stride_y = 16; stride_uv = 8;
						}


						if(pWMVDec->m_bLuminanceWarp)	
						{
							if(pWMVDec->m_pLUT == NULL || pWMVDec->m_pLUTUV == NULL)
								return -1;
							IntensityCompensation_MB(pSrcY, stride_y,	RefY,16,pWMVDec->m_pLUT,16);						
							IntensityCompensation_MB(pSrcU, stride_uv,RefU,8,pWMVDec->m_pLUTUV,8);
							IntensityCompensation_MB(pSrcV, stride_uv,RefV,8,pWMVDec->m_pLUTUV,8);
						}

						g_MotionCompZeroMotion_WMV_Fun( pPB->m_ppxliCurrBlkBase[0] + imbY*16*pPB->m_iWidthPrev[0]+imbX*16,
																		pPB->m_ppxliCurrBlkBase[4] + imbY*8*pPB->m_iWidthPrev[1]+imbX*8,
																		pPB->m_ppxliCurrBlkBase[5] + imbY*8*pPB->m_iWidthPrev[1]+imbX*8,
																		RefY,RefU,RefV,pPB->m_iWidthPrevY,pPB->m_iWidthPrevUV,
																		16,8);
					}
					else
#endif
					{
						g_MotionCompZeroMotion_WMV_Fun( pPB->m_ppxliCurrBlkBase[0] + imbY*16*pPB->m_iWidthPrev[0]+imbX*16,
																		pPB->m_ppxliCurrBlkBase[4] + imbY*8*pPB->m_iWidthPrev[1]+imbX*8,
																		pPB->m_ppxliCurrBlkBase[5] + imbY*8*pPB->m_iWidthPrev[1]+imbX*8,
																		pPB->m_ppxliRefBlkBase[0] + imbY*16*pPB->m_iWidthPrev[0]+imbX*16,
																		pPB->m_ppxliRefBlkBase[4] + imbY*8*pPB->m_iWidthPrev[1]+imbX*8,
																		pPB->m_ppxliRefBlkBase[5] + imbY*8*pPB->m_iWidthPrev[1]+imbX*8,
																		pPB->m_iWidthPrevY,pPB->m_iWidthPrevUV,pPB->m_iWidthPrevY,pPB->m_iWidthPrevUV);
					}
                    result = WMV_Succeeded;
                }
                else
                {
#if defined(VOARMV7)||defined(NEW_C)
                    result = MotionCompMB_WMV3_EMB_new (pPMainLoop,imbY,imbX,pPB);
#else
                    result = MotionCompMB_WMV3_EMB (pPMainLoop,imbY,imbX,pPB);
#endif                    
                    if (WMV_Succeeded != result)
                        return result;  
                }
            }
            else
            {
#if defined(VOARMV7)||defined(NEW_C)
                result = DecodeMB_X9_Fast_IdctMC_new(pPMainLoop, pmbmd,  imbX, imbY, TRUE_WMV,pPB);
#else
                result = DecodeMB_X9_Fast_IdctMC(pPMainLoop, pmbmd,  imbX, imbY, TRUE_WMV,pPB);
#endif
                if (WMV_Succeeded != result)
                    return result;  
            }
        }
        else
		{  // 4MV
#if defined(VOARMV7)||defined(NEW_C)
            result = DecodeMB_X9_Fast_IdctMC_new (pPMainLoop, pmbmd,  imbX, imbY, FALSE_WMV,pPB);
#else
             result = DecodeMB_X9_Fast_IdctMC(pPMainLoop, pmbmd,  imbX, imbY, FALSE_WMV,pPB);
#endif
        }                  
        if (WMV_Succeeded != result)
            return result;   
        pmbmd++; 

    } //for (x = 0, imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++, x += MB_SIZE)
     
    return result;
}

tWMVDecodeStatus ProcessMBRow_Parser(tWMVDecInternalMember *pWMVDec,
                                                        CWMVMBMode* pmbmd,
                                                        CWMVMBMode* pmbmd_lastrow,
                                                        int imbY,
                                                        I32_WMV *bIntraFlag, I32_WMV *b1MVFlag,
                                                        EMB_PBMainLoop *pPB)
{
    U32_WMV x,imbX;
    tWMVDecodeStatus result = WMV_Succeeded;
    //EMB_PMainLoop  * pPMainLoop = &pWMVDec->m_EMB_PMainLoop;

    if (pWMVDec->m_bSliceWMVA)
    {
        if (pWMVDec->m_pbStartOfSliceRow[imbY]) {
            pWMVDec->m_uiCurrSlice++; 
            if(BS_flush(pWMVDec->m_pbitstrmIn))
                return WMV_Failed; 
            if (ReadSliceHeaderWMVA (pWMVDec, imbY) != ICERR_OK)
                return WMV_Failed;
            pWMVDec->m_bFirstEscCodeInFrame = TRUE;
        }
    }
    
    if(pWMVDec->m_bStartCode) {
        if(SliceStartCode(pWMVDec, imbY)!= WMV_Succeeded)
            return WMV_Failed;
    } 

    for (x = 0, imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++, x += MB_SIZE) 
    {    
        pPB->m_rgiCurrBaseBK[1] = imbY*pWMVDec->m_uintNumMBX+imbX; 

        if(imbX == 0 && imbY == 2 &&pWMVDec->m_nPframes == 1)
          imbX =imbX;

        pPB->m_pDiffMV_EMB =  pPB->m_pDiffMV_EMB_Bak + imbX*6;  //zou mv


        if(imbX == 3 && imbY == 3)
             imbX =imbX;

        result= Step1_Paser(pWMVDec, pmbmd, pmbmd_lastrow,imbX, imbY, x,&bIntraFlag[imbX], &b1MVFlag[imbX],pPB);     
        if (WMV_Succeeded != result)
            return result;

        pPB->m_rgiCurrBaseBK[0] += 2; 
        pPB->m_iBlkMBBase[0] += 2;            
        //pPB->m_iFrmMBOffset[0] += MB_SIZE;
        //pPB->m_iFrmMBOffset[1] += BLOCK_SIZE;  

        if(pWMVDec->m_bLoopFilter) 
            ComputeLoopFilterFlags_EMB_MB(pWMVDec, pmbmd, pmbmd_lastrow,imbY,imbX);

        pmbmd++;
        pmbmd_lastrow++;
    } //for (x = 0, imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++, x += MB_SIZE)

    return result;
}

//static int g_dbg_cnt10, g_dbg_cnt11, g_dbg_cnt12;
//#pragma code_seg (EMBSEC_PML)
tWMVDecodeStatus WMVideoDecDecodeP_X9_EMB (tWMVDecInternalMember *pWMVDec)
{
    U8_WMV * ppxliCurrQY;
    U8_WMV * ppxliCurrQU;
    U8_WMV * ppxliCurrQV;
    U32_WMV imbY;
    U32_WMV i;
    Bool_WMV bFrameInPostBuf;
    CWMVMBMode* pmbmd;
    CWMVMBMode* pmbmd_lastrow=NULL;
    //tWMVDecodeStatus result = WMV_Succeeded;
    EMB_PMainLoop  * pPMainLoop = &pWMVDec->m_EMB_PMainLoop;
    Bool_WMV bTopRowOfSlice = TRUE;   
    EMB_PBMainLoop *pMainPB = &pPMainLoop->PB[0];
    //EMB_PBMainLoop *pThreadPB = &pPMainLoop->PB[1];
	U32_WMV MCBuffer[320];

	//pWMVDec->m_scaling =0;
	//pWMVDec->m_bLuminanceWarp = 0;
#ifdef VOARMV6
	if ( pWMVDec->m_scaling || pWMVDec->m_bLuminanceWarp)//zou 30130307
	{
		U32_WMV mb_row = 0;
		pWMVDec->m_EMB_PMainLoop.PB[0].m_ppxliRefBlkBase[0] = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
		pWMVDec->m_EMB_PMainLoop.PB[0].m_ppxliRefBlkBase[4] = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
		pWMVDec->m_EMB_PMainLoop.PB[0].m_ppxliRefBlkBase[5] = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;

		for(mb_row = 0;mb_row<2+pWMVDec->m_uintNumMBY+2;mb_row++)//pad 4 mb row 
		{	
			AdjustReconRange_mbrow(pWMVDec, mb_row);
			IntensityCompensation_mbrow (pWMVDec, mb_row);
		}
		pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmRef0Process->m_pucYPlane;
		pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Process->m_pucUPlane;
		pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Process->m_pucVPlane;
		pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
	}
#endif

	pWMVDec->iSliceHead		= 0;
	pWMVDec->iSliceHead_Pre	= 0;
	pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;
    
    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    
    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];
    
    if (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR)
        pWMVDec->m_iFilterType = FILTER_BILINEAR;
    else
        pWMVDec->m_iFilterType = FILTER_BICUBIC;
  
    // update overlap related variables
    UpdateDCStepSize(pWMVDec, pWMVDec->m_iStepSize);
        
    // SET_NEW_FRAME
    pWMVDec->m_iHalfPelMV =
        (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL || pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR);
 

    SetTransformTypeHuffmanTable (pWMVDec, pWMVDec->m_iStepSize);
    SetupMVDecTable_EMB(pWMVDec);
    
    ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    pmbmd = pWMVDec->m_rgmbmd;   

	pWMVDec->m_rgmv1_EMB = pWMVDec->m_pfrmCurrQ->m_rgmv1_EMB;

    SetupEMBPMainLoopCtl(pWMVDec,pMainPB);
	//ZOU TODO
    pMainPB->prgMotionCompBuffer_EMB =  MCBuffer;//pWMVDec->m_rgMotionCompBuffer_EMB;

	if ( pWMVDec->m_iNumBFrames > 0 && (pWMVDec->m_scaling || pWMVDec->m_bLuminanceWarp))
		pWMVDec->m_pfrmRef0Process->m_decodeprocess = -5;

    {
        for (imbY = 0 ; imbY < pWMVDec->m_uintNumMBY; imbY++)
        {  
#ifdef USE_FRAME_THREAD
			if (pWMVDec->CpuNumber > 1  && pWMVDec->m_iNumBFrames > 0 && (pWMVDec->m_scaling || pWMVDec->m_bLuminanceWarp))
			{
				//tongbu since maybe do sclaing for B frame
				volatile I32_WMV *ref_decodeprocess = &pWMVDec->m_pfrmRef0Q->m_decodeprocess;
				while((int)imbY >= *ref_decodeprocess)
					thread_sleep(0);//printf("p ");
			}
#endif
            pMainPB->m_rgiCurrBaseBK[0] = 2*imbY*(pWMVDec->m_uintNumMBX<<1);
            pMainPB->m_iBlkMBBase[Y_INDEX] = (I16_WMV)(2*imbY);
             pMainPB->m_iBlkMBBase[X_INDEX] = 0;
            pWMVDec->m_nrows = imbY;	

            ppxliCurrQY = pMainPB->m_ppxliCurrBlkBase[0] + imbY*16*pMainPB->m_iWidthPrev[0]+0*16;
            ppxliCurrQU = pMainPB->m_ppxliCurrBlkBase[4] + imbY*8*pMainPB->m_iWidthPrev[1]+0*8;
            ppxliCurrQV = pMainPB->m_ppxliCurrBlkBase[5] + imbY*8*pMainPB->m_iWidthPrev[1]+0*8;
            
            if (pWMVDec->m_iOverlap && pWMVDec->m_bSliceWMVA && pWMVDec->m_pbStartOfSliceRow[imbY] )
                bTopRowOfSlice = TRUE;

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
                (pmbmd+i)->m_chMBMode =  (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_chMBMode;
                (pmbmd+i)->m_iQP =  pMainPB->m_iQP;
                (pmbmd+i)->m_bBoundary =  (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_bBoundary;
                (pmbmd+i)->m_bOverlapIMB = (pWMVDec->m_rgmbmd + imbY*pWMVDec->m_uintNumMBX +i)->m_bOverlapIMB;
            }

            pWMVDec->error_flag_main = ProcessMBRow_Parser(pWMVDec,
                                        pmbmd,
                                        pmbmd_lastrow,
                                        imbY, 
                                        pMainPB->bIntraFlag, pMainPB->b1MVFlag,pMainPB);  
			if(pWMVDec->error_flag_main)
				return -1;

#ifndef VOARMV6
			if ( pWMVDec->m_iNumBFrames > 0 && (pWMVDec->m_scaling || pWMVDec->m_bLuminanceWarp))//zou 30130307
			{ //for B frame to use

				if( pWMVDec->m_bLuminanceWarp&&(pWMVDec->m_pLUT == NULL || pWMVDec->m_pLUTUV == NULL))
					return -1;

				if(imbY==0)
				{
					AdjustReconRange_mbrow(pWMVDec, 0);
					IntensityCompensation_mbrow (pWMVDec, 0);
					AdjustReconRange_mbrow(pWMVDec, 1);
					IntensityCompensation_mbrow (pWMVDec, 1);
				}
				else if(imbY == pWMVDec->m_uintNumMBY-1)
				{
					AdjustReconRange_mbrow(pWMVDec, imbY+3);
					IntensityCompensation_mbrow (pWMVDec, imbY+3);
					AdjustReconRange_mbrow(pWMVDec, imbY+4);
					IntensityCompensation_mbrow (pWMVDec, imbY+4);				

					//printf(" %d \n",pWMVDec->m_pfrmRef0Process->m_decodeprocess);
				}
				AdjustReconRange_mbrow(pWMVDec, imbY+2);
				IntensityCompensation_mbrow (pWMVDec, imbY+2);

				pWMVDec->m_pfrmRef0Process->m_decodeprocess = imbY;

				//printf("PPP pWMVDec->m_pfrmRef0Process->m_decodeprocess = %d \n",pWMVDec->m_pfrmRef0Process->m_decodeprocess);
			}
#endif

            pWMVDec->error_flag_main = ProcessMBRow_MC (pWMVDec,pmbmd,(I32_WMV)imbY,
                                         pMainPB->bIntraFlag, pMainPB->b1MVFlag,pMainPB,0);
			if(pWMVDec->error_flag_main)
				return -1;

            if ( pWMVDec->m_iOverlap )
            {
                OverlapMBRow_Slice_EMB (pWMVDec, imbY, 
                                                   ppxliCurrQY,ppxliCurrQU,ppxliCurrQV,
                                                   bTopRowOfSlice ,FALSE,0);
                bTopRowOfSlice = FALSE;
                if ( (imbY < pWMVDec->m_uintNumMBY - 1 && pWMVDec->m_pbStartOfSliceRow[imbY + 1]) 
                    || (imbY == pWMVDec->m_uintNumMBY - 1) )
                    OverlapMBRow_Slice_EMB (pWMVDec, imbY,
                                                    ppxliCurrQY + pWMVDec->m_iMBSizeXWidthPrevY, 
                                                    ppxliCurrQU + pWMVDec->m_iBlkSizeXWidthPrevUV, 
                                                    ppxliCurrQV + pWMVDec->m_iBlkSizeXWidthPrevUV,
                                                    FALSE ,TRUE,0);
            } 

			if(pWMVDec->CpuNumber > 1)
			{
				if (pWMVDec->m_bLoopFilter ) 
					DeblockMBRow(pWMVDec, imbY);

				if(imbY > 0)
				{
					Repeatpad_mbrow(pWMVDec,imbY-1);
					pWMVDec->m_pfrmCurrQ->m_decodeprocess = imbY-1;
					tbprintf("[thread%d]  PVOP decode process %d %d Cur 0x%x Ref 0x%x \n",pWMVDec->nthreads,pWMVDec->m_pfrmCurrQ->m_decodeprocess,pWMVDec->nInputFrameIndx,pWMVDec->m_pfrmCurrQ,pWMVDec->m_pfrmRef0Q);

				}
			}

            pPMainLoop->m_ppPredictHybridMV_EMB = pPMainLoop->m_rgfPredictHybridMV_EMB + 1; 
        }     
    }

    //repeat the last row
	if(pWMVDec->CpuNumber > 1 )
		Repeatpad_mbrow(pWMVDec,imbY-1);

    bFrameInPostBuf = FALSE_WMV;
    //voDumpYuv_P( pWMVDec,pWMVDec->m_nPframes); 

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
	   if (pWMVDec->m_bLoopFilter ) 
			DeblockSLFrame_V9 (pWMVDec, 0, pWMVDec->m_uiMBEnd0);
	}
#endif

#ifdef TRACEVC1
	{
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp," P   LOOP  finished  \n");
		fclose(fp);
	}
#endif

    //voDumpYuv_P( pWMVDec,pWMVDec->m_nPframes);

	if(pWMVDec->CpuNumber > 1 )
	{
		volatile I32_WMV *ref_decodeprocess = &pWMVDec->m_pfrmRef0Q->m_decodeprocess;
		while(*ref_decodeprocess < (I32_WMV)(pWMVDec->m_uintNumMBY+2)) //control the finish order
			;
	}
    return WMV_Succeeded;
}

static I32_WMV LOOPFILTERFLAGS_WMV9[17][16] =
{
 {0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfd,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xfc,0xfc,0xfd,0xfd,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xff,0xff,0xfe,0xfe,0xff,0xff},
 {0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfd,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xff,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff,0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff},
 {0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf7,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xf3,0xf3,0xf3,0xf3,0xf7,0xf7,0xf7,0xf7,0xfb,0xfb,0xfb,0xfb,0xff,0xff,0xff,0xff},
 {0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xff,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf7,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff},
 {0x0f,0x5f,0x9f,0xdf,0x6f,0x7f,0xff,0xff,0xaf,0xff,0xbf,0xff,0xef,0xff,0xff,0xff}
};

static I32_WMV LEFTFLAGS[5][16] =
{{0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfd,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xfc,0xfc,0xfd,0xfd,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xff,0xff,0xfe,0xfe,0xff,0xff},
 {0xfc,0xfc,0xfd,0xfd,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xff,0xff,0xfe,0xfe,0xff,0xff}
};
static I32_WMV LEFTPREDFLAGS[5][16] =
{{0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfd,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xff,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff,0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff},
 {0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff,0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff}
};
static I32_WMV TOPFLAGS[5][16] =
{{0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf7,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xf3,0xf3,0xf3,0xf3,0xf7,0xf7,0xf7,0xf7,0xfb,0xfb,0xfb,0xfb,0xff,0xff,0xff,0xff},
 {0xf3,0xf3,0xf3,0xf3,0xf7,0xf7,0xf7,0xf7,0xfb,0xfb,0xfb,0xfb,0xff,0xff,0xff,0xff}
};
static I32_WMV TOPPREDFLAGS[5][16] =
{{0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xff,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf7,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff},
 {0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff}
};
static I32_WMV MIDDLEFLAGS[16] =
 {0x0f,0x5f,0x9f,0xdf,0x6f,0x7f,0xff,0xff,0xaf,0xff,0xbf,0xff,0xef,0xff,0xff,0xff};

//#pragma code_seg (EMBSEC_DEF)
Void_WMV ComputeMBLoopFilterFlags_WMV9_EMB (tWMVDecInternalMember *pWMVDec,
                                       I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
                                       I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
                                       U8_WMV *pCBPCurr, U8_WMV *pCBPTop, U8_WMV *pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft)
{
    I32_WMV iBlk0 = iMBY * 4 * pWMVDec->m_uintNumMBX + iMBX * 2;
    I32_WMV iBlk2 = iBlk0 + 2 * pWMVDec->m_uintNumMBX;
    I32_WMV iBlkUp = iBlk0 - 2 * pWMVDec->m_uintNumMBX;
    I8_WMV* pchLoopFilterFlag = pWMVDec->m_pLoopFilterFlags[iMBY*pWMVDec->m_uintNumMBX + iMBX].chFlag;
    
    I32_WMV iBlk;
    
    // in the interests of time and complexity, sub-block Xforms in B frames 
    // aren't internally loop filtered - besides "loop filtering" isn't really
    // necessary - postfiltering with equal visual quality can take care of it
    if (pWMVDec->m_tFrmType == BVOP) {
		pchLoopFilterFlag[0] = (I8_WMV)0x0f;
        pchLoopFilterFlag[1] = (I8_WMV)0x0f;
        pchLoopFilterFlag[2] = (I8_WMV)0x0f;
        pchLoopFilterFlag[3] = (I8_WMV)0x0f;
        pchLoopFilterFlag[4] = (I8_WMV)0x0f;
        pchLoopFilterFlag[5] = (I8_WMV)0x0f;
    } 
    else {
		I32_WMV *pLoopFliterFlag16 = LOOPFILTERFLAGS_WMV9[16];
		iBlk = 0;
        do {
            if (pBlockXformMode[iBlk] == XFORMMODE_8x8)
                pchLoopFilterFlag[iBlk] = 0x0f;
            else if (pBlockXformMode[iBlk] == XFORMMODE_8x4)
                pchLoopFilterFlag[iBlk] = (I8_WMV)0xcf;
            else if (pBlockXformMode[iBlk] == XFORMMODE_4x8)
                pchLoopFilterFlag[iBlk] = 0x3f;
            else if (pBlockXformMode[iBlk] == XFORMMODE_4x4)
				pchLoopFilterFlag[iBlk] = (I8_WMV)pLoopFliterFlag16[pCBPCurr[iBlk]];
        } while ( ++iBlk < 6 );
    }
        
    if (bTop && bLeft) 
    {  // top left
        pchLoopFilterFlag[0] &= 0xf0;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xf0;
        pchLoopFilterFlag[5] &= 0xf0;
    }
    else if (bTop) 
    {  // top row
        pchLoopFilterFlag[0] &= 0xf3;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[4] &= 0xf3;
        pchLoopFilterFlag[5] &= 0xf3;
    }
    else if (bLeft) 
    {  // left col
        pchLoopFilterFlag[0] &= 0xfc;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xfc;
        pchLoopFilterFlag[5] &= 0xfc;
    }
    
    if (pWMVDec->m_tFrmType  == BVOP)
        return;
    
    if (chMBMode != MB_1MV || pWMVDec->m_pMotion[0].I32 != IBLOCKMV)
    {
        I32_WMV iBlkC;
		UMotion_EMB *pMotion  = pWMVDec->m_pMotion;
		UMotion_EMB *pMotionC = pWMVDec->m_pMotionC;

        // block 0 rules
        if (!bTop && (pMotion[iBlk0].I32 != IBLOCKMV) && (pMotion[iBlkUp].I32 == pMotion[iBlk0].I32)) {
            pchLoopFilterFlag[0] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]+8][pCBPCurr[0]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[2]+12][pCBPTop[2]]);
		}
        if (!bLeft && (pMotion[iBlk0].I32 != IBLOCKMV) && (pMotion[iBlk0 - 1].I32 == pMotion[iBlk0].I32)) {
            pchLoopFilterFlag[0] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]][pCBPCurr[0]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[1]+4][pCBPLeft[1]]);
		}

        // block 1 rules
        if (!bTop && (pMotion[iBlk0 + 1].I16[X_INDEX] != IBLOCKMV) && (pMotion[iBlkUp + 1].I32 == pMotion[iBlk0 + 1].I32)) {
            pchLoopFilterFlag[1] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]+8][pCBPCurr[1]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[3]+12][pCBPTop[3]]);
		}
        if ((pMotion[iBlk0 + 1].I32  != IBLOCKMV) && (pMotion[iBlk0].I32 == pMotion[iBlk0 + 1].I32)) {
            pchLoopFilterFlag[1] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]][pCBPCurr[1]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]+4][pCBPCurr[0]]);        
		}
        
        // block 2 rules
        if ((pMotion[iBlk2].I32 != IBLOCKMV) && (pMotion[iBlk0].I32 == pMotion[iBlk2].I32)) {
            pchLoopFilterFlag[2] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[2]+8][pCBPCurr[2]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]+12][pCBPCurr[0]]);
		}
        if (!bLeft && (pMotion[iBlk2].I32 != IBLOCKMV) && (pMotion[iBlk2 - 1].I32 == pMotion[iBlk2].I32)) {
            pchLoopFilterFlag[2] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[2]][pCBPCurr[2]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[3]+4][pCBPLeft[3]]);
		}

        // block 3 rules
        if ((pMotion[iBlk2 + 1].I32 != IBLOCKMV) && (pMotion[iBlk0 + 1].I32 == pMotion[iBlk2 + 1].I32)) {
            pchLoopFilterFlag[3] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[3]+8][pCBPCurr[3]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]+12][pCBPCurr[1]]);
		}
        if ((pMotion[iBlk2 + 1].I32 != IBLOCKMV) && (pMotion[iBlk2].I32== pMotion[iBlk2 + 1].I32)) {
            pchLoopFilterFlag[3] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[3]][pCBPCurr[3]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]+4][pCBPCurr[1]]);
		}
        
        // block 4 & 5 rules
        iBlkC = iMBY * pWMVDec->m_uintNumMBX + iMBX;
        if (!bTop && (pMotionC[iBlkC].I32!= IBLOCKMV) && (pMotionC[iBlkC - pWMVDec->m_uintNumMBX].I32 == pMotionC[iBlkC].I32)) {
            pchLoopFilterFlag[4] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[4]+8][pCBPCurr[4]] | 
                 LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[4]+12][pCBPTop[4]]);

            pchLoopFilterFlag[5] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[5]+8][pCBPCurr[5]] | 
                 LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[5]+12][pCBPTop[5]]);

        }
        if (!bLeft && (pMotionC[iBlkC].I32 != IBLOCKMV) && (pMotionC[iBlkC - 1].I32== pMotionC[iBlkC].I32)) {
            pchLoopFilterFlag[4] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[4]][pCBPCurr[4]] | 
                 LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[4]+4][pCBPLeft[4]]);

            pchLoopFilterFlag[5] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[5]][pCBPCurr[5]] | 
                 LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[5]+4][pCBPLeft[5]]);
        }
    }
}

//#pragma code_seg (EMBSEC_DEF)
Void_WMV ComputeMBLoopFilterFlags_WMVA_EMB (tWMVDecInternalMember *pWMVDec,
    I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
    I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
    U8_WMV* pCBPCurr, U8_WMV* pCBPTop, U8_WMV* pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft)
{
    I32_WMV iBlk0 = iMBY * 4 * pWMVDec->m_uintNumMBX + iMBX * 2;
    I32_WMV iBlk2 = iBlk0 + 2 * pWMVDec->m_uintNumMBX;
    I32_WMV iBlkUp = iBlk0 - 2 * pWMVDec->m_uintNumMBX;
    I8_WMV* pchLoopFilterFlag = pWMVDec->m_pLoopFilterFlags[iMBY*pWMVDec->m_uintNumMBX + iMBX].chFlag;

    I32_WMV iBlk;

    if (!pWMVDec->m_bInterlaceV2 && pWMVDec->m_tFrmType == BVOP) 
    {
        pchLoopFilterFlag[0] = (I8_WMV)0x0f;
        pchLoopFilterFlag[1] = (I8_WMV)0x0f;
        pchLoopFilterFlag[2] = (I8_WMV)0x0f;
        pchLoopFilterFlag[3] = (I8_WMV)0x0f;
        pchLoopFilterFlag[4] = (I8_WMV)0x0f;
        pchLoopFilterFlag[5] = (I8_WMV)0x0f;
    }
    else
    {
		iBlk = 0;
        do {
            if (pBlockXformMode[iBlk] == XFORMMODE_8x8)
                pchLoopFilterFlag[iBlk] = (I8_WMV)0x0f;
            else if (pBlockXformMode[iBlk] == XFORMMODE_8x4)
                pchLoopFilterFlag[iBlk] = (I8_WMV)0xcf;
            else if (pBlockXformMode[iBlk] == XFORMMODE_4x8)
                pchLoopFilterFlag[iBlk] = (I8_WMV)0x3f;
			else if (pBlockXformMode[iBlk] == XFORMMODE_4x4)
				pchLoopFilterFlag[iBlk] = (I8_WMV)MIDDLEFLAGS[pCBPCurr[iBlk]];
		} while ( ++iBlk < 6 );

    }

    if (bTop && bLeft) {  
        // top left
        pchLoopFilterFlag[0] &= 0xf0;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xf0;
        pchLoopFilterFlag[5] &= 0xf0;
    }
    else if (bTop) {  
        pchLoopFilterFlag[0] &= 0xf3;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[4] &= 0xf3;
        pchLoopFilterFlag[5] &= 0xf3;
    }
    else if (bLeft) {  
        pchLoopFilterFlag[pWMVDec->m_iBetaRTMMismatchIndex] &= 0xfc;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xfc;
        pchLoopFilterFlag[5] &= 0xfc;
    }


    if (pWMVDec->m_tFrmType  == BVOP)
        return;

    if (chMBMode != MB_INTRA) {
        I32_WMV iBlkC;
		UMotion_EMB *pMotion  = pWMVDec->m_pMotion;
		UMotion_EMB *pMotionC = pWMVDec->m_pMotionC;

		// block 0 rules
        if (!bTop && (pMotion[iBlk0].I32 != IBLOCKMV) && 
            (pMotion[iBlkUp].I32 == pMotion[iBlk0].I32))
            pchLoopFilterFlag[0] &= 
                (TOPFLAGS[pBlockXformMode[0]][pCBPCurr[0]] | 
                TOPPREDFLAGS[pBlockXformModeTop[2]][pCBPTop[2]]);

        if (!bLeft && (pMotion[iBlk0].I32 != IBLOCKMV) && 
            (pMotion[iBlk0 - 1].I32 == pMotion[iBlk0].I32))
            pchLoopFilterFlag[0] &= 
                (LEFTFLAGS[pBlockXformMode[0]][pCBPCurr[0]] | 
                LEFTPREDFLAGS[pBlockXformModeLeft[1]][pCBPLeft[1]]);
              
        // block 1 rules
        if (!bTop && (pMotion[iBlk0 + 1].I32 != IBLOCKMV) && 
            (pMotion[iBlkUp + 1].I32 == pMotion[iBlk0 + 1].I32))
            pchLoopFilterFlag[1] &= 
                (TOPFLAGS[pBlockXformMode[1]][pCBPCurr[1]] | 
                TOPPREDFLAGS[pBlockXformModeTop[3]][pCBPTop[3]]);

        if ((pMotion[iBlk0 + 1].I32 != IBLOCKMV) && 
            (pMotion[iBlk0].I32 == pMotion[iBlk0 + 1].I32))
            pchLoopFilterFlag[1] &= 
                (LEFTFLAGS[pBlockXformMode[1]][pCBPCurr[1]] | 
                LEFTPREDFLAGS[pBlockXformMode[0]][pCBPCurr[0]]);        

        // block 2 rules
        if ((pMotion[iBlk2].I32 != IBLOCKMV) && 
            (pMotion[iBlk0].I32 == pMotion[iBlk2].I32))
            pchLoopFilterFlag[2] &= 
                (TOPFLAGS[pBlockXformMode[2]][pCBPCurr[2]] | 
                TOPPREDFLAGS[pBlockXformMode[0]][pCBPCurr[0]]);

        if (!bLeft && (pMotion[iBlk2].I32 != IBLOCKMV) && 
            (pMotion[iBlk2 - 1].I32 == pMotion[iBlk2].I32))
            pchLoopFilterFlag[2] &= 
                (LEFTFLAGS[pBlockXformMode[2]][pCBPCurr[2]] | 
                LEFTPREDFLAGS[pBlockXformModeLeft[3]][pCBPLeft[3]]);

        // block 3 rules
        if ((pMotion[iBlk2 + 1].I32 != IBLOCKMV) && 
            (pMotion[iBlk0 + 1].I32 == pMotion[iBlk2 + 1].I32))
            pchLoopFilterFlag[3] &= 
                (TOPFLAGS[pBlockXformMode[3]][pCBPCurr[3]] | 
                TOPPREDFLAGS[pBlockXformMode[1]][pCBPCurr[1]]);

        if ((pMotion[iBlk2 + 1].I32 != IBLOCKMV) && 
            (pMotion[iBlk2].I32 == pMotion[iBlk2 + 1].I32))
            pchLoopFilterFlag[3] &= 
                (LEFTFLAGS[pBlockXformMode[3]][pCBPCurr[3]] | 
                LEFTPREDFLAGS[pBlockXformMode[2]][pCBPCurr[2]]);

        // block 4 & 5 rules
        iBlkC = iMBY * pWMVDec->m_uintNumMBX + iMBX;
        if (!bTop && (pMotionC[iBlkC].I32 != IBLOCKMV) && 
            (pMotionC[iBlkC - pWMVDec->m_uintNumMBX].I32 == pMotionC[iBlkC].I32)) {
            pchLoopFilterFlag[4] &= 
                (TOPFLAGS[pBlockXformMode[4]][pCBPCurr[4]] | 
                TOPPREDFLAGS[pBlockXformModeTop[4]][pCBPTop[4]]);
            pchLoopFilterFlag[5] &= 
                (TOPFLAGS[pBlockXformMode[5]][pCBPCurr[5]] | 
                TOPPREDFLAGS[pBlockXformModeTop[5]][pCBPTop[5]]);
        }

        if (!bLeft && (pMotionC[iBlkC].I32 != IBLOCKMV) && 
            (pMotionC[iBlkC - 1].I32 == pMotionC[iBlkC].I32)) {
            pchLoopFilterFlag[4] &= 
                (LEFTFLAGS[pBlockXformMode[4]][pCBPCurr[4]] | 
                LEFTPREDFLAGS[pBlockXformModeLeft[4]][pCBPLeft[4]]);
            pchLoopFilterFlag[5] &= 
                (LEFTFLAGS[pBlockXformMode[5]][pCBPCurr[5]] | 
                LEFTPREDFLAGS[pBlockXformModeLeft[5]][pCBPLeft[5]]);
        }

    }
}

//#pragma code_seg (EMBSEC_PML2)
Void_WMV OverlapMBRow_Slice_EMB (tWMVDecInternalMember * pWMVDec, I32_WMV imbY, U8_WMV *ppxliRecnY, U8_WMV *ppxliRecnU,
                           U8_WMV *ppxliRecnV, Bool_WMV bTopRowOfSlice, Bool_WMV bBottomRowOfSlice,U32_WMV threadflag)
{
    U32_WMV  ii ,iXBlocks = pWMVDec->m_uintNumMBX << 1;
    I32_WMV  iStrideUV = (I32_WMV) pWMVDec->m_uintNumMBX << 3;
    I32_WMV  iStride = iStrideUV << 1;
    I32_WMV  iBlockIndex ;
    UMotion_EMB  *pMotion ;
    UMotion_EMB  *pMotionUV ;
    I16_WMV     **pIntraBlockRow, **pIntraMBRowU, **pIntraMBRowV;

    EMB_PMainLoop  * pPMainLoop = &pWMVDec->m_EMB_PMainLoop;
    EMB_PBMainLoop *pMainPB = &pPMainLoop->PB[0];
    EMB_PBMainLoop *pThreadPB = &pPMainLoop->PB[1];

    EMB_PBMainLoop *pPB =&pPMainLoop->PB[threadflag];
    //EMB_PBMainLoop *pPBOther =&pPMainLoop->PB[threadflag^1];

    imbY += (bBottomRowOfSlice != FALSE);    

    if(bBottomRowOfSlice != FALSE)
        bBottomRowOfSlice = bBottomRowOfSlice ;

    iBlockIndex = imbY * iXBlocks * 2;
    pMotion = pWMVDec->m_pMotion + iBlockIndex;
    pMotionUV = pWMVDec->m_pMotionC + imbY * (I32_WMV) pWMVDec->m_uintNumMBX;
 
    pIntraBlockRow = pPB->pThreadIntraBlockRow0;
    pIntraMBRowU = pPB->pThreadIntraMBRowU0;
    pIntraMBRowV = pPB->pThreadIntraMBRowV0; 
    
    // filter vertical edges
    if (!bBottomRowOfSlice)
    {
        for (ii = 1; ii <(I32_WMV) pWMVDec->m_uintNumMBX; ii++)
        {
            if (pMotion[2*ii-1].I16[X_INDEX] == IBLOCKMV && pMotion[(2*ii-1) - 1].I16[X_INDEX] == IBLOCKMV) {
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraBlockRow[0] + ( (2*ii-1) << 3), iStride);
            }
            if (pMotion[2*ii-1 + iXBlocks].I16[X_INDEX] == IBLOCKMV && pMotion[2*ii-1 + iXBlocks - 1].I16[X_INDEX] == IBLOCKMV) {
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraBlockRow[2] + ((2*ii-1) << 3), iStride);
            }

            if(2*ii < iXBlocks )
            {
                if (pMotion[2*ii].I16[X_INDEX] == IBLOCKMV && pMotion[2*ii - 1].I16[X_INDEX] == IBLOCKMV) {
                    pWMVDec->m_pOverlapBlockVerticalEdge (pIntraBlockRow[0] + ( (2*ii) << 3), iStride);
                }
                if (pMotion[2*ii + iXBlocks].I16[X_INDEX] == IBLOCKMV && pMotion[2*ii + iXBlocks - 1].I16[X_INDEX] == IBLOCKMV) {
                    pWMVDec->m_pOverlapBlockVerticalEdge (pIntraBlockRow[2] + ( (2*ii) << 3), iStride);
                }   
            }

            if(ii < pWMVDec->m_uintNumMBX )
            {
                if (pMotionUV[ii].I16[X_INDEX] == IBLOCKMV && pMotionUV[ii - 1].I16[X_INDEX] == IBLOCKMV) {
                    pWMVDec->m_pOverlapBlockVerticalEdge (pIntraMBRowU[0] + (ii << 3), iStrideUV);
                    pWMVDec->m_pOverlapBlockVerticalEdge (pIntraMBRowV[0] + (ii << 3), iStrideUV);
                }
            }
        }

        {
            if (pMotion[2*ii-1].I16[X_INDEX] == IBLOCKMV && pMotion[(2*ii-1) - 1].I16[X_INDEX] == IBLOCKMV) {
                    pWMVDec->m_pOverlapBlockVerticalEdge (pIntraBlockRow[0] + ( (2*ii-1) << 3), iStride);
                }
                if (pMotion[2*ii-1 + iXBlocks].I16[X_INDEX] == IBLOCKMV && pMotion[2*ii-1 + iXBlocks - 1].I16[X_INDEX] == IBLOCKMV) {
                    pWMVDec->m_pOverlapBlockVerticalEdge (pIntraBlockRow[2] + ((2*ii-1) << 3), iStride);
                }
        }
    }


    //filter horizontal edges
    //for (ii = 0; ii < (I32_WMV) pWMVDec->m_uintNumMBX +1; ii++)
    for (ii = 0; ii < (I32_WMV) pWMVDec->m_uintNumMBX ; ii++)
    {
        Bool_WMV  bTop = (imbY==0) ?0: (!bTopRowOfSlice) && pMotion[(2*ii-1) - iXBlocks].I16[X_INDEX] == IBLOCKMV;
        Bool_WMV  bDn  = (!bBottomRowOfSlice);
        Bool_WMV  bUp = FALSE;

        if((I32_WMV)(2*ii-1)>=0)
        {
            if (bDn) {
                bUp = pMotion[(2*ii-1)].I16[X_INDEX] == IBLOCKMV;
                bDn = pMotion[(2*ii-1) + iXBlocks].I16[X_INDEX] == IBLOCKMV;
            }        
            if  (bTop || bUp) {
                pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[1] + ((2*ii-1) << 3), pIntraBlockRow[0] + ((2*ii-1) << 3),
                    iStride, ppxliRecnY + ((2*ii-1) << 3), pWMVDec->m_iWidthPrevY, bTop, bUp, FALSE);
            }        
            if  (bUp || bDn) {
                pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[0] + ((2*ii-1) << 3), pIntraBlockRow[2] + ((2*ii-1) << 3),
                    iStride, ppxliRecnY + ((pWMVDec->m_iWidthPrevY + (2*ii-1)) << 3), pWMVDec->m_iWidthPrevY, bUp, bDn, FALSE);
            }
        }
        /**/
         if(2*ii < iXBlocks )
         {
            Bool_WMV  bTop = (imbY==0) ?0:(!bTopRowOfSlice) && pMotion[(2*ii) - iXBlocks].I16[X_INDEX] == IBLOCKMV;
            Bool_WMV  bDn  = (!bBottomRowOfSlice), bUp = FALSE;
            if (bDn) {
                bUp = pMotion[(2*ii)].I16[X_INDEX] == IBLOCKMV;
                bDn = pMotion[(2*ii) + iXBlocks].I16[X_INDEX] == IBLOCKMV;
            }
            
            if  (bTop || bUp) {
                pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[1] + ((2*ii) << 3), pIntraBlockRow[0] + ((2*ii) << 3),
                    iStride, ppxliRecnY + ((2*ii) << 3), pWMVDec->m_iWidthPrevY, bTop, bUp, FALSE);
            }
            
            if  (bUp || bDn) {
                pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[0] + ((2*ii) << 3), pIntraBlockRow[2] + ((2*ii) << 3),
                    iStride, ppxliRecnY + ((pWMVDec->m_iWidthPrevY + (2*ii)) << 3), pWMVDec->m_iWidthPrevY, bUp, bDn, FALSE);
            }
         }

        /*U V*/
        if(ii < pWMVDec->m_uintNumMBX )
        {
            Bool_WMV  bUp = (imbY==0) ?0:(!bTopRowOfSlice) && (pMotionUV[ii - (I32_WMV) pWMVDec->m_uintNumMBX].I16[X_INDEX] == IBLOCKMV);
            Bool_WMV  bDn = (!bBottomRowOfSlice) && (pMotionUV[ii].I16[X_INDEX] == IBLOCKMV);

            
            if  (bUp || bDn) {
                pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraMBRowU[1] + (ii << 3), pIntraMBRowU[0] + (ii << 3),
                    iStrideUV, ppxliRecnU + (ii << 3), pWMVDec->m_iWidthPrevUV, bUp, bDn, FALSE);
                pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraMBRowV[1] + (ii << 3), pIntraMBRowV[0] + (ii << 3),
                    iStrideUV, ppxliRecnV + (ii << 3), pWMVDec->m_iWidthPrevUV, bUp, bDn, FALSE);
            }
        }
    }   

     ii= pWMVDec->m_uintNumMBX;
    {
        Bool_WMV  bTop = (imbY==0) ?0:(!bTopRowOfSlice) && pMotion[(2*ii-1) - iXBlocks].I16[X_INDEX] == IBLOCKMV;
        Bool_WMV  bDn  = (!bBottomRowOfSlice), bUp = FALSE;  

        if (bDn) {
            bUp = pMotion[(2*ii-1)].I16[X_INDEX] == IBLOCKMV;
            bDn = pMotion[(2*ii-1) + iXBlocks].I16[X_INDEX] == IBLOCKMV;
        }        
        if  (bTop || bUp) {
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[1] + ((2*ii-1) << 3), pIntraBlockRow[0] + ((2*ii-1) << 3),
                iStride, ppxliRecnY + ((2*ii-1) << 3), pWMVDec->m_iWidthPrevY, bTop, bUp, FALSE);
        }        
        if  (bUp || bDn) {
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[0] + ((2*ii-1) << 3), pIntraBlockRow[2] + ((2*ii-1) << 3),
                iStride, ppxliRecnY + ((pWMVDec->m_iWidthPrevY + (2*ii-1)) << 3), pWMVDec->m_iWidthPrevY, bUp, bDn, FALSE);
        }
    }

   // pPB->prev_overlap_mb_h =  pWMVDec->m_uintNumMBX +1;

    {
        I16_WMV * pMotion1;
        pMotion1 = pIntraBlockRow[1];
        pIntraBlockRow[1] = pIntraBlockRow[2];
        pIntraBlockRow[2] = pMotion1;
        
        pMotion1 = pIntraMBRowU[0];
        pIntraMBRowU[0] = pIntraMBRowU[1];
        pIntraMBRowU[1] = pMotion1;
        
        pMotion1 = pIntraMBRowV[0];
        pIntraMBRowV[0] = pIntraMBRowV[1];
        pIntraMBRowV[1] = pMotion1;

        pPB->m_pIntraBlockRow[0] = pPB->m_pIntraBlockRow[1] = pPB->pThreadIntraBlockRow0[0];
        pPB->m_pIntraBlockRow[2] = pPB->m_pIntraBlockRow[3] = pPB->pThreadIntraBlockRow0[2];
        pPB->m_pIntraBlockRow[4] = pPB->pThreadIntraMBRowU0[0];
        pPB->m_pIntraBlockRow[5] = pPB->pThreadIntraMBRowV0[0];

        //if(threadflag)
        //{
        //    memcpy(pMainPB->pThreadIntraBlockRow0[1], pPB->pThreadIntraBlockRow0[1],pWMVDec->m_uintNumMBX * 16 * 8*sizeof(I16_WMV));
        //    memcpy(pMainPB->pThreadIntraMBRowU0[1], pPB->pThreadIntraMBRowU0[1],pWMVDec->m_uintNumMBX * 8 * 8*sizeof(I16_WMV));
        //    memcpy(pMainPB->pThreadIntraMBRowV0[1], pPB->pThreadIntraMBRowV0[1],pWMVDec->m_uintNumMBX * 8 * 8*sizeof(I16_WMV));
        //}
        //else
        //{
        //    memcpy(pThreadPB->pThreadIntraBlockRow0[1], pPB->pThreadIntraBlockRow0[1],pWMVDec->m_uintNumMBX * 16 * 8*sizeof(I16_WMV));
        //    memcpy(pThreadPB->pThreadIntraMBRowU0[1], pPB->pThreadIntraMBRowU0[1],pWMVDec->m_uintNumMBX * 8 * 8*sizeof(I16_WMV));
        //    memcpy(pThreadPB->pThreadIntraMBRowV0[1], pPB->pThreadIntraMBRowV0[1],pWMVDec->m_uintNumMBX * 8 * 8*sizeof(I16_WMV));
        //}

    }
}