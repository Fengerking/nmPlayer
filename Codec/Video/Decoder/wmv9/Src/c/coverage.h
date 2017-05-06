//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#ifndef _COVERAGE_H
#define _COVERAGE_H

#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "huffman_wmv.h"
#include "tchar.h"
#include "windows.h"

typedef __int64 CVTYPE;

//
// define various types for different bit lengths of parameters
//
typedef CVTYPE    cv1b[2];
typedef CVTYPE    cv2b[4];
typedef CVTYPE    cv3b[8];
typedef CVTYPE    cv4b[16];
typedef CVTYPE    cv5b[32];
typedef CVTYPE    cv6b[64];
typedef CVTYPE    cv7b[128];
typedef CVTYPE    CVTYPE3[3];

//
// enum type for locations and center values
//
typedef enum
{
    CV_LOCATION_UPPER_LEFT = 0,
    CV_LOCATION_LOWER_LEFT,
    CV_LOCATION_UPPER_RIGHT,
    CV_LOCATION_LOWER_RIGHT,
    CV_LOCATION_CENTER,
    CV_LOCATION_OTHER,

    CV_LOCATION_NUM
} eCoverageLocation;

static char *gDcttabStrings[CV_LOCATION_NUM] = 
{
    "DcttabUpperLeft",
    "DcttabLowerLeft",
    "DcttabUpperRight",
    "DcttabLowerRight",
    "DcttabCenter",
    "DcttabOther"
};
static char *gAcpredFrameStrings[CV_LOCATION_NUM] = 
{
    "AcpredFrameUpperLeft",
    "AcpredFrameLowerLeft",
    "AcpredFrameUpperRight",
    "AcpredFrameLowerRight",
    "AcpredFrameCenter",
    "AcpredFrameOther"
};
static char *gAcpredBFieldStrings[CV_LOCATION_NUM] = 
{
    "AcpredBFieldUpperLeft",
    "AcpredBFieldLowerLeft",
    "AcpredBFieldUpperRight",
    "AcpredBFieldLowerRight",
    "AcpredBFieldCenter",
    "AcpredBFieldOther"
};
static char *gAcpredTFieldStrings[CV_LOCATION_NUM] = 
{
    "AcpredTFieldUpperLeft",
    "AcpredTFieldLowerLeft",
    "AcpredTFieldUpperRight",
    "AcpredTFieldLowerRight",
    "AcpredTFieldCenter",
    "AcpredTFieldOther"
};


typedef enum
{
    CV_DCDIFF_NEG =0,
    CV_DCDIFF_POS,
    CV_DCDIFF_ZERO,
    CV_DCDIFF_1BIT,
    CV_DCDIFF_2BIT,
    CV_DCDIFF_8BIT,
    CV_DCDIFF_9BIT,
    CV_DCDIFF_10BIT,

    CV_DCDIFF_NUM
} eCoverageDCDiff;
static char *gDcdiffStrings [CV_DCDIFF_NUM] =
{
    "Dcdiff Negative",
    "Dcdiff Positive",
    "Dcdiff Zero",
    "Dcdiff 1Bit",
    "Dcdiff 2Bits",
    "Dcdiff 8Bits",
    "Dcdiff 9Bits",
    "Dcdiff 10Bits"
};

typedef enum
{
    CV_ACCOEF_ESC1VLC = 0,
    CV_ACCOEF_ESC01VLC,
    CV_ACCOEF_ESC00FLC,

    CV_ACCOEF_NUM
} eCoverageAccoefEscape;

