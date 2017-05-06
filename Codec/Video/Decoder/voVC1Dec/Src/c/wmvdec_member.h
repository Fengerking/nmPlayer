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
#include "constants_wmv.h"
#include "xplatform_wmv.h"
#include "voWmvPort.h"
#include "huffman_wmv.h"
#include "strmdec_wmv.h"
#include "motioncomp_wmv.h"
#include "wmvdec_api.h"

#define MULTI_CORE_DEC  1

#ifdef MULTI_CORE_DEC
#include "voVc1Thread.h"
#endif

/* Should close this log info when release. */
#ifdef _VOLOG_INFO
//#define VO_LOG_INFO_MemoryFree
//#define VO_LOG_INFO_traceFunction
#endif

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

#define BLOCK_SIZE_2    32
#define BLOCK_SQUARE_SIZE_2 256
#define BLOCK_SQUARE_SIZE_2MINUS2 252
#define BLOCK_SQUARE_SIZE_MINUS1_4 252

#define MAXHALFQP           8
#define MAXHIGHRATEQP       8
#define MAX3QP              8

typedef enum DCACPREDDIR {LEFT, TOP, NONE} DCACPREDDIR;


extern const I32_WMV g_iStepRemap[];      // QP-index to QP mapping array used for 5QP deadzone quantizer.

// Profiles for WMV3

enum { NOT_WMV3 = -1, WMV3_SIMPLE_PROFILE, WMV3_MAIN_PROFILE, WMV3_PC_PROFILE, WMV3_ADVANCED_PROFILE, WMV3_SCREEN };


    typedef I32_WMV I_SIMD;
    typedef U32_WMV U_SIMD;



#ifdef _12BITCOEFCLIP
    extern Void_WMV ClipCoef32 (U32_WMV b, U32_WMV n, I32_WMV *p);
    extern Void_WMV ClipCoef16 (U32_WMV b, U32_WMV n, I16_WMV *p);
#   define CLIPCOEF32(b,n,p32) ClipCoef32(b,n,p32)
#   define CLIPCOEF16(b,n,p16) ClipCoef16(b,n,p16)
#else
#   define CLIPCOEF32(b,n,p32)
#   define CLIPCOEF16(b,n,p16)
#endif

#define CLIP(x) ((U32_WMV) x <= 255 ? x : (x < 0 ? 0: 255))

//extern Bool_WMV g_bSupportMMX_WMV;

#define MAX_ADDR_RELOCABLE 8

//typedef struct MBDCTCOEFF {
//    I16_WMV  coeff[384];
//}VODCTCOEFF;

//typedef struct BLOCKMODE {
//    U32_WMV Intra[4];
//}VOBLOCKMODE;

//typedef struct tUNCACHE_RELOC_CTL
//{
//
//    U8_WMV ** ppAddrReloc[MAX_ADDR_RELOCABLE];
//    
//    I32_WMV   iNumAddrReloc;
//
//}
//UNCACHE_RELOC_CTL;

//typedef struct tUNCACHE_CTL
//{
// //   U8_WMV ** ppAddrRelocatable;
//    UNCACHE_RELOC_CTL reloc;
//    
//    I32_WMV  addr;
//    struct tUNCACHE_CTL * next;
//}
//UNCACHE_CTL;

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
//typedef struct tagEMB_DecodeP_ShortCut
//{
//
//    struct tagWMVDecInternalMember * pVideoDecoderObject;
//
//    I32_WMV * m_rgiCoefRecon;
//    own CInputBitStream_WMV * m_pbitstrmIn;
//    I32_WMV m_i2DoublePlusStepSize;
//    I32_WMV m_iDoubleStepSize;
//    I32_WMV m_iStepMinusStepIsEven;
//
//    I32_WMV XFormMode;
//    I32_WMV m_iDCTHorzFlags;
//
//}EMB_DecodeP_ShortCut;



                            
typedef enum THREADTYPE_DEC {REPEATPAD, DECODE, RENDER, LOOPFILTER, DEBLOCK, DEBLOCK_RENDER, DEINTERLACE} THREADTYPE_DEC;
typedef enum KEYS {FPPM, CPPM, PPM, RES, BITRATE, ADAPTPPM, ADAPTMHZ, QPCLEVEL1, QPCLEVEL2, QPCLEVEL3, QPCLEVEL4, OMITBFM, FAKEBEHIND, COUNTCPUCYCLES} KEYS;
typedef enum SKIPBITCODINGMODE {Normal = 0, RowPredict, ColPredict} SKIPBITCODINGMODE;


// Intra X8 stuff
//typedef struct t_SpatialPredictor {
//    const  U8_WMV   *m_pClampTable;
//    const U8_WMV    *m_pRef;
//    I32_WMV         m_iRefStride;
//    Bool_WMV        m_bFlat;
//    I32_WMV         m_iDcValue;
//    U8_WMV          *m_pLeft, *m_pTop;
//    U8_WMV          *m_pNeighbors;
//    Bool_WMV        m_bInitZeroPredictor;
//    U16_WMV         *m_pSums;
//    U16_WMV         *m_pLeftSum;
//    U16_WMV         *m_pTopSum;
//    I16_WMV         *m_pBuffer16; // buffer for setupZeroPredictor speedup
//
//    Void_WMV  (*m_pIDCT_Dec) (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffset, I32_WMV iFlags);  // decoder side
//
//} t_CSpatialPredictor;


//typedef struct t_ContextWMV {
//    I32_WMV m_iRow;
//    I32_WMV m_iCol;
//    I32_WMV m_iColGt2;
//    U8_WMV *m_pData; // one copy enough?
//} t_CContextWMV;

//struct t_AltTablesDecoder; 

/**********************************************************************
 CLocalHuffmanDecoder : definition
**********************************************************************/
//typedef struct t_LocalHuffmanDecoder
//{
//    struct t_AltTablesDecoder  *m_pAltTable;
//
//    I32_WMV  m_iN;  // number of symbols inc escapes
//    const U8_WMV *m_pFine;
//    I32_WMV  m_iMode;
//    I32_WMV  m_i1shiftMode;
//    I32_WMV  m_iK;  // number of codeword tables
//    I32_WMV  logK;
//    U8_WMV   *opt;
//    U8_WMV   *m_pCodeTablePtr;
//
//    I32_WMV  *fineMask;
//    Huffman_WMV  *m_pHuff;
//    tPackHuffmanCode_WMV      *m_pCodeTable;
//
//} t_CLocalHuffmanDecoder;


enum { LH_INTER0, LH_INTER, LH_INTRAY0, LH_INTRAY,
       // IntraUV uses Inter tables
       LH_ORIENT, LH_INTRAZ, LH_INTRANZ, LH_INTRAC0, 
       LH_MV0, LH_MV1, LH_CBP,
       LH_INTRAC1,              // New Intra coding
       LH_ENUM_SIZE             // possible number of contexts
    };


//typedef struct t_AltTablesDecoder
//{
//    U32_WMV    m_uiUserData;
//    //I32_WMV    m_iNewFrame;
//    //I32_WMV    m_iNewIntraFrame;
//    struct t_LocalHuffmanDecoder   *m_paLH[LH_ENUM_SIZE];
//    //Bool_WMV   m_bIntraFrame;
//    I32_WMV    m_iNContexts;
//
//    //I32_WMV    m_iQLev;
//    //Bool_WMV   m_bIsFirstRun;
//    I32_WMV    m_iHalfPelMV; // X9
//
//} t_CAltTablesDecoder;// no need

