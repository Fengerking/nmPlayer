/*
 * NOTE: Porting candidate.
 */

#ifndef __MKBase_CStr_h__
#define __MKBase_CStr_h__

#include <MKBase/Platform.h>
#include <MKBase/Config.h>
#include <MKBase/Assert.h>
#include <MKBase/Type.h>
#include <MKBase/Util.h>
#include <MKBase/Memory.h>
#include <MKBase/Char.h>
#include <MKBase/BStr.h>

#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

/*
 * Convenience macro for casting to MK_Char* type.
 */
#define MK_ToCStr(aVal) MK_Cast(MK_Char*, aVal)

/******************************************************************************/

/*
 * Return a pointer to the end of the string (the '\0' character).
 *
 * The N version stops at aN characters if the end has not been found.
 */
MK_INLINE MK_Char* MK_CStr_End(const MK_Char* aStr);
MK_INLINE MK_Char* MK_CStr_EndN(const MK_Char* aStr, MK_U32 aN);

/*
 * Return the length of aStr.
 *
 * The N version stops counting at aN characters if the end has not been found.
 */
MK_INLINE MK_U32 MK_CStr_Len(const MK_Char* aStr);
MK_INLINE MK_U32 MK_CStr_LenN(const MK_Char* aStr, MK_U32 aN);

/*
 * Compare strings aStr1 and aStr2. Returns 0 if the strings are equal,
 * otherwise a negative value is returned if aStr1 < aStr2 and a positive value
 * if aStr1 > aStr2.
 *
 * The N versions compares a maximum of aN characters for each string.
 * The I versions does a case-insensitive comparison.
 */
MK_INLINE MK_S32 MK_CStr_Cmp(const MK_Char* aStr1, const MK_Char* aStr2);
MK_INLINE MK_S32 MK_CStr_CmpN(const MK_Char* aStr1, const MK_Char* aStr2, MK_U32 aN);
MK_INLINE MK_S32 MK_CStr_CmpI(const MK_Char* aStr1, const MK_Char* aStr2);
MK_INLINE MK_S32 MK_CStr_CmpNI(const MK_Char* aStr1, const MK_Char* aStr2, MK_U32 aN);

/*
 * Return a pointer to the first occurrence of aCh in aStr (including the
 * terminating '\0'). If no instance of aCh is found NULL is returned.
 *
 * The N version searches only the first aN characters.
 */
MK_INLINE MK_Char* MK_CStr_FindCh(const MK_Char* aStr, MK_Char aCh);
MK_INLINE MK_Char* MK_CStr_FindChN(const MK_Char* aStr, MK_Char aCh, MK_U32 aN);

/*
 * Return a pointer to the last occurrence of aCh in aStr (including the
 * terminating '\0'). If no instance of aCh is found NULL is returned.
 *
 * The N version searches only the first aN characters.
 */
MK_INLINE MK_Char* MK_CStr_RFindCh(const MK_Char* aStr, MK_Char aCh);
MK_INLINE MK_Char* MK_CStr_RFindChN(const MK_Char* aStr, MK_Char aCh, MK_U32 aN);

/*
 * Return a pointer to the first occurrence of aSub in aStr. If no instance of
 * aSub is found NULL is returned.
 *
 * The N versions compares a maximum of aN characters from aStr.
 * The I versions does a case-insensitive comparison.
 */
MK_INLINE MK_Char* MK_CStr_FindStr(const MK_Char* aStr, const MK_Char* aSub);
MK_INLINE MK_Char* MK_CStr_FindStrN(const MK_Char* aStr, const MK_Char* aSub, MK_U32 aN);
MK_INLINE MK_Char* MK_CStr_FindStrI(const MK_Char* aStr, const MK_Char* aSub);
MK_INLINE MK_Char* MK_CStr_FindStrNI(const MK_Char* aStr, const MK_Char* aSub, MK_U32 aN);

/*
 * Return the index of the first character in aStr which _IS_NOT_ part of aSet.
 * If all characters of aStr occur in aSet the length of aStr is returned (i.e.
 * the index to the terminating '\0').
 *
 * The N version spans a maximum of aN characters of aStr.
 *
 * Similar to MK_CStr_CBreak() and MK_CStr_CBreakN().
 */
MK_INLINE MK_U32 MK_CStr_Span(const MK_Char* aStr, const MK_Char* aSet);
MK_INLINE MK_U32 MK_CStr_SpanN(const MK_Char* aStr, const MK_Char* aSet, MK_U32 aN);

/*
 * Return the index of the first character in aStr which _IS_ part of aSet. If
 * no characters of aSet occur in aStr the length of aStr is returned (i.e. the
 * index to the terminating '\0').
 *
 * The N version spans a maximum of aN characters of aStr.
 *
 * Similar to MK_CStr_Break() and MK_CStr_BreakN().
 */
