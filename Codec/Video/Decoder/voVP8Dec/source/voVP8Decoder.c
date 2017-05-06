#include "stdlib.h"
#include "voVP8Decoder.h"
#include "voVP8DecFrame.h"
#include "voVP8Memory.h"


extern int voVP8CreateThreads(VP8D_COMP *pbi);
extern int voVP8DestoryThreads(VP8D_COMP *pbi);

extern void vp8_loop_filter_frame(VP8D_COMP *pbi,VP8_COMMON *cm,    MACROBLOCKD *mbd,    int default_filt_lvl);
extern void vp8_init_loop_filter(VP8_COMMON *cm);
extern void vp8cx_init_de_quantizer(VP8D_COMP *pbi);

static vpx_image_t *img_alloc_helper(vpx_image_t  *img,
                                     img_fmt_t     fmt,
                                     unsigned int  d_w,
                                     unsigned int  d_h,
                                     unsigned int  stride_align,
                                     unsigned char      *img_data)
{

    unsigned int  h, w, s, xcs=1, ycs=1, bps=12;
    int  align = (1 << xcs) - 1;

    /* Calculate storage sizes given the chroma subsampling */
    w = (d_w + align) & ~align;
    align = (1 << ycs) - 1;
    h = (d_h + align) & ~align;
    s = (fmt & IMG_FMT_PLANAR) ? w : bps * w / 8;
    s = (s + stride_align - 1) & ~(stride_align - 1);

    /* Allocate the new image */   
    memset(img, 0, sizeof(vpx_image_t));

    img->img_data = img_data;

    img->fmt = fmt;
    img->w = w;
    img->h = h;
    img->x_chroma_shift = xcs;
    img->y_chroma_shift = ycs;
    img->bps = bps;

    /* Calculate strides */
    img->stride[PLANE_Y] = img->stride[PLANE_ALPHA] = s;
    img->stride[PLANE_U] = img->stride[PLANE_V] = s >> xcs;

    /* Default viewport to entire image */
    if (!vpx_img_set_rect(img, 0, 0, d_w, d_h))
        return img;
	else
		return NULL;
}

vpx_codec_err_t vp8_peek_si(const unsigned char *data, unsigned int data_sz, vpx_codec_stream_info_t *si)
{
    vpx_codec_err_t res = VPX_CODEC_OK;
    {
        si->is_kf = 0;
        if (data_sz >= 10 && !(data[0] & 0x01))  /* I-Frame */
        {
            const unsigned char *c = data + 3;
            si->is_kf = 1;

            // vet via sync code
            if (c[0] != 0x9d || c[1] != 0x01 || c[2] != 0x2a)
                res = VPX_CODEC_UNSUP_BITSTREAM;

            si->w = (*(const unsigned short *)(c + 3)) & 0x3fff;
            si->h = (*(const unsigned short *)(c + 5)) & 0x3fff;
            if (!(si->h | si->w))
                res = VPX_CODEC_UNSUP_BITSTREAM;
        }
        else
            res = VPX_CODEC_UNSUP_BITSTREAM;
    }
    return res;
}

vpx_image_t *vpx_img_wrap(vpx_image_t  *img, img_fmt_t fmt, unsigned int d_w, unsigned int  d_h, unsigned int  stride_align, unsigned char *img_data)
{
    return img_alloc_helper(img, fmt, d_w, d_h, stride_align, img_data);;
}

