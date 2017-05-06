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
#ifdef _WMV_TARGET_X86_
#include "cpudetect.h"
#endif // _WMV_TARGET_X86_
#ifdef macintosh
#include "cpudetect_mac.h"
#include "motioncomp_altivec.h"
#endif
#include "tables_wmv.h"

#include "strmdec_wmv.h"
#include "motioncomp_wmv.h"
#include "idctdec_wmv2.h"
#include "repeatpad_wmv.h"
#include "postfilter_wmv.h"

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
tWMVDecodeStatus setVlcTableInfo_WMVA(tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus InitDataMembers_WMVA (tWMVDecInternalMember *pWMVDec, 
    I32_WMV iWidthSource,
    I32_WMV iHeightSource);

#ifdef WMVIMAGE_V2
#include "effects_wmv.h"
#endif

#ifdef XBOXGPU
#include "xboxdec.h"
#include "xboxfunc.h"
#else
#ifdef XBOXGPU_NOGPU_DISPLAY
extern Void_WMV InitOverlay(int Width,int Height);
#endif
#endif

extern I32_WMV WMVideoIsSpriteMode (HWMVDecoder hWMVDecoder);


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

#ifndef DISABLE_UNUSED_CODE_INIT
Void_WMV computePars (tWMVDecInternalMember *pWMVDec);
#endif

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


#ifdef _WMMX_
tWMVDecodeStatus  Plane_init_MEM (tWMVDecInternalMember *pWMVDec, tYUV420Frame_WMV * pFramePlane, I32_WMV iSizeYplane, I32_WMV iSizeUVplane, CodecVersion vCodecVersion);
#endif

tWMVDecodeStatus CombinedLevelRunInit(tWMVDecInternalMember *pWMVDec, U8_WMV * pRun, I8_WMV * pLevel, I32_WMV buffer_size, 
                                      CDCTTableInfo_Dec * pInterDCTTableInfo_Dec) 
{                                                                      

    I16_WMV * pCombLevelRun;
    I32_WMV i;

    buffer_size ++;

#ifdef XDM
	pCombLevelRun = pInterDCTTableInfo_Dec->combined_levelrun = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += 2*buffer_size;
#else
    pCombLevelRun = pInterDCTTableInfo_Dec->combined_levelrun=(I16_WMV *)wmvMalloc(pWMVDec, 2*buffer_size, DHEAP_HUFFMAN);
    if (pCombLevelRun ==  NULL) {
        return WMV_BadMemory;
    }
#endif 

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


//I32_WMV spWMVDec->m_iIDCTDecCount = 0;

//#ifndef _WMV9AP_
tWMVDecodeStatus setVlcTableInfo(tWMVDecInternalMember *pWMVDec)
{

    tWMVDecodeStatus status=WMV_Succeeded;

    /// Set tables for High Motion
    setTable(
        pWMVDec->InterDCTTableInfo_Dec_HghMt,
        &pWMVDec->m_hufDCTACInterDec_HghMt,
        UVTCOEF_ESCAPE_HghMt,
        NUMOFCODESINNOTLASTINTERMinus1_HghMt,
        //STARTINDEXOFLASTRUN_HghMt, 
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
        //STARTINDEXOFLASTRUN_HghMt, 
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
        //STARTINDEXOFLASTRUN_Talking, 
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
        //STARTINDEXOFLASTRUN_Talking, 
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
        //STARTINDEXOFLASTRUN_MPEG4, 
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
        //STARTINDEXOFLASTRUN_MPEG4, 
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


    pWMVDec->m_pHufMVDec_Set[0] = &pWMVDec->m_hufMVDec_Talking;
    pWMVDec->m_pHufMVDec_Set[1] = &pWMVDec->m_hufMVDec_HghMt;
    pWMVDec->m_puMvFromIndex_Set[0] = sm_uXMvFromIndex_Talking;
    pWMVDec->m_puMvFromIndex_Set[1] = sm_uYMvFromIndex_Talking;
    pWMVDec->m_puMvFromIndex_Set[2] = sm_uXMvFromIndex_HghMt;
    pWMVDec->m_puMvFromIndex_Set[3] = sm_uYMvFromIndex_HghMt;

    return status;

}

//#endif //_WMV9AP_

Void_WMV freeFrame_MEM(tWMVDecInternalMember *pWMVDec, tYUV420Frame_WMV* pFrm)
{
    if (pFrm) {
        //FREE_PTR(pWMVDec, pFrm->m_pucYPlane_Unaligned);
        //FREE_PTR(pWMVDec, pFrm->m_pucUPlane_Unaligned);
        //FREE_PTR(pWMVDec, pFrm->m_pucVPlane_Unaligned);
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO freeFrame_MEM  :%x",(U32_WMV)pFrm );
#endif
        FREE_PTR(pWMVDec, pFrm);
    }
}

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

#if defined(WANT_HEAP_MEASURE)
#   define DEBUG_HEAP_SUB_FRAME(a)                                          \
{                                                                       \
    I32_WMV iWidthMBAligned = (pWMVDec->m_iFrmWidthSrc + 15) & ~15;     \
    I32_WMV iHeightMBAligned = (pWMVDec->m_iFrmHeightSrc + 15) & ~15;   \
    I32_WMV iSizeYplane = (iWidthMBAligned + pWMVDec->EXPANDY_REFVOP * 2) * (iHeightMBAligned + pWMVDec->EXPANDY_REFVOP * 2);                     \
    I32_WMV iSizeUVplane = ((iWidthMBAligned >> 1) + pWMVDec->EXPANDUV_REFVOP * 2) * ((iHeightMBAligned  >> 1)+ pWMVDec->EXPANDUV_REFVOP * 2);    \
    DEBUG_HEAP_SUB( 0, a * sizeof(tYUV420Frame_WMV) );                                                                             \
    DEBUG_HEAP_SUB( 1, a * (((iSizeYplane + 32)*sizeof(U8_WMV)) + 2*((iSizeUVplane + 32) * sizeof (U8_WMV)) ) );                \
}
#else
#   define DEBUG_HEAP_SUB_FRAME(a)
#endif


tWMVDecodeStatus  VodecDestruct (tWMVDecInternalMember *pWMVDec)
{
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO VodecDestruct START");
#endif

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pBMPINFOHEADER :%x",(U32_WMV)pWMVDec->m_pBMPINFOHEADER);
#endif
	if(pWMVDec->m_pBMPINFOHEADER){
        wmvFree (pWMVDec, pWMVDec->m_pBMPINFOHEADER);
		pWMVDec->m_pBMPINFOHEADER = NULL;
	}

#   ifdef XBOXGPU
        if(pWMVDec->m_bXBOXUsingGPU)
            FreeGPUDecoder(&pWMVDec->m_gpuDec,pWMVDec);
#   endif

#ifdef WMVIMAGE_V2
    //if (pWMVDec->m_pEffects)
		//WMVImage_EffectRelease(pWMVDec, pWMVDec->m_pEffects);
	if (pWMVDec->m_pEffects){
        wmvFree(pWMVDec, pWMVDec->m_pEffects->m_pOuterLeftX);
        wmvFree(pWMVDec, pWMVDec->m_pEffects->m_pInnerLeftX);
        wmvFree(pWMVDec, pWMVDec->m_pEffects->m_pOuterRightX);
        wmvFree(pWMVDec, pWMVDec->m_pEffects->m_pInnerRightX);
        wmvFree(pWMVDec, pWMVDec->m_pEffects->m_pFlipFlag);
        wmvFree(pWMVDec, pWMVDec->m_pEffects->m_pFlipX);
        wmvFree(pWMVDec, pWMVDec->m_pEffects->m_pFlipYRatio);
        wmvFree(pWMVDec, pWMVDec->m_pEffects);
	}
	//

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pAffineCoefficients :%x",(U32_WMV)pWMVDec->m_pAffineCoefficients);
#endif
    FREE_PTR(pWMVDec, pWMVDec->m_pAffineCoefficients);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pEffectParameters :%x",(U32_WMV)pWMVDec->m_pEffectParameters);
#endif
    FREE_PTR(pWMVDec, pWMVDec->m_pEffectParameters);
#endif

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pInputBitstream :%x",(U32_WMV)pWMVDec->m_pInputBitstream);
#endif
	if (pWMVDec ->m_pInputBitstream){
       FREE_PTR(pWMVDec, pWMVDec ->m_pInputBitstream);
	}

#ifndef _WMV9AP_
    FREE_PTR (pWMVDec, pWMVDec->m_riReconBuf); 
    FREE_PTR (pWMVDec, pWMVDec->m_riPixelError); 
#endif

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgErrorBlock :%x",(U32_WMV)pWMVDec->m_rgErrorBlock);
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_rgErrorBlock);

    if (pWMVDec->m_cvCodecVersion >= WMV3)
    {
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pDiffMV :%x",(U32_WMV)pWMVDec->m_pDiffMV);
#endif
        FREE_PTR(pWMVDec, pWMVDec->m_pDiffMV);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_ppxliFieldMBBuffer :%x",(U32_WMV)pWMVDec->m_ppxliFieldMBBuffer);
#endif
        FREE_PTR(pWMVDec, pWMVDec->m_ppxliFieldMBBuffer);
    }

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_puiFirstMBRowOfSlice :%x",(U32_WMV)pWMVDec->m_puiFirstMBRowOfSlice);
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_puiFirstMBRowOfSlice);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pbStartOfSliceRowOrig :%x",(U32_WMV)pWMVDec->m_pbStartOfSliceRowOrig);
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_pbStartOfSliceRowOrig);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_puiNumBytesOfSliceWMVA :%x",(U32_WMV)pWMVDec->m_puiNumBytesOfSliceWMVA);
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_puiNumBytesOfSliceWMVA);

    if (pWMVDec->m_cvCodecVersion == WMVA) {
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_ppxliIntraRowY :%x",(U32_WMV)pWMVDec->m_ppxliIntraRowY);
#endif
        FREE_PTR (pWMVDec, pWMVDec->m_ppxliIntraRowY);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_ppxliIntraRowU :%x",(U32_WMV)pWMVDec->m_ppxliIntraRowU);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_ppxliIntraRowU);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_ppxliIntraRowV :%x",(U32_WMV)pWMVDec->m_ppxliIntraRowV);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_ppxliIntraRowV);
    }

#   ifdef _EMB_WMV3_
#       ifndef WMV_OPT_MOTIONCOMP_SH3DSP

#           ifdef _EMB_SSIMD64_
            if( pWMVDec->m_rgMotionCompBuffer_EMB )
                pWMVDec->m_rgMotionCompBuffer_EMB -= *(pWMVDec->m_rgMotionCompBuffer_EMB - 1);
#           endif
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgMotionCompBuffer_EMB :%x",(U32_WMV)pWMVDec->m_rgMotionCompBuffer_EMB);
#endif
            FREE_PTR (pWMVDec, pWMVDec->m_rgMotionCompBuffer_EMB);

#       endif
#   endif

#ifdef VIDEO_MEM_SHARE
	if(pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP) {
		int i;
		pWMVDec->m_videoMemShare.Uninit(VO_INDEX_DEC_WMV);
		for(i = 0; i < pWMVDec->m_frameBufNum; i++) {
			tYUV420Frame_WMV *pTmpBuff = pWMVDec->m_memManageFIFO.img_seq_backup[i];
			if(pTmpBuff) {
#ifdef VO_LOG_INFO_MemoryFree
				VOLOGI("\n VO_LOG_INFO frame buff %d:%x",i,(U32_WMV)pTmpBuff);
#endif
				FREE_PTR(pWMVDec, pTmpBuff);
				pWMVDec->m_memManageFIFO.img_seq_backup[i] = NULL;
			}
		}
	}
	else {
#endif

    if (pWMVDec->m_bFrameInPHYSMEM)
        freeFrame_MEM (pWMVDec, pWMVDec->m_pfrmCurrQ);
    else
    {
        freeFrame (pWMVDec->m_pfrmCurrQ, pWMVDec);
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pfrmCurrQ :%x",(U32_WMV)pWMVDec->m_pfrmCurrQ);
#endif
        FREE_PTR(pWMVDec, pWMVDec->m_pfrmCurrQ);
    }

    freeFrame (pWMVDec->m_pfrmRef0Q, pWMVDec);
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pfrmRef0Q :%x",(U32_WMV)pWMVDec->m_pfrmRef0Q);
#endif
    FREE_PTR(pWMVDec, pWMVDec->m_pfrmRef0Q);

        
    pWMVDec->m_pfrmCurrQ = pWMVDec->m_pfrmRef0Q = NULL_WMV;

    if (!pWMVDec->m_bSpriteMode) {
			freeFrame (pWMVDec->m_pfrmRef1Q, pWMVDec);
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pfrmRef1Q :%x",(U32_WMV)pWMVDec->m_pfrmRef1Q);
#endif
			FREE_PTR(pWMVDec, pWMVDec->m_pfrmRef1Q);

			freeFrame (pWMVDec->m_pfrmMultiresQ, pWMVDec);
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pfrmMultiresQ :%x",(U32_WMV)pWMVDec->m_pfrmMultiresQ);
#endif
			FREE_PTR(pWMVDec, pWMVDec->m_pfrmMultiresQ);
#   ifdef XBOXGPU
        }     
#   endif    
		pWMVDec->m_pfrmMultiresQ = pWMVDec->m_pfrmRef1Q = NULL_WMV;
    }

    DEBUG_HEAP_SUB_FRAME(2);

    freeFrame (pWMVDec->m_pfrmPostQ, pWMVDec);
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pfrmPostQ :%x",(U32_WMV)pWMVDec->m_pfrmPostQ);
#endif
    FREE_PTR(pWMVDec, pWMVDec->m_pfrmPostQ);

	freeFrame (pWMVDec->m_pfrmPostPrevQ, pWMVDec);
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pfrmPostPrevQ :%x",(U32_WMV)pWMVDec->m_pfrmPostPrevQ);
#endif
    FREE_PTR(pWMVDec, pWMVDec->m_pfrmPostPrevQ);
#   ifdef XBOXGPU
        }
#   endif 
       
    pWMVDec->m_pfrmPostQ = pWMVDec->m_pfrmPostPrevQ = NULL_WMV;

    if (pWMVDec->m_bYUV411) {
		freeFrame (pWMVDec->m_pfrmYUV411toYUV420Q, pWMVDec);
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pfrmYUV411toYUV420Q :%x",(U32_WMV)pWMVDec->m_pfrmYUV411toYUV420Q);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_pfrmYUV411toYUV420Q);
		pWMVDec->m_pfrmYUV411toYUV420Q = NULL_WMV;
	}
#ifdef VIDEO_MEM_SHARE
	}
#endif

    DEBUG_HEAP_SUB_FRAME(1);

#   ifdef XBOXGPU
        if(!pWMVDec->m_bXBOXUsingGPU)
#   endif    

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgmv :%x",(U32_WMV)pWMVDec->m_rgmv);
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_rgmv);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgmbmd :%x",(U32_WMV)pWMVDec->m_rgmbmd);
#endif
	FREE_PTR (pWMVDec, pWMVDec->m_rgmbmd);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pmbmdZeroCBPCY :%x",(U32_WMV)pWMVDec->m_pmbmdZeroCBPCY);
#endif
	FREE_PTR (pWMVDec, pWMVDec->m_pmbmdZeroCBPCY);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pAvgQuanDctCoefDec :%x",(U32_WMV)pWMVDec->m_pAvgQuanDctCoefDec);
#endif
	FREE_PTR (pWMVDec, pWMVDec->m_pAvgQuanDctCoefDec);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pAvgQuanDctCoefDecC :%x",(U32_WMV)pWMVDec->m_pAvgQuanDctCoefDecC);
#endif
	FREE_PTR (pWMVDec, pWMVDec->m_pAvgQuanDctCoefDecC);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pAvgQuanDctCoefDecLeft :%x",(U32_WMV)pWMVDec->m_pAvgQuanDctCoefDecLeft);
#endif
	FREE_PTR (pWMVDec, pWMVDec->m_pAvgQuanDctCoefDecLeft);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pAvgQuanDctCoefDecTop :%x",(U32_WMV)pWMVDec->m_pAvgQuanDctCoefDecTop);
#endif
	FREE_PTR (pWMVDec, pWMVDec->m_pAvgQuanDctCoefDecTop);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pLoopFilterFlags :%x",(U32_WMV)pWMVDec->m_pLoopFilterFlags);
#endif
#ifdef XBOXGPU
    if(!pWMVDec->m_bXBOXUsingGPU)
#endif        
    FREE_PTR (pWMVDec, pWMVDec->m_pLoopFilterFlags);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgmv1 :%x",(U32_WMV)pWMVDec->m_rgmv1);
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_rgmv1);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgmv1_EMB :%x",(U32_WMV)pWMVDec->m_rgmv1_EMB);
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_rgmv1_EMB); 

    if (pWMVDec->m_bBFrameOn)
    {

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgmv_X9 :%x",(U32_WMV)pWMVDec->m_rgmv_X9);
#endif
        FREE_PTR (pWMVDec, pWMVDec->m_rgmv_X9);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgmvForwardPred :%x",(U32_WMV)pWMVDec->m_rgmvForwardPred);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_rgmvForwardPred);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgmvBackwardPred :%x",(U32_WMV)pWMVDec->m_rgmvBackwardPred);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_rgmvBackwardPred);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgmbmd1 :%x",(U32_WMV)pWMVDec->m_rgmbmd1);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_rgmbmd1);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pvopfRef1 :%x",(U32_WMV)pWMVDec->m_pvopfRef1);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_pvopfRef1);
    }

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgiQuanCoefACPred :%x",(U32_WMV)pWMVDec->m_rgiQuanCoefACPred);
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_rgiQuanCoefACPred);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgiQuanCoefACPredTable :%x",(U32_WMV)pWMVDec->m_rgiQuanCoefACPredTable);
#endif
	FREE_PTR (pWMVDec, pWMVDec->m_rgiQuanCoefACPredTable);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgchSkipPrevFrame :%x",(U32_WMV)pWMVDec->m_rgchSkipPrevFrame);
#endif
	FREE_PTR (pWMVDec, pWMVDec->m_rgchSkipPrevFrame);
    pWMVDec->m_pBMPBits = NULL_WMV;

    DEBUG_HEAP_SUB(g_iHeapLastClass,0);  // indicate the following huffman tables are localHuffman

    if (pWMVDec->m_cvCodecVersion >= WMV2) {
        t_AltTablesDecoderDestruct (pWMVDec, pWMVDec->m_pAltTables);     pWMVDec->m_pAltTables = NULL_WMV; 
#       ifndef WMV9_SIMPLE_ONLY
            t_SpatialPredictorDestruct (pWMVDec, pWMVDec->m_pSp);        
            t_ContextWMVDestruct (pWMVDec, pWMVDec->m_pContext);         
#       endif // WMV9_SIMPLE_ONLY
			pWMVDec->m_pSp = NULL_WMV;  
			pWMVDec->m_pContext = NULL_WMV;
    }

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO VodecDestruct Huffman_WMV_destruct 1");
#endif

    DEBUG_HEAP_SUB(DHEAP_HUFFMAN,0);  // indicate the following huffman tables are regular tables (non-localHuffman)

    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufMVDec_Talking);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufMVDec_HghMt);

    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTDCyDec_Talking);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTDCcDec_Talking);

    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTDCyDec_HghMt);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTDCcDec_HghMt);

    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufICBPCYDec);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec);

    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_HighRate);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_MidRate);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_LowRate);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO VodecDestruct Huffman_WMV_destruct 2");
#endif
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACInterDec_HghMt);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_HghMt);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACInterDec_Talking);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_Talking);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACInterDec_MPEG4);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_MPEG4);

    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACInterDec_HghRate);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_HghRate);

    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_1);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_2);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_3);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_4);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO VodecDestruct Huffman_WMV_destruct 3");
#endif
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV1);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV2);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV3);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV4);

    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_huf4x4PatternHighQPDec);   
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_huf4x4PatternMidQPDec);    
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_huf4x4PatternLowQPDec);    
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufMBXformTypeLowQPDec);   
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufMBXformTypeMidQPDec);   
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufMBXformTypeHighQPDec);  
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufBlkXformTypeLowQPDec);  
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufBlkXformTypeMidQPDec);  
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufBlkXformTypeHighQPDec); 

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO VodecDestruct Huffman_WMV_destruct 3");
#endif
///////////////
    if (pWMVDec->m_cvCodecVersion == WMVA) {

#ifndef PPCWMP
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode0);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode0);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode3);
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO VodecDestruct Huffman_WMV_destruct 4");
#endif
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_4);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_5);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_6);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_7);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_8);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV4);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV5);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV6);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV7);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV8);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV4);
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO VodecDestruct Huffman_WMV_destruct 5");
#endif
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV4);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV5);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV6);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV7);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV8);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV4);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV5);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV6);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV7);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV8);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP4);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP4);
#endif //PPCWMP

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pParseStartCodeBuffer: %x", pWMVDec->m_pParseStartCodeBuffer);
#endif
        FREE_PTR (pWMVDec, pWMVDec->m_pParseStartCodeBuffer);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pParseStartCodeBitstream: %x", pWMVDec->m_pParseStartCodeBitstream);
#endif
        FREE_PTR (pWMVDec, pWMVDec->m_pParseStartCodeBitstream);
    }

//////////////

    CloseThreads(pWMVDec);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO VodecDestruct CombinedLevelRunFree");
#endif
    CombinedLevelRunFree(pWMVDec, &pWMVDec->IntraDCTTableInfo_Dec_HghRate, sizeof(sm_rgLevelAtIndxOfIntraY_HghRate));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->InterDCTTableInfo_Dec_HghRate, sizeof(sm_rgLevelAtIndx_HghRate));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->IntraDCTTableInfo_Dec_MPEG4, sizeof(sm_rgLevelAtIndxOfIntraY_MPEG4));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->InterDCTTableInfo_Dec_MPEG4, sizeof(sm_rgLevelAtIndx_MPEG4));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->IntraDCTTableInfo_Dec_Talking, sizeof(sm_rgLevelAtIndxOfIntraY_Talking));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->InterDCTTableInfo_Dec_Talking,sizeof(sm_rgLevelAtIndx_Talking));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->IntraDCTTableInfo_Dec_HghMt,sizeof(sm_rgLevelAtIndxOfIntraY_HghMt));
    CombinedLevelRunFree(pWMVDec, &pWMVDec->InterDCTTableInfo_Dec_HghMt, sizeof(sm_rgLevelAtIndx_HghMt));  

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pDiffMV: %x", pWMVDec->m_pDiffMV);
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_pDiffMV);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pXMotion: %x", pWMVDec->m_pXMotion);
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_pXMotion);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pXMotionC: %x", pWMVDec->m_pXMotionC);
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_pXMotionC);

#   ifdef _EMB_WMV3_
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pDiffMV_EMB: %x", pWMVDec->m_pDiffMV_EMB);
#endif
        FREE_PTR (pWMVDec, pWMVDec->m_pDiffMV_EMB);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgiMVDecLUT_EMB: %x", pWMVDec->m_rgiMVDecLUT_EMB);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_rgiMVDecLUT_EMB);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pMotion: %x", pWMVDec->m_pMotion);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_pMotion);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pMotionC: %x", pWMVDec->m_pMotionC);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_pMotionC);
#   endif

    // _ZONE_VLC_
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_ppxliFieldMBBuffer: %x", pWMVDec->m_ppxliFieldMBBuffer);
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_ppxliFieldMBBuffer);

    //    FREE_PTR (pWMVDec->m_pcAffineTf);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_ppIntraDCTTableInfo_Dec: %x", pWMVDec->m_ppIntraDCTTableInfo_Dec);
#endif
    FREE_PTR (pWMVDec, pWMVDec->m_ppIntraDCTTableInfo_Dec);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_ppInterDCTTableInfo_Dec: %x", pWMVDec->m_ppInterDCTTableInfo_Dec);
#endif
	FREE_PTR (pWMVDec, pWMVDec->m_ppInterDCTTableInfo_Dec);

    if (pWMVDec->m_bX9){
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pX9dct: %x", pWMVDec->m_pX9dct);
#endif
        FREE_PTR (pWMVDec, pWMVDec->m_pX9dct);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pFieldMvX: %x", pWMVDec->m_pFieldMvX);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_pFieldMvX);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pMultiresBuffer: %x", pWMVDec->m_pMultiresBuffer);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_pMultiresBuffer);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pIntraBlockRowData: %x", pWMVDec->m_pIntraBlockRowData);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_pIntraBlockRowData);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgmv1: %x", pWMVDec->m_rgmv1);
#endif
        FREE_PTR (pWMVDec, pWMVDec->m_rgmv1);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_rgmv1_EMB: %x", pWMVDec->m_rgmv1_EMB);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_rgmv1_EMB);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pInterpY0Buffer: %x", pWMVDec->m_pInterpY0Buffer);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_pInterpY0Buffer);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pFieldMvX_Pbuf: %x", pWMVDec->m_pFieldMvX_Pbuf);
#endif
        FREE_PTR (pWMVDec, pWMVDec->m_pFieldMvX_Pbuf);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pB411InterpX: %x", pWMVDec->m_pB411InterpX);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_pB411InterpX);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pFieldMvX_FPred: %x", pWMVDec->m_pFieldMvX_FPred);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_pFieldMvX_FPred);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pFieldMvX_BPred: %x", pWMVDec->m_pFieldMvX_BPred);
#endif
		FREE_PTR (pWMVDec, pWMVDec->m_pFieldMvX_BPred);

    }

#   ifdef _EMB_SSIMD_IDCT_
#       ifndef WMV_OPT_MOTIONCOMP_SH3DSP
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_pZigzagSSIMD: %x", pWMVDec->m_pZigzagSSIMD);
#endif
            FREE_PTR(pWMVDec, pWMVDec->m_pZigzagSSIMD );  
#       endif
#   endif

    if(pWMVDec->m_cReferenceLibrary != NULL)
    {
        RL_CloseReferenceLibrary( pWMVDec );
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO m_cReferenceLibrary: %x", pWMVDec->m_cReferenceLibrary);
#endif
        FREE_PTR (pWMVDec, pWMVDec->m_cReferenceLibrary);
    }
    
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO VodecDestruct END");
#endif
    return WMV_Succeeded;
}

