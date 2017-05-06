//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) 2001  Microsoft Corporation

Module Name:

        deinterlace.cpp

Abstract:

        De-interlacing functions - ported from scrunchcore\interlace_film


Revision History:

*************************************************************************/

#include "xplatform_wmv.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"

#ifndef WMV9_SIMPLE_ONLY

#define LINE_INC_INTRA  8
#define LINE_INC_INTER  2
Void_WMV InitDeinterlace (tWMVDecInternalMember *pWMVDec)
{
     DEBUG_PROFILE_FRAME_FUNCTION_COUNT(InitDeinterlace);
   // For all deinterlacing modes to work properly for 1, 2 or 4 processors, the starting input line number
    // for each thread must be a multiple of 4. This explains the calculation of pWMVDec->m_uiStart1, pWMVDec->m_uiStart2 and pWMVDec->m_uiStart3
    // below. 
    if (pWMVDec->m_uiNumProcessors == 1)
    {
        pWMVDec->m_iStartDeinterlace1 = pWMVDec->m_iHeightPrevY;
    }
    else 
    {
        I32_WMV iTotLines = pWMVDec->m_iHeightPrevY/(2*LINE_INC_INTRA);
        I32_WMV iNum4LineUnits = pWMVDec->m_iHeightPrevY/4;

        if (pWMVDec->m_uiNumProcessors == 2)
        {
            // If the number of 4-line units is odd, give the first processor the extra 4 lines
            pWMVDec->m_iStartDeinterlace1 = ((iNum4LineUnits + 1) / 2) * 4; 
            pWMVDec->m_iStartDeinterlace2 = pWMVDec->m_iHeightPrevY;
        }
        else if (pWMVDec->m_uiNumProcessors == 4)
        {
            I32_WMV iSegs = iTotLines/4;

            iSegs = iNum4LineUnits/4;
            pWMVDec->m_iStartDeinterlace1 = pWMVDec->m_iStartDeinterlace2 = pWMVDec->m_iStartDeinterlace3 = iSegs;
            switch (iNum4LineUnits - iSegs*4)
            {
                case 0:
                    break;
                case 1:
                    pWMVDec->m_iStartDeinterlace1 += 1;
                    break;
                case 2:
                    pWMVDec->m_iStartDeinterlace1 += 1;
                    pWMVDec->m_iStartDeinterlace2 += 1;
                    break;
                case 3:
                    pWMVDec->m_iStartDeinterlace1 += 1;
                    pWMVDec->m_iStartDeinterlace2 += 1;
                    pWMVDec->m_iStartDeinterlace3 += 1;
                    break;
            }
            pWMVDec->m_iStartDeinterlace1 = pWMVDec->m_iStartDeinterlace1*4;
            pWMVDec->m_iStartDeinterlace2 = pWMVDec->m_iStartDeinterlace1 + pWMVDec->m_iStartDeinterlace2*4;
            pWMVDec->m_iStartDeinterlace3 = pWMVDec->m_iStartDeinterlace2 + pWMVDec->m_iStartDeinterlace3*4;
        }
    }
}

#if 0
Void_WMV Deinterlaceprocess0 (tWMVDecInternalMember *pWMVDec )
{
    (*pWMVDec->m_pDeinterlaceProc)(pWMVDec, pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliPostQY, 0, pWMVDec->m_iStartDeinterlace1);
}

Void_WMV Deinterlaceprocess1 (tWMVDecInternalMember *pWMVDec )
{
    (*pWMVDec->m_pDeinterlaceProc)(pWMVDec, pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliPostQY, pWMVDec->m_iStartDeinterlace1, pWMVDec->m_iStartDeinterlace2);
}

Void_WMV Deinterlaceprocess2 (tWMVDecInternalMember *pWMVDec )
{
    (*pWMVDec->m_pDeinterlaceProc)(pWMVDec, pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliPostQY, pWMVDec->m_iStartDeinterlace2, pWMVDec->m_iStartDeinterlace3);
}

Void_WMV Deinterlaceprocess3 (tWMVDecInternalMember *pWMVDec)
{
    (*pWMVDec->m_pDeinterlaceProc)(pWMVDec, pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliPostQY, pWMVDec->m_iStartDeinterlace3, pWMVDec->m_iHeightPrevY);
}
#endif
#endif //WMV9_SIMPLE_ONLY
