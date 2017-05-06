#include "stream.h"
#include "frame.h"

static int sample_rate_table[] = { 
	0, 0, 0, 0,
	8000, 16000, 22050, 24000, 32000, 44100, 48000, 96000,
	0, 0, 0, 0 
};

static int sample_size_table[] = { 
	0, 8, 12, 0, 16, 20, 24, 0 
};

static int blocksize_table[] = {
	0,    192, 576<<0, 576<<1, 576<<2, 576<<3,      0,      0,
	256<<0, 256<<1, 256<<2, 256<<3, 256<<4, 256<<5, 256<<6, 256<<7
};

int	voFLACDecReadMetaInfo(FLACDecInfo *decoder, unsigned char* inbuf, int length)
{
	int ret, i;
	Bitstream bitpr;
	MStreamInfo *pMetaInfo;
	VO_MEM_OPERATOR *pMemOP;

	pMetaInfo = &decoder->MetaInfo;
	pMemOP    = decoder->pvoMemop;

	voFLACDecInitBits(&bitpr, inbuf, length);

	pMetaInfo->MinBlockSize = voFLACDecGetBits(&bitpr, 16);
	pMetaInfo->MaxBlockSize = voFLACDecGetBits(&bitpr, 16);
	pMetaInfo->MinFrameSize = voFLACDecGetBits(&bitpr, 24);
	pMetaInfo->MaxFrameSize = voFLACDecGetBits(&bitpr, 24);
	pMetaInfo->SampleRate   = voFLACDecGetBits(&bitpr, 20);
	pMetaInfo->Channels		= voFLACDecGetBits(&bitpr, 3) + 1;
	pMetaInfo->BitsPerSample= voFLACDecGetBits(&bitpr, 5) + 1;

	voFLACDecSkipBits(&bitpr, 36);

	for(i = 0; i < 16; i++)
	{
		pMetaInfo->md5sum[i] = voFLACDecGetBits(&bitpr, 8);
	}

	if(pMetaInfo->MaxFrameSize == 0 && pMetaInfo->MaxBlockSize){
		pMetaInfo->MaxFrameSize= (pMetaInfo->Channels * pMetaInfo->BitsPerSample * 
			pMetaInfo->MaxBlockSize + 7) / 8; 
	}
	else if(pMetaInfo->MaxFrameSize == 0 && pMetaInfo->MaxBlockSize == 0)
	{
		pMetaInfo->MaxFrameSize = 32768;
	}

	ret = voFLACDecInitStreamBuf(decoder->stream, pMetaInfo->MaxFrameSize, pMemOP);
	if(ret < 0)
		return ret;

	decoder->samplerate = pMetaInfo->SampleRate;
	decoder->channel = pMetaInfo->Channels;

	return 0;
}

int voFLACDecUpdateFrameBuffer(SubFrame *fsubframe, int blockSize, VO_MEM_OPERATOR *pMemOP)
{
	if(fsubframe->blockSize < blockSize)
	{
		if(fsubframe->sample)
			voFLACDecmem_free(pMemOP, fsubframe->sample);

		fsubframe->sample = (int *)voFLACDecmem_malloc(pMemOP, blockSize*sizeof(int), 32);
		if(fsubframe->sample == NULL)
			return -1;

		fsubframe->blockSize = blockSize;
	}
	return 0;
}

int	 voFLACDecHeaderSync(unsigned char *buf, int nbytes)
{
	int i;
	if(nbytes < BUFFER_GUARD)
		return -1;

	for (i = 0; i < nbytes - BUFFER_GUARD; i++) {
		if ( buf[i+0] == 0xff && (buf[i+1] & 0xfc) == 0xf8)
			return i;
	}

	return -1;
}

