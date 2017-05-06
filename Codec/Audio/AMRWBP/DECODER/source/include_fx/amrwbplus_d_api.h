/**
 * \file amrwbplus_d_api.h
 * \brief header file for AMRWB and AMRWB+ decoder
 * \version 1.0
 * \date 07/17/2006 updated, ver 1.0
 * \date 07/17/2006 created
 */


#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include "voAudio.h"
#include "voType.h"

#define AMRWBDAPI 

/*!
 * Decoding Error Code
 */
typedef enum AMRWBPLUS_Dec_Result_tag
{
    AMRWBPLUS_DEC_OPERATION_DONE        = 0,                                        /*!< no error, operation is finished */
    AMRWBPLUS_DEC_DECODE_ONE_FRAME      = AMRWBPLUS_DEC_OPERATION_DONE + 1,         /*!< no error, finish decoding a frame */
    
    AMRWBPLUS_DEC_NEED_MORE_DATA        = 0x1001,                                   /*!< need more input data */
    AMRWBPLUS_DEC_OPEN_INPUT_FILE_FAIL  = AMRWBPLUS_DEC_NEED_MORE_DATA + 1,         /*!< cannot open input file */
    AMRWBPLUS_DEC_INPUT_POINTER_INVALID = AMRWBPLUS_DEC_OPEN_INPUT_FILE_FAIL + 1,   /*!< input pointer is invalid */
    AMRWBPLUS_DEC_INPUT_PARA_INVALID    = AMRWBPLUS_DEC_INPUT_POINTER_INVALID + 1,  /*!< input parameter is invalid */
    AMRWBPLUS_DEC_INPUT_DARA_ERROR      = AMRWBPLUS_DEC_INPUT_PARA_INVALID + 1,     /*!< error in input data */
    AMRWBPLUS_DEC_INPUT_DARA_TYPE_INVALID   = AMRWBPLUS_DEC_INPUT_DARA_ERROR + 1,   /*!< input data type is invalid */

    AMRWBPLUS_DEC_DECODE_FAIL           = 0x2001,                                   /*!< decoding fail */
    AMRWBPLUS_DEC_ALLOCATE_MEMORY_FAIL  = AMRWBPLUS_DEC_DECODE_FAIL + 1,            /*!< allocating memory fail */
    AMRWBPLUS_DEC_INPUT_DATA_OVERFLOW   = AMRWBPLUS_DEC_ALLOCATE_MEMORY_FAIL + 1,   /*!< input data overflow */
    AMRWBPLUS_DEC_UNKNOWN_ERROR         = AMRWBPLUS_DEC_INPUT_DATA_OVERFLOW + 1,    /*!< unknown erro*/

    AMRWBPLUS_DEC_OPEN_OUTPUT_FILE_FAIL = 0x3001,                                   /*!< can't open output file */
    AMRWBPLUS_DEC_NULL_OUTPUT_POINTER   = AMRWBPLUS_DEC_OPEN_OUTPUT_FILE_FAIL + 1,  /*!< output pointer is invalid */
    
    AMRWBPLUS_DEC_FAILED_MAX            = 0x7fff,                                    /*!< maximum error flag of AMRWB+(AMRWB) decoder (not used) */
    AMRWBPLUS_DEC_RESULT_MAX            = VO_MAX_ENUM_VALUE
}
AMRWBPLUS_DEC_RESULT_T;

/*!
 * Input AMRWB(AMRWB+) Data type
 */
typedef enum Input_AMRWB_Data_enum
{
    WBPLUS = 0,             /*!< AMRWBPLUS data */
    WB_MIME = WBPLUS + 1,   /*!< AMRWB data (storage format) */
    OTHER_FORMAT = 0xff,     /*!< maximum data type code (not used) */
	INPUT_AMRWB_DATA_E_MAX = VO_MAX_ENUM_VALUE
}INPUT_AMRWB_DATA_E;

/*!
 * decoder configuration
 */
