/** 
 * \file sdkbase.h
 * \brief Common constants, types, strucs of VisualOn Codec SDK.
 * \version 0.1
 * \date 11/04/2008 modify DDraw memory operator intface BFJ
 * \date 10/29/2008 added DDraw memory operator intface BFJ
 * \date 11/9/2007 added WMV9
 * \date 7/24/2007 added AC_AC3
 * \date 5/10/2007 added VORETURNCODE
 * \date 5/8/2007 typedef VOCOMMONRETURNCODE as int for compile compatible
 * \date 5/8/2007 added VORC_FAIL
 * \date 4/18/2007 added stream base code VO*_STREAM*_BASE
 * \date 4/18/2007 added VOAUDIOFORMAT, VOVIDEOFORMAT,
 * \date 4/18/2007 added VOAVCODEC, VOIMAGECODEC (move from filebase.h)
 * \date 4/18/2007 added VOAPI, VOCBI
 * \date 10/27/2006 created
 * \author VisualOn
 */


#ifndef __VO_SDK_BASE_H_
#define __VO_SDK_BASE_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(WIN32)||defined(_WIN32)
#    pragma pack(push, 4)	
#    define VOCODECAPI __cdecl
#	define VOAPI __cdecl
#	define VOCBI __stdcall
#else// defined(LINUX) || defined(RVDS) || defined(_IOS) || defined(_MAC_OS)
#	define VOCODECAPI 
#	define VOAPI 
#	define VOCBI 
#endif//defined(WIN32)||defined(_WIN32)


	
#define _MAKEBASE(base, type) \
VORC_##type##DEC_BASE =  _VOBASE_RC | _VOBASE_DEC | base, \
VORC_##type##ENC_BASE =  _VOBASE_RC | _VOBASE_ENC | base, \
VOID_##type##_BASE =  _VOBASE_ID | base, 

enum
{
#ifdef RVDS//some tool regard enum as unsigned value
	_VOBASE_RC     = 0x10000000,
#else//RVDS
	_VOBASE_RC     = 0x80000000,
#endif//RVDS
	_VOBASE_ID     = 0x40000000,
	_VOBASE_FILE   = 0x00000000, // set it to 0, it is default
	_VOBASE_STREAM = 0x20000000, 
	_VOBASE_DEC    = 0x01000000,
	_VOBASE_ENC    = 0x02000000,
	_VOBASE_READ   = 0x03000000,
	_VOBASE_WRITE  = 0x04000000,
	_VOBASE_PARSE  = 0x05000000,
	_VOBASE_SOURCE = 0x06000000,

	VORC_COM_BASE = _VOBASE_RC,
	VOID_COM_BASE = _VOBASE_ID,

	VORC_FILEREAD_BASE  = _VOBASE_RC	| _VOBASE_READ,
	VORC_FILEWRITE_BASE = _VOBASE_RC | _VOBASE_WRITE,
	VOID_FILEREAD_BASE  = _VOBASE_ID	| _VOBASE_READ,
	VOID_FILEWRITE_BASE = _VOBASE_ID | _VOBASE_WRITE,

	VORC_STREAMREAD_BASE   = _VOBASE_RC | _VOBASE_STREAM | _VOBASE_READ,
	VORC_STREAMPARSE_BASE  = _VOBASE_RC | _VOBASE_STREAM | _VOBASE_PARSE,
	VORC_STREAMSOURCE_BASE = _VOBASE_RC | _VOBASE_STREAM | _VOBASE_SOURCE,
	VOID_STREAMREAD_BASE   = _VOBASE_ID | _VOBASE_STREAM | _VOBASE_READ,
	VOID_STREAMPARSE_BASE  = _VOBASE_ID | _VOBASE_STREAM | _VOBASE_PARSE,
	VOID_STREAMSOURCE_BASE = _VOBASE_ID | _VOBASE_STREAM | _VOBASE_SOURCE,

