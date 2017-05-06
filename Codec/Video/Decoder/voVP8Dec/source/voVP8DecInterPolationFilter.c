#include "voVP8DecInterPolationFilter.h"

#define BLOCK_HEIGHT_WIDTH 4
#define VP8_FILTER_WEIGHT 128
#define VP8_FILTER_SHIFT  7

const int g_sub_pel_filters[64] =
{

    0,  0,  128,    0,   0,  0 ,0  ,0 ,         // note that 1/8 pel positions are just as per alpha -0.5 bicubic
    0, -6,  123,   12,  -1,  0 ,0  ,0 ,
    2, -11, 108,   36,  -8,  1 ,0  ,0 ,         // New 1/4 pel 6 tap filter
    0, -9,   93,   50,  -6,  0 ,0  ,0 ,
    3, -16,  77,   77, -16,  3 ,0  ,0 ,         // New 1/2 pel 6 tap filter
    0, -6,   50,   93,  -9,  0 ,0  ,0 ,
    1, -8,   36,  108, -11,  2 ,0  ,0 ,         // New 1/4 pel 6 tap filter
    0, -1,   12,  123,  -6,  0 ,0  ,0
};

#ifdef VOARMV7
#elif VOARMV6
static const int bilinear_filters[8][2] =
{
    { 128,   0 },    { 112,  16 },
    {  96,  32 },    {  80,  48 },
    {  64,  64 },    {  48,  80 },
    {  32,  96 },    {  16, 112 }
};
static const short sub_pel_filters[8][6] =
{

    { 0,  0,  128,    0,   0,  0 },         // note that 1/8 pel positions are just as per alpha -0.5 bicubic
    { 0, -6,  123,   12,  -1,  0 },
    { 2, -11, 108,   36,  -8,  1 },         // New 1/4 pel 6 tap filter
    { 0, -9,   93,   50,  -6,  0 },
    { 3, -16,  77,   77, -16,  3 },         // New 1/2 pel 6 tap filter
    { 0, -6,   50,   93,  -9,  0 },
    { 1, -8,   36,  108, -11,  2 },         // New 1/4 pel 6 tap filter
    { 0, -1,   12,  123,  -6,  0 },
};
void vp8_sixtap_predict_armv6
(
    unsigned char  *src_ptr,
    int  src_pixels_per_line,
    int  xoffset,
    int  yoffset,
    unsigned char *dst_ptr,
    int  dst_pitch
)
{
    const short  *HFilter;
    const short  *VFilter;
    //DECLARE_ALIGNED_ARRAY(4, short, FData, 12*4); /* Temp data bufffer used in filtering */
	short FData[12*4];


    HFilter = sub_pel_filters[xoffset];   /* 6 tap */
    VFilter = sub_pel_filters[yoffset];       /* 6 tap */

    /* Vfilter is null. First pass only */
    if (xoffset && !yoffset)
    {
        /*vp8_filter_block2d_first_pass_armv6 ( src_ptr, FData+2, src_pixels_per_line, 4, 4, HFilter );
        vp8_filter_block2d_second_pass_armv6 ( FData+2, dst_ptr, dst_pitch, 4, VFilter );*/

        vp8_filter_block2d_first_pass_only_armv6(src_ptr, dst_ptr, src_pixels_per_line, 4, dst_pitch, HFilter);
    }
    /* Hfilter is null. Second pass only */
    else if (!xoffset && yoffset)
    {
        vp8_filter_block2d_second_pass_only_armv6(src_ptr, dst_ptr, src_pixels_per_line, 4, dst_pitch, VFilter);
    }
    else
    {
        /* Vfilter is a 4 tap filter */
        if (yoffset & 0x1)
        {
            vp8_filter_block2d_first_pass_armv6(src_ptr - src_pixels_per_line, FData + 1, src_pixels_per_line, 4, 7, HFilter);
            vp8_filter4_block2d_second_pass_armv6(FData + 2, dst_ptr, dst_pitch, 4, VFilter);
        }
        /* Vfilter is 6 tap filter */
        else
        {
            vp8_filter_block2d_first_pass_armv6(src_ptr - (2 * src_pixels_per_line), FData, src_pixels_per_line, 4, 9, HFilter);
            vp8_filter_block2d_second_pass_armv6(FData + 2, dst_ptr, dst_pitch, 4, VFilter);
        }
    }
}

