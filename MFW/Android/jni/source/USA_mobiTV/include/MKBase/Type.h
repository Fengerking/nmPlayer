/*
 * NOTE: Porting candidate.
 *
 * Defines basic types & conversions.
 */

#ifndef __MKBase_Type_h__
#define __MKBase_Type_h__

#include <MKBase/Platform.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

#define MK_Cast(aType, aVal) ((aType)(aVal))

/******************************************************************************/

typedef int MK_Bool;
typedef char MK_Char;
typedef long MK_PInt;

/******************************************************************************/

#define MK_ToBool(aVal) MK_Cast(MK_Bool, aVal)
#define MK_ToChar(aVal) MK_Cast(MK_Char, aVal)
#define MK_ToPInt(aVal) MK_Cast(MK_PInt, aVal)

/******************************************************************************/

typedef unsigned char MK_U8;
typedef unsigned short MK_U16;
typedef unsigned int MK_U32;
typedef unsigned long long MK_U64;

typedef signed char MK_S8;
typedef signed short MK_S16;
typedef signed int MK_S32;
typedef signed long long MK_S64;

/******************************************************************************/

#define MK_ToU8(aVal) MK_Cast(MK_U8, aVal)
#define MK_ToU16(aVal) MK_Cast(MK_U16, aVal)
#define MK_ToU32(aVal) MK_Cast(MK_U32, aVal)
#define MK_ToU64(aVal) MK_Cast(MK_U64, aVal)

#define MK_ToS8(aVal) MK_Cast(MK_S8, aVal)
#define MK_ToS16(aVal) MK_Cast(MK_S16, aVal)
#define MK_ToS32(aVal) MK_Cast(MK_S32, aVal)
#define MK_ToS64(aVal) MK_Cast(MK_S64, aVal)

/******************************************************************************/

#define MK_U8C(aVal) _MK_U8C(aVal)
#define MK_U16C(aVal) _MK_U16C(aVal)
#define MK_U32C(aVal) _MK_U32C(aVal)
#define MK_U64C(aVal) _MK_U64C(aVal)

#define MK_S8C(aVal) _MK_S8C(aVal)
#define MK_S16C(aVal) _MK_S16C(aVal)
#define MK_S32C(aVal) _MK_S32C(aVal)
#define MK_S64C(aVal) _MK_S64C(aVal)

/******************************************************************************/

#define MK_U8_MIN MK_U8C(_MK_U8_MIN)
#define MK_U8_MAX MK_U8C(_MK_U8_MAX)
#define MK_U16_MIN MK_U16C(_MK_U16_MIN)
#define MK_U16_MAX MK_U16C(_MK_U16_MAX)
#define MK_U32_MIN MK_U32C(_MK_U32_MIN)
#define MK_U32_MAX MK_U32C(_MK_U32_MAX)
#define MK_U64_MIN MK_U64C(_MK_U64_MIN)
#define MK_U64_MAX MK_U64C(_MK_U64_MAX)

/*
 * NOTE: The literal "most negative int" cannot be written in C -- the rules in
 * the standard (section 6.4.4.1 in C99) will give it an unsigned type, so
 * MK_S32_MIN (and the most negative member of any larger signed type) must be
 * written via a constant expression.
 */

#define MK_S8_MIN (-MK_S8_MAX - MK_S8C(1))
#define MK_S8_MINU (MK_U8_MAX - MK_U8C(_MK_S8_MAX))
#define MK_S8_MAX MK_S8C(_MK_S8_MAX)
#define MK_S8_MAXU MK_U8C(_MK_S8_MAX)

#define MK_S16_MIN (-MK_S16_MAX - MK_S16C(1))
#define MK_S16_MINU (MK_U16_MAX - MK_U16C(_MK_S16_MAX))
#define MK_S16_MAX MK_S16C(_MK_S16_MAX)
#define MK_S16_MAXU MK_U16C(_MK_S16_MAX)

#define MK_S32_MIN (-MK_S32_MAX - MK_S32C(1))
#define MK_S32_MINU (MK_U32_MAX - MK_U32C(_MK_S32_MAX))
#define MK_S32_MAX MK_S32C(_MK_S32_MAX)
#define MK_S32_MAXU MK_U32C(_MK_S32_MAX)

#define MK_S64_MIN (-MK_S64_MAX - MK_S64C(1))
#define MK_S64_MINU (MK_U64_MAX - MK_U64C(_MK_S64_MAX))
#define MK_S64_MAX MK_S64C(_MK_S64_MAX)
#define MK_S64_MAXU MK_U64C(_MK_S64_MAX)

/******************************************************************************/

typedef MK_S32 MK_Result;

/******************************************************************************/

#define MK_ToResult(aVal) MK_Cast(MK_Result, aVal)

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

#define _MK_U8C(aVal) (aVal ## U)
#define _MK_U16C(aVal) (aVal ## U)
#define _MK_U32C(aVal) (aVal ## U)
#define _MK_U64C(aVal) (aVal ## ULL)

#define _MK_S8C(aVal) (aVal)
#define _MK_S16C(aVal) (aVal)
#define _MK_S32C(aVal) (aVal)
#define _MK_S64C(aVal) (aVal ## LL)

/******************************************************************************/

#define _MK_U8_MIN 0
#define _MK_U8_MAX 255
#define _MK_U16_MIN 0
#define _MK_U16_MAX 65535
#define _MK_U32_MIN 0
#define _MK_U32_MAX 4294967295
#define _MK_U64_MIN 0
#define _MK_U64_MAX 18446744073709551615

#define _MK_S8_MAX 127
#define _MK_S16_MAX 32767
#define _MK_S32_MAX 2147483647
#define _MK_S64_MAX 9223372036854775807

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
