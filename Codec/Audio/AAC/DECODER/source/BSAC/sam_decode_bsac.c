#include "struct.h"
#include "decoder.h"
#include "sam_dec.h"
#ifdef  BSAC_DEC
#include "decode_bsac.h"
#include "sam_model.h"

#define	TOP_LAYER		48
#define	PNS_SF_OFFSET	90
#define	MAX_CBAND0_SI_LEN	11

/*--------------------------------------------------------------*/
/***************   coding band side infomation   ****************/
/*--------------------------------------------------------------*/
const char max_model0index_tbl[32] = 
{ 
    6, 6,   8, 8, 8,   10, 10, 10, 10,   12, 12, 12, 12,   14, 14, 14, 14, 14,
	15, 15, 15, 15, 15,   16, 16,  17, 17,  18,  19,  20,  21,  22
};

const char max_modelindex_tbl[32] = 
{ 
	4, 6,   4, 6, 8,   4, 6, 8, 10,   4, 6, 8, 12,   4, 6, 8, 12, 14,
	4, 6, 8, 12, 15,   12, 16,   14, 17,  18,  19,  20,  21,  22
};


const char max_cband_si_len_tbl[32] = 
{ 
	6, 5,   6, 5, 6,   6, 5, 6, 5,   6, 5, 6, 8,   6, 5, 6, 8, 9,
	6, 5, 6, 8, 10,   8, 10,   9, 10,  10,  12,  12,  12,  12
}; 

const char cband_si_cbook_tbl[32] = 
{ 
	0, 1,   0, 1, 2,   0, 1, 2, 3,   0, 1, 2, 4,   0, 1, 2, 4, 5,
	0, 1, 2, 4, 6,   4, 6,   5, 6,  6,  6,  6,  6,  6
}; 

const short min_freq[16] = { 
	16384,  8192,  4096,  2048,  1024,   512,   256,    128,
	64,    32,    16,     8,     4,     2,     1,      0
};

int select_freq1(
				 int model_index,     /* model index for coding quantized spectral data */
				 int  bpl,            /* bit position */
				 int coded_samp_bit,  /* vector of previously encoded MSB of sample */
				 int available_len)
{
	int no_model;
	int dbpl;
	int  freq;

	if (model_index >= 15)
		dbpl =  model_index - 7  - bpl;
	else 
		dbpl =  (model_index+1)/2  - bpl;

	if (dbpl>=4) 
		no_model = model_offset_tbl[model_index][7];
	else 
		no_model = model_offset_tbl[model_index][dbpl+3];

	if (coded_samp_bit > 15)
		no_model += 15;
	else
		no_model += coded_samp_bit - 1;

	if(no_model >= 1016) no_model = 1015;
	freq = AModelSpectrum[no_model];
	if ( available_len < 14) { 
		if ( AModelSpectrum[no_model] < min_freq[available_len] ) 
			freq = min_freq[available_len];
		else if ( AModelSpectrum[no_model] > (16384-min_freq[available_len]) ) 
			freq = 16384 - min_freq[available_len];
	}

	return freq;
}

int select_freq0 (
				  int model_index,    /* model index for coding quantized spectral data */
				  int bpl,            /* bit position */
				  int enc_msb_vec,    /* vector of the encoded bits of 4-samples until the 'bpl+1'-th bit position */ 									 
				  int samp_pos,       /* current sample position among 4-samples */
				  int enc_csb_vec,    /* vector of the previously encoded bits of 4-samples at the current bit plane  */														 
				  int available_len)
{
	int no_model;
	int dbpl;
	int  freq;

	if (model_index >= 15)
		dbpl =  model_index - 7  - bpl;
	else 
		dbpl =  (model_index+1)/2  - bpl;

	if (dbpl>=3) 
		no_model = *(model_offset_tbl[model_index]+3);
	else
		no_model =*(model_offset_tbl[model_index]+dbpl);

	no_model += small_step_offset_tbl[enc_msb_vec][samp_pos][enc_csb_vec];

	if(no_model >= 1016) 
		no_model = 1015;
	else if(no_model < 0)
		no_model = 0;


	freq = AModelSpectrum[no_model];
	if ( available_len < 14) { 
		if ( AModelSpectrum[no_model] < min_freq[available_len] ) 
			freq = min_freq[available_len];
		else if ( AModelSpectrum[no_model] > (16384-min_freq[available_len]) ) 
			freq = 16384 - min_freq[available_len];
	}

	return freq;
}

