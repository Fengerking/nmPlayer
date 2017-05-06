/****************************************************************************
*
*   Module Title :     pbdll.h
*
*   Description  :     Decoder definition header file.
*
****************************************************************************/
#ifndef __INC_GEN_PBDLL_H
#define __INC_GEN_PBDLL_H

/****************************************************************************
*  Module statics.
****************************************************************************/

/****************************************************************************
*  Header Files
****************************************************************************/
#include "codec_common.h"
#include "yv12extend.h"

#include "huffman.h"
#include "tokenentropy.h"
#include "voVP6back.h"

#include "quantize.h"
#include "boolhuff.h"

#if  defined(VOARMV4) || defined(VOWMMX)
#define IDCTvp6_Block1x1a VP6DEC_VO_Armv4IdctC
#define IDCTvp6_Block4x4a VP6DEC_VO_Armv4IdctA
#define IDCTvp6_Block8x8a VP6DEC_VO_Armv4IdctB
// this case the src is not alignment
#define IDCTvp6_Block1x1 IDCTvp6_Block1x1_c
#define IDCTvp6_Block4x4 IDCTvp6_Block4x4_c
#define IDCTvp6_Block8x8 IDCTvp6_Block8x8_c
#define FilterBlock1dBil_wRecon	FilterBlock1dBil_wRecon_C
#define FilterBlock2dBil_wRecon	FilterBlock2dBil_wRecon_C
#define FilterBlock1d_wRecon	FilterBlock1d_wRecon_C
#define FilterBlock2d_wRecon	FilterBlock2d_wRecon_C

#define Copy16x12 Copy16x12_Armv4
#define VP6_FilteringHoriz_12	VP6_FilteringHoriz_12_C
#define VP6_FilteringVert_12	VP6_FilteringVert_12_C
#elif defined(VOARMV6)
#define IDCTvp6_Block1x1a	VP6DEC_VO_Armv6IdctC
#define IDCTvp6_Block4x4a	VP6DEC_VO_Armv6IdctA
#define IDCTvp6_Block8x8a	VP6DEC_VO_Armv6IdctB

// this case the src is not alignment
#define IDCTvp6_Block1x1		IDCTvp6_Block1x1_c
#define IDCTvp6_Block4x4		IDCTvp6_Block4x4_c
#define IDCTvp6_Block8x8		IDCTvp6_Block8x8_c

#define FilterBlock1dBil_wRecon		FilterBlock1dBil_wRecon_Armv6
#define FilterBlock2dBil_wRecon		FilterBlock2dBil_wRecon_Armv6
#define FilterBlock1d_wRecon			FilterBlock1d_wRecon_Armv6
#define FilterBlock2d_wRecon			FilterBlock2d_wRecon_Armv6

#define Copy16x12						Copy16x12_Armv6
#define VP6_FilteringHoriz_12			VP6_FilteringHoriz_12_C
#define VP6_FilteringVert_12			VP6_FilteringVert_12_C
#elif defined(VOARMV7)
#define IDCTvp6_Block1x1a VP6DEC_VO_Armv7IdctC
#define IDCTvp6_Block4x4a VP6DEC_VO_Armv7IdctA
#define IDCTvp6_Block8x8a VP6DEC_VO_Armv7IdctB

#define IDCTvp6_Block1x1 VP6DEC_VO_Armv7IdctC
#define IDCTvp6_Block4x4 VP6DEC_VO_Armv7IdctA
#define IDCTvp6_Block8x8 VP6DEC_VO_Armv7IdctB
#define FilterBlock1dBil_wRecon	FilterBlock1dBil_wRecon_Armv7
#define FilterBlock2dBil_wRecon	FilterBlock2dBil_wRecon_Armv7
#define FilterBlock1d_wRecon	FilterBlock1d_wRecon_Armv7
#define FilterBlock2d_wRecon	FilterBlock2d_wRecon_Armv7

