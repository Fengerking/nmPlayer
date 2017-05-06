#include "frame.h"

static int voFLACDecResiDuals(SubFrame *nsubframe, Bitstream *bitptr, int blockSize)
{
	int method, partorder,partsize, riceparam;
	int nsamples, n, i, j;
	int *sample;

	sample = nsubframe->sample;
	
	method = voFLACDecGetBits(bitptr, 2);

	if(method > 1)
		return -1;
	
	partorder = voFLACDecGetBits(bitptr, 4);

	nsamples = blockSize >> partorder;

	partsize = 1 << partorder;

	if(nsamples < nsubframe->order)
		return -1;
	
	n = i = nsubframe->order;

	for(j = 0; j < partsize; j++)
	{
		//update for #7306 bug, lhp
		riceparam = voFLACDecGetBits(bitptr, method == 0 ? 4 : 5);
		if(riceparam == (method == 0 ? 15 : 31))
		{
			riceparam = voFLACDecGetBits(bitptr, 5);

			for( ; i < nsamples; i++, n++)
			{
				sample[n] = voFLACDecGetBits(bitptr, riceparam);
			}

		}
		else
		{
			for( ; i < nsamples; i++, n++)
			{
				sample[n] = voFLACDecGetRiceBits(bitptr, riceparam);
			}

		}
		i = 0;
	}
	
	return 0;
}

static int voFLACDecSubFix(SubFrame *nsubframe, Bitstream *bitptr, int bps, int blockSize)
{
	int ret, order, i;
	int a, b, c, d;
	int *sample;
	
	order = nsubframe->order;
	sample = nsubframe->sample;

	for(i = 0; i < order; i++)
	{
		sample[i] = voFLACDecGetsBits(bitptr, bps);
	}	
		
	ret = voFLACDecResiDuals(nsubframe, bitptr, blockSize);
	if(ret < 0)
        return -1;
	
    switch(order)
    {
        case 0:
            break;
        case 1:
			a = sample[order - 1];
            for (i = order; i < blockSize; i++)
                sample[i] = a += sample[i];
            break;
        case 2:
			a = sample[order - 1];
			b = a - sample[order - 2];
            for (i = order; i < blockSize; i++)
                sample[i] = a += b += sample[i];
            break;
        case 3:
			a = sample[order - 1];
			b = a - sample[order - 2];
			c = b - sample[order - 2] + sample[order - 3];
            for (i = order; i < blockSize; i++)
                sample[i] = a += b += c += sample[i];
            break;
        case 4:
			a = sample[order - 1];
			b = a - sample[order - 2];
			c = b - sample[order - 2] + sample[order - 3];
			d = c - sample[order - 2] + 2 *sample[order - 3] - sample[order - 4];
            for (i = order; i < blockSize; i++)
                sample[i] = a += b += c += d += sample[i];
            break;
        default:
            return -1;
    }

	return 0;	
}

static int voFLACDecSubLPC(SubFrame *nsubframe, Bitstream *bitptr, int bps, int blockSize)
{
	int i, j, n;
    int coefprec, qlevel, order;
	int ret, sum;
    int coefs[MAX_LPC_ORDER];
    int *sample, *psample;	
	
	order = nsubframe->order;
	sample = nsubframe->sample;

	for(n = 0; n < order; n++)
	{
		*sample++ = voFLACDecGetsBits(bitptr, bps);
	}	

	coefprec = voFLACDecGetBits(bitptr, 4) + 1;
	if(coefprec == 16)
		return -1;
	
	qlevel = voFLACDecGetsBits(bitptr, 5);
	if(qlevel < 0)
		return -1;
	
	for(i = 0; i < order; i++)
	{
		coefs[i] = voFLACDecGetsBits(bitptr, coefprec);
	}
	
	ret = voFLACDecResiDuals(nsubframe, bitptr, blockSize);
	if(ret < 0)
        return -1;

	if(bps <= 16) {
		for(i = order; i < blockSize; i++) {			
			sum = 0;
			psample = sample;
			for(j = 0; j < order; j++) {
				sum += coefs[j] * (*(--psample));
			}
			
			*sample = (*sample) + (sum >> qlevel);
			sample++;
		}
    }
	else
	{
        int64 sum64;
		for(i = order; i < blockSize; i++) {			
			sum64 = 0;
			psample = sample;
			for(j = 0; j < order; j++) {
				sum64 += (int64)coefs[j] * (*(--psample));
			}
			
			*sample = (*sample) + (int)(sum64 >> qlevel);
			sample++;
		}
	}
	
	return 0;
}


int voFLACDecodeSubFrame(FLACDecInfo *decoder, int nch)
{
	SubFrame	  *nsubframe;
	FrameStream   *stream;
	Bitstream     *bitptr;
	FrameHeader   *fHeader;
	int			  headdata, bps;
	int			  wasted_btis, gv;
	int			  *sample;
	int			  ret = 0, i;
	
	stream = decoder->stream;
	fHeader = &decoder->Fheader;
	bitptr = &stream->bitptr;
	nsubframe = &decoder->Fsubframe[nch];
	sample = nsubframe->sample;

	bps = fHeader->BitsperSample;

	switch(fHeader->ChanAssignment) {
	case INDEPENDENT:
		/* no adjustment needed */
		break;
	case LEFT_SIDE:
		if(nch == 1)
			bps++;
		break;
	case RIGHT_SIDE:
		if(nch == 0)
			bps++;
		break;
	case MID_SIDE:
		if(nch == 1)
			bps++;
		break;
	}

	headdata = voFLACDecGetBits(bitptr, 8);
	if(headdata & 0x80)	
		return -1;

	wasted_btis = (headdata & 1);	
	headdata >>= 1;

	if(wasted_btis)
	{
		wasted_btis = voFLACDecGetUnaryBits(bitptr) + 1;
		bps -= wasted_btis;
		nsubframe->WastedBits = wasted_btis;
	}
	else{
		nsubframe->WastedBits = 0;
	}

	if(headdata == 0)
	{
		nsubframe->SubType = SUBFRAME_CONSTANT;
		gv = voFLACDecGetsBits(bitptr, bps);
        for (i = 0; i < fHeader->BlockSize; i++)
            sample[i] = gv;
	}
	else if(headdata == 1)
	{
		nsubframe->SubType = SUBFRAME_VERBATIM;
		for (i = 0; i < fHeader->BlockSize; i++)
            sample[i] = voFLACDecGetsBits(bitptr, bps);
	}
	else if((headdata >= 8) && (headdata <= 12))
	{
		nsubframe->SubType = SUBFRAME_FIXED;
		nsubframe->order = (headdata & 0x7);
		
		ret = voFLACDecSubFix(nsubframe, bitptr, bps, fHeader->BlockSize);
		
	}
	else if(headdata >= 32)
	{
		nsubframe->SubType = SUBFRAME_LPC;
		nsubframe->order = (headdata & 0x1F) + 1;

		ret = voFLACDecSubLPC(nsubframe, bitptr, bps, fHeader->BlockSize);
	}
	else
	{
		return -1;
	}

	if (wasted_btis)
    {
        for (i = 0; i < fHeader->BlockSize; i++)
            sample[i] <<= wasted_btis;
    }

	return ret;
}