//
// define types for various tables
//
typedef enum
{
/* 00 */CV_HufMVTable_Talking = 0,
        CV_HufMVTable_HghMt,

        CV_HufICBPCYTable,                // iCBPCY has a general table as well as specific ones for each of the following
        CV_HufICBPCYTable_UpperLeft,
        CV_HufICBPCYTable_LowerLeft,
        CV_HufICBPCYTable_UpperRight,
        CV_HufICBPCYTable_LowerRight,
        CV_HufICBPCYTable_Center,

/* 08 */CV_HufPCBPCYTable,
        CV_HufPCBPCYTable_UpperLeft,
        CV_HufPCBPCYTable_LowerLeft,
        CV_HufPCBPCYTable_UpperRight,
        CV_HufPCBPCYTable_LowerRight,
        CV_HufPCBPCYTable_Center,

        CV_HufPCBPCYTable_HighRate,
        CV_HufPCBPCYTable_MidRate,
/* 10 */CV_HufPCBPCYTable_LowRate,
        CV_HufDCTDCyTable_Talking,
        CV_HufDCTDCcTable_Talking,
        CV_HufDCTDCyTable_HghMt,
        CV_HufDCTDCcTable_HghMt,
        CV_HufDCTACInterTable_HghMt,
        CV_HufDCTACIntraTable_HghMt,
        CV_HufDCTACInterTable_Talking,
/* 18 */CV_HufDCTACIntraTable_Talking,
        CV_HufDCTACInterTable_MPEG4,
        CV_HufDCTACIntraTable_MPEG4,
        CV_HufDCTACInterTable_HghRate,
        CV_HufDCTACIntraTable_HghRate,
        CV_HufPCBPCYTable_V9_1,
        CV_HufPCBPCYTable_V9_2,
        CV_HufPCBPCYTable_V9_3,
/* 20 */CV_HufPCBPCYTable_V9_4,
        CV_HufMVTable_V9_MV1,
        CV_HufMVTable_V9_MV2,
        CV_HufMVTable_V9_MV3,
        CV_HufMVTable_V9_MV4,
        CV_Huf4x4PatternHighQPTable,
        CV_Huf4x4PatternMidQPTable,
        CV_Huf4x4PatternLowQPTable,
/* 28 */CV_HufMBXformTypeLowQPTable,
        CV_HufMBXformTypeMidQPTable,
        CV_HufMBXformTypeHighQPTable,
        CV_HufBlkXformTypeLowQPTable,
        CV_HufBlkXformTypeMidQPTable,
        CV_HufBlkXformTypeHighQPTable,
        CV_pLhdec_m_pCodeTable,

        COVERAGE_MAX_NUM_HUFFMAN_TABLES
} eCoverageHuffmanTables;

typedef enum
{
    CV_SP = 0,        // simple profile outside normal ranges
    CV_SP_LL,
    CV_SP_ML,
    CV_MP,            // main profile outside normal ranges
    CV_MP_LL,
    CV_MP_ML,
    CV_MP_HL,
    CV_MISC,          // none of the above

    CV_NUM_PROFILES
} eCoverageProfiles;
static char *gCoverageProfileStrings [CV_NUM_PROFILES] = 
{
    "Simple General",
    "Simple Low Level",
    "Simple Med Level",
    "Main General",
    "Main Low Level",
    "Main Med Level",
    "Main High Level",
    "Miscelaneous"
};

//
// this is a little wasteful and redundant, but each huffman table will contain its
// alphabet number and name
//
typedef struct
{
    I32_WMV   alphabetSize;
    TCHAR     *name;
    CVTYPE    *coverage;
} tCoverageHuffmanTable;

typedef struct
{
    tCoverageHuffmanTable   tables [COVERAGE_MAX_NUM_HUFFMAN_TABLES];
} tCoverageHuffman;

typedef struct
{
    CVTYPE    coverage [64];    // assume max of 8x8
} tAcPred;

typedef struct
{
    cv2b      profile;
    cv1b      spritemode;
    cv1b      interlace;
    cv3b      framerate;
    cv5b      bitrate;
    cv1b      loopfilter;
    cv1b      x8intra;
    cv1b      multires;
    cv1b      fasttx;
    cv1b      fastuvmc;
    cv1b      broadcast;
    cv2b      dquant;
    cv1b      vstransform;
    cv1b      dcttabswitch;
    cv1b      overlap;
    cv1b      startcode;
    cv1b      preproc;      
    cv3b      numbframes;
    cv1b      explicitSeqQuantizer;
    cv1b      explicitFrameQuantizer;
    cv1b      use3QPDZQuantizer;
    cv1b      frameinterp;
    cv1b      rtmContent;
    cv1b      betaContent;
} tCoverageSequence;

