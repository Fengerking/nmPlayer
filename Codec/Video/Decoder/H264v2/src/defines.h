#ifndef _DEFINES_H_
#define _DEFINES_H_
#ifdef TRACE
#undef TRACE
#endif
#if defined _DEBUG
# define TRACE           0     //!< 0:Trace off 1:Trace on 2:detailed CABAC context information
#else
# define TRACE           0     //!< 0:Trace off 1:Trace on 2:detailed CABAC context information
#endif


#define MVC_EXTENSION_ENABLE      0    //!< enable support for the Multiview High Profile
#define CAL_PARSER    0
#define CAL_SLICE     0
#define CAL_DEBLOCK   0
#define CAL_MC        0
#define CAL_HEADER    0
#define ENABLE_DEBLOCK_MB       1


#define FAST_FILTER 1
//#define INT_MAX (1<<30)

#include "typedefs.h"

//#define MAX_NUM_SLICES 150
#define MAX_NUM_SLICES     32
#define MAX_REFERENCE_PICTURES 32               //!< H.264 allows 32 fields
#define MAX_REFERENCE_FRAMES  16
#define MAX_CODED_FRAME_SIZE 1000000         //!< bytes for one frame
#define MAX_NUM_DECSLICES  16
#define MAX_DEC_THREADS    16                  //16 core deocoding;
#define MCBUF_LUMA_PAD_X        32
#define MCBUF_LUMA_PAD_Y        20
#define MCBUF_CHROMA_PAD_X      16
#define MCBUF_CHROMA_PAD_Y      8

//AVC Profile IDC definitions
typedef enum {
  FREXT_CAVLC444 = 44,       //!< YUV 4:4:4/14 "CAVLC 4:4:4"
  BASELINE       = 66,       //!< YUV 4:2:0/8  "Baseline"
  MAIN           = 77,       //!< YUV 4:2:0/8  "Main"
  EXTENDED       = 88,       //!< YUV 4:2:0/8  "Extended"
  FREXT_HP       = 100,      //!< YUV 4:2:0/8  "High"
  FREXT_Hi10P    = 110,      //!< YUV 4:2:0/10 "High 10"
  FREXT_Hi422    = 122,      //!< YUV 4:2:2/10 "High 4:2:2"
  FREXT_Hi444    = 244,      //!< YUV 4:4:4/14 "High 4:4:4"
  MVC_HIGH       = 118,      //!< YUV 4:2:0/8  "Multiview High"
  STEREO_HIGH    = 128       //!< YUV 4:2:0/8  "Stereo High"
} ProfileIDC;

#define FILE_NAME_SIZE  255
#define INPUT_TEXT_SIZE 1024

#define NUM_BLOCK_TYPES 10



//#define _LEAKYBUCKET_

#define BLOCK_SHIFT            2
#define BLOCK_SIZE             4
#define BLOCK_SIZE_8x8         8
#define SMB_BLOCK_SIZE         8
#define BLOCK_PIXELS          16
#define MB_BLOCK_SIZE         16
#define MB_PIXELS            256 // MB_BLOCK_SIZE * MB_BLOCK_SIZE
#define MB_PIXELS_SHIFT        8 // log2(MB_BLOCK_SIZE * MB_BLOCK_SIZE)
#define MB_BLOCK_SHIFT         4
#define BLOCK_MULTIPLE         4 // (MB_BLOCK_SIZE/BLOCK_SIZE)
#define MB_BLOCK_PARTITIONS   16 // (BLOCK_MULTIPLE * BLOCK_MULTIPLE)
#define BLOCK_CONTEXT         64 // (4 * MB_BLOCK_PARTITIONS)
#define MB_DEC_BLOCK_SIZE     32

// These variables relate to the subpel accuracy supported by the software (1/4)
#define BLOCK_SIZE_SP      16  // BLOCK_SIZE << 2
#define BLOCK_SIZE_8x8_SP  32  // BLOCK_SIZE8x8 << 2