MK_INLINE MK_U32 MK_CStr_CSpan(const MK_Char* aStr, const MK_Char* aSet);
MK_INLINE MK_U32 MK_CStr_CSpanN(const MK_Char* aStr, const MK_Char* aSet, MK_U32 aN);

/*
 * Return a pointer to the first character in aStr which _IS_ part of aSet. If
 * no characters of aSet occur in aStr NULL is returned.
 *
 * The N version searches a maximum of aN characters of aStr.
 *
 * Similar to MK_CStr_CSpan() and MK_CStr_CSpanN().
 */
MK_INLINE MK_Char* MK_CStr_Break(const MK_Char* aStr, const MK_Char* aSet);
MK_INLINE MK_Char* MK_CStr_BreakN(const MK_Char* aStr, const MK_Char* aSet, MK_U32 aN);

/*
 * Return a pointer to the first character in aStr which _IS_NOT_ part of aSet.
 * If all characters of aStr occur in aSet NULL is returned.
 *
 * The N version searches a maximum of aN characters of aStr.
 *
 * Similar to MK_CStr_Span() and MK_CStr_SpanN().
 */
MK_INLINE MK_Char* MK_CStr_CBreak(const MK_Char* aStr, const MK_Char* aSet);
MK_INLINE MK_Char* MK_CStr_CBreakN(const MK_Char* aStr, const MK_Char* aSet, MK_U32 aN);

/*
 * Converts the characters of aStr to uppercase. Returns the original pointer
 * value of aStr (to ease nesting).
 *
 * The N version converts a maximum of aN characters of aStr.
 */
MK_INLINE MK_Char* MK_CStr_ToUpper(MK_Char* aStr);
MK_INLINE MK_Char* MK_CStr_ToUpperN(MK_Char* aStr, MK_U32 aN);

/*
 * Converts the characters of aStr to lowercase. Returns the original pointer
 * value of aStr (to ease nesting).
 *
 * The N version converts a maximum of aN characters of aStr.
 */
MK_INLINE MK_Char* MK_CStr_ToLower(MK_Char* aStr);
MK_INLINE MK_Char* MK_CStr_ToLowerN(MK_Char* aStr, MK_U32 aN);

/******************************************************************************/

/*
 * Copy the contents of aStr to aTo and return the required string length.
 *
 * The N version copies a maximum of aN - 1 characters but still returns the
 * string length a full copy would have produced. This allows easy truncation
 * detection (and buffer reallocation size) by comparing the result with aN
 * (e.g. a result >= aN means truncation occured). The resulting string is
 * always '\0' terminated unless aN == 0.
 *
 * The LN version behaves much like the N version but copies at most aL
 * characters of aStr (and thus returns a maximum value of aL).
 */
MK_INLINE MK_U32 MK_CStr_Copy(const MK_Char* MK_RESTRICT aStr, MK_Char* MK_RESTRICT aTo);
MK_INLINE MK_U32 MK_CStr_CopyN(const MK_Char* MK_RESTRICT aStr, MK_Char* MK_RESTRICT aTo, MK_U32 aN);
MK_INLINE MK_U32 MK_CStr_CopyLN(const MK_Char* MK_RESTRICT aStr, MK_U32 aL, MK_Char* MK_RESTRICT aTo, MK_U32 aN);

/*
 * Append the contents of aStr to aTo and return the required string length.
 *
 * The N version appends a maximum of aN - len(aTo) - 1 characters but still
 * returns the string length a full append would have produced. This allows easy
 * truncation detection (and buffer reallocation size) by comparing the result
 * with aN (e.g. a result >= aN means truncation occured). The resulting string
 * is always '\0' terminated unless aN == 0 or if aN < len(aTo) (which suggests
 * incorrect usage). In this case aN is interpreted as the length of aTo and
 * aN + len(aStr) is returned.
 *
 * The LN version behaves much like the N version but appends at most aL
 * characters of aStr (and thus returns a maximum value of aN + aL)
 */
MK_INLINE MK_U32 MK_CStr_Append(const MK_Char* MK_RESTRICT aStr, MK_Char* MK_RESTRICT aTo);
MK_INLINE MK_U32 MK_CStr_AppendN(const MK_Char* MK_RESTRICT aStr, MK_Char* MK_RESTRICT aTo, MK_U32 aN);
MK_INLINE MK_U32 MK_CStr_AppendLN(const MK_Char* MK_RESTRICT aStr, MK_U32 aL, MK_Char* MK_RESTRICT aTo, MK_U32 aN);

/******************************************************************************/

/*
 * Find the substrings separated by EOL-sequences (CRLF, CR or LF) and
 * return them as ptr/length-pairs in aLn and aLnN. Returns the number of
 * substrings found.
 *
 * The N version finds a maximum of aN substrings but still returns the
 * number of lines a full search would have produced. This allows easy
 * truncation detection (and buffer reallocation size) by comparing the result
 * with aN (e.g. a result > aN means truncation occured). The resulting
 * substrings are simply references into the original string and is _NOT_ '\0'
 * terminated (a simple loop with aLn[i][aLnN[i]] = '\0' can remedy this).
 * always '\0' terminated unless aN == 0.
 *
 * The LN version behaves much like the N version but searches at most aL
 * characters of aStr.
 */