typedef struct
{
    I32_WMV   m_iWMV3Profile;
    Bool_WMV  m_bInterlaceCodingOn;
    I32_WMV   m_bSpriteMode;
    I32_WMV   m_iFrameRate;
    I32_WMV   m_iBitRate;    
    Bool_WMV  m_bV9LoopFilter;
    Bool_WMV  m_bXintra8Switch;
    Bool_WMV  m_bMultiresEnabled;
    Bool_WMV  m_b16bitXform;
    Bool_WMV  m_bUVHpelBilinear;
    Bool_WMV  m_bBroadcastMode;
    Bool_WMV  m_iDQuantCodingOn;       // why does a 2 bit element get saved as a boolean?
    Bool_WMV  m_bXformSwitch;
    Bool_WMV  m_bDCTTable_MB_ENABLED;
    Bool_WMV  m_bSequenceOverlap;
    Bool_WMV  m_bStartCode;
    Bool_WMV  m_bPreProcRange;
    U8_WMV    m_iNumBFrames;
    Bool_WMV  m_bExplicitSeqQuantizer;
    Bool_WMV  m_bUse3QPDZQuantizer;
    Bool_WMV  m_bExplicitFrameQuantizer;
    Bool_WMV  m_bSeqFrameInterpolation;
    Bool_WMV  m_bRTMContent;
    Bool_WMV  m_bBetaContent;
    Bool_WMV  m_iBetaRTMMismatchIndex;
} tCoverageSequenceLayer;

typedef enum
{
    CV_BASELINE_I = 0,
    CV_INTERLACE_I,
    CV_X8INTRA_I,
    CV_PROGRESSIVE_P,
    CV_INTERLACE_P,
    CV_PROGRESSIVE_B,
    CV_INTERLACE_B,
    CV_PROGRESSIVE_B2I,
    CV_INTERLACE_B2I,

    CV_FRAME_TYPE_NUM
} eCoverageFrameType;

typedef enum
{
    CV_DCPRED_LEFT = 0,
    CV_DCPRED_TOP,

    CV_DCPRED_NUM
} eCoverageDcpred;
static char *gDcpredStrings [CV_DCPRED_NUM] =
{
    "DcpredLeft",
    "DcpredTop"
};

typedef enum
{
    CV_SUBBLKPATUV_RIGHT = 0,
    CV_SUBBLKPATUV_LEFT,
    CV_SUBBLKPATUV_BOTH,

    CV_SUBBLKPATUV_NUM
} eCoverageSubblkpatUV;
static char *gSubblkpatUVStrings [CV_SUBBLKPATUV_NUM] = 
{
    "subblkpatUVRight",
    "subblkpatUVLeft",
    "subblkpatUVBoth"
};

#define CV_NUMBITS_ESC_LEVEL_MAX   11
#define CV_NUMBITS_ESC_RUN_MAX     6
typedef struct
{
    cv1b                          accoefSign;
    CVTYPE                        accoefEsc [CV_ACCOEF_NUM];
    CVTYPE                        NumbitsEscLevel [CV_NUMBITS_ESC_LEVEL_MAX+1];
    CVTYPE                        NumbitsEscRun [CV_NUMBITS_ESC_RUN_MAX+1];
} tCoverageBlock;

typedef struct
{
    cv1b                          bitplaneInvert;
    CVTYPE                        bitplanCoding[7];
} tCoverageBitplane;

typedef struct
{
    cv1b                          acpredFrame[CV_LOCATION_NUM];
    CVTYPE3                       dcttab[CV_LOCATION_NUM];
    cv1b                          dcdiffSign;
    CVTYPE                        dcdiff[CV_DCDIFF_NUM];
    CVTYPE                        dcpred[CV_DCPRED_NUM];
    tCoverageBlock                block;
    CVTYPE                        acpred[64];
    CVTYPE                        coefInputBits[33];
    CVTYPE                        coefInternalBits[33];
    tCoverageHuffman              huffman;
} tCoverageBaselineIFrame;