#ifdef _WMV9AP_

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
#ifdef VIDEO_MEM_SHARE
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
		freeFrame (pWMVDec->m_pfrmCurrQ, pWMVDec);
		freeFrame (pWMVDec->m_pfrmRef0Q, pWMVDec);
		freeFrame (pWMVDec->m_pfrmRef1Q, pWMVDec);
		freeFrame (pWMVDec->m_pfrmMultiresQ, pWMVDec);

		//#ifdef _SUPPORT_POST_FILTERS_
		freeFrame (pWMVDec->m_pfrmPostQ, pWMVDec);
		freeFrame (pWMVDec->m_pfrmPostPrevQ, pWMVDec);

		FREE_PTR (pWMVDec, pWMVDec->m_pfrmCurrQ);
		FREE_PTR (pWMVDec, pWMVDec->m_pfrmRef0Q);
		FREE_PTR (pWMVDec, pWMVDec->m_pfrmRef1Q);
		FREE_PTR (pWMVDec, pWMVDec->m_pfrmMultiresQ);
		FREE_PTR (pWMVDec, pWMVDec->m_pfrmPostQ);
		FREE_PTR (pWMVDec, pWMVDec->m_pfrmPostPrevQ);
		if (pWMVDec->m_bYUV411) {
			freeFrame (pWMVDec->m_pfrmYUV411toYUV420Q, pWMVDec);
			FREE_PTR (pWMVDec, pWMVDec->m_pfrmYUV411toYUV420Q);
		}
	}

#ifdef _DUMPFRAME_
    freeFrame(pWMVDec->m_pvopfMR);
#endif

    FREE_PTR (pWMVDec, pWMVDec->m_puiFirstMBRowOfSlice);
    FREE_PTR (pWMVDec, pWMVDec->m_pbStartOfSliceRowOrig);
    FREE_PTR (pWMVDec, pWMVDec->m_puiNumBytesOfSliceWMVA);
    FREE_PTR (pWMVDec, pWMVDec->m_rgmv1_EMB);


   // FREE_PTR (pWMVDec->m_rgDCAvgPMB);
    FREE_PTR (pWMVDec, pWMVDec->m_pLoopFilterFlags);

    if (pWMVDec->m_cvCodecVersion == WMVA) {
        FREE_PTR (pWMVDec, pWMVDec->m_ppxliIntraRowY);
        FREE_PTR (pWMVDec, pWMVDec->m_ppxliIntraRowU);
        FREE_PTR (pWMVDec, pWMVDec->m_ppxliIntraRowV);
    }

    if (pWMVDec->m_bX9){
        FREE_PTR (pWMVDec, pWMVDec->m_pX9dct);
        FREE_PTR (pWMVDec, pWMVDec->m_pFieldMvX);
        FREE_PTR (pWMVDec, pWMVDec->m_pMultiresBuffer);
        FREE_PTR (pWMVDec, pWMVDec->m_pIntraBlockRowData);
        FREE_PTR (pWMVDec, pWMVDec->m_rgmv1);
        FREE_PTR (pWMVDec, pWMVDec->m_pInterpY0Buffer);
        FREE_PTR (pWMVDec, pWMVDec->m_pFieldMvX_Pbuf);
        FREE_PTR (pWMVDec, pWMVDec->m_pB411InterpX);
        FREE_PTR (pWMVDec, pWMVDec->m_pFieldMvX_FPred);
        FREE_PTR (pWMVDec, pWMVDec->m_pFieldMvX_BPred);
    }
    // End of common encoder/decoder stuff

    FREE_PTR (pWMVDec, pWMVDec->m_pXMotion);
    FREE_PTR (pWMVDec, pWMVDec->m_pXMotionC);

    if (pWMVDec->m_bBFrameOn)
    {
        FREE_PTR (pWMVDec, pWMVDec->m_rgmv_X9);
        FREE_PTR (pWMVDec, pWMVDec->m_rgmvForwardPred);
        FREE_PTR (pWMVDec, pWMVDec->m_rgmvBackwardPred);
        FREE_PTR (pWMVDec, pWMVDec->m_rgmbmd1);
    }

    FREE_PTR (pWMVDec, pWMVDec->m_rgmv);
    FREE_PTR (pWMVDec, pWMVDec->m_rgmbmd);
    FREE_PTR (pWMVDec, pWMVDec->m_pmbmdZeroCBPCY);
//    FREE_PTR (pWMVDec, pWMVDec->m_rgiQuanCoef_IMBInPFrame_DCPredTable0);
//    FREE_PTR (pWMVDec, pWMVDec->m_rgiQuanCoef_IMBInPFrame_DCPredTable1);

    FREE_PTR (pWMVDec, pWMVDec->m_rgiQuanCoefACPred);
    FREE_PTR (pWMVDec, pWMVDec->m_rgiQuanCoefACPredTable);
    FREE_PTR (pWMVDec, pWMVDec->m_rgchSkipPrevFrame);
#ifdef _MULTITHREADED_RECON_
    FREE_PTR (pWMVDec, pWMVDec->m_rllArray);
#endif

#ifndef WMV9_SIMPLE_ONLY
    if (pWMVDec->m_cvCodecVersion >= WMV2) 
	{
        t_ContextWMVDestruct(pWMVDec, pWMVDec->m_pContext);
	    pWMVDec->m_pContext = NULL;
	}
#endif

    // loopfilter
    FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8RowFlagY0);
    FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x4RowFlagY0);
    FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8RowFlagU0); 
    FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x4RowFlagU0); 
    FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8RowFlagV0); 
    FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x4RowFlagV0); 
    FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8ColFlagY0);
    FREE_PTR (pWMVDec, pWMVDec->m_puchFilter4x8ColFlagY0);
    FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8ColFlagU0); 
    FREE_PTR (pWMVDec, pWMVDec->m_puchFilter4x8ColFlagU0); 
    FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8ColFlagV0); 
    FREE_PTR (pWMVDec, pWMVDec->m_puchFilter4x8ColFlagV0);
    if (pWMVDec->m_uiNumProcessors >= 2)
    {
        FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8RowFlagY1);
        FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x4RowFlagY1);
        FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8RowFlagU1); 
        FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x4RowFlagU1); 
        FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8RowFlagV1); 
        FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x4RowFlagV1); 
        FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8ColFlagY1);
        FREE_PTR (pWMVDec, pWMVDec->m_puchFilter4x8ColFlagY1);
        FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8ColFlagU1); 
        FREE_PTR (pWMVDec, pWMVDec->m_puchFilter4x8ColFlagU1); 
        FREE_PTR (pWMVDec, pWMVDec->m_puchFilter8x8ColFlagV1); 
        FREE_PTR (pWMVDec, pWMVDec->m_puchFilter4x8ColFlagV1);
    }

#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO FreeFrameAreaDependentMemory 2");
#endif
    if(pWMVDec->m_cReferenceLibrary != NULL)
    {
        RL_CloseReferenceLibrary( pWMVDec );
    }
}

#endif

Void_WMV InitVars_Tmp(tWMVDecInternalMember *pWMVDec)
{
    pWMVDec->m_t = 0; 
    // for _MPEG4_BITSTRM
    pWMVDec->m_tModuloBaseDecd = 0;
    pWMVDec->m_tModuloBaseDisp = 0;
    pWMVDec->m_iNumBitsTimeIncr = 0;
    // _MPEG4_BITSTRM
    pWMVDec->m_iMvResolution = 0; 
    pWMVDec->m_bDCTTable_MB_ENABLED= FALSE_WMV; 
    pWMVDec->m_bDCTTable_MB= FALSE_WMV; 
    pWMVDec->m_bDCPred_IMBInPFrame= FALSE_WMV;
    pWMVDec->m_bMixedPel= FALSE_WMV; 
    pWMVDec->m_bLoopFilter = FALSE_WMV; 
    pWMVDec->m_bNeedLoopFilterOnCurrFrm = FALSE_WMV;
    pWMVDec->m_bXformSwitch = FALSE_WMV;
    pWMVDec->m_bSKIPBIT_CODING_= FALSE_WMV; 
    pWMVDec->m_bNEW_PCBPCY_TABLE= FALSE_WMV; 
    pWMVDec->m_bXintra8Switch = FALSE_WMV;
    pWMVDec->m_bXintra8 = FALSE_WMV; 
    pWMVDec->m_bFrmHybridMVOn = FALSE_WMV;

    pWMVDec->m_bRndCtrlOn = FALSE_WMV;
    pWMVDec->m_bDeblockOn = FALSE_WMV; // no deblocking at the beginning
    pWMVDec->m_bDeringOn = FALSE_WMV;
    pWMVDec->m_fPrepared = FALSE_WMV;
    pWMVDec->m_fDecodedI = FALSE_WMV;    // Haven't seen an I-Picture yet.
    pWMVDec->m_rgchSkipPrevFrame = NULL_WMV; pWMVDec->m_rgmv = NULL_WMV;
    pWMVDec->m_pBMPBits = NULL_WMV;
    pWMVDec->m_rgiQuanCoefACPred = NULL_WMV;
    pWMVDec->m_rgiQuanCoefACPredTable= NULL_WMV;
    pWMVDec->m_pAvgQuanDctCoefDec = NULL_WMV;
    pWMVDec->m_pAvgQuanDctCoefDecC = NULL_WMV;
    pWMVDec->m_pAvgQuanDctCoefDecLeft = NULL_WMV;
    pWMVDec->m_pAvgQuanDctCoefDecTop = NULL_WMV;
    pWMVDec->m_rgmbmd = NULL_WMV;
    pWMVDec->m_pmbmdZeroCBPCY = NULL_WMV;
    pWMVDec->m_iPostProcessMode= 0;
    pWMVDec->m_bUseOldSetting= FALSE_WMV;
    pWMVDec->m_iPostFilterLevel= -1;



    memset(pWMVDec->m_rgiDCTACInterTableIndx, 0, 3 * sizeof(U32_WMV));
    memset(pWMVDec->m_rgiDCTACIntraTableIndx, 0, 3 * sizeof(U32_WMV));

    pWMVDec->m_bUVHpelBilinear = FALSE_WMV;

    pWMVDec->m_bRefreshDisplay_AllMB_Enable= FALSE_WMV;
    pWMVDec->m_bRefreshDisplay_AllMB= FALSE_WMV;
    pWMVDec->m_iRefreshDisplay_AllMB_Cnt= 0;
    pWMVDec->m_iRefreshDisplay_AllMB_Period= 1000;
    pWMVDec->m_bCopySkipMBToPostBuf = TRUE_WMV;
    pWMVDec->m_bSafeMode = FALSE_WMV;
    pWMVDec->m_bStrongARM = FALSE_WMV;
    pWMVDec->m_iDispRotateDegree = WMV_DispRotate0;

#ifdef WMV_OPT_SPRITE
    pWMVDec->m_fltA1 = pWMVDec->m_fltB1 = pWMVDec->m_fltC1 = pWMVDec->m_fltD1 = pWMVDec->m_fltE1 = pWMVDec->m_fltF1 = pWMVDec->m_fltFading1 = 0;
    pWMVDec->m_fltA2 = pWMVDec->m_fltB2 = pWMVDec->m_fltC2 = pWMVDec->m_fltD2 = pWMVDec->m_fltE2 = pWMVDec->m_fltF2 = pWMVDec->m_fltFading2 = 0;
#endif

    pWMVDec->m_pCallBackApp = NULL_WMV;
    pWMVDec->m_u32SCEnableMask = 0;
    pWMVDec->m_bResizeDimensions = FALSE_WMV;

}    


Bool_WMV g_bSupportMMX_WMV = 0;
Bool_WMV g_bSupportAltiVec_WMV = 0;
Bool_WMV g_bSupportWMMX_WMV = 0;


tWMVDecodeStatus VodecConstruct (
                                 tWMVDecInternalMember *pWMVDec, 
                                 U32_WMV               dwFOURCC,
                                 I32_WMV                 iFrameRate,
                                 I32_WMV                 iWidthSource, 
                                 I32_WMV                 iHeightSource,
                                 CInputBitStream_WMV     *pInputBitstream//,
                                 )  
{
   // VResultCode vr;
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    I32_WMV result = 0;


#ifdef  _WMV_TARGET_X86_
    g_bSupportMMX_WMV = g_SupportMMX ();
#endif //  _WMV_TARGET_X86_

#if defined(_MAC) || defined(macintosh)
    g_bSupportAltiVec_WMV = g_SupportAltiVec ();
#endif

#ifdef _ARM_
    g_bSupportWMMX_WMV = 0; //disable Bulverde code path switching temporally, g_supportWMMX ();
#endif  

    /*
    pWMVDec->m_bPreviousDQuantOn  = FALSE_WMV;
    pWMVDec->m_bPreviousDQuantBiLevel = FALSE_WMV;
    pWMVDec->m_iPreviousPanning = 0; 
    */

    pWMVDec->m_bExtendedMvMode = FALSE_WMV;
    pWMVDec->m_iMVRangeIndex  = 0;

    pWMVDec->m_bStartCode = FALSE;

    pWMVDec->m_bSliceWMVA =FALSE;
    pWMVDec->m_puiFirstMBRowOfSlice = NULL;
    pWMVDec->m_puiNumBytesOfSliceWMVA = NULL;
    pWMVDec->m_uiNumBytesFrameHeaderSlice = 0;
    pWMVDec->m_uiNumBytesFieldHeaderSlice = 0;
    pWMVDec->m_uiNumSlicesWMVA = 0;

#ifdef _EMB_WMV3_
    pWMVDec->m_pDiffMV_EMB = NULL;
    pWMVDec->m_rgiMVDecLUT_EMB = NULL;
#endif
    pWMVDec->m_pDiffMV = NULL;


    //VOConstruct();
    pWMVDec->m_uintNumMBYSlice = 1;
    pWMVDec->m_bCODFlagOn = TRUE_WMV; 
    pWMVDec->m_bFirstEscCodeInFrame= TRUE_WMV; 
    //VODecConstruct
    pWMVDec->m_iFrameRate = iFrameRate;

    //   pWMVDec->m_pchXformType  = NULL_WMV,

    pWMVDec->m_iRefreshDisplay_AllMB_Period= 1000;
    pWMVDec->m_iPostProcessMode= 0;
    pWMVDec->m_bCopySkipMBToPostBuf = TRUE_WMV;
    pWMVDec->m_pLoopFilterFlags  = NULL_WMV;
    pWMVDec->m_bStartCode  = FALSE_WMV;
    pWMVDec->m_pIntraBlockRowData  = 0; 
    pWMVDec->m_bSequenceOverlap = FALSE_WMV;
    pWMVDec->m_iOverlap = FALSE_WMV;
    pWMVDec->m_bUVHpelBilinear = FALSE_WMV;
    pWMVDec->m_iRangeState =0;
    pWMVDec->m_iReconRangeState = 0;
    pWMVDec->m_bPreProcRange = FALSE_WMV;
    pWMVDec->m_iNumBFrames = 0;
    pWMVDec->m_bBetaContent  = TRUE_WMV;
    pWMVDec->m_bRTMContent  = FALSE_WMV;
    pWMVDec->m_iBetaRTMMismatchIndex = 0;

#ifdef _WMV9AP_
    pWMVDec->m_iMaxPrevA2LCFrameArea = 0;

    pWMVDec->m_bParseStartCodeFrameOnly = FALSE_WMV;
    pWMVDec->m_iParseStartCodeLastType = 0;
    pWMVDec->m_iParseStartCodeRemLen = 0;
    pWMVDec->m_pBufferedUserData = NULL;
    pWMVDec->m_iBufferedUserDataLen = 0;
#endif

#ifdef _EMB_SSIMD_IDCT_
    pWMVDec->m_pZigzagSSIMD = NULL_WMV;  
#endif

    pWMVDec->m_rgmv_X9 = NULL_WMV;
    pWMVDec->m_rgmvForwardPred = NULL_WMV;
    pWMVDec->m_rgmvBackwardPred = NULL_WMV;
    pWMVDec->m_rgmbmd1 = NULL_WMV;
    pWMVDec->m_pvopfRef1 = NULL_WMV;
    pWMVDec->m_pfrmMultiresQ = NULL_WMV;
    pWMVDec->m_bPrevRefInPrevPost  = FALSE;

    pWMVDec->m_rgmv1_EMB = NULL_WMV;
    pWMVDec->m_rgmv1 = NULL_WMV; pWMVDec->m_pInterpY0Buffer= NULL_WMV;
    pWMVDec->m_pInterpY0 = NULL_WMV; pWMVDec->m_pInterpY1 = NULL_WMV; 
    pWMVDec->m_pInterpU0 = NULL_WMV; pWMVDec->m_pInterpU1 = NULL_WMV;
    pWMVDec->m_pInterpV0 = NULL_WMV; pWMVDec->m_pInterpV1 = NULL_WMV;

    pWMVDec->m_eFrameWasOutput = FrameDecOut_None;
    pWMVDec->m_iNumFrameDropped = 0;
    pWMVDec->m_pvopcRenderThis = NULL_WMV;

    pWMVDec->m_pFieldMvX_Pbuf = NULL_WMV; pWMVDec->m_pB411InterpX = NULL_WMV;
    pWMVDec->m_pFieldMvX_FPred = NULL_WMV; pWMVDec->m_pFieldMvX_BPred = NULL_WMV;

    pWMVDec->m_bExplicitSeqQuantizer = FALSE_WMV; pWMVDec->m_bExplicitFrameQuantizer = FALSE_WMV; 
    pWMVDec->m_bUse3QPDZQuantizer = FALSE_WMV; pWMVDec->m_bHalfStep = FALSE_WMV;
    pWMVDec->m_bSeqFrameInterpolation= FALSE_WMV; pWMVDec->m_bInterpolateCurrentFrame= FALSE_WMV;


    Huffman_WMV_construct(&pWMVDec->m_hufMVDec_Talking);
    Huffman_WMV_construct(&pWMVDec->m_hufMVDec_HghMt);

    Huffman_WMV_construct(&pWMVDec->m_hufDCTDCyDec_Talking);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTDCcDec_Talking);

    Huffman_WMV_construct(&pWMVDec->m_hufDCTDCyDec_HghMt);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTDCcDec_HghMt);

    Huffman_WMV_construct(&pWMVDec->m_hufICBPCYDec);
    Huffman_WMV_construct(&pWMVDec->m_hufPCBPCYDec);

    Huffman_WMV_construct(&pWMVDec->m_hufPCBPCYDec_HighRate);
    Huffman_WMV_construct(&pWMVDec->m_hufPCBPCYDec_MidRate);
    Huffman_WMV_construct(&pWMVDec->m_hufPCBPCYDec_LowRate);

    Huffman_WMV_construct(&pWMVDec->m_hufDCTACInterDec_HghMt);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTACIntraDec_HghMt);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTACInterDec_Talking);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTACIntraDec_Talking);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTACInterDec_MPEG4);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTACIntraDec_MPEG4);

    Huffman_WMV_construct(&pWMVDec->m_hufDCTACInterDec_HghRate);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTACIntraDec_HghRate);

    Huffman_WMV_construct(&pWMVDec->m_hufPCBPCYDec_V9_1);
    Huffman_WMV_construct(&pWMVDec->m_hufPCBPCYDec_V9_2);
    Huffman_WMV_construct(&pWMVDec->m_hufPCBPCYDec_V9_3);
    Huffman_WMV_construct(&pWMVDec->m_hufPCBPCYDec_V9_4);

    Huffman_WMV_construct(&pWMVDec->m_HufMVTable_V9_MV1);
    Huffman_WMV_construct(&pWMVDec->m_HufMVTable_V9_MV2);
    Huffman_WMV_construct(&pWMVDec->m_HufMVTable_V9_MV3);
    Huffman_WMV_construct(&pWMVDec->m_HufMVTable_V9_MV4);



    InitVars_Tmp(pWMVDec);

    pWMVDec->m_bX9 = FALSE_WMV;
    //pWMVDec->m_bFieldCodingOn = FALSE_WMV;


    pWMVDec->m_pXMotion = NULL_WMV;
    pWMVDec->m_pXMotionC = NULL_WMV;

#ifdef _EMB_WMV3_
     pWMVDec->m_pMotion = NULL_WMV;
     pWMVDec->m_pMotionC = NULL_WMV;
#endif

#ifdef _EMB_3FRAMES_
     pWMVDec->m_bEMB3FrmEnabled = TRUE_WMV;
     pWMVDec->m_bOutputOpEnabled = FALSE_WMV;
#else
    pWMVDec->m_bEMB3FrmEnabled = FALSE_WMV;
#endif



    pWMVDec->m_b16bitXform = FALSE_WMV;
    // disable for now  pWMVDec->m_bHostDeinterlace = TRUE_WMV;

    pWMVDec->m_ppIntraDCTTableInfo_Dec = NULL_WMV;
    pWMVDec->m_ppInterDCTTableInfo_Dec = NULL_WMV;

    pWMVDec->m_bMultiresEnabled = FALSE_WMV;
    pWMVDec->m_bDQuantOn = FALSE_WMV;
    pWMVDec->m_iDQuantCodingOn = 0;
    pWMVDec->m_bDQuantBiLevel= FALSE_WMV;
    // pWMVDec->m_bX4LoopFilter  = FALSE_WMV;

    pWMVDec->m_bNewDCQuant  = FALSE_WMV;

    pWMVDec->m_cReferenceLibrary =  NULL;

    //pWMVDec->m_bYUV411  = FALSE_WMV;

    pWMVDec->m_ppxliFieldMBBuffer  =  NULL_WMV;
    pWMVDec->m_ppxliFieldMB  = NULL_WMV;

    pWMVDec->m_iPanning = 0;

#ifndef _WMV9AP_
    pWMVDec->m_bV9LoopFilter  = FALSE_WMV;
#endif

    pWMVDec->m_bSkipThisFrame  = FALSE_WMV;
    pWMVDec->m_bOmittedBFrame  = FALSE_WMV;

    pWMVDec->m_bInterlaceV2 = FALSE; 
    pWMVDec->m_bOmittedBFrame  = FALSE_WMV;
    pWMVDec->m_bFieldMode = FALSE;
    pWMVDec->m_iCurrentField = 0;
    //pWMVDec->m_bPMBFieldDctSwitch = FALSE;
    //pWMVDec->m_bExplicitACPredSwitch = FALSE;

    pWMVDec->m_bBroadcastFlags  = FALSE_WMV;
    pWMVDec->m_bTemporalFrmCntr  = FALSE_WMV;
    pWMVDec->m_bTopFieldFirst  = FALSE_WMV;
    pWMVDec->m_bRepeatFirstField  = FALSE_WMV;
    pWMVDec->m_ppxliIntraRowY  = NULL_WMV;
    pWMVDec->m_ppxliIntraRowU  = NULL_WMV;
    pWMVDec->m_ppxliIntraRowV  = NULL_WMV;
    pWMVDec->m_bAdvancedProfile  = FALSE_WMV;

    pWMVDec->m_bFieldMode = FALSE_WMV;
    pWMVDec->m_iCurrentField = 0;
    pWMVDec->m_iCurrentTemporalField = 0;
    pWMVDec->m_iPrevSpatialField = 0;
    pWMVDec->m_bUseSameFieldForRef = TRUE_WMV;
    pWMVDec->m_bTwoRefPictures = TRUE_WMV;
    pWMVDec->m_bUseOppFieldForRef = TRUE_WMV;
    pWMVDec->m_bBroadcastFlags = FALSE_WMV;
    
    pWMVDec->m_bTopFieldFirst = TRUE_WMV;
    pWMVDec->m_bRepeatFirstField = FALSE_WMV;

    pWMVDec->m_bInterlacedSource = FALSE;
    pWMVDec-> m_iRepeatFrameCount = 0;


   // pWMVDec->m_bCacheRTCFrame = FALSE_WMV;
   // pWMVDec->m_bRestoreCachedRTCFrame = FALSE_WMV;
   // pWMVDec->m_bRefreshSequenceHeader = FALSE_WMV;
    pWMVDec->m_bProgressive420 = TRUE_WMV;

        
    pWMVDec->m_bExtendedDeltaMvMode = FALSE_WMV;
    pWMVDec->m_iDeltaMVRangeIndex = 0;
    pWMVDec->m_iExtendedDMVX = 0;
    pWMVDec->m_iExtendedDMVY = 0;

    pWMVDec->m_iRefFrameDistance = 0; 
    pWMVDec->m_iBackRefFrameDistance = 0; 
    pWMVDec->m_iMaxZone1ScaledFarMVX = 0; 
    pWMVDec->m_iMaxZone1ScaledFarMVY = 0;
    pWMVDec->m_iZone1OffsetScaledFarMVX = 0; 
    pWMVDec->m_iZone1OffsetScaledFarMVY = 0; 
    pWMVDec->m_iFarFieldScale1 = 0; 
    pWMVDec->m_iFarFieldScale2 = 0;
    pWMVDec->m_iNearFieldScale = 0; 
    pWMVDec->m_iMaxZone1ScaledFarBackMVX = 0; 
    pWMVDec->m_iMaxZone1ScaledFarBackMVY = 0; 
    pWMVDec->m_iZone1OffsetScaledFarBackMVX = 0;
    pWMVDec->m_iZone1OffsetScaledFarBackMVY = 0; 
    pWMVDec->m_iFarFieldScaleBack1 = 0; 
    pWMVDec->m_iFarFieldScaleBack2 = 0; 
    pWMVDec->m_iNearFieldScaleBack = 0;
    //pWMVDec->m_rgErrorBlock = NULL_WMV;

    pWMVDec->m_bMBXformSwitching = FALSE;

    pWMVDec->m_iChromaFormat = 1;
    pWMVDec->m_bLargerSeqBufferRequired = TRUE;
    pWMVDec->m_bSeqFrameWidthOrHeightChanged = TRUE;
    pWMVDec->m_bPrevSeqCBEntropyCoding = FALSE;
    pWMVDec->m_iPrevSeqFrmWidthSrc = 0;
    pWMVDec->m_iPrevSeqFrmHeightSrc = 0;
    pWMVDec->m_iMaxPrevSeqFrameArea = 0;

#ifdef WMVIMAGE_V2
    pWMVDec->m_bSpriteModeV2 = FALSE_WMV;
