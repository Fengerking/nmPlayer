/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
	File:		voWMVVC1Parser.h

	Contains:	 WMVVC1 Parser Header File

	Written by:	Zhihuang Zou

	Change History (most recent first):
	2012-02-14		ZZH			Create file

*******************************************************************************/
#ifndef  __VO_VC1_PARSER_H__
#define  __VO_VC1_PARSER_H__
#include "voParser.h"
#include "voVideoParser.h"
#include "voVideoParserDecID.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TRUE_WMV    1
#define FALSE_WMV   0
#define ASFBINDING_SIZE 1
#define SC_SEQ          0x0F
#define SC_ENTRY        0x0E
#define SC_FRAME        0x0D
#define SC_FIELD        0x0C
#define SC_SLICE        0x0B
#define SC_ENDOFSEQ     0x0A
#define SC_SEQ_DATA     0x1F
#define SC_ENTRY_DATA   0x1E
#define SC_FRAME_DATA   0x1D
#define SC_FIELD_DATA   0x1C
#define SC_SLICE_DATA   0x1B

#define NUMBITS_SLICE_SIZE_WMV2         3

typedef enum tagFrameType_WMV
{
      IVOP = 0, 
      PVOP,
      BVOP,
      SPRITE
      , BIVOP,
      SKIPFRAME
} tFrameType_WMV;


#define MAKEFOURCC_WMV(ch0, ch1, ch2, ch3) \
        ((VO_U32)(VO_U8)(ch0) | ((VO_U32)(VO_U8)(ch1) << 8) |   \
        ((VO_U32)(VO_U8)(ch2) << 16) | ((VO_U32)(VO_U8)(ch3) << 24 ))

#define mmioFOURCC_WMV(ch0, ch1, ch2, ch3)  MAKEFOURCC_WMV(ch0, ch1, ch2, ch3)

#define FOURCC_WMVA_WMV     mmioFOURCC_WMV('W','M','V','A')
#define FOURCC_wmva_WMV     mmioFOURCC_WMV('w','m','v','a')
#define FOURCC_WMVP_WMV     mmioFOURCC_WMV('W','M','V','P')
#define FOURCC_wmvp_WMV     mmioFOURCC_WMV('w','m','v','p')
#define FOURCC_WVP2_WMV     mmioFOURCC_WMV('W','V','P','2')
#define FOURCC_wvp2_WMV     mmioFOURCC_WMV('w','v','p','2')
#define FOURCC_WMV3_WMV     mmioFOURCC_WMV('W','M','V','3')
#define FOURCC_wmv3_WMV     mmioFOURCC_WMV('w','m','v','3')
#define FOURCC_WMV2_WMV     mmioFOURCC_WMV('W','M','V','2')
#define FOURCC_wmv2_WMV     mmioFOURCC_WMV('w','m','v','2')
#define FOURCC_WMV1_WMV     mmioFOURCC_WMV('W','M','V','1')
#define FOURCC_wmv1_WMV     mmioFOURCC_WMV('w','m','v','1')
#define FOURCC_M4S2_WMV     mmioFOURCC_WMV('M','4','S','2')
#define FOURCC_m4s2_WMV     mmioFOURCC_WMV('m','4','s','2')
#define FOURCC_MP43_WMV     mmioFOURCC_WMV('M','P','4','3')
#define FOURCC_mp43_WMV     mmioFOURCC_WMV('m','p','4','3')
#define FOURCC_MP4S_WMV     mmioFOURCC_WMV('M','P','4','S')
#define FOURCC_mp4s_WMV     mmioFOURCC_WMV('m','p','4','s')
#define FOURCC_MP42_WMV     mmioFOURCC_WMV('M','P','4','2')
#define FOURCC_mp42_WMV     mmioFOURCC_WMV('m','p','4','2')
#define FOURCC_WVC1_WMV     mmioFOURCC_WMV('W','V','C','1')
#define FOURCC_wvc1_WMV     mmioFOURCC_WMV('w','v','c','1')

typedef struct tagCInputBitStream_WMV {
    VO_U32 m_uBitMask;     // bit mask contains at least 16 bits all the time (until we have less than 16 bits left)
    VO_S32 m_iBitsLeft;    // number of bits available minus 16
    VO_U8 *m_pCurr;        // current data pointer
    VO_U8 *m_pLast;        // address of last available byte
}WMVBitStream;

typedef struct tagWMVPRIVATE {
	WMVBitStream *pBS;
	VO_U32 uStartCode;
	VO_U32 iCodecVer;
	VO_U32 nChromaFormat;
	VO_U32 nFrameRate;
	VO_U32 nBitRate;
	VO_U32 nPostProcInfoPresent;
	VO_U32 nBroadcastFlags;
    VO_U32 nInterlacedSource;
    VO_U32 nTemporalFrmCntr;
    VO_U32 nSeqFrameInterpolation;
	VO_S32 nYUV411;
	VO_S32 nSpriteMode;
	VO_U32 nLoopFilter;
	VO_U32 nXintra8Switch;
    VO_U32 nMultiresEnabled;
    VO_U32 n16bitXform; 
    VO_U32 nUVHpelBilinear;
    VO_U32 nExtendedMvMode;
    VO_U32 nDQuantCodingOn;
	VO_U32 nformSwitch;
    VO_U32 nDCTTable_MB_ENABLED; 
    VO_U32 nSequenceOverlap;
    VO_U32 nStartCode;
    VO_U32 nPreProcRange;
    VO_U32 nNumBFrames;
	VO_U32 nExplicitSeqQuantizer;
	VO_U32 nUse3QPDZQuantizer;
	VO_U32 nExplicitFrameQuantizer;
	VO_U32 nExplicitQuantizer;
	VO_U32 nSliceCode;
	VO_U32 nFrmHybridMVOn;
	VO_U32 nXformSwitch;
	VO_U32 nMixedPel;
	VO_U32 nRndCtrlOn;
	VO_U32 nInterpolateCurrentFrame;
	VO_U32 nRangeState;
	VO_U32 nRefFrameNum;
	VO_U32 nIsBChangedToI;
	VO_U32 nFrmPredType;
	VO_U32 nNumMotionVectorSets;
}WMVPRIVATE;

VO_S32 voWMVVC1Init(VO_HANDLE *pParHandle);
VO_S32 voWMVVC1Parser(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData);
VO_S32 voWMVVC1GetInfo(VO_HANDLE pParHandle,VO_S32 nID,VO_PTR pValue);
VO_S32 voWMVVC1Uninit(VO_HANDLE pParHandle);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VO_WMVVC1PARSER_H__