typedef struct
{
    cv1b                          acpredFrame[CV_LOCATION_NUM];
    cv1b                          acpredBField[CV_LOCATION_NUM];
    cv1b                          acpredTField[CV_LOCATION_NUM];
    CVTYPE                        subblkpatUV[CV_SUBBLKPATUV_NUM];
    tCoverageBlock                block;
    tCoverageBitplane             fieldFrameBitplane;
    CVTYPE                        acpred[64];
    CVTYPE                        coefInputBits[33];
    CVTYPE                        coefInternalBits[33];
    tCoverageHuffman              huffman;
} tCoverageInterlaceIFrame;

typedef struct
{
    cv1b                          dcdiffSign;
    CVTYPE                        dcdiff[CV_DCDIFF_NUM];
    tCoverageBlock                block;
    CVTYPE                        acpred[64];
    CVTYPE                        coefInputBits[33];
    CVTYPE                        coefInternalBits[33];
    tCoverageHuffman              huffman;
} tCoverageX8IntraIFrame;

typedef struct
{
    CVTYPE                        xform [5];    // coordinate with typedef.h enum
    cv1b                          dcdiffSign;
    CVTYPE                        dcdiff[CV_DCDIFF_NUM];
    tCoverageBlock                block;
    CVTYPE                        acpred[64];
    CVTYPE                        coefInputBits[33];
    CVTYPE                        coefInternalBits[33];
    tCoverageHuffman              huffman;
} tCoverageProgressivePFrame;

typedef struct
{
    cv1b                          acpredFrame[CV_LOCATION_NUM];
    cv1b                          acpredBField[CV_LOCATION_NUM];
    cv1b                          acpredTField[CV_LOCATION_NUM];
    tCoverageBlock                block;
    CVTYPE                        acpred[64];
    CVTYPE                        coefInputBits[33];
    CVTYPE                        coefInternalBits[33];
    tCoverageHuffman              huffman;
} tCoverageInterlacePFrame;

typedef struct
{
    CVTYPE                        acpred[64];
    CVTYPE                        coefInputBits[33];
    CVTYPE                        coefInternalBits[33];
    tCoverageHuffman              huffman;
} tCoverageInterlaceBFrame;

typedef struct
{
    CVTYPE                        acpred[64];
    CVTYPE                        coefInputBits[33];
    CVTYPE                        coefInternalBits[33];
    tCoverageHuffman              huffman;
} tCoverageProgressiveBFrame;

typedef struct
{
    CVTYPE                        acpred[64];
    CVTYPE                        coefInputBits[33];
    CVTYPE                        coefInternalBits[33];
    tCoverageHuffman              huffman;
} tCoverageInterlaceB2IFrame;

typedef struct
{
    CVTYPE                        acpred[64];
    CVTYPE                        coefInputBits[33];
    CVTYPE                        coefInternalBits[33];
    tCoverageHuffman              huffman;
} tCoverageProgressiveB2IFrame;

typedef struct
{
    CVTYPE                        acpred[64];
    CVTYPE                        coefInputBits[33];
    CVTYPE                        coefInternalBits[33];
    tCoverageHuffman              huffman;
} tCoverageAllFrames;



typedef struct
{
    cv1b        m_bInterpolateCurrentFrame;
    cv1b        iFrmCntMod4;
    cv1b        m_iRangeState;
    cv1b        m_bIsBChangedToI;
    cv1b        m_bRefLibEnabled;
    cv1b        m_iRefFrameNum;
    CVTYPE      m_tFrmPredType[4];
    CVTYPE      m_BfractionShort[7];
    CVTYPE      m_BfractionLong[16];
    cv7b        iBf;
    cv5b        m_iQPIndex;
    cv1b        m_bHalfStep;
    cv1b        m_bUse3QPDZQuantizer;
    CVTYPE      m_MVRange [4];
    cv2b        m_iResIndexProgressive;
    cv1b        m_iResIndexInterlaced;
    cv1b        m_Xintra8;
    cv1b        m_bFieldCodingOn;
    cv1b        m_bLuminanceWarp;

    eCoverageFrameType  curFrameType;               // frame type of the current picture, set for every picture decoded.
    CVTYPE      frameType [CV_FRAME_TYPE_NUM];      // coverage stats for the picture types.
} tCoveragePicture;

