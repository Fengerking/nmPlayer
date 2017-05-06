#include "voVP8Common.h"
#include "voVP8Memory.h"
#include "voVP8DecInterPolationFilter.h"
#include "voVP8DecMV.h"

extern void vo_Extend_Broder_y_neon(VO_U8*src_ptr1,VO_U8*src_ptr2,int plane_stride,int plane_height );
extern void vo_Extend_Broder_uv_neon(VO_U8*src_ptr1,VO_U8*src_ptr2,int plane_stride,int plane_height );
extern const int vp8_default_zig_zag1d[16];

int vp8_yv12_de_alloc_frame_buffer(YV12_BUFFER_CONFIG *ybf)
{
    if (ybf)
    {
        if (ybf->buffer_alloc)
        {
			FreeMem(ybf->pUserData, ybf->nCodecIdx, ybf->buffer_alloc);
        }

        ybf->buffer_alloc = 0;
    }
    else
    {
        return -1;
    }

    return 0;
}
/****************************************************************************
 *
 ****************************************************************************/
int vp8_yv12_alloc_frame_buffer(YV12_BUFFER_CONFIG *ybf, int width, int height, int border)
{
//NOTE:

    int yplane_size = (height + 2 * border) * (width + 2 * border);
    int uvplane_size = ((1 + height) / 2 + border) * ((1 + width) / 2 + border);

    if (ybf)
    {
        vp8_yv12_de_alloc_frame_buffer(ybf);

        ybf->y_width  = width;
        ybf->y_height = height;
        ybf->y_stride = width + 2 * border;

        ybf->uv_width = (1 + width) / 2;
        ybf->uv_height = (1 + height) / 2;
        ybf->uv_stride = ybf->uv_width + border;

        ybf->border = border;
        ybf->frame_size = yplane_size + 2 * uvplane_size;

        // Added 2 extra lines to framebuffer so that copy12x12 doesn't fail
        // when we have a large motion vector in V on the last v block.
        // Note : We never use these pixels anyway so this doesn't hurt.
        //ybf->buffer_alloc = (unsigned char *) duck_memalign(32,  ybf->frame_size + (ybf->y_stride * 2) + 32, 0);
		ybf->buffer_alloc = (unsigned char*)MallocMem(ybf->pUserData, ybf->nCodecIdx, ybf->frame_size + (ybf->y_stride * 2), CACHE_LINE);
		if(!ybf->buffer_alloc)
			return VO_ERR_OUTOF_MEMORY;
		SetMem(ybf->pUserData, ybf->nCodecIdx,ybf->buffer_alloc,0,ybf->frame_size + (ybf->y_stride * 2));
		//SetMem(pUserData, nCodecIdx, (VO_U8 *)decoder, 0, sizeof(vpx_codec_ctx_t));

        if (ybf->buffer_alloc == 0)
            return -1;

        ybf->y_buffer = ybf->buffer_alloc + (border * ybf->y_stride) + border;

        if (yplane_size & 0xf)
            yplane_size += 16 - (yplane_size & 0xf);

        ybf->u_buffer = ybf->buffer_alloc + yplane_size + (border / 2  * ybf->uv_stride) + border / 2;
        ybf->v_buffer = ybf->buffer_alloc + yplane_size + uvplane_size + (border / 2  * ybf->uv_stride) + border / 2;
    }
    else
    {
        return -2;
    }

    return 0;
}

void vp8_de_alloc_frame_buffers(VP8_COMMON *oci)
{
    int i;

    for (i = 0; i < NUM_YV12_BUFFERS; i++)
        vp8_yv12_de_alloc_frame_buffer(&oci->yv12_fb[i]);
	if(oci->above_context)
		FreeMem(oci->pUserData, oci->nCodecIdx, oci->above_context);
	if(oci->mip)
		FreeMem(oci->pUserData, oci->nCodecIdx, oci->mip);
	oci->above_context = 0;
    oci->mip = 0;
}