void vp8_sixtap_predict8x8_armv6
(
    unsigned char  *src_ptr,
    int  src_pixels_per_line,
    int  xoffset,
    int  yoffset,
    unsigned char *dst_ptr,
    int  dst_pitch
)
{
    const short  *HFilter;
    const short  *VFilter;
    //DECLARE_ALIGNED_ARRAY(4, short, FData, 16*8); /* Temp data bufffer used in filtering */
	short FData[16*8];

    HFilter = sub_pel_filters[xoffset];   /* 6 tap */
    VFilter = sub_pel_filters[yoffset];       /* 6 tap */

    if (xoffset && !yoffset)
    {
        vp8_filter_block2d_first_pass_only_armv6(src_ptr, dst_ptr, src_pixels_per_line, 8, dst_pitch, HFilter);
    }
    /* Hfilter is null. Second pass only */
    else if (!xoffset && yoffset)
    {
        vp8_filter_block2d_second_pass_only_armv6(src_ptr, dst_ptr, src_pixels_per_line, 8, dst_pitch, VFilter);
    }
    else
    {
        if (yoffset & 0x1)
        {
            vp8_filter_block2d_first_pass_armv6(src_ptr - src_pixels_per_line, FData + 1, src_pixels_per_line, 8, 11, HFilter);
            vp8_filter4_block2d_second_pass_armv6(FData + 2, dst_ptr, dst_pitch, 8, VFilter);
        }
        else
        {
            vp8_filter_block2d_first_pass_armv6(src_ptr - (2 * src_pixels_per_line), FData, src_pixels_per_line, 8, 13, HFilter);
            vp8_filter_block2d_second_pass_armv6(FData + 2, dst_ptr, dst_pitch, 8, VFilter);
        }
    }
}
void vp8_sixtap_predict16x16_armv6
(
    unsigned char  *src_ptr,
    int  src_pixels_per_line,
    int  xoffset,
    int  yoffset,
    unsigned char *dst_ptr,
    int  dst_pitch
)
{
    const short  *HFilter;
    const short  *VFilter;
    //DECLARE_ALIGNED_ARRAY(4, short, FData, 24*16);    /* Temp data bufffer used in filtering */
	short FData[16*24];

    HFilter = sub_pel_filters[xoffset];   /* 6 tap */
    VFilter = sub_pel_filters[yoffset];       /* 6 tap */

    if (xoffset && !yoffset)
    {
        vp8_filter_block2d_first_pass_only_armv6(src_ptr, dst_ptr, src_pixels_per_line, 16, dst_pitch, HFilter);
    }
    /* Hfilter is null. Second pass only */
    else if (!xoffset && yoffset)
    {
        vp8_filter_block2d_second_pass_only_armv6(src_ptr, dst_ptr, src_pixels_per_line, 16, dst_pitch, VFilter);
    }
    else
    {
        if (yoffset & 0x1)
        {
            vp8_filter_block2d_first_pass_armv6(src_ptr - src_pixels_per_line, FData + 1, src_pixels_per_line, 16, 19, HFilter);
            vp8_filter4_block2d_second_pass_armv6(FData + 2, dst_ptr, dst_pitch, 16, VFilter);
        }
        else
        {
            vp8_filter_block2d_first_pass_armv6(src_ptr - (2 * src_pixels_per_line), FData, src_pixels_per_line, 16, 21, HFilter);
            vp8_filter_block2d_second_pass_armv6(FData + 2, dst_ptr, dst_pitch, 16, VFilter);
        }
    }

}

void vp8_filter_block2d_bil_armv6
(
    unsigned char *src_ptr,
    unsigned char *output_ptr,
    unsigned int   src_pixels_per_line,
    unsigned int   dst_pitch,
    const short      *HFilter,
    const short      *VFilter,
    int            Width,
    int            Height
)
{

    unsigned short FData[36*16]; /* Temp data bufffer used in filtering */

    /* First filter 1-D horizontally... */
    /* pixel_step = 1; */
    vp8_filter_block2d_bil_first_pass_armv6(src_ptr, FData, src_pixels_per_line, Height + 1, Width, HFilter);

    /* then 1-D vertically... */
    vp8_filter_block2d_bil_second_pass_armv6(FData, output_ptr, dst_pitch, Height, Width, VFilter);
}