#endif

    pWMVDec->m_iLevel = 0;
    
    pWMVDec->m_bSwitchingEntryPoint = FALSE;

    pWMVDec->m_bRenderDirectWithPrerender = FALSE_WMV;

    pWMVDec->m_bRangeRedY_Flag = FALSE;
    pWMVDec->m_bRangeRedUV_Flag = FALSE;

    pWMVDec->m_uiCurrSlice = 0; 
    pWMVDec->m_pbStartOfSliceRowOrig = NULL;
    pWMVDec->m_bHRD_PARAM_FLAG = FALSE; 
    pWMVDec->m_ihrd_num_leaky_buckets = 1;

    pWMVDec->m_bBrokenLink = FALSE;
    pWMVDec->m_bClosedEntryPoint = FALSE;  
    pWMVDec->m_bPanScanPresent = FALSE; 
    pWMVDec->m_bRefDistPresent = FALSE;

    if (dwFOURCC == FOURCC_WMV2_WMV || dwFOURCC == FOURCC_wmv2_WMV) {
        pWMVDec->m_cvCodecVersion = WMV2;
        PPC_TEST_SUPPORT_PROFILE(WMVSupportWMV8);
    } else if (dwFOURCC == FOURCC_WMV3_WMV || dwFOURCC == FOURCC_wmv3_WMV) {
        pWMVDec->m_cvCodecVersion = WMV3;
    }
    else if (dwFOURCC == FOURCC_WMVA_WMV || dwFOURCC == FOURCC_wmva_WMV) {
        pWMVDec->m_cvCodecVersion = WMVA;
        // PPC_TEST_SUPPORT_PROFILE is handled in sequence header decodeVOLHead_WMV3()
    } else if (dwFOURCC == FOURCC_WMV1_WMV || dwFOURCC == FOURCC_wmv1_WMV) {
        pWMVDec->m_cvCodecVersion = WMV1;
        PPC_TEST_SUPPORT_PROFILE(WMVSupportWMV7);
    } else if (dwFOURCC == FOURCC_MP43_WMV || dwFOURCC == FOURCC_mp43_WMV) {
        pWMVDec->m_cvCodecVersion = MP43;
        PPC_TEST_SUPPORT_PROFILE(WMVSupportMP43);
    } else if (dwFOURCC == FOURCC_MP42_WMV || dwFOURCC == FOURCC_mp42_WMV 
        || dwFOURCC ==  FOURCC_DIV2_WMV || dwFOURCC ==  FOURCC_div2_WMV ) {
        pWMVDec->m_cvCodecVersion = MP42;
        PPC_TEST_SUPPORT_PROFILE(WMVSupportMP42);
    } else if (dwFOURCC == FOURCC_MP4S_WMV || dwFOURCC == FOURCC_mp4s_WMV) {
        pWMVDec->m_cvCodecVersion = MP4S;
    } else if (dwFOURCC == FOURCC_WMVP_WMV || dwFOURCC == FOURCC_wmvp_WMV) {
        pWMVDec->m_cvCodecVersion = WMV3;
        PPC_TEST_SUPPORT_PROFILE(WMVSupportWMVP);
    }
#ifdef WMVIMAGE_V2
	else if (dwFOURCC == FOURCC_WVP2_WMV || dwFOURCC == FOURCC_wvp2_WMV) {
        pWMVDec->m_cvCodecVersion = WMVA;
        pWMVDec->m_bSpriteModeV2 = TRUE_WMV;
        PPC_TEST_SUPPORT_PROFILE(WMVSupportWMVP2);
    } 
#endif
#ifndef WIN32_DEG
	else
        return WMV_UnSupportedCompressedFormat;
#else
	else {
		I8_WMV cTmp[50];
		strcpy(pWMVDec->m_SeqHeadInfoString, "Codec Version: ");
		memcpy(cTmp,(I8_WMV*)&dwFOURCC,4);
		cTmp[4] = '\0';
		strcat(cTmp, "(Unsupported FOURCC)");
		strcat(pWMVDec->m_SeqHeadInfoString, cTmp);
        return WMV_UnSupportedCompressedFormat;
	}
	{
		I8_WMV cTmp[50];
		strcpy(pWMVDec->m_SeqHeadInfoString, "Codec Version: ");
		memcpy(cTmp,(I8_WMV*)&dwFOURCC,4);
		cTmp[4] = '\0';
		strcat(cTmp, "(FOURCC)");
		strcat(pWMVDec->m_SeqHeadInfoString, cTmp);
	}
#endif

    if (pWMVDec->m_cvCodecVersion >= WMV3) 
    {
        pWMVDec->m_bBFrameOn = TRUE;
        pWMVDec->m_bX9 = TRUE;
        pWMVDec->m_bMixedPel = FALSE;
        pWMVDec->m_bNewDCQuant = TRUE;

    }

    if (WMVideoIsSpriteMode(pWMVDec))
    {
        pWMVDec->m_bBFrameOn = FALSE;

        //By default WMVA bitstream turned this on. This should not be used however for SpriteMode.
        //Turn it on cause AV. So, keep it off.
        //pWMVDec->m_bSeqFrameWidthOrHeightChanged = FALSE;
    }
    else
        pWMVDec->m_bBFrameOn = TRUE;

    if(pWMVDec->m_cvCodecVersion != WMV3 || pWMVDec->m_bYUV411 ||  WMVideoIsSpriteMode (pWMVDec) )
        pWMVDec->m_bEMB3FrmEnabled = FALSE_WMV;


#if !defined(NO_WINDOWS) && defined(_MultiThread_Decode_)  
    SYSTEM_INFO sysinfo;
    GetSystemInfo (&sysinfo);
    pWMVDec->m_uiNumProcessors = sysinfo.dwNumberOfProcessors;
#else
    pWMVDec->m_uiNumProcessors = 1;
#endif


    pWMVDec->m_uiNumProcessors = 1;  // implemented for only one processor

    //   if (pWMVDec->m_bYUV411)
    if ( pWMVDec->m_iNumBFrames > 0)
        pWMVDec->m_uiNumProcessors = 1;


    if (pWMVDec->m_uiNumProcessors > 2)
        pWMVDec->m_uiNumProcessors = 2;


    if (pWMVDec->m_cvCodecVersion == WMVA) {
        pWMVDec->m_uiNumProcessors = 1;
        pWMVDec->m_bAdvancedProfile = TRUE;
    }


#ifdef _EMB_SSIMD_IDCT_
    if (pWMVDec->m_cvCodecVersion >= WMV3) 
    {
#ifdef WMV_OPT_MOTIONCOMP_SH3DSP

#define pDspPhysicalXmem  0xA5007000
        pWMVDec->m_pZigzagSSIMD = (U8_WMV *)((I32_WMV)pDspPhysicalXmem+512);

#else
#ifdef XDM
		pWMVDec->m_pZigzagSSIMD = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += 64+64;
#else
        pWMVDec->m_pZigzagSSIMD  = (U8_WMV*)wmvMalloc(pWMVDec, 64+64, DHEAP_STRUCT);
        if(pWMVDec->m_pZigzagSSIMD == NULL)
             goto lerror;
#endif 
#endif // WMV_OPT_MOTIONCOMP_SH3DSP

        pWMVDec->m_p8x4ZigzagSSIMD = pWMVDec->m_pZigzagSSIMD  + 0;
        pWMVDec->m_p4x4ZigzagSSIMD = pWMVDec->m_pZigzagSSIMD  + 64 ;
     }
#endif


    if (pWMVDec->m_cvCodecVersion == WMVA)
    {
        g_InitDecGlobalVars ();
        m_InitIDCT_Dec(pWMVDec,TRUE);
        decideMMXRoutines (pWMVDec);
        m_InitFncPtrAndZigzag(pWMVDec);

        //This was originally in InitDataMembers(). But when multiple sequences are present,
        // the malloc(sizeof( sequence header is read before calling InitDataMembers. Therefore we should
       
        // move the reset of m_iNumBFrames out of InitDataMembers().

        //  In Sprite Mode, BFrame should be turned off by default. 
        //  This is really to fix the resize code uses
        //	if(pWMVDec->m_bSeqFrameWidthOrHeightChanged == TRUE && pWMVDec->m_iNumBFrames > 0 && pWMVDec->m_t > 1) to
        //  tell whether resize is happening for wmva bitstream.

        if (!WMVideoIsSpriteMode(pWMVDec))
            pWMVDec->m_iNumBFrames = 1;// This is used as a boolean which enables B frame decoding. For WMVA we always support B frame decoding
        else
            pWMVDec->m_iNumBFrames = 0;
        
        pWMVDec->m_iParseStartCodeBufLen    = 4096;
#ifdef XDM
		pWMVDec->m_pParseStartCodeBuffer = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += pWMVDec->m_iParseStartCodeBufLen;
#else
        pWMVDec->m_pParseStartCodeBuffer    = (U8_WMV*)wmvMalloc(pWMVDec, pWMVDec->m_iParseStartCodeBufLen, DHEAP_STRUCT);
        if (pWMVDec->m_pParseStartCodeBuffer == NULL_WMV) {
            return WMV_BadMemory;
        }
#endif
        
#ifdef XDM
		pWMVDec->m_pParseStartCodeBitstream = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += pWMVDec->m_iParseStartCodeBufLen;
#else
        pWMVDec->m_pParseStartCodeBitstream = (U8_WMV*)wmvMalloc(pWMVDec, pWMVDec->m_iParseStartCodeBufLen, DHEAP_STRUCT);
        if (pWMVDec->m_pParseStartCodeBitstream == NULL_WMV) {
            return WMV_BadMemory;
        }
#endif  
        
    }
    else
    {
        g_InitDecGlobalVars ();
        m_InitIDCT_Dec(pWMVDec,TRUE);
        m_InitFncPtrAndZigzag(pWMVDec);
        decideMMXRoutines (pWMVDec);
        //tWMVStatus = InitDataMembers (pWMVDec, iWidthSource, iHeightSource);

        if (tWMVStatus != WMV_Succeeded) {
            return tWMVStatus;
        }
    }


#ifdef WMVIMAGE_V2
    if (WMVideoIsSpriteMode (pWMVDec)) 
    {
        tWMVStatus = Effects_Init(pWMVDec);
        if (tWMVStatus != WMV_Succeeded) {
            return tWMVStatus;
        }
    }
#endif

    // for multi-threading
    pWMVDec->m_bShutdownThreads = FALSE_WMV;

    tWMVStatus = initMultiThreadVars_Dec (pWMVDec);
    if (tWMVStatus != WMV_Succeeded) {
        return tWMVStatus;
    }

    if (pWMVDec->m_cvCodecVersion != WMVA) {
        tWMVStatus = InitThreads (pWMVDec);
       if (tWMVStatus != WMV_Succeeded) {
            return tWMVStatus;
        }
    }

    pWMVDec->m_pbitstrmIn = pInputBitstream;

    tWMVStatus = initSeqIndependentMemory(pWMVDec);
    if (tWMVStatus != WMV_Succeeded) {
        return tWMVStatus;
    }

    /*
    tWMVStatus = InitDataMembers_WMVA (pWMVDec, iWidthSource, iHeightSource);
    if (tWMVStatus != WMV_Succeeded) {
        return tWMVStatus;
    }
    */

    //the decoder is initialized according to display size, obtained from ASF header.
    //future sequence needs to compare with this to decide whether to reallocate buffers or not.
    pWMVDec->m_iMaxPrevSeqFrameArea = iWidthSource * iHeightSource;
    pWMVDec->m_iMaxFrmWidthSrc = iWidthSource;
    pWMVDec->m_iMaxFrmHeightSrc = iHeightSource;

    //old codecs only needs to call memory alloc once
    //allocate according to max size
    result = initFrameAreaDependentMemory(pWMVDec, iWidthSource, iHeightSource);
    if (ICERR_OK != result)
        return WMV_Failed;

    tWMVStatus = InitDataMembers_WMVA (pWMVDec, iWidthSource, iHeightSource);
    if (WMV_Succeeded != tWMVStatus)
        return tWMVStatus;

    //InitThreads (pWMVDec);

#   if defined(SH3_DSP_ASM_IDCT)
#       define pDspPhysicalYmem 0xA5017000
    pWMVDec->m_ppxliErrorQ = (UnionBuffer *)((I32_WMV)pDspPhysicalYmem+512);
    pWMVDec->m_rgiCoefReconBuf = (UnionBuffer *)((I32_WMV)pDspPhysicalYmem+768);
#   else
    // Set 32-byte aligned pointers to IDCT recon coeff and pixel error buffers
    pWMVDec->m_ppxliErrorQ = (UnionBuffer *)(((DWORD_PTR)pWMVDec->m_riPixelError + 31) & ~31);
    pWMVDec->m_rgiCoefReconBuf = (UnionBuffer *)(((DWORD_PTR)pWMVDec->m_riReconBuf + 31) & ~31);
#   endif
    pWMVDec->m_rgiCoefRecon = (PixelI32 *)pWMVDec->m_rgiCoefReconBuf;

    pWMVDec->m_rgiCoefReconPlus1 = &pWMVDec->m_rgiCoefRecon[1];
    pWMVDec->m_iStepSize = 0;

#ifndef _EMB_WMV2_
    pWMVDec->m_pMotionComp = pWMVDec->m_pMotionCompUV = pWMVDec->m_pMotionCompRndCtrlOff;
    pWMVDec->m_pMotionCompAndAddError = pWMVDec->m_pMotionCompAndAddErrorUV = pWMVDec->m_pMotionCompAndAddErrorRndCtrlOff;
#endif

#ifdef _EMB_WMV2_
    pWMVDec->m_iMotionCompRndCtrl=VECTORIZE_MOTIONCOMP_RNDCTRL_OFF;
#endif

#ifdef DYNAMIC_EDGEPAD                
    pWMVDec->m_tEdgePad.m_iround_ctrl=pWMVDec->m_iMotionCompRndCtrl;
#endif


#ifndef WMV9_SIMPLE_ONLY
    InitMultires (pWMVDec);
#endif
    ComputeDQuantDecParam (pWMVDec);

    InitBlockInterpolation (pWMVDec);

    // initialize it for error resilience
    pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmCurrQ;




//IW, no longer used anywhere in decoder_c9e
//    pWMVDec->m_pDeinterlaceProc = VideoDIDeinterlace;

    //_MV

    if (pWMVDec->m_cvCodecVersion != WMVA)
    {

        VResultCode vr;

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO Huffman_WMV_init 1");
#endif
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMVDec_Talking, pWMVDec->m_uiUserData, sm_HufMVTable_Talking, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufMVDec_Talking, CV_HufMVTable_Talking, sm_HufMVTable_Talking, "HufMVTable_Talking");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMVDec_HghMt, pWMVDec->m_uiUserData, sm_HufMVTable_HghMt, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufMVDec_HghMt, CV_HufMVTable_HghMt, sm_HufMVTable_HghMt, "HufMVTable_HghMt");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufICBPCYDec,pWMVDec->m_uiUserData, sm_HufICBPCYTable, SIZE_sm_HufICBPCYTable);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN_ICBPCY

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec, pWMVDec->m_uiUserData, sm_HufPCBPCYTable, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN_PCBPCY

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO Huffman_WMV_init 2");
#endif
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_HighRate, pWMVDec->m_uiUserData, sm_HufPCBPCYTable_HighRate, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufPCBPCYDec_HighRate, CV_HufPCBPCYTable_HighRate, sm_HufPCBPCYTable_HighRate, "HufPCBPCYTable_HighRate");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_MidRate, pWMVDec->m_uiUserData, sm_HufPCBPCYTable_MidRate, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufPCBPCYDec_MidRate, CV_HufPCBPCYTable_MidRate, sm_HufPCBPCYTable_MidRate, "HufPCBPCYTable_MidRate");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_LowRate, pWMVDec->m_uiUserData, sm_HufPCBPCYTable_LowRate, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufPCBPCYDec_LowRate, CV_HufPCBPCYTable_LowRate, sm_HufPCBPCYTable_LowRate, "HufPCBPCYTable_LowRate");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCyDec_Talking, pWMVDec->m_uiUserData, sm_HufDCTDCyTable_Talking, SIZE_sm_HufDCTDCyTable_Talking);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufDCTDCyDec_Talking, CV_HufDCTDCyTable_Talking, sm_HufDCTDCyTable_Talking, "HufDCTDCyTable_Talking");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCcDec_Talking, pWMVDec->m_uiUserData, sm_HufDCTDCcTable_Talking, SIZE_sm_HufDCTDCcTable_Talking);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufDCTDCcDec_Talking, CV_HufDCTDCcTable_Talking, sm_HufDCTDCcTable_Talking, "HufDCTDCcTable_Talking");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCyDec_HghMt, pWMVDec->m_uiUserData, sm_HufDCTDCyTable_HghMt, SIZE_sm_HufDCTDCyTable_HghMt);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufDCTDCyDec_HghMt, CV_HufDCTDCyTable_HghMt, sm_HufDCTDCyTable_HghMt, "HufDCTDCyTable_HghMt");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCcDec_HghMt, pWMVDec->m_uiUserData, sm_HufDCTDCcTable_HghMt, SIZE_sm_HufDCTDCcTable_HghMt);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufDCTDCcDec_HghMt, CV_HufDCTDCcTable_HghMt, sm_HufDCTDCcTable_HghMt, "HufDCTDCcTable_HghMt");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_HghMt, pWMVDec->m_uiUserData, sm_HufDCTACInterTable_HghMt, SIZE_sm_HufDCTACInterTable_HghMt);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufDCTACInterDec_HghMt, CV_HufDCTACInterTable_HghMt, sm_HufDCTACInterTable_HghMt, "HufDCTACInterTable_HghMt");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_HghMt, pWMVDec->m_uiUserData, sm_HufDCTACIntraTable_HghMt, SIZE_sm_HufDCTACIntraTable_HghMt);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufDCTACIntraDec_HghMt, CV_HufDCTACIntraTable_HghMt, sm_HufDCTACIntraTable_HghMt, "HufDCTACIntraTable_HghMt");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_Talking, pWMVDec->m_uiUserData, sm_HufDCTACInterTable_Talking, SIZE_sm_HufDCTACInterTable_Talking);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufDCTACInterDec_Talking, CV_HufDCTACInterTable_Talking, sm_HufDCTACInterTable_Talking, "HufDCTACInterTable_Talking");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_Talking, pWMVDec->m_uiUserData, sm_HufDCTACIntraTable_Talking, SIZE_sm_HufDCTACIntraTable_Talking);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufDCTACIntraDec_Talking, CV_HufDCTACIntraTable_Talking, sm_HufDCTACIntraTable_Talking, "HufDCTACIntraTable_Talking");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_MPEG4, pWMVDec->m_uiUserData, sm_HufDCTACInterTable_MPEG4, SIZE_sm_HufDCTACInterTable_MPEG4);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufDCTACInterDec_MPEG4, CV_HufDCTACInterTable_MPEG4, sm_HufDCTACInterTable_MPEG4, "HufDCTACInterTable_MPEG4");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_MPEG4, pWMVDec->m_uiUserData, sm_HufDCTACIntraTable_MPEG4, SIZE_sm_HufDCTACIntraTable_MPEG4);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufDCTACIntraDec_MPEG4, CV_HufDCTACIntraTable_MPEG4, sm_HufDCTACIntraTable_MPEG4, "HufDCTACIntraTable_MPEG4");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_HghRate, pWMVDec->m_uiUserData, sm_HufDCTACInterTable_HghRate, SIZE_sm_HufDCTACInterTable_HghRate);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN( &pWMVDec->m_hufDCTACInterDec_HghRate, CV_HufDCTACInterTable_HghRate, sm_HufDCTACInterTable_HghRate, "HufDCTACInterTable_HghRate");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_HghRate, pWMVDec->m_uiUserData, sm_HufDCTACIntraTable_HghRate, SIZE_sm_HufDCTACIntraTable_HghRate);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN( &pWMVDec->m_hufDCTACIntraDec_HghRate, CV_HufDCTACIntraTable_HghRate, sm_HufDCTACIntraTable_HghRate, "HufDCTACIntraTable_HghRate");


#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO Huffman_WMV_init 3");
#endif
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_1,pWMVDec->m_uiUserData,sm_HufPCBPCYTable_V9_1, SIZE_sm_HufPCBPCYTable_V9_1);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufPCBPCYDec_V9_1,CV_HufPCBPCYTable_V9_1,sm_HufPCBPCYTable_V9_1, "HufPCBPCYTable_V9_1");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_2,pWMVDec->m_uiUserData,sm_HufPCBPCYTable_V9_2, SIZE_sm_HufPCBPCYTable_V9_2);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufPCBPCYDec_V9_2,CV_HufPCBPCYTable_V9_2,sm_HufPCBPCYTable_V9_2, "HufPCBPCYTable_V9_2");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_3,pWMVDec->m_uiUserData,sm_HufPCBPCYTable_V9_3, SIZE_sm_HufPCBPCYTable_V9_3);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufPCBPCYDec_V9_3,CV_HufPCBPCYTable_V9_3,sm_HufPCBPCYTable_V9_3, "HufPCBPCYTable_V9_3");
    
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_4,pWMVDec->m_uiUserData,sm_HufPCBPCYTable_V9_4, SIZE_sm_HufPCBPCYTable_V9_4);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufPCBPCYDec_V9_4,CV_HufPCBPCYTable_V9_4,sm_HufPCBPCYTable_V9_4, "HufPCBPCYTable_V9_4");

        pWMVDec->m_pHufPCBPCYTables[0] = &pWMVDec->m_hufPCBPCYDec_V9_1;
        pWMVDec->m_pHufPCBPCYTables[1] = &pWMVDec->m_hufPCBPCYDec_V9_2;
        pWMVDec->m_pHufPCBPCYTables[2] = &pWMVDec->m_hufPCBPCYDec_V9_3;
        pWMVDec->m_pHufPCBPCYTables[3] = &pWMVDec->m_hufPCBPCYDec_V9_4;

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV1,pWMVDec->m_uiUserData,sm_HufMVTable_V9_MV1, SIZE_sm_HufMVTable_V9_MV1);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_HufMVTable_V9_MV1,CV_HufMVTable_V9_MV1,sm_HufMVTable_V9_MV1, "HufMVTable_V9_MV1");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV2,pWMVDec->m_uiUserData,sm_HufMVTable_V9_MV2, SIZE_sm_HufMVTable_V9_MV2);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_HufMVTable_V9_MV2,CV_HufMVTable_V9_MV2,sm_HufMVTable_V9_MV2, "HufMVTable_V9_MV2");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV3,pWMVDec->m_uiUserData,sm_HufMVTable_V9_MV3, SIZE_sm_HufMVTable_V9_MV3);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_HufMVTable_V9_MV3,CV_HufMVTable_V9_MV3,sm_HufMVTable_V9_MV3, "HufMVTable_V9_MV3");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV4,pWMVDec->m_uiUserData,sm_HufMVTable_V9_MV4, SIZE_sm_HufMVTable_V9_MV4);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_HufMVTable_V9_MV4,CV_HufMVTable_V9_MV4,sm_HufMVTable_V9_MV4, "HufMVTable_V9_MV4");

        pWMVDec->m_pHufMVTables[0] = &pWMVDec->m_HufMVTable_V9_MV1;
        pWMVDec->m_pHufMVTables[1] = &pWMVDec->m_HufMVTable_V9_MV2;
        pWMVDec->m_pHufMVTables[2] = &pWMVDec->m_HufMVTable_V9_MV3;
        pWMVDec->m_pHufMVTables[3] = &pWMVDec->m_HufMVTable_V9_MV4;


