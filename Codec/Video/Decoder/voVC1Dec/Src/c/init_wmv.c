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


// Uncomment to force the malloc function to fail after being called some number 
// of times.  For more information, look in the prvMalloc function below.
//#define WMV_INCREMENTAL_MALLOC_FAILURE

#ifdef WMV_INCREMENTAL_MALLOC_FAILURE
static int gNum_mallocs=0;
#endif

Void_WMV computePars_WMVA (tWMVDecInternalMember *pWMVDec);
//Void_WMV FreeVlcTables(tWMVDecInternalMember *pWMVDec);//sw:20080108

tWMVDecodeStatus setVlcTableInfo(tWMVDecInternalMember *pWMVDec);
//tWMVDecodeStatus setVlcTableInfo_WMVA(tWMVDecInternalMember *pWMVDec);
//tWMVDecodeStatus InitDataMembers_WMVA (tWMVDecInternalMember *pWMVDec, 
//    I32_WMV iWidthSource,
//    I32_WMV iHeightSource);

#define UVTCOEF_ESCAPE_HghMt 168                            
#define NUMOFCODESINNOTLASTINTERMinus1_HghMt 98
#define TCOEF_ESCAPEINTRAY_HghMt 185                            
#define NUMOFCODESINNOTLASTINTRAMinus1_HghMt 118

#define UVTCOEF_ESCAPE_Talking 148                          
#define NUMOFCODESINNOTLASTINTERMinus1_Talking 80
#define TCOEF_ESCAPEINTRAY_Talking 132                          
#define NUMOFCODESINNOTLASTINTRAMinus1_Talking 84

#define UVTCOEF_ESCAPE_MPEG4 102                            
#define NUMOFCODESINNOTLASTINTERMinus1_MPEG4 57
#define TCOEF_ESCAPEINTRAY_MPEG4 102
#define NUMOFCODESINNOTLASTINTRAMinus1_MPEG4 66

#define UVTCOEF_ESCAPE_HghRate 174                            
#define NUMOFCODESINNOTLASTINTERMinus1_HghRate 108

#define TCOEF_ESCAPEINTRAY_HghRate 162
#define NUMOFCODESINNOTLASTINTRAMinus1_HghRate 125

#define setTable(                                                               \
    vlcTableInfo,                                                           \
    hufDec_DCTAC,                                                           \
    iTCOEF_ESCAPE,                                                          \
    iNUMOFCODESINNOTLASTMinus1,                                             \
    igrgLevelAtIndx_HghMt,                                                  \
    igrgRunAtIndx_HghMt,                                                    \
    igrgIfNotLastNumOfLevelAtRun,                                           \
    igrgIfLastNumOfLevelAtRun,                                              \
    igrgIfNotLastNumOfRunAtLevel,                                           \
    igrgIfLastNumOfRunAtLevel                                               \
    )                                                                       \
    vlcTableInfo.hufDCTACDec = hufDec_DCTAC;                                \
    vlcTableInfo.iTcoef_ESCAPE = iTCOEF_ESCAPE;                             \
    vlcTableInfo.iStartIndxOfLastRunMinus1 = iNUMOFCODESINNOTLASTMinus1;    \
    vlcTableInfo.pcLevelAtIndx = igrgLevelAtIndx_HghMt;                     \
    vlcTableInfo.puiRunAtIndx = igrgRunAtIndx_HghMt;                        \
    vlcTableInfo.puiNotLastNumOfLevelAtRun = igrgIfNotLastNumOfLevelAtRun;  \
    vlcTableInfo.puiLastNumOfLevelAtRun = igrgIfLastNumOfLevelAtRun;        \
    vlcTableInfo.puiNotLastNumOfRunAtLevel = igrgIfNotLastNumOfRunAtLevel;  \
    vlcTableInfo.puiLastNumOfRunAtLevel = igrgIfLastNumOfRunAtLevel;        \
    vlcTableInfo.combined_levelrun=NULL_WMV;                                \

tWMVDecodeStatus CombinedLevelRunInit(tWMVDecInternalMember *pWMVDec, U8_WMV * pRun, I8_WMV * pLevel, I32_WMV buffer_size, 
                                      CDCTTableInfo_Dec * pInterDCTTableInfo_Dec) 
{                                                                      

    I16_WMV * pCombLevelRun;
    I32_WMV i;

    buffer_size ++;

    pCombLevelRun = pInterDCTTableInfo_Dec->combined_levelrun=(I16_WMV *)wmvMalloc(pWMVDec, 2*buffer_size, DHEAP_HUFFMAN);
    if (pCombLevelRun ==  NULL) {
        return WMV_BadMemory;
    }

    for(i=0;i<buffer_size-1;i++)
    {
        *pCombLevelRun++=((*pLevel)<<8)|(*pRun);
        pLevel++;
        pRun++;
    }
    
    *pCombLevelRun = 0;

    return WMV_Succeeded;
}


void CombinedLevelRunFree(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec * pInterDCTTableInfo_Dec, I32_WMV buffer_size) {
    if ( pInterDCTTableInfo_Dec->combined_levelrun != NULL ) {
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO combined_levelrun  :%x",(U32_WMV)pInterDCTTableInfo_Dec->combined_levelrun );
#endif
        wmvFree(pWMVDec, pInterDCTTableInfo_Dec->combined_levelrun);
        pInterDCTTableInfo_Dec->combined_levelrun = NULL;
    }
}


#define USE_NEW_MMX_IDCT


#define UVTCOEF_ESCAPE_HghMt 168                            
#define NUMOFCODESINNOTLASTINTERMinus1_HghMt 98
#define TCOEF_ESCAPEINTRAY_HghMt 185                            
#define NUMOFCODESINNOTLASTINTRAMinus1_HghMt 118

#define UVTCOEF_ESCAPE_Talking 148                          
#define NUMOFCODESINNOTLASTINTERMinus1_Talking 80
#define TCOEF_ESCAPEINTRAY_Talking 132                          
#define NUMOFCODESINNOTLASTINTRAMinus1_Talking 84

#define UVTCOEF_ESCAPE_MPEG4 102                            
#define NUMOFCODESINNOTLASTINTERMinus1_MPEG4 57
#define TCOEF_ESCAPEINTRAY_MPEG4 102
#define NUMOFCODESINNOTLASTINTRAMinus1_MPEG4 66

#define UVTCOEF_ESCAPE_HghRate 174                            
#define NUMOFCODESINNOTLASTINTERMinus1_HghRate 108

#define TCOEF_ESCAPEINTRAY_HghRate 162
#define NUMOFCODESINNOTLASTINTRAMinus1_HghRate 125

tWMVDecodeStatus setVlcTableInfo(tWMVDecInternalMember *pWMVDec)
{

    tWMVDecodeStatus status=WMV_Succeeded;

    /// Set tables for High Motion
    setTable(
        pWMVDec->InterDCTTableInfo_Dec_HghMt,
        &pWMVDec->m_hufDCTACInterDec_HghMt,
        UVTCOEF_ESCAPE_HghMt,
        NUMOFCODESINNOTLASTINTERMinus1_HghMt,
        sm_rgLevelAtIndx_HghMt, 
        sm_rgRunAtIndx_HghMt, 
        sm_rgIfNotLastNumOfLevelAtRun_HghMt,
        sm_rgIfLastNumOfLevelAtRun_HghMt,
        sm_rgIfNotLastNumOfRunAtLevel_HghMt,
        sm_rgIfLastNumOfRunAtLevel_HghMt
        );


    status=CombinedLevelRunInit(pWMVDec, sm_rgRunAtIndx_HghMt, sm_rgLevelAtIndx_HghMt, 
        sizeof(sm_rgLevelAtIndx_HghMt), 
        &pWMVDec->InterDCTTableInfo_Dec_HghMt);
    if(status!=WMV_Succeeded) return status;


    setTable(
        pWMVDec->IntraDCTTableInfo_Dec_HghMt,
        &pWMVDec->m_hufDCTACIntraDec_HghMt,
        TCOEF_ESCAPEINTRAY_HghMt,
        NUMOFCODESINNOTLASTINTRAMinus1_HghMt,
        sm_rgLevelAtIndxOfIntraY_HghMt, 
        sm_rgRunAtIndxOfIntraY_HghMt, 
        sm_rgIfNotLastNumOfLevelAtRunIntraY_HghMt,
        sm_rgIfLastNumOfLevelAtRunIntraY_HghMt,
        sm_rgIfNotLastNumOfRunAtLevelIntraY_HghMt,
        sm_rgIfLastNumOfRunAtLevelIntraY_HghMt
        );

    status=CombinedLevelRunInit(pWMVDec, sm_rgRunAtIndxOfIntraY_HghMt, sm_rgLevelAtIndxOfIntraY_HghMt, 
        sizeof(sm_rgLevelAtIndxOfIntraY_HghMt), 
        &pWMVDec->IntraDCTTableInfo_Dec_HghMt);
    if(status!=WMV_Succeeded) return status;

    /// Set tables for Talking head
    setTable(
        pWMVDec->InterDCTTableInfo_Dec_Talking,
        &pWMVDec->m_hufDCTACInterDec_Talking,
        UVTCOEF_ESCAPE_Talking,
        NUMOFCODESINNOTLASTINTERMinus1_Talking,
        sm_rgLevelAtIndx_Talking, 
        sm_rgRunAtIndx_Talking, 
        sm_rgIfNotLastNumOfLevelAtRun_Talking,
        sm_rgIfLastNumOfLevelAtRun_Talking,
        sm_rgIfNotLastNumOfRunAtLevel_Talking,
        sm_rgIfLastNumOfRunAtLevel_Talking
        );

    CombinedLevelRunInit(pWMVDec, sm_rgRunAtIndx_Talking, sm_rgLevelAtIndx_Talking, 
        sizeof(sm_rgLevelAtIndx_Talking), 
        &pWMVDec->InterDCTTableInfo_Dec_Talking);
    if(status!=WMV_Succeeded) return status;


    setTable(
        pWMVDec->IntraDCTTableInfo_Dec_Talking,
        &pWMVDec->m_hufDCTACIntraDec_Talking,
        TCOEF_ESCAPEINTRAY_Talking,
        NUMOFCODESINNOTLASTINTRAMinus1_Talking,
        sm_rgLevelAtIndxOfIntraY_Talking, 
        sm_rgRunAtIndxOfIntraY_Talking, 
        sm_rgIfNotLastNumOfLevelAtRunIntraY_Talking,
        sm_rgIfLastNumOfLevelAtRunIntraY_Talking,
        sm_rgIfNotLastNumOfRunAtLevelIntraY_Talking,
        sm_rgIfLastNumOfRunAtLevelIntraY_Talking
        );


    status=CombinedLevelRunInit(pWMVDec, sm_rgRunAtIndxOfIntraY_Talking, sm_rgLevelAtIndxOfIntraY_Talking, 
        sizeof(sm_rgLevelAtIndxOfIntraY_Talking), 
        &pWMVDec->IntraDCTTableInfo_Dec_Talking);
    if(status!=WMV_Succeeded) return status;

    /// Set tables for MPEG4
    setTable(
        pWMVDec->InterDCTTableInfo_Dec_MPEG4,
        &pWMVDec->m_hufDCTACInterDec_MPEG4,
        UVTCOEF_ESCAPE_MPEG4,
        NUMOFCODESINNOTLASTINTERMinus1_MPEG4,
        sm_rgLevelAtIndx_MPEG4, 
        sm_rgRunAtIndx_MPEG4, 
        sm_rgIfNotLastNumOfLevelAtRun_MPEG4,
        sm_rgIfLastNumOfLevelAtRun_MPEG4,
        sm_rgIfNotLastNumOfRunAtLevel_MPEG4,
        sm_rgIfLastNumOfRunAtLevel_MPEG4
        );


    CombinedLevelRunInit(pWMVDec, sm_rgRunAtIndx_MPEG4, sm_rgLevelAtIndx_MPEG4, 
        sizeof(sm_rgLevelAtIndx_MPEG4), 
        &pWMVDec->InterDCTTableInfo_Dec_MPEG4);
    if(status!=WMV_Succeeded) return status;


    setTable(
        pWMVDec->IntraDCTTableInfo_Dec_MPEG4,
        &pWMVDec->m_hufDCTACIntraDec_MPEG4,
        TCOEF_ESCAPEINTRAY_MPEG4,
        NUMOFCODESINNOTLASTINTRAMinus1_MPEG4,
        sm_rgLevelAtIndxOfIntraY_MPEG4, 
        sm_rgRunAtIndxOfIntraY_MPEG4, 
        sm_rgIfNotLastNumOfLevelAtRunIntraY_MPEG4,
        sm_rgIfLastNumOfLevelAtRunIntraY_MPEG4,
        sm_rgIfNotLastNumOfRunAtLevelIntraY_MPEG4,
        sm_rgIfLastNumOfRunAtLevelIntraY_MPEG4
        );

    CombinedLevelRunInit(pWMVDec, sm_rgRunAtIndxOfIntraY_MPEG4, sm_rgLevelAtIndxOfIntraY_MPEG4, 
        sizeof(sm_rgLevelAtIndxOfIntraY_MPEG4), 
        &pWMVDec->IntraDCTTableInfo_Dec_MPEG4);
    if(status!=WMV_Succeeded) return status;

    /// Set tables for High Rate
    setTable(
        pWMVDec->InterDCTTableInfo_Dec_HghRate,
        &pWMVDec->m_hufDCTACInterDec_HghRate,
        UVTCOEF_ESCAPE_HghRate,
        NUMOFCODESINNOTLASTINTERMinus1_HghRate,
        //STARTINDEXOFLASTRUN_MPEG4, 
        sm_rgLevelAtIndx_HghRate, 
        sm_rgRunAtIndx_HghRate, 
        sm_rgIfNotLastNumOfLevelAtRun_HghRate,
        sm_rgIfLastNumOfLevelAtRun_HghRate,
        sm_rgIfNotLastNumOfRunAtLevel_HghRate,
        sm_rgIfLastNumOfRunAtLevel_HghRate
        );

    CombinedLevelRunInit(pWMVDec, sm_rgRunAtIndx_HghRate, sm_rgLevelAtIndx_HghRate, 
        sizeof(sm_rgLevelAtIndx_HghRate), 
        &pWMVDec->InterDCTTableInfo_Dec_HghRate);
    if(status!=WMV_Succeeded) return status;

    setTable(
        pWMVDec->IntraDCTTableInfo_Dec_HghRate,
        &pWMVDec->m_hufDCTACIntraDec_HghRate,
        TCOEF_ESCAPEINTRAY_HghRate,
        NUMOFCODESINNOTLASTINTRAMinus1_HghRate,
        sm_rgLevelAtIndxOfIntraY_HghRate, 
        sm_rgRunAtIndxOfIntraY_HghRate, 
        sm_rgIfNotLastNumOfLevelAtRunIntraY_HghRate,
        sm_rgIfLastNumOfLevelAtRunIntraY_HghRate,
        sm_rgIfNotLastNumOfRunAtLevelIntraY_HghRate,
        sm_rgIfLastNumOfRunAtLevelIntraY_HghRate
        );


    CombinedLevelRunInit(pWMVDec, sm_rgRunAtIndxOfIntraY_HghRate, sm_rgLevelAtIndxOfIntraY_HghRate, 
        sizeof(sm_rgLevelAtIndxOfIntraY_HghRate), 
        &pWMVDec->IntraDCTTableInfo_Dec_HghRate);
    if(status!=WMV_Succeeded) return status;


    pWMVDec->m_pInterDCTTableInfo_Dec_Set[0] = &pWMVDec->InterDCTTableInfo_Dec_Talking;
    pWMVDec->m_pInterDCTTableInfo_Dec_Set[1] = &pWMVDec->InterDCTTableInfo_Dec_HghMt;
    pWMVDec->m_pInterDCTTableInfo_Dec_Set[2] = &pWMVDec->InterDCTTableInfo_Dec_MPEG4;

    pWMVDec->m_pIntraDCTTableInfo_Dec_Set[0] = &pWMVDec->IntraDCTTableInfo_Dec_Talking;
    pWMVDec->m_pIntraDCTTableInfo_Dec_Set[1] = &pWMVDec->IntraDCTTableInfo_Dec_HghMt;
    pWMVDec->m_pIntraDCTTableInfo_Dec_Set[2] = &pWMVDec->IntraDCTTableInfo_Dec_MPEG4;

    pWMVDec->m_pHufDCTDCDec_Set[0] = &pWMVDec->m_hufDCTDCyDec_Talking;
    pWMVDec->m_pHufDCTDCDec_Set[1] = &pWMVDec->m_hufDCTDCcDec_Talking;
    pWMVDec->m_pHufDCTDCDec_Set[2] = &pWMVDec->m_hufDCTDCyDec_HghMt;
    pWMVDec->m_pHufDCTDCDec_Set[3] = &pWMVDec->m_hufDCTDCcDec_HghMt;
    return status;

}
#if 0
Void_WMV freeFrame(tYUV420Frame_WMV* pFrm, tWMVDecInternalMember * pWMVDec)
{
	if (pFrm) {
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO freeFrame  :%x",(U32_WMV)pFrm->m_pCurrPlaneBase );
#endif
		FREE_PTR(pWMVDec, pFrm->m_pCurrPlaneBase);
		//FREE_PTR(pWMVDec, pFrm);
	}
}
#endif


