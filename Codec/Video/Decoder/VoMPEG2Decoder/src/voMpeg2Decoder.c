/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2007		            *
*								 	                                    *
************************************************************************/
#include "voMpeg2Decoder.h"
#include "voMpegReadbits.h"
#include "voMpegReadbits.h"
#include "voMpeg2MB.h"
#include "voMpegMC.h"
#include "voMpeg2Parser.h"

#include "voMpeg2DecGlobal.h"
#include "voMpegMem.h"
#include "voMpegBuf.h"
#include "idct_add.h"
#include "voMpegPort.h"

#include "irdetoCallbacks.h"

#if FF_DUMP
int ff_debug=0; 
FILE* fLog=NULL;
#endif//FF_DUMP

//int not_code;
static VO_VOID Picture( MpegDecode* dec )
{
	VO_U32 data;
	// 0 bits available here;
    //FLUSH_BITS(dec,10); // temporal ref
	dec->temporal_reference = GetBits(dec,10);
    UPDATE_CACHE(dec);
	// 30 bits available here;
	data = SHOW_BITS(dec, 27);
	dec->prediction_type = ((data>>24)&0x7); //GetBits(dec,3);
	// skip 16 bits;
    // i = ((data>>8)&0xffff); //FLUSH_BITS(dec,16); // non constant bit rate
    if (dec->prediction_type > I_VOP){
		// P or B types;
        dec->full_pixel[0] = ((data>>7)&0x1); //GetBits(dec,1);
		dec->mpeg_f_code[0][0] = \
			dec->mpeg_f_code[0][1] = ((data>>4)&0x7) - 1; //GetBits(dec, 3) - 1;
		if (dec->prediction_type == B_VOP){ 
			dec->full_pixel[1] = ((data>>3)&0x1);
			dec->mpeg_f_code[1][0] = \
				dec->mpeg_f_code[1][1] = ((data>>0)&0x7) - 1;
			FLUSH_BITS(dec,27);
		}
		else
			FLUSH_BITS(dec,23);
    }
	else 
		FLUSH_BITS(dec,19);
    dec->frame_state = FRAME_DECODING;
}


static INLINE VO_VOID clearblock(VO_S16 *psblock) 
{
	int *i=(int*)psblock,*ie=(int*)(psblock+64);
	do{
		i[3] = i[2] = i[1] = i[0] = 0;
		i[7] = i[6] = i[5] = i[4] = 0;
		i+=8;
	}
	while (i!=ie);
}


#if defined(IPP_EDIT)
static INLINE VO_VOID clearblock4x8(VO_S16 *psblock) 
{
	int *i=(int*)psblock,*ie=(int*)(psblock+64);
	do{
		i[1] = i[0] = 0;
		i[5] = i[4] = 0;
		i+=8;
	}
	while (i!=ie);
}
#endif


// max 9 bits
static INLINE int getDCsizeLum( MpegDecode* dec, VO_U32 *cache_a, VO_U32 *cache_b, VO_S32 *bitpos)
{
	int code;
	if (!GET_BITS_1_POS(*cache_a, *cache_b, *bitpos))
		return GET_BITS_1_POS(*cache_a, *cache_b, *bitpos) ? 2 : 1;

	if (!GET_BITS_1_POS(*cache_a, *cache_b, *bitpos))
		return GET_BITS_1_POS(*cache_a, *cache_b, *bitpos) ? 3 : 0;

	code = SHOW_BIT_NEG_POS(*cache_a, 7);
	code = dec->numLeadingZeros[code];
	FLUSH_BIT_POS(*cache_a, *cache_b, *bitpos, code);
	return code + 3;
}

// max 10 bits
static INLINE int getDCsizeChr( MpegDecode* dec, VO_U32 *cache_a, VO_U32 *cache_b, VO_S32 *bitpos )
{
	int code;
	code = SHOW_BIT_POS(*cache_a, 2);
	if (code < 3){
		// first two bits are not 0x11;
		FLUSH_BIT_POS(*cache_a, *cache_b, *bitpos, 2);
		return code;
	}
	// first two bits are 0x11;
	code = SHOW_BIT_NEG_POS(*cache_a, 10);
	code = dec->numLeadingZeros[code] + 3;
	FLUSH_BIT_POS(*cache_a, *cache_b, *bitpos, code);
	return code;
}

// max 11bits
static INLINE int getDCdiff(int dct_dc_size, MpegDecode* dec, VO_U32 *cache_a, VO_U32 *cache_b, VO_S32 *bitpos)
{
	int i, code = SHOW_BIT_POS(*cache_a, 32); //we need only dct_dc_size bits (but in the higher bits)
	FLUSH_BIT_POS(*cache_a, *cache_b, *bitpos, dct_dc_size);
	i = ((VO_U32)code >> (32-dct_dc_size));
	if (code >= 0)
		return i + (-1 << dct_dc_size) + 1;

	return i;
}



#define TABLE_1			0
#define TABLE_2			256
#define TABLE_3			256+112
#define TABLE_END		256+112+112

VOCONST VO_U16 mpeg1_vld[256+112+112] = 
{
0xffff, 0xffff, 0xffff, 0xffff, 0x607d, 0x607d, 0x607d, 0x607d, 
0x7105, 0x7105, 0x7483, 0x7483, 0x7009, 0x7009, 0x7403, 0x7403, 
0x6383, 0x6383, 0x6383, 0x6383, 0x6303, 0x6303, 0x6303, 0x6303, 
0x6085, 0x6085, 0x6085, 0x6085, 0x6283, 0x6283, 0x6283, 0x6283, 
0x8683, 0x800d, 0x8603, 0x8583, 0x8185, 0x8087, 0x800b, 0x8503, 
0x5007, 0x5007, 0x5007, 0x5007, 0x5007, 0x5007, 0x5007, 0x5007, 
0x5203, 0x5203, 0x5203, 0x5203, 0x5203, 0x5203, 0x5203, 0x5203, 
0x5183, 0x5183, 0x5183, 0x5183, 0x5183, 0x5183, 0x5183, 0x5183, 
0x4005, 0x4005, 0x4005, 0x4005, 0x4005, 0x4005, 0x4005, 0x4005, 
0x4005, 0x4005, 0x4005, 0x4005, 0x4005, 0x4005, 0x4005, 0x4005, 
0x4103, 0x4103, 0x4103, 0x4103, 0x4103, 0x4103, 0x4103, 0x4103, 
0x4103, 0x4103, 0x4103, 0x4103, 0x4103, 0x4103, 0x4103, 0x4103, 
0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 
0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 
0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 
0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 
0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 
0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 
0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 
0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 
0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 
0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 
0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 
0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 0x207f, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0xb505, 0xb485, 0xb287, 0xb189, 0xb10b, 0xb08f, 0xb08d, 0xb01f, 
0xb01d, 0xb01b, 0xb019, 0xbd03, 0xbc83, 0xbc03, 0xbb83, 0xbb03, 
0xa017, 0xa017, 0xa405, 0xa405, 0xa207, 0xa207, 0xa015, 0xa015, 
0xa109, 0xa109, 0xa385, 0xa385, 0xaa83, 0xaa83, 0xaa03, 0xaa03, 
0xa013, 0xa013, 0xa983, 0xa983, 0xa903, 0xa903, 0xa08b, 0xa08b, 
0xa187, 0xa187, 0xa011, 0xa011, 0xa305, 0xa305, 0xa883, 0xa883, 
0x8803, 0x8803, 0x8803, 0x8803, 0x8803, 0x8803, 0x8803, 0x8803, 
0x8285, 0x8285, 0x8285, 0x8285, 0x8285, 0x8285, 0x8285, 0x8285, 
0x800f, 0x800f, 0x800f, 0x800f, 0x800f, 0x800f, 0x800f, 0x800f, 
0x8107, 0x8107, 0x8107, 0x8107, 0x8107, 0x8107, 0x8107, 0x8107, 
0x8089, 0x8089, 0x8089, 0x8089, 0x8089, 0x8089, 0x8089, 0x8089, 
0x8783, 0x8783, 0x8783, 0x8783, 0x8783, 0x8783, 0x8783, 0x8783, 
0x8703, 0x8703, 0x8703, 0x8703, 0x8703, 0x8703, 0x8703, 0x8703, 
0x8205, 0x8205, 0x8205, 0x8205, 0x8205, 0x8205, 0x8205, 0x8205, 
0xe0a5, 0xe0a3, 0xe0a1, 0xe09f, 0xe307, 0xe805, 0xe785, 0xe705, 
0xe685, 0xe605, 0xe585, 0xef83, 0xef03, 0xee83, 0xee03, 0xed83, 
0xd051, 0xd051, 0xd04f, 0xd04f, 0xd04d, 0xd04d, 0xd04b, 0xd04b, 
0xd049, 0xd049, 0xd047, 0xd047, 0xd045, 0xd045, 0xd043, 0xd043, 
0xd041, 0xd041, 0xd09d, 0xd09d, 0xd09b, 0xd09b, 0xd099, 0xd099, 
0xd097, 0xd097, 0xd095, 0xd095, 0xd093, 0xd093, 0xd091, 0xd091, 
0xc03f, 0xc03f, 0xc03f, 0xc03f, 0xc03d, 0xc03d, 0xc03d, 0xc03d, 
0xc03b, 0xc03b, 0xc03b, 0xc03b, 0xc039, 0xc039, 0xc039, 0xc039, 
0xc037, 0xc037, 0xc037, 0xc037, 0xc035, 0xc035, 0xc035, 0xc035, 
0xc033, 0xc033, 0xc033, 0xc033, 0xc031, 0xc031, 0xc031, 0xc031, 
0xc02f, 0xc02f, 0xc02f, 0xc02f, 0xc02d, 0xc02d, 0xc02d, 0xc02d, 
0xc02b, 0xc02b, 0xc02b, 0xc02b, 0xc029, 0xc029, 0xc029, 0xc029, 
0xc027, 0xc027, 0xc027, 0xc027, 0xc025, 0xc025, 0xc025, 0xc025, 
0xc023, 0xc023, 0xc023, 0xc023, 0xc021, 0xc021, 0xc021, 0xc021
};

VOCONST VO_U16 mpeg2_vld[256+112+112] = {
0xffff, 0xffff, 0xffff, 0xffff, 0x607d, 0x607d, 0x607d, 0x607d, 
0x7383, 0x7383, 0x7403, 0x7403, 0x7303, 0x7303, 0x7105, 0x7105, 
0x600f, 0x600f, 0x600f, 0x600f, 0x600d, 0x600d, 0x600d, 0x600d, 
0x6203, 0x6203, 0x6203, 0x6203, 0x6283, 0x6283, 0x6283, 0x6283, 
0x808b, 0x8583, 0x8017, 0x8015, 0x8683, 0x8603, 0x8185, 0x8089, 
0x5103, 0x5103, 0x5103, 0x5103, 0x5103, 0x5103, 0x5103, 0x5103, 
0x5085, 0x5085, 0x5085, 0x5085, 0x5085, 0x5085, 0x5085, 0x5085, 
0x5183, 0x5183, 0x5183, 0x5183, 0x5183, 0x5183, 0x5183, 0x5183, 
0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 
0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 
0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 
0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 0x3083, 
0x407f, 0x407f, 0x407f, 0x407f, 0x407f, 0x407f, 0x407f, 0x407f, 
0x407f, 0x407f, 0x407f, 0x407f, 0x407f, 0x407f, 0x407f, 0x407f, 
0x4007, 0x4007, 0x4007, 0x4007, 0x4007, 0x4007, 0x4007, 0x4007, 
0x4007, 0x4007, 0x4007, 0x4007, 0x4007, 0x4007, 0x4007, 0x4007, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 0x2003, 
0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 
0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 
0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 
0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 0x3005, 
0x5009, 0x5009, 0x5009, 0x5009, 0x5009, 0x5009, 0x5009, 0x5009, 
0x500b, 0x500b, 0x500b, 0x500b, 0x500b, 0x500b, 0x500b, 0x500b, 
0x7483, 0x7483, 0x7087, 0x7087, 0x7503, 0x7503, 0x7011, 0x7011, 
0x7013, 0x7013, 0x8019, 0x801b, 0x8107, 0x8205, 0x801d, 0x801f, 
0xb505, 0xb485, 0xb287, 0xb189, 0xb10b, 0xb08f, 0xb08d, 0x0001, 
0x0001, 0x0001, 0x0001, 0xbd03, 0xbc83, 0xbc03, 0xbb83, 0xbb03, 
0x0001, 0x0001, 0xa405, 0xa405, 0xa207, 0xa207, 0x0001, 0x0001, 
0x0001, 0x0001, 0xa385, 0xa385, 0xaa83, 0xaa83, 0xaa03, 0xaa03, 
0x0001, 0x0001, 0xa983, 0xa983, 0xa903, 0xa903, 0x0001, 0x0001, 
0xa187, 0xa187, 0x0001, 0x0001, 0xa305, 0xa305, 0xa883, 0xa883, 
0x7285, 0x7285, 0x7285, 0x7285, 0x7285, 0x7285, 0x7285, 0x7285, 
0x7285, 0x7285, 0x7285, 0x7285, 0x7285, 0x7285, 0x7285, 0x7285, 
0x7703, 0x7703, 0x7703, 0x7703, 0x7703, 0x7703, 0x7703, 0x7703, 
0x7703, 0x7703, 0x7703, 0x7703, 0x7703, 0x7703, 0x7703, 0x7703, 
0x8109, 0x8109, 0x8109, 0x8109, 0x8109, 0x8109, 0x8109, 0x8109, 
0x8803, 0x8803, 0x8803, 0x8803, 0x8803, 0x8803, 0x8803, 0x8803, 
0x7783, 0x7783, 0x7783, 0x7783, 0x7783, 0x7783, 0x7783, 0x7783, 
0x7783, 0x7783, 0x7783, 0x7783, 0x7783, 0x7783, 0x7783, 0x7783, 
0xe0a5, 0xe0a3, 0xe0a1, 0xe09f, 0xe307, 0xe805, 0xe785, 0xe705, 
0xe685, 0xe605, 0xe585, 0xef83, 0xef03, 0xee83, 0xee03, 0xed83, 
0xd051, 0xd051, 0xd04f, 0xd04f, 0xd04d, 0xd04d, 0xd04b, 0xd04b, 
0xd049, 0xd049, 0xd047, 0xd047, 0xd045, 0xd045, 0xd043, 0xd043, 
0xd041, 0xd041, 0xd09d, 0xd09d, 0xd09b, 0xd09b, 0xd099, 0xd099, 
0xd097, 0xd097, 0xd095, 0xd095, 0xd093, 0xd093, 0xd091, 0xd091, 
0xc03f, 0xc03f, 0xc03f, 0xc03f, 0xc03d, 0xc03d, 0xc03d, 0xc03d, 
0xc03b, 0xc03b, 0xc03b, 0xc03b, 0xc039, 0xc039, 0xc039, 0xc039, 
0xc037, 0xc037, 0xc037, 0xc037, 0xc035, 0xc035, 0xc035, 0xc035, 
0xc033, 0xc033, 0xc033, 0xc033, 0xc031, 0xc031, 0xc031, 0xc031, 
0xc02f, 0xc02f, 0xc02f, 0xc02f, 0xc02d, 0xc02d, 0xc02d, 0xc02d, 
0xc02b, 0xc02b, 0xc02b, 0xc02b, 0xc029, 0xc029, 0xc029, 0xc029, 
0xc027, 0xc027, 0xc027, 0xc027, 0xc025, 0xc025, 0xc025, 0xc025, 
0xc023, 0xc023, 0xc023, 0xc023, 0xc021, 0xc021, 0xc021, 0xc021 
};

