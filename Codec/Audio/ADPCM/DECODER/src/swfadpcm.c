/********************************************************************
* Copyright 2003~2011 by VisualOn Software, Inc.
* All modifications are confidential and proprietary information
* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
*********************************************************************
* File Name: 
*            swfadpcm.c
*
* Project:
* contents/description: swf adpcm decoder (ShockWave)
*            
***************************** Change History**************************
* 
*    DD/MMM/YYYY     Code Ver     Description             Author
*    -----------     --------     -----------             ------
*    08-01-2011        1.0        File imported from      Huaping Liu
*                                             
**********************************************************************/

#include "Frame.h"
static const int swf_index_tables[4][16] = {
	/* 2 */{-1, 2},
	/* 3 */{-1, -1, 2, 4},
	/* 4 */{-1, -1, -1, -1, 2, 4, 6, 8},
	/*16 */{-1, -1, -1, -1, -1, -1, -1, -1, 1, 2, 4, 6, 8, 10, 13, 16}
};

typedef struct {
	unsigned char *byte;
	unsigned int  cache;
	int   bitsleft;
	int   nbytes;	
}Bitstream;

static void voInitBits(Bitstream *bitptr, unsigned char *byte, unsigned int length)
{
	bitptr->byte  = byte;
	bitptr->cache = 0;
	bitptr->bitsleft  = 0;
	bitptr->nbytes = length;
}

static __inline void Reflush(Bitstream *bitptr)
{
	int nBytes = bitptr->nbytes;

	if (nBytes >= 4) {
		bitptr->cache  = (*bitptr->byte++) << 24;
		bitptr->cache |= (*bitptr->byte++) << 16;
		bitptr->cache |= (*bitptr->byte++) <<  8;
		bitptr->cache |= (*bitptr->byte++);
		bitptr->bitsleft = 32;
		bitptr->nbytes -= 4;
	} else {
		bitptr->cache = 0;
		while (nBytes-- > 0) {
			bitptr->cache |= (*bitptr->byte++);
			bitptr->cache <<= 8;
		}
		bitptr->cache <<= ((3 - bitptr->nbytes)*8);
		bitptr->bitsleft = 8*bitptr->nbytes;
		bitptr->nbytes = 0;
	}
}

static unsigned int voGetBits(Bitstream *bitptr, int nBits)
{
	unsigned int data, lowBits;

	nBits &= 0x1f;						
	data = bitptr->cache >> (31 - nBits);	
	data >>= 1;							
	bitptr->cache <<= nBits;				
	bitptr->bitsleft -= nBits;			

	if (bitptr->bitsleft < 0) {
		lowBits = -bitptr->bitsleft;
		Reflush(bitptr);
		data |= bitptr->cache >> (32 - lowBits);

		bitptr->bitsleft -= lowBits;			
		bitptr->cache <<= lowBits;			
	}

	return data;
}

int voADPCMDecSWFADPCMDec(ADPCMDecInfo *decoder, short* samples)
{
	const int *table;
	int  k0, signmask, nb_bits;
	int  chs, chan, lens;
	int  count;
	int  delta, step, k, vpdiff;
	int  predictor[2];
	int  step_index[2];
	int  leftbits;

	unsigned char *buf;
	Bitstream bitptr;

	buf = decoder->stream->this_frame;
	lens = decoder->stream->length;
	chs = decoder->channel;

	voInitBits(&bitptr, buf, lens);
	/* swf Adpcmcodesize UB(2):
	   0 = 2bits/sample 
	   1 = 3bits/sample 
	   2 = 4bits/sample
	   3 = 5bits/sample
	   Bits per ADPCM code less 2. The actual size of each code 
	   is Adpcmcodesize + 2.
	*/
	nb_bits = voGetBits(&bitptr, 2) + 2;

	table = swf_index_tables[nb_bits - 2];
	k0 = 1 << (nb_bits - 2);
	signmask = 1 << (nb_bits - 1);

    /* get first sample and initial index into the ADPCM StepSize Table */
	for (chan = 0; chan < chs; chan++)
	{
		/* First sample, identical to first sample in uncompressed sound */
		*samples++ = predictor[chan] = voGetBits(&bitptr, 16); 
		if (predictor[chan] & 0x8000)
			predictor[chan] -= 0x10000 ;

		step_index[chan] = voGetBits(&bitptr, 6);
	}

    leftbits = (bitptr.nbytes << 3) + bitptr.bitsleft;

	for (count = 0; leftbits >= nb_bits * chs && count < decoder->framesperblock; count++)
	{
		for (chan = 0; chan < chs; chan++)
		{
			vpdiff = 0;
			k = k0;
			delta = voGetBits(&bitptr, nb_bits);  // get one sample delta
			step  = step_table[step_index[chan]];

            do 
            {
				if (delta & k)
				{
					vpdiff += step;
				}
				step >>= 1;
				k >>= 1;
            } while (k);

			if (delta & signmask)
			{
				predictor[chan] -= vpdiff;
			}
			else
			{
				predictor[chan] += vpdiff;
			}

			step_index[chan] += table[delta & (~signmask)];

			if (step_index[chan] < 0)
			{
				step_index[chan] = 0;
			}
			else if (step_index[chan] > 88)
			{
				step_index[chan] = 88;
			}

			if (predictor[chan] > 32767)
			{
				predictor[chan] = 32767;
			}
			else if (predictor[chan] < -32768)
			{
				predictor[chan] = -32768;
			}

			*samples++ = predictor[chan];
		}
		leftbits = (bitptr.nbytes << 3) + bitptr.bitsleft;
	}

	while(count < decoder->framesperblock)
	{
		*samples++ = 0;
		count++;
	}

	return 0;
}

