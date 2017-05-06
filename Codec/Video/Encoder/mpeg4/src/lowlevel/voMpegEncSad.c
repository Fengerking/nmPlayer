/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include "voMpegEncSad.h"
#include "../voUtilits.h"

#define DIFFABS(n)    VOGETABS(*(ptr_cur_mb+n) - *(ptr_ref_mb+n))

#define VOSADLINE8(sad)  \
     sad += DIFFABS(0) + DIFFABS(1) + DIFFABS(2) + DIFFABS(3)\
            + DIFFABS(4) + DIFFABS(5) + DIFFABS(6) + DIFFABS(7);\
    ptr_cur_mb += cur_stride;\
    ptr_ref_mb += ref_stride;

#define VOSADLINE16(sad)  \
      sad += DIFFABS(0) + DIFFABS(1) + DIFFABS(2) + DIFFABS(3)\
        + DIFFABS(4) + DIFFABS(5) + DIFFABS(6) + DIFFABS(7)\
        + DIFFABS(8) + DIFFABS(9) + DIFFABS(10) + DIFFABS(11)\
        + DIFFABS(12) + DIFFABS(13) + DIFFABS(14) + DIFFABS(15)

#define VOSUMLINE16(ptr_cur_mb) \
    *(ptr_cur_mb + 0) + *(ptr_cur_mb + 1) + *(ptr_cur_mb + 2) + *(ptr_cur_mb + 3)+\
    *(ptr_cur_mb + 4) + *(ptr_cur_mb + 5) + *(ptr_cur_mb + 6) + *(ptr_cur_mb + 7) + \
    *(ptr_cur_mb + 8) + *(ptr_cur_mb + 9) + *(ptr_cur_mb + 10) + *(ptr_cur_mb + 11) +\
    *(ptr_cur_mb + 12) + *(ptr_cur_mb + 13) + *(ptr_cur_mb + 14) + *(ptr_cur_mb + 15);

VO_U32 Sad16x16_C(const VO_U8 * const cur_mb, 
                            const VO_U8 * const ref_mb,
                            const VO_U32 cur_stride,
                            const VO_U32 ref_stride,
                            const VO_U32 best_sad)
{

    VO_U32 sad = 0;
    VO_U32 i;
    VO_U8 const *ptr_cur_mb = cur_mb;
    VO_U8 const *ptr_ref_mb = ref_mb;

    for (i = 0; i < 16; i++) {
        VOSADLINE16(sad);
        if(sad < best_sad)
        {
            ptr_cur_mb += cur_stride;
            ptr_ref_mb += ref_stride;
        }
        else
        {
            return sad;
        }
    }
    return sad;
}

VO_U32 Sad8x8_C(const VO_U8 * const cur_mb,
                        const VO_U8 * const ref_mb,
                        const VO_U32 cur_stride,
                        const VO_U32 ref_stride)
{
    VO_U32 sad = 0;
    VO_U8 const *ptr_cur_mb = cur_mb;
    VO_U8 const *ptr_ref_mb = ref_mb;

    VOSADLINE8(sad);
    VOSADLINE8(sad);
    VOSADLINE8(sad);
    VOSADLINE8(sad);
    VOSADLINE8(sad);
    VOSADLINE8(sad);
    VOSADLINE8(sad);
    VOSADLINE8(sad);
    return sad;
}

// average deviation from mean 
VO_U32 Dev16x16_C(const VO_U8 * const cur_mb, const VO_U32 stride)
{
    VO_U32 mean = 0, dev = 0, i = 0;
    VO_U8 const *ptr_cur_mb = cur_mb;
    VO_S32  ptr_ref_mb[16];

     for (i = 0; i < 16; i++) {
        mean += VOSUMLINE16(ptr_cur_mb);
        ptr_cur_mb += stride;    
    }

    mean >>= 8;
    for (i = 0; i < 16; i++)
        ptr_ref_mb[i] = (VO_S32)mean;

    ptr_cur_mb = cur_mb;
    for (i = 0; i < 16; i++){
        VOSADLINE16(dev);
        ptr_cur_mb += stride;
    }
    return dev;
}