static int voFLACDecodeHeaderinfo(FLACDecInfo* decoder)
{
	int64 hlen;	
	MStreamInfo	  *fMetaInfo;
	FrameHeader   *fHeader;
	FrameStream   *stream;
	Bitstream     *bitptr;
	int vBolck, blockSize, samplerate;
	int bitsps, chassign;
	int len; 

	stream = decoder->stream;
	fHeader = &decoder->Fheader;
	bitptr = &stream->bitptr;	
	fMetaInfo = &decoder->MetaInfo;

	voFLACDecSkipBits(bitptr, 14);	
	len		   = voFLACDecGetBits(bitptr, 1);
	vBolck	   = voFLACDecGetBits(bitptr, 1);

	blockSize  = voFLACDecGetBits(bitptr, 4);
	samplerate = voFLACDecGetBits(bitptr, 4);
	chassign   = voFLACDecGetBits(bitptr, 4);
	bitsps     = voFLACDecGetBits(bitptr, 3);
	len		   = voFLACDecGetBits(bitptr, 1);


	hlen  = voFLACDecGetUtf8(bitptr);

	if(hlen < 0)
		return -1;

	fHeader->nDecNum = hlen;

	if(bitsps == 0)
		fHeader->BitsperSample = fMetaInfo->BitsPerSample;
	else if((bitsps != 3) && (bitsps != 7))
		fHeader->BitsperSample = sample_size_table[bitsps];
	else
		return -1;

	if (chassign < 8 && fMetaInfo->Channels == chassign+1)
	{
		fHeader->ChanAssignment = INDEPENDENT;
		fHeader->Channels = fMetaInfo->Channels;
	}
	else if (chassign >=8 && chassign < 11 && fMetaInfo->Channels == 2)
	{
		fHeader->ChanAssignment = LEFT_SIDE + chassign - 8;
		fHeader->Channels = fMetaInfo->Channels;
	}
	else
	{
		return -1;
	}

	if (blockSize == 0)
		fHeader->BlockSize = fMetaInfo->MinBlockSize;
	else if (blockSize == 6)
		fHeader->BlockSize = voFLACDecGetBits(bitptr, 8) + 1;
	else if (blockSize == 7)
		fHeader->BlockSize = voFLACDecGetBits(bitptr, 16) + 1;
	else
		fHeader->BlockSize = blocksize_table[blockSize];


	if (fHeader->BlockSize < fMetaInfo->MinBlockSize || fHeader->BlockSize > fMetaInfo->MaxBlockSize)
	{
		return -1;
	}
	if (samplerate == 0){
		fHeader->SampleRate = fMetaInfo->SampleRate;
	}
	else if((samplerate > 3) && (samplerate < 12))
		fHeader->SampleRate = sample_rate_table[samplerate];
	else if (samplerate == 12)
		fHeader->SampleRate = voFLACDecGetBits(bitptr, 8) * 1000;
	else if (samplerate == 13)
		fHeader->SampleRate = voFLACDecGetBits(bitptr, 16);
	else if (samplerate == 14)
		fHeader->SampleRate = voFLACDecGetBits(bitptr, 16) * 10;
	else{
		return -1;
	}

	fHeader->Crc = voFLACDecGetBits(bitptr, 8);

	return 0;
}

