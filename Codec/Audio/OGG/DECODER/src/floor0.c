//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    floor0.c

Abstract:

    floor backend 0 unpack c file.

Author:

    Witten Wen 14-September-2009

Revision History:

*************************************************************************/

#include "macros.h"
#include "vovorbisdec.h"
#include "floor.h"
#include "ogglookup.h"
//#include "block.h"

#define LSP_FRACBITS 14

/*************** vorbis decode glue ************/

static void floor0_free_info(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfoFloor *i)
{
	VorbisInfoFloor0 *info = (VorbisInfoFloor0 *)i;
	if (info)
	{
		voOGGMemset(info, 0, sizeof(VorbisInfoFloor0));
		voOGGFree(info);
	}
}

static VorbisInfoFloor *floor0_unpack (CVOVorbisObjectDecoder *pvorbisdec, VorbisInfo *vi, VOOGGInputBitStream *obs)
{
    CodecSetupInfo     *ci = (CodecSetupInfo *)vi->m_pCodecSetup;
    int j;

    VorbisInfoFloor0 *info = (VorbisInfoFloor0 *)voOGGMalloc(sizeof(VorbisInfoFloor0));
    info->m_Order = voOGGDecPackReadBits(obs, 8);
    info->m_Rate = voOGGDecPackReadBits(obs, 16);
    info->m_BarkMap = voOGGDecPackReadBits(obs, 16);
	  info->m_AmpBits = voOGGDecPackReadBits(obs, 6);
    info->m_AmpDB = voOGGDecPackReadBits(obs, 8);
    info->m_NumBooks = voOGGDecPackReadBits(obs, 4) + 1;
  
	if (info->m_Order < 1)
		goto err_out;
	if (info->m_Rate < 1)
		goto err_out;
	if (info->m_BarkMap < 1)
		goto err_out;
	if (info->m_NumBooks < 1)
		goto err_out;
    
	for (j=0; j<info->m_NumBooks; j++)
	{
		info->m_Books[j] = voOGGDecPackReadBits(obs, 8);
		if (info->m_Books[j]<0 || info->m_Books[j]>=ci->m_Books)
			goto err_out;
	}
	return(info);

err_out:
	floor0_free_info(pvorbisdec, info);
	return(NULL);
}

/* interpolated 1./sqrt(p) where .5 <= a < 1. (.100000... to .111111...) in
   16.16 format 
   steps: 1. loop up table
          2. interpolate
          3. adjust value
   returns in m.8 format */

STIN OGG_S32 VorbisInvsqlookI(long a, long e)	//vorbis_invsqlook_i
{
	long i=(a&0x7fff)/(1<<(INVSQ_LOOKUP_I_SHIFT-1)); 
	long d=a&INVSQ_LOOKUP_I_MASK;																			 /*  0.10 */
	long kd = ((INVSQ_LOOKUP_IDel[i]*d)>>INVSQ_LOOKUP_I_SHIFT);        /* result 1.16 */
	long val=INVSQ_LOOKUP_I[i] - kd;																		 /*  1.16 */
	if(e&1){
		val*=5792;
		e=(e+41)>>1;
	}else{
		val*=8192;
		e=(e+42)>>1;
	}

	return(val>>e);

}

/* interpolated lookup based fromdB function, domain is from -140dB to 0dB only */
/* a is in n.12 format */
STIN OGG_S32 VorbisFromdBlookI(long a)	//vorbis_fromdBlook_i
{
	int i = (-a)/(1<<(12-FROMdB2_SHIFT));
	if(i>=0&&i<(FROMdB_LOOKUP_SZ<<FROMdB_SHIFT)){
		int e = i>>FROMdB_SHIFT;
		int m = i&FROMdB2_MASK;
		return FROMdB_LOOKUP[e] * FROMdB2_LOOKUP[m];
	}
	else if (i<0) {
		return 0x7fffffff;
	}
	else{
		return 0;
	}

}

/* domain is from 0 to PI only , interpolated lookup based cos function*/
/* a is in 0.16 format, where 0==0, 2^^16-1==PI, return 0.14 */
STIN OGG_S32 VorbisCoslookI(long a)	//vorbis_coslook_i
{
	int i = a>>COS_LOOKUP_I_SHIFT;
	int d = a - (i<<COS_LOOKUP_I_SHIFT);
	if(i<COS_LOOKUP_I_SZ){
		OGG_S32 x = COS_LOOKUP_I[i] - COS_LOOKUP_I[i+1];
		OGG_S32 kx = ((d*x)>>COS_LOOKUP_I_SHIFT);
		return COS_LOOKUP_I[i] - kx;
	}else{
		return COS_LOOKUP_I[COS_LOOKUP_I_SZ] >> COS_LOOKUP_I_SHIFT;
	}
 
}

