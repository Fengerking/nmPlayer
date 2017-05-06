/**                                                                                                                             
* \file    MP4H263_E_API.h                                                                                              
* \brief   API definition for the VisualOn MPEG-4 and H.263 encoder
* \version 0.1          
*/

#ifndef _VOMPEG4ENC_BACK_H_
#define _VOMPEG4ENC_BACK_H_

#include "voMPEG4.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_VALUE_ENUM			0X70000000
#define VOID_MPEG4_BASE         0x1000000

/*!
* Defination of supported profile and level for MPEG4
*
*/
typedef enum {
        PROFILE_S_L0    =0x01,  
        PROFILE_S_L1    =0x02,
        PROFILE_S_L2    =0x03,
        PROFILE_S_L3    =0x04, 
		PROFILE_MAX     =MAX_VALUE_ENUM
} MPEG4_PROFILE_LEVEL;

/*!
* Return codes
*
* Negative presents failed, including invalid configure parameters
* and coding failures
*
*/ 
typedef enum {
        FAILED                                  = 0x80000000, /*!< Error codes, < 0 */

        INVALID_BITRATE                         = 0x80000000,
        INVALID_MX_KEY_FRAME_INTERVAL           = 0x80000001,
        INVALID_ENC_QUALITY                     = 0x80000002,
        INVALID_FRAME_RATE                      = 0x80000003,
        INVALID_PROFILE_LEVEL                   = 0x80000004,
        INVALID_DYNAMIC_GENERATE_IFRAME         = 0x80000005,
        INVALID_VIDEO_PACKET_HEADERS            = 0x80000006,
        INVALID_ROUNDING_CONTROL                = 0x80000007,

        INVALID_INPUT_SIZE                      = 0x80000008,
        INVALID_FORCE_INTRA                     = 0x80000009,
		INVALID_INPUT_RAW_DATA_ADDR             = 0x8000000a,
        INVALID_INTERNAL_MEM_MALLOC             = 0x8000000b,
		UNSUPPORT_INPUT_TYPE                    = 0x8000000c,

        FAILED_TO_ENCODE_I_FRAME                = 0xa0000000,
        FAILED_TO_ENCODE_P_FRAME                = 0xb0000000,
        
        FAILED_TO_ENCODE_INTRA_MB               = 0xf0000000,
        FAILED_TO_ENCODE_INTER_MB               = 0xf1000000,
        
        FAILED_TO_LICENSE_LIMIT                 = 0xFFFFFFF0,
        FATAL_ERROR_END                         = 0xffffffff,

        MPEG_SUCCEEDED                            = 0x00000000  /*!< Success codes, >= 0 */
} RETURN_CODE;


/*!
* Customer initial configure structure for encoder 
*
*/
typedef struct
{
        unsigned int    codec_id;                          /*!< VO_INDEX_ENC_MPEG4, VO_INDEX_ENC_H263 */
        int             image_width;                       /*!< Input frame width */   
        int             image_height;                      /*!< Input frame height */                  
        int             bitrate;                           /*!< 0-VBR, else CBR bitrate setting*/
        int             max_key_frame_interval;            /*!< Max key frame interval setting, >=0 */ 
        int             enc_quality;                       /*!< 1 or 2 or 3 encoder quality setting, only support 2, 3 now, H.263 only support 2*/
        float           framerate;                         /*!< Frame rate setting */
        int             profile_level;                     /*!< Profile and level setting*/
        int             vp_size;						   /*!< Set video packet size(byte), default is"0" and means no video packet*/	
        int             frameCount;                        /*!< Number of frames to be encoded*/
		int				rt_type;						   /*!< Input rotation type */
 //       FILE            *inputFile;                        /*!< Input file for encoding file*/             
 //       FILE            *outputFile;                       /*!< Output file for encoding file*/                    
} CUSTOM_CONFIG;

/*!
* Configure structure to encode a frame 
*
*/
typedef struct
{
    int force_intra_frame;                  /*!< 0-default, 1-coding as intra frame*/
    VO_IV_COLORTYPE input_type;                         /*!< Input source type */
    int do_preprocess;						/*!< Preprocess flag, 1-do preprocess 0-don't do*/
    VO_VIDEO_FRAMETYPE frame_type;                         /*!< To return encoded frame type, 0-Iframe, 1-Pframe */
} FRAME_INFO_CONFIG;

/*!
 * Parse the configure file of encoder. 
 *
 * \param fileName [in] File point of configure file
 * \param configiration [out] Customer initial configure structure to initialize encoder
 * \remarks This function should be called if there is a configure file 
 * to initialize the CUSTOM_CONFIG structure.
 */
//extern RETURN_CODE __cdecl vompeg4_parse_configuration_file(const char *fileName, CUSTOM_CONFIG *configiration);

/*!
 * Initialize the encoder.
 *
 * \param custom_config [in] configure structure to initialize encoder
 * \remarks This function should be called before to encode a frame
 * If the items in CUSTOM_CONFIG strcture are changed, pls call voMpeg4EncFinish() and 
 * this function to initialize the encoder again.
 */
extern VO_VOID * vompeg4_enc_init(CUSTOM_CONFIG* custom_config);

/*!
 * encode a frame.
 *
 * \param frame_info_config [in/out] configure structure to encode a frame 
 * \param input [in] Input raw data
 * \param output [out] Output encoded data
 * \return positive number presents the length of compressed data, negative number presents the errors
 */
extern RETURN_CODE voMpeg4EncFrame(FRAME_INFO_CONFIG* frame_info_config, VO_VOID * enc_handle, unsigned char* input, unsigned char* output);

/*!
 * Close the encoder.
 *
 * \remarks Call this function to close the encoder
 */
extern RETURN_CODE voMpeg4EncFinish(VO_VOID * enc_handle);

/*!
 * Write VOL header
 *
 * \param output [out] Output of VOL data
 * \remarks This function should be called after vompeg4_enc_init().
 */
extern VO_S32 voMpeg4WriteVIH(VO_VOID * enc_handle, VO_U8 *output);

#ifdef __cplusplus
}
#endif

#endif//_VOMPEG4ENC_BACK_H_
