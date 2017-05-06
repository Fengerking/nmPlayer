//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    mapping.c

Abstract:

    channel mapping 0 unpack c file.

Author:

    Witten Wen 10-October-2009

Revision History:

*************************************************************************/


#include "vovorbisdec.h"
#include "mapping.h"
#include "codebook.h"
#include "functionreg.h"
#include "mdct_fft.h"
#include "window.h"
#include "block.h"

static void mapping0_free_info(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfoMapping *i)
{
	VorbisInfoMapping0 *info = (VorbisInfoMapping0 *)i;
	if(info)
	{
		voOGGMemset(info, 0, sizeof(*info));
		voOGGFree(info);
	}
}

static void mapping0_free_look(CVOVorbisObjectDecoder *pvorbisdec, VorbisLookMapping *look)
{
	int i;
	VorbisLookMapping0 *l = (VorbisLookMapping0 *)look;
	if(l)
	{
		for(i=0; i<l->m_pMap->m_Submaps; i++)
		{
			l->m_ppFloorFunc[i]->free_look(pvorbisdec, l->m_ppFloorLook[i]);
			l->m_ppResidueFunc[i]->free_look(pvorbisdec, l->m_ppResidueLook[i]);
		}

		voOGGFree(l->m_ppFloorFunc);
		voOGGFree(l->m_ppResidueFunc);
		voOGGFree(l->m_ppFloorLook);
		voOGGFree(l->m_ppResidueLook);

		/* Witten Added */
		voOGGFree(l->m_ppFloorMemo);
		voOGGFree(l->m_pNonzero);
		voOGGFree(l->m_ppPCMBundle);
		voOGGFree(l->m_pZeroBundle);

		voOGGMemset(l, 0, sizeof(*l));
		voOGGFree(l);
	}
}

/* also responsible for range checking */
static VorbisInfoMapping *mapping0_unpack(CVOVorbisObjectDecoder *pvorbisdec, VorbisInfo *vi)
{
	int i;
	CodecSetupInfo     *ci   = (CodecSetupInfo *)vi->m_pCodecSetup;
	VOOGGInputBitStream *obs = pvorbisdec->m_oggbs;

	VorbisInfoMapping0 *info = (VorbisInfoMapping0 *)voOGGCalloc(1, sizeof(*info));	
	voOGGMemset(info, 0, sizeof(*info));

	if(voOGGDecPackReadBits(obs, 1))
		info->m_Submaps = voOGGDecPackReadBits(obs, 4) + 1;
	else
		info->m_Submaps = 1;

	if (voOGGDecPackReadBits(obs, 1))
	{
		info->m_CouplingSteps = voOGGDecPackReadBits(obs, 8) + 1;

		for (i=0; i<info->m_CouplingSteps; i++)
		{
			int testM = info->m_CouplingMag[i] = voOGGDecPackReadBits(obs, _ILOG(vi->m_Channels));
			int testA = info->m_CouplingAng[i] = voOGGDecPackReadBits(obs, _ILOG(vi->m_Channels)); 

			if(testM<0 || 
				testA<0 || 
				testM==testA || 
				testM>=vi->m_Channels ||
				testA>=vi->m_Channels) goto err_out;
		}

	}

	if (voOGGDecPackReadBits(obs, 2)>0)goto err_out; /* 2,3:reserved */

	if (info->m_Submaps>1)
	{
		for (i=0; i<vi->m_Channels; i++)
		{
			info->m_ChMuxList[i] = voOGGDecPackReadBits(obs, 4);
			if (info->m_ChMuxList[i]>=info->m_Submaps)
				goto err_out;
		}
	}
	for (i=0; i<info->m_Submaps; i++)
	{
		int temp = voOGGDecPackReadBits(obs, 8);
		if (temp>=ci->m_Times)
			goto err_out;
		info->m_FloorSubmap[i] = voOGGDecPackReadBits(obs, 8);
		if (info->m_FloorSubmap[i]>=ci->m_Floors)
			goto err_out;
		info->m_ResidueSubmap[i] = voOGGDecPackReadBits(obs, 8);
		if (info->m_ResidueSubmap[i]>=ci->m_Residues)
			goto err_out;
	}
	return info;

err_out:
	mapping0_free_info(pvorbisdec, info);
	return(NULL);
}