#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO Huffman_WMV_init 4");
#endif

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_huf4x4PatternHighQPDec, pWMVDec->m_uiUserData,sm_Huf4x4PatternHighQPTable, SIZE_sm_Huf4x4PatternHighQPTable);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_huf4x4PatternHighQPDec, CV_Huf4x4PatternHighQPTable,sm_Huf4x4PatternHighQPTable, "Huf4x4PatternHighQPTable");
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_huf4x4PatternMidQPDec, pWMVDec->m_uiUserData,sm_Huf4x4PatternMidQPTable, SIZE_sm_Huf4x4PatternMidQPTable);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_huf4x4PatternMidQPDec, CV_Huf4x4PatternMidQPTable,sm_Huf4x4PatternMidQPTable, "Huf4x4PatternMidQPTable");
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_huf4x4PatternLowQPDec, pWMVDec->m_uiUserData,sm_Huf4x4PatternLowQPTable, SIZE_sm_Huf4x4PatternLowQPTable);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_huf4x4PatternLowQPDec, CV_Huf4x4PatternLowQPTable,sm_Huf4x4PatternLowQPTable, "Huf4x4PatternLowQPTable");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMBXformTypeLowQPDec, pWMVDec->m_uiUserData,sm_HufMBXformTypeLowQPTable, SIZE_sm_HufMBXformTypeLowQPTable);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufMBXformTypeLowQPDec, CV_HufMBXformTypeLowQPTable,sm_HufMBXformTypeLowQPTable, "HufMBXformTypeLowQPTable");
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMBXformTypeMidQPDec, pWMVDec->m_uiUserData,sm_HufMBXformTypeMidQPTable, SIZE_sm_HufMBXformTypeMidQPTable);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufMBXformTypeMidQPDec, CV_HufMBXformTypeMidQPTable,sm_HufMBXformTypeMidQPTable, "HufMBXformTypeMidQPTable");
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMBXformTypeHighQPDec, pWMVDec->m_uiUserData,sm_HufMBXformTypeHighQPTable, SIZE_sm_HufMBXformTypeHighQPTable);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufMBXformTypeHighQPDec, CV_HufMBXformTypeHighQPTable,sm_HufMBXformTypeHighQPTable, "HufMBXformTypeHighQPTable");

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufBlkXformTypeLowQPDec, pWMVDec->m_uiUserData,sm_HufBlkXformTypeLowQPTable, SIZE_sm_HufBlkXformTypeLowQPTable);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufBlkXformTypeLowQPDec, CV_HufBlkXformTypeLowQPTable,sm_HufBlkXformTypeLowQPTable, "HufBlkXformTypeLowQPTable");
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufBlkXformTypeMidQPDec, pWMVDec->m_uiUserData,sm_HufBlkXformTypeMidQPTable, SIZE_sm_HufBlkXformTypeMidQPTable);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufBlkXformTypeMidQPDec, CV_HufBlkXformTypeMidQPTable,sm_HufBlkXformTypeMidQPTable, "HufBlkXformTypeMidQPTable");
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufBlkXformTypeHighQPDec, pWMVDec->m_uiUserData,sm_HufBlkXformTypeHighQPTable, SIZE_sm_HufBlkXformTypeHighQPTable);
        if (vr != vrNoError) goto lerror;
        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufBlkXformTypeHighQPDec, CV_HufBlkXformTypeHighQPTable,sm_HufBlkXformTypeHighQPTable, "HufBlkXformTypeHighQPTable");

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO Huffman_WMV_init 5");
#endif

        if (pWMVDec->m_cvCodecVersion >= WMV2) 
        {
            if (pWMVDec->m_pAltTables == NULL_WMV)
                pWMVDec->m_pAltTables = t_AltTablesDecoderConstruct (pWMVDec, pWMVDec->m_uiUserData);

            if (pWMVDec->m_pAltTables == NULL_WMV)
                goto lerror;

    #ifndef WMV9_SIMPLE_ONLY
            // Construct spatial predictor
            if (pWMVDec->m_pSp == NULL_WMV)
            {
                pWMVDec->m_pSp = t_SpatialPredictorConstruct (pWMVDec);

                if (!pWMVDec->m_pSp)
                    goto lerror;

            }
            pWMVDec->m_pSp->m_pIDCT_Dec = pWMVDec->m_pIntraX9IDCT_Dec;

            // Construct contexts, and DCT array
            if (pWMVDec->m_pContext == NULL_WMV)
            {
                pWMVDec->m_pContext = t_ContextWMVConstruct (pWMVDec, pWMVDec->m_uintNumMBX*2, 2);  // 2 rows enough
                if (!pWMVDec->m_pContext)
                    goto lerror;
            }
    #endif // WMV9_SIMPLE_ONLY
        }
    }
    else
    {
        if (pWMVDec->m_cvCodecVersion >= WMV2) {
#ifndef WMV9_SIMPLE_ONLY
			pWMVDec->m_pSp->m_pIDCT_Dec = pWMVDec->m_pIntraX9IDCT_Dec;
#endif

                //Initialize all VLC tables
            if (ICERR_OK != initVlcTables(pWMVDec)) {
                 goto lerror;
            }
        }
    }

    tWMVStatus = setVlcTableInfo(pWMVDec);

    //#ifdef X9
    if (pWMVDec->m_cvCodecVersion >= WMV3) {

        if (pWMVDec->m_pDiffMV == NULL)
        {
#ifdef XDM
			pWMVDec->m_pDiffMV = (CDiffMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += 4*sizeof( CDiffMV);
#else
            pWMVDec->m_pDiffMV = (CDiffMV *)wmvMalloc(pWMVDec, 4*sizeof( CDiffMV), DHEAP_STRUCT);
            if (pWMVDec->m_pDiffMV == NULL)
                goto lerror;
#endif   
        }

        //        pWMVDec->m_pDecodePMB = DecodeMB_X9;
        pWMVDec->m_pDecodeP = WMVideoDecDecodeP_X9;
    
#ifdef _EMB_WMV3_
#if defined(macintosh) || defined(_ARM_)
    if ((!g_bSupportAltiVec_WMV) && (!g_bSupportWMMX_WMV))
#endif
    {
#ifdef XDM
		pWMVDec->m_pDiffMV_EMB = (CDiffMV_EMB *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += 6*sizeof( CDiffMV_EMB);
#else
        pWMVDec->m_pDiffMV_EMB= (CDiffMV_EMB*)wmvMalloc(pWMVDec, 6*sizeof( CDiffMV_EMB), DHEAP_STRUCT);
        if (pWMVDec->m_pDiffMV_EMB == NULL)
            goto lerror;
#endif

#ifdef XDM
		pWMVDec->m_rgiMVDecLUT_EMB = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += 36*sizeof(I16_WMV);
#else
        pWMVDec->m_rgiMVDecLUT_EMB = (I16_WMV*)wmvMalloc(pWMVDec, 36*sizeof(I16_WMV), DHEAP_STRUCT);
        if (pWMVDec->m_rgiMVDecLUT_EMB == NULL)
            goto lerror;
#endif
    }
#endif

        //if (pWMVDec->m_cvCodecVersion != WMVA)
        {

        if (pWMVDec->m_pXMotion==NULL_WMV)
        {
#ifdef XDM
			pWMVDec->m_pXMotion = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I16_WMV)*(pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY * 8);
#else
            pWMVDec->m_pXMotion = (I16_WMV*)wmvMalloc(pWMVDec, sizeof(I16_WMV)*(pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY * 8), DHEAP_STRUCT);
#endif
            pWMVDec->m_pYMotion = pWMVDec->m_pXMotion + pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY * 4;
            if (pWMVDec->m_pXMotion == NULL)
                goto lerror;
        }

#ifdef _EMB_WMV3_
#if defined(macintosh) || defined(_ARM_)
        if ((!g_bSupportAltiVec_WMV) && (!g_bSupportWMMX_WMV))
#endif
            {
                if (pWMVDec->m_pMotion == NULL_WMV)
                {
#ifdef XDM
					pWMVDec->m_pMotion = (UMotion_EMB *)pWMVDec->m_MemXDM.pPrivateBuff;
					pWMVDec->m_MemXDM.pPrivateBuff += sizeof(UMotion_EMB)*(pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY * 4);
#else
                    pWMVDec->m_pMotion = (UMotion_EMB*)wmvMalloc(pWMVDec, sizeof(UMotion_EMB)*(pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY * 4), DHEAP_STRUCT);
                    if (pWMVDec->m_pMotion == NULL)
                        goto lerror;
#endif
                }

                if (pWMVDec->m_pMotionC == NULL_WMV)
                {
#ifdef XDM
					pWMVDec->m_pMotionC = (UMotion_EMB *)pWMVDec->m_MemXDM.pPrivateBuff;
					pWMVDec->m_MemXDM.pPrivateBuff += sizeof(UMotion_EMB)*(pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY );
#else
                    pWMVDec->m_pMotionC = (UMotion_EMB*)wmvMalloc(pWMVDec, sizeof(UMotion_EMB)*(pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY ), DHEAP_STRUCT);
                    if (pWMVDec->m_pMotionC == NULL)
                        goto lerror;
#endif  
                }
            }

#endif
       


        // for Interlace Decoding

#ifndef WMV9_SIMPLE_ONLY

        if (pWMVDec->m_ppxliFieldMBBuffer==NULL_WMV)
        {
#ifdef XDM
			pWMVDec->m_ppxliFieldMBBuffer = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += 2*(6 * BLOCK_SQUARE_SIZE + 32);
#else
            pWMVDec->m_ppxliFieldMBBuffer = (I16_WMV*)wmvMalloc(pWMVDec, 2*(6 * BLOCK_SQUARE_SIZE + 32), DHEAP_STRUCT);
            if (pWMVDec->m_ppxliFieldMBBuffer == NULL)
                goto lerror;
#endif

            pWMVDec->m_ppxliFieldMB = (I16_WMV *) (((DWORD_PTR)pWMVDec->m_ppxliFieldMBBuffer + 31) & ~31);
        }

#endif // WMV9_SIMPLE_ONLY

        if (pWMVDec->m_pXMotionC == NULL_WMV)
        {
#ifdef XDM
			pWMVDec->m_ppxliFieldMBBuffer = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += 2* (pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY * 2);
#else
            pWMVDec->m_pXMotionC = (I16_WMV*)wmvMalloc(pWMVDec, 2* (pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY * 2), DHEAP_STRUCT);
#endif       
            pWMVDec->m_pYMotionC = pWMVDec->m_pXMotionC + pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY;
            if (pWMVDec->m_pXMotionC == NULL)
            goto lerror;
        }

        }
        // ---------------------
    }

#ifdef _EMB_WMV3_
#if defined(macintosh) || defined(_ARM_)
    if ((!g_bSupportAltiVec_WMV) && (!g_bSupportWMMX_WMV))
#endif
    {
        if (pWMVDec->m_cvCodecVersion == WMV3) {
            InitEMBPMainLoopCtl(pWMVDec);
        }
    }
#endif

    return tWMVStatus;

lerror:
    return WMV_BadMemory;
}

