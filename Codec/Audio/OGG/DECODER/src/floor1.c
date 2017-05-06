//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    floor1.c

Abstract:

    floor backend 1 unpack c file.

Author:

    Witten Wen 14-September-2009

Revision History:

*************************************************************************/

#include <stdlib.h>

#include "macros.h"
#include "vovorbisdec.h"
#include "floor.h"
//#include "block.h"

/***********************************************/
 
static void floor1_free_info(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfoFloor *i)
{
	VorbisInfoFloor1 *info=(VorbisInfoFloor1 *)i;
	if(info)
	{
		voOGGMemset(info,0,sizeof(*info));
		voOGGFree(info);
	}
}

static void floor1_free_look(CVOVorbisObjectDecoder *pvorbisdec, VorbisLookFloor *i)
{
	VorbisLookFloor1 *look=(VorbisLookFloor1 *)i;
	if(look)
	{
		voOGGMemset(look,0,sizeof(*look));
		voOGGFree(look);
	}
}

static VorbisInfoFloor *floor1_unpack (CVOVorbisObjectDecoder *pvorbisdec, 
										 VorbisInfo *vi, 
										 VOOGGInputBitStream *obs)
{
	CodecSetupInfo     *ci = (CodecSetupInfo *)vi->m_pCodecSetup;
	int j, k, count = 0, maxclass = -1, rangebits;

	VorbisInfoFloor1 *info=(VorbisInfoFloor1 *)voOGGCalloc(1, sizeof(VorbisInfoFloor1));
	/* read partitions */
	info->m_Partitions = voOGGDecPackReadBits(obs, 5); /* only 0 to 31 legal */
	for(j=0; j<info->m_Partitions; j++)
	{
		info->m_ParttClass[j] = voOGGDecPackReadBits(obs, 4); /* only 0 to 15 legal */
		if (maxclass<info->m_ParttClass[j])
			maxclass = info->m_ParttClass[j];
	}

	/* read partition classes */
	for(j=0; j<maxclass+1; j++)
	{
		info->m_ClassDim[j] = voOGGDecPackReadBits(obs, 3) + 1; /* 1 to 8 */
		info->m_ClassSubs[j] = voOGGDecPackReadBits(obs, 2); /* 0,1,2,3 bits */
		if (info->m_ClassSubs[j]<0)
			goto err_out;
		if (info->m_ClassSubs[j])
			info->m_ClassBook[j] = voOGGDecPackReadBits(obs, 8);
		if (info->m_ClassBook[j]<0 || info->m_ClassBook[j]>=ci->m_Books)
			goto err_out;
		for (k=0; k<(1<<info->m_ClassSubs[j]); k++)
		{
			info->m_ClassSubbook[j][k] = voOGGDecPackReadBits(obs, 8) - 1;
			if (info->m_ClassSubbook[j][k]<-1 || info->m_ClassSubbook[j][k]>=ci->m_Books)
				goto err_out;
		}
	}

	/* read the post list */
	info->m_Mult = voOGGDecPackReadBits(obs, 2) + 1;     /* only 1,2,3,4 legal now */ 
	rangebits = voOGGDecPackReadBits(obs, 4);

	for(j=0, k=0; j<info->m_Partitions; j++)
	{
		count += info->m_ClassDim[info->m_ParttClass[j]]; 
		for(; k<count; k++)
		{
			int t = info->m_PostList[k+2] = voOGGDecPackReadBits(obs, rangebits);
			if (t<0 || t>=(1<<rangebits))
				goto err_out;
		}
	}
	info->m_PostList[0] = 0;
	info->m_PostList[1] = 1<<rangebits;

	return(info);

err_out:
	floor1_free_info(pvorbisdec, info);
	return(NULL);
}

static int icomp(const void *a,const void *b)
{
	return(**(int **)a-**(int **)b);
}

