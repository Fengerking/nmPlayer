//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    block.c

Abstract:

    block functions.

Author:

    Witten Wen 14-September-2009

Revision History:

*************************************************************************/

#include "vovorbisdec.h"
#include "window.h"
#include "functionreg.h"

#ifndef WORD_ALIGN
#define WORD_ALIGN 8
#endif

void *voOGGDecBlockAlloc(CVOVorbisObjectDecoder *pvorbisdec, VorbisBlock *vb, long bytes)	//_vorbis_block_alloc
{
	bytes = (bytes + (WORD_ALIGN-1)) & ~(WORD_ALIGN-1);
	if (bytes + vb->m_LocalTop > vb->m_LocalAlloc)
	{
		/* can't just _ogg_realloc... there are outstanding pointers */
		if (vb->m_pLocalStore)
		{
			struct AllocChain *link = (struct AllocChain *)voOGGMalloc(sizeof(*link));
			vb->m_TotleUse += vb->m_LocalTop;
			link->next = vb->m_pReap;
			link->ptr = vb->m_pLocalStore;
			vb->m_pReap = link;
		}
		/* highly conservative */
		vb->m_LocalAlloc = bytes;
		vb->m_pLocalStore = voOGGMalloc(vb->m_LocalAlloc);
		vb->m_LocalTop = 0;
	}
	{
		void *ret = (void *)(((char *)vb->m_pLocalStore) + vb->m_LocalTop);
		vb->m_LocalTop += bytes;
		return ret;
	}
}

static int VDSInit(CVOVorbisObjectDecoder *pvorbisdec, VorbisDSPState *v, VorbisInfo *vi)	//_vds_init
{
	int i;
	CodecSetupInfo *ci	= (CodecSetupInfo *)vi->m_pCodecSetup;
	PrivateState *b	= NULL;

	voOGGMemset(v, 0, sizeof(*v));
	b = (PrivateState *)(v->m_pBackendState = voOGGCalloc(1, sizeof(*b)));

	v->m_pVI = vi;
	b->m_Modebits = _ILOG(ci->m_Modes);

	/* Vorbis I uses only window type 0 */
	b->m_pWindow[0] = VorbisWindow(0, ci->m_BlockSize[0]/2);
	b->m_pWindow[1] = VorbisWindow(0, ci->m_BlockSize[1]/2);

	/* finish the codebooks */
	if (!ci->m_pFullbooks)
	{
		ci->m_pFullbooks = (CodeBook *)voOGGCalloc(ci->m_Books, sizeof(*ci->m_pFullbooks));

		for(i=0; i<ci->m_Books; i++)
		{
			VorbisBookDecodeInit(pvorbisdec, ci->m_pFullbooks+i, ci->m_pBookParam[i]);//decode codebook
			/* decode codebooks are now standalone after init */
			voOGGDecFreeStaticbook(pvorbisdec, ci->m_pBookParam[i]);	//delete primary static book
			ci->m_pBookParam[i] = NULL;
		}
	}

	v->m_PCMStorage = ci->m_BlockSize[1];
	v->m_ppPCM = (OGG_S32 **)voOGGMalloc(vi->m_Channels*sizeof(*v->m_ppPCM));
	v->m_ppPCMRet = (OGG_S32 **)voOGGMalloc(vi->m_Channels*sizeof(*v->m_ppPCMRet));
	for(i=0; i<vi->m_Channels; i++)
		v->m_ppPCM[i] = (OGG_S32 *)voOGGCalloc(v->m_PCMStorage, sizeof(*v->m_ppPCM[i]));

	/* all 1 (large block) or 0 (small block) */
	/* explicitly set for the sake of clarity */
	v->m_lW = 0; /* previous window size */
	v->m_W = 0;  /* current window size */

	/* initialize all the mapping/backend lookups */
	b->m_ppMode = (VorbisLookMapping **)voOGGCalloc(ci->m_Modes, sizeof(*b->m_ppMode));
	for (i=0; i<ci->m_Modes; i++)
	{
		int mapnum  = ci->m_pModeParam[i]->m_Mapping;
		int maptype = ci->m_MapType[mapnum];
		b->m_ppMode[i] = Mapping_P[maptype]->look(pvorbisdec, v, ci->m_pModeParam[i], 
			ci->m_pMapParam[mapnum]);
	}
	return(0);
}

