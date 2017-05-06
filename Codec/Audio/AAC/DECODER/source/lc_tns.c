   /************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#include "struct.h"
#include "decoder.h"

const int tns_coef_0_3[] =
{
    Q31( 0.0000000000), Q31( 0.4338837391), Q31( 0.7818314825), Q31( 0.9749279122),
	Q31(-0.9848077530), Q31(-0.8660254038), Q31(-0.6427876097), Q31(-0.3420201433),
	Q31(-0.4338837391), Q31(-0.7818314825), Q31(-0.9749279122), Q31(-0.9749279122),
	Q31(-0.9848077530), Q31(-0.8660254038), Q31(-0.6427876097), Q31(-0.3420201433)
};

const int tns_coef_0_4[] =
{
    Q31( 0.0000000000), Q31( 0.2079116908), Q31( 0.4067366431), Q31( 0.5877852523),
	Q31( 0.7431448255), Q31( 0.8660254038), Q31( 0.9510565163), Q31( 0.9945218954),
	Q31(-0.9957341763), Q31(-0.9618256432), Q31(-0.8951632914), Q31(-0.7980172273),
	Q31(-0.6736956436), Q31(-0.5264321629), Q31(-0.3612416662), Q31(-0.1837495178)
};

const int tns_coef_1_3[] =
{
    Q31( 0.0000000000), Q31( 0.4338837391), Q31(-0.6427876097), Q31(-0.3420201433),
	Q31( 0.9749279122), Q31( 0.7818314825), Q31(-0.6427876097), Q31(-0.3420201433),
	Q31(-0.4338837391), Q31(-0.7818314825), Q31(-0.6427876097), Q31(-0.3420201433),
	Q31(-0.7818314825), Q31(-0.4338837391), Q31(-0.6427876097), Q31(-0.3420201433)
};

const int tns_coef_1_4[] =
{
    Q31( 0.0000000000), Q31( 0.2079116908), Q31( 0.4067366431), Q31( 0.5877852523),
	Q31(-0.6736956436), Q31(-0.5264321629), Q31(-0.3612416662), Q31(-0.1837495178),
	Q31( 0.9945218954), Q31( 0.9510565163), Q31( 0.8660254038), Q31( 0.7431448255),
	Q31(-0.6736956436), Q31(-0.5264321629), Q31(-0.3612416662), Q31(-0.1837495178)
};



/* TNS MAX bands (table 4.139) and MAX order (table 4.138) */

#define  MAX_ORDER_SHORT 7
#define  MAX_ORDER_LONG 12
#define  MAX_ORDER_LONG_MAIN 20

//Table 4.103 ¨C Definition of TNS_MAX_BANDS depending on AOT, windowing and sampling rate
const unsigned char tnsMaxBandsShort[NUM_SAMPLE_RATES] = {
	9,  9, 10, 14, 14, 14, 14, 14, 14, 14, 14, 14		
};


const unsigned char tnsMaxBandsLong[NUM_SAMPLE_RATES] = {
	31, 31, 34, 40, 42, 51, 46, 46, 42, 42, 42, 39
};

/* total number of scale factor bands in one window */
const unsigned char vosfBandTotalShort[NUM_SAMPLE_RATES] = {
	12, 12, 12, 14, 14, 14, 15, 15, 15, 15, 15, 15
};

const unsigned char vosfBandTotalLong[NUM_SAMPLE_RATES] = {
	41, 41, 47, 49, 49, 51, 47, 47, 43, 43, 43, 40
};


