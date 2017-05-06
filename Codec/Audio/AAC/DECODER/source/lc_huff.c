    /************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#include "struct.h"
#include "decoder.h"
#include "volc_huff.h"

static int HuffmanSignedQuads(BitStream *bs, int cb, int nSpec, int *coef)
{
	int w, x, y, z;
	int maxBits, nCodeBits, nSkipBits, HufVal;
	unsigned int bitCache;
	HuffInfoData* huffInfo;
	short *HufTab, *sHufTab;

	huffInfo = (HuffInfoData *)(huffTabSpecInfo + cb - 1);

	maxBits = huffInfo->maxBits;
	sHufTab = (short *)huffInfo->HufTab;

	while (nSpec > 0) {

		bitCache = BitStreamShowBits(bs, maxBits) << (32 - maxBits);

		HufTab = sHufTab;
		nSkipBits = huffInfo->startBits;
		nCodeBits = 0;
		HufVal = HufTab[bitCache >> (32 - nSkipBits)];

		while(GET_OFFBIT(HufVal))
		{
			bitCache <<= nSkipBits;
			nCodeBits += nSkipBits;
			HufTab = sHufTab + GET_OFFSET(HufVal);
			nSkipBits =	GET_BITS(HufVal);		
			HufVal = HufTab[bitCache >> (32 - nSkipBits)];
		}	

		nCodeBits += GET_BITS(HufVal);	

		w = GET_QUAD_W(HufVal);
		x = GET_QUAD_X(HufVal);
		y = GET_QUAD_Y(HufVal);
		z = GET_QUAD_Z(HufVal);

		*coef++ = w; *coef++ = x; *coef++ = y; *coef++ = z;
		nSpec -= 4;

		BitStreamSkip(bs, nCodeBits);
	}

	return 0;
}

static int HuffmanUnSignedQuads(BitStream *bs, int cb, int nSpec, int *coef)
{
	int w, x, y, z;
	int maxBits, nCodeBits, nSkipBits, HufVal;
	unsigned int bitCache;
	HuffInfoData* huffInfo;
	const short *HufTab;
	const short *sHufTab;

	huffInfo = (HuffInfoData *)(huffTabSpecInfo + cb - 1);

	maxBits = huffInfo->maxBits + 4;
	sHufTab = huffInfo->HufTab;

	while (nSpec > 0) {

		bitCache = BitStreamShowBits(bs, maxBits) << (32 - maxBits);

		HufTab = sHufTab;
		nSkipBits = huffInfo->startBits;
		nCodeBits = 0;
		HufVal = HufTab[bitCache >> (32 - nSkipBits)];

		while(GET_OFFBIT(HufVal))
		{
			bitCache <<= nSkipBits;
			nCodeBits += nSkipBits;
			HufTab = sHufTab + GET_OFFSET(HufVal);
			nSkipBits =	GET_BITS(HufVal);		
			HufVal = HufTab[bitCache >> (32 - nSkipBits)];
		}	

		nSkipBits = GET_BITS(HufVal);
		nCodeBits += nSkipBits;
		bitCache <<= nSkipBits;

		w = GET_QUAD_W(HufVal);
		x = GET_QUAD_X(HufVal);
		y = GET_QUAD_Y(HufVal);
		z = GET_QUAD_Z(HufVal);

		if(w) { SET_SIGN(w, bitCache); bitCache <<= 1; nCodeBits++; }
		if(x) { SET_SIGN(x, bitCache); bitCache <<= 1; nCodeBits++; }
		if(y) { SET_SIGN(y, bitCache); bitCache <<= 1; nCodeBits++; }
		if(z) { SET_SIGN(z, bitCache); bitCache <<= 1; nCodeBits++; }

		*coef++ = w; *coef++ = x; *coef++ = y; *coef++ = z;
		nSpec -= 4;

		BitStreamSkip(bs, nCodeBits);
	}

	return 0;
}

static int HuffmanSignedPairs(BitStream *bs, int cb, int nSpec, int *coef)
{
	int y, z;
	int maxBits, nCodeBits, nSkipBits, HufVal;
	unsigned int bitCache;
	HuffInfoData* huffInfo;
	short *HufTab, *sHufTab;

	huffInfo = (HuffInfoData *)(huffTabSpecInfo + cb - 1);

	maxBits = huffInfo->maxBits;
	sHufTab = (short *)huffInfo->HufTab;

	while (nSpec > 0) {
		
		bitCache = BitStreamShowBits(bs, maxBits) << (32 - maxBits);

		HufTab = sHufTab;
		nSkipBits = huffInfo->startBits;
		nCodeBits = 0;
		HufVal = HufTab[bitCache >> (32 - nSkipBits)];

		while(GET_OFFBIT(HufVal))
		{
			bitCache <<= nSkipBits;
			nCodeBits += nSkipBits;
			HufTab = sHufTab + GET_OFFSET(HufVal);
			nSkipBits =	GET_BITS(HufVal);		
			HufVal = HufTab[bitCache >> (32 - nSkipBits)];
		}	

		nCodeBits += GET_BITS(HufVal);	

		y = GET_PAIR_Y(HufVal);
		z = GET_PAIR_Z(HufVal);

		*coef++ = y; *coef++ = z;
		nSpec -= 2;

		BitStreamSkip(bs, nCodeBits);
	}

	return 0;
}

static int HuffmanUnSignedPairs(BitStream *bs, int cb, int nSpec, int *coef)
{
	int y, z;
	int maxBits, nCodeBits, nSkipBits, HufVal;
	unsigned int bitCache;
	HuffInfoData* huffInfo;
	const short *HufTab, *sHufTab;

	huffInfo = (HuffInfoData *)(huffTabSpecInfo + cb - 1);

	maxBits = huffInfo->maxBits + 2;
	sHufTab = huffInfo->HufTab;

	while (nSpec > 0) {
		
		bitCache = BitStreamShowBits(bs, maxBits) << (32 - maxBits);

		HufTab = sHufTab;
		nSkipBits = huffInfo->startBits;
		nCodeBits = 0;
		HufVal = HufTab[bitCache >> (32 - nSkipBits)];

		while(GET_OFFBIT(HufVal))
		{
			bitCache <<= nSkipBits;
			nCodeBits += nSkipBits;
			HufTab = sHufTab + GET_OFFSET(HufVal);
			nSkipBits =	GET_BITS(HufVal);		
			HufVal = HufTab[bitCache >> (32 - nSkipBits)];
		}	

		nSkipBits = GET_BITS(HufVal);
		nCodeBits += nSkipBits;
		bitCache <<= nSkipBits;

		y = GET_PAIR_Y(HufVal);
		z = GET_PAIR_Z(HufVal);

		if(y) { SET_SIGN(y, bitCache); bitCache <<= 1; nCodeBits++; }
		if(z) { SET_SIGN(z, bitCache); bitCache <<= 1; nCodeBits++; }

		*coef++ = y; *coef++ = z;
		nSpec -= 2;

		BitStreamSkip(bs, nCodeBits);
	}

	return 0;
}

static int HuffmanEscPairs(BitStream *bs, int cb, int nSpec, int *coef)
{
	int y, z, n;
	int maxBits, nCodeBits, nSkipBits, HufVal;
	unsigned int bitCache;
	HuffInfoData* huffInfo;
	const short *HufTab, *sHufTab;

	huffInfo = (HuffInfoData *)(huffTabSpecInfo + cb - 1);

	maxBits = huffInfo->maxBits + 2;
	sHufTab = huffInfo->HufTab;

	while (nSpec > 0) {

		bitCache = BitStreamShowBits(bs, maxBits) << (32 - maxBits);

		HufTab = sHufTab;
		nSkipBits = huffInfo->startBits;
		nCodeBits = 0;
		HufVal = HufTab[bitCache >> (32 - nSkipBits)];

		while(GET_OFFBIT(HufVal))
		{
			bitCache <<= nSkipBits;
			nCodeBits += nSkipBits;
			HufTab = sHufTab + GET_OFFSET(HufVal);
			nSkipBits =	GET_BITS(HufVal);		
			HufVal = HufTab[bitCache >> (32 - nSkipBits)];
		}	
		
		nSkipBits = GET_BITS(HufVal);
		nCodeBits += nSkipBits;
		bitCache <<= nSkipBits;

		y = GET_ESC_Y(HufVal);
		z = GET_ESC_Z(HufVal);

		if(y) { nCodeBits++; }
		if(z) { nCodeBits++; }

		BitStreamSkip(bs, nCodeBits);

		if (y == 16) {
			n = 4;
			while (BitStreamGetBits(bs, 1) == 1)
				n++;
			y = (1 << n) + BitStreamGetBits(bs, n);
		}

		if (z == 16) {
			n = 4;
			while (BitStreamGetBits(bs, 1) == 1)
				n++;
			z = (1 << n) + BitStreamGetBits(bs, n);
		}

		if(y) { SET_SIGN(y, bitCache); bitCache <<= 1;}
		if(z) { SET_SIGN(z, bitCache); bitCache <<= 1;}

		*coef++ = y; *coef++ = z;
		nSpec -= 2;
	}

	return 0;
}

static int SetZeros(int nSpec, int *coef)
{
	while (nSpec > 0) {
		*coef++ = 0;
		*coef++ = 0;
		*coef++ = 0;
		*coef++ = 0;
		nSpec -= 4;
	}

	return 0;
}

int spectral_data(AACDecoder* decoder,ICS_Data *ics,int ch)
{
	BitStream *bs = &decoder->bs;	
	int ret, g, cb, numsfb, w, offset, sfb;
	int window_group_length,max_sfb,num_window_groups;
	unsigned char *sfb_cb;
	short *sfb_offset;
	int *spec, *spec1;	

	spec = decoder->coef[ch];
	sfb_cb = decoder->sfb_cb[ch];
	max_sfb = ics->max_sfb;
	spec1 = spec;
	ret = 0;
	
	if (ics->window_sequence != EIGHT_SHORT_SEQUENCE) {
		sfb_offset = (short*)(sfBandTabLong + sfBandTabLongOffset[decoder->sampRateIdx]);

		for (sfb = 0; sfb < max_sfb; sfb++) {
			numsfb = sfb_offset[sfb+1] - sfb_offset[sfb];
			cb = *sfb_cb++;

			switch(cb)
			{
			case 1:
			case 2:
				ret = HuffmanSignedQuads(bs, cb, numsfb, spec);
				break;
			case 3:
			case 4:
				ret = HuffmanUnSignedQuads(bs, cb, numsfb, spec);
				break;
			case 5:
			case 6:
				ret = HuffmanSignedPairs(bs, cb, numsfb, spec);
				break;
			case 7:
			case 8:
			case 9:
			case 10:
				ret = HuffmanUnSignedPairs(bs, cb, numsfb, spec);
				break;
			case 11:
				ret = HuffmanEscPairs(bs, cb, numsfb, spec);
				break;
			default:
				ret = SetZeros(numsfb, spec);
				break;
			}

			if(ret) return -1;

			spec += numsfb;
		}

		numsfb = SIZE_LONG_WIN - sfb_offset[sfb];
		SetZeros(numsfb, spec);

		if (decoder->pulse_data_present[ch]) {
			Pulse_Data* pulse = &decoder->pulse_data[ch];
			int i,number_pulse = pulse->number_pulse+1;
			spec = decoder->coef[ch];
			offset = sfb_offset[pulse->pulse_start_sfb];
			for (i = 0; i < number_pulse; i++) {
				offset += pulse->pulse_offset[i];
				if(offset >= SIZE_LONG_WIN)
				{
					return VO_ERR_AAC_INVSTREAM;
				}

				if (spec[offset] > 0)
					spec[offset] += pulse->pulse_amp[i];
				else
					spec[offset] -= pulse->pulse_amp[i];
			}
		}
	}
	else
	{
		sfb_offset = (short*)(sfBandTabShort + sfBandTabShortOffset[decoder->sampRateIdx]);
		num_window_groups = ics->num_window_groups;
		for (g = 0; g < num_window_groups; g++) {
			window_group_length = ics->window_group_length[g];
			for (sfb = 0; sfb < max_sfb; sfb++) {
				numsfb = sfb_offset[sfb+1] - sfb_offset[sfb];
				cb = *sfb_cb++;

				offset = 0;
				for (w = 0; w <window_group_length ; w++) {	
					switch(cb)
					{
					case 1:
					case 2:
						ret = HuffmanSignedQuads(bs, cb, numsfb, spec + offset);
						break;
					case 3:
					case 4:
						ret = HuffmanUnSignedQuads(bs, cb, numsfb, spec + offset);
						break;
					case 5:
					case 6:
						ret = HuffmanSignedPairs(bs, cb, numsfb, spec + offset);
						break;
					case 7:
					case 8:
					case 9:
					case 10:
						ret = HuffmanUnSignedPairs(bs, cb, numsfb, spec + offset);
						break;
					case 11:
						ret = HuffmanEscPairs(bs, cb, numsfb, spec + offset);
						break;
					default:
						ret = SetZeros(numsfb, spec + offset);
						break;
					}

					if(ret) return -1;

					offset += SIZE_SHORT_WIN;
				}

				spec += numsfb;
			}

			for (w = 0; w <window_group_length ; w++) {
				offset = w*SIZE_SHORT_WIN;
				numsfb = SIZE_SHORT_WIN - sfb_offset[sfb];
				SetZeros(numsfb, spec + offset);
			}

			spec += SIZE_SHORT_WIN - sfb_offset[sfb];
			spec += (window_group_length - 1)*SIZE_SHORT_WIN;
		}
	}

	return 0;
}
