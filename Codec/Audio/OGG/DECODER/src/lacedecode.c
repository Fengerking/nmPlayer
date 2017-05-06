//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    lacedecode.c

Abstract:

    single block PCM decoding.

Author:

    Witten Wen 19-October-2009

Revision History:

*************************************************************************/

#include "vovorbisdec.h"
#include "functionreg.h"

/*
	0,  voOGGDecAudioPacket succeeds
	other value, voOGGDecAudioPacket fails

	This function do steps listed as [1.3.2. Decode Procedure] in SPEC

 */
int voOGGDecAudioPacket(CVOVorbisObjectDecoder *pvorbisdec, VorbisBlock *vb, OGGPacket *op, int decodep)
{
	VorbisDSPState		*vd = vb->vd;
	PrivateState		 *b  = (PrivateState *)vd->m_pBackendState;
	VorbisInfo           *vi = vd->m_pVI;
	CodecSetupInfo		 *ci = (CodecSetupInfo *)vi->m_pCodecSetup;
	VOOGGInputBitStream *obs = pvorbisdec->m_oggbs;
	int                type, mode, i;

	/* first things first.  Make sure decode is ready */
	voOGGDecBlockRipcord(pvorbisdec, vb);
	if(pvorbisdec->m_fileformat == 1)
		voOGGDecPackReadinit(obs, op);

	/* Check the packet type */
	if (voOGGDecPackReadBits(obs, 1)!=0)
	{							//////////////1.decode packet type flag
		/* Oops.  This is not an audio data packet */
		return(OV_ENOTAUDIO);
	}

	/* read our mode and pre/post windowsize */
	mode = voOGGDecPackReadBits(obs, b->m_Modebits);					//////////////2.decode mode number
	if (mode==-1) return(OV_EBADPACKET);

//	vb->m_Mode = mode;
	vb->m_W = ci->m_pModeParam[mode]->m_BlockFlag;
	if (vb->m_W)
	{
		vb->m_lW = voOGGDecPackReadBits(obs, 1);
		vb->m_nW = voOGGDecPackReadBits(obs, 1);
		if(vb->m_nW==-1)   return(OV_EBADPACKET);
	}else{
		vb->m_lW = 0;
		vb->m_nW = 0;
	}

	/* more setup */
	vb->m_GranulePos = op->m_GranulePos;
	vb->m_Sequence = op->m_PacketNo - 3; /* first block is third packet */
	vb->m_EOffLag = op->m_EOS;

	if (decodep)
	{
		/* alloc pcm passback storage */
		vb->m_PCMEnd = ci->m_BlockSize[vb->m_W];
		vb->m_ppPCM = (OGG_S32 **)voOGGDecBlockAlloc(pvorbisdec, vb, sizeof(*vb->m_ppPCM)*vi->m_Channels);
		for (i=0; i<vi->m_Channels; i++)
			vb->m_ppPCM[i] = (OGG_S32 *)voOGGDecBlockAlloc(pvorbisdec, vb, vb->m_PCMEnd*sizeof(*vb->m_ppPCM[i]));

		/* unpack_header enforces range checking */
		type = ci->m_MapType[ci->m_pModeParam[mode]->m_Mapping];

		(Mapping_P[type]->inverse(pvorbisdec, vb, b->m_ppMode[mode]));


		return 0;



	}else{
		/* no pcm */
		vb->m_PCMEnd = 0;
		vb->m_ppPCM = NULL;

		return(0);
	}
}

