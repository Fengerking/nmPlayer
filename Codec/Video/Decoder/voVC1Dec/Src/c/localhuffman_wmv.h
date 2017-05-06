//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//#define ALT_TRAIN
/*************************************************************************

Copyright (c) 1996 -- 2000  Microsoft Corporation

Module Name:

    localHuffman.h

Abstract:

    Local Huffman code generator.

Revision History:

    Sridhar Srinivasan: 3/16/00

*************************************************************************/

#include "xplatform_wmv.h"
#include "typedef.h"
#include "codehead.h"

#include "voWmvPort.h"
#if  !defined(__arm)
#include <memory.h>
#endif

//#include "strmdec_wmv.h"
#include "huffman_wmv.h"
#include "wmvdec_member.h"

/********************************************************************
  end of definition of CLocalHuffman
********************************************************************/

enum { QP_AT_8BIT_LEVEL = 5 };

enum { L0a=46, L0b, L0c, L0d, L0e, L0f,  L0z, L0y, L0x, L0w, L0v,  L0u, L0t,
/*59*/ L1a, L1b, L1c, L1d, L1e, L1f,  L1g, L1h, L1i,  L1z, L1y, L1x, L1w, L1v,
/*73*/ Q0, Q1, X0, X1 };

static const I32_WMV SIGNSHIFT=16; // right shift leaves only info regarding sign
// low QP encoding (static works best)

static const I32_WMV iModeArray[]={ LH_INTER0, LH_INTRAY0, LH_INTER0,
    LH_INTRAY, LH_INTER /*experimental*/ };

/**********************************************************************
 CAltTablesDecoder : definition
**********************************************************************/
//t_CAltTablesDecoder *t_AltTablesDecoderConstruct (tWMVDecInternalMember *pWMVDec, U32_WMV uiUserData);
//Void_WMV    t_AltTablesDecoderDestruct (tWMVDecInternalMember *pWMVDec, t_CAltTablesDecoder *pAltTab);
//Void_WMV    t_AltTablesSetNewFrame (t_CAltTablesDecoder *pAltTab,
//                                    I32_WMV, I32_WMV);
//Void_WMV t_AltTablesDecodeHeader (t_CAltTablesDecoder *pAltTab, CInputBitStream_WMV *pbsIn);
/**********************************************************************
 Static variables
**********************************************************************/