	_MAKEBASE(0x00100000, H264)  
	_MAKEBASE(0x00200000, MPEG4) 
	_MAKEBASE(0x00300000, H263)
	_MAKEBASE(0x00400000, JPEG) 
	_MAKEBASE(0x00500000, MJPEG) 
	_MAKEBASE(0x00800000, AAC)
	_MAKEBASE(0x00900000, AMRNB) 
	_MAKEBASE(0x00A00000, AMRWB)
	_MAKEBASE(0x00700000, AMRWBPLUS) 
	_MAKEBASE(0x00B00000, MP3)
	_MAKEBASE(0x00C00000, QCP)  
	_MAKEBASE(0x00D00000, EVRC)  
	_MAKEBASE(0x00E00000, CC) 
	_MAKEBASE(0x00F00000, DIVX3)
	_MAKEBASE(0x00110000, MPEG2)	
	_MAKEBASE(0x00120000, OGG)	
	_MAKEBASE(0x00130000, WMV9)
	_MAKEBASE(0x00140000, DAEFFECT)
	_MAKEBASE(0x00150000, WMA)
        _MAKEBASE(0x00160000, FLAC)
};

/**
 * Should use a common type for all return code types 
 */
typedef int VORETURNCODE;

/**
 * Common return code
 */
enum _VOCOMMONRETURNCODE
{
	VORC_OK = 0,                                          /*!< No Error, everything OK */
	VORC_FAIL                       = VORC_COM_BASE,      /*!< General fail */
	VORC_COM_NOT_IMPLEMENT			= VORC_COM_BASE | 0x0001,   /*!< Not Implement */
	VORC_COM_INVALID_ARG			= VORC_COM_BASE | 0x0002,   /*!< Invalid argument, such as the handle is null, index out of range,not suported format */
	VORC_COM_INPUT_BUFFER_SMALL     = VORC_COM_BASE | 0x0003,   /*!< The input buffer is too small to decode */
	VORC_COM_OUTPUT_BUFFER_SMALL    = VORC_COM_BASE | 0x0004,   /*!< The output buffer is too small to fill */
	VORC_COM_OUT_OF_MEMORY			= VORC_COM_BASE | 0x0005,   /*!< Out of memory, cannot allocate memory */
	VORC_COM_WRONG_STATUS			= VORC_COM_BASE | 0x0006,   /*!< The function is not available at current status */
	VORC_COM_WRONG_PARAM_ID			= VORC_COM_BASE | 0x0007,   /*!< The parameter is not supported */
	VORC_COM_MODULE_NOT_FOUND       = VORC_COM_BASE | 0x0008,   /*!< Cannot find the specific module */
	VORC_COM_ROUTINE_NOT_FOUND      = VORC_COM_BASE | 0x0009,   /*!< Cannot find the specific routine(function) in a specific module */
	VORC_COM_NO_PROGRAM				= VORC_COM_BASE | 0x000A,	/*!< Cannot find the specific program */
	VORC_COM_QUERYMEMERROR				= VORC_COM_BASE | 0x000B,	/*!< Query memory error */
	
	VORC_COM_LICENSE_LIMIT			= VORC_COM_BASE | 0xffff,   /*!< License expired or a limited license */
};
typedef VORETURNCODE VOCOMMONRETURNCODE;


/**
 * Check the return code if succeeded or not.
 */
#define VORC_SUCCEEDED(rc) (rc >= 0)

/**
 * Check the return code if failed or not.
 */
#define VORC_FAILED(rc) (rc < 0)



/**
 * Common parameter ID
 */
typedef enum
{
	VOID_COM_AUDIO_FORMAT			= VOID_COM_BASE | 0x0001,  /*!< Audio format */
	VOID_COM_AUDIO_CHANNELS			= VOID_COM_BASE | 0x0002,  /*!< Audio channels */
	VOID_COM_AUDIO_SAMPLERATE		= VOID_COM_BASE | 0x0003,  /*!< Audio sample rate */
	VOID_COM_VIDEO_FORMAT			= VOID_COM_BASE | 0x0004,  /*!< Video format */
	VOID_COM_VIDEO_WIDTH			= VOID_COM_BASE | 0x0005,  /*!< Video width */
	VOID_COM_VIDEO_HEIGHT			= VOID_COM_BASE | 0x0006,  /*!< Video height */
	VOID_COM_HEAD_DATA				= VOID_COM_BASE | 0x0007,  /*!< The head data of decoder in track */
	VOID_COM_DURATION				= VOID_COM_BASE | 0x0008,  /*!< The Duration*/
	VOID_COM_VIDEO_FRAME_RATE		= VOID_COM_BASE	| 0x0009,  /*!< Video frame rate*/
	VOID_COM_VIDEO_FORMAT_INFO		= VOID_COM_BASE | 0x000A,  /*!< Video format information, PBITMAPINFOHEAD*/
	VOID_COM_AUDIO_FORMAT_INFO		= VOID_COM_BASE | 0x000B,  /*!< Audio format information, PWAVEFORMATEX*/
	VOID_COM_VIDEO_MEMORYOPERATOR	= VOID_COM_BASE | 0x000C,  /*!< Video memory operation for frame buffer*/
	VOID_COM_VIDEO_XDOWNSAMPLE		= VOID_COM_BASE | 0x000D,
	VOID_COM_VIDEO_YDOWNSAMPLE		= VOID_COM_BASE | 0x000E,
	VOID_COM_VIDEO_FULLSCREENFORMAT = VOID_COM_BASE | 0x000F,
	VOID_COM_VIDEO_QUERYMEM 		= VOID_COM_BASE | 0x0010,  /*!< Query memory structure from video codec*/
	VOID_COM_VIDEO_DDMEMORYOPERATOR	= VOID_COM_BASE | 0x0011   /*!< DDraw Video memory operation for frame buffer*/
}
VOCOMMONPARAMETERID;