//
// tCoverage collects stats for an individual input file.  This means that tCoverage is for a
// single profile (if there is one identified).  Multiple profiles are collected in the tCoverageSet
// defined below.
//
typedef struct
{
    //
    // this defines the profile type for this file
    //
    eCoverageProfiles             profileType;
    I32_WMV                       width;
    I32_WMV                       height;
    I32_WMV                       frameRate;
    I32_WMV                       numFrames;

    //
    // make sure that all elements here are based on CVTYPE
    //
    tCoverageSequence             sequence;
    tCoveragePicture              picture;

    //
    // each picture will be one of these frame types
    // and each frame type has its own coverage stats
    // for that type of frame
    //
    tCoverageBaselineIFrame       baselineI;
    tCoverageInterlaceIFrame      interlaceI;
    tCoverageX8IntraIFrame        x8intraI;
    tCoverageProgressivePFrame    progressiveP;
    tCoverageInterlacePFrame      interlaceP;
    tCoverageProgressiveBFrame    progressiveB;
    tCoverageInterlaceBFrame      interlaceB;
    tCoverageProgressiveB2IFrame  progressiveB2I;
    tCoverageInterlaceB2IFrame    interlaceB2I;

    tCoverageAllFrames            allFrames;

    //
    // now we can put in non CVTYPE elements
    //
    TCHAR                         mpgFileName [2*MAX_PATH];   // this should be 2*MAX_PATH
    tCoverageSequenceLayer        sequenceLayer;
} tCoverage;


//
// the coverage stats are collected by sequence type
//
typedef struct
{
    CVTYPE                        profileTypes [CV_NUM_PROFILES];   // collect stats on profile types in the file set
    tCoverage                     profiles [CV_NUM_PROFILES+1];     // collection for individual profiles from all files
                                                                    // the last (+1 index) profile is union of all.
} tCoverageSet;

extern tCoverage      *gpCoverage;       // pointer to tCoverage based on profile selected
extern tCoverage      gCoverageFile;     // Coverage for a specific file (only one profile type per file)
extern tCoverageSet   gCoverageSet;      // Coverage for All files in the input set.  Has stores for all profile types

extern HANDLE g_fpCoverage;