int voFLACDecHeaderDecode(FLACDecInfo *decoder)
{
	unsigned char *ptr, *end, *ptr_bk;
	FrameStream   *stream;
	Bitstream     *bitptr;
	Bitstream     *Nextbitptr;
	FrameHeader   *fHeader;
	int len, hlen; 

	stream = decoder->stream;
	fHeader = &decoder->Fheader;
	bitptr = &stream->bitptr;
	Nextbitptr = &stream->PostBitptr;

	ptr = stream->buffer;
	end = stream->bufend;
	hlen = 0;

	do{
		if(hlen < 0) ptr++;
		len  = 	voFLACDecHeaderSync(ptr, end - ptr);
		if(len < 0)
		{
			if(end - ptr > decoder->MetaInfo.MaxFrameSize)
				stream->buffer = end - BUFFER_GUARD;
			return -1;
		}

		ptr += len;
		stream->buffer = ptr; 

		if(end - ptr < decoder->MetaInfo.MinFrameSize)
			return -1;	

		voFLACDecInitBits(bitptr, ptr, end - ptr);

		hlen = voFLACDecodeHeaderinfo(decoder);
	}while(hlen < 0);

	ptr_bk = ptr;
	do
	{
		ptr++;
		len  = 	voFLACDecHeaderSync(ptr, end - ptr);
		if(len < 0)
		{
			if(end - stream->buffer > decoder->MetaInfo.MaxFrameSize)
				stream->buffer = end - BUFFER_GUARD;
			return -1;
		}

		ptr += len;

		if(ptr[1] == ptr_bk[1] && ptr[2] == ptr_bk[2] && ((ptr[3] & 0x0f) == (ptr_bk[3] & 0x0f)))
		{
			voFLACDecInitBits(Nextbitptr, ptr+4, end - ptr - 4);
			if ((fHeader->nDecNum + 1) == voFLACDecGetUtf8(Nextbitptr))
			{
				break;
			}
		}

	}while(1);

	return hlen;
}

