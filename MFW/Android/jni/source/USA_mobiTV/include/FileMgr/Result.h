/*
 * Extends result codes with FileMgr specific errors.
 */

#ifndef __FileMgr_Result_h__
#define __FileMgr_Result_h__

/* TODO: Include all direct base libraries as they are added */
#include <DRM/Result.h>
#include <MKBase/Result.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* FILEMGR CODES                                                              */
/******************************************************************************/

/* Helper macros for HTTP code creation */

#define MK_C_FILEMGR         MK_C(0x400)
#define MK_S_FILEMGR(aCode)  MK_S(MK_C_FILEMGR, aCode) 
#define MK_E_FILEMGR(aCode)  MK_E(MK_C_FILEMGR, aCode)

/* Success codes */

/* Error codes */

#define MK_E_FILEMGR_FAIL    MK_E_FILEMGR(0x000) /* Unspecified failure */
#define MK_E_FILEMGR_CRIT    MK_E_FILEMGR(0x001) /* Unspecified critical/abortive failure */

/******************************************************************************/
/* HTTP CODES (may be moved to separate HTTP lib at some point)               */
/******************************************************************************/

/* Helper macros for HTTP code creation */

#define MK_C_HTTP         MK_C(0x200)
#define MK_S_HTTP(aCode)  MK_S(MK_C_HTTP, aCode) 
#define MK_E_HTTP(aCode)  MK_E(MK_C_HTTP, aCode)
#define MK_R_HTTP(aCode)  _MK_R_HTTP(aCode)

/* Success codes */

#define MK_S_HTTP_100     MK_S_HTTP(0x100) /* HTTP/100: Continue */
#define MK_S_HTTP_101     MK_S_HTTP(0x101) /* HTTP/101: Switching Protocols */
#define MK_S_HTTP_200     MK_S_HTTP(0x200) /* HTTP/200: OK */
#define MK_S_HTTP_201     MK_S_HTTP(0x201) /* HTTP/201: Created */
#define MK_S_HTTP_202     MK_S_HTTP(0x202) /* HTTP/202: Accepted */
#define MK_S_HTTP_203     MK_S_HTTP(0x203) /* HTTP/203: Non-Authoritative Information */
#define MK_S_HTTP_204     MK_S_HTTP(0x204) /* HTTP/204: No Content */
#define MK_S_HTTP_205     MK_S_HTTP(0x205) /* HTTP/205: Reset Content */
#define MK_S_HTTP_206     MK_S_HTTP(0x206) /* HTTP/206: Partial Content */
#define MK_S_HTTP_300     MK_S_HTTP(0x300) /* HTTP/300: Multiple Choices */
#define MK_S_HTTP_301     MK_S_HTTP(0x301) /* HTTP/301: Moved Permanently */
#define MK_S_HTTP_302     MK_S_HTTP(0x302) /* HTTP/302: Found */
#define MK_S_HTTP_303     MK_S_HTTP(0x303) /* HTTP/303: See Other */
#define MK_S_HTTP_304     MK_S_HTTP(0x304) /* HTTP/304: Not Modified */
#define MK_S_HTTP_305     MK_S_HTTP(0x305) /* HTTP/305: Use Proxy */
#define MK_S_HTTP_307     MK_S_HTTP(0x307) /* HTTP/307: Temporary Redirect */

/* Error codes */

#define MK_E_HTTP_FAIL    MK_E_HTTP(0x000) /* Unspecified failure */
#define MK_E_HTTP_CRIT    MK_E_HTTP(0x001) /* Unspecified critical/abortive failure */

#define MK_E_HTTP_400     MK_E_HTTP(0x400) /* HTTP/400: Bad Request */
#define MK_E_HTTP_401     MK_E_HTTP(0x401) /* HTTP/401: Unauthorized */
#define MK_E_HTTP_402     MK_E_HTTP(0x402) /* HTTP/402: Payment Required */
#define MK_E_HTTP_403     MK_E_HTTP(0x403) /* HTTP/403: Forbidden */
#define MK_E_HTTP_404     MK_E_HTTP(0x404) /* HTTP/404: Not Found */
#define MK_E_HTTP_405     MK_E_HTTP(0x405) /* HTTP/405: Method Not Allowed */
#define MK_E_HTTP_406     MK_E_HTTP(0x406) /* HTTP/406: Not Acceptable */
#define MK_E_HTTP_407     MK_E_HTTP(0x407) /* HTTP/407: Proxy Authentication Required */
#define MK_E_HTTP_408     MK_E_HTTP(0x408) /* HTTP/408: Request Time-out */
#define MK_E_HTTP_409     MK_E_HTTP(0x409) /* HTTP/409: Conflict */
#define MK_E_HTTP_410     MK_E_HTTP(0x410) /* HTTP/410: Gone */
#define MK_E_HTTP_411     MK_E_HTTP(0x411) /* HTTP/411: Length Required */
#define MK_E_HTTP_412     MK_E_HTTP(0x412) /* HTTP/412: Precondition Failed */
#define MK_E_HTTP_413     MK_E_HTTP(0x413) /* HTTP/413: Request Entity Too Large */
#define MK_E_HTTP_414     MK_E_HTTP(0x414) /* HTTP/414: Request-URI Too Large */
#define MK_E_HTTP_415     MK_E_HTTP(0x415) /* HTTP/415: Unsupported Media Type */
#define MK_E_HTTP_416     MK_E_HTTP(0x416) /* HTTP/416: Requested range not satisfiable */
#define MK_E_HTTP_417     MK_E_HTTP(0x417) /* HTTP/417: Expectation Failed */
#define MK_E_HTTP_500     MK_E_HTTP(0x500) /* HTTP/500: Internal Server Error */
#define MK_E_HTTP_501     MK_E_HTTP(0x501) /* HTTP/501: Not Implemented */
#define MK_E_HTTP_502     MK_E_HTTP(0x502) /* HTTP/502: Bad Gateway */
#define MK_E_HTTP_503     MK_E_HTTP(0x503) /* HTTP/503: Service Unavailable */
#define MK_E_HTTP_504     MK_E_HTTP(0x504) /* HTTP/504: Gateway Time-out */
#define MK_E_HTTP_505     MK_E_HTTP(0x505) /* HTTP/505: HTTP Version not supported */

