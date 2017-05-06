#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "voEqualizer.h"
#define MAX_DB	20
#define MAX_CHANS 8
#define EQZ_IN_FACTOR 2

typedef struct
{
	fix_t alpha;
	fix_t beta;
	fix_t gamma;
	fix_t alpha0;	
} eqfilter;

typedef struct
{
	eqfilter	Filter[MAXFILTER];
	fix_t		XBuf[MAX_CHANS][2];
	fix_t		YBuf[MAX_CHANS][MAXFILTER][2];
	int			nEqCoef[MAXFILTER];
	int         nEqualizerOn;
	int         nVolume;
	int			Amplify;	//-20..20
	int			nSurroundOn;
	int			IsHeadPhone;
	int			channel;
	int			samplerate;
	int			eq_mode;
} Equalizer;

/* Value from equ-xmms */
static const eqfilter Bands[][MAXFILTER] =
{
	///eqfilter Band48000[MAXFILTER]=
	{
		{FIXC(0.002769),FIXC(0.994462),FIXC(1.994400)},
		{FIXC(0.007806),FIXC(0.984388),FIXC(1.983897)},
		{FIXC(0.014143),FIXC(0.971714),FIXC(1.970091)},
		{FIXC(0.027011),FIXC(0.945978),FIXC(1.939979)},
		{FIXC(0.044203),FIXC(0.911595),FIXC(1.895241)},
		{FIXC(0.121223),FIXC(0.757553),FIXC(1.623767)},
		{FIXC(0.212888),FIXC(0.574224),FIXC(1.113145)},
		{FIXC(0.331347),FIXC(0.337307),FIXC(-0.000000)},
		{FIXC(0.355263),FIXC(0.289473),FIXC(-0.333740)},
		{FIXC(0.371900),FIXC(0.256201),FIXC(-0.628100)},
	},
	///eqfilter Band44100[MAXFILTER]=
	{
		{FIXC(0.003013),FIXC(0.993973),FIXC(1.993901)},
		{FIXC(0.008490),FIXC(0.983019),FIXC(1.982437)},
		{FIXC(0.015374),FIXC(0.969252),FIXC(1.967331)},
		{FIXC(0.029328),FIXC(0.941343),FIXC(1.934254)},
		{FIXC(0.047918),FIXC(0.904163),FIXC(1.884869)},
		{FIXC(0.130408),FIXC(0.739184),FIXC(1.582718)},
		{FIXC(0.226555),FIXC(0.546889),FIXC(1.015267)},
		{FIXC(0.344937),FIXC(0.310127),FIXC(-0.181410)},
		{FIXC(0.366438),FIXC(0.267123),FIXC(-0.521151)},
		{FIXC(0.379009),FIXC(0.241981),FIXC(-0.808451)},
	},
	///eqfilter Band32000[MAXFILTER]=
	{
		{FIXC(0.003113),FIXC(0.993773),FIXC(1.993695)},
		{FIXC(0.006207),FIXC(0.987585),FIXC(1.987275)},
		{FIXC(0.012436),FIXC(0.975128),FIXC(1.973875)},
		{FIXC(0.024563),FIXC(0.950875),FIXC(1.945927)},
		{FIXC(0.047918),FIXC(0.904163),FIXC(1.884869)},
		{FIXC(0.091243),FIXC(0.817514),FIXC(1.744223)},
		{FIXC(0.165797),FIXC(0.668405),FIXC(1.404719)},
		{FIXC(0.275708),FIXC(0.448584),FIXC(0.605175)},
		{FIXC(0.379009),FIXC(0.241981),FIXC(-0.808451)},
		{FIXC(0.332734),FIXC(0.334532),FIXC(-1.334499)},
	},
	///eqfilter Band22050[MAXFILTER]=
	{
		{FIXC(0.003113),FIXC(0.993773),FIXC(1.993695)},
		{FIXC(0.006207),FIXC(0.987585),FIXC(1.987275)},
		{FIXC(0.012436),FIXC(0.975128),FIXC(1.973875)},
		{FIXC(0.024563),FIXC(0.950875),FIXC(1.945927)},
		{FIXC(0.047918),FIXC(0.904163),FIXC(1.884869)},
		{FIXC(0.091243),FIXC(0.817514),FIXC(1.744223)},
		{FIXC(0.165797),FIXC(0.668405),FIXC(1.404719)},
		{FIXC(0.275708),FIXC(0.448584),FIXC(0.605175)},
		{FIXC(0.379009),FIXC(0.241981),FIXC(-0.808451)},
		{FIXC(0.332734),FIXC(0.334532),FIXC(-1.334499)},
	},
	///eqfilter Band16000[MAXFILTER]=
	{
		{FIXC(0.006207),FIXC(0.987585),FIXC(1.987275)},
		{FIXC(0.012338),FIXC(0.975325),FIXC(1.974092)},
		{FIXC(0.024563),FIXC(0.950875),FIXC(1.945927)},
		{FIXC(0.047918),FIXC(0.904163),FIXC(1.884869)},
		{FIXC(0.091243),FIXC(0.817514),FIXC(1.744223)},
		{FIXC(0.165797),FIXC(0.668405),FIXC(1.404719)},
		{FIXC(0.275708),FIXC(0.448584),FIXC(0.605175)},
		{FIXC(0.344937),FIXC(0.310127),FIXC(-0.181410)},
		{FIXC(0.379009),FIXC(0.241981),FIXC(-0.808451)},
		{FIXC(0.332734),FIXC(0.334532),FIXC(-1.334499)},
	},
	///eqfilter Band11025[MAXFILTER]=
	{
		{FIXC(0.006207),FIXC(0.987585),FIXC(1.987275)},
		{FIXC(0.012338),FIXC(0.975325),FIXC(1.974092)},
		{FIXC(0.024563),FIXC(0.950875),FIXC(1.945927)},
		{FIXC(0.047918),FIXC(0.904163),FIXC(1.884869)},
		{FIXC(0.091243),FIXC(0.817514),FIXC(1.744223)},
		{FIXC(0.165797),FIXC(0.668405),FIXC(1.404719)},
		{FIXC(0.275708),FIXC(0.448584),FIXC(0.605175)},
		{FIXC(0.344937),FIXC(0.310127),FIXC(-0.181410)},
		{FIXC(0.379009),FIXC(0.241981),FIXC(-0.808451)},
		{FIXC(0.332734),FIXC(0.334532),FIXC(-1.334499)},
	},
	///eqfilter Band8000[MAXFILTER]=
	{
		{FIXC(0.008534),FIXC(0.982931),FIXC(1.982343)},
		{FIXC(0.008534),FIXC(0.982931),FIXC(1.982343)},
		{FIXC(0.008534),FIXC(0.982931),FIXC(1.982343)},
		{FIXC(0.016923),FIXC(0.966154),FIXC(1.963823)},
		{FIXC(0.033533),FIXC(0.932935),FIXC(1.923627)},
		{FIXC(0.064816),FIXC(0.870368),FIXC(1.834429)},
		{FIXC(0.121223),FIXC(0.757553),FIXC(1.623767)},
		{FIXC(0.212888),FIXC(0.574224),FIXC(1.113145)},
		{FIXC(0.331347),FIXC(0.337307),FIXC(-0.000000)},
		{FIXC(0.331347),FIXC(0.337307),FIXC(-1.337307)},
	},				
};