int voFLACDecChanAssignMix(FLACDecInfo *decoder, short *samples)
{
	FrameHeader	 *fHeader;	
	int	 *lsamples1, *lsamples2;
	short *ssamples;
	int  nch, nbits, i;

	fHeader = &decoder->Fheader;

	nbits = 24 - fHeader->BitsperSample;

	if(nbits == 8)
	{
		switch(fHeader->ChanAssignment)
		{
		case INDEPENDENT:
			for (nch = 0; nch < decoder->channel; nch++)
			{
				lsamples1 = decoder->Fsubframe[nch].sample;
				ssamples = samples + nch;
				for(i = 0; i < fHeader->BlockSize; i++)
				{
					*ssamples = (short)lsamples1[i];
					ssamples += decoder->channel;
				}
			}
			break;
		case LEFT_SIDE:
			if(decoder->channel == 2)
			{
				lsamples1 = decoder->Fsubframe[0].sample;
				lsamples2 = decoder->Fsubframe[1].sample;

				ssamples = samples;

				for(i = 0; i < fHeader->BlockSize; i++)
				{
					*ssamples++ = (short)(lsamples1[i]);
					*ssamples++ = (short)(lsamples1[i] - lsamples2[i]);
				}
			}
			else
			{
				for (nch = 0; nch < decoder->channel; nch++)
				{
					lsamples1 = decoder->Fsubframe[nch].sample;
					ssamples = samples;
					for(i = 0; i < fHeader->BlockSize; i++)
					{
						*ssamples = (short)lsamples1[i];
						ssamples += decoder->channel;
					}
				}
			}
			break;
		case RIGHT_SIDE:
			if(decoder->channel == 2)
			{
				lsamples1 = decoder->Fsubframe[0].sample;
				lsamples2 = decoder->Fsubframe[1].sample;

				ssamples = samples;

				for(i = 0; i < fHeader->BlockSize; i++)
				{
					*ssamples++ = (short)(lsamples1[i] + lsamples2[i]);
					*ssamples++ = (short)(lsamples2[i]);
				}
			}
			else
			{
				for (nch = 0; nch < decoder->channel; nch++)
				{
					lsamples1 = decoder->Fsubframe[nch].sample;
					ssamples = samples;
					for(i = 0; i < fHeader->BlockSize; i++)
					{
						*ssamples = (short)(lsamples1[i]);
						ssamples += decoder->channel;
					}
				}
			}
			break;
		case MID_SIDE:
			if(decoder->channel == 2)
			{
				int mid, side;
				lsamples1 = decoder->Fsubframe[0].sample;
				lsamples2 = decoder->Fsubframe[1].sample;

				ssamples = samples;

				for(i = 0; i < fHeader->BlockSize; i++)
				{
					mid = lsamples1[i];
					side = lsamples2[i];
					mid <<= 1;
					if(side & 1)
						mid++;
					*ssamples++ = (mid + side)>>1;
					*ssamples++ = (mid - side)>>1;

					//side = (lsamples2[i] + 1) >> 1;
					//mid = lsamples1[i] - side;
					//side = mid + lsamples2[i];
					//*ssamples++ = (short)side;
					//*ssamples++ = (short)mid ;
				}
			}
			else
			{
				for (nch = 0; nch < decoder->channel; nch++)
				{
					lsamples1 = decoder->Fsubframe[nch].sample;
					ssamples = samples;
					for(i = 0; i < fHeader->BlockSize; i++)
					{
						*ssamples = (short)(lsamples1[i]);
						ssamples += decoder->channel;
					}
				}
			}
			break;
		default:
			return -1;
		}	
	}
	else
	{
		switch(fHeader->ChanAssignment)
		{
		case INDEPENDENT:
			for (nch = 0; nch < decoder->channel; nch++)
			{
				lsamples1 = decoder->Fsubframe[nch].sample;
				ssamples = samples + nch;
				for(i = 0; i < fHeader->BlockSize; i++)
				{
					*ssamples = (short)((lsamples1[i] << nbits) >> 8);
					ssamples += decoder->channel;
				}
			}
			break;
		case LEFT_SIDE:
			if(decoder->channel == 2)
			{
				lsamples1 = decoder->Fsubframe[0].sample;
				lsamples2 = decoder->Fsubframe[1].sample;

				ssamples = samples;

				for(i = 0; i < fHeader->BlockSize; i++)
				{
					*ssamples++ = (short)((lsamples1[i] << nbits) >> 8);
					*ssamples++ = (short)(((lsamples1[i] - lsamples2[i]) << nbits) >> 8);
				}
			}
			else
			{
				for (nch = 0; nch < decoder->channel; nch++)
				{
					lsamples1 = decoder->Fsubframe[nch].sample;
					ssamples = samples;
					for(i = 0; i < fHeader->BlockSize; i++)
					{
						*ssamples = (short)((lsamples1[i] << nbits) >> 8);
						ssamples += decoder->channel;
					}
				}
			}
			break;
		case RIGHT_SIDE:
			if(decoder->channel == 2)
			{
				lsamples1 = decoder->Fsubframe[0].sample;
				lsamples2 = decoder->Fsubframe[1].sample;

				ssamples = samples;

				for(i = 0; i < fHeader->BlockSize; i++)
				{
					*ssamples++ = (short)(((lsamples1[i] + lsamples2[i]) << nbits) >> 8);
					*ssamples++ = (short)((lsamples2[i] << nbits) >> 8);
				}
			}
			else
			{
				for (nch = 0; nch < decoder->channel; nch++)
				{
					lsamples1 = decoder->Fsubframe[nch].sample;
					ssamples = samples;
					for(i = 0; i < fHeader->BlockSize; i++)
					{
						*ssamples = (short)((lsamples1[i] << nbits) >> 8);
						ssamples += decoder->channel;
					}
				}
			}
			break;
		case MID_SIDE:
			if(decoder->channel == 2)
			{
				int mid, side;
				lsamples1 = decoder->Fsubframe[0].sample;
				lsamples2 = decoder->Fsubframe[1].sample;

				ssamples = samples;

				for(i = 0; i < fHeader->BlockSize; i++)
				{
					side = (lsamples2[i] + 1) >> 1;
					mid = lsamples1[i] - side;
					side = mid + lsamples2[i];
					*ssamples++ = (short)((side << nbits) >> 8);
					*ssamples++ = (short)((mid << nbits) >> 8);
				}
			}
			else
			{
				for (nch = 0; nch < decoder->channel; nch++)
				{
					lsamples1 = decoder->Fsubframe[nch].sample;
					ssamples = samples;
					for(i = 0; i < fHeader->BlockSize; i++)
					{
						*ssamples = (short)((lsamples1[i] << nbits) >> 8);
						ssamples += decoder->channel;
					}
				}
			}
			break;
		default:
			return -1;
		}	
	}

	return 0;
}