/**
 * Frame type
 */
typedef enum
{
        I_FRAME                = 0,   /*!< I frame */
        P_FRAME                = 1,   /*!< P frame */
        B_FRAME                = 2,   /*!< B frame */
        S_FRAME                = 3,   /*!< S frame */
        NULL_FRAME             = 4,   /*!< NULL frame*/
		VOFRAMETYPE_END2           = 0x7fffffff,

}
VOFRAMETYPE;

#define DEC_MAX_NUM_REF_FRAME 10

typedef struct 
{
	unsigned int decPictureWidth;				/*!< picture width offered outside*/
	unsigned int decPictureHeight;			/*!< picture height offered outside*/
	unsigned int decNumRefFrames;				/*!< number of reference and reconstruction  frames */
	unsigned int decReferenceFrameWidth;			/*!< including padding */
	unsigned int decReferenceFrameHeight;			/*!< including padding */
	unsigned int decPrivateDataSize;			/*!< size in byte */
	unsigned char *decReferenceFrames[DEC_MAX_NUM_REF_FRAME][3];
	void *pDecPrivateData;
}VOVIDEODECQUERYMEM;

typedef enum
{
	STEREO			= 0,
	DUALMONO		= 2,
	MONO			= 3,
	MULTICH			= 5,
}VOCHANNELMODE;

/**
 * Video type
 */
typedef enum
{
	VOYUV_PLANAR444		= 0,		/*!< YUV planar mode:444  vertical sample is 1, horizontal is 1  */
	VOYUV_PLANAR422_12	= 1,		/*!< YUV planar mode:422, vertical sample is 1, horizontal is 2  */
	VOYUV_PLANAR422_21	= 2,		/*!< YUV planar mode:422  vertical sample is 2, horizontal is 1  */
	VOYUV_PLANAR420		= 3,		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  */
	VOYUV_PLANAR411		= 4,		/*!< YUV planar mode:411  vertical sample is 1, horizontal is 4  */
	VOGRAY_PLANARGRAY		= 5,		/*!< gray planar mode, just Y value								 */	
	VOYUYV422_PACKED		= 6,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y0 U0 Y1 V0  */
	VOYVYU422_PACKED		= 7,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y0 V0 Y1 U0  */
	VOUYVY422_PACKED		= 8,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: U0 Y0 V0 Y1  */
	VOVYUY422_PACKED		= 9,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: V0 Y0 U0 Y1  */		
	VOYUV444_PACKED			= 10,		/*!< YUV packed mode:444, vertical sample is 1, horizontal is 1, data: Y U V	*/
	VOYUV_420_PACK			= 11, 		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  , Y planar, UV Packed*/
	VOYUV_420_PACK_2		= 35, 		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  , Y planar, VU Packed*/
	VOYVU_PLANAR420			= 12,
	VOYVU_PLANAR422_12		= 13,
	VOYUYV422_PACKED_2		= 14,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y1 U0 Y0 V0  */
	VOYVYU422_PACKED_2		= 15,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y1 V0 Y0 U0  */
	VOUYVY422_PACKED_2		= 16,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: U0 Y1 V0 Y0  */
	VOVYUY422_PACKED_2		= 17,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: V0 Y1 U0 Y0  */
	VORGB565_PACKED		= 30,		/*!< RGB packed mode, data: B:5 G:6 R:5   						 */
	VORGB555_PACKED		= 31,		/*!< RGB packed mode, data: B:5 G:5 R:5   						 */
	VORGB888_PACKED		= 32,		/*!< RGB packed mode, data: B G R		 						 */
	VORGB32_PACKED		= 33,		/*!< RGB packed mode, data: B G R A								 */
	VORGB888_PLANAR		= 34	,	/*!< RGB planar mode											 */
	VOVIDEOTYPE_END2           = 0x7fffffff,

}
VOVIDEOTYPE;