//  Available MB modes
typedef enum {
  BSKIP_DIRECT =  0,
  PSKIP		   =  0,
  P16x16       =  1,
  P16x8        =  2,
  P8x16        =  3,
  SMB8x8       =  4,
  SMB8x4       =  5,
  SMB4x8       =  6,
  SMB4x4       =  7,
  P8x8         =  8,
  I4MB         =  9,
  I16MB        = 10,
  IBLOCK       = 11,
  SI4MB        = 12,
  I8MB         = 13,
  IPCM         = 14,
  MAXMODE      = 15
} MBModeTypes;
#define VO_I4x4   0x0001
#define VO_I16x16 0x0002 
#define VO_IPCM  0x0004 
#define VO_16x16      0x0008
#define VO_16x8       0x0010
#define VO_8x16       0x0020
#define VO_8x8        0x0040
#define VO_INTERLACED 0x0080
#define VO_DIRECT     0x0100
#define VO_ACPRED     0x0200
#define VO_GMC        0x0400
#define VO_SKIP       0x0800
#define VO_P0L0       0x1000
#define VO_P1L0       0x2000
#define VO_P0L1       0x4000
#define VO_P1L1       0x8000
#define VO_L0         (VO_P0L0 | VO_P1L0)
#define VO_L1         (VO_P0L1 | VO_P1L1)
#define VO_L0L1       (VO_L0   | VO_L1)
#define VO_QUANT      0x00010000
#define VO_CBP        0x00020000
#define VO_REF        VO_ACPRED 
#define VO_8x8DCT     0x01000000
#define VO_INTRA_OR_16X16 (VO_16x16|VO_I4x4|VO_I16x16|VO_IPCM)


#define IS_REF0(type)         ((type) & VO_REF)
#define IS_8x8DCT(type)       ((type) & VO_8x8DCT)
#define MB_TYPE_INTRA    VO_I4x4 
#define IS_INTRA4x4(type)   ((type)&VO_I4x4)
#define IS_INTRA16x16(type) ((type)&VO_I16x16)
#define IS_PCM(type)        ((type)&VO_IPCM)
#define IS_INTRA0(type)      ((type)&7)
#define IS_INTER(type)      ((type)&(VO_16x16|VO_16x8|VO_8x16|VO_8x8))
#define IS_SKIP(type)       ((type)&VO_SKIP)
#define IS_INTRA_PCM(type)  ((type)&VO_IPCM)
#define IS_INTERLACED(type) ((type)&VO_INTERLACED)
#define IS_DIRECT_VO(type)     ((type)&VO_DIRECT)
#define IS_GMC(type)        ((type)&VO_GMC)
#define IS_16X16(type)      ((type)&VO_16x16)
#define IS_16X8(type)       ((type)&VO_16x8)
#define IS_8X16(type)       ((type)&VO_8x16)
#define IS_8X8(type)        ((type)&VO_8x8)
#define IS_SUB_8X8(type)    ((type)&VO_16x16) 
#define IS_SUB_8X4(type)    ((type)&VO_16x8)  
#define IS_SUB_4X8(type)    ((type)&VO_8x16)  
#define IS_SUB_4X4(type)    ((type)&VO_8x8)   
#define IS_ACPRED(type)     ((type)&VO_ACPRED)
#define IS_QUANT(type)      ((type)&VO_QUANT)
#define IS_DIR(type, part, list) ((type) & (VO_P0L0<<((part)+2*(list))))
#define USES_LIST(type, list) ((type) & ((VO_P0L0|VO_P1L0)<<(2*(list)))) ///< does this mb use listX, note does not work if subMBs
#define HAS_CBP(type)        ((type)&VO_CBP)

// number of intra prediction modes
#define NO_INTRA_PMODE  9
typedef struct VOIMode{
    unsigned short mb_type;
    unsigned char mode;
    unsigned char cbp;
} VOIModeStruct;

static VOIModeStruct i_mb_mode[26]=
{
  {VO_I4x4  , -1, -1},
  {VO_I16x16,  2,  0},
  {VO_I16x16,  1,  0},
  {VO_I16x16,  0,  0},
  {VO_I16x16,  3,  0},
  {VO_I16x16,  2,  16},
  {VO_I16x16,  1,  16},
  {VO_I16x16,  0,  16},
  {VO_I16x16,  3,  16},
  {VO_I16x16,  2,  32},
  {VO_I16x16,  1,  32},
  {VO_I16x16,  0,  32},
  {VO_I16x16,  3,  32},
  {VO_I16x16,  2,  15+0},
  {VO_I16x16,  1,  15+0},
  {VO_I16x16,  0,  15+0},
  {VO_I16x16,  3,  15+0},
  {VO_I16x16,  2,  15+16},
  {VO_I16x16,  1,  15+16},
  {VO_I16x16,  0,  15+16},
  {VO_I16x16,  3,  15+16},
  {VO_I16x16,  2,  15+32},
  {VO_I16x16,  1,  15+32},
  {VO_I16x16,  0,  15+32},
  {VO_I16x16,  3,  15+32},
  {VO_IPCM , -1, -1},
};