int vp8_alloc_frame_buffers(VP8_COMMON *oci, int width, int height)
{
    int i;

    vp8_de_alloc_frame_buffers(oci);

    // our internal buffers are always multiples of 16
    if ((width & 0xf) != 0)
        width += 16 - (width & 0xf);

    if ((height & 0xf) != 0)
        height += 16 - (height & 0xf);


    for (i = 0; i < NUM_YV12_BUFFERS; i++)
    {
      oci->fb_idx_ref_cnt[0] = 0;

	  oci->yv12_fb[i].pUserData = oci->pUserData;
	  oci->yv12_fb[i].nCodecIdx =  oci->nCodecIdx;

      if (vp8_yv12_alloc_frame_buffer(&oci->yv12_fb[i],  width, height, VP8BORDERINPIXELS) < 0)
        {
            vp8_de_alloc_frame_buffers(oci);
            return ALLOC_FAILURE;
        }
    }

    oci->new_fb_idx = 0;
    oci->lst_fb_idx = 1;
    oci->gld_fb_idx = 2;
    oci->alt_fb_idx = 3;

    oci->fb_idx_ref_cnt[0] = 1;
    oci->fb_idx_ref_cnt[1] = 1;
    oci->fb_idx_ref_cnt[2] = 1;
    oci->fb_idx_ref_cnt[3] = 1;

    oci->mb_rows = height >> 4;
    oci->mb_cols = width >> 4;
    oci->MBs = oci->mb_rows * oci->mb_cols;
    oci->mode_info_stride = oci->mb_cols + 1;
	oci->mip = (MODE_INFO*)MallocMem(oci->pUserData, oci->nCodecIdx, sizeof(MODE_INFO)*(oci->mb_cols + 1) * (oci->mb_rows + 1), CACHE_LINE);
	SetMem(oci->pUserData, oci->nCodecIdx, (VO_U8 *)oci->mip, 0, sizeof(MODE_INFO)*(oci->mb_cols + 1) * (oci->mb_rows + 1));

    if (!oci->mip)
    {
        vp8_de_alloc_frame_buffers(oci);
        return ALLOC_FAILURE;
    }

    oci->mi = oci->mip + oci->mode_info_stride + 1;
   
	oci->above_context = (ENTROPY_CONTEXT_PLANES*)MallocMem(oci->pUserData, oci->nCodecIdx, sizeof(ENTROPY_CONTEXT_PLANES) * oci->mb_cols, CACHE_LINE);
	SetMem(oci->pUserData, oci->nCodecIdx, (VO_U8 *)oci->above_context, 0, sizeof(ENTROPY_CONTEXT_PLANES) * oci->mb_cols);

    if (!oci->above_context)
    {
        vp8_de_alloc_frame_buffers(oci);
        return ALLOC_FAILURE;
    }

	SetMem(oci->pUserData, oci->nCodecIdx, (VO_U8 *)(oci->mi - oci->mb_cols - 2), 0, sizeof(MODE_INFO) * (oci->mb_cols + 1));

    for (i = 0; i < oci->mb_rows; i++)
    {
		SetMem(oci->pUserData, oci->nCodecIdx, (VO_U8 *)(&oci->mi[i*oci->mb_cols-1]), 0, sizeof(MODE_INFO));
    }

    return 0;
}

void vp8_extend_copy16x16(unsigned char* dst,unsigned long dst_stride,unsigned char* src)
{
	 int r;
    for (r = 0; r < 16; r++)
    {
        ((int *)dst)[0] = ((int *)src)[0] ;
        ((int *)dst)[1] = ((int *)src)[1] ;
        ((int *)dst)[2] = ((int *)src)[2] ;
        ((int *)dst)[3] = ((int *)src)[3] ;
        dst += dst_stride;
    }
}
void vp8_extend_copy8x8(unsigned char* dst,unsigned long dst_stride,unsigned char* src)
{
	 int r;
    for (r = 0; r < 8; r++)
    {
        ((int *)dst)[0] = ((int *)src)[0] ;
        ((int *)dst)[1] = ((int *)src)[1] ;
        dst += dst_stride;
    }
}

