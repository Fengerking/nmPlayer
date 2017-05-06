/* ***** BEGIN LICENSE BLOCK ***** 
 * Version: RCSL 1.0 and Exhibits. 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM 
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. 
 * All Rights Reserved. 
 * 
 * The contents of this file, and the files included with this file, are 
 * subject to the current version of the RealNetworks Community Source 
 * License Version 1.0 (the "RCSL"), including Attachments A though H, 
 * all available at http://www.helixcommunity.org/content/rcsl. 
 * You may also obtain the license terms directly from RealNetworks. 
 * You may not use this file except in compliance with the RCSL and 
 * its Attachments. There are no redistribution rights for the source 
 * code of this file. Please see the applicable RCSL for the rights, 
 * obligations and limitations governing use of the contents of the file. 
 * 
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the portions 
 * it created. 
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * Technology Compatibility Kit Test Suite(s) Location: 
 * https://rarvcode-tck.helixcommunity.org 
 * 
 * Contributor(s): 
 * 
 * ***** END LICENSE BLOCK ***** */ 

// Gecko2DecShim.cpp: implementation of the Gecko2DecoderShim class.
//
//////////////////////////////////////////////////////////////////////

#include "hxtypes.h"
#include "geckoopaquedata.h"

#include "gecko2codec.h"
#include "gecko2decshim.h"

CGecko2DecoderShim::CGecko2DecoderShim() {}
CGecko2DecoderShim::~CGecko2DecoderShim(){}

HX_RESULT CGecko2DecoderShim::CreateInstance(CGecko2DecoderShim** ppInstance, int version)
{
  *ppInstance = 0 ;
  switch (version)
  {
    case OLDGECKO_VERSION:
    case BETA1_GECKO_VERSION:
      *ppInstance = new COldGeckoDecoderShim() ;
      break ;

    case GECKO_VERSION:
      *ppInstance = new CNewGeckoDecoderShim() ;
      break ;

    /* no multichannel support yet */

    default:
      return HXR_INVALID_VERSION ;
      break ;
  }
  if (!ppInstance)
    return HXR_OUTOFMEMORY ;

  return HXR_OK ;
}

COldGeckoDecoderShim::COldGeckoDecoderShim() {}
COldGeckoDecoderShim::~COldGeckoDecoderShim(){}

HX_RESULT COldGeckoDecoderShim::InitDecoder(int sampleRate, int nChannels, int nFrameBits, BYTE *pOpaqueData, int opaqueDataLength, int &codingDelay)
{
  int i ;

  GeckoOpaqueData janusData ;
  janusData.unpack(pOpaqueData, opaqueDataLength) ;

  mChannels     = nChannels ;
  mFrameSamples = janusData.nSamples / nChannels ;
  mFrameBytes   = nFrameBits / (8*nChannels);

  mInterleaveBuffer = 0 ;

	if (mChannels > 1) {
		mInterleaveBuffer = new short[mFrameSamples];
		if (!mInterleaveBuffer) {
			FreeDecoder() ;
			return HXR_OUTOFMEMORY;
		}
	}
	
	for (i = 0; i < 2; i++)
		gecko2Decoder[i] = 0;

  for (i = 0; i < nChannels; i++) {
		gecko2Decoder[i] = Gecko2InitDecoder(mFrameSamples, 1, janusData.nRegions, 8*mFrameBytes, sampleRate, 0, 0, &codingDelay);
		if (!gecko2Decoder[i]) {
			FreeDecoder();
			return HXR_OUTOFMEMORY;
		}
	}

	return HXR_OK;
}

HX_RESULT COldGeckoDecoderShim::Decode(unsigned char * bitstream, int nextlost, signed short * output)
{
	int i, j, err = 0 ;
	
	/* if we are decoding mono, decode into output directly, otherwise interleave */
	if (mChannels == 1) {
		err = Gecko2Decode(gecko2Decoder[0], bitstream, /*8*mFrameBytes,*/ nextlost, output) ;
	} else {
		for (i = 0; i < mChannels && err == 0; i++) {
			err = Gecko2Decode(gecko2Decoder[i], bitstream, /*8*mFrameBytes,*/ nextlost, mInterleaveBuffer) ;
			for (j = 0; j < mFrameSamples; j++)
				output[2*j+i] = mInterleaveBuffer[j];
			bitstream += mFrameBytes ;
		}
	}
	
	if (err)
		return HXR_FAIL;

	return HXR_OK;
}

void COldGeckoDecoderShim::FreeDecoder()
{
    int i;

	for (i = 0; i < mChannels; i++) {
		if (gecko2Decoder[i])
			Gecko2FreeDecoder(gecko2Decoder[i]);
		gecko2Decoder[i] = 0;
	}
	
	if (mInterleaveBuffer) {
		delete[] mInterleaveBuffer;
		mInterleaveBuffer = 0;
	}
}

/* the shim for the new codec is rather thin in fact */

CNewGeckoDecoderShim::CNewGeckoDecoderShim() {}
CNewGeckoDecoderShim::~CNewGeckoDecoderShim(){}

HX_RESULT CNewGeckoDecoderShim::InitDecoder(int sampleRate, int nChannels, int nFrameBits, BYTE *pOpaqueData, int opaqueDataLength, int &codingDelay)
{
  GeckoOpaqueData janusData ;
  janusData.unpack(pOpaqueData, opaqueDataLength) ;

  mFrameBits = nFrameBits ;
  mSamplesPerFrame = janusData.nSamples ;

  gecko2Decoder = Gecko2InitDecoder(janusData.nSamples / nChannels, nChannels, janusData.nRegions, mFrameBits, sampleRate, janusData.cplStart, janusData.cplQBits, &codingDelay);
	if (!gecko2Decoder)
		return HXR_OUTOFMEMORY ;

	return HXR_OK;
}

HX_RESULT CNewGeckoDecoderShim::Decode(unsigned char * bitstream, int nextlost, signed short * output)
{
	if (Gecko2Decode(gecko2Decoder, /* mFrameBits,*/ bitstream, nextlost, output))
		return HXR_FAIL ;

	return HXR_OK;
}

void CNewGeckoDecoderShim::FreeDecoder()
{
	Gecko2FreeDecoder(gecko2Decoder);
	gecko2Decoder = 0;
}
