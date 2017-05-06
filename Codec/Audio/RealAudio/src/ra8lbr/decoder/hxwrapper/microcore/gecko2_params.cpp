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

#include "pack.h"
#include "gecko2_params.h"

Gecko2Params::Gecko2Params( unsigned int frameBits,
			    unsigned int channels,
			    unsigned int sampleRate) :
    m_frameBits(frameBits),
    m_channels(channels),
    m_sampleRate(sampleRate),
    m_version(0),
    m_samples(0),
    m_regions(0),
    m_delay(0),
    m_cplStart(0),
    m_cplQBits(0)
{}

bool Gecko2Params::Unpack(const unsigned char* pOpaqueData,
			  unsigned int opaqueSize)
{
    bool ret = false;

    if (opaqueSize >= 8)
    {
	char* pData = (char*)pOpaqueData;

	m_version = ::Unpack_ui(pData);
	m_samples = ::Unpack_us(pData);
	m_regions = ::Unpack_us(pData);

	if (m_version >= NP_GECKO2_CODEC_VERSION)
	{
	    m_delay = ::Unpack_ui(pData);
	    m_cplStart = ::Unpack_us(pData);
	    m_cplQBits = ::Unpack_us(pData);
	}

	ret = true;
    }

    return ret;
}


unsigned int Gecko2Params::FrameBits() const
{
    return m_frameBits;
}


unsigned int Gecko2Params::Channels() const
{
    return m_channels;
}


unsigned int Gecko2Params::SampleRate() const
{
    return m_sampleRate;
}


unsigned int Gecko2Params::Version() const
{
    return m_version;
}


unsigned int Gecko2Params::Samples() const
{
    return m_samples;
}


unsigned int Gecko2Params::Regions() const
{
    return m_regions;
}


unsigned int Gecko2Params::Delay() const
{
    return m_delay;
}


unsigned int Gecko2Params::CPLStart() const
{
    return m_cplStart;
}


unsigned int Gecko2Params::CPLQBits() const
{
    return m_cplQBits;
}
