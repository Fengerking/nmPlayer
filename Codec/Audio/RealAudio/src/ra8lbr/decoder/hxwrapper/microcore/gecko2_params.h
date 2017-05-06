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

#ifndef _GECKO2_PARAMS_H_
#define _GECKO2_PARAMS_H_

#define NP_GECKO_CODEC_VERSION  0x01000002 // G2 audio version
#define NP_GECKO2_CODEC_VERSION 0x01000003 // RA8 version

class Gecko2Params
{
public:
    Gecko2Params( unsigned int frameBits,
		 unsigned int channels,
		 unsigned int sampleRate);

    bool Unpack(const unsigned char* pOpaqueData,
		unsigned int opaqueSize);

    unsigned int FrameBits() const;
    unsigned int Channels() const;
    unsigned int SampleRate() const;
    unsigned int Version() const;
    unsigned int Samples() const;
    unsigned int Regions() const;
    unsigned int Delay() const;
    unsigned int CPLStart() const;
    unsigned int CPLQBits() const;

private:
    unsigned int m_frameBits;
    unsigned int m_channels;
    unsigned int m_sampleRate;
    unsigned int m_version;
    unsigned int m_samples;
    unsigned int m_regions;
    unsigned int m_delay;
    unsigned int m_cplStart;
    unsigned int m_cplQBits;
};

#endif //_GECKO2_PARAMS_H_