static const fix_t PowTab[37] = //pow(10,(n-18)/20.f))
{
	FIXC(0.125892541),FIXC(0.141253754),FIXC(0.158489319),FIXC(0.177827941),
	FIXC(0.199526231),FIXC(0.223872114),FIXC(0.251188643),FIXC(0.281838293),
	FIXC(0.316227766),FIXC(0.354813389),FIXC(0.398107171),FIXC(0.446683592),
	FIXC(0.501187234),FIXC(0.562341325),FIXC(0.630957344),FIXC(0.707945784),
	FIXC(0.794328235),FIXC(0.891250938),FIXC(1.000000000),FIXC(1.122018454),
	FIXC(1.258925412),FIXC(1.412537545),FIXC(1.584893192),FIXC(1.778279410),
	FIXC(1.995262315),FIXC(2.238721139),FIXC(2.511886432),FIXC(2.818382931),
	FIXC(3.162277660),FIXC(3.548133892),FIXC(3.981071706),FIXC(4.466835922),
	FIXC(5.011872336),FIXC(5.623413252),FIXC(6.309573445),FIXC(7.079457844),
	FIXC(7.943282347),
};

#define FIXC2(a) (int32_t)((a)<0?(a-0.5):(a+0.5))
fix_t EQ_Preset[][MAXFILTER]=//the Number MAXFILTER+1 is the preamp
{
	/*normal*/	{0,0,0,0,0,0,0,0,0,0},
	/*pop*/		{ -2,	5,	7,	8,	6,	6,	2,	0, -2, -2	},
	/*rock*/	{ 8,	5,	-5, -8, -3, 4,	9,	11, 11, 11	},
	/*jazz*/	{ 2,	2,	4,	6,	6,	6,	0,	5,	9,	9	},
	/*dance*/	{ 5,	4,	0,	6,	6,	4,	6,	9,	4,	6	},
	/*classic*/	{ 8,	5,	0, -8, -3,  0,	5,	9,  6,  4	},
	/*live*/		{ -3,	-1, 4,	4,	-2,	-2, 2,	2,	6,	9	},	
};