/* domain is from 0 to 2PI,interpolated lookup based cos function */
/* a is in 0.16 format, where 0==0, 2^^16==PI, return .LSP_FRACBITS */
/*in current context, some statements have no use, so disable them*/
STIN OGG_S32 VorbisCoslook2I(long a)		//vorbis_coslook2_i
{
	
	//a = a&0x1ffff;

	//if (a > 0x10000)
	//	a = 0x20000 - a;

	{               
		int i = a >> COS_LOOKUP_I_SHIFT;
		int d = a - (i<<COS_LOOKUP_I_SHIFT);
		if(i<COS_LOOKUP_I_SZ){
			OGG_S32 x = COS_LOOKUP_I[i] - COS_LOOKUP_I[i+1];
			OGG_S32 kx = d*x;
			OGG_S32 y = ((COS_LOOKUP_I[i] << COS_LOOKUP_I_SHIFT)- kx);
			a = y >> (COS_LOOKUP_I_SHIFT-LSP_FRACBITS+14);
		}else{
			a = COS_LOOKUP_I[COS_LOOKUP_I_SZ] >> (COS_LOOKUP_I_SHIFT-LSP_FRACBITS+14);

		}
	}
	return(a);
}

static const int barklook[29] = 
{
	//barklook[28] is the extra data to avoid different handle for last element
 0x0 ,	 0x64 ,	 0xc8 ,	 0x12d ,	
 0x195 ,	 0x204 ,	 0x27b ,	 0x2fe ,	
 0x390 ,	 0x435 ,	 0x4ef ,	 0x5c4 ,	
 0x6b8 ,	 0x7d3 ,	 0x91d ,	 0xaa1 ,	
 0xc70 ,	 0xe9e ,	 0x114c ,	 0x14a5 ,	
 0x18e8 ,	 0x1e6f ,	 0x25be ,	 0x2f95 ,	
 0x3d08 ,	 0x4fad ,	 0x69cf ,	 0x8eca ,	
 0x8eca ,	
};

/* used in init only; interpolate the long way */
STIN OGG_S32 toBARK(int n)
{

	int start = 0, end = 27, mid;
	//binary search bark band index and result is in mid
	while(start<=end){
		mid = (start+end)/2;
		if (n>=barklook[mid] && n<barklook[mid+1]){
			break;
		}else if(n<barklook[mid]) {
			end = mid - 1;
		}else{
			start = mid + 1;
		}
	}	

	{
		int gap = barklook[mid+1] - barklook[mid];
		int del = n - barklook[mid];
		return((mid<<15) + ((del<<15)/gap));
	}
		

}

/* initialize Bark scale and normalization lookups.  We could do this
   with static tables, but Vorbis allows a number of possible
   combinations, so it's best to do it computationally.

   The below is authoritative in terms of defining scale mapping.
   Note that the scale depends on the sampling rate as well as the
   linear block and mapping sizes */

static VorbisLookFloor *floor0_look ( CVOVorbisObjectDecoder *pvorbisdec, 
										VorbisDSPState *vd, 
										VorbisInfoMode *mi,
										VorbisInfoFloor *i )
{
	int j;
	VorbisInfo        *vi	= vd->m_pVI;
	CodecSetupInfo   *ci	= (CodecSetupInfo *)vi->m_pCodecSetup;
	VorbisInfoFloor0 *info	= (VorbisInfoFloor0 *)i;
	VorbisLookFloor0 *look	= (VorbisLookFloor0 *)voOGGCalloc(1,sizeof(VorbisLookFloor0));
	look->m_M		= info->m_Order;
	look->m_N		= ci->m_BlockSize[mi->m_BlockFlag] / 2;
	look->m_Ln	= info->m_BarkMap;
	look->vi	= info;
	look->m_pIlsp	= (OGG_S32 *)voOGGCalloc(look->m_M, sizeof(OGG_S32));
	/* the mapping from a linear scale to a smaller bark scale is
		straightforward.  We do *not* make sure that the linear mapping
		does not skip bark-scale bins; the decoder simply skips them and
		the encoder may do what it wishes in filling them.  They're
		necessary in some mapping combinations to keep the scale spacing
		accurate */
	look->m_pLinearMap	= (int *)voOGGMalloc((look->m_N+1) * sizeof(*look->m_pLinearMap));
	for(j=0; j<look->m_N; j++)
	{
		int val = (look->m_Ln *
			((toBARK(info->m_Rate/2*j/look->m_N)<<11)/toBARK(info->m_Rate/2)))>>11;

		if (val>=look->m_Ln)
			val = look->m_Ln - 1; /* guard against the approximation */
		look->m_pLinearMap[j] = val;
	}
	look->m_pLinearMap[j] = -1;

	look->m_pLSPLook = (OGG_S32 *)voOGGMalloc(look->m_Ln * sizeof(*look->m_pLSPLook));
	for(j=0; j<look->m_Ln; j++)
		look->m_pLSPLook[j] = VorbisCoslook2I(0x10000 * j / look->m_Ln);

	return look;
}