int initialize_layer_data(AACDecoder *decoder,
						  ICS_Data* ICS_Data,
						  int	top_band, 
						  char	*layer_bit_flush,
						  char	*layer_reg)
{
	int		i, g;
	int		sfb, cband, reg;
	int		layer;
	int		slayer_size = 0;
	WINDOW_SEQUENCE windowSequence = ICS_Data->window_sequence;
	int		num_window_groups = ICS_Data->num_window_groups; 
	char	*window_group_length = (char *)ICS_Data->window_group_length;
	BSAC_AAC* bsac = decoder->bsac;
	int		base_band = bsac->base_band;
	int		enc_top_layer = bsac->top_layer;
	int		max_sfb = ICS_Data->max_sfb;
	int		sampling_rate = decoder->sampleRate;
	int		*end_freq = bsac->layer_extra_len;
	int		*end_cband= bsac->layer_buf_offset;
	int		*last_freq= bsac->layer_si_maxlen;
	int		*init_max_freq = bsac->start_freq[0];
	int     *layer_max_freq = bsac->layer_max_freq;
	int     *layer_max_cband = bsac->layer_max_cband;
	int		*layer_max_qband = bsac->layer_max_qband;
	
	if(windowSequence == 2) {
		slayer_size = 0;
		bsac->preWinSequence=windowSequence;
		for(g = 0; g < num_window_groups; g++) {

			init_max_freq[g] = base_band * window_group_length[g] * 4;
			last_freq[g] = bsac->swb_offset[g][max_sfb];

			switch(sampling_rate) {
				case 48000:
				case 44100:
					if ((init_max_freq[g]&31) >= 16)
						init_max_freq[g] = (init_max_freq[g] >> 5) * 32 + 20;
					else if ((init_max_freq[g]&31) >= 4)
						init_max_freq[g] = (init_max_freq[g] >> 5) * 32 + 8;
					break;
				case 32000:
				case 24000:
				case 22050:
					init_max_freq[g] = (init_max_freq[g] >> 4) * 16;
					break;
				case 16000:
				case 12000:
				case 11025:
					init_max_freq[g] = (init_max_freq[g] >> 5) * 32;
					break;
				case  8000:
					init_max_freq[g] = (init_max_freq[g] >> 6) * 64;
					break;
			}

            if (init_max_freq[g] > last_freq[g])
            	init_max_freq[g] = last_freq[g]; 
			end_freq[g] = init_max_freq[g];
			end_cband[g] = (end_freq[g] + 31) >> 5;
			slayer_size += (init_max_freq[g] + 31) >> 5;
		}
        

		layer = 0;
		for(g = 0; g < num_window_groups; g++)	{
			for (cband = 1; cband <= end_cband[g]; layer++, cband++) {
				
				layer_reg[layer] = g;
				layer_max_freq[layer] = cband * 32;
				if (layer_max_freq[layer] > init_max_freq[g])
					layer_max_freq[layer] = init_max_freq[g];
				
				layer_max_cband[layer] = cband;
			}
		}
	
		layer = slayer_size;
		for(g = 0; g < num_window_groups; g++) {
			for (i=0; i<window_group_length[g]; i++) { 
				layer_reg[layer] = g;
				layer++;
			}
		}
	
		for (layer=slayer_size+8; layer<MAX_LAYER; layer++) 
			layer_reg[layer] = layer_reg[layer-8];

		for (layer = slayer_size; layer <MAX_LAYER; layer++) {
			reg = layer_reg[layer];
			switch(sampling_rate) {
				case 48000:
				case 44100:
					end_freq[reg] += 8;
					if ((end_freq[reg]&31) != 0)
						end_freq[reg] += 4;
					break;
				case 32000:
				case 24000:
				case 22050:
					end_freq[reg] += 16;
					break;
				case 16000:
				case 12000:
				case 11025:
					end_freq[reg] += 32;
				break;
				default:
					end_freq[reg] += 64;
					break;
			}
			if (end_freq[reg] > last_freq[reg] )
				end_freq[reg] = last_freq[reg];

			layer_max_freq[layer] = end_freq[reg];
			layer_max_cband[layer] = (layer_max_freq[layer]+31) >> 5;
		}


		for (layer = 0; layer < (enc_top_layer+slayer_size-1); layer++) {
			if (layer_max_cband[layer] != layer_max_cband[layer+1] ||
				layer_reg[layer] != layer_reg[layer+1] )
				layer_bit_flush[layer] = 1;
			else
				layer_bit_flush[layer] = 0;
		}
		
		for ( ; layer < MAX_LAYER; layer++)
				layer_bit_flush[layer] = 1;

		for (layer = 0; layer < MAX_LAYER; layer++) {
			int	end_layer;

			end_layer = layer;
			while ( !layer_bit_flush[end_layer] ) 
				end_layer++;

			reg = layer_reg[layer];
			for (sfb=0; sfb<top_band; sfb++) {
				if (layer_max_freq[end_layer] <= bsac->swb_offset[reg][sfb+1]) {
					layer_max_qband[layer] = sfb+1;
					break;
				}
			}
			if (layer_max_qband[layer] > max_sfb)
				layer_max_qband[layer] = max_sfb;
		}
	} else {
		g = 0;

		if(bsac->preWinSequence==windowSequence&&max_sfb==bsac->preMaxSfb
			&&base_band==bsac->preBaseBand&&bsac->preTopLayer==enc_top_layer)
		{
			bsac->SameAsPrev = 1;
			return bsac->preSlayerSize;
		}
		else
		{
			bsac->SameAsPrev = 0;
			bsac->preWinSequence=windowSequence;
			bsac->preMaxSfb = max_sfb;
			bsac->preBaseBand = base_band;
			bsac->preTopLayer = enc_top_layer;
		}
		last_freq[g] = bsac->swb_offset[g][max_sfb];
	
		/************************************************/
		/* calculate layer_max_cband and layer_max_freq */
		/************************************************/
		init_max_freq[g] = base_band * 32;

		if (init_max_freq[g] > last_freq[g])
			init_max_freq[g] = last_freq[g];
		slayer_size = (init_max_freq[g] + 31) >> 5;

		cband = 1;
		for (layer = 0; layer < slayer_size; layer++, cband++) {

			layer_max_freq[layer] = cband * 32;
			if (layer_max_freq[layer] > init_max_freq[g])
				layer_max_freq[layer] = init_max_freq[g];

			layer_max_cband[layer] = cband;
		}

		for (layer = slayer_size; layer < MAX_LAYER; layer++) {

			switch(sampling_rate) {
				case 48000:
				case 44100:
					layer_max_freq[layer] = layer_max_freq[layer-1] + 8;

					if ((layer_max_freq[layer]&31) != 0)
						layer_max_freq[layer] += 4;
					break;
				case 32000:
				case 24000:
				case 22050:
					layer_max_freq[layer] = layer_max_freq[layer-1] + 16;
					break;
				case 16000:
				case 12000:
				case 11025:
					layer_max_freq[layer] = layer_max_freq[layer-1] + 32;
				break;
				default:
					layer_max_freq[layer] = layer_max_freq[layer-1] + 64;
					break;
			}
			if (layer_max_freq[layer] > last_freq[g])
				layer_max_freq[layer] = last_freq[g];

			layer_max_cband[layer] = (layer_max_freq[layer] + 31) >> 5;
		}


		/*****************************/
		/* calculate layer_bit_flush */
		/*****************************/
		for (layer = 0; layer < (enc_top_layer+slayer_size-1); layer++) { 
			if (layer_max_cband[layer] != layer_max_cband[layer+1])
				layer_bit_flush[layer] = 1;
			else
				layer_bit_flush[layer] = 0;
		}
		for ( ; layer < MAX_LAYER; layer++)
				layer_bit_flush[layer] = 1;

		/*****************************/
		/* calculate layer_max_qband */
		/*****************************/
		for (layer = 0; layer < MAX_LAYER; layer++) {
			int	end_layer;

			end_layer = layer;
			while ( !layer_bit_flush[end_layer] ) 
				end_layer++;

			for (sfb=0; sfb < top_band; sfb++) {
				if (layer_max_freq[end_layer] <= bsac->swb_offset[0][sfb+1]) {
					layer_max_qband[layer] = sfb+1;
					break;
				}
			}
			if (layer_max_qband[layer] > max_sfb)
				layer_max_qband[layer] = max_sfb;
		}
	}
	
	bsac->preSlayerSize = slayer_size;
	return slayer_size;
}


