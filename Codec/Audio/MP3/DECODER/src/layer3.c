#include "config.h"
#include "global.h"
#include "bit.h"
#include "stream.h"
#include "frame.h"
#include "huffman.h"
#include "voMP3.h"

enum {
	count1table_select = 0x01,
	scalefac_scale     = 0x02,
	preflag			   = 0x04,
	mixed_block_flag   = 0x08
};

enum {
	I_STEREO  = 0x1,
	MS_STEREO = 0x2
};

#ifdef ARMV7
void voMP3Decidct9_2(int *x);
#endif
/*
* scalefactor bit lengths
* derived from section 2.4.2.7 of ISO/IEC 11172-3
*/
static struct {
	unsigned char slen1;
	unsigned char slen2;
} const sflen_table[16] = {
	{ 0x0, 0x0 }, { 0x0, 0x1 }, { 0x0, 0x2 }, { 0x0, 0x3 },
	{ 0x3, 0x0 }, { 0x1, 0x1 }, { 0x1, 0x2 }, { 0x1, 0x3 },
	{ 0x2, 0x1 }, { 0x2, 0x2 }, { 0x2, 0x3 }, { 0x3, 0x1 },
	{ 0x3, 0x2 }, { 0x3, 0x3 }, { 0x4, 0x2 }, { 0x4, 0x3 }
};

/*
* number of LSF scalefactor band values
* derived from section 2.4.3.2 of ISO/IEC 13818-3
*/
static unsigned char const nsfb_table[6][3][4] = {
	{ {0x6, 0x5, 0x5, 0x5 },
    {  0x9, 0x9, 0x9, 0x9 },
    {  0x6, 0x9, 0x9, 0x9 } },
	
	{ {0x6, 0x5, 0x7, 0x3 },
    {  0x9, 0x9, 0xc, 0x6 },
    {  0x6, 0x9, 0xc, 0x6 } },
	
	{ {0xb, 0xa,  0x0, 0x0 },
    { 0x12, 0x12, 0x0, 0x0 },
    { 0xf,  0x12, 0x0, 0x0 } },
	
	{ {0x7, 0x7, 0x7, 0x0 },
    { 0xc, 0xc, 0xc, 0x0 },
    { 0x6, 0xf, 0xc, 0x0 } },
	
	{ {0x6, 0x6, 0x6, 0x3 },
    { 0xc, 0x9, 0x9, 0x6 },
    { 0x6, 0xc, 0x9, 0x6 } },
	
	{ {0x8, 0x8, 0x5, 0x0 },
    { 0xf,  0xc, 0x9, 0x0 },
    { 0x6, 0x12, 0x9, 0x0 } }
};

/*
* MPEG-1 scalefactor band widths
* derived from Table B.8 of ISO/IEC 11172-3
*/
static unsigned char const sfband_48000_long[] = {
   0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x6, 0x6, 0x6, 0x8, 0xa,
   0xc, 0x10, 0x12, 0x16, 0x1c, 0x22, 0x28, 0x2e, 0x36, 0x36, 0xc0};

static unsigned char const sfband_44100_long[] = {
   0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x6, 0x6, 0x8, 0x8, 0xa,
   0xc, 0x10, 0x14, 0x18, 0x1c, 0x22, 0x2a, 0x32, 0x36, 0x4c, 0x9e
};

static unsigned char const sfband_32000_long[] = {
   0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x6, 0x6, 0x8, 0xa, 0xc, 
   0x10, 0x14, 0x18, 0x1e, 0x26, 0x2e, 0x38, 0x44, 0x54, 0x66, 0x1a
};

static unsigned char const sfband_48000_short[] = {
   0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x6,
   0x6, 0x6, 0x6, 0x6, 0x6, 0xa, 0xa, 0xa, 0xc, 0xc, 0xc, 0xe, 0xe, 
   0xe, 0x10, 0x10, 0x10, 0x14, 0x14, 0x14, 0x1a, 0x1a, 0x1a, 0x42, 0x42, 0x42
};

static unsigned char const sfband_44100_short[] = {
   0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x6,
   0x6, 0x6, 0x8, 0x8, 0x8, 0xa, 0xa, 0xa, 0xc, 0xc, 0xc, 0xe, 0xe,
   0xe, 0x12, 0x12, 0x12, 0x16, 0x16, 0x16, 0x1e, 0x1e, 0x1e, 0x38, 0x38, 0x38
};

static unsigned char const sfband_32000_short[] = {
   0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x6,
   0x6, 0x6, 0x8, 0x8, 0x8, 0xc, 0xc, 0xc, 0x10, 0x10, 0x10, 0x14, 0x14,
   0x14, 0x1a, 0x1a, 0x1a, 0x22, 0x22, 0x22, 0x2a, 0x2a, 0x2a, 0xc, 0xc, 0xc
};

static unsigned char const sfband_48000_mixed[] = {
	/* long */  0x4, 0x4, 0x4, 0x4, 0x4, 0x4,  0x6,  0x6,
	/* short */ 0x4, 0x4, 0x4,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6, 0xa,
	0xa, 0xa, 0xc, 0xc, 0xc, 0xe, 0xe, 0xe, 0x10, 0x10,
	0x10, 0x14, 0x14, 0x14, 0x1a, 0x1a, 0x1a, 0x42, 0x42, 0x42
};

static unsigned char const sfband_44100_mixed[] = {
	/* long */  0x4, 0x4, 0x4, 0x4, 0x4, 0x4,  0x6,  0x6,
	/* short */ 0x4, 0x4, 0x4,  0x6,  0x6,  0x6,  0x8,  0x8,  0x8, 0xa,
	0xa, 0xa, 0xc, 0xc, 0xc, 0xe, 0xe, 0xe, 0x12, 0x12,
	0x12, 0x16, 0x16, 0x16, 0x1e, 0x1e, 0x1e, 0x38, 0x38, 0x38
};

static unsigned char const sfband_32000_mixed[] = {
	/* long */  0x4, 0x4, 0x4, 0x4, 0x4, 0x4,  0x6,  0x6,
	/* short */ 0x4, 0x4, 0x4,  0x6,  0x6,  0x6,  0x8,  0x8,  0x8, 0xc,
	0xc, 0xc, 0x10, 0x10, 0x10, 0x14, 0x14, 0x14, 0x1a, 0x1a,
	0x1a, 0x22, 0x22, 0x22, 0x2a, 0x2a, 0x2a, 0xc, 0xc, 0xc
};

/*
* MPEG-2 scalefactor band widths
* derived from Table B.2 of ISO/IEC 13818-3
*/
static unsigned char const sfband_24000_long[] = {
	0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x8, 0xa, 0xc,  0xe,  0x10,
	0x12, 0x16, 0x1a, 0x20, 0x26, 0x2e, 0x36, 0x3e, 0x46,  0x4c,  0x24
};

static unsigned char const sfband_22050_long[] = {
	0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x8, 0xa, 0xc,  0xe,  0x10,
	0x14, 0x18, 0x1c, 0x20, 0x26, 0x2e, 0x34, 0x3c, 0x44,  0x3a,  0x36
};

# define sfband_16000_long  sfband_22050_long

static unsigned char const sfband_24000_short[] = {
	0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,  0x6,  0x6,  0x6,  0x8,
	0x8,  0x8, 0xa, 0xa, 0xa, 0xc, 0xc, 0xc, 0xe, 0xe, 0xe, 0x12, 0x12,
	0x12, 0x18, 0x18, 0x18, 0x20, 0x20, 0x20, 0x2c, 0x2c, 0x2c, 0xc, 0xc, 0xc
};

static unsigned char const sfband_22050_short[] = {
	0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,  0x6,  0x6,  0x6,  0x6,
	0x6,  0x6,  0x8,  0x8,  0x8, 0xa, 0xa, 0xa, 0xe, 0xe, 0xe, 0x12, 0x12,
	0x12, 0x1a, 0x1a, 0x1a, 0x20, 0x20, 0x20, 0x2a, 0x2a, 0x2a, 0x12, 0x12, 0x12
};

static unsigned char const sfband_16000_short[] = {
	0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,  0x6,  0x6,  0x6,  0x8,
	0x8,  0x8, 0xa, 0xa, 0xa, 0xc, 0xc, 0xc, 0xe, 0xe, 0xe, 0x12, 0x12,
	0x12, 0x18, 0x18, 0x18, 0x1e, 0x1e, 0x1e, 0x28, 0x28, 0x28, 0x12, 0x12, 0x12
};

static unsigned char const sfband_24000_mixed[] = {
	/* long */   0x6,  0x6,  0x6,  0x6,  0x6,  0x6,
	/* short */  0x6,  0x6,  0x6,  0x8,  0x8,  0x8, 0xa, 0xa, 0xa, 0xc,
	0xc, 0xc, 0xe, 0xe, 0xe, 0x12, 0x12, 0x12, 0x18, 0x18,
	0x18, 0x20, 0x20, 0x20, 0x2c, 0x2c, 0x2c, 0xc, 0xc, 0xc
};

static unsigned char const sfband_22050_mixed[] = {
	/* long */   0x6,  0x6,  0x6,  0x6,  0x6,  0x6,
	/* short */  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x8,  0x8,  0x8, 0xa,
	0xa, 0xa, 0xe, 0xe, 0xe, 0x12, 0x12, 0x12, 0x1a, 0x1a,
	0x1a, 0x20, 0x20, 0x20, 0x2a, 0x2a, 0x2a, 0x12, 0x12, 0x12
};