static void floor0_free_look(CVOVorbisObjectDecoder *pvorbisdec, VorbisLookFloor *i)
{
	VorbisLookFloor0 *look = (VorbisLookFloor0 *)i;
	if (look)
	{

		if (look->m_pLinearMap)
			voOGGFree(look->m_pLinearMap);
		if (look->m_pLSPLook)
			voOGGFree(look->m_pLSPLook);
//		voOGGMemset(look, 0, sizeof(*look));
		if (look->m_pIlsp)
			voOGGFree(look->m_pIlsp);
		voOGGFree(look);
	}
}

static void *floor0_inverse1(CVOVorbisObjectDecoder *pvorbisdec, VorbisBlock *vb,VorbisLookFloor *i)
{
	VorbisLookFloor0 *look = (VorbisLookFloor0 *)i;
	VorbisInfoFloor0 *info = look->vi;
	VOOGGInputBitStream *obs = pvorbisdec->m_oggbs;

	int j, k;
  
	int ampraw = voOGGDecPackReadBits(obs, info->m_AmpBits);
	if (ampraw>0)
	{ /* also handles the -1 out of data case */
		long maxval = (1<<info->m_AmpBits) - 1;
		int amp = ((ampraw * info->m_AmpDB)<<4) / maxval;
		int booknum = voOGGDecPackReadBits(obs, ILOG(info->m_NumBooks));
    
		if (booknum!=-1 && booknum<info->m_NumBooks)
		{ /* be paranoid */
			CodecSetupInfo  *ci = (CodecSetupInfo *)vb->vd->m_pVI->m_pCodecSetup;
			CodeBook *b = ci->m_pFullbooks + info->m_Books[booknum];
			OGG_S32 last = 0;
			OGG_S32 *lsp = (OGG_S32 *)voOGGDecBlockAlloc(pvorbisdec, vb,sizeof(*lsp)*(look->m_M+1));
            
			for (j=0; j<look->m_M; j+=b->m_Dim)
				if(voOGGDecBookDecodevSet(b, lsp+j, obs, b->m_Dim, -24)==-1)
					goto eop;
			for (j=0; j<look->m_M;)
			{
				for(k=0; k<b->m_Dim; k++, j++)
					lsp[j] += last;
				last = lsp[j-1];
			}
      
			lsp[look->m_M] = amp;
			return(lsp);
		}
	}
eop:
	return(NULL);
}

static const unsigned char MLOOP_1[64]={
   0,10,11,11, 12,12,12,12, 13,13,13,13, 13,13,13,13,
  14,14,14,14, 14,14,14,14, 14,14,14,14, 14,14,14,14,
  15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
  15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
};

static const unsigned char MLOOP_2[64]={
  0,4,5,5, 6,6,6,6, 7,7,7,7, 7,7,7,7,
  8,8,8,8, 8,8,8,8, 8,8,8,8, 8,8,8,8,
  9,9,9,9, 9,9,9,9, 9,9,9,9, 9,9,9,9,
  9,9,9,9, 9,9,9,9, 9,9,9,9, 9,9,9,9,
};

static const unsigned char MLOOP_3[8]={0,1,2,2,3,3,3,3};


STIN int calShiftBits(OGG_U32 x){
	OGG_S32 shift;
	if (!(shift = MLOOP_1[x>>25]))
			if (!(shift = MLOOP_2[x>>19]))
				shift = MLOOP_3[x>>16];
	return shift;

}

