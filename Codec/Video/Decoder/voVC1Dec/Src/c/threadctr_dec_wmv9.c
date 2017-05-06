//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) 1996, 1997, 1998  Microsoft Corporation

Module Name:

        vopmbEnc.cpp

Abstract:

        Encoder VOP composed of MB's

Author:

        Ming-Chieh Lee (mingcl@microsoft.com)
        Bruce Lin (blin@microsoft.com)
        Wei-ge Chen (wchen@microsoft.com)
                        
        20-March-1996

Revision History:
        Bruce Lin (blin@microsoft.com)
        Rewrite
                textureDecodePMB ();
        11/04/96


*************************************************************************/
#include "voWmvPort.h"
#include "xplatform_wmv.h"
#include "limits.h"
#include "typedef.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"

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

#include "limits.h"
//#include "windows.h"
#include "voWmvPort.h"
//#include <fstream.h>
//#include <iostream.h>

// while many Target CPU's only come in single proc configuratiions (like all targets in WinCE)
// some housekeeping is doen in this tread specific code so one must call at least init and close
Void_WMV CloseThreads (tWMVDecInternalMember * pWMVDec)
{
    FREE_PTR(pWMVDec, pWMVDec->m_puchFilter8x8RowFlagY0);
    //FREE_PTR(pWMVDec, pWMVDec->m_puchFilter8x8ColFlagY0);
}
//Adjust thread-related variables and pointers whenever encoding size is changed by entry point header.
// Move memory allocation to AllocateMultiThreadBufs_Dec().
//Void_WMV initMultiThreadVars_Dec_WMVA (tWMVDecInternalMember *pWMVDec)
//{	
//	pWMVDec->m_uiMBEnd0 = pWMVDec->m_uintNumMBY;   
//    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;     
//}

//allocate thread buffers according to max encoding size.
//This is only called when the max encoding size is changed by sequence header.
I32_WMV AllocateMultiThreadBufs_Dec_WMVA (tWMVDecInternalMember *pWMVDec,     I32_WMV iMaxEncWidth,    I32_WMV iMaxEncHeight)
{	
	U32_WMV uiMaxMBEnd0;
    I32_WMV iWidthMBAligned = (iMaxEncWidth + 15) & ~15;
    I32_WMV iHeightMBAligned = (iMaxEncHeight + 15) & ~15;
    I32_WMV iWidthY = iWidthMBAligned;
    I32_WMV iWidthUV = iWidthMBAligned >> 1;
    I32_WMV iMaxNumMBX = iWidthMBAligned >> 4; // divided by 16, MB size
    I32_WMV iMaxNumMBY = iHeightMBAligned >> 4;
    I32_WMV iLoopFHeight;
    I32_WMV total_size =0,total_size1 = 0;

    uiMaxMBEnd0 = iMaxNumMBY;
    total_size = 2*(uiMaxMBEnd0 * 2 ) * ((iWidthY + 31) >> 5)+32 
		+ ((uiMaxMBEnd0 * 2 ) >> 1) * ((iWidthUV + 31) >> 5)*4+32;
    iLoopFHeight  = uiMaxMBEnd0 * 16;
    total_size1 = (iMaxNumMBX * 2 - 1) * ((iLoopFHeight + 31) >> 5) +32
                   + (iMaxNumMBX * 2) * ((iLoopFHeight + 31) >> 5)+32
                   +((iMaxNumMBX * 2 - 1) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5)+32
                   +((iMaxNumMBX * 2) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5)+32
                   +((iMaxNumMBX * 2 - 1) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5)+32
                   +((iMaxNumMBX * 2) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5)+32;
#ifdef STABILITY
    if(pWMVDec->m_puchFilter8x8RowFlagY0)
        wmvFree(pWMVDec,pWMVDec->m_puchFilter8x8RowFlagY0);
#endif

     pWMVDec->m_puchFilter8x8RowFlagY0 = (U8_WMV *)wmvMalloc(pWMVDec, total_size + total_size1, DHEAP_STRUCT);
     if (!pWMVDec->m_puchFilter8x8RowFlagY0)
        return ICERR_MEMORY;
    memset(pWMVDec->m_puchFilter8x8RowFlagY0,0,total_size+total_size1);
    
    pWMVDec->m_puchFilter8x4RowFlagY0 = (U8_WMV*)ALIGN(pWMVDec->m_puchFilter8x8RowFlagY0 + (uiMaxMBEnd0 * 2 ) * ((iWidthY + 31) >> 5));
    pWMVDec->m_puchFilter8x8RowFlagU0 = (U8_WMV*)ALIGN(pWMVDec->m_puchFilter8x4RowFlagY0 + (uiMaxMBEnd0 * 2 ) * ((iWidthY + 31) >> 5));
    pWMVDec->m_puchFilter8x4RowFlagU0 = (U8_WMV*)ALIGN(pWMVDec->m_puchFilter8x8RowFlagU0 + ((uiMaxMBEnd0 * 2 ) >> 1) * ((iWidthUV + 31) >> 5));
    pWMVDec->m_puchFilter8x8RowFlagV0 = (U8_WMV*)ALIGN(pWMVDec->m_puchFilter8x4RowFlagU0 + ((uiMaxMBEnd0 * 2 ) >> 1) * ((iWidthUV + 31) >> 5));
    pWMVDec->m_puchFilter8x4RowFlagV0 = (U8_WMV*)ALIGN(pWMVDec->m_puchFilter8x8RowFlagV0 + ((uiMaxMBEnd0 * 2 ) >> 1) * ((iWidthUV + 31) >> 5));

    pWMVDec->m_puchFilter8x8ColFlagY0 =  (U8_WMV*)ALIGN(pWMVDec->m_puchFilter8x8RowFlagY0 + total_size);
    pWMVDec->m_puchFilter4x8ColFlagY0 =  (U8_WMV*)ALIGN(pWMVDec->m_puchFilter8x8ColFlagY0 + (iMaxNumMBX * 2 - 1) * ((iLoopFHeight + 31) >> 5));
    pWMVDec->m_puchFilter8x8ColFlagU0 =  (U8_WMV*)ALIGN(pWMVDec->m_puchFilter4x8ColFlagY0 + (iMaxNumMBX * 2) * ((iLoopFHeight + 31) >> 5));
    pWMVDec->m_puchFilter4x8ColFlagU0 =  (U8_WMV*)ALIGN(pWMVDec->m_puchFilter8x8ColFlagU0 + ((iMaxNumMBX * 2 - 1) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5));
    pWMVDec->m_puchFilter8x8ColFlagV0 =  (U8_WMV*)ALIGN(pWMVDec->m_puchFilter4x8ColFlagU0 + ((iMaxNumMBX * 2) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5));
    pWMVDec->m_puchFilter4x8ColFlagV0 =  (U8_WMV*)ALIGN(pWMVDec->m_puchFilter8x8ColFlagV0 + ((iMaxNumMBX * 2 - 1) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5));

	return ICERR_OK;
}
