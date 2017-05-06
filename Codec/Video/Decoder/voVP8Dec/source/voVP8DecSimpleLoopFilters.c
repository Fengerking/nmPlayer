#include "voVP8LoopFilter.h"

__inline signed char vp8_simple_filter_mask( unsigned char limit,  unsigned char flimit, uc p1, uc p0, uc q0, uc q1)
{
// Why does this cause problems for win32?
// error C2143: syntax error : missing ';' before 'type'
//  (void) limit;
    signed char mask = (ABS(p0 - q0) * 2 + ABS(p1 - q1) / 2  <= flimit /** 2 + limit*/) * -1;
    return mask;
}

__inline void vp8_simple_filter(signed char mask, uc *op1, uc *op0, uc *oq0, uc *oq1)
{
    signed char vp8_filter, Filter1, Filter2;
    signed char p1 = (signed char) * op1 ^ 0x80;
    signed char p0 = (signed char) * op0 ^ 0x80;
    signed char q0 = (signed char) * oq0 ^ 0x80;
    signed char q1 = (signed char) * oq1 ^ 0x80;
    signed char u;

    vp8_filter = vp8_signed_char_clamp(p1 - q1);
    vp8_filter = vp8_signed_char_clamp(vp8_filter + 3 * (q0 - p0));
    vp8_filter &= mask;

    // save bottom 3 bits so that we round one side +4 and the other +3
    Filter1 = vp8_signed_char_clamp(vp8_filter + 4);
    Filter1 >>= 3;
    u = vp8_signed_char_clamp(q0 - Filter1);
    *oq0  = u ^ 0x80;

    Filter2 = vp8_signed_char_clamp(vp8_filter + 3);
    Filter2 >>= 3;
    u = vp8_signed_char_clamp(p0 + Filter2);
    *op0 = u ^ 0x80;
}

void vp8_loop_filter_simple_horizontal_edge_c
(
    unsigned char *s,
    int p,
    const  unsigned char *flimit,
    const  unsigned char *limit,
    const  unsigned char *thresh,
    int count
)
{
    signed char mask = 0;
    int i = 0;
    (void) thresh;

    do
    {
        //mask = vp8_simple_filter_mask( limit[i], flimit[i],s[-1*p],s[0*p]);
        mask = vp8_simple_filter_mask(limit[i], flimit[i], s[-2*p], s[-1*p], s[0*p], s[1*p]);
        vp8_simple_filter(mask, s - 2 * p, s - 1 * p, s, s + 1 * p);
        ++s;
    }
    while (++i < count * 8);
}

void vp8_loop_filter_simple_vertical_edge_c
(
    unsigned char *s,
    int p,
    const  unsigned char *flimit,
    const  unsigned char *limit,
    const  unsigned char *thresh,
    int count
)
{
    signed char mask = 0;
    int i = 0;
    (void) thresh;

    do
    {
        //mask = vp8_simple_filter_mask( limit[i], flimit[i],s[-1],s[0]);
        mask = vp8_simple_filter_mask(limit[i], flimit[i], s[-2], s[-1], s[0], s[1]);
        vp8_simple_filter(mask, s - 2, s - 1, s, s + 1);
        s += p;
    }
    while (++i < count * 8);
}

/**************************/
void vp8_loop_filter_mbhs(unsigned char *y_ptr, unsigned char *u_ptr, unsigned char *v_ptr,
                            int y_stride, int uv_stride, loop_filter_info *lfi, int simpler_lpf)
{
    (void) u_ptr;
    (void) v_ptr;
    (void) uv_stride;
    (void) simpler_lpf;
#ifdef VOARMV7
	vp8_loop_filter_simple_horizontal_edge_neon(y_ptr, y_stride, lfi->mbflim, lfi->lim, lfi->thr, 2);
#elif VOARMV6
	vp8_loop_filter_simple_horizontal_edge_armv6(y_ptr, y_stride, lfi->mbflim, lfi->lim, lfi->thr, 2);
#else
    vp8_loop_filter_simple_horizontal_edge_c(y_ptr, y_stride, lfi->mbflim, lfi->lim, lfi->thr, 2);
#endif
}

void vp8_loop_filter_mbvs(unsigned char *y_ptr, unsigned char *u_ptr, unsigned char *v_ptr,
                            int y_stride, int uv_stride, loop_filter_info *lfi, int simpler_lpf)
{
    (void) u_ptr;
    (void) v_ptr;
    (void) uv_stride;
    (void) simpler_lpf;
#ifdef VOARMV7
	vp8_loop_filter_simple_vertical_edge_neon(y_ptr, y_stride, lfi->mbflim, lfi->lim, lfi->thr, 2);
#elif VOARMV6
	vp8_loop_filter_simple_vertical_edge_armv6(y_ptr, y_stride, lfi->mbflim, lfi->lim, lfi->thr, 2);
#else
    vp8_loop_filter_simple_vertical_edge_c(y_ptr, y_stride, lfi->mbflim, lfi->lim, lfi->thr, 2);
#endif
}

void vp8_loop_filter_bhs(unsigned char *y_ptr, unsigned char *u_ptr, unsigned char *v_ptr,
                           int y_stride, int uv_stride, loop_filter_info *lfi, int simpler_lpf)
{
    (void) u_ptr;
    (void) v_ptr;
    (void) uv_stride;
    (void) simpler_lpf;
#ifdef VOARMV7
	vp8_loop_filter_simple_horizontal_edge_neon(y_ptr + 4 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_simple_horizontal_edge_neon(y_ptr + 8 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_simple_horizontal_edge_neon(y_ptr + 12 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
#elif VOARMV6
	vp8_loop_filter_simple_horizontal_edge_armv6(y_ptr + 4 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_simple_horizontal_edge_armv6(y_ptr + 8 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_simple_horizontal_edge_armv6(y_ptr + 12 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
#else
    vp8_loop_filter_simple_horizontal_edge_c(y_ptr + 4 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_simple_horizontal_edge_c(y_ptr + 8 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_simple_horizontal_edge_c(y_ptr + 12 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
#endif
}

void vp8_loop_filter_bvs(unsigned char *y_ptr, unsigned char *u_ptr, unsigned char *v_ptr,
                           int y_stride, int uv_stride, loop_filter_info *lfi, int simpler_lpf)
{
    (void) u_ptr;
    (void) v_ptr;
    (void) uv_stride;
    (void) simpler_lpf;
#ifdef VOARMV7
	 vp8_loop_filter_simple_vertical_edge_neon(y_ptr + 4, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_simple_vertical_edge_neon(y_ptr + 8, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_simple_vertical_edge_neon(y_ptr + 12, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
#elif VOARMV6
	vp8_loop_filter_simple_vertical_edge_armv6(y_ptr + 4, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_simple_vertical_edge_armv6(y_ptr + 8, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_simple_vertical_edge_armv6(y_ptr + 12, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
#else
    vp8_loop_filter_simple_vertical_edge_c(y_ptr + 4, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_simple_vertical_edge_c(y_ptr + 8, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_simple_vertical_edge_c(y_ptr + 12, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
#endif
}