#ifdef _WMV9AP_
//****************************************************
// InitDataMembers:
// Called by both constructor and sequence switch.
//
// Make sure initializations in this function do not override the 
// sequence header read from decodeSequenceHead_Advanced().
//****************************************************
tWMVDecodeStatus InitDataMembers_WMVA (tWMVDecInternalMember *pWMVDec, 
    I32_WMV iWidthSource,
    I32_WMV iHeightSource
)
{

    I32_WMV result;
    //Bool_WMV bAllocFail = FALSE;

    // Default transform-switching settings
  //  pWMVDec->m_iFrameXformMode = XFORMMODE_8x8;
  //  pWMVDec->m_bMBXformSwitching = FALSE;

//    pWMVDec->m_bBFrameOn = FALSE;

#ifdef _GENERATE_DXVA_DATA_
    pWMVDec->m_bDxvaOffLine = FALSE;
    pWMVDec->m_bDxvaPostProcOnly = FALSE;
#endif

    pWMVDec->m_tFrmType = IVOP;

    if (!pWMVDec->m_bSwitchingEntryPoint)
    {
        pWMVDec->m_iBState = -3;
        pWMVDec->m_iPrevIFrame = 0;
        pWMVDec->m_bPrevAnchor = FALSE;          
        pWMVDec->m_iDroppedPFrame = 0;
        pWMVDec->m_bIsBChangedToI = FALSE;
    }

    pWMVDec->m_bShutdownThreads = FALSE;

#ifdef _MULTITHREADED_RECON_
    pWMVDec->m_iRLLCount = 0;
#endif


#if defined(_EMB_SSIMD32_)
	if (pWMVDec->m_rgMotionCompBuffer_EMB == NULL) {
#ifdef XDM
		pWMVDec->m_rgMotionCompBuffer_EMB = (U32_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += (160) * sizeof (U32_WMV);
#else
        pWMVDec->m_rgMotionCompBuffer_EMB = (U32_WMV*) wmvMalloc (pWMVDec, (160) * sizeof (U32_WMV), DHEAP_STRUCT);
#endif
	}
#endif //_EMB_SSIMD64_

    if (pWMVDec->m_bSeqFrameWidthOrHeightChanged) {
        result = initFrameWidthAndHeightDependentVariables(pWMVDec, iWidthSource, iHeightSource);
        if (ICERR_OK != result)
            return WMV_Failed;
        initFrameWidthAndHeightDependentPointers(pWMVDec);
    }

    return WMV_Succeeded;
}

Void_WMV ResetSizeOfRenderThis (tWMVDecInternalMember *pWMVDec)
{
    
   
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

    if (!WMVideoIsSpriteMode(pWMVDec))
    {
        pWMVDec->m_iFrmWidthSrcOld = pWMVDec->m_iWidthY;
        pWMVDec->m_iFrmHeightSrcOld = pWMVDec->m_iHeightY;
    }
    
    pWMVDec->m_iWidthY = iWidthMBAligned;
    pWMVDec->m_iHeightY = iHeightMBAligned;

    //Always set SpriteMode to no changes on Resize
    if (WMVideoIsSpriteMode(pWMVDec))
    {
        pWMVDec->m_iFrmWidthSrcOld = pWMVDec->m_iWidthY;
        pWMVDec->m_iFrmHeightSrcOld = pWMVDec->m_iHeightY;
    }

    pWMVDec->m_iMVLeftBound = -63; // in half pel
    pWMVDec->m_iMVRightBound = 63; // in half pel

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
    //I32_WMV result = assignVODataMembers ();

  //      AdjustPictureCYUV420(pWMVDec);

    computePars_WMVA (pWMVDec);

        // for multi-threading, need m_iWidthPrevYXExpPlusExp etc, should be called after computePars().
    initMultiThreadVars_Dec_WMVA (pWMVDec);

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
   
    if (!pWMVDec->m_bSpriteMode)
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
    else
    {
		iFrameNum += 4;
        //The picture sequence coding(such as WMVP) does not use m_pfrmMultiresQ and m_pfrmRef1Q buffer, so skip using it.
        result= RL_SetupReferenceLibrary (pWMVDec, iSizeYplane, iSizeUVplane, iFrameNum, 
            iPlaneWidth * (pWMVDec->m_cvCodecVersion == WMVA)*4, iPlaneWidth, iPlaneHeight);
    
        if (ICERR_OK != result)
            return result;

		RL_RemoveFrame(pWMVDec->m_cReferenceLibrary, (Void_WMV **)&pWMVDec->m_pfrmCurrQ, -1);
		RL_RemoveFrame(pWMVDec->m_cReferenceLibrary, (Void_WMV **)&pWMVDec->m_pfrmRef0Q, -1);
		RL_RemoveFrame(pWMVDec->m_cReferenceLibrary, (Void_WMV **)&pWMVDec->m_pfrmPostQ, -1);
		RL_RemoveFrame(pWMVDec->m_cReferenceLibrary, (Void_WMV **)&pWMVDec->m_pfrmPostPrevQ, -1);
		if (pWMVDec->m_bYUV411) 
			RL_RemoveFrame(pWMVDec->m_cReferenceLibrary, (Void_WMV **)&pWMVDec->m_pfrmYUV411toYUV420Q, -1);
    }

    //Return error if any of the frames is allocated.
    if (pWMVDec->m_pfrmCurrQ==0 || pWMVDec->m_pfrmRef0Q==0 || pWMVDec->m_pfrmPostQ==0 || pWMVDec->m_pfrmPostPrevQ==0)
        return ICERR_MEMORY;

    pWMVDec->m_ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
    pWMVDec->m_ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
    pWMVDec->m_ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane;

    pWMVDec->m_ppxliCurrQPlusExpY0 = (U8_WMV*) pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    pWMVDec->m_ppxliCurrQPlusExpU0 = (U8_WMV*) pWMVDec->m_ppxliCurrQU  + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    pWMVDec->m_ppxliCurrQPlusExpV0 = (U8_WMV*) pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;


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

//*****************************************
// initSeqIndependentMemory:
// Sequence independent memory allocation. 
// Only need to be called once by the constructor.
// Free in FreeSeqIndependentMemory().
//*****************************************
tWMVDecodeStatus initSeqIndependentMemory(tWMVDecInternalMember *pWMVDec)
{
    //Bool_WMV bAllocFail = FALSE;

#ifdef XDM
	pWMVDec->m_cReferenceLibrary = (CReferenceLibrary *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof(CReferenceLibrary);
#else
    pWMVDec->m_cReferenceLibrary = (CReferenceLibrary *)wmvMalloc(pWMVDec, sizeof(CReferenceLibrary), DHEAP_STRUCT);
    if(pWMVDec->m_cReferenceLibrary == NULL)
    {
                return WMV_BadMemory;
    }
#endif   
    memset(pWMVDec->m_cReferenceLibrary, 0, sizeof(CReferenceLibrary));
        
       // _ZONE_VLC_ 
#ifdef XDM
	pWMVDec->m_ppInterDCTTableInfo_Dec = (CDCTTableInfo_Dec **)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += 3*sizeof(CDCTTableInfo_Dec*);
#else
    pWMVDec->m_ppInterDCTTableInfo_Dec = (CDCTTableInfo_Dec **)wmvMalloc(pWMVDec, 3*sizeof(CDCTTableInfo_Dec*), DHEAP_STRUCT);
    if (!pWMVDec->m_ppInterDCTTableInfo_Dec)
        return WMV_BadMemory;
#endif   

#ifdef XDM
	pWMVDec->m_ppIntraDCTTableInfo_Dec = (CDCTTableInfo_Dec **)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += 3*sizeof(CDCTTableInfo_Dec*);
#else
    pWMVDec->m_ppIntraDCTTableInfo_Dec = (CDCTTableInfo_Dec **)wmvMalloc(pWMVDec, 3*sizeof(CDCTTableInfo_Dec*), DHEAP_STRUCT);
    if (!pWMVDec->m_ppIntraDCTTableInfo_Dec)
                return WMV_BadMemory;
#endif   
      
#ifdef XDM
	pWMVDec->m_pAvgQuanDctCoefDec     = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff   += sizeof (I16_WMV) * BLOCK_SIZE_TIMES2;
	pWMVDec->m_pAvgQuanDctCoefDecC    = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff   += sizeof (I16_WMV) * BLOCK_SIZE_TIMES2;
	pWMVDec->m_pAvgQuanDctCoefDecLeft = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff   += sizeof (I16_WMV) * BLOCK_SIZE_TIMES2;
	pWMVDec->m_pAvgQuanDctCoefDecTop  = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff   += sizeof (I16_WMV) * BLOCK_SIZE_TIMES2;
#else
    pWMVDec->m_pAvgQuanDctCoefDec		= (I16_WMV*) wmvMalloc (pWMVDec, sizeof (I16_WMV) * BLOCK_SIZE_TIMES2, DHEAP_STRUCT);
    pWMVDec->m_pAvgQuanDctCoefDecC		= (I16_WMV*) wmvMalloc (pWMVDec, sizeof (I16_WMV) * BLOCK_SIZE_TIMES2, DHEAP_STRUCT);
    pWMVDec->m_pAvgQuanDctCoefDecLeft	= (I16_WMV*) wmvMalloc (pWMVDec, sizeof (I16_WMV) * BLOCK_SIZE_TIMES2, DHEAP_STRUCT);
    pWMVDec->m_pAvgQuanDctCoefDecTop	= (I16_WMV*) wmvMalloc (pWMVDec, sizeof (I16_WMV) * BLOCK_SIZE_TIMES2, DHEAP_STRUCT);
    if (!pWMVDec->m_pAvgQuanDctCoefDec || !pWMVDec->m_pAvgQuanDctCoefDecC || !pWMVDec->m_pAvgQuanDctCoefDecLeft || !pWMVDec->m_pAvgQuanDctCoefDecTop)
            return WMV_BadMemory;
#endif   


    memset(pWMVDec->m_pAvgQuanDctCoefDec, 0, BLOCK_SIZE_TIMES2 * sizeof(I16_WMV));
    memset(pWMVDec->m_pAvgQuanDctCoefDecC, 0, BLOCK_SIZE_TIMES2 * sizeof(I16_WMV));
    memset(pWMVDec->m_pAvgQuanDctCoefDecLeft, 0, BLOCK_SIZE_TIMES2 * sizeof(I16_WMV));
    memset(pWMVDec->m_pAvgQuanDctCoefDecTop, 0, BLOCK_SIZE_TIMES2 * sizeof(I16_WMV));

    if (pWMVDec->m_cvCodecVersion == MP4S) {
        pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 1024;
        pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 1024;
        pWMVDec->m_bRndCtrlOn = TRUE_WMV;
    }
    else{
        pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 128;
        pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 128;
        pWMVDec->m_bRndCtrlOn = FALSE_WMV;
    }

    if (pWMVDec->m_cvCodecVersion >= WMV2) 
    {
         pWMVDec->m_pAltTables = t_AltTablesDecoderConstruct (pWMVDec, pWMVDec->m_uiUserData);
         if (pWMVDec->m_pAltTables == NULL_WMV)
                goto lerror;
            // Construct spatial predictor
#ifndef WMV9_SIMPLE_ONLY
         pWMVDec->m_pSp = t_SpatialPredictorConstruct (pWMVDec);
         if (!pWMVDec->m_pSp)
                goto lerror;
#endif
    }


//#ifdef X9
    if (pWMVDec->m_cvCodecVersion >= WMV3) {
        if (pWMVDec->m_pDiffMV == NULL)
        {        
#ifdef XDM
			pWMVDec->m_pDiffMV = (CDiffMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += 4*sizeof( CDiffMV);
#else
            pWMVDec->m_pDiffMV = (CDiffMV *)wmvMalloc(pWMVDec, 4*sizeof( CDiffMV), DHEAP_STRUCT);
            if (pWMVDec->m_pDiffMV == NULL)
                goto lerror;
#endif   
        }

        // for Interlace Decoding
#ifdef XDM
		pWMVDec->m_ppxliFieldMBBuffer = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += 2*(6 * BLOCK_SQUARE_SIZE + 32);
#else
         pWMVDec->m_ppxliFieldMBBuffer = (I16_WMV *)wmvMalloc(pWMVDec, 2*(6 * BLOCK_SQUARE_SIZE + 32), DHEAP_STRUCT);
        if (pWMVDec->m_ppxliFieldMBBuffer == NULL)
            goto lerror;
#endif   

        pWMVDec->m_ppxliFieldMB = (I16_WMV *) (((I32_WMV)pWMVDec->m_ppxliFieldMBBuffer + 31) & ~31);
        // ---------------------
     }

//#endif

    // Allocating pWMVDec->m_rgErrorBlock aligned.
#ifdef XDM
	pWMVDec->m_rgErrorBlock = (UnionBuffer *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += 6*sizeof(UnionBuffer);
#else
    pWMVDec->m_rgErrorBlock = (UnionBuffer*) wmvMalloc(pWMVDec, 6*sizeof(UnionBuffer), DHEAP_STRUCT);
    if (pWMVDec->m_rgErrorBlock == NULL)
        goto lerror;
#endif

#ifdef _EMB_WMV3_
    #ifdef _EMB_SSIMD64_
        /* 8 bytes aligned */
#ifdef XDM
		pWMVDec->m_rgMotionCompBuffer_EMB = (U32_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += (200) * sizeof (U32_WMV);
#else
        pWMVDec->m_rgMotionCompBuffer_EMB = (U32_WMV*) wmvMalloc (pWMVDec, (200) * sizeof (U32_WMV), DHEAP_STRUCT); //48 * 16 / 4 = 192
#endif
        if ((U32_WMV)pWMVDec->m_rgMotionCompBuffer_EMB & 7)
            *(pWMVDec->m_rgMotionCompBuffer_EMB) = 1;
        else if (pWMVDec->m_rgMotionCompBuffer_EMB)
            *(pWMVDec->m_rgMotionCompBuffer_EMB+1) = 2;

        if( pWMVDec->m_rgMotionCompBuffer_EMB )
            pWMVDec->m_rgMotionCompBuffer_EMB = (U32_WMV *)((DWORD_PTR)(pWMVDec->m_rgMotionCompBuffer_EMB+2) & ~7);

    #else

#ifdef XDM
		pWMVDec->m_rgMotionCompBuffer_EMB = (U32_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += (160) * sizeof (U32_WMV);
#else
        pWMVDec->m_rgMotionCompBuffer_EMB = (U32_WMV*) wmvMalloc (pWMVDec, (160) * sizeof (U32_WMV), DHEAP_STRUCT);
        if (pWMVDec->m_rgMotionCompBuffer_EMB == NULL)
            goto lerror;
    #endif
#endif
#endif

    return WMV_Succeeded;
lerror:
    return WMV_BadMemory;
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
I32_WMV initFrameAreaDependentMemory(tWMVDecInternalMember *pWMVDec, I32_WMV iMaxEncWidth, I32_WMV iMaxEncHeight)
{
    //Bool_WMV bAllocFail = FALSE;
    I32_WMV iMaxSliceBufSize;

    I32_WMV twStatus;

    //summary of change: 
    // replace all m_uintNumMB, m_uintNumMBX, m_uintNumMBY by iMaxNumMB, iMaxNumMBX, iMaxNumMBY.
    //also replace m_iWidthY, m_iHeightY by iMaxEncWidth and iMaxEncHeight.
            

    I32_WMV iWidthMBAligned = (iMaxEncWidth + 15) & ~15;
    I32_WMV iHeightMBAligned = (iMaxEncHeight + 15) & ~15;
    I32_WMV iMaxNumMBX = iWidthMBAligned >> 4; // divided by 16, MB size
    I32_WMV iMaxNumMBY = iHeightMBAligned >> 4;
    I32_WMV iMaxNumMB = iMaxNumMBX * iMaxNumMBY;

    twStatus = AllocateMultiThreadBufs_Dec_WMVA(pWMVDec, iMaxEncWidth, iMaxEncHeight);
    if (twStatus != ICERR_OK)
        return twStatus;

    twStatus = AllocatePictureCYUV420(pWMVDec, iMaxEncWidth, iMaxEncHeight);
    if (twStatus != ICERR_OK)
        return twStatus;

    iMaxSliceBufSize = ((iHeightMBAligned >> 4) + 1);

#ifdef XDM
	pWMVDec->m_puiFirstMBRowOfSlice = (U32_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof(U32_WMV)*iMaxSliceBufSize;
#else
    pWMVDec->m_puiFirstMBRowOfSlice = (U32_WMV *)wmvMalloc(pWMVDec, sizeof(U32_WMV)*iMaxSliceBufSize, DHEAP_STRUCT);
    if (pWMVDec->m_puiFirstMBRowOfSlice == NULL)
        return ICERR_ERROR;
#endif

#ifdef XDM
	pWMVDec->m_pbStartOfSliceRowOrig = (Bool_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof(Bool_WMV)*iMaxSliceBufSize;
#else
    pWMVDec->m_pbStartOfSliceRowOrig = (Bool_WMV *)wmvMalloc(pWMVDec, sizeof(Bool_WMV)*iMaxSliceBufSize, DHEAP_STRUCT);
    if (pWMVDec->m_pbStartOfSliceRowOrig == NULL) 
        return ICERR_ERROR;
#endif
    memset (pWMVDec->m_pbStartOfSliceRowOrig, 0, sizeof(Bool_WMV)* iMaxSliceBufSize);
    pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;

#ifdef XDM
	pWMVDec->m_puiNumBytesOfSliceWMVA = (U32_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( U32_WMV) *iMaxSliceBufSize;
#else
    pWMVDec->m_puiNumBytesOfSliceWMVA = (U32_WMV *)wmvMalloc(pWMVDec, sizeof( U32_WMV) *iMaxSliceBufSize, DHEAP_STRUCT);
    if (pWMVDec->m_puiNumBytesOfSliceWMVA == NULL) 
        return ICERR_ERROR;
#endif
    memset (pWMVDec->m_puiNumBytesOfSliceWMVA, 0, sizeof (U32_WMV) * iMaxSliceBufSize);
    
//#ifdef X9

    if (pWMVDec->m_cvCodecVersion == WMVA) {
#ifdef XDM
		pWMVDec->m_ppxliIntraRowY       = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof( I16_WMV) * iMaxNumMBX * 16 * 16;
		pWMVDec->m_ppxliIntraRowU       = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof( I16_WMV)* iMaxNumMBX * 8 * 8;
		pWMVDec->m_ppxliIntraRowV       = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof( I16_WMV)* iMaxNumMBX * 8 * 8;
#else
        pWMVDec->m_ppxliIntraRowY = (I16_WMV *)wmvMalloc(pWMVDec, sizeof( I16_WMV) * iMaxNumMBX * 16 * 16, DHEAP_STRUCT);
        pWMVDec->m_ppxliIntraRowU = (I16_WMV *)wmvMalloc(pWMVDec, sizeof( I16_WMV)* iMaxNumMBX * 8 * 8, DHEAP_STRUCT);
        pWMVDec->m_ppxliIntraRowV = (I16_WMV *)wmvMalloc(pWMVDec, sizeof( I16_WMV)* iMaxNumMBX * 8 * 8, DHEAP_STRUCT);
#endif
    }

    if (pWMVDec->m_bX9){
#ifdef XDM
		pWMVDec->m_pIntraBlockRowData = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I16_WMV) * (4+ iMaxNumMBX * ((pWMVDec->m_uiNumProcessors == 2) ? 1536 : 640));
#else
        pWMVDec->m_pIntraBlockRowData = (I16_WMV *)wmvMalloc(pWMVDec, sizeof(I16_WMV) * (4+ iMaxNumMBX * ((pWMVDec->m_uiNumProcessors == 2) ? 1536 : 640)), DHEAP_STRUCT);
        if (!pWMVDec->m_pIntraBlockRowData)
            return ICERR_MEMORY;
#endif

        pWMVDec->m_pX9dct = NULL;

        // YUV411
        if (pWMVDec->m_bYUV411) {
            // 4 Y Block of 16 Bytes each and U,V of 24 bytes each.
#ifdef XDM
			pWMVDec->m_pX9dct = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I16_WMV) * (iMaxNumMB * ( 4 * 16 + 2 * 24));
#else
            pWMVDec->m_pX9dct = (I16_WMV *)wmvMalloc(pWMVDec,  sizeof(I16_WMV)*(iMaxNumMB * ( 4 * 16 + 2 * 24)), DHEAP_STRUCT);
#endif
        } else {
#ifdef XDM
			pWMVDec->m_pX9dct = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I16_WMV) * (iMaxNumMB * 6 * 16);
#else
            pWMVDec->m_pX9dct = (I16_WMV *)wmvMalloc(pWMVDec,  sizeof(I16_WMV)*(iMaxNumMB * 6 * 16), DHEAP_STRUCT);
#endif
        }

        if (!pWMVDec->m_pX9dct)
            return ICERR_MEMORY;

#ifdef XDM
		pWMVDec->m_rgmv1 = (CMotionVector_X9 *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof( CMotionVector_X9)* 5 * iMaxNumMB;
#else
        pWMVDec->m_rgmv1 = (CMotionVector_X9 *)wmvMalloc(pWMVDec, sizeof( CMotionVector_X9)* 5 * iMaxNumMB, DHEAP_STRUCT);
        if (!pWMVDec->m_rgmv1)
            return ICERR_MEMORY;
#endif

#ifdef XDM
		pWMVDec->m_pInterpY0Buffer = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += (pWMVDec->m_uiNumProcessors * (2 * MB_SIZE * MB_SIZE + 4 * BLOCK_SIZE * BLOCK_SIZE + 32 + 160) + (pWMVDec->m_uiNumProcessors - 1) * 32);
#else
        pWMVDec->m_pInterpY0Buffer = (U8_WMV *)wmvMalloc(pWMVDec, pWMVDec->m_uiNumProcessors * (2 * MB_SIZE * MB_SIZE + 4 * BLOCK_SIZE * BLOCK_SIZE + 32 + 160) + (pWMVDec->m_uiNumProcessors - 1) * 32, DHEAP_STRUCT);
        if (pWMVDec->m_pInterpY0Buffer == NULL)
            return ICERR_MEMORY;
#endif

        pWMVDec->m_pMultiresBuffer = NULL;
        {
        I32_WMV iMax = pWMVDec->m_iWidthY * 6 + 128 + 32;
        if (iMax < pWMVDec->m_iHeightY * 4 + 256 + 32)
            iMax = pWMVDec->m_iHeightY * 4 + 256 + 32;

#ifdef XDM
			pWMVDec->m_pMultiresBuffer = (I8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
			pWMVDec->m_MemXDM.pPrivateBuff += iMax;
#else
        pWMVDec->m_pMultiresBuffer = (I8_WMV *)wmvMalloc( pWMVDec, iMax, DHEAP_STRUCT);
        if (!pWMVDec->m_pMultiresBuffer)
            return ICERR_MEMORY;
#endif
        }
        pWMVDec->m_pAlignedMultiresBuffer = (I8_WMV *)(((U32_WMV)pWMVDec->m_pMultiresBuffer + 31) & ~31);

        if (pWMVDec->m_bYUV411) {
            memset (pWMVDec->m_pX9dct, 0, iMaxNumMB * ( 4 * 16 + 2 * 24) * sizeof(I16_WMV));
        } else {
            memset (pWMVDec->m_pX9dct, 0, iMaxNumMB * 6 * 16 * sizeof(I16_WMV));
        }

#ifdef XDM
		pWMVDec->m_pFieldMvX = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I16_WMV) * iMaxNumMB * 8;
#else
        pWMVDec->m_pFieldMvX = (I16_WMV *)wmvMalloc( pWMVDec, sizeof(I16_WMV)*iMaxNumMB * 8, DHEAP_STRUCT);
        if (pWMVDec->m_pFieldMvX == NULL)
            return ICERR_MEMORY;
#endif

#ifdef XDM
		pWMVDec->m_pFieldMvX_Pbuf = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I16_WMV)*iMaxNumMBX * (iMaxNumMBY + 2) * 8;
#else
        pWMVDec->m_pFieldMvX_Pbuf = (I16_WMV *)wmvMalloc( pWMVDec, sizeof(I16_WMV)*iMaxNumMBX * (iMaxNumMBY + 2) * 8, DHEAP_STRUCT);
        if (pWMVDec->m_pFieldMvX_Pbuf == NULL)
            return ICERR_MEMORY;
#endif

        memset(pWMVDec->m_pFieldMvX_Pbuf, 0, sizeof(I16_WMV)*iMaxNumMBX * (iMaxNumMBY + 2) * 8);


#ifdef XDM
		pWMVDec->m_pB411InterpX = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I16_WMV)*iMaxNumMB * 8;
#else
        pWMVDec->m_pB411InterpX = (I16_WMV *)wmvMalloc( pWMVDec, sizeof(I16_WMV)*iMaxNumMB * 8, DHEAP_STRUCT);
        if (pWMVDec->m_pB411InterpX == NULL)
            return ICERR_MEMORY;
#endif
        pWMVDec->m_pB411InterpY = pWMVDec->m_pB411InterpX + iMaxNumMB * 4;

#ifdef XDM
		pWMVDec->m_pFieldMvX_FPred      = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I16_WMV)*iMaxNumMB * 8;
		pWMVDec->m_pFieldMvX_BPred      = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I16_WMV)*iMaxNumMB * 8;
#else
        pWMVDec->m_pFieldMvX_FPred = (I16_WMV *)wmvMalloc( pWMVDec, sizeof(I16_WMV)*iMaxNumMB * 8, DHEAP_STRUCT);
        if (pWMVDec->m_pFieldMvX_FPred == NULL)
            return ICERR_MEMORY;
        pWMVDec->m_pFieldMvX_BPred = (I16_WMV *)wmvMalloc( pWMVDec, sizeof(I16_WMV)*iMaxNumMB * 8, DHEAP_STRUCT);
        if (pWMVDec->m_pFieldMvX_BPred == NULL)
            return ICERR_MEMORY;
#endif
    }
#ifdef XDM
	pWMVDec->m_pLoopFilterFlags = (LOOPF_FLAG *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( LOOPF_FLAG ) *iMaxNumMB;
#else
    pWMVDec->m_pLoopFilterFlags = (LOOPF_FLAG *)wmvMalloc(pWMVDec, sizeof( LOOPF_FLAG ) *iMaxNumMB, DHEAP_STRUCT);
    if (!pWMVDec->m_pLoopFilterFlags)
        return ICERR_MEMORY;
#endif

//#endif // X9

//#ifdef INTERLACE
//#endif


#if !defined(_WIN32)
    //assert((pWMVDec->m_iClapTabCenter << 1) * sizeof(U8_WMV) < UINT_MAX);
#endif
    //end of original assignCommonDataMembers()

#ifdef XDM
	pWMVDec->m_rgmv = (CVector *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( CVector )* iMaxNumMBX * 2;
#else
    pWMVDec->m_rgmv = (CVector *)wmvMalloc(pWMVDec, sizeof( CVector )* iMaxNumMBX * 2, DHEAP_STRUCT);
    if (!pWMVDec->m_rgmv)
        return ICERR_MEMORY;
#endif

    if (pWMVDec->m_bBFrameOn)
    {
#ifdef XDM
		pWMVDec->m_rgmv_X9              = (CVector_X9 *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof(CVector_X9)*iMaxNumMBX * 2;
		pWMVDec->m_rgmvForwardPred      = (CVector_X9 *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof( CVector_X9)*iMaxNumMB;
		pWMVDec->m_rgmvBackwardPred     = (CVector_X9 *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof( CVector_X9)*iMaxNumMB;
#else
        pWMVDec->m_rgmv_X9           = (CVector_X9 *)wmvMalloc(pWMVDec, sizeof(CVector_X9)*iMaxNumMBX * 2, DHEAP_STRUCT);
        pWMVDec->m_rgmvForwardPred   = (CVector_X9 *)wmvMalloc(pWMVDec, sizeof( CVector_X9)*iMaxNumMB, DHEAP_STRUCT);
        pWMVDec->m_rgmvBackwardPred  = (CVector_X9 *)wmvMalloc(pWMVDec, sizeof( CVector_X9)*iMaxNumMB, DHEAP_STRUCT);
        if (!pWMVDec->m_rgmv_X9 || !pWMVDec->m_rgmvForwardPred || !pWMVDec->m_rgmvBackwardPred)
            return ICERR_MEMORY;
#endif
    }

    // MB data
#ifdef XDM
	pWMVDec->m_rgmbmd = (CWMVMBMode *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( CWMVMBMode ) *iMaxNumMB;
#else
    pWMVDec->m_rgmbmd = (CWMVMBMode *)wmvMalloc(pWMVDec, sizeof( CWMVMBMode ) *iMaxNumMB, DHEAP_STRUCT);
    if (!pWMVDec->m_rgmbmd)
        return ICERR_MEMORY;
#endif

    if (pWMVDec->m_bBFrameOn)
    {
#ifdef XDM
		pWMVDec->m_rgmbmd1 = (CWMVMBMode *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof( CWMVMBMode ) * iMaxNumMB;
#else
        pWMVDec->m_rgmbmd1 = (CWMVMBMode *)wmvMalloc(pWMVDec, sizeof( CWMVMBMode ) * iMaxNumMB, DHEAP_STRUCT);
        if (!pWMVDec->m_rgmbmd1)
            return ICERR_MEMORY;
#endif
        memset(pWMVDec->m_rgmbmd1, 0, sizeof(CWMVMBMode) * iMaxNumMB);
    }

#ifdef XDM
	pWMVDec->m_pmbmdZeroCBPCY = (CWMVMBMode *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( CWMVMBMode );
#else
    pWMVDec->m_pmbmdZeroCBPCY = (CWMVMBMode *)wmvMalloc(pWMVDec, sizeof( CWMVMBMode), DHEAP_STRUCT);
    if (!pWMVDec->m_pmbmdZeroCBPCY)
        return ICERR_MEMORY;
#endif

    pWMVDec->m_iNumOfQuanDctCoefForACPredPerRow = BLOCK_SIZE_TIMES2_TIMES6 * iMaxNumMBX; 
#ifdef XDM
	pWMVDec->m_rgiQuanCoefACPred    = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( I32_WMV) *iMaxNumMBX * 2 * BLOCK_SIZE_TIMES2_TIMES6;
	pWMVDec->m_rgiQuanCoefACPredTable = (I16_WMV **)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff   += sizeof( I32_WMV*) *iMaxNumMBX * 2  * 36;
#else
    pWMVDec->m_rgiQuanCoefACPred = (I16_WMV *)wmvMalloc(pWMVDec, sizeof( I32_WMV) *iMaxNumMBX * 2 * BLOCK_SIZE_TIMES2_TIMES6, DHEAP_STRUCT); // Allocate 2 rows buffer, 6 blocks in an MB
    if (!pWMVDec->m_rgiQuanCoefACPred)
        return ICERR_MEMORY;
    pWMVDec->m_rgiQuanCoefACPredTable = (I16_WMV **)wmvMalloc(pWMVDec, sizeof( I32_WMV*) *iMaxNumMBX * 2  * 36, DHEAP_STRUCT);
    if (!pWMVDec->m_rgiQuanCoefACPredTable)
        return ICERR_MEMORY;
#endif


#if !defined(_WIN32)
    assert(iMaxNumMB <= UINT_MAX);
#endif

#ifdef XDM
	pWMVDec->m_rgchSkipPrevFrame = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( U8_WMV) *iMaxNumMB;
#else
    pWMVDec->m_rgchSkipPrevFrame = (U8_WMV *)wmvMalloc(pWMVDec, sizeof( U8_WMV) *iMaxNumMB, DHEAP_STRUCT);
    if (pWMVDec->m_rgchSkipPrevFrame == NULL) {
        ;
        return ICERR_MEMORY;
    }
#endif

    memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) iMaxNumMB);

#ifdef _MULTITHREADED_RECON_
#ifdef XDM
	pWMVDec->m_rllArray = pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( I16_WMV) * (iMaxNumMB * 64 * 6 * 2 + iMaxNumMB);
#else
    pWMVDec->m_rllArray = wmvMalloc(pWMVDec, sizeof( I16_WMV) * (iMaxNumMB * 64 * 6 * 2 + iMaxNumMB), DHEAP_STRUCT);
    if (pWMVDec->m_rllArray == NULL) {
        ;
        return ICERR_MEMORY;
    }
#endif
#endif

#ifndef WMV9_SIMPLE_ONLY
    // Construct contexts, and DCT array
    if (pWMVDec->m_cvCodecVersion >= WMV2) {
        pWMVDec->m_pContext = t_ContextWMVConstruct (pWMVDec, iMaxNumMBX*2, 2);  // 2 rows enough
        if (!pWMVDec->m_pContext)
            return ICERR_MEMORY;
    }
#endif
   
#ifdef XDM
	pWMVDec->m_pXMotion = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( I16_WMV) * iMaxNumMBX * iMaxNumMBY * 8;
	pWMVDec->m_pXMotionC = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( I16_WMV) *iMaxNumMBX * iMaxNumMBY * 2;
#else
    pWMVDec->m_pXMotion = (I16_WMV *)wmvMalloc(pWMVDec, sizeof( I16_WMV) * iMaxNumMBX * iMaxNumMBY * 8, DHEAP_STRUCT);
    if (pWMVDec->m_pXMotion == NULL)
        return ICERR_MEMORY;

    pWMVDec->m_pXMotionC = (I16_WMV *)wmvMalloc(pWMVDec, sizeof( I16_WMV) *iMaxNumMBX * iMaxNumMBY * 2, DHEAP_STRUCT);
    if (pWMVDec->m_pXMotionC == NULL)
        return ICERR_MEMORY;
#endif

    // initialize it for error resilience
    pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmCurrQ;

#ifdef XDM
	pWMVDec->m_rgmv1_EMB = (CMotionVector_X9_EMB *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( CMotionVector_X9_EMB)* 5 * iMaxNumMB;
#else
    pWMVDec->m_rgmv1_EMB = (CMotionVector_X9_EMB*)wmvMalloc(pWMVDec, sizeof( CMotionVector_X9_EMB)* 5 * iMaxNumMB, DHEAP_STRUCT);
    if (pWMVDec->m_rgmv1_EMB == NULL)
        return ICERR_MEMORY;
#endif

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
  
    if (pWMVDec->m_bX9){
        pWMVDec->m_pIntraBlockRow0[0] = (I16_WMV *)(((U32_WMV) pWMVDec->m_pIntraBlockRowData + 7) & ~7);
        pWMVDec->m_pIntraBlockRow0[1] = pWMVDec->m_pIntraBlockRow0[0] + pWMVDec->m_uintNumMBX * 16 * 8;
        pWMVDec->m_pIntraBlockRow0[2] = pWMVDec->m_pIntraBlockRow0[1] + pWMVDec->m_uintNumMBX * 16 * 8;
        pWMVDec->m_pIntraMBRowU0[0]   = pWMVDec->m_pIntraBlockRow0[2] + pWMVDec->m_uintNumMBX * 16 * 8;
        pWMVDec->m_pIntraMBRowU0[1]   = pWMVDec->m_pIntraMBRowU0[0] + pWMVDec->m_uintNumMBX * 8 * 8;
        pWMVDec->m_pIntraMBRowV0[0]   = pWMVDec->m_pIntraMBRowU0[1] + pWMVDec->m_uintNumMBX * 8 * 8;
        pWMVDec->m_pIntraMBRowV0[1]   = pWMVDec->m_pIntraMBRowV0[0] + pWMVDec->m_uintNumMBX * 8 * 8;

        if (pWMVDec->m_uiNumProcessors == 2) {
            pWMVDec->m_pIntraBlockRow1[0] = pWMVDec->m_pIntraMBRowV0[1] + pWMVDec->m_uintNumMBX * 8 * 8;
            pWMVDec->m_pIntraBlockRow1[1] = pWMVDec->m_pIntraBlockRow1[0] + pWMVDec->m_uintNumMBX * 16 * 8;
            pWMVDec->m_pIntraBlockRow1[2] = pWMVDec->m_pIntraBlockRow1[1] + pWMVDec->m_uintNumMBX * 16 * 8;
            pWMVDec->m_pIntraBlockRow1[3] = pWMVDec->m_pIntraBlockRow1[2] + pWMVDec->m_uintNumMBX * 16 * 8;
            pWMVDec->m_pIntraMBRowU1[0]   = pWMVDec->m_pIntraBlockRow1[3] + pWMVDec->m_uintNumMBX * 16 * 8;
            pWMVDec->m_pIntraMBRowU1[1]   = pWMVDec->m_pIntraMBRowU1[0] + pWMVDec->m_uintNumMBX * 8 * 8;
            pWMVDec->m_pIntraMBRowU1[2]   = pWMVDec->m_pIntraMBRowU1[1] + pWMVDec->m_uintNumMBX * 8 * 8;
            pWMVDec->m_pIntraMBRowV1[0]   = pWMVDec->m_pIntraMBRowU1[2] + pWMVDec->m_uintNumMBX * 8 * 8;
            pWMVDec->m_pIntraMBRowV1[1]   = pWMVDec->m_pIntraMBRowV1[0] + pWMVDec->m_uintNumMBX * 8 * 8;
            pWMVDec->m_pIntraMBRowV1[2]   = pWMVDec->m_pIntraMBRowV1[1] + pWMVDec->m_uintNumMBX * 8 * 8;
        }

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
            pWMVDec->m_pX9dctU = pWMVDec->m_pX9dct + pWMVDec->m_uintNumMB * 4 * 16;
            pWMVDec->m_pX9dctV = pWMVDec->m_pX9dctU + pWMVDec->m_uintNumMB * 16;
        }

        pWMVDec->m_pFieldMvY = pWMVDec->m_pFieldMvX + pWMVDec->m_uintNumMB * 4;
        pWMVDec->m_pFieldMvY_Pbuf = pWMVDec->m_pFieldMvX_Pbuf + (pWMVDec->m_uintNumMBX * (pWMVDec->m_uintNumMBY + 2)) * 4;

        pWMVDec->m_pB411InterpY = pWMVDec->m_pB411InterpX + pWMVDec->m_uintNumMB * 4;

        pWMVDec->m_pFieldMvY_FPred = pWMVDec->m_pFieldMvX_FPred + pWMVDec->m_uintNumMB * 4;
        pWMVDec->m_pFieldMvY_BPred = pWMVDec->m_pFieldMvX_BPred + pWMVDec->m_uintNumMB * 4;

    }

//#endif // X9

//#ifdef INTERLACE
//#endif


#if !defined(_WIN32)
    //assert((pWMVDec->m_iClapTabCenter << 1) * sizeof(U8_WMV) < UINT_MAX);
#endif
    //end of original assignCommonDataMembers()

    
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
    pWMVDec->m_iWidthPrevYTimes8Minus8 = pWMVDec->m_iWidthPrevY * 8 - 8;
    pWMVDec->m_iWidthPrevUVTimes4Minus4 = pWMVDec->m_iWidthPrevUV * 4 - 4;

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
                    pDct[0] = pWMVDec->m_pAvgQuanDctCoefDec;
                    pDct[1] = pDct1 - BLOCK_SIZE_TIMES2_TIMES5; 
                    pDct[2] = pWMVDec->m_pAvgQuanDctCoefDec;
                    pDct[3] = pDct1 - iNumOfQuanDctCoefForACPredPerRow+ BLOCK_SIZE_TIMES2_TIMES2;
                    pDct[4] = pWMVDec->m_pAvgQuanDctCoefDec;
                    pDct[5] = pDct1 - iNumOfQuanDctCoefForACPredPerRow - BLOCK_SIZE_TIMES2_TIMES3;
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
    pWMVDec->m_pYMotionC = pWMVDec->m_pXMotionC + pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY;

    // initialize it for error resilience
    pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmCurrQ;
}

//*****************************************
// Initialization of all Huffman tables
//*****************************************
I32_WMV initVlcTables(tWMVDecInternalMember *pWMVDec)
{
    
    VResultCode vr;

    I32_WMV maxBits[3];
    maxBits[0] = 6; maxBits[1] = 5; maxBits[2] = 6;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMVDec_Talking, pWMVDec->m_uiUserData, sm_HufMVTable_Talking, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    maxBits[0] = 5,  maxBits[1] = 5,  maxBits[2] = 5;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMVDec_HghMt, pWMVDec->m_uiUserData, sm_HufMVTable_HghMt, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    ///#ifdef __TALKING_HEADER
    maxBits[0] = 6,  maxBits[1] = 3,  maxBits[2] = 4;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufICBPCYDec, pWMVDec->m_uiUserData, sm_HufICBPCYTable, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    maxBits[0] = 7; maxBits[1] = 6; maxBits[2] = 8;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec, pWMVDec->m_uiUserData, sm_HufPCBPCYTable, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    //NEW_PCBPCY_TABLE
    maxBits[0] = 9; maxBits[1] = 7; maxBits[2] = 8;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_HighRate, pWMVDec->m_uiUserData, sm_HufPCBPCYTable_HighRate, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    maxBits[0] = 7; maxBits[1] = 6; maxBits[2] = 7;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_MidRate, pWMVDec->m_uiUserData, sm_HufPCBPCYTable_MidRate, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    maxBits[0] = 6; maxBits[1] = 6; maxBits[2] = 5;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_LowRate, pWMVDec->m_uiUserData, sm_HufPCBPCYTable_LowRate, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    maxBits[0] = 5; maxBits[1] = 4; maxBits[2] = 4;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_1, pWMVDec->m_uiUserData, sm_HufPCBPCYTable_V9_1, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 4;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_2, pWMVDec->m_uiUserData, sm_HufPCBPCYTable_V9_2, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    maxBits[0] = 5; maxBits[1] = 4; maxBits[2] = 4;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_3, pWMVDec->m_uiUserData, sm_HufPCBPCYTable_V9_3, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    maxBits[0] = 3; maxBits[1] = 3; maxBits[2] = 3;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_4, pWMVDec->m_uiUserData, sm_HufPCBPCYTable_V9_4, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    pWMVDec->m_pHufPCBPCYTables[0] = &pWMVDec->m_hufPCBPCYDec_V9_1;
    pWMVDec->m_pHufPCBPCYTables[1] = &pWMVDec->m_hufPCBPCYDec_V9_2;
    pWMVDec->m_pHufPCBPCYTables[2] = &pWMVDec->m_hufPCBPCYDec_V9_3;
    pWMVDec->m_pHufPCBPCYTables[3] = &pWMVDec->m_hufPCBPCYDec_V9_4;

    maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 4;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV1, pWMVDec->m_uiUserData, sm_HufMVTable_V9_MV1, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 4;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV2, pWMVDec->m_uiUserData, sm_HufMVTable_V9_MV2, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    maxBits[0] = 4; maxBits[1] = 4; maxBits[2] = 4;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV3, pWMVDec->m_uiUserData, sm_HufMVTable_V9_MV3, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 5;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV4, pWMVDec->m_uiUserData, sm_HufMVTable_V9_MV4, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    pWMVDec->m_pHufMVTables[0] = &pWMVDec->m_HufMVTable_V9_MV1;
    pWMVDec->m_pHufMVTables[1] = &pWMVDec->m_HufMVTable_V9_MV2;
    pWMVDec->m_pHufMVTables[2] = &pWMVDec->m_HufMVTable_V9_MV3;
    pWMVDec->m_pHufMVTables[3] = &pWMVDec->m_HufMVTable_V9_MV4;

    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_huf4x4PatternHighQPDec, pWMVDec->m_uiUserData, sm_Huf4x4PatternHighQPTable, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_huf4x4PatternMidQPDec, pWMVDec->m_uiUserData, sm_Huf4x4PatternMidQPTable, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_huf4x4PatternLowQPDec, pWMVDec->m_uiUserData, sm_Huf4x4PatternLowQPTable, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMBXformTypeLowQPDec, pWMVDec->m_uiUserData, sm_HufMBXformTypeLowQPTable, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMBXformTypeMidQPDec, pWMVDec->m_uiUserData, sm_HufMBXformTypeMidQPTable, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufMBXformTypeHighQPDec, pWMVDec->m_uiUserData, sm_HufMBXformTypeHighQPTable, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufBlkXformTypeLowQPDec, pWMVDec->m_uiUserData, sm_HufBlkXformTypeLowQPTable, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufBlkXformTypeMidQPDec, pWMVDec->m_uiUserData, sm_HufBlkXformTypeMidQPTable, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufBlkXformTypeHighQPDec, pWMVDec->m_uiUserData, sm_HufBlkXformTypeHighQPTable, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

#ifndef PPCWMP
    if (pWMVDec->m_cvCodecVersion == WMVA) {
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode0, pWMVDec->m_uiUserData, sm_HufInterlaceFrame4MvMBModeTable0, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode1, pWMVDec->m_uiUserData, sm_HufInterlaceFrame4MvMBModeTable1, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode2, pWMVDec->m_uiUserData, sm_HufInterlaceFrame4MvMBModeTable2, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode3, pWMVDec->m_uiUserData, sm_HufInterlaceFrame4MvMBModeTable3, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        pWMVDec->m_pInterlaceFrame4MvMBModeTables[0] = &pWMVDec->m_hufInterlaceFrame4MvMBMode0;
        pWMVDec->m_pInterlaceFrame4MvMBModeTables[1] = &pWMVDec->m_hufInterlaceFrame4MvMBMode1;
        pWMVDec->m_pInterlaceFrame4MvMBModeTables[2] = &pWMVDec->m_hufInterlaceFrame4MvMBMode2;
        pWMVDec->m_pInterlaceFrame4MvMBModeTables[3] = &pWMVDec->m_hufInterlaceFrame4MvMBMode3;

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode0, pWMVDec->m_uiUserData, sm_HufInterlaceFrame1MvMBModeTable0, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode1, pWMVDec->m_uiUserData, sm_HufInterlaceFrame1MvMBModeTable1, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode2, pWMVDec->m_uiUserData, sm_HufInterlaceFrame1MvMBModeTable2, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode3, pWMVDec->m_uiUserData, sm_HufInterlaceFrame1MvMBModeTable3, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        pWMVDec->m_pInterlaceFrame1MvMBModeTables[0] = &pWMVDec->m_hufInterlaceFrame1MvMBMode0;
        pWMVDec->m_pInterlaceFrame1MvMBModeTables[1] = &pWMVDec->m_hufInterlaceFrame1MvMBMode1;
        pWMVDec->m_pInterlaceFrame1MvMBModeTables[2] = &pWMVDec->m_hufInterlaceFrame1MvMBMode2;
        pWMVDec->m_pInterlaceFrame1MvMBModeTables[3] = &pWMVDec->m_hufInterlaceFrame1MvMBMode3;

        maxBits[0] = 5; maxBits[1] = 4; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_1, pWMVDec->m_uiUserData, sm_HufCBPCYTable_Interlace_1, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_2, pWMVDec->m_uiUserData, sm_HufCBPCYTable_Interlace_2, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 5; maxBits[1] = 4; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_3, pWMVDec->m_uiUserData, sm_HufCBPCYTable_Interlace_3, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 3; maxBits[1] = 3; maxBits[2] = 3;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_4, pWMVDec->m_uiUserData, sm_HufCBPCYTable_Interlace_4, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 5; maxBits[1] = 4; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_5, pWMVDec->m_uiUserData, sm_HufCBPCYTable_Interlace_5, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_6, pWMVDec->m_uiUserData, sm_HufCBPCYTable_Interlace_6, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 5; maxBits[1] = 4; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_7, pWMVDec->m_uiUserData, sm_HufCBPCYTable_Interlace_7, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 3; maxBits[1] = 3; maxBits[2] = 3;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_8, pWMVDec->m_uiUserData, sm_HufCBPCYTable_Interlace_8, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;

        pWMVDec->m_pHufInterlaceCBPCYTables[0] = &pWMVDec->m_hufCBPCYDec_Interlace_1;
        pWMVDec->m_pHufInterlaceCBPCYTables[1] = &pWMVDec->m_hufCBPCYDec_Interlace_2;
        pWMVDec->m_pHufInterlaceCBPCYTables[2] = &pWMVDec->m_hufCBPCYDec_Interlace_3;
        pWMVDec->m_pHufInterlaceCBPCYTables[3] = &pWMVDec->m_hufCBPCYDec_Interlace_4;
        pWMVDec->m_pHufInterlaceCBPCYTables[4] = &pWMVDec->m_hufCBPCYDec_Interlace_5;
        pWMVDec->m_pHufInterlaceCBPCYTables[5] = &pWMVDec->m_hufCBPCYDec_Interlace_6;
        pWMVDec->m_pHufInterlaceCBPCYTables[6] = &pWMVDec->m_hufCBPCYDec_Interlace_7;
        pWMVDec->m_pHufInterlaceCBPCYTables[7] = &pWMVDec->m_hufCBPCYDec_Interlace_8;

        maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV1, pWMVDec->m_uiUserData, sm_HufMVTable_Interlace_MV1, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV2, pWMVDec->m_uiUserData, sm_HufMVTable_Interlace_MV2, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 4; maxBits[1] = 4; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV3, pWMVDec->m_uiUserData, sm_HufMVTable_Interlace_MV3, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 5;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV4, pWMVDec->m_uiUserData, sm_HufMVTable_Interlace_MV4, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV5, pWMVDec->m_uiUserData, sm_HufMVTable_Interlace_MV5, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV6, pWMVDec->m_uiUserData, sm_HufMVTable_Interlace_MV6, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 4; maxBits[1] = 4; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV7, pWMVDec->m_uiUserData, sm_HufMVTable_Interlace_MV7, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 5;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV8, pWMVDec->m_uiUserData, sm_HufMVTable_Interlace_MV8, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;

 

        maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV1, pWMVDec->m_uiUserData, sm_HufMVTable_Progressive_MV1, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV2, pWMVDec->m_uiUserData, sm_HufMVTable_Progressive_MV2, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 4; maxBits[1] = 4; maxBits[2] = 4;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV3, pWMVDec->m_uiUserData, sm_HufMVTable_Progressive_MV3, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        maxBits[0] = 5; maxBits[1] = 5; maxBits[2] = 5;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV4, pWMVDec->m_uiUserData, sm_HufMVTable_Progressive_MV4, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV1, pWMVDec->m_uiUserData, sm_HufFieldPicture1MVMBModeTable_1, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV2, pWMVDec->m_uiUserData, sm_HufFieldPicture1MVMBModeTable_2, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV3, pWMVDec->m_uiUserData, sm_HufFieldPicture1MVMBModeTable_3, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV4, pWMVDec->m_uiUserData, sm_HufFieldPicture1MVMBModeTable_4, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV5, pWMVDec->m_uiUserData, sm_HufFieldPicture1MVMBModeTable_5, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV6, pWMVDec->m_uiUserData, sm_HufFieldPicture1MVMBModeTable_6, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV7, pWMVDec->m_uiUserData, sm_HufFieldPicture1MVMBModeTable_7, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV8, pWMVDec->m_uiUserData, sm_HufFieldPicture1MVMBModeTable_8, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;

        pWMVDec->m_pHufMBMode1MVTables[0] = &pWMVDec->m_hufFieldPictureMBMode1MV1;
        pWMVDec->m_pHufMBMode1MVTables[1] = &pWMVDec->m_hufFieldPictureMBMode1MV2;
        pWMVDec->m_pHufMBMode1MVTables[2] = &pWMVDec->m_hufFieldPictureMBMode1MV3;
        pWMVDec->m_pHufMBMode1MVTables[3] = &pWMVDec->m_hufFieldPictureMBMode1MV4;
        pWMVDec->m_pHufMBMode1MVTables[4] = &pWMVDec->m_hufFieldPictureMBMode1MV5;
        pWMVDec->m_pHufMBMode1MVTables[5] = &pWMVDec->m_hufFieldPictureMBMode1MV6;
        pWMVDec->m_pHufMBMode1MVTables[6] = &pWMVDec->m_hufFieldPictureMBMode1MV7;
        pWMVDec->m_pHufMBMode1MVTables[7] = &pWMVDec->m_hufFieldPictureMBMode1MV8;

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV1, pWMVDec->m_uiUserData, sm_HufFieldPictureMixedMVMBModeTable_1, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV2, pWMVDec->m_uiUserData, sm_HufFieldPictureMixedMVMBModeTable_2, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV3, pWMVDec->m_uiUserData, sm_HufFieldPictureMixedMVMBModeTable_3, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV4, pWMVDec->m_uiUserData, sm_HufFieldPictureMixedMVMBModeTable_4, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV5, pWMVDec->m_uiUserData, sm_HufFieldPictureMixedMVMBModeTable_5, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV6, pWMVDec->m_uiUserData, sm_HufFieldPictureMixedMVMBModeTable_6, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV7, pWMVDec->m_uiUserData, sm_HufFieldPictureMixedMVMBModeTable_7, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV8, pWMVDec->m_uiUserData, sm_HufFieldPictureMixedMVMBModeTable_8, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;

        pWMVDec->m_pHufMBModeMixedMVTables[0] = &pWMVDec->m_hufFieldPictureMBModeMixedMV1;
        pWMVDec->m_pHufMBModeMixedMVTables[1] = &pWMVDec->m_hufFieldPictureMBModeMixedMV2;
        pWMVDec->m_pHufMBModeMixedMVTables[2] = &pWMVDec->m_hufFieldPictureMBModeMixedMV3;
        pWMVDec->m_pHufMBModeMixedMVTables[3] = &pWMVDec->m_hufFieldPictureMBModeMixedMV4;
        pWMVDec->m_pHufMBModeMixedMVTables[4] = &pWMVDec->m_hufFieldPictureMBModeMixedMV5;
        pWMVDec->m_pHufMBModeMixedMVTables[5] = &pWMVDec->m_hufFieldPictureMBModeMixedMV6;
        pWMVDec->m_pHufMBModeMixedMVTables[6] = &pWMVDec->m_hufFieldPictureMBModeMixedMV7;
        pWMVDec->m_pHufMBModeMixedMVTables[7] = &pWMVDec->m_hufFieldPictureMBModeMixedMV8;

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP1, pWMVDec->m_uiUserData, sm_HufFieldPictureMVBP_1, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP2, pWMVDec->m_uiUserData, sm_HufFieldPictureMVBP_2, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP3, pWMVDec->m_uiUserData, sm_HufFieldPictureMVBP_3, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP4, pWMVDec->m_uiUserData, sm_HufFieldPictureMVBP_4, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;

        pWMVDec->m_p4MVBPTables[0] = &pWMVDec->m_hufFieldPictureMVBP1;
        pWMVDec->m_p4MVBPTables[1] = &pWMVDec->m_hufFieldPictureMVBP2;
        pWMVDec->m_p4MVBPTables[2] = &pWMVDec->m_hufFieldPictureMVBP3;
        pWMVDec->m_p4MVBPTables[3] = &pWMVDec->m_hufFieldPictureMVBP4;

        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP1, pWMVDec->m_uiUserData, sm_HufInterlaceFrame2MVBP_1, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP2, pWMVDec->m_uiUserData, sm_HufInterlaceFrame2MVBP_2, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP3, pWMVDec->m_uiUserData, sm_HufInterlaceFrame2MVBP_3, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;
        vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP4, pWMVDec->m_uiUserData, sm_HufInterlaceFrame2MVBP_4, HUFFMAN_DECODE_ROOT_BITS);
        if (vr != vrNoError) goto lerror;

        pWMVDec->m_p2MVBPTables[0] = &pWMVDec->m_hufInterlaceFrame2MVBP1;
        pWMVDec->m_p2MVBPTables[1] = &pWMVDec->m_hufInterlaceFrame2MVBP2;
        pWMVDec->m_p2MVBPTables[2] = &pWMVDec->m_hufInterlaceFrame2MVBP3;
        pWMVDec->m_p2MVBPTables[3] = &pWMVDec->m_hufInterlaceFrame2MVBP4;

    }
#endif //PPCWMP

    ///#endif
    //_DCTDC
    maxBits[0] = 8; maxBits[1] = 8; maxBits[2] = 8;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCyDec_Talking, pWMVDec->m_uiUserData, sm_HufDCTDCyTable_Talking, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    maxBits[0] = 8; maxBits[1] = 7; maxBits[2] = 8;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCcDec_Talking, pWMVDec->m_uiUserData, sm_HufDCTDCcTable_Talking, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    maxBits[0] = 9; maxBits[1] = 9; maxBits[2] = 8;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCyDec_HghMt, pWMVDec->m_uiUserData, sm_HufDCTDCyTable_HghMt, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    
    maxBits[0] = 9; maxBits[1] = 8; maxBits[2] = 8; 
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTDCcDec_HghMt, pWMVDec->m_uiUserData, sm_HufDCTDCcTable_HghMt, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    // _DCTAC
    maxBits[0] = 6,  maxBits[1] = 5,  maxBits[2] = 4;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_HghMt, pWMVDec->m_uiUserData, sm_HufDCTACInterTable_HghMt, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    maxBits[0] = 7; maxBits[1] = 4; maxBits[2] = 4;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_HghMt, pWMVDec->m_uiUserData, sm_HufDCTACIntraTable_HghMt, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    maxBits[0] = 7; maxBits[1] = 4; maxBits[2] = 4;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_Talking, pWMVDec->m_uiUserData, sm_HufDCTACInterTable_Talking, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    maxBits[0] = 5; maxBits[1] = 4; maxBits[2] = 4;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_Talking, pWMVDec->m_uiUserData, sm_HufDCTACIntraTable_Talking, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;
    
    maxBits[0] = 3; maxBits[1] = 6; maxBits[2] = 3;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_MPEG4, pWMVDec->m_uiUserData, sm_HufDCTACInterTable_MPEG4, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    maxBits[0] = 3; maxBits[1] = 6; maxBits[2] = 3;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_MPEG4, pWMVDec->m_uiUserData, sm_HufDCTACIntraTable_MPEG4, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    maxBits[0] = 8,  maxBits[1] = 8,  maxBits[2] = 7;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACInterDec_HghRate, pWMVDec->m_uiUserData, sm_HufDCTACInterTable_HghRate, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    maxBits[0] = 6,  maxBits[1] = 6,  maxBits[2] = 4;
    vr = Huffman_WMV_init(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_HghRate, pWMVDec->m_uiUserData, sm_HufDCTACIntraTable_HghRate, HUFFMAN_DECODE_ROOT_BITS);
    if (vr != vrNoError) goto lerror;

    return ICERR_OK;
lerror:
    return ICERR_MEMORY;
}

#if 0 //sw: Only called by VodecDestruct_WMVA(). But VodecDestruct_WMVA() is not called by any functions.20080108
//*****************************************
// Initialization of all Huffman tables
//*****************************************
Void_WMV FreeVlcTables(tWMVDecInternalMember *pWMVDec)
{
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufMVDec_Talking);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufMVDec_HghMt);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufICBPCYDec);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_HighRate);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_MidRate);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_LowRate);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_1);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_2);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_3);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufPCBPCYDec_V9_4);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV1);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV2);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV3);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_V9_MV4);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_huf4x4PatternHighQPDec);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_huf4x4PatternMidQPDec);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_huf4x4PatternLowQPDec);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufMBXformTypeLowQPDec);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufMBXformTypeMidQPDec);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufMBXformTypeHighQPDec);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufBlkXformTypeLowQPDec);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufBlkXformTypeMidQPDec);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufBlkXformTypeHighQPDec);

    if (pWMVDec->m_cvCodecVersion == WMVA) {
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode0);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame4MvMBMode3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode0);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame1MvMBMode3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_4);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_5);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_6);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_7);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufCBPCYDec_Interlace_8);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV4);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV5);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV6);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV7);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Interlace_MV8);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_HufMVTable_Progressive_MV4);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV4);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV5);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV6);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV7);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBMode1MV8);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV4);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV5);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV6);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV7);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMBModeMixedMV8);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufFieldPictureMVBP4);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP1);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP2);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP3);
        Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufInterlaceFrame2MVBP4);
    }

    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTDCyDec_Talking);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTDCcDec_Talking);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTDCyDec_HghMt);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTDCcDec_HghMt);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACInterDec_HghMt);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_HghMt);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACInterDec_Talking);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_Talking);
    //Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACInterDec_MMIDRATE);
    //Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_MMIDRATE);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACInterDec_HghRate);
    Huffman_WMV_destruct(pWMVDec, &pWMVDec->m_hufDCTACIntraDec_HghRate);
}
#endif

