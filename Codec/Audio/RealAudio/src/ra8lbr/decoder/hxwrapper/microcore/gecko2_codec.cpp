/* ***** BEGIN LICENSE BLOCK *****
 * Version: RCSL 1.0/RPSL 1.0
 *
 * Portions Copyright (c) 1995-2004 RealNetworks, Inc. All Rights Reserved.
 *
 * The contents of this file, and the files included with this file, are
 * subject to the current version of the RealNetworks Public Source License
 * Version 1.0 (the "RPSL") available at
 * http://www.helixcommunity.org/content/rpsl unless you have licensed
 * the file under the RealNetworks Community Source License Version 1.0
 * (the "RCSL") available at http://www.helixcommunity.org/content/rcsl,
 * in which case the RCSL will apply. You may also obtain the license terms
 * directly from RealNetworks.  You may not use this file except in
 * compliance with the RPSL or, if you have a valid RCSL with RealNetworks
 * applicable to this file, the RCSL.  Please see the applicable RPSL or
 * RCSL for the rights, obligations and limitations governing use of the
 * contents of the file.
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
 *    http://www.helixcommunity.org/content/tck
 *
 * Contributor(s):
 *
 * ***** END LICENSE BLOCK ***** */
#include <string.h>

#include "gecko2_codec.h"

npGecko2Codec::npGecko2Codec() :
    m_initialized(false),
    m_maxSamples(-1),
    m_sampleRate(-1),
    m_channels(-1),
    m_delay(-1),
    m_concealFrames(0),
    m_frameSize(0),
    m_delayFrames(0),
    m_pCodec(0)
{}

npGecko2Codec::~npGecko2Codec()
{
    if (m_pCodec)
    {
	m_pCodec->FreeDecoder();
	delete m_pCodec;
	m_pCodec = 0;
    }
}


int npGecko2Codec::InitDecoder(int nSamples,
				   int sampleRate,
			       int nChannels,
			       int nFrameBits,
			       BYTE *pOpaqueData,
			       int opaqueDataLength)
{
    int ret = 0;

    int codingDelay = 0;

    if (m_pCodec)
    {
	m_pCodec->FreeDecoder();
	delete m_pCodec;
	m_pCodec = 0;
    }

    m_pCodec = new CNewGeckoDecoderShim();

    if (m_pCodec && m_pCodec->InitDecoder(sampleRate,
					  nChannels,
					  nFrameBits,
					  pOpaqueData,
					  opaqueDataLength,
					  codingDelay) == 0)
    {
	m_frameSize = nFrameBits >> 3;

	m_delayFrames = codingDelay;

	m_maxSamples = nSamples * nChannels;
	m_sampleRate = sampleRate;
	m_channels = nChannels;
	m_delay = m_delayFrames * m_maxSamples;

	m_initialized = true;
	ret = 1;
    }
    else
    {
	delete m_pCodec;
	m_pCodec = 0;
    }

    return ret;
}

void npGecko2Codec::Init(int type, const char* pConfig, int configSize)
{
    // We do not use this interface for initialization since
    // the builder uses the InitDecoder call to initialize things
}

bool npGecko2Codec::Initialized() const
{
    return m_initialized;
}

int npGecko2Codec::MaxSamples() const
{
    return m_maxSamples;
}

int npGecko2Codec::SampleRate() const
{
    return m_sampleRate;
}

int npGecko2Codec::Channels() const
{
    return m_channels;
}

int npGecko2Codec::Delay() const
{
    return m_delay;
}

void npGecko2Codec::Reset()
{
    if (m_pCodec)
    {
	unsigned char* pIn = new unsigned char[m_frameSize];
	short* pOut = new short[m_maxSamples];

	if (pIn && pOut)
	{
	    // Zero out the frame.
	    ::memset(pIn, 0, m_frameSize);

	    for (int i = 0; i < m_delayFrames; i++)
	    {
		m_pCodec->Decode(pIn, 0, pOut);
	    }

	    delete [] pIn;
	    delete [] pOut;
	}

	m_concealFrames = 0;
    }
}

int npGecko2Codec::Decode(const char* pData, int dataSize,
			  int& consumed,
			  signed short* pOut,
			  bool eof)
{
    int ret = -1;

    if (m_pCodec)
    {
	int nextLost = 0;

	if (m_concealFrames)
	{
	    // Handle conceal frames

	    if (m_pCodec->Decode((unsigned char*)pData, 1, pOut) == 0)
	    {
		// We were doing concealment and did not
		// consume any of the data bits handed in
		consumed = 0;
		m_concealFrames--;

		ret = m_maxSamples;
	    }

	}
	else if (dataSize)
	{
	    if (m_pCodec->Decode((unsigned char*)pData, 0, pOut) == 0)
	    {
		// Signal that all the data bits were consumed
		consumed = dataSize;

		ret = m_maxSamples;
	    }
	}
	else
	    ret = 0;
    }

    return ret;
}

void npGecko2Codec::Conceal(int samples)
{
    int frameCount = (samples + m_maxSamples - 1) / m_maxSamples;

    m_concealFrames += frameCount;
}