// size:4 run:5		((level<<1)+1):7
// size:4 escape    62
// size:4 end		63
// In oder to reduce the operation of Table 1, size is stored in Table 1.
// (size - 2) is stored in Table 2 & 3. Also in order to reduce inter frame
// complexity, (level<<1)+1 is stored in last 7 bits; 

#define vld_decode								\
	CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max bits 9 needed*/\
	code = SHOW_BIT_POS(cache_a,8);			\
	if (code >= (1<<2)){						\
		code = table[code];								\
		FLUSH_BIT_POS(cache_a, cache_b, bitpos, (code >> 12));		\
	}													\
	else {												\
		CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max bits 17 needed*/ \
		code = SHOW_BIT_POS(cache_a,16);			\
		if (code >= (1<<7))							\
			code = (code >> 3) - 16 + TABLE_2;		\
		else /* if (code >= 16)  but we don't care about invalid huffman codes */ \
			code = code - 16 + TABLE_3;				\
		code = table[code];							\
		FLUSH_BIT_POS(cache_a, cache_b, bitpos,2+(code >> 12)); \
	}

static const VO_U8 non_linear_qscale[32] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    8,10,12,14,16,18,20,22,
    24,28,32,36,40,44,48,52,
    56,64,72,80,88,96,104,112,
};

static INLINE int get_qscale( MpegDecode* dec )
{
    int qscale = GetBits(dec, 5);

	if( qscale<=0)
	{
		qscale = 1;
	}
	if(qscale>31)
	{
		qscale =31;
	}

	if(!IsMpeg1(dec)){
		if (GET_Q_SCALE_TYPE(dec->mpeg2_flag)) {
			return non_linear_qscale[qscale];
		} else {
			return qscale << 1;
		}
	}else
	{
		return qscale;
	}
}

VOCONST VO_U8 mcbp_p[32*2+128*2] = 
{
0x00,0x00,0x08,0x39,0x08,0x2b,0x08,0x29,0x07,0x22,0x07,0x21,0x06,0x3f,0x06,0x24,
0x05,0x3e,0x05,0x02,0x05,0x3d,0x05,0x01,0x05,0x38,0x05,0x34,0x05,0x2c,0x05,0x1c,
0x05,0x28,0x05,0x14,0x05,0x30,0x05,0x0c,0x04,0x20,0x04,0x20,0x04,0x10,0x04,0x10,
0x04,0x08,0x04,0x08,0x04,0x04,0x04,0x04,0x03,0x3c,0x03,0x3c,0x03,0x3c,0x03,0x3c,
0x00,0x00,0x00,0x00,0x09,0x27,0x09,0x1b,0x09,0x3b,0x09,0x37,0x09,0x2f,0x09,0x1f,
0x08,0x3a,0x08,0x3a,0x08,0x36,0x08,0x36,0x08,0x2e,0x08,0x2e,0x08,0x1e,0x08,0x1e,
0x08,0x39,0x08,0x39,0x08,0x35,0x08,0x35,0x08,0x2d,0x08,0x2d,0x08,0x1d,0x08,0x1d,
0x08,0x26,0x08,0x26,0x08,0x1a,0x08,0x1a,0x08,0x25,0x08,0x25,0x08,0x19,0x08,0x19,
0x08,0x2b,0x08,0x2b,0x08,0x17,0x08,0x17,0x08,0x33,0x08,0x33,0x08,0x0f,0x08,0x0f,
0x08,0x2a,0x08,0x2a,0x08,0x16,0x08,0x16,0x08,0x32,0x08,0x32,0x08,0x0e,0x08,0x0e,
0x08,0x29,0x08,0x29,0x08,0x15,0x08,0x15,0x08,0x31,0x08,0x31,0x08,0x0d,0x08,0x0d,
0x08,0x23,0x08,0x23,0x08,0x13,0x08,0x13,0x08,0x0b,0x08,0x0b,0x08,0x07,0x08,0x07,
0x07,0x22,0x07,0x22,0x07,0x22,0x07,0x22,0x07,0x12,0x07,0x12,0x07,0x12,0x07,0x12,
0x07,0x0a,0x07,0x0a,0x07,0x0a,0x07,0x0a,0x07,0x06,0x07,0x06,0x07,0x06,0x07,0x06,
0x07,0x21,0x07,0x21,0x07,0x21,0x07,0x21,0x07,0x11,0x07,0x11,0x07,0x11,0x07,0x11,
0x07,0x09,0x07,0x09,0x07,0x09,0x07,0x09,0x07,0x05,0x07,0x05,0x07,0x05,0x07,0x05,
0x06,0x3f,0x06,0x3f,0x06,0x3f,0x06,0x3f,0x06,0x3f,0x06,0x3f,0x06,0x3f,0x06,0x3f,
0x06,0x03,0x06,0x03,0x06,0x03,0x06,0x03,0x06,0x03,0x06,0x03,0x06,0x03,0x06,0x03,
0x06,0x24,0x06,0x24,0x06,0x24,0x06,0x24,0x06,0x24,0x06,0x24,0x06,0x24,0x06,0x24,
0x06,0x18,0x06,0x18,0x06,0x18,0x06,0x18,0x06,0x18,0x06,0x18,0x06,0x18,0x06,0x18
};

VOCONST VO_U8 mb_type_p[64] =
{
0x00,0xd1,0xa9,0xa9,0xab,0xab,0xb0,0xb0,0x62,0x62,0x62,0x62,0x62,0x62,0x62,0x62,
0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x48,
0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,
0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a,0x2a
};

VOCONST VO_U8 mb_type_b[64] =
{
0x00,0xd1,0xcd,0xcb,0xaf,0xaf,0xb0,0xb0,0x82,0x82,0x82,0x82,0x8a,0x8a,0x8a,0x8a,
0x64,0x64,0x64,0x64,0x64,0x64,0x64,0x64,0x6c,0x6c,0x6c,0x6c,0x6c,0x6c,0x6c,0x6c,
0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,
0x4e,0x4e,0x4e,0x4e,0x4e,0x4e,0x4e,0x4e,0x4e,0x4e,0x4e,0x4e,0x4e,0x4e,0x4e,0x4e
};

#define SKIP_1	 0
#define SKIP_2	 8 // SKIP_1 table has 16 elements;
#define SKIP_3	 (8+8)
#define SKIP_END (8+8+64)

VOCONST VO_U8 skip[SKIP_END*2] = 
{
0xff,0xff,0x64,0x54,0x43,0x43,0x33,0x33,0x22,0x22,0x22,0x22,0x12,0x12,0x12,0x12,
0x07,0x0c,0x07,0x0b,0x07,0x0a,0x07,0x09,0x06,0x08,0x06,0x08,0x06,0x07,0x06,0x07,
0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,
0x0a,0x21,0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,
0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,0x0a,0x22,
0x0a,0x20,0x0a,0x1f,0x0a,0x1e,0x0a,0x1d,0x0a,0x1c,0x0a,0x1b,0x0a,0x1a,0x0a,0x19,
0x0a,0x18,0x0a,0x17,0x0a,0x16,0x0a,0x15,0x09,0x14,0x09,0x14,0x09,0x13,0x09,0x13,
0x09,0x12,0x09,0x12,0x09,0x11,0x09,0x11,0x09,0x10,0x09,0x10,0x09,0x0f,0x09,0x0f,
0x07,0x0e,0x07,0x0e,0x07,0x0e,0x07,0x0e,0x07,0x0e,0x07,0x0e,0x07,0x0e,0x07,0x0e,
0x07,0x0d,0x07,0x0d,0x07,0x0d,0x07,0x0d,0x07,0x0d,0x07,0x0d,0x07,0x0d,0x07,0x0d
};

static INLINE VO_VOID readskip( MpegDecode* dec )
{
	VO_U32 i;
	// at least 24 bits available here;
	while (!GetBits(dec, 1))
	{
		i = SHOW_BITS(dec,4);
		if (i >= 2){
			i = skip[i];
			FLUSH_BITS(dec, (i&0xf));
			dec->skip += (i>>4);
			return;
		}

		i = SHOW_BITS(dec,10);
		if (i >= (1<<6))
			i = (i >> 3)-8 + SKIP_2;
		else
			i = i + SKIP_3;

		i <<= 1;
		FLUSH_BITS(dec,skip[i]);
		i = skip[i+1];
		dec->skip += i;
		if (i<33)
			break;

		// i should be 33;
		if (EofBits(dec))
			break;

		// need 11 bits for next iteration;
// 		loadbits_check(dec, dec->bits, dec->bitpos, 32-11);
		UPDATE_CACHE(dec);
	}
}

VOCONST VO_U8 mv_tab[64] = 
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x84,0x7c,0x74,0x6c,
	0x64,0x5c,0x53,0x53,0x4b,0x4b,0x43,0x43,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,
	0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x29,0x29,0x29,0x29,0x29,0x29,0x29,0x29,
	0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20
};

#define getMVData( dec, table, fc1, last, bitpos, cache_a, cache_b)\
{														\
	int code, v, fcode1, fcode2, shift;					\
	CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max 17 bits needed*/ \
	code = SHOW_BIT_POS(cache_a,10);					\
	if (code < 512){									\
		if (code >= 64){								\
			v = dec->numLeadingZeros[code>>3];			\
			code = v-1;									\
		}else{											\
			code = table[code];							\
			v = (code & 7)+6;							\
			code >>= 3;									\
		}												\
		FLUSH_BIT_POS(cache_a, cache_b, bitpos,v);					\
		v = GET_BITS_1_POS(cache_a, cache_b, bitpos);					\
		fcode1 = fc1;									\
		fcode2 = 1 << (fcode1+4);						\
		if(!IsMpeg1(dec))								\
			fcode1 -= 1;								\
		/* To Check: max 6bits */						\
		if (fcode1){									\
			code = (code - 1) << fcode1;				\
			code |= SHOW_BIT_POS(cache_a,fcode1);	\
			FLUSH_BIT_POS(cache_a, cache_b, bitpos,fcode1);			\
			code++;										\
		}												\
		if (v)											\
			code = -code;								\
		last += code;									\
		last = ((last + fcode2) & (fcode2*2-1)) - fcode2;\
		shift = INT_BIT - 5 - fcode1;					\
		last = (last<<shift)>>shift;					\
	}else{												\
		FLUSH_BIT_POS(cache_a, cache_b, bitpos,1);					\
	}													\
}


Image*  buf_seq_ctl(FifoType *priv, Image* img , const VO_U32 flag)
{
	Image *temp = NULL;
	VO_U32 w_idx = 0;

	w_idx = priv->w_idx;    /* Save the old index before proceeding */
	if (FIFO_WRITE == flag){ /*write data*/

		if(NULL != img){
			/* Save it to buffer */
			if (((priv->w_idx + 1) % FIFO_NUM) == priv->r_idx) {
				/* Adjust read index since buffer is full */
				/* Keep the latest one and drop the oldest one */
				priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
			}
			priv->img_seq[priv->w_idx] = img;
			priv->w_idx = (priv->w_idx + 1) % FIFO_NUM;
		}

	}else{/*read data*/
		if ((priv->r_idx == w_idx)){
			return NULL;/*there is no free buffer*/
		}else{
			temp = priv->img_seq[priv->r_idx];
			priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
		}
	}
	return temp;
}


#define INT_BIT 32

static  VO_VOID dualPrimeArithmetic(VO_S32 dmvec[][1+6], VO_S32 *dmvector, VO_S32 mvx, VO_S32 mvy , MpegDecode* dec)
{
	VO_S32 top_field_first;
	VO_S32 DMV[2][2];
	top_field_first = dec->mpeg2_flag && 8;
	if (PICT_FRAME==dec->picture_structure)
	{
		if (top_field_first)
		{
			/* vector for prediction of top field from bottom field */
			DMV[0][0] = ((mvx  +(mvx>0))>>1) + dmvector[0];
			DMV[0][1] = ((mvy  +(mvy>0))>>1) + dmvector[1] - 1;

			/* vector for prediction of bottom field from top field */
			DMV[1][0] = ((3*mvx+(mvx>0))>>1) + dmvector[0];
			DMV[1][1] = ((3*mvy+(mvy>0))>>1) + dmvector[1] + 1;
			dmvec[2][1] = MAKEMV(DMV[0][0], DMV[0][1]);
			dmvec[3][1] = MAKEMV(DMV[1][0], DMV[1][1]);

			DMV[0][0] /= 2;
			DMV[0][1] /= 2;
			DMV[1][0] /= 2;
			DMV[1][1] /= 2;
			dmvec[2][5] = MAKEMV(DMV[0][0], DMV[0][1]);
			dmvec[3][5] = MAKEMV(DMV[1][0], DMV[1][1]);
			
			
			
		}
		else
		{
			/* vector for prediction of top field from bottom field */
			DMV[0][0] = ((3*mvx+(mvx>0))>>1) + dmvector[0];
			DMV[0][1] = ((3*mvy+(mvy>0))>>1) + dmvector[1] - 1;

			/* vector for prediction of bottom field from top field */
			DMV[1][0] = ((mvx  +(mvx>0))>>1) + dmvector[0];
			DMV[1][1] = ((mvy  +(mvy>0))>>1) + dmvector[1] + 1;
			dmvec[2][1] = MAKEMV(DMV[0][0], DMV[0][1]);
			dmvec[3][1] = MAKEMV(DMV[1][0], DMV[1][1]);

			DMV[0][0] /= 2;
			DMV[0][1] /= 2;
			DMV[1][0] /= 2;
			DMV[1][1] /= 2;
			dmvec[2][5] = MAKEMV(DMV[0][0], DMV[0][1]);
			dmvec[3][5] = MAKEMV(DMV[1][0], DMV[1][1]);
		}
	}
	else
	{
		/* vector for prediction from field of opposite 'parity' */
		DMV[0][0] = ((mvx+(mvx>0))>>1) + dmvector[0];
		DMV[0][1] = ((mvy+(mvy>0))>>1) + dmvector[1];

		/* correct for vertical field shift */
		if (PICT_TOP_FIELD==dec->picture_structure)
			DMV[0][1]--;
		else
			DMV[0][1]++;
		dmvec[2][1] = MAKEMV(DMV[0][0], DMV[0][1]);

		DMV[0][0] /= 2;
		dmvec[2][5] = MAKEMV(DMV[0][0], DMV[0][1]);
		
	}
}

