#ifndef __AC3DATASTRUCT_H__
#define __AC3DATASTRUCT_H__
#include"voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

/* Audio channel masks */
#define CH_FRONT_LEFT             0x00000001
#define CH_FRONT_RIGHT            0x00000002
#define CH_FRONT_CENTER           0x00000004
#define CH_LOW_FREQUENCY          0x00000008
#define CH_BACK_LEFT              0x00000010
#define CH_BACK_RIGHT             0x00000020
#define CH_FRONT_LEFT_OF_CENTER   0x00000040
#define CH_FRONT_RIGHT_OF_CENTER  0x00000080
#define CH_BACK_CENTER            0x00000100
#define CH_SIDE_LEFT              0x00000200
#define CH_SIDE_RIGHT             0x00000400
#define CH_TOP_CENTER             0x00000800
#define CH_TOP_FRONT_LEFT         0x00001000
#define CH_TOP_FRONT_CENTER       0x00002000
#define CH_TOP_FRONT_RIGHT        0x00004000
#define CH_TOP_BACK_LEFT          0x00008000
#define CH_TOP_BACK_CENTER        0x00010000
#define CH_TOP_BACK_RIGHT         0x00020000
#define CH_STEREO_LEFT            0x20000000  ///< Stereo downmix.
#define CH_STEREO_RIGHT           0x40000000  ///< See CH_STEREO_LEFT.

#define AC3_MAX_CODED_FRAME_SIZE 3840 /* in bytes */
#define AC3_MAX_CHANNELS 6 /* including LFE channel */

#define AC3_MAX_COEFS   256
#define AC3_BLOCK_SIZE  256
#define AC3_MAX_BLOCKS    6
#define AC3_FRAME_SIZE (AC3_MAX_BLOCKS * 256)
#define AC3_WINDOW_SIZE (AC3_BLOCK_SIZE * 2)
#define AC3_CRITICAL_BANDS 50

/* exponent encoding strategy */
#define EXP_REUSE 0
#define EXP_NEW   1
#define EXP_D15   1
#define EXP_D25   2
#define EXP_D45   3

typedef enum {
    AAC_AC3_PARSE_ERROR_SYNC        = -1,
    AAC_AC3_PARSE_ERROR_BSID        = -2,
    AAC_AC3_PARSE_ERROR_SAMPLE_RATE = -3,
    AAC_AC3_PARSE_ERROR_FRAME_SIZE  = -4,
    AAC_AC3_PARSE_ERROR_FRAME_TYPE  = -5,
    AAC_AC3_PARSE_ERROR_CRC         = -6,
    AAC_AC3_PARSE_ERROR_CHANNEL_CFG = -7,
} AACAC3ParseError;

/** Delta bit allocation strategy */
typedef enum {
    DBA_REUSE = 0,
    DBA_NEW,
    DBA_NONE,
    DBA_RESERVED
} AC3DeltaStrategy;

/** Channel mode (audio coding mode) */
typedef enum {
    AC3_CHMODE_DUALMONO = 0,
    AC3_CHMODE_MONO,
    AC3_CHMODE_STEREO,
    AC3_CHMODE_3F,
    AC3_CHMODE_2F1R,
    AC3_CHMODE_3F1R,
    AC3_CHMODE_2F2R,
    AC3_CHMODE_3F2R
} AC3ChannelMode;

typedef struct AC3BitAllocParameters {
    int sr_code;
    int sr_shift;
    int slow_gain, slow_decay, fast_decay, db_per_bit, floor;
    int cpl_fast_leak, cpl_slow_leak;
} AC3BitAllocParameters;

typedef struct GetBitContext {
    const VO_U8 *buffer, *buffer_end;
    int index;
    int size_in_bits;
} GetBitContext;

typedef struct {
    VO_U16 sync_word;
    VO_U16 crc1;
    VO_U8 sr_code;
    VO_U8 bitstream_id;
    VO_U8 channel_mode;
    VO_U8 lfe_on;
    VO_U8 frame_type;
    int substreamid;                        ///< substream identification
    int center_mix_level;                   ///< Center mix level index
    int surround_mix_level;                 ///< Surround mix level index
    VO_U16 channel_map;
    int num_blocks;                         ///< number of audio blocks

    VO_U8 sr_shift;
    VO_U16 sample_rate;
    VO_U32 bit_rate;
    VO_U8 channels;
    VO_U16 frame_size;
    VO_S64 channel_layout;
    /** @} */
} AC3HeaderInfo;

typedef enum {
    EAC3_FRAME_TYPE_INDEPENDENT = 0,
    EAC3_FRAME_TYPE_DEPENDENT,
    EAC3_FRAME_TYPE_AC3_CONVERT,
    EAC3_FRAME_TYPE_RESERVED
} EAC3FrameType;


#ifdef _VONAMESPACE
}
#endif

#endif