static void tns_decode_coef(int order, unsigned char coef_res_bits, unsigned char coef_compress,
                            unsigned char *coef, int *a)
{
    int i, m, t;
    const int *tmp2;
	int *b;

	b = a + 24;
	
    /* Conversion to signed integer */
	if (coef_compress == 0)
	{
		if (coef_res_bits == 3)
		{
			tmp2 = tns_coef_0_3;
		} else {
			tmp2 = tns_coef_0_4;
		}
	} else {
		if (coef_res_bits == 3)
		{
			tmp2 = tns_coef_1_3;
		} else {
			tmp2 = tns_coef_1_4;
		}
	}
	
    /* Conversion to LPC coefficients */
    a[0] = Q24(1.0);
    for (m = 1; m <= order; m++)
    {
        t = tmp2[coef[m - 1] & 0x0f];
		for (i = 1; i < m; i++) /* loop only while i<m */
            b[i] = a[i] + (MUL_32(t, a[m-i]) << 1);
		
        for (i = 1; i < m; i++) /* loop only while i<m */
            a[i] = b[i];
		
        a[m] = t >> 7; /* changed */
    }
}

static void tns_ma_filter(int *spectrum, int size, int inc, int *lpc,
						  int order)
{
	VO_S64  y;
	int  j;
    int  i;    
	int ys;
	int  *state = lpc + 24;

	for (i = 0; i < order; i++)
		state[i] = 0;
 
	for (i = 0; i < size; i++)
    {
        y = ((VO_S64)*spectrum) << 24; 
		
        for (j = order-1; j > 0; j--)
		{
			y += ((VO_S64)state[j] * (VO_S64)lpc[j+1]);
			state[j] = state[j-1];
		}

		y += ((VO_S64)state[j] * (VO_S64)lpc[j+1]);


		ys = (int)((y + (1 << 23)) >> 24);
		
        state[0] = ys;
        *spectrum = ys;
        spectrum += inc;
    }
}
   
static void tns_ar_filter(int *spectrum, int size, int inc, int *lpc,
                          int order)
{	
    VO_S64  y;
	int  j;
    int  i;    
	int ys;
	int  *state = lpc + 24;

	for (i = 0; i < order; i++)
		state[i] = 0;
 
	for (i = 0; i < size; i++)
    {
        y = ((VO_S64)*spectrum) << 28; 
		
        for (j = order-1; j > 0; j--)
		{
			y -= ((VO_S64)state[j] * (VO_S64)lpc[j+1]);
			state[j] = state[j-1];
		}

		y -= ((VO_S64)state[j] * (VO_S64)lpc[j+1]);


		ys = (int)((y + (1 << 27)) >> 28);
		
        state[0] = ys;
        *spectrum = ys;
        spectrum += inc;
    }
}

int tns_decode(AACDecoder* decoder,int channels)
{
	int w, f, tns_order,num_windows,n_filt,window_len;
    int inc;
    int size;
    int bottom, top, start, end,ch,num_swb,tns_max_order,tns_max_band;
	ICS_Data *ics;
	TNS_Data *tns;
	short*	 swb_offset;
	int* spec = NULL;
		
	for(ch=0;ch<channels;ch++)
	{
		spec = decoder->coef[ch];

		tns = decoder->tns_data[ch];
		
		if (!decoder->tns_data_present[ch])
			continue;

		if(!EnableDecodeCurrChannel(decoder,ch))
			continue;
		
		if(decoder->common_window)
			ics = &decoder->ICS_Data[0];
		else
			ics = &decoder->ICS_Data[ch];

	
		if (ics->window_sequence == EIGHT_SHORT_SEQUENCE) {
			num_windows = NUM_SHORT_WIN;
			window_len = SIZE_SHORT_WIN;
			num_swb = vosfBandTotalShort[decoder->sampRateIdx];
			tns_max_order = MAX_ORDER_SHORT;
			swb_offset = (short*)(sfBandTabShort + sfBandTabShortOffset[decoder->sampRateIdx]);
			tns_max_band = tnsMaxBandsShort[decoder->sampRateIdx];		
		} else {
			num_windows = NUM_LONG_WIN;
			window_len = SIZE_LONG_WIN;
			num_swb = vosfBandTotalLong[decoder->sampRateIdx];
			if(decoder->profile == VOAAC_AAC_MAIN)
				tns_max_order = MAX_ORDER_LONG_MAIN;
			else
				tns_max_order = MAX_ORDER_LONG;
			swb_offset = (short*)(sfBandTabLong + sfBandTabLongOffset[decoder->sampRateIdx]);
			tns_max_band = tnsMaxBandsLong[decoder->sampRateIdx];
		}
		
		if(tns_max_band>ics->max_sfb)
			tns_max_band = ics->max_sfb;

		for (w = 0; w < num_windows; w++)
		{
			 bottom = num_swb;
			 n_filt = tns->n_filt;
			 for (f = 0; f < n_filt; f++)
			 {
				 top = bottom;
				 bottom = MAX(top - tns->length[f], 0);
				 tns_order = MIN(tns->order[f], tns_max_order);
				 if (!tns_order)
					 continue;

				 start = swb_offset[MIN(bottom, tns_max_band)];
				 end   = swb_offset[MIN(top, tns_max_band)];
				 
				 size = end - start;
				 if (size <= 0)
					 continue;
				 
				 tns_decode_coef(tns_order, tns->coef_res+3,
					 tns->coef_compress[f], tns->coef[f], decoder->tns_lpc);				 
				 
				 if (tns->direction[f])
				 {	 inc = -1;
					 start = end - 1;
				 } else {
					 inc = 1;
				 }
				 
				 tns_ar_filter(&decoder->coef[ch][(w*window_len)+start], size, inc, decoder->tns_lpc, tns_order);
			 }

			 tns++;
		}

		spec = decoder->coef[ch];
	}
	return 0;
}

