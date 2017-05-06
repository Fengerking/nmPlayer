# include "config.h"
# include "global.h"
# include "frame.h"

#ifdef ARMV6
const short polyCoef[264] = {
#else
const int polyCoef[264] = {
#endif
	     0,      7,     53,    115,    509,   1288,   1644,   9372, 
	 18760,  -9372,   1644,  -1288,    509,   -115,     53,     -7, 
	     0,      7,     55,    100,    500,   1197,   1490,   8910, 
	 18748,  -9834,   1784,  -1379,    516,   -130,     52,     -8, 
	     0,      6,     56,     87,    488,   1106,   1322,   8448, 
	 18714, -10294,   1910,  -1470,    520,   -145,     51,     -9, 
	     0,      5,     56,     74,    473,   1016,   1140,   7987, 
	 18658, -10751,   2023,  -1559,    522,   -161,     49,     -9, 
	     0,      5,     57,     61,    456,    926,    944,   7528, 
	 18578, -11205,   2123,  -1647,    521,   -178,     48,    -10, 
	     0,      4,     57,     49,    435,    838,    734,   7072, 
	18477, -11654,   2210,  -1734,    519,   -195,     46,    -11, 
	    0,      4,     57,     38,    411,    751,    509,   6621, 
	18354, -12097,   2285,  -1818,    514,   -212,     44,    -12, 
	    0,      4,     57,     28,    384,    666,    271,   6174, 
	18209, -12534,   2347,  -1899,    508,   -230,     42,    -13, 
	    0,      3,     56,     18,    354,    583,     18,   5732, 
	18042, -12963,   2398,  -1977,    500,   -248,     40,    -14, 
	    0,      3,     55,      9,    320,    502,   -249,   5297, 
	17855, -13383,   2438,  -2052,    491,   -266,     39,    -16, 
	    0,      3,     54,      1,    283,    423,   -530,   4870, 
	17648, -13794,   2466,  -2123,    480,   -284,     37,    -17, 
	   -1,      2,     52,     -7,    243,    347,   -825,   4450, 
	 17420, -14194,   2484,  -2189,    468,   -302,     35,    -18, 
	   -1,      2,     50,    -14,    199,    274,  -1133,   4039, 
	17173, -14583,   2492,  -2249,    454,   -321,     33,    -20, 
	   -1,      2,     47,    -21,    151,    204,  -1454,   3637, 
	 16907, -14959,   2490,  -2305,    440,   -339,     31,    -21, 
	   -1,      2,     44,    -26,    101,    136,  -1788,   3245, 
	16624, -15322,   2479,  -2354,    425,   -357,     29,    -23, 
	   -1,      2,     41,    -32,     46,     72,  -2135,   2864, 
	 16323, -15671,   2460,  -2396,    409,   -374,     28,    -24, 
		26,    392,   2432,  16005,  -2494,    -11,     37,     -1
};

#ifndef ARM
#define DEF_NFRACBITS	(25 - 2 - 2 - 15)	
#define CSHIFT	12	

static __inline short ClipToShort(int x, int fracBits)
{
	int sign;
	
	x >>= fracBits;
	
	sign = x >> 31;
	if (sign != (x >> 15))
		x = sign ^ ((1 << 15) - 1);

	return (short)x;
}

#define MC0M(x)	{ \
	c1 = *coef;		coef++;		c2 = *coef;		coef++; \
	vLo = *(vb1+(x));			vHi = *(vb1+(23-(x))); \
	sum1L += vLo * c1;	sum1L -= vHi * c2; \
}

#define MC1M(x)	{ \
	c1 = *coef;		coef++; \
	vLo = *(vb1+(x)); \
	sum1L += vLo * c1; \
}

#define MC2M(x)	{ \
		c1 = *coef;		coef++;		c2 = *coef;		coef++; \
		vLo = *(vb1+(x));	vHi = *(vb1+(23-(x))); \
		sum1L += vLo * c1;	sum2L += vLo * c2; \
		sum1L -= vHi * c2;	sum2L += vHi * c1; \
}