tWMVDecodeStatus setVlcTableInfo_WMVA(tWMVDecInternalMember *pWMVDec)
{
    tWMVDecodeStatus status=WMV_Succeeded;
    /// Set tables for High Motion
    setTable(
         pWMVDec->InterDCTTableInfo_Dec_HghMt,
        &pWMVDec->m_hufDCTACInterDec_HghMt,
        UVTCOEF_ESCAPE_HghMt,
        NUMOFCODESINNOTLASTINTERMinus1_HghMt,
        //STARTINDEXOFLASTRUN_HghMt, 
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
        //STARTINDEXOFLASTRUN_HghMt, 
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
        //STARTINDEXOFLASTRUN_Talking, 
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
        //STARTINDEXOFLASTRUN_Talking, 
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
        //STARTINDEXOFLASTRUN_MPEG4, 
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
        //STARTINDEXOFLASTRUN_MPEG4, 
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


    pWMVDec->m_pHufMVDec_Set[0] = &pWMVDec->m_hufMVDec_Talking;
    pWMVDec->m_pHufMVDec_Set[1] = &pWMVDec->m_hufMVDec_HghMt;
    pWMVDec->m_puMvFromIndex_Set[0] = sm_uXMvFromIndex_Talking;
    pWMVDec->m_puMvFromIndex_Set[1] = sm_uYMvFromIndex_Talking;
    pWMVDec->m_puMvFromIndex_Set[2] = sm_uXMvFromIndex_HghMt;
    pWMVDec->m_puMvFromIndex_Set[3] = sm_uYMvFromIndex_HghMt;

    return status;

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
    pWMVDec->m_iRefFrameDistance = 0;
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

    assert(pWMVDec->m_cvCodecVersion == WMVA);   //only allowed in WMVA.

        //reset necessary flags
    ResetConditionalVariablesForSequenceSwitch(pWMVDec);

    //If the first frame has seq header, it cannot check mem realloc, 
    // since GetExtendedFormat() already did that, otherwise m_bLargerSeqBufferRequired will be reset.
    //pWMVDec->m_bCheckMaxSeqFrameArea = pWMVDec->m_t > 0;   

    //decodeSequenceHead_Advanced() will free previous buffer if needed. Realloc in SwitchEntryPoint().
    result = decodeSequenceHead_Advanced(pWMVDec, 0, 0, 0, 0, 0);
    if (ICERR_OK != result) {
        return result;
    }

    if (pWMVDec->m_iMaxFrmWidthSrc * pWMVDec->m_iMaxFrmHeightSrc > pWMVDec->m_iMaxPrevSeqFrameArea)
    {
        pWMVDec->m_iMaxPrevSeqFrameArea = pWMVDec->m_iMaxFrmWidthSrc * pWMVDec->m_iMaxFrmHeightSrc;
        pWMVDec->m_bLargerSeqBufferRequired = TRUE;

        FreeFrameAreaDependentMemory(pWMVDec);

        result = initFrameAreaDependentMemory(pWMVDec, pWMVDec->m_iMaxFrmWidthSrc, pWMVDec->m_iMaxFrmHeightSrc);
        if (ICERR_OK != result)
            return result;

        //printf("Reallocate all memory.\n");
    } else {
        //skip unnecessary init in WMVA when the new seq frame size is same as the old one
        pWMVDec->m_bLargerSeqBufferRequired = FALSE;
    }

    return ICERR_OK;
}

Void_WMV GetSeqFrameSize(tWMVDecInternalMember *pWMVDec, I32_WMV *piNewSeqFrmWidth, I32_WMV *piNewSeqFrmHeight)
{
    *piNewSeqFrmWidth = pWMVDec->m_iMaxFrmWidthSrc;
    *piNewSeqFrmHeight = pWMVDec->m_iMaxFrmHeightSrc;
}

#endif
/************************************************************************************
  SwapMVTables : switches between tables used in interlace and progressive decoding
  based on frame type
************************************************************************************/
Void_WMV SwapMVTables (tWMVDecInternalMember *pWMVDec)
{
    if (pWMVDec->m_bInterlaceV2) {
        if (pWMVDec->m_bFieldMode) {
            if (pWMVDec->m_bTwoRefPictures) {
                // interlace MV tables

        pWMVDec->m_pHufMVTables[0] = &pWMVDec->m_HufMVTable_Interlace_MV1;
        pWMVDec->m_pHufMVTables[1] = &pWMVDec->m_HufMVTable_Interlace_MV2;
        pWMVDec->m_pHufMVTables[2] = &pWMVDec->m_HufMVTable_Interlace_MV3;
        pWMVDec->m_pHufMVTables[3] = &pWMVDec->m_HufMVTable_Interlace_MV4;
        pWMVDec->m_pHufMVTables[4] = &pWMVDec->m_HufMVTable_Interlace_MV5;
        pWMVDec->m_pHufMVTables[5] = &pWMVDec->m_HufMVTable_Interlace_MV6;
        pWMVDec->m_pHufMVTables[6] = &pWMVDec->m_HufMVTable_Interlace_MV7;
        pWMVDec->m_pHufMVTables[7] = &pWMVDec->m_HufMVTable_Interlace_MV8;
    }
            else {
                // progressive MV tables
                pWMVDec->m_pHufMVTables[0] = &pWMVDec->m_HufMVTable_Progressive_MV1;
                pWMVDec->m_pHufMVTables[1] = &pWMVDec->m_HufMVTable_Progressive_MV2;
                pWMVDec->m_pHufMVTables[2] = &pWMVDec->m_HufMVTable_Progressive_MV3;
                pWMVDec->m_pHufMVTables[3] = &pWMVDec->m_HufMVTable_Progressive_MV4;
            }
        } else {
            pWMVDec->m_pHufMVTables[0] = &pWMVDec->m_HufMVTable_Progressive_MV1;
            pWMVDec->m_pHufMVTables[1] = &pWMVDec->m_HufMVTable_Progressive_MV2;
            pWMVDec->m_pHufMVTables[2] = &pWMVDec->m_HufMVTable_Progressive_MV3;
            pWMVDec->m_pHufMVTables[3] = &pWMVDec->m_HufMVTable_Progressive_MV4;
        }
    }

    else { // progressive tables

        pWMVDec->m_pHufMVTables[0] = &pWMVDec->m_HufMVTable_V9_MV1;
        pWMVDec->m_pHufMVTables[1] = &pWMVDec->m_HufMVTable_V9_MV2;
        pWMVDec->m_pHufMVTables[2] = &pWMVDec->m_HufMVTable_V9_MV3;
        pWMVDec->m_pHufMVTables[3] = &pWMVDec->m_HufMVTable_V9_MV4;
    }
}

#ifdef _WMV9AP_
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
#ifdef VIDEO_MEM_SHARE
	if( pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP ) 
	{
		pWMVDec->m_iWidthPrevY = pWMVDec->m_frameBufStrideY;
		pWMVDec->m_iWidthPrevUV = pWMVDec->m_frameBufStrideUV;
	}
	else
#endif
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



    // guard against small height images when multi-proc encoding is on.
    if (pWMVDec->m_uiNumProcessors == 4) {
        if (pWMVDec->m_uintNumMBY < 4) {
            pWMVDec->m_uiNumProcessors = (pWMVDec->m_uintNumMBY >= 2) ? 2 : 1;
        }
    } else if (pWMVDec->m_uiNumProcessors == 2) {
        if (pWMVDec->m_uintNumMBY == 1) {
            pWMVDec->m_uiNumProcessors = 1;
        }
    }

    //Zone_VLC
    pWMVDec->m_rgiEndIndexOfCurrZone [0] = END1STZONE;
    pWMVDec->m_rgiEndIndexOfCurrZone [1] = END2NDZONE;
    pWMVDec->m_rgiEndIndexOfCurrZone [2] = 64;
    pWMVDec->m_bInterZoneVLCTable = FALSE;

#ifndef WMV9_SIMPLE_ONLY
    SetupMultiResParams(pWMVDec);
#endif

    //slice
    pWMVDec->m_iSliceBufSize = ((pWMVDec->m_iHeightY >> 4) + 1);

    //pWMVDec->m_cColorControl.init(pWMVDec->m_iWidthPrevY,pWMVDec->m_iWidthPrevUV,g_InitDecGlobalVars ());
    //pWMVDec->m_cColorControl.initFrameVars(pWMVDec->m_iWidthY,pWMVDec->m_iWidthUV, pWMVDec->m_iHeightY, pWMVDec->m_iHeightUV);


}
#endif

//#endif

#ifdef IPAQ_HACK
I32_WMV ipaq_hack(U8_WMV * * ppData, I32_WMV size);

#   define CHECK_MALLOC2(x,y)               \
{                                       \
    I32_WMV reslt=ipaq_hack(&ppxlTmp,y);  \
    x = ppxlTmp;                     \
    if (ppxlTmp == NULL_WMV)            \
    return WMV_BadMemory;           \
    DEBUG_HEAP_ADD(DHEAP_FRAMES,y);                 \
}
#else
#   define CHECK_MALLOC2(x,y) DEBUG_HEAP_ADD(DHEAP_FRAMES,y);
#endif