static VorbisLookFloor *floor1_look(CVOVorbisObjectDecoder *pvorbisdec, 
									  VorbisDSPState *vd, 
									  VorbisInfoMode *mi,
									  VorbisInfoFloor *in)
{
	int *sortpointer[VIF_POSIT+2];
	VorbisInfoFloor1 *info = (VorbisInfoFloor1 *)in;
	int i, j, n = 0;
	VorbisLookFloor1 *look = (VorbisLookFloor1 *)voOGGCalloc(1, sizeof(VorbisLookFloor1));
	

	look->vi = info; 
	look->m_N = info->m_PostList[1];

	/* we drop each position value in-between already decoded values,
	and use linear interpolation to predict each new value past the
	edges.  The positions are read in the order of the position
	list... we precompute the bounding positions in the lookup.  Of
	course, the neighbors can change (if a position is declined), but
	this is an initial mapping */

	for (i=0; i<info->m_Partitions; i++)
		n += info->m_ClassDim[info->m_ParttClass[i]];
	n += 2;
	look->m_Posts = n;

	/* also store a sorted position index */
	for (i=0; i<n; i++)
		sortpointer[i] = info->m_PostList + i;
	qsort(sortpointer, n, sizeof(*sortpointer), icomp);

	/* points from sort order back to range number */
	for (i=0; i<n; i++)
		look->m_ForwardIndex[i] = sortpointer[i] - info->m_PostList;

	/* quantize values to multiplier spec */
	switch(info->m_Mult)
	{
	case 1: /* 1024 -> 256 */
		look->m_QuantQ = 256;
		break;
	case 2: /* 1024 -> 128 */
		look->m_QuantQ = 128;
		break;
	case 3: /* 1024 -> 86 */
		look->m_QuantQ = 86;
		break;
	case 4: /* 1024 -> 64 */
		look->m_QuantQ = 64;
		break;
	}

	/* discover our neighbors for decode where we don't use fit flags
	(that would push the neighbors outward) */
	for(i=0; i<n-2; i++)
	{
		int lo = 0;
		int hi = 1;
		int lx = 0;
		int hx = look->m_N;
		int currentx = info->m_PostList[i+2];
		for(j=0; j<i+2; j++)
		{
			int x = info->m_PostList[j];
			if(x>lx && x<currentx)
			{
				lo = j;
				lx = x;
			}
			if(x<hx && x>currentx)
			{
				hi = j;
				hx = x;
			}
		}
		look->m_LoNeighbor[i] = lo;
		look->m_HiNeighbor[i] = hi;
	}

	return(look);
}

static int RenderPoint(int x0, int x1, int y0, int y1, int x)	//render_point
{
	y0 &= 0x7fff; /* mask off flag */
	y1 &= 0x7fff;

	{
		int dy  = y1 - y0;
		int adx = x1 - x0;
		int ady = abs(dy);
		int err = ady * (x-x0);

		int off = err/adx;
		if (dy<0)return(y0-off);
		return(y0+off);
	}
}

/*
 Refer to SPEC [7.2.2.2.1] for more details
 */
static void floor1_synthesis_amplitude(VorbisLookFloor1 *look,int *fit_value){

	int i;
	VorbisInfoFloor1 *info = look->vi;
	/* unwrap positive values and reconsitute via linear interpolation */
	for(i=2; i<look->m_Posts; i++)
	{
		int neighborIndex = i - 2;
		int predicted = RenderPoint(info->m_PostList[look->m_LoNeighbor[neighborIndex]],
			info->m_PostList[look->m_HiNeighbor[neighborIndex]],
			fit_value[look->m_LoNeighbor[neighborIndex]],
			fit_value[look->m_HiNeighbor[neighborIndex]],
			info->m_PostList[i]);
		int hiroom = look->m_QuantQ - predicted;
		int loroom = predicted;
		int room   = (hiroom<loroom?hiroom:loroom)<<1;
		int val    = fit_value[i];
		
		if(val)
		{
			if (val>=room)
			{
				if(hiroom>loroom)
				{
					val = val - loroom;
				}else{
					val = hiroom - val - 1; //-1-(val-hiroom);
				}
			}else{
				if(val&1)
				{
					val = -((val+1)>>1);
				}else{
					val >>= 1;
				}
			}
			
			fit_value[i] = val + predicted;
			fit_value[look->m_LoNeighbor[neighborIndex]] &= 0xffff;			//clear floor1_step2_flag
			fit_value[look->m_HiNeighbor[neighborIndex]] &= 0xffff;
			
		}else{
			fit_value[i] = predicted | 0x10000;								//set floor1_step2_flag
		}
		
	}

}
/* 
   Refer to SPEC [7.2.2.1] for more details
 */