static __inline int  getDmvector(MpegDecode* dec,int bitpos)
{
	int dmvector_bit;
	
	dmvector_bit = GetBits(dec,1);

	if (1==dmvector_bit)
	{		
		dmvector_bit = GetBits(dec, 1)	;
        return (1==dmvector_bit) ? -1 : 1;
	}
	else
	{
		return 0;
	}

}

static VO_VOID getMV(MpegDecode* dec, int index, int field, int isForward,const int dmv_flage)
{
	VO_S32 mv_x, mv_y;
	VO_U32 cache_a, cache_b;
	VO_S32 bitpos;
	VO_S32 full;
	VO_S16 *pred_mv;
	VO_S32 *fcode, *mv;
	VO_S32 *mvb;

	fcode = &dec->mpeg_f_code[0][0];
	pred_mv = dec->pred_mv + (index<<1);
	if (isForward){
		mv = &dec->fmv[0][0];
		full = dec->full_pixel[0];
	}
	else {
		fcode += 2;
		pred_mv += 4;
		mv = &dec->bmv[0][0];
		full = dec->full_pixel[1];
	}
	mv_x = pred_mv[MVXIDX];
	mv_y = pred_mv[MVYIDX]>>field;

	BEGIN_BITS_POS(dec);
	getMVData(dec, mv_tab, fcode[0], mv_x, bitpos, cache_a, cache_b);
	if(1==dmv_flage)
	{
		END_BITS_POS(dec);
	    mvb = &dec->fmv[0][1];	
		dec->dmvector[0] =  getDmvector(dec,bitpos);
		BEGIN_BITS_POS(dec);
	}
	else
	{
		mvb = NULL;	//for clean warning
	}
	getMVData(dec, mv_tab, fcode[1], mv_y, bitpos, cache_a, cache_b);
	END_BITS_POS(dec);

	if(1==dmv_flage)
	{
	     dec->dmvector[1] =  getDmvector(dec,bitpos);
	}

	pred_mv[MVXIDX] = (VO_S16)mv_x;
	pred_mv[MVYIDX] = (VO_S16)(mv_y<<field);

	if (full){
		mv_x <<= 1;
		mv_y <<= 1;
	}

	//TBD
	index += 1; 
    
	dec->mvx = mv_x;
	dec->mvy = mv_y;
	mv[index] = MAKEMV(mv_x, mv_y);
	if(1==dmv_flage)
	{
		mvb[index] = MAKEMV(mv_x, mv_y);
	}
	//TBD
	mv_x /= 2;
	mv_y /= 2;

	mv[4+index] = mv[5+index] = MAKEMV(mv_x, mv_y);
	if(1 == dmv_flage)
	{
		mvb[4+index] = mvb[5+index] = MAKEMV(mv_x, mv_y);
	}
}

static VO_VOID update_mv(VO_S32 *mv, VO_S16 mv_x, VO_S16 mv_y)
{
	mv[0] = MAKEMV(mv_x, mv_y);

	mv_x /= 2;
	mv_y /= 2;

	mv[4] = mv[5] = MAKEMV(mv_x, mv_y);
}

VO_VOID Bit16_Sat_Add_c(VO_S16 *buf, VO_U8 *src, VO_U8 *dst, VO_S32 buf_stride,VO_S32 src_stride, VO_S32 dst_stride)
{
	VO_U32 i;
	VO_U32 data = 0;
	VO_S32 d0,d1,d2,d3,d4,d5,d6,d7;

	for(i = 0; i < 8; i++){
		d0 = buf[0];
		d1 = buf[1];
		d2 = buf[2];
		d3 = buf[3];
		d4 = buf[4];
		d5 = buf[5];
		d6 = buf[6];
		d7 = buf[7];

		if (src){
			d0 += src[0];
			d1 += src[1];
			d2 += src[2];
			d3 += src[3];
			d4 += src[4];
			d5 += src[5];
			d6 += src[6];
			d7 += src[7];
			src += src_stride;
		}

		if ((d5|d1|d7|d2|d3|d4|d0|d6)>>8){
			SAT_16(d0)
			SAT_16(d1)
			SAT_16(d2)
			SAT_16(d3)
			SAT_16(d4)
			SAT_16(d5)
			SAT_16(d6)
			SAT_16(d7)
		}

		data = d0|(d1 << 8)|(d2 << 16)|(d3 << 24);
		((VO_U32 *)dst)[0] = data;
		data = d4|(d5 << 8)|(d6 << 16)|(d7 << 24);
		((VO_U32 *)dst)[1] = data;
		buf += buf_stride;
		dst += dst_stride;
	}

}
//#endif

#ifndef MPEG2_DEC
static VO_VOID mpeg1_dec_mb_intra(MpegDecode* dec, Mp2DecLocal* mp2_dec_local)
{
	int j, i;
	VO_U32 cache_a, cache_b;
	VO_S32 bitpos;

	const VO_U16 *table = mpeg1_vld;
	const int qscale = dec->qscale;
	VO_S16 *block = dec->blockptr;

	if(GET_CON_MOTION_VEC(dec->mpeg2_flag))
	{
		getMV(dec, 0, 0, 1, 0);
		UPDATE_CACHE(dec);
		GetBits(dec, 1);
	}

	BEGIN_BITS_POS(dec);
	for (j = 0; j < 6; ++j){
		int dct_dc_size, dct_dc_diff;
		int scantype,  len;	
		VO_S32 dst_stride = dec->img_stride;
		VO_U8 *pDst;

		CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max 22 bits needed for DC*/
		if (j < 4){
			
			if((1 == dec->ds_Bframe)&&(B_VOP == dec->prediction_type))
				pDst = mp2_dec_local->frame_dst[0] + (j >> 1) * dst_stride * 4 + ((j&1)<<3);
			else
				pDst = mp2_dec_local->frame_dst[0] + (j >> 1) * dst_stride * 8 + ((j&1)<<(3-DOWNSAMPLE_FLAG(dec)));
			dct_dc_size = getDCsizeLum(dec, &cache_a, &cache_b, &bitpos);
			i = 0;
		}
		else {
			dst_stride = dec->img_stride_uv;
			pDst = mp2_dec_local->frame_dst[j-3];
			dct_dc_size = getDCsizeChr(dec, &cache_a, &cache_b, &bitpos);
			i = j - 3;
		}

		dct_dc_diff = dec->last_dc[i];
		if (dct_dc_size){
			dct_dc_diff += getDCdiff(dct_dc_size,dec, &cache_a, &cache_b, &bitpos);
			dec->last_dc[i] = dct_dc_diff;
		}

		clearblock(block);	
		len = 1;
		
		do { // event vld
		
			VO_S32 code,level;
			
			*block = (VO_S16)(dct_dc_diff << 3);//huwei 20101209 optimizing compilers bug
			
			vld_decode;
			level = code & 0x7f;
			if (level < (1+(62<<1))) 
			{
				level = (level>>1) * qscale;
				len += (0x1f & (code>>7)); // run
				level *= dec->IntraMatrix[len];
				level >>= 3;
				level = (level-1)|1;
				if (GET_BITS_1_POS(cache_a, cache_b, bitpos)) 
					level = -level;
			}else {
				if (level==(1+(63<<1)))
					break;

				// this value is escaped
				CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max bits 22 needed*/
				len += SHOW_BIT_POS(cache_a,6); FLUSH_BIT_POS(cache_a, cache_b, bitpos,6);
				code = SHOW_BIT_POS(cache_a,8); FLUSH_BIT_POS(cache_a, cache_b, bitpos,8);
				level = (code << 24) >> 24; //sign extend the lower 8 bits
				if (level == -128){
					level = SHOW_BIT_POS(cache_a,8)-256; FLUSH_BIT_POS(cache_a, cache_b, bitpos,8);
				}else if(level == 0){
					level = SHOW_BIT_POS(cache_a,8); FLUSH_BIT_POS(cache_a, cache_b, bitpos,8);
				}

				if (level<0){
					level= -level;
					level *= qscale * dec->IntraMatrix[len];
					level >>= 3;
					level= (level-1)|1;
					level= -level;
				}else{
					level *= qscale * dec->IntraMatrix[len];
					level >>= 3;
					level = (level-1)|1;
				}
			}
			block[dec->zigzag[len]] = (VO_S16)level;
			++len;
		} while (len <= 64);
		scantype = IDCTSCAN_ZIGZAG;


		if(DOWNSAMPLE_FLAG(dec)){
			IDCT_Block4x8_2(block,pDst,dst_stride<<(dec->interlaced_dct*(j<4)),NULL, 0);
		}else{
#ifndef IPP_EDIT
			if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
			{
				if (scantype!=IDCTSCAN_ALT_HORI && len < 15){
					IDCT_Block4x4_2(block,pDst,dst_stride,NULL, 0);
				}else{
					IDCT_Block8x4(block,pDst,dst_stride,NULL, 0);
				}
			}
			else
			{
				if (dec->callback_flage){
					if (scantype!=IDCTSCAN_ALT_HORI && len < 11){
						Bit16_IDCT_Block4x4_Block8x8(block,dec->block_buffer,8,NULL, 0);
					}else if(scantype!=IDCTSCAN_ALT_HORI && len < 15){
						Bit16_IDCT_Block4x8(block,dec->block_buffer,8,NULL, 0);
					}else{
						Bit16_IDCT_Block8x8(block,dec->block_buffer,8,NULL, 0);
					}
					dec->fp_postIDCTCallback(dec->group_time_code, dec->temporal_reference, dec->prediction_type, 
						(dec->xmb_pos + 1), (dec->ymb_pos + 1), j, dec->block_buffer);
					Bit16_Sat_Add(dec->block_buffer, NULL, pDst, 8,  0, dst_stride);
				} else{
					if (scantype!=IDCTSCAN_ALT_HORI && len < 11){
						IDCT_Block4x4(block,pDst,dst_stride,NULL, 0);
					}else if(scantype!=IDCTSCAN_ALT_HORI && len < 15){
						IDCT_Block4x8(block,pDst,dst_stride,NULL, 0);
					}else{
						IDCT_Block8x8(block,pDst,dst_stride,NULL, 0);
					}
				}				
			}
#else //IPP_EDIT
			ippiDCT8x8Inv_16s8u_C1R(block,pDst,dst_stride<<(dec->interlaced_dct*(j<4)));
			clearblock(dec->blockptr);
#endif //IPP_EDIT
		}
	}

	END_BITS_POS(dec);
}
#endif

static VO_VOID mpeg2_dec_mb_intra(MpegDecode* dec, Mp2DecLocal* mp2_dec_local)
{
	int j, i;
	VO_U32 cache_a, cache_b;
	VO_S32 bitpos;

	const VO_U16 *table;
	const int qscale = dec->qscale;

	if(GET_INTRA_VLC_FORT(dec->mpeg2_flag))
		table = mpeg2_vld;
	else
		table = mpeg1_vld;

	if(GET_CON_MOTION_VEC(dec->mpeg2_flag))
	{
		getMV(dec, 0, 0, 1, 0); 
		UPDATE_CACHE(dec);
		GetBits(dec, 1);
	}
	
	BEGIN_BITS_POS(dec);
	for (j = 0; j < 6; ++j){
		int dct_dc_size, dct_dc_diff;
		int scantype, len;
		VO_S16 *block = dec->blockptr;
		int mismatch;
		VO_S32 dst_stride = dec->img_stride;
		VO_U8 *pDst;

		CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max 22 bits needed for DC*/
		if (j < 4){
			if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
			{
				pDst = mp2_dec_local->frame_dst[0] + (j >> 1) * dst_stride * 
					(4 - dec->interlaced_dct*3) + ((j&1)<<3);
			}
			else
			{
				pDst = mp2_dec_local->frame_dst[0] + (j >> 1) * dst_stride * 
					(8 - dec->interlaced_dct*7) + ((j&1)<<(3-DOWNSAMPLE_FLAG(dec)));
			}
			dct_dc_size = getDCsizeLum(dec, &cache_a, &cache_b, &bitpos);
			i = 0;
		}
		else {
			dst_stride = dec->img_stride_uv;
			pDst = mp2_dec_local->frame_dst[j-3];
			dct_dc_size = getDCsizeChr(dec, &cache_a, &cache_b, &bitpos);
			i = j - 3;
		}
		
		dct_dc_diff = dec->last_dc[i];
		if (dct_dc_size){
			dct_dc_diff += getDCdiff(dct_dc_size,dec, &cache_a, &cache_b, &bitpos);
			dec->last_dc[i] = dct_dc_diff;
		}

		clearblock(block);	
		*block = (VO_S16)(dct_dc_diff << (3 - dec->intra_dc_precision));
		mismatch = block[0] ^ 1;
		len = 1;

		do { // event vld		
			VO_S32 code,level;
			vld_decode
			level = code & 0x7f;
			if (level < (1+(62<<1))){ 
				level = (level>>1)*qscale;
				len += (0x1f & (code>>7)); // run
				level *= dec->IntraMatrix[len];
				level >>= 4;
				if (GET_BITS_1_POS(cache_a, cache_b, bitpos)) 
					level = -level;
			}else {
				if (level==(1+(63<<1)))
					break;

				// this value is escaped
				CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max bits 18 needed*/
				len += SHOW_BIT_POS(cache_a, 6); 
				level = ((VO_S32)((cache_a) << 6) >> 20); //sign extend the lower 12 bits
				FLUSH_BIT_POS(cache_a, cache_b, bitpos, 18);

               if(level<0){
					level = -level;
                    level= (level*qscale*dec->IntraMatrix[len])>>4;
                    level= -level;
                }else{
                    level= (level*qscale*dec->IntraMatrix[len])>>4;
                }

			}
			mismatch^= level;
			block[dec->zigzag[len]] = (VO_S16)level;
			++len;
		} while (len <= 64);
		block[63]^= mismatch&1;
		scantype = IDCTSCAN_ZIGZAG;

#ifndef REF_DCT
		if(DOWNSAMPLE_FLAG(dec)){
			IDCT_Block4x8_2(block,pDst,dst_stride<<(dec->interlaced_dct*(j<4)),NULL, 0);
		}else{
#ifndef IPP_EDIT
			if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
			{
				if((dec->interlaced_dct)&&(j<4))
				{
					if(j<2)
					{
						if (scantype!=IDCTSCAN_ALT_HORI && len < 15){
							IDCT_Block4x8(block,pDst,dst_stride,NULL, 0);
						}else{
							IDCT_Block8x8(block,pDst,dst_stride,NULL, 0);
						}
					}
				}
				else
				{
					if (scantype!=IDCTSCAN_ALT_HORI && len < 15){
						IDCT_Block4x4_2(block,pDst,dst_stride,NULL, 0);
					}else{
						IDCT_Block8x4(block,pDst,dst_stride,NULL, 0);
					}
				}
			}
			else
			{				
				if (dec->callback_flage){
					if (scantype!=IDCTSCAN_ALT_HORI && len < 11){
						Bit16_IDCT_Block4x4_Block8x8(block,dec->block_buffer,8,NULL, 0);
					}else if (scantype!=IDCTSCAN_ALT_HORI && len < 15){
						Bit16_IDCT_Block4x8(block,dec->block_buffer,8, NULL, 0);
					}else{
						Bit16_IDCT_Block8x8(block,dec->block_buffer,8, NULL, 0);
					}
					dec->fp_postIDCTCallback(dec->group_time_code, dec->temporal_reference, dec->prediction_type,
						(dec->xmb_pos + 1), (dec->ymb_pos + 1), j, dec->block_buffer);
					Bit16_Sat_Add(dec->block_buffer, NULL, pDst, 8,  0, dst_stride<<(dec->interlaced_dct*(j<4)));
				}else{
					if (scantype!=IDCTSCAN_ALT_HORI && len < 11){
						IDCT_Block4x4(block,pDst,dst_stride<<(dec->interlaced_dct*(j<4)),NULL, 0);
					}else if (scantype!=IDCTSCAN_ALT_HORI && len < 15){
						IDCT_Block4x8(block,pDst,dst_stride<<(dec->interlaced_dct*(j<4)),NULL, 0);
					}else{
						IDCT_Block8x8(block,pDst,dst_stride<<(dec->interlaced_dct*(j<4)),NULL, 0);
					}
				}
			}
#else //IPP_EDIT
			ippiDCT8x8Inv_16s8u_C1R(block,pDst,dst_stride<<(dec->interlaced_dct*(j<4)));
			clearblock(dec->blockptr);
#endif //IPP_EDIT
		}

#else
		Reference_IDCT(block);
		Add_Block(block, pDst, dst_stride<<(dec->interlaced_dct*(j<4)), 0, pDst, 0);
#endif
	}
	END_BITS_POS(dec);
}

