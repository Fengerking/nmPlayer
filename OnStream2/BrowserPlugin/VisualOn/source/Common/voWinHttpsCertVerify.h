
/*
 *
 * VisualOn 
 * david
 * 2011-07-06
 *
 */

#if !defined __voWinHttpsCertVerify_H__
#define __voWinHttpsCertVerify_H__

#define VOCERT_ERROR_NONE 0
#define VOCERT_ERROR_VERIFY_FAILED -1
#define VOCERT_ERROR_NOT_HTTPS -2 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int voWinHttpsCertVerify(void* pswzURI2Open);

#ifdef __cplusplus
}
#endif

#endif