static void *floor1_inverse1(CVOVorbisObjectDecoder *pvorbisdec, VorbisBlock *vb,VorbisLookFloor *in)
{
	VorbisLookFloor1 *look = (VorbisLookFloor1 *)in;
	VorbisInfoFloor1 *info = look->vi;
	CodecSetupInfo   *ci   = (CodecSetupInfo *)vb->vd->m_pVI->m_pCodecSetup;
	VOOGGInputBitStream *obs = pvorbisdec->m_oggbs;

	int i, j, k;
	CodeBook *books = ci->m_pFullbooks;

	
	if(voOGGDecPackReadBits(obs, 1) == 1)
	{
		/* unpack wrapped/predicted values from stream */
		int *fit_value = (int *)voOGGDecBlockAlloc(pvorbisdec, vb, (look->m_Posts) * sizeof(*fit_value));
		
		j=2;	
		fit_value[0] = voOGGDecPackReadBits(obs, ILOG(look->m_QuantQ-1));
		fit_value[1] = voOGGDecPackReadBits(obs, ILOG(look->m_QuantQ-1));
		
		
		/* partition by partition */
		for(i=0; i<info->m_Partitions; i++)
		{
			int classv = info->m_ParttClass[i];
			int cdim   = info->m_ClassDim[classv];
			int csubbits = info->m_ClassSubs[classv];
			int csub = (1<<csubbits)-1;
			int cval = 0;
			
			/* decode the partition's first stage cascade value */
			if(csubbits)
			{
				cval = voOGGDecBookDecode(books+info->m_ClassBook[classv], obs);				
				if (cval == -1) {
					goto eop;
				}
			}
			
			for(k=0; k<cdim; k++)
			{
				int book = info->m_ClassSubbook[classv][cval&csub];				
				if (book>=0)
				{
					if((fit_value[j+k] = voOGGDecBookDecode(books+book, obs)) == -1){
						goto eop;
					}
				}else{
					fit_value[j+k] = 0;
				}
				cval >>= csubbits;
			}
			j += cdim;
		}
		
		floor1_synthesis_amplitude(look,fit_value);
		
		return(fit_value);
	}
eop:
	return(NULL);
}

#ifdef _LOW_ACCURACY_
#  define XdB(n) ((((n)>>8)+1)>>1)
#else
#  define XdB(n) (n)
#endif

