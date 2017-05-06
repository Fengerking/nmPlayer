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

#include "gecko2_codec_bld.h"
#include "gecko_codec.h"
#include "gecko2_codec.h"
#include "ra5_hdr.h"
#include "gecko2_params.h"

#define CODEC_ID "cook"

static const int MaxGeckoFlavor = 17;

npAudioCodec* npGecko2CodecBuilder::BuildCodec(const npAudioHeaderPtr& spHdr)
{
    npAudioCodec* pRet = 0;

    if (spHdr->Type() == npAudioHeader::htRA5)
    {
	npRealAudio5Header* pRA5 = (npRealAudio5Header*)spHdr.raw_ptr();

	if (!strcmp(pRA5->CodecID(), CODEC_ID))
	{
	    Gecko2Params params(pRA5->CodecFrameSize(),
				pRA5->Channels(),
				pRA5->SampleRate());

	    if (params.Unpack((const unsigned char*)pRA5->OpaqueData(),
			      pRA5->OpaqueDataSize()))
	    {
		if (pRA5->Flavor() < MaxGeckoFlavor)
		{
		    // This is G2 Gecko
		    npGeckoCodec* pGecko = new npGeckoCodec();
		    if (pGecko && pGecko->InitDecoder((int)(params.Samples() / params.Channels()),
			    (int)pRA5->SampleRate(),
			    (int)pRA5->Channels(),
			    (int)pRA5->CodecFrameSize(),
			    (BYTE*)pRA5->OpaqueData(),
			    (int)pRA5->OpaqueDataSize()))
			pRet = pGecko;
		    else
			delete pGecko;

		}
		else if (m_allowGecko2)
		{
		    // This is RA8 Gecko
		    npGecko2Codec* pGecko2 = new npGecko2Codec();

		    if (pGecko2 && pGecko2->InitDecoder((int)(params.Samples() / params.Channels()),
			    (int)pRA5->SampleRate(),
			    (int)pRA5->Channels(),
			    (int)pRA5->CodecFrameSize(),
			    (BYTE*)pRA5->OpaqueData(),
			    (int)pRA5->OpaqueDataSize()))
			pRet = pGecko2;
		    else
			delete pGecko2;
		}
		else
		{
		 //   DPRINTF (D_INFO,
		//	     ("npGecko2ShimBuilder::BuildShim() : RA8 Gecko support not enabled\n"));
		}
	    }
	}
    }

    return pRet;
}