static unsigned char const sfband_16000_mixed[] = {
	/* long */   0x6,  0x6,  0x6,  0x6,  0x6,  0x6,
	/* short */  0x6,  0x6,  0x6,  0x8,  0x8,  0x8, 0xa, 0xa, 0xa, 0xc,
	0xc, 0xc, 0xe, 0xe, 0xe, 0x12, 0x12, 0x12, 0x18, 0x18,
	0x18, 0x1e, 0x1e, 0x1e, 0x28, 0x28, 0x28, 0x12, 0x12, 0x12
};

/*
* MPEG 2.5 scalefactor band widths
* derived from public sources
*/
# define sfband_12000_long  sfband_16000_long
# define sfband_11025_long  sfband_12000_long

static unsigned char const sfband_8000_long[] = {
	0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0x10, 0x14, 0x18,  0x1c,  0x20,
	0x28, 0x30, 0x38, 0x40, 0x4c, 0x5a,  0x2,  0x2,  0x2,   0x2,   0x2
};

# define sfband_12000_short  sfband_16000_short
# define sfband_11025_short  sfband_12000_short

static
unsigned char const sfband_8000_short[] = {
	0x8,  0x8,  0x8,  0x8,  0x8,  0x8,  0x8,  0x8,  0x8, 0xc, 0xc, 0xc, 0x10,
	0x10, 0x10, 0x14, 0x14, 0x14, 0x18, 0x18, 0x18, 0x1c, 0x1c, 0x1c, 0x24, 0x24,
	0x24,  0x2,  0x2,  0x2,  0x2,  0x2,  0x2,  0x2,  0x2,  0x2, 0x1a, 0x1a, 0x1a
};

# define sfband_12000_mixed  sfband_16000_mixed
# define sfband_11025_mixed  sfband_12000_mixed

/* the 8000 Hz short block scalefactor bands do not break after
the first 36 frequency lines, so this is probably wrong */
static
unsigned char const sfband_8000_mixed[] = {
	/* long */  0xc, 0xc, 0xc,
	/* short */ 0x4, 0x4, 0x4,  0x8,  0x8,  0x8, 0xc, 0xc, 0xc, 0x10, 0x10, 0x10,
	0x14, 0x14, 0x14, 0x18, 0x18, 0x18, 0x1c, 0x1c, 0x1c, 0x24, 0x24, 0x24,
	0x2,  0x2,  0x2,  0x2,  0x2,  0x2,  0x2,  0x2,  0x2, 0x1a, 0x1a, 0x1a
};

static
struct {
	unsigned char const *l;
	unsigned char const *s;
	unsigned char const *m;
} const sfbwidth_table[9] = {
	{ sfband_44100_long, sfband_44100_short, sfband_44100_mixed },
	{ sfband_48000_long, sfband_48000_short, sfband_48000_mixed },
	{ sfband_32000_long, sfband_32000_short, sfband_32000_mixed },
	{ sfband_22050_long, sfband_22050_short, sfband_22050_mixed },
	{ sfband_24000_long, sfband_24000_short, sfband_24000_mixed },
	{ sfband_16000_long, sfband_16000_short, sfband_16000_mixed },
	{ sfband_11025_long, sfband_11025_short, sfband_11025_mixed },
	{ sfband_12000_long, sfband_12000_short, sfband_12000_mixed },
	{  sfband_8000_long,  sfband_8000_short,  sfband_8000_mixed }
};

/*
* scalefactor band preemphasis (used only when preflag is set)
* derived from Table B.6 of ISO/IEC 11172-3
*/
static
unsigned char const pretab[22] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3, 2, 0
};

/*
* fractional powers of two
* used for requantization and joint stereo decoding
*
* root_table[3 + x] = 2^(x/4)
*/
static
int const root_table[7] = {
	(0x09837f05*4) /* 2^(-3/4) == 0.59460355750136 */,
	(0x0b504f33*4) /* 2^(-2/4) == 0.70710678118655 */,
	(0x0d744fcd*4) /* 2^(-1/4) == 0.84089641525371 */,
	(0x10000000*4) /* 2^( 0/4) == 1.00000000000000 */,
	(0x1306fe0a*4) /* 2^(+1/4) == 1.18920711500272 */,
	(0x16a09e66*4) /* 2^(+2/4) == 1.41421356237310 */,
	(0x1ae89f99*4) /* 2^(+3/4) == 1.68179283050743 */
};

static
int const imdctWin[4][36] = {
	{
	0x02aace8b, 0x07311c28, 0x0a868fec, 0x0c913b52, 0x0d413ccd, 0x0c913b52, 0x0a868fec, 0x07311c28, 
	0x02aace8b, 0xfd16d8dd, 0xf6a09e66, 0xef7a6275, 0xe7dbc161, 0xe0000000, 0xd8243e9f, 0xd0859d8b, 
	0xc95f619a, 0xc2e92723, 0xbd553175, 0xb8cee3d8, 0xb5797014, 0xb36ec4ae, 0xb2bec333, 0xb36ec4ae, 
	0xb5797014, 0xb8cee3d8, 0xbd553175, 0xc2e92723, 0xc95f619a, 0xd0859d8b, 0xd8243e9f, 0xe0000000, 
	0xe7dbc161, 0xef7a6275, 0xf6a09e66, 0xfd16d8dd, 
	},
	{
	0x02aace8b, 0x07311c28, 0x0a868fec, 0x0c913b52, 0x0d413ccd, 0x0c913b52, 0x0a868fec, 0x07311c28, 
	0x02aace8b, 0xfd16d8dd, 0xf6a09e66, 0xef7a6275, 0xe7dbc161, 0xe0000000, 0xd8243e9f, 0xd0859d8b, 
	0xc95f619a, 0xc2e92723, 0xbd44ef14, 0xb831a052, 0xb3aa3837, 0xafb789a4, 0xac6145bb, 0xa9adecdc, 
	0xa864491f, 0xad1868f0, 0xb8431f49, 0xc8f42236, 0xdda8e6b1, 0xf47755dc, 0x00000000, 0x00000000, 
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 
	},
	{
	0x07311c28, 0x0d413ccd, 0x07311c28, 0xf6a09e66, 0xe0000000, 0xc95f619a, 0xb8cee3d8, 0xb2bec333, 
	0xb8cee3d8, 0xc95f619a, 0xe0000000, 0xf6a09e66, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 
	},
	{
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x028e9709, 0x04855ec0, 
	0x026743a1, 0xfcde2c10, 0xf515dc82, 0xec93e53b, 0xe4c880f8, 0xdd5d0b08, 0xd63510b7, 0xcf5e834a, 
	0xc8e6b562, 0xc2da4105, 0xbd553175, 0xb8cee3d8, 0xb5797014, 0xb36ec4ae, 0xb2bec333, 0xb36ec4ae, 
	0xb5797014, 0xb8cee3d8, 0xbd553175, 0xc2e92723, 0xc95f619a, 0xd0859d8b, 0xd8243e9f, 0xe0000000, 
	0xe7dbc161, 0xef7a6275, 0xf6a09e66, 0xfd16d8dd, 
	},
};

#ifdef ARM
const int  csa_table[8][4] = {
	{0x36e12a00, 0xdf128041, 0x15f3aa41, 0xa8315641},	
	{0x386e7600, 0xe1cf24a1, 0x1a3d9aa1, 0xa960aea1},	
	{0x3cc6b740, 0xebf19fa1, 0x28b856e1, 0xaf2ae861},	
	{0x3eeea040, 0xf45b88c1, 0x334a2901, 0xb56ce881},	
	{0x3fb69040, 0xf9f27f19, 0x39a90f59, 0xba3beed9},	
	{0x3ff23f40, 0xfd60d1e5, 0x3d531125, 0xbd6e92a5},	
	{0x3ffe5940, 0xff175ee4, 0x3f15b824, 0xbf1905a4},	
	{0x3fffe340, 0xffc3612f, 0x3fc3446f, 0xbfc37def}
};

const int IDCT9_D[5] = {
	0x6ed9eba1,
    0x620dbe8b,
    0x163a1a7e,
    0x5246dd49,
    0x7e0e2e32,
};
#endif

#ifndef ARM
static 
int const csa_table[8][4] = {
	{0x36e12a00, 0xdf128041, 0x15f3aa41, 0xa8315641}, 
	{0x386e7600, 0xe1cf24a1, 0x1a3d9aa1, 0xa960aea1}, 
	{0x3cc6b740, 0xebf19fa1, 0x28b856e1, 0xaf2ae861}, 
	{0x3eeea040, 0xf45b88c1, 0x334a2901, 0xb56ce881}, 
	{0x3fb69040, 0xf9f27f19, 0x39a90f59, 0xba3beed9}, 
	{0x3ff23f40, 0xfd60d1e5, 0x3d531125, 0xbd6e92a5}, 
	{0x3ffe5940, 0xff175ee4, 0x3f15b824, 0xbf1905a4}, 
	{0x3fffe340, 0xffc3612f, 0x3fc3446f, 0xbfc37def}
};
#endif

/*
* coefficients for intensity stereo processing
* derived from section 2.4.3.4.9.3 of ISO/IEC 11172-3
*
* is_ratio[i] = tan(i * (PI / 12))
* is_table[i] = is_ratio[i] / (1 + is_ratio[i])
*/
static
int const is_table[7] = {
	0x00000000*4 /* 0.000000000 */,
	0x0361962f*4 /* 0.211324865 */,
	0x05db3d74*4 /* 0.366025404 */,
	0x08000000*4 /* 0.500000000 */,
	0x0a24c28c*4 /* 0.633974596 */,
	0x0c9e69d1*4 /* 0.788675135 */,
	0x10000000*4 /* 1.000000000 */
};