MK_INLINE MK_U32 MK_CStr_Lines(const MK_Char* aStr, const MK_Char* aLn[], MK_U32 aLnN[]);
MK_INLINE MK_U32 MK_CStr_LinesN(const MK_Char* aStr, const MK_Char* aLn[], MK_U32 aLnN[], MK_U32 aN);
MK_INLINE MK_U32 MK_CStr_LinesLN(const MK_Char* aStr, MK_U32 aL, const MK_Char* aLn[], MK_U32 aLnN[], MK_U32 aN);

/******************************************************************************/

/*
 * Create a string using printf()-style formatting and return the length of the
 * produced string. The resulting string is always '\0' terminated.
 *
 * The N versions use at most aN bytes to store the resulting string (including
 * the terminating '\0'). They still return the length that would have been
 * produced had aN been unlimited (e.g. a result >= aN means truncation
 * occured). The result is always '\0' terminated unless aN is 0.
 *
 * The V versions take a va_list instead of ... argument.
 */
MK_U32 MK_CStr_Fmt(MK_Char* MK_RESTRICT aTo, const MK_Char* MK_RESTRICT aFmt, ...);
MK_U32 MK_CStr_FmtN(MK_Char* MK_RESTRICT aTo, MK_U32 aN, const MK_Char* MK_RESTRICT aFmt, ...);
MK_U32 MK_CStr_FmtV(MK_Char* MK_RESTRICT aTo, const MK_Char* MK_RESTRICT aFmt, va_list aArgs);
MK_U32 MK_CStr_FmtVN(MK_Char* MK_RESTRICT aTo, MK_U32 aN, const MK_Char* MK_RESTRICT aFmt, va_list aArgs);

/******************************************************************************/

/*
 * Allocate a copy of aStr and return a pointer to it. Returns NULL on failure.
 *
 * The resulting string should be freed with MK_Mem_Free().
 *
 * The N version copies at most aN characters from aStr.
 */
MK_INLINE MK_Char* MK_CStr_Clone(const MK_Char* aStr);
MK_INLINE MK_Char* MK_CStr_CloneN(const MK_Char* aStr, MK_U32 aN);

/******************************************************************************/

/*
 * Interpret aStr as an octal/decimal/hex value and return it. If aNext is
 * non-NULL a pointer to the first invalid character is stored there.
 *
 * Accepts leading spaces and sign characters (+-). Overflows in either
 * direction will return the corresponding MIN/MAX value for the type in
 * question.
 *
 * The S (signed) versions interprets any +- charcters preceeding the digits and
 * determines the resulting sign (two - make a +).
 *
 * The U (unsigned) versions ignores the signedness of the interpreted value
 * (e.g. the string is interpreted as unsigned regardless of +- characters).
 *
 * The Hex versions accepts a leading 0x (or 0X) as long as there are valid hex-
 * digits directly following it.
 */
