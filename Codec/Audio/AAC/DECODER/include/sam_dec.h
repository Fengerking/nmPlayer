	/************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#ifndef _sam_dec_h_
#define _sam_dec_h_


#ifdef __cplusplus
extern "C" {
#endif

  //int sam_decode_init(int sampling_rate_decoded, int block_size_samples);
  //void sam_scale_bits_init(int sampling_rate_decoded, int block_size_samples);

  ///* ----- Decode one frame with the BSAC-decoder  ----- */
  void sam_initArDecode(int);
  void sam_setArDecode(int);
  //void sam_storeArDecode(int);

  //int sam_decode_symbol(short *, int *);

	int decode_spectra(
				   AACDecoder *psi,
				   int *samples,
				   int s_reg,
				   int e_reg,
				   int s_freq[][8],
				   int e_freq[][8],
				   char *cur_snf[2][8],
				   int min_bpl,
				   int available_len,
				   int max_snf
				   );

	int decode_cband_si(
		BitStream	*bs,
		int band_snf[][8][32],
		int model_index[][8][32],
		int cband_si_type[],
		int start_cband[][8],
		int end_cband[][8],
		int g,
		int nch );

	int decode_scfband_si(
		AACDecoder *decoder,
		int	scf_model[],
		int pns_data_present,
		int pns_sfb_start,
		int pns_pcm_flag[],
		int max_pns_energy[],
		int g
		);

	int initialize_layer_data(AACDecoder *decoder,
		ICS_Data* ICS_Data,
		int top_band,
		char	*layer_bit_flush,
		char	*layer_reg);

	void sam_decode_bsac_stream (AACDecoder *decoder,
							 ICS_Data* ICS_Data,
							 int long_sfb_top,
							 int used_bits,
							 int* samplesBuf);

#ifdef __cplusplus
           }
#endif

#endif /* #ifndef       _sam_dec_h_ */

