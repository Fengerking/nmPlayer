#include "Frame.h"

static int find_bitrate_index(int type, int bitrate)
{
	int i;
	
	for(i=0;i<14;i++) {
		if(bitrate >= bitr_index[type][i] && bitrate < bitr_index[type][i+1])
			break;
	}
	
	return i;
}

static int find_samplerate_index(long freq, int *mp3_type)
{                                
	int    mpg, rate;
	
	*mp3_type = 1;
	
	for(mpg=0; mpg<2; mpg++) {
		for(rate=0; rate<3; rate++) {
			if(freq == samTab[mpg][rate])
			{ 
				*mp3_type = mpg; 
				return rate; 
			}
		}
	}
	
	return -1;
}

int encInit(MP3EncInfo *encInfo, int nch, int samplerate, int bitrate)
{
	FrameInfo *frameInfo;
	HeadInfo *header;
	int *factScale;
	int avg_byte_per_frame;

	frameInfo = encInfo->frame;
	header = &(frameInfo->header);

	nch = frameInfo->channels;
	samplerate = frameInfo->samplerate;
	bitrate = frameInfo->bitrate;

	if(nch == 0 || nch > 2)
		return VO_ERR_AUDIO_UNSCHANNEL;
	
	header->mode = (nch == 2) ? 0 : 3;
	header->smpl_id = find_samplerate_index(samplerate, &header->type);
	if(header->smpl_id < 0)
	{
		return VO_ERR_AUDIO_UNSSAMPLERATE;
	}

	frameInfo->ngrans  = 2;

	header->bitr_id = find_bitrate_index(header->type, bitrate);
	if(header->bitr_id == 0 || header->bitr_id == 14)
	{
		bitrate = frameInfo->bitrate = 64*nch;
		header->bitr_id = find_bitrate_index(header->type, bitrate);		
	}
	
	frameInfo->num_bands = num_bands[(nch == 2) ? header->type : 2][header->bitr_id];
	frameInfo->ResvSize = 0;

	avg_byte_per_frame = SAMPL2 * 16000 * bitrate / (2 - header->type);
	avg_byte_per_frame = avg_byte_per_frame / samplerate;
	encInfo->byte_per_frame = avg_byte_per_frame >> 6;
	encInfo->frac_per_frame = avg_byte_per_frame & 63;
	encInfo->slot_lag  = 0;
	encInfo->enclen = (512 + SAMP_PER_FRAME)*nch;
	frameInfo->si_len  = 32 + (header->type ? (nch == 1 ? 136 : 256)
		: (nch == 1 ?  72 : 136));
	
	factScale = (int *)sfBand[header->smpl_id + 3*header->type];

	frameInfo->SideInfoPS.sis[0][0].factScale = factScale;
	frameInfo->SideInfoPS.sis[0][1].factScale = factScale;
	frameInfo->SideInfoPS.sis[1][0].factScale = factScale;
	frameInfo->SideInfoPS.sis[1][1].factScale = factScale;
		
	return 0;
}


int encHeadSideInfo(MP3EncInfo *encInfo)
{
    FrameInfo   *frameInfo;
	Bitstream   *stream;
	HeadInfo	*header;
	SideInfo	*si;
	SideInfoSub *sis;
	int ngrans, nch;
	int gr, ch;
	
	frameInfo = encInfo->frame;
	header = &(frameInfo->header);
	stream = encInfo->stream;
	ngrans = frameInfo->ngrans;
	nch = frameInfo->channels;

	initstream(stream, encInfo->headdata, frameInfo->si_len/8 + 4);

	putbits(stream, 0xfff, 12);			//synic words
	putbits(stream, header->type, 1);	//type
	putbits(stream, 1, 2);				//layer 3
	putbits(stream, 1, 1);				//non crc
	putbits(stream, header->bitr_id, 4);//bitrate
	putbits(stream, header->smpl_id, 2);//samplerate
	putbits(stream, header->padding, 1);//padding 
	putbits(stream, 0, 1);				//private_bit
	putbits(stream, header->mode, 2);	//mode
	putbits(stream, 0, 2);				//extmode
	putbits(stream, 0, 1);				//copyright 
	putbits(stream, 0, 1);				//original
	putbits(stream, 0, 2);				//emphasis

	si = &(frameInfo->SideInfoPS);

	if (header->type) {
		/* MPEG 1 */
		putbits(stream, si->mainDataBegin,  9);
		putbits(stream, si->privateBits, ((nch == 1) ? 5 : 3));

		for (ch = 0; ch < nch; ch++)
			putbits(stream, si->scfsi[ch], 4);
	} else {
		/* MPEG 2*/
		putbits(stream, si->mainDataBegin,  8);
		putbits(stream, si->privateBits, ((nch == 1) ? 1 : 2));
	}
	
	for(gr =0; gr < ngrans; gr++) {
		for (ch = 0; ch < nch; ch++) {
			sis = &si->sis[gr][ch];	
			
			putbits(stream, sis->part23Length, 12);
			putbits(stream, sis->nBigvals >> 1, 9);
			putbits(stream, sis->globalGain, 8);
			putbits(stream, sis->sfCompress, (header->type ? 4 : 9));
			putbits(stream, 0, 1);

			putbits(stream, sis->tableSelect[0], 5);
			putbits(stream, sis->tableSelect[1], 5);
			putbits(stream, sis->tableSelect[2], 5);
			putbits(stream, sis->region_0_1, 7);

			if(header->type)
				putbits(stream, 0, 1);
			putbits(stream, 0, 1);
			putbits(stream, sis->count1TableSelect, 1);
		}		
	}
	
	return 0;
}