#   define DEBUG_HEAP_SUB_FRAME(a)


tWMVDecodeStatus  VodecDestruct (tWMVDecInternalMember *pWMVDec)
{
	if(pWMVDec->m_pBMPINFOHEADER){
        wmvFree (pWMVDec, pWMVDec->m_pBMPINFOHEADER);
		pWMVDec->m_pBMPINFOHEADER = NULL;
	}

	if (pWMVDec ->m_pInputBitstream){
       FREE_PTR(pWMVDec, pWMVDec ->m_pInputBitstream);
	}

#if 0
    FREE_PTR (pWMVDec, pWMVDec->m_rgErrorBlock);

    if (pWMVDec->m_cvCodecVersion >= WMV3)
    {
        FREE_PTR(pWMVDec, pWMVDec->m_ppxliFieldMBBuffer);
    }
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_pbStartOfSliceRowOrig);

    if (pWMVDec->m_cvCodecVersion == WMVA) 
	{
        FREE_PTR (pWMVDec, pWMVDec->m_ppxliIntraRowY);
		//FREE_PTR (pWMVDec, pWMVDec->m_ppxliIntraRowU);
		//FREE_PTR (pWMVDec, pWMVDec->m_ppxliIntraRowV);
    }
#if 0
    FREE_PTR (pWMVDec, pWMVDec->m_rgMotionCompBuffer_EMB);
#endif

#if 0
	if(pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP) {
		int i;
		pWMVDec->m_videoMemShare.Uninit(VO_INDEX_DEC_WMV);
		for(i = 0; i < pWMVDec->m_frameBufNum; i++) {
			tYUV420Frame_WMV *pTmpBuff = pWMVDec->m_memManageFIFO.img_seq_backup[i];
			if(pTmpBuff) {
				FREE_PTR(pWMVDec, pTmpBuff);
				pWMVDec->m_memManageFIFO.img_seq_backup[i] = NULL;
			}
		}
	}
	else
#endif
	{
		FREE_PTR (pWMVDec, pWMVDec->m_framebase);
		FREE_PTR (pWMVDec, pWMVDec->m_rgmv1_EMB_base);
#if 0
		freeFrame (pWMVDec->m_pfrmCurrQ, pWMVDec);
		freeFrame (pWMVDec->m_pfrmRef0Q, pWMVDec);
		freeFrame (pWMVDec->m_pfrmMultiresQ, pWMVDec);
		freeFrame (pWMVDec->m_pfrmRef1Q, pWMVDec);
		freeFrame (pWMVDec->m_pfrmPostQ, pWMVDec);
		freeFrame (pWMVDec->m_pfrmPostPrevQ, pWMVDec);

		FREE_PTR(pWMVDec, pWMVDec->m_pfrmCurrQ);
		FREE_PTR(pWMVDec, pWMVDec->m_pfrmRef0Q);
		FREE_PTR(pWMVDec, pWMVDec->m_pfrmRef1Q);		
		FREE_PTR(pWMVDec, pWMVDec->m_pfrmMultiresQ);
		FREE_PTR(pWMVDec, pWMVDec->m_pfrmPostQ);		
		FREE_PTR(pWMVDec, pWMVDec->m_pfrmPostPrevQ);

		if (pWMVDec->m_bYUV411) {
			freeFrame (pWMVDec->m_pfrmYUV411toYUV420Q, pWMVDec);
			FREE_PTR (pWMVDec, pWMVDec->m_pfrmYUV411toYUV420Q);
		}
#endif
	}
#if 0
	FREE_PTR (pWMVDec, pWMVDec->m_pLoopFilterFlags);
#endif

#if 0
    if (pWMVDec->m_cvCodecVersion >= WMV2) {
        t_AltTablesDecoderDestruct (pWMVDec, pWMVDec->m_pAltTables);
        pWMVDec->m_pAltTables = NULL_WMV; 
    }
#endif

	FREE_PTR(pWMVDec,pWMVDec->huffmanbuffer);

///////////////
    if (pWMVDec->m_cvCodecVersion == WMVA) {
        FREE_PTR (pWMVDec, pWMVDec->m_pParseStartCodeBuffer);
    }

//////////////


    CloseThreads(pWMVDec);

    CombinedLevelRunFree(pWMVDec, &pWMVDec->IntraDCTTableInfo_Dec_HghRate, sizeof(sm_rgLevelAtIndxOfIntraY_HghRate));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->InterDCTTableInfo_Dec_HghRate, sizeof(sm_rgLevelAtIndx_HghRate));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->IntraDCTTableInfo_Dec_MPEG4, sizeof(sm_rgLevelAtIndxOfIntraY_MPEG4));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->InterDCTTableInfo_Dec_MPEG4, sizeof(sm_rgLevelAtIndx_MPEG4));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->IntraDCTTableInfo_Dec_Talking, sizeof(sm_rgLevelAtIndxOfIntraY_Talking));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->InterDCTTableInfo_Dec_Talking,sizeof(sm_rgLevelAtIndx_Talking));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->IntraDCTTableInfo_Dec_HghMt,sizeof(sm_rgLevelAtIndxOfIntraY_HghMt));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->InterDCTTableInfo_Dec_HghMt, sizeof(sm_rgLevelAtIndx_HghMt));  

	//FREE_PTR (pWMVDec, pWMVDec->m_pMotion);
	//FREE_PTR (pWMVDec, pWMVDec->m_pMotionC);

    // _ZONE_VLC_
#if 0
    FREE_PTR (pWMVDec, pWMVDec->m_ppxliFieldMBBuffer);
#endif


    //if (pWMVDec->m_bX9)
	{
        FREE_PTR (pWMVDec, pWMVDec->m_pX9dct);		
		FREE_PTR (pWMVDec, pWMVDec->m_pMultiresBuffer);
		FREE_PTR (pWMVDec, pWMVDec->m_pIntraBlockRowData);		
		FREE_PTR (pWMVDec, pWMVDec->m_pFieldMvX_FPred);
    }

    if(pWMVDec->m_cReferenceLibrary != NULL)
    {
        RL_CloseReferenceLibrary( pWMVDec );
        FREE_PTR (pWMVDec, pWMVDec->m_cReferenceLibrary);
    }
   
    return WMV_Succeeded;
}
//**********************************************
// FreeFrameAreaDependentMemory:
// Buffers in this function depend only on frame area (W * H), 
// This function is called during sequence switch only when the malloc(sizeof( frame area is
// greater than the largest frame area of ALL previous sequences.
// If only width or height is changed, but the area is less than
// previous maximum, only FreeFrameWidthAndHeightDependentMemory() is needed.
// Allocated by initFrameAreaDependentMemory().
//**********************************************
Void_WMV FreeFrameAreaDependentMemory (tWMVDecInternalMember *pWMVDec)
{
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO FreeFrameAreaDependentMemory 1");
#endif
	{
		 FREE_PTR (pWMVDec, pWMVDec->m_framebase);
		 FREE_PTR (pWMVDec, pWMVDec->m_rgmv1_EMB_base);
	}

    if (pWMVDec->m_cvCodecVersion == WMVA) {
        FREE_PTR (pWMVDec, pWMVDec->m_ppxliIntraRowY);
        //FREE_PTR (pWMVDec, pWMVDec->m_ppxliIntraRowU);
        //FREE_PTR (pWMVDec, pWMVDec->m_ppxliIntraRowV);
    }

    //if (pWMVDec->m_bX9)
	{
        FREE_PTR (pWMVDec, pWMVDec->m_pX9dct);        
        FREE_PTR (pWMVDec, pWMVDec->m_pMultiresBuffer);
        FREE_PTR (pWMVDec, pWMVDec->m_pIntraBlockRowData);
		FREE_PTR (pWMVDec, pWMVDec->m_pFieldMvX_FPred);
    }
    // End of common encoder/decoder stuff


	FREE_PTR (pWMVDec, pWMVDec->m_pbStartOfSliceRowOrig);    
    
    //if (pWMVDec->m_cvCodecVersion >= WMV2) 
    //    t_ContextWMVDestruct(pWMVDec, pWMVDec->m_pContext);


    // loopfilter
    FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8RowFlagY0);
    //FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8ColFlagY0);

    if(pWMVDec->m_cReferenceLibrary != NULL)
    {
        RL_CloseReferenceLibrary( pWMVDec );
    }
}

//Bool_WMV g_bSupportMMX_WMV = 0;
Bool_WMV g_bSupportAltiVec_WMV = 0;
Bool_WMV g_bSupportWMMX_WMV = 0;


