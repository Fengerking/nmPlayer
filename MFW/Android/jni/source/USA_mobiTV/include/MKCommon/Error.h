#ifndef __MKCommon_Error_h__
#define __MKCommon_Error_h__

#include "../MKBase/Platform.h"
#include "../MKBase/Assert.h"
#include "../MKBase/Type.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

typedef MK_U32 MK_Error;

/******************************************************************************/

typedef void (*MK_ErrorFunc)(void* aObj, MK_Error aErr, const MK_Char* aMsg);

/******************************************************************************/

/*
 * TODO: Implement enums/macros/... to simplify use of the kind of errors
 *       specified in the Errors & Logging specification from the previous
 *       stack implementation.
 */

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