//void VorbisLsp2Curve(	CVOVorbisObjectDecoder *pvorbisdec, 
//						OGG_S32 *curve, int *map,int n,int ln,
//						OGG_S32 *lsp, int m,
//						OGG_S32 amp,
//						OGG_S32 ampoffset,
//						OGG_S32 *icos )
void VorbisLsp2Curve(	OGG_S32 *curve, VorbisLookFloor0 *look,
						OGG_S32 *lsp,
						OGG_S32 ampoffset)
{
	int *map	= look->m_pLinearMap;
	int n		= look->m_N;//n;
	int m		= look->m_M;
	OGG_S32 amp	= lsp[m];
	OGG_S32 *icos	= look->m_pLSPLook;
	/* m is order of filter, 0 <= m < 256 */

	/* set up for using all int later */
	int i;
	int ampoffseti = ampoffset<<12;
	int ampi = amp;
	OGG_S32 *ilsp = look->m_pIlsp;	
	/* lsp is in 8.24, range 0 to PI; coslook wants it in .16 , range from 0 to 1*/
	for (i=0; i<m; i++)
	{
#ifdef _LOW_ACCURACY_
		OGG_S32 val = ((lsp[i]>>10)*0x517d)>>14;
#else
		OGG_S32 val = MULT32(lsp[i], 0x517cc2);		
#endif

		/* safeguard against a malicious stream */
		if(val<0 || (val>>COS_LOOKUP_I_SHIFT)>=COS_LOOKUP_I_SZ)
		{
			voOGGMemset(curve, 0, sizeof(*curve)*n);
			return;
		}

		ilsp[i] = VorbisCoslookI(val);
	}

	i = 0; 
	while (i<n)
	{
		int j, k = map[i];
		OGG_U32 pi = 46341; /* 2**-.5 in 0.16 */
		OGG_U32 qi = 46341;
		OGG_S32 qexp = 0, shift=0;
		OGG_S32 wi = icos[k];

#ifdef _V_LSP_MATH_ASM
		//temporarily delete unimplemented code, maybe risky

#else
		for (j=1; j<m; j+=2)
		{
			
			qi = (qi>>shift) * labs(ilsp[j-1] - wi);
			pi = (pi>>shift) * labs(ilsp[j] - wi);
			qexp += shift;

			shift = calShiftBits(pi|qi);
		}

		pi >>= shift;
		qi >>= shift;
		qexp += shift;
	

		/* pi,qi normalized collectively, both tracked using qexp */
		if (m%2==0)
		{
			/* even order filter: still symmetric */

			/* p*=p(1-w), q*=q(1+w), let normalization drift because it isn't
			worth tracking step by step */	
			qexp -= 7*m;

			pi = ((pi*pi)>>16);
			qi = ((qi*qi)>>16);
			qexp = qexp*2 + m;

			pi *= (1<<14) - wi;
			qi *= (1<<14) + wi;
			qi = (qi+pi)>>14;

		}else{
			/* odd order filter: slightly assymetric */
			/* the last coefficient */
			qi = qi* labs(ilsp[j-1]-wi);
			pi = pi<<14;
			shift = calShiftBits(pi|qi);

			pi >>= shift;
			qi >>= shift;
			qexp += shift - 14*((m>>1)+1);

			pi = ((pi*pi)>>16);
			qi = ((qi*qi)>>16);
			qexp = qexp*2+m;

			pi *= (1<<14) - ((wi*wi)>>14);
			qi += pi>>14;
		}

		/* we've let the normalization drift because it wasn't important;
		however, for the lookup, things must be normalized again. */

		if (qi&0xffff0000)
		{	/* checks for 1.xxxxxxxxxxxxxxxx */
			do{
				qi >>= 1; qexp++; 
			}while(0);			//only change code format
			
		}else{
			while (qi && !(qi&0x8000))
			{ /* checks for 0.0xxxxxxxxxxxxxxx or less*/
				qi<<=1; qexp--; 
			}
		}

#endif
		
		amp = VorbisFromdBlookI(ampi*						/*  n.4         */
			VorbisInvsqlookI(qi, qexp) -                    /*  m.8, m+n>=8 */		                                                
			ampoffseti);                                    /*  8.12[0]     */

#ifdef _LOW_ACCURACY_
		amp>>=9;
#endif
		
		do {
			curve[i] = MULT31_SHIFT15(curve[i], amp);
		}while(map[++i]==k);
	}
}

static int floor0_inverse2(CVOVorbisObjectDecoder *pvorbisdec, 
						   VorbisBlock *vb,
						   VorbisLookFloor *i,
						   void *memo,
						   OGG_S32 *out)
{
	VorbisLookFloor0 *look = (VorbisLookFloor0 *)i;
	VorbisInfoFloor0 *info = look->vi;

	if (memo)
	{
		OGG_S32 *lsp = (OGG_S32 *)memo;
//		OGG_S32 amp = lsp[look->m];

		/* take the coefficients back to a spectral envelope curve */
//		VorbisLsp2Curve(pvorbisdec, out,look->linearmap,look->n,look->ln,
//			lsp,look->m,amp,info->ampdB,look->lsp_look);
		VorbisLsp2Curve(out, look, lsp, info->m_AmpDB);
		return(1);
	}
	voOGGMemset(out,0,sizeof(*out)*look->m_N);
	return(0);
}

/* export hooks */
VorbisFuncFloor floor0_exportbundle = 
{
    &floor0_unpack,
    &floor0_look,
	&floor0_free_info,
    &floor0_free_look,
	&floor0_inverse1,
	&floor0_inverse2
};