tWMVDecodeStatus VodecConstruct (tWMVDecInternalMember *pWMVDec, 
                                                I32_WMV iWidthSource, 
                                                I32_WMV iHeightSource)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    I32_WMV result = 0;

    pWMVDec->m_uintNumMBYSlice = 1;
    pWMVDec->m_bFirstEscCodeInFrame= TRUE_WMV; 

    pWMVDec->m_bTopFieldFirst = TRUE_WMV;
    pWMVDec->m_bProgressive420 = TRUE_WMV;

    pWMVDec->m_iChromaFormat = 1;
    pWMVDec->m_bSeqFrameWidthOrHeightChanged = TRUE;
    pWMVDec->m_ihrd_num_leaky_buckets = 1;

    pWMVDec->m_bBFrameOn = TRUE;

    m_InitIDCT_Dec(pWMVDec,TRUE);
    m_InitFncPtrAndZigzag(pWMVDec);
    decide_vc1_routines (pWMVDec);

    if (pWMVDec->m_cvCodecVersion == WMVA)
    {
        pWMVDec->m_bAdvancedProfile = TRUE;
        pWMVDec->m_iNumBFrames = 1;        
        pWMVDec->m_iParseStartCodeBufLen = 4096;
        pWMVDec->m_pParseStartCodeBuffer = (U8_WMV*)wmvMalloc(pWMVDec, 2*pWMVDec->m_iParseStartCodeBufLen+32, DHEAP_STRUCT);
        if (pWMVDec->m_pParseStartCodeBuffer == NULL_WMV)
            return WMV_BadMemory;

        pWMVDec->m_pParseStartCodeBitstream = pWMVDec->m_pParseStartCodeBuffer +((pWMVDec->m_iParseStartCodeBufLen+31)&~31);  
    }
    pWMVDec->m_pbitstrmIn = pWMVDec->m_pInputBitstream;
    tWMVStatus = initSeqIndependentMemory(pWMVDec);
    if (tWMVStatus != WMV_Succeeded)
        return tWMVStatus;
    //the decoder is initialized according to display size, obtained from ASF header.
    //future sequence needs to compare with this to decide whether to reallocate buffers or not.
    pWMVDec->m_iMaxPrevSeqFrameArea = iWidthSource * iHeightSource;
    pWMVDec->m_iMaxFrmWidthSrc = iWidthSource;
    pWMVDec->m_iMaxFrmHeightSrc = iHeightSource;

	pWMVDec->m_iWidthY = (iWidthSource + 15) & ~15;
    pWMVDec->m_iHeightY = (iHeightSource + 15) & ~15;
	pWMVDec->m_uintNumMBX = pWMVDec->m_iWidthY >> 4; // divided by 16, MB size
    pWMVDec->m_uintNumMBY = pWMVDec->m_iHeightY >> 4;
    //old codecs only needs to call memory alloc once
    //allocate according to max size
    result = initFrameAreaDependentMemory(pWMVDec, iWidthSource, iHeightSource);
    if (ICERR_OK != result)
        return WMV_Failed;

    tWMVStatus = InitDataMembers_WMVA (pWMVDec, iWidthSource, iHeightSource);
    if (WMV_Succeeded != tWMVStatus)
        return tWMVStatus;

    //muti-thread 
      // Set 32-byte aligned pointers to IDCT recon coeff and pixel error buffers
    pWMVDec->m_ppxliErrorQ = (UnionBuffer *)(((VO_U32)pWMVDec->m_riPixelError + 31) & ~31);
    pWMVDec->m_rgiCoefReconBuf = (UnionBuffer *)(((VO_U32)pWMVDec->m_riReconBuf + 31) & ~31);
    pWMVDec->m_rgiCoefRecon = (PixelI32 *)pWMVDec->m_rgiCoefReconBuf;

    pWMVDec->m_rgiCoefReconPlus1 = &pWMVDec->m_rgiCoefRecon[1];
    pWMVDec->m_iStepSize = 0;
    //pWMVDec->m_iMotionCompRndCtrl=VECTORIZE_MOTIONCOMP_RNDCTRL_OFF;
    InitMultires (pWMVDec);
    ComputeDQuantDecParam (pWMVDec);
    InitBlockInterpolation (pWMVDec);

    // initialize it for error resilience
    pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmCurrQ;

	//WMV3 24488 WVC1 21496
	if(pWMVDec->huffmanbuffer)
		FREE_PTR(pWMVDec,pWMVDec->huffmanbuffer);

	pWMVDec->huffmanbuffer = (I16_WMV *) wmvMalloc (pWMVDec, 24*1024+32*32, DHEAP_HUFFMAN);
	if (pWMVDec->huffmanbuffer == NULL_WMV)
		return WMV_BadMemory;
	pWMVDec->huffmansize	= 24*1024+32*32;
	pWMVDec->huffmanused   = 0;
	memset(pWMVDec->huffmanbuffer,0,pWMVDec->huffmansize);

    if (pWMVDec->m_cvCodecVersion != WMVA)
    {
        VResultCode vr;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufICBPCYDec, sm_HufICBPCYTable, SIZE_sm_HufICBPCYTable);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec, sm_HufPCBPCYTable, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCyDec_Talking, sm_HufDCTDCyTable_Talking, SIZE_sm_HufDCTDCyTable_Talking);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCcDec_Talking, sm_HufDCTDCcTable_Talking, SIZE_sm_HufDCTDCcTable_Talking);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCyDec_HghMt, sm_HufDCTDCyTable_HghMt, SIZE_sm_HufDCTDCyTable_HghMt);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCcDec_HghMt, sm_HufDCTDCcTable_HghMt, SIZE_sm_HufDCTDCcTable_HghMt);
        
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_HghMt, sm_HufDCTACInterTable_HghMt, SIZE_sm_HufDCTACInterTable_HghMt);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_HghMt, sm_HufDCTACIntraTable_HghMt, SIZE_sm_HufDCTACIntraTable_HghMt);
        
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_Talking, sm_HufDCTACInterTable_Talking, SIZE_sm_HufDCTACInterTable_Talking);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_Talking, sm_HufDCTACIntraTable_Talking, SIZE_sm_HufDCTACIntraTable_Talking);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_MPEG4, sm_HufDCTACInterTable_MPEG4, SIZE_sm_HufDCTACInterTable_MPEG4);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_MPEG4, sm_HufDCTACIntraTable_MPEG4, SIZE_sm_HufDCTACIntraTable_MPEG4);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_HghRate, sm_HufDCTACInterTable_HghRate, SIZE_sm_HufDCTACInterTable_HghRate);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_HghRate, sm_HufDCTACIntraTable_HghRate, SIZE_sm_HufDCTACIntraTable_HghRate);
        
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_1,sm_HufPCBPCYTable_V9_1, SIZE_sm_HufPCBPCYTable_V9_1);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_2,sm_HufPCBPCYTable_V9_2, SIZE_sm_HufPCBPCYTable_V9_2);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_3,sm_HufPCBPCYTable_V9_3, SIZE_sm_HufPCBPCYTable_V9_3);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_4,sm_HufPCBPCYTable_V9_4, SIZE_sm_HufPCBPCYTable_V9_4);

        if (vr != vrNoError) goto lerror;

        pWMVDec->m_pHufPCBPCYTables[0] = &pWMVDec->m_hufPCBPCYDec_V9_1;
        pWMVDec->m_pHufPCBPCYTables[1] = &pWMVDec->m_hufPCBPCYDec_V9_2;
        pWMVDec->m_pHufPCBPCYTables[2] = &pWMVDec->m_hufPCBPCYDec_V9_3;
        pWMVDec->m_pHufPCBPCYTables[3] = &pWMVDec->m_hufPCBPCYDec_V9_4;

        vr  = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV1,sm_HufMVTable_V9_MV1, SIZE_sm_HufMVTable_V9_MV1);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV2,sm_HufMVTable_V9_MV2, SIZE_sm_HufMVTable_V9_MV2);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV3,sm_HufMVTable_V9_MV3, SIZE_sm_HufMVTable_V9_MV3);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV4,sm_HufMVTable_V9_MV4, SIZE_sm_HufMVTable_V9_MV4);
        pWMVDec->m_pHufMVTables[0] = &pWMVDec->m_HufMVTable_V9_MV1;
        pWMVDec->m_pHufMVTables[1] = &pWMVDec->m_HufMVTable_V9_MV2;
        pWMVDec->m_pHufMVTables[2] = &pWMVDec->m_HufMVTable_V9_MV3;
        pWMVDec->m_pHufMVTables[3] = &pWMVDec->m_HufMVTable_V9_MV4;

        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_huf4x4PatternHighQPDec,sm_Huf4x4PatternHighQPTable, SIZE_sm_Huf4x4PatternHighQPTable);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_huf4x4PatternMidQPDec,sm_Huf4x4PatternMidQPTable, SIZE_sm_Huf4x4PatternMidQPTable);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_huf4x4PatternLowQPDec,sm_Huf4x4PatternLowQPTable, SIZE_sm_Huf4x4PatternLowQPTable);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMBXformTypeLowQPDec,sm_HufMBXformTypeLowQPTable, SIZE_sm_HufMBXformTypeLowQPTable);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMBXformTypeMidQPDec,sm_HufMBXformTypeMidQPTable, SIZE_sm_HufMBXformTypeMidQPTable);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMBXformTypeHighQPDec,sm_HufMBXformTypeHighQPTable, SIZE_sm_HufMBXformTypeHighQPTable);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufBlkXformTypeLowQPDec,sm_HufBlkXformTypeLowQPTable, SIZE_sm_HufBlkXformTypeLowQPTable);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufBlkXformTypeMidQPDec,sm_HufBlkXformTypeMidQPTable, SIZE_sm_HufBlkXformTypeMidQPTable);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufBlkXformTypeHighQPDec,sm_HufBlkXformTypeHighQPTable, SIZE_sm_HufBlkXformTypeHighQPTable);
        if (vr != vrNoError) goto lerror;

    }
    else {    
        //Initialize all VLC tables
        if (ICERR_OK != initVlcTables(pWMVDec))
            goto lerror;
    }

    tWMVStatus = setVlcTableInfo(pWMVDec);

#if defined(macintosh) || defined(_ARM_)
    if ((!g_bSupportAltiVec_WMV) && (!g_bSupportWMMX_WMV))
#endif
    {
        if (pWMVDec->m_cvCodecVersion == WMV3) {
            InitEMBPMainLoopCtl(pWMVDec,&pWMVDec->m_EMB_PMainLoop.PB[0]);
            InitEMBPMainLoopCtl(pWMVDec,&pWMVDec->m_EMB_PMainLoop.PB[1]);
        }
    }
    return tWMVStatus;

lerror:
    return WMV_BadMemory;
}

//****************************************************
// InitDataMembers:
// Called by both constructor and sequence switch.
//
// Make sure initializations in this function do not override the 
// sequence header read from decodeSequenceHead_Advanced().
//****************************************************
tWMVDecodeStatus InitDataMembers_WMVA (tWMVDecInternalMember *pWMVDec, I32_WMV iWidthSource, I32_WMV iHeightSource)
{
    I32_WMV result;
    pWMVDec->m_tFrmType = IVOP;

    if (!pWMVDec->m_bSwitchingEntryPoint)
    {
        pWMVDec->m_iBState = -3;
        pWMVDec->m_iPrevIFrame = 0;
        pWMVDec->m_bPrevAnchor = FALSE;          
        pWMVDec->m_iDroppedPFrame = 0;
        pWMVDec->m_bIsBChangedToI = FALSE;
    }

	//if (pWMVDec->m_rgMotionCompBuffer_EMB == NULL) {
 //       pWMVDec->m_rgMotionCompBuffer_EMB = (U32_WMV*) wmvMalloc (pWMVDec, (320) * sizeof (U32_WMV), DHEAP_STRUCT);
	//}
    if (pWMVDec->m_bSeqFrameWidthOrHeightChanged || pWMVDec->bNewSize) {
        result = initFrameWidthAndHeightDependentVariables(pWMVDec, iWidthSource, iHeightSource);
        if (ICERR_OK != result)
            return WMV_Failed;
		initFrameWidthAndHeightDependentPointers(pWMVDec);
    }

    return WMV_Succeeded;
}

//*******************************************************
// initFrameWidthAndHeightDependentVariables:
// This routine is called whenever the sequence frame width or height is changed,
// no matter whether the area is greater than previous seq frame or not.
// 
// Only frame size dependent variables such as iWidthMBAligned should go here.
// Other memory buffer-related pointers should go to initFrameWidthAndHeightDependentPointers(),
// after calling initFrameWidthAndHeightDependentMemory().
//*******************************************************
I32_WMV initFrameWidthAndHeightDependentVariables(tWMVDecInternalMember *pWMVDec, 
    I32_WMV     iWidthSource,
    I32_WMV     iHeightSource)
{
    //If called by constructor, iWidthSource and iHeightSource are obtained from avi header.
    //If called by WMVA re-init, pWMVDec->m_iFrmWidthSrc and pWMVDec->m_iFrmHeightSrc are already set in decodeExtendedSequenceHead_Advanced().
    I32_WMV iWidthMBAligned = (iWidthSource + 15) & ~15;
    I32_WMV iHeightMBAligned = (iHeightSource + 15) & ~15;

    pWMVDec->m_iFrmWidthSrc = iWidthSource;
    pWMVDec->m_iFrmHeightSrc = iHeightSource;
    
    pWMVDec->m_bMBAligned = (
        pWMVDec->m_iFrmWidthSrc == iWidthMBAligned && 
        pWMVDec->m_iFrmHeightSrc == iHeightMBAligned);

    
    pWMVDec->m_iWidthY = iWidthMBAligned;
    pWMVDec->m_iHeightY = iHeightMBAligned;

    //pWMVDec->m_iMVLeftBound = -63; // in half pel
    //pWMVDec->m_iMVRightBound = 63; // in half pel

    //moved from assignCommonDataMembers()

   if (pWMVDec->m_bYUV411) {
        pWMVDec->m_iWidthUV = pWMVDec->m_iWidthY >> 2;
        pWMVDec->m_iHeightUV = pWMVDec->m_iHeightY ;
    }
    else
    {
        pWMVDec->m_iWidthUV = pWMVDec->m_iWidthY >> 1;
        pWMVDec->m_iHeightUV = pWMVDec->m_iHeightY >> 1;
    }

    computePars_WMVA (pWMVDec);

    // for multi-threading, need m_iWidthPrevYXExpPlusExp etc, should be called after computePars().
    //initMultiThreadVars_Dec_WMVA (pWMVDec);
	pWMVDec->m_uiMBEnd0 = pWMVDec->m_uintNumMBY;   
    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;

    return ICERR_OK;
}


// Allocate picture buffer according to max encoding size.
// Use local rctOrgY. 
extern int EXPANDY_REFVOP, EXPANDUV_REFVOP;
I32_WMV AllocatePictureCYUV420 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iMaxEncWidth, 
    I32_WMV iMaxEncHeight)
{
    I32_WMV result;
    I32_WMV iSizeYplane , iSizeUVplane ;

    I32_WMV iWidthMBAlignedY = (iMaxEncWidth + 15) & ~15;
    I32_WMV iHeightMBAlignedY = (iMaxEncHeight + 15) & ~15;

    I32_WMV iWidthMBAlignedUV ;
    I32_WMV iHeightMBAlignedUV;

    I32_WMV iPlaneWidth, iPlaneHeight;
    I32_WMV iFrameNum;

    EXPANDY_REFVOP  = 32;
    EXPANDUV_REFVOP = 16;

    if (pWMVDec->m_bYUV411) {
        iWidthMBAlignedUV = iWidthMBAlignedY >>2;
        iHeightMBAlignedUV = iHeightMBAlignedY;
		iFrameNum = 1;
    } else {
        iWidthMBAlignedUV = iWidthMBAlignedY >>1 ;
        iHeightMBAlignedUV = iHeightMBAlignedY >> 1;
		iFrameNum = 0;
    }

    
   if (pWMVDec->m_cvCodecVersion == WMVA) {
        // For Broadcast profile we need to allocate frame buffers that have the number of lines (vertical size) 
        // aligned to a multiple of 32. This is because field pictures need to be a multiple of 16 lines.

        iHeightMBAlignedY = (iHeightMBAlignedY + 31) & ~31;
        iHeightMBAlignedUV = iHeightMBAlignedY >> 1;

   }
  
	iPlaneWidth  = iWidthMBAlignedY  + EXPANDY_REFVOP * 2;
	iPlaneHeight = iHeightMBAlignedY + EXPANDY_REFVOP * 2;

    iSizeYplane = iPlaneWidth * iPlaneHeight;
    iSizeUVplane = ((iWidthMBAlignedUV) + EXPANDUV_REFVOP * 2) * ((iHeightMBAlignedUV)+ EXPANDUV_REFVOP * 2);
   
    {
		iFrameNum += 6;
        result= RL_SetupReferenceLibrary (pWMVDec, iSizeYplane, iSizeUVplane, iFrameNum, 
            iPlaneWidth * (pWMVDec->m_cvCodecVersion == WMVA)*4, iPlaneWidth, iPlaneHeight);
    
        if (ICERR_OK != result)
            return result;

		RL_RemoveFrame(pWMVDec->m_cReferenceLibrary, (Void_WMV **)&pWMVDec->m_pfrmCurrQ, -1);
		RL_RemoveFrame(pWMVDec->m_cReferenceLibrary, (Void_WMV **)&pWMVDec->m_pfrmRef0Q, -1);
		RL_RemoveFrame(pWMVDec->m_cReferenceLibrary, (Void_WMV **)&pWMVDec->m_pfrmPostQ, -1);
		RL_RemoveFrame(pWMVDec->m_cReferenceLibrary, (Void_WMV **)&pWMVDec->m_pfrmPostPrevQ, -1);

        RL_RemoveFrame(pWMVDec->m_cReferenceLibrary, (Void_WMV **)&pWMVDec->m_pfrmMultiresQ, -1); //wshao: Multires<=>Multiresolution coding. WVP2 will use it.
        RL_RemoveFrame(pWMVDec->m_cReferenceLibrary, (Void_WMV **)&pWMVDec->m_pfrmRef1Q, -1);
		if (pWMVDec->m_bYUV411) 
			RL_RemoveFrame(pWMVDec->m_cReferenceLibrary, (Void_WMV **)&pWMVDec->m_pfrmYUV411toYUV420Q, -1);

    }



    //Return error if any of the frames is allocated.
    if (pWMVDec->m_pfrmCurrQ==0 || pWMVDec->m_pfrmRef0Q==0 || pWMVDec->m_pfrmPostQ==0 || pWMVDec->m_pfrmPostPrevQ==0)
        return ICERR_MEMORY;

    pWMVDec->m_ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
    pWMVDec->m_ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
    pWMVDec->m_ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane;

   /* pWMVDec->m_ppxliCurrQPlusExpY0 = (U8_WMV*) pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    pWMVDec->m_ppxliCurrQPlusExpU0 = (U8_WMV*) pWMVDec->m_ppxliCurrQU  + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    pWMVDec->m_ppxliCurrQPlusExpV0 = (U8_WMV*) pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;*/


    memset (pWMVDec->m_ppxliCurrQY,  0,iSizeYplane);
    memset (pWMVDec->m_ppxliCurrQU,128,iSizeUVplane);
    memset (pWMVDec->m_ppxliCurrQV,128,iSizeUVplane);

    pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmRef0Q->m_pucYPlane;
    pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Q->m_pucUPlane;
    pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Q->m_pucVPlane;
    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;


    pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
    pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
    pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;

    pWMVDec->m_ppxliPostPrevQY = pWMVDec->m_pfrmPostPrevQ->m_pucYPlane ;
    pWMVDec->m_ppxliPostPrevQU = pWMVDec->m_pfrmPostPrevQ->m_pucUPlane ;
    pWMVDec->m_ppxliPostPrevQV = pWMVDec->m_pfrmPostPrevQ->m_pucVPlane;

   
    if (pWMVDec->m_pfrmMultiresQ)
    {
        pWMVDec->m_ppxliMultiresY = pWMVDec->m_pfrmMultiresQ->m_pucYPlane ;
        pWMVDec->m_ppxliMultiresU = pWMVDec->m_pfrmMultiresQ->m_pucUPlane ;
        pWMVDec->m_ppxliMultiresV = pWMVDec->m_pfrmMultiresQ->m_pucVPlane;
    }
       
    if (pWMVDec->m_pfrmRef1Q)
    {
        pWMVDec->m_ppxliRef1Y = pWMVDec->m_pfrmRef1Q->m_pucYPlane ;
        pWMVDec->m_ppxliRef1U = pWMVDec->m_pfrmRef1Q->m_pucUPlane ;
        pWMVDec->m_ppxliRef1V = pWMVDec->m_pfrmRef1Q->m_pucVPlane;

        pWMVDec->m_ppxliRef1YPlusExp = pWMVDec-> m_ppxliRef1Y + pWMVDec->m_iWidthPrevYXExpPlusExp; 
    }

    return ICERR_OK;
}

