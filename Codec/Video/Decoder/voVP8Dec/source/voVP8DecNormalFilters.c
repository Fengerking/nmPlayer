#include "voVP8LoopFilter.h"

// should we apply any filter at all ( 11111111 yes, 00000000 no)
__inline signed char vp8_filter_mask( unsigned char limit,  unsigned char flimit, 
                                     uc p3, uc p2, uc p1, uc p0, uc q0, uc q1, uc q2, uc q3) 
{
    signed char mask = 0;
    mask |= (ABS(p3 - p2) > limit) * -1;
	//if(mask<0)
		//goto END;
    mask |= (ABS(p2 - p1) > limit) * -1;
    mask |= (ABS(p1 - p0) > limit) * -1;
    mask |= (ABS(q1 - q0) > limit) * -1;
    mask |= (ABS(q2 - q1) > limit) * -1;
    mask |= (ABS(q3 - q2) > limit) * -1;
    mask |= (ABS(p0 - q0) * 2 + ABS(p1 - q1) / 2  > flimit/* * 2 + limit*/)* - 1;
    mask = ~mask;

    return mask;
}

// is there high variance internal edge ( 11111111 yes, 00000000 no)
__inline signed char vp8_hevmask(signed char thresh, uc p1, uc p0, uc q0, uc q1)
{
    signed char hev = 0;
    hev  |= (ABS(p1 - p0) > thresh) * -1;
    hev  |= (ABS(q1 - q0) > thresh) * -1;
    return hev;
}

__inline void vp8_filter(signed char mask, signed char hev, uc *op1, uc *op0, uc *oq0, uc *oq1)

{
    signed char vp8_filter, Filter1, Filter2;

    // add outer taps if we have high edge variance
    vp8_filter = vp8_signed_char_clamp(*op1 - *oq1);
    vp8_filter &= hev;

    // inner taps
    vp8_filter = vp8_signed_char_clamp(vp8_filter + 3 * (*oq0 - *op0));
    vp8_filter &= mask;

    // save bottom 3 bits so that we round one side +4 and the other +3
    // if it equals 4 we'll set to adjust by -1 to account for the fact,
    // we'd round 3 the other way
    Filter1 = vp8_signed_char_clamp(vp8_filter + 4);
    Filter2 = vp8_signed_char_clamp(vp8_filter + 3);
    Filter1 >>= 3;
    Filter2 >>= 3;

    *oq0 = vp8_unsigned_char_clamp(*oq0-Filter1);
    *op0 = vp8_unsigned_char_clamp(*op0+Filter2);
    vp8_filter = Filter1;

    // outer tap adjustments
    vp8_filter += 1;
    vp8_filter >>= 1;
    vp8_filter &= ~hev;

    *oq1 = vp8_unsigned_char_clamp(*oq1- vp8_filter);
    *op1 = vp8_unsigned_char_clamp(*op1+ vp8_filter);
}

void vp8_loop_filter_horizontal_edge_c
(
    unsigned char *s,
    int p, //pitch
    const  unsigned char *flimit,
    const  unsigned char *limit,
    const  unsigned char *thresh,
    int count
)
{
    int  hev = 0; // high edge variance
    signed char mask = 0;
    int i = 0;

    // loop filter designed to work using chars so that we can make maximum use
    // of 8 bit simd instructions.
    do
    {
#ifdef NEW_LOOP
		if(s[-2*p]==s[1*p]&&s[-1*p]==s[0])
			;
		else
#endif
		{
			mask = vp8_filter_mask(limit[i], flimit[i],
								   s[-4*p], s[-3*p], s[-2*p], s[-1*p],
								   s[0*p], s[1*p], s[2*p], s[3*p]);
#ifdef NEW_LOOP
			if(mask==0)
				;
			else
#endif
			{
				hev = vp8_hevmask(thresh[i], s[-2*p], s[-1*p], s[0*p], s[1*p]);
				vp8_filter(mask, hev, s - 2 * p, s - 1 * p, s, s + 1 * p);
			}
		}

        ++s;
    }
    while (++i < count * 8);
}

void vp8_loop_filter_vertical_edge_c
(
    unsigned char *s,
    int p,
    const unsigned char *flimit,
    const unsigned char *limit,
    const unsigned char *thresh,
    int count
)
{
    int  hev = 0; // high edge variance
    signed char mask = 0;
    int i = 0;

    // loop filter designed to work using chars so that we can make maximum use
    // of 8 bit simd instructions.
    do
    {
#ifdef NEW_LOOP
		if(s[-2]==s[1]&&s[-1]==s[0])
			;
		else
#endif
		{
			mask = vp8_filter_mask(limit[i], flimit[i],
								   s[-4], s[-3], s[-2], s[-1], s[0], s[1], s[2], s[3]);
#ifdef NEW_LOOP
			if(mask==0)
				;
			else
#endif
			{
				hev = vp8_hevmask(thresh[i], s[-2], s[-1], s[0], s[1]);
				vp8_filter(mask, hev, s - 2, s - 1, s, s + 1);
			}
		}

        s += p;
    }
    while (++i < count * 8);

	return;
}