#define Copy16x12 Copy16x12_Armv7
#define VP6_FilteringHoriz_12	VP6_FilteringHoriz_12_C //VP6_FilteringHoriz_12_Armv7
#define VP6_FilteringVert_12	VP6_FilteringVert_12_C //VP6_FilteringVert_12_Armv7
#else
// Note the IDCTvp6_Block1x1_c can't be replaced by 4x4 or 8x8
#define IDCTvp6_Block1x1 IDCTvp6_Block1x1_c
#define IDCTvp6_Block4x4 IDCTvp6_Block4x4_c
#define IDCTvp6_Block8x8 IDCTvp6_Block8x8_c

#define IDCTvp6_Block1x1a IDCTvp6_Block1x1_c
#define IDCTvp6_Block4x4a IDCTvp6_Block4x4_c
#define IDCTvp6_Block8x8a IDCTvp6_Block8x8_c
#define FilterBlock1dBil_wRecon	FilterBlock1dBil_wRecon_C
#define FilterBlock2dBil_wRecon	FilterBlock2dBil_wRecon_C
#define FilterBlock1d_wRecon	FilterBlock1d_wRecon_C
#define FilterBlock2d_wRecon	FilterBlock2d_wRecon_C

#define Copy16x12 Copy16x12_C
#define VP6_FilteringHoriz_12	VP6_FilteringHoriz_12_C
#define VP6_FilteringVert_12	VP6_FilteringVert_12_C
#endif

#define MAXFRAMES		2

/****************************************************************************
*  MACROS
****************************************************************************/
#define Clamp255(x)	(UINT8) ( (x) < 0 ? 0 : ( (x) <= 255 ? (x) : 255 ) )

#define VAL_RANGE 256

// Enumeration of how block is coded
// VP6.2 version is >= 8
#define CURRENT_ENCODE_VERSION  8
#define CURRENT_DECODE_VERSION  8

#define SIMPLE_PROFILE			0
#define PROFILE_1				1
#define PROFILE_2				2
#define ADVANCED_PROFILE		3

// Loop filter options
#define NO_LOOP_FILTER			0
#define LOOP_FILTER_BASIC		2
#define LOOP_FILTER_DERING		3

#define UMV_BORDER              48
#define STRIDE_EXTRA            (UMV_BORDER * 2)
#define BORDER_MBS				(UMV_BORDER>>4)

#define MAX_MV_EXTENT           63      //  Max search distance in half pixel increments
#define MV_ENTROPY_TOKENS       511     
#define LONG_MV_BITS            8

#define PPROC_QTHRESH           64

#define MAX_MODES               10

#define MAX_NEAREST_ADJ_INDEX	2 

#define Y_MVSHIFT       0x2 
#define UV_MVSHIFT      0x3
#define Y_MVMODMASK     0x3
#define UV_MVMODMASK    0x7

//    INT32  MvShift;                 // motion vector shift value
//    INT32  MvModMask; 

// Prediction filter modes:
// Note: when trying to use an enum here we ran into an odd compiler bug in
// the WriteFrameHeader() code. Also an enum type is implicitly an int which 
// is a bit big for something that can only have 3 values
#define BILINEAR_ONLY_PM	    0
#define BICUBIC_ONLY_PM		    1
#define AUTO_SELECT_PM		    2

#define DCProbOffset(A,B) \
	( (A) * (MAX_ENTROPY_TOKENS-1) \
    + (B) )

#define ACProbOffset(A,B,C,D) \
	( (A) * PREC_CASES * VP6_AC_BANDS * (MAX_ENTROPY_TOKENS-1) \
	+ (B) * VP6_AC_BANDS * (MAX_ENTROPY_TOKENS-1) \
	+ (C) * (MAX_ENTROPY_TOKENS-1) \
	+ (D) ) 

#define DcNodeOffset(A,B,C) \
	( (A) * DC_TOKEN_CONTEXTS * CONTEXT_NODES \
	+ (B) * CONTEXT_NODES \
	+ (C) ) 