static const OGG_S32 FLOOR_fromdB_LOOKUP[256]={
    XdB(       229),	XdB(       244),	XdB(       259),	XdB(       276),	
    XdB(       294),	XdB(       313),	XdB(       334),	XdB(       355),	
    XdB(       378),	XdB(       403),	XdB(       429),	XdB(       457),	
    XdB(       487),	XdB(       518),	XdB(       552),	XdB(       588),	
    XdB(       626),	XdB(       667),	XdB(       710),	XdB(       756),	
    XdB(       806),	XdB(       858),	XdB(       914),	XdB(       973),	
    XdB(      1036),	XdB(      1104),	XdB(      1175),	XdB(      1252),	
    XdB(      1333),	XdB(      1420),	XdB(      1512),	XdB(      1610),	
    XdB(      1715),	XdB(      1826),	XdB(      1945),	XdB(      2072),	
    XdB(      2206),	XdB(      2350),	XdB(      2502),	XdB(      2665),	
    XdB(      2838),	XdB(      3023),	XdB(      3219),	XdB(      3428),	
    XdB(      3651),	XdB(      3888),	XdB(      4141),	XdB(      4410),	
    XdB(      4696),	XdB(      5002),	XdB(      5327),	XdB(      5673),	
    XdB(      6042),	XdB(      6434),	XdB(      6852),	XdB(      7298),	
    XdB(      7772),	XdB(      8277),	XdB(      8815),	XdB(      9388),	
    XdB(      9998),	XdB(     10647),	XdB(     11339),	XdB(     12076),	
    XdB(     12861),	XdB(     13697),	XdB(     14587),	XdB(     15535),	
    XdB(     16544),	XdB(     17619),	XdB(     18764),	XdB(     19984),	
    XdB(     21283),	XdB(     22666),	XdB(     24139),	XdB(     25707),	
    XdB(     27378),	XdB(     29157),	XdB(     31052),	XdB(     33070),	
    XdB(     35219),	XdB(     37507),	XdB(     39945),	XdB(     42541),	
    XdB(     45305),	XdB(     48249),	XdB(     51385),	XdB(     54724),	
    XdB(     58281),	XdB(     62068),	XdB(     66101),	XdB(     70397),	
    XdB(     74972),	XdB(     79844),	XdB(     85033),	XdB(     90559),	
    XdB(     96444),	XdB(    102711),	XdB(    109386),	XdB(    116494),	
    XdB(    124065),	XdB(    132127),	XdB(    140714),	XdB(    149858),	
    XdB(    159597),	XdB(    169968),	XdB(    181014),	XdB(    192777),	
    XdB(    205305),	XdB(    218646),	XdB(    232855),	XdB(    247988),	
    XdB(    264103),	XdB(    281266),	XdB(    299544),	XdB(    319011),	
    XdB(    339742),	XdB(    361820),	XdB(    385333),	XdB(    410374),	
    XdB(    437043),	XdB(    465444),	XdB(    495691),	XdB(    527904),	
    XdB(    562210),	XdB(    598746),	XdB(    637656),	XdB(    679094),	
    XdB(    723226),	XdB(    770225),	XdB(    820278),	XdB(    873585),	
    XdB(    930355),	XdB(    990815),	XdB(   1055204),	XdB(   1123777),	
    XdB(   1196806),	XdB(   1274581),	XdB(   1357411),	XdB(   1445623),	
    XdB(   1539568),	XdB(   1639617),	XdB(   1746169),	XdB(   1859645),	
    XdB(   1980495),	XdB(   2109199),	XdB(   2246266),	XdB(   2392241),	
    XdB(   2547703),	XdB(   2713267),	XdB(   2889590),	XdB(   3077372),	
    XdB(   3277357),	XdB(   3490338),	XdB(   3717160),	XdB(   3958722),	
    XdB(   4215982),	XdB(   4489960),	XdB(   4781743),	XdB(   5092488),	
    XdB(   5423426),	XdB(   5775871),	XdB(   6151219),	XdB(   6550960),	
    XdB(   6976679),	XdB(   7430063),	XdB(   7912910),	XdB(   8427135),	
    XdB(   8974778),	XdB(   9558009),	XdB(  10179143),	XdB(  10840641),	
    XdB(  11545127),	XdB(  12295394),	XdB(  13094418),	XdB(  13945367),	
    XdB(  14851616),	XdB(  15816757),	XdB(  16844619),	XdB(  17939278),	
    XdB(  19105073),	XdB(  20346628),	XdB(  21668866),	XdB(  23077031),	
    XdB(  24576707),	XdB(  26173840),	XdB(  27874763),	XdB(  29686223),	
    XdB(  31615400),	XdB(  33669947),	XdB(  35858010),	XdB(  38188266),	
    XdB(  40669954),	XdB(  43312918),	XdB(  46127635),	XdB(  49125267),	
    XdB(  52317705),	XdB(  55717604),	XdB(  59338448),	XdB(  63194594),	
    XdB(  67301334),	XdB(  71674955),	XdB(  76332796),	XdB(  81293331),	
    XdB(  86576231),	XdB(  92202442),	XdB(  98194276),	XdB( 104575492),	
    XdB( 111371397),	XdB( 118608939),	XdB( 126316814),	XdB( 134525593),	
    XdB( 143267823),	XdB( 152578174),	XdB( 162493564),	XdB( 173053310),	
    XdB( 184299289),	XdB( 196276095),	XdB( 209031220),	XdB( 222615242),	
    XdB( 237082044),	XdB( 252488973),	XdB( 268897122),	XdB( 286371562),	
    XdB( 304981612),	XdB( 324801039),	XdB( 345908441),	XdB( 368387505),	
    XdB( 392327395),	XdB( 417823051),	XdB( 444975534),	XdB( 473892561),	
    XdB( 504688765),	XdB( 537486295),	XdB( 572415181),	XdB( 609613936),	
    XdB( 649230091),	XdB( 691420715),	XdB( 736353116),	XdB( 784205494),	
    XdB( 835167579),	XdB( 889441492),	XdB( 947242410),	XdB(1008799556),	
    XdB(1074357035),	XdB(1144174799),	XdB(1218529724),	XdB(1297716638),	
    XdB(1382049587),	XdB(1471862937),	XdB(1567512890),	XdB(1669378712),	
    XdB(1777864346),	XdB(1893399976),	XdB(2016443766),	XdB(2147483647),
};
  