typedef enum tagFrameType_WMV
{
//    IFRAME_WMV = 0, 
//    PFRAME_WMV
      IVOP = 0, 
      PVOP,
      BVOP,
      SPRITE
      , BIVOP,
      SKIPFRAME
} tFrameType_WMV;


#define FRMOP_DEBLOCK                           1
#define FRMOP_COPY_CURR2POST          2
#define FRMOP_COPY_REF2PREVPOST   3
#define FRMOP_ADJUST_DEC_RANGE       4
#define FRMOP_UPSAMPLE                          8


//typedef struct    tagDeBlockCtl 
//{
//    I32_WMV m_iOpCode ;
//    struct tagYUV420Frame_WMV * m_pfrmSrc;
//    I32_WMV m_iWidthPrevY ;
//    I32_WMV m_iWidthPrevUV;
//    I32_WMV m_iWidthPrevYXExpPlusExp;
//    I32_WMV m_iWidthPrevUVXExpPlusExp ;
//    I32_WMV m_uintNumMBX;
//    I32_WMV m_uintNumMBY;
//    I32_WMV m_iHeightUV;
//    I32_WMV m_iStepSize ;
//    I32_WMV m_iMBSizeXWidthPrevY;
//    I32_WMV m_iBlkSizeXWidthPrevUV;
//    //Bool_WMV m_bDeringOn ;
//    I32_WMV iMBStartY;
//    I32_WMV iMBEndY;
//}RF_DeBlockCtl ;

//typedef struct    tagCpyCtrl 
//{
//     I32_WMV m_iOpCode;
//     struct tagYUV420Frame_WMV *  m_pfrmSrc;
//      I32_WMV m_uintNumMBY;
//      I32_WMV m_iWidthPrevY  ;
//      I32_WMV m_iWidthPrevUV ;
//      I32_WMV m_iWidthPrevYXExpPlusExp ;
//      I32_WMV m_iWidthPrevUVXExpPlusExp ;
//}
//RF_CpyCtl;

//typedef struct    tagAdjustDecRangeCtrl 
//{
//     I32_WMV m_iOpCode;
//     struct tagYUV420Frame_WMV *  m_pfrmSrc;
//
//      I32_WMV m_iWidthPrevYXExpPlusExp ;
//      I32_WMV m_iWidthPrevUVXExpPlusExp ;
//      I32_WMV m_uintNumMBY;
//
//      I32_WMV m_iWidthPrevY;
//      I32_WMV m_iWidthPrevUV;
//     // I32_WMV m_iHeightPrevY;
//     // I32_WMV m_iHeightPrevUV;
//      I32_WMV m_iRangeState;
//}
//RF_AdjustDecRangeCtl;

//typedef struct  tagUpsampleRefCtl
//{
//     I32_WMV m_iOpCode;
//     struct tagYUV420Frame_WMV *  m_pfrmSrc;   
//
//     I32_WMV m_iHeightY;
//     I32_WMV m_iWidthY;
//     I32_WMV m_iHeightUV;
//     I32_WMV m_iWidthUV;
//
//     I32_WMV m_iWidthPrevYXExpPlusExpSrc ;
//     I32_WMV m_iWidthPrevUVXExpPlusExpSrc;
//     I32_WMV m_iWidthPrevYXExpPlusExpDest ;
//     I32_WMV m_iWidthPrevUVXExpPlusExpDest;
//     I32_WMV m_iResIndex ;
//     I32_WMV m_iResIndexPrev;
//     I32_WMV m_iWidthPrevYSrc ;
//     I32_WMV m_iWidthPrevUVSrc ;
//     I32_WMV m_iWidthPrevY ;
//     I32_WMV m_iWidthPrevUV;
//     U32_WMV m_uiNumMBY;
//}
//RF_UpsampleRefCtl;

//typedef union tagFrmOps
//{
//    RF_DeBlockCtl  m_OpDeblock;
//    RF_CpyCtl         m_OpCpyCtl;
//    RF_AdjustDecRangeCtl  m_OpDecRange;
//    RF_UpsampleRefCtl  m_OpUpSample;
//}FrmOpUnions;

typedef union _Motion_EMB
{
    I16_WMV   I16[2];
    U32_WMV  U32;
    I32_WMV   I32;
}UMotion_EMB;

typedef struct tCMotionVector_X9_EMB
{
    UMotion_EMB  m_vctTrueHalfPel;
}   CMotionVector_X9_EMB;

//#endif
typedef struct tagYUV420Frame_WMV
{
    U8_WMV* m_pucYPlane;
    U8_WMV* m_pucUPlane;
    U8_WMV* m_pucVPlane;

    U8_WMV* m_pucYPlane_Unaligned;
    U8_WMV* m_pucUPlane_Unaligned;
    U8_WMV* m_pucVPlane_Unaligned;
#ifdef USE_FRAME_THREAD
    I32_WMV  m_decodeprocess;		
#endif
	CMotionVector_X9_EMB * m_rgmv1_EMB;
    U8_WMV* m_pucYPlaneAlt;
    U8_WMV* m_pucUPlaneAlt;
    U8_WMV* m_pucVPlaneAlt;

    U8_WMV* m_pCurrPlaneBase;

    //FrmOpUnions   m_pOps[8];
    //I32_WMV  m_iNumOps;

	long long 	m_timeStamp;
	tFrameType_WMV m_frameType;
    VO_PTR		m_UserData;
#ifdef SHAREMEMORY
	VO_S32      m_AdjustDecRangeAdvNeed;
	VO_S32      m_AdjustDecRangeNeed;
#endif
	VO_S32  m_removeable;
	I32_WMV m_iFrmWidthSrc;
	I32_WMV m_iFrmHeightSrc;
	
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
     U8_WMV m_RndTbl[4];
 }PredictMVLUT_UNION_MISC;

typedef union __PredictMVLUT_UNION
{
    PredictMVLUT_UNION_MISC     U;
    PredictMV_LUT_PullBack m_PullBack[5];
}
PredictMVLUT_UNION;

typedef struct _PredictMVLUT
{
    I8_WMV m_rgcBOffset[16];
    PredictMVLUT_UNION U;      
}PredictMV_LUT;

//typedef struct MBDCTCOEFF {
//    I16_WMV coeff[384];
//}VODCTCOEFF;

