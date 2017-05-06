
#ifndef __VOADAPTIVESTREAMINGCONTROLLERAPI_H__

#define __VOADAPTIVESTREAMINGCONTROLLERAPI_H__

#include "voSource2.h"

extern "C"{

#ifndef _IOS
	VO_S32 VO_API voGetSrcHLSAPI(VO_SOURCE2_API* pHandle);

	VO_S32 VO_API voGetASCDASHAPI(VO_SOURCE2_API* pHandle);
#endif

	VO_S32 VO_API voGetAdaptiveStreamControllerAPI( VO_SOURCE2_API* pHandle );

};



#endif