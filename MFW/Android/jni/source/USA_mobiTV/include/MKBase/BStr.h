/*
 * NOTE: Porting candidate.
 *
 * WARNING: Considering not including this functionality at all since CStr
 *          family should handle most cases in a satisfactory manner.
 */

#ifndef __MKBase_BStr_h__
#define __MKBase_BStr_h__

#include <MKBase/Platform.h>
#include <MKBase/Config.h>
#include <MKBase/Assert.h>
#include <MKBase/Type.h>
#include <MKBase/Util.h>
#include <MKBase/Memory.h>
#include <MKBase/Char.h>

#include <ctype.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

/*
 * Convenience macro for casting to MK_Char* type.
 */
#define MK_ToBStr(aVal) MK_Cast(MK_Char*, aVal)

/******************************************************************************/

/*
 * Compare the strings aStr1 and aStr2. Returns zero if the strings are
 * identical, otherwise a negative value is returned to if aStr1 < aStr2 and
 * positive value if aStr1 > aStr2.
 */
MK_INLINE MK_S32 MK_BStr_Cmp(const MK_Char* aStr1, MK_U32 aLen1, const MK_Char* aStr2, MK_U32 aLen2)
{
    MK_ASSERT((NULL != aStr1 || 0 == aLen1) && (NULL != aStr2 || 0 == aLen2));
    if (aLen1 == aLen2)
    {
        return MK_Mem_Cmp(aStr1, aStr2, aLen1);
    }
    return (aLen1 > aLen2) ? 1 : -1;
}

/*
 * Compare first aN characters of the two strings aStr1 and aStr2. Returns zero
 * if the strings are identical, otherwise a negative value is returned to if
 * aStr1 < aStr2 and positive value if aStr1 > aStr2.
 */
MK_INLINE MK_S32 MK_BStr_CmpN(const MK_Char* aStr1, MK_U32 aLen1, const MK_Char* aStr2, MK_U32 aLen2, MK_U32 aN)
{
    MK_ASSERT(((NULL != aStr1 || 0 == aLen1) && (NULL != aStr2 || 0 == aLen2)) || 0 == aN);
    return MK_BStr_Cmp(aStr1, MK_Min_U32(aN, aLen1), aStr2, MK_Min_U32(aN, aLen2));
}

/*
 * Compare the strings aStr1 and aStr2 ignoring the case. Returns zero if the
 * strings are equal, otherwise a negative value is returned to if aStr1 < aStr2
 * and positive value if aStr1 > aStr2.
 */
MK_INLINE MK_S32 MK_BStr_CmpI(const MK_Char* aStr1, MK_U32 aLen1, const MK_Char* aStr2, MK_U32 aLen2)
{
    MK_ASSERT((NULL != aStr1 || 0 == aLen1) && (NULL != aStr2 || 0 == aLen2));
    if (aLen1 == aLen2)
    {
        MK_S32 r = 0;
        MK_U32 i = 0;
        for (; i < aLen1 && 0 == (r = MK_Char_CmpI(aStr1[i], aStr2[i])); ++i);
        return r;
    }
    return (aLen1 > aLen2) ? 1 : -1;
}

/*
 * Compare first aN characters of the two strings aStr1 and aStr2 ignoring the
 * case. Returns zero if the strings are identical, otherwise a negative value
 * is returned to if aStr1 < aStr2 and positive value if aStr1 > aStr2.
 */
MK_INLINE MK_S32 MK_BStr_CmpNI(const MK_Char* aStr1, MK_U32 aLen1, const MK_Char* aStr2, MK_U32 aLen2, MK_U32 aN)
{
    MK_ASSERT(((NULL != aStr1 || 0 == aLen1) && (NULL != aStr2 || 0 == aLen2)) || 0 == aN);
    return MK_BStr_CmpI(aStr1, MK_Min_U32(aN, aLen1), aStr2, MK_Min_U32(aN, aLen2));
}

/******************************************************************************/

/*
 * Find first occurance of aCh in aStr (including the terminating zero). Returns
 * a pointer to the occurance of aCh or NULL if it was not found.
 */
MK_INLINE MK_Char* MK_BStr_FindCh(const MK_Char* aStr, MK_U32 aLen, MK_Char aCh)
{
    MK_ASSERT(NULL != aStr || 0 == aLen);
    return MK_Cast(MK_Char*, MK_Mem_Find(aStr, aCh, aLen));
}

/*
 * Find first occurance of aVal in aPtr within the first aN characters. Returns
 * a pointer to the occurance of aCh or NULL if it was not found.
 */