void vp8_bilinear_predict4x4_armv6
(
    unsigned char  *src_ptr,
    int   src_pixels_per_line,
    int  xoffset,
    int  yoffset,
    unsigned char *dst_ptr,
    int dst_pitch
)
{
    const short  *HFilter;
    const short  *VFilter;

    HFilter = bilinear_filters[xoffset];
    VFilter = bilinear_filters[yoffset];

    vp8_filter_block2d_bil_armv6(src_ptr, dst_ptr, src_pixels_per_line, dst_pitch, HFilter, VFilter, 4, 4);
}

void vp8_bilinear_predict8x8_armv6
(
    unsigned char  *src_ptr,
    int  src_pixels_per_line,
    int  xoffset,
    int  yoffset,
    unsigned char *dst_ptr,
    int  dst_pitch
)
{
    const short  *HFilter;
    const short  *VFilter;

    HFilter = bilinear_filters[xoffset];
    VFilter = bilinear_filters[yoffset];

    vp8_filter_block2d_bil_armv6(src_ptr, dst_ptr, src_pixels_per_line, dst_pitch, HFilter, VFilter, 8, 8);
}

void vp8_bilinear_predict8x4_armv6
(
    unsigned char  *src_ptr,
    int  src_pixels_per_line,
    int  xoffset,
    int  yoffset,
    unsigned char *dst_ptr,
    int  dst_pitch
)
{
    const short  *HFilter;
    const short  *VFilter;

    HFilter = bilinear_filters[xoffset];
    VFilter = bilinear_filters[yoffset];

    vp8_filter_block2d_bil_armv6(src_ptr, dst_ptr, src_pixels_per_line, dst_pitch, HFilter, VFilter, 8, 4);
}

void vp8_bilinear_predict16x16_armv6
(
    unsigned char  *src_ptr,
    int  src_pixels_per_line,
    int  xoffset,
    int  yoffset,
    unsigned char *dst_ptr,
    int  dst_pitch
)
{
    const short  *HFilter;
    const short  *VFilter;

    HFilter = bilinear_filters[xoffset];
    VFilter = bilinear_filters[yoffset];

    vp8_filter_block2d_bil_armv6(src_ptr, dst_ptr, src_pixels_per_line, dst_pitch, HFilter, VFilter, 16, 16);
}

