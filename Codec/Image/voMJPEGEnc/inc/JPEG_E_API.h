/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

/** 
 * \file JPEG_E_API.h
 * \brief  VisualOn JPEG encode application
 * \version 0.1
 * \date 4/16/2006 created
 */

#ifndef __RAWTOJPEG_H__
#define __RAWTOJPEG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*! 
 * An enum.
 * descript the input YUV mode
 * More detailed enum description.
 */
typedef enum {
	ERROR_YUV							=	-1	,	/**< define input YUV error mode		 */

	YUV_444_PLANER						=	 0	,	/**< define input YUV planar 444 mode   */
	YUV_444_INTERLEAVE					=	 1 	,	/**< define input YUV interleave 444 mode   */

	YUV_422_PLANER						=	10	,	/**< define input YUV planar 422 mode   */
	YUV_422_YUYV						=	11	,	/**< define input YUV interleave 422 mode: Y U Y V  */
	YUV_422_YVYU						=	12	,	/**< define input YUV interleave 422 mode: Y V Y U  */
	YUV_422_UYVY						=	13	,	/**< define input YUV interleave 422 mode: U Y V Y	*/
	YUV_422_VYUY						=	14	,	/**< define input YUV interleave 422 mode: V Y U Y  */

	YUV_420_PLANAR						=	20	,	/**< define input YUV planar 420 mode   */
	
	RGB_888_PACKED						=	30	,	/**< define input RGB 888 packet   */
	RGB_565_PACKED						=	31	,	/**< define input RGB 565 packet   */
	
	RGB_888_PLANAR						=	35		/**< define input RGB 888 PLANAR   */
} INPUT_COL_MODE;

/*! 
 * An enum.
 * descript the return value from encode
 * More detailed enum description.
 */
typedef enum {
	FAILED								= 0x80000000,  /**< error codes, <= 0							 */

	INVALID_ENC_FLAG					= 0x80000001,  /**< invalid input flag							 */
	INVALID_IN_BUF_ADR					= 0x80000002,  /**< invalid input buffer address				 */
	INVALID_IN_BUF_LEN                  = 0x80000003,  /**< invalid input buffer length					 */
	INVALID_DATA						= 0x80000004,  /**< invalid input buffer data					 */
	INVALID_MEM_MALLOC					= 0x80000004,  /**< encoding failures, no enough memery			 */

	FAILED_NO_ENOUGH_DATA 				= 0x80000005,  /**< encoding failures, no enough input raw data	 */
	FAILED_UNSUPORT_FEATURE				= 0x91000000,  /**< unsuport feature							 */

	FATAL_ERROR_END						= 0xffffffff, /**< fatal error end								 */

	SUCCEEDED							= 0x00000000  /**< success codes for carrying additional information, >=0  */
} RETURN_CODE;

/*! 
 * A structure.
 * descript the input raw YUV data infomation
 * More detailed structure description.
 */
typedef struct
{
	unsigned char *image_y;		/**< input image YUV Y, if interleave YUV, maybe has u ,v data */
	unsigned char *image_u;		/**< input image YUV U, if interleave YUV, it is NULL		   */
	unsigned char *image_v;		/**< input image YUV V, if interleave YUV, it is NULL		   */
} INPUT_PIC_DATA;

/*! 
 * A structure.
 * descript the input buffer and length infomation
 * and output encoded JPEG data and its length
 * More detailed structure description.
 */
typedef struct  
{
	unsigned char *outbuffer;	/**< input buffer point and output encoded image		*/
	unsigned int   runlength;	/**< input buffer length and out encoded image length	*/
}OUTPUT_PIC_DATA;

/*! 
 *
 * this function is the initialize JPEG encode function
 * it create JPEG encode engine and inititalize some paremeter
 * 
 * \param image_width  [in] initialize the width of picture
 * \param image_height [in] initialize the height of picture
 * \param image_type   [in] initialize the type of YUV mode, only can support YUVPlanar444, YUVPlanar422, YUVPlanar420
 * \param quality	   [in]	initialize the quality of encode JPEG, just in [1, 100], value is bigger, the encoded quality is more high 
 * \retval the enum of RETURN_CODE
 *
 * \remarks This application allows to manipulate JPEG encode engine. 
 * you can first create JPEG encode engine using function CJPEG_Init(),
 * and then use function CJPEG_Enc() encode YUV raw data to JPEG.
 * at last you should apply function CJPEG_Done() to free JPEG encode engine and buffer.
 * if you use intel IPP to optimize mode, you should add the intel's library ippSP_WMMX41PPC_r.lib 
 * and ippJP_WMMX41PPC_r.lib. 
 * the encode process example is follow:
 * \code
 * ...
 * INPUT_PIC_DATA inputdata;
 * OUTPUT_PIC_DATA outputdata;
 * ...
 * CJPEG_Encode_Init(240, 320, YUVPlanar420, 75);
 * ...
 * CJPEG_Encode(&inputdata, &outputdata);		
 * ...
 * CJPEG_Encode_Done();
 * ...
 * \endcode
 */
extern RETURN_CODE __cdecl CJPEG_Encode_Init(int image_width, int image_height, INPUT_COL_MODE image_type, int quality);

/*! 
 * this function is the encode JPEG core function.
 * input YUV raw data and encoded it to JPEG data.
 * 
 * \param Pic_data	  [in] input YUV raw data, and YUV is must planner mode, and YUV mode should be  4:4:4, or 4:2:2 or 4:2:0
 * \param output_data [in/out] input a enough buffer and length for encode JPEG data and at last
 *							   output a frame of encoded JPEG data and its length
 * \retval the enum of RETURN_CODE
 */
extern RETURN_CODE __cdecl CJPEG_Encode(INPUT_PIC_DATA* Pic_data, OUTPUT_PIC_DATA* output_data);

/*! 
 * this function is mainly free memery and JPEG encoded engine
 * no parameter is input or output.
 * \retval the enum of RETURN_CODE
 */
extern RETURN_CODE __cdecl CJPEG_Encode_Done();

#ifdef __cplusplus
}
#endif

#endif  //__RAWTOJPEG_H__
