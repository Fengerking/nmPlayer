#include "config.h"
#include "global.h"
#include "bit.h"
#include "stream.h"
#include "frame.h"
#include "voMP3.h"

/*
* scalefactor table
* used in both Layer I and Layer II decoding
* Q28
*/
static int const sf_table[64] = {
 536870912, 426114725, 338207482, 268435456, 213057363, 169103741, 134217728, 106528681,
  84551870,  67108864,  53264341,  42275935,  33554432,  26632170,  21137968,  16777216,
  13316085,  10568984,   8388608,   6658043,   5284492,   4194304,   3329021,   2642246,
   2097152,   1664511,   1321123,   1048576,    832255,    660561,    524288,    416128,

    330281,    262144,    208064,    165140,    131072,    104032,     82570,     65536,
     52016,     41285,     32768,     26008,     20643,     16384,     13004,     10321,
      8192,      6502,      5161,      4096,      3251,      2580,      2048,      1625,
      1290,      1024,       813,       645,       512,       406,       323,         0

};

/* --- Layer I ------------------------------------------------------------- */

/* linear scaling table */
/* Q29 */
static int const linear_table[14] = {
	357913941, 306783378, 286331153, 277094664, 272696336, 270549121, 
	269488144, 268960770, 268697856, 268566592, 268501008, 268468228,
	268451841, 268443648
};

/*
* NAME:	I_sample()
* DESCRIPTION:	decode one requantized Layer I sample from a bitstream
*/
static int I_dequantize_sample(Bitstream *ptr, unsigned int nb)
{
	int sample;
	
	sample = voMP3DecGetBits(ptr, nb);
	
	sample ^= 1 << (nb - 1);
	sample |= -(sample & (1 << (nb - 1)));
	
	sample <<= F_FRACBITS - (nb - 1);
	
	/* requantize the sample */
	sample += F_ONE >> (nb - 1);
	
	return MUL_32(sample, linear_table[nb - 2]) >> 1 ;	

}

/*
* NAME:	layer->I()
* DESCRIPTION:	decode a single Layer I frame
*/
int voMP3DecLayerI(FrameDataInfo *frame, FrameStream *stream)
{
	FrameHeader *pHeader = &frame->header;
	unsigned int nch, bound, ch, sIndex, sBand, nBits;
	Bitstream* pBit;
	unsigned char *pBuf, *allocations[2], *scalefactor[2];

	pBuf = (unsigned char *)frame->overlap[0];
	
	allocations[0] = pBuf;
	allocations[1] = pBuf + 32;

	scalefactor[0] = pBuf + 64;
	scalefactor[1] = pBuf + 96;
	
	nch = pHeader->channels;	
	bound = 32;
	if (pHeader->mode == MPA_MODE_JOINT_STEREO) 
	{
		bound = 4 + pHeader->modeext * 4;
	}
	pBit = &stream->bitptr;

	voMP3DecInitBits(pBit, stream->this_frame + pHeader->headLen, pHeader->framelen - pHeader->headLen);
	
	/* decode bit allocations */	
	for (sBand = 0; sBand < bound; sBand++) 
	{
		for (ch = 0; ch < nch; ch++) 
		{
			nBits = voMP3DecGetBits(pBit, 4);
			
			if (nBits == 15) 
			{
				return -1;
			}
			
			allocations[ch][sBand] = nBits ? nBits + 1 : 0;
		}
	}
	
	for (sBand = bound; sBand < 32; sBand++) 
	{
		nBits = voMP3DecGetBits(pBit, 4);
		
		if (nBits == 15) 
		{
			return -1;
		}
		
		allocations[0][sBand] =
			allocations[1][sBand] = nBits ? nBits + 1 : 0;
	}
	
	/* decode scalefactors */
	
	for (sBand = 0; sBand < 32; sBand++) 
	{
		for (ch = 0; ch < nch; ch++) 
		{
			if (allocations[ch][sBand]) 
			{
				scalefactor[ch][sBand] = voMP3DecGetBits(pBit, 6);
			}
		}
	}
	
	/* decode samples */
	
	for (sIndex = 0; sIndex < 12; sIndex++) 
	{
		for (sBand = 0; sBand < bound; sBand++) 
		{
			for (ch = 0; ch < nch; ch++) 
			{
				nBits = allocations[ch][sBand];
				frame->sbSample[ch][sIndex][sBand] = nBits ?
					MUL_28(I_dequantize_sample(pBit, nBits),
					sf_table[scalefactor[ch][sBand]]) : 0;
			}
		}
		
		for (sBand = bound; sBand < 32; sBand++) 
		{
			if ((nBits = allocations[0][sBand])) 
			{
				int sample;				
				sample = I_dequantize_sample(pBit, nBits);				
				for (ch = 0; ch < nch; ch++) 
				{
					frame->sbSample[ch][sIndex][sBand] =
						MUL_28(sample, sf_table[scalefactor[ch][sBand]]);
				}
			}
			else 
			{
				for (ch = 0; ch < nch; ch++)
					frame->sbSample[ch][sIndex][sBand] = 0;
			}
		}
	}
	
	return VO_MP3_Mdoule_SUCCEED;
}

