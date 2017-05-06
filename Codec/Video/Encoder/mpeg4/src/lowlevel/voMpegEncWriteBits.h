/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#ifndef _WRITEBITS_H_
#define _WRITEBITS_H_
#include "../voMpegProtect.h"
#include "voType.h"
#include "../voUtilits.h"

typedef struct
{
	VO_U32 cache;
    VO_U32 *tail;
	VO_U32 *head;
	VO_U32 cur_pos;	
	VO_U32 init_pos;
}
BitStream;

// init bs
static VO_VOID __inline InitBits(BitStream * const bs, VO_VOID *const bitstream)
{
    VO_U32 out_buf = (VO_U32)bitstream;
    bs->head = bs->tail = (VO_U32 *) ((out_buf>>2)<<2);
	bs->cache = 0;
	bs->cur_pos = bs->init_pos = ((VO_U32)out_buf&4)<<3;
}

//get bitstream bits position
static VO_U32 __inline GetBitsPos(const BitStream * const bs)
{
    VO_U32 position = ((VO_U32)(8*((VO_U32)bs->tail - (VO_U32)bs->head) 
                        + bs->cur_pos - bs->init_pos));
	return position;
}
// flush bits & return bytes length
static VO_U32 __inline FlushBits(BitStream * const bs)
{
    VO_U32 len = (VO_U32)((VO_U32)bs->tail - (VO_U32)bs->head);

    if (bs->init_pos)
        len += ((bs->cur_pos + 7)>>3) - (bs->init_pos>>3);
    else
        len += ((bs->cur_pos + 7)>>3);

     if (bs->cur_pos) {
        VO_U32 value = bs->cache;
        VOSWAPTOBIGENDIAN(value);
        *bs->tail = value;
	}

    return len;
}

static VO_VOID __inline PutBit(BitStream * const bs, const VO_U32 value)
{
	if (value)
		bs->cache |= (0x80000000 >> bs->cur_pos);

	bs->cur_pos++;
	if(bs->cur_pos == 32){
		VO_U32 value = bs->cache;	
        VOSWAPTOBIGENDIAN(value);
		*(bs->tail++) = value;
		bs->cache = 0;
		bs->cur_pos = 0;
	}
}

static VO_VOID __inline PutBits(BitStream * const bs, const VO_U32 value, const VO_U32 size)
{
	VO_S32 left = (VO_S32)(32 - bs->cur_pos - size);

	if (left > 0) {
		bs->cache |= (value << left);
		bs->cur_pos += size;
	} else {
		VO_U32 buf = bs->cache;
        if(left) {
            left = -left;	
            buf |= (value >> left);
            VOSWAPTOBIGENDIAN(buf);
            *(bs->tail++) = buf;
            bs->cache = value << (32 - left);
			bs->cur_pos = left;
        }
        else {
		    buf |= (value);
            VOSWAPTOBIGENDIAN(buf);
		    *(bs->tail++) = buf;
		    bs->cache = 0;
		    bs->cur_pos = 0;
        }
	}
}

// pad bitstream to the next byte boundary
static VO_VOID __inline PadBits(BitStream * const bs)
{
	VO_S32 size = 8 - (bs->cur_pos&7);
	if (size < 8)
         PutBits(bs, 0xff>>(9-size), size);
}

// alway pad: even if currently at the byte boundary
static VO_VOID __inline PadBitsAlways(BitStream * const bs, VO_U32 index)
{
	VO_S32 size = 8 - (bs->cur_pos&7);
    if(index)
        PutBits(bs, 0xff>>(9-size), size);
    else
        PutBits(bs, 0, size);
}

#endif 
// _WRITEBITS_H_ 