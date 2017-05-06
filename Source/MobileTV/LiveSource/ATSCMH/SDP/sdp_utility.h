#ifndef __SDP_UTILITY_H__
#define __SDP_UTILITY_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "commonheader.h"


const char * SDP_GetNextLine(const char * _sdpData);

size_t SDP_GetLineLength(const char * _sdpLine);

char * SDP_StringDup(const char * _string);
char * SDP_StringDup_N(const char * _string, size_t len); 



#ifdef __cplusplus
}
#endif

#endif //__SDP_UTILITY_H__