#define MBOffset(row,col) ( (row) * pbi->MBCols + (col) )

/****************************************************************************
*  Types
****************************************************************************/
#ifndef HAVE_CODING_MODE
typedef enum
{
    CODE_INTER_NO_MV        = 0x0,      // INTER prediction, (0,0) motion vector implied.
    CODE_INTRA              = 0x1,      // INTRA i.e. no prediction.
    CODE_INTER_PLUS_MV      = 0x2,      // INTER prediction, non zero motion vector.
    CODE_INTER_NEAREST_MV   = 0x3,      // Use Last Motion vector
    CODE_INTER_NEAR_MV      = 0x4,      // Prior last motion vector
    CODE_USING_GOLDEN       = 0x5,      // 'Golden frame' prediction (no MV).
    CODE_GOLDEN_MV          = 0x6,      // 'Golden frame' prediction plus MV.
    CODE_INTER_FOURMV       = 0x7,      // Inter prediction 4MV per macro block.
    CODE_GOLD_NEAREST_MV    = 0x8,      // Use Last Motion vector
    CODE_GOLD_NEAR_MV       = 0x9,      // Prior last motion vector
    DO_NOT_CODE             = 0x10       // Fake Mode
} CODING_MODE;
#endif

typedef struct _DCINFO
{
    Q_LIST_ENTRY dc;
    short frame;
} DCINFO;

// defined so i don't have to remember which block goes where
typedef enum
{
    TOP_LEFT_Y_BLOCK        = 0,
    TOP_RIGHT_Y_BLOCK       = 1,
    BOTTOM_LEFT_Y_BLOCK     = 2,
    BOTTOM_RIGHT_Y_BLOCK    = 3,
    U_BLOCK                 = 4,
    V_BLOCK                 = 5
} BLOCK_POSITION;

// all the information gathered from a block to be used as context in the next block
#ifndef HAVE_BLOCK_CONTEXT
#if 0
typedef struct
{
    UINT8        Token;
    CODING_MODE  Mode;
    UINT16       Frame;
    Q_LIST_ENTRY Dc;
    UINT8        unused[3];
}  BLOCK_CONTEXT;
#else
//sizeof(BLOCK_CONTEXT) must always be equal to 4 
typedef struct
{
    UINT8        Token;
    UINT8       Frame;
    Q_LIST_ENTRY Dc;
}  BLOCK_CONTEXT;
#endif
#endif

// all the contexts maintained for a frame
typedef struct
{
    BLOCK_CONTEXT    LeftY[2];   // 1 for each block row in a macroblock
    BLOCK_CONTEXT    LeftU;
    BLOCK_CONTEXT    LeftV;

    BLOCK_CONTEXT   *AboveY;
    BLOCK_CONTEXT   *AboveU;
    BLOCK_CONTEXT   *AboveV;

    Q_LIST_ENTRY     LastDcY[4]; // 1 for each frame 
    Q_LIST_ENTRY     LastDcU[4];
    Q_LIST_ENTRY     LastDcV[4];

} FRAME_CONTEXT;

#ifndef HAVE_BLOCK_DX_INFO
typedef struct
{
    INT16 *dequantPtr;
    INT16 *coeffsPtr;
    //INT16 *idctOutputPtr;

    INT32  MvShift;                 // motion vector shift value
    INT32  MvModMask;               // motion vector mod mask

    INT32  FrameReconStride;        // Stride of the frame
    INT32  CurrentReconStride;      // pitch of reconstruction

    INT32  CurrentSourceStride;     // pitch of source (compressor only)
	INT32  FrameSourceStride;		// Stride of the frame (compressor only)

    BLOCK_CONTEXT  *Above;          // above block context
    BLOCK_CONTEXT  *Left;           // left block context
    Q_LIST_ENTRY   *LastDc;         // last dc value seen

    UINT32 thisRecon;               // index for recon
    UINT32 Source;                  // index for source (compressor only)
	INT16 *DCT_codes;				// buffer of fdct results
	INT16 *DCTDataBuffer;			// input data for fdct

    UINT32 EobPos;

	UINT8	*BaselineProbsPtr;
	UINT8	*ContextProbsPtr;

	UINT8	*AcProbsBasePtr; 
	UINT8	*DcProbsBasePtr; 
	UINT8	*DcNodeContextsBasePtr; 
    UINT8	*ZeroRunProbsBasePtr;

    CODING_MODE   BlockMode;
    MOTION_VECTOR Mv;

//    BOOL_CODER *br; 
//    INT32	token;
//    UINT8 *MergedScanOrder;
//    UINT8 *MergedScanOrderPtr;

}BLOCK_DX_INFO;
#endif