#ifndef MPEG2_DEC
static int decodeInter_mpeg1( MpegDecode* dec, VO_S16 *block )
{
	const VO_U16 *table = mpeg1_vld;
	const int qscale = dec->qscale;

	VO_U32 cache_a, cache_b;
	VO_S32 bitpos;
	int len;
	int code,level;	

	BEGIN_BITS_POS(dec);
	CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max 2 bits needed*/
	len = 0;

	// special case for dc
	code = SHOW_BIT_POS(cache_a,2);
	if (code & 2){ 
		FLUSH_BIT_POS(cache_a, cache_b, bitpos,2);
		level= (3 * qscale * dec->InterMatrix[0]) >> 4;
		level= (level-1)|1;
		if (code & 1)
			level= -level;
		block[0] = (VO_S16)level;
		++len;
	}

	do { // event vld
		vld_decode;
		level = code & 0x7f;
		if (level < (1+(62<<1))){ 
			level *= qscale;
			len += (0x1f & (code>>7)); // run
			level *= dec->InterMatrix[len];
			level >>= 4;
			level = (level-1)|1;

			if (GET_BITS_1_POS(cache_a, cache_b, bitpos)) 
				level = -level;
		}else{ 
			if (level==(1+(63<<1)))
				break;

			// this value is escaped
			CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max bits 22 needed*/

			len += SHOW_BIT_POS(cache_a,6); FLUSH_BIT_POS(cache_a, cache_b, bitpos,6);
			code = SHOW_BIT_POS(cache_a,8); FLUSH_BIT_POS(cache_a, cache_b, bitpos,8);
			level = (code << 24) >> 24; //sign extend the lower 8 bits
			if (level == -128){
				level = SHOW_BIT_POS(cache_a,8)-256; FLUSH_BIT_POS(cache_a, cache_b, bitpos,8);
			}else if (level == 0){
				level = SHOW_BIT_POS(cache_a,8); FLUSH_BIT_POS(cache_a, cache_b, bitpos,8);
			}

			if (level<0){
				level= -level;
				level = (((level<<1)+1) * qscale * dec->InterMatrix[len])>>4;
				level= (level-1)|1;
				level= -level;
			}else{
				level = (((level<<1)+1) * qscale * dec->InterMatrix[len])>>4;
				level = (level-1)|1;
			}
		}
		block[dec->zigzag[len]] = (VO_S16)level;
		++len;
	} while (len <= 64);

	END_BITS_POS(dec);
	return len;
}
#endif
static int decodeInter_mpeg2(MpegDecode* dec, VO_S16 *block )
{
	const VO_U16 *table = mpeg1_vld;
	const int qscale = dec->qscale;

	VO_U32 cache_a, cache_b;
	VO_S32 bitpos;
	int len;
	int code,level;
    int mismatch = 1;

	BEGIN_BITS_POS(dec);
	len = 0;
	// special case for dc
	CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max 2 bits needed*/
	code = SHOW_BIT_POS(cache_a,2);
	if (code & 2){ 
		FLUSH_BIT_POS(cache_a, cache_b, bitpos,2);
		level= (3 * qscale * dec->InterMatrix[0]) >> 5;
		if (code & 1)
			level= -level;
		block[0] = (VO_S16)level;
		mismatch ^= level;
		len = 1;
	}

	do { // event vld
		vld_decode
		level = code & 0x7f;
		if (level < (1+(62<<1))) {
			level *= qscale;
			len += (0x1f & (code>>7)); // run
			level = ((level * dec->InterMatrix[len]) >> 5);
			if (GET_BITS_1_POS(cache_a, cache_b, bitpos)) 
				level = -level;
		}else{
			if (level==(1+(63<<1)))
				break;
			//escaped
			CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max bits 18 needed*/
			len += SHOW_BIT_POS(cache_a,6); 
			level = ((VO_S32)((cache_a) << 6) >> 20); //sign extend the lower 12 bits
			FLUSH_BIT_POS(cache_a, cache_b, bitpos, 18);
			if (level<0){
				level= -level;
				level = (((level<<1)+1) * qscale * dec->InterMatrix[len])>>5;
				level= -level;
			}else{
				level =(((level<<1)+1) * qscale * dec->InterMatrix[len])>>5;
			}
		}
		mismatch ^= level;
		block[dec->zigzag[len]] = (VO_S16)level;
		len++;
	} while (len <= 64);
	block[63] ^= (mismatch & 1);
	END_BITS_POS(dec);
	return len;
}

static VO_VOID Bmpeg2_dec_mb_intra(MpegDecode* dec, Mp2DecLocal* mp2_dec_local)
{
	int j, i;
	VO_U32 cache_a, cache_b;
	VO_S32 bitpos;
	const VO_U16 *table;
	const int qscale = dec->qscale;

	if(GET_INTRA_VLC_FORT(dec->mpeg2_flag))
		table = mpeg2_vld;
	else
		table = mpeg1_vld;

	if(GET_CON_MOTION_VEC(dec->mpeg2_flag))
	{
		getMV(dec, 0, 0, 1, 0); 
		UPDATE_CACHE(dec);
		GetBits(dec, 1);
	}
	
	BEGIN_BITS_POS(dec);
	for (j = 0; j < 6; ++j){
		int dct_dc_size, dct_dc_diff;
		int scantype, len;
		VO_S16 *block = dec->blockptr;
		int mismatch;
		VO_S32 dst_stride = dec->img_stride;
		VO_U8 *pDst;

		CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max 22 bits needed for DC*/
		if (j < 4){
			if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
			{
				pDst = mp2_dec_local->frame_dst[0] + (j >> 1) * dst_stride * 
					(4 - dec->interlaced_dct*3) + ((j&1)<<3);
			}
			else
			{
				pDst = mp2_dec_local->frame_dst[0] + (j >> 1) * dst_stride * 
					(8 - dec->interlaced_dct*7) + ((j&1)<<(3-DOWNSAMPLE_FLAG(dec)));
			}
			dct_dc_size = getDCsizeLum(dec, &cache_a, &cache_b, &bitpos);
			i = 0;
		}
		else {
			dst_stride = dec->img_stride_uv;
			pDst = mp2_dec_local->frame_dst[j-3];
			dct_dc_size = getDCsizeChr(dec, &cache_a, &cache_b, &bitpos);
			i = j - 3;
		}
		
		dct_dc_diff = dec->last_dc[i];
		if (dct_dc_size){
			dct_dc_diff += getDCdiff(dct_dc_size,dec, &cache_a, &cache_b, &bitpos);
			dec->last_dc[i] = dct_dc_diff;
		}

		clearblock(block);	
		*block = (VO_S16)(dct_dc_diff << (3 - dec->intra_dc_precision));
		mismatch = block[0] ^ 1;
		len = 1;
		if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type)&&(j>1)&&(j<4)&&(dec->interlaced_dct))
		{
			do { // event vld		
				VO_S32 code,level;
				vld_decode
				level = code & 0x7f;
				if (level < (1+(62<<1))){ 
					len += (0x1f & (code>>7)); // run
					FLUSH_BIT_POS(cache_a, cache_b, bitpos, 1);
				}else {
					if (level==(1+(63<<1)))
						break;

					// this value is escaped
					CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max bits 18 needed*/
					len += SHOW_BIT_POS(cache_a, 6); 
					level = ((VO_S32)((cache_a) << 6) >> 20); //sign extend the lower 12 bits
					FLUSH_BIT_POS(cache_a, cache_b, bitpos, 18);
				}
				++len;
			} while (len <= 64);
		}
		else
		{
			do { // event vld		
				VO_S32 code,level;
				vld_decode
				level = code & 0x7f;
				if (level < (1+(62<<1))){ 
					len += (0x1f & (code>>7)); // run
					level = (level>>1)*qscale;
					level *= dec->IntraMatrix[len];
					level >>= 4;
					if (GET_BITS_1_POS(cache_a, cache_b, bitpos)) 
						level = -level;
				}else {
					if (level==(1+(63<<1)))
						break;

					// this value is escaped
					CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max bits 18 needed*/
					len += SHOW_BIT_POS(cache_a, 6); 
					level = ((VO_S32)((cache_a) << 6) >> 20); //sign extend the lower 12 bits
					FLUSH_BIT_POS(cache_a, cache_b, bitpos, 18);
               	if(level<0){
				level = -level;
				level= (level*qscale*dec->IntraMatrix[len])>>4;
				level= -level;
                	}else{
				level= (level*qscale*dec->IntraMatrix[len])>>4;
                	}

				}
				mismatch^= level;
				block[dec->zigzag[len]] = (VO_S16)level;
				++len;
			} while (len <= 64);
			block[63]^= mismatch&1;
		}
		scantype = IDCTSCAN_ZIGZAG;
#ifndef REF_DCT
		if(DOWNSAMPLE_FLAG(dec)){
			IDCT_Block4x8_2(block,pDst,dst_stride<<(dec->interlaced_dct*(j<4)),NULL, 0);
		}
		else{
#ifndef IPP_EDIT
			if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
			{
				if((dec->interlaced_dct)&&(j<4))
				{
					if(j<2)
					{
						if (scantype!=IDCTSCAN_ALT_HORI && len < 15){
							IDCT_Block4x8(block,pDst,dst_stride,NULL, 0);
						}else{
							IDCT_Block8x8(block,pDst,dst_stride,NULL, 0);
						}
					}
				}
				else
				{
					IDCT_Block8x4(block,pDst,dst_stride,NULL, 0);
				}
			}else{
				if (scantype!=IDCTSCAN_ALT_HORI && len < 11){
					IDCT_Block4x4(block,pDst,dst_stride<<(dec->interlaced_dct*(j<4)),NULL, 0);
				}else if(scantype!=IDCTSCAN_ALT_HORI && len < 15){
					IDCT_Block4x8(block,pDst,dst_stride<<(dec->interlaced_dct*(j<4)),NULL, 0);
				}else{
					IDCT_Block8x8(block,pDst,dst_stride<<(dec->interlaced_dct*(j<4)),NULL, 0);
				}
			}
#else //IPP_EDIT
			ippiDCT8x8Inv_16s8u_C1R(block,pDst,dst_stride<<(dec->interlaced_dct*(j<4)));
			clearblock(dec->blockptr);
#endif //IPP_EDIT
		}

#else
		Reference_IDCT(block);
		Add_Block(block, pDst, dst_stride<<(dec->interlaced_dct*(j<4)), 0, pDst, 0);
#endif
	}
	END_BITS_POS(dec);
}