#ifndef DISABLE_UNUSED_CODE_INIT
Void_WMV computePars (tWMVDecInternalMember *pWMVDec)
{
    //if (pWMVDec->m_cvCodecVersion != MP4S) {
    pWMVDec->EXPANDY_REFVOP = 32;
    pWMVDec->EXPANDUV_REFVOP = 16;
#if defined( _EMB_WMV3_) && !defined(_64BIT_)
#if defined(macintosh) || defined(_ARM_)
    if ((!g_bSupportAltiVec_WMV) && (!g_bSupportWMMX_WMV))
#endif
    {
        if (pWMVDec->m_cvCodecVersion == WMV3) {
            pWMVDec->EXPANDY_REFVOP = 24;
            pWMVDec->EXPANDUV_REFVOP = 12;
        }
    }
#endif

    g_pRepeatRef0Y = g_RepeatRef0Y_32; 
    g_pRepeatRef0UV = g_RepeatRef0UV_16; 

#ifdef _EMB_WMV3_
#if defined(macintosh) || defined(_ARM_)
    if ((!g_bSupportAltiVec_WMV) && (!g_bSupportWMMX_WMV))
#endif
    {
        if (pWMVDec->m_cvCodecVersion == WMV3) {
            if(pWMVDec->EXPANDY_REFVOP == 24)
            {
                g_pRepeatRef0Y = g_RepeatRef0Y_24; 
                g_pRepeatRef0UV = g_RepeatRef0UV_12; 
            }
            else if(pWMVDec->EXPANDY_REFVOP == 32)
            {
                g_pRepeatRef0Y = g_RepeatRef0Y_32; 
                g_pRepeatRef0UV = g_RepeatRef0UV_16; 
            }
        }
    }
#endif
    pWMVDec->m_iWidthYRepeatPad = pWMVDec->m_iWidthY;
    pWMVDec->m_iWidthUVRepeatPad = pWMVDec->m_iWidthUV;
    pWMVDec->m_iHeightYRepeatPad = pWMVDec->m_iHeightY;
    pWMVDec->m_iHeightUVRepeatPad = pWMVDec->m_iHeightUV;
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
    pWMVDec->m_iWidthPrevY = pWMVDec->m_iWidthY + (pWMVDec->EXPANDY_REFVOP << 1);
    pWMVDec->m_iWidthPrevUV = pWMVDec->m_iWidthUV + (pWMVDec->EXPANDUV_REFVOP << 1);
    pWMVDec->m_iHeightPrevY = pWMVDec->m_iHeightY + (pWMVDec->EXPANDY_REFVOP << 1);
    pWMVDec->m_iHeightPrevUV = pWMVDec->m_iHeightUV + (pWMVDec->EXPANDUV_REFVOP << 1);

    // for update
    pWMVDec->m_iWidthPrevYXExpPlusExp = pWMVDec->m_iWidthPrevY * pWMVDec->EXPANDY_REFVOP + pWMVDec->EXPANDY_REFVOP;
    pWMVDec->m_iWidthPrevUVXExpPlusExp = pWMVDec->m_iWidthPrevUV * pWMVDec->EXPANDUV_REFVOP + pWMVDec->EXPANDUV_REFVOP;

    // for motion estimation
    pWMVDec->m_iMBSizeXWidthPrevY = pWMVDec->m_iWidthPrevY << 4; // * MB_SIZE
    pWMVDec->m_iBlkSizeXWidthPrevUV = pWMVDec->m_iWidthPrevUV << 3; // * BLOCK_SIZE



    // guard against small height images when multi-proc encoding is on.
    if (pWMVDec->m_uiNumProcessors == 4) {
        if (pWMVDec->m_uintNumMBY < 4) {
            pWMVDec->m_uiNumProcessors = (pWMVDec->m_uintNumMBY >= 2) ? 2 : 1;
        }
    } else if (pWMVDec->m_uiNumProcessors == 2) {
        if (pWMVDec->m_uintNumMBY == 1) {
            pWMVDec->m_uiNumProcessors = 1;
        }
    }

    //Zone_VLC
    pWMVDec->m_rgiEndIndexOfCurrZone [0] = END1STZONE;
    pWMVDec->m_rgiEndIndexOfCurrZone [1] = END2NDZONE;
    pWMVDec->m_rgiEndIndexOfCurrZone [2] = 64;
    pWMVDec->m_bInterZoneVLCTable = FALSE;

#ifndef WMV9_SIMPLE_ONLY
    SetupMultiResParams(pWMVDec);
#endif

}
#endif

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

//****************************************************************************************
//Loopfilter etc
//****************************************************************************************

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

#ifdef XDM

    pFramePlane->m_pucYPlane_Unaligned = ppxlTmp = (U8_WMV *)pWMVDec->m_MemXDM.pFrameBuff;
	pFramePlane->m_pCurrPlaneBase      = ppxlTmp;
    pWMVDec->m_MemXDM.pFrameBuff      += sizeof(U8_WMV)*(iYSize + iUSize + iVSize);
	if (ppxlTmp == NULL_WMV)
		return WMV_BadMemory;

	//Y
	CHECK_MALLOC2(ppxlTmp, iYSize + iUSize + iVSize);
	pFramePlane->m_pucYPlane  = (U8_WMV *)(((U32_WMV)ppxlTmp + iPad + 31) & ~31);

	//U
	ppxlTmp  = pFramePlane->m_pucYPlane_Unaligned + iYSize;
	ppxlTmp += iPad>>1;
	uv_offset = ((U32_WMV)ppxlTmp) & 31 ;
	if(uv_offset != 0)
		uv_offset = 32 - uv_offset;
#	    if (( EXPANDUV_REFVOP == 4 ) && defined (_MIPS64)) || defined (_SH4_)
	   if(uv_offset!=0)
		   uv_offset -= 4;
	   else
		   uv_offset = 28;
#	    endif
	pFramePlane->m_pucUPlane  = (U8_WMV *)((U32_WMV)ppxlTmp + uv_offset);

	//V
	ppxlTmp  = pFramePlane->m_pucYPlane_Unaligned + iYSize + iUSize;
	ppxlTmp += iPad>>1;
	uv_offset = ((U32_WMV)ppxlTmp) & 31 ;
	if(uv_offset != 0)
		uv_offset = 32 - uv_offset;
#	    if (( EXPANDUV_REFVOP == 4 ) && defined (_MIPS64)) || defined (_SH4_) 
	   if(uv_offset!=0)
		   uv_offset -= 4;
	   else
		   uv_offset = 28;
#	    endif
	pFramePlane->m_pucVPlane  = (U8_WMV *)((U32_WMV)ppxlTmp + uv_offset);

//	printf("\n\n----------->Y: %x; U:%x; V:%x\n",(U32_WMV)(pFramePlane->m_pucYPlane),(U32_WMV)(pFramePlane->m_pucUPlane),(U32_WMV)(pFramePlane->m_pucVPlane));

    return WMV_Succeeded;

#else

	if( pWMVDec->m_MemOperator.Type == NORMAL_MEM_OP || pWMVDec->m_MemOperator.Type == NONE_MEM_OP) 
	{
		pFramePlane->m_pucYPlane_Unaligned = ppxlTmp = (U8_WMV *) wmvMalloc ( pWMVDec, iYSize + iUSize + iVSize, DHEAP_FRAMES);
		pFramePlane->m_pCurrPlaneBase = ppxlTmp;
		if (ppxlTmp == NULL_WMV)
			return WMV_BadMemory;

		//Y
		CHECK_MALLOC2(ppxlTmp, iYSize + iUSize + iVSize);
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
#endif	//XDM

    return WMV_Succeeded;
}

#ifdef _WMMX_
tWMVDecodeStatus  Plane_init_MEM (tWMVDecInternalMember *pWMVDec, tYUV420Frame_WMV * pFramePlane, I32_WMV iSizeYplane, I32_WMV iSizeUVplane, CodecVersion vCodecVersion)
{
    U8_WMV *ppxlTmp;
    I32_WMV uv_offset;
    
    memset(pFramePlane, 0, sizeof (tYUV420Frame_WMV) ); // protect low memory issues when first fails and second tries to be freed.

    //pFramePlane->m_pucYPlane_Unaligned = ppxlTmp = (U8_WMV *) wmvMalloc (pWMVDec, (iSizeYplane + 32) * sizeof (U8_WMV));
    pFramePlane->m_pucYPlane_Unaligned = ppxlTmp = pWMVDec->m_ppxlCurrPHYSADDR;
    pWMVDec->m_ppxlCurrPHYSADDR += (iSizeYplane + 32) * sizeof (U8_WMV);

    if (ppxlTmp == NULL_WMV)
        return WMV_BadMemory;

    CHECK_MALLOC2(pFramePlane->m_pucYPlane_Unaligned,(iSizeYplane + 32)*sizeof(U8_WMV));

#if defined(macintosh) || defined(_ARM_) 
    if (vCodecVersion == WMV3 && !g_bSupportAltiVec_WMV && !g_bSupportWMMX_WMV)
    {
        uv_offset = ((U32_WMV)ppxlTmp) & 15 ;
        if(uv_offset != 0)
            uv_offset = 16 - uv_offset;
        uv_offset += 8;
        pFramePlane->m_pucYPlane  = (U8_WMV *)((U32_WMV)ppxlTmp + uv_offset);
    }
    else
        pFramePlane->m_pucYPlane  = (U8_WMV *)(((DWORD_PTR)ppxlTmp + 31) & ~31);
#else
    pFramePlane->m_pucYPlane  = (U8_WMV *)(((DWORD_PTR)ppxlTmp + 31) & ~31);
#endif

    //pFramePlane->m_pucUPlane_Unaligned = ppxlTmp = (U8_WMV *) malloc ((iSizeUVplane + 32) * sizeof (U8_WMV));
    pFramePlane->m_pucUPlane_Unaligned = ppxlTmp = pWMVDec->m_ppxlCurrPHYSADDR;
    pWMVDec->m_ppxlCurrPHYSADDR += (iSizeUVplane + 32) * sizeof (U8_WMV);

    if (ppxlTmp == NULL_WMV)
        return WMV_BadMemory;

    CHECK_MALLOC2(pFramePlane->m_pucUPlane_Unaligned,(iSizeUVplane + 32) * sizeof (U8_WMV));

#if defined(macintosh) || defined(_ARM_)
    if (vCodecVersion == WMV3 && !g_bSupportAltiVec_WMV && !g_bSupportWMMX_WMV)
    {
        uv_offset = ((U32_WMV)ppxlTmp) & 15 ;
        if(uv_offset != 0)
            uv_offset = 16 - uv_offset;
        uv_offset += 4;
    }
    else
    {
        uv_offset = ((U32_WMV)ppxlTmp) & 31 ;
        if(uv_offset != 0)
            uv_offset = 32 - uv_offset;
    }
#else
    uv_offset = ((U32_WMV)ppxlTmp) & 31 ;
    if(uv_offset != 0)
        uv_offset = 32 - uv_offset;
#endif

#if (( EXPANDUV_REFVOP == 4 ) && defined (_MIPS64)) || defined (_SH4_)
    if(uv_offset!=0)
        uv_offset -= 4;
    else
        uv_offset = 28;
#endif

    pFramePlane->m_pucUPlane  = (U8_WMV *)((U32_WMV)ppxlTmp + uv_offset);

    //pFramePlane->m_pucVPlane_Unaligned = ppxlTmp = (U8_WMV *) wmvMalloc (pWMVDec, (iSizeUVplane + 32) * sizeof (U8_WMV));
    pFramePlane->m_pucVPlane_Unaligned = ppxlTmp = pWMVDec->m_ppxlCurrPHYSADDR;
    pWMVDec->m_ppxlCurrPHYSADDR += (iSizeUVplane + 32) * sizeof (U8_WMV);

    if (ppxlTmp == NULL_WMV)
        return WMV_BadMemory;

    CHECK_MALLOC2(pFramePlane->m_pucVPlane_Unaligned,(iSizeUVplane + 32) * sizeof (U8_WMV));

#if defined(macintosh) || defined(_ARM_)
    if (vCodecVersion == WMV3 && !g_bSupportAltiVec_WMV && !g_bSupportWMMX_WMV)
    {
        uv_offset = ((U32_WMV)ppxlTmp) & 15 ;
        if(uv_offset != 0)
            uv_offset = 16 - uv_offset;
        uv_offset += 4;
    }
    else
    {
        uv_offset = ((U32_WMV)ppxlTmp) & 31 ;
        if(uv_offset != 0)
            uv_offset = 32 - uv_offset;
    }
#else
    uv_offset = ((U32_WMV)ppxlTmp) & 31 ;
    if(uv_offset != 0)
        uv_offset = 32 - uv_offset;
#endif

#if (( EXPANDUV_REFVOP == 4 ) && defined (_MIPS64)) || defined (_SH4_) 
    if(uv_offset!=0)
        uv_offset -= 4;
    else
        uv_offset = 28;
#endif

    pFramePlane->m_pucVPlane  = (U8_WMV *)((U32_WMV)ppxlTmp + uv_offset);

    return WMV_Succeeded;
}
#endif

Void_WMV   m_InitIDCT_Dec(tWMVDecInternalMember *pWMVDec, Bool_WMV fUseTransposedMatrix)
{
    pWMVDec->m_pIntraIDCT_Dec = g_IDCTDec_WMV2_Intra;

    pWMVDec->m_pInterIDCT_Dec = g_IDCTDec_WMV2_Inter;
    pWMVDec->m_pIntraX9IDCT_Dec = g_IDCTDec16_WMV2;
    pWMVDec->m_pInter8x4IDCT_Dec = g_8x4IDCTDec_WMV2;
    pWMVDec->m_pInter4x8IDCT_Dec = g_4x8IDCTDec_WMV2;
    pWMVDec->m_pInter4x4IDCT_Dec = g_4x4IDCTDec_WMV2;   

    if (pWMVDec->m_bYUV411) {
        pWMVDec->m_pInterIDCT_Dec = g_IDCTDec16_WMV2;
        pWMVDec->m_pInter8x4IDCT_Dec = g_8x4IDCTDec16_WMV2;
        pWMVDec->m_pInter4x8IDCT_Dec = g_4x8IDCTDec16_WMV2;
    }

#if defined( _WMV_TARGET_X86_) || defined(_Embedded_x86)
    if (g_bSupportMMX_WMV){

        pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInvRotated;
        
        if (!pWMVDec->m_b16bitXform) {
            pWMVDec->m_pInter8x4IDCT_Dec = g_8x4IDCTDec16MMX_WMV2;
            pWMVDec->m_pInter4x8IDCT_Dec = g_4x8IDCTDec16MMX_WMV2;
        }

        pWMVDec->m_pInter4x4IDCT_Dec = g_4x4IDCTDec16MMX_WMV2;
        pWMVDec->m_pInterIDCT_Dec = g_IDCTDecMMX_WMV2_Inter;
        pWMVDec->m_pIntraX9IDCT_Dec = g_IDCTDecMMX_WMV2_Inter;
        if (fUseTransposedMatrix) {
            pWMVDec->m_pIntraIDCT_Dec = g_IDCTDecMMX_WMV2;
            pWMVDec->m_bRotatedIdct = TRUE;
        }
    }
#endif

    if (pWMVDec->m_b16bitXform) {
        pWMVDec->m_bRotatedIdct = TRUE;
        pWMVDec->m_pIntraIDCT_Dec = g_IDCTDec_WMV3_Fun;
        pWMVDec->m_pInterIDCT_Dec = g_IDCTDec16_WMV3;
        pWMVDec->m_pInter8x4IDCT_Dec = g_8x4IDCTDec_WMV3;
        pWMVDec->m_pInter4x8IDCT_Dec = g_4x8IDCTDec_WMV3;
        pWMVDec->m_pInter4x4IDCT_Dec = g_4x4IDCTDec_WMV3;
        pWMVDec->m_pIntraX9IDCT_Dec = g_IDCTDec16_WMV3;
#if defined( _WMV_TARGET_X86_) || defined(_Embedded_x86)
        if (g_bSupportMMX_WMV){
            pWMVDec->m_pIntraIDCT_Dec    = g_IntraIDCTDecMMX_WMV3;
            pWMVDec->m_pIntraX9IDCT_Dec  = g_InterIDCTDecMMX_WMV3;
            pWMVDec->m_pInterIDCT_Dec    = g_InterIDCTDecMMX_WMV3;
            pWMVDec->m_pInter8x4IDCT_Dec = g_8x4IDCTDecMMX_WMV3;
            pWMVDec->m_pInter4x8IDCT_Dec = g_4x8IDCTDecMMX_WMV3;
            pWMVDec->m_pInter4x4IDCT_Dec = g_4x4IDCTDecMMX_WMV3;
        }
#endif        
    }

    // Set members used in AC prediction
    if (pWMVDec->m_bRotatedIdct)
    {
        pWMVDec->m_iACColPredOffset = 0;
        pWMVDec->m_iACRowPredOffset = 8;
        pWMVDec->m_iACColPredShift = 0;
        pWMVDec->m_iACRowPredShift = 3;
    }
    else
    {
        pWMVDec->m_iACColPredOffset = 8;
        pWMVDec->m_iACRowPredOffset = 0;
        pWMVDec->m_iACColPredShift = 3;
        pWMVDec->m_iACRowPredShift = 0;
    }
}

Void_WMV decideMMXRoutines (tWMVDecInternalMember *pWMVDec)
{

    m_InitIDCT_Dec(pWMVDec,TRUE); //codes in this function can be merged into decodeMMXRoutines. m_InitIDCT_Dec can then be removed.
    
    pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInv;
    pWMVDec->m_p8x4ZigzagScanOrder = pWMVDec->m_p8x4ZigzagInv;
    pWMVDec->m_p4x8ZigzagScanOrder = pWMVDec->m_p4x8ZigzagInv;
    pWMVDec->m_p4x4ZigzagScanOrder = pWMVDec->m_p4x4ZigzagInv;

    if (pWMVDec->m_b16bitXform)
    {
        pWMVDec->m_bRotatedIdct = 1;
        pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInvRotated;
        pWMVDec->m_p8x4ZigzagScanOrder = pWMVDec->m_p8x4ZigzagInvRotated;
        pWMVDec->m_p4x8ZigzagScanOrder = pWMVDec->m_p4x8ZigzagInvRotated;  
        pWMVDec->m_p4x4ZigzagScanOrder = pWMVDec->m_p4x4ZigzagInvRotated;  
    } 

#ifdef _EMB_SSIMD_IDCT_

     if (pWMVDec->m_cvCodecVersion == WMV3) 
     {
        EMB_InitZigZagTable_SSIMD(pWMVDec->m_p8x4ZigzagScanOrder, pWMVDec->m_p8x4ZigzagSSIMD, 32);
        EMB_InitZigZagTable_SSIMD(pWMVDec->m_p4x4ZigzagScanOrder, pWMVDec->m_p4x4ZigzagSSIMD, 16);
     }

#endif
    
     // g_InitRepeatRefInfo
   // g_pRepeatRef0Y = g_RepeatRef0Y_32; 
   // g_pRepeatRef0UV = g_RepeatRef0UV_16; 

//#ifdef _WMV9AP_
     //if (pWMVDec->m_cvCodecVersion == WMVA)
     {
        g_pRepeatRef0Y = g_RepeatRef0Y; 
       // g_pRepeatRef0Y = g_RepeatRef0Y_32; 
        g_pRepeatRef0UV = g_RepeatRef0UV; 
#ifndef WMV9_SIMPLE_ONLY
            g_InitRepeatRefInfo (pWMVDec, pWMVDec->m_bAdvancedProfile);
#endif

            // Set members used in AC prediction
        if (pWMVDec->m_bRotatedIdct)
        {
            pWMVDec->m_iACColPredOffset = 0;
            pWMVDec->m_iACRowPredOffset = 8;
            pWMVDec->m_iACColPredShift = 0;
            pWMVDec->m_iACRowPredShift = 3;
        }
        else
        {
            pWMVDec->m_iACColPredOffset = 8;
            pWMVDec->m_iACRowPredOffset = 0;
            pWMVDec->m_iACColPredShift = 3;
            pWMVDec->m_iACRowPredShift = 0;
        }

     }
//#endif

    // MC
    pWMVDec->m_pMotionCompZero = g_MotionCompZeroMotion_WMV_Fun;
    pWMVDec->m_pcClapTable = g_InitDecGlobalVars();

#ifndef _EMB_WMV2_
    pWMVDec->m_pMotionCompAndAddErrorRndCtrlOn = g_MotionCompAndAddErrorRndCtrl;
    pWMVDec->m_pMotionCompRndCtrlOn = g_MotionCompRndCtrl;
    pWMVDec->m_pMotionCompAndAddErrorRndCtrlOff = g_MotionCompAndAddError;
    pWMVDec->m_pMotionCompRndCtrlOff = g_MotionComp;



    pWMVDec->m_pMotionCompMixed = MotionCompMixed;
    pWMVDec->m_pMotionCompMixedAndAddError = MotionCompMixedAndAddError;
#endif //_EMB_WMV2_

#ifndef WMV9_SIMPLE_ONLY
#   ifndef WMV789_ONLY
    if (pWMVDec->m_bX9) {
        pWMVDec->m_pFilterHorizontalEdge = &g_FilterHorizontalEdgeV9;
        pWMVDec->m_pFilterVerticalEdge = &g_FilterVerticalEdgeV9;
    }
    else {

        // Loop Filter Selection
        pWMVDec->m_pFilterHorizontalEdge = g_FilterHorizontalEdge_WMV;
        pWMVDec->m_pFilterVerticalEdge = g_FilterVerticalEdge_WMV;
    }
#   endif
    pWMVDec->m_pDeblockInterlaceFrame = DeblockInterlaceFrame;
#endif // !(defined(WMV9_SIMPLE_ONLY)

    pWMVDec->m_pBlkAvgX8_MMX = BlkAvgX8;

}



Void_WMV m_InitFncPtrAndZigzag(tWMVDecInternalMember *pWMVDec)
{
    pWMVDec->m_bRefreshDisplay_AllMB_Enable = TRUE_WMV;
    // At the decoder side, Both Intra and Inter Frame might use Rotated matrix depending on which idct is called.
    if (pWMVDec->m_cvCodecVersion == WMV2 || pWMVDec->m_cvCodecVersion >= WMV3) {
        pWMVDec->m_p8x4ZigzagInv = grgi8x4ZigzagInv;
        pWMVDec->m_p4x8ZigzagInv = grgi4x8ZigzagInv;
        pWMVDec->m_p8x4ZigzagInvRotated = grgi8x4ZigzagInvRotated;
        pWMVDec->m_p4x8ZigzagInvRotated = grgi4x8ZigzagInvRotated;
        pWMVDec->m_p4x4ZigzagInv = grgi4x4ZigzagInv;
        pWMVDec->m_p4x4ZigzagInvRotated = grgi4x4ZigzagInvRotated;

//#ifdef _WMV9AP_
        if (pWMVDec->m_cvCodecVersion == WMVA)
        pWMVDec->m_p4x4ZigzagScanOrder = pWMVDec->m_p4x4ZigzagInvRotated; //All malloc(sizeof( and old 4x4 IDCT's now use this zigzag scan order
//#endif

#if defined( _WMV_TARGET_X86_) || defined(_Embedded_x86)
        if(pWMVDec->m_cvCodecVersion == WMV3) {
            if (g_bSupportMMX_WMV) {
                if (!pWMVDec->m_b16bitXform) {
                    pWMVDec->m_p8x4ZigzagInv = grgi8x4ZigzagInvRotated8x8;
                    pWMVDec->m_p4x8ZigzagInv = grgi4x8ZigzagInvRotated8x8;
                    pWMVDec->m_p4x4ZigzagInv = grgi4x4ZigzagInvRotated;
                }
            }
        }
        if(pWMVDec->m_cvCodecVersion == WMV2) {
            if (g_bSupportMMX_WMV) {
                pWMVDec->m_p8x4ZigzagInv = grgi8x4ZigzagInvRotated8x8;
                pWMVDec->m_p4x8ZigzagInv = grgi4x8ZigzagInvRotated8x8;
            }
        }
#endif

        if(pWMVDec->m_cvCodecVersion == WMV2)
        {
            pWMVDec->m_bSKIPBIT_CODING_ = TRUE_WMV;
            pWMVDec->m_bNEW_PCBPCY_TABLE = TRUE_WMV;
        }
        else
        {
            pWMVDec->m_bSKIPBIT_CODING_ = FALSE_WMV;
            pWMVDec->m_bNEW_PCBPCY_TABLE = FALSE_WMV;
        }
    }

    if (pWMVDec->m_cvCodecVersion >= WMV1) {
        pWMVDec->m_pZigzagInv_I = grgiZigzagInv_NEW_I;
        pWMVDec->m_pZigzagInvRotated_I = grgiZigzagInvRotated_NEW_I;
        pWMVDec->m_pHorizontalZigzagInv = grgiHorizontalZigzagInv_NEW;
        pWMVDec->m_pVerticalZigzagInv = grgiVerticalZigzagInv_NEW;
        //Decoder Only
        pWMVDec->m_pHorizontalZigzagInvRotated = grgiHorizontalZigzagInvRotated_NEW;
        pWMVDec->m_pVerticalZigzagInvRotated = grgiVerticalZigzagInvRotated_NEW;

        pWMVDec->m_pZigzagInvRotated = grgiZigzagInvRotated_NEW;
        pWMVDec->m_pZigzagInv = grgiZigzagInv_NEW;

        pWMVDec->m_pDecodeIMBAcPred = DecodeIMBAcPred;    
        setRefreshPeriod(pWMVDec);
    }
#ifndef WMV789_ONLY
    else{
        pWMVDec->m_pZigzagInv_I = grgiZigzagInv;
        pWMVDec->m_pZigzagInvRotated_I = grgiZigzagInvRotated;
        pWMVDec->m_pHorizontalZigzagInv = grgiHorizontalZigzagInv;
        pWMVDec->m_pVerticalZigzagInv = grgiVerticalZigzagInv;
        //Decoder Only
        pWMVDec->m_pHorizontalZigzagInvRotated = grgiVerticalZigzagInv;
        pWMVDec->m_pVerticalZigzagInvRotated = grgiHorizontalZigzagInv;

        pWMVDec->m_pZigzagInvRotated = grgiZigzagInvRotated;
        pWMVDec->m_pZigzagInv = grgiZigzagInv;

        pWMVDec->m_pDecodeIMBAcPred = DecodeIMBAcPred_MP4X;
    }
#endif

//#ifdef _WMV9AP_
    if (pWMVDec->m_cvCodecVersion == WMVA)
    {
        pWMVDec->m_p8x4ZigzagScanOrder = pWMVDec->m_p8x4ZigzagInv;
        pWMVDec->m_p4x8ZigzagScanOrder = pWMVDec->m_p4x8ZigzagInv;
        pWMVDec->m_p4x4ZigzagScanOrder = pWMVDec->m_p4x4ZigzagInv;
        pWMVDec->m_bRotatedIdct = FALSE_WMV;
        pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInv;

        pWMVDec->m_pHorizontalZigzagScanOrder = pWMVDec->m_pHorizontalZigzagInv;
        pWMVDec->m_pVerticalZigzagScanOrder = pWMVDec->m_pVerticalZigzagInv;


        if (pWMVDec->m_b16bitXform)
        {
            pWMVDec->m_bRotatedIdct = 1;
            pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInvRotated;
            pWMVDec->m_p8x4ZigzagScanOrder = pWMVDec->m_p8x4ZigzagInvRotated;
            pWMVDec->m_p4x8ZigzagScanOrder = pWMVDec->m_p4x8ZigzagInvRotated;  
            pWMVDec->m_p4x4ZigzagScanOrder = pWMVDec->m_p4x4ZigzagInvRotated;  

            pWMVDec->m_pHorizontalZigzagScanOrder = pWMVDec->m_pHorizontalZigzagInvRotated;
            pWMVDec->m_pVerticalZigzagScanOrder = pWMVDec->m_pVerticalZigzagInvRotated;


        } 
#ifdef _EMB_SSIMD_IDCT_
        if (pWMVDec->m_cvCodecVersion == WMVA) 
        {
            EMB_InitZigZagTable_SSIMD(pWMVDec->m_p8x4ZigzagScanOrder, pWMVDec->m_p8x4ZigzagSSIMD, 32);
            EMB_InitZigZagTable_SSIMD(pWMVDec->m_p4x4ZigzagScanOrder, pWMVDec->m_p4x4ZigzagSSIMD, 16);
        }
#endif
    }
//#endif


    // Func Pointers
    pWMVDec->m_pDecodeI = WMVideoDecDecodeI;
    pWMVDec->m_pDecodeP = WMVideoDecDecodeP;
#ifndef _EMB_WMV2_
    pWMVDec->m_pDecodePMB=DecodePMB;
#else //_EMB_WMV2_
    pWMVDec->m_pDecodePMB=DecodePMB_EMB;
    InitEMB_FuncTable(pWMVDec);
#endif


    if (pWMVDec->m_cvCodecVersion >= WMV3)
    {
        pWMVDec->m_pDecodeP = WMVideoDecDecodeP_X9;
        pWMVDec->m_pDecodeB = decodeB_Deblock;
    }

    if (pWMVDec->m_cvCodecVersion != MP4S) {
        pWMVDec->m_iEscRunDiffV2V3 = (pWMVDec->m_cvCodecVersion >= MP43)? 1 : 0;
        pWMVDec->m_iDCPredCorrect = (pWMVDec->m_cvCodecVersion >= WMV1)? 1 : 0;

        if (pWMVDec->m_pInterIDCT_Dec == g_IDCTDec_WMV2_Inter)
            pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantizeEscCode;
        else
            pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantize16;
#ifdef  _WMV_TARGET_X86_
            if (g_bSupportMMX_WMV && pWMVDec->m_cvCodecVersion != WMV3)
            pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantizeEscCode_MMX;
#endif //  _WMV_TARGET_X86_


        if (pWMVDec->m_cvCodecVersion >= MP43) {
            pWMVDec->m_pDecodeMBOverheadOfIVOP = decodeMBOverheadOfIVOP_MSV;
            pWMVDec->m_pDecodeMBOverheadOfPVOP = decodeMBOverheadOfPVOP_MSV; 
        }
#ifndef WMV789_ONLY
        else{ // MP42
            pWMVDec->m_pDecodeMBOverheadOfIVOP = decodeMBOverheadOfIVOP_New;
            pWMVDec->m_pDecodeMBOverheadOfPVOP = decodeMBOverheadOfPVOP_New; 
        }
#endif
    }
#ifndef WMV789_ONLY
    else{
        pWMVDec->m_iDCPredCorrect = 1;
        pWMVDec->m_uintNumMBYSlice = pWMVDec->m_uintNumMBY;
        pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantizeEscCode_MPEG4;
#ifdef  _WMV_TARGET_X86_
        if (g_bSupportMMX_WMV) 
            pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantizeEscCode_MMX_MPEG4;
#endif //  _WMV_TARGET_X86_
        pWMVDec->m_pDecodeMBOverheadOfIVOP = decodeMBOverheadOfIVOP_MPEG4;
        pWMVDec->m_pDecodeMBOverheadOfPVOP = decodeMBOverheadOfPVOP_MPEG4; 
    }
#endif

}