#ifdef SHAREMEMORY
tYUV420Frame_WMV* AllocateOneFrame(tWMVDecInternalMember *pWMVDec, I32_WMV iYSize,  
		I32_WMV iUSize, I32_WMV iPad)
{
	U32_WMV iVSize = iUSize;
	U8_WMV *ppxlTmp;
	I32_WMV uv_offset;
	tYUV420Frame_WMV* pFrame = NULL;

	pFrame = (tYUV420Frame_WMV *)(pWMVDec->m_framebase + pWMVDec->framebaseused);
	pWMVDec->framebaseused += sizeof (tYUV420Frame_WMV);
	pWMVDec->framebaseused = (pWMVDec->framebaseused+31)&(~31);

	memset(pFrame, 0, sizeof (tYUV420Frame_WMV) );
	pFrame->m_pucYPlane_Unaligned = ppxlTmp = pWMVDec->m_framebase + pWMVDec->framebaseused;
	pFrame->m_pCurrPlaneBase = ppxlTmp;
	pWMVDec->framebaseused += (iYSize+iUSize+iVSize);
	pWMVDec->framebaseused = (pWMVDec->framebaseused+31)&(~31);
	
	//Y
	pFrame->m_pucYPlane  = (U8_WMV *)(((U32_WMV)ppxlTmp + iPad + 31) & ~31);

	//U
	ppxlTmp  = pFrame->m_pucYPlane_Unaligned + iYSize;
	ppxlTmp += iPad>>1;
	uv_offset = ((U32_WMV)ppxlTmp) & 31 ;
	if(uv_offset != 0)
		uv_offset = 32 - uv_offset;
	pFrame->m_pucUPlane  = (U8_WMV *)((U32_WMV)ppxlTmp + uv_offset);

	//V
	ppxlTmp  = pFrame->m_pucYPlane_Unaligned + iYSize + iUSize;
	ppxlTmp += iPad>>1;
	uv_offset = ((U32_WMV)ppxlTmp) & 31 ;
	if(uv_offset != 0)
		uv_offset = 32 - uv_offset;

	pFrame->m_pucVPlane  = (U8_WMV *)((U32_WMV)ppxlTmp + uv_offset);

	pFrame->m_timeStamp = 0;
	pFrame->m_frameType = 0;

	return pFrame;
}
I32_WMV AllocatePictureCYUV420_NEW (tWMVDecInternalMember *pWMVDec, I32_WMV iMaxEncWidth, I32_WMV iMaxEncHeight)
{
    I32_WMV iSizeYplane , iSizeUVplane ;

    I32_WMV iWidthMBAlignedY = (iMaxEncWidth + 15) & ~15;
    I32_WMV iHeightMBAlignedY = (iMaxEncHeight + 15) & ~15;

    I32_WMV iWidthMBAlignedUV ;
    I32_WMV iHeightMBAlignedUV;

    I32_WMV iPlaneWidth, iPlaneHeight;
    I32_WMV i;

    EXPANDY_REFVOP  = 32;
    EXPANDUV_REFVOP = 16;

    if (pWMVDec->m_bYUV411) {
        iWidthMBAlignedUV = iWidthMBAlignedY >>2;
        iHeightMBAlignedUV = iHeightMBAlignedY;
    } else {
        iWidthMBAlignedUV = iWidthMBAlignedY >>1 ;
        iHeightMBAlignedUV = iHeightMBAlignedY >> 1;
    }

    
   if (pWMVDec->m_cvCodecVersion == WMVA) {
        // For Broadcast profile we need to allocate frame buffers that have the number of lines (vertical size) 
        // aligned to a multiple of 32. This is because field pictures need to be a multiple of 16 lines.
        iHeightMBAlignedY = (iHeightMBAlignedY + 31) & ~31;
        iHeightMBAlignedUV = iHeightMBAlignedY >> 1;
   }
  
	iPlaneWidth  = iWidthMBAlignedY  + EXPANDY_REFVOP * 2;
	iPlaneHeight = iHeightMBAlignedY + EXPANDY_REFVOP * 2;

    iSizeYplane = iPlaneWidth * iPlaneHeight;
    iSizeUVplane = ((iWidthMBAlignedUV) + EXPANDUV_REFVOP * 2) * ((iHeightMBAlignedUV)+ EXPANDUV_REFVOP * 2);
  

	{
		U32_WMV iPad = iPlaneWidth * (pWMVDec->m_cvCodecVersion == WMVA)*4;
		U32_WMV iYSize = (iSizeYplane  + (iPad<<1) + 32) * sizeof (U8_WMV)+32;
		U32_WMV iUSize = (iSizeUVplane + iPad      + 32) * sizeof (U8_WMV)+32;
		U32_WMV iVSize = iUSize;
		U32_WMV total_size = (sizeof (tYUV420Frame_WMV)+iYSize+iUSize+iVSize+64)*pWMVDec->ntotal_frames;

		//pWMVDec->m_pfrmList_size =  sizeof (tYUV420Frame_WMV)+iYSize+iUSize+iVSize+64;
		pWMVDec->m_framebase = (U8_WMV *)wmvMalloc (pWMVDec,total_size, DHEAP_STRUCT); 
		if(pWMVDec->m_framebase == NULL )
			return WMV_BadMemory;
		pWMVDec->framebaseused = 0;	
		pWMVDec->m_rgmv1_EMB_base = (CMotionVector_X9_EMB*)wmvMalloc (pWMVDec,
			pWMVDec->ntotal_frames*pWMVDec->m_uintNumMBX*pWMVDec->m_uintNumMBY*sizeof(CMotionVector_X9_EMB), DHEAP_STRUCT);

		if(pWMVDec->m_rgmv1_EMB_base == NULL )
			return WMV_BadMemory;

		for(i=0;i<pWMVDec->ntotal_frames;i++)
		{
			pWMVDec->m_pfrmList[i] = AllocateOneFrame(pWMVDec,iYSize, iUSize, iPad);

			pWMVDec->m_pfrmList[i]->m_rgmv1_EMB = pWMVDec->m_rgmv1_EMB_base + i*pWMVDec->m_uintNumMBX*pWMVDec->m_uintNumMBY;
            //ZOU FIFO
			pWMVDec->m_memManageFIFO.img_seq[i] = pWMVDec->m_pfrmList[i];
			voprintf("---0x%x \n",pWMVDec->m_memManageFIFO.img_seq[i]);
		}

		//(1+1+pWMVDec->CpuNumber) : 411,Muilt,Ref0Post
		pWMVDec->m_memManageFIFO.img_seq[pWMVDec->ntotal_frames-(1+1+pWMVDec->CpuNumber)] = NULL;
		pWMVDec->m_pfrmCurrQ = pWMVDec->m_pfrmList[0];
		pWMVDec->m_nIdx = -1;
		pWMVDec->m_pfrmRef0Q = pWMVDec->m_pfrmList[0];
		pWMVDec->m_pfrmRef1Q = pWMVDec->m_pfrmList[0];
		pWMVDec->m_pfrmPostQ = pWMVDec->m_pfrmList[0];
		pWMVDec->m_pfrmPostPrevQ = pWMVDec->m_pfrmList[0];
		pWMVDec->m_pfrmPrevious = pWMVDec->m_pfrmCurrQ;

		pWMVDec->m_memManageFIFO.r_idx = 0;
		pWMVDec->m_memManageFIFO.w_idx = pWMVDec->ntotal_frames-(1+1+pWMVDec->CpuNumber);
		pWMVDec->m_memManageFIFO.buf_num = pWMVDec->ntotal_frames-(1+1+pWMVDec->CpuNumber);

		pWMVDec->m_ref0Indx = 0;
		for(i=0; i < pWMVDec->CpuNumber; i++)
		{
			pWMVDec->m_pfrmRef0Buffer[i]= pWMVDec->m_pfrmList[pWMVDec->ntotal_frames-(2+i)-1];
		}
		pWMVDec->m_pfrmRef0Process = pWMVDec->m_pfrmRef0Buffer[0];

		pWMVDec->m_pfrmMultiresQ = pWMVDec->m_pfrmList[pWMVDec->ntotal_frames-2];		
		if (pWMVDec->m_bYUV411) 
			pWMVDec->m_pfrmYUV411toYUV420Q = pWMVDec->m_pfrmList[pWMVDec->ntotal_frames-1];


#if 0
		pWMVDec->m_memManageFIFO.img_seq[pWMVDec->ntotal_frames-3] = NULL;	

		pWMVDec->m_pfrmCurrQ = pWMVDec->m_pfrmList[0];
		pWMVDec->m_nIdx = -1;
		pWMVDec->m_pfrmRef0Q = pWMVDec->m_pfrmList[0];
		pWMVDec->m_pfrmRef1Q = pWMVDec->m_pfrmList[0];
		pWMVDec->m_pfrmPostQ = pWMVDec->m_pfrmList[0];
		pWMVDec->m_pfrmPostPrevQ = pWMVDec->m_pfrmList[0];
		pWMVDec->m_pfrmPrevious = pWMVDec->m_pfrmCurrQ;

//ZOU FIFO
		pWMVDec->m_memManageFIFO.r_idx = 0;
		pWMVDec->m_memManageFIFO.w_idx = pWMVDec->ntotal_frames-3;
		pWMVDec->m_memManageFIFO.buf_num = pWMVDec->ntotal_frames-3;		

		//TODO
		pWMVDec->m_ref0Indx = 0;
		pWMVDec->m_pfrmRef0Buffer[0]= pWMVDec->m_pfrmList[pWMVDec->ntotal_frames-3];
		pWMVDec->m_pfrmRef0Buffer[1]= pWMVDec->m_pfrmList[pWMVDec->ntotal_frames-2];

		pWMVDec->m_pfrmRef0Process = pWMVDec->m_pfrmRef0Buffer[0];

		pWMVDec->m_pfrmMultiresQ = pWMVDec->m_pfrmList[pWMVDec->ntotal_frames-2];		
		if (pWMVDec->m_bYUV411) 
			pWMVDec->m_pfrmYUV411toYUV420Q = pWMVDec->m_pfrmList[pWMVDec->ntotal_frames-1];
#endif
	}

    //Return error if any of the frames is allocated.
    if (pWMVDec->m_pfrmCurrQ==0 || pWMVDec->m_pfrmRef0Q==0 || pWMVDec->m_pfrmPostQ==0 || pWMVDec->m_pfrmPostPrevQ==0)
        return ICERR_MEMORY;

    pWMVDec->m_ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
    pWMVDec->m_ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
    pWMVDec->m_ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane;

    memset (pWMVDec->m_ppxliCurrQY,  0,iSizeYplane);
    memset (pWMVDec->m_ppxliCurrQU,128,iSizeUVplane);
    memset (pWMVDec->m_ppxliCurrQV,128,iSizeUVplane);

	//printf("memset \n");

    pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmRef0Q->m_pucYPlane;
    pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Q->m_pucUPlane;
    pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Q->m_pucVPlane;
    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;

    pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
    pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
    pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;

    pWMVDec->m_ppxliPostPrevQY = pWMVDec->m_pfrmPostPrevQ->m_pucYPlane ;
    pWMVDec->m_ppxliPostPrevQU = pWMVDec->m_pfrmPostPrevQ->m_pucUPlane ;
    pWMVDec->m_ppxliPostPrevQV = pWMVDec->m_pfrmPostPrevQ->m_pucVPlane;

   
    if (pWMVDec->m_pfrmMultiresQ)
    {
        pWMVDec->m_ppxliMultiresY = pWMVDec->m_pfrmMultiresQ->m_pucYPlane ;
        pWMVDec->m_ppxliMultiresU = pWMVDec->m_pfrmMultiresQ->m_pucUPlane ;
        pWMVDec->m_ppxliMultiresV = pWMVDec->m_pfrmMultiresQ->m_pucVPlane;
    }
       
    if (pWMVDec->m_pfrmRef1Q)
    {
        pWMVDec->m_ppxliRef1Y = pWMVDec->m_pfrmRef1Q->m_pucYPlane ;
        pWMVDec->m_ppxliRef1U = pWMVDec->m_pfrmRef1Q->m_pucUPlane ;
        pWMVDec->m_ppxliRef1V = pWMVDec->m_pfrmRef1Q->m_pucVPlane;

        pWMVDec->m_ppxliRef1YPlusExp = pWMVDec-> m_ppxliRef1Y + pWMVDec->m_iWidthPrevYXExpPlusExp; 
    }

    return ICERR_OK;
}
#endif

//*****************************************
// initSeqIndependentMemory:
// Sequence independent memory allocation. 
// Only need to be called once by the constructor.
// Free in FreeSeqIndependentMemory().
//*****************************************
tWMVDecodeStatus initSeqIndependentMemory(tWMVDecInternalMember *pWMVDec)
{
	I32_WMV total_size = sizeof(CReferenceLibrary) //m_cReferenceLibrary
		+ sizeof(I16_WMV)*(6 * BLOCK_SQUARE_SIZE + 32) //m_ppxliFieldMBBuffer
		+ 6*sizeof(UnionBuffer)//m_rgErrorBlock
		+ 320*sizeof (U32_WMV);//m_rgMotionCompBuffer_EMB

	pWMVDec->m_cReferenceLibrary =  (CReferenceLibrary *)wmvMalloc(pWMVDec, total_size, DHEAP_STRUCT);
	 if(pWMVDec->m_cReferenceLibrary == NULL)
        return WMV_BadMemory;
    memset(pWMVDec->m_cReferenceLibrary, 0, sizeof(CReferenceLibrary));

	pWMVDec->m_ppxliFieldMBBuffer = (I16_WMV *)(pWMVDec->m_cReferenceLibrary + 1);
	pWMVDec->m_ppxliFieldMB = (I16_WMV *) (((I32_WMV)pWMVDec->m_ppxliFieldMBBuffer + 31) & ~31);
	pWMVDec->m_rgErrorBlock = (UnionBuffer*)(pWMVDec->m_ppxliFieldMBBuffer + (6 * BLOCK_SQUARE_SIZE + 32));
	pWMVDec->m_rgMotionCompBuffer_EMB = (U32_WMV*)(pWMVDec->m_rgErrorBlock + 6);

	pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 128;
    pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 128;

#if 0
	pWMVDec->m_pAltTables = t_AltTablesDecoderConstruct (pWMVDec);
	if (pWMVDec->m_pAltTables == NULL_WMV)
		return WMV_BadMemory;
#endif

    return WMV_Succeeded;
}


