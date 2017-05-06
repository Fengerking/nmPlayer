//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#ifndef __WMVDEC_MEMBER_H_
#define __WMVDEC_MEMBER_H_
#include "wmvdec_api.h"
#include "typedef.h"

#include "xplatform_wmv.h"
#include "voWmvPort.h"
#include "huffman_wmv.h"
#include "strmdec_wmv.h"
#include "motioncomp_wmv.h"
#include "wmvdec_api.h"
#ifdef XBOXGPU
#include "xboxdec.h"
#endif

/* Should close this log info when release. */
#ifdef _VOLOG_INFO
//#define VO_LOG_INFO_MemoryFree
//#define VO_LOG_INFO_traceFunction
#endif

/* Should close this log info when release. */
//#define DUMP_INPUT_DATA


#define VIDEO_MEM_SHARE
#define VIDEO_MEM_SHARE_tmpVersion //Only share video memory,not use the APP's buffer management(use decoder's internal buffer management).

#if defined(_WIN32) || defined(_WIN16)
#       include <windows.h>
#       ifdef UNDER_CE
#           include "vfw_ce.h"
#       else
#        //   include <vfw.h>
#       endif // UNDER_CE
#endif

#ifdef _VOLOG_INFO
//#include "voLog.h"
#endif

//class CInputBitStream_WMV;
//class ifstream;
//class istream;
//class Huffman_WMV;


#ifdef WMVIMAGE_V2
#include "EffectTypes_wmv.h"
#endif

#define BLOCK_SIZE_2    32
#define BLOCK_SQUARE_SIZE_2 256
#define BLOCK_SQUARE_SIZE_2MINUS2 252
#define BLOCK_SQUARE_SIZE_MINUS1_4 252

#define MAXHALFQP           8
#define MAXHIGHRATEQP       8
#define MAX3QP              8

#ifdef _WMV9AP_
typedef enum DCACPREDDIR {LEFT, TOP, NONE} DCACPREDDIR;
#endif

extern I32_WMV g_iStepRemap[];      // QP-index to QP mapping array used for 5QP deadzone quantizer.

// Profiles for WMV3



#ifdef _WMV9AP_
enum { NOT_WMV3 = -1, WMV3_SIMPLE_PROFILE, WMV3_MAIN_PROFILE, WMV3_PC_PROFILE, WMV3_ADVANCED_PROFILE, WMV3_SCREEN };
#else
enum { NOT_WMV3 = -1, WMV3_SIMPLE_PROFILE, WMV3_MAIN_PROFILE, WMV3_PC_PROFILE, WMV3_SCREEN };
#endif

#ifdef _EMB_SSIMD64_
    typedef I64_WMV I_SIMD;
    typedef U64_WMV U_SIMD;
#elif defined(_EMB_SSIMD32_)
    typedef I32_WMV I_SIMD;
    typedef U32_WMV U_SIMD;
#else
    typedef I32_WMV I_SIMD;
    typedef U32_WMV U_SIMD;
#endif


#ifdef _12BITCOEFCLIP
    extern Void_WMV ClipCoef32 (U32_WMV b, U32_WMV n, I32_WMV *p);
    extern Void_WMV ClipCoef16 (U32_WMV b, U32_WMV n, I16_WMV *p);
#   define CLIPCOEF32(b,n,p32) ClipCoef32(b,n,p32)
#   define CLIPCOEF16(b,n,p16) ClipCoef16(b,n,p16)
#else
#   define CLIPCOEF32(b,n,p32)
#   define CLIPCOEF16(b,n,p16)
#endif



extern Bool_WMV g_bSupportMMX_WMV;

#ifdef _6BIT_COLORCONV_OPT_
typedef struct
{
    I8_WMV g_iYscale [256];
    I8_WMV g_iVtoR [256];
    I8_WMV g_iVtoG [256];
    I8_WMV g_iUtoG [256];
    I8_WMV g_iUtoB [256];
    
}
YUV2RGB_6BIT_TABLE;

#endif

#define MAX_ADDR_RELOCABLE 8

typedef struct tUNCACHE_RELOC_CTL
{

    U8_WMV ** ppAddrReloc[MAX_ADDR_RELOCABLE];
    
    I32_WMV   iNumAddrReloc;

}
UNCACHE_RELOC_CTL;

typedef struct tUNCACHE_CTL
{
 //   U8_WMV ** ppAddrRelocatable;
    UNCACHE_RELOC_CTL reloc;
    
    I32_WMV  addr;
    struct tUNCACHE_CTL * next;
}
UNCACHE_CTL;

#define MAXPANSCANWINDOWS       4
typedef struct tagCPanScanWindowInfo {
    I32_WMV     iWidth;
    I32_WMV     iHeight;
    I32_WMV     iHorizOffset;
    I32_WMV     iVertOffset;
}CPanScanWindowInfo;

typedef struct tagCPanScanInfo {
    CPanScanWindowInfo  sWindowInfo [MAXPANSCANWINDOWS];
}CPanScanInfo;


#ifdef _EMB_WMV2_

/* the relative offsets of the fields of these classes are important for CE assembly optimizations. Affected files are:
    blkdec_ce_arm.s
*/

struct tagWMVDecInternalMember;

typedef struct _CDCTTableInfo_Dec {
    Huffman_WMV* hufDCTACDec; 
    U32_WMV  iTcoef_ESCAPE;
    U32_WMV  iStartIndxOfLastRunMinus1;
    U8_WMV* puiNotLastNumOfLevelAtRun; 
    U8_WMV* puiLastNumOfLevelAtRun; 
    U8_WMV* puiNotLastNumOfRunAtLevel; 
    U8_WMV* puiLastNumOfRunAtLevel;
    I8_WMV* pcLevelAtIndx; 
    U8_WMV* puiRunAtIndx; 
    I16_WMV * combined_levelrun;
}CDCTTableInfo_Dec;

/* the relative offsets of the fields of these classes are important for CE assembly optimizations. Affected files are:
    blkdec_ce_arm.s
*/
typedef struct tagEMB_DecodeP_ShortCut
{

    struct tagWMVDecInternalMember * pVideoDecoderObject;

    I32_WMV * m_rgiCoefRecon;
    own CInputBitStream_WMV * m_pbitstrmIn;
    I32_WMV m_i2DoublePlusStepSize;
    I32_WMV m_iDoubleStepSize;
    I32_WMV m_iStepMinusStepIsEven;

    I32_WMV XFormMode;
    I32_WMV m_iDCTHorzFlags;

}EMB_DecodeP_ShortCut;

#else

typedef struct _CDCTTableInfo_Dec {
    Huffman_WMV* hufDCTACDec; 
    U32_WMV  iTcoef_ESCAPE;
    U32_WMV  iStartIndxOfLastRunMinus1;
    U8_WMV* puiNotLastNumOfLevelAtRun; 
    U8_WMV* puiLastNumOfLevelAtRun; 
    U8_WMV* puiNotLastNumOfRunAtLevel; 
    U8_WMV* puiLastNumOfRunAtLevel;
    I8_WMV* pcLevelAtIndx; 
    U8_WMV* puiRunAtIndx;
    I16_WMV * combined_levelrun;
}CDCTTableInfo_Dec;

#endif

                            
typedef enum THREADTYPE_DEC {REPEATPAD, DECODE, RENDER, LOOPFILTER, DEBLOCK, DEBLOCK_RENDER, DEINTERLACE} THREADTYPE_DEC;
typedef enum KEYS {FPPM, CPPM, PPM, RES, BITRATE, ADAPTPPM, ADAPTMHZ, QPCLEVEL1, QPCLEVEL2, QPCLEVEL3, QPCLEVEL4, OMITBFM, FAKEBEHIND, COUNTCPUCYCLES} KEYS;
typedef enum SKIPBITCODINGMODE {Normal = 0, RowPredict, ColPredict} SKIPBITCODINGMODE;

#ifdef _SUPPORT_POST_FILTERS_
    I32_WMV prvWMVAccessRegistry(enum KEYS k, char rw, I32_WMV iData);
#endif // _SUPPORT_POST_FILTERS_

// Intra X8 stuff
typedef struct t_SpatialPredictor {
    const  U8_WMV   *m_pClampTable;
    const U8_WMV    *m_pRef;
    I32_WMV         m_iRefStride;
    Bool_WMV        m_bFlat;
    I32_WMV         m_iDcValue;
    U8_WMV          *m_pLeft, *m_pTop;
    U8_WMV          *m_pNeighbors;
    Bool_WMV        m_bInitZeroPredictor;
    U16_WMV         *m_pSums;
    U16_WMV         *m_pLeftSum;
    U16_WMV         *m_pTopSum;
    I16_WMV         *m_pBuffer16; // buffer for setupZeroPredictor speedup

    Void_WMV  (*m_pIDCT_Dec) (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffset, I32_WMV iFlags);  // decoder side

} t_CSpatialPredictor;


typedef struct t_ContextWMV {
    I32_WMV m_iRow;
    I32_WMV m_iCol;
    I32_WMV m_iColGt2;
    U8_WMV *m_pData; // one copy enough?
} t_CContextWMV;

struct t_AltTablesDecoder; 

/**********************************************************************
 CLocalHuffmanDecoder : definition
**********************************************************************/
typedef struct t_LocalHuffmanDecoder
{
    struct t_AltTablesDecoder  *m_pAltTable;

    I32_WMV  m_iN;  // number of symbols inc escapes
    const U8_WMV *m_pFine;
    I32_WMV  m_iMode;
    I32_WMV  m_i1shiftMode;
    I32_WMV  m_iK;  // number of codeword tables
    I32_WMV  logK;
    U8_WMV   *opt;
    U8_WMV   *m_pCodeTablePtr;

    I32_WMV  *fineMask;
    Huffman_WMV  *m_pHuff;
    tPackHuffmanCode_WMV      *m_pCodeTable;

} t_CLocalHuffmanDecoder;


enum { LH_INTER0, LH_INTER, LH_INTRAY0, LH_INTRAY,
       // IntraUV uses Inter tables
       LH_ORIENT, LH_INTRAZ, LH_INTRANZ, LH_INTRAC0, 
       LH_MV0, LH_MV1, LH_CBP,
       LH_INTRAC1,              // New Intra coding
       LH_ENUM_SIZE             // possible number of contexts
    };


typedef struct t_AltTablesDecoder
{
    U32_WMV    m_uiUserData;
    I32_WMV    m_iNewFrame;
    I32_WMV    m_iNewIntraFrame;
    struct t_LocalHuffmanDecoder   *m_paLH[LH_ENUM_SIZE];
    Bool_WMV   m_bIntraFrame;
    I32_WMV    m_iNContexts;

    I32_WMV    m_iQLev;
    Bool_WMV   m_bIsFirstRun;
    I32_WMV    m_iHalfPelMV; // X9

} t_CAltTablesDecoder;

// WMV2's new routines

#ifdef DYNAMIC_EDGEPAD
typedef struct
{
    I32_WMV m_iStartOfLastLine;
    I16_WMV m_iPMBMV_limit_x;  //m_iWidth-BLOCKSIZE_PLUS_2EDGES
    I16_WMV m_iPMBMV_limit_y;  //m_iHeight-BLOCKSIZE_PLUS_2EDGES
    I16_WMV m_iWidth;    
    I16_WMV m_iHeight;
}EdgePad_Y_UV_Data;

typedef struct
{
    U8_WMV * m_ppxliRef; //MB(Y) or Blk(UV) starting address for ref plane
    U8_WMV * m_ppxliCurr; //MB(Y) or Blk(UV) starting address for curr plane
    U8_WMV * m_ppxliRef0; //origins of ref for the YUV planes
         
    I32_WMV m_BlkInc;  //block incremental in the normal case
}
EdgePad_Blk_Data;

typedef struct _WMVEdgePad
{
    I16_WMV m_iPBMV_x;  //position of the (enlarged) block : block + surrounding edges
    I16_WMV m_iPBMV_y;  
    I16_WMV m_iWidthYMinusAllEdges;  
    I16_WMV m_iHeightYMinusAllEdges;  
    I16_WMV x_off_left;
    I16_WMV x_off_right;
    I32_WMV src_offset; 
    U8_WMV * ppxlcRefBufferOrig; 
    U8_WMV * ppxlcRef; 
    U8_WMV * ppxlcCurr;
    U32_WMV m_iWidthCombine;
    U32_WMV m_iround_ctrl;

    EdgePad_Y_UV_Data y_uv_data[2];

    EdgePad_Blk_Data blk_data[6];

    U8_WMV * buffer;

#ifdef MEMBER_EDGEPAD_FUNC_TABLE
    Void_WMV (*EdgePadFuncTable[16])(struct _WMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData)
#endif

}
tWMVEdgePad;