extern void CoverageInit          ();
extern void CoverageFree          ();
extern void CoverageDumpWorking   (HANDLE fpCoverage);
extern void CoverageDumpRunning   (HANDLE fpCoverage);
extern void CoverageUpdateRunning ();
extern void CoverageSequence (void *pWMVDec);
extern void CoveragePicture  (void *pWMVDec);
extern void CoverageLinkToWMVDec (TCHAR *mpgFileName, HWMVDecoder *phWMVDec);
extern void CoverageSetFrametype (tWMVDecInternalMember *pWMVDec);
extern void CoverageCreateFile (TCHAR *filename);
extern void CoverageHuffman (I32_WMV index, I32_WMV value);
extern void CoverageSpecialHuffmanICBPCY(tWMVDecInternalMember *pWMVDec, U32_WMV imbX, U32_WMV imbY, I32_WMV iCBPCY);
extern void CoverageInitHuffman (Huffman_WMV * pThis, I32_WMV index, I32_WMV numElements, TCHAR *name);
extern void CoverageDcpred (tWMVDecInternalMember *pWMVDec, I32_WMV pred);
extern void CoverageDcdiff (tWMVDecInternalMember *pWMVDec, I32_WMV siz);
extern void CoverageDcdiffSign (tWMVDecInternalMember *pWMVDec, I32_WMV value);
extern void CoverageAccoefSign (tWMVDecInternalMember *pWMVDec, I32_WMV value);
extern void CoverageAccoefEsc (tWMVDecInternalMember *pWMVDec, I32_WMV esc);
extern void CoverageAcPred (tWMVDecInternalMember *pWMVDec, I32_WMV esc);
extern void CoverageCoefInputBits (U32_WMV numValues, const I32_WMV *pValues);
extern void CoverageCoefInputBits16 (U32_WMV numValues, const I16_WMV *pValues);
extern void CoverageCoefInternalBits (I32_WMV value);
extern void CoverageBlockNumBitsEsc (tWMVDecInternalMember *pWMVDec);
extern void CoveragePXform (tWMVDecInternalMember *pWMVDec, I32_WMV iXformType);
extern void CoverageAcpredFrame(tWMVDecInternalMember *pWMVDec, U32_WMV imbX, U32_WMV imbY, I32_WMV value);
extern void CoverageAcpredTField(tWMVDecInternalMember *pWMVDec, U32_WMV imbX, U32_WMV imbY, I32_WMV value);
extern void CoverageAcpredBField(tWMVDecInternalMember *pWMVDec, U32_WMV imbX, U32_WMV imbY, I32_WMV value);
extern void CoverageDcttab(tWMVDecInternalMember *pWMVDec, U32_WMV imbX, U32_WMV imbY, I32_WMV value);
extern void CoverageSubblkpatUV(tWMVDecInternalMember *pWMVDec, U32_WMV iBlk, I32_WMV value);

#define CVOFFSET(m)                         (((unsigned int)&(((tV9Coverage*)0)->m))/sizeof(CVTYPE))
#define COVERAGE_FREE()                     CoverageFree()
#define COVERAGE_SEQUENCE(_p)               CoverageSequence(_p)
#define COVERAGE_PICTURE(_p)                CoveragePicture(_p)
#define COVERAGE_LINK_TO_WMVDEC(_m,_p)      CoverageLinkToWMVDec(_m, _p)

#define COVERAGE_CVSIZE(_m)                 (sizeof(gCoverageFile._m))
#define COVERAGE_CVNUMELEMENTS(_m)          (COVERAGE_CVSIZE(_m)/sizeof(CVTYPE))
#define COVERAGE_FILE_ADDR(_m)              ((CVTYPE*)(&gCoverageFile._m))
#define COVERAGE_SET_ADDR(_m,_pt)           ((CVTYPE*)(&gCoverageSet.profiles[_pt]._m))
#define COVERAGE_CVSET(_m,_v)               if ((U32_WMV)(_v)<COVERAGE_CVNUMELEMENTS(_m)) gCoverageFile._m [_v] ++

#define COVERAGE_SET_FRAMETYPE(_p)          CoverageSetFrametype (_p);
#define COVERAGE_INIT_FILE_HANDLE           g_fpCoverage=INVALID_HANDLE_VALUE;
#define COVERAGE_CLOSE_FILE                 if (g_fpCoverage!=INVALID_HANDLE_VALUE)                             \
                                                CloseHandle( g_fpCoverage );
#define COVERAGE_HUFFMAN(p,v)               CoverageHuffman(p->m_uiCoverageIndex, v);
#define COVERAGE_SPECIAL_ICBPCY(p,X,Y,i)    CoverageSpecialHuffmanICBPCY(p,X,Y,i);