void vp8_yv12_extend_frame_borders(YV12_BUFFER_CONFIG *ybf)
{
    unsigned char *src_ptr1, *src_ptr2;
    unsigned char *dest_ptr1, *dest_ptr2;
	int i;

    unsigned int Border;
    int plane_stride;
    int plane_height;
    int plane_width;

    /***********/
    /* Y Plane */
    /***********/
    Border = ybf->border;
    plane_stride = ybf->y_stride;
    plane_height = ybf->y_height;
    plane_width = ybf->y_width;

    // copy the left and right most columns out
    src_ptr1 = ybf->y_buffer;
    src_ptr2 = src_ptr1 + plane_width - 1;
#ifdef VOARMV7
	vo_Extend_Broder_y_neon(src_ptr1,src_ptr2,plane_stride,plane_height);
#else
	dest_ptr1 = src_ptr1 - Border;
    dest_ptr2 = src_ptr2 + 1;
    for (i = 0; i < plane_height; i++)
    {
		SetMem(ybf->pUserData, ybf->nCodecIdx, (VO_U8 *)dest_ptr1, src_ptr1[0], Border);
		SetMem(ybf->pUserData, ybf->nCodecIdx, (VO_U8 *)dest_ptr2, src_ptr2[0], Border);

        src_ptr1  += plane_stride;
        src_ptr2  += plane_stride;
        dest_ptr1 += plane_stride;
        dest_ptr2 += plane_stride;
    }
#endif
    // Now copy the top and bottom source lines into each line of the respective borders
    src_ptr1 = ybf->y_buffer - Border;
    src_ptr2 = src_ptr1 + (plane_height * plane_stride) - plane_stride;
    dest_ptr1 = src_ptr1 - (Border * plane_stride);
    dest_ptr2 = src_ptr2 + plane_stride;

    for (i = 0; i < (int)Border; i++)
    {
		CopyMem(ybf->pUserData, ybf->nCodecIdx, dest_ptr1, src_ptr1, plane_stride);
		CopyMem(ybf->pUserData, ybf->nCodecIdx, dest_ptr2, src_ptr2, plane_stride);
        dest_ptr1 += plane_stride;
        dest_ptr2 += plane_stride;
    }

    /***********/
    /* U Plane */
    /***********/
    plane_stride = ybf->uv_stride;
    plane_height = ybf->uv_height;
    plane_width = ybf->uv_width;
    Border /= 2;

    // copy the left and right most columns out
    src_ptr1 = ybf->u_buffer;
    src_ptr2 = src_ptr1 + plane_width - 1;
    

#ifdef VOARMV7
	vo_Extend_Broder_uv_neon(src_ptr1,src_ptr2,plane_stride,plane_height);
#else
	dest_ptr1 = src_ptr1 - Border;
    dest_ptr2 = src_ptr2 + 1;
    for (i = 0; i < plane_height; i++)
    {
		SetMem(ybf->pUserData, ybf->nCodecIdx, (VO_U8 *)dest_ptr1, src_ptr1[0], Border);
		SetMem(ybf->pUserData, ybf->nCodecIdx, (VO_U8 *)dest_ptr2, src_ptr2[0], Border);
        src_ptr1  += plane_stride;
        src_ptr2  += plane_stride;
        dest_ptr1 += plane_stride;
        dest_ptr2 += plane_stride;
    }
#endif
    // Now copy the top and bottom source lines into each line of the respective borders
    src_ptr1 = ybf->u_buffer - Border;
    src_ptr2 = src_ptr1 + (plane_height * plane_stride) - plane_stride;
    dest_ptr1 = src_ptr1 - (Border * plane_stride);
    dest_ptr2 = src_ptr2 + plane_stride;

    for (i = 0; i < (int)(Border); i++)
    {
		CopyMem(ybf->pUserData, ybf->nCodecIdx, dest_ptr1, src_ptr1, plane_stride);
		CopyMem(ybf->pUserData, ybf->nCodecIdx, dest_ptr2, src_ptr2, plane_stride);
        dest_ptr1 += plane_stride;
        dest_ptr2 += plane_stride;
    }
    /***********/
    /* V Plane */
    /***********/

    // copy the left and right most columns out
    src_ptr1 = ybf->v_buffer;
    src_ptr2 = src_ptr1 + plane_width - 1;
#ifdef VOARMV7
	vo_Extend_Broder_uv_neon(src_ptr1,src_ptr2,plane_stride,plane_height);
#else
	dest_ptr1 = src_ptr1 - Border;
    dest_ptr2 = src_ptr2 + 1;
    for (i = 0; i < plane_height; i++)
    {
		SetMem(ybf->pUserData, ybf->nCodecIdx, (VO_U8 *)dest_ptr1, src_ptr1[0], Border);
		SetMem(ybf->pUserData, ybf->nCodecIdx, (VO_U8 *)dest_ptr2, src_ptr2[0], Border);
        src_ptr1  += plane_stride;
        src_ptr2  += plane_stride;
        dest_ptr1 += plane_stride;
        dest_ptr2 += plane_stride;
    }
#endif
    // Now copy the top and bottom source lines into each line of the respective borders
    src_ptr1 = ybf->v_buffer - Border;
    src_ptr2 = src_ptr1 + (plane_height * plane_stride) - plane_stride;
    dest_ptr1 = src_ptr1 - (Border * plane_stride);
    dest_ptr2 = src_ptr2 + plane_stride;

    for (i = 0; i < (int)(Border); i++)
    {
		CopyMem(ybf->pUserData, ybf->nCodecIdx, dest_ptr1, src_ptr1, plane_stride);
		CopyMem(ybf->pUserData, ybf->nCodecIdx, dest_ptr2, src_ptr2, plane_stride);
        dest_ptr1 += plane_stride;
        dest_ptr2 += plane_stride;
    }
}