#ifndef MPEG2_DEC
static int BdecodeInter_mpeg1( MpegDecode* dec, VO_S16 *block )
{
	const VO_U16 *table = mpeg1_vld;
//	const int qscale = dec->qscale;

	VO_U32 cache_a, cache_b;
	VO_S32 bitpos;
	int len;
	int code,level;	

	BEGIN_BITS_POS(dec);
	CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max 2 bits needed*/
	len = 0;

	// special case for dc
	code = SHOW_BIT_POS(cache_a,2);
	if (code & 2){ 
		FLUSH_BIT_POS(cache_a, cache_b, bitpos,2);
		++len;
	}

	do { // event vld
		vld_decode;
		level = code & 0x7f;
		if (level < (1+(62<<1))){ 
			len += (0x1f & (code>>7)); // run
			FLUSH_BIT_POS(cache_a, cache_b, bitpos, 1);
		}else{ 
			if (level==(1+(63<<1)))
				break;

			// this value is escaped
			CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max bits 22 needed*/

			len += SHOW_BIT_POS(cache_a,6); FLUSH_BIT_POS(cache_a, cache_b, bitpos,6);
			code = SHOW_BIT_POS(cache_a,8); FLUSH_BIT_POS(cache_a, cache_b, bitpos,8);
			level = (code << 24) >> 24; //sign extend the lower 8 bits
			if (level == -128){
				level = SHOW_BIT_POS(cache_a,8)-256; FLUSH_BIT_POS(cache_a, cache_b, bitpos,8);
			}else if (level == 0){
				level = SHOW_BIT_POS(cache_a,8); FLUSH_BIT_POS(cache_a, cache_b, bitpos,8);
			}
		}
		++len;
	} while (len <= 64);

	END_BITS_POS(dec);
	return len;
}
#endif
static int BdecodeInter_mpeg2(MpegDecode* dec, VO_S16 *block )
{
	const VO_U16 *table = mpeg1_vld;
//	const int qscale = dec->qscale;

	VO_U32 cache_a, cache_b;
	VO_S32 bitpos;
	int len;
	int code,level;

	BEGIN_BITS_POS(dec);
	len = 0;
	// special case for dc
	CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max 2 bits needed*/
	code = SHOW_BIT_POS(cache_a,2);
	if (code & 2){ 
		FLUSH_BIT_POS(cache_a, cache_b, bitpos,2);
		len = 1;
	}


	do { // event vld
		vld_decode
		level = code & 0x7f;
		if (level < (1+(62<<1))) {
			len += (0x1f & (code>>7)); // run
			FLUSH_BIT_POS(cache_a, cache_b, bitpos, 1);
		}else{
			if (level==(1+(63<<1)))
				break;
			//escaped
			CACHE_CHECK(dec, cache_a, cache_b, bitpos); /*max bits 18 needed*/
			len += SHOW_BIT_POS(cache_a,6); 
			level = ((VO_S32)((cache_a) << 6) >> 20); //sign extend the lower 12 bits
			FLUSH_BIT_POS(cache_a, cache_b, bitpos, 18);
		}
		len++;
	} while (len <= 64);
	END_BITS_POS(dec);
	return len;
}

static VO_VOID dec_mb_inter( MpegDecode* dec, Mp2DecLocal *mp2_dec_local, int mb_type, int dmv_flage)
{
	int len1,cbp,j, jCount; 
	int isMBPat = mb_type & MB_PAT;
	if (isMBPat){
		UPDATE_CACHE(dec);
		len1 = SHOW_BITS(dec,9);
		if (len1 >= 128)
			len1 >>= 4;
		else
			len1 += 32;
		len1 <<= 1;
		if(len1<-1)
		{
			len1 = -1;
		}
		if(len1>318)
		{
			len1 = 318;
		}
		FLUSH_BITS(dec,mcbp_p[len1]);
		cbp = mcbp_p[len1+1];
		if ((cbp & 0x3) == 0 && (!mp2_dec_local->MVBack ||
			!mp2_dec_local->MVFwd)){
			// Chroma coeff are all zeros;
			// avoid frame data cache miss in B prediction;
			isMBPat |= (1<<(MB_PAT+1)); 
			jCount = 4;
		}
		else
			jCount = 6;
	}
	else
	{
		cbp = jCount = 0;//for clean warning
	}

	if (dec->mv_type == MT_FRAME){
#ifdef BFRAME_NOERR
		const COPYBLOCK_PTR *all_copy_block;
		const addblock *all_add_block;
		if ((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type)){
			all_copy_block = dec->all_copy_block;
			all_add_block  = dec->all_add_block;

			dec->all_copy_block = BAllCopyBlock8x4;
			dec->all_add_block  = BAllAddBlock8x4;
		}
#endif//BFRAME_NOERR
		MotionComp(dec, mp2_dec_local, 0, isMBPat, 0);
#ifdef BFRAME_NOERR
		if ((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type)){
			dec->all_copy_block = all_copy_block;
			dec->all_add_block  = all_add_block;
		}
#endif//BFRAME_NOERR
	}
	else if(dec->mv_type ==MT_DMV){
		if(IsFramePicture(dec))
		{
			if((1 == dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
			{
				MotionComp(dec, mp2_dec_local, 1, isMBPat, 0);     
				MotionComp(dec, mp2_dec_local, 3, isMBPat, 1);
			}
			else
			{
				MotionComp(dec, mp2_dec_local, 1, isMBPat, 0);     
				MotionComp(dec, mp2_dec_local, 3, isMBPat, dmv_flage);
				MotionComp(dec, mp2_dec_local, 3, isMBPat, 0); 
				MotionComp(dec, mp2_dec_local, 1, isMBPat, dmv_flage);
			}
		}
		else
		{
			if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
			{
				MotionComp(dec, mp2_dec_local, 1, isMBPat, 0);     
				MotionComp(dec, mp2_dec_local, 3, isMBPat, 1);
			}
			else
			{
				MotionComp(dec, mp2_dec_local, 1, isMBPat, 0);     
				MotionComp(dec, mp2_dec_local, 3, isMBPat, dmv_flage);
				MotionComp(dec, mp2_dec_local, 3, isMBPat, 0); 
				MotionComp(dec, mp2_dec_local, 1, isMBPat, dmv_flage);
			}
		}
	}
	else{
		if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
		{
			MotionComp(dec, mp2_dec_local, 1, isMBPat, 0);
		}
		else
		{
			MotionComp(dec, mp2_dec_local, 1, isMBPat, 0);
			MotionComp(dec, mp2_dec_local, 3, isMBPat, 0);
		}
	}

	if (isMBPat){
//		DPRINTF(ff_debug,"cbp:%d \n",cbp);
		for (j = 0; j < jCount; j++, cbp+=cbp){
			int len;
			VO_S32 src_stride;
			VO_U8 *dst, *src;
			VO_S16 *block = dec->blockptr;
			VO_S32 dst_stride = dec->img_stride;			
			
			if (j < 4){
				if(DOWNSAMPLE_FLAG(dec))
				{
					dst = mp2_dec_local->frame_dst[0] + (j >> 1) * dst_stride * 
						(8 - dec->interlaced_dct*7) + ((j&1)<<2);

					src =  dec->mc_dst[0] + 
						(j>>1)*8*(8-dec->interlaced_dct*7) + ((j&1)<<2);
					dst_stride <<= dec->interlaced_dct;
					src_stride   = (8<<dec->interlaced_dct);
				}
				else
				{
					if(1== dec->ds_Bframe)
					{
						if(B_VOP==dec->prediction_type)
						{
							dst = mp2_dec_local->frame_dst[0] + (j >> 1) * dst_stride * 
								(4 - dec->interlaced_dct*3) + ((j&1)<<3);
							src =  dec->mc_dst[0] + 
								(j>>1)*16*(4 -dec->interlaced_dct*3) + ((j&1)<<3);
							
							src_stride   = 16;
						}
						else
						{
							dst = mp2_dec_local->frame_dst[0] + 
								(j>>1)*dst_stride*(8-dec->interlaced_dct*7) + ((j&1)<<3);
							src =  dec->mc_dst[0] + 
								(j>>1)*16*(8-dec->interlaced_dct*7) + ((j&1)<<3);
							
							dst_stride <<= dec->interlaced_dct;
							src_stride   = (16<<dec->interlaced_dct);
						}
					}
					else
					{
						dst = mp2_dec_local->frame_dst[0] + (j >> 1) * dst_stride*
							(8 - dec->interlaced_dct*7) + ((j&1)<<3);
						src =  dec->mc_dst[0] + 
							(j>>1)*16*(8-dec->interlaced_dct*7) + ((j&1)<<3);

						dst_stride <<= dec->interlaced_dct;
						src_stride   = (16<<dec->interlaced_dct);
					}
				}
			}
			else {

				if (dec->uv_interlace_flage)
				{
					src_stride = 16>>(DOWNSAMPLE_FLAG(dec));
				} 
				else
				{
					src_stride = 8>>(DOWNSAMPLE_FLAG(dec));
					dst_stride >>= 1;
				}
				dst = mp2_dec_local->frame_dst[j-3];
				src = dec->mc_dst[j-3];
			}

			if (cbp & 32){
				clearblock(block);
				if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type)&&(j>1)&&(j<4)&&(dec->interlaced_dct))
				{
					if(!IsMpeg1(dec))
						len = BdecodeInter_mpeg2(dec, block);				
					else
						len = BdecodeInter_mpeg1(dec, block);
				}
				else
				{
					if(!IsMpeg1(dec))
						len = decodeInter_mpeg2(dec, block);				
					else
						len = decodeInter_mpeg1(dec, block);
				}
				
#ifndef REF_DCT
				if(DOWNSAMPLE_FLAG(dec)){
					IDCT_Block4x8_2(block,dst,dst_stride,src, src_stride);
				}
				else
				{
#ifndef IPP_EDIT
					if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
					{
						//IDCT_Block4x8_2(block,dst,dst_stride,src, src_stride);
						if((dec->interlaced_dct)&&(j<4))
						{
							if(j<2)
							{
								if (len == 1){
									IDCT_Const8x8((block[0]+4)>>3, dst,dst_stride, src,src_stride);
								}else if (len < 15 || (len<26 && ((VO_U32*)block)[2]==0 && ((VO_U32*)block)[6]==0)){
									IDCT_Block4x8(block,dst,dst_stride,src,src_stride);
								}else{
									IDCT_Block8x8(block,dst,dst_stride,src,src_stride);
								}
							}
						}
						else
						{
							if (len < 15 || (len<26 && ((VO_U32*)block)[2]==0 && ((VO_U32*)block)[6]==0))
							{
								// TBD: about 70% here is 4x4, consider to use 4x4 IDCT 
								IDCT_Block4x4_2(block,dst,dst_stride,src,src_stride);
							}
							else
							{
								IDCT_Block8x4(block,dst,dst_stride,src, src_stride);
							}
						}
					}
					else
					{
						if (dec->callback_flage){
							if (len == 1){
								Bit16_IDCT_Const8x8((block[0]+4)>>3, dec->block_buffer, 8, src,src_stride);
							}else if(len < 11){
								Bit16_IDCT_Block4x4_Block8x8(block, dec->block_buffer, 8,src,src_stride);
							}else if (len < 15 || (len < 26 && ((VO_U32*)block)[2]==0 && ((VO_U32*)block)[6]==0)){
								Bit16_IDCT_Block4x8(block,dec->block_buffer, 8,src,src_stride);
							}else{
								Bit16_IDCT_Block8x8(block,dec->block_buffer, 8,src,src_stride);
							}
							dec->fp_postIDCTCallback(dec->group_time_code, dec->temporal_reference, dec->prediction_type,
								(dec->xmb_pos + 1), (dec->ymb_pos + 1), j, dec->block_buffer);
							Bit16_Sat_Add(dec->block_buffer, src, dst, 8,  src_stride, dst_stride);
						}else{
							if (len == 1){
								IDCT_Const8x8((block[0]+4)>>3, dst,dst_stride, src,src_stride);
							}else if(len < 11){
								IDCT_Block4x4(block,dst,dst_stride,src,src_stride);
							}else if (len < 15 || (len < 26 && ((VO_U32*)block)[2]==0 && ((VO_U32*)block)[6]==0)){
								// TBD: about 70% here is 4x4, consider to use 4x4 IDCT 
								// (len < 11 for both scans) to replace 4x8;
								IDCT_Block4x8(block,dst,dst_stride,src,src_stride);
							}else{
								IDCT_Block8x8(block,dst,dst_stride,src,src_stride);
							}
						}
					}
#else //IPP_EDIT
					if (len == 1){
						IDCT_Const8x8((block[0]+4)>>3, dst,dst_stride, src,src_stride);
					}else if (len < 11){
						ippiDCT8x8Inv_4x4_16s_C1(block, dec->pBuff);
						ippiAdd8x8_16s8u_C1IRS(dec->pBuff, 16, src, src_stride);
						ippiCopy8x8_8u_C1R(src, src_stride, dst, dst_stride);
						//CopyBlock8x8(src, dst, src_stride, dst_stride, 8);
						clearblock4x8(dec->blockptr);
					}else{
						 ippiDCT8x8Inv_16s_C1(block, dec->pBuff);
						 ippiAdd8x8_16s8u_C1IRS(dec->pBuff, 16, src, src_stride);
						 ippiCopy8x8_8u_C1R(src, src_stride, dst, dst_stride);
						 //CopyBlock8x8(src, dst, src_stride, dst_stride, 8);
						 clearblock(dec->blockptr);
					}
#endif //IPP_EDIT
				}
#else
				Reference_IDCT(block);
				Add_Block(block, dst, dst_stride, 1, src, src_stride);
#endif
			}else{
				if(DOWNSAMPLE_FLAG(dec))
				{
					CopyBlock4x8(src, dst, src_stride, dst_stride, 8);
				}
				else
				{
#ifndef IPP_EDIT
					if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
					{
						if((dec->interlaced_dct)&&(j<2))
							CopyBlock8x8(src, dst, src_stride, dst_stride, 8);
						else if((!(dec->interlaced_dct))||(j>3))
							CopyBlock8x8(src, dst, src_stride, dst_stride, 4);
					}
					else
					{
						CopyBlock8x8(src, dst, src_stride, dst_stride, 8);
					}
#else //IPP_EDIT
					ippiCopy8x8_8u_C1R(src, src_stride, dst, dst_stride);
#endif //IPP_EDIT
				}
			}
		}		
	}

	dec->last_dc[2] =
	dec->last_dc[1] =
	dec->last_dc[0] = 128 << dec->intra_dc_precision;
}