typedef struct
{
    BOOL_CODER *br;

    BLOCK_DX_INFO blockDxInfo[6];

    CODING_MODE   Mode;             // mode macroblock coded as
	INT32         NearestMvIndex;   // Indicates how neare nearest is.
    MOTION_VECTOR NearestInterMVect;// nearest mv in last frame
    MOTION_VECTOR NearInterMVect;   // near mv in last frame
    MOTION_VECTOR NearestGoldMVect; // nearest mv in gold frame
    MOTION_VECTOR NearGoldMVect;    // near mv in gold frame
	INT32         NearestGMvIndex;  // Indicates how neare nearest is.

	INT32  reserved;				

    INT16 *coeffsBasePtr;

} MACROBLOCK_INFO;

// Frame Header type
typedef struct FRAME_HEADER
{
    UINT8 *buffer;
    UINT32 value;
    INT32  bits_available;
    UINT32 pos;
} FRAME_HEADER;

typedef struct _BITREADER
{
	int bitsinremainder;				// # of bits still used in remainder
	UINT32 remainder;					// remaining bits from original long
	const UINT8 * position;		// character pointer position within data
	const UINT8 * position_end;		// character pointer position within data
	UINT32 errorflag;
} BITREADER;

typedef struct 
{
	// per frame information passed in
	INT32		 PostProcessingLevel;	// level of post processing to perform 
	INT32		 FrameQIndex;			// q index value used on passed in frame

	// per block info maintained by postprocessor
	INT32		*FragmentVariances;		// block's pseudo variance : allocated and filled

	// filter specific vars
    INT32		*BoundingValuePtr;		// pointer to a filter     
	INT32		*FiltBoundingValue;		// allocated (512 big)

	// deblocker specific vars
    INT32		*DeblockValuePtr;		// pointer to a filter     
	INT32		*DeblockBoundingValue;	// allocated (512 big)
    
//    UINT32		 UnitFragments;			// number of total fragments y+u+v 

    
    // PlaneAddNoise specific data
    char		GaussianCharDist[300];	// gaussian distribution lookup table
	UINT32		GaussianCharDistQ;		// Q value for this distribution
			
} VP6_POSTPROC_INSTANCE;