int tns_analysis_filter(AACDecoder* decoder,ICS_Data *ics,TNS_Data* tns,int* spec)
{
	int w, f, tns_order;
	int inc, size;
	int num_windows,window_len,num_swb,tns_max_order,tns_max_band;
	int bottom, top, start, end;
	int nshort = MAX_SAMPLES/8;
	short*	 swb_offset;
	if (ics->window_sequence == EIGHT_SHORT_SEQUENCE) 
	{
		num_windows = NUM_SHORT_WIN;
		window_len = SIZE_SHORT_WIN;
		num_swb = vosfBandTotalShort[decoder->sampRateIdx];
		tns_max_order = MAX_ORDER_SHORT;
		swb_offset = (short*)(sfBandTabShort + sfBandTabShortOffset[decoder->sampRateIdx]);
		tns_max_band = tnsMaxBandsShort[decoder->sampRateIdx];
	} 
	else 
	{
		num_windows = NUM_LONG_WIN;
		window_len = SIZE_LONG_WIN;
		num_swb = vosfBandTotalLong[decoder->sampRateIdx];
		if(decoder->profile == VOAAC_AAC_MAIN)
			tns_max_order = MAX_ORDER_LONG_MAIN;
		else
			tns_max_order = MAX_ORDER_LONG;
		
		swb_offset = (short*)(sfBandTabLong + sfBandTabLongOffset[decoder->sampRateIdx]);
		tns_max_band = tnsMaxBandsLong[decoder->sampRateIdx];
	}

	for (w = 0; w < num_windows; w++)
	{
		bottom = num_swb;

		for (f = 0; f < tns->n_filt; f++)
		{
			top = bottom;
			bottom = MAX(top - tns->length[f], 0);
			tns_order = MIN(tns->order[f], tns_max_order);
			if (!tns_order)
				continue;

			start = swb_offset[MIN(bottom, tns_max_band)];
			end   = swb_offset[MIN(top, tns_max_band)];
			
			size = end - start;
			if (size <= 0)
				continue;

			tns_decode_coef(tns_order, tns->coef_res+3,
				tns->coef_compress[f], tns->coef[f], decoder->tns_lpc);

			if (tns->direction[f])
			{
				inc = -1;
				start = end - 1;
			} else {
				inc = 1;
			}

			tns_ma_filter(&spec[(w*nshort)+start], size, inc, decoder->tns_lpc, tns_order);
		}

		tns++;
	}

	return 0;
}