int vpx_img_set_rect(vpx_image_t  *img,
                     unsigned int  x,
                     unsigned int  y,
                     unsigned int  w,
                     unsigned int  h)
{
    unsigned char      *data;

    if (x + w <= img->w && y + h <= img->h)
    {
        img->d_w = w;
        img->d_h = h;
        data = img->img_data;
        if (img->fmt & IMG_FMT_HAS_ALPHA)
        {
            img->planes[PLANE_ALPHA] =
                data + x + y * img->stride[PLANE_ALPHA];
            data += img->h * img->stride[PLANE_ALPHA];
        }
        img->planes[PLANE_Y] = data + x + y * img->stride[PLANE_Y];
        data += img->h * img->stride[PLANE_Y];

        img->planes[PLANE_U] = data
                               + (x >> img->x_chroma_shift)
                               + (y >> img->y_chroma_shift) * img->stride[PLANE_U];
        data += (img->h >> img->y_chroma_shift) * img->stride[PLANE_U];
        img->planes[PLANE_V] = data
                               + (x >> img->x_chroma_shift)
                               + (y >> img->y_chroma_shift) * img->stride[PLANE_V];            
        return 0;
    }
	else
		return -1;
}
int vp8dx_get_raw_frame(VP8D_PTR ptr, YV12_BUFFER_CONFIG *sd, unsigned long *time_stamp, unsigned long *time_end_stamp)
{
    int ret = -1;
    VP8D_COMP *pbi = (VP8D_COMP *) ptr;

    if (pbi->ready_for_new_data == 1)
        return ret;

    // ie no raw frame to show!!!
    if (pbi->common.show_frame == 0)
        return ret;

    pbi->ready_for_new_data = 1;
    *time_stamp = pbi->last_time_stamp;
    *time_end_stamp = 0;

    if (pbi->common.frame_to_show)
    {
        *sd = *pbi->common.frame_to_show;
        sd->y_width = pbi->common.Width;
        sd->y_height = pbi->common.Height;
        sd->uv_height = pbi->common.Height / 2;
        ret = 0;
    }
    else
    {
        ret = -1;
    }
    return ret;
}
int get_free_fb (VP8_COMMON *cm)
{
    int i;
    for (i = 0; i < NUM_YV12_BUFFERS; i++)
        if (cm->fb_idx_ref_cnt[i] == 0)
            break;

    cm->fb_idx_ref_cnt[i] = 1;
    return i;
}

