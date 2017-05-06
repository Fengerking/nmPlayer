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

// Gecko2CodecShim.h: interface for the Gecko2CodecShim class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GECKO2DECSHIM_H_
#define _GECKO2DECSHIM_H_

#include "gecko2shim.h"
#include "gecko2codec.h"

/* this is a pure virtual shim layer class. Its purpose is to 
make the old and new gecko codec behave alike. */

/* JR - notes:
 * Set up this way so that the wrapper function (in ragecko2.cpp) can use the
 *   same variable (CGecko2CodecShim *mCodecShim) to point to an instance of
 *   either old or new gecko
 */

class COldGeckoDecoderShim : public CGecko2DecoderShim
{
public:
	COldGeckoDecoderShim();
	virtual ~COldGeckoDecoderShim();

	virtual HX_RESULT InitDecoder(int sampleRate, int nChannels, int nFrameBits, BYTE *pOpaqueData, int opaqueDataLength, int &codingDelay) ;
	virtual void FreeDecoder();
	virtual HX_RESULT Decode(unsigned char *bitstream, int nextlost, signed short *output);
	virtual int GetSamplesPerFrame() const {return mFrameSamples * mChannels ;}

private:
	int mChannels, mFrameSamples, mFrameBytes;
	short *mInterleaveBuffer;
	HGecko2Decoder gecko2Decoder[2];	/* max 2 instances (for G2 stereo) */
};

class CNewGeckoDecoderShim : public CGecko2DecoderShim
{
public:
	CNewGeckoDecoderShim();
	virtual ~CNewGeckoDecoderShim();

	virtual HX_RESULT InitDecoder(int sampleRate, int nChannels, int nFrameBits, BYTE *pOpaqueData, int opaqueDataLength, int &codingDelay);
	virtual void FreeDecoder();
	virtual HX_RESULT Decode(unsigned char *bitstream, int nextlost, signed short *output);
	virtual int GetSamplesPerFrame() const {return mSamplesPerFrame;}

private:
  int mFrameBits, mSamplesPerFrame ;
	HGecko2Decoder gecko2Decoder;
};

#endif /* _GECKO2DECSHIM_H_ */