/* --- Layer II ------------------------------------------------------------ */

/* possible quantization per subband table */
static struct {
	unsigned int sblimit;
	unsigned char const offsets[30];
} const sb_table[5] = {
	/* ISO/IEC 11172-3 Table B.2a */
	{ 0x1b, { 0x7, 0x7, 0x7, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x3, 0x3, 0x3, 0x3, 0x3,	/* 0 */
		0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0, 0, 0, 0 } },
		/* ISO/IEC 11172-3 Table B.2b */
	{ 0x1e, { 0x7, 0x7, 0x7, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x3, 0x3, 0x3, 0x3, 0x3,	/* 1 */
	0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0, 0, 0, 0, 0, 0, 0 } },
	/* ISO/IEC 11172-3 Table B.2c */
	{  0x8, { 0x5, 0x5, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2 } },				/* 2 */
	/* ISO/IEC 11172-3 Table B.2d */
	{ 0xc, { 0x5, 0x5, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2 } },		/* 3 */
	/* ISO/IEC 13818-3 Table B.1 */
	{ 0x1e, { 0x4, 0x4, 0x4, 0x4, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1,	/* 4 */
	0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1 } }
};

/* bit allocation table */
static struct {
	unsigned short nbal;
	unsigned short offset;
} const bitalloc_table[8] = {
	{ 0x2, 0x0 },  /* 0 */
	{ 0x2, 0x3 },  /* 1 */
	{ 0x3, 0x3 },  /* 2 */
	{ 0x3, 0x1 },  /* 3 */
	{ 0x4, 0x2 },  /* 4 */
	{ 0x4, 0x3 },  /* 5 */
	{ 0x4, 0x4 },  /* 6 */
	{ 0x4, 0x5 }   /* 7 */
};

/* offsets into quantization class table */
static unsigned char const offset_table[6][15] = {
	{ 0x0, 0x1, 0x10                                             },  /* 0 */
	{ 0x0, 0x1,  0x2, 0x3, 0x4, 0x5, 0x10                                },  /* 1 */
	{ 0x0, 0x1,  0x2, 0x3, 0x4, 0x5,  0x6, 0x7,  0x8,  0x9, 0xa, 0xb, 0xc, 0xd, 0xe },  /* 2 */
	{ 0x0, 0x1,  0x3, 0x4, 0x5, 0x6,  0x7, 0x8,  0x9,  0xa, 0xb, 0xc, 0xd, 0xe, 0xf },  /* 3 */
	{ 0x0, 0x1,  0x2, 0x3, 0x4, 0x5,  0x6, 0x7,  0x8,  0x9, 0xa, 0xb, 0xc, 0xd, 0x10 },  /* 4 */
	{ 0x0, 0x2,  0x4, 0x5, 0x6, 0x7,  0x8, 0x9,  0xa,  0xb, 0xc, 0xd, 0xe, 0xf, 0x10 }   /* 5 */
};

