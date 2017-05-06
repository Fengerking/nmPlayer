#ifndef __MKBase_FBuf_h__
#define __MKBase_FBuf_h__

#include <MKBase/Platform.h>
#include <MKBase/Type.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

/*
 * TODO: Meant to become a full "fixed"-buffer implementation in the future. For
 *       now only the struct is defined.
 */

typedef struct _MK_FBuf MK_FBuf;

struct _MK_FBuf
{
    MK_U32 Sz;
    MK_U8* Buf;
};

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
