#ifndef __BITFUNCTIONS_H__
#define __BITFUNCTIONS_H__
#include"AC3DataStruct.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#   define MIN_CACHE_BITS 25
#   define AV_RB32(x)                           \
    ((((const VO_U8*)(x))[0] << 24) |         \
     (((const VO_U8*)(x))[1] << 16) |         \
     (((const VO_U8*)(x))[2] <<  8) |         \
      ((const VO_U8*)(x))[3])

#   define NEG_USR32(a,s) (((VO_U32)(a))>>(32-(s)))

#   define SHOW_UBITS(name, gb, num)\
        NEG_USR32(name##_cache, num)

#   define OPEN_READER(name, gb)\
        unsigned int name##_index= (gb)->index;\
        int name##_cache= 0;\

#   define CLOSE_READER(name, gb)\
        (gb)->index= name##_index;

#   define UPDATE_CACHE(name, gb)\
        name##_cache= AV_RB32( ((const VO_U8 *)(gb)->buffer)+(name##_index>>3) ) << (name##_index&0x07);\

#   define SKIP_CACHE(name, gb, num)\
        name##_cache <<= (num);

#   define SKIP_COUNTER(name, gb, num)\
        name##_index += (num);

#   define LAST_SKIP_BITS(name, gb, num) SKIP_COUNTER(name, gb, num)

static VO_U32 vo_get_ac3_bits(GetBitContext *gb, int n){
	register int tmp;
	//OPEN_READER macro
    unsigned int index= gb->index;
    int cache= 0;
	//UPDATE_CACHE macro
	cache= AV_RB32( ((const VO_U8 *)(gb)->buffer)+(index>>3) ) << (index&0x07);
	//SHOW_UBITS
	tmp = NEG_USR32(cache,n);
	//LAST_SKIP_BITS
	index += n;
	//CLOSE_READER
	gb->index = index;
	return tmp;
}

static VO_U32 get_bits1(GetBitContext *s){

    VO_U32 index= s->index;
    VO_U8 result= s->buffer[ index>>3 ];
    result<<= (index&0x07);
    result>>= 8 - 1;
    index++;
    s->index= index;
    return result;
}

static VO_U32 vo_get_ac3_bits1(GetBitContext *s){
    VO_U32 index= s->index;
    VO_U8 result= s->buffer[ index>>3 ];
    result<<= (index&0x07);
    result>>= 8 - 1;
    index++;
    s->index= index;
    return result;
}

static VO_U32 show_bits(GetBitContext *s, int n){
    register int tmp;
    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)
    tmp= SHOW_UBITS(re, s, n);
    return tmp;
}
static VO_U32 vo_show_ac3_bits(GetBitContext *gb, int n){
	register int tmp;
	//OPEN_READER macro
    unsigned int index= gb->index;
    int cache= 0;
	//UPDATE_CACHE macro
	cache= AV_RB32( ((const VO_U8 *)(gb)->buffer)+(index>>3) ) << (index&0x07);
	//SHOW_UBITS
	tmp = NEG_USR32(cache,n);
    return tmp;
}

static VO_U32 vo_get_ac3_bits_long(GetBitContext *s, int n){
	if(n<=MIN_CACHE_BITS) return vo_get_ac3_bits(s, n);
    int ret= vo_get_ac3_bits(s, 16) << (n-16);
    return ret | vo_get_ac3_bits(s, n-16);
}

static VO_U32 vo_show_ac3_bits_long(GetBitContext *s, int n){
    if(n<=MIN_CACHE_BITS) return vo_show_ac3_bits(s, n);
    else{
        GetBitContext gb= *s;
        return vo_get_ac3_bits_long(&gb, n);
    }
}

static void vo_ac3_skip_bits(GetBitContext *gb, int n){
 //Note gcc seems to optimize this to s->index+=n for the ALT_READER :))
     unsigned int index= gb->index;
    int cache= 0;
	//UPDATE_CACHE macro
	cache= AV_RB32( ((const VO_U8 *)(gb)->buffer)+(index>>3) ) << (index&0x07);
	//LAST_SKIP_BITS
	index += n;
	//CLOSE_READER
	gb->index = index;
}

#ifdef _VONAMESPACE
}
#endif

#endif