typedef struct HUFF_INSTANCE
{
	// Huffman code tables for DC, AC & Zero Run Length
	UINT32 DcHuffCode[2][MAX_ENTROPY_TOKENS];
	UINT8  DcHuffLength[2][MAX_ENTROPY_TOKENS];
	UINT32 DcHuffProbs[2][MAX_ENTROPY_TOKENS];
	HUFF_NODE DcHuffTree[2][MAX_ENTROPY_TOKENS];

	UINT32 AcHuffCode[PREC_CASES][2][VP6_AC_BANDS][MAX_ENTROPY_TOKENS];
	UINT8  AcHuffLength[PREC_CASES][2][VP6_AC_BANDS][MAX_ENTROPY_TOKENS];
	UINT32 AcHuffProbs[PREC_CASES][2][VP6_AC_BANDS][MAX_ENTROPY_TOKENS];
	HUFF_NODE AcHuffTree[PREC_CASES][2][VP6_AC_BANDS][MAX_ENTROPY_TOKENS];

	UINT32 ZeroHuffCode[ZRL_BANDS][ZERO_RUN_PROB_CASES];
	UINT8  ZeroHuffLength[ZRL_BANDS][ZERO_RUN_PROB_CASES];
	UINT32 ZeroHuffProbs[ZRL_BANDS][ZERO_RUN_PROB_CASES];
	HUFF_NODE ZeroHuffTree[ZRL_BANDS][ZERO_RUN_PROB_CASES];

	/* FAST look-up-table for huffman Trees */
	UINT16 DcHuffLUT[2][1<<HUFF_LUT_LEVELS];
	UINT16 AcHuffLUT[PREC_CASES][2][VP6_AC_BANDS][1<<HUFF_LUT_LEVELS];
	UINT16 ZeroHuffLUT[ZRL_BANDS][1<<HUFF_LUT_LEVELS];

	// Counters for runs of zeros at DC & EOB at first AC position in Huffman mode
	INT32  CurrentDcRunLen[2];
	INT32  CurrentAc1RunLen[2];

} HUFF_INSTANCE;

#define FIFO_WRITE	0
#define FIFO_READ	1
#define FIFO_NUM	(MAXFRAMES+1)

typedef struct {
  YV12_BUFFER_CONFIG *img_seq[FIFO_NUM];     /* Circular buffer */
  UINT32 w_idx;				/* Index of write */
  UINT32 r_idx;				/* Index of read */
}FIFOTYPE;