int VorbisSynthesisRestart(VorbisDSPState *v)	//vorbis_synthesis_restart
{
	VorbisInfo *vi = v->m_pVI;
	CodecSetupInfo *ci;

	if (!v->m_pBackendState)
		return -1;
	if (!vi)
		return -1;
	ci = vi->m_pCodecSetup;
	
	if (!ci)
		return -1;

	v->m_CenterW = ci->m_BlockSize[1] / 2;
	v->m_PCMCurrent = v->m_CenterW;

	v->m_PCMReturned = -1;
	v->m_GranulePos = -1;
	v->m_Sequence = -1;
	((PrivateState *)(v->m_pBackendState))->m_SampleCount = -1;

	return(0);
}

int voOGGDecSynthesisInit(CVOVorbisObjectDecoder *pvorbisdec, VorbisDSPState *v, VorbisInfo *vi)//vorbis_synthesis_init
{
	VDSInit(pvorbisdec, v, vi);
	VorbisSynthesisRestart(v);

	return(0);
}

int voOGGDecBlockInit(VorbisDSPState *v, VorbisBlock *vb)
{
	voOGGMemset(vb, 0, sizeof(*vb));
	vb->vd = v;
	vb->m_LocalAlloc = 0;
	vb->m_pLocalStore = NULL;

	return(0);
}

/* pcm==NULL indicates we just want the pending samples, no more */
int voOGGDecSynthesisPcmout(VorbisDSPState *v, OGG_S32 ***pcm)	//vorbis_synthesis_pcmout
{
	VorbisInfo *vi = v->m_pVI;
	if (v->m_PCMReturned>-1 && v->m_PCMReturned<v->m_PCMCurrent)
	{
		if (pcm)
		{
			int i;
			for(i=0; i<vi->m_Channels; i++)
				v->m_ppPCMRet[i] = v->m_ppPCM[i] + v->m_PCMReturned;
			*pcm = v->m_ppPCMRet;
		}
		return(v->m_PCMCurrent - v->m_PCMReturned);
	}
	return(0);
}

/* reap the chain, pull the ripcord */
void voOGGDecBlockRipcord(CVOVorbisObjectDecoder *pvorbisdec, VorbisBlock *vb)
{
	/* reap the chain */
	struct AllocChain *reap = vb->m_pReap;
	while(reap)
	{
		struct AllocChain *next = reap->next;
		voOGGFree(reap->ptr);
		voOGGMemset(reap, 0, sizeof(*reap));
		voOGGFree(reap);
		reap = next;
	}
	/* consolidate storage */
	if (vb->m_TotleUse)
	{
		vb->m_pLocalStore = voOGGRealloc(vb->m_pLocalStore, vb->m_TotleUse+vb->m_LocalAlloc);
		vb->m_LocalAlloc += vb->m_TotleUse;
		vb->m_TotleUse   = 0;
	}

	/* pull the ripcord */
	vb->m_LocalTop = 0;
	vb->m_pReap = NULL;
}

/* Unlike in analysis, the window is only partially applied for each
   block.  The time domain envelope is not yet handled at the point of
   calling (as it relies on the previous block). */