#define UP_Limit 16
static fix_t Pow(int n)
{
	if (n<-UP_Limit) n=-UP_Limit;
	if (n>UP_Limit) n=UP_Limit;
	return PowTab[n+18];
}

VO_S32	voEQInit(VO_HANDLE *phEQ)
{
	Equalizer* eqobj;

	eqobj = (Equalizer *)malloc(sizeof(Equalizer));
	if(eqobj ==  NULL)
		return -1;
	
	memset(eqobj, 0, sizeof(Equalizer));
	
	eqobj->Amplify = 2;
	eqobj->samplerate = sr_44100;
	eqobj->channel = 2;
	eqobj->eq_mode = 1;

	memcpy(eqobj->nEqCoef, EQ_Preset[1], 10*sizeof(int));

	eqobj->Amplify = Pow(eqobj->Amplify);

	*phEQ = (VO_HANDLE)eqobj;
	
	return VO_ERR_NONE;
}

VO_S32	voEQProcess(VO_HANDLE hCodec, VO_CODECBUFFER *pInData, VO_CODECBUFFER *pOutData)
{
	eqfilter* f;
	short *src,*dst;
	int i,ch,bands,y,x,x1,result;
	Equalizer* eqobj;
	int Channels; 
	int ChannelLen;

	if(hCodec == NULL)
		return -1;
	if(pInData->Buffer == NULL || pInData->Length <= 0)
		return -1;
	if(pOutData->Buffer == NULL || pOutData->Length < pInData->Length)
		return -1;
	eqobj = (Equalizer *)hCodec;

	if(eqobj->nEqualizerOn == 0 || eqobj->eq_mode == -1){
		memcpy(pOutData->Buffer, pInData->Buffer, pInData->Length);
		return 0;
	}

	Channels = eqobj->channel;
	ChannelLen = pInData->Length/(sizeof(short)*Channels);
	src = (short *)pInData->Buffer;
	dst = (short *)pOutData->Buffer;
	pOutData->Length = pInData->Length;
	f = eqobj->Filter;

	for(i=0;i<ChannelLen;i++)
	{
		for (ch=0;ch<Channels;ch++)
		{
			x1 = eqobj->XBuf[ch][1];
			eqobj->XBuf[ch][1] = eqobj->XBuf[ch][0];
			x = src[ch];
			eqobj->XBuf[ch][0] = x;
			result = 0;

			for(f=eqobj->Filter,bands=0;bands<MAXFILTER;bands++,f++)
			{
				if(f->alpha0)
				{
					y =	(	fix_mul(f->alpha,(x-x1))
						+	fix_mul(f->gamma,eqobj->YBuf[ch][bands][0])
						-	fix_mul(f->beta,eqobj->YBuf[ch][bands][1]));

					eqobj->YBuf[ch][bands][1] = eqobj->YBuf[ch][bands][0];
					eqobj->YBuf[ch][bands][0] = y;
					result += (fix_mul(y,f->alpha0));
					
				}
			}
			result = fix_mul(eqobj->Amplify,((x)+(result)))>>1;
			dst[ch] = CLIPTOSHORT(result);
		}
		

		dst+=Channels;
		src+=Channels;
	}	

	return VO_ERR_NONE;	
}