MK_INLINE MK_Char* MK_BStr_FindChN(const MK_Char* aStr, MK_U32 aLen, MK_Char aCh, MK_U32 aN)
{
    return MK_BStr_FindCh(aStr, MK_Min_U32(aN, aLen), aCh);
}

/*
 * Find last occurance of aCh in aStr (including the terminating zero). Returns
 * a pointer to the occurance of aCh or NULL if it was not found.
 */
MK_INLINE MK_Char* MK_BStr_RFindCh(const MK_Char* aStr, MK_U32 aLen, MK_Char aCh)
{
    MK_ASSERT(NULL != aStr || 0 == aLen);
    return MK_Cast(MK_Char*, MK_Mem_RFind(aStr, aCh, aLen));
}

/*
 * Find last occurance of aCh in aStr within the first aN characters. Returns a
 * pointer to the occurance of aCh or NULL if it was not found.
 */
MK_INLINE MK_Char* MK_BStr_RFindChN(const MK_Char* aStr, MK_U32 aLen, MK_Char aCh, MK_U32 aN)
{
    return MK_BStr_RFindCh(aStr, MK_Min_U32(aN, aLen), aCh);
}

/******************************************************************************/

/*
 * Find first occurance of aSub in aStr. Returns a pointer to the occurance of
 * aSub or NULL if it was not found.
 */
MK_INLINE MK_Char* MK_BStr_FindStr(const MK_Char* aStr, MK_U32 aLen, const MK_Char* aSub, MK_U32 aSubLen)
{
    MK_ASSERT((NULL != aStr || 0 == aLen) && (NULL != aSub || 0 == aSubLen));
    if (aLen >= aSubLen)
    {
        if (0 != aSubLen)
        {
            MK_Char i = *aSub++;
            const MK_Char* e = aStr + aLen - --aSubLen;
            do
            {
                do if (e <= aStr) return NULL; while (i != *aStr++);
            }
            while (0 != MK_BStr_Cmp(aStr, aSubLen, aSub, aSubLen));
            --aStr;
        }
        return MK_Cast(MK_Char*, aStr);
    }
    return NULL;
}

/*
 * Find first occurance of aSub in aStr within the first aN characters. Returns
 * a pointer to the occurance of aSub or NULL if it was not found.
 */
MK_INLINE MK_Char* MK_BStr_FindStrN(const MK_Char* aStr, MK_U32 aLen, const MK_Char* aSub, MK_U32 aSubLen, MK_U32 aN)
{
    return MK_BStr_FindStr(aStr, MK_Min_U32(aN, aLen), aSub, aSubLen);
}

/*
 * Find first occurance of aSub in aStr. Returns a pointer to the occurance of
 * aSub or NULL if it was not found.
 */
MK_INLINE MK_Char* MK_BStr_FindStrI(const MK_Char* aStr, MK_U32 aLen, const MK_Char* aSub, MK_U32 aSubLen)
{
    MK_ASSERT((NULL != aStr || 0 == aLen) && (NULL != aSub || 0 == aSubLen));
    if (aLen >= aSubLen)
    {
        if (0 != aSubLen)
        {
            MK_Char i = MK_Char_ToUpper(*aSub++);
            const MK_Char* e = aStr + aLen - --aSubLen;
            do
            {
                do if (e <= aStr) return NULL; while (i != MK_Char_ToUpper(*aStr++));
            }
            while (0 != MK_BStr_CmpI(aStr, aSubLen, aSub, aSubLen));
            --aStr;
        }
        return MK_Cast(MK_Char*, aStr);
    }
    return NULL;
}

/*
 * Find first occurance of aSub in aStr within the first aN characters. Returns
 * a pointer to the occurance of aSub or NULL if it was not found.
 */
MK_INLINE MK_Char* MK_BStr_FindStrNI(const MK_Char* aStr, MK_U32 aLen, const MK_Char* aSub, MK_U32 aSubLen, MK_U32 aN)
{
    return MK_BStr_FindStrI(aStr, MK_Min_U32(aN, aLen), aSub, aSubLen);
}

/******************************************************************************/

/*
 * Span initial part of aStr as long as the characters are in aSet and return
 * the number of characters spanned (the substring length). Will return the
 * equivalent of aLen if all characters of aStr is in aSet.
 *
 * Similar to MK_BStr_CBreak().
 */