#if defined(WANT_HEAP_MEASURE)
extern  int g_cbMaxHeapSize[7]={0}, g_cbHeapSize[7]={0}, g_iHeapLastClass = 0, g_HeapMeasureInitialize = 0 ;
#endif

//*******************************************************************************************************
//  Implement wmv memallo and free and heap stack tracing functions.            
//********************************************************************************************************

#define WMV_TRACE_HEAP          0x01
#define WMV_TRACE_STACK         0x02
#define WMV_TRACE_HEAP_DIST     0x04

/*
 * Turn on tracing in compile-time, switch the following macros
 * Testing can turn on tracing on run-time by calling WMVSetTraceFlag functions.
 */

#if 0
static unsigned int g_wmvTraceFlags=WMV_TRACE_HEAP | WMV_TRACE_STACK;
#else
static unsigned int g_wmvTraceFlags=0;
#endif

void WMVSetTraceFlag (unsigned int flag)
{
    g_wmvTraceFlags=flag;
}


//#ifndef WANT_HEAP_MEASURE

void *vowmvMalloc(tWMVDecInternalMember *pWMVDec, unsigned int size, int category)
{
	if(pWMVDec->m_memOp) {
		VO_MEM_INFO voMemInfo; 
		voMemInfo.Size = size;
		pWMVDec->m_memOp->Alloc(VO_INDEX_DEC_WMV, &voMemInfo); 
		return (void *)voMemInfo.VBuffer; 
	}
	else {
		return (void *)malloc(size);
	}
}

void   vowmvFree(tWMVDecInternalMember *pWMVDec, void *free_ptr)
{
	if(pWMVDec->m_memOp) {
		pWMVDec->m_memOp->Free(VO_INDEX_DEC_WMV, free_ptr); 
	}
	else {
	    free(free_ptr);
	}
}

//#else

/**************************************************************
 *     Trace malloc functions                     *
 **************************************************************/

//
// This must keep in the order from 0 - maximum category that supports. Here we use 0 - 4
//

typedef struct _RAMFILEUNIT //Record the usage of allocation by filename and line_no
{
    char filename[256];
    int  ramCat;        //CATEGORY MEMORY SIZE TRACE: HUFFMAN, LOCAL COUNT
    int  line_no;
    int  ramBytes;          //total ram allocated
    int  dynrambytes;       //Removed from HeapFree, Free
}RAMFILEUNIT;

typedef struct _RAM_TABLE
{
    void   *ptr;
    size_t  size;
    RAMFILEUNIT *ramUnit;
}RAM_TABLE;                 //Registered Entry for every allocated memory.

typedef struct
{
    int totalHeapSize;      //total alloc memory size
    int dynHeapSize;    //dyn alloc memory size
    int dynMax;         //maxium dyn malloc memory size
    int totalFreeSize;      //total freed size
    int oldPeakSize;

    int catSize[TOTAL_HEAP_CATEGORY];       //total allocated category size
    int catDynSize[TOTAL_HEAP_CATEGORY];        //total allocated category size
    int catPeakSize[TOTAL_HEAP_CATEGORY];   //peak allocated category size

    RAM_TABLE *     ramTable;
    RAMFILEUNIT *   heapFileAllocArray;
    int             heapFileAllocCounts;

    const char *memReportFile;

#ifdef MULTI_THREAD_APP
    CRITICAL_SECTION    syncSection;
#endif

}MEMTRACECONTEXT;

#define MAX_FILES_ALLOC 400  //maximum file allocations for alloc mem.

WMV_EXTERN_C RAM_TABLE*  prvRegRamTable(void *new_ptr, size_t n);
WMV_EXTERN_C void prvUnregRamTable(void *new_ptr);
WMV_EXTERN_C void prvRegAllocRamByFile(RAMFILEUNIT *pfileAllocArray, int *pfile_alloc_counts, int pBytes, char *pFile, int line_no, RAM_TABLE *pRamTable, int pRamCategory);
WMV_EXTERN_C void RegFreeRamByFile(RAMFILEUNIT *pfileAllocArray, int pfile_alloc_counts, void *pMem);


#ifndef PEEK_TRACE_INIT_LEVEL
#define PEEK_TRACE_INIT_LEVEL   10000000 //10000K
#endif

#ifndef PEEK_TRACE_STEP
#define PEEK_TRACE_STEP         2000000  //200K
#endif

#ifndef MAX_RAMS
#define MAX_RAMS    5000
#endif 

MEMTRACECONTEXT *gMemtraceContext = 0;

WMV_EXTERN_C MEMTRACECONTEXT *prvCreateMemTraceContext()
{
    int i;
    MEMTRACECONTEXT *context = (MEMTRACECONTEXT *) malloc(sizeof(MEMTRACECONTEXT));

    context->totalHeapSize=0;
    context->dynHeapSize=0; //dyn alloc memory size
    context->dynMax = 0;            //maxium dyn malloc memory size
    context->totalFreeSize = 0;     //total freed memory

    for (i=0; i<TOTAL_HEAP_CATEGORY; i++)
    {
        context->catSize[i] = 0;
        context->catPeakSize[i] = 0;
        context->catDynSize[i]=0;
    }

    context->memReportFile = 0;

    context->heapFileAllocArray = (RAMFILEUNIT *)malloc(sizeof(RAMFILEUNIT) * MAX_FILES_ALLOC);
    memset(context->heapFileAllocArray, 0, sizeof(RAMFILEUNIT) * MAX_FILES_ALLOC);
    context->heapFileAllocCounts=0;
    context->oldPeakSize = 0;
    context->ramTable = (RAM_TABLE *)malloc(sizeof(RAM_TABLE) * MAX_RAMS);
    memset(context->ramTable, 0, sizeof(RAM_TABLE) * MAX_RAMS);

#ifdef MULTI_THREAD_APP
    context->syncSection = InitializeCriticalSection(&syncSection);
#endif
    return context;
}


WMV_EXTERN_C RAM_TABLE*  prvRegRamTable(void *new_ptr, size_t n)
{
    int i;

    if (!new_ptr || !gMemtraceContext)  return 0;

    gMemtraceContext->totalHeapSize += n;
    gMemtraceContext->dynHeapSize += n;
    gMemtraceContext->dynMax= (gMemtraceContext->dynHeapSize > gMemtraceContext->dynMax) ? gMemtraceContext->dynHeapSize : gMemtraceContext->dynMax;

    if (g_wmvTraceFlags & WMV_TRACE_HEAP_DIST)
    {
        if ( ( abs(gMemtraceContext->dynMax - gMemtraceContext->oldPeakSize) > PEEK_TRACE_STEP ) && ( gMemtraceContext->dynMax > PEEK_TRACE_INIT_LEVEL ) )
        {   
            FILE *fd;

            gMemtraceContext->oldPeakSize = gMemtraceContext->dynMax;

            fd = fopen(gMemtraceContext->memReportFile, "at");
            if (!fd)    return 0;

            fprintf(fd, "\n*********************\n");
            fprintf(fd, "Peak Ram Used: %d\n Peak Ram Distributions:\n", gMemtraceContext->dynMax);
            for (i=0; i<gMemtraceContext->heapFileAllocCounts; i++)
            {
                if (gMemtraceContext->heapFileAllocArray[i].dynrambytes > 0)
                {
                    fprintf(fd, "%d bytes(%f percent) in %s line %d.\n", 
                        gMemtraceContext->heapFileAllocArray[i].dynrambytes, 
                        (float)gMemtraceContext->heapFileAllocArray[i].dynrambytes / gMemtraceContext->dynMax, 
                        gMemtraceContext->heapFileAllocArray[i].filename, 
                        gMemtraceContext->heapFileAllocArray[i].line_no);
                }
            }
            fclose(fd);
        }
    }

    for (i=0; i<MAX_RAMS; i++)
    {
        if (gMemtraceContext->ramTable[i].ptr==0)
        {
            gMemtraceContext->ramTable[i].ramUnit=0;
            gMemtraceContext->ramTable[i].ptr=new_ptr;
            gMemtraceContext->ramTable[i].size=n;
            return &gMemtraceContext->ramTable[i];
        }
    }
    return 0;
}

WMV_EXTERN_C void prvUnregRamTable(void *new_ptr)
{
    int i;

    if (!new_ptr || !gMemtraceContext)  return;

    for (i=0; i<MAX_RAMS; i++)
    {
        if (gMemtraceContext->ramTable[i].ptr==new_ptr)
        {
            gMemtraceContext->dynHeapSize -= gMemtraceContext->ramTable[i].size;
            gMemtraceContext->catDynSize[gMemtraceContext->ramTable[i].ramUnit->ramCat] -= gMemtraceContext->ramTable[i].size;

            gMemtraceContext->ramTable[i].ptr=0;
            gMemtraceContext->ramTable[i].ramUnit=0;
            gMemtraceContext->totalFreeSize += gMemtraceContext->ramTable[i].size;
            break;
        }
    }
}

WMV_EXTERN_C void prvRegAllocRamByFile(RAMFILEUNIT *pfileAllocArray, int *pfile_alloc_counts, int pBytes, char *pFile, int line_no, RAM_TABLE *pRamTable, int pRamCategory)
{
    int i;

    gMemtraceContext->catSize[pRamCategory] += pBytes;
    gMemtraceContext->catDynSize[pRamCategory] += pBytes;

    if (gMemtraceContext->catPeakSize[pRamCategory] < gMemtraceContext->catDynSize[pRamCategory])
        gMemtraceContext->catPeakSize[pRamCategory] = gMemtraceContext->catDynSize[pRamCategory];
    
    for (i=0; i<*pfile_alloc_counts; i++)
    {
        if (!strcmp(pfileAllocArray[i].filename, pFile) && pfileAllocArray[i].line_no==line_no)
        {
            pfileAllocArray[i].ramBytes+=pBytes;
            pfileAllocArray[i].dynrambytes+=pBytes;
            if (pRamTable){
                pRamTable->ramUnit = &pfileAllocArray[i];
                pRamTable = 0;
            }

            return;
        }
    }

    if (i<MAX_FILES_ALLOC)
    {
        strcpy(pfileAllocArray[i].filename, pFile);
        pfileAllocArray[i].line_no = line_no;
        pfileAllocArray[i].ramBytes = pBytes;
        pfileAllocArray[i].dynrambytes=pBytes;
        pfileAllocArray[i].ramCat=pRamCategory;

        if (pRamTable)
        {
            pRamTable->ramUnit = &pfileAllocArray[i];
            pRamTable = 0;
        }

        *pfile_alloc_counts = i+1;
    }
    else
        assert(0);
}

WMV_EXTERN_C void RegFreeRamByFile(RAMFILEUNIT *pfileAllocArray, int pfile_alloc_counts, void *pMem)
{
    int i;

    if (!pMem || !gMemtraceContext) return;

    for (i=0; i<MAX_RAMS; i++)
    {
        if (gMemtraceContext->ramTable[i].ptr==pMem && gMemtraceContext->ramTable[i].ramUnit)
        {
            gMemtraceContext->ramTable[i].ramUnit->dynrambytes-=gMemtraceContext->ramTable[i].size;
            return;
        }
    }
}

/***********************************************************************
 *        Implementation of prvMalloc and prvFree                      *
 ***********************************************************************/

WMV_EXTERN_C void * prvMalloc(tWMVDecInternalMember *pWMVDec,size_t pSize, char *pFile, int pLine, int category)
{
    void *new_ptr;
    RAM_TABLE *new_entry;

#ifdef WMV_INCREMENTAL_MALLOC_FAILURE

/*
	WMV_INCREMENTAL_MALLOC_FAILURE forces prvMalloc to fail in a predictable way.
	This is useful in tracking memory allocation failures that can occur under stress
	conditions.  A known file is used (and created if one does not exist).  This file
	contains the number of calls to prvMalloc which will succeed before forcing a
	failure.  Once failure occurs the counter in the file is incremented and any
	further mallocs will fail.

	The codec should respond gracefully, so by repeatedly opening and decoding a file
	you can incrementally check that low memory failures are correctly handled.
*/

	FILE* pCounter;
	char buffer[256];
	int avail_mallocs=0;
	
	//File test here.
	pCounter = fopen("wmvMallocCount.txt", "rb");

	if( pCounter ) {
		avail_mallocs = atoi( fgets( buffer, 256, pCounter) );
		fclose(pCounter);
	} else {

		avail_mallocs = 0;
		pCounter = fopen("wmvMallocCount.txt", "w+b");
		
		sprintf(buffer, "%d", 0);

		if( !pCounter ) {
			exit(-2);
		}
		fputs(buffer, pCounter);
		fclose(pCounter);
	}

	if( (gNum_mallocs >= 0) && (gNum_mallocs < avail_mallocs) ) {
		gNum_mallocs++;    
	    new_ptr = malloc(pSize);
	} else if(gNum_mallocs < 0) {
		new_ptr = NULL;
	} else {
		
		pCounter = fopen("wmvMallocCount.txt", "w+b");
		
		sprintf(buffer, "%d", gNum_mallocs+1);
		if( !pCounter ) {
			exit(-3);
		}
		fputs(buffer, pCounter);
		fclose(pCounter);
		new_ptr = NULL;
		gNum_mallocs = -1;
	}
#endif


	if(pWMVDec->m_memOp) {
		VO_MEM_INFO voMemInfo; 
		voMemInfo.Size = pSize;
		pWMVDec->m_memOp->Alloc(VO_INDEX_DEC_WMV, &voMemInfo); 
		new_ptr = (U8_WMV *)voMemInfo.VBuffer; 
	}
	else {
		new_ptr = malloc(pSize);
	}

    if (g_wmvTraceFlags & WMV_TRACE_HEAP)
    {

    #ifdef MULTI_THREAD_APP
        if (gMemtraceContext)   EnterCriticalSection(&gMemtraceContext->syncSection);
    #endif

        new_entry = prvRegRamTable(new_ptr, pSize);

        if (gMemtraceContext)
        {
            prvRegAllocRamByFile(gMemtraceContext->heapFileAllocArray, &gMemtraceContext->heapFileAllocCounts, pSize, pFile, pLine, new_entry, category);
        }

    #ifdef MULTI_THREAD_APP
        if (gMemtraceContext)   LeaveCriticalSection(&gMemtraceContext->syncSection);
    #endif

    }

    return new_ptr;
}

WMV_EXTERN_C void prvFree(tWMVDecInternalMember *pWMVDec,void *lpMem, char *pFile, int pLine)
{
    if (g_wmvTraceFlags & WMV_TRACE_HEAP)
    {

    #ifdef MULTI_THREAD_APP
        if (gMemtraceContext)   EnterCriticalSection(&gMemtraceContext->syncSection);
    #endif

        if (gMemtraceContext)
        {
            RegFreeRamByFile(gMemtraceContext->heapFileAllocArray, gMemtraceContext->heapFileAllocCounts, lpMem);
            prvUnregRamTable(lpMem);
        }

    #ifdef MULTI_THREAD_APP
        if (gMemtraceContext)   LeaveCriticalSection(&gMemtraceContext->syncSection);
    #endif
    }

	if(lpMem == NULL )
		return;
 	if(pWMVDec->m_memOp) {
		VO_MEM_OPERATOR*pMemOp = pWMVDec->m_memOp;
		pMemOp->Free(VO_INDEX_DEC_WMV, lpMem); 
	}
	else {
		free(lpMem);
		lpMem = NULL;
	}
}

/*
 * Use for tracing Allocated Memory - no actual malloc is called.
 */
WMV_EXTERN_C void * prvRegAllocMem(void *new_ptr, size_t pSize, char *pFile, int pLine, int category)
{
    RAM_TABLE *new_entry;
    
    if (g_wmvTraceFlags & WMV_TRACE_HEAP)
    {

    #ifdef MULTI_THREAD_APP
        if (gMemtraceContext)   EnterCriticalSection(&gMemtraceContext->syncSection);
    #endif

        new_entry = prvRegRamTable(new_ptr, pSize);

        if (gMemtraceContext)
        {
            prvRegAllocRamByFile(gMemtraceContext->heapFileAllocArray, &gMemtraceContext->heapFileAllocCounts, pSize, pFile, pLine, new_entry, category);
        }

    #ifdef MULTI_THREAD_APP
        if (gMemtraceContext)   LeaveCriticalSection(&gMemtraceContext->syncSection);
    #endif

    }

    return new_ptr;
}

WMV_EXTERN_C void prvUnregAllocMem(void *lpMem, char *pFile, int pLine)
{
    if (g_wmvTraceFlags & WMV_TRACE_HEAP)
    {

    #ifdef MULTI_THREAD_APP
        if (gMemtraceContext)   EnterCriticalSection(&gMemtraceContext->syncSection);
    #endif

        if (gMemtraceContext)
        {
            RegFreeRamByFile(gMemtraceContext->heapFileAllocArray, gMemtraceContext->heapFileAllocCounts, lpMem);
            prvUnregRamTable(lpMem);
        }

    #ifdef MULTI_THREAD_APP
        if (gMemtraceContext)   LeaveCriticalSection(&gMemtraceContext->syncSection);
    #endif
    }
}

/////////////////////////////////////////////////////////////////////////////
WMV_EXTERN_C void prvHeapMeasureStart(const char *logfile)
{
    if (g_wmvTraceFlags & WMV_TRACE_HEAP)
    {
        if (gMemtraceContext)
        {
            free(gMemtraceContext);
        }
        gMemtraceContext = prvCreateMemTraceContext();
        gMemtraceContext->memReportFile = logfile;
    }
}

const char *g_cat_str[TOTAL_HEAP_CATEGORY] = \
    { "General", "Frames", "HuffMan", "Local Huffman", "LocalCount" };

WMV_EXTERN_C int prvWriteMemTraceLog()
{
    FILE *fd;
    int i;

    if (g_wmvTraceFlags & WMV_TRACE_HEAP)
    {

        if (!gMemtraceContext || !gMemtraceContext->memReportFile)  return 0;

        fd = fopen(gMemtraceContext->memReportFile, "at");
        if (!fd)    return 0;

        fprintf(fd, "\nMemory Statistics:\n");
        fprintf(fd, "Total Allocated Memory: %d bytes.\n", gMemtraceContext->totalHeapSize);
        fprintf(fd, "Total Freed Memory: %d bytes.\n", gMemtraceContext->totalFreeSize);
        fprintf(fd, "Total Memory Leak: %d bytes.\n", gMemtraceContext->totalHeapSize-gMemtraceContext->totalFreeSize);
        fprintf(fd, "Maximum Ram Used: %d bytes.\n", gMemtraceContext->dynMax);

        fprintf(fd, "\nMemory Category Statistics:\n");

        for (i=0; i<TOTAL_HEAP_CATEGORY; i++)
        {
            fprintf(fd, "Category %s : Total Allocated %d bytes; Peak Usage: %d\n", g_cat_str[i], gMemtraceContext->catSize[i], gMemtraceContext->catPeakSize[i]);
        }


        if ( (g_wmvTraceFlags & WMV_TRACE_HEAP_DIST) || (gMemtraceContext->totalHeapSize-gMemtraceContext->totalFreeSize != 0))
        {
            fprintf(fd, "Detailed Heap Allocated.\n");
            for(i=0; i<gMemtraceContext->heapFileAllocCounts; i++)
            {
                fprintf(fd, "\n%d bytes in %s %d line. Remain %d bytes.", gMemtraceContext->heapFileAllocArray[i].ramBytes, gMemtraceContext->heapFileAllocArray[i].filename, gMemtraceContext->heapFileAllocArray[i].line_no, gMemtraceContext->heapFileAllocArray[i].dynrambytes);
            }
        }

        fprintf(fd, "\n\n");

        fclose(fd);

    #ifdef MULTI_THREAD_APP
        DeleteCriticalSection( &gMemtraceContext->syncSection );
    #endif

        free(gMemtraceContext);
        gMemtraceContext = 0;
    }

    return 1;
}

//#endif //WANT_HEAP_MEASURE


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

#ifdef WANT_HEAP_MEASURE
	{
		RAM_TABLE *new_entry;
		if (g_wmvTraceFlags & WMV_TRACE_HEAP)
		{

		#ifdef MULTI_THREAD_APP
			if (gMemtraceContext)   EnterCriticalSection(&gMemtraceContext->syncSection);
		#endif

			new_entry = prvRegRamTable(new_ptr, pSize);

			if (gMemtraceContext)
			{
				prvRegAllocRamByFile(gMemtraceContext->heapFileAllocArray, &gMemtraceContext->heapFileAllocCounts, pSize, pFile, pLine, new_entry, category);
			}

		#ifdef MULTI_THREAD_APP
			if (gMemtraceContext)   LeaveCriticalSection(&gMemtraceContext->syncSection);
		#endif

		}
	}
#endif

    return new_ptr;
}

#ifdef WANT_STACK_MEASURE
//Measure the depth of using stack
#define MAX_STACK_NO    4
volatile int* g_piStackMeasureBase[MAX_STACK_NO]={0, 0, 0, 0};
WMV_EXTERN_C void prvStackMeasureStart(int stack_no)
{
    volatile int stackinit[STACK_MEASURE_TOPS];
    int i;

    if (g_wmvTraceFlags & WMV_TRACE_STACK)
    {
        if (stack_no < MAX_STACK_NO && stack_no>=0 )
            g_piStackMeasureBase[stack_no] = stackinit;

        for( i=0; i<STACK_MEASURE_TOPS; i++ )
            stackinit[i] = 0x13579BDF;
    }
}    

WMV_EXTERN_C int prvStackUsed(int stack_no)
{
    //volatile int stackinit[STACK_MEASURE_TOPS];
    int i;
    volatile int *stackinit = g_piStackMeasureBase[stack_no];

//        assert( abs(g_piStackMeasureBase[stack_no] - stackinit) <= 16);

//    if (abs(g_piStackMeasureBase[stack_no] - stackinit) > 16 )
//        return -1;

#if 1
    // stack grows down
    if (g_wmvTraceFlags & WMV_TRACE_STACK)
    {
        for( i=2; i<STACK_MEASURE_TOPS-2; i++ )
        {
            if ( stackinit[i] != 0x13579BDF )
                break;
        }

        return (STACK_MEASURE_TOPS-i)*sizeof(int);
    }
    return 0;
#else
    // stack grows up
    for( i=STACK_MEASURE_TOPS-1; i>=0; i-- )
    {
        if ( stackinit[i] != 0x13579BDF )
            break;
    }
    return i*sizeof(int);

#endif
}

WMV_EXTERN_C int prvWriteStackTraceLog(const char *log_file, int stackNo, int stackSize)
{
    FILE *fd;

    if (g_wmvTraceFlags & WMV_TRACE_STACK)
    {
        if (!log_file)  return 0;

        fd = fopen(log_file, "at");
        if (!fd)    return 0;

        fprintf(fd, "\nStack %d Statistics:\n", stackNo);
        fprintf(fd, "Total Stack Size: %d bytes.\n", stackSize);
        fclose(fd);
    }

    return 1;
}

#endif //WANT_STACK_MEASURE