static void ref_cnt_fb (int *buf, int *idx, int new_idx)
{
    if (buf[*idx] > 0)
        buf[*idx]--;

    *idx = new_idx;

    buf[new_idx]++;
}
VP8D_PTR vp8dx_create_decompressor(VP8D_CONFIG *oxcf)
{
	int i=0;
	VP8D_COMP *pbi =   (VP8D_COMP*)MallocMem(oxcf->pUserData, oxcf->nCodecIdx, sizeof(VP8D_COMP), CACHE_LINE);

    if (!pbi)
        return NULL;
	SetMem(oxcf->pUserData, oxcf->nCodecIdx, (VO_U8 *)pbi, 0, sizeof(VP8D_COMP));


	pbi->pUserData = oxcf->pUserData;
	pbi->nCodecIdx = oxcf->nCodecIdx;

    vp8_initialize_common();

    vp8_create_common(&pbi->common);

    pbi->common.current_video_frame = 0;
    pbi->ready_for_new_data = 1;

	pbi->max_threads = oxcf->max_threads;

	//threads create 
	if(pbi->max_threads == 2)
		voVP8CreateThreads(pbi);

    // unnecessary calling of vp8cx_init_de_quantizer() for every frame.
    vp8cx_init_de_quantizer(pbi);

    {
        VP8_COMMON *cm = &pbi->common;

        vp8_init_loop_filter(cm);
        cm->last_frame_type = KEY_FRAME;
        cm->last_filter_type = cm->filter_type;
        cm->last_sharpness_level = cm->sharpness_level;
    }
#ifdef NEWTHREAD
	pbi->mbrow = (MACROBLOCKD*)MallocMem(oxcf->pUserData, oxcf->nCodecIdx, ((oxcf->Width + 16 - (oxcf->Width & 0xf))>>4)*sizeof(MACROBLOCKD), CACHE_LINE);
	pbi->mbrow2 = (MACROBLOCKD*)MallocMem(oxcf->pUserData, oxcf->nCodecIdx, ((oxcf->Width + 16 - (oxcf->Width & 0xf))>>4)*sizeof(MACROBLOCKD), CACHE_LINE);

	for(i=0;i< ((oxcf->Width + 16 - (oxcf->Width & 0xf))>>4);i++)
	{
		memset(pbi->mbrow[i].qcoeff,0,sizeof(short)*400);
		memset(pbi->mbrow2[i].qcoeff,0,sizeof(short)*400);
	}
	
#endif
    return (VP8D_PTR) pbi; 
}
void vp8dx_remove_decompressor(VP8D_PTR ptr)
{
	 VP8D_COMP *pbi = (VP8D_COMP *) ptr;

    if (!pbi)
        return;

	voVP8DestoryThreads(pbi);

    vp8_remove_common(&pbi->common);

#ifdef NEWTHREAD
	FreeMem(pbi->pUserData, pbi->nCodecIdx, pbi->mbrow);
	FreeMem(pbi->pUserData, pbi->nCodecIdx, pbi->mbrow2);
#endif
	FreeMem(pbi->pUserData, pbi->nCodecIdx, pbi);

	return;
}
static int swap_frame_buffers (VP8_COMMON *cm)
{
    int fb_to_update_with, err = 0;

    if (cm->refresh_last_frame)
        fb_to_update_with = cm->lst_fb_idx;
    else
        fb_to_update_with = cm->new_fb_idx;

    // The alternate reference frame or golden frame can be updated
    //  using the new, last, or golden/alt ref frame.  If it
    //  is updated using the newly decoded frame it is a refresh.
    //  An update using the last or golden/alt ref frame is a copy.
    if (cm->copy_buffer_to_arf)
    {
        int new_fb = 0;

        if (cm->copy_buffer_to_arf == 1)
            new_fb = fb_to_update_with;
        else if (cm->copy_buffer_to_arf == 2)
            new_fb = cm->gld_fb_idx;
        else
            err = -1;

        ref_cnt_fb (cm->fb_idx_ref_cnt, &cm->alt_fb_idx, new_fb);
    }

    if (cm->copy_buffer_to_gf)
    {
        int new_fb = 0;

        if (cm->copy_buffer_to_gf == 1)
            new_fb = fb_to_update_with;
        else if (cm->copy_buffer_to_gf == 2)
            new_fb = cm->alt_fb_idx;
        else
            err = -1;

        ref_cnt_fb (cm->fb_idx_ref_cnt, &cm->gld_fb_idx, new_fb);
    }

    if (cm->refresh_golden_frame)
        ref_cnt_fb (cm->fb_idx_ref_cnt, &cm->gld_fb_idx, cm->new_fb_idx);

    if (cm->refresh_alt_ref_frame)
        ref_cnt_fb (cm->fb_idx_ref_cnt, &cm->alt_fb_idx, cm->new_fb_idx);

    if (cm->refresh_last_frame)
    {
        ref_cnt_fb (cm->fb_idx_ref_cnt, &cm->lst_fb_idx, cm->new_fb_idx);

        cm->frame_to_show = &cm->yv12_fb[cm->lst_fb_idx];
    }
    else
        cm->frame_to_show = &cm->yv12_fb[cm->new_fb_idx];

    cm->fb_idx_ref_cnt[cm->new_fb_idx]--;

    return err;
}
int vp8dx_receive_compressed_data(VP8D_PTR ptr, unsigned long size, const unsigned char *source, unsigned long time_stamp)
{
	VP8D_COMP *pbi = (VP8D_COMP *) ptr;
	VP8_COMMON *cm = &pbi->common;
	int retcode = 0;

    if (ptr == 0)
        return -1;
    pbi->Source = source;
    pbi->source_sz = size;
    cm->new_fb_idx = get_free_fb (cm);

#ifdef STABILITY
	if(cm->new_fb_idx>3)
	{
		cm->fb_idx_ref_cnt[0] = cm->fb_idx_ref_cnt[1] =cm->fb_idx_ref_cnt[2] =cm->fb_idx_ref_cnt[3] =0;
		return VPX_CODEC_CORRUPT_FRAME;  //stability
	}
#endif

    retcode = vp8_decode_frame(pbi);  //75%
    if (retcode != VPX_CODEC_OK)
        return retcode;


    if (swap_frame_buffers (cm))
        return -1;

	if(!pbi->disable_deblock)
	{
		if (cm->filter_level > 0)
		{
			vp8_loop_filter_frame(pbi, cm, &pbi->mb, cm->filter_level); //20%
		}
	}

    if (cm->filter_level > 0) {
        cm->last_frame_type = cm->frame_type;
        cm->last_filter_type = cm->filter_type;
        cm->last_sharpness_level = cm->sharpness_level;
    }

    vp8_yv12_extend_frame_borders(cm->frame_to_show); //4.18%

    if (cm->show_frame)
        cm->current_video_frame++;

    pbi->ready_for_new_data = 0;
    pbi->last_time_stamp = time_stamp;
    return retcode;
}