void vp8_setup_version(VP8_COMMON *cm)
{
    switch (cm->version)
    {
    case 0:
        cm->no_lpf = 0;
        cm->simpler_lpf = 0;
        cm->use_bilinear_mc_filter = 0;
        cm->full_pixel = 0;
		cm->build_uvmvs = vp8_build_uvmvs;
        break;
    case 1:
        cm->no_lpf = 0;
        cm->simpler_lpf = 1;
        cm->use_bilinear_mc_filter = 1;
        cm->full_pixel = 0;
		cm->build_uvmvs = vp8_build_uvmvs;
        break;
    case 2:
        cm->no_lpf = 1;
        cm->simpler_lpf = 0;
        cm->use_bilinear_mc_filter = 1;
        cm->full_pixel = 0;
		cm->build_uvmvs = vp8_build_uvmvs;
        break;
    case 3:
        cm->no_lpf = 1;
        cm->simpler_lpf = 1;
        cm->use_bilinear_mc_filter = 1;
        cm->full_pixel = 1;
		cm->build_uvmvs = vp8_build_uvmvs_fullpixel;
        break;
    default:
        //4,5,6,7 are reserved for future use
        cm->no_lpf = 0;
        cm->simpler_lpf = 0;
        cm->use_bilinear_mc_filter = 0;
        cm->full_pixel = 0;
		cm->build_uvmvs = vp8_build_uvmvs;
        break;
    }
}
void vp8_create_common(VP8_COMMON *oci)
{
    vp8_default_bmode_probs(oci->fc.bmode_prob);

    oci->mb_no_coeff_skip = 1;
    oci->no_lpf = 0;
    oci->simpler_lpf = 0;
    oci->use_bilinear_mc_filter = 0;
    oci->full_pixel = 0;
    oci->multi_token_partition = ONE_PARTITION;
    oci->clr_type = REG_YUV;
    oci->clamp_type = RECON_CLAMP_REQUIRED;

    // Default disable buffer to buffer copying
    oci->copy_buffer_to_gf = 0;
    oci->copy_buffer_to_arf = 0;
	return;
}

void vp8_remove_common(VP8_COMMON *oci)
{
    vp8_de_alloc_frame_buffers(oci);
}

void vp8_initialize_common()
{
    vp8_coef_tree_initialize();
    vp8_entropy_mode_init();

}