int voOGGDecSynthesisBlockin(VorbisDSPState *v, VorbisBlock *vb)//vorbis_synthesis_blockin
{
	VorbisInfo *vi = v->m_pVI;
	CodecSetupInfo *ci = (CodecSetupInfo *)vi->m_pCodecSetup;
	PrivateState *b = v->m_pBackendState;
	int i, j;

	if (v->m_PCMCurrent>v->m_PCMReturned  && v->m_PCMReturned!=-1)return(OV_EINVAL);

	v->m_lW = v->m_W;
	v->m_W  = vb->m_W;
	v->m_nW = -1;

	if ((v->m_Sequence==-1) || (v->m_Sequence+1 != vb->m_Sequence))
	{
			v->m_GranulePos = -1; /* out of sequence; lose count */
			b->m_SampleCount = -1;
	}

	v->m_Sequence = vb->m_Sequence;

	if (vb->m_ppPCM)
	{  /* no pcm to process if vorbis_synthesis_trackonly 
				  was called on block */
		int n  = ci->m_BlockSize[v->m_W]/2;
		int n0 = ci->m_BlockSize[0]/2;
		int n1 = ci->m_BlockSize[1]/2;

		int thisCenter;
		int prevCenter;

		if (v->m_CenterW)
		{
			thisCenter = n1;
			prevCenter = 0;
		}else{
			thisCenter = 0;
			prevCenter = n1;
		}

		/* v->pcm is now used like a two-stage double buffer.  We don't want
		to have to constantly shift *or* adjust memory usage.  Don't
		accept a new block until the old is shifted out */

		/* overlap/add PCM */
		//////10.overlap/add left-hand output of transform with right-hand output of previous frame
		for (j=0; j<vi->m_Channels; j++)
		{		
			/* the overlap/add section */
			if (v->m_lW)
			{
				if (v->m_W)
				{
					/* large/large */
					OGG_S32 *pcm = v->m_ppPCM[j] + prevCenter;
					OGG_S32 *p = vb->m_ppPCM[j];
					for(i=0; i<n1; i++)
						pcm[i] += p[i];
				}else{
					/* large/small */
					OGG_S32 *pcm = v->m_ppPCM[j] + prevCenter + n1/2 - n0/2;
					OGG_S32 *p = vb->m_ppPCM[j];
					for(i=0; i<n0; i++)
						pcm[i] += p[i];
				}
			}else{
				if (v->m_W)
				{
					/* small/large */
					OGG_S32 *pcm = v->m_ppPCM[j] + prevCenter;
					OGG_S32 *p = vb->m_ppPCM[j] + n1/2 - n0/2;
					for(i=0; i<n0; i++)
						pcm[i] += p[i];
					for(; i<n1/2+n0/2; i+=2){
						pcm[i] = p[i];
						pcm[i+1] = p[i+1];
					}
				}else{
					/* small/small */
					OGG_S32 *pcm = v->m_ppPCM[j] + prevCenter;
					OGG_S32 *p = vb->m_ppPCM[j];
					for(i=0; i<n0; i+=2){
						pcm[i] += p[i];
						pcm[i+1] += p[i+1];
					}
				}
			}

			/* the copy section */
			{
				OGG_S32 *pcm = v->m_ppPCM[j] + thisCenter;
				OGG_S32 *p = vb->m_ppPCM[j] + n;
				if(pcm!=p){
					for(i=0; i<n; i++)
						pcm[i] = p[i];
				}
				
			}
		}

		/*equivalent to following snippet
		if (v->m_CenterW)
			v->m_CenterW = 0;
		else
			v->m_CenterW = n1;
		*/
		v->m_CenterW = n1 - v->m_CenterW;

		/* deal with initial packet state; we do this using the explicit
		pcm_returned==-1 flag, otherwise we're sensitive to first block
		being short or long */

		if (v->m_PCMReturned==-1)
		{
			v->m_PCMReturned = thisCenter;
			v->m_PCMCurrent = v->m_PCMReturned;
		}else{
			v->m_PCMReturned = prevCenter;
			v->m_PCMCurrent = v->m_PCMReturned + ci->m_BlockSize[v->m_lW]/4 + ci->m_BlockSize[v->m_W]/4;
		}

	}

	//temporarily delete unused code, maybe risky

	/* Update, cleanup */

	if (vb->m_EOffLag)
		v->m_EOffLag = 1;
	return(0);
}

int voOGGDecSynthesisRead(VorbisDSPState *v,int bytes)
{
	if(bytes && v->m_PCMReturned+bytes > v->m_PCMCurrent)
		return(OV_EINVAL);

	v->m_PCMReturned += bytes;
	
	return(0);
}

int voOGGDecBlockClear(CVOVorbisObjectDecoder *pvorbisdec, VorbisBlock *vb)
{
	voOGGDecBlockRipcord( pvorbisdec, vb);
	if(vb->m_pLocalStore)voOGGFree(vb->m_pLocalStore);

	voOGGMemset(vb, 0, sizeof(*vb));
	return(0);
}

void voOGGDecDSPClear(CVOVorbisObjectDecoder *pvorbisdec, VorbisDSPState *v)
{
	int i;
	if (v)
	{
		VorbisInfo *vi = v->m_pVI;
		CodecSetupInfo *ci = (CodecSetupInfo *)(vi?vi->m_pCodecSetup:NULL);
		PrivateState *b = (PrivateState *)v->m_pBackendState;

		if (v->m_ppPCM)
		{
			for(i=0; i<vi->m_Channels; i++)
				if (v->m_ppPCM[i])
					voOGGFree(v->m_ppPCM[i]);
			voOGGFree(v->m_ppPCM);
			if (v->m_ppPCMRet)
				voOGGFree(v->m_ppPCMRet);
		}

		/* free mode lookups; these are actually vorbis_look_mapping structs */
		if (ci)
		{
			for(i=0; i<ci->m_Modes; i++)
			{
				int mapnum = ci->m_pModeParam[i]->m_Mapping;
				int maptype = ci->m_MapType[mapnum];
				if (b && b->m_ppMode)
					Mapping_P[maptype]->free_look(pvorbisdec, b->m_ppMode[i]);
			}
		}

		if (b)
		{
			if (b->m_ppMode)
				voOGGFree(b->m_ppMode);    
			voOGGFree(b);
		}
		voOGGMemset(v, 0, sizeof(*v));
	}
}