/*
* coefficients for LSF intensity stereo processing
* derived from section 2.4.3.2 of ISO/IEC 13818-3
*
* is_lsf_table[0][i] = (1 / sqrt(sqrt(2)))^(i + 1)
* is_lsf_table[1][i] = (1 /      sqrt(2)) ^(i + 1)
*/
static
int const is_lsf_table[2][15] = {
	{
		(0x0d744fcd*4) /* 0.840896415 */,
		(0x0b504f33*4) /* 0.707106781 */,
		(0x09837f05*4) /* 0.594603558 */,
		(0x08000000*4) /* 0.500000000 */,
		(0x06ba27e6*4) /* 0.420448208 */,
		(0x05a8279a*4) /* 0.353553391 */,
		(0x04c1bf83*4) /* 0.297301779 */,
		(0x04000000*4) /* 0.250000000 */,
		(0x035d13f3*4) /* 0.210224104 */,
		(0x02d413cd*4) /* 0.176776695 */,
		(0x0260dfc1*4) /* 0.148650889 */,
		(0x02000000*4) /* 0.125000000 */,
		(0x01ae89fa*4) /* 0.105112052 */,
		(0x016a09e6*4) /* 0.088388348 */,
		(0x01306fe1*4) /* 0.074325445 */
	}, {
		(0x0b504f33*4) /* 0.707106781 */,
		(0x08000000*4) /* 0.500000000 */,
		(0x05a8279a*4) /* 0.353553391 */,
		(0x04000000*4) /* 0.250000000 */,
		(0x02d413cd*4) /* 0.176776695 */,
		(0x02000000*4) /* 0.125000000 */,
		(0x016a09e6*4) /* 0.088388348 */,
		(0x01000000*4) /* 0.062500000 */,
		(0x00b504f3*4) /* 0.044194174 */,
		(0x00800000*4) /* 0.031250000 */,
		(0x005a827a*4) /* 0.022097087 */,
		(0x00400000*4) /* 0.015625000 */,
		(0x002d413d*4) /* 0.011048543 */,
		(0x00200000*4) /* 0.007812500 */,
		(0x0016a09e*4) /* 0.005524272 */
		}
};

static void voMP3DecUnpackSideInfo(FrameDataInfo *frame, FrameStream *stream)
{
	int gr, ch, nch;
	Bitstream bitStream, *bsi;
	FrameHeader *header;
	SideInfo *si;
	SideInfoSub *sis;

	header = &frame->header;
	si = &frame->sideInfoPS;
	nch = header->channels;	
	bsi = &bitStream;	

	voMP3DecInitBits(bsi, stream->this_frame + header->headLen, frame->siLen);


	if (header->version == MPEG1)
	{
		/* MPEG 1 */
		si->mainDataBegin = voMP3DecGetBits(bsi, 9);
		si->privateBits =   voMP3DecGetBits(bsi, (header->mode == MPA_MODE_SINGLE_CHANNEL ? 5 : 3));

		for (ch = 0; ch < nch; ch++)
			si->scfSi[ch] = voMP3DecGetBits(bsi, 4);
	}
	else 
	{
		/* MPEG 2, MPEG 2.5 */
		si->mainDataBegin = voMP3DecGetBits(bsi, 8);
		si->privateBits =   voMP3DecGetBits(bsi, (header->mode == MPA_MODE_SINGLE_CHANNEL ? 1 : 2));
	}

	for(gr =0; gr < frame->nGrans; gr++) 
	{
		for (ch = 0; ch < nch; ch++) 
		{
			sis = &si->sis[gr][ch];						

			sis->part23Length =    voMP3DecGetBits(bsi, 12);
			sis->nBigvals =        voMP3DecGetBits(bsi, 9);
			sis->globalGain =      voMP3DecGetBits(bsi, 8);
			sis->sfCompress =      voMP3DecGetBits(bsi, (header->version == MPEG1 ? 4 : 9));
			sis->winSwitchFlag =   voMP3DecGetBits(bsi, 1);

			if(sis->winSwitchFlag)
			{
				sis->blockType =       voMP3DecGetBits(bsi, 2);		/* 0 = normal, 1 = start, 2 = short, 3 = stop */
				sis->mixedBlock =      voMP3DecGetBits(bsi, 1);		/* 0 = not mixed, 1 = mixed */
				sis->tableSelect[0] =  voMP3DecGetBits(bsi, 5);
				sis->tableSelect[1] =  voMP3DecGetBits(bsi, 5);
				sis->tableSelect[2] =  0;					
				sis->subBlockGain[0] = voMP3DecGetBits(bsi, 3);
				sis->subBlockGain[1] = voMP3DecGetBits(bsi, 3);
				sis->subBlockGain[2] = voMP3DecGetBits(bsi, 3);

				if (sis->blockType == 0) 
				{
					sis->nBigvals = 0;
					sis->part23Length = 0;
					sis->sfCompress = 0;
				} 
				else if (sis->blockType == 2 && sis->mixedBlock == 0) 
				{
					/* short block, not mixed */
					sis->region0Count = 8;
				} 
				else 
				{
					/* start, stop, or short-mixed */
					sis->region0Count = 7;
				}
				sis->region1Count = 36;
			} 
			else 
			{
				sis->blockType = 0;
				sis->mixedBlock = 0;
				sis->tableSelect[0] =  voMP3DecGetBits(bsi, 5);
				sis->tableSelect[1] =  voMP3DecGetBits(bsi, 5);
				sis->tableSelect[2] =  voMP3DecGetBits(bsi, 5);
				sis->region0Count =    voMP3DecGetBits(bsi, 4);
				sis->region1Count =    voMP3DecGetBits(bsi, 3);
			}
			sis->preFlag =           (header->version == MPEG1 ? voMP3DecGetBits(bsi, 1) : 0);
			sis->sfactScale =        voMP3DecGetBits(bsi, 1);
			sis->count1TableSelect = voMP3DecGetBits(bsi, 1);
		}
	}
}

static unsigned int voMP3DecUnpackScaleFactorsLsf(Bitstream* bitptr,
							                      SideInfoSub *sisSub,
							                      SideInfoSub *gr1ch, 
							                      int mode_extension)
{
	Bitstream start;
	unsigned int scalefactor_compress, index, scalefac_len[4], part, n, i;
	unsigned char const *nsfb;
	int preFlag = 0;
	
	start = *bitptr;
	
	scalefactor_compress = sisSub->sfCompress;
	index = (sisSub->blockType == 2) ?
		((sisSub->mixedBlock) ? 2 : 1) : 0;
	
	if (!((mode_extension & I_STEREO) && gr1ch)) 
	{
		if (scalefactor_compress < 400)
		{
			scalefac_len[0] = (scalefactor_compress >> 4) / 5;
			scalefac_len[1] = (scalefactor_compress >> 4) % 5;
			scalefac_len[2] = (scalefactor_compress & 15) >> 2;
			scalefac_len[3] =  scalefactor_compress & 3;
			
			nsfb = nsfb_table[0][index];
		}
		else if (scalefactor_compress < 500)
		{
			scalefactor_compress -= 400;
			
			scalefac_len[0] = (scalefactor_compress >> 2) / 5;
			scalefac_len[1] = (scalefactor_compress >> 2) % 5;
			scalefac_len[2] =  scalefactor_compress & 3;
			scalefac_len[3] = 0;
			
			nsfb = nsfb_table[1][index];
		}
		else
		{
			scalefactor_compress -= 500;
			
			scalefac_len[0] = scalefactor_compress / 3;
			scalefac_len[1] = scalefactor_compress % 3;
			scalefac_len[2] = 0;
			scalefac_len[3] = 0;
			
			preFlag = 1;
			
			nsfb = nsfb_table[2][index];
		}
		
		n = 0;
		for (part = 0; part < 4; ++part)
		{
			for (i = 0; i < nsfb[part]; ++i)
				sisSub->scalefac[n++] = voMP3DecGetBits(bitptr, scalefac_len[part]);
		}
		
		while (n < 39)
			sisSub->scalefac[n++] = 0;
	}
	else 
	{  /* (mode_extension & I_STEREO) && gr1ch (i.e. ch == 1) */
		scalefactor_compress >>= 1;
		
		if (scalefactor_compress < 180) 
		{
			scalefac_len[0] =  scalefactor_compress / 36;
			scalefac_len[1] = (scalefactor_compress % 36) / 6;
			scalefac_len[2] = (scalefactor_compress % 36) % 6;
			scalefac_len[3] = 0;
			
			nsfb = nsfb_table[3][index];
		}
		else if (scalefactor_compress < 244) 
		{
			scalefactor_compress -= 180;
			
			scalefac_len[0] = (scalefactor_compress & 63) >> 4;
			scalefac_len[1] = (scalefactor_compress & 15) >> 2;
			scalefac_len[2] =  scalefactor_compress &  3;
			scalefac_len[3] = 0;
			
			nsfb = nsfb_table[4][index];
		}
		else
		{
			scalefactor_compress -= 244;
			
			scalefac_len[0] = scalefactor_compress / 3;
			scalefac_len[1] = scalefactor_compress % 3;
			scalefac_len[2] = 0;
			scalefac_len[3] = 0;
			
			nsfb = nsfb_table[5][index];
		}
		
		n = 0;
		for (part = 0; part < 4; ++part) 
		{
			unsigned int max_index, is_pos;
			
			max_index = (1 << scalefac_len[part]) - 1;
			
			for (i = 0; i < nsfb[part]; ++i) 
			{
				is_pos = voMP3DecGetBits(bitptr, scalefac_len[part]);
				
				sisSub->scalefac[n] = is_pos;
				gr1ch->scalefac[n++] = (is_pos == max_index);
			}
		}
		
		while (n < 39)
		{
			sisSub->scalefac[n] = 0;
			gr1ch->scalefac[n++] = 0;  /* apparently not illegal */
		}
	}

	sisSub->preFlag = preFlag;
	
	return voMP3DecCalcBitsUsed(bitptr, &start);
}