void sam_decode_bsac_stream (AACDecoder *decoder,
							 ICS_Data* ICS_Data,
							 int long_sfb_top,
							 int used_bits,
							 int* samplesBuf) 
{

	BSAC_AAC* bsac = decoder->bsac;
	VO_MEM_OPERATOR *voMemop =decoder->voMemop;
	int    frameLength =decoder->frame_length*8;
	int    target = bsac->top_layer;
	int    enc_top_layer = bsac->top_layer;
	int    base_snf =bsac->base_snf_thr;
	//int    base_band = bsac->base_band;
	int    max_sfb = ICS_Data->max_sfb;
	WINDOW_SEQUENCE  windowSequence = ICS_Data->window_sequence;
	int    num_window_groups = ICS_Data->num_window_groups;
	char    *window_group_length = (char *)ICS_Data->window_group_length;

	int    *layer_max_freq = bsac->layer_max_freq;
	int    *layer_max_cband = bsac->layer_max_cband;
	int    *layer_max_qband = bsac->layer_max_qband;
	int    *layer_buf_offset = bsac->layer_buf_offset;
	int    *layer_si_maxlen = bsac->layer_si_maxlen;
	int    *layer_extra_len = bsac->layer_extra_len;

#if SUPPORT_SBA_MODE
	int    *layer_cw_offset = bsac->layer_cw_offset;
    int    sba_mode = bsac->sba_mode;
#endif
	char   *layer_reg = bsac->layer_reg;
	char   *layer_bit_flush = bsac->layer_bit_flush;

	int    maxbpl=0;
	int*    cband_si_type =bsac->cband_si_type;
	int*    scf_model0 = bsac->scf_model0;
	int*    scf_model1 = bsac->scf_model1;
	int*    max_sfb_si_len = bsac->max_sfb_si_len;
	char   *cur_snf[2][8];
	
	//int    stereo_mode = decoder->stereo_mode;	
	int    pns_data_present = decoder->pns_data_present[0];
	int    pns_sfb_start = decoder->pns_start_sfb;
	
	int    nch = decoder->channelNum;
	int    top_band = long_sfb_top;
	int    i, ch, g, k;
	int    qband, cband;
	int    cw_len;
	int    layer;
	int    slayer_size;
	int    si_offset;
	int    available_len = 0;
	
	int    flen;
	int    max_pns_energy[2];
	int    pns_pcm_flag[2] = {1, 1};
	long   avail_bit_len;
	int	   *samples,*temp;
	int    *dscale_bits;

	samples = samplesBuf;
	if (target > enc_top_layer) 
		target = enc_top_layer; 
	/* ***************************************************** */
	/*                  Initialize variables                 */
	/* ***************************************************** */
	for(ch = 0; ch < nch; ch++) {
		int s;
		temp = samples + 1024 * ch;
		s = 0;
		g = 0;
		cur_snf[ch][g] = &(bsac->cur_snf_buf[ch][0]);
		s += window_group_length[g];

		voMemop->Set(VO_INDEX_DEC_AAC, bsac->band_snf[ch][0], 0, 128);
		voMemop->Set(VO_INDEX_DEC_AAC, bsac->ModelIndex[ch][0], 0, 128);
		for(g = 1; g < num_window_groups; g++) { 
			cur_snf[ch][g] = &(bsac->cur_snf_buf[ch][128*s]);
			s += window_group_length[g];
			
			voMemop->Set(VO_INDEX_DEC_AAC, bsac->band_snf[ch][g], 0, 128);
			voMemop->Set(VO_INDEX_DEC_AAC, bsac->ModelIndex[ch][g], 0, 128);
		}
		
		voMemop->Set(VO_INDEX_DEC_AAC, bsac->sign_is_coded_buf[ch], 0, 1024);
		voMemop->Set(VO_INDEX_DEC_AAC, bsac->coded_samp_bit_buf[ch], 0, 1024);
		voMemop->Set(VO_INDEX_DEC_AAC, bsac->cur_snf_buf[ch], 0, 1024);
		voMemop->Set(VO_INDEX_DEC_AAC, temp, 0, 1024*4);
		voMemop->Set(VO_INDEX_DEC_AAC, bsac->enc_sign_vec_buf[ch], 0, 256);
		voMemop->Set(VO_INDEX_DEC_AAC, bsac->enc_vec_buf[ch], 0, 256);
		//voMemop->Set(VO_INDEX_DEC_AAC, bsac->pns_sfb_flag[ch], 0, MAX_SCFAC_BANDS*sizeof(int));		
	}
	
	//voMemop->Set(VO_INDEX_DEC_AAC, bsac->ms_mask,0,MAX_SCFAC_BANDS*sizeof(int));
	//voMemop->Set(VO_INDEX_DEC_AAC, bsac->pns_sfb_mode,0,MAX_SCFAC_BANDS*sizeof(int));
	//voMemop->Set(VO_INDEX_DEC_AAC, bsac->is_info,0,MAX_SCFAC_BANDS*sizeof(int));
	voMemop->Set(VO_INDEX_DEC_AAC, bsac->stereo_si_coded,0,MAX_SCFAC_BANDS*sizeof(char));
	voMemop->Set(VO_INDEX_DEC_AAC, layer_reg,0,MAX_LAYER);

	if(max_sfb == 0) {
		return;
	}
	
	slayer_size = initialize_layer_data(decoder,ICS_Data,top_band, layer_bit_flush, layer_reg);
 
  /* ##################################################### */
  /*                   BSAC MAIN ROUTINE                    */
  /* ##################################################### */
  
  /* ***************************************************** */
  /*                  BSAC_layer_stream()                  */
  /* ***************************************************** */    
	for(ch = 0; ch < nch; ch++)	{
		for(g = 0; g < num_window_groups; g++) {
			bsac->end_cband[ch][g] = bsac->end_qband[ch][g] = 0;
		}
	}
	
	for (layer=0; layer<(enc_top_layer+slayer_size); layer++) {
		layer_si_maxlen[layer] = 0;
		g = layer_reg[layer];
		for(ch = 0; ch < nch; ch++) {
			int start_q = bsac->end_qband[ch][g];
			int start_c = bsac->end_cband[ch][g];
			int end_q  = layer_max_qband[layer];
			int end_c = layer_max_cband[layer];


			/* coding band side infomation  */
			for (cband = start_c; cband < end_c; cband++) {
				if (cband <= 0) 
					layer_si_maxlen[layer] += MAX_CBAND0_SI_LEN;
				else 
					layer_si_maxlen[layer] += max_cband_si_len_tbl[cband_si_type[ch]];  
			}
			
			/* side infomation : scalefactor */
			for(qband = start_q; qband < end_q; qband++) {
				layer_si_maxlen[layer] += max_sfb_si_len[ch] + 5;
			}

			bsac->start_qband[ch][g] = start_q;
			bsac->start_cband[ch][g] = start_c;
			bsac->end_qband[ch][g] = end_q;
			bsac->end_cband[ch][g] = end_c;
		}
	}


	dscale_bits = bsac->sam_scale_bits_dec;
	flen = frameLength;
	if(flen < dscale_bits[enc_top_layer]) 
		flen = dscale_bits[enc_top_layer];
	
	//for (layer=enc_top_layer+slayer_size; layer< MAX_LAYER; layer++)
	// layer_buf_offset[layer] = flen;
	layer_buf_offset[enc_top_layer+slayer_size] = flen;
	
	si_offset = flen - layer_si_maxlen[enc_top_layer+slayer_size-1];
	/* 2000. 08. 22. enc_top_layer -> enc_top_layer+slayer_size-1. SHPARK */
	
	for (layer=(enc_top_layer+slayer_size-1); layer>=slayer_size; layer--) {
		if ( si_offset < dscale_bits[layer-slayer_size] )
			layer_buf_offset[layer] = si_offset;
		else
			layer_buf_offset[layer] = dscale_bits[layer-slayer_size];
		
		si_offset = layer_buf_offset[layer] - layer_si_maxlen[layer-1];
	}

	for (; layer>0; layer--) {
		layer_buf_offset[layer] = si_offset;
		si_offset = layer_buf_offset[layer] - layer_si_maxlen[layer-1];
	}
	
	layer_buf_offset[0] = used_bits;

	if (used_bits > si_offset) {
		si_offset = used_bits - si_offset;
		for (layer=(enc_top_layer+slayer_size-1); layer>=slayer_size; layer--) {
			cw_len  = layer_buf_offset[layer+1] - layer_buf_offset[layer];
			cw_len -= layer_si_maxlen[layer];

			if (cw_len >= si_offset) { 
				cw_len = si_offset; 
				si_offset = 0;
			} 
			else
				si_offset -= cw_len;
			
			for (i=1; i<=layer; i++) {
				layer_buf_offset[i] += cw_len;
			}
			
			if (si_offset==0) break;
		}
	} 
	else 
	{
		si_offset = si_offset - used_bits;
		for (layer = 1; layer < slayer_size; layer++) {
#if !USE_DIVIDE_FUNC
			layer_buf_offset[layer] = layer_buf_offset[layer-1] + layer_si_maxlen[layer-1] + si_offset/slayer_size;
#else
			layer_buf_offset[layer] = layer_buf_offset[layer-1] + layer_si_maxlen[layer-1] + voUnsignedDivide(decoder,si_offset,slayer_size);	
#endif
			if ( layer <= (si_offset%slayer_size) )
				layer_buf_offset[layer]++;
		}
	}
	
	/* added by shpark. 2001. 02. 15 */
	for (layer=enc_top_layer+slayer_size-1; layer>=0; layer--) {
		if (layer_buf_offset[layer] > frameLength) 
			layer_buf_offset[layer] = frameLength;
		else 
			break;
	}
	
	avail_bit_len = frameLength;
	for (layer=target+slayer_size-1; layer>=0; layer--) {
		if (layer_buf_offset[layer] > avail_bit_len) 
			target--;
		else 
			break;
	}

	for(ch = 0; ch < nch; ch++) {
		for(g = 0; g < num_window_groups; g++) 
		{
			bsac->start_qband[ch][g] = 0;;
			bsac->start_cband[ch][g] = 0;;
			bsac->start_freq[ch][g] = 0;
			bsac->end_qband[ch][g] = 0;;
			bsac->end_cband[ch][g] = 0;;
			bsac->end_freq[ch][g] = 0;
		}
	}
#if SUPPORT_SBA_MODE
	if(!sba_mode) 
#endif
	{
		sam_initArDecode(0);
		sam_setArDecode(0);
		//sam_setRBitBufPos(used_bits);
		available_len = 0;
	}
	
	for(ch = 0; ch < nch; ch++) {
		for(g = 0; g < num_window_groups; g++)
			bsac->s_freq[ch][g] = 0;
	}
	
	if (windowSequence == 2) {
		for (layer = 0; layer < slayer_size; layer++) {
			for(ch = 0; ch < nch; ch++) {
				g = layer_reg[layer];
				bsac->e_freq[ch][g]  = layer_max_freq[layer];
			}
		}
	}

	for (layer = 0; layer < target+slayer_size; layer++) {
		int min_snf;
		int *scf_model;
		g = layer_reg[layer];
		for(ch = 0; ch < nch; ch++) {
			bsac->start_freq[ch][g]  = bsac->end_freq[ch][g];
			bsac->start_qband[ch][g] = bsac->end_qband[ch][g];
			bsac->start_cband[ch][g] = bsac->end_cband[ch][g];
			
			bsac->end_qband[ch][g] = layer_max_qband[layer];
			bsac->end_cband[ch][g] = layer_max_cband[layer];
			bsac->end_freq[ch][g]  = layer_max_freq[layer];
			
			if (windowSequence == 2) {
				if(layer >= slayer_size) 
				{
					//for(ch = 0; ch < nch; ch++) 
					{
						g = layer_reg[layer];
						bsac->e_freq[ch][g]  = layer_max_freq[layer];
					}
				}
			} else {
				//for(ch = 0; ch < nch; ch++) 
				{
					if (layer >= slayer_size) 
						bsac->e_freq[ch][0] = layer_max_freq[layer];
					else
						bsac->e_freq[ch][0] = layer_max_freq[slayer_size-1];
				}
			}
		}

   
#if SUPPORT_SBA_MODE
		if(sba_mode) {
			if (layer==0 || layer_bit_flush[layer-1]) {
				sam_initArDecode(layer);
				sam_setArDecode(layer);
				sam_setRBitBufPos(layer_cw_offset[layer]);
				
				available_len = layer_buf_offset[layer+1]-layer_buf_offset[layer];
				available_len--;
			} else  {
				available_len += layer_buf_offset[layer+1]-layer_buf_offset[layer];
			}
		} 
		else 
#endif
		{
			available_len += layer_buf_offset[layer+1]-layer_buf_offset[layer];
		}
		
		if (available_len <= 0 )  {
			for(ch = 0; ch < nch; ch++) {
				k = bsac->end_freq[ch][g] - bsac->start_freq[ch][g];
				for(i = bsac->start_freq[ch][g]; i < bsac->end_freq[ch][g]; i+=32,k-=32)
				{
					int m = bsac->band_snf[ch][g][i >> 5];
					if(k>=32)
						voMemop->Set(VO_INDEX_DEC_AAC, cur_snf[ch][g]+i,m,32);
					else
						voMemop->Set(VO_INDEX_DEC_AAC, cur_snf[ch][g]+i,m,k);
				}
			}
			continue;
		}
										
		//continue;
		cw_len =  decode_cband_si(&decoder->bs, bsac->band_snf, bsac->ModelIndex, cband_si_type, 
			bsac->start_cband, bsac->end_cband,g, nch);
		maxbpl = 0;
		for(ch = 0; ch < nch; ch++) {
			k = bsac->end_freq[ch][g] - bsac->start_freq[ch][g];
			for(i = bsac->start_freq[ch][g]; i < bsac->end_freq[ch][g]; i+=32,k-=32)
			{
				int n = bsac->band_snf[ch][g][i/32];
				if(k>=32)
					voMemop->Set(VO_INDEX_DEC_AAC, cur_snf[ch][g]+i, n, 32);
				else
					voMemop->Set(VO_INDEX_DEC_AAC, cur_snf[ch][g]+i, n, k);
				if(maxbpl<n)
					maxbpl = n;
			}
		}

		g = layer_reg[layer];
		available_len -= cw_len;
		
		/* side infomation : scalefactor */
		if (layer < slayer_size) 
			scf_model = (int *)scf_model0;
		else
			scf_model = (int *)scf_model1;

		cw_len =  decode_scfband_si(
			decoder,
			scf_model, 
			pns_data_present,
			pns_sfb_start, 
			pns_pcm_flag, 
			max_pns_energy, 
			g); 
		
		available_len -= cw_len;		
		min_snf = layer < slayer_size ? base_snf : 1;

		/* Bit-Sliced Spectral Data Coding */
		cw_len = decode_spectra(decoder,samplesBuf, g, g+1, bsac->start_freq, bsac->end_freq, cur_snf,
			min_snf, available_len,maxbpl);   
		
		available_len -= cw_len;
#if SUPPORT_SBA_MODE
		if(!sba_mode) 
#endif
		{
			if((available_len > 0))
			{
				maxbpl=-1;

				cw_len = decode_spectra(decoder, samplesBuf, 0, num_window_groups, bsac->s_freq, bsac->e_freq, cur_snf,
					1, available_len, maxbpl);
				available_len -= cw_len;	
			}
		}
#if SUPPORT_SBA_MODE
		else if (layer_bit_flush[layer] && available_len > 0) {
			layer_extra_len[layer] = available_len;
			sam_storeArDecode(layer);
#if USE_EXTRA_BITBUFFER
			layer_cw_offset[layer] = sam_getRBitBufPos();//sam_getUsedBits();//
#else
			layer_cw_offset[layer] = sam_getUsedBits();;	
#endif
		}
#endif//SUPPORT_SBA_MODE   		
		
	} /* for (layer = 0; layer <= top_layer; layer++) */

#if SUPPORT_SBA_MODE
	if(!sba_mode) 
#endif
		layer_extra_len[enc_top_layer+slayer_size-1] = frameLength;
	
#if SUPPORT_SBA_MODE
	/* ***************************************************** */
	/*         Extra Encoding Process  for each layer        */
	/* ***************************************************** */
	for(ch = 0; ch < nch; ch++) {
		for(g = 0; g < num_window_groups; g++)
			start_freq[ch][g] = 0;
	}
	
	if (windowSequence == 2) {
		for (layer = 0; layer < slayer_size; layer++) {
			for(ch = 0; ch < nch; ch++) {
				g = layer_reg[layer];
				e_freq[ch][g]  = layer_max_freq[layer];
			}
		}
	}

	if(sba_mode) {
		for (layer = 0; layer < target+slayer_size; layer++) {
			available_len = layer_extra_len[layer];
			
			if (windowSequence == 2) {
				if(layer >= slayer_size) {
					for(ch = 0; ch < nch; ch++) {
						g = layer_reg[layer];
						e_freq[ch][g]  = layer_max_freq[layer];
					}
				}
			} 
			else 
			{
				for(ch = 0; ch < nch; ch++) {
					if (layer >= slayer_size) 
						e_freq[ch][0] = layer_max_freq[layer];
					else
						e_freq[ch][0] = layer_max_freq[slayer_size-1];
				}
			}

			if (available_len <= 0 )  continue;
			
			//if(sba_mode) 
			{
				sam_setArDecode(layer);
				//hbf TODO
				sam_setRBitBufPos(layer_cw_offset[layer]);
			}
			
			/* Extra Bit-Sliced Spectral Data Coding */
			cw_len = decode_spectra(decoder,bsac->ModelIndex, 0, num_window_groups, start_freq, e_freq, 1,
				available_len,-1);
			
			available_len -= cw_len;
			
			if(windowSequence == 2) {
				for(ch = 0; ch < nch; ch++)
					for(g = 0; g < num_window_groups; g++)
						s_freq[ch][g] = e_freq[ch][g];
			}
			
			for (k = layer+1; k < target+slayer_size; k++) {
				
				if (available_len <= 0 ) 
					break; 
				
				for(ch = 0; ch < nch; ch++) {
					g = layer_reg[k];
					s_freq[ch][g] = e_freq[ch][g];
					e_freq[ch][g] = layer_max_freq[k];
				}
				
				cw_len = decode_spectra(decoder, bsac->ModelIndex, 0, num_window_groups, s_freq, e_freq, 1,
					available_len,-1);
				available_len -= cw_len;
			}
		}
	}
#endif// SUPPORT_SBA_MODE
}


