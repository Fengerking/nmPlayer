/*
   declaration of constant tables
 */
#ifndef ROM_H
#define ROM_H

#include "config.h"
#include "psy_const.h"
#include "tns_param.h"

/*
  mdct
*/
//extern const Word16 LongWindowSine [FRAME_LEN_LONG];
extern const Word16 ShortWindowSine [FRAME_LEN_SHORT];
extern const Word16 LongWindowKBD [FRAME_LEN_LONG];

extern const int cos1sin1tab1024[128 + 1024];
extern const int sinWindow[128 + 1024];
extern const int kbdWindow[128 + 1024];
extern const int twidTabEven[4*6 + 16*6 + 64*6];
extern const int twidTabOdd[8*6 + 32*6 + 128*6];



extern const int nfftTab[2];
extern const int nfftlog2Tab[2];
extern const int nmdctTab[2];
extern const unsigned char bitrevtab[17 + 129];
extern const int bitrevtabOffset[2];
//extern const unsigned short bitrevtab512[65 + 513];
//extern const int bitirevtabOffset[2];
extern const int cos1sin1tabOffset[2];

//extern const Word16 fftTwiddleTable[LSI_FFT_TWIDDLE_TABLE_SIZE+1];


/*
  form factor
*/
extern const Word32 formfac_sqrttable[96];

/*
  quantizer
*/
extern const Word32 mTab_3_4[512];
extern const Word32 mTab_4_3[512];
/*! $2^{-\frac{n}{16}}$ table */
extern const Word16 pow2tominusNover16[17] ;

extern Word32 specExpMantTableComb_enc[4][14];
extern const UWord8 specExpTableComb_enc[4][14];

extern const Word16 quantBorders[4][4];
//extern const Word16 quantRecon[3][4];
extern const Word16 quantRecon[4][3];

/*
  huffman
*/
extern const UWord16 huff_ltab1_2[3][3][3][3];
extern const UWord16 huff_ltab3_4[3][3][3][3];
extern const UWord16 huff_ltab5_6[9][9];
extern const UWord16 huff_ltab7_8[8][8];
extern const UWord16 huff_ltab9_10[13][13];
extern const UWord16 huff_ltab11[17][17];
extern const UWord16 huff_ltabscf[121];
extern const UWord16 huff_ctab1[3][3][3][3];
extern const UWord16 huff_ctab2[3][3][3][3];
extern const UWord16 huff_ctab3[3][3][3][3];
extern const UWord16 huff_ctab4[3][3][3][3];
extern const UWord16 huff_ctab5[9][9];
extern const UWord16 huff_ctab6[9][9];
extern const UWord16 huff_ctab7[8][8];
extern const UWord16 huff_ctab8[8][8];
extern const UWord16 huff_ctab9[13][13];
extern const UWord16 huff_ctab10[13][13];
extern const UWord16 huff_ctab11[17][17];
extern const UWord32 huff_ctabscf[121];



/*
  misc
*/
extern const int sampRateTab[NUM_SAMPLE_RATES];
extern const UWord8 sfBandTotalShort[NUM_SAMPLE_RATES];
extern const UWord8 sfBandTotalLong[NUM_SAMPLE_RATES];
extern const int sfBandTabShortOffset[NUM_SAMPLE_RATES];
extern const short sfBandTabShort[76];
extern const int sfBandTabLongOffset[NUM_SAMPLE_RATES];
extern const short sfBandTabLong[325];

extern const Word32 m_log2_table[INT_BITS];

/*
  TNS
*/
//extern const TNS_MAX_TAB_ENTRY tnsMaxBandsTab[3];
//extern const TNS_CONFIG_TABULATED  p_8000_mono_long;
//extern const TNS_CONFIG_TABULATED  p_8000_mono_short;
//extern const TNS_CONFIG_TABULATED  p_8000_stereo_long;
//extern const TNS_CONFIG_TABULATED  p_8000_stereo_short;
//extern const TNS_CONFIG_TABULATED  p_16000_mono_long;
//extern const TNS_CONFIG_TABULATED  p_16000_mono_short;
//extern const TNS_CONFIG_TABULATED  p_16000_stereo_long;
//extern const TNS_CONFIG_TABULATED  p_16000_stereo_short;
//extern const TNS_CONFIG_TABULATED  p_24000_mono_long;
//extern const TNS_CONFIG_TABULATED  p_24000_mono_short;
//extern const TNS_CONFIG_TABULATED  p_24000_stereo_long;
//extern const TNS_CONFIG_TABULATED  p_24000_stereo_short;
//extern const TNS_CONFIG_TABULATED  p_32000_mono_long;
//extern const TNS_CONFIG_TABULATED  p_32000_mono_short;
//extern const TNS_CONFIG_TABULATED  p_32000_stereo_long;
//extern const TNS_CONFIG_TABULATED  p_32000_stereo_short;
//extern const TNS_INFO_TAB tnsInfoTab[4];
extern const Word32 tnsCoeff3[8];
extern const Word32 tnsCoeff3Borders[8];
extern const Word32 tnsCoeff4[16];
extern const Word32 tnsCoeff4Borders[16];
extern const Word32 invSBF[24];
extern const Word16 sideInfoTabLong[MAX_SFB_LONG + 1];
extern const Word16 sideInfoTabShort[MAX_SFB_SHORT + 1];
#endif