/* quantization class table */
static struct quantclass {
	unsigned short nlevels;
	unsigned char group;
	unsigned char bits;
	int C;
	int D;
} const qc_table[17] = {
{     0x3, 0x2,  0x5,
    (0x15555555) /* 1.33333333333 => 1.33333333209, e  0.00000000124 */,
    (0x08000000) /* 0.50000000000 => 0.50000000000, e  0.00000000000 */ },
  {   0x5, 0x3,  0x7,
    (0x1999999a) /* 1.60000000000 => 1.60000000149, e -0.00000000149 */,
    (0x08000000) /* 0.50000000000 => 0.50000000000, e  0.00000000000 */ },
  {   0x7, 0x0,  0x3,
    (0x12492492) /* 1.14285714286 => 1.14285714179, e  0.00000000107 */,
    (0x04000000) /* 0.25000000000 => 0.25000000000, e  0.00000000000 */ },
  {   0x9, 0x4,  0xa,
    (0x1c71c71c) /* 1.77777777777 => 1.77777777612, e  0.00000000165 */,
    (0x08000000) /* 0.50000000000 => 0.50000000000, e  0.00000000000 */ },
  {   0xf, 0x0,  0x4,
    (0x11111111) /* 1.06666666666 => 1.06666666642, e  0.00000000024 */,
    (0x02000000) /* 0.12500000000 => 0.12500000000, e  0.00000000000 */ },
  {   0x1f, 0x0, 0x5,
    (0x10842108) /* 1.03225806452 => 1.03225806355, e  0.00000000097 */,
    (0x01000000) /* 0.06250000000 => 0.06250000000, e  0.00000000000 */ },
  {   0x3f, 0x0, 0x6,
    (0x10410410) /* 1.01587301587 => 1.01587301493, e  0.00000000094 */,
    (0x00800000) /* 0.03125000000 => 0.03125000000, e  0.00000000000 */ },
  {   0x7f, 0x0, 0x7,
    (0x10204081) /* 1.00787401575 => 1.00787401572, e  0.00000000003 */,
    (0x00400000) /* 0.01562500000 => 0.01562500000, e  0.00000000000 */ },
  {   0xff, 0x0, 0x8,
    (0x10101010) /* 1.00392156863 => 1.00392156839, e  0.00000000024 */,
    (0x00200000) /* 0.00781250000 => 0.00781250000, e  0.00000000000 */ },
  {   0x1ff, 0x0, 0x9,
    (0x10080402) /* 1.00195694716 => 1.00195694715, e  0.00000000001 */,
    (0x00100000) /* 0.00390625000 => 0.00390625000, e  0.00000000000 */ },
  {   0x3ff, 0x0, 0xa,
    (0x10040100) /* 1.00097751711 => 1.00097751617, e  0.00000000094 */,
    (0x00080000) /* 0.00195312500 => 0.00195312500, e  0.00000000000 */ },
  {   0x7ff, 0x0, 0xb,
    (0x10020040) /* 1.00048851979 => 1.00048851967, e  0.00000000012 */,
    (0x00040000) /* 0.00097656250 => 0.00097656250, e  0.00000000000 */ },
  {   0xfff, 0x0, 0xc,
    (0x10010010) /* 1.00024420024 => 1.00024420023, e  0.00000000001 */,
    (0x00020000) /* 0.00048828125 => 0.00048828125, e  0.00000000000 */ },
  {  0x1fff, 0x0, 0xd,
    (0x10008004) /* 1.00012208522 => 1.00012208521, e  0.00000000001 */,
    (0x00010000) /* 0.00024414063 => 0.00024414062, e  0.00000000000 */ },
  {  0x3fff, 0x0, 0xe,
    (0x10004001) /* 1.00006103888 => 1.00006103888, e -0.00000000000 */,
    (0x00008000) /* 0.00012207031 => 0.00012207031, e -0.00000000000 */ },
  {  0x7fff, 0x0, 0xf,
    (0x10002000) /* 1.00003051851 => 1.00003051758, e  0.00000000093 */,
    (0x00004000) /* 0.00006103516 => 0.00006103516, e  0.00000000000 */ },
  {  0xffff, 0x0, 0x10,
    (0x10001000) /* 1.00001525902 => 1.00001525879, e  0.00000000023 */,
    (0x00002000) /* 0.00003051758 => 0.00003051758, e  0.00000000000 */ }
};