/**
 * Defination of Video Encoder quality level
 *
 */
typedef enum
{
        VIDEO_LOW_QUALITY      = 0,   /*!< low quality */
        VIDEO_MID_QUALITY      = 1,   /*!< middle quality */
        VIDEO_HIGH_QUALITY     = 2 ,   /*!< high quality */
		VOVIDEOENCQUALITY_END2           = 0x7fffffff,

}VOVIDEOENCQUALITY;


/**
 * Codec handle, an instance of a decoder/encoder.
 */
typedef void* HVOCODEC;



/**
 * General data buffer, used as input or output.
 */
typedef struct 
{
	unsigned char *buffer;  /*!< Buffer pointer */
	signed long length;   /*!< Buffer size in byte */
}
VOCODECDATABUFFER;

/*!
* Defination of video rotation type  
*
*/
typedef enum
{
	RT_DISABLE	    = 0,  /*!< No rotation			*/
	RT_90L		    = 1,  /*!< Left rotation 90		*/
	RT_90R          = 2,  /*!< Right rotation 90	*/
	RT_180			= 3,  /*!< Left rotation 180	*/
	RT_FLIP_Y		= 4,  /*!< Y direction flip		*/
	RT_FLIP_X		= 5,  /*!< X direction flip		*/
}RTTYPE;


/**
 * Video data buffer, usually used as iutput or output of video codec.
 */
typedef struct
{
	unsigned char *data_buf[3];			/*!< Buffer pointer */
	int  stride[3];				/*!< Buffer stride */
	VOVIDEOTYPE video_type;		/*!< Video type */
}
VOCODECVIDEOBUFFER;


/**
 * General audio format info
 */
typedef struct
{
	int sample_rate;  /*!< Sample rate */
	int channels;    /*!< Channel count */
	int sample_bits;  /*!< Bits per sample */
}
VOCODECAUDIOFORMAT;

typedef VOCODECAUDIOFORMAT VOAUDIOFORMAT;


/**
 * General video format info
 */
typedef struct
{
	int width;   /*!< Width */
	int height;  /*!< Height */
	VOFRAMETYPE frame_type; /*!< Frame type, such as I frame, P frame */
} VOCODECVIDEOFORMAT;

typedef VOCODECVIDEOFORMAT VOVIDEOFORMAT;

/**
 *  Audio Video Codec
 */
