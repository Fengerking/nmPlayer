#ifndef __DISCRETIXDRMAPI_H__

#define __DISCRETIXDRMAPI_H__


#include "DRMContentDecryption.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DiscretixDRMAPI
{
	DrmContextInitializerFunc				Init;
	DrmContextDestroyFunc					Destroy;

	DrmPiffFragmentSampleDecryptionFunc		PiffSampleDecrypt;
	DrmPiffFragmentDecryptionFunc			PiffFragmentDecrypt;

	DrmHlsPayloadDecryptionFunc				HLSDecrypt;

	HlsFormatConvertorToDx_3_0Func			Convertor;
	DestroyPlaylistFunc						ReleasePlaylist;

	DrmEnvelopePayloadDecryptionFunc		EnvelopeDecrypt;

	void*									pUserData;
};

#ifdef __cplusplus
}
#endif


#endif //__DISCRETIXDRMAPI_H__