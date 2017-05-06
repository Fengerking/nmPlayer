#ifndef STRUCT_H
#define STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NB_CUTOFF_DEFAULT	7
#define NB_BOOST_DEFAULT	6
#define NB_LEVEL_DEFAULT	0

#define MSR_ROOM_DEFAULT	2
#define MSR_BRIGHT_DEFAULT	0

#define HFE_DEPTH_DEFAULT	1


#define NB_CUTOFF_DEFAULT	7
#define NB_BOOST_DEFAULT	6
#define NB_LEVEL_DEFAULT	0

#define GEQ_NBANDS_DEFAULT	7
#define GEQ_PREAMP_DEFAULT	0
#define GEQ_MAXBOOST_DEFAULT	24
#define GEQ_BAND1_DEFAULT	6
#define GEQ_BAND2_DEFAULT	9
#define GEQ_BAND3_DEFAULT	9
#define GEQ_BAND4_DEFAULT	12
#define GEQ_BAND5_DEFAULT	15
#define GEQ_BAND6_DEFAULT	12
#define GEQ_BAND7_DEFAULT	6

typedef struct DOLBYEffectInfo {
	void*			hDEffect;
	int				nEffect;

	int				nInSampleRate;
	int				nOutSampleRate;
	int				nInSampleIndex;
	int				nOutSampleIndex;

	int				nInChannel;
	int				nOutChannel;
	int				nInChanIndex;
	int				nOutChanIndex;

	int				nUpsampleParam;
	int				nUpsampleNum;

	int				nInMatrix; 
	int				nChanFmt;
	int				nLfeMixLevel;
	int				nMSRMaxProfile;
	int				nBypass;

	int				nPortableModeEnable;
	int				nPortableModeGain;
	int				nPortableModeOrl;

	int				nPulseDownMix;
	int				nPulseDownMixAtten;

	int				nUPSampleEnable; 

	int				nM2SEnable; 

	int				nHFEEnable; 
	int				nHFEDepth;

	int				nMSREnable; 

	int				nMSRBright; 		
	int				nMSRRoom;	

	int				nNBEnable;
	int				nNBCutoff;
	int				nNBBoost;
	int				nNBLevel;

	int				nMupEnable;

	int				nGEQEnable;
	int				nGEQNband;
	int				nGEQPreamp;
	int				nGEQMaxBoost;
	int				nGEQBand1;
	int				nGEQBand2;
	int				nGEQBand3;
	int				nGEQBand4;
	int				nGEQBand5;
	int				nGEQBand6;
	int				nGEQBand7;


	unsigned char	*buf_ptr;                 //input_buffer pointer
	int            buf_len;                  //input_buffer length
	int            used_len;

	VO_MEM_OPERATOR *pvoMemop;
	VO_MEM_OPERATOR voMemoprator;
}DOLBYEffectInfo;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TDAS_H */