// Playback Instance Definition
typedef struct PB_INSTANCE
{
	MACROBLOCK_INFO  mbi;		// all the information needed for one macroblock
	FRAME_CONTEXT    fc;		// all of the context information needed for a frame
	QUANTIZER	    *quantizer;

    // Should be able to delete these entries when VP5 complete
//remove	INT32      CodedBlockIndex;		   
//remove	UINT8	  *DataOutputInPtr;		  

    /* Current access points fopr input and output buffers */
    BOOL_CODER br;
	BOOL_CODER br2;
    BITREADER  br3;

	// Decoder and Frame Type Information
	UINT8   Vp3VersionNo;
	UINT8	VpProfile;

	UINT32  PostProcessingLevel;	   /* Perform post processing */
	//UINT32  ProcessorFrequency;	   /* CPU frequency	*/
	UINT32  CPUFree;
	UINT8   FrameType;       

	CONFIG_TYPE Configuration;	// frame configuration
//remove	UINT32  CurrentFrameSize;

	UINT32  YPlaneSize;  
	UINT32  UVPlaneSize;  

	UINT32  VFragments;
	UINT32  HFragments;
	UINT32  UnitFragments;
	UINT32  YPlaneFragments;
	UINT32  UVPlaneFragments;
	
//remove    UINT32  ReconYPlaneSize;
//remove	UINT32  ReconUVPlaneSize;
	
	UINT32  YDataOffset;
	UINT32  UDataOffset;
	UINT32  VDataOffset;

	UINT32  ReconYDataOffset;
	UINT32  ReconUDataOffset;
	UINT32  ReconVDataOffset;

	UINT32  MacroBlocks;	// Number of Macro-Blocks in Y component
	UINT32  MBRows;			// Number of rows of MacroBlocks in a Y frame
	UINT32  MBCols;			// Number of cols of MacroBlocks in a Y frame

    UINT32	OutputWidth;
    UINT32	OutputHeight;
	
	// Frame Buffers 
	YUV_BUFFER_ENTRY *ThisFrameRecon;
	YUV_BUFFER_ENTRY *GoldenFrame; 
	YUV_BUFFER_ENTRY *LastFrameRecon;
	YUV_BUFFER_ENTRY *RefFrameReconHigh;
	YUV_BUFFER_ENTRY *RefFrameReconLow;

//remove    Q_LIST_ENTRY *quantized_list;  
	INT16		 *TmpDataBuffer;
    
	UINT8		 *LoopFilteredBlock;

    //used by compressor only
    //remove later
//remove    void (**idct)(INT16 *InputData, INT16 *QuantMatrix, INT16 * OutputData );
    //used by compressor only

	VP6_POSTPROC_INSTANCE *    postproc;

    CODING_MODE      LastMode;      // Last Mode decoded;

	UINT8 DcProbs[2*(MAX_ENTROPY_TOKENS-1)];
	UINT8 AcProbs[2*PREC_CASES*VP6_AC_BANDS*(MAX_ENTROPY_TOKENS-1)];

                               //3             MAX_ENTROPY_TOKENS-7                     
//	UINT8 DcNodeContexts[2][DC_TOKEN_CONTEXTS][CONTEXT_NODES];								// Plane, Contexts, Node
	UINT8 DcNodeContexts[2 * DC_TOKEN_CONTEXTS * CONTEXT_NODES];								// Plane, Contexts, Node
	
	UINT8 ZeroRunProbs[ZRL_BANDS][ZERO_RUN_PROB_CASES];

    UINT8 MergedScanOrder[BLOCK_SIZE + 65];
	UINT8 ModifiedScanOrder[BLOCK_SIZE];
	UINT8 EobOffsetTable[BLOCK_SIZE];
	UINT8 ScanBands[BLOCK_SIZE];

	UINT8  PredictionFilterMode;
	UINT8  PredictionFilterMvSizeThresh;
	UINT32 PredictionFilterVarThresh;
	UINT8  PredictionFilterAlpha;
	
	UINT32 RefreshGoldenFrame;

	UINT32 AvgFrameQIndex;
    
    UINT32 mvNearOffset[16];
	
	int reserved;

	char *predictionMode;
	MOTION_VECTOR *MBMotionVector;

	UINT8  MvSignProbs[2];
	UINT8  IsMvShortProb[2];
	UINT8  MvShortProbs[2][7];
	UINT8  MvSizeProbs[2][LONG_MV_BITS];

	UINT8 probXmitted[4][2][MAX_MODES];
	UINT8 probModeSame[4][MAX_MODES];
	UINT8 probMode[4][MAX_MODES][MAX_MODES-1]; // nearest+near,nearest only, nonearest+nonear, 10 preceding modes, 9 nodes

	UINT32 maxTimePerFrame;
	UINT32 thisDecodeTime;
	UINT32 avgDecodeTime;
	UINT32 avgPPTime[10];
	UINT32 avgBlitTime;

	// Does this frame use multiple data streams
	// Multistream is implicit for SIMPLE_PROFILE
	UINT32   MultiStream;

    // Second partition buffer details
    FRAME_HEADER Header;
    UINT32 Buff2Offset;

	UINT32			UseHuffman;	
	HUFF_INSTANCE	*huff;

    // Should we do loop filtering.
	// In simple profile this is ignored and there is no loop filtering	
	UINT8  UseLoopFilter;

    UINT32 BlackClamp;
    UINT32 WhiteClamp;


	UINT32 AddNoiseMode;


//    YV12_BUFFER_CONFIG *scaleYV12Config;
//    YV12_BUFFER_CONFIG *postprocYV12Config;
//    YV12_BUFFER_CONFIG *intermediateYV12Config;
    UINT32 LastFrameFlag;
    UINT32 ThisFrameFlag;
	YV12_BUFFER_CONFIG FrameYV12Config[MAXFRAMES];
    YV12_BUFFER_CONFIG *lastFrameYV12Config;
    YV12_BUFFER_CONFIG *thisFrameYV12Config;
    YV12_BUFFER_CONFIG goldenFrameYV12Config;
	FIFOTYPE frame_buf_fifo;
	UINT32 frameNumber;
	UINT32 frame_buf_num;
	voint64_t time_cur;

	UINT32 first_frametype;
	UINT32 keyframe_error;

} PB_INSTANCE;