typedef struct
{
  short mode;               /*!< AMR_WB core mode: 0..8 */
  short extension;          /*!< extension flag */
  short st_mode;            /*!< stereo mode 0..13 */
  short fscale;             /*!< Frequency scaling */
  long  fs;                 /*!< Sampling frequency */
  long  mono_dec_stereo;    /*!< flag to indicate whether decoding stereoto mono */
  long  limiter_on ;        /*!< flag to inicate whether smooth the output to avoid harch clipping */
  short FileFormat;         /*!< file format, only 0(raw) is supported */
  short fer_sim;            /*!< frame errasures simulation */
  short bfi[4];             /*!< bad frame indicator */
}
DecoderConfig;

/*!
 * input parameters for decoder
 */
typedef struct AMRWBPLUS_Dec_Param_tag
{
    unsigned char *inputStream;         /*!< pointer ot the buffer of input data */
    int inputSize;                      /*!< the size of input data */
    short *outputStream;                /*!< pointer to the buffer of output data */
    int outputSize;                     /*!< the size of the decoded frame. For wb frame, it is always 320*2 bytes */
	INPUT_AMRWB_DATA_E input_data_type; /*!< input data type (AMRWB or AMRWB+ data) */
    DecoderConfig conf;                 /*!< some decoder settings */
	int	channels;
	int sampleRate;
    long bitrate;                       /*!< decoder will give the bitrate to caller each time */
	int first;
}
AMRWBPLUS_DEC_PARAM_T;

typedef struct{
	Word16 channel_right[4*L_FRAME48k];
	Word16 channel_left[2*L_FRAME48k];
	Word16 mem_down_right[2*L_FILT_DECIM_FS];
	Word16 mem_down_left[2*L_FILT_DECIM_FS];
	Word16 tmpbfi[4];
	Word16 mem_fr_lim_right[2];
	Word16 mem_fr_lim_left[2];
	Word16 speech16[L_FRAME16k];
	UWord16 serial[NBITS_MAX];
	Word32 dataSize;
	DecoderConfig conf;
	//static Word16 tfi;
	Word32 fs_output;	
	void *stAmrwbDec;
	Decoder_State_Plus_fx *st_d;
	AMRWBPLUS_DEC_PARAM_T params;
	Word32			first;

	Word16 num_channels;
	Word16 L_frame;	
	Word16 frac_down_right;
	Word16 frac_down_left;
	Word16 fac_up;
	Word16 fac_down;
	Word16 last_scaling;
	Word16 nb_samp_fs;
	
	UWord8			*intbuf;
	UWord8			*thisframe;
	UWord8			*inbuf;
	int				framelen;
	int				inlen;
	int				uselength;
	void			*hCheck;

	VO_MEM_OPERATOR *pvoMemop;
	VO_MEM_OPERATOR voMemoprator;
}AMRWBGlobalData;



/*!
 * prepare for AMRWB(AMRWBPLUS) decoding.
 * This function initialize decoder and should be called first.
 * \param amrwbplus_para [in/out] decoder parameters
 */
AMRWBPLUS_DEC_RESULT_T AMRWBDAPI AMRWBPLUS_StartDecode(AMRWBGlobalData *amrwbplus_para);


/*!
 * decode.
 * This function decode a frame.
 * \param amrwbplus_para [in/out] decoder parameters, amrwbplus_para->inputSize should be a frame length
 */
AMRWBPLUS_DEC_RESULT_T AMRWBDAPI AMRWBPLUS_ReceiveData(AMRWBGlobalData *amrwbplus_para);


/*!
 * stop decoding.
 * This function reset decoder ang release resources. it should be called after decoding.
 * \param amrwbplus_para [in/out] decoder parameters
 */
AMRWBPLUS_DEC_RESULT_T AMRWBDAPI AMRWBPLUS_EndDecode(AMRWBGlobalData *amrwbplus_para);


/*!
 * get frame length
 * This function get frame length from the first byte
 * \param amrwbplus_para [in/out] decoder parameters
 * \retval frame length
 */
short AMRWBPLUS_GetFrameLenth(AMRWBPLUS_DEC_PARAM_T *amrwbplus_para, unsigned char*serial);


#ifdef __cplusplus
}
#endif