static void voMP3DecSynthMono(short *pcm, int *vbuf, const int *coefBase)
{	
	int i;
	const int *coef;
	int *vb1;
	int vLo, vHi, c1, c2;
	int sum1L, sum2L, rndVal;

	rndVal = 1 << (DEF_NFRACBITS + 20 - CSHIFT);

	/* special case, output sample 0 */
	coef = coefBase;
	vb1 = vbuf;
	sum1L = rndVal;

	MC0M(0)
	MC0M(1)
	MC0M(2)
	MC0M(3)
	MC0M(4)
	MC0M(5)
	MC0M(6)
	MC0M(7)

	*(pcm + 0) = ClipToShort(sum1L, (21 - CSHIFT + DEF_NFRACBITS));

	/* special case, output sample 16 */
	coef = coefBase + 256;
	vb1 = vbuf + 64*16;
	sum1L = rndVal;

	MC1M(0)
	MC1M(1)
	MC1M(2)
	MC1M(3)
	MC1M(4)
	MC1M(5)
	MC1M(6)
	MC1M(7)

	*(pcm + 16) = ClipToShort(sum1L, (21 - CSHIFT + DEF_NFRACBITS));

	/* main convolution loop: sum1L = samples 1, 2, 3, ... 15   sum2L = samples 31, 30, ... 17 */
	coef = coefBase + 16;
	vb1 = vbuf + 64;
	pcm++;

	for (i = 15; i > 0; i--) 
	{
		sum1L = sum2L = rndVal;

		MC2M(0)
		MC2M(1)
		MC2M(2)
		MC2M(3)
		MC2M(4)
		MC2M(5)
		MC2M(6)
		MC2M(7)

		vb1 += 64;
		*(pcm)       = ClipToShort(sum1L, (21 - CSHIFT + DEF_NFRACBITS));
		*(pcm + 2*i) = ClipToShort(sum2L, (21 - CSHIFT + DEF_NFRACBITS));
		pcm++;
	}
}

#define MC0S(x)	{ \
	c1 = *coef;		coef++;		c2 = *coef;		coef++; \
	vLo = *(vb1+(x));		vHi = *(vb1+(23-(x))); \
	sum1L += vLo * c1;	sum1L -= vHi * c2; \
	vLo = *(vb1+32+(x));	vHi = *(vb1+32+(23-(x))); \
	sum1R += vLo * c1;	sum1R -= vHi * c2; \
}

#define MC1S(x)	{ \
	c1 = *coef;		coef++; \
	vLo = *(vb1+(x)); \
	sum1L += vLo * c1; \
	vLo = *(vb1+32+(x)); \
	sum1R += vLo * c1; \
}

#define MC2S(x)	{ \
		c1 = *coef;		coef++;		c2 = *coef;		coef++; \
		vLo = *(vb1+(x));	vHi = *(vb1+(23-(x))); \
		sum1L += vLo * c1;	sum2L += vLo * c2; \
		sum1L -= vHi * c2;	sum2L += vHi * c1; \
		vLo = *(vb1+32+(x));	vHi = *(vb1+32+(23-(x))); \
		sum1R += vLo * c1;	sum2R += vLo * c2; \
		sum1R -= vHi * c2;	sum2R += vHi * c1; \
}