MK_INLINE MK_U32 MK_BStr_Span(const MK_Char* aStr, MK_U32 aLen, const MK_Char* aSet, MK_U32 aSetLen)
{
    MK_U32 lN = 0;
    MK_ASSERT((NULL != aStr || 0 == aLen) && (NULL != aSet || 0 == aSetLen));
    for (; lN < aLen && NULL != MK_BStr_FindCh(aSet, aSetLen, aStr[lN]); ++lN);
    return lN;
}

/*
 * Span initial part of aStr as long as the characters are in aSet to a maximum
 * value of aN characters and return the number of characters spanned (the
 * substring length). Will return the equivalent of Min(aN, aLen) if all
 * characters of aStr (up to aN) is in aSet.
 *
 * Similar to MK_BStr_CBreakN().
 */
MK_INLINE MK_U32 MK_BStr_SpanN(const MK_Char* aStr, MK_U32 aLen, const MK_Char* aSet, MK_U32 aSetLen, MK_U32 aN)
{
    return MK_BStr_Span(aStr, MK_Min_U32(aN, aLen), aSet, aSetLen);
}

/*
 * Span initial part of aStr as long as the characters are NOT in aSet (i.e. are
 * in the complement of aSet) and return the number of characters spanned (the
 * substring length). Will return the equivalent of aLen if no characters of
 * aStr are in aSet.
 *
 * Similar to MK_BStr_CBreak().
 */
MK_INLINE MK_U32 MK_BStr_CSpan(const MK_Char* aStr, MK_U32 aLen, const MK_Char* aSet, MK_U32 aSetLen)
{
    MK_U32 lN = 0;
    MK_ASSERT((NULL != aStr || 0 == aLen) && (NULL != aSet || 0 == aSetLen));
    for (; lN < aLen && NULL == MK_BStr_FindCh(aSet, aSetLen, aStr[lN]); ++lN);
    return lN;
}

/*
 * Span initial part of aStr as long as the characters are NOT in aSet (i.e. are
 * in the complement of aSet) to a maximum value of aN characters and return the
 * number of characters spanned (the substring length). Will return the
 * equivalent of Min(aN, aLen) if no characters of aStr (up to aN) are in aSet.
 *
 * Similar to MK_CStr_CBreak().
 */
MK_INLINE MK_U32 MK_BStr_CSpanN(const MK_Char* aStr, MK_U32 aLen, const MK_Char* aSet, MK_U32 aSetLen, MK_U32 aN)
{
    return MK_BStr_CSpan(aStr, MK_Min_U32(aN, aLen), aSet, aSetLen);
}

/******************************************************************************/

/*
 * Break at first occurance of any character from aSet in aStr. Returns a
 * pointer to the location of the matched character or NULL if no match was
 * found.
 *
 * Similar to MK_BStr_CSpan().
 */
MK_INLINE MK_Char* MK_BStr_Break(const MK_Char* aStr, MK_U32 aLen, const MK_Char* aSet, MK_U32 aSetLen)
{
    MK_U32 lN = MK_BStr_CSpan(aStr, aLen, aSet, aSetLen);
    return MK_Cast(MK_Char*, (aLen != lN) ? aStr + lN : NULL);
}

/*
 * Break at first occurance of any character from aSet in aStr within the first
 * aN characters. Returns a pointer to the location of the matched character or
 * NULL if no match was found.
 *
 * Similar to MK_BStr_CSpanN().
 */
MK_INLINE MK_Char* MK_BStr_BreakN(const MK_Char* aStr, MK_U32 aLen, const MK_Char* aSet, MK_U32 aSetLen, MK_U32 aN)
{
    return MK_BStr_Break(aStr, MK_Min_U32(aN, aLen), aSet, aSetLen);
}

/*
 * Break at first occurance of any character NOT from aSet (i.e. in the
 * complement of aSet) in aStr. Returns a pointer to the location of the matched
 * character or NULL if no match was found.
 *
 * Similar to MK_BStr_Span().
 */
MK_INLINE MK_Char* MK_BStr_CBreak(const MK_Char* aStr, MK_U32 aLen, const MK_Char* aSet, MK_U32 aSetLen)
{
    MK_U32 lN = MK_BStr_Span(aStr, aLen, aSet, aSetLen);
    return MK_Cast(MK_Char*, (aLen != lN) ? aStr + lN : NULL);
}

/*
 * Break at first occurance of any character NOT from aSet (i.e. in the
 * complement of aSet) in aStr within the first aN characters. Returns a pointer
 * to the location of the matched character or NULL if no match was found.
 *
 * Similar to MK_BStr_SpanN().
 */