static VorbisLookMapping *mapping0_look(CVOVorbisObjectDecoder *pvorbisdec, 
										VorbisDSPState *vd,
										VorbisInfoMode *vm,
										VorbisInfoMapping *m)
{
	int i;
	VorbisInfo         *vi   = vd->m_pVI;
	CodecSetupInfo     *ci   = (CodecSetupInfo *)vi->m_pCodecSetup;
	VorbisInfoMapping0 *info = (VorbisInfoMapping0 *)m;
	VorbisLookMapping0 *look = (VorbisLookMapping0 *)voOGGCalloc(1, sizeof(*look));
	
	look->m_pMap  = info;
//	look->mode = vm;

	look->m_ppFloorLook   = (VorbisLookFloor **)voOGGCalloc(info->m_Submaps, sizeof(*look->m_ppFloorLook));

	look->m_ppResidueLook = (VorbisLookResidue **)voOGGCalloc(info->m_Submaps, sizeof(*look->m_ppResidueLook));

	look->m_ppFloorFunc   = (VorbisFuncFloor **)voOGGCalloc(info->m_Submaps, sizeof(*look->m_ppFloorFunc));
	look->m_ppResidueFunc = (VorbisFuncResidue **)voOGGCalloc(info->m_Submaps, sizeof(*look->m_ppResidueFunc));

	for (i=0; i<info->m_Submaps; i++)
	{
		int floornum = info->m_FloorSubmap[i];
		int resnum   = info->m_ResidueSubmap[i];

		look->m_ppFloorFunc[i] = Floor_P[ci->m_FloorType[floornum]];
		look->m_ppFloorLook[i] = look->m_ppFloorFunc[i]->
			look(pvorbisdec, vd, vm, ci->m_pFloorParam[floornum]);
		look->m_ppResidueFunc[i] = Residue_P[ci->m_ResidueType[resnum]];
		look->m_ppResidueLook[i] = look->m_ppResidueFunc[i]->
			look(pvorbisdec, vd, vm, ci->m_pResidueParam[resnum]);

	}
	
	look->m_ppPCMBundle	= (OGG_S32 **)voOGGMalloc(sizeof(OGG_S32 **) * vi->m_Channels);
	look->m_pZeroBundle	= (int *)voOGGMalloc(sizeof(int *) * vi->m_Channels);
	look->m_pNonzero		= (int *)voOGGMalloc(sizeof(int *)*vi->m_Channels);
	look->m_ppFloorMemo	= (void **)voOGGMalloc(sizeof(int *)*vi->m_Channels);

//	look->m_Ch = vi->m_Channels;

	return(look);
}