typedef struct VOPMode{
    unsigned short mb_type;
    unsigned char partition_number;
} VOPModeStruct;

static VOPModeStruct p_mb_mode_info[5]=
{
  {VO_16x16|VO_P0L0, 1},
  {VO_16x8 |VO_P0L0|VO_P1L0, 2},
  {VO_8x16 |VO_P0L0|VO_P1L0, 2},
  {VO_8x8  |VO_P0L0|VO_P1L0, 4},
  {VO_8x8  |VO_P0L0|VO_P1L0|VO_REF, 4},
};

static VOPModeStruct p_sub_mb_mode_info[4]=
{
  {VO_16x16|VO_P0L0, 1},
  {VO_16x8 |VO_P0L0, 2},
  {VO_8x16 |VO_P0L0, 2},
  {VO_8x8  |VO_P0L0, 4},
};

static VOPModeStruct b_mb_mode_info[23]=
{
  {VO_DIRECT|VO_L0L1, 1, },
  {VO_16x16|VO_P0L0, 1, },
  {VO_16x16|VO_P0L1, 1, },
  {VO_16x16|VO_P0L0|VO_P0L1, 1, },
  {VO_16x8 |VO_P0L0|VO_P1L0, 2, },
  {VO_8x16 |VO_P0L0|VO_P1L0, 2, },
  {VO_16x8|VO_P0L1|VO_P1L1, 2, },
  {VO_8x16|VO_P0L1|VO_P1L1, 2, },
  {VO_16x8 |VO_P0L0|VO_P1L1, 2, },
  {VO_8x16 |VO_P0L0|VO_P1L1, 2, },
  {VO_16x8|VO_P0L1|VO_P1L0, 2, },
  {VO_8x16|VO_P0L1|VO_P1L0, 2, },
  {VO_16x8 |VO_P0L0|VO_P1L0|VO_P1L1, 2, },
  {VO_8x16 |VO_P0L0|VO_P1L0|VO_P1L1, 2, },
  {VO_16x8|VO_P0L1|VO_P1L0|VO_P1L1, 2, },
  {VO_8x16|VO_P0L1|VO_P1L0|VO_P1L1, 2, },
  {VO_16x8|VO_P0L0|VO_P0L1|VO_P1L0, 2, },
  {VO_8x16|VO_P0L0|VO_P0L1|VO_P1L0, 2, },
  {VO_16x8|VO_P0L0|VO_P0L1|VO_P1L1, 2, },
  {VO_8x16|VO_P0L0|VO_P0L1|VO_P1L1, 2, },
  {VO_16x8|VO_P0L0|VO_P0L1|VO_P1L0|VO_P1L1, 2, },
  {VO_8x16|VO_P0L0|VO_P0L1|VO_P1L0|VO_P1L1, 2, },
  {VO_8x8|VO_P0L0|VO_P0L1|VO_P1L0|VO_P1L1, 4, },
};

static VOPModeStruct b_sub_mb_mode_info[13]=
{
  {VO_DIRECT, 1, },
  {VO_16x16|VO_P0L0, 1, },
  {VO_16x16|VO_P0L1, 1, },
  {VO_16x16|VO_P0L0|VO_P0L1, 1, },
  {VO_16x8 |VO_P0L0|VO_P1L0, 2, },
  {VO_8x16 |VO_P0L0|VO_P1L0, 2, },
  {VO_16x8 |VO_P0L1|VO_P1L1, 2, },
  {VO_8x16 |VO_P0L1|VO_P1L1, 2, },
  {VO_16x8 |VO_P0L0|VO_P0L1|VO_P1L0|VO_P1L1, 2, },
  {VO_8x16 |VO_P0L0|VO_P0L1|VO_P1L0|VO_P1L1, 2, },
  {VO_8x8  |VO_P0L0|VO_P1L0, 4, },
  {VO_8x8  |VO_P0L1|VO_P1L1, 4, },
  {VO_8x8  |VO_P0L0|VO_P0L1|VO_P1L0|VO_P1L1, 4, },
};

// Direct Mode types
typedef enum {
  DIR_TEMPORAL = 0, //!< Temporal Direct Mode
  DIR_SPATIAL  = 1 //!< Spatial Direct Mode
} DirectModes;