static unsigned int voMP3DecUnpackScaleFactors(Bitstream* bitptr,  
						                       SideInfoSub *sisSub,
						                       SideInfoSub *gr1ch, 
						                       int scfsi)
{
	Bitstream start;
	unsigned int slen1, slen2, sfb_i;
	unsigned char  *scalefac;
	
	start = *bitptr;
	scalefac = sisSub->scalefac;
	
	slen1 = sflen_table[sisSub->sfCompress].slen1;
	slen2 = sflen_table[sisSub->sfCompress].slen2;
	
	if (sisSub->blockType == 2)
	{
		unsigned int nsfb;		
		sfb_i = 0;		
		nsfb = (sisSub->mixedBlock) ? 8 + 3 * 3 : 6 * 3;
		while (nsfb--)
			*scalefac++ = voMP3DecGetBits(bitptr, slen1);
		
		nsfb = 6 * 3;
		while (nsfb--)
			*scalefac++ = voMP3DecGetBits(bitptr, slen2);
		
		nsfb = 1 * 3;
		while (nsfb--)
			*scalefac++ = 0;		
	}
	else 
	{  /* channel->block_type != 2 */
		if (scfsi & 0x8) 
		{
			for (sfb_i = 0; sfb_i < 6; ++sfb_i)
				*scalefac++ = gr1ch->scalefac[sfb_i];
		}
		else 
		{
			for (sfb_i = 0; sfb_i < 6; ++sfb_i)
				*scalefac++ = voMP3DecGetBits(bitptr, slen1);
		}
		
		if (scfsi & 0x4)
		{
			for (sfb_i = 6; sfb_i < 11; ++sfb_i)
				*scalefac++ = gr1ch->scalefac[sfb_i];
		}
		else 
		{
			for (sfb_i = 6; sfb_i < 11; ++sfb_i)
				*scalefac++ = voMP3DecGetBits(bitptr, slen1);
		}
		
		if (scfsi & 0x2)
		{
			for (sfb_i = 11; sfb_i < 16; ++sfb_i)
				*scalefac++ = gr1ch->scalefac[sfb_i];
		}
		else
		{
			for (sfb_i = 11; sfb_i < 16; ++sfb_i)
				*scalefac++ = voMP3DecGetBits(bitptr, slen2);
		}
		
		if (scfsi & 0x1)
		{
			for (sfb_i = 16; sfb_i < 21; ++sfb_i)
				*scalefac++ = gr1ch->scalefac[sfb_i];
		}
		else
		{
			for (sfb_i = 16; sfb_i < 21; ++sfb_i)
				*scalefac++ = voMP3DecGetBits(bitptr, slen2);
		}
		
		*scalefac++ = 0;
	}
	
	return voMP3DecCalcBitsUsed(bitptr, &start);
}

static void voMP3DecExponents(SideInfoSub *sisSub,
			                  unsigned char const *sfb_width,
			                  int exponents[39], 
			                  int modeExt
							  )
{
	int gain;
	unsigned int scalefac_multiplier, sfb_i;
	
	gain =  sisSub->globalGain - 210;
	scalefac_multiplier = (sisSub->sfactScale) ? 2 : 1;
	if(modeExt >> 1)
		gain -= 2;
	gain += 2;
	
	if (sisSub->blockType == 2) 
	{
		unsigned int l;
		int gain0, gain1, gain2;
		
		sfb_i = l = 0;
		
		if (sisSub->mixedBlock) 
		{
			unsigned int premask;
			
			premask = (sisSub->preFlag) ? ~0 : 0;
			
			while (l < 36)
			{
				exponents[sfb_i] = gain -
					(signed int) ((sisSub->scalefac[sfb_i] + (pretab[sfb_i] & premask)) <<
					scalefac_multiplier);
				
				l += sfb_width[sfb_i++];
			}
		}
		
		gain0 = gain - 8 * sisSub->subBlockGain[0];
		gain1 = gain - 8 * sisSub->subBlockGain[1];
		gain2 = gain - 8 * sisSub->subBlockGain[2];
		
		while (l < 576) 
		{
			exponents[sfb_i + 0] = gain0 -
				(sisSub->scalefac[sfb_i + 0] << scalefac_multiplier);
			exponents[sfb_i + 1] = gain1 -
				(sisSub->scalefac[sfb_i + 1] << scalefac_multiplier);
			exponents[sfb_i + 2] = gain2 -
				(sisSub->scalefac[sfb_i + 2] << scalefac_multiplier);
			
			l    += 3 * sfb_width[sfb_i];
			sfb_i += 3;
		}
	}
	else
	{  
		if (sisSub->preFlag) 
		{
			for (sfb_i = 0; sfb_i < 22; ++sfb_i)
			{
				exponents[sfb_i] = gain -
					((sisSub->scalefac[sfb_i] + pretab[sfb_i]) << scalefac_multiplier);
			}
		}
		else
		{
			for (sfb_i = 0; sfb_i < 22; ++sfb_i) 
			{
				exponents[sfb_i] = gain -
					(sisSub->scalefac[sfb_i] << scalefac_multiplier);
			}
		}
	}
}

static int III_dequantize_samples(unsigned int value, int exp)
{
	int requantized;
	signed int frac;
	fixedfloat const *power;
	
	frac = exp & 3;  
	exp >>= 2;
	
	power = &voMP3DecRqTab[value];
	requantized = power->mantissa;
	exp += power->exponent - 3;
	
	if (exp < 0) 
	{
		if (-exp >= sizeof(int) * 8) 
		{
			requantized = 0;
		}
		else 
		{
			requantized += 1L << (-exp - 1);
			requantized >>= -exp;
		}
	}
	else 
	{
		if (exp >= 5) 
		{
			requantized = MAX_32;
		}
		else
			requantized <<= exp;
	}
	
	return frac ? (MUL_32(requantized, root_table[3 + frac]) << 2) : requantized;
}

# define MASK(cache, sz, bits)	\
(((cache) >> ((sz) - (bits))) & ((1 << (bits)) - 1))
# define MASK1BIT(cache, sz)  \
((cache) & (1 << ((sz) - 1)))

