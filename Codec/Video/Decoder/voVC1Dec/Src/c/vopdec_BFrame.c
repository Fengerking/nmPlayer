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

//static I32_WMV s_iBlkXformTypeLUT[8] = {
//    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
//    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
//};
//
//static I32_WMV s_iMBXformTypeLUT[16] = {
//    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
//    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
//    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
//    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
//};
//static I32_WMV s_iSubblkPatternLUT[16] = {0, 1, 2, 3, 1, 2, 3, 0, 0, 1, 2, 3, 1, 2, 3, 0};

/********************************************************************************
 session stuff (should be moved out to sesdec later)
********************************************************************************/
Void_WMV swapCurrRef1Pointers (tWMVDecInternalMember *pWMVDec)
{
#ifdef SHAREMEMORY
	pWMVDec->m_pfrmRef1Q = pWMVDec->m_pfrmCurrQ;
	pWMVDec->m_ppxliRef1Y = pWMVDec->m_pfrmRef1Q->m_pucYPlane;
    pWMVDec->m_ppxliRef1U = pWMVDec->m_pfrmRef1Q->m_pucUPlane;
    pWMVDec->m_ppxliRef1V = pWMVDec->m_pfrmRef1Q->m_pucVPlane;
    pWMVDec->m_ppxliRef1YPlusExp = pWMVDec->m_ppxliRef1Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
#else
    tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmCurrQ;
    pWMVDec->m_pfrmCurrQ = pWMVDec->m_pfrmRef1Q;
    pWMVDec->m_pfrmRef1Q = pTmp;
    pWMVDec->m_ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
    pWMVDec->m_ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
    pWMVDec->m_ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane;
    pWMVDec->m_ppxliRef1Y = pWMVDec->m_pfrmRef1Q->m_pucYPlane;
    pWMVDec->m_ppxliRef1U = pWMVDec->m_pfrmRef1Q->m_pucUPlane;
    pWMVDec->m_ppxliRef1V = pWMVDec->m_pfrmRef1Q->m_pucVPlane;
    pWMVDec->m_ppxliRef1YPlusExp = pWMVDec->m_ppxliRef1Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
#endif
}

Void_WMV swapRef0Ref1Pointers (tWMVDecInternalMember *pWMVDec)
{
    tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmRef0Q;

    pWMVDec->m_pfrmRef0Q = pWMVDec->m_pfrmRef1Q;
    pWMVDec->m_pfrmRef1Q = pTmp;
    
    pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmRef0Q->m_pucYPlane;
    pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Q->m_pucUPlane;
    pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Q->m_pucVPlane;
    
    pWMVDec->m_ppxliRef1Y = pWMVDec->m_pfrmRef1Q->m_pucYPlane;
    pWMVDec->m_ppxliRef1U = pWMVDec->m_pfrmRef1Q->m_pucUPlane;
    pWMVDec->m_ppxliRef1V = pWMVDec->m_pfrmRef1Q->m_pucVPlane;   
    
    pWMVDec->m_ppxliRef1YPlusExp = pWMVDec->m_ppxliRef1Y + pWMVDec->m_iWidthPrevYXExpPlusExp;    
    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
    
   
}