VO_U32 Sad16x16FourMv_C( VO_U8 *  cur_mb,
                                     VO_U8 *  ref_mb,
                                    const VO_U32 cur_stride,
                                    const VO_U32 ref_stride,
                                    VO_U32 *ptr_sad)
{
    VO_U32 sad_sum=0;
	VO_U8 *ptr_cur_mb = cur_mb;
	VO_U8 *ptr_ref_mb  = ref_mb;

	*ptr_sad = Sad8(ptr_cur_mb, ptr_ref_mb, cur_stride, ref_stride);
	
	ptr_cur_mb +=8;
    ptr_ref_mb +=8;
	*(ptr_sad+1) = Sad8(ptr_cur_mb, ptr_ref_mb, cur_stride, ref_stride);

	ptr_cur_mb =cur_mb + 8*cur_stride; 
    ptr_ref_mb= ref_mb + 8*ref_stride;
	*(ptr_sad+2) = Sad8(ptr_cur_mb, ptr_ref_mb, cur_stride, ref_stride);

	ptr_cur_mb +=8;
    ptr_ref_mb +=8;
	*(ptr_sad+3) = Sad8(ptr_cur_mb, ptr_ref_mb, cur_stride, ref_stride);

    sad_sum = *(ptr_sad)+*(ptr_sad+1)+*(ptr_sad+2)+*(ptr_sad+3);

	return sad_sum;
}

VO_VOID HalfpelRefineSad16x16( VO_U8 *cur_mb, 
                                    VO_U8 *ref_mb, 
                                    const VO_S32 ref_stride, 
                                    const VO_S32 rounding, 
                                    VO_U32* tmp_sad8)
{
	VO_U32 i, j;
	VO_U8 HV_buf0[24];
	VO_U8 V_buf0[24];

	const VO_S32 k = 1 - rounding;

	for(i = 0; i < 17; i++){
		V_buf0[i] = (VO_U8)((ref_mb[i - ref_stride] + ref_mb[i] + k) >> 1);
		HV_buf0[i] = (VO_U8)((ref_mb[i - 1 - ref_stride] + ref_mb[i - ref_stride]  + ref_mb[i - 1]+ ref_mb[i] + 1 + k) >> 2);
	}

	for(i = j = 0; j < 16; j++ ){
		VO_U32 blk;
		
		VO_S32 h = ((ref_mb[-1] +  ref_mb[0] + k) >> 1);
		VO_S32 hv = ((ref_mb[-1] + ref_mb[0]  + ref_mb[ref_stride - 1]+ ref_mb[ref_stride] + 1 + k) >> 2); 

		for(i = 0; i < 16; i++){
			VO_S32 c, v;
			blk = ((j > 7 ) << 1) + (i > 7);
			c = *(cur_mb++);
			tmp_sad8[16 + blk] += VOGETABS(HV_buf0[i] - c);
			tmp_sad8[20 + blk] += VOGETABS(HV_buf0[i + 1] - c);
			tmp_sad8[blk ] += VOGETABS(V_buf0[i] - c);
			tmp_sad8[8  + blk] += VOGETABS(h - c);

			h = ((ref_mb[i] +  ref_mb[i + 1] + k) >> 1);
			tmp_sad8[12 + blk] += VOGETABS(h - c);
			tmp_sad8[28 + blk] += VOGETABS(hv - c);
			HV_buf0[i] = (VO_U8)hv;
			hv = ((ref_mb[i] + ref_mb[i + 1]  + ref_mb[i + ref_stride]+ ref_mb[i + ref_stride + 1] + 1 + k) >> 2); 
			tmp_sad8[24 + blk] += VOGETABS(hv - c);
			v = ((ref_mb[i] + ref_mb[i + ref_stride] + k) >> 1);
			tmp_sad8[4  + blk] += VOGETABS(v - c);
			V_buf0[i] = (VO_U8)v; 
		}
        HV_buf0[i] = (VO_U8)hv;
		ref_mb += ref_stride;
	}
}