//*******************************************************
// initSeqFrameAreaDependentMemory:
// This routine is called by both constructor and WMVA sequence switch.
// In WMVA seq switch, it's called only when the frame area (W * H)
// of the next sequence is greater than all of previous frame area.
//
// However, some memory-related variables need to be re-initialized whenever the frame
// size parameters (Width, Height) are changed, no matter whether the area
// becomes larger or smaller. These variables should be re-inited in
// initFrameWidthAndHeightDependentPointers() instead of this function.
// Free by FreeFrameAreaDependentMemory().
//*******************************************************
I32_WMV initModeDependentMemory(tWMVDecInternalMember *pWMVDec, I32_WMV iMaxEncWidth, I32_WMV iMaxEncHeight)
{
    //Bool_WMV bAllocFail = FALSE;
    I32_WMV twStatus = 0;            

    I32_WMV iWidthMBAligned = (iMaxEncWidth + 15) & ~15;
    I32_WMV iHeightMBAligned = (iMaxEncHeight + 15) & ~15;
    I32_WMV iMaxNumMBX = iWidthMBAligned >> 4; // divided by 16, MB size
    I32_WMV iMaxNumMBY = iHeightMBAligned >> 4;
    I32_WMV iMaxNumMB = iMaxNumMBX * iMaxNumMBY;

#ifdef STABILITY //
    if (pWMVDec->m_cvCodecVersion == WMVA) 
#else
	if (pWMVDec->m_cvCodecVersion == WMVA && pWMVDec->m_bInterlaceV2) 
#endif
    {
#ifdef STABILITY
        if(pWMVDec->m_ppxliIntraRowY)
		{
            wmvFree(pWMVDec,pWMVDec->m_ppxliIntraRowY);
			pWMVDec->m_ppxliIntraRowY = NULL;
		}
#endif
		pWMVDec->m_ppxliIntraRowY = (I16_WMV *)wmvMalloc(pWMVDec, sizeof( I16_WMV) * iMaxNumMBX * (16*16+64+64), DHEAP_STRUCT);
		if(pWMVDec->m_ppxliIntraRowY == NULL)
			return WMV_BadMemory;
        pWMVDec->m_ppxliIntraRowU = pWMVDec->m_ppxliIntraRowY +  iMaxNumMBX * 16 * 16;
        pWMVDec->m_ppxliIntraRowV = pWMVDec->m_ppxliIntraRowU + iMaxNumMBX * 8 * 8;
    }
    /*else
    {
        pWMVDec->m_ppxliIntraRowY = NULL;
        pWMVDec->m_ppxliIntraRowU = NULL;
        pWMVDec->m_ppxliIntraRowV = NULL;
    }*/

    //if (pWMVDec->m_bX9)
    {
#ifdef STABILITY
        if(pWMVDec->m_pIntraBlockRowData)
            wmvFree(pWMVDec,pWMVDec->m_pIntraBlockRowData);
#endif
        pWMVDec->m_pIntraBlockRowData = (I16_WMV *)wmvMalloc(pWMVDec, sizeof(I16_WMV) * (4+ iMaxNumMBX *1936), DHEAP_STRUCT);
        if (!pWMVDec->m_pIntraBlockRowData)
            return ICERR_MEMORY;
        // pWMVDec->m_pX9dct = NULL;
        // YUV411
#ifdef STABILITY
		if(pWMVDec->m_pX9dct)
			wmvFree(pWMVDec,pWMVDec->m_pX9dct);
#endif
        if (pWMVDec->m_bYUV411) 
        {
            // 4 Y Block of 16 Bytes each and U,V of 24 bytes each.
            pWMVDec->m_pX9dct = (I16_WMV *)wmvMalloc(pWMVDec,  sizeof(I16_WMV)*(iMaxNumMB * ( 4 * 16 + 2 * 24)), DHEAP_STRUCT);
            if (!pWMVDec->m_pX9dct)
                return ICERR_MEMORY;
            memset (pWMVDec->m_pX9dct, 0, iMaxNumMB * ( 4 * 16 + 2 * 24) * sizeof(I16_WMV));            
        } 
        else 
        {
            pWMVDec->m_pX9dct = (I16_WMV *)wmvMalloc(pWMVDec,  sizeof(I16_WMV)*(/*iMaxNumMB*/ iMaxNumMBX*2* 6 * 16), DHEAP_STRUCT);
            if (!pWMVDec->m_pX9dct)
                return ICERR_MEMORY;
            memset (pWMVDec->m_pX9dct, 0, /*iMaxNumMB*/ iMaxNumMBX*2 * 6 * 16 * sizeof(I16_WMV));
        }   

        //if(pWMVDec->m_bYUV411 || pWMVDec->m_bInterlaceV2)  //zou 2012.11.20 some clips may have  progress, interframe,and interfiled
        {
			U32_WMV buffer_size = (sizeof(I16_WMV)*iMaxNumMB * 8 +32
				+ sizeof(I16_WMV)*iMaxNumMB * 8+32
				+ sizeof(I16_WMV)*iMaxNumMB * 8+32
				+ sizeof(I16_WMV)*iMaxNumMBX * (iMaxNumMBY + 2) * 8+32
				+ sizeof(I16_WMV)*iMaxNumMB * 8+32
				+ (2 * MB_SIZE * MB_SIZE + 4 * BLOCK_SIZE * BLOCK_SIZE + 32 + 160)+32);
#ifdef STABILITY
            if(pWMVDec->m_pFieldMvX_FPred)
                wmvFree(pWMVDec,pWMVDec->m_pFieldMvX_FPred);
#endif
			pWMVDec->m_pFieldMvX_FPred = (I16_WMV *)wmvMalloc( pWMVDec, buffer_size, DHEAP_STRUCT);
            if (pWMVDec->m_pFieldMvX_FPred == NULL)
                return ICERR_MEMORY;

			pWMVDec->m_pFieldMvX_BPred = (I16_WMV *)ALIGN(pWMVDec->m_pFieldMvX_FPred + iMaxNumMB * 8);
			pWMVDec->m_pFieldMvX  = (I16_WMV *)ALIGN(pWMVDec->m_pFieldMvX_BPred + iMaxNumMB * 8);
			pWMVDec->m_pFieldMvX_Pbuf = (I16_WMV *)ALIGN(pWMVDec->m_pFieldMvX + iMaxNumMB * 8);
			pWMVDec->m_pB411InterpX = (I16_WMV *)ALIGN(pWMVDec->m_pFieldMvX_Pbuf + iMaxNumMBX * (iMaxNumMBY + 2) * 8);
			pWMVDec->m_pB411InterpY = (I16_WMV *)ALIGN(pWMVDec->m_pB411InterpX + iMaxNumMB * 4);
			pWMVDec->m_pInterpY0Buffer = (U8_WMV*)ALIGN(pWMVDec->m_pB411InterpX + iMaxNumMB * 8);
			memset(pWMVDec->m_pFieldMvX_Pbuf, 0, sizeof(I16_WMV)*iMaxNumMBX * (iMaxNumMBY + 2) * 8);

			/*************************/
        }
        //else
        //{
        //    pWMVDec->m_pFieldMvX_FPred = NULL;
        //    pWMVDec->m_pFieldMvX_BPred = NULL;
        //}

        if(pWMVDec->m_bMultiresEnabled)
        {
            //I32_WMV iMax = pWMVDec->m_iWidthY * 6 + 128 + 32;
            //if (iMax < pWMVDec->m_iHeightY * 4 + 256 + 32)
            //    iMax = pWMVDec->m_iHeightY * 4 + 256 + 32;
			I32_WMV iMax = pWMVDec->m_iWidthSource * 6 + 128 + 32;
            if (iMax < pWMVDec->m_iHeightSource * 4 + 256 + 32)
                iMax = pWMVDec->m_iHeightSource* 4 + 256 + 32;
#ifdef STABILITY
            if(pWMVDec->m_pMultiresBuffer)
                wmvFree(pWMVDec,pWMVDec->m_pMultiresBuffer);
#endif
            pWMVDec->m_pMultiresBuffer = (I8_WMV *)wmvMalloc( pWMVDec, iMax, DHEAP_STRUCT);
            if (!pWMVDec->m_pMultiresBuffer)
                return ICERR_MEMORY;
            pWMVDec->m_pAlignedMultiresBuffer = (I8_WMV *)(((U32_WMV)pWMVDec->m_pMultiresBuffer + 31) & ~31);
        }  
        else
        {
            pWMVDec->m_pMultiresBuffer = NULL;
        }
    }

    return twStatus;
}
I32_WMV initFrameAreaDependentMemory(tWMVDecInternalMember *pWMVDec, I32_WMV iMaxEncWidth, I32_WMV iMaxEncHeight)
{
	I32_WMV i=0,total_size = 0,total_size0 = 0,total_size1 = 0;
    I32_WMV iMaxSliceBufSize;
    I32_WMV twStatus;            

    I32_WMV iWidthMBAligned = (iMaxEncWidth + 15) & ~15;
    I32_WMV iHeightMBAligned = (iMaxEncHeight + 15) & ~15;
    I32_WMV iMaxNumMBX = iWidthMBAligned >> 4; // divided by 16, MB size
    I32_WMV iMaxNumMBY = iHeightMBAligned >> 4;
    I32_WMV iMaxNumMB = iMaxNumMBX * iMaxNumMBY;

    twStatus = AllocateMultiThreadBufs_Dec_WMVA(pWMVDec, iMaxEncWidth, iMaxEncHeight);
    if (twStatus != ICERR_OK)
        return twStatus;

#ifdef SHAREMEMORY
#ifdef USE_FRAME_THREAD
	if(pWMVDec->isThreadCtx)
	{
		;// do nothing ,we will use the buffer of main contex
	}
	else
#endif
		twStatus = AllocatePictureCYUV420_NEW(pWMVDec, iMaxEncWidth, iMaxEncHeight);
	//printf("AllocatePictureCYUV420_NEW\n");
#else
	 twStatus = AllocatePictureCYUV420(pWMVDec, iMaxEncWidth, iMaxEncHeight);
#endif
    if (twStatus != ICERR_OK)
        return twStatus;

    iMaxSliceBufSize = ((iHeightMBAligned >> 4) + 1);

#ifdef STABILITY
    if(pWMVDec->m_pbStartOfSliceRowOrig)
        FREE_PTR(pWMVDec,pWMVDec->m_pbStartOfSliceRowOrig);
#endif

	total_size0 = sizeof(I32_WMV)*iMaxNumMBX	+32					//bIntraFlag
				+ sizeof(I32_WMV)*iMaxNumMBX+32						//b1MVFlag
				+ iMaxNumMBX*sizeof(CDiffMV_EMB)*6+32			//m_pDiffMV_EMB_Bak
				+ iMaxNumMBX *1 *sizeof(CWMVMBModeBack)+32;		//m_pMbMode

	total_size1 = sizeof( LOOPF_FLAG ) *iMaxNumMB+32 //m_pLoopFilterFlags
		+ sizeof( CWMVMBMode ) *(iMaxNumMB+2*iMaxNumMBX) +32//m_rgmbmd
		+ sizeof( CWMVMBMode) +32//m_pmbmdZeroCBPCY
		+ sizeof( I16_WMV) *iMaxNumMBX * 2 * BLOCK_SIZE_TIMES2_TIMES6+32 //m_rgiQuanCoefACPred
		+ sizeof( I16_WMV*) *iMaxNumMBX * 2  * 36+32 //m_rgiQuanCoefACPredTable
		+ sizeof( I16_WMV) * iMaxNumMBX * iMaxNumMBY * 8+32 //m_pXMotion
		+ sizeof( I16_WMV) * iMaxNumMBX * iMaxNumMBY * 1+32 //m_pXMotionC
		+ sizeof( CMotionVector_X9_EMB)* 5 * iMaxNumMB+32 //m_rgmv1_EMB 
		+ sizeof(UMotion_EMB)*(pWMVDec->m_uintNumMBX * (pWMVDec->m_uintNumMBY+1) * 4)+32 //m_pMotion
		+ sizeof(UMotion_EMB)*(pWMVDec->m_uintNumMBX * (pWMVDec->m_uintNumMBY+1) )+32 ;    //m_pMotionC

	total_size = sizeof(Bool_WMV)*iMaxSliceBufSize+32 //m_pbStartOfSliceRowOrig
		+ 2*total_size0 + total_size1;

    pWMVDec->m_pbStartOfSliceRowOrig = (Bool_WMV *)wmvMalloc(pWMVDec, (total_size+5*32)*2, DHEAP_STRUCT);

    if (pWMVDec->m_pbStartOfSliceRowOrig == NULL) 
        return ICERR_ERROR;
    memset (pWMVDec->m_pbStartOfSliceRowOrig, 0, sizeof(Bool_WMV)* iMaxSliceBufSize);
    pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;

    //
    for(i=0;i<1;i++)//if(pWMVDec->CpuNumber>=2)
    {
		pWMVDec->m_EMB_PMainLoop.PB[i].bIntraFlag = (I32_WMV*)ALIGN(pWMVDec->m_pbStartOfSliceRowOrig + iMaxSliceBufSize);
		pWMVDec->m_EMB_PMainLoop.PB[i].bIntraFlag = (I32_WMV*)ALIGN((U8_WMV*)pWMVDec->m_EMB_PMainLoop.PB[i].bIntraFlag + i*total_size0);
		pWMVDec->m_EMB_PMainLoop.PB[i].b1MVFlag = (I32_WMV*)ALIGN(pWMVDec->m_EMB_PMainLoop.PB[i].bIntraFlag + iMaxNumMBX);
		pWMVDec->m_EMB_PMainLoop.PB[i].m_pDiffMV_EMB_Bak = (CDiffMV_EMB*)ALIGN(pWMVDec->m_EMB_PMainLoop.PB[i].b1MVFlag + iMaxNumMBX);
		pWMVDec->m_EMB_PMainLoop.PB[i].m_pMbMode = (CWMVMBModeBack*)ALIGN(pWMVDec->m_EMB_PMainLoop.PB[i].m_pDiffMV_EMB_Bak + iMaxNumMBX*6);

		 //B frame
         /*pWMVDec->m_EMB_BMainLoop.PB[i].bIntraFlag = pWMVDec->m_EMB_PMainLoop.PB[i].bIntraFlag;
         pWMVDec->m_EMB_BMainLoop.PB[i].b1MVFlag = pWMVDec->m_EMB_PMainLoop.PB[i].b1MVFlag;
         pWMVDec->m_EMB_BMainLoop.PB[i].m_pDiffMV_EMB_Bak = pWMVDec->m_EMB_PMainLoop.PB[i].m_pDiffMV_EMB_Bak;
		 pWMVDec->m_EMB_BMainLoop.PB[i].m_pMbMode =pWMVDec->m_EMB_PMainLoop.PB[i].m_pMbMode;*/

		pWMVDec->m_EMB_BMainLoop.PB[i].bIntraFlag = (I32_WMV*)ALIGN((U8_WMV*)pWMVDec->m_pbStartOfSliceRowOrig + total_size+5*32);
		pWMVDec->m_EMB_BMainLoop.PB[i].b1MVFlag = (I32_WMV*)ALIGN((U8_WMV*)pWMVDec->m_EMB_BMainLoop.PB[i].bIntraFlag + iMaxNumMBX);
		pWMVDec->m_EMB_BMainLoop.PB[i].m_pDiffMV_EMB_Bak =  (CDiffMV_EMB*)ALIGN(pWMVDec->m_EMB_BMainLoop.PB[i].b1MVFlag + iMaxNumMBX);
		pWMVDec->m_EMB_BMainLoop.PB[i].m_pMbMode = (CWMVMBModeBack*)ALIGN(pWMVDec->m_EMB_BMainLoop.PB[i].m_pDiffMV_EMB_Bak + iMaxNumMBX*6);
    }

	pWMVDec->m_pLoopFilterFlags = (LOOPF_FLAG *)ALIGN(((U8_WMV*)pWMVDec->m_pbStartOfSliceRowOrig) + sizeof(Bool_WMV)*iMaxSliceBufSize + 2*total_size0);
	pWMVDec->m_rgmbmd = (CWMVMBMode*)ALIGN(pWMVDec->m_pLoopFilterFlags + iMaxNumMB);
	pWMVDec->m_rgmbmd_cur = (CWMVMBMode*)ALIGN(pWMVDec->m_rgmbmd + iMaxNumMB);
    pWMVDec->m_rgmbmd_last = (CWMVMBMode*)ALIGN(pWMVDec->m_rgmbmd_cur + iMaxNumMBX);
	pWMVDec->m_pmbmdZeroCBPCY = (CWMVMBMode*)ALIGN(pWMVDec->m_rgmbmd + (iMaxNumMB+2*iMaxNumMBX));
	pWMVDec->m_rgiQuanCoefACPred = (I16_WMV*)ALIGN(pWMVDec->m_pmbmdZeroCBPCY + 1);
	pWMVDec->m_rgiQuanCoefACPredTable = (I16_WMV**)ALIGN(pWMVDec->m_rgiQuanCoefACPred + iMaxNumMBX * 2 * BLOCK_SIZE_TIMES2_TIMES6);
	pWMVDec->m_pXMotion = (I16_WMV*)ALIGN(pWMVDec->m_rgiQuanCoefACPredTable + iMaxNumMBX * 2  * 36);
	pWMVDec->m_pXMotionC = (I16_WMV*)ALIGN(pWMVDec->m_pXMotion + iMaxNumMBX * iMaxNumMBY * 8);
	pWMVDec->m_rgmv1_EMB = (CMotionVector_X9_EMB*)ALIGN(pWMVDec->m_pXMotionC + iMaxNumMBX*iMaxNumMBY * 1);
	pWMVDec->m_pMotion = (UMotion_EMB*)ALIGN(pWMVDec->m_rgmv1_EMB + 5 * iMaxNumMB);
	pWMVDec->m_pMotionC = (UMotion_EMB*)ALIGN(pWMVDec->m_pMotion + (pWMVDec->m_uintNumMBX * (pWMVDec->m_uintNumMBY+1) * 4));

	memset(pWMVDec->m_rgmbmd,0,sizeof( CWMVMBMode ) *(iMaxNumMB+2*iMaxNumMBX));
	memset(pWMVDec->m_pMotion,0, sizeof(UMotion_EMB)*(pWMVDec->m_uintNumMBX * (pWMVDec->m_uintNumMBY+1) * 4));	
	memset(pWMVDec->m_pMotionC,0,sizeof(UMotion_EMB)*(pWMVDec->m_uintNumMBX * (pWMVDec->m_uintNumMBY+1)));
	
	pWMVDec->m_iNumOfQuanDctCoefForACPredPerRow = BLOCK_SIZE_TIMES2_TIMES6 * iMaxNumMBX; 
	pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmCurrQ;
    return ICERR_OK;
}

