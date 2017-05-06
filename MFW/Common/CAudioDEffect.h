	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioDecoder.h

	Contains:	CAudioDecoder header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CAudioDEffect_H__
#define __CAudioDEffect_H__

//#ifdef _LINUX
//#define _LINUX_ANDROID
//#define _VOLOG_INFO
////#define _VOLOG_FUNC
//#endif

#include <voAudio.h>
#include "CBaseNode.h"
#include "CBaseConfig.h"

class CAudioDEffect : public CBaseNode
{
public:
	CAudioDEffect ();
	virtual ~CAudioDEffect (void);

	virtual VO_U32		Init (VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32		Uninit (void);

	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);

	virtual VO_U32		Process(VO_CODECBUFFER * pInput, VO_CODECBUFFER * pOutput);
	virtual VO_U32		Flush (void);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

	virtual VO_U32		SetParameter (VO_S32 feature, VO_S32 parameter, VO_S32 Data);
	virtual VO_U32		GetParameter (VO_S32 feature, VO_S32 parameter);

	virtual VO_U32		SetCoef (VO_S32 feature, VO_S32 parameter, VO_S32 len, VO_PTR pData);

	virtual VO_U32		GetFormat (VO_AUDIO_FORMAT * pFormat);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	virtual VO_U32		UpdateParam (void);

protected:
	VO_HANDLE			m_hDEffect;
	VO_U32				m_nEffect;

	VO_U32				m_nInSampleRate;
	VO_U32				m_nOutSampleRate;
	VO_U32				m_nInSampleIndex;
	VO_U32				m_nOutSampleIndex;

	VO_U32				m_nInChannel;
	VO_U32				m_nOutChannel;
	VO_U32				m_nInChanIndex;
	VO_U32				m_nOutChanIndex;

	VO_S32				m_nUpsampleb;
	VO_S32				m_nUpsampleParam;
	VO_S32				m_nUpsampleNum;

	VO_S32				in_matrix; 
    VO_S32				chan_fmt;
    VO_S32				bypass;

	VO_S32				twoup_enable; 

	VO_S32				sparkler_enable; 
	VO_S32				sparkler_depth;

	VO_S32				stereowrapper_enable; 
	VO_S32				stereowrapper_width; 
	VO_S32				stereowrapper_spk_mode; 

	VO_S32				mobilesurround_enable; 
	VO_S32				mobilesurround_max_profile; 	
	VO_S32				mobilesurround_brightness; 		
	VO_S32				mobilesurround_room_mode;	

	VO_S32				latenight_enable; 
	VO_S32				latenight_level;
	VO_S32				latenight_depth; 

	VO_S32				volume_enable; 
    VO_S32				volume_tdas_gain;
    VO_S32				volume_external_gain; 
    VO_S32				volume_scale_type; 
    VO_S32				volume_balance;
    VO_S32				volume_mute;

	VO_S32				rumbler_enable;
	VO_S32				rumbler_cutoff;
	VO_S32				rumbler_boost;
	VO_S32				rumbler_level;

	VO_S32				fade_enable;
	VO_S32				fade_target;
	VO_S32				fade_time;

	VO_S32				spkeq_enable;
	VO_S32				spkeq_left_gaindB;
	VO_S32				spkeq_right_gaindB;

	VO_S32				dmx_lfe_level;

	VO_S32				m_bDoblyEffect;

	VO_S32				bMupEnable;

	VO_S32				geq_enable;
	VO_S32				geq_n_bands;
	VO_S32				geq_preamp;
	VO_S32				geq_maxboost;
	VO_S32				*geq_band;

	CBaseConfig *		m_pCfgEffect;

	VO_AUDIO_FORMAT		m_fmtAudio;
};

#endif // CAudioDEffect