static void RenderLine(int n, int x0,int x1,int y0,int y1,OGG_S32 *d)	//render_line
{
	int dy   = y1 - y0;
	int adx  = x1 - x0;
	int ady  = abs(dy);
	int base = dy / adx;
	int sy   = (dy<0 ? base-1 : base+1);
	int x    = x0;
	int y    = y0;
	int err  = 0;
	
	if (n>x1)n = x1;
	ady -= abs(base * adx);
	
	if (x<n)
		d[x] = MULT31_SHIFT15(d[x], FLOOR_fromdB_LOOKUP[y]);
	
	while (++x<n)
	{
		err = err + ady;
		if (err>=adx)
		{
			err -= adx;
			y   += sy;
		}else{
			y   += base;
		}
		d[x] = MULT31_SHIFT15(d[x], FLOOR_fromdB_LOOKUP[y]);
	}
}

static int floor1_inverse2(CVOVorbisObjectDecoder *pvorbisdec, 
						   VorbisBlock *vb,VorbisLookFloor *in,void *memo,
			  OGG_S32 *out)
{
	VorbisLookFloor1 *look = (VorbisLookFloor1 *)in;
	VorbisInfoFloor1 *info = look->vi;
	
	CodecSetupInfo   *ci   = (CodecSetupInfo *)vb->vd->m_pVI->m_pCodecSetup;
	int                n   = ci->m_BlockSize[vb->m_W]/2;
	int j;
	
	if (memo)
	{
		/* render the lines */
		int *fit_value = (int *)memo;
		int hx = 0;
		int lx = 0;
		int ly = fit_value[0] * info->m_Mult;
		// 14548 root cause: the last encoder frame is not enough, whose information has pack into packet header,but our file parser skip the 22 bytes.
		//if (ly == 204)  //temp resloved for 14548
		//{
		//	ly = 50;
		//}
		for(j=1; j<look->m_Posts; j++)
		{
			int current = look->m_ForwardIndex[j];
			int hy = fit_value[current] & 0xffff;     
			if (hy == fit_value[current])
			{				
				hy *= info->m_Mult;
				hx  = info->m_PostList[current];
				
				RenderLine(n, lx, hx, ly, hy, out);
				
				lx = hx;
				ly = hy;
			}
		}
		for(j=hx; j<n; j++)out[j] *= ly; /* be certain */    
		return(1);
	}
	voOGGMemset(out, 0, sizeof(*out) * n);
	return(0);
}

/* export hooks */
VorbisFuncFloor floor1_exportbundle =
{
	&floor1_unpack,
	&floor1_look,
	&floor1_free_info,
	&floor1_free_look,
	&floor1_inverse1,
	&floor1_inverse2
};