static int mapping0_inverse(CVOVorbisObjectDecoder *pvorbisdec, VorbisBlock *vb, VorbisLookMapping *l)
{
	VorbisDSPState       *vd = vb->vd;
	VorbisInfo           *vi = vd->m_pVI;
	CodecSetupInfo       *ci = (CodecSetupInfo *)vi->m_pCodecSetup;
	PrivateState         *b  = (PrivateState *)vd->m_pBackendState;
	VorbisLookMapping0 *look = (VorbisLookMapping0 *)l;
	VorbisInfoMapping0 *info = look->m_pMap;

	int                   i, j;
	long                  n = vb->m_PCMEnd = ci->m_BlockSize[vb->m_W];

	int   *nonzero   = look->m_pNonzero;



	/* recover the spectral envelope; store it in the PCM vector for now */
	/* 4.decode floor */
	for(i=0; i<vi->m_Channels; i++)
	{
		int submap   = info->m_ChMuxList[i];
		look->m_ppFloorMemo[i] = look->m_ppFloorFunc[submap]->
			inverse1(pvorbisdec, vb, look->m_ppFloorLook[submap]);

		if (look->m_ppFloorMemo[i])
			nonzero[i] = 1;
		else
			nonzero[i] = 0;
		voOGGMemset(vb->m_ppPCM[i], 0, sizeof(*vb->m_ppPCM[i])*n/2);
	}

	/* channel coupling can 'dirty' the nonzero listing */
	for (i=0; i<info->m_CouplingSteps; i++)
	{
		if (nonzero[info->m_CouplingMag[i]] ||
			nonzero[info->m_CouplingAng[i]])
		{
				nonzero[info->m_CouplingMag[i]] = 1; 
				nonzero[info->m_CouplingAng[i]] = 1; 
		}
	}

	/* recover the residue into our working vectors */
	for (i=0; i<info->m_Submaps; i++)
	{
		int ch_in_bundle = 0;
		for(j=0; j<vi->m_Channels; j++)
		{
			if (info->m_ChMuxList[j]==i)
			{
				if (nonzero[j])
					look->m_pZeroBundle[ch_in_bundle] = 1;
				else
					look->m_pZeroBundle[ch_in_bundle] = 0;
				look->m_ppPCMBundle[ch_in_bundle++] = vb->m_ppPCM[j];
			}
		}
		/* 5.decode residue into residue vectors */
		look->m_ppResidueFunc[i]->inverse(pvorbisdec, vb, look->m_ppResidueLook[i],		
			look->m_ppPCMBundle, look->m_pZeroBundle, ch_in_bundle);
	}

	/* 6. inverse channel coupling of residue vectors */
	for (i=info->m_CouplingSteps-1; i>=0; i--)
	{
		OGG_S32 *pcmM = vb->m_ppPCM[info->m_CouplingMag[i]];
		OGG_S32 *pcmA = vb->m_ppPCM[info->m_CouplingAng[i]];

		for(j=0; j<n/2; j++)
		{
			OGG_S32 mag = pcmM[j];
			OGG_S32 ang = pcmA[j];

			if (mag>0)
				if (ang>0)
				{
					pcmM[j] = mag;
					pcmA[j] = mag - ang;
				}else{
					pcmA[j] = mag;
					pcmM[j] = mag + ang;
				}
			else
				if (ang>0)
				{
					pcmM[j] = mag;
					pcmA[j] = mag + ang;
				}else{
					pcmA[j] = mag;
					pcmM[j] = mag-ang;
				}
		}
	}

	/* compute and apply spectral envelope */
	/* 7.generate floor curve from decoded floor data */
	/* 8.compute dot product of floor and residue, producing audio spectrum vector */
	for (i=0; i<vi->m_Channels; i++)
	{
		OGG_S32 *pcm = vb->m_ppPCM[i];
		int submap = info->m_ChMuxList[i];
		look->m_ppFloorFunc[submap]->
			inverse2(pvorbisdec, vb, look->m_ppFloorLook[submap], look->m_ppFloorMemo[i], pcm);	
	}												

	/* transform the PCM data; takes PCM vector, vb; modifies PCM vector */
	/* only MDCT right now.... */
	/* 9.inverse monolithic transform of audio spectrum vector, always an MDCT in Vorbis I */
	for(i=0; i<vi->m_Channels; i++)
	{
		OGG_S32 *pcm = vb->m_ppPCM[i];
#ifdef MDCTIV
		IMDCT(n, pcm, pcm);
#else
		MDCTBackward(n, pcm, pcm);
#endif
	}

	/* window the data */
	for(i=0; i<vi->m_Channels; i++)
	{
		OGG_S32 *pcm = vb->m_ppPCM[i];
		if (nonzero[i])
			VorbisApplyWindow(pcm, b->m_pWindow, ci->m_BlockSize, vb->m_lW, vb->m_W, vb->m_nW);		
		else
			for(j=0; j<n; j++)
				pcm[j] = 0;

	}

	/* overlap/add PCM */
	//////10.overlap/add left-hand output of transform with right-hand output of previous frame

	voOGGDecSynthesisBlockin(&pvorbisdec->vd, &pvorbisdec->vb);

	/* all done! */
	return(0);
}

/* export hooks */
VorbisFuncMapping mapping0_exportbundle = 
{
	&mapping0_unpack,
	&mapping0_look,
	&mapping0_free_info,
	&mapping0_free_look,
	&mapping0_inverse
};