vpx_codec_err_t VO_VP8_Decoder(vpx_codec_ctx_t *ctx,
                                 const unsigned char *data,
                                 unsigned int data_sz,
                                 void *user_priv,
                                 long deadline)
{
	 vpx_codec_err_t res = VPX_CODEC_OK;
	if (!ctx || !data || !data_sz)
		return VPX_CODEC_INVALID_PARAM;	
    ctx->img_avail = 0;

    if (!ctx->si.h)
        res = vp8_peek_si(data, data_sz, &ctx->si);

    /* Initialize the decoder instance on the first frame*/
    if (!res && !ctx->decoder_init)
    {
		
        VP8D_CONFIG oxcf;
        VP8D_PTR optr;
        oxcf.Width = ctx->si.w;
        oxcf.Height = ctx->si.h;
        oxcf.Version = 9;
        oxcf.postprocess = 0;
        oxcf.max_threads = ctx->nthreads;//zou
		oxcf.pUserData  = ctx->pUserData;
		oxcf.nCodecIdx  = ctx->nCodecIdx;

        optr = vp8dx_create_decompressor(&oxcf);
        if (!optr)
            res = VPX_CODEC_ERROR;
        else
			ctx ->pbi  = optr;
        ctx->decoder_init = 1;
    }

    if (!res && ctx->pbi)
    {
		VP8D_COMP *pbi = (VP8D_COMP *) ctx->pbi;
		YV12_BUFFER_CONFIG sd;
        unsigned long time_stamp = 0, time_end_stamp = 0;

		pbi->disable_deblock = ctx->disable_deblock;
		res = vp8dx_receive_compressed_data(ctx->pbi, data_sz, data, deadline);		

        if (!res && 0 == vp8dx_get_raw_frame(ctx->pbi, &sd, &time_stamp, &time_end_stamp))
        {
			unsigned int a_w = (sd.y_width + 15) & ~15;
            unsigned int a_h = (sd.y_height + 15) & ~15;

            vpx_img_wrap(&ctx->img, IMG_FMT_I420, a_w + 2 * VP8BORDERINPIXELS, a_h + 2 * VP8BORDERINPIXELS, 1, sd.buffer_alloc);

            vpx_img_set_rect(&ctx->img, VP8BORDERINPIXELS, VP8BORDERINPIXELS, sd.y_width, sd.y_height);
            ctx->img_avail = 1;
        }
		else
		{
			res = -1;
		}
    }
    return res;
}
vpx_image_t *vp8_get_frame(vpx_codec_ctx_t  *ctx,
                                  vpx_codec_iter_t      *iter)
{
    vpx_image_t *img = NULL;

    if (ctx->img_avail)
    {
        /* iter acts as a flip flop, so an image is only returned on the first
         * call to get_frame.
         */
        if (!(*iter))
        {
            img = &ctx->img;
            *iter = img;
        }
    }

    return img;
}

vpx_image_t *VO_VP8_GetFrame(vpx_codec_ctx_t *ctx, vpx_codec_iter_t *iter)
{
    vpx_image_t *img;

    if (!ctx || !iter)
        img = NULL;
    else
		img = vp8_get_frame(ctx, iter);

    return img;
}