#ifndef __VO_BASE64_H__
#define __VO_BASE64_H__

#include "voYYDef_filcmn.h"
#include <string.h>
#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#ifdef __cplusplus
extern "C" {
#endif
#endif


/**
	* Decode the input buffer
	* \param strInput [IN] the input buffer need to be decoded
	* \param uDecBufSize [OUT] real size of buffer after decode
	* \param bTrimTrailingZeros [IN] if need trim the trailing zero
	* \retval a buffer of size "uDecBufSize"
	*/
VO_U8 * Base64Decode(VO_CHAR * strInput, VO_U32 & uDecBufSize, VO_BOOL bTrimTrailingZeros = VO_TRUE);

/**
	* Decode the input buffer
	* \param strInput [IN] the input buffer need to be decoded
	* \param uBufSize [IN],the input buffer size,[OUT] real size of buffer after decode
	* \param bTrimTrailingZeros [IN] if need trim the trailing zero
	* \retval a buffer of size "uDecBufSize"
	*/
VO_U8 * Base64Decode2(VO_CHAR * strInput, VO_U32 & uBufSize, VO_BOOL bTrimTrailingZeros = VO_TRUE);

/*
	Returns a NULL-terminated string that
	the caller is responsible for delete[].
*/
/**
	* Encode the input buffer
	* \param strOri [IN] the input buffer need to be encoded
	* \param uStrLength [IN] the size of strOri
	* \retval a string encoded with null-terminated
	*/
VO_CHAR * Base64Encode(VO_CHAR const * strOri, VO_U32 uStrLength);

#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif

#endif
#endif //__VO_BASE64_H__