//*******************************************************
// initFrameWidthAndHeightDependentPointers:
// This routine is called by both constructor and WMVA sequence switch.
// In WMVA seq switch, it's called whenever the frame width or height is changed,
// but the buffers is only re-allocated when the current buffer is not large enough
// for the wmvMalloc(sizeof( sequence frame size.
// This function must be called after initFrameAreaDependentMemory().
//*******************************************************
Void_WMV initFrameWidthAndHeightDependentPointers(tWMVDecInternalMember *pWMVDec)
{
    U32_WMV iBlk = 0, imbY = 0;
    I32_WMV j,i ;
    I16_WMV **pDct; 
    I16_WMV *pDct1;
    I32_WMV iNumOfQuanDctCoefForACPredPerRow ;
    U32_WMV ii ;
    U32_WMV imby = 0;
    EMB_PMainLoop  * pPMainLoop = &pWMVDec->m_EMB_PMainLoop; 
    EMB_PBMainLoop *pMainPB = &pPMainLoop->PB[0];
    //EMB_PBMainLoop *pThreadPB = &pPMainLoop->PB[1];
  
    //if (pWMVDec->m_bX9)
	{
		pMainPB->pThreadIntraBlockRow0[0] = (I16_WMV *)(((U32_WMV) pWMVDec->m_pIntraBlockRowData + 7) & ~7);
        pMainPB->pThreadIntraBlockRow0[1] = pMainPB->pThreadIntraBlockRow0[0] + pWMVDec->m_uintNumMBX * 16 * 8;
        pMainPB->pThreadIntraBlockRow0[2] = pMainPB->pThreadIntraBlockRow0[1] + pWMVDec->m_uintNumMBX * 16 * 8;
        pMainPB->pThreadIntraMBRowU0[0] = pMainPB->pThreadIntraBlockRow0[2] + pWMVDec->m_uintNumMBX * 16 * 8;
        pMainPB->pThreadIntraMBRowU0[1] = pMainPB->pThreadIntraMBRowU0[0] + pWMVDec->m_uintNumMBX * 8 * 8;
        pMainPB->pThreadIntraMBRowV0[0] = pMainPB->pThreadIntraMBRowU0[1] + pWMVDec->m_uintNumMBX * 8 * 8;
        pMainPB->pThreadIntraMBRowV0[1] = pMainPB->pThreadIntraMBRowV0[0] + pWMVDec->m_uintNumMBX * 8 * 8;


        pWMVDec->m_pInterpY0 = (U8_WMV*) (((U32_WMV)pWMVDec->m_pInterpY0Buffer + 31) & ~31);
        pWMVDec->m_pInterpY1 = (U8_WMV*) (pWMVDec->m_pInterpY0 + MB_SIZE * MB_SIZE + 32);
        pWMVDec->m_pInterpU0 = (U8_WMV*) (pWMVDec->m_pInterpY1 + MB_SIZE * MB_SIZE + 32);
        pWMVDec->m_pInterpU1 = (U8_WMV*) (pWMVDec->m_pInterpU0 + BLOCK_SIZE * BLOCK_SIZE + 32);
        pWMVDec->m_pInterpV0 = (U8_WMV*) (pWMVDec->m_pInterpU1 + BLOCK_SIZE * BLOCK_SIZE + 32);
        pWMVDec->m_pInterpV1 = (U8_WMV*) (pWMVDec->m_pInterpV0 + BLOCK_SIZE * BLOCK_SIZE + 32);

        pWMVDec->m_pAlignedMultiresBuffer = (I8_WMV *)(((U32_WMV)pWMVDec->m_pMultiresBuffer + 31) & ~31);

        if (pWMVDec->m_bYUV411) {
            pWMVDec->m_pX9dctU = pWMVDec->m_pX9dct + pWMVDec->m_uintNumMB * 4 * 16;
            pWMVDec->m_pX9dctV = pWMVDec->m_pX9dctU + pWMVDec->m_uintNumMB * 24;
        } else {
            pWMVDec->m_pX9dctU = pWMVDec->m_pX9dct + 2*pWMVDec->m_uintNumMBX * 4 * 16;
            pWMVDec->m_pX9dctV = pWMVDec->m_pX9dctU + 2*pWMVDec->m_uintNumMBX * 16;
        }

        pWMVDec->m_pFieldMvY = pWMVDec->m_pFieldMvX + pWMVDec->m_uintNumMB * 4;
        pWMVDec->m_pFieldMvY_Pbuf = pWMVDec->m_pFieldMvX_Pbuf + (pWMVDec->m_uintNumMBX * (pWMVDec->m_uintNumMBY + 2)) * 4;

        pWMVDec->m_pB411InterpY = pWMVDec->m_pB411InterpX + pWMVDec->m_uintNumMB * 4;

        pWMVDec->m_pFieldMvY_FPred = pWMVDec->m_pFieldMvX_FPred + pWMVDec->m_uintNumMB * 4;
        pWMVDec->m_pFieldMvY_BPred = pWMVDec->m_pFieldMvX_BPred + pWMVDec->m_uintNumMB * 4;

    }
    
//zou 66
    for ( imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++){
        U32_WMV imbX = 0;
        for ( imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++){
            pWMVDec->m_rgmbmd [iBlk].m_bBoundary = ((imbY == pWMVDec->m_uintNumMBY - 1)<< 3) | ((imbX == pWMVDec->m_uintNumMBX - 1) << 2) | ((imbY == 0) << 1) | (imbX == 0);
            iBlk++;
        }
    }

    for ( j = (U32_WMV) Y_BLOCK1; j < (U32_WMV)V_BLOCK; j++)
        pWMVDec->m_pmbmdZeroCBPCY->m_rgbCodedBlockPattern2 [j] = 0;

    // for block-based rendering
    //pWMVDec->m_iWidthPrevYTimes8Minus8 = pWMVDec->m_iWidthPrevY * 8 - 8;
    //pWMVDec->m_iWidthPrevUVTimes4Minus4 = pWMVDec->m_iWidthPrevUV * 4 - 4;

    pWMVDec->m_iNumOfQuanDctCoefForACPredPerRow = BLOCK_SIZE_TIMES2_TIMES6 * pWMVDec->m_uintNumMBX; 

    pDct = pWMVDec->m_rgiQuanCoefACPredTable; 
    pDct1  = pWMVDec->m_rgiQuanCoefACPred;
    iNumOfQuanDctCoefForACPredPerRow = - pWMVDec->m_iNumOfQuanDctCoefForACPredPerRow;

    for ( ii = 0; ii < pWMVDec->m_uintNumMB; ii++) 
        pWMVDec->m_rgmbmd[ii].m_bBlkXformSwitchOn = FALSE;

    for ( imby = 0, i = 0; imby < 2; imby++) {
        U32_WMV imbx ;
        if (imby & 0x01)  // bTopRowBufferInOrder
            iNumOfQuanDctCoefForACPredPerRow = pWMVDec->m_iNumOfQuanDctCoefForACPredPerRow;

        for ( imbx = 0; imbx < pWMVDec->m_uintNumMBX; i++, imbx++) {
            I32_WMV ib ;
            for ( ib = 0;ib < 6;ib++) {
                switch (ib) {
                case 0://Y_BLOCK1:
                    pDct[0] = pWMVDec->m_pAvgQuanDctCoefDec;//letf ==0;
                    pDct[1] = pDct1 - BLOCK_SIZE_TIMES2_TIMES5;//left ==1
                    pDct[2] = pWMVDec->m_pAvgQuanDctCoefDec;//top
                    pDct[3] = pDct1 - iNumOfQuanDctCoefForACPredPerRow+ BLOCK_SIZE_TIMES2_TIMES2;//top
                    pDct[4] = pWMVDec->m_pAvgQuanDctCoefDec;//topleft
                    pDct[5] = pDct1 - iNumOfQuanDctCoefForACPredPerRow - BLOCK_SIZE_TIMES2_TIMES3;//topleft
                    break;
                case 1://Y_BLOCK2:
                    pDct[6]  = pDct[7] =  pDct1 - BLOCK_SIZE_TIMES2;
                    pDct[8] = pWMVDec->m_pAvgQuanDctCoefDec;
                    pDct[9] = pDct1- iNumOfQuanDctCoefForACPredPerRow + BLOCK_SIZE_TIMES2_TIMES2;
                    pDct[10] = pWMVDec->m_pAvgQuanDctCoefDec;       //(pWMVDec->m_uintNumMBX * 6 - 2)* 8 
                    pDct[11] = pDct1- iNumOfQuanDctCoefForACPredPerRow + BLOCK_SIZE_TIMES2;
                    break;
                case 2://Y_BLOCK3:
                    pDct[12] = pWMVDec->m_pAvgQuanDctCoefDec;
                    pDct[13] = pDct1 - BLOCK_SIZE_TIMES2_TIMES5 ;
                    pDct[14] = pDct[15] = pDct1 - BLOCK_SIZE_TIMES2_TIMES2;    //(pWMVDec->m_uintNumMBX * 6 - 2)* 8 
                    pDct[16] = pWMVDec->m_pAvgQuanDctCoefDec;
                    pDct[17] = pDct1 - BLOCK_SIZE_TIMES2_TIMES7 ;
                    break;
                case 3://Y_BLOCK4:
                    pDct[18] = pDct[19] = pDct1 - BLOCK_SIZE_TIMES2;
                    pDct[20] = pDct[21] = pDct1 - BLOCK_SIZE_TIMES2_TIMES2;    //(pWMVDec->m_uintNumMBX * 6 - 2)* 8 
                    pDct[22]= pDct[23] = pDct1 - BLOCK_SIZE_TIMES2_TIMES3;
                    break;
                case 4://U_BLOCK:
                    pDct[24] = pWMVDec->m_pAvgQuanDctCoefDecC;
                    pDct[25] = pDct1- BLOCK_SIZE_TIMES2_TIMES6 ;
                    pDct[26] = pWMVDec->m_pAvgQuanDctCoefDecC;      //(pWMVDec->m_uintNumMBX * 6 - 2)* 8 
                    pDct[27] = pDct1 - iNumOfQuanDctCoefForACPredPerRow ;
                    pDct[28] = pWMVDec->m_pAvgQuanDctCoefDecC;
                    pDct[29] = pDct1 - iNumOfQuanDctCoefForACPredPerRow - BLOCK_SIZE_TIMES2_TIMES6;
                    break;
                case 5://V_BLOCK:
                    pDct[30] = pWMVDec->m_pAvgQuanDctCoefDecC;
                    pDct[31] = pDct1 - BLOCK_SIZE_TIMES2_TIMES6 ;
                    pDct[32] = pWMVDec->m_pAvgQuanDctCoefDecC;
                    pDct[33] = pDct1 - iNumOfQuanDctCoefForACPredPerRow ;
                    pDct[34] = pWMVDec->m_pAvgQuanDctCoefDecC;
                    pDct[35] = pDct1 - iNumOfQuanDctCoefForACPredPerRow - BLOCK_SIZE_TIMES2_TIMES6 ;
                    break;
                }
                pDct1 += BLOCK_SIZE_TIMES2;
            }
            pDct += 36;
        }
    }

    pWMVDec->m_iWidthPrevYxBlkMinusBlk = (pWMVDec->m_iWidthPrevY * BLOCK_SIZE) - BLOCK_SIZE;


    if (pWMVDec->m_bBFrameOn)
    {
        pWMVDec->m_ppxliRef1YPlusExp = pWMVDec->m_ppxliRef1Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
    }

    pWMVDec->m_pYMotion = pWMVDec->m_pXMotion + pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY * 4;
    //pWMVDec->m_pYMotionC = pWMVDec->m_pXMotionC + pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY;

    // initialize it for error resilience
    pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmCurrQ;
}

