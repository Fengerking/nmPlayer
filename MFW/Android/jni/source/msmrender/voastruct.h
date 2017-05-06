/*
	voastruct.h
	for define some common structures
	visualon
	davidone
	2007/05/17
*/

#if !defined __VOA_STRUCT_H__
#define __VOA_STRUCT_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <errno.h>
//#include <sys/types.h>

typedef enum {
	VOARSTS_STOPPED		= 0,
	VOARSTS_RUNNING		= 1,
	VOARSTS_PAUSED		= 2,
	VOARSTS_FLUSHED		= 3,
} VOARSTAT;

/** The VO_DIRTYPE enumeration is used to indicate if a port is an input or
    an output port.  This enumeration is common across all component types.    
*/
typedef enum VO_ENDIANTYPE
{
    VO_EndianBig, /**< big endian */
    VO_EndianLittle, /**< little endian */
    VO_EndianMax = 0x7FFFFFFF
} VO_ENDIANTYPE;


/** The VO_NUMERICALDATATYPE enumeration is used to indicate if data 
    is signed or unsigned
 */
typedef enum VO_NUMERICALDATATYPE
{
    VO_NumericalDataSigned, /**< signed data */
    VO_NumericalDataUnsigned, /**< unsigned data */
    VO_NumercialDataMax = 0x7FFFFFFF
} VO_NUMERICALDATATYPE;


/** PCM mode type */ 
typedef enum VO_AUDIO_PCMMODETYPE { 
    VO_AUDIO_PCMModeLinear = 0,  /**< Linear PCM encoded data */ 
    VO_AUDIO_PCMModeALaw,        /**< A law PCM encoded data (G.711) */ 
    VO_AUDIO_PCMModeMULaw,       /**< Mu law PCM encoded data (G.711)  */ 
    VO_AUDIO_PCMModeMax = 0x7FFFFFFF 
}VO_AUDIO_PCMMODETYPE; 


typedef enum __VO_AUDIO_CHANNELTYPE {
    VO_AUDIO_ChannelNone = 0x0,    /**< Unused or empty */
    VO_AUDIO_ChannelLF   = 0x1,    /**< Left front */
    VO_AUDIO_ChannelRF   = 0x2,    /**< Right front */
    VO_AUDIO_ChannelCF   = 0x3,    /**< Center front */
    VO_AUDIO_ChannelLS   = 0x4,    /**< Left surround */
    VO_AUDIO_ChannelRS   = 0x5,    /**< Right surround */
    VO_AUDIO_ChannelLFE  = 0x6,    /**< Low frequency effects */
    VO_AUDIO_ChannelCS   = 0x7,    /**< Back surround */
    VO_AUDIO_ChannelLR   = 0x8,    /**< Left rear. */
    VO_AUDIO_ChannelRR   = 0x9,    /**< Right rear. */
    VO_AUDIO_ChannelMax  = 0x7FFFFFFF 
} __VO_AUDIO_CHANNELTYPE;

typedef struct {
	signed int SampleRate;
	signed int Channels;
	signed int SampleBits;
}VO_AUDIO_FORMAT;

typedef struct _VO_AUDIO_PARAM {
	bool binterleaved;
	unsigned long dwsamplerate;
	unsigned long dwAvgBytesPerSec; 
	VO_ENDIANTYPE eendiantype;
	VO_NUMERICALDATATYPE edatasigned;
	VO_AUDIO_PCMMODETYPE epcmmode;
	unsigned short wchannels;
	unsigned short wbitspersample;
	unsigned short wBlockAlign;
}VOAUDIOPARAM, *PVOAUDIOPARAM;


////////////////////////////////////////////////////////////////////////////////////////
//for MSM7k

#define AUDIO_IOCTL_MAGIC 'a'