int encMP3(MP3EncInfo *encInfo)
{
    FrameInfo	*frameInfo;
	SideInfoSub *si;
	short *mfbuf; 
	Bitstream *stream;
	int   *mdct_freq;
	int      ii, gr, k, ch, shift, gr_cnt;
    int      max, min;
	int      ngrans, nch;

	frameInfo = encInfo->frame;
	stream = encInfo->stream;
	ngrans = frameInfo->ngrans;
	nch = frameInfo->channels;

	frameInfo->ResvSize = 0;
	gr_cnt = ngrans * nch;

	mfbuf = encInfo->encbuf;

	initstream(stream, encInfo->maindata, BUFFER_MDLEN);
	
	for(gr=0; gr<ngrans; gr++)
	{
		short *wk = mfbuf + nch*286 + gr*576*nch;
		
		if(nch==1)
			window_subband1_1ch(wk, frameInfo->sbsample[0][1-gr][0], frameInfo->sbsample[1][1-gr][0]);
		else
			window_subband1_2ch(wk, frameInfo->sbsample[0][1-gr][0], frameInfo->sbsample[1][1-gr][0]);
		
		for(ch=0; ch<nch; ch++)
		{
			int   band;
			int   *mdct;
			
			si = &(frameInfo->SideInfoPS.sis[gr][ch]);
			
			for(k=0; k<18; k++)
			{
				int *subs = frameInfo->sbsample[ch][1-gr][k];
				window_subband2(subs);
				if(k & 1)
				{				
					for(band=1; band<32; band+=2)
						subs[band] *= -1;
				}
			}
			
			mdct_freq = frameInfo->xr[gr];
			shift = k = 14;
			for(ii=0; ii<2 && k; ii++)
			{
				mdct = mdct_freq;
				si->additStep = 4 * (14 - shift);
				for(band=0; band< frameInfo->num_bands; band++, mdct+=18)
				{
					int *band0 = frameInfo->sbsample[ch][  gr][0] + order[band];
					int *band1 = frameInfo->sbsample[ch][1-gr][0] + order[band];
					int work[18];
					
					for(k=-9; k<0; k++)
					{
						int a = shft_n(band1[(k+9)*32], shift);
						int b = shft_n(band1[(8-k)*32], shift);
						int c = shft_n(band0[(k+9)*32], shift);
						int d = shft_n(band0[(8-k)*32], shift);
						
						work[k+ 9] = shft16(a * win[k+ 9][0] + b * win[k+ 9][1]
							+ c * win[k+ 9][2] + d * win[k+ 9][3]);
						
						work[k+18] = shft16(c * win[k+18][0] + d * win[k+18][1]
							+ a * win[k+18][2] + b * win[k+18][3]);
					}
					
					mdct_long(mdct, work);
					
					if(band != 0) 
					{
						for(k=7; k>=0; --k)
						{
							int bu, bd;
							bu = shft15(mdct[k]) * ca[k] + shft15(mdct[-1-k]) * cs[k];
							bd = shft15(mdct[k]) * cs[k] - shft15(mdct[-1-k]) * ca[k];
							mdct[-1-k] = bu;
							mdct[ k  ] = bd;
						}
					}
				}
				
				max = min = 0;
				for(k=0; k<576; k++)
				{
					mdct_freq[k] = shft13(mdct_freq[k]);
					if(max < mdct_freq[k])  max = mdct_freq[k];
					if(min > mdct_freq[k])  min = mdct_freq[k];
				}
				
				max = (max > -min) ? max : -min;
				si->max_val = (long)max;
				
				for(k=0; max<(0x3c00>>k); k++);
				shift = 12 - k;
			}
			
			si->quantStep += si->additStep;
			
			iteration_loop(frameInfo, stream, si, mdct_freq, gr_cnt--, ch);
			Huffmancodebits(stream, frameInfo->ix, mdct_freq, si);
			
			si->quantStep -= si->additStep;
			
			if(ngrans == 1)
				encInfo->pvoMemop->Copy(VO_INDEX_ENC_MP3, frameInfo->sbsample[ch][0], frameInfo->sbsample[ch][1], 
					sizeof(frameInfo->sbsample[ch][0]));
		}
	}
	
	encHeadSideInfo(encInfo);

    return 0;
}
