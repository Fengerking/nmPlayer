/**
* Some definitions for Native Window
*
*	David@2011/12/2
*/


#if !defined __VONW_DEFS__
#define __VONW_DEFS__

#include <ui/GraphicBuffer.h>
#include "OMX_Types.h"
#include "OMX_Core.h"

using namespace android;

template<class T>
static void InitOMXParams(T *params) {
    params->nSize = sizeof(T);
    params->nVersion.s.nVersionMajor = 1;
    params->nVersion.s.nVersionMinor = 0;
    params->nVersion.s.nRevision = 0;
    params->nVersion.s.nStep = 0;
}

enum CreationFlags {
	kPreferSoftwareCodecs    = 1,
	kIgnoreCodecSpecificData = 2,

	// The client wants to access the output buffer's video
	// data for example for thumbnail extraction.
	kClientNeedsFramebuffer  = 4,

	// Request for software or hardware codecs. If request
	// can not be fullfilled, Create() returns NULL.
	kSoftwareCodecsOnly      = 8,
	kHardwareCodecsOnly      = 16,

	// Store meta data in video buffers
	kStoreMetaDataInVideoBuffers = 32,

	// Only submit one input buffer at one time.
	kOnlySubmitOneInputBufferAtOneTime = 64,

	// Enable GRALLOC_USAGE_PROTECTED for output buffers from native window
	kEnableGrallocUsageProtected = 128,

	// Secure decoding mode
	kUseSecureInputBuffers = 256,
};


typedef union OMXVERSIONTYPE {

	struct {
		unsigned char nVersionMajor;
		unsigned char nVersionMinor;
		unsigned char nRevision;
		unsigned char nStep;
	} s;
	unsigned int nVersion;
} OMXVERSIONTYPE;

typedef struct __VO_GRAPHICBUFFER_WRAPPER__ {
	void**			ppHeader;
	void* 			pBufferMeta;
	unsigned char*		pBuffer;
} VOGRAPHICBUFFERWRAPPER;

enum BufferStatus {
	OWNED_BY_US,
	OWNED_BY_COMPONENT,
	OWNED_BY_NATIVE_WINDOW,
	OWNED_BY_CLIENT,
};

typedef struct __VO_BUFFERINFO__{
	OMX_BUFFERHEADERTYPE*	pBuffHead;
	void*					pdata;
	size_t					nsize;
	BufferStatus			status;
	sp<GraphicBuffer>		pgb;
} VOBUFFERINFO;


#endif