int decode_cband_si(	   
					BitStream	  *bs,
					int band_snf[][8][32],
					int model_index[][8][32],
					int cband_si_type[],
					int start_cband[][8],
					int end_cband[][8],
					int g,
					int nch)
{
	int		ch, m;
	int		cband;
	int		cband_model;
	int		si_cw_len;
	int		*model_index_p, *band_snf_p;
	int		max_model_index[2];	
	
	si_cw_len = 0;
	for(ch = 0; ch < nch; ch++) {
		if(start_cband[ch][g] == 0) {
			cband_model = 7;
			max_model_index[ch] = max_model0index_tbl[cband_si_type[ch]];
		} else {
			cband_model = cband_si_cbook_tbl[cband_si_type[ch]];
			max_model_index[ch] = max_modelindex_tbl[cband_si_type[ch]];
		}

		model_index_p = model_index[ch][g] + start_cband[ch][g];
		band_snf_p = band_snf[ch][g] + start_cband[ch][g];
		for (cband = start_cband[ch][g]; cband < end_cband[ch][g]; cband++) {
			si_cw_len += sam_decode_symbol(bs, AModelCBand[cband_model], &m);
			*model_index_p = m;
			
			/* F O R  E R R O R */
			if (m > max_model_index[ch])
				*model_index_p = max_model_index[ch];
			
			if(*model_index_p >= 15)
				*band_snf_p = *model_index_p - 7;
			else
				*band_snf_p = (*model_index_p+1) / 2;

			model_index_p++; 
			band_snf_p++;			
		}
	}

	return si_cw_len;
}

