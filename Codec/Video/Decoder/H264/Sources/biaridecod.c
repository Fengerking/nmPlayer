

#if !BUILD_WITHOUT_C_LIB
#include <stdlib.h>
#endif
#include "global.h"
#include "biaridecod.h"
#include "memalloc.h"
#include "vlc.h"
#if FEATURE_CABAC
#if TRACE
extern avdNativeInt symbolCount;
avdNativeInt binCount = 0;
#endif //TRACE

/* Range table for  LPS */ 
const avdUInt8 avd_rLPS_table_64x4[64][4]=
{
	{ 128, 176, 208, 240},
	{ 128, 167, 197, 227},
	{ 128, 158, 187, 216},
	{ 123, 150, 178, 205},
	{ 116, 142, 169, 195},
	{ 111, 135, 160, 185},
	{ 105, 128, 152, 175},
	{ 100, 122, 144, 166},
	{  95, 116, 137, 158},
	{  90, 110, 130, 150},
	{  85, 104, 123, 142},
	{  81,  99, 117, 135},
	{  77,  94, 111, 128},
	{  73,  89, 105, 122},
	{  69,  85, 100, 116},
	{  66,  80,  95, 110},
	{  62,  76,  90, 104},
	{  59,  72,  86,  99},
	{  56,  69,  81,  94},
	{  53,  65,  77,  89},
	{  51,  62,  73,  85},
	{  48,  59,  69,  80},
	{  46,  56,  66,  76},
	{  43,  53,  63,  72},
	{  41,  50,  59,  69},
	{  39,  48,  56,  65},
	{  37,  45,  54,  62},
	{  35,  43,  51,  59},
	{  33,  41,  48,  56},
	{  32,  39,  46,  53},
	{  30,  37,  43,  50},
	{  29,  35,  41,  48},
	{  27,  33,  39,  45},
	{  26,  31,  37,  43},
	{  24,  30,  35,  41},
	{  23,  28,  33,  39},
	{  22,  27,  32,  37},
	{  21,  26,  30,  35},
	{  20,  24,  29,  33},
	{  19,  23,  27,  31},
	{  18,  22,  26,  30},
	{  17,  21,  25,  28},
	{  16,  20,  23,  27},
	{  15,  19,  22,  25},
	{  14,  18,  21,  24},
	{  14,  17,  20,  23},
	{  13,  16,  19,  22},
	{  12,  15,  18,  21},
	{  12,  14,  17,  20},
	{  11,  14,  16,  19},
	{  11,  13,  15,  18},
	{  10,  12,  15,  17},
	{  10,  12,  14,  16},
	{   9,  11,  13,  15},
	{   9,  11,  12,  14},
	{   8,  10,  12,  14},
	{   8,   9,  11,  13},
	{   7,   9,  11,  12},
	{   7,   9,  10,  12},
	{   7,   8,  10,  11},
	{   6,   8,   9,  11},
	{   6,   7,   9,  10},
	{   6,   7,   8,   9},
	{   2,   2,   2,   2}
};



const avdUInt8 avd_AC_next_state_MPS_64[64] =    
{
	1,2,3,4,5,6,7,8,9,10,
	11,12,13,14,15,16,17,18,19,20,
	21,22,23,24,25,26,27,28,29,30,
	31,32,33,34,35,36,37,38,39,40,
	41,42,43,44,45,46,47,48,49,50,
	51,52,53,54,55,56,57,58,59,60,
	61,62,62,63
};
	

const avdUInt8 avd_AC_next_state_LPS_64[64] =    
{
	 0, 0, 1, 2, 2, 4, 4, 5, 6, 7,
	 8, 9, 9,11,11,12,13,13,15,15, 
	 16,16,18,18,19,19,21,21,22,22,
	 23,24,24,25,26,26,27,27,28,29,
	 29,30,30,30,31,32,32,33,33,33,
	 34,34,35,35,35,36,36,36,37,37, 
	 37,38,38,63
};

void StartAriDeco(Bitstream *dep)
{
	avdUInt8 aligned = (dep->bBitToGo & 7);
	FlushBits(dep, aligned);
	dep->Drange = HALF-2;
	dep->Dvalue = (avdUInt16)GetBits(dep, B_BITS - 1);
}

/*!
 ************************************************************************
 * \brief
 *    biari_decode_symbol_eq_prob():
 * \return
 *    the decoded symbol
 ************************************************************************
 */
avdNativeUInt biari_decode_symbol_eq_prob(Bitstream *dep)
{
	/* Shift in next bit and add to value */	
	if ((dep->Dvalue = ((dep->Dvalue <<= 1) | (avdUInt16)GetBits(dep, 1)))
		>= dep->Drange) 
	{
		dep->Dvalue -= dep->Drange;
		return 1;
	}
	return 0;
}

/*!
 ************************************************************************
 * \brief
 *    biari_decode_symbol_final():
 * \return
 *    the decoded symbol
 ************************************************************************
 */
avdNativeUInt biari_decode_final(Bitstream *bs)
{
#if TRACE
	fprintf(p_trace, "%d  0x%04x\n", binCount++, bs->Drange );
#endif
	/*BUG: 9.3.3.2.4 need bs->Drange -= 2, before return 1*/
	if (bs->Dvalue >= bs->Drange - 2)
		return 1;

	bs->Drange -= 2;
	if (bs->Drange < QUARTER){
		avdUInt8 shift = avdNumLeadingZerosPlus1[bs->Drange];
		bs->Drange <<= shift;
		bs->Dvalue = (bs->Dvalue<<shift) | (avdUInt16)GetBits(bs, shift);
	}
	return 0;
}



/*!
 ************************************************************************
 * \brief
 *    Initializes a given context with some pre-defined probability state
 ************************************************************************
 */
void avd_biari_init_context (ImageParameters *img,BiContextTypePtr ctx, const avdInt8* ini)
{
	avdNativeInt pstate;
	pstate = ((ini[0]*(avdNativeInt)img->qp)>>4) + ini[1];
	pstate = min (max ( 1, pstate), 126);
	if ( pstate >= 64 )
	{
		ctx->state  = pstate - 64;
		ctx->MPS    = 1;
	}
	else
	{
		ctx->state  = 63 - pstate;
		ctx->MPS    = 0;
	}
}

#endif//FEATURE_CABAC