#endif
// basic structs

typedef enum tagFrameType_WMV
{
//    IFRAME_WMV = 0, 
//    PFRAME_WMV
      IVOP = 0, 
      PVOP,
      BVOP,
      SPRITE
#ifdef _WMV9AP_
      , BIVOP,
      SKIPFRAME
#endif
} tFrameType_WMV;

//#ifdef _EMB_3FRAMES_

#define FRMOP_DEBLOCK                           1
#define FRMOP_COPY_CURR2POST          2
#define FRMOP_COPY_REF2PREVPOST   3
#define FRMOP_ADJUST_DEC_RANGE       4
#define FRMOP_UPSAMPLE                          8


typedef struct    tagDeBlockCtl 
{

    I32_WMV m_iOpCode ;

    struct tagYUV420Frame_WMV * m_pfrmSrc;
    I32_WMV m_iWidthPrevY ;
    I32_WMV m_iWidthPrevUV;
    I32_WMV m_iWidthPrevYXExpPlusExp;
    I32_WMV m_iWidthPrevUVXExpPlusExp ;
    I32_WMV m_uintNumMBX;
    I32_WMV m_uintNumMBY;
    I32_WMV m_iHeightUV;
    I32_WMV m_iStepSize ;
    I32_WMV m_iMBSizeXWidthPrevY;
    I32_WMV m_iBlkSizeXWidthPrevUV;
    Bool_WMV m_bDeringOn ;

    I32_WMV iMBStartY;
    I32_WMV iMBEndY;

}RF_DeBlockCtl ;

typedef struct    tagCpyCtrl 
{
     I32_WMV m_iOpCode;

      struct tagYUV420Frame_WMV *  m_pfrmSrc;
  //    I32_WMV m_iAreaY  ;
  //    I32_WMV m_iAreaUV;

      I32_WMV m_uintNumMBY;
      I32_WMV m_iWidthPrevY  ;
      I32_WMV m_iWidthPrevUV ;
  //    I32_WMV m_iHeightUV;

      I32_WMV m_iWidthPrevYXExpPlusExp ;
      I32_WMV m_iWidthPrevUVXExpPlusExp ;
}
RF_CpyCtl;

typedef struct    tagAdjustDecRangeCtrl 
{

     I32_WMV m_iOpCode;
     struct tagYUV420Frame_WMV *  m_pfrmSrc;

      I32_WMV m_iWidthPrevYXExpPlusExp ;
      I32_WMV m_iWidthPrevUVXExpPlusExp ;
      I32_WMV m_uintNumMBY;

      I32_WMV m_iWidthPrevY;
      I32_WMV m_iWidthPrevUV;
     // I32_WMV m_iHeightPrevY;
     // I32_WMV m_iHeightPrevUV;
      I32_WMV m_iRangeState;
}
RF_AdjustDecRangeCtl;

typedef struct  tagUpsampleRefCtl
{

     I32_WMV m_iOpCode;
     struct tagYUV420Frame_WMV *  m_pfrmSrc;
    

     I32_WMV m_iHeightY;
     I32_WMV m_iWidthY;
     I32_WMV m_iHeightUV;
     I32_WMV m_iWidthUV;

     I32_WMV m_iWidthPrevYXExpPlusExpSrc ;
     I32_WMV m_iWidthPrevUVXExpPlusExpSrc;
     I32_WMV m_iWidthPrevYXExpPlusExpDest ;
     I32_WMV m_iWidthPrevUVXExpPlusExpDest;
     I32_WMV m_iResIndex ;
     I32_WMV m_iResIndexPrev;
     I32_WMV m_iWidthPrevYSrc ;
     I32_WMV m_iWidthPrevUVSrc ;
     I32_WMV m_iWidthPrevY ;
     I32_WMV m_iWidthPrevUV;
     U32_WMV m_uiNumMBY;
}
RF_UpsampleRefCtl;

typedef union tagFrmOps
{
    RF_DeBlockCtl  m_OpDeblock;
    RF_CpyCtl         m_OpCpyCtl;
    RF_AdjustDecRangeCtl  m_OpDecRange;
    RF_UpsampleRefCtl  m_OpUpSample;

}FrmOpUnions;



//#endif

typedef struct tagYUV420Frame_WMV
{
    U8_WMV* m_pucYPlane;
    U8_WMV* m_pucUPlane;
    U8_WMV* m_pucVPlane;

    U8_WMV* m_pucYPlane_Unaligned;
    U8_WMV* m_pucUPlane_Unaligned;
    U8_WMV* m_pucVPlane_Unaligned;

    Bool_WMV m_bAltUsed;
    U8_WMV* m_pucYPlaneAlt;
    U8_WMV* m_pucUPlaneAlt;
    U8_WMV* m_pucVPlaneAlt;

    U8_WMV* m_pCurrPlaneBase;
//#ifdef _EMB_3FRAMES_
    FrmOpUnions   m_pOps[8];
    I32_WMV  m_iNumOps;
//#endif
	long long	m_timeStamp;
	tFrameType_WMV m_frameType;
    VO_PTR		m_UserData;
	
} tYUV420Frame_WMV;

typedef struct tagMotionVector_WMV
{
    I16_WMV m_iMVX;
    I16_WMV m_iMVY;
} tMotionVector_WMV;

typedef struct {
    I32_WMV iWidthY;
    I32_WMV iWidthUV;
    I32_WMV iHeightY;
    I32_WMV iHeightUV;
    I32_WMV iFrmWidthSrc;
    I32_WMV iFrmHeightSrc;
    I32_WMV iWidthYPlusExp;
    I32_WMV iWidthUVPlusExp;
//    I32_WMV iAreaY;
    Bool_WMV bMBAligned;
    U32_WMV uiNumMBX;
    U32_WMV uiNumMBY;
    U32_WMV uiNumMB;
    U32_WMV uiRightestMB;
    I32_WMV iWidthPrevY;
    I32_WMV iWidthPrevUV;
    I32_WMV iHeightPrevY;
    I32_WMV iHeightPrevUV;
    I32_WMV iWidthPrevYXExpPlusExp;
    I32_WMV iWidthPrevUVXExpPlusExp;
    I32_WMV iMBSizeXWidthPrevY;
    I32_WMV iBlkSizeXWidthPrevUV;
} MULTIRES_PARAMS;

typedef struct _CQueueElement
{

//    CQueueElement(){};
    struct _CQueueElement* m_pNext;
    Void_WMV* m_pElement;
}CQueueElement;

typedef struct 
{
    CQueueElement *m_pUsedHead;
    CQueueElement *m_pUsedTail;
    CQueueElement *m_pAvailHead;
    CQueueElement *m_pAvailTail;

    I32_WMV m_iCurrQueueElements;
    I32_WMV m_iMaxQueueElements;
}CQueue;

typedef struct 
{

    I32_WMV m_iCurrRefFrames;
    I32_WMV m_iFrameNum;
    I32_WMV m_iFramesChecked;
    CQueue *m_cFrameQueue;
}CReferenceLibrary;

//#ifdef X9
typedef struct _CDiffMV
{
    I32_WMV      iX : 16;
    I32_WMV      iY : 12;
    U32_WMV     iLast : 1;
    U32_WMV     iIntra : 1;
    U32_WMV     iHybrid : 2;   // 2 bits enough
}CDiffMV;



typedef union _Motion_EMB
{
    I16_WMV   I16[2];
    U32_WMV  U32;
    I32_WMV   I32;
}
UMotion_EMB;

typedef struct _CDiffMV_EMB
{
    UMotion_EMB      Diff;
    
    U8_WMV     iLast ;
    U8_WMV     iIntra;
    U16_WMV     iHybrid;   // 2 bits enough
}CDiffMV_EMB;
// Zone_VLC
#define END1STZONE 3
#define END2NDZONE 10
#define MAXHALFQP           8
#define MAXHIGHRATEQP       8
#define MAX3QP              8


#define NUM_LIBREF_FRAMES   2

// Frame level motion mode
enum { MIXED_MV, ALL_1MV, ALL_1MV_HALFPEL, ALL_1MV_HALFPEL_BILINEAR };
// MB level motion mode
enum { MB_1MV, MB_4MV, MB_FIELD, MB_FIELD4MV,MB_INTRA};



// Filter type
enum { FILTER_BILINEAR, FILTER_BICUBIC, FILTER_6TAP };
// Frame level skipbit (or MV switch) coding mode
enum { SKIP_RAW, SKIP_NORM2, SKIP_DIFF2, SKIP_NORM6, SKIP_DIFF6, SKIP_ROW, SKIP_COL, NOT_DECIDED};
#define  IBLOCKMV  0x4000

typedef struct {
    I32_WMV iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven;    
    I32_WMV i2DoublePlusStepSize;
    I32_WMV i2DoublePlusStepSizeNeg;
    I32_WMV iDCStepSize; // For Intra    
} DQuantDecParam;

typedef struct {
    I8_WMV    chFlag[6];
} LOOPF_FLAG;

#define HARIZONTALEDGEMB 0x05
#define VERTICALEDGEMB 0x0a
#define DQUANTALLEDGEMB 0x0f

#define STARTCODE1 170 // 0xAA
#define STARTCODE2 171 // 0xAB
#define LENSTARTCODE 24

typedef struct tCVector_X9
{
    I16_WMV   x;
    I16_WMV   y;
}   CVector_X9;



typedef struct tCMotionVector_X9
{
    CVector_X9 m_vctTrueHalfPel;
    CVector_X9 m_vctPredMVHalfPel;
}   CMotionVector_X9;


typedef struct tCMotionVector_X9_EMB
{

    UMotion_EMB  m_vctTrueHalfPel;
}   CMotionVector_X9_EMB;


enum { FrameDecOut_None, FrameDecOut_OutputOptional, FrameDecOut_OutputDone, FrameDecOut_Decoded, FrameDecOut_Dropped };

typedef struct _PredictMVLUTPullBack
{
    U32_WMV m_uiMin;
    U32_WMV m_uiMax;
}
PredictMV_LUT_PullBack;

typedef struct _PredictMVLUT_UNION_MISC
    {
        I32_WMV  dummy[6];
        I8_WMV * m_pBOffset;
        U8_WMV m_RndTbl[4];
    }PredictMVLUT_UNION_MISC;

typedef union __PredictMVLUT_UNION
{
/*
    struct 
    {
        I32_WMV  dummy[6];
        I8_WMV * m_pBOffset;
        U8_WMV m_RndTbl[4];
    }U;
*/
    PredictMVLUT_UNION_MISC     U;
    PredictMV_LUT_PullBack m_PullBack[5];
}
PredictMVLUT_UNION;

typedef struct _PredictMVLUT
    {
        I8_WMV m_rgcBOffset[16];
     //   I8_WMV * m_pBOffset;
     //   U32_WMV m_uiMin[2];
    //    U32_WMV m_uiMax;
        PredictMVLUT_UNION U;
        
 
    }
    PredictMV_LUT;

//#define USE_SHORT