/******************************************************************************/

/* Success code aliases */

#define MK_S_HTTP_CONTINUE MK_S_HTTP_100
#define MK_S_HTTP_SWITCHING_PROTOCOLS MK_S_HTTP_101
#define MK_S_HTTP_OK MK_S_HTTP_200
#define MK_S_HTTP_CREATED MK_S_HTTP_201
#define MK_S_HTTP_ACCEPTED MK_S_HTTP_202
#define MK_S_HTTP_NON_AUTHORITATIVE_INFORMATION MK_S_HTTP_203
#define MK_S_HTTP_NO_CONTENT MK_S_HTTP_204
#define MK_S_HTTP_RESET_CONTENT MK_S_HTTP_205
#define MK_S_HTTP_PARTIAL_CONTENT MK_S_HTTP_206
#define MK_S_HTTP_MULTIPLE_CHOICES MK_S_HTTP_300
#define MK_S_HTTP_MOVED_PERMANENTLY MK_S_HTTP_301
#define MK_S_HTTP_FOUND MK_S_HTTP_302
#define MK_S_HTTP_SEE_OTHER MK_S_HTTP_303
#define MK_S_HTTP_NOT_MODIFIED MK_S_HTTP_304
#define MK_S_HTTP_USE_PROXY MK_S_HTTP_305
#define MK_S_HTTP_TEMPORARY_REDIRECT MK_S_HTTP_307

/* Error code aliases */

#define MK_E_HTTP_BAD_REQUEST MK_E_HTTP_400
#define MK_E_HTTP_UNAUTHORIZED MK_E_HTTP_401
#define MK_E_HTTP_PAYMENT_REQUIRED MK_E_HTTP_402
#define MK_E_HTTP_FORBIDDEN MK_E_HTTP_403
#define MK_E_HTTP_NOT_FOUND MK_E_HTTP_404
#define MK_E_HTTP_METHOD_NOT_ALLOWED MK_E_HTTP_405
#define MK_E_HTTP_NOT_ACCEPTABLE MK_E_HTTP_406
#define MK_E_HTTP_PROXY_AUTHENTICATION_REQUIRED MK_E_HTTP_407
#define MK_E_HTTP_REQUEST_TIME_OUT MK_E_HTTP_408
#define MK_E_HTTP_CONFLICT MK_E_HTTP_409
#define MK_E_HTTP_GONE MK_E_HTTP_410
#define MK_E_HTTP_LENGTH_REQUIRED MK_E_HTTP_411
#define MK_E_HTTP_PRECONDITION_FAILED MK_E_HTTP_412
#define MK_E_HTTP_REQUEST_ENTITY_TOO_LARGE MK_E_HTTP_413
#define MK_E_HTTP_REQUEST_URI_TOO_LARGE MK_E_HTTP_414
#define MK_E_HTTP_UNSUPPORTED_MEDIA_TYPE MK_E_HTTP_415
#define MK_E_HTTP_REQUESTED_RANGE_NOT_SATISFIABLE MK_E_HTTP_416
#define MK_E_HTTP_EXPECTATION_FAILED MK_E_HTTP_417
#define MK_E_HTTP_INTERNAL_SERVER_ERROR MK_E_HTTP_500
#define MK_E_HTTP_NOT_IMPLEMENTED MK_E_HTTP_501
#define MK_E_HTTP_BAD_GATEWAY MK_E_HTTP_502
#define MK_E_HTTP_SERVICE_UNAVAILABLE MK_E_HTTP_503
#define MK_E_HTTP_GATEWAY_TIME_OUT MK_E_HTTP_504
#define MK_E_HTTP_HTTP_VERSION_NOT_SUPPORTED MK_E_HTTP_505

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

/*
 * Resolves FileMgr (and sub-library) result-codes. Returns non-NULL if the
 * result-code was resolved and NULL otherwise.ific code matched).
 */
const MK_Char* MK_FileMgr_GetCategoryDesc(MK_Result aRes, const MK_Char** aName);
const MK_Char* MK_FileMgr_GetResultDesc(MK_Result aRes, const MK_Char** aName);

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

MK_INLINE MK_Result _MK_R_HTTP(MK_S32 aCode)
{
    return (0x400 <= aCode || 0x100 > aCode) ? MK_E_HTTP(aCode) : MK_S_HTTP(aCode);
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