/****************************************************************************
*  global function ptrs includes
****************************************************************************/
#include "dec_gfuncs.h" 


/****************************************************************************
*  Constant table Exports
****************************************************************************/
extern const INT32 BilinearFilters[8][2];
extern const INT32 BicubicFilterSet[17][8][4]; 

extern const UINT32 VP6_Mode2Frame[DO_NOT_CODE];
extern const UINT8  DefaultScanBands[BLOCK_SIZE]; 
extern const INT32  VP6_CoeffToBand[65];
extern BOOL         VP6_ModeUsesMC[MAX_MODES]; // table to indicate if the given mode uses motion estimation


/****************************************************************************
*  Exports
****************************************************************************/
extern UINT8 LimitVal_VP6[VAL_RANGE * 3];




extern void VP6C_VPInitLibrary(void);

extern void VP6_DeleteTmpBuffers ( PB_INSTANCE *pbi );
extern INT32 VP6_AllocateTmpBuffers ( PB_INSTANCE *pbi );

extern PB_INSTANCE *VP6_CreatePBInstance ( void );
extern void		    VP6_DeletePBInstance ( PB_INSTANCE** );
extern INT32        VP6_LoadFrameHeader ( PB_INSTANCE *pbi, UINT8 *Buffer , UINT32 bufferlen,const UINT32 frame_type_flag);
extern void	        VP6_SetFrameType ( PB_INSTANCE *pbi, UINT8 FrType );

extern INT32 VP6_InitFrameDetails ( PB_INSTANCE *pbi );
extern void	        VP6_ErrorTrap ( PB_INSTANCE *pbi, INT32 ErrorCode );
extern BOOL	        VP6_AllocateFragmentInfo ( PB_INSTANCE *pbi );

extern void	        VP6_DeleteFragmentInfo ( PB_INSTANCE *pbi );
extern void	        VP6_DeleteFrameInfo ( PB_INSTANCE *pbi );
extern void	        VP6_DMachineSpecificConfig ( void );
extern UINT32	    VP6_bitread1 ( BOOL_CODER *br ) ;
extern UINT32	    VP6_bitread ( BOOL_CODER *br, INT32 bits );
extern void         vp6_appendframe ( PB_INSTANCE *pbi );
extern void         VP6_ConfigureContexts ( PB_INSTANCE *pbi );
extern void         VP6_ResetAboveContext ( PB_INSTANCE *pbi );
extern void         VP6_ResetLeftContext ( PB_INSTANCE *pbi );

extern void         VP6_PredictDC_MB ( PB_INSTANCE *pbi );

extern int VP6_PredictFiltered
(
	PB_INSTANCE *pbi,
	UINT8 *SrcPtr,
	INT32 mx,
	INT32 my,
    UINT32 bp,
	UINT8 **pTempBuffer
) ;

extern void ZerosBoolTreeToHuffCodes ( UINT8 *BoolTreeProbs, UINT32 *HuffProbs );
extern void BoolTreeToHuffCodes ( UINT8 *BoolTreeProbs, UINT32 *HuffProbs );

extern UINT32 Var16Point ( UINT8 *DataPtr, INT32 SourceStride );
extern void VP6_BuildScanOrder( PB_INSTANCE *pbi, UINT8 * );

extern void Idct_MB(PB_INSTANCE *pbi);

/****************************************************************************
*  Post Proc Exports
****************************************************************************/
//extern void (*VP6_ClampLevels)
//( 
//	INT32        BlackClamp,			// number of values to clamp from 0 
//	INT32        WhiteClamp,			// number of values to clamp from 255
//	YV12_BUFFER_CONFIG		*Src,					// reconstruction buffer : passed in
//	YV12_BUFFER_CONFIG		*Dst					// postprocessing buffer : passed in
//);
INT32 VP6_AllocateHuffman (PB_INSTANCE *pbi);
void VP6_DeleteHuffman (PB_INSTANCE *pbi);


#endif

