
/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include "voVP8DecReadBits.h"

INLINE int vp8_treed_read_c(
    vp8_reader *const r,        /* !!! must return a 0 or 1 !!! */
    vp8_tree t,
    const unsigned char *const p
)
{
    register vp8_tree_index i = 0;

    while ((i = t[ i + vp8_read(r, p[i>>1])]) > 0)
		;

    return -i;
}

int vp8_treed_read_num(
    vp8_reader *const r,        /* !!! must return a 0 or 1 !!! */
    vp8_tree t,
    const unsigned char *const p
)
{
    vp8_tree_index i = 0;
    int v = 0, b;

    do
    {
        b = vp8_read(r, p[i>>1]);
        v = (v << 1) + b;
    }
    while ((i = t[i+b]) > 0);

    return v;
}