//*****************************************
// Initialization of all Huffman tables
//*****************************************
I32_WMV initVlcTables(tWMVDecInternalMember *pWMVDec)
{    
    VResultCode vr;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufICBPCYDec, sm_HufICBPCYTable, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec, sm_HufPCBPCYTable, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_1, sm_HufPCBPCYTable_V9_1, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_2, sm_HufPCBPCYTable_V9_2, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_3, sm_HufPCBPCYTable_V9_3, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_4, sm_HufPCBPCYTable_V9_4, HUFFMAN_DECODE_ROOT_BITS);
    pWMVDec->m_pHufPCBPCYTables[0] = &pWMVDec->m_hufPCBPCYDec_V9_1;
    pWMVDec->m_pHufPCBPCYTables[1] = &pWMVDec->m_hufPCBPCYDec_V9_2;
    pWMVDec->m_pHufPCBPCYTables[2] = &pWMVDec->m_hufPCBPCYDec_V9_3;
    pWMVDec->m_pHufPCBPCYTables[3] = &pWMVDec->m_hufPCBPCYDec_V9_4;

    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV1, sm_HufMVTable_V9_MV1, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV2, sm_HufMVTable_V9_MV2, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV3, sm_HufMVTable_V9_MV3, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV4, sm_HufMVTable_V9_MV4, HUFFMAN_DECODE_ROOT_BITS);
    pWMVDec->m_pHufMVTables[0] = &pWMVDec->m_HufMVTable_V9_MV1;
    pWMVDec->m_pHufMVTables[1] = &pWMVDec->m_HufMVTable_V9_MV2;
    pWMVDec->m_pHufMVTables[2] = &pWMVDec->m_HufMVTable_V9_MV3;
    pWMVDec->m_pHufMVTables[3] = &pWMVDec->m_HufMVTable_V9_MV4;

    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_huf4x4PatternHighQPDec, sm_Huf4x4PatternHighQPTable, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_huf4x4PatternMidQPDec, sm_Huf4x4PatternMidQPTable, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_huf4x4PatternLowQPDec, sm_Huf4x4PatternLowQPTable, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMBXformTypeLowQPDec, sm_HufMBXformTypeLowQPTable, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMBXformTypeMidQPDec, sm_HufMBXformTypeMidQPTable, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMBXformTypeHighQPDec, sm_HufMBXformTypeHighQPTable, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufBlkXformTypeLowQPDec, sm_HufBlkXformTypeLowQPTable, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufBlkXformTypeMidQPDec, sm_HufBlkXformTypeMidQPTable, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufBlkXformTypeHighQPDec, sm_HufBlkXformTypeHighQPTable, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    if (pWMVDec->m_cvCodecVersion == WMVA) {
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode0, sm_HufInterlaceFrame4MvMBModeTable0, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode1, sm_HufInterlaceFrame4MvMBModeTable1, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode2, sm_HufInterlaceFrame4MvMBModeTable2, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode3, sm_HufInterlaceFrame4MvMBModeTable3, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        pWMVDec->m_pInterlaceFrame4MvMBModeTables[0] = &pWMVDec->m_hufInterlaceFrame4MvMBMode0;
        pWMVDec->m_pInterlaceFrame4MvMBModeTables[1] = &pWMVDec->m_hufInterlaceFrame4MvMBMode1;
        pWMVDec->m_pInterlaceFrame4MvMBModeTables[2] = &pWMVDec->m_hufInterlaceFrame4MvMBMode2;
        pWMVDec->m_pInterlaceFrame4MvMBModeTables[3] = &pWMVDec->m_hufInterlaceFrame4MvMBMode3;

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode0, sm_HufInterlaceFrame1MvMBModeTable0, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode1, sm_HufInterlaceFrame1MvMBModeTable1, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode2, sm_HufInterlaceFrame1MvMBModeTable2, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode3, sm_HufInterlaceFrame1MvMBModeTable3, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        pWMVDec->m_pInterlaceFrame1MvMBModeTables[0] = &pWMVDec->m_hufInterlaceFrame1MvMBMode0;
        pWMVDec->m_pInterlaceFrame1MvMBModeTables[1] = &pWMVDec->m_hufInterlaceFrame1MvMBMode1;
        pWMVDec->m_pInterlaceFrame1MvMBModeTables[2] = &pWMVDec->m_hufInterlaceFrame1MvMBMode2;
        pWMVDec->m_pInterlaceFrame1MvMBModeTables[3] = &pWMVDec->m_hufInterlaceFrame1MvMBMode3;

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_1, sm_HufCBPCYTable_Interlace_1, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_2, sm_HufCBPCYTable_Interlace_2, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_3, sm_HufCBPCYTable_Interlace_3, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_4, sm_HufCBPCYTable_Interlace_4, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_5, sm_HufCBPCYTable_Interlace_5, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_6, sm_HufCBPCYTable_Interlace_6, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_7, sm_HufCBPCYTable_Interlace_7, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_8, sm_HufCBPCYTable_Interlace_8, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        pWMVDec->m_pHufInterlaceCBPCYTables[0] = &pWMVDec->m_hufCBPCYDec_Interlace_1;
        pWMVDec->m_pHufInterlaceCBPCYTables[1] = &pWMVDec->m_hufCBPCYDec_Interlace_2;
        pWMVDec->m_pHufInterlaceCBPCYTables[2] = &pWMVDec->m_hufCBPCYDec_Interlace_3;
        pWMVDec->m_pHufInterlaceCBPCYTables[3] = &pWMVDec->m_hufCBPCYDec_Interlace_4;
        pWMVDec->m_pHufInterlaceCBPCYTables[4] = &pWMVDec->m_hufCBPCYDec_Interlace_5;
        pWMVDec->m_pHufInterlaceCBPCYTables[5] = &pWMVDec->m_hufCBPCYDec_Interlace_6;
        pWMVDec->m_pHufInterlaceCBPCYTables[6] = &pWMVDec->m_hufCBPCYDec_Interlace_7;
        pWMVDec->m_pHufInterlaceCBPCYTables[7] = &pWMVDec->m_hufCBPCYDec_Interlace_8;

        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV1, sm_HufMVTable_Progressive_MV1, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV2, sm_HufMVTable_Progressive_MV2, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV3, sm_HufMVTable_Progressive_MV3, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV4, sm_HufMVTable_Progressive_MV4, HUFFMAN_DECODE_ROOT_BITS);

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP1,sm_HufFieldPictureMVBP_1, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP2,sm_HufFieldPictureMVBP_2, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP3,sm_HufFieldPictureMVBP_3, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP4,sm_HufFieldPictureMVBP_4, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;

        pWMVDec->m_p4MVBPTables[0] = &pWMVDec->m_hufFieldPictureMVBP1;
        pWMVDec->m_p4MVBPTables[1] = &pWMVDec->m_hufFieldPictureMVBP2;
        pWMVDec->m_p4MVBPTables[2] = &pWMVDec->m_hufFieldPictureMVBP3;
        pWMVDec->m_p4MVBPTables[3] = &pWMVDec->m_hufFieldPictureMVBP4;

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP1, sm_HufInterlaceFrame2MVBP_1, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP2, sm_HufInterlaceFrame2MVBP_2, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP3, sm_HufInterlaceFrame2MVBP_3, HUFFMAN_DECODE_ROOT_BITS);
        vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP4, sm_HufInterlaceFrame2MVBP_4, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;

        pWMVDec->m_p2MVBPTables[0] = &pWMVDec->m_hufInterlaceFrame2MVBP1;
        pWMVDec->m_p2MVBPTables[1] = &pWMVDec->m_hufInterlaceFrame2MVBP2;
        pWMVDec->m_p2MVBPTables[2] = &pWMVDec->m_hufInterlaceFrame2MVBP3;
        pWMVDec->m_p2MVBPTables[3] = &pWMVDec->m_hufInterlaceFrame2MVBP4;
    }
    //_DCTDC
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCyDec_Talking,sm_HufDCTDCyTable_Talking, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCcDec_Talking,sm_HufDCTDCcTable_Talking, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCyDec_HghMt,sm_HufDCTDCyTable_HghMt, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCcDec_HghMt, sm_HufDCTDCcTable_HghMt, HUFFMAN_DECODE_ROOT_BITS);
  // _DCTAC
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_HghMt, sm_HufDCTACInterTable_HghMt, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_HghMt,sm_HufDCTACIntraTable_HghMt, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_Talking, sm_HufDCTACInterTable_Talking, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_Talking,sm_HufDCTACIntraTable_Talking, HUFFMAN_DECODE_ROOT_BITS);  
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_MPEG4, sm_HufDCTACInterTable_MPEG4, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_MPEG4,sm_HufDCTACIntraTable_MPEG4, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_HghRate, sm_HufDCTACInterTable_HghRate, HUFFMAN_DECODE_ROOT_BITS);
    vr |= Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_HghRate, sm_HufDCTACIntraTable_HghRate, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    return ICERR_OK;
lerror:
    return ICERR_MEMORY;
}

//************************************************************
// ResetConditionalVariablesForSequenceSwitch:
// Variables reset in this functions are usually present in the VOP header
// if other flags are turned on in the sequence header.
// Reset them here to prevent flags in previous sequence from being carried over
// to the next sequence.
// These variables are used in
// decodeVOPHeadProgressiveWMVA, decodeVOPHeadInterlaceV2, decodeVOPHeadFieldPicture,
// decodeSequenceHead_Advanced, and decodeVOPHead_WMV3.
//************************************************************
Void_WMV ResetConditionalVariablesForSequenceSwitch (tWMVDecInternalMember *pWMVDec)
{
    pWMVDec->m_bExtendedDeltaMvMode = FALSE;
    pWMVDec->m_bExplicitSeqQuantizer = pWMVDec->m_bExplicitFrameQuantizer = pWMVDec->m_bUse3QPDZQuantizer = FALSE;
    pWMVDec->m_bTopFieldFirst = TRUE;
    pWMVDec->m_bRepeatFirstField = FALSE;
    pWMVDec->m_iRepeatFrameCount = 0;
    pWMVDec->m_bProgressive420 = TRUE;
    pWMVDec->m_bInterpolateCurrentFrame = FALSE;
    pWMVDec->m_iPostProcLevel = -1;
    pWMVDec->m_iMVRangeIndex = 0;
    pWMVDec->m_iDeltaMVRangeIndex = 0;
    pWMVDec->m_iExtendedDMVX = pWMVDec->m_iExtendedDMVY = 0;
    pWMVDec->m_bLuminanceWarp = FALSE;
    pWMVDec->m_iFrameXformMode = XFORMMODE_8x8;
    pWMVDec->m_bMBXformSwitching = FALSE;
}



/************************************************************************************
SwitchSequence: Switch video sequence within one input stream.
Read malloc(sizeof( sequence header from input stream, update variables, function pointers, 
and memory allocation etc accordingly.
************************************************************************************/
I32_WMV SwitchSequence (tWMVDecInternalMember *pWMVDec, Bool_WMV bDecodeVOLHead)
{
    I32_WMV result;

#ifdef STABILITY
    if(pWMVDec->m_cvCodecVersion != WMVA)
	{
        return -1;
	}
#endif
        //reset necessary flags
    ResetConditionalVariablesForSequenceSwitch(pWMVDec);

    result = decodeSequenceHead_Advanced(pWMVDec, 0, 0, 0, 0, 0);
    if (ICERR_OK != result) {
		pWMVDec->sequence_errorflag = 1;
		pWMVDec->m_iMaxFrmWidthSrc = 0;
		pWMVDec->m_iMaxFrmHeightSrc = 0;
        return result;
    }
	if (pWMVDec->m_iMaxFrmWidthSrc * pWMVDec->m_iMaxFrmHeightSrc != pWMVDec->m_iMaxPrevSeqFrameArea)
		pWMVDec->m_bSeqFrameWidthOrHeightChanged = 1;

    if (pWMVDec->m_iMaxFrmWidthSrc * pWMVDec->m_iMaxFrmHeightSrc > pWMVDec->m_iMaxPrevSeqFrameArea)
    {
        pWMVDec->m_iMaxPrevSeqFrameArea = pWMVDec->m_iMaxFrmWidthSrc * pWMVDec->m_iMaxFrmHeightSrc;
        FreeFrameAreaDependentMemory(pWMVDec);
        result = initFrameAreaDependentMemory(pWMVDec, pWMVDec->m_iMaxFrmWidthSrc, pWMVDec->m_iMaxFrmHeightSrc);
        if (ICERR_OK != result)
		{
            return result;
		}
    } 

    return ICERR_OK;
}

/************************************************************************************
  SwapMVTables : switches between tables used in interlace and progressive decoding
  based on frame type
************************************************************************************/
Void_WMV SwapMVTables (tWMVDecInternalMember *pWMVDec)
{
    if (pWMVDec->m_bInterlaceV2) 
    {
        pWMVDec->m_pHufMVTables[0] = &pWMVDec->m_HufMVTable_Progressive_MV1;
        pWMVDec->m_pHufMVTables[1] = &pWMVDec->m_HufMVTable_Progressive_MV2;
        pWMVDec->m_pHufMVTables[2] = &pWMVDec->m_HufMVTable_Progressive_MV3;
        pWMVDec->m_pHufMVTables[3] = &pWMVDec->m_HufMVTable_Progressive_MV4;
    }
    else { // progressive tables

        pWMVDec->m_pHufMVTables[0] = &pWMVDec->m_HufMVTable_V9_MV1;
        pWMVDec->m_pHufMVTables[1] = &pWMVDec->m_HufMVTable_V9_MV2;
        pWMVDec->m_pHufMVTables[2] = &pWMVDec->m_HufMVTable_V9_MV3;
        pWMVDec->m_pHufMVTables[3] = &pWMVDec->m_HufMVTable_V9_MV4;
    }
}

