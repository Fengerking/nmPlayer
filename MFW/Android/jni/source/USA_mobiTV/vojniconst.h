
#if !defined __VOJNI_CONST_H__
#define __VOJNI_CONST_H__

#include <utils/Log.h>

#define VOMAX_PATHLEN 256

// just for convenience
#define VOINFO(format, ...) do { \
	LOGI("%s::%s()->%d: " format "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); } while(0);


enum media_event_type {
    VOME_NOP               = 0, // interface test message
    VOME_PREPARED          = 1,
    VOME_PLAYBACK_COMPLETE = 2,
    VOME_BUFFERING_UPDATE  = 3,
    VOME_SEEK_COMPLETE     = 4,
    VOME_SET_VIDEO_SIZE    = 5,
    VOME_ERROR             = 100,
    VOME_INFO              = 200,
};

#define MEDIAEVENT_DRM_BASE		0x8610

enum media_error_type {
	VOME_ERROR_UNKNOWN = 1,
	VOME_ERROR_SERVER_DIED = 100,
	VOME_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 200,
	VOME_ERROR_DRM_EXPIRED	= (MEDIAEVENT_DRM_BASE | 0x1), 
	VOME_ERROR_DRM_NOAUTH = (MEDIAEVENT_DRM_BASE | 0x2), 
};

enum media_info_type {
	VOME_INFO_UNKNOWN = 1,
	VOME_INFO_VIDEO_TRACK_LAGGING = 700,
	VOME_INFO_BAD_INTERLEAVING = 800,
	VOME_INFO_NOT_SEEKABLE = 801,
	VOME_INFO_DRM_PURCHASE = (MEDIAEVENT_DRM_BASE | 0x1),	/* this type is reserved */
	VOME_INFO_DRM_RENTAL = (MEDIAEVENT_DRM_BASE | 0x2),	/* 0xRRRRUUTT R: reserved, U: used view count, T: total view count*/
};

#define OMX_COLOR_Format_QcomHW	0X7FA30C00


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef int64_t
typedef signed long long int64_t;
#endif // int64_t

/**
 * The source structure.
 */
typedef struct VOA_SOURCE {
    int			fd;			/*!< File ID  */
    int64_t		offset;		/*!< File begin pos  */
	int64_t		length;		/*!< File length  */
} VOA_SOURCE;

/**
 * The source structure.
 */
typedef struct VOA_NOTIFYEVENT {
    int					msg;		/*!< event ID  */
    int					ext1;		/*!< event param1  */
    int					ext2;		/*!< event param2  */
} VOA_NOTIFYEVENT;

/**
 * Defination of color format
 */
typedef enum
{
	VOA_COLOR_YUV420			= 0,		/*!< YUV planar mode:420  */
	VOA_COLOR_NV12				= 1,		/*!< YUV Y planar UV interlace */
	VOA_COLOR_RGB565			= 10,		/*!< RGB565  */
	VOA_COLOR_RGB888			= 11,		/*!< RGB888  */
	VOA_COLOR_TYPE_MAX			= 0X7FFFFFFF
} VOA_COLORTYPE;

/**
 * Video data buffer, usually used as iutput video information.
 */
typedef struct
{
	unsigned char *			virBuffer[3];			/*!< virtual Buffer pointer */
	unsigned char *			phyBuffer[3];			/*!< physical Buffer pointer */
	int						nStride[3];				/*!< Buffer stride */
	VOA_COLORTYPE			nColorType;				/*!< color type  */
} VOA_VIDEO_BUFFERTYPE;

/**
 * Audio format
 */
typedef struct
{
	int		nSampleRate;		/*!< Audio Samplerate */
	int		nChannels;			/*!< Audio Channels */
	int		nSampleBits;		/*!< Audio Sample Bits */
} VOA_AUDIO_FORMAT;

/**
 * VisualOn Android Engine Call Back function.
 * \param pUserData [IN] User data from caller.
 * \param nID [IN] The call back id.
 * \param pParam1 [IN/OUT] The call back param 1.
 * \param pParam2 [IN/OUT] The call back param 2.
 * \retval 0 succeeded..
 */
typedef int (* VOACallBack) (void * pUserData, int nID, void * pParam1, void * pParam2);

typedef void (* VOPACKUVAPI) (unsigned char* dstUV, unsigned char* srcU, unsigned char* srcV,
								int strideU, int strideV, int rows, int width, int stridedest);

#define	PLATFORM_PRIVATE_PMEM	1

// data structures for tunneling buffers
typedef struct PLATFORM_PRIVATE_PMEM_INFO
{
    /* pmem file descriptor */
    int		pmem_fd;
    int		offset;
} PLATFORM_PRIVATE_PMEM_INFO;

typedef struct PLATFORM_PRIVATE_ENTRY
{
    /* Entry type */
	int	 type;

    /* Pointer to platform specific entry */
    void * entry;
} PLATFORM_PRIVATE_ENTRY;

typedef struct PLATFORM_PRIVATE_LIST
{
    /* Number of entries */
	int nEntries;

    /* Pointer to array of platform specific entries *
     * Contiguous block of PLATFORM_PRIVATE_ENTRY elements */
    PLATFORM_PRIVATE_ENTRY* entryList;
} PLATFORM_PRIVATE_LIST;

/**
 * Defination of color format
 */
typedef enum
{
	VO_ANDROID_COLOR_YUV420				= 0,		/*!< YUV planar mode:420  */
	VO_ANDROID_COLOR_NV12				= 1,		/*!< YUV Y planar UV interlace */
	VO_ANDROID_COLOR_RGB565				= 10,		/*!< RGB565  */
	VO_ANDROID_COLOR_RGB888				= 11,		/*!< RGB888  */
	VO_ANDROID_COLOR_TYPE_MAX			= 0X7FFFFFFF
} VO_ANDROID_COLORTYPE;

/**
 * Video data buffer, usually used as iutput video information.
 */
typedef struct
{
	unsigned char *			virBuffer[3];			/*!< virtual Buffer pointer */
	unsigned char *			phyBuffer[3];			/*!< physical Buffer pointer */
	int						nStride[3];				/*!< Buffer stride */
	VO_ANDROID_COLORTYPE	nColorType;				/*!< color type  */
} VO_ANDROID_VIDEO_BUFFERTYPE;


/**
 * Video Render Type
 */
typedef enum
{
	VO_VIDEORENDER_AUTO			= 0,	    /*!< Auto detect VideoRender */
	VO_VIDEORENDER_STANDARD		= 1,	    /*!< Standard VideoRender */
	VO_VIDEORENDER_RGB565		= 2,		/*!< RGB565   VideoRender */
	VO_VIDEORENDER_TIOVERLAY	= 3,		/*!< TI Overlay  VideoRender */
	VO_VIDEORENDER_COPYBITS		= 4,		/*!< Copy Bits */
	VO_VIDEORENDER_JAVA			= 8			/*!< JAVA VideoRender */
} VO_VIDEORENDER_TYPE;


/**
* Audio Render Type
*/
typedef enum
{
	VO_AUDIORENDER_AUTO			= 0,	    /*!< Auto detect AideoRender */
	VO_AUDIORENDER_STANDARD		= 1,	    /*!< Standard AudioRender */
	VO_AUDIORENDER_JAVA	    	= 8			/*!< JAVA     AudioRender */	
} VO_AUDIORENDER_TYPE;


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif

