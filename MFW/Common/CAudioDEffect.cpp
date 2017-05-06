	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "voString.h"

#include "compiler.h"

#include "target_client.h"
#include "control.h"
#include "tdas_client.h" 
#include "tdas_resampler_helper.h"

#include "CAudioDEffect.h"
#include "voLog.h"

#define LOG_TAG "CAudioDEffect"

#ifdef _WIN32
#define VO_AUDIO_DOBLY_EFFECT_SETTING _T("VOAudioSetting.txt")
#elif defined _LINUX
#define VO_AUDIO_DOBLY_EFFECT_SETTING _T("/sdcard/VOAudioSetting.txt")
#endif

CAudioDEffect::CAudioDEffect ()
	: CBaseNode (NULL, NULL)
	, m_hDEffect (NULL)
	, m_nInSampleRate(44100)
	, m_nOutSampleRate(44100)
	, m_nInChannel(2)
	, m_nOutChannel(2)
	, m_nInChanIndex(CHANS_L_R)
	, m_nOutChanIndex (CHANS_L_R)
	, m_nInSampleIndex (SAMPLERATE_44100)
	, m_nOutSampleIndex (SAMPLERATE_44100)
	, m_nUpsampleb(0)
	, m_nUpsampleParam(1)
	, m_bDoblyEffect(0)
{
	VOLOGF ();

	in_matrix = MATRIX_PL2;
	chan_fmt = CHAN_FORMAT_INTERLACED;

	bypass = 0;

	twoup_enable = 0;

	sparkler_enable = 0;
	sparkler_depth = 0;

	stereowrapper_enable = 0;
	stereowrapper_width = 1;
	stereowrapper_spk_mode = 0;

	mobilesurround_enable = 0;
	mobilesurround_max_profile = TDAS_MSR_DEF_MAX_PROFILE;		
	mobilesurround_brightness = MSR_DEF_BRIGHT;	
	mobilesurround_room_mode = MSR_DEF_ROOM;	

	latenight_enable = 0;
	latenight_level = 0;
	latenight_depth = 0;

	volume_enable = 0;
	volume_tdas_gain = 0;
	volume_external_gain = 0;
	volume_scale_type = VOL_LOGARITHMIC;
	volume_balance = 0;
	volume_mute = 0;

	rumbler_enable = 0;
	rumbler_cutoff = 0;
	rumbler_boost = 0;
	rumbler_level = 0;

	fade_enable = 0;
	fade_target = 0;
	fade_time = 256;

	spkeq_enable = 0;
	spkeq_left_gaindB = 0;
	spkeq_right_gaindB = 0;

	dmx_lfe_level = DMX_LFE_MIX_DEFAULT;

	bMupEnable = 0;

	geq_enable = 0;
	geq_n_bands = GEQ_N_BANDS_MIN;
	geq_preamp = GEQ_DEFAULT_PREAMP;
	geq_maxboost = GEQ_DEFAULT_MAXBOOST;
	geq_band = new VO_S32[GEQ_N_BANDS_MAX];

	int i;

	for(i = 0; i < GEQ_N_BANDS_MAX; i++)
	{
		geq_band[i] = 0;
	}

	m_pCfgEffect = new CBaseConfig();

	m_hDEffect = tdas_open();

	VOLOGI ("Open audio effect handle successful!")
}

CAudioDEffect::~CAudioDEffect ()
{
	VOLOGF ();

	Uninit ();
	
	if(m_pCfgEffect)
	{
		delete m_pCfgEffect;
		m_pCfgEffect = NULL;
	}

	if(geq_band)
	{
		delete geq_band;
		geq_band = NULL;
	}

	voCAutoLock lock (&m_Mutex);

	tdas_close(m_hDEffect);

}

VO_U32 CAudioDEffect::Init (VO_AUDIO_FORMAT * pFormat)
{
	VOLOGF ();

	m_nInChannel = pFormat->Channels;
	m_nInSampleRate = pFormat->SampleRate;

	if(m_hDEffect)
	{
		voCAutoLock lock (&m_Mutex);

		tdas_reset(m_hDEffect);
	}
	else
	{
		m_hDEffect = tdas_open();
		if(m_hDEffect == NULL)
			return VO_ERR_FAILED;
	}

	UpdateParam();
	
	return VO_ERR_NONE;
}

VO_U32 CAudioDEffect::UpdateParam (void)
{
	VOLOGF ();

	//VO_S32 tGetValue;

	if (!m_pCfgEffect->Open (VO_AUDIO_DOBLY_EFFECT_SETTING))
	{
		VOLOGI ("Open audio effect setting file error! %s", VO_AUDIO_DOBLY_EFFECT_SETTING);
	}
	else
	{
		VOLOGI ("Open audio effect setting file successful!")
	}

	switch(m_nInChannel)
	{
	case 1:
		m_nInChanIndex = CHANS_C;
		break;
	case 2:
		m_nInChanIndex = CHANS_L_R;
		break;
	case 3:
		m_nInChanIndex = CHANS_L_C_R;
		break;
	case 4:
		m_nInChanIndex = CHANS_L_R_Ls_Rs;
		break;
	case 5:
		m_nInChanIndex = CHANS_L_C_R_Ls_Rs;
		break;
	case 6:
		m_nInChanIndex = CHANS_L_C_R_Ls_Rs_LFE;
		break;
	}

	switch(m_nInSampleRate)
	{
	case 48000:
		m_nInSampleIndex = SAMPLERATE_48000;
		break;
	case 44100:
		m_nInSampleIndex = SAMPLERATE_44100;
		break;
	case 32000:
		m_nInSampleIndex = SAMPLERATE_32000;
		break;
	case 24000:
		m_nInSampleIndex = SAMPLERATE_24000;
		break;
	case 22050:
		m_nInSampleIndex = SAMPLERATE_22050;
		break;
	case 16000:
		m_nInSampleIndex = SAMPLERATE_16000;
		break;
	case 12000:
		m_nInSampleIndex = SAMPLERATE_12000;
		break;
	case 11025:
		m_nInSampleIndex = SAMPLERATE_11025;
		break;
	case 8000:
		m_nInSampleIndex = SAMPLERATE_8000;
		break;
	}
	
	m_nUpsampleb = m_pCfgEffect->GetItemValue((char *)"Upsample", (char *)"UPSAMP_ENABLE",  m_nUpsampleb);

	if(m_nUpsampleb)
	{
		m_nOutSampleIndex = (int) tdas_resampler_get_output_samplerate((int) m_nInSampleIndex);
		m_nUpsampleParam = (int) tdas_resampler_get_multiplier((int) m_nInSampleIndex);
	}
	else
	{
		m_nOutSampleIndex = m_nInSampleIndex;
		m_nUpsampleParam = SAMPLERATE_MULTIPLIER_1;
	}

	m_nUpsampleNum = tdas_resampler_real_multiplier((int) m_nUpsampleParam);

	m_nOutSampleRate = m_nUpsampleNum*m_nInSampleRate;

	m_nOutChannel = 2;
	m_nOutChanIndex = CHANS_L_R;

	bypass = 0;

//	m_bDoblyEffect = m_pCfgEffect->GetItemValue((char *)"Auto", 
//		(char *)"Auto",  m_bDoblyEffect);
//	VOLOGI ("Auto = %d", m_bDoblyEffect);

	tdas_reset(m_hDEffect);

	SetParameter (TDAS_BUNDLE, TDAS_SAMPLERATE, m_nOutSampleIndex);
	SetParameter (TDAS_BUNDLE, TDAS_IN_CHANS, m_nInChanIndex);
	SetParameter (TDAS_BUNDLE, TDAS_IN_MATRIX, in_matrix);
	SetParameter (TDAS_BUNDLE, TDAS_RATE_MULTIPLIER, m_nUpsampleParam);
	SetParameter (TDAS_BUNDLE, TDAS_IN_CHAN_FORMAT, chan_fmt);
	SetParameter (TDAS_BUNDLE, TDAS_LFE_MIX_LEVEL, dmx_lfe_level);
	SetParameter (TDAS_BUNDLE, TDAS_MSR_MAX_PROFILE, mobilesurround_max_profile);


	if(m_bDoblyEffect == 0)
	{
		bypass = 1;
		SetParameter (TDAS_BUNDLE, TDAS_BYPASS, bypass);
		VOLOGI("Dolby Effect disable");
		return 0;
	}
	else
	{
		bypass = 0;
		SetParameter (TDAS_BUNDLE, TDAS_BYPASS, bypass);
		VOLOGI("Dolby Effect enable");
	}

	twoup_enable = 0;

	sparkler_enable = 0;
	sparkler_depth = 0;

	stereowrapper_enable = 0;
	stereowrapper_width = 1;
	stereowrapper_spk_mode = 0;

	mobilesurround_enable = 0;
	mobilesurround_max_profile = TDAS_MSR_DEF_MAX_PROFILE;		
	mobilesurround_brightness = MSR_DEF_BRIGHT;	
	mobilesurround_room_mode = MSR_DEF_ROOM;	


	rumbler_enable = 0;
	rumbler_cutoff = 0;
	rumbler_boost = 0;
	rumbler_level = 0;

	spkeq_enable = 0;
	spkeq_left_gaindB = 0;
	spkeq_right_gaindB = 0;

	dmx_lfe_level = DMX_LFE_MIX_DEFAULT;

	bMupEnable = 0;

	geq_enable = 0;
	geq_n_bands = GEQ_N_BANDS_MIN;
	geq_preamp = GEQ_DEFAULT_PREAMP;
	geq_maxboost = GEQ_DEFAULT_MAXBOOST;

	int i;

	for(i = 0; i < GEQ_N_BANDS_MAX; i++)
	{
		geq_band[i] = 0;
	}
	
	//Mobile Surround 
	mobilesurround_enable = m_pCfgEffect->GetItemValue((char *)"MobileSurround", 
		(char *)"MSR_ENABLE",  mobilesurround_enable);
	VOLOGI ("MSR_ENABLE = %d", mobilesurround_enable);
	SetParameter (TDAS_MSR,	   MSR_ENABLE, mobilesurround_enable);

	if(mobilesurround_enable) {
		mobilesurround_room_mode = m_pCfgEffect->GetItemValue((char *)"MobileSurround", 
			(char *)"MSR_ROOM",  mobilesurround_room_mode);
		VOLOGI ("MSR_ROOM = %d", mobilesurround_room_mode);
		SetParameter (TDAS_MSR,	   MSR_ROOM, mobilesurround_room_mode);

		mobilesurround_brightness = m_pCfgEffect->GetItemValue((char *)"MobileSurround", 
			(char *)"MSR_BRIGHT",  mobilesurround_brightness);
		VOLOGI ("MSR_BRIGHT = %d", mobilesurround_brightness);
		SetParameter (TDAS_MSR,	   MSR_BRIGHT, mobilesurround_brightness);
	}

	//Mobile Surround Upmixer
	bMupEnable = m_pCfgEffect->GetItemValue((char *)"MobileSurroundUpmixer", 
		(char *)"MUP_ENABLE",  bMupEnable);
	VOLOGI ("MUP_ENABLE = %d", bMupEnable);
	SetParameter (TDAS_MUP,	   MUP_ENABLE, bMupEnable);

	//High Frequency Enhancer
	sparkler_enable = m_pCfgEffect->GetItemValue((char *)"HighFrequencyEnhancer", 
		(char *)"HFE_ENABLE",  sparkler_enable);
	VOLOGI ("HFE_ENABLE = %d", sparkler_enable);
	SetParameter (TDAS_HFE, HFE_ENABLE, sparkler_enable);

	if(sparkler_enable) {
		sparkler_depth = m_pCfgEffect->GetItemValue((char *)"HighFrequencyEnhancer", 
			(char *)"HFE_DEPTH",  sparkler_depth);
		VOLOGI ("HFE_DEPTH = %d", sparkler_depth);
		SetParameter (TDAS_HFE, HFE_DEPTH, sparkler_depth);
	}

	//Sound Space Expander
	stereowrapper_enable = m_pCfgEffect->GetItemValue((char *)"SoundSpaceExpande", 
		(char *)"SSE_ENABLE",  stereowrapper_enable);
	VOLOGI ("SSE_ENABLE = %d", stereowrapper_enable);
	SetParameter (TDAS_SSE, SSE_ENABLE, stereowrapper_enable);

	if(stereowrapper_enable) {	
		stereowrapper_width = m_pCfgEffect->GetItemValue((char *)"SoundSpaceExpande", 
			(char *)"SSE_WIDTH",  stereowrapper_width);
		VOLOGI ("SSE_WIDTH = %d", stereowrapper_width);
		SetParameter (TDAS_SSE, SSE_WIDTH, stereowrapper_width);

		stereowrapper_spk_mode = m_pCfgEffect->GetItemValue((char *)"SoundSpaceExpande", 
			(char *)"SSE_SPK_MODE",  stereowrapper_spk_mode);
		VOLOGI ("SSE_SPK_MODE = %d", stereowrapper_spk_mode);
		SetParameter (TDAS_SSE, SSE_SPK_MODE, stereowrapper_spk_mode);
	}
	
	//Mono-to-Stereo Creator 
	twoup_enable = m_pCfgEffect->GetItemValue((char *)"MonoToStereoCreator", 
		(char *)"M2S_ENABLE",  twoup_enable);
	VOLOGI ("M2S_ENABLE = %d", twoup_enable);
	SetParameter (TDAS_M2S, M2S_ENABLE, twoup_enable);

	//Natural Bass
	rumbler_enable = m_pCfgEffect->GetItemValue((char *)"NaturalBass", 
		(char *)"NB_ENABLE",  rumbler_enable);
	VOLOGI ("NB_ENABLE = %d", rumbler_enable);
	SetParameter (TDAS_NB, NB_ENABLE, rumbler_enable);

	if(rumbler_enable) {
		rumbler_cutoff = m_pCfgEffect->GetItemValue((char *)"NaturalBass", 
			(char *)"NB_CUTOFF",  rumbler_cutoff);
		VOLOGI ("NB_CUTOFF = %d", rumbler_cutoff);
		SetParameter (TDAS_NB, NB_CUTOFF, rumbler_cutoff);

		rumbler_boost = m_pCfgEffect->GetItemValue((char *)"NaturalBass", 
			(char *)"NB_BOOST",  rumbler_boost);
		VOLOGI ("NB_BOOST = %d", rumbler_boost);
		SetParameter (TDAS_NB, NB_BOOST,rumbler_boost);

		rumbler_level = m_pCfgEffect->GetItemValue((char *)"NaturalBass", 
			(char *)"NB_LEVEL",  rumbler_level);
		VOLOGI ("NB_LEVEL = %d", rumbler_level);
		SetParameter (TDAS_NB, NB_LEVEL, rumbler_level);
	}

	// Graphic EQ		
	geq_enable = m_pCfgEffect->GetItemValue((char *)"GraphicEQ", 
		(char *)"GEQ_ENABLE",  geq_enable);
	VOLOGI ("GEQ_ENABLE = %d", geq_enable);
	SetParameter (TDAS_GEQ, GEQ_ENABLE, geq_enable);

	if(geq_enable)
	{
		geq_n_bands = m_pCfgEffect->GetItemValue((char *)"GraphicEQ", 
			(char *)"GEQ_NBANDS",  geq_n_bands);
		VOLOGI ("GEQ_NBANDS = %d", geq_n_bands);
		SetParameter (TDAS_GEQ, GEQ_NBANDS, geq_n_bands);

		geq_preamp = m_pCfgEffect->GetItemValue((char *)"GraphicEQ", 
			(char *)"GEQ_PREAMP",  geq_preamp);
		VOLOGI ("GEQ_PREAMP = %d", geq_preamp);
		SetParameter (TDAS_GEQ, GEQ_PREAMP, geq_preamp);

		geq_maxboost = m_pCfgEffect->GetItemValue((char *)"GraphicEQ", 
			(char *)"GEQ_MAXBOOST",  geq_maxboost);
		VOLOGI ("GEQ_MAXBOOST = %d", geq_maxboost);
		SetParameter (TDAS_GEQ, GEQ_MAXBOOST, geq_maxboost);

		geq_band[0] = m_pCfgEffect->GetItemValue((char *)"GraphicEQ", 
			(char *)"GEQ_BAND1",  geq_band[0]);
		VOLOGI ("GEQ_BAND1 = %d", geq_band[0]);
		SetParameter (TDAS_GEQ, GEQ_BAND1, geq_band[0]);

		geq_band[1] = m_pCfgEffect->GetItemValue((char *)"GraphicEQ", 
			(char *)"GEQ_BAND2",  geq_band[1]);
		VOLOGI ("GEQ_BAND2 = %d", geq_band[1]);
		SetParameter (TDAS_GEQ, GEQ_BAND2, geq_band[1]);

		geq_band[2] = m_pCfgEffect->GetItemValue((char *)"GraphicEQ", 
			(char *)"GEQ_BAND3",  geq_band[2]);
		VOLOGI ("GEQ_BAND3 = %d", geq_band[2]);
		SetParameter (TDAS_GEQ, GEQ_BAND3, geq_band[2]);

		geq_band[3] = m_pCfgEffect->GetItemValue((char *)"GraphicEQ", 
			(char *)"GEQ_BAND4",  geq_band[3]);
		VOLOGI ("GEQ_BAND4 = %d", geq_band[3]);
		SetParameter (TDAS_GEQ, GEQ_BAND4, geq_band[3]);

		geq_band[4] = m_pCfgEffect->GetItemValue((char *)"GraphicEQ", 
			(char *)"GEQ_BAND5",  geq_band[4]);
		VOLOGI ("GEQ_BAND5 = %d", geq_band[4]);
		SetParameter (TDAS_GEQ, GEQ_BAND5, geq_band[4]);

		geq_band[5] = m_pCfgEffect->GetItemValue((char *)"GraphicEQ", 
			(char *)"GEQ_BAND6",  geq_band[5]);
		VOLOGI ("GEQ_BAND6 = %d", geq_band[5]);
		SetParameter (TDAS_GEQ, GEQ_BAND6, geq_band[5]);

		geq_band[6] = m_pCfgEffect->GetItemValue((char *)"GraphicEQ", 
			(char *)"GEQ_BAND7",  geq_band[6]);
		VOLOGI ("GEQ_BAND7 = %d", geq_band[6]);
		SetParameter (TDAS_GEQ, GEQ_BAND7, geq_band[6]);
	}

//	m_bDoblyEffect = (m_nUpsampleb | rumbler_enable | sparkler_enable | mobilesurround_enable |
//					bMupEnable | sparkler_enable | twoup_enable | rumbler_enable | geq_enable);

	return 0;
}

VO_U32 CAudioDEffect::Uninit (void)
{
	VOLOGF ();

	return 0;
}

VO_U32 CAudioDEffect::Start(void)
{
	VOLOGF ();

	return 	0;
}

VO_U32 CAudioDEffect::Pause(void)
{
	VOLOGF ();

	return 	0;
}

VO_U32 CAudioDEffect::Stop(void)
{
	VOLOGF ();

	return 	0;
}


VO_U32 CAudioDEffect::Process(VO_CODECBUFFER * pInput, VO_CODECBUFFER * pOutput)
{
	VOLOGF ();

	VO_S16 *in, *out;
	VO_S32 n_samples;
	VO_S32 out_rate_multiplier;
	
	if (m_hDEffect == 0)
		return VO_ERR_WRONG_STATUS;
	
	in = (VO_S16 *)pInput->Buffer;
	n_samples = pInput->Length;

	n_samples = n_samples / (m_nInChannel * sizeof(VO_S16));

	out = (VO_S16 *)pOutput->Buffer;

	voCAutoLock lock (&m_Mutex);

	out_rate_multiplier = tdas_resampler_real_multiplier(tdas_get(m_hDEffect, TDAS_BUNDLE, TDAS_RATE_MULTIPLIER));

	if(!(n_samples & 63))
	{
		tdas_process(m_hDEffect, in, out, n_samples);
		//VOLOGI ("Using Dolby effect setting !")
	}
	else
	{
		int i, n = 0;
		if(m_nInChannel == 1) {

			for(i = 0; i < n_samples; i++)
			{
				out[n++] = in[i];
				out[n++] = in[i];
			}
		}
		else if(m_nInChannel == 2)
		{
			for(i = 0; i < n_samples*m_nInChannel; i += m_nInChannel)
			{
				out[n++] = in[i];
				out[n++] = in[i+1];
			}
		}
		else
		{
			for(i = 0; i < n_samples*m_nInChannel; i += m_nInChannel)
			{
				out[n++] = in[i];
				out[n++] = in[i+2];
			}
		}

		//VOLOGI ("no Dolby effect...!")
	}

	pOutput->Length = pInput->Length * out_rate_multiplier * m_nOutChannel / m_nInChannel;

	return VO_ERR_NONE;
}