/*
* NAME:	III_huffdecode()
* DESCRIPTION:	decode Huffman code words of one channel of one granule
*/
static int voMP3DecHuffDecode(Bitstream* bit_ptr, 
					          int *xr,
					          SideInfoSub *sisSub,
					          unsigned char const *sfb_width,
					          unsigned int part2_length, 
					          unsigned int Modeext,
					          int *exponents)
{
	int exp;
	int const *expptr;
	int bits_left, cache_sz;
	unsigned char *hbuf;
	register int *pXr;
	int const *sfbound;
	unsigned int bit_cache, nzero;
	
	bits_left = sisSub->part23Length - part2_length;
	if (bits_left < 0)
		return -1;
	
	voMP3DecExponents(sisSub, sfb_width, exponents, Modeext);
	
	if(bit_ptr->bitsleft < bits_left)
	{
		cache_sz  = bit_ptr->bitsleft;
		bit_cache   = voMP3DecGetBits(bit_ptr, cache_sz);
		bits_left -= cache_sz;
		hbuf =  bit_ptr->byte;
	}
	else
	{
		cache_sz  = bits_left;
		bit_cache   = voMP3DecGetBits(bit_ptr, bits_left);
		bits_left -= cache_sz;
		hbuf =  bit_ptr->byte;
	}

	
	if(voMP3DecSkipBits(bit_ptr, bits_left) < bits_left)
		return -1;
	
	while((31 - cache_sz) > 8) 
	{
		bit_cache   = (bit_cache << 8) | (*hbuf++);
		cache_sz   += 8;
		bits_left -= 8;		
	}
	
	pXr = &xr[0];
	
	/* big_values */
	{
		unsigned int region, rcount;
		struct huff_table const *entry;
		union huff_pair const *table;
		unsigned int linBits, startBits, big_values, reqhits_value;
		int reqcache[16];
		
		sfbound = pXr + *sfb_width++;
		rcount  = sisSub->region0Count + 1;
		
		entry     = &huff_pair_table[sisSub->tableSelect[region = 0]];
		table     = entry->pTable;
		linBits   = entry->linBits;
		startBits = entry->startBits;
		
		if (table == 0)
			return -1;
		
		expptr  = &exponents[0];
		exp     = *expptr++;
		reqhits_value = 0;
		
		big_values = sisSub->nBigvals;
		
		while (big_values-- && cache_sz + bits_left > 0) 
		{
			union huff_pair const *pPair;
			unsigned int clumpsz, value;
			int requantized_value;
			
			if (pXr == sfbound) 
			{
				
				if(pXr > &xr[574])
					return -1;

				sfbound += *sfb_width++;
				
				if (--rcount == 0) 
				{
					if (region == 0)
						rcount = sisSub->region1Count + 1;
					else
						rcount = 0;  /* all remaining */
					
					entry     = &huff_pair_table[sisSub->tableSelect[++region]];
					table     = entry->pTable;
					linBits   = entry->linBits;
					startBits = entry->startBits;
					
					if (table == 0)
						return -1;
				}
				
				if (exp != *expptr) 
				{
					exp = *expptr;
					reqhits_value = 0;
				}
				
				++expptr;				
			}
			
			if (cache_sz < 21) 
			{
				while((31 - cache_sz) > 8) 
				{
					bit_cache   = (bit_cache << 8) | (*hbuf++);
					cache_sz   += 8;
					bits_left -= 8;					
				}
			}
			
			clumpsz = startBits;
			pPair    = &table[MASK(bit_cache, cache_sz, clumpsz)];
			
			while (!pPair->finalB)
			{
				cache_sz -= clumpsz;
				
				clumpsz = pPair->ptr.bits;
				pPair    = &table[pPair->ptr.offset + MASK(bit_cache, cache_sz, clumpsz)];
			}
			
			cache_sz -= pPair->value.hLen;
			
			if (linBits) 
			{
				value = pPair->value.xHuff;
				
				switch (value) 
				{
				case 0:
					pXr[0] = 0;
					break;
					
				case 15:
					if (cache_sz < linBits + 2) 
					{
						bit_cache   = (bit_cache << 8) | (*hbuf++);
						bit_cache   = (bit_cache << 8) | (*hbuf++);
						cache_sz   += 16;
						bits_left -= 16;
					}
					
					value += MASK(bit_cache, cache_sz, linBits);
					cache_sz -= linBits;
					
					requantized_value = III_dequantize_samples(value, exp);
					goto x_final;
					
				default:
					if (reqhits_value & (1 << value))
						requantized_value = reqcache[value];
					else 
					{
						reqhits_value |= (1 << value);
						requantized_value = reqcache[value] = III_dequantize_samples(value, exp);
					}
					
x_final:
					pXr[0] = MASK1BIT(bit_cache, cache_sz--) ?
						-requantized_value : requantized_value;
				}
				
				/* y (0..14) */				
				value = pPair->value.yHuff;
				
				switch (value) 
				{
				case 0:
					pXr[1] = 0;
					break;
					
				case 15:
					if (cache_sz < linBits + 1) 
					{
						bit_cache   = (bit_cache << 8) | (*hbuf++);
						bit_cache   = (bit_cache << 8) | (*hbuf++);
						cache_sz   += 16;
						bits_left -= 16;
					}
					
					value += MASK(bit_cache, cache_sz, linBits);
					cache_sz -= linBits;
					
					requantized_value = III_dequantize_samples(value, exp);
					goto y_final;
					
				default:
					if (reqhits_value & (1 << value))
						requantized_value = reqcache[value];
					else 
					{
						reqhits_value |= (1 << value);
						requantized_value = reqcache[value] = III_dequantize_samples(value, exp);
					}
					
y_final:
					pXr[1] = MASK1BIT(bit_cache, cache_sz--) ?
						-requantized_value : requantized_value;
				}
			}
			else 
			{
				/* x (0..1) */
				
				value = pPair->value.xHuff;
				
				if (value == 0)
					pXr[0] = 0;
				else 
				{
					if (reqhits_value & (1 << value))
						requantized_value = reqcache[value];
					else 
					{
						reqhits_value |= (1 << value);
						requantized_value = reqcache[value] = III_dequantize_samples(value, exp);
					}
					
					pXr[0] = MASK1BIT(bit_cache, cache_sz--) ?
						-requantized_value : requantized_value;
				}
				
				/* y (0..1) */				
				value = pPair->value.yHuff;
				
				if (value == 0)
					pXr[1] = 0;
				else 
				{
					if (reqhits_value & (1 << value))
						requantized_value = reqcache[value];
					else 
					{
						reqhits_value |= (1 << value);
						requantized_value = reqcache[value] = III_dequantize_samples(value, exp);
					}
					
					pXr[1] = MASK1BIT(bit_cache, cache_sz--) ?
						-requantized_value : requantized_value;
				}
			}
			
			pXr += 2;
    }
  }
  
  if (cache_sz + bits_left < 0)
  {
	 if(pXr >= xr + 2)
		pXr -= 2;
  }  
  
  /* count1 */
  {
	  union huff_quad const *table;
	  register int requantized;
	  
	  table = pHuff_quad_table[sisSub->count1TableSelect];
	  
	  requantized = III_dequantize_samples(1, exp);
	  
	  while (cache_sz + bits_left > 0 && pXr <= &xr[572]) 
	  {
		  union huff_quad const *quad;
		  
		  /* hcod (1..6) */
		  
		  if (cache_sz < 10) 
		  {
			  bit_cache   = (bit_cache << 8) | (*hbuf++);
			  bit_cache   = (bit_cache << 8) | (*hbuf++);
			  cache_sz   += 16;
			  bits_left -= 16;
		  }
		  
		  quad = &table[MASK(bit_cache, cache_sz, 4)];
		  
		  if (!quad->finalB) 
		  {
			  cache_sz -= 4;
			  
			  quad = &table[quad->ptr.offset +
				  MASK(bit_cache, cache_sz, quad->ptr.bits)];
		  }
		  
		  cache_sz -= quad->value.hLen;
		  
		  if (pXr == sfbound) 
		  {
			  sfbound += *sfb_width++;
			  
			  if (exp != *expptr) 
			  {
				  exp = *expptr;
				  requantized = III_dequantize_samples(1, exp);
			  }
			  
			  ++expptr;
		  }
		  
		  /* v (0..1) */		  
		  pXr[0] = quad->value.vHuff ?
			  (MASK1BIT(bit_cache, cache_sz--) ? -requantized : requantized) : 0;
		  
		  /* w (0..1) */		  
		  pXr[1] = quad->value.wHuff ?
			  (MASK1BIT(bit_cache, cache_sz--) ? -requantized : requantized) : 0;
		  
		  pXr += 2;
		  
		  if (pXr == sfbound)
		  {
			  sfbound += *sfb_width++;
			  
			  if (exp != *expptr) 
			  {
				  exp = *expptr;
				  requantized = III_dequantize_samples(1, exp);
			  }
			  
			  ++expptr;
		  }
		  
		  /* x (0..1) */		  
		  pXr[0] = quad->value.xHuff ?
			  (MASK1BIT(bit_cache, cache_sz--) ? -requantized : requantized) : 0;
		  
		  /* y (0..1) */		  
		  pXr[1] = quad->value.yHuff ?
			  (MASK1BIT(bit_cache, cache_sz--) ? -requantized : requantized) : 0;
		  
		  pXr += 2;
	  }
	  
	  if (cache_sz + bits_left < 0) 
	  {
		  pXr -= 4;
	  }
  }
  
  /* rzero */
  nzero = &xr[576] - pXr;
  while (pXr < &xr[576]) 
  {
	  pXr[0] = 0;
	  pXr[1] = 0;
	  
	  pXr += 2;
  }
  
  return nzero;
}

# undef MASK
# undef MASK1BIT

static void ReorderBlock(int *xr, 
				         SideInfoSub *sisSub,
				         unsigned char const *sfb_width, 
				         int *nonzero,
				         int *tmp,
				         VO_MEM_OPERATOR *pMemop)
{
	unsigned int sb, l, f, n;
	int *pxr, *ptmp;
	
	sb = 0;
	if (sisSub->mixedBlock) 
	{
		sb = 2;
		
		l = 0;
		while (l < 36)
			l += *sfb_width++;
	}
	
	for (l = 18 * sb; l < 576; ) 
	{
		pxr = xr + l;
		ptmp = tmp + l;
		f = *sfb_width;
		for(n = 0; n < f; n++)
		{
			ptmp[3*n	] = pxr[      n];
			ptmp[3*n + 1] = pxr[  f + n];
			ptmp[3*n + 2] = pxr[2*f + n];
		}
		
		l += f*3;
		sfb_width += 3;
	}
	
	pMemop->Copy(VO_INDEX_DEC_MP3,
		&xr[18 * sb], &tmp[sb*18], (576 - 18 * sb) * sizeof(int));
	*nonzero = 576;
}

