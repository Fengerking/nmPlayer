
/******************************************************************************
*                                  *                                          *
*        VisualOn, Inc. Confidential and Proprietary, 2006                    *
*                                    *                                        *
*******************************************************************************/

#ifndef __AMRWBPLUS_D_API_H__
#define __AMRWBPLUS_D_API_H__

typedef struct {
  short mode;        /* AMR_WB core mode: 0..8 */
  short extension;   /* 0=AMRWB, 1=mono, 2=stereo20%, 3=stereo25% */
  short st_mode;     /* stereo mode 0..13 (not used, for ericsson cmd line?) */
  short fscale;   /* Frequency scaling */
  long  fs; 
  long  mono_dec_stereo; 
  long  limiter_on ;
  short FileFormat;
  short fer_sim;                /* frame errasures simulation */
  short bfi[4];         //valid only when fer_sim is not 0
  
}DecoderConfig;

typedef enum Input_AMRWB_Data_enum
{
    WBPLUS = 0,
    WB_MIME = WBPLUS + 1,
    OTHER_FORMAT = 0xff
}INPUT_AMRWB_DATA_E;

typedef struct AMRWBPLUS_Dec_Param_tag {
    //pointer ot the buffer of input data
    unsigned char *inputStream;
    //the size of input data
    int inputSize;
    
    //pointer to the buffer of output data.
    short *outputStream;
    //the size of the decoded frame. For wb frame, it is always 320*2 bytes.
    int outputSize;

    //input data is MIME WB frame, or it is WB+ frame type    
	INPUT_AMRWB_DATA_E input_data_type;

    //some decoder settings
    DecoderConfig conf;
    
    //decoder will give the bitrate to caller each time
    long bitrate;
} AMRWBPLUS_DEC_PARAM_T;


typedef enum AMRWBPLUS_Dec_Result_tag {
    AMRWBPLUS_DEC_OPERATION_DONE        = 0,
    AMRWBPLUS_DEC_DECODE_ONE_FRAME      = AMRWBPLUS_DEC_OPERATION_DONE + 1,
    
    AMRWBPLUS_DEC_NEED_MORE_DATA        = 0x1001,
    AMRWBPLUS_DEC_OPEN_INPUT_FILE_FAIL  = AMRWBPLUS_DEC_NEED_MORE_DATA + 1,
    AMRWBPLUS_DEC_INPUT_POINTER_INVALID = AMRWBPLUS_DEC_OPEN_INPUT_FILE_FAIL + 1,
    AMRWBPLUS_DEC_INPUT_PARA_INVALID    = AMRWBPLUS_DEC_INPUT_POINTER_INVALID + 1,
    AMRWBPLUS_DEC_INPUT_DARA_ERROR      = AMRWBPLUS_DEC_INPUT_PARA_INVALID + 1,
    AMRWBPLUS_DEC_INPUT_DARA_TYPE_INVALID   = AMRWBPLUS_DEC_INPUT_DARA_ERROR + 1,

    AMRWBPLUS_DEC_DECODE_FAIL           = 0x2001,
    AMRWBPLUS_DEC_ALLOCATE_MEMORY_FAIL  = AMRWBPLUS_DEC_DECODE_FAIL + 1,
    AMRWBPLUS_DEC_INPUT_DATA_OVERFLOW   = AMRWBPLUS_DEC_ALLOCATE_MEMORY_FAIL + 1,
    AMRWBPLUS_DEC_UNKNOWN_ERROR         = AMRWBPLUS_DEC_INPUT_DATA_OVERFLOW + 1,

    AMRWBPLUS_DEC_OPEN_OUTPUT_FILE_FAIL = 0x3001,
    AMRWBPLUS_DEC_NULL_OUTPUT_POINTER   = AMRWBPLUS_DEC_OPEN_OUTPUT_FILE_FAIL + 1,
    
    AMRWBPLUS_DEC_FAILED_MAX            = 0x7fff
} AMRWBPLUS_DEC_RESULT_T;


/************************************************************************
*                                    *                                    *
* APIs of decoder library                                                *
*  -- AMRWBPLUS_StartDecode(AMRWBPLUS_DEC_PARAM_T *amrwbplus_para)        *
*     initialize decoder and  user-defined input parameters.            *
*                                    *                                    *
*  -- AMRWBPLUS_ReceiveData(AMRWBPLUS_DEC_PARAM_T *amrwbplus_para)        *
*     inParam: user-defined input parameters; users modify data in input*
*            section of param to communicate with decoder; users get     *
*            data from output section of param.                         *
*                                    *                                    *
*  -- AMRWBPLUS_EndDecode(AMRWBPLUS_DEC_PARAM_T *amrwbplus_para)        *
*     clean up decoder;                                                    *
*                                    *                                    *
************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#define AMRWBDAPI __cdecl

//AMRWBPLUS_DEC_RESULT_T AMRWBDAPI AMRWBPLUS_StartDecode(AMRWBPLUS_DEC_PARAM_T *amrwbplus_para);
// AMRWBPLUS_DEC_RESULT_T AMRWBDAPI AMRWBPLUS_ReceiveData(AMRWBPLUS_DEC_PARAM_T *amrwbplus_para);
//AMRWBPLUS_DEC_RESULT_T AMRWBDAPI AMRWBPLUS_EndDecode(AMRWBPLUS_DEC_PARAM_T *amrwbplus_para);
//short AMRWBDAPI AMRWBPLUS_GetFrameLenth(AMRWBPLUS_DEC_PARAM_T *amrwbplus_para);


#ifdef __cplusplus
}
#endif

#endif //__AMRWBPLUS_D_API_H__