extern int EXPANDY_REFVOP, EXPANDUV_REFVOP;
Void_WMV computePars_WMVA (tWMVDecInternalMember *pWMVDec)
{
    pWMVDec->EXPANDY_REFVOP = EXPANDY_REFVOP;
    pWMVDec->EXPANDUV_REFVOP = EXPANDUV_REFVOP;

    //EXPANDY_REFVOP = pWMVDec->EXPANDY_REFVOP;
    //EXPANDUV_REFVOP = pWMVDec->EXPANDUV_REFVOP;


    //if (pWMVDec->m_cvCodecVersion != MP4S) {
    pWMVDec->m_iWidthYRepeatPad = pWMVDec->m_iWidthY;
    pWMVDec->m_iWidthUVRepeatPad = pWMVDec->m_iWidthUV;
    pWMVDec->m_iHeightYRepeatPad = pWMVDec->m_iHeightY;
    pWMVDec->m_iHeightUVRepeatPad = pWMVDec->m_iHeightUV;

    if (pWMVDec->m_bAdvancedProfile) {
            pWMVDec->m_iWidthUVRepeatPad = (pWMVDec->m_iFrmWidthSrc + 1) >> 1;
            pWMVDec->m_iWidthYRepeatPad = pWMVDec->m_iWidthUVRepeatPad * 2;
            pWMVDec->m_iHeightUVRepeatPad = (pWMVDec->m_iFrmHeightSrc + 1) >> 1;
            pWMVDec->m_iHeightYRepeatPad = pWMVDec->m_iHeightUVRepeatPad * 2;
    }

    //}
    //else {
    //    pWMVDec->m_iWidthYRepeatPad = pWMVDec->m_iFrmWidthSrc;
    //    pWMVDec->m_iWidthUVRepeatPad = pWMVDec->m_iFrmWidthSrc/2;
    //    pWMVDec->m_iHeightYRepeatPad = pWMVDec->m_iFrmHeightSrc;
    //    pWMVDec->m_iHeightUVRepeatPad = pWMVDec->m_iFrmHeightSrc/2;
    //}
    pWMVDec->m_iWidthYPlusExp = pWMVDec->m_iWidthYRepeatPad + pWMVDec->EXPANDY_REFVOP;
    pWMVDec->m_iWidthUVPlusExp = pWMVDec->m_iWidthUVRepeatPad + pWMVDec->EXPANDUV_REFVOP;

    pWMVDec->m_bMBAligned = (pWMVDec->m_iWidthY == pWMVDec->m_iFrmWidthSrc && pWMVDec->m_iHeightY == pWMVDec->m_iFrmHeightSrc);
    // see if the size is multiples of MB_SIZE

    pWMVDec->m_uintNumMBX = pWMVDec->m_iWidthY >> 4; // divided by 16, MB size
    pWMVDec->m_uintNumMBY = pWMVDec->m_iHeightY >> 4;
    pWMVDec->m_uintNumMB = pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY;
    pWMVDec->m_uiRightestMB = pWMVDec->m_uintNumMBX - 1;

    // for motion compensation
	if( pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP ) 
	{
		pWMVDec->m_iWidthPrevY = pWMVDec->m_frameBufStrideY;
		pWMVDec->m_iWidthPrevUV = pWMVDec->m_frameBufStrideUV;
	}
	else
	{
		pWMVDec->m_iWidthPrevY = pWMVDec->m_iWidthY + (pWMVDec->EXPANDY_REFVOP << 1);
		pWMVDec->m_iWidthPrevUV = pWMVDec->m_iWidthUV + (pWMVDec->EXPANDUV_REFVOP << 1);
	}
    pWMVDec->m_iHeightPrevY = pWMVDec->m_iHeightY + (pWMVDec->EXPANDY_REFVOP << 1);
    pWMVDec->m_iHeightPrevUV = pWMVDec->m_iHeightUV + (pWMVDec->EXPANDUV_REFVOP << 1);

    // for update
    pWMVDec->m_iWidthPrevYXExpPlusExp = pWMVDec->m_iWidthPrevY * pWMVDec->EXPANDY_REFVOP + pWMVDec->EXPANDY_REFVOP;
    pWMVDec->m_iWidthPrevUVXExpPlusExp = pWMVDec->m_iWidthPrevUV * pWMVDec->EXPANDUV_REFVOP + pWMVDec->EXPANDUV_REFVOP;

    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
    pWMVDec->m_ppxliRef1YPlusExp = pWMVDec->m_ppxliRef1Y + pWMVDec->m_iWidthPrevYXExpPlusExp;

    // for motion estimation
    pWMVDec->m_iMBSizeXWidthPrevY = pWMVDec->m_iWidthPrevY << 4; // * MB_SIZE
    pWMVDec->m_iBlkSizeXWidthPrevUV = pWMVDec->m_iWidthPrevUV << 3; // * BLOCK_SIZE

    //Zone_VLC
#if 0
    pWMVDec->m_rgiEndIndexOfCurrZone [0] = END1STZONE;
    pWMVDec->m_rgiEndIndexOfCurrZone [1] = END2NDZONE;
    pWMVDec->m_rgiEndIndexOfCurrZone [2] = 64;
    pWMVDec->m_bInterZoneVLCTable = FALSE;
#endif

    SetupMultiResParams(pWMVDec);

    //slice
    pWMVDec->m_iSliceBufSize = ((pWMVDec->m_iHeightY >> 4) + 1);

}


/****************************************************************************************
SetMVRangeFlag : set MV range and flags
****************************************************************************************/
Void_WMV SetMVRangeFlag (tWMVDecInternalMember *pWMVDec, I32_WMV iRangeIndex)
{
    I32_WMV  iXRange[] = { 9, 10, 12, 13 };
    I32_WMV  iYRange[] = { 8, 9, 10, 11 };

    pWMVDec->m_iMVRangeIndex = iRangeIndex;
    pWMVDec->m_iLogXRange = iXRange[iRangeIndex];
    pWMVDec->m_iLogYRange = iYRange[iRangeIndex];
    pWMVDec->m_iXMVRange = 1 << (pWMVDec->m_iLogXRange - 1);
    pWMVDec->m_iYMVRange = 1 << (pWMVDec->m_iLogYRange - 1);
    pWMVDec->m_iXMVFlag = 2 * pWMVDec->m_iXMVRange - 1;
    pWMVDec->m_iYMVFlag = 2 * pWMVDec->m_iYMVRange - 1;
}

//Loopfilter etc

tWMVDecodeStatus  Plane_init (tYUV420Frame_WMV * pFramePlane, I32_WMV iSizeYplane, I32_WMV iSizeUVplane, 
					I32_WMV iPad, tWMVDecInternalMember *pWMVDec, I32_WMV iPlaneWidth, I32_WMV iPlaneHeight)
{
    U8_WMV *ppxlTmp;
    I32_WMV uv_offset;
    I32_WMV iYSize, iUSize, iVSize;

    memset(pFramePlane, 0, sizeof (tYUV420Frame_WMV) ); // protect low memory issues when first fails and second tries to be freed.
	iYSize = (iSizeYplane  + (iPad<<1) + 32) * sizeof (U8_WMV);
	iUSize = (iSizeUVplane + iPad      + 32) * sizeof (U8_WMV);
	iVSize = iUSize;

	if( pWMVDec->m_MemOperator.Type == NORMAL_MEM_OP || pWMVDec->m_MemOperator.Type == NONE_MEM_OP) 
	{
		pFramePlane->m_pucYPlane_Unaligned = ppxlTmp = (U8_WMV *) wmvMalloc ( pWMVDec, iYSize + iUSize + iVSize, DHEAP_FRAMES);
		pFramePlane->m_pCurrPlaneBase = ppxlTmp;
		if (ppxlTmp == NULL_WMV)
			return WMV_BadMemory;

		//Y
		pFramePlane->m_pucYPlane  = (U8_WMV *)(((U32_WMV)ppxlTmp + iPad + 31) & ~31);

		//U
		ppxlTmp  = pFramePlane->m_pucYPlane_Unaligned + iYSize;
		ppxlTmp += iPad>>1;
		uv_offset = ((U32_WMV)ppxlTmp) & 31 ;
		if(uv_offset != 0)
			uv_offset = 32 - uv_offset;
		pFramePlane->m_pucUPlane  = (U8_WMV *)((U32_WMV)ppxlTmp + uv_offset);

		//V
		ppxlTmp  = pFramePlane->m_pucYPlane_Unaligned + iYSize + iUSize;
		ppxlTmp += iPad>>1;
		uv_offset = ((U32_WMV)ppxlTmp) & 31 ;
		if(uv_offset != 0)
			uv_offset = 32 - uv_offset;

		pFramePlane->m_pucVPlane  = (U8_WMV *)((U32_WMV)ppxlTmp + uv_offset);

	    return WMV_Succeeded;
	}

    return WMV_Succeeded;
}

Void_WMV   m_InitIDCT_Dec(tWMVDecInternalMember *pWMVDec, Bool_WMV fUseTransposedMatrix)
{
        pWMVDec->m_bRotatedIdct = TRUE;
        pWMVDec->m_pIntraIDCT_Dec = g_IDCTDec_WMV3_Fun;
        pWMVDec->m_pInterIDCT_Dec = g_IDCTDec16_WMV3;
        pWMVDec->m_pInter8x4IDCT_Dec = g_8x4IDCTDec_WMV3;
        pWMVDec->m_pInter4x8IDCT_Dec = g_4x8IDCTDec_WMV3;
        pWMVDec->m_pInter4x4IDCT_Dec = g_4x4IDCTDec_WMV3;
        pWMVDec->m_pIntraX9IDCT_Dec = g_IDCTDec16_WMV3;     


        pWMVDec->m_iACColPredOffset = 0;
        pWMVDec->m_iACRowPredOffset = 8;
        pWMVDec->m_iACColPredShift = 0;
        pWMVDec->m_iACRowPredShift = 3;
}

Void_WMV decide_vc1_routines (tWMVDecInternalMember *pWMVDec)
{
	EXPANDY_REFVOP = pWMVDec->EXPANDY_REFVOP;
	EXPANDUV_REFVOP = pWMVDec->EXPANDUV_REFVOP;
	if (pWMVDec->m_bAdvancedProfile) 
	{
		pWMVDec->pRepeatRef0Y = g_RepeatRef0Y_AP; 
		pWMVDec->pRepeatRef0UV = g_RepeatRef0UV_AP;
	}
	else
	{
		pWMVDec->pRepeatRef0Y = g_RepeatRef0Y; 
		pWMVDec->pRepeatRef0UV = g_RepeatRef0UV;
	}

#if  defined(VOARMV4)||defined(VOARMV6)||defined(VOARMV7)
    pWMVDec->m_pFilterHorizontalEdge = &g_FilterHorizontalEdgeV9_ARMV4;
    pWMVDec->m_pFilterVerticalEdge = &g_FilterVerticalEdgeV9_ARMV4;
#else
    pWMVDec->m_pFilterHorizontalEdge = &g_FilterHorizontalEdgeV9_C;
    pWMVDec->m_pFilterVerticalEdge = &g_FilterVerticalEdgeV9_C;
#endif
}

Void_WMV m_InitFncPtrAndZigzag(tWMVDecInternalMember *pWMVDec)
{
    //pWMVDec->m_bRefreshDisplay_AllMB_Enable = TRUE_WMV;
    // At the decoder side, Both Intra and Inter Frame might use Rotated matrix depending on which idct is called.
    pWMVDec->m_p8x4ZigzagInv = grgi8x4ZigzagInv;
    pWMVDec->m_p4x8ZigzagInv = grgi4x8ZigzagInv;
    pWMVDec->m_p8x4ZigzagInvRotated = grgi8x4ZigzagInvRotated;
    pWMVDec->m_p4x8ZigzagInvRotated = grgi4x8ZigzagInvRotated;
    pWMVDec->m_p4x4ZigzagInv = grgi4x4ZigzagInv;
    pWMVDec->m_p4x4ZigzagInvRotated = grgi4x4ZigzagInvRotated;

    pWMVDec->m_pZigzagInv_I = grgiZigzagInv_NEW_I;
    pWMVDec->m_pZigzagInvRotated_I = grgiZigzagInvRotated_NEW_I;
    pWMVDec->m_pHorizontalZigzagInv = grgiHorizontalZigzagInv_NEW;
    pWMVDec->m_pVerticalZigzagInv = grgiVerticalZigzagInv_NEW;
    //Decoder Only
    pWMVDec->m_pHorizontalZigzagInvRotated = grgiHorizontalZigzagInvRotated_NEW;
    pWMVDec->m_pVerticalZigzagInvRotated = grgiVerticalZigzagInvRotated_NEW;

    pWMVDec->m_pZigzagInvRotated = grgiZigzagInvRotated_NEW;
    pWMVDec->m_pZigzagInv = grgiZigzagInv_NEW; 

	pWMVDec->m_bRotatedIdct = 1;
	pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInvRotated;
	pWMVDec->m_p4x4ZigzagScanOrder = pWMVDec->m_p4x4ZigzagInvRotated;
	pWMVDec->m_p8x4ZigzagScanOrder = pWMVDec->m_p8x4ZigzagInvRotated;
	pWMVDec->m_p4x8ZigzagScanOrder = pWMVDec->m_p4x8ZigzagInvRotated;
	pWMVDec->m_pHorizontalZigzagScanOrder = pWMVDec->m_pHorizontalZigzagInvRotated;
	pWMVDec->m_pVerticalZigzagScanOrder = pWMVDec->m_pVerticalZigzagInvRotated;

	EMB_InitZigZagTable_SSIMD(pWMVDec->m_p8x4ZigzagScanOrder, pWMVDec->m_p8x4ZigzagSSIMD, 32);
	EMB_InitZigZagTable_SSIMD(pWMVDec->m_p4x4ZigzagScanOrder, pWMVDec->m_p4x4ZigzagSSIMD, 16);
}

//******************************************************************************************************
//  Implement wmv memallo and free and heap stack tracing functions.            
//********************************************************************************************************

#define WMV_TRACE_HEAP          0x01
#define WMV_TRACE_STACK         0x02
#define WMV_TRACE_HEAP_DIST     0x04

/*
 * Turn on tracing in compile-time, switch the following macros
 * Testing can turn on tracing on run-time by calling WMVSetTraceFlag functions.
 */
#ifdef MEMLEAK_CHECK
int mem_times = 0;
voMemCheck  g_memcheck[4096]= {0};
#endif

void *wmvMalloc(tWMVDecInternalMember *pWMVDec, unsigned int size, int category)
{
	if(pWMVDec->m_memOp) {
		VO_MEM_INFO voMemInfo; 
		voMemInfo.Size = size;
		pWMVDec->m_memOp->Alloc(VO_INDEX_DEC_WMV, &voMemInfo);
		return (void *)voMemInfo.VBuffer; 
	}
	else {
		void* tmp = (void *)malloc(size);
#ifdef MEMLEAK_CHECK
		g_memcheck[mem_times].alloc_add = (int)tmp;
		g_memcheck[mem_times++].alloc_size = size;
#endif
		return tmp;//(void *)malloc(size);
	}
}

void   wmvFree(tWMVDecInternalMember *pWMVDec, void *free_ptr)
{
#ifdef MEMLEAK_CHECK
	int i=0;
	for(i=0;i<4096;i++)
	{
		if(g_memcheck[i].alloc_add == (int)free_ptr)
			g_memcheck[i].free_add = (int)free_ptr;
	}
#endif

	if(pWMVDec->m_memOp) {
		pWMVDec->m_memOp->Free(VO_INDEX_DEC_WMV, free_ptr); 
        free_ptr = NULL;
	}
	else {
	    free(free_ptr);
        free_ptr = NULL;
	}
}




WMV_EXTERN_C void * MallocHandle(VO_MEM_OPERATOR *memOp, size_t pSize, char *pFile, int pLine, int category)
{
    void *new_ptr;

	if(memOp) {
		VO_MEM_INFO voMemInfo; 
		voMemInfo.Size = pSize;
		memOp->Alloc(VO_INDEX_DEC_WMV, &voMemInfo); 
		new_ptr = (U8_WMV *)voMemInfo.VBuffer; 
	}
	else {
		new_ptr = malloc(pSize);
	}

    return new_ptr;
}
