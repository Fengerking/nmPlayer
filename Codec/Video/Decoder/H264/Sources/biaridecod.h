


#ifndef _BIARIDECOD_H_
#define _BIARIDECOD_H_


/************************************************************************
 * D e f i n i t i o n s
 ***********************************************************************
 */
#define B_BITS    10	// Number of bits to represent the whole coding interval
#define HALF      (1 << (B_BITS-1))
#define QUARTER   (1 << (B_BITS-2))
extern const avdUInt8 avd_rLPS_table_64x4[64][4];
extern const avdUInt8 avd_AC_next_state_MPS_64[64];    
extern const avdUInt8 avd_AC_next_state_LPS_64[64];    

void avd_biari_init_context (ImageParameters *img,BiContextTypePtr ctx, const avdInt8* ini);
void rescale_cum_freq(BiContextTypePtr bi_ct);
//avdNativeUInt biari_decode_symbol(Bitstream *dep, BiContextTypePtr bi_ct );
avdNativeUInt biari_decode_symbol_eq_prob(Bitstream *dep);
avdNativeUInt biari_decode_final(Bitstream *dep);
void StartAriDeco(Bitstream *currStream);

#endif  // BIARIDECOD_H_