//int aaa,disp;
static int IVOP_Slice( MpegDecode* dec, int pos, VO_S32 *cur_pos)
{
	Mp2DecLocal mp2_dec_local;
	VO_S32 ymb_pos, xmb_pos, offset, endY, endUV, i, j;
	VO_S32 offset_temp;
//	VO_S32 ret;
	ymb_pos = POSY(pos); 
	xmb_pos = POSX(pos);

	offset = (ymb_pos << 4) * dec->img_stride;
	endY  = offset + (xmb_pos << (4- DOWNSAMPLE_FLAG(dec)));
	offset_temp = dec->uv_interlace_flage?(offset>>1):(offset>>2);
	endUV = offset_temp + (xmb_pos << (3- DOWNSAMPLE_FLAG(dec)));

	if((0 == pos)||(dec->get_buffer_flage)){
		dec->current_frame = buf_seq_ctl(&dec->privFIFO, NULL, FIFO_READ);
		if(NULL == dec->current_frame){
			dec->get_buffer_flage = 1;
			return VO_MEMRC_NO_YUV_BUFFER;
		}
		dec->get_buffer_flage = 0;
	}

	if((xmb_pos<0)||(xmb_pos>=dec->mb_w)){
		return ERR_SLICECODE;
	}

	if((ymb_pos<0)||(ymb_pos>=dec->mb_h)){
		return ERR_SLICECODE;
	}

	mp2_dec_local.frame_dst[0] = dec->current_frame->y + endY; 
	mp2_dec_local.frame_dst[1] = dec->current_frame->u + endUV; 
	mp2_dec_local.frame_dst[2] = dec->current_frame->v + endUV;

	endY  = (dec->img_stride<<4);
	offset_temp = (dec->uv_interlace_flage)?(endY>>1):(endY>>2);
	endUV = offset_temp + (8>>DOWNSAMPLE_FLAG(dec)) - (dec->img_stride>>1);
	endY += ((16>>DOWNSAMPLE_FLAG(dec)) - dec->img_stride);

//	DPRINTF(ff_debug,"------------------start I slice pos:%d \n",pos);//,FrameCount);

	for ( ; ymb_pos < dec->mb_h; ymb_pos++){ 
		for ( ; xmb_pos < dec->mb_w; ){ 		
			VO_S32 dequant;

			dec->xmb_pos = xmb_pos;
			dec->ymb_pos = ymb_pos;

			if(!(dequant = GetBits(dec, 1))){
				FLUSH_BITS(dec,1); // should be 1
			}
			if(!IsMpeg1(dec)){
				dec->interlaced_dct = (!GET_FPFD(dec->mpeg2_flag) && 
					IsFramePicture(dec))?GetBits(dec, 1):0;
			}

			if (!dequant){
	            dec->qscale = get_qscale(dec);
			}
			if(!IsMpeg1(dec))
				mpeg2_dec_mb_intra(dec, &mp2_dec_local);
			else
				mpeg1_dec_mb_intra(dec, &mp2_dec_local);

			UPDATE_CACHE(dec);
			if (SHOW_BITS(dec,8)==0){ // eof slice
				return ERR_NONE;
			}

			if (!GetBits(dec, 1))
				return ERR_NONE; // skip invalid with IVOP

			i = 16>>DOWNSAMPLE_FLAG(dec);
			j = 8>>DOWNSAMPLE_FLAG(dec);
			if (++xmb_pos >= dec->mb_w){
				i = endY;
				j = endUV;
			}
			mp2_dec_local.frame_dst[0] += i;
			mp2_dec_local.frame_dst[1] += j;
			mp2_dec_local.frame_dst[2] += j;
		}
		if(xmb_pos >= dec->mb_w)
			xmb_pos = 0;
	}

//	*cur_pos = pos;
	return ERR_NONE;
}