#else
static const int bilinear_filters[8][2] =
{
    { 128,   0 },    { 112,  16 },
    {  96,  32 },    {  80,  48 },
    {  64,  64 },    {  48,  80 },
    {  32,  96 },    {  16, 112 }
};
static const short sub_pel_filters[8][6] =
{

    { 0,  0,  128,    0,   0,  0 },         // note that 1/8 pel positions are just as per alpha -0.5 bicubic
    { 0, -6,  123,   12,  -1,  0 },
    { 2, -11, 108,   36,  -8,  1 },         // New 1/4 pel 6 tap filter
    { 0, -9,   93,   50,  -6,  0 },
    { 3, -16,  77,   77, -16,  3 },         // New 1/2 pel 6 tap filter
    { 0, -6,   50,   93,  -9,  0 },
    { 1, -8,   36,  108, -11,  2 },         // New 1/4 pel 6 tap filter
    { 0, -1,   12,  123,  -6,  0 },
};
void vp8_filter_block2d_first_pass_only
(
    unsigned char *src_ptr,
    unsigned char  *output_ptr,
    unsigned int src_pixels_per_line,
    unsigned int pixel_step,
    unsigned int output_height,
    unsigned int output_width,
    const short *vp8_filter,
	int dst_pich
)
{
    unsigned int i, j;
    int  Temp;//,fg=1;
    for (i = 0; i < output_height; i++)
    {
        for (j = 0; j < output_width; j++)
        {
            Temp = ((int)src_ptr[-2 * (int)pixel_step] * vp8_filter[0]) +
                   ((int)src_ptr[-1 * (int)pixel_step] * vp8_filter[1]) +
                   ((int)src_ptr[0]                 * vp8_filter[2]) +
                   ((int)src_ptr[pixel_step]         * vp8_filter[3]) +
                   ((int)src_ptr[2*pixel_step]       * vp8_filter[4]) +
                   ((int)src_ptr[3*pixel_step]       * vp8_filter[5]) +
                   (VP8_FILTER_WEIGHT >> 1);      // Rounding         
            // Normalize back to 0-255
            Temp = Temp >> VP8_FILTER_SHIFT;    
            if (Temp < 0)
                Temp = 0;
            else if (Temp > 255)
                Temp = 255;
            output_ptr[j] = Temp;
            src_ptr++;
		}
        // Next row...
        src_ptr    += src_pixels_per_line - output_width;
        output_ptr += dst_pich;
    }
}
void vp8_filter_block2d_second_pass_only
(
    unsigned char *src_ptr,
    unsigned char *output_ptr,
    int output_pitch,
    unsigned int src_pixels_per_line,
    unsigned int pixel_step,
    unsigned int output_height,
    unsigned int output_width,
    const short *vp8_filter
)
{
    unsigned int i, j;
    int  Temp;
    for (i = 0; i < output_height; i++)
    {
        for (j = 0; j < output_width; j++)
        {
            // Apply filter
            Temp = ((int)src_ptr[-2 * (int)pixel_step] * vp8_filter[0]) +
                   ((int)src_ptr[-1 * (int)pixel_step] * vp8_filter[1]) +
                   ((int)src_ptr[0]                 * vp8_filter[2]) +
                   ((int)src_ptr[pixel_step]         * vp8_filter[3]) +
                   ((int)src_ptr[2*pixel_step]       * vp8_filter[4]) +
                   ((int)src_ptr[3*pixel_step]       * vp8_filter[5]) +
                   (VP8_FILTER_WEIGHT >> 1);   // Rounding 
            // Normalize back to 0-255
            Temp = Temp >> VP8_FILTER_SHIFT;
            if (Temp < 0)
                Temp = 0;
            else if (Temp > 255)
                Temp = 255; 
            output_ptr[j] = (unsigned char)Temp;
            src_ptr++;
        }
        // Start next row
        src_ptr    += src_pixels_per_line - output_width;
        output_ptr += output_pitch;
    }
}
void vp8_filter_block2d_first_pass
(
    unsigned char *src_ptr,
    int *output_ptr,
    unsigned int src_pixels_per_line,
    unsigned int pixel_step,
    unsigned int output_height,
    unsigned int output_width,
    const short *vp8_filter
)
{
    unsigned int i, j;
    int  Temp;
	//int fg=1;
	//static int munb=0,munb2=0;;

    for (i = 0; i < output_height; i++)
    {
        for (j = 0; j < output_width; j++)
        {
            Temp = ((int)src_ptr[-2 /** (int)pixel_step*/] * vp8_filter[0]) +
                   ((int)src_ptr[-1 /** (int)pixel_step*/] * vp8_filter[1]) +
                   ((int)src_ptr[0]                 * vp8_filter[2]) +
                   ((int)src_ptr[1/*pixel_step*/]         * vp8_filter[3]) +
                   ((int)src_ptr[2/**pixel_step*/]       * vp8_filter[4]) +
                   ((int)src_ptr[3/**pixel_step*/]       * vp8_filter[5]) +
                   (VP8_FILTER_WEIGHT >> 1);      // Rounding

            // Normalize back to 0-255
            Temp = Temp >> VP8_FILTER_SHIFT;

            if (Temp < 0)
                Temp = 0;
            else if (Temp > 255)
                Temp = 255;

            output_ptr[j] = Temp;
            src_ptr++;
        }
        src_ptr    += src_pixels_per_line - output_width;
        output_ptr += output_width;
    }
}
void vp8_filter_block2d_second_pass
(
    int *src_ptr,
    unsigned char *output_ptr,
    int output_pitch,
    unsigned int src_pixels_per_line,
    unsigned int pixel_step,
    unsigned int output_height,
    unsigned int output_width,
    const short *vp8_filter
)
{
    unsigned int i, j;
    int  Temp;//,fg=1;

    for (i = 0; i < output_height; i++)
    {
        for (j = 0; j < output_width; j++)
        {
            // Apply filter
            Temp = ((int)src_ptr[-2 * (int)pixel_step] * vp8_filter[0]) +
                   ((int)src_ptr[-1 * (int)pixel_step] * vp8_filter[1]) +
                   ((int)src_ptr[0]                 * vp8_filter[2]) +
                   ((int)src_ptr[pixel_step]         * vp8_filter[3]) +
                   ((int)src_ptr[2*pixel_step]       * vp8_filter[4]) +
                   ((int)src_ptr[3*pixel_step]       * vp8_filter[5]) +
                   (VP8_FILTER_WEIGHT >> 1);   // Rounding

            // Normalize back to 0-255
            Temp = Temp >> VP8_FILTER_SHIFT;

            if (Temp < 0)
                Temp = 0;
            else if (Temp > 255)
                Temp = 255;

            output_ptr[j] = (unsigned char)Temp;
            src_ptr++;
        }
        src_ptr    += src_pixels_per_line - output_width;
        output_ptr += output_pitch;
    }
}

