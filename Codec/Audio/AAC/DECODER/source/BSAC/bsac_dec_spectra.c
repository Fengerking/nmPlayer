#include "struct.h"
#ifdef  BSAC_DEC
#include "decode_bsac.h"

/*--------------------------------------------------------------*/
/***********     Bit-Sliced Spectral Data        ****************/
/*--------------------------------------------------------------*/
int decode_spectra(
				   AACDecoder *psi,
				   int *samples,
				   int s_reg,
				   int e_reg,
				   int s_freq[][8],
				   int e_freq[][8],
				   unsigned char *cur_snf[2][8],
				   int min_bpl,
				   int available_len,
				   int max_snf
				   )
{
	int ch, i, m, k,j;
	
	int  bpl;
	int  cband;
	int  maxhalf;
	int  freq;
	int  cw_len;
	int  cw_len1;
	int  reg;
	BSAC_AAC *bsac = psi->bsac;
	BitStream *bs = &psi->bs;
	int    *sample[2][8];
	char   *sign_is_coded[2][8];
	char   *coded_samp_bit[2][8];
	char   *enc_sign_vec[2][8];
	char   *enc_vec[2][8];

	int sba_mode=bsac->sba_mode;
	int nch = psi->channelNum;
	unsigned char *snf;
	unsigned char *window_group_length = psi->ICS_Data[0].window_group_length;
	int num_window_groups = psi->ICS_Data[0].num_window_groups;


	for(ch = 0; ch < nch; ch++) {
		int *temp = samples + 1024 * ch;
		m = 0;
		k = 0;
		sample[ch][k] = temp;//&(sample_buf[ch][0]);
		sign_is_coded[ch][k] = &(bsac->sign_is_coded_buf[ch][0]);
		coded_samp_bit[ch][k] = &(bsac->coded_samp_bit_buf[ch][0]);
		enc_sign_vec[ch][k] = &(bsac->enc_sign_vec_buf[ch][0]);
		enc_vec[ch][k] = &(bsac->enc_vec_buf[ch][0]);
		m += window_group_length[k];

		for(k = 1; k < num_window_groups; k++) { 
			sample[ch][k] = (temp+128*m);//&(sample_buf[ch][1024*s/8]);
			sign_is_coded[ch][k] = &(bsac->sign_is_coded_buf[ch][128*m]);
			coded_samp_bit[ch][k] = &(bsac->coded_samp_bit_buf[ch][128*m]);
			enc_sign_vec[ch][k] = &(bsac->enc_sign_vec_buf[ch][32*m]);
			enc_vec[ch][k] = &(bsac->enc_vec_buf[ch][32*m]);
			m += window_group_length[k];			
		}
	}

	if(max_snf == -1)
	{
		max_snf = 0;
		for(ch = 0; ch < nch; ch++) {
			for (reg = s_reg; reg < e_reg; reg++)
			{
				i=e_freq[ch][reg] - s_freq[ch][reg];
				snf = cur_snf[ch][reg] + s_freq[ch][reg];
				while(--i>=0)
				{
					if (max_snf < *snf) 
						max_snf = *snf;
					snf++;
				};
			}
		}
	}
	
	cw_len = available_len;
	for (bpl=max_snf; bpl>=min_bpl; bpl--) {
		if (cw_len <= 0 ) return (available_len-cw_len);
		maxhalf = 1<<(bpl-1);

		for (reg = s_reg; reg < e_reg; reg++) {
			for (i = s_freq[0][reg]; i < e_freq[0][reg]; i++) {			
				for(ch = 0; ch < nch; ch++) {
					char *coded_samp_bit_p = &(coded_samp_bit[ch][reg][i]);

					if ( cur_snf[ch][reg][i] < bpl ) continue; 
					if (*coded_samp_bit_p==0 || sign_is_coded[ch][reg][i]==1) {
						char *enc_vec_p;
						cband = i >> 5;
						j = i >> 2;

						enc_vec_p = &(enc_vec[ch][reg][j]);
						if ((i&3)==0) {
							enc_sign_vec[ch][reg][j] |= *enc_vec_p;
							*enc_vec_p = 0;
						}

						cw_len1 = cw_len;
						if(!sba_mode) cw_len1 = 100;
						if (*coded_samp_bit_p) 
							freq  = select_freq1(bsac->ModelIndex[ch][reg][cband], bpl, 
							(int)*coded_samp_bit_p, cw_len1);
						else
							freq  = select_freq0(bsac->ModelIndex[ch][reg][cband], bpl, 
							(int)enc_sign_vec[ch][reg][j], i&3, *enc_vec_p, cw_len1);

						k = sam_decode_symbol2(bs, freq, &m);
						cw_len -= k;

						if (m) {
							if (sample[ch][reg][i]>=0)   
								sample[ch][reg][i] += maxhalf;
							else           
								sample[ch][reg][i] -= maxhalf;
						}

						*enc_vec_p = (*enc_vec_p<<1) | m;
						*coded_samp_bit_p = (*coded_samp_bit_p << 1) | m;
					}

					if ( *coded_samp_bit_p && sign_is_coded[ch][reg][i]==0) {
						if (cw_len <= 0 ) return (available_len-cw_len); 

						cw_len -= sam_decode_symbol2(bs, 8192, &m);

						if (m) sample[ch][reg][i] *= -1;
						sign_is_coded[ch][reg][i] = 0x01;
					}
					cur_snf[ch][reg][i]-=1;

					if (cw_len <= 0 ) return (available_len-cw_len);
				}
			}
		}
	}

	return (available_len-cw_len);
}
#endif