__inline void vp8_mbfilter(signed char mask, signed char hev,
                           uc *op2, uc *op1, uc *op0, uc *oq0, uc *oq1, uc *oq2)
{
    signed char u;
    signed char vp8_filter, Filter1, Filter2;
	unsigned char q0,p0;
    // add outer taps if we have high edge variance
    vp8_filter = vp8_signed_char_clamp(*op1 - *oq1);
    vp8_filter = vp8_signed_char_clamp(vp8_filter + 3 * (*oq0 - *op0));
    vp8_filter &= mask;

    Filter2 = vp8_filter;
    Filter2 &= hev;

    // save bottom 3 bits so that we round one side +4 and the other +3
    Filter1 = vp8_signed_char_clamp(Filter2 + 4);
    Filter2 = vp8_signed_char_clamp(Filter2 + 3);
    Filter1 >>= 3;
    Filter2 >>= 3;
    q0 = vp8_unsigned_char_clamp(*oq0 - Filter1) ;
    p0 = vp8_unsigned_char_clamp(*op0 + Filter2);

    // only apply wider filter if not high edge variance
    vp8_filter &= ~hev;
    Filter2 = vp8_filter;

    // roughly 3/7th difference across boundary
    u = vp8_signed_char_clamp((63 + Filter2 * 27) >> 7);
    *oq0 = vp8_unsigned_char_clamp(q0 - u);
    *op0 = vp8_unsigned_char_clamp(p0 + u);

    // roughly 2/7th difference across boundary
    u = vp8_signed_char_clamp((63 + Filter2 * 18) >> 7);
    *oq1 = vp8_unsigned_char_clamp(*oq1-u);
    *op1 = vp8_unsigned_char_clamp(*op1+u);

    // roughly 1/7th difference across boundary
    u = vp8_signed_char_clamp((63 + Filter2 * 9) >> 7);
    *oq2 = vp8_unsigned_char_clamp(*oq2-u);
    *op2 = vp8_unsigned_char_clamp(*op2+u);
}
void vp8_mbloop_filter_horizontal_edge_c
(
    unsigned char *s,
    int p,
    const unsigned char *flimit,
    const unsigned char *limit,
    const unsigned char *thresh,
    int count
)
{
    signed char hev = 0; // high edge variance
    signed char mask = 0;
    int i = 0;
    // loop filter designed to work using chars so that we can make maximum use
    // of 8 bit simd instructions.
    do
    {
#ifndef NEW_LOOP
		if(s[-2*p]==s[1*p]&&s[-1*p]==s[0])
			;
		else
#endif
		{
			mask = vp8_filter_mask(limit[i], flimit[i],
								   s[-4*p], s[-3*p], s[-2*p], s[-1*p],
								   s[0*p], s[1*p], s[2*p], s[3*p]);
#ifndef NEW_LOOP
			if(mask==0)
				;
			else
#endif
			{
				hev = vp8_hevmask(thresh[i], s[-2*p], s[-1*p], s[0*p], s[1*p]);
				vp8_mbfilter(mask, hev, s - 3 * p, s - 2 * p, s - 1 * p, s, s + 1 * p, s + 2 * p);
			}
		}

        ++s;
    }
    while (++i < count * 8);
}
void vp8_mbloop_filter_vertical_edge_c
(
    unsigned char *s,
    int p,
    const  unsigned char *flimit,
    const  unsigned char *limit,
    const  unsigned char *thresh,
    int count
)
{
    signed char hev = 0; // high edge variance
    signed char mask = 0;
    int i = 0;	
    do
    {
		if(s[-2]==s[1]&&s[-1]==s[0])
			;
		else
		{
			mask = vp8_filter_mask(limit[i], flimit[i],
                              s[-4], s[-3], s[-2], s[-1], s[0], s[1], s[2], s[3]);
			if(mask)
			{
				hev = vp8_hevmask(thresh[i], s[-2], s[-1], s[0], s[1]);
				vp8_mbfilter(mask, hev, s - 3, s - 2, s - 1, s, s + 1, s + 2);
			}
		}
        s += p;
    }
    while (++i < count * 8);
}