void vp8_sixtap_predict16x16_c(unsigned char *src_ptr, int src_pixels_per_line, int xoffset, int yoffset, unsigned char *dst_ptr, int dst_pitch)
{
    const short  *HFilter;
    const short  *VFilter;
    int FData[21*24];   // Temp data bufffer used in filtering

    HFilter = sub_pel_filters[xoffset];   // 6 tap
    VFilter = sub_pel_filters[yoffset];   // 6 tap
	if(xoffset==0)
	{
		vp8_filter_block2d_second_pass_only(src_ptr, dst_ptr, dst_pitch, src_pixels_per_line, src_pixels_per_line, 16, 16, VFilter);
	}
	else if(yoffset==0)
	{
		vp8_filter_block2d_first_pass_only(src_ptr, dst_ptr, src_pixels_per_line, 1, 16, 16, HFilter,dst_pitch);
	}
	else
	{        
		vp8_filter_block2d_first_pass(src_ptr - (2 * src_pixels_per_line), FData, src_pixels_per_line, 1, 21, 16, HFilter);
		vp8_filter_block2d_second_pass(FData + 32, dst_ptr, dst_pitch, 16, 16, 16, 16, VFilter);
	}
}

void vp8_sixtap_predict8x8_c(unsigned char *src_ptr, int src_pixels_per_line, int xoffset, int yoffset, unsigned char *dst_ptr, int dst_pitch)
{
    const short  *HFilter;
    const short  *VFilter;
    int FData[13*16];   // Temp data bufffer used in filtering
	//static long i1=0,i2=0,i3=0,i4=0,i5=0,i6=0;

    HFilter = sub_pel_filters[xoffset];   // 6 tap
    VFilter = sub_pel_filters[yoffset];   // 6 tap
	if(xoffset==0)
	{
		vp8_filter_block2d_second_pass_only(src_ptr, dst_ptr, dst_pitch, src_pixels_per_line, src_pixels_per_line, 8, 8, VFilter);
	}
	else if(yoffset==0)
	{
		vp8_filter_block2d_first_pass_only(src_ptr, dst_ptr, src_pixels_per_line, 1, 8, 8, HFilter,dst_pitch);
	}
	else
	{
		vp8_filter_block2d_first_pass(src_ptr - (2 * src_pixels_per_line), FData, src_pixels_per_line, 1, 13, 8, HFilter);
		vp8_filter_block2d_second_pass(FData + 16, dst_ptr, dst_pitch, 8, 8, 8, 8, VFilter);
	}
}