static void voMP3DecSynthStereo(short *pcm, int *vbuf, const int *coefBase)
{
	int i;
	const int *coef;
	int *vb1;
	int vLo, vHi, c1, c2;
	int sum1L, sum2L, sum1R, sum2R, rndVal;

	rndVal = 1 << (DEF_NFRACBITS + 20 - CSHIFT);

	/* special case, output sample 0 */
	coef = coefBase;
	vb1 = vbuf;
	sum1L = sum1R = rndVal;

	MC0S(0)
	MC0S(1)
	MC0S(2)
	MC0S(3)
	MC0S(4)
	MC0S(5)
	MC0S(6)
	MC0S(7)

	*(pcm + 0) = ClipToShort(sum1L, (21-CSHIFT + DEF_NFRACBITS));
	*(pcm + 1) = ClipToShort(sum1R, (21-CSHIFT + DEF_NFRACBITS));

	/* special case, output sample 16 */
	coef = coefBase + 256;
	vb1 = vbuf + 64*16;
	sum1L = sum1R = rndVal;

	MC1S(0)
	MC1S(1)
	MC1S(2)
	MC1S(3)
	MC1S(4)
	MC1S(5)
	MC1S(6)
	MC1S(7)

	*(pcm + 2*16 + 0) = ClipToShort(sum1L, (21-CSHIFT + DEF_NFRACBITS));
	*(pcm + 2*16 + 1) = ClipToShort(sum1R, (21-CSHIFT + DEF_NFRACBITS));

	/* main convolution loop: sum1L = samples 1, 2, 3, ... 15   sum2L = samples 31, 30, ... 17 */
	coef = coefBase + 16;
	vb1 = vbuf + 64;
	pcm += 2;

	for (i = 15; i > 0; i--) 
	{
		sum1L = sum2L = rndVal;
		sum1R = sum2R = rndVal;

		MC2S(0)
		MC2S(1)
		MC2S(2)
		MC2S(3)
		MC2S(4)
		MC2S(5)
		MC2S(6)
		MC2S(7)

		vb1 += 64;
		*(pcm + 0)         = ClipToShort(sum1L, (21-CSHIFT + DEF_NFRACBITS));
		*(pcm + 1)         = ClipToShort(sum1R, (21-CSHIFT + DEF_NFRACBITS));
		*(pcm + 2*2*i + 0) = ClipToShort(sum2L, (21-CSHIFT + DEF_NFRACBITS));
		*(pcm + 2*2*i + 1) = ClipToShort(sum2R, (21-CSHIFT + DEF_NFRACBITS));
		pcm += 2;
	}
}
#else
void voMP3DecSynthMono(short *pcm, int *vbuf, const short *coefBase);
void voMP3DecSynthStereo(short *pcm, int *vbuf, const short *coefBase);
#endif

int voMP3DecSubbandFrame(FrameDataInfo *frame, SubbandInfo *subband, short *outbuf, int nch, int ns)
{
	int b, ngr, gr, nsg;
	
	ngr = frame->nGrans;
	ns =  ns/ngr;
	
	if (nch == 2) 
	{
		/* stereo */
		for(gr = 0; gr < ngr; gr++)
		{			
			nsg = ns*gr;
			for (b = 0; b < ns; b++) 
			{
				voMP3DecDCT32(frame->sbSample[0][b+nsg], subband->vbuf + 0*32, subband->vindex, (b & 0x01));
				voMP3DecDCT32(frame->sbSample[1][b+nsg], subband->vbuf + 1*32, subband->vindex, (b & 0x01));
#ifdef ARMV7
                voMP3DecSynthStereo(outbuf, (int *)(subband->vbuf + subband->vindex + VBUF_LENGTH * (b & 0x01)), polyCoef);
#else
				voMP3DecSynthStereo(outbuf, subband->vbuf + subband->vindex + VBUF_LENGTH * (b & 0x01), polyCoef);
#endif
				subband->vindex = (subband->vindex - (b & 0x01)) & 7;
				outbuf += (2 * SBLIMIT);
			}
		}
	}
	else 
	{
		/* mono */
		for(gr = 0; gr < ngr; gr++)
		{			
			nsg = ns*gr;
			for (b = 0; b < ns; b++) 
			{
				voMP3DecDCT32(frame->sbSample[0][b+nsg], subband->vbuf + 0*32, subband->vindex, (b & 0x01));
#ifdef ARMV7
                voMP3DecSynthMono(outbuf, (int*)(subband->vbuf + subband->vindex + VBUF_LENGTH * (b & 0x01)), polyCoef);
#else
				voMP3DecSynthMono(outbuf, subband->vbuf + subband->vindex + VBUF_LENGTH * (b & 0x01), polyCoef);
#endif
				subband->vindex = (subband->vindex - (b & 0x01)) & 7;
				outbuf += SBLIMIT;
			}
		}
	}

	return 0;
}
