/*
 * Implements some simple utility functions.
 */

#ifndef __MKBase_Util_h__
#define __MKBase_Util_h__

#include <MKBase/Platform.h>
#include <MKBase/Type.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

/* Functions to determine the absolute value of an integer */

#define __GEN_MK_Abs(aType) \
    MK_INLINE MK_##aType MK_Abs_##aType(MK_##aType aVal) \
    { \
        return (aVal >= MK_To##aType(0) ? aVal : -aVal); \
    }

__GEN_MK_Abs(S8);
__GEN_MK_Abs(S16);
__GEN_MK_Abs(S32);
__GEN_MK_Abs(S64);

#undef __GEN_MK_Abs

/* Dummy methods for absolute value on unsigned integers */

#define __GEN_MK_Abs(aType) \
    MK_INLINE MK_##aType MK_Abs_##aType(MK_##aType aVal) \
    { \
        return aVal; \
    }

__GEN_MK_Abs(U8);
__GEN_MK_Abs(U16);
__GEN_MK_Abs(U32);
__GEN_MK_Abs(U64);

#undef __GEN_MK_Abs

/******************************************************************************/

/* Functions to determine the smallest value of two integers */

#define __GEN_MK_Min(aType) \
    MK_INLINE MK_##aType MK_Min_##aType(MK_##aType aVal1, MK_##aType aVal2) \
    { \
        return (aVal1 <= aVal2 ? aVal1 : aVal2); \
    }

__GEN_MK_Min(S8);
__GEN_MK_Min(S16);
__GEN_MK_Min(S32);
__GEN_MK_Min(S64);

__GEN_MK_Min(U8);
__GEN_MK_Min(U16);
__GEN_MK_Min(U32);
__GEN_MK_Min(U64);

#undef __GEN_MIN

/******************************************************************************/

/* Functions to determine the largest value of two integers */

#define __GEN_MK_Max(aType) \
    MK_INLINE MK_##aType MK_Max_##aType(MK_##aType aVal1, MK_##aType aVal2) \
    { \
        return (aVal1 >= aVal2 ? aVal1 : aVal2); \
    }

__GEN_MK_Max(S8);
__GEN_MK_Max(S16);
__GEN_MK_Max(S32);
__GEN_MK_Max(S64);

__GEN_MK_Max(U8);
__GEN_MK_Max(U16);
__GEN_MK_Max(U32);
__GEN_MK_Max(U64);

#undef __GEN_MAX

/******************************************************************************/

/* Functions to find the greatest common divisor of two integers */

#define __GEN_MK_GCD(aType) \
    MK_INLINE MK_##aType MK_GCD_##aType(MK_##aType aVal1, MK_##aType aVal2) \
    { \
        MK_##aType lMod; \
        aVal1 = MK_Abs_##aType(aVal1); \
        aVal2 = MK_Abs_##aType(aVal2); \
        do \
        { \
            lMod = aVal1 % aVal2; \
            aVal1 = aVal2; \
        } \
        while ((aVal2 = lMod) > MK_To##aType(0)); \
        return aVal1; \
    }

__GEN_MK_GCD(S8);
__GEN_MK_GCD(S16);
__GEN_MK_GCD(S32);
__GEN_MK_GCD(S64);

__GEN_MK_GCD(U8);
__GEN_MK_GCD(U16);
__GEN_MK_GCD(U32);
__GEN_MK_GCD(U64);

#undef __GEN_GCD

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