void vp8_sixtap_predict8x4_c(unsigned char *src_ptr, int src_pixels_per_line, int xoffset, int yoffset, unsigned char *dst_ptr, int dst_pitch)
{
    const short  *HFilter;
    const short  *VFilter;
    int FData[13*16];   // Temp data bufffer used in filtering

    HFilter = sub_pel_filters[xoffset];   // 6 tap
    VFilter = sub_pel_filters[yoffset];   // 6 tap

	if(xoffset==0)
	{
		vp8_filter_block2d_second_pass_only(src_ptr, dst_ptr, dst_pitch, src_pixels_per_line, src_pixels_per_line, 4, 8, VFilter);
	}
	else if(yoffset==0)
	{
		vp8_filter_block2d_first_pass_only(src_ptr, dst_ptr, src_pixels_per_line, 1, 4, 8, HFilter,dst_pitch);
	}
	else
	{
		vp8_filter_block2d_first_pass(src_ptr - (2 * src_pixels_per_line), FData, src_pixels_per_line, 1, 9, 8, HFilter);
		vp8_filter_block2d_second_pass(FData + 16, dst_ptr, dst_pitch, 8, 8, 4, 8, VFilter);
	}
}
void vp8_sixtap_predict_c(unsigned char *src_ptr, int src_pixels_per_line, int xoffset, int yoffset, unsigned char *dst_ptr, int dst_pitch)
{
	int FData[9*4]; // Temp data bufffer used in filtering
    const short  *HFilter;
    const short  *VFilter;

    HFilter = sub_pel_filters[xoffset];   // 6 tap
    VFilter = sub_pel_filters[yoffset];   // 6 tap

	if(xoffset==0)
	{
		vp8_filter_block2d_second_pass_only(src_ptr, dst_ptr, dst_pitch, src_pixels_per_line, src_pixels_per_line, 4, 4, VFilter);
	}
	else if(yoffset==0)
	{
		vp8_filter_block2d_first_pass_only(src_ptr, dst_ptr, src_pixels_per_line, 1, 4, 4, HFilter,dst_pitch);
	}
	else
	{
		vp8_filter_block2d_first_pass(src_ptr - (2 * src_pixels_per_line), FData, src_pixels_per_line, 1, 9, 4, HFilter);
		vp8_filter_block2d_second_pass(FData + 8, dst_ptr, dst_pitch, 4, 4, 4, 4, VFilter);
	}
}

/*******************/
void vp8_filter_block2d_bil_first_pass
(
    unsigned char *src_ptr,
    unsigned short *output_ptr,
    unsigned int src_pixels_per_line,
    int pixel_step,
    unsigned int output_height,
    unsigned int output_width,
    const int *vp8_filter
)
{
    unsigned int i, j;

    for (i = 0; i < output_height; i++)
    {
        for (j = 0; j < output_width; j++)
        {
            // Apply bilinear filter
            output_ptr[j] = (((int)src_ptr[0]          * vp8_filter[0]) +
                             ((int)src_ptr[pixel_step] * vp8_filter[1]) +
                             (VP8_FILTER_WEIGHT / 2)) >> VP8_FILTER_SHIFT;
            src_ptr++;
        }

        // Next row...
        src_ptr    += src_pixels_per_line - output_width;
        output_ptr += output_width;
    }
}

/****************************************************************************
 *
 *  ROUTINE       : filter_block2d_bil_second_pass
 *
 *  INPUTS        : INT32  *src_ptr          : Pointer to source block.
 *                  UINT32 src_pixels_per_line : Stride of input block.
 *                  UINT32 pixel_step        : Offset between filter input samples (see notes).
 *                  UINT32 output_height     : Input block height.
 *                  UINT32 output_width      : Input block width.
 *                  INT32  *vp8_filter          : Array of 2 bi-linear filter taps.
 *
 *  OUTPUTS       : UINT16 *output_ptr       : Pointer to filtered block.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Applies a 1-D 2-tap bi-linear filter to the source block in
 *                  either horizontal or vertical direction to produce the
 *                  filtered output block. Used to implement second-pass
 *                  of 2-D separable filter.
 *
 *  SPECIAL NOTES : Requires 32-bit input as produced by filter_block2d_bil_first_pass.
 *                  Two filter taps should sum to VP8_FILTER_WEIGHT.
 *                  pixel_step defines whether the filter is applied
 *                  horizontally (pixel_step=1) or vertically (pixel_step=stride).
 *                  It defines the offset required to move from one input
 *                  to the next.
 *
 ****************************************************************************/