static void II_dequantize_samples(Bitstream *ptr,
				                  struct quantclass const *quantclass,
				                  int output[3])
{
	unsigned int nBits, sIndex, sample[3];
	
	if ((nBits = quantclass->group)) 
	{
		unsigned int cValue, nLevels;
		
		cValue = voMP3DecGetBits(ptr, quantclass->bits);
		nLevels = quantclass->nlevels;
		
		for (sIndex = 0; sIndex < 3; sIndex++) 
		{
			sample[sIndex] = cValue % nLevels;
			cValue /= nLevels;
		}
	}
	else 
	{
		nBits = quantclass->bits;
		
		for (sIndex = 0; sIndex < 3; sIndex++)
			sample[sIndex] = voMP3DecGetBits(ptr, nBits);
	}
	
	for (sIndex = 0; sIndex < 3; sIndex++) 
	{
		int requantizedValue;
		
		requantizedValue  = sample[sIndex] ^ (1 << (nBits - 1));
		requantizedValue |= -(requantizedValue & (1 << (nBits - 1)));
		
		requantizedValue <<= F_FRACBITS - (nBits - 1);
		
		output[sIndex] = MUL_32(requantizedValue + quantclass->D, quantclass->C) >> 1;
	}
}

int voMP3DecLayerII(FrameDataInfo *frame, FrameStream *stream)
{
	FrameHeader *pHeader = &frame->header;
	Bitstream start;
	Bitstream* pBit;
	unsigned int index, sbLimit, nbal, nch;
	unsigned int nbits, bound, gran, ch, sIndex, sBand;
	unsigned char const *offsets;
	unsigned char scalefactor[2][32][3];
	unsigned char *sf, *pBuf;
	unsigned char *allocation[2], *scfsi[2];
	//unsigned char allocation[2][32], scalefactor[2][32];
	int samples_qcout[3];	

	pBuf = (unsigned char *)frame->overlap[0];
	
	allocation[0] = pBuf;
	allocation[1] = pBuf + 32;

	scfsi[0] = pBuf + 64;
	scfsi[1] = pBuf + 96;

	nch = pHeader->channels;
	
	if (pHeader->version == MPEG2)
		index = 4;
	else if (stream->free_bitrates)
		goto freeformat;
	else 
	{
		unsigned long bitrate_per_channel;
		
		bitrate_per_channel = pHeader->bitrate;
		if (nch == 2) 
		{
			bitrate_per_channel >>= 1;			
		}
		else 
		{  
			if (bitrate_per_channel > 320000) 
			{
				return -1;
			}
		}
		
		if (bitrate_per_channel <= 48000)
			index = (pHeader->samplerate == 32000) ? 3 : 2;
		else if (bitrate_per_channel <= 80000)
			index = 0;
		else 
		{
freeformat:
		index = (pHeader->samplerate == 48000) ? 0 : 1;
		}
	}
	
	sbLimit = sb_table[index].sblimit;
	offsets = sb_table[index].offsets;
	
	bound = 32;
	if (pHeader->mode == MPA_MODE_JOINT_STEREO) 
	{
		bound = 4 + pHeader->modeext * 4;
	}
	
	if (bound > sbLimit)
		bound = sbLimit;

	pBit = &stream->bitptr;
	voMP3DecInitBits(pBit, stream->this_frame + pHeader->headLen, pHeader->framelen - pHeader->headLen);
	start = stream->bitptr;		
	nbits = 0;
	/* decode bit allocations */
	
	for (sBand = 0; sBand < bound; sBand++) 
	{
		nbal = bitalloc_table[offsets[sBand]].nbal;
		
		for (ch = 0; ch < nch; ch++)
		{
			allocation[ch][sBand] = voMP3DecGetBits(pBit, nbal);
		}
	}
	
	for (sBand = bound; sBand < sbLimit; sBand++)
	{
		nbal = bitalloc_table[offsets[sBand]].nbal;
		
		allocation[0][sBand] =
		allocation[1][sBand] = voMP3DecGetBits(pBit, nbal);
	}
	
	/* decode scalefactor selection info */
	for (sBand = 0; sBand < bound; sBand++) 
	{
		for (ch = 0; ch < nch; ch++) 
		{
			if (allocation[ch][sBand])
			{
				scfsi[ch][sBand] = voMP3DecGetBits(pBit, 2);
			}
		}
	}

	for (sBand = bound; sBand < sbLimit; sBand++) 
	{
		if(allocation[0][sBand]) 
		{			
			for (ch = 0; ch < nch; ch++) 
			{
				scfsi[ch][sBand] = voMP3DecGetBits(pBit, 2);
			}
		}		
	}
	
	/* check CRC word */	
	if (pHeader->crc) 
	{		
		pHeader->crc_check =
			voMP3DecBits_Crc(&start, voMP3DecCalcBitsUsed(pBit, &start), pHeader->crc_check);
		
		if (pHeader->crc_check != pHeader->crc_target) 
		{
			return -1;
		}
	}

	/* decode scalefactors */	
	for (sBand = 0; sBand < bound; sBand++) 
	{
		for (ch = 0; ch < nch; ch++) 
		{
			if (allocation[ch][sBand]) 
			{
				sf = scalefactor[ch][sBand];
				sf[0] = voMP3DecGetBits(pBit, 6);
		
				switch (scfsi[ch][sBand]) 
				{
				case 2:
					sf[2] = sf[1] = sf[0];
					break;
					
				case 0:
					sf[1] = voMP3DecGetBits(pBit, 6);
						
				case 1:
				case 3:
					sf[2] = voMP3DecGetBits(pBit, 6);
		
				}
				
				if (scfsi[ch][sBand] & 1)
					sf[1] = sf[scfsi[ch][sBand] - 1];
			}
		}
	}

	for (sBand = bound; sBand < sbLimit; sBand++) 
	{
		if (allocation[0][sBand]) 
		{
			for (ch = 0; ch < nch; ch++) 
			{
				sf = scalefactor[ch][sBand];
				sf[0] = voMP3DecGetBits(pBit, 6);
	
				switch (scfsi[ch][sBand]) 
				{
				case 2:
					sf[2] = sf[1] = sf[0];
					break;

				case 0:
					sf[1] = voMP3DecGetBits(pBit, 6);
			
				case 1:
				case 3:
					sf[2] = voMP3DecGetBits(pBit, 6);
		
				}

				if (scfsi[ch][sBand] & 1)
					sf[1] = sf[scfsi[ch][sBand] - 1];
			}
		}
	}

	/* decode samples */
	
	for (gran = 0; gran < 12; gran++) 
	{
		for (sBand = 0; sBand < bound; sBand++) 
		{
			for (ch = 0; ch < nch; ch++) 
			{
				if ((index = allocation[ch][sBand])) 
				{
					index = offset_table[bitalloc_table[offsets[sBand]].offset][index - 1];
					
					II_dequantize_samples(pBit, &qc_table[index], samples_qcout);
					
					for (sIndex = 0; sIndex < 3; sIndex++) 
					{
						frame->sbSample[ch][3 * gran + sIndex][sBand] =
							MUL_28(samples_qcout[sIndex], sf_table[scalefactor[ch][sBand][gran / 4]]);
					}
				}
				else
				{
					frame->sbSample[ch][3 * gran + 0][sBand] = 0;
					frame->sbSample[ch][3 * gran + 1][sBand] = 0;
					frame->sbSample[ch][3 * gran + 2][sBand] = 0;
				}
			}
		}
		
		for (sBand = bound; sBand < sbLimit; sBand++) 
		{
			if ((index = allocation[0][sBand])) 
			{
				index = offset_table[bitalloc_table[offsets[sBand]].offset][index - 1];
				
				II_dequantize_samples(pBit, &qc_table[index], samples_qcout);
				
				for (ch = 0; ch < nch; ch++) 
				{
					for (sIndex = 0; sIndex < 3; sIndex++)
					{
						frame->sbSample[ch][3 * gran + sIndex][sBand] =
							MUL_28(samples_qcout[sIndex], sf_table[scalefactor[ch][sBand][gran / 4]]);
					}
				}
			}
			else
			{
				for (ch = 0; ch < nch; ch++) 
				{
					frame->sbSample[ch][3 * gran + 0][sBand] = 0;
					frame->sbSample[ch][3 * gran + 1][sBand] = 0;
					frame->sbSample[ch][3 * gran + 2][sBand] = 0;
				}
			}
		}
		
		for (ch = 0; ch < nch; ch++) 
		{
			for (sBand = sbLimit; sBand < 32; sBand++) 
			{
				frame->sbSample[ch][3 * gran + 0][sBand] = 0;
				frame->sbSample[ch][3 * gran + 1][sBand] = 0;
				frame->sbSample[ch][3 * gran + 2][sBand] = 0;
			}
		}
	}	
	return VO_MP3_Mdoule_SUCCEED;
}
