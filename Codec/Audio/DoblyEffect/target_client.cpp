#include "compiler.h"

#include "target_client.h"
#include "control.h"
#include "tdas_client.h" 
#include "tdas_resampler_helper.h"

typedef struct {
	unsigned char nLfeMixLevel;		/**< level at which the LFE channel is mixed into the output audio */
	unsigned char  nSampleRateIndex;	/**< Output sample rate */
	unsigned char  nInChans;		/**< Channel format of input audio */
	unsigned char  nInMatrix;		/**< L/R mixing specification for stereo audio input */
	unsigned char  nBypass;			/**< Audio Processing bypass */
	unsigned char  nRateMultipier;		/**< Sample-rate multiplier (output with respect to input) */
	unsigned char  nInChanFormat;		/**< Input/Output channel format */
	unsigned char  nMsrMaxProfile;		/**< Maximum number of virtual rendering channels in Mobile Surround */
	unsigned int   nChannel;
}TDASSTRUCTURE;

void tdas_query_memory(unsigned int *n_persist, unsigned int *n_scratch)
{
	return;
}

void *tdas_init(void *v_persist, void* v_scratch)
{
	return 0;
}

void tdas_uninit(void *v_tdas)
{
	return;
}

void *tdas_open(void)
{
	TDASSTRUCTURE* tmp;

	tmp = new TDASSTRUCTURE;
	return (void *)tmp;
}

void tdas_close(void *v_tdas)
{
	delete []v_tdas;
	return;
}

void tdas_reset(void *v_tdas)
{
	
	return;
}

int tdas_process(void *v_tdas, IO_DATUM *in, IO_DATUM *out, int n_samples)
{
	int i, j, n, nUpSample;
	TDASSTRUCTURE* tdas;
	

	tdas = (TDASSTRUCTURE*)v_tdas;

	nUpSample = tdas_resampler_real_multiplier(tdas->nRateMultipier);

	n = 0;
	if(tdas->nChannel == 1) {

		for(i = 0; i < n_samples; i++)
		{
			for(j = 0; j < nUpSample; j++)
			{
				out[n++] = in[i];
				out[n++] = in[i];
			}
		}
	}
	else if(tdas->nChannel == 2)
	{
		for(i = 0; i < n_samples*tdas->nChannel; i += tdas->nChannel)
		{
			for(j = 0; j < nUpSample; j++)
			{
				out[n++] = in[i];
				out[n++] = in[i+1];
			}
		}
	}
	else if(tdas->nChannel > 2)
	{
		for(i = 0; i < n_samples*tdas->nChannel; i += tdas->nChannel)
		{
			for(j = 0; j < nUpSample; j++)
			{
				out[n++] = in[i];
				out[n++] = in[i+2];
			}
		}
	}

	return 0;
}

void tdas_set(void* state_ptr, int feature, int parameter, INTEGER value)
{
	TDASSTRUCTURE* tdas;

	tdas = (TDASSTRUCTURE*)state_ptr;

	if(feature == TDAS_BUNDLE) {
		switch(parameter)
		{
		case TDAS_SAMPLERATE:
			value = tdas->nSampleRateIndex;
			break;
		case TDAS_IN_CHANS:
			tdas->nInChans = value;
			break;
		case TDAS_RATE_MULTIPLIER:
			tdas->nRateMultipier = value;
			break;
		case TDAS_IN_CHAN_FORMAT:
			tdas->nInChanFormat = value;
			break;
		}

		if(parameter == TDAS_IN_CHANS)
		{
			switch (value)
			{
			case CHANS_C:
			case CHANS_C_LFE:
				tdas->nChannel = 1;
				break;

			case CHANS_L_R:
			case CHANS_L_R_LFE:
				tdas->nChannel = 2;
				break;

			case CHANS_L_C_R:
			case CHANS_L_C_R_LFE:
				tdas->nChannel = 3;
				break;

			case CHANS_L_R_S:

			case CHANS_L_R_S_LFE:
				tdas->nChannel = 3;
				break;

			case CHANS_L_C_R_S:
			case CHANS_L_C_R_S_LFE:
				tdas->nChannel = 4;
				break;

			case CHANS_L_R_Ls_Rs:
			case CHANS_L_R_Ls_Rs_LFE:
				tdas->nChannel = 4;
				break;

			case CHANS_L_C_R_Ls_Rs:
			case CHANS_L_C_R_Ls_Rs_LFE:
				tdas->nChannel = 5;
				break;

			case CHANS_L_R_Ls_Rs_Cs:
			case CHANS_L_R_Ls_Rs_Cs_LFE:
				tdas->nChannel = 5;
				break;

			case CHANS_L_R_Ls_Rs_Lb_Rb:
			case CHANS_L_R_Ls_Rs_Lb_Rb_LFE:
				tdas->nChannel = 6;
				break;

			case CHANS_L_C_R_Ls_Rs_Cs:
			case CHANS_L_C_R_Ls_Rs_Cs_LFE:
				tdas->nChannel = 6;
				break;

			case CHANS_L_R_Ls_Rs_Ts_Cs:
			case CHANS_L_R_Ls_Rs_Ts_Cs_LFE:
				tdas->nChannel = 6;
				break;

			case CHANS_L_C_R_Ls_Rs_Cvh_Cs:
			case CHANS_L_C_R_Ls_Rs_Cvh_Cs_LFE:
				tdas->nChannel = 7;
				break;

			case CHANS_L_C_R_Ls_Rs_Ts_Cs:
			case CHANS_L_C_R_Ls_Rs_Ts_Cs_LFE:
				tdas->nChannel = 7;
				break;

			case CHANS_L_C_R_Ls_Rs_Lb_Rb:
			case CHANS_L_C_R_Ls_Rs_Lb_Rb_LFE:
				tdas->nChannel = 7;
				break;
			}
			if (value >= CHANS_C_LFE)
			{
				tdas->nChannel ++;
			}
		}
	}

	return;
}

INTEGER tdas_get(const void* state_ptr, int feature, int parameter)
{
	TDASSTRUCTURE* tdas;
	int value = 0;

	tdas = (TDASSTRUCTURE*)state_ptr;

	if(feature == TDAS_BUNDLE) {
		switch(parameter)
		{
		case TDAS_SAMPLERATE:
			tdas->nSampleRateIndex = value;
			break;
		case TDAS_IN_CHANS:
			value = tdas->nInChans;
			break;
		case TDAS_RATE_MULTIPLIER:
			value = tdas->nRateMultipier;
			break;
		case TDAS_IN_CHAN_FORMAT:
			value = tdas->nInChanFormat;
			break;
		}
	}

	return value;
}

void tdas_set_coef(void* state_ptr, int feature, int index, int len, char *values)
{
	return;
}