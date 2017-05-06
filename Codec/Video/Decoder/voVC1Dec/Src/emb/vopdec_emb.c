//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//codecs/Scrunch/wmv2/decoder/vopdec_ce.cpp#12 - edit change 23357 (text)
#include "xplatform_wmv.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "constants_wmv.h"
#include "tables_wmv.h"
#include "strmdec_wmv.h"
#include "codehead.h"
#include "voWmvPort.h"


#define WMV_ESC_Decoding()                                                          \
    if (pWMVDec->m_bFirstEscCodeInFrame){                                                    \
        decodeBitsOfESCCode (pWMVDec);                                                     \
        pWMVDec->m_bFirstEscCodeInFrame = FALSE;                                             \
    }                                                                               \
    uiRun = BS_getBits  (pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_RUN);                               \
    if (BS_getBit (pWMVDec->m_pbitstrmIn)) /* escape decoding */                              \
        iLevel = -1 * BS_getBits  (pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);                 \
    else                                                                            \
        iLevel = BS_getBits  (pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);         