typedef struct tagEMB_PBMainLoop
{

    struct tagWMVDecInternalMember  *m_pWMVDec;
    CDiffMV_EMB * m_pDiffMV_EMB ;
    I16_WMV * m_rgiMVDecLUT_EMB ;
#ifdef USE_SHORT
    U8_WMV m_iQP;
    U8_WMV m_iMVSwitchCodingMode ;
    U8_WMV m_iSkipbitCodingMode;
    U8_WMV m_bDQuantOn;
    U8_WMV m_bDCTTable_MB;
    U8_WMV m_bMBXformSwitching ;
    U8_WMV m_bHalfPelMV;
    U8_WMV m_bUVHpelBilinear ;
    U8_WMV m_iNumBFrames ;
    U8_WMV m_iOverlap;
    U8_WMV m_iFrameXformMode ;
    U8_WMV m_iRndCtrl ;
    U8_WMV m_rgiBlkIdx[6] ;
    U8_WMV m_iFilterType[2] ;
    U16_WMV m_iNumBlockX ;
    U16_WMV m_iNumBlockY;
    
    

    U16_WMV m_iWidthY;
    U16_WMV m_iHeightY;
    U16_WMV m_iWidthUV;
    U16_WMV m_iHeightUV;
    U16_WMV m_iXMVRange ;
    U16_WMV m_iYMVRange ;
    U16_WMV m_iXMVFlag ;
    U16_WMV m_iYMVFlag ;
    U16_WMV m_iLogXRange;
    U16_WMV m_iLogYRange;
    
    U16_WMV m_iWidthPrevY;
    U16_WMV m_iWidthPrevUV ;
    U16_WMV m_iHeightPrevY ;
    U16_WMV m_iHeightPrevUV ;
    U16_WMV m_iWidthPrev[2];
    I32_WMV m_iDCTHorzFlags;
#else
    I32_WMV m_iQP;
    I32_WMV m_iMVSwitchCodingMode ;
    I32_WMV m_iSkipbitCodingMode;
    Bool_WMV m_bDQuantOn;
    Bool_WMV m_bDCTTable_MB;
    Bool_WMV m_bMBXformSwitching ;
    Bool_WMV m_bHalfPelMV;
    I32_WMV m_iNumBlockX ;
    I32_WMV m_iNumBlockY;
    I32_WMV m_rgiBlkIdx[6] ;
    
    I32_WMV m_iWidthY;
    I32_WMV m_iHeightY;
    I32_WMV m_iWidthUV;
    I32_WMV m_iHeightUV;

    I32_WMV m_iXMVRange ;
    I32_WMV m_iYMVRange ;
    I32_WMV m_iXMVFlag ;
    I32_WMV m_iYMVFlag ;
    I32_WMV m_iLogXRange;
    I32_WMV m_iLogYRange;

    I32_WMV m_iNumBFrames ;
    Bool_WMV m_bUVHpelBilinear ;
    I32_WMV m_iWidthPrevY;
    I32_WMV m_iWidthPrevUV ;
    I32_WMV m_iHeightPrevY ;
    I32_WMV m_iHeightPrevUV ;
    I32_WMV m_iWidthPrev[2];
    I32_WMV m_iFrameXformMode ;
    I32_WMV m_iDCTHorzFlags;
    Bool_WMV m_iOverlap;
	I32_WMV m_iFilterType[2] ;
    I32_WMV m_iRndCtrl ;
#endif

    own CInputBitStream_WMV* m_pbitstrmIn;
    Huffman_WMV* m_pHufMVTable_V9 ;
     I16_WMV  *m_pXMotion;
     I16_WMV  *m_pYMotion;
     UMotion_EMB * m_pMotion;
     UMotion_EMB * m_pMotionC;
//    I16_WMV  *m_pXMotionC;
//   I16_WMV    *m_pYMotionC;
    
        
    Huffman_WMV* m_pHufNewPCBPCYDec ;
    Huffman_WMV* m_pHufMBXformTypeDec;
    I32_WMV m_rgiCurrBaseBK[2];
    //I32_WMV m_iCurrMB ;


    CMotionVector_X9_EMB * m_rgmv1_EMB;



    DQuantDecParam * m_prgDQuantParam;
    CDCTTableInfo_Dec** m_pInterDCTTableInfo_Dec_Set ;
    CDCTTableInfo_Dec** m_pIntraDCTTableInfo_Dec_Set ;
    CDCTTableInfo_Dec** m_ppInterDCTTableInfo_Dec ;
    CDCTTableInfo_Dec** m_ppIntraDCTTableInfo_Dec ;
    UnionBuffer* m_rgiCoefReconBuf ;
    I16_WMV  * m_pX9dct ;
    I16_WMV  * m_pX9dctUV[2] ;
    


    U8_WMV* m_pZigzagScanOrder;

    I32_WMV m_rgiSSIMD_00Tbl[2];

    I8_WMV m_rgiSubsampleBicubic[16] ;

    U8_WMV * m_ppxliRefBlkBase[6];
   
    U8_WMV * m_ppxliCurrBlkBase[6] ;
    
    I32_WMV m_iFrmMBOffset[2] ;
 
    UMotion_EMB m_iPred;

 
    //PredictMV_LUT  m_PredictMVLUT;

    I16_WMV m_iBlkMBBase[2];
    

     I16_WMV  * m_pIntraBlockRow[6] ;
     
     I32_WMV m_iACColPredShift;
     I32_WMV m_iACRowPredShift;
     I32_WMV m_iACColPredOffset;
     I32_WMV m_iACRowPredOffset;

     Huffman_WMV* m_pHufBlkXformTypeDec ;
     Huffman_WMV* m_pHuf4x4PatternDec;

     UnionBuffer* m_ppxliErrorQ ;

     
     U8_WMV * m_pZigzag[4] ;
 
     I32_WMV (*m_pDecodeInterError[4])(struct tagEMB_PBMainLoop  * pPMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
     U8_WMV m_uiIDCTClearOffset[4][4];


     U8_WMV m_iBlkXformTypeLUT[8];
     U8_WMV m_iSubblkPatternLUT[8] ;
    // U8_WMV m_SSIMD_DstLUT_1MV[6];
    // U8_WMV m_SSIMD_DstLUT_BK[6];
     U8_WMV  *m_pSSIMD_DstOffSet;
/*
     Void_WMV (*m_pInterpolateBlock[2][4][4]) (const U8_WMV *pSrc, I32_WMV iSrcStride, 
                                U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
*/

     I_SIMD (*m_pInterpolateBicubic[4][4]) (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, struct tagEMB_PBMainLoop  * pPBMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV * pTbl);

     Void_WMV (*m_pIDCTDec16_WMV3) (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
     Void_WMV (*m_pAddError) (U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch);
     Void_WMV (*m_pAddNull)(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch);
     Void_WMV (*m_pInterpolateBlockBilinear[4][4]) (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
     I32_WMV  (*m_pDecodeInterError8x8)(struct tagEMB_PBMainLoop* pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ);
     I_SIMD  (*m_pInterpolateBlock_00)(const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, struct tagEMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV,U16_WMV *pTbl);
     Void_WMV (*m_pSubBlkIDCTClear) (UnionBuffer * piDstBuf,  I32_WMV iIdx);

     U8_WMV m_iNumCoefTbl[8];
     U8_WMV m_IDCTShortCutLUT[64];
     

     U32_WMV m_ui001f001fPiRndCtl;
     U32_WMV m_ui000f000fPiRndCtl;
     U32_WMV m_ui00070007PiRndCtl;
     U32_WMV m_ui00030003PiRndCtl;
     U32_WMV m_ui00200020MiRndCtl;
     U32_WMV m_ui00400040MiRndCtl;
     U32_WMV m_ui00080008MiRndCtl;
     U32_WMV m_uiffff07ff;
     U32_WMV m_uiffff7fff;
     U32_WMV m_uiffff1fff;
     U32_WMV m_uic000c000;
    // U32_WMV m_ui80008000;
     U32_WMV m_uif000f000;
     U32_WMV m_ui00ff00ff;

     U16_WMV m_rgiNewVertTbl[2][2][4];
     U16_WMV m_rgiNewVertTbl_2[2][2][4]; //sw

}
EMB_PBMainLoop;

typedef struct tagEMB_PMainLoop
{
    EMB_PBMainLoop PB;

    U32_WMV* m_rgMotionCompBuffer_EMB ;
    I32_WMV (*m_rgfPredictHybridMV_EMB[3]) (struct tagEMB_PMainLoop  * pPMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk);
    I32_WMV (**m_ppPredictHybridMV_EMB) (struct tagEMB_PMainLoop  * pPMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk);

    PredictMV_LUT  m_PredictMVLUT;
    U8_WMV m_SSIMD_DstLUT_1MV[6];
    U8_WMV m_SSIMD_DstLUT_BK[6];
}
EMB_PMainLoop;

typedef struct tagEMB_BMainLoop
{
    EMB_PBMainLoop PB;

    I32_WMV m_iScaleFactor;
     
     U8_WMV  m_SSIMD_DstLUT_B[6];

     Void_WMV (*m_rgfPredictMV_EMB[3]) (struct tagEMB_BMainLoop  * pBMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk);
    Void_WMV (**m_ppPredictMV_EMB) (struct tagEMB_BMainLoop  * pBMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk);

   // PredictMV_LUT  m_PredictMVLUT;

    U8_WMV m_RndTbl[4];
    U8_WMV m_iMBTypeMask[4];
    I32_WMV m_iMVMin[3];
    I32_WMV m_iMVMax[3];
UMotion_EMB * m_pMotionCurr;
     UMotion_EMB  * m_pMotionArray[2];
#ifdef _EMB_SSIMD64_
	 U64_WMV spacer;	// Ensure that the motion compensation buffer is doubleword aligned.
     U32_WMV m_rgIntraMotionCompBuffer_EMB[96] ;
#else
     U32_WMV m_rgIntraMotionCompBuffer_EMB[80] ;
#endif
    U32_WMV* m_pMotionCompBuffer_EMB[2];
     I32_WMV m_iDirectCodingMode;
    MBType  m_mbtShortBSymbol;
    MBType  m_mbtLongBSymbol;
    Bool_WMV m_bHalfPelBiLinear;


}
EMB_BMainLoop;




#define FORWARD_EMB 0
#define BACKWARD_EMB 1

#define Y_EMB  0
#define U_EMB 1
#define V_EMB 2

// ===========================================================================
// private data members
// ===========================================================================

#ifdef XDM
typedef struct tagMemOperatorXDM
{
    U8_WMV*  pPrivateBuff;
    U8_WMV*  pFrameBuff;
    U8_WMV*  pPrivateBuffBack;
    U8_WMV*  pFrameBuffBack;
}MemOpteratorXDM;
#endif

typedef enum
{
    NONE_MEM_OP   = 0,   
    NORMAL_MEM_OP = 1,   
    VIDEO_MEM_SHARE_OP = 2,   
}VOMEMORYOPERATOR_TYPE;

typedef struct tagMemOperatorInternal
{
	VOMEMORYOPERATOR_TYPE	Type;
	void* (VO_API *AllocDD) (int nWidth, int nHeight, VO_VIDEO_BUFFER * pVideoInfo);
	void* (VO_API *Alloc) (int size, U32_WMV* pPhysAddr);
	void  (VO_API *Free)  (void* buffer);
	void  (VO_API *Lock)  (void* buffer);
	void  (VO_API *Unlock) (void* buffer);
}MemOpteratorInternal;


#ifdef VIDEO_MEM_SHARE

#define FIFO_WRITE	0
#define FIFO_READ	1 
#define MAXFRAMES	6
#define FIFO_NUM	(MAXFRAMES+1)

typedef struct {
  tYUV420Frame_WMV *img_seq[FIFO_NUM];			/* Circular buffer */
  tYUV420Frame_WMV *img_seq_backup[FIFO_NUM];     /* backup buffer for free*/
  U32_WMV w_idx;									/* Index of write */
  U32_WMV r_idx;								/* Index of read */
}FIFOTYPE;
#endif

struct tagWMVDecInternalMember;
typedef struct tagWMVDecInternalMember 
{
    // Don't add any thing before this variable! It has been used by ARM assembly code
#if defined(OUTPUT_ALL_FORMATS) || defined(OUTPUT_RGB12) || defined(OUTPUT_GRAY4) || defined(OUTPUT_RGB16)
    U8_WMV* m_rgiClapTab4BitDec;
    I32_WMV* m_rgiDithering;
    I32_WMV m_iDitheringIndex;
    U8_WMV* m_rgiClapTab565BitDec;
    I32_WMV m_iXscalePrefechDistY;
    I32_WMV m_iXscalePrefechDistUV;
    I32_WMV m_iXscalePrefechMBDistY;
    I32_WMV m_iXscalePrefechMBDistUV; 
#   ifdef _ARM_
//#       ifdef _WMMX_ //wshao 2008.12.19 . Should delete this marco,or doesn't support WVP2.
        U64_WMV m_uiCC16WMMX0;
        U64_WMV m_uiCC16WMMX1;             
        U64_WMV m_uiCC16WMMX2;
        U64_WMV m_uiCC16WMMX3;
        U64_WMV m_uiCC16WMMX4;           
        U64_WMV m_uiCC16WMMX5;
        U64_WMV m_uiCC16WMMX6;
//#       endif
#   endif
#endif

    U8_WMV* m_ppxlPHYSADDR;
    U8_WMV* m_ppxlCurrPHYSADDR;
    Bool_WMV m_bPHYSMEM;
    Bool_WMV m_bFrameInPHYSMEM;

#ifdef WMV_OPT_SPRITE
    I32_WMV iYLengthX, iUVLengthX;
    I32_WMV power, power_1, factor_rx_ry, rx, ry, rx_2, ry_2, rxXry, rxry, lrxry, iFading, oxRef, mask, lA, lAx2, factor;
#endif
    I32_WMV m_iOldWidth  ;
    
    // sequence layer info
    CInputBitStream_WMV *m_pInputBitstream;
    own CInputBitStream_WMV* m_pbitstrmIn;

    // size info
    I32_WMV m_iWidthSource; // source width, might not be MB aligned
    I32_WMV m_iHeightSource; // source height, might not be MB aligned
    I32_WMV m_iWidthInternal; // internal width, MB aligned
    I32_WMV m_iWidthInternalTimesMB;
    I32_WMV m_iHeightInternal;
    I32_WMV m_iWidthInternalUV;
    I32_WMV m_iWidthInternalUVTimesBlk;
    I32_WMV m_iHeightInternalUV;
    Bool_WMV m_bSizeMBAligned;
    U32_WMV m_uiNumMB, m_uiNumMBX, m_uiNumMBY;
    U32_WMV m_uintNumMBX, m_uintNumMBY, m_uintNumMB, m_uiRightestMB;
    Bool_WMV m_bMBAligned;
    I32_WMV m_iFrmWidthSrc, m_iFrmHeightSrc; // original, non-MB-aligned size of input bitmap
    I32_WMV m_iWidthYRepeatPad, m_iWidthUVRepeatPad; // equal original for MP43, else = m_iWidthY
    I32_WMV m_iHeightYRepeatPad, m_iHeightUVRepeatPad; // equal original for MP43, else = m_iHeightY
    I32_WMV m_iWidthY, m_iWidthYPlusExp, m_iHeightY;
    I32_WMV m_iWidthUV, m_iWidthUVPlusExp, m_iHeightUV;
    I32_WMV m_iWidthPrevY, m_iWidthPrevUV;
    I32_WMV m_iHeightPrevY, m_iHeightPrevUV;
    I32_WMV m_iWidthPrevYXExpPlusExp, m_iWidthPrevUVXExpPlusExp;
    I32_WMV m_iMBSizeXWidthPrevY; // MB size (16) * width of Y-Plane, for accessing MB data
    I32_WMV m_iBlkSizeXWidthPrevUV;
    I32_WMV m_iWidthPrevYxBlkMinusBlk;
    I32_WMV m_iMVLeftBound, m_iMVRightBound;
    I32_WMV m_iStepSize;
    Bool_WMV m_bHalfStep;

    // tables
    const U8_WMV* m_rgiClapTabDec;
    U8_WMV * m_pcClapTable;

    // picture layer
    CVector* m_rgmv;                    // 2 MB row's MV
    CWMVMBMode* m_rgmbmd;               // 2 MB row's MV
    CWMVMBMode* m_pmbmdZeroCBPCY;       // Pointer to the ZeroCBPCY
    CWMVMBMode* m_rgmbmd1;
    U8_WMV* m_rgchSkipPrevFrame;            // Prev frame's Skip bits
    Bool_WMV m_bDQuantOn; // frame level bit indicating DQuant
#if 1
	unsigned int m_nframes; //zou
	unsigned int m_nIframes;
	unsigned int m_nBframes;
	unsigned int m_nBIframes;
	unsigned int m_nPframes;
	unsigned int m_ncols;
	unsigned int m_nrows;
#endif
    tFrameType_WMV m_tFrmType;
    tFrameType_WMV m_tFrmPredType;
    tFrameType_WMV m_tFrmTypePriorFrame;
    I32_WMV m_iDCStepSize, m_iDCStepSizeC;
    I32_WMV m_i2DoublePlusStepSize, m_i2DoublePlusStepSizeNeg;
    I32_WMV m_iDoubleStepSize, m_iStepMinusStepIsEven;
    Bool_WMV m_bStepSizeIsEven;

    // MB layer info
    Bool_WMV m_bMBHybridMV;         // Indicates whether MB-based HybridMV is on
    Bool_WMV m_bMBXformSwitching;   // Indicates whether MB-based transform switching is chosen for the frame
    I32_WMV m_iMixedPelMV;          // 0 -> True MV, 1 -> Mixed
    I32_WMV m_iFrameXformMode;      // Transform mode (8x8, 8x4 or 4x8) for entire frame
    I32_WMV m_iOffsetToTopMB;       // m_uintNumMBX or -m_uintNumMBX to cycle 2 rows of MB data
    I32_WMV  m_iSkipbitCodingMode, m_iMVSwitchCodingMode;
    CDiffMV *m_pDiffMV;
#ifdef _EMB_WMV3_
    CDiffMV_EMB * m_pDiffMV_EMB;
     I16_WMV * m_rgiMVDecLUT_EMB;
     UMotion_EMB * m_pMotion;
     UMotion_EMB * m_pMotionC;
#endif
    Bool_WMV m_bDCTTable_MB, m_bDCTTable_MB_ENABLED, m_bDCPred_IMBInPFrame;
    I32_WMV  m_iMVRangeIndex, m_iLogXRange, m_iLogYRange, m_iXMVRange, m_iYMVRange, m_iXMVFlag, m_iYMVFlag;
    Bool_WMV m_bSKIPBIT_CODING_;
    Bool_WMV m_bXformSwitch;
    Bool_WMV m_bNEW_PCBPCY_TABLE;
    Bool_WMV m_bFrmHybridMVOn;
    Bool_WMV m_bCODFlagOn;          // COD flag is on (coded) for a P frame
    I32_WMV m_iMvResolution;        // Indicate current frame's MV resolution
    I32_WMV  m_iFilterType; // 2/4/6 tap filter choice
    I16_WMV  *m_pX9dct, *m_pX9dctU, *m_pX9dctV;
    Bool_WMV m_bDQuantBiLevel; // frame level bit indicating bilevel DQ or not

    // Block layer buffers

#ifdef _WMV9AP_
    U8_WMV m_riReconBuf[64*8+32];   // IDCT coeff recon buffer. Allow enough space for 32-byte alignment
    U8_WMV m_riPixelError[64*8+32]; // Pixel error buffer. Allow enough space for 32-byte alignment
#else
    U8_WMV* m_riReconBuf;       // IDCT coeff recon buffer. Allow enough space for 32-byte alignment  where 544==64*8+32 (incgen does not like this const expression)
    U8_WMV* m_riPixelError;     // Pixel error buffer. Allow enough space for 32-byte alignment
#endif
    I32_WMV* m_rgiCoefReconPlus1;
    I32_WMV* m_rgiCoefRecon;   
    UnionBuffer* m_ppxliErrorQ;
    UnionBuffer* m_rgiCoefReconBuf;
    I16_WMV  *m_pXMotion, *m_pYMotion, *m_pXMotionC, *m_pYMotionC;
    Bool_WMV m_b16bitXform;     // bilinear 16 bit transform flag
    Bool_WMV m_bUVHpelBilinear; // UV speedup
 
    // zigzag scan
    Bool_WMV m_bRotatedIdct;
    U8_WMV* m_pZigzagInv_I;
    U8_WMV* m_pHorizontalZigzagInv;
    U8_WMV* m_pVerticalZigzagInv;
    U8_WMV* m_pZigzagInvRotated_I;
    U8_WMV* m_pHorizontalZigzagInvRotated;
    U8_WMV* m_pVerticalZigzagInvRotated;
    U8_WMV* m_pZigzagInvRotated;
    U8_WMV* m_pZigzagInv;
    U8_WMV* m_pZigzagScanOrder;
    U8_WMV* m_p8x4ZigzagInv;
    U8_WMV* m_p4x8ZigzagInv;
    U8_WMV* m_p8x4ZigzagInvRotated;
    U8_WMV* m_p4x8ZigzagInvRotated;
    U8_WMV* m_p8x4ZigzagScanOrder;
    U8_WMV* m_p4x8ZigzagScanOrder;
    U8_WMV* m_p4x4ZigzagScanOrder;
    U8_WMV* m_p4x4ZigzagInv;
    U8_WMV* m_p4x4ZigzagInvRotated;
#ifdef _EMB_SSIMD_IDCT_
    U8_WMV * m_pZigzagSSIMD;
    U8_WMV * m_p8x4ZigzagSSIMD;
    U8_WMV * m_p4x8ZigzagSSIMD;
    U8_WMV * m_p4x4ZigzagSSIMD;
#endif
    U32_WMV* m_rgMotionCompBuffer_EMB;
    UnionBuffer* m_rgErrorBlock;          //This needs to be cache block alligned 
 
    // for DCT AC pred. 
    I32_WMV m_iNumOfQuanDctCoefForACPredPerRow;
    I16_WMV* m_rgiQuanCoefACPred;
    I16_WMV** m_rgiQuanCoefACPredTable;
    I16_WMV* m_pAvgQuanDctCoefDec;
    I16_WMV* m_pAvgQuanDctCoefDecC;
    I16_WMV* m_pAvgQuanDctCoefDecLeft;
    I16_WMV* m_pAvgQuanDctCoefDecTop;
    
    // DCT-DCAC prediction
    I32_WMV m_iACColPredOffset, m_iACRowPredOffset;
    I32_WMV m_iACColPredShift, m_iACRowPredShift;
    
    // Misc
    U32_WMV m_iEscRunDiffV2V3; // Correct EscRunCoding error in MP42
    I32_WMV m_iDCPredCorrect;   // Correct DC Pred error in MP42
    I32_WMV m_iDCTHorzFlags;    // Sparse IDCT flag
    Bool_WMV m_bFirstEscCodeInFrame;                    //WMV1 vlc EscCoding
    I32_WMV m_iNUMBITS_ESC_LEVEL, m_iNUMBITS_ESC_RUN;   //WMV1 vlc EscCoding
    I32_WMV sm_iIDCTDecCount;    // idct resource counter
    
    // for X8 : nonflat quantization
    I32_WMV  m_iNonflatQuant;
    t_CSpatialPredictor  *m_pSp;
    t_CContextWMV        *m_pContext;
    t_CAltTablesDecoder  *m_pAltTables;
    
    // Huffman_WMV Tables
    U8_WMV* m_puXMvFromIndex;
    U8_WMV* m_puYMvFromIndex;
    Huffman_WMV* m_pHufMVDec;
    Huffman_WMV m_hufMVDec_Talking;
    Huffman_WMV m_hufMVDec_HghMt;
    
    Huffman_WMV* m_pHufMVDec_Set[2];
    U8_WMV* m_puMvFromIndex_Set[4];
    U32_WMV m_iMVTable;
        
    // IntraDC
    //def _DCTDC
    Huffman_WMV m_hufDCTDCyDec_Talking;
    Huffman_WMV m_hufDCTDCcDec_Talking;
    Huffman_WMV m_hufDCTDCyDec_HghMt;
    Huffman_WMV m_hufDCTDCcDec_HghMt;
    
    U32_WMV m_iIntraDCTDCTable;
    Huffman_WMV* m_pHufDCTDCyDec;
    Huffman_WMV* m_pHufDCTDCcDec;
    Huffman_WMV* m_pHufDCTDCDec_Set[4];
    
    //NEW_PCBPCY_TABLE
    Huffman_WMV m_hufICBPCYDec, m_hufPCBPCYDec;
    Huffman_WMV* m_pHufNewPCBPCYDec;
    Huffman_WMV m_hufPCBPCYDec_HighRate; 
    Huffman_WMV m_hufPCBPCYDec_MidRate;
    Huffman_WMV m_hufPCBPCYDec_LowRate;
    //_DCTAC
    // For Table switching
    Huffman_WMV m_hufDCTACInterDec_HghMt;
    Huffman_WMV m_hufDCTACIntraDec_HghMt;
    Huffman_WMV m_hufDCTACInterDec_Talking;
    Huffman_WMV m_hufDCTACIntraDec_Talking;
    Huffman_WMV m_hufDCTACInterDec_MPEG4;
    Huffman_WMV m_hufDCTACIntraDec_MPEG4;
    Huffman_WMV m_hufDCTACIntraDec_HghRate;
    
    //Zone_VLC
    U32_WMV m_rgiEndIndexOfCurrZone[3];
    Bool_WMV m_bInterZoneVLCTable;
    Huffman_WMV m_hufPCBPCYDec_V9_1;
    Huffman_WMV m_hufPCBPCYDec_V9_2;
    Huffman_WMV m_hufPCBPCYDec_V9_3;
    Huffman_WMV m_hufPCBPCYDec_V9_4;
    Huffman_WMV m_HufMVTable_V9_MV1;
    Huffman_WMV m_HufMVTable_V9_MV2;
    Huffman_WMV m_HufMVTable_V9_MV3;
    Huffman_WMV m_HufMVTable_V9_MV4;
    Huffman_WMV* m_pHufMVTable_V9;
    Huffman_WMV* m_pHufPCBPCYTables[4];
    Huffman_WMV* m_pHufMVTables[8];
    Huffman_WMV m_hufDCTACInterDec_HghRate;

    // 4x4 DCT subblock pattern
    Huffman_WMV m_huf4x4PatternHighQPDec;
    Huffman_WMV m_huf4x4PatternMidQPDec;
    Huffman_WMV m_huf4x4PatternLowQPDec;
    Huffman_WMV* m_pHuf4x4PatternDec;
    Huffman_WMV m_hufMBXformTypeLowQPDec;
    Huffman_WMV m_hufMBXformTypeMidQPDec;
    Huffman_WMV m_hufMBXformTypeHighQPDec;
    Huffman_WMV* m_pHufMBXformTypeDec;
    Huffman_WMV m_hufBlkXformTypeLowQPDec;
    Huffman_WMV m_hufBlkXformTypeMidQPDec;
    Huffman_WMV m_hufBlkXformTypeHighQPDec;
    Huffman_WMV* m_pHufBlkXformTypeDec;

    CDCTTableInfo_Dec InterDCTTableInfo_Dec_HghMt;
    CDCTTableInfo_Dec IntraDCTTableInfo_Dec_HghMt;
    CDCTTableInfo_Dec InterDCTTableInfo_Dec_Talking;
    CDCTTableInfo_Dec IntraDCTTableInfo_Dec_Talking;
    CDCTTableInfo_Dec InterDCTTableInfo_Dec_MPEG4;
    CDCTTableInfo_Dec IntraDCTTableInfo_Dec_MPEG4;
    CDCTTableInfo_Dec IntraDCTTableInfo_Dec_HghRate;
    CDCTTableInfo_Dec InterDCTTableInfo_Dec_HghRate;

    CDCTTableInfo_Dec** m_ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** m_ppIntraDCTTableInfo_Dec;
    CDCTTableInfo_Dec* m_pInterDCTTableInfo_Dec_Set[3];
    CDCTTableInfo_Dec* m_pIntraDCTTableInfo_Dec_Set[3];
    
    U32_WMV m_rgiDCTACInterTableIndx[3];
    U32_WMV m_rgiDCTACIntraTableIndx[3];

    // IMBInPFrame DC Pred
    I32_WMV (*m_pBlkAvgX8_MMX) (const U8_WMV* ppxlcCurrRecnMB, I32_WMV iWidthPrev, I32_WMV iStepSize);
    I32_WMV m_rgiDCRef [3]; // intra DC predictor

    // overlapped transform stuff
    Bool_WMV m_iOverlap, m_bSequenceOverlap;

	Bool_WMV vo_OverLapFilter;     //zou
    I16_WMV  *m_pIntraBlockRowData;
    I16_WMV  *m_pIntraBlockRow0[3];
    I16_WMV  *m_pIntraMBRowU0[2];
    I16_WMV  *m_pIntraMBRowV0[2];
    I16_WMV  *m_pIntraBlockRow1[4];
    I16_WMV  *m_pIntraMBRowU1[3];
    I16_WMV  *m_pIntraMBRowV1[3];
    CMotionVector_X9 * m_rgmv1;
    CMotionVector_X9_EMB * m_rgmv1_EMB;

#ifdef DYNAMIC_EDGEPAD
    tWMVEdgePad m_tEdgePad;
#endif
    
    //EMB_PMainLoop  m_EMB_PMainLoop;
    // Functions

    tWMVDecodeStatus (*m_pDecodeP_X9) (struct tagWMVDecInternalMember *pWMVDec);
    tWMVDecodeStatus (*m_pDecodeB) (struct tagWMVDecInternalMember *pWMVDec);


    tWMVDecodeStatus (*m_pDecodePMB) (
        struct tagWMVDecInternalMember *pWMVDec,
        CWMVMBMode*        pmbmd,
        U8_WMV* ppxlcCurrQYMB,
        U8_WMV* ppxlcCurrQUMB,
        U8_WMV* ppxlcCurrQVMB,
        CoordI xRefY, CoordI yRefY,
        CoordI xRefUV, CoordI yRefUV
        );
    tWMVDecodeStatus (*m_pDecodeIMBAcPred) (
        struct tagWMVDecInternalMember *pWMVDec,
        CWMVMBMode*            pmbmd, 
        U8_WMV*      ppxliTextureQMBY,
        U8_WMV*      ppxliTextureQMBU, 
        U8_WMV*      ppxliTextureQMBV, 
        I16_WMV*                piQuanCoefACPred, 
        I16_WMV**                piQuanCoefACPredTable, 
        Bool_WMV                bLeftMB, 
        Bool_WMV                bTopMB, 
        Bool_WMV                bLeftTopMB, 
        I32_WMV                  iMBX
        );

    // Decoder MB Header
    tWMVDecodeStatus (* m_pDecodeMBOverheadOfIVOP) (struct tagWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);
    tWMVDecodeStatus (* m_pDecodeMBOverheadOfPVOP) (struct tagWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd);
   
    // Motion comp
#if !defined(_EMB_WMV2_) || defined(_WMV_TARGET_X86_)
    // Motion Comp
    Void_WMV (*m_pMotionCompAndAddError) (MOTION_COMP_ADD_ERROR_ARGS);
    Void_WMV (*m_pMotionComp) (MOTION_COMP_ARGS);
    Void_WMV (*m_pMotionCompAndAddErrorRndCtrlOn) (MOTION_COMP_ADD_ERROR_ARGS);
    Void_WMV (*m_pMotionCompRndCtrlOn) (MOTION_COMP_ARGS);
    Void_WMV (*m_pMotionCompAndAddErrorRndCtrlOff) (MOTION_COMP_ADD_ERROR_ARGS);
    Void_WMV (*m_pMotionCompRndCtrlOff) (MOTION_COMP_ARGS);
    Void_WMV (*m_pMotionCompUV) (MOTION_COMP_ARGS);
    Void_WMV (*m_pMotionCompAndAddErrorUV) (MOTION_COMP_ADD_ERROR_ARGS);
    Void_WMV (*m_pMotionCompMixed) (MOTION_COMP_ARGS);
    Void_WMV (*m_pMotionCompMixedAndAddError) (MOTION_COMP_ADD_ERROR_ARGS);
#endif
    Void_WMV (*m_pMotionCompZero) (MOTION_COMP_ZERO_ARGS);
#ifdef _EMB_WMV2_
    U32_WMV m_iMotionCompRndCtrl;
    EMB_DecodeP_ShortCut m_EMBDecodePShortCut;
    tWMVDecodeStatus (*m_CoefDecoderAndIdctDecTable[4])(struct tagWMVDecInternalMember * pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec, I32_WMV iXFormMode);
    tWMVDecodeStatus (*m_pCoefDecoderAndIdctDec8x4or4x8Table[8])(struct tagWMVDecInternalMember * pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec);
    Void_WMV (*m_MotionCompAndAddErrorTable[16])(U8_WMV * ppxlcCurrQMB, const U8_WMV * ppxlcRef, I32_WMV iWidthFrm, I32_WMV * pErrorBuf);
#endif
    Void_WMV (*m_pInterpolateMBRow) (struct tagWMVDecInternalMember * pWMVDec,
        U8_WMV *pSrc, I32_WMV iSrcStride, 
        U8_WMV *pDst, I32_WMV iDstStride, 
        I32_WMV iXFrac, I32_WMV iYFrac,
        I32_WMV iFilterType, I32_WMV iRndCtrl,
        I32_WMV iNumRows);
    Void_WMV (*m_pCopyMBRow) (
        U8_WMV *pSrc, I32_WMV iSrcStride, 
        U8_WMV *pDst, I32_WMV iDstStride,
        I32_WMV iNumRows);
    Void_WMV (*m_pInterpolateBlockBicubic) (const U8_WMV *pSrc, I32_WMV iSrcStride, 
        U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl);
    Void_WMV (*m_pInterpolateBlockBilinear) (const U8_WMV *pSrc, I32_WMV iSrcStride, 
        U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl); 
    Void_WMV (*m_pAddError) (U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch, U8_WMV* pcClapTabMC);
    Void_WMV (*m_pAddErrorIntra) (U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch, U8_WMV* pcClapTabMC);

    //block decoding fuctions
    tWMVDecodeStatus (*m_pDecodeInverseInterBlockQuantize) (struct tagWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec** InterDCTTableInfo_Dec, U8_WMV* pZigzag, I32_WMV iXformMode,
        DQuantDecParam *pDQ
        );    
    // IDCT routines

    Void_WMV (*m_pIntraIDCT_Dec) (U8_WMV* piDst, I32_WMV iOffsetToNextRowForDCT, const I32_WMV __huge* rgiCoefRecon);
	Void_WMV (*m_pInterIDCT_Dec) (UnionBuffer* piDst, UnionBuffer* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
    Void_WMV (*m_pInter8x4IDCT_Dec) (UnionBuffer* piDst, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefRecon, I32_WMV iHalf);
    Void_WMV (*m_pInter4x8IDCT_Dec) (UnionBuffer* piDst, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefRecon, I32_WMV iHalf);
    Void_WMV (*m_pInter4x4IDCT_Dec) (UnionBuffer __huge* piDst, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefRecon, I32_WMV iQuadrant);
    Void_WMV (*m_pIntraX9IDCT_Dec) (UnionBuffer* piDst, UnionBuffer* piSrc, I32_WMV  iOffsetToNextRowForDCT, I32_WMV  iDCTHorzFlags);
      
    Void_WMV (*m_pHorizDownsampleFilter6)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
    Void_WMV (*m_pVertDownsampleFilter6)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
    Void_WMV (*m_pHorizUpsampleFilter10)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
    Void_WMV (*m_pVertUpsampleFilter10)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
    Void_WMV  (*m_pOverlapBlockVerticalEdge) (I16_WMV *pInOut, I32_WMV iStride);
    Void_WMV  (*m_pOverlapBlockHorizontalEdge) (I16_WMV *pSrcTop, I16_WMV *pSrcCurr,
        I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,
        Bool_WMV bTop, Bool_WMV bCurrent, Bool_WMV bWindup);
    Void_WMV (*m_pPixelMean) (const U8_WMV *pSrc1, I32_WMV iStride1, const U8_WMV *pSrc2, 
        I32_WMV iStride2, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iWidth, I32_WMV iHeight);
    

    
#if defined( _EMB_WMV3_)
    I32_WMV (*m_pInterpolateBlock[2][4][4]) (const U8_WMV *pSrc, I32_WMV iSrcStride, 
                                U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
#endif

    // Sequence layer data
    U32_WMV  m_uiUserData;
    Bool_WMV m_bSafeMode;
    Bool_WMV m_bStrongARM;
    U32_WMV  m_uiNumProcessors;
    Bool_WMV m_bShutdownThreads;
    I32_WMV  m_iBetaRTMMismatchIndex;    // Used by V9 loopfilter for RTM/Beta mismatch. 
    Bool_WMV m_bBetaContent;// Flags to indicate encoder used to generate V9 content
    Bool_WMV m_bRTMContent;
    I32_WMV  m_iBState;
    I32_WMV  m_iPrevIFrame;
    I32_WMV  m_iDroppedPFrame;
    I32_WMV  m_iBDenominator;
    I32_WMV  m_iBNumerator;
    Bool_WMV m_bPrevAnchor;
    Bool_WMV m_bIsBChangedToI;
    Bool_WMV m_bOldRenderFromPostBuffer;
    Bool_WMV m_bSkipThisFrame;  // For B frames to handle seeking in the player
    Bool_WMV m_bPrevRefInPrevPost;
    Bool_WMV m_bFrameWasOutput;
    I32_WMV  m_iNumFrameDropped;    
    Bool_WMV m_bRenderDirect;
    Bool_WMV m_bUse3QPDZQuantizer;
    Bool_WMV m_bExplicitQuantizer;
    Bool_WMV m_bExplicitSeqQuantizer;
    Bool_WMV m_bExplicitFrameQuantizer;
    Bool_WMV m_bSeqFrameInterpolation, m_bInterpolateCurrentFrame;
    Bool_WMV m_bOmittedBFrame;
    
    // m_hAsyncProcess & m_hSyncEvent are only needed for the Asynchronous extension to the WMVideoDec INterface.
    // state for Asynchronous interface
    Handle_WMV m_hAsyncProcess;             // some sort of handle on the process that does the async decoding
    Handle_WMV m_hSyncEvent;                // holds a handle to an event to be called on each state transistion
    tWMVDecodeStatus m_tWMVDecodeStatus;    // Decode status from last async function
    
    // SILS - most of this thread stuff should be cleaned out - it confuses incgen
    // for multi-thread handles
    HANDLE hDecEvent0, hDecDone0;
    HANDLE hDecEvent1, hDecDone1;
    HANDLE hDecEvent2, hDecDone2;
    HANDLE hDecEvent3, hDecDone3;
    HANDLE threadDec0, threadDec1, threadDec2, threadDec3;
    THREADTYPE_DEC m_iMultiThreadType;
#ifdef _WMV_TARGET_X86_
    HANDLE   hHorizontalLpFltrDone[4];
    HANDLE   hReconComplete;
#endif
    // Multithread event for clearing frame
    HANDLE   hX8Clear, hX8MB[4];
    Bool_WMV     m_bWaitingForEvent[4];

    // MPEG4-compliant variables
    U32_WMV uiFCode;
    I32_WMV iRange;
    I32_WMV iScaleFactor;
    I32_WMV m_iClockRate;
    Time m_t; // current time
    I32_WMV m_iVPMBnum;
    Time m_tModuloBaseDecd;
    Time m_tModuloBaseDisp;
    Time m_tOldModuloBaseDecd;
    Time m_tOldModuloBaseDisp;
    I32_WMV m_iNumBitsTimeIncr;
    Bool_WMV m_bResyncDisable;
    // For MPG4

    U32_WMV m_uiFOURCCCompressed;
    I32_WMV m_iPostFilterLevel;
    Float_WMV m_fltFrameRate;
    Float_WMV m_fltBitRate;
    I32_WMV m_iFrameRate;
    I32_WMV m_iBitRate; // in Kbps
    
    //VOLMode m_volmd; // vol mode
    //VOPMode m_vopmd; // vop mode
    Bool_WMV m_fPrepared;
    Bool_WMV m_fDecodedI;
    
    U16_WMV m_uiNumFramesLeftForOutput;

    // pictures, frame's data
	//tYUV420Frame_WMV* m_pfrmLast;

	U8_WMV *lastY;
	U8_WMV *lastU;
	U8_WMV *lastV;


    tYUV420Frame_WMV* m_pfrmCurrQ;
    tYUV420Frame_WMV* m_pfrmRef1Q;
    tYUV420Frame_WMV* m_pfrmRef0Q;
    tYUV420Frame_WMV* m_pfrmMultiresQ;
    tYUV420Frame_WMV* m_pfrmPostQ;
    tYUV420Frame_WMV* m_pfrmPostPrevQ;
    tYUV420Frame_WMV *m_pvopfRef1;
    tYUV420Frame_WMV* m_pvopcRenderThis; // render this frame
    tYUV420Frame_WMV* m_pfrmYUV411toYUV420Q;//When output is YUV411,we need convert them to YUV420 which will be sent to APP.

    U8_WMV* m_ppxliCurrQY;
    U8_WMV* m_ppxliCurrQU;
    U8_WMV* m_ppxliCurrQV;
    U8_WMV* m_ppxliRef0Y;
    U8_WMV* m_ppxliRef0U;
    U8_WMV* m_ppxliRef0V;
    U8_WMV* m_ppxliMultiresY;
    U8_WMV* m_ppxliMultiresU;
    U8_WMV* m_ppxliMultiresV;
    U8_WMV* m_ppxliRef0YPlusExp;
    U8_WMV* m_ppxliRef1Y;
    U8_WMV* m_ppxliRef1U;
    U8_WMV* m_ppxliRef1V;
    U8_WMV* m_ppxliRef1YPlusExp;
    U8_WMV* m_ppxliPostQY;
    U8_WMV* m_ppxliPostQU;
    U8_WMV* m_ppxliPostQV;
    U8_WMV* m_ppxliPostPrevQY;
    U8_WMV* m_ppxliPostPrevQU;
    U8_WMV* m_ppxliPostPrevQV;
    U8_WMV* m_ppxliCurrQPlusExpY0;
    U8_WMV* m_ppxliCurrQPlusExpU0;
    U8_WMV* m_ppxliCurrQPlusExpV0;
  
	U8_WMV* vo_ppxliRefYPlusExp;//zou
	U8_WMV* vo_ppxliRefU;
	U8_WMV* vo_ppxliRefV;
  
    U32_WMV m_uiMBEnd0, m_uiMBEndX0; 
    I32_WMV m_iStartY0, m_iEndY0;
    I32_WMV m_iStartUV0, m_iEndUV0;
    I32_WMV m_iOffsetPrevY0, m_iOffsetPrevUV0;
    U32_WMV m_uiMBStart1, m_uiMBEnd1, m_uiMBEndX1; 
    I32_WMV m_iStartY1, m_iEndY1;
    I32_WMV m_iStartUV1, m_iEndUV1;
    I32_WMV m_iOffsetPrevY1, m_iOffsetPrevUV1;
    
    // sequence layer info
    Bool_WMV m_bIFrameDecoded;
    Bool_WMV m_bXintra8Switch;
    Bool_WMV m_bMixedPel;
    Bool_WMV m_bLoopFilter;
    Bool_WMV m_bNeedLoopFilterOnCurrFrm;
    Bool_WMV m_bRndCtrlOn;
    I32_WMV  m_iRndCtrl;
    I32_WMV  m_iWMV3Profile;
    Bool_WMV m_bV9LoopFilter;
    LOOPF_FLAG* m_pLoopFilterFlags;
    Bool_WMV m_bYUV411;
    Bool_WMV m_bBroadcastMode;
    Bool_WMV m_bStartCode;
    
    // picture layer info.
    SKIPBITCODINGMODE m_SkipBitCodingMode;
    Bool_WMV m_bXintra8;
    I32_WMV  m_iQPIndex;
    Bool_WMV m_bX9;
    I32_WMV  m_iX9MVMode;
    Bool_WMV m_bLuminanceWarp;
    I32_WMV  m_iLuminanceScale, m_iLuminanceShift;
    I32_WMV  m_iPanning; // image moving bot<<3 | right<<2 | top <<1 | left
  
    // Block layer
    Bool_WMV m_iDQuantCodingOn; // seq. level bit indicating possibility of DQuant at frame level
    I32_WMV m_iDQuantBiLevelStepSize; // value of the bilevel DQ
    DQuantDecParam m_rgDQuantParam3QPDeadzone [64];
    DQuantDecParam m_rgDQuantParam5QPDeadzone [64];
    DQuantDecParam *m_prgDQuantParam;
    Bool_WMV m_bNewDCQuant;     //indicates whether improved DC quantization is used at low QP
    
    // B frame
    U8_WMV* m_ppxliRefYPreproc;
    U8_WMV* m_ppxliRefUPreproc;
    U8_WMV* m_ppxliRefVPreproc;
    I32_WMV  m_iNumBFrames;
    I32_WMV  m_iBFrameReciprocal;
    MBType  m_mbtShortBSymbol, m_mbtLongBSymbol;
    Bool_WMV m_bBFrameOn;   // global flag to indicate B frame coding mode
    Bool_WMV m_bPreProcRange;
    I32_WMV m_iRangeState;
    I32_WMV m_iReconRangeState;
    I32_WMV m_iDirectCodingMode;
    CVector_X9* m_rgmv_X9;
    CVector_X9* m_rgmvForwardPred;
    CVector_X9* m_rgmvBackwardPred;
 
    // for multires coding //wshao:Multiresolution coding
    Bool_WMV m_bMultiresEnabled;
    I32_WMV m_iResIndex;
    I32_WMV m_iResIndexPrev;
    MULTIRES_PARAMS m_rMultiResParams[4];
    I32_WMV m_iCurrQYOffset1, m_iCurrQUVOffset1;
    I32_WMV m_iWidthPrevYTimes8Minus8, m_iWidthPrevUVTimes4Minus4; 
    I8_WMV *m_pMultiresBuffer;
    I8_WMV *m_pAlignedMultiresBuffer;

    // _REFLIBRARY_
    Bool_WMV m_bRefLibEnabled;
    I32_WMV m_iRefFrameNum;
    I32_WMV m_iNewRefFlag;
    CReferenceLibrary *m_cReferenceLibrary;

    //INTERLACE 
    //Bool_WMV m_bInterlaceCodingOn; // global flag to indicate interlace coding mode
    //Bool_WMV m_bFieldCodingOn; // frame level flag to indicate field motest
    I16_WMV *m_pFieldMvX, *m_pFieldMvY; // Field MV
    I16_WMV *m_ppxliFieldMBBuffer; // Unaligned Buffer
    I16_WMV *m_ppxliFieldMB; // Aligned Buffer
    I32_WMV m_iStartDeinterlace1;
    I32_WMV m_iStartDeinterlace2;
    I32_WMV m_iStartDeinterlace3;
    Bool_WMV m_bHostDeinterlace;
    
    U8_WMV *m_pInterpY0Buffer;
    U8_WMV *m_pInterpY0;        // Aligned interpolation buffers
    U8_WMV *m_pInterpY1;
    U8_WMV *m_pInterpU0;
    U8_WMV *m_pInterpU1;
    U8_WMV *m_pInterpV0;
    U8_WMV *m_pInterpV1;
    
    I16_WMV *m_pFieldMvX_Pbuf, *m_pFieldMvY_Pbuf;
    I16_WMV *m_pFieldMvX_FPred, *m_pFieldMvY_FPred;
    I16_WMV *m_pFieldMvX_BPred, *m_pFieldMvY_BPred;
    I16_WMV *m_pB411InterpX, *m_pB411InterpY;
    
    // Sprite mode //wshao:That is picture sequence coding(such as WMVP).It is similar with slideshow.
    I32_WMV m_bSpriteMode;
    I32_WMV m_iNumMotionVectorSets;
    I32_WMV m_iSpriteWidthDsiplay    ; 
    I32_WMV m_iSpriteHeightDisplay   ;
    I32_WMV m_iSpriteWidthDsiplayMB  ;
    I32_WMV m_iSpriteHeightDisplayMB ;
    I32_WMV m_iOldHeight ;
    I32_WMV m_iNewWidth  ;
    I32_WMV m_iNewHeight ;
    I32_WMV m_uiNumSlices;
    const unsigned char *m_dibBitsY;
    const unsigned char *m_dibBitsU;
    const unsigned char *m_dibBitsV;
    unsigned char *m_pFrameY;
    unsigned char *m_pFrameU;
    unsigned char *m_pFrameV;
    I32_WMV m_iAffinePattern;

#ifdef WMVIMAGE_V2
    I32_WMV m_bSpriteModeV2;

    WMVIMAGE_EFFECTS    *m_pEffects;
    I32_WMV             m_iEffectType;
    I32_WMV             m_iNumAffineCoefficients;
    Float_WMV           *m_pAffineCoefficients;
    I32_WMV             m_iNumEffectParameters;
    Float_WMV           *m_pEffectParameters;
    Bool_WMV            m_bKeepPrevImage;
    I32_WMV             m_iAffinePattern1, m_iAffinePattern2;

    Float_WMV m_fltA1, m_fltB1, m_fltC1, m_fltD1, m_fltE1, m_fltF1, m_fltFading1;
    Float_WMV m_fltA2, m_fltB2, m_fltC2, m_fltD2, m_fltE2, m_fltF2, m_fltFading2;
#else

#ifdef WMV_OPT_SPRITE
    Float_WMV m_fltA1, m_fltB1, m_fltC1, m_fltD1, m_fltE1, m_fltF1, m_fltFading1;
    Float_WMV m_fltA2, m_fltB2, m_fltC2, m_fltD2, m_fltE2, m_fltF2, m_fltFading2;
#endif

#endif

    //    Bool_WMV m_iSliceCode;
    I32_WMV m_iSliceCode;
    U32_WMV m_uintNumMBYSlice; // slice size (number of MB's vertically)
    CodecVersion m_cvCodecVersion;
    Bool_WMV m_bMainProfileOn;
    Bool_WMV m_bMMXSupport;

    // output BMP info
    Bool_WMV m_bBMPInitialized;
    U32_WMV m_uiFOURCCOutput;
    U16_WMV m_uiBitsPerPixelOutput;
    U32_WMV m_uiWidthBMPOutput;
    Bool_WMV m_bRefreshBMP;
    tWMVDecodeDispRotateDegree m_iDispRotateDegree;
    Bool_WMV m_bDeblockOn;
    Bool_WMV m_bDeringOn; // deblock must be on for dering to be on
    I32_WMV m_bUseOldSetting;
    Bool_WMV m_bRefreshDisplay_AllMB_Enable;
    Bool_WMV m_bRefreshDisplay_AllMB;
    I32_WMV m_iRefreshDisplay_AllMB_Cnt;
    I32_WMV m_iRefreshDisplay_AllMB_Period;
    Bool_WMV m_bCopySkipMBToPostBuf;
    Bool_WMV m_bDefaultColorSetting;
    Bool_WMV m_bCPUQmoved;
    I32_WMV m_iPostProcessMode;
    I32_WMV m_eFrameWasOutput;  //    I32_WMV m_iHurryUp;
    Bool_WMV m_bDisplay_AllMB;
    Bool_WMV m_bRenderFromPostBuffer;
    Bool_WMV m_bPlanarDstBMP;
    Bool_WMV m_bYUVDstBMP;
    I32_WMV m_iDstBMPOffsetU;
    I32_WMV m_iDstBMPOffsetV;
    U32_WMV m_uiRedscale, m_uiGreenscale;
    U32_WMV m_uiRedmask, m_uiGreenmask;
    I32_WMV* m_piYscale;
    I32_WMV* m_piVtoR;
    I32_WMV* m_piUtoG;
    I32_WMV* m_piVtoG;
    I32_WMV* m_piUtoB;
    I32_WMV m_iWidthBMP;
    I32_WMV m_iWidthBMPUV;
    I32_WMV m_iBMPPointerStart; 
    I32_WMV m_iBMPMBIncrement, m_iBMPBlkIncrement;
    I32_WMV m_iBMPMBIncrementUV;
    I32_WMV m_iBMPMBHeightIncrement, m_iBMPBlkHeightIncrement, m_iBMPMBHeightIncrementUV;
#ifdef _6BIT_COLORCONV_OPT_
    YUV2RGB_6BIT_TABLE *p_yuv2rgb_6bit_table;
    U8_WMV * m_rgiClapTab6BitDec;
#endif

    // Loopfilter
    U8_WMV *m_puchFilter8x8RowFlagY0, *m_puchFilter8x8RowFlagY1;
    U8_WMV *m_puchFilter8x4RowFlagY0, *m_puchFilter8x4RowFlagY1;
    U8_WMV *m_puchFilter8x8RowFlagU0, *m_puchFilter8x8RowFlagU1; 
    U8_WMV *m_puchFilter8x4RowFlagU0, *m_puchFilter8x4RowFlagU1; 
    U8_WMV *m_puchFilter8x8RowFlagV0, *m_puchFilter8x8RowFlagV1; 
    U8_WMV *m_puchFilter8x4RowFlagV0, *m_puchFilter8x4RowFlagV1; 
    U8_WMV *m_puchFilter8x8ColFlagY0, *m_puchFilter8x8ColFlagY1;
    U8_WMV *m_puchFilter4x8ColFlagY0, *m_puchFilter4x8ColFlagY1;
    U8_WMV *m_puchFilter8x8ColFlagU0, *m_puchFilter8x8ColFlagU1;
    U8_WMV *m_puchFilter4x8ColFlagU0, *m_puchFilter4x8ColFlagU1;
    U8_WMV *m_puchFilter8x8ColFlagV0, *m_puchFilter8x8ColFlagV1;
    U8_WMV *m_puchFilter4x8ColFlagV0, *m_puchFilter4x8ColFlagV1;
    
    // bitmap conversion
    U8_WMV* m_pBMPBits;

    // Functions

    tWMVDecodeStatus (*m_pDecodeI) (struct tagWMVDecInternalMember *pWMVDec);
    tWMVDecodeStatus (*m_pDecodeP) (struct tagWMVDecInternalMember *pWMVDec);
    
    // Multires
    Void_WMV (*m_pHorizDownsampleFilter9)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pHorizDownsampleFilter5)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pVertDownsampleFilter9)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pVertDownsampleFilter5)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pHorizUpsampleFilter7)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pHorizUpsampleFilter3)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pVertUpsampleFilter7)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pVertUpsampleFilter3)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pHorizDownsampleFilter)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pVertDownsampleFilter)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pHorizUpsampleFilter)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pVertUpsampleFilter)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
   
    // Interlaced
    tWMVDecodeStatus (*m_pDeinterlaceProc)(struct tagWMVDecInternalMember * pWMVDec, U8_WMV *pucInput, U8_WMV *pucOutput0, I32_WMV iFirstLine, I32_WMV iLastLine, I32_WMV iWidth, I32_WMV iHeight);
    Void_WMV (*m_pFieldBlockSub128) ( I16_WMV *pBlock, I32_WMV iNumCoef);
    Void_WMV (*m_pFieldBlockAdd128) ( I16_WMV *pBlock, I32_WMV iNumCoef);
    Void_WMV (*m_pFieldAddError8x8) (I16_WMV *pSrc, U8_WMV *pDst, I32_WMV iDstOffset);
    Void_WMV (*m_pFieldAddError4x8) (I16_WMV *pSrc, U8_WMV *pDst, I32_WMV iDstOffset);
    Void_WMV (*m_pInterpolate4ByteRow) (struct tagWMVDecInternalMember * pWMVDec,
    U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, 
    I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iFilterType, I32_WMV iRndCtrl, I32_WMV iNumRows);
 
    // Deblocking filter
    Void_WMV (*m_pDeblockInterlaceFrame) (struct tagWMVDecInternalMember *pWMVDec, U32_WMV iMBStartY, U32_WMV iMBEndY);
   Void_WMV (*m_pDeblockIFrame) (struct tagWMVDecInternalMember *pWMVDec, U8_WMV * ppxliY, U8_WMV * ppxliU, U8_WMV * ppxliV, U32_WMV iMBStartY, U32_WMV iMBEndY);
   Void_WMV (*m_pDeblockPFrame) (struct tagWMVDecInternalMember *pWMVDec, U8_WMV * ppxliY, U8_WMV * ppxliU, U8_WMV * ppxliV, U32_WMV iMBStartY, U32_WMV iMBEndY );
    Void_WMV (*m_pFilterHorizontalEdge)(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
    Void_WMV (*m_pFilterVerticalEdge)(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);

    // rendering function pointers
    Void_WMV (*m_pWMVideoDecUpdateDstMB) (
        struct tagWMVDecInternalMember *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidth
        );
    Void_WMV (*m_pWMVideoDecUpdateDstPartialMB) (
        struct tagWMVDecInternalMember *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iRgbWidth, I32_WMV iRgbHeight
        );
    Void_WMV (*m_pWMVideoDecUpdateDstBlk) (
        struct tagWMVDecInternalMember *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidth
        );
    
    Void_WMV (*m_pWMVideoDecUpdateDstYUVMB) (
        struct tagWMVDecInternalMember *pWMVDec,
        U8_WMV* pBitsY, U8_WMV* pBitsU, U8_WMV* pBitsV,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidthY, I32_WMV iBitmapWidthUV
        );
    Void_WMV (*m_pWMVideoDecUpdateDstPartialYUVMB) (
        struct tagWMVDecInternalMember *pWMVDec,
        U8_WMV* pBitsY, U8_WMV* pBitsU, U8_WMV* pBitsV,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidthY, I32_WMV iBitmapWidthUV,
        I32_WMV iValidWidthY, I32_WMV iValidHeightY
        );
    Void_WMV (*m_pWMVideoDecUpdateDstLine) (
        U8_WMV* pucBmp,
        const U8_WMV*  pucCurrY, const U8_WMV*  pucCurrU, const U8_WMV* pucCurrV,
        const I32_WMV iWidthOrigY
        );

    Void_WMV (*m_pWMVideoDecUpdateDstPlane) (
        U8_WMV __huge*            pBits,
        U8_WMV __huge* pucCurr,
        I32_WMV iLines, I32_WMV iWidthOrig,
        I32_WMV iWidthOrigInc,
        I32_WMV iBitmapWidth
        );
    
//#ifdef _EMB_3FRAMES_
    Bool_WMV m_bEMB3FrmEnabled;
     Bool_WMV m_bAltRefFrameEnabled;
     Bool_WMV m_bRef0InRefAlt;
     tYUV420Frame_WMV* m_pfrmRef0AltQ;
//#endif
    
#ifdef _EMB_WMV3_
    EMB_PMainLoop  m_EMB_PMainLoop;
    EMB_BMainLoop  m_EMB_BMainLoop;
#endif

    I32_WMV EXPANDY_REFVOP;
    I32_WMV EXPANDUV_REFVOP;

    U8_WMV  *m_pSequenceHeader;
    U32_WMV m_uipSequenceHeaderLength;

//#ifdef _EMB_3FRAMES_
    Bool_WMV m_bOutputOpEnabled;
    tYUV420Frame_WMV* m_pOutputTmpFrm;
    tYUV420Frame_WMV* m_pOutputTmpFrm2;
//#endif

#ifdef _SUPPORT_POST_FILTERS_
    // Info and functions for dynamically adapting the post processing level
    I32_WMV     m_bOK2AdaptPostLevel;
    I32_WMV     m_iAdaptSingleFrame;
    I32_WMV     m_iRegkeyAdapt;
    I32_WMV     m_iCountCPUCyclesRegKey;
    __int64     m_i64QPCDecode;
    __int64     m_i64QPCPost;
    __int64     m_i64QPCPostThisFrame;
    I32_WMV     m_bInTimeDecode;
    I32_WMV     m_bInTimePost;
    I32_WMV     m_iLast8QPCDecodes[8];
    I32_WMV     m_iSumLast8QPCDecodes;
    I32_WMV     m_idxLast8QPCDecodes;
    I32_WMV     m_iFramesSinceLastAdapt;
    I32_WMV     m_iMSPerFrame;
    I32_WMV     m_iCountFrames;
    I32_WMV     m_iCountSkippedFrames;
    I32_WMV     m_iQPC2DecodeAvgFrame;
    I32_WMV     m_iQPC2DecodeAvgFrame4Up;
    I32_WMV     m_iNeededQPCUnitsperFrame4Level[_TOP_POST_FILTER_LEVEL_+2];
    __int64     m_i64SumQPCUsed4Level[_TOP_POST_FILTER_LEVEL_+1];
    I32_WMV     m_iCountQPCUsed4Level[_TOP_POST_FILTER_LEVEL_+1];
    I32_WMV     m_iPostLevelPrior2DownAdapt;
    I32_WMV     m_iQPCPerMS;
    U32_WMV     m_dwTickCountWhenAdaptedDownPostLevel;
    U32_WMV     m_dwTickCountWhenLastUpdatedRegKey;
#endif // _SUPPORT_POST_FILTERS_

    U8_WMV   * m_ppxli2MBRowY[2];
    U8_WMV   * m_ppxli2MBRowU[2];
    U8_WMV   * m_ppxli2MBRowV[2];

    U8_WMV * m_ppxliMultires4MBRowY;
    U8_WMV * m_ppxliMultires4MBRowU;
    U8_WMV * m_ppxliMultires4MBRowV;


        Bool_WMV m_bInterlaceV2;
        Bool_WMV m_bFieldMode;
    I32_WMV m_iCurrentField;    
        
    I32_WMV m_iFieldDctIFrameCodingMode;
    
    I16_WMV* m_ppxliIntraRowY;
    I16_WMV* m_ppxliIntraRowU;
    I16_WMV* m_ppxliIntraRowV;


    I32_WMV m_iACPredIFrameCodingMode;

   
     Void_WMV (*m_pInterpolateBlockRowBilinear) (struct tagWMVDecInternalMember * pWMVDec,
        U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, 
        I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iFilterType, I32_WMV iRndCtrl, I32_WMV iNumRows);

    Void_WMV (*m_pInterpolate4ByteRowBilinear) (struct tagWMVDecInternalMember * pWMVDec,
        U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, 
        I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iFilterType, I32_WMV iRndCtrl, I32_WMV iNumRows);

    Void_WMV (*m_pInterpolateBlockRow) (struct tagWMVDecInternalMember * pWMVDec,
        U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, 
        I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iFilterType, I32_WMV iRndCtrl, I32_WMV iNumRows);

    Bool_WMV m_bLuminanceWarpTop, m_bLuminanceWarpBottom;
    I32_WMV  m_iLuminanceScaleTop, m_iLuminanceShiftTop;
    I32_WMV  m_iLuminanceScaleBottom, m_iLuminanceShiftBottom;

    U8_WMV *m_pHorizontalZigzagScanOrder;
    U8_WMV *m_pVerticalZigzagScanOrder;

    Bool_WMV  m_bAdvancedProfile;

        Bool_WMV m_bBFrameDropped;

        Huffman_WMV *m_pInterlaceFrameMBMode;

    Huffman_WMV *m_pInterlaceFrame4MvMBModeTables [4];
    Huffman_WMV m_hufInterlaceFrame4MvMBMode0;
    Huffman_WMV m_hufInterlaceFrame4MvMBMode1;
    Huffman_WMV m_hufInterlaceFrame4MvMBMode2;
    Huffman_WMV m_hufInterlaceFrame4MvMBMode3;

    Huffman_WMV *m_pInterlaceFrame1MvMBModeTables [4];
    Huffman_WMV m_hufInterlaceFrame1MvMBMode0;
    Huffman_WMV m_hufInterlaceFrame1MvMBMode1;
    Huffman_WMV m_hufInterlaceFrame1MvMBMode2;
    Huffman_WMV m_hufInterlaceFrame1MvMBMode3;


        // Field Picture
    Huffman_WMV* m_pHufMBMode;
    Huffman_WMV* m_pHufMBModeMixedMVTables[8];
    Huffman_WMV* m_pHufMBMode1MVTables[8];

    Huffman_WMV* m_p4MVBP;
    Huffman_WMV*m_p4MVBPTables[4];

    Huffman_WMV* m_p2MVBP;
    Huffman_WMV*m_p2MVBPTables[4];

    Huffman_WMV m_hufCBPCYDec_Interlace_1;
    Huffman_WMV m_hufCBPCYDec_Interlace_2;
    Huffman_WMV m_hufCBPCYDec_Interlace_3;
    Huffman_WMV m_hufCBPCYDec_Interlace_4;
    Huffman_WMV m_hufCBPCYDec_Interlace_5;
    Huffman_WMV m_hufCBPCYDec_Interlace_6;
    Huffman_WMV m_hufCBPCYDec_Interlace_7;
    Huffman_WMV m_hufCBPCYDec_Interlace_8;
    Huffman_WMV m_HufMVTable_Interlace_MV1;
    Huffman_WMV m_HufMVTable_Interlace_MV2;
    Huffman_WMV m_HufMVTable_Interlace_MV3;
    Huffman_WMV m_HufMVTable_Interlace_MV4;
    Huffman_WMV m_HufMVTable_Interlace_MV5;
    Huffman_WMV m_HufMVTable_Interlace_MV6;
    Huffman_WMV m_HufMVTable_Interlace_MV7;
    Huffman_WMV m_HufMVTable_Interlace_MV8;
    Huffman_WMV m_HufMVTable_Progressive_MV1;
    Huffman_WMV m_HufMVTable_Progressive_MV2;
    Huffman_WMV m_HufMVTable_Progressive_MV3;
    Huffman_WMV m_HufMVTable_Progressive_MV4;
    Huffman_WMV m_hufFieldPictureMBMode1MV1;
    Huffman_WMV m_hufFieldPictureMBMode1MV2;
    Huffman_WMV m_hufFieldPictureMBMode1MV3;
    Huffman_WMV m_hufFieldPictureMBMode1MV4;
    Huffman_WMV m_hufFieldPictureMBMode1MV5;
    Huffman_WMV m_hufFieldPictureMBMode1MV6;
    Huffman_WMV m_hufFieldPictureMBMode1MV7;
    Huffman_WMV m_hufFieldPictureMBMode1MV8;
    Huffman_WMV m_hufFieldPictureMBModeMixedMV1;
    Huffman_WMV m_hufFieldPictureMBModeMixedMV2;
    Huffman_WMV m_hufFieldPictureMBModeMixedMV3;
    Huffman_WMV m_hufFieldPictureMBModeMixedMV4;
    Huffman_WMV m_hufFieldPictureMBModeMixedMV5;
    Huffman_WMV m_hufFieldPictureMBModeMixedMV6;
    Huffman_WMV m_hufFieldPictureMBModeMixedMV7;
    Huffman_WMV m_hufFieldPictureMBModeMixedMV8;
    Huffman_WMV m_hufFieldPictureMVBP1;
    Huffman_WMV m_hufFieldPictureMVBP2;
    Huffman_WMV m_hufFieldPictureMVBP3;
    Huffman_WMV m_hufFieldPictureMVBP4;
    Huffman_WMV m_hufInterlaceFrame2MVBP1;
    Huffman_WMV m_hufInterlaceFrame2MVBP2;
    Huffman_WMV m_hufInterlaceFrame2MVBP3;
    Huffman_WMV m_hufInterlaceFrame2MVBP4;
    
    // Extension Header / GOP
    Bool_WMV m_bBroadcastFlags,  m_bTopFieldFirst, m_bRepeatFirstField;
    Bool_WMV m_bSkipFrame, m_bTemporalFrmCntr;

    Double_WMV m_dFrameRate;

    Bool_WMV m_bExtendedMvMode;

    Bool_WMV m_bPostProcInfoPresent;

    I32_WMV m_iPostProcLevel;

    U8_WMV m_pZigzagInvDCTHorzFlags [BLOCK_SQUARE_SIZE];

    I32_WMV m_iOverlapIMBCodingMode;

    I32_WMV     m_iNbrBlkRunNoContext;
    Bool_WMV    m_bISLOnly;
    Bool_WMV m_bExtendedDeltaMvMode;
    I32_WMV  m_iDeltaMVRangeIndex;
    I32_WMV  m_iExtendedDMVX, m_iExtendedDMVY;

    Bool_WMV m_bProgressive420;
    //Bool_WMV m_bRefreshSequenceHeader;
    Bool_WMV m_bUseMostRecentFieldForRef;
    Bool_WMV m_bUseSameFieldForRef;
    Bool_WMV m_bUseOppFieldForRef;
    Bool_WMV m_bTwoRefPictures;

    I32_WMV  m_iCurrentTemporalField;
    I32_WMV m_iPrevSpatialField;


    Huffman_WMV* m_pHufICBPCYDec;
    Huffman_WMV* m_pHufPCBPCYDec;
    Huffman_WMV* m_pHufInterlaceCBPCYTables[8];

    I32_WMV     m_iTopNbrMBIdx;
    I32_WMV     m_iTopNbrBlkIdx;
    Bool_WMV    m_bTopNbrMode;

    I32_WMV     m_iLeftNbrMBIdx;
    I32_WMV     m_iLeftNbrBlkIdx;
    Bool_WMV    m_bLeftNbrMode;

    tFrameType_WMV m_PredTypePrev;

    tFrameType_WMV m_FirstFieldType;
    tFrameType_WMV m_SecondFieldType;
    I32_WMV m_iRefFrameDistance;        // Used for P fields and forward B
    I32_WMV m_iBackRefFrameDistance;    // Used for backward B
    I32_WMV m_iMaxZone1ScaledFarMVX;
    I32_WMV m_iMaxZone1ScaledFarMVY;
    I32_WMV m_iZone1OffsetScaledFarMVX;
    I32_WMV m_iZone1OffsetScaledFarMVY;
    I32_WMV m_iFarFieldScale1;
    I32_WMV m_iFarFieldScale2;
    I32_WMV m_iNearFieldScale;
    I32_WMV m_iMaxZone1ScaledFarBackMVX;
    I32_WMV m_iMaxZone1ScaledFarBackMVY;
    I32_WMV m_iZone1OffsetScaledFarBackMVX;
    I32_WMV m_iZone1OffsetScaledFarBackMVY;
    I32_WMV m_iFarFieldScaleBack1;
    I32_WMV m_iFarFieldScaleBack2;
    I32_WMV m_iNearFieldScaleBack;

    I32_WMV     m_iTopNbrXformMode;
    I32_WMV     m_iLeftNbrXformMode;

    I32_WMV     m_iTopNbrEdgeMask;
    I32_WMV     m_iLeftNbrEdgeMask;

    Bool_WMV m_bInterlacedSource; 
    I32_WMV m_iRepeatFrameCount;
   
    //variables for re-init sequence header
    I32_WMV m_iPrevSeqFrmWidthSrc, m_iPrevSeqFrmHeightSrc, m_iMaxPrevSeqFrameArea;
    Bool_WMV m_bLargerSeqBufferRequired, m_bSeqFrameWidthOrHeightChanged, m_bPrevSeqCBEntropyCoding;

    I32_WMV m_iChromaFormat;


    Bool_WMV m_bHRD_PARAM_FLAG;
    I32_WMV  m_ihrd_num_leaky_buckets;
    Bool_WMV m_bRangeRedY_Flag, m_bRangeRedUV_Flag;
    I32_WMV m_iRangeRedY, m_iRangeRedUV;

    I32_WMV m_iLevel;
    I32_WMV m_iMaxPrevA2LCFrameArea;

    Bool_WMV m_bSliceWMVA;
    U32_WMV *m_puiFirstMBRowOfSlice, m_uiNumSlicesWMVA;
    U32_WMV m_uiNumSlicesField1WMVA;
    U32_WMV *m_puiNumBytesOfSliceWMVA, m_uiNumBytesFrameHeaderSlice, m_uiNumBytesFieldHeaderSlice;
    Bool_WMV *m_pbStartOfSliceRow, *m_pbStartOfSliceRowOrig;
    U32_WMV m_uiCurrSlice;
    I32_WMV m_iSliceBufSize;
	U32_WMV SliceY; //zou



    I32_WMV m_iFrameNumber;
    Bool_WMV m_bParseStartCodeFrameOnly;
    I32_WMV m_iParseStartCodeLastType;
    I32_WMV m_iParseStartCodeRemLen;
    I32_WMV m_iBufferedUserDataLen;
    U8_WMV * m_pBufferedUserData;

    U8_WMV  m_iParseStartCodeRem[3];

    U8_WMV *m_pParseStartCodeBuffer;
    U8_WMV *m_pParseStartCodeBitstream;
    U32_WMV m_iParseStartCodeBufLen;

    // members associated with StartCode Callback
    Void_WMV * m_pCallBackApp;          // Opaque pointer passed back to the App in the callback
    U32_WMV m_u32SCEnableMask;

    Bool_WMV m_bResizeDimensions;

    Bool_WMV m_bDelaySetSliceWMVA;
    Bool_WMV m_bParseStartCodeSecondField;

#ifdef DUMPLOG
    FILE * m_pFileLog;
#endif

    I32_WMV m_iMaxFrmWidthSrc, m_iMaxFrmHeightSrc; // Maximum coded frame size for sequence
	
    Bool_WMV m_bSwitchingEntryPoint;

    Bool_WMV m_bBrokenLink, m_bClosedEntryPoint, m_bPanScanPresent, m_bRefDistPresent;

	I32_WMV m_iFrmWidthSrcOld, m_iFrmHeightSrcOld;

    Bool_WMV m_bRenderDirectWithPrerender;

    // from ASF Binding enum
    Bool_WMV m_bNoSliceCodeInSequence;
    Bool_WMV m_bNoMultipleEntryInSequence;
    Bool_WMV m_bNoMultipleSequenceHeaderInSequence;
	Bool_WMV m_bNoInterlaceSource;
	Bool_WMV m_bNoBFrameInSequence;

	//Other memory usage supplied by APP.
	MemOpteratorInternal	m_MemOperator;	//Video memory shared with APP.
#ifdef XDM
	MemOpteratorXDM     	m_MemXDM;
#endif
	VO_MEM_OPERATOR			*m_memOp;		//Buffer memory alloced outside decoder.

	void*					m_pInputFrameData;
	I32_WMV					m_InBuffLenLeft;	
	long long				m_inTimeStamp;
//	long long				m_outTimeStamp;
	U32_WMV					m_outputMode;
    VO_PTR    m_inUserData;
						
	void*					m_phCheck;
	VO_VIDEO_BUFFER  m_pPrevOutputBuff;
	I32_WMV					m_iRefBuffOffsetForCheck;
	I32_WMV					m_iRefBuffYStrideForCheck;
						
	Bool_WMV				m_bIsHeadDataParsed;
#ifdef VIDEO_MEM_SHARE
	VO_MEM_VIDEO_OPERATOR	m_videoMemShare;
	FIFOTYPE				m_memManageFIFO;
	I32_WMV					m_frameBufNum;
	I32_WMV					m_frameBufStrideY;
	I32_WMV					m_frameBufStrideUV;
#endif

#ifdef WIN32
	I8_WMV					m_SeqHeadInfoString[2048];
#endif
	I32_WMV					m_frameNum;
	/* For WVC1 decoding */
	Bool_WMV				m_bCodecIsWVC1;
	VO_BITMAPINFOHEADER*	m_pBMPINFOHEADER;
	I32_WMV					m_IBPFlag;

    tWMVDimensions tDimensions;

#ifdef DUMP_INPUT_DATA
	FILE					*m_fpDumpData;
	char					fileName[256];
#endif
} tWMVDecInternalMember;

#include "dectable_c.h"

#endif // __WMVDEC_MEMBER_H_