static int ComputerStereo(int *xr_l,
				          int *xr_r,
				          FrameDataInfo *frame,
				          FrameHeader *header,
				          unsigned char const *sfb_width, 
				          int	gr)
{
	int *modes;
	SideInfo	*sisPs;
	SideInfoSub *sisSub_L;
	SideInfoSub *sisSub_R;	
	unsigned int sfb_i, l, n, i;
	unsigned int sample_len;

	sisPs = &frame->sideInfoPS;	
	modes = frame->modes;
	sisSub_L = &sisPs->sis[gr][0];	
	sisSub_R = &sisPs->sis[gr][1];	
	if (sisSub_L->blockType != sisSub_R->blockType ||
		sisSub_L->mixedBlock != sisSub_R->mixedBlock)
		return -1;
	
	sample_len = frame->xr_nzero[0];
	for (i = 0; i < 39; ++i)
		modes[i] = header->modeext;
	
	if (header->modeext & I_STEREO)
	{
		int const *right_xr = xr_r;
		unsigned int is_pos;
		
		if (sisSub_R->blockType == 2) 
		{
			unsigned int lower_index, start_index, max_index, bound[3], w_index;
			
			lower_index = start_index = max_index = bound[0] = bound[1] = bound[2] = 0;
			
			sfb_i = l = 0;
			
			if (sisSub_R->mixedBlock) 
			{
				while (l < 36) 
				{
					n = sfb_width[sfb_i++];
					
					for (i = 0; i < n; ++i) 
					{
						if (right_xr[i]) 
						{
							lower_index = sfb_i;
							break;
						}
					}
					
					right_xr += n;
					l += n;
				}
				
				start_index = sfb_i;
			}
			
			w_index = 0;
			while (l < sample_len) 
			{
				n = sfb_width[sfb_i++];
				
				for (i = 0; i < n; ++i) 
				{
					if (right_xr[i]) 
					{
						max_index = bound[w_index] = sfb_i;
						break;
					}
				}
				
				right_xr += n;
				l += n;
				w_index = (w_index + 1) % 3;
			}
			
			if (max_index)
				lower_index = start_index;
			
			/* long blocks */			
			for (i = 0; i < lower_index; ++i)
				modes[i] = header->modeext & ~I_STEREO;
			
			/* short blocks */			
			w_index = 0;
			for (i = start_index; i < max_index; ++i) 
			{
				if (i < bound[w_index])
					modes[i] = header->modeext & ~I_STEREO;
				
				w_index = (w_index + 1) % 3;
			}
		}
		else 
		{ 
			unsigned int bound;
			
			bound = 0;
			for (sfb_i = l = 0; l < sample_len; l += n) 
			{
				n = sfb_width[sfb_i++];
				
				for (i = 0; i < n; ++i) 
				{
					if (right_xr[i]) 
					{
						bound = sfb_i;
						break;
					}
				}
				
				right_xr += n;
			}
			
			for (i = 0; i < bound; ++i)
				modes[i] = header->modeext & ~I_STEREO;
		}
		
		if (header->version > MPEG1) 
		{
			unsigned char const *illegal_pos = sisPs->sis[1][1].scalefac;
			int   const *lsf_scale;
			
			lsf_scale = is_lsf_table[sisSub_R->sfCompress & 0x1];
			
			for (sfb_i = l = 0; l < sample_len; ++sfb_i, l += n)
			{
				n = sfb_width[sfb_i];
				
				if (!(modes[sfb_i] & I_STEREO))
					continue;
				
				if (illegal_pos[sfb_i]) 
				{
					modes[sfb_i] &= ~I_STEREO;
					continue;
				}
				
				is_pos = sisSub_R->scalefac[sfb_i];				
				for (i = 0; i < n; ++i) 
				{
					register int left;
					
					left = xr_l[l + i];
					
					if (is_pos == 0)
						xr_r[l + i] = left;
					else 
					{
						int opposite;
						
						opposite = MUL_32(left, lsf_scale[(is_pos - 1) >> 1]);
						opposite <<= 2;
						
						if (is_pos & 1) 
						{
							xr_l[l + i] = opposite;
							xr_r[l + i] = left;
						}
						else
							xr_r[l + i] = opposite;
					}
				}
			}
		}
		else 
		{  
			for (sfb_i = l = 0; l < sample_len; ++sfb_i, l += n) 
			{
				n = sfb_width[sfb_i];
				
				if (!(modes[sfb_i] & I_STEREO))
					continue;
				
				is_pos = sisSub_R->scalefac[sfb_i];
				
				if (is_pos >= 7) 
				{  
					modes[sfb_i] &= ~I_STEREO;
					continue;
				}
				
				for (i = 0; i < n; ++i) 
				{
					int left;
					
					left = xr_l[l + i] << 2;
					
					xr_l[l + i] = MUL_32(left, is_table[    is_pos]);
					xr_r[l + i] = MUL_32(left, is_table[6 - is_pos]);
				}
			}
		}
  }
  
  if (header->modeext & MS_STEREO) 
  {
	  for (sfb_i = l = 0; l < sample_len; ++sfb_i, l += n)
	  {
		  n = sfb_width[sfb_i];
		  
		  if (modes[sfb_i] != MS_STEREO)
			  continue;
		  
		  for (i = 0; i < n; ++i) 
		  {
			  int m, s;
			  
			  m = xr_l[l + i];
			  s = xr_r[l + i];
			  
			  xr_l[l + i] = m + s; 
			  xr_r[l + i] = m - s;  
		  }
	  }
  }
  
  return 0;
}

#ifndef ARM
/*
* NAME:	III_aliasreduce()
* DESCRIPTION:	perform frequency line alias reduction
*/
static void AliasReduce(int *xr, int nsb)
{
	int i, *ptr;
	int const *csa;

	ptr = xr + 18;
    for(i = nsb; i > 0;i--) 
	{
        int tmp0, tmp1, tmp2;
        csa = csa_table[0];
#define INT_AA(j) \
            tmp0 = ptr[-1-j];\
            tmp1 = ptr[   j];\
            tmp2= MUL_32(tmp0 + tmp1, csa[0+4*j]);\
            ptr[-1-j] = 4*(tmp2 - MUL_32(tmp1, csa[2+4*j]));\
            ptr[   j] = 4*(tmp2 + MUL_32(tmp0, csa[3+4*j]));

        INT_AA(0)
        INT_AA(1)
        INT_AA(2)
        INT_AA(3)
        INT_AA(4)
        INT_AA(5)
        INT_AA(6)
        INT_AA(7)

        ptr += 18;
    }
}
#else
void AliasReduce(int *xr, int nsb);
#endif

static int FreqInvert(int *y)
{
	int y0, y1, y2, y3, y4, y5, y6, y7, y8;
	
	y += SBLIMIT;
	y0 = *y;	y += 2*SBLIMIT;
	y1 = *y;	y += 2*SBLIMIT;
	y2 = *y;	y += 2*SBLIMIT;
	y3 = *y;	y += 2*SBLIMIT;
	y4 = *y;	y += 2*SBLIMIT;
	y5 = *y;	y += 2*SBLIMIT;
	y6 = *y;	y += 2*SBLIMIT;
	y7 = *y;	y += 2*SBLIMIT;
	y8 = *y;	y += 2*SBLIMIT;
	
	y -= 18*SBLIMIT;
	*y = -y0;	y += 2*SBLIMIT;
	*y = -y1;	y += 2*SBLIMIT;
	*y = -y2;	y += 2*SBLIMIT;
	*y = -y3;	y += 2*SBLIMIT;
	*y = -y4;	y += 2*SBLIMIT;
	*y = -y5;	y += 2*SBLIMIT;
	*y = -y6;	y += 2*SBLIMIT;
	*y = -y7;	y += 2*SBLIMIT;
	*y = -y8;	y += 2*SBLIMIT;
	
	return 0;
}

static void WinPrevious(int *xPrev, int *xPrevWin, int btPrev)
{
	int i, x, *xp, *xpwLo, *xpwHi, wLo, wHi;
	int const*wpLo, *wpHi;

	xp = xPrev;
	/* mapping (see IMDCT12x3): xPrev[0-2] = sum[6-8], xPrev[3-8] = sum[12-17] */
	if (btPrev == 2) 
	{
		/* this could be reordered for minimum loads/stores */
		wpLo = imdctWin[btPrev];
		xPrevWin[ 0] = MUL_32(wpLo[ 6], xPrev[2]) + MUL_32(wpLo[0], xPrev[6]);
		xPrevWin[ 1] = MUL_32(wpLo[ 7], xPrev[1]) + MUL_32(wpLo[1], xPrev[7]);
		xPrevWin[ 2] = MUL_32(wpLo[ 8], xPrev[0]) + MUL_32(wpLo[2], xPrev[8]);
		xPrevWin[ 3] = MUL_32(wpLo[ 9], xPrev[0]) + MUL_32(wpLo[3], xPrev[8]);
		xPrevWin[ 4] = MUL_32(wpLo[10], xPrev[1]) + MUL_32(wpLo[4], xPrev[7]);
		xPrevWin[ 5] = MUL_32(wpLo[11], xPrev[2]) + MUL_32(wpLo[5], xPrev[6]);
		xPrevWin[ 6] = MUL_32(wpLo[ 6], xPrev[5]);
		xPrevWin[ 7] = MUL_32(wpLo[ 7], xPrev[4]);
		xPrevWin[ 8] = MUL_32(wpLo[ 8], xPrev[3]);
		xPrevWin[ 9] = MUL_32(wpLo[ 9], xPrev[3]);
		xPrevWin[10] = MUL_32(wpLo[10], xPrev[4]);
		xPrevWin[11] = MUL_32(wpLo[11], xPrev[5]);
		xPrevWin[12] = xPrevWin[13] = xPrevWin[14] = xPrevWin[15] = xPrevWin[16] = xPrevWin[17] = 0;
	} 
	else 
	{
		/* use ARM-style pointers (*ptr++) so that ADS compiles well */
		wpLo = imdctWin[btPrev] + 18;
		wpHi = wpLo + 17;
		xpwLo = xPrevWin;
		xpwHi = xPrevWin + 17;
		for (i = 9; i > 0; i--) 
		{
			x = *xp++;	wLo = *wpLo++;	wHi = *wpHi--;
			*xpwLo++ = MUL_32(wLo, x);
			*xpwHi-- = MUL_32(wHi, x);
		}
	}
}


/* format = Q31
 * cos(((0:8) + 0.5) * (pi/18)) 
 */
