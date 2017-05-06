#ifndef __SDP_ERROR_H__
#define __SDP_ERROR_H__

#ifdef __cplusplus
extern "C" {
#endif



typedef enum
{
	SDPERROR_SUCCESS = 0,
	SDPERROR_UNKNOWN,
	SDPERROR_NOT_ENOUGH_MEMORY,
	SDPERROR_DESCRIPTION_NOT_FOUND,

}SDPERRORCODE;

static int SDP_ErrorCode = 0;
int SDP_GetLastError();
void SDP_SetLastError(int errorCode);



#ifdef __cplusplus
}
#endif

#endif //__SDP_ERROR_H__