/*--------------------------------------------------------------*/
/***********  scalefactor band side infomation   ****************/
/*--------------------------------------------------------------*/
int decode_scfband_si(
	AACDecoder *decoder,
	int	scf_model[],
	int pns_data_present,
	int pns_sfb_start,
	int pns_pcm_flag[],
	int max_pns_energy[],
	int g
	)
{
	int ch;
	int	m;
	int sfb, nsfb;
	int si_cw_len;
	ICS_Data* ICS_Data = &decoder->ICS_Data[0];
	short *scf;
	BSAC_AAC* bsac = decoder->bsac;
	BitStream *bs = &decoder->bs;
	int *max_scf = bsac->max_scalefactor;
	short *AModelNoiseNrg = bsac->AModelNoiseNrg;
	unsigned char *ms_mask = decoder->ms_used[g];
	unsigned char *is_info = decoder->is_used[g];
	unsigned char *stereo_si_coded = (unsigned char *)bsac->stereo_si_coded;
	unsigned char *pns_sfb_mode = decoder->pns_sfb_mode;
	unsigned char *pns_sfb_flag[2];
	int	stereo_mode = decoder->stereo_mode;
	int max_sfb = ICS_Data->max_sfb*g;
	int nch = decoder->channelNum;


	pns_sfb_flag[0] = decoder->pns_sfb_flag[0];
	pns_sfb_flag[1] = decoder->pns_sfb_flag[1];

	si_cw_len = 0;
	for(ch = 0; ch < nch; ch++) {
		scf= (decoder->scaleFactors[ch]);
		for(sfb = bsac->start_qband[ch][g]; sfb < bsac->end_qband[ch][g]; sfb++) {
			nsfb = max_sfb+sfb;
			if(nch == 1) {
				if(pns_data_present && sfb >= pns_sfb_start) {
					si_cw_len += sam_decode_symbol(bs, AModelNoiseFlag, &m);
					pns_sfb_flag[0][nsfb] = m;
				}
			} else if(stereo_si_coded[nsfb] == 0) {
				if(stereo_mode != 2) {
					m = 0;
					if(stereo_mode == 1)
						si_cw_len += sam_decode_symbol(bs, AModelMsUsed, &m);
					else if(stereo_mode == 3)
						si_cw_len += sam_decode_symbol(bs, AModelStereoInfo, &m);
#if DUMPBSAC
					{
						DPRINTF(DUMPBSAC_spectra,"\ndecode_scfband_si****channel=%d,sfb=%d,stereomode=%d,len=%d\n",ch,sfb,stereo_mode,si_cw_len);
					}
#endif
					
					switch(m) {
					case 1:
						ms_mask[sfb] = 1;
						break;
					case 2:
						is_info[sfb] = 1;
						break;
					case 3:
						is_info[sfb] = 2;
						break;
					}
					
					if(pns_data_present && sfb >= pns_sfb_start) {
						si_cw_len += sam_decode_symbol(bs, AModelNoiseFlag, &m);
						pns_sfb_flag[0][nsfb] = m;
						si_cw_len += sam_decode_symbol(bs, AModelNoiseFlag, &m);
						pns_sfb_flag[1][nsfb] = m;
						if(stereo_mode == 3 && is_info[sfb] == 2) {
							if(pns_sfb_flag[0][nsfb] && pns_sfb_flag[1][nsfb]) {
								si_cw_len += sam_decode_symbol(bs, AModelNoiseMode, &m);
								pns_sfb_mode[nsfb] = m;
							}
						}
					}
				}
				stereo_si_coded[nsfb] = 1;
			}
            if (pns_sfb_flag[ch][nsfb]) {
				if (pns_pcm_flag[ch]==1) {
					si_cw_len += sam_decode_symbol(bs, AModelNoiseNrg,&m);
					max_pns_energy[ch] = m;
					pns_pcm_flag[ch] = 0;
				}
				si_cw_len += sam_decode_symbol(bs, AModelScf[scf_model[ch]],&m);
                scf[nsfb] = max_pns_energy[ch] - m;
            }
            else if ( stereo_mode == 3 && ch==1) {
				if (scf_model[ch]==0) {
					scf[nsfb] = 0;
				}
				else 
				{
					/* is_position */
					si_cw_len += sam_decode_symbol(bs, AModelScf[scf_model[ch]],&m);
					if (m&1) 
						scf[nsfb] = -((m+1)/2);
					else
						scf[nsfb] = (m/2);
				}
				
            }
            else
			{ 
				if (scf_model[ch]==0) {
					scf[nsfb] = max_scf[ch];
				} else {
					si_cw_len += sam_decode_symbol(bs, AModelScf[scf_model[ch]],&m);
					scf[nsfb] = max_scf[ch] - m;
				}
				
            }
		}
	}

	return si_cw_len;
}



#endif  