void vp8_filter_block2d_bil_second_pass
(
    unsigned short *src_ptr,
    unsigned char  *output_ptr,
    int output_pitch,
    unsigned int  src_pixels_per_line,
    unsigned int  pixel_step,
    unsigned int  output_height,
    unsigned int  output_width,
    const int *vp8_filter
)
{
    unsigned int  i, j;
    int  Temp;

    for (i = 0; i < output_height; i++)
    {
        for (j = 0; j < output_width; j++)
        {
            // Apply filter
            Temp = ((int)src_ptr[0]         * vp8_filter[0]) +
                   ((int)src_ptr[pixel_step] * vp8_filter[1]) +
                   (VP8_FILTER_WEIGHT / 2);
            output_ptr[j] = (unsigned int)(Temp >> VP8_FILTER_SHIFT);
            src_ptr++;
        }

        // Next row...
        src_ptr    += src_pixels_per_line - output_width;
        output_ptr += output_pitch;
    }
}


/****************************************************************************
 *
 *  ROUTINE       : filter_block2d_bil
 *
 *  INPUTS        : UINT8  *src_ptr          : Pointer to source block.
 *                  UINT32 src_pixels_per_line : Stride of input block.
 *                  INT32  *HFilter         : Array of 2 horizontal filter taps.
 *                  INT32  *VFilter         : Array of 2 vertical filter taps.
 *
 *  OUTPUTS       : UINT16 *output_ptr       : Pointer to filtered block.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : 2-D filters an input block by applying a 2-tap
 *                  bi-linear filter horizontally followed by a 2-tap
 *                  bi-linear filter vertically on the result.
 *
 *  SPECIAL NOTES : The largest block size can be handled here is 16x16
 *
 ****************************************************************************/
void vp8_filter_block2d_bil
(
    unsigned char *src_ptr,
    unsigned char *output_ptr,
    unsigned int   src_pixels_per_line,
    unsigned int   dst_pitch,
    const int      *HFilter,
    const int      *VFilter,
    int            Width,
    int            Height
)
{

    unsigned short FData[17*16];    // Temp data bufffer used in filtering

    // First filter 1-D horizontally...
    vp8_filter_block2d_bil_first_pass(src_ptr, FData, src_pixels_per_line, 1, Height + 1, Width, HFilter);

    // then 1-D vertically...
    vp8_filter_block2d_bil_second_pass(FData, output_ptr, dst_pitch, Width, Width, Height, Width, VFilter);
}


void vp8_bilinear_predict4x4_c(unsigned char *src_ptr, int src_pixels_per_line, int xoffset, int yoffset, unsigned char *dst_ptr, int dst_pitch)
{
    const int  *HFilter;
    const int  *VFilter;

    HFilter = bilinear_filters[xoffset];
    VFilter = bilinear_filters[yoffset];
    vp8_filter_block2d_bil(src_ptr, dst_ptr, src_pixels_per_line, dst_pitch, HFilter, VFilter, 4, 4);
}

void vp8_bilinear_predict8x8_c(unsigned char *src_ptr, int src_pixels_per_line, int xoffset, int yoffset, unsigned char *dst_ptr, int dst_pitch)
{
    const int  *HFilter;
    const int  *VFilter;

    HFilter = bilinear_filters[xoffset];
    VFilter = bilinear_filters[yoffset];

    vp8_filter_block2d_bil(src_ptr, dst_ptr, src_pixels_per_line, dst_pitch, HFilter, VFilter, 8, 8);
}

void vp8_bilinear_predict8x4_c(unsigned char *src_ptr, int src_pixels_per_line, int xoffset, int yoffset, unsigned char *dst_ptr, int dst_pitch)
{
    const int  *HFilter;
    const int  *VFilter;

    HFilter = bilinear_filters[xoffset];
    VFilter = bilinear_filters[yoffset];

    vp8_filter_block2d_bil(src_ptr, dst_ptr, src_pixels_per_line, dst_pitch, HFilter, VFilter, 8, 4);
}

void vp8_bilinear_predict16x16_c(unsigned char *src_ptr, int src_pixels_per_line, int xoffset, int yoffset, unsigned char *dst_ptr, int dst_pitch)
{
    const int  *HFilter;
    const int  *VFilter;

    HFilter = bilinear_filters[xoffset];
    VFilter = bilinear_filters[yoffset];

    vp8_filter_block2d_bil(src_ptr, dst_ptr, src_pixels_per_line, dst_pitch, HFilter, VFilter, 16, 16);
}
#endif