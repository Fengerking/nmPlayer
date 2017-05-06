#include "h265dec_sbac.h"
#include "h265_decoder.h"
#include "h265dec_utils.h"
#include "h265dec_bits.h"
#include <assert.h>

#ifndef VOARMV7
#define VOH265_NORM_SHIFT_SIZE 512
#endif

#define VOH265_LPS_RANGE_SIZE 256
#define VOH265_MPS_STATE_SIZE 128
#define VOH265_LAST_COEFF_FLAG_OFFSET_8x8_SIZE 64

#define NEW_CABAC 1


#ifndef VOARMV7
static const VO_U8 vo_h265_norm_shift[VOH265_NORM_SHIFT_SIZE] = {
  9,8,7,7,6,6,6,6,5,5,5,5,5,5,5,5,
  4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
#endif

static const VO_U8 vo_h265_lps_range[VOH265_LPS_RANGE_SIZE] = {
  128, 128, 128, 123, 116, 111, 105, 100,  95,  90,  85,  81,  77,  73,  69,  66, 
  62,  59,  56,  53,  51,  48,  46,  43,  41,  39,  37,  35,  33,  32,  30,  29, 
  27,  26,  24,  23,  22,  21,  20,  19,  18,  17,  16,  15,  14,  14,  13,  12, 
  12,  11,  11,  10,  10,   9,   9,   8,   8,   7,   7,   7,   6,   6,   6,   2, 
  176, 167, 158, 150, 142, 135, 128, 122, 116, 110, 104,  99,  94,  89,  85,  80, 
  76,  72,  69,  65,  62,  59,  56,  53,  50,  48,  45,  43,  41,  39,  37,  35, 
  33,  31,  30,  28,  27,  26,  24,  23,  22,  21,  20,  19,  18,  17,  16,  15, 
  14,  14,  13,  12,  12,  11,  11,  10,   9,   9,   9,   8,   8,   7,   7,   2, 
  208, 197, 187, 178, 169, 160, 152, 144, 137, 130, 123, 117, 111, 105, 100,  95, 
  90,  86,  81,  77,  73,  69,  66,  63,  59,  56,  54,  51,  48,  46,  43,  41, 
  39,  37,  35,  33,  32,  30,  29,  27,  26,  25,  23,  22,  21,  20,  19,  18, 
  17,  16,  15,  15,  14,  13,  12,  12,  11,  11,  10,  10,   9,   9,   8,   2, 
  240, 227, 216, 205, 195, 185, 175, 166, 158, 150, 142, 135, 128, 122, 116, 110, 
  104,  99,  94,  89,  85,  80,  76,  72,  69,  65,  62,  59,  56,  53,  50,  48, 
  45,  43,  41,  39,  37,  35,  33,  31,  30,  28,  27,  25,  24,  23,  22,  21, 
  20,  19,  18,  17,  16,  15,  14,  14,  13,  12,  12,  11,  11,  10,   9,   2

};


//derivated from above vo_lps_state and vo_mps_state, however, size is double
static const VO_U8 vo_h265_mlps_state[2*VOH265_MPS_STATE_SIZE] = {
  127, 126,  77,  76,  77,  76,  75,  74,  75,  74,  75,  74,  73,  72,  73,  72, 
  73,  72,  71,  70,  71,  70,  71,  70,  69,  68,  69,  68,  67,  66,  67,  66, 
  67,  66,  65,  64,  65,  64,  63,  62,  61,  60,  61,  60,  61,  60,  59,  58, 
  59,  58,  57,  56,  55,  54,  55,  54,  53,  52,  53,  52,  51,  50,  49,  48, 
  49,  48,  47,  46,  45,  44,  45,  44,  43,  42,  43,  42,  39,  38,  39,  38, 
  37,  36,  37,  36,  33,  32,  33,  32,  31,  30,  31,  30,  27,  26,  27,  26, 
  25,  24,  23,  22,  23,  22,  19,  18,  19,  18,  17,  16,  15,  14,  13,  12, 
  11,  10,   9,   8,   9,   8,   5,   4,   5,   4,   3,   2,   1,   0,   0,   1, 
  2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17, 
  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33, 
  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49, 
  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65, 
  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81, 
  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97, 
  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 
  114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 124, 125, 126, 127  
};

static const VO_U8 vo_h265_last_coeff_flag_offset_8x8[VOH265_LAST_COEFF_FLAG_OFFSET_8x8_SIZE] = {
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
  5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 0    // last 0 is padding data
};
#if NEW_CABAC
void vo_init_cabac_decoder(H265_DEC_CABAC *c, const VO_U8 *buf, int buf_size)
{
  BIT_STREAM *p_bs = &c->bs;
  c->stream_start = c->stream= buf;
  c->stream_end = buf + buf_size;
  UPDATE_CACHE(p_bs);
  c->low_bits =  GetBits(p_bs,8)<<18;
  c->low_bits+=  GetBits(p_bs,8)<<10;
  c->low_bits+= (GetBits(p_bs,8)<<2) + 2;
  UPDATE_CACHE(p_bs);
  c->range= 0x1FE;
#if CABAC_LOG
  printCABACState(c->range, c->low_bits,"vo_init_cabac_decoder");
#endif

}

static void VOINLINE refill2_cabac(H265_DEC_CABAC *c){
  int i, x;
  BIT_STREAM *p_bs = &c->bs;
  x= c->low_bits ^ (c->low_bits-1);

#if defined(VOARMV7) && defined(RVDS)
  __asm
  {
    CLZ i, x>>(CABAC_BITS-1)
      RSB i, i, #30
  }
#elif defined(__GNUC__) &&  defined(VOARMV7)
  __asm__ __volatile__ (
    "clz %0, %1 \n\t"
    "rsb %0, %0, #30"
    :"=r"(i)
    :"r"( x>>(CABAC_BITS-1))
    );
#else
  i= 7 - vo_h265_norm_shift[x>>(CABAC_BITS-1)];
#endif  

  x= -CABAC_MASK;

  x+= (GetByte(p_bs)<<9) + (GetByte(p_bs)<<1);
  UPDATE_CACHE(p_bs);
  c->low_bits += x<<i;

}

static  void VOINLINE refill_cabac(H265_DEC_CABAC *c){
  BIT_STREAM *p_bs = &c->bs;
  c->low_bits+= (GetByte(p_bs)<<9) + (GetByte(p_bs)<<1);
  UPDATE_CACHE(p_bs);
  c->low_bits -= CABAC_MASK;
}

#else
void vo_init_cabac_decoder(H265_DEC_CABAC *c, const VO_U8 *buf, int buf_size){
  c->stream_start=
    c->stream= buf;
  c->stream_end= buf + buf_size;
#if CABAC_BITS == 16
  c->low_bits =  (*c->stream++)<<18;
  c->low_bits+=  (*c->stream++)<<10;
#else
  c->low_bits =  (*c->stream++)<<10;
#endif
  c->low_bits+= ((*c->stream++)<<2) + 2;
  c->range= 0x1FE;
#if CABAC_LOG
	printCABACState(c->range, c->low_bits,"vo_init_cabac_decoder");
#endif

}

static void VOINLINE refill2_cabac(H265_DEC_CABAC *c){
  int i, x;

  x= c->low_bits ^ (c->low_bits-1);

#if defined(VOARMV7) && defined(RVDS)
  __asm
  {
    CLZ i, x>>(CABAC_BITS-1)
      RSB i, i, #30
  }
#elif defined(__GNUC__) &&  defined(VOARMV7)
  __asm__ __volatile__ (
    "clz %0, %1 \n\t"
    "rsb %0, %0, #30"
    :"=r"(i)
    :"r"( x>>(CABAC_BITS-1))
    );
#else
  i= 7 - vo_h265_norm_shift[x>>(CABAC_BITS-1)];
#endif  

  x= -CABAC_MASK;

#if CABAC_BITS == 16
  x+= (c->stream[0]<<9) + (c->stream[1]<<1);
#else
  x+= c->stream[0]<<1;
#endif

  c->low_bits += x<<i;
  c->stream += CABAC_BITS/8;
}


static  void VOINLINE refill_cabac(H265_DEC_CABAC *c){
#if CABAC_BITS == 16
  c->low_bits+= (c->stream[0]<<9) + (c->stream[1]<<1);
#else
  c->low_bits+= c->stream[0]<<1;
#endif
  c->low_bits -= CABAC_MASK;
  c->stream += CABAC_BITS / 8;
}
#endif


static  void VOINLINE renorm_cabac_decoder_once(H265_DEC_CABAC *c){
  VO_S32  shift= (VO_U32)(c->range - 0x100)>>31;
  c->range<<= shift;
  c->low_bits  <<= shift;
  if(!(c->low_bits & CABAC_MASK))
    refill_cabac(c);
}

VO_U32 voCabacDecodeBin( H265_DEC_CABAC *p_cabac,/* BIT_STREAM* p_bs,*/VO_U32 nCtxState )
{ 
  VO_U8 *state = &p_cabac->contextModels[nCtxState];
  VO_S32 s = *state;
  VO_S32 RangeLPS= vo_h265_lps_range[(p_cabac->range&0xC0) + (s>>1)];
  VO_S32 bit, lps_mask;

  p_cabac->range -= RangeLPS;
  lps_mask=  p_cabac->low_bits - (p_cabac->range<<(CABAC_BITS+1));

  if(lps_mask>=0){
    p_cabac->low_bits = lps_mask ;
    p_cabac->range = RangeLPS;
    s^=0xffffffff;
  }

  *state= (vo_h265_mlps_state+128)[s];
  bit= s&1;

#if defined(VOARMV7) && defined(RVDS)
  __asm
  {
    CLZ lps_mask, p_cabac->range
      SUB lps_mask, lps_mask, #23
  }
#elif defined(__GNUC__) &&  defined(VOARMV7)
  __asm__ __volatile__ (
    "clz %0, %1 \n\t"
    "sub %0, %0, #23"
    :"=r"(lps_mask)
    :"r"(p_cabac->range)
    );
#else
  lps_mask= vo_h265_norm_shift[p_cabac->range];
#endif

  p_cabac->range<<= lps_mask;
  p_cabac->low_bits  <<= lps_mask;
  if(!(p_cabac->low_bits & CABAC_MASK))
    refill2_cabac(p_cabac);
#if CABAC_LOG
  printCABACState(p_cabac->range, p_cabac->low_bits,"voCabacDecodeBin");
#endif
   return bit;
}

VO_U32 voCabacDecodeBinEP(H265_DEC_CABAC *p_cabac/*,BIT_STREAM* p_bs*/)
{ 
  int range;
  p_cabac->low_bits += p_cabac->low_bits;

  if(!(p_cabac->low_bits & CABAC_MASK))
    refill_cabac(p_cabac);

  range= p_cabac->range<<(CABAC_BITS+1);
  if(p_cabac->low_bits < range){
#if CABAC_LOG
    printCABACState(p_cabac->range, p_cabac->low_bits,"voCabacDecodeBinEP1");
#endif
    return 0;
  }else{
    p_cabac->low_bits -= range;
#if CABAC_LOG
    printCABACState(p_cabac->range, p_cabac->low_bits,"voCabacDecodeBinEP1");
#endif
    return 1;
  }
}

VO_U32 voCabacDecodeBinsEP(H265_DEC_CABAC *p_cabac,/*BIT_STREAM* p_bs,*/ VO_S32 numBins )
{
  VO_U32 bins = 0;
  VO_S32 i;
  int range;
  range = p_cabac->range << (CABAC_BITS + 1);

  for(i = 0; i < numBins; i++)
  {
    p_cabac->low_bits += p_cabac->low_bits;

    if(!(p_cabac->low_bits & CABAC_MASK))
      refill_cabac(p_cabac);

    bins += bins;
    if(p_cabac->low_bits >= range)
    {
      p_cabac->low_bits -= range;
      bins++;
    }
  }

#if CABAC_LOG
  printCABACState(p_cabac->range, p_cabac->low_bits,"voCabacDecodeBinsEP");
#endif
  return bins;
}
 

VO_U32 voCabacDecodeBinTrm(H265_DEC_CABAC *p_cabac/*, BIT_STREAM* p_bs*/ )
{
   p_cabac->range -= 2;
   if(p_cabac->low_bits < p_cabac->range<<(CABAC_BITS+1)){
     renorm_cabac_decoder_once(p_cabac);
#if CABAC_LOG
     printCABACState(p_cabac->range, p_cabac->low_bits,"voCabacDecodeBinTrm0");
#endif
     return 0;
   }else{
#if CABAC_LOG
     printCABACState(p_cabac->range, p_cabac->low_bits,"voCabacDecodeBinTrm1");
#endif
     return p_cabac->bs.p_bit_ptr - p_cabac->stream_start;
   }
 }

/** Parsing of coeff_abs_level_remaing
 * \param ruiSymbol reference to coeff_abs_level_remaing
 * \param ruiParam reference to parameter
 * \returns Void
 */
 VO_U32 xReadCoefRemainExGolomb ( H265_DEC_CABAC *p_cabac, /*BIT_STREAM* p_bs,*/ VO_U32 rParam )
{

  VO_U32  prefix   = 0;
  VO_U32  codeWord = 0;
  VO_U32 rSymbol;

  do
  {
    prefix++;
    codeWord = voCabacDecodeBinEP(p_cabac/*, p_bs*/);
  }
  while( codeWord);
  codeWord  = 1 - codeWord;
  prefix -= codeWord;
  codeWord=0;
#if COEF_REMAIN_BIN_REDUCTION
  if (prefix < COEF_REMAIN_BIN_REDUCTION )
#else
  if (prefix < 8 )
#endif
  {
	codeWord = voCabacDecodeBinsEP(p_cabac,/*p_bs,*/ rParam);
    rSymbol = (prefix<<(rParam)) + codeWord;
  }
  else
  {
#if COEF_REMAIN_BIN_REDUCTION
	codeWord = voCabacDecodeBinsEP(p_cabac,/*p_bs,*/ prefix-COEF_REMAIN_BIN_REDUCTION+rParam);//@@
    rSymbol = (((1<<(prefix-COEF_REMAIN_BIN_REDUCTION))+COEF_REMAIN_BIN_REDUCTION-1)<<(rParam))+codeWord;
#else
    m_pcTDecBinIf->decodeBinsEP(codeWord,prefix-8+rParam);
    *rSymbol = (((1<<(prefix-8))+8-1)<<rParam)+codeWord;
#endif
  }
  return rSymbol;
}


VO_U32 xReadEpExGolomb( H265_DEC_CABAC *p_cabac,
	/*BIT_STREAM *p_bs,*/
	VO_U32 uiCount )
{
    VO_U32  uiSymbol = 0;

    while ( voCabacDecodeBinEP( p_cabac/*, p_bs*/ ) ) {
		uiSymbol += ( 1 << uiCount );
		uiCount++;
    }

    if ( uiCount ) {
        uiSymbol += voCabacDecodeBinsEP( p_cabac, /*p_bs,*/ uiCount );
    }

    return uiSymbol;
}


VO_U32 xReadUnaryMaxSymbol( H265_DEC_CABAC *p_cabac,
    /*BIT_STREAM *p_bs,*/
	VO_U32 iBase, 
	VO_S32 iOffset, 
	VO_U32 uiMaxSymbol )
{

    VO_U32 uiSymbol = 0;
    VO_U32 uiCont;

 
    if ( !voCabacDecodeBin( p_cabac, /*p_bs ,*/ iBase ) ) {
		return 0;
    }
  
    do {
	    uiCont = voCabacDecodeBin( p_cabac, /*p_bs,*/ iBase + iOffset );
        uiSymbol++;
    } while( uiCont && ( uiSymbol < uiMaxSymbol - 1 ) );
  
    if ( uiCont && ( uiSymbol == uiMaxSymbol - 1 ) ) {
  
        uiSymbol++;
    }
    return uiSymbol;
}