MK_INLINE MK_Char* MK_BStr_CBreakN(const MK_Char* aStr, MK_U32 aLen, const MK_Char* aSet, MK_U32 aSetLen, MK_U32 aN)
{
    return MK_BStr_CBreak(aStr, MK_Min_U32(aN, aLen), aSet, aSetLen);
}

/******************************************************************************/

/*
 * Converts the contents of aStr to uppercase. Returns the original pointer
 * value of aStr (to allow nesting).
 */
MK_INLINE MK_Char* MK_BStr_ToUpper(MK_Char* aStr, MK_U32 aLen)
{
    MK_U32 i = 0;
    MK_ASSERT(NULL != aStr || 0 == aLen);
    for (; i < aLen; ++i)
    {
        aStr[i] = MK_Char_ToUpper(aStr[i]);
    }
    return aStr;
}

/*
 * Converts the first aN characters of aStr to uppercase. Returns the original
 * pointer value of aStr (to allow nesting).
 */
MK_INLINE MK_Char* MK_BStr_ToUpperN(MK_Char* aStr, MK_U32 aLen, MK_U32 aN)
{
    return MK_BStr_ToUpper(aStr, MK_Min_U32(aN, aLen));
}

/******************************************************************************/

/*
 * Converts the contents of aStr to lowercase. Returns the original pointer
 * value of aStr (to allow nesting).
 */
MK_INLINE MK_Char* MK_BStr_ToLower(MK_Char* aStr, MK_U32 aLen)
{
    MK_U32 i = 0;
    MK_ASSERT(NULL != aStr || 0 == aLen);
    for (; i < aLen; ++i)
    {
        aStr[i] = MK_Char_ToLower(aStr[i]);
    }
    return aStr;
}

/*
 * Converts the first aN characters of aStr to lowercase. Returns the original
 * pointer value of aStr (to allow nesting).
 */
MK_INLINE MK_Char* MK_BStr_ToLowerN(MK_Char* aStr, MK_U32 aLen, MK_U32 aN)
{
    return MK_BStr_ToLower(aStr, MK_Min_U32(aN, aLen));
}

/******************************************************************************/

/*
 * Copy from aStr to aTo and return the number of characters copied (i.e. aLen).
 *
 * Warning: Subject to buffer overflows since there is no size checking of
 * destination buffer. Use MK_BStr_CopyN() when possible.
 */
MK_INLINE MK_U32 MK_BStr_Copy(const MK_Char* MK_RESTRICT aStr, MK_U32 aLen, MK_Char* MK_RESTRICT aTo)
{
    MK_ASSERT((NULL != aStr || 0 == aLen) && NULL != aTo);
    MK_Mem_Copy(aStr, aTo, aLen);
    return aLen;
}

/*
 * Copy from aStr to aTo a maximum of aN characters. Returns the number of
 * characters that would have been copied if aN was unlimited (i.e. aLen). This
 * allows easy truncation detection by comparing the result with aN (e.g. a
 * result > aN means a truncation has occured).
 */
MK_INLINE MK_U32 MK_BStr_CopyN(const MK_Char* MK_RESTRICT aStr, MK_U32 aLen, MK_Char* MK_RESTRICT aTo, MK_U32 aN)
{
    MK_ASSERT((NULL != aStr || 0 == aLen) && (NULL != aTo || 0 == aN));
    MK_Mem_Copy(aStr, aTo, MK_Min_U32(aN, aLen));
    return aLen;
}

/******************************************************************************/

/*
 * Allocate the neccesary buffer and copy aStr to it. Returns NULL on failure.
 */
MK_INLINE MK_Char* MK_BStr_Clone(const MK_Char* aStr, MK_U32 aLen)
{
    MK_ASSERT(NULL != aStr || 0 == aLen);
    if (0 != aLen)
    {
        return MK_ToBStr(MK_Mem_AllocAndCopy(aStr, aLen, aLen));
    }
    return NULL;
}

/*
 * Allocate a buffer for up to aN characters and copy what fits of aStr to it.
 * Returns NULL on failure.
 */
MK_INLINE MK_Char* MK_BStr_CloneN(const MK_Char* aStr, MK_U32 aLen, MK_U32 aN)
{
    MK_ASSERT(NULL != aStr || 0 == aLen || 0 == aN);
    if (0 != aLen && 0 != aN)
    {
        MK_U32 lN = (0 != aN) ? MK_Min_U32(aN, aLen) : aN;
        MK_Char* lStr = MK_ToBStr(MK_Mem_AllocAndCopy(aStr, lN, lN));
        return lStr;
    }
    return NULL;
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