#define COVERAGE_CLEAR_HUFFMAN(p)           p->m_uiCoverageIndex=-1
#define COVERAGE_INIT_HUFFMAN(p,i,t,n)      CoverageInitHuffman(p,i,t[0].code,n)
#define COVERAGE_INIT_HUFFMAN_ICBPCY        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufICBPCYDec, CV_HufICBPCYTable_UpperLeft,  sm_HufICBPCYTable, "HufICBPCYTable_UpperLeft");     \
                                            COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufICBPCYDec, CV_HufICBPCYTable_LowerLeft,  sm_HufICBPCYTable, "HufICBPCYTable_LowerLeft");     \
                                            COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufICBPCYDec, CV_HufICBPCYTable_UpperRight, sm_HufICBPCYTable, "HufICBPCYTable_UpperRight");    \
                                            COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufICBPCYDec, CV_HufICBPCYTable_LowerRight, sm_HufICBPCYTable, "HufICBPCYTable_LowerRight");    \
                                            COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufICBPCYDec, CV_HufICBPCYTable_Center,     sm_HufICBPCYTable, "HufICBPCYTable_Center");        \
                                            COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufICBPCYDec, CV_HufICBPCYTable,            sm_HufICBPCYTable, "HufICBPCYTable");
#define COVERAGE_INIT_HUFFMAN_PCBPCY        COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufPCBPCYDec, CV_HufPCBPCYTable_UpperLeft,  sm_HufPCBPCYTable, "HufPCBPCYTable_UpperLeft");     \
                                            COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufPCBPCYDec, CV_HufPCBPCYTable_LowerLeft,  sm_HufPCBPCYTable, "HufPCBPCYTable_LowerLeft");     \
                                            COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufPCBPCYDec, CV_HufPCBPCYTable_UpperRight, sm_HufPCBPCYTable, "HufPCBPCYTable_UpperRight");    \
                                            COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufPCBPCYDec, CV_HufPCBPCYTable_LowerRight, sm_HufPCBPCYTable, "HufPCBPCYTable_LowerRight");    \
                                            COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufPCBPCYDec, CV_HufPCBPCYTable_Center,     sm_HufPCBPCYTable, "HufPCBPCYTable_Center");        \
                                            COVERAGE_INIT_HUFFMAN(&pWMVDec->m_hufPCBPCYDec, CV_HufPCBPCYTable,            sm_HufPCBPCYTable, "HufPCBPCYTable");
#define COVERAGE_INIT_FILE_STATS(_n)        memset ((void*)(&gCoverageFile), 0, sizeof (gCoverageFile)); \
                                            gCoverageFile.numFrames=_n;
#define COVERAGE_UPDATE(_fp)                CoverageDumpWorking(_fp); \
                                            CoverageUpdateRunning();
#define COVERAGE_ACPRED_FRAME(p,X,Y,v)      CoverageAcpredFrame(p,X,Y,v)
#define COVERAGE_ACPRED_TFIELD(p,X,Y,v)     CoverageAcpredTField(p,X,Y,v)
#define COVERAGE_ACPRED_BFIELD(p,X,Y,v)     CoverageAcpredBField(p,X,Y,v)
#define COVERAGE_DCTTAB(p,X,Y,v)            CoverageDcttab(p,X,Y,v)
#define COVERAGE_DCDIFF_SIGN(p,s)           CoverageDcdiffSign(p,s)
#define COVERAGE_DCDIFF(p,s)                CoverageDcdiff(p,s)
#define COVERAGE_DCPRED(p,v)                CoverageDcpred(p,v)
#define COVERAGE_ACCOEFSIGN(p,s)            CoverageAccoefSign(p,s)
#define COVERAGE_ACCOEFESC(p,e)             CoverageAccoefEsc(p,e)
#define COVERAGE_ACPRED(p,e)                CoverageAcPred(p,e)
#define COVERAGE_COEF_INPUT_BITS(n,p)       CoverageCoefInputBits(n,p)
#define COVERAGE_COEF_INPUT_BITS16(n,p)     CoverageCoefInputBits16(n,p)
#define COVERAGE_COEF_INTERNAL_BITS(v)      CoverageCoefInternalBits(v)
#define COVERAGE_BLOCK_NUMBITSESC(p)        CoverageBlockNumBitsEsc(p)
#define COVERAGE_PXFORM(p,x)                CoveragePXform(p,x)
#define COVERAGE_SUBBLKPATUV(p,i,v)         CoverageSubblkpatUV(p,i,v)

#endif // _COVERAGE_H