typedef struct tagEMB_PBMainLoop
{

    struct tagWMVDecInternalMember  *m_pWMVDec;
    CDiffMV_EMB * m_pDiffMV_EMB ;
    I16_WMV * m_rgiMVDecLUT_EMB ;

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
	I32_WMV m_iDctRunFlags;
    Bool_WMV m_iOverlap;
	I32_WMV m_iFilterType[2] ;
    I32_WMV m_iRndCtrl ;

    CInputBitStream_WMV* m_pbitstrmIn;
    Huffman_WMV* m_pHufMVTable_V9 ;
	I16_WMV  *m_pXMotion;
	I16_WMV  *m_pYMotion;
	UMotion_EMB * m_pMotion;
	UMotion_EMB * m_pMotionC;
        
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

#ifdef MULTI_CORE_DEC
    I32_WMV *bIntraFlag ;
    I32_WMV *b1MVFlag;
    I32_WMV m_uintNumMBX;
    CDiffMV_EMB * m_pDiffMV_EMB_Bak;
    CWMVMBModeBack *m_pMbMode;
	CWMVMBMode* m_rgmbmd_cur;
	CWMVMBMode* m_rgmbmd_last;
#endif
    U8_WMV* m_pZigzagScanOrder;
    I8_WMV m_rgiSubsampleBicubic[16] ;
    U8_WMV * m_ppxliRefBlkBase[6];   
    U8_WMV * m_ppxliCurrBlkBase[6] ;    
    //I32_WMV m_iFrmMBOffset[2] ; 
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

    I32_WMV (*m_pDecodeIdct[4])(UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags,I32_WMV i);
 
     //I32_WMV (*m_pDecodeInterError[4])(struct tagEMB_PBMainLoop  * pPMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
     U8_WMV m_uiIDCTClearOffset[4][4];


     U8_WMV m_iBlkXformTypeLUT[8];
     U8_WMV m_iSubblkPatternLUT[8] ;
     U8_WMV  *m_pSSIMD_DstOffSet;

     I_SIMD (*m_pInterpolateBicubic[4][4]) (const U8_WMV *pSrc,
                                                 I32_WMV iSrcStride, 
                                                 U8_WMV *pDst, 
                                                 struct tagEMB_PBMainLoop  * pPBMainLoop, 
                                                 I32_WMV iXFrac, 
                                                 I32_WMV iYFrac,
                                                 Bool_WMV b1MV, 
                                                 U16_WMV * pTbl);

     I_SIMD (*m_pInterpolateBicubic_new[4][4])(U8_WMV *pSrc, 
                                                I32_WMV iSrcStride,
                                                U8_WMV *pDst, 
                                                I32_WMV iDstStride,
                                                struct tagEMB_PBMainLoop * pPBMainLoop,
                                                I32_WMV iXFrac,
                                                I32_WMV iYFrac,
                                                Bool_WMV b1MV);


     Void_WMV (*m_pIDCTDec16_WMV3) (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
     Void_WMV (*m_pAddError) (U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch);
     Void_WMV (*m_pAddError_new) (U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch);
     Void_WMV (*m_pAddNull)(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch);
     Void_WMV (*m_pInterpolateBlockBilinear[4][4])        (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
     Void_WMV (*m_pInterpolateBlockBilinear_new[4][4]) (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,  I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
     //I32_WMV  (*m_pDecodeInterError8x8)(struct tagEMB_PBMainLoop* pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ);
     I_SIMD  (*m_pInterpolateBlock_00)(const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, struct tagEMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV,U16_WMV *pTbl);
     I_SIMD  (*m_pInterpolateBlock_00_new)(U8_WMV *pSrc,
                                                     I32_WMV iSrcStride, 
                                                     U8_WMV *pDst, 
                                                     I32_WMV iDstStride,
                                                     struct tagEMB_PBMainLoop * pMainLoop,
                                                     I32_WMV iXFrac,
                                                     I32_WMV iYFrac, 
                                                     Bool_WMV b1MV);
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
     U32_WMV m_uif000f000;
     U32_WMV m_ui00ff00ff;

     U16_WMV m_rgiNewVertTbl[2][2][4];
     //U16_WMV m_rgiNewVertTbl_2[2][2][4]; //sw


     CDiffMV_EMB   pDiffMV_EMB_Bak[6] ;
     U32_WMV bTopRowOfSlice;
     U32_WMV*  prgMotionCompBuffer_EMB;
     U32_WMV *pMotionCompBuffer_EMB[2];

     I16_WMV  *pThreadIntraBlockRow0[3];
     I16_WMV  *pThreadIntraMBRowU0[2];
     I16_WMV  *pThreadIntraMBRowV0[2];

     //I32_WMV  prev_overlap_mb_v;
     //I32_WMV  prev_overlap_mb_h;
     //I32_WMV  prev_overlap_mbrow;

}
EMB_PBMainLoop;


typedef struct tagEMB_PMainLoop
{
    EMB_PBMainLoop PB[2];

    U32_WMV* m_rgMotionCompBuffer_EMB ;
    I32_WMV (*m_rgfPredictHybridMV_EMB[3]) (struct tagEMB_PMainLoop  * pPMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk,EMB_PBMainLoop *pPB);
    I32_WMV (**m_ppPredictHybridMV_EMB) (struct tagEMB_PMainLoop  * pPMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk,EMB_PBMainLoop *pPB);

    PredictMV_LUT  m_PredictMVLUT;
    U8_WMV m_SSIMD_DstLUT_1MV[6];
    U8_WMV m_SSIMD_DstLUT_BK[6];

    EMB_PBMainLoop*pMainPB;
    EMB_PBMainLoop*pThreadPB;
}
EMB_PMainLoop;

typedef struct tagEMB_BMainLoop
{
    EMB_PBMainLoop PB[2];    
    I32_WMV m_iScaleFactor;     
     U8_WMV  m_SSIMD_DstLUT_B[6];
     Void_WMV (*m_rgfPredictMV_EMB[3]) (struct tagEMB_BMainLoop  * pBMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk,EMB_PBMainLoop *pPBB,U32_WMV ncols, U32_WMV nrows);
    Void_WMV (**m_ppPredictMV_EMB) (struct tagEMB_BMainLoop  * pBMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk,EMB_PBMainLoop *pPBB,U32_WMV ncols, U32_WMV nrows);

    U8_WMV m_RndTbl[4];
    U8_WMV m_iMBTypeMask[4];
    I32_WMV m_iMVMin[3];
    I32_WMV m_iMVMax[3];
    UMotion_EMB * m_pMotionCurr;
    UMotion_EMB  * m_pMotionArray[2];

    U32_WMV m_rgIntraMotionCompBuffer_EMB[80] ;

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

#define FIFO_WRITE	0
#define FIFO_READ	1 
#define MAXFRAMES	6
#define FIFO_NUM	(MAXFRAMES+1)

typedef struct {
	tYUV420Frame_WMV **img_seq; 
	VO_U32 buf_num;
	VO_U32 w_idx;	      /* Index of write */
	VO_U32 r_idx;           /* Index of read */
}FIFOTYPE;

typedef struct
{
    int iBytesReadFrm;
    int iCounter;
    U8_WMV *pVideoBuffer;
}StreamingModeDataCtl;

struct tagWMVDecInternalMember;
typedef struct tagWMVDecInternalMember 
{
    //I32_WMV m_iOldWidth  ;    
    // sequence layer info
    CInputBitStream_WMV *m_pInputBitstream;
    own CInputBitStream_WMV* m_pbitstrmIn;
    // size info
    I32_WMV m_iWidthSource; // source width, might not be MB aligned
    I32_WMV m_iHeightSource; // source height, might not be MB aligned
    I32_WMV m_iWidthInternal; // internal width, MB aligned
    I32_WMV m_iWidthInternalUV;
    U32_WMV m_uintNumMBX, m_uintNumMBY, m_uintNumMB, m_uiRightestMB;
	U32_WMV iSliceHead_Pre,iSliceHead;
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
    //I32_WMV m_iMVLeftBound, m_iMVRightBound;
    I32_WMV m_iStepSize;
    Bool_WMV m_bHalfStep;

    // tables
    //const U8_WMV* m_rgiClapTabDec;  //not needed
    //U8_WMV * m_pcClapTable;

    // picture layer
    //CVector* m_rgmv;                    // 2 MB row's MV
    CWMVMBMode* m_rgmbmd;               // 2 MB row's MV
    CWMVMBMode* m_rgmbmd_cur;               // 2 MB row's MV
    CWMVMBMode* m_rgmbmd_last;               // 2 MB row's MV

    //CWMVMBMode* m_rgmbmd_cur_thread;               // 2 MB row's MV
    //CWMVMBMode* m_rgmbmd_last_thread;               // 2 MB row's MV

    CWMVMBMode* m_pmbmdZeroCBPCY;       // Pointer to the ZeroCBPCY
    //CWMVMBMode* m_rgmbmd1;
    //U8_WMV* m_rgchSkipPrevFrame;            // Prev frame's Skip bits
    Bool_WMV m_bDQuantOn; // frame level bit indicating DQuant
    tFrameType_WMV m_tFrmType;
    tFrameType_WMV m_tFrmPredType;
    tFrameType_WMV m_tFrmTypePriorFrame;
	tFrameType_WMV m_tFrmTypePrevious;
    I32_WMV m_iDCStepSize, m_iDCStepSizeC;
    I32_WMV m_i2DoublePlusStepSize, m_i2DoublePlusStepSizeNeg;
    I32_WMV m_iDoubleStepSize, m_iStepMinusStepIsEven;
    Bool_WMV m_bStepSizeIsEven;

    // MB layer info
    //Bool_WMV m_bMBHybridMV;         // Indicates whether MB-based HybridMV is on
    Bool_WMV m_bMBXformSwitching;   // Indicates whether MB-based transform switching is chosen for the frame
    //I32_WMV m_iMixedPelMV;          // 0 -> True MV, 1 -> Mixed
    I32_WMV m_iFrameXformMode;      // Transform mode (8x8, 8x4 or 4x8) for entire frame
    I32_WMV  m_iSkipbitCodingMode, m_iMVSwitchCodingMode;
    CDiffMV m_pDiffMV[4];
    CDiffMV_EMB * m_pDiffMV_EMB;
     I16_WMV m_rgiMVDecLUT_EMB[36];
     UMotion_EMB * m_pMotion;
     UMotion_EMB * m_pMotionC;

    Bool_WMV m_bDCTTable_MB, m_bDCTTable_MB_ENABLED;//, m_bDCPred_IMBInPFrame;
    I32_WMV  m_iMVRangeIndex, m_iLogXRange, m_iLogYRange, m_iXMVRange, m_iYMVRange, m_iXMVFlag, m_iYMVFlag;
    //Bool_WMV m_bSKIPBIT_CODING_;
    Bool_WMV m_bXformSwitch;
    //Bool_WMV m_bNEW_PCBPCY_TABLE;
    //Bool_WMV m_bFrmHybridMVOn;
    //Bool_WMV m_bCODFlagOn;          // COD flag is on (coded) for a P frame
    //I32_WMV m_iMvResolution;        // Indicate current frame's MV resolution
    I32_WMV  m_iFilterType; // 2/4/6 tap filter choice
    I16_WMV  *m_pX9dct, *m_pX9dctU, *m_pX9dctV;
    Bool_WMV m_bDQuantBiLevel; // frame level bit indicating bilevel DQ or not

    // Block layer buffers

    //muti-threads
    //VODCTCOEFF* m_idctcoeff;
    //VOBLOCKMODE *m_blockmode;


    U8_WMV m_riReconBuf[64*8+32];   // IDCT coeff recon buffer. Allow enough space for 32-byte alignment
    U8_WMV m_riPixelError[64*8+32]; // Pixel error buffer. Allow enough space for 32-byte alignment

    I32_WMV* m_rgiCoefReconPlus1;
    I32_WMV* m_rgiCoefRecon;   
    UnionBuffer* m_ppxliErrorQ;
    UnionBuffer* m_rgiCoefReconBuf;
    I16_WMV  *m_pXMotion, *m_pYMotion, *m_pXMotionC;//, *m_pYMotionC;
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

    //U8_WMV * m_pZigzagSSIMD;
    U8_WMV  m_p8x4ZigzagSSIMD[64];
    U8_WMV  m_p4x4ZigzagSSIMD[64];

    U32_WMV* m_rgMotionCompBuffer_EMB;
    UnionBuffer* m_rgErrorBlock;          //This needs to be cache block alligned 
 
    // for DCT AC pred. 
    I32_WMV m_iNumOfQuanDctCoefForACPredPerRow;
    I16_WMV* m_rgiQuanCoefACPred;
    I16_WMV** m_rgiQuanCoefACPredTable;

     I16_WMV m_pAvgQuanDctCoefDec[16];
     I16_WMV m_pAvgQuanDctCoefDecC[16];
    //I16_WMV* m_pAvgQuanDctCoefDec;
    //I16_WMV* m_pAvgQuanDctCoefDecC;
    //I16_WMV* m_pAvgQuanDctCoefDecLeft;
    //I16_WMV* m_pAvgQuanDctCoefDecTop;
    
    // DCT-DCAC prediction
    I32_WMV m_iACColPredOffset, m_iACRowPredOffset;
    I32_WMV m_iACColPredShift, m_iACRowPredShift;
    
    // Misc
    I32_WMV m_iDCTHorzFlags;    // Sparse IDCT flag
    Bool_WMV m_bFirstEscCodeInFrame;                    //WMV1 vlc EscCoding
    I32_WMV m_iNUMBITS_ESC_LEVEL, m_iNUMBITS_ESC_RUN;   //WMV1 vlc EscCoding
    
    //t_CAltTablesDecoder  *m_pAltTables;   
	I32_WMV    m_iHalfPelMV;

	I16_WMV *huffmanbuffer;
	I32_WMV huffmansize;
	I32_WMV huffmanused;
        
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
#if 0
    U32_WMV m_rgiEndIndexOfCurrZone[3];
#endif
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

    CDCTTableInfo_Dec* m_ppInterDCTTableInfo_Dec[3];
    CDCTTableInfo_Dec* m_ppIntraDCTTableInfo_Dec[3];
    CDCTTableInfo_Dec* m_pInterDCTTableInfo_Dec_Set[3];
    CDCTTableInfo_Dec* m_pIntraDCTTableInfo_Dec_Set[3];
    
    U32_WMV m_rgiDCTACInterTableIndx[3];
    U32_WMV m_rgiDCTACIntraTableIndx[3];

    // IMBInPFrame DC Pred
    //I32_WMV (*m_pBlkAvgX8_MMX) (const U8_WMV* ppxlcCurrRecnMB, I32_WMV iWidthPrev, I32_WMV iStepSize);
    //I32_WMV m_rgiDCRef [3]; // intra DC predictor

    // overlapped transform stuff
    Bool_WMV m_iOverlap, m_bSequenceOverlap;
	Bool_WMV m_iCodedSizeFlag;

	//Bool_WMV vo_OverLapFilter;     //zou
    I16_WMV  *m_pIntraBlockRowData;
    //I16_WMV  *m_pIntraBlockRow0[3];
    //I16_WMV  *m_pIntraMBRowU0[2];
    //I16_WMV  *m_pIntraMBRowV0[2];

    //CMotionVector_X9 * m_rgmv1;
    CMotionVector_X9_EMB * m_rgmv1_EMB;
	CMotionVector_X9_EMB * m_rgmv1_EMB_base;
    
    // Functions
    //tWMVDecodeStatus (*m_pDecodeB) (struct tagWMVDecInternalMember *pWMVDec);


    /*tWMVDecodeStatus (*m_pDecodePMB) (
        struct tagWMVDecInternalMember *pWMVDec,
        CWMVMBMode*        pmbmd,
        U8_WMV* ppxlcCurrQYMB,
        U8_WMV* ppxlcCurrQUMB,
        U8_WMV* ppxlcCurrQVMB,
        CoordI xRefY, CoordI yRefY,
        CoordI xRefUV, CoordI yRefUV
        );*/
    /*tWMVDecodeStatus (*m_pDecodeIMBAcPred) (
        struct tagWMVDecInternalMember *pWMVDec,
        CWMVMBMode*            pmbmd, 
        CWMVMBMode* pmbmd_lastrow, 
        U8_WMV*      ppxliTextureQMBY,
        U8_WMV*      ppxliTextureQMBU, 
        U8_WMV*      ppxliTextureQMBV, 
        I16_WMV*                piQuanCoefACPred, 
        I16_WMV**                piQuanCoefACPredTable, 
        Bool_WMV                bLeftMB, 
        Bool_WMV                bTopMB, 
        Bool_WMV                bLeftTopMB, 
        I32_WMV                  iMBX
        );*/
#if 0
    // Decoder MB Header
    tWMVDecodeStatus (* m_pDecodeMBOverheadOfIVOP) (struct tagWMVDecInternalMember *pWMVDec,
                                                                            CWMVMBMode* pmbmd,
                                                                            CWMVMBMode* pmbmd_lastrow,
                                                                            I32_WMV imbX, I32_WMV imbY);

    //tWMVDecodeStatus (* m_pDecodeMBOverheadOfPVOP) (struct tagWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd);

    Void_WMV (*m_pMotionCompZero) (MOTION_COMP_ZERO_ARGS);
#endif
    //U32_WMV m_iMotionCompRndCtrl;
    //EMB_DecodeP_ShortCut m_EMBDecodePShortCut;
    //tWMVDecodeStatus (*m_CoefDecoderAndIdctDecTable[4])(struct tagWMVDecInternalMember * pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec, I32_WMV iXFormMode);
   // tWMVDecodeStatus (*m_pCoefDecoderAndIdctDec8x4or4x8Table[8])(struct tagWMVDecInternalMember * pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec);
    //Void_WMV (*m_MotionCompAndAddErrorTable[16])(U8_WMV * ppxlcCurrQMB, const U8_WMV * ppxlcRef, I32_WMV iWidthFrm, I32_WMV * pErrorBuf);

    Void_WMV (*m_pInterpolateMBRow) (struct tagWMVDecInternalMember * pWMVDec,
        U8_WMV *pSrc, I32_WMV iSrcStride, 
        U8_WMV *pDst, I32_WMV iDstStride, 
        I32_WMV iXFrac, I32_WMV iYFrac,
        I32_WMV iFilterType, I32_WMV iRndCtrl,
        I32_WMV iNumRows);

    //Void_WMV (*m_pInterpolateBlockBilinear) (const U8_WMV *pSrc, I32_WMV iSrcStride, 
   //     U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl); 
    Void_WMV (*m_pAddError) (U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch );
    Void_WMV (*m_pAddErrorIntra) (U8_WMV* ppxlcDst, U8_WMV* ppxlcRef, I16_WMV* ppxliError, I32_WMV iPitch );

    //block decoding fuctions
    //tWMVDecodeStatus (*m_pDecodeInverseInterBlockQuantize) (struct tagWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec** InterDCTTableInfo_Dec, U8_WMV* pZigzag, I32_WMV iXformMode,
   //     DQuantDecParam *pDQ );    
    // IDCT routines

    Void_WMV (*m_pIntraIDCT_Dec) (U8_WMV* piDst, I32_WMV iOffsetToNextRowForDCT, const I32_WMV __huge* rgiCoefRecon);
	Void_WMV (*m_pInterIDCT_Dec) (UnionBuffer* piDst, UnionBuffer* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
    Void_WMV (*m_pInter8x4IDCT_Dec) (UnionBuffer* piDst, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefRecon, I32_WMV iHalf);
    Void_WMV (*m_pInter4x8IDCT_Dec) (UnionBuffer* piDst, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefRecon, I32_WMV iHalf);
    Void_WMV (*m_pInter4x4IDCT_Dec) (UnionBuffer __huge* piDst, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefRecon, I32_WMV iQuadrant);
    Void_WMV (*m_pIntraX9IDCT_Dec) (UnionBuffer* piDst, UnionBuffer* piSrc, I32_WMV  iOffsetToNextRowForDCT, I32_WMV  iDCTHorzFlags);
      
    Void_WMV  (*m_pOverlapBlockVerticalEdge) (I16_WMV *pInOut, I32_WMV iStride);
    Void_WMV  (*m_pOverlapBlockHorizontalEdge) (I16_WMV *pSrcTop, I16_WMV *pSrcCurr,
        I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,
        Bool_WMV bTop, Bool_WMV bCurrent, Bool_WMV bWindup);
    Void_WMV (*m_pPixelMean) (const U8_WMV *pSrc1, I32_WMV iStride1, const U8_WMV *pSrc2, 
        I32_WMV iStride2, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iWidth, I32_WMV iHeight);

    // Sequence layer data
    //U32_WMV  m_uiUserData;

    I32_WMV  m_iBetaRTMMismatchIndex;    // Used by V9 loopfilter for RTM/Beta mismatch. 
    //Bool_WMV m_bRTMContent;
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
    Bool_WMV m_bRenderDirect;
    Bool_WMV m_bUse3QPDZQuantizer;
    Bool_WMV m_bExplicitQuantizer;
    Bool_WMV m_bExplicitSeqQuantizer;
    Bool_WMV m_bExplicitFrameQuantizer;
    Bool_WMV m_bSeqFrameInterpolation, m_bInterpolateCurrentFrame;
    Bool_WMV m_bOmittedBFrame;

    // Multithread event for clearing frame
    //HANDLE   hX8Clear, hX8MB[4];
    //Bool_WMV     m_bWaitingForEvent[4];

    // MPEG4-compliant variables
    //U32_WMV uiFCode;
    I32_WMV iRange;
    I32_WMV iScaleFactor;
    //I32_WMV m_iClockRate;
    Time m_t; // current time

    I32_WMV m_iFrameRate;
    I32_WMV m_iBitRate; // in Kbps
    
    //VOLMode m_volmd; // vol mode
    //VOPMode m_vopmd; // vop mode
    Bool_WMV m_fPrepared;
    Bool_WMV m_fDecodedI;
    
    U16_WMV m_uiNumFramesLeftForOutput;

    // pictures, frame's data
	//tYUV420Frame_WMV* m_pfrmLast;

	/*U8_WMV *lastY;
	U8_WMV *lastU;
	U8_WMV *lastV;*/

	U8_WMV* m_framebase;
	I32_WMV  framebaseused;
#ifdef SHAREMEMORY
	I32_WMV m_nIdx;
	I32_WMV m_benableShareMemory;
	I32_WMV m_nShareMemoryExt;
	tYUV420Frame_WMV** m_pfrmList;
	U32_WMV m_pfrmList_size;
#endif

    tYUV420Frame_WMV* m_pfrmCurrQ;
    tYUV420Frame_WMV* m_pfrmRef1Q;
    tYUV420Frame_WMV* m_pfrmRef0Q;
    tYUV420Frame_WMV* m_pfrmMultiresQ;
    tYUV420Frame_WMV* m_pfrmPostQ;
    tYUV420Frame_WMV* m_pfrmPostPrevQ;
    tYUV420Frame_WMV* m_pvopcRenderThis; // render this frame
    tYUV420Frame_WMV* m_pfrmYUV411toYUV420Q;//When output is YUV411,we need convert them to YUV420 which will be sent to APP.
#ifdef SHAREMEMORY    
	tYUV420Frame_WMV* m_pfrmDisplayPre;
	tYUV420Frame_WMV* m_pfrmRef0Process;
	tYUV420Frame_WMV* m_pfrmRef0Buffer[MAX_THREAD_NUM];
	I32_WMV m_ref0Indx;
#endif
	tYUV420Frame_WMV* m_pfrmPrevious;
	tYUV420Frame_WMV* m_pvopcRenderThisBak;
	tYUV420Frame_WMV* m_pfrmDisplayPreBak;

	I32_WMV bUseRef0Process;
	I32_WMV output_index; 
	tYUV420Frame_WMV* outputList[16]; //set 16 for tmp
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

    //U8_WMV* m_ppxliCurrQPlusExpY0;
    //U8_WMV* m_ppxliCurrQPlusExpU0;
    //U8_WMV* m_ppxliCurrQPlusExpV0;
  
	U8_WMV* vo_ppxliRefYPlusExp;//zou
	U8_WMV* vo_ppxliRefU;
	U8_WMV* vo_ppxliRefV;
  
    U32_WMV m_uiMBEnd0;
    
    // sequence layer info
    Bool_WMV m_bXintra8Switch;
    //Bool_WMV m_bMixedPel;
    Bool_WMV m_bLoopFilter;
    //Bool_WMV m_bNeedLoopFilterOnCurrFrm;
    //Bool_WMV m_bRndCtrlOn;
    I32_WMV  m_iRndCtrl;
    I32_WMV  m_iWMV3Profile;
    LOOPF_FLAG* m_pLoopFilterFlags;
    Bool_WMV m_bYUV411;
    Bool_WMV m_bStartCode;
    
    // picture layer info.
    //SKIPBITCODINGMODE m_SkipBitCodingMode;
    Bool_WMV m_bXintra8;
    I32_WMV  m_iQPIndex;
    //Bool_WMV m_bX9;
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
    //Bool_WMV m_bNewDCQuant;     //indicates whether improved DC quantization is used at low QP
    
    // B frame
    /*U8_WMV* m_ppxliRefYPreproc;
    U8_WMV* m_ppxliRefUPreproc;
    U8_WMV* m_ppxliRefVPreproc;*/
    I32_WMV  m_iNumBFrames;
    I32_WMV  m_iBFrameReciprocal;
    MBType  m_mbtShortBSymbol, m_mbtLongBSymbol;
    Bool_WMV m_bBFrameOn;   // global flag to indicate B frame coding mode
    Bool_WMV m_bPreProcRange;
    I32_WMV m_iRangeState;
    I32_WMV m_iReconRangeState;
	I32_WMV m_scaling;
    I32_WMV m_iDirectCodingMode;
    //CVector_X9* m_rgmv_X9;
    //CVector_X9* m_rgmvForwardPred;
    //CVector_X9* m_rgmvBackwardPred;
 
    // for multires coding //wshao:Multiresolution coding
    Bool_WMV m_bMultiresEnabled;
    I32_WMV m_iResIndex;
    I32_WMV m_iResIndexPrev;
    MULTIRES_PARAMS m_rMultiResParams[4];
    //I32_WMV m_iCurrQYOffset1, m_iCurrQUVOffset1;
    //I32_WMV m_iWidthPrevYTimes8Minus8, m_iWidthPrevUVTimes4Minus4; 
    I8_WMV *m_pMultiresBuffer;
    I8_WMV *m_pAlignedMultiresBuffer;

    // _REFLIBRARY_
    Bool_WMV m_bRefLibEnabled;
    I32_WMV m_iRefFrameNum;
    CReferenceLibrary *m_cReferenceLibrary;

    //INTERLACE 
    //Bool_WMV m_bInterlaceCodingOn; // global flag to indicate interlace coding mode
    //Bool_WMV m_bFieldCodingOn; // frame level flag to indicate field motest
    I16_WMV *m_pFieldMvX, *m_pFieldMvY; // Field MV
    I16_WMV *m_ppxliFieldMBBuffer; // Unaligned Buffer
    I16_WMV *m_ppxliFieldMB; // Aligned Buffer
    //I32_WMV m_iStartDeinterlace1;
    //I32_WMV m_iStartDeinterlace2;
    //I32_WMV m_iStartDeinterlace3;
    //Bool_WMV m_bHostDeinterlace;
    
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

    I32_WMV m_iSliceCode;
    U32_WMV m_uintNumMBYSlice; // slice size (number of MB's vertically)
    CodecVersion m_cvCodecVersion;

    // output BMP info
    //U32_WMV m_uiFOURCCOutput;
    //U16_WMV m_uiBitsPerPixelOutput;
    //U32_WMV m_uiWidthBMPOutput;
    //Bool_WMV m_bRenderFromPostBuffer;

    // Loopfilter
    U8_WMV *m_puchFilter8x8RowFlagY0;
    U8_WMV *m_puchFilter8x4RowFlagY0;
    U8_WMV *m_puchFilter8x8RowFlagU0; 
    U8_WMV *m_puchFilter8x4RowFlagU0; 
    U8_WMV *m_puchFilter8x8RowFlagV0; 
    U8_WMV *m_puchFilter8x4RowFlagV0; 
    U8_WMV *m_puchFilter8x8ColFlagY0;
    U8_WMV *m_puchFilter4x8ColFlagY0;
    U8_WMV *m_puchFilter8x8ColFlagU0;
    U8_WMV *m_puchFilter4x8ColFlagU0;
    U8_WMV *m_puchFilter8x8ColFlagV0;
    U8_WMV *m_puchFilter4x8ColFlagV0;
    
    // bitmap conversion
    //U8_WMV* m_pBMPBits;

    // Functions
    //tWMVDecodeStatus (*m_pDecodeI) (struct tagWMVDecInternalMember *pWMVDec);
    //tWMVDecodeStatus (*m_pDecodeP) (struct tagWMVDecInternalMember *pWMVDec);
    
    // Multires
    Void_WMV (*m_pHorizDownsampleFilter)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pVertDownsampleFilter)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pHorizUpsampleFilter)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
    Void_WMV (*m_pVertUpsampleFilter)(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
   
    // Interlaced
    //tWMVDecodeStatus (*m_pDeinterlaceProc)(struct tagWMVDecInternalMember * pWMVDec, U8_WMV *pucInput, U8_WMV *pucOutput0, I32_WMV iFirstLine, I32_WMV iLastLine, I32_WMV iWidth, I32_WMV iHeight);
    //Void_WMV (*m_pFieldBlockSub128) ( I16_WMV *pBlock, I32_WMV iNumCoef);
    Void_WMV (*m_pFieldBlockAdd128) ( I16_WMV *pBlock, I32_WMV iNumCoef);
    Void_WMV (*m_pFieldAddError8x8) (I16_WMV *pSrc, U8_WMV *pDst, I32_WMV iDstOffset);
    Void_WMV (*m_pFieldAddError4x8) (I16_WMV *pSrc, U8_WMV *pDst, I32_WMV iDstOffset);
    Void_WMV (*m_pInterpolate4ByteRow) (struct tagWMVDecInternalMember * pWMVDec,
                                        U8_WMV *pSrc, I32_WMV iSrcStride,U8_WMV *pDst,I32_WMV iDstStride, 
                                        I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iFilterType, I32_WMV iRndCtrl, I32_WMV iNumRows);
 
    // Deblocking filter
    //Void_WMV (*m_pDeblockInterlaceFrame) (struct tagWMVDecInternalMember *pWMVDec, U32_WMV iMBStartY, U32_WMV iMBEndY);
   //Void_WMV (*m_pDeblockIFrame) (struct tagWMVDecInternalMember *pWMVDec, U8_WMV * ppxliY, U8_WMV * ppxliU, U8_WMV * ppxliV, U32_WMV iMBStartY, U32_WMV iMBEndY);
   //Void_WMV (*m_pDeblockPFrame) (struct tagWMVDecInternalMember *pWMVDec, U8_WMV * ppxliY, U8_WMV * ppxliU, U8_WMV * ppxliV, U32_WMV iMBStartY, U32_WMV iMBEndY );
    Void_WMV (*m_pFilterHorizontalEdge)(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
    Void_WMV (*m_pFilterVerticalEdge)(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);

    // rendering function pointers
    //Bool_WMV m_bEMB3FrmEnabled;
    //Bool_WMV m_bRef0InRefAlt;
    //tYUV420Frame_WMV* m_pfrmRef0AltQ;
    
    EMB_PMainLoop  m_EMB_PMainLoop;
    EMB_BMainLoop  m_EMB_BMainLoop;

    I32_WMV EXPANDY_REFVOP;
    I32_WMV EXPANDUV_REFVOP;

    //U8_WMV  *m_pSequenceHeader;
    //U32_WMV m_uipSequenceHeaderLength;

    //Bool_WMV m_bOutputOpEnabled;

    Bool_WMV m_bInterlaceV2;        
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

    //Bool_WMV m_bLuminanceWarpTop, m_bLuminanceWarpBottom;
    //I32_WMV  m_iLuminanceScaleTop, m_iLuminanceShiftTop;
    //I32_WMV  m_iLuminanceScaleBottom, m_iLuminanceShiftBottom;

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
    /*Huffman_WMV m_HufMVTable_Interlace_MV1;
    Huffman_WMV m_HufMVTable_Interlace_MV2;
    Huffman_WMV m_HufMVTable_Interlace_MV3;
    Huffman_WMV m_HufMVTable_Interlace_MV4;
    Huffman_WMV m_HufMVTable_Interlace_MV5;
    Huffman_WMV m_HufMVTable_Interlace_MV6;
    Huffman_WMV m_HufMVTable_Interlace_MV7;
    Huffman_WMV m_HufMVTable_Interlace_MV8;*/
    Huffman_WMV m_HufMVTable_Progressive_MV1;
    Huffman_WMV m_HufMVTable_Progressive_MV2;
    Huffman_WMV m_HufMVTable_Progressive_MV3;
    Huffman_WMV m_HufMVTable_Progressive_MV4;

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

    //U8_WMV m_pZigzagInvDCTHorzFlags [BLOCK_SQUARE_SIZE];

    I32_WMV m_iOverlapIMBCodingMode;

    //I32_WMV     m_iNbrBlkRunNoContext;
    //Bool_WMV    m_bISLOnly;
    Bool_WMV m_bExtendedDeltaMvMode;
    I32_WMV  m_iDeltaMVRangeIndex;
    I32_WMV  m_iExtendedDMVX, m_iExtendedDMVY;

    Bool_WMV m_bProgressive420;

    Huffman_WMV* m_pHufICBPCYDec;
    Huffman_WMV* m_pHufPCBPCYDec;
    Huffman_WMV* m_pHufInterlaceCBPCYTables[8];

    //I32_WMV     m_iTopNbrMBIdx;
    //I32_WMV     m_iTopNbrBlkIdx;
    //Bool_WMV    m_bTopNbrMode;

    //I32_WMV     m_iLeftNbrMBIdx;
    //I32_WMV     m_iLeftNbrBlkIdx;
    //Bool_WMV    m_bLeftNbrMode;

    tFrameType_WMV m_PredTypePrev;

    Bool_WMV m_bInterlacedSource; 
    I32_WMV m_iRepeatFrameCount;
   
    //variables for re-init sequence header
    I32_WMV  m_iMaxPrevSeqFrameArea;
    Bool_WMV m_bSeqFrameWidthOrHeightChanged;

    I32_WMV m_iChromaFormat;

    Bool_WMV m_bHRD_PARAM_FLAG;
    I32_WMV  m_ihrd_num_leaky_buckets;
    Bool_WMV m_bRangeRedY_Flag, m_bRangeRedUV_Flag;
    I32_WMV m_iRangeRedY, m_iRangeRedUV;
    I32_WMV m_round;
    I32_WMV m_iLevel;

    Bool_WMV m_bSliceWMVA;
    //U32_WMV *m_puiFirstMBRowOfSlice, m_uiNumSlicesWMVA;
    Bool_WMV *m_pbStartOfSliceRow, *m_pbStartOfSliceRowOrig;
    U32_WMV m_uiCurrSlice;
    I32_WMV m_iSliceBufSize;
	//U32_WMV SliceY; //zou

    I32_WMV m_iFrameNumber;
    Bool_WMV m_bParseStartCodeFrameOnly;
    I32_WMV m_iParseStartCodeLastType;
    I32_WMV m_iParseStartCodeRemLen;

    U8_WMV  m_iParseStartCodeRem[3];

    U8_WMV *m_pParseStartCodeBuffer;
    U8_WMV *m_pParseStartCodeBitstream;
    U32_WMV m_iParseStartCodeBufLen;

    Bool_WMV m_bDelaySetSliceWMVA;
    Bool_WMV m_bParseStartCodeSecondField;

    I32_WMV m_iMaxFrmWidthSrc, m_iMaxFrmHeightSrc; // Maximum coded frame size for sequence
	
    Bool_WMV m_bSwitchingEntryPoint;

    Bool_WMV m_bBrokenLink, m_bClosedEntryPoint, m_bPanScanPresent, m_bRefDistPresent;

    Bool_WMV m_bRenderDirectWithPrerender;
    // from ASF Binding enum
    //Bool_WMV m_bNoSliceCodeInSequence;
	//Bool_WMV m_bNoInterlaceSource;
	//Bool_WMV m_bNoBFrameInSequence;

	//Other memory usage supplied by APP.
	MemOpteratorInternal	m_MemOperator;	//Video memory shared with APP.
	VO_MEM_OPERATOR     *m_memOp;		//Buffer memory alloced outside decoder.
	void*     m_pInputFrameData;
	I32_WMV    m_InBuffLenLeft;	
	long long    m_inTimeStamp;
	U32_WMV   m_outputMode;	
    VO_PTR		m_inUserData;
						
	void* m_phCheck;
	VO_VIDEO_BUFFER* m_pPrevOutputBuff;
						
	Bool_WMV m_bIsHeadDataParsed;

	VO_MEM_VIDEO_OPERATOR	m_videoMemShare;
	FIFOTYPE m_memManageFIFO;
	I32_WMV m_frameBufNum;
	I32_WMV m_frameBufStrideY;
	I32_WMV m_frameBufStrideUV;

//#ifdef WIN32
//	I8_WMV					m_SeqHeadInfoString[2048];
//#endif
	I32_WMV m_frameNum;
	I32_WMV m_input_header;
	I32_WMV nHasNewInput;
	/* For WVC1 decoding */
	Bool_WMV m_bCodecIsWVC1;
	VO_BITMAPINFOHEADER*	m_pBMPINFOHEADER;
	I32_WMV m_IBPFlag;


#if 1
	U32_WMV m_nframes; //zou
	U32_WMV m_nIframes;
	U32_WMV m_nBframes;
	U32_WMV m_nBIframes;
	U32_WMV m_nPframes;
	U32_WMV m_ncols;
	U32_WMV m_nrows;
    U32_WMV bTopBndry[4];
    U32_WMV xblock[4];
    U32_WMV yblock[4];
#endif

#ifdef MULTI_CORE_DEC
    U32_WMV  CpuNumber;
	U32_WMV preThdsIdx;
    //void *pDeblockParams;
    int nthreads;
    int   error_flag_thread;
    int   error_flag_main;
    pthread_t  h_thread_dec[16];
    //sem_t    *h_event_start_dec[16];
    //sem_t    *h_event_start_deblock[16];
    //U32_WMV   h_event_stop_dec[16];
    //sem_t        *m_event_main;
    //sem_t        *m_event_finish_deblock;
    //sem_t    *h_event_start_dec_row[16];
    //U32_WMV  do_horizontal_deblock;  // 1 horizontal; 0 vertical;
    //U32_WMV  do_AdjustDecRange_WMVA;
    //U32_WMV  do_IntensityCompensation;
    //U32_WMV  do_AdjustReconRange;
    //U32_WMV  do_AdjustDecRange;
	//U32_WMV  bThreadCreated;
#endif

	U32_WMV sequence_errorflag;
	U32_WMV entrypoint_errorflag;
	U32_WMV frame_errorflag;
	tWMVDimensions tDimensions;
	U32_WMV ntotal_frames;
#ifdef USE_FRAME_THREAD
	VO_U32 isThreadCtx;
    VO_VOID* pThreadCtx[MAX_THREAD_NUM];
	sem_t      *pThreadStart;
	sem_t      *pHasFinishedThread;
	sem_t      *pHasHeaderDecoded;
	U32_WMV pThreadStop;
	U32_WMV pThreadStatus;	
	U32_WMV nInputFrameIndx;
	U32_WMV nOutPutting;
	U32_WMV nIsLastframe;
#endif

	StreamingModeDataCtl g_StrmModedataCtl;

	I32_WMV headerwidth;
	I32_WMV headerheight;
	VO_BITMAPINFOHEADER   *pHeader;
	VO_U32 bNewheader;
	VO_U32 bNewSize;
	Void_WMV (*pRepeatRef0Y) (U8_WMV* ppxlcRef0Y, 
                        	I32_WMV  iStart,
                            I32_WMV  iEnd,
                        	I32_WMV     iOldLeftOffet, 
                        	Bool_WMV    fTop,
                            Bool_WMV    fBottom,
                            I32_WMV     iWidthY,
                            I32_WMV     iWidthYPlusExp,
                            I32_WMV     iWidthPrevY,
                            Bool_WMV bProgressive);
	Void_WMV (*pRepeatRef0UV) (U8_WMV* ppxlcRef0U,
                        	U8_WMV* ppxlcRef0V,
   	                        I32_WMV  iStart,
                            I32_WMV  iEnd,
	                        I32_WMV     iOldLeftOffet,
	                        Bool_WMV    fTop,
                            Bool_WMV    fBottom,
                            I32_WMV     iWidthUV,
                            I32_WMV     iWidthUVPlusExp,
                            I32_WMV     iWidthPrevUV,
                             Bool_WMV bProgressive);

	U8_WMV *pOutBuff[3];
	I32_WMV stride[3];

	U8_WMV *m_pLUT;
	U8_WMV *m_pLUTUV;

} tWMVDecInternalMember;

typedef struct {
	tWMVDecInternalMember * pThWMVDec;
	tWMVDecInternalMember * pMainWMVDec;
	I32_WMV nthreadidx;
}tWMVDecThdCtx;

//typedef struct {
//	//tYUV420Frame_WMV** m_pfrmList;
//	FIFOTYPE m_memManageFIFO;
//	tYUV420Frame_WMV* m_pfrmCurrQ;
//	tYUV420Frame_WMV* m_pfrmRef0Q;
//	tYUV420Frame_WMV* m_pfrmRef1Q;
//	tYUV420Frame_WMV* m_pfrmPostQ;
//	tYUV420Frame_WMV* m_pfrmPostPrevQ;
//	tYUV420Frame_WMV* m_pfrmRef0Process;
//	tYUV420Frame_WMV* m_pfrmMultiresQ;
//	tYUV420Frame_WMV* m_pfrmYUV411toYUV420Q;
//	U8_WMV* m_ppxliCurrQY;
//	U8_WMV* m_ppxliCurrQU;
//	U8_WMV* m_ppxliCurrQV;
//	U8_WMV* m_ppxliRef0Y;
//	U8_WMV* m_ppxliRef0U;
//	U8_WMV* m_ppxliRef0V;
//	U8_WMV* m_ppxliRef0YPlusExp;
//	U8_WMV* m_ppxliPostQY;
//	U8_WMV* m_ppxliPostQU;
//	U8_WMV* m_ppxliPostQV;
//	U8_WMV* m_ppxliPostPrevQY;
//	U8_WMV* m_ppxliPostPrevQU;
//	U8_WMV* m_ppxliPostPrevQV;
//	U8_WMV* m_ppxliMultiresY;
//	U8_WMV* m_ppxliMultiresU;
//	U8_WMV* m_ppxliMultiresV;
//	U8_WMV* m_ppxliRef1Y;
//	U8_WMV* m_ppxliRef1U;
//	U8_WMV* m_ppxliRef1V;
//	U8_WMV* m_ppxliRef1YPlusExp;
//	CMotionVector_X9_EMB * m_rgmv1_EMB;
//	Bool_WMV m_bPrevAnchor;
//	I32_WMV m_iReconRangeState;
//	I32_WMV m_iRndCtrl;
//}voGlobleFIFOInfo;
//
//voGlobleFIFOInfo  GlobleFIFO;

typedef struct ThreadCtx
{
	tWMVDecInternalMember pThVC1Dec[MAX_THREAD_NUM];
}VOTHREADCTX;

typedef struct DEBLOCKPARAM
{
    tWMVDecInternalMember *pWMVDec;
    U8_WMV* ppxliY;
    U8_WMV* ppxliU;
    U8_WMV* ppxliV;
    I32_WMV iNumRows;
    I32_WMV iNumCols;
    I32_WMV iHeight;
    U8_WMV* puchFilterFlagY;
    U8_WMV* puchFilterFlagU;
    U8_WMV* puchFilterFlagV;
    U8_WMV* puchFilterFlagY8x4;
    U8_WMV* puchFilterFlagU8x4;
    U8_WMV* puchFilterFlagV8x4;
}twmvDeBlockMember;

#include "dectable_c.h"

#endif // __WMVDEC_MEMBER_H_