static const int c18[9] = {
	0x7f834ed0, 0x7ba3751d, 0x7401e4c1, 0x68d9f964, 0x5a82799a, 0x496af3e2, 0x36185aee, 0x2120fb83, 0x0b27eb5c, 
};

#ifndef ARM
/* format = Q31
 * #define M_PI 3.14159265358979323846
 * double u = 2.0 * M_PI / 9.0;
 * float c0 = sqrt(3.0) / 2.0; 
 * float c1 = cos(u);          
 * float c2 = cos(2*u);        
 * float c3 = sin(u);          
 * float c4 = sin(2*u);
 */
static const int c9_0 = 0x6ed9eba1;
static const int c9_1 = 0x620dbe8b;
static const int c9_2 = 0x163a1a7e;
static const int c9_3 = 0x5246dd49;
static const int c9_4 = 0x7e0e2e32;



/* require at least 3 guard bits in x[] to ensure no overflow */
static void voMP3Decidct9(int *x)
{
	int a1, a2, a3, a4, a5, a6, a7, a8, a9;
	int a10, a11, a12, a13, a14, a15, a16, a17, a18;
	int a19, a20, a21, a22, a23, a24, a25, a26, a27;
	int m1, m3, m5, m6, m7, m8, m9, m10, m11, m12;
	int x0, x1, x2, x3, x4, x5, x6, x7, x8;

	x0 = x[0]; x1 = x[1]; x2 = x[2]; x3 = x[3]; x4 = x[4];
	x5 = x[5]; x6 = x[6]; x7 = x[7]; x8 = x[8];

	a1 = x0 - x6;
	a2 = x1 - x5;
	a3 = x1 + x5;
	a4 = x2 - x4;
	a5 = x2 + x4;
	a6 = x2 + x8;
	a7 = x1 + x7;

	a8 = a6 - a5;		/* ie x[8] - x[4] */
	a9 = a3 - a7;		/* ie x[5] - x[7] */
	a10 = a2 - x7;		/* ie x[1] - x[5] - x[7] */
	a11 = a4 - x8;		/* ie x[2] - x[4] - x[8] */

	m1 =  MUL_32(c9_0, x3);
	m3 =  MUL_32(c9_0, a10);
	m5 =  MUL_32(c9_1, a5);
	m6 =  MUL_32(c9_2, a6);
	m7 =  MUL_32(c9_1, a8);
	m8 =  MUL_32(c9_2, a5);
	m9 =  MUL_32(c9_3, a9);
	m10 = MUL_32(c9_4, a7);
	m11 = MUL_32(c9_3, a3);
	m12 = MUL_32(c9_4, a9);

	a12 = x[0] +  (x[6] >> 1);
	a13 = a12  +  (  m1 << 1);
	a14 = a12  -  (  m1 << 1);
	a15 = a1   +  ( a11 >> 1);
	a16 = ( m5 + m6 ) << 1;
	a17 = ( m7 - m8 ) << 1;
	a18 = a16 + a17;
	a19 = ( m9 + m10) << 1;
	a20 = (m11 - m12) << 1;

	a21 = a20 - a19;
	a22 = a13 + a16;
	a23 = a14 + a16;
	a24 = a14 + a17;
	a25 = a13 + a17;
	a26 = a14 - a18;
	a27 = a13 - a18;

	x0 = a22 + a19;			x[0] = x0;
	x1 = a15 + (m3 << 1);	x[1] = x1;
	x2 = a24 + a20;			x[2] = x2;
	x3 = a26 - a21;			x[3] = x3;
	x4 = a1 - a11;			x[4] = x4;
	x5 = a27 + a21;			x[5] = x5;
	x6 = a25 - a20;			x[6] = x6;
	x7 = a15 - (m3 << 1);	x[7] = x7;
	x8 = a23 - a19;			x[8] = x8;
}
#else
void voMP3Decidct9(int *x);
#endif

/* let c(j) = cos(M_PI/36 * ((j)+0.5)), s(j) = sin(M_PI/36 * ((j)+0.5))
 * then fastWin[2*j+0] = c(j)*(s(j) + c(j)), j = [0, 8]
 *      fastWin[2*j+1] = c(j)*(s(j) - c(j))
 * format = Q30
 */
int fastWin36[18] = {
	0x42aace8b, 0xc2e92724, 0x47311c28, 0xc95f619a, 0x4a868feb, 0xd0859d8c,
	0x4c913b51, 0xd8243ea0, 0x4d413ccc, 0xe0000000, 0x4c913b51, 0xe7dbc161,
	0x4a868feb, 0xef7a6275, 0x47311c28, 0xf6a09e67, 0x42aace8b, 0xfd16d8dd,
};

static 
void IMDCT36(int *xCurr, int *xPrev, int *y, int btCurr, int btPrev, int blockIdx, int* buf)
{
	int i, s, d, t;
	int acc1, acc2,mOut;
	int xo, xe, c, *xp, yLo, yHi;
	int *xBuf, *xPrevWin;
	int const *cp, *wp;

	xBuf = buf;
	xPrevWin = buf + 20;

#ifndef ARMV7
	acc1 = acc2 = 0;
	xCurr += 17;
	
	for (i = 8; i >= 0; i--) 
	{	
		acc1 = (*xCurr--) - acc1;
		acc2 = acc1 - acc2;
		acc1 = (*xCurr--) - acc1;
		xBuf[i+9] = acc2;	/* odd */
		xBuf[i+0] = acc1;	/* even */
	}

	xBuf[9] >>= 1;
	xBuf[0] >>= 1;

	voMP3Decidct9(xBuf+0);	/* even */
	voMP3Decidct9(xBuf+9);	/* odd */
#else
	acc1 = acc2 = 0;
	xCurr += 17;
	
	/* max gain = 18, assume adequate guard bits */
	for (i = 8; i >= 0; i--) 
	{	
		acc1 = (*xCurr--) - acc1;
		acc2 = acc1 - acc2;
		acc1 = (*xCurr--) - acc1;
		xBuf[2*i+1] = acc2;	/* odd */
		xBuf[2*i+0] = acc1;	/* even */
	}
	/* xEven[0] and xOdd[0] scaled by 0.5 */
	xBuf[1] >>= 1;
	xBuf[0] >>= 1;
	
	voMP3Decidct9_2(xBuf);
#endif	

	xp = xBuf + 8;
	cp = c18 + 8;
	mOut = 0;
	if (btPrev == 0 && btCurr == 0) 
	{
		wp = fastWin36;
		for (i = 0; i < 9; i++) 
		{			
			c = *cp--;	
			xo = *(xp + 9);		xe = *xp--;

			xo = MUL_32(c, xo);			
			xe >>= 2;

			s = -(*xPrev);		
			d = -(xe - xo);		
			(*xPrev++) = xe + xo;			
			t = (s - d) << 2;
			
			yLo = (d + MUL_32(t, *wp)); wp++;
			yHi = (s + MUL_32(t, *wp)); wp++;
			y[(i)*SBLIMIT]    =  yLo;
			y[(17-i)*SBLIMIT] =  yHi;
		}
	} 
	else 
	{
		WinPrevious(xPrev, xPrevWin, btPrev);

		wp = imdctWin[btCurr];
		for (i = 0; i < 9; i++)
		{
			c = *cp--;	
			xo = *(xp + 9);		xe = *xp--;

			xo = MUL_32(c, xo);			
			xe >>= 2;

			d = xe - xo;
			(*xPrev++) = xe + xo;	
			
			yLo = (xPrevWin[i]    + MUL_32(d, wp[i])) << 2;
			yHi = (xPrevWin[17-i] + MUL_32(d, wp[17-i])) << 2;
			y[(i)*SBLIMIT]    = yLo;
			y[(17-i)*SBLIMIT] = yHi;
		}
	}

	xPrev -= 9;
	if (blockIdx & 0x01) FreqInvert(y);
}

static int c3_0 = 0x6ed9eba1;	
static int c6[3] = { 0x7ba3751d, 0x5a82799a, 0x2120fb83 };	

/* 12-point inverse DCT, used in IMDCT12x3() 
 * 4 input guard bits will ensure no overflow
 */
