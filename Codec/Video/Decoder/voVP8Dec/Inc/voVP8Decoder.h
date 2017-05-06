#ifdef __cplusplus
extern "C" {
#endif

#ifndef VPX_DECODER_H
#define VPX_DECODER_H
#include "vpx_image.h"
#include "voVP8.h"
#include "voVP8GlobleDef.h"


typedef void   *VP8D_PTR;
typedef enum 
{
    VPX_CODEC_OK,
    VPX_CODEC_ERROR,
    VPX_CODEC_MEM_ERROR,
    VPX_CODEC_ABI_MISMATCH,
    VPX_CODEC_INCAPABLE,
    VPX_CODEC_UNSUP_BITSTREAM,
    VPX_CODEC_UNSUP_FEATURE,
    VPX_CODEC_CORRUPT_FRAME,
    VPX_CODEC_INVALID_PARAM,
    VPX_CODEC_LIST_END,
} vpx_codec_err_t;

typedef struct
{
    int     Width;
    int     Height;
    int     Version;
    int     postprocess;
    int     max_threads;
	VO_CODEC_INIT_USERDATA * pUserData;
	 long nCodecIdx;
} VP8D_CONFIG;

typedef struct
{
    unsigned int sz;     /**< Size of this structure */
    unsigned int w;      /**< Width (or 0 for unknown/default) */
    unsigned int h;      /**< Height (or 0 for unknown/default) */
    unsigned int is_kf;  /**< Current frame is a keyframe */
} vpx_codec_stream_info_t;

#define VP8BORDERINPIXELS       32
typedef enum
{
    REG_YUV = 0,    // Regular yuv
    INT_YUV = 1     // The type of yuv that can be tranfer to and from RGB through integer transform
}YUV_TYPE;

typedef struct
{
    int   y_width;
    int   y_height;
    int   y_stride;
    int   uv_width;
    int   uv_height;
    int   uv_stride;
    unsigned char *y_buffer;
    unsigned char *u_buffer;
    unsigned char *v_buffer;
    unsigned char *buffer_alloc;
    int border;
    int frame_size;

	VO_CODEC_INIT_USERDATA * pUserData;
	 long nCodecIdx;
} YV12_BUFFER_CONFIG;

typedef const void *vpx_codec_iter_t;
typedef struct
{
    const char *name;        /**< Printable interface name */
    long init_flags;  /**< Flags passed at init time */
	void*  pbi;
	vpx_codec_stream_info_t   si;
	vpx_image_t img;
     int img_avail;
	 int decoder_init;
	 unsigned char* framebuffer;
#ifdef LICENSEFILE
	 void* phCheck;//checkLib
	 VO_VIDEO_BUFFER *pOutVideoBuf;
#endif
	 unsigned long   framelength;
	 int     nthreads;
	 int disable_deblock;
	 VO_CODEC_INIT_USERDATA * pUserData;
	 long nCodecIdx;
	 VO_S64     nTimes;
	 VO_PTR  m_inUserData;
} vpx_codec_ctx_t;



vpx_codec_err_t VO_VP8_Decoder(vpx_codec_ctx_t *ctx, const unsigned char *data, unsigned int data_sz, void *user_priv, long deadline);
vpx_image_t *VO_VP8_GetFrame(vpx_codec_ctx_t *ctx, vpx_codec_iter_t *iter);
void vp8dx_remove_decompressor(VP8D_PTR ptr);

#endif

#ifdef __cplusplus
}
#endif

//#include "vpx_decoder_compat.h"