typedef enum
{
		_VC_MASK                = 0x0000ffff,   /*!< Video codec mask, internally use only. You should use VOCODEC_ISVIDEO */
		_AC_MASK                = 0xffff0000,   /*!< Audio codec mask, internally use only. You should use VOCODEC_ISAUDIO */

        VC_UNKNOWN				= 0X00000000,   /*!< Unknown video codec*/
        VC_H264					= 0X00000001,   /*!< H264 video codec*/
        VC_AVC                  = 0X0000fff1,   /*!< H264, 14496-15 */
        VC_H263					= 0X00000002,   /*!< H263 video codec*/
        VC_H261					= 0X00000004,   /*!< H261 video codec*/
        VC_MPEG4				= 0X00000010,   /*!< MPEG4 video codec*/
        VC_MPEG2				= 0X00000020,   /*!< MPEG2 video codec*/
        VC_MPEG1				= 0X00000040,   /*!< MPEG1 video codec*/
        VC_MJPEG				= 0X00000100,   /*!< Motion Jpeg video codec*/
        VC_WMV					= 0X00000200,   /*!< MS WMV video codec*/
        VC_DIVX					= 0X00000400,   /*!< Divx video codec*/
        VC_RV					= 0X00000800,   /*!< Real video codec*/
		VC_RICHMEDIAXML         = 0X00001000,   /*!< richmedia+xml*/
        AC_AAC					= 0X00010000,   /*!< AAC audio codec*/
        AC_AMR					= 0X00020000,   /*!< AMR NB audio codec*/
        AC_AWB					= 0X00040000,   /*!< AMR WB audio codec*/
        AC_AWBP					= 0X00050000,   /*!< AMR WB audio codec*/
        AC_QCELP				= 0X00080000,   /*!< QCELP audio codec*/
        AC_EVRC					= 0X00100000,   /*!< EVRC audio codec*/
        AC_MP3					= 0X00200000,   /*!< MP3 audio codec*/
        AC_MP2					= 0X00400000,   /*!< MP2 audio codec*/
        AC_MP1					= 0X00800000,   /*!< MP1 audio codec*/
		AC_OGG					= 0X00900000,   /*!< ogg audio codec*/
        AC_WMA					= 0X01000000,   /*!< MS WMA audio codec*/
        AC_MIDI					= 0X02000000,   /*!< MIDI audio codec*/
        AC_RA					= 0X04000000,   /*!< Real Audio audio codec*/
        AC_QTA					= 0X08000000,   /*!< QuickTime audio codec*/
        AC_PCM					= 0X10000000,   /*!< PCM audio codec*/
        AC_LPCM					= 0X20000000,   /*!< LPCM audio codec*/
        AC_ADPCM				= 0X40000000,   /*!< LPCM audio codec*/
        AC_AC3					= 0X80000000,   /*!< AC3 audio codec*/
		AC_FLAC					= 0x80000001,	/*!< FLAC audio codec*/
}VOAVCODEC;

/**
 *  Codec type, including video, audio
 */
#define VOCODECTYPE VOAVCODEC

/**
 * Check the codec type is video or not.
 */
#define VOCODEC_ISVIDEO(c) (c & _VC_MASK)


/**
 * Check the codec type is audio or not.
 */
#define VOCODEC_ISAUDIO(c) (c & _AC_MASK)


/**
 * Image Codec
 */
typedef enum
{
        IC_UNKNOWN				= 0X00000000,   /*!< Unknown image codec*/
        IC_JPEG					= 0X01000000,   /*!< Jpeg image codec*/
        IC_BMP					= 0X02000000,   /*!< BMP image codec */
        IC_GIF					= 0X04000000,   /*!< GIF image codec */
        IC_PNG					= 0X08000000,   /*!< PNG image codec */
        IC_TIFF					= 0X10000000,   /*!< TIFF image codec */
        IC_PCX					= 0X20000000,   /*!< PCX image codec */
        IC_J2000				= 0X40000000,   /*!< JPEG 2000 image codec */
}VOIMAGECODEC;

/**
 * Memory operating functions
 */
typedef struct
{
	void* (VOAPI *Alloc)(int size, unsigned int* pPhysAddr);
	void (VOAPI *Free) (void* buffer);
	void (VOAPI *Lock) (void* buffer);
	void (VOAPI *Unlock) (void* buffer);
}
VOMEMORYOPERATOR;

/**
 * DDraw Memory operating functions
 */
#define	VOMEMOP_DD_SURFACE		1
#define	VOMEMOP_DD_PHYSADDR		2

typedef struct
{
	int	 (VOAPI * SetFrameCount) (int nWidth, int nHeight, int nFrames);				/*!< return value > 0 OK, */
	void * (VOAPI * Alloc)(int nWidth, int nHeight, VOCODECVIDEOBUFFER * pVideoInfo);
	void (VOAPI * Free) (void * pBuffer);												/*!< pBuffer was returned by Alloc */
	int  (VOAPI * Lock) (void * pBuffer);												/*!< pBuffer was returned by Alloc */
	void (VOAPI * Unlock) (void * pBuffer);												/*!< pBuffer was returned by Alloc */
	int  (VOAPI * SetParam) (void * pBuffer, int nID, int nValue);						/*!< pBuffer was returned by Alloc */
	int  (VOAPI * GetParam) (void * pBuffer, int nID, int * pValue);					/*!< pBuffer was returned by Alloc */
}
VODDMEMORYOPERATOR;

#if defined(WIN32)||defined(_WIN32)
#	pragma pack(pop)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* __VO_SDK_BASE_H_ */