VO_U32 CAudioDEffect::Flush (void)
{
	VOLOGF ();

	if (m_hDEffect == 0)
		return VO_ERR_WRONG_STATUS;

	return 0;
}

VO_U32 CAudioDEffect::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	VOLOGF ();

	VO_AUDIO_FORMAT * pFormat;
	
	switch(uParamID)
	{
	case VO_PID_AUDIO_SAMPLEREATE:
		m_nInSampleRate = *((VO_U32 *)pData);
		break;
	case VO_PID_AUDIO_CHANNELS:
		m_nInChannel = *((VO_U32 *)pData);
		break;
	case VO_PID_AUDIO_FORMAT:
		pFormat = (VO_AUDIO_FORMAT *)pData;
		m_nInChannel = pFormat->Channels;
		m_nInSampleRate = pFormat->SampleRate;
		break;
	case VO_PID_AUDIO_CHANNELCONFIG:		
		m_bDoblyEffect = *((int *)pData);
		VOLOGI ("Dolby effect enable or disable...%d!", m_bDoblyEffect)
	    break;
	}

	UpdateParam();
	
	return 	0;
}

VO_U32 CAudioDEffect::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	VOLOGF ();

	VO_AUDIO_FORMAT * pFormat;
	
	switch(uParamID)
	{
	case VO_PID_AUDIO_SAMPLEREATE:
		VOLOGS ("Getting sampleRate.......");
		*((VO_U32 *)pData) = m_nOutSampleRate;
		break;
	case VO_PID_AUDIO_CHANNELS:
		VOLOGS ("Getting outChannel.......");
		*((VO_U32 *)pData) = m_nOutChannel;
		break;
	case VO_PID_AUDIO_FORMAT:
		VOLOGS ("Getting format.......");
		pFormat = (VO_AUDIO_FORMAT *)pData;
		pFormat->Channels = m_nOutChannel;
		pFormat->SampleRate = m_nOutSampleRate;
		break;
	case VO_PID_AUDIO_CHANNELCONFIG:
		*((int *)pData) = m_bDoblyEffect;
	    break;
	}

	return 0;
}