// CAVLC block types
typedef enum {
  LUMA              =  0,
  LUMA_INTRA16x16DC =  1,
  LUMA_INTRA16x16AC =  2,
  CB                =  3,
  CB_INTRA16x16DC   =  4,
  CB_INTRA16x16AC   =  5,
  CR                =  8,
  CR_INTRA16x16DC   =  9,
  CR_INTRA16x16AC   = 10
} CAVLCBlockTypes;

// CABAC block types
typedef enum {
  LUMA_16DC     =   0,
  LUMA_16AC     =   1,
  LUMA_8x8      =   2,
  LUMA_8x4      =   3,
  LUMA_4x8      =   4,
  LUMA_4x4      =   5,
  CHROMA_DC     =   6,
  CHROMA_AC     =   7,
  CHROMA_DC_2x4 =   8,
  CHROMA_DC_4x4 =   9,
  CB_16DC       =  10,
  CB_16AC       =  11,
  CB_8x8        =  12,
  CB_8x4        =  13,
  CB_4x8        =  14,
  CB_4x4        =  15,
  CR_16DC       =  16,
  CR_16AC       =  17,
  CR_8x8        =  18,
  CR_8x4        =  19,
  CR_4x8        =  20,
  CR_4x4        =  21
} CABACBlockTypes;

// Macro defines
#define Q_BITS          15
#define DQ_BITS          6
#define Q_BITS_8        16
#define DQ_BITS_8        6 


#define TOTRUN_NUM       15
#define RUNBEFORE_NUM     7
#define RUNBEFORE_NUM_M1  6

// Quantization parameter range
#define MIN_QP          0
#define MAX_QP          51
// 4x4 intra prediction modes 
typedef enum {
  VERT_PRED            = 0,
  HOR_PRED             = 1,
  DC_PRED              = 2,
  DIAG_DOWN_LEFT_PRED  = 3,
  DIAG_DOWN_RIGHT_PRED = 4,
  VERT_RIGHT_PRED      = 5,
  HOR_DOWN_PRED        = 6,
  VERT_LEFT_PRED       = 7,
  HOR_UP_PRED          = 8,
  DC_LEFT_PRED         = 9,
  DC_TOP_PRED          = 10,
  DC_128_PRED          = 11
} I4x4PredModes;


// 16x16 intra prediction modes
typedef enum {
  DC_PRED_16     = 0,
  HOR_PRED_16    = 1,
  VERT_PRED_16   = 2,
  PLANE_16       = 3,
  DC_LEFT_PRED_16= 4,
  DC_TOP_PRED_16 = 5,
  DC_128_PRED_16 = 6
} I16x16PredModes;

// 8x8 chroma intra prediction modes
typedef enum {
  DC_PRED_8     =  0,
  HOR_PRED_8    =  1,
  VERT_PRED_8   =  2,
  PLANE_8       =  3,
  DC_LEFT_PRED_8=  4,
  DC_TOP_PRED_8 =  5,
  DC_128_PRED_8 =  6
} I8x8PredModes;

typedef enum {
  EOS = 1,    //!< End Of Sequence
  SOP = 2,    //!< Start Of Picture
  SOS = 3,     //!< Start Of Slice
  SOS_CONT = 4
}EndTypes;

// MV Prediction types
typedef enum {
  MVPRED_MEDIAN   = 0,
  MVPRED_L        = 1,
  MVPRED_U        = 2,
  MVPRED_UR       = 3
} MVPredTypes;

typedef enum {
  DECODING_OK     = 0,
  SEARCH_SYNC     = 1,
  PICTURE_DECODED = 2
}IntraReturn;

#define  LAMBDA_ACCURACY_BITS         16
#define INVALIDINDEX  (-135792468)

#define RC_MAX_TEMPORAL_LEVELS   5

//Start code and Emulation Prevention need this to be defined in identical manner at encoder and decoder
#define ZEROBYTES_SHORTSTARTCODE 2 //indicates the number of zero bytes in the short start-code prefix

#define MAX_PLANE       3
#define IS_FREXT_PROFILE(profile_idc) ( profile_idc>=FREXT_HP || profile_idc == FREXT_CAVLC444 )
#define HI_INTRA_ONLY_PROFILE         (((currSlice->active_sps->profile_idc>=FREXT_Hi10P)&&(currSlice->active_sps->constrained_set3_flag))||(currSlice->active_sps->profile_idc==FREXT_CAVLC444)) 

#define VOH264ERROR(a)	\
{\
  return a;\
}
 
#endif

