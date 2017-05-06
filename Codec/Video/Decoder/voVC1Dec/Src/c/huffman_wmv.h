//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1998 - 1998
//
//  File:       huffman.h
//
//--------------------------------------------------------------------------

/*  -*-c++-*-
 *  Sanjeev Mehrotra
 *  6/19/1998
 *  Chuang Gu
 *  2/1/2000
 */
#ifndef _HUFFMAN_H
#define _HUFFMAN_H
#include "typedef.h"
//extern U32_WMV getMask [33];
#define BITSTREAM_READ 1
//class CInputBitStream_WMV;
typedef enum tagVResultCode
{
  vrNoError        = 0,
  vrFail           = 1,
  vrNotFound       = 2,
  vrExists         = 3,
  vrEof            = 4,
  vrOutOfMemory    = 5,
  vrOutOfResource  = 6,
  vrOutOfBounds    = 7,
  vrBadParam       = 8,
  vrBadInput       = 9,
  vrIOError        = 10,
  vrInterrupted    = 11,
  vrNotSupported   = 12,
  vrNotImplemented = 13,
  vrDropped        = 14,
  vr_ENDOFALLERRORS
} VResultCode;

#define MAX_STAGES 3

#define HUFFMAN_DECODE_ROOT_BITS_LOG    4

//
// returns dimension of decoding table (we suppose that prefix code is full)
//
#define HUFFMAN_DECODE_TABLE_SIZE(iRootBits,iAlphabetSize) ((1 << (iRootBits)) + (2 * (iAlphabetSize)))

//
// Default root table size
//
#ifdef _BIG_HUFFMAN_TABLE_
#define HUFFMAN_DECODE_ROOT_BITS    (10)

// Variable table size
#define SIZE_sm_HufMVTable_V9_MV1           14
#define SIZE_sm_HufMVTable_V9_MV2           14
#define SIZE_sm_HufMVTable_V9_MV3           12
#define SIZE_sm_HufMVTable_V9_MV4           10

#define SIZE_sm_HufBlkXformTypeLowQPTable   5
#define SIZE_sm_HufBlkXformTypeMidQPTable   4
#define SIZE_sm_HufBlkXformTypeHighQPTable  4

#define SIZE_sm_Huf4x4PatternLowQPTable     6
#define SIZE_sm_Huf4x4PatternMidQPTable     5
#define SIZE_sm_Huf4x4PatternHighQPTable    5

#define SIZE_sm_HufDCTACIntraTable_HghRate  13
#define SIZE_sm_HufDCTACInterTable_HghRate  13

#define SIZE_sm_HufDCTACIntraTable_MPEG4    12
#define SIZE_sm_HufDCTACInterTable_MPEG4    12

#define SIZE_sm_HufDCTACIntraTable_Talking  12
#define SIZE_sm_HufDCTACInterTable_Talking  10

#define SIZE_sm_HufDCTDCcTable_HghMt        13
#define SIZE_sm_HufDCTDCyTable_HghMt        14

#define SIZE_sm_HufDCTACIntraTable_HghMt    13
#define SIZE_sm_HufDCTACInterTable_HghMt    13

#define SIZE_sm_HufICBPCYTable              13

#define SIZE_sm_HufMBXformTypeHighQPTable   11
#define SIZE_sm_HufMBXformTypeMidQPTable    8
#define SIZE_sm_HufMBXformTypeLowQPTable    10

#define SIZE_sm_HufPCBPCYTable_V9_1         13
#define SIZE_sm_HufPCBPCYTable_V9_2         8
#define SIZE_sm_HufPCBPCYTable_V9_3         13
#define SIZE_sm_HufPCBPCYTable_V9_4         9

#define SIZE_sm_HufDCTDCcTable_Talking      11
#define SIZE_sm_HufDCTDCyTable_Talking      10
#else
#define HUFFMAN_DECODE_ROOT_BITS    (6)

// Variable table size
#define SIZE_sm_HufMVTable_V9_MV1           10
#define SIZE_sm_HufMVTable_V9_MV2           9
#define SIZE_sm_HufMVTable_V9_MV3           5
#define SIZE_sm_HufMVTable_V9_MV4           8

#define SIZE_sm_HufBlkXformTypeLowQPTable   5
#define SIZE_sm_HufBlkXformTypeMidQPTable   4
#define SIZE_sm_HufBlkXformTypeHighQPTable  4

#define SIZE_sm_Huf4x4PatternLowQPTable     6
#define SIZE_sm_Huf4x4PatternMidQPTable     5
#define SIZE_sm_Huf4x4PatternHighQPTable    5

#define SIZE_sm_HufDCTACIntraTable_HghRate  9
#define SIZE_sm_HufDCTACInterTable_HghRate  8

#define SIZE_sm_HufDCTACIntraTable_MPEG4    8
#define SIZE_sm_HufDCTACInterTable_MPEG4    8

#define SIZE_sm_HufDCTACIntraTable_Talking  8 /*9*/
#define SIZE_sm_HufDCTACInterTable_Talking  6

#define SIZE_sm_HufDCTDCcTable_HghMt        8
#define SIZE_sm_HufDCTDCyTable_HghMt        9

#define SIZE_sm_HufDCTACIntraTable_HghMt    8
#define SIZE_sm_HufDCTACInterTable_HghMt    8

#define SIZE_sm_HufICBPCYTable              8

#define SIZE_sm_HufMBXformTypeHighQPTable   8
#define SIZE_sm_HufMBXformTypeMidQPTable    8
#define SIZE_sm_HufMBXformTypeLowQPTable    7

#define SIZE_sm_HufPCBPCYTable_V9_1         8
#define SIZE_sm_HufPCBPCYTable_V9_2         8
#define SIZE_sm_HufPCBPCYTable_V9_3         9 /*10*/
#define SIZE_sm_HufPCBPCYTable_V9_4         3

#define SIZE_sm_HufDCTDCcTable_Talking      6
#define SIZE_sm_HufDCTDCyTable_Talking      8
#endif
//
// Max codeword length
//
#define HUFFMAN_MAX_CODEWORD_LENGTH (26)


#if !defined(UNDER_CE) && !defined(MIMIC_CE_ON_DESKTOP)   
    // define stages assuming large memory and cache
#   define BITS_STAGE1 10
#   define BITS_STAGE2 11
#else
    // define stages assuming small memory and cache
#   define BITS_STAGE1 6
#   define BITS_STAGE2 15
#endif

// Mode            Entry Size   Max # Symbols   Max codeword length
// ----------------------------------------------------------------
// LARGE_HUFFMAN    8 bytes         Large             32 bits
// MED_HUFFMAN      4 bytes         65536             32 bits
// SMALL_HUFFMAN    2 bytes          4096             32 bits

//#define LARGE_HUFFMAN
#ifndef LARGE_HUFFMAN
#define SMALL_HUFFMAN
#endif

#define ILLEGAL_SYMBOL 4095        /* 2^HUFFDEC_SYMBOL_BITS - 1 */

typedef struct _Huffman_WMV 
{
    I16_WMV *m_hufDecTable;  // new huffman table   
    //U32_WMV m_uiUserData;
    U8_WMV  m_tableSize;     // first table size  
}Huffman_WMV;
#endif