VO_U32 CAudioDEffect::GetFormat (VO_AUDIO_FORMAT * pFormat)
{
	VOLOGF ();

	pFormat->SampleRate = m_nOutSampleRate;
	pFormat->Channels = m_nOutChannel;
	pFormat->SampleBits = 16;

	return VO_ERR_NONE;
}

VO_U32 CAudioDEffect::SetParameter (VO_S32 feature, VO_S32 parameter, VO_S32 Data)
{
	VOLOGF ("Audio Effect Mode and parameter. S %d, C %d", feature, parameter);

	if (m_hDEffect == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	tdas_set(m_hDEffect, feature, parameter, Data);

	return 0;
}

VO_U32 CAudioDEffect::GetParameter (VO_S32 feature, VO_S32 parameter)
{
	VOLOGF ();
	
	VO_U32 nRc;

	voCAutoLock lock (&m_Mutex);

	if (m_hDEffect == 0)
		return VO_ERR_WRONG_STATUS;

	nRc = tdas_get(m_hDEffect, feature, parameter);

	return nRc;

}

VO_U32 CAudioDEffect::SetCoef (VO_S32 feature, VO_S32 parameter, VO_S32 len, VO_PTR pData)
{
	VOLOGF ();

	voCAutoLock lock (&m_Mutex);

	if (m_hDEffect == 0)
		return VO_ERR_WRONG_STATUS;

	tdas_set_coef(m_hDEffect, feature, parameter, len, (char *)pData);

	return 0;

}

VO_U32 CAudioDEffect::LoadLib (VO_HANDLE hInst)
{
	VOLOGF ();

	return 1;
}


extern "C"   {
	void __assert_fail(const char* c1, const char*c2, unsigned int n, const char* c3)
	{

		printf("---->>>> ASSERT");

	}
}