MK_INLINE MK_U32 MK_CStr_OctalToU32(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_U64 MK_CStr_OctalToU64(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_S32 MK_CStr_OctalToS32(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_S64 MK_CStr_OctalToS64(const MK_Char* aStr, MK_Char** aNext);

MK_INLINE MK_U32 MK_CStr_DecimalToU32(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_U64 MK_CStr_DecimalToU64(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_S32 MK_CStr_DecimalToS32(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_S64 MK_CStr_DecimalToS64(const MK_Char* aStr, MK_Char** aNext);

MK_INLINE MK_U32 MK_CStr_HexToU32(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_U64 MK_CStr_HexToU64(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_S32 MK_CStr_HexToS32(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_S64 MK_CStr_HexToS64(const MK_Char* aStr, MK_Char** aNext);

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

MK_INLINE MK_Char* MK_CStr_End(const MK_Char* aStr)
{
    MK_ASSERT(NULL != aStr);
    while ('\0' != *aStr) ++aStr;
    return MK_ToCStr(aStr);
}

MK_INLINE MK_Char* MK_CStr_EndN(const MK_Char* aStr, MK_U32 aN)
{
    MK_U32 n = 0;
    MK_ASSERT(NULL != aStr);
    while (aN != n++ && '\0' != *aStr) ++aStr;
    return MK_ToCStr(aStr);
}

MK_INLINE MK_U32 MK_CStr_Len(const MK_Char* aStr)
{
    #if MK_HAVE_STRLEN
        MK_ASSERT(NULL != aStr);
        return MK_ToU32(strlen(aStr));
    #else
        MK_U32 n = 0;
        MK_ASSERT(NULL != aStr);
        while ('\0' != *aStr++) ++n;
        return n;
    #endif
}

MK_INLINE MK_U32 MK_CStr_LenN(const MK_Char* aStr, MK_U32 aN)
{
    #if MK_HAVE_STRNLEN
        MK_ASSERT(NULL != aStr);
        return MK_ToU32(strnlen(aStr, aN));
    #else
        MK_U32 n = 0;
        MK_ASSERT(NULL != aStr);
        while (aN != n && '\0' != *aStr++) ++n;
        return n;
    #endif
}

MK_INLINE MK_S32 MK_CStr_Cmp(const MK_Char* aStr1, const MK_Char* aStr2)
{
    #if MK_HAVE_STRCMP
        MK_ASSERT(NULL != aStr1 && NULL != aStr2);
        return MK_ToS32(strcmp(aStr1, aStr2));
    #else
        MK_Char i, j;
        MK_ASSERT(NULL != aStr1 && NULL != aStr2);
        while ((i = *aStr1++) == (j = *aStr2++) && '\0' != i);
        return MK_Char_Cmp(i, j);
    #endif
}

MK_INLINE MK_S32 MK_CStr_CmpN(const MK_Char* aStr1, const MK_Char* aStr2, MK_U32 aN)
{
    #if MK_HAVE_STRNCMP
        MK_ASSERT((NULL != aStr1 && NULL != aStr2) || 0 == aN);
        return MK_ToS32(strncmp(aStr1, aStr2, aN));
    #else
        MK_Char i, j = i;
        MK_ASSERT(i == j && ((NULL != aStr1 && NULL != aStr2) || 0 == aN));
        while (0 != aN && (i = *aStr1++) == (j = *aStr2++) && '\0' != i) --aN;
        /* NOTE: i may be uninitialized but it is ok because of j = i above */
        return MK_Char_Cmp(i, j);
    #endif
}

MK_INLINE MK_S32 MK_CStr_CmpI(const MK_Char* aStr1, const MK_Char* aStr2)
{
    MK_ASSERT(NULL != aStr1 && NULL != aStr2);
    #if MK_HAVE_STRCASECMP
        return MK_ToS32(strcasecmp(aStr1, aStr2));
    #elif MK_HAVE__STRICMP
        return MK_ToS32(_stricmp(aStr1, aStr2));
    #else
        MK_Char i, j;
        MK_ASSERT(NULL != aStr1 && NULL != aStr2);
        while ((i = MK_Char_ToUpper(*aStr1++)) == (j = MK_Char_ToUpper(*aStr2++)) && '\0' != i);
        return MK_Char_Cmp(i, j);
    #endif
}

MK_INLINE MK_S32 MK_CStr_CmpNI(const MK_Char* aStr1, const MK_Char* aStr2, MK_U32 aN)
{
    MK_ASSERT((NULL != aStr1 && NULL != aStr2) || 0 == aN);
    #if MK_HAVE_STRNCASECMP
        return MK_ToS32(strncasecmp(aStr1, aStr2, aN));
    #elif MK_HAVE__STRNICMP
        return (0 != aN) ? MK_ToS32(_strnicmp(aStr1, aStr2, aN)) : 0;
    #else
        MK_Char i, j = i;
        MK_ASSERT(i == j && ((NULL != aStr1 && NULL != aStr2) || 0 == aN));
        while (0 != aN && (i = MK_Char_ToUpper(*aStr1++)) == (j = MK_Char_ToUpper(*aStr2++)) && '\0' != i) --aN;
        /* NOTE: i may be uninitialized but it is ok because of j = i above */
        return MK_Char_Cmp(i, j);
    #endif
}

MK_INLINE MK_Char* MK_CStr_FindCh(const MK_Char* aStr, MK_Char aCh)
{
    #if MK_HAVE_STRCHR
        MK_ASSERT(NULL != aStr);
        return MK_ToCStr(strchr(aStr, aCh));
    #else
        MK_Char c;
        MK_ASSERT(NULL != aStr);
        for (;; ++aStr)
        {
            if (aCh == (c = *aStr)) return MK_ToCStr(aStr);
            if ('\0' == c) break;
        }
        return NULL;
    #endif
}

MK_INLINE MK_Char* MK_CStr_FindChN(const MK_Char* aStr, MK_Char aCh, MK_U32 aN)
{
    #if MK_HAVE_STRNCHR
        MK_ASSERT(NULL != aStr || 0 == aN);
        return MK_ToCStr(strnchr(aStr, aCh));
    #else
        MK_Char c;
        MK_ASSERT(NULL != aStr || 0 == aN);
        for (; 0 != aN; --aN, ++aStr)
        {
            if (aCh == (c = *aStr)) return MK_ToCStr(aStr);
            if ('\0' == c) break;
        }
        return NULL;
    #endif
}

MK_INLINE MK_Char* MK_CStr_RFindCh(const MK_Char* aStr, MK_Char aCh)
{
    #if MK_HAVE_STRNCHR
        MK_ASSERT(NULL != aStr);
        return MK_ToCStr(strrchr(aStr, aCh));
    #else
        MK_Char *p = NULL, c;
        MK_ASSERT(NULL != aStr);
        for (;; ++aStr)
        {
            if (aCh == (c = *aStr)) p = MK_ToCStr(aStr);
            if ('\0' == c) break;
        }
        return p;
    #endif
}

MK_INLINE MK_Char* MK_CStr_RFindChN(const MK_Char* aStr, MK_Char aCh, MK_U32 aN)
{
    #if MK_HAVE_STRNRCHR
        MK_ASSERT(NULL != aStr || 0 == aN);
        return MK_ToCStr(strnrchr(aStr, aCh));
    #else
        MK_Char* p = NULL,  c;
        MK_ASSERT(NULL != aStr || 0 == aN);
        for (; 0 != aN; --aN, ++aStr)
        {
            if (aCh == (c = *aStr)) p = MK_ToCStr(aStr);
            if ('\0' == c) break;
        }
        return p;
    #endif
}

MK_INLINE MK_Char* MK_CStr_FindStr(const MK_Char* aStr, const MK_Char* aSub)
{
    #if MK_HAVE_STRSTR
        MK_ASSERT(NULL != aStr && NULL != aSub);
        return MK_ToCStr(strstr(aStr, aSub));
    #else
        MK_Char i, j;
        MK_ASSERT(NULL != aStr && NULL != aSub);
        if ('\0' != (i = *aSub++))
        {
            MK_U32 n = MK_CStr_Len(aSub);
            do
            {
                do if ('\0' == (j = *aStr++)) return NULL; while (i != j);
            }
            while (0 != MK_CStr_CmpN(aStr, aSub, n));
            --aStr;
        }
        return MK_ToCStr(aStr);
    #endif
}

MK_INLINE MK_Char* MK_CStr_FindStrN(const MK_Char* aStr, const MK_Char* aSub, MK_U32 aN)
{
    #if MK_HAVE_STRNSTR
        MK_ASSERT((NULL != aStr || 0 == aN) && NULL != aSub);
        return MK_ToCStr(strnstr(aStr, aSub, aN));
    #else
        MK_Char i, j;
        MK_ASSERT((NULL != aStr || 0 == aN) && NULL != aSub);
        if ('\0' != (i = *aSub++))
        {
            MK_U32 n = MK_CStr_Len(aSub);
            do
            {
                do if (aN-- <= n || '\0' == (j = *aStr++)) return NULL; while (i != j);
            }
            while (0 != MK_CStr_CmpN(aStr, aSub, n));
            --aStr;
        }
        return MK_ToCStr(aStr);
    #endif
}

MK_INLINE MK_Char* MK_CStr_FindStrI(const MK_Char* aStr, const MK_Char* aSub)
{
    #if MK_HAVE_STRCASESTR
        MK_ASSERT(NULL != aStr && NULL != aSub);
        return MK_ToCStr(strcasestr(aStr, aSub));
    #else
        MK_Char i, j;
        MK_ASSERT(NULL != aStr && NULL != aSub);
        if ('\0' != (i = *aSub++))
        {
            MK_U32 n = MK_CStr_Len(aSub);
            i = MK_Char_ToUpper(i);
            do
            {
                do if ('\0' == (j = *aStr++)) return NULL; while (i != MK_Char_ToUpper(j));
            }
            while (0 != MK_CStr_CmpNI(aStr, aSub, n));
            --aStr;
        }
        return MK_ToCStr(aStr);
    #endif
}

MK_INLINE MK_Char* MK_CStr_FindStrNI(const MK_Char* aStr, const MK_Char* aSub, MK_U32 aN)
{
    #if MK_HAVE_STRNCASESTR
        MK_ASSERT((NULL != aStr || 0 == aN) && NULL != aSub);
        return MK_ToCStr(strncasestr(aStr, aSub, aN));
    #else
        MK_Char i, j;
        MK_ASSERT((NULL != aStr || 0 == aN) && NULL != aSub);
        if ('\0' != (i = *aSub++))
        {
            MK_U32 n = MK_CStr_Len(aSub);
            i = MK_Char_ToUpper(i);
            do
            {
                do if (aN-- <= n || '\0' == (j = *aStr++)) return NULL; while (i != MK_Char_ToUpper(j));
            }
            while (0 != MK_CStr_CmpNI(aStr, aSub, n));
            --aStr;
        }
        return MK_ToCStr(aStr);
    #endif
}

MK_INLINE MK_U32 MK_CStr_Span(const MK_Char* aStr, const MK_Char* aSet)
{
    #if MK_HAVE_STRSPN
        MK_ASSERT(NULL != aStr && NULL != aSet);
        return MK_ToU32(strspn(aStr, aSet));
    #else
        /* TODO: Make non-inlined table-driven version? */
        MK_U32 n = 0;
        MK_Char c;
        MK_ASSERT(NULL != aStr && NULL != aSet);
        while ('\0' != (c = aStr[n]) && NULL != MK_CStr_FindCh(aSet, c)) ++n;
        return n;
    #endif
}

MK_INLINE MK_U32 MK_CStr_SpanN(const MK_Char* aStr, const MK_Char* aSet, MK_U32 aN)
{
    /* TODO: Make non-inlined table-driven version? */
    MK_U32 n = 0;
    MK_Char c;
    MK_ASSERT((NULL != aStr && NULL != aSet) || 0 == aN);
    while (aN > n && '\0' != (c = aStr[n]) && NULL != MK_CStr_FindCh(aSet, c)) ++n;
    return n;
}

MK_INLINE MK_U32 MK_CStr_CSpan(const MK_Char* aStr, const MK_Char* aSet)
{
    #if MK_HAVE_STRCSPN
        MK_ASSERT(NULL != aStr && NULL != aSet);
        return MK_ToU32(strcspn(aStr, aSet));
    #elif MK_HAVE_STRPBRK
        MK_Char* lStr;
        MK_ASSERT(NULL != aStr && NULL != aSet);
        return MK_ToU32((NULL != (lStr = strpbrk(aStr, aSet))) ? lStr - aStr : 0);
    #else
        /* TODO: Make non-inlined table-driven version? */
        MK_U32 n = 0;
        MK_ASSERT(NULL != aStr && NULL != aSet);
        for (; NULL == MK_CStr_FindCh(aSet, aStr[n]); ++n);
        return n;
    #endif
}

MK_INLINE MK_U32 MK_CStr_CSpanN(const MK_Char* aStr, const MK_Char* aSet, MK_U32 aN)
{
    /* TODO: Make non-inlined table-driven version? */
    MK_U32 n = 0;
    MK_ASSERT((NULL != aStr && NULL != aSet) || 0 == aN);
    for (; aN > n && NULL == MK_CStr_FindCh(aSet, aStr[n]); ++n);
    return n;
}

MK_INLINE MK_Char* MK_CStr_Break(const MK_Char* aStr, const MK_Char* aSet)
{
    #if MK_HAVE_STRPBRK
        MK_ASSERT(NULL != aStr && NULL != aSet);
        return MK_ToCStr(strpbrk(aStr, aSet));
    #else
        return MK_ToCStr(('\0' != *(aStr += MK_CStr_CSpan(aStr, aSet))) ? aStr : NULL);
    #endif
}

MK_INLINE MK_Char* MK_CStr_BreakN(const MK_Char* aStr, const MK_Char* aSet, MK_U32 aN)
{
    MK_U32 n = MK_CStr_CSpanN(aStr, aSet, aN);
    return MK_ToCStr((aN != n && '\0' != aStr[n]) ? aStr + n : NULL);
}

MK_INLINE MK_Char* MK_CStr_CBreak(const MK_Char* aStr, const MK_Char* aSet)
{
    return MK_ToCStr(('\0' != *(aStr += MK_CStr_Span(aStr, aSet))) ? aStr : NULL);
}

MK_INLINE MK_Char* MK_CStr_CBreakN(const MK_Char* aStr, const MK_Char* aSet, MK_U32 aN)
{
    MK_U32 n = MK_CStr_SpanN(aStr, aSet, aN);
    return MK_ToCStr((aN != n && '\0' != aStr[n]) ? aStr + n : NULL);
}

MK_INLINE MK_Char* MK_CStr_ToUpper(MK_Char* aStr)
{
    MK_Char* lStr = aStr;
    MK_ASSERT(NULL != aStr);
    for (; '\0' != *lStr; ++lStr)
    {
        *lStr = MK_Char_ToUpper(*lStr);
    }
    return aStr;
}

MK_INLINE MK_Char* MK_CStr_ToUpperN(MK_Char* aStr, MK_U32 aN)
{
    MK_Char* lStr = aStr;
    MK_ASSERT(NULL != aStr || 0 == aN);
    for (; aN-- && '\0' != *lStr; ++lStr)
    {
        *lStr = MK_Char_ToUpper(*lStr);
    }
    return aStr;
}

MK_INLINE MK_Char* MK_CStr_ToLower(MK_Char* aStr)
{
    MK_Char* lStr = aStr;
    MK_ASSERT(NULL != aStr);
    for (; '\0' != *lStr; ++lStr)
    {
        *lStr = MK_Char_ToLower(*lStr);
    }
    return aStr;
}

MK_INLINE MK_Char* MK_CStr_ToLowerN(MK_Char* aStr, MK_U32 aN)
{
    MK_Char* lStr = aStr;
    MK_ASSERT(NULL != aStr || 0 == aN);
    for (; aN-- && '\0' != *lStr; ++lStr)
    {
        *lStr = MK_Char_ToLower(*lStr);
    }
    return aStr;
}

/******************************************************************************/

MK_INLINE MK_U32 MK_CStr_Copy(const MK_Char* MK_RESTRICT aStr, MK_Char* MK_RESTRICT aTo)
{
    MK_U32 n = 0;
    MK_ASSERT(NULL != aStr && NULL != aTo);
    while ('\0' != (aTo[n] = aStr[n])) ++n;
    return n;
}

MK_INLINE MK_U32 MK_CStr_CopyN(const MK_Char* MK_RESTRICT aStr, MK_Char* MK_RESTRICT aTo, MK_U32 aN)
{
    #if MK_HAVE_STRLCPY
        MK_ASSERT(NULL != aStr && (NULL != aTo || 0 == aN));
        return MK_ToU32(strlcpy(aTo, aStr, aN));
    #else
        const MK_Char* MK_RESTRICT s = aStr;
        MK_ASSERT(NULL != aStr && (NULL != aTo || 0 == aN));
        if (0 != aN)
        {
            while (0 != --aN && '\0' != *s) *aTo++ = *s++;
            *aTo = '\0';
        }
        while ('\0' != *s) ++s;
        return MK_ToU32(s - aStr);
    #endif
}

MK_INLINE MK_U32 MK_CStr_CopyLN(const MK_Char* MK_RESTRICT aStr, MK_U32 aL, MK_Char* MK_RESTRICT aTo, MK_U32 aN)
{
    const MK_Char* MK_RESTRICT s = aStr;
    MK_ASSERT(NULL != aStr && (NULL != aTo || 0 == aN));
    if (0 != aN)
    {
        for (; 0 != --aN && 0 != aL && '\0' != *s; *aTo++ = *s++, --aL);
        *aTo = '\0';
    }
    while (0 != aL-- && '\0' != *s) ++s;
    return MK_ToU32(s - aStr);
}

MK_INLINE MK_U32 MK_CStr_Append(const MK_Char* MK_RESTRICT aStr, MK_Char* MK_RESTRICT aTo)
{
    MK_U32 n = MK_CStr_Len(aTo);
    return (n + MK_CStr_Copy(aStr, aTo + n));
}

MK_INLINE MK_U32 MK_CStr_AppendN(const MK_Char* MK_RESTRICT aStr, MK_Char* MK_RESTRICT aTo, MK_U32 aN)
{
    #if MK_HAVE_STRLCAT
        MK_ASSERT(NULL != aStr && (NULL != aTo || 0 == aN));
        return MK_ToU32(strlcat(aTo, aStr, aN));
    #else
        MK_U32 n = MK_CStr_LenN(aTo, aN);
        return (n + MK_CStr_CopyN(aStr, aTo + n, aN - n));
    #endif
}

MK_INLINE MK_U32 MK_CStr_AppendLN(const MK_Char* MK_RESTRICT aStr, MK_U32 aL, MK_Char* MK_RESTRICT aTo, MK_U32 aN)
{
    MK_U32 n = MK_CStr_LenN(aTo, aN);
    return (n + MK_CStr_CopyLN(aStr, aL, aTo + n, aN - n));
}

/******************************************************************************/

MK_INLINE MK_U32 MK_CStr_Lines(const MK_Char* aStr, const MK_Char* aLn[], MK_U32 aLnN[])
{
    MK_U32 i = 0, n;
    MK_ASSERT(NULL != aStr && NULL != aLn && NULL != aLnN);
    do
    {
        n = MK_CStr_CSpan(aStr, "\r\n");
        aLn[i] = aStr;
        aLnN[i] = n;
        ++i;
        if ('\r' == aStr[n]) ++n;
        if ('\n' == aStr[n]) ++n;
    }
    while ('\0' != *(aStr += n));
    return i;
}

MK_INLINE MK_U32 MK_CStr_LinesN(const MK_Char* aStr, const MK_Char* aLn[], MK_U32 aLnN[], MK_U32 aN)
{
    MK_U32 i = 0, n;
    MK_ASSERT(NULL != aStr && ((NULL != aLn && NULL != aLnN) || 0 == aN));
    do
    {
        n = MK_CStr_CSpan(aStr, "\r\n");
        if (i < aN)
        {
            aLn[i] = aStr;
            aLnN[i] = n;
        }
        ++i;
        if ('\r' == aStr[n]) ++n;
        if ('\n' == aStr[n]) ++n;
    }
    while ('\0' != *(aStr += n));
    return i;
}


MK_INLINE MK_U32 MK_CStr_LinesLN(const MK_Char* aStr, MK_U32 aL, const MK_Char* aLn[], MK_U32 aLnN[], MK_U32 aN)
{
    MK_U32 i = 0, n;
    MK_ASSERT(NULL != aStr && ((NULL != aLn && NULL != aLnN) || 0 == aN));
    do
    {
        n = MK_CStr_CSpanN(aStr, "\r\n", aL);
        if (i < aN)
        {
            aLn[i] = aStr;
            aLnN[i] = n;
        }
        ++i;
        if (n < aL && '\r' == aStr[n]) ++n;
        if (n < aL && '\n' == aStr[n]) ++n;
    }
    while (0 != (aL -= n) && '\0' != *(aStr += n));
    return i;
}

/******************************************************************************/

MK_INLINE MK_Char* MK_CStr_Clone(const MK_Char* aStr)
{
    #if MK_HAVE_STRDUP
        MK_ASSERT(NULL != aStr);
        return MK_ToCStr(strdup(aStr));
    #elif MK_HAVE__STRDUP
        MK_ASSERT(NULL != aStr);
        return MK_ToCStr(_strdup(aStr));
    #else
        MK_U32 n = MK_CStr_Len(aStr) + 1;
        return MK_Mem_AllocAndCopy(aStr, n, n);
    #endif
}

MK_INLINE MK_Char* MK_CStr_CloneN(const MK_Char* aStr, MK_U32 aN)
{
    MK_Char* lStr;
    if (0 != aN) aN = MK_Min_U32(aN, MK_CStr_Len(aStr)); 
    lStr = MK_ToCStr(MK_Mem_AllocAndCopy(aStr, aN, aN + 1));
    if (NULL != lStr) lStr[aN] = '\0';
    return lStr;
}

/******************************************************************************/

/*
 * NOTE: Not using strtoX-family for now since it seems buggy on some platforms
 *       (and is partially C99 only).
 */

MK_U32 _MK_CStr_OctalToU32(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_U32 MK_CStr_OctalToU32(const MK_Char* aStr, MK_Char** aNext)
{
    MK_ASSERT(NULL != aStr);
    return _MK_CStr_OctalToU32(aStr, aNext);
}

MK_U64 _MK_CStr_OctalToU64(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_U64 MK_CStr_OctalToU64(const MK_Char* aStr, MK_Char** aNext)
{
    MK_ASSERT(NULL != aStr);
    return _MK_CStr_OctalToU64(aStr, aNext);
}

MK_S32 _MK_CStr_OctalToS32(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_S32 MK_CStr_OctalToS32(const MK_Char* aStr, MK_Char** aNext)
{
    MK_ASSERT(NULL != aStr);
    return _MK_CStr_OctalToS32(aStr, aNext);
}

MK_S64 _MK_CStr_OctalToS64(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_S64 MK_CStr_OctalToS64(const MK_Char* aStr, MK_Char** aNext)
{
    MK_ASSERT(NULL != aStr);
    return _MK_CStr_OctalToS64(aStr, aNext);
}

MK_U32 _MK_CStr_DecimalToU32(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_U32 MK_CStr_DecimalToU32(const MK_Char* aStr, MK_Char** aNext)
{
    MK_ASSERT(NULL != aStr);
    return _MK_CStr_DecimalToU32(aStr, aNext);
}

MK_U64 _MK_CStr_DecimalToU64(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_U64 MK_CStr_DecimalToU64(const MK_Char* aStr, MK_Char** aNext)
{
    MK_ASSERT(NULL != aStr);
    return _MK_CStr_DecimalToU64(aStr, aNext);
}

MK_S32 _MK_CStr_DecimalToS32(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_S32 MK_CStr_DecimalToS32(const MK_Char* aStr, MK_Char** aNext)
{
    MK_ASSERT(NULL != aStr);
    return _MK_CStr_DecimalToS32(aStr, aNext);
}

MK_S64 _MK_CStr_DecimalToS64(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_S64 MK_CStr_DecimalToS64(const MK_Char* aStr, MK_Char** aNext)
{
    MK_ASSERT(NULL != aStr);
    return _MK_CStr_DecimalToS64(aStr, aNext);
}

MK_U32 _MK_CStr_HexToU32(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_U32 MK_CStr_HexToU32(const MK_Char* aStr, MK_Char** aNext)
{
    MK_ASSERT(NULL != aStr);
    return _MK_CStr_HexToU32(aStr, aNext);
}

MK_U64 _MK_CStr_HexToU64(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_U64 MK_CStr_HexToU64(const MK_Char* aStr, MK_Char** aNext)
{
    MK_ASSERT(NULL != aStr);
    return _MK_CStr_HexToU64(aStr, aNext);
}

MK_S32 _MK_CStr_HexToS32(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_S32 MK_CStr_HexToS32(const MK_Char* aStr, MK_Char** aNext)
{
    MK_ASSERT(NULL != aStr);
    return _MK_CStr_HexToS32(aStr, aNext);
}

MK_S64 _MK_CStr_HexToS64(const MK_Char* aStr, MK_Char** aNext);
MK_INLINE MK_S64 MK_CStr_HexToS64(const MK_Char* aStr, MK_Char** aNext)
{
    MK_ASSERT(NULL != aStr);
    return _MK_CStr_HexToS64(aStr, aNext);
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