static int PVOP_Slice( MpegDecode* dec, VO_S32 pos, VO_S32 *cur_pos)
{
	Mp2DecLocal mp2_dec_local;
	VO_S32 *tmp32;
	VO_S32 ymb_pos, xmb_pos, offset, endY, endUV, i, j, numMB;
	VO_S32 offset_temp;
//	VO_S32 ret;

	VO_S32 dmv_flage = 0;
//	DPRINTF(ff_debug,"------------------start P slice pos:%d \n",pos);//,FrameCount);
	mp2_dec_local.MVFwd = NULL;	
	ymb_pos = POSY(pos); 
	xmb_pos = POSX(pos);

	offset = (ymb_pos << 4) * dec->img_stride;
	endY  = offset + (xmb_pos << (4 - DOWNSAMPLE_FLAG(dec)));
	offset_temp = (dec->uv_interlace_flage)?(offset>>1):(offset>>2);
	endUV = offset_temp + (xmb_pos <<(3 - DOWNSAMPLE_FLAG(dec)));

	if((0 == pos)||(dec->get_buffer_flage)){
		dec->current_frame = buf_seq_ctl(&dec->privFIFO, NULL, FIFO_READ);
		if(NULL == dec->current_frame){
			dec->get_buffer_flage = 1;
			return VO_MEMRC_NO_YUV_BUFFER;
		}
		dec->get_buffer_flage = 0;//huwei 20101228 stability
	}

	if((xmb_pos<0)||(xmb_pos>= dec->mb_w)){
		return ERR_SLICECODE;
	}

	if((ymb_pos<0)||(ymb_pos>= dec->mb_h)){
		return ERR_SLICECODE;
	}	

	mp2_dec_local.frame_dst[0] = dec->current_frame->y + endY; 
	mp2_dec_local.frame_dst[1] = dec->current_frame->u + endUV; 
	mp2_dec_local.frame_dst[2] = dec->current_frame->v + endUV;
	mp2_dec_local.ref[0]	   = dec->reference_frame->y + endY; 			
	mp2_dec_local.ref[1]       = dec->reference_frame->u + endUV; 
	mp2_dec_local.ref[2]       = dec->reference_frame->v + endUV; 

	endY  = (dec->img_stride<<4);
	offset_temp = (dec->uv_interlace_flage)?(endY>>1):(endY>>2);
	endUV = offset_temp + (8>>DOWNSAMPLE_FLAG(dec)) - (dec->img_stride>>1);
	endY += ((16>>DOWNSAMPLE_FLAG(dec)) - dec->img_stride);

	numMB = ymb_pos * dec->mb_w + xmb_pos;
	for ( ; ymb_pos < dec->mb_h; ymb_pos++){ 
		for ( ; xmb_pos < dec->mb_w; ){
#ifdef SKIP_IDENTICAL_COPY
			VO_U32 copyMask;
			uint8 *copyLoc = dec->frPCopyFlag + (numMB>>3);
			copyMask = (1<<(numMB&7));
			numMB++;
#endif
			dmv_flage = 0;
			dec->xmb_pos = xmb_pos;
			dec->ymb_pos = ymb_pos;
			if (!dec->skip){
				int mb_type;
#ifdef SKIP_IDENTICAL_COPY
				*copyLoc &= ~copyMask; // unmark copied;
#endif
				UPDATE_CACHE(dec);
				mb_type = mb_type_p[SHOW_BITS(dec, 6)];
				FLUSH_BITS(dec, mb_type>>5);

				//motion type
				if(!IsMpeg1(dec)){
					if(mb_type & MB_FOR){
						dec->mv_type = GET_FPFD(dec->mpeg2_flag) ? MT_FRAME : GetBits(dec, 2);
					}
					
					dec->interlaced_dct = (!GET_FPFD(dec->mpeg2_flag)) && 
						(mb_type & (MB_PAT|MB_INTRA)) && IsFramePicture(dec) ? GetBits(dec, 1) :0;
				}

				if (mb_type & MB_QUANT)
		            dec->qscale = get_qscale(dec);

				if (!(mb_type & MB_INTRA)){
					if (mb_type & MB_FOR){
						if(IsFramePicture(dec)){
							switch(dec->mv_type){
							case MT_FRAME:
								dec->field_select[0][0] = dec->field_select[0][1] = 0;
								getMV(dec, 0, 0, 1, 0);
//								*(VO_S32 *)(dec->pred_mv+2) = *(VO_S32 *)(dec->pred_mv);
								dec->pred_mv[2] = dec->pred_mv[0];
								dec->pred_mv[3] = dec->pred_mv[1];
//								DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[0][0][0]);
//								DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[0][0][1]);
								break;
							case MT_FIELD:
								for(j = 0; j < 2; j++) {
									dec->field_select[0][j] = GetBits(dec, 1);
									getMV(dec, j, 1, 1, 0);
//									DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[0][j][0]);
//									DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[0][j][1]>>1);
								}
								break;
							case MT_DMV:
								dmv_flage = 1; 
								dec->field_select[0][0] = 0;
								dec->field_select[0][1] = 1;
								getMV(dec, 0, 1, 1, dmv_flage);
// 								*(VO_S32 *)(dec->pred_mv+2) = *(VO_S32 *)(dec->pred_mv);
								dec->pred_mv[2] = dec->pred_mv[0];
								dec->pred_mv[3] = dec->pred_mv[1];
//							    DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[0][j][0]);
//								DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[0][j][1]>>1);
								dualPrimeArithmetic(dec->fmv,dec->dmvector, dec->mvx, dec->mvy, dec);
								break;
							default:
								break;
							}
						}
						else{
							switch(dec->mv_type){
							case MT_FRAME:
								for(j = 0; j < 2; j++) {
									dec->field_select[0][j] = GetBits(dec, 1);
									getMV(dec, j, 0, 1, 0);
//									DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[0][j][0]);
//									DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[0][j][1]);
								} 
								break;
							case MT_FIELD:
								dec->field_select[0][0] = GetBits(dec, 1);
								getMV(dec, 0, 1, 1, 0);
//								DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[0][0][0]);
//								DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[0][0][1]>>1);
								break;
							case MT_DMV:
								dmv_flage = 1;
								dec->field_select[0][0] = GetBits(dec, 1);
								getMV(dec, 0, 1, 1, dmv_flage);
//								DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[0][0][0]);
//								DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[0][0][1]>>1);
								dualPrimeArithmetic(dec->fmv,dec->dmvector, dec->mvx, dec->mvy, dec);
								break;
							default:
								break;
							}
						}
					}else{
						if(IsFramePicture(dec)){
							dec->mv_type = MT_FRAME;
						}else{
							dec->mv_type = MT_FIELD;
							dec->field_select[0][0] = (dec->picture_structure==PICT_BOTTOM_FIELD);
						}
						
						dec->fmv[0][1] = dec->fmv[0][2] = dec->fmv[0][5] = dec->fmv[0][6] = 0;
						tmp32 = (VO_S32 *)dec->pred_mv;
						tmp32[0] = tmp32[1] = 0;
					}
					mp2_dec_local.MVBack = &dec->fmv[0][1];	
					dec_mb_inter(dec, &mp2_dec_local, mb_type, dmv_flage);
				}else{
					if(!IsMpeg1(dec)){
						mpeg2_dec_mb_intra(dec, &mp2_dec_local);
					}
					else{
						mpeg1_dec_mb_intra(dec, &mp2_dec_local);
						dec->fmv[0][0] = dec->bmv[0][0] = 0;
					}
					tmp32 = (VO_S32 *)dec->pred_mv;
					tmp32[0] = tmp32[1] = tmp32[2] = tmp32[3] = 0;
				}	

				if((ymb_pos == (dec->mb_h -1)) && (xmb_pos == (dec->mb_w - 1))){
					return ERR_NONE;
				}
				
				UPDATE_CACHE(dec);
				if (SHOW_BITS(dec,8)==0){ // eof slice
					*cur_pos = pos;
					return ERR_NONE;
				}

				readskip(dec); // after this 12bits left

			}else{
#ifdef SKIP_IDENTICAL_COPY
				if (!(*copyLoc & copyMask)){
					*copyLoc |= copyMask; // mark as copied;
#else // SKIP_IDENTICAL_COPY
				{
#endif // SKIP_IDENTICAL_COPY
					// not coded macroblock
					if(DOWNSAMPLE_FLAG(dec))
					{
						CopyBlock8x16(mp2_dec_local.ref[0], mp2_dec_local.frame_dst[0], dec->img_stride, dec->img_stride, 16);
						CopyBlock4x8(mp2_dec_local.ref[1], mp2_dec_local.frame_dst[1], dec->img_stride_uv, dec->img_stride_uv, 8);
						CopyBlock4x8(mp2_dec_local.ref[2], mp2_dec_local.frame_dst[2], dec->img_stride_uv, dec->img_stride_uv, 8);
					}
					else
					{
#ifndef IPP_EDIT
						CopyBlock16x16(mp2_dec_local.ref[0], mp2_dec_local.frame_dst[0], dec->img_stride, dec->img_stride, 16);
						CopyBlock8x8(mp2_dec_local.ref[1], mp2_dec_local.frame_dst[1], dec->img_stride_uv, dec->img_stride_uv, 8);
						CopyBlock8x8(mp2_dec_local.ref[2], mp2_dec_local.frame_dst[2], dec->img_stride_uv, dec->img_stride_uv, 8);
#else //IPP_EDIT
						ippiCopy16x16_8u_C1R(mp2_dec_local.ref[0], dec->img_stride, mp2_dec_local.frame_dst[0],  dec->img_stride);
						ippiCopy8x8_8u_C1R(mp2_dec_local.ref[1], dec->img_stride_uv, mp2_dec_local.frame_dst[1], dec->img_stride_uv);
						ippiCopy8x8_8u_C1R(mp2_dec_local.ref[2], dec->img_stride_uv, mp2_dec_local.frame_dst[2], dec->img_stride_uv);
#endif //IPP_EDIT
					}
				}
				if (--dec->skip == 0){
					//Reset predict mv
					tmp32 = (VO_S32 *)dec->pred_mv;
					tmp32[0] = tmp32[1] = 0;				
					dec->last_dc[2] =\
						dec->last_dc[1] =\
						dec->last_dc[0] = 128 << dec->intra_dc_precision;
				}
			}
			i = 16>>DOWNSAMPLE_FLAG(dec);
			j = 8>>DOWNSAMPLE_FLAG(dec);
			if (++xmb_pos >= dec->mb_w){
				i = endY;
				j = endUV;
			}
			mp2_dec_local.ref[0] += i; 
			mp2_dec_local.ref[1] += j; 
			mp2_dec_local.ref[2] += j; 
			mp2_dec_local.frame_dst[0] += i;
			mp2_dec_local.frame_dst[1] += j;
			mp2_dec_local.frame_dst[2] += j;
		}
		if(xmb_pos >= dec->mb_w)
			xmb_pos = 0;
	}

	return ERR_NONE;
}

static int BVOP_Slice( MpegDecode* dec, VO_S32 pos, VO_S32 *cur_pos )
{
	Mp2DecLocal mp2_dec_local;
	VO_S32 ymb_pos, xmb_pos, offset, endY, endUV, i, j, numMB;
	VO_S32 offset_temp;
	VO_S32 BendY, BendUV,Bi, Bj;
//	VO_S32 ret;
	VO_S32 dmv_flage = 0;
	VO_S32 skipReset = 1;
	VO_U8 **srcCopyBlk = NULL;

	mp2_dec_local.MVFwd = NULL;
	mp2_dec_local.MVBack = NULL;

//	DPRINTF(ff_debug,"------------------start %d B slice pos:%d \n",iii, pos)
	ymb_pos = POSY(pos); 
	xmb_pos = POSX(pos);
	
	if((xmb_pos<0)||(xmb_pos>=dec->mb_w)){
		return ERR_SLICECODE;
	}

	if((ymb_pos<0)||(ymb_pos>=dec->mb_h)){
		return ERR_SLICECODE;
	}

	if((0 == pos)||(dec->get_buffer_flage)){
		dec->Bframe = buf_seq_ctl(&dec->privFIFO, NULL, FIFO_READ);
		if(NULL == dec->Bframe){
			dec->get_buffer_flage = 1;
			return VO_MEMRC_NO_YUV_BUFFER;
		}
		dec->get_buffer_flage = 0;
	}

	offset = (ymb_pos << 4) * dec->img_stride;
	endY  = offset + (xmb_pos << (4- DOWNSAMPLE_FLAG(dec)));
	offset_temp = (dec->uv_interlace_flage)?(offset>>1):(offset>>2);
	endUV = offset_temp + (xmb_pos << (3- DOWNSAMPLE_FLAG(dec)));

	if ((NULL == dec->current_frame)||(NULL == dec->reference_frame)){
		return ERR_INVALID_DATA;
	}
	
	mp2_dec_local.ref[0] = dec->current_frame->y + endY; 
	mp2_dec_local.ref[1] = dec->current_frame->u + endUV; 
	mp2_dec_local.ref[2] = dec->current_frame->v + endUV; 
	mp2_dec_local.cur[0] = dec->reference_frame->y + endY; 
	mp2_dec_local.cur[1] = dec->reference_frame->u + endUV; 
	mp2_dec_local.cur[2] = dec->reference_frame->v + endUV;
	
	if(1== dec->ds_Bframe)
	{
		BendY  = (offset>>1) + (xmb_pos<<4);
		offset_temp = (dec->uv_interlace_flage)?(offset>>2):(offset>>3);
		BendUV = offset_temp + (xmb_pos<<3);

		mp2_dec_local.frame_dst[0] = dec->Bframe->y + BendY;
		mp2_dec_local.frame_dst[1] = dec->Bframe->u + BendUV;
		mp2_dec_local.frame_dst[2] = dec->Bframe->v + BendUV;
	}
	else
	{
		BendY = BendUV = 0;//for clean warning
		mp2_dec_local.frame_dst[0] = dec->Bframe->y + endY;
		mp2_dec_local.frame_dst[1] = dec->Bframe->u + endUV;
		mp2_dec_local.frame_dst[2] = dec->Bframe->v + endUV;
	}

	endY  = (dec->img_stride<<4);
	offset_temp = (dec->uv_interlace_flage)?(endY>>1):(endY>>2);
	endUV = offset_temp + (8>>DOWNSAMPLE_FLAG(dec)) - (dec->img_stride>>1);
	endY += ((16>>DOWNSAMPLE_FLAG(dec)) - dec->img_stride);

	if(1== dec->ds_Bframe)
	{
		BendY  = (dec->img_stride<<3);
		offset_temp = (dec->uv_interlace_flage)?(BendY>>1):(BendY>>2);
		BendUV = offset_temp + 8 - (dec->img_stride>>1);
		BendY += (16 - dec->img_stride);
	}

	numMB = ymb_pos * dec->mb_w + xmb_pos;
	for ( ; ymb_pos < dec->mb_h; ymb_pos++){ 
		for ( ; xmb_pos < dec->mb_w; numMB++){ 		
#ifdef SKIP_IDENTICAL_COPY
			VO_U32 copyMask, copyShift;
			uint8 *copyLoc = dec->frBCopyFlag + (numMB>>2);
			copyShift = ((numMB&3)<<1);
			copyMask = (0x3<<copyShift);
#endif
			dmv_flage = 0;
			dec->xmb_pos = xmb_pos;
			dec->ymb_pos = ymb_pos;
			if (!dec->skip){
				int mb_type;
#ifdef SKIP_IDENTICAL_COPY
				*copyLoc &= ~copyMask; // unmark copied;
#endif
				skipReset = 1;
				UPDATE_CACHE(dec);
				mb_type = mb_type_b[SHOW_BITS(dec, 6)];
				FLUSH_BITS(dec, mb_type>>5); // after this 6bits left

				if(!IsMpeg1(dec)){
					if(mb_type & (MB_FOR | MB_BACK)){
						dec->mv_type = GET_FPFD(dec->mpeg2_flag) ? MT_FRAME : GetBits(dec, 2);
					}
					
					dec->interlaced_dct = (!GET_FPFD(dec->mpeg2_flag)) && 
						(mb_type & (MB_PAT|MB_INTRA)) && IsFramePicture(dec) ? GetBits(dec, 1) : 0;
				}


				if (mb_type & MB_QUANT)
		            dec->qscale = get_qscale(dec);

				if (!(mb_type & MB_INTRA)){
					mp2_dec_local.MVBack = (mb_type & MB_FOR) ? &dec->fmv[0][1] : NULL;
					mp2_dec_local.MVFwd = (mb_type & MB_BACK) ? &dec->bmv[0][1] : NULL;
					if(IsFramePicture(dec)){
						switch(dec->mv_type){
						case MT_FRAME:
							if (mb_type & MB_FOR){	
								dec->field_select[0][0] = 0;
								dec->field_select[0][1] = 1;
								getMV(dec, 0, 0, 1, 0);
// 								*(VO_S32 *)(&dec->pred_mv[2]) = *(VO_S32 *)dec->pred_mv;
								dec->pred_mv[2] = dec->pred_mv[0];
								dec->pred_mv[3] = dec->pred_mv[1];
//								DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[0][0][0]);
//								DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[0][0][1]);
							}
							if (mb_type & MB_BACK){	
								dec->field_select[1][0] = dec->field_select[1][1] = 0;
								getMV(dec, 0, 0, 0, 0);
// 								*(VO_S32 *)(&dec->pred_mv[6]) = *(VO_S32 *)(&dec->pred_mv[4]);
								dec->pred_mv[6] = dec->pred_mv[4];
								dec->pred_mv[7] = dec->pred_mv[5];
//								DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[1][0][0]);
//								DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[1][0][1]);
							}
							break;
						case MT_FIELD:
							if (mb_type & MB_FOR){	
								for(j = 0; j < 2; j++) {
									dec->field_select[0][j] = GetBits(dec, 1);
									getMV(dec, j, 1, 1, 0);
//									DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[0][j][0]);
//									DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[0][j][1]>>1);
								}
							}
							if (mb_type & MB_BACK){	
								for(j = 0; j < 2; j++) {
									dec->field_select[1][j] = GetBits(dec, 1);
									getMV(dec, j, 1, 0, 0);		
//									DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[1][j][0]);
//									DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[1][j][1]>>1);
								}
							}
							break;
						case MT_DMV:
							//don't support now
							break;
						default:
							break;
						}
					}
					else {
						switch(dec->mv_type){
						case MT_FRAME:
							if (mb_type & MB_FOR){	
								for(j = 0; j < 2; j++) {
									dec->field_select[0][j] = GetBits(dec, 1);
									getMV(dec, j, 0, 1, 0);
//									DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[0][j][0]);
//									DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[0][j][1]);
								}
							}
							if (mb_type & MB_BACK){	
								for(j = 0; j < 2; j++) {
									dec->field_select[1][j] = GetBits(dec, 1);
									getMV(dec, j, 0, 0, 0);	
//									DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[1][j][0]);
//									DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[1][j][1]);
								}
							}
							break;
						case MT_FIELD:
							if (mb_type & MB_FOR){	
								dec->field_select[0][0] = GetBits(dec, 1);
								getMV(dec, 0, 1, 1, 0);
//								DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[0][0][0]);
//								DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[0][0][1]>>1);
							}
							if (mb_type & MB_BACK){	
								dec->field_select[1][0] = GetBits(dec, 1);
								getMV(dec, 0, 1, 0, 0);	
//								DPRINTF(ff_debug,"mvx:%d \n",dec->pred_mv[1][0][0]);
//								DPRINTF(ff_debug,"mvy:%d \n",dec->pred_mv[1][0][1]>>1);
							}
							break;
						case MT_DMV:
							//don't support now
							break;
						default:
							break;
						}
					}
					dec_mb_inter( dec, &mp2_dec_local, mb_type, dmv_flage);
				}else{
					VO_S32 *tmp32;
					if(!IsMpeg1(dec)){
						//mpeg2_dec_mb_intra(dec, &mp2_dec_local);
						if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type)&&(dec->interlaced_dct))
							Bmpeg2_dec_mb_intra(dec, &mp2_dec_local);
						else
							mpeg2_dec_mb_intra(dec, &mp2_dec_local);
					}
					else{
						mpeg1_dec_mb_intra(dec, &mp2_dec_local);
						dec->fmv[0][0] = dec->bmv[0][0] = 0;
					}

					tmp32 = (VO_S32 *)dec->pred_mv;
					tmp32[0] = tmp32[1] = tmp32[2] = tmp32[3] = 0;
				}	

				if((ymb_pos == (dec->mb_h -1)) && (xmb_pos == (dec->mb_w - 1))){
					return ERR_NONE;
				}

				UPDATE_CACHE(dec);
				if (SHOW_BITS(dec,8)==0){ // eof slice
					*cur_pos = pos; // TBD: not accurate;
					return ERR_NONE;
				}

				readskip(dec); // after this 12bits left
			}else{
				// not coded macroblock, use last motion compensation vectors
				dec->skip--;
				if (skipReset){
					skipReset = 0;
					if(IsFramePicture(dec)){
						dec->mv_type = MT_FRAME;
					}else{
						dec->mv_type = MT_FIELD;
						dec->field_select[0][0] = dec->field_select[0][1] = 
							(dec->picture_structure==PICT_BOTTOM_FIELD); 
					}

					srcCopyBlk = NULL;
					if(mp2_dec_local.MVFwd)
						update_mv(&dec->bmv[0][1], dec->pred_mv[4], dec->pred_mv[5]);
					else if (dec->fmv[0][1] == 0){
						srcCopyBlk = mp2_dec_local.ref;
#ifdef SKIP_IDENTICAL_COPY
						srcId = dec->current_frame->Id;
#endif // SKIP_IDENTICAL_COPY
					}

					if(mp2_dec_local.MVBack)
						update_mv(&dec->fmv[0][1], dec->pred_mv[0], dec->pred_mv[1]);
					else if (dec->bmv[0][1] == 0){
						srcCopyBlk = mp2_dec_local.cur;
#ifdef SKIP_IDENTICAL_COPY
						srcId = dec->reference_frame->Id;
#endif // SKIP_IDENTICAL_COPY
					}
					dec->last_dc[2] =\
						dec->last_dc[1] =\
						dec->last_dc[0] = 128 << dec->intra_dc_precision;
				}

				if(!srcCopyBlk){
#ifdef BFRAME_NOERR
					const COPYBLOCK_PTR *all_copy_block;
					const addblock *all_add_block;
					if ((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type)){
						all_copy_block = dec->all_copy_block;
						all_add_block  = dec->all_add_block;

						dec->all_copy_block = BAllCopyBlock8x4;
						dec->all_add_block  = BAllAddBlock8x4;
					}
#endif//BFRAME_NOERR
					MotionComp(dec, &mp2_dec_local, 0, 0, 0);
#ifdef BFRAME_NOERR
					if ((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type)){
						dec->all_copy_block = all_copy_block;
						dec->all_add_block  = all_add_block;
					}
#endif//BFRAME_NOERR
				}else{
//					VO_U32 prevBRefId;
					if(DOWNSAMPLE_FLAG(dec))
					{
						CopyBlock8x16(srcCopyBlk[0], mp2_dec_local.frame_dst[0], dec->img_stride, dec->img_stride, 16);
						CopyBlock4x8(srcCopyBlk[1], mp2_dec_local.frame_dst[1], dec->img_stride_uv, dec->img_stride_uv, 8);
						CopyBlock4x8(srcCopyBlk[2], mp2_dec_local.frame_dst[2], dec->img_stride_uv, dec->img_stride_uv, 8);
					}
					else
					{
#ifdef SKIP_IDENTICAL_COPY
						prevBRefId = (*copyLoc & copyMask);
						if (prevBRefId != (srcId<<copyShift)){
							*copyLoc = (*copyLoc & ~copyMask) | (srcId<<copyShift); // mark new id;
							// refId is different; But check if P2 ref P1 and if prevB ref,
							// for non-firstBFrameAfterIP, still the same data;
							if (!prevBRefId || dec->firstBFrameAfterIP || 
								!(dec->frPCopyFlag[numMB>>3]&(1<<(numMB&7))))
							{
#endif // SKIP_IDENTICAL_COPY

#ifndef IPP_EDIT
								if(1== dec->ds_Bframe)
								{
									CopyBlock16x16(srcCopyBlk[0], mp2_dec_local.frame_dst[0], dec->img_stride<<1, dec->img_stride, 8);
									CopyBlock8x8(srcCopyBlk[1], mp2_dec_local.frame_dst[1], dec->img_stride_uv<<1, dec->img_stride_uv, 4);
									CopyBlock8x8(srcCopyBlk[2], mp2_dec_local.frame_dst[2], dec->img_stride_uv<<1, dec->img_stride_uv, 4);
								}
								else
								{
									CopyBlock16x16(srcCopyBlk[0], mp2_dec_local.frame_dst[0], dec->img_stride, dec->img_stride, 16);
									CopyBlock8x8(srcCopyBlk[1], mp2_dec_local.frame_dst[1], dec->img_stride_uv, dec->img_stride_uv, 8);
									CopyBlock8x8(srcCopyBlk[2], mp2_dec_local.frame_dst[2], dec->img_stride_uv, dec->img_stride_uv, 8);
								}
#else //IPP_EDIT
								ippiCopy16x16_8u_C1R(srcCopyBlk[0],dec->img_stride, mp2_dec_local.frame_dst[0],  dec->img_stride);
								ippiCopy8x8_8u_C1R(srcCopyBlk[1], dec->img_stride_uv, mp2_dec_local.frame_dst[1], dec->img_stride_uv);
								ippiCopy8x8_8u_C1R(srcCopyBlk[2], dec->img_stride_uv, mp2_dec_local.frame_dst[2], dec->img_stride_uv);
#endif

#ifdef SKIP_IDENTICAL_COPY
							}
						}
#endif // SKIP_IDENTICAL_COPY
					}
				}
			}
			i = 16>>DOWNSAMPLE_FLAG(dec);
			j = 8>>DOWNSAMPLE_FLAG(dec);

			Bi = i;
			Bj = j;
			if (++xmb_pos >= dec->mb_w){
				i = endY;
				j = endUV;
				
				if(1== dec->ds_Bframe)
				{
					Bi = BendY;
					Bj = BendUV;
				}
				else
				{
					Bi = endY;
					Bj = endUV;
				}
			}
			mp2_dec_local.ref[0] += i; 
			mp2_dec_local.ref[1] += j; 
			mp2_dec_local.ref[2] += j;
			mp2_dec_local.cur[0] += i; 
			mp2_dec_local.cur[1] += j; 
			mp2_dec_local.cur[2] += j;

			mp2_dec_local.frame_dst[0] += Bi;
			mp2_dec_local.frame_dst[1] += Bj;
			mp2_dec_local.frame_dst[2] += Bj;
		}
		if(xmb_pos >= dec->mb_w)
			xmb_pos = 0;
	}

	*cur_pos = pos; // TBD: not accurate;
	return ERR_NONE;
}

