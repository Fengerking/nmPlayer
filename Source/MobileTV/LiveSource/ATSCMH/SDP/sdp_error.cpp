
#include "sdp_error.h"

int SDP_GetLastError()
{
	return SDP_ErrorCode;
}

void SDP_SetLastError(int errorCode)
{
	SDP_ErrorCode = errorCode;
}