#define AUDIO_START        _IOW(AUDIO_IOCTL_MAGIC, 0, unsigned)
#define AUDIO_STOP         _IOW(AUDIO_IOCTL_MAGIC, 1, unsigned)
#define AUDIO_FLUSH        _IOW(AUDIO_IOCTL_MAGIC, 2, unsigned)
#define AUDIO_GET_CONFIG   _IOR(AUDIO_IOCTL_MAGIC, 3, unsigned)
#define AUDIO_SET_CONFIG   _IOW(AUDIO_IOCTL_MAGIC, 4, unsigned)
#define AUDIO_GET_STATS    _IOR(AUDIO_IOCTL_MAGIC, 5, unsigned)
#define AUDIO_ENABLE_AUDPP _IOW(AUDIO_IOCTL_MAGIC, 6, unsigned)
#define AUDIO_SET_ADRC     _IOW(AUDIO_IOCTL_MAGIC, 7, unsigned)
#define AUDIO_SET_EQ       _IOW(AUDIO_IOCTL_MAGIC, 8, unsigned)
#define AUDIO_SET_RX_IIR   _IOW(AUDIO_IOCTL_MAGIC, 9, unsigned)

#define EQ_MAX_BAND_NUM 12

#define ADRC_ENABLE  0x0001
#define ADRC_DISABLE 0x0000
#define EQ_ENABLE    0x0002
#define EQ_DISABLE   0x0000
#define IIR_ENABLE   0x0004
#define IIR_DISABLE  0x0000

struct eq_filter_type {
    short gain;
    unsigned short freq;
    unsigned short type;
    unsigned short qf;
};

struct eqalizer {
    unsigned short bands;
    unsigned short params[132];
};

struct rx_iir_filter {
    unsigned short num_bands;
    unsigned short iir_params[48];
};

/*struct msm_audio_config {
    unsigned int buffer_size;
    unsigned int buffer_count;
    unsigned int channel_count;
    unsigned int sample_rate;
    unsigned int codec_type;
    unsigned int unused[3];
};*/

#define CODEC_TYPE_PCM 0
#define AUDIO_HW_NUM_OUT_BUF 2  // Number of buffers in audio driver for output
// TODO: determine actual audio DSP and hardware latency
#define AUDIO_HW_OUT_LATENCY_MS 0  // Additionnal latency introduced by audio DSP and hardware in ms

#define AUDIO_HW_IN_SAMPLERATE 8000                 // Default audio input sample rate
#define AUDIO_HW_IN_CHANNELS 1                      // Default audio input number of channels
#define AUDIO_HW_IN_BUFFERSIZE 2048                 // Default audio input buffer size
#define AUDIO_HW_IN_FORMAT (AudioSystem::PCM_16_BIT)  // Default audio input sample format
 
/*enum {
    OK                = 0,    // Everything's swell.
    NO_ERROR          = 0,    // No errors.
    
    UNKNOWN_ERROR       = 0x80000000,

    NO_MEMORY           = -ENOMEM,
    INVALID_OPERATION   = -ENOSYS,
    BAD_VALUE           = -EINVAL,
    BAD_TYPE            = 0x80000001,
    NAME_NOT_FOUND      = -ENOENT,
    PERMISSION_DENIED   = -EPERM,
    NO_INIT             = -ENODEV,
    ALREADY_EXISTS      = -EEXIST,
    DEAD_OBJECT         = -EPIPE,
    FAILED_TRANSACTION  = 0x80000002,
    JPARKS_BROKE_IT     = -EPIPE,
#if !defined(HAVE_MS_C_RUNTIME)
    BAD_INDEX           = -EOVERFLOW,
    NOT_ENOUGH_DATA     = -ENODATA,
    WOULD_BLOCK         = -EWOULDBLOCK, 
    TIMED_OUT           = -ETIME,
    UNKNOWN_TRANSACTION = -EBADMSG,
#else    
    BAD_INDEX           = -E2BIG,
    NOT_ENOUGH_DATA     = 0x80000003,
    WOULD_BLOCK         = 0x80000004,
    TIMED_OUT           = 0x80000005,
    UNKNOWN_TRANSACTION = 0x80000006,
#endif    
}; */
//end for MSM7k
/////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
}
#endif

#endif // __VOA_STRUCT_H__