static __inline void imdct12(int *x, int *out)
{
	int a0, a1, a2;
	int x0, x1, x2, x3, x4, x5;

	x0 = *x;	x+=3;	x1 = *x;	x+=3;
	x2 = *x;	x+=3;	x3 = *x;	x+=3;
	x4 = *x;	x+=3;	x5 = *x;	x+=3;

	x4 -= x5;
	x3 -= x4;
	x2 -= x3;
	x3 -= x5;
	x1 -= x2;
	x0 -= x1;
	x1 -= x3;

	x0 >>= 1;
	x1 >>= 1;

	a0 = MUL_32(c3_0, (x2 << 1)) ;
	a1 = x0 + (x4 >> 1);
	a2 = x0 - x4;
	x0 = a1 + a0;
	x2 = a2;
	x4 = a1 - a0;

	a0 = MUL_32(c3_0, (x3 << 1)) ;
	a1 = x1 + (x5 >> 1);
	a2 = x1 - x5;

	x1 = MUL_32(c6[0], ((a1 + a0) << 2));			
	x3 = MUL_32(c6[1], (a2 << 2));
	x5 = MUL_32(c6[2], ((a1 - a0) << 2));

	*out = x0 + x1;	out++;
	*out = x2 + x3;	out++;
	*out = x4 + x5;	out++;
	*out = x4 - x5;	out++;
	*out = x2 - x3;	out++;
	*out = x0 - x1;
}

 static int IMDCT12x3(int *xCurr, int *xPrev, int *y, int btPrev, int blockIdx, int* buf)
{
	int i, mOut, yLo;	
	int *xBuf, *xPrevWin;
	const int *wp;

	xBuf = buf;
	xPrevWin = buf + 20;

	imdct12(xCurr + 0, xBuf + 0);
	imdct12(xCurr + 1, xBuf + 6);
	imdct12(xCurr + 2, xBuf + 12);

	WinPrevious(xPrev, xPrevWin, btPrev);

	wp = imdctWin[2];
	mOut = 0;
	for (i = 0; i < 3; i++) 
	{
		yLo = (xPrevWin[ 0+i] << 2);
		y[( 0+i)*SBLIMIT] = yLo;
		yLo = (xPrevWin[ 3+i] << 2);
		y[( 3+i)*SBLIMIT] = yLo;
		yLo = (xPrevWin[ 6+i] << 2) + (MUL_32(wp[0+i], xBuf[3+i]));	
		y[( 6+i)*SBLIMIT] = yLo;
		yLo = (xPrevWin[ 9+i] << 2) + (MUL_32(wp[3+i], xBuf[5-i]));	
		y[( 9+i)*SBLIMIT] = yLo;
		yLo = (xPrevWin[12+i] << 2) + (MUL_32(wp[6+i], xBuf[2-i]) + MUL_32(wp[0+i], xBuf[(6+3)+i]));	
		y[(12+i)*SBLIMIT] = yLo;
		yLo = (xPrevWin[15+i] << 2) + (MUL_32(wp[9+i], xBuf[0+i]) + MUL_32(wp[3+i], xBuf[(6+5)-i]));	
		y[(15+i)*SBLIMIT] = yLo;
	}

	for (i = 6; i < 9; i++)
		*xPrev++ = xBuf[i] >> 2;
	for (i = 12; i < 18; i++)
		*xPrev++ = xBuf[i] >> 2;

	xPrev -= 9;
	if (blockIdx & 0x01) FreqInvert(y);

	return 0;
}
 
static
void IMDCT(FrameDataInfo *frame, int *xr, int gr, int ch)
{
	int *xPrevWin, *ptr;
	int *buf, *out_ptr, *sbsamples, *modes;
	SideInfoSub *sisSub;
    int i, j, mdct_long_end, sblimit;
	int currWinIdx, prevWinIdx;
	
	sisSub = &frame->sideInfoPS.sis[gr][ch];
	sbsamples = &frame->sbSample[ch][18 * gr][0];
    sblimit = MIN((frame->xr_nzero[ch] / 18) + 1, 32);
	modes = frame->modes;

    if (sisSub->blockType == 2) 
	{
        if (sisSub->mixedBlock)
            mdct_long_end = 2;
        else
            mdct_long_end = 0;
    } 
	else 
	{
        mdct_long_end = sblimit;
    }

    buf = &frame->overlap[ch][0];
    ptr = xr;
	currWinIdx = sisSub->blockType;
	prevWinIdx = frame->preType[ch];
    for(i=0; i<mdct_long_end; i++) 
	{
        out_ptr = sbsamples + i;
        IMDCT36(ptr, buf, out_ptr, currWinIdx, prevWinIdx, i, modes);
        out_ptr += 18*SBLIMIT;
        ptr += 18;
        buf += 9;
    }
    for(i=mdct_long_end;i<sblimit;i++) 
	{
        out_ptr = sbsamples + i;

		IMDCT12x3(ptr, buf, out_ptr, prevWinIdx, i, modes);

        ptr += 18;
        buf += 9;
    }

	xPrevWin = modes;
	for(i=sblimit;i<frame->preIMDCT[ch];i++) 
	{
        int fiBit, xp;

        out_ptr = sbsamples + i;
		WinPrevious(buf, xPrevWin, prevWinIdx);

		fiBit = i << 31;
		for (j = 0; j < 9; j++) 
		{
			xp = xPrevWin[2*j+0] << 2;	
			*out_ptr = xp;

			xp = xPrevWin[2*j+1] << 2;
			xp = (xp ^ (fiBit >> 31)) + (i & 0x01);	
			*(out_ptr+32) = xp;

			buf[j] = 0;

			out_ptr += 2*SBLIMIT;
		}
		buf += 9;
    }

	sblimit = MAX(frame->preIMDCT[ch], sblimit);
	out_ptr = sbsamples;
	for (j = 0; j < 18; j++) 	
	{
		for (i = sblimit; i < SBLIMIT; i++) 
		{
			out_ptr[i] = 0;			
		}
		out_ptr += SBLIMIT;
	}
	
	frame->preType[ch] = sisSub->blockType;
	frame->preIMDCT[ch] = sblimit;
}

int voMP3DecLayerIII(FrameDataInfo *frame, FrameStream *stream, VO_MEM_OPERATOR *pMemop)
{
	int *pXr[MAX_NCHAN];
	FrameHeader *pHeader;
	Bitstream* pBit;
	SideInfo *pSi;
	int *exponents;
	int	result;
	int len, nch;
	int ch, gr;

	pHeader = &frame->header;
	exponents = frame->modes;
	pBit = &stream->bitptr;
	nch = pHeader->channels;
	frame->siLen = (pHeader->version > MPEG1) ?
		(nch == 1 ? 9 : 17) : (nch == 1 ? 17 : 32);
	for(ch = 0; ch < nch;  ch++)
	{
		pXr[ch] = &frame->xr[ch][0];
	}


	voMP3DecUnpackSideInfo(frame, stream);
	pSi = &frame->sideInfoPS;

	if (stream->md_len >= pSi->mainDataBegin) 
	{
		len  = pHeader->framelen - pHeader->headLen - frame->siLen;
		pMemop->Copy(VO_INDEX_DEC_MP3, 
			stream->main_data, stream->main_data + stream->md_len - pSi->mainDataBegin, pSi->mainDataBegin);
		pMemop->Copy(VO_INDEX_DEC_MP3, 
			stream->main_data + pSi->mainDataBegin, stream->this_frame + pHeader->headLen + frame->siLen, len);
		
		stream->md_len =  pSi->mainDataBegin + len;
	} 
	else 
	{
		len  = pHeader->framelen - pHeader->headLen - frame->siLen;
		pMemop->Copy(VO_INDEX_DEC_MP3, 
			stream->main_data + stream->md_len, stream->this_frame + pHeader->headLen + frame->siLen, len);
		stream->md_len += len;
		return VO_MP3_Mdoule_INPUT_BUFFER_SMALL;
	}
	
	pSi = &frame->sideInfoPS;
	voMP3DecInitBits(pBit, stream->main_data, stream->md_len);
	for (gr = 0; gr < frame->nGrans; ++gr) 
	{
		SideInfoSub *sisSub;
		unsigned char const *sfb_width[2];
		for (ch = 0; ch < nch; ++ch) 
		{
			unsigned int part2_length;
			
			sisSub = &pSi->sis[gr][ch];			
			sfb_width[ch] = sfbwidth_table[pHeader->subIndex].l;
			if (sisSub->blockType == 2) 
			{
				sfb_width[ch] = (sisSub->mixedBlock) ?
					sfbwidth_table[pHeader->subIndex].m : sfbwidth_table[pHeader->subIndex].s;
			}
			
			if (pHeader->version > MPEG1) 
			{
				part2_length = voMP3DecUnpackScaleFactorsLsf(pBit, sisSub,
					ch == 0 ? 0 : &pSi->sis[1][1],
					pHeader->modeext);
			}
			else 
			{
				part2_length = voMP3DecUnpackScaleFactors(pBit, sisSub, &pSi->sis[0][ch],
					gr == 0 ? 0 : pSi->scfSi[ch]);
			}
			
			result = voMP3DecHuffDecode(pBit, pXr[ch], sisSub, sfb_width[ch], part2_length, pHeader->modeext, exponents);
			if (result < 0)
				return result;

			frame->xr_nzero[ch] = 576 - result;		
			if((frame->xr_nzero[ch]<0) || (frame->xr_nzero[ch]>576))
			{
				frame->xr_nzero[ch] = 0;
			}
		}
		
		if (pHeader->mode == MPA_MODE_JOINT_STEREO && pHeader->modeext) 
		{
			len = MAX(frame->xr_nzero[0], frame->xr_nzero[1]);
			frame->xr_nzero[0] = frame->xr_nzero[1] = len;
			result = ComputerStereo(pXr[0], pXr[1], frame, pHeader, sfb_width[0], gr);
			if (result)
				return result;
		}

		for (ch = 0; ch < nch; ++ch) 
		{			
			sisSub = &pSi->sis[gr][ch];
			if (sisSub->blockType == 2) 
			{
				ReorderBlock(pXr[ch], sisSub, &sfb_width[ch][0], 
					&frame->xr_nzero[ch], (int *)&frame->sbSample[ch][18 * gr][0], pMemop);
				
				if (sisSub->mixedBlock)
					AliasReduce(pXr[ch], 2);
			}
			else
			{
				len = MIN(((frame->xr_nzero[ch] + 7) / 18) + 1, 32) - 1;
				AliasReduce(pXr[ch], len);
				frame->xr_nzero[ch] = MAX(frame->xr_nzero[ch], (len * 18) + 8);
			}

			IMDCT(frame, pXr[ch], gr, ch);
		}
	}
	
	return VO_MP3_Mdoule_SUCCEED;
}