static int mp2FindStartCode(MpegDecode* dec)
{
	//find 0x000001
	const VO_U8 *head, *endData = dec->pBitEnd - 8;
	head = bytepos(dec);

	if(head >= endData){
		return 0;
	}

	do {
		if (head[0]) {// faster if most of bytes are not zero;	
			head++;
			continue;
		}
		// find 00xx
		if (head[1]){
			head += 2; // skip two bytes;
			continue;
		}
		// find 0000xx;
		head += 3; // update pointer first, since it will be used in all cases below;
		switch (head[-1]){
		case 0:
			// 3 zeros;
			while (!*head++) 
			{
				if(head >= endData)
				{
					return 0;
				}
			}
			if (head[-1] != 1){
				continue;
			}
			// do not use "break;", let it enter "case 1";
		case 1:
			// find header;
// 			dec->bits = 0;
// 			dec->bitpos = 32;
// 			dec->bitptr = head;
			InitBitStream(dec, head, endData - head);
			return 1;
		default:
			// find 0000xx, xx > 1;
			// do nothing;
			break;
		}
	} while (head < endData);
	// not finding start code;
	return 0;
}

VO_U32 get_frame_type(MpegDecode *dec)
{
	VO_U32 data;
	VO_U32 Result = VO_ERR_NONE;
	VO_S32 Code;

	do 
	{
		if (!mp2FindStartCode(dec))
			return VO_ERR_FAILED;
		UPDATE_CACHE(dec);
		Code = GetBits(dec,8); 

	} while(PICTURE_START_CODE != Code);

	if (Code == PICTURE_START_CODE){
		// 0 bits available here;
		FLUSH_BITS(dec,10); // temporal ref
		UPDATE_CACHE(dec);
		// 30 bits available here;
		data = SHOW_BITS(dec, 27);
		dec->prediction_type = ((data>>24)&0x7); //GetBits(dec,3);
	}

	return Result;
}

VO_VOID static SetFunPtr(MpegDecode* dec)
{
	if(DOWNSAMPLE_FLAG(dec)){
		dec->fp_copy_block_luma   = CopyBlock8x16;
		dec->fp_copy_block_chroma = CopyBlock4x8;
		dec->all_copy_block = NewAllCopyBlock4_8;
		dec->all_add_block  = NewAllAddBlock4x8;
	}else{
		dec->fp_copy_block_luma   = CopyBlock16x16;
		dec->fp_copy_block_chroma = CopyBlock8x8;
		dec->all_copy_block = AllCopyBlock;
		dec->all_add_block  = AllAddBlock;
	}
}

int dec_frame( MpegDecode* dec, VO_U8* Ptr, int Len , int header, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat)
{
	int Result = ERR_NONE;
	int Code;
	VO_S32 cur_pos;
	VO_S32 *tmp32;

	if (Len == 0)
		return ERR_INVALID_DATA;

	dec->get_buffer_flage = 1;
	dec->xmb_pos = 0;
	dec->ymb_pos = 0;
	
	InitBitStream(dec,Ptr,Len);

	do{
		if (!mp2FindStartCode(dec))
			break;

		UPDATE_CACHE(dec);
		Code = GetBits(dec,8); 
		if(EofBits(dec)){
			break;
		}
		if (Code >= SLICE_MIN_START_CODE && Code <= SLICE_MAX_START_CODE){
			// found a slice
			if (Code > dec->mb_h){
				Result = ERR_INVALID_DATA;
				break;
			}
			if (dec->frame_state!=FRAME_DECODING)
			{
				Result = VO_ERR_WRONG_STATUS;
				break;
			}
			dec->qscale = get_qscale(dec);
			/* extra slice info */
			while (GetBits(dec, 1)){
				FLUSH_BITS(dec,8);
				UPDATE_CACHE(dec);
			}

			dec->skip = 0;
			readskip(dec); // TBD: is this necessary?
			Code = MB_X*(Code-1) + dec->skip;
			dec->skip = 0;

			dec->last_dc[2] =
			dec->last_dc[1] =
			dec->last_dc[0] = 128 << dec->intra_dc_precision;

			dec->fmv[0][0] = dec->bmv[0][0] = 0;
			tmp32 = (VO_S32 *)dec->pred_mv;
			tmp32[0] = tmp32[1] = tmp32[2] = tmp32[3] = 0;
			UPDATE_CACHE(dec);
			if((PICT_TOP_FIELD==dec->picture_structure)||(PICT_BOTTOM_FIELD==dec->picture_structure))
				return  VO_ERR_DEC_MPEG2_NOTSUPPORT_FIELD_ERR;
			
			switch (dec->prediction_type){ 
			case P_VOP:
				Result = PVOP_Slice(dec,Code, &cur_pos);			
				break;
			case I_VOP:
				Result = IVOP_Slice(dec,Code, &cur_pos);				
				break;
			case B_VOP:
				Result = BVOP_Slice(dec,Code, &cur_pos);	
				break;
			}

			if ((dec->ymb_pos == (dec->mb_h -1)) && (dec->xmb_pos == (dec->mb_w - 1))){
				dec->frame_state = FRAME_FINISH;
				break;
			}
		}else if (Code == PICTURE_START_CODE){
			if(dec->frame_state==FRAME_DECODING){
				dec->frame_state=FRAME_READY;
				break;
			}
				
			if (GET_VALID_SEQ(dec->mpeg2_flag)){

				Picture(dec);
				if ((dec->frame_num == 0 && dec->prediction_type != I_VOP) || 
					(dec->frame_num < 2 && dec->prediction_type == B_VOP))
				{
					return VO_ERR_WRONG_STATUS;
				}
				SetFunPtr(dec);

#ifdef LICENSEFILE
				if (((I_VOP == dec->display_ptype)||(P_VOP == dec->display_ptype)) &&(!dec->first_Iframe_flage) ){
					VO_VIDEO_BUFFER outBuffer;

					outBuffer.Buffer[0] = dec->pDisplayFrame->y;
					outBuffer.Buffer[1] = dec->pDisplayFrame->u;
					outBuffer.Buffer[2] = dec->pDisplayFrame->v;
					outBuffer.Stride[0] = dec->img_stride;
					outBuffer.Stride[1] = dec->img_stride_uv;
					outBuffer.Stride[2] = dec->img_stride_uv;
					outBuffer.ColorType = VO_COLOR_YUV_PLANAR420;
// 					outBuffer.Time = dec->outTimeStamp;
					voCheckLibResetVideo(dec->phCheck, &outBuffer);
				}
#endif

			}else{
				Result = ERR_INVALID_DATA;
			}
		}else if(Code == SEQ_START_CODE){
			sequence_header(dec);
		}else if(Code == GOP_START_CODE){
			group_header(dec);
		}else if (Code == EXT_START_CODE){
			Result = mpeg_decode_extension(dec);
			
			if((PICT_TOP_FIELD==dec->picture_structure)||(PICT_BOTTOM_FIELD==dec->picture_structure))
				return  VO_ERR_DEC_MPEG2_NOTSUPPORT_FIELD_ERR;
			
			if(header)
				break;
		}
	}while (Result == ERR_NONE);
	

	if(!header){

		if ((FRAME_READY == dec->frame_state)||(FRAME_FINISH == dec->frame_state)/*||(dec->xmb_pos > 0)||(dec->ymb_pos > 0)*/){
			//TBD
			dec->frame_num++;
			//dec->frame_state = FRAME_BEGIN;

			if (dec->get_buffer_flage){
				pOutData->Buffer[0] = NULL;
				pOutData->Buffer[1] = NULL;
				pOutData->Buffer[2] = NULL;
				pOutFormat->Format.Type = VO_VIDEO_FRAME_NULL;

				return ERR_INVALID_DATA;
			}

			switch (dec->prediction_type){ 
			case I_VOP:
#ifdef SKIP_IDENTICAL_COPY
				// reset frame copy flag;
				tmp32 = (VO_S32 *)dec->frPCopyFlag;
				i = dec->frPCopySize32;
				do {
					*tmp32++ = 0;
				} while (--i);
				// do not use break here, let it go to next case;
#endif // SKIP_IDENTICAL_COPY
			case P_VOP:
#ifdef SKIP_IDENTICAL_COPY
				dec->lastRefId++;
				if (dec->lastRefId == 4)
					dec->lastRefId = 1;
				dec->current_frame->Id = dec->lastRefId;
				dec->firstBFrameAfterIP = 1;
#endif // SKIP_IDENTICAL_COPY

				if (0 == dec->outputMode){
					pOutData->Buffer[0] = dec->reference_frame->y;
					pOutData->Buffer[1] = dec->reference_frame->u;
					pOutData->Buffer[2] = dec->reference_frame->v;
				}else{
					pOutData->Buffer[0] = dec->current_frame->y;
					pOutData->Buffer[1] = dec->current_frame->u;
					pOutData->Buffer[2] = dec->current_frame->v;
				}

				break;
			case B_VOP:
				pOutData->Buffer[0] = dec->Bframe->y;
				pOutData->Buffer[1] = dec->Bframe->u;
				pOutData->Buffer[2] = dec->Bframe->v;

#ifdef SKIP_IDENTICAL_COPY
				dec->firstBFrameAfterIP = 0;
#endif // SKIP_IDENTICAL_COPY
				break;
			}

			pOutData->Stride[0] = dec->img_stride;

			pOutData->Stride[1] = 
				pOutData->Stride[2] = dec->img_stride_uv;

			pOutData->ColorType = VO_COLOR_YUV_PLANAR420;
			pOutFormat->Format.Width = dec->out_img_width>>DOWNSAMPLE_FLAG(dec);
			
			if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
				pOutFormat->Format.Height = (dec->out_img_height>>1);
			else
				pOutFormat->Format.Height = dec->out_img_height;
			
			if((I_VOP == dec->prediction_type)&&(1 == dec->first_Iframe_flage)){
				if (0 == dec->outputMode){
					pOutFormat->Format.Type = VO_VIDEO_FRAME_NULL;
					pOutData->Time  = 0;
// 					dec->timeStampPre = dec->timeStamp;
					dec->user_data_pre  = dec->user_data;
					dec->first_Iframe_flage = 0;
					dec->previous_prediction_type = dec->prediction_type;
					dec->displayFramePre = NULL;
					dec->reference_frame = dec->current_frame;
					dec->pre_progressive_frame = dec->cur_progressive_frame;
					dec->displayFrameProgressive = 1;
				}else{
					pOutFormat->Format.Type = dec->prediction_type;
// 					pOutData->Time  = dec->timeStamp;
					pOutData->Time  = dec->user_data.time_stamp;
					pOutData->UserData = dec->user_data.UserData;
					dec->first_Iframe_flage = 0;
					dec->previous_prediction_type = dec->prediction_type;
					dec->displayFramePre = NULL;
					dec->reference_frame = dec->current_frame;					
					dec->displayFrameProgressive = 1;
				}
			}else{
				Image *tmp;

				if(B_VOP == dec->prediction_type){
					pOutFormat->Format.Type = (VO_VIDEO_FRAMETYPE)(dec->prediction_type-1);
// 					pOutData->Time          =  dec->timeStamp;
					pOutData->Time          =  dec->user_data.time_stamp;
					pOutData->UserData = dec->user_data.UserData;
					buf_seq_ctl(&dec->privFIFO, dec->Bframe, FIFO_WRITE);
					if ((1 == dec->progressive_sequence) || IsMpeg1(dec)){
						dec->displayFrameProgressive = 1;
					}else{
						dec->displayFrameProgressive = dec->cur_progressive_frame;
					}
				}else{
					tmp = dec->reference_frame;
					dec->reference_frame = dec->current_frame;
					dec->current_frame = tmp;

					if (0 == dec->outputMode){
						pOutFormat->Format.Type = (VO_VIDEO_FRAMETYPE)(dec->previous_prediction_type - 1);
						dec->previous_prediction_type = dec->prediction_type;
// 						pOutData->Time = dec->timeStampPre;
// 						dec->timeStampPre = dec->timeStamp;
						pOutData->Time = dec->user_data_pre.time_stamp;
						pOutData->UserData = dec->user_data_pre.UserData;
						dec->user_data_pre = dec->user_data;
						buf_seq_ctl(&dec->privFIFO, dec->displayFramePre, FIFO_WRITE);
						dec->displayFramePre = dec->current_frame;

						if ((1 == dec->progressive_sequence) || IsMpeg1(dec)){
							dec->displayFrameProgressive = 1;
						}else{
							dec->displayFrameProgressive = dec->pre_progressive_frame;
						}
						dec->pre_progressive_frame = dec->cur_progressive_frame;

					}else{
						pOutFormat->Format.Type = (VO_VIDEO_FRAMETYPE)(dec->prediction_type - 1);
						dec->previous_prediction_type = dec->prediction_type;
// 						pOutData->Time = dec->timeStamp;
						pOutData->Time = dec->user_data.time_stamp ;
						pOutData->UserData = dec->user_data.UserData;
						buf_seq_ctl(&dec->privFIFO, dec->displayFramePre, FIFO_WRITE);
						dec->displayFramePre = dec->current_frame;

						if ((1 == dec->progressive_sequence) || IsMpeg1(dec)){
							dec->displayFrameProgressive = 1;
						}else{
							dec->displayFrameProgressive = dec->cur_progressive_frame;
						}
					}

				}

// 				dec->outTimeStamp = pOutData->Time;
				dec->user_data.time_stamp  = pOutData->Time;
				dec->user_data.UserData   = pOutData->UserData ;
			}
	
		}else{
			pOutFormat->Format.Type = VO_VIDEO_FRAME_NULL;
		}
	}

	return Result;
}