VO_S32 voEQUnInit(VO_HANDLE hCodec)
{
	if(hCodec){
		free(hCodec);
		hCodec = 0;
	}
	
	return VO_ERR_NONE;
}

#define DB_LIMIT 0
#if DB_LIMIT

static const DB_LIMITER[MAXFILTER][2]=
{
	{-4,MAX_DB},
	{-4,MAX_DB},
	{-4,MAX_DB},
	{-8,MAX_DB},
	{-8,MAX_DB},
	{-MAX_DB,MAX_DB},
	{-MAX_DB,MAX_DB},
	{-MAX_DB,MAX_DB},
	{-MAX_DB,MAX_DB},
	{-MAX_DB,MAX_DB},
		
};
#endif//DB_LIMIT

static int UpdateEQCOEF(Equalizer* p, int* Eq)
{

	int n;
	const eqfilter *src;
	eqfilter *dst;
	
	src = Bands[p->samplerate];
	dst = p->Filter;
	
#if !DB_LIMIT
	if(p->IsHeadPhone==0)
		p->IsHeadPhone = 1;
#endif//DB_LIMIT		
	for (n=0;n<MAXFILTER;++n,++src,++dst)
	{
		
		if(p->IsHeadPhone)
		{
			if(Eq[n]>MAX_DB)
				Eq[n] =  MAX_DB;
			if(Eq[n]<-MAX_DB)
				Eq[n] = -MAX_DB;
		}
		else
		{
#if DB_LIMIT				
			int* limit = DB_LIMITER[n];
			if(Eq[n]>limit[1])
				Eq[n] =  limit[1];
			if(Eq[n]<limit[0])
				Eq[n] =  limit[0];
#endif//DB_LIMIT
		}	
		
		dst->alpha0 = (Pow(Eq[n])-FIXC(1.));//>>EQZ_IN_FACTOR;//fix_mul(src->alpha,Pow(Eq[n])-FIXC(1.));//(Pow(Eq[n])-FIXC(1.))>>EQZ_IN_FACTOR;//fix_mul(src->alpha,Pow(Eq[n])-FIXC(1.));
		dst->alpha  = (src->alpha);
		dst->beta	= (src->beta);
		dst->gamma	= (src->gamma);
	}

	return VO_ERR_NONE;
}

VO_S32 voEQSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	Equalizer* eqobj;
	int *Eq;
	if(NULL == hCodec || NULL == pData)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	eqobj = (Equalizer *)hCodec;
	
	switch(uParamID)
	{
	case VO_EQ_ONOFF:
		eqobj->nEqualizerOn = *((int*)pData);
		if( *((int*)pData))
		{
			eqobj->eq_mode = 1;
			Eq = EQ_Preset[ *((int*)pData)];
			UpdateEQCOEF(eqobj, Eq);
		}
		break;
	case VO_SAMPLERATE:
		eqobj->samplerate = *((int*)pData);
		UpdateEQCOEF(eqobj, eqobj->nEqCoef);
		break;
	case VO_CHANNELNUM:
		eqobj->channel = *((int*)pData);
		break;
	case VO_EQMODE:
		eqobj->eq_mode = *((int*)pData);
		if( *((int*)pData) > 0)
		{
			Eq = EQ_Preset[*((int*)pData)];
			memcpy(eqobj->nEqCoef, Eq, 10*sizeof(int));
			UpdateEQCOEF(eqobj, Eq);
		}
		break;
	case VO_EA_EQUAT_COEF:
		eqobj->eq_mode = 0;
		memcpy(eqobj->nEqCoef,  pData, 10*sizeof(int));
		UpdateEQCOEF(eqobj, pData);
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	
	return VO_ERR_NONE;
}

VO_S32 voEQGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	Equalizer* eqobj;
	if(NULL == hCodec || NULL == pData)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	eqobj = (Equalizer *)hCodec;
	
	switch(uParamID)
	{
	case VO_EQ_ONOFF:
		*((int*)pData) = eqobj->nEqualizerOn;
		break;
	case VO_SAMPLERATE:
		*((int*)pData) = eqobj->samplerate;
		break;
	case VO_CHANNELNUM:
		*((int*)pData) = eqobj->channel;
		break;
	case VO_EQMODE:
		*((int*)pData) = eqobj->eq_mode;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	
	return VO_ERR_NONE;
}
















